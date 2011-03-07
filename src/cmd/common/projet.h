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

#ifndef __PROJET_H
#define __PROJET_H

#include <list.h>

typedef enum
{
	PAYS_EU,
	PAYS_FR
} Type_Pays;


typedef struct __CombinaisonsEL
{
	LIST		*elu_equ;
	LIST		*elu_str;
	LIST		*elu_geo;
	LIST		*elu_fat;
	LIST		*elu_acc;
	LIST		*elu_sis;
	LIST		*els_car;
	LIST		*els_freq;
	LIST		*els_perm;
} CombinaisonsEL;

typedef struct __Projet
{
	LIST			*actions;
	LIST			*groupes;
	CombinaisonsEL		combinaisons;
	Type_Pays		pays;
} Projet;

Projet *projet_init();
int projet_free(Projet *projet);

#endif
