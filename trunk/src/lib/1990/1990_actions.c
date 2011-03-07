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
#include <stdio.h>
#include <stdlib.h>

#include "1990_actions.h"
#include "projet.h"
#include "erreurs.h"

// _1990_duree_projet_txt_eu : renvoie la description des catérogies des durées indicative de la norme européenne
// Renvoie NULL si la catégorie n'existe pas
// type : catégorie de durée d'utilisation de projet
char *_1990_action_categorie_bat_txt_eu(int type)
{
	switch(type)
	{
		case 0 : { return gettext("Permanente"); break; }
		case 1 : { return gettext("Précontrainte"); break; }
		case 2 : { return gettext("Exploitation : Catégorie A : habitation, zones résidentielles"); break; }
		case 3 : { return gettext("Exploitation : Catégorie B : bureaux"); break; }
		case 4 : { return gettext("Exploitation : Catégorie C : lieux de réunion"); break; }
		case 5 : { return gettext("Exploitation : Catégorie D : commerces"); break; }
		case 6 : { return gettext("Exploitation : Catégorie E : stockage"); break; }
		case 7 : { return gettext("Exploitation : Catégorie F : zone de trafic, véhicules de poids inférieur à 30 kN"); break; }
		case 8 : { return gettext("Exploitation : Catégorie G : zone de trafic, véhicules de poids entre 30 kN et 160 kN"); break; }
		case 9 : { return gettext("Exploitation : Catégorie H : toits"); break; }
		case 10 : { return gettext("Neige : Finlande, Islande, Norvège, Suède"); break; }
		case 11 : { return gettext("Neige : Autres états membres CEN, altitude > 1000 m"); break; }
		case 12 : { return gettext("Neige : Autres états membres CEN, altitude <= 1000 m"); break; }
		case 13 : { return gettext("Vent"); break; }
		case 14 : { return gettext("Température (hors incendie)"); break; }
		case 15 : { return gettext("Accidentelle"); break; }
		case 16 : { return gettext("Sismique"); break; }
		default : { BUG(NULL); break; }
	}
}

// _1990_duree_projet_txt_fr : renvoie la description des catérogies des durées indicative de la norme européenne
// Renvoie NULL si la catégorie n'existe pas
// type : catégorie de durée d'utilisation de projet
char *_1990_action_categorie_bat_txt_fr(int type)
{
	switch(type)
	{
		case 0 : { return gettext("Permanente"); break; }
		case 1 : { return gettext("Précontrainte"); break; }
		case 2 : { return gettext("Exploitation : Catégorie A : habitation, zones résidentielles"); break; }
		case 3 : { return gettext("Exploitation : Catégorie B : bureaux"); break; }
		case 4 : { return gettext("Exploitation : Catégorie C : lieux de réunion"); break; }
		case 5 : { return gettext("Exploitation : Catégorie D : commerces"); break; }
		case 6 : { return gettext("Exploitation : Catégorie E : stockage"); break; }
		case 7 : { return gettext("Exploitation : Catégorie F : zone de trafic, véhicules de poids inférieur à 30 kN"); break; }
		case 8 : { return gettext("Exploitation : Catégorie G : zone de trafic, véhicules de poids entre 30 kN et 160 kN"); break; }
		case 9 : { return gettext("Exploitation : Catégorie H : toits d'un bâtiment de catégorie A ou B"); break; }
		case 10 : { return gettext("Exploitation : Catégorie I : toitures accessibles avec locaux des catégories A ou B"); break; }
		case 11 : { return gettext("Exploitation : Catégorie I : toitures accessibles avec locaux des catégories C ou D"); break; }
		case 12 : { return gettext("Exploitation : Catégorie K : Hélicoptère sur la toiture"); break; }
		case 13 : { return gettext("Exploitation : Catégorie K : Hélicoptère sur la toiture, autres charges (fret, personnel, accessoires ou équipements divers)"); break; }
		case 14 : { return gettext("Neige : Saint-Pierre-et-Miquelon"); break; }
		case 15 : { return gettext("Neige : Altitude > 1000 m"); break; }
		case 16 : { return gettext("Neige : Altitude <= 1000 m"); break; }
		case 17 : { return gettext("Vent"); break; }
		case 18 : { return gettext("Température (hors incendie)"); break; }
		case 19 : { return gettext("Accidentelle"); break; }
		case 20 : { return gettext("Sismique"); break; }
		case 21 : { return gettext("Eaux souterraines"); break; }
		default : { BUG(NULL); break; }
	}
}

