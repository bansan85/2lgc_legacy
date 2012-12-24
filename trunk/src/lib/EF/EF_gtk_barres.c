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
#include "EF_noeuds.h"
#include "EF_relachement.h"
#include "EF_sections.h"
#include "1992_1_1_barres.h"
#include "1992_1_1_materiaux.h"

G_MODULE_EXPORT void EF_gtk_barres_fermer(GtkButton *button __attribute__((unused)),
  Projet *projet)
/* Description : Ferme la fenêtre.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_barres.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Barres");
    
    gtk_widget_destroy(projet->list_gtk.ef_barres.window);
    
    return;
}


G_MODULE_EXPORT void EF_gtk_barres_window_destroy(GtkWidget *object __attribute__((unused)),
  Projet *projet)
/* Description : met projet->list_gtk.ef_barres.builder à NULL quand la fenêtre se ferme,
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_barres.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Barres");
    
    g_object_unref(G_OBJECT(projet->list_gtk.ef_barres.builder));
    projet->list_gtk.ef_barres.builder = NULL;
    return;
}


G_MODULE_EXPORT gboolean EF_gtk_barres_window_key_press(
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
    BUGMSG(projet->list_gtk.ef_barres.builder, TRUE, gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Barres");
    
    if (event->key.keyval == GDK_KEY_Escape)
    {
        gtk_widget_destroy(projet->list_gtk.ef_barres.window);
        return TRUE;
    }
    else
        return FALSE;
}


G_MODULE_EXPORT void EF_gtk_barres_edit_type(GtkCellRendererText *cell __attribute__((unused)),
  const gchar *path_string, const gchar *new_text, Projet *projet)
/* Description : Changement du type d'une barre.
 * Paramètres : GtkCellRendererText *cell : cellule en cours,
 *            : gchar *path_string : path de la ligne en cours,
 *            : gchar *new_text : nom du type de barre,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
*/
{
    Gtk_EF_Barres   *ef_gtk;
    GtkTreeModel    *model;
    GtkTreeIter     iter, iter2;
    guint           numero_barre;
    Beton_Barre     *barre = NULL;
    char            *nom_type;
    gint            parcours;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_barres.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Barres");
    BUGMSG(projet->list_gtk.ef_barres.liste_types, , gettext("La liste des types de barre est indéfinie.\n"));
    
    ef_gtk = &projet->list_gtk.ef_barres;
    
    model = GTK_TREE_MODEL(gtk_builder_get_object(ef_gtk->builder, "EF_barres_treestore"));
    
    gtk_tree_model_get_iter_from_string(model, &iter, path_string);
    gtk_tree_model_get(model, &iter, 0, &numero_barre, -1);
    
    BUG(barre = _1992_1_1_barres_cherche_numero(projet, numero_barre, TRUE), );
    
    parcours = 0;
    BUGMSG(gtk_tree_model_get_iter_first(GTK_TREE_MODEL(projet->list_gtk.ef_barres.liste_types), &iter2) == TRUE, , gettext("Aucun type de barre n'est défini.\n"));
    gtk_tree_model_get(GTK_TREE_MODEL(projet->list_gtk.ef_barres.liste_types), &iter2, 0, &nom_type, -1);
    if (strcmp(nom_type, new_text) == 0)
        BUG(_1992_1_1_barres_change_type(barre, (Type_Element)0, projet), );
    else
    {
        parcours = 1;
        while (gtk_tree_model_iter_next(GTK_TREE_MODEL(projet->list_gtk.ef_barres.liste_types), &iter2) == TRUE)
        {
            gtk_tree_model_get(GTK_TREE_MODEL(projet->list_gtk.ef_barres.liste_types), &iter2, 0, &nom_type, -1);
            if (strcmp(nom_type, new_text) == 0)
            {
                BUG(_1992_1_1_barres_change_type(barre, (Type_Element)parcours, projet), );
                return;
            }
            parcours++;
        }
        // Impossible (normalement)
        BUGMSG(NULL, , gettext("Impossible de trouver le type de l'élément.\n"));
    }
    
    return;
}


G_MODULE_EXPORT void EF_gtk_barres_edit_section(
  GtkCellRendererText *cell __attribute__((unused)), const gchar *path_string,
  const gchar *new_text, Projet *projet)
