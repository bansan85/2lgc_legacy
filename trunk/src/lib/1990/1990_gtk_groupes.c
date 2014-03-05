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
#include <math.h>

#include "1990_action.h"
#include "1990_groupe.h"
#include "1990_ponderations.h"
#include "1990_combinaisons.h"
#include "common_erreurs.h"
#include "common_projet.h"
#include "common_tooltip.h"
#include "common_gtk.h"

const GtkTargetEntry drag_targets_groupes_1[] = {{(gchar*)PACKAGE"1_SAME_PROC",
                                                  GTK_TARGET_SAME_APP,
                                                  0}};
const GtkTargetEntry drag_targets_groupes_2[] = {{(gchar*)PACKAGE"2_SAME_PROC",
                                                  GTK_TARGET_SAME_APP,
                                                  0}};
const GtkTargetEntry drag_targets_groupes_3[] = {{(gchar*)PACKAGE"1_SAME_PROC",
                                                  GTK_TARGET_SAME_APP,
                                                  0},
                                                 {(gchar*)PACKAGE"2_SAME_PROC",
                                                  GTK_TARGET_SAME_APP,
                                                  0}};
 

GTK_WINDOW_KEY_PRESS (_1990, groupes);


GTK_WINDOW_CLOSE(_1990, groupes);


void *
_1990_gtk_get_groupe (GtkTreeModel *tree_model, GtkTreeIter *iter)
/**
 * \brief Renvoie le groupe de l'élément iter.
 * \param tree_model : tree_model contenant la liste des éléments,
 * \param iter : contient la ligne à étudier.
 * \return Le groupe.
 */
{
  void       *retour;
  GtkTreeIter iter_parent;
  
  if (gtk_tree_model_iter_parent (tree_model, &iter_parent, iter))
    gtk_tree_model_get (tree_model, &iter_parent, 0, &retour, -1);
  else
    gtk_tree_model_get (tree_model, iter, 0, &retour, -1);
  
  return retour;
}


void
_1990_gtk_groupes_tree_view_etat_cursor_changed (GtkTreeView *tree_view,
                                                 Projet      *p)
