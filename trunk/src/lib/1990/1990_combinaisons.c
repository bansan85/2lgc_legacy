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
#include "maths.h"
#include "1990_actions.h"
#include "1990_groupes.h"
#include "1990_combinaisons.h"


// Renvoie 0 si ponderation_a_verifier n'est pas dans ponderations
int _1990_combinaisons_verifie_ponderation_double(LIST *ponderations, Ponderation* ponderation_a_verifier)
{
	Ponderation		*ponderation;
	Ponderation_Element	*elem1, *elem2;
	int			doublon;
	if (list_size(ponderations) == 0)
		return 0;
	if (list_size(ponderation_a_verifier->elements) == 0)
		return 1;
	list_mvfront(ponderations);

	do
	{
		doublon = 1;
		ponderation = (Ponderation*)list_curr(ponderations);
		if (list_size(ponderation->elements) == list_size(ponderation_a_verifier->elements))
		{
			list_mvfront(ponderation->elements);
			list_mvfront(ponderation_a_verifier->elements);
			do
			{
				elem1 = list_curr(ponderation->elements);
				elem2 = list_curr(ponderation_a_verifier->elements);
				if ((elem1->action != elem2->action) || (elem1->psi != elem2->psi) || (!(ERREUR_RELATIVE_EGALE(elem1->ponderation, elem2->ponderation, ERREUR_RELATIVE_MIN))))
					doublon = 0;
			}
			while ((list_mvnext(ponderation->elements) != NULL) && (list_mvnext(ponderation_a_verifier->elements) != NULL) && (doublon == 1));
			if (doublon == 1)
				return 1;
		}
	}
	while (list_mvnext(ponderations) != NULL);
	return 0;
}

int _1990_combinaisons_duplique_ponderations_sans_double(LIST *destination, LIST *source)
{
	Ponderation		*ponderation_source;
	Ponderation		ponderation_destination;
	Ponderation_Element	*element_source;
	Ponderation_Element	element_destination;
	
	if (list_size(source) == 0)
		return 0;
	list_mvrear(destination);
	list_mvfront(source);
	do
	{
		ponderation_source = list_curr(source);
		if (_1990_combinaisons_verifie_ponderation_double(destination, ponderation_source) == 0)
		{
			ponderation_destination.elements = list_init();
			if (ponderation_destination.elements == NULL)
				BUG(-2);
			if ((ponderation_source != NULL) && (list_curr(ponderation_source->elements) != NULL))
			{
				list_mvfront(ponderation_source->elements);
				do
				{
					element_source = list_curr(ponderation_source->elements);
					element_destination.action = element_source->action;
					element_destination.flags = element_source->flags;
					element_destination.psi = element_source->psi;
					element_destination.ponderation = element_source->ponderation;
					if (list_insert_after(ponderation_destination.elements, (void*)&element_destination, sizeof(element_destination)) == NULL)
						BUG(-3);
		
				}
				while (list_mvnext(ponderation_source->elements) != NULL);
			}
			if (list_insert_after(destination, (void*)&ponderation_destination, sizeof(ponderation_destination)) == NULL)
				BUG(-4);
		}
	}
	while (list_mvnext(source) != NULL);
	return 0;
}

// Renvoie 0 si combinaison_a_verifier n'est pas dans ponderations
int _1990_combinaisons_verifie_combinaison_double(LIST *combinaisons, Combinaison* combinaison_a_verifier)
{
	Combinaison		*combinaison;
	Combinaison_Element	*elem1, *elem2;
	int			doublon;
	if (list_size(combinaisons) == 0)
		return 0;
	if (list_size(combinaison_a_verifier->elements) == 0)
		return 1;
	list_mvfront(combinaisons);

	do
	{
		doublon = 1;
		combinaison = (Combinaison*)list_curr(combinaisons);
		if (list_size(combinaison->elements) == list_size(combinaison_a_verifier->elements))
		{
			list_mvfront(combinaison->elements);
			list_mvfront(combinaison_a_verifier->elements);
			do
			{
				elem1 = list_curr(combinaison->elements);
				elem2 = list_curr(combinaison_a_verifier->elements);
				if ((elem1->action != elem2->action) || (elem1->flags != elem2->flags))
					doublon = 0;
			}
			while ((list_mvnext(combinaison->elements) != NULL) && (list_mvnext(combinaison_a_verifier->elements) != NULL) && (doublon == 1));
			if (doublon == 1)
				return 1;
		}
	}
	while (list_mvnext(combinaisons) != NULL);
	return 0;
}

int _1990_combinaisons_duplique_combinaisons_avec_double(LIST *destination, LIST *source)
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
		combinaison_destination.elements = list_init();
		if (combinaison_destination.elements == NULL)
			BUG(-2);
		if ((combinaison_source != NULL) && (list_curr(combinaison_source->elements) != NULL))
		{
			list_mvfront(combinaison_source->elements);
			do
			{
				element_source = list_curr(combinaison_source->elements);
				element_destination.action = element_source->action;
				element_destination.flags = element_source->flags;
				if (list_insert_after(combinaison_destination.elements, (void*)&element_destination, sizeof(element_destination)) == NULL)
					BUG(-3);
	
			}
			while (list_mvnext(combinaison_source->elements) != NULL);
		}
		if (list_insert_after(destination, (void*)&combinaison_destination, sizeof(combinaison_destination)) == NULL)
			BUG(-4);
	}
	while (list_mvnext(source) != NULL);
	return 0;
}

