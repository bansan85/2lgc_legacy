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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>
#include <math.h>

#include "common_m3d.hpp"

extern "C" {
#include "common_projet.h"
#include "common_erreurs.h"
#include "common_gtk.h"
#include "common_maths.h"
#include "common_selection.h"
#include "EF_appuis.h"
#include "EF_noeud.h"
#include "EF_section.h"
#include "EF_relachement.h"
#include "EF_charge_noeud.h"
#include "1990_actions.h"
#include "1992_1_1_barres.h"
#include "1992_1_1_materiaux.h"

G_MODULE_EXPORT void EF_gtk_sections_fermer(GtkButton *button __attribute__((unused)),
  Projet *projet)
/* Description : Ferme la fenêtre.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_sections.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Section");
    
    gtk_widget_destroy(projet->list_gtk.ef_sections.window);
    
    return;
}


G_MODULE_EXPORT void EF_gtk_sections_window_destroy(GtkWidget *object __attribute__((unused)),
  Projet *projet)
/* Description : met projet->list_gtk.ef_sections.builder à NULL quand la fenêtre se ferme,
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_sections.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Section");
    
    projet->list_gtk.ef_sections.builder = NULL;
    
    return;
}


G_MODULE_EXPORT gboolean EF_gtk_sections_window_key_press(
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
    BUGMSG(projet->list_gtk.ef_sections.builder, TRUE, gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Section");
    
    if (event->key.keyval == GDK_KEY_Escape)
    {
        gtk_widget_destroy(projet->list_gtk.ef_sections.window);
        return TRUE;
    }
    else
        return FALSE;
}


G_MODULE_EXPORT void EF_gtk_sections_select_changed(
  GtkTreeSelection *treeselection __attribute__((unused)), Projet *projet)
/* Description : En fonction de la sélection, active ou désactive le bouton supprimer.
 * Paramètres : GtkTreeSelection *treeselection : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 *   Echec : projet == NULL,
 *           interface graphique non initialisée.
 */
{
    GtkTreeModel    *model;
    GtkTreeIter     Iter;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_sections.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Section");
    
    // Si aucune section n'est sélectionné, il n'est pas possible d'en supprimer un.
    if (!gtk_tree_selection_get_selected(GTK_TREE_SELECTION(gtk_builder_get_object(projet->list_gtk.ef_sections.builder, "EF_sections_treeview_select")), &model, &Iter))
    {
        gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_sections.builder, "EF_sections_boutton_supprimer_direct")), FALSE);
        gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_sections.builder, "EF_sections_boutton_supprimer_menu")), FALSE);
        gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_sections.builder, "EF_sections_boutton_supprimer_direct")), TRUE);
        gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_sections.builder, "EF_sections_boutton_supprimer_menu")), FALSE);
    }
    else
    {
        char        *nom;
        EF_Section  *section;
        
        gtk_tree_model_get(model, &Iter, 1, &nom, -1);
        
        BUG(section = EF_sections_cherche_nom(projet, nom, TRUE), );
        
        if (EF_sections_verifie_dependances(projet, section))
        {
            gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_sections.builder, "EF_sections_boutton_supprimer_direct")), FALSE);
            gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_sections.builder, "EF_sections_boutton_supprimer_menu")), TRUE);
            gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_sections.builder, "EF_sections_boutton_supprimer_direct")), FALSE);
            gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_sections.builder, "EF_sections_boutton_supprimer_menu")), TRUE);
        }
        else
        {
            gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_sections.builder, "EF_sections_boutton_supprimer_direct")), TRUE);
            gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_sections.builder, "EF_sections_boutton_supprimer_menu")), FALSE);
            gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_sections.builder, "EF_sections_boutton_supprimer_direct")), TRUE);
            gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_sections.builder, "EF_sections_boutton_supprimer_menu")), FALSE);
        }
        
        free(nom);
    }
    
    return;
}


G_MODULE_EXPORT void EF_gtk_sections_edit_nom(GtkCellRendererText *cell __attribute__((unused)),
  gchar *path_string, gchar *new_text, Projet *projet)
