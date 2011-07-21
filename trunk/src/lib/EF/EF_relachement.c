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
#include "EF_relachement.h"

/* EF_relachement_init
 * Description : Initialise la liste des relachements
 * Paramètres : Projet *projet : la variable projet
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : valeur négative
 */
int EF_relachement_init(Projet *projet)
{
	if (projet == NULL)
		BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
	
	projet->ef_donnees.relachements = list_init();
	if (projet->ef_donnees.relachements == NULL)
		BUGTEXTE(-2, gettext("Erreur d'allocation mémoire.\n"));
	else
		return 0;
}


/* EF_relachement_ajout
 * Description : Ajouter un relachement
 * Paramètres : Projet *projet : la variable projet
 *            : Type_EF_Appui rx_debut : relachement de la rotation par rapport à l'axe x du début de la barre
 *            : Type_EF_Appui ry_debut : relachement de la rotation par rapport à l'axe y du début de la barre
 *            : Type_EF_Appui rz_debut : relachement de la rotation par rapport à l'axe z du début de la barre
 *            : Type_EF_Appui rx_fin : relachement de la rotation par rapport à l'axe x de la fin de la barre
 *            : Type_EF_Appui ry_fin : relachement de la rotation par rapport à l'axe y de la fin de la barre
 *            : Type_EF_Appui rz_fin : relachement de la rotation par rapport à l'axe z de la fin de la barre
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : valeur négative
 */
int EF_relachement_ajout(Projet *projet, Type_EF_Relachement rx_debut, Type_EF_Relachement ry_debut, Type_EF_Relachement rz_debut, Type_EF_Relachement rx_fin, Type_EF_Relachement ry_fin, Type_EF_Relachement rz_fin)
{
	Relachement		*relachement_en_cours, relachement_nouveau;
	
	if ((projet == NULL) || (projet->ef_donnees.relachements == NULL) || ((rx_debut == EF_RELACHEMENT_LIBRE) && (rx_fin == EF_RELACHEMENT_LIBRE)))
		BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
	
	list_mvrear(projet->ef_donnees.relachements);
	relachement_nouveau.rx_debut = rx_debut;
	relachement_nouveau.ry_debut = ry_debut;
	relachement_nouveau.rz_debut = rz_debut;
	relachement_nouveau.rx_fin = rx_fin;
	relachement_nouveau.ry_fin = ry_fin;
	relachement_nouveau.rz_fin = rz_fin;
	
	relachement_en_cours = (Relachement *)list_rear(projet->ef_donnees.relachements);
	if (relachement_en_cours == NULL)
		relachement_nouveau.numero = 0;
	else
		relachement_nouveau.numero = relachement_en_cours->numero+1;
	
	if (list_insert_after(projet->ef_donnees.relachements, &(relachement_nouveau), sizeof(relachement_nouveau)) == NULL)
		BUGTEXTE(-2, gettext("Erreur d'allocation mémoire.\n"));
	
	return 0;
}


/* EF_relachement_cherche_numero
 * Description : Renvoie le relachement cherché
 * Paramètres : Projet *projet : la variable projet
 *            : unsigned int numero : le numéro du noeud
 * Valeur renvoyée :
 *   Succès : pointeur vers le relachement recherché
 *   Échec : NULL
 */
Relachement* EF_relachement_cherche_numero(Projet *projet, unsigned int numero)
{
	if ((projet == NULL) || (projet->ef_donnees.relachements == NULL) || (list_size(projet->ef_donnees.relachements) == 0))
		BUGTEXTE(NULL, gettext("Paramètres invalides.\n"));
	
	list_mvfront(projet->ef_donnees.relachements);
	do
	{
		Relachement	*relachement = list_curr(projet->ef_donnees.relachements);
		
		if (relachement->numero == numero)
			return relachement;
	}
	while (list_mvnext(projet->ef_donnees.relachements) != NULL);
	
	BUGTEXTE(NULL, gettext("Relachement n°%d introuvable.\n"), numero);
}


/* EF_relachement_free
 * Description : Libère l'ensemble des relachements
 * Paramètres : Projet *projet : la variable projet
 * Valeur renvoyée :
 *   Succès : 0 même si aucun noeud n'est existant
 *   Échec : valeur négative si la liste des relachements n'est pas initialisée ou a déjà été libérée
 */
int EF_relachement_free(Projet *projet)
{
	if ((projet == NULL) || (projet->ef_donnees.relachements == NULL))
		BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
	
	while (!list_empty(projet->ef_donnees.relachements))
	{
		Relachement	*relachement = list_remove_front(projet->ef_donnees.relachements);
		
		free(relachement);
	}
	
	free(projet->ef_donnees.relachements);
	projet->ef_donnees.relachements = NULL;
	
	return 0;
}

