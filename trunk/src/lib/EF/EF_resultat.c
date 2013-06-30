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

#include "common_projet.h"
#include "common_math.h"
#include "common_erreurs.h"
#include "common_fonction.h"


Action *EF_resultat_action_ponderation(GList* ponderation, Projet *projet)
/* Description : Crée une fausse action sur la base d'une combinaison. L'objectif de cette
 *               fonction est uniquement de regrouper les résultats pondérés de chaque action.
 * Paramètres : GList *ponderation : pondération sur laquelle sera créée l'action,
 *              Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : pointeur vers l'action équivalente créée.
 *   Échec : NULL :
 *             projet == NULL,
 *             ponderation == NULL,
 *             une des actions de pondération est introuvable.
 */
{
    GList   *list_parcours;
    Action  *action;
    double  *x, *y;

    
    BUGMSG(projet, NULL, gettext("Paramètre %s incorrect.\n"), "projet");
    
    // Initialisation de l'action
    BUGMSG(action = malloc(sizeof(Action)), NULL, gettext("Erreur d'allocation mémoire.\n"));
    BUGMSG(action->efforts_noeuds = malloc(sizeof(cholmod_sparse)), NULL, gettext("Erreur d'allocation mémoire.\n"));
    BUGMSG(action->efforts_noeuds->x = malloc(sizeof(double)*g_list_length(projet->modele.noeuds)*6), NULL, gettext("Erreur d'allocation mémoire.\n"));
    memset(action->efforts_noeuds->x, 0, sizeof(double)*g_list_length(projet->modele.noeuds)*6);
    BUGMSG(action->deplacement_complet = malloc(sizeof(cholmod_sparse)), NULL, gettext("Erreur d'allocation mémoire.\n"));
    BUGMSG(action->deplacement_complet->x = malloc(sizeof(double)*g_list_length(projet->modele.noeuds)*6), NULL, gettext("Erreur d'allocation mémoire.\n"));
    memset(action->deplacement_complet->x, 0, sizeof(double)*g_list_length(projet->modele.noeuds)*6);
    BUG(common_fonction_init(projet, action), NULL);
    x = action->efforts_noeuds->x;
    y = action->deplacement_complet->x;
    
    // Remplissage de la variable action.
    list_parcours = ponderation;
    while (list_parcours != NULL)
    {
        Ponderation     *element = list_parcours->data;
        double          *x2 = element->action->efforts_noeuds->x;
        double          *y2 = element->action->deplacement_complet->x;
        double          mult = element->ponderation*(element->psi == 0 ? common_math_get(element->action->psi0) : element->psi == 1 ? common_math_get(element->action->psi1) : element->psi == 2 ? common_math_get(element->action->psi2) : 1.);
        unsigned int    i;
        
        for (i=0;i<g_list_length(projet->modele.noeuds)*6;i++)
        {
            x[i] = x[i] + mult*x2[i];
            y[i] = y[i] + mult*y2[i];
        }
        
        for (i=0;i<g_list_length(projet->modele.barres);i++)
        {
            BUG(common_fonction_ajout_fonction(action->fonctions_efforts[0][i], element->action->fonctions_efforts[0][i], mult), NULL);
            BUG(common_fonction_ajout_fonction(action->fonctions_efforts[1][i], element->action->fonctions_efforts[1][i], mult), NULL);
            BUG(common_fonction_ajout_fonction(action->fonctions_efforts[2][i], element->action->fonctions_efforts[2][i], mult), NULL);
            BUG(common_fonction_ajout_fonction(action->fonctions_efforts[3][i], element->action->fonctions_efforts[3][i], mult), NULL);
            BUG(common_fonction_ajout_fonction(action->fonctions_efforts[4][i], element->action->fonctions_efforts[4][i], mult), NULL);
            BUG(common_fonction_ajout_fonction(action->fonctions_efforts[5][i], element->action->fonctions_efforts[5][i], mult), NULL);
            BUG(common_fonction_ajout_fonction(action->fonctions_deformation[0][i], element->action->fonctions_deformation[0][i], mult), NULL);
            BUG(common_fonction_ajout_fonction(action->fonctions_deformation[1][i], element->action->fonctions_deformation[1][i], mult), NULL);
            BUG(common_fonction_ajout_fonction(action->fonctions_deformation[2][i], element->action->fonctions_deformation[2][i], mult), NULL);
            BUG(common_fonction_ajout_fonction(action->fonctions_rotation[0][i], element->action->fonctions_rotation[0][i], mult), NULL);
            BUG(common_fonction_ajout_fonction(action->fonctions_rotation[1][i], element->action->fonctions_rotation[1][i], mult), NULL);
            BUG(common_fonction_ajout_fonction(action->fonctions_rotation[2][i], element->action->fonctions_rotation[2][i], mult), NULL);
        }
        
        list_parcours = g_list_next(list_parcours);
    }
    
    return action;
}


