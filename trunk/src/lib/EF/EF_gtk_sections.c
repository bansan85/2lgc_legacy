/*
 * 2lgc_code : calcul de résistance des matériaux selon les normes Eurocodes
 * Copyright (C) 2011
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"

#ifdef ENABLE_GTK
#include <libintl.h>
#include <locale.h>
#include <gtk/gtk.h>
#include <string.h>

#include "common_m3d.hpp"

#include "common_projet.h"
#include "common_erreurs.h"
#include "common_gtk.h"
#include "common_maths.h"
#include "common_selection.h"
#include "EF_section.h"
#include "EF_gtk_section_rectangulaire.h"
#include "EF_gtk_section_T.h"
#include "EF_gtk_section_carree.h"
#include "EF_gtk_section_circulaire.h"

G_MODULE_EXPORT void EF_gtk_sections_fermer(GtkButton *button __attribute__((unused)),
  Projet *projet)
/* Description : Ferme la fenêtre.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_sections.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Section");
    
    gtk_widget_destroy(projet->list_gtk.ef_sections.window);
    
    return;
}


G_MODULE_EXPORT void EF_gtk_sections_window_destroy(GtkWidget *object __attribute__((unused)),
  Projet *projet)
/* Description : met projet->list_gtk.ef_sections.builder à NULL quand la fenêtre se ferme,
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_sections.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Section");
    
    projet->list_gtk.ef_sections.builder = NULL;
    
    return;
}


G_MODULE_EXPORT gboolean EF_gtk_sections_window_key_press(
  GtkWidget *widget __attribute__((unused)), GdkEvent *event, Projet *projet)
/* Description : Ferme la fenêtre si la touche ECHAP est pressée.
 * Paramètres : GtkWidget *widget : composant à l'origine de l'évènement,
 *            : GdkEvent *event : Caractéristique de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : TRUE si la touche DELETE est pressée, FALSE sinon.
 *   Echec : projet == NULL,
 *           interface graphique non initialisée.
 *  
 */
{
    BUGMSG(projet, TRUE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_sections.builder, TRUE, gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Section");
    
    if (event->key.keyval == GDK_KEY_Escape)
    {
        gtk_widget_destroy(projet->list_gtk.ef_sections.window);
        return TRUE;
    }
    else
        return FALSE;
}


G_MODULE_EXPORT void EF_gtk_sections_select_changed(
  GtkTreeSelection *treeselection __attribute__((unused)), Projet *projet)
/* Description : En fonction de la sélection, active ou désactive le bouton supprimer.
 * Paramètres : GtkTreeSelection *treeselection : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 *   Echec : projet == NULL,
 *           interface graphique non initialisée.
 */
{
    GtkTreeModel    *model;
    GtkTreeIter     Iter;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_sections.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Section");
    
    // Si aucune section n'est sélectionnée, il n'est pas possible d'en supprimer ou d'en éditer une.
    if (!gtk_tree_selection_get_selected(GTK_TREE_SELECTION(gtk_builder_get_object(projet->list_gtk.ef_sections.builder, "EF_sections_treeview_select")), &model, &Iter))
    {
        gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_sections.builder, "EF_sections_boutton_supprimer_direct")), FALSE);
        gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_sections.builder, "EF_sections_boutton_supprimer_menu")), FALSE);
        gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_sections.builder, "EF_sections_boutton_modifier")), FALSE);
        gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_sections.builder, "EF_sections_boutton_supprimer_direct")), TRUE);
        gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_sections.builder, "EF_sections_boutton_supprimer_menu")), FALSE);
    }
    else
    {
        char        *nom;
        EF_Section  *section;
        
        gtk_tree_model_get(model, &Iter, 1, &nom, -1);
        
        BUG(section = EF_sections_cherche_nom(projet, nom, TRUE), );
        
        gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_sections.builder, "EF_sections_boutton_modifier")), TRUE);
        if (EF_sections_verifie_dependances(projet, section))
        {
            gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_sections.builder, "EF_sections_boutton_supprimer_direct")), FALSE);
            gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_sections.builder, "EF_sections_boutton_supprimer_menu")), TRUE);
            gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_sections.builder, "EF_sections_boutton_supprimer_direct")), FALSE);
            gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_sections.builder, "EF_sections_boutton_supprimer_menu")), TRUE);
        }
        else
        {
            gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_sections.builder, "EF_sections_boutton_supprimer_direct")), TRUE);
            gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_sections.builder, "EF_sections_boutton_supprimer_menu")), FALSE);
            gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_sections.builder, "EF_sections_boutton_supprimer_direct")), TRUE);
            gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_sections.builder, "EF_sections_boutton_supprimer_menu")), FALSE);
        }
        
        free(nom);
    }
    
    return;
}


