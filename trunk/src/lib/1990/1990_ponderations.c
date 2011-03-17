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


// Renvoie 0 si ponderation_a_verifier n'est pas dans ponderations
int _1990_ponderations_verifie_double(LIST *ponderations, Ponderation* ponderation_a_verifier)
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

int _1990_ponderations_duplique_sans_double(LIST *destination, LIST *source)
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
		if (_1990_ponderations_verifie_double(destination, ponderation_source) == 0)
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

int _1990_ponderations_genere_un(Projet *projet, LIST* combinaisons_destination, double* coef_min, double* coef_max, int dim_coef, int psi_dominante, int psi_accompagnement)
{
	int			nbboucle=1, j, suivant, categorie, tmp, variable, variable_dominante;
	Ponderation		ponderation;
	Ponderation_Element	ponderation_element;
	Combinaison		*combinaison;
	Combinaison_Element	*combinaison_element;
	Groupe			*groupe;
	Niveau_Groupe		*niveau;
	
	niveau = list_rear(projet->niveaux_groupes);
	if (niveau == NULL)
		BUG(-1);
	if (list_front(niveau->groupes) != list_rear(niveau->groupes))
		BUG(-2);
	if (list_size(niveau->groupes) == 0)
		return 0;
	groupe = list_front(niveau->groupes);
	
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
			if ((suivant == 0) && (list_size(ponderation.elements) != 0) && (_1990_ponderations_verifie_double(combinaisons_destination, &ponderation) == 0))
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

int _1990_ponderations_genere_eu(Projet *projet)
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
		_1990_ponderations_genere_un(projet, projet->combinaisons.elu_equ, coef_min, coef_max, 5, -1, 0);
	}
	// Equilibre + Résistance structurelle
	else
	{
		coef_min[0] = 1.15; coef_max[0] = 1.35; // poids propre
		coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
		coef_min[2] = 0.0; coef_max[2] = 1.5; // variable
		coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
		coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
		_1990_ponderations_genere_un(projet, projet->combinaisons.elu_equ, coef_min, coef_max, 5, -1, 0);
		coef_min[0] = 1.0; coef_max[0] = 1.0; // poids propre
		coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
		coef_min[2] = 0.0; coef_max[2] = 1.5; // variable
		coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
		coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
		_1990_ponderations_genere_un(projet, projet->combinaisons.elu_equ, coef_min, coef_max, 5, -1, 0);
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
				_1990_ponderations_genere_un(projet, projet->combinaisons.elu_str, coef_min, coef_max, 5, 0, 0);
				coef_min[0] = 1.0; coef_max[0] = 1.15; // poids propre
				coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
				coef_min[2] = 0.0; coef_max[2] = 1.5; // variable
				coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
				coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
				_1990_ponderations_genere_un(projet, projet->combinaisons.elu_str, coef_min, coef_max, 5, -1, 0);
				coef_min[0] = 1.0; coef_max[0] = 1.0; // poids propre
				coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
				coef_min[2] = 0.0; coef_max[2] = 1.3; // variable
				coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
				coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
				_1990_ponderations_genere_un(projet, projet->combinaisons.elu_str, coef_min, coef_max, 5, -1, 0);
				_1990_ponderations_duplique_sans_double(projet->combinaisons.elu_geo, projet->combinaisons.elu_str);
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
				_1990_ponderations_genere_un(projet, projet->combinaisons.elu_str, coef_min, coef_max, 5, 0, 0);
				coef_min[0] = 1.0; coef_max[0] = 1.15; // poids propre
				coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
				coef_min[2] = 0.0; coef_max[2] = 1.5; // variable
				coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
				coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
				_1990_ponderations_genere_un(projet, projet->combinaisons.elu_str, coef_min, coef_max, 5, -1, 0);
				_1990_ponderations_duplique_sans_double(projet->combinaisons.elu_geo, projet->combinaisons.elu_str);
				break;
			}
			case 4:
			{
				coef_min[0] = 1.0; coef_max[0] = 1.35; // poids propre
				coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
				coef_min[2] = 0.0; coef_max[2] = 1.5; // variable
				coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
				coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
				_1990_ponderations_genere_un(projet, projet->combinaisons.elu_str, coef_min, coef_max, 5, 0, 0);
				coef_min[0] = 1.0; coef_max[0] = 1.15; // poids propre
				coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
				coef_min[2] = 0.0; coef_max[2] = 1.5; // variable
				coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
				coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
				_1990_ponderations_genere_un(projet, projet->combinaisons.elu_str, coef_min, coef_max, 5, -1, 0);
				coef_min[0] = 1.0; coef_max[0] = 1.0; // poids propre
				coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
				coef_min[2] = 0.0; coef_max[2] = 1.3; // variable
				coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
				coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
				_1990_ponderations_genere_un(projet, projet->combinaisons.elu_geo, coef_min, coef_max, 5, -1, 0);
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
				_1990_ponderations_genere_un(projet, projet->combinaisons.elu_str, coef_min, coef_max, 5, -1, 0);
				coef_min[0] = 1.0; coef_max[0] = 1.0; // poids propre
				coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
				coef_min[2] = 0.0; coef_max[2] = 1.3; // variable
				coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
				coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
				_1990_ponderations_genere_un(projet, projet->combinaisons.elu_str, coef_min, coef_max, 5, -1, 0);
				_1990_ponderations_duplique_sans_double(projet->combinaisons.elu_geo, projet->combinaisons.elu_str);
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
				_1990_ponderations_genere_un(projet, projet->combinaisons.elu_str, coef_min, coef_max, 5, -1, 0);
				_1990_ponderations_duplique_sans_double(projet->combinaisons.elu_geo, projet->combinaisons.elu_str);
				break;
			}
			case 4:
			{
				coef_min[0] = 1.0; coef_max[0] = 1.35; // poids propre
				coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
				coef_min[2] = 0.0; coef_max[2] = 1.5; // variable
				coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
				coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
				_1990_ponderations_genere_un(projet, projet->combinaisons.elu_str, coef_min, coef_max, 5, -1, 0);
				coef_min[0] = 1.0; coef_max[0] = 1.0; // poids propre
				coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
				coef_min[2] = 0.0; coef_max[2] = 1.3; // variable
				coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
				coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
				_1990_ponderations_genere_un(projet, projet->combinaisons.elu_geo, coef_min, coef_max, 5, -1, 0);
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
		_1990_ponderations_genere_un(projet, projet->combinaisons.elu_acc, coef_min, coef_max, 5, 1, 2);
	}
	else
	{
		coef_min[0] = 1.0; coef_max[0] = 1.0; // poids propre
		coef_min[1] = 1.0; coef_max[1] = 1.0; // précontrainte (1992-1)
		coef_min[2] = 0.0; coef_max[2] = 1.0; // variable
		coef_min[3] = 1.0; coef_max[3] = 1.0; // Accidentelle
		coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
		_1990_ponderations_genere_un(projet, projet->combinaisons.elu_acc, coef_min, coef_max, 5, 2, 2);
	}
	
	coef_min[0] = 1.0; coef_max[0] = 1.0; // poids propre
	coef_min[1] = 1.0; coef_max[1] = 1.0; // précontrainte (1992-1)
	coef_min[2] = 0.0; coef_max[2] = 1.0; // variable
	coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
	coef_min[4] = 1.0; coef_max[4] = 1.0; // Sismique
	_1990_ponderations_genere_un(projet, projet->combinaisons.elu_sis, coef_min, coef_max, 5, 2, 2);
	
	coef_min[0] = 1.0; coef_max[0] = 1.0; // poids propre
	coef_min[1] = 1.0; coef_max[1] = 1.0; // précontrainte (1992-1)
	coef_min[2] = 0.0; coef_max[2] = 1.0; // variable
	coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
	coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
	_1990_ponderations_genere_un(projet, projet->combinaisons.els_car, coef_min, coef_max, 5, -1, 0);

	coef_min[0] = 1.0; coef_max[0] = 1.0; // poids propre
	coef_min[1] = 1.0; coef_max[1] = 1.0; // précontrainte (1992-1)
	coef_min[2] = 0.0; coef_max[2] = 1.0; // variable
	coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
	coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
	_1990_ponderations_genere_un(projet, projet->combinaisons.els_freq, coef_min, coef_max, 5, 1, 2);

	coef_min[0] = 1.0; coef_max[0] = 1.0; // poids propre
	coef_min[1] = 1.0; coef_max[1] = 1.0; // précontrainte (1992-1)
	coef_min[2] = 0.0; coef_max[2] = 1.0; // variable
	coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
	coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
	_1990_ponderations_genere_un(projet, projet->combinaisons.els_perm, coef_min, coef_max, 5, 2, 2);

	free(coef_min);
	free(coef_max);
	
	return 0;
}

