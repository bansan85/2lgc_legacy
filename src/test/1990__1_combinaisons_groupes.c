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
	projet->pays = 1;
	if (projet == NULL) BUG(-1);
	if (_1990_action_ajout(projet, 0) != 0) BUG(-1);
	if (_1990_action_ajout(projet, 1) != 0) BUG(-1);
	if (_1990_action_ajout(projet, 2) != 0) BUG(-1);
	if (_1990_action_ajout(projet, 3) != 0) BUG(-1);
	if (_1990_action_ajout(projet, 4) != 0) BUG(-1);
	if (_1990_action_ajout(projet, 5) != 0) BUG(-1);
	if (_1990_action_ajout(projet, 5) != 0) BUG(-1);
	if (_1990_action_ajout(projet, 5) != 0) BUG(-1);
	if (_1990_action_ajout(projet, 5) != 0) BUG(-1);
	if (_1990_action_ajout(projet, 5) != 0) BUG(-1);
	if (_1990_action_ajout(projet, 5) != 0) BUG(-1);
	if (_1990_action_ajout(projet, 5) != 0) BUG(-1);
	if (_1990_action_ajout(projet, 5) != 0) BUG(-1);
	if (_1990_action_ajout(projet, 5) != 0) BUG(-1);
	if (_1990_action_ajout(projet, 5) != 0) BUG(-1);
	if (_1990_action_ajout(projet, 5) != 0) BUG(-1);
	if (_1990_action_ajout(projet, 5) != 0) BUG(-1);
	if (_1990_action_ajout(projet, 5) != 0) BUG(-1);
	if (_1990_action_ajout(projet, 5) != 0) BUG(-1);
	if (_1990_action_ajout(projet, 5) != 0) BUG(-1);
	if (_1990_action_ajout(projet, 5) != 0) BUG(-1);
	if (_1990_action_ajout(projet, 5) != 0) BUG(-1);
	if (_1990_action_ajout(projet, 5) != 0) BUG(-1);
	if (_1990_action_ajout(projet, 5) != 0) BUG(-1);
	if (_1990_action_ajout(projet, 5) != 0) BUG(-1);
	if (_1990_action_ajout(projet, 19) != 0) BUG(-1);
	if (_1990_action_ajout(projet, 20) != 0) BUG(-1);
	_1990_action_affiche_tout(projet);
	if (_1990_groupe_etage_ajout(projet, 0) != 0) BUG(-1);
	if (_1990_groupe_etage_ajout(projet, 1) != 0) BUG(-1);
	if (_1990_groupe_etage_ajout(projet, 2) != 0) BUG(-1);
	if (_1990_groupe_ajout(projet, 0, 0, GROUPE_COMBINAISON_AND) != 0) BUG(-1);
	if (_1990_groupe_ajout(projet, 0, 1, GROUPE_COMBINAISON_XOR) != 0) BUG(-1);
	if (_1990_groupe_ajout(projet, 0, 2, GROUPE_COMBINAISON_OR) != 0) BUG(-1);
	if (_1990_groupe_ajout(projet, 0, 3, GROUPE_COMBINAISON_AND) != 0) BUG(-1);
	if (_1990_groupe_ajout(projet, 0, 4, GROUPE_COMBINAISON_XOR) != 0) BUG(-1);
	if (_1990_groupe_ajout(projet, 0, 5, GROUPE_COMBINAISON_OR) != 0) BUG(-1);
	if (_1990_groupe_ajout(projet, 0, 6, GROUPE_COMBINAISON_AND) != 0) BUG(-1);
	if (_1990_groupe_ajout(projet, 0, 7, GROUPE_COMBINAISON_XOR) != 0) BUG(-1);
	if (_1990_groupe_ajout(projet, 0, 8, GROUPE_COMBINAISON_OR) != 0) BUG(-1);
	if (_1990_groupe_ajout(projet, 1, 0, GROUPE_COMBINAISON_XOR) != 0) BUG(-1);
	if (_1990_groupe_ajout(projet, 1, 1, GROUPE_COMBINAISON_OR) != 0) BUG(-1);
	if (_1990_groupe_ajout(projet, 1, 2, GROUPE_COMBINAISON_AND) != 0) BUG(-1);
	if (_1990_groupe_ajout(projet, 2, 0, GROUPE_COMBINAISON_OR) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 0, 0, 0) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 0, 0, 1) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 0, 0, 2) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 0, 1, 3) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 0, 1, 4) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 0, 1, 5) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 0, 2, 6) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 0, 2, 7) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 0, 2, 8) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 0, 3, 9) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 0, 3, 10) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 0, 3, 11) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 0, 4, 12) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 0, 4, 13) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 0, 4, 14) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 0, 5, 15) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 0, 5, 16) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 0, 5, 17) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 0, 6, 18) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 0, 6, 19) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 0, 6, 20) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 0, 7, 21) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 0, 7, 22) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 0, 7, 23) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 0, 8, 24) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 0, 8, 25) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 0, 8, 26) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 1, 0, 0) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 1, 0, 1) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 1, 0, 2) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 1, 1, 3) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 1, 1, 4) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 1, 1, 5) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 1, 2, 6) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 1, 2, 7) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 1, 2, 8) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 2, 0, 0) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 2, 0, 1) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 2, 0, 2) != 0) BUG(-1);
	if (_1990_combinaisons_genere(projet) != 0) BUG(-1);
	_1990_groupe_affiche_tout(projet);
	projet_free(projet);
	
	return EXIT_SUCCESS;
}
