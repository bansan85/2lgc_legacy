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

int EF_rigidite_init(Projet *projet)
/* Description : Initialise à NULL les différentes matrices de rigidité
 * Paramètres : Projet *projet : la variable projet
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet == NULL)
 */
{
    BUGMSG(projet, -1, gettext("Paramètre incorrect\n"));
    
    // Trivial
    projet->ef_donnees.rigidite_matrice_partielle = NULL;
    projet->ef_donnees.rigidite_matrice_complete = NULL;
    projet->ef_donnees.numeric = NULL;
    projet->ef_donnees.ap = NULL;
    projet->ef_donnees.ai = NULL;
    projet->ef_donnees.ax = NULL;
    projet->ef_donnees.triplet_rigidite_partielle = NULL;
    projet->ef_donnees.triplet_rigidite_complete = NULL;
    projet->ef_donnees.noeuds_pos_complete = NULL;
    projet->ef_donnees.noeuds_pos_partielle = NULL;
    
    return 0;
}


int EF_rigidite_free(Projet *projet)
/* Description : Libère la liste contenant la matrice de rigidité
 * Paramètres : Projet *projet : la variable projet
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet == NULL)
 */
{
    unsigned int    i;
    
    BUGMSG(projet, -1, gettext("Paramètre incorrect\n"));
    
    // Trivial
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
    umfpack_dl_free_numeric(&projet->ef_donnees.numeric);
    free(projet->ef_donnees.ap);
    free(projet->ef_donnees.ai);
    free(projet->ef_donnees.ax);
    
    if (projet->ef_donnees.noeuds_pos_complete != NULL)
    {
        for (i=0;i<list_size(projet->ef_donnees.noeuds);i++)
            free(projet->ef_donnees.noeuds_pos_complete[i]);
        free(projet->ef_donnees.noeuds_pos_complete);
    }
    if (projet->ef_donnees.noeuds_pos_partielle != NULL)
    {
        for (i=0;i<list_size(projet->ef_donnees.noeuds);i++)
            free(projet->ef_donnees.noeuds_pos_partielle[i]);
        free(projet->ef_donnees.noeuds_pos_partielle);
    }
    
    return 0;
}

