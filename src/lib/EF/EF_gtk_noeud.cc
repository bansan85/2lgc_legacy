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
#include "EF_noeud.h"
#include "EF_appuis.h"
#include "EF_charge_noeud.h"
#include "1990_actions.h"
#include "1992_1_1_barres.h"

G_MODULE_EXPORT void EF_gtk_noeud_fermer(GtkButton *button __attribute__((unused)),
  Projet *projet)
/* Description : Ferme la fenêtre sans effectuer les modifications.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_noeud.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Noeuds");
    
    gtk_widget_destroy(projet->list_gtk.ef_noeud.window);
    
    return;
}


G_MODULE_EXPORT void EF_gtk_noeud_ajouter(GtkButton *button __attribute__((unused)),
  Projet *projet)
/* Description : Ajoute un nouveau noeud libre ou intermédiaire en fonction de l'onglet en cours
 *               d'affichage.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    List_Gtk_EF_Noeud   *ef_gtk;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_noeud.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Noeuds");
    
    ef_gtk = &projet->list_gtk.ef_noeud;
    
    // On ajoute un noeud libre
    if (gtk_notebook_get_current_page(GTK_NOTEBOOK(ef_gtk->notebook)) == 0)
    {
        EF_Noeud    *noeud;
        EF_Point    *point;
        
        BUG(noeud = EF_noeuds_ajout_noeud_libre(projet, 0., 0., 0., NULL), );
        BUG(point = EF_noeuds_renvoie_position(noeud), );
        gtk_tree_store_append(ef_gtk->tree_store_libre, &noeud->Iter, NULL);
        gtk_tree_store_set(ef_gtk->tree_store_libre, &noeud->Iter, 0, noeud->numero, 1, point->x, 2, point->y, 3, point->z, 4, (noeud->appui == NULL ? gettext("Aucun") : noeud->appui->nom), -1);
        
        free(point);
    }
    // On ajoute un noeud intermédiaire
    else
    {
        EF_Noeud    *noeud;
        EF_Point        *point;
        EF_Noeud_Barre  *info;
        
        if (projet->beton.barres == NULL)
            return;
        
        BUG(noeud = EF_noeuds_ajout_noeud_barre(projet, (Beton_Barre*)projet->beton.barres->data, 0.5, NULL), );
        BUG(point = EF_noeuds_renvoie_position(noeud), );
        info = (EF_Noeud_Barre *)noeud->data;
        gtk_tree_store_append(ef_gtk->tree_store_barre, &noeud->Iter, NULL);
        gtk_tree_store_set(ef_gtk->tree_store_barre, &noeud->Iter, 0, noeud->numero, 1, point->x, 2, point->y, 3, point->z, 4, (noeud->appui == NULL ? gettext("Aucun") : noeud->appui->nom), 5, info->barre->numero, 6, info->position_relative_barre, -1);
        
        free(point);
    }
    
    return;
}


G_MODULE_EXPORT void EF_gtk_noeud_supprimer(GtkButton *button __attribute__((unused)),
  Projet *projet)
/* Description : Supprime le noeud sélectionné en fonction de l'onglet en cours d'affichage.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    List_Gtk_EF_Noeud   *ef_gtk;
    GtkTreeModel        *model;
    GtkTreeIter         Iter;
    unsigned int        num;
    EF_Noeud            *noeud;
    GList               *list = NULL;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_noeud.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Noeuds");
    
    ef_gtk = &projet->list_gtk.ef_noeud;
    
    // On supprimer un noeud libre
    if (gtk_notebook_get_current_page(GTK_NOTEBOOK(ef_gtk->notebook)) == 0)
    {
        if (!gtk_tree_selection_get_selected(GTK_TREE_SELECTION(gtk_builder_get_object(ef_gtk->builder, "EF_noeuds_treeview_noeuds_libres_select")), &model, &Iter))
            BUGMSG(NULL, , gettext("Aucun noeud n'est sélectionné.\n"));
    }
    // On supprimer un noeud intermédiaire
    else
    {
        if (!gtk_tree_selection_get_selected(GTK_TREE_SELECTION(gtk_builder_get_object(ef_gtk->builder, "EF_noeuds_treeview_noeuds_intermediaires_select")), &model, &Iter))
            BUGMSG(NULL, , gettext("Aucun noeud n'est sélectionné.\n"));
    }
    
    gtk_tree_model_get(model, &Iter, 0, &num, -1);
    
    BUG(noeud = EF_noeuds_cherche_numero(projet, num), );
    
    list = g_list_append(list, noeud);
    
    BUG(_1992_1_1_barres_supprime_liste(projet, list, NULL), );
    
    g_list_free(list);
    
    BUG(m3d_rafraichit(projet), );
    
    return;
}


G_MODULE_EXPORT void EF_noeuds_treeview_select_changed(
  GtkTreeSelection *treeselection __attribute__((unused)), Projet *projet)
/* Description : En fonction de la sélection, active ou désactive le bouton supprimer.
 * Paramètres : GtkTreeSelection *treeselection : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 *   Echec : projet == NULL,
 *           interface graphique non initialisée.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_noeud.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Noeud");
    
    if (gtk_notebook_get_current_page(GTK_NOTEBOOK(projet->list_gtk.ef_noeud.notebook)) == 0)
    {
        if (!gtk_tree_selection_get_selected(GTK_TREE_SELECTION(gtk_builder_get_object(projet->list_gtk.ef_noeud.builder, "EF_noeuds_treeview_noeuds_libres_select")), NULL, NULL))
            gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_noeud.builder, "EF_noeuds_boutton_supprimer")), FALSE);
        else
            gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_noeud.builder, "EF_noeuds_boutton_supprimer")), TRUE);
    }
    // On supprimer un noeud intermédiaire
    else
    {
        if (!gtk_tree_selection_get_selected(GTK_TREE_SELECTION(gtk_builder_get_object(projet->list_gtk.ef_noeud.builder, "EF_noeuds_treeview_noeuds_intermediaires_select")), NULL, NULL))
            gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_noeud.builder, "EF_noeuds_boutton_supprimer")), FALSE);
        else
            gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_noeud.builder, "EF_noeuds_boutton_supprimer")), TRUE);
    }
    
    return;
}


G_MODULE_EXPORT void EF_gtk_noeuds_notebook_change(
  GtkNotebook *notebook __attribute__((unused)), GtkWidget *page __attribute__((unused)),
  guint page_num, Projet *projet)
/* Description : Le changement de la page en cours nécessite l'actualisation de la disponibilité
 *               du bouton supprimer.
 * Paramètres : GtkNotebook *notebook : le composant notebook,
 *            : gint arg1 : argument inconnu,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    if (page_num == 0)
    {
        if (!gtk_tree_selection_get_selected(GTK_TREE_SELECTION(gtk_builder_get_object(projet->list_gtk.ef_noeud.builder, "EF_noeuds_treeview_noeuds_libres_select")), NULL, NULL))
            gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_noeud.builder, "EF_noeuds_boutton_supprimer")), FALSE);
        else
            gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_noeud.builder, "EF_noeuds_boutton_supprimer")), TRUE);
    }
    // On supprimer un noeud intermédiaire
    else
    {
        if (!gtk_tree_selection_get_selected(GTK_TREE_SELECTION(gtk_builder_get_object(projet->list_gtk.ef_noeud.builder, "EF_noeuds_treeview_noeuds_intermediaires_select")), NULL, NULL))
            gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_noeud.builder, "EF_noeuds_boutton_supprimer")), FALSE);
        else
            gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_noeud.builder, "EF_noeuds_boutton_supprimer")), TRUE);
    }
    
    return;
}


G_MODULE_EXPORT void EF_gtk_noeud_edit_pos_abs(GtkCellRendererText *cell, gchar *path_string,
  gchar *new_text, Projet *projet)
/* Description : Changement de la position d'un noeud.
 * Paramètres : GtkCellRendererText *cell : cellule en cours,
 *            : gchar *path_string : path de la ligne en cours,
 *            : gchar *new_text : nouvelle valeur,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    List_Gtk_EF_Noeud   *gtk_noeud;
    GtkTreeModel        *model;
    GtkTreePath         *path;
    GtkTreeIter         iter;
    unsigned int        i;
    char                *fake = (char*)malloc(sizeof(char)*(strlen(new_text)+1));
    double              conversion;
    gint                column = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(cell), "column"));
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_noeud.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Noeuds");
    BUGMSG(projet->ef_donnees.noeuds, , gettext("Aucun noeud n'est existant.\n"));
    BUGMSG(fake, , gettext("Erreur d'allocation mémoire.\n"));
    BUGMSG(new_text, , gettext("Paramètre %s incorrect.\n"), "new_text");
    
    gtk_noeud = &projet->list_gtk.ef_noeud;
    model = GTK_TREE_MODEL(gtk_noeud->tree_store_libre);
    path = gtk_tree_path_new_from_string(path_string);
    
    gtk_tree_model_get_iter(model, &iter, path);
    gtk_tree_path_free(path);
    gtk_tree_model_get(model, &iter, 0, &i, -1);
    
    // On vérifie si le texte contient bien un nombre flottant
    if (sscanf(new_text, "%lf%s", &conversion, fake) == 1)
    {
        EF_Noeud    *noeud;
        EF_Point    *point;
        GList       *liste_noeuds_dep, *liste_barres_dep;
        GList       *liste_noeuds = NULL;
        
        // On modifie l'action
        BUG(noeud = EF_noeuds_cherche_numero(projet, i), );
        point = (EF_Point *)noeud->data;
        
        switch (column)
        {
            case 1:
            {
                point->x = conversion;
                break;
            }
            case 2:
            {
                point->y = conversion;
                break;
            }
            case 3:
            {
                point->z = conversion;
                break;
            }
            default :
            {
                BUGMSG(NULL, , gettext("La colonne d'où provient l'édition est incorrecte.\n"));
                break;
            }
        }
        
        liste_noeuds = g_list_append(liste_noeuds, noeud);
        BUG(_1992_1_1_barres_cherche_dependances(projet, liste_noeuds, NULL, &liste_noeuds_dep, &liste_barres_dep), );
        g_list_free(liste_noeuds);
        
        BUG(m3d_actualise_graphique(projet, liste_noeuds_dep, liste_barres_dep), );
        BUG(m3d_rafraichit(projet), );
        
        g_list_free(liste_noeuds_dep);
        g_list_free(liste_barres_dep);
        
        // On modifie le tree-view-actions
        gtk_tree_store_set(gtk_noeud->tree_store_libre, &iter, column, conversion, -1);
    }
    
    free(fake);
     
    return;
}


G_MODULE_EXPORT void EF_gtk_noeud_edit_pos_relat(GtkCellRendererText *cell, gchar *path_string,
  gchar *new_text, Projet *projet)
/* Description : Changement de la position d'un noeud de type barre.
 * Paramètres : GtkCellRendererText *cell : cellule en cours,
 *            : gchar *path_string : path de la ligne en cours,
 *            : gchar *new_text : nouvelle valeur,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    List_Gtk_EF_Noeud       *gtk_noeud;
    GtkTreeModel            *model;
    GtkTreePath             *path;
    GtkTreeIter             iter;
    gint                    i;
    char                    *fake = (char*)malloc(sizeof(char)*(strlen(new_text)+1));
    double                  conversion;
    gint                    column = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(cell), "column"));
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_noeud.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Noeuds");
    BUGMSG(projet->ef_donnees.noeuds, , gettext("Aucun noeud n'est existant.\n"));
    BUGMSG(fake, , gettext("Erreur d'allocation mémoire.\n"));
    BUGMSG(new_text, , gettext("Paramètre %s incorrect.\n"), "new_text");
    
    gtk_noeud = &projet->list_gtk.ef_noeud;
    model = GTK_TREE_MODEL(gtk_noeud->tree_store_barre);
    path = gtk_tree_path_new_from_string(path_string);
    
    gtk_tree_model_get_iter(model, &iter, path);
    gtk_tree_path_free(path);
    gtk_tree_model_get(model, &iter, 0, &i, -1);
    
    // On vérifie si le texte contient bien un nombre flottant
    if (sscanf(new_text, "%lf%s", &conversion, fake) == 1)
    {
        EF_Noeud    *noeud;
        GList       *liste_noeuds_dep, *liste_barres_dep;
        GList       *liste_noeuds = NULL;
        
        if ((0.0 > conversion) || (conversion > 1.0))
        {
            free(fake);
            return;
        }
        
        // On modifie l'action
        BUG(noeud = EF_noeuds_cherche_numero(projet, i), );
        
        if ((noeud->type == NOEUD_BARRE) && (column == 6))
        {
            EF_Noeud_Barre  *info = (EF_Noeud_Barre *)noeud->data;
            
            info->position_relative_barre = conversion;
        }
        else
            BUGMSG(NULL, , gettext("Le type du noeud ou la colonne d'édition est incorrect.\n"));
        
        liste_noeuds = g_list_append(liste_noeuds, noeud);
        BUG(_1992_1_1_barres_cherche_dependances(projet, liste_noeuds, NULL, &liste_noeuds_dep, &liste_barres_dep), );
        g_list_free(liste_noeuds);
        
        BUG(m3d_actualise_graphique(projet, liste_noeuds_dep, liste_barres_dep), );
        BUG(m3d_rafraichit(projet), );
        
        g_list_free(liste_noeuds_dep);
        g_list_free(liste_barres_dep);
        
        // On modifie le tree-view-barre
        gtk_tree_store_set(gtk_noeud->tree_store_barre, &iter, column, conversion, -1);
    }
    
    free(fake);
     
    return;
}


void EF_gtk_render_actualise_position(GtkTreeViewColumn *tree_column __attribute__((unused)),
  GtkCellRenderer *cell, GtkTreeModel *tree_model, GtkTreeIter *iter, gpointer data)
/* Description : Personnalise l'affichage des nombres de type double dans un treeview.
 * Paramètres : GtkTreeViewColumn *tree_column : la colonne,
 *            : GtkCellRenderer *cell : la cellule,
 *            : GtkTreeModel *tree_model : le tree_model,
 *            : GtkTreeIter *iter : et le paramètre iter,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : void.
 */
{
    Projet      *projet = (Projet *)data;
    gchar       texte[30];
    gint        colonne;
    int         noeud;
    EF_Point    *point;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_noeud.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Noeuds");
    BUGMSG(projet->ef_donnees.noeuds, , gettext("Aucun noeud n'est existant.\n"));
    
    colonne = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(cell), "column"));
    gtk_tree_model_get(tree_model, iter, 0, &noeud, -1);
    BUG(point = EF_noeuds_renvoie_position(EF_noeuds_cherche_numero(projet, noeud)), );
    
    if (colonne == 1)
        common_math_double_to_char(point->x, texte, GTK_DECIMAL_DISTANCE);
    else if (colonne == 2)
        common_math_double_to_char(point->y, texte, GTK_DECIMAL_DISTANCE);
    else if (colonne == 3)
        common_math_double_to_char(point->z, texte, GTK_DECIMAL_DISTANCE);
    else
        BUGMSG(NULL, , gettext("La colonne d'où provient l'édition est incorrecte.\n"));
    
    g_object_set(GTK_CELL_RENDERER_TEXT(cell), "text", texte, NULL);
    
    free(point);
    
    return;
}


