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
	// Rigidite doit être libéré avant noeud car pour libérer toute la mémoire, il est nécessaire d'avoir accès aux informations contenues dans les noeuds
	EF_rigidite_free(projet);
	if (projet->ef_donnees.noeuds != NULL)
		EF_noeuds_free(projet);
	if (projet->beton.sections != NULL)
		_1992_1_1_sections_free(projet);
	if (projet->beton.elements != NULL)
		_1992_1_1_elements_free(projet);
	if (projet->ef_donnees.appuis != NULL)
		EF_appuis_free(projet);
	if (projet->beton.materiaux != NULL)
		_1992_1_1_materiaux_free(projet);
	
	cholmod_l_finish(projet->ef_donnees.c);
	
	// On ne vérifie pas si l'élément est NULL car free s'en charge avant de libérer la mémoire
	free(projet->list_gtk._1990);
	projet->list_gtk._1990 = NULL;
	free(projet);
	
	return 0;
}
