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
#include <string.h>
#include <gmodule.h>
#include <math.h>

#include "common_projet.h"
#include "common_erreurs.h"
#include "common_math.h"
#include "common_fonction.h"

G_MODULE_EXPORT gboolean common_fonction_init(Projet *projet, Action *action)
/* Description : Initialise les fonctions décrivant les sollicitations, les rotations ou les
 *               déplacements des barres. Cette fonction doit être appelée lorsque toutes les
 *               barres ont été modélisées. En effet, il est nécessaire de connaître leur
 *               nombre afin de stocker dans un tableau dynamique unique les fonctions.
 *               L'initialisation des fonctions consiste à définir un nombre de tronçon à 0 et
 *               les données à NULL.
 * Paramètres : Projet *projet : la variable projet,
 *            : Action *action : pointeur vers l'action.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL,
 *             action == NULL,
 *             en cas d'erreur d'allocation mémoire.
 */
{
    unsigned int        i, j;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(action, FALSE, gettext("Paramètre %s incorrect.\n"), "action");
    
    // Trivial
    for (i=0;i<6;i++)
    {
        BUGMSG(action->fonctions_efforts[i] = (Fonction**)malloc(sizeof(Fonction*)*g_list_length(projet->modele.barres)), FALSE, gettext("Erreur d'allocation mémoire.\n"));
        for (j=0;j<g_list_length(projet->modele.barres);j++)
        {
            BUGMSG(action->fonctions_efforts[i][j] = (Fonction*)malloc(sizeof(Fonction)), FALSE, gettext("Erreur d'allocation mémoire.\n"));
            action->fonctions_efforts[i][j]->nb_troncons = 0;
            action->fonctions_efforts[i][j]->troncons = NULL;
        }
    }
    
    for (i=0;i<3;i++)
    {
        BUGMSG(action->fonctions_deformation[i] = (Fonction**)malloc(sizeof(Fonction*)*g_list_length(projet->modele.barres)), FALSE, gettext("Erreur d'allocation mémoire.\n"));
        for (j=0;j<g_list_length(projet->modele.barres);j++)
        {
            BUGMSG(action->fonctions_deformation[i][j] = (Fonction*)malloc(sizeof(Fonction)), FALSE, gettext("Erreur d'allocation mémoire.\n"));
            action->fonctions_deformation[i][j]->nb_troncons = 0;
            action->fonctions_deformation[i][j]->troncons = NULL;
        }
        
        BUGMSG(action->fonctions_rotation[i] = (Fonction**)malloc(sizeof(Fonction*)*g_list_length(projet->modele.barres)), FALSE, gettext("Erreur d'allocation mémoire.\n"));
        for (j=0;j<g_list_length(projet->modele.barres);j++)
        {
            BUGMSG(action->fonctions_rotation[i][j] = (Fonction*)malloc(sizeof(Fonction)), FALSE, gettext("Erreur d'allocation mémoire.\n"));
            action->fonctions_rotation[i][j]->nb_troncons = 0;
            action->fonctions_rotation[i][j]->troncons = NULL;
        }
    }
    
    return TRUE;
}


gboolean common_fonction_scinde_troncon(Fonction* fonction, double coupure)
/* Description : Divise un tronçon en deux à la position coupure.
 *               Si la coupure est en dehors de la borne de validité actuelle de la fonction,
 *               les bornes s'en trouvent modifiées.
 *               Si la coupure correspond déjà à une jonction entre deux tronçons, la
 *               fonction ne fait rien et renvoie 0.
 * Paramètres : Fonction* fonction : la variable contenant la fonction,
 *            : double coupure : position de la coupure.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             fonction == NULL,
 *             fonction->nb_troncons == 0,
 *             en cas d'erreur d'allocation mémoire.
 */
{
    unsigned int i, j;
    
    BUGMSG(fonction, FALSE, gettext("Paramètre %s incorrect.\n"), "fonction");
    BUGMSG(fonction->nb_troncons, FALSE, gettext("Impossible de scinder une fonction vide\n"));
    
    // Trivial
    /* Si la coupure est égale au début du premier tronçon Alors
     *     Fin. */
    if (ERREUR_RELATIVE_EGALE(fonction->troncons[0].debut_troncon, coupure))
        return TRUE;
    /* Sinon Si la coupure est inférieure au début du premier troncon Alors
     *     Insertion d'un tronçon en première position.
     *     Initialisation de tous les coefficients à 0.*/
    else if (coupure < fonction->troncons[0].debut_troncon)
    {
        fonction->nb_troncons++;
        fonction->troncons = (Troncon*)realloc(fonction->troncons, fonction->nb_troncons*sizeof(Troncon));
        BUGMSG(fonction->troncons, FALSE, gettext("Erreur d'allocation mémoire.\n"));
        for(i=fonction->nb_troncons-1;i>0;i--)
            memcpy(&(fonction->troncons[i]), &(fonction->troncons[i-1]), sizeof(Troncon));
        fonction->troncons[0].debut_troncon = coupure;
        fonction->troncons[0].fin_troncon = fonction->troncons[1].debut_troncon;
        fonction->troncons[0].x0 = 0.;
        fonction->troncons[0].x1 = 0.;
        fonction->troncons[0].x2 = 0.;
        fonction->troncons[0].x3 = 0.;
        fonction->troncons[0].x4 = 0.;
        fonction->troncons[0].x5 = 0.;
        fonction->troncons[0].x6 = 0.;
        return TRUE;
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
                return TRUE;
            else if (fonction->troncons[i].fin_troncon > coupure)
            {
                fonction->nb_troncons++;
                BUGMSG(fonction->troncons = (Troncon*)realloc(fonction->troncons, fonction->nb_troncons*sizeof(Troncon)), FALSE, gettext("Erreur d'allocation mémoire.\n"));
                for (j=fonction->nb_troncons-1;j>i;j--)
                    memcpy(&(fonction->troncons[j]), &(fonction->troncons[j-1]), sizeof(Troncon));
                fonction->troncons[i+1].debut_troncon = coupure;
                fonction->troncons[i].fin_troncon = coupure;
                return TRUE;
            }
        }
    /* Si la position de la coupure est au-delà à la borne supérieure du dernier tronçon
     *     Ajout d'un nouveau tronçon en dernière position.
     *     Initialisation de tous les coefficients à 0.
     *     Fin.
     * FinSi */
        fonction->nb_troncons++;
        fonction->troncons = (Troncon*)realloc(fonction->troncons, fonction->nb_troncons*sizeof(Troncon));
        BUGMSG(fonction->troncons, FALSE, gettext("Erreur d'allocation mémoire.\n"));
        fonction->troncons[fonction->nb_troncons-1].debut_troncon = fonction->troncons[fonction->nb_troncons-2].fin_troncon;
        fonction->troncons[fonction->nb_troncons-1].fin_troncon = coupure;
        fonction->troncons[fonction->nb_troncons-1].x0 = 0.;
        fonction->troncons[fonction->nb_troncons-1].x1 = 0.;
        fonction->troncons[fonction->nb_troncons-1].x2 = 0.;
        fonction->troncons[fonction->nb_troncons-1].x3 = 0.;
        fonction->troncons[fonction->nb_troncons-1].x4 = 0.;
        fonction->troncons[fonction->nb_troncons-1].x5 = 0.;
        fonction->troncons[fonction->nb_troncons-1].x6 = 0.;
        return TRUE;
    }
}


G_MODULE_EXPORT gboolean common_fonction_ajout_poly(Fonction* fonction, double debut_troncon,
  double fin_troncon, double x0, double x1, double x2, double x3, double x4, double x5,
  double x6, double t)
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
 *   Succès : TRUE
 *   Échec : FALSE :
 *             fonction == NULL,
 *             fin_troncon < debut_troncon,
 *             en cas d'erreur d'allocation mémoire,
 *             en cas d'erreur due à une fonction interne.
 */
{
    double  x0_t, x1_t, x2_t, x3_t, x4_t, x5_t, x6_t;
    
    BUGMSG(fonction, FALSE, gettext("Paramètre %s incorrect.\n"), "fonction");
    // Si fin_troncon == debut_troncon Alors
    //     Fin.
    // FinSi
    if (ERREUR_RELATIVE_EGALE(fin_troncon, debut_troncon))
        return TRUE;
    
    BUGMSG(fin_troncon > debut_troncon, FALSE, "Le début du tronçon (%.20f) est supérieur à la fin (%.20f).\n", debut_troncon, fin_troncon);
    
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
        BUGMSG(fonction->troncons = (Troncon *)malloc(sizeof(Troncon)), FALSE, gettext("Erreur d'allocation mémoire.\n"));
        fonction->troncons[0].debut_troncon = debut_troncon;
        fonction->troncons[0].fin_troncon = fin_troncon;
        fonction->troncons[0].x0 = x0_t;
        fonction->troncons[0].x1 = x1_t;
        fonction->troncons[0].x2 = x2_t;
        fonction->troncons[0].x3 = x3_t;
        fonction->troncons[0].x4 = x4_t;
        fonction->troncons[0].x5 = x5_t;
        fonction->troncons[0].x6 = x6_t;
        return TRUE;
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
        unsigned int i = 0;
        
        BUG(common_fonction_scinde_troncon(fonction, debut_troncon), FALSE);
        BUG(common_fonction_scinde_troncon(fonction, fin_troncon), FALSE);
        while ((i<fonction->nb_troncons))
        {
            if (ERREUR_RELATIVE_EGALE(fonction->troncons[i].debut_troncon, fin_troncon))
                return TRUE;
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
        return TRUE;
    }
}


gboolean common_fonction_ajout_fonction(Fonction* fonction, Fonction *fonction_a_ajouter,
  double multi)
