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

gboolean EF_gtk_section_circulaire_window_key_press(GtkWidget *widget, GdkEvent *event,
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
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet")
    BUGMSG(projet->list_gtk.ef_sections_circulaire.builder, FALSE, gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Ajout Section Circulaire")
    
    if (event->key.keyval == GDK_KEY_Escape)
    {
        gtk_widget_destroy(projet->list_gtk.ef_sections_circulaire.window);
        return TRUE;
    }
    else
        return FALSE;
}


void EF_gtk_section_circulaire_window_destroy(GtkWidget *object, Projet *projet)
/* Description : Met projet->list_gtk.ef_sections_circulaire.builder à NULL quand la
 *               fenêtre se ferme.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet")
    BUGMSG(projet->list_gtk.ef_sections_circulaire.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Ajout Section Circulaire")
    
    g_object_unref(G_OBJECT(projet->list_gtk.ef_sections_circulaire.builder));
    projet->list_gtk.ef_sections_circulaire.builder = NULL;
    
    return;
}


gboolean EF_gtk_section_circulaire_recupere_donnees(Projet *projet, double *diametre,
  gchar **nom)
/* Description : Récupère toutes les données de la fenêtre permettant d'ajouter ou d'éditer une
 *               section.
 * Paramètres : Projet *projet : la variable projet,
 *            : double *diametre : le coté de la section,
 *            : gchar **nom : le nom de la section,
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL, diametre == NULL, nom == NULL,
 *             en cas d'erreur d'allocation mémoire.
 */
{
    GtkTextIter     start, end;
    GtkTextBuffer   *textbuffer;
    gboolean        ok = TRUE;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet")
    BUGMSG(diametre, FALSE, gettext("Paramètre %s incorrect.\n"), "diametre")
    BUGMSG(nom, FALSE, gettext("Paramètre %s incorrect.\n"), "nom")
    BUGMSG(projet->list_gtk.ef_sections_circulaire.builder, FALSE, gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Ajout Section Circulaire")
    
    *diametre = common_gtk_text_buffer_double(GTK_TEXT_BUFFER(gtk_builder_get_object(projet->list_gtk.ef_sections_circulaire.builder, "EF_section_circulaire_buffer_diametre")), 0, FALSE, INFINITY, FALSE);
    if (isnan(*diametre))
        ok = FALSE;
    
    // Si tous les paramètres sont corrects
    textbuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(gtk_builder_get_object(projet->list_gtk.ef_sections_circulaire.builder, "EF_section_circulaire_textview_nom")));
    
    gtk_text_buffer_get_iter_at_offset(textbuffer, &start, 0);
    gtk_text_buffer_get_iter_at_offset(textbuffer, &end, -1);
    *nom = gtk_text_buffer_get_text(textbuffer, &start, &end, FALSE);
    
    gtk_text_buffer_remove_all_tags(textbuffer, &start, &end);
    
    if (projet->list_gtk.ef_sections_circulaire.section == NULL)
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
      ((strcmp(projet->list_gtk.ef_sections_circulaire.section->nom, *nom) != 0) && (EF_sections_cherche_nom(projet, *nom, FALSE))))
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


void EF_gtk_section_circulaire_check(GtkWidget *object, Projet *projet)
/* Description : Vérifie si l'ensemble des éléments est correct pour activer le bouton add/edit.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    double  diametre;
    char    *nom;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet")
    BUGMSG(projet->list_gtk.ef_sections_circulaire.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Ajout Section Circulaire")
    
    if (!EF_gtk_section_circulaire_recupere_donnees(projet, &diametre, &nom))
        gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_sections_circulaire.builder, "EF_section_circulaire_button_add_edit")), FALSE);
    else
    {
        gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_sections_circulaire.builder, "EF_section_circulaire_button_add_edit")), TRUE);
        free(nom);
    }
    
    return;
}


void EF_gtk_section_circulaire_ajouter_clicked(GtkButton *button, Projet *projet)
/* Description : Ferme la fenêtre en ajoutant la section.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    double  diametre;
    gchar   *texte;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet")
    BUGMSG(projet->list_gtk.ef_sections_circulaire.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Ajout Section Circulaire")
    
    if (!(EF_gtk_section_circulaire_recupere_donnees(projet, &diametre, &texte)))
        return;
    
    gtk_widget_destroy(projet->list_gtk.ef_sections_circulaire.window);
    
    BUG(EF_sections_circulaire_ajout(projet, texte, common_math_f(diametre, FLOTTANT_UTILISATEUR)), )
    
    free(texte);
    
    return;
}


void EF_gtk_section_circulaire_annuler_clicked(GtkButton *button, Projet *projet)
/* Description : Ferme la fenêtre sans effectuer les modifications.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet")
    BUGMSG(projet->list_gtk.ef_sections_circulaire.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Ajout Section Circulaire")
    
    gtk_widget_destroy(projet->list_gtk.ef_sections_circulaire.window);
    
    return;
}


void EF_gtk_section_circulaire_modifier_clicked(GtkButton *button, Projet *projet)
/* Description : Ferme la fenêtre en appliquant les modifications.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    double      diametre;
    gchar       *texte;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet")
    BUGMSG(projet->list_gtk.ef_sections_circulaire.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Ajout Section Circulaire")
    
    if (!(EF_gtk_section_circulaire_recupere_donnees(projet, &diametre, &texte)))
        return;
    
    BUG(EF_sections_circulaire_modif(projet, projet->list_gtk.ef_sections_circulaire.section, texte, common_math_f(diametre, FLOTTANT_UTILISATEUR)), )
    
    free(texte);
    
    gtk_widget_destroy(projet->list_gtk.ef_sections_circulaire.window);
    
    return;
}


gboolean EF_gtk_section_circulaire(Projet *projet, EF_Section *section)
/* Description : Affichage de la fenêtre permettant de créer ou modifier une section de type
 *               circulaire.
 * Paramètres : Projet *projet : la variable projet,
 *            : EF_Section *section : section à modifier. NULL si nouvelle section,
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Echec : FALSE :
 *             projet == NULL,
 *             Fenêtre graphique déjà initialisée.
 */
{
    Gtk_EF_Sections_Circulaire  *ef_gtk;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet")
    
    ef_gtk = &projet->list_gtk.ef_sections_circulaire;
    if (projet->list_gtk.ef_sections_circulaire.builder != NULL)
    {
        gtk_window_present(GTK_WINDOW(projet->list_gtk.ef_sections_circulaire.window));
        if (projet->list_gtk.ef_sections_circulaire.section == section)
            return TRUE;
    }
    else
    {
        ef_gtk->builder = gtk_builder_new();
        BUGMSG(gtk_builder_add_from_resource(ef_gtk->builder, "/org/2lgc/codegui/ui/EF_sections_circulaire.ui", NULL) != 0, FALSE, gettext("Builder Failed\n"))
        gtk_builder_connect_signals(ef_gtk->builder, projet);
        ef_gtk->window = GTK_WIDGET(gtk_builder_get_object(ef_gtk->builder, "EF_section_circulaire_window"));
    }
    
    if (section == NULL)
    {
        gtk_window_set_title(GTK_WINDOW(ef_gtk->window), gettext("Ajout d'une section circulaire"));
        ef_gtk->section = NULL;
        
        gtk_button_set_label(GTK_BUTTON(gtk_builder_get_object(ef_gtk->builder, "EF_section_circulaire_button_add_edit")), gettext("_Ajouter"));
        g_signal_connect(gtk_builder_get_object(ef_gtk->builder, "EF_section_circulaire_button_add_edit"), "clicked", G_CALLBACK(EF_gtk_section_circulaire_ajouter_clicked), projet);
        EF_gtk_section_circulaire_check(NULL, projet);
    }
    else
    {
        gchar           tmp[30];
        Section_Circulaire  *data;
        
        gtk_window_set_title(GTK_WINDOW(ef_gtk->window), gettext("Modification d'une section circulaire"));
        ef_gtk->section = section;
        BUGMSG(ef_gtk->section->type == SECTION_CIRCULAIRE, FALSE, gettext("La section à modifier n'est pas circulaire.\n"))
        data = ef_gtk->section->data;
        
        gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(gtk_builder_get_object(ef_gtk->builder, "EF_section_circulaire_textview_nom"))), ef_gtk->section->nom, -1);
        common_math_double_to_char2(data->diametre, tmp, DECIMAL_DISTANCE);
        gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder, "EF_section_circulaire_buffer_diametre")), tmp, -1);
        
        gtk_button_set_label(GTK_BUTTON(gtk_builder_get_object(ef_gtk->builder, "EF_section_circulaire_button_add_edit")), gettext("_Modifier"));
        g_signal_connect(gtk_builder_get_object(ef_gtk->builder, "EF_section_circulaire_button_add_edit"), "clicked", G_CALLBACK(EF_gtk_section_circulaire_modifier_clicked), projet);
    }
    
    
    gtk_window_set_transient_for(GTK_WINDOW(ef_gtk->window), GTK_WINDOW(projet->list_gtk.comp.window));
    
    return TRUE;
}
#endif
