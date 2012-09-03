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

G_MODULE_EXPORT void EF_gtk_barres_add_annuler_clicked(
  GtkButton *button __attribute__((unused)), Projet *projet)
/* Description : Ferme la fenêtre sans effectuer les modifications.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_barres.builder_add, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Ajout Appui");
    
    gtk_widget_destroy(projet->list_gtk.ef_barres.window_add);
    
    return;
}


G_MODULE_EXPORT void EF_gtk_barres_add_add_clicked(GtkButton *button __attribute__((unused)),
  Projet *projet)
/* Description : Ajoute une nouvelle barre.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    int         type;
    char        *nom;
    void        *section;
    int         materiau;
    int         noeud_debut;
    int         noeud_fin;
    EF_Relachement* relachement;
    GtkTreeModel   *model;
    GtkTreeIter Iter;
    
    List_Gtk_EF_Barres  *ef_gtk;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_barres.builder_add, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Ajout Appui");
    
    ef_gtk = &projet->list_gtk.ef_barres;
    
    type = gtk_combo_box_get_active(GTK_COMBO_BOX(gtk_builder_get_object(ef_gtk->builder_add, "EF_gtk_barres_add_type_combobox")));
    
    model = gtk_combo_box_get_model(GTK_COMBO_BOX(gtk_builder_get_object(ef_gtk->builder_add, "EF_gtk_barres_add_section_combobox")));
    type = gtk_combo_box_get_active(GTK_COMBO_BOX(gtk_builder_get_object(ef_gtk->builder_add, "EF_gtk_barres_add_section_combobox")));
    gtk_tree_model_iter_nth_child(model, &Iter, NULL, type);
    gtk_tree_model_get(model, &Iter, 0, &nom, -1);
    BUG(section = _1992_1_1_sections_cherche_nom(projet, nom), );
    free(nom);
    
    materiau = gtk_combo_box_get_active(GTK_COMBO_BOX(gtk_builder_get_object(ef_gtk->builder_add, "EF_gtk_barres_add_materiau_combobox")));
    
    model = gtk_combo_box_get_model(GTK_COMBO_BOX(gtk_builder_get_object(ef_gtk->builder_add, "EF_gtk_barres_add_relachement_combobox")));
    type = gtk_combo_box_get_active(GTK_COMBO_BOX(gtk_builder_get_object(ef_gtk->builder_add, "EF_gtk_barres_add_relachement_combobox")));
    if (type == -1)
        return;
    else if (type == 0)
        relachement = NULL;
    else
    {
        gtk_tree_model_iter_nth_child(model, &Iter, NULL, type);
        gtk_tree_model_get(model, &Iter, 0, &nom, -1);
        BUG(relachement = EF_relachement_cherche_nom(projet, nom), );
        free(nom);
    }
    
    noeud_debut = gtk_common_entry_renvoie_int(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder_add, "EF_gtk_barres_add_noeud1_buffer")));
    noeud_fin = gtk_common_entry_renvoie_int(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder_add, "EF_gtk_barres_add_noeud2_buffer")));
    if ((EF_noeuds_cherche_numero(projet, noeud_debut) == NULL) || (EF_noeuds_cherche_numero(projet, noeud_fin) == NULL))
        return;
    
    if ((type != -1) && (materiau != -1))
    {
        BUG(_1992_1_1_barres_ajout(projet, (Type_Element)type, section, materiau, noeud_debut, noeud_fin, relachement, 0), );
        BUG(m3d_rafraichit(projet), );
    }
    
    return;
}


G_MODULE_EXPORT void EF_gtk_barres_add_window_destroy(GtkWidget *object __attribute__((unused)), Projet *projet)
/* Description : met projet->list_gtk.ef_barres.builder_add à NULL quand la fenêtre se ferme.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_barres.builder_add, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Ajout Appui");
    
    projet->list_gtk.ef_barres.builder_add = NULL;
    return;
}


G_MODULE_EXPORT gboolean EF_gtk_barres_add_window_key_press(
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
    BUGMSG(projet->list_gtk.ef_barres.builder_add, TRUE, gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Ajout Appui");
    
    if (event->key.keyval == GDK_KEY_Escape)
    {
        gtk_widget_destroy(projet->list_gtk.ef_barres.window_add);
        return TRUE;
    }
    else
        return FALSE;
}


G_MODULE_EXPORT void EF_gtk_barres_add_check_add(GtkWidget *widget, Projet *projet)
/* Description : Vérifie à chaque modification d'un champ si la fenêtre possède toutes les
 *               informations correctes pour créer une barre et active / désactive en fonction
 *               le bouton ajouter.
 * Paramètres : GtkWidget *widget : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    List_Gtk_EF_Barres  *ef_gtk;
    gboolean            ok = FALSE;
    EF_Noeud            *noeud1, *noeud2;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_barres.builder_add, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Ajout Appui");
    
    ef_gtk = &projet->list_gtk.ef_barres;
    
    noeud1 = EF_noeuds_cherche_numero(projet, gtk_common_entry_renvoie_int(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder_add, "EF_gtk_barres_add_noeud1_buffer")))); 
    noeud2 = EF_noeuds_cherche_numero(projet, gtk_common_entry_renvoie_int(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder_add, "EF_gtk_barres_add_noeud2_buffer")))); 

    if (
        (gtk_combo_box_get_active(GTK_COMBO_BOX(gtk_builder_get_object(ef_gtk->builder_add, "EF_gtk_barres_add_type_combobox"))) != -1) &&
        (gtk_combo_box_get_active(GTK_COMBO_BOX(gtk_builder_get_object(ef_gtk->builder_add, "EF_gtk_barres_add_section_combobox"))) != -1) &&
        (gtk_combo_box_get_active(GTK_COMBO_BOX(gtk_builder_get_object(ef_gtk->builder_add, "EF_gtk_barres_add_materiau_combobox"))) != -1) &&
        (gtk_combo_box_get_active(GTK_COMBO_BOX(gtk_builder_get_object(ef_gtk->builder_add, "EF_gtk_barres_add_relachement_combobox"))) != -1) &&
        (noeud1 != NULL) && (noeud2 != NULL) && (noeud1 != noeud2)
    )
        ok = TRUE;
    
    if (GTK_IS_TEXT_BUFFER(widget))
        gtk_common_entry_check_int(GTK_TEXT_BUFFER(widget), NULL);
    
    gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(ef_gtk->builder_add, "EF_gtk_barres_add_button_add")), ok);
    
    return;
}


G_MODULE_EXPORT void EF_gtk_barres_ajouter(GtkButton *button __attribute__((unused)),
  Projet *projet)
/* Description : Création de la fenêtre permettant d'ajouter des barres.
 * Paramètres : GtkWidget *widget : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    List_Gtk_EF_Barres  *ef_gtk;
    char                *nb_barres;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_barres.builder_add == NULL, , gettext("La fenêtre graphique %s est déjà initialisée.\n"), "Ajout Appui");
    
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
    
    BUGMSG(nb_barres = g_strdup_printf("%d", g_list_length(projet->beton.barres)), , gettext("Erreur d'allocation mémoire.\n"));
    gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(ef_gtk->builder_add, "EF_gtk_barres_add_numero_label2")), nb_barres);
    free(nb_barres);
    
    gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(ef_gtk->builder_add, "EF_gtk_barres_add_button_add")), FALSE);
    
    gtk_window_set_transient_for(GTK_WINDOW(ef_gtk->window_add), GTK_WINDOW(projet->list_gtk.comp.window));
}


}

#endif
