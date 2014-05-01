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
#include <gmodule.h>

#include "common_erreurs.hpp"
#include "common_projet.hpp"
#include "1990_duree.hpp"


/**
 * \brief Renvoie la durée (en année) indicative de la norme européenne. Dans
 *        le cas où la durée indiquée par la norme européenne est un intervalle
 *        d'années, la durée la plus importante est renvoyée. La description de
 *        type est donnée par #_1990_duree_norme_txt_eu.
 * \param type : catégorie de durée d'utilisation de projet.
 * \return
 *   Succès : Durée indicative en année :
 *     - type 0 : 10,
 *     - type 1 : 25,
 *     - type 2 : 30,
 *     - type 3 : 50,
 *     - type 4 : 100.
 * \return
 *   Échec : 0 :
 *     - le type n'existe pas.
 * \warning Fonction interne. Il convient d'utiliser la fonction
 *          #_1990_duree_norme.
 */
static
uint16_t
_1990_duree_norme_eu (uint8_t type)
{
  switch (type)
  {
    case 0 : return 10;
    case 1 : return 25;
    case 2 : return 30;
    case 3 : return 50;
    case 4 : return 100;
    default : { FAILPARAM (type, "%u", 0) break; }
  }
}


/**
 * \brief Renvoie la durée (en année) indicative de la norme française. La
 *        description de type est donnée par #_1990_duree_norme_txt_fr.
 * \param type : catégorie de durée d'utilisation de projet.
 * \return
 *   Succès : Durée indicative en année :
 *     - type 0 : 10,
 *     - type 1 : 25,
 *     - type 2 : 25,
 *     - type 3 : 50,
 *     - type 4 : 100.
 * \return
 *   Échec : 0 :
 *     - le type n'existe pas.
 * \warning Fonction interne. Il convient d'utiliser la fonction
 *          #_1990_duree_norme.
 */
static
uint16_t
_1990_duree_norme_fr (uint8_t type)
{
  switch (type)
  {
    case 0 : return 10;
    case 1 : return 25;
    case 2 : return 25;
    case 3 : return 50;
    case 4 : return 100;
    default : { FAILPARAM (type, "%u", 0) break; }
  }
}


/**
 * \brief Renvoie la description des catérogies des durées indicatives de la
 *        norme européenne.
 * \param type : catégorie de durée d'utilisation de projet.
 * \return
 *   Succès : Texte contenant la description :
 *     - type 0 : "Structures provisoires",
 *     - type 1 : "Éléments structuraux remplaçables, par exemple poutres de
 *                 roulement",
 *     - type 2 : "Structures agricoles et similaires",
 *     - type 3 : "Structures de bâtiments et autres structures courantes",
 *     - type 4 : "Structures monumentales de bâtiments, pont et autres
 *                 ouvrages de GC".
 * \return
 *   Échec : NULL :
 *     - le type n'existe pas.
 * \warning Fonction interne. Il convient d'utiliser la fonction
 *          #_1990_duree_norme_txt.
 */
static
char *
_1990_duree_norme_txt_eu (uint8_t type)
{
  switch (type)
  {
    case 0 : return gettext ("Structures provisoires");
    case 1 : return gettext ("Éléments structuraux remplaçables, par exemple poutres de roulement, appareils d'appui");
    case 2 : return gettext ("Structures agricoles et similaires");
    case 3 : return gettext ("Structures de bâtiments et autres structures courantes");
    case 4 : return gettext ("Structures monumentales de bâtiments, pont et autres ouvrages de génie civil");
    default : { FAILPARAM (type, "%u", NULL) break; }
  }
}


/**
 * \brief Renvoie la description des catérogies des durées indicative de la
 *        norme française.
 * \param type : catégorie de durée d'utilisation de projet.
 * \return
 *   Succès : Texte contenant la description :
 *     - type 0 : "Structures provisoires",
 *     - type 1 : "Éléments structuraux remplaçables, par exemple poutres de
 *                 roulement",
 *     - type 2 : "Structures agricoles et similaires",
 *     - type 3 : "Structures de bâtiments et autres structures courantes",
 *     - type 4 : "Structures monumentales de bâtiments, pont et autres
 *                 ouvrages de GC".
 * \return
 *   Échec : NULL :
 *     - le type n'existe pas.
 * \warning Fonction interne. Il convient d'utiliser la fonction
 *          #_1990_duree_norme_txt.
 */
static
char *
_1990_duree_norme_txt_fr (uint8_t type)
{
  switch (type)
  {
    case 0 : return gettext ("Structures provisoires");
    case 1 : return gettext ("Éléments structuraux remplaçables, par exemple poutres de roulement, appareils d'appui");
    case 2 : return gettext ("Structures agricoles et similaires");
    case 3 : return gettext ("Structures de bâtiments et autres structures courantes");
    case 4 : return gettext ("Structures monumentales de bâtiments, pont et autres ouvrages de génie civil");
    default : { FAILPARAM (type, "%u", NULL) break; }
  }
}


/**
 * \brief Renvoie la durée (en année) indicative de la norme spécifiée.
 * \param type : categorie de durée d'utilisation de projet,
 * \param norme : norme souhaitée.
 * \return
 *   Succès : cf les fonctions _1990_duree_norme_PAYS.\n
 *   Échec : 0 :
 *     - Pays inconnu,
 *     - erreur dans l'une des fonctions _1990_duree_norme_PAYS.
 */
uint16_t
_1990_duree_norme (uint8_t type,
                   Norme   norme)
{
  switch (norme)
  {
    case NORME_EU : return _1990_duree_norme_eu (type);
    case NORME_FR : return _1990_duree_norme_fr (type);
    default : { FAILPARAM (norme, "%d", 0) break; }
  }
}


/**
 * \brief Renvoie la description des catérogies des durées indicatives de la
 *        norme spécifiée.
 * \param type : categorie de durée d'utilisation de projet,
 * \param norme : norme souhaitée.
 * \return
 *   Succès : cf les fonctions _1990_duree_norme_txt_PAYS.\n
 *   Échec : NULL :
 *     - Pays inconnu,
 *     - erreur dans l'une des fonctions _1990_duree_norme_txt_PAYS.
 */
char *
_1990_duree_norme_txt (uint8_t type,
                       Norme   norme)
{
  switch (norme)
  {
    case NORME_EU : return _1990_duree_norme_txt_eu (type);
    case NORME_FR : return _1990_duree_norme_txt_fr (type);
    default : { FAILPARAM (norme, "%d", NULL) break; }
  }
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
