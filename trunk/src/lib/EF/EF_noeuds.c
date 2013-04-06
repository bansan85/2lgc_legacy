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
#include <libintl.h>
#include <locale.h>
#include <gmodule.h> 
#include <string.h>
#include <math.h>

#include "common_projet.h"
#include "common_erreurs.h"
#include "common_math.h"
#include "common_m3d.hpp"
#include "EF_calculs.h"
#include "1992_1_1_barres.h"
#ifdef ENABLE_GTK
#include "EF_gtk_appuis.h"
#endif

gboolean EF_noeuds_init(Projet *projet)
/* Description : Initialise la liste des noeuds.
 * Paramètres : Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL.
 */
{
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    // Trivial
    projet->modele.noeuds = NULL;
    
    return TRUE;
}


EF_Point *EF_noeuds_renvoie_position(EF_Noeud *noeud) 
/* Description : Renvoie un point contenant la position du noeud.
 *               La valeur de retour doit être libérée par l'appel à la fonction free();
 * Paramètres : EF_Noeud *noeud : le noeud à étudier.
 * Valeur renvoyée :
 *   Succès : pointeur vers la position du noeud.
 *   Échec : NULL en cas de paramètres invalides
 *             noeud == NULL,
 *             en cas d'erreur d'allocation mémoire.
 */
{
    EF_Point *retour;
    
    BUGMSG(noeud, NULL, gettext("Paramètre %s incorrect.\n"), "noeud");
    BUGMSG(retour = malloc(sizeof(EF_Point)), NULL, gettext("Erreur d'allocation mémoire.\n"));
    
    switch (noeud->type)
    {
        case NOEUD_LIBRE :
        {
            EF_Noeud_Libre  *tmp = noeud->data;
            
            if (tmp->relatif != NULL)
            {
                EF_Point        *p;
                
                BUG(p = EF_noeuds_renvoie_position(tmp->relatif), NULL);
                
                retour->x = common_math_add(p->x, tmp->x);
                retour->y = common_math_add(p->y, tmp->y);
                retour->z = common_math_add(p->z, tmp->z);
                
                free(p);
            }
            else
            {
                retour->x = tmp->x;
                retour->y = tmp->y;
                retour->z = tmp->z;
            }
            
            break;
        }
        case NOEUD_BARRE :
        {
            EF_Noeud_Barre  *data = noeud->data;
            EF_Point        *point1, *point2;
            
            BUG(point1 = EF_noeuds_renvoie_position(data->barre->noeud_debut), NULL);
            BUG(point2 = EF_noeuds_renvoie_position(data->barre->noeud_fin), NULL);
            
            retour->x = common_math_add(point1->x, common_math_dot_f(common_math_sub(point2->x, point1->x), data->position_relative_barre));
            retour->y = common_math_add(point1->y, common_math_dot_f(common_math_sub(point2->y, point1->y), data->position_relative_barre));
            retour->z = common_math_add(point1->z, common_math_dot_f(common_math_sub(point2->z, point1->z), data->position_relative_barre));
            
            free(point1);
            free(point2);
            
            break;
        }
        default :
        {
            BUGMSG(0, NULL, gettext("Le type de noeud %d est inconnu.\n"), noeud->type);
            break;
        }
    }
    
    return retour;
}


EF_Noeud *EF_noeuds_ajout_noeud_libre(Projet *projet, Flottant x, Flottant y, Flottant z,
  EF_Appui *appui, EF_Noeud *relatif)
