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

#ifndef __COMMON_FONCTION_H
#define __COMMON_FONCTION_H

#include "config.h"
#include "common_projet.h"


gboolean common_fonction_init(Projet *projet, Action *action) __attribute__((__warn_unused_result__));

gboolean common_fonction_ajout(Fonction* fonction, double debut_troncon, double fin_troncon, double coef_0, double coef_x, double coef_x2, double coef_x3, double coef_x4, double coef_x5, double coef_x6, double translate) __attribute__((__warn_unused_result__));
double common_fonction_y(Fonction* fonction, double x, int position) __attribute__((__warn_unused_result__));
gboolean common_fonction_affiche(Fonction* fonction) __attribute__((__warn_unused_result__));
GdkPixbuf* common_fonction_dessin(Fonction* fonction, int width, int height, int decimales) __attribute__((__warn_unused_result__));
char* common_fonction_affiche_caract(Fonction* fonction, int decimales_x, int decimales_y) __attribute__((__warn_unused_result__));

char* common_fonction_renvoie(Fonction* fonction, int decimales) __attribute__((__warn_unused_result__));

gboolean common_fonction_free(Projet *projet, Action *action) __attribute__((__warn_unused_result__));

#endif