/* Description : Changement la section d'une barre.
 * Paramètres : GtkCellRendererText *cell : cellule en cours,
 *            : gchar *path_string : path de la ligne en cours,
 *            : gchar *new_text : nom du type de barre,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
*/
{
    GtkTreeModel    *model;
    GtkTreeIter     iter;
    guint           numero_barre;
    Beton_Barre     *barre = NULL;
    EF_Section      *section;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_barres.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Barres");
    BUGMSG(projet->list_gtk.ef_sections.liste_sections, , gettext("La liste des sections est indéfinie.\n"));
    
    model = GTK_TREE_MODEL(gtk_builder_get_object(projet->list_gtk.ef_barres.builder, "EF_barres_treestore"));
    
    gtk_tree_model_get_iter_from_string(model, &iter, path_string);
    gtk_tree_model_get(model, &iter, 0, &numero_barre, -1);
    
    BUG(barre = _1992_1_1_barres_cherche_numero(projet, numero_barre, TRUE), );
    BUG(section = EF_sections_cherche_nom(projet, new_text, TRUE), );
    BUG(_1992_1_1_barres_change_section(barre, section, projet), );
    
    return;
}


G_MODULE_EXPORT void EF_gtk_barres_edit_materiau(
  GtkCellRendererText *cell __attribute__((unused)), const gchar *path_string,
  const gchar *new_text, Projet *projet)
/* Description : Changement la section d'une barre.
 * Paramètres : GtkCellRendererText *cell : cellule en cours,
 *            : gchar *path_string : path de la ligne en cours,
 *            : gchar *new_text : nom du type de barre,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
*/
{
    Gtk_EF_Barres   *ef_gtk;
    GtkTreeModel    *model;
    GtkTreeIter     iter;
    guint           numero_barre;
    Beton_Barre     *barre = NULL;
    Beton_Materiau  *materiau;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_barres.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Barres");
    BUGMSG(projet->list_gtk.ef_materiaux.liste_materiaux, , gettext("La liste des matériaux est indéfinie.\n"));
    
    ef_gtk = &projet->list_gtk.ef_barres;
    
    model = GTK_TREE_MODEL(gtk_builder_get_object(ef_gtk->builder, "EF_barres_treestore"));
    
    gtk_tree_model_get_iter_from_string(model, &iter, path_string);
    gtk_tree_model_get(model, &iter, 0, &numero_barre, -1);
    
    BUG(barre = _1992_1_1_barres_cherche_numero(projet, numero_barre, TRUE), );
    BUG(materiau = _1992_1_1_materiaux_cherche_nom(projet, new_text, TRUE), );
    BUG(_1992_1_1_barres_change_materiau(barre, materiau, projet), );
    
    return;
}


G_MODULE_EXPORT void EF_gtk_barres_edit_relachement(
  GtkCellRendererText *cell __attribute__((unused)), const gchar *path_string,
  const gchar *new_text, Projet *projet)
/* Description : Changement du relachement d'une barre.
 * Paramètres : GtkCellRendererText *cell : cellule en cours,
 *            : gchar *path_string : path de la ligne en cours,
 *            : gchar *new_text : nom du type de barre,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
*/
{
    Gtk_EF_Barres   *ef_gtk;
    GtkTreeModel    *model;
    GtkTreeIter     iter;
    guint           numero_barre;
    Beton_Barre     *barre = NULL;
    EF_Relachement  *relachement;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_barres.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Barres");
    BUGMSG(projet->list_gtk.ef_relachements.liste_relachements, , gettext("La liste des relâchements de barre est indéfinie.\n"));
    
    ef_gtk = &projet->list_gtk.ef_barres;
    
    model = GTK_TREE_MODEL(gtk_builder_get_object(ef_gtk->builder, "EF_barres_treestore"));
    
    gtk_tree_model_get_iter_from_string(model, &iter, path_string);
    gtk_tree_model_get(model, &iter, 0, &numero_barre, -1);
    
    BUG(barre = _1992_1_1_barres_cherche_numero(projet, numero_barre, TRUE), );
    if (strcmp(gettext("Aucun"), new_text) == 0)
        BUG(_1992_1_1_barres_change_relachement(barre, NULL, projet), );
    else
    {
        BUG(relachement = EF_relachement_cherche_nom(projet, new_text, TRUE), );
        BUG(_1992_1_1_barres_change_relachement(barre, relachement, projet), );
    }
    
    return;
}


