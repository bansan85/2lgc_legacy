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
#include "EF_rigidite.h"

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
	
	projet->ef_donnees.rigidite = list_init();
	if (projet->ef_donnees.rigidite == NULL)
		BUGTEXTE(-2, gettext("Erreur d'allocation mémoire.\n"));
	else
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
	if ((projet == NULL) || (projet->ef_donnees.rigidite == NULL))
		BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
	
	while (!list_empty(projet->ef_donnees.rigidite))
	{
		EF_rigidite	*rigidite = list_remove_front(projet->ef_donnees.rigidite);
		free(rigidite->matrice);
		
		free(rigidite);
	}
	
	free(projet->ef_donnees.rigidite);
	projet->ef_donnees.rigidite = NULL;
	
	return 0;
}

