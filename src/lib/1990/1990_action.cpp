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

#include <memory>
#include <algorithm>
#include <locale>
#include <string.h>
#include <math.h>

#include "1990_action_private.hpp"
#include "common_projet.hpp"
#include "1990_coef_psi.hpp"
#include "1990_groupe.hpp"
#include "1990_action.hpp"
#include "common_math.hpp"
#include "common_erreurs.hpp"
#include "common_fonction.hpp"
#include "EF_charge_noeud.hpp"
#include "EF_charge_barre_ponctuelle.hpp"
#include "EF_charge_barre_repartie_uniforme.hpp"
#include "EF_calculs.hpp"

#ifdef ENABLE_GTK
#include "common_gtk.hpp"
#include "1990_gtk_actions.hpp"
#endif

}

/**
 * \brief Initialise la liste des actions.
 * \param p : la variable projet.
 * \return
 *   Succès : true\n
 *   Échec : false :
 *     - p == NULL.
 * \warning Fonction interne. Il convient d'utiliser la fonction #projet_init.
 */
bool
_1990_action_init (Projet *p)
{
#ifdef ENABLE_GTK
  uint8_t    i;
  GtkWidget *w_temp;
#endif
  
  BUGPARAMCRIT (p, "%p", p, false)
  
  p->actions.clear ();
  
#ifdef ENABLE_GTK
  UI_ACT.liste = gtk_list_store_new (1, G_TYPE_STRING);
  UI_ACT.builder = NULL;
  
  UI_ACT.type_action = GTK_MENU (gtk_menu_new ());
  UI_ACT.choix_type_action = gtk_list_store_new (1, G_TYPE_STRING);
  for (i = 0; i < _1990_action_num_bat_txt (p->parametres.norme); i++)
  {
    GtkTreeIter iter;
    
    // Génération du menu contenant la liste des types d'action pour la
    // création d'une nouvelle action.
    w_temp = gtk_menu_item_new_with_label (
                  _1990_action_bat_txt_type (i, p->parametres.norme).c_str ());
    gtk_menu_shell_append (GTK_MENU_SHELL (UI_ACT.type_action), w_temp);
    UI_ACT.items_type_action.push_back (w_temp);
    gtk_widget_show (w_temp);
    g_signal_connect (w_temp,
                      "activate",
                      G_CALLBACK (_1990_gtk_nouvelle_action),
                      p);
    
    // Génération de la liste des types d'action pour la modification via le
    // treeview Action.
    gtk_list_store_append (UI_ACT.choix_type_action, &iter);
    gtk_list_store_set (
      UI_ACT.choix_type_action,
      &iter,
      0, _1990_action_bat_txt_type (i, p->parametres.norme).c_str (),
      -1);
  }
  
  UI_ACT.type_charges = GTK_MENU (gtk_menu_new ());
  w_temp = gtk_menu_item_new_with_label (gettext ("Charge nodale"));
  gtk_menu_shell_append (GTK_MENU_SHELL (UI_ACT.type_charges), w_temp);
  g_signal_connect (w_temp,
                    "activate",
                    G_CALLBACK (_1990_gtk_nouvelle_charge_nodale),
                    p);
  w_temp = gtk_menu_item_new_with_label (gettext ("Charge ponctuelle sur barre"));
  gtk_menu_shell_append (GTK_MENU_SHELL (UI_ACT.type_charges), w_temp);
  g_signal_connect (w_temp,
                    "activate",
                    G_CALLBACK (_1990_gtk_nouvelle_charge_barre_ponctuelle),
                    p);
  w_temp = gtk_menu_item_new_with_label (gettext ("Charge répartie uniforme sur barre"));
  gtk_menu_shell_append (GTK_MENU_SHELL (UI_ACT.type_charges), w_temp);
  g_signal_connect (
    w_temp,
    "activate",
    G_CALLBACK (_1990_gtk_nouvelle_charge_barre_repartie_uniforme),
    p);
  gtk_widget_show_all (GTK_WIDGET (UI_ACT.type_charges));
  
  // Sinon sous Windows, ils sont libérés à la première fermeture de la fenêtre
  // Actions et/ ils ne réapparaissent plus lors de la deuxième ouverture.
  g_object_ref (UI_ACT.type_charges);
#endif
  
  return true;
}

/**
 * \brief Ajoute une nouvelle action à la liste des actions.
 * \param p : la variable projet,
 * \param type : le type de l'action dont la description est donnée par
 *               #_1990_action_bat_txt_type,
 * \param nom : nom de la nouvelle action.
 * \return
 *   Succès : Pointeur vers la nouvelle action.\n
 *   Échec : NULL :
 *     - p == NULL,
 *     - #_1990_action_categorie_bat (type) == ACTION_INCONNUE,
 *     - #_1990_coef_psi0_bat (type),
 *     - #_1990_coef_psi1_bat (type),
 *     - #_1990_coef_psi2_bat (type),
 *     - erreur d'allocation mémoire.
 */