gboolean EF_resultat_noeud_reaction_appui(GList *liste, EF_Noeud *noeud, int indice,
  Projet *projet, char **texte, double* mini, double *maxi)
/* Description : Renvoie la réaction d'appui d'un noeud en fonction d'une liste de pondérations.
 * Paramètres : GList *liste : liste d'actions préalablement créées par la fonction 
 *                EF_resultat_action_ponderation,
 *              EF_Noeud *noeud : noeud à étudier,
 *              int indice : 0 si Fx, 1 si Fy, 2 si Fz, 3 si Mx, 4 si My et 5 si Mz,
 *              Projet *projet : la variable projet,
 *              char **texte : la réaction d'appui sous forme d'un texte (peut être NULL),
 *              double *mini : la valeur minimale (dans le cas d'une liste à plusieurs actions),
 *                               peut être NULL,
 *              double *maxi : la valeur maximale (dans le cas d'une liste à plusieurs actions),
 *                               peut être NULL.
 * Valeur renvoyée :
 *   Succès : TRUE.
 *   Échec : FALSE :
 *             liste == NULL,
 *             noeud == NULL,
 *             projet == NULL.
 */
{
    GList   *list_parcours;
    int     i;
    double  mi, ma;
    double  *x;
    Action  *action;
    
    BUGMSG(noeud, FALSE, gettext("Paramètre %s incorrect.\n"), "noeud");
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG((0 <= indice) && (indice <= 5), FALSE, gettext("Paramètre %s incorrect.\n"), gettext("Indice hors limite.\n"));
    
    if (liste == NULL)
    {
        if (texte != NULL)
            BUGMSG(*texte = g_strdup_printf("%.*lf", indice < 3 ? DECIMAL_FORCE : DECIMAL_MOMENT, 0.), FALSE, gettext("Erreur d'allocation mémoire.\n"));
        if (mini != NULL)
            *mini = 0.;
        if (maxi != NULL)
            *maxi = 0.;
        return TRUE;
    }
    
    i = g_list_index(projet->modele.noeuds, noeud);
    
    list_parcours = liste;
    action = list_parcours->data;
    x = action->efforts_noeuds->x;
    mi = x[i*6+indice];
    ma = x[i*6+indice];
    list_parcours = g_list_next(list_parcours);
    while (list_parcours != NULL)
    {
        action = list_parcours->data;
        
        x = action->efforts_noeuds->x;
        
        if (x[i*6+indice] < mi)
            mi = x[i*6+indice];
        if (x[i*6+indice] > ma)
            ma = x[i*6+indice];
        
        list_parcours = g_list_next(list_parcours);
    }
    
    if (mini != NULL)
        *mini = mi;
    if (maxi != NULL)
        *maxi = ma;
    if (texte != NULL)
    {
        if (!ERREUR_RELATIVE_EGALE(mi, ma))
            BUGMSG(*texte = g_strdup_printf("%.*lf/%.*lf", indice < 3 ? DECIMAL_FORCE : DECIMAL_MOMENT, mi, indice < 3 ? DECIMAL_FORCE : DECIMAL_MOMENT, ma), FALSE, gettext("Erreur d'allocation mémoire.\n"));
        else
            BUGMSG(*texte = g_strdup_printf("%.*lf", indice < 3 ? DECIMAL_FORCE : DECIMAL_MOMENT, mi), FALSE, gettext("Erreur d'allocation mémoire.\n"));
    }
    
    return TRUE;
}


