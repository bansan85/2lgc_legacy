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

#include "1990_action.h"
#include "common_erreurs.h"
#include "common_projet.h"
#include "common_gtk.h"
#include "common_text.h"
#include "common_math.h"
#include "EF_charge.h"
#include "EF_charge_noeud.h"
#include "EF_gtk_charge_noeud.h"
#include "EF_charge_barre_ponctuelle.h"
#include "EF_gtk_charge_barre_ponctuelle.h"
#include "EF_charge_barre_repartie_uniforme.h"
#include "EF_gtk_charge_barre_repartie_uniforme.h"


void _1990_gtk_menu_suppr_action_activate (GtkWidget   *toolbutton, Projet *p);
void _1990_gtk_menu_suppr_charge_clicked  (GtkWidget   *toolbutton, Projet *p);
void _1990_gtk_actions_cursor_changed     (GtkTreeView *tree_view,  Projet *p);
void _1990_gtk_menu_edit_charge_clicked   (GtkWidget   *toolbutton, Projet *p);


const GtkTargetEntry drag_targets_actions[] = { {(gchar *)PACKAGE"1_SAME_PROC",
                                                 GTK_TARGET_SAME_APP,
                                                 0}};


GTK_WINDOW_KEY_PRESS (_1990, actions);


GTK_WINDOW_DESTROY (
  _1990,
  actions,
  g_signal_handler_block (UI_ACT.tree_view_actions,
                          g_signal_handler_find (G_OBJECT (
                                                     UI_ACT.tree_view_actions),
                          G_SIGNAL_MATCH_FUNC,
                          0,
                          0,
                          NULL,
                          _1990_gtk_actions_cursor_changed,
                          NULL));
);


GTK_WINDOW_CLOSE (_1990, actions);
// Le ; est facultatif pour la compilation mais nécessaire pour Doxygen.


/********************** Tout ce qui concerne les actions *********************/


/**
 * \brief Evènement lorsqu'il y a un changement de ligne sélectionnée dans la
 *        liste des actions.\n
 *        Cela rend éditable les cœfficients &psi; s'il s'agit d'actions non
 *        permanentes et actualise la liste des charges.
 * \param tree_view : composant tree_view à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée,
 *     - type d'action inconnu.
 */
void
_1990_gtk_actions_cursor_changed (GtkTreeView *tree_view,
                                  Projet      *p)
{
  GtkTreeModel *model;
  GtkTreeIter   iter;
  Action       *action;
  GList        *list_parcours;
  
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_ACT.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Actions"); )
  
  // On récupère l'action sélectionnée.
  if (!gtk_tree_selection_get_selected (UI_ACT.tree_select_actions,
                                        &model,
                                        &iter))
  {
    return;
  }
  
  gtk_tree_model_get (model, &iter, 0, &action, -1);
  
  switch (_1990_action_categorie_bat (_1990_action_type_renvoie (action),
                                      p->parametres.norme))
  {
    case ACTION_POIDS_PROPRE :
    case ACTION_PRECONTRAINTE :
    {
      g_object_set (GTK_CELL_RENDERER_TEXT (gtk_builder_get_object (
                               UI_ACT.builder, "1990_actions_treeview_cell2")),
                    "editable", FALSE,
                    NULL);
      g_object_set (GTK_CELL_RENDERER_TEXT (gtk_builder_get_object (
                               UI_ACT.builder, "1990_actions_treeview_cell3")),
                    "editable", FALSE,
                    NULL);
      g_object_set (GTK_CELL_RENDERER_TEXT (gtk_builder_get_object (
                               UI_ACT.builder, "1990_actions_treeview_cell4")),
                    "editable", FALSE,
                    NULL);
      break;
    }
    case ACTION_VARIABLE :
    case ACTION_ACCIDENTELLE :
    case ACTION_SISMIQUE :
    case ACTION_EAUX_SOUTERRAINES :
    {
      g_object_set (GTK_CELL_RENDERER_TEXT (gtk_builder_get_object (
                               UI_ACT.builder, "1990_actions_treeview_cell2")),
                    "editable", TRUE,
                    NULL);
      g_object_set (GTK_CELL_RENDERER_TEXT (gtk_builder_get_object (
                               UI_ACT.builder, "1990_actions_treeview_cell3")),
                    "editable", TRUE,
                    NULL);
      g_object_set (GTK_CELL_RENDERER_TEXT (gtk_builder_get_object (
                               UI_ACT.builder, "1990_actions_treeview_cell4")),
                    "editable", TRUE,
                    NULL);
      break;
    }
    case ACTION_INCONNUE :
    default :
    {
      FAILCRIT (,
                (gettext ("Type d'action %u inconnu.\n"),
                          _1990_action_type_renvoie (action)); )
      break;
    }
  }
  
  // On actualise la liste des charges
  gtk_tree_store_clear (UI_ACT.tree_store_charges);
  
  list_parcours = _1990_action_charges_renvoie (action);
  while (list_parcours != NULL)
  {
    Charge *charge = list_parcours->data;
    
    gtk_tree_store_append (UI_ACT.tree_store_charges, &charge->Iter, NULL);
    gtk_tree_store_set (UI_ACT.tree_store_charges,
                        &charge->Iter,
                        0, list_parcours->data,
                        -1);
    
    list_parcours = g_list_next (list_parcours);
  }
  
  return;
}


