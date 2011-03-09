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

#include "1990_actions.h"
#include "1990_groupes.h"
#include "1990_combinaisons.h"
#include "1990_ponderations.h"
#include "erreurs.h"
#include "projet.h"
#define	TEST_BUG 0


int main(void)
{
	Projet	*projet;
	
	setlocale( LC_ALL, "" );
	bindtextdomain(PACKAGE, LOCALEDIR);
	textdomain(PACKAGE);
	
	// Combinaison
	
	projet = NULL;
	projet = projet_init();
	projet->pays = PAYS_EU;
	projet->combinaisons.flags = 0 | 8 | 2;
	if (projet == NULL) BUG(-1);
	if (_1990_action_ajout(projet, 0) != 0) BUG(-1); // 0
	if (_1990_action_ajout(projet, 0) != 0) BUG(-1); // 1
	if (_1990_action_ajout(projet, 2) != 0) BUG(-1); // 2
	if (_1990_action_ajout(projet, 2) != 0) BUG(-1); // 3
	if (_1990_action_ajout(projet, 10) != 0) BUG(-1); // 4
	if (_1990_action_ajout(projet, 13) != 0) BUG(-1); // 5
	_1990_action_affiche_tout(projet);
	if (_1990_groupe_etage_ajout(projet, 0) != 0) BUG(-1);
	if (_1990_groupe_etage_ajout(projet, 1) != 0) BUG(-1);
	if (_1990_groupe_etage_ajout(projet, 2) != 0) BUG(-1);
	if (_1990_groupe_ajout(projet, 0, 0, GROUPE_COMBINAISON_AND) != 0) BUG(-1); // Charges permanentes
	if (_1990_groupe_ajout(projet, 0, 1, GROUPE_COMBINAISON_OR) != 0) BUG(-1); // Charges d'exploitation
	if (_1990_groupe_ajout(projet, 0, 2, GROUPE_COMBINAISON_XOR) != 0) BUG(-1); // Neige
	if (_1990_groupe_ajout(projet, 0, 3, GROUPE_COMBINAISON_XOR) != 0) BUG(-1); // Vent
	if (_1990_groupe_ajout(projet, 1, 0, GROUPE_COMBINAISON_AND) != 0) BUG(-1); // Charges permanentes
	if (_1990_groupe_ajout(projet, 1, 1, GROUPE_COMBINAISON_OR) != 0) BUG(-1); // Charges permanentes
	if (_1990_groupe_ajout(projet, 2, 0, GROUPE_COMBINAISON_AND) != 0) BUG(-1); // Tout
	if (_1990_groupe_ajout_element(projet, 0, 0, 0) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 0, 0, 1) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 0, 1, 2) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 0, 1, 3) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 0, 2, 4) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 0, 3, 5) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 1, 0, 0) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 1, 1, 1) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 1, 1, 2) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 1, 1, 3) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 2, 0, 0) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 2, 0, 1) != 0) BUG(-1);
	if (_1990_combinaisons_genere(projet) != 0) BUG(-1);
	_1990_groupe_affiche_tout(projet);
	_1990_ponderations_affiche_tout(projet);
	projet_free(projet);
	
	return EXIT_SUCCESS;
}
