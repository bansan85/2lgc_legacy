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


int _1992_1_1_materiaux_init(Projet *projet);
int _1992_1_1_materiaux_ajout(Projet *projet, double fck, double nu);
Beton_Materiau* _1992_1_1_materiaux_cherche_numero(Projet *projet, unsigned int numero);
int _1992_1_1_materiaux_free(Projet *projet);


#endif
