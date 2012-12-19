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

#include "common_projet.h"
#include "common_erreurs.h"
#include "common_gtk.h"
#include "common_math.h"
#include "common_selection.h"
#include "EF_sections.h"

G_MODULE_EXPORT gboolean EF_gtk_section_rectangulaire_window_key_press(
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
    BUGMSG(projet->list_gtk.ef_sections_rectangulaire.builder, FALSE, gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Ajout Section Rectangulaire");
    
    if (event->key.keyval == GDK_KEY_Escape)
    {
        gtk_widget_destroy(projet->list_gtk.ef_sections_rectangulaire.window);
        return TRUE;
    }
    else
        return FALSE;
}


G_MODULE_EXPORT void EF_gtk_section_rectangulaire_window_destroy(
  GtkWidget *object __attribute__((unused)), Projet *projet)
/* Description : Met projet->list_gtk.ef_sections_rectangulaire.builder à NULL quand la
 *               fenêtre se ferme.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_sections_rectangulaire.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Ajout Section Rectangulaire");
    
    projet->list_gtk.ef_sections_rectangulaire.builder = NULL;
    
    return;
}


gboolean EF_gtk_section_rectangulaire_recupere_donnees(Projet *projet, double *largeur,
  double *hauteur, gchar **nom)
/* Description : Récupère toutes les données de la fenêtre permettant d'ajouter ou d'éditer une
 *               section.
 * Paramètres : Projet *projet : la variable projet,
 *            : double *largeur : la largeur de la section,
 *            : double *hauteur : la hauteur de la section,
 *            : gchar **nom : le nom de la section,
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL, largeur == NULL, hauteur == NULL, nom == NULL,
 *             en cas d'erreur d'allocation mémoire.
 */
{
    Gtk_EF_Sections_Rectangulaire *ef_gtk;
    
    GtkWidget       *dialog;
    GtkTextIter     start, end;
    GtkTextBuffer   *textbuffer;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(largeur, FALSE, gettext("Paramètre %s incorrect.\n"), "largeur");
    BUGMSG(hauteur, FALSE, gettext("Paramètre %s incorrect.\n"), "hauteur");
    BUGMSG(nom, FALSE, gettext("Paramètre %s incorrect.\n"), "nom");
    BUGMSG(projet->list_gtk.ef_sections_rectangulaire.builder, FALSE, gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Ajout Section Rectangulaire");
    
    ef_gtk = &projet->list_gtk.ef_sections_rectangulaire;
    
    *largeur = common_gtk_entry_renvoie_double(GTK_TEXT_BUFFER(gtk_builder_get_object(projet->list_gtk.ef_sections_rectangulaire.builder, "EF_section_rectangulaire_buffer_largeur")));
    if ((isnan(*largeur)) || (*largeur < ERREUR_RELATIVE_MIN))
    {
        dialog = gtk_message_dialog_new(GTK_WINDOW(ef_gtk->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, gettext("La valeur de la largeur est incorrecte."));
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return FALSE;
    }
    
    *hauteur = common_gtk_entry_renvoie_double(GTK_TEXT_BUFFER(gtk_builder_get_object(projet->list_gtk.ef_sections_rectangulaire.builder, "EF_section_rectangulaire_buffer_hauteur")));
    if ((isnan(*hauteur)) || (*hauteur < ERREUR_RELATIVE_MIN))
    {
        dialog = gtk_message_dialog_new(GTK_WINDOW(ef_gtk->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, gettext("La valeur de la hauteur est incorrecte."));
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return FALSE;
    }
    
    // Si tous les paramètres sont corrects
    textbuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(gtk_builder_get_object(projet->list_gtk.ef_sections_rectangulaire.builder, "EF_section_rectangulaire_textview_nom")));
    
    gtk_text_buffer_get_iter_at_offset(textbuffer, &start, 0);
    gtk_text_buffer_get_iter_at_offset(textbuffer, &end, -1);
    *nom = gtk_text_buffer_get_text(textbuffer, &start, &end, FALSE);
    
    return TRUE;
}


G_MODULE_EXPORT void EF_gtk_section_rectangulaire_ajouter_clicked(
  GtkButton *button __attribute__((unused)), Projet *projet)
/* Description : Ferme la fenêtre en ajoutant la charge.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    double  largeur, hauteur;
    gchar   *texte;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_sections_rectangulaire.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Ajout Section Rectangulaire");
    
    if (!(EF_gtk_section_rectangulaire_recupere_donnees(projet, &largeur, &hauteur, &texte)))
        return;
    
    // Création de la nouvelle charge ponctuelle au noeud
    BUG(EF_sections_ajout_rectangulaire(projet, texte, largeur, hauteur), );
    
    free(texte);
    
    gtk_widget_destroy(projet->list_gtk.ef_sections_rectangulaire.window);
    
    return;
}


G_MODULE_EXPORT void EF_gtk_section_rectangulaire_annuler_clicked(
  GtkButton *button __attribute__((unused)), Projet *projet)
/* Description : Ferme la fenêtre sans effectuer les modifications.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_sections_rectangulaire.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Ajout Section Rectangulaire");
    
    gtk_widget_destroy(projet->list_gtk.ef_sections_rectangulaire.window);
    
    return;
}


G_MODULE_EXPORT void EF_gtk_section_rectangulaire_modifier_clicked(
  GtkButton *button __attribute__((unused)), Projet *projet)
/* Description : Ferme la fenêtre en appliquant les modifications.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    double      largeur, hauteur;
    gchar       *texte;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_sections_rectangulaire.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Ajout Section Rectangulaire");
    
    if (!(EF_gtk_section_rectangulaire_recupere_donnees(projet, &largeur, &hauteur, &texte)))
        return;
    
    BUG(EF_sections_modif_rectangulaire(projet, projet->list_gtk.ef_sections_rectangulaire.section, texte, largeur, hauteur), );
    
    gtk_widget_destroy(projet->list_gtk.ef_sections_rectangulaire.window);
    
    return;
}


G_MODULE_EXPORT gboolean EF_gtk_section_rectangulaire(Projet *projet, EF_Section *section __attribute__((unused)))
/* Description : Affichage de la fenêtre permettant de créer ou modifier une section de type
 *               rectangulaire.
 * Paramètres : Projet *projet : la variable projet,
 *            : EF_Section *section : section à modifier. NULL si nouvelle section,
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Echec : FALSE :
 *             projet == NULL,
 *             Fenêtre graphique déjà initialisée.
 */
{
    Gtk_EF_Sections_Rectangulaire   *ef_gtk;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    if (projet->list_gtk.ef_sections_rectangulaire.builder != NULL)
    {
        gtk_window_present(GTK_WINDOW(projet->list_gtk.ef_sections_rectangulaire.window));
        return TRUE;
    }
    
    ef_gtk = &projet->list_gtk.ef_sections_rectangulaire;
    ef_gtk->builder = gtk_builder_new();
    BUGMSG(gtk_builder_add_from_file(ef_gtk->builder, DATADIR"/ui/EF_sections_rectangulaire.ui", NULL) != 0, FALSE, gettext("Builder Failed\n"));
    gtk_builder_connect_signals(ef_gtk->builder, projet);
    
    ef_gtk->window = GTK_WIDGET(gtk_builder_get_object(ef_gtk->builder, "EF_section_rectangulaire_window"));
    
    if (section == NULL)
    {
        gtk_window_set_title(GTK_WINDOW(ef_gtk->window), gettext("Ajout d'une section rectangulaire"));
        ef_gtk->section = NULL;
        
        gtk_button_set_label(GTK_BUTTON(gtk_builder_get_object(ef_gtk->builder, "EF_section_rectangulaire_button_add_edit")), "gtk-add");
        g_signal_connect(gtk_builder_get_object(ef_gtk->builder, "EF_section_rectangulaire_button_add_edit"), "clicked", G_CALLBACK(EF_gtk_section_rectangulaire_ajouter_clicked), projet);
    }
    else
    {
        gchar       tmp[30];
        Section_T   *data;
        
        gtk_window_set_title(GTK_WINDOW(ef_gtk->window), gettext("Modification d'une section rectangulaire"));
        ef_gtk->section = section;
        BUGMSG(ef_gtk->section->type == SECTION_RECTANGULAIRE, FALSE, gettext("La section à modifier n'est pas rectangulaire.\n"));
        data = ef_gtk->section->data;
        
        gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(gtk_builder_get_object(ef_gtk->builder, "EF_section_rectangulaire_textview_nom"))), ef_gtk->section->nom, -1);
        common_math_double_to_char(data->largeur_retombee, tmp, DECIMAL_DISTANCE);
        gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder, "EF_section_rectangulaire_buffer_largeur")), tmp, -1);
        common_math_double_to_char(data->hauteur_retombee, tmp, DECIMAL_DISTANCE);
        gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder, "EF_section_rectangulaire_buffer_hauteur")), tmp, -1);
        
        gtk_button_set_label(GTK_BUTTON(gtk_builder_get_object(ef_gtk->builder, "EF_section_rectangulaire_button_add_edit")), "gtk-edit");
        g_signal_connect(gtk_builder_get_object(ef_gtk->builder, "EF_section_rectangulaire_button_add_edit"), "clicked", G_CALLBACK(EF_gtk_section_rectangulaire_modifier_clicked), projet);
    }
    
    gtk_window_set_transient_for(GTK_WINDOW(ef_gtk->window), GTK_WINDOW(projet->list_gtk.comp.window));
    
    return TRUE;
}
#endif
