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

int common_fonction_init(Projet *projet, void *action_void)
/* Description : Initialise les fonctions décrivant les sollicitations, les rotations ou les
 *               déplacements des barres. Cette fonction doit être appelée lorsque toutes les
 *               barres ont été modélisées. En effet, il est nécessaire de connaître leur
 *               nombre afin de stocker dans un tableau dynamique unique les fonctions.
 *               L'initialisation des fonctions consiste à définir un nombre de tronçon à 0 et
 *               les données à NULL.
 * Paramètres : Projet *projet : la variable projet,
 *            : void *action_void : pointeur vers l'action. Il est utilisé void à la place de
 *              Action* pour éviter une dépendence circulaire.
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet == NULL) ou
 *             (action_void == NULL)
 *           -2 en cas d'erreur d'allocation mémoire
 */
{
    Action              *action = action_void;
    unsigned int        i, j;
    
    BUGMSG(projet, -1, "common_fonction_init\n");
    BUGMSG(action_void, -1, "common_fonction_init\n");
    
    // Trivial
    for (i=0;i<6;i++)
    {
        action->fonctions_efforts[i] = (Fonction**)malloc(sizeof(Fonction*)*list_size(projet->beton.barres));
        BUGMSG(action->fonctions_efforts[i], -2, gettext("%s : Erreur d'allocation mémoire.\n"), "common_fonction_init");
        for (j=0;j<list_size(projet->beton.barres);j++)
        {
            action->fonctions_efforts[i][j] = (Fonction*)malloc(sizeof(Fonction));
            BUGMSG(action->fonctions_efforts[i][j], -2, gettext("%s : Erreur d'allocation mémoire.\n"), "common_fonction_init");
            action->fonctions_efforts[i][j]->nb_troncons = 0;
            action->fonctions_efforts[i][j]->troncons = NULL;
        }
    }
    
    for (i=0;i<3;i++)
    {
        action->fonctions_deformation[i] = (Fonction**)malloc(sizeof(Fonction*)*list_size(projet->beton.barres));
        BUGMSG(action->fonctions_deformation[i], -2, gettext("%s : Erreur d'allocation mémoire.\n"), "common_fonction_init");
        for (j=0;j<list_size(projet->beton.barres);j++)
        {
            action->fonctions_deformation[i][j] = (Fonction*)malloc(sizeof(Fonction));
            BUGMSG(action->fonctions_deformation[i][j], -2, gettext("%s : Erreur d'allocation mémoire.\n"), "common_fonction_init");
            action->fonctions_deformation[i][j]->nb_troncons = 0;
            action->fonctions_deformation[i][j]->troncons = NULL;
        }
        
        action->fonctions_rotation[i] = (Fonction**)malloc(sizeof(Fonction*)*list_size(projet->beton.barres));
        BUGMSG(action->fonctions_rotation[i], -2, gettext("%s : Erreur d'allocation mémoire.\n"), "common_fonction_init");
        for (j=0;j<list_size(projet->beton.barres);j++)
        {
            action->fonctions_rotation[i][j] = (Fonction*)malloc(sizeof(Fonction));
            BUGMSG(action->fonctions_rotation[i][j], -2, gettext("%s : Erreur d'allocation mémoire.\n"), "common_fonction_init");
            action->fonctions_rotation[i][j]->nb_troncons = 0;
            action->fonctions_rotation[i][j]->troncons = NULL;
        }
    }
    
    return 0;
}


