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
#include <math.h>

#include "common_m3d.hpp"

#include "common_projet.h"
#include "common_erreurs.h"
#include "common_gtk.h"
#include "common_math.h"
#include "common_selection.h"
#include "common_text.h"
#include "1992_1_1_materiaux.h"
#include "1992_1_1_barres.h"
#include "1993_1_1_materiaux.h"
#include "EF_materiaux.h"

void EF_gtk_materiaux_fermer(GtkButton *button, Projet *projet)
/* Description : Ferme la fenêtre.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet")
    BUGMSG(projet->list_gtk.ef_materiaux.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Matériau")
    
    gtk_widget_destroy(projet->list_gtk.ef_materiaux.window);
    
    return;
}


void EF_gtk_materiaux_window_destroy(GtkWidget *object, Projet *projet)
/* Description : met projet->list_gtk.ef_materiaux.builder à NULL quand la fenêtre se ferme,
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet")
    BUGMSG(projet->list_gtk.ef_materiaux.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Matériau")
    
    g_object_unref(G_OBJECT(projet->list_gtk.ef_materiaux.builder));
    projet->list_gtk.ef_materiaux.builder = NULL;
    
    return;
}


gboolean EF_gtk_materiaux_window_key_press(GtkWidget *widget, GdkEvent *event, Projet *projet)
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
    BUGMSG(projet, TRUE, gettext("Paramètre %s incorrect.\n"), "projet")
    BUGMSG(projet->list_gtk.ef_materiaux.builder, TRUE, gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Matériau")
    
    if (event->key.keyval == GDK_KEY_Escape)
    {
        gtk_widget_destroy(projet->list_gtk.ef_materiaux.window);
        return TRUE;
    }
    else
        return FALSE;
}


gboolean EF_gtk_materiaux_treeview_key_press(GtkTreeView *treeview, GdkEvent *event,
  Projet *projet)
/* Description : Supprime un matériau sans dépendance si la touche SUPPR est appuyée.
 * Paramètres : GtkTreeView *treeview : composant à l'origine de l'évènement,
 *            : GdkEvent *event : Caractéristique de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : TRUE si la touche SUPPR est pressée, FALSE sinon.
 *   Echec : projet == NULL,
 *           interface graphique non initialisée.
 *  
 */
{
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet")
    BUGMSG(projet->list_gtk.ef_materiaux.builder, FALSE, gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Matériau")
    
    if (event->key.keyval == GDK_KEY_Delete)
    {
        GtkTreeIter     Iter;
        GtkTreeModel    *model;
        EF_Materiau     *materiau;
        GList           *liste_materiaux = NULL;
        
        if (!gtk_tree_selection_get_selected(gtk_tree_view_get_selection(treeview), &model, &Iter))
            return FALSE;
        
        gtk_tree_model_get(model, &Iter, 0, &materiau, -1);
        
        liste_materiaux = g_list_append(liste_materiaux, materiau);
        if (_1992_1_1_barres_cherche_dependances(projet, NULL, NULL, NULL, liste_materiaux, NULL, NULL, NULL, NULL, NULL, FALSE, FALSE) == FALSE)
            BUG(EF_materiaux_supprime(materiau, projet), FALSE)
        
        g_list_free(liste_materiaux);
        
        return TRUE;
    }
    else
        return FALSE;
}


void EF_gtk_materiaux_edit_nom(GtkCellRendererText *cell, gchar *path_string, gchar *new_text,
  Projet *projet)
/* Description : Modification du nom d'un matériau.
 * Paramètres : GtkCellRendererText *cell : cellule en cours,
 *            : gchar *path_string : path de la ligne en cours,
 *            : gchar *new_text : le nouveau nom,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
*/
{
    Gtk_EF_Materiaux    *ef_gtk;
    GtkTreeModel        *model;
    GtkTreeIter         iter;
    GtkTreePath         *path;
    EF_Materiau         *materiau;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet")
    BUGMSG(projet->list_gtk.ef_materiaux.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Matériau")
    
    ef_gtk = &projet->list_gtk.ef_materiaux;
    model = GTK_TREE_MODEL(ef_gtk->materiaux);
    path = gtk_tree_path_new_from_string(path_string);
    gtk_tree_model_get_iter(model, &iter, path);
    gtk_tree_path_free(path);
    gtk_tree_model_get(model, &iter, 0, &materiau, -1);
    if ((strcmp(materiau->nom, new_text) == 0) || (strcmp(new_text, "") == 0))
        return;
    
    if (EF_materiaux_cherche_nom(projet, new_text, FALSE))
        return;
    
    switch (materiau->type)
    {
        case MATERIAU_BETON :
        {
            BUG(_1992_1_1_materiaux_modif(projet, materiau, new_text, common_math_f(NAN, FLOTTANT_ORDINATEUR), common_math_f(NAN, FLOTTANT_ORDINATEUR), common_math_f(NAN, FLOTTANT_ORDINATEUR), common_math_f(NAN, FLOTTANT_ORDINATEUR), common_math_f(NAN, FLOTTANT_ORDINATEUR), common_math_f(NAN, FLOTTANT_ORDINATEUR), common_math_f(NAN, FLOTTANT_ORDINATEUR), common_math_f(NAN, FLOTTANT_ORDINATEUR), common_math_f(NAN, FLOTTANT_ORDINATEUR), common_math_f(NAN, FLOTTANT_ORDINATEUR), common_math_f(NAN, FLOTTANT_ORDINATEUR), common_math_f(NAN, FLOTTANT_ORDINATEUR), common_math_f(NAN, FLOTTANT_ORDINATEUR), common_math_f(NAN, FLOTTANT_ORDINATEUR), common_math_f(NAN, FLOTTANT_ORDINATEUR)), )
            break;
        }
        case MATERIAU_ACIER :
        {
            BUG(_1993_1_1_materiaux_modif(projet, materiau, new_text, common_math_f(NAN, FLOTTANT_ORDINATEUR), common_math_f(NAN, FLOTTANT_ORDINATEUR), common_math_f(NAN, FLOTTANT_ORDINATEUR), common_math_f(NAN, FLOTTANT_ORDINATEUR)), )
            break;
        }
        default :
        {
            BUGMSG(NULL, , gettext("Le type de matériau %d n'existe pas.\n"), materiau->type)
            break;
        }
    }
    
    return;
}


void EF_gtk_materiaux_select_changed(GtkTreeSelection *treeselection, Projet *projet)
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
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet")
    BUGMSG(projet->list_gtk.ef_materiaux.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Matériau")
    
    // Si aucune section n'est sélectionnée, il n'est pas possible d'en supprimer ou d'en éditer une.
    if (!gtk_tree_selection_get_selected(GTK_TREE_SELECTION(gtk_builder_get_object(projet->list_gtk.ef_materiaux.builder, "EF_materiaux_treeview_select")), &model, &Iter))
    {
        gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_materiaux.builder, "EF_materiaux_boutton_supprimer_direct")), FALSE);
        gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_materiaux.builder, "EF_materiaux_boutton_supprimer_menu")), FALSE);
        gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_materiaux.builder, "EF_materiaux_boutton_modifier")), FALSE);
        gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_materiaux.builder, "EF_materiaux_boutton_supprimer_direct")), TRUE);
        gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_materiaux.builder, "EF_materiaux_boutton_supprimer_menu")), FALSE);
    }
    else
    {
        EF_Materiau *materiau;
        
        GList       *liste_materiaux = NULL;
        
        gtk_tree_model_get(model, &Iter, 0, &materiau, -1);
        
        gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_materiaux.builder, "EF_materiaux_boutton_modifier")), TRUE);
        
        liste_materiaux = g_list_append(liste_materiaux, materiau);
        if (_1992_1_1_barres_cherche_dependances(projet, NULL, NULL, NULL, liste_materiaux, NULL, NULL, NULL, NULL, NULL, FALSE, FALSE))
        {
            gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_materiaux.builder, "EF_materiaux_boutton_supprimer_direct")), FALSE);
            gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_materiaux.builder, "EF_materiaux_boutton_supprimer_menu")), TRUE);
            gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_materiaux.builder, "EF_materiaux_boutton_supprimer_direct")), FALSE);
            gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_materiaux.builder, "EF_materiaux_boutton_supprimer_menu")), TRUE);
        }
        else
        {
            gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_materiaux.builder, "EF_materiaux_boutton_supprimer_direct")), TRUE);
            gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_materiaux.builder, "EF_materiaux_boutton_supprimer_menu")), FALSE);
            gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_materiaux.builder, "EF_materiaux_boutton_supprimer_direct")), TRUE);
            gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_materiaux.builder, "EF_materiaux_boutton_supprimer_menu")), FALSE);
        }
        
        g_list_free(liste_materiaux);
    }
    
    return;
}


