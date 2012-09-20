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
#include "common_maths.h"
#include "EF_noeud.h"
#include "1992_1_1_barres.h"
#include "common_m3d.hpp"

G_MODULE_EXPORT gboolean m3d_init(Projet *projet)
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


G_MODULE_EXPORT gboolean m3d_configure_event(GtkWidget *drawing __attribute__((unused)),
  GdkEventConfigure *ev, gpointer *data2)
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


G_MODULE_EXPORT gboolean m3d_draw(GtkWidget *drawing,
  GdkEventExpose* ev __attribute__((unused)), gpointer *data)
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


G_MODULE_EXPORT gboolean m3d_camera_axe_x_z(Projet *projet)
/* Description : Positionne la caméra pour voir toute la structure dans le plan xz.
 * Paramètres : Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : TRUE. Ne fait rien si (list_size(projet->ef_donnees.noeuds) <= 1)
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
    if (g_list_length(projet->ef_donnees.noeuds) <= 1)
        return TRUE;
    BUG(EF_noeuds_min_max(projet, &x_min, &x_max, NULL, NULL, &z_min, &z_max), FALSE);
    
    x = (x_min+x_max)/2.;
    z = (z_min+z_max)/2.;
    
    list_parcours = projet->ef_donnees.noeuds;
    noeud = (EF_Noeud *)list_parcours->data;
    BUG(point = EF_noeuds_renvoie_position(noeud), FALSE);
    y = point->y-sqrt((point->x-x)*(point->x-x)+(point->z-z)*(point->z-z));
    free(point);
    
    list_parcours = g_list_next(list_parcours);
    while (list_parcours != NULL)
    {
        noeud = (EF_Noeud *)list_parcours->data;
        BUG(point = EF_noeuds_renvoie_position(noeud), FALSE);
        y = MIN(y, point->y-sqrt((point->x-x)*(point->x-x)+(point->z-z)*(point->z-z)));
        free(point);
        list_parcours = g_list_next(list_parcours);
    }
    
    m3d = &projet->list_gtk.m3d;
    BUGMSG(m3d->data, FALSE, gettext("Paramètre %s incorrect.\n"), "m3d->data");
    vue = (SGlobalData*)m3d->data;
    
    if (vue->camera == NULL)
        vue->camera = new CM3dCamera (x, y*1.1, z, x, 0., z, 90, (int)(x_max-x_min), (int)(z_max-z_min));
    else
    {
        vue->camera->set_position(x, y, z);
        vue->camera->set_target(x, y+1, z);
    }
    
    return TRUE;
}


G_MODULE_EXPORT gboolean m3d_actualise_graphique(Projet *projet, GList *noeuds, GList *barres)
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
    
    BUG(_1992_1_1_barres_cherche_dependances(projet, noeuds, barres, &noeuds_dep, &barres_dep), FALSE);
    
    list_parcours = noeuds_dep;
    while (list_parcours != NULL)
    {
        BUG(m3d_noeud(&projet->list_gtk.m3d, (EF_Noeud *)list_parcours->data), FALSE);
        list_parcours = g_list_next(list_parcours);
    }
    
    list_parcours = barres_dep;
    while (list_parcours != NULL)
    {
        BUG(m3d_barre(&projet->list_gtk.m3d, (Beton_Barre *)list_parcours->data), FALSE);
        list_parcours = g_list_next(list_parcours);
    }
    
    return TRUE;
}


G_MODULE_EXPORT gboolean m3d_rafraichit(Projet *projet)
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


G_MODULE_EXPORT void* m3d_noeud(void *donnees_m3d, EF_Noeud *noeud)
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
    cube->set_position(point->x, point->y, point->z);
    
    free(nom);
    free(point);
    
    return cube;
}


G_MODULE_EXPORT void m3d_noeud_free(void *donnees_m3d, EF_Noeud *noeud)
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


G_MODULE_EXPORT void* m3d_barre(void *donnees_m3d, Beton_Barre *barre)
/* Description : Crée une barre dans l'affichage graphique. Si la barre existe, elle est
 *               détruite au préalable.
 * Paramètres : void *donnees_m3d : données graphiques,
 *              Beton_Barre *barre : barre devant être représentée.
 * Valeur renvoyée :
 *   Succès : Pointeur vers le cube.
 *   Échec : NULL :
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
    
    BUGMSG(donnees_m3d, NULL, gettext("Paramètre %s incorrect.\n"), "donnees_m3d");
    BUGMSG(barre, NULL, gettext("Paramètre %s incorrect.\n"), "barre");
    
    m3d = (Gtk_m3d *)donnees_m3d;
    vue = (SGlobalData*)m3d->data;
    
    // On supprime l'élément s'il existe déjà
    BUGMSG(tmp = g_strdup_printf("barre %u", barre->numero), NULL, gettext("Erreur d'allocation mémoire.\n"));

    objet = vue->scene->get_object_by_name(tmp);
    if (objet != NULL)
        vue->scene->remove_object(objet);
    
    // On l'(ré)ajoute
    longueur = EF_noeuds_distance(barre->noeud_debut, barre->noeud_fin);
    BUG(!isnan(longueur), NULL);
            
    switch (barre->section->type)
    {
        EF_Point *p_d, *p_f;
        case SECTION_RECTANGULAIRE :
        {
            double      y, z;
            CM3dObject  *bas, *haut, *gauche, *droite;
            Section_Rectangulaire *section = (Section_Rectangulaire *)barre->section->data;
            
            droite = M3d_plan_new("", longueur, section->hauteur, 1);
            droite->rotations(180., 0., 0.);
            droite->set_position(0., -section->largeur/2., 0.);
            
            gauche = M3d_plan_new("", longueur, section->hauteur, 1);
            gauche->set_position(0., section->largeur/2., 0.);
            
            bas = M3d_plan_new("", longueur, section->largeur, 1);
            bas->rotations(90., 180., 0.);
            bas->set_position(0., 0., -section->hauteur/2.);
            
            haut = M3d_plan_new("", longueur, section->largeur, 1);
            haut->rotations(90., 0., 0.);
            haut->set_position(0., 0., section->hauteur/2.);
            
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
            BUG(_1992_1_1_barres_angle_rotation(barre, &y, &z), NULL);
            tout->rotations(0., -y/M_PI*180., z/M_PI*180.);
            BUG(p_d = EF_noeuds_renvoie_position(barre->noeud_debut), NULL);
            BUG(p_f = EF_noeuds_renvoie_position(barre->noeud_fin), NULL);
            tout->set_position((p_d->x+p_f->x)/2., (p_d->y+p_f->y)/2., (p_d->z+p_f->z)/2.);
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
            double  la = section->largeur_ame;
            double  ht = section->hauteur_table;
            double  ha = section->hauteur_ame;
            double  cdgh = (lt*ht*ht/2.+la*ha*(ht+ha/2.))/(lt*ht+la*ha);
            double  cdgb = (ht+ha)-cdgh;
            
            CM3dObject  *ame_inf, *ame_droite, *ame_gauche, *dalle_bas_droite, *dalle_bas_gauche, *dalle_droite, *dalle_gauche, *dalle_sup;
            
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
            
            delete ame_inf;
            delete ame_droite;
            delete ame_gauche;
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
            BUG(_1992_1_1_barres_angle_rotation(barre, &y, &z), NULL);
            tout->rotations(0., -y/M_PI*180., z/M_PI*180.);
            BUG(p_d = EF_noeuds_renvoie_position(barre->noeud_debut), NULL);
            BUG(p_f = EF_noeuds_renvoie_position(barre->noeud_fin), NULL);
            tout->set_position((p_d->x+p_f->x)/2., (p_d->y+p_f->y)/2., (p_d->z+p_f->z)/2.);
            free(p_d);
            free(p_f);
            
            vue->scene->add_object(tout);
            
            break;
            
        }
        case SECTION_CARRE :
        {
            Section_Carre *section = (Section_Carre *)barre->section->data;
            double  y, z;
            CM3dObject  *bas, *haut, *gauche, *droite;
            
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
            BUG(_1992_1_1_barres_angle_rotation(barre, &y, &z), NULL);
            tout->rotations(0., -y/M_PI*180., z/M_PI*180.);
            BUG(p_d = EF_noeuds_renvoie_position(barre->noeud_debut), NULL);
            BUG(p_f = EF_noeuds_renvoie_position(barre->noeud_fin), NULL);
            tout->set_position((p_d->x+p_f->x)/2., (p_d->y+p_f->y)/2., (p_d->z+p_f->z)/2.);
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
            BUG(_1992_1_1_barres_angle_rotation(barre, &y, &z), NULL);
            tout->rotations(0., -y/M_PI*180., z/M_PI*180.);
            BUG(p_d = EF_noeuds_renvoie_position(barre->noeud_debut), NULL);
            BUG(p_f = EF_noeuds_renvoie_position(barre->noeud_fin), NULL);
            tout->set_position((p_d->x+p_f->x)/2., (p_d->y+p_f->y)/2., (p_d->z+p_f->z)/2.);
            free(p_d);
            free(p_f);
            
            vue->scene->add_object(tout);
            
            break;
            
        }
        default :
        {
            BUGMSG(0, NULL, gettext("Type de section %d inconnu.\n"), barre->section->type);
            break;
        }
    }
    
    free(tmp);
    
    return tout;
}


G_MODULE_EXPORT void m3d_barre_free(void *donnees_m3d, Beton_Barre *barre)
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


G_MODULE_EXPORT gboolean m3d_free(Projet *projet)
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
