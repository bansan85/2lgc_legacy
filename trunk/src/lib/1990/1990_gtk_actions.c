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

G_MODULE_EXPORT void _1990_gtk_tree_view_actions_cell_edited(GtkCellRendererText *cell __attribute__((unused)), gchar *path_string, gchar *new_text, Projet *projet)
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
    BUGMSG(projet->list_gtk._1990_actions.builder, , gettext("Paramètre incorrect\n"));
    
    // On récupère l'action en cours d'édition
    list_gtk_1990_actions = &projet->list_gtk._1990_actions;
    path = gtk_tree_path_new_from_string(path_string);
    model = GTK_TREE_MODEL(list_gtk_1990_actions->tree_store_actions);
    
    gtk_tree_model_get_iter(model, &iter, path);
    gtk_tree_model_get(model, &iter, 0, &numero, 1, &description, -1);
    
    BUG(action = _1990_action_cherche_numero(projet, numero), );
    
    // On lui modifie son nom
    action->description = realloc(action->description, sizeof(gchar)*(strlen(new_text)+1));
    BUGMSG(action->description, , gettext("Erreur d'allocation mémoire.\n"));
    strcpy(action->description, new_text);
    
    // et dans le tree-view
    memset(&nouvelle_valeur, 0, sizeof(nouvelle_valeur));
    g_value_init(&nouvelle_valeur, G_TYPE_STRING);
    g_value_set_string(&nouvelle_valeur, new_text);
    gtk_tree_store_set_value(GTK_TREE_STORE(model), &iter, 1, &nouvelle_valeur);
    
    gtk_tree_path_free(path);
    
    return;
}


G_MODULE_EXPORT void _1990_gtk_tree_view_charges_cell_edited(GtkCellRendererText *cell __attribute__((unused)), gchar *path_string, gchar *new_text, Projet *projet)
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
    BUGMSG(projet->list_gtk._1990_actions.builder, , gettext("Paramètre incorrect\n"));
    
    // On recherche la charge en cours d'édition
    list_gtk_1990_actions = &projet->list_gtk._1990_actions;
    path = gtk_tree_path_new_from_string(path_string);
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(list_gtk_1990_actions->tree_view_charges));
    
    if (!gtk_tree_selection_get_selected(list_gtk_1990_actions->tree_select_actions, &model_action, &iter_action))
        return;
    gtk_tree_model_get(model_action, &iter_action, 0, &numero_action, -1);
    
    gtk_tree_model_get_iter(model, &iter, path);
    gtk_tree_model_get(model, &iter, 0, &numero_charge, 1, &description, -1);
    
    BUG(charge = _1990_action_cherche_charge(projet, numero_action, numero_charge), );
    
    // On lui modifie son nom
    charge->description = realloc(charge->description, sizeof(gchar)*(strlen(new_text)+1));
    BUGMSG(charge->description, , gettext("Erreur d'allocation mémoire.\n"));
    strcpy(charge->description, new_text);
    
    // et dans le tree-view
    memset(&nouvelle_valeur, 0, sizeof(nouvelle_valeur));
    g_value_init(&nouvelle_valeur, G_TYPE_STRING);
    g_value_set_string(&nouvelle_valeur, new_text);
    gtk_tree_store_set_value(list_gtk_1990_actions->tree_store_charges, &iter, 1, &nouvelle_valeur);
    
    gtk_tree_path_free(path);
    
    return;
}


G_MODULE_EXPORT void _1990_gtk_tree_view_actions_cursor_changed(GtkTreeView *tree_view __attribute__((unused)), Projet *projet)
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
    
    if (projet->list_gtk._1990_actions.builder == NULL)
        return;
    
    list_gtk_1990_actions = &projet->list_gtk._1990_actions;
    
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


G_MODULE_EXPORT void _1990_gtk_actions_tree_view_drag_begin(GtkWidget *widget __attribute__((unused)), 
  GdkDragContext *drag_context __attribute__((unused)), void *data __attribute__((unused)))
{
    return;
}