G_MODULE_EXPORT void EF_gtk_noeud_edit_noeud_appui(
  GtkCellRendererText *cell __attribute__((unused)), const gchar *path_string,
  const gchar *new_text, Projet *projet)
/* Description : Changement du type d'appui d'un noeud.
 * Paramètres : GtkCellRendererText *cell : cellule en cours,
 *            : gchar *path_string : path de la ligne en cours,
 *            : gchar *new_text : nom de l'appui,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
*/
{
    List_Gtk_EF_Noeud   *ef_gtk;
    GtkTreeModel        *model;
    GtkTreePath         *path;
    GtkTreeIter         iter;
    gint                numero_noeud;
    EF_Noeud            *noeud;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_noeud.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Noeuds");
    BUGMSG(projet->ef_donnees.noeuds, , gettext("Aucun noeud n'est existant.\n"));
    BUGMSG(new_text, , gettext("Paramètre %s incorrect.\n"), "new_text");
    
    if (projet->ef_donnees.appuis == NULL)
        return;
    
    ef_gtk = &projet->list_gtk.ef_noeud;
    
    // On récupère le model du treeview en cours d'édition.
    if (gtk_notebook_get_current_page(GTK_NOTEBOOK(ef_gtk->notebook)) == 0)
        model = GTK_TREE_MODEL(ef_gtk->tree_store_libre);
    else
        model = GTK_TREE_MODEL(ef_gtk->tree_store_barre);
    path = gtk_tree_path_new_from_string (path_string);
    
    gtk_tree_model_get_iter(model, &iter, path);
    gtk_tree_model_get(model, &iter, 0, &numero_noeud, -1);
    gtk_tree_path_free (path);
    
    BUG(noeud = EF_noeuds_cherche_numero(projet, numero_noeud), );
    
    // Si on souhaite que l'appui ne soit plus appuyé.
    if (strcmp(new_text, gettext("Aucun")) == 0)
        BUG(EF_noeuds_change_appui(projet, noeud, NULL), );
    else
    {
        EF_Appui    *appui;
        
        BUG(appui = EF_appuis_cherche_nom(projet, new_text, TRUE), );
        BUG(EF_noeuds_change_appui(projet, noeud, appui), );
    }
    
    return;
}