int _1990_combinaisons_duplique_combinaisons_sans_double(LIST *destination, LIST *source)
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
		if (_1990_combinaisons_verifie_combinaison_double(destination, combinaison_source) == 0)
		{
			combinaison_destination.elements = list_init();
			if (combinaison_destination.elements == NULL)
				BUG(-2);
			if ((combinaison_source != NULL) && (list_curr(combinaison_source->elements) != NULL))
			{
				list_mvfront(combinaison_source->elements);
				do
				{
					element_source = list_curr(combinaison_source->elements);
					element_destination.action = element_source->action;
					element_destination.flags = element_source->flags;
					if (list_insert_after(combinaison_destination.elements, (void*)&element_destination, sizeof(element_destination)) == NULL)
						BUG(-3);
		
				}
				while (list_mvnext(combinaison_source->elements) != NULL);
			}
			if (list_insert_after(destination, (void*)&combinaison_destination, sizeof(combinaison_destination)) == NULL)
				BUG(-4);
		}
	}
	while (list_mvnext(source) != NULL);
	return 0;
}

void _1990_combinaisons_flags_1(Combinaison *combinaison)
{
	Combinaison_Element *combinaison_element;
	list_mvfront(combinaison->elements);
	if (list_size(combinaison->elements) == 0)
		return;
	do
	{
		combinaison_element = list_curr(combinaison->elements);
		if (_1990_action_type_combinaison_bat(combinaison_element->action->categorie, PAYS_EU) == 2)
			combinaison_element->flags = 1;
	}
	while (list_mvnext(combinaison->elements));
	return;
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
			element.flags = element.action->flags;
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
			if (_1990_combinaisons_duplique_combinaisons_sans_double(groupe->tmp_combinaison.combinaisons, groupe_n_1->tmp_combinaison.combinaisons) != 0)
				BUG(-11);
		}
		while (list_mvnext(groupe->elements) != NULL);
		
		etage_tmp = etage->etage+1;
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
		element_destination.flags = element_source->flags;
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
	Combinaison_Element	element, *comb_elem;
	Element			*element_tmp;
	int			etage_tmp, i, j, nbboucle, action_predominante;
	LIST			*transition, *nouvelles_combinaisons;
	
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
		action_predominante = 0;
		do
		{
			element_tmp = (Element*)list_curr(groupe->elements);
			if (_1990_action_cherche_et_positionne(projet, element_tmp->numero) != 0)
				BUG(-4);
			element.action = list_curr(projet->actions);
			element.flags = element.action->flags;
			if ((element.flags & 1) != 0)
				action_predominante = 1;
			if (list_insert_after(comb.elements, (void*)&element, sizeof(element)) == NULL)
				BUG(-5);
		}
		while (list_mvnext(groupe->elements) != NULL);
		if (action_predominante == 1)
			_1990_combinaisons_flags_1(&(comb));
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
		nouvelles_combinaisons = list_init();
		if (nouvelles_combinaisons == NULL)
			BUG(-9);
		etage = list_curr(projet->groupes);
		list_mvfront(groupe->elements);
		do
		{
			element_tmp = (Element*)list_curr(groupe->elements);
			if (_1990_groupe_traverse_et_positionne(etage, element_tmp->numero) != 0)
				BUG(-10);
			groupe_n_1 = list_curr(etage->groupe);
			if (list_front(groupe->elements) == element_tmp)
			{
				if (_1990_combinaisons_duplique_combinaisons_avec_double(nouvelles_combinaisons, groupe_n_1->tmp_combinaison.combinaisons) != 0)
					BUG(-11);
			}
			else
			{
				transition = list_init();
				if (transition == NULL)
					BUG(-12);
				if (_1990_combinaisons_duplique_combinaisons_avec_double(transition, nouvelles_combinaisons) != 0)
					BUG(-13);
				nbboucle = list_size(groupe_n_1->tmp_combinaison.combinaisons);
				for (i=2;i<=nbboucle;i++)
				{
					if (_1990_combinaisons_duplique_combinaisons_avec_double(nouvelles_combinaisons, transition) != 0)
						BUG(-14);
				}
				
				list_mvfront(nouvelles_combinaisons);
				list_mvfront(groupe_n_1->tmp_combinaison.combinaisons);
				for (i=1;i<=nbboucle;i++)
				{
					combinaison2 = (Combinaison*)list_curr(groupe_n_1->tmp_combinaison.combinaisons);
					for (j=1;j<=list_size(transition);j++)
					{
						combinaison1 = (Combinaison*)list_curr(nouvelles_combinaisons);
						if (_1990_combinaisons_ajout_combinaison(combinaison1, combinaison2) != 0)
							BUG(-15);
						list_mvnext(nouvelles_combinaisons);
					}
					list_mvnext(groupe_n_1->tmp_combinaison.combinaisons);
				}
				list_free(transition, &(_1990_groupe_free_groupe_tmp_combinaison));
			}
			if (list_size(nouvelles_combinaisons) != 0)
			{
				list_mvfront(nouvelles_combinaisons);
				do
				{
					combinaison1 = (Combinaison*)list_curr(nouvelles_combinaisons);
					if (list_size(combinaison1->elements) != 0)
					{
						list_mvfront(combinaison1->elements);
						do
						{
							comb_elem = list_curr(combinaison1->elements);
						} while (list_mvnext(combinaison1->elements));
					}
					
				}
				while (list_mvnext(nouvelles_combinaisons) != NULL);
			}
		}
		while (list_mvnext(groupe->elements) != NULL);
		_1990_combinaisons_duplique_combinaisons_sans_double(groupe->tmp_combinaison.combinaisons, nouvelles_combinaisons);
		list_free(nouvelles_combinaisons, &(_1990_groupe_free_groupe_tmp_combinaison));
		
		etage_tmp = etage->etage+1;
		if (list_traverse(projet->groupes, (void *)&etage_tmp, _1990_groupe_etage_cherche, LIST_ALTR) != LIST_OK)
			BUG(-16);
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
	int			etage_tmp, boucle = 2, i, j, k, tmp, action_predominante;
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
			action_predominante = 0;
			do
			{
				if ((tmp & 1) == 1)
				{
					elem_tmp = (Element*)list_curr(groupe->elements);
					if (_1990_action_cherche_et_positionne(projet, elem_tmp->numero) != 0)
						BUG(-3);
					element.action = list_curr(projet->actions);
					element.flags = element.action->flags;
					if ((element.flags & 1) != 0)
						action_predominante = 1;
					if (list_insert_after(comb.elements, (void*)&element, sizeof(element)) == NULL)
						BUG(-4);
				}
				tmp = tmp >> 1;
				list_mvnext(groupe->elements);
			}
			while (tmp != 0);
			if (action_predominante == 1)
				_1990_combinaisons_flags_1(&(comb));
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
							if (_1990_combinaisons_duplique_combinaisons_avec_double(combs.combinaisons, groupe_n_1->tmp_combinaison.combinaisons) != 0)
								BUG(-10);
						}
						else
						{
							if (_1990_combinaisons_duplique_combinaisons_avec_double(transition, combs.combinaisons) != 0)
								BUG(-11);
							for (j=2;j<=list_size(groupe_n_1->tmp_combinaison.combinaisons);j++)
							{
								if (_1990_combinaisons_duplique_combinaisons_avec_double(combs.combinaisons, transition) != 0)
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
				list_free(transition, &(_1990_groupe_free_groupe_tmp_combinaison));
			}
			while (tmp != 0);
			if (_1990_combinaisons_duplique_combinaisons_sans_double(groupe->tmp_combinaison.combinaisons, combs.combinaisons) != 0)
				BUG(-14);
			list_free(combs.combinaisons, &(_1990_groupe_free_groupe_tmp_combinaison));
		}
		etage_tmp = etage->etage+1;
		if (list_traverse(projet->groupes, (void *)&etage_tmp, _1990_groupe_etage_cherche, LIST_ALTR) != LIST_OK)
			BUG(-15);
	}
	return 0;
}

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

