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
#include <stdio.h>
#include <stdlib.h>
#include <libintl.h>
#include <locale.h>

#include <list.h>

#include "1990_coef_psi.h"
#include "common_erreurs.h"
#include "common_maths.h"
#define	TEST_BUG 0


int main(void)
{
	setlocale( LC_ALL, "" );
	bindtextdomain(PACKAGE, LOCALEDIR);
	textdomain(PACKAGE);
	
	// Test 1990_coef_psi.c
	
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi0_bat_eu(0))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi0_bat_eu(1))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat_eu(2))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat_eu(3))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat_eu(4))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat_eu(5))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(1.0, _1990_coef_psi0_bat_eu(6))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat_eu(7))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat_eu(8))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi0_bat_eu(9))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat_eu(10))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat_eu(11))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.5, _1990_coef_psi0_bat_eu(12))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.6, _1990_coef_psi0_bat_eu(13))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.6, _1990_coef_psi0_bat_eu(14))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi0_bat_eu(15))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi0_bat_eu(16))) BUG(-1);
	
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi1_bat_eu(0))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi1_bat_eu(1))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.5, _1990_coef_psi1_bat_eu(2))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.5, _1990_coef_psi1_bat_eu(3))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi1_bat_eu(4))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi1_bat_eu(5))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.9, _1990_coef_psi1_bat_eu(6))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi1_bat_eu(7))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.5, _1990_coef_psi1_bat_eu(8))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi1_bat_eu(9))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.5, _1990_coef_psi1_bat_eu(10))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.5, _1990_coef_psi1_bat_eu(11))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.2, _1990_coef_psi1_bat_eu(12))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.2, _1990_coef_psi1_bat_eu(13))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.5, _1990_coef_psi1_bat_eu(14))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi1_bat_eu(15))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi1_bat_eu(16))) BUG(-1);
	
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi2_bat_eu(0))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi2_bat_eu(1))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.3, _1990_coef_psi2_bat_eu(2))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.3, _1990_coef_psi2_bat_eu(3))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.6, _1990_coef_psi2_bat_eu(4))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.6, _1990_coef_psi2_bat_eu(5))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.8, _1990_coef_psi2_bat_eu(6))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.6, _1990_coef_psi2_bat_eu(7))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.3, _1990_coef_psi2_bat_eu(8))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi2_bat_eu(9))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.2, _1990_coef_psi2_bat_eu(10))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.2, _1990_coef_psi2_bat_eu(11))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi2_bat_eu(12))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi2_bat_eu(13))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi2_bat_eu(14))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi2_bat_eu(15))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi2_bat_eu(16))) BUG(-1);
	
	
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi0_bat_fr(0))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi0_bat_fr(1))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat_fr(2))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat_fr(3))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat_fr(4))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat_fr(5))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(1.0, _1990_coef_psi0_bat_fr(6))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat_fr(7))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat_fr(8))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi0_bat_fr(9))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat_fr(10))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat_fr(11))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(1.0, _1990_coef_psi0_bat_fr(12))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat_fr(13))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat_fr(14))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat_fr(15))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.5, _1990_coef_psi0_bat_fr(16))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.6, _1990_coef_psi0_bat_fr(17))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.6, _1990_coef_psi0_bat_fr(18))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi0_bat_fr(19))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi0_bat_fr(20))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi0_bat_fr(21))) BUG(-1);
	
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi1_bat_fr(0))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi1_bat_fr(1))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.5, _1990_coef_psi1_bat_fr(2))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.5, _1990_coef_psi1_bat_fr(3))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi1_bat_fr(4))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi1_bat_fr(5))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.9, _1990_coef_psi1_bat_fr(6))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi1_bat_fr(7))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.5, _1990_coef_psi1_bat_fr(8))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi1_bat_fr(9))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.5, _1990_coef_psi1_bat_fr(10))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi1_bat_fr(11))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.9, _1990_coef_psi1_bat_fr(12))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.5, _1990_coef_psi1_bat_fr(13))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.5, _1990_coef_psi1_bat_fr(14))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.5, _1990_coef_psi1_bat_fr(15))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.2, _1990_coef_psi1_bat_fr(16))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.2, _1990_coef_psi1_bat_fr(17))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.5, _1990_coef_psi1_bat_fr(18))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi1_bat_fr(19))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi1_bat_fr(20))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi1_bat_fr(21))) BUG(-1);
	
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi2_bat_fr(0))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi2_bat_fr(1))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.3, _1990_coef_psi2_bat_fr(2))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.3, _1990_coef_psi2_bat_fr(3))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.6, _1990_coef_psi2_bat_fr(4))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.6, _1990_coef_psi2_bat_fr(5))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.8, _1990_coef_psi2_bat_fr(6))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.6, _1990_coef_psi2_bat_fr(7))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.3, _1990_coef_psi2_bat_fr(8))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi2_bat_fr(9))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.3, _1990_coef_psi2_bat_fr(10))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.6, _1990_coef_psi2_bat_fr(11))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.5, _1990_coef_psi2_bat_fr(12))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.3, _1990_coef_psi2_bat_fr(13))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.2, _1990_coef_psi2_bat_fr(14))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.2, _1990_coef_psi2_bat_fr(15))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi2_bat_fr(16))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi2_bat_fr(17))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi2_bat_fr(18))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi2_bat_fr(19))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi2_bat_fr(20))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi2_bat_fr(21))) BUG(-1);
	
	#if TEST_BUG == 1
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi0_bat_eu(17))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi1_bat_eu(17))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi2_bat_eu(17))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi0_bat_fr(22))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi1_bat_fr(22))) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi2_bat_fr(22))) BUG(-1);
	#endif
	
	return EXIT_SUCCESS;
}
