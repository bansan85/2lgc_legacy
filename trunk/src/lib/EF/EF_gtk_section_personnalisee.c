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
#include <math.h>
#include <string.h>

#include "common_projet.h"
#include "common_erreurs.h"
#include "common_gtk.h"
#include "common_math.h"
#include "common_text.h"
#include "common_selection.h"
#include "EF_sections.h"
#include "EF_gtk_sections.h"

gboolean EF_gtk_section_personnalisee_window_key_press(GtkWidget *widget, GdkEvent *event,
  Projet *projet)
/* Description : Gestion des touches de l'ensemble des composants de la fenêtre.
 * Paramètres : GtkWidget *widget : composant à l'origine de l'évènement,
 *            : GdkEvent *event : description de la touche pressée,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : TRUE si la touche ECHAP est pressée, FALSE sinon.
 *   Echec : FALSE :
 *             projet == NULL,
 *             interface graphique non initialisée.
 */
{
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_sections_personnalisee.builder, FALSE, gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Ajout Section Personnalisee");
    
    if (event->key.keyval == GDK_KEY_Escape)
    {
        gtk_widget_destroy(projet->list_gtk.ef_sections_personnalisee.window);
        return TRUE;
    }
    else
        return FALSE;
}


void EF_gtk_section_personnalisee_window_destroy(GtkWidget *object, Projet *projet)
/* Description : Met projet->list_gtk.ef_sections_personnalisee.builder à NULL quand la
 *               fenêtre se ferme.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    Gtk_EF_Sections_Personnalisee  *ef_gtk;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    ef_gtk = &projet->list_gtk.ef_sections_personnalisee;
    BUGMSG(ef_gtk->builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Ajout Section Personnalisee");
    
    if (ef_gtk->keep == FALSE)
    {
        GtkTreeIter iter;
        
        if (gtk_tree_model_get_iter_first(ef_gtk->model, &iter))
        {
            do
            {
                GtkTreeIter iter2;
                
                if (gtk_tree_model_iter_children(ef_gtk->model, &iter2, &iter))
                {
                    do
                    {
                        EF_Point    *point;
                        
                        gtk_tree_model_get(ef_gtk->model, &iter, 0, &point, -1);
                        free(point);
                    } while (gtk_tree_model_iter_next(ef_gtk->model, &iter2));
                }
            } while (gtk_tree_model_iter_next(ef_gtk->model, &iter));
        }
    }
    
    g_object_unref(G_OBJECT(projet->list_gtk.ef_sections_personnalisee.builder));
    projet->list_gtk.ef_sections_personnalisee.builder = NULL;
    
    return;
}


gboolean EF_gtk_section_personnalisee_recupere_donnees(Projet *projet, double *j, double *iy,
  double *iz, double *vy, double *vyp, double *vz, double *vzp, double *s, GList **forme,
  gchar **nom, gchar **description)
/* Description : Récupère toutes les données de la fenêtre permettant d'ajouter ou d'éditer une
 *               section de type personnalisée.
 * Paramètres : Projet *projet : la variable projet,
 *            : tous les autres paramètres,
 *            : gchar **nom : le nom de la section,
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL,
 *             j == NULL,
 *             iy == NULL,
 *             iz == NULL,
 *             vy == NULL,
 *             vyp == NULL,
 *             vz == NULL,
 *             vzp == NULL,
 *             s == NULL,
 *             forme == NULL,
 *             nom == NULL,
 *             en cas d'erreur d'allocation mémoire.
 */
{
    GtkTextIter     start, end;
    GtkTextBuffer   *textbuffer;
    gboolean        ok = TRUE;
    GtkTreeIter     iter;
    Gtk_EF_Sections_Personnalisee  *ef_gtk;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(j, FALSE, gettext("Paramètre %s incorrect.\n"), "j");
    BUGMSG(iy, FALSE, gettext("Paramètre %s incorrect.\n"), "iy");
    BUGMSG(iz, FALSE, gettext("Paramètre %s incorrect.\n"), "iz");
    BUGMSG(vy, FALSE, gettext("Paramètre %s incorrect.\n"), "vy");
    BUGMSG(vyp, FALSE, gettext("Paramètre %s incorrect.\n"), "vyp");
    BUGMSG(vz, FALSE, gettext("Paramètre %s incorrect.\n"), "vz");
    BUGMSG(vzp, FALSE, gettext("Paramètre %s incorrect.\n"), "vzp");
    BUGMSG(s, FALSE, gettext("Paramètre %s incorrect.\n"), "s");
    BUGMSG(forme, FALSE, gettext("Paramètre %s incorrect.\n"), "forme");
    BUGMSG(nom, FALSE, gettext("Paramètre %s incorrect.\n"), "nom");
    BUGMSG(projet->list_gtk.ef_sections_personnalisee.builder, FALSE, gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Ajout Section Personnalisee");
    
    ef_gtk = &projet->list_gtk.ef_sections_personnalisee;
    
    *j = common_gtk_text_buffer_double(GTK_TEXT_BUFFER(gtk_builder_get_object(projet->list_gtk.ef_sections_personnalisee.builder, "EF_section_personnalisee_buffer_j")), 0, FALSE, INFINITY, FALSE);
    if (isnan(*j))
        ok = FALSE;
    
    *iy = common_gtk_text_buffer_double(GTK_TEXT_BUFFER(gtk_builder_get_object(projet->list_gtk.ef_sections_personnalisee.builder, "EF_section_personnalisee_buffer_iy")), 0, FALSE, INFINITY, FALSE);
    if (isnan(*iy))
        ok = FALSE;
    
    *iz = common_gtk_text_buffer_double(GTK_TEXT_BUFFER(gtk_builder_get_object(projet->list_gtk.ef_sections_personnalisee.builder, "EF_section_personnalisee_buffer_iz")), 0, FALSE, INFINITY, FALSE);
    if (isnan(*iz))
        ok = FALSE;
    
    *vy = common_gtk_text_buffer_double(GTK_TEXT_BUFFER(gtk_builder_get_object(projet->list_gtk.ef_sections_personnalisee.builder, "EF_section_personnalisee_buffer_vy")), 0, FALSE, INFINITY, FALSE);
    if (isnan(*vy))
        ok = FALSE;
    
    *vyp = common_gtk_text_buffer_double(GTK_TEXT_BUFFER(gtk_builder_get_object(projet->list_gtk.ef_sections_personnalisee.builder, "EF_section_personnalisee_buffer_vyp")), 0, FALSE, INFINITY, FALSE);
    if (isnan(*vyp))
        ok = FALSE;
    
    *vz = common_gtk_text_buffer_double(GTK_TEXT_BUFFER(gtk_builder_get_object(projet->list_gtk.ef_sections_personnalisee.builder, "EF_section_personnalisee_buffer_vz")), 0, FALSE, INFINITY, FALSE);
    if (isnan(*vz))
        ok = FALSE;
    
    *vzp = common_gtk_text_buffer_double(GTK_TEXT_BUFFER(gtk_builder_get_object(projet->list_gtk.ef_sections_personnalisee.builder, "EF_section_personnalisee_buffer_vzp")), 0, FALSE, INFINITY, FALSE);
    if (isnan(*vzp))
        ok = FALSE;
    
    *s = common_gtk_text_buffer_double(GTK_TEXT_BUFFER(gtk_builder_get_object(projet->list_gtk.ef_sections_personnalisee.builder, "EF_section_personnalisee_buffer_s")), 0, FALSE, INFINITY, FALSE);
    if (isnan(*s))
        ok = FALSE;
    
    *forme = NULL;
    if (!gtk_tree_model_get_iter_first(ef_gtk->model, &iter))
        ok = FALSE;
    else
    {
        do
        {
            GtkTreeIter iter2;
            
            if (gtk_tree_model_iter_children(ef_gtk->model, &iter2, &iter))
            {
                GList       *liste_noeuds = NULL;
                EF_Point    *point;
                
                gtk_tree_model_get(ef_gtk->model, &iter2, 0, &point, -1);
                // Comme ça, on ajoute pas le dernier point qui est le même que le premier.
                while (gtk_tree_model_iter_next(ef_gtk->model, &iter2))
                {
                    liste_noeuds = g_list_append(liste_noeuds, point);
                    
                    gtk_tree_model_get(ef_gtk->model, &iter2, 0, &point, -1);
                }
                
                if (liste_noeuds != NULL)
                    *forme = g_list_append(*forme, liste_noeuds);
            }
            
        } while (gtk_tree_model_iter_next(ef_gtk->model, &iter));
    }
    
    // Si tous les paramètres sont corrects
    textbuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(gtk_builder_get_object(projet->list_gtk.ef_sections_personnalisee.builder, "EF_section_personnalisee_textview_description")));
    gtk_text_buffer_get_iter_at_offset(textbuffer, &start, 0);
    gtk_text_buffer_get_iter_at_offset(textbuffer, &end, -1);
    *description = gtk_text_buffer_get_text(textbuffer, &start, &end, FALSE);
    
    textbuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(gtk_builder_get_object(projet->list_gtk.ef_sections_personnalisee.builder, "EF_section_personnalisee_textview_nom")));
    gtk_text_buffer_get_iter_at_offset(textbuffer, &start, 0);
    gtk_text_buffer_get_iter_at_offset(textbuffer, &end, -1);
    *nom = gtk_text_buffer_get_text(textbuffer, &start, &end, FALSE);
    
    gtk_text_buffer_remove_all_tags(textbuffer, &start, &end);
    
    if (projet->list_gtk.ef_sections_personnalisee.section == NULL)
    {
        if ((strcmp(*nom, "") == 0) || (EF_sections_cherche_nom(projet, *nom, FALSE)))
        {
            gtk_text_buffer_apply_tag_by_name(textbuffer, "mauvais", &start, &end);
            ok = FALSE;
        }
        else
            gtk_text_buffer_apply_tag_by_name(textbuffer, "OK", &start, &end);
    }
    else if ((strcmp(*nom, "") == 0) ||
      ((strcmp(projet->list_gtk.ef_sections_personnalisee.section->nom, *nom) != 0) && (EF_sections_cherche_nom(projet, *nom, FALSE))))
    {
        gtk_text_buffer_apply_tag_by_name(textbuffer, "mauvais", &start, &end);
        ok = FALSE;
    }
    else
        gtk_text_buffer_apply_tag_by_name(textbuffer, "OK", &start, &end);
    
    if (ok == FALSE)
    {
        free(*nom);
        *nom = NULL;
    }
    
    return ok;
}