/**
 * \brief Evènement lorsqu'il y a une touche appuyée sur le clavier dans la
 *        liste des actions.\n
 *        Si c'est la touche SUPPR, l'action est supprimée.
 * \param widget : composant tree_view à l'origine de l'évènement,
 * \param event : caractéristiques de la touche pressée,
 * \param p : la variable projet.
 * \return TRUE si la touche DELETE est pressée, FALSE sinon.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
gboolean
_1990_gtk_actions_tree_view_key_press_event (GtkWidget *widget,
                                             GdkEvent  *event,
                                             Projet    *p)
{
  BUGPARAMCRIT (p, "%p", p, FALSE)
  BUGCRIT (UI_ACT.builder,
           FALSE,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Actions"); )
  
  if (event->key.keyval == GDK_KEY_Delete)
  {
    _1990_gtk_menu_suppr_action_activate (NULL, p);
    
    return TRUE;
  }
  
  return FALSE; // Pour permettre aux autres touches d'être fonctionnelles
}


/** 
 * \brief Change d'action la charge sélectionnée par un mouvement de glissement
 *        de la souris.
 * \param widget : composant ayant réalisé l'évènement,
 * \param drag_context : inutile,
 * \param x : coordonnées du relachement de la souris,
 * \param y : coordonnées du relachement de la souris,
 * \param tim : inutile,
 * \param p : la variable projet.
 * \return FALSE.\n
 *   Echec : FALSE :
 *    - p == NULL,
 *    - interface graphique non initialisée,
 *    - #EF_charge_deplace.
 */
