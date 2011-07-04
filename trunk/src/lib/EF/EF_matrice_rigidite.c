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
#include "EF_appui.h"

/* EF_appuis_init
 * Description : Initialise la liste des appuis
 * Paramètres : Projet *projet : la variable projet
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : valeur négative
 */
int EF_appuis_init(Projet *projet)
{
	if (projet == NULL)
		BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
	
	projet->ef_donnees.appuis = list_init();
	if (projet->ef_donnees.appuis == NULL)
		BUGTEXTE(-2, gettext("Erreur d'allocation mémoire.\n"));
	else
		return 0;
}


/* EF_appuis_ajout
 * Description : Ajouter un appui à la structure
 * Paramètres : Projet *projet : la variable projet
 *            : Type_EF_Appui x : définition du déplacement en x,
 *            : Type_EF_Appui y : définition du déplacement en y,
 *            : Type_EF_Appui z : définition du déplacement en z,
 *            : Type_EF_Appui rx : définition de la rotation autour de l'axe x,
 *            : Type_EF_Appui ry : définition de la rotation autour de l'axe y,
 *            : Type_EF_Appui rz : définition de la rotation autour de l'axe z.
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : valeur négative
 */
int EF_appuis_ajout(Projet *projet, Type_EF_Appui x, Type_EF_Appui y, Type_EF_Appui z, Type_EF_Appui rx, Type_EF_Appui ry, Type_EF_Appui rz)
{
	EF_Appui		*appui_en_cours, appui_nouveau;
	
	if ((projet == NULL) || (projet->ef_donnees.appuis == NULL))
		BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
	
	list_mvrear(projet->ef_donnees.appuis);
	appui_nouveau.x = x;
	switch (x)
	{
		case EF_APPUI_LIBRE :
		case EF_APPUI_BLOQUE :
		{
			appui_nouveau.x_donnees = NULL;
			break;
		}
		default:
		{
			BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
			break;
		}
	}
	appui_nouveau.y = y;
	switch (y)
	{
		case EF_APPUI_LIBRE :
		case EF_APPUI_BLOQUE :
		{
			appui_nouveau.y_donnees = NULL;
			break;
		}
		default:
		{
			BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
			break;
		}
	}
	appui_nouveau.z = z;
	switch (z)
	{
		case EF_APPUI_LIBRE :
		case EF_APPUI_BLOQUE :
		{
			appui_nouveau.z_donnees = NULL;
			break;
		}
		default:
		{
			BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
			break;
		}
	}
	appui_nouveau.rx = rx;
	switch (rx)
	{
		case EF_APPUI_LIBRE :
		case EF_APPUI_BLOQUE :
		{
			appui_nouveau.rx_donnees = NULL;
			break;
		}
		default:
		{
			BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
			break;
		}
	}
	appui_nouveau.ry = ry;
	switch (ry)
	{
		case EF_APPUI_LIBRE :
		case EF_APPUI_BLOQUE :
		{
			appui_nouveau.ry_donnees = NULL;
			break;
		}
		default:
		{
			BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
			break;
		}
	}
	appui_nouveau.rz = rz;
	switch (rz)
	{
		case EF_APPUI_LIBRE :
		case EF_APPUI_BLOQUE :
		{
			appui_nouveau.rz_donnees = NULL;
			break;
		}
		default:
		{
			BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
			break;
		}
	}
	
	appui_en_cours = (EF_Appui *)list_rear(projet->ef_donnees.appuis);
	if (appui_en_cours == NULL)
		appui_nouveau.numero = 0;
	else
		appui_nouveau.numero = appui_en_cours->numero+1;
	
	if (list_insert_after(projet->ef_donnees.appuis, &(appui_nouveau), sizeof(appui_nouveau)) == NULL)
		BUGTEXTE(-2, gettext("Erreur d'allocation mémoire.\n"));
	
	return 0;
}


/* EF_appuis_cherche_numero
 * Description : Positionne dans la liste des appuis au numéro souhaité
 * Paramètres : Projet *projet : la variable projet
 *            : int numero : le numéro de l'appui
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : valeur négative
 */
int EF_appuis_cherche_numero(Projet *projet, int numero)
{
	if ((projet == NULL) || (projet->ef_donnees.appuis == NULL) || (list_size(projet->ef_donnees.appuis) == 0))
		BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
	
	list_mvfront(projet->ef_donnees.appuis);
	do
	{
		EF_Appui	*appui = list_curr(projet->ef_donnees.appuis);
		
		if (appui->numero == numero)
			return 0;
	}
	while (list_mvnext(projet->ef_donnees.appuis) != NULL);
	
	BUGTEXTE(-2, gettext("Appui n°%d introuvable.\n"), numero);
}


/* EF_appuis_free
 * Description : Libère l'ensemble des appuis
 * Paramètres : Projet *projet : la variable projet
 * Valeur renvoyée :
 *   Succès : 0 même si aucun appui n'est existant
 *   Échec : valeur négative si la liste des appuis n'est pas initialisée ou a déjà été libérée
 */
int EF_appuis_free(Projet *projet)
{
	if ((projet == NULL) || (projet->ef_donnees.appuis == NULL))
		BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
	
	while (!list_empty(projet->ef_donnees.appuis))
	{
		EF_Appui	*appui = list_remove_front(projet->ef_donnees.appuis);
		
		free(appui);
	}
	
	free(projet->ef_donnees.appuis);
	projet->ef_donnees.appuis = NULL;
	
	return 0;
}

