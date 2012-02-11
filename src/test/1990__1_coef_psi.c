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

#include "1990_coef_psi.h"
#include "common_erreurs.h"
#include "common_maths.h"


int main(void)
{
    setlocale( LC_ALL, "" );
    bindtextdomain(PACKAGE, LOCALEDIR);
    textdomain(PACKAGE);
    
    // Test 1990_coef_psi.c
    
    BUG(ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi0_bat(0, PAYS_EU)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi0_bat(1, PAYS_EU)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat(2, PAYS_EU)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat(3, PAYS_EU)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat(4, PAYS_EU)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat(5, PAYS_EU)), -1);
    BUG(ERREUR_RELATIVE_EGALE(1.0, _1990_coef_psi0_bat(6, PAYS_EU)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat(7, PAYS_EU)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat(8, PAYS_EU)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi0_bat(9, PAYS_EU)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat(10, PAYS_EU)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat(11, PAYS_EU)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.5, _1990_coef_psi0_bat(12, PAYS_EU)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.6, _1990_coef_psi0_bat(13, PAYS_EU)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.6, _1990_coef_psi0_bat(14, PAYS_EU)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi0_bat(15, PAYS_EU)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi0_bat(16, PAYS_EU)), -1);
    
    BUG(ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi1_bat(0, PAYS_EU)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi1_bat(1, PAYS_EU)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.5, _1990_coef_psi1_bat(2, PAYS_EU)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.5, _1990_coef_psi1_bat(3, PAYS_EU)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi1_bat(4, PAYS_EU)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi1_bat(5, PAYS_EU)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.9, _1990_coef_psi1_bat(6, PAYS_EU)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi1_bat(7, PAYS_EU)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.5, _1990_coef_psi1_bat(8, PAYS_EU)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi1_bat(9, PAYS_EU)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.5, _1990_coef_psi1_bat(10, PAYS_EU)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.5, _1990_coef_psi1_bat(11, PAYS_EU)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.2, _1990_coef_psi1_bat(12, PAYS_EU)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.2, _1990_coef_psi1_bat(13, PAYS_EU)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.5, _1990_coef_psi1_bat(14, PAYS_EU)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi1_bat(15, PAYS_EU)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi1_bat(16, PAYS_EU)), -1);
    
    BUG(ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi2_bat(0, PAYS_EU)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi2_bat(1, PAYS_EU)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.3, _1990_coef_psi2_bat(2, PAYS_EU)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.3, _1990_coef_psi2_bat(3, PAYS_EU)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.6, _1990_coef_psi2_bat(4, PAYS_EU)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.6, _1990_coef_psi2_bat(5, PAYS_EU)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.8, _1990_coef_psi2_bat(6, PAYS_EU)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.6, _1990_coef_psi2_bat(7, PAYS_EU)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.3, _1990_coef_psi2_bat(8, PAYS_EU)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi2_bat(9, PAYS_EU)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.2, _1990_coef_psi2_bat(10, PAYS_EU)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.2, _1990_coef_psi2_bat(11, PAYS_EU)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi2_bat(12, PAYS_EU)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi2_bat(13, PAYS_EU)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi2_bat(14, PAYS_EU)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi2_bat(15, PAYS_EU)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi2_bat(16, PAYS_EU)), -1);
    
    
    BUG(ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi0_bat(0, PAYS_FR)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi0_bat(1, PAYS_FR)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat(2, PAYS_FR)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat(3, PAYS_FR)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat(4, PAYS_FR)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat(5, PAYS_FR)), -1);
    BUG(ERREUR_RELATIVE_EGALE(1.0, _1990_coef_psi0_bat(6, PAYS_FR)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat(7, PAYS_FR)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat(8, PAYS_FR)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi0_bat(9, PAYS_FR)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat(10, PAYS_FR)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat(11, PAYS_FR)), -1);
    BUG(ERREUR_RELATIVE_EGALE(1.0, _1990_coef_psi0_bat(12, PAYS_FR)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat(13, PAYS_FR)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat(14, PAYS_FR)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat(15, PAYS_FR)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.5, _1990_coef_psi0_bat(16, PAYS_FR)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.6, _1990_coef_psi0_bat(17, PAYS_FR)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.6, _1990_coef_psi0_bat(18, PAYS_FR)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi0_bat(19, PAYS_FR)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi0_bat(20, PAYS_FR)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi0_bat(21, PAYS_FR)), -1);
    
    BUG(ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi1_bat(0, PAYS_FR)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi1_bat(1, PAYS_FR)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.5, _1990_coef_psi1_bat(2, PAYS_FR)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.5, _1990_coef_psi1_bat(3, PAYS_FR)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi1_bat(4, PAYS_FR)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi1_bat(5, PAYS_FR)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.9, _1990_coef_psi1_bat(6, PAYS_FR)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi1_bat(7, PAYS_FR)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.5, _1990_coef_psi1_bat(8, PAYS_FR)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi1_bat(9, PAYS_FR)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.5, _1990_coef_psi1_bat(10, PAYS_FR)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi1_bat(11, PAYS_FR)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.9, _1990_coef_psi1_bat(12, PAYS_FR)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.5, _1990_coef_psi1_bat(13, PAYS_FR)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.5, _1990_coef_psi1_bat(14, PAYS_FR)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.5, _1990_coef_psi1_bat(15, PAYS_FR)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.2, _1990_coef_psi1_bat(16, PAYS_FR)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.2, _1990_coef_psi1_bat(17, PAYS_FR)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.5, _1990_coef_psi1_bat(18, PAYS_FR)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi1_bat(19, PAYS_FR)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi1_bat(20, PAYS_FR)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi1_bat(21, PAYS_FR)), -1);
    
    BUG(ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi2_bat(0, PAYS_FR)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi2_bat(1, PAYS_FR)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.3, _1990_coef_psi2_bat(2, PAYS_FR)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.3, _1990_coef_psi2_bat(3, PAYS_FR)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.6, _1990_coef_psi2_bat(4, PAYS_FR)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.6, _1990_coef_psi2_bat(5, PAYS_FR)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.8, _1990_coef_psi2_bat(6, PAYS_FR)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.6, _1990_coef_psi2_bat(7, PAYS_FR)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.3, _1990_coef_psi2_bat(8, PAYS_FR)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi2_bat(9, PAYS_FR)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.3, _1990_coef_psi2_bat(10, PAYS_FR)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.6, _1990_coef_psi2_bat(11, PAYS_FR)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.5, _1990_coef_psi2_bat(12, PAYS_FR)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.3, _1990_coef_psi2_bat(13, PAYS_FR)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.2, _1990_coef_psi2_bat(14, PAYS_FR)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.2, _1990_coef_psi2_bat(15, PAYS_FR)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi2_bat(16, PAYS_FR)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi2_bat(17, PAYS_FR)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi2_bat(18, PAYS_FR)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi2_bat(19, PAYS_FR)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi2_bat(20, PAYS_FR)), -1);
    BUG(ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi2_bat(21, PAYS_FR)), -1);
    
/*    BUG(ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi0_bat(17, PAYS_EU)), -1);
    BUG(ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi2_bat(17, PAYS_EU)), -1);
    BUG(ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi2_bat(17, PAYS_EU)), -1);
    BUG(ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi0_bat(22, PAYS_FR)), -1);
    BUG(ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi1_bat(22, PAYS_FR)), -1);
    BUG(ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi2_bat(22, PAYS_FR)), -1);*/
    
    return EXIT_SUCCESS;
}
