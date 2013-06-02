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

#ifndef __EF_CHARGE_NOEUD_H
#define __EF_CHARGE_NOEUD_H

#include "config.h"
#include "common_projet.h"

Charge_Noeud* EF_charge_noeud_ajout(Projet *projet, unsigned int num_action, GList *noeud, Flottant fx, Flottant fy, Flottant fz, Flottant mx, Flottant my, Flottant mz, const char* nom) __attribute__((__warn_unused_result__));
char* EF_charge_noeud_description(Charge_Noeud *charge) __attribute__((__warn_unused_result__));
gboolean EF_charge_noeud_enleve_noeuds(Charge_Noeud *charge, GList *noeuds, Projet *projet) __attribute__((__warn_unused_result__));
gboolean EF_charge_noeud_free(Charge_Noeud *charge) __attribute__((__warn_unused_result__));

#endif
