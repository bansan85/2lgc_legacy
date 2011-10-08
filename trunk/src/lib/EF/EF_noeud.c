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
#include "EF_noeud.h"
#include "EF_appuis.h"

int EF_noeuds_init(Projet *projet)
/* Description : Initialise la liste des noeuds
 * Paramètres : Projet *projet : la variable projet
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet == NULL)
 *           -2 en cas d'erreur d'allocation mémoire
 */
{
    BUGMSG(projet, -1, "EF_noeuds_init\n");
    // Trivial
    projet->ef_donnees.noeuds = list_init();
    BUGMSG(projet->ef_donnees.noeuds, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_resoud_charge");
    return 0;
}


int EF_noeuds_ajout(Projet *projet, double x, double y, double z, int appui)
/* Description : Ajouter un noeud à la liste des noeuds en lui attribuant le numéro suivant le
 *                 dernier noeud existant
 * Paramètres : Projet *projet : la variable projet
 *            : double x : position en x
 *            : double y : position en y
 *            : double z : position en z
 *            : int appui : numéro de l'appui. -1 si aucun.
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet == NULL) ou
 *             (projet->ef_donnees.noeuds == NULL) ou
 *             (EF_appuis_cherche_numero(projet, appui) == NULL)
 *           -2 en cas d'erreur d'allocation mémoire
 */
{
    EF_Noeud        *noeud_en_cours, noeud_nouveau;
    
    BUGMSG(projet, -1, "EF_noeuds_ajout\n");
    BUGMSG(projet->ef_donnees.noeuds, -1, "EF_noeuds_ajout\n");
    
    // Trivial
    list_mvrear(projet->ef_donnees.noeuds);
    noeud_nouveau.position.x = x;
    noeud_nouveau.position.y = y;
    noeud_nouveau.position.z = z;
    
    if (appui == -1)
        noeud_nouveau.appui = NULL;
    else
    {
        noeud_nouveau.appui = EF_appuis_cherche_numero(projet, appui);
        BUGMSG(noeud_nouveau.appui, -1, "%s : %s %d\n", "EF_noeuds_ajout", "appui", appui);
    }
        
    noeud_en_cours = (EF_Noeud *)list_rear(projet->ef_donnees.noeuds);
    if (noeud_en_cours == NULL)
        noeud_nouveau.numero = 0;
    else
        noeud_nouveau.numero = noeud_en_cours->numero+1;
    
    BUGMSG(list_insert_after(projet->ef_donnees.noeuds, &(noeud_nouveau), sizeof(noeud_nouveau)), -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_noeuds_ajout");
    
    return 0;
}


EF_Noeud* EF_noeuds_cherche_numero(Projet *projet, int numero)
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
    BUGMSG(projet, NULL, "EF_noeuds_cherche_numero\n");
    BUGMSG(projet->ef_donnees.noeuds, NULL, "EF_noeuds_cherche_numero\n");
    BUGMSG(list_size(projet->ef_donnees.noeuds), NULL, "EF_noeuds_cherche_numero\n");
    // Trivial
    list_mvfront(projet->ef_donnees.noeuds);
    do
    {
        EF_Noeud    *noeud = (EF_Noeud*)list_curr(projet->ef_donnees.noeuds);
        
        if (noeud->numero == numero)
            return noeud;
    }
    while (list_mvnext(projet->ef_donnees.noeuds) != NULL);
    
    BUGMSG(0, NULL, gettext("Noeud n°%d introuvable.\n"), numero);
}


double EF_noeuds_distance(EF_Noeud* n1, EF_Noeud* n2)
/* Description : Renvoie la distance entre deux noeuds.
 * Paramètres : EF_Noeud* n1 : noeud de départ
 *            : EF_Noeud* n2 : noeud de fin
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet == NULL) ou
 *             (projet->ef_donnees.noeuds == NULL)
 */
{
    double x, y, z;
    
    // \end{verbatim}\texttt{distance }$= \sqrt{x^2+y^2+z^2}$\begin{verbatim}
    BUGMSG(n1, NAN, "EF_noeuds_distance\n");
    BUGMSG(n2, NAN, "EF_noeuds_distance\n");
    
    x = n2->position.x - n1->position.x;
    y = n2->position.y - n1->position.y;
    z = n2->position.z - n1->position.z;
    return sqrt(x*x+y*y+z*z);
}


int EF_noeuds_free(Projet *projet)
/* Description : Libère l'ensemble des noeuds et la liste les contenant.
 * Paramètres : Projet *projet : la variable projet
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet == NULL) ou
 *             (projet->ef_donnees.noeuds == NULL)
 */
{
    BUGMSG(projet, -1, "EF_noeuds_free\n");
    BUGMSG(projet->ef_donnees.noeuds, -1, "EF_noeuds_free\n");
    
    // Trivial
    while (!list_empty(projet->ef_donnees.noeuds))
    {
        EF_Noeud    *noeud = (EF_Noeud*)list_remove_front(projet->ef_donnees.noeuds);
        
        free(noeud);
    }
    
    free(projet->ef_donnees.noeuds);
    projet->ef_donnees.noeuds = NULL;
    
    return 0;
}
