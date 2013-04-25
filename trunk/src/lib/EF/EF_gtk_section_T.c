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
#include <math.h>
#include <string.h>

#include "common_projet.h"
#include "common_erreurs.h"
#include "common_gtk.h"
#include "common_math.h"
#include "common_selection.h"
#include "EF_sections.h"

gboolean EF_gtk_section_T_window_key_press(GtkWidget *widget, GdkEvent *event, Projet *projet)
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
    BUGMSG(projet->list_gtk.ef_sections_T.builder, FALSE, gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Ajout Section T");
    
    if (event->key.keyval == GDK_KEY_Escape)
    {
        gtk_widget_destroy(projet->list_gtk.ef_sections_T.window);
        return TRUE;
    }
    else
        return FALSE;
}


void EF_gtk_section_T_window_destroy(GtkWidget *object, Projet *projet)
/* Description : Met projet->list_gtk.ef_sections_T.builder à NULL quand la
 *               fenêtre se ferme.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_sections_T.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Ajout Section T");
    
    g_object_unref(G_OBJECT(projet->list_gtk.ef_sections_T.builder));
    projet->list_gtk.ef_sections_T.builder = NULL;
    
    return;
}


gboolean EF_gtk_section_T_recupere_donnees(Projet *projet, double *lt, double *ht, double *lr,
  double *hr, gchar **nom)
/* Description : Récupère toutes les données de la fenêtre permettant d'ajouter ou d'éditer une
 *               section. Ici, la fonction utilise return et non pas la macro BUG.
 * Paramètres : Projet *projet : la variable projet,
 *            : double *lt : la largeur de la table de la section,
 *            : double *ht : la hauteur de la table de la section,
 *            : double *lr : la largeur de la retombée de la section,
 *            : double *hr : la hauteur de la retombée de la section,
 *            : gchar **nom : le nom de la section,
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL, lt == NULL, ht == NULL, la == NULL, ha == NULL, nom == NULL,
 *             en cas d'erreur d'allocation mémoire.
 */
{
    GtkTextIter     start, end;
    GtkTextBuffer   *textbuffer;
    gboolean        ok = TRUE;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(lt, FALSE, gettext("Paramètre %s incorrect.\n"), "largeur");
    BUGMSG(ht, FALSE, gettext("Paramètre %s incorrect.\n"), "hauteur");
    BUGMSG(lr, FALSE, gettext("Paramètre %s incorrect.\n"), "largeur");
    BUGMSG(hr, FALSE, gettext("Paramètre %s incorrect.\n"), "hauteur");
    BUGMSG(nom, FALSE, gettext("Paramètre %s incorrect.\n"), "nom");
    BUGMSG(projet->list_gtk.ef_sections_T.builder, FALSE, gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Ajout Section T");
    
    *lr = common_gtk_text_buffer_double(GTK_TEXT_BUFFER(gtk_builder_get_object(projet->list_gtk.ef_sections_T.builder, "EF_section_T_buffer_lr")), 0., FALSE, INFINITY, FALSE);
    if (isnan(*lr))
        ok = FALSE;
    
    *hr = common_gtk_text_buffer_double(GTK_TEXT_BUFFER(gtk_builder_get_object(projet->list_gtk.ef_sections_T.builder, "EF_section_T_buffer_hr")), 0., FALSE, INFINITY, FALSE);
    if (isnan(*hr))
        ok = FALSE;
    
    *lt = common_gtk_text_buffer_double(GTK_TEXT_BUFFER(gtk_builder_get_object(projet->list_gtk.ef_sections_T.builder, "EF_section_T_buffer_lt")), 0., FALSE, INFINITY, FALSE);
    if (isnan(*lt))
        ok = FALSE;
    
    *ht = common_gtk_text_buffer_double(GTK_TEXT_BUFFER(gtk_builder_get_object(projet->list_gtk.ef_sections_T.builder, "EF_section_T_buffer_ht")), 0., FALSE, INFINITY, FALSE);
    if (isnan(*ht))
        ok = FALSE;
    
    // Si tous les paramètres sont corrects
    textbuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(gtk_builder_get_object(projet->list_gtk.ef_sections_T.builder, "EF_section_T_textview_nom")));
    
    gtk_text_buffer_get_iter_at_offset(textbuffer, &start, 0);
    gtk_text_buffer_get_iter_at_offset(textbuffer, &end, -1);
    *nom = gtk_text_buffer_get_text(textbuffer, &start, &end, FALSE);
    
    gtk_text_buffer_remove_all_tags(textbuffer, &start, &end);
    
    if (projet->list_gtk.ef_sections_T.section == NULL)
    {
        if ((strcmp(*nom, "") == 0) || (EF_sections_cherche_nom(projet, *nom, FALSE)))
        {
            gtk_text_buffer_apply_tag_by_name(textbuffer, "mauvais", &start, &end);
            ok = FALSE;
        }
        else
            gtk_text_buffer_apply_tag_by_name(textbuffer, "OK", &start, &end);
    }
    else if ((strcmp(*nom, "") == 0) ||
      ((strcmp(projet->list_gtk.ef_sections_T.section->nom, *nom) != 0) && (EF_sections_cherche_nom(projet, *nom, FALSE))))
    {
        gtk_text_buffer_apply_tag_by_name(textbuffer, "mauvais", &start, &end);
        ok = FALSE;
    }
    else
        gtk_text_buffer_apply_tag_by_name(textbuffer, "OK", &start, &end);
    
    if (ok == FALSE)
        free(*nom);
    
    return ok;
}


