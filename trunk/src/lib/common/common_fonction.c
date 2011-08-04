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

#include <stdio.h>
#include <stdlib.h>
#include <libintl.h>
#include <locale.h>
#include <list.h>
#include <string.h>
#include "common_projet.h"
#include "common_fonction.h"
#include "common_erreurs.h"
#include "common_maths.h"
#include "1990_actions.h"

/* common_fonction_init
 * Description : Initialise les fonctions des sollicitations des barres. Cette fonction doit être appelée lorsque toutes les barres ont été initialisées.
 * Paramètres : Projet *projet : la variable projet
 *            : void *action_void : pointeur vers l'action. Il est utilisé void * à la place de Action* pour éviter une dépendence circulaire.
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : valeur négative
 */
int common_fonction_init(Projet *projet, void *action_void)
{
    Action *action = action_void;
    unsigned int        i, j;
    
    if ((projet == NULL) || (action_void == NULL))
        BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
    
    for (i=0;i<6;i++)
    {
        action->fonctions_efforts[i] = (Fonction**)malloc(sizeof(Fonction*)*list_size(projet->beton.elements));
        if (action->fonctions_efforts[i] == NULL)
            BUGTEXTE(-2, gettext("Erreur d'allocation mémoire.\n"));
        for (j=0;j<list_size(projet->beton.elements);j++)
        {
            action->fonctions_efforts[i][j] = (Fonction*)malloc(sizeof(Fonction));
            if (action->fonctions_efforts[i][j] == NULL)
                BUGTEXTE(-2, gettext("Erreur d'allocation mémoire.\n"));
            action->fonctions_efforts[i][j]->nb_troncons = 0;
            action->fonctions_efforts[i][j]->troncons = NULL;
        }
    }
    
    for (i=0;i<3;i++)
    {
        action->fonctions_fleche[i] = (Fonction**)malloc(sizeof(Fonction*)*list_size(projet->beton.elements));
        if (action->fonctions_fleche[i] == NULL)
            BUGTEXTE(-2, gettext("Erreur d'allocation mémoire.\n"));
        for (j=0;j<list_size(projet->beton.elements);j++)
        {
            action->fonctions_fleche[i][j] = (Fonction*)malloc(sizeof(Fonction));
            if (action->fonctions_fleche[i][j] == NULL)
                BUGTEXTE(-2, gettext("Erreur d'allocation mémoire.\n"));
            action->fonctions_fleche[i][j]->nb_troncons = 0;
            action->fonctions_fleche[i][j]->troncons = NULL;
        }
        
        action->fonctions_rotation[i] = (Fonction**)malloc(sizeof(Fonction*)*list_size(projet->beton.elements));
        if (action->fonctions_rotation[i] == NULL)
            BUGTEXTE(-2, gettext("Erreur d'allocation mémoire.\n"));
        for (j=0;j<list_size(projet->beton.elements);j++)
        {
            action->fonctions_rotation[i][j] = (Fonction*)malloc(sizeof(Fonction));
            if (action->fonctions_rotation[i][j] == NULL)
                BUGTEXTE(-2, gettext("Erreur d'allocation mémoire.\n"));
            action->fonctions_rotation[i][j]->nb_troncons = 0;
            action->fonctions_rotation[i][j]->troncons = NULL;
        }
    }
    
    return 0;
}


/* common_fonction_ajout_troncon
 * Description : Coupe un tronçon en deux à la position fin_troncon
 * Paramètres : Fonction* fonction : la variable contenant la fonction
 *            : unsigned int num_troncon : numero du troncon
 *            : double fin_troncon : fin du tronçon de validité de la fonction
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : valeur négative
 */