gboolean
_1990_gtk_actions_tree_view_drag (GtkWidget      *widget,
                                  GdkDragContext *drag_context,
                                  gint            x,
                                  gint            y,
                                  guint           tim,
                                  Projet         *p)
{
  gint          cx, cy;
  GtkTreePath  *path;
  GtkTreeModel *list_store;
  
  BUGPARAMCRIT (p, "%p", p, FALSE)
  BUGCRIT (UI_ACT.builder,
           FALSE,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Actions"); )
  
  // On récupère la ligne de la nouvelle action des charges sélectionnées
  gdk_window_get_geometry (gtk_tree_view_get_bin_window (GTK_TREE_VIEW (
                                                    UI_ACT.tree_view_actions)),
                           &cx,
                           &cy,
                           NULL,
                           NULL);
  gtk_tree_view_get_path_at_pos (GTK_TREE_VIEW (widget),
                                 x -= cx,
                                 y -= cy,
                                 &path,
                                 NULL,
                                 &cx,
                                 &cy);
  list_store = gtk_tree_view_get_model (GTK_TREE_VIEW (
                                                    UI_ACT.tree_view_actions));
  if (path != NULL)
  {
    Action       *action_dest, *action_source;
    Charge       *charge_source;
    GtkTreeIter   iter_action_dest, iter_charge_source, iter_action_source;
    GtkTreeModel *model_charge_source, *model_action_source;
    GList        *list, *list_fixe, *list_parcours;
    
    // On récupère l'action de destination
    gtk_tree_model_get_iter (list_store, &iter_action_dest, path);
    gtk_tree_model_get (list_store, &iter_action_dest, 0, &action_dest, -1);
    gtk_tree_path_free (path);
    
    // On récupère l'action actuelle
    if (!gtk_tree_selection_get_selected (UI_ACT.tree_select_actions,
                                          &model_action_source,
                                          &iter_action_source))
    {
      return FALSE;
    }
    
    gtk_tree_model_get (model_action_source,
                        &iter_action_source,
                        0,
                        &action_source,
                        -1);
    
    // Si l'action de destination est la même que l'action source, on ne fait
    // rien.
    if (action_dest == action_source)
    {
      return FALSE;
    }
    
    list = gtk_tree_selection_get_selected_rows (UI_ACT.tree_select_charges,
                                                 &model_charge_source);
    // On converti les lignes en ligne fixe sinon, le tree-view-charges se perd
    // lorsqu'on supprime les lignes dues au déplacement des actions en cours
    // de route.
    list_parcours = g_list_last (list);
    list_fixe = NULL;
    for ( ;
         list_parcours != NULL;
         list_parcours = g_list_previous (list_parcours))
    {
      list_fixe = g_list_append (list_fixe,
                                 gtk_tree_row_reference_new (
                                                           model_charge_source,
                                         (GtkTreePath *) list_parcours->data));
    }
    g_list_foreach (list, (GFunc) gtk_tree_path_free, NULL);
    g_list_free (list);
    
    // On déplace les charges, charge par charge vers leur nouvelle action;
    list_parcours = g_list_last (list_fixe);
    for ( ;
         list_parcours != NULL;
         list_parcours = g_list_previous (list_parcours))
    {
      if (gtk_tree_model_get_iter (model_charge_source,
                                   &iter_charge_source,
                                   gtk_tree_row_reference_get_path (
                                  (GtkTreeRowReference *)list_parcours->data)))
      {
        gtk_tree_model_get (model_charge_source,
                            &iter_charge_source,
                            0,
                            &charge_source,
                            -1);
        BUG (EF_charge_deplace (p, action_source, charge_source, action_dest),
             FALSE)
      }
    }
    g_list_foreach (list_fixe, (GFunc) gtk_tree_row_reference_free, NULL);
    g_list_free (list_fixe);
  }
  
  return FALSE;
}


/**
 * \brief Réajuste automatiquement la propriété sensitive des composants
 *        d'édition en fonction de la sélection dans les tree-views action et
 *        charge.
 * \param treeselection : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
void
_1990_gtk_actions_select_changed (GtkTreeSelection *treeselection,
                                  Projet           *p)
{
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_ACT.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Actions"); )
  
  // Si aucune action n'est sélectionnée, il n'est pas possible de supprimer
  // une action ou d'ajouter une charge à l'action en cours.
  if (!gtk_tree_selection_get_selected (UI_ACT.tree_select_actions,
                                        NULL,
                                        NULL))
  {
    gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
                         UI_ACT.builder, "1990_actions_toolbar_action_suppr")),
                              FALSE);
    gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
                         UI_ACT.builder, "1990_actions_toolbar_charge_ajout")),
                              FALSE);
  }
  else
  {
    gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
                         UI_ACT.builder, "1990_actions_toolbar_action_suppr")),
                              TRUE);
    gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
                         UI_ACT.builder, "1990_actions_toolbar_charge_ajout")),
                              TRUE);
  }
  
  // Si aucune charge n'est sélectionnée, il n'est pas possible d'éditer ou de
  // supprimer une charge à l'action en cours.
  if (gtk_tree_selection_count_selected_rows (UI_ACT.tree_select_charges) == 0)
  {
    gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
                          UI_ACT.builder, "1990_actions_toolbar_charge_edit")),
                              FALSE);
    gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
                         UI_ACT.builder, "1990_actions_toolbar_charge_suppr")),
                              FALSE);
  }
  else
  {
    gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
                          UI_ACT.builder, "1990_actions_toolbar_charge_edit")),
                              TRUE);
    gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
                         UI_ACT.builder, "1990_actions_toolbar_charge_suppr")),
                              TRUE);
  }
  
  return;
}


/**
 * \brief Édite le nom d'une actions via la fenêtre d'actions.
 * \param cell : la cellule éditée,
 * \param path_string : chemin vers la cellule,
 * \param new_text : le nouveau nom,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée,
 *     - #_1990_action_nom_change.
 */
void
_1990_gtk_actions_nom_edited (GtkCellRendererText *cell,
                              gchar               *path_string,
                              gchar               *new_text,
                              Projet              *p)
{
  GtkTreeIter iter;
  Action     *action;
  
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_ACT.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Actions"); )
  
  // On récupère l'action en cours d'édition
  gtk_tree_model_get_iter_from_string (GTK_TREE_MODEL (
                                                    UI_ACT.tree_store_actions),
                                       &iter,
                                       path_string);
  gtk_tree_model_get (GTK_TREE_MODEL (UI_ACT.tree_store_actions),
                      &iter,
                      0,
                      &action,
                      -1);
  
  // On modifie son nom
  BUG (_1990_action_nom_change (p, action, new_text), )
  
  return;
}


/**
 * \brief Change le type (&psi;<sub>0</sub>, &psi;<sub>1</sub> et
 *        &psi;<sub>2</sub> sont automatiquement mis à jour) d'une action.
 * \brief cell : cellule en cours,
 * \brief path_string : path de la ligne en cours,
 * \brief new_text : le nouveau type,
 * \brief p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée,
 *     - type d'action new_text inconnu,
 *     - #_1990_action_type_change.
 */
void
_1990_gtk_actions_type_edited (GtkCellRendererText *cell,
                               const gchar         *path_string,
                               const gchar         *new_text,
                               Projet              *p)
{
  GtkTreeIter iter;
  uint8_t     type;
  Action     *action;
  
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_ACT.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Actions"); )
  
  gtk_tree_model_get_iter_from_string (GTK_TREE_MODEL (
                                                    UI_ACT.tree_store_actions),
                                       &iter,
                                       path_string);
         gtk_tree_model_get (GTK_TREE_MODEL (UI_ACT.tree_store_actions),
                      &iter,
                      0, &action,
                      -1);
  
  for (type = 0; type < _1990_action_num_bat_txt (p->parametres.norme); type++)
  {
    if (strcmp (new_text, _1990_action_bat_txt_type (type,
                                                    p->parametres.norme)) == 0)
    {
      break;
    }
  }
  BUGCRIT (type != _1990_action_num_bat_txt (p->parametres.norme),
           ,
           (gettext ("Type d'action '%s' inconnu.\n"), new_text); )
  
  BUG (_1990_action_type_change (p, action, type), )
  
  return;
}


/**
 * \brief Changement d'un coefficient &psi; d'une action.
 * \brief cell : cellule en cours,
 * \brief path_string : path de la ligne en cours,
 * \brief new_text : nouveau coefficient &psi;,
 * \brief p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée,
 *     - #_1990_action_nom_change.
 */
void
_1990_gtk_tree_view_actions_psi_edited (GtkCellRendererText *cell,
                                        gchar               *path_string,
                                        gchar               *new_text,
                                        Projet              *p)
{
  GtkTreeIter iter;
  Action     *action;
  double      convertion;
  guint       column;
  
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_ACT.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Actions"); )
  
  column = GPOINTER_TO_UINT (g_object_get_data (G_OBJECT (cell), "column"));
  
  gtk_tree_model_get_iter_from_string (GTK_TREE_MODEL (
                                                    UI_ACT.tree_store_actions),
                                       &iter,
                                       path_string);
  gtk_tree_model_get (GTK_TREE_MODEL (UI_ACT.tree_store_actions),
                      &iter,
                      0, &action,
                      -1);
  
  // On vérifie si le texte contient bien un nombre flottant
  convertion = common_text_str_to_double (new_text, 0, TRUE, 1., TRUE);
  if (!isnan (convertion))
  {
    BUG (_1990_action_psi_change (p,
                                  action,
                                  (uint8_t) (column - 3U),
                                  m_f (convertion, FLOTTANT_UTILISATEUR)),
        )
  }
  
  return;
}


/**
 * \brief Ajout d'une nouvelle action depuis le menu de la barre d'outils.
 * \param menuitem : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée,
 *     - erreur d'allocation mémoire,
 *     - #_1990_action_ajout,
 *     - type d'action inconnu.
 */
void
_1990_gtk_nouvelle_action (GtkMenuItem *menuitem,
                           Projet      *p)
{
  uint8_t type = 0;
  GList  *list_parcours;
  
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_ACT.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Actions"); )
  
  list_parcours = UI_ACT.items_type_action;
  
  while (list_parcours != NULL)
  {
    if ((GTK_IS_MENU_ITEM (menuitem)) &&
        ((GtkMenuItem *) list_parcours->data == menuitem))
    {
      char        *tmp;
      Action      *action;
      GtkTreePath *path;
      
      BUGCRIT (tmp = g_strdup_printf ("%s %u",
                                      gettext ("Sans nom"),
                                      g_list_length (p->actions)),
               ,
               (gettext ("Erreur d'allocation mémoire.\n")); )
      // On crée l'action en fonction de la catégorie sélectionnée dans le menu
      // déroulant.
      BUG (action = _1990_action_ajout (p, type, tmp), , free (tmp); )
      free (tmp);
      
      path = gtk_tree_model_get_path (GTK_TREE_MODEL (
                                                    UI_ACT.tree_store_actions),
                                   _1990_action_Iter_fenetre_renvoie (action));
      gtk_tree_view_set_cursor (GTK_TREE_VIEW (UI_ACT.tree_view_actions),
                                path,
                                gtk_tree_view_get_column (GTK_TREE_VIEW (
                                                     UI_ACT.tree_view_actions),
                                                          0),
                                TRUE);
      gtk_tree_path_free (path);
      
      return;
    }
    type++;
    list_parcours = g_list_next (list_parcours);
  }
  
  FAILCRIT ( ,
            (gettext ("Impossible de trouver le type d'action correspondant à cette action.\n")); )
}


/**
 * \brief Supprimer l'action sélectionnée.
 * \param toolbutton : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée,
 *     - erreur d'allocation mémoire,
 *     - #_1990_action_nom_change.
 */
void
_1990_gtk_menu_suppr_action_activate (GtkWidget *toolbutton,
                                      Projet    *p)
{
  GtkTreeIter   iter;
  GtkTreeModel *model;
  Action       *action;
  
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_ACT.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Actions"); )
  
  if (!gtk_tree_selection_get_selected (UI_ACT.tree_select_actions,
                                        &model,
                                        &iter))
  {
    return;
  }
  
  // On récupère l'action sélectionnée
  gtk_tree_model_get (model, &iter, 0, &action, -1);
  
  // Et on la supprime ainsi que les charges la contenant
  BUG (_1990_action_free_1 (p, action), )
  
  return;
}


/********************** Tout ce qui concerne les charges *********************/


/**
 * \brief Gestion des touches du composant treeview charge.
 *        Si c'est la touche SUPPR, la charge est supprimée.
 * \param widget : composant tree_view à l'origine de l'évènement,
 * \param event : description de la touche pressée,
 * \param p : la variable projet.
 * \return TRUE si la touche DELETE est pressée, FALSE sinon.\n
 *   Echec : FALSE.
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
gboolean
_1990_gtk_actions_charge_key_press_event (GtkWidget *widget,
                                          GdkEvent  *event,
                                          Projet    *p)
{
  BUGPARAMCRIT (p, "%p", p, FALSE)
  BUGCRIT (UI_ACT.builder,
           FALSE,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Actions"); )
  
  if (event->key.keyval == GDK_KEY_Delete)
  {
    _1990_gtk_menu_suppr_charge_clicked (widget, p);
    return TRUE;
  }
  
  return FALSE; /* Pour permettre aux autres touches d'être fonctionnelles  */
}