/* Description : Additionne une fonction à une fonction existante.
 * Paramètres : Fonction* fonction : fonction à modifier,
 *              Fonction *fonction_a_ajouter : fonction à ajouter,
 *              double multi : coefficient multiplicateur de la fonction à ajouter.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             fonction == NULL,
 *             fonction_a_ajouter == NULL,
 *             en cas d'erreur d'allocation mémoire.
 */
{
    unsigned int    i;
    BUGMSG(fonction, FALSE, gettext("Paramètre %s incorrect.\n"), "fonction");
    BUGMSG(fonction_a_ajouter, FALSE, gettext("Paramètre %s incorrect.\n"), "fonction_a_ajouter");
    
    for (i=0;i<fonction_a_ajouter->nb_troncons;i++)
        BUG(common_fonction_ajout_poly(
          fonction,
          fonction_a_ajouter->troncons[i].debut_troncon,
          fonction_a_ajouter->troncons[i].fin_troncon,
          multi*fonction_a_ajouter->troncons[i].x0,
          multi*fonction_a_ajouter->troncons[i].x1,
          multi*fonction_a_ajouter->troncons[i].x2,
          multi*fonction_a_ajouter->troncons[i].x3,
          multi*fonction_a_ajouter->troncons[i].x4,
          multi*fonction_a_ajouter->troncons[i].x5,
          multi*fonction_a_ajouter->troncons[i].x6,
          0.), FALSE);
    
    return TRUE;
}


gboolean common_fonction_compacte(Fonction* fonction, Fonction *index)
/* Description : Fusionne les tronçons voisins ayant une fonction identique.
 * Paramètres : Fonction* fonction : fonction à afficher,
 *              Fonction *index : index de la fonction fonction. Peut être NULL.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             fonction == NULL,
 *             en cas d'erreur d'allocation mémoire.
 */
{
    unsigned int i; /* Numéro du tronçon en cours */
    unsigned int j; /* Nombre de tronçons à conserver */
    unsigned int k; /* Numéro du précédent tronçon identique */
    
    // Trivial
    BUGMSG(fonction, FALSE, gettext("Paramètre %s incorrect.\n"), "fonction");
    
    if ((fonction->nb_troncons == 0) || (fonction->nb_troncons == 1))
        return TRUE;
    j = 1;
    k = 0;
    for (i=1;i<fonction->nb_troncons;i++)
    {
        if ((ERREUR_RELATIVE_EGALE(fonction->troncons[i].x0, fonction->troncons[k].x0)) && (ERREUR_RELATIVE_EGALE(fonction->troncons[i].x1, fonction->troncons[k].x1)) && (ERREUR_RELATIVE_EGALE(fonction->troncons[i].x2, fonction->troncons[k].x2)) && (ERREUR_RELATIVE_EGALE(fonction->troncons[i].x3, fonction->troncons[k].x3)) && (ERREUR_RELATIVE_EGALE(fonction->troncons[i].x4, fonction->troncons[k].x4)) && (ERREUR_RELATIVE_EGALE(fonction->troncons[i].x5, fonction->troncons[k].x5)) && (ERREUR_RELATIVE_EGALE(fonction->troncons[i].x6, fonction->troncons[k].x6)) && ((index == NULL) || ((index != NULL) && (ERREUR_RELATIVE_EGALE(index->troncons[i].x0, index->troncons[k].x0)))))
        {
            fonction->troncons[j-1].fin_troncon = fonction->troncons[i].fin_troncon;
            if (index != NULL)
                index->troncons[j-1].fin_troncon = index->troncons[i].fin_troncon;
        }
        else
        {
            j++;
            if (j-1 != i)
            {
                memcpy(&(fonction->troncons[j-1]), &(fonction->troncons[i]), sizeof(Troncon));
                if (index != NULL)
                    memcpy(&(index->troncons[j-1]), &(index->troncons[i]), sizeof(Troncon));
            }
            k = i;
        }
    }
    BUGMSG(fonction->troncons = realloc(fonction->troncons, sizeof(Troncon)*j), FALSE, gettext("Erreur d'allocation mémoire.\n"));
    fonction->nb_troncons = j;
    if (index != NULL)
    {
        BUGMSG(index->troncons = realloc(index->troncons, sizeof(Troncon)*j), FALSE, gettext("Erreur d'allocation mémoire.\n"));
        index->nb_troncons = j;
    }
    
    return TRUE;
}


void common_fonction_ax2_bx_c(double xx1, long double yy1, double xx2, long double yy2, double xx3, long double yy3, double *a, double *b, double *c)
/* Description : renvoie a, b, et c en fonction de f(xx1)=yy1, ....
 * Paramètres : double xx1, double yy1 : coordonnée du permier point,
 *            : double xx2, double yy2 : coordonnée du deuxième point,
 *            : double xx3, double yy3 : coordonnée du troisième point,
 *            : double *a, double *b, double *c : retour de la fonction a*x²+b*x+c
 * Valeur renvoyée : Aucun.
 */
{
    // maxima : solve([a*xx1^2+b*xx1+c=yy1,a*xx2^2+b*xx2+c=yy2,a*xx3^2+b*xx3+c=yy3],[a,b,c]);
    *a=(xx1*(yy3-yy2)-xx2*yy3+xx3*yy2+(xx2-xx3)*yy1)/(xx1*(xx3*xx3-xx2*xx2)-xx2*xx3*xx3+xx2*xx2*xx3+xx1*xx1*(xx2-xx3));
    *b=-(xx1*xx1*(yy3-yy2)-xx2*xx2*yy3+xx3*xx3*yy2+(xx2*xx2-xx3*xx3)*yy1)/(xx1*(xx3*xx3-xx2*xx2)-xx2*xx3*xx3+xx2*xx2*xx3+xx1*xx1*(xx2-xx3));
    *c=(xx1*(xx3*xx3*yy2-xx2*xx2*yy3)+xx1*xx1*(xx2*yy3-xx3*yy2)+(xx2*xx2*xx3-xx2*xx3*xx3)*yy1)/(xx1*(xx3*xx3-xx2*xx2)-xx2*xx3*xx3+xx2*xx2*xx3+xx1*xx1*(xx2-xx3));
    
    return;
}


G_MODULE_EXPORT long double common_fonction_y(Fonction* fonction, long double x, int position)
/* Description : Renvoie la valeur f(x). Un ordinateur étant ce qu'il est, lorsqu'on recherche
 *               par approximation successive un zéro, il est nécessaire d'avoir accès au 
 *               maximum de décimales disponible. Le type double ne permet pas toujours
 *               d'atteindre une précision de 1e-8. Le type long double permet environ 1e-10.
 * Paramètres : Fonction* fonction : fonction à afficher,
 *            : double x : abscisse de la fonction à renvoyer,
 *            : int position : cette variable est utilisé dans un cas particulier. Une
 *              abscisse peut posséder renvoyer à deux valeurs différentes. Dans le cas où
 *              l'abscisse demandée est exactement celle séparant deux tronçons, une
 *              discontinuité peut apparaître. Ainsi, si position vaut -1 la valeur du tronçon
 *              inférieure sera renvoyée (et NAN si position vaut l'abscisse inférieure de la
 *              fonction). Si position vaut 1, la valeur du tronçon supérieure est renvoyée ( et
 *              NAN si la position vaut l'abscisse supérieure de la fonction). Si position vaut
 *              0, l'abscisse inférieure sera renvoyée mais sans NAN en cas d'erreur.
 * Valeur renvoyée :
 *   Succès : La valeur souhaitée,
 *   Échec : NAN :
 *             fonction == NULL,
 *             x hors domaine,
 *             position < -1 ou position > 1
 */
{
    unsigned int i;
    
    BUGMSG(fonction, FALSE, gettext("Paramètre %s incorrect.\n"), "fonction");
    BUGMSG((-1 <= position) && (position <= 1), FALSE, gettext("Paramètre %s incorrect.\n"), "position");
    
    if (fonction->nb_troncons == 0)
        return NAN;
    
    if ((ERREUR_RELATIVE_EGALE(fonction->troncons[0].debut_troncon, x)) && (position == -1))
        return NAN;
    
    for (i=0;i<fonction->nb_troncons;i++)
    {
        if (x <= fonction->troncons[i].fin_troncon*(1+ERREUR_RELATIVE_MIN))
        {
            if (x < fonction->troncons[i].debut_troncon)
                return NAN;
            else
            {
                if ((ERREUR_RELATIVE_EGALE(fonction->troncons[i].fin_troncon, x)) && (position == 1))
                {
                    if (i == fonction->nb_troncons-1)
                        return NAN;
                    else
                        i++;
                }
                return (long double)
                      fonction->troncons[i].x0+
                      fonction->troncons[i].x1*x+
                      fonction->troncons[i].x2*x*x+
                      fonction->troncons[i].x3*x*x*x+
                      fonction->troncons[i].x4*x*x*x*x+
                      fonction->troncons[i].x5*x*x*x*x*x+
                      fonction->troncons[i].x6*x*x*x*x*x*x;
            }
        }
    }
    
    return NAN;
}


gboolean common_fonction_cherche_zero(Fonction* fonction, double mini, double maxi,
  double *zero_1, double *zero_2)
