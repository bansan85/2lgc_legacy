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

#include <gtk/gtk.h>
#include <libintl.h>
#include <locale.h>
#include <string.h>
#include "common_projet.hpp"
#include "common_erreurs.hpp"
#include "common_math.hpp"
#include "common_gtk.hpp"
#include "common_selection.hpp"
#include "EF_noeuds.hpp"
#include "1992_1_1_barres.hpp"
#include "common_m3d.hpp"


gboolean
m3d_configure_event (GtkWidget         *drawing,
                     GdkEventConfigure *ev,
                     gpointer          *data2)
/**
 * \brief Configuration de la caméra en fonction de la taille du composant
 *        graphique.
 * \param drawing : composant graphique,
 * \param ev : caractéristique de l'évènement,
 * \param data2 : données SGlobalData.
 * \return FALSE.\n
 *   Echec : FALSE :
 *     - interface graphique non initialisée.
 */
{
  Projet      *p = (Projet *) data2;
  SGlobalData *data = (SGlobalData *) UI_M3D.data;
  
  BUGCRIT (UI_GTK.window,
           FALSE,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "principale");)
  
  data->camera->set_size_of_window (ev->width, ev->height);
  
  if (data->camera->get_window_height () < data->camera->get_window_width ())
    data->camera->set_d (data->camera->get_window_height () /
                                   (2 * tan (data->camera->get_angle () / 2)));
  else
    data->camera->set_d (data->camera->get_window_width () /
                                   (2 * tan (data->camera->get_angle () / 2)));
  
  data->scene->rendering (*data->camera);
  
  return FALSE;
}


gboolean
m3d_init (Projet *p)
/**
 * \brief Initialise l'affichage graphique de la structure.
 * \param p : la variable projet.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL,
 *     - en cas d'erreur d'allocation mémoire.
 */
{
  SGlobalData *global_data;
  CM3dLight    light;
  
  BUGPARAM (p, "%p", p, FALSE)
  
  UI_M3D.drawing = gtk_drawing_area_new ();
  gtk_widget_add_events(UI_M3D.drawing, GDK_POINTER_MOTION_MASK);
  gtk_widget_add_events(UI_M3D.drawing, GDK_BUTTON_PRESS_MASK);
  BUGCRIT (UI_M3D.data = malloc (sizeof (SGlobalData)),
           FALSE,
           (gettext ("Erreur d'allocation mémoire.\n"));)
  memset (UI_M3D.data, 0, sizeof (SGlobalData));
  
  global_data = (SGlobalData *) UI_M3D.data;
  global_data->scene = new CM3dScene ();
  global_data->scene->reverse_y_axis ();
  global_data->scene->show_repere (true, 1.1);
  global_data->scene->set_ambient_light (1.);
  global_data->scene->set_show_type (SOLID);
  global_data->camera = new CM3dCamera ();
  global_data->camera->set_position (10., 10., 10.);
  global_data->camera->set_target (10., 11., 10.);
  global_data->camera->set_angle (90.);
  
  light.set_name ("lumiere 1");
  light.set_type (DIFFUS);
  light.set_intensity (1);
  light.set_position (10., 20., -20.);
  global_data->scene->add_light (light);
  
  g_signal_connect (UI_M3D.drawing,
                    "draw",
                    G_CALLBACK (m3d_draw),
                    global_data);
  g_signal_connect (UI_M3D.drawing,
                    "configure-event",
                    G_CALLBACK (m3d_configure_event),
                    p);
  
  BUG (projet_init_graphique (p), FALSE)
  
  return TRUE;
}


gboolean
m3d_draw (GtkWidget      *drawing,
          GdkEventExpose *ev,
          gpointer       *data)
/**
 * \brief Rendu de l'image 3D dans le widget Zone-dessin.
 * \param drawing : composant graphique,
 * \param ev : caractéristique de l'évènement,
 * \param data : données SGlobalData.
 * \return FALSE.
 */
{
  SGlobalData *data2 = (SGlobalData *) data;

  data2->scene->show_to_GtkDrawingarea (drawing, *data2->camera);
  
  return FALSE;
}


gboolean
m3d_key_press (GtkWidget   *widget,
               GdkEventKey *event,
               Projet      *p)
