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

#ifndef __EF_CHARGE_BARRE_REPARTIE_UNIFORME
#define __EF_CHARGE_BARRE_REPARTIE_UNIFORME

#include "config.h"
#include "common_projet.h"

Charge_Barre_Repartie_Uniforme *EF_charge_barre_repartie_uniforme_ajout(Projet *projet,
  int num_action, Beton_Barre *barre, int repere_local, int projection, double a, double b,
  double fx, double fy, double fz, double mx, double my, double mz, const char *nom);
int EF_charge_barre_repartie_uniforme_mx(Beton_Barre *barre, unsigned int discretisation,
  double a, double b, Barre_Info_EF *infos, double mx, double *ma, double *mb);
int EF_charge_barre_repartie_uniforme_def_ang_iso_y(Beton_Barre *barre,
  unsigned int discretisation, double a, double b, double fz, double my,
  double *phia, double *phib);
int EF_charge_barre_repartie_uniforme_def_ang_iso_z(Beton_Barre *barre,
  unsigned int discretisation, double a, double b, double fy, double mz,
  double *phia, double *phib);
double EF_charge_barre_repartie_uniforme_position_resultante_x(void *section,
  double a, double b, double l);
int EF_charge_barre_repartie_uniforme_fonc_rx(Fonction *fonction, Beton_Barre *barre,
  unsigned int discretisation, double a, double b, double max, double mbx);
int EF_charge_barre_repartie_uniforme_fonc_ry(Fonction *f_rotation, Fonction* f_deform,
  Beton_Barre *barre, unsigned int discretisation, double a, double b, double fz, double my,
  double may, double mby);
int EF_charge_barre_repartie_uniforme_fonc_rz(Fonction *f_rotation, Fonction* f_deform,
  Beton_Barre *barre, unsigned int discretisation, double a, double b, double fy, double mz,
  double maz, double mbz);
int EF_charge_barre_repartie_uniforme_n(Fonction *fonction, Beton_Barre *barre,
  unsigned int discretisation, double a, double b, double fax, double fbx);

#endif