/**
 * \brief Evènement lorsqu'il y a un changement de ligne sélectionnée. Il
 *        permet de mettre à jour si le groupe sélectionné est de type OR, XOR
 *        ou AND.
 * \param tree_view : composant tree_view à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 * Echec :
 *   - p == NULL,
 *   - Aucun niveau de groupes,
 *   - interface graphique non initialisée,
 *   - la combinaison n'est ni or ni xor ni and.
 */
{
  GtkTreeModel *model;
  GtkTreeIter   iter;
  Groupe       *groupe;
  
  BUGPARAMCRIT (p, "%p", p, )
  INFO (p->niveaux_groupes,
        ,
        (gettext ("Le projet ne possède pas de niveaux de groupes.\n"));)
  BUGCRIT (UI_GRO.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Groupes");)
  
  // Si aucune sélection.
  if (!gtk_tree_selection_get_selected (UI_GRO.tree_select_etat,
                                        &model,
                                        &iter))
    return;
  
  groupe = _1990_gtk_get_groupe (model, &iter);
  
  // On active le toggle_button correspondant au type de combinaison du groupe.
  switch (groupe->type_combinaison)
  {
    case GROUPE_COMBINAISON_OR :
    {
      gtk_toggle_tool_button_set_active (GTK_TOGGLE_TOOL_BUTTON (
                                                        UI_GRO.item_groupe_or),
                                         TRUE);
      break;
    }
    case GROUPE_COMBINAISON_XOR :
    {
      gtk_toggle_tool_button_set_active (GTK_TOGGLE_TOOL_BUTTON (
                                                       UI_GRO.item_groupe_xor),
                                         TRUE);
      break;
    }
    case GROUPE_COMBINAISON_AND :
    {
      gtk_toggle_tool_button_set_active (GTK_TOGGLE_TOOL_BUTTON (
                                                       UI_GRO.item_groupe_and),
                                         TRUE);
      break;
    }
    default :
    {
      FAILCRIT ( ,
                (gettext ("Le groupe %s n'est combiné ni de type OR, XOR ou AND.\n"),
                       groupe->nom);)
      break;
    }
  }
  
  return;
}


GTK_WINDOW_DESTROY (
  _1990,
  groupes,
  g_signal_handler_block (UI_GRO.tree_view_etat,
                          g_signal_handler_find (G_OBJECT (
                                                        UI_GRO.tree_view_etat),
                          G_SIGNAL_MATCH_FUNC,
                          0,
                          0,
                          NULL,
                          _1990_gtk_groupes_tree_view_etat_cursor_changed,
                          NULL));
);


gboolean
_1990_gtk_groupes_affiche_niveau (Projet      *p,
                                  unsigned int niveau)
/**
 * \brief Affiche le niveau souhaité dans l'interface graphique.
 * \param p : variable projet,
 * \param niveau : niveau à afficher.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL,
 *     - aucun niveau de groupes,
 *     - interface graphique non initialisée,
 *     - niveau introuvable,
 *     - erreur d'allocation mémoire.
 */
{
  Niveau_Groupe *niveau_groupe;
  unsigned int   dispo_max, i;
  char          *dispos;
  gboolean       premier = TRUE;
  GtkTreePath   *path;
  GList         *list_parcours, *liste_actions;
  
  BUGPARAMCRIT (p, "%p", p, FALSE)
  INFO (p->niveaux_groupes,
        FALSE,
        (gettext ("Le projet ne possède pas de niveaux de groupes.\n"));)
  BUGCRIT (UI_GRO.builder,
           FALSE,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Groupes");)
  
  
  // Il convient de bloquer le signal. Sinon, des erreurs peuvent apparaitre
  // si une ligne a été sélectionnée (par exemple la première) à la souris et
  // qu'on demande ensuite de changer de niveau via le spin_button.
  g_signal_handler_block (UI_GRO.tree_view_etat,
    g_signal_handler_find (G_OBJECT (UI_GRO.tree_view_etat),
                           G_SIGNAL_MATCH_FUNC,
                           0,
                           0,
                           NULL,
                           _1990_gtk_groupes_tree_view_etat_cursor_changed,
                           NULL));
  
  // On supprime le contenu des deux composants tree_view.
  gtk_tree_store_clear (UI_GRO.tree_store_etat);
  g_signal_handler_unblock (UI_GRO.tree_view_etat,
    g_signal_handler_find (G_OBJECT (UI_GRO.tree_view_etat),
                           G_SIGNAL_MATCH_FUNC,
                           0,
                           0,
                           NULL,
                           _1990_gtk_groupes_tree_view_etat_cursor_changed,
                           NULL));
  gtk_tree_store_clear (UI_GRO.tree_store_dispo);
  
  // dispo_max contient le nombre d'éléments maximum pouvant être disponible
  // depuis le niveau 'niveau'-1.
  if (niveau == 0)
    liste_actions = p->actions;
  else
  {
    BUG (niveau_groupe = g_list_nth_data (p->niveaux_groupes, niveau - 1),
         FALSE)
    liste_actions = niveau_groupe->groupes;
  }
  dispo_max = g_list_length (liste_actions);
  
  // Ensuite, on initialise un tableau contenant une liste de boolean pour
  // déterminer au fur et à mesure de l'avancement de l'algorithme quels
  // sont les éléments du niveau n-1 encore non placés.
  if (dispo_max != 0)
  {
    BUGCRIT (dispos = (char *) malloc (sizeof (char) * dispo_max),
             FALSE,
             (gettext ("Erreur d'allocation mémoire.\n"));)
    for (i = 0; i < dispo_max; i++)
      dispos[i] = 0;
  }
  else
    dispos = NULL;
  
  BUG (niveau_groupe = g_list_nth_data (p->niveaux_groupes, niveau),
       FALSE,
       free (dispos);)
  list_parcours = niveau_groupe->groupes;
  
  // Parcours le niveau à afficher.
  while (list_parcours != NULL)
  {
    Groupe *groupe = list_parcours->data;
    
    // Ajout de la ligne dans le tree_store.
    gtk_tree_store_append (UI_GRO.tree_store_etat,
                           &groupe->Iter_groupe,
                           NULL);
    gtk_tree_store_set (UI_GRO.tree_store_etat,
                        &groupe->Iter_groupe,
                        0, groupe,
                        -1);
    
    // Sélection de la première ligne du tree_view_etat.
    if (list_parcours->data == list_parcours->data)
    {
      path = gtk_tree_model_get_path (GTK_TREE_MODEL (UI_GRO.tree_store_etat),
                                      &groupe->Iter_groupe);
      gtk_tree_selection_select_path (UI_GRO.tree_select_etat, path);
      gtk_tree_path_free (path);
    }
    
    if (groupe->elements != NULL)
    {
      GList *list_parcours2 = groupe->elements;
      
      do
      {
        Groupe *groupe2 = list_parcours2->data;
        
        // On signale que l'élément a déjà été inséré.
        dispos[g_list_index (liste_actions, groupe2)] = 1;
        
        gtk_tree_store_append (UI_GRO.tree_store_etat,
                               &groupe2->Iter_groupe,
                               &groupe->Iter_groupe);
        gtk_tree_store_set (UI_GRO.tree_store_etat,
                            &groupe2->Iter_groupe,
                            0, groupe2,
                            -1);
        
        list_parcours2 = g_list_next (list_parcours2);
      }
      while (list_parcours2 != NULL);
    }
    
    // Si la dernière fois que la ligne était affiché, elle était 'expand', on
    // rétablit son attribut.
    if (groupe->Iter_expand == 1)
    {
      path = gtk_tree_model_get_path (GTK_TREE_MODEL (UI_GRO.tree_store_etat),
                                      &groupe->Iter_groupe);
      gtk_tree_view_expand_row (UI_GRO.tree_view_etat, path, FALSE);
      gtk_tree_path_free (path);
    }
    list_parcours = g_list_next (list_parcours);
  }
  _1990_gtk_groupes_tree_view_etat_cursor_changed (GTK_TREE_VIEW (
                                                        UI_GRO.tree_view_etat),
                                                   p);
  
  // On affiche tous les éléments disponibles dans le tree_view_dispo.
  for (i = 0; i < dispo_max; i++)
  {
    if (dispos[i] == 0)
    {
      GtkTreeIter Iter;
      
      gtk_tree_store_append (UI_GRO.tree_store_dispo, &Iter, NULL);
      
      // Sélection de la première ligne du tree_view_dispo.
      if (premier == TRUE)
      {
        path = gtk_tree_model_get_path (GTK_TREE_MODEL (
                                                      UI_GRO.tree_store_dispo),
                                        &Iter);
        gtk_tree_selection_select_path (UI_GRO.tree_select_dispo,
                                        path);
        gtk_tree_path_free (path);
        premier = FALSE;
      }
      
      if (niveau == 0)
        gtk_tree_store_set (UI_GRO.tree_store_dispo,
                            &Iter,
                            0, g_list_nth_data(p->actions, i),
                            -1);
      else
      {
        niveau_groupe = g_list_nth_data (p->niveaux_groupes, niveau-1);
        gtk_tree_store_set (UI_GRO.tree_store_dispo,
                            &Iter,
                            0, g_list_nth_data (niveau_groupe->groupes, i),
                            -1);
      }
    }
  }

  free (dispos);
  
  return TRUE;
}


void
_1990_gtk_spin_button_niveau_change (GtkWidget *button,
                                     Projet    *p)
/**
 * \brief Met à jour l'affichage graphique en fonction de la valeur du
 *        spin_button.
 * \param button : composant spin_button ayant réalisé l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 * Echec :
 *   - p == NULL,
 *   - interface graphique non initialisée,
 *   - #_1990_gtk_groupes_affiche_niveau.
 */
{
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_GRO.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Groupes");)
  
  BUG (_1990_gtk_groupes_affiche_niveau (p, GTK_COMMON_SPINBUTTON_AS_UINT(
                                 GTK_SPIN_BUTTON (UI_GRO.spin_button_niveau))),
      )
  return;
}


void
_1990_gtk_button_niveau_suppr_clicked (GtkWidget *button,
                                       Projet    *p)
/**
 * \brief Supprime le niveau en cours.
 * \param button : button ayant réalisé l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 * Echec :
 *   - p == NULL,
 *   - interface graphique non initialisée,
 *   - #_1990_groupe_free_niveau.
 */
{
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_GRO.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Groupes");)
  
  BUG (_1990_groupe_free_niveau (
         p,
         g_list_nth_data (p->niveaux_groupes, GTK_COMMON_SPINBUTTON_AS_UINT (
                                GTK_SPIN_BUTTON ( UI_GRO.spin_button_niveau))),
         FALSE),
      )
  
  return;
}


void
_1990_gtk_button_niveau_ajout_clicked (GtkWidget *button,
                                       Projet *p)
/**
 * \brief Ajoute un niveau de groupe.
 * \param button : button ayant réalisé l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 * Echec :
 *   - p == NULL,
 *   - interface graphique non initialisée,
 *   - #_1990_groupe_ajout_niveau.
 */
{
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_GRO.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Groupes");)
  
  BUG (_1990_groupe_ajout_niveau (p), )
  
  return;
}


void
_1990_gtk_button_groupe_ajout_clicked (GtkWidget *button,
                                       Projet    *p)
/**
 * \brief Ajoute un groupe dans le niveau en cours.
 * \param button : button ayant réalisé l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 * Echec :
 *   - p == NULL,
 *   - interface graphique non initialisée,
 *   - #_1990_groupe_ajout_groupe.
 */
{
  Niveau_Groupe *niveau_groupe;
  
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_GRO.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Groupes");)
  
  BUG (niveau_groupe = g_list_nth_data (p->niveaux_groupes,
                               GTK_COMMON_SPINBUTTON_AS_UINT (GTK_SPIN_BUTTON (
                                                  UI_GRO.spin_button_niveau))),
      )
  BUG (_1990_groupe_ajout_groupe (p,
                                  niveau_groupe,
                                  GROUPE_COMBINAISON_AND,
                                  gettext ("Sans nom")), )
  
  return;
}


gboolean
_1990_gtk_insert_dispo (Projet        *p,
                        void          *data,
                        Niveau_Groupe *niveau)
/**
 * \brief Ajoute dans le tree_view_dispo le numéro de l'élément disponible.
 *        Cette fonction ne gère que l'interface graphique de tree_view_dispo.
 * \param p : la variable projet,
 * \param data : l'élément,
 * \param niveau : niveau à étudier.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
{
  GtkTreeIter       iter;
  GtkTreePath      *path;
  
  BUGPARAMCRIT (p, "%p", p, FALSE)
  BUGCRIT (UI_GRO.builder,
           FALSE,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Groupes");)
  
  gtk_tree_store_append (UI_GRO.tree_store_dispo, &iter, NULL);
  
  path = gtk_tree_model_get_path (GTK_TREE_MODEL (UI_GRO.tree_store_dispo),
                                  &iter);
  gtk_tree_selection_select_path (UI_GRO.tree_select_dispo, path);
  gtk_tree_path_free (path);
  
  gtk_tree_store_set (UI_GRO.tree_store_dispo, &iter, 0, data, -1);
  
  return TRUE;
}


void
_1990_gtk_button_groupe_suppr_clicked (GtkWidget *button, Projet *p)
/**
 * \brief Supprime le groupe sélectionné dans le niveau en cours.
 * \param button : composant spin_button ayant réalisé l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Échec :
 *     - p == NULL,
 *     - interface graphique non initialisée,
 *     - #_1990_gtk_insert_dispo ou #_1990_groupe_retire_element si c'est un
 *       élément d'un groupe,
 *     - #_1990_groupe_free_groupe si c'est un groupe.
 */
{
  GtkTreeModel  *model;
  GtkTreeIter    iter, iter_tmp;
  Niveau_Groupe *niveau_groupe;
  Groupe        *groupe;
  
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_GRO.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Groupes");)
  
  if (!gtk_tree_selection_get_selected (UI_GRO.tree_select_etat,
                                        &model,
                                        &iter))
    return;
  
  niveau_groupe = g_list_nth_data (p->niveaux_groupes,
                               GTK_COMMON_SPINBUTTON_AS_UINT (GTK_SPIN_BUTTON (
                                                  UI_GRO.spin_button_niveau)));
  
  gtk_tree_model_get (model, &iter, 0, &groupe, -1);
  
  if (gtk_tree_model_iter_parent (model, &iter_tmp, &iter))
  {
    Groupe *groupe2;
    
    gtk_tree_model_get (model, &iter_tmp, 0, &groupe2, -1);
    BUG (_1990_gtk_insert_dispo (p, groupe, niveau_groupe), )
    BUG (_1990_groupe_retire_element (p, niveau_groupe, groupe2, groupe), )
  }
  else
    BUG (_1990_groupe_free_groupe (p, niveau_groupe, groupe), )
  
  return;
}


void
_1990_gtk_tree_view_drag_begin (GtkWidget      *widget,
                                GdkDragContext *drag_context,
                                Projet         *p)
/**
 * \brief Défini le composant à l'origine de DnD
 * \param widget : composant à l'origine de l'évènement,
 * \param drag_context : description du DnD,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Échec :
 *     - p == NULL.
 */
{
  BUGPARAMCRIT (p, "%p", p, )
  
  UI_GRO.drag_from = widget;
  
  return;
}


gboolean
_1990_gtk_tree_view_dispo_drag (GtkWidget      *widget,
                                GdkDragContext *drag_context,
                                gint            x,
                                gint            y,
                                guint           tim,
                                Projet         *p)
/**
 * \brief Supprimer les éléments sélectionnées du tree_view_etat.
 * \param widget : composant spin_button ayant réalisé l'évènement,
 * \param drag_context : inutile,
 * \param x : inutile,
 * \param y : inutile,
 * \param tim : inutile,
 * \param p : la variable projet.
 * \return FALSE.
 */
{
  BUGPARAMCRIT (p, "%p", p, FALSE)
  
  _1990_gtk_button_groupe_suppr_clicked (widget, p);
  
  return FALSE;
}


gboolean
_1990_gtk_button_ajout_dispo_proc (Groupe *groupe,
                                   Projet *p)
/**
 * \brief Ajoute les éléments disponibles sélectionnés dans le tree_view 
 *        "dispo" dans le groupe du niveau en cours.
 * \param groupe : groupe du niveau en cours où sera ajouté les éléments
 *                 disponibles sélectionnés,
 * \param p : la variable projet.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL,
 *     - aucun niveau de groupes,
 *     - interface graphique non initialisée,
 *     - #_1990_groupe_ajout_element.
 */
{
  GtkTreeModel  *model1;
  GtkTreeIter  	 iter1;
  Niveau_Groupe *niveau_groupe;
  GList         *list, *list_orig;
  
  BUGPARAMCRIT (p, "%p", p, FALSE)
  INFO (p->niveaux_groupes,
        FALSE,
        (gettext ("Le projet ne possède pas de niveaux de groupes.\n"));)
  BUGCRIT (UI_GRO.builder,
           FALSE,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Groupes");)
  
  BUG (niveau_groupe = g_list_nth_data (p->niveaux_groupes,
                               GTK_COMMON_SPINBUTTON_AS_UINT (GTK_SPIN_BUTTON (
                                                  UI_GRO.spin_button_niveau))),
       FALSE)
  list_orig = gtk_tree_selection_get_selected_rows (UI_GRO.tree_select_dispo,
                                                    &model1);
  list = g_list_last (list_orig);
  
  for (; list != NULL; list = g_list_previous (list))
  {
    Groupe *groupe2;
    
    gtk_tree_model_get_iter (model1, &iter1, (GtkTreePath *) list->data);
    
    // On récupère les informations des lignes sélectionnées
    gtk_tree_model_get (model1, &iter1, 0, &groupe2, -1);
    
    // On ajoute l'élément au groupe
    BUG (_1990_groupe_ajout_element (p, niveau_groupe, groupe, groupe2),
         FALSE,
         g_list_foreach (list_orig, (GFunc) gtk_tree_path_free, NULL);)
  }
  
  g_list_foreach (list_orig, (GFunc) gtk_tree_path_free, NULL);
  
  return TRUE;
}


void
_1990_gtk_button_ajout_dispo_clicked (GtkWidget *button,
                                      Projet    *p)
/**
 * \brief Ajoute les éléments disponibles sélectionnés dans le groupe
 *        sélectionné.
 * \param button : composant spin_button ayant réalisé l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Échec : FALSE :
 *     - p == NULL,
 *     - aucun niveau de groupes,
 *     - interface graphique non initialisée,
 *     - #_1990_gtk_button_ajout_dispo_proc.
 */
{
  GtkTreeIter   iter;
  GtkTreeModel *model;
  Groupe       *groupe;
  
  BUGPARAMCRIT (p, "%p", p, )
  INFO (p->niveaux_groupes,
        ,
        (gettext ("Le projet ne possède pas de niveaux de groupes.\n"));)
  BUGCRIT (UI_GRO.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Groupes");)
  
  /* On vérifie s'il y a des éléments sélectionnés */
  if (!gtk_tree_selection_get_selected (UI_GRO.tree_select_etat,
                                        &model,
                                        &iter))
    return;
  
  groupe = _1990_gtk_get_groupe (model, &iter);
  
  BUG (_1990_gtk_button_ajout_dispo_proc (groupe, p), )
  
  return;
}


void
_1990_gtk_button_ajout_tout_dispo_clicked (GtkWidget *button,
                                           Projet *p)
/**
 * \brief Ajoute tous les éléments disponibles dans le groupe sélectionné.
 * \param button : composant spin_button ayant réalisé l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Échec : FALSE :
 *     - p == NULL,
 *     - aucun niveau de groupes,
 *     - interface graphique non initialisée,
 *     - #_1990_gtk_button_ajout_dispo_proc.
 */
{
  GtkTreeIter   iter;
  GtkTreeModel *model;
  Groupe       *groupe;
  
  BUGPARAMCRIT (p, "%p", p, )
  INFO (p->niveaux_groupes,
        ,
        (gettext ("Le projet ne possède pas de niveaux de groupes.\n"));)
  BUGCRIT (UI_GRO.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Groupes");)
  
  gtk_tree_selection_select_all (UI_GRO.tree_select_dispo);
  
  if (!gtk_tree_selection_get_selected (UI_GRO.tree_select_etat,
                                        &model,
                                        &iter))
    return;
  groupe = _1990_gtk_get_groupe (model, &iter);
  
  BUG (_1990_gtk_button_ajout_dispo_proc (groupe, p), )
  
  return;
}


void
_1990_gtk_tree_view_etat_drag (GtkWidget      *widget,
                               GdkDragContext *drag_context,
                               gint x,
                               gint y,
                               guint tim,
                               Projet *p)
/**
 * \brief Modifie le groupe d'un élément via le DnD.
 * \param widget : composant ayant réalisé l'évènement,
 * \param drag_context : inutile,
 * \param x : inutile,
 * \param y : inutile,
 * \param tim : inutile,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Échec : FALSE :
 *     - p == NULL,
 *     - aucun niveau de groupes,
 *     - interface graphique non initialisée,
 *     - #_1990_gtk_button_ajout_dispo_proc si l'élément est dispo,
 *     - #_1990_groupe_retire_element ou _1990_groupe_ajout_element si
 *       l'élément est déjà dans un groupe.
 */
{
  gint              cx, cy;
  GtkTreePath      *path;
  GtkTreeIter       iter, iter_tmp;
  
  BUGPARAMCRIT (p, "%p", p, )
  INFO (p->niveaux_groupes,
        ,
        (gettext ("Le projet ne possède pas de niveaux de groupes.\n"));)
  BUGCRIT (UI_GRO.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Groupes");)
  
  gdk_window_get_geometry (gtk_tree_view_get_bin_window (GTK_TREE_VIEW (
                                                       UI_GRO.tree_view_etat)),
                           &cx,
                           &cy,
                           NULL,
                           NULL);
  gtk_tree_view_get_path_at_pos (GTK_TREE_VIEW (widget),
                                 x -=cx,
                                 y -=cy,
                                 &path,
                                 NULL,
                                 &cx,
                                 &cy);
  if (path != NULL)
  {
    GtkTreeModel *list_store;
    Groupe       *groupe_dest;
    
    list_store = gtk_tree_view_get_model (GTK_TREE_VIEW (
                                                       UI_GRO.tree_view_etat));
    gtk_tree_model_get_iter (list_store, &iter, path);
    groupe_dest = _1990_gtk_get_groupe (list_store, &iter);
    // Ajoute les éléments disponibles sélectionnées dans le groupe souhaité.
    if (UI_GRO.drag_from == GTK_WIDGET (UI_GRO.tree_view_dispo))
      BUG (_1990_gtk_button_ajout_dispo_proc (groupe_dest, p), )
    // Déplace l'élément sélectionné dans un autre groupe.
    else
    {
      GtkTreeModel  *model;
      Niveau_Groupe *niveau_groupe;
      
      niveau_groupe = g_list_nth_data (p->niveaux_groupes,
                                       GTK_COMMON_SPINBUTTON_AS_UINT (
                                 GTK_SPIN_BUTTON (UI_GRO.spin_button_niveau)));
      gtk_tree_selection_get_selected (UI_GRO.tree_select_etat,
                                       &model,
                                       &iter);
      if (gtk_tree_model_iter_parent (list_store, &iter_tmp, &iter))
      {
        Groupe *groupe_source = _1990_gtk_get_groupe (model, &iter);
        
        if (groupe_source != groupe_dest)
        {
          Groupe *groupe;
          
          gtk_tree_model_get (model, &iter, 0, &groupe, -1);
          BUG (_1990_groupe_retire_element (p,
                                            niveau_groupe,
                                            groupe_source,
                                            groupe),
              )
          BUG (_1990_groupe_ajout_element (p,
                                           niveau_groupe,
                                           groupe_dest,
                                           groupe),
              )
        }
      }
    }
  }
  
  return;
}


void
_1990_gtk_tree_view_etat_row_expanded (GtkTreeView *tree_view,
                                       GtkTreeIter *iter,
                                       GtkTreePath *path,
                                       Projet      *p)
/**
 * \brief Évènement lorsqu'on ouvre une ligne contenant un enfant. Est utilisé
 *        pour se souvenir des lignes qui sont ouvertes et fermées et les
 *        restituer lorsqu'on change de niveau.
 * \param tree_view : composant tree_view à l'origine de l'évènement,
 * \param iter : ligne s'"ouvrant",
 * \param path : chemin vers la ligne,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Échec :
 *     - p == NULL,
 *     - aucun niveau de groupes,
 *     - interface graphique non initialisée.
 */
{
  GtkTreeModel *model = gtk_tree_view_get_model (tree_view);
  Groupe       *groupe;
  
  BUGPARAMCRIT (p, "%p", p, )
  INFO (p->niveaux_groupes,
        ,
        (gettext ("Le projet ne possède pas de niveaux de groupes.\n"));)
  BUGCRIT (UI_GRO.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Groupes");)
  
  // On détermine le groupe ayant entraîné une ouverture. En effet, seul un
  // groupe dans le tree_view_etat peut entraîner une ouverture.
  groupe = _1990_gtk_get_groupe(model, iter);
  
  // On le marque comme ouvert.
  groupe->Iter_expand = 1;
  
  return;
}


void
_1990_gtk_tree_view_etat_row_collapsed (GtkTreeView *tree_view,
                                        GtkTreeIter *iter,
                                        GtkTreePath *path,
                                        Projet      *p)
/**
 * \brief Évènement lorsqu'on ferme une ligne contenant un enfant. Est utilisé
 *        pour se souvenir des lignes qui sont ouvertes et fermées et les
 *        restituer lorsqu'on change de niveau.
 * \param tree_view : composant tree_view à l'origine de l'évènement,
 * \param iter : ligne se "fermant",
 * \param path : chemin vers la ligne,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Échec :
 *     - p == NULL,
 *     - aucun niveau de groupes,
 *     - interface graphique non initialisée.
 */
{
  GtkTreeModel *model = gtk_tree_view_get_model (tree_view);
  Groupe       *groupe;
  
  BUGPARAMCRIT (p, "%p", p, )
  INFO (p->niveaux_groupes,
        ,
        (gettext ("Le projet ne possède pas de niveaux de groupes.\n"));)
  BUGCRIT (UI_GRO.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Groupes");)
  
  // On détermine le groupe ayant entraîné une fermeture. En effet, seul un
  // groupe dans le tree_view_etat peut entraîner une fermeture
  groupe = _1990_gtk_get_groupe (model, iter);
  
  // On le marque comme fermé.
  groupe->Iter_expand = 0;
  
  return;
}


void
_1990_gtk_button_groupe_toggled (GtkRadioToolButton *radiobutton,
                                 Projet             *p)
/**
 * \brief Évènement lorsqu'il y a un changement de type de combinaison (OR, XOR
 *        ou AND).
 * \param radiobutton : composant radiobutton à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Échec :
 *     - p == NULL,
 *     - aucun niveau de groupes,
 *     - interface graphique non initialisée,
 *     - #_1990_groupe_modifie_combinaison,
 *     - type de combinaison inconnu.
 */
{
  GtkTreeModel *model;
  GtkTreeIter   iter;
  Groupe       *groupe;
  
  BUGPARAMCRIT (p, "%p", p, )
  INFO (p->niveaux_groupes,
        ,
        (gettext ("Le projet ne possède pas de niveaux de groupes.\n"));)
  BUGCRIT (UI_GRO.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Groupes");)
  
  /* On détermine le groupe sélectionné */
  if (!gtk_tree_selection_get_selected(UI_GRO.tree_select_etat,
                                       &model,
                                       &iter))
    return;
  
  groupe = _1990_gtk_get_groupe (model, &iter);
  
  /* On attribue le nouveau type de combinaison */
  if (radiobutton == (void *) UI_GRO.item_groupe_and)
    BUG (_1990_groupe_modifie_combinaison (groupe, GROUPE_COMBINAISON_AND), )
  else if (radiobutton == (void *) UI_GRO.item_groupe_or)
    BUG (_1990_groupe_modifie_combinaison (groupe, GROUPE_COMBINAISON_OR), )
  else if (radiobutton == (void *) UI_GRO.item_groupe_xor)
    BUG (_1990_groupe_modifie_combinaison (groupe, GROUPE_COMBINAISON_XOR), )
  else
    FAILINFO (, (gettext ("Le type de combinaison est inconnu.\n"));)
  
  return;
}


void
_1990_gtk_groupes_button_generer_clicked (GtkWidget *button,
                                          Projet    *p)
/**
 * \brief Génère les pondérations.
 * \param button : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Échec :
 *     - p == NULL,
 *     - aucun niveau de groupes,
 *     - #_1990_combinaisons_genere,
 *     - #_1990_groupe_affiche_tout,
 *     - #_1990_ponderations_affiche_tout.
 */
{
  BUGPARAMCRIT (p, "%p", p, )
  INFO (p->niveaux_groupes,
        ,
        (gettext ("Le projet ne possède pas de niveaux de groupes.\n"));)
  
  BUG (_1990_combinaisons_genere (p), )
  BUG (_1990_ponderations_affiche_tout (p), )
  
  return;
}


void
_1990_gtk_radio_button_eluequ_equ_seul (GtkRadioButton *radiobutton,
                                        Projet         *p)
/**
 * \brief Modifie les options de combinaison pour que l'ELU EQU calcule à
 *        l'équilibre seulement.
 * \param radiobutton : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Échec :
 *     - p == NULL,
 *     - #_1990_combinaisons_eluequ_equ_seul.
 */
{
  BUGPARAMCRIT (p, "%p", p, )
  
  BUG (_1990_combinaisons_eluequ_equ_seul (p), )
  
  return;
}


void
_1990_gtk_radio_button_eluequ_equ_resist (GtkRadioButton *radiobutton,
                                          Projet         *p)
/**
 * \brief Modifie les options de combinaison pour que l'ELU EQU calcule à
 *        l'équilibre et à la résistance structurelle.
 * \param radiobutton : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Échec :
 *     - p == NULL,
 *     - #_1990_combinaisons_eluequ_equ_resist.
 */
{
  BUGPARAMCRIT (p, "%p", p, )
  
  BUG (_1990_combinaisons_eluequ_equ_resist (p), )
  
  return;
}


void
_1990_gtk_radio_button_elustrgeo_1 (GtkRadioButton *radiobutton,
                                    Projet         *p)
/**
 * \brief Modifie les options de combinaison pour que l'ELU STR/GEO calcule
 *        selon l'approche 1.
 * \param radiobutton : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Échec :
 *     - p == NULL,
 *     - #_1990_combinaisons_elustrgeo_1.
 */
{
  BUGPARAMCRIT (p, "%p", p, )
  
  BUG (_1990_combinaisons_elustrgeo_1 (p), )
  
  return;
}


void
_1990_gtk_radio_button_elustrgeo_2 (GtkRadioButton *radiobutton,
                                    Projet         *p)
/**
 * \brief Modifie les options de combinaison pour que l'ELU STR/GEO calcule
 *        selon l'approche 2.
 * \param radiobutton : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Échec :
 *     - p == NULL,
 *     - #_1990_combinaisons_elustrgeo_2.
 */
{
  BUGPARAMCRIT (p, "%p", p, )
  
  BUG (_1990_combinaisons_elustrgeo_2 (p), )
  
  return;
}


void
_1990_gtk_radio_button_elustrgeo_3 (GtkRadioButton *radiobutton,
                                    Projet         *p)
/**
 * \brief Modifie les options de combinaison pour que l'ELU STR/GEO calcule
 *        selon l'approche 3.
 * \param radiobutton : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Échec :
 *     - p == NULL,
 *     - #_1990_combinaisons_elustrgeo_3.
 */
{
  BUGPARAMCRIT (p, "%p", p, )
  
  BUG (_1990_combinaisons_elustrgeo_3 (p), )
  
  return;
}


void
_1990_gtk_radio_button_elustrgeo_6_10 (GtkRadioButton *radiobutton,
                                       Projet         *p)
/**
 * \brief Modifie les options de combinaison pour que l'ELU STR/GEO calcule
 *        selon la formule 6.10 de l'EN 1990.
 * \param radiobutton : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Échec :
 *     - p == NULL,
 *     - #_1990_combinaisons_elustrgeo_6_10.
 */
{
  BUGPARAMCRIT (p, "%p", p, )
  
  BUG (_1990_combinaisons_elustrgeo_6_10 (p), )
  
  return;
}


void
_1990_gtk_radio_button_elustrgeo_6_10ab (GtkRadioButton *radiobutton,
                                         Projet         *p)
/**
 * \brief Modifie les options de combinaison pour que l'ELU STR/GEO calcule
 *        selon la formule 6.10 (a) et (b) de l'EN 1990.
 * \param radiobutton : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Échec :
 *     - p == NULL,
 *     - #_1990_combinaisons_elustrgeo_6_10ab.
 */
{
  BUGPARAMCRIT (p, "%p", p, )
  
  BUG (_1990_combinaisons_elustrgeo_6_10ab (p), )
  
  return;
}


void
_1990_gtk_radio_button_eluacc_frequente (GtkRadioButton *radiobutton,
                                         Projet         *p)
/**
 * \brief Modifie les options de combinaison pour que l'ELU ACC calcule avec
 *        les valeurs fréquentes des actions variables.
 * \param radiobutton : composant l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Échec :
 *     - p == NULL,
 *     - #_1990_combinaisons_eluacc_frequente.
 */
{
  BUGPARAMCRIT (p, "%p", p, )
  
  BUG (_1990_combinaisons_eluacc_frequente (p), )
  
  return;
}


void
_1990_gtk_radio_button_eluacc_quasi_permanente (GtkRadioButton *radiobutton,
                                                Projet         *p)
/**
 * \brief Modifie les options de combinaison pour que l'ELU ACC calcule avec
 *        les valeurs quasi_permanente des actions variables.
 * \param radiobutton : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Échec :
 *     - p == NULL,
 *     - #_1990_combinaisons_eluacc_quasi_permanente.
 */
{
  BUGPARAMCRIT (p, "%p", p, )
  
  BUG (_1990_combinaisons_eluacc_quasi_permanente (p), )
  
  return;
}


void
_1990_gtk_tooltip (GtkWidget  *widget,
                   gint        x,
                   gint        y,
                   gboolean    keyboard_mode,
                   GtkTooltip *tooltip,
                   gpointer    user_data)
/**
 * \brief Cette fonction doit obligatoirement être relié à l'évènement
 *        "query-tooltip" pour qu'apparaisse la fenêtre tooltip.
 * \param widget : composant à l'origine de l'évènement,
 * \param x : inutile,
 * \param y : inutile,
 * \param keyboard_mode : inutile,
 * \param tooltip : inutile,
 * \param user_data : la variable projet.
 * \return Rien.
 */
{
  return;
}


void
_1990_gtk_tree_view_etat_cell_edited (GtkCellRendererText *cell,
                                      gchar               *path_string,
                                      gchar               *new_text,
                                      Projet              *p)
/**
 * \brief Édition du nom dans le tree-view-etat. On édite donc soit le nom d'un
 *        groupe, soit le nom d'une action.
 * \param cell : composant à l'origine de l'évènement,
 * \param path_string : ligne en cours d'édition,
 * \param new_text : nouveau nom,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Échec :
 *     - p == NULL,
 *     - interface graphique non initialisée,
 *     - #_1990_groupe_modifie_nom ou #_1990_action_nom_change.
 */
{
  GtkTreePath  *path;
  GtkTreeIter   iter;
  GtkTreeModel *model;
  unsigned int  niveau;
  GtkTreeIter   iter_parent;
  
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_GRO.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Groupes");)
  
  path = gtk_tree_path_new_from_string (path_string);
  model = gtk_tree_view_get_model (GTK_TREE_VIEW (UI_GRO.tree_view_etat));
  gtk_tree_model_get_iter (model, &iter, path);
  
  gtk_tree_path_free (path);
  
  niveau = GTK_COMMON_SPINBUTTON_AS_UINT (GTK_SPIN_BUTTON (
                                            UI_GRO.spin_button_niveau));
  
  // On prend le niveau correspondant à niveau
  if (!gtk_tree_model_iter_parent (model, &iter_parent, &iter))
  {
    Groupe *groupe;
    
    gtk_tree_model_get (model, &iter, 0, &groupe, -1);
    BUG (_1990_groupe_modifie_nom (g_list_nth_data (p->niveaux_groupes,
                                                    niveau),
                                   groupe,
                                   new_text,
                                   p),
        )
  }
  else // On prend niveau-1
  {
    // Le nom est celui d'une action
    if (niveau == 0)
    {
      Action *action;
      
      gtk_tree_model_get (model, &iter, 0, &action, -1);
      BUG (_1990_action_nom_change (p, action, new_text), )
    }
    else // Le nom est celui d'un groupe du niveau n-1
    {
      Groupe *groupe;
      
      gtk_tree_model_get (model, &iter, 0, &groupe, -1);
      BUG (_1990_groupe_modifie_nom (g_list_nth_data (p->niveaux_groupes,
                                                      niveau - 1),
                                     groupe,
                                     new_text,
                                     p),
          )
    }
  }
}


void
_1990_gtk_tree_view_dispo_cell_edited (GtkCellRendererText *cell,
                                       gchar               *path_string,
                                       gchar               *new_text,
                                       Projet              *p)
/**
 * \brief Édition du nom dans le tree-view-dispo. On édite donc soit le nom
 *        d'un groupe, soit le nom d'une action.
 * \param cell : composant à l'origine de l'évènement,
 * \param path_string : ligne en cours d'édition,
 * \param new_text : nouveau nom,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Échec :
 *     - p == NULL,
 *     - interface graphique non initialisée,
 *     - #_1990_action_nom_change,
 *     - #_1990_groupe_modifie_nom.
 */
{
  GtkTreePath  *path;
  GtkTreeIter   iter;
  GtkTreeModel *model;
  unsigned int  niveau;
  
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_GRO.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Groupes");)
  
  path = gtk_tree_path_new_from_string (path_string);
  model = gtk_tree_view_get_model (GTK_TREE_VIEW (UI_GRO.tree_view_dispo));
  gtk_tree_model_get_iter (model, &iter, path);
  
  gtk_tree_path_free (path);
  
  niveau = GTK_COMMON_SPINBUTTON_AS_UINT (GTK_SPIN_BUTTON (
                                                   UI_GRO.spin_button_niveau));
  
  // Le nom est celui d'une action
  if (niveau == 0)
  {
    Action *action;
    
    gtk_tree_model_get (model, &iter, 0, &action, -1);
    BUG (_1990_action_nom_change (p, action, new_text), )
  }
  else // Le nom est celui d'un groupe du niveau n-1
  {
    Groupe *groupe;
    
    gtk_tree_model_get (model, &iter, 0, &groupe, -1);
    BUG (_1990_groupe_modifie_nom (g_list_nth_data (p->niveaux_groupes,
                                                    niveau - 1),
                                   groupe,
                                   new_text,
                                   p),
        )
  }
}


gboolean
_1990_gtk_tree_view_etat_key_press_event (GtkWidget *widget,
                                          GdkEvent  *event,
                                          Projet    *p)
/**
 * \brief Gère les touches appuyées du treeview etat.
 * \param widget : composant à l'origine de l'évènement,
 * \param event : caractéristiques de l'évènement,
 * \param p : la variable projet.
 * \return FALSE.\n
 *   Échec : FALSE :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
{
  BUGPARAMCRIT (p, "%p", p, FALSE)
  BUGCRIT (UI_GRO.builder,
           FALSE,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Groupes");)
  
  if (event->key.keyval == GDK_KEY_Delete)
    _1990_gtk_button_groupe_suppr_clicked (widget, p);
  
  // Pour permettre aux autres touches d'être fonctionnelles.
  return FALSE;
}


void
_1990_gtk_tree_select_changed (GtkTreeSelection *treeselection,
                               Projet           *p)
/**
 * \brief Permet de activer/désactiver les boutons ajout en fonction de la
 *        selection.
 * \param treeselection : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Échec :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
{
  GtkTreeIter       iter;
  
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_GRO.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Groupes");)
  
  // Si le tree_view est vide ou qu'aucun groupe n'est sélectionné
  if ((!gtk_tree_model_get_iter_first (GTK_TREE_MODEL (
                                                      UI_GRO.tree_store_dispo),
                                       &iter)) ||
      (!gtk_tree_selection_get_selected (UI_GRO.tree_select_etat,
                                         NULL,
                                         NULL)))
    gtk_widget_set_sensitive (GTK_WIDGET (UI_GRO.item_ajout_tout_dispo),
                              FALSE);
  else
    gtk_widget_set_sensitive (GTK_WIDGET (UI_GRO.item_ajout_tout_dispo),
                              TRUE);
  
  // Si aucun groupe n'est sélectionné
  if (!gtk_tree_selection_get_selected (UI_GRO.tree_select_etat,
                                        NULL,
                                        NULL))
  {
    gtk_widget_set_sensitive (GTK_WIDGET (UI_GRO.item_ajout_dispo),
                              FALSE);
    gtk_widget_set_sensitive (GTK_WIDGET (UI_GRO.item_groupe_suppr),
                              FALSE);
    gtk_widget_set_sensitive (GTK_WIDGET (UI_GRO.item_groupe_and),
                              FALSE);
    gtk_widget_set_sensitive (GTK_WIDGET (UI_GRO.item_groupe_or),
                              FALSE);
    gtk_widget_set_sensitive (GTK_WIDGET (UI_GRO.item_groupe_xor),
                              FALSE);
  }
  else
  {
    GList *list_orig;
    GList *list;
    
    gtk_widget_set_sensitive (GTK_WIDGET (UI_GRO.item_groupe_suppr),
                              TRUE);
    gtk_widget_set_sensitive (GTK_WIDGET (UI_GRO.item_groupe_and),
                              TRUE);
    gtk_widget_set_sensitive (GTK_WIDGET (UI_GRO.item_groupe_or),
                              TRUE);
    gtk_widget_set_sensitive (GTK_WIDGET (UI_GRO.item_groupe_xor),
                              TRUE);
    
    // Si aucune sélection dans la liste des éléments disponibles
    list_orig = gtk_tree_selection_get_selected_rows (UI_GRO.tree_select_dispo,
                                                      NULL);
    list = g_list_last (list_orig);
    
    if (list == NULL)
      gtk_widget_set_sensitive (GTK_WIDGET (UI_GRO.item_ajout_dispo),
                                FALSE);
    else
      gtk_widget_set_sensitive (GTK_WIDGET (UI_GRO.item_ajout_dispo),
                                TRUE);
    
    g_list_foreach (list_orig, (GFunc) gtk_tree_path_free, NULL);
    g_list_free (list_orig);
  }
}


void
EF_gtk_noeuds_render_etat (GtkTreeViewColumn *tree_column,
                           GtkCellRenderer   *cell,
                           GtkTreeModel      *tree_model,
                           GtkTreeIter       *iter,
                           gpointer           data)
/**
 * \return Affiche le nom d'un groupe ou d'une action dans le treeview_etat.
 * \param tree_column : composant à l'origine de l'évènement,
 * \param cell : la cellule en cours d'édition,
 * \param tree_model : le mode en cours d'édition,
 * \param iter : la ligne en cours d'édition,
 * \param data : la variable projet.
 * \return Rien.
 */
{
  Groupe  *groupe;
  
  gtk_tree_model_get (tree_model, iter, 0, &groupe, -1);
  g_object_set (cell, "text", groupe->nom, NULL);
  
  return;
}


void
EF_gtk_noeuds_render_dispo (GtkTreeViewColumn *tree_column,
                            GtkCellRenderer   *cell,
                            GtkTreeModel      *tree_model,
                            GtkTreeIter       *iter,
                            gpointer data)
/**
 * \brief Affiche le nom d'un groupe ou d'une action dans le treeview_dispo.
 * \param tree_column : composant à l'origine de l'évènement,
 * \param cell : la cellule en cours d'édition,
 * \param tree_model : le mode en cours d'édition,
 * \param iter : la ligne en cours d'édition,
 * \param data : la variable projet.
 * \return Rien.
 */
{
  Groupe  *groupe;
  
  gtk_tree_model_get (tree_model, iter, 0, &groupe, -1);
  g_object_set (cell, "text", groupe->nom, NULL);
  
  return;
}


void
_1990_gtk_groupes (Projet *p)
/**
 * \brief Création de la fenêtre Groupes.
 * \param p : la variable projet.
 * \return Rien.
 *   Échec :
 *     - p == NULL,
 *     - interface graphique impossible à générer,
 *     - _1990_groupe_ajout_niveau si aucun niveau n'est existant.
 */
{
  BUGPARAMCRIT (p, "%p", p, )
  
  if (UI_GRO.builder != NULL)
  {
    gtk_window_present (GTK_WINDOW (UI_GRO.window));
    return;
  }
  
  if (p->niveaux_groupes == NULL)
    BUG (_1990_groupe_ajout_niveau (p), )
  
  UI_GRO.builder = gtk_builder_new ();
  BUGCRIT (gtk_builder_add_from_resource (UI_GRO.builder,
                                         "/org/2lgc/codegui/ui/1990_groupes.ui",
                                         NULL) != 0,
          ,
          (gettext ("La génération de la fenêtre %s a échouée.\n"),
                    "Groupes");)
  gtk_builder_connect_signals (UI_GRO.builder, p);
  
  UI_GRO.window = GTK_WIDGET (gtk_builder_get_object (
                                       UI_GRO.builder, "1990_groupes_window"));
  UI_GRO.tree_store_etat = GTK_TREE_STORE (gtk_builder_get_object (
                              UI_GRO.builder, "1990_actions_tree_store_etat"));
  UI_GRO.tree_store_dispo = GTK_TREE_STORE (gtk_builder_get_object (
                             UI_GRO.builder, "1990_actions_tree_store_dispo"));
  UI_GRO.tree_select_etat = GTK_TREE_SELECTION (gtk_builder_get_object (
                         UI_GRO.builder, "1990_groupes_treeview_select_etat"));
  UI_GRO.tree_view_etat = GTK_TREE_VIEW (gtk_builder_get_object (
                                UI_GRO.builder, "1990_groupes_treeview_etat"));
  UI_GRO.tree_select_dispo = GTK_TREE_SELECTION (gtk_builder_get_object 
                       (UI_GRO.builder, "1990_groupes_treeview_select_dispo"));
  UI_GRO.spin_button_niveau = GTK_WIDGET (gtk_builder_get_object (
                          UI_GRO.builder, "1990_groupes_spin_button_niveaux"));
  UI_GRO.tree_view_dispo = GTK_TREE_VIEW (gtk_builder_get_object (
                               UI_GRO.builder, "1990_groupes_treeview_dispo"));
  
  UI_GRO.item_groupe_and = GTK_TOOL_ITEM (gtk_builder_get_object (
                             UI_GRO.builder, "1990_charges_toolbar_etat_and"));
  UI_GRO.item_groupe_or = GTK_TOOL_ITEM (gtk_builder_get_object (
                              UI_GRO.builder, "1990_charges_toolbar_etat_or"));
  UI_GRO.item_groupe_xor = GTK_TOOL_ITEM (gtk_builder_get_object (
                             UI_GRO.builder, "1990_charges_toolbar_etat_xor"));
  UI_GRO.item_groupe_suppr = GTK_TOOL_ITEM (gtk_builder_get_object (
                           UI_GRO.builder, "1990_charges_toolbar_etat_suppr"));
  UI_GRO.item_ajout_dispo = GTK_TOOL_ITEM (gtk_builder_get_object (
                          UI_GRO.builder, "1990_charges_toolbar_dispo_ajout"));
  UI_GRO.item_ajout_tout_dispo = GTK_TOOL_ITEM (gtk_builder_get_object (
                     UI_GRO.builder, "1990_charges_toolbar_dispo_ajout_tout"));
  
  gtk_drag_source_set (GTK_WIDGET (UI_GRO.tree_view_etat),
                       GDK_BUTTON1_MASK,
                       drag_targets_groupes_1,
                       1,
                       GDK_ACTION_MOVE); 
  gtk_drag_dest_set (GTK_WIDGET (UI_GRO.tree_view_dispo),
                     GTK_DEST_DEFAULT_ALL,
                     drag_targets_groupes_1,
                     1,
                     GDK_ACTION_MOVE);
  gtk_drag_source_set (GTK_WIDGET (UI_GRO.tree_view_dispo),
                       GDK_BUTTON1_MASK,
                       drag_targets_groupes_2,
                       1,
                       GDK_ACTION_MOVE); 
  gtk_drag_dest_set (GTK_WIDGET (UI_GRO.tree_view_etat),
                     GTK_DEST_DEFAULT_ALL,
                     drag_targets_groupes_3,
                     2,
                     GDK_ACTION_MOVE);
  
  gtk_adjustment_set_upper (gtk_spin_button_get_adjustment (GTK_SPIN_BUTTON (
                                        gtk_builder_get_object (UI_GRO.builder,
                                         "1990_groupes_spin_button_niveaux"))),
                            g_list_length (p->niveaux_groupes)-1);
  
  gtk_tree_view_column_set_cell_data_func (
    GTK_TREE_VIEW_COLUMN (gtk_builder_get_object (UI_GRO.builder,
                                        "1990_groupes_treeview_etat_column0")),
    GTK_CELL_RENDERER (gtk_builder_get_object (UI_GRO.builder,
                                          "1990_groupes_treeview_etat_cell0")),
    EF_gtk_noeuds_render_etat,
    p,
    NULL);
  gtk_tree_view_column_set_cell_data_func (
    GTK_TREE_VIEW_COLUMN (gtk_builder_get_object (UI_GRO.builder,
                                       "1990_groupes_treeview_dispo_column0")),
    GTK_CELL_RENDERER (gtk_builder_get_object (UI_GRO.builder,
                                         "1990_groupes_treeview_dispo_cell0")),
    EF_gtk_noeuds_render_dispo,
    p,
    NULL);
  
  BUG (_1990_gtk_groupes_affiche_niveau (
         p,
         GTK_COMMON_SPINBUTTON_AS_UINT (GTK_SPIN_BUTTON (gtk_builder_get_object
                       (UI_GRO.builder, "1990_groupes_spin_button_niveaux")))),
      )
  gtk_window_set_transient_for (GTK_WINDOW (UI_GRO.window),
                                GTK_WINDOW (UI_GTK.window));
  
  return;
}

#endif

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