/**
 * \brief Gestion des touches pour la navigation 3D :\n
 *        - + pour zoomer,
 *        - - pour reculer,
 *        - haut, bas, gauche, droite : défilement de l'affichage.
 * \param widget : composant ayant réalisé l'évènement,
 * \param event : caractéristique de l'évènement,
 * \param p : la variable projet.
 * \return
 *   Succès : FALSE.\n
 *   Échec : FALSE :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
{
  SGlobalData *data  = (SGlobalData *) UI_M3D.data;
  
  BUGPARAM (p, "%p", p, FALSE)
  BUGCRIT (UI_GTK.window,
           FALSE,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "principale");)
  
  if (event->type == GDK_KEY_PRESS)
  {
    switch (event->keyval)
    {
      case GDK_KEY_KP_Add :
      case GDK_KEY_plus :
      {
        data->camera->move_forward (1.);
        
        BUG (m3d_rafraichit (p), FALSE)
        
        break;
      }
      case GDK_KEY_KP_Subtract :
      case GDK_KEY_minus :
      {
        data->camera->move_back (1.);
        
        BUG (m3d_rafraichit (p), FALSE)
        
        break;
      }
      case GDK_KEY_KP_Right :
      case GDK_KEY_Right :
      {
        data->camera->go_right (1.);
        
        BUG (m3d_rafraichit (p), FALSE)
        
        break;
      }
      case GDK_KEY_KP_Left :
      case GDK_KEY_Left :
      {
        data->camera->go_left (1.);
        
        BUG (m3d_rafraichit (p), FALSE)
        
        break;
      }
      case GDK_KEY_KP_Up :
      case GDK_KEY_Up :
      {
        data->camera->go_up (1., data->scene->y_axis_is_inverted ());
        
        BUG (m3d_rafraichit (p), FALSE)
        
        break;
      }
      case GDK_KEY_KP_Down :
      case GDK_KEY_Down :
      {
        data->camera->go_down (1., data->scene->y_axis_is_inverted ());
        
        BUG (m3d_rafraichit (p), FALSE)
        
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


gboolean
m3d_get_rect (double *xmin,
              double *xmax,
              double *ymin,
              double *ymax,
              Projet *p)
/**
 * \brief Renvoie le rectangle anglobant la structure dans le cas d'une
 *        projection en 2D en fonction de la caméra en cours.
 * \param xmin : l'abscisse mini,
 * \param xmax : l'abscisse maxi,
 * \param ymin : l'ordonnée mini,
 * \param ymax : l'ordonnée maxi,
 * \param p : la variable projet.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL,
 *     - xmin == NULL,
 *     - xmax == NULL,
 *     - ymin == NULL,
 *     - ymax == NULL,
 *     - Aucun noeud n'existe,
 *     - interface graphique non initialisée.
 */
{
  SGlobalData *vue;
  GList       *list_parcours;
  EF_Noeud    *noeud;
  EF_Point     point;
  CM3dVertex   v1, *v2, *v3;     // Vecteur permettant de créer le polygone
  double       x1, y1; // Les coordonnées des vecteurs.
  
  BUGPARAM (p, "%p", p, FALSE)
  BUGPARAM (xmin, "%p", xmin, FALSE)
  BUGPARAM (xmax, "%p", xmax, FALSE)
  BUGPARAM (ymin, "%p", ymin, FALSE)
  BUGPARAM (ymax, "%p", ymax, FALSE)
  INFO (p->modele.noeuds, FALSE, (gettext ("Aucun noeud n'est existant.\n"));)
  BUGCRIT (UI_GTK.window,
           FALSE,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "principale");)
  
  vue = (SGlobalData *) UI_M3D.data;
  
  noeud = (EF_Noeud *) p->modele.noeuds->data;
  BUG (EF_noeuds_renvoie_position (noeud, &point), FALSE)
  v1.set_coordinates (m_g (point.x), m_g (point.y), m_g (point.z));
  v2 = vue->camera->convert_vertex_by_camera_view (v1);
  v3 = vue->camera->convert_vertex_to_2d (*v2);
  delete v2;
  v3->get_coordinates (&x1, &y1, NULL);
  delete v3;
  *ymax = y1;
  *ymin = y1;
  *xmax = x1;
  *xmin = x1;
  
  list_parcours = g_list_next (p->modele.noeuds);
  while (list_parcours != NULL)
  {
    noeud = (EF_Noeud *) list_parcours->data;
    BUG (EF_noeuds_renvoie_position (noeud, &point), FALSE)
    v1.set_coordinates (m_g (point.x), m_g (point.y), m_g (point.z));
    v2 = vue->camera->convert_vertex_by_camera_view (v1);
    v3 = vue->camera->convert_vertex_to_2d (*v2);
    delete v2;
    v3->get_coordinates (&x1, &y1, NULL);
    delete v3;
    
    if (*xmin > x1)
      *xmin = x1;
    if (*xmax < x1)
      *xmax = x1;
    if (*ymin > y1)
      *ymin = y1;
    if (*ymax < y1)
      *ymax = y1;
    
    list_parcours = g_list_next (list_parcours);
  }
  
  return TRUE;
}


