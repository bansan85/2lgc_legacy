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
#include <stdio.h>
#include <stdlib.h>
#include <cholmod.h>
#include <string.h>

#include "1990_coef_psi.h"
#include "common_projet.h"
#include "common_maths.h"
#include "common_erreurs.h"
#include "common_fonction.h"
#include "EF_charge_noeud.h"
#include "EF_charge_barre_ponctuelle.h"
#include "EF_charge_barre_repartie_uniforme.h"


char* _1990_action_type_bat_txt_eu(int type)
/* Description : renvoie la description des types de charge pour les bâtiments de la norme
 *                 européenne.
 *               FONCTION INTERNE. Utiliser _1990_action_type_bat_txt.
 * Paramètres : int type : type de charge
 * Valeur renvoyée :
 *   Succès : le texte correspondant :
 *            0 : Permanente
 *            1 : Précontrainte
 *            2 : Exploitation : Catégorie A : habitation, zones résidentielles
 *            3 : Exploitation : Catégorie B : bureaux
 *            4 : Exploitation : Catégorie C : lieux de réunion
 *            5 : Exploitation : Catégorie D : commerces
 *            6 : Exploitation : Catégorie E : stockage
 *            7 : Exploitation : Catégorie F : zone de trafic, véhicules inférieur à 30 kN
 *            8 : Exploitation : Catégorie G : zone de trafic, véhicules entre 30 kN et 160 kN
 *            9 : Exploitation : Catégorie H : toits
 *            10: Neige : Finlande, Islande, Norvège, Suède
 *            11: Neige : Autres états membres CEN, altitude > 1000 m
 *            12: Neige : Autres états membres CEN, altitude <= 1000 m
 *            13: Vent
 *            14: Température (hors incendie)
 *            15: Accidentelle
 *            16: Sismique
 *   Échec : NULL en cas de paraètres invalides :
 *             la catégorie n'existe pas.
 */
{
    // Trivial
    switch(type)
    {
        case 0 : { return gettext("Permanente"); break; }
        case 1 : { return gettext("Précontrainte"); break; }
        case 2 : { return gettext("Exploitation : Catégorie A : habitation, zones résidentielles"); break; }
        case 3 : { return gettext("Exploitation : Catégorie B : bureaux"); break; }
        case 4 : { return gettext("Exploitation : Catégorie C : lieux de réunion"); break; }
        case 5 : { return gettext("Exploitation : Catégorie D : commerces"); break; }
        case 6 : { return gettext("Exploitation : Catégorie E : stockage"); break; }
        case 7 : { return gettext("Exploitation : Catégorie F : zone de trafic, véhicules de poids inférieur à 30 kN"); break; }
        case 8 : { return gettext("Exploitation : Catégorie G : zone de trafic, véhicules de poids entre 30 kN et 160 kN"); break; }
        case 9 : { return gettext("Exploitation : Catégorie H : toits"); break; }
        case 10 : { return gettext("Neige : Finlande, Islande, Norvège, Suède"); break; }
        case 11 : { return gettext("Neige : Autres états membres CEN, altitude > 1000 m"); break; }
        case 12 : { return gettext("Neige : Autres états membres CEN, altitude <= 1000 m"); break; }
        case 13 : { return gettext("Vent"); break; }
        case 14 : { return gettext("Température (hors incendie)"); break; }
        case 15 : { return gettext("Accidentelle"); break; }
        case 16 : { return gettext("Sismique"); break; }
        default : { BUGMSG(0, NULL, gettext("Catégorie %d inconnue.\n"), type); break; }
    }
}