/**
 * \brief Lance la fenêtre d'édition de la charge sélectionnée en cas de
 *        double-clique dans le tree-view charge.
 * \param widget : composant à l'origine de l'évènement,
 * \param event : Information sur l'évènement,
 * \param p : la variable projet.
 * \return TRUE s'il y a édition via un double-clique, FALSE sinon.\n
 *   Echec : FALSE :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
gboolean
_1990_gtk_actions_charge_double_clicked (GtkWidget *widget,
                                         GdkEvent  *event,
                                         Projet    *p)
{
  BUGPARAMCRIT (p, "%p", p, FALSE)
  BUGCRIT (UI_ACT.builder,
           FALSE,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Actions"); )
  
  if ((event->type == GDK_2BUTTON_PRESS) &&
      (gtk_widget_get_sensitive (GTK_WIDGET (gtk_builder_get_object
                       (UI_ACT.builder, "1990_actions_toolbar_charge_edit")))))
  {
    _1990_gtk_menu_edit_charge_clicked (widget, p);
    return TRUE;
  }
  else
  {
    return common_gtk_treeview_button_press_unselect (GTK_TREE_VIEW(widget),
                                                      (GdkEventButton*) event,
                                                      p);
  }
}


/**
 * \brief Évènement "drag-begin" vide mais nécessaire au treeview charge pour
 *        que la fonction DnD marche.
 * \param widget : le composant à l'origine de l'évènement,
 * \param drag_context : drag_context,
 * \param data : non défini.
 * \return Rien.
 */
void
_1990_gtk_actions_tree_view_drag_begin (GtkWidget      *widget,
                                        GdkDragContext *drag_context,
                                        void           *data)
{
  return;
}


/**
 * \brief Pour éditer le nom des charges via la fenêtre d'actions.
 * \param cell : la cellule éditée,
 * \param path_string : chemin vers la cellule,
 * \param new_text : nouveau texte,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée,
 *     - #EF_charge_renomme.
 */
void
_1990_gtk_tree_view_charges_description_edited (
                                              GtkCellRendererText *cell,
                                              gchar               *path_string,
                                              gchar               *new_text,
                                              Projet              *p)
{
  GtkTreeModel *model;
  GtkTreeIter   iter, iter_action;
  GtkTreeModel *model_action;
  Action       *action;
  Charge       *charge;
  
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_ACT.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Actions"); )
  
  // On recherche la charge en cours d'édition
  model = gtk_tree_view_get_model (GTK_TREE_VIEW (UI_ACT.tree_view_charges));
  
  if (!gtk_tree_selection_get_selected (UI_ACT.tree_select_actions,
                                        &model_action,
                                        &iter_action))
  {
    return;
  }
  gtk_tree_model_get (model_action, &iter_action, 0, &action, -1);
  
  gtk_tree_model_get_iter_from_string (model, &iter, path_string);
  gtk_tree_model_get (model, &iter, 0, &charge, -1);
  
  BUG (EF_charge_renomme (p, charge, new_text), )
  
  return;
}


/**
 * \brief Édite les charges sélectionnées.
 * \param toolbutton : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée,
 *     - #EF_gtk_charge_noeud ou #EF_gtk_charge_barre_ponctuelle ou
 *       #EF_gtk_charge_barre_repartie_uniforme.
 */
