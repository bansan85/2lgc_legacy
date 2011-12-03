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
#include "common_projet.h"

char *_1990_action_type_bat_txt(int type, Type_Pays pays);

Action_Categorie _1990_action_categorie_bat(int type, Type_Pays pays);

int _1990_action_init(Projet *projet);
int _1990_action_ajout(Projet *projet, int categorie);
int _1990_action_cherche_numero(Projet *projet, size_t numero);
void *_1990_action_cherche_charge(Projet *projet, size_t num_action, size_t num_charge);
int _1990_action_deplace_charge(Projet *projet, size_t action_src, size_t charge_src, size_t action_dest);
int _1990_action_affiche_tout(Projet *projet);
int _1990_action_affiche_resultats(Projet *projet, int num_action);
int _1990_action_free(Projet *projet);

#endif
