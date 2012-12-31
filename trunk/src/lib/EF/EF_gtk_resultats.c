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

#include "1990_action.h"
#include "common_projet.h"
#include "common_erreurs.h"
#include "common_gtk.h"

G_MODULE_EXPORT void EF_gtk_resultats_fermer(GtkButton *button __attribute__((unused)),
  Projet *projet)
/* Description : Ferme la fenêtre.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_resultats.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Résultats");
    
    gtk_widget_destroy(projet->list_gtk.ef_resultats.window);
    
    return;
}


G_MODULE_EXPORT void EF_gtk_resultats_window_destroy(GtkWidget *object __attribute__((unused)),
  Projet *projet)
/* Description : met projet->list_gtk.ef_resultats.builder à NULL quand la fenêtre se ferme,
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_resultats.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Résultats");
    
    g_object_unref(G_OBJECT(projet->list_gtk.ef_resultats.builder));
    projet->list_gtk.ef_resultats.builder = NULL;
    
    return;
}


G_MODULE_EXPORT gboolean EF_gtk_resultats_window_key_press(
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
    BUGMSG(projet->list_gtk.ef_resultats.builder, TRUE, gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Résultats");
    
    if (event->key.keyval == GDK_KEY_Escape)
    {
        gtk_widget_destroy(projet->list_gtk.ef_resultats.window);
        return TRUE;
    }
    else
        return FALSE;
}


G_MODULE_EXPORT void EF_gtk_resultats_notebook_switch(GtkNotebook *notebook,
  GtkWidget *page __attribute__((unused)), gint page_num,
  Projet *projet __attribute__((unused)))
/* Description : Le changement de la page en cours nécessite l'actualisation de la disponibilité
 *               du bouton supprimer.
 * Paramètres : GtkNotebook *notebook : le composant notebook,
 *            : GtkWidget *page : composant désignant la page,
 *            : guint page_num : numéro de la page,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    if (page_num == gtk_notebook_get_n_pages(notebook)-1)
        g_signal_stop_emission_by_name(notebook, "switch-page");
    
    return;
}


G_MODULE_EXPORT void EF_gtk_resultats_add_page(GtkMenuItem *menuitem, Projet *projet)
/* Description : Ajoute une page au Notebook sur la base de divers tableaux "de base".
 * Paramètres : GtkMenuItem *menuitem : le composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    GtkWidget           *p_scrolled_window;
    GList               *list_parcours;
    int                 i = 0;
    Action              *action_en_cours = _1990_action_cherche_numero(projet, 0);
    GtkCellRenderer     *cell;
    GtkTreeViewColumn   *column;
    Gtk_EF_Resultats_Tableau    *res;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(menuitem, , gettext("Paramètre %s incorrect.\n"), "menuitem");
    BUGMSG(res = malloc(sizeof(Gtk_EF_Resultats_Tableau)), , gettext("Erreur d'allocation mémoire.\n"));
    
    p_scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(p_scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    
    gtk_notebook_insert_page(projet->list_gtk.ef_resultats.notebook, p_scrolled_window, GTK_WIDGET(gtk_label_new("Réactions d'appuis")), gtk_notebook_get_n_pages(projet->list_gtk.ef_resultats.notebook)-1);
    
    res->treeview = GTK_TREE_VIEW(gtk_tree_view_new());
    gtk_container_add(GTK_CONTAINER(p_scrolled_window), GTK_WIDGET(res->treeview));
    
    cell = gtk_cell_renderer_text_new();
    gtk_cell_renderer_set_alignment(cell, 0.5, 0.5);
    column = gtk_tree_view_column_new_with_attributes(gettext("Noeuds"), cell, "text", 0, NULL);
    gtk_tree_view_column_set_alignment(column, 0.5);
    gtk_tree_view_append_column(res->treeview, column);
    
    cell = gtk_cell_renderer_text_new();
    gtk_cell_renderer_set_alignment(cell, 1.0, 0.5);
    column = gtk_tree_view_column_new_with_attributes(gettext("Fx [N]"), cell, "text", 1, NULL);
    gtk_tree_view_column_set_alignment(column, 0.5);
    gtk_tree_view_append_column(res->treeview, column);
    g_object_set_data(G_OBJECT(cell), "column", GINT_TO_POINTER(1));
    gtk_tree_view_column_set_cell_data_func(column, cell, common_gtk_render_double, GINT_TO_POINTER(DECIMAL_FORCE), NULL);
    
    cell = gtk_cell_renderer_text_new();
    gtk_cell_renderer_set_alignment(cell, 1.0, 0.5);
    column = gtk_tree_view_column_new_with_attributes(gettext("Fy [N]"), cell, "text", 2, NULL);
    gtk_tree_view_column_set_alignment(column, 0.5);
    gtk_tree_view_append_column(res->treeview, column);
    g_object_set_data(G_OBJECT(cell), "column", GINT_TO_POINTER(2));
    gtk_tree_view_column_set_cell_data_func(column, cell, common_gtk_render_double, GINT_TO_POINTER(DECIMAL_FORCE), NULL);
    
    cell = gtk_cell_renderer_text_new();
    gtk_cell_renderer_set_alignment(cell, 1.0, 0.5);
    column = gtk_tree_view_column_new_with_attributes(gettext("Fz [N]"), cell, "text", 3, NULL);
    gtk_tree_view_column_set_alignment(column, 0.5);
    gtk_tree_view_append_column(res->treeview, column);
    g_object_set_data(G_OBJECT(cell), "column", GINT_TO_POINTER(3));
    gtk_tree_view_column_set_cell_data_func(column, cell, common_gtk_render_double, GINT_TO_POINTER(DECIMAL_FORCE), NULL);
    
    cell = gtk_cell_renderer_text_new();
    gtk_cell_renderer_set_alignment(cell, 1.0, 0.5);
    column = gtk_tree_view_column_new_with_attributes(gettext("Mx [N.m]"), cell, "text", 4, NULL);
    gtk_tree_view_column_set_alignment(column, 0.5);
    gtk_tree_view_append_column(res->treeview, column);
    g_object_set_data(G_OBJECT(cell), "column", GINT_TO_POINTER(4));
    gtk_tree_view_column_set_cell_data_func(column, cell, common_gtk_render_double, GINT_TO_POINTER(DECIMAL_FORCE), NULL);
    
    cell = gtk_cell_renderer_text_new();
    gtk_cell_renderer_set_alignment(cell, 1.0, 0.5);
    column = gtk_tree_view_column_new_with_attributes(gettext("My [N.m]"), cell, "text", 5, NULL);
    gtk_tree_view_column_set_alignment(column, 0.5);
    gtk_tree_view_append_column(res->treeview, column);
    g_object_set_data(G_OBJECT(cell), "column", GINT_TO_POINTER(5));
    gtk_tree_view_column_set_cell_data_func(column, cell, common_gtk_render_double, GINT_TO_POINTER(DECIMAL_FORCE), NULL);
    
    cell = gtk_cell_renderer_text_new();
    gtk_cell_renderer_set_alignment(cell, 1.0, 0.5);
    column = gtk_tree_view_column_new_with_attributes(gettext("Mz [N.m]"), cell, "text", 6, NULL);
    gtk_tree_view_column_set_alignment(column, 0.5);
    gtk_tree_view_append_column(res->treeview, column);
    g_object_set_data(G_OBJECT(cell), "column", GINT_TO_POINTER(6));
    gtk_tree_view_column_set_cell_data_func(column, cell, common_gtk_render_double, GINT_TO_POINTER(DECIMAL_FORCE), NULL);
    
    res->col_tab = malloc(sizeof(Colonne_Resultats)*8);
    res->col_tab[0] = 7;
    res->col_tab[1] = COLRES_NUM_NOEUDS;
    res->col_tab[2] = COLRES_REACTION_APPUI_FX;
    res->col_tab[3] = COLRES_REACTION_APPUI_FY;
    res->col_tab[4] = COLRES_REACTION_APPUI_FZ;
    res->col_tab[5] = COLRES_REACTION_APPUI_MX;
    res->col_tab[6] = COLRES_REACTION_APPUI_MX;
    res->col_tab[7] = COLRES_REACTION_APPUI_MX;
    
    res->list_store = gtk_list_store_new(7, G_TYPE_INT, G_TYPE_DOUBLE, G_TYPE_DOUBLE, G_TYPE_DOUBLE, G_TYPE_DOUBLE, G_TYPE_DOUBLE, G_TYPE_DOUBLE);
    gtk_tree_view_set_model(res->treeview, GTK_TREE_MODEL(res->list_store));
    g_object_unref(res->list_store);
    
    list_parcours = projet->ef_donnees.noeuds;
    while (list_parcours != NULL)
    {
        EF_Noeud    *noeud = list_parcours->data;
        
        if (noeud->appui != NULL)
        {
            GtkTreeIter Iter;
            double      *x = action_en_cours->efforts_noeuds->x;
            
            gtk_list_store_append(res->list_store, &Iter);
            gtk_list_store_set(res->list_store, &Iter, 0, noeud->numero, 1, x[i*6+0], 2, x[i*6+1], 3, x[i*6+2], 4, x[i*6+3], 5, x[i*6+4], 6, x[i*6+5], -1);
        }
        
        list_parcours = g_list_next(list_parcours);
        i++;
    }
    
    projet->list_gtk.ef_resultats.tableaux = g_list_append(projet->list_gtk.ef_resultats.tableaux, res);
    
    gtk_widget_show_all(p_scrolled_window);
    
    gtk_notebook_set_current_page(projet->list_gtk.ef_resultats.notebook, gtk_notebook_get_n_pages(projet->list_gtk.ef_resultats.notebook)-2);
    
    return;
}


G_MODULE_EXPORT void EF_gtk_resultats(Projet *projet)
/* Description : Création de la fenêtre permettant d'afficher les résultats sous forme d'un
 *               tableau.
 * Paramètres : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 *   Echec : projet == NULL,
 *           interface graphique impossible à générer.
 */
{
    Gtk_EF_Resultats    *ef_gtk;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    
    BUG(_1990_action_affiche_resultats(projet, 0), );
    
    if (projet->list_gtk.ef_resultats.builder != NULL)
    {
        gtk_window_present(GTK_WINDOW(projet->list_gtk.ef_resultats.window));
        return;
    }
    
    ef_gtk = &projet->list_gtk.ef_resultats;
    
    ef_gtk->builder = gtk_builder_new();
    BUGMSG(gtk_builder_add_from_file(ef_gtk->builder, DATADIR"/ui/EF_resultats.ui", NULL) != 0, , gettext("Builder Failed\n"));
    gtk_builder_connect_signals(ef_gtk->builder, projet);
    
    ef_gtk->window = GTK_WIDGET(gtk_builder_get_object(ef_gtk->builder, "EF_resultats_window"));
    ef_gtk->notebook = GTK_NOTEBOOK(gtk_builder_get_object(ef_gtk->builder, "EF_resultats_notebook"));
    
    gtk_window_set_transient_for(GTK_WINDOW(ef_gtk->window), GTK_WINDOW(projet->list_gtk.comp.window));
}


G_MODULE_EXPORT void EF_gtk_resultats_free(Projet *projet)
/* Description : Libère l'ensemble des éléments utilisés pour l'affichage des résultats.
 * Paramètres : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 *   Echec : projet == NULL,
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    
    while (projet->list_gtk.ef_resultats.tableaux != NULL)
    {
        Gtk_EF_Resultats_Tableau    *res = projet->list_gtk.ef_resultats.tableaux->data;
        
        free(res->col_tab);
        free(res);
        
        projet->list_gtk.ef_resultats.tableaux = g_list_delete_link(projet->list_gtk.ef_resultats.tableaux, projet->list_gtk.ef_resultats.tableaux);
    }
    
    return;
}
#endif
