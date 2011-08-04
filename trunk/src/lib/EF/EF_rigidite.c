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
    projet->ef_donnees.rigidite_matrice_complete = NULL;
//  Pour utiliser cholmod dans les calculs de matrices.
//  projet->ef_donnees.factor_rigidite_matrice_partielle = NULL;
    projet->ef_donnees.QR = NULL;
    projet->ef_donnees.triplet_rigidite_partielle = NULL;
    projet->ef_donnees.triplet_rigidite_complete = NULL;
    
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
    unsigned int    i;
    
    if (projet == NULL)
        BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
    
    if (projet->ef_donnees.triplet_rigidite_partielle != NULL)
        cholmod_l_free_triplet(&projet->ef_donnees.triplet_rigidite_partielle, projet->ef_donnees.c);
    if (projet->ef_donnees.triplet_rigidite_complete != NULL)
        cholmod_l_free_triplet(&projet->ef_donnees.triplet_rigidite_complete, projet->ef_donnees.c);
    
    if (projet->ef_donnees.rigidite_matrice_complete != NULL)
    {
        cholmod_l_free_sparse(&(projet->ef_donnees.rigidite_matrice_complete), projet->ef_donnees.c);
        projet->ef_donnees.rigidite_matrice_complete = NULL;
    }
    if (projet->ef_donnees.rigidite_matrice_partielle != NULL)
    {
        cholmod_l_free_sparse(&(projet->ef_donnees.rigidite_matrice_partielle), projet->ef_donnees.c);
        projet->ef_donnees.rigidite_matrice_partielle = NULL;
    }
//  Pour utiliser cholmod dans les calculs de matrices.
//  if (projet->ef_donnees.factor_rigidite_matrice_partielle != NULL)
    if (projet->ef_donnees.QR != NULL)
    {
        SuiteSparseQR_C_free(&projet->ef_donnees.QR, projet->ef_donnees.c);
//      Pour utiliser cholmod dans les calculs de matrices.
//      cholmod_l_free_factor(&(projet->ef_donnees.factor_rigidite_matrice_partielle), projet->ef_donnees.c);
//      projet->ef_donnees.factor_rigidite_matrice_partielle = NULL;
    }
    
    if (projet->ef_donnees.noeuds_flags_complete != NULL)
    {
        for (i=0;i<list_size(projet->ef_donnees.noeuds);i++)
            free(projet->ef_donnees.noeuds_flags_complete[i]);
        free(projet->ef_donnees.noeuds_flags_complete);
    }
    if (projet->ef_donnees.noeuds_flags_partielle != NULL)
    {
        for (i=0;i<list_size(projet->ef_donnees.noeuds);i++)
            free(projet->ef_donnees.noeuds_flags_partielle[i]);
        free(projet->ef_donnees.noeuds_flags_partielle);
    }
    
    
    return 0;
}

