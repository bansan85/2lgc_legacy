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
#include <string.h>
#include <gmodule.h>

#include "common_projet.h"
#include "common_erreurs.h"
#include "common_selection.h"
#include "EF_noeuds.h"
#include "1992_1_1_barres.h"

#ifdef ENABLE_GTK
#include <gtk/gtk.h>
#include "common_m3d.hpp"
#endif

G_MODULE_EXPORT gboolean EF_appuis_init(Projet *projet)
/* Description : Initialise la liste des types d'appuis.
 * Paramètres : Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL.
 */
{
#ifdef ENABLE_GTK
    GtkTreeIter     iter;
#endif
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");

    // Trivial
    projet->ef_donnees.appuis = NULL;
    
#ifdef ENABLE_GTK
    projet->list_gtk.ef_appuis.liste_appuis = gtk_list_store_new(1, G_TYPE_STRING);
    gtk_list_store_append(projet->list_gtk.ef_appuis.liste_appuis, &iter);
    gtk_list_store_set(projet->list_gtk.ef_appuis.liste_appuis, &iter, 0, gettext("Aucun"), -1);
    projet->list_gtk.ef_appuis.liste_type_appui = gtk_list_store_new(1, G_TYPE_STRING);
    gtk_list_store_append(projet->list_gtk.ef_appuis.liste_type_appui, &iter);
    gtk_list_store_set(projet->list_gtk.ef_appuis.liste_type_appui, &iter, 0, gettext("Libre"), -1);
    gtk_list_store_append(projet->list_gtk.ef_appuis.liste_type_appui, &iter);
    gtk_list_store_set(projet->list_gtk.ef_appuis.liste_type_appui, &iter, 0, gettext("Bloqué"), -1);
#endif    
    return TRUE;
}


G_MODULE_EXPORT EF_Appui* EF_appuis_cherche_nom(Projet *projet, const char *nom,
  gboolean critique)
/* Description : Renvoie l'appui correspondant au numéro souhaité.
 * Paramètres : Projet *projet : la variable projet,
 *            : const char *nom : le nom de l'appui,
 *            : gboolean critique : TRUE si en cas d'échec, la fonction BUG est utilisée.
 * Valeur renvoyée :
 *   Succès : pointeur vers l'appui.
 *   Échec : NULL :
 *             projet == NULL,
 *             l'appui n'existe pas.
 */
{
    GList   *list_parcours;
    
    BUGMSG(projet, NULL, gettext("Paramètre %s incorrect.\n"), "projet");
    
    // Trivial
    list_parcours = projet->ef_donnees.appuis;
    while (list_parcours != NULL)
    {
        EF_Appui    *appui = list_parcours->data;
        
        if (strcmp(appui->nom, nom) == 0)
            return appui;
        
        list_parcours = g_list_next(list_parcours);
    }
    
    if (critique)
        BUGMSG(0, NULL, "Appui '%s' est introuvable.\n", nom);
    else
        return NULL;
}


G_MODULE_EXPORT gboolean EF_appuis_get_description(EF_Appui* appui, char **txt_uxa,
  char **txt_uya, char **txt_uza, char **txt_rxa, char **txt_rya, char **txt_rza)
