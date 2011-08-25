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

#ifndef __1990_PONDERATIONS_H
#define __1990_PONDERATIONS_H

#include "config.h"
#include <list.h>

#include "common_projet.h"
#include "1990_actions.h"

typedef struct __Ponderation_Element
{
    double      ponderation;
    int         psi;
    int         flags;
    Action      *action;
} Ponderation_Element;

// Ne pas supprimer l'espace après le __Ponderation, c'est pour la génération du manuel Latex
typedef struct __Ponderation 
{
    LIST        *elements; // Liste de pointeur Action* avec leur ponderation
} Ponderation;

int _1990_ponderations_genere(Projet *projet);
void _1990_ponderations_affiche_tout(Projet *projet);
#endif