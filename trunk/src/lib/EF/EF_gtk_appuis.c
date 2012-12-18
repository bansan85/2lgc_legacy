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
#include "common_selection.h"
#include "common_text.h"
#include "EF_appuis.h"
#include "1992_1_1_barres.h"

G_MODULE_EXPORT void EF_gtk_appuis_fermer(GtkButton *button __attribute__((unused)),
  Projet *projet)
/* Description : Ferme la fenêtre.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_appuis.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Appui");
    
    gtk_widget_destroy(projet->list_gtk.ef_appuis.window);
    
    return;
}


G_MODULE_EXPORT void EF_gtk_appuis_window_destroy(GtkWidget *object __attribute__((unused)),
  Projet *projet)
/* Description : met projet->list_gtk.ef_appuis.builder à NULL quand la fenêtre se ferme,
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_appuis.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Appui");
    
    projet->list_gtk.ef_appuis.builder = NULL;
    
    return;
}


G_MODULE_EXPORT gboolean EF_gtk_appuis_window_key_press(
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
    BUGMSG(projet->list_gtk.ef_appuis.builder, TRUE, gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Appui");
    
    if (event->key.keyval == GDK_KEY_Escape)
    {
        gtk_widget_destroy(projet->list_gtk.ef_appuis.window);
        return TRUE;
    }
    else
        return FALSE;
}


G_MODULE_EXPORT void EF_gtk_appuis_ajouter(GtkButton *button __attribute__((unused)),
  Projet *projet)
/* Description : Ajout un nouvel appui.
 * Paramètres : GtkWidget *widget : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    EF_Appui    *appui;
    GtkTreePath *path;
    GtkTreeModel *model;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_appuis.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Appui");
    
    if (EF_appuis_cherche_nom(projet, gettext("Sans nom"), FALSE) == NULL)
        BUG(appui = EF_appuis_ajout(projet, gettext("Sans nom"), EF_APPUI_LIBRE, EF_APPUI_LIBRE, EF_APPUI_LIBRE, EF_APPUI_LIBRE, EF_APPUI_LIBRE, EF_APPUI_LIBRE), );
    else
    {
        char    *nom;
        int     i = 2;
        
        nom = g_strdup_printf("%s (%d)", gettext("Sans nom"), i);
        while (EF_appuis_cherche_nom(projet, nom, FALSE) != NULL)
        {
            i++;
            free(nom);
            nom = g_strdup_printf("%s (%d)", gettext("Sans nom"), i);
        }
        BUG(appui = EF_appuis_ajout(projet, nom, EF_APPUI_LIBRE, EF_APPUI_LIBRE, EF_APPUI_LIBRE, EF_APPUI_LIBRE, EF_APPUI_LIBRE, EF_APPUI_LIBRE), );
        free(nom);
    }
    
    model = GTK_TREE_MODEL(gtk_builder_get_object(projet->list_gtk.ef_appuis.builder, "EF_appuis_treestore"));
    path = gtk_tree_model_get_path(model, &appui->Iter_fenetre);
    gtk_tree_view_set_cursor(GTK_TREE_VIEW(gtk_builder_get_object(projet->list_gtk.ef_appuis.builder, "EF_appuis_treeview")), path, gtk_tree_view_get_column(GTK_TREE_VIEW(gtk_builder_get_object(projet->list_gtk.ef_appuis.builder, "EF_appuis_treeview")), 0), TRUE);
    gtk_tree_path_free(path);
}


G_MODULE_EXPORT void EF_gtk_appuis_supprimer(GtkButton *button __attribute__((unused)),
  Projet *projet)
/* Description : Supprime l'appui sélectionné dans le treeview.
 * Paramètres : GtkWidget *widget : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    GtkTreeIter     iter;
    GtkTreeModel    *model;
    char            *nom;
    EF_Appui        *appui;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_appuis.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Appui");
    
    if (!gtk_tree_selection_get_selected(GTK_TREE_SELECTION(gtk_builder_get_object(projet->list_gtk.ef_appuis.builder, "EF_appuis_treeview_select")), &model, &iter))
        return;
    
    gtk_tree_model_get(model, &iter, 0, &nom, -1);
    
    BUG(appui = EF_appuis_cherche_nom(projet, nom, TRUE), );
    BUG(EF_appuis_supprime(appui, TRUE, FALSE, projet), );
    
    BUG(m3d_rafraichit(projet), );
    
    free(nom);
    
    return;
}


G_MODULE_EXPORT void EF_gtk_appuis_supprimer_menu_suppr_noeud(
  GtkButton *button __attribute__((unused)), Projet *projet)
/* Description : Supprime l'appui sélectionné dans le treeview et ainsi que les noeuds
 *               utilisant l'appui.
 * Paramètres : GtkWidget *widget : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    GtkTreeIter     iter;
    GtkTreeModel    *model;
    char            *nom;
    EF_Appui        *appui;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_appuis.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Appui");
    
    if (!gtk_tree_selection_get_selected(GTK_TREE_SELECTION(gtk_builder_get_object(projet->list_gtk.ef_appuis.builder, "EF_appuis_treeview_select")), &model, &iter))
        return;
    
    gtk_tree_model_get(model, &iter, 0, &nom, -1);
    
    BUG(appui = EF_appuis_cherche_nom(projet, nom, TRUE), );
    BUG(EF_appuis_supprime(appui, FALSE, TRUE, projet), );
    
    BUG(m3d_rafraichit(projet), );
    
    free(nom);
    
    return;
}


G_MODULE_EXPORT void EF_gtk_appuis_supprimer_menu_modif_noeud(
  GtkButton *button __attribute__((unused)), Projet *projet)
/* Description : Supprime l'appui sélectionné dans le treeview et les noeuds le possédant
 *               deviennent sans appui.
 * Paramètres : GtkWidget *widget : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    GtkTreeIter     iter;
    GtkTreeModel    *model;
    char            *nom;
    EF_Appui        *appui;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_appuis.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Appui");
    
    if (!gtk_tree_selection_get_selected(GTK_TREE_SELECTION(gtk_builder_get_object(projet->list_gtk.ef_appuis.builder, "EF_appuis_treeview_select")), &model, &iter))
        return;
    
    gtk_tree_model_get(model, &iter, 0, &nom, -1);
    
    BUG(appui = EF_appuis_cherche_nom(projet, nom, TRUE), );
    BUG(EF_appuis_supprime(appui, FALSE, FALSE, projet), );
    
    free(nom);
    
    return;
}


G_MODULE_EXPORT void EF_gtk_appuis_edit_type(GtkCellRendererText *cell, gchar *path_string,
  gchar *new_text, Projet *projet)
/* Description : Modification d'un appui.
 * Paramètres : GtkCellRendererText *cell : cellule en cours,
 *            : gchar *path_string : path de la ligne en cours,
 *            : gchar *new_text : nouvelle valeur,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
*/
{
    Gtk_EF_Appuis   *ef_gtk;
    GtkTreeModel    *model;
    GtkTreeIter     iter;
    GtkTreePath     *path;
    char            *nom;
    EF_Appui        *appui;
    int             column = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(cell), "column"));
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_appuis.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Appui");
    
    ef_gtk = &projet->list_gtk.ef_appuis;
    model = GTK_TREE_MODEL(ef_gtk->appuis);
    path = gtk_tree_path_new_from_string(path_string);
    gtk_tree_model_get_iter(model, &iter, path);
    gtk_tree_path_free(path);
    gtk_tree_model_get(model, &iter, 0, &nom, -1);
    BUG(appui = EF_appuis_cherche_nom(projet, nom, TRUE), );
    free(nom);

    if (strcmp(new_text, gettext("Libre")) == 0)
        BUG(EF_appuis_edit(appui, column-1, EF_APPUI_LIBRE, projet), );
    else if (strcmp(new_text, gettext("Bloqué")) == 0)
        BUG(EF_appuis_edit(appui, column-1, EF_APPUI_BLOQUE, projet), );
    else
        BUGMSG(NULL, , gettext("Type d'appui %s inconnu.\n"), new_text);
    
    return;
}


