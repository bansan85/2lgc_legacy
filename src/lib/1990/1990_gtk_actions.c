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
    List_Gtk_1990_Actions *list_gtk_1990_actions;
    GtkTreePath   *path;
    GtkTreeIter   iter;
    GtkTreeModel  *model;
    GValue        nouvelle_valeur;
    unsigned int  numero;
    char          *description;
    Action        *action;
    
    BUGMSG(projet, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->actions, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk._1990_actions.window, , gettext("Paramètre incorrect\n"));
    
    // On récupère l'action en cours d'édition
    list_gtk_1990_actions = &projet->list_gtk._1990_actions;
    path = gtk_tree_path_new_from_string(path_string);
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(list_gtk_1990_actions->tree_view_actions));
    
    gtk_tree_model_get_iter (model, &iter, path);
    gtk_tree_model_get(model, &iter, 0, &numero, 1, &description, -1);
    
    BUG(action = _1990_action_cherche_numero(projet, numero), );
    
    // On lui modifie son nom
    action->description = realloc(action->description, sizeof(gchar)*(strlen(new_text)+1));
    BUGMSG(action->description, , gettext("Erreur d'allocation mémoire.\n"));
    strcpy(action->description, new_text);
    
    // et dans le tree-view
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
    List_Gtk_1990_Actions *list_gtk_1990_actions;
    GtkTreePath     *path;
    GtkTreeModel    *model;
    GtkTreeIter     iter, iter_action;
    GtkTreeModel    *model_action;
    GValue          nouvelle_valeur;
    unsigned int    numero_action, numero_charge;
    char            *description;
    Charge_Noeud    *charge;
    
    BUGMSG(projet, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->actions, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk._1990_actions.window, , gettext("Paramètre incorrect\n"));
    
    // On recherche la charge en cours d'édition
    list_gtk_1990_actions = &projet->list_gtk._1990_actions;
    path = gtk_tree_path_new_from_string (path_string);
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(list_gtk_1990_actions->tree_view_charges));
    
    if (!gtk_tree_selection_get_selected(list_gtk_1990_actions->tree_select_actions, &model_action, &iter_action))
        return;
    gtk_tree_model_get(model_action, &iter_action, 0, &numero_action, -1);
    
    gtk_tree_model_get_iter (model, &iter, path);
    gtk_tree_model_get(model, &iter, 0, &numero_charge, 1, &description, -1);
    
    BUG(charge = _1990_action_cherche_charge(projet, numero_action, numero_charge), );
    
    // On lui modifie son nom
    charge->description = realloc(charge->description, sizeof(gchar)*(strlen(new_text)+1));
    BUGMSG(charge->description, , gettext("Erreur d'allocation mémoire.\n"));
    strcpy(charge->description, new_text);
    
    // et dans le tree-view
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
    unsigned int    numero;
    Action          *action;
    
    BUGMSG(projet, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->actions, , gettext("Paramètre incorrect\n"));
    
    list_gtk_1990_actions = &projet->list_gtk._1990_actions;
    
    if (list_gtk_1990_actions->window == NULL) // Sous Windows, cette action est lancée quand la fenêtre est fermée.
        return;
    
    // On récupère l'action sélectionnée.
    if (!gtk_tree_selection_get_selected(list_gtk_1990_actions->tree_select_actions, &model, &iter))
        return;
    gtk_tree_model_get(model, &iter, 0, &numero, -1);
    
    BUG(action = _1990_action_cherche_numero(projet, numero), );
    
    // On actualise la liste des charges
    gtk_tree_store_clear(list_gtk_1990_actions->tree_store_charges);
    if (action->charges != NULL)
    {
        GList   *list_parcours = action->charges;
        
        do
        {
            Charge_Barre_Ponctuelle *charge_tmp = list_parcours->data;
            switch (charge_tmp->type)
            {
                case CHARGE_NOEUD :
                {
                    Charge_Noeud *charge = list_parcours->data;
                    
                    BUG(EF_gtk_charge_noeud_ajout_affichage(charge, projet, TRUE) == 0, );
                    
                    break;
                }
                case CHARGE_BARRE_PONCTUELLE :
                {
                    Charge_Barre_Ponctuelle *charge = list_parcours->data;
                    
                    BUG(EF_gtk_charge_barre_ponctuelle_ajout_affichage(charge, projet, TRUE) == 0, );
                    
                    break;
                }
                case CHARGE_BARRE_REPARTIE_UNIFORME :
                {
                    Charge_Barre_Repartie_Uniforme *charge = list_parcours->data;
                    
                    BUG(EF_gtk_charge_barre_repartie_uniforme_ajout_affichage(charge, projet, TRUE) == 0, );
                    
                    break;
                }
                default :
                {
                    BUGMSG(0, , gettext("Paramètre incorrect\n"));
                    break;
                }
            }
            list_parcours = g_list_next(list_parcours);
        } while (list_parcours != NULL);
    }
    
    return;
}


