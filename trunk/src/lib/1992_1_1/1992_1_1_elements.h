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

#ifndef __1992_1_1_ELEMENTS_H
#define __1992_1_1_ELEMENTS_H

#include "EF_noeud.h"
#include "1992_1_1_section.h"

typedef enum
{
	BETON_ELEMENT_POTEAU,
	BETON_ELEMENT_POUTRE
} Type_Beton_Element;

typedef struct __Beton_Element_Poutre
{
	Type_Beton_Section	section;
	Type_Beton_Element	element;
	void			*dimensions;
	
	EF_noeud		noeud_debut;
	EF_noeud		noeud_fin;
} Beton_Element_Poutre;

int _1992_1_1_elements_init(Projet *projet);
int _1992_1_1_elements_free(Projet *projet);


#endif
