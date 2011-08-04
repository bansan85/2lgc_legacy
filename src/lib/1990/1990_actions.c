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

#include "1990_actions.h"
#include "1990_coef_psi.h"
#include "common_projet.h"
#include "common_erreurs.h"
#include "EF_noeud.h"
#include "common_fonction.h"


/* _1990_action_categorie_bat_txt_eu
 * Description : renvoie la description des catérogies des durées indicative de la norme européenne
 * Paramètres : int type : catégorie de durée d'utilisation de projet
 * Valeur renvoyée :
 *   Succès : le texte correspondant
 *   Échec : NULL si la catégorie n'existe pas
 */
char *_1990_action_categorie_bat_txt_eu(int type)
{
    switch(type)
    {
        case 0 : { return gettext("Permanente"); break; }
        case 1 : { return gettext("Précontrainte"); break; }
        case 2 : { return gettext("Exploitation : Catégorie A : habitation, zones résidentielles"); break; }
        case 3 : { return gettext("Exploitation : Catégorie B : bureaux"); break; }
        case 4 : { return gettext("Exploitation : Catégorie C : lieux de réunion"); break; }
        case 5 : { return gettext("Exploitation : Catégorie D : commerces"); break; }
        case 6 : { return gettext("Exploitation : Catégorie E : stockage"); break; }
        case 7 : { return gettext("Exploitation : Catégorie F : zone de trafic, véhicules de poids inférieur à 30 kN"); break; }
        case 8 : { return gettext("Exploitation : Catégorie G : zone de trafic, véhicules de poids entre 30 kN et 160 kN"); break; }
        case 9 : { return gettext("Exploitation : Catégorie H : toits"); break; }
        case 10 : { return gettext("Neige : Finlande, Islande, Norvège, Suède"); break; }
        case 11 : { return gettext("Neige : Autres états membres CEN, altitude > 1000 m"); break; }
        case 12 : { return gettext("Neige : Autres états membres CEN, altitude <= 1000 m"); break; }
        case 13 : { return gettext("Vent"); break; }
        case 14 : { return gettext("Température (hors incendie)"); break; }
        case 15 : { return gettext("Accidentelle"); break; }
        case 16 : { return gettext("Sismique"); break; }
        default : { BUGTEXTE(NULL, gettext("Catégorie inconnue.\n")); break; }
    }
}

/* _1990_action_categorie_bat_txt_fr
 * Description : renvoie la description des catérogies des durées indicative de la norme européenne
 * Paramètres : int type : catégorie de durée d'utilisation de projet
 * Valeur renvoyée :
 *   Succès : le texte correspondant
 *   Échec : NULL si la catégorie n'existe pas
 */
char *_1990_action_categorie_bat_txt_fr(int type)
{
    switch(type)
    {
        case 0 : { return gettext("Permanente"); break; }
        case 1 : { return gettext("Précontrainte"); break; }
        case 2 : { return gettext("Exploitation : Catégorie A : habitation, zones résidentielles"); break; }
        case 3 : { return gettext("Exploitation : Catégorie B : bureaux"); break; }
        case 4 : { return gettext("Exploitation : Catégorie C : lieux de réunion"); break; }
        case 5 : { return gettext("Exploitation : Catégorie D : commerces"); break; }
        case 6 : { return gettext("Exploitation : Catégorie E : stockage"); break; }
        case 7 : { return gettext("Exploitation : Catégorie F : zone de trafic, véhicules de poids inférieur à 30 kN"); break; }
        case 8 : { return gettext("Exploitation : Catégorie G : zone de trafic, véhicules de poids entre 30 kN et 160 kN"); break; }
        case 9 : { return gettext("Exploitation : Catégorie H : toits d'un bâtiment de catégorie A ou B"); break; }
        case 10 : { return gettext("Exploitation : Catégorie I : toitures accessibles avec locaux des catégories A ou B"); break; }
        case 11 : { return gettext("Exploitation : Catégorie I : toitures accessibles avec locaux des catégories C ou D"); break; }
        case 12 : { return gettext("Exploitation : Catégorie K : Hélicoptère sur la toiture"); break; }
        case 13 : { return gettext("Exploitation : Catégorie K : Hélicoptère sur la toiture, autres charges (fret, personnel, accessoires ou équipements divers)"); break; }
        case 14 : { return gettext("Neige : Saint-Pierre-et-Miquelon"); break; }
        case 15 : { return gettext("Neige : Altitude > 1000 m"); break; }
        case 16 : { return gettext("Neige : Altitude <= 1000 m"); break; }
        case 17 : { return gettext("Vent"); break; }
        case 18 : { return gettext("Température (hors incendie)"); break; }
        case 19 : { return gettext("Accidentelle"); break; }
        case 20 : { return gettext("Sismique"); break; }
        case 21 : { return gettext("Eaux souterraines"); break; }
        default : { BUGTEXTE(NULL, gettext("Catégorie inconnue.\n")); break; }
    }
}

