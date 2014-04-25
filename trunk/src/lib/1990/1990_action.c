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
#include <cholmod.h>
#include <string.h>
#include <gmodule.h>
#include <math.h>
#include <stdint.h>

#include "1990_action_private.h"
#include "common_projet.h"
#include "1990_coef_psi.h"
#include "1990_groupe.h"
#include "common_math.h"
#include "common_erreurs.h"
#include "common_fonction.h"
#include "EF_charge_noeud.h"
#include "EF_charge_barre_ponctuelle.h"
#include "EF_charge_barre_repartie_uniforme.h"
#include "EF_calculs.h"

#ifdef ENABLE_GTK
#include "common_gtk.h"
#include "1990_gtk_actions.h"
#endif

/**
 * \brief Renvoie la description du type de l'action pour les bâtiments de la
 *        norme européenne.
 * \param type : type de l'action. Ce paramètre doit être compris entre 0 et la
 *               valeur retournée par #_1990_action_num_bat_txt.
 * \return 
 *   Succès : le texte correspondant en fonction du paramètre type :\n
 *     - 0 : "Permanente",
 *     - 1 : "Précontrainte",
 *     - 2 : "Exploitation : Catégorie A : habitation, zones résidentielles",
 *     - 3 : "Exploitation : Catégorie B : bureaux",
 *     - 4 : "Exploitation : Catégorie C : lieux de réunion",
 *     - 5 : "Exploitation : Catégorie D : commerces",
 *     - 6 : "Exploitation : Catégorie E : stockage",
 *     - 7 : "Exploitation : Catégorie F : zone de trafic, véhicules inférieur
 *            à 30 kN",
 *     - 8 : "Exploitation : Catégorie G : zone de trafic, véhicules entre 30
 *            kN et 160 kN",
 *     - 9 : "Exploitation : Catégorie H : toits",
 *     - 10: "Neige : Finlande, Islande, Norvège, Suède",
 *     - 11: "Neige : Autres états membres CEN, altitude > 1000 m",
 *     - 12: "Neige : Autres états membres CEN, altitude <= 1000 m",
 *     - 13: "Vent",
 *     - 14: "Température (hors incendie)",
 *     - 15: "Accidentelle",
 *     - 16: "Sismique".
 * \return Échec : NULL :
 *     - Le type d'action n'existe pas.
 * \warning Fonction interne. Il convient d'utiliser la fonction
 *          #_1990_action_bat_txt_type.
 */
char *
_1990_action_bat_txt_type_eu (uint8_t type)
{
  switch (type)
  {
    case 0 : return gettext ("Permanente");
    case 1 : return gettext ("Précontrainte");
    case 2 : return gettext ("Exploitation : Catégorie A : habitation, zones résidentielles");
    case 3 : return gettext ("Exploitation : Catégorie B : bureaux");
    case 4 : return gettext ("Exploitation : Catégorie C : lieux de réunion");
    case 5 : return gettext ("Exploitation : Catégorie D : commerces");
    case 6 : return gettext ("Exploitation : Catégorie E : stockage");
    case 7 : return gettext ("Exploitation : Catégorie F : zone de trafic, véhicules de poids inférieur à 30 kN");
    case 8 : return gettext ("Exploitation : Catégorie G : zone de trafic, véhicules de poids entre 30 kN et 160 kN");
    case 9 : return gettext ("Exploitation : Catégorie H : toits"); 
    case 10 : return gettext ("Neige : Finlande, Islande, Norvège, Suède"); 
    case 11 : return gettext ("Neige : Autres états membres CEN, altitude > 1000 m"); 
    case 12 : return gettext ("Neige : Autres états membres CEN, altitude <= 1000 m"); 
    case 13 : return gettext ("Vent");
    case 14 : return gettext ("Température (hors incendie)");
    case 15 : return gettext ("Accidentelle");
    case 16 : return gettext ("Sismique");
    default : { FAILPARAM (type, "%u", NULL) break; }
  }
}


/**
 * \brief Renvoie la description du type de l'action pour les bâtiments de la
 *        norme française.
 * \param type : type de l'action. Ce paramètre doit être compris entre 0 et la
 *               valeur retournée par #_1990_action_num_bat_txt.
 * \return
 *   Succès : le texte correspondant en fonction du paramètre type :\n
 *     - 0 : "Permanente",
 *     - 1 : "Précontrainte",
 *     - 2 : "Exploitation : Catégorie A : habitation, zones résidentielles",
 *     - 3 : "Exploitation : Catégorie B : bureaux",
 *     - 4 : "Exploitation : Catégorie C : lieux de réunion",
 *     - 5 : "Exploitation : Catégorie D : commerces",
 *     - 6 : "Exploitation : Catégorie E : stockage",
 *     - 7 : "Exploitation : Catégorie F : zone de trafic, véhicules inférieur
 *           à 30 kN",
 *     - 8 : "Exploitation : Catégorie G : zone de trafic, véhicules entre 30
 *           kN et 160 kN",
 *     - 9 : "Exploitation : Catégorie H : toits d'un bâtiment de catégorie A
 *           ou B",
 *     - 10: "Exploitation : Catégorie I : toitures accessibles avec locaux de
 *           type A ou B",
 *     - 11: "Exploitation : Catégorie I : toitures accessibles avec locaux de
 *           type C ou D",
 *     - 12: "Exploitation : Catégorie K : Hélicoptère sur la toiture",
 *     - 13: "Exploitation : Catégorie K : Hélicoptère sur la toiture, autres
 *           charges",
 *     - 14: "Neige : Saint-Pierre-et-Miquelon",
 *     - 15: "Neige : Altitude > 1000 m",
 *     - 16: "Neige : Altitude <= 1000 m",
 *     - 17: "Vent",
 *     - 18: "Température (hors incendie)",
 *     - 19: "Accidentelle",
 *     - 20: "Sismique",
 *     - 21: "Eaux souterraines".
 * \return Échec : NULL :
 *     - Le type d'action n'existe pas.
 * \warning Fonction interne. Il convient d'utiliser la fonction
 *          #_1990_action_bat_txt_type.
 */