void _1990_gtk_actions_tree_view_drag_begin(GtkWidget *widget __attribute__((unused)), 
  GdkDragContext *drag_context __attribute__((unused)), void *data __attribute__((unused)))
{
    return;
}


gboolean _1990_gtk_actions_tree_view_drag(GtkWidget *widget,
  GdkDragContext *drag_context __attribute__((unused)), gint x, gint y,
  guint tim __attribute__((unused)), Projet *projet)
/* Description : Charge d'action la charge sélectionnée.
 * Paramètres : GtkWidget *button : composant ayant réalisé l'évènement,
 *            : GdkDragContext *drag_context : inutile,
 *            : gint x, gint y : coordonnées du relachement de la souris,
 *            : guint tim : inutile,
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : FALSE
 */
{
    gint                    cx, cy;
    List_Gtk_1990_Actions   *list_gtk_1990_actions = &projet->list_gtk._1990_actions;
    GtkTreePath             *path;
    GtkTreeModel            *list_store;
    
    BUGMSG(projet, FALSE, gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk._1990_actions.window, FALSE, gettext("Paramètre incorrect\n"));
    
    // On récupère la ligne de la nouvelle action des charges sélectionnées
    gdk_window_get_geometry(gtk_tree_view_get_bin_window(GTK_TREE_VIEW(list_gtk_1990_actions->tree_view_actions)), &cx, &cy, NULL, NULL);
    gtk_tree_view_get_path_at_pos(GTK_TREE_VIEW(widget), x -=cx, y -=cy, &path, NULL, &cx, &cy);
    list_store = gtk_tree_view_get_model(GTK_TREE_VIEW(list_gtk_1990_actions->tree_view_actions));
    if (path != NULL)
    {
        unsigned int    num_action_dest,  num_charge_source,  num_action_source;
        GtkTreeIter     iter_action_dest, iter_charge_source, iter_action_source;
        GtkTreeModel    *model_charge_source, *model_action_source;
        GList           *list, *list_fixe, *list_parcours;
        
        // On récupère le numéro de l'action de destination
        gtk_tree_model_get_iter(list_store, &iter_action_dest, path);
        gtk_tree_model_get(list_store, &iter_action_dest, 0, &num_action_dest, -1);
        
        // On récupère le numéro de l'action actuelle
        if (!gtk_tree_selection_get_selected(list_gtk_1990_actions->tree_select_actions, &model_action_source, &iter_action_source))
        {
            gtk_tree_path_free(path);
            return FALSE;
        }
        gtk_tree_model_get(model_action_source, &iter_action_source, 0, &num_action_source, -1);
        
        if (num_action_dest == num_action_source)
        {
            gtk_tree_path_free(path);
            return FALSE;
        }
        
        list = gtk_tree_selection_get_selected_rows(list_gtk_1990_actions->tree_select_charges, &model_charge_source);
        // On converti les lignes en ligne fixe sinon, le tree-view-charges se perd lorsqu'on supprime les lignes dues au déplacement des actions en cours de route.
        list_parcours = g_list_last(list);
        list_fixe = NULL;
        for(;list_parcours != NULL; list_parcours = g_list_previous(list_parcours))
            list_fixe = g_list_append(list_fixe, gtk_tree_row_reference_new(model_charge_source, (GtkTreePath*)list_parcours->data));
        g_list_foreach(list, (GFunc)gtk_tree_path_free, NULL);
        g_list_free(list);
        
        // On déplace les charges, charge par charge dans leur nouvelle action;
        list_parcours = g_list_last(list_fixe);
        for(;list_parcours != NULL; list_parcours = g_list_previous(list_parcours))
        {
            if (gtk_tree_model_get_iter(model_charge_source, &iter_charge_source, gtk_tree_row_reference_get_path((GtkTreeRowReference*)list_parcours->data)))
            {
                gtk_tree_model_get(model_charge_source, &iter_charge_source, 0, &num_charge_source, -1);
                BUG(_1990_action_deplace_charge(projet, num_action_source, num_charge_source, num_action_dest) == 0, FALSE);
            }
        }
        g_list_foreach(list_fixe, (GFunc)gtk_tree_row_reference_free, NULL);
        g_list_free(list_fixe);
        
        gtk_tree_path_free(path);
    }
    
    return FALSE;
}


