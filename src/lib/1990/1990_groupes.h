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

#ifndef __1990_GROUPES_H
#define __1990_GROUPES_H

#include "config.h"
#include "common_projet.h"

#include <glib.h>

int _1990_groupe_init(Projet *projet);

int _1990_groupe_ajout_niveau(Projet *projet);
Groupe *_1990_groupe_ajout_groupe(Projet *projet, unsigned int niveau, Type_Groupe_Combinaison type_combinaison, const char* nom);
int _1990_groupe_ajout_element(Projet *projet, unsigned int niveau, unsigned int groupe_n, unsigned int groupe_n_1);

Niveau_Groupe *_1990_groupe_positionne_niveau(Projet *projet, unsigned int numero);
Groupe *_1990_groupe_positionne_groupe(Niveau_Groupe *niveau, unsigned int numero);
Element *_1990_groupe_positionne_element(Groupe *groupe, unsigned int numero);

int _1990_groupe_free_niveau(Projet *projet, unsigned int niveau);
int _1990_groupe_free_groupe(Projet *projet, unsigned int niveau, unsigned int groupe);
int _1990_groupe_free_element(Projet *projet, unsigned int niveau, unsigned int groupe, unsigned int element);
int _1990_groupe_free(Projet *projet);

int _1990_groupe_affiche_tout(Projet *projet);

#endif
