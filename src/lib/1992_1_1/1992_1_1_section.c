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
#include "1992_1_1_section.h"

/* _1992_1_1_sections_init
 * Description : Initialise la liste des section en béton
 * Paramètres : Projet *projet : la variable projet
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : valeur négative
 */
int _1992_1_1_sections_init(Projet *projet)
{
	if (projet == NULL)
		BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
	
	projet->beton.sections = list_init();
	if (projet->beton.sections == NULL)
		BUGTEXTE(-2, gettext("Erreur d'allocation mémoire.\n"));
	else
		return 0;
}

/* _1992_1_1_sections_ajout_rectangulaire
 * Description : ajouter une nouvelle section rectangulaire à la liste des sections en béton
 * Paramètres : Projet *projet : la variable projet
 *            : double largeur : la largeur
 *            : double hauteur : la hauteur
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : valeur négative
 */
int _1992_1_1_sections_ajout_rectangulaire(Projet *projet, double largeur, double hauteur)
{
	Beton_Section_Extrait		*section_en_cours;
	Beton_Section_Rectangulaire	section_nouvelle;
	
	if ((projet == NULL) || (projet->beton.sections == NULL))
		BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
	
	list_mvrear(projet->beton.sections);
	section_nouvelle.section = BETON_SECTION_RECTANGULAIRE;
	section_nouvelle.largeur = largeur;
	section_nouvelle.hauteur = hauteur;
	
	section_en_cours = (Beton_Section_Extrait *)list_rear(projet->beton.sections);
	if (section_en_cours == NULL)
		section_nouvelle.numero = 0;
	else
		section_nouvelle.numero = section_en_cours->numero+1;
	
	if (list_insert_after(projet->beton.sections, &(section_nouvelle), sizeof(section_nouvelle)) == NULL)
		BUGTEXTE(-2, gettext("Erreur d'allocation mémoire.\n"));
	
	return 0;
}

/* _1992_1_1_sections_ajout_T
 * Description : ajouter une nouvelle section en T à la liste des sections en béton
 * Paramètres : Projet *projet : la variable projet
 *            : double largeur_table : la largeur de la table
 *            : double largeur_ame : la largeur de l'âme
 *            : double hauteur_table : la hauteur de la table
 *            : double hauteur_ame : la hauteur de l'âme
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : valeur négative
 */
int _1992_1_1_sections_ajout_T(Projet *projet, double largeur_table, double largeur_ame, double hauteur_table, double hauteur_ame)
{
	Beton_Section_Extrait		*section_en_cours;
	Beton_Section_T			section_nouvelle;
	
	if ((projet == NULL) || (projet->beton.sections == NULL))
		BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
	
	list_mvrear(projet->beton.sections);
	section_nouvelle.section = BETON_SECTION_T;
	section_nouvelle.largeur_table = largeur_table;
	section_nouvelle.largeur_ame = largeur_ame;
	section_nouvelle.hauteur_table = hauteur_table;
	section_nouvelle.hauteur_ame = hauteur_ame;
	
	section_en_cours = (Beton_Section_Extrait *)list_rear(projet->beton.sections);
	if (section_en_cours == NULL)
		section_nouvelle.numero = 0;
	else
		section_nouvelle.numero = section_en_cours->numero+1;
	
	if (list_insert_after(projet->beton.sections, &(section_nouvelle), sizeof(section_nouvelle)) == NULL)
		BUGTEXTE(-2, gettext("Erreur d'allocation mémoire.\n"));
	
	return 0;
}

/* _1992_1_1_sections_ajout_carre
 * Description : ajouter une nouvelle section carrée à la liste des sections en béton
 * Paramètres : Projet *projet : la variable projet
 *            : double cote : le coté
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : valeur négative
 */
int _1992_1_1_sections_ajout_carre(Projet *projet, double cote)
{
	Beton_Section_Extrait		*section_en_cours;
	Beton_Section_Carre		section_nouvelle;
	
	if ((projet == NULL) || (projet->beton.sections == NULL))
		BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
	
	list_mvrear(projet->beton.sections);
	section_nouvelle.section = BETON_SECTION_CARRE;
	section_nouvelle.cote = cote;
	
	section_en_cours = (Beton_Section_Extrait *)list_rear(projet->beton.sections);
	if (section_en_cours == NULL)
		section_nouvelle.numero = 0;
	else
		section_nouvelle.numero = section_en_cours->numero+1;
	
	if (list_insert_after(projet->beton.sections, &(section_nouvelle), sizeof(section_nouvelle)) == NULL)
		BUGTEXTE(-2, gettext("Erreur d'allocation mémoire.\n"));
	
	return 0;
}

/* _1992_1_1_sections_ajout_circulaire
 * Description : ajouter une nouvelle section circulaire à la liste des sections en béton
 * Paramètres : Projet *projet : la variable projet
 *            : double diametre : le diamètre
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : valeur négative
 */
int _1992_1_1_sections_ajout_circulaire(Projet *projet, double diametre)
{
	Beton_Section_Extrait		*section_en_cours;
	Beton_Section_Circulaire	section_nouvelle;
	
	if ((projet == NULL) || (projet->beton.sections == NULL))
		BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
	
	list_mvrear(projet->beton.sections);
	section_nouvelle.section = BETON_SECTION_CIRCULAIRE;
	section_nouvelle.diametre = diametre;
	
	section_en_cours = (Beton_Section_Extrait *)list_rear(projet->beton.sections);
	if (section_en_cours == NULL)
		section_nouvelle.numero = 0;
	else
		section_nouvelle.numero = section_en_cours->numero+1;
	
	if (list_insert_after(projet->beton.sections, &(section_nouvelle), sizeof(section_nouvelle)) == NULL)
		BUGTEXTE(-2, gettext("Erreur d'allocation mémoire.\n"));
	
	return 0;
}

/* _1992_1_1_sections_free
 * Description : Libère l'ensemble des sections en béton
 * Paramètres : Projet *projet : la variable projet
 * Valeur renvoyée :
 *   Succès : 0 même si aucune section n'est existante
 *   Échec : valeur négative si la liste des sections n'est pas initialisée ou a déjà été libérée
 */
int _1992_1_1_sections_free(Projet *projet)
{
	if ((projet == NULL) || (projet->beton.sections == NULL))
		BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
	
	while (!list_empty(projet->beton.sections))
	{
		free(list_remove_front(projet->beton.sections));
	}
	
	free(projet->beton.sections);
	projet->beton.sections = NULL;
	
	return 0;
}
