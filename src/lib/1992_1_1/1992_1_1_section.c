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

#include <stdlib.h>
#include <libintl.h>
#include "common_projet.h"
#include "common_erreurs.h"

int _1992_1_1_sections_init(Projet *projet)
{
	if (projet == NULL)
		BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
	
	projet->beton.sections = list_init();
	if (projet->beton.sections == NULL)
		BUGTEXTE(-2, gettext("Erreur d'allocation mémoire.\n"));
	else
		return 0;
}

int _1992_1_1_sections_free(Projet *projet)
{
	if ((projet == NULL) || (projet->beton.sections == NULL))
		BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
	
	while (!list_empty(projet->beton.sections))
	{
		free(list_remove_front(projet->beton.sections));
	}
	
	free(projet->beton.sections);
	projet->beton.sections = NULL;
	
	return 0;
}