int common_fonction_scinde_troncon(Fonction* fonction, double coupure)
/* Description : Divise un tronçon en deux à la position coupure.
 *               Si la coupure est en dehors de la borne de validité actuelle de la fonction,
 *               les bornes s'en trouvent modifiées.
 *               Si la coupure correspond déjà à une jonction entre deux tronçons, la
 *               fonction ne fait rien et renvoie 0.
 * Paramètres : Fonction* fonction : la variable contenant la fonction,
 *            : double coupure : position de la coupure.
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (fonction == NULL) ou
 *             (fonction->nb_troncons == 0)
 *           -2 en cas d'erreur d'allocation mémoire
 */
{
    int i, j;
    
    BUGMSG(fonction, -1, "common_fonction_scinde_troncon\n");
    BUGMSG(fonction->nb_troncons, -1, "common_fonction_scinde_troncon\n");
    
    // Trivial
    /* Si la coupure est égale au début du premier tronçon Alors
     *     Fin. */
    if (ERREUR_RELATIVE_EGALE(fonction->troncons[0].debut_troncon, coupure))
        return 0;
    /* Sinon Si la coupure est inférieure au début du premier troncon Alors
     *     Insertion d'un tronçon en première position.
     *     Initialisation de tous les coefficients à 0.*/
    else if (coupure < fonction->troncons[0].debut_troncon)
    {
        fonction->nb_troncons++;
        fonction->troncons = (Troncon*)realloc(fonction->troncons, fonction->nb_troncons*sizeof(Troncon));
        BUGMSG(fonction->troncons, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "common_fonction_scinde_troncon");
        for(i=fonction->nb_troncons-2;i>=0;i--)
            memcpy(&(fonction->troncons[i+1]), &(fonction->troncons[i]), sizeof(Troncon));
        fonction->troncons[0].debut_troncon = coupure;
        fonction->troncons[0].fin_troncon = fonction->troncons[1].debut_troncon;
        fonction->troncons[0].x0 = 0.;
        fonction->troncons[0].x1 = 0.;
        fonction->troncons[0].x2 = 0.;
        fonction->troncons[0].x3 = 0.;
        fonction->troncons[0].x4 = 0.;
        fonction->troncons[0].x5 = 0.;
        fonction->troncons[0].x6 = 0.;
        return 0;
    }
    else
    {
    /* Sinon
     *     Pour chaque tronçon
     *         Si la coupure correspond exactement à la borne supérieure
     *             Fin.
     *         Sinon si la coupure est à l'intérieur du tronçon étudié
     *             Scindage du tronçon en deux.
     *             Attribution des mêmes coefficients pour les deux tronçons.
     *             Fin.
     *         FinSi
     *     FinPour
     * FinSi */
        for (i=0;i<fonction->nb_troncons;i++)
        {
            if (ERREUR_RELATIVE_EGALE(fonction->troncons[i].fin_troncon, coupure))
                return 0;
            else if (fonction->troncons[i].fin_troncon > coupure)
            {
                fonction->nb_troncons++;
                fonction->troncons = (Troncon*)realloc(fonction->troncons, fonction->nb_troncons*sizeof(Troncon));
                BUGMSG(fonction->troncons, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "common_fonction_scinde_troncon");
                for(j=fonction->nb_troncons-2;j>=i;j--)
                    memcpy(&(fonction->troncons[j+1]), &(fonction->troncons[j]), sizeof(Troncon));
                fonction->troncons[i+1].debut_troncon = coupure;
                fonction->troncons[i].fin_troncon = coupure;
                return 0;
            }
        }
    /* Si la position de la coupure est au-delà à la borne supérieure du dernier tronçon
     *     Ajout d'un nouveau tronçon en dernière position.
     *     Initialisation de tous les coefficients à 0.
     *     Fin.
     * FinSi */
        fonction->nb_troncons++;
        fonction->troncons = (Troncon*)realloc(fonction->troncons, fonction->nb_troncons*sizeof(Troncon));
        BUGMSG(fonction->troncons, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "common_fonction_scinde_troncon");
        fonction->troncons[fonction->nb_troncons-1].debut_troncon = fonction->troncons[fonction->nb_troncons-2].fin_troncon;
        fonction->troncons[fonction->nb_troncons-1].fin_troncon = coupure;
        fonction->troncons[fonction->nb_troncons-1].x0 = 0.;
        fonction->troncons[fonction->nb_troncons-1].x1 = 0.;
        fonction->troncons[fonction->nb_troncons-1].x2 = 0.;
        fonction->troncons[fonction->nb_troncons-1].x3 = 0.;
        fonction->troncons[fonction->nb_troncons-1].x4 = 0.;
        fonction->troncons[fonction->nb_troncons-1].x5 = 0.;
        fonction->troncons[fonction->nb_troncons-1].x6 = 0.;
        return 0;
    }
}


int common_fonction_ajout(Fonction* fonction, double debut_troncon, double fin_troncon,
  double x0, double x1, double x2, double x3, double x4, double x5, double x6, double t)
