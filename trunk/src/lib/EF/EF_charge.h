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

#ifndef __EF_CHARGE_H
#define __EF_CHARGE_H

#include "config.h"
#include "common_projet.h"

void *EF_charge_cherche(Projet *projet, unsigned int num_action, unsigned int num_charge);
int EF_charge_renomme(Projet *projet, unsigned int numero_action, unsigned int numero_charge, const char *description);
int EF_charge_deplace(Projet *projet, unsigned int action_src, unsigned int charge_src, unsigned int action_dest);
int EF_charge_supprime(Projet *projet, unsigned int action_num, unsigned int charge_num);

#endif
