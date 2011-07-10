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
#include "EF_appui.h"

/* EF_rigidite_init
 * Description : Initialise la liste contenant la matrice de rigidité
 * Paramètres : Projet *projet : la variable projet
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : valeur négative
 */
int EF_rigidite_init(Projet *projet)
{
	if (projet == NULL)
		BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
	
	projet->ef_donnees.rigidite_matrice_calc = NULL;
	projet->ef_donnees.inv_rigidite_matrice_calc = NULL;
	projet->ef_donnees.rigidite_list = list_init();
	if (projet->ef_donnees.rigidite_list == NULL)
		BUGTEXTE(-2, gettext("Erreur d'allocation mémoire.\n"));
	else
		return 0;
}


/* EF_rigidite_ajout
 * Description : Ajoute (si nécessaire) l'élément correspondant aux numéros souhaités et le renvoi
 *             : Il reste alors à remplir la matrice
 * Paramètres : Projet *projet : la variable projet
 *            : int noeud1 : le numéro 1
 *            : int noeud2 : le numéro 2
 *            : EF_rigidite *rigidite : la rigidité correspondant aux numéros
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : valeur négative
 */
int EF_rigidite_ajout(Projet *projet, int noeudx, int noeudy, EF_rigidite **rigidite)
{
	EF_rigidite	*rigidite_en_cours, rigidite_nouvelle;
	
	if ((projet == NULL) || (projet->ef_donnees.rigidite_list == NULL))
		BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
	
	if (list_size(projet->ef_donnees.rigidite_list) != 0)
	{
		list_mvfront(projet->ef_donnees.rigidite_list);
		do
		{
			rigidite_en_cours = list_curr(projet->ef_donnees.rigidite_list);
			
			if ((rigidite_en_cours->noeudx == noeudx) && (rigidite_en_cours->noeudy == noeudy))
			{
				*rigidite = rigidite_en_cours;
				return 0;
			}
		}
		while (list_mvnext(projet->ef_donnees.rigidite_list) != NULL);
	}
	
	rigidite_nouvelle.noeudx = noeudx;
	rigidite_nouvelle.noeudy = noeudy;
	memset(&(rigidite_nouvelle.matrice), 0, sizeof(rigidite_nouvelle.matrice));
	if (list_insert_after(projet->ef_donnees.rigidite_list, &(rigidite_nouvelle), sizeof(rigidite_nouvelle)) == NULL)
		BUGTEXTE(-2, gettext("Erreur d'allocation mémoire.\n"));
	*rigidite = list_rear(projet->ef_donnees.rigidite_list);
	
	return 0;
}


/* EF_rigidite_genere_sparse
 * Description : Converti la matrice de rigidité complète de la structure sous forme de liste en une matrice de rigidité partielle (les lignes (et colonnes) dont on connait les déplacements ne sont pas insérées) sous forme d'une matrice sparse
 * Paramètres : Projet *projet : la variable projet
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : valeur négative si la liste de rigidité n'est pas initialisée ou a déjà été libérée
 */
