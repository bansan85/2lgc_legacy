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

#ifndef __1990_ACTIONS_PRIVATE_H
#define __1990_ACTIONS_PRIVATE_H

#include "common_projet.hpp"

/**
 * \brief Objet définissant une action avec sa liste de charges.
 */
struct __Action
{
  /// Le nom plus si nécessaire un Iter_groupe pour l'affichage graphique.
  ENTETE_ACTION_ET_GROUPE;
  /// La description de type est donnée par #_1990_action_bat_txt_type.
  uint8_t         type;
  /// Liste des charges (ponctuelle, répartie, …).
  GList          *charges;
  /// Flag utilisé temporairement.
  uint8_t         action_predominante : 1;
  /// Valeur de combinaison d'une charge variable.
  Flottant        psi0;
  /// Valeur fréquente d'une action variable.
  Flottant        psi1;
  /// Valeur quasi-permanente d'une action variable.
  Flottant        psi2;
  /// Déplacement des nœuds de la structure sous l'effet de l'action.
  cholmod_sparse *deplacement;
  /// Efforts équivalents des charges dans les nœuds de la structure.
  cholmod_sparse *forces;
  /// Efforts (y compris les réactions d'appui) dans les nœuds.
  cholmod_sparse *efforts_noeuds;
#ifdef ENABLE_GTK
  /// Iter utilisé par le treeview dans la fenêtre Action. 
  GtkTreeIter     Iter_fenetre;
  /// Iter utilisé par les menus des fenêtres Charges.
  GtkTreeIter     Iter_liste;
#endif
  /// Les 6 fonctions (N, Ty, Tz, Mx, My, Mz) par barre.
  Fonction      **efforts[6];
  /// Les 3 déformations x, y, z pour chaque barre dans le repère local.
  Fonction      **deformation[3]; 
  /// Les 3 rotations rx, ry, rz pour chaque barre dans le repère local.
  Fonction      **rotation[3];
};


gboolean _1990_action_init(Projet *p) __attribute__((__warn_unused_result__));

#endif

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
