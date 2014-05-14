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

#ifndef __EF_CHARGE_NOEUD_H
#define __EF_CHARGE_NOEUD_H

#include "config.h"
#include "common_projet.hpp"

Charge *EF_charge_noeud_ajout          (Projet                 *p,
                                        Action                 *action,
                                        std::list <EF_Noeud *> *noeud,
                                        Flottant                fx,
                                        Flottant                fy,
                                        Flottant                fz,
                                        Flottant                mx,
                                        Flottant                my,
                                        Flottant                mz,
                                        const char             *nom)
                                       __attribute__((__warn_unused_result__));

char *EF_charge_noeud_description      (Charge                 *charge)
                                       __attribute__((__warn_unused_result__));
gboolean EF_charge_noeud_enleve_noeuds (Charge                 *charge,
                                        std::list <EF_Noeud *> *noeuds,
                                        Projet                 *p)
                                       __attribute__((__warn_unused_result__));
gboolean EF_charge_noeud_free          (Charge                 *charge)
                                       __attribute__((__warn_unused_result__));

#endif

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