G_MODULE_EXPORT void EF_gtk_barres_edit_noeud(GtkCellRendererText *cell __attribute__((unused)),
  gchar *path_string, gchar *new_text, Projet *projet)
/* Description : Change le noeud initial de la barre.
 * Paramètres : GtkCellRendererText *cell : cellule en cours,
 *            : gchar *path_string : path de la ligne en cours,
 *            : gchar *new_text : nouvelle valeur,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    Gtk_EF_Barres   *gtk_barre;
    GtkTreeModel    *model;
    GtkTreeIter     iter;
    unsigned int    i;
    char            *fake = (char*)malloc(sizeof(char)*(strlen(new_text)+1));
    unsigned int    conversion;
    gint            column = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(cell), "column"));
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_barres.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Barres");
    BUGMSG(fake, , gettext("Erreur d'allocation mémoire.\n"));
    
    gtk_barre = &projet->list_gtk.ef_barres;
    
    model = GTK_TREE_MODEL(gtk_builder_get_object(gtk_barre->builder, "EF_barres_treestore"));
    
    gtk_tree_model_get_iter_from_string(model, &iter, path_string);
    gtk_tree_model_get(model, &iter, 0, &i, -1);
    
    // On vérifie si le texte contient bien un nombre entier
    if (sscanf(new_text, "%d%s", &conversion, fake) == 1)
    {
        EF_Noeud    *noeud;
        Beton_Barre *barre;
        
        // On modifie l'action
        BUG(barre = _1992_1_1_barres_cherche_numero(projet, i, TRUE), );
        BUG(noeud = EF_noeuds_cherche_numero(projet, conversion, TRUE), );
        
        if (column == 4)
            BUG(_1992_1_1_barres_change_noeud(barre, noeud, TRUE, projet), );
        else
            BUG(_1992_1_1_barres_change_noeud(barre, noeud, FALSE, projet), );
    }
    
    free(fake);
     
    return;
}


G_MODULE_EXPORT void EF_gtk_barres_supprimer(GtkButton *button __attribute__((unused)),
  Projet *projet)
/* Description : Supprime la barre sélectionnées en fonction de l'onglet en cours d'affichage.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    Gtk_EF_Barres   *ef_gtk;
    GtkTreeModel    *model;
    GtkTreeIter     Iter;
    unsigned int    num;
    Beton_Barre     *barre;
    GList           *list = NULL;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_barres.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Barres");
    
    ef_gtk = &projet->list_gtk.ef_barres;
    
    if (!gtk_tree_selection_get_selected(GTK_TREE_SELECTION(gtk_builder_get_object(ef_gtk->builder, "EF_barres_treeview_select")), &model, &Iter))
        BUGMSG(NULL, , gettext("Aucune barre n'est sélectionnée.\n"));
    
    gtk_tree_model_get(model, &Iter, 0, &num, -1);
    
    BUG(barre = _1992_1_1_barres_cherche_numero(projet, num, TRUE), );
    
    list = g_list_append(list, barre);
    
    BUG(_1992_1_1_barres_supprime_liste(projet, NULL, list), );
    
    g_list_free(list);
    
    BUG(m3d_rafraichit(projet), );
    
    return;
}


G_MODULE_EXPORT gboolean EF_gtk_barres_treeview_key_press(
  GtkWidget *widget __attribute__((unused)), GdkEvent *event, Projet *projet)
/* Description : Supprime une barre sans dépendance si la touche SUPPR est appuyée.
 * Paramètres : GtkWidget *widget : composant à l'origine de l'évènement,
 *            : GdkEvent *event : Caractéristique de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : TRUE si la touche SUPPR est pressée, FALSE sinon.
 *   Echec : projet == NULL,
 *           interface graphique non initialisée.
 *  
 */
{
    BUGMSG(projet, TRUE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_barres.builder, TRUE, gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Barres");
    
    if (event->key.keyval == GDK_KEY_Delete)
    {
        GtkTreeIter     Iter;
        GtkTreeModel    *model;
        
        if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION(gtk_builder_get_object(projet->list_gtk.ef_barres.builder, "EF_barres_treeview_select")), &model, &Iter))
        {
            unsigned int    num;
            Beton_Barre     *barre;
            
            GList           *liste_barres = NULL;
            
            gtk_tree_model_get(model, &Iter, 0, &num, -1);
            
            BUG(barre = _1992_1_1_barres_cherche_numero(projet, num, TRUE), FALSE);
            
            liste_barres = g_list_append(liste_barres, barre);
            if (_1992_1_1_barres_cherche_dependances(projet, NULL, NULL, NULL, NULL, NULL, liste_barres, NULL, NULL, NULL, FALSE, FALSE) == FALSE)
            {
                BUG(_1992_1_1_barres_supprime_liste(projet, NULL, liste_barres), FALSE);
                BUG(m3d_rafraichit(projet), FALSE);
            }
            
            g_list_free(liste_barres);
        }
        return TRUE;
    }
    else
        return FALSE;
}


