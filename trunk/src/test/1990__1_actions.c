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
#include "erreurs.h"
#define	TEST_BUG 0


int main(void)
{
	int	i;
	
	setlocale( LC_ALL, "" );
	bindtextdomain(PACKAGE, LOCALEDIR);
	textdomain(PACKAGE);
	
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
	
	return EXIT_SUCCESS;
}