Action *
_1990_action_ajout (Projet            *p,
                    uint8_t            type,
                    const std::string *nom)
{
  std::unique_ptr <Action> action_nouveau (new Action);

  Action *act;
  
  BUGPARAM (p, "%p", p, NULL)
  BUGPARAM (type,
            "%u",
            _1990_action_categorie_bat (type, p->parametres.norme) !=
                                                               ACTION_INCONNUE,
            NULL)
  
  BUG (EF_calculs_free (p), NULL)
  
  action_nouveau.get ()->type = type;
  action_nouveau.get ()->action_predominante = 0;
  action_nouveau.get ()->psi0 = m_f (_1990_coef_psi0_bat (type,
                                                          p->parametres.norme),
                                     FLOTTANT_ORDINATEUR);
  BUG (!std::isnan (m_g (action_nouveau.get ()->psi0)), NULL)
  action_nouveau.get ()->psi1 = m_f (_1990_coef_psi1_bat (type,
                                                          p->parametres.norme),
                                     FLOTTANT_ORDINATEUR);
  BUG (!std::isnan (m_g (action_nouveau.get ()->psi1)), NULL)
  action_nouveau.get ()->psi2 = m_f (_1990_coef_psi2_bat (type,
                                                          p->parametres.norme),
                                     FLOTTANT_ORDINATEUR);
  BUG (!std::isnan (m_g (action_nouveau.get ()->psi2)), NULL)
  action_nouveau->nom.assign (*nom);
  action_nouveau->deplacement = NULL;
  action_nouveau->forces = NULL;
  action_nouveau->efforts_noeuds = NULL;
  action_nouveau->efforts[0].clear ();
  action_nouveau->efforts[1].clear ();
  action_nouveau->efforts[2].clear ();
  action_nouveau->efforts[3].clear ();
  action_nouveau->efforts[4].clear ();
  action_nouveau->efforts[5].clear ();
  action_nouveau->deformation[0].clear ();
  action_nouveau->deformation[1].clear ();
  action_nouveau->deformation[2].clear ();
  action_nouveau->rotation[0].clear ();
  action_nouveau->rotation[1].clear ();
  action_nouveau->rotation[2].clear ();
  
  act = action_nouveau.release ();
  p->actions.push_back (act);
   
#ifdef ENABLE_GTK
  gtk_list_store_append (UI_ACT.liste, &act->Iter_liste);
  gtk_list_store_set (UI_ACT.liste,
                      &act->Iter_liste,
                      0, act->nom.c_str (),
                      -1);
  
  if (UI_ACT.builder != NULL)
  {
    gtk_tree_store_append (UI_ACT.tree_store_actions,
                           &act->Iter_fenetre,
                           NULL);
    gtk_tree_store_set (UI_ACT.tree_store_actions,
                        &act->Iter_fenetre,
                        0, act,
                        -1);
  }
  
  if ((UI_GRO.builder != NULL) &&
      (GTK_COMMON_SPINBUTTON_AS_UINT (GTK_SPIN_BUTTON (
                                             UI_GRO.spin_button_niveau)) == 0))
  {
    GtkTreeIter Iter;
    
    gtk_tree_store_append (UI_GRO.tree_store_dispo, &Iter, NULL);
    gtk_tree_store_set (UI_GRO.tree_store_dispo, &Iter, 0, act, -1);
  }
#endif
  
  return act;
}

/**
 * \brief Renvoie le nom de l'action. Il convient de ne pas libérer ou modifier
 *        la valeur renvoyée.
 * \param action : une action.
 * \return
 *   Succès : Le nom de l'action.\n
 *   Échec : NULL :
 *     - action == NULL,
 */
const std::string
_1990_action_nom_renvoie (Action *action)
{
  BUGPARAM (action, "%p", action, NULL)
  
  return action->nom;
}

/**
 * \brief Renomme une action. L'ancienne valeur est libérée.
 * \param p : la variable projet,
 * \param action : une action,
 * \param nom : nouveau nom de l'action.
 * \return
 *   Succès : true\n
 *   Échec : false :
 *     - p == NULL,
 *     - action == NULL,
 *     - erreur d'allocation mémoire.
 */
bool
_1990_action_nom_change (Projet            *p,
                         Action            *action,
                         const std::string *nom)
{
  BUGPARAM (p, "%p", p, false)
  BUGPARAM (action, "%p", action, false)
  
  action->nom.assign (*nom);
  
#ifdef ENABLE_GTK
  gtk_list_store_set (UI_ACT.liste,
                      &action->Iter_liste,
                      0, (*nom).c_str (),
                      -1);
  
  if (UI_ACT.builder != NULL)
  {
    gtk_widget_queue_resize (GTK_WIDGET (UI_ACT.tree_view_actions));
  }
  if ((UI_GRO.builder != NULL) &&
      (GTK_COMMON_SPINBUTTON_AS_UINT (GTK_SPIN_BUTTON (
                                             UI_GRO.spin_button_niveau)) == 0))
  {
    gtk_widget_queue_resize (GTK_WIDGET (UI_GRO.tree_view_etat));
    gtk_widget_queue_resize (GTK_WIDGET (UI_GRO.tree_view_dispo));
  }
#endif
  
  return true;
}

/**
 * \brief Renvoie le type de l'action. La correspondance avec la description
 *        est obtenue avec la fonction #_1990_action_bat_txt_type.
 * \param action : une action.
 * \return
 *   Succès : le type d'action.\n
 *   Échec : G_MAXUINT :
 *     - action == NULL.
 */
uint8_t
_1990_action_type_renvoie (Action *action)
{
  BUGPARAM (action, "%p", action, INT8_MAX)
  
  return action->type;
}

/**
 * \brief Change le type d'une action, y compris &psi;<sub>0</sub>,
 *        &psi;<sub>1</sub> et &psi;<sub>2</sub>.
 * \param p : la variable projet,
 * \param action : une action,
 * \param type : le nouveau type d'action.
 * \return
 *   Succès : true\n
 *   Échec : false :
 *     - p == NULL,
 *     - action == NULL,
 *     - erreur d'allocation mémoire.
 */
