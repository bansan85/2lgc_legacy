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

#ifndef __1990_COEF_PSI_H
#define __1990_COEF_PSI_H

#include "common_projet.h"

double _1990_coef_psi0_bat_eu(int categorie);
double _1990_coef_psi1_bat_eu(int categorie);
double _1990_coef_psi2_bat_eu(int categorie);
double _1990_coef_psi0_bat_fr(int categorie);
double _1990_coef_psi1_bat_fr(int categorie);
double _1990_coef_psi2_bat_fr(int categorie);
double _1990_coef_psi0_bat(int categorie, Type_Pays pays);
double _1990_coef_psi1_bat(int categorie, Type_Pays pays);
double _1990_coef_psi2_bat(int categorie, Type_Pays pays);

#endif
