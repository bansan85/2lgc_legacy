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

Charge_Barre_Repartie_Uniforme *EF_charge_barre_repartie_uniforme_ajout(Projet *projet, unsigned int num_action, GList *barres, int repere_local, int projection, double a, double b, double fx, double fy, double fz, double mx, double my, double mz, const char *nom) __attribute__((__warn_unused_result__));
char* EF_charge_barre_repartie_uniforme_description(Charge_Barre_Repartie_Uniforme *charge) __attribute__((__warn_unused_result__));

gboolean EF_charge_barre_repartie_uniforme_mx(Beton_Barre *barre, unsigned int discretisation, double a, double b, Barre_Info_EF *infos, double mx, double *ma, double *mb) __attribute__((__warn_unused_result__));

gboolean EF_charge_barre_repartie_uniforme_def_ang_iso_y(Beton_Barre *barre, unsigned int discretisation, double a, double b, double fz, double my, double *phia, double *phib) __attribute__((__warn_unused_result__));
gboolean EF_charge_barre_repartie_uniforme_def_ang_iso_z(Beton_Barre *barre, unsigned int discretisation, double a, double b, double fy, double mz, double *phia, double *phib) __attribute__((__warn_unused_result__));

double EF_charge_barre_repartie_uniforme_position_resultante_x(EF_Section *section, double a, double b, double l) __attribute__((__warn_unused_result__));

gboolean EF_charge_barre_repartie_uniforme_fonc_rx(Fonction *fonction, Beton_Barre *barre, unsigned int discretisation, double a, double b, double max, double mbx) __attribute__((__warn_unused_result__));
gboolean EF_charge_barre_repartie_uniforme_fonc_ry(Fonction *f_rotation, Fonction* f_deform, Beton_Barre *barre, unsigned int discretisation, double a, double b, double fz, double my, double may, double mby) __attribute__((__warn_unused_result__));
gboolean EF_charge_barre_repartie_uniforme_fonc_rz(Fonction *f_rotation, Fonction* f_deform, Beton_Barre *barre, unsigned int discretisation, double a, double b, double fy, double mz, double maz, double mbz) __attribute__((__warn_unused_result__));

gboolean EF_charge_barre_repartie_uniforme_n(Fonction *fonction, Beton_Barre *barre, unsigned int discretisation, double a, double b, double fax, double fbx) __attribute__((__warn_unused_result__));

gboolean EF_charge_barre_repartie_uniforme_enleve_barres(Charge_Barre_Repartie_Uniforme *charge, GList *barres, Projet *projet) __attribute__((__warn_unused_result__));
gboolean EF_charge_barre_repartie_uniforme_free(Charge_Barre_Repartie_Uniforme *charge) __attribute__((__warn_unused_result__));

#endif
