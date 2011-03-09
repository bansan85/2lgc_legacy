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

#include "config.h"
#include <libintl.h>
#include <locale.h>
#include "common_erreurs.h"

#include "common_projet.h"

// _1990_coef_psi0_bat_eu : renvoie le coefficient psi0 indiquée de la norme européenne
// pour les bâtiment et les charges d'exploitation
// Si la valeur de type ne correspond pas à une catégorie, la fonction renvoie -1.
// type : categorie de durée d'utilisation de projet
double _1990_coef_psi0_bat_eu(int type)
{
	switch (type)
	{
		case 0 : { return -1.; break; }
		case 1 : { return -1.; break; }
		case 2 : { return 0.7; break; }
		case 3 : { return 0.7; break; }
		case 4 : { return 0.7; break; }
		case 5 : { return 0.7; break; }
		case 6 : { return 1.0; break; }
		case 7 : { return 0.7; break; }
		case 8 : { return 0.7; break; }
		case 9 : { return 0.0; break; }
		case 10 : { return 0.7; break; }
		case 11 : { return 0.7; break; }
		case 12 : { return 0.5; break; }
		case 13 : { return 0.6; break; }
		case 14 : { return 0.6; break; }
		case 15 : { return -1.; break; }
		case 16 : { return -1.; break; }
		default : { BUG(-1.); break; }
	}
}
double _1990_coef_psi1_bat_eu(int type)
{
	switch (type)
	{
		case 0 : { return -1.; break; }
		case 1 : { return -1.; break; }
		case 2 : { return 0.5; break; }
		case 3 : { return 0.5; break; }
		case 4 : { return 0.7; break; }
		case 5 : { return 0.7; break; }
		case 6 : { return 0.9; break; }
		case 7 : { return 0.7; break; }
		case 8 : { return 0.5; break; }
		case 9 : { return 0.0; break; }
		case 10 : { return 0.5; break; }
		case 11 : { return 0.5; break; }
		case 12 : { return 0.2; break; }
		case 13 : { return 0.2; break; }
		case 14 : { return 0.5; break; }
		case 15 : { return -1.; break; }
		case 16 : { return -1.; break; }
		default : { BUG(-1.); break; }
	}
}
double _1990_coef_psi2_bat_eu(int type)
{
	switch (type)
	{
		case 0 : { return -1.; break; }
		case 1 : { return -1.; break; }
		case 2 : { return 0.3; break; }
		case 3 : { return 0.3; break; }
		case 4 : { return 0.6; break; }
		case 5 : { return 0.6; break; }
		case 6 : { return 0.8; break; }
		case 7 : { return 0.6; break; }
		case 8 : { return 0.3; break; }
		case 9 : { return 0.0; break; }
		case 10 : { return 0.2; break; }
		case 11 : { return 0.2; break; }
		case 12 : { return 0.0; break; }
		case 13 : { return 0.0; break; }
		case 14 : { return 0.0; break; }
		case 15 : { return -1.; break; }
		case 16 : { return -1.; break; }
		default : { BUG(-1.); break; }
	}
}

// _1990_coef_psi0_bat_fr : renvoie le coefficient psi0 indiquée de la norme française
// pour les bâtiment et les charges d'exploitation
double _1990_coef_psi0_bat_fr(int type)
{
	switch (type)
	{
		case 0 : { return -1.; break; }
		case 1 : { return -1.; break; }
		case 2 : { return 0.7; break; }
		case 3 : { return 0.7; break; }
		case 4 : { return 0.7; break; }
		case 5 : { return 0.7; break; }
		case 6 : { return 1.0; break; }
		case 7 : { return 0.7; break; }
		case 8 : { return 0.7; break; }
		case 9 : { return 0.0; break; }
		case 10 : { return 0.7; break; }
		case 11 : { return 0.7; break; }
		case 12 : { return 1.0; break; }
		case 13 : { return 0.7; break; }
		case 14 : { return 0.7; break; }
		case 15 : { return 0.7; break; }
		case 16 : { return 0.5; break; }
		case 17 : { return 0.6; break; }
		case 18 : { return 0.6; break; }
		case 19 : { return -1.; break; }
		case 20 : { return -1.; break; }
		case 21 : { return -1.; break; }
		default : { BUG(-1.); break; }
	}
}
double _1990_coef_psi1_bat_fr(int type)
{
	switch (type)
	{
		case 0 : { return -1.; break; }
		case 1 : { return -1.; break; }
		case 2 : { return 0.5; break; }
		case 3 : { return 0.5; break; }
		case 4 : { return 0.7; break; }
		case 5 : { return 0.7; break; }
		case 6 : { return 0.9; break; }
		case 7 : { return 0.7; break; }
		case 8 : { return 0.5; break; }
		case 9 : { return 0.0; break; }
		case 10 : { return 0.5; break; }
		case 11 : { return 0.7; break; }
		case 12 : { return 0.9; break; }
		case 13 : { return 0.5; break; }
		case 14 : { return 0.5; break; }
		case 15 : { return 0.5; break; }
		case 16 : { return 0.2; break; }
		case 17 : { return 0.2; break; }
		case 18 : { return 0.5; break; }
		case 19 : { return -1.; break; }
		case 20 : { return -1.; break; }
		case 21 : { return -1.; break; }
		default : { BUG(-1.); break; }
	}
}
double _1990_coef_psi2_bat_fr(int type)
{
	switch (type)
	{
		case 0 : { return -1.; break; }
		case 1 : { return -1.; break; }
		case 2 : { return 0.3; break; }
		case 3 : { return 0.3; break; }
		case 4 : { return 0.6; break; }
		case 5 : { return 0.6; break; }
		case 6 : { return 0.8; break; }
		case 7 : { return 0.6; break; }
		case 8 : { return 0.3; break; }
		case 9 : { return 0.0; break; }
		case 10 : { return 0.3; break; }
		case 11 : { return 0.6; break; }
		case 12 : { return 0.5; break; }
		case 13 : { return 0.3; break; }
		case 14 : { return 0.2; break; }
		case 15 : { return 0.2; break; }
		case 16 : { return 0.0; break; }
		case 17 : { return 0.0; break; }
		case 18 : { return 0.0; break; }
		case 19 : { return -1.; break; }
		case 20 : { return -1.; break; }
		case 21 : { return -1.; break; }
		default : { BUG(-1.); break; }
	}
}

double _1990_coef_psi0_bat(int type, int pays)
{
	switch (pays)
	{
		case PAYS_EU : { return _1990_coef_psi0_bat_eu(type); break; }
		case PAYS_FR : { return _1990_coef_psi0_bat_fr(type); break; }
		default : { BUG(-1.); break; }
	}
}

double _1990_coef_psi1_bat(int type, int pays)
{
	switch (pays)
	{
		case PAYS_EU : { return _1990_coef_psi1_bat_eu(type); break; }
		case PAYS_FR : { return _1990_coef_psi1_bat_fr(type); break; }
		default : { BUG(-1.); break; }
	}
}

double _1990_coef_psi2_bat(int type, int pays)
{
	switch (pays)
	{
		case PAYS_EU : { return _1990_coef_psi2_bat_eu(type); break; }
		case PAYS_FR : { return _1990_coef_psi2_bat_fr(type); break; }
		default : { BUG(-1.); break; }
	}
}
