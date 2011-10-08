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
#include <libintl.h>
#include "common_projet.h"
#include "common_erreurs.h"
#include "EF_appuis.h"

int EF_appuis_init(Projet *projet)
/* Description : Initialise la liste des types d'appuis
 * Paramètres : Projet *projet : la variable projet
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet == NULL)
 *           -2 en cas d'erreur d'allocation mémoire
 */
{
    BUGMSG(projet, -1, "EF_appuis_init\n");
    
    // Trivial
    projet->ef_donnees.appuis = list_init();
    BUGMSG(projet->ef_donnees.appuis, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_appuis_init");
    return 0;
}


int EF_appuis_ajout(Projet *projet, Type_EF_Appui x, Type_EF_Appui y, Type_EF_Appui z,
  Type_EF_Appui rx, Type_EF_Appui ry, Type_EF_Appui rz)
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
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet == NULL) ou
 *             (projet->ef_donnees.appuis == NULL) ou
 *             (x, y, z, rx, ry, rz sont de type inconnu)
 *           -2 en cas d'erreur d'allocation mémoire
 */
{
    EF_Appui        *appui_en_cours, appui_nouveau;
    
    BUGMSG(projet, -1, "EF_appuis_ajout\n");
    BUGMSG(projet->ef_donnees.appuis, -1, "EF_appuis_ajout\n");
    
    // Trivial
    list_mvrear(projet->ef_donnees.appuis);
    appui_nouveau.x = x;
    switch (x)
    {
        case EF_APPUI_LIBRE :
        case EF_APPUI_BLOQUE :
        {
            appui_nouveau.x_donnees = NULL;
            break;
        }
        default:
        {
            BUGMSG(0, -1, "EF_appuis_ajout : x %d\n", x);
            break;
        }
    }
    appui_nouveau.y = y;
    switch (y)
    {
        case EF_APPUI_LIBRE :
        case EF_APPUI_BLOQUE :
        {
            appui_nouveau.y_donnees = NULL;
            break;
        }
        default:
        {
            BUGMSG(0, -1, "EF_appuis_ajout : y %d\n", y);
            break;
        }
    }
    appui_nouveau.z = z;
    switch (z)
    {
        case EF_APPUI_LIBRE :
        case EF_APPUI_BLOQUE :
        {
            appui_nouveau.z_donnees = NULL;
            break;
        }
        default:
        {
            BUGMSG(0, -1, "EF_appuis_ajout : z %d\n", z);
            break;
        }
    }
    appui_nouveau.rx = rx;
    switch (rx)
    {
        case EF_APPUI_LIBRE :
        case EF_APPUI_BLOQUE :
        {
            appui_nouveau.rx_donnees = NULL;
            break;
        }
        default:
        {
            BUGMSG(0, -1, "EF_appuis_ajout : rx %d\n", rx);
            break;
        }
    }
    appui_nouveau.ry = ry;
    switch (ry)
    {
        case EF_APPUI_LIBRE :
        case EF_APPUI_BLOQUE :
        {
            appui_nouveau.ry_donnees = NULL;
            break;
        }
        default:
        {
            BUGMSG(0, -1, "EF_appuis_ajout : ry %d\n", ry);
            break;
        }
    }
    appui_nouveau.rz = rz;
    switch (rz)
    {
        case EF_APPUI_LIBRE :
        case EF_APPUI_BLOQUE :
        {
            appui_nouveau.rz_donnees = NULL;
            break;
        }
        default:
        {
            BUGMSG(0, -1, "EF_appuis_ajout : rz %d\n", rz);
            break;
        }
    }
    
    appui_en_cours = (EF_Appui *)list_rear(projet->ef_donnees.appuis);
    if (appui_en_cours == NULL)
        appui_nouveau.numero = 0;
    else
        appui_nouveau.numero = appui_en_cours->numero+1;
    
    BUGMSG(list_insert_after(projet->ef_donnees.appuis, &(appui_nouveau), sizeof(appui_nouveau)), -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_appuis_ajout");
    
    return 0;
}


EF_Appui* EF_appuis_cherche_numero(Projet *projet, int numero)
/* Description : Renvoie l'appui correspondant au numéro souhaité
 * Paramètres : Projet *projet : la variable projet
 *            : int numero : le numéro de l'appui
 * Valeur renvoyée :
 *   Succès : pointeur vers l'appui
 *   Échec : NULL en cas de paramètres invalides :
 *             (projet == NULL) ou
 *             (projet->ef_donnees.appuis == NULL) ou
 *             (list_size(projet->ef_donnees.appuis) == 0) ou
 *             l'appui n'existe pas.
 */
{
    BUGMSG(projet, NULL, "EF_appuis_cherche_numero\n");
    BUGMSG(projet->ef_donnees.appuis, NULL, "EF_appuis_cherche_numero\n");
    BUGMSG(list_size(projet->ef_donnees.appuis), NULL, "EF_appuis_cherche_numero\n");
    
    // Trivial
    list_mvfront(projet->ef_donnees.appuis);
    do
    {
        EF_Appui    *appui = (EF_Appui*)list_curr(projet->ef_donnees.appuis);
        
        if (appui->numero == numero)
            return appui;
    }
    while (list_mvnext(projet->ef_donnees.appuis) != NULL);
    
    BUGMSG(0, NULL, "EF_appuis_cherche_numero : numero %d\n", numero);
}


int EF_appuis_free(Projet *projet)
/* Description : Libère l'ensemble des types d'appuis ainsi que la liste les contenant
 * Paramètres : Projet *projet : la variable projet
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet == NULL) ou
 *             (projet->ef_donnees.appuis == NULL)
 */
{
    BUGMSG(projet, -1, "EF_appuis_cherche_numero\n");
    BUGMSG(projet->ef_donnees.appuis, -1, "EF_appuis_cherche_numero\n");
    
    // Trivial
    while (!list_empty(projet->ef_donnees.appuis))
    {
        EF_Appui    *appui = (EF_Appui*)list_remove_front(projet->ef_donnees.appuis);
        
        free(appui);
    }
    
    free(projet->ef_donnees.appuis);
    projet->ef_donnees.appuis = NULL;
    
    return 0;
}