/* Description : Renvoie la description d'un appui.
 * Paramètres : EF_Appui* appui : l'appui à décrire,
 *            : char **txt_uxa : description de ux,
 *            : char **txt_uya : description de ux,
 *            : char **txt_uza : description de uz,
 *            : char **txt_rxa : description de rx,
 *            : char **txt_rya : description de ry,
 *            : char **txt_rza : description de rz.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             appui == NULL,
 *             en cas d'erreur d'allocation mémoire.
 */
{
    const char  *txt_ux, *txt_uy, *txt_uz, *txt_rx, *txt_ry, *txt_rz;
    char        *txt_uxp, *txt_uyp, *txt_uzp, *txt_rxp, *txt_ryp, *txt_rzp;
    
    switch (appui->ux)
    {
        case EF_APPUI_LIBRE :
        {
            txt_ux = gettext("Libre");
            BUGMSG(appui->ux_donnees == NULL, FALSE, gettext("Le type d'appui de %s (%s) n'a pas à posséder de données.\n"), "ux", gettext("Libre"));
            txt_uxp = NULL;
            BUGMSG(*txt_uxa = g_strdup_printf("%s", txt_ux), FALSE, gettext("Erreur d'allocation mémoire.\n"));
            break;
        }
        case EF_APPUI_BLOQUE :
        {
            txt_ux = gettext("Bloqué");
            BUGMSG(appui->ux_donnees == NULL, FALSE, gettext("Le type d'appui de %s (%s) n'a pas à posséder de données.\n"), "ux", gettext("Bloqué"));
            txt_uxp = NULL;
            BUGMSG(*txt_uxa = g_strdup_printf("%s", txt_ux), FALSE, gettext("Erreur d'allocation mémoire.\n"));
            break;
        }
        default :
        {
            BUGMSG(NULL, FALSE, gettext("Le type d'appui de %s (%d) est inconnu.\n"), "ux", appui->ux);
        }
    }
    switch (appui->uy)
    {
        case EF_APPUI_LIBRE :
        {
            txt_uy = gettext("Libre");
            BUGMSG(appui->uy_donnees == NULL, FALSE, gettext("Le type d'appui de %s (%s) n'a pas à posséder de données.\n"), "uy", gettext("Libre"));
            txt_uyp = NULL;
            BUGMSG(*txt_uya = g_strdup_printf("%s", txt_uy), FALSE, gettext("Erreur d'allocation mémoire.\n"));
            break;
        }
        case EF_APPUI_BLOQUE :
        {
            txt_uy = gettext("Bloqué");
            BUGMSG(appui->uy_donnees == NULL, FALSE, gettext("Le type d'appui de %s (%s) n'a pas à posséder de données.\n"), "uy", gettext("Bloqué"));
            txt_uyp = NULL;
            BUGMSG(*txt_uya = g_strdup_printf("%s", txt_uy), FALSE, gettext("Erreur d'allocation mémoire.\n"));
            break;
        }
        default :
        {
            BUGMSG(NULL, FALSE, gettext("Le type d'appui de %s (%d) est inconnu.\n"), "uy", appui->ux);
        }
    }
    switch (appui->uz)
    {
        case EF_APPUI_LIBRE :
        {
            txt_uz = gettext("Libre");
            BUGMSG(appui->uz_donnees == NULL, FALSE, gettext("Le type d'appui de %s (%s) n'a pas à posséder de données.\n"), "uz", gettext("Libre"));
            txt_uzp = NULL;
            BUGMSG(*txt_uza = g_strdup_printf("%s", txt_uz), FALSE, gettext("Erreur d'allocation mémoire.\n"));
            break;
        }
        case EF_APPUI_BLOQUE :
        {
            txt_uz = gettext("Bloqué");
            BUGMSG(appui->uz_donnees == NULL, FALSE, gettext("Le type d'appui de %s (%s) n'a pas à posséder de données.\n"), "uz", gettext("Bloqué"));
            txt_uzp = NULL;
            BUGMSG(*txt_uza = g_strdup_printf("%s", txt_uz), FALSE, gettext("Erreur d'allocation mémoire.\n"));
            break;
        }
        default :
        {
            BUGMSG(NULL, FALSE, gettext("Le type d'appui de %s (%d) est inconnu.\n"), "uz", appui->ux);
        }
    }
    switch (appui->rx)
    {
        case EF_APPUI_LIBRE :
        {
            txt_rx = gettext("Libre");
            BUGMSG(appui->rx_donnees == NULL, FALSE, gettext("Le type d'appui de %s (%s) n'a pas à posséder de données.\n"), "rx", gettext("Libre"));
            txt_rxp = NULL;
            BUGMSG(*txt_rxa = g_strdup_printf("%s", txt_rx), FALSE, gettext("Erreur d'allocation mémoire.\n"));
            break;
        }
        case EF_APPUI_BLOQUE :
        {
            txt_rx = gettext("Bloqué");
            BUGMSG(appui->rx_donnees == NULL, FALSE, gettext("Le type d'appui de %s (%s) n'a pas à posséder de données.\n"), "rx", gettext("Bloqué"));
            txt_rxp = NULL;
            BUGMSG(*txt_rxa = g_strdup_printf("%s", txt_rx), FALSE, gettext("Erreur d'allocation mémoire.\n"));
            break;
        }
        default :
        {
            BUGMSG(NULL, FALSE, gettext("Le type d'appui de %s (%d) est inconnu.\n"), "rx", appui->ux);
        }
    }
    switch (appui->ry)
    {
        case EF_APPUI_LIBRE :
        {
            txt_ry = gettext("Libre");
            BUGMSG(appui->ry_donnees == NULL, FALSE, gettext("Le type d'appui de %s (%s) n'a pas à posséder de données.\n"), "ry", gettext("Libre"));
            txt_ryp = NULL;
            BUGMSG(*txt_rya = g_strdup_printf("%s", txt_ry), FALSE, gettext("Erreur d'allocation mémoire.\n"));
            break;
        }
        case EF_APPUI_BLOQUE :
        {
            txt_ry = gettext("Bloqué");
            BUGMSG(appui->ry_donnees == NULL, FALSE, gettext("Le type d'appui de %s (%s) n'a pas à posséder de données.\n"), "ry", gettext("Bloqué"));
            txt_ryp = NULL;
            BUGMSG(*txt_rya = g_strdup_printf("%s", txt_rx), FALSE, gettext("Erreur d'allocation mémoire.\n"));
            break;
        }
        default :
        {
            BUGMSG(NULL, FALSE, gettext("Le type d'appui de %s (%d) est inconnu.\n"), "ry", appui->ux);
        }
    }
    switch (appui->rz)
    {
        case EF_APPUI_LIBRE :
        {
            txt_rz = gettext("Libre");
            BUGMSG(appui->rz_donnees == NULL, FALSE, gettext("Le type d'appui de %s (%s) n'a pas à posséder de données.\n"), "rz", gettext("Libre"));
            txt_rzp = NULL;
            BUGMSG(*txt_rza = g_strdup_printf("%s", txt_rz), FALSE, gettext("Erreur d'allocation mémoire.\n"));
            break;
        }
        case EF_APPUI_BLOQUE :
        {
            txt_rz = gettext("Bloqué");
            BUGMSG(appui->rz_donnees == NULL, FALSE, gettext("Le type d'appui de %s (%s) n'a pas à posséder de données.\n"), "rz", gettext("Bloqué"));
            txt_rzp = NULL;
            BUGMSG(*txt_rza = g_strdup_printf("%s", txt_rz), FALSE, gettext("Erreur d'allocation mémoire.\n"));
            break;
        }
        default :
        {
            BUGMSG(NULL, FALSE, gettext("Le type d'appui de %s (%d) est inconnu.\n"), "rz", appui->ux);
        }
    }
    
    free(txt_uxp);
    free(txt_uyp);
    free(txt_uzp);
    free(txt_rxp);
    free(txt_ryp);
    free(txt_rzp);
    
    return TRUE;
}


