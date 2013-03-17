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
double _1992_1_1_materiaux_fctm(double fck, double fcm) __attribute__((__warn_unused_result__));
double _1992_1_1_materiaux_fctk_0_05(double fctm) __attribute__((__warn_unused_result__));
double _1992_1_1_materiaux_fctk_0_95(double fctm) __attribute__((__warn_unused_result__));
double _1992_1_1_materiaux_ecm(double fcm) __attribute__((__warn_unused_result__));
double _1992_1_1_materiaux_ec1(double fcm) __attribute__((__warn_unused_result__));
double _1992_1_1_materiaux_ecu1(double fcm, double fck) __attribute__((__warn_unused_result__));
double _1992_1_1_materiaux_ec2(double fck) __attribute__((__warn_unused_result__));
double _1992_1_1_materiaux_ecu2(double fck) __attribute__((__warn_unused_result__));
double _1992_1_1_materiaux_ec3(double fck) __attribute__((__warn_unused_result__));
double _1992_1_1_materiaux_ecu3(double fck) __attribute__((__warn_unused_result__));
double _1992_1_1_materiaux_n(double fck) __attribute__((__warn_unused_result__));
double _1992_1_1_materiaux_gnu(double ecm, double nu) __attribute__((__warn_unused_result__));

EF_Materiau* _1992_1_1_materiaux_ajout(Projet *projet, const char *nom, double fck) __attribute__((__warn_unused_result__));
gboolean _1992_1_1_materiaux_modif(Projet *projet, EF_Materiau *materiau, char *nom, double fck, double fckcube, double fcm, double fctm, double fctk_0_05, double fctk_0_95, double ecm, double ec1, double ecu1, double ec2, double ecu2, double n, double ec3, double ecu3, double nu) __attribute__((__warn_unused_result__));
EF_Materiau* _1992_1_1_materiaux_cherche_nom(Projet *projet, const char *nom, gboolean critique) __attribute__((__warn_unused_result__));

char *_1992_1_1_materiaux_get_description(EF_Materiau* materiau) __attribute__((__warn_unused_result__));

gboolean _1992_1_1_materiaux_update_ligne_treeview(Projet *projet, EF_Materiau *materiau) __attribute__((__warn_unused_result__));

gboolean _1992_1_1_materiaux_supprime(EF_Materiau* materiau, Projet *projet) __attribute__((__warn_unused_result__));
gboolean _1992_1_1_materiaux_free(Projet *projet) __attribute__((__warn_unused_result__));

#endif