/* Description : Additionne une fonction à une fonction existante dont le domaine de validité
 *               est compris entre debut_troncon et fin_troncon.
 *               Si la fonction ne possède pas un tronçon commençant à debut_troncon ou un
 *               tronçon finissant à fin_troncon, une scission sera effectuée par la fonction
 *               common_fonction_scinde_troncon.
 *               Si le domaine de validité du nouveau tronçon est totalement en dehors du
 *               domaine de validité de l'actuelle fonction, un tronçon intermédiaire sera
 *               créé et possèdera une fonction f(x)=0.
 * Paramètres : Fonction* fonction : la variable contenant la fonction
 *            : double debut_troncon : début du tronçon de validité de la fonction
 *            : double fin_troncon : fin du tronçon de validité de la fonction
 *            : double x0 : coefficients de la formule x0 +
 *            : double x1 : x1*x +
 *            : double x2 : x2*x^2 +
 *            : double x3 : x3*x^3 +
 *            : double x4 : x4*x^4 +
 *            : double x5 : x5*x^5 +
 *            : double x6 : x6*x^6
 *            : double t : modifie les coefficients ci-dessus afin d'effectuer une
 *                                 translation de la fonction de 0 à t.
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (fonction == NULL) ou
 *             (fin_troncon < debut_troncon)
 *           -2 en cas d'erreur d'allocation mémoire
 *           -3 en cas d'erreur due à une fonction interne
 */
{
    double  x0_t, x1_t, x2_t, x3_t, x4_t, x5_t, x6_t;
    
    BUGMSG(fonction, -1, "common_fonction_ajout\n");
    // Si fin_troncon == debut_troncon Alors
    //     Fin.
    // FinSi
    if (ERREUR_RELATIVE_EGALE(fin_troncon, debut_troncon))
        return 0;
    
    BUGMSG(fin_troncon > debut_troncon, -1, "common_fonction_ajout : debut_troncon %.20f > fin_troncon %.20f\n", debut_troncon, fin_troncon);
    
    debut_troncon = debut_troncon + t;
    fin_troncon = fin_troncon + t;
    
    // Détermination les nouveaux coefficients à partir du résultat de f(x-t).
    x6_t = x6;
    x5_t = -6.*x6*t+x5;
    x4_t = 15.*x6*t*t-5.*x5*t+x4;
    x3_t = -20.*x6*t*t*t+10.*x5*t*t-4.*x4*t+x3;
    x2_t = 15.*x6*t*t*t*t-10.*x5*t*t*t+6.*x4*t*t-3.*x3*t+x2;
    x1_t = -6.*x6*t*t*t*t*t+5.*x5*t*t*t*t-4.*x4*t*t*t+3.*x3*t*t-2.*x2*t+x1;
    x0_t = x6*t*t*t*t*t*t-x5*t*t*t*t*t+x4*t*t*t*t-x3*t*t*t+x2*t*t-x1*t+x0;
    
    // Si aucun troncon n'est présent (fonction vide) Alors
    //     Création d'un tronçon avec pour borne debut_troncon .. fin_troncon.
    //     Attribution des coefficients de la fonction.
    if (fonction->nb_troncons == 0)
    {
        fonction->nb_troncons = 1;
        fonction->troncons = (Troncon *)malloc(sizeof(Troncon));
        BUGMSG(fonction->troncons, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "common_fonction_ajout");
        fonction->troncons[0].debut_troncon = debut_troncon;
        fonction->troncons[0].fin_troncon = fin_troncon;
        fonction->troncons[0].x0 = x0_t;
        fonction->troncons[0].x1 = x1_t;
        fonction->troncons[0].x2 = x2_t;
        fonction->troncons[0].x3 = x3_t;
        fonction->troncons[0].x4 = x4_t;
        fonction->troncons[0].x5 = x5_t;
        fonction->troncons[0].x6 = x6_t;
        return 0;
    }
    // Sinon
    //     Scission de la fonction à debut_troncon. Pour rappel, si la scission existe déjà
    //       aucun nouveau tronçon n'est créé.
    //     Scission de la fonction à fin_troncon.
    //     Pour chaque tronçon compris entre debut_troncon et fin_troncon Faire
    //         Addition de la fonction existante avec les nouveaux coefficients.
    //     FinPour
    // FinSi
    else
    {
        int i = 0;
        
        BUG(common_fonction_scinde_troncon(fonction, debut_troncon) == 0, -3);
        BUG(common_fonction_scinde_troncon(fonction, fin_troncon) == 0, -3);
        while ((i<fonction->nb_troncons))
        {
            if (ERREUR_RELATIVE_EGALE(fonction->troncons[i].debut_troncon, fin_troncon))
                return 0;
            else if ((ERREUR_RELATIVE_EGALE(fonction->troncons[i].debut_troncon, debut_troncon)) || (fonction->troncons[i].debut_troncon > debut_troncon))
            {
                fonction->troncons[i].x0 += x0_t;
                fonction->troncons[i].x1 += x1_t;
                fonction->troncons[i].x2 += x2_t;
                fonction->troncons[i].x3 += x3_t;
                fonction->troncons[i].x4 += x4_t;
                fonction->troncons[i].x5 += x5_t;
                fonction->troncons[i].x6 += x6_t;
            }
            i++;
        }
        return 0;
    }
}


