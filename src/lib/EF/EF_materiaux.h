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

#ifndef __EF_MATERIAUX_H
#define __EF_MATERIAUX_H

#include "config.h"
#include "common_projet.h"

gboolean EF_materiaux_init            (Projet      *p)
                                       __attribute__((__warn_unused_result__));

gboolean EF_materiaux_insert          (Projet      *p,
                                       EF_Materiau *materiau)
                                       __attribute__((__warn_unused_result__));
gboolean EF_materiaux_repositionne    (Projet      *p,
                                       EF_Materiau *materiau)
                                       __attribute__((__warn_unused_result__));
EF_Materiau *EF_materiaux_cherche_nom (Projet      *p,
                                       const char  *nom,
                                       gboolean     critique)
                                       __attribute__((__warn_unused_result__));
char *EF_materiaux_get_description    (EF_Materiau *materiau)
                                       __attribute__((__warn_unused_result__));

Flottant EF_materiaux_E               (EF_Materiau *materiau)
                                       __attribute__((__warn_unused_result__));
Flottant EF_materiaux_G               (EF_Materiau *materiau,
                                       gboolean     nu_null)
                                       __attribute__((__warn_unused_result__));

gboolean EF_materiaux_supprime        (EF_Materiau *materiau,
                                       Projet      *p)
                                       __attribute__((__warn_unused_result__));
gboolean EF_materiaux_free            (Projet      *p)
                                       __attribute__((__warn_unused_result__));

#endif

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