/* _1990_action_categorie_bat_txt
 * Description : renvoie la description des catérogies des durées indicative en fonction du pays
 * Paramètres : int type : catégorie de durée d'utilisation de projet
 *            : Type_Pays pays : le numéro du pays
 * Valeur renvoyée :
 *   Succès : le texte correspondant
 *   Échec : NULL si la catégorie n'existe pas
 */
char *_1990_action_categorie_bat_txt(int type, Type_Pays pays)
{
    switch (pays)
    {
        case PAYS_EU : { return _1990_action_categorie_bat_txt_eu(type); break; }
        case PAYS_FR : { return _1990_action_categorie_bat_txt_fr(type); break; }
        default : { BUGTEXTE(NULL, gettext("Pays inconnu.\n")); break; }
    }
}

/* _1990_action_type_combinaison_bat_eu
 * Description : renvoie le type d'une action pour les combinaisons d'action des bâtiments selon la norme européenne
 * Paramètres : int categorie : categorie de l'action
 * Valeur renvoyée :
 *   Succès : 0 : Poids propre
 *            1 : Précontrainte
 *            2 : Action variable
 *            3 : Action accidentelle
 *            4 : Action sismique
 *   Échec : -1 si la catégorie n'existe pas
 */
int _1990_action_type_combinaison_bat_eu(int categorie)
{
    if (categorie == 0) // Poids propre
        return 0;
    else if (categorie == 1) // Charges d'exploitation
        return 1;
    else if ((2 <= categorie) && (categorie <= 14))
        return 2;
    else if (categorie == 15) // accidentelle
        return 3;
    else if (categorie == 16) // sismique
        return 4;
    else
        BUGTEXTE(-1, gettext("Catégorie inconnue.\n"));
}

/* _1990_action_type_combinaison_bat_fr
 * Description : renvoie le type d'une action pour les combinaisons d'action des bâtiments selon la norme française
 * Paramètres : int categorie : categorie de l'action
 * Valeur renvoyée :
 *   Succès : 0 : Poids propre
 *            1 : Précontrainte
 *            2 : Action variable
 *            3 : Action accidentelle
 *            4 : Action sismique
 *            5 : Action due aux eaux souterraines
 *   Échec : -1 si la catégorie n'existe pas
 */
int _1990_action_type_combinaison_bat_fr(int categorie)
{
    if (categorie == 0) // Poids propre
        return 0;
    else if (categorie == 1) // Précontrainte
        return 1;
    else if ((2 <= categorie) && (categorie <= 18)) // Charges d'exploitation
        return 2;
    else if (categorie == 19) // Accidentelle
        return 3;
    else if (categorie == 20) // Sismique
        return 4;
    else if (categorie == 21) // Eaux souterraines
        return 5;
    else
        BUGTEXTE(-1, gettext("Catégorie inconnue.\n"));
}

/* _1990_action_categorie_bat
 * Description : renvoie le type d'une action pour les combinaisons d'action des bâtiments en fonction du pays
 * Paramètres : int type : catégorie de l'action
 *            : Type_Pays pays : le numéro du pays
 * Valeur renvoyée :
 *   Succès : cf les fonctions _1990_action_categorie_bat_PAYS
 *   Échec : -1 si le pays n'existe pas
 */
int _1990_action_type_combinaison_bat(int categorie, Type_Pays pays)
{
    switch (pays)
    {
        case PAYS_EU : { return _1990_action_type_combinaison_bat_eu(categorie); break; }
        case PAYS_FR : { return _1990_action_type_combinaison_bat_fr(categorie); break; }
        default : { BUGTEXTE(-1, gettext("Pays inconnu.\n")); break; }
    }
}

/* _1990_action_init
 * Description : Initialise la liste des actions
 * Paramètres : Projet *projet : la variable projet
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : valeur négative
 */
int _1990_action_init(Projet *projet)
{
    if (projet == NULL)
        BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
    
    projet->actions = list_init();
    if (projet->actions == NULL)
        BUGTEXTE(-2, gettext("Erreur d'allocation mémoire.\n"));
    else
        return 0;
}

/* _1990_action_ajout
 * Description : ajouter une nouvelle action à la liste des actions
 * Paramètres : Projet *projet : la variable projet
 *            : int categorie : la catégorie de l'action
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : valeur négative
 */
