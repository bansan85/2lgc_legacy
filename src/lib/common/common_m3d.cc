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
#ifdef ENABLE_GTK
#include <M3d++.hpp>

extern "C" {

#include <gtk/gtk.h>
#include <libintl.h>
#include <locale.h>
#include <string.h>
#include "common_projet.h"
#include "common_erreurs.h"
#include "EF_noeud.h"
#include "1992_1_1_barres.h"
#include "common_m3d.hpp"

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
    
    M3d_init();
    m3d = &projet->list_gtk.m3d;
    m3d->drawing = gtk_drawing_area_new();
    m3d->data = malloc(sizeof(struct SGlobalData));
    BUGMSG(m3d->data, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "m3d_init");
    memset(m3d->data, 0, sizeof(struct SGlobalData));
    
    global_data = (struct SGlobalData*)m3d->data;
    global_data->scene = new CM3dScene();
    global_data->scene->reverse_y_axis();
    global_data->scene->show_repere(true, 1.1);
    global_data->scene->set_ambient_light(1.);
    global_data->scene->set_show_type(SOLID);
    
    light = new CM3dLight("lumiere 1", DIFFUS, 1);
    light->set_position(10., 20., -20.);
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
    
    m3d = &projet->list_gtk.m3d;
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
    BUGMSG(projet->ef_donnees.noeuds, -1, "m3d_genere_graphique\n");
    m3d = &projet->list_gtk.m3d;
    BUGMSG(m3d->data, -1, "m3d_genere_graphique\n");
    vue = (struct SGlobalData*)m3d->data;
    
    list_mvfront(projet->ef_donnees.noeuds);
    if (list_size(projet->ef_donnees.noeuds) != 0)
    {
        do
        {
            EF_Noeud    *noeud = (EF_Noeud*)list_curr(projet->ef_donnees.noeuds);
            CM3dObject  *cube;
            char        tmp[256];
            
            sprintf(tmp, "noeud_%d", noeud->numero);
            cube = M3d_cube_new (tmp, .1);
            
            cube->set_ambient_reflexion (1.);
            cube->set_smooth(GOURAUD);
            vue->scene->add_object(cube);
            cube->set_position(noeud->position.x, noeud->position.y, noeud->position.z);
        }
        while (list_mvnext(projet->ef_donnees.noeuds) != NULL);
    }
    
    list_mvfront(projet->beton.barres);
    if (list_size(projet->beton.barres) != 0)
    {
        do
        {
            Beton_Barre *barre = (Beton_Barre*)list_curr(projet->beton.barres);
            char        tmp[256];
            Beton_Section_Rectangulaire *section_tmp = (Beton_Section_Rectangulaire*)barre->section;
            
            sprintf(tmp, "barre_%d", barre->numero);
            
            switch (section_tmp->type)
            {
                case BETON_SECTION_RECTANGULAIRE :
                {
                    double  longueur = EF_noeuds_distance(barre->noeud_debut, barre->noeud_fin);
                    double  y, z;
                    CM3dObject  *bas, *haut, *gauche, *droite, *tout;
                    
                    droite = M3d_plan_new("", longueur, section_tmp->hauteur, 1);
                    droite->rotations(180., 0., 0.);
                    droite->set_position(0., -section_tmp->largeur/2., 0.);
                    
                    gauche = M3d_plan_new("", longueur, section_tmp->hauteur, 1);
                    gauche->set_position(0., section_tmp->largeur/2., 0.);
                    
                    bas = M3d_plan_new("", longueur, section_tmp->largeur, 1);
                    bas->rotations(90., 180., 0.);
                    bas->set_position(0., 0., -section_tmp->hauteur/2.);
                    
                    haut = M3d_plan_new("", longueur, section_tmp->largeur, 1);
                    haut->rotations(90., 0., 0.);
                    haut->set_position(0., 0., section_tmp->hauteur/2.);
                    
                    tout = M3d_object_new_group(tmp, droite, gauche, bas, haut, NULL);
                    switch(barre->numero)
                    {
                        case 0:
                        {
                            tout->set_color(255, 0, 0);
                            break;
                        }
                        case 1:
                        {
                            tout->set_color(0, 255, 0);
                            break;
                        }
                        case 2:
                        {
                            tout->set_color(0, 0, 255);
                            break;
                        }
                        default : break;

                    }
                    tout->set_smooth(GOURAUD);
                    _1992_1_1_barres_angle_rotation(barre, &y, &z);
                    tout->rotations(0., -y/M_PI*180., z/M_PI*180.);
                    tout->set_position((barre->noeud_debut->position.x+barre->noeud_fin->position.x)/2., (barre->noeud_debut->position.y+barre->noeud_fin->position.y)/2., (barre->noeud_debut->position.z+barre->noeud_fin->position.z)/2.);
                    tout->set_ambient_reflexion(0.8);
                    
                    vue->scene->add_object(tout);
                    break;
                    
                }
                case BETON_SECTION_T :
                {
                    Beton_Section_T *section = (Beton_Section_T*)barre->section;
                    
                    double  longueur = EF_noeuds_distance(barre->noeud_debut, barre->noeud_fin);
                    double  y, z;
                    double  lt = section->largeur_table;
                    double  la = section->largeur_ame;
                    double  ht = section->hauteur_table;
                    double  ha = section->hauteur_ame;
                    double  cdgh = (lt*ht*ht/2.+la*ha*(ht+ha/2.))/(lt*ht+la*ha);
                    double  cdgb = (ht+ha)-cdgh;
                    
                    CM3dObject  *ame_inf, *ame_droite, *ame_gauche, *dalle_bas_droite, *dalle_bas_gauche, *dalle_droite, *dalle_gauche, *dalle_sup, *tout;
                    
                    ame_inf = M3d_plan_new("", longueur, la, 1);
                    ame_inf->rotations(90., 180., 0.);
                    ame_inf->set_position(0., 0., -cdgb);
                    
                    ame_droite = M3d_plan_new("", longueur, ha, 1);
                    ame_droite->rotations(180., 0., 0.);
                    ame_droite->set_position(0., -la/2., -cdgb+ha/2.);
                    
                    ame_gauche = M3d_plan_new("", longueur, ha, 1);
                    ame_gauche->set_position(0., la/2., -cdgb+ha/2.);
                    
                    dalle_bas_gauche = M3d_plan_new("", longueur, (lt-la)/2., 1);
                    dalle_bas_gauche->rotations(90., 0., 0.);
                    dalle_bas_gauche->set_position(0., la/2.+(lt-la)/4., -cdgb+ha);
                    
                    dalle_bas_droite = M3d_plan_new("", longueur, (lt-la)/2., 1);
                    dalle_bas_droite->rotations(90., 180., 0.);
                    dalle_bas_droite->set_position(0., -la/2.-(lt-la)/4., -cdgb+ha);
                    
                    dalle_droite = M3d_plan_new("", longueur, ht, 1);
                    dalle_droite->rotations(180., 0., 0.);
                    dalle_droite->set_position(0., -lt/2., -cdgb+ha+ht/2.);
                    
                    dalle_gauche = M3d_plan_new("", longueur, ht, 1);
                    dalle_gauche->set_position(0., lt/2., -cdgb+ha+ht/2.);
                    
                    dalle_sup = M3d_plan_new("", longueur, lt, 1);
                    dalle_sup->rotations(90., 0., 0.);
                    dalle_sup->set_position(0., 0, -cdgb+ha+ht);
                    
                    tout = M3d_object_new_group(tmp, ame_inf, ame_droite, ame_gauche, dalle_bas_droite, dalle_bas_gauche, dalle_droite, dalle_gauche, dalle_sup, NULL);
                    switch(barre->numero)
                    {
                        case 0:
                        {
                            tout->set_color(255, 0, 0);
                            break;
                        }
                        case 1:
                        {
                            tout->set_color(0, 255, 0);
                            break;
                        }
                        case 2:
                        {
                            tout->set_color(0, 0, 255);
                            break;
                        }
                        default : break;

                    }
                    tout->set_ambient_reflexion(0.8);
                    tout->set_smooth(GOURAUD);
                    _1992_1_1_barres_angle_rotation(barre, &y, &z);
                    tout->rotations(0., -y/M_PI*180., z/M_PI*180.);
                    tout->set_position((barre->noeud_debut->position.x+barre->noeud_fin->position.x)/2., (barre->noeud_debut->position.y+barre->noeud_fin->position.y)/2., (barre->noeud_debut->position.z+barre->noeud_fin->position.z)/2.);
                    
                    vue->scene->add_object(tout);
                    break;
                    
                }
                case BETON_SECTION_CARRE :
                {
                    Beton_Section_Carre *section = (Beton_Section_Carre*)barre->section;
                    
                    double  longueur = EF_noeuds_distance(barre->noeud_debut, barre->noeud_fin);
                    double  y, z;
                    
                    CM3dObject  *bas, *haut, *gauche, *droite, *tout;
                    
                    droite = M3d_plan_new("", longueur, section->cote, 1);
                    droite->rotations(180., 0., 0.);
                    droite->set_position(0., -section->cote/2., 0.);
                    
                    gauche = M3d_plan_new("", longueur, section->cote, 1);
                    gauche->set_position(0., section->cote/2., 0.);
                    
                    bas = M3d_plan_new("", longueur, section->cote, 1);
                    bas->rotations(90., 180., 0.);
                    bas->set_position(0., 0., -section->cote/2.);
                    
                    haut = M3d_plan_new("", longueur, section->cote, 1);
                    haut->rotations(90., 0., 0.);
                    haut->set_position(0., 0., section->cote/2.);
                    
                    tout = M3d_object_new_group(tmp, droite, gauche, bas, haut, NULL);
                    switch(barre->numero)
                    {
                        case 0:
                        {
                            tout->set_color(255, 0, 0);
                            break;
                        }
                        case 1:
                        {
                            tout->set_color(0, 255, 0);
                            break;
                        }
                        case 2:
                        {
                            tout->set_color(0, 0, 255);
                            break;
                        }
                        default : break;

                    }
                    tout->set_ambient_reflexion(0.8);
                    tout->set_smooth(GOURAUD);
                    _1992_1_1_barres_angle_rotation(barre, &y, &z);
                    tout->rotations(0., -y/M_PI*180., z/M_PI*180.);
                    tout->set_position((barre->noeud_debut->position.x+barre->noeud_fin->position.x)/2., (barre->noeud_debut->position.y+barre->noeud_fin->position.y)/2., (barre->noeud_debut->position.z+barre->noeud_fin->position.z)/2.);
                    
                    vue->scene->add_object(tout);
                    break;
                    
                }
                case BETON_SECTION_CIRCULAIRE :
                {
                    Beton_Section_Circulaire *section = (Beton_Section_Circulaire*)barre->section;
                    
                    double  longueur = EF_noeuds_distance(barre->noeud_debut, barre->noeud_fin);
                    double  y, z;
                    
                    CM3dObject  *tout;
                    
                    tout = M3d_cylindre_new(tmp, section->diametre/2., longueur, 12);
                    tout->rotations(0., 0., 90.);
                    switch(barre->numero)
                    {
                        case 0:
                        {
                            tout->set_color(255, 0, 0);
                            break;
                        }
                        case 1:
                        {
                            tout->set_color(0, 255, 0);
                            break;
                        }
                        case 2:
                        {
                            tout->set_color(0, 0, 255);
                            break;
                        }
                        default : break;

                    }
                    tout->set_ambient_reflexion(0.8);
                    tout->set_smooth(GOURAUD);
                    _1992_1_1_barres_angle_rotation(barre, &y, &z);
                    tout->rotations(0., -y/M_PI*180., z/M_PI*180.);
                    tout->set_position((barre->noeud_debut->position.x+barre->noeud_fin->position.x)/2., (barre->noeud_debut->position.y+barre->noeud_fin->position.y)/2., (barre->noeud_debut->position.z+barre->noeud_fin->position.z)/2.);
                    
                    vue->scene->add_object(tout);
                    break;
                    
                }
                default :
                {
                    BUGMSG(0, 0., "m3d_genere_graphique\n");
                }
            }
            
        }
        while (list_mvnext(projet->beton.barres) != NULL);
    }
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
    
    m3d = &projet->list_gtk.m3d;
    free(m3d->data);
    m3d->data = NULL;
    
    return;
}


}

#endif