int EF_rigidite_genere_sparse(Projet *projet)
{
	cholmod_triplet		*triplet_rigidite;
	double			max_rig = 0.;
	int			nnz_max = 0, j, k;
	unsigned int		i;
	int			**noeuds_flags;
	long			*ai, *aj;	// Pointeur vers les données des triplets
	double			*ax;		// Pointeur vers les données des triplets
	
	if ((projet == NULL) || (projet->ef_donnees.rigidite_list == NULL) || (list_size(projet->ef_donnees.rigidite_list) == 0))
		BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
	
	// On commence par déterminer la valeur maximale d'une case de la matrice.
	// Cette valeur permettra de déterminer le "0" équivalent sur la base de ERREUR_RELATIVE_MIN.
	// On déermine également le nombre de case non zéro. Il est pris pour zéro 0 et non
	// max_rig*ERREUR_RELATIVE_MIN mais c'est pas grave, ça évite de parcourir 50 fois 
	// la liste. TODO : A tester afin de vérifier que ça ralentisse vraiment devant le temps de 
	// calcul de résolution du système.
	list_mvfront(projet->ef_donnees.rigidite_list);
	do
	{
		EF_rigidite	*rigidite = list_curr(projet->ef_donnees.rigidite_list);
		for (i=0;i<6;i++)
		{
			for (j=0;j<6;j++)
			{
				if (ABS(rigidite->matrice[i][j]) > max_rig)
					max_rig = ABS(rigidite->matrice[i][j]);
				if (rigidite->matrice[i][j] != 0.)
					nnz_max++;
			}
		}
	}
	while (list_mvnext(projet->ef_donnees.rigidite_list) != NULL);
	
	// Détermine pour chaque noeud si la ligne / colonne de la matrice de rigidité globale
	// doit être pris en compte dans la résolution du système.
	// le nombre [0] indique la colonne/ligne correspondant au déplacement selon l'axe x
	// le nombre [5] indique la colonne/ligne correspondant à la rotation autour de l'axe z
	// le nombre vaut -1 si la colonne n'est pas inséré dans la matrice de rigidité globale
	noeuds_flags = malloc(sizeof(int*)*list_size(projet->ef_donnees.noeuds));
	if (noeuds_flags == NULL)
		BUGTEXTE(-2, gettext("Erreur d'allocation mémoire.\n"));
	for (i=0;i<list_size(projet->ef_donnees.noeuds);i++)
	{
		noeuds_flags[i] = malloc(6*sizeof(int));
		if (noeuds_flags[i] == NULL)
			BUGTEXTE(-2, gettext("Erreur d'allocation mémoire.\n"));
	}
	i = 0; // Colonne en cours
	list_mvfront(projet->ef_donnees.noeuds);
	do
	{
		EF_noeud	*noeud = list_curr(projet->ef_donnees.noeuds);
		
		if (noeud->appui == NULL)
		{
			noeuds_flags[noeud->numero][0] = i; i++; // x
			noeuds_flags[noeud->numero][1] = i; i++; // y
			noeuds_flags[noeud->numero][2] = i; i++; // z
			noeuds_flags[noeud->numero][3] = i; i++; // rx
			noeuds_flags[noeud->numero][4] = i; i++; // ry
			noeuds_flags[noeud->numero][5] = i; i++; // rz
		}
		else
		{
			EF_Appui	*appui = noeud->appui;
			if (appui->x == EF_APPUI_LIBRE)
				{ noeuds_flags[noeud->numero][0] = i; i++; }
			else noeuds_flags[noeud->numero][0] = -1;
			if (appui->y == EF_APPUI_LIBRE)
				{ noeuds_flags[noeud->numero][1] = i; i++; }
			else noeuds_flags[noeud->numero][1] = -1;
			if (appui->z == EF_APPUI_LIBRE)
				{ noeuds_flags[noeud->numero][2] = i; i++; }
			else noeuds_flags[noeud->numero][2] = -1;
			if (appui->rx == EF_APPUI_LIBRE)
				{ noeuds_flags[noeud->numero][3] = i; i++; }
			else noeuds_flags[noeud->numero][3] = -1;
			if (appui->ry == EF_APPUI_LIBRE)
				{ noeuds_flags[noeud->numero][4] = i; i++; }
			else noeuds_flags[noeud->numero][4] = -1;
			if (appui->rz == EF_APPUI_LIBRE)
				{ noeuds_flags[noeud->numero][5] = i; i++; }
			else noeuds_flags[noeud->numero][5] = -1;
		}
	}
	while (list_mvnext(projet->ef_donnees.noeuds) != NULL);

	
	// On crée le triplet correspondant à la matrice de rigidité
	triplet_rigidite = cholmod_l_allocate_triplet(i, i, nnz_max, 0, CHOLMOD_REAL, projet->ef_donnees.c);
	ai = triplet_rigidite->i;
	aj = triplet_rigidite->j;
	ax = triplet_rigidite->x;
	k = 0; // Dernier triplet ajouté
	
	// On converti la liste en triplet
	list_mvfront(projet->ef_donnees.rigidite_list);
	do
	{
		EF_rigidite	*rigidite = list_curr(projet->ef_donnees.rigidite_list);
		for (i=0;i<6;i++)
		{
			for (j=0;j<6;j++)
			{
				if ((noeuds_flags[rigidite->noeudx][i] != -1) && (noeuds_flags[rigidite->noeudy][j] != -1) && (max_rig*ERREUR_RELATIVE_MIN < ABS(rigidite->matrice[i][j])))
				{
					ai[k] = i;
					aj[k] = j;
					ax[k] = rigidite->matrice[i][j];
					k++;
				}
			}
		}
	}
	while (list_mvnext(projet->ef_donnees.rigidite_list) != NULL);
	triplet_rigidite->nnz = k;
	
	// Puis en sparse matrice
	projet->ef_donnees.rigidite_matrice_calc = cholmod_l_triplet_to_sparse(triplet_rigidite, 0, projet->ef_donnees.c);
	cholmod_l_free_triplet(&triplet_rigidite, projet->ef_donnees.c);
	
	// Puis on inverse la matrice en cherchant la matrice x de tel sorte que Kx=1. Ainsi x est l'inverse
	cholmod_triplet *zero_rigidite = cholmod_l_allocate_triplet(6, 6, 6, 0, CHOLMOD_REAL, projet->ef_donnees.c);
	ai = zero_rigidite->i;
	aj = zero_rigidite->j;
	ax = zero_rigidite->x;
	zero_rigidite->nnz=6;
	for (i=0;i<6;i++)
	{
		ai[i] = i;
		aj[i] = i;
		ax[i] = 1.;
	}
	cholmod_sparse	*zeros_spars = cholmod_l_triplet_to_sparse(zero_rigidite, 0, projet->ef_donnees.c);
	cholmod_l_free_triplet(&zero_rigidite, projet->ef_donnees.c);
	projet->ef_donnees.inv_rigidite_matrice_calc = SuiteSparseQR_C_backslash_sparse(0, 0.000000000000000000000000000000001, projet->ef_donnees.rigidite_matrice_calc, zeros_spars, projet->ef_donnees.c);
	cholmod_l_free_sparse(&zeros_spars, projet->ef_donnees.c);
	
	for (i=0;i<list_size(projet->ef_donnees.noeuds);i++)
		free(noeuds_flags[i]);
	free(noeuds_flags);
	
	return 0;
}


