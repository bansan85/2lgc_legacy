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

#include "common_projet.h"
#include "common_erreurs.h"
#include "1990_actions.h"
#include "1990_groupes.h"
#include "1990_combinaisons.h"

int _1990_groupe_init(Projet *projet)
{
	projet->niveaux_groupes = list_init();
	if (projet->niveaux_groupes == NULL)
		BUG(-1);
	else
		return 0;
}

int _1990_groupe_element_cherche(void *input, void *curr)
{
	Element *element = (Element*)curr;
	int *numero = (int*)input;
	if (element->numero == (*numero))
		return FALSE;
	else
		return TRUE;
}

int _1990_groupe_niveau_cherche(void *input, void *curr)
{
	Niveau_Groupe *niveau = (Niveau_Groupe*)curr;
	int *numero = (int*)input;
	if (niveau->niveau == (*numero))
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

int _1990_groupe_niveau_ajout(Projet *projet, int niveau)
{
	Niveau_Groupe	niveau_nouveau;
	
	list_mvrear(projet->niveaux_groupes);
	niveau_nouveau.niveau = niveau;
	niveau_nouveau.groupes = list_init();
	niveau_nouveau.pIter = NULL;
	niveau_nouveau.pIter_expand = 0;
	if (niveau_nouveau.groupes == NULL)
		BUG(-1);
	if (list_insert_after(projet->niveaux_groupes, &(niveau_nouveau), sizeof(niveau_nouveau)) == NULL)
		BUG(-2);
	
	return 0;
}

int _1990_groupe_element_traverse_et_positionne(Groupe *groupe, int numero)
{
	if (list_size(groupe->elements) == 0)
		BUG(-1);
	if (list_traverse(groupe->elements, (void *)&numero, _1990_groupe_element_cherche, LIST_ALTR) != LIST_OK)
		BUG(-2);
	else
		return 0;
}

int _1990_groupe_traverse_et_positionne(Niveau_Groupe *niveau, int numero)
{
	if (list_size(niveau->groupes) == 0)
		BUG(-1);
	if (list_traverse(niveau->groupes, (void *)&numero, _1990_groupe_cherche, LIST_ALTR) != LIST_OK)
		BUG(-2);
	else
		return 0;
}

int _1990_groupe_niveau_traverse_et_positionne(LIST *source, int numero)
{
	if (list_size(source) == 0)
		BUG(-1);
	if (list_traverse(source, (void *)&numero, _1990_groupe_niveau_cherche, LIST_ALTR) != LIST_OK)
		BUG(-2);
	else
		return 0;
}

int _1990_groupe_ajout(Projet *projet, int niveau, int numero, Type_Groupe_Combinaison type_combinaison)
{
	Groupe		groupe_nouveau;
	Niveau_Groupe	*niveau_groupe;
	
	if (list_traverse(projet->niveaux_groupes, (void *)&niveau, _1990_groupe_niveau_cherche, LIST_ALTR) != LIST_OK)
		BUG(-1);
	niveau_groupe = list_curr(projet->niveaux_groupes);
	if (niveau_groupe == NULL)
		BUG(-2);
	list_mvrear(niveau_groupe->groupes);
	groupe_nouveau.numero = numero;
	groupe_nouveau.nom = NULL;
	groupe_nouveau.type_combinaison = type_combinaison;
	groupe_nouveau.tmp_combinaison.combinaisons = list_init();
	groupe_nouveau.pIter = NULL;
	groupe_nouveau.pIter_expand = 0;
	if (groupe_nouveau.tmp_combinaison.combinaisons == NULL)
		BUG(-3);
	groupe_nouveau.elements = list_init();
	if (groupe_nouveau.elements == NULL)
		BUG(-4);
	if (list_insert_after(niveau_groupe->groupes, &(groupe_nouveau), sizeof(groupe_nouveau)) == NULL)
		BUG(-5);
	return 0;
}

// Niveau représente le niveau du groupe.
// 1 indique le premier étage et ne peut donc contenir que des charges de projet->actions
// 2 indique le deuxième étage et ne peut contenir que des groupes du 1er étage , ....
// La formation des groupes est terminée lorsque :
// le premier étage contient toutes les actions
// le deuxième étage contient tous les groupes du premier étage
// Le dernier étage ne contient qu'un seul groupe
// Renvoie -1 si 
int _1990_groupe_ajout_element(Projet *projet, int niveau, int groupe_n, int groupe_n_1)
{
	Niveau_Groupe	*niveau_groupe;
	Groupe		*groupe;
	Element		*element, element_nouveau;
	int		tmp;
	
	// On commence par positionner le numéro groupe_n_1 de l'étage n-1
	if (niveau == 0)
	{
		if (list_traverse(projet->actions, (void *)&groupe_n_1, _1990_action_cherche, LIST_ALTR) != LIST_OK)
			BUG(-1);
	}
	else
	{
		tmp = niveau-1;
		if (list_traverse(projet->niveaux_groupes, (void *)&tmp, _1990_groupe_niveau_cherche, LIST_ALTR) != LIST_OK)
			BUG(-2);
		niveau_groupe = list_curr(projet->niveaux_groupes);
		if (list_traverse(niveau_groupe->groupes, (void *)&groupe_n_1, _1990_groupe_cherche, 0) != LIST_OK)
			BUG(-3);
	}
	if (list_traverse(projet->niveaux_groupes, (void *)&niveau, _1990_groupe_niveau_cherche, LIST_ALTR) != LIST_OK)
		BUG(-4);
	niveau_groupe = list_curr(projet->niveaux_groupes);
	if (niveau_groupe == NULL)
		BUG(-5);
	if (list_traverse(niveau_groupe->groupes, (void *)&groupe_n, _1990_groupe_cherche, LIST_ALTR) != LIST_OK)
		BUG(-6);
	groupe = list_curr(niveau_groupe->groupes);
	if (groupe == NULL)
		BUG(-7);
	element_nouveau.numero = groupe_n_1;
	element_nouveau.pIter = NULL;
	element_nouveau.pIter_expand = 0;
	if (list_size(groupe->elements) == 0)
	{
		if (list_insert_after(groupe->elements, &(element_nouveau), sizeof(element_nouveau)) == NULL)
			BUG(-8);
	}
	else
	{
		list_mvfront(groupe->elements);
		tmp = 0;
		do
		{
			element = list_curr(groupe->elements);
			if (element->numero > groupe_n_1)
			{
				if (list_insert_before(groupe->elements, &(element_nouveau), sizeof(element_nouveau)) == NULL)
					BUG(-9);
				tmp = 1;
			}
		}
		while (list_mvnext(groupe->elements) && (tmp == 0));
		if (tmp == 0)
		{
			if (list_insert_after(groupe->elements, &(element_nouveau), sizeof(element_nouveau)) == NULL)
				BUG(-10);
		}
	}
	return 0;
}

void _1990_groupe_affiche_tout(Projet *projet)
{
	Niveau_Groupe *niveau;
	Groupe *groupe;
	Element *element;
	Combinaison *combinaison;
	Combinaison_Element *comb_element;
	Action *action;
	if ((projet->niveaux_groupes != NULL) && (list_size(projet->niveaux_groupes) != 0))
	{
		list_mvfront(projet->niveaux_groupes);
		do
		{
			niveau = (Niveau_Groupe*)list_curr(projet->niveaux_groupes);
			printf("niveau : %d\n", niveau->niveau);
			printf("\tGroupes\n");
			if ((niveau->groupes != NULL) && (list_size(niveau->groupes) != 0))
			{
				list_mvfront(niveau->groupes);
				do
				{
					groupe = (Groupe*)list_curr(niveau->groupes);
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
				while (list_mvnext(niveau->groupes) != NULL);
			}
		}
		while (list_mvnext(projet->niveaux_groupes) != NULL);
	}
}

int _1990_groupe_free_element_courant(LIST *elements)
{
	Element		*element;
	
	if ((elements == NULL) || (list_curr(elements) == NULL))
		BUG(-1);
	element = list_curr(elements);
	
	if (element->pIter != NULL)
		free(element->pIter);
	free(list_remove_curr(elements));
	
	return 0;
}

int _1990_groupe_free_groupe_courant(LIST *groupes)
{
	Groupe		*groupe;
	Combinaison	*combinaison;
	
	groupe = list_curr(groupes);
	if (groupe == NULL)
		BUG(-1);
	if (groupe->pIter != NULL)
		free(groupe->pIter);
	if (groupe->elements != NULL)
	{
		list_mvfront(groupe->elements);
		while (!list_empty(groupe->elements))
			_1990_groupe_free_element_courant(groupe->elements);
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
	free(list_remove_curr(groupes));
	
	return 0;
}

int _1990_groupe_free_niveau_courant(Projet *projet)
{
	Niveau_Groupe	*niveau;
	
	if ((projet->niveaux_groupes == NULL) || (list_curr(projet->niveaux_groupes) == NULL))
		BUG(-1);
	
	niveau = list_curr(projet->niveaux_groupes);
	if (niveau->pIter != NULL)
		free(niveau->pIter);
	if (niveau->groupes != NULL)
	{
		list_mvfront(niveau->groupes);
		while (!list_empty(niveau->groupes))
			_1990_groupe_free_groupe_courant(niveau->groupes);
		free(niveau->groupes);
	}
	free(list_remove_curr(projet->niveaux_groupes));
	return 0;
}

int _1990_groupe_free_niveau_numero(Projet *projet, int numero)
{
	Niveau_Groupe	*niveau_groupe;
	if (_1990_groupe_niveau_traverse_et_positionne(projet->niveaux_groupes, numero) != 0)
		BUG(-1);
	niveau_groupe = list_curr(projet->niveaux_groupes);
	do
	{
		if (niveau_groupe->niveau >= numero)
		{
			if (_1990_groupe_free_niveau_courant(projet) != 0)
				BUG(-2);
			niveau_groupe = list_curr(projet->niveaux_groupes);
		}
		else
		{
			list_mvnext(projet->niveaux_groupes);
			niveau_groupe = list_curr(projet->niveaux_groupes);
		}
	}
	while ((list_size(projet->niveaux_groupes) != 0) && (niveau_groupe != NULL) && (niveau_groupe->niveau >= numero));
	return 0;
}

int _1990_groupe_free_groupe_numero(Projet *projet, int niveau, int numero)
{
	Niveau_Groupe	*niveau_groupe;
	Groupe		*groupe;
	Element		*element;
	int		dernier;
	if ((projet->niveaux_groupes == NULL) || (list_size(projet->niveaux_groupes) == 0))
		BUG(-1);
	if (_1990_groupe_niveau_traverse_et_positionne(projet->niveaux_groupes, niveau) != 0)
		BUG(-2);
	niveau_groupe = list_curr(projet->niveaux_groupes);
	if ((niveau_groupe->groupes == NULL) || (list_size(niveau_groupe->groupes) == 0))
		BUG(-3);
	if (_1990_groupe_traverse_et_positionne(niveau_groupe, numero) != 0)
		BUG(-4);
	if (_1990_groupe_free_groupe_courant(niveau_groupe->groupes) != 0)
		BUG(-5);
	
	if (list_curr(niveau_groupe->groupes) != NULL)
	{
		list_mvfront(niveau_groupe->groupes);
		do
		{
			groupe = list_curr(niveau_groupe->groupes);
			if (groupe->numero > numero)
				groupe->numero--;
		}
		while (list_mvnext(niveau_groupe->groupes));
	}
	niveau++;
	if (list_traverse(projet->niveaux_groupes, (void *)&niveau, _1990_groupe_niveau_cherche, LIST_ALTR) == LIST_OK)
	{
		niveau_groupe = list_curr(projet->niveaux_groupes);
		if (niveau_groupe != NULL)
		{
			if (list_curr(niveau_groupe->groupes) != NULL)
			{
				list_mvfront(niveau_groupe->groupes);
				do
				{
					groupe = list_curr(niveau_groupe->groupes);
					if (list_curr(groupe->elements) != NULL)
					{
						list_mvfront(groupe->elements);
						do
						{
							dernier = 0;
							element = list_curr(groupe->elements);
							if (element->numero == numero)
							{
								if (element == list_rear(groupe->elements))
									dernier = 1;
								free(list_remove_curr(groupe->elements));
								element = list_curr(groupe->elements);
							}
							if ((dernier == 0) && (element->numero > numero))
								element->numero--;
						}
						while (list_mvnext(groupe->elements) != NULL);
					}
				}
				while (list_mvnext(niveau_groupe->groupes));
			}
		}
	}
	return 0;
}

int _1990_groupe_free_element_numero(Projet *projet, int niveau, int numero, int element)
{
	Niveau_Groupe	*niveau_groupe;
	Groupe		*groupe;
	if ((projet->niveaux_groupes == NULL) || (list_size(projet->niveaux_groupes) == 0))
		BUG(-1);
	if (_1990_groupe_niveau_traverse_et_positionne(projet->niveaux_groupes, niveau) != 0)
		BUG(-2);
	niveau_groupe = list_curr(projet->niveaux_groupes);
	if ((niveau_groupe->groupes == NULL) || (list_size(niveau_groupe->groupes) == 0))
		BUG(-3);
	if (_1990_groupe_traverse_et_positionne(niveau_groupe, numero) != 0)
		BUG(-4);
	groupe = list_curr(niveau_groupe->groupes);
	if ((groupe->elements == NULL) || (list_size(groupe->elements) == 0))
		BUG(-5);
	if (_1990_groupe_element_traverse_et_positionne(groupe, element) != 0)
		BUG(-6);
	if (_1990_groupe_free_element_courant(groupe->elements) != 0)
		BUG(-7);
	
	return 0;
}

void _1990_groupe_free(Projet *projet)
{
	Niveau_Groupe	*niveau;
	Groupe		*groupe;
	Element		*element;
	Combinaison	*combinaison;
	
	if (projet->niveaux_groupes == NULL)
		return;
	
	list_mvfront(projet->niveaux_groupes);
	while (!list_empty(projet->niveaux_groupes))
	{
		niveau = list_front(projet->niveaux_groupes);
		if (niveau->pIter != NULL)
			free(niveau->pIter);
		if (niveau->groupes != NULL)
		{
			list_mvfront(niveau->groupes);
			while (!list_empty(niveau->groupes))
			{
				groupe = list_front(niveau->groupes);
				if (groupe->pIter != NULL)
					free(groupe->pIter);
				if (groupe->elements != NULL)
				{
					list_mvfront(groupe->elements);
					while (!list_empty(groupe->elements))
					{
						element = list_front(groupe->elements);
						free(element->pIter);
						free(list_remove_front(groupe->elements));
					}
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
				free(list_remove_front(niveau->groupes));
			}
			free(niveau->groupes);
		}
		free(list_remove_front(projet->niveaux_groupes));
	}
	free(projet->niveaux_groupes);
	projet->niveaux_groupes = NULL;
	return;
}

void _1990_groupe_free_seulement_tmp_combinaison(Projet *projet)
{
	Niveau_Groupe	*niveau;
	Groupe		*groupe;
	Combinaison	*combinaison;
	
	if ((projet->niveaux_groupes == NULL) || (list_size(projet->niveaux_groupes) == 0))
		return;
	
	list_mvfront(projet->niveaux_groupes);
	do
	{
		niveau = list_curr(projet->niveaux_groupes);
		if ((niveau->groupes != NULL) && (list_front(niveau->groupes) != NULL))
		{
			list_mvfront(niveau->groupes);
			do
			{
				groupe = list_curr(niveau->groupes);
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
			while (list_mvnext(niveau->groupes) != NULL);
		}
	}
	while (list_mvnext(projet->niveaux_groupes) != NULL);
	return;
}
