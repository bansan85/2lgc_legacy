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
#include "common_math.h"
#include "common_selection.h"
#include "common_text.h"
#include "EF_relachement.h"
#include "1992_1_1_barres.h"

G_MODULE_EXPORT void EF_gtk_relachements_fermer(GtkButton *button __attribute__((unused)),
  Projet *projet)
/* Description : Ferme la fenêtre.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_relachements.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Relachement");
    
    gtk_widget_destroy(projet->list_gtk.ef_relachements.window);
    
    return;
}


G_MODULE_EXPORT void EF_gtk_relachements_window_destroy(GtkWidget *object __attribute__((unused)),
  Projet *projet)
/* Description : met projet->list_gtk.ef_relachements.builder à NULL quand la fenêtre se ferme,
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_relachements.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Relachement");
    
    g_object_unref(G_OBJECT(projet->list_gtk.ef_relachements.builder));
    projet->list_gtk.ef_relachements.builder = NULL;
    
    return;
}


G_MODULE_EXPORT gboolean EF_gtk_relachements_window_key_press(
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
    BUGMSG(projet->list_gtk.ef_relachements.builder, TRUE, gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Relachement");
    
    if (event->key.keyval == GDK_KEY_Escape)
    {
        gtk_widget_destroy(projet->list_gtk.ef_relachements.window);
        return TRUE;
    }
    else
        return FALSE;
}


G_MODULE_EXPORT void EF_gtk_relachements_select_changed(
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
    BUGMSG(projet->list_gtk.ef_relachements.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Relachement");
    
    // Si aucun relâchenement n'est sélectionné, il n'est pas possible d'en supprimer ou d'en
    // éditer un.
    if (!gtk_tree_selection_get_selected(GTK_TREE_SELECTION(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_treeview_select")), &model, &Iter))
    {
        gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_boutton_supprimer_direct")), FALSE);
        gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_boutton_supprimer_menu")), FALSE);
        gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_boutton_supprimer_direct")), TRUE);
        gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_boutton_supprimer_menu")), FALSE);
    }
    else
    {
        char                *nom;
        EF_Relachement      *relachement;
        GtkCellRendererText *cell;
        GList               *liste_relachements = NULL;
        
        gtk_tree_model_get(model, &Iter, 0, &nom, -1);
        
        BUG(relachement = EF_relachement_cherche_nom(projet, nom, TRUE), );
        
        liste_relachements = g_list_append(liste_relachements, relachement);
        if (_1992_1_1_barres_cherche_dependances(projet, NULL, NULL, NULL, NULL, liste_relachements, NULL, NULL, NULL, NULL, FALSE, FALSE))
        {
            gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_boutton_supprimer_direct")), FALSE);
            gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_boutton_supprimer_menu")), TRUE);
            gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_boutton_supprimer_direct")), FALSE);
            gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_boutton_supprimer_menu")), TRUE);
        }
        else
        {
            gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_boutton_supprimer_direct")), TRUE);
            gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_boutton_supprimer_menu")), FALSE);
            gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_boutton_supprimer_direct")), TRUE);
            gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_boutton_supprimer_menu")), FALSE);
        }
        
        cell = GTK_CELL_RENDERER_TEXT(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_treeview_cell2"));
        switch (relachement->rx_debut)
        {
            case EF_RELACHEMENT_BLOQUE :
            case EF_RELACHEMENT_LIBRE :
            {
                g_object_set(cell, "editable", FALSE, NULL);
                break;
            }
            case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
            {
                g_object_set(cell, "editable", TRUE, NULL);
                break;
            }
            default :
            {
                BUGMSG(NULL, , gettext("Relachement %d inconnu."), relachement->rx_debut);
                break;
            }
        }
        cell = GTK_CELL_RENDERER_TEXT(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_treeview_cell4"));
        switch (relachement->ry_debut)
        {
            case EF_RELACHEMENT_BLOQUE :
            case EF_RELACHEMENT_LIBRE :
            {
                g_object_set(cell, "editable", FALSE, NULL);
                break;
            }
            case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
            {
                g_object_set(cell, "editable", TRUE, NULL);
                break;
            }
            default :
            {
                BUGMSG(NULL, , gettext("Relachement %d inconnu."), relachement->ry_debut);
                break;
            }
        }
        cell = GTK_CELL_RENDERER_TEXT(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_treeview_cell6"));
        switch (relachement->rz_debut)
        {
            case EF_RELACHEMENT_BLOQUE :
            case EF_RELACHEMENT_LIBRE :
            {
                g_object_set(cell, "editable", FALSE, NULL);
                break;
            }
            case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
            {
                g_object_set(cell, "editable", TRUE, NULL);
                break;
            }
            default :
            {
                BUGMSG(NULL, , gettext("Relachement %d inconnu."), relachement->rz_debut);
                break;
            }
        }
        cell = GTK_CELL_RENDERER_TEXT(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_treeview_cell8"));
        switch (relachement->rx_fin)
        {
            case EF_RELACHEMENT_BLOQUE :
            case EF_RELACHEMENT_LIBRE :
            {
                g_object_set(cell, "editable", FALSE, NULL);
                break;
            }
            case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
            {
                g_object_set(cell, "editable", TRUE, NULL);
                break;
            }
            default :
            {
                BUGMSG(NULL, , gettext("Relachement %d inconnu."), relachement->rx_fin);
                break;
            }
        }
        cell = GTK_CELL_RENDERER_TEXT(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_treeview_cell10"));
        switch (relachement->ry_fin)
        {
            case EF_RELACHEMENT_BLOQUE :
            case EF_RELACHEMENT_LIBRE :
            {
                g_object_set(cell, "editable", FALSE, NULL);
                break;
            }
            case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
            {
                g_object_set(cell, "editable", TRUE, NULL);
                break;
            }
            default :
            {
                BUGMSG(NULL, , gettext("Relachement %d inconnu."), relachement->ry_fin);
                break;
            }
        }
        cell = GTK_CELL_RENDERER_TEXT(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_treeview_cell12"));
        switch (relachement->rz_fin)
        {
            case EF_RELACHEMENT_BLOQUE :
            case EF_RELACHEMENT_LIBRE :
            {
                g_object_set(cell, "editable", FALSE, NULL);
                break;
            }
            case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
            {
                g_object_set(cell, "editable", TRUE, NULL);
                break;
            }
            default :
            {
                BUGMSG(NULL, , gettext("Relachement %d inconnu."), relachement->rz_fin);
                break;
            }
        }
        
        free(nom);
    }
    
    return;
}


G_MODULE_EXPORT void EF_gtk_relachements_boutton_supprimer_menu(
  GtkButton *widget __attribute__((unused)), Projet *projet)
/* Description : Affiche la liste des dépendances dans le menu lorsqu'on clique sur le bouton
 * Paramètres : GtkButton *widget : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 *   Echec : projet == NULL,
 *           interface graphique non initialisée.
 */
{
    GtkTreeModel        *model;
    GtkTreeIter         Iter;
    char                *nom;
    EF_Relachement      *relachement;
    GList               *liste_relachements = NULL, *liste_noeuds_dep, *liste_barres_dep, *liste_charges_dep;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_relachements.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Relachement");
    
    // Si aucun relâchenement n'est sélectionné, il n'est pas possible d'en supprimer ou d'en
    // éditer un.
    if (!gtk_tree_selection_get_selected(GTK_TREE_SELECTION(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_treeview_select")), &model, &Iter))
        BUGMSG(NULL, , gettext("Aucun élément n'est sélectionné.\n"));
    
    gtk_tree_model_get(model, &Iter, 0, &nom, -1);
    
    BUG(relachement = EF_relachement_cherche_nom(projet, nom, TRUE), );
    
    liste_relachements = g_list_append(liste_relachements, relachement);
    BUG(_1992_1_1_barres_cherche_dependances(projet, NULL, NULL, NULL, NULL, liste_relachements, NULL, &liste_noeuds_dep, &liste_barres_dep, &liste_charges_dep, FALSE, FALSE), );
    
    if ((liste_noeuds_dep != NULL) || (liste_barres_dep != NULL) || (liste_charges_dep != NULL))
    {
        char    *desc;
        
        desc = common_text_dependances(liste_noeuds_dep, liste_barres_dep, liste_charges_dep, projet);
        gtk_menu_item_set_label(GTK_MENU_ITEM(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_supprimer_menu_barres")), desc);
        free(desc);
    }
    else
        BUGMSG(NULL, , gettext("L'élément ne possède aucune dépendance.\n"));
    
    free(nom);
    g_list_free(liste_noeuds_dep);
    g_list_free(liste_barres_dep);
    g_list_free(liste_charges_dep);
    
    return;
}


G_MODULE_EXPORT void EF_gtk_relachements_edit_nom(
  GtkCellRendererText *cell __attribute__((unused)), gchar *path_string, gchar *new_text,
  Projet *projet)
/* Description : Modification du nom d'un relâchement.
 * Paramètres : GtkCellRendererText *cell : cellule en cours,
 *            : gchar *path_string : path de la ligne en cours,
 *            : gchar *new_text : le nouveau nom,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
*/
{
    Gtk_EF_Relachements *ef_gtk;
    GtkTreeModel        *model;
    GtkTreeIter         iter;
    GtkTreePath         *path;
    char                *nom;
    EF_Relachement      *relachement;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_relachements.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Relachement");
    
    ef_gtk = &projet->list_gtk.ef_relachements;
    model = GTK_TREE_MODEL(ef_gtk->relachements);
    path = gtk_tree_path_new_from_string(path_string);
    gtk_tree_model_get_iter(model, &iter, path);
    gtk_tree_path_free(path);
    gtk_tree_model_get(model, &iter, 0, &nom, -1);
    if (strcmp(nom, new_text) == 0)
    {
        free(nom);
        return;
    }
    BUG(relachement = EF_relachement_cherche_nom(projet, nom, TRUE), );
    free(nom);
    if (strcmp(relachement->nom, new_text) == 0)
        return;

    BUG(EF_relachement_modif(projet, relachement, new_text, UINT_MAX, NULL, UINT_MAX, NULL, UINT_MAX, NULL, UINT_MAX, NULL, UINT_MAX, NULL, UINT_MAX, NULL), );
    
    return;
}


G_MODULE_EXPORT void EF_gtk_relachements_supprimer_direct(
  GtkButton *button __attribute__((unused)), Projet *projet)
/* Description : Supprime le relâchement sélectionné dans le treeview.
 * Paramètres : GtkWidget *widget : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    GtkTreeIter     iter;
    GtkTreeModel    *model;
    char            *nom;
    EF_Relachement  *relachement;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_relachements.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Relachement");
    
    if (!gtk_tree_selection_get_selected(GTK_TREE_SELECTION(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_treeview_select")), &model, &iter))
        return;
    
    gtk_tree_model_get(model, &iter, 0, &nom, -1);
    
    BUG(relachement = EF_relachement_cherche_nom(projet, nom, TRUE), );
    BUG(EF_relachement_supprime(relachement, TRUE, projet), );
    
    BUG(m3d_rafraichit(projet), );
    
    free(nom);
    
    return;
}


G_MODULE_EXPORT gboolean EF_gtk_relachements_treeview_key_press(
  GtkWidget *widget __attribute__((unused)), GdkEvent *event, Projet *projet)
/* Description : Supprime un relâchement sans dépendance si la touche SUPPR est appuyée.
 * Paramètres : GtkWidget *widget : composant à l'origine de l'évènement,
 *            : GdkEvent *event : Caractéristique de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : TRUE si la touche SUPPR est pressée, FALSE sinon.
 *   Echec : projet == NULL,
 *           interface graphique non initialisée.
 *  
 */
{
    BUGMSG(projet, TRUE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_relachements.builder, TRUE, gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Relachement");
    
    if (event->key.keyval == GDK_KEY_Delete)
    {
        GtkTreeIter     Iter;
        GtkTreeModel    *model;
        
        if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_treeview_select")), &model, &Iter))
        {
            char            *nom;
            EF_Relachement  *relachement;
            
            GList   *liste_relachements = NULL;
            
            gtk_tree_model_get(model, &Iter, 0, &nom, -1);
            
            BUG(relachement = EF_relachement_cherche_nom(projet, nom, TRUE), FALSE);
            
            liste_relachements = g_list_append(liste_relachements, relachement);
            if (_1992_1_1_barres_cherche_dependances(projet, NULL, NULL, NULL, NULL, liste_relachements, NULL, NULL, NULL, NULL, FALSE, FALSE) == FALSE)
                EF_gtk_relachements_supprimer_direct(NULL, projet);
            
            free(nom);
            g_list_free(liste_relachements);
        }
        return TRUE;
    }
    else
        return FALSE;
}


G_MODULE_EXPORT void EF_gtk_relachements_supprimer_menu_barres(
  GtkButton *button __attribute__((unused)), Projet *projet)
/* Description : Supprime le relachement sélectionné dans le treeview, y compris les barres
 *               l'utilisant.
 * Paramètres : GtkWidget *widget : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    GtkTreeIter     iter;
    GtkTreeModel    *model;
    char            *nom;
    EF_Relachement  *relachement;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_relachements.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Relachement");
    
    if (!gtk_tree_selection_get_selected(GTK_TREE_SELECTION(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_treeview_select")), &model, &iter))
        return;
    
    gtk_tree_model_get(model, &iter, 0, &nom, -1);
    
    BUG(relachement = EF_relachement_cherche_nom(projet, nom, TRUE), );
    BUG(EF_relachement_supprime(relachement, FALSE, projet), );
    
    BUG(m3d_rafraichit(projet), );
    
    free(nom);
    
    return;
}


G_MODULE_EXPORT void EF_gtk_relachements_ajouter(GtkButton *button __attribute__((unused)),
  Projet *projet)
/* Description : Ajoute un nouveau relâchement vierge au treeview
 *               d'affichage.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    EF_Relachement      *relachement;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_relachements.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Relâchement");
    
    if (EF_relachement_cherche_nom(projet, gettext("Sans nom"), FALSE) == NULL)
        BUG(relachement = EF_relachement_ajout(projet, gettext("Sans nom"), EF_RELACHEMENT_BLOQUE, NULL, EF_RELACHEMENT_BLOQUE, NULL, EF_RELACHEMENT_BLOQUE, NULL, EF_RELACHEMENT_BLOQUE, NULL, EF_RELACHEMENT_BLOQUE, NULL, EF_RELACHEMENT_BLOQUE, NULL), );
    else
    {
        char    *nom;
        int     i = 2;
        
        nom = g_strdup_printf("%s (%d)", gettext("Sans nom"), i);
        while (EF_relachement_cherche_nom(projet, nom, FALSE) != NULL)
        {
            i++;
            free(nom);
            nom = g_strdup_printf("%s (%d)", gettext("Sans nom"), i);
        }
        BUG(relachement = EF_relachement_ajout(projet, nom, EF_RELACHEMENT_BLOQUE, NULL, EF_RELACHEMENT_BLOQUE, NULL, EF_RELACHEMENT_BLOQUE, NULL, EF_RELACHEMENT_BLOQUE, NULL, EF_RELACHEMENT_BLOQUE, NULL, EF_RELACHEMENT_BLOQUE, NULL), );
        free(nom);
    }
    
    return;
}


G_MODULE_EXPORT void EF_gtk_relachements_edit_clicked(GtkCellRendererText *cell,
  gchar *path_string, gchar *new_text, Projet *projet)
/* Description : Edite le relâchement sélectionné.
 * Paramètres : GtkCellRendererText *cell : cellule en cours,
 *            : gchar *path_string : path de la ligne en cours,
 *            : gchar *new_text : nouvelle valeur,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 *   Echec : projet == NULL,
 *           interface graphique non initialisée.
 */
{
    Gtk_EF_Relachements *ef_gtk;
    GtkTreeModel        *model;
    GtkTreeIter         iter;
    GtkTreePath         *path;
    gint                column = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(cell), "column"));
    char                *nom;
    EF_Relachement      *relachement;
    double              conversion;
    char                *fake = (char*)malloc(sizeof(char)*(strlen(new_text)+1));
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_relachements.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Relachement");
    
    ef_gtk = &projet->list_gtk.ef_relachements;
    model = GTK_TREE_MODEL(ef_gtk->relachements);
    path = gtk_tree_path_new_from_string(path_string);
    gtk_tree_model_get_iter(model, &iter, path);
    gtk_tree_path_free(path);
    gtk_tree_model_get(model, &iter, 0, &nom, -1);
    
    BUG(relachement = EF_relachement_cherche_nom(projet, nom, TRUE), );
    free(nom);
    
    if (sscanf(new_text, "%lf%s", &conversion, fake) != 1)
    {
        free(fake);
        return;
    }
    
    if (column == 0)
    {
        switch (relachement->rx_debut)
        {
            case EF_RELACHEMENT_BLOQUE :
            case EF_RELACHEMENT_LIBRE :
            {
                BUGMSG(NULL, , "Impossible d'éditer ce type de relâchement.\n");
                break;
            }
            case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
            {
                EF_Relachement_Donnees_Elastique_Lineaire data;
                
                data.raideur = conversion;
                BUG(EF_relachement_modif(projet, relachement, NULL, UINT_MAX, &data, UINT_MAX, NULL, UINT_MAX, NULL, UINT_MAX, NULL, UINT_MAX, NULL, UINT_MAX, NULL), );
                
                break;
            }
            default :
            {
                BUGMSG(NULL, , "Le type de relâchement est inconnu.\n");
                break;
            }
        }
    }
    else if (column == 1)
    {
        switch (relachement->ry_debut)
        {
            case EF_RELACHEMENT_BLOQUE :
            case EF_RELACHEMENT_LIBRE :
            {
                BUGMSG(NULL, , "Impossible d'éditer ce type de relâchement.\n");
                break;
            }
            case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
            {
                EF_Relachement_Donnees_Elastique_Lineaire data;
                
                data.raideur = conversion;
                BUG(EF_relachement_modif(projet, relachement, NULL, UINT_MAX, NULL, UINT_MAX, &data, UINT_MAX, NULL, UINT_MAX, NULL, UINT_MAX, NULL, UINT_MAX, NULL), );
                
                break;
            }
            default :
            {
                BUGMSG(NULL, , "Le type de relâchement est inconnu.\n");
                break;
            }
        }
    }
    else if (column == 2)
    {
        switch (relachement->rz_debut)
        {
            case EF_RELACHEMENT_BLOQUE :
            case EF_RELACHEMENT_LIBRE :
            {
                BUGMSG(NULL, , "Impossible d'éditer ce type de relâchement.\n");
                break;
            }
            case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
            {
                EF_Relachement_Donnees_Elastique_Lineaire data;
                
                data.raideur = conversion;
                BUG(EF_relachement_modif(projet, relachement, NULL, UINT_MAX, NULL, UINT_MAX, NULL, UINT_MAX, &data, UINT_MAX, NULL, UINT_MAX, NULL, UINT_MAX, NULL), );
                
                break;
            }
            default :
            {
                BUGMSG(NULL, , "Le type de relâchement est inconnu.\n");
                break;
            }
        }
    }
    else if (column == 3)
    {
        switch (relachement->rx_fin)
        {
            case EF_RELACHEMENT_BLOQUE :
            case EF_RELACHEMENT_LIBRE :
            {
                BUGMSG(NULL, , "Impossible d'éditer ce type de relâchement.\n");
                break;
            }
            case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
            {
                EF_Relachement_Donnees_Elastique_Lineaire data;
                
                data.raideur = conversion;
                BUG(EF_relachement_modif(projet, relachement, NULL, UINT_MAX, NULL, UINT_MAX, NULL, UINT_MAX, NULL, UINT_MAX, &data, UINT_MAX, NULL, UINT_MAX, NULL), );
                
                break;
            }
            default :
            {
                BUGMSG(NULL, , "Le type de relâchement est inconnu.\n");
                break;
            }
        }
    }
    else if (column == 4)
    {
        switch (relachement->ry_fin)
        {
            case EF_RELACHEMENT_BLOQUE :
            case EF_RELACHEMENT_LIBRE :
            {
                BUGMSG(NULL, , "Impossible d'éditer ce type de relâchement.\n");
                break;
            }
            case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
            {
                EF_Relachement_Donnees_Elastique_Lineaire data;
                
                data.raideur = conversion;
                BUG(EF_relachement_modif(projet, relachement, NULL, UINT_MAX, NULL, UINT_MAX, NULL, UINT_MAX, NULL, UINT_MAX, NULL, UINT_MAX, &data, UINT_MAX, NULL), );
                
                break;
            }
            default :
            {
                BUGMSG(NULL, , "Le type de relâchement est inconnu.\n");
                break;
            }
        }
    }
    else if (column == 5)
    {
        switch (relachement->rz_fin)
        {
            case EF_RELACHEMENT_BLOQUE :
            case EF_RELACHEMENT_LIBRE :
            {
                BUGMSG(NULL, , "Impossible d'éditer ce type de relâchement.\n");
                break;
            }
            case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
            {
                EF_Relachement_Donnees_Elastique_Lineaire data;
                
                data.raideur = conversion;
                BUG(EF_relachement_modif(projet, relachement, NULL, UINT_MAX, NULL, UINT_MAX, NULL, UINT_MAX, NULL, UINT_MAX, NULL, UINT_MAX, NULL, UINT_MAX, &data), );
                
                break;
            }
            default :
            {
                BUGMSG(NULL, , "Le type de relâchement est inconnu.\n");
                break;
            }
        }
    }
    
    return;
}


G_MODULE_EXPORT void EF_gtk_relachements_edit_type(GtkCellRendererText *cell,
  gchar *path_string, gchar *new_text, Projet *projet)
/* Description : Edite le type du relâchement sélectionné.
 * Paramètres : GtkCellRendererText *cell : cellule en cours,
 *            : gchar *path_string : path de la ligne en cours,
 *            : gchar *new_text : le nouveau type du relâchement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 *   Echec : projet == NULL,
 *           interface graphique non initialisée.
 */
{
    Gtk_EF_Relachements *ef_gtk;
    GtkTreeModel        *model;
    GtkTreeIter         iter;
    GtkTreePath         *path;
    gint                column = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(cell), "column"));
    char                *nom;
    EF_Relachement      *relachement;
    EF_Relachement_Type type;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_relachements.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Relachement");
    
    ef_gtk = &projet->list_gtk.ef_relachements;
    model = GTK_TREE_MODEL(ef_gtk->relachements);
    path = gtk_tree_path_new_from_string(path_string);
    gtk_tree_model_get_iter(model, &iter, path);
    gtk_tree_path_free(path);
    gtk_tree_model_get(model, &iter, 0, &nom, -1);
    
    BUG(relachement = EF_relachement_cherche_nom(projet, nom, TRUE), );
    free(nom);
    
    if (strcmp(gettext("Bloqué"), new_text) == 0)
        type = EF_RELACHEMENT_BLOQUE;
    else if (strcmp(gettext("Libre"), new_text) == 0)
        type = EF_RELACHEMENT_LIBRE;
    else if (strcmp(gettext("Linéaire"), new_text) == 0)
        type = EF_RELACHEMENT_ELASTIQUE_LINEAIRE;
    else
        BUGMSG(NULL, , gettext("Le type de relâchement est inconnu.\n"));
    
    switch (column)
    {
        case 0 :
        {
            BUG(EF_relachement_modif(projet, relachement, NULL, type, NULL, UINT_MAX, NULL, UINT_MAX, NULL, UINT_MAX, NULL, UINT_MAX, NULL, UINT_MAX, NULL), );
            break;
        }
        case 1 :
        {
            BUG(EF_relachement_modif(projet, relachement, NULL, UINT_MAX, NULL, type, NULL, UINT_MAX, NULL, UINT_MAX, NULL, UINT_MAX, NULL, UINT_MAX, NULL), );
            break;
        }
        case 2 :
        {
            BUG(EF_relachement_modif(projet, relachement, NULL, UINT_MAX, NULL, UINT_MAX, NULL, type, NULL, UINT_MAX, NULL, UINT_MAX, NULL, UINT_MAX, NULL), );
            break;
        }
        case 3 :
        {
            BUG(EF_relachement_modif(projet, relachement, NULL, UINT_MAX, NULL, UINT_MAX, NULL, UINT_MAX, NULL, type, NULL, UINT_MAX, NULL, UINT_MAX, NULL), );
            break;
        }
        case 4 :
        {
            BUG(EF_relachement_modif(projet, relachement, NULL, UINT_MAX, NULL, UINT_MAX, NULL, UINT_MAX, NULL, UINT_MAX, NULL, type, NULL, UINT_MAX, NULL), );
            break;
        }
        case 5 :
        {
            BUG(EF_relachement_modif(projet, relachement, NULL, UINT_MAX, NULL, UINT_MAX, NULL, UINT_MAX, NULL, UINT_MAX, NULL, UINT_MAX, NULL, type, NULL), );
            break;
        }
        default :
        {
            BUGMSG(NULL, , "Paramètre %s incorrect.\n", "column");
            break;
        }
    }
    
    return;
}


G_MODULE_EXPORT void EF_gtk_relachement(Projet *projet)
/* Description : Création de la fenêtre permettant d'afficher et d'éditer les relâchements sous
 *               forme d'un tableau.
 * Paramètres : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 *   Echec : projet == NULL,
 *           interface graphique impossible à générer.
 */
{
    Gtk_EF_Relachements *ef_gtk;
    GList               *list_parcours;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    if (projet->list_gtk.ef_relachements.builder != NULL)
    {
        gtk_window_present(GTK_WINDOW(projet->list_gtk.ef_relachements.window));
        return;
    }
    
    ef_gtk = &projet->list_gtk.ef_relachements;
    
    ef_gtk->builder = gtk_builder_new();
    BUGMSG(gtk_builder_add_from_file(ef_gtk->builder, DATADIR"/ui/EF_relachements.ui", NULL) != 0, , gettext("Builder Failed\n"));
    gtk_builder_connect_signals(ef_gtk->builder, projet);
    
    g_object_set_data(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_treeview_cell1"), "column", GINT_TO_POINTER(0));
    g_object_set_data(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_treeview_cell2"), "column", GINT_TO_POINTER(0));
    g_object_set_data(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_treeview_cell3"), "column", GINT_TO_POINTER(1));
    g_object_set_data(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_treeview_cell4"), "column", GINT_TO_POINTER(1));
    g_object_set_data(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_treeview_cell5"), "column", GINT_TO_POINTER(2));
    g_object_set_data(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_treeview_cell6"), "column", GINT_TO_POINTER(2));
    g_object_set_data(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_treeview_cell7"), "column", GINT_TO_POINTER(3));
    g_object_set_data(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_treeview_cell8"), "column", GINT_TO_POINTER(3));
    g_object_set_data(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_treeview_cell9"), "column", GINT_TO_POINTER(4));
    g_object_set_data(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_treeview_cell10"), "column", GINT_TO_POINTER(4));
    g_object_set_data(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_treeview_cell11"), "column", GINT_TO_POINTER(5));
    g_object_set_data(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_treeview_cell12"), "column", GINT_TO_POINTER(5));
    
    ef_gtk->window = GTK_WIDGET(gtk_builder_get_object(ef_gtk->builder, "EF_relachements_window"));
    ef_gtk->relachements = GTK_TREE_STORE(gtk_builder_get_object(ef_gtk->builder, "EF_relachements_treestore"));
    
    list_parcours = projet->ef_donnees.relachements;
    while (list_parcours != NULL)
    {
        EF_Relachement  *relachement = (EF_Relachement *)list_parcours->data;
        
        gtk_tree_store_append(ef_gtk->relachements, &relachement->Iter_fenetre, NULL);
        BUG(EF_relachements_update_ligne_treeview(projet, relachement), );
        
        list_parcours = g_list_next(list_parcours);
    }
    
    gtk_window_set_transient_for(GTK_WINDOW(ef_gtk->window), GTK_WINDOW(projet->list_gtk.comp.window));
}

#endif
