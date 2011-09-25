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

#ifndef __EF_NOEUD_H
#define __EF_NOEUD_H

#include "config.h"
#include "common_projet.h"
#include "EF_appuis.h"

typedef struct __EF_Point
{
    double      x;
    double      y;
    double      z;
} EF_Point;


typedef struct __EF_Noeud
{
    int         numero;
    EF_Point    position;
    EF_Appui    *appui;
} EF_Noeud;

int EF_noeuds_init(Projet *projet);
int EF_noeuds_ajout(Projet *projet, double x, double y, double z, int appui);
EF_Noeud* EF_noeuds_cherche_numero(Projet *projet, int numero);
double EF_noeuds_distance(EF_Noeud* n1, EF_Noeud* n2);
int EF_noeuds_free(Projet *projet);

#endif