char* _1990_action_type_bat_txt_fr(int type)
/* Description : renvoie la description des types de charge pour les bâtiments de la norme
 *                 française.
 *               FONCTION INTERNE. Utiliser _1990_action_type_bat_txt.
 * Paramètres : int type : type de charge
 * Valeur renvoyée :
 *   Succès : le texte correspondant :
 *            0 : Permanente
 *            1 : Précontrainte
 *            2 : Exploitation : Catégorie A : habitation, zones résidentielles
 *            3 : Exploitation : Catégorie B : bureaux
 *            4 : Exploitation : Catégorie C : lieux de réunion
 *            5 : Exploitation : Catégorie D : commerces
 *            6 : Exploitation : Catégorie E : stockage
 *            7 : Exploitation : Catégorie F : zone de trafic, véhicules inférieur à 30 kN
 *            8 : Exploitation : Catégorie G : zone de trafic, véhicules entre 30 kN et 160 kN
 *            9 : Exploitation : Catégorie H : toits d'un bâtiment de catégorie A ou B
 *            10: Exploitation : Catégorie I : toitures accessibles avec locaux de type A ou B
 *            11: Exploitation : Catégorie I : toitures accessibles avec locaux de type C ou D
 *            12: Exploitation : Catégorie K : Hélicoptère sur la toiture
 *            13: Exploitation : Catégorie K : Hélicoptère sur la toiture, autres charges
 *            14: Neige : Saint-Pierre-et-Miquelon
 *            15: Neige : Altitude > 1000 m
 *            16: Neige : Altitude <= 1000 m
 *            17: Vent
 *            18: Température (hors incendie)
 *            19: Accidentelle
 *            20: Sismique
 *            21: Eaux souterraines
 *   Échec : NULL en cas de paramètres invalides :
 *             la catégorie n'existe pas.
 */
{
    // Trivial
    switch(type)
    {
        case 0 : { return gettext("Permanente"); break; }
        case 1 : { return gettext("Précontrainte"); break; }
        case 2 : { return gettext("Exploitation : Catégorie A : habitation, zones résidentielles"); break; }
        case 3 : { return gettext("Exploitation : Catégorie B : bureaux"); break; }
        case 4 : { return gettext("Exploitation : Catégorie C : lieux de réunion"); break; }
        case 5 : { return gettext("Exploitation : Catégorie D : commerces"); break; }
        case 6 : { return gettext("Exploitation : Catégorie E : stockage"); break; }
        case 7 : { return gettext("Exploitation : Catégorie F : zone de trafic, véhicules de poids inférieur à 30 kN"); break; }
        case 8 : { return gettext("Exploitation : Catégorie G : zone de trafic, véhicules de poids entre 30 kN et 160 kN"); break; }
        case 9 : { return gettext("Exploitation : Catégorie H : toits d'un bâtiment de catégorie A ou B"); break; }
        case 10 : { return gettext("Exploitation : Catégorie I : toitures accessibles avec locaux des catégories A ou B"); break; }
        case 11 : { return gettext("Exploitation : Catégorie I : toitures accessibles avec locaux des catégories C ou D"); break; }
        case 12 : { return gettext("Exploitation : Catégorie K : Hélicoptère sur la toiture"); break; }
        case 13 : { return gettext("Exploitation : Catégorie K : Hélicoptère sur la toiture, autres charges (fret, personnel, accessoires ou équipements divers)"); break; }
        case 14 : { return gettext("Neige : Saint-Pierre-et-Miquelon"); break; }
        case 15 : { return gettext("Neige : Altitude > 1000 m"); break; }
        case 16 : { return gettext("Neige : Altitude <= 1000 m"); break; }
        case 17 : { return gettext("Vent"); break; }
        case 18 : { return gettext("Température (hors incendie)"); break; }
        case 19 : { return gettext("Accidentelle"); break; }
        case 20 : { return gettext("Sismique"); break; }
        case 21 : { return gettext("Eaux souterraines"); break; }
        default : { BUGMSG(0, NULL, gettext("Catégorie %d inconnue.\n"), type); break; }
    }
}


char* _1990_action_type_bat_txt(int type, Type_Pays pays)
/* Description : renvoie la description des types de charge pour les bâtiments en fonction de
 *                 la norme demandée.
 * Paramètres : int type : type de charge
 *            : Type_Pays pays : le numéro du pays
 * Valeur renvoyée :
 *   Succès : le texte correspondant
 *   Échec : NULL en cas de paramètres invalides :
 *             le type n'existe pas ou
 *             le pays n'existe pas.
 */
{
    // Trivial
    switch (pays)
    {
        case PAYS_EU : { return _1990_action_type_bat_txt_eu(type); break; }
        case PAYS_FR : { return _1990_action_type_bat_txt_fr(type); break; }
        default : { BUGMSG(0, NULL, gettext("Pays %d inconnu.\n"), pays); break; }
    }
}


Action_Categorie _1990_action_categorie_bat_eu(int type)
/* Description : renvoie la categorie d'une action pour les combinaisons d'action des bâtiments
 *                 selon la norme européenne.
 *               FONCTION INTERNE. Utiliser _1990_action_categorie_bat.
 * Paramètres : int type : type de l'action
 * Valeur renvoyée :
 *   Succès : ACTION_POIDS_PROPRE : Poids propre
 *            ACTION_PRECONTRAINTE : Précontrainte
 *            ACTION_VARIABLE : Action variable
 *            ACTION_ACCIDENTELLE : Action accidentelle
 *            ACTION_SISMIQUE : Action sismique
 *   Échec : ACTION_INCONNUE en cas de paramètres invalides :
 *             La catégorie n'existe pas
 */
{
    // Trivial
    if (type == 0)
        return ACTION_POIDS_PROPRE;
    else if (type == 1)
        return ACTION_PRECONTRAINTE;
    else if ((2 <= type) && (type <= 14))
        return ACTION_VARIABLE;
    else if (type == 15)
        return ACTION_ACCIDENTELLE;
    else if (type == 16)
        return ACTION_SISMIQUE;
    else
        BUGMSG(0, ACTION_INCONNUE, gettext("Catégorie %d inconnue.\n"), type);
}


