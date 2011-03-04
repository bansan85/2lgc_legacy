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
#include <list.h>
#include <stdlib.h>
#include <stdio.h>

#include "projet.h"
#include "erreurs.h"
#include "1990_actions.h"
#include "1990_groupes.h"
#include "1990_combinaisons.h"


int _1990_combinaisons_init(Projet *projet)
{
	projet->combinaisons.elu_equ = list_init();
	projet->combinaisons.elu_str = list_init();
	projet->combinaisons.elu_geo = list_init();
	projet->combinaisons.elu_fat = list_init();
	projet->combinaisons.elu_acc = list_init();
	projet->combinaisons.elu_sis = list_init();
	projet->combinaisons.els_car = list_init();
	projet->combinaisons.els_freq = list_init();
	projet->combinaisons.els_perm = list_init();
	if ((projet->combinaisons.elu_equ == NULL) || (projet->combinaisons.elu_str == NULL) || (projet->combinaisons.elu_geo == NULL) || (projet->combinaisons.elu_fat == NULL) || (projet->combinaisons.elu_acc == NULL) || (projet->combinaisons.elu_sis == NULL) || (projet->combinaisons.els_car == NULL) || (projet->combinaisons.els_freq == NULL) || (projet->combinaisons.els_perm == NULL))
		BUG(-1);
	else
		return 0;
}

/*int _1990_combinaisons_genere_eu(__attribute__((unused)) Projet *projet)
{
	int	*coef_min, *coef_max;
	coef_min = (int*)malloc(5*sizeof(double));
	if (coef_min == NULL)
		return -1;
	coef_max = (int*)malloc(5*sizeof(double));
	if (coef_max == NULL)
		return -2;
	
	// Pour ELU_EQU
	coef_min[0] = 0.9; coef_max[0] = 1.1; // poids propre
	coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
	coef_min[2] = 0.0; coef_max[2] = 1.5; // charge d'exploitation
	coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
	coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
	
	return 0;
}*/

int _1990_combinaisons_duplique_combinaisons(LIST *destination, LIST *source)
{
	Combinaison		*combinaison_source;
	Combinaison		combinaison_destination;
	Combinaison_Element	*element_source;
	Combinaison_Element	element_destination;
	
	if (list_size(source) == 0)
		return 0;
	list_mvrear(destination);
	list_mvfront(source);
	do
	{
		combinaison_source = list_curr(source);
		if (list_size(combinaison_source->elements) != 0)
		{
			combinaison_destination.elements = list_init();
			if (combinaison_destination.elements == NULL)
				BUG(-2);
			list_mvfront(combinaison_source->elements);
			do
			{
				element_source = list_curr(combinaison_source->elements);
				element_destination.action = element_source->action;
				if (list_insert_after(combinaison_destination.elements, (void*)&element_destination, sizeof(element_destination)) == NULL)
					BUG(-3);

			}
			while (list_mvnext(combinaison_source->elements) != NULL);
			if (list_insert_after(destination, (void*)&combinaison_destination, sizeof(combinaison_destination)) == NULL)
				BUG(-4);
		}
	}
	while (list_mvnext(source) != NULL);
	return 0;
}

