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
#include <string.h>
#include <gtk/gtk.h>

#include "1990_actions.h"
#include "common_erreurs.h"
#include "common_projet.h"
#include "common_gtk.h"
#include "EF_charge.h"
#include "EF_gtk_charge_noeud.h"
#include "EF_gtk_charge_barre_ponctuelle.h"
#include "EF_gtk_charge_barre_repartie_uniforme.h"


void _1990_gtk_menu_suppr_action_activate(GtkWidget *toolbutton, Projet *projet);
void _1990_gtk_menu_suppr_charge_clicked(GtkWidget *toolbutton, Projet *projet);
void _1990_gtk_actions_cursor_changed(GtkTreeView *tree_view, Projet *projet);
void _1990_gtk_menu_edit_charge_clicked(GtkWidget *toolbutton, Projet *projet);


/*********************** Tout ce qui concerne la fenêtre en général **********************/


const GtkTargetEntry drag_targets_actions[] = { {(gchar*)PACKAGE"1_SAME_PROC", GTK_TARGET_SAME_APP, 0}}; 


G_MODULE_EXPORT gboolean _1990_gtk_actions_window_key_press(GtkWidget *widget __attribute__((unused)), GdkEvent *event, Projet *projet)
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
    BUGMSG(projet->list_gtk._1990_actions.builder, FALSE, gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Actions");
    
    if (event->key.keyval == GDK_KEY_Escape)
    {
        gtk_widget_destroy(projet->list_gtk._1990_actions.window);
        return TRUE;
    }
    else
        return FALSE;
}


G_MODULE_EXPORT void _1990_gtk_actions_window_destroy(GtkWidget *object __attribute__((unused)), Projet *projet)
/* Description : Initialise les variables permettant de définir que la fenêtre de gestion des
 *               actions est fermée et libère la mémoire.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 *   Echec : projet == NULL,
 *           interface graphique non initialisée.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk._1990_actions.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Actions");
    
    // Désactivation des évènements pouvant s'activer lors de la fermeture de la fenêtre.
    g_signal_handler_block(projet->list_gtk._1990_actions.tree_view_actions, g_signal_handler_find(G_OBJECT(projet->list_gtk._1990_actions.tree_view_actions),G_SIGNAL_MATCH_FUNC,0,0,NULL,_1990_gtk_actions_cursor_changed,NULL));
    projet->list_gtk._1990_actions.builder = NULL;
    
    return;
}


/*********************** Tout ce qui concerne les actions **********************/


G_MODULE_EXPORT void _1990_gtk_actions_cursor_changed(
  GtkTreeView *tree_view __attribute__((unused)), Projet *projet)
/* Description : Evènement lorsqu'il y a un changement de ligne sélectionnée dans la liste
 *               des actions.
 * Paramètres : GtkTreeView *tree_view : composant tree_view à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 *   Echec : projet == NULL,
 *           interface graphique non initialisée,
 *           action introuvable.
 */
{
    GtkTreeModel    *model;
    GtkTreeIter     iter;
    unsigned int    numero;
    Action          *action;
    GList           *list_parcours;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk._1990_actions.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Actions");
    
    // On récupère l'action sélectionnée.
    if (!gtk_tree_selection_get_selected(projet->list_gtk._1990_actions.tree_select_actions, &model, &iter))
        return;
    gtk_tree_model_get(model, &iter, 0, &numero, -1);
    
    BUG(action = _1990_action_cherche_numero(projet, numero), );
    
    // On actualise la liste des charges
    gtk_tree_store_clear(projet->list_gtk._1990_actions.tree_store_charges);
    
    list_parcours = action->charges;
    while (list_parcours != NULL)
    {
        Charge_Barre_Ponctuelle *charge_tmp = list_parcours->data;
        switch (charge_tmp->type)
        {
            case CHARGE_NOEUD :
            {
                Charge_Noeud *charge = list_parcours->data;
                
                BUG(EF_gtk_charge_noeud_ajout_affichage(charge, projet, TRUE), );
                
                break;
            }
            case CHARGE_BARRE_PONCTUELLE :
            {
                Charge_Barre_Ponctuelle *charge = list_parcours->data;
                
                BUG(EF_gtk_charge_barre_ponctuelle_ajout_affichage(charge, projet, TRUE), );
                
                break;
            }
            case CHARGE_BARRE_REPARTIE_UNIFORME :
            {
                Charge_Barre_Repartie_Uniforme *charge = list_parcours->data;
                
                BUG(EF_gtk_charge_barre_repartie_uniforme_ajout_affichage(charge, projet, TRUE), );
                
                break;
            }
            default :
            {
                BUGMSG(0, , gettext("Type de charge %d inconnu."), charge_tmp->type);
                break;
            }
        }
        list_parcours = g_list_next(list_parcours);
    }
    
    return;
}


