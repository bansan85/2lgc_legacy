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
#include <libintl.h>
#include <locale.h>
#include <gmodule.h>
#include <string.h>

#include "common_projet.h"
#include "common_erreurs.h"
#ifdef ENABLE_GTK
#include "common_gtk.h"
#endif
#include "1990_action.h"
#include "EF_charge_noeud.h"
#include "EF_charge_barre_ponctuelle.h"
#include "EF_charge_barre_repartie_uniforme.h"
#include "EF_calculs.h"


Charge *
EF_charge_ajout (Projet     *p,
                 Action     *action,
                 Charge     *charge,
                 const char *nom)
/**
 * \brief Ajoute une charge ponctuelle à une action à l'intérieur d'une barre.
 * \param p : la variable projet,
 * \param action : l'action qui contiendra la charge,
 * \param charge : la charge à ajouter,
 * \param nom : nom de la charge.
 * \return
 *   Succès : pointeur vers la nouvelle charge.\n
 *   Échec : NULL :
 *     - p == NULL,
 *     - action == NULL,
 *     - charge == NULL.
 */
{
#ifdef ENABLE_GTK
  GtkTreeIter   iter_action;
  GtkTreeModel *model_action;
#endif
  
  BUGMSG (p, NULL, gettext ("Paramètre %s incorrect.\n"), "projet")
  BUGMSG (action, NULL, gettext ("Paramètre %s incorrect.\n"), "action")
  BUGMSG (charge, NULL, gettext ("Paramètre %s incorrect.\n"), "charge")
  
  BUGMSG (charge->nom = g_strdup_printf ("%s", nom),
          NULL,
          gettext ("Erreur d'allocation mémoire.\n"))
  
  BUG (_1990_action_charges_change (action,
                          g_list_append (_1990_action_charges_renvoie (action),
                                    charge)),
       NULL)
  
  BUG (EF_calculs_free (p), FALSE)
  
#ifdef ENABLE_GTK
  if ((UI_ACT.builder != NULL) &&
      (gtk_tree_selection_get_selected (UI_ACT.tree_select_actions,
                                        &model_action,
                                        &iter_action)))
  {
    Action *action2;
    
    gtk_tree_model_get (model_action, &iter_action, 0, &action2, -1);
    if (action2 == action)
    {
      gtk_tree_store_append (UI_ACT.tree_store_charges, &charge->Iter, NULL);
      gtk_tree_store_set (UI_ACT.tree_store_charges,
                          &charge->Iter,
                          0, charge,
                          -1);
    }
  }
#endif
  
  return charge;
}


Charge *EF_charge_cherche (Projet     *p,
                           Action     *action,
                           const char *nom)
/**
 * \brief Cherche et renvoie la charge demandée.
 * \param p : la variable projet,
 * \param action : l'action,
 * \param nom : le nom de la charge.
 * \return
 *   Succès : Pointeur vers la charge recherchée.\n
 *   Échec : NULL :
 *     - p == NULL,
 *     - action == NULL,
 *     - charge introuvable.
 */
{
  GList *list_parcours;
  
  BUGMSG (p, NULL, gettext ("Paramètre %s incorrect.\n"), "projet")
  
  list_parcours = _1990_action_charges_renvoie (action);
  while (list_parcours != NULL)
  {
    Charge *charge = list_parcours->data;
    
    if (strcmp (charge->nom, nom) == 0)
      return charge;
    
    list_parcours = g_list_next (list_parcours);
  }
  
  BUGMSG (0,
          NULL,
          gettext ("Charge '%s' de l'action '%s' introuvable.\n"), nom, _1990_action_nom_renvoie (action))
}


Action *
EF_charge_action (Projet *p,
                  Charge *charge)