/* Description : Cherche l'abscisse dont l'ordonnée vaut 0.
 * Paramètres : Fonction* fonction : fonction à analyser,
 *            : double mini : valeur basse du domaine de recherche,
 *            : double maxi : valeur haute du domaine de recherche,
 *            : double zero1 : première abscisse ou l'ordonnée vaut 0,
 *            : double zero2 : deuxième abscisse ou l'ordonnée vaut 0.
 *              Si un seul zéro est trouvé, il sera toujours dans zero1 et zero2 vaudra NAN.
 * Valeur renvoyée :
 *   le nombre de points caractéristiques.
 *   Échec : NAN :
 *             fonction == NULL.
 */
{
    double xx1_2, xx2_2, xx3_2;
    double yy1, yy2, yy3;
    double a, b, c;
    double zero1, zero2;
    double zero1_old, zero2_old;
    double ecart_x;
    
    BUGMSG(fonction, FALSE, gettext("Paramètre %s incorrect.\n"), "fonction");
    BUGMSG(maxi > mini, FALSE, gettext("Paramètre %s incorrect.\n"), gettext("Borne incorrecte.\n"));
    
    xx1_2 = mini;
    xx3_2 = maxi;
    xx2_2 = (xx1_2+xx3_2)/2.;
    yy1 = common_fonction_y(fonction, xx1_2, 1);
    yy2 = common_fonction_y(fonction, xx2_2, 0);
    yy3 = common_fonction_y(fonction, xx3_2, -1);
    
    common_fonction_ax2_bx_c(xx1_2, yy1, xx2_2, yy2, xx3_2, yy3, &a, &b, &c);
    
    ecart_x = (xx3_2-xx1_2)/4.;
    
    if (ERREUR_RELATIVE_EGALE(a, 0.))
    {
        if (ERREUR_RELATIVE_EGALE(b, 0.))
        {
            zero1 = NAN;
            zero2 = NAN;
        }
        else
        {
            zero1 = -c/b;
            if ((zero1 < mini*(1-ERREUR_RELATIVE_MIN)) || (zero1 > maxi*(1+ERREUR_RELATIVE_MIN)))
                zero1 = NAN;
            zero2 = NAN;
        }
    }
    else
    {
        if (b*b-4*a*c < 0.)
        {
            zero1 = NAN;
            zero2 = NAN;
        }
        else
        {
            zero1=(sqrt(b*b-4*a*c)-b)/(2*a);
            zero2=(-sqrt(b*b-4*a*c)-b)/(2*a);
        }
    }
    
    if ((zero1 < mini*(1-ERREUR_RELATIVE_MIN)) || (zero1 > maxi*(1+ERREUR_RELATIVE_MIN)))
        zero1 = NAN;
    if ((zero2 < mini*(1-ERREUR_RELATIVE_MIN)) || (zero2 > maxi*(1+ERREUR_RELATIVE_MIN)))
        zero2 = NAN;
    // Idéalement, il faudrait faire une boucle ci-dessous mais les limitations de la précision
    // de la virgule flottante va attendre ces limites en quelques itérations.
    // On utilise cette méthode 2 fois pour déterminer une première estimation précise des
    // zéro avec l'écart. Pour la suite, on le fait par le biais d'une recherche dicotomique.
    ecart_x = (maxi-mini)/4.;
    if (!isnan(zero1))
    {
        zero1_old = zero1;
        if (zero1-ecart_x < mini)
            xx1_2 = mini;
        else
            xx1_2 = zero1-ecart_x;
        if (zero1+ecart_x > maxi)
            xx3_2 = maxi;
        else
            xx3_2 = zero1+ecart_x;
        xx2_2 = (xx1_2+xx3_2)/2.;
        
        common_fonction_ax2_bx_c(xx1_2, common_fonction_y(fonction, xx1_2, 1), xx2_2, common_fonction_y(fonction, xx2_2, 0), xx3_2, common_fonction_y(fonction, xx3_2, -1), &a, &b, &c);
        
        if (ERREUR_RELATIVE_EGALE(a, 0.))
        {
            if (ERREUR_RELATIVE_EGALE(b, 0.))
                zero1 = NAN;
            else
                zero1 = -c/b;
        }
        else
        {
            if (b*b-4*a*c < 0.)
                zero1 = NAN;
            else
                zero1 = (sqrt(b*b-4*a*c)-b)/(2*a);
        }
        
        ecart_x = ABS(zero1_old-zero1)/2.;
    }
    if ((zero1 < mini*(1-ERREUR_RELATIVE_MIN)) || (zero1 > maxi*(1+ERREUR_RELATIVE_MIN)))
        zero1 = NAN;
    
    // Recherche dicotomique du zero1
    // Première vérification d'usage.
    if ((!isnan(zero1)) && (ecart_x > ERREUR_RELATIVE_MIN))
    {
        xx1_2 = MAX(zero1-ecart_x, mini);
        xx3_2 = MIN(zero1+ecart_x, maxi);
        xx2_2 = (xx1_2+xx3_2)/2.;
        while (xx3_2-xx1_2 > ERREUR_RELATIVE_MIN/10)
        {
            ecart_x = xx3_2-xx1_2;
            if (signbit(common_fonction_y(fonction, xx1_2, 1)) == signbit(common_fonction_y(fonction, xx2_2, 0)))
                xx1_2 = xx2_2;
            else
                xx3_2 = xx2_2;
            xx2_2 = (xx1_2+xx3_2)/2.;
        }
        zero1 = (xx3_2+xx1_2)/2.;
    }
    
    // On passe à la recherche du zero2 par la même méthode de travail
    ecart_x = (maxi-mini)/4.;
    if (!isnan(zero2))
    {
        zero2_old = zero2;
        if (zero2-ecart_x < mini)
            xx1_2 = mini;
        else
            xx1_2 = zero2-ecart_x;
        if (zero2+ecart_x > maxi)
            xx3_2 = maxi;
        else
            xx3_2 = zero2+ecart_x;
        xx2_2 = (xx1_2+xx3_2)/2.;
        
        common_fonction_ax2_bx_c(xx1_2, common_fonction_y(fonction, xx1_2, 1), xx2_2, common_fonction_y(fonction, xx2_2, 0), xx3_2, common_fonction_y(fonction, xx3_2, -1), &a, &b, &c);
        
        if (ERREUR_RELATIVE_EGALE(a, 0.))
        {
            if (ERREUR_RELATIVE_EGALE(b, 0.))
                zero2 = NAN;
            else
                zero2 = -c/b;
        }
        else
        {
            if (b*b-4*a*c < 0.)
                zero2 = NAN;
            else
                zero2=(-sqrt(b*b-4*a*c)-b)/(2*a);
        }
        
        ecart_x = ABS(zero2_old-zero2)/2.;
    }
    // Dicotomie
    if ((!isnan(zero2)) && (ecart_x > ERREUR_RELATIVE_MIN))
    {
        xx1_2 = MAX(zero2-ecart_x, mini);
        xx3_2 = MIN(zero2+ecart_x, maxi);
        xx2_2 = (xx1_2+xx3_2)/2.;
        while (xx3_2-xx1_2 > ERREUR_RELATIVE_MIN/10)
        {
            ecart_x = xx3_2-xx1_2;
            if (signbit(common_fonction_y(fonction, xx1_2, 1)) == signbit(common_fonction_y(fonction, xx2_2, 0)))
                xx1_2 = xx2_2;
            else
                xx3_2 = xx2_2;
            xx2_2 = (xx1_2+xx3_2)/2.;
        }
        zero2 = (xx3_2+xx1_2)/2.;
    }
    
    if ((zero2 < mini*(1-ERREUR_RELATIVE_MIN)) || (zero2 > maxi*(1+ERREUR_RELATIVE_MIN)))
        zero2 = NAN;
    
    if ((isnan(zero1)) && (isnan(zero2)))
    {
        *zero_1 = NAN;
        *zero_2 = NAN;
    }
    else if ((isnan(zero1)) && (!isnan(zero2)))
    {
        *zero_1 = zero2;
        *zero_2 = NAN;
    }
    else if ((!isnan(zero1)) && (isnan(zero2)))
    {
        *zero_1 = zero1;
        *zero_2 = NAN;
    }
    else if (ERREUR_RELATIVE_EGALE(zero1, zero2))
    {
        *zero_1 = zero1;
        *zero_2 = NAN;
    }
    else
    {
        *zero_1 = zero1;
        *zero_2 = zero2;
    }
    
    if ((isnan(*zero_1)) || (!isnan(*zero_2)))
        return FALSE;
    return TRUE;
}


unsigned int common_fonction_caracteristiques(Fonction* fonction, double **pos,
  double **val)
