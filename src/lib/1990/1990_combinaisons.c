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

#include "common_projet.h"
#include "common_erreurs.h"
#include "common_maths.h"

#include "1990_actions.h"
#include "1990_groupes.h"
#include "1990_combinaisons.h"
#include "1990_ponderations.h"


/* _1990_combinaisons_verifie_double
 * Description : Vérifie si une combinaison est déjà présente dans une liste de combinaisons
 * Paramètres : LIST *combinaisons : liste de combinaisons
 *            : Combinaison *combinaison_a_verifier : combinaison à vérifier
 * Valeur renvoyée :
 *   Succès : 0 si la combinaison n'est pas présente
 *          : 1 si la combinaison est présente
 *          : 1 si la combinaison est vide (afin de ne pas l'ajouter)
 *   Échec : -1 en cas d'erreur
 */
int _1990_combinaisons_verifie_double(LIST *liste_combinaisons, Combinaison *combinaison_a_verifier)
{
	if ((liste_combinaisons == NULL) || (combinaison_a_verifier == NULL))
		BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
	
	if (list_size(liste_combinaisons) == 0)
		return 0;
	if (list_size(combinaison_a_verifier->elements) == 0)
		return 1;
	
	list_mvfront(liste_combinaisons);
	do
	{
		int		doublon = 1;
		Combinaison	*combinaison_en_cours = (Combinaison*)list_curr(liste_combinaisons);
		
		// On vérifie si la combinaison en cours possède le même nombre d'élément que la combinaison à vérifier
		if (list_size(combinaison_en_cours->elements) == list_size(combinaison_a_verifier->elements))
		{
			list_mvfront(combinaison_en_cours->elements);
			list_mvfront(combinaison_a_verifier->elements);
			do
			{
				Combinaison_Element	*elem1 = list_curr(combinaison_en_cours->elements);
				Combinaison_Element	*elem2 = list_curr(combinaison_a_verifier->elements);
				// On vérifie que chaque élément pointe vers la même action
				// et que les flags (paramètres de calculs) sont les mêmes
				if ((elem1->action != elem2->action) || (elem1->flags != elem2->flags))
					doublon = 0;
			}
			while ((list_mvnext(combinaison_en_cours->elements) != NULL) && (list_mvnext(combinaison_a_verifier->elements) != NULL) && (doublon == 1));
			if (doublon == 1)
				return 1;
		}
	}
	while (list_mvnext(liste_combinaisons) != NULL);
	
	return 0;
}

/* _1990_combinaisons_duplique_avec_double
 * Description : ajoute à une liste de combinaisons existante une liste de combinaisons.
 *             : aucune vérification n'est effectuée pour s'assurer que la liste source ne
 *               possède pas une ou plusieurs combinaisons identique  que la liste de destination
 * Paramètres : LIST *liste_comb_destination : liste de combinaisons qui recevra les combinaisons sources
 *            : LIST *liste_comb_destination : liste de combinaisons source
 * Valeur renvoyée :
 *   Succès : 0 si les combinaisons sont ajoutées avec succès
 *   Échec : valeur négative en cas d'erreur
 */
int _1990_combinaisons_duplique_avec_double(LIST *liste_comb_destination, LIST *liste_comb_source)
{
	if ((liste_comb_destination == NULL) || (liste_comb_source == NULL))
		BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
	
	if (list_size(liste_comb_source) == 0)
		return 0;
	list_mvrear(liste_comb_destination);
	list_mvfront(liste_comb_source);
	do
	{
		Combinaison	*combinaison_source = list_curr(liste_comb_source);
		Combinaison	combinaison_destination;
		
		combinaison_destination.elements = list_init();
		if (combinaison_destination.elements == NULL)
			BUGTEXTE(-2, gettext("Erreur d'allocation mémoire.\n"));
		if ((combinaison_source != NULL) && (list_curr(combinaison_source->elements) != NULL))
		{
			list_mvfront(combinaison_source->elements);
			do
			{
				Combinaison_Element	*element_source = list_curr(combinaison_source->elements);
				Combinaison_Element	element_destination;
				
				element_destination.action = element_source->action;
				element_destination.flags = element_source->flags;
				if (list_insert_after(combinaison_destination.elements, (void*)&element_destination, sizeof(element_destination)) == NULL)
					BUGTEXTE(-3, gettext("Erreur d'allocation mémoire.\n"));
			}
			while (list_mvnext(combinaison_source->elements) != NULL);
		}
		if (list_insert_after(liste_comb_destination, (void*)&combinaison_destination, sizeof(combinaison_destination)) == NULL)
			BUGTEXTE(-4, gettext("Erreur d'allocation mémoire.\n"));
	}
	while (list_mvnext(liste_comb_source) != NULL);
	
	return 0;
}