void _1990_gtk_tree_view_actions_type_edited(GtkCellRendererText *cell __attribute__((unused)), const gchar *path_string, const gchar *new_text, Projet *projet)
/* Description : Changement du type (psi0, psi1 et psi2 y compris) d'une action
 * Paramètres : GtkCellRendererText *cell : cellule en cours,
 *            : gchar *path_string : path de la ligne en cours,
 *            : gchar *new_text : nouveau coefficient psi0,
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
*/
{
    List_Gtk_1990_Actions   *list_gtk_1990_actions;
    GtkTreeModel            *model;
    GtkTreePath             *path;
    GtkTreeIter             iter;
    unsigned int            i, j;
    Action                  *action;
    
    BUGMSG(projet, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->actions, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk._1990_actions.window, , gettext("Paramètre incorrect\n"));
    
    list_gtk_1990_actions = &projet->list_gtk._1990_actions;
    model = GTK_TREE_MODEL(list_gtk_1990_actions->tree_store_actions);
    path = gtk_tree_path_new_from_string (path_string);
    
    gtk_tree_model_get_iter(model, &iter, path);
    gtk_tree_model_get(model, &iter, 0, &i, -1);
    
    for (j=0;j<_1990_action_num_bat_txt(projet->pays);j++)
    {
        if (strcmp(new_text, _1990_action_type_bat_txt(j, projet->pays)) == 0)
            break;
    }
    BUGMSG(j != _1990_action_num_bat_txt(projet->pays), , gettext("Paramètre incorrect\n"));
    BUG(action = _1990_action_cherche_numero(projet, i), );
    action->type = j;
    action->psi0 = _1990_coef_psi0_bat(j, projet->pays);
    action->psi1 = _1990_coef_psi1_bat(j, projet->pays);
    action->psi2 = _1990_coef_psi2_bat(j, projet->pays);
    gtk_tree_store_set(list_gtk_1990_actions->tree_store_actions, &iter, 2, new_text, 3, action->psi0, 4, action->psi1, 5, action->psi2, -1);
    
    gtk_tree_path_free(path);
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
    List_Gtk_1990_Actions   *list_gtk_1990_actions;
    GtkTreeModel            *model;
    GtkTreePath             *path;
    GtkTreeIter             iter;
    unsigned int            i;
    char                    *fake;
    double                  convertion;
    Action                  *action;
    gint                    column = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(cell), "column"));
    
    BUGMSG(projet, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->actions, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk._1990_actions.window, , gettext("Paramètre incorrect\n"));
    fake = (char*)malloc(sizeof(char)*(strlen(new_text)+1));
    BUGMSG(fake, , gettext("Erreur d'allocation mémoire.\n"));
    
    list_gtk_1990_actions = &projet->list_gtk._1990_actions;
    model = GTK_TREE_MODEL(list_gtk_1990_actions->tree_store_actions);
    path = gtk_tree_path_new_from_string(path_string);
    
    gtk_tree_model_get_iter(model, &iter, path);
    gtk_tree_model_get(model, &iter, 0, &i, -1);
    
    // On vérifie si le texte contient bien un nombre flottant
    if (sscanf(new_text, "%lf%s", &convertion, fake) == 1)
    {
        // On modifie le tree-view-action
        gtk_tree_store_set(list_gtk_1990_actions->tree_store_actions, &iter, column, convertion, -1);
        
        // On modifie l'action
        BUG(action = _1990_action_cherche_numero(projet, i), );
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
                BUGMSG(NULL, , gettext("Paramètre incorrect\n"));
                break;
            }
        }
    }
    
    free(fake);
    gtk_tree_path_free(path);
     
    return;
}


