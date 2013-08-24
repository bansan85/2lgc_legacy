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
    
    light = new CM3dLight("lumiere 1", DIFFUS, 1.);
    light->set_position(10., 20., -20.);
    global_data->scene->add_light(light);
    
    g_signal_connect(m3d->drawing, "draw", G_CALLBACK(m3d_draw), global_data);
    g_signal_connect(m3d->drawing, "configure-event", G_CALLBACK(m3d_configure_event), projet);
    
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
    Projet      *projet = (Projet*) data2;
    SGlobalData *data = (SGlobalData*)projet->list_gtk.m3d.data;
    
    if ((data->camera == NULL) && (projet->list_gtk.comp.window != NULL))
        BUG(m3d_camera_axe_x_z_y(projet), FALSE);
    
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
    SGlobalData *data2 = (SGlobalData*)data;

    data2->scene->show_to_GtkDrawingarea(drawing, data2->camera);
    
    return FALSE;
}


gboolean m3d_key_press(GtkWidget *widget, GdkEventKey *event, Projet *projet)
{
    Gtk_m3d     *m3d = &projet->list_gtk.m3d;
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
                vue->camera->get_position()->get_coordinates(&x1,&y1,&z1);
                vue->camera->set_position(x1+m3d->gdx, y1+m3d->gdy, z1+m3d->gdz);
                
                vect = vue->camera->get_target();
                vect->get_coordinates(&x1,&y1,&z1);
                vue->camera->set_target(x1+m3d->gdx, y1+m3d->gdy, z1+m3d->gdz);
                
                BUG(m3d_rafraichit(projet), FALSE);
                
                break;
            }
            case GDK_KEY_KP_Left :
            case GDK_KEY_Left :
            case GDK_KEY_q :
            case GDK_KEY_Q :
            {
                vue->camera->get_position()->get_coordinates(&x1,&y1,&z1);
                vue->camera->set_position(x1-m3d->gdx, y1-m3d->gdy, z1-m3d->gdz);
                
                vect = vue->camera->get_target();
                vect->get_coordinates(&x1,&y1,&z1);
                vue->camera->set_target(x1-m3d->gdx, y1-m3d->gdy, z1-m3d->gdz);
                
                BUG(m3d_rafraichit(projet), FALSE);
                
                break;
            }
            case GDK_KEY_KP_Up :
            case GDK_KEY_Up :
            case GDK_KEY_z :
            case GDK_KEY_Z :
            {
                vue->camera->get_position()->get_coordinates(&x1,&y1,&z1);
                vue->camera->set_position(x1+m3d->hbx, y1+m3d->hby, z1+m3d->hbz);
                
                vect = vue->camera->get_target();
                vect->get_coordinates(&x1,&y1,&z1);
                vue->camera->set_target(x1+m3d->hbx, y1+m3d->hby, z1+m3d->hbz);
                
                BUG(m3d_rafraichit(projet), FALSE);
                
                break;
            }
            case GDK_KEY_KP_Down :
            case GDK_KEY_Down :
            case GDK_KEY_s :
            case GDK_KEY_S :
            {
                vue->camera->get_position()->get_coordinates(&x1,&y1,&z1);
                vue->camera->set_position(x1-m3d->hbx, y1-m3d->hby, z1-m3d->hbz);
                
                vect = vue->camera->get_target();
                vect->get_coordinates(&x1,&y1,&z1);
                vue->camera->set_target(x1-m3d->hbx, y1-m3d->hby, z1-m3d->hbz);
                
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


gboolean m3d_get_rect(double *xmin, double *xmax, double *ymin, double *ymax, Projet *projet)
/* Description : Renvoie le rectangle anglobant la structure dans le cas d'une projection en 2D
 *               en fonction de la caméra en cours.
 * Paramètres : double *xmin : l'abscisse mini,
 *            : double *xmax : l'abscisse maxi,
 *            : double *ymin : l'ordonnée mini,
 *            : double *ymax : l'ordonnée maxi,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : TRUE.
 *   Échec : FALSE :
 *             projet == NULL,
 *             projet->list_gtk.m3d == NULL,
 *             m3d->data == NULL,
 *             en cas d'erreur due à une fonction interne.
 */
{
    SGlobalData *vue;
    GList       *list_parcours;
    EF_Noeud    *noeud;
    EF_Point    point;
    CM3dVertex  v1; // Vecteur permettant de créer le polygone
    double      x1, y1; // valeurs permettant de récupérer les coordonnées des vecteurs.
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(xmin, FALSE, gettext("Paramètre %s incorrect.\n"), "xmin");
    BUGMSG(xmax, FALSE, gettext("Paramètre %s incorrect.\n"), "xmax");
    BUGMSG(ymin, FALSE, gettext("Paramètre %s incorrect.\n"), "ymin");
    BUGMSG(ymax, FALSE, gettext("Paramètre %s incorrect.\n"), "ymax");
    
    vue = (SGlobalData*)(&projet->list_gtk.m3d)->data;
    
    noeud = (EF_Noeud*)projet->modele.noeuds->data;
    BUG(EF_noeuds_renvoie_position(noeud, &point), FALSE);
    v1.set_coordinates(common_math_get(point.x), common_math_get(point.y), common_math_get(point.z));
    v1 = vue->camera->convert_vertex_by_camera_view(&v1);
    v1 = vue->camera->convert_vertex_to_2d(&v1);
    v1.get_coordinates(&x1, &y1, NULL);
    *ymax = y1;
    *ymin = y1;
    *xmax = x1;
    *xmin = x1;
    
    list_parcours = g_list_next(projet->modele.noeuds);
    while (list_parcours != NULL)
    {
        noeud = (EF_Noeud*)list_parcours->data;
        BUG(EF_noeuds_renvoie_position(noeud, &point), FALSE);
        
        v1.set_coordinates(common_math_get(point.x), common_math_get(point.y), common_math_get(point.z));
        v1 = vue->camera->convert_vertex_by_camera_view(&v1);
        v1 = vue->camera->convert_vertex_to_2d(&v1);
        v1.get_coordinates(&x1, &y1, NULL);
        
        if (*xmin > x1)
            *xmin = x1;
        if (*xmax < x1)
            *xmax = x1;
        if (*ymin > y1)
            *ymin = y1;
        if (*ymax < y1)
            *ymax = y1;
        
        list_parcours = g_list_next(list_parcours);
    }
    
    return TRUE;
}


gboolean m3d_camera_zoom_all(Projet *projet)
/* Description : Modifie la position de la caméra en x, y et z pour voir l'ensemble de la
 *               structure.
 * Paramètres : Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : TRUE.
 *   Échec : FALSE :
 *             projet == NULL,
 *             projet->list_gtk.m3d == NULL,
 *             m3d->data == NULL,
 *             en cas d'erreur due à une fonction interne.
 */
{
    Gtk_m3d     *m3d;
    SGlobalData *vue;
    double      x, y, z; // Les coordonnées de la caméra
    double      xtmp, ytmp, ztmp; // Les coordonnées de la caméra
    EF_Noeud    *noeud; // Noeud en cours d'étude
    EF_Point    point; // Position du noeud en cours d'étude
    double      ymax, ymin, xmin, xmax, zmin; // Leur projection en 2D
    GList       *list_parcours; // Noeud en cours d'étude
    GtkAllocation   allocation; // Dimension de la fenêtre 2D.
    double      cx, cy, cz; // Le vecteur de la caméra
    CM3dVertex  v1, v2;
    double      tmpx, tmpy, tmpz;
    double      dx, dy, dz, dztmp;
    double      xmin2, xmax2, ymin2, ymax2;
    double      yymin;
    int         i = 0; // Sécurité pour éviter que la boucle ne tourne à l'infini.
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    
    BUGMSG(projet->list_gtk.comp.window, FALSE, gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "principale");
    
    m3d = &projet->list_gtk.m3d;
    BUGMSG(m3d->data, FALSE, gettext("Paramètre %s incorrect.\n"), "m3d->data");
    vue = (SGlobalData*)m3d->data;
    
    BUGMSG(vue->camera, FALSE, gettext("La caméra n'est pas initialisée.\n"));
    
    // Aucune noeud, on ne fait rien
    if (projet->modele.noeuds == NULL)
        return TRUE;
    
    // Un seul noeud, on l'affiche en gros plan.
    noeud = (EF_Noeud*)projet->modele.noeuds->data;
    BUG(EF_noeuds_renvoie_position(noeud, &point), FALSE);
    
    gtk_widget_get_allocation(GTK_WIDGET(m3d->drawing), &allocation);
    x = 0;
    y = 0;
    z = 0;
    vue->camera->get_target_vector()->get_coordinates(&cx, &cy, &cz);
    vue->camera->set_position(x, y, z);
    vue->camera->set_target(x+cx, y+cy, z+cz);
    
    // On cherche le xmin, xmax, zmin, zmax et ymin de l'ensemble des noeuds afin de définir
    // la position optimale de la caméra.
    v1.set_coordinates(common_math_get(point.x), common_math_get(point.y), common_math_get(point.z));
    v1 = vue->camera->convert_vertex_by_camera_view(&v1);
    v1.get_coordinates(&tmpx, &tmpy, &tmpz);
    xmin = tmpx;
    xmax = tmpx;
    ymax = tmpy;
    ymin = tmpy;
    zmin = tmpz;
    list_parcours = g_list_next(projet->modele.noeuds);
    while (list_parcours != NULL)
    {
        noeud = (EF_Noeud*)list_parcours->data;
        BUG(EF_noeuds_renvoie_position(noeud, &point), FALSE);
        
        v1.set_coordinates(common_math_get(point.x), common_math_get(point.y), common_math_get(point.z));
        v1 = vue->camera->convert_vertex_by_camera_view(&v1);
        v1.get_coordinates(&tmpx, &tmpy, &tmpz);
        if (xmin > tmpx)
            xmin = tmpx;
        if (xmax < tmpx)
            xmax = tmpx;
        if (ymin > tmpy)
            ymin = tmpy;
        if (ymax < tmpy)
            ymax = tmpy;
        if (zmin > tmpz)
            zmin = tmpz;
        
        list_parcours = g_list_next(list_parcours);
    }
    
    // On positionne le centre de la caméra
    xtmp = (xmin+xmax)/2.;
    ytmp = zmin-sqrt((xmax-xmin)*(xmax-xmin)+(ymax-ymin)*(ymax-ymin));
    ztmp = (ymin+ymax)/2.;
    v1.set_coordinates(xtmp, ztmp, ytmp);
    v1.z_rotate(&v1, vue->camera->get_cosz(), -vue->camera->get_sinz());
    v1.x_rotate(&v1, vue->camera->get_cosx(), -vue->camera->get_sinx());
    v1.y_rotate(&v1, vue->camera->get_cosy(), -vue->camera->get_siny());
    v1.get_coordinates(&x, &y, &z);
    
    vue->camera->set_position(x, y, z);
    vue->camera->set_target(x+cx, y+cy, z+cz);
    // A ce stade, on est sûr qu'il n'y a besoin plus que de zoomer et de centrer la structure
    // au sein de la fenêtre.
    
    yymin = zmin - ytmp;
    do
    {
        // On centre les points par rapport à l'abscisse (x)
        BUG(m3d_get_rect(&xmin, &xmax, &ymin, &ymax, projet), FALSE);
        dx = 1;
        v1.set_coordinates(dx, 0, 0);
        v1.z_rotate(&v1, vue->camera->get_cosz(), -vue->camera->get_sinz());
        v1.x_rotate(&v1, vue->camera->get_cosx(), -vue->camera->get_sinx());
        v1.y_rotate(&v1, vue->camera->get_cosy(), -vue->camera->get_siny());
        v1.get_coordinates(&tmpx, &tmpy, &tmpz);
        do
        {
            vue->camera->set_position(x+tmpx, y+tmpy, z+tmpz);
            vue->camera->set_target(x+tmpx+cx, y+tmpy+cy, z+tmpz+cz);
            BUG(m3d_get_rect(&xmin2, &xmax2, &ymin2, &ymax2, projet), FALSE);
            // Droite (a*X+b=Y) passant en X=x et Y = (xmin+xmax)/2
            //                             X=x+dx et Y = (xmin2+xmax2)/2
            // Le nouveau x est obtenu en cherchant f(x)=allocation.width/2.
            if (!ERREUR_RELATIVE_EGALE(xmax-xmax2+xmin-xmin2, 0.))
            {
                dx = -dx*(allocation.width-xmax-xmin)/(xmax-xmax2+xmin-xmin2)/5.;
                v1.set_coordinates(dx, 0, 0);
                v1.z_rotate(&v1, vue->camera->get_cosz(), -vue->camera->get_sinz());
                v1.x_rotate(&v1, vue->camera->get_cosx(), -vue->camera->get_sinx());
                v1.y_rotate(&v1, vue->camera->get_cosy(), -vue->camera->get_siny());
                v1.get_coordinates(&tmpx, &tmpy, &tmpz);
                x = x + tmpx*5.;
                y = y + tmpy*5.;
                z = z + tmpz*5.;
            }
            else
                break;
            vue->camera->set_position(x+tmpx, y+tmpy, z+tmpz);
            vue->camera->set_target(x+tmpx+cx, y+tmpy+cy, z+tmpz+cz);
            BUG(m3d_get_rect(&xmin, &xmax, &ymin, &ymax, projet), FALSE);
        } while (fabs((xmin+xmax)/2.-(xmin2+xmax2)/2.) > 1.);
        
        // On centre les points par rapport à l'ordonnée (y)
        BUG(m3d_get_rect(&xmin, &xmax, &ymin, &ymax, projet), FALSE);
        dy = 1;
        v1.set_coordinates(0, dy, 0);
        v1.z_rotate(&v1, vue->camera->get_cosz(), -vue->camera->get_sinz());
        v1.x_rotate(&v1, vue->camera->get_cosx(), -vue->camera->get_sinx());
        v1.y_rotate(&v1, vue->camera->get_cosy(), -vue->camera->get_siny());
        v1.get_coordinates(&tmpx, &tmpy, &tmpz);
        do
        {
            vue->camera->set_position(x+tmpx, y+tmpy, z+tmpz);
            vue->camera->set_target(x+tmpx+cx, y+tmpy+cy, z+tmpz+cz);
            BUG(m3d_get_rect(&xmin2, &xmax2, &ymin2, &ymax2, projet), FALSE);
            if (!ERREUR_RELATIVE_EGALE(ymax-ymax2+ymin-ymin2, 0.))
            {
                dy = -dy*(allocation.height-ymax-ymin)/(ymax-ymax2+ymin-ymin2)/5.;
                v1.set_coordinates(0, dy, 0);
                v1.z_rotate(&v1, vue->camera->get_cosz(), -vue->camera->get_sinz());
                v1.x_rotate(&v1, vue->camera->get_cosx(), -vue->camera->get_sinx());
                v1.y_rotate(&v1, vue->camera->get_cosy(), -vue->camera->get_siny());
                v1.get_coordinates(&tmpx, &tmpy, &tmpz);
                x = x + tmpx*5.;
                y = y + tmpy*5.;
                z = z + tmpz*5.;
            }
            else
                break;
            vue->camera->set_position(x+tmpx, y+tmpy, z+tmpz);
            vue->camera->set_target(x+tmpx+cx, y+tmpy+cy, z+tmpz+cz);
            BUG(m3d_get_rect(&xmin, &xmax, &ymin, &ymax, projet), FALSE);
        } while (fabs((ymin+ymax)/2.-(ymin2+ymax2)/2.) > 1.);
        
        // On zoom autant que possible de tel sorte que la structure tienne au plus juste
        // dans la fenêtre.
        BUG(m3d_get_rect(&xmin, &xmax, &ymin, &ymax, projet), FALSE);
        dz = yymin/5.; // On commence par avancer d'1/5 de la distance maximale autorisée.
        do
        {
            // On avance de dz dans la direction de la caméra.
            vue->camera->set_position(x+dz*cx, y+dz*cy, z+dz*cz);
            vue->camera->set_target(x+dz*cx+cx, y+dz*cy+cy, z+dz*cz+cz);
            BUG(m3d_get_rect(&xmin2, &xmax2, &ymin2, &ymax2, projet), FALSE);
            // 1er cas : étude des abscisses.
            // Le choix de l'interpolation est : f(x) = (a*x+b)/(c*x+d).
            // avec les conditions suivantes : f(-inf) = 0. En effet, si on recule au maximum,
            // la largeur de la vue sera nulle. Soit a/c = 0. On peut donc fixer
            //   forfaitairement c = 1 et a à 0.
            // On se retrouve donc avec la fonction f(x) = b/(x+d)
            // Droite (b/(X+d)=Y) passant en X=z et Y= xmax-xmin=x1
            //                               X=z+dz et Y = xmax2-xmin2=x2
            // On obtient : b = (dz*x1*x2)/(x1-x2) et c = ((x2-x1)*z+dz*x2)/(x1-x2)
            // Le nouveau y est obtenu en cherchant f(x)=allocation.width
            // Ainsi, on obtient une valeur optimale de dz = dz*x2*(x1-w)/((x1-x2)*w)
            // On fait le même calcul pour les ordonnées.
            // Ensuite, on retient la valeur de dz minimale.
            if (((!ERREUR_RELATIVE_EGALE(xmax-xmin-(xmax2-xmin2), 0.)) || ((xmax-xmin < 1.) && (xmax2-xmin2 < 1.) && (fabs(allocation.width-xmax-xmin) < 1.) && (fabs(allocation.width-xmax2-xmin2) < 1.))) &&
              ((!ERREUR_RELATIVE_EGALE(ymax-ymin-(ymax2-ymin2), 0.)) || ((ymax-ymin < 1.) && (ymax2-ymin2 < 1.) && (fabs(allocation.height-ymax-ymin) < 1.) && (fabs(allocation.height-ymax2-ymin2) < 1.))))
            {
                // Ici, les conditions (xmax-xmin < 0.5) && (xmax2-xmin2 < 0.5) &&
                // (fabs(allocation.width-xmax-xmin) < 1.) &&
                // (fabs(allocation.width-xmax2-xmin2) < 1.) sont là au cas où tous les noeuds
                // sont alignés parfaitement à la vertical. Par exemple un schéma 2D en XZ
                // avec la vue en YZ.
                dztmp = NAN;
                if (!ERREUR_RELATIVE_EGALE(xmax-xmin-(xmax2-xmin2), 0.))
                    dztmp = dz*(xmax2-xmin2)*(xmax-xmin-allocation.width)/((xmax-xmin-(xmax2-xmin2))*allocation.width)/5.;
                if (!ERREUR_RELATIVE_EGALE(ymax-ymin-(ymax2-ymin2), 0.))
                {
                    if (isnan(dztmp))
                        dztmp = dz*(ymax2-ymin2)*(ymax-ymin-allocation.height)/((ymax-ymin-(ymax2-ymin2))*allocation.height)/5.;
                    else
                        dztmp = MIN(dz*(ymax2-ymin2)*(ymax-ymin-allocation.height)/((ymax-ymin-(ymax2-ymin2))*allocation.height)/5., dztmp);
                }
                dz = dztmp;
                // L'extrapolation dit qu'il faut plutôt avancer de dz*5.
                // Il est nécessaire de brider les déplacements pour éviter que l'estimation
                // ne mette un point derrière la caméra. On recule un tout petit peu plus pour
                // éviter que le point le plus proche de la caméra ne se trouve dans le plan XZ.
                // Normalement, ça ne devrait pas se produire mais c'est une sécurité.
                if (dz*5. > yymin)
                {
                    // Oups, on avance trop. On bride à yymin/5.5*5.
                    dz = yymin/5.5;
                }
                x = x+dz*cx*5.;
                y = y+dz*cy*5.;
                z = z+dz*cz*5.;
                yymin = yymin - dz*5;
                dz = yymin/5.;
                // Maintenant, on ne peut pas avancer de plus de yymin.
            }
            else
                break;
            vue->camera->set_position(x, y, z);
            vue->camera->set_target(x+cx, y+cy, z+cz);
            BUG(m3d_get_rect(&xmin, &xmax, &ymin, &ymax, projet), FALSE);
        } while ((fabs(xmax-xmin-allocation.width) > 1.) && (fabs(ymax-ymin-allocation.height) > 1.));
        
        BUG(m3d_get_rect(&xmin2, &xmax2, &ymin2, &ymax2, projet), FALSE);
        // Tant qu'une fois le zoom fini, le dessin n'est pas centré.
        i++;
        if (i==100)
            break;
    } while ((fabs(xmax2+xmin2-allocation.width) > 1.) || (fabs(ymax2+ymin2-allocation.height) > 1.));
    
    BUG(m3d_rafraichit(projet), FALSE);
    
    if (i == 100)
        BUGMSG(NULL, FALSE, "La fonction \"zoom tout\" vient de tourner en boucle.\n");
    
    return TRUE;
}


gboolean m3d_camera_axe_x_z_y(Projet *projet)
/* Description : Positionne la caméra pour voir toute la structure dans le plan xz vers la
 *               direction y.
 * Paramètres : Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : TRUE.
 *   Échec : FALSE :
 *             projet == NULL,
 *             projet->list_gtk.m3d == NULL,
 *             m3d->data == NULL,
 *             en cas d'erreur due à une fonction interne.
 */
{
    Gtk_m3d     *m3d;
    SGlobalData *vue;
    double      x, y, z; // Les coordonnées de la caméra
    EF_Noeud    *noeud; // Noeud en cours d'étude
    EF_Point    point; // Position du noeud en cours d'étude
    GtkAllocation   allocation; // Dimension de la fenêtre 2D.
    double      cx, cy, cz; // Le vecteur de la caméra
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    
    BUGMSG(projet->list_gtk.comp.window, FALSE, gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "principale");
    
    m3d = &projet->list_gtk.m3d;
    BUGMSG(m3d->data, FALSE, gettext("Paramètre %s incorrect.\n"), "m3d->data");
    vue = (SGlobalData*)m3d->data;
    
    // Aucune noeud, on ne fait rien
    if (projet->modele.noeuds == NULL)
        return TRUE;
    
    // Un seul noeud, on l'affiche en gros plan.
    noeud = (EF_Noeud*)projet->modele.noeuds->data;
    BUG(EF_noeuds_renvoie_position(noeud, &point), FALSE);
    
    gtk_widget_get_allocation(GTK_WIDGET(m3d->drawing), &allocation);
    x = 0;
    y = 0;
    z = 0;
    cx = 0.;
    cy = 1.;
    cz = 0.;
    if (vue->camera == NULL)
    {
        vue->camera = new CM3dCamera(x, y, z, x+cx, y+cy, z+cz, 90, allocation.width, allocation.height);
    }
    else
    {
        vue->camera->set_position(x, y, z);
        vue->camera->set_target(x+cx, y+cy, z+cz);
    }
    vue->camera->rotation_on_axe_of_view(0);
    
    // Initialisation du vecteur de déplacement.
    m3d->gdx = 1.;
    m3d->gdy = 0.;
    m3d->gdz = 0.;
    m3d->hbx = 0.;
    m3d->hby = 0.;
    m3d->hbz = 1.;
    
    BUG(m3d_camera_zoom_all(projet), FALSE);
    
    return TRUE;
}


gboolean m3d_camera_axe_x_z__y(Projet *projet)
/* Description : Positionne la caméra pour voir toute la structure dans le plan xz vers la
 *               direction -y.
 * Paramètres : Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : TRUE.
 *   Échec : FALSE :
 *             projet == NULL,
 *             projet->list_gtk.m3d == NULL,
 *             m3d->data == NULL,
 *             en cas d'erreur due à une fonction interne.
 */
{
    Gtk_m3d     *m3d;
    SGlobalData *vue;
    double      x, y, z; // Les coordonnées de la caméra
    EF_Noeud    *noeud; // Noeud en cours d'étude
    EF_Point    point; // Position du noeud en cours d'étude
    GtkAllocation   allocation; // Dimension de la fenêtre 2D.
    double      cx, cy, cz; // Le vecteur de la caméra
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    
    BUGMSG(projet->list_gtk.comp.window, FALSE, gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "principale");
    
    m3d = &projet->list_gtk.m3d;
    BUGMSG(m3d->data, FALSE, gettext("Paramètre %s incorrect.\n"), "m3d->data");
    vue = (SGlobalData*)m3d->data;
    
    // Aucune noeud, on ne fait rien
    if (projet->modele.noeuds == NULL)
        return TRUE;
    
    // Un seul noeud, on l'affiche en gros plan.
    noeud = (EF_Noeud*)projet->modele.noeuds->data;
    BUG(EF_noeuds_renvoie_position(noeud, &point), FALSE);
    
    gtk_widget_get_allocation(GTK_WIDGET(m3d->drawing), &allocation);
    x = 0;
    y = 0;
    z = 0;
    cx = 0.;
    cy = -1.;
    cz = 0.;
    if (vue->camera == NULL)
    {
        vue->camera = new CM3dCamera(x, y, z, x+cx, y+cy, z+cz, 90, allocation.width, allocation.height);
    }
    else
    {
        vue->camera->set_position(x, y, z);
        vue->camera->set_target(x+cx, y+cy, z+cz);
    }
    vue->camera->rotation_on_axe_of_view(180);
    
    // Initialisation du vecteur de déplacement.
    m3d->gdx = -1.;
    m3d->gdy = 0.;
    m3d->gdz = 0.;
    m3d->hbx = 0.;
    m3d->hby = 0.;
    m3d->hbz = 1.;
    
    BUG(m3d_camera_zoom_all(projet), FALSE);
    
    return TRUE;
}


gboolean m3d_camera_axe_y_z_x(Projet *projet)
/* Description : Positionne la caméra pour voir toute la structure dans le plan yz vers la
 *               direction +x.
 * Paramètres : Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : TRUE.
 *   Échec : FALSE :
 *             projet == NULL,
 *             projet->list_gtk.m3d == NULL,
 *             m3d->data == NULL,
 *             en cas d'erreur due à une fonction interne.
 */
{
    Gtk_m3d     *m3d;
    SGlobalData *vue;
    double      x, y, z; // Les coordonnées de la caméra
    EF_Noeud    *noeud; // Noeud en cours d'étude
    EF_Point    point; // Position du noeud en cours d'étude
    GtkAllocation   allocation; // Dimension de la fenêtre 2D.
    double      cx, cy, cz; // Le vecteur de la caméra
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    
    BUGMSG(projet->list_gtk.comp.window, FALSE, gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "principale");
    
    m3d = &projet->list_gtk.m3d;
    BUGMSG(m3d->data, FALSE, gettext("Paramètre %s incorrect.\n"), "m3d->data");
    vue = (SGlobalData*)m3d->data;
    
    // Aucune noeud, on ne fait rien
    if (projet->modele.noeuds == NULL)
        return TRUE;
    
    // Un seul noeud, on l'affiche en gros plan.
    noeud = (EF_Noeud*)projet->modele.noeuds->data;
    BUG(EF_noeuds_renvoie_position(noeud, &point), FALSE);
    
    gtk_widget_get_allocation(GTK_WIDGET(m3d->drawing), &allocation);
    x = 0;
    y = 0;
    z = 0;
    cx = 1.;
    cy = 0.;
    cz = 0.;
    if (vue->camera == NULL)
    {
        vue->camera = new CM3dCamera(x, y, z, x+cx, y+cy, z+cz, 90, allocation.width, allocation.height);
    }
    else
    {
        vue->camera->set_position(x, y, z);
        vue->camera->set_target(x+cx, y+cy, z+cz);
    }
    vue->camera->rotation_on_axe_of_view(90);
    
    // Initialisation du vecteur de déplacement.
    m3d->gdx = 0.;
    m3d->gdy = -1.;
    m3d->gdz = 0.;
    m3d->hbx = 0.;
    m3d->hby = 0.;
    m3d->hbz = 1.;
    
    BUG(m3d_camera_zoom_all(projet), FALSE);
    
    return TRUE;
}


gboolean m3d_camera_axe_y_z__x(Projet *projet)
/* Description : Positionne la caméra pour voir toute la structure dans le plan yz vers la
 *               direction -x.
 * Paramètres : Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : TRUE.
 *   Échec : FALSE :
 *             projet == NULL,
 *             projet->list_gtk.m3d == NULL,
 *             m3d->data == NULL,
 *             en cas d'erreur due à une fonction interne.
 */
{
    Gtk_m3d     *m3d;
    SGlobalData *vue;
    double      x, y, z; // Les coordonnées de la caméra
    EF_Noeud    *noeud; // Noeud en cours d'étude
    EF_Point    point; // Position du noeud en cours d'étude
    GtkAllocation   allocation; // Dimension de la fenêtre 2D.
    double      cx, cy, cz; // Le vecteur de la caméra
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    
    BUGMSG(projet->list_gtk.comp.window, FALSE, gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "principale");
    
    m3d = &projet->list_gtk.m3d;
    BUGMSG(m3d->data, FALSE, gettext("Paramètre %s incorrect.\n"), "m3d->data");
    vue = (SGlobalData*)m3d->data;
    
    // Aucune noeud, on ne fait rien
    if (projet->modele.noeuds == NULL)
        return TRUE;
    
    // Un seul noeud, on l'affiche en gros plan.
    noeud = (EF_Noeud*)projet->modele.noeuds->data;
    BUG(EF_noeuds_renvoie_position(noeud, &point), FALSE);
    
    gtk_widget_get_allocation(GTK_WIDGET(m3d->drawing), &allocation);
    x = 0;
    y = 0;
    z = 0;
    cx = -1.;
    cy = 0.;
    cz = 0.;
    if (vue->camera == NULL)
    {
        vue->camera = new CM3dCamera(x, y, z, x+cx, y+cy, z+cz, 90, allocation.width, allocation.height);
    }
    else
    {
        vue->camera->set_position(x, y, z);
        vue->camera->set_target(x+cx, y+cy, z+cz);
    }
    vue->camera->rotation_on_axe_of_view(-90);
    
    // Initialisation du vecteur de déplacement.
    m3d->gdx = 0.;
    m3d->gdy = 1.;
    m3d->gdz = 0.;
    m3d->hbx = 0.;
    m3d->hby = 0.;
    m3d->hbz = 1.;
    
    BUG(m3d_camera_zoom_all(projet), FALSE);
    
    return TRUE;
}


gboolean m3d_camera_axe_x_y_z(Projet *projet)
/* Description : Positionne la caméra pour voir toute la structure dans le plan xy vers la
 *               direction +z.
 * Paramètres : Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : TRUE.
 *   Échec : FALSE :
 *             projet == NULL,
 *             projet->list_gtk.m3d == NULL,
 *             m3d->data == NULL,
 *             en cas d'erreur due à une fonction interne.
 */
{
    Gtk_m3d     *m3d;
    SGlobalData *vue;
    double      x, y, z; // Les coordonnées de la caméra
    EF_Noeud    *noeud; // Noeud en cours d'étude
    EF_Point    point; // Position du noeud en cours d'étude
    GtkAllocation   allocation; // Dimension de la fenêtre 2D.
    double      cx, cy, cz; // Le vecteur de la caméra
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    
    BUGMSG(projet->list_gtk.comp.window, FALSE, gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "principale");
    
    m3d = &projet->list_gtk.m3d;
    BUGMSG(m3d->data, FALSE, gettext("Paramètre %s incorrect.\n"), "m3d->data");
    vue = (SGlobalData*)m3d->data;
    
    // Aucune noeud, on ne fait rien
    if (projet->modele.noeuds == NULL)
        return TRUE;
    
    // Un seul noeud, on l'affiche en gros plan.
    noeud = (EF_Noeud*)projet->modele.noeuds->data;
    BUG(EF_noeuds_renvoie_position(noeud, &point), FALSE);
    
    gtk_widget_get_allocation(GTK_WIDGET(m3d->drawing), &allocation);
    x = 0;
    y = 0;
    z = 0;
    cx = 0.;
    cy = 0.;
    cz = 1.;
    if (vue->camera == NULL)
    {
        vue->camera = new CM3dCamera(x, y, z, x+cx, y+cy, z+cz, 90, allocation.width, allocation.height);
    }
    else
    {
        vue->camera->set_position(x, y, z);
        vue->camera->set_target(x+cx, y+cy, z+cz);
    }
    vue->camera->rotation_on_axe_of_view(0);
    
    // Initialisation du vecteur de déplacement.
    m3d->gdx = 1.;
    m3d->gdy = 0.;
    m3d->gdz = 0.;
    m3d->hbx = 0.;
    m3d->hby = -1.;
    m3d->hbz = 0.;
    
    BUG(m3d_camera_zoom_all(projet), FALSE);
    
    return TRUE;
}


gboolean m3d_camera_axe_x_y__z(Projet *projet)
/* Description : Positionne la caméra pour voir toute la structure dans le plan xy vers la
 *               direction -z.
 * Paramètres : Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : TRUE.
 *   Échec : FALSE :
 *             projet == NULL,
 *             projet->list_gtk.m3d == NULL,
 *             m3d->data == NULL,
 *             en cas d'erreur due à une fonction interne.
 */
{
    Gtk_m3d     *m3d;
    SGlobalData *vue;
    double      x, y, z; // Les coordonnées de la caméra
    EF_Noeud    *noeud; // Noeud en cours d'étude
    EF_Point    point; // Position du noeud en cours d'étude
    GtkAllocation   allocation; // Dimension de la fenêtre 2D.
    double      cx, cy, cz; // Le vecteur de la caméra
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    
    BUGMSG(projet->list_gtk.comp.window, FALSE, gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "principale");
    
    m3d = &projet->list_gtk.m3d;
    BUGMSG(m3d->data, FALSE, gettext("Paramètre %s incorrect.\n"), "m3d->data");
    vue = (SGlobalData*)m3d->data;
    
    // Aucune noeud, on ne fait rien
    if (projet->modele.noeuds == NULL)
        return TRUE;
    
    // Un seul noeud, on l'affiche en gros plan.
    noeud = (EF_Noeud*)projet->modele.noeuds->data;
    BUG(EF_noeuds_renvoie_position(noeud, &point), FALSE);
    
    gtk_widget_get_allocation(GTK_WIDGET(m3d->drawing), &allocation);
    x = 0;
    y = 0;
    z = 0;
    cx = 0.;
    cy = 0.;
    cz = -1.;
    if (vue->camera == NULL)
    {
        vue->camera = new CM3dCamera(x, y, z, x+cx, y+cy, z+cz, 90, allocation.width, allocation.height);
    }
    else
    {
        vue->camera->set_position(x, y, z);
        vue->camera->set_target(x+cx, y+cy, z+cz);
    }
    vue->camera->rotation_on_axe_of_view(180);
    
    // Initialisation du vecteur de déplacement.
    m3d->gdx = 1.;
    m3d->gdy = 0.;
    m3d->gdz = 0.;
    m3d->hbx = 0.;
    m3d->hby = 1.;
    m3d->hbz = 0.;
    
    BUG(m3d_camera_zoom_all(projet), FALSE);
    
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
        BUG(m3d_noeud(&projet->list_gtk.m3d, (EF_Noeud*)list_parcours->data), FALSE);
        list_parcours = g_list_next(list_parcours);
    }
    g_list_free(noeuds_dep);
    
    list_parcours = barres_dep;
    while (list_parcours != NULL)
    {
        BUG(m3d_barre(&projet->list_gtk.m3d, (EF_Barre*)list_parcours->data), FALSE);
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
    gtk_widget_queue_resize(projet->list_gtk.m3d.drawing);
    
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
    EF_Point    point;
    SGlobalData *vue;
    
    BUGMSG(noeud, NULL, gettext("Paramètre %s incorrect.\n"), "noeud");
    BUGMSG(donnees_m3d, NULL, gettext("Paramètre %s incorrect.\n"), "donnees_m3d");
    
    BUGMSG(nom = g_strdup_printf("noeud %u", noeud->numero), NULL, gettext("Erreur d'allocation mémoire.\n"));
    BUG(EF_noeuds_renvoie_position(noeud, &point), NULL);
    
    vue = (SGlobalData*)((Gtk_m3d *)donnees_m3d)->data;
    
    cube = vue->scene->get_object_by_name(nom);
    if (cube != NULL)
        vue->scene->remove_object(cube);
    
    cube = M3d_cube_new(nom, .1);
    cube->set_ambient_reflexion(1.);
    cube->set_smooth(GOURAUD);
    vue->scene->add_object(cube);
    cube->set_position(common_math_get(point.x), common_math_get(point.y), common_math_get(point.z));
    
    free(nom);
    
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


gboolean m3d_barre_finition(CM3dObject *objet, EF_Barre *barre)
/* Description : Applique une liste d'instruction commune à toutes les barres.
 * Paramètres : CM3dObject *objet : la modélisation de la barre,
 *              EF_Barre *barre : barre devant être représentée.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             en cas d'erreur d'une fonction interne.
 */
{
    double      y, z;
    double      x1, y1, z1;
    double      dx, dy, dz;
    EF_Point    p_d, p_f;
    
    objet->set_color(100, 100, 100);
    objet->set_ambient_reflexion(0.8);
    objet->set_smooth(GOURAUD);
    switch (barre->section->type)
    {
        case SECTION_RECTANGULAIRE :
        case SECTION_T :
        case SECTION_CARREE :
        case SECTION_CIRCULAIRE :
        {
            x1 = 0.;
            y1 = 0.;
            z1 = 0.;
            break;
        }
        case SECTION_PERSONNALISEE :
        {
            objet->get_center()->get_coordinates(&x1, &y1, &z1);
            break;
        }
        default :
        {
            BUGMSG(0, FALSE, gettext("Type de section %d inconnu.\n"), barre->section->type);
            break;
        }
    }
    BUG(_1992_1_1_barres_angle_rotation(barre->noeud_debut, barre->noeud_fin, &y, &z), FALSE);
    objet->rotations(0., -y/M_PI*180., z/M_PI*180.);
    BUG(EF_noeuds_renvoie_position(barre->noeud_debut, &p_d), FALSE);
    BUG(EF_noeuds_renvoie_position(barre->noeud_fin, &p_f), FALSE);
    dx = (common_math_get(p_d.x)+common_math_get(p_f.x))/2.;
    dy = (common_math_get(p_d.y)+common_math_get(p_f.y))/2.;
    dz = (common_math_get(p_d.z)+common_math_get(p_f.z))/2.;
    objet->set_position(-(cos(z)*cos(y)*x1-sin(z)*y1-sin(y)*cos(z)*z1) + dx, sin(z)*cos(y)*x1+cos(z)*y1-sin(z)*sin(y)*z1 + dy, sin(y)*x1+cos(y)*z1 + dz);
    
    return TRUE;
}


gboolean m3d_barre(void *donnees_m3d, EF_Barre *barre)
/* Description : Crée une barre dans l'affichage graphique. Si la barre existe, elle est
 *               détruite au préalable.
 * Paramètres : void *donnees_m3d : données graphiques,
 *              EF_Barre *barre : barre devant être représentée.
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
        case SECTION_RECTANGULAIRE :
        {
            CM3dObject  *bas, *haut, *gauche, *droite;
            Section_T   *section = (Section_T *)barre->section->data;
            
            droite = M3d_plan_new("", longueur, common_math_get(section->hauteur_retombee), 1);
            droite->rotations(180., 0., 0.);
            droite->set_position(0., -common_math_get(section->largeur_retombee)/2., 0.);
            
            gauche = M3d_plan_new("", longueur, common_math_get(section->hauteur_retombee), 1);
            gauche->set_position(0., common_math_get(section->largeur_retombee)/2., 0.);
            
            bas = M3d_plan_new("", longueur, common_math_get(section->largeur_retombee), 1);
            bas->rotations(90., 180., 0.);
            bas->set_position(0., 0., -common_math_get(section->hauteur_retombee)/2.);
            
            haut = M3d_plan_new("", longueur, common_math_get(section->largeur_retombee), 1);
            haut->rotations(90., 0., 0.);
            haut->set_position(0., 0., common_math_get(section->hauteur_retombee)/2.);
            
            tout = M3d_object_new_group(tmp, droite, gauche, bas, haut, NULL);
            
            delete droite;
            delete gauche;
            delete bas;
            delete haut;
            
            m3d_barre_finition(tout, barre);
            vue->scene->add_object(tout);
            
            break;
        }
        case SECTION_T :
        {
            Section_T *section = (Section_T *)barre->section->data;
            
            double  lt = common_math_get(section->largeur_table);
            double  lr = common_math_get(section->largeur_retombee);
            double  ht = common_math_get(section->hauteur_table);
            double  hr = common_math_get(section->hauteur_retombee);
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
            
            m3d_barre_finition(tout, barre);
            vue->scene->add_object(tout);
            
            break;
        }
        case SECTION_CARREE :
        {
            Section_T   *section = (Section_T *)barre->section->data;
            CM3dObject  *bas, *haut, *gauche, *droite;
            
            droite = M3d_plan_new("", longueur, common_math_get(section->largeur_table), 1);
            droite->rotations(180., 0., 0.);
            droite->set_position(0., -common_math_get(section->largeur_table)/2., 0.);
            
            gauche = M3d_plan_new("", longueur, common_math_get(section->largeur_table), 1);
            gauche->set_position(0., common_math_get(section->largeur_table)/2., 0.);
            
            bas = M3d_plan_new("", longueur, common_math_get(section->largeur_table), 1);
            bas->rotations(90., 180., 0.);
            bas->set_position(0., 0., -common_math_get(section->largeur_table)/2.);
            
            haut = M3d_plan_new("", longueur, common_math_get(section->largeur_table), 1);
            haut->rotations(90., 0., 0.);
            haut->set_position(0., 0., common_math_get(section->largeur_table)/2.);
            
            tout = M3d_object_new_group(tmp, droite, gauche, bas, haut, NULL);
            
            delete droite;
            delete gauche;
            delete bas;
            delete haut;
            
            m3d_barre_finition(tout, barre);
            vue->scene->add_object(tout);
            
            break;
        }
        case SECTION_CIRCULAIRE :
        {
            Section_Circulaire *section = (Section_Circulaire *)barre->section->data;
            
            tout = M3d_cylindre_new(tmp, common_math_get(section->diametre)/2., longueur, 12);
            tout->rotations(0., 0., 90.);
            
            m3d_barre_finition(tout, barre);
            vue->scene->add_object(tout);
            
            break;
        }
        case SECTION_PERSONNALISEE :
        {
            Section_Personnalisee *section = (Section_Personnalisee*)barre->section->data;
            GList   *list_parcours;
            double  angle;
            
            tout = new CM3dObject("");
            
            list_parcours = section->forme;
            while (list_parcours != NULL)
            {
                GList       *list_parcours2;
                EF_Point    *point1 = NULL, *point2 = NULL;
                double      somme_angle = 0.;
                double      angle1 = NAN, angle2 = NAN;
                
                // On commence par parcourir la liste des points pour savoir si le dessin est
                // réalisé dans le sens horaire ou anti-horaire. Ce point est important car
                // les plans ne sont dessinés que s'ils sont vus de face.
                list_parcours2 = (GList*)list_parcours->data;
                while (list_parcours2 != NULL)
                {
                    if (point2 == NULL)
                        point2 = (EF_Point*)list_parcours2->data;
                    else
                    {
                        point1 = point2;
                        if (list_parcours2 != GINT_TO_POINTER(1))
                            point2 = (EF_Point*)list_parcours2->data;
                        else
                            point2 = (EF_Point*)((GList*)list_parcours->data)->data;
                        
                        angle = atan2(common_math_get(point2->y)-common_math_get(point1->y), common_math_get(point2->x)-common_math_get(point1->x))/M_PI*180.;
                        angle1 = angle2;
                        angle2 = angle;
                        if (!isnan(angle1))
                        {
                            angle = angle2 - angle1;
                            if (angle > 180.)
                                angle = angle - 180.;
                            if (angle < -180.)
                                angle = angle + 180.;
                            
                            somme_angle = somme_angle + angle;
                        }
                    }
                    
                    if (list_parcours2 != GINT_TO_POINTER(1))
                    {
                        list_parcours2 = g_list_next(list_parcours2);
                        // On force à faire un dernier passage après la fin de la liste dans le
                        // but de fermer la forme.
                        if (list_parcours2 == NULL)
                            list_parcours2 = static_cast<GList*>GINT_TO_POINTER(1);
                    }
                    else
                        list_parcours2 = NULL;
                }
                
                list_parcours2 = (GList*)list_parcours->data;
                while (list_parcours2 != NULL)
                {
                    CM3dObject *object_tmp = NULL;
                    
                    if (point2 == NULL)
                        point2 = (EF_Point*)list_parcours2->data;
                    else
                    {
                        GList   *list_poly;
                        
                        point1 = point2;
                        if (list_parcours2 != GINT_TO_POINTER(1))
                            point2 = (EF_Point*)list_parcours2->data;
                        else
                            point2 = (EF_Point*)((GList*)list_parcours->data)->data;
                        
                        angle = atan2(common_math_get(point2->y)-common_math_get(point1->y), common_math_get(point2->x)-common_math_get(point1->x))/M_PI*180.-180.;
                        if (somme_angle < 0)
                            angle = angle + 180.;
                        
                        object_tmp = M3d_plan_new("", longueur, EF_points_distance(point1, point2), 1);
                        object_tmp->rotations(angle, 180., 0.);
                        object_tmp->set_position(0., (common_math_get(point2->y)+common_math_get(point1->y))/2., (common_math_get(point2->x)+common_math_get(point1->x))/2.);
                        
                        list_poly = object_tmp->get_list_of_polygons();
                        while (list_poly != NULL)
                        {
                            CM3dPolygon *polygon = new CM3dPolygon(*(CM3dPolygon*)(list_poly->data));
                            tout->add_polygon(polygon);
                            list_poly = g_list_next(list_poly);
                        }
                        
                        delete object_tmp;
                    }
                    
                    if (list_parcours2 != GINT_TO_POINTER(1))
                    {
                        list_parcours2 = g_list_next(list_parcours2);
                        // On force à faire un dernier passage après la fin de la liste dans le
                        // but de fermer la forme.
                        if (list_parcours2 == NULL)
                            list_parcours2 = static_cast<GList*>GINT_TO_POINTER(1);
                    }
                    else
                        list_parcours2 = NULL;
                }
                
                list_parcours = g_list_next(list_parcours);
            }
            tout->rotations(90., 0., 0.);
            
            m3d_barre_finition(tout, barre);
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


void m3d_barre_free(void *donnees_m3d, EF_Barre *barre)
/* Description : Supprimer une barre dans l'affichage graphique.
 * Paramètres : void *donnees_m3d : données SGlobalData,
 *              EF_Barre *barre : barre à supprimer.
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