G_MODULE_EXPORT gboolean _1990_gtk_actions_tree_view_key_press_event(GtkWidget *widget,
  GdkEvent *event, Projet *projet)
/* Description : Evènement lorsqu'il y a un changement de sélection dans la liste des actions.
 * Paramètres : GtkTreeView *tree_view : composant tree_view à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : TRUE si la touche DELETE est pressée, FALSE sinon.
 *   Echec : projet == NULL,
 *           interface graphique non initialisée.
 */
{
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk._1990_actions.builder, FALSE, gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Actions");
    
    if (event->key.keyval == GDK_KEY_Delete)
    {
        _1990_gtk_menu_suppr_action_activate(widget, projet);
        return TRUE;
    }
    
    return FALSE; /* Pour permettre aux autres touches d'être fonctionnelles  */
}


G_MODULE_EXPORT gboolean _1990_gtk_actions_tree_view_drag(GtkWidget *widget,
  GdkDragContext *drag_context __attribute__((unused)), gint x, gint y,
  guint tim __attribute__((unused)), Projet *projet)
/* Description : Change d'action la charge sélectionnée.
 * Paramètres : GtkWidget *button : composant ayant réalisé l'évènement,
 *            : GdkDragContext *drag_context : inutile,
 *            : gint x, gint y : coordonnées du relachement de la souris,
 *            : guint tim : inutile,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : FALSE.
 *   Echec : FALSE :
 *             projet == NULL,
 *             interface graphique non initialisée,
 *             _1990_action_deplace_charge.
 */
{
    gint                cx, cy;
    Gtk_1990_Actions    *list_gtk_1990_actions = &projet->list_gtk._1990_actions;
    GtkTreePath         *path;
    GtkTreeModel        *list_store;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk._1990_actions.builder, FALSE, gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Actions");
    
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
        
        // Si l'action de destination est la même que l'action source, on ne fait rien.
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
        
        // On déplace les charges, charge par charge vers leur nouvelle action;
        list_parcours = g_list_last(list_fixe);
        for(;list_parcours != NULL; list_parcours = g_list_previous(list_parcours))
        {
            if (gtk_tree_model_get_iter(model_charge_source, &iter_charge_source, gtk_tree_row_reference_get_path((GtkTreeRowReference*)list_parcours->data)))
            {
                gtk_tree_model_get(model_charge_source, &iter_charge_source, 0, &num_charge_source, -1);
                BUG(EF_charge_deplace(projet, num_action_source, num_charge_source, num_action_dest), FALSE);
            }
        }
        g_list_foreach(list_fixe, (GFunc)gtk_tree_row_reference_free, NULL);
        g_list_free(list_fixe);
        
        gtk_tree_path_free(path);
    }
    
    return FALSE;
}


G_MODULE_EXPORT void _1990_gtk_actions_select_changed(
  GtkTreeSelection *treeselection __attribute__((unused)), Projet *projet)
