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
#include "EF_noeud.h"
#include "EF_relachement.h"
#include "EF_charge_noeud.h"
#include "1990_actions.h"
#include "1992_1_1_barres.h"
#include "1992_1_1_section.h"
#include "1992_1_1_materiaux.h"

G_MODULE_EXPORT void EF_gtk_barres_add_annuler_clicked(GtkButton *button __attribute__((unused)), Projet *projet)
/* Description : Ferme la fenêtre sans effectuer les modifications
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    BUGMSG(projet, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk.ef_barres.builder_add, , gettext("Paramètre incorrect\n"));
    
    gtk_widget_destroy(projet->list_gtk.ef_barres.window_add);
    
    return;
}


G_MODULE_EXPORT void EF_gtk_barres_add_add_clicked(GtkButton *button __attribute__((unused)), Projet *projet)
/* Description : Ajoute une nouvelle barre
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    int         type;
    int         section;
    int         materiau;
    int         noeud_debut;
    int         noeud_fin;
    EF_Relachement* relachement;
    
    List_Gtk_EF_Barres  *ef_gtk;
    
    BUGMSG(projet, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk.ef_barres.builder_add, , gettext("Paramètre incorrect\n"));
    
    ef_gtk = &projet->list_gtk.ef_barres;
    
    type = gtk_combo_box_get_active(GTK_COMBO_BOX(gtk_builder_get_object(ef_gtk->builder_add, "EF_gtk_barres_add_type_combobox")));
    section = gtk_combo_box_get_active(GTK_COMBO_BOX(gtk_builder_get_object(ef_gtk->builder_add, "EF_gtk_barres_add_section_combobox")));
    materiau = gtk_combo_box_get_active(GTK_COMBO_BOX(gtk_builder_get_object(ef_gtk->builder_add, "EF_gtk_barres_add_materiau_combobox")));
    relachement = EF_relachement_cherche_numero(projet, gtk_combo_box_get_active(GTK_COMBO_BOX(gtk_builder_get_object(ef_gtk->builder_add, "EF_gtk_barres_add_relachement_combobox")))-1);
    noeud_debut = gtk_common_entry_renvoie_int(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder_add, "EF_gtk_barres_add_noeud1_buffer")));
    noeud_fin = gtk_common_entry_renvoie_int(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder_add, "EF_gtk_barres_add_noeud2_buffer")));
    if ((EF_noeuds_cherche_numero(projet, noeud_debut) == NULL) || (EF_noeuds_cherche_numero(projet, noeud_fin) == NULL))
        return;
    
    if ((type != -1) && (section != -1) && (materiau != -1))
        BUG(_1992_1_1_barres_ajout(projet, (Type_Element)type, section, materiau, noeud_debut, noeud_fin, relachement, 0) == 0, );
    
    return;
}


G_MODULE_EXPORT void EF_gtk_barres_fermer(GtkButton *button __attribute__((unused)), Projet *projet)
/* Description : Ferme la fenêtre sans effectuer les modifications
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    BUGMSG(projet, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk.ef_barres.builder, , gettext("Paramètre incorrect\n"));
    
    gtk_widget_destroy(projet->list_gtk.ef_barres.window);
    
    return;
}


G_MODULE_EXPORT void EF_gtk_barres_add_window_destroy(GtkWidget *object __attribute__((unused)), Projet *projet)
/* Description : met projet->list_gtk._1990_groupes.window à NULL quand la fenêtre se ferme
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    BUGMSG(projet, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk.ef_barres.builder_add, , gettext("Paramètre incorrect\n"));
    
    projet->list_gtk.ef_barres.builder_add = NULL;
    return;
}


G_MODULE_EXPORT void EF_gtk_barres_window_destroy(GtkWidget *object __attribute__((unused)), Projet *projet)
/* Description : met projet->list_gtk._1990_groupes.window à NULL quand la fenêtre se ferme
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    BUGMSG(projet, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk.ef_barres.builder, , gettext("Paramètre incorrect\n"));
    
    projet->list_gtk.ef_barres.builder = NULL;
    return;
}


G_MODULE_EXPORT gboolean EF_gtk_barres_add_window_key_press(GtkWidget *widget __attribute__((unused)), GdkEvent *event, Projet *projet)
{
    BUGMSG(projet, TRUE, gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk.ef_barres.builder_add, TRUE, gettext("Paramètre incorrect\n"));
    
    if (event->key.keyval == GDK_KEY_Escape)
    {
        gtk_widget_destroy(projet->list_gtk.ef_barres.window_add);
        return TRUE;
    }
    else
        return FALSE;
}


G_MODULE_EXPORT gboolean EF_gtk_barres_window_key_press(GtkWidget *widget __attribute__((unused)), GdkEvent *event, Projet *projet)
{
    BUGMSG(projet, TRUE, gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk.ef_barres.builder, TRUE, gettext("Paramètre incorrect\n"));
    
    if (event->key.keyval == GDK_KEY_Escape)
    {
        gtk_widget_destroy(projet->list_gtk.ef_barres.window);
        return TRUE;
    }
    else
        return FALSE;
}


G_MODULE_EXPORT void EF_gtk_barres_edit_type(GtkCellRendererText *cell __attribute__((unused)), const gchar *path_string, const gchar *new_text, Projet *projet)
/* Description : Changement du type d'une barre.
 * Paramètres : GtkCellRendererText *cell : cellule en cours,
 *            : gchar *path_string : path de la ligne en cours,
 *            : gchar *new_text : nom du type de barre,
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
*/
{
    List_Gtk_EF_Barres  *ef_gtk;
    GtkTreeModel        *model;
    GtkTreePath         *path;
    GtkTreeIter         iter, iter2;
    guint               numero_barre;
    Beton_Barre         *barre = NULL;
    char                *nom_type;
    gint                parcours;
    
    BUGMSG(projet, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk.ef_barres.builder, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk.ef_barres.liste_types, , gettext("Paramètre incorrect\n"));
    
    ef_gtk = &projet->list_gtk.ef_barres;
    
    model = GTK_TREE_MODEL(gtk_builder_get_object(ef_gtk->builder, "EF_barres_treestore"));
    path = gtk_tree_path_new_from_string(path_string);
    
    gtk_tree_model_get_iter(model, &iter, path);
    gtk_tree_model_get(model, &iter, 0, &numero_barre, -1);
    
    BUG(barre = _1992_1_1_barres_cherche_numero(projet, numero_barre), );
    
    parcours = 0;
    BUGMSG(gtk_tree_model_get_iter_first(GTK_TREE_MODEL(projet->list_gtk.ef_barres.liste_types), &iter2) == TRUE, , gettext("Paramètre incorrect\n"));
    gtk_tree_model_get(GTK_TREE_MODEL(projet->list_gtk.ef_barres.liste_types), &iter2, 0, &nom_type, -1);
    if (strcmp(nom_type, new_text) == 0)
    {
        barre->type = (Type_Element)0;
        gtk_tree_store_set(GTK_TREE_STORE(model), &iter, 1, new_text, -1);
    }
    else
    {
        parcours = 1;
        while (gtk_tree_model_iter_next(GTK_TREE_MODEL(projet->list_gtk.ef_barres.liste_types), &iter2) == TRUE)
        {
            gtk_tree_model_get(GTK_TREE_MODEL(projet->list_gtk.ef_barres.liste_types), &iter2, 0, &nom_type, -1);
            if (strcmp(nom_type, new_text) == 0)
            {
                barre->type = (Type_Element)parcours;
                gtk_tree_store_set(GTK_TREE_STORE(model), &iter, 1, new_text, -1);
                return;
            }
            parcours++;
        }
        BUGMSG(NULL, , gettext("Impossible de trouver le type de l'élément.\n"));
    }
    
    return;
}


