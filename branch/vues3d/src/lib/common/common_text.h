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

#ifndef __COMMON_TEXT_H
#define __COMMON_TEXT_H

#include "config.h"
#include "common_projet.h"

void show_warranty();
void show_help();

double common_text_str_to_double(char *texte, double val_min, gboolean min_include, double val_max, gboolean max_include);
char *strcasestr_internal(const char *haystack, const char *needle);
char *common_text_dependances(GList *liste_noeuds, GList *liste_barres, GList *liste_charges, Projet *projet);
char *common_text_get_line(FILE *fichier);

#endif