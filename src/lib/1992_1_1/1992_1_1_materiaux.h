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

#include "common_projet.h"

typedef struct __Beton_Materiau
{
	int			numero;
	
	// Caractéristique du matériau béton conformément à EN 1992_1_1, Tableau 3.1
	double			fck;
	double			fckcube;
	double			fcm;
	double			fctm;
	double			fctk_0_05;
	double			fctk_0_95;
	double			ecm;
	double			ec1;
	double			ecu1;
	double			ec2;
	double			ecu2;
	double			n;
	double			ec3;
	double			ecu3;
	
	double			nu;
	double			gnu_0_2;
	double			gnu_0_0;
} Beton_Materiau;

int _1992_1_1_materiaux_init(Projet *projet);
int _1992_1_1_materiaux_ajout(Projet *projet, double fck, double nu);
int _1992_1_1_materiaux_free(Projet *projet);


#endif
