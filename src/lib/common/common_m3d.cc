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
#include "common_math.h"
#include "common_selection.h"
#include "EF_noeuds.h"
#include "1992_1_1_barres.h"
#include "common_m3d.hpp"

gboolean m3d_init(Projet *projet)
/* Description : Initialise l'affichage graphique de la structure.
 * Paramètres : Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL,
 *             en cas d'erreur d'allocation mémoire.
 */
{
    // Trivial
    Gtk_m3d     *m3d;
    SGlobalData *global_data;
    CM3dLight   *light;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    
    m3d = &projet->list_gtk.m3d;
    m3d->drawing = gtk_drawing_area_new();
    BUGMSG(m3d->data = malloc(sizeof(SGlobalData)), FALSE, gettext("Erreur d'allocation mémoire.\n"));
    memset(m3d->data, 0, sizeof(SGlobalData));
    
    global_data = (SGlobalData*)m3d->data;
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
    
    BUG(projet_init_graphique(projet), FALSE);
    
    return TRUE;
}


gboolean m3d_configure_event(GtkWidget *drawing, GdkEventConfigure *ev, gpointer *data2)
/* Description : Configuration de la caméra en fonction de la taille du composant graphique.
 * Paramètres : GtkWidget *drawing : composant graphique,
 *              GdkEventConfigure *ev : caractéristique de l'évènement,
 *              gpointer *data2 : données SGlobalData.
 * Valeur renvoyée : FALSE.
 */
{
    SGlobalData *data = (SGlobalData*)data2;
    data->camera->set_size_of_window(ev->width, ev->height);
    
    if (data->camera->get_window_height() < data->camera->get_window_width())
        data->camera->set_d(data->camera->get_window_height() / (2 * tan(data->camera->get_angle() / 2)));
    else
        data->camera->set_d(data->camera->get_window_width() / (2 * tan(data->camera->get_angle() / 2)));
    data->scene->rendering(data->camera);
    return FALSE;
}