/* Description : Ajouter un noeud à la liste des noeuds en lui attribuant le numéro suivant le
 *               dernier noeud existant.
 * Paramètres : Projet *projet : la variable projet,
 *            : Flottant x : position en x,
 *            : Flottant y : position en y,
 *            : Flottant z : position en z,
 *            : EF_Appui *appui : Pointeur vers l'appui, NULL si aucun,
 *            : EF_Noeud *relatif : Pointeur vers le noeud relatif, NULL si aucun.
 * Valeur renvoyée :
 *   Succès : Pointeur vers le nouveau noeud
 *   Échec : NULL en cas de paramètres invalides :
 *             projet == NULL,
 *             en cas d'erreur d'allocation mémoire.
 */
{
    EF_Noeud        *noeud_nouveau = malloc(sizeof(EF_Noeud));
    EF_Noeud_Libre  *data;
    
    BUGMSG(projet, NULL, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(noeud_nouveau, NULL, gettext("Erreur d'allocation mémoire.\n"));
    
    BUGMSG(data = malloc(sizeof(EF_Noeud_Libre)), NULL, gettext("Erreur d'allocation mémoire.\n"));
    
    // Trivial
    noeud_nouveau->type = NOEUD_LIBRE;
    noeud_nouveau->data = data;
    data->x = x;
    data->y = y;
    data->z = z;
    data->relatif = relatif;
    
    noeud_nouveau->appui = appui;
        
    if (projet->modele.noeuds == NULL)
        noeud_nouveau->numero = 0;
    else
        noeud_nouveau->numero = ((EF_Noeud *)g_list_last(projet->modele.noeuds)->data)->numero+1;
    
    BUG(EF_calculs_free(projet), NULL);
    
    projet->modele.noeuds = g_list_append(projet->modele.noeuds, noeud_nouveau);
    
#ifdef ENABLE_GTK
    BUG(m3d_noeud(&projet->list_gtk.m3d, noeud_nouveau), NULL);
    if (projet->list_gtk.ef_noeud.builder != NULL)
    {
        char    *tmp = NULL;
        gchar   xx[30], yy[30], zz[30];
        
        if (data->relatif != NULL)
            BUGMSG(tmp = g_strdup_printf("%d", data->relatif->numero), NULL, gettext("Erreur d'allocation mémoire.\n"));
        
        gtk_tree_store_append(projet->list_gtk.ef_noeud.tree_store_libre, &noeud_nouveau->Iter, NULL);
        common_math_double_to_char2(data->x, xx, DECIMAL_DISTANCE);
        common_math_double_to_char2(data->y, yy, DECIMAL_DISTANCE);
        common_math_double_to_char2(data->z, zz, DECIMAL_DISTANCE);
        gtk_tree_store_set(projet->list_gtk.ef_noeud.tree_store_libre, &noeud_nouveau->Iter, 0, noeud_nouveau->numero, 1, xx, 2, yy, 3, zz, 4, (data->relatif == NULL ? gettext("Aucun") : tmp), 5, (noeud_nouveau->appui == NULL ? gettext("Aucun") : noeud_nouveau->appui->nom), -1);
        
        free(tmp);
    }
#endif
    
    return noeud_nouveau;
}


EF_Noeud* EF_noeuds_ajout_noeud_barre(Projet *projet, Beton_Barre *barre,
  Flottant position_relative_barre, EF_Appui *appui)
/* Description : Ajouter un noeud à la liste des noeuds en lui attribuant le numéro suivant le
 *               dernier noeud existant. Ce noeud se situe à l'intérieur d'une barre et permet
 *               la discrétisation.
 * Paramètres : Projet *projet : la variable projet,
 *            : Beton_Barre *barre : barre qui contiendra le noeud intermédiaire,
 *            : Flottant position_relative_barre : position relative à l'intérieur de la barre
 *              (compris entre 0.0 et 1.0),
 *            : EF_Appui *appui : Pointeur vers l'appui, NULL si aucun.
 * Valeur renvoyée :
 *   Succès : Pointeur vers le nouveau noeud
 *   Échec : NULL :
 *             projet == NULL,
 *             en cas d'erreur d'allocation mémoire.
 */
{
    EF_Noeud        *noeud_nouveau = malloc(sizeof(EF_Noeud));
    EF_Noeud_Barre  *data;
    GList           *liste;
    
    BUGMSG(projet, NULL, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(noeud_nouveau, NULL, gettext("Erreur d'allocation mémoire.\n"));
    
    BUGMSG(data = malloc(sizeof(EF_Noeud_Barre)), NULL, gettext("Erreur d'allocation mémoire.\n"));
    
    // Trivial
    noeud_nouveau->type = NOEUD_BARRE;
    noeud_nouveau->data = data;
    data->barre = barre;
    data->position_relative_barre = position_relative_barre;
    
    noeud_nouveau->appui = appui;
        
    if (projet->modele.noeuds == NULL)
        noeud_nouveau->numero = 0;
    else
        noeud_nouveau->numero = ((EF_Noeud *)g_list_last(projet->modele.noeuds)->data)->numero+1;
    
    BUG(EF_calculs_free(projet), NULL);
    
    projet->modele.noeuds = g_list_append(projet->modele.noeuds, noeud_nouveau);
    
    barre->discretisation_element++;
    
    liste = barre->noeuds_intermediaires;
    while ((liste != NULL) && (common_math_get(((EF_Noeud_Barre*)liste->data)->position_relative_barre) < common_math_get(position_relative_barre)))
        liste = g_list_next(liste);
    
    barre->noeuds_intermediaires = g_list_insert_before(barre->noeuds_intermediaires, liste, noeud_nouveau);
    
    BUGMSG(barre->info_EF = realloc(barre->info_EF, sizeof(Barre_Info_EF)*(barre->discretisation_element+1)), NULL, gettext("Erreur d'allocation mémoire.\n"));
    memset(barre->info_EF, 0, sizeof(Barre_Info_EF)*(barre->discretisation_element+1));
    
#ifdef ENABLE_GTK
    BUG(m3d_noeud(&projet->list_gtk.m3d, noeud_nouveau), NULL);
    if (projet->list_gtk.ef_noeud.builder != NULL)
    {
        EF_Point    *point;
        gchar       tmp[30];
        
        BUG(point = EF_noeuds_renvoie_position(noeud_nouveau), NULL);
        gtk_tree_store_append(projet->list_gtk.ef_noeud.tree_store_barre, &noeud_nouveau->Iter, NULL);
        common_math_double_to_char2(data->position_relative_barre, tmp, DECIMAL_DISTANCE);
        gtk_tree_store_set(projet->list_gtk.ef_noeud.tree_store_barre, &noeud_nouveau->Iter, 0, noeud_nouveau->numero, 1, common_math_get(point->x), 2, common_math_get(point->y), 3, common_math_get(point->z), 4, (noeud_nouveau->appui == NULL ? gettext("Aucun") : noeud_nouveau->appui->nom), 5, data->barre->numero, 6, tmp, -1);
        
        free(point);
    }
#endif
    
    return noeud_nouveau;
}


gboolean EF_noeuds_min_max(Projet *projet, double *x_min, double *x_max, double *y_min,
  double *y_max, double *z_min, double *z_max)
/* Description : Détermine le cube contenant tous les points de la structure. 
 *               Une ou plusieurs valeurs min ou max peuvent valoir NULL.
 * Paramètres : Projet *projet : la variable projet,
 *            : double *x_min : coordonnée du point ayant la valeur x minimale,
 *            : double *x_max : coordonnée du point ayant la valeur x maximale,
 *            : double *y_max : coordonnée du point ayant la valeur y minimale,
 *            : double *y_max : coordonnée du point ayant la valeur y maximale,
 *            : double *z_max : coordonnée du point ayant la valeur z minimale,
 *            : double *z_max : coordonnée du point ayant la valeur z maximale.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL,
 *             list_size(projet->modele.noeuds) == 0.
 */
{
    GList       *list_parcours;
    EF_Noeud    *noeud;
    EF_Point    *point;
    double      x_mi, x_ma, y_mi, y_ma, z_mi, z_ma;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->modele.noeuds, FALSE, gettext("Aucun noeud n'est existant.\n"));
    
    list_parcours = projet->modele.noeuds;
    noeud = list_parcours->data;
    BUG(point = EF_noeuds_renvoie_position(noeud), FALSE);
    x_mi = common_math_get(point->x);
    x_ma = common_math_get(point->x);
    y_mi = common_math_get(point->y);
    y_ma = common_math_get(point->y);
    z_mi = common_math_get(point->z);
    z_ma = common_math_get(point->z);
    free(point);
    list_parcours = g_list_next(list_parcours);
    while (list_parcours != NULL)
    {
        noeud = list_parcours->data;
        BUG(point = EF_noeuds_renvoie_position(noeud), FALSE);
        
        if (common_math_get(point->x) < x_mi)
            x_mi = common_math_get(point->x);
        if (common_math_get(point->x) > x_ma)
            x_ma = common_math_get(point->x);
        if (common_math_get(point->y) < y_mi)
            y_mi = common_math_get(point->y);
        if (common_math_get(point->y) > y_ma)
            y_ma = common_math_get(point->y);
        if (common_math_get(point->z) < z_mi)
            z_mi = common_math_get(point->z);
        if (common_math_get(point->z) > z_ma)
            z_ma = common_math_get(point->z);
        
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
    
    return TRUE;
}


EF_Noeud* EF_noeuds_cherche_numero(Projet *projet, unsigned int numero, gboolean critique)
/* Description : Positionne dans la liste des noeuds le noeud souhaité et le renvoie.
 * Paramètres : Projet *projet : la variable projet,
 *            : unsigned int numero : le numéro du noeud.
 *            : gboolean critique : si TRUE alors BUGMSG, si FALSE alors return.
 * Valeur renvoyée :
 *   Succès : pointeur vers le noeud recherché
 *   Échec : NULL :
 *             projet == NULL,
 *             noeud introuvable
 */
{
    GList   *list_parcours;
    
    BUGMSG(projet, NULL, gettext("Paramètre %s incorrect.\n"), "projet");
    
    // Trivial
    list_parcours = projet->modele.noeuds;
    while (list_parcours != NULL)
    {
        EF_Noeud    *noeud = list_parcours->data;
        
        if (noeud->numero == numero)
            return noeud;
        
        list_parcours = g_list_next(list_parcours);
    }
    
    if (critique)
        BUGMSG(0, NULL, gettext("Noeud n°%u introuvable.\n"), numero);
    else
        return NULL;
}


gboolean EF_noeuds_change_pos_abs(Projet *projet, EF_Noeud *noeud, Flottant x, Flottant y,
  Flottant z)
/* Description : Change les coordonnées d'un noeud.
 * Paramètres : Projet *projet : la variable projet,
 *            : EF_Noeud *noeud : noeud à modifier,
 *            : Flottant x : la nouvelle coordonnée en x, peut être NAN si pas de modification,
 *            : Flottant y : la nouvelle coordonnée en y, peut être NAN si pas de modification,
 *            : Flottant z : la nouvelle coordonnée en z, peut être NAN si pas de modification.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL,
 *             noeud == NULL,
 *             noeud->type != NOEUD_LIBRE.
 */
{
    EF_Noeud_Libre  *point;
#ifdef ENABLE_GTK
    GList           *liste_noeuds = NULL;
#endif
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(noeud, FALSE, gettext("Paramètre %s incorrect.\n"), "noeud");
    BUGMSG(noeud->type == NOEUD_LIBRE, FALSE, gettext("Le type du noeud est incorrect.\n"));
    
    point = noeud->data;
    
    if (!isnan(common_math_get(x)))
        point->x = x;
    if (!isnan(common_math_get(y)))
        point->y = y;
    if (!isnan(common_math_get(z)))
        point->z = z;
    
#ifdef ENABLE_GTK
    liste_noeuds = g_list_append(liste_noeuds, noeud);
    
    BUG(m3d_actualise_graphique(projet, liste_noeuds, NULL), FALSE);
    BUG(m3d_rafraichit(projet), FALSE);
    
    g_list_free(liste_noeuds);
    
    if (projet->list_gtk.ef_noeud.builder != NULL)
    {
        gchar   xx[30], yy[30], zz[30];
        
        common_math_double_to_char2(point->x, xx, DECIMAL_DISTANCE);
        common_math_double_to_char2(point->y, yy, DECIMAL_DISTANCE);
        common_math_double_to_char2(point->z, zz, DECIMAL_DISTANCE);
        gtk_tree_store_set(projet->list_gtk.ef_noeud.tree_store_libre, &noeud->Iter, 1, xx, 2, yy, 3, zz, -1);
    }
#endif
    
    BUG(EF_calculs_free(projet), FALSE);
    
    return TRUE;
}


gboolean EF_noeuds_change_pos_relat(Projet *projet, EF_Noeud *noeud, Flottant pos)
/* Description : Change la coordonnée d'un noeud.
 * Paramètres : Projet *projet : la variable projet,
 *            : EF_Noeud *noeud : noeud à modifier,
 *            : double pos : la nouvelle position relative,
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL,
 *             noeud == NULL,
 *             noeud->type != NOEUD_BARRE,
 *             pos > 1.0 ou pos < 0.
 */
{
    EF_Noeud_Barre  *info;
    GList           *liste;
    double          avant, apres;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(noeud, FALSE, gettext("Paramètre %s incorrect.\n"), "noeud");
    BUGMSG(noeud->type == NOEUD_BARRE, FALSE, gettext("Le type du noeud est incorrect.\n"));
    BUGMSG((0. <= common_math_get(pos)) && (common_math_get(pos) <= 1.), FALSE, gettext("Paramètre %s incorrect.\n"), "pos");
    
    info = noeud->data;
    
    // On vérifie si il y a besoin de réorganiser l'ordre des noeuds intermédiaires.
    // On commence par récupérer les bornes qui ne nécessite pas de réorganisation.
    liste = g_list_find(info->barre->noeuds_intermediaires, noeud);
    if (g_list_previous(liste) == NULL)
        avant = 0.;
    else
    {
        EF_Noeud        *noeud2 = g_list_previous(liste)->data;
        EF_Noeud_Barre  *info2 = noeud2->data;
        
        avant = common_math_get(info2->position_relative_barre);
    }
    if (g_list_next(liste) == NULL)
        apres = 1.;
    else
    {
        EF_Noeud        *noeud2 = g_list_next(liste)->data;
        EF_Noeud_Barre  *info2 = noeud2->data;
        
        apres = common_math_get(info2->position_relative_barre);
    }
    if ((avant <= common_math_get(pos)) && (common_math_get(pos) <= apres))
        info->position_relative_barre = pos;
    // Il est nécessaire de réorganiser
    else
    {
        GList   *list_parcours;
        // On enlève l'élément de la liste
        info->barre->noeuds_intermediaires = g_list_remove_link(info->barre->noeuds_intermediaires, liste);
        
        // On parcours la liste pour l'insérer au bon endroit
        list_parcours = info->barre->noeuds_intermediaires;
        while ((list_parcours != NULL) && (common_math_get(((EF_Noeud_Barre*)list_parcours->data)->position_relative_barre) < common_math_get(pos)))
            list_parcours = g_list_next(list_parcours);
        
        info->barre->noeuds_intermediaires = g_list_insert_before(info->barre->noeuds_intermediaires, list_parcours, liste->data);
        
        g_list_free(liste);
    }
    
    info->position_relative_barre = pos;
    
#ifdef ENABLE_GTK
    liste = g_list_append(NULL, noeud);
    
    BUG(m3d_actualise_graphique(projet, liste, NULL), FALSE);
    BUG(m3d_rafraichit(projet), FALSE);
    
    g_list_free(liste);
    
    if (projet->list_gtk.ef_noeud.builder != NULL)
    {
        gchar   tmp[30];
        
        common_math_double_to_char2(info->position_relative_barre, tmp, DECIMAL_DISTANCE);
        gtk_tree_store_set(projet->list_gtk.ef_noeud.tree_store_barre, &noeud->Iter, 6, tmp, -1);
    }
#endif
    
    BUG(EF_calculs_free(projet), FALSE);
    
    return TRUE;
}


gboolean EF_noeuds_change_appui(Projet *projet, EF_Noeud *noeud, EF_Appui *appui)
/* Description : Change l'appui d'un noeud.
 * Paramètres : Projet *projet : la variable projet,
 *            : EF_Noeud *noeud : noeud à modifier,
 *            : EF_Appui *appui : le nouvel appui. NULL signifie aucun appui.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL,
 *             noeud == NULL.
 */
{
#ifdef ENABLE_GTK
    EF_Appui    *appui_old = noeud->appui;
#endif
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(noeud, FALSE, gettext("Paramètre %s incorrect.\n"), "noeud");
    
    noeud->appui = appui;
    
#ifdef ENABLE_GTK
    if (projet->list_gtk.ef_noeud.builder != NULL)
    {
        GtkTreeModel    *model;
        
        switch(noeud->type)
        {
            case NOEUD_LIBRE :
            {
                model = GTK_TREE_MODEL(projet->list_gtk.ef_noeud.tree_store_libre);
                break;
            }
            case NOEUD_BARRE :
            {
                model = GTK_TREE_MODEL(projet->list_gtk.ef_noeud.tree_store_barre);
                break;
            }
            default :
            {
                BUGMSG(NULL, FALSE, gettext("Le type de noeud %d est inconnu.\n"), noeud->type);
                break;
            }
        }
        
        if (appui == NULL)
            gtk_tree_store_set(GTK_TREE_STORE(model), &noeud->Iter, 5, gettext("Aucun"), -1);
        else
            gtk_tree_store_set(GTK_TREE_STORE(model), &noeud->Iter, 5, appui->nom, -1);
    }
    if (projet->list_gtk.ef_appuis.builder != NULL)
    {
        GtkTreeModel    *model;
        GtkTreeIter     Iter;
        
        if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION(gtk_builder_get_object(projet->list_gtk.ef_appuis.builder, "EF_appuis_treeview_select")), &model, &Iter))
        {
            char    *nom;
            
            gtk_tree_model_get(model, &Iter, 0, &nom, -1);
            
            if (((noeud->appui != NULL) && (strcmp(nom, noeud->appui->nom) == 0)) || ((appui_old != NULL) && (strcmp(nom, appui_old->nom) == 0)))
                EF_gtk_appuis_select_changed(NULL, projet);
            
            free(nom);
        }
    }
#endif
    
    return TRUE;
}


