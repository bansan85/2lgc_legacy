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

#ifndef __1990_GROUPES_H
#define __1990_GROUPES_H

#include "config.h"
#include <list.h>
#ifdef ENABLE_GTK
#include <gtk/gtk.h>
#endif

#include "1990_combinaisons.h"

typedef struct __Element
{
    int         numero;
#ifdef ENABLE_GTK
    GtkTreeIter *pIter;
    int         pIter_expand;
#endif
} Element;

typedef enum __Type_Groupe_Combinaison
{
    GROUPE_COMBINAISON_OR,
    GROUPE_COMBINAISON_XOR,
    GROUPE_COMBINAISON_AND
} Type_Groupe_Combinaison;

typedef struct __Groupe
{
    char                    *nom;
    int                     numero;
    Type_Groupe_Combinaison type_combinaison;
    LIST                    *elements;
    Combinaisons            tmp_combinaison;
#ifdef ENABLE_GTK
    GtkTreeIter             *pIter;
    int                     pIter_expand;
#endif
} Groupe;

typedef struct __Niveau_Groupe
{
    int             niveau;
    LIST            *groupes;
#ifdef ENABLE_GTK
    GtkTreeIter     *pIter;
#endif
} Niveau_Groupe;

int _1990_groupe_init(Projet *projet);

int _1990_groupe_ajout_niveau(Projet *projet);
int _1990_groupe_ajout_groupe(Projet *projet, int niveau,
  Type_Groupe_Combinaison type_combinaison, char* nom);
int _1990_groupe_ajout_element(Projet *projet, unsigned int niveau, int groupe_n, int groupe_n_1);

int _1990_groupe_positionne_niveau(LIST *source, int numero);
int _1990_groupe_positionne_groupe(Niveau_Groupe *niveau, int numero);
int _1990_groupe_positionne_element(Groupe *groupe, int numero);

void _1990_groupe_free_niveau(Projet *projet, int niveau);
void _1990_groupe_free_groupe(Projet *projet, int niveau, int groupe);
void _1990_groupe_free_element(Projet *projet, int niveau, int groupe, int element);
void _1990_groupe_free(Projet *projet);

int _1990_groupe_affiche_tout(Projet *projet);

#endif
