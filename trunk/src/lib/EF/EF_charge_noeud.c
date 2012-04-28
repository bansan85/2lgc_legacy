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


G_MODULE_EXPORT Charge_Noeud*  EF_charge_noeud_ajout(Projet *projet, unsigned int num_action, GList *noeuds,
  double fx, double fy, double fz, double mx, double my, double mz, const char* nom)
/* Description : Ajoute une charge ponctuelle à une action et à un noeud de la structure en
 *               lui attribuant le numéro suivant la dernière charge de l'action.
 * Paramètres : Projet *projet : la variable projet
 *            : int num_action : numero de l'action qui contiendra la charge
 *            : GList *noeuds : liste des noeuds qui supportera la charge
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
    Charge_Noeud    *charge_nouveau = malloc(sizeof(Charge_Noeud));
    
    // Trivial
    BUGMSG(projet, NULL, gettext("Paramètre incorrect\n"));
    BUGMSG(projet->actions, NULL, gettext("Paramètre incorrect\n"));
    BUG(action_en_cours = _1990_action_cherche_numero(projet, num_action), NULL);
    BUGMSG(charge_nouveau, NULL, gettext("Erreur d'allocation mémoire.\n"));
    
    charge_nouveau->type = CHARGE_NOEUD;
    BUGMSG(charge_nouveau->description = g_strdup_printf("%s", nom), NULL, gettext("Erreur d'allocation mémoire.\n"));
    charge_nouveau->noeuds = noeuds;
    charge_nouveau->fx = fx;
    charge_nouveau->fy = fy;
    charge_nouveau->fz = fz;
    charge_nouveau->mx = mx;
    charge_nouveau->my = my;
    charge_nouveau->mz = mz;
    
    charge_nouveau->numero = g_list_length(action_en_cours->charges);
    
    action_en_cours->charges = g_list_append(action_en_cours->charges, charge_nouveau);
    
    return charge_nouveau;
}


G_MODULE_EXPORT int EF_charge_noeud_free(Charge_Noeud *charge)
{
/* Description : Libère une charge nodale.
 * Paramètres : Charge_Noeud *charge : la charge à libérer.
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paraètres invalides :
 *             (charge == NULL) ou
 *             (charge->description == NULL) ou
 *             (charge->noeuds == NULL)
 */
    BUGMSG(charge, -1, gettext("Paramètre incorrect\n"));
    BUGMSG(charge->description, -1, gettext("Paramètre incorrect\n"));
    BUGMSG(charge->noeuds, -1, gettext("Paramètre incorrect\n"));
    
    free(charge->description);
    g_list_free(charge->noeuds);
    free(charge);
    
    return 0;
}