/* Description : Réajuste automatiquement la propriété sensitive des composants d'édition en
 *               fonction de la sélection dans les tree-views.
 * Paramètres : GtkTreeSelection *treeselection : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 *   Echec : projet == NULL,
 *           interface graphique non initialisée.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk._1990_actions.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Actions");
    
    // Si aucune action n'est sélectionnée, il n'est pas possible de supprimer une action ou
    // d'ajouter une charge à l'action en cours.
    if (!gtk_tree_selection_get_selected(projet->list_gtk._1990_actions.tree_select_actions, NULL, NULL))
    {
        gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk._1990_actions.builder, "1990_actions_toolbar_action_suppr")), FALSE);
        gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk._1990_actions.builder, "1990_actions_toolbar_charge_ajout")), FALSE);
    }
    else
    {
        gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk._1990_actions.builder, "1990_actions_toolbar_action_suppr")), TRUE);
        gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk._1990_actions.builder, "1990_actions_toolbar_charge_ajout")), TRUE);
    }
    
    // Si aucune charge n'est sélectionnée, il n'est pas possible d'éditer ou de supprimer une
    // une charge à l'action en cours.
    if (gtk_tree_selection_count_selected_rows(projet->list_gtk._1990_actions.tree_select_charges) == 0)
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


G_MODULE_EXPORT void _1990_gtk_actions_description_edited(
  GtkCellRendererText *cell __attribute__((unused)), gchar *path_string, gchar *new_text,
  Projet *projet)
/* Description : Pour éditer le nom des actions via la fenêtre d'actions.
 * Paramètres : GtkCellRendererText *cell : la cellule éditée,
 *            : gchar *path_string : chemin vers la cellule,
 *            : gchar *new_text : nouveau texte,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 *   Echec : projet == NULL,
 *           interface graphique non initialisée,
 *           _1990_action_renomme.
 */
{
    GtkTreeIter   iter;
    unsigned int  numero;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk._1990_actions.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Actions");
    
    // On récupère l'action en cours d'édition
    gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(projet->list_gtk._1990_actions.tree_store_actions), &iter, path_string);
    gtk_tree_model_get(GTK_TREE_MODEL(projet->list_gtk._1990_actions.tree_store_actions), &iter, 0, &numero, -1);
    
    // On lui modifie son nom
    BUG(_1990_action_renomme(projet, numero, new_text), );
    
    return;
}


G_MODULE_EXPORT void _1990_gtk_actions_type_edited(
  GtkCellRendererText *cell __attribute__((unused)), const gchar *path_string, 
  const gchar *new_text, Projet *projet)
/* Description : Change le type (psi0, psi1 et psi2 automatiquement mis à jour) d'une action.
 * Paramètres : GtkCellRendererText *cell : cellule en cours,
 *            : gchar *path_string : path de la ligne en cours,
 *            : gchar *new_text : nouveau coefficient psi0,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 *   Echec : projet == NULL,
 *           interface graphique non initialisée,
 *           type d'action new_text inconnu,
 *           _1990_action_change_type.
*/
{
    GtkTreeIter     iter;
    unsigned int    action_num, type;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk._1990_actions.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Actions");
    
    gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(projet->list_gtk._1990_actions.tree_store_actions), &iter, path_string);
    gtk_tree_model_get(GTK_TREE_MODEL(projet->list_gtk._1990_actions.tree_store_actions), &iter, 0, &action_num, -1);
    
    for (type=0;type<_1990_action_num_bat_txt(projet->pays);type++)
    {
        if (strcmp(new_text, _1990_action_type_bat_txt(type, projet->pays)) == 0)
            break;
    }
    BUGMSG(type != _1990_action_num_bat_txt(projet->pays), , gettext("Type d'action '%s' inconnu.\n"), new_text);
    
    BUG(_1990_action_change_type(projet, action_num, type), );
    
    return;
}


G_MODULE_EXPORT void _1990_gtk_tree_view_actions_psi_edited(GtkCellRendererText *cell,
  gchar *path_string, gchar *new_text, Projet *projet)
