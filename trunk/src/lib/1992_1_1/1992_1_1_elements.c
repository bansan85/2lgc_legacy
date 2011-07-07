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
#include <SuiteSparseQR_C.h>
#include "common_projet.h"
#include "common_erreurs.h"
#include "1992_1_1_elements.h"
#include "EF_rigidite.h"
#include "math.h"

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
int _1992_1_1_elements_ajout(Projet *projet, Type_Element type, int section, int noeud_debut, int noeud_fin, int discretisation_element)
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


/* _1992_1_1_rigidite_ajout
 * Description : ajouter une de la rigidité à la matrice
 * Paramètres : Projet *projet : la variable projet
 *            : Beton_Element* element : pointeur vers l'élément en béton
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : valeur négative
 */
int _1992_1_1_rigidite_ajout(Projet *projet, Beton_Element *element)
{
//	Action		*action_dernier, action_nouveau;
	int		noeudD = element->noeud_debut->numero, noeudF = element->noeud_fin->numero;
//	int		trouveDD = 0, trouveDF = 0, trouveFD = 0, trouveFF = 0; // La matrice de rigidité locale est décomposée en 4 parties en fonction du noeud de Départ et du noeud de Fin.
	long		*ai, *aj; // Pointeur vers les données des triplets
	double		*ax;      // Pointeur vers les données des triplets
	double		y, cosx, sinx; // valeurs de la matrice de rotation
	double		xx, yy, zz, ll; // Dimension de la barre dans 3D
	int		i;
	cholmod_triplet	*triplet_rotation;
	
	if ((projet == NULL) || (projet->ef_donnees.rigidite == NULL) || (element == NULL))
		BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
	
	// On commence par calculer les matrices de rotations 3D.
	triplet_rotation = cholmod_l_allocate_triplet(12, 12, 32, 0, CHOLMOD_REAL, projet->ef_donnees.c);
	ai = triplet_rotation->i;
	aj = triplet_rotation->j;
	ax = triplet_rotation->x;
	xx = element->noeud_fin->position.x - element->noeud_debut->position.x;
	yy = element->noeud_fin->position.y - element->noeud_debut->position.y;
	zz = element->noeud_fin->position.z - element->noeud_debut->position.z;
	ll = sqrt(xx*xx+yy*yy+zz*zz);
	y = asin(zz/ll);
	sinx = yy / sqrt(ll*ll-zz*zz);
	cosx = copysign(1.0, xx)*sqrt((ll*ll-yy*yy-zz*zz)/(ll*ll-zz*zz));
	for (i=0;i<4;i++)
	{
		ai[i*8+0] = i*3+0; aj[i*8+0] = i*3+0; ax[i*8+0] = cosx*cos(y);
		ai[i*8+1] = i*3+0; aj[i*8+1] = i*3+1; ax[i*8+1] = -sinx;
		ai[i*8+2] = i*3+0; aj[i*8+2] = i*3+2; ax[i*8+2] = -cosx*sin(y);
		ai[i*8+3] = i*3+1; aj[i*8+3] = i*3+0; ax[i*8+3] = sinx*cos(y);
		ai[i*8+4] = i*3+1; aj[i*8+4] = i*3+1; ax[i*8+4] = cosx;
		ai[i*8+5] = i*3+1; aj[i*8+5] = i*3+2; ax[i*8+5] = -sinx*sin(y);
		ai[i*8+6] = i*3+2; aj[i*8+6] = i*3+0; ax[i*8+6] = sin(y);
		ai[i*8+7] = i*3+2; aj[i*8+7] = i*3+2; ax[i*8+7] = cos(y);
	}
	element->matrice_rotation = cholmod_l_triplet_to_sparse(triplet_rotation, 0, projet->ef_donnees.c);
	cholmod_l_free_triplet(&triplet_rotation, projet->ef_donnees.c);
	
	// On calcule la matrice de rigidité.
	triplet_rotation = cholmod_l_allocate_triplet(12, 12, 40, 0, CHOLMOD_REAL, projet->ef_donnees.c);
	ai = triplet_rotation->i;
	aj = triplet_rotation->j;
	ax = triplet_rotation->x;
	ai[0] = 0;	aj[0] = 0;	ax[0] = 1; //TODO
	ai[1] = 0;	aj[1] = 6;	ax[1] = 1;
	ai[2] = 1;	aj[2] = 1;	ax[2] = 1;
	ai[3] = 1;	aj[3] = 5;	ax[3] = 1;
	ai[4] = 1;	aj[4] = 7;	ax[4] = 1;
	ai[5] = 1;	aj[5] = 11;	ax[5] = 1;
	ai[6] = 2;	aj[6] = 2;	ax[6] = 1;
	ai[7] = 2;	aj[7] = 4;	ax[7] = 1;
	ai[8] = 2;	aj[8] = 8;	ax[8] = 1;
	ai[9] = 2;	aj[9] = 10;	ax[9] = 1;
	ai[10] = 3;	aj[10] = 3;	ax[10] = 1;
	ai[11] = 3;	aj[11] = 9;	ax[11] = 1;
	ai[12] = 4;	aj[12] = 2;	ax[12] = 1;
	ai[13] = 4;	aj[13] = 4;	ax[13] = 1;
	ai[14] = 4;	aj[14] = 8;	ax[14] = 1;
	ai[15] = 4;	aj[15] = 10;	ax[15] = 1;
	ai[16] = 5;	aj[16] = 1;	ax[16] = 1;
	ai[17] = 5;	aj[17] = 5;	ax[17] = 1;
	ai[18] = 5;	aj[18] = 7;	ax[18] = 1;
	ai[19] = 5;	aj[19] = 11;	ax[19] = 1;
	ai[20] = 6;	aj[20] = 0;	ax[20] = 1;
	ai[21] = 6;	aj[21] = 6;	ax[21] = 1;
	ai[22] = 7;	aj[22] = 1;	ax[22] = 1;
	ai[23] = 7;	aj[23] = 5;	ax[23] = 1;
	ai[24] = 7;	aj[24] = 7;	ax[24] = 1;
	ai[25] = 7;	aj[25] = 11;	ax[25] = 1;
	ai[26] = 8;	aj[26] = 2;	ax[26] = 1;
	ai[27] = 8;	aj[27] = 4;	ax[27] = 1;
	ai[28] = 8;	aj[28] = 8;	ax[28] = 1;
	ai[29] = 8;	aj[29] = 10;	ax[29] = 1;
	ai[30] = 9;	aj[30] = 3;	ax[30] = 1;
	ai[31] = 9;	aj[31] = 9;	ax[31] = 1;
	ai[32] = 10;	aj[32] = 2;	ax[32] = 1;
	ai[33] = 10;	aj[33] = 4;	ax[33] = 1;
	ai[34] = 10;	aj[34] = 8;	ax[34] = 1;
	ai[35] = 10;	aj[35] = 10;	ax[35] = 1;
	ai[36] = 11;	aj[36] = 1;	ax[36] = 1;
	ai[37] = 11;	aj[37] = 5;	ax[37] = 1;
	ai[38] = 11;	aj[38] = 7;	ax[38] = 1;
	ai[39] = 11;	aj[39] = 11;	ax[39] = 1;
	element->matrice_rotation = cholmod_l_triplet_to_sparse(triplet_rotation, 0, projet->ef_donnees.c);
	cholmod_l_free_triplet(&triplet_rotation, projet->ef_donnees.c);
	
	
	if (list_size(projet->ef_donnees.rigidite) != 0)
	{
		list_mvfront(projet->ef_donnees.rigidite);
		do
		{
			EF_rigidite	*rigidite_en_cours = list_curr(projet->actions);
			if ((rigidite_en_cours->noeudD == noeudD) && (rigidite_en_cours->noeudF == noeudF))
			{
				
			}
		}
		while (list_mvnext(projet->ef_donnees.rigidite) != NULL);
	}
	
	
/*	action_nouveau.nom = NULL;
	action_nouveau.description = NULL;
	action_nouveau.categorie = categorie;
	action_nouveau.flags = 0;
	
	action_dernier = (Action *)list_rear(projet->actions);
	if (action_dernier == NULL)
		action_nouveau.numero = 0;
	else
		action_nouveau.numero = action_dernier->numero+1;
	
	if (list_insert_after(projet->actions, &(action_nouveau), sizeof(action_nouveau)) == NULL)
		BUGTEXTE(-2, gettext("Erreur d'allocation mémoire.\n"));*/
	
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
