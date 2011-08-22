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

#ifndef __COMMON_TOOLTIP_H
#define __COMMON_TOOLTIP_H

#include "config.h"

#ifdef ENABLE_GTK
#include <gtk/gtk.h>

GtkWindow* common_tooltip_generation(const char *nom);
void wrapped_label_size_allocate_callback(GtkWidget *label, GtkAllocation *allocation, gpointer data __attribute__((unused)));
#endif

#endif