int _1990_combinaisons_genere_groupe_xor(Projet *projet, Groupe *groupe)
{
	Groupe			*groupe_n_1;
	Etage_Groupe		*etage;
	int			etage_tmp;
	Element			*element_tmp;
	Combinaison		comb;
	Combinaison_Element	element;
	Element			*tmp;
	
	// Premier étage
	if (list_curr(projet->groupes) == list_front(projet->groupes))
	{
		if (list_empty(groupe->elements) == TRUE)
			BUG(-1);
		list_mvfront(groupe->elements);
		do
		{
			tmp = (Element*)list_curr(groupe->elements);
			if (_1990_action_cherche_et_positionne(projet, tmp->numero) != 0)
				BUG(-2);
			comb.elements = list_init();
			if (comb.elements == NULL)
				BUG(-3);
			element.action = list_curr(projet->actions);
			if (list_insert_after(comb.elements, (void*)&element, sizeof(element)) == NULL)
				BUG(-4);
			if (list_insert_after(groupe->tmp_combinaison.combinaisons, &(comb), sizeof(comb)) == NULL)
				BUG(-5);
		}
		while (list_mvnext(groupe->elements) != NULL);
	}
	else
	{
		etage = list_curr(projet->groupes);
		if (etage == NULL)
			BUG(-6);
		etage_tmp = etage->etage-1;
		if (list_empty(groupe->elements) == TRUE)
			BUG(-7);
		if (list_traverse(projet->groupes, (void *)&etage_tmp, _1990_groupe_etage_cherche, LIST_ALTR) != LIST_OK)
			BUG(-8);
		etage = list_curr(projet->groupes);
		if (etage == NULL)
			BUG(-9);
		list_mvfront(groupe->elements);
		do
		{
			element_tmp = (Element*)list_curr(groupe->elements);
			if (_1990_groupe_traverse_et_positionne(etage, element_tmp->numero) != 0)
				BUG(-10);
			groupe_n_1 = list_curr(etage->groupe);
			if (_1990_combinaisons_duplique_combinaisons(groupe->tmp_combinaison.combinaisons, groupe_n_1->tmp_combinaison.combinaisons) != 0)
				BUG(-11);
		}
		while (list_mvnext(groupe->elements) != NULL);
		
		etage_tmp = etage->etage;
		if (list_traverse(projet->groupes, (void *)&etage_tmp, _1990_groupe_etage_cherche, LIST_ALTR) != LIST_OK)
			BUG(-12);
	}
	return 0;
}

int _1990_combinaisons_ajout_combinaison(Combinaison *destination, Combinaison *source)
{
	Combinaison_Element	*element_source;
	Combinaison_Element	element_destination;
	
	if (list_size(source->elements) == 0)
		return 0;
	list_mvrear(destination->elements);
	list_mvfront(source->elements);
	do
	{
		element_source = list_curr(source->elements);
		element_destination.action = element_source->action;
		if (list_insert_after(destination->elements, (void*)&element_destination, sizeof(element_destination)) == NULL)
			BUG(-1);
	}
	while (list_mvnext(source->elements) != NULL);
	return 0;
}