int _1990_action_ajout(Projet *projet, int categorie)
{
    Action      *action_dernier, action_nouveau;
    
    if ((projet == NULL) || (projet->actions == NULL) || (_1990_action_type_combinaison_bat(categorie, projet->pays) < 0))
        BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
    
    list_mvrear(projet->actions);
    action_nouveau.nom = NULL;
    action_nouveau.description = NULL;
    action_nouveau.categorie = categorie;
    action_nouveau.charges = list_init();
    if (action_nouveau.charges == NULL)
        BUGTEXTE(-2, gettext("Erreur d'allocation mémoire.\n"));
    action_nouveau.deplacement_partiel = NULL;
    action_nouveau.deplacement_complet = NULL;
    action_nouveau.forces_complet = NULL;
    action_nouveau.efforts_noeuds = NULL;
    action_nouveau.flags = 0;
    action_nouveau.psi0 = _1990_coef_psi0_bat(categorie, projet->pays);
    action_nouveau.psi1 = _1990_coef_psi1_bat(categorie, projet->pays);
    action_nouveau.psi2 = _1990_coef_psi2_bat(categorie, projet->pays);
    action_nouveau.fonctions_efforts[0] = NULL;
    action_nouveau.fonctions_efforts[1] = NULL;
    action_nouveau.fonctions_efforts[2] = NULL;
    action_nouveau.fonctions_efforts[3] = NULL;
    action_nouveau.fonctions_efforts[4] = NULL;
    action_nouveau.fonctions_efforts[5] = NULL;
    action_nouveau.fonctions_fleche[0] = NULL;
    action_nouveau.fonctions_fleche[1] = NULL;
    action_nouveau.fonctions_fleche[2] = NULL;
    action_nouveau.fonctions_rotation[0] = NULL;
    action_nouveau.fonctions_rotation[1] = NULL;
    action_nouveau.fonctions_rotation[2] = NULL;
    
    action_dernier = (Action *)list_rear(projet->actions);
    if (action_dernier == NULL)
        action_nouveau.numero = 0;
    else
        action_nouveau.numero = action_dernier->numero+1;
    
    if (list_insert_after(projet->actions, &(action_nouveau), sizeof(action_nouveau)) == NULL)
        BUGTEXTE(-2, gettext("Erreur d'allocation mémoire.\n"));
    
    return 0;
}

/* _1990_action_cherche_numero
 * Description : Cherche et marque l'action n°numero comme celle en cours
 *             : l'action correspondant au numéro doit obligatoirement est existant
 * Paramètres : Projet *projet : la variable projet
 *            : int numero : le numéro de l'action dans projet->actions
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : valeur négative
 */
int _1990_action_cherche_numero(Projet *projet, int numero)
{
    if ((projet == NULL) || (projet->actions == NULL) || (list_size(projet->actions) == 0))
        BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
    
    list_mvfront(projet->actions);
    do
    {
        Action      *action = list_curr(projet->actions);
        
        if (action->numero == numero)
            return 0;
    }
    while (list_mvnext(projet->actions) != NULL);
    BUGTEXTE(-2, gettext("Action %d introuvable.\n"), numero);
}

/* _1990_action_affiche_tout
 * Description : Affiche dans l'entrée standard les actions existantes
 * Paramètres : Projet *projet : la variable projet
 * Valeur renvoyée :
 *   Succès : 0 même si aucune action n'est existante
 *   Échec : valeur négative si la liste des actions n'est pas initialisée
 */
int _1990_action_affiche_tout(Projet *projet)
{
    if ((projet == NULL) || (projet->actions == NULL))
        BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
    if (list_size(projet->actions) == 0)
        return 0;
    
    list_mvfront(projet->actions);
    do
    {
        Action      *action = list_curr(projet->actions);
        
        printf("Action '%s', numéro %d, description '%s', catégorie n°%d\n", action->nom, action->numero, action->description, action->categorie);
    }
    while (list_mvnext(projet->actions) != NULL);
    
    return 0;
}

/* _1990_action_ajout_charge_ponctuelle_noeud
 * Description : ajouter une charge ponctuelle à une action et à un noeud de la structure
 * Paramètres : Projet *projet : la variable projet
 *            : int num_action : numero de l'action qui contiendra la charge
 *            : EF_Noeud *action : pointeur vers le noeud qui supportera la charge
 *            : double fx : force suivant l'axe global x
 *            : double fy : force suivant l'axe global y
 *            : double fz : force suivant l'axe global z
 *            : double rx : moment autour de l'axe global x
 *            : double ry : moment autour de l'axe global y
 *            : double rz : moment autour de l'axe global z
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : valeur négative
 */