int _1990_ponderations_genere_fr(Projet *projet)
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
		_1990_ponderations_genere_un(projet, projet->combinaisons.elu_equ, coef_min, coef_max, 6, -1, 0);
	}
	else
	{
		coef_min[0] = 1.15; coef_max[0] = 1.35; // poids propre
		coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
		coef_min[2] = 0.0; coef_max[2] = 1.5; // variable
		coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
		coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
		coef_min[5] = 0.0; coef_max[5] = 1.2; // Eaux souterraines
		_1990_ponderations_genere_un(projet, projet->combinaisons.elu_equ, coef_min, coef_max, 6, -1, 0);
		coef_min[0] = 1.0; coef_max[0] = 1.0; // poids propre
		coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
		coef_min[2] = 0.0; coef_max[2] = 1.5; // variable
		coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
		coef_min[4] = 0.0; coef_max[4] = 1.0; // Sismique
		coef_min[5] = 0.0; coef_max[5] = 1.2; // Eaux souterraines
		_1990_ponderations_genere_un(projet, projet->combinaisons.elu_equ, coef_min, coef_max, 6, -1, 0);
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
				_1990_ponderations_genere_un(projet, projet->combinaisons.elu_str, coef_min, coef_max, 6, 0, 0);
				coef_min[0] = 1.0; coef_max[0] = 1.15; // poids propre
				coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
				coef_min[2] = 0.0; coef_max[2] = 1.5; // variable
				coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
				coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
				coef_min[5] = 0.0; coef_max[5] = 1.2; // Eaux souterraines
				_1990_ponderations_genere_un(projet, projet->combinaisons.elu_str, coef_min, coef_max, 6, -1, 0);
				coef_min[0] = 1.0; coef_max[0] = 1.0; // poids propre
				coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
				coef_min[2] = 0.0; coef_max[2] = 1.3; // variable
				coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
				coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
				coef_min[5] = 0.0; coef_max[5] = 1.2; // Eaux souterraines
				_1990_ponderations_genere_un(projet, projet->combinaisons.elu_str, coef_min, coef_max, 6, -1, 0);
				_1990_ponderations_duplique_sans_double(projet->combinaisons.elu_geo, projet->combinaisons.elu_str);
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
				_1990_ponderations_genere_un(projet, projet->combinaisons.elu_str, coef_min, coef_max, 6, 0, 0);
				coef_min[0] = 1.0; coef_max[0] = 1.15; // poids propre
				coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
				coef_min[2] = 0.0; coef_max[2] = 1.5; // variable
				coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
				coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
				coef_min[5] = 0.0; coef_max[5] = 1.2; // Eaux souterraines
				_1990_ponderations_genere_un(projet, projet->combinaisons.elu_str, coef_min, coef_max, 6, -1, 0);
				_1990_ponderations_duplique_sans_double(projet->combinaisons.elu_geo, projet->combinaisons.elu_str);
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
				_1990_ponderations_genere_un(projet, projet->combinaisons.elu_str, coef_min, coef_max, 6, 0, 0);
				coef_min[0] = 1.0; coef_max[0] = 1.15; // poids propre
				coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
				coef_min[2] = 0.0; coef_max[2] = 1.5; // variable
				coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
				coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
				coef_min[5] = 0.0; coef_max[5] = 1.2; // Eaux souterraines
				_1990_ponderations_genere_un(projet, projet->combinaisons.elu_str, coef_min, coef_max, 6, -1, 0);
				coef_min[0] = 1.0; coef_max[0] = 1.0; // poids propre
				coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
				coef_min[2] = 0.0; coef_max[2] = 1.3; // variable
				coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
				coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
				coef_min[5] = 0.0; coef_max[5] = 1.2; // Eaux souterraines
				_1990_ponderations_genere_un(projet, projet->combinaisons.elu_geo, coef_min, coef_max, 6, -1, 0);
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
				_1990_ponderations_genere_un(projet, projet->combinaisons.elu_str, coef_min, coef_max, 6, -1, 0);
				coef_min[0] = 1.0; coef_max[0] = 1.0; // poids propre
				coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
				coef_min[2] = 0.0; coef_max[2] = 1.3; // variable
				coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
				coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
				coef_min[5] = 0.0; coef_max[5] = 1.2; // Eaux souterraines
				_1990_ponderations_genere_un(projet, projet->combinaisons.elu_str, coef_min, coef_max, 6, -1, 0);
				_1990_ponderations_duplique_sans_double(projet->combinaisons.elu_geo, projet->combinaisons.elu_str);
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
				_1990_ponderations_genere_un(projet, projet->combinaisons.elu_str, coef_min, coef_max, 6, -1, 0);
				_1990_ponderations_duplique_sans_double(projet->combinaisons.elu_geo, projet->combinaisons.elu_str);
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
				_1990_ponderations_genere_un(projet, projet->combinaisons.elu_str, coef_min, coef_max, 6, -1, 0);
				coef_min[0] = 1.0; coef_max[0] = 1.0; // poids propre
				coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
				coef_min[2] = 0.0; coef_max[2] = 1.3; // variable
				coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
				coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
				coef_min[5] = 0.0; coef_max[5] = 1.2; // Eaux souterraines
				_1990_ponderations_genere_un(projet, projet->combinaisons.elu_geo, coef_min, coef_max, 6, -1, 0);
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
		_1990_ponderations_genere_un(projet, projet->combinaisons.elu_acc, coef_min, coef_max, 6, 1, 2);
	}
	else
	{
		coef_min[0] = 1.0; coef_max[0] = 1.0; // poids propre
		coef_min[1] = 1.0; coef_max[1] = 1.0; // précontrainte (1992-1)
		coef_min[2] = 0.0; coef_max[2] = 1.0; // variable
		coef_min[3] = 1.0; coef_max[3] = 1.0; // Accidentelle
		coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
		coef_min[5] = 0.0; coef_max[5] = 1.0; // Eaux souterraines
		_1990_ponderations_genere_un(projet, projet->combinaisons.elu_acc, coef_min, coef_max, 6, 2, 2);
	}
	
	coef_min[0] = 1.0; coef_max[0] = 1.0; // poids propre
	coef_min[1] = 1.0; coef_max[1] = 1.0; // précontrainte (1992-1)
	coef_min[2] = 0.0; coef_max[2] = 1.0; // variable
	coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
	coef_min[4] = 1.0; coef_max[4] = 1.0; // Sismique
	coef_min[5] = 0.0; coef_max[5] = 1.0; // Eaux souterraines
	_1990_ponderations_genere_un(projet, projet->combinaisons.elu_acc, coef_min, coef_max, 6, 2, 2);
	
	coef_min[0] = 1.0; coef_max[0] = 1.0; // poids propre
	coef_min[1] = 1.0; coef_max[1] = 1.0; // précontrainte (1992-1)
	coef_min[2] = 0.0; coef_max[2] = 1.0; // variable
	coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
	coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
	coef_min[5] = 0.0; coef_max[5] = 1.2; // Eaux souterraines
	_1990_ponderations_genere_un(projet, projet->combinaisons.els_car, coef_min, coef_max, 6, -1, 0);

	coef_min[0] = 1.0; coef_max[0] = 1.0; // poids propre
	coef_min[1] = 1.0; coef_max[1] = 1.0; // précontrainte (1992-1)
	coef_min[2] = 0.0; coef_max[2] = 1.0; // variable
	coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
	coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
	coef_min[5] = 0.0; coef_max[5] = 1.0; // Eaux souterraines
	_1990_ponderations_genere_un(projet, projet->combinaisons.els_freq, coef_min, coef_max, 6, 1, 2);

	coef_min[0] = 1.0; coef_max[0] = 1.0; // poids propre
	coef_min[1] = 1.0; coef_max[1] = 1.0; // précontrainte (1992-1)
	coef_min[2] = 0.0; coef_max[2] = 1.0; // variable
	coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
	coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
	coef_min[5] = 0.0; coef_max[5] = 1.0; // Eaux souterraines
	_1990_ponderations_genere_un(projet, projet->combinaisons.els_perm, coef_min, coef_max, 6, 2, 2);

	free(coef_min);
	free(coef_max);
	
	return 0;
}

