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

#ifndef __1990_ACTIONS_PRIVATE_H
#define __1990_ACTIONS_PRIVATE_H

#include "common_projet.h"

struct __Action
{
    char            *nom;
    unsigned int    numero;
    unsigned int    type;                   // La description est type est donnée par
                                            // _1990_action_type_bat_txt.
    GList           *charges;
    unsigned int    action_predominante : 1;// flag utilisé à titre temporaire.
    Flottant        psi0;                   // valeur_combinaison
    Flottant        psi1;                   // valeur_frequente
    Flottant        psi2;                   // valeur_quasi_permanente
    cholmod_sparse  *deplacement_complet;
    cholmod_sparse  *forces_complet;
    cholmod_sparse  *efforts_noeuds;
#ifdef ENABLE_GTK
    GtkTreeIter     Iter_fenetre;           // Pour le treeview
    GtkTreeIter     Iter_liste;             // pour le composant list_actions
#endif
    
    Fonction        **fonctions_efforts[6]; // 6 fonctions (N, Ty, Tz, Mx, My, Mz) par barre.
                    // Les fonctions représentent la courbe des efforts dues aux charges dans
                    // les barres et dus aux déplacements calculés par la méthode des EF.
    Fonction        **fonctions_deformation[3]; // Les 3 déformations x, y, z pour chaque barre
    Fonction        **fonctions_rotation[3];    // Les 3 rotations rx, ry, rz pour chaque barre
};


#endif
