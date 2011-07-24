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
//#include <SuiteSparseQR_C.h>
#include <time.h>
#include <unistd.h>

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
	projet->ef_donnees.noeuds_flags_partielle = (int**)malloc(sizeof(int*)*list_size(projet->ef_donnees.noeuds));
	if (projet->ef_donnees.noeuds_flags_partielle == NULL)
		BUGTEXTE(-1, gettext("Erreur d'allocation mémoire.\n"));
	for (i=0;i<list_size(projet->ef_donnees.noeuds);i++)
	{
		projet->ef_donnees.noeuds_flags_partielle[i] = (int*)malloc(6*sizeof(int));
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
	projet->ef_donnees.rigidite_triplet->nnz = nnz_max;
	
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
	unsigned int		j;
	double			*ax;		// Pointeur vers les données des triplets
	
	if ((projet == NULL) || (projet->ef_donnees.rigidite_triplet == NULL))
		BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
	
	// On commence par déterminer la valeur maximale d'une case de la matrice pour 
	// ensuite supprimer les valeurs négligeables.
	projet->ef_donnees.max_rigidite = 0.;
	ax = projet->ef_donnees.rigidite_triplet->x;
	projet->ef_donnees.rigidite_triplet->nzmax = projet->ef_donnees.rigidite_triplet_en_cours;
	for (j=0;j<projet->ef_donnees.rigidite_triplet->nzmax;j++)
	{
		if (ABS(ax[j]) > projet->ef_donnees.max_rigidite)
			projet->ef_donnees.max_rigidite = ABS(ax[j]);
	}
	
	// Cela signifie que tous les noeuds sont bloqués (cas d'une poutre sur deux appuis sans discrétisation par exemple)
	if (projet->ef_donnees.nb_colonne_matrice == 0)
	{
		triplet_rigidite = cholmod_l_allocate_triplet(0, 0, 0, 0, CHOLMOD_REAL, projet->ef_donnees.c);
		projet->ef_donnees.rigidite_matrice_partielle = cholmod_l_triplet_to_sparse(triplet_rigidite, 0, projet->ef_donnees.c);
		projet->ef_donnees.rigidite_matrice_partielle->stype = 0;
		projet->ef_donnees.QR = SuiteSparseQR_C_factorize(0, 0., projet->ef_donnees.rigidite_matrice_partielle, projet->ef_donnees.c);
//		Pour utiliser cholmod dans les calculs de matrices.
//		projet->ef_donnees.factor_rigidite_matrice_partielle = cholmod_l_analyze (projet->ef_donnees.rigidite_matrice_partielle, projet->ef_donnees.c) ;
//		cholmod_l_factorize(projet->ef_donnees.rigidite_matrice_partielle, projet->ef_donnees.factor_rigidite_matrice_partielle, projet->ef_donnees.c);
		cholmod_l_free_triplet(&triplet_rigidite, projet->ef_donnees.c);
		
		return 0;
	}
	
	// On converti la liste des triplets en matrice sparse
	projet->ef_donnees.rigidite_matrice_partielle = cholmod_l_triplet_to_sparse(projet->ef_donnees.rigidite_triplet, 0, projet->ef_donnees.c);
	// On enlève les valeurs parasites
	cholmod_l_drop(projet->ef_donnees.max_rigidite*ERREUR_RELATIVE_MIN, projet->ef_donnees.rigidite_matrice_partielle, projet->ef_donnees.c);
//	cholmod_l_sort(projet->ef_donnees.rigidite_matrice_partielle, projet->ef_donnees.c);
	// On force la matrice à ne pas être symétrique.
	if (projet->ef_donnees.rigidite_matrice_partielle->stype != 0)
	{
		cholmod_sparse *A = cholmod_l_copy(projet->ef_donnees.rigidite_matrice_partielle, 0, 1, projet->ef_donnees.c);
		cholmod_l_free_sparse(&projet->ef_donnees.rigidite_matrice_partielle, projet->ef_donnees.c);
		projet->ef_donnees.rigidite_matrice_partielle = A;
	}
	
/*	Pour utiliser cholmod dans les calculs de matrices.
	 Et on factorise la matrice
	projet->ef_donnees.factor_rigidite_matrice_partielle = cholmod_l_analyze (projet->ef_donnees.rigidite_matrice_partielle, projet->ef_donnees.c) ;
	// Normalement, c'est par cette méthode qu'on résoud une matrice non symétrique. Mais en pratique, ça ne marche pas. Pourquoi ?!?
	double beta[2] = {1.e-6, 0.};
	if (cholmod_l_factorize_p(projet->ef_donnees.rigidite_matrice_partielle, beta, NULL, 0, projet->ef_donnees.factor_rigidite_matrice_partielle, projet->ef_donnees.c) == TRUE)
	{
		if (projet->ef_donnees.c->status == CHOLMOD_NOT_POSDEF)
			BUGTEXTE(-1, "Matrice non définie positive.\n");
	}*/
	
	projet->ef_donnees.QR = SuiteSparseQR_C_factorize(0, 0., projet->ef_donnees.rigidite_matrice_partielle, projet->ef_donnees.c);
	
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
	cholmod_dense		*dense_force;
	long			*ai, *aj;	// Pointeur vers les données des triplets
	double			*ax;		// Pointeur vers les données des triplets
	unsigned int		i;
	
	if ((projet == NULL) || (projet->actions == NULL) || (list_size(projet->actions) == 0) || (_1990_action_cherche_numero(projet, num_action) != 0) || (projet->ef_donnees.QR == NULL))
		BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
	
	// On crée la vecteur sparse contenant les actions extérieures
	action_en_cours = list_curr(projet->actions);
	triplet_force = cholmod_l_allocate_triplet(projet->ef_donnees.rigidite_matrice_partielle->nrow, 1, projet->ef_donnees.rigidite_matrice_partielle->nrow, 0, CHOLMOD_REAL, projet->ef_donnees.c);
	ai = triplet_force->i;
	aj = triplet_force->j;
	ax = triplet_force->x;
	triplet_force->nnz = projet->ef_donnees.rigidite_matrice_partielle->nrow;
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
			Charge_Ponctuelle_Noeud	*charge = list_curr(action_en_cours->charges);
			if (charge->type == CHARGE_PONCTUELLE_NOEUD)
			{
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
		}
		while (list_mvnext(action_en_cours->charges) != NULL);
	}
	sparse_force = cholmod_l_triplet_to_sparse(triplet_force, 0, projet->ef_donnees.c);
	cholmod_l_free_triplet(&triplet_force, projet->ef_donnees.c);
	dense_force = cholmod_l_sparse_to_dense(sparse_force, projet->ef_donnees.c);
	cholmod_l_free_sparse(&sparse_force, projet->ef_donnees.c);
	
