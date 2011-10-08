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

#include "config.h"
#include <M3d++.hpp>

extern "C" {

#include "common_projet.h"
#include "common_erreurs.h"
#include "common_m3d.hpp"
#include <gtk/gtk.h>
#include <libintl.h>
#include <locale.h>

struct SGlobalData
{
    CM3dScene *scene;
    CM3dCamera *camera;
};

int m3d_init(Projet *projet __attribute__((unused)))
/* Description : Initialise l'affichage graphique de la structure.
 * Paramètres : Aucun
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet == NULL)
 *           -2 en cas d'erreur d'allocation mémoire
 */
{
    // Trivial
    List_Gtk_m3d *m3d;
    BUGMSG(projet, -1, "m3d_init\n");
    
    projet->list_gtk.m3d = (List_Gtk_m3d*)malloc(sizeof(List_Gtk_m3d));
    BUGMSG(projet->list_gtk.m3d, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "m3d_init");
    M3d_init();
    m3d = (List_Gtk_m3d*)projet->list_gtk.m3d;
    m3d->drawing = gtk_drawing_area_new();
    
    return 0;
}

gboolean m3d_configure_event(GtkWidget *drawing __attribute__((unused)), GdkEventConfigure * ev, gpointer *data2)
{
    SGlobalData *data = (SGlobalData*)data2;
    data->camera->set_size_of_window (ev->width, ev->height);
    
    if (data->camera->get_window_height () < data->camera->get_window_width ())
        data->camera->set_d (data->camera->get_window_height () / (2 * tan (data->camera->get_angle () / 2)));
    else
        data->camera->set_d (data->camera->get_window_width () / (2 * tan (data->camera->get_angle () / 2)));
    data->scene->rendering (data->camera);
    return FALSE;
}

gboolean m3d_draw(GtkWidget *drawing, GdkEventExpose* ev __attribute__((unused)), gpointer *data)
{
    cairo_t *context = NULL;
    SGlobalData *data2 = (SGlobalData*)data;

    // Rendu de l'image 3D dans le widget Zone-dessin
    data2->scene->show_to_GtkDrawingarea (drawing, data2->camera);
    
    context = gdk_cairo_create (gtk_widget_get_window(drawing));
    cairo_select_font_face (context, "URW Chancery L", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size (context, 18);
    return FALSE;
}
              
void m3d_free(Projet *projet)
/* Description : Libère l'espace mémoire alloué pour l'affichage graphique de la structure.
 * Paramètres : Aucun
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet == NULL) ou
 *             (projet->list_gtk.m3d == NULL)
 */
{
    // Trivial
    BUGMSG(projet, , "m3d_free\n");
    BUGMSG(projet->list_gtk.m3d, , "m3d_free\n");
    
    free(projet->list_gtk.m3d);
    projet->list_gtk.m3d = NULL;
    
    return;
}


}