void _1990_combinaisons_free_free(void *data)
{
	Ponderation *ponderation = (Ponderation*)data;
	list_free(ponderation->elements, LIST_DEALLOC);
	free(data);
	return;
}

void _1990_combinaisons_free(Projet *projet)
{
	list_free(projet->combinaisons.elu_equ, &(_1990_combinaisons_free_free));
	list_free(projet->combinaisons.elu_str, &(_1990_combinaisons_free_free));
	list_free(projet->combinaisons.elu_geo, &(_1990_combinaisons_free_free));
	list_free(projet->combinaisons.elu_fat, &(_1990_combinaisons_free_free));
	list_free(projet->combinaisons.elu_acc, &(_1990_combinaisons_free_free));
	list_free(projet->combinaisons.elu_sis, &(_1990_combinaisons_free_free));
	list_free(projet->combinaisons.els_car, &(_1990_combinaisons_free_free));
	list_free(projet->combinaisons.els_freq, &(_1990_combinaisons_free_free));
	list_free(projet->combinaisons.els_perm, &(_1990_combinaisons_free_free));
	return;
}

int _1990_combinaisons_genere_ponderations(Projet *projet, LIST* combinaisons_destination, double* coef_min, double* coef_max, int dim_coef, int psi_dominante, int psi_accompagnement)
{
	int			nbboucle=1, j, suivant, categorie, tmp, variable, variable_dominante;
	Ponderation		ponderation;
	Ponderation_Element	ponderation_element;
	Combinaison		*combinaison;
	Combinaison_Element	*combinaison_element;
	Groupe			*groupe;
	Etage_Groupe		*etage;
	
	etage = list_rear(projet->groupes);
	if (etage == NULL)
		BUG(-1);
	if (list_front(etage->groupe) != list_rear(etage->groupe))
		BUG(-2);
	groupe = list_front(etage->groupe);
	
	if (list_size(groupe->tmp_combinaison.combinaisons) == 0)
		return 0;
	
	nbboucle = nbboucle << dim_coef;
	
	for (j=0;j<nbboucle;j++)
	{
		list_mvfront(groupe->tmp_combinaison.combinaisons);
		do
		{
			suivant = 0;
			variable = 0;
			variable_dominante = 0;
			combinaison = (Combinaison*) list_curr(groupe->tmp_combinaison.combinaisons);
			ponderation.elements = list_init();
			list_mvfront(combinaison->elements);
			if (list_size(combinaison->elements) != 0)
			{
				do
				{
					combinaison_element = (Combinaison_Element*)list_curr(combinaison->elements);
					ponderation_element.action = combinaison_element->action;
					ponderation_element.flags = combinaison_element->flags;
					if (_1990_action_type_combinaison_bat(ponderation_element.action->categorie, projet->pays) == 2)
					{
						variable = 1;
						if ((ponderation_element.flags & 1) != 0)
						{
							variable_dominante = 1;
							ponderation_element.psi = psi_dominante;
						}
						else
							ponderation_element.psi = psi_accompagnement;
					}
					else
						ponderation_element.psi = -1;
					categorie = _1990_action_type_combinaison_bat(ponderation_element.action->categorie, projet->pays);
					if ((ERREUR_RELATIVE_EGALE(0., coef_min[categorie], ERREUR_RELATIVE_MIN)) && (ERREUR_RELATIVE_EGALE(0., coef_max[categorie], ERREUR_RELATIVE_MIN)))
						suivant = 1;
					else
					{
						tmp = 1 << categorie;
						if ((j & tmp) != 0)
							ponderation_element.ponderation = coef_max[categorie];
						else
							ponderation_element.ponderation = coef_min[categorie];
						if (!(ERREUR_RELATIVE_EGALE(0., ponderation_element.ponderation, ERREUR_RELATIVE_MIN)))
						{
							if (list_insert_after(ponderation.elements, &ponderation_element, sizeof(ponderation_element)) == NULL)
								BUG(-1);
						}
					}
				}
				while ((list_mvnext(combinaison->elements) != NULL) && (suivant != 1));
			}
			if ((variable == 1) && (variable_dominante == 0))
				suivant = 1;
			if ((suivant == 0) && (list_size(ponderation.elements) != 0) && (_1990_combinaisons_verifie_ponderation_double(combinaisons_destination, &ponderation) == 0))
			{
				if (list_insert_after(combinaisons_destination, &ponderation, sizeof(ponderation)) == NULL)
					BUG(-1);
			}
			else
				list_free(ponderation.elements, LIST_DEALLOC);
		}
		while (list_mvnext(groupe->tmp_combinaison.combinaisons) != NULL);
	}
	
	return 0;
}