//	Pour utiliser cholmod dans les calculs de matrices.
/*	action_en_cours->deplacement_partiel = cholmod_l_spsolve (CHOLMOD_A, projet->ef_donnees.factor_rigidite_matrice_partielle, sparse_force, projet->ef_donnees.c);
	cholmod_sparse *r = cholmod_l_copy_sparse(sparse_force, projet->ef_donnees.c);
	cholmod_l_ssmult(projet->ef_donnees.rigidite_matrice_partielle, action_en_cours->deplacement_partiel, 0, TRUE, TRUE, projet->ef_donnees.c);
	action_en_cours->norm = cholmod_l_norm_sparse(r, 0, projet->ef_donnees.c);
	printf("résidu : %f\n", action_en_cours->norm);
	cholmod_l_free_sparse(&r, projet->ef_donnees.c);
	cholmod_l_write_sparse(stdout, action_en_cours->deplacement_partiel, NULL, NULL, projet->ef_donnees.c);*/
	
	// On résoud le système
	cholmod_dense *Y = SuiteSparseQR_C_qmult (SPQR_QTX, projet->ef_donnees.QR, dense_force, projet->ef_donnees.c);
	cholmod_dense *X = SuiteSparseQR_C_solve(SPQR_RX_EQUALS_B, projet->ef_donnees.QR, Y, projet->ef_donnees.c);
	action_en_cours->deplacement_partiel = cholmod_l_dense_to_sparse(X, TRUE, projet->ef_donnees.c);
	cholmod_l_write_sparse(stdout, action_en_cours->deplacement_partiel, NULL, NULL, projet->ef_donnees.c);
	
	// On calcul le résidu. Méthode trouvée dans le fichier cholmod_demo.c de la source de la librairie cholmod.
	cholmod_dense *r = cholmod_l_copy_dense(dense_force, projet->ef_donnees.c);
	double minusone[2] = {-1., 0.}, one[2] = {1., 0.};
	cholmod_l_sdmult(projet->ef_donnees.rigidite_matrice_partielle, 0, minusone, one, X, r, projet->ef_donnees.c);
	double bnorm = cholmod_l_norm_dense(dense_force, 0, projet->ef_donnees.c);
	double rnorm = cholmod_l_norm_dense(r, 0, projet->ef_donnees.c);
	double xnorm = cholmod_l_norm_dense(X, 0, projet->ef_donnees.c);
	double anorm = cholmod_l_norm_sparse (projet->ef_donnees.rigidite_matrice_partielle, 0, projet->ef_donnees.c) ;
	double axbnorm = (anorm * xnorm + bnorm) ;
	action_en_cours->norm = rnorm / axbnorm ;
	printf("résidu : %e\n", action_en_cours->norm);
	
	// On libère la mémoire
	cholmod_l_free_dense(&Y, projet->ef_donnees.c);
	cholmod_l_free_dense(&X, projet->ef_donnees.c);
	cholmod_l_free_dense(&r, projet->ef_donnees.c);
	cholmod_l_free_sparse(&sparse_force, projet->ef_donnees.c);
	cholmod_l_free_dense(&dense_force, projet->ef_donnees.c);
	
//	Pour utiliser cholmod dans les calculs de matrices.
//	Troisième méthode de calcul donnant directement les calculs sans passer par une matrice intermédiaire.
//	Est moins intéressant puisqu'il faut résoudre l'intégralité du système pour chaque cas de charge.
//	projet->ef_donnees.rigidite_matrice_partielle->stype = 0;
//	cholmod_sparse *tttt = SuiteSparseQR_C_backslash_sparse(0, 0., projet->ef_donnees.rigidite_matrice_partielle, sparse_force, projet->ef_donnees.c);
//	printf("déplacement 5\n");
//	cholmod_l_write_sparse(stdout, tttt, NULL, NULL, projet->ef_donnees.c);*/
	
	return 0;
}
