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

#ifndef __EF_GTK_RESULTATS_H
#define __EF_GTK_RESULTATS_H

#include "config.h"
#ifdef ENABLE_GTK
#include <gtk/gtk.h>

#include "common_projet.hpp"


/**
 * \struct Gtk_EF_Resultats_Tableau
 * \brief La structure de données du tableau d'affichage des résultats.
 *        Un par onglet.
 */
typedef struct
{
  /// Le treeview de la fenêtre résultat.
  GtkTreeView       *treeview;
  /// Le model du treeview.
  GtkListStore      *list_store;
  
  /// La première valeur du tableau est le nombre de colonnes à afficher.
  Colonne_Resultats *col_tab;
  /// Le filtre des résultats.
  Filtres            filtre;
  /// Titre de l'onglet.
  char              *nom;
} Gtk_EF_Resultats_Tableau;


void EF_gtk_resultats      (Projet *p);
void EF_gtk_resultats_free (Projet *p);

#endif

#endif

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