bool
_1990_action_type_change (Projet *p,
                          Action *action,
                          uint8_t type)
{
  Flottant  psi0, psi1, psi2;
  
  BUGPARAM (p, "%p", p, false)
  BUGPARAM (action, "%p", action, false)
  
  if (action->type == type)
  {
    return true;
  }
  
  psi0 = m_f (_1990_coef_psi0_bat (type, p->parametres.norme),
              FLOTTANT_ORDINATEUR);
  BUG (!std::isnan (m_g (psi0)), false)
  psi1 = m_f (_1990_coef_psi1_bat (type, p->parametres.norme),
              FLOTTANT_ORDINATEUR);
  BUG (!std::isnan (m_g (psi1)), false)
  psi2 = m_f (_1990_coef_psi2_bat (type, p->parametres.norme),
              FLOTTANT_ORDINATEUR);
  BUG (!std::isnan (m_g (psi2)), false)
  
  BUG (EF_calculs_free (p), false)
  
  action->type = type;
  action->psi0 = psi0;
  action->psi1 = psi1;
  action->psi2 = psi2;
  
#ifdef ENABLE_GTK
  if (UI_ACT.builder != NULL)
  {
    gtk_widget_queue_resize (GTK_WIDGET (UI_ACT.tree_view_actions));
  }
#endif
  
  return true;
}

/**
 * \brief Renvoie true si la liste des charges est vide ou si action est égale
 *        à NULL.
 * \param action : une action.
 * \return
 *   Succès : action->charges == NULL.\n
 *   Échec : true :
 *     - action == NULL.
 */
bool
_1990_action_charges_vide (Action *action)
{
  BUGPARAM (action, "%p", action, true)
  
  return action->charges.empty ();
}

/**
 * \brief Renvoie la liste des charges.
 * \param action : une action.
 * \return
 *   Succès : action->charges.\n
 *   Échec : NULL :
 *     - action == NULL.
 */
std::list <Charge *> *
_1990_action_charges_renvoie (Action *action)
{
  BUGPARAM (action, "%p", action, NULL)
  
  return &action->charges;
}

/**
 * \brief Renvoie le flag "Action prédominante" de l'action.
 * \param action : une action.
 * \return
 *   Succès : le flag de l'action (0 ou 1).\n
 *   Échec : 2 :
 *     - action == NULL.
 */
uint8_t
_1990_action_flags_action_predominante_renvoie (Action *action)
{
  BUGPARAM (action, "%p", action, 2)
  
  return action->action_predominante;
}

/**
 * \brief Change le flag "Action prédominante" de l'action.
 * \param action : une action,
 * \param flag : la nouvelle valeur du flag (0 ou 1).
 * \return
 *   Succès : true\n
 *   Échec : false :
 *     - action == NULL.
 *     - flag != 0 ou 1.
 */
bool
_1990_action_flags_action_predominante_change (Action *action,
                                               uint8_t flag)
{
  BUGPARAM (action, "%p", action, false)
  BUGPARAM (flag, "%u", (flag == 0) || (flag == 1), false)
  
  action->action_predominante = flag & 1U;
  
  return true;
}

/**
 * \brief Renvoie le coefficient &psi;<sub>0</sub> de l'action.
 * \param action : une action.
 * \return
 *   Succès : le coefficient &psi;<sub>0</sub>.
 *   Échec : NAN :
 *       action == NULL.
 */
Flottant
_1990_action_psi_renvoie_0 (Action *action)
{
  BUGPARAM (action, "%p", action, m_f (NAN, FLOTTANT_ORDINATEUR))
  
  return action->psi0;
}

/**
 * \brief Renvoie le coefficient &psi;<sub>1</sub> de l'action.
 * \param action : une action.
 * \return
 *   Succès : le coefficient &psi;<sub>1</sub>.\n
 *   Échec : NAN :
 *       action == NULL.
 */
Flottant
_1990_action_psi_renvoie_1 (Action *action)
{
  BUGPARAM (action, "%p", action, m_f (NAN, FLOTTANT_ORDINATEUR))
  
  return action->psi1;
}

/**
 * \brief Renvoie le coefficient &psi;<sub>2</sub> de l'action.
 * \param action : une action.
 * \return
 *   Succès : le coefficient &psi;<sub>2</sub>.\n
 *   Échec : NAN :
 *     - action == NULL.
 */
Flottant
_1990_action_psi_renvoie_2 (Action *action)
{
  BUGPARAM (action, "%p", action, m_f (NAN, FLOTTANT_ORDINATEUR))
  
  return action->psi2;
}

/**
 * \brief Change le coefficient &psi; d'une action.
 * \param p : la variable projet,
 * \param action : une action,
 * \param psi_num : coefficient &psi; à changer (0, 1 ou 2),
 * \param psi : nouveau coefficient psi (supérieur ou égale à 0).
 * \return
 *   Succès : true\n
 *   Échec : false :
 *     - p == NULL,
 *     - action == NULL,
 *     - psi_num != 0 et 1 et 2,
 *     - psi < 0.
 */
