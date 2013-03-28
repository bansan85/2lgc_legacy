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
#include <math.h>
#include <gmodule.h>

#include "common_erreurs.h"
#include "common_projet.h"


double _1990_coef_psi0_bat_eu(unsigned int type)
/* Description : renvoie le coefficient psi0 indiqué par la norme européenne pour les batiments.
 *               FONCTION INTERNE. Utiliser _1990_coef_psi0_bat.
 * Paramètres : unsigned int type : type de l'action
 * Valeur renvoyée :
 *   Succès : valeur en double du coefficient psi0.
 *            0.0 : Permanente
 *            0.0 : Précontrainte
 *            0.7 : Exploitation : Catégorie A : habitation, zones résidentielles
 *            0.7 : Exploitation : Catégorie B : bureaux
 *            0.7 : Exploitation : Catégorie C : lieux de réunion
 *            0.7 : Exploitation : Catégorie D : commerces
 *            1.0 : Exploitation : Catégorie E : stockage
 *            0.7 : Exploitation : Catégorie F : zone de trafic, véhicules inférieur à 30 kN
 *            0.7 : Exploitation : Catégorie G : zone de trafic, véhicules de 30 kN et 160 kN
 *            0.0 : Exploitation : Catégorie H : toits
 *            0.7 : Neige : Finlande, Islande, Norvège, Suède
 *            0.7 : Neige : Autres états membres CEN, altitude > 1000 m
 *            0.5 : Neige : Autres états membres CEN, altitude <= 1000 m
 *            0.6 : Vent
 *            0.6 : Température (hors incendie)
 *            0.0 : Accidentelle
 *            0.0 : Sismique
 *   Échec : NAN :
 *             le type n'existe pas.
 */
{
    // Trivial
    switch (type)
    {
        case 0 : { return 0.0; break; }
        case 1 : { return 0.0; break; }
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
        case 15 : { return 0.0; break; }
        case 16 : { return 0.0; break; }
        default : { BUGMSG(0, NAN, gettext("Type d'action %u inconnu.\n"), type); break; }
    }
}


double _1990_coef_psi1_bat_eu(unsigned int type)
/* Description : renvoie le coefficient psi1 indiqué par la norme européenne pour les batiments
 *               FONCTION INTERNE. Utiliser _1990_coef_psi1_bat.
 * Paramètres : unsigned int type : type de l'action.
 * Valeur renvoyée :
 *   Succès : valeur en double du coefficient psi1.
 *            0.0 : Permanente
 *            0.0 : Précontrainte
 *            0.5 : Exploitation : Catégorie A : habitation, zones résidentielles
 *            0.5 : Exploitation : Catégorie B : bureaux
 *            0.7 : Exploitation : Catégorie C : lieux de réunion
 *            0.7 : Exploitation : Catégorie D : commerces
 *            0.9 : Exploitation : Catégorie E : stockage
 *            0.7 : Exploitation : Catégorie F : zone de trafic, véhicules inférieur à 30 kN
 *            0.5 : Exploitation : Catégorie G : zone de trafic, véhicules de 30 kN et 160 kN
 *            0.0 : Exploitation : Catégorie H : toits
 *            0.5 : Neige : Finlande, Islande, Norvège, Suède
 *            0.5 : Neige : Autres états membres CEN, altitude > 1000 m
 *            0.2 : Neige : Autres états membres CEN, altitude <= 1000 m
 *            0.2 : Vent
 *            0.5 : Température (hors incendie)
 *            0.0 : Accidentelle
 *            0.0 : Sismique
 *   Échec : NAN :
 *             le type n'existe pas.
 */
{
    // Trivial
    switch (type)
    {
        case 0 : { return 0.0; break; }
        case 1 : { return 0.0; break; }
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
        case 15 : { return 0.0; break; }
        case 16 : { return 0.0; break; }
        default : { BUGMSG(0, NAN, gettext("Type d'action %u inconnu.\n"), type); break; }
    }
}


double _1990_coef_psi2_bat_eu(unsigned int type)
/* Description : renvoie le coefficient psi2 indiqué par la norme européenne pour les batiments
 *               FONCTION INTERNE. Utiliser _1990_coef_psi2_bat.
 * Paramètres : unsigned int type : type de l'action.
 * Valeur renvoyée :
 *   Succès : valeur en double du coefficient psi2.
 *            0.0 : Permanente
 *            0.0 : Précontrainte
 *            0.3 : Exploitation : Catégorie A : habitation, zones résidentielles
 *            0.3 : Exploitation : Catégorie B : bureaux
 *            0.6 : Exploitation : Catégorie C : lieux de réunion
 *            0.6 : Exploitation : Catégorie D : commerces
 *            0.8 : Exploitation : Catégorie E : stockage
 *            0.6 : Exploitation : Catégorie F : zone de trafic, véhicules inférieur à 30 kN
 *            0.3 : Exploitation : Catégorie G : zone de trafic, véhicules de 30 kN et 160 kN
 *            0.0 : Exploitation : Catégorie H : toits
 *            0.2 : Neige : Finlande, Islande, Norvège, Suède
 *            0.2 : Neige : Autres états membres CEN, altitude > 1000 m
 *            0.0 : Neige : Autres états membres CEN, altitude <= 1000 m
 *            0.0 : Vent
 *            0.0 : Température (hors incendie)
 *            0.0 : Accidentelle
 *            0.0 : Sismique
 *   Échec : NAN :
 *             le type n'existe pas.
 */
{
    // Trivial
    switch (type)
    {
        case 0 : { return 0.0; break; }
        case 1 : { return 0.0; break; }
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
        case 15 : { return 0.0; break; }
        case 16 : { return 0.0; break; }
        default : { BUGMSG(0, NAN, gettext("Type d'action %u inconnu.\n"), type); break; }
    }
}