G_MODULE_EXPORT void EF_gtk_appuis_edit_nom(GtkCellRendererText *cell __attribute__((unused)),
  gchar *path_string, gchar *new_text, Projet *projet)
/* Description : Modification du nom d'un appui.
 * Paramètres : GtkCellRendererText *cell : cellule en cours,
 *            : gchar *path_string : path de la ligne en cours,
 *            : gchar *new_text : le nouveau nom,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
*/
{
    Gtk_EF_Appuis   *ef_gtk;
    GtkTreeModel    *model;
    GtkTreeIter     iter;
    GtkTreePath     *path;
    char            *nom;
    EF_Appui        *appui;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_appuis.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Appui");
    
    ef_gtk = &projet->list_gtk.ef_appuis;
    model = GTK_TREE_MODEL(ef_gtk->appuis);
    path = gtk_tree_path_new_from_string(path_string);
    gtk_tree_model_get_iter(model, &iter, path);
    gtk_tree_path_free(path);
    gtk_tree_model_get(model, &iter, 0, &nom, -1);
    if (strcmp(nom, new_text) == 0)
    {
        free(nom);
        return;
    }
    BUG(appui = EF_appuis_cherche_nom(projet, nom, TRUE), );
    free(nom);
    if (strcmp(appui->nom, new_text) == 0)
        return;

    BUG(EF_appuis_renomme(appui, new_text, projet), );
    
    return;
}


