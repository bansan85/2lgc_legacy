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
#include <string.h>

#include "common_m3d.hpp"

#include "common_projet.h"
#include "common_erreurs.h"
#include "common_gtk.h"

void EF_gtk_rapport_fermer(GtkButton *button, Projet *projet)
/* Description : Ferme la fenêtre.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet")
    BUGMSG(projet->list_gtk.ef_rapport.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Appui")
    
    gtk_widget_destroy(projet->list_gtk.ef_rapport.window);
    
    return;
}


void EF_gtk_rapport_window_destroy(GtkWidget *object, Projet *projet)
/* Description : met projet->list_gtk.ef_rapport.builder à NULL quand la fenêtre se ferme,
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet")
    BUGMSG(projet->list_gtk.ef_rapport.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Rapport")
    
    g_object_unref(G_OBJECT(projet->list_gtk.ef_rapport.builder));
    projet->list_gtk.ef_rapport.builder = NULL;
    
    return;
}


gboolean EF_gtk_rapport_window_key_press(GtkWidget *widget, GdkEvent *event, Projet *projet)
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
    BUGMSG(projet, TRUE, gettext("Paramètre %s incorrect.\n"), "projet")
    BUGMSG(projet->list_gtk.ef_rapport.builder, TRUE, gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Rapport")
    
    if (event->key.keyval == GDK_KEY_Escape)
    {
        gtk_widget_destroy(projet->list_gtk.ef_rapport.window);
        return TRUE;
    }
    else
        return FALSE;
}


void EF_gtk_rapport(Projet *projet, GList *rapport)
/* Description : Création de la fenêtre permettant d'afficher un rapport.
 * Paramètres : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 *   Echec : projet == NULL,
 *           interface graphique impossible à générer.
 */
{
    Gtk_EF_Rapport  *ef_gtk;
    GList           *list_parcours;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet")
    
    ef_gtk = &projet->list_gtk.ef_rapport;
    
    if (projet->list_gtk.ef_rapport.builder != NULL)
    {
        gtk_window_present(GTK_WINDOW(projet->list_gtk.ef_rapport.window));
        gtk_list_store_clear(ef_gtk->liste);
    }
    else
    {
        
        ef_gtk->builder = gtk_builder_new();
        BUGMSG(gtk_builder_add_from_resource(ef_gtk->builder, "/org/2lgc/codegui/ui/EF_rapport.ui", NULL) != 0, , gettext("Builder Failed\n"))
        gtk_builder_connect_signals(ef_gtk->builder, projet);
        
        ef_gtk->window = GTK_WIDGET(gtk_builder_get_object(ef_gtk->builder, "EF_rapport_window"));
        ef_gtk->liste = GTK_LIST_STORE(gtk_builder_get_object(ef_gtk->builder, "EF_rapport_treestore"));
    }
    ef_gtk->rapport = rapport;
    
    list_parcours = rapport;
    while (list_parcours != NULL)
    {
        Analyse_Comm    *analyse = list_parcours->data;
        GtkTreeIter     Iter;
        GdkPixbuf       *pixbuf;
        
        if (analyse->resultat == 0)
            pixbuf = gtk_widget_render_icon_pixbuf(ef_gtk->window, GTK_STOCK_APPLY, GTK_ICON_SIZE_MENU);
        else if (analyse->resultat == 1)
            pixbuf = gtk_widget_render_icon_pixbuf(ef_gtk->window, GTK_STOCK_DIALOG_WARNING, GTK_ICON_SIZE_MENU);
        else if (analyse->resultat == 2)
            pixbuf = gtk_widget_render_icon_pixbuf(ef_gtk->window, GTK_STOCK_DIALOG_ERROR, GTK_ICON_SIZE_MENU);
        else
            BUGMSG(NULL, , gettext("Le résultat d'un rapport doit être compris entre 0 et 2.\n"))
        
        gtk_list_store_append(ef_gtk->liste, &Iter);
        gtk_list_store_set(ef_gtk->liste, &Iter, 0, pixbuf, 1, analyse->analyse, 2, analyse->commentaire, -1);
        g_object_unref(pixbuf);

        list_parcours = g_list_next(list_parcours);
    }
    
    gtk_window_set_transient_for(GTK_WINDOW(ef_gtk->window), GTK_WINDOW(projet->list_gtk.comp.window));
}

#endif