bool
_1990_action_psi_change (Projet  *p,
                         Action  *action,
                         uint8_t  psi_num,
                         Flottant psi)
{
  BUGPARAM (p, "%p", p, false)
  BUGPARAM (action, "%p", action, false)
  BUGPARAM (psi_num,
            "%u",
            (psi_num == 0) || (psi_num == 1) || (psi_num == 2),
            false)
  BUGPARAM (m_g (psi), "%lf", m_g (psi) >= 0., false)
  
  if (psi_num == 0)
  {
    if (errrel (m_g (psi), m_g (action->psi0)))
    {
      return true;
    }
    
    action->psi0 = psi;
#ifdef ENABLE_GTK
    if (UI_ACT.builder != NULL)
    {
      gtk_widget_queue_resize (GTK_WIDGET (UI_ACT.tree_view_actions));
    }
#endif
  }
  else if (psi_num == 1)
  {
    if (errrel (m_g (psi), m_g (action->psi1)))
    {
      return true;
    }
    
    action->psi1 = psi;
#ifdef ENABLE_GTK
    if (UI_ACT.builder != NULL)
    {
      gtk_widget_queue_resize (GTK_WIDGET (UI_ACT.tree_view_actions));
    }
#endif
  }
  else if (psi_num == 2)
  {
    if (errrel (m_g (psi), m_g (action->psi2)))
    {
      return true;
    }
    
    action->psi2 = psi;
#ifdef ENABLE_GTK
    if (UI_ACT.builder != NULL)
    {
      gtk_widget_queue_resize (GTK_WIDGET (UI_ACT.tree_view_actions));
    }
#endif
  }
  
  BUG (EF_calculs_free (p), false)
  
  return true;
}

/**
 * \brief Renvoie la matrice sparse deplacement de l'action.
 * \param action : une action.
 * \return
 *   Succès : la matrice sparse deplacement.\n
 *   Échec : NULL :
 *     - action == NULL.
 */
cholmod_sparse *
_1990_action_deplacement_renvoie (Action *action)
{
  BUGPARAM (action, "%p", action, NULL)
  
  return action->deplacement;
}

/**
 * \brief Modifie la matrice sparse deplacement de l'action. La précédente
 *        n'est pas libérée.
 * \param action : une action,
 * \param sparse : la nouvelle matrice.
 * \return
 *   Succès : true.\n
 *   Échec : false :
 *     - action == NULL,
 *     - sparse == NULL.
 */
bool
_1990_action_deplacement_change (Action         *action,
                                 cholmod_sparse *sparse)
{
  BUGPARAM (action, "%p", action, false)
  BUGPARAM (sparse, "%p", sparse, false)
  
  action->deplacement = sparse;
  
  return true;
}

/**
 * \brief Renvoie la matrice sparse forces de l'action.
 * \param action : une action.
 * \return
 *   Succès : la matrice sparse forces.\n
 *   Échec : NULL :
 *     - action == NULL.
 */
cholmod_sparse *
_1990_action_forces_renvoie (Action *action)
{
  BUGPARAM (action, "%p", action, NULL)
  
  return action->forces;
}

/**
 * \brief Modifie la matrice sparse forces de l'action. La précédente n'est pas
 *        libérée.
 * \param action : une action,
 * \param sparse : la nouvelle matrice.
 * \return
 *   Succès : true.\n
 *   Échec : false :
 *     - action == NULL,
 *       sparse == NULL.
 */
bool
_1990_action_forces_change (Action         *action,
                            cholmod_sparse *sparse)
{
  BUGPARAM (action, "%p", action, false)
  BUGPARAM (sparse, "%p", sparse, false)
  
  action->forces = sparse;
  
  return true;
}

/**
 * \brief Renvoie la matrice sparse efforts_noeuds de l'action.
 * \param action : une action.
 * \return
 *   Succès : la matrice sparse efforts_noeuds correspondante.\n
 *   Échec : NULL :
 *     - action == NULL.
 */
cholmod_sparse *
_1990_action_efforts_noeuds_renvoie (Action *action)
{
  BUGPARAM (action, "%p", action, NULL)
  
  return action->efforts_noeuds;
}

/**
 * \brief Modifie la matrice sparse efforts_noeuds de l'action. La précédente
 *        n'est pas libérée.
 * \param action : une action,
 * \param sparse : la nouvelle matrice.
 * \return
 *   Succès : true.\n
 *   Échec : false :
 *     - action == NULL,
 *       sparse == NULL.
 */
bool
_1990_action_efforts_noeuds_change (Action         *action,
                                    cholmod_sparse *sparse)
{
  BUGPARAM (action, "%p", action, false)
  BUGPARAM (sparse, "%p", sparse, false)
  
  action->efforts_noeuds = sparse;
  
  return true;
}

#ifdef ENABLE_GTK
/**
 * \brief Renvoie la variable Iter_fenetre.
 * \param action : une action.
 * \return
 *   Succès : Iter_fenetre.\n
 *   Échec : NULL :
 *     - action == NULL.
 */
GtkTreeIter *
_1990_action_Iter_fenetre_renvoie (Action *action)
{
  BUGPARAM (action, "%p", action, NULL)
  
  return &action->Iter_fenetre;
}
#endif

/**
 * \brief Renvoie la fonction Effort dans la barre.
 * \param action : une action,
 * \param effort : l'effort (N : 0, T<sub>y</sub> : 1, T<sub>z</sub> : 2,
 *                 M<sub>x</sub> : 3, M<sub>y</sub> : 4, M<sub>z</sub> : 5),
 * \param barre : la position de la barre dans la liste des barres. 
 * \return
 *   Succès : la fonction effort correspondante.\n
 *   Échec : NULL :
 *     - action == NULL,
 *     - effort != 0, 1, 2, 3, 4 et 5.
 */
Fonction *
_1990_action_efforts_renvoie (Action  *action,
                              uint8_t  effort,
                              uint32_t barre)
{
  BUGPARAM (action, "%p", action, NULL)
  BUGPARAM (effort, "%d", effort <= 5, NULL)
  
  return action->efforts[effort].at (barre);
}

/**
 * \brief Renvoie la fonction Rotation dans la barre.
 * \param action : une action,
 * \param effort : l'effort (r<sub>x</sub> : 0, r<sub>y</sub> : 1,
 *                 r<sub>z</sub> : 2),
 * \param barre : la position de la barre dans la liste des barres.
 * \return
 *   Succès : la fonction Effort correspondante.\n
 *   Échec : NULL :
 *     - action == NULL,
 *     - effort != 0, 1 et 2.
 */
