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
#include "1992_1_1_elements.h"

/* _1992_1_1_elements_init
 * Description : Initialise la liste des éléments en béton
 * Paramètres : Projet *projet : la variable projet
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : valeur négative
 */
int _1992_1_1_elements_init(Projet *projet)
{
	if (projet == NULL)
		BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
	
	projet->beton.elements = list_init();
	if (projet->beton.elements == NULL)
		BUGTEXTE(-2, gettext("Erreur d'allocation mémoire.\n"));
	else
		return 0;
}

/* _1992_1_1_elements_ajout
 * Description : Ajoute un élément à la liste des éléments en béton
 * Paramètres : Projet *projet : la variable projet
 *            : Type_Beton_Element type : type de l'élément en béton
 *            : int section : numéro de la section correspondant à l'élément
 *            : int noeud_debut : numéro de départ de l'élément
 *            : int noeud_fin : numéro de fin de l'élément
 *            : int discretisation_element : nombre d'élément une fois discrétisé
 * Valeur renvoyée :
 *   Succès : 0 même si aucune section n'est existante
 *   Échec : valeur négative si la liste des éléments n'est pas initialisée ou a déjà été libérée
 */
int _1992_1_1_elements_ajout(Projet *projet, Type_Beton_Element type, int section, int noeud_debut, int noeud_fin, int discretisation_element)
{
	Beton_Element	*element_en_cours, element_nouveau;
	int		i;
	double		dx, dy, dz;
	
	if ((projet == NULL) || (projet->beton.elements == NULL))
		BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
	
	list_mvrear(projet->beton.elements);
	element_nouveau.element = type;
	
	if (_1992_1_1_sections_cherche_numero(projet, section) != 0)
		return -2;
	element_nouveau.section = list_curr(projet->beton.sections);
	
	if (EF_noeuds_cherche_numero(projet, noeud_debut) != 0)
		return -3;
	element_nouveau.noeud_debut = list_curr(projet->ef_donnees.noeuds);
	
	if (EF_noeuds_cherche_numero(projet, noeud_fin) != 0)
		return -4;
	element_nouveau.noeud_fin = list_curr(projet->ef_donnees.noeuds);
	
	element_nouveau.discretisation_element = discretisation_element;
	if (discretisation_element != 0)
	{
		element_nouveau.noeuds_intermediaires = malloc(sizeof(EF_noeud)*(discretisation_element-1));
		if (element_nouveau.noeuds_intermediaires == NULL)
			BUGTEXTE(-5, gettext("Erreur d'allocation mémoire.\n"));
		for (i=0;i<=discretisation_element-2;i++)
		{
			dx = (element_nouveau.noeud_fin->position.x-element_nouveau.noeud_debut->position.x)/discretisation_element;
			dy = (element_nouveau.noeud_fin->position.y-element_nouveau.noeud_debut->position.y)/discretisation_element;
			dz = (element_nouveau.noeud_fin->position.z-element_nouveau.noeud_debut->position.z)/discretisation_element;
			if (EF_noeuds_ajout(projet, element_nouveau.noeud_debut->position.x+dx*(i+1), element_nouveau.noeud_debut->position.y+dy*(i+1), element_nouveau.noeud_debut->position.z+dz*(i+1), -1) != 0)
				return -6;
			element_nouveau.noeuds_intermediaires[i] = list_rear(projet->ef_donnees.noeuds);
		}
	}
	else
		element_nouveau.noeuds_intermediaires = NULL;
	
	element_en_cours = (Beton_Element *)list_rear(projet->beton.elements);
	if (element_en_cours == NULL)
		element_nouveau.numero = 0;
	else
		element_nouveau.numero = element_en_cours->numero+1;
	
	if (list_insert_after(projet->beton.elements, &(element_nouveau), sizeof(element_nouveau)) == NULL)
		BUGTEXTE(-6, gettext("Erreur d'allocation mémoire.\n"));
	
	return 0;
}


/* _1992_1_1_elements_free
 * Description : Libère l'ensemble des éléments  en béton
 * Paramètres : Projet *projet : la variable projet
 * Valeur renvoyée :
 *   Succès : 0 même si aucune section n'est existante
 *   Échec : valeur négative si la liste des éléments n'est pas initialisée ou a déjà été libérée
 */
int _1992_1_1_elements_free(Projet *projet)
{
	if ((projet == NULL) || (projet->beton.elements == NULL))
		BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
	
	while (!list_empty(projet->beton.elements))
	{
		Beton_Element *element = list_remove_front(projet->beton.elements);
		
		free(element->noeuds_intermediaires);
		
		free(element);
	}
	
	free(projet->beton.elements);
	projet->beton.elements = NULL;
	
	return 0;
}