G_MODULE_EXPORT void EF_gtk_sections_edit_nom(GtkCellRendererText *cell __attribute__((unused)),
  gchar *path_string, gchar *new_text, Projet *projet)
/* Description : Modification du nom d'une section.
 * Paramètres : GtkCellRendererText *cell : cellule en cours,
 *            : gchar *path_string : path de la ligne en cours,
 *            : gchar *new_text : le nouveau nom,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
*/
{
    Gtk_EF_Sections *ef_gtk;
    GtkTreeModel    *model;
    GtkTreeIter     iter;
    GtkTreePath     *path;
    char            *nom;
    EF_Section      *section;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_sections.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Section");
    
    ef_gtk = &projet->list_gtk.ef_sections;
    model = GTK_TREE_MODEL(ef_gtk->sections);
    path = gtk_tree_path_new_from_string(path_string);
    gtk_tree_model_get_iter(model, &iter, path);
    gtk_tree_path_free(path);
    gtk_tree_model_get(model, &iter, 1, &nom, -1);
    if (strcmp(nom, new_text) == 0)
    {
        free(nom);
        return;
    }
    BUG(section = EF_sections_cherche_nom(projet, nom, TRUE), );
    free(nom);
    if (strcmp(section->nom, new_text) == 0)
        return;

    BUG(EF_sections_renomme(section, new_text, projet), );
    
    return;
}


G_MODULE_EXPORT void EF_gtk_sections_supprimer_direct(GtkButton *button __attribute__((unused)),
  Projet *projet)
