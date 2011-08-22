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
#include "common_erreurs.h"
#include "common_projet.h"
#define TEST_BUG 0


int main(void)
{
    Projet  *projet;
    
    setlocale( LC_ALL, "" );
    bindtextdomain(PACKAGE, LOCALEDIR);
    textdomain(PACKAGE);
    
    // Combinaison
    
    projet = projet_init(PAYS_EU);
    BUG(projet, -1);
    projet->combinaisons.flags = 0 | 8 | 2;
    BUG(_1990_action_ajout(projet, 0) == 0, -1); // Poids prore
    BUG(_1990_action_ajout(projet, 2) == 0, -1); // 2
    BUG(_1990_action_ajout(projet, 12) == 0, -1); // 3
    BUG(_1990_action_ajout(projet, 13) == 0, -1); // 4
    _1990_action_affiche_tout(projet);
    BUG(_1990_groupe_ajout_niveau(projet) == 0, -1);
    BUG(_1990_groupe_ajout_niveau(projet) == 0, -1);
    BUG(_1990_groupe_ajout_niveau(projet) == 0, -1);
    BUG(_1990_groupe_ajout_niveau(projet) == 0, -1);
    BUG(_1990_groupe_ajout_groupe(projet, 0, GROUPE_COMBINAISON_AND) == 0, -1); // Charges permanentes
    BUG(_1990_groupe_ajout_groupe(projet, 0, GROUPE_COMBINAISON_AND) == 0, -1); // Charges d'exploitation
    BUG(_1990_groupe_ajout_groupe(projet, 0, GROUPE_COMBINAISON_XOR) == 0, -1); // Neige
    BUG(_1990_groupe_ajout_groupe(projet, 0, GROUPE_COMBINAISON_XOR) == 0, -1); // Vent
    BUG(_1990_groupe_ajout_groupe(projet, 1, GROUPE_COMBINAISON_AND) == 0, -1); // Charges permanentes
    BUG(_1990_groupe_ajout_groupe(projet, 1, GROUPE_COMBINAISON_AND) == 0, -1); // Charges d'exploitation
    BUG(_1990_groupe_ajout_groupe(projet, 1, GROUPE_COMBINAISON_OR) == 0, -1); // Charges climatiques
    BUG(_1990_groupe_ajout_groupe(projet, 2, GROUPE_COMBINAISON_AND) == 0, -1); // Tout
    BUG(_1990_groupe_ajout_groupe(projet, 2, GROUPE_COMBINAISON_OR) == 0, -1); // Tout
    BUG(_1990_groupe_ajout_groupe(projet, 3, GROUPE_COMBINAISON_AND) == 0, -1); // Tout
    BUG(_1990_groupe_ajout_element(projet, 0, 0, 0) == 0, -1);
    BUG(_1990_groupe_ajout_element(projet, 0, 1, 1) == 0, -1);
    BUG(_1990_groupe_ajout_element(projet, 0, 2, 2) == 0, -1);
    BUG(_1990_groupe_ajout_element(projet, 0, 3, 3) == 0, -1);
    BUG(_1990_groupe_ajout_element(projet, 1, 0, 0) == 0, -1);
    BUG(_1990_groupe_ajout_element(projet, 1, 1, 1) == 0, -1);
    BUG(_1990_groupe_ajout_element(projet, 1, 2, 2) == 0, -1);
    BUG(_1990_groupe_ajout_element(projet, 1, 2, 3) == 0, -1);
    BUG(_1990_groupe_ajout_element(projet, 2, 0, 0) == 0, -1);
    BUG(_1990_groupe_ajout_element(projet, 2, 1, 1) == 0, -1);
    BUG(_1990_groupe_ajout_element(projet, 2, 1, 2) == 0, -1);
    BUG(_1990_groupe_ajout_element(projet, 3, 0, 0) == 0, -1);
    BUG(_1990_groupe_ajout_element(projet, 3, 0, 1) == 0, -1);
    BUG(_1990_combinaisons_genere(projet) == 0, -1);
    _1990_groupe_affiche_tout(projet);
    _1990_ponderations_affiche_tout(projet);
    projet_free(projet);

    return EXIT_SUCCESS;
}
