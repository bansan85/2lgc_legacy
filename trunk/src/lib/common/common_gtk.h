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

#ifndef __COMMON_GTK_H
#define __COMMON_GTK_H

#include "config.h"
#include "common_projet.h"
#include <math.h>
#include <gtk/gtk.h>


#define UI_ACT    p->ui._1990_actions
#define UI_GRO    p->ui._1990_groupes
#define UI_GROOP  p->ui._1990_groupes_options
#define UI_BET    p->ui._1992_1_1_materiaux
#define UI_ACI    p->ui._1993_1_1_materiaux
#define UI_INFO   p->ui.common_informations
#define UI_M3D    p->ui.m3d
#define UI_GTK    p->ui.comp
#define UI_APP    p->ui.ef_appuis
#define UI_MAT    p->ui.ef_materiaux
#define UI_NOE    p->ui.ef_noeud
#define UI_REL    p->ui.ef_relachements
#define UI_BAR    p->ui.ef_barres
#define UI_BARADD p->ui.ef_barres_add
#define UI_CHBARP p->ui.ef_charge_barre_ponctuelle
#define UI_CHBARR p->ui.ef_charge_barre_repartie_uniforme
#define UI_CHNO   p->ui.ef_charge_noeud
#define UI_MATX   p->ui.ef_materiaux
#define UI_SEC    p->ui.ef_sections
#define UI_SEC_RE p->ui.ef_section_rectangulaire
#define UI_SEC_T  p->ui.ef_section_T
#define UI_SEC_CA p->ui.ef_section_carree
#define UI_SEC_CI p->ui.ef_section_circulaire
#define UI_SEC_PE p->ui.ef_section_personnalisee
#define UI_RAP    p->ui.ef_rapport
#define UI_RES    p->ui.ef_resultats


#define GTK_COMMON_SPINBUTTON_AS_UINT(spinbutton) \
  ((unsigned int) round (gtk_spin_button_get_value (spinbutton)))

#define GTK_COMMON_SPINBUTTON_AS_INT(spinbutton) \
  ((int) round (gtk_spin_button_get_value (spinbutton)))

#define GTK_WINDOW_KEY_PRESS(pref, nom) \
gboolean \
pref##_gtk_##nom##_window_key_press (GtkWidget *widget, \
                                     GdkEvent  *event, \
                                     Projet    *p) \
{ \
  BUGMSG (p, FALSE, gettext ("Paramètre %s incorrect."), "Projet") \
  BUGMSG (p->ui.pref##_##nom.builder, \
          FALSE, \
          gettext ("La fenêtre graphique %s n\'est pas initialisée."), #nom) \
  \
  if (event->key.keyval == GDK_KEY_Escape) \
  { \
    gtk_widget_destroy (p->ui.pref##_##nom.window); \
    return TRUE; \
  } \
  else \
    return FALSE; \
}
/**
 * \def GTK_WINDOW_KEY_PRESS(pref, nom)
 * \brief Gestion des touches de l'ensemble des composants de la fenêtre.\n
 *          - widget : composant à l'origine de l'évènement,
 *          - event : description de la touche pressée,
 *          - p : la variable projet.
 * \param pref : le préfixe de la fonction,
 * \param nom : le nom de la fonction.
 * \return TRUE si la touche ECHAP est pressée, FALSE sinon.\n
 *   Echec : FALSE :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */


#define GTK_WINDOW_DESTROY(pref, nom, ...) \
void \
pref##_gtk_##nom##_window_destroy (GtkWidget *object, \
                                   Projet    *p) \
{ \
  BUGMSG (p, , gettext ("Paramètre %s incorrect.\n"), "Projet") \
  BUGMSG (p->ui.pref##_##nom.builder, \
          , \
          gettext ("La fenêtre graphique %s n\'est pas initialisée."), #nom) \
  \
  __VA_ARGS__ \
  g_object_unref (G_OBJECT (p->ui.pref##_##nom.builder)); \
  p->ui.pref##_##nom.builder = NULL; \
  \
  return; \
}
/**
 * \def GTK_WINDOW_DESTROY(pref, nom, ...)
 * \brief Initialise les variables permettant de définir que la fenêtre est
 *        fermée et libère la mémoire.
 *          - widget : composant à l'origine de l'évènement,
 *          - p : la variable projet.
 * \param pref : le préfixe de la fonction,
 * \param nom : le nom de la fonction.
 * \param ... : les instructions à exécuter avant la libération de la fenêtre.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */


#define GTK_WINDOW_CLOSE(pref, nom) \
void \
pref##_gtk_##nom##_window_button_close (GtkButton *button, \
                                        Projet    *p) \
{ \
  BUGMSG (p, , gettext ("Paramètre %s incorrect.\n"), "Projet") \
  BUGMSG (p->ui.pref##_##nom.builder, \
          , \
          gettext ("La fenêtre graphique %s n\'est pas initialisée."), #nom) \
  \
  gtk_widget_destroy (p->ui.pref##_##nom.window); \
  \
  return; \
}
/**
 * \def GTK_WINDOW_CLOSE(pref, nom)
 * \brief Ferme la fenêtre.
 *          - toolbutton : composant à l'origine de l'évènement,
 *          - p : la variable projet.
 * \param pref : le préfixe de la fonction,
 * \param nom : le nom de la fonction.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */    


gboolean common_gtk_treeview_button_press_unselect (GtkTreeView    *widget,
                                                    GdkEventButton *event,
                                                    Projet         *p);

double conv_buff_d                 (GtkTextBuffer *textbuffer,
                                    double         val_min,
                                    gboolean       min_include,
                                    double         val_max,
                                    gboolean       max_include);
unsigned int conv_buff_u           (GtkTextBuffer *textbuffer,
                                    unsigned int   val_min,
                                    gboolean       min_include,
                                    unsigned int   val_max,
                                    gboolean       max_include);
unsigned int common_gtk_entry_uint (GtkEntry      *entry,
                                    unsigned int   val_min,
                                    gboolean       min_include,
                                    unsigned int   val_max,
                                    gboolean       max_include);

void common_gtk_render_double   (GtkTreeViewColumn *tree_column,
                                 GtkCellRenderer   *cell,
                                 GtkTreeModel      *tree_model,
                                 GtkTreeIter       *iter,
                                 gpointer           data);
void common_gtk_render_flottant (GtkTreeViewColumn *tree_column,
                                 GtkCellRenderer   *cell,
                                 GtkTreeModel      *tree_model,
                                 GtkTreeIter       *iter,
                                 gpointer           data);

GtkTreeViewColumn *common_gtk_cree_colonne (char  *nom,
                                            GType  type,
                                            int    num_colonne,
                                            double xalign,
                                            int    num_decimales);

#endif