/* _1990_combinaisons_duplique_sans_double
 * Description : ajoute à une liste de combinaisons existante une liste de combinaisons.
 *             : une vérification est effectuée pour s'assurer que la liste source ne
 *               possède pas une ou plusieurs combinaisons identique que la liste de destination
 * Paramètres : LIST *liste_comb_destination : liste de combinaisons qui recevra les combinaisons sources
 *            : LIST *liste_comb_source : liste de combinaisons source
 * Valeur renvoyée :
 *   Succès : 0 si les combinaisons sont ajoutées avec succès
 *   Échec : valeur négative en cas d'erreur
 */
int _1990_combinaisons_duplique_sans_double(LIST *liste_comb_destination, LIST *liste_comb_source)
{
	if ((liste_comb_destination == NULL) || (liste_comb_source == NULL))
		BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
	
	if (list_size(liste_comb_source) == 0)
		return 0;
	
	list_mvrear(liste_comb_destination);
	list_mvfront(liste_comb_source);
	do
	{
		Combinaison	*combinaison_source = list_curr(liste_comb_source);
		
		switch (_1990_combinaisons_verifie_double(liste_comb_destination, combinaison_source))
		{
			// La combinaison n'est pas présente. On l'ajoute donc
			case 0 :
			{
				Combinaison	combinaison_destination;
				
				combinaison_destination.elements = list_init();
				if (combinaison_destination.elements == NULL)
					BUGTEXTE(-2, gettext("Erreur d'allocation mémoire.\n"));
				if ((combinaison_source != NULL) && (list_curr(combinaison_source->elements) != NULL))
				{
					list_mvfront(combinaison_source->elements);
					do
					{
						Combinaison_Element	*element_source = list_curr(combinaison_source->elements);
						Combinaison_Element	element_destination;
						
						element_destination.action = element_source->action;
						element_destination.flags = element_source->flags;
						if (list_insert_after(combinaison_destination.elements, (void*)&element_destination, sizeof(element_destination)) == NULL)
							BUGTEXTE(-3, gettext("Erreur d'allocation mémoire.\n"));
			
					}
					while (list_mvnext(combinaison_source->elements) != NULL);
				}
				if (list_insert_after(liste_comb_destination, (void*)&combinaison_destination, sizeof(combinaison_destination)) == NULL)
					BUGTEXTE(-4, gettext("Erreur d'allocation mémoire.\n"));
				break;
			}
			case 1 :
			{
				break;
			}
			default :
			{
				BUG(-5);
			}
		}
	}
	while (list_mvnext(liste_comb_source) != NULL);
	
	return 0;
}

/* _1990_combinaisons_action_prioritaire
 * Description : attribut toutes les actions variables d'une combinaisons comme action prioritaire 
 *             : en plaçant son (flags&1) à 1.
 * Paramètres : Combinaison *combinaison : combinaison à modifier
 *            : Type_Pays pays : le payx
 * Valeur renvoyée :
 *   Succès : 0 si la combinaison est modifiée avec succès
 *   Échec : valeur négative en cas d'erreur
 */
int _1990_combinaisons_action_prioritaire(Combinaison *combinaison, Type_Pays pays)
{
	if (combinaison == NULL)
		BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
	
	if (list_size(combinaison->elements) == 0)
		return 0;
	
	list_mvfront(combinaison->elements);
	do
	{
		Combinaison_Element	*combinaison_element = list_curr(combinaison->elements);
		if (_1990_action_type_combinaison_bat(combinaison_element->action->categorie, pays) == 2)
			combinaison_element->flags = 1;
	}
	while (list_mvnext(combinaison->elements));
	
	return 0;
}

/* _1990_combinaisons_genere_groupe_xor
 * Description : Génère toutes les combinaisons d'un groupe possédant comme attribut de combinaison XOR.
 *             : Le groupe étudié est déterminé par le groupe en cours du niveau en cours
 * Paramètres : Projet	*projet : la variable projet
 * Valeur renvoyée :
 *   Succès : 0 si les combinaisons sont générées avec succès
 *   Échec : valeur négative en cas d'erreur
 */