void EF_gtk_materiaux_boutton_supprimer_menu(GtkButton *widget, Projet *projet)
/* Description : Affiche la liste des dépendances dans le menu lorsqu'on clique sur le bouton
 * Paramètres : GtkButton *widget : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 *   Echec : projet == NULL,
 *           interface graphique non initialisée.
 */
{
    GtkTreeModel    *model;
    GtkTreeIter     Iter;
    EF_Materiau     *materiau;
    GList           *liste_materiaux = NULL, *liste_noeuds_dep, *liste_barres_dep, *liste_charges_dep;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet")
    BUGMSG(projet->list_gtk.ef_materiaux.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Matériau")
    
    // Si aucune section n'est sélectionnée, il n'est pas possible d'en supprimer ou d'en
    // éditer une.
    if (!gtk_tree_selection_get_selected(GTK_TREE_SELECTION(gtk_builder_get_object(projet->list_gtk.ef_materiaux.builder, "EF_materiaux_treeview_select")), &model, &Iter))
        BUGMSG(NULL, , gettext("Aucun élément n'est sélectionné.\n"))
    
    gtk_tree_model_get(model, &Iter, 0, &materiau, -1);
    
    liste_materiaux = g_list_append(liste_materiaux, materiau);
    BUG(_1992_1_1_barres_cherche_dependances(projet, NULL, NULL, NULL, liste_materiaux, NULL, NULL, &liste_noeuds_dep, &liste_barres_dep, &liste_charges_dep, FALSE, FALSE), )
    g_list_free(liste_materiaux);
    
    if ((liste_noeuds_dep != NULL) || (liste_barres_dep != NULL) || (liste_charges_dep != NULL))
    {
        char    *desc;
        
        desc = common_text_dependances(liste_noeuds_dep, liste_barres_dep, liste_charges_dep, projet);
        gtk_menu_item_set_label(GTK_MENU_ITEM(gtk_builder_get_object(projet->list_gtk.ef_materiaux.builder, "EF_materiaux_supprimer_menu_barres")), desc);
        free(desc);
    }
    else
        BUGMSG(NULL, , gettext("L'élément ne possède aucune dépendance.\n"))
    
    g_list_free(liste_noeuds_dep);
    g_list_free(liste_barres_dep);
    g_list_free(liste_charges_dep);
    
    return;
}


void EF_gtk_materiaux_supprimer_menu_barres(GtkButton *button, Projet *projet)
/* Description : Supprime le matériau sélectionné dans le treeview, y compris les barres
 *               l'utilisant.
 * Paramètres : GtkWidget *widget : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    GtkTreeIter     iter;
    GtkTreeModel    *model;
    EF_Materiau     *materiau;
    GList           *liste_materiaux = NULL, *liste_barres_dep;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet")
    BUGMSG(projet->list_gtk.ef_materiaux.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Matériau")
    
    if (!gtk_tree_selection_get_selected(GTK_TREE_SELECTION(gtk_builder_get_object(projet->list_gtk.ef_materiaux.builder, "EF_materiaux_treeview_select")), &model, &iter))
        return;
    
    gtk_tree_model_get(model, &iter, 0, &materiau, -1);
    
    liste_materiaux = g_list_append(liste_materiaux, materiau);
    BUG(_1992_1_1_barres_cherche_dependances(projet, NULL, NULL, NULL, liste_materiaux, NULL, NULL, NULL, &liste_barres_dep, NULL, FALSE, FALSE), )
    g_list_free(liste_materiaux);
    BUG(_1992_1_1_barres_supprime_liste(projet, NULL, liste_barres_dep), )
    g_list_free(liste_barres_dep);
    BUG(EF_materiaux_supprime(materiau, projet), )
    
    BUG(m3d_rafraichit(projet), )
    
    return;
}


void EF_gtk_materiaux_supprimer_direct(GtkButton *button, Projet *projet)
/* Description : Supprime le matériau sélectionné dans le treeview.
 * Paramètres : GtkWidget *widget : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    GtkTreeIter     iter;
    GtkTreeModel    *model;
    EF_Materiau     *materiau;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet")
    BUGMSG(projet->list_gtk.ef_materiaux.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Matériau")
    
    if (!gtk_tree_selection_get_selected(GTK_TREE_SELECTION(gtk_builder_get_object(projet->list_gtk.ef_materiaux.builder, "EF_materiaux_treeview_select")), &model, &iter))
        return;
    
    gtk_tree_model_get(model, &iter, 0, &materiau, -1);
    
    BUG(EF_materiaux_supprime(materiau, projet), )
    
    return;
}


void EF_gtk_materiaux_render_0(GtkTreeViewColumn *tree_column, GtkCellRenderer *cell,
  GtkTreeModel *tree_model, GtkTreeIter *iter, gpointer data2)
/* Description : Affiche la distance vy de la section.
 * Paramètres : GtkTreeViewColumn *tree_column : composant à l'origine de l'évènement,
 *            : GtkCellRenderer *cell : la cellule en cours d'édition,
 *            : GtkTreeModel *tree_model : le mode en cours d'édition,
 *            : GtkTreeIter *iter : la ligne en cours d'édition,
 *            : gpointer data2 : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    EF_Materiau *materiau;
    
    gtk_tree_model_get(tree_model, iter, 0, &materiau, -1);
    
    g_object_set(cell, "text", materiau->nom, NULL);
    
    return;
}


void EF_gtk_materiaux_render_1(GtkTreeViewColumn *tree_column, GtkCellRenderer *cell,
  GtkTreeModel *tree_model, GtkTreeIter *iter, gpointer data2)
/* Description : Affiche la distance vy de la section.
 * Paramètres : GtkTreeViewColumn *tree_column : composant à l'origine de l'évènement,
 *            : GtkCellRenderer *cell : la cellule en cours d'édition,
 *            : GtkTreeModel *tree_model : le mode en cours d'édition,
 *            : GtkTreeIter *iter : la ligne en cours d'édition,
 *            : gpointer data2 : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    EF_Materiau *materiau;
    
    gtk_tree_model_get(tree_model, iter, 0, &materiau, -1);
    
    switch (materiau->type)
    {
        case MATERIAU_BETON :
        {
            g_object_set(cell, "text", gettext("Béton"), NULL);
            break;
        }
        case MATERIAU_ACIER :
        {
            g_object_set(cell, "text", gettext("Acier"), NULL);
            break;
        }
        default :
        {
            g_object_set(cell, "text", gettext("Inconnu"), NULL);
            break;
        }
    }
    
    return;
}


void EF_gtk_materiaux_render_2(GtkTreeViewColumn *tree_column, GtkCellRenderer *cell,
  GtkTreeModel *tree_model, GtkTreeIter *iter, gpointer data2)
/* Description : Affiche la distance vy de la section.
 * Paramètres : GtkTreeViewColumn *tree_column : composant à l'origine de l'évènement,
 *            : GtkCellRenderer *cell : la cellule en cours d'édition,
 *            : GtkTreeModel *tree_model : le mode en cours d'édition,
 *            : GtkTreeIter *iter : la ligne en cours d'édition,
 *            : gpointer data2 : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    EF_Materiau *materiau;
    char        *c;
    
    gtk_tree_model_get(tree_model, iter, 0, &materiau, -1);
    
    switch (materiau->type)
    {
        case MATERIAU_BETON :
        case MATERIAU_ACIER :
        {
            c = EF_materiaux_get_description(materiau);
            g_object_set(cell, "markup", c, NULL);
            free(c);
            break;
        }
        default :
        {
            g_object_set(cell, "markup", gettext("Inconnu"), NULL);
            break;
        }
    }
    
    return;
}


void EF_gtk_materiaux(Projet *projet)
/* Description : Création de la fenêtre permettant d'afficher les matériaux sous forme d'un
 *               tableau.
 * Paramètres : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 *   Echec : projet == NULL,
 *           interface graphique impossible à générer.
 */
{
    Gtk_EF_Materiaux *ef_gtk;
    GList            *list_parcours;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet")
    if (projet->list_gtk.ef_materiaux.builder != NULL)
    {
        gtk_window_present(GTK_WINDOW(projet->list_gtk.ef_materiaux.window));
        return;
    }
    
    ef_gtk = &projet->list_gtk.ef_materiaux;
    
    ef_gtk->builder = gtk_builder_new();
    BUGMSG(gtk_builder_add_from_resource(ef_gtk->builder, "/org/2lgc/codegui/ui/EF_materiaux.ui", NULL) != 0, , gettext("Builder Failed\n"))
    gtk_builder_connect_signals(ef_gtk->builder, projet);
    
    ef_gtk->window = GTK_WIDGET(gtk_builder_get_object(ef_gtk->builder, "EF_materiaux_window"));
    ef_gtk->materiaux = GTK_TREE_STORE(gtk_builder_get_object(ef_gtk->builder, "EF_materiaux_treestore"));
    
    gtk_tree_view_column_set_cell_data_func(GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(ef_gtk->builder, "EF_materiaux_treeview_column0")), GTK_CELL_RENDERER(gtk_builder_get_object(ef_gtk->builder, "EF_materiaux_treeview_cell0")), EF_gtk_materiaux_render_0, projet, NULL);
    gtk_tree_view_column_set_cell_data_func(GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(ef_gtk->builder, "EF_materiaux_treeview_column1")), GTK_CELL_RENDERER(gtk_builder_get_object(ef_gtk->builder, "EF_materiaux_treeview_cell1")), EF_gtk_materiaux_render_1, projet, NULL);
    gtk_tree_view_column_set_cell_data_func(GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(ef_gtk->builder, "EF_materiaux_treeview_column2")), GTK_CELL_RENDERER(gtk_builder_get_object(ef_gtk->builder, "EF_materiaux_treeview_cell2")), EF_gtk_materiaux_render_2, projet, NULL);
    
    list_parcours = projet->modele.materiaux;
    while (list_parcours != NULL)
    {
        EF_Materiau *materiau = list_parcours->data;
        
        gtk_tree_store_append(ef_gtk->materiaux, &materiau->Iter_fenetre, NULL);
        gtk_tree_store_set(ef_gtk->materiaux, &materiau->Iter_fenetre, 0, materiau, -1);
        
        list_parcours = g_list_next(list_parcours);
    }
    
    gtk_window_set_transient_for(GTK_WINDOW(ef_gtk->window), GTK_WINDOW(projet->list_gtk.comp.window));
}

#endif