G_MODULE_EXPORT void EF_gtk_appuis_select_changed(
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
    BUGMSG(projet->list_gtk.ef_appuis.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Appui");
    
    // Si aucun appui n'est sélectionné, il n'est pas possible d'en supprimer un.
    if (!gtk_tree_selection_get_selected(GTK_TREE_SELECTION(gtk_builder_get_object(projet->list_gtk.ef_appuis.builder, "EF_appuis_treeview_select")), &model, &Iter))
    {
        gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_appuis.builder, "EF_appuis_boutton_supprimer_direct")), FALSE);
        gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_appuis.builder, "EF_appuis_boutton_supprimer_menu")), FALSE);
        gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_appuis.builder, "EF_appuis_boutton_supprimer_direct")), FALSE);
        gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_appuis.builder, "EF_appuis_boutton_supprimer_menu")), TRUE);
    }
    else
    {
        char        *nom;
        EF_Appui    *appui;
        
        GList   *liste_appuis = NULL, *liste_noeuds_dep, *liste_barres_dep, *liste_charges_dep;
        
        gtk_tree_model_get(model, &Iter, 0, &nom, -1);
        
        BUG(appui = EF_appuis_cherche_nom(projet, nom, TRUE), );
        
        liste_appuis = g_list_append(liste_appuis, appui);
        BUG(_1992_1_1_barres_cherche_dependances(projet, liste_appuis, NULL, NULL, NULL, NULL, &liste_noeuds_dep, &liste_barres_dep, &liste_charges_dep, FALSE, FALSE), );
        g_list_free(liste_appuis);
        
        if ((liste_noeuds_dep != NULL) || (liste_barres_dep != NULL) || (liste_charges_dep != NULL))
        {
            char    *desc;
            
            gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_appuis.builder, "EF_appuis_boutton_supprimer_direct")), FALSE);
            gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_appuis.builder, "EF_appuis_boutton_supprimer_menu")), TRUE);
            gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_appuis.builder, "EF_appuis_boutton_supprimer_direct")), FALSE);
            gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_appuis.builder, "EF_appuis_boutton_supprimer_menu")), TRUE);
            desc = common_text_dependances(liste_noeuds_dep, liste_barres_dep, liste_charges_dep, projet);
            gtk_menu_item_set_label(GTK_MENU_ITEM(gtk_builder_get_object(projet->list_gtk.ef_appuis.builder, "EF_appuis_supprimer_menu_suppr_noeud")), desc);
            free(desc);
        }
        else
        {
            gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_appuis.builder, "EF_appuis_boutton_supprimer_direct")), TRUE);
            gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_appuis.builder, "EF_appuis_boutton_supprimer_menu")), FALSE);
            gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_appuis.builder, "EF_appuis_boutton_supprimer_direct")), TRUE);
            gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_appuis.builder, "EF_appuis_boutton_supprimer_menu")), FALSE);
        }
        
        free(nom);
        g_list_free(liste_noeuds_dep);
        g_list_free(liste_barres_dep);
        g_list_free(liste_charges_dep);
    }
    
    return;
}


