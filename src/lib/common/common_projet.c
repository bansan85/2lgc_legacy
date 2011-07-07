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

#include <stdio.h>
#include <stdlib.h>
#include <libintl.h>
#include <locale.h>
#include <SuiteSparseQR_C.h>

#include "common_projet.h"
#include "common_erreurs.h"
#include "common_maths.h"
#include "1990_actions.h"
#include "1990_groupes.h"
#include "1990_combinaisons.h"
#include "EF_noeud.h"
#include "EF_appui.h"
#include "EF_rigidite.h"
#include "1992_1_1_elements.h"
#include "1992_1_1_section.h"
#include "1992_1_1_materiaux.h"

/* projet_init
 * Description : Initialise la variable projet
 * Paramètres : Aucun
 * Valeur renvoyée :
 *   Succès : Un pointeur vers une zone mémoire projet
 *   Échec : NULL
 */
Projet *projet_init()
{
	Projet		*projet;
	cholmod_dense	*X, *B;
	
	
	projet = (Projet*)malloc(sizeof(Projet));
	if (projet == NULL)
		BUG(NULL);
	if (_1990_action_init(projet) != 0)
	{
		free(projet);
		BUG(NULL);
	}
	if (_1990_groupe_init(projet) != 0)
	{
		_1990_action_free(projet);
		free(projet);
		BUG(NULL);
	}
	if (_1990_combinaisons_init(projet) != 0)
	{
		_1990_groupe_free(projet);
		_1990_action_free(projet);
		free(projet);
		BUG(NULL);
	}
	if (EF_noeuds_init(projet) != 0)
	{
		_1990_combinaisons_free(projet);
		_1990_groupe_free(projet);
		_1990_action_free(projet);
		free(projet);
		BUG(NULL);
	}
	if (_1992_1_1_sections_init(projet) != 0)
	{
		EF_noeuds_free(projet);
		_1990_combinaisons_free(projet);
		_1990_groupe_free(projet);
		_1990_action_free(projet);
		free(projet);
		BUG(NULL);
	}
	if (_1992_1_1_elements_init(projet) != 0)
	{
		_1992_1_1_sections_free(projet);
		EF_noeuds_free(projet);
		_1990_combinaisons_free(projet);
		_1990_groupe_free(projet);
		_1990_action_free(projet);
		free(projet);
		BUG(NULL);
	}
	if (EF_appuis_init(projet) != 0)
	{
		_1992_1_1_elements_free(projet);
		_1992_1_1_sections_free(projet);
		EF_noeuds_free(projet);
		_1990_combinaisons_free(projet);
		_1990_groupe_free(projet);
		_1990_action_free(projet);
		free(projet);
		BUG(NULL);
	}
	if (EF_rigidite_init(projet) != 0)
	{
		EF_appuis_free(projet);
		_1992_1_1_elements_free(projet);
		_1992_1_1_sections_free(projet);
		EF_noeuds_free(projet);
		_1990_combinaisons_free(projet);
		_1990_groupe_free(projet);
		_1990_action_free(projet);
		free(projet);
		BUGTEXTE(NULL, gettext("Erreur d'allocation mémoire.\n"));
	}
	if (_1992_1_1_materiaux_init(projet) != 0)
	{
		EF_rigidite_free(projet);
		EF_appuis_free(projet);
		_1992_1_1_elements_free(projet);
		_1992_1_1_sections_free(projet);
		EF_noeuds_free(projet);
		_1990_combinaisons_free(projet);
		_1990_groupe_free(projet);
		_1990_action_free(projet);
		free(projet);
		BUGTEXTE(NULL, gettext("Erreur d'allocation mémoire.\n"));
	}

	projet->ef_donnees.c = &(projet->ef_donnees.Common);
	cholmod_l_start(projet->ef_donnees.c);
	printf("1.\n");
	projet->ef_donnees.a = cholmod_l_allocate_triplet(3, 3, 9, 0, CHOLMOD_REAL, projet->ef_donnees.c);
	projet->ef_donnees.ai = projet->ef_donnees.a->i;
	projet->ef_donnees.aj = projet->ef_donnees.a->j;
	projet->ef_donnees.ax = projet->ef_donnees.a->x;
	projet->ef_donnees.ai[0] = 0;
	projet->ef_donnees.aj[0] = 0;
	projet->ef_donnees.ax[0] = 1;
	projet->ef_donnees.ai[8] = 1;
	projet->ef_donnees.aj[8] = 0;
	projet->ef_donnees.ax[8] = 3;
	projet->ef_donnees.ai[1] = 1;
	projet->ef_donnees.aj[1] = 1;
	projet->ef_donnees.ax[1] = 4;
	projet->ef_donnees.ai[7] = 2;
	projet->ef_donnees.aj[7] = 0;
	projet->ef_donnees.ax[7] = 4;
	projet->ef_donnees.ai[3] = 2;
	projet->ef_donnees.aj[3] = 1;
	projet->ef_donnees.ax[3] = 5;
	projet->ef_donnees.ai[6] = 2;
	projet->ef_donnees.aj[6] = 2;
	projet->ef_donnees.ax[6] = 6;
	projet->ef_donnees.ai[5] = 0;
	projet->ef_donnees.aj[5] = 1;
	projet->ef_donnees.ax[5] = 3;
	projet->ef_donnees.ai[4] = 0;
	projet->ef_donnees.aj[4] = 2;
	projet->ef_donnees.ax[4] = 4;
	projet->ef_donnees.ai[2] = 1;
	projet->ef_donnees.aj[2] = 2;
	projet->ef_donnees.ax[2] = 5;
	projet->ef_donnees.a->nnz = 9;
	
	printf("2.\n");
	projet->ef_donnees.A = cholmod_l_triplet_to_sparse(projet->ef_donnees.a, 0, projet->ef_donnees.c);
	printf("3.\n");
	cholmod_l_free_triplet(&(projet->ef_donnees.a), projet->ef_donnees.c);
	projet->ef_donnees.a = NULL;
	projet->ef_donnees.ai = NULL;
	projet->ef_donnees.aj = NULL;
	projet->ef_donnees.ax = NULL;
	printf("4.\n");
	cholmod_l_print_sparse(projet->ef_donnees.A, "A", projet->ef_donnees.c);
	printf("5.\n");
	cholmod_l_write_sparse(stdout, projet->ef_donnees.A, NULL, NULL, projet->ef_donnees.c);
	
	printf("testt\n");
	cholmod_sparse *AA = cholmod_l_copy_sparse(projet->ef_donnees.A, projet->ef_donnees.c);
	cholmod_l_write_sparse(stdout, AA, NULL, NULL, projet->ef_donnees.c);
	cholmod_sparse *testt = SuiteSparseQR_C_backslash_sparse(1, ERREUR_RELATIVE_MIN, projet->ef_donnees.A, AA, projet->ef_donnees.c);
	cholmod_l_write_sparse(stdout, testt, NULL, NULL, projet->ef_donnees.c);
	B = cholmod_l_ones (projet->ef_donnees.A->nrow, 1, projet->ef_donnees.A->xtype, projet->ef_donnees.c);
	
	printf("treztrez\n");
	X = SuiteSparseQR_C_backslash(SPQR_ORDERING_DEFAULT, ERREUR_RELATIVE_MIN, projet->ef_donnees.A, B, projet->ef_donnees.c);
/*	cholmod_factor *L;
	L = cholmod_l_analyze (projet->ef_donnees.A, projet->ef_donnees.c) ;
	cholmod_l_factorize (projet->ef_donnees.A, L, projet->ef_donnees.c) ;
	X = cholmod_l_solve (CHOLMOD_A, L, B, projet->ef_donnees.c) ;*/
	printf("treztreztrez\n");
	cholmod_l_print_dense(X, "X", projet->ef_donnees.c);
	printf("treztreztreztrez\n");
	cholmod_l_write_dense(stdout, X, NULL, projet->ef_donnees.c);
	printf("treztreztreztreztrze\n");
	
	cholmod_l_free_sparse(&(projet->ef_donnees.A), projet->ef_donnees.c);
	cholmod_l_free_dense(&B, projet->ef_donnees.c);
	cholmod_l_free_dense(&X, projet->ef_donnees.c);
	
	projet->list_gtk._1990 = NULL;
	
	projet->pays = PAYS_EU;
	return projet;
}