int common_fonction_compacte(Fonction* fonction)
/* Description : Fusionne les tronçons voisins ayant une fonction identique.
 * Paramètres : Fonction* fonction : fonction à afficher
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (fonction == NULL)
 *           -2 en cas d'erreur d'allocation mémoire
 */
{
    int i; /* Numéro du tronçon en cours */
    int j; /* Nombre de tronçons à conserver */
    int k; /* Numéro du précédent tronçon identique */
    
    // Trivial
    BUGMSG(fonction, -1, "%s\n", "common_fonction_compacte");
    
    if ((fonction->nb_troncons == 0) || (fonction->nb_troncons == 1))
        return 0;
    j = 1;
    k = 0;
    for (i=1;i<fonction->nb_troncons;i++)
    {
        if ((ERREUR_RELATIVE_EGALE(fonction->troncons[i].x0, fonction->troncons[k].x0)) && (ERREUR_RELATIVE_EGALE(fonction->troncons[i].x1, fonction->troncons[k].x1)) && (ERREUR_RELATIVE_EGALE(fonction->troncons[i].x2, fonction->troncons[k].x2)) && (ERREUR_RELATIVE_EGALE(fonction->troncons[i].x3, fonction->troncons[k].x3)) && (ERREUR_RELATIVE_EGALE(fonction->troncons[i].x4, fonction->troncons[k].x4)) && (ERREUR_RELATIVE_EGALE(fonction->troncons[i].x5, fonction->troncons[k].x5)) && (ERREUR_RELATIVE_EGALE(fonction->troncons[i].x6, fonction->troncons[k].x6)))
            fonction->troncons[j-1].fin_troncon = fonction->troncons[i].fin_troncon;
        else
        {
            j++;
            if (j-1 != i)
                memcpy(&(fonction->troncons[j-1]), &(fonction->troncons[i]), sizeof(Troncon));
            k = i;
        }
    }
    memmove(fonction->troncons, fonction->troncons, sizeof(Troncon)*j);
    BUGMSG(fonction->troncons, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "common_fonction_compacte");
    fonction->nb_troncons = j;
    return 0;
}


