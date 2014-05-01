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

#ifndef __COMMON_MATHS_H
#define __COMMON_MATHS_H

#include "config.h"
#include <cholmod.h>
#include <glib.h>
#include "common_projet.hpp"

#ifndef M_PI
#define M_PI 3.1415926535897932384626433
#endif

#ifndef MAXDOUBLE
#define MAXDOUBLE 1.7976931348623158e308
#endif

#define ERREUR_RELATIVE_PUISSANCE 8
// Avec 13, common_fonction_compacte peut ne pas marcher correctement.

gboolean errmin (double calc,
                 double err_min);
gboolean errmax (double calc,
                 double err_max);
gboolean errmoy (double calc,
                 double err_moy);
gboolean errrel (double calc,
                 double theo);

#define ERRMAX_POND 100.
#define ERRMOY_RAIDEUR 100000.
#define ERRMOY_DIST 1.


/* Vérifie si x == y avec une erreur relative maximale D,
 * y étant la valeur recherchée et x la valeur à vérifier */

double common_math_arrondi_nombre  (double           nombre);
void common_math_arrondi_triplet   (cholmod_triplet *triplet);
void   common_math_arrondi_sparse  (cholmod_sparse  *sparse);
void   common_math_double_to_char  (double           nombre,
                                    char            *dest,
                                    int8_t           decimales);
void   conv_f_c                    (Flottant         nombre,
                                    char            *dest,
                                    uint8_t          decimales);

double   m_g     (Flottant      f);
Flottant m_f     (double        f,
                  Type_Flottant type);
Flottant m_add_f (Flottant      f1,
                  Flottant      f2);
Flottant m_sub_f (Flottant      f1,
                  Flottant      f2);
Flottant m_div_d (Flottant      f,
                  double        d);
Flottant m_dot_d (Flottant      f,
                  double        d);
Flottant m_dot_f (Flottant      f1,
                  Flottant      f2);

#endif

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
