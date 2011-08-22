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

#include "config.h"
#include <list.h>
#include <cholmod.h>

#include "common_projet.h"
#include "common_fonction.h"
#include "EF_noeud.h"

typedef enum __Action_Categorie
{
    ACTION_POIDS_PROPRE,
    ACTION_PRECONTRAINTE,
    ACTION_VARIABLE,
    ACTION_ACCIDENTELLE,
    ACTION_SISMIQUE,
    ACTION_EAUX_SOUTERRAINES,
    ACTION_INCONNUE
} Action_Categorie;

typedef struct __Action
{
    char            *nom;
    char            *description;
    int             numero;
    int             type;  // Les catégories sont conformes à _1990_action_type
    LIST            *charges;
    int             flags;
    double          psi0;       // valeur_combinaison
    double          psi1;       // valeur_frequente
    double          psi2;       // valeur_quasi_permanente
    cholmod_sparse  *deplacement_complet;
    cholmod_sparse  *forces_complet;
    cholmod_sparse  *efforts_noeuds;
    double          norm;
    Fonction        **fonctions_efforts[6]; // 6 fonctions (N, Ty, Tz, Mx, My, Mz) par barre.
                    // Les fonctions représentent la courbe des efforts dues aux charges dans
                    // les barres et dus aux déplacements calculés par la méthode des EF.
    Fonction        **fonctions_fleche[3];      // Les 3 flèches x, y, z pour chaque barre.
    Fonction        **fonctions_rotation[3];    // Les 3 rotations rx, ry, rz pour chaque barre
} Action;

char *_1990_action_type_bat_txt(int type, Type_Pays pays);

Action_Categorie _1990_action_categorie_bat(int type, Type_Pays pays);

int _1990_action_init(Projet *projet);
int _1990_action_ajout(Projet *projet, int categorie);
int _1990_action_cherche_numero(Projet *projet, int numero);
int _1990_action_affiche_tout(Projet *projet);
int _1990_action_free(Projet *projet);

#endif
