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
#include "EF_noeud.h"
#include "EF_appui.h"

/* EF_noeuds_init
 * Description : Initialise la liste des noeuds
 * Paramètres : Projet *projet : la variable projet
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : valeur négative
 */
int EF_noeuds_init(Projet *projet)
{
	if (projet == NULL)
		BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
	
	projet->ef_donnees.noeuds = list_init();
	if (projet->ef_donnees.noeuds == NULL)
		BUGTEXTE(-2, gettext("Erreur d'allocation mémoire.\n"));
	else
		return 0;
}


/* EF_noeuds_ajout
 * Description : Ajouter un noeud à la structure
 * Paramètres : Projet *projet : la variable projet
 *            : double x : position en x
 *            : double y : position en y
 *            : double z : position en z
 *            : int appui : numéro de l'appui. -1 si aucun appui.
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : valeur négative
 */
int EF_noeuds_ajout(Projet *projet, double x, double y, double z, int appui)
{
	EF_noeud		*noeud_en_cours, noeud_nouveau;
	
	if ((projet == NULL) || (projet->ef_donnees.noeuds == NULL))
		BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
	
	list_mvrear(projet->ef_donnees.noeuds);
	noeud_nouveau.position.x = x;
	noeud_nouveau.position.y = y;
	noeud_nouveau.position.z = z;
	
	if (appui == -1)
		noeud_nouveau.appui = NULL;
	else
	{
		if (EF_appuis_cherche_numero(projet, appui) != 0)
			return -2;
		noeud_nouveau.appui = (EF_Appui *)(list_curr(projet->ef_donnees.appuis));
	}
		
	noeud_en_cours = (EF_noeud *)list_rear(projet->ef_donnees.noeuds);
	if (noeud_en_cours == NULL)
		noeud_nouveau.numero = 0;
	else
		noeud_nouveau.numero = noeud_en_cours->numero+1;
	
	if (list_insert_after(projet->ef_donnees.noeuds, &(noeud_nouveau), sizeof(noeud_nouveau)) == NULL)
		BUGTEXTE(-2, gettext("Erreur d'allocation mémoire.\n"));
	
	return 0;
}


/* EF_noeuds_cherche_numero
 * Description : Positionne dans la liste des noeuds l'élément courant au numéro souhaité
 * Paramètres : Projet *projet : la variable projet
 *            : int numero : le numéro du noeud
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : valeur négative
 */
int EF_noeuds_cherche_numero(Projet *projet, int numero)
{
	if ((projet == NULL) || (projet->ef_donnees.noeuds == NULL) || (list_size(projet->ef_donnees.noeuds) == 0))
		BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
	
	list_mvfront(projet->ef_donnees.noeuds);
	do
	{
		EF_noeud	*noeud = list_curr(projet->ef_donnees.noeuds);
		
		if (noeud->numero == numero)
			return 0;
	}
	while (list_mvnext(projet->ef_donnees.noeuds) != NULL);
	
	BUGTEXTE(-2, gettext("Noeud n°%d introuvable.\n"), numero);
}


/* EF_noeuds_free
 * Description : Libère l'ensemble des noeuds
 * Paramètres : Projet *projet : la variable projet
 * Valeur renvoyée :
 *   Succès : 0 même si aucun noeud n'est existant
 *   Échec : valeur négative si la liste des noeuds n'est pas initialisée ou a déjà été libérée
 */
int EF_noeuds_free(Projet *projet)
{
	if ((projet == NULL) || (projet->ef_donnees.noeuds == NULL))
		BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
	
	while (!list_empty(projet->ef_donnees.noeuds))
	{
		EF_noeud	*noeud = list_remove_front(projet->ef_donnees.noeuds);
		
		free(noeud);
	}
	
	free(projet->ef_donnees.noeuds);
	projet->ef_donnees.noeuds = NULL;
	
	return 0;
}