gboolean
m3d_camera_zoom_all (Projet *p)
/**
 * \brief Modifie la position de la caméra en x, y et z pour voir l'ensemble de
 *        la structure.
 * \param p : la variable projet.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     -  p == NULL,
 *     - interface graphique non initialisée.
 */
{
  SGlobalData  *vue;
  double        x, y, z;       // Les coordonnées de la caméra
  double        xtmp, ytmp, ztmp; // Les coordonnées de la caméra
  EF_Noeud     *noeud;         // Noeud en cours d'étude
  EF_Point      point;         // Position du noeud en cours d'étude
  double        ymax, ymin, xmin, xmax, zmin; // Leur projection en 2D
  GList        *list_parcours; // Noeud en cours d'étude
  GtkAllocation allocation;    // Dimension de la fenêtre 2D.
  double        cx, cy, cz;    // Le vecteur de la caméra
  CM3dVertex    v1, *v2;
  double        tmpx, tmpy, tmpz;
  double        dx, dy, dz, dztmp;
  double        xmin2, xmax2, ymin2, ymax2;
  double        yymin;
  int           i = 0; // Sécurité pour que la boucle ne tourne à l'infini.
  
  BUGPARAM (p, "%p", p, FALSE)
  BUGCRIT (UI_GTK.window,
           FALSE,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "principale");)
  
  vue = (SGlobalData *) UI_M3D.data;
  
  // Aucune noeud, on ne fait rien
  if (p->modele.noeuds == NULL)
    return TRUE;
  
  // Un seul noeud, on l'affiche en gros plan.
  noeud = (EF_Noeud *) p->modele.noeuds->data;
  BUG (EF_noeuds_renvoie_position (noeud, &point), FALSE);
  
  gtk_widget_get_allocation (GTK_WIDGET (UI_M3D.drawing), &allocation);
  x = 0;
  y = 0;
  z = 0;
  vue->camera->get_target_vector ().get_coordinates (&cx, &cy, &cz);
  vue->camera->set_position (x, y, z);
  vue->camera->set_target (x + cx, y + cy, z + cz);
  
  // On cherche le xmin, xmax, zmin, zmax et ymin de l'ensemble des noeuds afin
  // de définir la position optimale de la caméra.
  v1.set_coordinates (m_g (point.x), m_g (point.y), m_g (point.z));
  v2 = vue->camera->convert_vertex_by_camera_view (v1);
  v2->get_coordinates (&tmpx, &tmpy, &tmpz);
  delete v2;
  xmin = tmpx;
  xmax = tmpx;
  ymax = tmpy;
  ymin = tmpy;
  zmin = tmpz;
  list_parcours = g_list_next (p->modele.noeuds);
  while (list_parcours != NULL)
  {
    noeud = (EF_Noeud *) list_parcours->data;
    BUG (EF_noeuds_renvoie_position (noeud, &point), FALSE)
    
    v1.set_coordinates (m_g (point.x), m_g (point.y), m_g (point.z));
    v2 = vue->camera->convert_vertex_by_camera_view (v1);
    v2->get_coordinates (&tmpx, &tmpy, &tmpz);
    delete v2;
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
    
    list_parcours = g_list_next (list_parcours);
  }
  
  // On positionne le centre de la caméra
  xtmp = (xmin + xmax) / 2.;
  ytmp = zmin - sqrt ((xmax - xmin) * (xmax - xmin) +
                      (ymax - ymin) * (ymax - ymin));
  ztmp = (ymin + ymax) / 2.;
  v1.set_coordinates (xtmp, ztmp, ytmp);
  v1.z_rotate (v1, vue->camera->get_cosz (), -vue->camera->get_sinz ());
  v1.x_rotate (v1, vue->camera->get_cosx (), -vue->camera->get_sinx ());
  v1.y_rotate (v1, vue->camera->get_cosy (), -vue->camera->get_siny ());
  v1.get_coordinates (&x, &y, &z);
  
  vue->camera->set_position (x, y, z);
  vue->camera->set_target (x + cx, y + cy, z + cz);
  // A ce stade, on est sûr qu'il n'y a besoin plus que de zoomer et de centrer
  // la structure au sein de la fenêtre.
  
  yymin = zmin - ytmp;
  do
  {
    // On centre les points par rapport à l'abscisse (x)
    BUG (m3d_get_rect (&xmin, &xmax, &ymin, &ymax, p), FALSE)
    dx = 1;
    v1.set_coordinates (dx, 0, 0);
    v1.z_rotate (v1, vue->camera->get_cosz (), -vue->camera->get_sinz ());
    v1.x_rotate (v1, vue->camera->get_cosx (), -vue->camera->get_sinx ());
    v1.y_rotate (v1, vue->camera->get_cosy (), -vue->camera->get_siny ());
    v1.get_coordinates (&tmpx, &tmpy, &tmpz);
    do
    {
      vue->camera->set_position (x + tmpx, y + tmpy, z + tmpz);
      vue->camera->set_target (x + tmpx + cx, y + tmpy + cy, z + tmpz + cz);
      BUG (m3d_get_rect (&xmin2, &xmax2, &ymin2, &ymax2, p), FALSE)
      // Droite (a*X+b=Y) passant en X=x    et Y = (xmin +xmax )/2
      //                             X=x+dx et Y = (xmin2+xmax2)/2
      // Le nouveau x est obtenu en cherchant f(x)=allocation.width/2.
      if (!errmax (xmax - xmax2 + xmin - xmin2, ABS (allocation.width)))
      {
        dx = -dx * (allocation.width - xmax - xmin) /
                   (xmax - xmax2 + xmin - xmin2) / 5.;
        v1.set_coordinates (dx, 0, 0);
        v1.z_rotate (v1, vue->camera->get_cosz (), -vue->camera->get_sinz ());
        v1.x_rotate (v1, vue->camera->get_cosx (), -vue->camera->get_sinx ());
        v1.y_rotate (v1, vue->camera->get_cosy (), -vue->camera->get_siny ());
        v1.get_coordinates (&tmpx, &tmpy, &tmpz);
        x = x + tmpx * 5.;
        y = y + tmpy * 5.;
        z = z + tmpz * 5.;
      }
      else
        break;
      vue->camera->set_position (x + tmpx, y + tmpy, z + tmpz);
      vue->camera->set_target (x + tmpx + cx, y + tmpy + cy, z + tmpz + cz);
      BUG (m3d_get_rect (&xmin, &xmax, &ymin, &ymax, p), FALSE)
    } while (fabs ((xmin + xmax) / 2. - (xmin2 + xmax2) / 2.) > 1.);
    
    // On centre les points par rapport à l'ordonnée (y)
    BUG (m3d_get_rect (&xmin, &xmax, &ymin, &ymax, p), FALSE)
    dy = 1;
    v1.set_coordinates (0, dy, 0);
    v1.z_rotate (v1, vue->camera->get_cosz (), -vue->camera->get_sinz ());
    v1.x_rotate (v1, vue->camera->get_cosx (), -vue->camera->get_sinx ());
    v1.y_rotate (v1, vue->camera->get_cosy (), -vue->camera->get_siny ());
    v1.get_coordinates (&tmpx, &tmpy, &tmpz);
    do
    {
      vue->camera->set_position (x + tmpx, y + tmpy, z + tmpz);
      vue->camera->set_target (x + tmpx + cx, y + tmpy + cy, z + tmpz + cz);
      BUG (m3d_get_rect (&xmin2, &xmax2, &ymin2, &ymax2, p), FALSE)
      if (!errmax (ymax - ymax2 + ymin - ymin2, ABS (allocation.height)))
      {
        dy = -dy * (allocation.height - ymax - ymin) /
                   (ymax - ymax2 + ymin - ymin2) / 5.;
        v1.set_coordinates (0, dy, 0);
        v1.z_rotate (v1, vue->camera->get_cosz (), -vue->camera->get_sinz ());
        v1.x_rotate (v1, vue->camera->get_cosx (), -vue->camera->get_sinx ());
        v1.y_rotate (v1, vue->camera->get_cosy (), -vue->camera->get_siny ());
        v1.get_coordinates (&tmpx, &tmpy, &tmpz);
        x = x + tmpx * 5.;
        y = y + tmpy * 5.;
        z = z + tmpz * 5.;
      }
      else
        break;
      vue->camera->set_position (x + tmpx, y + tmpy, z + tmpz);
      vue->camera->set_target (x + tmpx + cx, y + tmpy + cy, z + tmpz + cz);
      BUG (m3d_get_rect (&xmin, &xmax, &ymin, &ymax, p), FALSE)
    } while (fabs ((ymin + ymax) / 2. - (ymin2 + ymax2) / 2.) > 1.);
    
    // On zoom autant que possible de tel sorte que la structure tienne au plus
    // juste dans la fenêtre.
    BUG (m3d_get_rect (&xmin, &xmax, &ymin, &ymax, p), FALSE)
    dz = yymin / 5.; // On commence par avancer d'1/5 de la distance maximale
                     // autorisée.
    do
    {
      // On avance de dz dans la direction de la caméra.
      vue->camera->set_position (x + dz * cx, y + dz * cy, z + dz * cz);
      vue->camera->set_target (x + dz * cx + cx,
                               y + dz * cy + cy,
                               z + dz * cz + cz);
      BUG (m3d_get_rect (&xmin2, &xmax2, &ymin2, &ymax2, p), FALSE)
      // 1er cas : étude des abscisses.
      // Le choix de l'interpolation est : f(x) = (a*x+b)/(c*x+d).
      // avec les conditions suivantes : f(-inf) = 0. En effet, si on recule au
      // maximum, la largeur de la vue sera nulle. Soit a/c = 0. On peut donc
      // fixer forfaitairement c = 1 et a à 0.
      // On se retrouve donc avec la fonction f(x) = b/(x+d)
      // Droite (b/(X+d)=Y) passant en X=z et Y= xmax-xmin=x1
      //                 X=z+dz et Y = xmax2-xmin2=x2
      // On obtient : b = (dz*x1*x2)/(x1-x2) et c = ((x2-x1)*z+dz*x2)/(x1-x2)
      // Le nouveau y est obtenu en cherchant f(x)=allocation.width
      // Ainsi, on obtient une valeur optimale de dz = dz*x2*(x1-w)/((x1-x2)*w)
      // On fait le même calcul pour les ordonnées.
      // Ensuite, on retient la valeur de dz minimale.
      if (((!errmax (xmax - xmin - (xmax2 - xmin2), allocation.width)) ||
           ((xmax - xmin < 1.) &&
            (xmax2 - xmin2 < 1.) &&
            (fabs (allocation.width - xmax - xmin) < 1.) &&
            (fabs (allocation.width - xmax2 - xmin2) < 1.))) &&
          ((!errrel (ymax - ymin - (ymax2 - ymin2), allocation.height)) ||
           ((ymax - ymin < 1.) &&
            (ymax2 - ymin2 < 1.) &&
            (ABS (allocation.height - ymax - ymin) < 1.) &&
            (ABS (allocation.height - ymax2 - ymin2) < 1.))))
      {
        // Ici, les conditions (xmax-xmin < 0.5) && (xmax2-xmin2 < 0.5) &&
        // (fabs(allocation.width-xmax-xmin) < 1.) &&
        // (fabs(allocation.width-xmax2-xmin2) < 1.) sont là au cas où tous les
        // noeuds sont alignés parfaitement à la vertical. Par exemple un
        // schéma 2D en XZ avec la vue en YZ.
        dztmp = NAN;
        if (!errrel (xmax - xmin - (xmax2 - xmin2), allocation.width))
          dztmp = dz * (xmax2 - xmin2) * (xmax - xmin - allocation.width) /
                  ((xmax - xmin - (xmax2 - xmin2)) * allocation.width) / 5.;
        if (!errrel (ymax - ymin - (ymax2 - ymin2), allocation.height))
        {
          if (isnan (dztmp))
            dztmp = dz * (ymax2 - ymin2) * (ymax - ymin - allocation.height) /
                    ((ymax - ymin - (ymax2 - ymin2)) * allocation.height) / 5.;
          else
            dztmp = MIN(dz * (ymax2 - ymin2) * (ymax - ymin -
                                                allocation.height) /
                          ((ymax - ymin - (ymax2 - ymin2)) * allocation.height)
                          / 5.,
                        dztmp);
        }
        dz = dztmp;
        // L'extrapolation dit qu'il faut plutôt avancer de dz*5.
        // Il est nécessaire de brider les déplacements pour éviter que
        // l'estimation ne mette un point derrière la caméra. On recule un tout
        // petit peu plus pour éviter que le point le plus proche de la caméra
        // ne se trouve dans le plan XZ.
        // Normalement, ça ne devrait pas se produire mais c'est une sécurité.
        if (dz * 5. > yymin)
          // Oups, on avance trop. On bride à yymin / 5.5 * 5.
          dz = yymin / 5.5;
        x = x + dz * cx * 5.;
        y = y + dz * cy * 5.;
        z = z + dz * cz * 5.;
        yymin = yymin - dz * 5;
        dz = yymin / 5.;
        // Maintenant, on ne peut pas avancer de plus de yymin.
      }
      else
        break;
      vue->camera->set_position (x, y, z);
      vue->camera->set_target (x + cx, y + cy, z + cz);
      BUG (m3d_get_rect (&xmin, &xmax, &ymin, &ymax, p), FALSE)
    } while ((fabs (xmax - xmin - allocation.width) > 1.) &&
             (fabs (ymax - ymin - allocation.height) > 1.));
    
    BUG (m3d_get_rect (&xmin2, &xmax2, &ymin2, &ymax2, p), FALSE)
    // Tant qu'une fois le zoom fini, le dessin n'est pas centré.
    i++;
    if (i == 100)
      break;
  } while ((fabs (xmax2 + xmin2 - allocation.width) > 1.) ||
           (fabs (ymax2 + ymin2 - allocation.height) > 1.));
  
  BUG (m3d_rafraichit (p), FALSE)
  
  if (i == 100)
    FAILCRIT (FALSE,
              (gettext ("La fonction \"zoom tout\" vient de tourner en boucle.\n"));)
  
  return TRUE;
}

