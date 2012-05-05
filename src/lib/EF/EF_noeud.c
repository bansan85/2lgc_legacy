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
#include <stdlib.h>
#include <math.h>
#include <libintl.h>
#include "common_projet.h"
#include "common_erreurs.h"
#include "EF_appuis.h"

G_MODULE_EXPORT int EF_noeuds_init(Projet *projet)
/* Description : Initialise la liste des noeuds
 * Paramètres : Projet *projet : la variable projet
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet == NULL)
 *           -2 en cas d'erreur d'allocation mémoire
 */
{
    BUGMSG(projet, -1, gettext("Paramètre incorrect\n"));
    // Trivial
    projet->ef_donnees.noeuds = NULL;
    
    return 0;
}


G_MODULE_EXPORT EF_Noeud *EF_noeuds_ajout_noeud_libre(Projet *projet, double x, double y, double z, EF_Appui *appui)
/* Description : Ajouter un noeud à la liste des noeuds en lui attribuant le numéro suivant le
 *                 dernier noeud existant
 * Paramètres : Projet *projet : la variable projet
 *            : double x : position en x
 *            : double y : position en y
 *            : double z : position en z
 *            : int appui : numéro de l'appui. -1 si aucun.
 * Valeur renvoyée :
 *   Succès : Pointeur vers le nouveau noeud
 *   Échec : NULL en cas de paramètres invalides :
 *             (projet == NULL) ou
 *             (projet->ef_donnees.noeuds == NULL) ou
 *             (EF_appuis_cherche_numero(projet, appui) == NULL)
 *           NULL en cas d'erreur d'allocation mémoire
 */
{
    EF_Noeud        *noeud_nouveau = malloc(sizeof(EF_Noeud));
    EF_Point        *data;
    
    BUGMSG(projet, NULL, gettext("Paramètre incorrect\n"));
    BUGMSG(noeud_nouveau, NULL, gettext("Erreur d'allocation mémoire.\n"));
    
    BUGMSG(data = malloc(sizeof(EF_Point)), NULL, gettext("Erreur d'allocation mémoire.\n"));
    
    // Trivial
    noeud_nouveau->type = NOEUD_LIBRE;
    noeud_nouveau->data = data;
    data->x = x;
    data->y = y;
    data->z = z;
    
    noeud_nouveau->appui = appui;
        
    noeud_nouveau->numero = g_list_length(projet->ef_donnees.noeuds);
    
    projet->ef_donnees.noeuds = g_list_append(projet->ef_donnees.noeuds, noeud_nouveau);
    
    if (projet->ef_donnees.noeuds_pos_partielle != NULL)
    {
        BUGMSG(projet->ef_donnees.noeuds_pos_partielle = (unsigned int**)realloc(projet->ef_donnees.noeuds_pos_partielle, sizeof(unsigned int*)*(noeud_nouveau->numero+1)), NULL, gettext("Erreur d'allocation mémoire.\n"));
        BUGMSG(projet->ef_donnees.noeuds_pos_partielle[noeud_nouveau->numero] = (unsigned int*)malloc(6*sizeof(unsigned int)), NULL, gettext("Erreur d'allocation mémoire.\n"));
    }
    if (projet->ef_donnees.noeuds_pos_complete != NULL)
    {
        BUGMSG(projet->ef_donnees.noeuds_pos_complete = (unsigned int**)realloc(projet->ef_donnees.noeuds_pos_complete, sizeof(unsigned int*)*(noeud_nouveau->numero+1)), NULL, gettext("Erreur d'allocation mémoire.\n"));
        BUGMSG(projet->ef_donnees.noeuds_pos_complete[noeud_nouveau->numero] = (unsigned int*)malloc(6*sizeof(unsigned int)), NULL, gettext("Erreur d'allocation mémoire.\n"));
    }
    
    return noeud_nouveau;
}


