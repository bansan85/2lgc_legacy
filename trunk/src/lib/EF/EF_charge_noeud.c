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
#include <stdio.h>
#include <stdlib.h>
#include <cholmod.h>
#include <string.h>

#include "1990_actions.h"
#include "common_projet.h"
#include "common_erreurs.h"


Charge_Noeud*  EF_charge_noeud_ajout(Projet *projet, int num_action, LIST *noeuds,
  double fx, double fy, double fz, double mx, double my, double mz, const char* nom)
/* Description : Ajoute une charge ponctuelle à une action et à un noeud de la structure en
 *               lui attribuant le numéro suivant la dernière charge de l'action.
 * Paramètres : Projet *projet : la variable projet
 *            : int num_action : numero de l'action qui contiendra la charge
 *            : EF_Noeud *action : pointeur vers le noeud qui supportera la charge
 *            : double fx : force suivant l'axe global x
 *            : double fy : force suivant l'axe global y
 *            : double fz : force suivant l'axe global z
 *            : double mx : moment autour de l'axe global x
 *            : double my : moment autour de l'axe global y
 *            : double mz : moment autour de l'axe global z
 * Valeur renvoyée :
 *   Succès : un pointeur vers la nouvelle charge
 *   Échec : NULL en cas de paramètres invalides :
 *             (projet == NULL) ou
 *             (projet->actions == NULL) ou
 *             (list_size(projet->actions) == 0) ou
 *             (noeud == NULL)
 *           NULL en cas d'erreur d'allocation mémoire
 */
{
    Action          *action_en_cours;
    Charge_Noeud    *charge_dernier, charge_nouveau;
    
    // Trivial
    BUGMSG(projet, NULL, "EF_charge_noeud_ajout\n");
    BUGMSG(projet->actions, NULL, "EF_charge_noeud_ajout\n");
    BUGMSG(list_size(projet->actions), NULL, "EF_charge_noeud_ajout\n");
    BUGMSG(noeuds, NULL, "EF_charge_noeud_ajout\n");
    BUG(_1990_action_cherche_numero(projet, num_action) == 0, NULL);
    
    action_en_cours = (Action*)list_curr(projet->actions);
    
    charge_nouveau.type = CHARGE_NOEUD;
    charge_nouveau.description = (char*)malloc(sizeof(char)*(strlen(nom)+1));
    BUGMSG(charge_nouveau.description, NULL, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_charge_noeud_ajout");
    strcpy(charge_nouveau.description, nom);
    charge_nouveau.noeuds = noeuds;
    charge_nouveau.fx = fx;
    charge_nouveau.fy = fy;
    charge_nouveau.fz = fz;
    charge_nouveau.mx = mx;
    charge_nouveau.my = my;
    charge_nouveau.mz = mz;
    
    charge_dernier = (Charge_Noeud *)list_rear(action_en_cours->charges);
    if (charge_dernier == NULL)
        charge_nouveau.numero = 0;
    else
        charge_nouveau.numero = charge_dernier->numero+1;
    
    list_mvrear(action_en_cours->charges);
    BUGMSG(list_insert_after(action_en_cours->charges, &(charge_nouveau), sizeof(charge_nouveau)), NULL, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_charge_noeud_ajout");
    
    return (Charge_Noeud*)list_curr(action_en_cours->charges);
}


void EF_charge_noeud_free(Charge_Noeud *charge)
{
/* Description : Libère le contenu alloué dans une charge nodale.
 * Paramètres : Charge_Noeud *charge : la charge à libérer.
 * Valeur renvoyée : void
 */
    free(charge->description);
    list_free(charge->noeuds, LIST_DEALLOC);
    return;
}