gboolean EF_noeuds_change_noeud_relatif(Projet *projet, EF_Noeud *noeud, EF_Noeud *relatif)
/* Description : Change le noeud relatif d'un noeud.
 * Paramètres : Projet *projet : la variable projet,
 *            : EF_Noeud *noeud : noeud à modifier,
 *            : EF_Noeud *relatif : le nouveau noeud relatif, NULL si aucun.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL,
 *             noeud == NULL,
 *             noeud->type != NOEUD_LIBRE.
 */
{
    EF_Noeud_Libre  *point;
    GList           *liste_noeuds = NULL, *noeuds_dep = NULL;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(noeud, FALSE, gettext("Paramètre %s incorrect.\n"), "noeud");
    BUGMSG(noeud->type == NOEUD_LIBRE, FALSE, gettext("Le type du noeud est incorrect.\n"));
    
    point = noeud->data;
    
    if (point->relatif == relatif)
        return TRUE;
    
    if (relatif != NULL)
    {
        liste_noeuds = g_list_append(NULL, relatif);
        BUG(_1992_1_1_barres_cherche_dependances(projet, NULL, liste_noeuds, NULL, NULL, NULL, NULL, &noeuds_dep, NULL, NULL, FALSE, TRUE), FALSE);
        
        if (g_list_find(noeuds_dep, noeud) != NULL)
        {
            g_list_free(noeuds_dep);
            g_list_free(liste_noeuds);
            BUGMSG(NULL, FALSE, gettext("Le noeud %d est déjà dépendant du noeud %d.\n"), relatif->numero, noeud->numero);
        }
        
        g_list_free(noeuds_dep);
        g_list_free(liste_noeuds);
        
        liste_noeuds = g_list_append(NULL, noeud);
        BUG(_1992_1_1_barres_cherche_dependances(projet, NULL, liste_noeuds, NULL, NULL, NULL, NULL, &noeuds_dep, NULL, NULL, FALSE, TRUE), FALSE);
        
        if (g_list_find(noeuds_dep, relatif) != NULL)
        {
            g_list_free(noeuds_dep);
            g_list_free(liste_noeuds);
            BUGMSG(NULL, FALSE, gettext("Le noeud %d est déjà dépendant du noeud %d.\n"), noeud->numero, relatif->numero);
        }
        
        g_list_free(noeuds_dep);
        g_list_free(liste_noeuds);
    }
    
    point->relatif = relatif;
    
    liste_noeuds = g_list_append(NULL, noeud);
#ifdef ENABLE_GTK
    BUG(m3d_actualise_graphique(projet, liste_noeuds, NULL), FALSE);
    BUG(m3d_rafraichit(projet), FALSE);
    
    if (projet->list_gtk.ef_noeud.builder != NULL)
    {
        if (relatif == NULL)
            gtk_tree_store_set(projet->list_gtk.ef_noeud.tree_store_libre, &noeud->Iter, 4, gettext("Aucun"), -1);
        else
        {
            char    *tmp;
            
            BUGMSG(tmp = g_strdup_printf("%d", relatif->numero), FALSE, gettext("Erreur d'allocation mémoire.\n"));
            
            gtk_tree_store_set(projet->list_gtk.ef_noeud.tree_store_libre, &noeud->Iter, 4, tmp, -1);
            free(tmp);
        }
    }
#endif
    
    g_list_free(liste_noeuds);
    
    BUG(EF_calculs_free(projet), FALSE);
    
    return TRUE;
}


