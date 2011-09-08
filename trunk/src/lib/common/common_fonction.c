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
/* Description : Initialise les fonctions décrivant les sollicitations, les rotations et les
 * déplacements des barres. Cette fonction doit être appelée lorsque toutes les barres ont été
 * modélisées. En effet, il est nécessaire de connaître leur nombre afin de stocker dans un
 * tableau dynamique unique les fonctions décrivant les efforts internes, les rotations et les
 * flèches. L'initialisation des fonctions consiste à définir un nombre de tronçon à 0 et les
 * données à NULL.
 * Paramètres : Projet *projet : la variable projet
 *            : void *action_void : pointeur vers l'action. Il est utilisé void *
 *                à la place de Action* pour éviter une dépendence circulaire.
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
 * Paramètres : Fonction* fonction : la variable contenant la fonction
 *            : double coupure : position de la coupure
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
    
    // Si la coupure est égale au début du premier tronçon Alors
    //     Fin.
    if (ERREUR_RELATIVE_EGALE(fonction->troncons[0].debut_troncon, coupure))
        return 0;
    // Sinon Si la coupure est inférieure au début du premier troncon Alors
    //     Insertion d'un tronçon en première position.
    //     Initialisation de tous les coefficients à 0.
    else if (coupure < fonction->troncons[0].debut_troncon)
    {
        fonction->nb_troncons++;
        fonction->troncons = (Troncon*)realloc(fonction->troncons, fonction->nb_troncons*sizeof(Troncon));
        BUGMSG(fonction->troncons, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "common_fonction_scinde_troncon");
        for(i=fonction->nb_troncons-2;i>=0;i--)
            memcpy(&(fonction->troncons[i+1]), &(fonction->troncons[i]), sizeof(Troncon));
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
        // Sinon
        //     Pour chaque tronçon
        //         Si la coupure correspond exactement à la borne supérieure
        //             Fin.
        //         Sinon si la coupure est à l'intérieur du tronçon étudié
        //             Scindage du tronçon en deux.
        //             Attribution des mêmes coefficients pour les deux tronçons.
        //             Fin.
        //         FinSi
        //     FinPour
        // FinSi
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
        // Si la position de la coupure est au-delà à la borne supérieure du dernier tronçon
        //     Ajout d'un nouveau tronçon en dernière position.
        //     Initialisation de tous les coefficients à 0.
        //     Fin.
        // FinSi
        fonction->nb_troncons++;
        fonction->troncons = (Troncon*)realloc(fonction->troncons, fonction->nb_troncons*sizeof(Troncon));
        BUGMSG(fonction->troncons, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "common_fonction_scinde_troncon");
        fonction->troncons[fonction->nb_troncons-1].debut_troncon = fonction->troncons[fonction->nb_troncons-2].fin_troncon;
        fonction->troncons[fonction->nb_troncons-1].fin_troncon = coupure;
        fonction->troncons[fonction->nb_troncons-1].coef_0 = 0.;
        fonction->troncons[fonction->nb_troncons-1].coef_x = 0.;
        fonction->troncons[fonction->nb_troncons-1].coef_x2 = 0.;
        fonction->troncons[fonction->nb_troncons-1].coef_x3 = 0.;
        return 0;
    }
}


int common_fonction_ajout(Fonction* fonction, double debut_troncon, double fin_troncon,
  double coef_0, double coef_x, double coef_x2, double coef_x3, double translate)
/* Description : Additionne une fonction à une fonction existante dont le domaine de 
 *   validité est compris entre debut_troncon et fin_troncon.
 * Paramètres : Fonction* fonction : la variable contenant la fonction
 *            : double debut_troncon : début du tronçon de validité de la fonction
 *            : double fin_troncon : fin du tronçon de validité de la fonction
 *            : double coef_0  : coefficients de la formule coef_0 +
 *            : double coef_x  : coef_x*x +
 *            : double coef_x2 : coef_x2*x^2
 *            : double coef_x3 : coef_x3*x^3
 *            : double translate : modifie les coefficients ci-dessus afin d'effectuer une
 *                                 translation de la fonction de 0 à translate.
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (fonction == NULL) ou
 *             (fin_troncon < debut_troncon)
 *           -2 en cas d'erreur d'allocation mémoire
 *           -3 en cas d'erreur due à une fonction interne
 */
{
    double  coef_0_t, coef_x_t, coef_x2_t, coef_x3_t;
    
    BUGMSG(fonction, -1, "common_fonction_ajout\n");
    // Si fin_troncon == debut_troncon Alors
    //     Fin.
    // FinSi
    if (ERREUR_RELATIVE_EGALE(fin_troncon, debut_troncon))
        return 0;
    
    BUGMSG(fin_troncon > debut_troncon, -1, "common_fonction_ajout : debut_troncon %f > fin_troncon %f\n", debut_troncon, fin_troncon);
    
    debut_troncon = debut_troncon + translate;
    fin_troncon = fin_troncon + translate;
    
    // Détermination les coefficients à partir du résultat de f(x-translate). Ainsi, les
    //   nouveaux coefficients (indice t) deviennent :\end{verbatim}\begin{align*}
    //   &coef_{x^3t} = coef_{x^3}\nonumber\\
    //   &coef_{x^2t} = coef_{x^2} - 3*translate*coef_{x^3}\nonumber\\
    //   &coef_{xt} = 3*translate^2*coef_{x^3}-2*translate*coef_{x^2}+coef_x\nonumber\\
    //   &coef_{0t} = -translate^3*coef_{x^3}+translate^2*coef_{x^2}-translate*coef_x+coef_0\end{align*}\begin{verbatim}
    coef_x3_t = coef_x3;
    coef_x2_t = coef_x2 - 3*translate*coef_x3;
    coef_x_t = 3*translate*translate*coef_x3-2*translate*coef_x2+coef_x;
    coef_0_t = -translate*translate*translate*coef_x3+translate*translate*coef_x2-translate*coef_x+coef_0;
    
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
        fonction->troncons[0].coef_0 = coef_0_t;
        fonction->troncons[0].coef_x = coef_x_t;
        fonction->troncons[0].coef_x2 = coef_x2_t;
        fonction->troncons[0].coef_x3 = coef_x3_t;
        return 0;
    }
    // Sinon
    //     Scission de la fonction à debut_troncon. Pour rappel, si la scission existe déjà
    //       aucun nouveau tronçon n'est créé.
    //     Scission de la fonction à fin_troncon.
    //     Pour chaque tronçon
    //         Si le troncon étudié est compris entre debut_troncon et fin_troncon
    //             Addition de la fonction existante avec les nouveaux coefficients.
    //         FinSi
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


