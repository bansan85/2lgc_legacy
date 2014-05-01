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

#ifndef __1990_GTK_ACTIONS_H
#define __1990_GTK_ACTIONS_H

#include "config.h"
#ifdef ENABLE_GTK
#include <gtk/gtk.h>

#include "common_projet.hpp"

void _1990_gtk_nouvelle_action                         (GtkMenuItem *menuitem,
                                                        Projet      *p);
void _1990_gtk_nouvelle_charge_nodale                  (GtkMenuItem *menuitem,
                                                        Projet      *p);
void _1990_gtk_nouvelle_charge_barre_ponctuelle        (GtkMenuItem *menuitem,
                                                        Projet      *p);
void _1990_gtk_nouvelle_charge_barre_repartie_uniforme (GtkMenuItem *menuitem,
                                                        Projet      *p);

extern "C"
void _1990_gtk_actions_cursor_changed (GtkTreeView *tree_view,
                                       Projet      *p);

void _1990_gtk_actions (Projet *p);

#endif

#endif

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