G_MODULE_EXPORT void EF_gtk_appuis(Projet *projet)
/* Description : Création de la fenêtre permettant d'afficher les appuis sous forme d'un
 *               tableau.
 * Paramètres : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 *   Echec : projet == NULL,
 *           interface graphique impossible à générer.
 */
{
    Gtk_EF_Appuis   *ef_gtk;
    GList           *list_parcours;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    if (projet->list_gtk.ef_appuis.builder != NULL)
    {
        gtk_window_present(GTK_WINDOW(projet->list_gtk.ef_appuis.window));
        return;
    }
    
    ef_gtk = &projet->list_gtk.ef_appuis;
    
    ef_gtk->builder = gtk_builder_new();
    BUGMSG(gtk_builder_add_from_file(ef_gtk->builder, DATADIR"/ui/EF_appuis.ui", NULL) != 0, , gettext("Builder Failed\n"));
    gtk_builder_connect_signals(ef_gtk->builder, projet);
    
    ef_gtk->window = GTK_WIDGET(gtk_builder_get_object(ef_gtk->builder, "EF_appuis_window"));;
    ef_gtk->appuis = GTK_TREE_STORE(gtk_builder_get_object(ef_gtk->builder, "EF_appuis_treestore"));
    
    g_object_set(gtk_builder_get_object(ef_gtk->builder, "EF_appuis_treeview_cell1"), "model", projet->list_gtk.ef_appuis.liste_type_appui, NULL);
    g_object_set(gtk_builder_get_object(ef_gtk->builder, "EF_appuis_treeview_cell2"), "model", projet->list_gtk.ef_appuis.liste_type_appui, NULL);
    g_object_set(gtk_builder_get_object(ef_gtk->builder, "EF_appuis_treeview_cell3"), "model", projet->list_gtk.ef_appuis.liste_type_appui, NULL);
    g_object_set(gtk_builder_get_object(ef_gtk->builder, "EF_appuis_treeview_cell4"), "model", projet->list_gtk.ef_appuis.liste_type_appui, NULL);
    g_object_set(gtk_builder_get_object(ef_gtk->builder, "EF_appuis_treeview_cell5"), "model", projet->list_gtk.ef_appuis.liste_type_appui, NULL);
    g_object_set(gtk_builder_get_object(ef_gtk->builder, "EF_appuis_treeview_cell6"), "model", projet->list_gtk.ef_appuis.liste_type_appui, NULL);
    
    list_parcours = projet->ef_donnees.appuis;
    while (list_parcours != NULL)
    {
        EF_Appui    *appui = (EF_Appui*)list_parcours->data;
        char        *txt_uxa, *txt_uya, *txt_uza, *txt_rxa, *txt_rya, *txt_rza;
        
        BUG(EF_appuis_get_description(appui, &txt_uxa, &txt_uya, &txt_uza, &txt_rxa, &txt_rya, &txt_rza), );
        gtk_tree_store_append(ef_gtk->appuis, &appui->Iter_fenetre, NULL);
        gtk_tree_store_set(ef_gtk->appuis, &appui->Iter_fenetre, 0, appui->nom, 1, txt_uxa, 2, txt_uya, 3, txt_uza, 4, txt_rxa, 5, txt_rya, 6, txt_rza, -1);
        free(txt_uxa);
        free(txt_uya);
        free(txt_uza);
        free(txt_rxa);
        free(txt_rya);
        free(txt_rza);
        list_parcours = g_list_next(list_parcours);
    }
    
    g_object_set_data(gtk_builder_get_object(ef_gtk->builder, "EF_appuis_treeview_cell1"), "column", GINT_TO_POINTER(1));
    g_object_set_data(gtk_builder_get_object(ef_gtk->builder, "EF_appuis_treeview_cell2"), "column", GINT_TO_POINTER(2));
    g_object_set_data(gtk_builder_get_object(ef_gtk->builder, "EF_appuis_treeview_cell3"), "column", GINT_TO_POINTER(3));
    g_object_set_data(gtk_builder_get_object(ef_gtk->builder, "EF_appuis_treeview_cell4"), "column", GINT_TO_POINTER(4));
    g_object_set_data(gtk_builder_get_object(ef_gtk->builder, "EF_appuis_treeview_cell5"), "column", GINT_TO_POINTER(5));
    g_object_set_data(gtk_builder_get_object(ef_gtk->builder, "EF_appuis_treeview_cell6"), "column", GINT_TO_POINTER(6));
    
    gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_appuis.builder, "EF_appuis_boutton_supprimer_direct")), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_appuis.builder, "EF_appuis_boutton_supprimer_menu")), FALSE);
    
    gtk_window_set_transient_for(GTK_WINDOW(ef_gtk->window), GTK_WINDOW(projet->list_gtk.comp.window));
}

#endif