G_MODULE_EXPORT gboolean _1990_gtk_actions_tree_view_drag(GtkWidget *widget,
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
    BUGMSG(projet->list_gtk._1990_actions.builder, FALSE, gettext("Paramètre incorrect\n"));
    
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


G_MODULE_EXPORT void _1990_gtk_tree_view_actions_type_edited(GtkCellRendererText *cell __attribute__((unused)), const gchar *path_string, const gchar *new_text, Projet *projet)
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
    BUGMSG(projet->list_gtk._1990_actions.builder, , gettext("Paramètre incorrect\n"));
    
    list_gtk_1990_actions = &projet->list_gtk._1990_actions;
    model = GTK_TREE_MODEL(list_gtk_1990_actions->tree_store_actions);
    path = gtk_tree_path_new_from_string(path_string);
    
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


G_MODULE_EXPORT void _1990_gtk_tree_view_actions_psi_edited(GtkCellRendererText *cell, gchar *path_string, gchar *new_text, Projet *projet)
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
    BUGMSG(projet->list_gtk._1990_actions.builder, , gettext("Paramètre incorrect\n"));
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


G_MODULE_EXPORT void _1990_gtk_menu_nouvelle_action_activate(GtkMenuItem *menuitem, Projet *projet)
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
    BUGMSG(projet->list_gtk._1990_actions.builder, , gettext("Paramètre incorrect\n"));
    
    list_gtk_1990_actions = &projet->list_gtk._1990_actions;
    
    BUGMSG(list_gtk_1990_actions->menu_list_widget_action, , gettext("Paramètre incorrect\n"));
    
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


G_MODULE_EXPORT void _1990_gtk_menu_suppr_action_activate(GtkWidget *toolbutton __attribute__((unused)), Projet *projet)
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
    BUGMSG(projet->list_gtk._1990_actions.builder, , gettext("Paramètre incorrect\n"));
    
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
    BUGMSG(projet->list_gtk._1990_actions.builder, , gettext("Paramètre incorrect\n"));
    
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
    BUGMSG(projet->list_gtk._1990_actions.builder, , gettext("Paramètre incorrect\n"));
    
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
    BUGMSG(projet->list_gtk._1990_actions.builder, , gettext("Paramètre incorrect\n"));
    
    if (!gtk_tree_selection_get_selected(projet->list_gtk._1990_actions.tree_select_actions, &model_action, &iter_action))
        return;
    gtk_tree_model_get(model_action, &iter_action, 0, &numero_action, -1);
    BUG(EF_gtk_charge_barre_repartie_uniforme(projet, numero_action, G_MAXUINT) == 0, );
    
    return;
}


