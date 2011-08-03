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

#ifndef __1990_ACTIONS_H
#define __1990_ACTIONS_H

#include <list.h>
#include <cholmod.h>

#include "common_projet.h"
#include "common_fonction.h"
#include "EF_noeud.h"

typedef enum
{
	CHARGE_PONCTUELLE_NOEUD,
	CHARGE_PONCTUELLE_BARRE
} Charge_Type;


typedef struct __Charge_Ponctuelle_Noeud
{
	Charge_Type		type;
	int			numero;
	char			*nom;
	char			*description;
	EF_Noeud		*noeud;
	double			x;
	double			y;
	double			z;
	double			rx;
	double			ry;
	double			rz;
} Charge_Ponctuelle_Noeud;

typedef struct __Charge_Ponctuelle_Barre
{
	Charge_Type		type;
	int			numero;
	char			*nom;
	char			*description;
	void			*barre; // Peut être Beton_Element, Acier_Element ou autre
	int			repere_local;
	double			position; // Position de la charge ponctuelle en mètre depuis le début de la barre
	double			x;
	double			y;
	double			z;
	double			rx;
	double			ry;
	double			rz;
} Charge_Ponctuelle_Barre;

typedef struct __Action
{
	char			*nom;
	char			*description;
	int			numero;
	int			categorie;			// Les catégories sont définies dans les fonctions _1990_action_categorie
	LIST			*charges;
	int			flags;
	double			psi0;		// valeur_combinaison
	double			psi1;		// valeur_frequente
	double			psi2;		// valeur_quasi_permanente
	cholmod_sparse		*deplacement_partiel;
	cholmod_sparse		*deplacement_complet;
	cholmod_sparse		*forces_complet;
	cholmod_sparse		*efforts_noeuds;
	double			norm;
	Fonction		**fonctions_efforts[6];		// Les 6 fonctions (N, Ty, Tz, Mx, My, Mz) pour chaque barre.
								// Les fonctions représentent la courbe des efforts dues aux charges dans
								// les barres et dus aux déplacements calculés par la méthode des EF
	Fonction		**fonctions_fleche[3];		// Les 3 flèches x, y, z pour chaque barre.
	Fonction		**fonctions_rotation[3];	// Les 3 rotations rx, ry, rz pour chaque barre.
} Action;

char *_1990_action_categorie_bat_txt_eu(int type);
char *_1990_action_categorie_bat_txt_fr(int type);
char *_1990_action_categorie_bat_txt(int type, Type_Pays pays);

int _1990_action_type_combinaison_bat_eu(int categorie);
int _1990_action_type_combinaison_bat_fr(int categorie);
int _1990_action_type_combinaison_bat(int categorie, Type_Pays pays);

int _1990_action_init(Projet *projet);
int _1990_action_ajout(Projet *projet, int categorie);
int _1990_action_cherche_numero(Projet *projet, int numero);
int _1990_action_affiche_tout(Projet *projet);
int _1990_action_ajout_charge_ponctuelle_noeud(Projet *projet, int num_action, EF_Noeud *noeud, double fx, double fy, double fz, double rx, double ry, double rz);
int _1990_action_ajout_charge_ponctuelle_barre(Projet *projet, int num_action, void *barre, int repere_local, double position, double fx, double fy, double fz, double rx, double ry, double rz);
int _1990_action_free(Projet *projet);

#endif
