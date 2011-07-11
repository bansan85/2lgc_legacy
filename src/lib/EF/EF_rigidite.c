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
#include <cholmod.h>
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
	
	projet->ef_donnees.rigidite_matrice_partielle = NULL;
	projet->ef_donnees.inv_rigidite_matrice_partielle = NULL;
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


/* EF_rigidite_free
 * Description : Libère la liste contenant la matrice de rigidité
 * Paramètres : Projet *projet : la variable projet
 * Valeur renvoyée :
 *   Succès : 0 même si la matrice de rigidité est vide
 *   Échec : valeur négative si la liste n'est pas initialisée ou a déjà été libérée
 */
int EF_rigidite_free(Projet *projet)
{
	unsigned int	i;
	
	if ((projet == NULL) || (projet->ef_donnees.rigidite_list == NULL))
		BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
	
	while (!list_empty(projet->ef_donnees.rigidite_list))
	{
		EF_rigidite	*rigidite = list_remove_front(projet->ef_donnees.rigidite_list);
		
		free(rigidite);
	}
	
	free(projet->ef_donnees.rigidite_list);
	projet->ef_donnees.rigidite_list = NULL;
	if (projet->ef_donnees.rigidite_matrice_partielle != NULL)
	{
		cholmod_l_free_sparse(&(projet->ef_donnees.rigidite_matrice_partielle), projet->ef_donnees.c);
		projet->ef_donnees.rigidite_matrice_partielle = NULL;
	}
	if (projet->ef_donnees.inv_rigidite_matrice_partielle != NULL)
	{
		cholmod_l_free_sparse(&(projet->ef_donnees.inv_rigidite_matrice_partielle), projet->ef_donnees.c);
		projet->ef_donnees.inv_rigidite_matrice_partielle = NULL;
	}
	
	if (projet->ef_donnees.noeuds_flags_partielle != NULL)
	{
		for (i=0;i<list_size(projet->ef_donnees.noeuds);i++)
			free(projet->ef_donnees.noeuds_flags_partielle[i]);
		free(projet->ef_donnees.noeuds_flags_partielle);
	}
	
	
	return 0;
}

