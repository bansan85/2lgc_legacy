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

// _1990_coef_psi0_bat_exp_eu : renvoie le coefficient psi0 indiquée de la norme européenne
// pour les bâtiment et les charges d'exploitation
// Si la valeur de type ne correspond pas à une catégorie, la fonction renvoie -1.
// type : categorie de durée d'utilisation de projet
double _1990_coef_psi0_bat_eu(int type);
double _1990_coef_psi1_bat_eu(int type);
double _1990_coef_psi2_bat_eu(int type);
// _1990_coef_psi0_bat_exp_fr : renvoie le coefficient psi0 indiquée de la norme française
// pour les bâtiment et les charges d'exploitation
double _1990_coef_psi0_bat_fr(int type);
double _1990_coef_psi1_bat_fr(int type);
double _1990_coef_psi2_bat_fr(int type);

double _1990_coef_psi0_bat(int type, int pays);
double _1990_coef_psi1_bat(int type, int pays);
double _1990_coef_psi2_bat(int type, int pays);

#endif
