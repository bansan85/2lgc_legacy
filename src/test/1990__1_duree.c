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
	
	setlocale( LC_ALL, "" );
	bindtextdomain(PACKAGE, LOCALEDIR);
	textdomain(PACKAGE);
	
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
	
	return EXIT_SUCCESS;
}
