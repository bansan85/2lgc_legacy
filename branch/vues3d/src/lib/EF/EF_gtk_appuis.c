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
#include "common_selection.h"
#include "common_text.h"
#include "EF_appuis.h"
#include "1992_1_1_barres.h"

void EF_gtk_appuis_fermer(GtkButton *button, Projet *projet)
/* Description : Ferme la fenêtre.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_appuis.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Appui");
    
    gtk_widget_destroy(projet->list_gtk.ef_appuis.window);
    
    return;
}


void EF_gtk_appuis_window_destroy(GtkWidget *object, Projet *projet)
/* Description : met projet->list_gtk.ef_appuis.builder à NULL quand la fenêtre se ferme,
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_appuis.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Appui");
    
    g_object_unref(G_OBJECT(projet->list_gtk.ef_appuis.builder));
    projet->list_gtk.ef_appuis.builder = NULL;
    
    return;
}


gboolean EF_gtk_appuis_window_key_press(GtkWidget *widget, GdkEvent *event, Projet *projet)
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
    BUGMSG(projet->list_gtk.ef_appuis.builder, TRUE, gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Appui");
    
    if (event->key.keyval == GDK_KEY_Escape)
    {
        gtk_widget_destroy(projet->list_gtk.ef_appuis.window);
        return TRUE;
    }
    else
        return FALSE;
}


void EF_gtk_appuis_ajouter(GtkButton *button, Projet *projet)
/* Description : Ajout un nouvel appui.
 * Paramètres : GtkWidget *widget : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    EF_Appui    *appui;
    GtkTreePath *path;
    GtkTreeModel *model;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_appuis.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Appui");
    
    if (EF_appuis_cherche_nom(projet, gettext("Sans nom"), FALSE) == NULL)
        BUG(appui = EF_appuis_ajout(projet, gettext("Sans nom"), EF_APPUI_LIBRE, EF_APPUI_LIBRE, EF_APPUI_LIBRE, EF_APPUI_LIBRE, EF_APPUI_LIBRE, EF_APPUI_LIBRE), );
    else
    {
        char    *nom;
        int     i = 2;
        
        nom = g_strdup_printf("%s (%d)", gettext("Sans nom"), i);
        while (EF_appuis_cherche_nom(projet, nom, FALSE) != NULL)
        {
            i++;
            free(nom);
            nom = g_strdup_printf("%s (%d)", gettext("Sans nom"), i);
        }
        BUG(appui = EF_appuis_ajout(projet, nom, EF_APPUI_LIBRE, EF_APPUI_LIBRE, EF_APPUI_LIBRE, EF_APPUI_LIBRE, EF_APPUI_LIBRE, EF_APPUI_LIBRE), );
        free(nom);
    }
    
    model = GTK_TREE_MODEL(gtk_builder_get_object(projet->list_gtk.ef_appuis.builder, "EF_appuis_treestore"));
    path = gtk_tree_model_get_path(model, &appui->Iter_fenetre);
    gtk_tree_view_set_cursor(GTK_TREE_VIEW(gtk_builder_get_object(projet->list_gtk.ef_appuis.builder, "EF_appuis_treeview")), path, gtk_tree_view_get_column(GTK_TREE_VIEW(gtk_builder_get_object(projet->list_gtk.ef_appuis.builder, "EF_appuis_treeview")), 0), TRUE);
    gtk_tree_path_free(path);
}


void EF_gtk_appuis_supprimer(GtkButton *button, Projet *projet)
/* Description : Supprime l'appui sélectionné dans le treeview.
 * Paramètres : GtkWidget *widget : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    GtkTreeIter     iter;
    GtkTreeModel    *model;
    EF_Appui        *appui;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_appuis.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Appui");
    
    if (!gtk_tree_selection_get_selected(GTK_TREE_SELECTION(gtk_builder_get_object(projet->list_gtk.ef_appuis.builder, "EF_appuis_treeview_select")), &model, &iter))
        return;
    
    gtk_tree_model_get(model, &iter, 0, &appui, -1);
    
    BUG(EF_appuis_supprime(appui, TRUE, FALSE, projet), );
    
    BUG(m3d_rafraichit(projet), );
    
    return;
}


gboolean EF_gtk_appuis_treeview_key_press(GtkWidget *widget, GdkEvent *event, Projet *projet)
/* Description : Supprime un appui sans dépendance si la touche SUPPR est appuyée.
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
    BUGMSG(projet->list_gtk.ef_appuis.builder, TRUE, gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Appui");
    
    if (event->key.keyval == GDK_KEY_Delete)
    {
        GtkTreeIter     Iter;
        GtkTreeModel    *model;
        
        if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION(gtk_builder_get_object(projet->list_gtk.ef_appuis.builder, "EF_appuis_treeview_select")), &model, &Iter))
        {
            EF_Appui    *appui;
            
            GList   *liste_appuis = NULL;
            
            gtk_tree_model_get(model, &Iter, 0, &appui, -1);
            
            liste_appuis = g_list_append(liste_appuis, appui);
            if (_1992_1_1_barres_cherche_dependances(projet, liste_appuis, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, FALSE, FALSE) == FALSE)
                EF_gtk_appuis_supprimer(NULL, projet);
            
            g_list_free(liste_appuis);
        }
        return TRUE;
    }
    else
        return FALSE;
}


void EF_gtk_appuis_supprimer_menu_suppr_noeud(GtkButton *button, Projet *projet)
/* Description : Supprime l'appui sélectionné dans le treeview et ainsi que les noeuds
 *               utilisant l'appui.
 * Paramètres : GtkWidget *widget : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    GtkTreeIter     iter;
    GtkTreeModel    *model;
    EF_Appui        *appui;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_appuis.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Appui");
    
    if (!gtk_tree_selection_get_selected(GTK_TREE_SELECTION(gtk_builder_get_object(projet->list_gtk.ef_appuis.builder, "EF_appuis_treeview_select")), &model, &iter))
        return;
    
    gtk_tree_model_get(model, &iter, 0, &appui, -1);
    
    BUG(EF_appuis_supprime(appui, FALSE, TRUE, projet), );
    
    BUG(m3d_rafraichit(projet), );
    
    return;
}


void EF_gtk_appuis_supprimer_menu_modif_noeud(GtkButton *button, Projet *projet)
/* Description : Supprime l'appui sélectionné dans le treeview et les noeuds le possédant
 *               deviennent sans appui.
 * Paramètres : GtkWidget *widget : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    GtkTreeIter     iter;
    GtkTreeModel    *model;
    EF_Appui        *appui;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_appuis.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Appui");
    
    if (!gtk_tree_selection_get_selected(GTK_TREE_SELECTION(gtk_builder_get_object(projet->list_gtk.ef_appuis.builder, "EF_appuis_treeview_select")), &model, &iter))
        return;
    
    gtk_tree_model_get(model, &iter, 0, &appui, -1);
    
    BUG(EF_appuis_supprime(appui, FALSE, FALSE, projet), );
    
    return;
}


void EF_gtk_appuis_edit_type(GtkCellRendererText *cell, gchar *path_string, gchar *new_text,
  Projet *projet)
/* Description : Modification d'un appui.
 * Paramètres : GtkCellRendererText *cell : cellule en cours,
 *            : gchar *path_string : path de la ligne en cours,
 *            : gchar *new_text : nouvelle valeur,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
*/
{
    Gtk_EF_Appuis   *ef_gtk;
    GtkTreeModel    *model;
    GtkTreeIter     iter;
    GtkTreePath     *path;
    EF_Appui        *appui;
    int             column = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(cell), "column"));
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_appuis.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Appui");
    
    ef_gtk = &projet->list_gtk.ef_appuis;
    model = GTK_TREE_MODEL(ef_gtk->appuis);
    path = gtk_tree_path_new_from_string(path_string);
    gtk_tree_model_get_iter(model, &iter, path);
    gtk_tree_path_free(path);
    gtk_tree_model_get(model, &iter, 0, &appui, -1);

    if (strcmp(new_text, gettext("Libre")) == 0)
        BUG(EF_appuis_edit(appui, column-1, EF_APPUI_LIBRE, projet), );
    else if (strcmp(new_text, gettext("Bloqué")) == 0)
        BUG(EF_appuis_edit(appui, column-1, EF_APPUI_BLOQUE, projet), );
    else
        BUGMSG(NULL, , gettext("Type d'appui %s inconnu.\n"), new_text);
    
    return;
}