/* Description : Supprime la section sélectionnée dans le treeview.
 * Paramètres : GtkWidget *widget : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    GtkTreeIter     iter;
    GtkTreeModel    *model;
    char            *nom;
    EF_Section      *section;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_sections.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Section");
    
    if (!gtk_tree_selection_get_selected(GTK_TREE_SELECTION(gtk_builder_get_object(projet->list_gtk.ef_sections.builder, "EF_sections_treeview_select")), &model, &iter))
        return;
    
    gtk_tree_model_get(model, &iter, 1, &nom, -1);
    
    BUG(section = EF_sections_cherche_nom(projet, nom, TRUE), );
    BUG(EF_sections_supprime(section, TRUE, projet), );
    
    BUG(m3d_rafraichit(projet), );
    
    free(nom);
    
    return;
}


G_MODULE_EXPORT void EF_gtk_sections_supprimer_menu_barres(
  GtkButton *button __attribute__((unused)), Projet *projet)
/* Description : Supprime la section sélectionnée dans le treeview, y compris les barres
 *               l'utilisant.
 * Paramètres : GtkWidget *widget : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    GtkTreeIter     iter;
    GtkTreeModel    *model;
    char            *nom;
    EF_Section      *section;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_sections.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Section");
    
    if (!gtk_tree_selection_get_selected(GTK_TREE_SELECTION(gtk_builder_get_object(projet->list_gtk.ef_sections.builder, "EF_sections_treeview_select")), &model, &iter))
        return;
    
    gtk_tree_model_get(model, &iter, 1, &nom, -1);
    
    BUG(section = EF_sections_cherche_nom(projet, nom, TRUE), );
    BUG(EF_sections_supprime(section, FALSE, projet), );
    
    BUG(m3d_rafraichit(projet), );
    
    free(nom);
    
    return;
}


G_MODULE_EXPORT GdkPixbuf *EF_gtk_sections_dessin(EF_Section *section, int width, int height)
/* Description : Renvoie un dessin représentant la section.
 * Paramètres : EF_Section *section : la section à dessiner,
 *              int width : la largeur du dessin,
 *              int height : la hauteur du dessin.
 * Valeur renvoyée : Aucune.
 *   Echec : section == NULL,
 *           width == NULL,
 *           height == 0.
 */
{
    int             rowstride, n_channels;
    int             x, y;
    guchar          *pixels, *p;
    GdkPixbuf       *pixbuf;
    double          a;
    cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
    cairo_t         *cr = cairo_create(surface);
    double          convert;
    cairo_path_t    *save_path;
    
    BUGMSG(section, NULL, gettext("Paramètre %s incorrect.\n"), "section");
    BUGMSG(width, NULL, gettext("La largeur du dessin ne peut être nulle.\n"));
    BUGMSG(height, NULL, gettext("La hauteur du dessin ne peut être nulle.\n"));
    BUGMSG(cairo_surface_status(surface) == CAIRO_STATUS_SUCCESS, NULL, gettext("Erreur d'allocation mémoire.\n"));
    
    a = (double)width/height;
    pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, TRUE, 8, width, height);
    pixels = gdk_pixbuf_get_pixels(pixbuf);
    rowstride = gdk_pixbuf_get_rowstride(pixbuf);
    n_channels = gdk_pixbuf_get_n_channels(pixbuf);
    
    cairo_set_antialias(cr, CAIRO_ANTIALIAS_NONE);
    cairo_set_source_rgba(cr, 1., 1., 1., 0.);
    cairo_rectangle(cr, 0, 0, width, height);
    cairo_fill(cr);
        
    // On replie tout avec un fond blanc
    for (y=0;y<height;y++)
        for (x=0;x<width;x++)
        {
            p = pixels + y * rowstride + x * n_channels;
            p[0] = 255;
            p[1] = 255;
            p[2] = 255;
            if (n_channels == 4)
                p[3] = 0;
        }
    
    switch (section->type)
    {
        case SECTION_RECTANGULAIRE :
        case SECTION_T :
        {
            Section_T   *data = (Section_T *)section->data;
            
            double      lt = data->largeur_table;
            double      lr = data->largeur_retombee;
            double      ht = data->hauteur_table;
            double      hr = data->hauteur_retombee;
            
            double      aa = MAX(lt, lr)/(ht + hr);
            
            cairo_set_source_rgba(cr, 0.8, 0.8, 0.8, 1.);
            cairo_new_path(cr);
            
            // Le schéma prend toute la largeur
            if (aa > a)
                convert = (width-1)/MAX(lt, lr);
            else
                convert = (height-1)/(ht+hr);
            cairo_move_to(cr, (width/2. - lt/2.*convert), (height/2. - (ht + hr)/2.*convert));
            cairo_rel_line_to(cr, 0., ht*convert);
            cairo_rel_line_to(cr, (lt-lr)/2.*convert, 0.);
            cairo_rel_line_to(cr, 0., hr*convert);
            cairo_rel_line_to(cr, lr*convert, 0.);
            cairo_rel_line_to(cr, 0., -hr*convert);
            cairo_rel_line_to(cr, (lt-lr)/2.*convert, 0.);
            cairo_rel_line_to(cr, 0., -ht*convert);
            cairo_close_path(cr);
            save_path = cairo_copy_path(cr);
            cairo_fill(cr);
            cairo_set_source_rgba(cr, 0., 0., 0., 1.);
            cairo_set_line_width(cr, 1.);
            cairo_new_path(cr);
            cairo_append_path(cr, save_path);
            cairo_stroke(cr);
            
            cairo_path_destroy(save_path);
            cairo_destroy(cr);
            pixbuf = gdk_pixbuf_get_from_surface(surface, 0, 0, width, height);
            cairo_surface_destroy(surface);
            
            break;
        }
        case SECTION_CARREE :
        {
            cairo_set_source_rgba(cr, 0.8, 0.8, 0.8, 1.);
            
            if (a < 1)
            {
                double y_h = height/2. - width/2.;
                double y_b = height/2. + width/2.;
                
                cairo_rectangle(cr, 1, y_h+1, width-1, y_b-y_h-1);
                cairo_fill(cr);
                cairo_set_source_rgba(cr, 0., 0., 0., 1.);
                cairo_set_line_width(cr, 1.);
                cairo_rectangle(cr, 1, y_h+1, width-1, y_b-y_h-1);
            }
            else
            {
                double x_g = width/2. - height/2.;
                double x_d = width/2. + height/2.;
                
                cairo_rectangle(cr, x_g+1., 1., x_d-x_g-1., height-1.);
                cairo_fill(cr);
                cairo_set_source_rgba(cr, 0., 0., 0., 1.);
                cairo_set_line_width(cr, 1.);
                cairo_rectangle(cr, x_g+1., 1., x_d-x_g-1., height-1.);
            }
            cairo_stroke(cr);
            
            cairo_destroy(cr);
            pixbuf = gdk_pixbuf_get_from_surface(surface, 0, 0, width, height);
            cairo_surface_destroy(surface);
            
            break;
        }
        case SECTION_CIRCULAIRE :
        {
            cairo_set_source_rgba(cr, 0.8, 0.8, 0.8, 1.);
            
            if (a < 1)
            {
                double y_h = height/2. - width/2.;
                double y_b = height/2. + width/2.;
                
                cairo_arc(cr, width/2., y_h+(y_b-y_h)/2., width/2.-1, 0, 2. * M_PI);
                cairo_fill(cr);
                cairo_set_source_rgba(cr, 0., 0., 0., 1.);
                cairo_set_line_width(cr, 1.);
                cairo_arc(cr, width/2., y_h+(y_b-y_h)/2., width/2.-1, 0, 2. * M_PI);
            }
            else
            {
                double x_g = width/2. - height/2.;
                double x_d = width/2. + height/2.;
                
                cairo_arc(cr, x_g+(x_d-x_g)/2., height/2., height/2.-1, 0, 2. * M_PI);
                cairo_fill(cr);
                cairo_set_source_rgba(cr, 0., 0., 0., 1.);
                cairo_set_line_width(cr, 1.);
                cairo_arc(cr, x_g+(x_d-x_g)/2., height/2., height/2.-1, 0, 2. * M_PI);
            }
            cairo_stroke(cr);
            
            cairo_destroy(cr);
            pixbuf = gdk_pixbuf_get_from_surface(surface, 0, 0, width, height);
            cairo_surface_destroy(surface);
            
            break;
        }
        default :
        {
            BUGMSG(0, NULL, gettext("Type de section %d inconnu."), section->type);
            break;
        }
    }
    
    return pixbuf;
}