/* Description : Changement d'un coefficient psi d'une action.
 * Paramètres : GtkCellRendererText *cell : cellule en cours,
 *            : gchar *path_string : path de la ligne en cours,
 *            : gchar *new_text : nouveau coefficient psi,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 *   Echec : projet == NULL,
 *           interface graphique non initialisée,
 *           erreur d'allocation mémoire,
 *           _1990_action_renomme.
 */
{
    GtkTreeIter     iter;
    unsigned int    num_action;
    char            *fake;
    double          convertion;
    guint           column = GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(cell), "column"));
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk._1990_actions.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Actions");
    
    fake = (char*)malloc(sizeof(char)*(strlen(new_text)+1));
    BUGMSG(fake, , gettext("Erreur d'allocation mémoire.\n"));
    
    gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(projet->list_gtk._1990_actions.tree_store_actions), &iter, path_string);
    gtk_tree_model_get(GTK_TREE_MODEL(projet->list_gtk._1990_actions.tree_store_actions), &iter, 0, &num_action, -1);
    
    // On vérifie si le texte contient bien un nombre flottant
    if (sscanf(new_text, "%lf%s", &convertion, fake) == 1)
        BUG(_1990_action_change_psi(projet, num_action, column-3, convertion), );
    
    free(fake);
     
    return;
}


G_MODULE_EXPORT void _1990_gtk_menu_nouvelle_action_activate(GtkMenuItem *menuitem,
  Projet *projet)
/* Description : Ajout d'une nouvelle action depuis le menu de la barre d'outils.
 * Paramètres : GtkMenuItem *menuitem : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 *   Echec : projet == NULL,
 *           interface graphique non initialisée,
 *           erreur d'allocation mémoire,
 *           _1990_action_ajout,
 *           type d'action inconnu.
 */
{
    unsigned int    type = 0;
    GList           *list_parcours;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk._1990_actions.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Actions");
    
    list_parcours = projet->list_gtk._1990_actions.menu_list_widget_action;
    while (list_parcours != NULL)
    {
        if ((GTK_IS_MENU_TOOL_BUTTON(menuitem)) || ((GtkMenuItem *)list_parcours->data == menuitem))
        {
            char        *tmp;
            Action      *action;
            GtkTreePath *path;
            
            BUGMSG(tmp = g_strdup_printf("%s %u", gettext("Sans nom"), g_list_length(projet->actions)), , gettext("Erreur d'allocation mémoire.\n"));
            // On crée l'action en fonction de la catégorie sélectionnée dans le menu déroulant.
            BUG(action = _1990_action_ajout(projet, type, tmp), );
            free(tmp);
            
            path = gtk_tree_model_get_path(GTK_TREE_MODEL(projet->list_gtk._1990_actions.tree_store_actions), &action->Iter_fenetre);
            gtk_tree_view_set_cursor(GTK_TREE_VIEW(projet->list_gtk._1990_actions.tree_view_actions), path, gtk_tree_view_get_column(GTK_TREE_VIEW(projet->list_gtk._1990_actions.tree_view_actions), 1), TRUE);
            gtk_tree_path_free(path);
            
            return;
        }
        type++;
        list_parcours = g_list_next(list_parcours);
    }
    
    BUGMSG(list_parcours, , gettext("Impossible de trouver le type d'action correspondant à cette action.\n"));
}


G_MODULE_EXPORT void _1990_gtk_menu_suppr_action_activate(
  GtkWidget *toolbutton __attribute__((unused)), Projet *projet)
/* Description : Supprimer l'action sélectionnée.
 * Paramètres : GtkToolButton *toolbutton : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 *   Echec : projet == NULL,
 *           interface graphique non initialisée,
 *           erreur d'allocation mémoire,
 *           _1990_action_renomme.
 */
{
    GtkTreeIter     iter;
    GtkTreeModel    *model;
    unsigned int    numero_action;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk._1990_actions.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Actions");
    
    if (!gtk_tree_selection_get_selected(projet->list_gtk._1990_actions.tree_select_actions, &model, &iter))
        return;
    
    // On récupère le numéro de l'action sélectionnée
    gtk_tree_model_get(model, &iter, 0, &numero_action, -1);
    
    // Et on la supprime ainsi que les charges la contenant
    BUG(_1990_action_free_num(projet, numero_action), );
    
    return;
}