void _1990_gtk_menu_nouvelle_action_activate(GtkMenuItem *menuitem, Projet *projet)
/* Description : Ajout d'une nouvelle action depuis le menu de la barre d'outils
 * Paramètres : GtkMenuItem *menuitem : composant à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    List_Gtk_1990_Actions   *list_gtk_1990_actions;
    unsigned int            i = 0;
    GList                   *list_parcours;
    
    BUGMSG(projet, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->actions, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk._1990_actions.window, , gettext("Paramètre incorrect\n"));
    
    list_gtk_1990_actions = &projet->list_gtk._1990_actions;
    
    BUGMSG(g_list_length(list_gtk_1990_actions->menu_list_widget_action) != 0, , gettext("Paramètre incorrect\n"));
    
    list_parcours = list_gtk_1990_actions->menu_list_widget_action;
    do
    {
        if ((GTK_IS_MENU_TOOL_BUTTON(menuitem)) || ((GtkMenuItem *)list_parcours->data == menuitem))
        {
            char        *tmp;
            Action      *action;
            GtkTreePath *path;
            
            BUGMSG(tmp = g_strdup_printf("%s %u", gettext("Sans nom"), g_list_length(projet->actions)), , gettext("Erreur d'allocation mémoire.\n"));
            // On crée l'action en fonction de la catégorie sélectionnée dans le menu déroulant.
            BUG(action = _1990_action_ajout(projet, i, tmp), );
            free(tmp);
            
            // On actualise l'affichage
            gtk_tree_store_append(list_gtk_1990_actions->tree_store_actions, &action->Iter, NULL);
            gtk_tree_store_set(list_gtk_1990_actions->tree_store_actions, &action->Iter, 0, action->numero, 1, action->description, 2, _1990_action_type_bat_txt(action->type, projet->pays), 3, action->psi0, 4, action->psi1, 5, action->psi2, -1);
            path = gtk_tree_model_get_path(GTK_TREE_MODEL(list_gtk_1990_actions->tree_store_actions), &action->Iter);
            gtk_tree_view_set_cursor(GTK_TREE_VIEW(list_gtk_1990_actions->tree_view_actions), path, gtk_tree_view_get_column(GTK_TREE_VIEW(list_gtk_1990_actions->tree_view_actions), 1), TRUE);
            gtk_tree_path_free(path);
            
            return;
        }
        i++;
        list_parcours = g_list_next(list_parcours);
    } while (list_parcours != NULL);
    
    BUGMSG(NULL, , gettext("Paramètre incorrect\n"));
}


void _1990_gtk_menu_suppr_action_activate(GtkWidget *toolbutton __attribute__((unused)), Projet *projet)
/* Description : Supprimer l'action sélectionnée
 * Paramètres : GtkToolButton *toolbutton : composant à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    List_Gtk_1990_Actions *list_gtk_1990_actions = &projet->list_gtk._1990_actions;
    GtkTreeIter     iter;
    GtkTreeModel    *model;
    unsigned int    numero_action;
    
    BUGMSG(projet, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk._1990_actions.window, , gettext("Paramètre incorrect\n"));
    
    list_gtk_1990_actions = &projet->list_gtk._1990_actions;
    
    if (!gtk_tree_selection_get_selected(list_gtk_1990_actions->tree_select_actions, &model, &iter))
        return;
    
    // On récupère l'action sélectionnée
    gtk_tree_model_get(model, &iter, 0, &numero_action, -1);
    
    // Et on la supprime ainsi que les charges la contenant
    gtk_tree_store_remove(list_gtk_1990_actions->tree_store_actions, &iter);
    gtk_tree_store_clear(list_gtk_1990_actions->tree_store_charges);
    BUG(_1990_action_free_num(projet, numero_action) == 0, );
    if (projet->actions != NULL)
    {
        GList   *list_parcours = projet->actions;
        
        do
        {
            Action *action = list_parcours->data;
            
            // On met à jour les actions qui ont eu leur numérotation modifiée.
            if (action->numero >= numero_action)
                gtk_tree_store_set(list_gtk_1990_actions->tree_store_actions, &action->Iter, 0, action->numero, -1);
            
            list_parcours = g_list_next(list_parcours);
        } while (list_parcours != NULL);
    }
    return;
}


void _1990_gtk_menu_nouvelle_charge_nodale_activate(GtkMenuItem *menuitem __attribute__((unused)), Projet* projet)
/* Description : Ouvre la fenêtre permettant d'ajouter une charge nodale.
 * Paramètres : GtkMenuItem *menuitem : composant à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    GtkTreeIter     iter_action;
    GtkTreeModel    *model_action;
    unsigned int    numero_action;
    
    BUGMSG(projet, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->actions, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk._1990_actions.window, , gettext("Paramètre incorrect\n"));
    
    if (!gtk_tree_selection_get_selected(projet->list_gtk._1990_actions.tree_select_actions, &model_action, &iter_action))
        return;
    gtk_tree_model_get(model_action, &iter_action, 0, &numero_action, -1);
    BUG(EF_gtk_charge_noeud(projet, numero_action, G_MAXUINT) == 0, );
}


void _1990_gtk_menu_nouvelle_charge_barre_ponctuelle_activate(GtkMenuItem *menuitem __attribute__((unused)), Projet* projet)
/* Description : Ouvre la fenêtre permettant d'ajouter une charge ponctuelle sur barre.
 * Paramètres : GtkMenuItem *menuitem : composant à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    GtkTreeIter                 iter_action;
    GtkTreeModel                *model_action;
    unsigned int                numero_action;
    
    BUGMSG(projet, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->actions, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk._1990_actions.window, , gettext("Paramètre incorrect\n"));
    
    if (!gtk_tree_selection_get_selected(projet->list_gtk._1990_actions.tree_select_actions, &model_action, &iter_action))
        return;
    gtk_tree_model_get(model_action, &iter_action, 0, &numero_action, -1);
    BUG(EF_gtk_charge_barre_ponctuelle(projet, numero_action, G_MAXUINT) == 0, );
    
    return;
}


void _1990_gtk_menu_nouvelle_charge_barre_repartie_uniforme_activate(GtkMenuItem *menuitem __attribute__((unused)), Projet* projet)
/* Description : Ouvre la fenêtre permettant d'ajouter une charge répartie uniforme.
 * Paramètres : GtkMenuItem *menuitem : composant à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    GtkTreeIter                 iter_action;
    GtkTreeModel                *model_action;
    unsigned int                numero_action;
    
    BUGMSG(projet, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->actions, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk._1990_actions.window, , gettext("Paramètre incorrect\n"));
    
    if (!gtk_tree_selection_get_selected(projet->list_gtk._1990_actions.tree_select_actions, &model_action, &iter_action))
        return;
    gtk_tree_model_get(model_action, &iter_action, 0, &numero_action, -1);
    BUG(EF_gtk_charge_barre_repartie_uniforme(projet, numero_action, G_MAXUINT) == 0, );
    
    return;
}


void _1990_gtk_actions_select_changed(GtkTreeSelection *treeselection __attribute__((unused)), Projet *projet)
/* Description : Réajuste automatiquement la propriété sensitive en fonction de la sélection dans les tree-views
 * Paramètres : GtkMenuItem *menuitem : composant à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    List_Gtk_1990_Actions   *list_gtk_1990_actions;
    
    BUGMSG(projet, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk._1990_actions.window, , gettext("Paramètre incorrect\n"));
    
    list_gtk_1990_actions = &projet->list_gtk._1990_actions;
    
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


void _1990_gtk_menu_suppr_charge_clicked(GtkWidget *toolbutton __attribute__((unused)), Projet *projet)
/* Description : Supprimer les actions sélectionnées
 * Paramètres : GtkToolButton *toolbutton : composant à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    List_Gtk_1990_Actions *list_gtk_1990_actions;
    GtkTreeIter     iter;
    GtkTreeModel    *model;
    unsigned int    numero_action, numero_charge;
    GList           *list, *list_fixe, *list_parcours;
    
    BUGMSG(projet, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->actions, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk._1990_actions.window, , gettext("Paramètre incorrect\n"));
    
    list_gtk_1990_actions = &projet->list_gtk._1990_actions;
    
    // On récupère le numéro de l'action qui contient les charges à supprimer.
    if (!gtk_tree_selection_get_selected(list_gtk_1990_actions->tree_select_actions, &model, &iter))
        return;
    gtk_tree_model_get(model, &iter, 0, &numero_action, -1);
    
    // On récupère la liste des charges à supprimer.
    list = gtk_tree_selection_get_selected_rows(list_gtk_1990_actions->tree_select_charges, &model);
    // On converti les lignes en ligne fixe sinon, le tree-view-charges se perd lorsqu'on supprime les lignes dues au déplacement des actions en cours de route.
    list_parcours = g_list_last(list);
    list_fixe = NULL;
    for(;list_parcours != NULL; list_parcours = g_list_previous(list_parcours))
        list_fixe = g_list_append(list_fixe, gtk_tree_row_reference_new(model, (GtkTreePath*)list_parcours->data));
    g_list_foreach(list, (GFunc)gtk_tree_path_free, NULL);
    g_list_free(list);
    
    // On supprime les charges sélectionnées. Pas besoin de remettre à jour le tree-view, c'est inclus dans _1990_action_supprime_charge
    list_parcours = g_list_first(list_fixe);
    for(;list_parcours != NULL; list_parcours = g_list_next(list_parcours))
    {
        if (gtk_tree_model_get_iter(model, &iter, gtk_tree_row_reference_get_path((GtkTreeRowReference*)list_parcours->data)))
        {
            gtk_tree_model_get(model, &iter, 0, &numero_charge, -1);
            BUG(_1990_action_supprime_charge(projet, numero_action, numero_charge) == 0, );
        }
    }
    g_list_foreach(list_fixe, (GFunc)gtk_tree_row_reference_free, NULL);
    g_list_free(list_fixe);
    
    return;
}


void _1990_gtk_menu_edit_charge_clicked(GtkToolButton *toolbutton __attribute__((unused)), Projet *projet)
/* Description : Edite les charges sélectionnées
 * Paramètres : GtkToolButton *toolbutton : composant à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    List_Gtk_1990_Actions *list_gtk_1990_actions;
    GtkTreeIter     iter;
    GtkTreeModel    *model;
    unsigned int    numero_action, numero_charge;
    GList           *list, *list_parcours;
    
    BUGMSG(projet, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->actions, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk._1990_actions.window, , gettext("Paramètre incorrect\n"));
    
    list_gtk_1990_actions = &projet->list_gtk._1990_actions;
    
    // On récupère le numéro de l'action qui contient les charges à éditer.
    if (!gtk_tree_selection_get_selected(list_gtk_1990_actions->tree_select_actions, &model, &iter))
        return;
    gtk_tree_model_get(model, &iter, 0, &numero_action, -1);
    
    // On récupère la liste des charges à éditer.
    list = gtk_tree_selection_get_selected_rows(list_gtk_1990_actions->tree_select_charges, &model);
    list_parcours = g_list_first(list);
    for(;list_parcours != NULL; list_parcours = g_list_next(list_parcours))
    {
        if (gtk_tree_model_get_iter(model, &iter, (GtkTreePath*)list_parcours->data))
        {
    // Et on les édite les unes après les autres.
            Charge_Noeud    *charge_noeud;
            gtk_tree_model_get(model, &iter, 0, &numero_charge, -1);
            charge_noeud = _1990_action_cherche_charge(projet, numero_action, numero_charge);
            BUG(charge_noeud, );
            switch (charge_noeud->type)
            {
                case CHARGE_NOEUD :
                {
                    BUG(EF_gtk_charge_noeud(projet, numero_action, numero_charge) == 0, );
                    break;
                }
                case CHARGE_BARRE_PONCTUELLE :
                {
                    BUG(EF_gtk_charge_barre_ponctuelle(projet, numero_action, numero_charge) == 0, );
                    break;
                }
                case CHARGE_BARRE_REPARTIE_UNIFORME :
                {
                    BUG(EF_gtk_charge_barre_repartie_uniforme(projet, numero_action, numero_charge) == 0, );
                    break;
                }
                default :
                {
                    BUGMSG(0, , gettext("Type de charge %d inconnu."), charge_noeud->type);
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
/* Description : met projet->list_gtk._1990_actions.window à NULL quand la fenêtre se ferme
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    BUGMSG(projet, , gettext("Paramètre incorrect\n"));
    
    projet->list_gtk._1990_actions.window = NULL;
    return;
}


gboolean _1990_gtk_actions_charge_double_clicked(GtkWidget *widget __attribute__((unused)), GdkEvent *event, Projet *projet)
/* Description : Lance la fenêtre d'édition de la charge sélectionnée en cas de double-clique dans le tree-view
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : GdkEvent *event : Information sur l'évènement,
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    BUGMSG(projet, FALSE, gettext("Paramètre incorrect\n"));
    
    if ((event->type == GDK_2BUTTON_PRESS) && (gtk_widget_get_sensitive(GTK_WIDGET(projet->list_gtk._1990_actions.item_charge_edit))))
        _1990_gtk_menu_edit_charge_clicked(NULL, projet);
    return FALSE;
}


gboolean _1990_gtk_tree_view_actions_key_press_event(GtkWidget *widget, GdkEvent *event, Projet *projet)
{
    BUGMSG(projet, FALSE, gettext("Paramètre incorrect\n"));
    if (event->key.keyval == GDK_KEY_Delete)
        _1990_gtk_menu_suppr_action_activate(widget, projet);
    return FALSE; /* Pour permettre aux autres touches d'être fonctionnelles  */
}