G_MODULE_EXPORT void _1990_gtk_actions_select_changed(GtkTreeSelection *treeselection __attribute__((unused)), Projet *projet)
/* Description : Réajuste automatiquement la propriété sensitive en fonction de la sélection dans les tree-views
 * Paramètres : GtkMenuItem *menuitem : composant à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    List_Gtk_1990_Actions   *list_gtk_1990_actions;
    
    BUGMSG(projet, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk._1990_actions.builder, , gettext("Paramètre incorrect\n"));
    
    list_gtk_1990_actions = &projet->list_gtk._1990_actions;
    
    if (!gtk_tree_selection_get_selected(list_gtk_1990_actions->tree_select_actions, NULL, NULL))
    {
        gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk._1990_actions.builder, "1990_actions_toolbar_action_suppr")), FALSE);
        gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk._1990_actions.builder, "1990_actions_toolbar_charge_ajout")), FALSE);
    }
    else
    {
        gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk._1990_actions.builder, "1990_actions_toolbar_action_suppr")), TRUE);
        gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk._1990_actions.builder, "1990_actions_toolbar_charge_ajout")), TRUE);
    }
    
    if (gtk_tree_selection_count_selected_rows(list_gtk_1990_actions->tree_select_charges) == 0)
    {
        gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk._1990_actions.builder, "1990_actions_toolbar_charge_edit")), FALSE);
        gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk._1990_actions.builder, "1990_actions_toolbar_charge_suppr")), FALSE);
    }
    else
    {
        gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk._1990_actions.builder, "1990_actions_toolbar_charge_edit")), TRUE);
        gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk._1990_actions.builder, "1990_actions_toolbar_charge_suppr")), TRUE);
    }
    
    return;
}


G_MODULE_EXPORT void _1990_gtk_menu_suppr_charge_clicked(GtkWidget *toolbutton __attribute__((unused)), Projet *projet)
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
    BUGMSG(projet->list_gtk._1990_actions.builder, , gettext("Paramètre incorrect\n"));
    
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


G_MODULE_EXPORT void _1990_gtk_menu_edit_charge_clicked(GtkToolButton *toolbutton __attribute__((unused)), Projet *projet)
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
    BUGMSG(projet->list_gtk._1990_actions.builder, , gettext("Paramètre incorrect\n"));
    
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


G_MODULE_EXPORT void _1990_gtk_actions_window_destroy(GtkWidget *object __attribute__((unused)), Projet *projet)
/* Description : met projet->list_gtk._1990_actions.window à NULL quand la fenêtre se ferme
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    BUGMSG(projet, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk._1990_actions.builder, , gettext("Paramètre incorrect\n"));
    
    projet->list_gtk._1990_actions.builder = NULL;
    
    return;
}


G_MODULE_EXPORT gboolean _1990_gtk_actions_charge_double_clicked(GtkWidget *widget __attribute__((unused)), GdkEvent *event, Projet *projet)
/* Description : Lance la fenêtre d'édition de la charge sélectionnée en cas de double-clique dans le tree-view
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : GdkEvent *event : Information sur l'évènement,
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    BUGMSG(projet, FALSE, gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk._1990_actions.builder, FALSE, gettext("Paramètre incorrect\n"));
    
    if ((event->type == GDK_2BUTTON_PRESS) && (gtk_widget_get_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk._1990_actions.builder, "1990_actions_toolbar_charge_edit")))))
        _1990_gtk_menu_edit_charge_clicked(NULL, projet);
    return FALSE;
}


G_MODULE_EXPORT gboolean _1990_gtk_tree_view_actions_key_press_event(GtkWidget *widget, GdkEvent *event, Projet *projet)
{
    BUGMSG(projet, FALSE, gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk._1990_actions.builder, FALSE, gettext("Paramètre incorrect\n"));
    
    if (event->key.keyval == GDK_KEY_Delete)
        _1990_gtk_menu_suppr_action_activate(widget, projet);
    return FALSE; /* Pour permettre aux autres touches d'être fonctionnelles  */
}


G_MODULE_EXPORT gboolean _1990_gtk_actions_charge_key_press_event(GtkWidget *widget, GdkEvent *event, Projet *projet)
{
    BUGMSG(projet, FALSE, gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk._1990_actions.builder, FALSE, gettext("Paramètre incorrect\n"));
    
    if (event->key.keyval == GDK_KEY_Delete)
        _1990_gtk_menu_suppr_charge_clicked(widget, projet);
    return FALSE; /* Pour permettre aux autres touches d'être fonctionnelles  */
}


G_MODULE_EXPORT gboolean _1990_gtk_window_key_press(GtkWidget *widget __attribute__((unused)), GdkEvent *event, Projet *projet)
{
    BUGMSG(projet, TRUE, gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk._1990_actions.builder, TRUE, gettext("Paramètre incorrect\n"));
    
    if (event->key.keyval == GDK_KEY_Escape)
    {
        gtk_widget_destroy(projet->list_gtk._1990_actions.window);
        return TRUE;
    }
    else
        return FALSE;
}



