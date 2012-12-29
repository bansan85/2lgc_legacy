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

#ifndef __COMMON_GTK_H
#define __COMMON_GTK_H

#include "config.h"
#include "common_projet.h"
#include <gtk/gtk.h>

#define GTK_COMMON_SPINBUTTON_AS_UINT(spinbutton) ((unsigned int)round(gtk_spin_button_get_value(spinbutton)))

gboolean common_gtk_treeview_button_press_unselect(GtkTreeView *widget, GdkEvent *event, Projet *projet);
double common_gtk_text_buffer_double(GtkTextBuffer *textbuffer, double val_min,
  gboolean min_include, double val_max, gboolean max_include);
uint common_gtk_text_buffer_uint(GtkTextBuffer *textbuffer, uint val_min, gboolean min_include,
  uint val_max, gboolean max_include);
void common_gtk_render_double(GtkTreeViewColumn *tree_column, GtkCellRenderer *cell, GtkTreeModel *tree_model, GtkTreeIter *iter, gpointer data);

#endif
