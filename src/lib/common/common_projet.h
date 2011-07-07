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

#ifndef __COMMON_PROJET_H
#define __COMMON_PROJET_H

#include <list.h>
#include <cholmod.h>

typedef enum
{
	PAYS_EU,
	PAYS_FR
} Type_Pays;


typedef struct __CombinaisonsEL
{
	int		flags;		// bit 1	: ELU_EQU : méthode note 1 si le bit 1 = 0
					//		: ELU_EQU : méthode note 2 si le bit 1 = 1
					// bit 2 et 3	: ELU_GEO/STR : 00 méthode approche 1
					// 		: ELU_GEO/STR : 01 méthode approche 2
					// 		: ELU_GEO/STR : 10 méthode approche 3
					// bit 4	: ELU_ACC : 0 si utilisation de psi1,1
					// 		: ELU_ACC : 1 si utilisation de psi2,1
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

typedef struct __List_Gtk
{
	void			*_1990;
} List_Gtk;

typedef enum
{
	BETON_ELEMENT_POTEAU,
	BETON_ELEMENT_POUTRE
} Type_Element;

typedef struct
{
	LIST			*sections;
	LIST			*elements;
	LIST			*materiaux;
} Beton_Donnees;

typedef struct
{
	cholmod_common		Common;
	cholmod_common		*c;
	cholmod_triplet		*a;
	long			*ai;
	long			*aj;
	double			*ax;
	cholmod_sparse		*A;
	LIST			*noeuds;
	LIST			*appuis;
	LIST			*rigidite; // Liste temporaire avant transformation en triplet (a) puis en matrice common (c)
} EF;

typedef struct __Projet
{
	LIST			*actions;
	LIST			*niveaux_groupes;
	CombinaisonsEL		combinaisons;
	Type_Pays		pays;
	List_Gtk		list_gtk;
	EF			ef_donnees;
	Beton_Donnees		beton;
} Projet;

Projet *projet_init();
int projet_free(Projet *projet);

#endif
