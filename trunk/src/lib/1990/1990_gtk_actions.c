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

#include "1990_actions.h"
#include "1990_coef_psi.h"
#include "common_erreurs.h"
#include "common_projet.h"
#include "common_gtk.h"
#include "EF_gtk_charge_noeud.h"
#include "EF_gtk_charge_barre_ponctuelle.h"
#include "EF_gtk_charge_barre_repartie_uniforme.h"

const GtkTargetEntry drag_targets_actions[] = { {(gchar*)PACKAGE"1_SAME_PROC", GTK_TARGET_SAME_APP, 0}}; 

void _1990_gtk_tree_view_actions_cell_edited(GtkCellRendererText *cell __attribute__((unused)), gchar *path_string, gchar *new_text, Projet *projet)
/* Description : Pour éditer le nom des actions via la fenêtre d'actions.
 * Paramètres : GtkCellRendererText *cell : la cellule éditée,
 *            : gchar *path_string : chemin vers la cellule,
 *            : gchar *new_text : nouveau texte,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune
 */
{
    List_Gtk_1990_Actions *list_gtk_1990_actions = &projet->list_gtk._1990_actions;
    GtkTreePath   *path = gtk_tree_path_new_from_string (path_string);
    GtkTreeIter   iter;
    GtkTreeModel  *model = gtk_tree_view_get_model(GTK_TREE_VIEW(list_gtk_1990_actions->tree_view_actions));
    GValue        nouvelle_valeur;
    int           numero;
    char          *description;
    Action        *action;
    
    gtk_tree_model_get_iter (model, &iter, path);
    gtk_tree_model_get(model, &iter, 0, &numero, 1, &description, -1);
    
    BUGMSG(_1990_action_cherche_numero(projet, numero) == 0, , "_1990_gtk_tree_view_action_cell_edited\n");
    action = list_curr(projet->actions);
    action->description = realloc(action->description, sizeof(gchar)*(strlen(new_text)+1));
    BUGMSG(action->description, , gettext("%s : Erreur d'allocation mémoire.\n"), "_1990_gtk_tree_view_action_cell_edited");
    strcpy(action->description, new_text);
    
    memset(&nouvelle_valeur, 0, sizeof(nouvelle_valeur));
    g_value_init (&nouvelle_valeur, G_TYPE_STRING);
    g_value_set_string (&nouvelle_valeur, new_text);
    gtk_tree_store_set_value(list_gtk_1990_actions->tree_store_actions, &iter, 1, &nouvelle_valeur);
    
    gtk_tree_path_free (path);
    
    return;
}


void _1990_gtk_tree_view_charges_cell_edited(GtkCellRendererText *cell __attribute__((unused)), gchar *path_string, gchar *new_text, Projet *projet)
/* Description : Pour éditer le nom des charges via la fenêtre d'actions.
 * Paramètres : GtkCellRendererText *cell : la cellule éditée,
 *            : gchar *path_string : chemin vers la cellule,
 *            : gchar *new_text : nouveau texte,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune
 */
{
    List_Gtk_1990_Actions *list_gtk_1990_actions = &projet->list_gtk._1990_actions;
    GtkTreePath     *path = gtk_tree_path_new_from_string (path_string);
    GtkTreeIter     iter, iter_action;
    GtkTreeModel    *model_action;
    GtkTreeModel    *model = gtk_tree_view_get_model(GTK_TREE_VIEW(list_gtk_1990_actions->tree_view_charges));
    GValue          nouvelle_valeur;
    int             numero_action, numero_charge;
    char            *description;
    Charge_Noeud    *charge;
    
    if (!gtk_tree_selection_get_selected(list_gtk_1990_actions->tree_select_actions, &model_action, &iter_action))
        return;
    gtk_tree_model_get(model_action, &iter_action, 0, &numero_action, -1);
    
    gtk_tree_model_get_iter (model, &iter, path);
    gtk_tree_model_get(model, &iter, 0, &numero_charge, 1, &description, -1);
    
    charge = _1990_action_cherche_charge(projet, numero_action, numero_charge);
    BUG(charge, );
    
    charge->description = realloc(charge->description, sizeof(gchar)*(strlen(new_text)+1));
    BUGMSG(charge->description, , gettext("%s : Erreur d'allocation mémoire.\n"), "_1990_gtk_tree_view_action_cell_edited");
    strcpy(charge->description, new_text);
    
    memset(&nouvelle_valeur, 0, sizeof(nouvelle_valeur));
    g_value_init (&nouvelle_valeur, G_TYPE_STRING);
    g_value_set_string (&nouvelle_valeur, new_text);
    gtk_tree_store_set_value(list_gtk_1990_actions->tree_store_charges, &iter, 1, &nouvelle_valeur);
    
    gtk_tree_path_free (path);
    
    return;
}