int _1990_combinaisons_genere_groupe_xor(Projet *projet)
{
	Groupe			*groupe;
	Niveau_Groupe		*niveau;
	
	if ((projet == NULL) || (projet->niveaux_groupes == NULL) || (list_size(projet->niveaux_groupes) == 0))
		BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
	
	niveau = list_curr(projet->niveaux_groupes);
	
	if ((niveau->groupes == NULL) || (list_size(niveau->groupes) == 0))
		BUGTEXTE(-2, gettext("Paramètres invalides.\n"));
	
	groupe = list_curr(niveau->groupes);
	if (groupe->type_combinaison != GROUPE_COMBINAISON_XOR)
		BUGTEXTE(-3, gettext("Paramètres invalides.\n"));
	
	// Si le nombre d'éléments est nul, il n'y a aucune combinaison à réaliser
	if (list_empty(groupe->elements) == TRUE)
		return 0;
	
	list_mvfront(groupe->elements);
	
	// Premier étage (l'étage inférieur est donc les actions)
	// Un XOR pour premier étage consiste à ajouter les actions à la suite des autres sans aucune combinaison
	// On crée donc autant de combinaisons qu'il y a d'éléments dans le groupe
	// Chaque combinaison n'ayant qu'un seul élément.
	if (list_curr(projet->niveaux_groupes) == list_front(projet->niveaux_groupes))
	{
		
		do
		{
			Element			*element_en_cours;
			Combinaison		nouvelle_combinaison;
			Combinaison_Element	nouveau_element;
			
			element_en_cours = (Element*)list_curr(groupe->elements);
			if (_1990_action_positionne(projet, element_en_cours->numero) != 0)
				BUG(-4);
			nouvelle_combinaison.elements = list_init();
			if (nouvelle_combinaison.elements == NULL)
				BUGTEXTE(-5, gettext("Erreur d'allocation mémoire.\n"));
			nouveau_element.action = list_curr(projet->actions);
			nouveau_element.flags = nouveau_element.action->flags;
			if (list_insert_after(nouvelle_combinaison.elements, (void*)&nouveau_element, sizeof(nouveau_element)) == NULL)
				BUGTEXTE(-6, gettext("Erreur d'allocation mémoire.\n"));
			if (list_insert_after(groupe->tmp_combinaison.combinaisons, &(nouvelle_combinaison), sizeof(nouvelle_combinaison)) == NULL)
				BUGTEXTE(-7, gettext("Erreur d'allocation mémoire.\n"));
		}
		while (list_mvnext(groupe->elements) != NULL);
	}
	// Autres étages (l'étage inférieur est donc des combinaisons intermédiaires)
	// Un XOR pour les autres égage consiste à ajouter toutes les combinaisons des
	// groupe de l'étage n-1 qui sont indiquées dans le groupe de l'étage n.
	else
	{
		list_mvprev(projet->niveaux_groupes);
		niveau = list_curr(projet->niveaux_groupes);
		do
		{
			Element		*element_tmp = (Element*)list_curr(groupe->elements);
			Groupe		*groupe_n_1;
			
			if (_1990_groupe_positionne(niveau, element_tmp->numero) != 0)
				BUG(-8);
			groupe_n_1 = list_curr(niveau->groupes);
			if (_1990_combinaisons_duplique_sans_double(groupe->tmp_combinaison.combinaisons, groupe_n_1->tmp_combinaison.combinaisons) != 0)
				BUG(-9);
		}
		while (list_mvnext(groupe->elements) != NULL);
		
		list_mvnext(projet->niveaux_groupes);
	}
	
	return 0;
}

/* _1990_combinaisons_fusion
 * Description : fusionne deux combinaisons. Les éléments de la combinaison source
 *             : sont ajoutée à la fin de la combinaison destination
 * Paramètres : Combinaison *destination : combinaison de destination
 *            : Combinaison *source : combinaison source
 * Valeur renvoyée :
 *   Succès : 0 si les combinaisons sont générées avec succès
 *   Échec : valeur négative en cas d'erreur
 */
int _1990_combinaisons_fusion(Combinaison *destination, Combinaison *source)
{
 	if ((destination == NULL) || (source == NULL) || (destination->elements == NULL) || (source->elements == NULL))
		BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
	
	// Rien à jouter
	if (list_size(source->elements) == 0)
		return 0;
	
	list_mvrear(destination->elements);
	list_mvfront(source->elements);
	do
	{
		Combinaison_Element	*element_source = list_curr(source->elements);
		Combinaison_Element	element_destination;
		
		element_destination.action = element_source->action;
		element_destination.flags = element_source->flags;
		if (list_insert_after(destination->elements, (void*)&element_destination, sizeof(element_destination)) == NULL)
			BUGTEXTE(-2, gettext("Erreur d'allocation mémoire.\n"));
	}
	while (list_mvnext(source->elements) != NULL);
	
	return 0;
}

/* _1990_groupe_free_groupe_tmp_combinaison
 * Description : Permet de supprimer toutes les combinaisons temporaires contenues dans les groupes
 *             : À utiliser avec la fonction list_traverse
 * Paramètres : void *data : donnée à libérer
 * Valeur renvoyée : Aucune
 */
void _1990_groupe_free_groupe_tmp_combinaison(void *data)
{
	Combinaison *combinaison = (Combinaison*)data;
	if (combinaison->elements != NULL)
		list_free(combinaison->elements, LIST_DEALLOC);
	free(data);
	return;
}

/* _1990_combinaisons_genere_groupe_and
 * Description : Génère toutes les combinaisons d'un groupe possédant comme attribut de combinaison AND.
 *             : Le groupe étudié est déterminé par le groupe en cours du niveau en cours
 * Paramètres : Projet	*projet : la variable projet
 * Valeur renvoyée :
 *   Succès : 0 si la combinaison est modifiée avec succès
 *   Échec : valeur négative en cas d'erreur
 */
