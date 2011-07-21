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
#include <math.h>
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
	Beton_Section_Rectangulaire	*section_en_cours; // Ici, on ne se préoccupe pas de la nature de la section par on utilisera seulement le champ section qui est par définition toujours le premier
	Beton_Section_Rectangulaire	section_nouvelle;
	double				a, b;
	
	if ((projet == NULL) || (projet->beton.sections == NULL))
		BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
	
	list_mvrear(projet->beton.sections);
	section_nouvelle.caracteristiques = (Beton_Section_Caracteristiques*)malloc(sizeof(Beton_Section_Caracteristiques));
	if (section_nouvelle.caracteristiques == NULL)
		BUGTEXTE(-2, gettext("Erreur d'allocation mémoire.\n"));
	section_nouvelle.caracteristiques->type = BETON_SECTION_RECTANGULAIRE;
	section_nouvelle.largeur = largeur;
	section_nouvelle.hauteur = hauteur;
	section_nouvelle.caracteristiques->a = largeur*hauteur;
	section_nouvelle.caracteristiques->cdgh = hauteur/2.;
	section_nouvelle.caracteristiques->cdgb = hauteur/2.;
	section_nouvelle.caracteristiques->cdgd = largeur/2.;
	section_nouvelle.caracteristiques->cdgg = largeur/2.;
	section_nouvelle.caracteristiques->iy = largeur*hauteur*hauteur*hauteur/12.;
	section_nouvelle.caracteristiques->iz = hauteur*largeur*largeur*largeur/12.;
	if (largeur > hauteur)
		{ a = largeur; b = hauteur; }
	else
		{ a = hauteur; b = largeur; }
	section_nouvelle.caracteristiques->j = a*b*b*b/16.*(16./3.-3.364*b/a*(1.-b*b*b*b/(12.*a*a*a*a)));
	
	section_en_cours = (Beton_Section_Rectangulaire*)list_rear(projet->beton.sections);
	if (section_en_cours == NULL)
		section_nouvelle.caracteristiques->numero = 0;
	else
		section_nouvelle.caracteristiques->numero = section_en_cours->caracteristiques->numero+1;
	
	if (list_insert_after(projet->beton.sections, &(section_nouvelle), sizeof(section_nouvelle)) == NULL)
		BUGTEXTE(-3, gettext("Erreur d'allocation mémoire.\n"));
	
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
	Beton_Section_T			*section_en_cours;
	Beton_Section_T			section_nouvelle;
	double				a, b, aa, bb;
	
	if ((projet == NULL) || (projet->beton.sections == NULL))
		BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
	
	list_mvrear(projet->beton.sections);
	section_nouvelle.caracteristiques = (Beton_Section_Caracteristiques*)malloc(sizeof(Beton_Section_Caracteristiques));
	if (section_nouvelle.caracteristiques == NULL)
		BUGTEXTE(-2, gettext("Erreur d'allocation mémoire.\n"));
	section_nouvelle.caracteristiques->type = BETON_SECTION_T;
	section_nouvelle.largeur_table = largeur_table;
	section_nouvelle.largeur_ame = largeur_ame;
	section_nouvelle.hauteur_table = hauteur_table;
	section_nouvelle.hauteur_ame = hauteur_ame;
	section_nouvelle.caracteristiques->a = largeur_table*hauteur_table+largeur_ame*hauteur_ame;
	section_nouvelle.caracteristiques->cdgh = (largeur_table*hauteur_table*hauteur_table/2+largeur_ame*hauteur_ame*(hauteur_table+hauteur_ame/2))/section_nouvelle.caracteristiques->a;
	section_nouvelle.caracteristiques->cdgb = (hauteur_table+hauteur_ame)-section_nouvelle.caracteristiques->cdgh;
	section_nouvelle.caracteristiques->cdgd = largeur_table/2;
	section_nouvelle.caracteristiques->cdgg = largeur_table/2;
	section_nouvelle.caracteristiques->iy = largeur_table*hauteur_table*hauteur_table*hauteur_table/12+largeur_ame*hauteur_ame*hauteur_ame*hauteur_ame/12+largeur_table*hauteur_table*(hauteur_table/2-section_nouvelle.caracteristiques->cdgh)*(hauteur_table/2-section_nouvelle.caracteristiques->cdgh)+largeur_ame*hauteur_ame*(hauteur_ame/2-section_nouvelle.caracteristiques->cdgb)*(hauteur_ame/2-section_nouvelle.caracteristiques->cdgb);
	section_nouvelle.caracteristiques->iz = hauteur_table*largeur_table*largeur_table*largeur_table/12+hauteur_ame*largeur_ame*largeur_ame*largeur_ame/12;
	
	if (largeur_table > hauteur_table)
		{ a = largeur_table; b = hauteur_table; }
	else
		{ a = hauteur_table; b = largeur_table; }
	if (largeur_ame > hauteur_ame)
		{ aa = largeur_ame; bb = hauteur_ame; }
	else
		{ aa = hauteur_ame; bb = largeur_ame; }
	
	section_nouvelle.caracteristiques->j = a*b*b*b/16*(16/3-3.364*b/a*(1-b*b*b*b/(12*a*a*a*a)))+aa*bb*bb*bb/16*(16/3-3.364*bb/aa*(1-bb*bb*bb*bb/(12*aa*aa*aa*aa)));
	
	section_en_cours = (Beton_Section_T*)list_rear(projet->beton.sections);
	if (section_en_cours == NULL)
		section_nouvelle.caracteristiques->numero = 0;
	else
		section_nouvelle.caracteristiques->numero = section_en_cours->caracteristiques->numero+1;
	
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
	Beton_Section_Carre		*section_en_cours;
	Beton_Section_Carre		section_nouvelle;
	
	if ((projet == NULL) || (projet->beton.sections == NULL))
		BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
	
	list_mvrear(projet->beton.sections);
	section_nouvelle.caracteristiques = (Beton_Section_Caracteristiques*)malloc(sizeof(Beton_Section_Caracteristiques));
	if (section_nouvelle.caracteristiques == NULL)
		BUGTEXTE(-2, gettext("Erreur d'allocation mémoire.\n"));
	section_nouvelle.caracteristiques->type = BETON_SECTION_CARRE;
	section_nouvelle.cote = cote;
	section_nouvelle.caracteristiques->a = cote*cote;
	section_nouvelle.caracteristiques->cdgh = cote/2.;
	section_nouvelle.caracteristiques->cdgb = cote/2.;
	section_nouvelle.caracteristiques->cdgd = cote/2.;
	section_nouvelle.caracteristiques->cdgg = cote/2.;
	section_nouvelle.caracteristiques->iy = cote*cote*cote*cote/12.;
	section_nouvelle.caracteristiques->iz = section_nouvelle.caracteristiques->iy;
	section_nouvelle.caracteristiques->j = cote*cote*cote*cote/16.*(16./3.-3.364*(1.-1./12.));
	
	section_en_cours = (Beton_Section_Carre*)list_rear(projet->beton.sections);
	if (section_en_cours == NULL)
		section_nouvelle.caracteristiques->numero = 0;
	else
		section_nouvelle.caracteristiques->numero = section_en_cours->caracteristiques->numero+1;
	
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
	Beton_Section_Circulaire	*section_en_cours;
	Beton_Section_Circulaire	section_nouvelle;
	
	if ((projet == NULL) || (projet->beton.sections == NULL))
		BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
	
	list_mvrear(projet->beton.sections);
	section_nouvelle.caracteristiques = (Beton_Section_Caracteristiques*)malloc(sizeof(Beton_Section_Caracteristiques));
	if (section_nouvelle.caracteristiques == NULL)
		BUGTEXTE(-2, gettext("Erreur d'allocation mémoire.\n"));
	section_nouvelle.caracteristiques->type = BETON_SECTION_CIRCULAIRE;
	section_nouvelle.diametre = diametre;
	section_nouvelle.caracteristiques->a = M_PI*diametre*diametre/4.;
	section_nouvelle.caracteristiques->cdgh = diametre/2.;
	section_nouvelle.caracteristiques->cdgb = diametre/2.;
	section_nouvelle.caracteristiques->cdgd = diametre/2.;
	section_nouvelle.caracteristiques->cdgg = diametre/2.;
	section_nouvelle.caracteristiques->iy = M_PI*diametre*diametre*diametre*diametre/64.;
	section_nouvelle.caracteristiques->iz = section_nouvelle.caracteristiques->iy;
	section_nouvelle.caracteristiques->j = M_PI*diametre*diametre*diametre*diametre/32.;
	
	section_en_cours = (Beton_Section_Circulaire*)list_rear(projet->beton.sections);
	if (section_en_cours == NULL)
		section_nouvelle.caracteristiques->numero = 0;
	else
		section_nouvelle.caracteristiques->numero = section_en_cours->caracteristiques->numero+1;
	
	if (list_insert_after(projet->beton.sections, &(section_nouvelle), sizeof(section_nouvelle)) == NULL)
		BUGTEXTE(-2, gettext("Erreur d'allocation mémoire.\n"));
	
	return 0;
}


