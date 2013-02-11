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
#include <math.h>

#include "common_projet.h"
#include "common_erreurs.h"
#include "common_math.h"
#include "common_selection.h"
#include "1992_1_1_barres.h"


gboolean EF_verif_bloc(GList *noeuds, GList *barres, GList **noeuds_dep, GList **barres_dep,
  Projet *projet)
/* Description : Renvoie l'ensemble des noeuds et barres étant connectés aux noeuds et barres.
 * Paramètres : GList *noeuds : noeuds définissant l'ensemble de départ,
 *              GList *barres : barres définissant l'ensemble de départ,
 *              GList **noeuds_dep : noeuds de l'ensemble du bloc,
 *              GList **barres_dep : barres de l'ensemble du bloc,
 *              Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE
 */
{
    GList   *noeuds_todo = NULL, *barres_todo = NULL;
    GList   *list_parcours;
    
    BUGMSG(noeuds_dep, FALSE, gettext("Paramètre %s incorrect.\n"), "noeuds_dep");
    BUGMSG(barres_dep, FALSE, gettext("Paramètre %s incorrect.\n"), "barres_dep");
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    
    *noeuds_dep = NULL;
    *barres_dep = NULL;
    
    list_parcours = noeuds;
    while (list_parcours != NULL)
    {
        if (g_list_find(noeuds_todo, list_parcours->data) == NULL)
            noeuds_todo = g_list_append(noeuds_todo, list_parcours->data);
        list_parcours = g_list_next(list_parcours);
    }
    
    list_parcours = barres;
    while (list_parcours != NULL)
    {
        if (g_list_find(barres_todo, list_parcours->data) == NULL)
            barres_todo = g_list_append(barres_todo, list_parcours->data);
        list_parcours = g_list_next(list_parcours);
    }
    
    while ((noeuds_todo != NULL) || (barres_todo != NULL))
    {
        while (noeuds_todo != NULL)
        {
            if (g_list_find(*noeuds_dep, noeuds_todo->data) == NULL)
                *noeuds_dep = g_list_append(*noeuds_dep, noeuds_todo->data);
            
            list_parcours = projet->modele.barres;
            while (list_parcours != NULL)
            {
                Beton_Barre *barre = list_parcours->data;
                
                if ((barre->noeud_debut == noeuds_todo->data) || (barre->noeud_fin == noeuds_todo->data) || (g_list_find(barre->noeuds_intermediaires, noeuds_todo->data) != NULL))
                {
                    if ((g_list_find(*barres_dep, barre) == NULL) && (g_list_find(barres_todo, barre) == NULL))
                        barres_todo = g_list_append(barres_todo, barre);
                }
                
                list_parcours = g_list_next(list_parcours);
            }
            
            noeuds_todo = g_list_delete_link(noeuds_todo, noeuds_todo);
        }
        while (barres_todo != NULL)
        {
            Beton_Barre *barre_parcours = barres_todo->data;
            
            if (g_list_find(*barres_dep, barres_todo->data) == NULL)
                *barres_dep = g_list_append(*barres_dep, barres_todo->data);
            
            if ((g_list_find(*noeuds_dep, barre_parcours->noeud_debut) == NULL) && (g_list_find(noeuds_todo, barre_parcours->noeud_debut) == NULL))
                noeuds_todo = g_list_append(noeuds_todo, barre_parcours->noeud_debut);
            if ((g_list_find(*noeuds_dep, barre_parcours->noeud_fin) == NULL) && (g_list_find(noeuds_todo, barre_parcours->noeud_fin) == NULL))
                noeuds_todo = g_list_append(noeuds_todo, barre_parcours->noeud_fin);
            
            list_parcours = barre_parcours->noeuds_intermediaires;
            while (list_parcours != NULL)
            {
                if ((g_list_find(*noeuds_dep, list_parcours->data) == NULL) && (g_list_find(noeuds_todo, list_parcours->data) == NULL))
                    noeuds_todo = g_list_append(noeuds_todo, list_parcours->data);
                
                list_parcours = g_list_next(list_parcours);
            }
            
            barres_todo = g_list_delete_link(barres_todo, barres_todo);
        }
    }
    
    return TRUE;
}