G_MODULE_EXPORT void EF_gtk_barres_edit_section(GtkCellRendererText *cell __attribute__((unused)), const gchar *path_string, const gchar *new_text, Projet *projet)
/* Description : Changement la section d'une barre.
 * Paramètres : GtkCellRendererText *cell : cellule en cours,
 *            : gchar *path_string : path de la ligne en cours,
 *            : gchar *new_text : nom du type de barre,
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
*/
{
    List_Gtk_EF_Barres  *ef_gtk;
    GtkTreeModel        *model;
    GtkTreePath         *path;
    GtkTreeIter         iter;
    guint               numero_barre;
    Beton_Barre         *barre = NULL;
    void                *section;
    
    BUGMSG(projet, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk.ef_barres.builder, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk.ef_barres.liste_sections, , gettext("Paramètre incorrect\n"));
    
    ef_gtk = &projet->list_gtk.ef_barres;
    
    model = GTK_TREE_MODEL(gtk_builder_get_object(ef_gtk->builder, "EF_barres_treestore"));
    path = gtk_tree_path_new_from_string(path_string);
    
    gtk_tree_model_get_iter(model, &iter, path);
    gtk_tree_model_get(model, &iter, 0, &numero_barre, -1);
    
    BUG(barre = _1992_1_1_barres_cherche_numero(projet, numero_barre), );
    BUG(section = _1992_1_1_sections_cherche_nom(projet, new_text), );
    barre->section = section;
    
    gtk_tree_store_set(GTK_TREE_STORE(model), &iter, 2, new_text, -1);
    
    return;
}


