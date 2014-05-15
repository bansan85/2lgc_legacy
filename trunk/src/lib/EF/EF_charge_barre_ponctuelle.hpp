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

#ifndef __EF_CHARGE_BARRE_PONCTUELLE_H
#define __EF_CHARGE_BARRE_PONCTUELLE_H

#include "config.h"
#include "common_projet.hpp"


Charge *EF_charge_barre_ponctuelle_ajout (Projet                 *p,
                                          Action                 *action,
                                          std::list <EF_Barre *> *barres,
                                          bool                    repere_local,
                                          Flottant                a,
                                          Flottant                fx,
                                          Flottant                fy,
                                          Flottant                fz,
                                          Flottant                mx,
                                          Flottant                my,
                                          Flottant                mz,
                                          const char              *nom)
                                       __attribute__((__warn_unused_result__));
char *EF_charge_barre_ponctuelle_description      (Charge      *charge)
                                       __attribute__((__warn_unused_result__));

bool EF_charge_barre_ponctuelle_mx            (EF_Barre    *barre,
                                               uint16_t     discretisation,
                                               double       position,
                                               Barre_Info_EF *infos,
                                               double       mx,
                                               double      *ma,
                                               double      *mb)
                                       __attribute__((__warn_unused_result__));

bool EF_charge_barre_ponctuelle_def_ang_iso_y (EF_Barre    *barre,
                                               uint16_t     discretisation,
                                               double       position,
                                               double       fz,
                                               double       my,
                                               double      *phia,
                                               double      *phib)
                                       __attribute__((__warn_unused_result__));
bool EF_charge_barre_ponctuelle_def_ang_iso_z (EF_Barre    *barre,
                                               uint16_t     discretisation,
                                               double       position,
                                               double       fy,
                                               double       mz,
                                               double      *phia,
                                               double      *phib)
                                       __attribute__((__warn_unused_result__));

bool EF_charge_barre_ponctuelle_fonc_rx       (Fonction    *fonction,
                                               EF_Barre    *barre,
                                               uint16_t     discretisation,
                                               double       a,
                                               double       max,
                                               double       mbx)
                                       __attribute__((__warn_unused_result__));
bool EF_charge_barre_ponctuelle_fonc_ry       (Fonction    *f_rotation,
                                               Fonction    *f_deform,
                                               EF_Barre    *barre,
                                               uint16_t     discretisation,
                                               double       position,
                                               double       fz,
                                               double       my,
                                               double       may,
                                               double       mby)
                                       __attribute__((__warn_unused_result__));
bool EF_charge_barre_ponctuelle_fonc_rz       (Fonction *f_rotation,
                                               Fonction *f_deform,
                                               EF_Barre *barre,
                                               uint16_t  discretisation,
                                               double    a,
                                               double    fy,
                                               double    mz,
                                               double    maz,
                                               double    mbz)
                                       __attribute__((__warn_unused_result__));

bool EF_charge_barre_ponctuelle_n             (Fonction *fonction,
                                               EF_Barre *barre,
                                               uint16_t  discretisation,
                                               double    a,
                                               double    fax,
                                               double    fbx)
                                       __attribute__((__warn_unused_result__));

bool EF_charge_barre_ponctuelle_enleve_barres (
       Charge                 *charge,
       std::list <EF_Barre *> *barres,
       Projet                 *p)
                                       __attribute__((__warn_unused_result__));

bool EF_charge_barre_ponctuelle_free          (Charge      *charge)
                                       __attribute__((__warn_unused_result__));

#endif

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
