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
#include <string.h>
#include <SuiteSparseQR_C.h>
#include "common_projet.h"
#include "common_erreurs.h"
#include "common_maths.h"
#include "EF_rigidite.h"
#include "EF_noeud.h"
#include "1990_actions.h"
#include "1992_1_1_elements.h"

/* EF_calculs_initialise
 * Description : Initialise les divers variables nécessaire à l'ajout des rigidités
 * Paramètres : Projet *projet : la variable projet
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : valeur négative
 */
int EF_calculs_initialise(Projet *projet)
{
	unsigned int	i, nnz_max;
	
	// Détermine pour chaque noeud si la ligne / colonne de la matrice de rigidité globale
	// doit être pris en compte dans la résolution du système.
	// le nombre [0] indique la colonne/ligne correspondant au déplacement selon l'axe x
	// le nombre [5] indique la colonne/ligne correspondant à la rotation autour de l'axe z
	// le nombre vaut -1 si la colonne n'est pas inséré dans la matrice de rigidité globale
	projet->ef_donnees.noeuds_flags_partielle = malloc(sizeof(int*)*list_size(projet->ef_donnees.noeuds));
	if (projet->ef_donnees.noeuds_flags_partielle == NULL)
		BUGTEXTE(-1, gettext("Erreur d'allocation mémoire.\n"));
	for (i=0;i<list_size(projet->ef_donnees.noeuds);i++)
	{
		projet->ef_donnees.noeuds_flags_partielle[i] = malloc(6*sizeof(int));
		if (projet->ef_donnees.noeuds_flags_partielle[i] == NULL)
			BUGTEXTE(-2, gettext("Erreur d'allocation mémoire.\n"));
	}
	i = 0; // Nombre de colonnes/lignes de la matrice
	list_mvfront(projet->ef_donnees.noeuds);
	do
	{
		EF_Noeud	*noeud = list_curr(projet->ef_donnees.noeuds);
		
		if (noeud->appui == NULL)
		{
			projet->ef_donnees.noeuds_flags_partielle[noeud->numero][0] = i; i++; // x
			projet->ef_donnees.noeuds_flags_partielle[noeud->numero][1] = i; i++; // y
			projet->ef_donnees.noeuds_flags_partielle[noeud->numero][2] = i; i++; // z
			projet->ef_donnees.noeuds_flags_partielle[noeud->numero][3] = i; i++; // rx
			projet->ef_donnees.noeuds_flags_partielle[noeud->numero][4] = i; i++; // ry
			projet->ef_donnees.noeuds_flags_partielle[noeud->numero][5] = i; i++; // rz
		}
		else
		{
			EF_Appui	*appui = noeud->appui;
			if (appui->x == EF_APPUI_LIBRE)
				{ projet->ef_donnees.noeuds_flags_partielle[noeud->numero][0] = i; i++; }
			else projet->ef_donnees.noeuds_flags_partielle[noeud->numero][0] = -1;
			if (appui->y == EF_APPUI_LIBRE)
				{ projet->ef_donnees.noeuds_flags_partielle[noeud->numero][1] = i; i++; }
			else projet->ef_donnees.noeuds_flags_partielle[noeud->numero][1] = -1;
			if (appui->z == EF_APPUI_LIBRE)
				{ projet->ef_donnees.noeuds_flags_partielle[noeud->numero][2] = i; i++; }
			else projet->ef_donnees.noeuds_flags_partielle[noeud->numero][2] = -1;
			if (appui->rx == EF_APPUI_LIBRE)
				{ projet->ef_donnees.noeuds_flags_partielle[noeud->numero][3] = i; i++; }
			else projet->ef_donnees.noeuds_flags_partielle[noeud->numero][3] = -1;
			if (appui->ry == EF_APPUI_LIBRE)
				{ projet->ef_donnees.noeuds_flags_partielle[noeud->numero][4] = i; i++; }
			else projet->ef_donnees.noeuds_flags_partielle[noeud->numero][4] = -1;
			if (appui->rz == EF_APPUI_LIBRE)
				{ projet->ef_donnees.noeuds_flags_partielle[noeud->numero][5] = i; i++; }
			else projet->ef_donnees.noeuds_flags_partielle[noeud->numero][5] = -1;
		}
	}
	while (list_mvnext(projet->ef_donnees.noeuds) != NULL);
	projet->ef_donnees.nb_colonne_matrice = i;
	
	// On détermine au maximum le nombre de triplet il sera nécessaire pour obtenir la matrice de rigidité globale, soit 12*12*nombre_d'éléments (y compris discrétisation)
	nnz_max = 0;
	projet->ef_donnees.rigidite_triplet_en_cours = 0;
	list_mvfront(projet->beton.elements);
	do
	{
		Beton_Element	*element = list_curr(projet->beton.elements);
		
		nnz_max += 12*12*(element->discretisation_element+1);
	}
	while (list_mvnext(projet->beton.elements) != NULL);
	projet->ef_donnees.rigidite_triplet = cholmod_l_allocate_triplet(projet->ef_donnees.nb_colonne_matrice, projet->ef_donnees.nb_colonne_matrice, nnz_max, 0, CHOLMOD_REAL, projet->ef_donnees.c);
	
	return 0;
}