G_MODULE_EXPORT void EF_gtk_barres_edit_materiau(GtkCellRendererText *cell __attribute__((unused)), const gchar *path_string, const gchar *new_text, Projet *projet)
/* Description : Changement la section d'une barre.
 * Paramètres : GtkCellRendererText *cell : cellule en cours,
 *            : gchar *path_string : path de la ligne en cours,
 *            : gchar *new_text : nom du type de barre,
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
*/
{
    List_Gtk_EF_Barres  *ef_gtk;
    GtkTreeModel        *model;
    GtkTreePath         *path;
    GtkTreeIter         iter;
    guint               numero_barre;
    Beton_Barre         *barre = NULL;
    Beton_Materiau      *materiau;
    
    BUGMSG(projet, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk.ef_barres.builder, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk.ef_barres.liste_materiaux, , gettext("Paramètre incorrect\n"));
    
    ef_gtk = &projet->list_gtk.ef_barres;
    
    model = GTK_TREE_MODEL(gtk_builder_get_object(ef_gtk->builder, "EF_barres_treestore"));
    path = gtk_tree_path_new_from_string(path_string);
    
    gtk_tree_model_get_iter(model, &iter, path);
    gtk_tree_model_get(model, &iter, 0, &numero_barre, -1);
    
    BUG(barre = _1992_1_1_barres_cherche_numero(projet, numero_barre), );
    BUG(materiau = _1992_1_1_materiaux_cherche_nom(projet, new_text), );
    barre->materiau = materiau;
    
    gtk_tree_store_set(GTK_TREE_STORE(model), &iter, 3, new_text, -1);
    
    return;
}