gboolean EF_verif_EF(Projet *projet, GList **rapport, int *erreur)
/* Description : Vérifie toutes les données utilisées dans les calculs.
 * Paramètres : Projet *projet : la variable projet,
 *              GList **rapport : liste d'Analyse_Comm,
 *              boolean **erreur : renvoie le plus haut niveau d'erreur rencontré.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE
 */
{
    Analyse_Comm    *ligne;
    GList           *list_parcours;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(rapport, FALSE, gettext("Paramètre %s incorrect.\n"), "rapport");
    BUGMSG(erreur, FALSE, gettext("Paramètre %s incorrect.\n"), "erreur");
    
    *rapport = NULL;
    *erreur = 0;
    
    // On vérifie si la structure possède au moins une barre (et donc deux noeuds).
    BUGMSG(ligne = malloc(sizeof(Analyse_Comm)), FALSE, gettext("Erreur d'allocation mémoire.\n"));
    BUGMSG(ligne->analyse = g_strdup_printf("%s", gettext("Structure non vide (minimum 2 noeuds et une barre)")), FALSE, gettext("Erreur d'allocation mémoire.\n"));
    ligne->resultat = 0;
    ligne->commentaire = NULL;
    if ((projet->modele.noeuds == NULL) || (g_list_next(projet->modele.noeuds) == NULL))
    {
        ligne->resultat = 2;
        if (*erreur < ligne->resultat)
            *erreur = ligne->resultat;
        BUGMSG(ligne->commentaire = g_strdup_printf("%s (%d).", gettext("Nombre de noeuds insuffisant"), g_list_length(projet->modele.noeuds)), FALSE, gettext("Erreur d'allocation mémoire.\n"));
    }
    if (projet->modele.barres == NULL)
    {
        ligne->resultat = 2;
        if (*erreur < ligne->resultat)
            *erreur = ligne->resultat;
        if (ligne->commentaire == NULL)
            BUGMSG(ligne->commentaire = g_strdup_printf("%s", gettext("Aucune barre n'est existante.")), FALSE, gettext("Erreur d'allocation mémoire.\n"));
        else
        {
            char    *tmp = ligne->commentaire;
            
            BUGMSG(ligne->commentaire = g_strdup_printf("%s\n%s", tmp, gettext("Aucune barre n'est existante.")), FALSE, gettext("Erreur d'allocation mémoire.\n"));
            free(tmp);
        }
    }
    *rapport = g_list_append(*rapport, ligne);
    
    // On vérifie si la structure est dans un seul bloc.
    BUGMSG(ligne = malloc(sizeof(Analyse_Comm)), FALSE, gettext("Erreur d'allocation mémoire.\n"));
    BUGMSG(ligne->analyse = g_strdup_printf("%s", gettext("Structure en un seul bloc")), FALSE, gettext("Erreur d'allocation mémoire.\n"));
    ligne->resultat = 0;
    ligne->commentaire = NULL;
    if (projet->modele.noeuds != NULL)
    {
        int     i = 1;
        GList   *noeuds, *barres, *tmp = NULL;
        GList   *noeuds_tout = NULL, *barres_tout = NULL;
        
        tmp = g_list_append(tmp, projet->modele.noeuds->data);
        BUG(EF_verif_bloc(tmp, NULL, &noeuds, &barres, projet), FALSE);
        g_list_free(tmp);
        
        // Structure disjointe
        while ((g_list_length(noeuds_tout) != g_list_length(projet->modele.noeuds)) || (g_list_length(barres_tout) != g_list_length(projet->modele.barres)))
        {
            char    *noeuds_tmp, *barres_tmp, *tmp2;
            
            noeuds_tout = g_list_concat(noeuds_tout, noeuds);
            barres_tout = g_list_concat(barres_tout, barres);
        
            if (i == 1)
                ligne->resultat = 0;
            else
                ligne->resultat = 2;
            if (*erreur < ligne->resultat)
                *erreur = ligne->resultat;
            tmp2 = ligne->commentaire;
            BUGMSG(ligne->commentaire = g_strdup_printf(gettext("%s%sBloc %d :"), tmp2 != NULL ? ligne->commentaire : "", ligne->commentaire != NULL ? "\n" : "", i), FALSE, gettext("Erreur d'allocation mémoire.\n"));
            free(tmp2);
            
            if (noeuds != NULL)
            {
                noeuds_tmp = common_selection_converti_noeuds_en_texte(noeuds);
                tmp2 = ligne->commentaire;
                BUGMSG(ligne->commentaire = g_strdup_printf("%s %s : %s%s", tmp2, g_list_next(noeuds) == NULL ? gettext("noeud") : gettext("noeuds"), noeuds_tmp, barres != NULL ? "," : ""), FALSE, gettext("Erreur d'allocation mémoire.\n"));
                free(tmp2);
                free(noeuds_tmp);
            }
            if (barres != NULL)
            {
                barres_tmp = common_selection_converti_barres_en_texte(barres);
                tmp2 = ligne->commentaire;
                BUGMSG(ligne->commentaire = g_strdup_printf("%s %s : %s", tmp2, g_list_next(barres) == NULL ? gettext("barre") : gettext("barres"), barres_tmp), FALSE, gettext("Erreur d'allocation mémoire.\n"));
                free(tmp2);
                free(barres_tmp);
            }
            
            if (g_list_length(noeuds_tout) != g_list_length(projet->modele.noeuds))
            {
                list_parcours = projet->modele.noeuds;
                
                while (list_parcours != NULL)
                {
                    if (g_list_find(noeuds_tout, list_parcours->data) == NULL)
                    {
                        EF_Noeud *n = list_parcours->data;
                        printf("Noeud %d\n", n->numero);
                        tmp = NULL;
                        tmp = g_list_append(tmp, list_parcours->data);
                        BUG(EF_verif_bloc(tmp, NULL, &noeuds, &barres, projet), FALSE);
                        g_list_free(tmp);
                        break;
                    }
                    
                    list_parcours = g_list_next(list_parcours);
                }
            }
            
            i++;
        }
        
        g_list_free(noeuds_tout);
        g_list_free(barres_tout);
    }
    *rapport = g_list_append(*rapport, ligne);
    
    // On vérifie si la structure est bien bloquée en ux, uy et uz.
    BUGMSG(ligne = malloc(sizeof(Analyse_Comm)), FALSE, gettext("Erreur d'allocation mémoire.\n"));
    BUGMSG(ligne->analyse = g_strdup_printf("%s", gettext("Structure bloquée en déplacement")), FALSE, gettext("Erreur d'allocation mémoire.\n"));
    ligne->resultat = 0;
    ligne->commentaire = NULL;
    {
        char        *tmp;
        gboolean    ux = FALSE, uy = FALSE, uz = FALSE;
        
        list_parcours = projet->modele.appuis;
        while (list_parcours != NULL)
        {
            EF_Appui    *appui = list_parcours->data;
            
            if (appui->ux != EF_APPUI_LIBRE)
                ux = TRUE;
            if (appui->uy != EF_APPUI_LIBRE)
                uy = TRUE;
            if (appui->uz != EF_APPUI_LIBRE)
                uz = TRUE;
            
            list_parcours = g_list_next(list_parcours);
        }
        
        if (ux == FALSE)
        {
            if (ligne->commentaire == NULL)
                BUGMSG(ligne->commentaire = g_strdup_printf("%s", gettext("Aucun appui ne permet de bloquer le déplacement vers x.")), FALSE, gettext("Erreur d'allocation mémoire.\n"));
            else
            {
                tmp = ligne->commentaire;
                BUGMSG(ligne->commentaire = g_strdup_printf("%s\n%s", ligne->commentaire, gettext("Aucun appui ne permet de bloquer le déplacement vers x.")), FALSE, gettext("Erreur d'allocation mémoire.\n"));
                free(tmp);
            }
        }
        if (uy == FALSE)
        {
            if (ligne->commentaire == NULL)
                BUGMSG(ligne->commentaire = g_strdup_printf("%s", gettext("Aucun appui ne permet de bloquer le déplacement vers y.")), FALSE, gettext("Erreur d'allocation mémoire.\n"));
            else
            {
                tmp = ligne->commentaire;
                BUGMSG(ligne->commentaire = g_strdup_printf("%s\n%s", ligne->commentaire, gettext("Aucun appui ne permet de bloquer le déplacement vers y.")), FALSE, gettext("Erreur d'allocation mémoire.\n"));
                free(tmp);
            }
        }
        if (uz == FALSE)
        {
            if (ligne->commentaire == NULL)
                BUGMSG(ligne->commentaire = g_strdup_printf("%s", gettext("Aucun appui ne permet de bloquer le déplacement vers z.")), FALSE, gettext("Erreur d'allocation mémoire.\n"));
            else
            {
                tmp = ligne->commentaire;
                BUGMSG(ligne->commentaire = g_strdup_printf("%s\n%s", ligne->commentaire, gettext("Aucun appui ne permet de bloquer le déplacement vers z.")), FALSE, gettext("Erreur d'allocation mémoire.\n"));
                free(tmp);
            }
        }
        
        if ((ux == TRUE) && (uy == TRUE) && (uz == TRUE))
        {
            // On vérifie si au moins un des appuis de chaque ux,y,z est bien utilisé.
            ux = FALSE;
            uy = FALSE;
            uz = FALSE;
            
            list_parcours = projet->modele.noeuds;
            while ((list_parcours != NULL) && ((ux == FALSE) || (uy == FALSE) || (uz == FALSE)))
            {
                EF_Noeud    *noeud = list_parcours->data;
                
                if (noeud->appui != NULL)
                {
                    if (noeud->appui->ux != EF_APPUI_LIBRE)
                        ux = TRUE;
                    if (noeud->appui->uy != EF_APPUI_LIBRE)
                        uy = TRUE;
                    if (noeud->appui->uz != EF_APPUI_LIBRE)
                        uz = TRUE;
                }
                
                list_parcours = g_list_next(list_parcours);
            }
            
            if (ux == FALSE)
            {
                if (ligne->commentaire == NULL)
                    BUGMSG(ligne->commentaire = g_strdup_printf("%s", gettext("Aucun noeud ne permet de bloquer le déplacement vers x.")), FALSE, gettext("Erreur d'allocation mémoire.\n"));
                else
                {
                    tmp = ligne->commentaire;
                    BUGMSG(ligne->commentaire = g_strdup_printf("%s\n%s", ligne->commentaire, gettext("Aucun noeud ne permet de bloquer le déplacement vers x.")), FALSE, gettext("Erreur d'allocation mémoire.\n"));
                    free(tmp);
                }
            }
            if (uy == FALSE)
            {
                if (ligne->commentaire == NULL)
                    BUGMSG(ligne->commentaire = g_strdup_printf("%s", gettext("Aucun noeud ne permet de bloquer le déplacement vers y.")), FALSE, gettext("Erreur d'allocation mémoire.\n"));
                else
                {
                    tmp = ligne->commentaire;
                    BUGMSG(ligne->commentaire = g_strdup_printf("%s\n%s", ligne->commentaire, gettext("Aucun noeud ne permet de bloquer le déplacement vers y.")), FALSE, gettext("Erreur d'allocation mémoire.\n"));
                    free(tmp);
                }
            }
            if (uz == FALSE)
            {
                if (ligne->commentaire == NULL)
                    BUGMSG(ligne->commentaire = g_strdup_printf("%s", gettext("Aucun noeud ne permet de bloquer le déplacement vers z.")), FALSE, gettext("Erreur d'allocation mémoire.\n"));
                else
                {
                    tmp = ligne->commentaire;
                    BUGMSG(ligne->commentaire = g_strdup_printf("%s\n%s", ligne->commentaire, gettext("Aucun noeud ne permet de bloquer le déplacement vers z.")), FALSE, gettext("Erreur d'allocation mémoire.\n"));
                    free(tmp);
                }
            }
            
            if ((ux == FALSE) || (uy == FALSE) || (uz == FALSE))
            {
                ligne->resultat = 2;
                if (*erreur < ligne->resultat)
                    *erreur = ligne->resultat;
            }
        }
        else
        {
            ligne->resultat = 2;
            if (*erreur < ligne->resultat)
                *erreur = ligne->resultat;
        }
    }
    *rapport = g_list_append(*rapport, ligne);
    
    return TRUE;
}