int _1990_ponderations_genere(Projet *projet)
{
	switch (projet->pays)
	{
		case PAYS_EU : { return _1990_ponderations_genere_eu(projet); break; }
		case PAYS_FR : { return _1990_ponderations_genere_fr(projet); break; }
		default : { BUG(-1.); break; }
	}
}

void _1990_ponderations_affiche(LIST *ponderations)
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

void _1990_ponderations_affiche_tout(Projet *projet)
{
	printf("elu_equ\n");
	_1990_ponderations_affiche(projet->combinaisons.elu_equ);
	printf("elu_str\n");
	_1990_ponderations_affiche(projet->combinaisons.elu_str);
	printf("elu_geo\n");
	_1990_ponderations_affiche(projet->combinaisons.elu_geo);
	printf("elu_fat\n");
	_1990_ponderations_affiche(projet->combinaisons.elu_fat);
	printf("elu_acc\n");
	_1990_ponderations_affiche(projet->combinaisons.elu_acc);
	printf("elu_sis\n");
	_1990_ponderations_affiche(projet->combinaisons.elu_sis);
	printf("els_car\n");
	_1990_ponderations_affiche(projet->combinaisons.els_car);
	printf("els_freq\n");
	_1990_ponderations_affiche(projet->combinaisons.els_freq);
	printf("els_perm\n");
	_1990_ponderations_affiche(projet->combinaisons.els_perm);
	return;
}