Action_Categorie _1990_action_categorie_bat_fr(int type)
/* Description : renvoie la catégorie d'une action pour les combinaisons d'action des bâtiments
 *                 selon la norme française.
 *               FONCTION INTERNE. Utiliser _1990_action_categorie_bat.
 * Paramètres : int type : catégorie de l'action
 * Valeur renvoyée :
 *   Succès : ACTION_POIDS_PROPRE : Poids propre
 *            ACTION_PRECONTRAINTE : Précontrainte
 *            ACTION_VARIABLE : Action variable
 *            ACTION_ACCIDENTELLE : Action accidentelle
 *            ACTION_SISMIQUE : Action sismique
 *            ACTION_EAUX_SOUTERRAINES : Action due aux eaux souterraines
 *   Échec : ACTION_INCONNUE en cas de paramètres invalides :
 *             La catégorie n'existe pas
 */
{
    // Trivial
    if (type == 0) 
        return ACTION_POIDS_PROPRE;
    else if (type == 1) 
        return ACTION_PRECONTRAINTE;
    else if ((2 <= type) && (type <= 18))
        return ACTION_VARIABLE;
    else if (type == 19)
        return ACTION_ACCIDENTELLE;
    else if (type == 20)
        return ACTION_SISMIQUE;
    else if (type == 21)
        return ACTION_EAUX_SOUTERRAINES;
    else
        BUGMSG(0, ACTION_INCONNUE, gettext("Catégorie %d inconnue.\n"), type);
}


Action_Categorie _1990_action_categorie_bat(int type, Type_Pays pays)
/* Description : renvoie la catégorie d'une action pour les combinaisons d'action des bâtiments
 *                 en fonction du pays.
 * Paramètres : int type : type de l'action
 *            : Type_Pays pays : le numéro du pays
 * Valeur renvoyée :
 *   Succès : cf les fonctions _1990_action_categorie_bat_PAYS
 *   Échec : ACTION_INCONNUE en cas de paramètres invalides :
 *             La catégorie n'existe pas.
 *             Le pays n'existe pas.
 */
{
    // Trivial
    switch (pays)
    {
        case PAYS_EU : { return _1990_action_categorie_bat_eu(type); break; }
        case PAYS_FR : { return _1990_action_categorie_bat_fr(type); break; }
        default : { BUGMSG(0, ACTION_INCONNUE, gettext("Pays %d inconnu.\n"), pays); break; }
    }
}


int _1990_action_num_bat_txt(Type_Pays pays)
/* Description : renvoie le nombre de catégorie d'actions des bâtiments en fonction du pays.
 * Paramètres : Type_Pays pays : le numéro du pays
 * Valeur renvoyée :
 *   Succès : le nombre de catégorie d'actions
 *   Échec : 0 en cas de paramètres invalides :
 *             Le pays n'existe pas.
 */
{
    // Trivial
    switch (pays)
    {
        case PAYS_EU : { return 17; break; }
        case PAYS_FR : { return 22; break; }
        default : { BUGMSG(0, 0, gettext("Pays %d inconnu.\n"), pays); break; }
    }
}


int _1990_action_init(Projet *projet)
/* Description : Initialise la liste des actions.
 * Paramètres : Projet *projet : la variable projet
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet == NULL)
 *           -2 en cas d'erreur d'allocation mémoire
 */
{
    // Trivial
    BUGMSG(projet, -1, gettext("Paramètre incorrect\n"));
    
    projet->actions = list_init();
    BUGMSG(projet->actions, -2, gettext("Erreur d'allocation mémoire.\n"));
    
    return 0;
}


int _1990_action_ajout(Projet *projet, int type, const char* description)
/* Description : ajoute une nouvelle action à la liste des actions en lui attribuant le numéro
 *                 suivant le dernier relachement existant.
 * Paramètres : Projet *projet : la variable projet
 *            : int type : le type de l'action
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet == NULL) ou
 *             (projet->actions == NULL) ou
 *           -2 en cas d'erreur d'allocation mémoire
 *           -3 en cas d'erreur due à une fonction interne
 */
{
    // Trivial
    Action      *action_dernier, action_nouveau;
    
    BUGMSG(projet, -1, gettext("Paramètre incorrect\n"));
    BUGMSG(projet->actions, -1, gettext("Paramètre incorrect\n"));
    BUG(_1990_action_categorie_bat(type, projet->pays) != ACTION_INCONNUE, -3);
    
    list_mvrear(projet->actions);
    BUGMSG(action_nouveau.description = g_strdup_printf("%s", description), -2, gettext("Erreur d'allocation mémoire.\n"));
    action_nouveau.type = type;
    BUGMSG(action_nouveau.charges = list_init(), -2, gettext("Erreur d'allocation mémoire.\n"));
    action_nouveau.deplacement_complet = NULL;
    action_nouveau.forces_complet = NULL;
    action_nouveau.efforts_noeuds = NULL;
    action_nouveau.flags = 0;
    action_nouveau.psi0 = _1990_coef_psi0_bat(type, projet->pays);
    BUG(action_nouveau.psi0 >= 0, -3);
    action_nouveau.psi1 = _1990_coef_psi1_bat(type, projet->pays);
    BUG(action_nouveau.psi1 >= 0, -3);
    action_nouveau.psi2 = _1990_coef_psi2_bat(type, projet->pays);
    BUG(action_nouveau.psi2 >= 0, -3);
    action_nouveau.fonctions_efforts[0] = NULL;
    action_nouveau.fonctions_efforts[1] = NULL;
    action_nouveau.fonctions_efforts[2] = NULL;
    action_nouveau.fonctions_efforts[3] = NULL;
    action_nouveau.fonctions_efforts[4] = NULL;
    action_nouveau.fonctions_efforts[5] = NULL;
    action_nouveau.fonctions_deformation[0] = NULL;
    action_nouveau.fonctions_deformation[1] = NULL;
    action_nouveau.fonctions_deformation[2] = NULL;
    action_nouveau.fonctions_rotation[0] = NULL;
    action_nouveau.fonctions_rotation[1] = NULL;
    action_nouveau.fonctions_rotation[2] = NULL;
    
    action_dernier = (Action *)list_rear(projet->actions);
    if (action_dernier == NULL)
        action_nouveau.numero = 0;
    else
        action_nouveau.numero = action_dernier->numero+1;
    
    BUGMSG(list_insert_after(projet->actions, &(action_nouveau), sizeof(action_nouveau)), -2, gettext("Erreur d'allocation mémoire.\n"));
    
    return 0;
}


