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

// _1990_coef_psi0_bat_exp_eu : renvoie le coefficient psi0 indiquée de la norme européenne
// pour les bâtiment et les charges d'exploitation
// Si la valeur de type ne correspond pas à une catégorie, la fonction renvoie -1.
// type : categorie de durée d'utilisation de projet
double _1990_coef_psi0_bat_exp_eu(int type);
double _1990_coef_psi1_bat_exp_eu(int type);
double _1990_coef_psi2_bat_exp_eu(int type);
// _1990_coef_psi0_bat_exp_fr : renvoie le coefficient psi0 indiquée de la norme française
// pour les bâtiment et les charges d'exploitation
double _1990_coef_psi0_bat_exp_fr(int type);
double _1990_coef_psi1_bat_exp_fr(int type);
double _1990_coef_psi2_bat_exp_fr(int type);

// _1990_coef_psi0_bat_exp_txt_eu : renvoie le descriptif de la catégorie du coefficient psi0
// indiquée de la norme européenne pour les bâtiment et les charges d'exploitation
// Si la valeur de type ne correspond pas à une catégorie, la fonction renvoie NULL.
// type : categorie de durée d'utilisation de projet
char* _1990_coef_psi_bat_exp_txt_eu(int type);
// _1990_coef_psi0_bat_exp_txt_eu : renvoie le descriptif de la catégorie du coefficient psi0
// indiquée de la norme française pour les bâtiment et les charges d'exploitation
char* _1990_coef_psi_bat_exp_txt_fr(int type);

// _1990_coef_psi0_bat_neige_eu : renvoie le coefficient psi0 indiquée de la norme européenne
// pour les bâtiment et les charges de neige
// Si la valeur de type ne correspond pas à une catégorie, la fonction renvoie -1.
// type : categorie de durée d'utilisation de projet
double _1990_coef_psi0_bat_neige_eu(int type);
double _1990_coef_psi1_bat_neige_eu(int type);
double _1990_coef_psi2_bat_neige_eu(int type);
// _1990_coef_psi0_bat_exp_fr : renvoie le coefficient psi0 indiquée de la norme française
// pour les bâtiment et les charges de neige
double _1990_coef_psi0_bat_neige_fr(int type);
double _1990_coef_psi1_bat_neige_fr(int type);
double _1990_coef_psi2_bat_neige_fr(int type);

// _1990_coef_psi0_bat_neige_txt_eu : renvoie le descriptif de la catégorie du coefficient psi0
// indiquée de la norme européenne pour les bâtiment et les charges de neige
// Si la valeur de type ne correspond pas à une catégorie, la fonction renvoie NULL.
// type : categorie de durée d'utilisation de projet
char* _1990_coef_psi_bat_neige_txt_eu(int type);
// _1990_coef_psi0_bat_neige_txt_eu : renvoie le descriptif de la catégorie du coefficient psi0
// indiquée de la norme française pour les bâtiment et les charges de neige
char* _1990_coef_psi_bat_neige_txt_fr(int type);

// _1990_coef_psi0_bat_vent_eu : renvoie le coefficient psi0 indiquée de la norme européenne
// pour les bâtiment et les charges de vent
// Si la valeur de type ne correspond pas à une catégorie, la fonction renvoie -1.
// type : categorie de durée d'utilisation de projet
double _1990_coef_psi0_bat_vent_eu(int type);
double _1990_coef_psi1_bat_vent_eu(int type);
double _1990_coef_psi2_bat_vent_eu(int type);
// _1990_coef_psi0_bat_vent_fr : renvoie le coefficient psi0 indiquée de la norme française
// pour les bâtiment et les charges de vent
double _1990_coef_psi0_bat_vent_fr(int type);
double _1990_coef_psi1_bat_vent_fr(int type);
double _1990_coef_psi2_bat_vent_fr(int type);

// _1990_coef_psi0_bat_vent_txt_eu : renvoie le descriptif de la catégorie du coefficient psi0
// indiquée de la norme européenne pour les bâtiment et les charges de vent
// Si la valeur de type ne correspond pas à une catégorie, la fonction renvoie NULL.
// type : categorie de durée d'utilisation de projet
char* _1990_coef_psi_bat_vent_txt_eu(int type);
// _1990_coef_psi0_bat_vent_txt_eu : renvoie le descriptif de la catégorie du coefficient psi0
// indiquée de la norme française pour les bâtiment et les charges de vent
char* _1990_coef_psi_bat_vent_txt_fr(int type);

// _1990_coef_psi0_bat_temp_eu : renvoie le coefficient psi0 indiquée de la norme européenne
// pour les bâtiment et les charges de température
// Si la valeur de type ne correspond pas à une catégorie, la fonction renvoie -1.
// type : categorie de durée d'utilisation de projet
double _1990_coef_psi0_bat_temp_eu(int type);
double _1990_coef_psi1_bat_temp_eu(int type);
double _1990_coef_psi2_bat_temp_eu(int type);
// _1990_coef_psi0_bat_temp_fr : renvoie le coefficient psi0 indiquée de la norme française
// pour les bâtiment et les charges de température
double _1990_coef_psi0_bat_temp_fr(int type);
double _1990_coef_psi1_bat_temp_fr(int type);
double _1990_coef_psi2_bat_temp_fr(int type);

// _1990_coef_psi0_bat_temp_txt_eu : renvoie le descriptif de la catégorie du coefficient psi0
// indiquée de la norme européenne pour les bâtiment et les charges de température
// Si la valeur de type ne correspond pas à une catégorie, la fonction renvoie NULL.
// type : categorie de durée d'utilisation de projet
char* _1990_coef_psi_bat_temp_txt_eu(int type);
// _1990_coef_psi0_bat_temp_txt_eu : renvoie le descriptif de la catégorie du coefficient psi0
// indiquée de la norme française pour les bâtiment et les charges de température
char* _1990_coef_psi_bat_temp_txt_fr(int type);

