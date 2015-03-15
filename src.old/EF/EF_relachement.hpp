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

#ifndef __EF_RELACHEMENT_H
#define __EF_RELACHEMENT_H

#include "config.h"
#include "common_projet.hpp"

bool     EF_relachement_init         (Projet             *p)
                                       __attribute__((__warn_unused_result__));

EF_Relachement *EF_relachement_ajout (Projet             *p,
                                      std::string        *nom,
                                      EF_Relachement_Type rx_debut,
                                      void               *rx_d_data,
                                      EF_Relachement_Type ry_debut,
                                      void               *ry_d_data,
                                      EF_Relachement_Type rz_debut,
                                      void               *rz_d_data,
                                      EF_Relachement_Type rx_fin,
                                      void               *rx_f_data,
                                      EF_Relachement_Type ry_fin,
                                      void               *ry_f_data,
                                      EF_Relachement_Type rz_fin,
                                      void               *rz_f_data)
                                       __attribute__((__warn_unused_result__));

EF_Relachement* EF_relachement_cherche_nom (Projet             *p,
                                            std::string        *nom,
                                            bool                critique)
                                       __attribute__((__warn_unused_result__));
bool     EF_relachement_modif    (Projet             *p,
                                  EF_Relachement     *relachement,
                                  std::string        *nom,
                                  EF_Relachement_Type rx_debut,
                                  void               *rx_d_data,
                                  EF_Relachement_Type ry_debut,
                                  void               *ry_d_data,
                                  EF_Relachement_Type rz_debut,
                                  void               *rz_d_data,
                                  EF_Relachement_Type rx_fin,
                                  void               *rx_f_data,
                                  EF_Relachement_Type ry_fin,
                                  void               *ry_f_data,
                                  EF_Relachement_Type rz_fin,
                                  void               *rz_f_data)
                                       __attribute__((__warn_unused_result__));

bool     EF_relachement_supprime (EF_Relachement     *relachement,
                                  bool             annule_si_utilise,
                                  Projet             *p)
                                       __attribute__((__warn_unused_result__));
bool     EF_relachement_free     (Projet             *p)
                                       __attribute__((__warn_unused_result__));

#endif

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
