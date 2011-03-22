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

#ifndef __1990_ACTIONS_H
#define __1990_ACTIONS_H

#include <list.h>

#include "common_projet.h"

typedef struct __Charge
{
	char			*nom;
	char			*description;
	double			valeur_caracteristique;
	double			valeur_combinaison;		// valeur_caracteristique * psi0
	double			valeur_frequente;		// valeur_caracteristique * psi1
	double			valeur_quasi_permanente;	// valeur_caracteristique * psi2
} Charge;

typedef struct __Action
{
	char			*nom;
	char			*description;
	int			numero;
	int			categorie;			// Les catégories sont définies dans les fonctions _1990_action_categorie
	LIST			*charges;
	int			flags;
} Action;

char *_1990_action_categorie_bat_txt_eu(int type);
char *_1990_action_categorie_bat_txt_fr(int type);
char *_1990_action_categorie_bat_txt(int type, Type_Pays pays);

int _1990_action_type_combinaison_bat_eu(int categorie);
int _1990_action_type_combinaison_bat_fr(int categorie);
int _1990_action_type_combinaison_bat(int categorie, Type_Pays pays);

int _1990_action_init(Projet *projet);
int _1990_action_ajout(Projet *projet, int categorie);
int _1990_action_positionne(Projet *projet, int numero);
int _1990_action_affiche_tout(Projet *projet);
int _1990_action_free(Projet *projet);

#endif