/* EF_calculs_genere_sparse
 * Description : Converti la matrice de rigidité complète de la structure sous forme de liste en une matrice de rigidité partielle (les lignes (et colonnes) dont on connait les déplacements ne sont pas insérées) sous forme d'une matrice sparse
 * Paramètres : Projet *projet : la variable projet
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : valeur négative si la liste de rigidité n'est pas initialisée ou a déjà été libérée
 */
int EF_calculs_genere_sparse(Projet *projet)
{
	cholmod_triplet		*triplet_rigidite;
	double			max_rig = 0.;
	int			nnz_max = 0;
//	int			k;
	unsigned int		i, j;
/*	long			*ai2, *aj2;	// Pointeur vers les données des triplets
	double			*ax2;		// Pointeur vers les données des triplets*/
	long			*ai, *aj;	// Pointeur vers les données des triplets
	double			*ax;		// Pointeur vers les données des triplets
	
	if ((projet == NULL) || (projet->ef_donnees.rigidite_triplet == NULL))
		BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
	
	// On commence par déterminer la valeur maximale d'une case de la matrice pour 
	// ensuite supprimer les valeurs négligeables.
	ax = projet->ef_donnees.rigidite_triplet->x;
	for (j=0;j<projet->ef_donnees.rigidite_triplet->nzmax;j++)
	{
		if (ABS(ax[j]) > max_rig)
			max_rig = ABS(ax[j]);
	}
	for (j=0;j<projet->ef_donnees.rigidite_triplet->nzmax;j++)
	{
		if (ABS(ax[j]) < max_rig*ERREUR_RELATIVE_MIN)
			ax[j] = 0.;
		else
			nnz_max++;
	}
	
	// Cela signifie que tous les noeuds sont bloqués (cas d'une poutre sur deux appuis sans discrétisation par exemple)
	if (projet->ef_donnees.nb_colonne_matrice == 0)
	{
		triplet_rigidite = cholmod_l_allocate_triplet(0, 0, 0, 0, CHOLMOD_REAL, projet->ef_donnees.c);
		projet->ef_donnees.rigidite_matrice_partielle = cholmod_l_triplet_to_sparse(triplet_rigidite, 0, projet->ef_donnees.c);
		projet->ef_donnees.inv_rigidite_matrice_partielle = cholmod_l_triplet_to_sparse(triplet_rigidite, 0, projet->ef_donnees.c);
		cholmod_l_free_triplet(&triplet_rigidite, projet->ef_donnees.c);
		
		return 0;
	}
	
	// On crée le triplet correspondant à la matrice de rigidité partielle
/*	triplet_rigidite = cholmod_l_allocate_triplet(projet->ef_donnees.nb_colonne_matrice, projet->ef_donnees.nb_colonne_matrice, nnz_max, 0, CHOLMOD_REAL, projet->ef_donnees.c);
	ai = triplet_rigidite->i;
	aj = triplet_rigidite->j;
	ax = triplet_rigidite->x;
	ai2 = projet->ef_donnees.rigidite_triplet->i;
	aj2 = projet->ef_donnees.rigidite_triplet->j;
	ax2 = projet->ef_donnees.rigidite_triplet->x;
	k = 0; // Dernier triplet ajouté
	for (i=0;i<projet->ef_donnees.rigidite_triplet->nzmax;i++)
	{
		if ((projet->ef_donnees.noeuds_flags_partielle[ai2[i]/6][ai2[i]%6] != -1) && (projet->ef_donnees.noeuds_flags_partielle[aj2[i]/6][aj2[i]%6] != -1) && (ax2[i] != 0.))
		{
			ai[k] = projet->ef_donnees.noeuds_flags_partielle[ai2[i]/6][ai2[i]%6];
			aj[k] = projet->ef_donnees.noeuds_flags_partielle[aj2[i]/6][aj2[i]%6];
			ax[k] = ax[i];
			k++;
		}
	}
	triplet_rigidite->nnz = k;*/
	
	// On converti la liste des triplets en matrice sparse
	projet->ef_donnees.rigidite_matrice_partielle = cholmod_l_triplet_to_sparse(projet->ef_donnees.rigidite_triplet, 0, projet->ef_donnees.c);
	
	// Puis on inverse la matrice en cherchant la matrice x de tel sorte que Kx=[1]. Ainsi x est l'inverse
	cholmod_triplet *zero_rigidite = cholmod_l_allocate_triplet(projet->ef_donnees.rigidite_matrice_partielle->nrow, projet->ef_donnees.rigidite_matrice_partielle->ncol, projet->ef_donnees.rigidite_matrice_partielle->nrow, 0, CHOLMOD_REAL, projet->ef_donnees.c);
	ai = zero_rigidite->i;
	aj = zero_rigidite->j;
	ax = zero_rigidite->x;
	zero_rigidite->nnz=projet->ef_donnees.rigidite_matrice_partielle->nrow;
	for (i=0;i<zero_rigidite->nnz;i++)
	{
		ai[i] = i;
		aj[i] = i;
		ax[i] = 1.;
	}
	cholmod_sparse	*zeros_spars = cholmod_l_triplet_to_sparse(zero_rigidite, 0, projet->ef_donnees.c);
	cholmod_l_free_triplet(&zero_rigidite, projet->ef_donnees.c);
	projet->ef_donnees.inv_rigidite_matrice_partielle = SuiteSparseQR_C_backslash_sparse(0, ERREUR_RELATIVE_MIN, projet->ef_donnees.rigidite_matrice_partielle, zeros_spars, projet->ef_donnees.c);
	cholmod_l_free_sparse(&zeros_spars, projet->ef_donnees.c);
	
	return 0;
}