gboolean m3d_draw(GtkWidget *drawing, GdkEventExpose* ev, gpointer *data)
/* Description : Rendu de l'image 3D dans le widget Zone-dessin.
 * Paramètres : GtkWidget *drawing : composant graphique,
 *              GdkEventConfigure *ev : caractéristique de l'évènement,
 *              gpointer *data2 : données SGlobalData.
 * Valeur renvoyée : FALSE.
 */
{
    cairo_t *context = NULL;
    SGlobalData *data2 = (SGlobalData*)data;

    data2->scene->show_to_GtkDrawingarea (drawing, data2->camera);
    
    context = gdk_cairo_create (gtk_widget_get_window(drawing));
    cairo_select_font_face (context, "URW Chancery L", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size (context, 18);
    return FALSE;
}


gboolean m3d_key_press(GtkWidget *widget, GdkEventKey *event, Projet *projet)
{
    SGlobalData *vue  = (SGlobalData*)projet->list_gtk.m3d.data;
    
    if (event->type == GDK_KEY_PRESS)
    {
        double  x1, y1, z1;
        double  x2, y2, z2;
        CM3dVertex  *vect;
        CM3dVertex S1;
        CM3dVertex som1_rotx;
        CM3dVertex som1_roty;
        
        switch (event->keyval)
        {
            case GDK_KEY_KP_Add :
            case GDK_KEY_plus :
            {
                vect = vue->camera->get_position();
                vect->get_coordinates(&x1, &y1, &z1);
                vect = vue->camera->get_target_vector();
                vect->get_coordinates(&x2, &y2, &z2);
                
                vue->camera->set_position(x1+x2, y1+y2, z1+z2);
                vect = vue->camera->get_target();
                vect->get_coordinates(&x1, &y1, &z1);
                vue->camera->set_target(x1+x2, y1+y2, z1+z2);
                
                BUG(m3d_rafraichit(projet), FALSE);
                
                break;
            }
            case GDK_KEY_KP_Subtract :
            case GDK_KEY_minus :
            {
                vect = vue->camera->get_position();
                vect->get_coordinates(&x1, &y1, &z1);
                vect = vue->camera->get_target_vector();
                vect->get_coordinates(&x2, &y2, &z2);
                
                vue->camera->set_position(x1-x2, y1-y2, z1-z2);
                vect = vue->camera->get_target();
                vect->get_coordinates(&x1, &y1, &z1);
                vue->camera->set_target(x1-x2, y1-y2, z1-z2);
                
                BUG(m3d_rafraichit(projet), FALSE);
                
                break;
            }
            case GDK_KEY_KP_Right :
            case GDK_KEY_Right :
            case GDK_KEY_d :
            case GDK_KEY_D :
            {
                S1.set_coordinates(1., 0., 0.);
                S1.y_rotate(&som1_roty, vue->camera->get_cosy(), vue->camera->get_siny());
                som1_roty.x_rotate(&som1_rotx, vue->camera->get_cosx(), vue->camera->get_sinx());
                som1_rotx.z_rotate(&S1, vue->camera->get_cosz(), vue->camera->get_sinz());
                S1.get_coordinates(&x2,&y2,&z2);
                
                vect = vue->camera->get_position();
                vect->get_coordinates(&x1,&y1,&z1);
                vue->camera->set_position(x1+x2, y1+y2, z1+z2);
                
                vect = vue->camera->get_target();
                vect->get_coordinates(&x1,&y1,&z1);
                vue->camera->set_target(x1+x2, y1+y2, z1+z2);
                
                BUG(m3d_rafraichit(projet), FALSE);
                
                break;
            }
            case GDK_KEY_KP_Left :
            case GDK_KEY_Left :
            case GDK_KEY_q :
            case GDK_KEY_Q :
            {
                S1.set_coordinates(-1., 0., 0.);
                S1.y_rotate(&som1_roty, vue->camera->get_cosy(), vue->camera->get_siny());
                som1_roty.x_rotate(&som1_rotx, vue->camera->get_cosx(), vue->camera->get_sinx());
                som1_rotx.z_rotate(&S1, vue->camera->get_cosz(), vue->camera->get_sinz());
                S1.get_coordinates(&x2,&y2,&z2);
                
                vect = vue->camera->get_position();
                vect->get_coordinates(&x1,&y1,&z1);
                vue->camera->set_position(x1+x2, y1+y2, z1+z2);
                
                vect = vue->camera->get_target();
                vect->get_coordinates(&x1,&y1,&z1);
                vue->camera->set_target(x1+x2, y1+y2, z1+z2);
                
                BUG(m3d_rafraichit(projet), FALSE);
                
                break;
            }
            case GDK_KEY_KP_Up :
            case GDK_KEY_Up :
            case GDK_KEY_z :
            case GDK_KEY_Z :
            {
                if (vue->scene->y_axis_is_inverted())
                    S1.set_coordinates(0., 1., 0.);
                else
                    S1.set_coordinates(0., -1., 0.);
                S1.y_rotate(&som1_roty, vue->camera->get_cosy(), vue->camera->get_siny());
                som1_roty.x_rotate(&som1_rotx, vue->camera->get_cosx(), vue->camera->get_sinx());
                som1_rotx.z_rotate(&S1, vue->camera->get_cosz(), vue->camera->get_sinz());
                S1.get_coordinates(&x2,&y2,&z2);
                
                vect = vue->camera->get_position();
                vect->get_coordinates(&x1,&y1,&z1);
                vue->camera->set_position(x1+x2, y1+y2, z1+z2);
                
                vect = vue->camera->get_target();
                vect->get_coordinates(&x1,&y1,&z1);
                vue->camera->set_target(x1+x2, y1+y2, z1+z2);
                
                BUG(m3d_rafraichit(projet), FALSE);
                
                break;
            }
            case GDK_KEY_KP_Down :
            case GDK_KEY_Down :
            case GDK_KEY_s :
            case GDK_KEY_S :
            {
                if (vue->scene->y_axis_is_inverted())
                    S1.set_coordinates(0., -1., 0.);
                else
                    S1.set_coordinates(0., 1., 0.);
                S1.y_rotate(&som1_roty, vue->camera->get_cosy(), vue->camera->get_siny());
                som1_roty.x_rotate(&som1_rotx, vue->camera->get_cosx(), vue->camera->get_sinx());
                som1_rotx.z_rotate(&S1, vue->camera->get_cosz(), vue->camera->get_sinz());
                S1.get_coordinates(&x2,&y2,&z2);
                
                vect = vue->camera->get_position();
                vect->get_coordinates(&x1,&y1,&z1);
                vue->camera->set_position(x1+x2, y1+y2, z1+z2);
                
                vect = vue->camera->get_target();
                vect->get_coordinates(&x1,&y1,&z1);
                vue->camera->set_target(x1+x2, y1+y2, z1+z2);
                
                BUG(m3d_rafraichit(projet), FALSE);
                
                break;
            }
            default :
            {
                break;
            }
        }
    }
   
    
    return FALSE;
}



gboolean m3d_camera_axe_x_z(Projet *projet)
/* Description : Positionne la caméra pour voir toute la structure dans le plan xz.
 * Paramètres : Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : TRUE. Ne fait rien si (list_size(projet->modele.noeuds) <= 1)
 *   Échec : FALSE :
 *             projet == NULL,
 *             projet->list_gtk.m3d == NULL,
 *             m3d->data == NULL,
 *             en cas d'erreur due à une fonction interne.
 */
{
    GList       *list_parcours;
    Gtk_m3d     *m3d;
    SGlobalData *vue;
    double      x_min, x_max, z_min, z_max, x, y, z;
    EF_Noeud    *noeud;
    EF_Point    *point;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    if (g_list_length(projet->modele.noeuds) <= 1)
        return TRUE;
    BUG(EF_noeuds_min_max(projet, &x_min, &x_max, NULL, NULL, &z_min, &z_max), FALSE);
    
    x = (x_min+x_max)/2.;
    z = (z_min+z_max)/2.;
    
    list_parcours = projet->modele.noeuds;
    noeud = (EF_Noeud *)list_parcours->data;
    BUG(point = EF_noeuds_renvoie_position(noeud), FALSE);
    y = common_math_get(point->y)-sqrt((common_math_get(point->x)-x)*(common_math_get(point->x)-x)+(common_math_get(point->z)-z)*(common_math_get(point->z)-z));
    free(point);
    
    list_parcours = g_list_next(list_parcours);
    while (list_parcours != NULL)
    {
        noeud = (EF_Noeud *)list_parcours->data;
        BUG(point = EF_noeuds_renvoie_position(noeud), FALSE);
        y = MIN(y, common_math_get(point->y)-sqrt((common_math_get(point->x)-x)*(common_math_get(point->x)-x)+(common_math_get(point->z)-z)*(common_math_get(point->z)-z)));
        free(point);
        list_parcours = g_list_next(list_parcours);
    }
    
    m3d = &projet->list_gtk.m3d;
    BUGMSG(m3d->data, FALSE, gettext("Paramètre %s incorrect.\n"), "m3d->data");
    vue = (SGlobalData*)m3d->data;
    
    if (vue->camera == NULL)
    {
        vue->camera = new CM3dCamera (x, y*1.1, z, x, 0., z, 90, (int)(x_max-x_min), (int)(z_max-z_min));
        vue->camera->rotation_on_axe_of_view(0);
    }
    else
    {
        vue->camera->set_position(x, y, z);
        vue->camera->set_target(x, y+1, z);
    }
    
    return TRUE;
}


gboolean m3d_actualise_graphique(Projet *projet, GList *noeuds, GList *barres)
/* Description : Met à jour l'affichage graphique en actualisant l'affichage des noeuds et barres passés en argument. Les listes contient une série de pointeur.
 * Paramètres : Projet *projet : la variable projet,
 *            : GList *noeuds : Liste de pointeurs vers les noeuds à actualiser,
 *            : GList *barres : Liste de pointeurs vers les barres à actualiser.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL,
 *             noeud == NULL,
 *             en cas d'erreur due à une fonction interne.
 */
{
    GList *noeuds_dep, *barres_dep;
    GList *list_parcours;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    
    BUG(_1992_1_1_barres_cherche_dependances(projet, NULL, noeuds, NULL, NULL, NULL, barres, &noeuds_dep, &barres_dep, NULL, FALSE, TRUE), FALSE);
    
    list_parcours = noeuds_dep;
    while (list_parcours != NULL)
    {
        BUG(m3d_noeud(&projet->list_gtk.m3d, (EF_Noeud *)list_parcours->data), FALSE);
        list_parcours = g_list_next(list_parcours);
    }
    g_list_free(noeuds_dep);
    
    list_parcours = barres_dep;
    while (list_parcours != NULL)
    {
        BUG(m3d_barre(&projet->list_gtk.m3d, (Beton_Barre *)list_parcours->data), FALSE);
        list_parcours = g_list_next(list_parcours);
    }
    g_list_free(barres_dep);
    
    return TRUE;
}


gboolean m3d_rafraichit(Projet *projet)
/* Description : Force le rafraichissement de l'affichage graphique.
 *               Nécessaire après l'utilisation d'une des fonctions d'actualisation de
 *                 l'affichage graphique.
 * Paramètres : Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL.
 */
{
    SGlobalData *vue;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    vue = (SGlobalData*)projet->list_gtk.m3d.data;
    // On force l'actualisation de l'affichage
    vue->scene->rendering(vue->camera);
    gtk_widget_queue_draw(projet->list_gtk.m3d.drawing);
    
    return TRUE;
}


void* m3d_noeud(void *donnees_m3d, EF_Noeud *noeud)
/* Description : Crée un noeud dans l'affichage graphique. Si le noeud existe, il est détruit au
 *               préalable.
 * Paramètres : void *donnees_m3d : données SGlobalData,
 *              EF_Noeud *noeud : noeud à ajouter ou à actualiser.
 * Valeur renvoyée :
 *   Succès : Pointeur vers le nouvel objet noeud.
 *   Échec : NULL :
 *             noeud == NULL,
 *             donnees_m3d == NULL,
 *             en cas d'erreur d'allocation mémoire.
 */
{
    CM3dObject  *cube;
    char        *nom;
    EF_Point    *point;
    SGlobalData *vue;
    
    BUGMSG(noeud, NULL, gettext("Paramètre %s incorrect.\n"), "noeud");
    BUGMSG(donnees_m3d, NULL, gettext("Paramètre %s incorrect.\n"), "donnees_m3d");
    
    BUGMSG(nom = g_strdup_printf("noeud %u", noeud->numero), NULL, gettext("Erreur d'allocation mémoire.\n"));
    BUG(point = EF_noeuds_renvoie_position(noeud), NULL);
    
    vue = (SGlobalData*)((Gtk_m3d *)donnees_m3d)->data;
    
    cube = vue->scene->get_object_by_name(nom);
    if (cube != NULL)
        vue->scene->remove_object(cube);
    
    cube = M3d_cube_new(nom, .1);
    cube->set_ambient_reflexion (1.);
    cube->set_smooth(GOURAUD);
    vue->scene->add_object(cube);
    cube->set_position(common_math_get(point->x), common_math_get(point->y), common_math_get(point->z));
    
    free(nom);
    free(point);
    
    return cube;
}


void m3d_noeud_free(void *donnees_m3d, EF_Noeud *noeud)
/* Description : Supprimer un noeud dans l'affichage graphique.
 * Paramètres : void *donnees_m3d : données SGlobalData,
 *              EF_Noeud *noeud : noeud à ajouter ou à actualiser.
 * Valeur renvoyée :
 *   Succès : Pointeur vers le nouvel objet noeud.
 *   Échec : NULL :
 *             noeud == NULL,
 *             donnees_m3d == NULL,
 *             en cas d'erreur d'allocation mémoire.
 */
{
    CM3dObject  *cube;
    char        *nom;
    SGlobalData *vue;
    
    BUGMSG(noeud, , gettext("Paramètre %s incorrect.\n"), "noeud");
    BUGMSG(donnees_m3d, , gettext("Paramètre %s incorrect.\n"), "donnees_m3d");
    
    BUGMSG(nom = g_strdup_printf("noeud %u", noeud->numero), , gettext("Erreur d'allocation mémoire.\n"));
    
    vue = (SGlobalData*)((Gtk_m3d *)donnees_m3d)->data;
    
    cube = vue->scene->get_object_by_name(nom);
    vue->scene->remove_object(cube);
    
    free(nom);
    
    return;
}


gboolean m3d_barre(void *donnees_m3d, Beton_Barre *barre)
/* Description : Crée une barre dans l'affichage graphique. Si la barre existe, elle est
 *               détruite au préalable.
 * Paramètres : void *donnees_m3d : données graphiques,
 *              Beton_Barre *barre : barre devant être représentée.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             donnees_m3d == NULL,
 *             barre == NULL,
 *             si la longueur de la barre est nulle,
 *             le type de barre est inconnu,
 *             en cas d'erreur d'allocation mémoire,
 *             en cas d'erreur d'une fonction interne.
 */
{
    Gtk_m3d     *m3d;
    SGlobalData *vue;
    CM3dObject  *objet;
    char        *tmp;
    CM3dObject  *tout;
    double      longueur;
    
    BUGMSG(donnees_m3d, FALSE, gettext("Paramètre %s incorrect.\n"), "donnees_m3d");
    BUGMSG(barre, FALSE, gettext("Paramètre %s incorrect.\n"), "barre");
    
    m3d = (Gtk_m3d *)donnees_m3d;
    vue = (SGlobalData*)m3d->data;
    
    // On supprime l'élément s'il existe déjà
    BUGMSG(tmp = g_strdup_printf("barre %u", barre->numero), FALSE, gettext("Erreur d'allocation mémoire.\n"));

    longueur = EF_noeuds_distance(barre->noeud_debut, barre->noeud_fin);
    BUG(!isnan(longueur), FALSE);
    
    objet = vue->scene->get_object_by_name(tmp);
    if (objet != NULL)
        vue->scene->remove_object(objet);
    if (ERREUR_RELATIVE_EGALE(longueur, 0.))
        return TRUE;
    
    switch (barre->section->type)
    {
        EF_Point *p_d, *p_f;
        case SECTION_RECTANGULAIRE :
        {
            double      y, z;
            CM3dObject  *bas, *haut, *gauche, *droite;
            Section_T   *section = (Section_T *)barre->section->data;
            
            droite = M3d_plan_new("", longueur, section->hauteur_retombee, 1);
            droite->rotations(180., 0., 0.);
            droite->set_position(0., -section->largeur_retombee/2., 0.);
            
            gauche = M3d_plan_new("", longueur, section->hauteur_retombee, 1);
            gauche->set_position(0., section->largeur_retombee/2., 0.);
            
            bas = M3d_plan_new("", longueur, section->largeur_retombee, 1);
            bas->rotations(90., 180., 0.);
            bas->set_position(0., 0., -section->hauteur_retombee/2.);
            
            haut = M3d_plan_new("", longueur, section->largeur_retombee, 1);
            haut->rotations(90., 0., 0.);
            haut->set_position(0., 0., section->hauteur_retombee/2.);
            
            tout = M3d_object_new_group(tmp, droite, gauche, bas, haut, NULL);
            
            delete droite;
            delete gauche;
            delete bas;
            delete haut;
            
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
            BUG(_1992_1_1_barres_angle_rotation(barre->noeud_debut, barre->noeud_fin, &y, &z), FALSE);
            tout->rotations(0., -y/M_PI*180., z/M_PI*180.);
            BUG(p_d = EF_noeuds_renvoie_position(barre->noeud_debut), FALSE);
            BUG(p_f = EF_noeuds_renvoie_position(barre->noeud_fin), FALSE);
            tout->set_position((common_math_get(p_d->x)+common_math_get(p_f->x))/2., (common_math_get(p_d->y)+common_math_get(p_f->y))/2., (common_math_get(p_d->z)+common_math_get(p_f->z))/2.);
            tout->set_ambient_reflexion(0.8);
            free(p_d);
            free(p_f);
            
            vue->scene->add_object(tout);
            
            break;
            
        }
        case SECTION_T :
        {
            Section_T *section = (Section_T *)barre->section->data;
            
            double  y, z;
            double  lt = section->largeur_table;
            double  lr = section->largeur_retombee;
            double  ht = section->hauteur_table;
            double  hr = section->hauteur_retombee;
            double  cdgh = (lt*ht*ht/2.+lr*hr*(ht+hr/2.))/(lt*ht+lr*hr);
            double  cdgb = (ht+hr)-cdgh;
            
            CM3dObject  *retombee_inf, *retombee_droite, *retombee_gauche, *dalle_bas_droite, *dalle_bas_gauche, *dalle_droite, *dalle_gauche, *dalle_sup;
            
            retombee_inf = M3d_plan_new("", longueur, lr, 1);
            retombee_inf->rotations(90., 180., 0.);
            retombee_inf->set_position(0., 0., -cdgb);
            
            retombee_droite = M3d_plan_new("", longueur, hr, 1);
            retombee_droite->rotations(180., 0., 0.);
            retombee_droite->set_position(0., -lr/2., -cdgb+hr/2.);
            
            retombee_gauche = M3d_plan_new("", longueur, hr, 1);
            retombee_gauche->set_position(0., lr/2., -cdgb+hr/2.);
            
            dalle_bas_gauche = M3d_plan_new("", longueur, (lt-lr)/2., 1);
            dalle_bas_gauche->rotations(90., 180., 0.);
            dalle_bas_gauche->set_position(0., lr/2.+(lt-lr)/4., -cdgb+hr);
            
            dalle_bas_droite = M3d_plan_new("", longueur, (lt-lr)/2., 1);
            dalle_bas_droite->rotations(90., 180., 0.);
            dalle_bas_droite->set_position(0., -lr/2.-(lt-lr)/4., -cdgb+hr);
            
            dalle_droite = M3d_plan_new("", longueur, ht, 1);
            dalle_droite->rotations(180., 0., 0.);
            dalle_droite->set_position(0., -lt/2., -cdgb+hr+ht/2.);
            
            dalle_gauche = M3d_plan_new("", longueur, ht, 1);
            dalle_gauche->set_position(0., lt/2., -cdgb+hr+ht/2.);
            
            dalle_sup = M3d_plan_new("", longueur, lt, 1);
            dalle_sup->rotations(90., 0., 0.);
            dalle_sup->set_position(0., 0, -cdgb+hr+ht);
            
            tout = M3d_object_new_group(tmp, retombee_inf, retombee_droite, retombee_gauche, dalle_bas_droite, dalle_bas_gauche, dalle_droite, dalle_gauche, dalle_sup, NULL);
            
            delete retombee_inf;
            delete retombee_droite;
            delete retombee_gauche;
            delete dalle_bas_droite;
            delete dalle_bas_gauche;
            delete dalle_droite;
            delete dalle_gauche;
            delete dalle_sup;
            
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
            BUG(_1992_1_1_barres_angle_rotation(barre->noeud_debut, barre->noeud_fin, &y, &z), FALSE);
            tout->rotations(0., -y/M_PI*180., z/M_PI*180.);
            BUG(p_d = EF_noeuds_renvoie_position(barre->noeud_debut), FALSE);
            BUG(p_f = EF_noeuds_renvoie_position(barre->noeud_fin), FALSE);
            tout->set_position((common_math_get(p_d->x)+common_math_get(p_f->x))/2., (common_math_get(p_d->y)+common_math_get(p_f->y))/2., (common_math_get(p_d->z)+common_math_get(p_f->z))/2.);
            free(p_d);
            free(p_f);
            
            vue->scene->add_object(tout);
            
            break;
            
        }
        case SECTION_CARREE :
        {
            Section_T   *section = (Section_T *)barre->section->data;
            double      y, z;
            CM3dObject  *bas, *haut, *gauche, *droite;
            
            droite = M3d_plan_new("", longueur, section->largeur_table, 1);
            droite->rotations(180., 0., 0.);
            droite->set_position(0., -section->largeur_table/2., 0.);
            
            gauche = M3d_plan_new("", longueur, section->largeur_table, 1);
            gauche->set_position(0., section->largeur_table/2., 0.);
            
            bas = M3d_plan_new("", longueur, section->largeur_table, 1);
            bas->rotations(90., 180., 0.);
            bas->set_position(0., 0., -section->largeur_table/2.);
            
            haut = M3d_plan_new("", longueur, section->largeur_table, 1);
            haut->rotations(90., 0., 0.);
            haut->set_position(0., 0., section->largeur_table/2.);
            
            tout = M3d_object_new_group(tmp, droite, gauche, bas, haut, NULL);
            
            delete droite;
            delete gauche;
            delete bas;
            delete haut;
            
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
            BUG(_1992_1_1_barres_angle_rotation(barre->noeud_debut, barre->noeud_fin, &y, &z), FALSE);
            tout->rotations(0., -y/M_PI*180., z/M_PI*180.);
            BUG(p_d = EF_noeuds_renvoie_position(barre->noeud_debut), FALSE);
            BUG(p_f = EF_noeuds_renvoie_position(barre->noeud_fin), FALSE);
            tout->set_position((common_math_get(p_d->x)+common_math_get(p_f->x))/2., (common_math_get(p_d->y)+common_math_get(p_f->y))/2., (common_math_get(p_d->z)+common_math_get(p_f->z))/2.);
            free(p_d);
            free(p_f);
            
            vue->scene->add_object(tout);
            
            break;
            
        }
        case SECTION_CIRCULAIRE :
        {
            Section_Circulaire *section = (Section_Circulaire *)barre->section->data;
            double  y, z;
            
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
            BUG(_1992_1_1_barres_angle_rotation(barre->noeud_debut, barre->noeud_fin, &y, &z), FALSE);
            tout->rotations(0., -y/M_PI*180., z/M_PI*180.);
            BUG(p_d = EF_noeuds_renvoie_position(barre->noeud_debut), FALSE);
            BUG(p_f = EF_noeuds_renvoie_position(barre->noeud_fin), FALSE);
            tout->set_position((common_math_get(p_d->x)+common_math_get(p_f->x))/2., (common_math_get(p_d->y)+common_math_get(p_f->y))/2., (common_math_get(p_d->z)+common_math_get(p_f->z))/2.);
            free(p_d);
            free(p_f);
            
            vue->scene->add_object(tout);
            
            break;
            
        }
        default :
        {
            BUGMSG(0, FALSE, gettext("Type de section %d inconnu.\n"), barre->section->type);
            break;
        }
    }
    
    free(tmp);
    
    return TRUE;
}


void m3d_barre_free(void *donnees_m3d, Beton_Barre *barre)
/* Description : Supprimer une barre dans l'affichage graphique.
 * Paramètres : void *donnees_m3d : données SGlobalData,
 *              EF_Noeud *noeud : noeud à ajouter ou à actualiser.
 * Valeur renvoyée :
 *   Succès : Pointeur vers le nouvel objet noeud.
 *   Échec : NULL :
 *             noeud == NULL,
 *             donnees_m3d == NULL,
 *             en cas d'erreur d'allocation mémoire.
 */
{
    CM3dObject  *cube;
    char        *nom;
    SGlobalData *vue;
    
    BUGMSG(barre, , gettext("Paramètre %s incorrect.\n"), "noeud");
    BUGMSG(donnees_m3d, , gettext("Paramètre %s incorrect.\n"), "donnees_m3d");
    
    BUGMSG(nom = g_strdup_printf("barre %u", barre->numero), , gettext("Erreur d'allocation mémoire.\n"));
    
    vue = (SGlobalData*)((Gtk_m3d *)donnees_m3d)->data;
    
    cube = vue->scene->get_object_by_name(nom);
    vue->scene->remove_object(cube);
    
    free(nom);
    
    return;
}


gboolean m3d_free(Projet *projet)
/* Description : Libère l'espace mémoire alloué pour l'affichage graphique de la structure.
 * Paramètres : Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL.
 */
{
    // Trivial
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    
    delete ((SGlobalData*)projet->list_gtk.m3d.data)->scene;
    delete ((SGlobalData*)projet->list_gtk.m3d.data)->camera;
    free(projet->list_gtk.m3d.data);
    projet->list_gtk.m3d.data = NULL;
    
    return TRUE;
}


}

#endif