double EF_noeuds_distance(EF_Noeud* n1, EF_Noeud* n2)
/* Description : Renvoie la distance entre deux noeuds.
 * Paramètres : EF_Noeud* n1 : noeud de départ,
 *            : EF_Noeud* n2 : noeud de fin.
 * Valeur renvoyée :
 *   Succès : distance entre les deux noeuds
 *   Échec : NAN :
 *             n1 == NULL,
 *             n2 == NULL.
 */
{
    EF_Point    *p1, *p2;
    double      x, y, z;
    
    // \end{verbatim}\texttt{distance }$= \sqrt{x^2+y^2+z^2}$\begin{verbatim}
    BUGMSG(n1, NAN, gettext("Paramètre %s incorrect.\n"), "n1");
    BUGMSG(n2, NAN, gettext("Paramètre %s incorrect.\n"), "n2");
    
    BUG(p1 = EF_noeuds_renvoie_position(n1), NAN);
    BUG(p2 = EF_noeuds_renvoie_position(n2), NAN);
    
    x = common_math_get(p2->x) - common_math_get(p1->x);
    y = common_math_get(p2->y) - common_math_get(p1->y);
    z = common_math_get(p2->z) - common_math_get(p1->z);
    
    free(p1);
    free(p2);
    
    return sqrt(x*x+y*y+z*z);
}