void _1990_gtk_tree_view_actions_cursor_changed(GtkTreeView *tree_view __attribute__((unused)), Projet *projet)
/* Description : Evènement lorsqu'il y a un changement de ligne sélectionnée dans la liste
 *                 des actions
 * Paramètres : GtkTreeView *tree_view : composant tree_view à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    List_Gtk_1990_Actions   *list_gtk_1990_actions;
    GtkTreeModel    *model;
    GtkTreeIter     iter;
    int             numero;
    char            *nom;
    Action          *action;
    
    BUGMSG(projet, , "_1990_gtk_tree_view_actions_cursor_changed\n");
    BUGMSG(list_size(projet->actions), , "_1990_gtk_tree_view_actions_cursor_changed\n");
    
    list_gtk_1990_actions = &projet->list_gtk._1990_actions;
    
    if (list_gtk_1990_actions->window == NULL)
        return;
    
    if (!gtk_tree_selection_get_selected(list_gtk_1990_actions->tree_select_actions, &model, &iter))
        return;
    gtk_tree_model_get(model, &iter, 0, &numero, 1, &nom, -1);
    
    BUGMSG(_1990_action_cherche_numero(projet, numero) == 0, , "_1990_gtk_tree_view_actions_cursor_changed\n");
    action = list_curr(projet->actions);
    
    gtk_tree_store_clear(list_gtk_1990_actions->tree_store_charges);
    if (list_size(action->charges))
    {
        list_mvfront(action->charges);
        do
        {
            Charge_Barre_Ponctuelle *charge_tmp = list_curr(action->charges);
            switch (charge_tmp->type)
            {
                case CHARGE_NOEUD :
                {
                    Charge_Noeud *charge = list_curr(action->charges);
                    
                    EF_gtk_charge_noeud_ajout_affichage(charge, projet, TRUE);
                    
                    break;
                }
                case CHARGE_BARRE_PONCTUELLE :
                {
                    Charge_Barre_Ponctuelle *charge = list_curr(action->charges);
                    
                    EF_gtk_charge_barre_ponctuelle_ajout_affichage(charge, projet, TRUE);
                    
                    break;
                }
                case CHARGE_BARRE_REPARTIE_UNIFORME :
                {
                    Charge_Barre_Repartie_Uniforme *charge = list_curr(action->charges);
                    
                    EF_gtk_charge_barre_repartie_uniforme_ajout_affichage(charge, projet, TRUE);
                    
                    break;
                }
                default :
                {
                    BUGMSG(0, , "_1990_gtk_tree_view_actions_cursor_changed\n");
                    break;
                }
            }
        } while (list_mvnext(action->charges) != NULL);
    }
    
    return;
}


void _1990_gtk_actions_tree_view_drag_begin(GtkWidget *widget __attribute__((unused)), 
  GdkDragContext *drag_context __attribute__((unused)), void *data __attribute__((unused)))
{
    return;
}


gboolean _1990_gtk_actions_tree_view_drag(GtkWidget *widget __attribute__((unused)),
  GdkDragContext *drag_context __attribute__((unused)),
  gint x __attribute__((unused)), gint y __attribute__((unused)),
  guint tim __attribute__((unused)), Projet *projet __attribute__((unused)))
/* Description : Charge d'action la charge sélectionnée.
 * Paramètres : GtkWidget *button : composant ayant réalisé l'évènement,
 *            : GdkDragContext *drag_context : inutile,
 *            : GtkSelectionData *data : inutile,
 *            : guint info : inutile,
 *            : guint time : inutile,
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : FALSE
 */
{
    gint                    cx, cy;
    List_Gtk_1990_Actions   *list_gtk_1990_actions = &projet->list_gtk._1990_actions;
    GtkTreePath             *path;
    GtkTreeModel            *list_store;
    
    gdk_window_get_geometry(gtk_tree_view_get_bin_window(GTK_TREE_VIEW(list_gtk_1990_actions->tree_view_actions)), &cx, &cy, NULL, NULL);
    gtk_tree_view_get_path_at_pos(GTK_TREE_VIEW(widget), x -=cx, y -=cy, &path, NULL, &cx, &cy);
    list_store = gtk_tree_view_get_model(GTK_TREE_VIEW(list_gtk_1990_actions->tree_view_actions));
    if (path != NULL)
    {
        int                 num_action_dest,  num_charge_source,  num_action_source;
        GtkTreeIter         iter_action_dest, iter_charge_source, iter_action_source;
        GtkTreeModel        *model_charge_source, *model_action_source;
        GList               *list, *list_fixe, *list_parcours;
        
        gtk_tree_model_get_iter(list_store, &iter_action_dest, path);
        gtk_tree_model_get(list_store, &iter_action_dest, 0, &num_action_dest, -1);
        
        if (!gtk_tree_selection_get_selected(list_gtk_1990_actions->tree_select_actions, &model_action_source, &iter_action_source))
            return FALSE;
        gtk_tree_model_get(model_action_source, &iter_action_source, 0, &num_action_source, -1);
        
        list = gtk_tree_selection_get_selected_rows(list_gtk_1990_actions->tree_select_charges, &model_charge_source);
        // On converti les lignes en ligne fixe
        list_parcours = g_list_last(list);
        list_fixe = NULL;
        for(;list_parcours != NULL; list_parcours = g_list_previous(list_parcours))
            list_fixe = g_list_append(list_fixe, gtk_tree_row_reference_new(model_charge_source, (GtkTreePath*)list_parcours->data));
        g_list_foreach(list, (GFunc)gtk_tree_path_free, NULL);
        g_list_free(list);
        
        // On travaille sur les lignes fixes
        list_parcours = g_list_last(list_fixe);
        for(;list_parcours != NULL; list_parcours = g_list_previous(list_parcours))
        {
            if (gtk_tree_model_get_iter(model_charge_source, &iter_charge_source, gtk_tree_row_reference_get_path((GtkTreeRowReference*)list_parcours->data)))
            {
                gtk_tree_model_get(model_charge_source, &iter_charge_source, 0, &num_charge_source, -1);
                _1990_action_deplace_charge(projet, num_action_source, num_charge_source, num_action_dest);
            }
        }
        g_list_foreach(list_fixe, (GFunc)gtk_tree_row_reference_free, NULL);
        g_list_free(list_fixe);
    }
    
    return FALSE;
}


void _1990_gtk_tree_view_actions_type_edited(GtkCellRendererText *cell __attribute__((unused)), const gchar *path_string, const gchar *new_text, Projet *projet)
/* Description : Changement du type d'une action
 * Paramètres : GtkCellRendererText *cell : cellule en cours,
 *            : gchar *path_string : path de la ligne en cours,
 *            : gchar *new_text : nouveau coefficient psi0,
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
*/
{
    List_Gtk_1990_Actions   *list_gtk_1990_actions = &projet->list_gtk._1990_actions;
    GtkTreeModel            *model = GTK_TREE_MODEL(list_gtk_1990_actions->tree_store_actions);
    GtkTreePath             *path = gtk_tree_path_new_from_string (path_string);
    GtkTreeIter             iter;
    gint                    i, j;
    Action                  *action;
    
    gtk_tree_model_get_iter(model, &iter, path);
    
    i = gtk_tree_path_get_indices(path)[0];
    
    for (j=0;j<_1990_action_num_bat_txt(projet->pays);j++)
    {
        if (strcmp(new_text, _1990_action_type_bat_txt(j, projet->pays)) == 0)
            break;
    }
    BUGMSG(j != _1990_action_num_bat_txt(projet->pays), , "cell_edited");
    BUGMSG(_1990_action_cherche_numero(projet, i) == 0, , "cell_edited");
    action = list_curr(projet->actions);
    action->type = j;
    action->psi0 = _1990_coef_psi0_bat(j, projet->pays);
    action->psi1 = _1990_coef_psi1_bat(j, projet->pays);
    action->psi2 = _1990_coef_psi2_bat(j, projet->pays);
    gtk_tree_store_set(list_gtk_1990_actions->tree_store_actions, &iter, 2, new_text, 3, action->psi0, 4, action->psi1, 5, action->psi2, -1);
    
    gtk_tree_path_free (path);
}


