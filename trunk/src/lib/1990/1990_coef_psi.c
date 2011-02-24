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

// _1990_coef_psi0_bat_exp_eu : renvoie le coefficient psi0 indiquée de la norme européenne
// pour les bâtiment et les charges d'exploitation
// Si la valeur de type ne correspond pas à une catégorie, la fonction renvoie -1.
// type : categorie de durée d'utilisation de projet
double _1990_coef_psi0_bat_exp_eu(int type)
{
	switch (type)
	{
		case 1 : { return 0.7; break; }
		case 2 : { return 0.7; break; }
		case 3 : { return 0.7; break; }
		case 4 : { return 0.7; break; }
		case 5 : { return 1.0; break; }
		case 6 : { return 0.7; break; }
		case 7 : { return 0.7; break; }
		case 8 : { return 0.0; break; }
		default : { return -1.; break; }
	}
}
double _1990_coef_psi1_bat_exp_eu(int type)
{
	switch (type)
	{
		case 1 : { return 0.5; break; }
		case 2 : { return 0.5; break; }
		case 3 : { return 0.7; break; }
		case 4 : { return 0.7; break; }
		case 5 : { return 0.9; break; }
		case 6 : { return 0.7; break; }
		case 7 : { return 0.5; break; }
		case 8 : { return 0.0; break; }
		default : { return -1.; break; }
	}
}
double _1990_coef_psi2_bat_exp_eu(int type)
{
	switch (type)
	{
		case 1 : { return 0.3; break; }
		case 2 : { return 0.3; break; }
		case 3 : { return 0.6; break; }
		case 4 : { return 0.6; break; }
		case 5 : { return 0.8; break; }
		case 6 : { return 0.6; break; }
		case 7 : { return 0.3; break; }
		case 8 : { return 0.0; break; }
		default : { return -1.; break; }
	}
}

// _1990_coef_psi0_bat_exp_fr : renvoie le coefficient psi0 indiquée de la norme française
// pour les bâtiment et les charges d'exploitation
double _1990_coef_psi0_bat_exp_fr(int type)
{
	switch (type)
	{
		case 1 : { return 0.7; break; }
		case 2 : { return 0.7; break; }
		case 3 : { return 0.7; break; }
		case 4 : { return 0.7; break; }
		case 5 : { return 1.0; break; }
		case 6 : { return 0.7; break; }
		case 7 : { return 0.7; break; }
		case 8 : { return 0.0; break; }
		case 9 : { return 0.7; break; }
		case 10 : { return 1.0; break; }
		case 11 : { return 0.7; break; }
		default : { return -1.; break; }
	}
}
double _1990_coef_psi1_bat_exp_fr(int type)
{
	switch (type)
	{
		case 1 : { return 0.5; break; }
		case 2 : { return 0.5; break; }
		case 3 : { return 0.7; break; }
		case 4 : { return 0.7; break; }
		case 5 : { return 0.9; break; }
		case 6 : { return 0.7; break; }
		case 7 : { return 0.5; break; }
		case 8 : { return 0.0; break; }
		case 9 : { return 0.7; break; }
		case 10 : { return 0.9; break; }
		case 11 : { return 0.5; break; }
		default : { return -1.; break; }
	}
}
double _1990_coef_psi2_bat_exp_fr(int type)
{
	switch (type)
	{
		case 1 : { return 0.3; break; }
		case 2 : { return 0.3; break; }
		case 3 : { return 0.6; break; }
		case 4 : { return 0.6; break; }
		case 5 : { return 0.8; break; }
		case 6 : { return 0.6; break; }
		case 7 : { return 0.3; break; }
		case 8 : { return 0.0; break; }
		case 9 : { return 0.6; break; }
		case 10 : { return 0.5; break; }
		case 11 : { return 0.3; break; }
		default : { return -1.; break; }
	}
}

// _1990_coef_psi_bat_exp_txt_eu : renvoie le descriptif de la catégorie du coefficient psi0
// indiquée de la norme européenne pour les bâtiment et les charges d'exploitation
// Si la valeur de type ne correspond pas à une catégorie, la fonction renvoie NULL.
// type : categorie de durée d'utilisation de projet
char* _1990_coef_psi_bat_exp_txt_eu(int type)
{
	switch(type)
	{
		case 1 : { return gettext("Catégorie A : habitation, zones résidentielles"); break; }
		case 2 : { return gettext("Catégorie B : bureaux"); break; }
		case 3 : { return gettext("Catégorie C : lieux de réunion"); break; }
		case 4 : { return gettext("Catégorie D : commerces"); break; }
		case 5 : { return gettext("Catégorie E : stockage"); break; }
		case 6 : { return gettext("Catégorie F : zone de trafic, véhicules de poids inférieur à 30 kN"); break; }
		case 7 : { return gettext("Catégorie G : zone de trafic, véhicules de poids entre 30 kN et 160 kN"); break; }
		case 8 : { return gettext("Catégorie H : toits"); break; }
		default : { return NULL; break; }
	}
}