int _1990_action_cherche_numero(Projet *projet, size_t numero)
/* Description : Cherche et marque l'action numero comme celle en cours.
 * Paramètres : Projet *projet : la variable projet
 *            : size_t numero : le numéro de l'action
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet == NULL) ou
 *             (projet->actions == NULL) ou
 *             (list_size(projet->actions) == 0) ou
 *             action introuvable
 */
{
    BUGMSG(projet, -1, gettext("Paramètre incorrect\n"));
    BUGMSG(projet->actions, -1, gettext("Paramètre incorrect\n"));
    BUGMSG(list_size(projet->actions), -1, gettext("Paramètre incorrect\n"));
    
    // Trivial
    list_mvfront(projet->actions);
    do
    {
        Action      *action = (Action*)list_curr(projet->actions);
        
        if (action->numero == numero)
            return 0;
    }
    while (list_mvnext(projet->actions) != NULL);
    
    BUGMSG(0, -1, gettext("Action %zu introuvable.\n"), numero);
}


void *_1990_action_cherche_charge(Projet *projet, size_t num_action, size_t num_charge)
/* Description : Renvoie la charge demandée.
 * Paramètres : Projet *projet : la variable projet
 *            : size_t num_action : le numéro de l'action
 *            : size_t num_charge : le numéro de la charge
 * Valeur renvoyée :
 *   Succès : Pointeur vers la charge recherchée.
 *   Échec : NULL en cas de paramètres invalides :
 *             (projet == NULL) ou
 *             (projet->actions == NULL) ou
 *             (list_size(projet->actions) == 0) ou
 *             (list_size(projet->charges) == 0)
 *           NULL en cas d'erreur due à une fonction interne :
 *             action introuvable
 */
{
    Action *action;
    
    BUGMSG(projet, NULL, gettext("Paramètre incorrect\n"));
    BUGMSG(projet->actions, NULL, gettext("Paramètre incorrect\n"));
    BUGMSG(list_size(projet->actions), NULL, gettext("Paramètre incorrect\n"));
    
    BUG(_1990_action_cherche_numero(projet, num_action) == 0, NULL);
    action = list_curr(projet->actions);
    
    BUGMSG(list_size(action->charges), NULL, gettext("Paramètre incorrect\n"));
    
    list_mvfront(action->charges);
    do
    {
        Charge_Noeud *charge = list_curr(action->charges);
        
        if (charge->numero == num_charge)
            return charge;
    }
    while (list_mvnext(action->charges));
    
    BUGMSG(NULL, NULL, gettext("Charge %zu introuvable.\n"), num_charge);
}


int _1990_action_deplace_charge(Projet *projet, size_t action_src, size_t charge_src,
  size_t action_dest)