double _1990_coef_psi0_bat_fr(unsigned int type)
/* Description : renvoie le coefficient psi0 indiqué par la norme française pour les batiments.
 *               FONCTION INTERNE. Utiliser _1990_coef_psi0_bat.
 * Paramètres : unsigned int type : type de l'action.
 * Valeur renvoyée :
 *   Succès : valeur en double du coefficient psi0.
 *            0.0 : Permanente
 *            0.0 : Précontrainte
 *            0.7 : Exploitation : Catégorie A : habitation, zones résidentielles
 *            0.7 : Exploitation : Catégorie B : bureaux
 *            0.7 : Exploitation : Catégorie C : lieux de réunion
 *            0.7 : Exploitation : Catégorie D : commerces
 *            1.0 : Exploitation : Catégorie E : stockage
 *            0.7 : Exploitation : Catégorie F : zone de trafic, véhicules inférieur à 30 kN
 *            0.7 : Exploitation : Catégorie G : zone de trafic, véhicules de 30 kN et 160 kN
 *            0.0 : Exploitation : Catégorie H : toits d'un bâtiment de catégorie A ou B
 *            0.7 : Exploitation : Catégorie I : toitures accessibles (locaux de type A ou B)
 *            0.7 : Exploitation : Catégorie I : toitures accessibles (locaux de type C ou D)
 *            1.0 : Exploitation : Catégorie K : Hélicoptère sur la toiture
 *            0.7 : Exploitation : Catégorie K : Hélicoptère sur la toiture, autres charges
 *            0.7 : Neige : Saint-Pierre-et-Miquelon
 *            0.7 : Neige : Altitude > 1000 m
 *            0.5 : Neige : Altitude <= 1000 m
 *            0.6 : Vent
 *            0.6 : Température (hors incendie)
 *            0.0 : Accidentelle
 *            0.0 : Sismique
 *            0.0 : Eaux souterraines
 *   Échec : NAN :
 *             le type n'existe pas.
 */
{
    // Trivial
    switch (type)
    {
        case 0 : { return 0.0; break; }
        case 1 : { return 0.0; break; }
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
        case 19 : { return 0.0; break; }
        case 20 : { return 0.0; break; }
        case 21 : { return 0.0; break; }
        default : { BUGMSG(0, NAN, gettext("Type d'action %u inconnu.\n"), type); break; }
    }
}


double _1990_coef_psi1_bat_fr(unsigned int type)
/* Description : renvoie le coefficient psi1 indiqué par la norme française pour les batiments.
 *               FONCTION INTERNE. Utiliser _1990_coef_psi1_bat.
 * Paramètres : unsigned int type : type de l'action.
 * Valeur renvoyée :
 *   Succès : valeur en double du coefficient psi1.
 *            0.0 : Permanente
 *            0.0 : Précontrainte
 *            0.5 : Exploitation : Catégorie A : habitation, zones résidentielles
 *            0.5 : Exploitation : Catégorie B : bureaux
 *            0.7 : Exploitation : Catégorie C : lieux de réunion
 *            0.7 : Exploitation : Catégorie D : commerces
 *            0.9 : Exploitation : Catégorie E : stockage
 *            0.7 : Exploitation : Catégorie F : zone de trafic, véhicules inférieur à 30 kN
 *            0.5 : Exploitation : Catégorie G : zone de trafic, véhicules de 30 kN et 160 kN
 *            0.0 : Exploitation : Catégorie H : toits d'un bâtiment de catégorie A ou B
 *            0.5 : Exploitation : Catégorie I : toitures accessibles (locaux de type A ou B)
 *            0.7 : Exploitation : Catégorie I : toitures accessibles (locaux de type C ou D)
 *            0.9 : Exploitation : Catégorie K : Hélicoptère sur la toiture
 *            0.5 : Exploitation : Catégorie K : Hélicoptère sur la toiture, autres charges
 *            0.5 : Neige : Saint-Pierre-et-Miquelon
 *            0.5 : Neige : Altitude > 1000 m
 *            0.2 : Neige : Altitude <= 1000 m
 *            0.2 : Vent
 *            0.5 : Température (hors incendie)
 *            0.0 : Accidentelle
 *            0.0 : Sismique
 *            0.0 : Eaux souterraines
 *   Échec : NAN :
 *             le type n'existe pas.
 */
{
    // Trivial
    switch (type)
    {
        case 0 : { return 0.0; break; }
        case 1 : { return 0.0; break; }
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
        case 19 : { return 0.0; break; }
        case 20 : { return 0.0; break; }
        case 21 : { return 0.0; break; }
        default : { BUGMSG(0, NAN, gettext("Type d'action %u inconnu.\n"), type); break; }
    }
}


