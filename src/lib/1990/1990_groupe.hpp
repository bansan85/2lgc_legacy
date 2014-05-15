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
#include "common_projet.hpp"

bool     _1990_groupe_init (Projet *p) __attribute__((__warn_unused_result__));

bool     _1990_groupe_ajout_niveau  (Projet                 *p)
                                       __attribute__((__warn_unused_result__));
Groupe * _1990_groupe_ajout_groupe  (Projet                 *p,
                                     Niveau_Groupe          *niveau_groupe,
                                     Type_Groupe_Combinaison type_combinaison,
                                     const char             *nom)
                                       __attribute__((__warn_unused_result__));
bool     _1990_groupe_ajout_element (Projet                 *p,
                                     Niveau_Groupe          *niveau_groupe,
                                     Groupe                 *groupe,
                                     void                   *element_add)
                                       __attribute__((__warn_unused_result__));
bool     _1990_groupe_modifie_combinaison (Groupe           *groupe,
                                     Type_Groupe_Combinaison type_combinaison)
                                       __attribute__((__warn_unused_result__));
bool     _1990_groupe_modifie_nom   (Niveau_Groupe          *groupe_niveau,
                                     Groupe                 *groupe,
                                     const char             *nom,
                                     Projet                 *p)
                                       __attribute__((__warn_unused_result__));

bool     _1990_groupe_free_combinaisons (
           std::list <std::list <Combinaison *> *> *liste);
bool     _1990_groupe_free_niveau          (Projet        *p,
                                            Niveau_Groupe *niveau_groupe,
                                            bool           accept_vide)
                                       __attribute__((__warn_unused_result__));
bool     _1990_groupe_free_groupe          (Projet        *p,
                                            Niveau_Groupe *niveau_groupe,
                                            Groupe        *groupe)
                                       __attribute__((__warn_unused_result__));
bool     _1990_groupe_retire_element       (Projet        *p,
                                            Niveau_Groupe *niveau_groupe,
                                            Groupe        *groupe,
                                            void          *element)
                                       __attribute__((__warn_unused_result__));
bool     _1990_groupe_free                 (Projet        *p)
                                       __attribute__((__warn_unused_result__));

bool     _1990_groupe_affiche_tout         (Projet *p)
                                       __attribute__((__warn_unused_result__));

#endif

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