gboolean EF_resultat_noeud_deplacement(GList *liste, EF_Noeud *noeud, int indice,
  Projet *projet, char **texte, double* mini, double *maxi)
/* Description : Renvoie le déplacement d'un noeud en fonction d'une liste de pondérations.
 * Paramètres : GList *liste : liste d'actions préalablement créées par la fonction 
 *                EF_resultat_action_ponderation,
 *              EF_Noeud *noeud : noeud à étudier,
 *              int indice : 0 si Ux, 1 si Uy, 2 si Uz, 3 si rx, 4 si ry et 5 si rz,
 *              Projet *projet : la variable projet,
 *              char **texte : le déplacement sous forme d'un texte (peut être NULL),
 *              double *mini : la valeur minimale (dans le cas d'une liste à plusieurs actions),
 *                               peut être NULL,
 *              double *maxi : la valeur maximale (dans le cas d'une liste à plusieurs actions),
 *                               peut être NULL.
 * Valeur renvoyée :
 *   Succès : TRUE.
 *   Échec : FALSE :
 *             liste == NULL,
 *             noeud == NULL,
 *             projet == NULL.
 */
{
    GList   *list_parcours;
    int     i;
    double  mi, ma;
    double  *x;
    Action  *action;
    
    BUGMSG(noeud, FALSE, gettext("Paramètre %s incorrect.\n"), "noeud");
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG((0 <= indice) && (indice <= 5), FALSE, gettext("Paramètre %s incorrect.\n"), gettext("Indice hors limite.\n"));
    
    if (liste == NULL)
    {
        if (texte != NULL)
            BUGMSG(*texte = g_strdup_printf("%.*lf", indice < 3 ? DECIMAL_DEPLACEMENT : DECIMAL_ROTATION, 0.), FALSE, gettext("Erreur d'allocation mémoire.\n"));
        if (mini != NULL)
            *mini = 0.;
        if (maxi != NULL)
            *maxi = 0.;
        return TRUE;
    }
    
    i = g_list_index(projet->modele.noeuds, noeud);
    
    list_parcours = liste;
    action = list_parcours->data;
    x = action->deplacement_complet->x;
    mi = x[i*6+indice];
    ma = x[i*6+indice];
    list_parcours = g_list_next(list_parcours);
    while (list_parcours != NULL)
    {
        action = list_parcours->data;
        
        x = action->deplacement_complet->x;
        
        if (x[i*6+indice] < mi)
            mi = x[i*6+indice];
        if (x[i*6+indice] > ma)
            ma = x[i*6+indice];
        
        list_parcours = g_list_next(list_parcours);
    }
    
    if (mini != NULL)
        *mini = mi;
    if (maxi != NULL)
        *maxi = ma;
    if (texte != NULL)
    {
        if (!ERREUR_RELATIVE_EGALE(mi, ma))
            BUGMSG(*texte = g_strdup_printf("%.*lf/%.*lf", indice < 3 ? DECIMAL_DEPLACEMENT : DECIMAL_ROTATION, mi, indice < 3 ? DECIMAL_DEPLACEMENT : DECIMAL_ROTATION, ma), FALSE, gettext("Erreur d'allocation mémoire.\n"));
        else
            BUGMSG(*texte = g_strdup_printf("%.*lf", indice < 3 ? DECIMAL_DEPLACEMENT : DECIMAL_ROTATION, mi), FALSE, gettext("Erreur d'allocation mémoire.\n"));
    }
    
    return TRUE;
}