Fonction *
_1990_action_rotation_renvoie (Action  *action,
                               uint8_t  effort,
                               uint32_t barre)
{
  BUGPARAM (action, "%p", action, NULL)
  BUGPARAM (effort, "%d", effort <= 2, NULL)
  
  return action->rotation[effort].at (barre);
}

/**
 * \brief Renvoie la fonction Déformation dans la barre.
 * \param action : une action,
 * \param effort : l'effort (u<sub>x</sub> : 0, u<sub>y</sub> : 1,
 *                 u<sub>z</sub> : 2),
 * \param barre : la position de la barre dans la liste des barres.
 * \return
 *   Succès : la fonction Effort correspondante.\n
 *   Échec : NULL :
 *     - action == NULL,
 *     - effort != 0, 1 et 2.
 */
Fonction *
_1990_action_deformation_renvoie (Action  *action,
                                  uint8_t  effort,
                                  uint32_t barre)
{
  BUGPARAM (action, "%p", action, NULL)
  BUGPARAM (effort, "%d", effort <= 2, NULL)
  
  return action->deformation[effort].at (barre);
}

/**
 * \brief Libère les fonctions de toutes les barres de l'action souhaitée.
 * \param action : une action.
 * \return
 *   Succès : true.\n
 *   Échec : false :
 *     - p == NULL,
 *     - action == NULL.
 */
bool
_1990_action_fonction_free (Action *action)
{
  uint8_t  i;
  
  BUGPARAM (action, "%p", action, false)
  
  for (i = 0; i < 6; i++)
  {
    for_each (action->efforts[i].begin (),
              action->efforts[i].end (),
              std::default_delete <Fonction> ());
    action->efforts[i].clear ();
  }
  
  for (i = 0; i < 3; i++)
  {
    for_each (action->deformation[i].begin (),
              action->deformation[i].end (),
              std::default_delete <Fonction> ());
    action->deformation[i].clear ();
    
    for_each (action->rotation[i].begin (),
              action->rotation[i].end (),
              std::default_delete <Fonction> ());
    action->rotation[i].clear ();
  }
  
  return true;
}

/**
 * \brief Initialise les fonctions décrivant les sollicitations, les rotations
 *        et les déplacements des barres. Cette fonction doit être appelée
 *        lorsque toutes les barres ont été modélisées. En effet, il est
 *        nécessaire de connaître leur nombre afin de stocker dans un tableau
 *        dynamique unique les fonctions. L'initialisation des fonctions
 *        consiste à définir un nombre de tronçon à 0 et les données à NULL.
 * \param p : la variable projet,
 * \param action : une action.
 * \return
 *   Succès : true.\n
 *   Échec : false :
 *     - p == NULL,
 *     - action == NULL,
 *     - en cas d'erreur d'allocation mémoire.
 */
bool
_1990_action_fonction_init (Projet *p,
                            Action *action)
{
  uint8_t  i;
  uint32_t j;
  
  BUGPARAM (p, "%p", p, false)
  BUGPARAM (action, "%p", action, false)
  
  for (i = 0; i < 6; i++)
  {
    for (j = 0; j < p->modele.barres.size (); j++)
    {
      Fonction *fn = new Fonction;
      
      action->efforts[i].push_back (fn);
    }
  }
  
  for (i = 0; i < 3; i++)
  {
    for (j = 0; j < p->modele.barres.size (); j++)
    {
      Fonction *fn;
      
      fn = new Fonction;
      action->deformation[i].push_back (fn);
      
      fn = new Fonction;
      action->rotation[i].push_back (fn);
    }
  }
  
  return true;
}

/**
 * \brief Affiche dans l'entrée standard les actions existantes.
 * \param p : la variable projet.
 * \return
 *   Succès : true.\n
 *   Échec : false :
 *     - p == NULL.
 */
bool
_1990_action_affiche_tout (Projet *p)
{
  std::list <Action *>::iterator it;
  
  BUGPARAM (p, "%p", p, false)
  
  if (p->actions.empty ())
  {
    printf (gettext ("Aucune action existante.\n"));
    return true;
  }
  
  it = p->actions.begin ();
  while (it != p->actions.end ())
  {
    Action *action = *it;
    
    printf (gettext ("Action '%s', type n°%d\n"),
                     action->nom.c_str (),
                     action->type);
    
    ++it;
  }
  
  return true;
}

/**
 * \brief Affiche tous les résultats des calculs dans l'entrée standard.
 * \param p : la variable projet,
 * \param action : une action.
 * \return
 *   Succès : true.\n
 *   Échec : false :
 *     - p == NULL,
 *     - action == NULL.
 */
