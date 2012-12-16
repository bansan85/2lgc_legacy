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

#include "common_erreurs.h"
#include "common_projet.h"
#include "common_tooltip.h"

G_MODULE_EXPORT gboolean _1990_gtk_groupe_option_window_key_press(
  GtkWidget *widget __attribute__((unused)), GdkEvent *event, Projet *projet)
/* Description : Ferme la fenêtre si la touche Escape est pressée.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : GdkEvent *event : paramètres de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : TRUE si la touche ECHAP est pressée, FALSE sinon.
 * Echec : FALSE :
 *           projet == NULL,
 *           interface graphique non initialisée.
 */
{
    BUGMSG(projet, TRUE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk._1990_groupes.builder_options, TRUE, gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Option Groupes");
    
    if (event->key.keyval == GDK_KEY_Escape)
    {
        gtk_widget_destroy(widget);
        return TRUE;
    }
    else
        return FALSE;
}


G_MODULE_EXPORT void _1990_gtk_groupes_options_window_destroy(
  GtkWidget *object __attribute__((unused)), Projet *projet)
/* Description : met projet->list_gtk._1990_groupes.builder_options à NULL quand la fenêtre se
 *               ferme.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk._1990_groupes.builder_options, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Option Groupes");
    
    projet->list_gtk._1990_groupes.builder_options = NULL;
    
    return;
}


G_MODULE_EXPORT void _1990_gtk_groupes_option_window_quitter_button(
  GtkWidget *object __attribute__((unused)), Projet *projet)
/* Description : Bouton de fermeture de la fenêtre.
 * Paramètres : GtkComboBox *widget : composant à l'origine de la demande,
 *            : GtkWidget *fenêtre : la fenêtre d'options.
 * Valeur renvoyée : Aucune.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk._1990_groupes.builder_options, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Option Groupes");
    
    gtk_widget_destroy(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk._1990_groupes.builder_options, "1990_groupes_options_window")));
    
    return;
}


G_MODULE_EXPORT void _1990_gtk_groupes_button_options_clicked(
  GtkWidget *button __attribute__((unused)), Projet *projet)
/* Description : Création de la fenêtre des options des Groupes.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *              Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 *   Echec : projet == NULL,
 *           interface graphique impossible à générer.
 */
{
    GtkSettings *settings;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk._1990_groupes.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Groupes");
    if (projet->list_gtk._1990_groupes.builder_options != NULL)
    {
        gtk_window_present(GTK_WINDOW(gtk_builder_get_object(projet->list_gtk._1990_groupes.builder_options, "1990_groupes_options_window")));
        return;
    }
    
    projet->list_gtk._1990_groupes.builder_options = gtk_builder_new();
    BUGMSG(gtk_builder_add_from_file(projet->list_gtk._1990_groupes.builder_options, DATADIR"/ui/1990_groupes_options.ui", NULL) != 0, , gettext("Builder Failed\n"));
    gtk_builder_connect_signals(projet->list_gtk._1990_groupes.builder_options, projet);
    
    if ((projet->combinaisons.flags & 1) == 0)
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(projet->list_gtk._1990_groupes.builder_options, "1990_groupes_options_radio_button_EQU")), TRUE);
    else
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(projet->list_gtk._1990_groupes.builder_options, "1990_groupes_options_radio_button_EQU_RES")), TRUE);
    
    if ((projet->combinaisons.flags & 8) == 0)
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(projet->list_gtk._1990_groupes.builder_options, "1990_groupes_options_radio_button_6_10a_b")), TRUE);
    else
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(projet->list_gtk._1990_groupes.builder_options, "1990_groupes_options_radio_button_6_10")), TRUE);
    gtk_widget_set_tooltip_window(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk._1990_groupes.builder_options, "1990_groupes_options_radio_button_6_10")), GTK_WINDOW(common_tooltip_generation("1990_6_10")));
    gtk_widget_set_tooltip_window(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk._1990_groupes.builder_options, "1990_groupes_options_radio_button_6_10a_b")), GTK_WINDOW(common_tooltip_generation("1990_6_10a_b")));
    settings = gtk_widget_get_settings(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk._1990_groupes.builder_options, "1990_groupes_options_radio_button_6_10")));
    g_object_set(settings, "gtk-tooltip-timeout", 0, NULL);
    settings = gtk_widget_get_settings(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk._1990_groupes.builder_options, "1990_groupes_options_radio_button_6_10a_b")));
    g_object_set(settings, "gtk-tooltip-timeout", 0, NULL);
    
    if ((projet->combinaisons.flags & 6) == 4)
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(projet->list_gtk._1990_groupes.builder_options, "1990_groupes_options_radio_button_appr3")), TRUE);
    else if ((projet->combinaisons.flags & 6) == 2)
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(projet->list_gtk._1990_groupes.builder_options, "1990_groupes_options_radio_button_appr2")), TRUE);
    else
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(projet->list_gtk._1990_groupes.builder_options, "1990_groupes_options_radio_button_appr1")), TRUE);
    
    if ((projet->combinaisons.flags & 16) == 0)
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(projet->list_gtk._1990_groupes.builder_options, "1990_groupes_options_radio_button_freq")), TRUE);
    else
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(projet->list_gtk._1990_groupes.builder_options, "1990_groupes_options_radio_button_quasi_perm")), TRUE);
    
    gtk_window_set_transient_for(GTK_WINDOW(gtk_builder_get_object(projet->list_gtk._1990_groupes.builder_options, "1990_groupes_options_window")), GTK_WINDOW(gtk_builder_get_object(projet->list_gtk._1990_groupes.builder, "1990_groupes_options_window")));
    
    return;
}


#endif