int _1990_combinaisons_genere_groupe_and(Projet *projet)
{
	Niveau_Groupe		*niveau;
	Groupe			*groupe_n_1, *groupe;
	
	if ((projet == NULL) || (projet->niveaux_groupes == NULL) || (list_size(projet->niveaux_groupes) == 0))
		BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
	
	niveau = list_curr(projet->niveaux_groupes);
	
	if ((niveau->groupes == NULL) || (list_size(niveau->groupes) == 0))
		BUGTEXTE(-2, gettext("Paramètres invalides.\n"));
	
	groupe = list_curr(niveau->groupes);
	if (groupe->type_combinaison != GROUPE_COMBINAISON_AND)
		BUGTEXTE(-3, gettext("Paramètres invalides.\n"));
	
	if (list_empty(groupe->elements) == TRUE)
		return 0;
	
	// Premier étage
	// La génération consiste à créer une combinaison contenant l'ensemble des actions du groupe
	if (list_curr(projet->niveaux_groupes) == list_front(projet->niveaux_groupes))
	{
		Combinaison	comb;
		int		action_predominante = 0;	// Indique la présence d'une action dominante parmi les actions du groupe
		
		comb.elements = list_init();
		if (comb.elements == NULL)
			BUGTEXTE(-4, gettext("Erreur d'allocation mémoire.\n"));
		
		list_mvfront(groupe->elements);
		do
		{
			Element			*element_en_cours = (Element*)list_curr(groupe->elements);
			Combinaison_Element	nouveau_element;
			
			if (_1990_action_positionne(projet, element_en_cours->numero) != 0)
				BUG(-4);
			nouveau_element.action = list_curr(projet->actions);
			nouveau_element.flags = nouveau_element.action->flags;
			if ((nouveau_element.flags & 1) != 0)
				action_predominante = 1;
			if (list_insert_after(comb.elements, (void*)&nouveau_element, sizeof(nouveau_element)) == NULL)
				BUGTEXTE(-5, gettext("Erreur d'allocation mémoire.\n"));
		}
		while (list_mvnext(groupe->elements) != NULL);
		
		// Dès qu'une seule action est considérée comme dominante, toutes les actions doivent l'être aussi
		if (action_predominante == 1)
			_1990_combinaisons_action_prioritaire(&(comb), projet->pays);
		if (list_insert_after(groupe->tmp_combinaison.combinaisons, &(comb), sizeof(comb)) == NULL)
			BUGTEXTE(-6, gettext("Erreur d'allocation mémoire.\n"));
	}
	// Autre étage
	// La génération consiste à créer un nombre de combinaisons égal au produit du nombre de combinaison de chaque élément du groupe.
	// Par exemple, si il y a trois groupes contenant respectivement 1, 2 et 3 éléments, il y aura ainsi 1*2*3 combinaisons différentes.
	// Les combinaisons sont réalisées de la façon suivante :
	// 	- Première passe : on ajoute l'ensemble des combinaisons du premier éléments.
	// 		soit (dans le cas de notre exemple) : 1
	// 	- Deuxième passe : on duplique l'ensemble des combinaisons du premier élément autant de fois qu'il y a d'élément dans le groupe 2.
	// 		soit : 1, 1
	// 	- Deuxième passe : on ajoute à toutes les combinaisons créées les éléments du groupe 2.
	// 		soit : 1 2_1, 1 2_2
	// 	- Troisième passe : on recommence au niveau de la deuxième passe :
	// 		soit : 1 2_1, 1 2_2, 1 2_1, 1 2_2, 1 2_1, 1 2_2
	//		soit : 1 2_1 3_1, 1 2_2 3_1, 1 2_1 3_2, 1 2_2 3_2, 1 2_1 3_3, 1 2_2 3_3
	else
	{
		// nouvelles_combinaisons contient de façon temporaire (avant l'ajout final dans le groupe en cours)
		LIST		*nouvelles_combinaisons;
		
		nouvelles_combinaisons = list_init();
		if (nouvelles_combinaisons == NULL)
			BUGTEXTE(-7, gettext("Erreur d'allocation mémoire.\n"));
		
		list_mvprev(projet->niveaux_groupes);
		niveau = list_curr(projet->niveaux_groupes);
		list_mvfront(groupe->elements);
		do
		{
			Element		*element_en_cours = (Element*)list_curr(groupe->elements);
			
			// On se positionne sur l'élément en cours du groupe
			if (_1990_groupe_positionne(niveau, element_en_cours->numero) != 0)
				BUG(-8);
			groupe_n_1 = list_curr(niveau->groupes);
			
			// Alors, il s'agit de la première passe. On duplique donc simplement.
			if (list_front(groupe->elements) == element_en_cours)
			{
				if (_1990_combinaisons_duplique_avec_double(nouvelles_combinaisons, groupe_n_1->tmp_combinaison.combinaisons) != 0)
					BUG(-9);
			}
			else
			{
				// transition est utilisée de façon temporaire pour dupliquer nouvelles_combinaisons en cas de besoin
				LIST		*transition = list_init();
				unsigned int	j;
				int		i, nbboucle;
				
				if (transition == NULL)
					BUGTEXTE(-10, gettext("Erreur d'allocation mémoire.\n"));
				
				// On duplique les combinaisons actuellement dans nouvelles_combinaisons autant de fois (moins 1)
				// qu'il y a d'éléments dans le groupe de la passe actuelle. (première partie de la passe 2)
				if (_1990_combinaisons_duplique_avec_double(transition, nouvelles_combinaisons) != 0)
					BUG(-11);
				nbboucle = list_size(groupe_n_1->tmp_combinaison.combinaisons);
				for (i=2;i<=nbboucle;i++)
				{
					if (_1990_combinaisons_duplique_avec_double(nouvelles_combinaisons, transition) != 0)
						BUG(-12);
				}
				
				// On ajoute à la fin de toutes les combinaisons dupliquées les combinaisons
				// contenues dans le groupe en cours (deuxième partie de la passe 2)
				list_mvfront(nouvelles_combinaisons);
				list_mvfront(groupe_n_1->tmp_combinaison.combinaisons);
				for (i=1;i<=nbboucle;i++)
				{
					Combinaison	*combinaison2;
					
					combinaison2 = (Combinaison*)list_curr(groupe_n_1->tmp_combinaison.combinaisons);
					for (j=1;j<=list_size(transition);j++)
					{
						Combinaison	*combinaison1;
						combinaison1 = (Combinaison*)list_curr(nouvelles_combinaisons);
						if (_1990_combinaisons_fusion(combinaison1, combinaison2) != 0)
							BUG(-13);
						list_mvnext(nouvelles_combinaisons);
					}
					list_mvnext(groupe_n_1->tmp_combinaison.combinaisons);
				}
				
				list_free(transition, &(_1990_groupe_free_groupe_tmp_combinaison));
			}
		}
		while (list_mvnext(groupe->elements) != NULL);
		
		// On ajoute définitivement les nouvelles combinaisons
		if (_1990_combinaisons_duplique_sans_double(groupe->tmp_combinaison.combinaisons, nouvelles_combinaisons) != 0)
			BUG(-14);
		list_free(nouvelles_combinaisons, &(_1990_groupe_free_groupe_tmp_combinaison));
		
		list_mvnext(projet->niveaux_groupes);
	}
	
	return 0;
}

