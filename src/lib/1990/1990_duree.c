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
#include "erreurs.h"
#include <libintl.h>
#include <locale.h>

// _1990_duree_projet_eu : renvoie la durée (en année) indicative de la norme européenne
// Dans le cas où la durée indiquée par la norme européenne est un
// intervalle d'année, la durée la plus importante est renvoyée.
// Si la valeur de type ne correspond pas à une catégorie, la fonction renvoie -1.
// type : categorie de durée d'utilisation de projet
int _1990_duree_projet_eu(int type)
{
	switch (type)
	{
		case 0 : { return 10; break; }
		case 1 : { return 25; break; }
		case 2 : { return 30; break; }
		case 3 : { return 50; break; }
		case 4 : { return 100; break; }
		default : { BUG(-1); break; }
	}
}

// _1990_duree_projet_fr : renvoie la durée (en année) indicative de la norme française
// Dans le cas où la durée indiquée par la norme européenne est un
// intervalle d'année, la durée la plus importante est renvoyée.
// Si la valeur de type ne correspond pas à une catégorie, la fonction renvoie -1.
// type : categorie de durée d'utilisation de projet
int _1990_duree_projet_fr(int type)
{
	switch (type)
	{
		case 0 : { return 10; break; }
		case 1 : { return 25; break; }
		case 2 : { return 25; break; }
		case 3 : { return 50; break; }
		case 4 : { return 100; break; }
		default : { BUG(-1); break; }
	}
}


// _1990_duree_projet_txt_eu : renvoie la description des catérogies des durées indicative de la norme européenne
// Renvoie NULL si la catégorie n'existe pas
// type : catégorie de durée d'utilisation de projet
char *_1990_duree_projet_txt_eu(int type)
{
	switch(type)
	{
		case 0 : { return gettext("Structures provisoires"); break; }
		case 1 : { return gettext("Éléments structuraux remplaçables, par exemple poutres de roulement, appareils d'appui"); break; }
		case 2 : { return gettext("Structures agricoles et similaires"); break; }
		case 3 : { return gettext("Structures de bâtiments et autres structures courantes"); break; }
		case 4 : { return gettext("Structures monumentales de bâtiments, pont et autres ouvrages de génie civil"); break; }
		default : { BUG(NULL); break; }
	}
}

// _1990_duree_projet_txt_fr : renvoie la description des catérogies des durées indicative de la norme française
char *_1990_duree_projet_txt_fr(int type)
{
	switch(type)
	{
		case 0 : { return gettext("Structures provisoires"); break; }
		case 1 : { return gettext("Éléments structuraux remplaçables, par exemple poutres de roulement, appareils d'appui"); break; }
		case 2 : { return gettext("Structures agricoles et similaires"); break; }
		case 3 : { return gettext("Structures de bâtiments et autres structures courantes"); break; }
		case 4 : { return gettext("Structures monumentales de bâtiments, pont et autres ouvrages de génie civil"); break; }
		default : { BUG(NULL); break; }
	}
}