int _1990_combinaisons_genere_ponderation_eu(Projet *projet)
{
	double		*coef_min, *coef_max;
	
	
	coef_min = (double*)malloc(5*sizeof(double));
	if (coef_min == NULL)
		BUG(-2);
	coef_max = (double*)malloc(5*sizeof(double));
	if (coef_max == NULL)
	{
		free(coef_min);
		BUG(-3);
	}
	
	// Pour ELU_EQU
	// Equilibre seulement
	if ((projet->combinaisons.flags & 1) == 0)
	{
		coef_min[0] = 0.9; coef_max[0] = 1.1; // poids propre
		coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
		coef_min[2] = 0.0; coef_max[2] = 1.5; // variable
		coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
		coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
		_1990_combinaisons_genere_ponderations(projet, projet->combinaisons.elu_equ, coef_min, coef_max, 5, -1, 0);
	}
	// Equilibre + Résistance structurelle
	else
	{
		coef_min[0] = 1.15; coef_max[0] = 1.35; // poids propre
		coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
		coef_min[2] = 0.0; coef_max[2] = 1.5; // variable
		coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
		coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
		_1990_combinaisons_genere_ponderations(projet, projet->combinaisons.elu_equ, coef_min, coef_max, 5, -1, 0);
		coef_min[0] = 1.0; coef_max[0] = 1.0; // poids propre
		coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
		coef_min[2] = 0.0; coef_max[2] = 1.5; // variable
		coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
		coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
		_1990_combinaisons_genere_ponderations(projet, projet->combinaisons.elu_equ, coef_min, coef_max, 5, -1, 0);
	}
	// Equation 6.10a et 6.10b
	if ((projet->combinaisons.flags & 8) == 0)
	{
		switch (projet->combinaisons.flags & 6)
		{
			// Approche 1
			case 0:
			{
				coef_min[0] = 1.0; coef_max[0] = 1.35; // poids propre
				coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
				coef_min[2] = 0.0; coef_max[2] = 1.5; // variable
				coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
				coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
				_1990_combinaisons_genere_ponderations(projet, projet->combinaisons.elu_str, coef_min, coef_max, 5, 0, 0);
				coef_min[0] = 1.0; coef_max[0] = 1.15; // poids propre
				coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
				coef_min[2] = 0.0; coef_max[2] = 1.5; // variable
				coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
				coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
				_1990_combinaisons_genere_ponderations(projet, projet->combinaisons.elu_str, coef_min, coef_max, 5, -1, 0);
				coef_min[0] = 1.0; coef_max[0] = 1.0; // poids propre
				coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
				coef_min[2] = 0.0; coef_max[2] = 1.3; // variable
				coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
				coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
				_1990_combinaisons_genere_ponderations(projet, projet->combinaisons.elu_str, coef_min, coef_max, 5, -1, 0);
				_1990_combinaisons_duplique_ponderations_sans_double(projet->combinaisons.elu_geo, projet->combinaisons.elu_str);
				break;
			}
			// Approche 2
			case 2:
			{
				coef_min[0] = 1.0; coef_max[0] = 1.35; // poids propre
				coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
				coef_min[2] = 0.0; coef_max[2] = 1.5; // variable
				coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
				coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
				_1990_combinaisons_genere_ponderations(projet, projet->combinaisons.elu_str, coef_min, coef_max, 5, 0, 0);
				coef_min[0] = 1.0; coef_max[0] = 1.15; // poids propre
				coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
				coef_min[2] = 0.0; coef_max[2] = 1.5; // variable
				coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
				coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
				_1990_combinaisons_genere_ponderations(projet, projet->combinaisons.elu_str, coef_min, coef_max, 5, -1, 0);
				_1990_combinaisons_duplique_ponderations_sans_double(projet->combinaisons.elu_geo, projet->combinaisons.elu_str);
				break;
			}
			case 4:
			{
				coef_min[0] = 1.0; coef_max[0] = 1.35; // poids propre
				coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
				coef_min[2] = 0.0; coef_max[2] = 1.5; // variable
				coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
				coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
				_1990_combinaisons_genere_ponderations(projet, projet->combinaisons.elu_str, coef_min, coef_max, 5, 0, 0);
				coef_min[0] = 1.0; coef_max[0] = 1.15; // poids propre
				coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
				coef_min[2] = 0.0; coef_max[2] = 1.5; // variable
				coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
				coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
				_1990_combinaisons_genere_ponderations(projet, projet->combinaisons.elu_str, coef_min, coef_max, 5, -1, 0);
				coef_min[0] = 1.0; coef_max[0] = 1.0; // poids propre
				coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
				coef_min[2] = 0.0; coef_max[2] = 1.3; // variable
				coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
				coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
				_1990_combinaisons_genere_ponderations(projet, projet->combinaisons.elu_geo, coef_min, coef_max, 5, -1, 0);
				break;
			}
			default:
			{
				BUG(-4);
				break;
			}
		}
	}
	// Equation 6.10
	else
	{
		switch (projet->combinaisons.flags & 6)
		{
			// Approche 1
			case 0:
			{
				coef_min[0] = 1.0; coef_max[0] = 1.35; // poids propre
				coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
				coef_min[2] = 0.0; coef_max[2] = 1.5; // variable
				coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
				coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
				_1990_combinaisons_genere_ponderations(projet, projet->combinaisons.elu_str, coef_min, coef_max, 5, -1, 0);
				coef_min[0] = 1.0; coef_max[0] = 1.0; // poids propre
				coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
				coef_min[2] = 0.0; coef_max[2] = 1.3; // variable
				coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
				coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
				_1990_combinaisons_genere_ponderations(projet, projet->combinaisons.elu_str, coef_min, coef_max, 5, -1, 0);
				_1990_combinaisons_duplique_ponderations_sans_double(projet->combinaisons.elu_geo, projet->combinaisons.elu_str);
				break;
			}
			// Approche 2
			case 2:
			{
				coef_min[0] = 1.0; coef_max[0] = 1.35; // poids propre
				coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
				coef_min[2] = 0.0; coef_max[2] = 1.5; // variable
				coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
				coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
				_1990_combinaisons_genere_ponderations(projet, projet->combinaisons.elu_str, coef_min, coef_max, 5, -1, 0);
				_1990_combinaisons_duplique_ponderations_sans_double(projet->combinaisons.elu_geo, projet->combinaisons.elu_str);
				break;
			}
			case 4:
			{
				coef_min[0] = 1.0; coef_max[0] = 1.35; // poids propre
				coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
				coef_min[2] = 0.0; coef_max[2] = 1.5; // variable
				coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
				coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
				_1990_combinaisons_genere_ponderations(projet, projet->combinaisons.elu_str, coef_min, coef_max, 5, -1, 0);
				coef_min[0] = 1.0; coef_max[0] = 1.0; // poids propre
				coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
				coef_min[2] = 0.0; coef_max[2] = 1.3; // variable
				coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
				coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
				_1990_combinaisons_genere_ponderations(projet, projet->combinaisons.elu_geo, coef_min, coef_max, 5, -1, 0);
				break;
			}
			default :
			{
				BUG(-4);
				break;
			}
		}
	}
	if ((projet->combinaisons.flags & 16) == 0)
	{
		coef_min[0] = 1.0; coef_max[0] = 1.0; // poids propre
		coef_min[1] = 1.0; coef_max[1] = 1.0; // précontrainte (1992-1)
		coef_min[2] = 0.0; coef_max[2] = 1.0; // variable
		coef_min[3] = 1.0; coef_max[3] = 1.0; // Accidentelle
		coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
		_1990_combinaisons_genere_ponderations(projet, projet->combinaisons.elu_acc, coef_min, coef_max, 5, 1, 2);
	}
	else
	{
		coef_min[0] = 1.0; coef_max[0] = 1.0; // poids propre
		coef_min[1] = 1.0; coef_max[1] = 1.0; // précontrainte (1992-1)
		coef_min[2] = 0.0; coef_max[2] = 1.0; // variable
		coef_min[3] = 1.0; coef_max[3] = 1.0; // Accidentelle
		coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
		_1990_combinaisons_genere_ponderations(projet, projet->combinaisons.elu_acc, coef_min, coef_max, 5, 2, 2);
	}
	
	coef_min[0] = 1.0; coef_max[0] = 1.0; // poids propre
	coef_min[1] = 1.0; coef_max[1] = 1.0; // précontrainte (1992-1)
	coef_min[2] = 0.0; coef_max[2] = 1.0; // variable
	coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
	coef_min[4] = 1.0; coef_max[4] = 1.0; // Sismique
	_1990_combinaisons_genere_ponderations(projet, projet->combinaisons.elu_sis, coef_min, coef_max, 5, 2, 2);
	
	coef_min[0] = 1.0; coef_max[0] = 1.0; // poids propre
	coef_min[1] = 1.0; coef_max[1] = 1.0; // précontrainte (1992-1)
	coef_min[2] = 0.0; coef_max[2] = 1.0; // variable
	coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
	coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
	_1990_combinaisons_genere_ponderations(projet, projet->combinaisons.els_car, coef_min, coef_max, 5, -1, 0);

	coef_min[0] = 1.0; coef_max[0] = 1.0; // poids propre
	coef_min[1] = 1.0; coef_max[1] = 1.0; // précontrainte (1992-1)
	coef_min[2] = 0.0; coef_max[2] = 1.0; // variable
	coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
	coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
	_1990_combinaisons_genere_ponderations(projet, projet->combinaisons.els_freq, coef_min, coef_max, 5, 1, 2);

	coef_min[0] = 1.0; coef_max[0] = 1.0; // poids propre
	coef_min[1] = 1.0; coef_max[1] = 1.0; // précontrainte (1992-1)
	coef_min[2] = 0.0; coef_max[2] = 1.0; // variable
	coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
	coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
	_1990_combinaisons_genere_ponderations(projet, projet->combinaisons.els_perm, coef_min, coef_max, 5, 2, 2);

	free(coef_min);
	free(coef_max);
	
	return 0;
}