/* _1990_combinaisons_genere_groupe_or
 * Description : Génère toutes les combinaisons d'un groupe possédant comme attribut de combinaison OR.
 *             : Le groupe étudié est déterminé par le groupe en cours du niveau en cours
 * Paramètres : Projet	*projet : la variable projet
 * Valeur renvoyée :
 *   Succès : 0 si les combinaisons sont générées avec succès
 *   Échec : valeur négative en cas d'erreur
 */
int _1990_combinaisons_genere_groupe_or(Projet *projet)
{
	Niveau_Groupe		*niveau;
	Groupe			*groupe;
	Combinaison_Element	element;
	unsigned int		boucle, i, j, k;
	
	if ((projet == NULL) || (projet->niveaux_groupes == NULL) || (list_size(projet->niveaux_groupes) == 0))
		BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
	
	niveau = list_curr(projet->niveaux_groupes);
	
	if ((niveau->groupes == NULL) || (list_size(niveau->groupes) == 0))
		BUGTEXTE(-2, gettext("Paramètres invalides.\n"));
	
	groupe = list_curr(niveau->groupes);
	if (groupe->type_combinaison != GROUPE_COMBINAISON_OR)
		BUGTEXTE(-3, gettext("Paramètres invalides.\n"));
	
	if (list_size(groupe->elements) == 0)
		return 0;
	
	// Afin de générer l'ensemble des combinaisons, il va être nécessaire de réaliser une boucle
	// de 2^(list_size(groupe->elements))
	// Le principe consiste à générer toutes les combinaisons possibles avec une combinaison de type OR
	// Ainsi, dans le cas où il y a trois éléments dans un groupe, il est donc possible de générer les combinaisons suivantes :
	// 000
	// 001
	// 010
	// 011
	// 100
	// 101
	// 110
	// 111
	// Soit bien 2^3 = 8 possibilités.
	// Chaque bit correspond à un élément.
	// Lorsque le bit vaut 0, l'élément n'est pas pris en compte dans la combinaison
	// Lorsque le bit vaut 1, l'élément est pris en compte dans la combinaison.
	// Cette approche n'est valable que pour le premier étage.
	boucle = 2;
	for (i=2;i<=list_size(groupe->elements);i++)
		boucle = boucle*2;
	
	// Premier étage
	if (list_curr(projet->niveaux_groupes) == list_front(projet->niveaux_groupes))
	{
		for (i=0;i<boucle;i++)
		{
			int		parcours_bits = i;
			int		action_predominante = 0;
			Combinaison	nouvelle_combinaison;
			
			nouvelle_combinaison.elements = list_init();
			if (nouvelle_combinaison.elements == NULL)
				BUGTEXTE(-4, gettext("Erreur d'allocation mémoire.\n"));
			
			list_mvfront(groupe->elements);
			do
			{
				if ((parcours_bits & 1) == 1)
				{
					Element		*element_en_cours = (Element*)list_curr(groupe->elements);
					
					if (_1990_action_positionne(projet, element_en_cours->numero) != 0)
						BUG(-5);
					element.action = list_curr(projet->actions);
					element.flags = element.action->flags;
					if ((element.flags & 1) != 0)
						action_predominante = 1;
					if (list_insert_after(nouvelle_combinaison.elements, (void*)&element, sizeof(element)) == NULL)
						BUGTEXTE(-6, gettext("Erreur d'allocation mémoire.\n"));
				}
				parcours_bits = parcours_bits >> 1;
				list_mvnext(groupe->elements);
			}
			while (parcours_bits != 0);
			
			// Dans le premier étage, lorsqu'une action variable est prédominante, l'ensemble des actions variables le devient
			if (action_predominante == 1)
				_1990_combinaisons_action_prioritaire(&(nouvelle_combinaison), projet->pays);
			if (list_insert_after(groupe->tmp_combinaison.combinaisons, &(nouvelle_combinaison), sizeof(nouvelle_combinaison)) == NULL)
				BUGTEXTE(-7, gettext("Erreur d'allocation mémoire.\n"));
		}
	}
	else
	{
		// On effectue le même principe que pour le premier étage :
		// 	- Création d'une boucle pour générer 2^(list_size(groupe->elements)) combinaisons
		list_mvprev(projet->niveaux_groupes);
		niveau = list_curr(projet->niveaux_groupes);
		for (i=0;i<boucle;i++)
		{
			int		parcours_bits = i;
			LIST		*nouvelles_combinaisons;
			
			nouvelles_combinaisons = list_init();
			if (nouvelles_combinaisons == NULL)
				BUGTEXTE(-8, gettext("Erreur d'allocation mémoire.\n"));
			
			list_mvfront(groupe->elements);
			do
			{
				if ((parcours_bits & 1) == 1)
				{
					Element		*element_en_cours = (Element*)list_curr(groupe->elements);
					Groupe		*groupe_n_1;
					
					if (_1990_groupe_positionne(niveau, element_en_cours->numero) != 0)
						BUG(-9);
					groupe_n_1 = list_curr(niveau->groupes);
					if (list_size(groupe_n_1->tmp_combinaison.combinaisons) != 0)
					{
						// Alors, il s'agit de la première passe. On duplique donc simplement.
						if (list_size(nouvelles_combinaisons) == 0)
						{
							if (_1990_combinaisons_duplique_avec_double(nouvelles_combinaisons, groupe_n_1->tmp_combinaison.combinaisons) != 0)
								BUG(-10);
						}
						else
						{
							// transition est utilisée de façon temporaire pour dupliquer nouvelles_combinaisons en cas de besoin
							LIST		*transition = list_init();
							
							if (transition == NULL)
								BUGTEXTE(-11, gettext("Erreur d'allocation mémoire.\n"));
							
							if (_1990_combinaisons_duplique_avec_double(transition, nouvelles_combinaisons) != 0)
								BUG(-12);
							
							// On duplique les combinaisons actuellement dans nouvelles_combinaisons autant de fois (moins 1)
							// qu'il y a d'éléments dans le groupe de la passe actuelle.
							for (j=2;j<=list_size(groupe_n_1->tmp_combinaison.combinaisons);j++)
							{
								if (_1990_combinaisons_duplique_avec_double(nouvelles_combinaisons, transition) != 0)
									BUG(-13);
							}
							
							// Ensuite on fusionne chaque série de doublon créée avec une combinaison provenant de groupe_n_1
							list_mvfront(nouvelles_combinaisons);
							list_mvfront(groupe_n_1->tmp_combinaison.combinaisons);
							for (j=1;j<=list_size(groupe_n_1->tmp_combinaison.combinaisons);j++)
							{
								Combinaison	*combinaison2 = (Combinaison*)list_curr(groupe_n_1->tmp_combinaison.combinaisons);
								
								for (k=1;k<=list_size(transition);k++)
								{
									Combinaison	*combinaison1 = (Combinaison*)list_curr(nouvelles_combinaisons);
									
									if (_1990_combinaisons_fusion(combinaison1, combinaison2) != 0)
										BUG(-14);
									list_mvnext(nouvelles_combinaisons);
								}
								list_mvnext(groupe_n_1->tmp_combinaison.combinaisons);
							}
							list_free(transition, &(_1990_groupe_free_groupe_tmp_combinaison));
						}
					}
				}
				parcours_bits = parcours_bits >> 1;
				list_mvnext(groupe->elements);
			}
			while (parcours_bits != 0);
			
			if (_1990_combinaisons_duplique_sans_double(groupe->tmp_combinaison.combinaisons, nouvelles_combinaisons) != 0)
				BUG(-15);
			list_free(nouvelles_combinaisons, &(_1990_groupe_free_groupe_tmp_combinaison));
		}
		list_mvnext(projet->niveaux_groupes);
	}
	return 0;
}