void EF_gtk_section_T_check(GtkWidget *object, Projet *projet)
/* Description : Vérifie si l'ensemble des éléments est correct pour activer le bouton add/edit.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    double  lt, ht, lr, hr;
    char    *nom;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_sections_T.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Ajout Section T");
    
    if (!EF_gtk_section_T_recupere_donnees(projet, &lt, &ht, &lr, &hr, &nom))
        gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_sections_T.builder, "EF_section_T_button_add_edit")), FALSE);
    else
    {
        gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_sections_T.builder, "EF_section_T_button_add_edit")), TRUE);
        free(nom);
    }
    
    return;
}


void EF_gtk_section_T_ajouter_clicked(GtkButton *button, Projet *projet)
/* Description : Ferme la fenêtre en ajoutant la charge.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    double  lr, hr, lt, ht;
    gchar   *texte;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_sections_T.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Ajout Section T");
    
    if (!(EF_gtk_section_T_recupere_donnees(projet, &lt, &ht, &lr, &hr, &texte)))
        return;
    
    // Création de la nouvelle charge ponctuelle au noeud
    BUG(EF_sections_T_ajout(projet, texte, common_math_f(lt, FLOTTANT_UTILISATEUR), common_math_f(lr, FLOTTANT_UTILISATEUR), common_math_f(ht, FLOTTANT_UTILISATEUR), common_math_f(hr, FLOTTANT_UTILISATEUR)), );
    
    free(texte);
    
    gtk_widget_destroy(projet->list_gtk.ef_sections_T.window);
    
    return;
}


void EF_gtk_section_T_annuler_clicked(GtkButton *button, Projet *projet)
/* Description : Ferme la fenêtre sans effectuer les modifications.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_sections_T.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Ajout Section T");
    
    gtk_widget_destroy(projet->list_gtk.ef_sections_T.window);
    
    return;
}


void EF_gtk_section_T_modifier_clicked(GtkButton *button, Projet *projet)
/* Description : Ferme la fenêtre en appliquant les modifications.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    double      lt, ht, lr, hr;
    gchar       *texte;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_sections_T.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Ajout Section T");
    
    if (!(EF_gtk_section_T_recupere_donnees(projet, &lt, &ht, &lr, &hr, &texte)))
        return;
    
    gtk_widget_destroy(projet->list_gtk.ef_sections_T.window);
    
    BUG(EF_sections_T_modif(projet, projet->list_gtk.ef_sections_T.section, texte, common_math_f(lt, FLOTTANT_UTILISATEUR), common_math_f(lr, FLOTTANT_UTILISATEUR), common_math_f(ht, FLOTTANT_UTILISATEUR), common_math_f(hr, FLOTTANT_UTILISATEUR)), );
    
    free(texte);
    
    return;
}


gboolean EF_gtk_section_T(Projet *projet, EF_Section *section)
/* Description : Affichage de la fenêtre permettant de créer ou modifier une section de type
 *               en T.
 * Paramètres : Projet *projet : la variable projet,
 *            : EF_Section *section : section à modifier. NULL si nouvelle section,
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Echec : FALSE :
 *             projet == NULL,
 *             Fenêtre graphique déjà initialisée.
 */
{
    Gtk_EF_Sections_T   *ef_gtk;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    
    ef_gtk = &projet->list_gtk.ef_sections_T;
    if (projet->list_gtk.ef_sections_T.builder != NULL)
    {
        gtk_window_present(GTK_WINDOW(projet->list_gtk.ef_sections_T.window));
        if (projet->list_gtk.ef_sections_T.section == section)
            return TRUE;
    }
    else
    {
        ef_gtk->builder = gtk_builder_new();
        BUGMSG(gtk_builder_add_from_resource(ef_gtk->builder, "/org/2lgc/codegui/ui/EF_sections_T.ui", NULL) != 0, FALSE, gettext("Builder Failed\n"));
        gtk_builder_connect_signals(ef_gtk->builder, projet);
        ef_gtk->window = GTK_WIDGET(gtk_builder_get_object(ef_gtk->builder, "EF_section_T_window"));
    }
    
    if (section == NULL)
    {
        gtk_window_set_title(GTK_WINDOW(ef_gtk->window), gettext("Ajout d'une section en T"));
        ef_gtk->section = NULL;
        
        gtk_button_set_label(GTK_BUTTON(gtk_builder_get_object(ef_gtk->builder, "EF_section_T_button_add_edit")), "gtk-add");
        g_signal_connect(gtk_builder_get_object(ef_gtk->builder, "EF_section_T_button_add_edit"), "clicked", G_CALLBACK(EF_gtk_section_T_ajouter_clicked), projet);
        EF_gtk_section_T_check(NULL, projet);
    }
    else
    {
        gchar       tmp[30];
        Section_T   *data;
        
        gtk_window_set_title(GTK_WINDOW(ef_gtk->window), gettext("Modification d'une section en T"));
        ef_gtk->section = section;
        BUGMSG(ef_gtk->section->type == SECTION_T, FALSE, gettext("La section à modifier n'est pas en T.\n"));
        data = ef_gtk->section->data;
        
        gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(gtk_builder_get_object(ef_gtk->builder, "EF_section_T_textview_nom"))), ef_gtk->section->nom, -1);
        common_math_double_to_char2(data->largeur_table, tmp, DECIMAL_DISTANCE);
        gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder, "EF_section_T_buffer_lt")), tmp, -1);
        common_math_double_to_char2(data->hauteur_table, tmp, DECIMAL_DISTANCE);
        gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder, "EF_section_T_buffer_ht")), tmp, -1);
        common_math_double_to_char2(data->largeur_retombee, tmp, DECIMAL_DISTANCE);
        gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder, "EF_section_T_buffer_lr")), tmp, -1);
        common_math_double_to_char2(data->hauteur_retombee, tmp, DECIMAL_DISTANCE);
        gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder, "EF_section_T_buffer_hr")), tmp, -1);
        
        gtk_button_set_label(GTK_BUTTON(gtk_builder_get_object(ef_gtk->builder, "EF_section_T_button_add_edit")), "gtk-edit");
        g_signal_connect(gtk_builder_get_object(ef_gtk->builder, "EF_section_T_button_add_edit"), "clicked", G_CALLBACK(EF_gtk_section_T_modifier_clicked), projet);
    }
    
    gtk_window_set_transient_for(GTK_WINDOW(ef_gtk->window), GTK_WINDOW(projet->list_gtk.comp.window));
    
    return TRUE;
}
#endif
