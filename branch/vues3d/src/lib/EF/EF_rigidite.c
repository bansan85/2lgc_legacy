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
#include <string.h>
#include <cholmod.h>
#include <gmodule.h>

#include "common_projet.h"
#include "common_erreurs.h"

gboolean EF_rigidite_init(Projet *projet)
/* Description : Initialise à NULL les différentes matrices de rigidité.
 * Paramètres : Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL.
 */
{
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    
    // Trivial
    projet->calculs.rigidite_matrice_partielle = NULL;
    projet->calculs.rigidite_matrice_complete = NULL;
    projet->calculs.numeric = NULL;
    projet->calculs.ap = NULL;
    projet->calculs.ai = NULL;
    projet->calculs.ax = NULL;
    projet->calculs.triplet_rigidite_partielle = NULL;
    projet->calculs.triplet_rigidite_complete = NULL;
    projet->calculs.noeuds_pos_complete = NULL;
    projet->calculs.noeuds_pos_partielle = NULL;
    
    return TRUE;
}


gboolean EF_rigidite_free(Projet *projet)
/* Description : Libère la liste contenant la matrice de rigidité.
 * Paramètres : Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL.
 */
{
    unsigned int    i;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    
    // Trivial
    cholmod_free_triplet(&projet->calculs.triplet_rigidite_partielle, projet->calculs.c);
    projet->calculs.triplet_rigidite_partielle = NULL;
    cholmod_free_triplet(&projet->calculs.triplet_rigidite_complete, projet->calculs.c);
    projet->calculs.triplet_rigidite_complete = NULL;
    
    cholmod_free_sparse(&(projet->calculs.rigidite_matrice_complete), projet->calculs.c);
    projet->calculs.rigidite_matrice_complete = NULL;
    cholmod_free_sparse(&(projet->calculs.rigidite_matrice_partielle), projet->calculs.c);
    projet->calculs.rigidite_matrice_partielle = NULL;
    
    umfpack_di_free_numeric(&projet->calculs.numeric);
    projet->calculs.numeric = NULL;
    
    free(projet->calculs.ap);
    projet->calculs.ap = NULL;
    free(projet->calculs.ai);
    projet->calculs.ai = NULL;
    free(projet->calculs.ax);
    projet->calculs.ax = NULL;
    
    if (projet->calculs.noeuds_pos_complete != NULL)
    {
        for (i=0;i<g_list_length(projet->modele.noeuds);i++)
            free(projet->calculs.noeuds_pos_complete[i]);
        free(projet->calculs.noeuds_pos_complete);
        projet->calculs.noeuds_pos_complete = NULL;
    }
    if (projet->calculs.noeuds_pos_partielle != NULL)
    {
        for (i=0;i<g_list_length(projet->modele.noeuds);i++)
            free(projet->calculs.noeuds_pos_partielle[i]);
        free(projet->calculs.noeuds_pos_partielle);
        projet->calculs.noeuds_pos_partielle = NULL;
    }
    
    return TRUE;
}