int common_fonction_compacte(Fonction* fonction)
/* Description : Fusionne dans une fonction les tronçons identiques.
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
        if ((ERREUR_RELATIVE_EGALE(fonction->troncons[i].coef_0, fonction->troncons[k].coef_0)) && (ERREUR_RELATIVE_EGALE(fonction->troncons[i].coef_x, fonction->troncons[k].coef_x)) && (ERREUR_RELATIVE_EGALE(fonction->troncons[i].coef_x2, fonction->troncons[k].coef_x2)) && (ERREUR_RELATIVE_EGALE(fonction->troncons[i].coef_x3, fonction->troncons[k].coef_x3)))
        {
            fonction->troncons[j-1].fin_troncon = fonction->troncons[i].fin_troncon;
        }
        else
        {
            j++;
            if (j-1 != i)
                memcpy(&(fonction->troncons[j-1]), &(fonction->troncons[i]), sizeof(Troncon));
            k = i;
        }
    }
    memmove(fonction->troncons, fonction->troncons, sizeof(Troncon)*j);
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
    BUGMSG(fonction, -1, "%s\n", "common_fonction_affiche");
    
    if (fonction->nb_troncons == 0)
        printf(gettext("Fonction indéfinie.\n"));
    common_fonction_compacte(fonction);
    for (i=0;i<fonction->nb_troncons;i++)
    {
        printf("debut_troncon : %f\tfin_troncon : %f\t0 : %f\tx : %f\tx2 : %f\tx3 : %f\tsoit f(%f) = %f\tf(%f) = %f\n", fonction->troncons[i].debut_troncon, fonction->troncons[i].fin_troncon, fonction->troncons[i].coef_0, fonction->troncons[i].coef_x, fonction->troncons[i].coef_x2, fonction->troncons[i].coef_x3, fonction->troncons[i].debut_troncon, fonction->troncons[i].coef_0+fonction->troncons[i].coef_x*fonction->troncons[i].debut_troncon+fonction->troncons[i].coef_x2*fonction->troncons[i].debut_troncon*fonction->troncons[i].debut_troncon+fonction->troncons[i].coef_x3*fonction->troncons[i].debut_troncon*fonction->troncons[i].debut_troncon*fonction->troncons[i].debut_troncon, fonction->troncons[i].fin_troncon, fonction->troncons[i].coef_0+fonction->troncons[i].coef_x*fonction->troncons[i].fin_troncon+fonction->troncons[i].coef_x2*fonction->troncons[i].fin_troncon*fonction->troncons[i].fin_troncon+fonction->troncons[i].coef_x3*fonction->troncons[i].fin_troncon*fonction->troncons[i].fin_troncon*fonction->troncons[i].fin_troncon);
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
