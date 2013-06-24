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

#include "config.h"
#ifdef ENABLE_GTK
#include <gtk/gtk.h>


#ifdef __cplusplus
#include <M3d++.hpp>

typedef struct
{
    CM3dScene *scene;
    CM3dCamera *camera;
} SGlobalData;


extern "C" {
#endif

#include "common_projet.h"

gboolean m3d_init(Projet *projet) __attribute__((__warn_unused_result__));

gboolean m3d_draw(GtkWidget *drawing, GdkEventExpose* ev, gpointer data);
gboolean m3d_configure_event(GtkWidget *drawing, GdkEventConfigure * ev, gpointer data);

gboolean m3d_key_press(GtkWidget *widget, GdkEventKey *event, Projet *projet);

gboolean m3d_camera_axe_x_z_y(Projet *projet) __attribute__((__warn_unused_result__));
gboolean m3d_camera_axe_x_z__y(Projet *projet) __attribute__((__warn_unused_result__));
gboolean m3d_camera_axe_y_z_x(Projet *projet) __attribute__((__warn_unused_result__));
gboolean m3d_camera_axe_y_z__x(Projet *projet) __attribute__((__warn_unused_result__));
gboolean m3d_camera_axe_x_y_z(Projet *projet) __attribute__((__warn_unused_result__));
gboolean m3d_camera_axe_x_y__z(Projet *projet) __attribute__((__warn_unused_result__));
gboolean m3d_actualise_graphique(Projet *projet, GList *noeuds, GList *barres) __attribute__((__warn_unused_result__));
gboolean m3d_rafraichit(Projet *projet) __attribute__((__warn_unused_result__));

void* m3d_noeud(void *donnees_m3d, EF_Noeud *noeud) __attribute__((__warn_unused_result__));
void m3d_noeud_free(void *donnees_m3d, EF_Noeud *noeud);
gboolean m3d_barre(void *donnees_m3d, Beton_Barre *barre);
void m3d_barre_free(void *donnees_m3d, Beton_Barre *barre);

gboolean m3d_free(Projet *projet) __attribute__((__warn_unused_result__));

#ifdef __cplusplus
}
#endif

#endif
#endif