/* Description : Affiche les points caractéristiques de la fonction.
 * Paramètres : Fonction* fonction : fonction à afficher,
 *            : double *pos : position des points caractéristiques,
 *            : double *val : valeur des points caractéristiques.
 *            : int decimales : précision que doit avoir la variable pos.
 * Valeur renvoyée :
 *   le nombre de points caractéristiques.
 *   Échec : 0 :
 *             fonction == NULL,
 *             erreur d'allocation mémoire.
 */
{
    unsigned int    i, nb = 0, j;
    double          *pos_tmp = NULL, *val_tmp = NULL;
    
    BUGMSG(fonction, 0, gettext("Paramètre %s incorrect.\n"), "fonction");
    
    if (fonction->nb_troncons == 0)
    {
        *pos = NULL;
        *val = NULL;
        return 0;
    }
    
    for (i=0;i<fonction->nb_troncons;i++)
    {
        // On commence par s'occuper du début du tronçon. 
        
        // On ajoute si c'est le début de la fonction
        if (i==0)
        {
            BUGMSG(pos_tmp = malloc(sizeof(double)), 0, gettext("Erreur d'allocation mémoire.\n"));;
            pos_tmp[0] = fonction->troncons[0].debut_troncon;
            BUGMSG(val_tmp = malloc(sizeof(double)), 0, gettext("Erreur d'allocation mémoire.\n"));
            val_tmp[0] = common_fonction_y(fonction, fonction->troncons[0].debut_troncon, 1);
            nb = 1;
        }
        else
        {
            // On vérifie si la fonction est discontinue en x
            if ((!ERREUR_RELATIVE_EGALE(fonction->troncons[i].debut_troncon, fonction->troncons[i-1].fin_troncon)) ||
            // ou si elle est discontinue en y
              (!ERREUR_RELATIVE_EGALE(common_fonction_y(fonction, fonction->troncons[i].debut_troncon, -1), common_fonction_y(fonction, fonction->troncons[i].debut_troncon, 1))))
            {
                nb++;
                BUGMSG(pos_tmp = realloc(pos_tmp, sizeof(double)*nb), 0, gettext("Erreur d'allocation mémoire.\n"));
                pos_tmp[nb-1] = fonction->troncons[i].debut_troncon;
                BUGMSG(val_tmp = realloc(val_tmp, sizeof(double)*nb), 0, gettext("Erreur d'allocation mémoire.\n"));
                val_tmp[nb-1] = common_fonction_y(fonction, fonction->troncons[i].debut_troncon, -1);
                nb++;
                BUGMSG(pos_tmp = realloc(pos_tmp, sizeof(double)*nb), 0, gettext("Erreur d'allocation mémoire.\n"));
                pos_tmp[nb-1] = fonction->troncons[i].debut_troncon;
                BUGMSG(val_tmp = realloc(val_tmp, sizeof(double)*nb), 0, gettext("Erreur d'allocation mémoire.\n"));
                val_tmp[nb-1] = common_fonction_y(fonction, fonction->troncons[i].debut_troncon, 1);
            }
            // Si elle est continue, elle est un point caractéristique si sa dérivée change de
            // signe. Rappel : f'(x)=(f(x+Dx)-f(x))/Dx. Dans notre cas, on prendra Dx = 1/1000
            // du troncon.
            else
            {
                double dx;
                double fprim1, fprim2;
                
                dx = (fonction->troncons[i-1].fin_troncon-fonction->troncons[i-1].debut_troncon)/1000.;
                fprim1 = (common_fonction_y(fonction, fonction->troncons[i].debut_troncon, -1)-common_fonction_y(fonction, fonction->troncons[i].debut_troncon-dx, 0))/dx;
                
                dx = (fonction->troncons[i].fin_troncon-fonction->troncons[i].debut_troncon)/1000.;
                fprim2 = (common_fonction_y(fonction, fonction->troncons[i].debut_troncon+dx, 0)-common_fonction_y(fonction, fonction->troncons[i].debut_troncon, 1))/dx;
                
                if (signbit(fprim1) != signbit(fprim2))
                {
                    nb++;
                    BUGMSG(pos_tmp = realloc(pos_tmp, sizeof(double)*nb), 0, gettext("Erreur d'allocation mémoire.\n"));
                    pos_tmp[nb-1] = fonction->troncons[i].debut_troncon;
                    BUGMSG(val_tmp = realloc(val_tmp, sizeof(double)*nb), 0, gettext("Erreur d'allocation mémoire.\n"));
                    val_tmp[nb-1] = common_fonction_y(fonction, fonction->troncons[i].debut_troncon, -1);
                }
            }
        }
        
        // On étudie l'intérieur du tronçon
        for (j=0;j<9;j++)
        {
            double  xx1, xx2, xx3;
            double  xx1_2, xx2_2, xx3_2;
            double  a, b, c; // forme de l'interpolation : a*x²+b*x+c
            double  zero1, zero2, deriv_zero;
            double  zero1_old, zero2_old = NAN, deriv_zero_old = NAN;
            double  ecart_x, ecart_old;
            
            // On commence par calculer une interpolation hyperbolique
            // On s'arrange pour que la deuxième moitié du tronçon à analyser soit la première
            // moitié du tronçon suivant. C'est une sécurité. En effet, on ne connait pas 
            // l'erreur de l'approximation faite. Il se pourrait qu'avec l'approximation de base
            // x soit dans l'intervalle mais que, suite à l'affinement x en sorte.
            xx1 = fonction->troncons[i].debut_troncon+(fonction->troncons[i].fin_troncon-fonction->troncons[i].debut_troncon)/10.*j;
            xx2 = fonction->troncons[i].debut_troncon+(fonction->troncons[i].fin_troncon-fonction->troncons[i].debut_troncon)/10.*(j+1);
            xx3 = fonction->troncons[i].debut_troncon+(fonction->troncons[i].fin_troncon-fonction->troncons[i].debut_troncon)/10.*(j+2.);
            
            common_fonction_ax2_bx_c(xx1, common_fonction_y(fonction, xx1, 1), xx2, common_fonction_y(fonction, xx2, 0), xx3, common_fonction_y(fonction, xx3, -1), &a, &b, &c);
            
            if (ERREUR_RELATIVE_EGALE(a, 0.))
            {
                if (ERREUR_RELATIVE_EGALE(b, 0.))
                {
                    zero1 = NAN;
                    zero2 = NAN;
                }
                else
                {
                    zero1 = -c/b;
                    zero2 = NAN;
                }
                deriv_zero = NAN;
            }
            else
            {
                if (b*b-4*a*c < 0.)
                {
                    zero1 = NAN;
                    zero2 = NAN;
                }
                else
                {
                    // maxima : solve(a*x^2+b*x+c=0,x);
                    zero1=(sqrt(b*b-4*a*c)-b)/(2*a);
                    zero2=(-sqrt(b*b-4*a*c)-b)/(2*a);
                }
                deriv_zero = -b/(2*a);
            }
            
            if ((deriv_zero < xx1) || (deriv_zero > xx3))
                deriv_zero = NAN;
            if ((zero1 < xx1) || (zero1 > xx3))
                zero1 = NAN;
            if ((zero2 < xx1) || (zero2 > xx3))
                zero2 = NAN;
            
            ecart_old = xx3-xx1;
            ecart_x = (xx3-xx1)/4.;
            while ((!isnan(zero1)) && (ABS(ecart_x) > ERREUR_RELATIVE_MIN/1000.))
            {
                zero1_old = zero1;
                if (zero1-ecart_x < xx1)
                    xx1_2 = xx1;
                else
                    xx1_2 = zero1-ecart_x;
                if (zero1+ecart_x > xx3)
                    xx3_2 = xx3;
                else
                    xx3_2 = zero1+ecart_x;
                xx2_2 = (xx1_2+xx3_2)/2.;
                
                common_fonction_ax2_bx_c(xx1_2, common_fonction_y(fonction, xx1_2, 1), xx2_2, common_fonction_y(fonction, xx2_2, 0), xx3_2, common_fonction_y(fonction, xx3_2, -1), &a, &b, &c);
                
                if (ERREUR_RELATIVE_EGALE(a, 0.))
                {
                    if (ERREUR_RELATIVE_EGALE(b, 0.))
                        zero1 = NAN;
                    else
                    {
                        zero1 = -c/b;
                        if ((zero1 < xx1) || (zero1 > xx3))
                            zero1 = NAN;
                    }
                }
                else
                {
                    if (b*b-4*a*c < 0.)
                        zero1 = NAN;
                    else
                    {
                        zero1=(sqrt(b*b-4*a*c)-b)/(2*a);
                        if ((zero1 < xx1) || (zero1 > xx3))
                            zero1 = NAN;
                    }
                }
                ecart_x = ABS(zero1_old-zero1)/4.;
                if (ecart_x > ecart_old)
                {
                    // Si on arrive ici, c'est que la méthode ci-dessus ne marche plus à cause
                    // des imprécisions dûes aux virgules flottantes.
                    // On passe donc en mode recherche dicotomique en calculant le signe
                    // auix points xx1_2, xx2_2 et xx3_2.
                    // Si le signe de xx1_2 est le même que xx2_2, xx1_2 devient égal à xx2_2.
                    // Si le signe de xx3_2 est le même que xx2_2, xx3_2 devient égal à xx2_2.
                    // La méthode est un peu plus longue mais est moins problématique.
                    while (TRUE)
                    {
                        ecart_old = xx3_2-xx1_2;
                        if (signbit(common_fonction_y(fonction, xx1_2, 1)) == signbit(common_fonction_y(fonction, xx2_2, 0)))
                            xx1_2 = xx2_2;
                        else xx3_2 = xx2_2;
                        if (ABS(xx3_2-xx1_2) < ERREUR_RELATIVE_MIN/1000.)
                        {
                            zero1 = (xx3_2+xx1_2)/2.;
                            break;
                        }
                        xx2_2 = (xx3_2+xx1_2)/2.;
                    }
                    
                    break;
                }
                else
                {
                    if ((zero1 < xx1) || (zero1 > xx3))
                        zero1 = NAN;
                }
                ecart_old = ecart_x;
            }
            
            ecart_old = xx3-xx1;
            ecart_x = (xx3-xx1)/4.;
            while ((!isnan(zero2)) && (ABS(ecart_x) > ERREUR_RELATIVE_MIN/1000.))
            {
                zero2_old = zero2;
                if (zero2-ecart_x < xx1)
                    xx1_2 = xx1;
                else
                    xx1_2 = zero2-ecart_x;
                if (zero2+ecart_x > xx3)
                    xx3_2 = xx3;
                else
                    xx3_2 = zero2+ecart_x;
                xx2_2 = (xx1_2+xx3_2)/2.;
                
                common_fonction_ax2_bx_c(xx1_2, common_fonction_y(fonction, xx1_2, 1), xx2_2, common_fonction_y(fonction, xx2_2, 0), xx3_2, common_fonction_y(fonction, xx3_2, -1), &a, &b, &c);
                
                if (ERREUR_RELATIVE_EGALE(a, 0.))
                {
                    if (ERREUR_RELATIVE_EGALE(b, 0.))
                        zero2 = NAN;
                    else
                    {
                        zero2 = -c/b;
                        if ((zero2 < xx1) || (zero2 > xx3))
                            zero2 = NAN;
                    }
                }
                else
                {
                    if (b*b-4*a*c < 0.)
                        zero2 = NAN;
                    else
                    {
                        zero2=(-sqrt(b*b-4*a*c)-b)/(2*a);
                        if ((zero2 < xx1) || (zero2 > xx3))
                            zero2 = NAN;
                    }
                }
                ecart_x = ABS(zero2_old-zero2)/4.;
                if (ecart_x > ecart_old)
                {
                    // Si on arrive ici, c'est que la méthode ci-dessus ne marche plus à cause
                    // des imprécisions dûes aux virgules flottantes.
                    // On passe donc en mode recherche dicotomique en calculant le signe
                    // auix points xx1_2, xx2_2 et xx3_2.
                    // Si le signe de xx1_2 est le même que xx2_2, xx1_2 devient égal à xx2_2.
                    // Si le signe de xx3_2 est le même que xx2_2, xx3_2 devient égal à xx2_2.
                    // La méthode est un peu plus longue mais est moins problématique.
                    while (TRUE)
                    {
                        ecart_old = xx3_2-xx1_2;
                        if (signbit(common_fonction_y(fonction, xx1_2, 1)) == signbit(common_fonction_y(fonction, xx2_2, 0)))
                            xx1_2 = xx2_2;
                        else xx3_2 = xx2_2;
                        if (ABS(xx3_2-xx1_2) < ERREUR_RELATIVE_MIN/1000.)
                        {
                            zero2 = (xx3_2+xx1_2)/2.;
                            break;
                        }
                        xx2_2 = (xx3_2+xx1_2)/2.;
                    }
                    
                    break;
                }
                else
                {
                    if ((zero2 < xx1) || (zero2 > xx3))
                        zero2 = NAN;
                }
            }
            
            ecart_old = xx3-xx1;
            ecart_x = (xx3-xx1)/4.;
            while ((!isnan(deriv_zero)) && (ABS(ecart_x) > ERREUR_RELATIVE_MIN/1000.))
            {
                deriv_zero_old = deriv_zero;
                if (deriv_zero-ecart_x < xx1)
                    xx1_2 = xx1;
                else
                    xx1_2 = deriv_zero-ecart_x;
                if (deriv_zero+ecart_x > xx3)
                    xx3_2 = xx3;
                else
                    xx3_2 = deriv_zero+ecart_x;
                xx2_2 = (xx1_2+xx3_2)/2.;
                
                common_fonction_ax2_bx_c(xx1_2, common_fonction_y(fonction, xx1_2, 1), xx2_2, common_fonction_y(fonction, xx2_2, 0), xx3_2, common_fonction_y(fonction, xx3_2, -1), &a, &b, &c);
                
                if (ERREUR_RELATIVE_EGALE(a, 0.))
                    deriv_zero = NAN;
                else
                    deriv_zero = -b/(2*a);
                ecart_x = ABS(deriv_zero_old-deriv_zero)/4.;
                if (ecart_x > ecart_old)
                {
                    // Si on arrive ici, c'est que la méthode ci-dessus ne marche plus à cause
                    // des imprécisions dûes aux virgules flottantes.
                    // On passe donc en mode recherche dicotomique en calculant le signe
                    // de la dérivée aux points xx1_2, xx2_2 et xx3_2.
                    // Si le signe de xx1_2 est le même que xx2_2, xx1_2 devient égal à xx2_2.
                    // Si le signe de xx3_2 est le même que xx2_2, xx3_2 devient égal à xx2_2.
                    // La méthode est un peu plus longue mais est moins problématique.
                    while (TRUE)
                    {
                        ecart_old = xx3_2-xx1_2;
                        a = common_fonction_y(fonction, xx1_2+ecart_old/10., 1)-common_fonction_y(fonction, xx1_2, 1);
                        b = common_fonction_y(fonction, xx2_2+ecart_old/10., 0)-common_fonction_y(fonction, xx2_2, 0);
                        c = common_fonction_y(fonction, xx3_2, -1)-common_fonction_y(fonction, xx3_2-ecart_old/10., -1);
                        if (signbit(a) == signbit(b))
                            xx1_2 = xx2_2;
                        else xx3_2 = xx2_2;
                        if (ABS(xx3_2-xx1_2) < ERREUR_RELATIVE_MIN/1000.)
                        {
                            deriv_zero = (xx3_2+xx1_2)/2.;
                            break;
                        }
                        xx2_2 = (xx3_2+xx1_2)/2.;
                    }
                    
                    break;
                }
                else
                {
                    if ((deriv_zero < xx1) || (deriv_zero > xx3))
                        deriv_zero = NAN;
                }
                ecart_old = ecart_x;
            }
            
            // On tri les résultats.
            a = zero1;
            b = zero2;
            c = deriv_zero;
            
            if (a>b)
            {
                a = zero2;
                b = zero1;
            }
            if (b>c)
            {
                ecart_x = b;
                b = c;
                c = ecart_x;
            }
            if (a>c)
            {
                ecart_x = a;
                a = c;
                c = ecart_x;
            }
            
            if ((!isnan(a)) && (!ERREUR_RELATIVE_EGALE(pos_tmp[nb-1]-a, 0.)))
            {
                nb++;
                BUGMSG(pos_tmp = realloc(pos_tmp, sizeof(double)*nb), 0, gettext("Erreur d'allocation mémoire.\n"));
                pos_tmp[nb-1] = a;
                BUGMSG(val_tmp = realloc(val_tmp, sizeof(double)*nb), 0, gettext("Erreur d'allocation mémoire.\n"));
                val_tmp[nb-1] = common_fonction_y(fonction, a, 0);
            }
            if ((!isnan(b)) && (!ERREUR_RELATIVE_EGALE(pos_tmp[nb-1]-b, 0.)))
            {
                nb++;
                BUGMSG(pos_tmp = realloc(pos_tmp, sizeof(double)*nb), 0, gettext("Erreur d'allocation mémoire.\n"));
                pos_tmp[nb-1] = b;
                BUGMSG(val_tmp = realloc(val_tmp, sizeof(double)*nb), 0, gettext("Erreur d'allocation mémoire.\n"));
                val_tmp[nb-1] = common_fonction_y(fonction, b, 0);
            }
            if ((!isnan(c)) && (!ERREUR_RELATIVE_EGALE(pos_tmp[nb-1]-c, 0.)))
            {
                nb++;
                BUGMSG(pos_tmp = realloc(pos_tmp, sizeof(double)*nb), 0, gettext("Erreur d'allocation mémoire.\n"));
                pos_tmp[nb-1] = c;
                BUGMSG(val_tmp = realloc(val_tmp, sizeof(double)*nb), 0, gettext("Erreur d'allocation mémoire.\n"));
                val_tmp[nb-1] = common_fonction_y(fonction, c, 0);
            }
        }
    }
    
    if (!ERREUR_RELATIVE_EGALE(pos_tmp[nb-1], fonction->troncons[fonction->nb_troncons-1].fin_troncon))
    {
        nb++;
        BUGMSG(pos_tmp = realloc(pos_tmp, sizeof(double)*nb), 0, gettext("Erreur d'allocation mémoire.\n"));
        pos_tmp[nb-1] = fonction->troncons[fonction->nb_troncons-1].fin_troncon;
        BUGMSG(val_tmp = realloc(val_tmp, sizeof(double)*nb), 0, gettext("Erreur d'allocation mémoire.\n"));
        val_tmp[nb-1] = common_fonction_y(fonction, fonction->troncons[fonction->nb_troncons-1].fin_troncon, -1);
    }
    
    *pos = pos_tmp;
    *val = val_tmp;
    
    return nb;
}


