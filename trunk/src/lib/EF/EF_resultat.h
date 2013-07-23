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

#ifndef __EF_RESULTAT_H
#define __EF_RESULTAT_H

#include "config.h"
#include "common_projet.h"
#include "1990_action.h"

gboolean EF_resultat_noeud_reaction_appui(GList *liste, EF_Noeud *noeud, int indice, Projet *projet, char **texte, double* mini, double *maxi) __attribute__((__warn_unused_result__));
gboolean EF_resultat_noeud_deplacement(GList *liste, EF_Noeud *noeud, int indice, Projet *projet, char **texte, double* mini, double *maxi) __attribute__((__warn_unused_result__));

#endif