G_MODULE_EXPORT void EF_gtk_sections_ajout_rectangulaire(
  GtkMenuItem *menuitem __attribute__((unused)), Projet *projet)
/* Description : Lance la fenêtre permettant d'ajouter une section rectangulaire.
 * Paramètres : GtkMenuItem *menuitem : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 *   Echec : projet == NULL,
 *           interface graphique non initialisée.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_sections.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Section");
    
    BUG(EF_gtk_section_rectangulaire(projet, NULL), );
}


G_MODULE_EXPORT void EF_gtk_sections_ajout_T(
  GtkMenuItem *menuitem __attribute__((unused)), Projet *projet)
/* Description : Lance la fenêtre permettant d'ajouter une section en T.
 * Paramètres : GtkMenuItem *menuitem : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 *   Echec : projet == NULL,
 *           interface graphique non initialisée.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_sections.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Section");
    
    BUG(EF_gtk_section_T(projet, NULL), );
}


G_MODULE_EXPORT void EF_gtk_sections_ajout_carree(
  GtkMenuItem *menuitem __attribute__((unused)), Projet *projet)
/* Description : Lance la fenêtre permettant d'ajouter une section carrée.
 * Paramètres : GtkMenuItem *menuitem : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 *   Echec : projet == NULL,
 *           interface graphique non initialisée.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_sections.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Section");
    
    BUG(EF_gtk_section_carree(projet, NULL), );
}


G_MODULE_EXPORT void EF_gtk_sections_ajout_circulaire(
  GtkMenuItem *menuitem __attribute__((unused)), Projet *projet)
/* Description : Lance la fenêtre permettant d'ajouter une section circulaire.
 * Paramètres : GtkMenuItem *menuitem : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 *   Echec : projet == NULL,
 *           interface graphique non initialisée.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_sections.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Section");
    
    BUG(EF_gtk_section_circulaire(projet, NULL), );
}


G_MODULE_EXPORT void EF_gtk_sections_edit_clicked(GtkWidget *widget  __attribute__((unused)),
  Projet *projet)
/* Description : Edite les sections sélectionnées.
 * Paramètres : GtkToolButton *toolbutton : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 *   Echec : projet == NULL,
 *           interface graphique non initialisée.
 */
{
    GtkTreeIter     iter;
    GtkTreeModel    *model;
    char            *nom;
    GList           *list, *list_parcours;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_sections.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Section");
    
    // On récupère la liste des charges à éditer.
    list = gtk_tree_selection_get_selected_rows(GTK_TREE_SELECTION(gtk_builder_get_object(projet->list_gtk.ef_sections.builder, "EF_sections_treeview_select")), &model);
    list_parcours = g_list_first(list);
    for(;list_parcours != NULL; list_parcours = g_list_next(list_parcours))
    {
        if (gtk_tree_model_get_iter(model, &iter, (GtkTreePath*)list_parcours->data))
        {
    // Et on les édite les unes après les autres.
            EF_Section  *section;
            
            gtk_tree_model_get(model, &iter, 1, &nom, -1);
            BUG(section = EF_sections_cherche_nom(projet, nom, TRUE), );
            free(nom);
            
            switch (section->type)
            {
                case SECTION_RECTANGULAIRE :
                {
                    BUG(EF_gtk_section_rectangulaire(projet, section), );
                    break;
                }
                case SECTION_T :
                {
                    BUG(EF_gtk_section_T(projet, section), );
                    break;
                }
                case SECTION_CARREE :
                {
                    BUG(EF_gtk_section_carree(projet, section), );
                    break;
                }
                case SECTION_CIRCULAIRE :
                {
                    BUG(EF_gtk_section_circulaire(projet, section), );
                    break;
                }
                default :
                {
                    BUGMSG(0, , gettext("Type de section %d inconnu."), section->type);
                    break;
                }
            }
        }
    }
    g_list_foreach(list, (GFunc)gtk_tree_path_free, NULL);
    g_list_free(list);
    
    return;
}


