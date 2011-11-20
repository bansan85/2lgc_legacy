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

#ifndef __EF_CALCULS_H
#define __EF_CALCULS_H

#include "config.h"
#include "common_projet.h"

int EF_calculs_initialise(Projet *projet);
int EF_calculs_genere_mat_rig(Projet *projet);
int EF_calculs_moment_hyper_y(Barre_Info_EF *infos, double phia, double phib,
  double *ma, double *mb);
int EF_calculs_moment_hyper_z(Barre_Info_EF *infos, double phia, double phib,
  double *ma, double *mb);
int EF_calculs_resoud_charge(Projet *projet, int num_action);

#endif
