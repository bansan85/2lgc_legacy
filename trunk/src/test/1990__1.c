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

#include "1990_duree.h"
#include "1990_coef_psi.h"
#include "1990_actions.h"
#include "maths.h"
#include "erreurs.h"

int main(void)
{
	int	i;
	
	setlocale( LC_ALL, "" );
	bindtextdomain(PACKAGE, LOCALEDIR);
	textdomain(PACKAGE);
	
	// TEST INDEPENDANT 1
	
	if (_1990_duree_projet_eu(-150) != -1) BUG;
	if (_1990_duree_projet_eu(0) != -1) BUG;
	if (_1990_duree_projet_eu(1) != 10) BUG;
	if (_1990_duree_projet_eu(2) != 25) BUG;
	if (_1990_duree_projet_eu(3) != 30) BUG;
	if (_1990_duree_projet_eu(4) != 50) BUG;
	if (_1990_duree_projet_eu(5) != 100) BUG;
	if (_1990_duree_projet_eu(6) != -1) BUG;
	if (_1990_duree_projet_eu(15690) != -1) BUG;
	
	if (_1990_duree_projet_fr(-150) != -1) BUG;
	if (_1990_duree_projet_fr(0) != -1) BUG;
	if (_1990_duree_projet_fr(1) != 10) BUG;
	if (_1990_duree_projet_fr(2) != 25) BUG;
	if (_1990_duree_projet_fr(3) != 25) BUG;
	if (_1990_duree_projet_fr(4) != 50) BUG;
	if (_1990_duree_projet_fr(5) != 100) BUG;
	if (_1990_duree_projet_fr(6) != -1) BUG;
	if (_1990_duree_projet_fr(15690) != -1) BUG;

	if (_1990_duree_projet_txt_eu(-150) != NULL) BUG;
	if (_1990_duree_projet_txt_eu(0) != NULL) BUG;
	for (i=1;i<=5;i++)
		if (_1990_duree_projet_txt_eu(i) == NULL) BUG;
	if (_1990_duree_projet_txt_eu(6) != NULL) BUG;
	if (_1990_duree_projet_txt_eu(15690) != NULL) BUG;
	
	if (_1990_duree_projet_txt_fr(-150) != NULL) BUG;
	if (_1990_duree_projet_txt_fr(0) != NULL) BUG;
	for (i=1;i<=5;i++)
		if (_1990_duree_projet_txt_fr(i) == NULL) BUG;
	if (_1990_duree_projet_txt_fr(6) != NULL) BUG;
	if (_1990_duree_projet_txt_fr(15690) != NULL) BUG;
	
	// TEST INDEPENDANT 2
	
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi0_bat_eu(-150), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi0_bat_eu(0), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi0_bat_eu(1), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi0_bat_eu(2), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat_eu(3), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat_eu(4), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat_eu(5), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat_eu(6), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(1.0, _1990_coef_psi0_bat_eu(7), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat_eu(8), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat_eu(9), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi0_bat_eu(10), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat_eu(11), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat_eu(12), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.5, _1990_coef_psi0_bat_eu(13), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.6, _1990_coef_psi0_bat_eu(14), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.6, _1990_coef_psi0_bat_eu(15), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi0_bat_eu(16), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi0_bat_eu(17), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi0_bat_eu(15690), ERREUR_RELATIVE_MIN)) BUG;
	
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi1_bat_eu(-150), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi1_bat_eu(0), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi1_bat_eu(1), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi1_bat_eu(2), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.5, _1990_coef_psi1_bat_eu(3), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.5, _1990_coef_psi1_bat_eu(4), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi1_bat_eu(5), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi1_bat_eu(6), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.9, _1990_coef_psi1_bat_eu(7), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi1_bat_eu(8), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.5, _1990_coef_psi1_bat_eu(9), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi1_bat_eu(10), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.5, _1990_coef_psi1_bat_eu(11), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.5, _1990_coef_psi1_bat_eu(12), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.2, _1990_coef_psi1_bat_eu(13), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.2, _1990_coef_psi1_bat_eu(14), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.5, _1990_coef_psi1_bat_eu(15), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi1_bat_eu(16), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi1_bat_eu(17), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi1_bat_eu(15690), ERREUR_RELATIVE_MIN)) BUG;
	
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi2_bat_eu(-150), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi2_bat_eu(0), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi2_bat_eu(1), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi2_bat_eu(2), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.3, _1990_coef_psi2_bat_eu(3), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.3, _1990_coef_psi2_bat_eu(4), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.6, _1990_coef_psi2_bat_eu(5), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.6, _1990_coef_psi2_bat_eu(6), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.8, _1990_coef_psi2_bat_eu(7), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.6, _1990_coef_psi2_bat_eu(8), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.3, _1990_coef_psi2_bat_eu(9), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi2_bat_eu(10), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.2, _1990_coef_psi2_bat_eu(11), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.2, _1990_coef_psi2_bat_eu(12), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi2_bat_eu(13), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi2_bat_eu(14), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi2_bat_eu(15), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi2_bat_eu(16), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi2_bat_eu(17), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi2_bat_eu(15690), ERREUR_RELATIVE_MIN)) BUG;
	
	if (_1990_action_categorie_bat_txt_eu(-150) != NULL) BUG;
	if (_1990_action_categorie_bat_txt_eu(0) != NULL) BUG;
	for (i=1;i<=16;i++)
		if (_1990_action_categorie_bat_txt_eu(i) == NULL) BUG;
	if (_1990_action_categorie_bat_txt_eu(17) != NULL) BUG;
	if (_1990_action_categorie_bat_txt_eu(15690) != NULL) BUG;
	
	
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi0_bat_fr(-150), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi0_bat_fr(0), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi0_bat_fr(1), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi0_bat_fr(2), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat_fr(3), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat_fr(4), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat_fr(5), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat_fr(6), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(1.0, _1990_coef_psi0_bat_fr(7), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat_fr(8), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat_fr(9), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi0_bat_fr(10), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat_fr(11), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat_fr(12), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(1.0, _1990_coef_psi0_bat_fr(13), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat_fr(14), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat_fr(15), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat_fr(16), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.5, _1990_coef_psi0_bat_fr(17), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.6, _1990_coef_psi0_bat_fr(18), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.6, _1990_coef_psi0_bat_fr(19), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi0_bat_fr(20), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi0_bat_fr(21), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi0_bat_fr(22), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi0_bat_fr(15690), ERREUR_RELATIVE_MIN)) BUG;
	
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi1_bat_fr(-150), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi1_bat_fr(0), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi1_bat_fr(1), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi1_bat_fr(2), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.5, _1990_coef_psi1_bat_fr(3), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.5, _1990_coef_psi1_bat_fr(4), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi1_bat_fr(5), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi1_bat_fr(6), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.9, _1990_coef_psi1_bat_fr(7), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi1_bat_fr(8), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.5, _1990_coef_psi1_bat_fr(9), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi1_bat_fr(10), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.5, _1990_coef_psi1_bat_fr(11), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi1_bat_fr(12), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.9, _1990_coef_psi1_bat_fr(13), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.5, _1990_coef_psi1_bat_fr(14), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.5, _1990_coef_psi1_bat_fr(15), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.5, _1990_coef_psi1_bat_fr(16), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.2, _1990_coef_psi1_bat_fr(17), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.2, _1990_coef_psi1_bat_fr(18), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.5, _1990_coef_psi1_bat_fr(19), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi1_bat_fr(20), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi1_bat_fr(21), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi1_bat_fr(22), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi1_bat_fr(15690), ERREUR_RELATIVE_MIN)) BUG;
	
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi2_bat_fr(-150), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi2_bat_fr(0), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi2_bat_fr(1), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi2_bat_fr(2), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.3, _1990_coef_psi2_bat_fr(3), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.3, _1990_coef_psi2_bat_fr(4), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.6, _1990_coef_psi2_bat_fr(5), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.6, _1990_coef_psi2_bat_fr(6), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.8, _1990_coef_psi2_bat_fr(7), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.6, _1990_coef_psi2_bat_fr(8), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.3, _1990_coef_psi2_bat_fr(9), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi2_bat_fr(10), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.3, _1990_coef_psi2_bat_fr(11), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.6, _1990_coef_psi2_bat_fr(12), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.5, _1990_coef_psi2_bat_fr(13), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.3, _1990_coef_psi2_bat_fr(14), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.2, _1990_coef_psi2_bat_fr(15), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.2, _1990_coef_psi2_bat_fr(16), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi2_bat_fr(17), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi2_bat_fr(18), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi2_bat_fr(19), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi2_bat_fr(20), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi2_bat_fr(21), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi2_bat_fr(22), ERREUR_RELATIVE_MIN)) BUG;
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi2_bat_fr(15690), ERREUR_RELATIVE_MIN)) BUG;
	
	if (_1990_action_categorie_bat_txt_fr(-150) != NULL) BUG;
	if (_1990_action_categorie_bat_txt_fr(0) != NULL) BUG;
	for (i=1;i<=21;i++)
		if (_1990_action_categorie_bat_txt_fr(i) == NULL) BUG;
	if (_1990_action_categorie_bat_txt_fr(22) != NULL) BUG;
	if (_1990_action_categorie_bat_txt_fr(15690) != NULL) BUG;
	
	if (_1990_action_type_combinaison_bat_eu(-150) != -1) BUG;
	if (_1990_action_type_combinaison_bat_eu(0) != -1) BUG;
	if (_1990_action_type_combinaison_bat_eu(1) != 1) BUG;
	if (_1990_action_type_combinaison_bat_eu(2) != 2) BUG;
	if (_1990_action_type_combinaison_bat_eu(3) != 3) BUG;
	if (_1990_action_type_combinaison_bat_eu(4) != 3) BUG;
	if (_1990_action_type_combinaison_bat_eu(5) != 3) BUG;
	if (_1990_action_type_combinaison_bat_eu(6) != 3) BUG;
	if (_1990_action_type_combinaison_bat_eu(7) != 3) BUG;
	if (_1990_action_type_combinaison_bat_eu(8) != 3) BUG;
	if (_1990_action_type_combinaison_bat_eu(9) != 3) BUG;
	if (_1990_action_type_combinaison_bat_eu(10) != 3) BUG;
	if (_1990_action_type_combinaison_bat_eu(11) != 3) BUG;
	if (_1990_action_type_combinaison_bat_eu(12) != 3) BUG;
	if (_1990_action_type_combinaison_bat_eu(13) != 3) BUG;
	if (_1990_action_type_combinaison_bat_eu(14) != 3) BUG;
	if (_1990_action_type_combinaison_bat_eu(15) != 3) BUG;
	if (_1990_action_type_combinaison_bat_eu(16) != 4) BUG;
	if (_1990_action_type_combinaison_bat_eu(17) != -1) BUG;
	if (_1990_action_type_combinaison_bat_eu(15690) != -1) BUG;
	
	if (_1990_action_type_combinaison_bat_fr(-150) != -1) BUG;
	if (_1990_action_type_combinaison_bat_fr(0) != -1) BUG;
	if (_1990_action_type_combinaison_bat_fr(1) != 1) BUG;
	if (_1990_action_type_combinaison_bat_fr(2) != 2) BUG;
	if (_1990_action_type_combinaison_bat_fr(3) != 3) BUG;
	if (_1990_action_type_combinaison_bat_fr(4) != 3) BUG;
	if (_1990_action_type_combinaison_bat_fr(5) != 3) BUG;
	if (_1990_action_type_combinaison_bat_fr(6) != 3) BUG;
	if (_1990_action_type_combinaison_bat_fr(7) != 3) BUG;
	if (_1990_action_type_combinaison_bat_fr(8) != 3) BUG;
	if (_1990_action_type_combinaison_bat_fr(9) != 3) BUG;
	if (_1990_action_type_combinaison_bat_fr(10) != 3) BUG;
	if (_1990_action_type_combinaison_bat_fr(11) != 3) BUG;
	if (_1990_action_type_combinaison_bat_fr(12) != 3) BUG;
	if (_1990_action_type_combinaison_bat_fr(13) != 3) BUG;
	if (_1990_action_type_combinaison_bat_fr(14) != 3) BUG;
	if (_1990_action_type_combinaison_bat_fr(15) != 3) BUG;
	if (_1990_action_type_combinaison_bat_fr(16) != 3) BUG;
	if (_1990_action_type_combinaison_bat_fr(17) != 3) BUG;
	if (_1990_action_type_combinaison_bat_fr(18) != 3) BUG;
	if (_1990_action_type_combinaison_bat_fr(19) != 3) BUG;
	if (_1990_action_type_combinaison_bat_fr(20) != 4) BUG;
	if (_1990_action_type_combinaison_bat_fr(21) != 5) BUG;
	if (_1990_action_type_combinaison_bat_fr(22) != -1) BUG;
	if (_1990_action_type_combinaison_bat_fr(15690) != -1) BUG;
	
	
	return EXIT_SUCCESS;
}