G_MODULE_EXPORT EF_Noeud* EF_noeuds_ajout_noeud_barre(Projet *projet, Beton_Barre *barre, double position_relative_barre, EF_Appui *appui)
/* Description : Ajouter un noeud à la liste des noeuds en lui attribuant le numéro suivant le
 *                 dernier noeud existant. Ce noeud se situe à l'intérieur d'une barre et
 *                 permet la discrétisation.
 * Paramètres : Projet *projet : la variable projet
 *            : double position_relative_barre : position relative à l'intérieur de la barre (compris entre 0.0 et 1.0)
 *            : int appui : numéro de l'appui. -1 si aucun.
 * Valeur renvoyée :
 *   Succès : Pointeur vers le nouveau noeud
 *   Échec : NULL en cas de paramètres invalides :
 *             (projet == NULL) ou
 *             (projet->ef_donnees.noeuds == NULL) ou
 *             (EF_appuis_cherche_numero(projet, appui) == NULL)
 *           NULL en cas d'erreur d'allocation mémoire
 */
{
    EF_Noeud        *noeud_nouveau = malloc(sizeof(EF_Noeud));
    EF_Noeud_Barre  *data;
    GList           *liste;
    
    BUGMSG(projet, NULL, gettext("Paramètre incorrect\n"));
    BUGMSG(projet->ef_donnees.noeuds, NULL, gettext("Paramètre incorrect\n"));
    BUGMSG(noeud_nouveau, NULL, gettext("Erreur d'allocation mémoire.\n"));
    
    BUGMSG(data = malloc(sizeof(EF_Noeud_Barre)), NULL, gettext("Erreur d'allocation mémoire.\n"));
    
    // Trivial
    noeud_nouveau->type = NOEUD_BARRE;
    noeud_nouveau->data = data;
    data->barre = barre;
    data->position_relative_barre = position_relative_barre;
    
    noeud_nouveau->appui = appui;
        
    noeud_nouveau->numero = g_list_length(projet->ef_donnees.noeuds);
    
    projet->ef_donnees.noeuds = g_list_append(projet->ef_donnees.noeuds, noeud_nouveau);
    
    if (projet->ef_donnees.noeuds_pos_partielle != NULL)
    {
        BUGMSG(projet->ef_donnees.noeuds_pos_partielle = (unsigned int**)realloc(projet->ef_donnees.noeuds_pos_partielle, sizeof(unsigned int*)*(noeud_nouveau->numero+1)), NULL, gettext("Erreur d'allocation mémoire.\n"));
        BUGMSG(projet->ef_donnees.noeuds_pos_partielle[noeud_nouveau->numero] = (unsigned int*)malloc(6*sizeof(unsigned int)), NULL, gettext("Erreur d'allocation mémoire.\n"));
    }
    if (projet->ef_donnees.noeuds_pos_complete != NULL)
    {
        BUGMSG(projet->ef_donnees.noeuds_pos_complete = (unsigned int**)realloc(projet->ef_donnees.noeuds_pos_complete, sizeof(unsigned int*)*(noeud_nouveau->numero+1)), NULL, gettext("Erreur d'allocation mémoire.\n"));
        BUGMSG(projet->ef_donnees.noeuds_pos_complete[noeud_nouveau->numero] = (unsigned int*)malloc(6*sizeof(unsigned int)), NULL, gettext("Erreur d'allocation mémoire.\n"));
    }
    
    barre->discretisation_element++;
    
    liste = barre->noeuds_intermediaires;
    while ((liste != NULL) && (((EF_Noeud_Barre*)liste->data)->position_relative_barre < position_relative_barre))
        liste = g_list_next(liste);
    
    if (liste == NULL)
        barre->noeuds_intermediaires = g_list_append(barre->noeuds_intermediaires, noeud_nouveau);
    else
        barre->noeuds_intermediaires = g_list_insert_before(barre->noeuds_intermediaires, liste, noeud_nouveau);
    
    BUGMSG(barre->info_EF = realloc(barre->info_EF, sizeof(Barre_Info_EF)*(barre->discretisation_element+1)), NULL, gettext("Erreur d'allocation mémoire.\n"));
    
    return noeud_nouveau;
}