void EF_gtk_appuis_edit_nom(GtkCellRendererText *cell, gchar *path_string, gchar *new_text,
  Projet *projet)
/* Description : Modification du nom d'un appui.
 * Paramètres : GtkCellRendererText *cell : cellule en cours,
 *            : gchar *path_string : path de la ligne en cours,
 *            : gchar *new_text : le nouveau nom,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
*/
{
    Gtk_EF_Appuis   *ef_gtk;
    GtkTreeModel    *model;
    GtkTreeIter     iter;
    GtkTreePath     *path;
    EF_Appui        *appui;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_appuis.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Appui");
    
    ef_gtk = &projet->list_gtk.ef_appuis;
    model = GTK_TREE_MODEL(ef_gtk->appuis);
    path = gtk_tree_path_new_from_string(path_string);
    gtk_tree_model_get_iter(model, &iter, path);
    gtk_tree_path_free(path);
    gtk_tree_model_get(model, &iter, 0, &appui, -1);
    if (strcmp(appui->nom, new_text) == 0)
        return;

    if (EF_appuis_renomme(appui, new_text, projet, FALSE))
        return;
    
    return;
}


void EF_gtk_appuis_select_changed(GtkTreeSelection *treeselection, Projet *projet)
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
    BUGMSG(projet->list_gtk.ef_appuis.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Appui");
    
    // Si aucun appui n'est sélectionné, il n'est pas possible d'en supprimer un.
    if (!gtk_tree_selection_get_selected(GTK_TREE_SELECTION(gtk_builder_get_object(projet->list_gtk.ef_appuis.builder, "EF_appuis_treeview_select")), &model, &Iter))
    {
        gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_appuis.builder, "EF_appuis_boutton_supprimer_direct")), FALSE);
        gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_appuis.builder, "EF_appuis_boutton_supprimer_menu")), FALSE);
        gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_appuis.builder, "EF_appuis_boutton_supprimer_direct")), FALSE);
        gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_appuis.builder, "EF_appuis_boutton_supprimer_menu")), TRUE);
    }
    else
    {
        EF_Appui    *appui;
        GList       *liste_appuis = NULL;
        
        gtk_tree_model_get(model, &Iter, 0, &appui, -1);
        
        liste_appuis = g_list_append(liste_appuis, appui);
        if (_1992_1_1_barres_cherche_dependances(projet, liste_appuis, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, FALSE, FALSE))
        {
            gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_appuis.builder, "EF_appuis_boutton_supprimer_direct")), FALSE);
            gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_appuis.builder, "EF_appuis_boutton_supprimer_menu")), TRUE);
            gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_appuis.builder, "EF_appuis_boutton_supprimer_direct")), FALSE);
            gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_appuis.builder, "EF_appuis_boutton_supprimer_menu")), TRUE);
        }
        else
        {
            gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_appuis.builder, "EF_appuis_boutton_supprimer_direct")), TRUE);
            gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_appuis.builder, "EF_appuis_boutton_supprimer_menu")), FALSE);
            gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_appuis.builder, "EF_appuis_boutton_supprimer_direct")), TRUE);
            gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_appuis.builder, "EF_appuis_boutton_supprimer_menu")), FALSE);
        }
        
        g_list_free(liste_appuis);
    }
    
    return;
}