/* _1990_combinaisons_init
 * Description : Initialise la mémoire pour les combinaisons à l'ELU et l'ELS
 * Paramètres : Projet	*projet : la variable projet
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : valeur négative en cas d'erreur
 */
int _1990_combinaisons_init(Projet *projet)
{
	if (projet == NULL)
		BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
	
	projet->combinaisons.flags = 0;
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
		BUGTEXTE(-2, gettext("Erreur d'allocation mémoire.\n"));
	else
		return 0;
}

/* _1990_combinaisons_empty
 * Description : libère l'ensemble des combinaisons à l'ELU et l'ELS sans libérer la liste
 * Paramètres : Projet	*projet : la variable projet
 * Valeur renvoyée : Aucune
 */
void _1990_combinaisons_empty(Projet *projet)
{
	Ponderation *ponderation;
	
	list_mvfront(projet->combinaisons.elu_equ);
	while (!list_empty(projet->combinaisons.elu_equ))
	{
		ponderation = list_remove_front(projet->combinaisons.elu_equ);
		list_free(ponderation->elements, LIST_DEALLOC);
		free(ponderation);
	}
	list_mvfront(projet->combinaisons.elu_str);
	while (!list_empty(projet->combinaisons.elu_str))
	{
		ponderation = list_remove_front(projet->combinaisons.elu_str);
		list_free(ponderation->elements, LIST_DEALLOC);
		free(ponderation);
	}
	list_mvfront(projet->combinaisons.elu_geo);
	while (!list_empty(projet->combinaisons.elu_geo))
	{
		ponderation = list_remove_front(projet->combinaisons.elu_geo);
		list_free(ponderation->elements, LIST_DEALLOC);
		free(ponderation);
	}
	list_mvfront(projet->combinaisons.elu_fat);
	while (!list_empty(projet->combinaisons.elu_fat))
	{
		ponderation = list_remove_front(projet->combinaisons.elu_fat);
		list_free(ponderation->elements, LIST_DEALLOC);
		free(ponderation);
	}
	list_mvfront(projet->combinaisons.elu_acc);
	while (!list_empty(projet->combinaisons.elu_acc))
	{
		ponderation = list_remove_front(projet->combinaisons.elu_acc);
		list_free(ponderation->elements, LIST_DEALLOC);
		free(ponderation);
	}
	list_mvfront(projet->combinaisons.elu_sis);
	while (!list_empty(projet->combinaisons.elu_sis))
	{
		ponderation = list_remove_front(projet->combinaisons.elu_sis);
		list_free(ponderation->elements, LIST_DEALLOC);
		free(ponderation);
	}
	list_mvfront(projet->combinaisons.els_car);
	while (!list_empty(projet->combinaisons.els_car))
	{
		ponderation = list_remove_front(projet->combinaisons.els_car);
		list_free(ponderation->elements, LIST_DEALLOC);
		free(ponderation);
	}
	list_mvfront(projet->combinaisons.els_freq);
	while (!list_empty(projet->combinaisons.els_freq))
	{
		ponderation = list_remove_front(projet->combinaisons.els_freq);
		list_free(ponderation->elements, LIST_DEALLOC);
		free(ponderation);
	}
	list_mvfront(projet->combinaisons.els_perm);
	while (!list_empty(projet->combinaisons.els_perm))
	{
		ponderation = list_remove_front(projet->combinaisons.els_perm);
		list_free(ponderation->elements, LIST_DEALLOC);
		free(ponderation);
	}
	return;
}

