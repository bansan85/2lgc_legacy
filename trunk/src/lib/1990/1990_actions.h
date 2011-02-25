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

typedef struct __charge {
	char			*nom;
	char			*description;
	double			valeur_caracteristique;
	double			valeur_combinaison;		// valeur_caracteristique * psi0
	double			valeur_frequente;		// valeur_caracteristique * psi1
	double			valeur_quasi_permanente;	// valeur_caracteristique * psi2
} charge;


typedef struct __action {
	char			*nom;
	char			*description;
	int			categorie;			// Les catégories sont défnies dans les fonctions _1990_action_categorie
	charge*			*charges;
	struct __action		*suivant;
} action;

// _1990_action_categorie_bat_txt_eu : renvoie la description des catérogies des actions des bâtiments selon la norme européenne
// Renvoie NULL si la catégorie n'existe pas
// type : catégorie de durée d'utilisation de projet
char *_1990_action_categorie_bat_txt_eu(int type);
// _1990_action_categorie_bat_txt_fr : renvoie la description des catérogies des actions des bâtimentsselon la norme française
char *_1990_action_categorie_bat_txt_fr(int type);

// _1990_action_type_combinaison_bat_eu : renvoie le type d'une action pour les combinaisons d'action des bâtiments selon la norme européenne
// Renvoie	: -1 si la catégorie n'existe pas
//		: 1 si Action permanente
//		: 2 si action précontrainte
//		: 3 si action variable
//		: 4 si accidentelle
// type : catégorie de durée d'utilisation de projet
int _1990_action_type_combinaison_bat_eu(int categorie);
// _1990_action_type_combinaison_bat_fr : renvoie le type d'une action pour les combinaisons d'action des bâtiments selon la norme française
int _1990_action_type_combinaison_bat_fr(int categorie);
// Renvoie	: -1 si la catégorie n'existe pas
//		: 1 si Action permanente
//		: 2 si action précontrainte
//		: 3 si action variable
//		: 4 si accidentelle
//		: 5 si eaux souterraines