int _1990_combinaisons_genere_groupe_and(Projet *projet, Groupe *groupe)
{
	Etage_Groupe		*etage;
	Groupe			*groupe_n_1;
	Combinaison		comb, *combinaison1, *combinaison2;
	Combinaison_Element	element;
	Element			*element_tmp;
	int			etage_tmp, i, j, nbboucle;
	LIST			*transition;
	
	if (list_size(projet->groupes) == 0)
		BUG(-1);
	
	// Premier étage
	if (list_curr(projet->groupes) == list_front(projet->groupes))
	{
		if (list_empty(groupe->elements) == TRUE)
			BUG(-2);
		comb.elements = list_init();
		if (comb.elements == NULL)
			BUG(-3);
		list_mvfront(groupe->elements);
		do
		{
			element_tmp = (Element*)list_curr(groupe->elements);
			if (_1990_action_cherche_et_positionne(projet, element_tmp->numero) != 0)
				BUG(-4);
			element.action = list_curr(projet->actions);
			if (list_insert_after(comb.elements, (void*)&element, sizeof(element)) == NULL)
				BUG(-5);
		}
		while (list_mvnext(groupe->elements) != NULL);
		if (list_insert_after(groupe->tmp_combinaison.combinaisons, &(comb), sizeof(comb)) == NULL)
			BUG(-6);
	}
	else
	{
		etage = list_curr(projet->groupes);
		etage_tmp = etage->etage-1;
		if (list_empty(groupe->elements) == TRUE)
			BUG(-7);
		if (list_traverse(projet->groupes, (void *)&etage_tmp, _1990_groupe_etage_cherche, LIST_ALTR) != LIST_OK)
			BUG(-8);
		etage = list_curr(projet->groupes);
		list_mvfront(groupe->elements);
		do
		{
			element_tmp = (Element*)list_curr(groupe->elements);
			if (_1990_groupe_traverse_et_positionne(etage, element_tmp->numero) != 0)
				BUG(-9);
			groupe_n_1 = list_curr(etage->groupe);
			if (list_front(groupe->elements) == element_tmp)
			{
				if (_1990_combinaisons_duplique_combinaisons(groupe->tmp_combinaison.combinaisons, groupe_n_1->tmp_combinaison.combinaisons) != 0)
					BUG(-10);
			}
			else
			{
				transition = list_init();
				if (transition == NULL)
					BUG(-11);
				if (_1990_combinaisons_duplique_combinaisons(transition, groupe->tmp_combinaison.combinaisons) != 0)
					BUG(-12);
				nbboucle = list_size(groupe_n_1->tmp_combinaison.combinaisons);
				for (i=1;i<=nbboucle;i++)
				{
					if (_1990_combinaisons_duplique_combinaisons(groupe->tmp_combinaison.combinaisons, transition) != 0)
						BUG(-13);
				}
				
				list_mvfront(groupe->tmp_combinaison.combinaisons);
				list_mvfront(groupe_n_1->tmp_combinaison.combinaisons);
				for (i=1;i<=nbboucle;i++)
				{
					combinaison2 = (Combinaison*)list_curr(groupe_n_1->tmp_combinaison.combinaisons);
					for (j=1;j<=list_size(transition);j++)
					{
						combinaison1 = (Combinaison*)list_curr(groupe->tmp_combinaison.combinaisons);
						if (_1990_combinaisons_ajout_combinaison(combinaison1, combinaison2) != 0)
							BUG(-14);
						list_mvnext(groupe->tmp_combinaison.combinaisons);
					}
					list_mvnext(groupe_n_1->tmp_combinaison.combinaisons);
				}
			}
		}
		while (list_mvnext(groupe->elements) != NULL);
		
		etage_tmp = etage->etage;
		if (list_traverse(projet->groupes, (void *)&etage_tmp, _1990_groupe_etage_cherche, LIST_ALTR) != LIST_OK)
			BUG(-15);
	}
	return 0;
}