/* projet_free
 * Description : Libère les allocations mémoires de l'ensemble de la variable projet
 * Paramètres : Aucun
 * Valeur renvoyée :
 *   Succès : Un pointeur vers une zone mémoire projet
 *   Échec : NULL
 */
int projet_free(Projet *projet)
{
	if (projet->actions != NULL)
		_1990_action_free(projet);
	if (projet->niveaux_groupes != NULL)
		_1990_groupe_free(projet);
	if (projet->combinaisons.elu_equ != NULL)
		_1990_combinaisons_free(projet);
	if (projet->ef_donnees.noeuds != NULL)
		EF_noeuds_free(projet);
	if (projet->beton.sections != NULL)
		_1992_1_1_sections_free(projet);
	if (projet->beton.elements != NULL)
		_1992_1_1_elements_free(projet);
	if (projet->ef_donnees.appuis != NULL)
		EF_appuis_free(projet);
	if (projet->ef_donnees.rigidite != NULL)
		EF_rigidite_free(projet);
	if (projet->beton.materiaux != NULL)
		_1992_1_1_materiaux_free(projet);
	
	cholmod_finish(projet->ef_donnees.c);
	
	// On ne vérifie pas si l'élément est NULL car free s'en charge avant de libérer la mémoire
	free(projet->list_gtk._1990);
	projet->list_gtk._1990 = NULL;
	free(projet);
	
	return 0;
}
