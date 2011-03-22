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

#ifndef __1990_COMBINAISONS_H
#define __1990_COMBINAISONS_H

#include <list.h>

#include "common_projet.h"
#include "1990_actions.h"

typedef struct __Combinaison_Element
{
	Action		*action;
	int		flags;				// 1 si les actions variables sont principales
} Combinaison_Element;

typedef struct __Combinaison
{
	LIST		*elements;			// Liste de pointeur Combinaison_Element
} Combinaison;

typedef struct __Combinaisons
{
	LIST		*combinaisons;			// Liste de "combinaison"
} Combinaisons;

int _1990_combinaisons_init(Projet *projet);
int _1990_combinaisons_genere(Projet *projet);
int _1990_combinaisons_free(Projet *projet);
#endif