G_MODULE_EXPORT void EF_gtk_barres_select_changed(
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
    BUGMSG(projet->list_gtk.ef_barres.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Barres");
    
    // Si aucune barre n'est sélectionnée, il n'est pas possible d'en supprimer une.
    if (!gtk_tree_selection_get_selected(GTK_TREE_SELECTION(gtk_builder_get_object(projet->list_gtk.ef_barres.builder, "EF_barres_treeview_select")), &model, &Iter))
    {
        gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_barres.builder, "EF_barres_boutton_supprimer_direct")), FALSE);
        gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_barres.builder, "EF_barres_boutton_supprimer_menu")), FALSE);
        gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_barres.builder, "EF_barres_boutton_supprimer_direct")), FALSE);
        gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_barres.builder, "EF_barres_boutton_supprimer_menu")), TRUE);
    }
    else
    {
        unsigned int    num;
        Beton_Barre     *barre;
        GList           *liste_barres = NULL;
        
        gtk_tree_model_get(model, &Iter, 0, &num, -1);
        
        BUG(barre = _1992_1_1_barres_cherche_numero(projet, num, TRUE), );
        
        liste_barres = g_list_append(liste_barres, barre);
        if (_1992_1_1_barres_cherche_dependances(projet, NULL, NULL, NULL, NULL, NULL, liste_barres, NULL, NULL, NULL, FALSE, FALSE))
        {
            gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_barres.builder, "EF_barres_boutton_supprimer_direct")), FALSE);
            gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_barres.builder, "EF_barres_boutton_supprimer_menu")), TRUE);
            gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_barres.builder, "EF_barres_boutton_supprimer_direct")), FALSE);
            gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_barres.builder, "EF_barres_boutton_supprimer_menu")), TRUE);
        }
        else
        {
            gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_barres.builder, "EF_barres_boutton_supprimer_direct")), TRUE);
            gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_barres.builder, "EF_barres_boutton_supprimer_menu")), FALSE);
            gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_barres.builder, "EF_barres_boutton_supprimer_direct")), TRUE);
            gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_barres.builder, "EF_barres_boutton_supprimer_menu")), FALSE);
        }
        
        g_list_free(liste_barres);
    }
    
    return;
}


G_MODULE_EXPORT void EF_gtk_barres_boutton_supprimer_menu(
  GtkButton *widget __attribute__((unused)), Projet *projet)
/* Description : Affiche la liste des dépendances dans le menu lorsqu'on clique sur le bouton
 * Paramètres : GtkButton *widget : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 *   Echec : projet == NULL,
 *           interface graphique non initialisée.
 */
{
    GtkTreeModel    *model;
    GtkTreeIter     Iter;
    unsigned int    num;
    Beton_Barre     *barre;
    GList           *liste_barres = NULL, *liste_noeuds_dep, *liste_barres_dep, *liste_charges_dep;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_barres.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Barres");
    
    // Si aucune barre n'est sélectionnée, il n'est pas possible d'en supprimer une.
    if (!gtk_tree_selection_get_selected(GTK_TREE_SELECTION(gtk_builder_get_object(projet->list_gtk.ef_barres.builder, "EF_barres_treeview_select")), &model, &Iter))
        BUGMSG(NULL, , gettext("Aucun élément n'est sélectionné.\n"));
    
    gtk_tree_model_get(model, &Iter, 0, &num, -1);
    
    BUG(barre = _1992_1_1_barres_cherche_numero(projet, num, TRUE), );
    
    liste_barres = g_list_append(liste_barres, barre);
    BUG(_1992_1_1_barres_cherche_dependances(projet, NULL, NULL, NULL, NULL, NULL, liste_barres, &liste_noeuds_dep, &liste_barres_dep, &liste_charges_dep, FALSE, FALSE), );
    g_list_free(liste_barres);
    
    if ((liste_noeuds_dep != NULL) || (liste_barres_dep != NULL) || (liste_charges_dep != NULL))
    {
        char    *desc;
        
        desc = common_text_dependances(liste_noeuds_dep, liste_barres_dep, liste_charges_dep, projet);
        gtk_menu_item_set_label(GTK_MENU_ITEM(gtk_builder_get_object(projet->list_gtk.ef_barres.builder, "EF_barres_supprimer_menu_barres")), desc);
        free(desc);
    }
    else
        BUGMSG(NULL, , gettext("L'élément ne possède aucune dépendance.\n"));
    
    g_list_free(liste_noeuds_dep);
    g_list_free(liste_barres_dep);
    g_list_free(liste_charges_dep);
    
    return;
}


