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

#ifndef __1990_GTK_GROUPES_H
#define __1990_GTK_GROUPES_H

#include "config.h"
#ifdef ENABLE_GTK
#include <gtk/gtk.h>

const GtkTargetEntry drag_targets_1 = { (gchar*)PACKAGE"1_SAME_PROC", GTK_TARGET_SAME_APP, 0}; 
const GtkTargetEntry drag_targets_2 = { (gchar*)PACKAGE"2_SAME_PROC", GTK_TARGET_SAME_APP, 0}; 
const GtkTargetEntry drag_targets_3[] = { {(gchar*)PACKAGE"1_SAME_PROC", GTK_TARGET_SAME_APP, 0},
                                          {(gchar*)PACKAGE"2_SAME_PROC", GTK_TARGET_SAME_APP, 0}}; 

typedef struct __List_Gtk_1990
{
    GtkWidget       *window_groupe;
    GtkWidget       *table_groupe;
    GtkWidget       *table_combinaison;
    GtkWidget       *spin_button_niveau;
    GtkWidget       *button_niveau_ajout;
    GtkWidget       *button_niveau_suppr;
    GtkWidget       *frame_groupe;
    GtkWidget       *table_groupes;
    GtkTreeStore    *tree_store_etat;
    GtkTreeView     *tree_view_etat;
    GtkWidget       *scroll_etat;
    GtkWidget       *button_groupe_ajout;
    GtkWidget       *button_groupe_suppr;
    GtkWidget       *button_groupe_and;
    GtkWidget       *button_groupe_or;
    GtkWidget       *button_groupe_xor;
    GtkWidget       *button_groupe_nom;
    GtkWidget       *entry_groupe_nom;
    GtkWidget       *frame_dispo;
    GtkWidget       *table_dispo;
    GtkTreeStore    *tree_store_dispo;
    GtkTreeView     *tree_view_dispo;
    GtkWidget       *scroll_dispo;
    GtkWidget       *button_ajout_dispo;
    GtkWidget       *table_bas;
    GtkWidget       *button_generer;
    GtkWidget       *button_options;
    GtkWidget       *button_quitter;
    GtkWidget       *drag_from;
} List_Gtk_1990;

void _1990_gtk_groupes(GtkWidget *button __attribute__((unused)), Projet *projet);

#endif

#endif
