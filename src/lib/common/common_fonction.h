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

#ifndef __COMMON_FONCTION_H
#define __COMMON_FONCTION_H

#include "config.h"
#include "common_projet.h"


gboolean common_fonction_scinde_troncon          (Fonction    *fonction,
                                                  double       coupure)
                                       __attribute__((__warn_unused_result__));

gboolean common_fonction_ajout_poly              (Fonction    *fonction,
                                                  double       debut_troncon,
                                                  double       fin_troncon,
                                                  double       coef_0,
                                                  double       coef_x,
                                                  double       coef_x2,
                                                  double       coef_x3,
                                                  double       coef_x4,
                                                  double       coef_x5,
                                                  double       coef_x6,
                                                  double       translate)
                                       __attribute__((__warn_unused_result__));

gboolean common_fonction_ajout_fonction          (Fonction    *fonction,
                                                  Fonction *fonction_a_ajouter,
                                                  double       multi)
                                       __attribute__((__warn_unused_result__));

double   common_fonction_y                       (Fonction    *fonction,
                                                  double       x_,
                                                  int8_t       position)
                                       __attribute__((__warn_unused_result__));

gboolean common_fonction_affiche                 (Fonction    *fonction)
                                       __attribute__((__warn_unused_result__));

#ifdef ENABLE_GTK
GdkPixbuf *common_fonction_dessin                (GList       *fonctions,
                                                  uint16_t     width,
                                                  uint16_t     height,
                                                  int8_t       decimales)
                                       __attribute__((__warn_unused_result__));
#endif

char *common_fonction_affiche_caract             (Fonction    *fonction,
                                                  uint8_t      decimales_x,
                                                  uint8_t      decimales_y)
                                       __attribute__((__warn_unused_result__));

gboolean common_fonction_conversion_combinaisons (Fonction    *fonction,
                                                  GList       *ponderations,
                                                  GList      **liste)
                                       __attribute__((__warn_unused_result__));

char *common_fonction_renvoie                    (Fonction    *fonction,
                                                  GList       *index,
                                                  int8_t       decimales)
                                       __attribute__((__warn_unused_result__));

gboolean common_fonction_renvoie_enveloppe       (GList       *fonctions,
                                                  Fonction    *fonction_min,
                                                  Fonction    *fonction_max,
                                                  Fonction    *comb_min,
                                                  Fonction    *comb_max)
                                       __attribute__((__warn_unused_result__));

#endif

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