int _1990_combinaisons_genere_ponderation_fr(Projet *projet)
{
	double		*coef_min, *coef_max;
	
	
	coef_min = (double*)malloc(6*sizeof(double));
	if (coef_min == NULL)
		BUG(-2);
	coef_max = (double*)malloc(6*sizeof(double));
	if (coef_max == NULL)
	{
		free(coef_min);
		BUG(-3);
	}
	
	// Pour ELU_EQU
	if ((projet->combinaisons.flags & 1) == 0)
	{
		coef_min[0] = 0.9; coef_max[0] = 1.1; // poids propre
		coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
		coef_min[2] = 0.0; coef_max[2] = 1.5; // variable
		coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
		coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
		coef_min[5] = 0.0; coef_max[5] = 1.2; // Eaux souterraines
		_1990_combinaisons_genere_ponderations(projet, projet->combinaisons.elu_equ, coef_min, coef_max, 6, -1, 0);
	}
	else
	{
		coef_min[0] = 1.15; coef_max[0] = 1.35; // poids propre
		coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
		coef_min[2] = 0.0; coef_max[2] = 1.5; // variable
		coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
		coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
		coef_min[5] = 0.0; coef_max[5] = 1.2; // Eaux souterraines
		_1990_combinaisons_genere_ponderations(projet, projet->combinaisons.elu_equ, coef_min, coef_max, 6, -1, 0);
		coef_min[0] = 1.0; coef_max[0] = 1.0; // poids propre
		coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
		coef_min[2] = 0.0; coef_max[2] = 1.5; // variable
		coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
		coef_min[4] = 0.0; coef_max[4] = 1.0; // Sismique
		coef_min[5] = 0.0; coef_max[5] = 1.2; // Eaux souterraines
		_1990_combinaisons_genere_ponderations(projet, projet->combinaisons.elu_equ, coef_min, coef_max, 6, -1, 0);
	}
	// On utilise l'équation 6.10a et 6.10b
	if ((projet->combinaisons.flags & 8) == 0)
	{
		switch (projet->combinaisons.flags & 6)
		{
			case 0:
			{
				coef_min[0] = 1.0; coef_max[0] = 1.35; // poids propre
				coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
				coef_min[2] = 0.0; coef_max[2] = 1.5; // variable
				coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
				coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
				coef_min[5] = 0.0; coef_max[5] = 1.2; // Eaux souterraines
				_1990_combinaisons_genere_ponderations(projet, projet->combinaisons.elu_str, coef_min, coef_max, 6, 0, 0);
				coef_min[0] = 1.0; coef_max[0] = 1.15; // poids propre
				coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
				coef_min[2] = 0.0; coef_max[2] = 1.5; // variable
				coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
				coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
				coef_min[5] = 0.0; coef_max[5] = 1.2; // Eaux souterraines
				_1990_combinaisons_genere_ponderations(projet, projet->combinaisons.elu_str, coef_min, coef_max, 6, -1, 0);
				coef_min[0] = 1.0; coef_max[0] = 1.0; // poids propre
				coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
				coef_min[2] = 0.0; coef_max[2] = 1.3; // variable
				coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
				coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
				coef_min[5] = 0.0; coef_max[5] = 1.2; // Eaux souterraines
				_1990_combinaisons_genere_ponderations(projet, projet->combinaisons.elu_str, coef_min, coef_max, 6, -1, 0);
				_1990_combinaisons_duplique_ponderations_sans_double(projet->combinaisons.elu_geo, projet->combinaisons.elu_str);
				break;
			}
			case 2:
			{
				coef_min[0] = 1.0; coef_max[0] = 1.35; // poids propre
				coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
				coef_min[2] = 0.0; coef_max[2] = 1.5; // variable
				coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
				coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
				coef_min[5] = 0.0; coef_max[5] = 1.2; // Eaux souterraines
				_1990_combinaisons_genere_ponderations(projet, projet->combinaisons.elu_str, coef_min, coef_max, 6, 0, 0);
				coef_min[0] = 1.0; coef_max[0] = 1.15; // poids propre
				coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
				coef_min[2] = 0.0; coef_max[2] = 1.5; // variable
				coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
				coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
				coef_min[5] = 0.0; coef_max[5] = 1.2; // Eaux souterraines
				_1990_combinaisons_genere_ponderations(projet, projet->combinaisons.elu_str, coef_min, coef_max, 6, -1, 0);
				_1990_combinaisons_duplique_ponderations_sans_double(projet->combinaisons.elu_geo, projet->combinaisons.elu_str);
				break;
			}
			case 4:
			{
				coef_min[0] = 1.0; coef_max[0] = 1.35; // poids propre
				coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
				coef_min[2] = 0.0; coef_max[2] = 1.5; // variable
				coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
				coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
				coef_min[5] = 0.0; coef_max[5] = 1.2; // Eaux souterraines
				_1990_combinaisons_genere_ponderations(projet, projet->combinaisons.elu_str, coef_min, coef_max, 6, 0, 0);
				coef_min[0] = 1.0; coef_max[0] = 1.15; // poids propre
				coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
				coef_min[2] = 0.0; coef_max[2] = 1.5; // variable
				coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
				coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
				coef_min[5] = 0.0; coef_max[5] = 1.2; // Eaux souterraines
				_1990_combinaisons_genere_ponderations(projet, projet->combinaisons.elu_str, coef_min, coef_max, 6, -1, 0);
				coef_min[0] = 1.0; coef_max[0] = 1.0; // poids propre
				coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
				coef_min[2] = 0.0; coef_max[2] = 1.3; // variable
				coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
				coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
				coef_min[5] = 0.0; coef_max[5] = 1.2; // Eaux souterraines
				_1990_combinaisons_genere_ponderations(projet, projet->combinaisons.elu_geo, coef_min, coef_max, 6, -1, 0);
				break;
			}
			default :
			{
				BUG(-4);
				break;
			}
		}
	}
	else
	// équation 6.10
	{
		switch (projet->combinaisons.flags & 6)
		{
			case 0:
			{
				coef_min[0] = 1.0; coef_max[0] = 1.35; // poids propre
				coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
				coef_min[2] = 0.0; coef_max[2] = 1.5; // variable
				coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
				coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
				coef_min[5] = 0.0; coef_max[5] = 1.2; // Eaux souterraines
				_1990_combinaisons_genere_ponderations(projet, projet->combinaisons.elu_str, coef_min, coef_max, 6, -1, 0);
				coef_min[0] = 1.0; coef_max[0] = 1.0; // poids propre
				coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
				coef_min[2] = 0.0; coef_max[2] = 1.3; // variable
				coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
				coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
				coef_min[5] = 0.0; coef_max[5] = 1.2; // Eaux souterraines
				_1990_combinaisons_genere_ponderations(projet, projet->combinaisons.elu_str, coef_min, coef_max, 6, -1, 0);
				_1990_combinaisons_duplique_ponderations_sans_double(projet->combinaisons.elu_geo, projet->combinaisons.elu_str);
				break;
			}
			case 2:
			{
				coef_min[0] = 1.0; coef_max[0] = 1.35; // poids propre
				coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
				coef_min[2] = 0.0; coef_max[2] = 1.5; // variable
				coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
				coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
				coef_min[5] = 0.0; coef_max[5] = 1.2; // Eaux souterraines
				_1990_combinaisons_genere_ponderations(projet, projet->combinaisons.elu_str, coef_min, coef_max, 6, -1, 0);
				_1990_combinaisons_duplique_ponderations_sans_double(projet->combinaisons.elu_geo, projet->combinaisons.elu_str);
				break;
			}
			case 4:
			{
				coef_min[0] = 1.0; coef_max[0] = 1.35; // poids propre
				coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
				coef_min[2] = 0.0; coef_max[2] = 1.5; // variable
				coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
				coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
				coef_min[5] = 0.0; coef_max[5] = 1.2; // Eaux souterraines
				_1990_combinaisons_genere_ponderations(projet, projet->combinaisons.elu_str, coef_min, coef_max, 6, -1, 0);
				coef_min[0] = 1.0; coef_max[0] = 1.0; // poids propre
				coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
				coef_min[2] = 0.0; coef_max[2] = 1.3; // variable
				coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
				coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
				coef_min[5] = 0.0; coef_max[5] = 1.2; // Eaux souterraines
				_1990_combinaisons_genere_ponderations(projet, projet->combinaisons.elu_geo, coef_min, coef_max, 6, -1, 0);
				break;
			}
			default :
			{
				BUG(-4);
				break;
			}
		}
	}
	if ((projet->combinaisons.flags & 16) == 0)
	{
		coef_min[0] = 1.0; coef_max[0] = 1.0; // poids propre
		coef_min[1] = 1.0; coef_max[1] = 1.0; // précontrainte (1992-1)
		coef_min[2] = 0.0; coef_max[2] = 1.0; // variable
		coef_min[3] = 1.0; coef_max[3] = 1.0; // Accidentelle
		coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
		coef_min[5] = 0.0; coef_max[5] = 1.0; // Eaux souterraines
		_1990_combinaisons_genere_ponderations(projet, projet->combinaisons.elu_acc, coef_min, coef_max, 6, 1, 2);
	}
	else
	{
		coef_min[0] = 1.0; coef_max[0] = 1.0; // poids propre
		coef_min[1] = 1.0; coef_max[1] = 1.0; // précontrainte (1992-1)
		coef_min[2] = 0.0; coef_max[2] = 1.0; // variable
		coef_min[3] = 1.0; coef_max[3] = 1.0; // Accidentelle
		coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
		coef_min[5] = 0.0; coef_max[5] = 1.0; // Eaux souterraines
		_1990_combinaisons_genere_ponderations(projet, projet->combinaisons.elu_acc, coef_min, coef_max, 6, 2, 2);
	}
	
	coef_min[0] = 1.0; coef_max[0] = 1.0; // poids propre
	coef_min[1] = 1.0; coef_max[1] = 1.0; // précontrainte (1992-1)
	coef_min[2] = 0.0; coef_max[2] = 1.0; // variable
	coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
	coef_min[4] = 1.0; coef_max[4] = 1.0; // Sismique
	coef_min[5] = 0.0; coef_max[5] = 1.0; // Eaux souterraines
	_1990_combinaisons_genere_ponderations(projet, projet->combinaisons.elu_acc, coef_min, coef_max, 6, 2, 2);
	
	coef_min[0] = 1.0; coef_max[0] = 1.0; // poids propre
	coef_min[1] = 1.0; coef_max[1] = 1.0; // précontrainte (1992-1)
	coef_min[2] = 0.0; coef_max[2] = 1.0; // variable
	coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
	coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
	coef_min[5] = 0.0; coef_max[5] = 1.2; // Eaux souterraines
	_1990_combinaisons_genere_ponderations(projet, projet->combinaisons.els_car, coef_min, coef_max, 6, -1, 0);

	coef_min[0] = 1.0; coef_max[0] = 1.0; // poids propre
	coef_min[1] = 1.0; coef_max[1] = 1.0; // précontrainte (1992-1)
	coef_min[2] = 0.0; coef_max[2] = 1.0; // variable
	coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
	coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
	coef_min[5] = 0.0; coef_max[5] = 1.0; // Eaux souterraines
	_1990_combinaisons_genere_ponderations(projet, projet->combinaisons.els_freq, coef_min, coef_max, 6, 1, 2);

	coef_min[0] = 1.0; coef_max[0] = 1.0; // poids propre
	coef_min[1] = 1.0; coef_max[1] = 1.0; // précontrainte (1992-1)
	coef_min[2] = 0.0; coef_max[2] = 1.0; // variable
	coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
	coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
	coef_min[5] = 0.0; coef_max[5] = 1.0; // Eaux souterraines
	_1990_combinaisons_genere_ponderations(projet, projet->combinaisons.els_perm, coef_min, coef_max, 6, 2, 2);

	free(coef_min);
	free(coef_max);
	
	return 0;
}

