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

#ifndef __EF_GTK_CHARGE_NOEUD_H
#define __EF_GTK_CHARGE_NOEUD_H

#include "config.h"
#ifdef ENABLE_GTK
#include <gtk/gtk.h>

#include "common_projet.h"

void EF_gtk_charge_noeud(Projet *projet, gint action_defaut, gint charge);
void EF_gtk_charge_noeud_ajout_affichage(Charge_Noeud *charge, Projet *projet, gboolean nouvelle_ligne);

#endif

#endif
