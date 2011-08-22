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

#include "common_projet.h"
#include "common_fonction.h"
#include "EF_noeud.h"

typedef struct __Charge_Barre_Ponctuelle
{
    Charge_Type     type;
    int             numero;
    char            *nom;
    char            *description;
    void            *barre;
    int             repere_local;
    double          position; // Position de la charge ponctuelle en mètre
                              // depuis le début de la barre
    double          x;
    double          y;
    double          z;
    double          rx;
    double          ry;
    double          rz;
} Charge_Barre_Ponctuelle;

int EF_charge_barre_ponctuelle_ajout(Projet *projet, int num_action, void *barre, int repere_local, double position, double fx, double fy, double fz, double rx, double ry, double rz);

#endif