/**
 * \brief Envoie l'action possédant la charge.
 * \param p : la variable projet,
 * \param charge : la charge dont on souhaite connaitre l'action.
 * \return
 *   Succès : pointeur vers l'action.\n
 *   Échec : NULL :
 *     - p == NULL,
 *     - charge == NULL,
 *     - la charge n'est pas dans une action.
 */
{
  GList *list_parcours;
  
  BUGMSG (p, FALSE, gettext ("Paramètre %s incorrect.\n"), "projet")
  BUGMSG (charge, FALSE, gettext ("Paramètre %s incorrect.\n"), "charge")
  
  list_parcours = p->actions;
  while (list_parcours != NULL)
  {
    Action *action = list_parcours->data;
    
    if (g_list_find (_1990_action_charges_renvoie (action), charge) != NULL)
      return action;
    
    list_parcours = g_list_next (list_parcours);
  }
  
  BUGMSG (0, NULL, gettext ("La charge n'est dans aucune action.\n"))
}


gboolean
EF_charge_renomme (Projet     *p,
                   Charge     *charge,
                   const char *nom)
/**
 * \brief Renomme une charge.
 * \param p : la variable projet,
 * \param charge : la charge,
 * \param nom : le nouveau nom.
 * \return
 *   Succès : TRUE.
 *   Échec : FALSE :
 *     - p == NULL,
 *     - charge == NULL,
 *     - erreur d'allocation mémoire.
 */
{
  BUGMSG (p, FALSE, gettext ("Paramètre %s incorrect.\n"), "projet")
  BUGMSG (charge, FALSE, gettext ("Paramètre %s incorrect.\n"), "charge")
  
  free (charge->nom);
  BUGMSG (charge->nom = g_strdup_printf ("%s", nom),
          FALSE,
          gettext ("Erreur d'allocation mémoire.\n"))
  
#ifdef ENABLE_GTK
  if (UI_ACT.builder != NULL)
    gtk_widget_queue_resize (GTK_WIDGET (UI_ACT.tree_view_charges));
#endif
  
  return TRUE;
}


gboolean
EF_charge_deplace (Projet *p,
                   Action *action_src,
                   Charge *charge_s,
                   Action *action_dest)
/**
 * \brief Déplace une charge d'une action à l'autre. La charge une fois
 *        déplacée sera en fin de la liste et les numéros des charges dans
 *        l'action d'origine seront décrémentés afin que les numéros soit
 *        toujours continus.
 * \param p : la variable projet,
 * \param action_src : l'action où se situe la charge à déplacer,
 * \param charge_s : la charge à déplacer,
 * \param action_dest : l'action où sera déplacer la charge.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL,
 *     - action_src == NULL,
 *     - action_dest == NULL,
 *     - charge charge_src dans l'action action_src introuvable.
 */
{
  Charge *charge_data = NULL;
  GList  *list_parcours;
  
  BUGMSG (p, FALSE, gettext ("Paramètre %s incorrect.\n"), "projet")
  BUGMSG (action_src,
          FALSE,
          gettext ("Paramètre %s incorrect.\n"), "action_src")
  BUGMSG (charge_s,
          FALSE,
          gettext ("Paramètre %s incorrect.\n"), "charge_s")
  BUGMSG (action_dest,
          FALSE,
          gettext ("Paramètre %s incorrect.\n"), "action_dest")
  
  if (action_src == action_dest)
    return TRUE;
  
  // Lorsqu'elle est trouvée,
  list_parcours = _1990_action_charges_renvoie (action_src);
  //   Pour chaque charge de l'action en cours Faire
  while (list_parcours != NULL)
  {
    Charge *charge = list_parcours->data;
    
  //     Si la charge est celle à supprimer Alors
    if (charge == charge_s)
    {
      GList *list_next = g_list_next (list_parcours);
      
#ifdef ENABLE_GTK
  //       On la supprime du tree-view-charge
      if (UI_ACT.builder != NULL)
        gtk_tree_store_remove (GTK_TREE_STORE (gtk_builder_get_object (
                            UI_ACT.builder, "1990_actions_tree_store_charge")),
                               &charge->Iter);
#endif
  //       et de la liste des charges tout en conservant les données
  //         de la charge dans charge_data.
      charge_data = charge;
      BUG (_1990_action_charges_change (action_src,
                                        g_list_delete_link
                                                (_1990_action_charges_renvoie (
                                                                   action_src),
                                                 list_parcours)),
           FALSE)
      list_parcours = list_next;
      if (list_parcours != NULL)
        charge = list_parcours->data;
      else
        charge = NULL;
    }
    
    list_parcours = g_list_next (list_parcours);
  //   FinPour
  }
  
  BUGMSG (charge_data,
          FALSE,
          gettext ("Charge '%s' de l'action %s introuvable.\n"), charge_s->nom, _1990_action_nom_renvoie (action_src))
  
  // On insère la charge à la fin de la liste des charges dans l'action de
  // destination en modifiant son numéro.
  BUG (_1990_action_charges_change (
         action_dest,
         g_list_append (_1990_action_charges_renvoie (action_dest),
                        charge_data)),
       FALSE)
  
  BUG (EF_calculs_free (p), FALSE)
  
  return TRUE;
}