G_MODULE_EXPORT gboolean EF_gtk_sections_double_clicked(GtkWidget *widget, GdkEvent *event,
  Projet *projet)
/* Description : Lance la fenêtre d'édition de la section sélectionnée en cas de double-clique
 *               dans le tree-view.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : GdkEvent *event : Information sur l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : TRUE s'il y a édition via un double-clique, FALSE sinon.
 *   Echec : FALSE :
 *             projet == NULL,
 *             interface graphique non initialisée.
 */
{
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_sections.builder, FALSE, gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Section");
    
    if ((event->type == GDK_2BUTTON_PRESS) && (gtk_widget_get_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_sections.builder, "EF_sections_boutton_modifier")))))
    {
        EF_gtk_sections_edit_clicked(widget, projet);
        return TRUE;
    }
    
    return FALSE;
}


G_MODULE_EXPORT void EF_gtk_sections(Projet *projet)
/* Description : Création de la fenêtre permettant d'afficher les sections sous forme d'un
 *               tableau.
 * Paramètres : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 *   Echec : projet == NULL,
 *           interface graphique impossible à générer.
 */
{
    Gtk_EF_Sections *ef_gtk;
    GList           *list_parcours;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_sections.builder == NULL, , gettext("La fenêtre graphique %s est déjà initialisée.\n"), "Section");
    
    ef_gtk = &projet->list_gtk.ef_sections;
    
    ef_gtk->builder = gtk_builder_new();
    BUGMSG(gtk_builder_add_from_file(ef_gtk->builder, DATADIR"/ui/EF_sections.ui", NULL) != 0, , gettext("Builder Failed\n"));
    gtk_builder_connect_signals(ef_gtk->builder, projet);
    
    ef_gtk->window = GTK_WIDGET(gtk_builder_get_object(ef_gtk->builder, "EF_sections_window"));
    ef_gtk->sections = GTK_TREE_STORE(gtk_builder_get_object(ef_gtk->builder, "EF_sections_treestore"));
    
    list_parcours = projet->beton.sections;
    while (list_parcours != NULL)
    {
        EF_Section  *section = (EF_Section *)list_parcours->data;
        
        gtk_tree_store_append(ef_gtk->sections, &section->Iter_fenetre, NULL);
        BUG(EF_sections_update_ligne_treeview(projet, section), );
        
        list_parcours = g_list_next(list_parcours);
    }
    
    gtk_window_set_transient_for(GTK_WINDOW(ef_gtk->window), GTK_WINDOW(projet->list_gtk.comp.window));
}

#endif
