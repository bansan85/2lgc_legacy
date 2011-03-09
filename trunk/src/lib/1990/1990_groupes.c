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
#include <stdlib.h>
#include <stdio.h>

#include "projet.h"
#include "erreurs.h"
#include "1990_actions.h"
#include "1990_groupes.h"
#include "1990_combinaisons.h"

int _1990_groupe_init(Projet *projet)
{
	projet->groupes = list_init();
	if (projet->groupes == NULL)
		BUG(-1);
	else
		return 0;
}

int _1990_groupe_etage_cherche(void *input, void *curr)
{
	Etage_Groupe *etage = (Etage_Groupe*)curr;
	int *numero = (int*)input;
	if (etage->etage == (*numero))
		return FALSE;
	else
		return TRUE;
}

int _1990_groupe_cherche(void *input, void *curr)
{
	Groupe *groupe = (Groupe*)curr;
	int *numero = (int*)input;
	if (groupe->numero == (*numero))
		return FALSE;
	else
		return TRUE;
}

int _1990_groupe_etage_ajout(Projet *projet, int etage)
{
	Etage_Groupe	etage_nouveau;
	
	list_mvrear(projet->groupes);
	etage_nouveau.etage = etage;
	etage_nouveau.groupe = list_init();
	if (etage_nouveau.groupe == NULL)
		BUG(-1);
	if (list_insert_after(projet->groupes, &(etage_nouveau), sizeof(etage_nouveau)) == NULL)
		BUG(-2);
	
	return 0;
}

int _1990_groupe_traverse_et_positionne(Etage_Groupe *etage, int numero)
{
	if (list_traverse(etage->groupe, (void *)&numero, _1990_groupe_cherche, LIST_ALTR) != LIST_OK)
		BUG(-1);
	else
		return 0;
}

int _1990_groupe_ajout(Projet *projet, int etage, int numero, Type_Groupe_Combinaison type_combinaison)
{
	Groupe		groupe_nouveau;
	Etage_Groupe	*etage_groupe;
	
	if (list_traverse(projet->groupes, (void *)&etage, _1990_groupe_etage_cherche, LIST_ALTR) != LIST_OK)
		BUG(-1);
	etage_groupe = list_curr(projet->groupes);
	if (etage_groupe == NULL)
		BUG(-2);
	list_mvrear(etage_groupe->groupe);
	groupe_nouveau.numero = numero;
	groupe_nouveau.type_combinaison = type_combinaison;
	groupe_nouveau.tmp_combinaison.combinaisons = list_init();
	if (groupe_nouveau.tmp_combinaison.combinaisons == NULL)
		BUG(-3);
	groupe_nouveau.elements = list_init();
	if (groupe_nouveau.elements == NULL)
		BUG(-4);
	if (list_insert_after(etage_groupe->groupe, &(groupe_nouveau), sizeof(groupe_nouveau)) == NULL)
		BUG(-5);
	return 0;
}

// Etage représente le niveau du groupe.
// 1 indique le premier étage et ne peut donc contenir que des charges de projet->actions
// 2 indique le deuxième étage et ne peut contenir que des groupes du 1er étage , ....
// La formation des groupes est terminée lorsque :
// le premier étage contient toutes les actions
// le deuxième étage contient tous les groupes du premier étage
// Le dernier étage ne contient qu'un seul groupe
// Renvoie -1 si 
int _1990_groupe_ajout_element(Projet *projet, int etage, int groupe_n, int groupe_n_1)
{
	Etage_Groupe	*etage_groupe;
	Groupe		*groupe;
	Element		element_nouveau;
	int		tmp;
	
	// On commence par positionner le numéro groupe_n_1 de l'étage n-1
	if (etage == 0)
	{
		if (list_traverse(projet->actions, (void *)&groupe_n_1, _1990_action_cherche, LIST_ALTR) != LIST_OK)
			BUG(-1);
	}
	else
	{
		tmp = etage-1;
		if (list_traverse(projet->groupes, (void *)&tmp, _1990_groupe_etage_cherche, LIST_ALTR) != LIST_OK)
			BUG(-2);
		etage_groupe = list_curr(projet->groupes);
		if (list_traverse(etage_groupe->groupe, (void *)&groupe_n_1, _1990_groupe_cherche, 0) != LIST_OK)
			BUG(-3);
	}
	if (list_traverse(projet->groupes, (void *)&etage, _1990_groupe_etage_cherche, LIST_ALTR) != LIST_OK)
		BUG(-4);
	etage_groupe = list_curr(projet->groupes);
	if (etage_groupe == NULL)
		BUG(-5);
	if (list_traverse(etage_groupe->groupe, (void *)&groupe_n, _1990_groupe_cherche, LIST_ALTR) != LIST_OK)
		BUG(-6);
	groupe = list_curr(etage_groupe->groupe);
	if (groupe == NULL)
		BUG(-7);
	element_nouveau.numero = groupe_n_1;
	if (list_insert_after(groupe->elements, &(element_nouveau), sizeof(element_nouveau)) == NULL)
		BUG(-8);
	return 0;
}