gboolean _1990_gtk_actions_charge_key_press_event(GtkWidget *widget, GdkEvent *event, Projet *projet)
{
    BUGMSG(projet, FALSE, gettext("Paramètre incorrect\n"));
    if (event->key.keyval == GDK_KEY_Delete)
        _1990_gtk_menu_suppr_charge_clicked(widget, projet);
    return FALSE; /* Pour permettre aux autres touches d'être fonctionnelles  */
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
    unsigned int            i;
    GtkWidget               *w_temp;
    GtkTreeViewColumn       *column;
    
    BUGMSG(projet, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->actions, , gettext("Paramètre incorrect\n"));
    
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
    list_gtk_1990_actions->tree_store_actions = gtk_tree_store_new(6, G_TYPE_UINT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_DOUBLE, G_TYPE_DOUBLE, G_TYPE_DOUBLE);
    list_gtk_1990_actions->tree_view_actions = (GtkTreeView*)gtk_tree_view_new_with_model(GTK_TREE_MODEL(list_gtk_1990_actions->tree_store_actions));
    list_gtk_1990_actions->tree_select_actions = gtk_tree_view_get_selection(list_gtk_1990_actions->tree_view_actions);
    gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(list_gtk_1990_actions->scroll_actions), GTK_WIDGET(list_gtk_1990_actions->tree_view_actions));
    g_signal_connect(G_OBJECT(list_gtk_1990_actions->tree_view_actions), "cursor-changed", G_CALLBACK(_1990_gtk_tree_view_actions_cursor_changed), projet);
    g_signal_connect(G_OBJECT(list_gtk_1990_actions->tree_view_actions), "key-press-event", G_CALLBACK(_1990_gtk_tree_view_actions_key_press_event), projet);
    g_signal_connect(G_OBJECT(list_gtk_1990_actions->tree_select_actions), "changed", G_CALLBACK(_1990_gtk_actions_select_changed), projet);
    gtk_table_attach(GTK_TABLE(list_gtk_1990_actions->table_actions), list_gtk_1990_actions->scroll_actions, 0, 1, 0, 1, (GtkAttachOptions)(GTK_EXPAND | GTK_FILL), (GtkAttachOptions)(GTK_EXPAND | GTK_FILL), 0, 0);
    // Colonne numéro
    pCellRenderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(list_gtk_1990_actions->tree_view_actions), -1, gettext("Numéro"), pCellRenderer, "text", 0, NULL);
    // Colonne description
    pCellRenderer = gtk_cell_renderer_text_new();
    g_object_set(pCellRenderer, "editable", TRUE, NULL);
    g_signal_connect(pCellRenderer, "edited", G_CALLBACK(_1990_gtk_tree_view_actions_cell_edited), projet);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(list_gtk_1990_actions->tree_view_actions), -1, gettext("Description"), pCellRenderer, "text", 1, NULL);
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
    list_gtk_1990_actions->menu_list_widget_action = NULL;
    for (i=0;i<_1990_action_num_bat_txt(projet->pays);i++)
    {
        w_temp = gtk_menu_item_new_with_label(_1990_action_type_bat_txt(i, projet->pays));
        gtk_menu_shell_append(GTK_MENU_SHELL(list_gtk_1990_actions->menu_type_list_action), w_temp);
        list_gtk_1990_actions->menu_list_widget_action = g_list_append(list_gtk_1990_actions->menu_list_widget_action, w_temp);
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
    list_gtk_1990_actions->tree_store_charges = gtk_tree_store_new(4, G_TYPE_UINT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
    list_gtk_1990_actions->tree_view_charges = (GtkTreeView*)gtk_tree_view_new_with_model(GTK_TREE_MODEL(list_gtk_1990_actions->tree_store_charges));
    list_gtk_1990_actions->tree_select_charges = gtk_tree_view_get_selection(list_gtk_1990_actions->tree_view_charges);
    gtk_tree_selection_set_mode(list_gtk_1990_actions->tree_select_charges, GTK_SELECTION_MULTIPLE);
    gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(list_gtk_1990_actions->scroll_charges), GTK_WIDGET(list_gtk_1990_actions->tree_view_charges));
    gtk_tree_view_set_reorderable(GTK_TREE_VIEW(list_gtk_1990_actions->tree_view_charges), TRUE);
    g_signal_connect(G_OBJECT(list_gtk_1990_actions->tree_view_charges), "button-press-event", G_CALLBACK(_1990_gtk_actions_charge_double_clicked), projet);
    g_signal_connect(G_OBJECT(list_gtk_1990_actions->tree_view_charges), "key-press-event", G_CALLBACK(_1990_gtk_actions_charge_key_press_event), projet);
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
    list_gtk_1990_actions->menu_list_widget_charge = NULL;
    
    w_temp = gtk_menu_item_new_with_label(gettext("Charge nodale"));
    gtk_menu_shell_append(GTK_MENU_SHELL(list_gtk_1990_actions->menu_type_list_charge), w_temp);
    list_gtk_1990_actions->menu_list_widget_charge = g_list_append(list_gtk_1990_actions->menu_list_widget_charge, w_temp);
    g_signal_connect(w_temp, "activate", G_CALLBACK(_1990_gtk_menu_nouvelle_charge_nodale_activate), projet);
    gtk_menu_tool_button_set_menu(GTK_MENU_TOOL_BUTTON(list_gtk_1990_actions->item_charge_ajout), list_gtk_1990_actions->menu_type_list_charge);
    w_temp = gtk_menu_item_new_with_label(gettext("Charge ponctuelle sur barre"));
    gtk_menu_shell_append(GTK_MENU_SHELL(list_gtk_1990_actions->menu_type_list_charge), w_temp);
    list_gtk_1990_actions->menu_list_widget_charge = g_list_append(list_gtk_1990_actions->menu_list_widget_charge, w_temp);
    g_signal_connect(w_temp, "activate", G_CALLBACK(_1990_gtk_menu_nouvelle_charge_barre_ponctuelle_activate), projet);
    gtk_menu_tool_button_set_menu(GTK_MENU_TOOL_BUTTON(list_gtk_1990_actions->item_charge_ajout), list_gtk_1990_actions->menu_type_list_charge);
    w_temp = gtk_menu_item_new_with_label(gettext("Charge répartie uniforme sur barre"));
    gtk_menu_shell_append(GTK_MENU_SHELL(list_gtk_1990_actions->menu_type_list_charge), w_temp);
    list_gtk_1990_actions->menu_list_widget_charge = g_list_append(list_gtk_1990_actions->menu_list_widget_charge, w_temp);
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
    
    // Affiche la liste des actions
    if (projet->actions != NULL)
    {
        GList   *list_parcours = projet->actions;
        
        do
        {
            Action  *action = list_parcours->data;
            
            gtk_tree_store_append(list_gtk_1990_actions->tree_store_actions, &action->Iter, NULL);
            gtk_tree_store_set(list_gtk_1990_actions->tree_store_actions, &action->Iter, 0, action->numero, 1, action->description, 2, _1990_action_type_bat_txt(action->type, projet->pays), 3, action->psi0, 4, action->psi1, 5, action->psi2, -1);
            
            list_parcours = g_list_next(list_parcours);
        } while (list_parcours != NULL);
    }
    
    gtk_window_set_modal(GTK_WINDOW(list_gtk_1990_actions->window), TRUE);
    gtk_window_set_transient_for(GTK_WINDOW(list_gtk_1990_actions->window), GTK_WINDOW(projet->list_gtk.comp.window));
    gtk_widget_show_all(list_gtk_1990_actions->window);
    
    return;
}

#endif
