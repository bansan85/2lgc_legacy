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
#include "EF_noeud.h"
#include "common_m3d.hpp"
#include <gtk/gtk.h>
#include <libintl.h>
#include <locale.h>
#include <string.h>

struct SGlobalData
{
    CM3dScene *scene;
    CM3dCamera *camera;
};

int m3d_init(Projet *projet)
/* Description : Initialise l'affichage graphique de la structure.
 * Paramètres : Projet *projet : la variable projet
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet == NULL)
 *           -2 en cas d'erreur d'allocation mémoire
 */
{
    // Trivial
    List_Gtk_m3d        *m3d;
    struct SGlobalData  *global_data;
    CM3dLight           *light;
    
    BUGMSG(projet, -1, "m3d_init\n");
    
    projet->list_gtk.m3d = (List_Gtk_m3d*)malloc(sizeof(List_Gtk_m3d));
    BUGMSG(projet->list_gtk.m3d, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "m3d_init");
    M3d_init();
    m3d = (List_Gtk_m3d*)projet->list_gtk.m3d;
    m3d->drawing = gtk_drawing_area_new();
    m3d->data = malloc(sizeof(struct SGlobalData));
    BUGMSG(m3d->data, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "m3d_init");
    memset(m3d->data, 0, sizeof(struct SGlobalData));
    
    global_data = (struct SGlobalData*)m3d->data;
    global_data->scene = new CM3dScene();
    global_data->scene->show_repere(true, 1.1);
    global_data->scene->set_ambient_light(1.);
    global_data->scene->set_show_type(SOLID);
    
    light = new CM3dLight("lumiere 1", DIFFUS, 1);
    light->set_position(100., 200., -200.);
    global_data->scene->add_light(light);
    
    g_signal_connect(m3d->drawing, "draw", G_CALLBACK(m3d_draw), global_data);
    g_signal_connect(m3d->drawing, "configure-event", G_CALLBACK(m3d_configure_event), global_data);
    
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
              
int m3d_camera_axe_x_z(Projet *projet)
/* Description : Positionne la caméra pour voir toute la structure dans le plan xz
 * Paramètres : Projet *projet
 * Valeur renvoyée :
 *   Succès : 0. Ne fait rien si (list_size(projet->ef_donnees.noeuds) <= 1)
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet == NULL) ou
 *             (projet->list_gtk.m3d == NULL) ou
 *             (projet->ef_donnees.noeuds == NULL) ou
 *             (list_size(projet->ef_donnees.noeuds) <= 1)
 *             (m3d->data == NULL)
 *           -3 en cas d'erreur due à une fonction interne
 */
{
    List_Gtk_m3d        *m3d;
    struct SGlobalData  *vue;
    double              x_min, x_max, z_min, z_max, x, y, z;
    EF_Noeud            *noeud;
    
    BUGMSG(projet, -1, "m3d_camera_axe_x_z\n");
    BUGMSG(projet->list_gtk.m3d, -1, "m3d_camera_axe_x_z\n");
    BUGMSG(projet->ef_donnees.noeuds, -1, "m3d_camera_axe_x_z\n");
    if (list_size(projet->ef_donnees.noeuds) <= 1)
        return 0;
    BUG(EF_noeuds_min_max(projet, &x_min, &x_max, NULL, NULL, &z_min, &z_max) == 0, -3);
    
    x = (x_min+x_max)/2.;
    z = (z_min+z_max)/2.;
    
    list_mvfront(projet->ef_donnees.noeuds);
    noeud = (EF_Noeud*)list_curr(projet->ef_donnees.noeuds);
    y = noeud->position.y-sqrt((noeud->position.x-x)*(noeud->position.x-x)+(noeud->position.z-z)*(noeud->position.z-z));
    
    while (list_mvnext(projet->ef_donnees.noeuds) != NULL)
    {
        noeud = (EF_Noeud*)list_curr(projet->ef_donnees.noeuds);
        y = MIN(y, noeud->position.y-sqrt((noeud->position.x-x)*(noeud->position.x-x)+(noeud->position.z-z)*(noeud->position.z-z)));
    }
    
    m3d = (List_Gtk_m3d*)projet->list_gtk.m3d;
    BUGMSG(m3d->data, -1, "m3d_camera_axe_x_z\n");
    vue = (struct SGlobalData*)m3d->data;
    
    if (vue->camera == NULL)
        vue->camera = new CM3dCamera (x, y*1.1, z, x, 0., z, 90, x_max-x_min, z_max-z_min);
    else
    {
        vue->camera->set_position(x, y, z);
        vue->camera->set_target(x, y+1, z);
    }
    
    return 0;
}

int m3d_genere_graphique(Projet *projet)
/* Description : Génère l'affichage 3D
 * Paramètres : Projet *projet
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet == NULL) ou
 *             (projet->list_gtk.m3d == NULL) ou
 *             (projet->ef_donnees.noeuds == NULL) ou
 *             (m3d->data == NULL)
 *           -3 en cas d'erreur due à une fonction interne
 */
{
    List_Gtk_m3d        *m3d;
    struct SGlobalData  *vue;
    
    BUGMSG(projet, -1, "m3d_genere_graphique\n");
    BUGMSG(projet->list_gtk.m3d, -1, "m3d_genere_graphique\n");
    BUGMSG(projet->ef_donnees.noeuds, -1, "m3d_genere_graphique\n");
    m3d = (List_Gtk_m3d*)projet->list_gtk.m3d;
    BUGMSG(m3d->data, -1, "m3d_genere_graphique\n");
    vue = (struct SGlobalData*)m3d->data;
    
    list_mvfront(projet->ef_donnees.noeuds);
    do
    {
        EF_Noeud    *noeud = (EF_Noeud*)list_curr(projet->ef_donnees.noeuds);
        CM3dObject  *cube;
        char        tmp[256];
        
        sprintf(tmp, "noeud%d", noeud->numero);
        cube = M3d_cube_new (tmp, .1);
        
        cube->set_ambient_reflexion (1.);
        cube->set_smooth(GOURAUD);
        vue->scene->add_object(cube);
        cube->set_position(noeud->position.x, noeud->position.y, noeud->position.z);
    }
    while (list_mvnext(projet->ef_donnees.noeuds) != NULL);
    return 0;
}

void m3d_free(Projet *projet)
/* Description : Libère l'espace mémoire alloué pour l'affichage graphique de la structure.
 * Paramètres : Projet *projet : la variable projet
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet == NULL) ou
 *             (projet->list_gtk.m3d == NULL)
 */
{
    // Trivial
    List_Gtk_m3d *m3d;
    
    BUGMSG(projet, , "m3d_free\n");
    BUGMSG(projet->list_gtk.m3d, , "m3d_free\n");
    
    m3d = (List_Gtk_m3d*)projet->list_gtk.m3d;
    free(m3d->data);
    free(projet->list_gtk.m3d);
    projet->list_gtk.m3d = NULL;
    
    return;
}


}
