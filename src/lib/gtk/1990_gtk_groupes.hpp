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

#ifndef __1990_GTK_GROUPES_H
#define __1990_GTK_GROUPES_H

#include "config.h"
#ifdef ENABLE_GTK
#include <gtk/gtk.h>

#include "common_projet.hpp"

bool _1990_gtk_insert_dispo           (Projet        *p,
                                       void          *data,
                                       Niveau_Groupe *niveau)
                                       __attribute__((__warn_unused_result__));
bool _1990_gtk_groupes_affiche_niveau (Projet        *p,
                                       uint16_t       niveau)
                                       __attribute__((__warn_unused_result__));
void _1990_gtk_groupes                (Projet        *p);

#endif

#endif

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