/* Description : Modification du nom d'une section.
 * Paramètres : GtkCellRendererText *cell : cellule en cours,
 *            : gchar *path_string : path de la ligne en cours,
 *            : gchar *new_text : le nouveau nom,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
*/
{
    List_Gtk_EF_Sections  *ef_gtk;
    GtkTreeModel *model;
    GtkTreeIter  iter;
    GtkTreePath  *path;
    char         *nom;
    EF_Section   *section;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_sections.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Section");
    
    ef_gtk = &projet->list_gtk.ef_sections;
    model = GTK_TREE_MODEL(ef_gtk->sections);
    path = gtk_tree_path_new_from_string(path_string);
    gtk_tree_model_get_iter(model, &iter, path);
    gtk_tree_path_free(path);
    gtk_tree_model_get(model, &iter, 1, &nom, -1);
    if (strcmp(nom, new_text) == 0)
    {
        free(nom);
        return;
    }
    BUG(section = EF_sections_cherche_nom(projet, nom, TRUE), );
    free(nom);
    if (strcmp(section->nom, new_text) == 0)
        return;

    BUG(EF_sections_renomme(section, new_text, projet), );
    
    return;
}


G_MODULE_EXPORT void EF_gtk_sections_supprimer_direct(GtkButton *button __attribute__((unused)),
  Projet *projet)
/* Description : Supprime la section sélectionné dans le treeview.
 * Paramètres : GtkWidget *widget : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    GtkTreeIter     iter;
    GtkTreeModel    *model;
    char            *nom;
    EF_Section      *section;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_sections.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Appui");
    
    if (!gtk_tree_selection_get_selected(GTK_TREE_SELECTION(gtk_builder_get_object(projet->list_gtk.ef_sections.builder, "EF_sections_treeview_select")), &model, &iter))
        return;
    
    gtk_tree_model_get(model, &iter, 1, &nom, -1);
    
    BUG(section = EF_sections_cherche_nom(projet, nom, TRUE), );
    BUG(EF_sections_supprime(section, TRUE, projet), );
    
    BUG(m3d_rafraichit(projet), );
    
    free(nom);
    
    return;
}


G_MODULE_EXPORT void EF_gtk_sections_supprimer_menu_barres(
  GtkButton *button __attribute__((unused)), Projet *projet)
/* Description : Supprime la section sélectionné dans le treeview, y compris les barres
 *               l'utilisant.
 * Paramètres : GtkWidget *widget : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    GtkTreeIter     iter;
    GtkTreeModel    *model;
    char            *nom;
    EF_Section      *section;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_sections.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Appui");
    
    if (!gtk_tree_selection_get_selected(GTK_TREE_SELECTION(gtk_builder_get_object(projet->list_gtk.ef_sections.builder, "EF_sections_treeview_select")), &model, &iter))
        return;
    
    gtk_tree_model_get(model, &iter, 1, &nom, -1);
    
    BUG(section = EF_sections_cherche_nom(projet, nom, TRUE), );
    BUG(EF_sections_supprime(section, FALSE, projet), );
    
    BUG(m3d_rafraichit(projet), );
    
    free(nom);
    
    return;
}


G_MODULE_EXPORT void EF_gtk_sections(Projet *projet)
/* Description : Création de la fenêtre permettant d'afficher les sections sous forme d'un
 *               tableau.
 * Paramètres : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 *   Echec : projet == NULL,
 *           interface graphique impossible à générer.
 */
{
    List_Gtk_EF_Sections    *ef_gtk;
    GList   *list_parcours;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_sections.builder == NULL, , gettext("La fenêtre graphique %s est déjà initialisée.\n"), "Section");
    
    ef_gtk = &projet->list_gtk.ef_sections;
    
    ef_gtk->builder = gtk_builder_new();
    BUGMSG(gtk_builder_add_from_file(ef_gtk->builder, DATADIR"/ui/EF_sections.ui", NULL) != 0, , gettext("Builder Failed\n"));
    gtk_builder_connect_signals(ef_gtk->builder, projet);
    
    ef_gtk->window = GTK_WIDGET(gtk_builder_get_object(ef_gtk->builder, "EF_sections_window"));;
    ef_gtk->sections = GTK_TREE_STORE(gtk_builder_get_object(ef_gtk->builder, "EF_sections_treestore"));
    
    list_parcours = projet->beton.sections;
    while (list_parcours != NULL)
    {
        EF_Section  *section = (EF_Section *)list_parcours->data;
        char        *description;
        
        BUG(description = EF_sections_get_description(section), );
        gtk_tree_store_append(ef_gtk->sections, &section->Iter_fenetre, NULL);
        gtk_tree_store_set(ef_gtk->sections, &section->Iter_fenetre, 0, NULL, 1, section->nom, 2, description, -1);
        free(description);
        
        list_parcours = g_list_next(list_parcours);
    }
    
    gtk_window_set_transient_for(GTK_WINDOW(ef_gtk->window), GTK_WINDOW(projet->list_gtk.comp.window));
}

}

#endif
