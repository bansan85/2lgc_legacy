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
#include <libintl.h>
#include <locale.h>

// _1990_duree_projet_txt_eu : renvoie la description des catérogies des durées indicative de la norme européenne
// Renvoie NULL si la catégorie n'existe pas
// type : catégorie de durée d'utilisation de projet
char *_1990_action_categorie_bat_txt_eu(int type)
{
	switch(type)
	{
		case 1 : { return gettext("Permanente"); break; }
		case 2 : { return gettext("Précontrainte"); break; }
		case 3 : { return gettext("Exploitation : Catégorie A : habitation, zones résidentielles"); break; }
		case 4 : { return gettext("Exploitation : Catégorie B : bureaux"); break; }
		case 5 : { return gettext("Exploitation : Catégorie C : lieux de réunion"); break; }
		case 6 : { return gettext("Exploitation : Catégorie D : commerces"); break; }
		case 7 : { return gettext("Exploitation : Catégorie E : stockage"); break; }
		case 8 : { return gettext("Exploitation : Catégorie F : zone de trafic, véhicules de poids inférieur à 30 kN"); break; }
		case 9 : { return gettext("Exploitation : Catégorie G : zone de trafic, véhicules de poids entre 30 kN et 160 kN"); break; }
		case 10 : { return gettext("Exploitation : Catégorie H : toits"); break; }
		case 11 : { return gettext("Neige : Finlande, Islande, Norvège, Suède"); break; }
		case 12 : { return gettext("Neige : Autres états membres CEN, altitude > 1000 m"); break; }
		case 13 : { return gettext("Neige : Autres états membres CEN, altitude <= 1000 m"); break; }
		case 14 : { return gettext("Vent"); break; }
		case 15 : { return gettext("Température (hors incendie)"); break; }
		case 16 : { return gettext("Accidentelle"); break; }
		default : { return NULL; break; }
	}
}

// _1990_duree_projet_txt_fr : renvoie la description des catérogies des durées indicative de la norme européenne
// Renvoie NULL si la catégorie n'existe pas
// type : catégorie de durée d'utilisation de projet
char *_1990_action_categorie_bat_txt_fr(int type)
{
	switch(type)
	{
		case 1 : { return gettext("Permanente"); break; }
		case 2 : { return gettext("Précontrainte"); break; }
		case 3 : { return gettext("Exploitation : Catégorie A : habitation, zones résidentielles"); break; }
		case 4 : { return gettext("Exploitation : Catégorie B : bureaux"); break; }
		case 5 : { return gettext("Exploitation : Catégorie C : lieux de réunion"); break; }
		case 6 : { return gettext("Exploitation : Catégorie D : commerces"); break; }
		case 7 : { return gettext("Exploitation : Catégorie E : stockage"); break; }
		case 8 : { return gettext("Exploitation : Catégorie F : zone de trafic, véhicules de poids inférieur à 30 kN"); break; }
		case 9 : { return gettext("Exploitation : Catégorie G : zone de trafic, véhicules de poids entre 30 kN et 160 kN"); break; }
		case 10 : { return gettext("Exploitation : Catégorie H : toits d'un bâtiment de catégorie A ou B"); break; }
		case 11 : { return gettext("Exploitation : Catégorie I : toitures accessibles avec locaux des catégories A ou B"); break; }
		case 12 : { return gettext("Exploitation : Catégorie I : toitures accessibles avec locaux des catégories C ou D"); break; }
		case 13 : { return gettext("Exploitation : Catégorie K : Hélicoptère sur la toiture"); break; }
		case 14 : { return gettext("Exploitation : Catégorie K : Hélicoptère sur la toiture, autres charges (fret, personnel, accessoires ou équipements divers)"); break; }
		case 15 : { return gettext("Neige : Saint-Pierre-et-Miquelon"); break; }
		case 16 : { return gettext("Neige : Altitude > 1000 m"); break; }
		case 17 : { return gettext("Neige : Altitude <= 1000 m"); break; }
		case 18 : { return gettext("Vent"); break; }
		case 19 : { return gettext("Température (hors incendie)"); break; }
		case 20 : { return gettext("Accidentelle"); break; }
		case 21 : { return gettext("Eaux souterraines"); break; }
		default : { return NULL; break; }
	}
}

// _1990_action_type_combinaison_bat_eu : renvoie le type d'une action pour les combinaisons d'action des bâtiments selon la norme européenne
// Renvoie	: -1 si la catégorie n'existe pas
//		: 1 si Action permanente
//		: 2 si action précontrainte
//		: 3 si action variable
//		: 4 si accidentelle
// type : catégorie de durée d'utilisation de projet
int _1990_action_type_combinaison_bat_eu(int categorie)
{
	if (categorie == 1)
		return 1;
	else if (categorie == 2)
		return 2;
	else if ((3 <= categorie) && (categorie <= 15))
		return 3;
	else if (categorie == 16)
		return 4;
	else
		return -1;
}

// _1990_action_type_combinaison_bat_fr : renvoie le type d'une action pour les combinaisons d'action des bâtiments selon la norme française
// Renvoie	: -1 si la catégorie n'existe pas
//		: 1 si Action permanente
//		: 2 si action précontrainte
//		: 3 si action variable
//		: 4 si accidentelle
//		: 5 si eaux souterraines
int _1990_action_type_combinaison_bat_fr(int categorie)
{
	if (categorie == 1)
		return 1;
	else if (categorie == 2)
		return 2;
	else if ((3 <= categorie) && (categorie <= 19))
		return 3;
	else if (categorie == 20)
		return 4;
	else if (categorie == 21)
		return 5;
	else
		return -1;
}