G_MODULE_EXPORT EF_Appui* EF_appuis_ajout(Projet *projet, const char *nom, Type_EF_Appui x,
  Type_EF_Appui y, Type_EF_Appui z, Type_EF_Appui rx, Type_EF_Appui ry, Type_EF_Appui rz)
/* Description : Ajoute un appui à la structure en lui attribuant le numéro suivant le dernier
 *                 appui existant.
 * Paramètres : Projet *projet : la variable projet
 *            : Type_EF_Appui x : définition du déplacement en x,
 *            : Type_EF_Appui y : définition du déplacement en y,
 *            : Type_EF_Appui z : définition du déplacement en z,
 *            : Type_EF_Appui rx : définition de la rotation autour de l'axe x,
 *            : Type_EF_Appui ry : définition de la rotation autour de l'axe y,
 *            : Type_EF_Appui rz : définition de la rotation autour de l'axe z.
 * Valeur renvoyée :
 *   Succès : pointeur vers le nouvel appui.
 *   Échec : NULL :
 *             projet == NULL,
 *             projet->ef_donnees.appuis == NULL,
 *             x, y, z, rx, ry, rz sont de type inconnu,
 *             en cas d'erreur d'allocation mémoire.
 */
{
    EF_Appui    *appui_nouveau, *appui_parcours;
    GList       *list_parcours;
    int         i = 1; // Le premier est le "Aucun"
#ifdef ENABLE_GTK
    char        *txt_uxa, *txt_uya, *txt_uza, *txt_rxa, *txt_rya, *txt_rza;
#endif
    
    BUGMSG(projet, NULL, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(strcmp(nom, gettext("Aucun")), NULL, gettext("Impossible d'utiliser comme nom 'Aucun'.\n"));
    
    BUGMSG(EF_appuis_cherche_nom(projet, nom, FALSE) == NULL, NULL, gettext("L'appui '%s' existe déjà.\n"), nom);
     
    BUGMSG(appui_nouveau = malloc(sizeof(EF_Appui)), NULL, gettext("Erreur d'allocation mémoire.\n"));
    // Trivial
    BUGMSG(appui_nouveau->nom =  g_strdup_printf("%s", nom), NULL, gettext("Erreur d'allocation mémoire.\n"));
    appui_nouveau->ux = x;
    switch (x)
    {
        case EF_APPUI_LIBRE :
        case EF_APPUI_BLOQUE :
        {
            appui_nouveau->ux_donnees = NULL;
            break;
        }
        default:
        {
            BUGMSG(0, NULL, gettext("Type d'appui %d inconnu.\n"), x);
            break;
        }
    }
    appui_nouveau->uy = y;
    switch (y)
    {
        case EF_APPUI_LIBRE :
        case EF_APPUI_BLOQUE :
        {
            appui_nouveau->uy_donnees = NULL;
            break;
        }
        default:
        {
            BUGMSG(0, NULL, gettext("Type d'appui %d inconnu.\n"), y);
            break;
        }
    }
    appui_nouveau->uz = z;
    switch (z)
    {
        case EF_APPUI_LIBRE :
        case EF_APPUI_BLOQUE :
        {
            appui_nouveau->uz_donnees = NULL;
            break;
        }
        default:
        {
            BUGMSG(0, NULL, gettext("Type d'appui %d inconnu.\n"), z);
            break;
        }
    }
    appui_nouveau->rx = rx;
    switch (rx)
    {
        case EF_APPUI_LIBRE :
        case EF_APPUI_BLOQUE :
        {
            appui_nouveau->rx_donnees = NULL;
            break;
        }
        default:
        {
            BUGMSG(0, NULL, gettext("Type d'appui %d inconnu.\n"), rx);
            break;
        }
    }
    appui_nouveau->ry = ry;
    switch (ry)
    {
        case EF_APPUI_LIBRE :
        case EF_APPUI_BLOQUE :
        {
            appui_nouveau->ry_donnees = NULL;
            break;
        }
        default:
        {
            BUGMSG(0, NULL, gettext("Type d'appui %d inconnu.\n"), ry);
            break;
        }
    }
    appui_nouveau->rz = rz;
    switch (rz)
    {
        case EF_APPUI_LIBRE :
        case EF_APPUI_BLOQUE :
        {
            appui_nouveau->rz_donnees = NULL;
            break;
        }
        default:
        {
            BUGMSG(0, NULL, gettext("Type d'appui %d inconnu.\n"), rz);
            break;
        }
    }
    
    list_parcours = projet->ef_donnees.appuis;
    while (list_parcours != NULL)
    {
        appui_parcours = list_parcours->data;
        if (strcmp(nom, appui_parcours->nom) < 0)
        {
#ifdef ENABLE_GTK
            gtk_list_store_insert(projet->list_gtk.ef_appuis.liste_appuis, &appui_nouveau->Iter_liste, i);
            gtk_list_store_set(projet->list_gtk.ef_appuis.liste_appuis, &appui_nouveau->Iter_liste, 0, nom, -1);
            if (projet->list_gtk.ef_appuis.builder != NULL)
            {
                if (g_list_previous(list_parcours) == NULL)
                    gtk_tree_store_prepend(GTK_TREE_STORE(gtk_builder_get_object(projet->list_gtk.ef_appuis.builder, "EF_appuis_treestore")), &appui_nouveau->Iter_fenetre, NULL);
                else
                    gtk_tree_store_insert_before(GTK_TREE_STORE(gtk_builder_get_object(projet->list_gtk.ef_appuis.builder, "EF_appuis_treestore")), &appui_nouveau->Iter_fenetre, NULL, &appui_parcours->Iter_fenetre);
                BUG(EF_appuis_get_description(appui_nouveau, &txt_uxa, &txt_uya, &txt_uza, &txt_rxa, &txt_rya, &txt_rza), NULL);
                gtk_tree_store_set(GTK_TREE_STORE(gtk_builder_get_object(projet->list_gtk.ef_appuis.builder, "EF_appuis_treestore")), &appui_nouveau->Iter_fenetre, 0, appui_nouveau->nom, 1, txt_uxa, 2, txt_uya, 3, txt_uza, 4, txt_rxa, 5, txt_rya, 6, txt_rza, -1);
                free(txt_uxa);
                free(txt_uya);
                free(txt_uza);
                free(txt_rxa);
                free(txt_rya);
                free(txt_rza);
            }
#endif
            projet->ef_donnees.appuis = g_list_insert_before(projet->ef_donnees.appuis, list_parcours, appui_nouveau);
            
            return appui_nouveau;
        }
        i++;
        list_parcours = g_list_next(list_parcours);
    }
    projet->ef_donnees.appuis = g_list_append(projet->ef_donnees.appuis, appui_nouveau);
#ifdef ENABLE_GTK
    gtk_list_store_append(projet->list_gtk.ef_appuis.liste_appuis, &appui_nouveau->Iter_liste);
    gtk_list_store_set(projet->list_gtk.ef_appuis.liste_appuis, &appui_nouveau->Iter_liste, 0, nom, -1);
    if (projet->list_gtk.ef_appuis.builder != NULL)
    {
        gtk_tree_store_append(GTK_TREE_STORE(gtk_builder_get_object(projet->list_gtk.ef_appuis.builder, "EF_appuis_treestore")), &appui_nouveau->Iter_fenetre, NULL);
        BUG(EF_appuis_get_description(appui_nouveau, &txt_uxa, &txt_uya, &txt_uza, &txt_rxa, &txt_rya, &txt_rza), NULL);
        gtk_tree_store_set(GTK_TREE_STORE(gtk_builder_get_object(projet->list_gtk.ef_appuis.builder, "EF_appuis_treestore")), &appui_nouveau->Iter_fenetre, 0, appui_nouveau->nom, 1, txt_uxa, 2, txt_uya, 3, txt_uza, 4, txt_rxa, 5, txt_rya, 6, txt_rza, -1);
        free(txt_uxa);
        free(txt_uya);
        free(txt_uza);
        free(txt_rxa);
        free(txt_rya);
        free(txt_rza);
    }
#endif
    
    return appui_nouveau;
}


G_MODULE_EXPORT gboolean EF_appuis_edit(EF_Appui *appui, int x, Type_EF_Appui type_x,
  Projet *projet)
/* Description : Modifie un appui.
 * Paramètres : EF_Appui *appui : appui à modifier,
 *            : int x : désigne le paramètre à modifier, 0 pour ux et 5 pour rz,
 *            : Type_EF_Appui type_x : le nouveau type,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL,
 *             appui == NULL,
 *             type_x inconnu.
 */
{
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(appui, FALSE, gettext("Paramètre %s incorrect.\n"), "appui");
    BUGMSG((type_x == EF_APPUI_LIBRE) || (type_x == EF_APPUI_BLOQUE), FALSE, gettext("Type d'appui %d inconnu.\n"), type_x);
    
    switch (x)
    {
        case 0 :
        {
            appui->ux = type_x;
            break;
        }
        case 1 :
        {
            appui->uy = type_x;
            break;
        }
        case 2 :
        {
            appui->uz = type_x;
            break;
        }
        case 3 :
        {
            appui->rx = type_x;
            break;
        }
        case 4 :
        {
            appui->ry = type_x;
            break;
        }
        case 5 :
        {
            appui->rz = type_x;
            break;
        }
        default :
        {
            BUGMSG(0, FALSE, gettext("Afin de modifier un appui, le paramètre x doit être compris entre 0 et 5 inclu.\n"));
            return FALSE;
        }
    }
    
#ifdef ENABLE_GTK
    if (projet->list_gtk.ef_appuis.builder != NULL)
    {
        if (type_x == EF_APPUI_LIBRE)
            gtk_tree_store_set(projet->list_gtk.ef_appuis.appuis, &appui->Iter_fenetre, x+1, gettext("Libre"), -1);
        else if (type_x == EF_APPUI_BLOQUE)
            gtk_tree_store_set(projet->list_gtk.ef_appuis.appuis, &appui->Iter_fenetre, x+1, gettext("Bloqué"), -1);
        else // Impossible
            return FALSE;
    }
#endif
    
    return TRUE;
}


G_MODULE_EXPORT gboolean EF_appuis_renomme(EF_Appui *appui, gchar *nom, Projet *projet)
/* Description : Renomme un appui.
 * Paramètres : EF_Appui *appui : appui à renommer,
 *            : const char *nom : le nouveau nom,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL,
 *             appui == NULL,
 *             appui possédant le nouveau nom est déjà existant.
 */
{
    GList       *list_parcours;
    gboolean    ajoute;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(appui, FALSE, gettext("Paramètre %s incorrect.\n"), "appui");
    BUGMSG(EF_appuis_cherche_nom(projet, nom, FALSE) == NULL, FALSE, gettext("L'appui '%s' existe déjà.\n"), nom);
    
    free(appui->nom);
    BUGMSG(appui->nom = g_strdup_printf("%s", nom), FALSE, gettext("Erreur d'allocation mémoire.\n"));
    projet->ef_donnees.appuis = g_list_remove(projet->ef_donnees.appuis, appui);
    
    // On réinsère l'appui au bon endroit
    list_parcours = projet->ef_donnees.appuis;
    ajoute = FALSE;
    while (list_parcours != NULL)
    {
        EF_Appui    *appui_parcours = list_parcours->data;
        
        if (strcmp(nom, appui_parcours->nom) < 0)
        {
            projet->ef_donnees.appuis = g_list_insert_before(projet->ef_donnees.appuis, list_parcours, appui);
            
            gtk_list_store_move_before(projet->list_gtk.ef_appuis.liste_appuis, &appui->Iter_liste, &appui_parcours->Iter_liste);
            if (projet->list_gtk.ef_appuis.builder != NULL)
                gtk_tree_store_move_before(projet->list_gtk.ef_appuis.appuis, &appui->Iter_fenetre, &appui_parcours->Iter_fenetre);
            list_parcours = NULL;
            ajoute = TRUE;
        }
        
        list_parcours = g_list_next(list_parcours);
    }
    if (ajoute == FALSE)
    {
        projet->ef_donnees.appuis = g_list_append(projet->ef_donnees.appuis, appui);
        
        gtk_list_store_move_before(projet->list_gtk.ef_appuis.liste_appuis, &appui->Iter_liste, NULL);
        if (projet->list_gtk.ef_appuis.builder != NULL)
            gtk_tree_store_move_before(projet->list_gtk.ef_appuis.appuis, &appui->Iter_fenetre, NULL);
    }
    
#ifdef ENABLE_GTK
    gtk_list_store_set(projet->list_gtk.ef_appuis.liste_appuis, &appui->Iter_liste, 0, nom, -1);
    if (projet->list_gtk.ef_appuis.builder != NULL)
    {
        GtkTreePath *path;
        
        gtk_tree_store_set(projet->list_gtk.ef_appuis.appuis, &appui->Iter_fenetre, 0, nom, -1);
        
        // On modifie la position de l'ascenseur la ligne reste visible même si elle sort de la
        // fenêtre.
        path = gtk_tree_model_get_path(gtk_tree_view_get_model(GTK_TREE_VIEW(gtk_builder_get_object(projet->list_gtk.ef_appuis.builder, "EF_appuis_treeview"))), &appui->Iter_fenetre);
        gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(gtk_builder_get_object(projet->list_gtk.ef_appuis.builder, "EF_appuis_treeview")), path, NULL, FALSE, 0., 0.);
        gtk_tree_path_free(path);
    }
    
    if (projet->list_gtk.ef_noeud.builder != NULL)
    {
        GtkTreeModel    *model;
        GList           *parcours;
        
        parcours = projet->ef_donnees.noeuds;
        while (parcours != NULL)
        {
            EF_Noeud    *noeud = parcours->data;
            
            if (noeud->type == NOEUD_LIBRE)
                model = GTK_TREE_MODEL(projet->list_gtk.ef_noeud.tree_store_libre);
            else
                model = GTK_TREE_MODEL(projet->list_gtk.ef_noeud.tree_store_barre);
            
            if (noeud->appui == appui)
                gtk_tree_store_set(GTK_TREE_STORE(model), &noeud->Iter, 4, appui->nom, -1);
            
            parcours = g_list_next(parcours);
        }
    }
#endif
    
    return TRUE;
}