/* EF_rigidite_free
 * Description : Libère la liste contenant la matrice de rigidité
 * Paramètres : Projet *projet : la variable projet
 * Valeur renvoyée :
 *   Succès : 0 même si la matrice de rigidité est vide
 *   Échec : valeur négative si la liste n'est pas initialisée ou a déjà été libérée
 */
int EF_rigidite_free(Projet *projet)
{
	if ((projet == NULL) || (projet->ef_donnees.rigidite_list == NULL))
		BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
	
	while (!list_empty(projet->ef_donnees.rigidite_list))
	{
		EF_rigidite	*rigidite = list_remove_front(projet->ef_donnees.rigidite_list);
		
		free(rigidite);
	}
	
	free(projet->ef_donnees.rigidite_list);
	projet->ef_donnees.rigidite_list = NULL;
	if (projet->ef_donnees.rigidite_matrice_calc != NULL)
	{
		cholmod_l_free_sparse(&(projet->ef_donnees.rigidite_matrice_calc), projet->ef_donnees.c);
		projet->ef_donnees.rigidite_matrice_calc = NULL;
	}
	if (projet->ef_donnees.inv_rigidite_matrice_calc != NULL)
	{
		cholmod_l_free_sparse(&(projet->ef_donnees.inv_rigidite_matrice_calc), projet->ef_donnees.c);
		projet->ef_donnees.inv_rigidite_matrice_calc = NULL;
	}
	
	return 0;
}

