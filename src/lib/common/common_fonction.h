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

#ifndef __COMMON_EQUATION_H
#define __COMMON_EQUATION_H

#include "common_projet.h"

typedef struct				// Fonction de base : coef_0 + coef_x*x + coef_x2*x*x
{
	double		debut_troncon;	// Début et la fin du tronçon de validité de la fonction
	double		fin_troncon;
	double		coef_0;	
	double		coef_x;	
	double		coef_x2;
	double		coef_x3;
} Troncon;

typedef struct
{
	int		nb_troncons;	// Les fonctions n'étant pas forcément continues le long de la barre (par exemple charge ponctuelle),
					// il est nécessaire de définir plusieurs tronçons avec pour chaque tronçon son fonction.
					// Toutes les fonctions possèderont les mêmes tronçons afin de faciliter leur utilisation combinées.
	Troncon		*troncons;	// Tableau dynamique contenant les fonctions par tronçons
} Fonction;

// On est obligé de mettre action en void* et pas en Action* pour éviter une dépence circulaire des fichiers dû à l'inclusion du fichier en-tête "1990_actions.h"
int common_fonction_init(Projet *projet, void *action_void);
int common_fonction_ajout(Fonction* fonction, double debut_troncon, double fin_troncon, double coef_0, double coef_x, double coef_x2, double coef_x3);
int common_fonction_affiche(Fonction* fonction);
int common_fonction_free(Projet *projet, void *action_void);

#endif