void _1990_gtk_actions(Projet *projet)
{
    GtkTreeIter     iter;
    unsigned int    i;
    GtkWidget       *w_temp;
    
    BUGMSG(projet, , gettext("Paramètre incorrect\n"));
    
    projet->list_gtk._1990_actions.builder = gtk_builder_new();
    BUGMSG(gtk_builder_add_from_file(projet->list_gtk._1990_actions.builder, DATADIR"/ui/1990_gtk_actions.ui", NULL) != 0, , gettext("Builder Failed\n"));
    gtk_builder_connect_signals(projet->list_gtk._1990_actions.builder, projet);
    
    projet->list_gtk._1990_actions.window = GTK_WIDGET(gtk_builder_get_object(projet->list_gtk._1990_actions.builder, "1990_actions_window"));
    projet->list_gtk._1990_actions.tree_store_actions = GTK_TREE_STORE(gtk_builder_get_object(projet->list_gtk._1990_actions.builder, "1990_actions_tree_store_action"));
    projet->list_gtk._1990_actions.tree_view_actions = GTK_TREE_VIEW(gtk_builder_get_object(projet->list_gtk._1990_actions.builder, "1990_actions_treeview_action"));
    projet->list_gtk._1990_actions.tree_select_actions = GTK_TREE_SELECTION(gtk_tree_view_get_selection(projet->list_gtk._1990_actions.tree_view_actions));
    
    projet->list_gtk._1990_actions.choix_type_action = gtk_list_store_new(1, G_TYPE_STRING);
    for (i=0;i<_1990_action_num_bat_txt(projet->pays);i++)
    {
        gtk_list_store_append(projet->list_gtk._1990_actions.choix_type_action, &iter);
        gtk_list_store_set(projet->list_gtk._1990_actions.choix_type_action, &iter, 0, _1990_action_type_bat_txt(i, projet->pays), -1);
    }
    g_object_set(gtk_builder_get_object(projet->list_gtk._1990_actions.builder, "1990_actions_treeview_cell2"), "model", projet->list_gtk._1990_actions.choix_type_action, NULL);
    
    g_object_set_data(gtk_builder_get_object(projet->list_gtk._1990_actions.builder, "1990_actions_treeview_cell3"), "column", GINT_TO_POINTER(3));
    g_object_set_data(gtk_builder_get_object(projet->list_gtk._1990_actions.builder, "1990_actions_treeview_cell4"), "column", GINT_TO_POINTER(4));
    g_object_set_data(gtk_builder_get_object(projet->list_gtk._1990_actions.builder, "1990_actions_treeview_cell5"), "column", GINT_TO_POINTER(5));
    
    // Affiche la liste des actions
    if (projet->actions != NULL)
    {
        GList           *list_parcours = projet->actions;
        GtkTreeStore    *store_actions = GTK_TREE_STORE(gtk_builder_get_object(projet->list_gtk._1990_actions.builder, "1990_actions_tree_store_action"));
        
        do
        {
            Action  *action = list_parcours->data;
            
            gtk_tree_store_append(store_actions, &action->Iter, NULL);
            gtk_tree_store_set(store_actions, &action->Iter, 0, action->numero, 1, action->description, 2, _1990_action_type_bat_txt(action->type, projet->pays), 3, action->psi0, 4, action->psi1, 5, action->psi2, -1);
            
            list_parcours = g_list_next(list_parcours);
        } while (list_parcours != NULL);
    }
    
    projet->list_gtk._1990_actions.tree_store_charges = GTK_TREE_STORE(gtk_builder_get_object(projet->list_gtk._1990_actions.builder, "1990_actions_tree_store_charge"));
    projet->list_gtk._1990_actions.tree_view_charges = GTK_TREE_VIEW(gtk_builder_get_object(projet->list_gtk._1990_actions.builder, "1990_actions_treeview_charge"));
    projet->list_gtk._1990_actions.tree_select_charges = GTK_TREE_SELECTION(gtk_tree_view_get_selection(projet->list_gtk._1990_actions.tree_view_charges));
    projet->list_gtk._1990_actions.menu_list_widget_action = NULL;
    projet->list_gtk._1990_actions.menu_type_list_action = gtk_menu_new();
    for (i=0;i<_1990_action_num_bat_txt(projet->pays);i++)
    {
        w_temp = gtk_menu_item_new_with_label(_1990_action_type_bat_txt(i, projet->pays));
        gtk_menu_shell_append(GTK_MENU_SHELL(projet->list_gtk._1990_actions.menu_type_list_action), w_temp);
        projet->list_gtk._1990_actions.menu_list_widget_action = g_list_append(projet->list_gtk._1990_actions.menu_list_widget_action, w_temp);
        gtk_widget_show(w_temp);
        g_signal_connect(w_temp, "activate", G_CALLBACK(_1990_gtk_menu_nouvelle_action_activate), projet);
    }
    gtk_menu_tool_button_set_menu(GTK_MENU_TOOL_BUTTON(gtk_builder_get_object(projet->list_gtk._1990_actions.builder, "1990_actions_toolbar_action_ajout")), projet->list_gtk._1990_actions.menu_type_list_action);

    projet->list_gtk._1990_actions.menu_type_list_charge = gtk_menu_new();
    w_temp = gtk_menu_item_new_with_label(gettext("Charge nodale"));
    gtk_menu_shell_append(GTK_MENU_SHELL(projet->list_gtk._1990_actions.menu_type_list_charge), w_temp);
    projet->list_gtk._1990_actions.menu_list_widget_charge = g_list_append(projet->list_gtk._1990_actions.menu_list_widget_charge, w_temp);
    g_signal_connect(w_temp, "activate", G_CALLBACK(_1990_gtk_menu_nouvelle_charge_nodale_activate), projet);
    gtk_menu_tool_button_set_menu(GTK_MENU_TOOL_BUTTON(gtk_builder_get_object(projet->list_gtk._1990_actions.builder, "1990_actions_toolbar_charge_ajout")), projet->list_gtk._1990_actions.menu_type_list_charge);
    w_temp = gtk_menu_item_new_with_label(gettext("Charge ponctuelle sur barre"));
    gtk_menu_shell_append(GTK_MENU_SHELL(projet->list_gtk._1990_actions.menu_type_list_charge), w_temp);
    projet->list_gtk._1990_actions.menu_list_widget_charge = g_list_append(projet->list_gtk._1990_actions.menu_list_widget_charge, w_temp);
    g_signal_connect(w_temp, "activate", G_CALLBACK(_1990_gtk_menu_nouvelle_charge_barre_ponctuelle_activate), projet);
    gtk_menu_tool_button_set_menu(GTK_MENU_TOOL_BUTTON(gtk_builder_get_object(projet->list_gtk._1990_actions.builder, "1990_actions_toolbar_charge_ajout")), projet->list_gtk._1990_actions.menu_type_list_charge);
    w_temp = gtk_menu_item_new_with_label(gettext("Charge répartie uniforme sur barre"));
    gtk_menu_shell_append(GTK_MENU_SHELL(projet->list_gtk._1990_actions.menu_type_list_charge), w_temp);
    projet->list_gtk._1990_actions.menu_list_widget_charge = g_list_append(projet->list_gtk._1990_actions.menu_list_widget_charge, w_temp);
    g_signal_connect(w_temp, "activate", G_CALLBACK(_1990_gtk_menu_nouvelle_charge_barre_repartie_uniforme_activate), projet);
    gtk_widget_show_all(projet->list_gtk._1990_actions.menu_type_list_charge);
    gtk_menu_tool_button_set_menu(GTK_MENU_TOOL_BUTTON(gtk_builder_get_object(projet->list_gtk._1990_actions.builder, "1990_actions_toolbar_charge_ajout")), projet->list_gtk._1990_actions.menu_type_list_charge);
    
    /* Défini le comportement du glissé etat vers dispo*/
    gtk_drag_source_set(GTK_WIDGET(projet->list_gtk._1990_actions.tree_view_charges), GDK_BUTTON1_MASK, drag_targets_actions, 1, GDK_ACTION_MOVE); 
    gtk_drag_dest_set(GTK_WIDGET(projet->list_gtk._1990_actions.tree_view_actions), GTK_DEST_DEFAULT_ALL, drag_targets_actions, 1, GDK_ACTION_MOVE);
    
    gtk_window_set_transient_for(GTK_WINDOW(projet->list_gtk._1990_actions.window), GTK_WINDOW(projet->list_gtk.comp.window));
    
    return;
}

#endif