G_MODULE_EXPORT gboolean EF_appuis_supprime(EF_Appui *appui, gboolean annule_si_utilise,
  gboolean supprime, Projet *projet)
/* Description : Supprime l'appui spécifié.
 * Paramètres : EF_Appui *appui : l'appui à supprimer,
 *            : gboolean annule_si_utilise : possibilité d'annuler la suppression si l'appui est
 *              attribué à un noeud. Si l'option est désactivée, les noeuds possédant l'appui
 *              seront modifiés en fonction du paramètre supprime.
 *            : gboolean supprime : Paramètre utilisé uniquement si annule_si_utilise == FALSE.
 *              Si TRUE alors, les noeuds (et les barres et noeuds intermédaires dépendants)
 *              utilisant l'appui seront supprimés. Si FALSE alors les noeuds deviendront sans
 *              appui.
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL,
 *             appui == NULL.
 */
{
    GList   *list_appuis = NULL, *list_parcours;
    GList   *noeuds_suppr;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(appui, FALSE, gettext("Paramètre %s incorrect.\n"), "appui");
    
    // On vérifie les dépendances.
    list_appuis = g_list_append(list_appuis, appui);
    BUG(_1992_1_1_barres_cherche_dependances(projet, list_appuis, NULL, NULL, NULL, NULL, NULL, &noeuds_suppr, NULL, NULL, FALSE, FALSE), FALSE);
    
    if ((annule_si_utilise) && (noeuds_suppr != NULL))
    {
        char *liste;
        
        liste = common_selection_converti_noeuds_en_texte(noeuds_suppr);
        if (g_list_next(noeuds_suppr) == NULL)
            BUGMSG(NULL, FALSE, gettext("Impossible de supprimer l'appui car il est utilisé par le noeud %s.\n"), liste);
        else
            BUGMSG(NULL, FALSE, gettext("Impossible de supprimer l'appui car il est utilisé par les noeuds %s.\n"), liste);
    }
    
    // On enlève l'appui pour les noeuds dépendants (si supprime == FALSE).
    if (supprime == FALSE)
    {
        list_parcours = noeuds_suppr;
        while (list_parcours != NULL)
        {
            EF_Noeud    *noeud = list_parcours->data;
            
            BUG(EF_noeuds_change_appui(projet, noeud, NULL), TRUE);
            
            list_parcours = g_list_next(list_parcours);
        }
    }
    else
        BUG(_1992_1_1_barres_supprime_liste(projet, noeuds_suppr, NULL), TRUE);
    
    g_list_free(list_appuis);
    g_list_free(noeuds_suppr);
    
    free(appui->nom);
    switch (appui->ux)
    {
        case EF_APPUI_LIBRE :
        case EF_APPUI_BLOQUE :
            break;
        default :
        {
            BUGMSG(NULL, FALSE, gettext("Le type d'appui de %s (%d) est inconnu.\n"), "ux", appui->ux);
        }
    }
    switch (appui->uy)
    {
        case EF_APPUI_LIBRE :
        case EF_APPUI_BLOQUE :
            break;
        default :
        {
            BUGMSG(NULL, FALSE, gettext("Le type d'appui de %s (%d) est inconnu.\n"), "uy", appui->ux);
        }
    }
    switch (appui->uz)
    {
        case EF_APPUI_LIBRE :
        case EF_APPUI_BLOQUE :
            break;
        default :
        {
            BUGMSG(NULL, FALSE, gettext("Le type d'appui de %s (%d) est inconnu.\n"), "uz", appui->ux);
        }
    }
    switch (appui->rx)
    {
        case EF_APPUI_LIBRE :
        case EF_APPUI_BLOQUE :
            break;
        default :
        {
            BUGMSG(NULL, FALSE, gettext("Le type d'appui de %s (%d) est inconnu.\n"), "rx", appui->ux);
        }
    }
    switch (appui->ry)
    {
        case EF_APPUI_LIBRE :
        case EF_APPUI_BLOQUE :
            break;
        default :
        {
            BUGMSG(NULL, FALSE, gettext("Le type d'appui de %s (%d) est inconnu.\n"), "ry", appui->ux);
        }
    }
    switch (appui->rz)
    {
        case EF_APPUI_LIBRE :
        case EF_APPUI_BLOQUE :
            break;
        default :
        {
            BUGMSG(NULL, FALSE, gettext("Le type d'appui de %s (%d) est inconnu.\n"), "rz", appui->ux);
        }
    }
    projet->ef_donnees.appuis = g_list_remove(projet->ef_donnees.appuis, appui);
    
#ifdef ENABLE_GTK
    gtk_list_store_remove(projet->list_gtk.ef_appuis.liste_appuis, &appui->Iter_liste);
    if (projet->list_gtk.ef_appuis.builder != NULL)
        gtk_tree_store_remove(projet->list_gtk.ef_appuis.appuis, &appui->Iter_fenetre);
#endif
    
    free(appui);
    
    return TRUE;
}


G_MODULE_EXPORT gboolean EF_appuis_free(Projet *projet)
/* Description : Libère l'ensemble des types d'appuis ainsi que la liste les contenant.
 * Paramètres : Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL.
 */
{
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    
    // Trivial
    while (projet->ef_donnees.appuis != NULL)
    {
        EF_Appui    *appui = projet->ef_donnees.appuis->data;
        
        projet->ef_donnees.appuis = g_list_delete_link(projet->ef_donnees.appuis, projet->ef_donnees.appuis);
        free(appui->nom);
        free(appui);
    }
    
    free(projet->ef_donnees.appuis);
    projet->ef_donnees.appuis = NULL;
    
#ifdef ENABLE_GTK
    g_object_unref(projet->list_gtk.ef_appuis.liste_appuis);
    g_object_unref(projet->list_gtk.ef_appuis.liste_type_appui);
#endif
    
    return TRUE;
}