G_MODULE_EXPORT void EF_gtk_noeud_edit_noeud_barre_barre(
  GtkCellRendererText *cell __attribute__((unused)), const gchar *path_string,
  const gchar *new_text, Projet *projet)
/* Description : Changement de barre d'un noeud intermédiaire.
 * Paramètres : GtkCellRendererText *cell : cellule en cours,
 *            : gchar *path_string : path de la ligne en cours,
 *            : gchar *new_text : nom de l'appui,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
*/
{
    List_Gtk_EF_Noeud   *gtk_noeud;
    char                *fake = (char*)malloc(sizeof(char)*(strlen(new_text)+1));
    unsigned int        numero, conversion;
    GtkTreeModel        *model;
    GtkTreePath         *path;
    GtkTreeIter         iter;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_noeud.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Noeuds");
    BUGMSG(projet->ef_donnees.noeuds, , gettext("Aucun noeud n'est existant.\n"));
    BUGMSG(fake, , gettext("Erreur d'allocation mémoire.\n"));
    BUGMSG(new_text, , gettext("Paramètre %s incorrect.\n"), "new_text");
    
    gtk_noeud = &projet->list_gtk.ef_noeud;
    model = GTK_TREE_MODEL(gtk_noeud->tree_store_barre);
    path = gtk_tree_path_new_from_string(path_string);
    
    gtk_tree_model_get_iter(model, &iter, path);
    gtk_tree_path_free(path);
    gtk_tree_model_get(model, &iter, 0, &numero, -1);
    
    // On vérifie si le texte contient bien un numéro
    if (sscanf(new_text, "%u%s", &conversion, fake) == 1)
    {
        Beton_Barre *barre;
        EF_Noeud    *noeud;
        GList       *liste_noeuds_dep, *liste_barres_dep;
        GList       *liste_noeuds = NULL;
        
        free(fake);
        
        // On modifie l'action
        BUG(noeud = EF_noeuds_cherche_numero(projet, numero), );
        barre = _1992_1_1_barres_cherche_numero(projet, conversion);
        if (barre == NULL)
            return;
        
        if (noeud->type == NOEUD_BARRE)
        {
            EF_Noeud_Barre  *info = (EF_Noeud_Barre *)noeud->data;
            
            info->barre = barre;
        }
        else
            BUGMSG(NULL, , gettext("Le noeud doit être de type intermédiaire.\n"));
        
        liste_noeuds = g_list_append(liste_noeuds, noeud);
        BUG(_1992_1_1_barres_cherche_dependances(projet, liste_noeuds, NULL, &liste_noeuds_dep, &liste_barres_dep), );
        g_list_free(liste_noeuds);
        
        BUG(m3d_actualise_graphique(projet, liste_noeuds_dep, liste_barres_dep), );
        BUG(m3d_rafraichit(projet), );
        
        g_list_free(liste_noeuds_dep);
        g_list_free(liste_barres_dep);
        
        // On modifie le tree-view-actions
        gtk_tree_store_set(gtk_noeud->tree_store_barre, &iter, 5, conversion, -1);
    }
    else
        free(fake);
    
    return;
}