void
_1990_gtk_menu_edit_charge_clicked (GtkWidget *toolbutton,
                                    Projet    *p)
{
  GtkTreeIter   iter;
  GtkTreeModel *model;
  Action       *action;
  Charge       *charge;
  GList        *list, *list_parcours;
  
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_ACT.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Actions"); )
  
  // On récupère l'action qui contient les charges à éditer.
  if (!gtk_tree_selection_get_selected (UI_ACT.tree_select_actions,
                                        &model,
                                        &iter))
  {
    return;
  }
  gtk_tree_model_get (model, &iter, 0, &action, -1);
  
  // On récupère la liste des charges à éditer.
  list = gtk_tree_selection_get_selected_rows (UI_ACT.tree_select_charges,
                                               &model);
  
  list_parcours = g_list_first (list);
  for ( ; list_parcours != NULL; list_parcours = g_list_next (list_parcours))
  {
    if (gtk_tree_model_get_iter (model,
                                 &iter,
                                 (GtkTreePath *) list_parcours->data))
    {
      // Et on les édite les unes après les autres.
      gtk_tree_model_get (model, &iter, 0, &charge, -1);
      
      switch (charge->type)
      {
        case CHARGE_NOEUD :
        {
          BUG (EF_gtk_charge_noeud (p, action, charge),
               ,
               g_list_foreach (list, (GFunc) gtk_tree_path_free, NULL);
                 g_list_free (list); )
          break;
        }
        case CHARGE_BARRE_PONCTUELLE :
        {
          BUG (EF_gtk_charge_barre_ponctuelle (p, action, charge),
               ,
               g_list_foreach (list, (GFunc) gtk_tree_path_free, NULL);
                 g_list_free (list); )
          break;
        }
        case CHARGE_BARRE_REPARTIE_UNIFORME :
        {
          BUG (EF_gtk_charge_barre_repartie_uniforme (p, action, charge),
               ,
               g_list_foreach (list, (GFunc) gtk_tree_path_free, NULL);
                 g_list_free (list); )
          break;
        }
        default :
        {
          FAILINFO ( , 
                    (gettext ("Type de charge %d inconnu.\n"), charge->type);
                      g_list_foreach (list, (GFunc) gtk_tree_path_free, NULL);
                      g_list_free (list); )
          break;
        }
      }
    }
  }
  g_list_foreach (list, (GFunc) gtk_tree_path_free, NULL);
  g_list_free (list);
  
  return;
}


/**
 * \brief Supprimer les actions sélectionnées.
 * \param toolbutton : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée,
 *     - #EF_charge_supprime.
 */
void
_1990_gtk_menu_suppr_charge_clicked (GtkWidget *toolbutton,
                                     Projet    *p)
{
  GtkTreeIter   iter;
  GtkTreeModel *model;
  Action       *action;
  Charge       *charge;
  GList        *list, *list_fixe, *list_parcours;
  
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_ACT.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Actions"); )
  
  // On récupère l'action qui contient les charges à supprimer.
  if (!gtk_tree_selection_get_selected (UI_ACT.tree_select_actions,
                                        &model,
                                        &iter))
  {
    return;
  }
  
  gtk_tree_model_get (model, &iter, 0, &action, -1);
  
  // On récupère la liste des charges à supprimer.
  list = gtk_tree_selection_get_selected_rows (
                                           UI_ACT.tree_select_charges, &model);
  
  // On converti les lignes en ligne fixe sinon, le tree-view-charges se perd
  // lorsqu'on supprime les lignes dues au déplacement des actions en cours de
  // route.
  list_parcours = g_list_last (list);
  list_fixe = NULL;
  for ( ;
       list_parcours != NULL;
       list_parcours = g_list_previous (list_parcours))
  {
    list_fixe = g_list_append (list_fixe,
                               gtk_tree_row_reference_new (model,
                                           (GtkTreePath *) list_parcours->data)
                              );
  }
  g_list_free_full (list, (GDestroyNotify) gtk_tree_path_free);
  
  // On supprime les charges sélectionnées. Pas besoin de remettre à jour le
  // tree-view, c'est inclus dans EF_charge_supprime
  list_parcours = g_list_first (list_fixe);
  for ( ; list_parcours != NULL; list_parcours = g_list_next (list_parcours))
  {
    if (gtk_tree_model_get_iter (model,
                                 &iter,
                                 gtk_tree_row_reference_get_path(
                                  (GtkTreeRowReference *) list_parcours->data))
       )
    {
      gtk_tree_model_get (model, &iter, 0, &charge, -1);
      BUG (EF_charge_supprime (p, action, charge),
           ,
           g_list_free_full (list_fixe,
                             (GDestroyNotify) gtk_tree_row_reference_free); )
    }
  }
  g_list_free_full (list_fixe, (GDestroyNotify) gtk_tree_row_reference_free);
  
  return;
}


/****************** Fonction permettant de créer les charges *****************/


/**
 * \brief Ouvre la fenêtre permettant d'ajouter une charge nodale.
 * \param menuitem : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée,
 *     - #EF_gtk_charge_noeud.
 */
void
_1990_gtk_nouvelle_charge_nodale (GtkMenuItem *menuitem,
                                  Projet      *p)
{
  GtkTreeIter   iter_action;
  GtkTreeModel *model_action;
  Action       *action;
  
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_ACT.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Actions"); )
  
  if (!gtk_tree_selection_get_selected (UI_ACT.tree_select_actions,
                                        &model_action,
                                        &iter_action))
  {
    return;
  }
  
  gtk_tree_model_get (model_action, &iter_action, 0, &action, -1);
  
  BUG (EF_gtk_charge_noeud (p, action, NULL), )
}


/**
 * \brief Ouvre la fenêtre permettant d'ajouter une charge ponctuelle sur
 *        barre.
 * \param menuitem : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée,
 *     - #EF_gtk_charge_barre_ponctuelle.
 */
void
_1990_gtk_nouvelle_charge_barre_ponctuelle (GtkMenuItem *menuitem,
                                            Projet      *p)
{
  GtkTreeIter   iter_action;
  GtkTreeModel *model_action;
  Action       *action;
  
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_ACT.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Actions"); )
  
  if (!gtk_tree_selection_get_selected (UI_ACT.tree_select_actions,
                                        &model_action,
                                        &iter_action))
  {
    return;
  }
  
  gtk_tree_model_get (model_action, &iter_action, 0, &action, -1);
  
  BUG (EF_gtk_charge_barre_ponctuelle (p, action, NULL), )
  
  return;
}