#define M3D_CAMERA(NOM, CX, CY, CZ) \
gboolean \
m3d_camera_axe_##NOM (Projet *p) \
{ \
  SGlobalData  *vue; \
  double        x, y, z; \
  EF_Noeud     *noeud; \
  EF_Point      point; \
  GtkAllocation allocation; \
  \
  BUGPARAM (p, "%p", p, FALSE) \
  \
  BUGCRIT (UI_GTK.window, \
           FALSE, \
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"), \
                     "principale");) \
  \
  vue = (SGlobalData *) UI_M3D.data; \
  \
  if (p->modele.noeuds == NULL) \
    return TRUE; \
  \
  noeud = (EF_Noeud *) p->modele.noeuds->data; \
  BUG (EF_noeuds_renvoie_position (noeud, &point), FALSE) \
  \
  gtk_widget_get_allocation (GTK_WIDGET (UI_M3D.drawing), &allocation); \
  x = 0; \
  y = 0; \
  z = 0; \
  vue->camera->set_position (x, y, z); \
  vue->camera->set_target (x + CX, y + CY, z + CZ); \
  vue->camera->rotation_on_axe_of_view (0); \
  \
  BUG (m3d_camera_zoom_all (p), FALSE) \
  \
  return TRUE; \
}
/**
 * \def M3D_CAMERA(NOM, CX, CY, CZ)
 * \brief Positionne la caméra pour voir toute la structure dans le plan xz
 *        vers la direction y.
 * \param NOM : nom de la fonctior,
 * \param CX : direction de la caméra en x,
 * \param CY : direction de la caméra en y,
 * \param CZ : direction de la caméra en z,
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */


M3D_CAMERA (x_z_y,   0., 1.,   0.);
M3D_CAMERA (x_z__y,  0., -1.,  0.);
M3D_CAMERA (y_z_x,   1., 0.,   0.);
M3D_CAMERA (y_z__x, -1., 0.,   0.);
M3D_CAMERA (x_y_z,   0., 0.,   1.);
M3D_CAMERA (x_y__z,  0., 0.,  -1.);


gboolean
m3d_actualise_graphique (Projet *p,
                         GList  *noeuds,
                         GList *barres)
/**
 * \brief Met à jour l'affichage graphique en actualisant l'affichage des
 *        noeuds et barres passés en argument. Les listes contient une série de
 *        pointeurs.
 * \param p : la variable projet,
 * \param noeuds : Liste de pointeurs vers les noeuds à actualiser,
 * \param barres : Liste de pointeurs vers les barres à actualiser.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL.
 */
{
  GList *noeuds_dep, *barres_dep;
  GList *list_parcours;
  
  BUGPARAM (p, "%p", p, FALSE)
  
  BUG (_1992_1_1_barres_cherche_dependances (p,
                                             NULL,
                                             noeuds,
                                             NULL,
                                             NULL,
                                             NULL,
                                             barres,
                                             &noeuds_dep,
                                             &barres_dep,
                                             NULL,
                                             FALSE,
                                             TRUE),
       FALSE);
  
  list_parcours = noeuds_dep;
  while (list_parcours != NULL)
  {
    BUG (m3d_noeud (&UI_M3D, (EF_Noeud *) list_parcours->data),
         FALSE,
         g_list_free (noeuds_dep);
           g_list_free (barres_dep);)
    list_parcours = g_list_next (list_parcours);
  }
  g_list_free (noeuds_dep);
  
  list_parcours = barres_dep;
  while (list_parcours != NULL)
  {
    BUG (m3d_barre (&UI_M3D, (EF_Barre *) list_parcours->data),
         FALSE,
         g_list_free (barres_dep);)
    list_parcours = g_list_next (list_parcours);
  }
  g_list_free (barres_dep);
  
  return TRUE;
}


gboolean
m3d_rafraichit (Projet *p)
/**
 * \brief Force le rafraichissement de l'affichage graphique. Nécessaire après
 *        l'utilisation d'une des fonctions d'actualisation de l'affichage
 *        graphique.
 * \param p : la variable projet.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL.
 */
{
  SGlobalData *vue;
  CM3dVertex A,B;
  
  BUGPARAM (p, "%p", p, FALSE)
  vue = (SGlobalData *) UI_M3D.data;
  // On force l'actualisation de l'affichage
  vue->scene->rendering (*vue->camera);
  gtk_widget_queue_resize (UI_M3D.drawing);
  
  A = vue->camera->get_position ();
  B = vue->camera->get_target_vector ();

  return TRUE;
}


gboolean
m3d_noeud (void     *donnees_m3d,
           EF_Noeud *noeud)
/**
 * \brief Crée un noeud dans l'affichage graphique. Si le noeud existe, il est
 *        détruit au préalable.
 * \param donnees_m3d : données SGlobalData,
 * \param noeud : noeud à ajouter ou à actualiser.
 * \return
 *   Succès : Pointeur vers le nouvel objet noeud.\n
 *   Échec : NULL :
 *     - noeud == NULL,
 *     - donnees_m3d == NULL,
 *     - en cas d'erreur d'allocation mémoire.
 */
{
  CM3dCube     cube;
  CM3dObject  *cube_old;
  char        *nom;
  EF_Point     point;
  SGlobalData *vue;
  
  BUGPARAM (noeud, "%p", noeud, FALSE)
  BUGPARAM (donnees_m3d, "%p", donnees_m3d, FALSE)
  
  BUG (EF_noeuds_renvoie_position (noeud, &point), FALSE);
  BUGCRIT (nom = g_strdup_printf ("noeud %u", noeud->numero),
           FALSE,
           (gettext ("Erreur d'allocation mémoire.\n"));)
  
  vue = (SGlobalData *) ((Gtk_m3d *) donnees_m3d)->data;
  
  cube_old = vue->scene->get_object_by_name (nom);
  if (cube_old != NULL)
    vue->scene->remove_object (*cube_old);
  
  cube.set_all_datas (nom, .1);
  cube.set_ambient_reflexion (1.);
  cube.set_smooth (GOURAUD);
  cube.set_position (m_g (point.x), m_g (point.y), m_g (point.z));
  vue->scene->add_object (cube);
  
  free (nom);
  
  return TRUE;
}


void
m3d_noeud_free (void     *donnees_m3d,
                EF_Noeud *noeud)
/**
 * \brief Supprimer un noeud dans l'affichage graphique.
 * \param donnees_m3d : données SGlobalData,
 * \param *noeud : noeud à ajouter ou à actualiser.
 * \return
 *   Succès : Pointeur vers le nouvel objet noeud.\n
 *   Échec : NULL :
 *     - noeud == NULL,
 *     - donnees_m3d == NULL,
 *     - en cas d'erreur d'allocation mémoire.
 */
{
  CM3dObject  *cube;
  char        *nom;
  SGlobalData *vue;
  
  BUGPARAM (noeud, "%p", noeud, )
  BUGPARAM (donnees_m3d, "%p", donnees_m3d, )
  
  BUGCRIT (nom = g_strdup_printf ("noeud %u", noeud->numero),
           ,
           (gettext ("Erreur d'allocation mémoire.\n"));)
  
  vue = (SGlobalData *) ((Gtk_m3d *) donnees_m3d)->data;
  
  cube = vue->scene->get_object_by_name (nom);
  vue->scene->remove_object (*cube);
  
  free (nom);
  
  return;
}


gboolean
m3d_barre_finition (CM3dObject *objet,
                    EF_Barre   *barre)
/**
 * \brief Applique une liste d'instructions commune à une barre. Fonction
 *        interne à #m3d_barre.
 * \param objet : la modélisation de la barre,
 * \param barre : barre devant être représentée.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - barre == NULL,
 *     - objet == NULL.
 */
{
  double   y, z;
  double   x1, y1, z1;
  double   dx, dy, dz;
  EF_Point p_d, p_f;
  
  BUGPARAM (barre, "%p", barre, FALSE)
  BUGPARAM (objet, "%p", objet, FALSE)
  
  objet->rotations (m_g (barre->angle), 0., 0.);
  objet->set_color (100, 100, 100);
  objet->set_ambient_reflexion (0.8);
  objet->set_smooth (GOURAUD);
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
      objet->get_center ()->get_coordinates (&x1, &y1, &z1);
      break;
    }
    default :
    {
      FAILCRIT (FALSE,
                (gettext ("Type de section %d inconnu.\n"),
                          barre->section->type);)
      break;
    }
  }
  BUG (_1992_1_1_barres_angle_rotation (barre->noeud_debut,
                                        barre->noeud_fin,
                                        &y,
                                        &z),
       FALSE);
  objet->rotations (0., -y / M_PI * 180., z / M_PI * 180.);
  BUG (EF_noeuds_renvoie_position (barre->noeud_debut, &p_d), FALSE);
  BUG (EF_noeuds_renvoie_position (barre->noeud_fin, &p_f), FALSE);
  dx = (m_g (p_d.x) + m_g (p_f.x)) / 2.;
  dy = (m_g (p_d.y) + m_g (p_f.y)) / 2.;
  dz = (m_g (p_d.z) + m_g (p_f.z)) / 2.;
  objet->set_position (
    -(cos (z) * cos (y) * x1 - sin (z) * y1 - sin (y) * cos (z) * z1) + dx,
      sin (z) * cos (y) * x1 + cos (z) * y1 - sin (z) * sin (y) * z1 + dy,
      sin (y) * x1 + cos (y) * z1 + dz);
  
  return TRUE;
}


