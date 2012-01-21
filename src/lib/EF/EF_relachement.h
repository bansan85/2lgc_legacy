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

#ifndef __EF_RELACHEMENT_H
#define __EF_RELACHEMENT_H

#include "config.h"
#include "common_projet.h"

int EF_relachement_init(Projet *projet);
int EF_relachement_ajout(Projet *projet, const char *nom, EF_Relachement_Type rx_debut,
  void* rx_d_data, EF_Relachement_Type ry_debut, void* ry_d_data, EF_Relachement_Type rz_debut,
  void* rz_d_data, EF_Relachement_Type rx_fin, void* rx_f_data, EF_Relachement_Type ry_fin,
  void* ry_f_data, EF_Relachement_Type rz_fin, void* rz_f_data);
EF_Relachement* EF_relachement_cherche_numero(Projet *projet, unsigned int numero);
int EF_relachement_free(Projet *projet);

#endif
