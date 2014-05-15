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
#include "common_projet.hpp"

bool     EF_noeuds_init                 (Projet  *p)
                                       __attribute__((__warn_unused_result__));

EF_Noeud* EF_noeuds_ajout_noeud_libre   (Projet      *p,
                                         Flottant     x,
                                         Flottant     y,
                                         Flottant     z,
                                         EF_Appui    *appui,
                                         EF_Noeud    *relatif)
                                       __attribute__((__warn_unused_result__));
EF_Noeud* EF_noeuds_ajout_noeud_barre   (Projet      *p,
                                         EF_Barre    *barre,
                                         Flottant     position_relative_barre,
                                         EF_Appui    *appui)
                                       __attribute__((__warn_unused_result__));

bool     EF_noeuds_renvoie_position     (EF_Noeud    *noeud,
                                         EF_Point    *point)
                                       __attribute__((__warn_unused_result__));

EF_Noeud* EF_noeuds_cherche_numero      (Projet      *p,
                                         unsigned int numero,
                                         bool         critique)
                                       __attribute__((__warn_unused_result__));

bool     EF_noeuds_change_pos_abs       (Projet      *p,
                                         EF_Noeud    *noeud,
                                         Flottant     x,
                                         Flottant     y,
                                         Flottant     z)
                                       __attribute__((__warn_unused_result__));
bool     EF_noeuds_change_pos_relat     (Projet      *p,
                                         EF_Noeud    *noeud,
                                         Flottant     pos)
                                       __attribute__((__warn_unused_result__));
bool     EF_noeuds_change_appui         (Projet      *p,
                                         EF_Noeud    *noeud,
                                         EF_Appui    *appui)
                                       __attribute__((__warn_unused_result__));
bool     EF_noeuds_change_noeud_relatif (Projet      *p,
                                         EF_Noeud    *noeud,
                                         EF_Noeud    *relatif)
                                       __attribute__((__warn_unused_result__));

double EF_points_distance               (EF_Point    *p1,
                                         EF_Point    *p2)
                                       __attribute__((__warn_unused_result__));
double EF_noeuds_distance               (EF_Noeud    *n1,
                                         EF_Noeud    *n2)
                                       __attribute__((__warn_unused_result__));
double EF_noeuds_distance_x_y_z         (EF_Noeud    *n1,
                                         EF_Noeud    *n2,
                                         double      *x,
                                         double      *y,
                                         double      *z)
                                       __attribute__((__warn_unused_result__));

void EF_noeuds_free_foreach             (EF_Noeud    *noeud,
                                         Projet      *p);
bool     EF_noeuds_free                 (Projet *p)
                                       __attribute__((__warn_unused_result__));

#endif

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