char *
_1990_action_bat_txt_type_fr (uint8_t type)
{
  switch (type)
  {
    case 0 : return gettext ("Permanente");
    case 1 : return gettext ("Précontrainte");
    case 2 : return gettext ("Exploitation : Catégorie A : habitation, zones résidentielles");
    case 3 : return gettext ("Exploitation : Catégorie B : bureaux");
    case 4 : return gettext ("Exploitation : Catégorie C : lieux de réunion");
    case 5 : return gettext ("Exploitation : Catégorie D : commerces");
    case 6 : return gettext ("Exploitation : Catégorie E : stockage");
    case 7 : return gettext ("Exploitation : Catégorie F : zone de trafic, véhicules de poids inférieur à 30 kN");
    case 8 : return gettext ("Exploitation : Catégorie G : zone de trafic, véhicules de poids entre 30 kN et 160 kN");
    case 9 : return gettext ("Exploitation : Catégorie H : toits d'un bâtiment de catégorie A ou B");
    case 10 : return gettext ("Exploitation : Catégorie I : toitures accessibles avec locaux des catégories A ou B");
    case 11 : return gettext ("Exploitation : Catégorie I : toitures accessibles avec locaux des catégories C ou D");
    case 12 : return gettext ("Exploitation : Catégorie K : Hélicoptère sur la toiture");
    case 13 : return gettext ("Exploitation : Catégorie K : Hélicoptère sur la toiture, autres charges (fret, personnel, accessoires ou équipements divers)");
    case 14 : return gettext ("Neige : Saint-Pierre-et-Miquelon");
    case 15 : return gettext ("Neige : Altitude > 1000 m");
    case 16 : return gettext ("Neige : Altitude <= 1000 m");
    case 17 : return gettext ("Vent");
    case 18 : return gettext ("Température (hors incendie)");
    case 19 : return gettext ("Accidentelle");
    case 20 : return gettext ("Sismique");
    case 21 : return gettext ("Eaux souterraines");
    default : { FAILPARAM (type, "%u", NULL) break; }
  }
}


/**
 * \brief Renvoie la description du type de l'action pour les bâtiments en
 *        fonction de la norme demandée. Le retour ne doit pas être libéré.
 * \param type : type de l'action,
 * \param norme : la norme souhaitée.
 * \return
 *   Succès : cf. _1990_action_bat_txt_type_PAYS.\n
 *   Échec : NULL :
 *     - le type d'action n'existe pas,
 *     - la norme n'existe pas.
 */
char *
_1990_action_bat_txt_type (uint8_t type,
                           Norme   norme)
{
  switch (norme)
  {
    case NORME_EU : return _1990_action_bat_txt_type_eu (type);
    case NORME_FR : return _1990_action_bat_txt_type_fr (type);
    default : { FAILPARAM (norme, "%d", NULL) break; }
  }
}


/**
 * \brief Renvoie la catégorie du type de l'action pour les bâtiments selon la
 *        norme européenne.
 * \param type : type de l'action.
 * \return
 *   Succès :\n
 *     - 0 : #ACTION_POIDS_PROPRE (Poids propre),
 *     - 1 : #ACTION_PRECONTRAINTE (Précontrainte),
 *     - 2 à 14 : #ACTION_VARIABLE (Action variable),
 *     - 15 : #ACTION_ACCIDENTELLE (Action accidentelle),
 *     - 16 : #ACTION_SISMIQUE (Action sismique).
 * \return Échec : #ACTION_INCONNUE :
 *     - Le type d'action n'existe pas.
 * \warning Fonction interne. Il convient d'utiliser la fonction
 *          #_1990_action_categorie_bat.
 */
Action_Categorie
_1990_action_categorie_bat_eu (uint8_t type)
{
  if (type == 0)
  {
    return ACTION_POIDS_PROPRE;
  }
  else if (type == 1)
  {
    return ACTION_PRECONTRAINTE;
  }
  else if ((2 <= type) && (type <= 14))
  {
    return ACTION_VARIABLE;
  }
  else if (type == 15)
  {
    return ACTION_ACCIDENTELLE;
  }
  else if (type == 16)
  {
    return ACTION_SISMIQUE;
  }
  else
  {
    FAILPARAM (type, "%u", ACTION_INCONNUE)
  }
}


/**
 * \brief Renvoie la catégorie du type de l'action pour les bâtiments selon la
 *        norme française.
 * \param type : type de l'action.
 * \return
 *   Succès :\n
 *     - 0 : #ACTION_POIDS_PROPRE (Poids propre),
 *     - 1 : #ACTION_PRECONTRAINTE (Précontrainte),
 *     - 2 à 18: #ACTION_VARIABLE (Action variable),
 *     - 19 : #ACTION_ACCIDENTELLE (Action accidentelle),
 *     - 20 : #ACTION_SISMIQUE (Action sismique),
 *     - 21 : #ACTION_EAUX_SOUTERRAINES (Action due aux eaux souterraines).
 * \return Échec : #ACTION_INCONNUE :
 *     - Le type d'action n'existe pas.
 * \warning Fonction interne. Il convient d'utiliser la fonction
 *          #_1990_action_categorie_bat.
 */