int _1990_combinaisons_genere_groupe_or(Projet *projet, Groupe *groupe)
{
	Etage_Groupe		*etage;
	Groupe			*groupe_n_1;
	Combinaison		comb, *combinaison1, *combinaison2;
	Combinaisons		combs;
	Combinaison_Element	element;
	Element			*elem_tmp, *element_tmp;
	int			etage_tmp, boucle = 2, i, j, k, tmp;
	LIST			*transition;
	
	if (list_size(groupe->elements) == 0)
		BUG(-1);
	
	for (i=2;i<=list_size(groupe->elements);i++)
		boucle = boucle*2;
	
	// Premier étage
	if (list_curr(projet->groupes) == list_front(projet->groupes))
	{
		for (i=0;i<boucle;i++)
		{
			tmp = i;
			comb.elements = list_init();
			if (comb.elements == NULL)
				BUG(-2);
			list_mvfront(groupe->elements);
			
			do
			{
				if ((tmp & 1) == 1)
				{
					elem_tmp = (Element*)list_curr(groupe->elements);
					if (_1990_action_cherche_et_positionne(projet, elem_tmp->numero) != 0)
						BUG(-3);
					element.action = list_curr(projet->actions);
					if (list_insert_after(comb.elements, (void*)&element, sizeof(element)) == NULL)
						BUG(-4);
				}
				tmp = tmp >> 1;
				list_mvnext(groupe->elements);
			}
			while (tmp != 0);
			
			if (list_insert_after(groupe->tmp_combinaison.combinaisons, &(comb), sizeof(comb)) == NULL)
				BUG(-5);
		}
	}
	else
	{
		etage = list_curr(projet->groupes);
		etage_tmp = etage->etage-1;
		if (list_traverse(projet->groupes, (void *)&etage_tmp, _1990_groupe_etage_cherche, LIST_ALTR) != LIST_OK)
			BUG(-6);
		etage = list_curr(projet->groupes);
		for (i=0;i<boucle;i++)
		{
			tmp = i;
			combs.combinaisons = list_init();
			if (combs.combinaisons == NULL)
				BUG(-7);
			
			list_mvfront(groupe->elements);
			do
			{
				transition = list_init();
				if (transition == NULL)
					BUG(-8);
				if ((tmp & 1) == 1)
				{
					element_tmp = (Element*)list_curr(groupe->elements);
					if (_1990_groupe_traverse_et_positionne(etage, element_tmp->numero) != 0)
						BUG(-9);
					groupe_n_1 = list_curr(etage->groupe);
					if (list_size(groupe_n_1->tmp_combinaison.combinaisons) != 0)
					{
						if (list_size(combs.combinaisons) == 0)
						{
							if (_1990_combinaisons_duplique_combinaisons(combs.combinaisons, groupe_n_1->tmp_combinaison.combinaisons) != 0)
								BUG(-10);
						}
						else
						{
							if (_1990_combinaisons_duplique_combinaisons(transition, combs.combinaisons) != 0)
								BUG(-11);
							for (j=2;j<=list_size(groupe_n_1->tmp_combinaison.combinaisons);j++)
							{
								if (_1990_combinaisons_duplique_combinaisons(combs.combinaisons, transition) != 0)
									BUG(-12);
							}
							list_mvfront(combs.combinaisons);
							list_mvfront(groupe_n_1->tmp_combinaison.combinaisons);

							for (j=1;j<=list_size(groupe_n_1->tmp_combinaison.combinaisons);j++)
							{
								combinaison2 = (Combinaison*)list_curr(groupe_n_1->tmp_combinaison.combinaisons);
								for (k=1;k<=list_size(transition);k++)
								{
									combinaison1 = (Combinaison*)list_curr(combs.combinaisons);
									if (_1990_combinaisons_ajout_combinaison(combinaison1, combinaison2) != 0)
										BUG(-13);
									list_mvnext(combs.combinaisons);
								}
								list_mvnext(groupe_n_1->tmp_combinaison.combinaisons);
							}
						}
					}
				}
				tmp = tmp >> 1;
				list_mvnext(groupe->elements);
			}
			while (tmp != 0);
			if (_1990_combinaisons_duplique_combinaisons(groupe->tmp_combinaison.combinaisons, combs.combinaisons) != 0)
				BUG(-14);
		}
	}
	return 0;
}

int _1990_combinaisons_genere_groupe(void *input, void *curr)
{
	Projet		*projet = (Projet*)input;
	Groupe		*groupe = (Groupe*)curr;
	
	switch (groupe->type_combinaison)
	{
		case GROUPE_COMBINAISON_OR :
		{
			if (_1990_combinaisons_genere_groupe_or(projet, groupe) != 0)
				return FALSE;

			break;
		}
		case GROUPE_COMBINAISON_XOR :
		{
			if (_1990_combinaisons_genere_groupe_xor(projet, groupe) != 0)
				return FALSE;
			break;
		}
		case GROUPE_COMBINAISON_AND :
		{
			if (_1990_combinaisons_genere_groupe_and(projet, groupe) != 0)
				return FALSE;
			break;
		}
	}
	return TRUE;
}

int _1990_combinaisons_genere_etage(void *input, void *curr)
{
	Etage_Groupe	*etage = (Etage_Groupe*)curr;
	Projet		*projet = (Projet*)input;
	if (list_traverse(etage->groupe, (void *)projet, _1990_combinaisons_genere_groupe, 0) != LIST_EXTENT)
		return FALSE;
	else
		return TRUE;
}

int _1990_combinaisons_genere(Projet *projet)
{
	if (list_traverse(projet->groupes, (void *)projet, _1990_combinaisons_genere_etage, LIST_ALTR) != LIST_EXTENT)
		BUG(-1);
	else
		return 0;
}
