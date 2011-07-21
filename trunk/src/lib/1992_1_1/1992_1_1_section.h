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


typedef struct __Beton_Section_Caracteristiques
{
	Type_Beton_Section	type;
	unsigned int		numero;
	double			a;
	double			cdgh;
	double			cdgb;
	double			cdgg;
	double			cdgd;
	double			iy;
	double			iz;
	double			j;
} Beton_Section_Caracteristiques;


typedef struct __Beton_Section_Rectangulaire
{
	Beton_Section_Caracteristiques	*caracteristiques;
	double			largeur;
	double			hauteur;
} Beton_Section_Rectangulaire;


typedef struct __Beton_Section_T
{
	Beton_Section_Caracteristiques	*caracteristiques;
	double			largeur_table;
	double			largeur_ame;
	double			hauteur_table;
	double			hauteur_ame;
} Beton_Section_T;


typedef struct __Beton_Section_Carre
{
	Beton_Section_Caracteristiques	*caracteristiques;
	double			cote;
} Beton_Section_Carre;


typedef struct __Beton_Section_Circulaire
{
	Beton_Section_Caracteristiques	*caracteristiques;
	double			diametre;
} Beton_Section_Circulaire;


int _1992_1_1_sections_init(Projet *projet);
int _1992_1_1_sections_ajout_rectangulaire(Projet *projet, double largeur, double hauteur);
int _1992_1_1_sections_ajout_T(Projet *projet, double largeur_table, double largeur_ame, double hauteur_table, double hauteur_ame);
int _1992_1_1_sections_ajout_carre(Projet *projet, double cote);
int _1992_1_1_sections_ajout_circulaire(Projet *projet, double diametre);
void* _1992_1_1_sections_cherche_numero(Projet *projet, unsigned int numero);
int _1992_1_1_sections_free(Projet *projet);

#endif