void _1990_gtk_tree_view_actions_psi_edited(GtkCellRendererText *cell, gchar *path_string, gchar *new_text, Projet *projet)
/* Description : Changement du coefficient psi0 d'une action
 * Paramètres : GtkCellRendererText *cell : cellule en cours,
 *            : gchar *path_string : path de la ligne en cours,
 *            : gchar *new_text : nouveau coefficient psi0,
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    List_Gtk_1990_Actions   *list_gtk_1990_actions = &projet->list_gtk._1990_actions;
    GtkTreeModel            *model = GTK_TREE_MODEL(list_gtk_1990_actions->tree_store_actions);
    GtkTreePath             *path = gtk_tree_path_new_from_string(path_string);
    GtkTreeIter             iter;
    gint                    i;
    char                    *fake = (char*)malloc(sizeof(char)*(strlen(new_text)+1));
    double                  convertion;
    Action                  *action;
    gint                    column = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(cell), "column"));
    
    gtk_tree_model_get_iter(model, &iter, path);
    
    i = gtk_tree_path_get_indices(path)[0];
    
    if (sscanf(new_text, "%lf%s", &convertion, fake) == 1)
    {
        gtk_tree_store_set(list_gtk_1990_actions->tree_store_actions, &iter, column, convertion, -1);
        BUGMSG(_1990_action_cherche_numero(projet, i) == 0, , "_1990_gtk_tree_view_actions_psi_edited");
        action = list_curr(projet->actions);
        switch (column)
        {
            case 3:
            {
                action->psi0 = convertion;
                break;
            }
            case 4:
            {
                action->psi1 = convertion;
                break;
            }
            case 5:
            {
                action->psi2 = convertion;
                break;
            }
            default :
            {
                BUGMSG(NULL, , "_1990_gtk_tree_view_actions_psi_edited");
                break;
            }
        }
    }
    
    gtk_tree_path_free (path);
}


void _1990_gtk_menu_nouvelle_action_activate(GtkMenuItem *menuitem, Projet *projet)
/* Description : Ajout d'une nouvelle action depuis le menu de la barre d'outils
 * Paramètres : GtkMenuItem *menuitem : composant à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    List_Gtk_1990_Actions   *list_gtk_1990_actions = &projet->list_gtk._1990_actions;
    int                     i = 0;
    char                    *tmp = (char*)malloc(sizeof(char)*(strlen(gettext("Sans nom")+10)));
    
    if (list_size(list_gtk_1990_actions->menu_list_widget_action) == 0)
        return;
    list_mvfront(list_gtk_1990_actions->menu_list_widget_action);
    do
    {
        if ((GTK_IS_MENU_TOOL_BUTTON(menuitem)) || (*(GtkMenuItem **)list_curr(list_gtk_1990_actions->menu_list_widget_action) == menuitem))
        {
            Action      *action;
            GtkTreePath *path;
            
            sprintf(tmp, "%s %zu", gettext("Sans nom"), list_size(projet->actions));
            _1990_action_ajout(projet, i, tmp);
            
            action = list_curr(projet->actions);
            gtk_tree_store_append(list_gtk_1990_actions->tree_store_actions, &action->Iter, NULL);
            gtk_tree_store_set(list_gtk_1990_actions->tree_store_actions, &action->Iter, 0, action->numero, 1, action->description, 2, _1990_action_type_bat_txt(action->type, projet->pays), 3, action->psi0, 4, action->psi1, 5, action->psi2, -1);
            path = gtk_tree_model_get_path(GTK_TREE_MODEL(list_gtk_1990_actions->tree_store_actions), &action->Iter);
            gtk_tree_view_set_cursor(GTK_TREE_VIEW(list_gtk_1990_actions->tree_view_actions), path, gtk_tree_view_get_column(GTK_TREE_VIEW(list_gtk_1990_actions->tree_view_actions), 1), TRUE);
            gtk_tree_path_free(path);
            
            return;
        }
        i++;
    } while (list_mvnext(list_gtk_1990_actions->menu_list_widget_action));
    
    return;
}


void _1990_gtk_menu_suppr_action_activate(GtkToolButton *toolbutton __attribute__((unused)), Projet *projet)
/* Description : Supprimer l'action sélectionnée
 * Paramètres : GtkToolButton *toolbutton : composant à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    List_Gtk_1990_Actions *list_gtk_1990_actions = &projet->list_gtk._1990_actions;
    GtkTreeIter     iter;
    GtkTreeModel    *model;
    size_t          numero_action;
    int             numer;
    
    if (!gtk_tree_selection_get_selected(list_gtk_1990_actions->tree_select_actions, &model, &iter))
        return;
    
    gtk_tree_model_get(model, &iter, 0, &numer, -1);
    numero_action = numer;
    gtk_tree_store_remove(list_gtk_1990_actions->tree_store_actions, &iter);
    _1990_action_free_num(projet, numero_action);
    list_mvfront(projet->actions);
    if (list_size(projet->actions) != 0)
    {
        do
        {
            Action *action = list_curr(projet->actions);
            
            if (action->numero >= numero_action)
                gtk_tree_store_set(list_gtk_1990_actions->tree_store_actions, &action->Iter, 0, action->numero, -1);
        } while (list_mvnext(projet->actions) != NULL);
    }
    gtk_tree_store_clear(list_gtk_1990_actions->tree_store_charges);
    return;
}


void _1990_gtk_menu_nouvelle_charge_nodale_activate(GtkMenuItem *menuitem __attribute__((unused)), Projet* projet)
{
    GtkTreeIter                 iter_action;
    GtkTreeModel                *model_action;
    int                         numero_action;
    
    BUGMSG(projet, , "_1990_gtk_menu_nouvelle_charge_ponctuel_activate\n");
    BUGMSG(projet->actions, , "_1990_gtk_menu_nouvelle_charge_ponctuel_activate\n");
    BUGMSG(list_size(projet->actions) != 0, , "_1990_gtk_menu_nouvelle_charge_ponctuel_activate\n");
    
    if (!gtk_tree_selection_get_selected(projet->list_gtk._1990_actions.tree_select_actions, &model_action, &iter_action))
        return;
    gtk_tree_model_get(model_action, &iter_action, 0, &numero_action, -1);
    EF_gtk_charge_noeud(projet, numero_action, -1);
    
    return;
}


void _1990_gtk_menu_nouvelle_charge_barre_ponctuelle_activate(GtkMenuItem *menuitem __attribute__((unused)), Projet* projet)
{
    GtkTreeIter                 iter_action;
    GtkTreeModel                *model_action;
    int                         numero_action;
    
    BUGMSG(projet, , "_1990_gtk_menu_nouvelle_charge_ponctuel_activate\n");
    BUGMSG(projet->actions, , "_1990_gtk_menu_nouvelle_charge_ponctuel_activate\n");
    BUGMSG(list_size(projet->actions) != 0, , "_1990_gtk_menu_nouvelle_charge_ponctuel_activate\n");
    
    if (!gtk_tree_selection_get_selected(projet->list_gtk._1990_actions.tree_select_actions, &model_action, &iter_action))
        return;
    gtk_tree_model_get(model_action, &iter_action, 0, &numero_action, -1);
    EF_gtk_charge_barre_ponctuelle(projet, numero_action, -1);
    
    return;
}


void _1990_gtk_menu_nouvelle_charge_barre_repartie_uniforme_activate(GtkMenuItem *menuitem __attribute__((unused)), Projet* projet)
{
    GtkTreeIter                 iter_action;
    GtkTreeModel                *model_action;
    int                         numero_action;
    
    BUGMSG(projet, , "_1990_gtk_menu_nouvelle_charge_repartie_uniforme_activate\n");
    BUGMSG(projet->actions, , "_1990_gtk_menu_nouvelle_charge_repartie_uniforme_activate\n");
    BUGMSG(list_size(projet->actions) != 0, , "_1990_gtk_menu_nouvelle_charge_repartie_uniforme_activate\n");
    
    if (!gtk_tree_selection_get_selected(projet->list_gtk._1990_actions.tree_select_actions, &model_action, &iter_action))
        return;
    gtk_tree_model_get(model_action, &iter_action, 0, &numero_action, -1);
    EF_gtk_charge_barre_repartie_uniforme(projet, numero_action, -1);
    
    return;
}


void _1990_gtk_actions_select_changed(GtkTreeSelection *treeselection __attribute__((unused)), Projet *projet)
{
    List_Gtk_1990_Actions   *list_gtk_1990_actions = &projet->list_gtk._1990_actions;
    
    if (!gtk_tree_selection_get_selected(list_gtk_1990_actions->tree_select_actions, NULL, NULL))
    {
        gtk_widget_set_sensitive(GTK_WIDGET(list_gtk_1990_actions->item_action_suppr), FALSE);
        gtk_widget_set_sensitive(GTK_WIDGET(list_gtk_1990_actions->item_charge_ajout), FALSE);
    }
    else
    {
        gtk_widget_set_sensitive(GTK_WIDGET(list_gtk_1990_actions->item_action_suppr), TRUE);
        gtk_widget_set_sensitive(GTK_WIDGET(list_gtk_1990_actions->item_charge_ajout), TRUE);
    }
    
    if (gtk_tree_selection_count_selected_rows(list_gtk_1990_actions->tree_select_charges) == 0)
    {
        gtk_widget_set_sensitive(GTK_WIDGET(list_gtk_1990_actions->item_charge_edit), FALSE);
        gtk_widget_set_sensitive(GTK_WIDGET(list_gtk_1990_actions->item_charge_suppr), FALSE);
    }
    else
    {
        gtk_widget_set_sensitive(GTK_WIDGET(list_gtk_1990_actions->item_charge_edit), TRUE);
        gtk_widget_set_sensitive(GTK_WIDGET(list_gtk_1990_actions->item_charge_suppr), TRUE);
    }
    
    return;
}


void _1990_gtk_menu_suppr_charge_clicked(GtkToolButton *toolbutton __attribute__((unused)), Projet *projet)
/* Description : Supprimer l'action sélectionnée
 * Paramètres : GtkToolButton *toolbutton : composant à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    List_Gtk_1990_Actions *list_gtk_1990_actions = &projet->list_gtk._1990_actions;
    GtkTreeIter     iter;
    GtkTreeModel    *model;
    size_t          numero_action, numero_charge;
    int             numer;
    GList           *list, *list_fixe, *list_parcours;
    
    if (!gtk_tree_selection_get_selected(list_gtk_1990_actions->tree_select_actions, &model, &iter))
        return;
    gtk_tree_model_get(model, &iter, 0, &numer, -1);
    numero_action = numer;
    
    list = gtk_tree_selection_get_selected_rows(list_gtk_1990_actions->tree_select_charges, &model);
    // On converti les lignes en ligne fixe
    list_parcours = g_list_last(list);
    list_fixe = NULL;
    for(;list_parcours != NULL; list_parcours = g_list_previous(list_parcours))
        list_fixe = g_list_append(list_fixe, gtk_tree_row_reference_new(model, (GtkTreePath*)list_parcours->data));
    g_list_foreach(list, (GFunc)gtk_tree_path_free, NULL);
    g_list_free(list);
    
    // On travaille sur les lignes fixes
    list_parcours = g_list_first(list_fixe);
    for(;list_parcours != NULL; list_parcours = g_list_next(list_parcours))
    {
        if (gtk_tree_model_get_iter(model, &iter, gtk_tree_row_reference_get_path((GtkTreeRowReference*)list_parcours->data)))
        {
            gtk_tree_model_get(model, &iter, 0, &numer, -1);
            numero_charge = numer;
            _1990_action_supprime_charge(projet, numero_action, numero_charge);
        }
    }
    g_list_foreach(list_fixe, (GFunc)gtk_tree_row_reference_free, NULL);
    g_list_free(list_fixe);
    
    return;
}


void _1990_gtk_menu_edit_charge_clicked(GtkToolButton *toolbutton __attribute__((unused)), Projet *projet)
/* Description : Supprimer l'action sélectionnée
 * Paramètres : GtkToolButton *toolbutton : composant à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    List_Gtk_1990_Actions *list_gtk_1990_actions = &projet->list_gtk._1990_actions;
    GtkTreeIter     iter;
    GtkTreeModel    *model;
    size_t          numero_action, numero_charge;
    int             numer;
    GList           *list, *list_parcours;
    
    if (!gtk_tree_selection_get_selected(list_gtk_1990_actions->tree_select_actions, &model, &iter))
        return;
    gtk_tree_model_get(model, &iter, 0, &numer, -1);
    numero_action = numer;
    
    list = gtk_tree_selection_get_selected_rows(list_gtk_1990_actions->tree_select_charges, &model);
    list_parcours = g_list_first(list);
    for(;list_parcours != NULL; list_parcours = g_list_next(list_parcours))
    {
        if (gtk_tree_model_get_iter(model, &iter, (GtkTreePath*)list_parcours->data))
        {
            Charge_Noeud    *charge_noeud;
            gtk_tree_model_get(model, &iter, 0, &numer, -1);
            numero_charge = numer;
            charge_noeud = _1990_action_cherche_charge(projet, numero_action, numero_charge);
            switch (charge_noeud->type)
            {
                case CHARGE_NOEUD :
                {
                    EF_gtk_charge_noeud(projet, numero_action, numero_charge);
                    break;
                }
                case CHARGE_BARRE_PONCTUELLE :
                {
                    EF_gtk_charge_barre_ponctuelle(projet, numero_action, numero_charge);
                    break;
                }
                case CHARGE_BARRE_REPARTIE_UNIFORME :
                {
                    EF_gtk_charge_barre_repartie_uniforme(projet, numero_action, numero_charge);
                    break;
                }
                default :
                {
                    BUG(0, );
                    break;
                }
            }
        }
    }
    g_list_foreach(list, (GFunc)gtk_tree_path_free, NULL);
    g_list_free(list);
    
    return;
}


void _1990_gtk_actions_window_destroy(GtkWidget *object __attribute__((unused)), Projet *projet)
{
    projet->list_gtk._1990_actions.window = NULL;
    return;
}


gboolean _1990_gtk_actions_charge_double_clicked(GtkWidget *widget __attribute__((unused)), GdkEvent *event, Projet *projet)
/* Description : Lance la fenêtre d'édition de la charge sélectionnée
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : GdkEvent *event : Information sur l'évènement,
 *            : Projet *projet : la variable projet
 *            : gboolean nouveau : vaut TRUE si une nouvelle charge doit être ajoutée,
 *                                 vaut FALSE si la charge en cours doit être modifiée
 * Valeur renvoyée : Aucune
 */
{
    if ((event->type == GDK_2BUTTON_PRESS) && (gtk_widget_get_sensitive(GTK_WIDGET(projet->list_gtk._1990_actions.item_charge_edit))))
        _1990_gtk_menu_edit_charge_clicked(NULL, projet);
    return FALSE;
}


