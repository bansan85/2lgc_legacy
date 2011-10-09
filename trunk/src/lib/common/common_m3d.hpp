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

#ifndef __COMMON_SHOW_H
#define __COMMON_SHOW_H

#include <gtk/gtk.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "common_projet.h"

typedef struct __List_Gtk_m3d
{
    GtkWidget   *drawing;
    gpointer    data;
} List_Gtk_m3d;


int m3d_init(Projet *projet);

gboolean m3d_draw(GtkWidget *drawing, GdkEventExpose* ev, gpointer *data);
gboolean m3d_configure_event(GtkWidget *drawing, GdkEventConfigure * ev, gpointer *data);

int m3d_camera_axe_x_z(Projet *projet);
int m3d_genere_graphique(Projet *projet);

void m3d_free(Projet *projet);

#ifdef __cplusplus
}
#endif

#endif