G_MODULE_EXPORT EF_Point *EF_noeuds_renvoie_position(EF_Noeud *noeud) 
/* Description : Renvoie un point contenant la position du noeud.
 *               La valeur de retour doit être libérée par l'appel à la fonction free();
 * Paramètres : EF_Noeud *noeud : le noeud à étudier.
 * Valeur renvoyée :
 *   Succès : pointeur vers la position du noeud.
 *   Échec : NULL en cas de paramètres invalides
 *             (noeud == NULL)
 */
{
    EF_Point *retour;
    
    BUGMSG(noeud, NULL, gettext("Paramètre incorrect\n"));
    BUGMSG(retour = malloc(sizeof(EF_Point)), NULL, gettext("Erreur d'allocation mémoire.\n"));
    
    switch (noeud->type)
    {
        case NOEUD_LIBRE :
        {
            EF_Point    *tmp = noeud->data;
            
            retour->x = tmp->x;
            retour->y = tmp->y;
            retour->z = tmp->z;
            
            break;
        }
        case NOEUD_BARRE :
        {
            EF_Noeud_Barre  *data = noeud->data;
            EF_Point        *point1, *point2;
            
            BUG(point1 = EF_noeuds_renvoie_position(data->barre->noeud_debut), NULL);
            BUG(point2 = EF_noeuds_renvoie_position(data->barre->noeud_fin), NULL);
            
            retour->x = point1->x + (point2->x-point1->x)*data->position_relative_barre;
            retour->y = point1->y + (point2->y-point1->y)*data->position_relative_barre;
            retour->z = point1->z + (point2->z-point1->z)*data->position_relative_barre;
            free(point1);
            free(point2);
            
            break;
        }
        default :
        {
            BUGMSG(0, NULL, gettext("Paramètre incorrect\n"));
            break;
        }
    }
    
    return retour;
}


G_MODULE_EXPORT int EF_noeuds_min_max(Projet *projet, double *x_min, double *x_max, double *y_min,
  double *y_max, double *z_min, double *z_max)
/* Description : Détermine le cube contenant tous les points de la structure. 
 *               Une ou plusieurs valeurs min ou max peuvent valoir NULL.
 * Paramètres : Projet *projet : la variable projet
 *            : double *x_min : coordonnée du point ayant la valeur x minimale
 *            : double *x_max : coordonnée du point ayant la valeur x maximale
 *            : double *y_max : coordonnée du point ayant la valeur y minimale
 *            : double *y_max : coordonnée du point ayant la valeur y maximale
 *            : double *z_max : coordonnée du point ayant la valeur z minimale
 *            : double *z_max : coordonnée du point ayant la valeur z maximale
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet == NULL) ou
 *             (projet->ef_donnees.noeuds == NULL) ou
 *             (list_size(projet->ef_donnees.noeuds) == 0)
 */
{
    GList       *list_parcours;
    EF_Noeud    *noeud;
    EF_Point    *point;
    double      x_mi, x_ma, y_mi, y_ma, z_mi, z_ma;
    
    BUGMSG(projet, -1, gettext("Paramètre incorrect\n"));
    BUGMSG(projet->ef_donnees.noeuds, -1, gettext("Paramètre incorrect\n"));
    
    list_parcours = projet->ef_donnees.noeuds;
    noeud = list_parcours->data;
    BUG(point = EF_noeuds_renvoie_position(noeud), -3);
    x_mi = point->x;
    x_ma = point->x;
    y_mi = point->y;
    y_ma = point->y;
    z_mi = point->z;
    z_ma = point->z;
    free(point);
    list_parcours = g_list_next(list_parcours);
    while (list_parcours != NULL)
    {
        noeud = list_parcours->data;
        BUG(point = EF_noeuds_renvoie_position(noeud), -3);
        
        if (point->x < x_mi)
            x_mi = point->x;
        if (point->x > x_ma)
            x_ma = point->x;
        if (point->y < y_mi)
            y_mi = point->y;
        if (point->y > y_ma)
            y_ma = point->y;
        if (point->z < z_mi)
            z_mi = point->z;
        if (point->z > z_ma)
            z_ma = point->z;
        
        free(point);
        
        list_parcours = g_list_next(list_parcours);
    }
    
    if (x_min != NULL)
        *x_min = x_mi;
    if (x_max != NULL)
        *x_max = x_ma;
    if (y_min != NULL)
        *y_min = y_mi;
    if (y_max != NULL)
        *y_max = y_ma;
    if (z_min != NULL)
        *z_min = z_mi;
    if (z_max != NULL)
        *z_max = z_ma;
    
    return 0;
}


