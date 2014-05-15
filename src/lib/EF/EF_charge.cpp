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

#include <algorithm>

#include "common_projet.hpp"
#include "common_erreurs.hpp"
#ifdef ENABLE_GTK
#include "common_gtk.hpp"
#endif
#include "1990_action.hpp"
#include "EF_charge_noeud.hpp"
#include "EF_charge_barre_ponctuelle.hpp"
#include "EF_charge_barre_repartie_uniforme.hpp"
#include "EF_calculs.hpp"
#include "EF_charge.hpp"


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
Charge *
EF_charge_ajout (Projet     *p,
                 Action     *action,
                 Charge     *charge,
                 const char *nom)
{
#ifdef ENABLE_GTK
  GtkTreeIter   iter_action;
  GtkTreeModel *model_action;
#endif
  
  BUGPARAM (p, "%p", p, NULL)
  BUGPARAM (action, "%p", action, NULL)
  BUGPARAM (charge, "%p", charge, NULL)
  
  BUGCRIT (charge->nom = g_strdup_printf ("%s", nom),
           NULL,
           (gettext ("Erreur d'allocation mémoire.\n")); )
  
  _1990_action_charges_renvoie (action)->push_back (charge);
  
  BUG (EF_calculs_free (p), NULL)
  
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
Action *
EF_charge_action (Projet *p,
                  Charge *charge)
{
  std::list <Action *>::iterator it;
  
  BUGPARAM (p, "%p", p, NULL)
  BUGPARAM (charge, "%p", charge, NULL)
  
  it = p->actions.begin ();
  while (it != p->actions.end ())
  {
    Action *action = *it;
    
    if (std::find (_1990_action_charges_renvoie (action)->begin (),
                   _1990_action_charges_renvoie (action)->end (),
                   charge) != _1990_action_charges_renvoie (action)->end ())
    {
      return action;
    }
    
    ++it;
  }
  
  FAILINFO (NULL, (gettext ("La charge n'est dans aucune action.\n")); )
}


/**
 * \brief Renomme une charge.
 * \param p : la variable projet,
 * \param charge : la charge,
 * \param nom : le nouveau nom.
 * \return
 *   Succès : true.
 *   Échec : false :
 *     - p == NULL,
 *     - charge == NULL,
 *     - erreur d'allocation mémoire.
 */
bool
EF_charge_renomme (Projet     *p,
                   Charge     *charge,
                   const char *nom)
{
  BUGPARAM (p, "%p", p, false)
  BUGPARAM (charge, "%p", charge, false)
  
  free (charge->nom);
  BUGCRIT (charge->nom = g_strdup_printf ("%s", nom),
           false,
           (gettext ("Erreur d'allocation mémoire.\n")); )
  
#ifdef ENABLE_GTK
  if (UI_ACT.builder != NULL)
  {
    gtk_widget_queue_resize (GTK_WIDGET (UI_ACT.tree_view_charges));
  }
#endif
  
  return true;
}


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
 *   Succès : true.\n
 *   Échec : false :
 *     - p == NULL,
 *     - action_src == NULL,
 *     - action_dest == NULL,
 *     - charge_s == NULL,
 */
bool
EF_charge_deplace (Projet *p,
                   Action *action_src,
                   Charge *charge_s,
                   Action *action_dest)
{
  Charge *charge_data = NULL;
  std::list <Charge *>::iterator it;
  
  BUGPARAM (p, "%p", p, false)
  BUGPARAM (action_src, "%p", action_src, false)
  BUGPARAM (charge_s, "%p", charge_s, false)
  BUGPARAM (action_dest, "%p", action_dest, false)
  
  if (action_src == action_dest)
  {
    return true;
  }
  
  // Lorsqu'elle est trouvée,
  it = _1990_action_charges_renvoie (action_src)->begin ();
  //   Pour chaque charge de l'action en cours Faire
  while (it != _1990_action_charges_renvoie (action_src)->end ())
  {
    Charge *charge = *it;
    
  //     Si la charge est celle à supprimer Alors
    if (charge == charge_s)
    {
#ifdef ENABLE_GTK
  //       On la supprime du tree-view-charge
      if (UI_ACT.builder != NULL)
      {
        gtk_tree_store_remove (GTK_TREE_STORE (gtk_builder_get_object (
                            UI_ACT.builder, "1990_actions_tree_store_charge")),
                               &charge->Iter);
      }
#endif
  //       et de la liste des charges tout en conservant les données
  //         de la charge dans charge_data.
      charge_data = charge;
      _1990_action_charges_renvoie (action_src)->remove (charge);
      
      break;
    }
    
    ++it;
  //   FinPour
  }
  
  INFO (charge_data,
        false,
        (gettext ("Charge '%s' de l'action %s introuvable.\n"),
                  charge_s->nom,
                  _1990_action_nom_renvoie (action_src)); )
  
  // On insère la charge à la fin de la liste des charges dans l'action de
  // destination en modifiant son numéro.
  _1990_action_charges_renvoie (action_dest)->push_back (charge_data);
  
  BUG (EF_calculs_free (p), false)
  
  return true;
}


/**
 * \brief Supprime une charge. Décrémente également le numéro des charges
 *        possédant un numéro supérieur à la charge supprimée afin que la liste
 *        des numéros soit toujours continue.
 * \param p : la variable projet,
 * \param action : l'action où se situe la charge à supprimer,
 * \param charge_s : la charge à supprimer.
 * \return
 *   Succès : true.\n
 *   Échec : false :
 *     - p == NULL,
 *     - action == NULL,
 *     - charge_s == NULL.
 */
bool
EF_charge_supprime (Projet *p,
                    Action *action,
                    Charge *charge_s)
{
  Charge *charge_data = NULL;
  std::list <Charge *>::iterator it;
  
  BUGPARAM (p, "%p", p, false)
  
  it = _1990_action_charges_renvoie (action)->begin ();
  // Pour chaque charge de l'action en cours Faire
  while (it != _1990_action_charges_renvoie (action)->end ())
  {
    Charge *charge = *it;
  //   Si la charge est celle à supprimer Alors
    if (charge == charge_s)
    {
#ifdef ENABLE_GTK
  //     On la supprime du tree-view-charge
      if (UI_ACT.builder != NULL)
      {
        gtk_tree_store_remove (GTK_TREE_STORE (gtk_builder_get_object (
                                 UI_ACT.builder,
                                 "1990_actions_tree_store_charge")),
                               &charge->Iter);
      }
#endif
  //     et de la liste des charges tout en conservant les données
  //       de la charge dans charge_data
      charge_data = *it;
      _1990_action_charges_renvoie (action)->remove (charge_data);
  
  //     On libère la charge charge_data
      switch (charge_data->type)
      {
        case CHARGE_NOEUD :
        {
          BUG (EF_charge_noeud_free (charge_data), false)
          break;
        }
        case CHARGE_BARRE_PONCTUELLE :
        {
          BUG (EF_charge_barre_ponctuelle_free (charge_data), false)
          break;
        }
        case CHARGE_BARRE_REPARTIE_UNIFORME :
        {
          BUG (EF_charge_barre_repartie_uniforme_free (charge_data), false)
          break;
        }
        default :
        {
          FAILCRIT (false,
                   (gettext ("Type de charge %d inconnu.\n"),
                             charge_data->type); )
          break;
        }
      }
      break;
    }
  // FinPour
    
    ++it;
  }
  
  INFO (charge_data,
        false,
        (gettext ("Charge '%s' de l'action %s introuvable.\n"),
                  charge_s->nom,
                  _1990_action_nom_renvoie (action)); )
  
  BUG (EF_calculs_free (p), false)
  
  return true;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