G_MODULE_EXPORT char* common_fonction_affiche_caract(Fonction* fonction, int decimales_x,
  int decimales_y)
/* Description : Affiche les points caractéristiques d'une fonction
 * Paramètres : Fonction* fonction : fonction à afficher,
 *            : int decimales_x : nombre de décimales à afficher pour l'abscisse.
 *            : int decimales_y : nombre de décimales à afficher pour l'ordonnée.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             fonction == NULL,
 *             Echec de la fonction common_fonction_caracteristiques
 */
{
    double          *pos, *val;
    unsigned int    nb_val, i;
    char            *retour, *tmp;
    
    BUGMSG(fonction, FALSE, gettext("Paramètre %s incorrect.\n"), "fonction");
    
    nb_val = common_fonction_caracteristiques(fonction, &pos, &val);
    
    if (nb_val == 0)
        return NULL;
    
    BUGMSG(retour = g_strdup_printf("%.*lf : %.*lf", decimales_x, pos[0], decimales_y, val[0]), NULL, gettext("Erreur d'allocation mémoire.\n"));
    
    for (i=1;i<nb_val;i++)
    {
        tmp = retour;
        BUGMSG(retour = g_strdup_printf("%s\n%.*lf : %.*lf", retour, decimales_x, pos[i], decimales_y, val[i]), NULL, gettext("Erreur d'allocation mémoire.\n"));
        free(tmp);
    }
    
    free(pos);
    free(val);
    
    return retour;
}