Action_Categorie
_1990_action_categorie_bat_fr (uint8_t type)
{
  if (type == 0) 
  {
    return ACTION_POIDS_PROPRE;
  }
  else if (type == 1) 
  {
    return ACTION_PRECONTRAINTE;
  }
  else if ((2 <= type) && (type <= 18))
  {
    return ACTION_VARIABLE;
  }
  else if (type == 19)
  {
    return ACTION_ACCIDENTELLE;
  }
  else if (type == 20)
  {
    return ACTION_SISMIQUE;
  }
  else if (type == 21)
  {
    return ACTION_EAUX_SOUTERRAINES;
  }
  else
  {
    FAILPARAM (type, "%u", ACTION_INCONNUE)
  }
}


/**
 * \brief Renvoie la catégorie du type de l'action pour les bâtiments en
 *        fonction de la norme souhaitée.
 * \param type : type de l'action,
 * \param norme : la norme souhaitée.
 * \return
 *   Succès : cf. _1990_action_categorie_bat_PAYS.\n
 *   Échec : #ACTION_INCONNUE :
 *     - La catégorie n'existe pas,
 *     - La norme n'existe pas.
 */
Action_Categorie
_1990_action_categorie_bat (uint8_t type,
                            Norme   norme)
{
  switch (norme)
  {
    case NORME_EU : return _1990_action_categorie_bat_eu (type);
    case NORME_FR : return _1990_action_categorie_bat_fr (type);
    default : { FAILPARAM (norme, "%d", ACTION_INCONNUE) break; }
  }
}


/**
 * \brief Renvoie le nombre de catégories d'actions des bâtiments en fonction
 *        de la norme.
 * \param norme : la norme souhaitée.
 * \return
 *   Succès : le nombre de catégorie d'actions.\n
 *   Échec : 0 :
 *     - La norme n'existe pas.
 */
uint8_t
_1990_action_num_bat_txt (Norme norme)
{
  switch (norme)
  {
    case NORME_EU : return 17;
    case NORME_FR : return 22;
    default : { FAILPARAM (norme, "%d", 0) break; }
  }
}


/**
 * \brief Initialise la liste des actions.
 * \param p : la variable projet.
 * \return
 *   Succès : TRUE\n
 *   Échec : FALSE :
 *     - p == NULL.
 * \warning Fonction interne. Il convient d'utiliser la fonction #projet_init.
 */
