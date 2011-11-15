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

#ifndef __EF_CHARGE_BARRE_PONCTUELLE_H
#define __EF_CHARGE_BARRE_PONCTUELLE_H

#include "config.h"
#include <list.h>
#include <cholmod.h>
#include <gtk/gtk.h>

#include "common_projet.h"
#include "common_fonction.h"
#include "EF_noeud.h"
#include "EF_calculs.h"
#include "1992_1_1_barres.h"

typedef struct __Charge_Barre_Ponctuelle
{
    Barre_Charge_Type   type;
    int                 numero;
    char                *nom;
    char                *description;
    void                *barre;
    int                 repere_local;
    double              position; // Position de la charge ponctuelle en mètre
                                  // depuis le début de la barre
    double              fx;
    double              fy;
    double              fz;
    double              mx;
    double              my;
    double              mz;
    
    GtkTreeIter         *pIter;
} Charge_Barre_Ponctuelle;

int EF_charge_barre_ponctuelle_ajout(Projet *projet, int num_action, Beton_Barre *barre, 
  int repere_local, double a, double fx, double fy, double fz, double mx, double my,
  double mz, char* nom);
int EF_charge_barre_ponctuelle_mx(Beton_Barre *barre, unsigned int discretisation,
  double position, Barre_Info_EF *infos, double mx, double *ma, double *mb);
int EF_charge_barre_ponctuelle_def_ang_iso_y(Beton_Barre *barre, unsigned int discretisation,
  double position, double fz, double my, double *phia, double *phib);
int EF_charge_barre_ponctuelle_def_ang_iso_z(Beton_Barre *barre, unsigned int discretisation,
  double position, double fy, double mz, double *phia, double *phib);
int EF_charge_barre_ponctuelle_fonc_rx(Fonction *fonction, Beton_Barre *barre,
  unsigned int discretisation, double a, double max, double mbx);
int EF_charge_barre_ponctuelle_fonc_ry(Fonction *f_rotation, Fonction* f_deform,
  Beton_Barre *barre, unsigned int discretisation, double position, double fz, double my,
  double may, double mby);
int EF_charge_barre_ponctuelle_fonc_rz(Fonction *f_rotation, Fonction* f_deform,
  Beton_Barre *barre, unsigned int discretisation, double a, double fy, double mz,
  double maz, double mbz);
int EF_charge_barre_ponctuelle_n(Fonction *fonction, Beton_Barre *barre,
  unsigned int discretisation, double a, double fax, double fbx);

#endif