gboolean
m3d_barre (void     *donnees_m3d,
           EF_Barre *barre)
/**
 * \brief Crée une barre dans l'affichage graphique. Si la barre existe, elle
 *        est détruite au préalable.
 * \param donnees_m3d : données graphiques,
 * \param barre : barre devant être représentée.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - donnees_m3d == NULL,
 *     - barre == NULL,
 *     - si la longueur de la barre est nulle,
 *     - le type de barre est inconnu,
 *     - en cas d'erreur d'allocation mémoire.
 */
{
  SGlobalData *vue;
  CM3dObject  *objet;
  char        *tmp;
  CM3dObject  *tout;
  double       longueur;
  
  BUGPARAM (donnees_m3d, "%p", donnees_m3d, FALSE)
  BUGPARAM (barre, "%p", barre, FALSE)
  
  vue = (SGlobalData *) ((Gtk_m3d *) donnees_m3d)->data;
  
  // On supprime l'élément s'il existe déjà
  BUGCRIT (tmp = g_strdup_printf ("barre %u", barre->numero),
           FALSE,
           (gettext ("Erreur d'allocation mémoire.\n"));)
  
  objet = vue->scene->get_object_by_name(tmp);
  if (objet != NULL)
    vue->scene->remove_object(*objet);
  
  longueur = EF_noeuds_distance (barre->noeud_debut, barre->noeud_fin);
  BUG (!isnan (longueur), FALSE, free (tmp););
  
  if (errmoy (longueur, ERRMOY_DIST))
    return TRUE;
  
  switch (barre->section->type)
  {
    case SECTION_RECTANGULAIRE :
    {
      CM3dPlan   bas, haut, gauche, droite;
      Section_T *section = (Section_T *) barre->section->data;
      
      droite.set_all_datas ("", longueur, m_g (section->hauteur_retombee), 1);
      droite.rotations (180., 0., 0.);
      droite.set_position (0., -m_g (section->largeur_retombee) / 2., 0.);
      
      gauche.set_all_datas ("", longueur, m_g (section->hauteur_retombee), 1);
      gauche.set_position (0., m_g (section->largeur_retombee) / 2., 0.);
      
      bas.set_all_datas ("", longueur, m_g (section->largeur_retombee), 1);
      bas.rotations (90., 180., 0.);
      bas.set_position (0., 0., -m_g (section->hauteur_retombee) / 2.);
      
      haut.set_all_datas ("", longueur, m_g(section->largeur_retombee), 1);
      haut.rotations (90., 0., 0.);
      haut.set_position (0., 0., m_g (section->hauteur_retombee) / 2.);
      
      tout = M3d_object_new_group (tmp, &droite, &gauche, &bas, &haut, NULL);
      
      m3d_barre_finition (tout, barre);
      vue->scene->add_object (*tout);
      
      break;
    }
    case SECTION_T :
    {
      Section_T *section = (Section_T *) barre->section->data;
      
      double      lt   = m_g (section->largeur_table);
      double      lr   = m_g (section->largeur_retombee);
      double      ht   = m_g (section->hauteur_table);
      double      hr   = m_g (section->hauteur_retombee);
      double      cdgh = (lt * ht * ht / 2. + lr * hr * (ht + hr / 2.)) /
                         (lt * ht + lr * hr);
      double      cdgb = (ht + hr) - cdgh;
      
      CM3dPlan retombee_inf, retombee_droite, retombee_gauche,
               dalle_bas_droite, dalle_bas_gauche, dalle_droite,
               dalle_gauche, dalle_sup;
      
      retombee_inf.set_all_datas ("", longueur, lr, 1);
      retombee_inf.rotations (90., 180., 0.);
      retombee_inf.set_position (0., 0., -cdgb);
      
      retombee_droite.set_all_datas ("", longueur, hr, 1);
      retombee_droite.rotations (180., 0., 0.);
      retombee_droite.set_position (0., -lr / 2., -cdgb + hr / 2.);
      
      retombee_gauche.set_all_datas ("", longueur, hr, 1);
      retombee_gauche.set_position (0., lr / 2., -cdgb + hr / 2.);
      
      dalle_bas_gauche.set_all_datas ("", longueur, (lt - lr) / 2., 1);
      dalle_bas_gauche.rotations (90., 180., 0.);
      dalle_bas_gauche.set_position (0., lr / 2. + (lt - lr) / 4., -cdgb + hr);
      
      dalle_bas_droite.set_all_datas ("", longueur, (lt - lr) / 2., 1);
      dalle_bas_droite.rotations (90., 180., 0.);
      dalle_bas_droite.set_position (0.,
                                     -lr / 2. - (lt - lr) / 4.,
                                     -cdgb + hr);
      
      dalle_droite.set_all_datas ("", longueur, ht, 1);
      dalle_droite.rotations (180., 0., 0.);
      dalle_droite.set_position (0., -lt / 2., -cdgb + hr + ht / 2.);
      
      dalle_gauche.set_all_datas ("", longueur, ht, 1);
      dalle_gauche.set_position (0., lt / 2., -cdgb + hr + ht / 2.);
      
      dalle_sup.set_all_datas ("", longueur, lt, 1);
      dalle_sup.rotations (90., 0., 0.);
      dalle_sup.set_position (0., 0, -cdgb + hr + ht);
      
      tout = M3d_object_new_group (tmp,
                                   &retombee_inf,
                                   &retombee_droite,
                                   &retombee_gauche,
                                   &dalle_bas_droite,
                                   &dalle_bas_gauche,
                                   &dalle_droite,
                                   &dalle_gauche,
                                   &dalle_sup,
                                   NULL);
      
      m3d_barre_finition (tout, barre);
      vue->scene->add_object (*tout);
      
      break;
    }
    case SECTION_CARREE :
    {
      Section_T *section = (Section_T *) barre->section->data;
      CM3dPlan   bas, haut, gauche, droite;
      
      droite.set_all_datas ("", longueur, m_g (section->largeur_table), 1);
      droite.rotations (180., 0., 0.);
      droite.set_position (0., -m_g (section->largeur_table) / 2., 0.);
      
      gauche.set_all_datas ("", longueur, m_g (section->largeur_table), 1);
      gauche.set_position (0., m_g (section->largeur_table) / 2., 0.);
      
      bas.set_all_datas ("", longueur, m_g (section->largeur_table), 1);
      bas.rotations (90., 180., 0.);
      bas.set_position (0., 0., -m_g (section->largeur_table) / 2.);
      
      haut.set_all_datas ("", longueur, m_g (section->largeur_table), 1);
      haut.rotations (90., 0., 0.);
      haut.set_position (0., 0., m_g (section->largeur_table) / 2.);
      
      tout = M3d_object_new_group (tmp, &droite, &gauche, &bas, &haut, NULL);
      
      m3d_barre_finition (tout, barre);
      vue->scene->add_object (*tout);
      
      break;
    }
    case SECTION_CIRCULAIRE :
    {
      Section_Circulaire *section;
      CM3dCylinder        cylindre;
      
      section = (Section_Circulaire *) barre->section->data;
      
      cylindre.set_all_datas (tmp, m_g (section->diametre) / 2., longueur, 12);
      
      tout = M3d_object_new_group (tmp, &cylindre, NULL);
      tout->rotations (0., 0., 90.);
      
      m3d_barre_finition (tout, barre);
      vue->scene->add_object (*tout);
      
      break;
    }
    case SECTION_PERSONNALISEE :
    {
      Section_Personnalisee *section;
      double                 angle;
      
      std::list <std::list <EF_Point *> *>::iterator it;
      
      section = (Section_Personnalisee *) barre->section->data;
      tout = new CM3dObject ("");
      
      it = section->forme->begin ();

      while (it != section->forme->end ())
      {
        std::list <EF_Point *>          *forme_e;
        std::list <EF_Point *>::iterator it2;
        EF_Point *point1 = NULL, *point2 = NULL;
        double    somme_angle = 0.;
        double    angle1 = NAN, angle2 = NAN;
        // On force à faire un dernier passage après la fin de la liste
        // dans le but de fermer la forme.
        bool      last = false;
        
        // On commence par parcourir la liste des points pour savoir si le
        // dessin est réalisé dans le sens horaire ou anti-horaire. Ce point
        // est important car les plans ne sont dessinés que s'ils sont vus de
        // face.
        forme_e = *it;
        it2 = forme_e->begin ();
        while ((it2 != forme_e->end ()) || (last))
        {
          if (point2 == NULL)
            point2 = *it2;
          else
          {
            point1 = point2;
            if (!last)
              point2 = *it2;
            else
              point2 = *forme_e->begin ();
            
            BUGCRIT (point2,
                     FALSE,
                     (gettext ("Impossible\n"));
                       delete tout;)
            angle = atan2 (m_g (point2->y) - m_g (point1->y),
                           m_g (point2->x) - m_g (point1->x)) / M_PI * 180.;
            angle1 = angle2;
            angle2 = angle;
            if (!isnan (angle1))
            {
              angle = angle2 - angle1;
              if (angle > 180.)
                angle = angle - 180.;
              if (angle < -180.)
                angle = angle + 180.;
              
              somme_angle = somme_angle + angle;
            }
          }
          
          if (!last)
          {
            ++it2;
            if (it2 == forme_e->end ())
              last = true;
          }
          else
            last = false;
        }
        
        // Puis on dessine
        point1 = NULL;
        point2 = NULL;
        it2 = forme_e->begin ();
        last = false;
        while ((it2 != forme_e->end ()) || (last))
        {
          if (point2 == NULL)
            point2 = *it2;
          else
          {
            std::vector <CM3dPolygon*>::iterator it;
            CM3dPlan object_tmp;
            
            point1 = point2;
            if (!last)
              point2 = *it2;
            else
              point2 = *forme_e->begin ();
            BUGCRIT (point2,
                     FALSE,
                     (gettext ("Impossible\n"));
                       delete tout;)
            
            angle = atan2 (m_g (point2->y) - m_g (point1->y),
                           m_g (point2->x) - m_g (point1->x))
                    / M_PI * 180. - 180.;
            if (somme_angle < 0)
              angle = angle + 180.;
            
            object_tmp.set_all_datas ("",
                                      longueur,
                                      EF_points_distance (point1, point2),
                                      1);
            object_tmp.rotations (angle, 180., 0.);
            object_tmp.set_position (0.,
                                     (m_g (point2->y) + m_g (point1->y)) / 2.,
                                     (m_g (point2->x) + m_g (point1->x)) / 2.);
            
            for (it = object_tmp.get_list_of_polygons ().begin ();
                 it != object_tmp.get_list_of_polygons ().end ();
                 ++it)
            {
              CM3dPolygon *polygon = *it;
              
              tout->add_polygon (*polygon);
            }
          }
          
          if (!last)
          {
            ++it2;
            if (it2 == forme_e->end ())
              last = true;
          }
          else
            last = false;
        }
        
        ++it;
      }
      tout->rotations (90., 0., 0.);
      tout->set_name (tmp);
      
      m3d_barre_finition (tout, barre);
      vue->scene->add_object (*tout);
      
      break;
    }
    default :
    {
      FAILCRIT (FALSE,
                (gettext ("Type de section %d inconnu.\n"),
                          barre->section->type);
                  free (tmp);)
      break;
    }
  }
  
  free (tmp);
  
  return TRUE;
}


