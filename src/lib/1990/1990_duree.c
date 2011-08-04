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

#include <libintl.h>
#include <locale.h>

#include "config.h"
#include "common_erreurs.h"
#include "common_projet.h"


/* _1990_duree_projet_eu
 * Description : Renvoie la durée (en année) indicative de la norme européenne
 *             : Dans le cas où la durée indiquée par la norme européenne est un
 *               intervalle d'année, la durée la plus importante est renvoyée.
 * Paramètres : int type : categorie de durée d'utilisation de projet
 * Valeur renvoyée :
 *   Succès : Durée indicative en année
 *   Échec : valeur négative en cas d'erreur
 */
int _1990_duree_projet_eu(int type)
{
    switch (type)
    {
        case 0 : { return 10; break; }
        case 1 : { return 25; break; }
        case 2 : { return 30; break; }
        case 3 : { return 50; break; }
        case 4 : { return 100; break; }
        default : { BUGTEXTE(-1, gettext("Catégorie inconnue.\n")); break; }
    }
}

/* _1990_duree_projet_fr
 * Description : Renvoie la durée (en année) indicative de la norme française
 * Paramètres : int type : categorie de durée d'utilisation de projet
 * Valeur renvoyée :
 *   Succès : Durée indicative en année
 *   Échec : valeur négative en cas d'erreur
 */
int _1990_duree_projet_fr(int type)
{
    switch (type)
    {
        case 0 : { return 10; break; }
        case 1 : { return 25; break; }
        case 2 : { return 25; break; }
        case 3 : { return 50; break; }
        case 4 : { return 100; break; }
        default : { BUGTEXTE(-1, gettext("Catégorie inconnue.\n")); break; }
    }
}

/* _1990_duree_projet_txt_eu
 * Description : renvoie la description des catérogies des durées indicative de la norme européenne
 * Paramètres : int type : categorie de durée d'utilisation de projet
 * Valeur renvoyée :
 *   Succès : Texte contenant la description
 *   Échec : NULL en cas d'erreur
 */
char *_1990_duree_projet_txt_eu(int type)
{
    switch(type)
    {
        case 0 : { return gettext("Structures provisoires"); break; }
        case 1 : { return gettext("Éléments structuraux remplaçables, par exemple poutres de roulement, appareils d'appui"); break; }
        case 2 : { return gettext("Structures agricoles et similaires"); break; }
        case 3 : { return gettext("Structures de bâtiments et autres structures courantes"); break; }
        case 4 : { return gettext("Structures monumentales de bâtiments, pont et autres ouvrages de génie civil"); break; }
        default : { BUGTEXTE(NULL, gettext("Catégorie inconnue.\n")); break; }
    }
}

/* _1990_duree_projet_txt_fr
 * Description : renvoie la description des catérogies des durées indicative de la norme française
 * Paramètres : int type : categorie de durée d'utilisation de projet
 * Valeur renvoyée :
 *   Succès : Texte contenant la description
 *   Échec : NULL en cas d'erreur
 */
char *_1990_duree_projet_txt_fr(int type)
{
    switch(type)
    {
        case 0 : { return gettext("Structures provisoires"); break; }
        case 1 : { return gettext("Éléments structuraux remplaçables, par exemple poutres de roulement, appareils d'appui"); break; }
        case 2 : { return gettext("Structures agricoles et similaires"); break; }
        case 3 : { return gettext("Structures de bâtiments et autres structures courantes"); break; }
        case 4 : { return gettext("Structures monumentales de bâtiments, pont et autres ouvrages de génie civil"); break; }
        default : { BUGTEXTE(NULL, gettext("Catégorie inconnue.\n")); break; }
    }
}

/* _1990_duree_projet
 * Description : Renvoie la durée (en année) indicative de la norme du pays spécifié
 * Paramètres : int type : categorie de durée d'utilisation de projet
 *            : Type_Pays pays : pays souhaité
 * Valeur renvoyée :
 *   Succès : Durée indicative en année
 *   Échec : valeur négative en cas d'erreur
 */
int _1990_duree_projet(int type, Type_Pays pays)
{
    switch (pays)
    {
        case PAYS_EU : { return _1990_duree_projet_eu(type); break; }
        case PAYS_FR : { return _1990_duree_projet_fr(type); break; }
        default : { BUGTEXTE(-1, gettext("Pays inconnu.\n")); break; }
    }
}

/* _1990_duree_projet_txt
 * Description : renvoie la description des catérogies des durées indicative de la norme du pays spécifié
 * Paramètres : int type : categorie de durée d'utilisation de projet
 *            : Type_Pays pays : pays souhaité
 * Valeur renvoyée :
 *   Succès : Texte contenant la description
 *   Échec : NULL en cas d'erreur
 */
char *_1990_duree_projet_txt(int type, Type_Pays pays)
{
    switch (pays)
    {
        case PAYS_EU : { return _1990_duree_projet_txt_eu(type); break; }
        case PAYS_FR : { return _1990_duree_projet_txt_fr(type); break; }
        default : { BUGTEXTE(NULL, gettext("Pays inconnu.\n")); break; }
    }
}
