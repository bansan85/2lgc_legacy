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

#ifndef __COMMON_SELECTION_H
#define __COMMON_SELECTION_H

#include "config.h"
#include "common_projet.h"
#include <list.h>


LIST *common_selection_renvoie_numeros(const char *texte);
LIST *common_selection_converti_numeros_en_noeuds(LIST *liste_numeros, Projet *projet);
LIST *common_selection_converti_numeros_en_barres(LIST *liste_numeros, Projet *projet);
char *common_selection_converti_noeuds_en_texte(LIST *liste_noeuds);
char *common_selection_converti_barres_en_texte(LIST *liste_barres);

#endif
