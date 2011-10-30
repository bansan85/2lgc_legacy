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

#ifndef __1992_1_1_BARRES_H
#define __1992_1_1_BARRES_H

#include "config.h"
#include "EF_relachement.h"
#include "EF_noeud.h"
#include "1992_1_1_materiaux.h"
#include "common_projet.h"
#include "cholmod.h"

typedef struct __Barre_Info_EF
{
    cholmod_sparse      *matrice_rigidite_locale;
    
    double              ay; // Paramètres de souplesse de la poutre
    double              by;
    double              cy;
    double              az;
    double              bz;
    double              cz;
    
    double              kAx;
    double              kAy; // coefficient défissant l'inverse de la raideur aux noeuds
    double              kAz;
    double              kBx;
    double              kBy;
    double              kBz;
} Barre_Info_EF;

typedef struct __Beton_Barre
{
    unsigned int        numero;
    Type_Element        type;
    void                *section;
    Beton_Materiau      *materiau;
    
    EF_Noeud            *noeud_debut;
    EF_Noeud            *noeud_fin;
    unsigned int        discretisation_element;     // Nombre de noeuds intermédiaires
    Barre_Info_EF       *info_EF;                   // Une info par élément discrétisé
    EF_Noeud            **noeuds_intermediaires;
    EF_Relachement      *relachement;
    
    cholmod_sparse      *matrice_rotation;
    cholmod_sparse      *matrice_rotation_transpose;
} Beton_Barre;

int _1992_1_1_barres_init(Projet *projet);
int _1992_1_1_barres_ajout(Projet *projet, Type_Element type, unsigned int section, unsigned int materiau, unsigned int noeud_debut, unsigned int noeud_fin, int relachement, unsigned int discretisation_element);
Beton_Barre* _1992_1_1_barres_cherche_numero(Projet *projet, unsigned int numero);
int _1992_1_1_barres_angle_rotation(Beton_Barre *barre, double *y, double *z);
int _1992_1_1_barres_rigidite_ajout(Projet *projet, Beton_Barre *element);
int _1992_1_1_barres_rigidite_ajout_tout(Projet *projet);
int _1992_1_1_barres_free(Projet *projet);

#endif
