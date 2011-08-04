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

/* _1990_coef_psi0_bat_eu
 * Description : renvoie le coefficient psi0 indiquée par la norme européenne pour les batiments
 * Paramètres : int categorie : catégorie de l'action
 * Valeur renvoyée :
 *   Succès : valeur en double du coefficient psi0
 *   Échec : valeur en double -1. si la catégorie n'existe pas
 */
double _1990_coef_psi0_bat_eu(int categorie)
{
    switch (categorie)
    {
        case 0 : { return -1.; break; }
        case 1 : { return -1.; break; }
        case 2 : { return 0.7; break; }
        case 3 : { return 0.7; break; }
        case 4 : { return 0.7; break; }
        case 5 : { return 0.7; break; }
        case 6 : { return 1.0; break; }
        case 7 : { return 0.7; break; }
        case 8 : { return 0.7; break; }
        case 9 : { return 0.0; break; }
        case 10 : { return 0.7; break; }
        case 11 : { return 0.7; break; }
        case 12 : { return 0.5; break; }
        case 13 : { return 0.6; break; }
        case 14 : { return 0.6; break; }
        case 15 : { return -1.; break; }
        case 16 : { return -1.; break; }
        default : { BUGTEXTE(-1., gettext("Catégorie inconnue.\n")); break; }
    }
}

/* _1990_coef_psi1_bat_eu
 * Description : renvoie le coefficient psi1 indiquée par la norme européenne pour les batiments
 * Paramètres : int categorie : catégorie de l'action
 * Valeur renvoyée :
 *   Succès : valeur en double du coefficient psi1
 *   Échec : valeur en double -1. si la catégorie n'existe pas
 */
double _1990_coef_psi1_bat_eu(int categorie)
{
    switch (categorie)
    {
        case 0 : { return -1.; break; }
        case 1 : { return -1.; break; }
        case 2 : { return 0.5; break; }
        case 3 : { return 0.5; break; }
        case 4 : { return 0.7; break; }
        case 5 : { return 0.7; break; }
        case 6 : { return 0.9; break; }
        case 7 : { return 0.7; break; }
        case 8 : { return 0.5; break; }
        case 9 : { return 0.0; break; }
        case 10 : { return 0.5; break; }
        case 11 : { return 0.5; break; }
        case 12 : { return 0.2; break; }
        case 13 : { return 0.2; break; }
        case 14 : { return 0.5; break; }
        case 15 : { return -1.; break; }
        case 16 : { return -1.; break; }
        default : { BUGTEXTE(-1., gettext("Catégorie inconnue.\n")); break; }
    }
}

/* _1990_coef_psi2_bat_eu
 * Description : renvoie le coefficient psi2 indiquée par la norme européenne pour les batiments
 * Paramètres : int categorie : catégorie de l'action
 * Valeur renvoyée :
 *   Succès : valeur en double du coefficient psi2
 *   Échec : valeur en double -1. si la catégorie n'existe pas
 */
double _1990_coef_psi2_bat_eu(int categorie)
{
    switch (categorie)
    {
        case 0 : { return -1.; break; }
        case 1 : { return -1.; break; }
        case 2 : { return 0.3; break; }
        case 3 : { return 0.3; break; }
        case 4 : { return 0.6; break; }
        case 5 : { return 0.6; break; }
        case 6 : { return 0.8; break; }
        case 7 : { return 0.6; break; }
        case 8 : { return 0.3; break; }
        case 9 : { return 0.0; break; }
        case 10 : { return 0.2; break; }
        case 11 : { return 0.2; break; }
        case 12 : { return 0.0; break; }
        case 13 : { return 0.0; break; }
        case 14 : { return 0.0; break; }
        case 15 : { return -1.; break; }
        case 16 : { return -1.; break; }
        default : { BUGTEXTE(-1., gettext("Catégorie inconnue.\n")); break; }
    }
}

/* _1990_coef_psi0_bat_fr
 * Description : renvoie le coefficient psi0 indiquée par la norme française pour les batiments
 * Paramètres : int categorie : catégorie de l'action
 * Valeur renvoyée :
 *   Succès : valeur en double du coefficient psi0
 *   Échec : valeur en double -1. si la catégorie n'existe pas
 */
double _1990_coef_psi0_bat_fr(int categorie)
{
    switch (categorie)
    {
        case 0 : { return -1.; break; }
        case 1 : { return -1.; break; }
        case 2 : { return 0.7; break; }
        case 3 : { return 0.7; break; }
        case 4 : { return 0.7; break; }
        case 5 : { return 0.7; break; }
        case 6 : { return 1.0; break; }
        case 7 : { return 0.7; break; }
        case 8 : { return 0.7; break; }
        case 9 : { return 0.0; break; }
        case 10 : { return 0.7; break; }
        case 11 : { return 0.7; break; }
        case 12 : { return 1.0; break; }
        case 13 : { return 0.7; break; }
        case 14 : { return 0.7; break; }
        case 15 : { return 0.7; break; }
        case 16 : { return 0.5; break; }
        case 17 : { return 0.6; break; }
        case 18 : { return 0.6; break; }
        case 19 : { return -1.; break; }
        case 20 : { return -1.; break; }
        case 21 : { return -1.; break; }
        default : { BUGTEXTE(-1., gettext("Catégorie inconnue.\n")); break; }
    }
}