/* Description : Déplace une charge d'une action à l'autre. Décrémente également le numéro
 *               des charges afin que la liste des numéros soit toujours continue.
 * Paramètres : Projet *projet : la variable projet,
 *              size_t action_src : numéro de l'action où se situe la charge à déplacer,
 *              size_t charge_src : numéro de la charge à déplacer,
 *              size_t action_dest : numéro de l'action où sera déplacer la charge.
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet == NULL) ou
 *             (projet->actions == NULL) ou
 *             (list_size(projet->actions)-1 < MAX(action_src, action_dest)) ou
 *             (action->charges == NULL) avec action = action_en_cours ou
 *             charge introuvable
 *           -2 en cas d'erreur d'allocation mémoire
 */
{
    Charge_Noeud            *charge_data = NULL;
#ifdef ENABLE_GTK
    List_Gtk_1990_Actions   *list_gtk_1990_actions;
#endif
    
    BUGMSG(projet, -1, gettext("Paramètre incorrect\n"));
    BUGMSG(projet->actions, -1, gettext("Paramètre incorrect\n"));
    BUGMSG(list_size(projet->actions)-1 >= action_src, -1, gettext("Paramètre incorrect : %zu\n"), action_src);
    BUGMSG(list_size(projet->actions)-1 >= action_dest, -1, gettext("Paramètre incorrect : %zu\n"), action_dest);
    
#ifdef ENABLE_GTK
    list_gtk_1990_actions = &projet->list_gtk._1990_actions;
#endif
    
    if (action_src == action_dest)
        return 0;
    
    list_mvfront(projet->actions);
    // On cherche l'action qui contient la charge
    do
    {
        Action *action = (Action*)list_curr(projet->actions);
        
    // Lorsqu'elle est trouvée,
        if (action->numero == action_src)
        {
            BUGMSG(action->charges, -1, gettext("Paramètre incorrect\n"));
            BUGMSG(list_size(action->charges), -1, gettext("Charge %zu introuvable.\n"), charge_src);
            
            list_mvfront(action->charges);
    //     Pour chaque charge de l'action en cours Faire
            do
            {
                Charge_Noeud *charge = (Charge_Noeud*)list_curr(action->charges);
                
    //         Si la charge est celle à supprimer Alors
                if (charge->numero == charge_src)
                {
#ifdef ENABLE_GTK
    //             On la supprime du tree-view-charge
                    gtk_tree_store_remove(list_gtk_1990_actions->tree_store_charges, &charge->Iter);
#endif
    //             et de la liste des charges tout en conservant les données
    //               de la charge dans charge_data.
                    charge_data = list_remove_curr(action->charges);
                    charge = (Charge_Noeud*)list_curr(action->charges);
                }
                
    //         Sinon Si la charge possède un numéro supérieur à la charge supprimée alors
                if ((charge_data != NULL) && (charge != NULL) && (charge->numero > charge_src))
                {
    //             On décrémente son numéro dans le tree-view-charges
                    charge->numero--;
#ifdef ENABLE_GTK
                    gtk_tree_store_set(list_gtk_1990_actions->tree_store_charges, &charge->Iter, 0, charge->numero, -1);
#endif
                }
    //     FinPour
            }
            while (list_mvnext(action->charges) != NULL);
            
            BUGMSG(charge_data, -1, gettext("Charge %zu introuvable.\n"), charge_src);
        }
    }
    while ((list_mvnext(projet->actions) != NULL) && (charge_data == NULL));
    
    BUGMSG(charge_data, -1, gettext("Action %zu introuvable.\n"), action_src);
    
    // On cherche l'action qui contiendra la charge.
    
    list_mvfront(projet->actions);
    do
    {
        Action  *action = (Action*)list_curr(projet->actions);
    // Lorsqu'elle est trouvée, on l'insère à la fin de la liste des charges
    //   en modifiant son numéro.
        if (action->numero == action_dest)
        {
            BUGMSG(action->charges, -1, gettext("Paramètre incorrect\n"));
            
            switch (charge_data->type)
            {
                case CHARGE_NOEUD :
                {
                    charge_data->numero = list_size(action->charges);
                    list_mvrear(action->charges);
                    BUGMSG(list_insert_after(action->charges, charge_data, sizeof(Charge_Noeud)), -2, gettext("Erreur d'allocation mémoire.\n"));
                    charge_data = NULL;
                    break;
                }
                case CHARGE_BARRE_PONCTUELLE :
                {
                    Charge_Barre_Ponctuelle *charge = (Charge_Barre_Ponctuelle*)charge_data;
                    charge->numero = list_size(action->charges);
                    list_mvrear(action->charges);
                    BUGMSG(list_insert_after(action->charges, charge, sizeof(Charge_Barre_Ponctuelle)), -2, gettext("Erreur d'allocation mémoire.\n"));
                    charge_data = NULL;
                    break;
                }
                case CHARGE_BARRE_REPARTIE_UNIFORME :
                {
                    Charge_Barre_Repartie_Uniforme *charge = (Charge_Barre_Repartie_Uniforme*)charge_data;
                    charge->numero = list_size(action->charges);
                    list_mvrear(action->charges);
                    BUGMSG(list_insert_after(action->charges, charge, sizeof(Charge_Barre_Repartie_Uniforme)), -2, gettext("Erreur d'allocation mémoire.\n"));
                    charge_data = NULL;
                    break;
                }
                default :
                {
                    BUGMSG(0, -1, gettext("Type de charge %d inconnu."), charge_data->type);
                    break;
                }
            }
            return 0;
        }
    }
    while ((list_mvnext(projet->actions) != NULL) && (charge_data != NULL));
    
    BUGMSG(charge_data != NULL, -1, gettext("Action %zu introuvable.\n"), action_dest);
    
    return -1;
}


