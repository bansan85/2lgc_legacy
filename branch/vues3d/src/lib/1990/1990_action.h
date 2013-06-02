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

char *_1990_action_type_bat_txt(unsigned int type, Type_Pays pays) __attribute__((__warn_unused_result__));
unsigned int _1990_action_num_bat_txt(Type_Pays pays) __attribute__((__warn_unused_result__));
Action_Categorie _1990_action_categorie_bat(unsigned int type, Type_Pays pays) __attribute__((__warn_unused_result__));

gboolean _1990_action_init(Projet *projet) __attribute__((__warn_unused_result__));

Action *_1990_action_ajout(Projet *projet, unsigned int categorie, const char* description) __attribute__((__warn_unused_result__));
Action *_1990_action_cherche_numero(Projet *projet, unsigned int numero) __attribute__((__warn_unused_result__));

gboolean _1990_action_renomme(Projet *projet, Action *action, const char* description) __attribute__((__warn_unused_result__));
gboolean _1990_action_change_type(Projet *projet, Action *action, unsigned int type) __attribute__((__warn_unused_result__));
gboolean _1990_action_change_psi(Projet *projet, Action *action, unsigned int psi_num, Flottant psi) __attribute__((__warn_unused_result__));

gboolean _1990_action_affiche_tout(Projet *projet) __attribute__((__warn_unused_result__));
gboolean _1990_action_affiche_resultats(Projet *projet, unsigned int num_action) __attribute__((__warn_unused_result__));
gboolean _1990_action_free_num(Projet *projet, unsigned int num) __attribute__((__warn_unused_result__));
gboolean _1990_action_free(Projet *projet) __attribute__((__warn_unused_result__));

#endif
