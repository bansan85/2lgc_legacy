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

#include "common_m3d.hpp"

#include "common_projet.h"
#include "common_erreurs.h"
#include "common_gtk.h"
#include "common_selection.h"
#include "EF_noeuds.h"
#include "EF_relachement.h"
#include "EF_sections.h"
#include "1992_1_1_barres.h"
#include "1992_1_1_materiaux.h"

void EF_gtk_barres_add_annuler_clicked(GtkButton *button, Projet *projet)
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


void EF_gtk_barres_add_add_clicked(GtkButton *button, Projet *projet)
/* Description : Ajoute une nouvelle barre.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    int             type;
    char            *nom;
    EF_Section      *section;
    EF_Materiau     *materiau;
    unsigned int    noeud_debut;
    unsigned int    noeud_fin;
    EF_Relachement  *relachement;
    unsigned int    nb_noeuds;
    GtkTreeModel    *model;
    GtkTreeIter     Iter;
    
    Gtk_EF_Barres   *ef_gtk;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_barres.builder_add, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Ajout Appui");
    
    ef_gtk = &projet->list_gtk.ef_barres;
    
    model = gtk_combo_box_get_model(GTK_COMBO_BOX(gtk_builder_get_object(ef_gtk->builder_add, "EF_gtk_barres_add_section_combobox")));
    type = gtk_combo_box_get_active(GTK_COMBO_BOX(gtk_builder_get_object(ef_gtk->builder_add, "EF_gtk_barres_add_section_combobox")));
    if (type == -1)
        return;
    gtk_tree_model_iter_nth_child(model, &Iter, NULL, type);
    gtk_tree_model_get(model, &Iter, 0, &nom, -1);
    BUG(section = EF_sections_cherche_nom(projet, nom, TRUE), );
    free(nom);
    
    model = gtk_combo_box_get_model(GTK_COMBO_BOX(gtk_builder_get_object(ef_gtk->builder_add, "EF_gtk_barres_add_materiau_combobox")));
    type = gtk_combo_box_get_active(GTK_COMBO_BOX(gtk_builder_get_object(ef_gtk->builder_add, "EF_gtk_barres_add_materiau_combobox")));
    if (type == -1)
        return;
    gtk_tree_model_iter_nth_child(model, &Iter, NULL, type);
    gtk_tree_model_get(model, &Iter, 0, &nom, -1);
    BUG(materiau = _1992_1_1_materiaux_cherche_nom(projet, nom, TRUE), );
    free(nom);
    
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
        BUG(relachement = EF_relachement_cherche_nom(projet, nom, TRUE), );
        free(nom);
    }
    
    noeud_debut = common_gtk_text_buffer_uint(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder_add, "EF_gtk_barres_add_noeud1_buffer")), 0, TRUE, UINT_MAX, FALSE);
    noeud_fin = common_gtk_text_buffer_uint(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder_add, "EF_gtk_barres_add_noeud2_buffer")), 0, TRUE, UINT_MAX, FALSE);
    if ((EF_noeuds_cherche_numero(projet, noeud_debut, TRUE) == NULL) || (EF_noeuds_cherche_numero(projet, noeud_fin, TRUE) == NULL))
        return;
    
    type = gtk_combo_box_get_active(GTK_COMBO_BOX(gtk_builder_get_object(ef_gtk->builder_add, "EF_gtk_barres_add_type_combobox")));
    
    nb_noeuds = common_gtk_text_buffer_uint(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder_add, "EF_gtk_barres_add_nb_noeuds_intermediaire_buffer")), 0, TRUE, UINT_MAX, FALSE);
    BUG(nb_noeuds != UINT_MAX, );
    
    BUG(_1992_1_1_barres_ajout(projet, (Type_Element)type, section, materiau, noeud_debut, noeud_fin, relachement, nb_noeuds), );
    BUG(m3d_rafraichit(projet), );
    
    return;
}


void EF_gtk_barres_add_window_destroy(GtkWidget *object, Projet *projet)
/* Description : met projet->list_gtk.ef_barres.builder_add à NULL quand la fenêtre se ferme.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_barres.builder_add, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Ajout Appui");
    
    g_object_unref(G_OBJECT(projet->list_gtk.ef_barres.builder_add));
    projet->list_gtk.ef_barres.builder_add = NULL;
    return;
}


gboolean EF_gtk_barres_add_window_key_press(GtkWidget *widget, GdkEvent *event, Projet *projet)
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


void EF_gtk_barres_add_check_add(GtkWidget *widget, Projet *projet)
/* Description : Vérifie à chaque modification d'un champ si la fenêtre possède toutes les
 *               informations correctes pour créer une barre et active / désactive en fonction
 *               le bouton ajouter.
 * Paramètres : GtkWidget *widget : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    Gtk_EF_Barres   *ef_gtk;
    gboolean        ok = FALSE;
    EF_Noeud        *noeud1, *noeud2;
    GtkTextIter     start, end;
    GtkTextBuffer   *buff;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_barres.builder_add, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Ajout Appui");
    
    ef_gtk = &projet->list_gtk.ef_barres;
    
    noeud1 = EF_noeuds_cherche_numero(projet, common_gtk_text_buffer_uint(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder_add, "EF_gtk_barres_add_noeud1_buffer")), 0, TRUE, UINT_MAX, FALSE), FALSE);
    buff = GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder_add, "EF_gtk_barres_add_noeud1_buffer"));
    gtk_text_buffer_get_iter_at_offset(buff, &start, 0);
    gtk_text_buffer_get_iter_at_offset(buff, &end, -1);
    gtk_text_buffer_remove_all_tags(buff, &start, &end);
    if (noeud1 == NULL)
        gtk_text_buffer_apply_tag_by_name(buff, "mauvais", &start, &end);
    else
        gtk_text_buffer_apply_tag_by_name(buff, "OK", &start, &end);
    noeud2 = EF_noeuds_cherche_numero(projet, common_gtk_text_buffer_uint(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder_add, "EF_gtk_barres_add_noeud2_buffer")), 0, TRUE, UINT_MAX, FALSE), FALSE);
    buff = GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder_add, "EF_gtk_barres_add_noeud2_buffer"));
    gtk_text_buffer_get_iter_at_offset(buff, &start, 0);
    gtk_text_buffer_get_iter_at_offset(buff, &end, -1);
    gtk_text_buffer_remove_all_tags(buff, &start, &end);
    if (noeud2 == NULL)
        gtk_text_buffer_apply_tag_by_name(buff, "mauvais", &start, &end);
    else
        gtk_text_buffer_apply_tag_by_name(buff, "OK", &start, &end);
    
    if ((gtk_combo_box_get_active(GTK_COMBO_BOX(gtk_builder_get_object(ef_gtk->builder_add, "EF_gtk_barres_add_type_combobox"))) != -1) &&
      (gtk_combo_box_get_active(GTK_COMBO_BOX(gtk_builder_get_object(ef_gtk->builder_add, "EF_gtk_barres_add_section_combobox"))) != -1) &&
      (gtk_combo_box_get_active(GTK_COMBO_BOX(gtk_builder_get_object(ef_gtk->builder_add, "EF_gtk_barres_add_materiau_combobox"))) != -1) &&
      (gtk_combo_box_get_active(GTK_COMBO_BOX(gtk_builder_get_object(ef_gtk->builder_add, "EF_gtk_barres_add_relachement_combobox"))) != -1) &&
      (noeud1 != NULL) && (noeud2 != NULL) && (noeud1 != noeud2))
        ok = TRUE;
    
    if (common_gtk_text_buffer_uint(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder_add, "EF_gtk_barres_add_nb_noeuds_intermediaire_buffer")), 0, TRUE, UINT_MAX, FALSE) == UINT_MAX)
        ok = FALSE;
    
    gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(ef_gtk->builder_add, "EF_gtk_barres_add_button_add")), ok);
    
    return;
}


void EF_gtk_barres_ajouter(GtkButton *button, Projet *projet)
/* Description : Création de la fenêtre permettant d'ajouter des barres.
 * Paramètres : GtkWidget *widget : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    Gtk_EF_Barres   *ef_gtk;
    char            *nb_barres;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    if (projet->list_gtk.ef_barres.builder_add != NULL)
    {
        gtk_window_present(GTK_WINDOW(projet->list_gtk.ef_barres.window_add));
        return;
    }
    
    ef_gtk = &projet->list_gtk.ef_barres;
    
    ef_gtk->builder_add = gtk_builder_new();
    BUGMSG(gtk_builder_add_from_file(ef_gtk->builder_add, DATADIR"/ui/EF_barres_add.ui", NULL) != 0, , gettext("Builder Failed\n"));
    gtk_builder_connect_signals(ef_gtk->builder_add, projet);
    
    ef_gtk->window_add = GTK_WIDGET(gtk_builder_get_object(ef_gtk->builder_add, "EF_barres_add_window"));
    
    g_object_set(gtk_builder_get_object(ef_gtk->builder_add, "EF_gtk_barres_add_type_combobox"), "model", projet->list_gtk.ef_barres.liste_types, NULL);
    g_object_set(gtk_builder_get_object(ef_gtk->builder_add, "EF_gtk_barres_add_section_combobox"), "model", projet->list_gtk.ef_sections.liste_sections, NULL);
    g_object_set(gtk_builder_get_object(ef_gtk->builder_add, "EF_gtk_barres_add_materiau_combobox"), "model", projet->list_gtk.ef_materiaux.liste_materiaux, NULL);
    g_object_set(gtk_builder_get_object(ef_gtk->builder_add, "EF_gtk_barres_add_relachement_combobox"), "model", projet->list_gtk.ef_relachements.liste_relachements, NULL);
    
    if (projet->modele.barres != NULL)
    {
        BUGMSG(nb_barres = g_strdup_printf("%d", ((Beton_Barre *)g_list_last(projet->modele.barres)->data)->numero+1), , gettext("Erreur d'allocation mémoire.\n"));
        gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(ef_gtk->builder_add, "EF_gtk_barres_add_numero_label2")), nb_barres);
        free(nb_barres);
    }
    else
        gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(ef_gtk->builder_add, "EF_gtk_barres_add_numero_label2")), "0");
    
    gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(ef_gtk->builder_add, "EF_gtk_barres_add_button_add")), FALSE);
    
    gtk_window_set_transient_for(GTK_WINDOW(ef_gtk->window_add), GTK_WINDOW(projet->list_gtk.comp.window));
}

#endif
