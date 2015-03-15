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

#ifndef __EF_GTK_CHARGE_BARRE_REPARTIE_UNIFORME_H
#define __EF_GTK_CHARGE_BARRE_REPARTIE_UNIFORME_H

#include "config.h"
#ifdef ENABLE_GTK
#include <gtk/gtk.h>

#include "common_projet.hpp"

bool EF_gtk_charge_barre_repartie_uniforme (Projet *p,
                                            Action *action_defaut,
                                            Charge *charge)
                                       __attribute__((__warn_unused_result__));

#endif

#endif

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