G_MODULE_EXPORT void EF_gtk_barres(Projet *projet)
/* Description : Création de la fenêtre permettant d'afficher les barres sous forme d'un
 *               tableau.
 * Paramètres : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 *   Echec : projet == NULL,
 *           interface graphique impossible à générer.
 */
{
    Gtk_EF_Barres   *ef_gtk;
    GList           *list_parcours;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    if (projet->list_gtk.ef_barres.builder != NULL)
    {
        gtk_window_present(GTK_WINDOW(projet->list_gtk.ef_barres.window));
        return;
    }
    
    ef_gtk = &projet->list_gtk.ef_barres;
    
    ef_gtk->builder = gtk_builder_new();
    BUGMSG(gtk_builder_add_from_file(ef_gtk->builder, DATADIR"/ui/EF_barres.ui", NULL) != 0, , gettext("Builder Failed\n"));
    gtk_builder_connect_signals(ef_gtk->builder, projet);
    
    ef_gtk->window = GTK_WIDGET(gtk_builder_get_object(ef_gtk->builder, "EF_barres_window"));;
    
    g_object_set(gtk_builder_get_object(ef_gtk->builder, "EF_barres_treeview_cell1"), "model", projet->list_gtk.ef_barres.liste_types, NULL);
    g_object_set(gtk_builder_get_object(ef_gtk->builder, "EF_barres_treeview_cell2"), "model", projet->list_gtk.ef_sections.liste_sections, NULL);
    g_object_set(gtk_builder_get_object(ef_gtk->builder, "EF_barres_treeview_cell3"), "model", projet->list_gtk.ef_materiaux.liste_materiaux, NULL);
    g_object_set(gtk_builder_get_object(ef_gtk->builder, "EF_barres_treeview_cell6"), "model", projet->list_gtk.ef_relachements.liste_relachements, NULL);
    
    list_parcours = projet->beton.barres;
    while (list_parcours != NULL)
    {
        Beton_Barre *barre = (Beton_Barre*)list_parcours->data;
        GtkTreeIter iter;
        char        *tmp;
        
        BUGMSG(tmp = g_strdup_printf("%d", (int)barre->type), , gettext("Erreur d'allocation mémoire.\n"));
        gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(projet->list_gtk.ef_barres.liste_types), &iter, tmp);
        free(tmp);
        gtk_tree_model_get(GTK_TREE_MODEL(projet->list_gtk.ef_barres.liste_types), &iter, 0, &tmp, -1);
        
        gtk_tree_store_append(GTK_TREE_STORE(gtk_builder_get_object(ef_gtk->builder, "EF_barres_treestore")), &barre->Iter, NULL);
        gtk_tree_store_set(GTK_TREE_STORE(gtk_builder_get_object(ef_gtk->builder, "EF_barres_treestore")), &barre->Iter, 0, barre->numero, 1, tmp, 2, barre->section->nom, 3, barre->materiau->nom, 4, barre->noeud_debut->numero, 5, barre->noeud_fin->numero, 6, (barre->relachement == NULL ? gettext("Aucun") : barre->relachement->nom), -1);
        
        free(tmp);
        
        list_parcours = g_list_next(list_parcours);
    }
    
    g_object_set_data(gtk_builder_get_object(ef_gtk->builder, "EF_barres_treeview_cell4"), "column", GINT_TO_POINTER(4));
    g_object_set_data(gtk_builder_get_object(ef_gtk->builder, "EF_barres_treeview_cell5"), "column", GINT_TO_POINTER(5));
    
    gtk_window_set_transient_for(GTK_WINDOW(ef_gtk->window), GTK_WINDOW(projet->list_gtk.comp.window));
}

#endif