/* _1992_1_1_sections_cherche_numero
 * Description : Positionne dans la liste des sections en béton l'élément courant au numéro souhaité
 * Paramètres : Projet *projet : la variable projet
 *            : unsigned int numero : le numéro de la section
 * Valeur renvoyée :
 *   Succès : pointeur vers la section
 *   Échec : NULL
 */
void* _1992_1_1_sections_cherche_numero(Projet *projet, unsigned int numero)
{
	if ((projet == NULL) || (projet->beton.sections == NULL) || (list_size(projet->beton.sections) == 0))
		BUGTEXTE(NULL, gettext("Paramètres invalides.\n"));
	
	list_mvfront(projet->beton.sections);
	do
	{
		Beton_Section_Circulaire	*section = list_curr(projet->beton.sections);
		
		if (section->caracteristiques->numero == numero)
			return section;
	}
	while (list_mvnext(projet->beton.sections) != NULL);
	
	BUGTEXTE(NULL, gettext("Section en béton n°%d introuvable.\n"), numero);
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
		
		Beton_Section_Circulaire	*section;
		section = list_remove_front(projet->beton.sections);
		free(section->caracteristiques);
		free(section);
	}
	
	free(projet->beton.sections);
	projet->beton.sections = NULL;
	
	return 0;
}
