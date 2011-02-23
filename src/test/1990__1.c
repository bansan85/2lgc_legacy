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

int main(void)
{
	
	setlocale( LC_ALL, "" );
	bindtextdomain(PACKAGE, LOCALEDIR);
	textdomain(PACKAGE);
	
	if (duree_projet_eu(-150) != -1)
		return -1;
	if (duree_projet_eu(0) != -1)
		return -1;
	if (duree_projet_eu(1) != 10)
		return -1;
	if (duree_projet_eu(2) != 25)
		return -1;
	if (duree_projet_eu(3) != 30)
		return -1;
	if (duree_projet_eu(4) != 50)
		return -1;
	if (duree_projet_eu(5) != 100)
		return -1;
	if (duree_projet_eu(6) != -1)
		return -1;
	if (duree_projet_eu(15690) != -1)
		return -1;
	
	if (duree_projet_fr(-150) != -1)
		return -1;
	if (duree_projet_fr(0) != -1)
		return -1;
	if (duree_projet_fr(1) != 10)
		return -1;
	if (duree_projet_fr(2) != 25)
		return -1;
	if (duree_projet_fr(3) != 25)
		return -1;
	if (duree_projet_fr(4) != 50)
		return -1;
	if (duree_projet_fr(5) != 100)
		return -1;
	if (duree_projet_fr(6) != -1)
		return -1;
	if (duree_projet_fr(15690) != -1)
		return -1;

	return EXIT_SUCCESS;;
}