void EF_gtk_section_personnalisee_check(GtkWidget *object, Projet *projet)
/* Description : Vérifie si l'ensemble des éléments est correct pour activer le bouton add/edit.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    double      j, iy, iz, vy, vyp, vz, vzp, s;
    GList       *forme = NULL;
    char        *nom = NULL, *description = NULL;
    EF_Section  section;
    Section_Personnalisee   data;
    GdkPixbuf   *pixbuf;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_sections_personnalisee.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Ajout Section Personnalisee");
    
    if (!EF_gtk_section_personnalisee_recupere_donnees(projet, &j, &iy, &iz, &vy, &vyp, &vz, &vzp, &s, &forme, &nom, &description))
        gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_sections_personnalisee.builder, "EF_section_personnalisee_button_add_edit")), FALSE);
    else
        gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_sections_personnalisee.builder, "EF_section_personnalisee_button_add_edit")), TRUE);
    
    section.type = SECTION_PERSONNALISEE;
    section.data = &data;
    data.forme = forme;
    
    BUG(pixbuf = EF_gtk_sections_dessin(&section, 32, 32), );
    
    gtk_image_set_from_pixbuf(GTK_IMAGE(gtk_builder_get_object(projet->list_gtk.ef_sections_personnalisee.builder, "EF_section_personnalisee_image_forme")), pixbuf);
    
    g_object_unref(pixbuf);
    
    free(nom);
    free(description);
    g_list_free_full(forme, (GDestroyNotify)g_list_free);
    
    return;
}


void EF_gtk_section_personnalisee_ajouter_clicked(GtkButton *button, Projet *projet)
/* Description : Ferme la fenêtre en ajoutant la section.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    double  j, iy, iz, vy, vyp, vz, vzp, s;
    GList   *forme = NULL;
    gchar   *texte = NULL, *description = NULL;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_sections_personnalisee.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Ajout Section Personnalisee");
    
    if (!EF_gtk_section_personnalisee_recupere_donnees(projet, &j, &iy, &iz, &vy, &vyp, &vz, &vzp, &s, &forme, &texte, &description))
    {
        free(texte);
        free(description);
        g_list_free_full(forme, (GDestroyNotify)g_list_free);
        return;
    }
    
    BUG(EF_sections_personnalisee_ajout(projet, texte, description, common_math_f(j, FLOTTANT_UTILISATEUR), common_math_f(iy, FLOTTANT_UTILISATEUR), common_math_f(iz, FLOTTANT_UTILISATEUR), common_math_f(vy, FLOTTANT_UTILISATEUR), common_math_f(vyp, FLOTTANT_UTILISATEUR), common_math_f(vz, FLOTTANT_UTILISATEUR), common_math_f(vzp, FLOTTANT_UTILISATEUR), common_math_f(s, FLOTTANT_UTILISATEUR), forme), );
    
    free(texte);
    free(description);
    
    projet->list_gtk.ef_sections_personnalisee.keep = TRUE;
    gtk_widget_destroy(projet->list_gtk.ef_sections_personnalisee.window);
    
    return;
}


void EF_gtk_section_personnalisee_annuler_clicked(GtkButton *button, Projet *projet)
/* Description : Ferme la fenêtre sans effectuer les modifications.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_sections_personnalisee.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Ajout Section Personnalisee");
    
    gtk_widget_destroy(projet->list_gtk.ef_sections_personnalisee.window);
    
    return;
}


void EF_gtk_section_personnalisee_modifier_clicked(GtkButton *button, Projet *projet)
/* Description : Ferme la fenêtre en appliquant les modifications.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    double  j, iy, iz, vy, vyp, vz, vzp, s;
    GList   *forme;
    gchar   *texte, *description;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_sections_personnalisee.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Ajout Section Personnalisee");
    
    if (!EF_gtk_section_personnalisee_recupere_donnees(projet, &j, &iy, &iz, &vy, &vyp, &vz, &vzp, &s, &forme, &texte, &description))
        return;
    
    BUG(EF_sections_personnalisee_modif(projet, projet->list_gtk.ef_sections_personnalisee.section, texte, description, common_math_f(j, FLOTTANT_UTILISATEUR), common_math_f(iy, FLOTTANT_UTILISATEUR), common_math_f(iz, FLOTTANT_UTILISATEUR), common_math_f(vy, FLOTTANT_UTILISATEUR), common_math_f(vyp, FLOTTANT_UTILISATEUR), common_math_f(vz, FLOTTANT_UTILISATEUR), common_math_f(vzp, FLOTTANT_UTILISATEUR), common_math_f(s, FLOTTANT_UTILISATEUR), forme), );
    
    free(texte);
    free(description);
    
    projet->list_gtk.ef_sections_personnalisee.keep = TRUE;
    gtk_widget_destroy(projet->list_gtk.ef_sections_personnalisee.window);
    
    return;
}


void EF_gtk_section_personnalisee_render_0(GtkTreeViewColumn *tree_column,
  GtkCellRenderer *cell, GtkTreeModel *tree_model, GtkTreeIter *iter, gpointer data2)
/* Description : Affiche la première colonne qui n'a rien à afficher.
 * Paramètres : GtkTreeViewColumn *tree_column : composant à l'origine de l'évènement,
 *            : GtkCellRenderer *cell : la cellule en cours d'édition,
 *            : GtkTreeModel *tree_model : le mode en cours d'édition,
 *            : GtkTreeIter *iter : la ligne en cours d'édition,
 *            : gpointer data2 : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    GtkTreeIter iter2;
    char        *tmp;
    char        *tmp2;
    Projet      *projet = data2;
    int         nombre;
    
    // C'est une ligne de groupe de points
    if (!gtk_tree_model_iter_parent(projet->list_gtk.ef_sections_personnalisee.model, &iter2, iter))
    {
        tmp = gtk_tree_model_get_string_from_iter(projet->list_gtk.ef_sections_personnalisee.model, iter);
        BUGMSG(sscanf(tmp, "%d", &nombre) == 1, , gettext("Erreur impossible.\n"));
        g_free(tmp);
        BUGMSG(tmp = g_strdup_printf(gettext("Groupe %d"), nombre+1), , gettext("Erreur d'allocation mémoire.\n"));
        g_object_set(cell, "text", tmp, NULL);
        g_free(tmp);
    }
    else
    {
        tmp = gtk_tree_model_get_string_from_iter(projet->list_gtk.ef_sections_personnalisee.model, iter);
        tmp2 = strchr(tmp, ':')+1;
        BUGMSG(sscanf(tmp2, "%d", &nombre) == 1, , gettext("Erreur impossible.\n"));
        BUGMSG(tmp2 = g_strdup_printf(gettext("Point %d"), nombre+1), , gettext("Erreur d'allocation mémoire.\n"));
        g_object_set(cell, "text", tmp2, NULL);
        g_free(tmp);
        g_free(tmp2);
    }
    
    return;
}


void EF_gtk_section_personnalisee_render_1(GtkTreeViewColumn *tree_column,
  GtkCellRenderer *cell, GtkTreeModel *tree_model, GtkTreeIter *iter, gpointer data2)
/* Description : Affiche les coordonnées en x du point.
 * Paramètres : GtkTreeViewColumn *tree_column : composant à l'origine de l'évènement,
 *            : GtkCellRenderer *cell : la cellule en cours d'édition,
 *            : GtkTreeModel *tree_model : le mode en cours d'édition,
 *            : GtkTreeIter *iter : la ligne en cours d'édition,
 *            : gpointer data2 : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    EF_Point    *point;
    char        tmp[30];
    
    gtk_tree_model_get(tree_model, iter, 0, &point, -1);
    
    if (point == NULL)
    {
        g_object_set(cell, "text", "", NULL);
        return;
    }
    
    common_math_double_to_char2(point->x, tmp, DECIMAL_DISTANCE);
    
    g_object_set(cell, "text", tmp, NULL);
    
    return;
}


void EF_gtk_section_personnalisee_render_2(GtkTreeViewColumn *tree_column,
  GtkCellRenderer *cell, GtkTreeModel *tree_model, GtkTreeIter *iter, gpointer data2)
/* Description : Affiche les coordonnées en y du point.
 * Paramètres : GtkTreeViewColumn *tree_column : composant à l'origine de l'évènement,
 *            : GtkCellRenderer *cell : la cellule en cours d'édition,
 *            : GtkTreeModel *tree_model : le mode en cours d'édition,
 *            : GtkTreeIter *iter : la ligne en cours d'édition,
 *            : gpointer data2 : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    EF_Point    *point;
    char        tmp[30];
    
    gtk_tree_model_get(tree_model, iter, 0, &point, -1);
    
    if (point == NULL)
    {
        g_object_set(cell, "text", "", NULL);
        return;
    }
    
    common_math_double_to_char2(point->y, tmp, DECIMAL_DISTANCE);
    
    g_object_set(cell, "text", tmp, NULL);
    
    return;
}


void EF_gtk_tree_select_changed(GtkTreeSelection *treeselection, Projet *projet)
/* Description : Permet de activer/désactiver les boutons + et - correspondant au treeview en
 *               fonction de la selection.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    Gtk_EF_Sections_Personnalisee   *ef_gtk;
    GtkTreeIter                     iter, iter2;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    ef_gtk = &projet->list_gtk.ef_sections_personnalisee;
    BUGMSG(ef_gtk->builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Ajout Section Personnalisee");
    
    if (!gtk_tree_selection_get_selected(treeselection, NULL, &iter))
        gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(ef_gtk->builder, "EF_section_personnalisee_button_treeview_remove")), FALSE);
    else
    {
        gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(ef_gtk->builder, "EF_section_personnalisee_button_treeview_remove")), TRUE);
        // Possède un parent donc les coordonnées peuvent être éditées.
        if (gtk_tree_model_iter_parent(ef_gtk->model, &iter2, &iter))
        {
            g_object_set(gtk_builder_get_object(ef_gtk->builder, "EF_section_treeview_cell1"), "editable", TRUE, NULL);
            g_object_set(gtk_builder_get_object(ef_gtk->builder, "EF_section_treeview_cell2"), "editable", TRUE, NULL);
        }
        else
        {
            g_object_set(gtk_builder_get_object(ef_gtk->builder, "EF_section_treeview_cell1"), "editable", FALSE, NULL);
            g_object_set(gtk_builder_get_object(ef_gtk->builder, "EF_section_treeview_cell2"), "editable", FALSE, NULL);
        }
    }
    
    return;
}


void EF_gtk_section_personnalisee_treeview_remove(GtkToolButton *widget, Projet *projet)
/* Description : Permet de supprimer une ligne dans le treeview contenant la liste des points.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    Gtk_EF_Sections_Personnalisee   *ef_gtk;
    GtkTreeIter                     iter;
    EF_Point                        *point;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    ef_gtk = &projet->list_gtk.ef_sections_personnalisee;
    BUGMSG(ef_gtk->builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Ajout Section Personnalisee");
    
    if (!gtk_tree_selection_get_selected(GTK_TREE_SELECTION(gtk_builder_get_object(ef_gtk->builder, "EF_section_treeview_select")), NULL, &iter))
        return;
    
    gtk_tree_model_get(ef_gtk->model, &iter, 0, &point, -1);
    
    // C'est un parent
    if (point == NULL)
    {
        GtkTreeIter iter2;
        if (gtk_tree_model_iter_children(ef_gtk->model, &iter2, &iter))
        {
            do
            {
                gtk_tree_model_get(ef_gtk->model, &iter, 0, &point, -1);
                free(point);
            } while (gtk_tree_model_iter_next(ef_gtk->model, &iter2));
        }
    }
    else
        free(point);
    
    gtk_tree_store_remove(GTK_TREE_STORE(ef_gtk->model), &iter);
    
    EF_gtk_section_personnalisee_check(NULL, projet);
    
    return;
}


void EF_gtk_section_personnalisee_treeview_add(GtkToolButton *widget, Projet *projet)
/* Description : Ajoute une ligne dans le treeview contenant la liste des points.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    Gtk_EF_Sections_Personnalisee   *ef_gtk;
    GtkTreeIter                     iter;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    ef_gtk = &projet->list_gtk.ef_sections_personnalisee;
    BUGMSG(ef_gtk->builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Ajout Section Personnalisee");
    
    // On ajoute un groupe de points
    if (!gtk_tree_selection_get_selected(GTK_TREE_SELECTION(gtk_builder_get_object(ef_gtk->builder, "EF_section_treeview_select")), NULL, &iter))
    {
        gtk_tree_store_append(GTK_TREE_STORE(ef_gtk->model), &iter, NULL);
        gtk_tree_store_set(GTK_TREE_STORE(ef_gtk->model), &iter, 0, NULL, -1);
    }
    else
    {
        GtkTreeIter iter2;
        GtkTreeIter iter3;
        EF_Point    *point;
        
        point = malloc(sizeof(EF_Point));
        point->x = common_math_f(0., FLOTTANT_UTILISATEUR);
        point->y = common_math_f(0., FLOTTANT_UTILISATEUR);
        point->z = common_math_f(0., FLOTTANT_UTILISATEUR);
        
        // Si iter est un parent
        if (!gtk_tree_model_iter_parent(ef_gtk->model, &iter2, &iter))
        {
            // On ajout un point à la fin de la liste afin que la section soit toujours fermée.
            if (!gtk_tree_model_iter_has_child(ef_gtk->model, &iter))
            {
                gtk_tree_store_append(GTK_TREE_STORE(ef_gtk->model), &iter2, &iter);
                gtk_tree_store_set(GTK_TREE_STORE(ef_gtk->model), &iter2, 0, point, -1);
            }
            gtk_tree_model_iter_nth_child(ef_gtk->model, &iter3, &iter, gtk_tree_model_iter_n_children(ef_gtk->model, &iter)-1);
            gtk_tree_store_insert_before(GTK_TREE_STORE(ef_gtk->model), &iter2, &iter, &iter3);
            gtk_tree_store_set(GTK_TREE_STORE(ef_gtk->model), &iter2, 0, point, -1);
        }
        else
        {
            GtkTreeIter *iter_test = gtk_tree_iter_copy(&iter);
            
            if (gtk_tree_model_iter_next(ef_gtk->model, iter_test))
                gtk_tree_store_insert_after(GTK_TREE_STORE(ef_gtk->model), &iter3, &iter2, &iter);
            else
                gtk_tree_store_insert_before(GTK_TREE_STORE(ef_gtk->model), &iter3, &iter2, &iter);
            gtk_tree_iter_free(iter_test);
            gtk_tree_store_set(GTK_TREE_STORE(ef_gtk->model), &iter3, 0, point, -1);
        }
    }
    
    EF_gtk_section_personnalisee_check(NULL, projet);
    
    return;
}


void EF_gtk_section_personnalisee_edit_x(GtkCellRendererText *cell, gchar *path_string,
  gchar *new_text, Projet *projet)
/* Description : Change la coordonnée en x du noeud de la forme de la section.
 * Paramètres : GtkCellRendererText *cell : cellule en cours,
 *            : gchar *path_string : path de la ligne en cours,
 *            : gchar *new_text : nouvelle valeur,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    Gtk_EF_Sections_Personnalisee   *ef_gtk;
    GtkTreeIter     iter;
    double          conversion;
    EF_Point        *point;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    ef_gtk = &projet->list_gtk.ef_sections_personnalisee;;
    BUGMSG(ef_gtk->builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Ajout Section Personnalisee");
    
    gtk_tree_model_get_iter_from_string(ef_gtk->model, &iter, path_string);
    gtk_tree_model_get(ef_gtk->model, &iter, 0, &point, -1);
    
    conversion = common_text_str_to_double(new_text, -INFINITY, FALSE, INFINITY, FALSE);
    if (!isnan(conversion))
    {
        point->x = common_math_f(conversion, FLOTTANT_UTILISATEUR);
        
        gtk_widget_queue_resize(GTK_WIDGET(gtk_builder_get_object(ef_gtk->builder, "EF_section_personnalisee_treeview")));
    }
    
    EF_gtk_section_personnalisee_check(NULL, projet);
    
    return;
}


void EF_gtk_section_personnalisee_edit_y(GtkCellRendererText *cell, gchar *path_string,
  gchar *new_text, Projet *projet)
/* Description : Change la coordonnée en y du noeud de la forme de la section.
 * Paramètres : GtkCellRendererText *cell : cellule en cours,
 *            : gchar *path_string : path de la ligne en cours,
 *            : gchar *new_text : nouvelle valeur,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    Gtk_EF_Sections_Personnalisee   *ef_gtk;
    GtkTreeIter     iter;
    double          conversion;
    EF_Point        *point;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    ef_gtk = &projet->list_gtk.ef_sections_personnalisee;;
    BUGMSG(ef_gtk->builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Ajout Section Personnalisee");
    
    gtk_tree_model_get_iter_from_string(ef_gtk->model, &iter, path_string);
    gtk_tree_model_get(ef_gtk->model, &iter, 0, &point, -1);
    
    conversion = common_text_str_to_double(new_text, -INFINITY, FALSE, INFINITY, FALSE);
    if (!isnan(conversion))
    {
        point->y = common_math_f(conversion, FLOTTANT_UTILISATEUR);
        
        gtk_widget_queue_resize(GTK_WIDGET(gtk_builder_get_object(ef_gtk->builder, "EF_section_personnalisee_treeview")));
    }
    
    EF_gtk_section_personnalisee_check(NULL, projet);
    
    return;
}


gboolean EF_gtk_section_personnalisee(Projet *projet, EF_Section *section)
/* Description : Affichage de la fenêtre permettant de créer ou modifier une section de type
 *               personnalisée.
 * Paramètres : Projet *projet : la variable projet,
 *            : EF_Section *section : section à modifier. NULL si nouvelle section,
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Echec : FALSE :
 *             projet == NULL,
 *             Fenêtre graphique déjà initialisée.
 */
{
    Gtk_EF_Sections_Personnalisee  *ef_gtk;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    
    ef_gtk = &projet->list_gtk.ef_sections_personnalisee;
    if (projet->list_gtk.ef_sections_personnalisee.builder != NULL)
    {
        gtk_window_present(GTK_WINDOW(projet->list_gtk.ef_sections_personnalisee.window));
        if (projet->list_gtk.ef_sections_personnalisee.section == section)
            return TRUE;
    }
    else
    {
        ef_gtk->builder = gtk_builder_new();
        BUGMSG(gtk_builder_add_from_resource(ef_gtk->builder, "/org/2lgc/codegui/ui/EF_sections_personnalisee.ui", NULL) != 0, FALSE, gettext("Builder Failed\n"));
        gtk_builder_connect_signals(ef_gtk->builder, projet);
        ef_gtk->window = GTK_WIDGET(gtk_builder_get_object(ef_gtk->builder, "EF_section_personnalisee_window"));
        ef_gtk->model = GTK_TREE_MODEL(gtk_builder_get_object(ef_gtk->builder, "EF_section_personnalisee_treestore"));
        ef_gtk->keep = FALSE;
    }
    
    if (section == NULL)
    {
        gtk_window_set_title(GTK_WINDOW(ef_gtk->window), gettext("Ajout d'une section personnalisée"));
        ef_gtk->section = NULL;
        
        gtk_button_set_label(GTK_BUTTON(gtk_builder_get_object(ef_gtk->builder, "EF_section_personnalisee_button_add_edit")), "gtk-add");
        g_signal_connect(gtk_builder_get_object(ef_gtk->builder, "EF_section_personnalisee_button_add_edit"), "clicked", G_CALLBACK(EF_gtk_section_personnalisee_ajouter_clicked), projet);
        EF_gtk_section_personnalisee_check(NULL, projet);
    }
    else
    {
        gchar                   tmp[30];
        Section_Personnalisee   *data;
        GList                   *list_parcours;
        
        gtk_window_set_title(GTK_WINDOW(ef_gtk->window), gettext("Modification d'une section personnalisée"));
        ef_gtk->section = section;
        BUGMSG(ef_gtk->section->type == SECTION_PERSONNALISEE, FALSE, gettext("La section à modifier n'est pas personnalisée.\n"));
        data = ef_gtk->section->data;
        
        gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(gtk_builder_get_object(ef_gtk->builder, "EF_section_personnalisee_textview_nom"))), ef_gtk->section->nom, -1);
        gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(gtk_builder_get_object(ef_gtk->builder, "EF_section_personnalisee_textview_description"))), data->description, -1);
        common_math_double_to_char2(data->j, tmp, DECIMAL_M4);
        gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder, "EF_section_personnalisee_buffer_j")), tmp, -1);
        common_math_double_to_char2(data->iy, tmp, DECIMAL_M4);
        gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder, "EF_section_personnalisee_buffer_iy")), tmp, -1);
        common_math_double_to_char2(data->iz, tmp, DECIMAL_M4);
        gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder, "EF_section_personnalisee_buffer_iz")), tmp, -1);
        common_math_double_to_char2(data->vy, tmp, DECIMAL_DISTANCE);
        gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder, "EF_section_personnalisee_buffer_vy")), tmp, -1);
        common_math_double_to_char2(data->vyp, tmp, DECIMAL_DISTANCE);
        gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder, "EF_section_personnalisee_buffer_vyp")), tmp, -1);
        common_math_double_to_char2(data->vz, tmp, DECIMAL_DISTANCE);
        gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder, "EF_section_personnalisee_buffer_vz")), tmp, -1);
        common_math_double_to_char2(data->vzp, tmp, DECIMAL_DISTANCE);
        gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder, "EF_section_personnalisee_buffer_vzp")), tmp, -1);
        common_math_double_to_char2(data->s, tmp, DECIMAL_SURFACE);
        gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder, "EF_section_personnalisee_buffer_s")), tmp, -1);
        
        list_parcours = data->forme;
        while (list_parcours != NULL)
        {
            GList       *list_parcours2 = list_parcours->data;
            GtkTreeIter iter, iter_last;
            EF_Point    *point_bis;
            
            gtk_tree_store_append(GTK_TREE_STORE(ef_gtk->model), &iter, NULL);
            gtk_tree_store_set(GTK_TREE_STORE(ef_gtk->model), &iter, 0, NULL, -1);
            
            // On ajoute le dernier point de la forme qui est le même que le premier.
            if (list_parcours2 != NULL)
            {
                gtk_tree_store_append(GTK_TREE_STORE(ef_gtk->model), &iter_last, &iter);
                point_bis = malloc(sizeof(EF_Point));
                memcpy(point_bis, list_parcours2->data, sizeof(EF_Point));
                gtk_tree_store_set(GTK_TREE_STORE(ef_gtk->model), &iter_last, 0, point_bis, -1);
            }
            
            while (list_parcours2 != NULL)
            {
                GtkTreeIter iter2;
                
                gtk_tree_store_insert_before(GTK_TREE_STORE(ef_gtk->model), &iter2, &iter, &iter_last);
                // On forme le dernier point à être le même que le premier.
                if (list_parcours2 != list_parcours->data)
                {
                    point_bis = malloc(sizeof(EF_Point));
                    memcpy(point_bis, list_parcours2->data, sizeof(EF_Point));
                }
                gtk_tree_store_set(GTK_TREE_STORE(ef_gtk->model), &iter2, 0, point_bis, -1);
                
                list_parcours2 = g_list_next(list_parcours2);
            }
            
            list_parcours = g_list_next(list_parcours);
        }
        
        gtk_button_set_label(GTK_BUTTON(gtk_builder_get_object(ef_gtk->builder, "EF_section_personnalisee_button_add_edit")), "gtk-edit");
        g_signal_connect(gtk_builder_get_object(ef_gtk->builder, "EF_section_personnalisee_button_add_edit"), "clicked", G_CALLBACK(EF_gtk_section_personnalisee_modifier_clicked), projet);
    }
    
    gtk_tree_view_column_set_cell_data_func(GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(ef_gtk->builder, "EF_section_treeview_column0")), GTK_CELL_RENDERER(gtk_builder_get_object(ef_gtk->builder, "EF_section_treeview_cell0")), EF_gtk_section_personnalisee_render_0, projet, NULL);
    gtk_tree_view_column_set_cell_data_func(GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(ef_gtk->builder, "EF_section_treeview_column1")), GTK_CELL_RENDERER(gtk_builder_get_object(ef_gtk->builder, "EF_section_treeview_cell1")), EF_gtk_section_personnalisee_render_1, projet, NULL);
    gtk_tree_view_column_set_cell_data_func(GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(ef_gtk->builder, "EF_section_treeview_column2")), GTK_CELL_RENDERER(gtk_builder_get_object(ef_gtk->builder, "EF_section_treeview_cell2")), EF_gtk_section_personnalisee_render_2, projet, NULL);
    
    gtk_window_set_transient_for(GTK_WINDOW(ef_gtk->window), GTK_WINDOW(projet->list_gtk.comp.window));
    
    return TRUE;
}
#endif