char *_1990_action_categorie_bat_txt(int type, int pays)
{
	switch (pays)
	{
		case PAYS_EU : { return _1990_action_categorie_bat_txt_eu(type); break; }
		case PAYS_FR : { return _1990_action_categorie_bat_txt_fr(type); break; }
		default : { BUG(NULL); break; }
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
	if (categorie == 0) // Poids propre
		return 0;
	else if (categorie == 1) // Charges d'exploitation
		return 1;
	else if ((2 <= categorie) && (categorie <= 14))
		return 2;
	else if (categorie == 15) // accidentelle
		return 3;
	else if (categorie == 16) // sismique
		return 4;
	else
		BUG(-1);
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
	if (categorie == 0) // Poids propre
		return 0;
	else if (categorie == 1) // Précontrainte
		return 1;
	else if ((2 <= categorie) && (categorie <= 18)) // Charges d'exploitation
		return 2;
	else if (categorie == 19) // Accidentelle
		return 3;
	else if (categorie == 20) // Sismique
		return 4;
	else if (categorie == 21) // Eaux souterraines
		return 5;
	else
		BUG(-1);
}

int _1990_action_type_combinaison_bat(int categorie, int pays)
{
	switch (pays)
	{
		case PAYS_EU : { return _1990_action_type_combinaison_bat_eu(categorie); break; }
		case PAYS_FR : { return _1990_action_type_combinaison_bat_fr(categorie); break; }
		default : { BUG(-1); break; }
	}
}

// _1990_action_init : Initialise la liste des actions
int _1990_action_init(Projet *projet)
{
	projet->actions = list_init();
	if (projet->actions == NULL)
		BUG(-1);
	else
		return 0;
}

// _1990_action_nouveau_init : ajouter une nouvelle action à la liste des actions
// Renvoie -1 si le pays ou la catégorie n'est pas bon
// Renvoie -2 si l'insertion a échoué
// Renvoie 0 si tout va bien
int _1990_action_ajout(Projet *projet, int categorie)
{
	Action *action_dernier, action_nouveau;

	if (_1990_action_type_combinaison_bat(categorie, projet->pays) == -1)
		BUG(-1);
	list_mvrear(projet->actions);
	action_dernier = (Action *)list_rear(projet->actions);
	action_nouveau.nom = NULL;
	action_nouveau.description = NULL;
	action_nouveau.categorie = categorie;
	action_nouveau.flags = 0;
	if (action_dernier == NULL)
		action_nouveau.numero = 0;
	else
		action_nouveau.numero = action_dernier->numero+1;
	
	if (list_insert_after(projet->actions, &(action_nouveau), sizeof(action_nouveau)) == NULL)
		BUG(-2);
	
	return 0;
}

int _1990_action_cherche(void *input, void *curr)
{
	Action *action = (Action*)curr;
	int *numero = (int *)input;
	if (action->numero == (*numero))
		return FALSE;
	else
		return TRUE;
}

int _1990_action_cherche_et_positionne(Projet *projet, int numero)
{
	if (list_traverse(projet->actions, (void *)&(numero), _1990_action_cherche, LIST_ALTR) == LIST_OK)
		return 0;
	else
		BUG(-1);
}

int _1990_action_affiche(__attribute__((unused)) void *input, void *curr)
{
	Action *action = (Action*)curr;
	printf("%s %s %d %d\n", action->nom, action->description, action->numero, action->categorie);
	return TRUE;
}

void _1990_action_affiche_tout(Projet *projet)
{
	list_traverse(projet->actions, (void *)NULL, _1990_action_affiche, 0);
	return;
}

void _1990_action_free_free(void *data)
{
	Action *action = (Action*)data;
	if (action->nom != NULL)
		free(action->nom);
	if (action->description != NULL)
		free(action->description);
	free(action);
	return;
}

void _1990_action_free(Projet *projet)
{
	list_free(projet->actions, &(_1990_action_free_free));
	projet->actions = NULL;
	return;
}
