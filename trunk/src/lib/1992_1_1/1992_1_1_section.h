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

#ifndef __1992_1_1_SECTION_H
#define __1992_1_1_SECTION_H

typedef enum
{
	BETON_SECTION_RECTANGULAIRE,
	BETON_SECTION_T,
	BETON_SECTION_CARRE,
	BETON_SECTION_CIRCULAIRE
} Type_Beton_Section;


typedef struct __Beton_Section_Rectangulaire
{
	double		largeur;
	double		hauteur;
} Beton_Section_Rectangulaire;


typedef struct __Beton_Section_T
{
	double		largeur_table;
	double		largeur_ame;
	double		hauteur_table;
	double		hauteur_ame;
} Beton_Section_T;


typedef struct __Beton_Section_Carre
{
	double		cote;
} Beton_Section_Carre;


typedef struct __Beton_Section_Circulaire
{
	double		diametre;
} Beton_Section_Circulaire;


int _1992_1_1_sections_init(Projet *projet);
int _1992_1_1_sections_free(Projet *projet);

#endif