void
m3d_barre_free (void     *donnees_m3d,
                EF_Barre *barre)
/**
 * \brief Supprimer une barre dans l'affichage graphique.
 * \param donnees_m3d : données SGlobalData,
 * \param barre : barre à supprimer.
 * \return
 *   Succès : Pointeur vers le nouvel objet noeud.\n
 *   Échec : NULL :
 *     - noeud == NULL,
 *     - donnees_m3d == NULL,
 *     - en cas d'erreur d'allocation mémoire.
 */
{
  CM3dObject  *cube;
  char        *nom;
  SGlobalData *vue;
  
  BUGPARAM (barre, "%p", barre, )
  BUGPARAM (donnees_m3d, "%p", donnees_m3d, )
  
  BUGCRIT (nom = g_strdup_printf ("barre %u", barre->numero),
           ,
           (gettext ("Erreur d'allocation mémoire.\n"));)
  
  vue = (SGlobalData *) ((Gtk_m3d *) donnees_m3d)->data;
  
  cube = vue->scene->get_object_by_name (nom);
  vue->scene->remove_object (*cube);
  
  free (nom);
  
  return;
}


gboolean
m3d_free (Projet *p)
/**
 * \brief Libère l'espace mémoire alloué pour l'affichage graphique de la
 *        structure.
 * \param p : la variable projet.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL.
 */
{
  SGlobalData *global_data;
  BUGPARAM (p, "%p", p, FALSE)
  
  global_data = (SGlobalData *) UI_M3D.data;
  
  delete global_data->scene;
  delete global_data->camera;
  
  free (UI_M3D.data);
  UI_M3D.data = NULL;
  
  return TRUE;
}


#endif

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