// _1990_coef_psi_bat_exp_txt_fr : renvoie le descriptif de la catégorie du coefficient psi0
// indiquée de la norme française pour les bâtiment et les charges d'exploitation
char* _1990_coef_psi_bat_exp_txt_fr(int type)
{
	switch(type)
	{
		case 1 : { return gettext("Catégorie A : habitation, zones résidentielles"); break; }
		case 2 : { return gettext("Catégorie B : bureaux"); break; }
		case 3 : { return gettext("Catégorie C : lieux de réunion"); break; }
		case 4 : { return gettext("Catégorie D : commerces"); break; }
		case 5 : { return gettext("Catégorie E : stockage"); break; }
		case 6 : { return gettext("Catégorie F : zone de trafic, véhicules de poids inférieur à 30 kN"); break; }
		case 7 : { return gettext("Catégorie G : zone de trafic, véhicules de poids entre 30 kN et 160 kN"); break; }
		case 8 : { return gettext("Catégorie H : toits"); break; }
		case 9 : { return gettext("Catégorie I  : toitures accessibles avec locaux des catégories A à D"); break; }
		case 10 : { return gettext("Catégorie K : Hélicoptère sur la toiture"); break; }
		case 11 : { return gettext("Catégorie K : Hélicoptère sur la toiture, autres charges (fret, personnel, accessoires ou équipements divers)"); break; }
		default : { return NULL; break; }
	}
}

// _1990_coef_psi0_bat_neige_eu : renvoie le coefficient psi0 indiquée de la norme européenne
// pour les bâtiment et les charges de neige
// Si la valeur de type ne correspond pas à une catégorie, la fonction renvoie -1.
// type : categorie de durée d'utilisation de projet
double _1990_coef_psi0_bat_neige_eu(int type)
{
	switch (type)
	{
		case 1 : { return 0.7; break; }
		case 2 : { return 0.7; break; }
		case 3 : { return 0.5; break; }
		default : { return -1.; break; }
	}
}
double _1990_coef_psi1_bat_neige_eu(int type)
{
	switch (type)
	{
		case 1 : { return 0.5; break; }
		case 2 : { return 0.5; break; }
		case 3 : { return 0.2; break; }
		default : { return -1.; break; }
	}
}
double _1990_coef_psi2_bat_neige_eu(int type)
{
	switch (type)
	{
		case 1 : { return 0.2; break; }
		case 2 : { return 0.2; break; }
		case 3 : { return 0.0; break; }
		default : { return -1.; break; }
	}
}

// _1990_coef_psi0_bat_exp_fr : renvoie le coefficient psi0 indiquée de la norme française
// pour les bâtiment et les charges de neige
double _1990_coef_psi0_bat_neige_fr(int type)
{
	switch (type)
	{
		case 1 : { return 0.7; break; }
		case 2 : { return 0.7; break; }
		case 3 : { return 0.5; break; }
		case 4 : { return 0.7; break; }
		default : { return -1.; break; }
	}
}
double _1990_coef_psi1_bat_neige_fr(int type)
{
	switch (type)
	{
		case 1 : { return 0.5; break; }
		case 2 : { return 0.5; break; }
		case 3 : { return 0.2; break; }
		case 4 : { return 0.5; break; }
		default : { return -1.; break; }
	}
}
double _1990_coef_psi2_bat_neige_fr(int type)
{
	switch (type)
	{
		case 1 : { return 0.2; break; }
		case 2 : { return 0.2; break; }
		case 3 : { return 0.0; break; }
		case 4 : { return 0.2; break; }
		default : { return -1.; break; }
	}
}

// _1990_coef_psi_bat_neige_txt_eu : renvoie le descriptif de la catégorie du coefficient psi0
// indiquée de la norme européenne pour les bâtiment et les charges de neige
// Si la valeur de type ne correspond pas à une catégorie, la fonction renvoie NULL.
// type : categorie de durée d'utilisation de projet
char* _1990_coef_psi_bat_neige_txt_eu(int type)
{
	switch (type)
	{
		case 1 : { return gettext("Finlande, Islande, Norvège, Suède"); break; }
		case 2 : { return gettext("Autres états membres CEN, altitude > 1000 m"); break; }
		case 3 : { return gettext("Autres états membres CEN, altitude <= 1000 m"); break; }
		default : { return NULL; break; }
	}
}
// _1990_coef_psi_bat_neige_txt_eu : renvoie le descriptif de la catégorie du coefficient psi0
// indiquée de la norme française pour les bâtiment et les charges de neige
char* _1990_coef_psi_bat_neige_txt_fr(int type)
{
	switch (type)
	{
		case 1 : { return gettext("Finlande, Islande, Norvège, Suède"); break; }
		case 2 : { return gettext("Autres états membres CEN, altitude > 1000 m"); break; }
		case 3 : { return gettext("Autres états membres CEN, altitude <= 1000 m"); break; }
		case 4 : { return gettext("Saint-Pierre-et-Miquelon"); break; }
		default : { return NULL; break; }
	}
}