double EF_noeuds_distance_x_y_z(EF_Noeud* n1, EF_Noeud* n2, double *x, double *y, double *z)
/* Description : Renvoie la distance entre deux noeuds par retour de fonction et renvoie la
 *               distance entre deux noeuds selon les 3 axes par argument.
 * Paramètres : EF_Noeud* n1 : noeud de départ,
 *            : EF_Noeud* n2 : noeud de fin,
 *            : double *x : distance selon l'axe x,
 *            : double *y : distance selon l'axe y,
 *            : double *z : distance selon l'axe z.
 * Valeur renvoyée :
 *   Succès : distance entre les deux points.
 *   Échec : NAN :
 *             n1 == NULL,
 *             n2 == NULL,
 *             x == NULL,
 *             y == NULL,
 *             z == NULL.
 */
{
    EF_Point    *p1, *p2;
    
    // \end{verbatim}\texttt{distance }$= \sqrt{x^2+y^2+z^2}$\begin{verbatim}
    BUGMSG(n1, NAN, gettext("Paramètre %s incorrect.\n"), "n1");
    BUGMSG(n2, NAN, gettext("Paramètre %s incorrect.\n"), "n2");
    
    BUG(p1 = EF_noeuds_renvoie_position(n1), NAN);
    BUG(p2 = EF_noeuds_renvoie_position(n2), NAN);

    *x = common_math_get(p2->x) - common_math_get(p1->x);
    *y = common_math_get(p2->y) - common_math_get(p1->y);
    *z = common_math_get(p2->z) - common_math_get(p1->z);
    
    free(p1);
    free(p2);
    
    return sqrt((*x)*(*x)+(*y)*(*y)+(*z)*(*z));
}