/*********************** Tout ce qui concerne les charges **********************/


G_MODULE_EXPORT gboolean _1990_gtk_actions_charge_key_press_event(GtkWidget *widget,
  GdkEvent *event, Projet *projet)
/* Description : Gestion des touches du composant treeview charge.
 * Paramètres : GtkWidget *widget : composant tree_view à l'origine de l'évènement,
 *            : GdkEvent *event : description de la touche pressée,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : TRUE si la touche DELETE est pressée, FALSE sinon.
 *   Echec : FALSE :
 *             projet == NULL,
 *             interface graphique non initialisée.
 */
{
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk._1990_actions.builder, FALSE, gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Actions");
    
    if (event->key.keyval == GDK_KEY_Delete)
    {
        _1990_gtk_menu_suppr_charge_clicked(widget, projet);
        return TRUE;
    }
    
    return FALSE; /* Pour permettre aux autres touches d'être fonctionnelles  */
}


G_MODULE_EXPORT gboolean _1990_gtk_actions_charge_double_clicked(GtkWidget *widget,
  GdkEvent *event, Projet *projet)
/* Description : Lance la fenêtre d'édition de la charge sélectionnée en cas de double-clique
 *               dans le tree-view charge.
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
    BUGMSG(projet->list_gtk._1990_actions.builder, FALSE, gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Actions");
    
    if ((event->type == GDK_2BUTTON_PRESS) && (gtk_widget_get_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk._1990_actions.builder, "1990_actions_toolbar_charge_edit")))))
    {
        _1990_gtk_menu_edit_charge_clicked(widget, projet);
        return TRUE;
    }
    
    return FALSE;
}


G_MODULE_EXPORT void _1990_gtk_actions_tree_view_drag_begin(
  GtkWidget *widget __attribute__((unused)), 
  GdkDragContext *drag_context __attribute__((unused)), void *data __attribute__((unused)))
/* Description : Évènement "drag-begin" nécessaire au treeview charge pour que la fonction DnD
 *               marche.
 * Paramètres : GtkWidget *widget : le composant à l'origine de l'évènement,
 *            : GdkDragContext *drag_context : drag_context,
 *            : void *data : non défini.
 * Valeur renvoyée : Aucune.
 */
{
    return;
}


G_MODULE_EXPORT void _1990_gtk_tree_view_charges_description_edited(
  GtkCellRendererText *cell __attribute__((unused)), gchar *path_string, gchar *new_text,
  Projet *projet)
/* Description : Pour éditer le nom des charges via la fenêtre d'actions.
 * Paramètres : GtkCellRendererText *cell : la cellule éditée,
 *            : gchar *path_string : chemin vers la cellule,
 *            : gchar *new_text : nouveau texte,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 *   Echec : projet == NULL,
 *           interface graphique non initialisée,
 *           EF_charge_renomme.
 */
{
    GtkTreeModel    *model;
    GtkTreeIter     iter, iter_action;
    GtkTreeModel    *model_action;
    unsigned int    numero_action, numero_charge;
    char            *description;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk._1990_actions.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Actions");
    
    // On recherche la charge en cours d'édition
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(projet->list_gtk._1990_actions.tree_view_charges));
    
    if (!gtk_tree_selection_get_selected(projet->list_gtk._1990_actions.tree_select_actions, &model_action, &iter_action))
        return;
    gtk_tree_model_get(model_action, &iter_action, 0, &numero_action, -1);
    
    gtk_tree_model_get_iter_from_string(model, &iter, path_string);
    gtk_tree_model_get(model, &iter, 0, &numero_charge, 1, &description, -1);
    
    BUG(EF_charge_renomme(projet, numero_action, numero_charge, new_text), );
    
    return;
}