/**
 * \brief Ouvre la fenêtre permettant d'ajouter une charge répartie uniforme.
 * \param menuitem : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée,
 *     - #EF_gtk_charge_barre_repartie_uniforme.
 */
void
_1990_gtk_nouvelle_charge_barre_repartie_uniforme (GtkMenuItem *menuitem,
                                                   Projet      *p)
{
  GtkTreeIter   iter_action;
  GtkTreeModel *model_action;
  Action       *action;
  
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_ACT.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Actions"); )
  
  if (!gtk_tree_selection_get_selected (UI_ACT.tree_select_actions,
                                        &model_action,
                                        &iter_action))
  {
    return;
  }
  
  gtk_tree_model_get (model_action, &iter_action, 0, &action, -1);
  
  BUG (EF_gtk_charge_barre_repartie_uniforme (p, action, NULL), )
  
  return;
}


/**
 * \brief Affiche le nom de l'action dans le tree_view.
 * \param tree_column : composant à l'origine de l'évènement,
 * \param cell : la cellule en cours d'édition,
 * \param tree_model : le mode en cours d'édition,
 * \param iter : la ligne en cours d'édition,
 * \param data2 : la variable projet.
 * \return Rien.
 */
void
_1990_gtk_actions_render_0 (GtkTreeViewColumn *tree_column,
                            GtkCellRenderer   *cell,
                            GtkTreeModel      *tree_model,
                            GtkTreeIter       *iter,
                            gpointer           data2)
{
  Action *action;
  
  gtk_tree_model_get (tree_model, iter, 0, &action, -1);
  BUGPARAM (action, "%p", action, )
  
  g_object_set (cell, "text", _1990_action_nom_renvoie (action), NULL);
  
  return;
}


/**
 * \brief Affiche le type de l'action dans le tree_view.
 * \brief tree_column : composant à l'origine de l'évènement,
 * \brief cell : la cellule en cours d'édition,
 * \brief tree_model : le mode en cours d'édition,
 * \brief iter : la ligne en cours d'édition,
 * \brief data2 : la variable projet.
 * \return Rien.
 */
void
_1990_gtk_actions_render_1 (GtkTreeViewColumn *tree_column,
                            GtkCellRenderer   *cell,
                            GtkTreeModel      *tree_model,
                            GtkTreeIter       *iter,
                            gpointer           data2)
{
  Action *action;
  Projet *p = data2;
  
  gtk_tree_model_get (tree_model, iter, 0, &action, -1);
  BUGPARAM (action, "%p", action, )
  
  g_object_set (cell,
                "text",
                _1990_action_bat_txt_type (_1990_action_type_renvoie (action),
                                           p->parametres.norme),
                NULL);
  
  return;
}


/**
 * \brief Affiche le coefficient &psi;<sub>0</sub> de l'action dans le
 *        tree_view.
 * \param tree_column : composant à l'origine de l'évènement,
 * \param cell : la cellule en cours d'édition,
 * \param tree_model : le mode en cours d'édition,
 * \param iter : la ligne en cours d'édition,
 * \param data2 : la variable projet.
 * \return Rien.
 */
void
_1990_gtk_actions_render_2 (GtkTreeViewColumn *tree_column,
                            GtkCellRenderer   *cell,
                            GtkTreeModel      *tree_model,
                            GtkTreeIter       *iter,
                            gpointer           data2)
{
  Action *action;
  char    tmp[30];
  
  gtk_tree_model_get (tree_model, iter, 0, &action, -1);
  BUGPARAM (action, "%p", action, )
  
  conv_f_c (_1990_action_psi_renvoie_0 (action), tmp, DECIMAL_SANS_UNITE);
  
  g_object_set (cell, "text", tmp, NULL);
  
  return;
}


/**
 * \brief Affiche le coefficient &psi;<sub>1</sub> de l'action dans le
 *        tree_view.
 * \param tree_column : composant à l'origine de l'évènement,
 * \param cell : la cellule en cours d'édition,
 * \param tree_model : le mode en cours d'édition,
 * \param iter : la ligne en cours d'édition,
 * \param data2 : la variable projet.
 * \return Rien.
 */
void
_1990_gtk_actions_render_3 (GtkTreeViewColumn *tree_column,
                            GtkCellRenderer   *cell,
                            GtkTreeModel      *tree_model,
                            GtkTreeIter       *iter,
                            gpointer           data2)
{
  Action *action;
  char    tmp[30];
  
  gtk_tree_model_get (tree_model, iter, 0, &action, -1);
  BUGPARAM (action, "%p", action, )
  
  conv_f_c (_1990_action_psi_renvoie_1 (action), tmp, DECIMAL_SANS_UNITE);
  
  g_object_set (cell, "text", tmp, NULL);
  
  return;
}


/**
 * \brief Affiche le coefficient &psi;<sub>2</sub> de l'action dans le
 *        tree_view.
 * \param tree_column : composant à l'origine de l'évènement,
 * \param cell : la cellule en cours d'édition,
 * \param tree_model : le mode en cours d'édition,
 * \param iter : la ligne en cours d'édition,
 * \param data2 : la variable projet.
 * \return Rien.
 */
void
_1990_gtk_actions_render_4 (GtkTreeViewColumn *tree_column,
                            GtkCellRenderer   *cell,
                            GtkTreeModel      *tree_model,
                            GtkTreeIter       *iter,
                            gpointer           data2)
{
  Action *action;
  char    tmp[30];
  
  gtk_tree_model_get (tree_model, iter, 0, &action, -1);
  BUGPARAM (action, "%p", action, )
  
  conv_f_c (_1990_action_psi_renvoie_2 (action), tmp, DECIMAL_SANS_UNITE);
  
  g_object_set (cell, "text", tmp, NULL);
  
  return;
}


/**
 * \brief Affiche le nom de la charge dans le tree_view.
 * \param tree_column : composant à l'origine de l'évènement,
 * \param cell : la cellule en cours d'édition,
 * \param tree_model : le mode en cours d'édition,
 * \param iter : la ligne en cours d'édition,
 * \param data2 : la variable projet.
 * \return Rien.
 */
