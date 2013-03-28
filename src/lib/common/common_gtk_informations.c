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

gboolean common_gtk_informations_window_key_press(GtkWidget *widget, GdkEvent *event,
  Projet *projet)
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
    BUGMSG(projet->list_gtk.common_informations.builder, FALSE, gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Informations");
    
    if (event->key.keyval == GDK_KEY_Escape)
    {
        gtk_widget_destroy(projet->list_gtk.common_informations.window);
        return TRUE;
    }
    else
        return FALSE;
}


void common_gtk_informations_window_destroy(GtkWidget *object, Projet *projet)
/* Description : Met projet->list_gtk.common_informations.builder à NULL quand la
 *               fenêtre se ferme.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.common_informations.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Informations");
    
    g_object_unref(G_OBJECT(projet->list_gtk.common_informations.builder));
    projet->list_gtk.common_informations.builder = NULL;
    
    return;
}


gboolean common_gtk_informations_recupere_donnees(Projet *projet, char **destinataire,
  char **adresse, unsigned int *code_postal, const char **ville)
/* Description : Récupère toutes les données de la fenêtre permettant d'éditer l'adresse du
 *               projet.
 * Paramètres : Projet *projet : la variable projet,
 *            : char **destinataire : le nom du destinataire,
 *            : char **adresse : la rue du projet,
 *            : unsigned int *code_postal : le code postal du projet,
 *            : char **ville : la ville du projet.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL, cote == NULL, nom == NULL,
 *             en cas d'erreur d'allocation mémoire.
 */
{
    gboolean        ok = TRUE;
    GtkTextIter     start, end;
    GtkTextBuffer   *textbuffer;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(destinataire, FALSE, gettext("Paramètre %s incorrect.\n"), "destinataire");
    BUGMSG(adresse, FALSE, gettext("Paramètre %s incorrect.\n"), "adresse");
    BUGMSG(code_postal, FALSE, gettext("Paramètre %s incorrect.\n"), "code_postal");
    BUGMSG(ville, FALSE, gettext("Paramètre %s incorrect.\n"), "ville");
    BUGMSG(projet->list_gtk.common_informations.builder, FALSE, gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Informations");
    
    *code_postal = common_gtk_entry_uint(GTK_ENTRY(gtk_builder_get_object(projet->list_gtk.common_informations.builder, "common_informations_entry_code_postal")), 0, TRUE, UINT_MAX, FALSE);
    if (*code_postal == UINT_MAX)
        ok = FALSE;
    
    textbuffer = GTK_TEXT_BUFFER(gtk_builder_get_object(projet->list_gtk.common_informations.builder, "common_informations_buffer_destinataire"));
    gtk_text_buffer_get_iter_at_offset(textbuffer, &start, 0);
    gtk_text_buffer_get_iter_at_offset(textbuffer, &end, -1);
    *destinataire = gtk_text_buffer_get_text(textbuffer, &start, &end, FALSE);
    
    textbuffer = GTK_TEXT_BUFFER(gtk_builder_get_object(projet->list_gtk.common_informations.builder, "common_informations_buffer_adresse"));
    gtk_text_buffer_get_iter_at_offset(textbuffer, &start, 0);
    gtk_text_buffer_get_iter_at_offset(textbuffer, &end, -1);
    *adresse = gtk_text_buffer_get_text(textbuffer, &start, &end, FALSE);
    
    *ville = gtk_entry_buffer_get_text(GTK_ENTRY_BUFFER(gtk_builder_get_object(projet->list_gtk.common_informations.builder, "common_informations_buffer_ville")));
    
    return ok;
}


void common_gtk_informations_check(GtkWidget *object, Projet *projet)
/* Description : Vérifie si l'ensemble des éléments est correct pour activer le bouton add/edit.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    unsigned int    code_postal;
    char            *destinataire, *adresse;
    const char      *ville;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.common_informations.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Informations");
    
    if (!common_gtk_informations_recupere_donnees(projet, &destinataire, &adresse, &code_postal, &ville))
        gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.common_informations.builder, "common_informations_button_edit")), FALSE);
    else
    {
        gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.common_informations.builder, "common_informations_button_edit")), TRUE);
        free(destinataire);
        free(adresse);
    }
    
    return;
}


void common_gtk_informations_entry_add_char(GtkEntryBuffer *buffer, guint position,
  gchar *chars, guint n_chars, Projet *projet)
{
    common_gtk_informations_check(NULL, projet);
    return;
}


void common_gtk_informations_entry_del_char(GtkEntryBuffer *buffer, guint position,
  guint n_chars, Projet *projet)
{
    common_gtk_informations_check(NULL, projet);
    return;
}


void common_gtk_informations_annuler_clicked(GtkButton *button, Projet *projet)
/* Description : Ferme la fenêtre sans effectuer les modifications.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.common_informations.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Informations");
    
    gtk_widget_destroy(projet->list_gtk.common_informations.window);
    
    return;
}


void common_gtk_informations_modifier_clicked(GtkButton *button, Projet *projet)
/* Description : Ferme la fenêtre en appliquant les modifications.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    unsigned int    code_postal;
    char            *destinataire, *adresse;
    const char      *ville;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.common_informations.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Informations");
    
    if (!common_gtk_informations_recupere_donnees(projet, &destinataire, &adresse, &code_postal, &ville))
        return;
    
//    BUG(EF_sections_carree_modif(projet, projet->list_gtk.common_informations.section, texte, cote), );
    
    free(destinataire);
    free(adresse);
    
    gtk_widget_destroy(projet->list_gtk.common_informations.window);
    
    return;
}


gboolean common_gtk_informations(Projet *projet)
/* Description : Affichage de la fenêtre permettant de personnaliser l'adresse du projet.
 * Paramètres : Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Echec : FALSE :
 *             projet == NULL,
 *             Fenêtre graphique déjà initialisée.
 */
{
    Gtk_Common_Informations  *ef_gtk;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    
    ef_gtk = &projet->list_gtk.common_informations;
    if (projet->list_gtk.common_informations.builder != NULL)
    {
        gtk_window_present(GTK_WINDOW(projet->list_gtk.common_informations.window));
        return TRUE;
    }
    
    ef_gtk->builder = gtk_builder_new();
    BUGMSG(gtk_builder_add_from_file(ef_gtk->builder, DATADIR"/ui/common_informations.ui", NULL) != 0, FALSE, gettext("Builder Failed\n"));
    gtk_builder_connect_signals(ef_gtk->builder, projet);
    ef_gtk->window = GTK_WIDGET(gtk_builder_get_object(ef_gtk->builder, "common_informations_window"));
    
    common_gtk_informations_check(NULL, projet);
    
/*    gtk_button_set_label(GTK_BUTTON(gtk_builder_get_object(ef_gtk->builder, "EF_section_carree_button_add_edit")), "gtk-add");
    g_signal_connect(gtk_builder_get_object(ef_gtk->builder, "EF_section_carree_button_add_edit"), "clicked", G_CALLBACK(common_gtk_informations_ajouter_clicked), projet);
    {
        gchar       tmp[30];
        Section_T   *data;
        
        gtk_window_set_title(GTK_WINDOW(ef_gtk->window), gettext("Modification d'une section carrée"));
        ef_gtk->section = section;
        BUGMSG(ef_gtk->section->type == SECTION_CARREE, FALSE, gettext("La section à modifier n'est pas carrée.\n"));
        data = ef_gtk->section->data;
        
        gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(gtk_builder_get_object(ef_gtk->builder, "EF_section_carree_textview_nom"))), ef_gtk->section->nom, -1);
        common_math_double_to_char(data->largeur_table, tmp, DECIMAL_DISTANCE);
        gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder, "EF_section_carree_buffer_cote")), tmp, -1);
        
        gtk_button_set_label(GTK_BUTTON(gtk_builder_get_object(ef_gtk->builder, "EF_section_carree_button_add_edit")), "gtk-edit");
        g_signal_connect(gtk_builder_get_object(ef_gtk->builder, "EF_section_carree_button_add_edit"), "clicked", G_CALLBACK(common_gtk_informations_modifier_clicked), projet);
    }*/
    
    gtk_window_set_transient_for(GTK_WINDOW(ef_gtk->window), GTK_WINDOW(projet->list_gtk.comp.window));
    
    return TRUE;
}
#endif