void EF_gtk_appuis_boutton_supprimer_menu(GtkButton *widget, Projet *projet)
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
    EF_Appui        *appui;
    GList           *liste_appuis = NULL, *liste_noeuds_dep, *liste_barres_dep, *liste_charges_dep;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_appuis.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Appui");
    
    if (!gtk_tree_selection_get_selected(GTK_TREE_SELECTION(gtk_builder_get_object(projet->list_gtk.ef_appuis.builder, "EF_appuis_treeview_select")), &model, &Iter))
        BUGMSG(NULL, , gettext("Aucun élément n'est sélectionné.\n"));
    
    gtk_tree_model_get(model, &Iter, 0, &appui, -1);
    
    liste_appuis = g_list_append(liste_appuis, appui);
    BUG(_1992_1_1_barres_cherche_dependances(projet, liste_appuis, NULL, NULL, NULL, NULL, NULL, &liste_noeuds_dep, &liste_barres_dep, &liste_charges_dep, FALSE, FALSE), );
    g_list_free(liste_appuis);
    
    if ((liste_noeuds_dep != NULL) || (liste_barres_dep != NULL) || (liste_charges_dep != NULL))
    {
        char    *desc;
        
        desc = common_text_dependances(liste_noeuds_dep, liste_barres_dep, liste_charges_dep, projet);
        gtk_menu_item_set_label(GTK_MENU_ITEM(gtk_builder_get_object(projet->list_gtk.ef_appuis.builder, "EF_appuis_supprimer_menu_suppr_noeud")), desc);
        free(desc);
    }
    else
        BUGMSG(NULL, , gettext("L'élément ne possède aucune dépendance.\n"));
    
    g_list_free(liste_noeuds_dep);
    g_list_free(liste_barres_dep);
    g_list_free(liste_charges_dep);
    
    return;
}


