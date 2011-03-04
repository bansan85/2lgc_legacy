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

#include "1990_duree.h"
#include "1990_coef_psi.h"
#include "1990_actions.h"
#include "1990_groupes.h"
#include "maths.h"
#include "erreurs.h"
#include "projet.h"
#define	TEST_BUG 0


int main(void)
{
	int	i;
	Projet	*projet;
	
	setlocale( LC_ALL, "" );
	bindtextdomain(PACKAGE, LOCALEDIR);
	textdomain(PACKAGE);
	
	// Test 1990_actions.c
	
	for (i=0;i<=16;i++)
		if (_1990_action_categorie_bat_txt_eu(i) == NULL) BUG(-1);
	
	for (i=0;i<=21;i++)
		if (_1990_action_categorie_bat_txt_fr(i) == NULL) BUG(-1);
	
	if (_1990_action_type_combinaison_bat_eu(0) != 0) BUG(-1);
	if (_1990_action_type_combinaison_bat_eu(1) != 1) BUG(-1);
	for (i=2;i<=14;i++)
		if (_1990_action_type_combinaison_bat_eu(i) != 2) BUG(-1);
	if (_1990_action_type_combinaison_bat_eu(15) != 3) BUG(-1);
	if (_1990_action_type_combinaison_bat_eu(16) != 4) BUG(-1);
	
	if (_1990_action_type_combinaison_bat_fr(0) != 0) BUG(-1);
	if (_1990_action_type_combinaison_bat_fr(1) != 1) BUG(-1);
	for (i=2;i<=18;i++)
		if (_1990_action_type_combinaison_bat_fr(i) != 2) BUG(-1);
	if (_1990_action_type_combinaison_bat_fr(19) != 3) BUG(-1);
	if (_1990_action_type_combinaison_bat_fr(20) != 4) BUG(-1);
	if (_1990_action_type_combinaison_bat_fr(21) != 5) BUG(-1);
	
	#if TEST_BUG == 1
	if (_1990_action_categorie_bat_txt_eu(17) != NULL) BUG(-1);
	if (_1990_action_categorie_bat_txt_fr(22) != NULL) BUG(-1);
	if (_1990_action_type_combinaison_bat_eu(17) != -1) BUG(-1);
	if (_1990_action_type_combinaison_bat_fr(22) != -1) BUG(-1);
	#endif
	
	// Test 1990_coef_psi.c
	
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi0_bat_eu(0), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi0_bat_eu(1), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat_eu(2), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat_eu(3), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat_eu(4), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat_eu(5), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(1.0, _1990_coef_psi0_bat_eu(6), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat_eu(7), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat_eu(8), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi0_bat_eu(9), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat_eu(10), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat_eu(11), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.5, _1990_coef_psi0_bat_eu(12), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.6, _1990_coef_psi0_bat_eu(13), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.6, _1990_coef_psi0_bat_eu(14), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi0_bat_eu(15), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi0_bat_eu(16), ERREUR_RELATIVE_MIN)) BUG(-1);
	
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi1_bat_eu(0), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi1_bat_eu(1), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.5, _1990_coef_psi1_bat_eu(2), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.5, _1990_coef_psi1_bat_eu(3), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi1_bat_eu(4), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi1_bat_eu(5), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.9, _1990_coef_psi1_bat_eu(6), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi1_bat_eu(7), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.5, _1990_coef_psi1_bat_eu(8), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi1_bat_eu(9), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.5, _1990_coef_psi1_bat_eu(10), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.5, _1990_coef_psi1_bat_eu(11), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.2, _1990_coef_psi1_bat_eu(12), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.2, _1990_coef_psi1_bat_eu(13), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.5, _1990_coef_psi1_bat_eu(14), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi1_bat_eu(15), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi1_bat_eu(16), ERREUR_RELATIVE_MIN)) BUG(-1);
	
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi2_bat_eu(0), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi2_bat_eu(1), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.3, _1990_coef_psi2_bat_eu(2), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.3, _1990_coef_psi2_bat_eu(3), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.6, _1990_coef_psi2_bat_eu(4), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.6, _1990_coef_psi2_bat_eu(5), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.8, _1990_coef_psi2_bat_eu(6), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.6, _1990_coef_psi2_bat_eu(7), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.3, _1990_coef_psi2_bat_eu(8), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi2_bat_eu(9), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.2, _1990_coef_psi2_bat_eu(10), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.2, _1990_coef_psi2_bat_eu(11), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi2_bat_eu(12), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi2_bat_eu(13), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi2_bat_eu(14), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi2_bat_eu(15), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi2_bat_eu(16), ERREUR_RELATIVE_MIN)) BUG(-1);
	
	
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi0_bat_fr(0), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi0_bat_fr(1), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat_fr(2), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat_fr(3), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat_fr(4), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat_fr(5), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(1.0, _1990_coef_psi0_bat_fr(6), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat_fr(7), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat_fr(8), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi0_bat_fr(9), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat_fr(10), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat_fr(11), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(1.0, _1990_coef_psi0_bat_fr(12), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat_fr(13), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat_fr(14), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi0_bat_fr(15), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.5, _1990_coef_psi0_bat_fr(16), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.6, _1990_coef_psi0_bat_fr(17), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.6, _1990_coef_psi0_bat_fr(18), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi0_bat_fr(19), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi0_bat_fr(20), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi0_bat_fr(21), ERREUR_RELATIVE_MIN)) BUG(-1);
	
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi1_bat_fr(0), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi1_bat_fr(1), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.5, _1990_coef_psi1_bat_fr(2), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.5, _1990_coef_psi1_bat_fr(3), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi1_bat_fr(4), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi1_bat_fr(5), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.9, _1990_coef_psi1_bat_fr(6), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi1_bat_fr(7), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.5, _1990_coef_psi1_bat_fr(8), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi1_bat_fr(9), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.5, _1990_coef_psi1_bat_fr(10), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.7, _1990_coef_psi1_bat_fr(11), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.9, _1990_coef_psi1_bat_fr(12), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.5, _1990_coef_psi1_bat_fr(13), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.5, _1990_coef_psi1_bat_fr(14), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.5, _1990_coef_psi1_bat_fr(15), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.2, _1990_coef_psi1_bat_fr(16), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.2, _1990_coef_psi1_bat_fr(17), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.5, _1990_coef_psi1_bat_fr(18), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi1_bat_fr(19), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi1_bat_fr(20), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi1_bat_fr(21), ERREUR_RELATIVE_MIN)) BUG(-1);
	
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi2_bat_fr(0), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi2_bat_fr(1), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.3, _1990_coef_psi2_bat_fr(2), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.3, _1990_coef_psi2_bat_fr(3), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.6, _1990_coef_psi2_bat_fr(4), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.6, _1990_coef_psi2_bat_fr(5), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.8, _1990_coef_psi2_bat_fr(6), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.6, _1990_coef_psi2_bat_fr(7), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.3, _1990_coef_psi2_bat_fr(8), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi2_bat_fr(9), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.3, _1990_coef_psi2_bat_fr(10), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.6, _1990_coef_psi2_bat_fr(11), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.5, _1990_coef_psi2_bat_fr(12), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.3, _1990_coef_psi2_bat_fr(13), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.2, _1990_coef_psi2_bat_fr(14), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.2, _1990_coef_psi2_bat_fr(15), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi2_bat_fr(16), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi2_bat_fr(17), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(0.0, _1990_coef_psi2_bat_fr(18), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi2_bat_fr(19), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi2_bat_fr(20), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi2_bat_fr(21), ERREUR_RELATIVE_MIN)) BUG(-1);
	
	#if TEST_BUG == 1
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi0_bat_eu(17), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi1_bat_eu(17), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi2_bat_eu(17), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi0_bat_fr(22), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi1_bat_fr(22), ERREUR_RELATIVE_MIN)) BUG(-1);
	if (!ERREUR_RELATIVE_EGALE(-1., _1990_coef_psi2_bat_fr(22), ERREUR_RELATIVE_MIN)) BUG(-1);
	#endif
	
	// Test 1990_duree.c
	
	if (_1990_duree_projet_eu(0) != 10) BUG(-1);
	if (_1990_duree_projet_eu(1) != 25) BUG(-1);
	if (_1990_duree_projet_eu(2) != 30) BUG(-1);
	if (_1990_duree_projet_eu(3) != 50) BUG(-1);
	if (_1990_duree_projet_eu(4) != 100) BUG(-1);
	
	if (_1990_duree_projet_fr(0) != 10) BUG(-1);
	if (_1990_duree_projet_fr(1) != 25) BUG(-1);
	if (_1990_duree_projet_fr(2) != 25) BUG(-1);
	if (_1990_duree_projet_fr(3) != 50) BUG(-1);
	if (_1990_duree_projet_fr(4) != 100) BUG(-1);
	
	for (i=0;i<=4;i++)
		if (_1990_duree_projet_txt_eu(i) == NULL) BUG(-1);
	
	for (i=0;i<=4;i++)
		if (_1990_duree_projet_txt_fr(i) == NULL) BUG(-1);
	
	#if TEST_BUG == 1
	if (_1990_duree_projet_eu(5) != -1) BUG(-1);
	if (_1990_duree_projet_fr(5) != -1) BUG(-1);
	if (_1990_duree_projet_txt_eu(5) != NULL) BUG(-1);
	if (_1990_duree_projet_txt_fr(5) != NULL) BUG(-1);
	#endif
	
	// Combinaison
	
	projet = NULL;
	projet = projet_init();
	projet->pays = 1;
	if (projet == NULL) BUG(-1);
	if (_1990_action_ajout(projet, 0) != 0) BUG(-1);
	if (_1990_action_ajout(projet, 1) != 0) BUG(-1);
	if (_1990_action_ajout(projet, 2) != 0) BUG(-1);
	if (_1990_action_ajout(projet, 3) != 0) BUG(-1);
	if (_1990_action_ajout(projet, 4) != 0) BUG(-1);
	if (_1990_action_ajout(projet, 5) != 0) BUG(-1);
	if (_1990_action_ajout(projet, 19) != 0) BUG(-1);
	if (_1990_action_ajout(projet, 20) != 0) BUG(-1);
	_1990_action_affiche_tout(projet);
	if (_1990_groupe_etage_init(projet) != 0) BUG(-1);
	if (_1990_groupe_etage_ajout(projet, 0) != 0) BUG(-1);
	if (_1990_groupe_etage_ajout(projet, 1) != 0) BUG(-1);
	if (_1990_groupe_ajout(projet, 0, 0, GROUPE_COMBINAISON_AND) != 0) BUG(-1);
	if (_1990_groupe_ajout(projet, 0, 1, GROUPE_COMBINAISON_XOR) != 0) BUG(-1);
	if (_1990_groupe_ajout(projet, 0, 2, GROUPE_COMBINAISON_OR) != 0) BUG(-1);
	if (_1990_groupe_ajout(projet, 1, 0, GROUPE_COMBINAISON_OR) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 0, 0, 0) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 0, 0, 1) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 0, 0, 2) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 0, 1, 3) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 0, 1, 4) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 0, 2, 5) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 0, 2, 6) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 0, 2, 7) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 1, 0, 0) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 1, 0, 1) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 1, 0, 2) != 0) BUG(-1);
	if (_1990_combinaisons_genere(projet) != 0) BUG(-1);
	_1990_groupe_affiche_tout(projet);
	projet_free(projet);
	
	return EXIT_SUCCESS;
}