gboolean EF_verif_rapport_affiche(GList *rapport)
/* Description : Affiche un rapport d'une vérification.
 * Paramètres : GList **rapport : liste d'Analyse_Comm.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE
 */
{
    GList   *list_parcours;
    
    if (rapport == NULL)
        printf(gettext("Rapport vide.\n"));
    
    list_parcours = rapport;
    while (list_parcours != NULL)
    {
        Analyse_Comm *ligne = list_parcours->data;
        
        printf(gettext("Point : %s\nCommentaire : %s\nErreur : %s\n\n"), ligne->analyse, ligne->commentaire == NULL ? gettext("Aucun") : ligne->commentaire, ligne->resultat == 0 ? gettext("Aucune") : ligne->resultat == 1 ? gettext("Avertissement") : gettext("Critique"));
        
        list_parcours = g_list_next(list_parcours);
    }
    
    return TRUE;
}


void EF_verif_rapport_free(GList *rapport)
/* Description : Libère la mémoire utilisée pour un rapport.
 * Paramètres : GList **rapport : le rapport à libérer.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE
 */
{
    GList   *list_parcours = rapport;
    
    while (list_parcours != NULL)
    {
        Analyse_Comm *ligne = list_parcours->data;
        
        free(ligne->analyse);
        free(ligne->commentaire);
        free(ligne);
        
        list_parcours = g_list_next(list_parcours);
    }
    
    g_list_free(rapport);
}