G_MODULE_EXPORT void EF_gtk_barres_edit_relachement(GtkCellRendererText *cell __attribute__((unused)), const gchar *path_string, const gchar *new_text, Projet *projet)
/* Description : Changement du relachement d'une barre.
 * Paramètres : GtkCellRendererText *cell : cellule en cours,
 *            : gchar *path_string : path de la ligne en cours,
 *            : gchar *new_text : nom du type de barre,
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
*/
{
    List_Gtk_EF_Barres  *ef_gtk;
    GtkTreeModel        *model;
    GtkTreePath         *path;
    GtkTreeIter         iter;
    guint               numero_barre;
    Beton_Barre         *barre = NULL;
    EF_Relachement      *relachement;
    
    BUGMSG(projet, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk.ef_barres.builder, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk.ef_barres.liste_relachements, , gettext("Paramètre incorrect\n"));
    
    ef_gtk = &projet->list_gtk.ef_barres;
    
    model = GTK_TREE_MODEL(gtk_builder_get_object(ef_gtk->builder, "EF_barres_treestore"));
    path = gtk_tree_path_new_from_string(path_string);
    
    gtk_tree_model_get_iter(model, &iter, path);
    gtk_tree_model_get(model, &iter, 0, &numero_barre, -1);
    
    BUG(barre = _1992_1_1_barres_cherche_numero(projet, numero_barre), );
    if (strcmp(gettext("Aucun"), new_text) == 0)
        barre->relachement = NULL;
    else
    {
        BUG(relachement = EF_relachement_cherche_nom(projet, new_text), );
        barre->relachement = relachement;
    }
    
    gtk_tree_store_set(GTK_TREE_STORE(model), &iter, 6, new_text, -1);
    
    return;
}


