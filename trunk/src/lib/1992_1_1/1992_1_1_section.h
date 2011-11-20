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

#include "config.h"
#include "common_projet.h"

int _1992_1_1_sections_init(Projet *projet);
int _1992_1_1_sections_ajout_rectangulaire(Projet *projet, double l, double h);
int _1992_1_1_sections_ajout_T(Projet *projet, double lt, double la, double ht, double ha);
int _1992_1_1_sections_ajout_carre(Projet *projet, double cote);
int _1992_1_1_sections_ajout_circulaire(Projet *projet, double diametre);

void* _1992_1_1_sections_cherche_numero(Projet *projet, unsigned int numero);

double _1992_1_1_sections_ay(Beton_Barre *barre, unsigned int discretisation);
double _1992_1_1_sections_by(Beton_Barre *barre, unsigned int discretisation);
double _1992_1_1_sections_cy(Beton_Barre *barre, unsigned int discretisation);
double _1992_1_1_sections_az(Beton_Barre *barre, unsigned int discretisation);
double _1992_1_1_sections_bz(Beton_Barre *barre, unsigned int discretisation);
double _1992_1_1_sections_cz(Beton_Barre *barre, unsigned int discretisation);

double _1992_1_1_sections_s(void *sect);
double _1992_1_1_sections_es_l(Beton_Barre *barre, unsigned int discretisation,
  double debut, double fin);
double _1992_1_1_sections_gj_l(Beton_Barre *barre, unsigned int discretisation);
double _1992_1_1_sections_j(void* section);
double _1992_1_1_sections_iy(void* section);
double _1992_1_1_sections_iz(void* section);

int _1992_1_1_sections_free(Projet *projet);

#endif
