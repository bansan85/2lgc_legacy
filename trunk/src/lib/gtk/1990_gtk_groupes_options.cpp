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
#ifdef ENABLE_GTK
#include <libintl.h>
#include <locale.h>
#include <gtk/gtk.h>

#include "common_erreurs.hpp"
#include "common_gtk.hpp"
#include "common_projet.hpp"
#include "common_tooltip.hpp"
#include "1990_gtk_groupes_options.hpp"


GTK_WINDOW_KEY_PRESS (_1990, groupes_options);


GTK_WINDOW_DESTROY (_1990, groupes_options, );


GTK_WINDOW_CLOSE (_1990, groupes_options);


/**
 * \brief Création de la fenêtre des options des Groupes.
 * \param button : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique impossible à générer.
 */
extern "C"
void
_1990_gtk_groupes_button_options_clicked (GtkWidget *button,
                                          Projet    *p)
{
  GtkSettings *settings;
  
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_GRO.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Groupes"); )
  if (UI_GROOP.builder != NULL)
  {
    gtk_window_present (GTK_WINDOW (UI_GROOP.window));
    return;
  }
  
  UI_GROOP.builder = gtk_builder_new ();
  BUGCRIT (gtk_builder_add_from_resource (UI_GROOP.builder,
                                "/org/2lgc/codegui/ui/1990_groupes_options.ui",
                                         NULL) != 0,
          ,
          (gettext ("La génération de la fenêtre %s a échouée.\n"),
                    "Actions_Option"); )
  gtk_builder_connect_signals (UI_GROOP.builder, p);
  
  UI_GROOP.window = GTK_WIDGET (gtk_builder_get_object (
                             UI_GROOP.builder, "1990_groupes_options_window"));
  
  if (p->combinaisons.elu_equ_methode == 0)
  {
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gtk_builder_get_object (
                   UI_GROOP.builder, "1990_groupes_options_radio_button_EQU")),
                                  TRUE);
  }
  else
  {
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gtk_builder_get_object (
               UI_GROOP.builder, "1990_groupes_options_radio_button_EQU_RES")),
                                  TRUE);
  }
  
  if (p->combinaisons.form_6_10 == 0)
  {
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gtk_builder_get_object (
               UI_GROOP.builder, "1990_groupes_options_radio_button_6_10a_b")),
                                  TRUE);
  }
  else
  {
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gtk_builder_get_object (
                  UI_GROOP.builder, "1990_groupes_options_radio_button_6_10")),
                                  TRUE);
  }
  gtk_widget_set_tooltip_window (GTK_WIDGET (gtk_builder_get_object (
                  UI_GROOP.builder, "1990_groupes_options_radio_button_6_10")),
                         GTK_WINDOW (common_tooltip_generation ("1990_6_10")));
  gtk_widget_set_tooltip_window (GTK_WIDGET (gtk_builder_get_object (
               UI_GROOP.builder, "1990_groupes_options_radio_button_6_10a_b")),
                      GTK_WINDOW (common_tooltip_generation ("1990_6_10a_b")));
  settings = gtk_widget_get_settings (GTK_WIDGET (gtk_builder_get_object (
                 UI_GROOP.builder, "1990_groupes_options_radio_button_6_10")));
  g_object_set (settings, "gtk-tooltip-timeout", 0, NULL);
  settings = gtk_widget_get_settings (GTK_WIDGET (gtk_builder_get_object (
              UI_GROOP.builder, "1990_groupes_options_radio_button_6_10a_b")));
  g_object_set (settings, "gtk-tooltip-timeout", 0, NULL);
  
  if (p->combinaisons.elu_geo_str_methode == 2)
  {
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gtk_builder_get_object (
                 UI_GROOP.builder, "1990_groupes_options_radio_button_appr3")),
                                  TRUE);
  }
  else if (p->combinaisons.elu_geo_str_methode == 1)
  {
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gtk_builder_get_object (
                 UI_GROOP.builder, "1990_groupes_options_radio_button_appr2")),
                                  TRUE);
  }
  else
  {
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gtk_builder_get_object (
                 UI_GROOP.builder, "1990_groupes_options_radio_button_appr1")),
                                  TRUE);
  }
  
  if (p->combinaisons.elu_acc_psi == 0)
  {
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gtk_builder_get_object (
                  UI_GROOP.builder, "1990_groupes_options_radio_button_freq")),
                                  TRUE);
  }
  else
  {
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gtk_builder_get_object (
            UI_GROOP.builder, "1990_groupes_options_radio_button_quasi_perm")),
                                  TRUE);
  }
  
  gtk_window_set_transient_for (GTK_WINDOW (gtk_builder_get_object (
                             UI_GROOP.builder, "1990_groupes_options_window")),
                                GTK_WINDOW (gtk_builder_get_object (
                              UI_GRO.builder, "1990_groupes_options_window")));
  
  return;
}


#endif

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