G_MODULE_EXPORT void EF_gtk_barres_edit_noeud(GtkCellRendererText *cell __attribute__((unused)), gchar *path_string, gchar *new_text, Projet *projet)
/* Description : Change le noeud initial de la barre.
 * Paramètres : GtkCellRendererText *cell : cellule en cours,
 *            : gchar *path_string : path de la ligne en cours,
 *            : gchar *new_text : nouvelle valeur,
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    List_Gtk_EF_Barres      *gtk_barre;
    GtkTreeModel            *model;
    GtkTreePath             *path;
    GtkTreeIter             iter;
    gint                    i;
    char                    *fake = (char*)malloc(sizeof(char)*(strlen(new_text)+1));
    unsigned int            conversion;
    gint                    column = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(cell), "column"));
    
    BUGMSG(projet, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->ef_donnees.noeuds, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk.ef_barres.builder, , gettext("Paramètre incorrect\n"));
    BUGMSG(fake, , gettext("Erreur d'allocation mémoire.\n"));
    
    gtk_barre = &projet->list_gtk.ef_barres;
    
    model = GTK_TREE_MODEL(gtk_builder_get_object(gtk_barre->builder, "EF_barres_treestore"));
    path = gtk_tree_path_new_from_string(path_string);
    
    gtk_tree_model_get_iter(model, &iter, path);
    gtk_tree_path_free(path);
    gtk_tree_model_get(model, &iter, 0, &i, -1);
    
    // On vérifie si le texte contient bien un nombre flottant
    if (sscanf(new_text, "%d%s", &conversion, fake) == 1)
    {
        EF_Noeud    *noeud;
        Beton_Barre *barre;
        
        // On modifie l'action
        BUG(barre = _1992_1_1_barres_cherche_numero(projet, i), );
        noeud = EF_noeuds_cherche_numero(projet, conversion);
        
        if (noeud != NULL)
        {
            if (column == 4)
                barre->noeud_debut = noeud;
            else
                barre->noeud_fin = noeud;
            
            // On modifie le tree-view-barre
            gtk_tree_store_set(GTK_TREE_STORE(model), &iter, column, conversion, -1);
        }
    }
    
    free(fake);
     
    return;
}


G_MODULE_EXPORT void EF_gtk_barres_add_check_add(GtkWidget *widget, Projet *projet)
{
    List_Gtk_EF_Barres  *ef_gtk;
    gboolean            ok = FALSE;
    
    BUGMSG(projet, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk.ef_barres.builder_add, , gettext("Paramètre incorrect\n"));
    
    ef_gtk = &projet->list_gtk.ef_barres;
    
    if (
        (gtk_combo_box_get_active(GTK_COMBO_BOX(gtk_builder_get_object(ef_gtk->builder_add, "EF_gtk_barres_add_type_combobox"))) != -1) &&
        (gtk_combo_box_get_active(GTK_COMBO_BOX(gtk_builder_get_object(ef_gtk->builder_add, "EF_gtk_barres_add_section_combobox"))) != -1) &&
        (gtk_combo_box_get_active(GTK_COMBO_BOX(gtk_builder_get_object(ef_gtk->builder_add, "EF_gtk_barres_add_materiau_combobox"))) != -1) &&
        (gtk_combo_box_get_active(GTK_COMBO_BOX(gtk_builder_get_object(ef_gtk->builder_add, "EF_gtk_barres_add_relachement_combobox"))) != -1) &&
        (EF_noeuds_cherche_numero(projet, gtk_common_entry_renvoie_int(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder_add, "EF_gtk_barres_add_noeud1_buffer")))) != NULL) &&
        (EF_noeuds_cherche_numero(projet, gtk_common_entry_renvoie_int(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder_add, "EF_gtk_barres_add_noeud2_buffer")))) != NULL)
    )
        ok = TRUE;
    
    if (GTK_IS_TEXT_BUFFER(widget))
        gtk_common_entry_check_int(GTK_TEXT_BUFFER(widget), NULL);
    
    gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(ef_gtk->builder_add, "EF_gtk_barres_add_button_add")), ok);
    
    return;
}


G_MODULE_EXPORT void EF_gtk_barres_ajouter(GtkButton *button __attribute__((unused)), Projet *projet)
{
    List_Gtk_EF_Barres  *ef_gtk;
    char                *nb_barres;
    
    BUGMSG(projet, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk.ef_barres.builder_add == NULL, , gettext("Paramètre incorrect\n"));
    
    ef_gtk = &projet->list_gtk.ef_barres;
    
    ef_gtk->builder_add = gtk_builder_new();
    BUGMSG(gtk_builder_add_from_file(ef_gtk->builder_add, DATADIR"/ui/EF_gtk_barres_add.ui", NULL) != 0, , gettext("Builder Failed\n"));
    gtk_builder_connect_signals(ef_gtk->builder_add, projet);
    
    ef_gtk->window_add = GTK_WIDGET(gtk_builder_get_object(ef_gtk->builder_add, "EF_barres_add_window"));;
    
    g_object_set(gtk_builder_get_object(ef_gtk->builder_add, "EF_gtk_barres_add_type_combobox"), "model", projet->list_gtk.ef_barres.liste_types, NULL);
    g_object_set(gtk_builder_get_object(ef_gtk->builder_add, "EF_gtk_barres_add_section_combobox"), "model", projet->list_gtk.ef_barres.liste_sections, NULL);
    g_object_set(gtk_builder_get_object(ef_gtk->builder_add, "EF_gtk_barres_add_materiau_combobox"), "model", projet->list_gtk.ef_barres.liste_materiaux, NULL);
    g_object_set(gtk_builder_get_object(ef_gtk->builder_add, "EF_gtk_barres_add_relachement_combobox"), "model", projet->list_gtk.ef_barres.liste_relachements, NULL);
    
    // Pour résoudre un bug dans l'affichage graphique.
    // En effet, sans ces lignes, la hauteur et largeur des ComboBox est au minimum et lors du passage de la souris au dessus, les composants prennent leur bonne taille.
    // https://bugzilla.gnome.org/show_bug.cgi?id=675228
    gtk_combo_box_set_active(GTK_COMBO_BOX(gtk_builder_get_object(ef_gtk->builder_add, "EF_gtk_barres_add_type_combobox")), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(gtk_builder_get_object(ef_gtk->builder_add, "EF_gtk_barres_add_section_combobox")), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(gtk_builder_get_object(ef_gtk->builder_add, "EF_gtk_barres_add_materiau_combobox")), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(gtk_builder_get_object(ef_gtk->builder_add, "EF_gtk_barres_add_relachement_combobox")), -1);
    
    nb_barres = g_strdup_printf("%d", g_list_length(projet->beton.barres));
    gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(ef_gtk->builder_add, "EF_gtk_barres_add_numero_label2")), nb_barres);
    free(nb_barres);
    
    gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(ef_gtk->builder_add, "EF_gtk_barres_add_button_add")), FALSE);
    
    gtk_window_set_transient_for(GTK_WINDOW(ef_gtk->window_add), GTK_WINDOW(projet->list_gtk.comp.window));
}


G_MODULE_EXPORT void EF_gtk_barres(Projet *projet)
{
    List_Gtk_EF_Barres  *ef_gtk;
    
    BUGMSG(projet, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk.ef_barres.builder == NULL, , gettext("Paramètre incorrect\n"));
    
    ef_gtk = &projet->list_gtk.ef_barres;
    
    ef_gtk->builder = gtk_builder_new();
    BUGMSG(gtk_builder_add_from_file(ef_gtk->builder, DATADIR"/ui/EF_gtk_barres.ui", NULL) != 0, , gettext("Builder Failed\n"));
    gtk_builder_connect_signals(ef_gtk->builder, projet);
    
    ef_gtk->window = GTK_WIDGET(gtk_builder_get_object(ef_gtk->builder, "EF_barres_window"));;
    
    g_object_set(gtk_builder_get_object(ef_gtk->builder, "EF_barres_treeview_cell1"), "model", projet->list_gtk.ef_barres.liste_types, NULL);
    g_object_set(gtk_builder_get_object(ef_gtk->builder, "EF_barres_treeview_cell2"), "model", projet->list_gtk.ef_barres.liste_sections, NULL);
    g_object_set(gtk_builder_get_object(ef_gtk->builder, "EF_barres_treeview_cell3"), "model", projet->list_gtk.ef_barres.liste_materiaux, NULL);
    g_object_set(gtk_builder_get_object(ef_gtk->builder, "EF_barres_treeview_cell6"), "model", projet->list_gtk.ef_barres.liste_relachements, NULL);
    
    if (projet->beton.barres != NULL)
    {
        GList   *list_parcours = projet->beton.barres;
        
        do
        {
            Beton_Barre *barre = (Beton_Barre*)list_parcours->data;
            GtkTreeIter iter;
            char        *tmp;
            Beton_Section_Rectangulaire *section;
            
            tmp = g_strdup_printf("%d", (int)barre->type);
            gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(projet->list_gtk.ef_barres.liste_types), &iter, tmp);
            free(tmp);
            gtk_tree_model_get(GTK_TREE_MODEL(projet->list_gtk.ef_barres.liste_types), &iter, 0, &tmp, -1);
            
            section = (Beton_Section_Rectangulaire*)barre->section;
            
            gtk_tree_store_append(GTK_TREE_STORE(gtk_builder_get_object(ef_gtk->builder, "EF_barres_treestore")), &iter, NULL);
            gtk_tree_store_set(GTK_TREE_STORE(gtk_builder_get_object(ef_gtk->builder, "EF_barres_treestore")), &iter, 0, barre->numero, 1, tmp, 2, section->nom, 3, barre->materiau->nom, 4, barre->noeud_debut->numero, 5, barre->noeud_fin->numero, 6, (barre->relachement == NULL ? gettext("Aucun") : barre->relachement->nom), -1);
            
            free(tmp);
            
            list_parcours = g_list_next(list_parcours);
        }
        while (list_parcours != NULL);
    }
    
    g_object_set_data(gtk_builder_get_object(ef_gtk->builder, "EF_barres_treeview_cell4"), "column", GINT_TO_POINTER(4));
    g_object_set_data(gtk_builder_get_object(ef_gtk->builder, "EF_barres_treeview_cell5"), "column", GINT_TO_POINTER(5));
    
    gtk_window_set_transient_for(GTK_WINDOW(ef_gtk->window), GTK_WINDOW(projet->list_gtk.comp.window));
}

}

#endif