int _1990_action_ajout_charge_ponctuelle_noeud(Projet *projet, int num_action, EF_Noeud *noeud, double fx, double fy, double fz, double rx, double ry, double rz)
{
    Action          *action_en_cours;
    Charge_Ponctuelle_Noeud *charge_dernier, charge_nouveau;
    
    if ((projet == NULL) || (projet->actions == NULL) || (list_size(projet->actions) == 0) || (noeud == NULL))
        BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
    
       if (_1990_action_cherche_numero(projet, num_action) != 0)
               BUGTEXTE(-2, gettext("Paramètres invalides.\n"));
       action_en_cours = list_curr(projet->actions);
    
    charge_nouveau.type = CHARGE_PONCTUELLE_NOEUD;
    charge_nouveau.nom = NULL;
    charge_nouveau.description = NULL;
    charge_nouveau.noeud = noeud;
    charge_nouveau.x = fx;
    charge_nouveau.y = fy;
    charge_nouveau.z = fz;
    charge_nouveau.rx = rx;
    charge_nouveau.ry = ry;
    charge_nouveau.rz = rz;
    
    charge_dernier = (Charge_Ponctuelle_Noeud *)list_rear(action_en_cours->charges);
    if (charge_dernier == NULL)
        charge_nouveau.numero = 0;
    else
        charge_nouveau.numero = charge_dernier->numero+1;
    
    list_mvrear(action_en_cours->charges);
    if (list_insert_after(action_en_cours->charges, &(charge_nouveau), sizeof(charge_nouveau)) == NULL)
        BUGTEXTE(-2, gettext("Erreur d'allocation mémoire.\n"));
    
    return 0;
}


/* _1990_action_ajout_charge_ponctuelle_barre
 * Description : ajouter une charge ponctuelle à une action et à l'intérieur d'une barre de la structure
 * Paramètres : Projet *projet : la variable projet
 *            : int num_action : numero de l'action qui contiendra la charge
 *            : void *barre : barre qui supportera la charge
 *            : int repere_local : TRUE si les charges doivent être prise dans le repère local, FALSE pour le repère global
 *            : double position : position en mètre de la charge par rapport au début de la barre
 *            : double fx : force suivant l'axe x
 *            : double fy : force suivant l'axe y
 *            : double fz : force suivant l'axe z
 *            : double rx : moment autour de l'axe x
 *            : double ry : moment autour de l'axe y
 *            : double rz : moment autour de l'axe z
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : valeur négative
 */
int _1990_action_ajout_charge_ponctuelle_barre(Projet *projet, int num_action, void *barre, int repere_local, double position, double fx, double fy, double fz, double rx, double ry, double rz)
{
    Action          *action_en_cours;
    Charge_Ponctuelle_Barre *charge_dernier, charge_nouveau;
    
    if ((projet == NULL) || (projet->actions == NULL) || (list_size(projet->actions) == 0) || (barre == NULL))
        BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
    
       if (_1990_action_cherche_numero(projet, num_action) != 0)
               BUGTEXTE(-2, gettext("Paramètres invalides.\n"));
       action_en_cours = list_curr(projet->actions);
    
    charge_nouveau.type = CHARGE_PONCTUELLE_BARRE;
    charge_nouveau.nom = NULL;
    charge_nouveau.description = NULL;
    charge_nouveau.barre = barre;
    charge_nouveau.repere_local = repere_local;
    charge_nouveau.position = position;
    charge_nouveau.x = fx;
    charge_nouveau.y = fy;
    charge_nouveau.z = fz;
    charge_nouveau.rx = rx;
    charge_nouveau.ry = ry;
    charge_nouveau.rz = rz;
    
    charge_dernier = (Charge_Ponctuelle_Barre *)list_rear(action_en_cours->charges);
    if (charge_dernier == NULL)
        charge_nouveau.numero = 0;
    else
        charge_nouveau.numero = charge_dernier->numero+1;
    
    list_mvrear(action_en_cours->charges);
    if (list_insert_after(action_en_cours->charges, &(charge_nouveau), sizeof(charge_nouveau)) == NULL)
        BUGTEXTE(-2, gettext("Erreur d'allocation mémoire.\n"));
    
    return 0;
}


/* _1990_action_free
 * Description : Libère l'ensemble des actions existantes
 * Paramètres : Projet *projet : la variable projet
 * Valeur renvoyée :
 *   Succès : 0 même si aucune action n'est existante
 *   Échec : valeur négative si la liste des actions n'est pas initialisée ou a déjà été libérée
 */
int _1990_action_free(Projet *projet)
{
    if ((projet == NULL) || (projet->actions == NULL))
        BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
    
    while (!list_empty(projet->actions))
    {
        Action      *action = list_remove_front(projet->actions);
        
        if (action->nom != NULL)
            free(action->nom);
        if (action->description != NULL)
            free(action->description);
        while (!list_empty(action->charges))
        {
            Charge_Ponctuelle_Noeud *charge = list_remove_front(action->charges);
            if (charge->nom != NULL)
                free(charge->nom);
            if (charge->description != NULL)
                free(charge->description);
            free(charge);
        }
        free(action->charges);
        if (action->deplacement_partiel != NULL)
            cholmod_l_free_sparse(&action->deplacement_partiel, projet->ef_donnees.c);
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
    
    free(projet->actions);
    projet->actions = NULL;
    
    return 0;
}
