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

#include "common_erreurs.h"
#include "common_projet.h"


int _1990_duree_projet_eu(int type)
/* Description : Renvoie la durée (en année) indicative de la norme européenne. Dans le cas où
 *                 la durée indiquée par la norme européenne est un intervalle d'année, la
 *                 durée la plus importante est renvoyée.
 *               FONCTION INTERNE. Utiliser _1990_duree_projet.
 * Paramètres : int type : categorie de durée d'utilisation de projet
 * Valeur renvoyée :
 *   Succès : Durée indicative en année :
 *            type 0 : 10
 *            type 1 : 25
 *            type 2 : 30
 *            type 3 : 50
 *            type 4 : 100
 *   Échec : 0 si le type n'existe pas
 */
{
    // Trivial
    switch (type)
    {
        case 0 : { return 10; break; }
        case 1 : { return 25; break; }
        case 2 : { return 30; break; }
        case 3 : { return 50; break; }
        case 4 : { return 100; break; }
        default : { BUGMSG(0, -1, gettext("Catégorie %d inconnue.\n"), type); break; }
    }
}


int _1990_duree_projet_fr(int type)
/* Description : Renvoie la durée (en année) indicative de la norme française
 *               FONCTION INTERNE. Utiliser _1990_duree_projet.
 * Paramètres : int type : categorie de durée d'utilisation de projet
 * Valeur renvoyée :
 *   Succès : Durée indicative en année :
 *            type 0 : 10
 *            type 1 : 25
 *            type 2 : 25
 *            type 3 : 50
 *            type 4 : 100
 *   Échec : 0 si le type n'existe pas
 */
{
    // Trivial
    switch (type)
    {
        case 0 : { return 10; break; }
        case 1 : { return 25; break; }
        case 2 : { return 25; break; }
        case 3 : { return 50; break; }
        case 4 : { return 100; break; }
        default : { BUGMSG(0, -1, gettext("Catégorie %d inconnue.\n"), type); break; }
    }
}


char* _1990_duree_projet_txt_eu(int type)
/* Description : renvoie la description des catérogies des durées indicative de la norme
 *                 européenne.
 *               FONCTION INTERNE. Utiliser _1990_duree_projet_txt
 * Paramètres : int type : categorie de durée d'utilisation de projet
 * Valeur renvoyée :
 *   Succès : Texte contenant la description :
 *            type 0 : "Structures provisoires"
 *            type 1 : "Éléments structuraux remplaçables, par exemple poutres de roulement"
 *            type 2 : "Structures agricoles et similaires"
 *            type 3 : "Structures de bâtiments et autres structures courantes"
 *            type 4 : "Structures monumentales de bâtiments, pont et autres ouvrages de GC"
 *   Échec : NULL si le type n'existe pas
 */
{
    // Trivial
    switch(type)
    {
        case 0 : { return gettext("Structures provisoires"); break; }
        case 1 : { return gettext("Éléments structuraux remplaçables, par exemple poutres de roulement, appareils d'appui"); break; }
        case 2 : { return gettext("Structures agricoles et similaires"); break; }
        case 3 : { return gettext("Structures de bâtiments et autres structures courantes"); break; }
        case 4 : { return gettext("Structures monumentales de bâtiments, pont et autres ouvrages de génie civil"); break; }
        default : { BUGMSG(0, NULL, gettext("Catégorie %d inconnue.\n"), type); break; }
    }
}


char* _1990_duree_projet_txt_fr(int type)
/* Description : renvoie la description des catérogies des durées indicative de la norme
 *                 française
 *               FONCTION INTERNE. Utiliser _1990_duree_projet_txt
 * Paramètres : int type : categorie de durée d'utilisation de projet
 * Valeur renvoyée :
 *   Succès : Texte contenant la description :
 *            type 0 : "Structures provisoires"
 *            type 1 : "Éléments structuraux remplaçables, par exemple poutres de roulement"
 *            type 2 : "Structures agricoles et similaires"
 *            type 3 : "Structures de bâtiments et autres structures courantes"
 *            type 4 : "Structures monumentales de bâtiments, pont et autres ouvrages de GC"
 *   Échec : NULL si le type n'existe pas
 */
{
    // Trivial
    switch(type)
    {
        case 0 : { return gettext("Structures provisoires"); break; }
        case 1 : { return gettext("Éléments structuraux remplaçables, par exemple poutres de roulement, appareils d'appui"); break; }
        case 2 : { return gettext("Structures agricoles et similaires"); break; }
        case 3 : { return gettext("Structures de bâtiments et autres structures courantes"); break; }
        case 4 : { return gettext("Structures monumentales de bâtiments, pont et autres ouvrages de génie civil"); break; }
        default : { BUGMSG(0, NULL, gettext("Catégorie %d inconnue.\n"), type); break; }
    }
}


int _1990_duree_projet(int type, Type_Pays pays)
/* Description : Renvoie la durée (en année) indicative de la norme du pays spécifié.
 * Paramètres : int type : categorie de durée d'utilisation de projet
 *            : Type_Pays pays : pays souhaité
 * Valeur renvoyée :
 *   Succès : cf les fonctions _1990_duree_projet_PAYS
 *   Échec : 0 en cas d'erreur.
 */
{
    // Trivial
    switch (pays)
    {
        case PAYS_EU : { return _1990_duree_projet_eu(type); break; }
        case PAYS_FR : { return _1990_duree_projet_fr(type); break; }
        default : { BUGMSG(0, -1, gettext("Pays %d inconnu.\n"), pays); break; }
    }
}


char* _1990_duree_projet_txt(int type, Type_Pays pays)
/* Description : Renvoie la description des catérogies des durées indicatives de la norme du
 *                 pays spécifié.
 * Paramètres : int type : categorie de durée d'utilisation de projet
 *            : Type_Pays pays : pays souhaité
 * Valeur renvoyée :
 *   Succès : cf les fonctions _1990_duree_projet_txt_PAYS
 *   Échec : NULL en cas d'erreur.
 */
{
    // Trivial
    switch (pays)
    {
        case PAYS_EU : { return _1990_duree_projet_txt_eu(type); break; }
        case PAYS_FR : { return _1990_duree_projet_txt_fr(type); break; }
        default : { BUGMSG(0, NULL, gettext("Pays %d inconnu.\n"), pays); break; }
    }
}
