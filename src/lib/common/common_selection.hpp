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

#ifndef __COMMON_SELECTION_H
#define __COMMON_SELECTION_H

#include "config.h"
#include "common_projet.hpp"


void common_selection_ajout_nombre  (uint32_t                nb,
                                     std::list <uint32_t>   *liste);
void common_selection_ajout_nombre  (EF_Noeud *              noeud,
                                     std::list <EF_Noeud *> *liste);
void common_selection_ajout_nombre  (EF_Barre *              barre,
                                     std::list <EF_Barre *> *liste);

void common_selection_ajout_nombre  (Charge               *charge,
                                     std::list <Charge *> *liste,
                                     Projet               *p);
std::list <uint32_t>   *common_selection_renvoie_numeros (const char *texte)
                                       __attribute__((__warn_unused_result__));

std::list <EF_Noeud *> *common_selection_numeros_en_noeuds (
                          std::list <uint32_t> *liste_numeros,
                          Projet               *p)
                                       __attribute__((__warn_unused_result__));
std::list <EF_Barre *> *common_selection_numeros_en_barres (
                          std::list <uint32_t> *liste_numeros,
                          Projet               *p)
                                       __attribute__((__warn_unused_result__));
char    *common_selection_noeuds_en_texte   (std::list <EF_Noeud *> *liste)
                                       __attribute__((__warn_unused_result__));
char    *common_selection_barres_en_texte   (std::list <EF_Barre *> *liste)
                                       __attribute__((__warn_unused_result__));
char    *common_selection_charges_en_texte  (
           std::list <Charge *> *liste_charges,
           Projet               *p)
                                       __attribute__((__warn_unused_result__));

#endif

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