bool
_1990_action_affiche_resultats (Projet *p,
                                Action *action)
{
  uint32_t i;
  
  BUGPARAM (p, "%p", p, false)
  BUGPARAM (action, "%p", action, false)
  
  if (p->modele.barres.empty ())
  {
    printf (gettext ("Aucune barre existante.\n"));
    return true;
  }
  
  // Affichage des efforts aux noeuds et des réactions d'appuis
  printf ("Effort aux noeuds & Réactions d'appuis :\n");
  common_math_arrondi_sparse (action->efforts_noeuds);
  cholmod_write_sparse (stdout,
                        action->efforts_noeuds,
                        NULL,
                        NULL,
                        p->calculs.c);
  
  // Affichage des déplacements des noeuds
  printf ("\nDéplacements :\n");
  common_math_arrondi_sparse (action->deplacement);
  cholmod_write_sparse (stdout,
                        action->deplacement,
                        NULL,
                        NULL,
                        p->calculs.c);
  
  // Pour chaque barre
  for (i = 0; i < p->modele.barres.size (); i++)
  {
    // Affichage de la courbe des sollicitations de l'effort normal
    printf ("Barre n°%u, Effort normal\n", i);
    BUG (common_fonction_affiche (action->efforts[0].at (i)), false)
    // Affichage de la courbe des sollicitations de l'effort tranchant selon Y
    printf ("Barre n°%u, Effort tranchant Y\n", i);
    BUG (common_fonction_affiche (action->efforts[1].at (i)), false)
    // Affichage de la courbe des sollicitations de l'effort tranchant selon Z
    printf ("Barre n°%u, Effort tranchant Z\n", i);
    BUG (common_fonction_affiche (action->efforts[2].at (i)), false)
    // Affichage de la courbe des sollicitations du moment de torsion
    printf ("Barre n°%u, Moment de torsion\n", i);
    BUG (common_fonction_affiche (action->efforts[3].at (i)), false)
    // Affichage de la courbe des sollicitations du moment fléchissant selon Y
    printf ("Barre n°%u, Moment de flexion Y\n", i);
    BUG (common_fonction_affiche (action->efforts[4].at (i)), false)
    // Affichage de la courbe des sollicitations du moment fléchissant selon Z
    printf ("Barre n°%u, Moment de flexion Z\n", i);
    BUG (common_fonction_affiche (action->efforts[5].at (i)), false)
  }
  for (i = 0; i < p->modele.barres.size (); i++)
  {
    // Affichage de la courbe de déformation selon l'axe X
    printf ("Barre n°%u, Déformation en X\n", i);
    BUG (common_fonction_affiche (action->deformation[0].at (i)), false)
    // Affichage de la courbe de déformation selon l'axe Y
    printf ("Barre n°%u, Déformation en Y\n", i);
    BUG (common_fonction_affiche (action->deformation[1].at (i)), false)
    // Affichage de la courbe de déformation selon l'axe Z
    printf ("Barre n°%u, Déformation en Z\n", i);
    BUG (common_fonction_affiche (action->deformation[2].at (i)), false)
    // Affichage de la courbe de rotation selon l'axe X
    printf ("Barre n°%u, Rotation en X\n", i);
    BUG (common_fonction_affiche (action->rotation[0].at (i)), false)
    // Affichage de la courbe de rotation selon l'axe Y
    printf ("Barre n°%u, Rotation en Y\n", i);
    BUG (common_fonction_affiche (action->rotation[1].at (i)), false)
    // Affichage de la courbe de rotation selon l'axe Z
    printf ("Barre n°%u, Rotation en Z\n", i);
    BUG (common_fonction_affiche (action->rotation[2].at (i)), false)
  }
  // FinPour
  
  return true;
}

/**
 * \brief Libère les résultats de l'action souhaitée crée par
 *        #_1990_action_ponderation_resultat.
 * \param action : une action.
 * \return
 *   Succès : true.\n
 *   Échec : false :
 *     - action == NULL.
 */
bool
_1990_action_ponderation_resultat_free_calculs (Action *action)
{
  BUGPARAM (action, "%p", action, false)
  
  free (action->deplacement->x);
  free (action->deplacement);
  free (action->efforts_noeuds->x);
  free (action->efforts_noeuds);
  
  return true;
}

/**
 * \brief Crée une fausse action sur la base d'une combinaison. L'objectif est
 *        uniquement de regrouper les résultats pondérés de chaque action.
 * \param ponderation : pondération selon laquelle sera créée l'action,
 * \param p : la variable projet.
 * \return
 *   Succès : pointeur vers l'action équivalente créée.\n
 *   Échec : NULL :
 *     - p == NULL,
 *     - aucun noeud n'existe,
 *     - en cas d'erreur d'allocation mémoire (#_1990_action_fonction_init,
 *       #common_fonction_ajout_fonction).
 */