/* _1990_combinaisons_free
 * Description : libère la mémoire pour les combinaisons à l'ELU et l'ELS
 * Paramètres : Projet	*projet : la variable projet
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : valeur négative en cas d'erreur
 */
int _1990_combinaisons_free(Projet *projet)
{
	if ((projet->combinaisons.elu_equ == NULL) || (projet->combinaisons.elu_str == NULL) || (projet->combinaisons.elu_geo == NULL) || (projet->combinaisons.elu_fat == NULL) || (projet->combinaisons.elu_acc == NULL) || (projet->combinaisons.elu_sis == NULL) || (projet->combinaisons.els_car == NULL) || (projet->combinaisons.els_freq == NULL) || (projet->combinaisons.els_perm == NULL))
		BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
	
	_1990_combinaisons_empty(projet);
	free(projet->combinaisons.elu_equ);
	projet->combinaisons.elu_equ = NULL;
	free(projet->combinaisons.elu_str);
	projet->combinaisons.elu_str = NULL;
	free(projet->combinaisons.elu_geo);
	projet->combinaisons.elu_geo = NULL;
	free(projet->combinaisons.elu_fat);
	projet->combinaisons.elu_fat = NULL;
	free(projet->combinaisons.elu_acc);
	projet->combinaisons.elu_acc = NULL;
	free(projet->combinaisons.elu_sis);
	projet->combinaisons.elu_sis = NULL;
	free(projet->combinaisons.els_car);
	projet->combinaisons.els_car = NULL;
	free(projet->combinaisons.els_freq);
	projet->combinaisons.els_freq = NULL;
	free(projet->combinaisons.els_perm);
	projet->combinaisons.els_perm = NULL;
	
	return 0;
}

