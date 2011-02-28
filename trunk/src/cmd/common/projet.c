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

#include <stdio.h>
#include <stdlib.h>
#include <libintl.h>
#include <locale.h>

#include "projet.h"

Projet *projet_init()
{
	Projet *projet;
	projet = (Projet*)malloc(sizeof(Projet));
	if (projet == NULL)
		return NULL;
	projet->actions = NULL;
	projet->groupes = NULL;
	projet->pays = 0;
	return projet;
}

int projet_free(Projet *projet)
{
	if (projet->actions != NULL)
		_1990_action_free(projet);
	if (projet->groupes != NULL)
	{
		list_free(projet->groupes, LIST_DEALLOC);
		projet->groupes = NULL;
	}
	return 0;
}