G_MODULE_EXPORT gboolean common_fonction_affiche(Fonction* fonction)
/* Description : Affiche la fonction (coefficients pour chaque tronçon) ainsi que la valeur
 *                 de la fonction pour chaque extrémité du tronçon.
 * Paramètres : Fonction* fonction : fonction à afficher.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             fonction == NULL.
 */
{
    unsigned int i;
    
    // Trivial
    BUGMSG(fonction, FALSE, gettext("Paramètre %s incorrect.\n"), "fonction");
    
    if (fonction->nb_troncons == 0)
        printf(gettext("Fonction indéfinie.\n"));
    
    for (i=0;i<fonction->nb_troncons;i++)
    {
        printf("debut_troncon : %.5f\tfin_troncon : %.5f\t0 : %.20f\tx : %.20f\tx2 : %.20f\tx3 : %.20f\tx4 : %.20f\tx5 : %.20f\tx6 : %.20f\tsoit f(%.5f) = %.20f\tf(%.5f) = %.20f\n",
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
    
    return TRUE;
}


GdkPixbuf* common_fonction_dessin(GList *fonctions, int width, int height, int decimales)
/* Description : Renvoie un dessin représentant la courbe enveloppe.
 * Paramètres : GList *fonctions : la liste contenant les fonctions à dessiner,
 *              int width : la largeur du dessin,
 *              int height : la hauteur du dessin.
 * Valeur renvoyée : Aucune.
 *   Echec : fonction == NULL,
 *           width == 0,
 *           height == 0,
 *           si toutes les fonctions ne commencent et ne finissent pas à la même longueur.
 */
{
    int             rowstride, n_channels;
    int             x, y;
    guchar          *pixels, *p;
    GdkPixbuf       *pixbuf;
    cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
    cairo_t         *cr = cairo_create(surface);
    double          fy_min = 0., fy_max = 0., echelle;
    cairo_path_t    *save_path;
    double          *mi, *ma;
    GList           *list_parcours;
    Fonction        *fonction;
    
    BUGMSG(fonctions, NULL, gettext("Paramètre %s incorrect.\n"), "fonctions");
    BUGMSG(width, NULL, gettext("La largeur du dessin ne peut être nulle.\n"));
    BUGMSG(height, NULL, gettext("La hauteur du dessin ne peut être nulle.\n"));
    BUGMSG(cairo_surface_status(surface) == CAIRO_STATUS_SUCCESS, NULL, gettext("Erreur d'allocation mémoire.\n"));
    BUGMSG(mi = malloc(sizeof(double)*width), NULL, gettext("Erreur d'allocation mémoire.\n"));
    BUGMSG(ma = malloc(sizeof(double)*width), NULL, gettext("Erreur d'allocation mémoire.\n"));
    
    pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, TRUE, 8, width, height);
    pixels = gdk_pixbuf_get_pixels(pixbuf);
    rowstride = gdk_pixbuf_get_rowstride(pixbuf);
    n_channels = gdk_pixbuf_get_n_channels(pixbuf);
    
    cairo_set_antialias(cr, CAIRO_ANTIALIAS_NONE);
    cairo_set_source_rgba(cr, 1., 1., 1., 0.);
    cairo_rectangle(cr, 0, 0, width, height);
    cairo_fill(cr);
        
    // On remplie tout avec un fond blanc
    for (y=0;y<height;y++)
        for (x=0;x<width;x++)
        {
            p = pixels + y * rowstride + x * n_channels;
            p[0] = 255;
            p[1] = 255;
            p[2] = 255;
            if (n_channels == 4)
                p[3] = 0;
        }
    
    list_parcours = fonctions;
    fonction = list_parcours->data;
    for (x=0;x<width;x++)
    {
        mi[x] = common_fonction_y(fonction, fonction->troncons[0].debut_troncon+x*(fonction->troncons[fonction->nb_troncons-1].fin_troncon-fonction->troncons[0].debut_troncon)/(width-1), 0);
        ma[x] = mi[x];
        
        if (fy_max < mi[x])
            fy_max = mi[x];
        if (fy_min > mi[x])
            fy_min = mi[x];
    }
    list_parcours = g_list_next(list_parcours);
    while (list_parcours != NULL)
    {
        fonction = list_parcours->data;
        for (x=0;x<width;x++)
        {
            echelle = common_fonction_y(fonction, fonction->troncons[0].debut_troncon+x*(fonction->troncons[fonction->nb_troncons-1].fin_troncon-fonction->troncons[0].debut_troncon)/(width-1), 0);
            if (echelle > ma[x])
                ma[x] = echelle;
            if (echelle < mi[x])
                mi[x] = echelle;
            
            if (fy_max < echelle)
                fy_max = echelle;
            if (fy_min > echelle)
                fy_min = echelle;
        }
        list_parcours = g_list_next(list_parcours);
    }

    if (ABS(fy_max) < pow(10, -decimales))
        fy_max = 0.;
    if (ABS(fy_min) < pow(10, -decimales))
        fy_min = 0.;
    
    cairo_new_path(cr);
    cairo_set_line_width(cr, 1.);
    if ((ABS(fy_max) > pow(10, -decimales)) || (ABS(fy_min) > pow(10, -decimales)))
    {
        echelle = ((height-1.)/2.)/MAX(ABS(fy_max), ABS(fy_min));
        
        cairo_set_source_rgba(cr, 0.8, 0.8, 0.8, 1.);
        // On inverse le signe en y car au milieu, le fait d'ajouter fait descendre la position.
        cairo_move_to(cr, 1., height/2.-mi[0]*echelle);
        
        for (x=1;x<width;x++)
            cairo_rel_line_to(cr, 1., -(mi[x]-mi[x-1])*echelle);
        
        if (g_list_next(fonctions) != NULL)
        {
            cairo_rel_line_to(cr, 0., -(ma[width-1]-mi[width-1])*echelle);
            for (x=width-2;x>=0;x--)
                cairo_rel_line_to(cr, -1., -(ma[x]-ma[x+1])*echelle);
        }
        else
        {
            cairo_line_to(cr, width, height/2.);
            cairo_line_to(cr, 1., height/2.);
        }
        
        cairo_close_path(cr);
        save_path = cairo_copy_path(cr);
        cairo_fill(cr);
        cairo_set_source_rgba(cr, 0., 0., 0., 1.);
        cairo_new_path(cr);
        cairo_append_path(cr, save_path);
        cairo_stroke(cr);
        cairo_path_destroy(save_path);
    }
    
    cairo_set_source_rgba(cr, 1., 1., 1., 1.);
    cairo_move_to(cr, 1., height/2.);
    cairo_line_to(cr, width, height/2.);
    cairo_stroke(cr);
    cairo_set_source_rgba(cr, 0., 0., 0., 1.);
    echelle = 5.;
    cairo_set_dash(cr, &echelle, 1, 0);
    cairo_move_to(cr, 1., height/2.);
    cairo_line_to(cr, width, height/2.);
    cairo_stroke(cr);
    
    cairo_destroy(cr);
    
    pixbuf = gdk_pixbuf_get_from_surface(surface, 0, 0, width, height);
    cairo_surface_destroy(surface);
    free(mi);
    free(ma);
    
    return pixbuf;
}


G_MODULE_EXPORT char* common_fonction_renvoie(Fonction* fonction, Fonction *index, 
  int decimales)
/* Description : Renvoie la fonction sous forme de texte (coefficients pour chaque tronçon).
 * Paramètres : Fonction* fonction : fonction à afficher.
 *              Fonction *index : nombre (x0) à ajouter avant les équations entre parenthèse.
 *                                peut être NULL,
 *              int decimales : nombre de décimales à afficher.
 * Valeur renvoyée :
 *   Succès : pointeur contenant la fonction sous forme de texte.
 *   Échec : NULL :
 *             fonction == NULL.
 */
{
    unsigned int    i;
    char            *retour, *ajout;
    double          minimum = pow(10, -decimales);
    
    // Trivial
    BUGMSG(fonction, NULL, gettext("Paramètre %s incorrect.\n"), "fonction");
    
    BUG(common_fonction_compacte(fonction, index), NULL);
    
    if (fonction->nb_troncons == 0)
        BUGMSG(retour = g_strdup_printf("%.*lf", decimales, 0.), NULL, gettext("Erreur d'allocation mémoire.\n"));
    else
    {
        BUGMSG(retour = malloc(sizeof(char)), NULL, gettext("Erreur d'allocation mémoire.\n"));
        retour[0] = 0;
    }
    
    for (i=0;i<fonction->nb_troncons;i++)
    {
        char    *tmp;
        
        BUGMSG(ajout = malloc(sizeof(char)), NULL, gettext("Erreur d'allocation mémoire.\n"));
        ajout[0] = 0;
        
        if (i != 0)
        {
            tmp = retour;
            BUGMSG(retour = g_strdup_printf("%s\n", retour), NULL, gettext("Erreur d'allocation mémoire.\n"));
            free(tmp);
        }
        
        if (index != NULL)
        {
            tmp = retour;
            BUGMSG(retour = g_strdup_printf("%s(%.0lf) : ", retour, index->troncons[i].x0), NULL, gettext("Erreur d'allocation mémoire.\n"));
            free(tmp);
        }
        
        if (fonction->nb_troncons != 1)
        {
            tmp = retour;
            BUGMSG(retour = g_strdup_printf(gettext("%sde %.*lfm à %.*lfm : "), tmp, DECIMAL_DISTANCE, fonction->troncons[i].debut_troncon, DECIMAL_DISTANCE, fonction->troncons[i].fin_troncon), NULL, gettext("Erreur d'allocation mémoire.\n"));
            free(tmp);
        }
        
        if (ABS(fonction->troncons[i].x0) > minimum)
        {
            tmp = ajout;
            BUGMSG(ajout = g_strdup_printf("%s%.*lf", ajout, decimales, fonction->troncons[i].x0), NULL, gettext("Erreur d'allocation mémoire.\n"));
            free(tmp);
        }
        
        if (ABS(fonction->troncons[i].x1) > minimum)
        {
            tmp = ajout;
            BUGMSG(ajout = g_strdup_printf("%s%s%.*lf*x", ajout, fonction->troncons[i].x1>0 ? (strcmp(ajout, "") == 0 ? "" : "+") : "", decimales, fonction->troncons[i].x1), NULL, gettext("Erreur d'allocation mémoire.\n"));
            free(tmp);
        }
        
        if (ABS(fonction->troncons[i].x2) > minimum)
        {
            tmp = ajout;
            BUGMSG(ajout = g_strdup_printf("%s%s%.*lf*x²", ajout, fonction->troncons[i].x2>0 ? (strcmp(ajout, "") == 0 ? "" : "+") : "", decimales, fonction->troncons[i].x2), NULL, gettext("Erreur d'allocation mémoire.\n"));
            free(tmp);
        }
        
        if (ABS(fonction->troncons[i].x3) > minimum)
        {
            tmp = ajout;
            BUGMSG(ajout = g_strdup_printf("%s%s%.*lf*x³", ajout, fonction->troncons[i].x3>0 ? (strcmp(ajout, "") == 0 ? "" : "+") : "", decimales, fonction->troncons[i].x3), NULL, gettext("Erreur d'allocation mémoire.\n"));
            free(tmp);
        }
        
        if (ABS(fonction->troncons[i].x4) > minimum)
        {
            tmp = ajout;
            BUGMSG(ajout = g_strdup_printf("%s%s%.*lf*x⁴", ajout, fonction->troncons[i].x4>0 ? (strcmp(ajout, "") == 0 ? "" : "+") : "", decimales, fonction->troncons[i].x4), NULL, gettext("Erreur d'allocation mémoire.\n"));
            free(tmp);
        }
        
        if (ABS(fonction->troncons[i].x5) > minimum)
        {
            tmp = ajout;
            BUGMSG(ajout = g_strdup_printf("%s%s%.*lf*x⁵", ajout, fonction->troncons[i].x5>0 ? (strcmp(ajout, "") == 0 ? "" : "+") : "", decimales, fonction->troncons[i].x5), NULL, gettext("Erreur d'allocation mémoire.\n"));
            free(tmp);
        }
        
        if (ABS(fonction->troncons[i].x6) > minimum)
        {
            tmp = ajout;
            BUGMSG(ajout = g_strdup_printf("%s%s%.*lf*x⁶", ajout, fonction->troncons[i].x6>0 ? (strcmp(ajout, "") == 0 ? "" : "+") : "", decimales, fonction->troncons[i].x6), NULL, gettext("Erreur d'allocation mémoire.\n"));
            free(tmp);
        }
        
        if (strcmp(ajout, "") != 0)
        {
            tmp = retour;
            BUGMSG(retour = g_strdup_printf("%s %s", retour, ajout), NULL, gettext("Erreur d'allocation mémoire.\n"));
            free(tmp);
            free(ajout);
        }
        else
        {
            tmp = retour;
            BUGMSG(retour = g_strdup_printf("%s %.*lf", retour, decimales, 0.), NULL, gettext("Erreur d'allocation mémoire.\n"));
            free(tmp);
            free(ajout);
        }
    }
    
    return retour;
}


gboolean common_fonction_renvoie_enveloppe(GList* fonctions, Fonction *fonction_min,
  Fonction *fonction_max, Fonction *comb_min, Fonction *comb_max)
/* Description : Renvoie deux fonctions qui enveloppent la liste des fonctions. 
 * Paramètres : GList* fonctions : liste des fonctions à envelopper,
 *              Fonction* fonction_min : courbe enveloppe inférieure (en valeur algébrique),
 *              Fonction* fonction_max : courbe enveloppe supérieure (en valeur algébrique),
 *              Fonction *comb_min : le numéro de la combinaison (dans x0) prépondérante,
 *              Fonction *comb_max : le numéro de la combinaison (dans x0) prépondérante.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             fonctions == NULL.
 *             fonction_min == NULL.
 *             fonction_max == NULL.
 */
{
    Fonction    *fonction;
    GList       *list_parcours;
    int         num;
    
    BUGMSG(fonctions, FALSE, gettext("Paramètre %s incorrect.\n"), "fonctions");
    BUGMSG(fonction_min, FALSE, gettext("Paramètre %s incorrect.\n"), "fonction_min");
    BUGMSG(fonction_max, FALSE, gettext("Paramètre %s incorrect.\n"), "fonction_max");
    BUGMSG(comb_min, FALSE, gettext("Paramètre %s incorrect.\n"), "comb_min");
    BUGMSG(comb_max, FALSE, gettext("Paramètre %s incorrect.\n"), "comb_max");
    
    fonction_min->nb_troncons = 0;
    fonction_min->troncons = NULL;
    fonction_max->nb_troncons = 0;
    fonction_max->troncons = NULL;
    comb_min->nb_troncons = 0;
    comb_min->troncons = NULL;
    comb_max->nb_troncons = 0;
    comb_max->troncons = NULL;
    
    list_parcours = fonctions;
    fonction = list_parcours->data;
    
    BUG(common_fonction_ajout_fonction(fonction_min, fonction, 1.), FALSE);
    BUG(common_fonction_ajout_fonction(fonction_max, fonction, 1.), FALSE);
    BUG(common_fonction_ajout_fonction(comb_min, fonction, 0.), FALSE);
    BUG(common_fonction_ajout_fonction(comb_max, fonction, 0.), FALSE);
    
    list_parcours = g_list_next(list_parcours);
    num = 1;
    while (list_parcours != NULL)
    {
        unsigned int    i, j, k;
        double          val[10], x[10];
        Fonction        fonction_moins, fonction_bis;
        double          x_base, tmp;
        
        fonction = list_parcours->data;
        
        memset(&fonction_moins, 0, sizeof(fonction_moins));
        BUG(common_fonction_ajout_fonction(&fonction_moins, fonction, 1.), FALSE);
        BUG(common_fonction_ajout_fonction(&fonction_moins, fonction_max, -1.), FALSE);
        memset(&fonction_bis, 0, sizeof(fonction_bis));
        BUG(common_fonction_ajout_fonction(&fonction_bis, fonction, 1.), FALSE);
        for (i=0;i<fonction_moins.nb_troncons;i++)
        {
            int         modif;
            double      zero1, zero2;
            
            x_base = fonction_moins.troncons[i].debut_troncon;
            BUG(common_fonction_scinde_troncon(&fonction_bis, x_base), FALSE);
            BUG(common_fonction_scinde_troncon(fonction_max, x_base), FALSE);
            BUG(common_fonction_scinde_troncon(comb_max, x_base), FALSE);
            BUG(common_fonction_scinde_troncon(&fonction_moins, x_base), FALSE);
            
            x[0] = x_base;
            val[0] = common_fonction_y(&fonction_moins, x[0], 1);
            for (j=1;j<9;j++)
            {
                x[j] = fonction_moins.troncons[i].debut_troncon + j*(fonction_moins.troncons[i].fin_troncon-fonction_moins.troncons[i].debut_troncon)/9.;
                val[j] = common_fonction_y(&fonction_moins, x[j], 0);
            }
            x[9] = fonction_moins.troncons[i].fin_troncon;
            val[9] = common_fonction_y(&fonction_moins, x[9], -1);
            
            // On vérifie en 10 points si la fonction est toujours inférieure à fonction_max
            // modif = 0 si la fonction en cours d'étude est inférieure à fonction_max.
            // Elle vaut 1 si elle est supérieure.
            if (ERREUR_RELATIVE_EGALE(val[0], 0.))
                modif = -1;
            else if (val[0] < 0.)
                modif = 0;
            else
                modif = 1;
            for (j=1;j<10;j++)
            {
                if ((val[j] > 0.) && (!ERREUR_RELATIVE_EGALE(val[j], 0.)) && (modif == -1))
                    modif = 1;
                else if ((val[j] > 0.) && (!ERREUR_RELATIVE_EGALE(val[j], 0.)) && (modif == 0))
                {
                    BUG(common_fonction_cherche_zero(&fonction_moins, x[j-1], x[j], &zero1, &zero2), FALSE);
                    BUGMSG((!isnan(zero1)) && (isnan(zero2)), FALSE, gettext("Zéro impossible à trouver.\n"));
                    BUG(common_fonction_scinde_troncon(fonction_max, zero1), FALSE);
                    BUG(common_fonction_scinde_troncon(comb_max, zero1), FALSE);
                    BUG(common_fonction_scinde_troncon(&fonction_bis, zero1), FALSE);
                    BUG(common_fonction_scinde_troncon(&fonction_moins, zero1), FALSE);
                    x_base = zero1;
                    modif = 1;
                }
                else if ((val[j] < 0.) && (!ERREUR_RELATIVE_EGALE(val[j], 0.)) && (modif == -1))
                    modif = 0;
                else if ((val[j] < 0.) && (!ERREUR_RELATIVE_EGALE(val[j], 0.)) && (modif == 1))
                {
                    BUG(common_fonction_cherche_zero(&fonction_moins, x[j-1], x[j], &zero1, &zero2), FALSE);
                    BUGMSG((!isnan(zero1)) && (isnan(zero2)), FALSE, gettext("Zéro impossible à trouver.\n"));
                    tmp = fonction_moins.troncons[i].fin_troncon;
                    BUG(common_fonction_scinde_troncon(fonction_max, zero1), FALSE);
                    BUG(common_fonction_scinde_troncon(comb_max, zero1), FALSE);
                    BUG(common_fonction_scinde_troncon(&fonction_bis, zero1), FALSE);
                    BUG(common_fonction_scinde_troncon(&fonction_moins, zero1), FALSE);
                    
                    for (k=0;k<fonction_max->nb_troncons;k++)
                    {
                        if (fonction_max->troncons[k].fin_troncon > zero1)
                            break;
                        if (fonction_max->troncons[k].fin_troncon*(1-ERREUR_RELATIVE_MIN) > x_base)
                        {
                            fonction_max->troncons[k].x0 = fonction_bis.troncons[i].x0;
                            fonction_max->troncons[k].x1 = fonction_bis.troncons[i].x1;
                            fonction_max->troncons[k].x2 = fonction_bis.troncons[i].x2;
                            fonction_max->troncons[k].x3 = fonction_bis.troncons[i].x3;
                            fonction_max->troncons[k].x4 = fonction_bis.troncons[i].x4;
                            fonction_max->troncons[k].x5 = fonction_bis.troncons[i].x5;
                            fonction_max->troncons[k].x6 = fonction_bis.troncons[i].x6;
                            comb_max->troncons[k].x0 = num;
                        }
                    }
                    x_base = zero1;
                    modif = 0;
                    break;
                }
            }
            if (modif == 1)
            {
                tmp = fonction_moins.troncons[i].fin_troncon;
                BUG(common_fonction_scinde_troncon(fonction_max, tmp), FALSE);
                BUG(common_fonction_scinde_troncon(comb_max, tmp), FALSE);
                BUG(common_fonction_scinde_troncon(&fonction_bis, tmp), FALSE);
                BUG(common_fonction_scinde_troncon(&fonction_moins, tmp), FALSE);
                
                for (j=0;j<fonction_max->nb_troncons;j++)
                {
                    if (fonction_max->troncons[j].fin_troncon > tmp)
                        break;
                    if (fonction_max->troncons[j].fin_troncon*(1-ERREUR_RELATIVE_MIN) > x_base)
                    {
                        fonction_max->troncons[j].x0 = fonction_bis.troncons[i].x0;
                        fonction_max->troncons[j].x1 = fonction_bis.troncons[i].x1;
                        fonction_max->troncons[j].x2 = fonction_bis.troncons[i].x2;
                        fonction_max->troncons[j].x3 = fonction_bis.troncons[i].x3;
                        fonction_max->troncons[j].x4 = fonction_bis.troncons[i].x4;
                        fonction_max->troncons[j].x5 = fonction_bis.troncons[i].x5;
                        fonction_max->troncons[j].x6 = fonction_bis.troncons[i].x6;
                        comb_max->troncons[j].x0 = num;
                    }
                }
            }
        }
        free(fonction_moins.troncons);
        free(fonction_bis.troncons);
        BUG(common_fonction_compacte(fonction_max, comb_max), FALSE);
        
        // On passe à la courbe enveloppe inférieure. On utilise exactement le même code que ci-
        // dessus sauf qu'on fait -fonction+fonction_min à la place de fonction-fonction_max.
        // On remplace les fonction_max par fonction_min. C'est tout !
        memset(&fonction_moins, 0, sizeof(fonction_moins));
        BUG(common_fonction_ajout_fonction(&fonction_moins, fonction, -1.), FALSE);
        BUG(common_fonction_ajout_fonction(&fonction_moins, fonction_min, 1.), FALSE);
        memset(&fonction_bis, 0, sizeof(fonction_bis));
        BUG(common_fonction_ajout_fonction(&fonction_bis, fonction, 1.), FALSE);
        for (i=0;i<fonction_moins.nb_troncons;i++)
        {
            int         modif;
            double      zero1, zero2;
            
            x_base = fonction_moins.troncons[i].debut_troncon;
            BUG(common_fonction_scinde_troncon(&fonction_bis, x_base), FALSE);
            BUG(common_fonction_scinde_troncon(fonction_min, x_base), FALSE);
            BUG(common_fonction_scinde_troncon(comb_min, x_base), FALSE);
            BUG(common_fonction_scinde_troncon(&fonction_moins, x_base), FALSE);
            
            x[0] = x_base;
            val[0] = common_fonction_y(&fonction_moins, x[0], 1);
            for (j=1;j<9;j++)
            {
                x[j] = fonction_moins.troncons[i].debut_troncon + j*(fonction_moins.troncons[i].fin_troncon-fonction_moins.troncons[i].debut_troncon)/9.;
                val[j] = common_fonction_y(&fonction_moins, x[j], 0);
            }
            x[9] = fonction_moins.troncons[i].fin_troncon;
            val[9] = common_fonction_y(&fonction_moins, x[9], -1);
            
            // On vérifie en 10 points si la fonction est toujours inférieure à fonction_min
            // modif = 0 si la fonction en cours d'étude est inférieure à fonction_min.
            // Elle vaut 1 si elle est supérieure.
            if (ERREUR_RELATIVE_EGALE(val[0], 0.))
                modif = -1;
            else if (val[0] < 0.)
                modif = 0;
            else
                modif = 1;
            for (j=1;j<10;j++)
            {
                if ((val[j] > 0.) && (!ERREUR_RELATIVE_EGALE(val[j], 0.)) && (modif == -1))
                    modif = 1;
                else if ((val[j] > 0.) && (!ERREUR_RELATIVE_EGALE(val[j], 0.)) && (modif == 0))
                {
                    BUG(common_fonction_cherche_zero(&fonction_moins, x[j-1], x[j], &zero1, &zero2), FALSE);
                    BUGMSG((!isnan(zero1)) && (isnan(zero2)), FALSE, gettext("Zéro impossible à trouver.\n"));
                    BUG(common_fonction_scinde_troncon(fonction_min, zero1), FALSE);
                    BUG(common_fonction_scinde_troncon(comb_min, zero1), FALSE);
                    BUG(common_fonction_scinde_troncon(&fonction_bis, zero1), FALSE);
                    BUG(common_fonction_scinde_troncon(&fonction_moins, zero1), FALSE);
                    x_base = zero1;
                    modif = 1;
                }
                else if ((val[j] < 0.) && (!ERREUR_RELATIVE_EGALE(val[j], 0.)) && (modif == -1))
                    modif = 0;
                else if ((val[j] < 0.) && (!ERREUR_RELATIVE_EGALE(val[j], 0.)) && (modif == 1))
                {
                    BUG(common_fonction_cherche_zero(&fonction_moins, x[j-1], x[j], &zero1, &zero2), FALSE);
                    BUGMSG((!isnan(zero1)) && (isnan(zero2)), FALSE, gettext("Zéro impossible à trouver.\n"));
                    tmp = fonction_moins.troncons[i].fin_troncon;
                    BUG(common_fonction_scinde_troncon(fonction_min, zero1), FALSE);
                    BUG(common_fonction_scinde_troncon(comb_min, zero1), FALSE);
                    BUG(common_fonction_scinde_troncon(&fonction_bis, zero1), FALSE);
                    BUG(common_fonction_scinde_troncon(&fonction_moins, zero1), FALSE);
                    
                    for (k=0;k<fonction_min->nb_troncons;k++)
                    {
                        if (fonction_min->troncons[k].fin_troncon > zero1)
                            break;
                        if (fonction_min->troncons[k].fin_troncon*(1-ERREUR_RELATIVE_MIN) > x_base)
                        {
                            fonction_min->troncons[k].x0 = fonction_bis.troncons[i].x0;
                            fonction_min->troncons[k].x1 = fonction_bis.troncons[i].x1;
                            fonction_min->troncons[k].x2 = fonction_bis.troncons[i].x2;
                            fonction_min->troncons[k].x3 = fonction_bis.troncons[i].x3;
                            fonction_min->troncons[k].x4 = fonction_bis.troncons[i].x4;
                            fonction_min->troncons[k].x5 = fonction_bis.troncons[i].x5;
                            fonction_min->troncons[k].x6 = fonction_bis.troncons[i].x6;
                            comb_min->troncons[k].x0 = num;
                        }
                    }
                    x_base = zero1;
                    modif = 0;
                    break;
                }
            }
            if (modif == 1)
            {
                tmp = fonction_moins.troncons[i].fin_troncon;
                BUG(common_fonction_scinde_troncon(fonction_min, tmp), FALSE);
                BUG(common_fonction_scinde_troncon(comb_min, tmp), FALSE);
                BUG(common_fonction_scinde_troncon(&fonction_bis, tmp), FALSE);
                BUG(common_fonction_scinde_troncon(&fonction_moins, tmp), FALSE);
                
                for (j=0;j<fonction_min->nb_troncons;j++)
                {
                    if (fonction_min->troncons[j].fin_troncon > tmp)
                        break;
                    if (fonction_min->troncons[j].fin_troncon*(1-ERREUR_RELATIVE_MIN) > x_base)
                    {
                        fonction_min->troncons[j].x0 = fonction_bis.troncons[i].x0;
                        fonction_min->troncons[j].x1 = fonction_bis.troncons[i].x1;
                        fonction_min->troncons[j].x2 = fonction_bis.troncons[i].x2;
                        fonction_min->troncons[j].x3 = fonction_bis.troncons[i].x3;
                        fonction_min->troncons[j].x4 = fonction_bis.troncons[i].x4;
                        fonction_min->troncons[j].x5 = fonction_bis.troncons[i].x5;
                        fonction_min->troncons[j].x6 = fonction_bis.troncons[i].x6;
                        comb_min->troncons[j].x0 = num;
                    }
                }
            }
        }
        free(fonction_moins.troncons);
        free(fonction_bis.troncons);
        BUG(common_fonction_compacte(fonction_min, comb_min), FALSE);
        
        list_parcours = g_list_next(list_parcours);
        num++;
    }
    
    return TRUE;
}


G_MODULE_EXPORT gboolean common_fonction_free(Projet *projet, Action *action)
/* Description : Libère les fonctions de toutes les barres de l'action souhaitée.
 * Paramètres : Projet *projet : la variable projet,
 *            : Action *action : pointeur vers l'action.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL,
 *             action == NULL,
 *             projet->modele.barres == NULL.
 */
{
    unsigned int        i, j;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(action, FALSE, gettext("Paramètre %s incorrect.\n"), "action");
    
    // Trivial
    for (i=0;i<6;i++)
    {
        if (action->fonctions_efforts[i] != NULL)
        {
            for (j=0;j<g_list_length(projet->modele.barres);j++)
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
            for (j=0;j<g_list_length(projet->modele.barres);j++)
            {
                free(action->fonctions_deformation[i][j]->troncons);
                free(action->fonctions_deformation[i][j]);
            }
            free(action->fonctions_deformation[i]);
            action->fonctions_deformation[i] = NULL;
        }
        
        if (action->fonctions_rotation[i] != NULL)
        {
            for (j=0;j<g_list_length(projet->modele.barres);j++)
            {
                free(action->fonctions_rotation[i][j]->troncons);
                free(action->fonctions_rotation[i][j]);
            }
            free(action->fonctions_rotation[i]);
            action->fonctions_rotation[i] = NULL;
        }
    }
    
    return TRUE;
}
