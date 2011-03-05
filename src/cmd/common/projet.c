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
#include "erreurs.h"
#include "1990_actions.h"
#include "1990_groupes.h"
#include "1990_combinaisons.h"

Projet *projet_init()
{
	Projet *projet;
	projet = (Projet*)malloc(sizeof(Projet));
	if (projet == NULL)
		BUG(NULL);
	if (_1990_action_init(projet) != 0)
	{
		free(projet);
		BUG(NULL);
	}
	if (_1990_groupe_init(projet) != 0)
	{
		_1990_action_free(projet);
		free(projet);
		BUG(NULL);
	}
	if (_1990_combinaisons_init(projet) != 0)
	{
		_1990_groupe_free(projet);
		_1990_action_free(projet);
		free(projet);
		BUG(NULL);
	}
	
	projet->pays = 0;
	return projet;
}

int projet_free(Projet *projet)
{
	if (projet->actions != NULL)
		_1990_action_free(projet);
	if (projet->groupes != NULL)
		_1990_groupe_free(projet);
	if (projet->combinaisons.elu_equ != NULL)
		_1990_combinaisons_free(projet);
	free(projet);
	return 0;
}