Action *
_1990_action_ponderation_resultat (std::list <Ponderation *> *ponderation,
                                   Projet                    *p)
{
  std::list <Ponderation *>::iterator it;
  Action *action;
  double *x, *y;
  
  BUGPARAM (p, "%p", p, NULL)
  INFO (!p->modele.noeuds.empty (),
        NULL,
        (gettext ("Aucun noeud n'est existant.\n")); )
  
  // Initialisation de l'action
  action = new Action;
  action->efforts_noeuds = new cholmod_sparse;
  action->efforts_noeuds->x = new double [p->modele.noeuds.size () * 6];
  memset (action->efforts_noeuds->x, 0, p->modele.noeuds.size () * 6);
  action->deplacement = new cholmod_sparse;
  action->deplacement->x = new double [p->modele.noeuds.size () * 6];
  memset (action->deplacement->x, 0, p->modele.noeuds.size () * 6);
  BUG (_1990_action_fonction_init (p, action),
       NULL,
       _1990_action_ponderation_resultat_free_calculs (action);
         delete action; )
  x = (double *) action->efforts_noeuds->x;
  y = (double *) action->deplacement->x;
  
#define FREE_ALL \
  _1990_action_fonction_free (action); \
  _1990_action_ponderation_resultat_free_calculs (action); \
  delete action;
  
  // Remplissage de la variable action.
  it = ponderation->begin ();
  while (it != ponderation->end ())
  {
    Ponderation *element = *it;
    double      *x2 = (double *) element->action->efforts_noeuds->x;
    double      *y2 = (double *) element->action->deplacement->x;
    double       mult;
    uint32_t     i;
    
    mult = element->ponderation *
                             (element->psi == 0 ? m_g (element->action->psi0) :
                              element->psi == 1 ? m_g (element->action->psi1) :
                              element->psi == 2 ? m_g (element->action->psi2) :
                               1.);
    for (i = 0; i < p->modele.noeuds.size () * 6; i++)
    {
      x[i] = x[i] + mult*x2[i];
      y[i] = y[i] + mult*y2[i];
    }
    
    for (i = 0; i < p->modele.barres.size (); i++)
    {
      BUG (common_fonction_ajout_fonction (action->efforts[0][i],
                                           element->action->efforts[0][i],
                                           mult),
           NULL,
           FREE_ALL)
      BUG (common_fonction_ajout_fonction (action->efforts[1][i],
                                           element->action->efforts[1][i],
                                           mult),
           NULL,
           FREE_ALL)
      BUG (common_fonction_ajout_fonction (action->efforts[2][i],
                                           element->action->efforts[2][i],
                                           mult),
           NULL,
           FREE_ALL)
      BUG (common_fonction_ajout_fonction (action->efforts[3][i],
                                           element->action->efforts[3][i],
                                           mult),
           NULL,
           FREE_ALL)
      BUG (common_fonction_ajout_fonction (action->efforts[4][i],
                                           element->action->efforts[4][i],
                                           mult),
           NULL,
           FREE_ALL)
      BUG (common_fonction_ajout_fonction (action->efforts[5][i],
                                           element->action->efforts[5][i],
                                           mult),
           NULL,
           FREE_ALL)
      BUG (common_fonction_ajout_fonction (action->deformation[0][i],
                                           element->action->deformation[0][i],
                                           mult),
           NULL,
           FREE_ALL)
      BUG (common_fonction_ajout_fonction (action->deformation[1][i],
                                           element->action->deformation[1][i],
                                           mult),
           NULL,
           FREE_ALL)
      BUG (common_fonction_ajout_fonction (action->deformation[2][i],
                                           element->action->deformation[2][i],
                                           mult),
           NULL,
           FREE_ALL)
      BUG (common_fonction_ajout_fonction (action->rotation[0][i],
                                           element->action->rotation[0][i],
                                           mult),
           NULL,
           FREE_ALL)
      BUG (common_fonction_ajout_fonction (action->rotation[1][i],
                                           element->action->rotation[1][i],
                                           mult),
           NULL,
           FREE_ALL)
      BUG (common_fonction_ajout_fonction (action->rotation[2][i],
                                           element->action->rotation[2][i],
                                           mult),
           NULL,
           FREE_ALL)
    }
    
    ++it;
  }
  
#undef FREE_ALL
  
  return action;
}

/**
 * \brief Libère les résultats de l'action souhaitée.
 * \param p : la variable projet,
 * \param action : une action.
 * \return
 *   Succès : true.\n
 *   Échec : false :
 *     - p == NULL,
 *     - action == NULL.
 */
bool
_1990_action_free_calculs (Projet *p,
                           Action *action)
{
  BUGPARAM (p, "%p", p, false)
  BUGPARAM (action, "%p", action, false)
  
  if (action->deplacement != NULL)
  {
    cholmod_free_sparse (&action->deplacement, p->calculs.c);
    action->deplacement = NULL;
  }
  
  if (action->forces != NULL)
  {
    cholmod_free_sparse (&action->forces, p->calculs.c);
    action->forces = NULL;
  }
  
  if (action->efforts_noeuds != NULL)
  {
    cholmod_free_sparse (&action->efforts_noeuds, p->calculs.c);
    action->efforts_noeuds = NULL;
  }
  
  if (!action->efforts[0].empty ())
  {
    BUG (_1990_action_fonction_free (action), false)
  }
  
  return true;
}

/**
 * \brief Libère l'action souhaitée.
 * \param p : la variable projet,
 * \param action_free : une action à supprimer.
 * \return
 *   Succès : true.\n
 *   Échec : false :
 *     - p == NULL,
 *     - action_free == NULL,
 *     - un type d'une des actions est inconnu.
 */