void _1990_gtk_actions(Projet *projet)
/* Description : Affichage de la fenêtre permettant de gérer les actions
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    List_Gtk_1990_Actions   *list_gtk_1990_actions;
    GtkCellRenderer         *pCellRenderer;
    GList                   *list;
    GtkTreeIter             iter;
    int                     i;
    GtkWidget               *w_temp;
    GtkTreeViewColumn       *column;
    
    BUGMSG(projet, , "_1990_gtk_actions\n");
    BUGMSG(projet->actions, , "_1990_gtk_actions\n");
    
    list_gtk_1990_actions = &projet->list_gtk._1990_actions;
    GTK_NOUVELLE_FENETRE(list_gtk_1990_actions->window, gettext("Actions"), 800, 600);
    g_signal_connect(G_OBJECT(list_gtk_1990_actions->window), "destroy", G_CALLBACK(_1990_gtk_actions_window_destroy), projet);
    
    list_gtk_1990_actions->table = gtk_table_new(1, 1, FALSE);
    gtk_container_add(GTK_CONTAINER(list_gtk_1990_actions->window), list_gtk_1990_actions->table);
    
    list_gtk_1990_actions->paned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_paned_set_position(GTK_PANED(list_gtk_1990_actions->paned), 400);
    gtk_table_attach(GTK_TABLE(list_gtk_1990_actions->table), list_gtk_1990_actions->paned, 0, 1, 0, 1, (GtkAttachOptions)(GTK_EXPAND | GTK_FILL), (GtkAttachOptions)(GTK_EXPAND | GTK_FILL), 0, 0);
    
    
    // Le coté actions
    list_gtk_1990_actions->table_actions = gtk_table_new(2, 1, FALSE);
    gtk_paned_add1(GTK_PANED(list_gtk_1990_actions->paned), list_gtk_1990_actions->table_actions);
    list_gtk_1990_actions->scroll_actions = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(list_gtk_1990_actions->scroll_actions), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    // 0 : numero, 1 : description, 2 : type, 3 : psi0, 4 : psi1, 5 : psi2
    list_gtk_1990_actions->tree_store_actions = gtk_tree_store_new(6, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_DOUBLE, G_TYPE_DOUBLE, G_TYPE_DOUBLE);
    list_gtk_1990_actions->tree_view_actions = (GtkTreeView*)gtk_tree_view_new_with_model(GTK_TREE_MODEL(list_gtk_1990_actions->tree_store_actions));
    list_gtk_1990_actions->tree_select_actions = gtk_tree_view_get_selection(list_gtk_1990_actions->tree_view_actions);
    gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(list_gtk_1990_actions->scroll_actions), GTK_WIDGET(list_gtk_1990_actions->tree_view_actions));
    gtk_table_attach(GTK_TABLE(list_gtk_1990_actions->table_actions), list_gtk_1990_actions->scroll_actions, 0, 1, 0, 1, (GtkAttachOptions)(GTK_EXPAND | GTK_FILL), (GtkAttachOptions)(GTK_EXPAND | GTK_FILL), 0, 0);
    // Colonne numéro
    pCellRenderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(list_gtk_1990_actions->tree_view_actions), -1, gettext("Numéro"), pCellRenderer, "text", 0, NULL);
    // Colonne description
    pCellRenderer = gtk_cell_renderer_text_new();
    g_object_set(pCellRenderer, "editable", TRUE, NULL);
    g_signal_connect(pCellRenderer, "edited", G_CALLBACK(_1990_gtk_tree_view_actions_cell_edited), projet);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(list_gtk_1990_actions->tree_view_actions), -1, gettext("Description"), pCellRenderer, "text", 1, NULL);
    g_signal_connect(G_OBJECT(list_gtk_1990_actions->tree_view_actions), "cursor-changed", G_CALLBACK(_1990_gtk_tree_view_actions_cursor_changed), projet);
    // Colonne type
    pCellRenderer = gtk_cell_renderer_combo_new();
    list_gtk_1990_actions->choix_type_action = gtk_list_store_new(1, G_TYPE_STRING);
    for (i=0;i<_1990_action_num_bat_txt(projet->pays);i++)
    {
        gtk_list_store_append(list_gtk_1990_actions->choix_type_action, &iter);
        gtk_list_store_set(list_gtk_1990_actions->choix_type_action, &iter, 0, _1990_action_type_bat_txt(i, projet->pays), -1);
    }
    g_object_set(pCellRenderer, "editable", TRUE, "model", list_gtk_1990_actions->choix_type_action, "text-column", 0, "has-entry", FALSE, NULL);
    g_signal_connect(pCellRenderer, "edited", G_CALLBACK(_1990_gtk_tree_view_actions_type_edited), projet);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(list_gtk_1990_actions->tree_view_actions), -1, gettext("Type"), pCellRenderer, "text", 2, NULL);
    // Colonnes psi0, psi1, psi2
    pCellRenderer = gtk_cell_renderer_text_new();
    g_object_set(pCellRenderer, "editable", TRUE, NULL);
    g_signal_connect(pCellRenderer, "edited", G_CALLBACK(_1990_gtk_tree_view_actions_psi_edited), projet);
    g_object_set_data(G_OBJECT(pCellRenderer), "column", GINT_TO_POINTER(3));
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(list_gtk_1990_actions->tree_view_actions), -1, gettext("psi0"), pCellRenderer, "text", 3, NULL);
    pCellRenderer = gtk_cell_renderer_text_new();
    g_object_set(pCellRenderer, "editable", TRUE, NULL);
    g_signal_connect(pCellRenderer, "edited", G_CALLBACK(_1990_gtk_tree_view_actions_psi_edited), projet);
    g_object_set_data(G_OBJECT(pCellRenderer), "column", GINT_TO_POINTER(4));
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(list_gtk_1990_actions->tree_view_actions), -1, gettext("psi1"), pCellRenderer, "text", 4, NULL);
    pCellRenderer = gtk_cell_renderer_text_new();
    g_object_set(pCellRenderer, "editable", TRUE, NULL);
    g_signal_connect(pCellRenderer, "edited", G_CALLBACK(_1990_gtk_tree_view_actions_psi_edited), projet);
    g_object_set_data(G_OBJECT(pCellRenderer), "column", GINT_TO_POINTER(5));
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(list_gtk_1990_actions->tree_view_actions), -1, gettext("psi2"), pCellRenderer, "text", 5, NULL);
    list = gtk_tree_view_get_columns(list_gtk_1990_actions->tree_view_actions);
    g_list_first(list);
    g_list_foreach(list, (GFunc)gtk_tree_view_column_set_resizable, (gpointer)TRUE);
    g_list_free(list);
    column = gtk_tree_view_get_column(list_gtk_1990_actions->tree_view_actions, 2);
    gtk_tree_view_column_set_sizing(column, GTK_TREE_VIEW_COLUMN_FIXED);
    gtk_tree_view_column_set_fixed_width(column, 180);
    g_signal_connect(G_OBJECT(list_gtk_1990_actions->tree_select_actions), "changed", G_CALLBACK(_1990_gtk_actions_select_changed), projet);
    // On cache la colonne où les numéros s'affichent.
    column = gtk_tree_view_get_column(list_gtk_1990_actions->tree_view_actions, 0);
    gtk_tree_view_column_set_visible(column, FALSE);
    
    // Génération de la toolbar pour les actions
    list_gtk_1990_actions->toolbar_actions = gtk_toolbar_new();
    gtk_orientable_set_orientation(GTK_ORIENTABLE(list_gtk_1990_actions->toolbar_actions), GTK_ORIENTATION_HORIZONTAL);
    gtk_toolbar_set_style(GTK_TOOLBAR(list_gtk_1990_actions->toolbar_actions), GTK_TOOLBAR_ICONS);
    gtk_container_set_border_width(GTK_CONTAINER(list_gtk_1990_actions->toolbar_actions), 0);
    gtk_table_attach(GTK_TABLE(list_gtk_1990_actions->table_actions), list_gtk_1990_actions->toolbar_actions, 0, 1, 1, 2, (GtkAttachOptions)(GTK_EXPAND | GTK_FILL), (GtkAttachOptions)(GTK_SHRINK | GTK_FILL), 0, 0);
    list_gtk_1990_actions->img_action_ajout = gtk_image_new_from_stock(GTK_STOCK_ADD, GTK_ICON_SIZE_SMALL_TOOLBAR);
    list_gtk_1990_actions->item_action_ajout = gtk_menu_tool_button_new(list_gtk_1990_actions->img_action_ajout, gettext("Ajouter"));
    list_gtk_1990_actions->menu_type_list_action = gtk_menu_new();
    list_gtk_1990_actions->menu_list_widget_action = list_init();
    for (i=0;i<_1990_action_num_bat_txt(projet->pays);i++)
    {
        w_temp = gtk_menu_item_new_with_label(_1990_action_type_bat_txt(i, projet->pays));
        gtk_menu_shell_append(GTK_MENU_SHELL(list_gtk_1990_actions->menu_type_list_action), w_temp);
        list_insert_after(list_gtk_1990_actions->menu_list_widget_action, &w_temp, sizeof(&w_temp));
        g_signal_connect(w_temp, "activate", G_CALLBACK(_1990_gtk_menu_nouvelle_action_activate), projet);
    }
    gtk_menu_tool_button_set_menu(GTK_MENU_TOOL_BUTTON(list_gtk_1990_actions->item_action_ajout), list_gtk_1990_actions->menu_type_list_action);
    gtk_widget_show_all(list_gtk_1990_actions->menu_type_list_action);
    gtk_toolbar_insert(GTK_TOOLBAR(list_gtk_1990_actions->toolbar_actions), list_gtk_1990_actions->item_action_ajout, -1);
    g_signal_connect(G_OBJECT(list_gtk_1990_actions->item_action_ajout), "clicked", G_CALLBACK(_1990_gtk_menu_nouvelle_action_activate), projet);
    list_gtk_1990_actions->img_action_suppr = gtk_image_new_from_stock(GTK_STOCK_DELETE, GTK_ICON_SIZE_SMALL_TOOLBAR);
    list_gtk_1990_actions->item_action_suppr = gtk_tool_button_new(list_gtk_1990_actions->img_action_suppr, gettext("Supprimer"));
    gtk_toolbar_insert(GTK_TOOLBAR(list_gtk_1990_actions->toolbar_actions), list_gtk_1990_actions->item_action_suppr, -1);
    g_signal_connect(G_OBJECT(list_gtk_1990_actions->item_action_suppr), "clicked", G_CALLBACK(_1990_gtk_menu_suppr_action_activate), projet);
    gtk_widget_set_sensitive(GTK_WIDGET(list_gtk_1990_actions->item_action_suppr), FALSE);
    
    
    
    // Le coté charges
    list_gtk_1990_actions->table_charges = gtk_table_new(2, 1, FALSE);
    gtk_paned_add2(GTK_PANED(list_gtk_1990_actions->paned), list_gtk_1990_actions->table_charges);
    list_gtk_1990_actions->scroll_charges = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(list_gtk_1990_actions->scroll_charges), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    list_gtk_1990_actions->tree_store_charges = gtk_tree_store_new(4, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
    list_gtk_1990_actions->tree_view_charges = (GtkTreeView*)gtk_tree_view_new_with_model(GTK_TREE_MODEL(list_gtk_1990_actions->tree_store_charges));
    list_gtk_1990_actions->tree_select_charges = gtk_tree_view_get_selection(list_gtk_1990_actions->tree_view_charges);
    gtk_tree_selection_set_mode(list_gtk_1990_actions->tree_select_charges, GTK_SELECTION_MULTIPLE);
    gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(list_gtk_1990_actions->scroll_charges), GTK_WIDGET(list_gtk_1990_actions->tree_view_charges));
    gtk_tree_view_set_reorderable(GTK_TREE_VIEW(list_gtk_1990_actions->tree_view_charges), TRUE);
    g_signal_connect(G_OBJECT(list_gtk_1990_actions->tree_view_charges), "button-press-event", G_CALLBACK(_1990_gtk_actions_charge_double_clicked), projet);
    gtk_table_attach(GTK_TABLE(list_gtk_1990_actions->table_charges), list_gtk_1990_actions->scroll_charges, 0, 1, 0, 1, (GtkAttachOptions)(GTK_EXPAND | GTK_FILL), (GtkAttachOptions)(GTK_EXPAND | GTK_FILL), 0, 0);
    // Génération des colonnes
    pCellRenderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(list_gtk_1990_actions->tree_view_charges), -1, gettext("Numéro"), pCellRenderer, "text", 0, NULL);
    pCellRenderer = gtk_cell_renderer_text_new();
    g_object_set(pCellRenderer, "editable", TRUE, NULL);
    g_signal_connect(pCellRenderer, "edited", G_CALLBACK(_1990_gtk_tree_view_charges_cell_edited), projet);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(list_gtk_1990_actions->tree_view_charges), -1, gettext("Description"), pCellRenderer, "text", 1, NULL);
    pCellRenderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(list_gtk_1990_actions->tree_view_charges), -1, gettext("Charge"), pCellRenderer, "text", 2, NULL);
    pCellRenderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(list_gtk_1990_actions->tree_view_charges), -1, gettext("Valeur"), pCellRenderer, "text", 3, NULL);
    list = gtk_tree_view_get_columns(GTK_TREE_VIEW(list_gtk_1990_actions->tree_view_charges));
    g_list_first(list);
    g_list_foreach(list, (GFunc)gtk_tree_view_column_set_resizable, (gpointer)TRUE);
    g_list_free(list);
    g_signal_connect(GTK_WIDGET(list_gtk_1990_actions->tree_view_charges), "drag-begin", G_CALLBACK(_1990_gtk_actions_tree_view_drag_begin), NULL);
    g_signal_connect(GTK_WIDGET(list_gtk_1990_actions->tree_view_actions), "drag-drop", G_CALLBACK(_1990_gtk_actions_tree_view_drag), projet);
    // On cache la colonne avec les numéros
    column = gtk_tree_view_get_column(list_gtk_1990_actions->tree_view_charges, 0);
    gtk_tree_view_column_set_visible(column, FALSE);
    // Génération de la toolbar pour les charges
    list_gtk_1990_actions->toolbar_charges = gtk_toolbar_new();
    gtk_orientable_set_orientation(GTK_ORIENTABLE(list_gtk_1990_actions->toolbar_charges), GTK_ORIENTATION_HORIZONTAL);
    gtk_toolbar_set_style(GTK_TOOLBAR(list_gtk_1990_actions->toolbar_charges), GTK_TOOLBAR_ICONS);
    gtk_container_set_border_width(GTK_CONTAINER(list_gtk_1990_actions->toolbar_charges), 0);
    gtk_table_attach(GTK_TABLE(list_gtk_1990_actions->table_charges), list_gtk_1990_actions->toolbar_charges, 0, 1, 1, 2, (GtkAttachOptions)(GTK_EXPAND | GTK_FILL), (GtkAttachOptions)(GTK_SHRINK | GTK_FILL), 0, 0);
    
    list_gtk_1990_actions->img_charge_ajout = gtk_image_new_from_stock(GTK_STOCK_ADD, GTK_ICON_SIZE_SMALL_TOOLBAR);
    list_gtk_1990_actions->item_charge_ajout = gtk_menu_tool_button_new(list_gtk_1990_actions->img_charge_ajout, gettext("Ajouter"));
    list_gtk_1990_actions->menu_type_list_charge = gtk_menu_new();
    list_gtk_1990_actions->menu_list_widget_charge = list_init();
    
    w_temp = gtk_menu_item_new_with_label(gettext("Charge nodale"));
    gtk_menu_shell_append(GTK_MENU_SHELL(list_gtk_1990_actions->menu_type_list_charge), w_temp);
    list_insert_after(list_gtk_1990_actions->menu_list_widget_charge, &w_temp, sizeof(&w_temp));
    g_signal_connect(w_temp, "activate", G_CALLBACK(_1990_gtk_menu_nouvelle_charge_nodale_activate), projet);
    gtk_menu_tool_button_set_menu(GTK_MENU_TOOL_BUTTON(list_gtk_1990_actions->item_charge_ajout), list_gtk_1990_actions->menu_type_list_charge);
    w_temp = gtk_menu_item_new_with_label(gettext("Charge ponctuelle sur barre"));
    gtk_menu_shell_append(GTK_MENU_SHELL(list_gtk_1990_actions->menu_type_list_charge), w_temp);
    list_insert_after(list_gtk_1990_actions->menu_list_widget_charge, &w_temp, sizeof(&w_temp));
    g_signal_connect(w_temp, "activate", G_CALLBACK(_1990_gtk_menu_nouvelle_charge_barre_ponctuelle_activate), projet);
    gtk_menu_tool_button_set_menu(GTK_MENU_TOOL_BUTTON(list_gtk_1990_actions->item_charge_ajout), list_gtk_1990_actions->menu_type_list_charge);
    w_temp = gtk_menu_item_new_with_label(gettext("Charge répartie uniforme sur barre"));
    gtk_menu_shell_append(GTK_MENU_SHELL(list_gtk_1990_actions->menu_type_list_charge), w_temp);
    list_insert_after(list_gtk_1990_actions->menu_list_widget_charge, &w_temp, sizeof(&w_temp));
    g_signal_connect(w_temp, "activate", G_CALLBACK(_1990_gtk_menu_nouvelle_charge_barre_repartie_uniforme_activate), projet);
    gtk_menu_tool_button_set_menu(GTK_MENU_TOOL_BUTTON(list_gtk_1990_actions->item_charge_ajout), list_gtk_1990_actions->menu_type_list_charge);
    gtk_widget_show_all(list_gtk_1990_actions->menu_type_list_charge);
    gtk_toolbar_insert(GTK_TOOLBAR(list_gtk_1990_actions->toolbar_charges), list_gtk_1990_actions->item_charge_ajout, -1);
    gtk_widget_set_sensitive(GTK_WIDGET(list_gtk_1990_actions->item_charge_ajout), FALSE);
    list_gtk_1990_actions->img_charge_edit = gtk_image_new_from_stock(GTK_STOCK_EDIT, GTK_ICON_SIZE_SMALL_TOOLBAR);
    list_gtk_1990_actions->item_charge_edit = gtk_tool_button_new(list_gtk_1990_actions->img_charge_edit, gettext("Éditer"));
    gtk_toolbar_insert(GTK_TOOLBAR(list_gtk_1990_actions->toolbar_charges), list_gtk_1990_actions->item_charge_edit, -1);
    gtk_widget_set_sensitive(GTK_WIDGET(list_gtk_1990_actions->item_charge_edit), FALSE);
    g_signal_connect(G_OBJECT(list_gtk_1990_actions->item_charge_edit), "clicked", G_CALLBACK(_1990_gtk_menu_edit_charge_clicked), projet);
    list_gtk_1990_actions->img_charge_suppr = gtk_image_new_from_stock(GTK_STOCK_DELETE, GTK_ICON_SIZE_SMALL_TOOLBAR);
    list_gtk_1990_actions->item_charge_suppr = gtk_tool_button_new(list_gtk_1990_actions->img_charge_suppr, gettext("Supprimer"));
    gtk_toolbar_insert(GTK_TOOLBAR(list_gtk_1990_actions->toolbar_charges), list_gtk_1990_actions->item_charge_suppr, -1);
    gtk_widget_set_sensitive(GTK_WIDGET(list_gtk_1990_actions->item_charge_suppr), FALSE);
    g_signal_connect(G_OBJECT(list_gtk_1990_actions->item_charge_suppr), "clicked", G_CALLBACK(_1990_gtk_menu_suppr_charge_clicked), projet);
    g_signal_connect(G_OBJECT(list_gtk_1990_actions->tree_select_charges), "changed", G_CALLBACK(_1990_gtk_actions_select_changed), projet);
    
    
    
    
    /* Défini le comportement du glissé etat vers dispo*/
    gtk_drag_source_set(GTK_WIDGET(list_gtk_1990_actions->tree_view_charges), GDK_BUTTON1_MASK, drag_targets_actions, 1, GDK_ACTION_MOVE); 
    gtk_drag_dest_set(GTK_WIDGET(list_gtk_1990_actions->tree_view_actions), GTK_DEST_DEFAULT_ALL, drag_targets_actions, 1, GDK_ACTION_MOVE);
    
    if (list_size(projet->actions) != 0)
    {
        list_mvfront(projet->actions);
        do
        {
            Action  *action = (Action*)list_curr(projet->actions); 
            
            gtk_tree_store_append(list_gtk_1990_actions->tree_store_actions, &action->Iter, NULL);
            gtk_tree_store_set(list_gtk_1990_actions->tree_store_actions, &action->Iter, 0, action->numero, 1, action->description, 2, _1990_action_type_bat_txt(action->type, projet->pays), 3, action->psi0, 4, action->psi1, 5, action->psi2, -1);
            
            
        } while (list_mvnext(projet->actions) != NULL);
    }
    
    gtk_window_set_modal(GTK_WINDOW(list_gtk_1990_actions->window), TRUE);
    gtk_window_set_transient_for(GTK_WINDOW(list_gtk_1990_actions->window), GTK_WINDOW(projet->list_gtk.comp.window));
    gtk_widget_show_all(list_gtk_1990_actions->window);
    
    return;
}

#endif