/* _1990_coef_psi1_bat_fr
 * Description : renvoie le coefficient psi1 indiquée par la norme française pour les batiments
 * Paramètres : int categorie : catégorie de l'action
 * Valeur renvoyée :
 *   Succès : valeur en double du coefficient psi1
 *   Échec : valeur en double -1. si la catégorie n'existe pas
 */
double _1990_coef_psi1_bat_fr(int categorie)
{
    switch (categorie)
    {
        case 0 : { return -1.; break; }
        case 1 : { return -1.; break; }
        case 2 : { return 0.5; break; }
        case 3 : { return 0.5; break; }
        case 4 : { return 0.7; break; }
        case 5 : { return 0.7; break; }
        case 6 : { return 0.9; break; }
        case 7 : { return 0.7; break; }
        case 8 : { return 0.5; break; }
        case 9 : { return 0.0; break; }
        case 10 : { return 0.5; break; }
        case 11 : { return 0.7; break; }
        case 12 : { return 0.9; break; }
        case 13 : { return 0.5; break; }
        case 14 : { return 0.5; break; }
        case 15 : { return 0.5; break; }
        case 16 : { return 0.2; break; }
        case 17 : { return 0.2; break; }
        case 18 : { return 0.5; break; }
        case 19 : { return -1.; break; }
        case 20 : { return -1.; break; }
        case 21 : { return -1.; break; }
        default : { BUGTEXTE(-1., gettext("Catégorie inconnue.\n")); break; }
    }
}

/* _1990_coef_psi2_bat_fr
 * Description : renvoie le coefficient psi2 indiquée par la norme française pour les batiments
 * Paramètres : int categorie : catégorie de l'action
 * Valeur renvoyée :
 *   Succès : valeur en double du coefficient psi2
 *   Échec : valeur en double -1. si la catégorie n'existe pas
 */
double _1990_coef_psi2_bat_fr(int categorie)
{
    switch (categorie)
    {
        case 0 : { return -1.; break; }
        case 1 : { return -1.; break; }
        case 2 : { return 0.3; break; }
        case 3 : { return 0.3; break; }
        case 4 : { return 0.6; break; }
        case 5 : { return 0.6; break; }
        case 6 : { return 0.8; break; }
        case 7 : { return 0.6; break; }
        case 8 : { return 0.3; break; }
        case 9 : { return 0.0; break; }
        case 10 : { return 0.3; break; }
        case 11 : { return 0.6; break; }
        case 12 : { return 0.5; break; }
        case 13 : { return 0.3; break; }
        case 14 : { return 0.2; break; }
        case 15 : { return 0.2; break; }
        case 16 : { return 0.0; break; }
        case 17 : { return 0.0; break; }
        case 18 : { return 0.0; break; }
        case 19 : { return -1.; break; }
        case 20 : { return -1.; break; }
        case 21 : { return -1.; break; }
        default : { BUGTEXTE(-1., gettext("Catégorie inconnue.\n")); break; }
    }
}

/* _1990_coef_psi0_bat
 * Description : renvoie le coefficient psi0 en fonction de la norme du pays spécifié pour les batiments
 * Paramètres : int categorie : catégorie de l'action
 *            : int pays : référence du pays
 * Valeur renvoyée :
 *   Succès : cf les fonctions _1990_coef_psi0_bat_PAYS
 *   Échec : valeur en double -1. si le pays n'existe pas
 */
double _1990_coef_psi0_bat(int categorie, Type_Pays pays)
{
    switch (pays)
    {
        case PAYS_EU : { return _1990_coef_psi0_bat_eu(categorie); break; }
        case PAYS_FR : { return _1990_coef_psi0_bat_fr(categorie); break; }
        default : { BUGTEXTE(-1., gettext("Pays inconnu.\n")); break; }
    }
}

/* _1990_coef_psi1_bat
 * Description : renvoie le coefficient psi1 en fonction de la norme du pays spécifié pour les batiments
 * Paramètres : int categorie : catégorie de l'action
 *            : int pays : référence du pays
 * Valeur renvoyée :
 *   Succès : cf les fonctions _1990_coef_psi1_bat_PAYS
 *   Échec : valeur en double -1. si le pays n'existe pas
 */
double _1990_coef_psi1_bat(int categorie, Type_Pays pays)
{
    switch (pays)
    {
        case PAYS_EU : { return _1990_coef_psi1_bat_eu(categorie); break; }
        case PAYS_FR : { return _1990_coef_psi1_bat_fr(categorie); break; }
        default : { BUGTEXTE(-1., gettext("Pays inconnu.\n")); break; }
    }
}

/* _1990_coef_psi2_bat
 * Description : renvoie le coefficient psi2 en fonction de la norme du pays spécifié pour les batiments
 * Paramètres : int categorie : catégorie de l'action
 *            : int pays : référence du pays
 * Valeur renvoyée :
 *   Succès : cf les fonctions _1990_coef_psi2_bat_PAYS
 *   Échec : valeur en double -1. si le pays n'existe pas
 */
double _1990_coef_psi2_bat(int categorie, Type_Pays pays)
{
    switch (pays)
    {
        case PAYS_EU : { return _1990_coef_psi2_bat_eu(categorie); break; }
        case PAYS_FR : { return _1990_coef_psi2_bat_fr(categorie); break; }
        default : { BUGTEXTE(-1., gettext("Pays inconnu.\n")); break; }
    }
}
