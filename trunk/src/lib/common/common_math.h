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
#include "common_projet.h"

#ifndef M_PI
#define M_PI    3.1415926535897932384626433
#endif

#ifndef MAXDOUBLE
#define MAXDOUBLE   1.7976931348623158e+308
#endif

#define ERREUR_RELATIVE_PUISSANCE 8 // Avec 13, common_fonction_compacte peut ne pas marcher
                   // correctement.
#define ERR_MIN 1e-8
#define ERR(x, y) ( \
   (((ABS (x)) < ERR_MIN) || ((ABS (y)) < ERR_MIN)) ? \
   ((ABS (MAX (x,y) - MIN (x,y))) < (ERR_MIN)) : \
   ((ABS ((MAX (x,y) - MIN (x,y)) / (MIN (x,y)))) < (ERR_MIN) ))
/* Vérifie si x == y avec une erreur relative maximale D,
 * y étant la valeur recherchée et x la valeur à vérifier */

double common_math_arrondi_nombre  (double           nombre);
void   common_math_arrondi_triplet (cholmod_triplet *triplet);
void   common_math_arrondi_sparse  (cholmod_sparse  *sparse);
void   common_math_double_to_char  (double nombre,
                                    char            *dest,
                                    int              decimales);
void   conv_f_c                    (Flottant         nombre,
                                    char            *dest,
                                    int              decimales);

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
