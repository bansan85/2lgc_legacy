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

#include "1990_actions.h"
#include "common_projet.h"
#include "common_erreurs.h"
#include "common_fonction.h"
#include "EF_noeud.h"
#include "EF_charge_noeud.h"
#include "EF_charge_barre_ponctuelle.h"


int EF_charge_barre_ponctuelle_ajout(Projet *projet, int num_action, void *barre, 
  int repere_local, double position, double fx, double fy, double fz, double rx, double ry,
  double rz)
/* Description : ajoute une charge ponctuelle à une action et à l'intérieur d'une barre en lui
 *                 attribuant le numéro suivant la dernière charge de l'action.
 * Paramètres : Projet *projet : la variable projet
 *            : int num_action : numero de l'action qui contiendra la charge
 *            : void *barre : barre qui supportera la charge
 *            : int repere_local : TRUE si les charges doivent être prise dans le repère local,
 *                                 FALSE pour le repère global
 *            : double position : position en mètre de la charge par rapport au début de la
 *                                  barre
 *            : double fx : force suivant l'axe x
 *            : double fy : force suivant l'axe y
 *            : double fz : force suivant l'axe z
 *            : double rx : moment autour de l'axe x
 *            : double ry : moment autour de l'axe y
 *            : double rz : moment autour de l'axe z
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet == NULL) ou
 *             (projet->actions == NULL) ou
 *             (list_size(projet->actions) == 0) ou
 *             (barre == NULL) ou
 *             (_1990_action_cherche_numero(projet, num_action) != 0)
 *           -2 en cas d'erreur d'allocation mémoire
 */
{
    Action          *action_en_cours;
    Charge_Barre_Ponctuelle *charge_dernier, charge_nouveau;
    
    // Trivial
    BUGMSG(projet, -1, "_1990_action_ajout_charge_ponctuelle_barre\n");
    BUGMSG(projet->actions, -1, "_1990_action_ajout_charge_ponctuelle_barre\n");
    BUGMSG(list_size(projet->actions), -1, "_1990_action_ajout_charge_ponctuelle_barre\n");
    BUGMSG(barre, -1, "_1990_action_ajout_charge_ponctuelle_barre\n");
    
    BUG(_1990_action_cherche_numero(projet, num_action) == 0, -1);
    action_en_cours = list_curr(projet->actions);
    
    charge_nouveau.type = CHARGE_PONCTUELLE_BARRE;
    charge_nouveau.nom = NULL;
    charge_nouveau.description = NULL;
    charge_nouveau.barre = barre;
    charge_nouveau.repere_local = repere_local;
    charge_nouveau.position = position;
    charge_nouveau.x = fx;
    charge_nouveau.y = fy;
    charge_nouveau.z = fz;
    charge_nouveau.rx = rx;
    charge_nouveau.ry = ry;
    charge_nouveau.rz = rz;
    
    charge_dernier = (Charge_Barre_Ponctuelle *)list_rear(action_en_cours->charges);
    if (charge_dernier == NULL)
        charge_nouveau.numero = 0;
    else
        charge_nouveau.numero = charge_dernier->numero+1;
    
    list_mvrear(action_en_cours->charges);
    BUGMSG(list_insert_after(action_en_cours->charges, &(charge_nouveau), sizeof(charge_nouveau)), -2, gettext("%s : Erreur d'allocation mémoire.\n"), "_1990_action_ajout_charge_ponctuelle_barre");
    
    return 0;
}
