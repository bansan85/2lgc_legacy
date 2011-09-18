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
#include <list.h>
#include <cholmod.h>

#include "common_projet.h"
#include "common_fonction.h"
#include "EF_noeud.h"
#include "EF_calculs.h"

typedef struct __Charge_Noeud
{
    Barre_Charge_Type   type;
    int                 numero;
    char                *nom;
    char                *description;
    EF_Noeud            *noeud;
    double              x;
    double              y;
    double              z;
    double              mx;
    double              my;
    double              mz;
} Charge_Noeud;

int EF_charge_noeud_ajout(Projet *projet, int num_action, EF_Noeud *noeud,
  double fx, double fy, double fz, double mx, double my, double mz);

#endif