gboolean
_1990_action_init (Projet *p)
{
#ifdef ENABLE_GTK
  uint8_t    i;
  GtkWidget *w_temp;
#endif
  
  BUGPARAMCRIT (p, "%p", p, FALSE)
  
  p->actions = NULL;
  
#ifdef ENABLE_GTK
  UI_ACT.liste = gtk_list_store_new (1, G_TYPE_STRING);
  UI_ACT.builder = NULL;
  
  UI_ACT.items_type_action = NULL;
  UI_ACT.type_action = GTK_MENU (gtk_menu_new ());
  UI_ACT.choix_type_action = gtk_list_store_new (1, G_TYPE_STRING);
  for (i = 0; i < _1990_action_num_bat_txt (p->parametres.norme); i++)
  {
    GtkTreeIter iter;
    
    // Génération du menu contenant la liste des types d'action pour la
    // création d'une nouvelle action.
    w_temp = gtk_menu_item_new_with_label (
                           _1990_action_bat_txt_type (i, p->parametres.norme));
    gtk_menu_shell_append (GTK_MENU_SHELL (UI_ACT.type_action), w_temp);
    UI_ACT.items_type_action = g_list_append (UI_ACT.items_type_action,
                                              w_temp);
    gtk_widget_show (w_temp);
    g_signal_connect (w_temp,
                      "activate",
                      G_CALLBACK (_1990_gtk_nouvelle_action),
                      p);
    
    // Génération de la liste des types d'action pour la modification via le
    // treeview Action.
    gtk_list_store_append (UI_ACT.choix_type_action, &iter);
    gtk_list_store_set (UI_ACT.choix_type_action,
                        &iter,
                        0, _1990_action_bat_txt_type (i, p->parametres.norme),
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
  
  return TRUE;
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
_1990_action_ajout (Projet     *p,
                    uint8_t     type,
                    const char *nom)
{
  Action  *action_nouveau;
  
  BUGPARAM (p, "%p", p, NULL)
  BUGPARAM (type,
            "%u",
            _1990_action_categorie_bat (type, p->parametres.norme) !=
                                                               ACTION_INCONNUE,
            NULL)
  
  BUG (EF_calculs_free (p), NULL)
  
  BUGCRIT (action_nouveau = (Action *) malloc (sizeof (Action)),
           NULL,
           (gettext ("Erreur d'allocation mémoire.\n")); )
  action_nouveau->type = type;
  action_nouveau->charges = NULL;
  action_nouveau->action_predominante = 0;
  action_nouveau->psi0 = m_f (_1990_coef_psi0_bat (type, p->parametres.norme),
                              FLOTTANT_ORDINATEUR);
  BUG (!isnan (m_g (action_nouveau->psi0)), NULL, free (action_nouveau); )
  action_nouveau->psi1 = m_f (_1990_coef_psi1_bat (type, p->parametres.norme),
                              FLOTTANT_ORDINATEUR);
  BUG (!isnan (m_g (action_nouveau->psi1)), NULL, free (action_nouveau); )
  action_nouveau->psi2 = m_f (_1990_coef_psi2_bat (type, p->parametres.norme),
                              FLOTTANT_ORDINATEUR);
  BUG (!isnan (m_g (action_nouveau->psi2)), NULL, free (action_nouveau); )
  BUGCRIT (action_nouveau->nom = g_strdup_printf ("%s", nom),
           NULL,
           (gettext ("Erreur d'allocation mémoire.\n"));
             free (action_nouveau); )
  action_nouveau->deplacement = NULL;
  action_nouveau->forces = NULL;
  action_nouveau->efforts_noeuds = NULL;
  action_nouveau->efforts[0] = NULL;
  action_nouveau->efforts[1] = NULL;
  action_nouveau->efforts[2] = NULL;
  action_nouveau->efforts[3] = NULL;
  action_nouveau->efforts[4] = NULL;
  action_nouveau->efforts[5] = NULL;
  action_nouveau->deformation[0] = NULL;
  action_nouveau->deformation[1] = NULL;
  action_nouveau->deformation[2] = NULL;
  action_nouveau->rotation[0] = NULL;
  action_nouveau->rotation[1] = NULL;
  action_nouveau->rotation[2] = NULL;
  
  p->actions = g_list_append (p->actions, action_nouveau);
   
#ifdef ENABLE_GTK
  gtk_list_store_append (UI_ACT.liste, &action_nouveau->Iter_liste);
  gtk_list_store_set (UI_ACT.liste,
                      &action_nouveau->Iter_liste,
                      0, action_nouveau->nom,
                      -1);
  
  if (UI_ACT.builder != NULL)
  {
    gtk_tree_store_append (UI_ACT.tree_store_actions,
                           &action_nouveau->Iter_fenetre,
                           NULL);
    gtk_tree_store_set (UI_ACT.tree_store_actions,
                        &action_nouveau->Iter_fenetre,
                        0, action_nouveau,
                        -1);
  }
  
  if ((UI_GRO.builder != NULL) &&
      (GTK_COMMON_SPINBUTTON_AS_UINT (GTK_SPIN_BUTTON (
                                             UI_GRO.spin_button_niveau)) == 0))
  {
    GtkTreeIter Iter;
    
    gtk_tree_store_append (UI_GRO.tree_store_dispo, &Iter, NULL);
    gtk_tree_store_set (UI_GRO.tree_store_dispo,
                        &Iter,
                        0, action_nouveau,
                        -1);
  }
#endif
  
  return action_nouveau;
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
const char *
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
 *   Succès : TRUE\n
 *   Échec : FALSE :
 *     - p == NULL,
 *     - action == NULL,
 *     - erreur d'allocation mémoire.
 */
gboolean
_1990_action_nom_change (Projet     *p,
                         Action     *action,
                         const char *nom)
{
  BUGPARAM (p, "%p", p, FALSE)
  BUGPARAM (action, "%p", action, FALSE)
  BUGPARAM (nom, "%p", nom, FALSE)
  
  free (action->nom);
  BUGCRIT (action->nom = g_strdup_printf ("%s", nom),
           FALSE,
           (gettext ("Erreur d'allocation mémoire.\n")); )
  
#ifdef ENABLE_GTK
  gtk_list_store_set (UI_ACT.liste, &action->Iter_liste, 0, nom, -1);
  
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
  
  return TRUE;
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
 *   Succès : TRUE\n
 *   Échec : FALSE :
 *     - p == NULL,
 *     - action == NULL,
 *     - erreur d'allocation mémoire.
 */
gboolean
_1990_action_type_change (Projet *p,
                          Action *action,
                          uint8_t type)
{
  Flottant  psi0, psi1, psi2;
  
  BUGPARAM (p, "%p", p, FALSE)
  BUGPARAM (action, "%p", action, FALSE)
  
  if (action->type == type)
  {
    return TRUE;
  }
  
  psi0 = m_f (_1990_coef_psi0_bat (type, p->parametres.norme),
              FLOTTANT_ORDINATEUR);
  BUG (!isnan (m_g (psi0)), FALSE)
  psi1 = m_f (_1990_coef_psi1_bat (type, p->parametres.norme),
              FLOTTANT_ORDINATEUR);
  BUG (!isnan (m_g (psi1)), FALSE)
  psi2 = m_f (_1990_coef_psi2_bat (type, p->parametres.norme),
              FLOTTANT_ORDINATEUR);
  BUG (!isnan (m_g (psi2)), FALSE)
  
  BUG (EF_calculs_free (p), FALSE)
  
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
  
  return TRUE;
}


/**
 * \brief Renvoie TRUE si la liste des charges est vide ou si action est égale
 *        à NULL.
 * \param action : une action.
 * \return
 *   Succès : action->charges == NULL.\n
 *   Échec : TRUE :
 *     - action == NULL.
 */
gboolean
_1990_action_charges_vide (Action *action)
{
  BUGPARAM (action, "%p", action, TRUE)
  
  return action->charges == NULL;
}


/**
 * \brief Renvoie la liste des charges.
 * \param action : une action.
 * \return
 *   Succès : action->charges.\n
 *   Échec : NULL :
 *     - action == NULL.
 */
GList *
_1990_action_charges_renvoie (Action *action)
{
  BUGPARAM (action, "%p", action, NULL)
  
  return action->charges;
}


/**
 * \brief Change la liste des charges. L'ancienne liste n'est pas libérée.
 * \param action : une action,
 * \param charges : la nouvelle liste des charges.
 * \return
 *   Succès : TRUE\n
 *   Échec : FALSE :
 *     - action == NULL.
 */
gboolean
_1990_action_charges_change (Action *action,
                             GList  *charges)
{
  BUGPARAM (action, "%p", action, FALSE)
  
  action->charges = charges;
  
  return TRUE;
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
 *   Succès : TRUE\n
 *   Échec : FALSE :
 *     - action == NULL.
 *     - flag != 0 ou 1.
 */
gboolean
_1990_action_flags_action_predominante_change (Action *action,
                                               uint8_t flag)
{
  BUGPARAM (action, "%p", action, FALSE)
  BUGPARAM (flag, "%u", (flag == 0) || (flag == 1), FALSE)
  
  action->action_predominante = flag & 1U;
  
  return TRUE;
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
 *   Succès : TRUE\n
 *   Échec : FALSE :
 *     - p == NULL,
 *     - action == NULL,
 *     - psi_num != 0 et 1 et 2,
 *     - psi < 0.
 */
gboolean
_1990_action_psi_change (Projet  *p,
                         Action  *action,
                         uint8_t  psi_num,
                         Flottant psi)
{
  BUGPARAM (p, "%p", p, FALSE)
  BUGPARAM (action, "%p", action, FALSE)
  BUGPARAM (psi_num,
            "%u",
            (psi_num == 0) || (psi_num == 1) || (psi_num == 2),
            FALSE)
  BUGPARAM (m_g (psi), "%lf", m_g (psi) >= 0., FALSE)
  
  if (psi_num == 0)
  {
    if (errrel (m_g (psi), m_g (action->psi0)))
    {
      return TRUE;
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
      return TRUE;
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
      return TRUE;
    }
    
    action->psi2 = psi;
#ifdef ENABLE_GTK
    if (UI_ACT.builder != NULL)
    {
      gtk_widget_queue_resize (GTK_WIDGET (UI_ACT.tree_view_actions));
    }
#endif
  }
  
  BUG (EF_calculs_free (p), FALSE)
  
  return TRUE;
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
 *   Succès : TRUE.\n
 *   Échec : NULL :
 *     - action == NULL,
 *     - sparse == NULL.
 */
gboolean
_1990_action_deplacement_change (Action         *action,
                                 cholmod_sparse *sparse)
{
  BUGPARAM (action, "%p", action, FALSE)
  BUGPARAM (sparse, "%p", sparse, FALSE)
  
  action->deplacement = sparse;
  
  return TRUE;
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
 *   Succès : TRUE.\n
 *   Échec : NULL :
 *     - action == NULL,
 *       sparse == NULL.
 */
gboolean
_1990_action_forces_change (Action         *action,
                            cholmod_sparse *sparse)
{
  BUGPARAM (action, "%p", action, FALSE)
  BUGPARAM (sparse, "%p", sparse, FALSE)
  
  action->forces = sparse;
  
  return TRUE;
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
 *   Succès : TRUE.\n
 *   Échec : NULL :
 *     - action == NULL,
 *       sparse == NULL.
 */
gboolean
_1990_action_efforts_noeuds_change (Action         *action,
                                    cholmod_sparse *sparse)
{
  BUGPARAM (action, "%p", action, FALSE)
  BUGPARAM (sparse, "%p", sparse, FALSE)
  
  action->efforts_noeuds = sparse;
  
  return TRUE;
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
  
  return action->efforts[effort][barre];
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
  
  return action->rotation[effort][barre];
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
  
  return action->deformation[effort][barre];
}


/**
 * \brief Libère les fonctions de toutes les barres de l'action souhaitée.
 * \param p : la variable projet,
 * \param action : une action.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL,
 *     - action == NULL.
 */
gboolean
_1990_action_fonction_free (Projet *p,
                            Action *action)
{
  uint8_t  i;
  uint32_t j;
  
  BUGPARAM (p, "%p", p, FALSE)
  BUGPARAM (action, "%p", action, FALSE)
  
  for (i = 0; i < 6; i++)
  {
    if (action->efforts[i] != NULL)
    {
      for (j = 0; j < g_list_length (p->modele.barres); j++)
      {
        free (action->efforts[i][j]->troncons);
        free (action->efforts[i][j]);
      }
      free (action->efforts[i]);
      action->efforts[i] = NULL;
    }
  }
  
  for (i = 0; i < 3; i++)
  {
    if (action->deformation[i] != NULL)
    {
      for (j = 0; j < g_list_length (p->modele.barres); j++)
      {
        free (action->deformation[i][j]->troncons);
        free (action->deformation[i][j]);
      }
      free (action->deformation[i]);
      action->deformation[i] = NULL;
    }
    
    if (action->rotation[i] != NULL)
    {
      for (j = 0; j < g_list_length (p->modele.barres); j++)
      {
        free (action->rotation[i][j]->troncons);
        free (action->rotation[i][j]);
      }
      free (action->rotation[i]);
      action->rotation[i] = NULL;
    }
  }
  
  return TRUE;
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
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL,
 *     - action == NULL,
 *     - en cas d'erreur d'allocation mémoire.
 */
gboolean
_1990_action_fonction_init (Projet *p,
                            Action *action)
{
  uint8_t  i;
  uint32_t j;
  
  BUGPARAM (p, "%p", p, FALSE)
  BUGPARAM (action, "%p", action, FALSE)
  
  for (i = 0; i < 6; i++)
  {
    BUGCRIT (action->efforts[i] = (Fonction **) malloc (
                       sizeof (Fonction *) * g_list_length (p->modele.barres)),
             FALSE,
             (gettext ("Erreur d'allocation mémoire.\n"));
               _1990_action_fonction_free (p, action); )
    for (j = 0; j < g_list_length (p->modele.barres); j++)
    {
      BUGCRIT (action->efforts[i][j] = (Fonction *) malloc (sizeof (Fonction)),
               FALSE,
               (gettext ("Erreur d'allocation mémoire.\n"));
                 _1990_action_fonction_free (p, action); )
      action->efforts[i][j]->nb_troncons = 0;
      action->efforts[i][j]->troncons = NULL;
    }
  }
  
  for (i = 0; i < 3; i++)
  {
    BUGCRIT (action->deformation[i] = (Fonction **) malloc (
                       sizeof (Fonction *) * g_list_length (p->modele.barres)),
             FALSE,
             (gettext("Erreur d'allocation mémoire.\n"));
               _1990_action_fonction_free (p, action); )
    for (j = 0; j < g_list_length (p->modele.barres); j++)
    {
      BUGCRIT (action->deformation[i][j] = (Fonction *) malloc (
                                                            sizeof (Fonction)),
               FALSE,
               (gettext ("Erreur d'allocation mémoire.\n"));
                 _1990_action_fonction_free (p, action); )
      action->deformation[i][j]->nb_troncons = 0;
      action->deformation[i][j]->troncons = NULL;
    }
    
    BUGCRIT (action->rotation[i] = (Fonction **) malloc (
                       sizeof (Fonction *) * g_list_length (p->modele.barres)),
             FALSE,
             (gettext ("Erreur d'allocation mémoire.\n"));
               _1990_action_fonction_free (p, action); )
    for (j = 0; j < g_list_length (p->modele.barres); j++)
    {
      BUGCRIT (action->rotation[i][j] = (Fonction *) malloc (sizeof (
                                                                    Fonction)),
               FALSE,
               (gettext ("Erreur d'allocation mémoire.\n"));
                 _1990_action_fonction_free (p, action); )
      action->rotation[i][j]->nb_troncons = 0;
      action->rotation[i][j]->troncons = NULL;
    }
  }
  
  return TRUE;
}


/**
 * \brief Affiche dans l'entrée standard les actions existantes.
 * \param p : la variable projet.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL.
 */
gboolean
_1990_action_affiche_tout (Projet *p)
{
  GList *list_parcours;
  
  BUGPARAM (p, "%p", p, FALSE)
  
  if (p->actions == NULL)
  {
    printf (gettext ("Aucune action existante.\n"));
    return TRUE;
  }
  
  list_parcours = p->actions;
  do
  {
    Action *action = list_parcours->data;
    
    printf (gettext ("Action '%s', type n°%d\n"), action->nom, action->type);
    
    list_parcours = g_list_next (list_parcours);
  }
  while (list_parcours != NULL);
  
  return TRUE;
}


/**
 * \brief Affiche tous les résultats des calculs dans l'entrée standard.
 * \param p : la variable projet,
 * \param action : une action.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL,
 *     - action == NULL.
 */
gboolean
_1990_action_affiche_resultats (Projet *p,
                                Action *action)
{
  uint32_t i;
  
  BUGPARAM (p, "%p", p, FALSE)
  BUGPARAM (action, "%p", action, FALSE)
  
  if (p->modele.barres == NULL)
  {
    printf (gettext ("Aucune barre existante.\n"));
    return TRUE;
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
  for (i = 0; i < g_list_length (p->modele.barres); i++)
  {
    // Affichage de la courbe des sollicitations de l'effort normal
    printf ("Barre n°%u, Effort normal\n", i);
    BUG (common_fonction_affiche (action->efforts[0][i]), FALSE)
    // Affichage de la courbe des sollicitations de l'effort tranchant selon Y
    printf ("Barre n°%u, Effort tranchant Y\n", i);
    BUG (common_fonction_affiche (action->efforts[1][i]), FALSE)
    // Affichage de la courbe des sollicitations de l'effort tranchant selon Z
    printf ("Barre n°%u, Effort tranchant Z\n", i);
    BUG (common_fonction_affiche (action->efforts[2][i]), FALSE)
    // Affichage de la courbe des sollicitations du moment de torsion
    printf ("Barre n°%u, Moment de torsion\n", i);
    BUG (common_fonction_affiche (action->efforts[3][i]), FALSE)
    // Affichage de la courbe des sollicitations du moment fléchissant selon Y
    printf ("Barre n°%u, Moment de flexion Y\n", i);
    BUG (common_fonction_affiche (action->efforts[4][i]), FALSE)
    // Affichage de la courbe des sollicitations du moment fléchissant selon Z
    printf ("Barre n°%u, Moment de flexion Z\n", i);
    BUG (common_fonction_affiche (action->efforts[5][i]), FALSE)
  }
  for (i = 0; i < g_list_length (p->modele.barres); i++)
  {
    // Affichage de la courbe de déformation selon l'axe X
    printf ("Barre n°%u, Déformation en X\n", i);
    BUG (common_fonction_affiche (action->deformation[0][i]), FALSE)
    // Affichage de la courbe de déformation selon l'axe Y
    printf ("Barre n°%u, Déformation en Y\n", i);
    BUG (common_fonction_affiche (action->deformation[1][i]), FALSE)
    // Affichage de la courbe de déformation selon l'axe Z
    printf ("Barre n°%u, Déformation en Z\n", i);
    BUG (common_fonction_affiche (action->deformation[2][i]), FALSE)
    // Affichage de la courbe de rotation selon l'axe X
    printf ("Barre n°%u, Rotation en X\n", i);
    BUG (common_fonction_affiche (action->rotation[0][i]), FALSE)
    // Affichage de la courbe de rotation selon l'axe Y
    printf ("Barre n°%u, Rotation en Y\n", i);
    BUG (common_fonction_affiche (action->rotation[1][i]), FALSE)
    // Affichage de la courbe de rotation selon l'axe Z
    printf ("Barre n°%u, Rotation en Z\n", i);
    BUG (common_fonction_affiche (action->rotation[2][i]), FALSE)
  }
  // FinPour
  
  return TRUE;
}


/**
 * \brief Libère les résultats de l'action souhaitée crée par
 *        #_1990_action_ponderation_resultat.
 * \param action : une action.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - action == NULL.
 */
gboolean
_1990_action_ponderation_resultat_free_calculs (Action *action)
{
  BUGPARAM (action, "%p", action, FALSE)
  
  free (action->deplacement->x);
  free (action->deplacement);
  free (action->efforts_noeuds->x);
  free (action->efforts_noeuds);
  
  return TRUE;
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
// coverity[+alloc]
Action *
_1990_action_ponderation_resultat (GList  *ponderation,
                                   Projet *p)
{
  GList  *list_parcours;
  Action *action;
  double *x, *y;
  
  BUGPARAM (p, "%p", p, NULL)
  INFO (p->modele.noeuds, NULL, (gettext ("Aucun noeud n'est existant.\n")); )
  
  // Initialisation de l'action
  BUGCRIT (action = malloc (sizeof (Action)),
           NULL,
           (gettext ("Erreur d'allocation mémoire.\n")); )
  BUGCRIT (action->efforts_noeuds = malloc (sizeof (cholmod_sparse)),
           NULL,
           (gettext ("Erreur d'allocation mémoire.\n"));
             free (action); )
  BUGCRIT (action->efforts_noeuds->x = malloc (sizeof (double) *
                                         g_list_length (p->modele.noeuds) * 6),
           NULL,
           (gettext ("Erreur d'allocation mémoire.\n"));
             free (action->efforts_noeuds);
             free (action); )
  memset (action->efforts_noeuds->x,
          0,
          sizeof (double) * g_list_length (p->modele.noeuds) * 6);
  BUGCRIT (action->deplacement = malloc (sizeof (cholmod_sparse)),
           NULL,
           (gettext ("Erreur d'allocation mémoire.\n"));
             free (action->efforts_noeuds->x);
             free (action->efforts_noeuds);
             free (action); )
  BUGCRIT (action->deplacement->x = malloc (sizeof (double) *
                                         g_list_length (p->modele.noeuds) * 6),
           NULL,
           (gettext ("Erreur d'allocation mémoire.\n"));
             free (action->deplacement);
             free (action->efforts_noeuds->x);
             free (action->efforts_noeuds);
             free (action); )
  memset (action->deplacement->x,
          0,
          sizeof (double) * g_list_length (p->modele.noeuds) * 6);
  BUG (_1990_action_fonction_init (p, action),
       NULL,
       _1990_action_ponderation_resultat_free_calculs (action);
         free (action); )
  x = action->efforts_noeuds->x;
  y = action->deplacement->x;
  
#define FREE_ALL \
  _1990_action_fonction_free (p, action); \
  _1990_action_ponderation_resultat_free_calculs (action); \
  free (action);
  
  // Remplissage de la variable action.
  list_parcours = ponderation;
  while (list_parcours != NULL)
  {
    Ponderation *element = list_parcours->data;
    double      *x2 = element->action->efforts_noeuds->x;
    double      *y2 = element->action->deplacement->x;
    double       mult;
    uint32_t     i;
    
    mult = element->ponderation *
                             (element->psi == 0 ? m_g (element->action->psi0) :
                              element->psi == 1 ? m_g (element->action->psi1) :
                              element->psi == 2 ? m_g (element->action->psi2) :
                               1.);
    for (i = 0; i < g_list_length (p->modele.noeuds) * 6; i++)
    {
      x[i] = x[i] + mult*x2[i];
      y[i] = y[i] + mult*y2[i];
    }
    
    for (i = 0; i < g_list_length (p->modele.barres); i++)
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
    
    list_parcours = g_list_next (list_parcours);
  }
  
#undef FREE_ALL
  
  return action;
}


/**
 * \brief Libère les résultats de l'action souhaitée.
 * \param p : la variable projet,
 * \param action : une action.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL,
 *     - action == NULL.
 */
gboolean
_1990_action_free_calculs (Projet *p,
                           Action *action)
{
  BUGPARAM (p, "%p", p, FALSE)
  BUGPARAM (action, "%p", action, FALSE)
  
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
  
  if (action->efforts[0] != NULL)
  {
    BUG (_1990_action_fonction_free (p, action), FALSE)
  }
  
  return TRUE;
}


/**
 * \brief Libère l'action souhaitée.
 * \param p : la variable projet,
 * \param action_free : une action à supprimer.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL,
 *     - action_free == NULL,
 *     - un type d'une des actions est inconnu.
 */
gboolean 
_1990_action_free_1 (Projet *p,
                     Action *action_free)
{
  GList *list_parcours;
  
#ifdef ENABLE_GTK
  GtkTreeIter Iter;
#endif
  
  BUGPARAM (p, "%p", p, FALSE)
  BUGPARAM (action_free, "%p", action_free, FALSE)
  
  // On enlève l'action de la liste des actions
  list_parcours = g_list_last (p->actions);
  do
  {
    Action *action = list_parcours->data;
    
    list_parcours = g_list_previous (list_parcours);
    if (action == action_free)
    {
      p->actions = g_list_remove (p->actions, action);
      free (action->nom);
      while (action->charges != NULL)
      {
        Charge *charge;
        
        charge = action->charges->data;
        action->charges = g_list_delete_link (action->charges,
                                              action->charges);
        switch (charge->type)
        {
          case CHARGE_NOEUD :
          {
            BUG (EF_charge_noeud_free (charge), FALSE)
            break;
          }
          case CHARGE_BARRE_PONCTUELLE :
          {
            BUG (EF_charge_barre_ponctuelle_free (charge), FALSE)
            break;
          }
          case CHARGE_BARRE_REPARTIE_UNIFORME :
          {
            BUG (EF_charge_barre_repartie_uniforme_free (charge), FALSE)
            break;
          }
          default :
          {
            FAILCRIT (FALSE,
                      (gettext ("Type de charge %d inconnu.\n"),
                                charge->type); )
            break;
          }
        }
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
      
      if (action->efforts[0] != NULL)
      {
        BUG (_1990_action_fonction_free (p, action), FALSE)
      }
      
#ifdef ENABLE_GTK
      if (UI_ACT.builder != NULL)
      {
        if (gtk_tree_selection_iter_is_selected (UI_ACT.tree_select_actions,
                                                 &action->Iter_fenetre))
        {
          gtk_tree_store_clear (UI_ACT.tree_store_charges);
        }
        gtk_tree_store_remove (UI_ACT.tree_store_actions,
                               &action->Iter_fenetre);
      }
      
      gtk_list_store_remove (UI_ACT.liste, &action->Iter_liste);
#endif
      
      free (action);
      
      break;
    }
  } while (list_parcours != NULL);

#ifdef ENABLE_GTK
  if (UI_ACT.builder != NULL)
  {
    gtk_widget_queue_resize (GTK_WIDGET (UI_ACT.tree_view_actions));
  }
#endif
                                                                     
  // On enlève l'action dans la liste des groupes de niveau 0.
  list_parcours = p->niveaux_groupes;
  if (list_parcours != NULL)
  {
    Niveau_Groupe *niveau_groupe = list_parcours->data;
    GList         *list_groupes = niveau_groupe->groupes;
    
    while (list_groupes != NULL)
    {
      Groupe *groupe = list_groupes->data;
      GList  *list_elements = groupe->elements;
      
      while (list_elements != NULL)
      {
        if (list_elements->data == action_free)
        {
          BUG (_1990_groupe_retire_element (p,
                                            niveau_groupe,
                                            groupe,
                                            list_elements->data),
               FALSE)
          break;
        }
        
        list_elements = g_list_next (list_elements);
      }
      
      list_groupes = g_list_next (list_groupes);
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
  
  BUG (EF_calculs_free (p), FALSE)
  
  return TRUE;
}


/**
 * \brief Libère l'ensemble des actions existantes.
 * \param p : la variable projet.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL.
 */
gboolean
_1990_action_free (Projet *p)
{
  BUGPARAM (p, "%p", p, FALSE)
  
  while (p->actions != NULL)
  {
    Action *action = p->actions->data;
    
    p->actions = g_list_delete_link (p->actions, p->actions);
    
    free (action->nom);
    while (action->charges != NULL)
    {
      Charge *charge = action->charges->data;
      
      action->charges = g_list_delete_link (action->charges, action->charges);
      
      switch (charge->type)
      {
        case CHARGE_NOEUD :
        {
          BUG (EF_charge_noeud_free (charge), FALSE)
          break;
        }
        case CHARGE_BARRE_PONCTUELLE :
        {
          BUG (EF_charge_barre_ponctuelle_free (charge), FALSE)
          break;
        }
        case CHARGE_BARRE_REPARTIE_UNIFORME :
        {
          BUG (EF_charge_barre_repartie_uniforme_free (charge), FALSE)
          break;
        }
        default :
        {
          FAILCRIT (FALSE,
                    (gettext ("Type de charge %d inconnu.\n"), charge->type); )
          break;
        }
      }
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
    
    if (action->efforts[0] != NULL)
    {
      BUG (_1990_action_fonction_free (p, action), FALSE)
    }
    
    free (action);
  }
  
#ifdef ENABLE_GTK
  if (UI_ACT.builder != NULL)
  {
    gtk_tree_store_clear (UI_ACT.tree_store_charges);
  }
  g_object_ref_sink (UI_ACT.type_action);
  g_object_unref (UI_ACT.type_action);
  g_object_unref (UI_ACT.type_charges);
  g_object_unref (UI_ACT.liste);
  gtk_list_store_clear (UI_ACT.choix_type_action);
  g_object_unref (UI_ACT.choix_type_action);
  g_list_free (UI_ACT.items_type_action);
  UI_ACT.items_type_action = NULL;
#endif
  
  return TRUE;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
