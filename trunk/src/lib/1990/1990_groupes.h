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

#ifndef __1990_GROUPES_H
#define __1990_GROUPES_H

#include <list.h>

#include "projet.h"

typedef struct __Element
{
	int			numero;
} Element;

typedef enum
{
	GROUPE_COMBINAISON_OR,
	GROUPE_COMBINAISON_XOR,
	GROUPE_COMBINAISON_AND
} Type_Groupe_Combinaison;

typedef struct __Groupe
{
	char			*nom;
	int			numero;
	Type_Groupe_Combinaison	combinaison;			
	LIST			*elements;
} Groupe;

typedef struct __Etage_Groupe
{
	int			etage;
	LIST			*groupe;
} Etage_Groupe;

int _1990_groupe_etage_init(Projet *projet);
int _1990_groupe_etage_ajout(Projet *projet, int etage);
int _1990_groupe_etage_cherche(void *input, void *curr);
int _1990_groupe_ajout(Projet *projet, int etage, int numero, Type_Groupe_Combinaison combinaison);
int _1990_groupe_ajout_element(Projet *projet, int etage, __attribute__((unused)) int groupe_n, int groupe_n_1);
void _1990_groupe_affiche_tout(Projet *projet);
void _1990_groupe_free(Projet *projet);

#endif
