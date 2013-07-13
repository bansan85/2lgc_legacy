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

#ifndef __COMMON_VILLE_H
#define __COMMON_VILLE_H

#include "config.h"
#include "common_projet.h"

gboolean common_ville_init(Projet *projet) __attribute__((__warn_unused_result__));
gboolean common_ville_get_ville(char *ligne, int *cdc, int *cheflieu, int *reg, char *dep, int *com, int *ar, int *ct, int *tncc, char **artmaj, char **ncc, char **artmin, char **nccenr, int *code_postal, int *altitude, int *population) __attribute__((__warn_unused_result__));
gboolean common_ville_set(Projet *projet, char *departement, const char *ville, int graphique_seul) __attribute__((__warn_unused_result__));
gboolean common_ville_free(Projet *projet) __attribute__((__warn_unused_result__));

#endif