void EF_gtk_appuis_render_0(GtkTreeViewColumn *tree_column, GtkCellRenderer *cell,
  GtkTreeModel *tree_model, GtkTreeIter *iter, gpointer data2)
/* Description : Affiche le nom de l'appui.
 * Paramètres : GtkTreeViewColumn *tree_column : composant à l'origine de l'évènement,
 *            : GtkCellRenderer *cell : la cellule en cours d'édition,
 *            : GtkTreeModel *tree_model : le mode en cours d'édition,
 *            : GtkTreeIter *iter : la ligne en cours d'édition,
 *            : gpointer data2 : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    EF_Appui    *appui;
    
    gtk_tree_model_get(tree_model, iter, 0, &appui, -1);
    
    g_object_set(cell, "text", appui->nom, NULL);
}


void EF_gtk_appuis_render_1(GtkTreeViewColumn *tree_column, GtkCellRenderer *cell,
  GtkTreeModel *tree_model, GtkTreeIter *iter, gpointer data2)
/* Description : Affiche le type d'appui en ux.
 * Paramètres : GtkTreeViewColumn *tree_column : composant à l'origine de l'évènement,
 *            : GtkCellRenderer *cell : la cellule en cours d'édition,
 *            : GtkTreeModel *tree_model : le mode en cours d'édition,
 *            : GtkTreeIter *iter : la ligne en cours d'édition,
 *            : gpointer data2 : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    EF_Appui    *appui;
    char        *txt;
    
    gtk_tree_model_get(tree_model, iter, 0, &appui, -1);
    
    switch (appui->ux)
    {
        case EF_APPUI_LIBRE :
        {
            txt = gettext("Libre");
            BUGMSG(appui->ux_donnees == NULL, , gettext("Le type d'appui de %s (%s) n'a pas à posséder de données.\n"), "ux", gettext("Libre"));
            break;
        }
        case EF_APPUI_BLOQUE :
        {
            txt = gettext("Bloqué");
            BUGMSG(appui->ux_donnees == NULL, , gettext("Le type d'appui de %s (%s) n'a pas à posséder de données.\n"), "ux", gettext("Bloqué"));
            break;
        }
        default :
        {
            BUGMSG(NULL, , gettext("Le type d'appui de %s (%d) est inconnu.\n"), "ux", appui->ux);
        }
    }
    
    g_object_set(cell, "text", txt, NULL);
}


void EF_gtk_appuis_render_2(GtkTreeViewColumn *tree_column, GtkCellRenderer *cell,
  GtkTreeModel *tree_model, GtkTreeIter *iter, gpointer data2)
/* Description : Affiche le type d'appui en uy.
 * Paramètres : GtkTreeViewColumn *tree_column : composant à l'origine de l'évènement,
 *            : GtkCellRenderer *cell : la cellule en cours d'édition,
 *            : GtkTreeModel *tree_model : le mode en cours d'édition,
 *            : GtkTreeIter *iter : la ligne en cours d'édition,
 *            : gpointer data2 : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    EF_Appui    *appui;
    char        *txt;
    
    gtk_tree_model_get(tree_model, iter, 0, &appui, -1);
    
    switch (appui->uy)
    {
        case EF_APPUI_LIBRE :
        {
            txt = gettext("Libre");
            BUGMSG(appui->uy_donnees == NULL, , gettext("Le type d'appui de %s (%s) n'a pas à posséder de données.\n"), "uy", gettext("Libre"));
            break;
        }
        case EF_APPUI_BLOQUE :
        {
            txt = gettext("Bloqué");
            BUGMSG(appui->uy_donnees == NULL, , gettext("Le type d'appui de %s (%s) n'a pas à posséder de données.\n"), "uy", gettext("Bloqué"));
            break;
        }
        default :
        {
            BUGMSG(NULL, , gettext("Le type d'appui de %s (%d) est inconnu.\n"), "ux", appui->uy);
        }
    }
    
    g_object_set(cell, "text", txt, NULL);
}


void EF_gtk_appuis_render_3(GtkTreeViewColumn *tree_column, GtkCellRenderer *cell,
  GtkTreeModel *tree_model, GtkTreeIter *iter, gpointer data2)
/* Description : Affiche le type d'appui en uz.
 * Paramètres : GtkTreeViewColumn *tree_column : composant à l'origine de l'évènement,
 *            : GtkCellRenderer *cell : la cellule en cours d'édition,
 *            : GtkTreeModel *tree_model : le mode en cours d'édition,
 *            : GtkTreeIter *iter : la ligne en cours d'édition,
 *            : gpointer data2 : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    EF_Appui    *appui;
    char        *txt;
    
    gtk_tree_model_get(tree_model, iter, 0, &appui, -1);
    
    switch (appui->uz)
    {
        case EF_APPUI_LIBRE :
        {
            txt = gettext("Libre");
            BUGMSG(appui->uz_donnees == NULL, , gettext("Le type d'appui de %s (%s) n'a pas à posséder de données.\n"), "uz", gettext("Libre"));
            break;
        }
        case EF_APPUI_BLOQUE :
        {
            txt = gettext("Bloqué");
            BUGMSG(appui->uz_donnees == NULL, , gettext("Le type d'appui de %s (%s) n'a pas à posséder de données.\n"), "uz", gettext("Bloqué"));
            break;
        }
        default :
        {
            BUGMSG(NULL, , gettext("Le type d'appui de %s (%d) est inconnu.\n"), "ux", appui->uz);
        }
    }
    
    g_object_set(cell, "text", txt, NULL);
}


void EF_gtk_appuis_render_4(GtkTreeViewColumn *tree_column, GtkCellRenderer *cell,
  GtkTreeModel *tree_model, GtkTreeIter *iter, gpointer data2)
/* Description : Affiche le type d'appui en rx.
 * Paramètres : GtkTreeViewColumn *tree_column : composant à l'origine de l'évènement,
 *            : GtkCellRenderer *cell : la cellule en cours d'édition,
 *            : GtkTreeModel *tree_model : le mode en cours d'édition,
 *            : GtkTreeIter *iter : la ligne en cours d'édition,
 *            : gpointer data2 : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    EF_Appui    *appui;
    char        *txt;
    
    gtk_tree_model_get(tree_model, iter, 0, &appui, -1);
    
    switch (appui->rx)
    {
        case EF_APPUI_LIBRE :
        {
            txt = gettext("Libre");
            BUGMSG(appui->rx_donnees == NULL, , gettext("Le type d'appui de %s (%s) n'a pas à posséder de données.\n"), "rx", gettext("Libre"));
            break;
        }
        case EF_APPUI_BLOQUE :
        {
            txt = gettext("Bloqué");
            BUGMSG(appui->rx_donnees == NULL, , gettext("Le type d'appui de %s (%s) n'a pas à posséder de données.\n"), "rx", gettext("Bloqué"));
            break;
        }
        default :
        {
            BUGMSG(NULL, , gettext("Le type d'appui de %s (%d) est inconnu.\n"), "rx", appui->rx);
        }
    }
    
    g_object_set(cell, "text", txt, NULL);
}


void EF_gtk_appuis_render_5(GtkTreeViewColumn *tree_column, GtkCellRenderer *cell,
  GtkTreeModel *tree_model, GtkTreeIter *iter, gpointer data2)
/* Description : Affiche le type d'appui en ry.
 * Paramètres : GtkTreeViewColumn *tree_column : composant à l'origine de l'évènement,
 *            : GtkCellRenderer *cell : la cellule en cours d'édition,
 *            : GtkTreeModel *tree_model : le mode en cours d'édition,
 *            : GtkTreeIter *iter : la ligne en cours d'édition,
 *            : gpointer data2 : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    EF_Appui    *appui;
    char        *txt;
    
    gtk_tree_model_get(tree_model, iter, 0, &appui, -1);
    
    switch (appui->ry)
    {
        case EF_APPUI_LIBRE :
        {
            txt = gettext("Libre");
            BUGMSG(appui->ry_donnees == NULL, , gettext("Le type d'appui de %s (%s) n'a pas à posséder de données.\n"), "ry", gettext("Libre"));
            break;
        }
        case EF_APPUI_BLOQUE :
        {
            txt = gettext("Bloqué");
            BUGMSG(appui->ry_donnees == NULL, , gettext("Le type d'appui de %s (%s) n'a pas à posséder de données.\n"), "ry", gettext("Bloqué"));
            break;
        }
        default :
        {
            BUGMSG(NULL, , gettext("Le type d'appui de %s (%d) est inconnu.\n"), "ry", appui->ry);
        }
    }
    
    g_object_set(cell, "text", txt, NULL);
}


void EF_gtk_appuis_render_6(GtkTreeViewColumn *tree_column, GtkCellRenderer *cell,
  GtkTreeModel *tree_model, GtkTreeIter *iter, gpointer data2)
/* Description : Affiche le type d'appui en rz.
 * Paramètres : GtkTreeViewColumn *tree_column : composant à l'origine de l'évènement,
 *            : GtkCellRenderer *cell : la cellule en cours d'édition,
 *            : GtkTreeModel *tree_model : le mode en cours d'édition,
 *            : GtkTreeIter *iter : la ligne en cours d'édition,
 *            : gpointer data2 : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    EF_Appui    *appui;
    char        *txt;
    
    gtk_tree_model_get(tree_model, iter, 0, &appui, -1);
    
    switch (appui->rz)
    {
        case EF_APPUI_LIBRE :
        {
            txt = gettext("Libre");
            BUGMSG(appui->rz_donnees == NULL, , gettext("Le type d'appui de %s (%s) n'a pas à posséder de données.\n"), "rz", gettext("Libre"));
            break;
        }
        case EF_APPUI_BLOQUE :
        {
            txt = gettext("Bloqué");
            BUGMSG(appui->rz_donnees == NULL, , gettext("Le type d'appui de %s (%s) n'a pas à posséder de données.\n"), "rz", gettext("Bloqué"));
            break;
        }
        default :
        {
            BUGMSG(NULL, , gettext("Le type d'appui de %s (%d) est inconnu.\n"), "rz", appui->rz);
        }
    }
    
    g_object_set(cell, "text", txt, NULL);
}


void EF_gtk_appuis(Projet *projet)
/* Description : Création de la fenêtre permettant d'afficher les appuis sous forme d'un
 *               tableau.
 * Paramètres : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 *   Echec : projet == NULL,
 *           interface graphique impossible à générer.
 */
{
    Gtk_EF_Appuis   *ef_gtk;
    GList           *list_parcours;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    if (projet->list_gtk.ef_appuis.builder != NULL)
    {
        gtk_window_present(GTK_WINDOW(projet->list_gtk.ef_appuis.window));
        return;
    }
    
    ef_gtk = &projet->list_gtk.ef_appuis;
    
    ef_gtk->builder = gtk_builder_new();
    BUGMSG(gtk_builder_add_from_resource(ef_gtk->builder, "/org/2lgc/codegui/ui/EF_appuis.ui", NULL) != 0, , gettext("Builder Failed\n"));
    gtk_builder_connect_signals(ef_gtk->builder, projet);
    
    ef_gtk->window = GTK_WIDGET(gtk_builder_get_object(ef_gtk->builder, "EF_appuis_window"));
    ef_gtk->appuis = GTK_TREE_STORE(gtk_builder_get_object(ef_gtk->builder, "EF_appuis_treestore"));
    
    g_object_set(gtk_builder_get_object(ef_gtk->builder, "EF_appuis_treeview_cell1"), "model", projet->list_gtk.ef_appuis.liste_type_appui, NULL);
    g_object_set(gtk_builder_get_object(ef_gtk->builder, "EF_appuis_treeview_cell2"), "model", projet->list_gtk.ef_appuis.liste_type_appui, NULL);
    g_object_set(gtk_builder_get_object(ef_gtk->builder, "EF_appuis_treeview_cell3"), "model", projet->list_gtk.ef_appuis.liste_type_appui, NULL);
    g_object_set(gtk_builder_get_object(ef_gtk->builder, "EF_appuis_treeview_cell4"), "model", projet->list_gtk.ef_appuis.liste_type_appui, NULL);
    g_object_set(gtk_builder_get_object(ef_gtk->builder, "EF_appuis_treeview_cell5"), "model", projet->list_gtk.ef_appuis.liste_type_appui, NULL);
    g_object_set(gtk_builder_get_object(ef_gtk->builder, "EF_appuis_treeview_cell6"), "model", projet->list_gtk.ef_appuis.liste_type_appui, NULL);
    
    gtk_tree_view_column_set_cell_data_func(GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(ef_gtk->builder, "EF_appuis_treeview_column0")), GTK_CELL_RENDERER(gtk_builder_get_object(ef_gtk->builder, "EF_appuis_treeview_cell0")), EF_gtk_appuis_render_0, projet, NULL);
    gtk_tree_view_column_set_cell_data_func(GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(ef_gtk->builder, "EF_appuis_treeview_column1")), GTK_CELL_RENDERER(gtk_builder_get_object(ef_gtk->builder, "EF_appuis_treeview_cell1")), EF_gtk_appuis_render_1, projet, NULL);
    gtk_tree_view_column_set_cell_data_func(GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(ef_gtk->builder, "EF_appuis_treeview_column2")), GTK_CELL_RENDERER(gtk_builder_get_object(ef_gtk->builder, "EF_appuis_treeview_cell2")), EF_gtk_appuis_render_2, projet, NULL);
    gtk_tree_view_column_set_cell_data_func(GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(ef_gtk->builder, "EF_appuis_treeview_column3")), GTK_CELL_RENDERER(gtk_builder_get_object(ef_gtk->builder, "EF_appuis_treeview_cell3")), EF_gtk_appuis_render_3, projet, NULL);
    gtk_tree_view_column_set_cell_data_func(GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(ef_gtk->builder, "EF_appuis_treeview_column4")), GTK_CELL_RENDERER(gtk_builder_get_object(ef_gtk->builder, "EF_appuis_treeview_cell4")), EF_gtk_appuis_render_4, projet, NULL);
    gtk_tree_view_column_set_cell_data_func(GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(ef_gtk->builder, "EF_appuis_treeview_column5")), GTK_CELL_RENDERER(gtk_builder_get_object(ef_gtk->builder, "EF_appuis_treeview_cell5")), EF_gtk_appuis_render_5, projet, NULL);
    gtk_tree_view_column_set_cell_data_func(GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(ef_gtk->builder, "EF_appuis_treeview_column6")), GTK_CELL_RENDERER(gtk_builder_get_object(ef_gtk->builder, "EF_appuis_treeview_cell6")), EF_gtk_appuis_render_6, projet, NULL);
    
    list_parcours = projet->modele.appuis;
    while (list_parcours != NULL)
    {
        EF_Appui    *appui = (EF_Appui*)list_parcours->data;
        
        gtk_tree_store_append(ef_gtk->appuis, &appui->Iter_fenetre, NULL);
        gtk_tree_store_set(ef_gtk->appuis, &appui->Iter_fenetre, 0, appui, -1);
        
        list_parcours = g_list_next(list_parcours);
    }
    
    g_object_set_data(gtk_builder_get_object(ef_gtk->builder, "EF_appuis_treeview_cell1"), "column", GINT_TO_POINTER(1));
    g_object_set_data(gtk_builder_get_object(ef_gtk->builder, "EF_appuis_treeview_cell2"), "column", GINT_TO_POINTER(2));
    g_object_set_data(gtk_builder_get_object(ef_gtk->builder, "EF_appuis_treeview_cell3"), "column", GINT_TO_POINTER(3));
    g_object_set_data(gtk_builder_get_object(ef_gtk->builder, "EF_appuis_treeview_cell4"), "column", GINT_TO_POINTER(4));
    g_object_set_data(gtk_builder_get_object(ef_gtk->builder, "EF_appuis_treeview_cell5"), "column", GINT_TO_POINTER(5));
    g_object_set_data(gtk_builder_get_object(ef_gtk->builder, "EF_appuis_treeview_cell6"), "column", GINT_TO_POINTER(6));
    
    gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_appuis.builder, "EF_appuis_boutton_supprimer_direct")), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_appuis.builder, "EF_appuis_boutton_supprimer_menu")), FALSE);
    
    gtk_window_set_transient_for(GTK_WINDOW(ef_gtk->window), GTK_WINDOW(projet->list_gtk.comp.window));
}

#endif