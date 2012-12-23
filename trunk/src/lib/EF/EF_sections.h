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

gboolean EF_sections_init(Projet *projet) __attribute__((__warn_unused_result__));

#ifdef ENABLE_GTK
gboolean EF_sections_update_ligne_treeview(Projet *projet, EF_Section *section) __attribute__((__warn_unused_result__));
#endif

gboolean EF_sections_rectangulaire_ajout(Projet *projet, const char* nom, double l, double h) __attribute__((__warn_unused_result__));
gboolean EF_sections_rectangulaire_modif(Projet *projet, EF_Section *section, const char* nom, double l, double h) __attribute__((__warn_unused_result__));
gboolean EF_sections_T_ajout(Projet *projet, const char* nom, double lt, double lr, double ht, double hr) __attribute__((__warn_unused_result__));
gboolean EF_sections_T_modif(Projet *projet, EF_Section *section, const char* nom, double lt, double lr, double ht, double hr) __attribute__((__warn_unused_result__));
gboolean EF_sections_carree_ajout(Projet *projet, const char* nom, double cote) __attribute__((__warn_unused_result__));
gboolean EF_sections_carree_modif(Projet *projet, EF_Section *section, const char* nom, double cote)  __attribute__((__warn_unused_result__));
gboolean EF_sections_circulaire_ajout(Projet *projet, const char* nom, double diametre) __attribute__((__warn_unused_result__));
gboolean EF_sections_circulaire_modif(Projet *projet, EF_Section *section, const char* nom, double diametre) __attribute__((__warn_unused_result__));

EF_Section* EF_sections_cherche_nom(Projet *projet, const char *nom, gboolean critique) __attribute__((__warn_unused_result__));
char *EF_sections_get_description(EF_Section *sect) __attribute__((__warn_unused_result__));

gboolean EF_sections_renomme(EF_Section *section, gchar *nom, Projet *projet) __attribute__((__warn_unused_result__));
gboolean EF_sections_supprime(EF_Section *section, gboolean annule_si_utilise, Projet *projet) __attribute__((__warn_unused_result__));

double EF_sections_j(EF_Section *section) __attribute__((__warn_unused_result__));
double EF_sections_iy(EF_Section *section) __attribute__((__warn_unused_result__));
double EF_sections_iz(EF_Section *section) __attribute__((__warn_unused_result__));
double EF_sections_vy(EF_Section *sect) __attribute__((__warn_unused_result__));
double EF_sections_vyp(EF_Section *sect) __attribute__((__warn_unused_result__));
double EF_sections_vz(EF_Section *sect) __attribute__((__warn_unused_result__));
double EF_sections_vzp(EF_Section *sect) __attribute__((__warn_unused_result__));

double EF_sections_ay(Beton_Barre *barre, unsigned int discretisation) __attribute__((__warn_unused_result__));
double EF_sections_by(Beton_Barre *barre, unsigned int discretisation) __attribute__((__warn_unused_result__));
double EF_sections_cy(Beton_Barre *barre, unsigned int discretisation) __attribute__((__warn_unused_result__));
double EF_sections_az(Beton_Barre *barre, unsigned int discretisation) __attribute__((__warn_unused_result__));
double EF_sections_bz(Beton_Barre *barre, unsigned int discretisation) __attribute__((__warn_unused_result__));
double EF_sections_cz(Beton_Barre *barre, unsigned int discretisation) __attribute__((__warn_unused_result__));

double EF_sections_s(EF_Section *sect) __attribute__((__warn_unused_result__));
double EF_sections_es_l(Beton_Barre *barre, unsigned int discretisation, double debut, double fin) __attribute__((__warn_unused_result__));
double EF_sections_gj_l(Beton_Barre *barre, unsigned int discretisation) __attribute__((__warn_unused_result__));

gboolean EF_sections_free(Projet *projet) __attribute__((__warn_unused_result__));

#endif