double _1990_coef_psi2_bat_fr(unsigned int type)
/* Description : renvoie le coefficient psi2 indiqué par la norme française pour les batiments.
 *               FONCTION INTERNE. Utiliser _1990_coef_psi2_bat.
 * Paramètres : unsigned int type : type de l'action.
 * Valeur renvoyée :
 *   Succès : valeur en double du coefficient psi2
 *            0.0 : Permanente
 *            0.0 : Précontrainte
 *            0.3 : Exploitation : Catégorie A : habitation, zones résidentielles
 *            0.3 : Exploitation : Catégorie B : bureaux
 *            0.6 : Exploitation : Catégorie C : lieux de réunion
 *            0.6 : Exploitation : Catégorie D : commerces
 *            0.8 : Exploitation : Catégorie E : stockage
 *            0.6 : Exploitation : Catégorie F : zone de trafic, véhicules inférieur à 30 kN
 *            0.3 : Exploitation : Catégorie G : zone de trafic, véhicules de 30 kN et 160 kN
 *            0.0 : Exploitation : Catégorie H : toits d'un bâtiment de catégorie A ou B
 *            0.3 : Exploitation : Catégorie I : toitures accessibles (locaux de type A ou B)
 *            0.6 : Exploitation : Catégorie I : toitures accessibles (locaux de type C ou D)
 *            0.5 : Exploitation : Catégorie K : Hélicoptère sur la toiture
 *            0.3 : Exploitation : Catégorie K : Hélicoptère sur la toiture, autres charges
 *            0.2 : Neige : Saint-Pierre-et-Miquelon
 *            0.2 : Neige : Altitude > 1000 m
 *            0.0 : Neige : Altitude <= 1000 m
 *            0.0 : Vent
 *            0.0 : Température (hors incendie)
 *            0.0 : Accidentelle
 *            0.0 : Sismique
 *            0.0 : Eaux souterraines
 *   Échec : NAN :
 *             le type n'existe pas.
 */
{
    // Trivial
    switch (type)
    {
        case 0 : { return 0.0; break; }
        case 1 : { return 0.0; break; }
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
        case 19 : { return 0.0; break; }
        case 20 : { return 0.0; break; }
        case 21 : { return 0.0; break; }
        default : { BUGMSG(0, NAN, gettext("Type d'action %u inconnu.\n"), type); break; }
    }
}


double _1990_coef_psi0_bat(unsigned int type, Type_Pays pays)
/* Description : renvoie le coefficient psi0 en fonction de la norme du pays spécifié pour les
 *               batiments.
 * Paramètres : unsigned int type : type de l'action,
 *            : Type_Pays pays : référence du pays.
 * Valeur renvoyée :
 *   Succès : cf les fonctions _1990_coef_psi0_bat_PAYS.
 *   Échec : NAN :
 *             le pays ou le type n'existe pas.
 */
{
    // Trivial
    switch (pays)
    {
        case PAYS_EU : { return _1990_coef_psi0_bat_eu(type); break; }
        case PAYS_FR : { return _1990_coef_psi0_bat_fr(type); break; }
        default : { BUGMSG(0, NAN, gettext("Pays %d inconnu.\n"), pays); break; }
    }
}


double _1990_coef_psi1_bat(unsigned int type, Type_Pays pays)
/* Description : renvoie le coefficient psi1 en fonction de la norme du pays spécifié pour les
 *               batiments.
 * Paramètres : unsigned int type : type de l'action,
 *            : Type_Pays pays : référence du pays.
 * Valeur renvoyée :
 *   Succès : cf les fonctions _1990_coef_psi1_bat_PAYS.
 *   Échec : NAN :
 *             le pays ou le type n'existe pas.
 */
{
    // Trivial
    switch (pays)
    {
        case PAYS_EU : { return _1990_coef_psi1_bat_eu(type); break; }
        case PAYS_FR : { return _1990_coef_psi1_bat_fr(type); break; }
        default : { BUGMSG(0, NAN, gettext("Pays %d inconnu.\n"), pays); break; }
    }
}


double _1990_coef_psi2_bat(unsigned int type, Type_Pays pays)
/* Description : renvoie le coefficient psi2 en fonction de la norme du pays spécifié pour les
 *               batiments.
 * Paramètres : unsigned int type : type de l'action,
 *            : Type_Pays pays : référence du pays.
 * Valeur renvoyée :
 *   Succès : cf les fonctions _1990_coef_psi2_bat_PAYS.
 *   Échec : NAN :
 *             le pays ou le type n'existe pas.
 */
{
    // Trivial
    switch (pays)
    {
        case PAYS_EU : { return _1990_coef_psi2_bat_eu(type); break; }
        case PAYS_FR : { return _1990_coef_psi2_bat_fr(type); break; }
        default : { BUGMSG(0, NAN, gettext("Pays %d inconnu.\n"), pays); break; }
    }
}