int _1990_action_supprime_charge(Projet *projet, size_t action_num, size_t charge_num)
/* Description : Supprime une charge. Décrémente également le numéro des charges afin que
 *               la liste des numéros soit toujours continue.
 * Paramètres : Projet *projet : la variable projet,
 *              size_t action_num : numéro de l'action où se situe la charge à supprimer,
 *              size_t charge_num : numéro de la charge à supprimer,
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet == NULL) ou
 *             (projet->actions == NULL) ou
 *             (list_size(projet->actions)-1 < action_num)
 *           -3 en cas d'erreur due à une fonction interne
 */
{
    Charge_Noeud            *charge_data = NULL;
#ifdef ENABLE_GTK
    List_Gtk_1990_Actions   *list_gtk_1990_actions;
#endif
    
    BUGMSG(projet, -1, gettext("Paramètre incorrect\n"));
    BUGMSG(projet->actions, -1, gettext("Paramètre incorrect\n"));
    BUGMSG(list_size(projet->actions)-1 >= action_num, -1, gettext("Paramètre incorrect : %zu\n"), action_num);
    
#ifdef ENABLE_GTK
    list_gtk_1990_actions = &projet->list_gtk._1990_actions;
#endif
    
    list_mvfront(projet->actions);
    // On cherche l'action qui contient la charge
    do
    {
        Action *action = (Action*)list_curr(projet->actions);
    // Lorsqu'elle est trouvée,
        if (action->numero == action_num)
        {
            BUGMSG(action->charges, -1, gettext("Paramètre incorrect\n"));
            BUGMSG(list_size(action->charges), -1, gettext("Charge %zu introuvable.\n"), charge_num);
            
            list_mvfront(action->charges);
    //     Pour chaque charge de l'action en cours Faire
            do
            {
                Charge_Noeud *charge = (Charge_Noeud*)list_curr(action->charges);
    //         Si la charge est celle à supprimer Alors
                if (charge->numero == charge_num)
                {
#ifdef ENABLE_GTK
    //             On la supprime du tree-view-charge
                    gtk_tree_store_remove(list_gtk_1990_actions->tree_store_charges, &charge->Iter);
#endif
    //             et de la liste des charges tout en conservant les données
    //               de la charge dans charge_data
                    charge_data = list_remove_curr(action->charges);
                    charge = (Charge_Noeud*)list_curr(action->charges);
    
    //             On libère la charge charge_data
                    switch (charge_data->type)
                    {
                        case CHARGE_NOEUD :
                        {
                            BUG(EF_charge_noeud_free(charge_data) == 0, -3);
                            break;
                        }
                        case CHARGE_BARRE_PONCTUELLE :
                        {
                            Charge_Barre_Ponctuelle *charge2 = (Charge_Barre_Ponctuelle*)charge_data;
                            BUG(EF_charge_barre_ponctuelle_free(charge2) == 0, -3);
                            break;
                        }
                        case CHARGE_BARRE_REPARTIE_UNIFORME :
                        {
                            Charge_Barre_Repartie_Uniforme *charge2 = (Charge_Barre_Repartie_Uniforme*)charge_data;
                            BUG(EF_charge_barre_repartie_uniforme_free(charge2) == 0, -3);
                            break;
                        }
                        default :
                        {
                            BUGMSG(0, -1, gettext("Type de charge %d inconnu."), charge_data->type);
                            break;
                        }
                    }
                }
    //         Sinon Si la charge possède un numéro supérieur à la charge supprimée alors
                if ((charge_data != NULL) && (charge != NULL) && (charge->numero > charge_num))
                {
    //             On décrémente son numéro dans le tree-view
                    charge->numero--;
#ifdef ENABLE_GTK
                    gtk_tree_store_set(list_gtk_1990_actions->tree_store_charges, &charge->Iter, 0, charge->numero, -1);
#endif
                }
    //     FinPour
            }
            while (list_mvnext(action->charges) != NULL);
            
            BUGMSG(charge_data, -1, gettext("Charge %zu introuvable.\n"), charge_num);
        }
    }
    while ((list_mvnext(projet->actions) != NULL) && (charge_data == NULL));
    
    BUGMSG(charge_data, -1, gettext("Action %zu introuvable.\n"), action_num);
    
    return 0;
}


int _1990_action_affiche_tout(Projet *projet)
/* Description : Affiche dans l'entrée standard les actions existantes
 * Paramètres : Projet *projet : la variable projet
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet == NULL) ou
 *             (projet->actions == NULL)
 */
{
    BUGMSG(projet, -1, gettext("Paramètre incorrect\n"));
    BUGMSG(projet->actions, -1, gettext("Paramètre incorrect\n"));
    
    // Trivial
    if (list_size(projet->actions) == 0)
        return 0;
    
    list_mvfront(projet->actions);
    do
    {
        Action      *action = (Action*)list_curr(projet->actions);
        
        printf(gettext("Action n° %zu, description '%s', type n°%d\n"), action->numero, action->description, action->type);
    }
    while (list_mvnext(projet->actions) != NULL);
    
    return 0;
}