int common_fonction_ajout_troncon(Fonction* fonction, double coupure)
{
    int i, j;
    
    if ((fonction == NULL) || (fonction->nb_troncons == 0))
        BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
    
    // On commence par vérifier si la coupure existe déjà
    if (ERREUR_RELATIVE_EGALE(fonction->troncons[0].debut_troncon, coupure))
        return 0;
    else if (coupure < fonction->troncons[0].debut_troncon)
    {
        fonction->nb_troncons++;
        fonction->troncons = (Troncon*)realloc(fonction->troncons, fonction->nb_troncons*sizeof(Troncon));
        if (fonction->troncons == NULL)
            BUGTEXTE(-2, gettext("Erreur d'allocation mémoire.\n"));
        for(i=fonction->nb_troncons-2;i>=0;i--)
            memcpy(&(fonction->troncons[i+1]), &(fonction->troncons[i]), sizeof(Troncon));
        // Comme on ne connait pas ce qui se situe dans l'intervalle (agrandissement du domaine de l'équation)
        // on initialise tout à 0.
        fonction->troncons[0].debut_troncon = coupure;
        fonction->troncons[0].fin_troncon = fonction->troncons[1].debut_troncon;
        fonction->troncons[0].coef_0 = 0.;
        fonction->troncons[0].coef_x = 0.;
        fonction->troncons[0].coef_x2 = 0.;
        fonction->troncons[0].coef_x3 = 0.;
        return 0;
    }
    else
    {
        for (i=0;i<fonction->nb_troncons;i++)
        {
            // On vérifie si la borne supérieure du troncon est égale à la coupure
            if (ERREUR_RELATIVE_EGALE(fonction->troncons[i].fin_troncon, coupure))
                return 0;
            else if (fonction->troncons[i].fin_troncon > coupure)
            // On ajoute le nouveau troncon
            {
                fonction->nb_troncons++;
                fonction->troncons = (Troncon*)realloc(fonction->troncons, fonction->nb_troncons*sizeof(Troncon));
                if (fonction->troncons == NULL)
                    BUGTEXTE(-2, gettext("Erreur d'allocation mémoire.\n"));
                // On décale les données d'un cran
                for(j=fonction->nb_troncons-2;j>=i;j--)
                    memcpy(&(fonction->troncons[j+1]), &(fonction->troncons[j]), sizeof(Troncon));
                // Et on réajuste l'intervalle
                fonction->troncons[i+1].debut_troncon = coupure;
                fonction->troncons[i].fin_troncon = coupure;
                return 0;
            }
        }
        // Si on arrive ici, c'est que la coupure est supérieure au domaine de validité de la formule
        // On vérifie quand même...
        if (fonction->troncons[fonction->nb_troncons-1].fin_troncon > coupure)
            BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
        fonction->nb_troncons++;
        fonction->troncons = (Troncon*)realloc(fonction->troncons, fonction->nb_troncons*sizeof(Troncon));
        if (fonction->troncons == NULL)
            BUGTEXTE(-2, gettext("Erreur d'allocation mémoire.\n"));
        fonction->troncons[fonction->nb_troncons-1].debut_troncon = fonction->troncons[fonction->nb_troncons-2].fin_troncon;
        fonction->troncons[fonction->nb_troncons-1].fin_troncon = coupure;
        fonction->troncons[fonction->nb_troncons-1].coef_0 = 0.;
        fonction->troncons[fonction->nb_troncons-1].coef_x = 0.;
        fonction->troncons[fonction->nb_troncons-1].coef_x2 = 0.;
        fonction->troncons[fonction->nb_troncons-1].coef_x3 = 0.;
        return 0;
    }
}


/* common_fonction_ajout
 * Description : Ajoute une fonction à une fonction existante (nb_troncons peut être égal à 0 et fonctions à NULL).
 * Paramètres : Fonction* fonction : la variable contenant la fonction
 *            : double debut_troncon : début du tronçon de validité de la fonction
 *            : double fin_troncon : fin du tronçon de validité de la fonction
 *            : double coef_0  :
 *            : double coef_x  :
 *            : double coef_x2 :
 *            : double coef_x3 : coefficients de la formule coef_0 + coef_x*x + coef_x2*x*x + coef_x3*x*x*x
 *            : int translate : modifie les coefficients ci-dessus afin d'effectuer une translation de la fonction de 0 à debut_troncon.
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : valeur négative
 */
int common_fonction_ajout(Fonction* fonction, double debut_troncon, double fin_troncon, double coef_0, double coef_x, double coef_x2, double coef_x3, double translate)
{
    double  coef_0_t, coef_x_t, coef_x2_t, coef_x3_t;
    
    if (ERREUR_RELATIVE_EGALE(fin_troncon, debut_troncon))
        return 0;
    
    if ((fonction == NULL) || (fin_troncon < debut_troncon))
        BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
    
    // On réajuste les coefficients en calculant f(x-translate)
    coef_x3_t = coef_x3;
    coef_x2_t = coef_x2 - 3*translate*coef_x3;
    coef_x_t = 3*translate*translate*coef_x3-2*translate*coef_x2+coef_x;
    coef_0_t = -translate*translate*translate*coef_x3+translate*translate*coef_x2-translate*coef_x+coef_0;
    
    if (fonction->nb_troncons == 0)
    {
        fonction->nb_troncons = 1;
        fonction->troncons = (Troncon *)malloc(sizeof(Troncon));
        if (fonction->troncons == NULL)
            BUGTEXTE(-2, gettext("Erreur d'allocation mémoire.\n"));
        fonction->troncons[0].debut_troncon = debut_troncon;
        fonction->troncons[0].fin_troncon = fin_troncon;
        fonction->troncons[0].coef_0 = coef_0_t;
        fonction->troncons[0].coef_x = coef_x_t;
        fonction->troncons[0].coef_x2 = coef_x2_t;
        fonction->troncons[0].coef_x3 = coef_x3_t;
        return 0;
    }
    else
    {
        int i = 0;
        // La fonction common_fonction_ajout_troncon s'occupera de vérifier si le troncon est déjà existant et le créera le cas echéant.
        common_fonction_ajout_troncon(fonction, debut_troncon);
        common_fonction_ajout_troncon(fonction, fin_troncon);
        while ((i<fonction->nb_troncons))
        {
            if (ERREUR_RELATIVE_EGALE(fonction->troncons[i].debut_troncon, fin_troncon))
                return 0;
            else if ((ERREUR_RELATIVE_EGALE(fonction->troncons[i].debut_troncon, debut_troncon)) || (fonction->troncons[i].debut_troncon > debut_troncon))
            {
                // Toute la fonction rentre dans le tronçon
                fonction->troncons[i].coef_0 += coef_0_t;
                fonction->troncons[i].coef_x += coef_x_t;
                fonction->troncons[i].coef_x2 += coef_x2_t;
                fonction->troncons[i].coef_x3 += coef_x3_t;
            }
            i++;
        }
        return 0;
    }
}