/* EF_calculs_resoud_charge
 * Description : Déterminer à partir de la matrice de rigidité globale partielle les déplacements des noeuds pour une action particulière.
 * Paramètres : Projet *projet : la variable projet
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : valeur négative si la liste de rigidité n'est pas initialisée ou a déjà été libérée
 */
int EF_calculs_resoud_charge(Projet *projet, int num_action)
{
	Action			*action_en_cours;
	cholmod_triplet		*triplet_force;
	cholmod_sparse		*sparse_force;
	long			*ai, *aj;	// Pointeur vers les données des triplets
	double			*ax;		// Pointeur vers les données des triplets
	unsigned int		i;
	
	if ((projet == NULL) || (projet->actions == NULL) || (list_size(projet->actions) == 0) || (_1990_action_cherche_numero(projet, num_action) != 0) || (projet->ef_donnees.inv_rigidite_matrice_partielle == NULL))
		BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
	
	action_en_cours = list_curr(projet->actions);
	triplet_force = cholmod_l_allocate_triplet(projet->ef_donnees.inv_rigidite_matrice_partielle->nrow, 1, projet->ef_donnees.inv_rigidite_matrice_partielle->nrow, 0, CHOLMOD_REAL, projet->ef_donnees.c);
	ai = triplet_force->i;
	aj = triplet_force->j;
	ax = triplet_force->x;
	triplet_force->nnz = projet->ef_donnees.inv_rigidite_matrice_partielle->nrow;
	for (i=0;i<triplet_force->nnz;i++)
	{
		ai[i] = i;
		aj[i] = 0;
		ax[i] = 0.;
	}
	
	if (list_size(action_en_cours->charges) != 0)
	{
		list_mvfront(action_en_cours->charges);
		do
		{
			Charge_Ponctuelle	*charge = list_curr(action_en_cours->charges);
			if (projet->ef_donnees.noeuds_flags_partielle[charge->noeud->numero][0] != -1)
				ax[projet->ef_donnees.noeuds_flags_partielle[charge->noeud->numero][0]] += charge->x;
			if (projet->ef_donnees.noeuds_flags_partielle[charge->noeud->numero][1] != -1)
				ax[projet->ef_donnees.noeuds_flags_partielle[charge->noeud->numero][1]] += charge->y;
			if (projet->ef_donnees.noeuds_flags_partielle[charge->noeud->numero][2] != -1)
				ax[projet->ef_donnees.noeuds_flags_partielle[charge->noeud->numero][2]] += charge->z;
			if (projet->ef_donnees.noeuds_flags_partielle[charge->noeud->numero][3] != -1)
				ax[projet->ef_donnees.noeuds_flags_partielle[charge->noeud->numero][3]] += charge->rx;
			if (projet->ef_donnees.noeuds_flags_partielle[charge->noeud->numero][4] != -1)
				ax[projet->ef_donnees.noeuds_flags_partielle[charge->noeud->numero][4]] += charge->ry;
			if (projet->ef_donnees.noeuds_flags_partielle[charge->noeud->numero][5] != -1)
				ax[projet->ef_donnees.noeuds_flags_partielle[charge->noeud->numero][5]] += charge->rz;
		}
		while (list_mvnext(action_en_cours->charges) != NULL);
	}
	
	sparse_force = cholmod_l_triplet_to_sparse(triplet_force, 0, projet->ef_donnees.c);
	cholmod_l_free_triplet(&triplet_force, projet->ef_donnees.c);
	
	action_en_cours->deplacement_partiel = cholmod_l_ssmult(projet->ef_donnees.inv_rigidite_matrice_partielle, sparse_force, 0, 1, 0, projet->ef_donnees.c);
	
	cholmod_l_free_sparse(&sparse_force, projet->ef_donnees.c);
	
	return 0;
}
