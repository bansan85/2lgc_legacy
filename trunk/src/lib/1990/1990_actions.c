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

#include "1990_actions.h"
#include "1990_coef_psi.h"
#include "common_projet.h"
#include "common_maths.h"
#include "common_erreurs.h"
#include "common_fonction.h"
#include "EF_noeud.h"
#include "EF_charge_noeud.h"
#include "EF_charge_barre_ponctuelle.h"


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
        default : { BUGMSG(0, NULL, gettext("%s : Catégorie %d inconnue.\n"), "_1990_action_type_bat_txt_eu", type); break; }
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
        default : { BUGMSG(0, NULL, gettext("%s : Catégorie %d inconnue.\n"), "_1990_action_type_bat_txt_fr", type); break; }
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
        default : { BUGMSG(0, NULL, gettext("%s : Pays %d inconnu.\n"), "_1990_action_type_bat_txt", pays); break; }
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
        BUGMSG(0, ACTION_INCONNUE, gettext("%s : Catégorie %d inconnue.\n"), "_1990_action_categorie_bat_eu", type);
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
        BUGMSG(0, ACTION_INCONNUE, gettext("%s : Catégorie %d inconnue.\n"), "_1990_action_categorie_bat_fr", type);
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
        default : { BUGMSG(0, ACTION_INCONNUE, gettext("%s : Pays %d inconnu.\n"), "_1990_action_categorie_bat", pays); break; }
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
    BUGMSG(projet, -1, "%s\n", "_1990_action_init");
    
    projet->actions = list_init();
    BUGMSG(projet->actions, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "_1990_action_init");
    
    return 0;
}


int _1990_action_ajout(Projet *projet, int type)
/* Description : ajoute une nouvelle action à la liste des actions en lui attribuant le numéro
 *                 suivant le dernier relachement existant.
 * Paramètres : Projet *projet : la variable projet
 *            : int type : le type de l'action
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet == NULL) ou
 *             (projet->actions == NULL) ou
 *             _1990_action_categorie_bat(categorie, projet->pays) < 0 ou
 *             _1990_coef_psi0_bat(categorie, projet->pays) < 0 ou
 *             _1990_coef_psi1_bat(categorie, projet->pays) < 0 ou
 *             _1990_coef_psi2_bat(categorie, projet->pays) < 0
 *           -2 en cas d'erreur d'allocation mémoire
 */
{
    // Trivial
    Action      *action_dernier, action_nouveau;
    
    BUGMSG(projet, -1, "%s\n", "_1990_action_ajout");
    BUGMSG(projet->actions, -1, "%s\n", "_1990_action_ajout");
    BUG(_1990_action_categorie_bat(type, projet->pays) != ACTION_INCONNUE, -1);
    
    list_mvrear(projet->actions);
    action_nouveau.nom = (char*)malloc(sizeof(char)*(strlen(_1990_action_type_bat_txt(type, projet->pays))+1));
    BUGMSG(action_nouveau.nom, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "_1990_action_ajout");
    strcpy(action_nouveau.nom, _1990_action_type_bat_txt(type, projet->pays));
    action_nouveau.type = type;
    action_nouveau.charges = list_init();
    BUGMSG(action_nouveau.charges, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "_1990_action_ajout");
    action_nouveau.deplacement_complet = NULL;
    action_nouveau.forces_complet = NULL;
    action_nouveau.efforts_noeuds = NULL;
    action_nouveau.flags = 0;
    action_nouveau.psi0 = _1990_coef_psi0_bat(type, projet->pays);
    BUGMSG(action_nouveau.psi0 >= 0, -1, "type : %d, pays : %d\n", type, projet->pays);
    action_nouveau.psi1 = _1990_coef_psi1_bat(type, projet->pays);
    BUGMSG(action_nouveau.psi1 >= 0, -1, "type : %d, pays : %d\n", type, projet->pays);
    action_nouveau.psi2 = _1990_coef_psi2_bat(type, projet->pays);
    BUGMSG(action_nouveau.psi2 >= 0, -1, "type : %d, pays : %d\n", type, projet->pays);
    action_nouveau.pIter = NULL;
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
    
    BUGMSG(list_insert_after(projet->actions, &(action_nouveau), sizeof(action_nouveau)), -2, gettext("%s : Erreur d'allocation mémoire.\n"), "_1990_action_ajout");
    
    return 0;
}


int _1990_action_cherche_numero(Projet *projet, int numero)
/* Description : Cherche et marque l'action numero comme celle en cours.
 * Paramètres : Projet *projet : la variable projet
 *            : int numero : le numéro de l'action
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet == NULL) ou
 *             (projet->actions == NULL) ou
 *             (list_size(projet->actions) == 0) ou
 *             action introuvable
 */
{
    BUGMSG(projet, -1, "_1990_action_cherche_numero\n");
    BUGMSG(projet->actions, -1, "_1990_action_cherche_numero\n");
    BUGMSG(list_size(projet->actions), -1, "_1990_action_cherche_numero\n");
    
    // Trivial
    list_mvfront(projet->actions);
    do
    {
        Action      *action = (Action*)list_curr(projet->actions);
        
        if (action->numero == numero)
            return 0;
    }
    while (list_mvnext(projet->actions) != NULL);
    BUGMSG(0, -2, gettext("%s : Action %d introuvable.\n"), "_1990_action_cherche_numero", numero);
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
    BUGMSG(projet, -1, "_1990_action_affiche_tout\n");
    BUGMSG(projet->actions, -1, "_1990_action_affiche_tout\n");
    
    // Trivial
    
    if (list_size(projet->actions) == 0)
        return 0;
    
    list_mvfront(projet->actions);
    do
    {
        Action      *action = (Action*)list_curr(projet->actions);
        
        printf("Action '%s', numéro %d, nom '%s', type n°%d\n", action->nom, action->numero, action->nom, action->type);
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
 *             (_1990_action_cherche_numero(projet, num_action) != 0) ou
 *             (projet->beton.barres == NULL)
 *           -3 en cas d'erreur due à une fonction interne
 */
{
    Action          *action_en_cours;
    unsigned int    i;
    
    BUGMSG(projet, -1, "EF_calculs_affiche_resultats\n");
    BUGMSG(projet->actions, -1, "EF_calculs_affiche_resultats\n");
    BUGMSG(list_size(projet->actions), -1, "EF_calculs_affiche_resultats\n");
    BUGMSG(_1990_action_cherche_numero(projet, num_action) == 0, -1, "EF_calculs_affiche_resultats : num_action %d\n", num_action);
    BUGMSG(projet->beton.barres,  -1, "EF_calculs_affiche_resultats\n");
    
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


int _1990_action_free(Projet *projet)
/* Description : Libère l'ensemble des actions existantes.
 * Paramètres : Projet *projet : la variable projet
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet == NULL) ou
 *             (projet->actions == NULL)
 */
{
    BUGMSG(projet, -1, "_1990_action_free\n");
    BUGMSG(projet->actions, -1, "_1990_action_free\n");
    
    // Trivial
    while (!list_empty(projet->actions))
    {
        Action      *action = (Action*)list_remove_front(projet->actions);
        
        free(action->nom);
        while (!list_empty(action->charges))
        {
            Charge_Barre_Ponctuelle *charge = (Charge_Barre_Ponctuelle*)list_remove_front(action->charges);
            free(charge->nom);
            free(charge);
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
    
    free(projet->actions);
    projet->actions = NULL;
    
    return 0;
}