int _1990_combinaisons_genere_ponderation(Projet *projet)
{
	switch (projet->pays)
	{
		case PAYS_EU : { return _1990_combinaisons_genere_ponderation_eu(projet); break; }
		case PAYS_FR : { return _1990_combinaisons_genere_ponderation_fr(projet); break; }
		default : { BUG(-1.); break; }
	}
}

int _1990_combinaisons_genere(Projet *projet)
{
	Etage_Groupe	*etage;
	Groupe		*groupe;
	Action		*action;
	int		i, j;
	
	list_mvfront(projet->groupes);
	if (list_curr(projet->groupes) == NULL)
		return 0;
	for (i=1;i<=list_size(projet->actions);i++)
	{
		_1990_groupe_free_seulement_tmp_combinaison(projet);
		list_mvfront(projet->actions);
		for (j=1;j<i;j++)
		{
			action = list_curr(projet->actions);
			action->flags = 0;
			list_mvnext(projet->actions);
		}
		action = list_curr(projet->actions);
		if (_1990_action_type_combinaison_bat(action->categorie, PAYS_EU) == 2)
			action->flags = 1;
		else
			action->flags = 0;
		list_mvnext(projet->actions);
		for (j=i+1;j<=list_size(projet->actions);j++)
		{
			action = list_curr(projet->actions);
			action->flags = 0;
			list_mvnext(projet->actions);
		}
			
		list_mvfront(projet->groupes);
		do
		{
			etage = (Etage_Groupe*)list_curr(projet->groupes);
			
			list_mvfront(etage->groupe);
			if (list_curr(etage->groupe) != NULL)
			{
				do
				{
					groupe = (Groupe*)list_curr(etage->groupe);
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
				}
				while (list_mvnext(etage->groupe));
			}
		}
		while (list_mvnext(projet->groupes) != NULL);
		
		_1990_combinaisons_genere_ponderation(projet);
	}
	return 0;
}