G_MODULE_EXPORT gboolean EF_gtk_noeuds_window_key_press(
  GtkWidget *widget __attribute__((unused)), GdkEvent *event, Projet *projet)
/* Description : Gestion des touches de l'ensemble des composants de la fenêtre.
 * Paramètres : GtkWidget *widget : composant à l'origine de l'évènement,
 *            : GdkEvent *event : description de la touche pressée,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : TRUE si la touche ECHAP est pressée, FALSE sinon.
 *   Echec : FALSE :
 *             projet == NULL,
 *             interface graphique non initialisée.
 */
{
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_noeud.builder, FALSE, gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Noeuds");
    
    if (event->key.keyval == GDK_KEY_Escape)
    {
        gtk_widget_destroy(projet->list_gtk.ef_noeud.window);
        return TRUE;
    }
    else
        return FALSE;
}


G_MODULE_EXPORT void EF_gtk_noeuds_window_destroy(GtkWidget *object __attribute__((unused)),
  Projet *projet)
/* Description : met projet->list_gtk.ef_noeud.builder à NULL quand la fenêtre se ferme
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_noeud.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Noeuds");
    
    projet->list_gtk.ef_noeud.builder = NULL;
    
    return;
}


G_MODULE_EXPORT void EF_gtk_noeud(Projet *projet)
/* Description : Affichage de la fenêtre permettant de créer ou modifier des noeuds.
 * Paramètres : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    List_Gtk_EF_Noeud   *ef_gtk;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_noeud.builder == NULL, , gettext("La fenêtre graphique %s est déjà initialisée.\n"), "Neouds");
    
    ef_gtk = &projet->list_gtk.ef_noeud;
    
    ef_gtk->builder = gtk_builder_new();
    BUGMSG(gtk_builder_add_from_file(ef_gtk->builder, DATADIR"/ui/EF_noeud.ui", NULL) != 0, , gettext("Builder Failed\n"));
    gtk_builder_connect_signals(ef_gtk->builder, projet);
    
    ef_gtk->window = GTK_WIDGET(gtk_builder_get_object(ef_gtk->builder, "EF_noeuds_window"));
    ef_gtk->notebook = GTK_WIDGET(gtk_builder_get_object(ef_gtk->builder, "EF_noeuds_notebook"));
    ef_gtk->tree_store_libre = GTK_TREE_STORE(gtk_builder_get_object(ef_gtk->builder, "EF_noeuds_treestore_noeuds_libres"));
    ef_gtk->tree_store_barre = GTK_TREE_STORE(gtk_builder_get_object(ef_gtk->builder, "EF_noeuds_treestore_noeuds_intermediaires"));
    
    g_object_set_data(gtk_builder_get_object(ef_gtk->builder, "EF_noeuds_treeview_noeuds_libres_cell1"), "column", GINT_TO_POINTER(1));
    gtk_tree_view_column_set_cell_data_func(GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(ef_gtk->builder, "EF_noeuds_treeview_noeuds_libres_column1")), GTK_CELL_RENDERER(gtk_builder_get_object(ef_gtk->builder, "EF_noeuds_treeview_noeuds_libres_cell1")), gtk_common_render_double, GINT_TO_POINTER(GTK_DECIMAL_DISTANCE), NULL);
    g_object_set_data(gtk_builder_get_object(ef_gtk->builder, "EF_noeuds_treeview_noeuds_libres_cell2"), "column", GINT_TO_POINTER(2));
    gtk_tree_view_column_set_cell_data_func(GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(ef_gtk->builder, "EF_noeuds_treeview_noeuds_libres_column2")), GTK_CELL_RENDERER(gtk_builder_get_object(ef_gtk->builder, "EF_noeuds_treeview_noeuds_libres_cell2")), gtk_common_render_double, GINT_TO_POINTER(GTK_DECIMAL_DISTANCE), NULL);
    g_object_set_data(gtk_builder_get_object(ef_gtk->builder, "EF_noeuds_treeview_noeuds_libres_cell3"), "column", GINT_TO_POINTER(3));
    gtk_tree_view_column_set_cell_data_func(GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(ef_gtk->builder, "EF_noeuds_treeview_noeuds_libres_column3")), GTK_CELL_RENDERER(gtk_builder_get_object(ef_gtk->builder, "EF_noeuds_treeview_noeuds_libres_cell3")), gtk_common_render_double, GINT_TO_POINTER(GTK_DECIMAL_DISTANCE), NULL);
    
    g_object_set(gtk_builder_get_object(ef_gtk->builder, "EF_noeuds_treeview_noeuds_libres_cell4"), "model", projet->list_gtk.ef_appuis.liste_appuis, NULL);
    
    g_object_set_data(gtk_builder_get_object(ef_gtk->builder, "EF_noeuds_treeview_noeuds_intermediaires_cell1"), "column", GINT_TO_POINTER(1));
    gtk_tree_view_column_set_cell_data_func(GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(ef_gtk->builder, "EF_noeuds_treeview_noeuds_intermediaires_column1")), GTK_CELL_RENDERER(gtk_builder_get_object(ef_gtk->builder, "EF_noeuds_treeview_noeuds_intermediaires_cell1")), EF_gtk_render_actualise_position, projet, NULL);
    g_object_set_data(gtk_builder_get_object(ef_gtk->builder, "EF_noeuds_treeview_noeuds_intermediaires_cell2"), "column", GINT_TO_POINTER(2));
    gtk_tree_view_column_set_cell_data_func(GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(ef_gtk->builder, "EF_noeuds_treeview_noeuds_intermediaires_column2")), GTK_CELL_RENDERER(gtk_builder_get_object(ef_gtk->builder, "EF_noeuds_treeview_noeuds_intermediaires_cell2")), EF_gtk_render_actualise_position, projet, NULL);
    g_object_set_data(gtk_builder_get_object(ef_gtk->builder, "EF_noeuds_treeview_noeuds_intermediaires_cell3"), "column", GINT_TO_POINTER(3));
    gtk_tree_view_column_set_cell_data_func(GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(ef_gtk->builder, "EF_noeuds_treeview_noeuds_intermediaires_column3")), GTK_CELL_RENDERER(gtk_builder_get_object(ef_gtk->builder, "EF_noeuds_treeview_noeuds_intermediaires_cell3")), EF_gtk_render_actualise_position, projet, NULL);
    g_object_set_data(gtk_builder_get_object(ef_gtk->builder, "EF_noeuds_treeview_noeuds_intermediaires_cell6"), "column", GINT_TO_POINTER(6));
    gtk_tree_view_column_set_cell_data_func(GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(ef_gtk->builder, "EF_noeuds_treeview_noeuds_intermediaires_column6")), GTK_CELL_RENDERER(gtk_builder_get_object(ef_gtk->builder, "EF_noeuds_treeview_noeuds_intermediaires_cell6")), gtk_common_render_double, GINT_TO_POINTER(GTK_DECIMAL_DISTANCE), NULL);
    
    g_object_set(gtk_builder_get_object(ef_gtk->builder, "EF_noeuds_treeview_noeuds_intermediaires_cell4"), "model", projet->list_gtk.ef_appuis.liste_appuis, NULL);
    
    if (projet->ef_donnees.noeuds != NULL)
    {
        GList   *list_parcours = projet->ef_donnees.noeuds;
        
        do
        {
            EF_Noeud    *noeud = (EF_Noeud *)list_parcours->data;
            EF_Point    *point = EF_noeuds_renvoie_position(noeud);
            
            BUG(point, );
            
            if (noeud->type == NOEUD_LIBRE)
            {
                gtk_tree_store_append(ef_gtk->tree_store_libre, &noeud->Iter, NULL);
                gtk_tree_store_set(ef_gtk->tree_store_libre, &noeud->Iter, 0, noeud->numero, 1, point->x, 2, point->y, 3, point->z, 4, (noeud->appui == NULL ? gettext("Aucun") : noeud->appui->nom), -1);
            }
            else if (noeud->type == NOEUD_BARRE)
            {
                EF_Noeud_Barre  *info = (EF_Noeud_Barre *)noeud->data;
                
                gtk_tree_store_append(ef_gtk->tree_store_barre, &noeud->Iter, NULL);
                gtk_tree_store_set(ef_gtk->tree_store_barre, &noeud->Iter, 0, noeud->numero, 1, point->x, 2, point->y, 3, point->z, 4, (noeud->appui == NULL ? gettext("Aucun") : noeud->appui->nom), 5, info->barre->numero, 6, info->position_relative_barre, -1);
            }
            
            free(point);
            
            list_parcours = g_list_next(list_parcours);
        }
        while (list_parcours != NULL);
    }
    
    gtk_window_set_transient_for(GTK_WINDOW(ef_gtk->window), GTK_WINDOW(projet->list_gtk.comp.window));
}

}

#endif