/* common_fonction_affiche
 * Description : Affiche les différents tronçon de la fonction
 * Paramètres : Fonction* fonction : fonction à afficher
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : valeur négative
 */
int common_fonction_affiche(Fonction* fonction)
{
    int i;
    
    if (fonction == NULL)
        BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
    
    if (fonction->nb_troncons == 0)
        printf(gettext("Fonction indéfinie.\n"));
    for (i=0;i<fonction->nb_troncons;i++)
    {
        printf("debut_troncon : %f, fin_troncon : %f, coef_0 : %f, coef_x : %f, coef_x2 : %f, coef_x3 : %f soit f(%f) = %f et f(%f) = %f\n", fonction->troncons[i].debut_troncon, fonction->troncons[i].fin_troncon, fonction->troncons[i].coef_0, fonction->troncons[i].coef_x, fonction->troncons[i].coef_x2, fonction->troncons[i].coef_x3, fonction->troncons[i].debut_troncon, fonction->troncons[i].coef_0+fonction->troncons[i].coef_x*fonction->troncons[i].debut_troncon+fonction->troncons[i].coef_x2*fonction->troncons[i].debut_troncon*fonction->troncons[i].debut_troncon+fonction->troncons[i].coef_x3*fonction->troncons[i].debut_troncon*fonction->troncons[i].debut_troncon*fonction->troncons[i].debut_troncon, fonction->troncons[i].fin_troncon, fonction->troncons[i].coef_0+fonction->troncons[i].coef_x*fonction->troncons[i].fin_troncon+fonction->troncons[i].coef_x2*fonction->troncons[i].fin_troncon*fonction->troncons[i].fin_troncon+fonction->troncons[i].coef_x3*fonction->troncons[i].fin_troncon*fonction->troncons[i].fin_troncon*fonction->troncons[i].fin_troncon);
    }
    return 0;
}


/* common_fonction_free
 * Description : Libère les fonctions des barres.
 * Paramètres : Projet *projet : la variable projet
 *            : void *action_void : pointeur vers l'action. Il est utilisé void * à la place de Action* pour éviter une dépendence circulaire.
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : valeur négative
 */
int common_fonction_free(Projet *projet, void *action_void)
{
    Action *action = action_void;
    unsigned int        i, j;
    
    if ((projet == NULL) || (action == NULL))
        BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
    
    for (i=0;i<6;i++)
    {
        if (action->fonctions_efforts[i] != NULL)
        {
            for (j=0;j<list_size(projet->beton.elements);j++)
            {
                free(action->fonctions_efforts[i][j]->troncons);
                free(action->fonctions_efforts[i][j]);
            }
            free(action->fonctions_efforts[i]);
            action->fonctions_efforts[i] = NULL;
        }
    }
    
    for (i=0;i<3;i++)
    {
        if (action->fonctions_fleche[i] != NULL)
        {
            for (j=0;j<list_size(projet->beton.elements);j++)
            {
                free(action->fonctions_fleche[i][j]->troncons);
                free(action->fonctions_fleche[i][j]);
            }
            free(action->fonctions_fleche[i]);
            action->fonctions_fleche[i] = NULL;
        }
        
        if (action->fonctions_rotation[i] != NULL)
        {
            for (j=0;j<list_size(projet->beton.elements);j++)
            {
                free(action->fonctions_rotation[i][j]->troncons);
                free(action->fonctions_rotation[i][j]);
            }
            free(action->fonctions_rotation[i]);
            action->fonctions_rotation[i] = NULL;
        }
    }
    
    return 0;
}