// _1990_coef_psi0_bat_vent_eu : renvoie le coefficient psi0 indiquée de la norme européenne
// pour les bâtiment et les charges de vent
// Si la valeur de type ne correspond pas à une catégorie, la fonction renvoie -1.
// type : categorie de durée d'utilisation de projet
double _1990_coef_psi0_bat_vent_eu(int type)
{
	switch (type)
	{
		case 1 : { return 0.6; break; }
		default : { return -1.; break; }
	}
}
double _1990_coef_psi1_bat_vent_eu(int type)
{
	switch (type)
	{
		case 1 : { return 0.2; break; }
		default : { return -1.; break; }
	}
}
double _1990_coef_psi2_bat_vent_eu(int type)
{
	switch (type)
	{
		case 1 : { return 0.0; break; }
		default : { return -1.; break; }
	}
}
// _1990_coef_psi0_bat_vent_fr : renvoie le coefficient psi0 indiquée de la norme française
// pour les bâtiment et les charges de vent
double _1990_coef_psi0_bat_vent_fr(int type)
{
	switch (type)
	{
		case 1 : { return 0.6; break; }
		default : { return -1.; break; }
	}
}
double _1990_coef_psi1_bat_vent_fr(int type)
{
	switch (type)
	{
		case 1 : { return 0.2; break; }
		default : { return -1.; break; }
	}
}
double _1990_coef_psi2_bat_vent_fr(int type)
{
	switch (type)
	{
		case 1 : { return 0.0; break; }
		default : { return -1.; break; }
	}
}

// _1990_coef_psi_bat_vent_txt_eu : renvoie le descriptif de la catégorie du coefficient psi0
// indiquée de la norme européenne pour les bâtiment et les charges de vent
// Si la valeur de type ne correspond pas à une catégorie, la fonction renvoie NULL.
// type : categorie de durée d'utilisation de projet
char* _1990_coef_psi_bat_vent_txt_eu(int type)
{
	switch (type)
	{
		case 1 : { return gettext("Charges dues au vent sur les bâtiments"); break; }
		default : { return NULL; break; }
	}
}
// _1990_coef_psi_bat_vent_txt_eu : renvoie le descriptif de la catégorie du coefficient psi0
// indiquée de la norme française pour les bâtiment et les charges de vent
char* _1990_coef_psi_bat_vent_txt_fr(int type)
{
	switch (type)
	{
		case 1 : { return gettext("Charges dues au vent sur les bâtiments"); break; }
		default : { return NULL; break; }
	}
}

// _1990_coef_psi0_bat_temp_eu : renvoie le coefficient psi0 indiquée de la norme européenne
// pour les bâtiment et les charges de température
// Si la valeur de type ne correspond pas à une catégorie, la fonction renvoie -1.
// type : categorie de durée d'utilisation de projet
double _1990_coef_psi0_bat_temp_eu(int type)
{
	switch (type)
	{
		case 1 : { return 0.6; break; }
		default : { return -1.; break; }
	}
}
double _1990_coef_psi1_bat_temp_eu(int type)
{
	switch (type)
	{
		case 1 : { return 0.5; break; }
		default : { return -1.; break; }
	}
}
double _1990_coef_psi2_bat_temp_eu(int type)
{
	switch (type)
	{
		case 1 : { return 0.0; break; }
		default : { return -1.; break; }
	}
}
// _1990_coef_psi0_bat_temp_fr : renvoie le coefficient psi0 indiquée de la norme française
// pour les bâtiment et les charges de température
double _1990_coef_psi0_bat_temp_fr(int type)
{
	switch (type)
	{
		case 1 : { return 0.6; break; }
		default : { return -1; break; }
	}
}
double _1990_coef_psi1_bat_temp_fr(int type)
{
	switch (type)
	{
		case 1 : { return 0.5; break; }
		default : { return -1; break; }
	}
}
double _1990_coef_psi2_bat_temp_fr(int type)
{
	switch (type)
	{
		case 1 : { return 0.0; break; }
		default : { return -1; break; }
	}
}

// _1990_coef_psi_bat_temp_txt_eu : renvoie le descriptif de la catégorie du coefficient psi0
// indiquée de la norme européenne pour les bâtiment et les charges de température
// Si la valeur de type ne correspond pas à une catégorie, la fonction renvoie NULL.
// type : categorie de durée d'utilisation de projet
char* _1990_coef_psi_bat_temp_txt_eu(int type)
{
	switch (type)
	{
		case 1 : { return gettext("Température (hors incendie) dans les bâtiments"); break; }
		default : { return NULL; break; }
	}
}

// _1990_coef_psi_bat_temp_txt_eu : renvoie le descriptif de la catégorie du coefficient psi0
// indiquée de la norme française pour les bâtiment et les charges de température
char* _1990_coef_psi_bat_temp_txt_fr(int type)
{
	switch (type)
	{
		case 1 : { return gettext("Température (hors incendie) dans les bâtiments"); break; }
		default : { return NULL; break; }
	}
}