void _1990_groupe_affiche_tout(Projet *projet)
{
	Etage_Groupe *etage;
	Groupe *groupe;
	Element *element;
	Combinaison *combinaison;
	Combinaison_Element *comb_element;
	Action *action;
	if ((projet->groupes != NULL) && (list_size(projet->groupes) != 0))
	{
		list_mvfront(projet->groupes);
		do
		{
			etage = (Etage_Groupe*)list_curr(projet->groupes);
			printf("etage : %d\n", etage->etage);
			printf("\tGroupes\n");
			if ((etage->groupe != NULL) && (list_size(etage->groupe) != 0))
			{
				list_mvfront(etage->groupe);
				do
				{
					groupe = (Groupe*)list_curr(etage->groupe);
					printf("\tgroupe : %d", groupe->numero);
					switch(groupe->type_combinaison)
					{
						case GROUPE_COMBINAISON_OR :
						{
							printf(" OR\n");
							break;
						}
						case GROUPE_COMBINAISON_XOR :
						{
							printf(" XOR\n");
							break;
						}
						case GROUPE_COMBINAISON_AND :
						{
							printf(" AND\n");
							break;
						}
					}
					if ((groupe->elements != NULL) && (list_size(groupe->elements) != 0))
					{
						list_mvfront(groupe->elements);
						do
						{
							element = (Element*)list_curr(groupe->elements);
							printf("%d ", element->numero);
						}
						while (list_mvnext(groupe->elements) != NULL);
					}
					printf("\n");
					printf("\tCombinaisons :\n");
					if ((groupe->tmp_combinaison.combinaisons != NULL) && (list_size(groupe->tmp_combinaison.combinaisons) != 0))
					{
						list_mvfront(groupe->tmp_combinaison.combinaisons);
						do
						{
							combinaison = (Combinaison*)list_curr(groupe->tmp_combinaison.combinaisons);
							printf("\t\tNouvelle combinaison : ");
							if ((combinaison->elements != NULL) && (list_size(combinaison->elements) != 0))
							{
								list_mvfront(combinaison->elements);
								do
								{
									comb_element = (Combinaison_Element*)list_curr(combinaison->elements);
									action = (Action*)comb_element->action;
									printf("%d(%d) ", action->numero, comb_element->flags);
								}
								while (list_mvnext(combinaison->elements) != NULL);
							}
							printf("\n");
						}
						while (list_mvnext(groupe->tmp_combinaison.combinaisons) != NULL);
					}
				}
				while (list_mvnext(etage->groupe) != NULL);
			}
		}
		while (list_mvnext(projet->groupes) != NULL);
	}
}

void _1990_groupe_free(Projet *projet)
{
	Etage_Groupe	*etage;
	Groupe		*groupe;
	Combinaison	*combinaison;
	
	if (projet->groupes == NULL)
		return;
	
	list_mvfront(projet->groupes);
	while (!list_empty(projet->groupes))
	{
		etage = list_front(projet->groupes);
		if (etage->groupe != NULL)
		{
			list_mvfront(etage->groupe);
			while (!list_empty(etage->groupe))
			{
				groupe = list_front(etage->groupe);
				if (groupe->elements != NULL)
				{
					list_mvfront(groupe->elements);
					while (!list_empty(groupe->elements))
						free(list_remove_front(groupe->elements));
					free(groupe->elements);
				}
				if (groupe->tmp_combinaison.combinaisons != NULL)
				{
					list_mvfront(groupe->tmp_combinaison.combinaisons);
					while (!list_empty(groupe->tmp_combinaison.combinaisons))
					{
						combinaison = list_front(groupe->tmp_combinaison.combinaisons);
						if (combinaison->elements != NULL)
						{
							list_mvfront(combinaison->elements);
							while (!list_empty(combinaison->elements))
								free(list_remove_front(combinaison->elements));
							free(combinaison->elements);
						}
						free(list_remove_front(groupe->tmp_combinaison.combinaisons));
					}
					free(groupe->tmp_combinaison.combinaisons);
				}
				free(list_remove_front(etage->groupe));
			}
			free(etage->groupe);
		}
		free(list_remove_front(projet->groupes));
	}
	free(projet->groupes);
	projet->groupes = NULL;
	return;
}

void _1990_groupe_free_seulement_tmp_combinaison(Projet *projet)
{
	Etage_Groupe	*etage;
	Groupe		*groupe;
	Combinaison	*combinaison;
	
	if (projet->groupes == NULL)
		return;
	
	if (list_front(projet->groupes) == NULL)
		return;
	list_mvfront(projet->groupes);
	do
	{
		etage = list_curr(projet->groupes);
		if ((etage->groupe != NULL) && (list_front(etage->groupe) != NULL))
		{
			list_mvfront(etage->groupe);
			do
			{
				groupe = list_curr(etage->groupe);
				if (groupe->tmp_combinaison.combinaisons != NULL)
				{
					list_mvfront(groupe->tmp_combinaison.combinaisons);
					while (!list_empty(groupe->tmp_combinaison.combinaisons))
					{
						combinaison = list_front(groupe->tmp_combinaison.combinaisons);
						list_free(combinaison->elements, LIST_DEALLOC);
						free(list_remove_front(groupe->tmp_combinaison.combinaisons));
					}
				}
			}
			while (list_mvnext(etage->groupe) != NULL);
		}
	}
	while (list_mvnext(projet->groupes) != NULL);
	return;
}