gboolean
EF_charge_supprime (Projet *p,
                    Action *action,
                    Charge *charge_s)
/**
 * \brief Supprime une charge. Décrémente également le numéro des charges
 *        possédant un numéro supérieur à la charge supprimée afin que la liste
 *        des numéros soit toujours continue.
 * \param p : la variable projet,
 * \param action : l'action où se situe la charge à supprimer,
 * \param charge_s : la charge à supprimer.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL,
 *     - action == NULL,
 *     - charge_s == NULL.
 */
{
  Charge *charge_data = NULL;
  GList  *list_parcours;
  
  BUGMSG (p, FALSE, gettext ("Paramètre %s incorrect.\n"), "projet")
  
  list_parcours = _1990_action_charges_renvoie (action);
  // Pour chaque charge de l'action en cours Faire
  while (list_parcours != NULL)
  {
    Charge *charge = list_parcours->data;
  //   Si la charge est celle à supprimer Alors
    if (charge == charge_s)
    {
      GList *list_next = g_list_next (list_parcours);
#ifdef ENABLE_GTK
  //     On la supprime du tree-view-charge
       if (UI_ACT.builder != NULL)
         gtk_tree_store_remove (GTK_TREE_STORE (gtk_builder_get_object (
                            UI_ACT.builder, "1990_actions_tree_store_charge")),
                                &charge->Iter);
#endif
  //     et de la liste des charges tout en conservant les données
  //       de la charge dans charge_data
      charge_data = list_parcours->data;
      BUG (_1990_action_charges_change (action,
                      g_list_delete_link (_1990_action_charges_renvoie(action),
                                        list_parcours)),
           FALSE)
      list_parcours = list_next;
      if (list_parcours != NULL)
        charge = list_parcours->data;
      else
        charge = NULL;
  
  //     On libère la charge charge_data
      switch (charge_data->type)
      {
        case CHARGE_NOEUD :
        {
          BUG (EF_charge_noeud_free (charge_data), FALSE)
          break;
        }
        case CHARGE_BARRE_PONCTUELLE :
        {
          BUG (EF_charge_barre_ponctuelle_free (charge_data), FALSE)
          break;
        }
        case CHARGE_BARRE_REPARTIE_UNIFORME :
        {
          BUG (EF_charge_barre_repartie_uniforme_free (charge_data), FALSE)
          break;
        }
        default :
        {
          BUGMSG (0,
                  FALSE,
                  gettext ("Type de charge %d inconnu.\n"), charge_data->type)
          break;
        }
      }
      break;
    }
  // FinPour
    list_parcours = g_list_next (list_parcours);
  }
  
  BUGMSG (charge_data,
          FALSE,
          gettext("Charge '%s' de l'action %s introuvable.\n"), charge_s->nom, _1990_action_nom_renvoie (action))
  
  BUG (EF_calculs_free (p), FALSE)
  
  return TRUE;
}