int common_fonction_affiche(Fonction* fonction)
/* Description : Affiche la fonction (coefficients pour chaque tronçon) ainsi que la valeur
 *                 de la fonction pour chaque extrémité du tronçon.
 * Paramètres : Fonction* fonction : fonction à afficher
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (fonction == NULL)
 */
{
    int i;
    
    // Trivial
    BUGMSG(fonction, -1, "common_fonction_affiche\n");
    
    if (fonction->nb_troncons == 0)
        printf(gettext("Fonction indéfinie.\n"));
    common_fonction_compacte(fonction);
    for (i=0;i<fonction->nb_troncons;i++)
    {
        printf("debut_troncon : %.5f\tfin_troncon : %.5f\t0 : %.20f\tx : %.20f\tx2 : %.20f\tx3 : %.20f\tx4 : %.20f\tx5 : %.20f\tx6 : %.20f\tsoit f(%.5f) = %.10f\tf(%.5f) = %.10f\n",
          fonction->troncons[i].debut_troncon,
          fonction->troncons[i].fin_troncon,
          fonction->troncons[i].x0,
          fonction->troncons[i].x1,
          fonction->troncons[i].x2,
          fonction->troncons[i].x3,
          fonction->troncons[i].x4,
          fonction->troncons[i].x5,
          fonction->troncons[i].x6,
          fonction->troncons[i].debut_troncon,
          fonction->troncons[i].x0+fonction->troncons[i].x1*fonction->troncons[i].debut_troncon+fonction->troncons[i].x2*fonction->troncons[i].debut_troncon*fonction->troncons[i].debut_troncon+fonction->troncons[i].x3*fonction->troncons[i].debut_troncon*fonction->troncons[i].debut_troncon*fonction->troncons[i].debut_troncon+fonction->troncons[i].x4*fonction->troncons[i].debut_troncon*fonction->troncons[i].debut_troncon*fonction->troncons[i].debut_troncon*fonction->troncons[i].debut_troncon+fonction->troncons[i].x5*fonction->troncons[i].debut_troncon*fonction->troncons[i].debut_troncon*fonction->troncons[i].debut_troncon*fonction->troncons[i].debut_troncon*fonction->troncons[i].debut_troncon+fonction->troncons[i].x6*fonction->troncons[i].debut_troncon*fonction->troncons[i].debut_troncon*fonction->troncons[i].debut_troncon*fonction->troncons[i].debut_troncon*fonction->troncons[i].debut_troncon*fonction->troncons[i].debut_troncon,
          fonction->troncons[i].fin_troncon,
          fonction->troncons[i].x0+fonction->troncons[i].x1*fonction->troncons[i].fin_troncon+fonction->troncons[i].x2*fonction->troncons[i].fin_troncon*fonction->troncons[i].fin_troncon+fonction->troncons[i].x3*fonction->troncons[i].fin_troncon*fonction->troncons[i].fin_troncon*fonction->troncons[i].fin_troncon+fonction->troncons[i].x4*fonction->troncons[i].fin_troncon*fonction->troncons[i].fin_troncon*fonction->troncons[i].fin_troncon*fonction->troncons[i].fin_troncon+fonction->troncons[i].x5*fonction->troncons[i].fin_troncon*fonction->troncons[i].fin_troncon*fonction->troncons[i].fin_troncon*fonction->troncons[i].fin_troncon*fonction->troncons[i].fin_troncon+fonction->troncons[i].x6*fonction->troncons[i].fin_troncon*fonction->troncons[i].fin_troncon*fonction->troncons[i].fin_troncon*fonction->troncons[i].fin_troncon*fonction->troncons[i].fin_troncon*fonction->troncons[i].fin_troncon);
    }
    return 0;
}


int common_fonction_free(Projet *projet, void *action_void)
/* Description : Libère les fonctions de toutes les barres de l'action souhaitée.
 * Paramètres : Projet *projet : la variable projet
 *            : void *action_void : pointeur vers l'action. Il est utilisé void * à la place
 *                                  de Action* pour éviter une dépendence circulaire.
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet == NULL) ou
 *             (action == NULL)
 */
{
    Action *action = action_void;
    unsigned int        i, j;
    
    BUGMSG(projet, -1, "common_fonction_free\n");
    BUGMSG(action, -1, "common_fonction_free\n");
    BUGMSG(projet->beton.barres, -1, "common_fonction_free\n");
    
    // Trivial
    for (i=0;i<6;i++)
    {
        if (action->fonctions_efforts[i] != NULL)
        {
            for (j=0;j<list_size(projet->beton.barres);j++)
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
        if (action->fonctions_deformation[i] != NULL)
        {
            for (j=0;j<list_size(projet->beton.barres);j++)
            {
                free(action->fonctions_deformation[i][j]->troncons);
                free(action->fonctions_deformation[i][j]);
            }
            free(action->fonctions_deformation[i]);
            action->fonctions_deformation[i] = NULL;
        }
        
        if (action->fonctions_rotation[i] != NULL)
        {
            for (j=0;j<list_size(projet->beton.barres);j++)
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
