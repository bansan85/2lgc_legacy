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

#ifndef __1992_1_1_MATERIAUX_H
#define __1992_1_1_MATERIAUX_H

#include "config.h"
#include "common_projet.h"

#define COEFFICIENT_NU_BETON    0.2

gboolean _1992_1_1_materiaux_init(Projet *projet) __attribute__((__warn_unused_result__));

double _1992_1_1_materiaux_fckcube(double fck) __attribute__((__warn_unused_result__));
double _1992_1_1_materiaux_fcm(double fck) __attribute__((__warn_unused_result__));
double _1992_1_1_materiaux_fctm(double fck) __attribute__((__warn_unused_result__));
double _1992_1_1_materiaux_fctk_0_05(double fck) __attribute__((__warn_unused_result__));
double _1992_1_1_materiaux_fctk_0_95(double fck) __attribute__((__warn_unused_result__));
double _1992_1_1_materiaux_ecm(double fck) __attribute__((__warn_unused_result__));
double _1992_1_1_materiaux_ec1(double fck) __attribute__((__warn_unused_result__));
double _1992_1_1_materiaux_ecu1(double fck) __attribute__((__warn_unused_result__));
double _1992_1_1_materiaux_ec2(double fck) __attribute__((__warn_unused_result__));
double _1992_1_1_materiaux_ecu2(double fck) __attribute__((__warn_unused_result__));
double _1992_1_1_materiaux_ec3(double fck) __attribute__((__warn_unused_result__));
double _1992_1_1_materiaux_ecu3(double fck) __attribute__((__warn_unused_result__));
double _1992_1_1_materiaux_n(double fck) __attribute__((__warn_unused_result__));
double _1992_1_1_materiaux_gnu(double fck, double nu) __attribute__((__warn_unused_result__));

gboolean _1992_1_1_materiaux_ajout(Projet *projet, const char *nom, double fck) __attribute__((__warn_unused_result__));
Beton_Materiau* _1992_1_1_materiaux_cherche_nom(Projet *projet, const char *nom, gboolean critique) __attribute__((__warn_unused_result__));
gboolean _1992_1_1_materiaux_renomme(Beton_Materiau *materiau, gchar *nom, Projet *projet) __attribute__((__warn_unused_result__));

gboolean _1992_1_1_materiaux_cherche_dependances(Projet *projet, Beton_Materiau* materiau, GList** barres_dep) __attribute__((__warn_unused_result__));
gboolean _1992_1_1_materiaux_verifie_dependances(Projet *projet, Beton_Materiau* materiau) __attribute__((__warn_unused_result__));

char *_1992_1_1_materiaux_get_description(Beton_Materiau* materiau) __attribute__((__warn_unused_result__));

gboolean _1992_1_1_materiaux_supprime(Beton_Materiau* materiau, gboolean annule_si_utilise, Projet *projet) __attribute__((__warn_unused_result__));
gboolean _1992_1_1_materiaux_free(Projet *projet) __attribute__((__warn_unused_result__));

#endif