void _1990_combinaisons_affiche_ponderation(LIST *ponderations)
{
	Ponderation		*ponderation;
	Ponderation_Element	*ponderation_element;
	if (list_size(ponderations) != 0)
	{
		list_mvfront(ponderations);
		do
		{
			ponderation = list_curr(ponderations);
			if (list_size(ponderation->elements) != 0)
			{
				list_mvfront(ponderation->elements);
				do
				{
					ponderation_element = list_curr(ponderation->elements);
					printf("%d*%f(%d)+", ponderation_element->action->numero+1, ponderation_element->ponderation, ponderation_element->psi);
				}
				while (list_mvnext(ponderation->elements));
				printf("\n");
			}
		}
		while (list_mvnext(ponderations));
	}

	return;
}

void _1990_combinaisons_affiche_ponderations(Projet *projet)
{
	printf("elu_equ\n");
	_1990_combinaisons_affiche_ponderation(projet->combinaisons.elu_equ);
	printf("elu_str\n");
	_1990_combinaisons_affiche_ponderation(projet->combinaisons.elu_str);
	printf("elu_geo\n");
	_1990_combinaisons_affiche_ponderation(projet->combinaisons.elu_geo);
	printf("elu_fat\n");
	_1990_combinaisons_affiche_ponderation(projet->combinaisons.elu_fat);
	printf("elu_acc\n");
	_1990_combinaisons_affiche_ponderation(projet->combinaisons.elu_acc);
	printf("elu_sis\n");
	_1990_combinaisons_affiche_ponderation(projet->combinaisons.elu_sis);
	printf("els_car\n");
	_1990_combinaisons_affiche_ponderation(projet->combinaisons.els_car);
	printf("els_freq\n");
	_1990_combinaisons_affiche_ponderation(projet->combinaisons.els_freq);
	printf("els_perm\n");
	_1990_combinaisons_affiche_ponderation(projet->combinaisons.els_perm);
	return;
}