void
_1990_gtk_actions_charge_render_0 (GtkTreeViewColumn *tree_column,
                                   GtkCellRenderer   *cell,
                                   GtkTreeModel      *tree_model,
                                   GtkTreeIter       *iter,
                                   gpointer           data2)
{
  Charge *charge;
  
  gtk_tree_model_get (tree_model, iter, 0, &charge, -1);
  BUGPARAM (charge, "%p", charge, )
  
  g_object_set (cell, "text", charge->nom, NULL);
  
  return;
}


/**
 * \brief Affiche le type de la charge dans le tree_view.
 * \param tree_column : composant à l'origine de l'évènement,
 * \param cell : la cellule en cours d'édition,
 * \param tree_model : le mode en cours d'édition,
 * \param iter : la ligne en cours d'édition,
 * \param data2 : la variable projet.
 * \return Rien.
 */
void
_1990_gtk_actions_charge_render_1 (GtkTreeViewColumn *tree_column,
                                   GtkCellRenderer   *cell,
                                   GtkTreeModel      *tree_model,
                                   GtkTreeIter       *iter,
                                   gpointer           data2)
{
  Charge *charge;
  
  gtk_tree_model_get (tree_model, iter, 0, &charge, -1);
  BUGPARAM (charge, "%p", charge, )
  
  switch (charge->type)
  {
    case CHARGE_NOEUD :
    {
      g_object_set (cell, "text", gettext ("Ponctuelle sur noeud"), NULL);
      break;
    }
    case CHARGE_BARRE_PONCTUELLE :
    {
      g_object_set (cell, "text", gettext ("Ponctuelle sur barre"), NULL);
      break;
    }
    case CHARGE_BARRE_REPARTIE_UNIFORME :
    {
      g_object_set (cell,
                    "text",
                    gettext ("Répartie uniforme sur barre"),
                    NULL);
      break;
    }
    default :
    {
      FAILCRIT (, (gettext ("Type de charge %d inconnu.\n"), charge->type); )
      break;
    }
  }
  
  return;
}


/**
 * \brief Affiche la description de la charge dans le tree_view..
 * \param tree_column : composant à l'origine de l'évènement,
 * \param cell : la cellule en cours d'édition,
 * \param tree_model : le mode en cours d'édition,
 * \param iter : la ligne en cours d'édition,
 * \param data2 : la variable projet.
 * \return Rien.
 */
void
_1990_gtk_actions_charge_render_2 (GtkTreeViewColumn *tree_column,
                                   GtkCellRenderer   *cell,
                                   GtkTreeModel      *tree_model,
                                   GtkTreeIter       *iter,
                                   gpointer           data2)
{
  Charge *charge;
  char   *tmp;
  
  gtk_tree_model_get (tree_model, iter, 0, &charge, -1);
  BUGPARAM (charge, "%p", charge, )
  
  switch (charge->type)
  {
    case CHARGE_NOEUD :
    {
      tmp = EF_charge_noeud_description (charge);
      g_object_set (cell, "text", tmp, NULL);
      free (tmp);
      break;
    }
    case CHARGE_BARRE_PONCTUELLE :
    {
      tmp = EF_charge_barre_ponctuelle_description (charge);
      g_object_set (cell, "text", tmp, NULL);
      free (tmp);
      break;
    }
    case CHARGE_BARRE_REPARTIE_UNIFORME :
    {
      tmp = EF_charge_barre_repartie_uniforme_description (charge);
      g_object_set (cell, "text", tmp, NULL);
      free (tmp);
      break;
    }
    default :
    {
      FAILCRIT (, (gettext ("Type de charge %d inconnu.\n"), charge->type); )
      break;
    }
  }
  
  return;
}


/********** Fonction créant la fenêtre de gestion des actions ****************/


/**
 * \brief Création de la fenêtre Actions.
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique impossible à générer.
 */
