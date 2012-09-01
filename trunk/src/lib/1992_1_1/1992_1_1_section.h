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

gboolean _1992_1_1_sections_init(Projet *projet) __attribute__((__warn_unused_result__));

gboolean _1992_1_1_sections_ajout_rectangulaire(Projet *projet, const char* nom, double l, double h) __attribute__((__warn_unused_result__));
gboolean _1992_1_1_sections_ajout_T(Projet *projet, const char* nom, double lt, double la, double ht, double ha) __attribute__((__warn_unused_result__));
gboolean _1992_1_1_sections_ajout_carre(Projet *projet, const char* nom, double cote) __attribute__((__warn_unused_result__));
gboolean _1992_1_1_sections_ajout_circulaire(Projet *projet, const char* nom, double diametre) __attribute__((__warn_unused_result__));

void* _1992_1_1_sections_cherche_nom(Projet *projet, const char *nom) __attribute__((__warn_unused_result__));

double _1992_1_1_sections_j(void* section) __attribute__((__warn_unused_result__));
double _1992_1_1_sections_iy(void* section) __attribute__((__warn_unused_result__));
double _1992_1_1_sections_iz(void* section) __attribute__((__warn_unused_result__));

double _1992_1_1_sections_ay(Beton_Barre *barre, unsigned int discretisation) __attribute__((__warn_unused_result__));
double _1992_1_1_sections_by(Beton_Barre *barre, unsigned int discretisation) __attribute__((__warn_unused_result__));
double _1992_1_1_sections_cy(Beton_Barre *barre, unsigned int discretisation) __attribute__((__warn_unused_result__));
double _1992_1_1_sections_az(Beton_Barre *barre, unsigned int discretisation) __attribute__((__warn_unused_result__));
double _1992_1_1_sections_bz(Beton_Barre *barre, unsigned int discretisation) __attribute__((__warn_unused_result__));
double _1992_1_1_sections_cz(Beton_Barre *barre, unsigned int discretisation) __attribute__((__warn_unused_result__));

double _1992_1_1_sections_s(void *sect) __attribute__((__warn_unused_result__));
double _1992_1_1_sections_es_l(Beton_Barre *barre, unsigned int discretisation, double debut, double fin) __attribute__((__warn_unused_result__));
double _1992_1_1_sections_gj_l(Beton_Barre *barre, unsigned int discretisation) __attribute__((__warn_unused_result__));

gboolean _1992_1_1_sections_free(Projet *projet) __attribute__((__warn_unused_result__));

#endif