void EF_noeuds_free_foreach(EF_Noeud *noeud, Projet *projet)
/* Description : Fonction permettant de libérer un noeud contenu dans une liste.
 * Paramètres : EF_Noeud *noeud : le noeud à libérer,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    if (noeud == NULL)
        return;
    
    if (noeud->type == NOEUD_BARRE)
    {
        EF_Noeud_Barre  *infos = noeud->data;
        unsigned int    i;
        
        infos->barre->noeuds_intermediaires = g_list_remove(infos->barre->noeuds_intermediaires, noeud);
        for (i=0;i<=infos->barre->discretisation_element;i++)
        {
            if (infos->barre->info_EF[i].matrice_rigidite_locale != NULL)
                cholmod_free_sparse(&infos->barre->info_EF[i].matrice_rigidite_locale, projet->calculs.c);
        }
        infos->barre->discretisation_element--;
        BUGMSG(infos->barre->info_EF = realloc(infos->barre->info_EF, sizeof(Barre_Info_EF)*(infos->barre->discretisation_element+1)), , gettext("Erreur d'allocation mémoire.\n"));
        memset(infos->barre->info_EF, 0, sizeof(Barre_Info_EF)*(infos->barre->discretisation_element+1));
    }
    
    free(noeud->data);
    
#ifdef ENABLE_GTK
    if (projet->list_gtk.ef_noeud.builder != NULL)
    {
        GtkTreeModel    *model;
        
        switch(noeud->type)
        {
            case NOEUD_LIBRE :
            {
                model = GTK_TREE_MODEL(projet->list_gtk.ef_noeud.tree_store_libre);
                break;
            }
            case NOEUD_BARRE :
            {
                model = GTK_TREE_MODEL(projet->list_gtk.ef_noeud.tree_store_barre);
                break;
            }
            default :
            {
                BUGMSG(NULL, , gettext("Le type de noeud %d est inconnu.\n"), noeud->type);
                break;
            }
        }
        
        gtk_tree_store_remove(GTK_TREE_STORE(model), &noeud->Iter);
    }
    m3d_noeud_free(&projet->list_gtk.m3d, noeud);
#endif
    free(noeud);
    
    return;
}


gboolean EF_noeuds_free(Projet *projet)
/* Description : Libère l'ensemble des noeuds et la liste les contenant.
 * Paramètres : Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL.
 */
{
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    
    BUG(EF_calculs_free(projet), FALSE);
    
    // Trivial
    g_list_foreach(projet->modele.noeuds, (GFunc)EF_noeuds_free_foreach, projet);
    g_list_free(projet->modele.noeuds);
    
    return TRUE;
}