G_MODULE_EXPORT void _1990_gtk_menu_edit_charge_clicked(
  GtkWidget *toolbutton __attribute__((unused)), Projet *projet)
/* Description : Edite les charges sélectionnées.
 * Paramètres : GtkToolButton *toolbutton : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 *   Echec : projet == NULL,
 *           interface graphique non initialisée.
 */
{
    GtkTreeIter     iter;
    GtkTreeModel    *model;
    unsigned int    numero_action, numero_charge;
    GList           *list, *list_parcours;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk._1990_actions.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Actions");
    
    // On récupère le numéro de l'action qui contient les charges à éditer.
    if (!gtk_tree_selection_get_selected(projet->list_gtk._1990_actions.tree_select_actions, &model, &iter))
        return;
    gtk_tree_model_get(model, &iter, 0, &numero_action, -1);
    
    // On récupère la liste des charges à éditer.
    list = gtk_tree_selection_get_selected_rows(projet->list_gtk._1990_actions.tree_select_charges, &model);
    list_parcours = g_list_first(list);
    for(;list_parcours != NULL; list_parcours = g_list_next(list_parcours))
    {
        if (gtk_tree_model_get_iter(model, &iter, (GtkTreePath*)list_parcours->data))
        {
    // Et on les édite les unes après les autres.
            Charge_Noeud    *charge_noeud;
            
            gtk_tree_model_get(model, &iter, 0, &numero_charge, -1);
            BUG(charge_noeud = EF_charge_cherche(projet, numero_action, numero_charge), );
            
            switch (charge_noeud->type)
            {
                case CHARGE_NOEUD :
                {
                    BUG(EF_gtk_charge_noeud(projet, numero_action, numero_charge), );
                    break;
                }
                case CHARGE_BARRE_PONCTUELLE :
                {
                    BUG(EF_gtk_charge_barre_ponctuelle(projet, numero_action, numero_charge), );
                    break;
                }
                case CHARGE_BARRE_REPARTIE_UNIFORME :
                {
                    BUG(EF_gtk_charge_barre_repartie_uniforme(projet, numero_action, numero_charge), );
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


G_MODULE_EXPORT void _1990_gtk_menu_suppr_charge_clicked(
  GtkWidget *toolbutton __attribute__((unused)), Projet *projet)
/* Description : Supprimer les actions sélectionnées.
 * Paramètres : GtkToolButton *toolbutton : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 *   Echec : projet == NULL,
 *           interface graphique non initialisée.
 */
{
    GtkTreeIter     iter;
    GtkTreeModel    *model;
    unsigned int    numero_action, numero_charge;
    GList           *list, *list_fixe, *list_parcours;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk._1990_actions.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Actions");
    
    // On récupère le numéro de l'action qui contient les charges à supprimer.
    if (!gtk_tree_selection_get_selected(projet->list_gtk._1990_actions.tree_select_actions, &model, &iter))
        return;
    gtk_tree_model_get(model, &iter, 0, &numero_action, -1);
    
    // On récupère la liste des charges à supprimer.
    list = gtk_tree_selection_get_selected_rows(projet->list_gtk._1990_actions.tree_select_charges, &model);
    
    // On converti les lignes en ligne fixe sinon, le tree-view-charges se perd lorsqu'on supprime les lignes dues au déplacement des actions en cours de route.
    list_parcours = g_list_last(list);
    list_fixe = NULL;
    for(;list_parcours != NULL; list_parcours = g_list_previous(list_parcours))
        list_fixe = g_list_append(list_fixe, gtk_tree_row_reference_new(model, (GtkTreePath*)list_parcours->data));
    g_list_foreach(list, (GFunc)gtk_tree_path_free, NULL);
    g_list_free(list);
    
    // On supprime les charges sélectionnées. Pas besoin de remettre à jour le tree-view, c'est inclus dans EF_charge_supprime
    list_parcours = g_list_first(list_fixe);
    for(;list_parcours != NULL; list_parcours = g_list_next(list_parcours))
    {
        if (gtk_tree_model_get_iter(model, &iter, gtk_tree_row_reference_get_path((GtkTreeRowReference*)list_parcours->data)))
        {
            gtk_tree_model_get(model, &iter, 0, &numero_charge, -1);
            BUG(EF_charge_supprime(projet, numero_action, numero_charge), );
        }
    }
    g_list_foreach(list_fixe, (GFunc)gtk_tree_row_reference_free, NULL);
    g_list_free(list_fixe);
    
    return;
}


/*********************** Bouton de fermeture de la fenêtre **********************/


G_MODULE_EXPORT void _1990_gtk_actions_window_button_close(GtkButton *button __attribute__((unused)), Projet *projet)
/* Description : Supprimer les actions sélectionnées.
 * Paramètres : GtkToolButton *toolbutton : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 *   Echec : projet == NULL,
 *           interface graphique non initialisée.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk._1990_actions.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Actions");
    
    gtk_widget_destroy(projet->list_gtk._1990_actions.window);
}


/*********************** Fonction permettant de créer les charges **********************/


void _1990_gtk_menu_nouvelle_charge_nodale_activate(
  GtkMenuItem *menuitem __attribute__((unused)), Projet* projet)
/* Description : Ouvre la fenêtre permettant d'ajouter une charge nodale.
 * Paramètres : GtkMenuItem *menuitem : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 *   Echec : projet == NULL,
 *           interface graphique non initialisée.
 */
{
    GtkTreeIter     iter_action;
    GtkTreeModel    *model_action;
    unsigned int    numero_action;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk._1990_actions.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Actions");
    
    if (!gtk_tree_selection_get_selected(projet->list_gtk._1990_actions.tree_select_actions, &model_action, &iter_action))
        return;
    gtk_tree_model_get(model_action, &iter_action, 0, &numero_action, -1);
    
    BUG(EF_gtk_charge_noeud(projet, numero_action, G_MAXUINT), );
}


void _1990_gtk_menu_nouvelle_charge_barre_ponctuelle_activate(
  GtkMenuItem *menuitem __attribute__((unused)), Projet* projet)
/* Description : Ouvre la fenêtre permettant d'ajouter une charge ponctuelle sur barre.
 * Paramètres : GtkMenuItem *menuitem : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 *   Echec : projet == NULL,
 *           interface graphique non initialisée.
 */
{
    GtkTreeIter                 iter_action;
    GtkTreeModel                *model_action;
    unsigned int                numero_action;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk._1990_actions.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Actions");
    
    if (!gtk_tree_selection_get_selected(projet->list_gtk._1990_actions.tree_select_actions, &model_action, &iter_action))
        return;
    gtk_tree_model_get(model_action, &iter_action, 0, &numero_action, -1);
    
    BUG(EF_gtk_charge_barre_ponctuelle(projet, numero_action, G_MAXUINT), );
    
    return;
}


void _1990_gtk_menu_nouvelle_charge_barre_repartie_uniforme_activate(
  GtkMenuItem *menuitem __attribute__((unused)), Projet* projet)
/* Description : Ouvre la fenêtre permettant d'ajouter une charge répartie uniforme.
 * Paramètres : GtkMenuItem *menuitem : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 *   Echec : projet == NULL,
 *           interface graphique non initialisée.
 */
{
    GtkTreeIter                 iter_action;
    GtkTreeModel                *model_action;
    unsigned int                numero_action;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk._1990_actions.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Actions");
    
    if (!gtk_tree_selection_get_selected(projet->list_gtk._1990_actions.tree_select_actions, &model_action, &iter_action))
        return;
    gtk_tree_model_get(model_action, &iter_action, 0, &numero_action, -1);
    
    BUG(EF_gtk_charge_barre_repartie_uniforme(projet, numero_action, G_MAXUINT), );
    
    return;
}


/**************** Fonction créant la fenêtre de gestion des actions **********************/


G_MODULE_EXPORT void _1990_gtk_actions(Projet *projet)
/* Description : Création de la fenêtre Actions.
 * Paramètres : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 *   Echec : projet == NULL,
 *           interface graphique impossible à générer.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    
    projet->list_gtk._1990_actions.builder = gtk_builder_new();
    BUGMSG(gtk_builder_add_from_file(projet->list_gtk._1990_actions.builder, DATADIR"/ui/1990_actions.ui", NULL) != 0, , gettext("La génération de la fenêtre %s a échouée.\n"), "Actions");
    gtk_builder_connect_signals(projet->list_gtk._1990_actions.builder, projet);
    
    projet->list_gtk._1990_actions.window = GTK_WIDGET(gtk_builder_get_object(projet->list_gtk._1990_actions.builder, "1990_actions_window"));
    projet->list_gtk._1990_actions.tree_store_actions = GTK_TREE_STORE(gtk_builder_get_object(projet->list_gtk._1990_actions.builder, "1990_actions_tree_store_action"));
    projet->list_gtk._1990_actions.tree_view_actions = GTK_TREE_VIEW(gtk_builder_get_object(projet->list_gtk._1990_actions.builder, "1990_actions_treeview_action"));
    projet->list_gtk._1990_actions.tree_select_actions = GTK_TREE_SELECTION(gtk_tree_view_get_selection(projet->list_gtk._1990_actions.tree_view_actions));
    
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
            
            gtk_tree_store_append(store_actions, &action->Iter_fenetre, NULL);
            gtk_tree_store_set(store_actions, &action->Iter_fenetre, 0, action->numero, 1, action->nom, 2, _1990_action_type_bat_txt(action->type, projet->pays), 3, action->psi0, 4, action->psi1, 5, action->psi2, -1);
            
            list_parcours = g_list_next(list_parcours);
        } while (list_parcours != NULL);
    }
    
    projet->list_gtk._1990_actions.tree_store_charges = GTK_TREE_STORE(gtk_builder_get_object(projet->list_gtk._1990_actions.builder, "1990_actions_tree_store_charge"));
    projet->list_gtk._1990_actions.tree_view_charges = GTK_TREE_VIEW(gtk_builder_get_object(projet->list_gtk._1990_actions.builder, "1990_actions_treeview_charge"));
    projet->list_gtk._1990_actions.tree_select_charges = GTK_TREE_SELECTION(gtk_tree_view_get_selection(projet->list_gtk._1990_actions.tree_view_charges));
    
    gtk_menu_button_set_popup(GTK_MENU_BUTTON(gtk_builder_get_object(projet->list_gtk._1990_actions.builder, "1990_actions_toolbar_action_ajout")), projet->list_gtk._1990_actions.menu_type_list_action);
    // Pour éviter que le menu menu_type_list_action soit libéré en même temps que le bouton 1990_actions_toolbar_action_ajout.
    g_object_ref(projet->list_gtk._1990_actions.menu_type_list_action);

    gtk_menu_button_set_popup(GTK_MENU_BUTTON(gtk_builder_get_object(projet->list_gtk._1990_actions.builder, "1990_actions_toolbar_charge_ajout")), projet->list_gtk._1990_actions.menu_type_list_charge);
    gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk._1990_actions.builder, "1990_actions_toolbar_charge_ajout")), FALSE);
    
    /* Défini le comportement du glissé etat vers dispo*/
    gtk_drag_source_set(GTK_WIDGET(projet->list_gtk._1990_actions.tree_view_charges), GDK_BUTTON1_MASK, drag_targets_actions, 1, GDK_ACTION_MOVE); 
    gtk_drag_dest_set(GTK_WIDGET(projet->list_gtk._1990_actions.tree_view_actions), GTK_DEST_DEFAULT_ALL, drag_targets_actions, 1, GDK_ACTION_MOVE);
    
    gtk_window_set_transient_for(GTK_WINDOW(projet->list_gtk._1990_actions.window), GTK_WINDOW(projet->list_gtk.comp.window));
    
    return;
}

#endif