bool
_1990_action_free_1 (Projet *p,
                     Action *action_free)
{
  std::list <Charge *>::iterator it2;
  
#ifdef ENABLE_GTK
  GtkTreeIter Iter;
#endif
  
  BUGPARAM (p, "%p", p, false)
  BUGPARAM (action_free, "%p", action_free, false)
  
  // On enlève l'action de la liste des actions
  p->actions.remove (action_free);
  
  it2 = action_free->charges.begin ();
  while (it2 != action_free->charges.end ())
  {
    Charge *charge;
    
    charge = *it2;
    switch (charge->type)
    {
      case CHARGE_NOEUD :
      {
        BUG (EF_charge_noeud_free (charge), false)
        break;
      }
      case CHARGE_BARRE_PONCTUELLE :
      {
        BUG (EF_charge_barre_ponctuelle_free (charge), false)
        break;
      }
      case CHARGE_BARRE_REPARTIE_UNIFORME :
      {
        BUG (EF_charge_barre_repartie_uniforme_free (charge), false)
        break;
      }
      default :
      {
        FAILCRIT (false,
                  (gettext ("Type de charge %d inconnu.\n"),
                            charge->type); )
        break;
      }
    }
    
    ++it2;
  }
  
  if (action_free->deplacement != NULL)
  {
    cholmod_free_sparse (&action_free->deplacement, p->calculs.c);
  }
  if (action_free->forces != NULL)
  {
    cholmod_free_sparse (&action_free->forces, p->calculs.c);
  }
  if (action_free->efforts_noeuds != NULL)
  {
    cholmod_free_sparse (&action_free->efforts_noeuds, p->calculs.c);
  }
  
  if (!action_free->efforts[0].empty ())
  {
    BUG (_1990_action_fonction_free (action_free), false)
  }
      
#ifdef ENABLE_GTK
  if (UI_ACT.builder != NULL)
  {
    if (gtk_tree_selection_iter_is_selected (UI_ACT.tree_select_actions,
                                             &action_free->Iter_fenetre))
    {
      gtk_tree_store_clear (UI_ACT.tree_store_charges);
    }
    gtk_tree_store_remove (UI_ACT.tree_store_actions,
                           &action_free->Iter_fenetre);
  }
  
  gtk_list_store_remove (UI_ACT.liste, &action_free->Iter_liste);
#endif
  
  delete action_free;
      

#ifdef ENABLE_GTK
  if (UI_ACT.builder != NULL)
  {
    gtk_widget_queue_resize (GTK_WIDGET (UI_ACT.tree_view_actions));
  }
#endif
                                                                     
  // On enlève l'action dans la liste des groupes de niveau 0.
  if (!p->niveaux_groupes.empty ())
  {
    Niveau_Groupe *niveau_groupe = *p->niveaux_groupes.begin ();
    std::list <Groupe *>::iterator it3 = niveau_groupe->groupes.begin ();
    
    while (it3 != niveau_groupe->groupes.end ())
    {
      Groupe *groupe = *it3;
      std::list <void *>::iterator it = groupe->elements.begin ();
      
      while (it != groupe->elements.end ())
      {
        Action *act_it = static_cast <Action *> (*it);
        if (act_it == action_free)
        {
          BUG (_1990_groupe_retire_element (p,
                                            niveau_groupe,
                                            groupe,
                                            act_it),
               false)
          break;
        }
      }
      
      ++it3;
    }
  }
  
  // Il faut aussi parcourir le treeview des éléments inutilisés pour l'enlever
  // au cas où.
#ifdef ENABLE_GTK
  if ((UI_GRO.builder != NULL) &&
      (GTK_COMMON_SPINBUTTON_AS_UINT (GTK_SPIN_BUTTON (
                                           UI_GRO.spin_button_niveau)) == 0) &&
      (gtk_tree_model_get_iter_first (GTK_TREE_MODEL (
                                             UI_GRO.tree_store_dispo), &Iter)))
  {
    do
    {
      GtkTreeIter Iter_en_cours = Iter;
      Action     *action_en_cours;
      
      gtk_tree_model_get (GTK_TREE_MODEL (UI_GRO.tree_store_dispo),
                          &Iter_en_cours,
                          0, &action_en_cours,
                          -1);
      if (action_free == action_en_cours)
      {
        gtk_tree_store_remove (GTK_TREE_STORE (UI_GRO.tree_store_dispo),
                               &Iter_en_cours);
        break;
      }
      
    } while (gtk_tree_model_iter_next (GTK_TREE_MODEL (
                                                      UI_GRO.tree_store_dispo),
                                       &Iter));
  }
#endif
  
  BUG (EF_calculs_free (p), false)
  
  return true;
}

/**
 * \brief Libère l'ensemble des actions existantes.
 * \param p : la variable projet.
 * \return
 *   Succès : true.\n
 *   Échec : false :
 *     - p == NULL.
 */
bool
_1990_action_free (Projet *p)
{
  std::list <Action *>::iterator it;
  
  BUGPARAM (p, "%p", p, false)
  
  it = p->actions.begin ();
  while (it != p->actions.end ())
  {
    Action *action = *it;
    std::list <Charge *>::iterator it2 = action->charges.begin ();
    
    while (it2 != action->charges.end ())
    {
      Charge *charge = *it2;
      
      switch (charge->type)
      {
        case CHARGE_NOEUD :
        {
          BUG (EF_charge_noeud_free (charge), false)
          break;
        }
        case CHARGE_BARRE_PONCTUELLE :
        {
          BUG (EF_charge_barre_ponctuelle_free (charge), false)
          break;
        }
        case CHARGE_BARRE_REPARTIE_UNIFORME :
        {
          BUG (EF_charge_barre_repartie_uniforme_free (charge), false)
          break;
        }
        default :
        {
          FAILCRIT (false,
                    (gettext ("Type de charge %d inconnu.\n"), charge->type); )
          break;
        }
      }
      
      ++it2;
    }
    
    if (action->deplacement != NULL)
    {
      cholmod_free_sparse (&action->deplacement, p->calculs.c);
    }
    if (action->forces != NULL)
    {
      cholmod_free_sparse (&action->forces, p->calculs.c);
    }
    if (action->efforts_noeuds != NULL)
    {
      cholmod_free_sparse (&action->efforts_noeuds, p->calculs.c);
    }
    
    if (!action->efforts[0].empty ())
    {
      BUG (_1990_action_fonction_free (action), false)
    }
    
    delete action;
    
    ++it;
  }
  p->actions.clear ();
  
#ifdef ENABLE_GTK
  if (UI_ACT.builder != NULL)
  {
    gtk_tree_store_clear (UI_ACT.tree_store_charges);
  }
  g_object_unref (UI_ACT.type_charges);
  g_object_unref (UI_ACT.liste);
  gtk_list_store_clear (UI_ACT.choix_type_action);
  g_object_unref (UI_ACT.choix_type_action);
  
  for_each (UI_ACT.items_type_action.begin (),
            UI_ACT.items_type_action.end (),
            gtk_widget_destroy);
  g_object_ref_sink (UI_ACT.type_action);
  g_object_unref (UI_ACT.type_action);
  UI_ACT.items_type_action.clear ();
#endif
  
  return true;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
