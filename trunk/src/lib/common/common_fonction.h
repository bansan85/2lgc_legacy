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

#ifndef __COMMON_FONCTION_H
#define __COMMON_FONCTION_H

#include "config.h"
#include "common_projet.h"

typedef struct __Troncon // Définition de la structure Troncon qui contient le 
                         // domaine de validité de la fonction
{
    double      debut_troncon;  // Début du tronçon de validité de la fonction
    double      fin_troncon;    // Fin du tronçon de validité de la fonction
    double      x0;        // La fonction mathématique est définie par :
    double      x1;        // coef_0 + coef_x*x +
    double      x2;        // coef_x2*x^2 +
    double      x3;        // coef_x3*x^3 +
    double      x4;        // coef_x4*x^4 +
    double      x5;        // coef_x5*x^5 +
    double      x6;        // coef_x6*x^6
} Troncon;

typedef struct __Fonction
{
    int          nb_troncons;    /* Les fonctions n'étant pas forcément continues
                                 * le long de la barre (par exemple de part et 
        * d'une charge ponctuelle), il est nécessaire de définir plusieurs
        * tronçons avec pour chaque tronçon sa fonction.
        * nb_troncons défini le nombre de tronçons que possède la fonction.*/
    
    Troncon      *troncons;      /* Tableau dynamique contenant les fonctions
                                 * continues par tronçon. */
} Fonction;

/* On est obligé de mettre action en void* et pas en Action* pour éviter une dépence circulaire des fichiers due à l'inclusion du fichier en-tête "1990_actions.h"*/
int common_fonction_init(Projet *projet, void *action_void);
int common_fonction_ajout(Fonction* fonction, double debut_troncon, double fin_troncon, double coef_0, double coef_x, double coef_x2, double coef_x3, double coef_x4, double coef_x5, double coef_x6, double translate);
int common_fonction_affiche(Fonction* fonction);
int common_fonction_free(Projet *projet, void *action_void);

#endif