void
_1990_gtk_actions (Projet *p)
{
  GList *list_parcours;
  
  BUGPARAMCRIT (p, "%p", p, )
  
  if (UI_ACT.builder != NULL)
  {
    gtk_window_present (GTK_WINDOW (UI_ACT.window));
    return;
  }
  
  UI_ACT.builder = gtk_builder_new ();
  BUGCRIT (gtk_builder_add_from_resource (UI_ACT.builder,
                                        "/org/2lgc/codegui/ui/1990_actions.ui",
                                          NULL) != 0,
          ,
          (gettext ("La génération de la fenêtre %s a échouée.\n"),
                    "Actions"); )
  gtk_builder_connect_signals (UI_ACT.builder, p);
  
  UI_ACT.window = GTK_WIDGET (gtk_builder_get_object (
                                       UI_ACT.builder, "1990_actions_window"));
  UI_ACT.tree_store_actions = GTK_TREE_STORE (gtk_builder_get_object (
                            UI_ACT.builder, "1990_actions_tree_store_action"));
  UI_ACT.tree_view_actions = GTK_TREE_VIEW (gtk_builder_get_object (
                              UI_ACT.builder, "1990_actions_treeview_action"));
  UI_ACT.tree_select_actions = GTK_TREE_SELECTION (
                       gtk_tree_view_get_selection (UI_ACT.tree_view_actions));
  
  g_object_set (gtk_builder_get_object (UI_ACT.builder,
                                        "1990_actions_treeview_cell1"),
                "model", UI_ACT.choix_type_action,
                NULL);
  
  g_object_set_data (gtk_builder_get_object (UI_ACT.builder,
                                             "1990_actions_treeview_cell2"),
                     "column", GINT_TO_POINTER(3));
  g_object_set_data (gtk_builder_get_object (UI_ACT.builder,
                                             "1990_actions_treeview_cell3"),
                     "column", GINT_TO_POINTER(4));
  g_object_set_data (gtk_builder_get_object (UI_ACT.builder,
                                             "1990_actions_treeview_cell4"),
                     "column", GINT_TO_POINTER(5));
  
  gtk_tree_view_column_set_cell_data_func (
    GTK_TREE_VIEW_COLUMN (gtk_builder_get_object (UI_ACT.builder,
                                             "1990_actions_treeview_column0")),
    GTK_CELL_RENDERER (gtk_builder_get_object (UI_ACT.builder,
                                               "1990_actions_treeview_cell0")),
    _1990_gtk_actions_render_0,
    p,
    NULL);
  gtk_tree_view_column_set_cell_data_func (
    GTK_TREE_VIEW_COLUMN (gtk_builder_get_object (UI_ACT.builder,
                                             "1990_actions_treeview_column1")),
    GTK_CELL_RENDERER (gtk_builder_get_object (UI_ACT.builder,
                                               "1990_actions_treeview_cell1")),
    _1990_gtk_actions_render_1,
    p,
    NULL);
  gtk_tree_view_column_set_cell_data_func (
    GTK_TREE_VIEW_COLUMN (gtk_builder_get_object (UI_ACT.builder,
                                             "1990_actions_treeview_column2")),
    GTK_CELL_RENDERER (gtk_builder_get_object (UI_ACT.builder,
                                               "1990_actions_treeview_cell2")),
    _1990_gtk_actions_render_2,
    p,
    NULL);
  gtk_tree_view_column_set_cell_data_func (
    GTK_TREE_VIEW_COLUMN (gtk_builder_get_object (UI_ACT.builder,
                                             "1990_actions_treeview_column3")),
    GTK_CELL_RENDERER (gtk_builder_get_object (UI_ACT.builder,
                                               "1990_actions_treeview_cell3")),
    _1990_gtk_actions_render_3,
    p,
    NULL);
  gtk_tree_view_column_set_cell_data_func (
    GTK_TREE_VIEW_COLUMN (gtk_builder_get_object (UI_ACT.builder,
                                             "1990_actions_treeview_column4")),
    GTK_CELL_RENDERER (gtk_builder_get_object (UI_ACT.builder,
                                               "1990_actions_treeview_cell4")),
    _1990_gtk_actions_render_4,
    p,
    NULL);
  
  gtk_tree_view_column_set_cell_data_func (
    GTK_TREE_VIEW_COLUMN (gtk_builder_get_object (UI_ACT.builder,
                                      "1990_actions_treeview_charge_column0")),
    GTK_CELL_RENDERER (gtk_builder_get_object (UI_ACT.builder,
                                        "1990_actions_treeview_charge_cell0")),
    _1990_gtk_actions_charge_render_0,
    p,
    NULL);
  gtk_tree_view_column_set_cell_data_func (
    GTK_TREE_VIEW_COLUMN (gtk_builder_get_object (UI_ACT.builder,
                                      "1990_actions_treeview_charge_column1")),
    GTK_CELL_RENDERER (gtk_builder_get_object (UI_ACT.builder,
                                        "1990_actions_treeview_charge_cell1")),
    _1990_gtk_actions_charge_render_1,
    p,
    NULL);
  gtk_tree_view_column_set_cell_data_func (
    GTK_TREE_VIEW_COLUMN (gtk_builder_get_object (UI_ACT.builder,
                                      "1990_actions_treeview_charge_column2")),
    GTK_CELL_RENDERER (gtk_builder_get_object (UI_ACT.builder,
                                        "1990_actions_treeview_charge_cell2")),
    _1990_gtk_actions_charge_render_2,
    p,
    NULL);
  
  // Affiche la liste des actions
  list_parcours = p->actions;
  while (list_parcours != NULL)
  {
    Action *action = list_parcours->data;
    
    gtk_tree_store_append (UI_ACT.tree_store_actions,
                           _1990_action_Iter_fenetre_renvoie (action),
                           NULL);
    gtk_tree_store_set (UI_ACT.tree_store_actions,
                        _1990_action_Iter_fenetre_renvoie (action),
                        0, action,
                        -1);
    
    list_parcours = g_list_next (list_parcours);
  }
  
  UI_ACT.tree_store_charges = GTK_TREE_STORE (gtk_builder_get_object (
                            UI_ACT.builder, "1990_actions_tree_store_charge"));
  UI_ACT.tree_view_charges = GTK_TREE_VIEW (gtk_builder_get_object (
                              UI_ACT.builder, "1990_actions_treeview_charge"));
  UI_ACT.tree_select_charges = GTK_TREE_SELECTION (
                       gtk_tree_view_get_selection (UI_ACT.tree_view_charges));
  
  gtk_menu_tool_button_set_menu (
    GTK_MENU_TOOL_BUTTON (gtk_builder_get_object (UI_ACT.builder,
                                         "1990_actions_toolbar_action_ajout")),
    GTK_WIDGET (UI_ACT.type_action));
  // Pour éviter que le menu type_action soit libéré en même temps que le
  // bouton 1990_actions_toolbar_action_ajout. Uniquement pour Windows,
  g_object_ref (UI_ACT.type_action);

  gtk_menu_tool_button_set_menu (
    GTK_MENU_TOOL_BUTTON (gtk_builder_get_object (UI_ACT.builder,
                                         "1990_actions_toolbar_charge_ajout")),
    GTK_WIDGET (UI_ACT.type_charges));
  gtk_widget_set_sensitive (
    GTK_WIDGET (gtk_builder_get_object (UI_ACT.builder,
                                        "1990_actions_toolbar_charge_ajout")),
    FALSE);
  
  // Défini le comportement du glissé etat vers dispo
  gtk_drag_source_set (GTK_WIDGET (UI_ACT.tree_view_charges),
                       GDK_BUTTON1_MASK,
                       drag_targets_actions,
                       1,
                       GDK_ACTION_MOVE); 
  gtk_drag_dest_set (GTK_WIDGET (UI_ACT.tree_view_actions),
                     GTK_DEST_DEFAULT_ALL,
                     drag_targets_actions,
                     1,
                     GDK_ACTION_MOVE);
  
  gtk_window_set_transient_for (GTK_WINDOW (UI_ACT.window),
                                GTK_WINDOW (UI_GTK.window));
  
  return;
}

#endif

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