/* _1990_combinaisons_genere
 * Description : Génère l'ensemble des combinaisons du projet
 * Paramètres : Projet	*projet : la variable projet
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : valeur négative en cas d'erreur
 */
int _1990_combinaisons_genere(Projet *projet)
{
	unsigned int		i;
	
	if ((projet == NULL) || (projet->niveaux_groupes == NULL) || (list_size(projet->niveaux_groupes) == 0))
		BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
	
	_1990_combinaisons_empty(projet);
	
	// Cette boucle permet de générer toutes les combinaisons en prenant en compte le principe d'action prédominante
	// Ainsi, à chaque passage de la boucle, l'action n°i est considérée comme action prédominante
	for (i=0;i<list_size(projet->actions);i++)
	{
		Action		*action;
		unsigned int	j;
		
		// On supprime les combinaisons temporaires générées lors du passage de la boucle précédente
		list_mvfront(projet->niveaux_groupes);
		do
		{
			Niveau_Groupe	*niveau = list_curr(projet->niveaux_groupes);
			
			list_mvfront(niveau->groupes);
			do
			{
				Groupe		*groupe = list_curr(niveau->groupes);
				
				if (groupe->tmp_combinaison.combinaisons != NULL)
				{
					list_mvfront(groupe->tmp_combinaison.combinaisons);
					while (!list_empty(groupe->tmp_combinaison.combinaisons))
					{
						Combinaison	*combinaison = list_front(groupe->tmp_combinaison.combinaisons);
						
						list_free(combinaison->elements, LIST_DEALLOC);
						free(list_remove_front(groupe->tmp_combinaison.combinaisons));
					}
				}
			}
			while (list_mvnext(niveau->groupes) != NULL);
		}
		while (list_mvnext(projet->niveaux_groupes) != NULL);
		
		// On attribue à l'action n°i le flags&1 = 1 afin d'indiquer qu'il s'agit d'une action prédominante
		list_mvfront(projet->actions);
		for (j=0;j<i;j++)
		{
			action = list_curr(projet->actions);
			action->flags = 0;
			list_mvnext(projet->actions);
		}
		action = list_curr(projet->actions);
		if (_1990_action_type_combinaison_bat(action->categorie, projet->pays) == 2)
			action->flags = 1;
		else
			action->flags = 0;
		list_mvnext(projet->actions);
		for (j=i+1;j<list_size(projet->actions);j++)
		{
			action = list_curr(projet->actions);
			action->flags = 0;
			list_mvnext(projet->actions);
		}
		
		// On demande à générer les combinaisons de toutes les groupes de tous les niveaux
		list_mvfront(projet->niveaux_groupes);
		do
		{
			Niveau_Groupe	*niveau = (Niveau_Groupe*)list_curr(projet->niveaux_groupes);
			
			list_mvfront(niveau->groupes);
			if (list_curr(niveau->groupes) != NULL)
			{
				do
				{
					Groupe	*groupe = (Groupe*)list_curr(niveau->groupes);
					switch (groupe->type_combinaison)
					{
						case GROUPE_COMBINAISON_OR :
						{
							if (_1990_combinaisons_genere_groupe_or(projet) != 0)
								BUG(-2);
							break;
						}
						case GROUPE_COMBINAISON_XOR :
						{
							if (_1990_combinaisons_genere_groupe_xor(projet) != 0)
								BUG(-3);
							break;
						}
						case GROUPE_COMBINAISON_AND :
						{
							if (_1990_combinaisons_genere_groupe_and(projet) != 0)
								BUG(-4);
							break;
						}
					}
				}
				while (list_mvnext(niveau->groupes));
			}
		}
		while (list_mvnext(projet->niveaux_groupes) != NULL);
		
		// Une fois que toutes les combinaisons sont générées, il faut alors générer les pondérations
		// (coefficients de sécurité)
		_1990_ponderations_genere(projet);
	}
	
	return 0;
}
