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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>
#include <math.h>

#include "common_projet.h"
#include "common_erreurs.h"
#include "common_gtk.h"
#include "common_maths.h"
#include "EF_noeud.h"
#include "EF_charge_noeud.h"
#include "1990_actions.h"
#include "common_selection.h"


void EF_gtk_noeud_annuler(GtkButton *button __attribute__((unused)), GtkWidget *fenetre)
/* Description : Ferme la fenêtre sans effectuer les modifications
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    BUGMSG(fenetre, , "_EF_gtk_charge_noeud\n");
    gtk_widget_destroy(fenetre);
    return;
}


void EF_gtk_noeud(Projet *projet)
/* Description : Affichage de la fenêtre permettant de créer ou modifier la liste des noeuds.
 * Paramètres : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    List_Gtk_EF_Noeud   *ef_gtk;
//    Charge_Noeud        *charge_noeud;
    GtkCellRenderer     *pCellRenderer;
/*    GtkTreeIter         iter;
    GtkTreeViewColumn   *column;*/
    
    BUGMSG(projet, , "_EF_gtk_charge_noeud\n");
    BUGMSG(projet->ef_donnees.noeuds, , "_EF_gtk_charge_noeud\n");
    
    ef_gtk = &projet->list_gtk.ef_noeud;
    
    GTK_NOUVELLE_FENETRE(ef_gtk->window, gettext("Gestion des noeuds"), 600, 400)
    
    ef_gtk->table = gtk_table_new(2, 2, FALSE);
    gtk_container_add(GTK_CONTAINER(ef_gtk->window), ef_gtk->table);
    
    // Le treeview : 0 : numero, 1 : x, 2 : y, 3 : z, 4 : appui/
    ef_gtk->tree_store = gtk_tree_store_new(5, G_TYPE_INT, G_TYPE_DOUBLE, G_TYPE_DOUBLE, G_TYPE_DOUBLE, G_TYPE_STRING);
    ef_gtk->tree_view = (GtkTreeView*)gtk_tree_view_new_with_model(GTK_TREE_MODEL(ef_gtk->tree_store));
    ef_gtk->tree_select = gtk_tree_view_get_selection(ef_gtk->tree_view);
    ef_gtk->scroll = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(ef_gtk->scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(ef_gtk->scroll), GTK_WIDGET(ef_gtk->tree_view));
    gtk_table_attach(GTK_TABLE(ef_gtk->table), GTK_WIDGET(ef_gtk->scroll), 0, 2, 0, 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
    // Colonne numéro
    pCellRenderer = gtk_cell_renderer_text_new();
    g_object_set(pCellRenderer, "background", "#EEEEEE", NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(ef_gtk->tree_view), -1, gettext("Numéro"), pCellRenderer, "text", 0, NULL);
    // Colonne x
    pCellRenderer = gtk_cell_renderer_text_new();
    g_object_set(pCellRenderer, "editable", TRUE, NULL);
//    g_signal_connect(pCellRenderer, "edited", G_CALLBACK(EF_gtk_noeud_edit_x), projet);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(ef_gtk->tree_view), -1, "x", pCellRenderer, "text", 1, NULL);
    // Colonne y
    pCellRenderer = gtk_cell_renderer_text_new();
    g_object_set(pCellRenderer, "editable", TRUE, NULL);
//    g_signal_connect(pCellRenderer, "edited", G_CALLBACK(EF_gtk_noeud_edit_y), projet);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(ef_gtk->tree_view), -1, "y", pCellRenderer, "text", 2, NULL);
    // Colonne z
    pCellRenderer = gtk_cell_renderer_text_new();
    g_object_set(pCellRenderer, "editable", TRUE, NULL);
//    g_signal_connect(pCellRenderer, "edited", G_CALLBACK(EF_gtk_noeud_edit_z), projet);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(ef_gtk->tree_view), -1, "z", pCellRenderer, "text", 3, NULL);
    // Colonne Appui
    pCellRenderer = gtk_cell_renderer_combo_new();
    g_object_set(pCellRenderer, "editable", TRUE, "model", ef_gtk->liste_appuis, "text-column", 0, "has-entry", FALSE, NULL);
//    g_signal_connect(pCellRenderer, "edited", G_CALLBACK(EF_gtk_noeud_edit_z), projet);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(ef_gtk->tree_view), -1, gettext("Appui"), pCellRenderer, "text", 4, NULL);
    // On ajoute les noeuds existants.
    if ((projet->ef_donnees.noeuds != NULL) && (list_size(projet->ef_donnees.noeuds) != 0))
    {
        list_mvfront(projet->ef_donnees.noeuds);
        do
        {
            EF_Noeud    *noeud = list_curr(projet->ef_donnees.noeuds);
            GtkTreeIter iter;
            
            gtk_tree_store_append(ef_gtk->tree_store, &iter, NULL);
            gtk_tree_store_set(ef_gtk->tree_store, &iter, 0, noeud->numero, 1, noeud->position.x, 2, noeud->position.y, 3, noeud->position.z, 4, (noeud->appui == NULL ? gettext("Aucun") : noeud->appui->nom), -1);

        }
        while (list_mvnext(projet->ef_donnees.noeuds) != NULL);
    }
    
    ef_gtk->button_valider = gtk_button_new_from_stock(GTK_STOCK_OK);
    //g_signal_connect(ef_gtk->button_fermer, "clicked", G_CALLBACK(EF_gtk_noeud_annuler), ef_gtk->window);
    gtk_table_attach(GTK_TABLE(ef_gtk->table), ef_gtk->button_valider, 1, 2, 1, 2, GTK_EXPAND | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
    
    ef_gtk->button_annuler = gtk_button_new_from_stock(GTK_STOCK_CANCEL);
    g_signal_connect(ef_gtk->button_annuler, "clicked", G_CALLBACK(EF_gtk_noeud_annuler), ef_gtk->window);
    gtk_table_attach(GTK_TABLE(ef_gtk->table), ef_gtk->button_annuler, 1, 2, 1, 2, GTK_EXPAND | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
    
    gtk_window_set_transient_for(GTK_WINDOW(ef_gtk->window), GTK_WINDOW(projet->list_gtk.comp.window));
    gtk_window_set_modal(GTK_WINDOW(ef_gtk->window), TRUE);
    gtk_widget_show_all(ef_gtk->window);
    
    return;
}


#endif
