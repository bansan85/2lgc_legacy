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
#include <cholmod_internal.h>
#include <values.h>
#include <glib.h>

#ifndef MAXDOUBLE
#define MAXDOUBLE   1.7976931348623158e+308
#endif

#define ERREUR_RELATIVE_PUISSANCE 8 // Avec 13, common_fonction_compate peut ne pas marcher
                                     // correctement.
 #define ERREUR_RELATIVE_MIN 1e-8
 #define ERREUR_RELATIVE_EGALE(x, y) ( \
   (((ABS(x)) < ERREUR_RELATIVE_MIN) || ((ABS(y)) < ERREUR_RELATIVE_MIN)) ? \
   ((ABS(MAX(x,y)-MIN(x,y))) < (ERREUR_RELATIVE_MIN)) : \
   ((ABS((MAX(x,y)-MIN(x,y))/(MIN(x,y)))) < (ERREUR_RELATIVE_MIN) ))
/* Vérifie si x == y avec une erreur relative maximale D,
 * y étant la valeur recherchée et x la valeur à vérifier */

double common_math_arrondi_nombre(double nombre);
void common_math_arrondi_triplet(cholmod_triplet *triplet);
void common_math_arrondi_sparse(cholmod_sparse *sparse);

#endif