int _1990_action_affiche_resultats(Projet *projet, int num_action)
/* Description : Affiche tous les résultats d'une action
 * Paramètres : Projet *projet : la variable projet
 *            : int num_action : numéro de l'action
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet == NULL) ou
 *             (projet->actions == NULL) ou
 *             (list_size(projet->actions) == 0) ou
 *             (projet->beton.barres == NULL)
 *           -3 en cas d'erreur due à une fonction interne
 */
{
    Action          *action_en_cours;
    unsigned int    i;
    
    BUGMSG(projet, -1, gettext("Paramètre incorrect\n"));
    BUGMSG(projet->actions, -1, gettext("Paramètre incorrect\n"));
    BUGMSG(list_size(projet->actions), -1, gettext("Paramètre incorrect\n"));
    BUG(_1990_action_cherche_numero(projet, num_action) == 0, -3);
    BUGMSG(projet->beton.barres,  -1, gettext("Paramètre incorrect\n"));
    
    // Affichage des efforts aux noeuds et des réactions d'appuis
    action_en_cours = (Action*)list_curr(projet->actions);
    printf("Effort aux noeuds & Réactions d'appuis\n");
    common_math_arrondi_sparse(action_en_cours->efforts_noeuds);
    cholmod_l_write_sparse(stdout, action_en_cours->efforts_noeuds, NULL, NULL, projet->ef_donnees.c);
    // Affichage des déplacements des noeuds
    printf("Déplacements\n");
    common_math_arrondi_sparse(action_en_cours->deplacement_complet);
    cholmod_l_write_sparse(stdout, action_en_cours->deplacement_complet, NULL, NULL, projet->ef_donnees.c);
    // Pour chaque barre
    for (i=0;i<list_size(projet->beton.barres);i++)
    {
    //     Affichage de la courbe des sollicitations vis-à-vis de l'effort normal
        printf("Barre n°%d, Effort normal\n", i);
        BUG(common_fonction_affiche(action_en_cours->fonctions_efforts[0][i]) == 0, -3);
    //     Affichage de la courbe des sollicitations vis-à-vis de l'effort tranchant selon Y
        printf("Barre n°%d, Effort Tranchant Y\n", i);
        BUG(common_fonction_affiche(action_en_cours->fonctions_efforts[1][i]) == 0, -3);
    //     Affichage de la courbe des sollicitations vis-à-vis de l'effort tranchant selon Z
        printf("Barre n°%d, Effort Tranchant Z\n", i);
        BUG(common_fonction_affiche(action_en_cours->fonctions_efforts[2][i]) == 0, -3);
    //     Affichage de la courbe des sollicitations vis-à-vis du moment de torsion
        printf("Barre n°%d, Moment de torsion\n", i);
        BUG(common_fonction_affiche(action_en_cours->fonctions_efforts[3][i]) == 0, -3);
    //     Affichage de la courbe des sollicitations vis-à-vis du moment fléchissant selon Y
        printf("Barre n°%d, Moment de flexion Y\n", i);
        BUG(common_fonction_affiche(action_en_cours->fonctions_efforts[4][i]) == 0, -3);
    //     Affichage de la courbe des sollicitations vis-à-vis du moment fléchissant selon Z
        printf("Barre n°%d, Moment de flexion Z\n", i);
        BUG(common_fonction_affiche(action_en_cours->fonctions_efforts[5][i]) == 0, -3);
    }
    for (i=0;i<list_size(projet->beton.barres);i++)
    {
    //     Affichage de la courbe de déformation selon l'axe X
        printf("Barre n°%d, Déformation en X\n", i);
        BUG(common_fonction_affiche(action_en_cours->fonctions_deformation[0][i]) == 0, -3);
    //     Affichage de la courbe de déformation selon l'axe Y
        printf("Barre n°%d, Déformation en Y\n", i);
        BUG(common_fonction_affiche(action_en_cours->fonctions_deformation[1][i]) == 0, -3);
    //     Affichage de la courbe de déformation selon l'axe Z
        printf("Barre n°%d, Déformation en Z\n", i);
        BUG(common_fonction_affiche(action_en_cours->fonctions_deformation[2][i]) == 0, -3);
    //     Affichage de la courbe de rotation selon l'axe X
        printf("Barre n°%d, Rotation en X\n", i);
        BUG(common_fonction_affiche(action_en_cours->fonctions_rotation[0][i]) == 0, -3);
    //     Affichage de la courbe de rotation selon l'axe Y
        printf("Barre n°%d, Rotation en Y\n", i);
        BUG(common_fonction_affiche(action_en_cours->fonctions_rotation[1][i]) == 0, -3);
    //     Affichage de la courbe de rotation selon l'axe Z
        printf("Barre n°%d, Rotation en Z\n", i);
        BUG(common_fonction_affiche(action_en_cours->fonctions_rotation[2][i]) == 0, -3);
    }
    // FinPour
    
    return 0;
}


