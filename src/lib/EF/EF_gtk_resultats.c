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

#include "common_projet.h"
#include "common_erreurs.h"

G_MODULE_EXPORT void EF_gtk_resultats_fermer(GtkButton *button __attribute__((unused)),
  Projet *projet)
/* Description : Ferme la fenêtre.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_resultats.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Résultats");
    
    gtk_widget_destroy(projet->list_gtk.ef_resultats.window);
    
    return;
}


G_MODULE_EXPORT void EF_gtk_resultats_window_destroy(GtkWidget *object __attribute__((unused)),
  Projet *projet)
/* Description : met projet->list_gtk.ef_resultats.builder à NULL quand la fenêtre se ferme,
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_resultats.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Résultats");
    
    g_object_unref(G_OBJECT(projet->list_gtk.ef_resultats.builder));
    projet->list_gtk.ef_resultats.builder = NULL;
    
    return;
}


G_MODULE_EXPORT gboolean EF_gtk_resultats_window_key_press(
  GtkWidget *widget __attribute__((unused)), GdkEvent *event, Projet *projet)
/* Description : Ferme la fenêtre si la touche ECHAP est pressée.
 * Paramètres : GtkWidget *widget : composant à l'origine de l'évènement,
 *            : GdkEvent *event : Caractéristique de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : TRUE si la touche DELETE est pressée, FALSE sinon.
 *   Echec : projet == NULL,
 *           interface graphique non initialisée.
 *  
 */
{
    BUGMSG(projet, TRUE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_resultats.builder, TRUE, gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Résultats");
    
    if (event->key.keyval == GDK_KEY_Escape)
    {
        gtk_widget_destroy(projet->list_gtk.ef_resultats.window);
        return TRUE;
    }
    else
        return FALSE;
}


G_MODULE_EXPORT void EF_gtk_resultats_notebook_switch(GtkNotebook *notebook,
  GtkWidget *page __attribute__((unused)), gint page_num,
  Projet *projet __attribute__((unused)))
/* Description : Le changement de la page en cours nécessite l'actualisation de la disponibilité
 *               du bouton supprimer.
 * Paramètres : GtkNotebook *notebook : le composant notebook,
 *            : GtkWidget *page : composant désignant la page,
 *            : guint page_num : numéro de la page,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    if (page_num == gtk_notebook_get_n_pages(notebook)-1)
        g_signal_stop_emission_by_name(notebook, "switch-page");
    
    return;
}


G_MODULE_EXPORT void EF_gtk_resultats(Projet *projet)
/* Description : Création de la fenêtre permettant d'afficher les résultats sous forme d'un
 *               tableau.
 * Paramètres : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 *   Echec : projet == NULL,
 *           interface graphique impossible à générer.
 */
{
    Gtk_EF_Resultats    *ef_gtk;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    if (projet->list_gtk.ef_resultats.builder != NULL)
    {
        gtk_window_present(GTK_WINDOW(projet->list_gtk.ef_resultats.window));
        return;
    }
    
    ef_gtk = &projet->list_gtk.ef_resultats;
    
    ef_gtk->builder = gtk_builder_new();
    BUGMSG(gtk_builder_add_from_file(ef_gtk->builder, DATADIR"/ui/EF_resultats.ui", NULL) != 0, , gettext("Builder Failed\n"));
    gtk_builder_connect_signals(ef_gtk->builder, projet);
    
    ef_gtk->window = GTK_WIDGET(gtk_builder_get_object(ef_gtk->builder, "EF_resultats_window"));
    
    gtk_window_set_transient_for(GTK_WINDOW(ef_gtk->window), GTK_WINDOW(projet->list_gtk.comp.window));
}

#endif
