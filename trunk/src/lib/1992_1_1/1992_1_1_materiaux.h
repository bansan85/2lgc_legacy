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

#ifndef __1992_1_1_MATERIAUX_H
#define __1992_1_1_MATERIAUX_H

#include "config.h"
#include "common_projet.h"

#define COEFFICIENT_NU_BETON 0.2

double _1992_1_1_materiaux_fckcube   (double fck)
                                       __attribute__((__warn_unused_result__));
double _1992_1_1_materiaux_fcm       (double fck)
                                       __attribute__((__warn_unused_result__));
double _1992_1_1_materiaux_fctm      (double fck,
                                      double fcm)
                                       __attribute__((__warn_unused_result__));
double _1992_1_1_materiaux_fctk_0_05 (double fctm)
                                       __attribute__((__warn_unused_result__));
double _1992_1_1_materiaux_fctk_0_95 (double fctm)
                                       __attribute__((__warn_unused_result__));
double _1992_1_1_materiaux_ecm       (double fcm)
                                       __attribute__((__warn_unused_result__));
double _1992_1_1_materiaux_ec1       (double fcm)
                                       __attribute__((__warn_unused_result__));
double _1992_1_1_materiaux_ecu1      (double fcm,
                                      double fck)
                                       __attribute__((__warn_unused_result__));
double _1992_1_1_materiaux_ec2       (double fck)
                                       __attribute__((__warn_unused_result__));
double _1992_1_1_materiaux_ecu2      (double fck)
                                       __attribute__((__warn_unused_result__));
double _1992_1_1_materiaux_ec3       (double fck)
                                       __attribute__((__warn_unused_result__));
double _1992_1_1_materiaux_ecu3      (double fck)
                                       __attribute__((__warn_unused_result__));
double _1992_1_1_materiaux_n         (double fck)
                                       __attribute__((__warn_unused_result__));
double _1992_1_1_materiaux_gnu       (double ecm,
                                      double nu)
                                       __attribute__((__warn_unused_result__));

EF_Materiau *_1992_1_1_materiaux_ajout (Projet     *p,
                                        const char *nom,
                                        Flottant    fck)
                                       __attribute__((__warn_unused_result__));

gboolean _1992_1_1_materiaux_modif (Projet      *p,
                                    EF_Materiau *materiau,
                                    char        *nom,
                                    Flottant     fck,
                                    Flottant     fckcube,
                                    Flottant     fcm,
                                    Flottant     fctm,
                                    Flottant     fctk_0_05,
                                    Flottant     fctk_0_95,
                                    Flottant     ecm,
                                    Flottant     ec1,
                                    Flottant     ecu1,
                                    Flottant     ec2,
                                    Flottant     ecu2,
                                    Flottant     n,
                                    Flottant     ec3,
                                    Flottant     ecu3,
                                    Flottant     nu)
                                       __attribute__((__warn_unused_result__));

char *_1992_1_1_materiaux_get_description (EF_Materiau *materiau)
                                       __attribute__((__warn_unused_result__));

#endif

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