int _1990_action_free_num(Projet *projet, size_t num)
/* Description : Libère l'action souhaitée et décrémente de 1 les actions dons le
 *                 numéro est supérieur.
 * Paramètres : Projet *projet : la variable projet,
 *            : size_t num : le numéro de l'action à supprimer.
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet == NULL) ou
 *             (projet->actions == NULL)
 *             (list_size(projet->actions) == NULL)
 *           -3 en cas d'erreur due à une fonction interne :
 *             action introuvable.
 */
{
    BUGMSG(projet, -1, gettext("Paramètre incorrect\n"));
    BUGMSG(projet->actions, -1, gettext("Paramètre incorrect\n"));
    BUGMSG(list_size(projet->actions), -1, gettext("Paramètre incorrect\n"));
    
    // Trivial
    BUG(_1990_action_cherche_numero(projet, num) == 0, -3);
    list_mvrear(projet->actions);
    do
    {
        Action      *action = (Action*)list_curr(projet->actions);
        
        if (action->numero == num)
        {
            action = list_remove_curr(projet->actions);
            free(action->description);
            while (!list_empty(action->charges))
            {
                Charge_Barre_Ponctuelle *charge = (Charge_Barre_Ponctuelle*)list_remove_front(action->charges);
                switch (charge->type)
                {
                    case CHARGE_NOEUD :
                    {
                        Charge_Noeud *charge2 = (Charge_Noeud *)charge;
                        BUG(EF_charge_noeud_free(charge2) == 0, -3);
                        break;
                    }
                    case CHARGE_BARRE_PONCTUELLE :
                    {
                        BUG(EF_charge_barre_ponctuelle_free(charge) == 0, -3);
                        break;
                    }
                    case CHARGE_BARRE_REPARTIE_UNIFORME :
                    {
                        Charge_Barre_Repartie_Uniforme *charge2 = (Charge_Barre_Repartie_Uniforme *)charge;
                        BUG(EF_charge_barre_repartie_uniforme_free(charge2) == 0, -3);
                        break;
                    }
                    default :
                    {
                        BUGMSG(0, -1, gettext("Type de charge %d inconnu."), charge->type);
                        break;
                    }
                }
            }
            free(action->charges);
            if (action->deplacement_complet != NULL)
                cholmod_l_free_sparse(&action->deplacement_complet, projet->ef_donnees.c);
            if (action->forces_complet != NULL)
                cholmod_l_free_sparse(&action->forces_complet, projet->ef_donnees.c);
            if (action->efforts_noeuds != NULL)
                cholmod_l_free_sparse(&action->efforts_noeuds, projet->ef_donnees.c);
            
            if (action->fonctions_efforts[0] != NULL)
                common_fonction_free(projet, action);
            free(action);
        }
        else if (action->numero > num)
            action->numero--;
    } while (list_mvprev(projet->actions) != NULL);
    
    return 0;
}


int _1990_action_free(Projet *projet)
/* Description : Libère l'ensemble des actions existantes.
 * Paramètres : Projet *projet : la variable projet
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet == NULL) ou
 *             (projet->actions == NULL)
 *           -3 en cas d'erreur due à une fonction interne
 */
{
    BUGMSG(projet, -1, gettext("Paramètre incorrect\n"));
    BUGMSG(projet->actions, -1, gettext("Paramètre incorrect\n"));
    
    // Trivial
    while (!list_empty(projet->actions))
    {
        Action      *action = (Action*)list_remove_front(projet->actions);
        
        free(action->description);
        while (!list_empty(action->charges))
        {
            Charge_Barre_Ponctuelle *charge = (Charge_Barre_Ponctuelle*)list_remove_front(action->charges);
            switch (charge->type)
            {
                case CHARGE_NOEUD :
                {
                    Charge_Noeud *charge2 = (Charge_Noeud *)charge;
                    BUG(EF_charge_noeud_free(charge2) == 0, -3);
                    break;
                }
                case CHARGE_BARRE_PONCTUELLE :
                {
                    BUG(EF_charge_barre_ponctuelle_free(charge) == 0, -3);
                    break;
                }
                case CHARGE_BARRE_REPARTIE_UNIFORME :
                {
                    Charge_Barre_Repartie_Uniforme *charge2 = (Charge_Barre_Repartie_Uniforme *)charge;
                    BUG(EF_charge_barre_repartie_uniforme_free(charge2) == 0, -3);
                    break;
                }
                default :
                {
                    BUGMSG(0, -1, gettext("Type de charge %d inconnu."), charge->type);
                    break;
                }
            }
        }
        free(action->charges);
        if (action->deplacement_complet != NULL)
            cholmod_l_free_sparse(&action->deplacement_complet, projet->ef_donnees.c);
        if (action->forces_complet != NULL)
            cholmod_l_free_sparse(&action->forces_complet, projet->ef_donnees.c);
        if (action->efforts_noeuds != NULL)
            cholmod_l_free_sparse(&action->efforts_noeuds, projet->ef_donnees.c);
        
        if (action->fonctions_efforts[0] != NULL)
        {
            BUG(common_fonction_free(projet, action) == 0, -3);
        }
        free(action);
    }
    
    free(projet->actions);
    projet->actions = NULL;
    
    return 0;
}