G_MODULE_EXPORT EF_Noeud* EF_noeuds_cherche_numero(Projet *projet, unsigned int numero)
/* Description : Positionne dans la liste des noeuds le noeud souhaité et le renvoie
 * Paramètres : Projet *projet : la variable projet
 *            : int numero : le numéro du noeud
 * Valeur renvoyée :
 *   Succès : pointeur vers le noeud recherché
 *   Échec : NULL en cas de paramètres invalides :
 *             (projet == NULL) ou
 *             (projet->ef_donnees.noeuds == NULL) ou
 *             (list_size(projet->ef_donnees.noeuds) == 0) ou
 *             noeud introuvable
 */
{
    GList   *list_parcours;
    
    BUGMSG(projet, NULL, gettext("Paramètre incorrect\n"));
    BUGMSG(projet->ef_donnees.noeuds, NULL, gettext("Paramètre incorrect\n"));
    
    // Trivial
    list_parcours = projet->ef_donnees.noeuds;
    do
    {
        EF_Noeud    *noeud = list_parcours->data;
        
        if (noeud->numero == numero)
            return noeud;
        
        list_parcours = g_list_next(list_parcours);
    }
    while (list_parcours != NULL);
    
    BUGMSG(0, NULL, gettext("Noeud n°%d introuvable.\n"), numero);
}


G_MODULE_EXPORT double EF_noeuds_distance(EF_Noeud* n1, EF_Noeud* n2)
/* Description : Renvoie la distance entre deux noeuds.
 * Paramètres : EF_Noeud* n1 : noeud de départ
 *            : EF_Noeud* n2 : noeud de fin
 * Valeur renvoyée :
 *   Succès : distance entre les deux noeuds
 *   Échec : NAN en cas de paramètres invalides :
 *             (projet == NULL) ou
 *             (projet->ef_donnees.noeuds == NULL)
 */
{
    EF_Point    *p1, *p2;
    double      x, y, z;
    
    // \end{verbatim}\texttt{distance }$= \sqrt{x^2+y^2+z^2}$\begin{verbatim}
    BUGMSG(n1, NAN, gettext("Paramètre incorrect\n"));
    BUGMSG(n2, NAN, gettext("Paramètre incorrect\n"));
    
    BUG(p1 = EF_noeuds_renvoie_position(n1), NAN);
    BUG(p2 = EF_noeuds_renvoie_position(n2), NAN);
    
    x = p2->x - p1->x;
    y = p2->y - p1->y;
    z = p2->z - p1->z;
    
    free(p1);
    free(p2);
    
    return sqrt(x*x+y*y+z*z);
}


G_MODULE_EXPORT double EF_noeuds_distance_x_y_z(EF_Noeud* n1, EF_Noeud* n2, double *x, double *y, double *z)
/* Description : Renvoie la distance entre deux noeuds par retour de fonction et renvoie la
 *               distance entre deux noeuds selon les 3 axes par argument.
 * Paramètres : EF_Noeud* n1 : noeud de départ
 *            : EF_Noeud* n2 : noeud de fin
 *            : double *x : distance selon l'axe x,
 *            : double *y : distance selon l'axe y,
 *            : double *z : distance selon l'axe z.
 * Valeur renvoyée :
 *   Succès : distance entre les deux points.
 *   Échec : NAN en cas de paramètres invalides :
 *             (n1 == NULL) || 
 *             (n2 == NULL)
 */
{
    EF_Point    *p1, *p2;
    
    // \end{verbatim}\texttt{distance }$= \sqrt{x^2+y^2+z^2}$\begin{verbatim}
    BUGMSG(n1, NAN, gettext("Paramètre incorrect\n"));
    BUGMSG(n2, NAN, gettext("Paramètre incorrect\n"));
    
    BUG(p1 = EF_noeuds_renvoie_position(n1), NAN);
    BUG(p2 = EF_noeuds_renvoie_position(n2), NAN);

    *x = p2->x - p1->x;
    *y = p2->y - p1->y;
    *z = p2->z - p1->z;
    
    free(p1);
    free(p2);
    
    return sqrt((*x)*(*x)+(*y)*(*y)+(*z)*(*z));
}


G_MODULE_EXPORT int EF_noeuds_free(Projet *projet)
/* Description : Libère l'ensemble des noeuds et la liste les contenant.
 * Paramètres : Projet *projet : la variable projet
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet == NULL) ou
 *             (projet->ef_donnees.noeuds == NULL)
 */
{
    BUGMSG(projet, -1, gettext("Paramètre incorrect\n"));
    BUGMSG(projet->ef_donnees.noeuds, -1, gettext("Paramètre incorrect\n"));
    
    // Trivial
    while (projet->ef_donnees.noeuds != NULL)
    {
        EF_Noeud    *noeud = projet->ef_donnees.noeuds->data;
        
        free(noeud->data);
        free(noeud);
        
        projet->ef_donnees.noeuds = g_list_delete_link(projet->ef_donnees.noeuds, projet->ef_donnees.noeuds);
    }
    
    return 0;
}
