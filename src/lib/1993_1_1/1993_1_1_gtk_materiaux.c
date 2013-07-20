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
#include "1993_1_1_materiaux.h"
#include "EF_materiaux.h"

gboolean _1993_1_1_gtk_materiaux_window_key_press(GtkWidget *widget, GdkEvent *event,
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
    BUGMSG(projet->list_gtk._1993_1_1_materiaux.builder, FALSE, gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Ajout Matériau Acier");
    
    if (event->key.keyval == GDK_KEY_Escape)
    {
        gtk_widget_destroy(projet->list_gtk._1993_1_1_materiaux.window);
        return TRUE;
    }
    else
        return FALSE;
}


void _1993_1_1_gtk_materiaux_window_destroy(GtkWidget *object, Projet *projet)
/* Description : Met projet->list_gtk._1993_1_1_materiaux.builder à NULL quand la
 *               fenêtre se ferme.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk._1993_1_1_materiaux.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Ajout Matériau Acier");
    
    g_object_unref(G_OBJECT(projet->list_gtk._1993_1_1_materiaux.builder));
    projet->list_gtk._1993_1_1_materiaux.builder = NULL;
    
    return;
}


gboolean _1993_1_1_gtk_materiaux_recupere_donnees(Projet *projet, char **nom, double *fy,
  double *fu, double *e, double *nu)
/* Description : Récupère toutes les données de la fenêtre permettant d'ajouter ou d'éditer un
 *               matériau de type acier.
 * Paramètres : Projet *projet : la variable projet,
 *            : le reste : les différentes propriétés du matériau béton, en unité SI.
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
    GtkBuilder      *builder;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(nom, FALSE, gettext("Paramètre %s incorrect.\n"), "nom");
    BUGMSG(fy, FALSE, gettext("Paramètre %s incorrect.\n"), "fy");
    BUGMSG(fu, FALSE, gettext("Paramètre %s incorrect.\n"), "fu");
    BUGMSG(e, FALSE, gettext("Paramètre %s incorrect.\n"), "e");
    BUGMSG(nu, FALSE, gettext("Paramètre %s incorrect.\n"), "nu");
    BUGMSG(projet->list_gtk._1993_1_1_materiaux.builder, FALSE, gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Ajout Matériau Acier");
    
    builder = projet->list_gtk._1993_1_1_materiaux.builder;
    
    *fy = common_gtk_text_buffer_double(GTK_TEXT_BUFFER(gtk_builder_get_object(builder, "_1993_1_1_materiaux_buffer_fy")), 0., FALSE, INFINITY, FALSE)*1000000.;
    if (isnan(*fy))
        ok = FALSE;
    
    if (ok)
    {
        *fu = common_gtk_text_buffer_double(GTK_TEXT_BUFFER(gtk_builder_get_object(builder, "_1993_1_1_materiaux_buffer_fu")), 0., FALSE, INFINITY, FALSE)*1000000.;
        if (isnan(*fu))
            ok = FALSE;
    }
    
    if (ok)
    {
        if (gtk_widget_get_visible(GTK_WIDGET(gtk_builder_get_object(builder, "_1993_1_1_materiaux_scrolledwindow_e"))))
        {
            *e = common_gtk_text_buffer_double(GTK_TEXT_BUFFER(gtk_builder_get_object(builder, "_1993_1_1_materiaux_buffer_e")), 0., FALSE, INFINITY, FALSE)*1000000000.;
            if (isnan(*e))
                ok = FALSE;
        }
        else
            *e = MODULE_YOUNG_ACIER;
    }
    
    if (ok)
    {
        if (gtk_widget_get_visible(GTK_WIDGET(gtk_builder_get_object(builder, "_1993_1_1_materiaux_scrolledwindow_nu"))))
        {
            *nu = common_gtk_text_buffer_double(GTK_TEXT_BUFFER(gtk_builder_get_object(builder, "_1993_1_1_materiaux_buffer_nu")), 0., TRUE, 0.5, FALSE);
            if (isnan(*nu))
                ok = FALSE;
        }
        else
            *nu = COEFFICIENT_NU_ACIER;
    }
    
    // Si tous les paramètres sont corrects
    textbuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(gtk_builder_get_object(projet->list_gtk._1993_1_1_materiaux.builder, "_1993_1_1_materiaux_textview_nom")));
    
    gtk_text_buffer_get_iter_at_offset(textbuffer, &start, 0);
    gtk_text_buffer_get_iter_at_offset(textbuffer, &end, -1);
    *nom = gtk_text_buffer_get_text(textbuffer, &start, &end, FALSE);
    
    gtk_text_buffer_remove_all_tags(textbuffer, &start, &end);
    
    if (projet->list_gtk._1993_1_1_materiaux.materiau == NULL)
    {
        if ((strcmp(*nom, "") == 0) || (EF_materiaux_cherche_nom(projet, *nom, FALSE)))
        {
            gtk_text_buffer_apply_tag_by_name(textbuffer, "mauvais", &start, &end);
            ok = FALSE;
        }
        else
            gtk_text_buffer_apply_tag_by_name(textbuffer, "OK", &start, &end);
    }
    else if ((strcmp(*nom, "") == 0) || 
      ((strcmp(projet->list_gtk._1993_1_1_materiaux.materiau->nom, *nom) != 0) && (EF_materiaux_cherche_nom(projet, *nom, FALSE))))
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


void _1993_1_1_gtk_materiaux_check(GtkWidget *object, Projet *projet)
/* Description : Vérifie si l'ensemble des éléments est correct pour activer le bouton add/edit.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    char *nom;
    double fy, fu, e, nu;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk._1993_1_1_materiaux.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Ajout Matériau Acier");
    
    if (!_1993_1_1_gtk_materiaux_recupere_donnees(projet, &nom, &fy, &fu, &e, &nu))
        gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk._1993_1_1_materiaux.builder, "_1993_1_1_materiaux_button_add_edit")), FALSE);
    else
    {
        gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk._1993_1_1_materiaux.builder, "_1993_1_1_materiaux_button_add_edit")), TRUE);
        free(nom);
    }
    
    return;
}


void _1993_1_1_gtk_materiaux_ajouter_clicked(GtkButton *button, Projet *projet)
/* Description : Ferme la fenêtre en ajoutant la charge.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    char            *nom;
    double          fy, fu, e, nu;
    EF_Materiau     *materiau;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk._1993_1_1_materiaux.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Ajout Matériau Acier");
    
    if (!_1993_1_1_gtk_materiaux_recupere_donnees(projet, &nom, &fy, &fu, &e, &nu))
        return;
    
    // Création de la nouvelle charge ponctuelle au noeud
    BUG(materiau = _1993_1_1_materiaux_ajout(projet, nom, common_math_f(fy/1000000., FLOTTANT_UTILISATEUR), common_math_f(fu/1000000., FLOTTANT_UTILISATEUR)), );
    BUG(_1993_1_1_materiaux_modif(projet, materiau, NULL, common_math_f(NAN, FLOTTANT_UTILISATEUR), common_math_f(NAN, FLOTTANT_UTILISATEUR), common_math_f(e, FLOTTANT_UTILISATEUR), common_math_f(nu, FLOTTANT_UTILISATEUR)), );
    
    free(nom);
    
    gtk_widget_destroy(projet->list_gtk._1993_1_1_materiaux.window);
    
    return;
}


void _1993_1_1_gtk_materiaux_annuler_clicked(GtkButton *button, Projet *projet)
/* Description : Ferme la fenêtre sans effectuer les modifications.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk._1993_1_1_materiaux.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Ajout Matériau Béton");
    
    gtk_widget_destroy(projet->list_gtk._1993_1_1_materiaux.window);
    
    return;
}


void _1993_1_1_gtk_materiaux_modifier_clicked(GtkButton *button, Projet *projet)
/* Description : Ferme la fenêtre en appliquant les modifications.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    Gtk_1993_1_1_Materiaux  *ef_gtk;
    
    char    *nom;
    double  fy, fu, e, nu;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk._1993_1_1_materiaux.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Ajout Matériau Acier");
    
    ef_gtk = &projet->list_gtk._1993_1_1_materiaux;
    
    if (!_1993_1_1_gtk_materiaux_recupere_donnees(projet, &nom, &fy, &fu, &e, &nu))
        return;
    
    BUG(_1993_1_1_materiaux_modif(projet, ef_gtk->materiau, nom, common_math_f(fy, FLOTTANT_UTILISATEUR), common_math_f(fu, FLOTTANT_UTILISATEUR), common_math_f(e, FLOTTANT_UTILISATEUR), common_math_f(nu, FLOTTANT_UTILISATEUR)), );
    
    free(nom);
    
    gtk_widget_destroy(projet->list_gtk._1993_1_1_materiaux.window);
    
    return;
}


void _1993_1_1_gtk_materiaux_toggled(GtkCheckMenuItem *checkmenuitem, Projet *projet)
/* Description : Evènement pour (dés)activer la personnalisation d'une propriété de l'acier.
 * Paramètres : GtkCheckMenuItem *checkmenuitem : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    GtkBuilder      *builder;
    gboolean        check = gtk_check_menu_item_get_active(checkmenuitem);
    EF_Materiau     *mat;
    Materiau_Acier  *acier_data;
    char            tmp[30];
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    
    builder = projet->list_gtk._1993_1_1_materiaux.builder;
    mat = projet->list_gtk._1993_1_1_materiaux.materiau;
    if (mat != NULL)
        acier_data = projet->list_gtk._1993_1_1_materiaux.materiau->data;
    else
        acier_data = NULL;
    
    if (checkmenuitem == GTK_CHECK_MENU_ITEM(gtk_builder_get_object(builder, "_1993_1_1_materiaux_personnaliser_menu_fu")))
    {
        if (check && mat)
        {
            common_math_double_to_char2(common_math_f(common_math_get(acier_data->fu)/1000000., acier_data->fu.type), tmp, DECIMAL_CONTRAINTE);
            gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gtk_builder_get_object(builder, "_1993_1_1_materiaux_buffer_fu")), tmp, -1);
        }
        
        gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(builder, "_1993_1_1_materiaux_label_fu")), check);
        gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(builder, "_1993_1_1_materiaux_scrolledwindow_fu")), check);
    }
    else if (checkmenuitem == GTK_CHECK_MENU_ITEM(gtk_builder_get_object(builder, "_1993_1_1_materiaux_personnaliser_menu_e")))
    {
        if (check && mat)
        {
            common_math_double_to_char2(common_math_f(common_math_get(acier_data->e)/1000000000., acier_data->e.type), tmp, DECIMAL_CONTRAINTE);
            gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gtk_builder_get_object(builder, "_1993_1_1_materiaux_buffer_e")), tmp, -1);
        }
        gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(builder, "_1993_1_1_materiaux_label_e")), check);
        gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(builder, "_1993_1_1_materiaux_scrolledwindow_e")), check);
    }
    else if (checkmenuitem == GTK_CHECK_MENU_ITEM(gtk_builder_get_object(builder, "_1993_1_1_materiaux_personnaliser_menu_nu")))
    {
        if (check && mat)
        {
            common_math_double_to_char2(acier_data->nu, tmp, DECIMAL_SANS_UNITE);
            gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gtk_builder_get_object(builder, "_1993_1_1_materiaux_buffer_nu")), tmp, -1);
        }
        gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(builder, "_1993_1_1_materiaux_label_nu")), check);
        gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(builder, "_1993_1_1_materiaux_scrolledwindow_nu")), check);
    }
    else
        BUGMSG(NULL, , gettext("Paramètre %s incorrect.\n"), "checkmenuitem");
    
    _1993_1_1_gtk_materiaux_check(NULL, projet);
    
    return;
}


gboolean _1993_1_1_gtk_materiaux(Projet *projet, EF_Materiau *materiau)
/* Description : Affichage de la fenêtre permettant de créer ou modifier un matériau de type
 *               béton.
 * Paramètres : Projet *projet : la variable projet,
 *            : EF_Materiau *materiau : materiau à modifier. NULL si nouveau matériau,
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Echec : FALSE :
 *             projet == NULL,
 *             Fenêtre graphique déjà initialisée.
 */
{
    Gtk_1993_1_1_Materiaux  *ef_gtk;
    Materiau_Acier          *acier_data;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    
    ef_gtk = &projet->list_gtk._1993_1_1_materiaux;
    if (ef_gtk->builder != NULL)
    {
        gtk_window_present(GTK_WINDOW(projet->list_gtk._1993_1_1_materiaux.window));
        if (projet->list_gtk._1993_1_1_materiaux.materiau == materiau)
            return TRUE;
    }
    else
    {
        ef_gtk->builder = gtk_builder_new();
        BUGMSG(gtk_builder_add_from_resource(ef_gtk->builder, "/org/2lgc/codegui/ui/1993_1_1_materiaux.ui", NULL) != 0, FALSE, gettext("Builder Failed\n"));
        gtk_builder_connect_signals(ef_gtk->builder, projet);
        ef_gtk->window = GTK_WIDGET(gtk_builder_get_object(ef_gtk->builder, "_1993_1_1_materiaux_window"));
    }
    
    if (materiau == NULL)
    {
        gtk_window_set_title(GTK_WINDOW(ef_gtk->window), gettext("Ajout d'un matériau acier"));
        ef_gtk->materiau = NULL;
        acier_data = NULL;
        gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(gtk_builder_get_object(ef_gtk->builder, "_1993_1_1_materiaux_textview_nom"))), "", -1);
        gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(gtk_builder_get_object(ef_gtk->builder, "_1993_1_1_materiaux_textview_fy"))), "", -1);
        gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(gtk_builder_get_object(ef_gtk->builder, "_1993_1_1_materiaux_textview_fu"))), "", -1);
        
        gtk_button_set_label(GTK_BUTTON(gtk_builder_get_object(ef_gtk->builder, "_1993_1_1_materiaux_button_add_edit")), "gtk-add");
        g_signal_connect(gtk_builder_get_object(ef_gtk->builder, "_1993_1_1_materiaux_button_add_edit"), "clicked", G_CALLBACK(_1993_1_1_gtk_materiaux_ajouter_clicked), projet);
        _1993_1_1_gtk_materiaux_check(NULL, projet);
    }
    else
    {
        gchar           tmp[30];
        
        BUGMSG(materiau->type == MATERIAU_ACIER, FALSE, gettext("Le matériau n'est pas en acier.\n"));
        gtk_window_set_title(GTK_WINDOW(ef_gtk->window), gettext("Modification d'un matériau acier"));
        ef_gtk->materiau = materiau;
        acier_data = materiau->data;
        
        gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(gtk_builder_get_object(ef_gtk->builder, "_1993_1_1_materiaux_textview_nom"))), materiau->nom, -1);
        common_math_double_to_char2(common_math_f(common_math_get(acier_data->fy)/1000000., acier_data->fy.type), tmp, DECIMAL_CONTRAINTE);
        gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder, "_1993_1_1_materiaux_buffer_fy")), tmp, -1);
        common_math_double_to_char2(common_math_f(common_math_get(acier_data->fu)/1000000., acier_data->fu.type), tmp, DECIMAL_CONTRAINTE);
        gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder, "_1993_1_1_materiaux_buffer_fu")), tmp, -1);
        
        gtk_button_set_label(GTK_BUTTON(gtk_builder_get_object(ef_gtk->builder, "_1993_1_1_materiaux_button_add_edit")), "gtk-edit");
        g_signal_connect(gtk_builder_get_object(ef_gtk->builder, "_1993_1_1_materiaux_button_add_edit"), "clicked", G_CALLBACK(_1993_1_1_gtk_materiaux_modifier_clicked), projet);
    }
    
    // On affiche les propriétés qui ne sont pas égale à celle par défaut.
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(ef_gtk->builder, "_1993_1_1_materiaux_personnaliser_menu_e")), acier_data && !ERREUR_RELATIVE_EGALE(common_math_get(acier_data->e), MODULE_YOUNG_ACIER));
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(ef_gtk->builder, "_1993_1_1_materiaux_personnaliser_menu_nu")), acier_data && !ERREUR_RELATIVE_EGALE(common_math_get(acier_data->nu), COEFFICIENT_NU_ACIER));
    
    gtk_window_set_transient_for(GTK_WINDOW(ef_gtk->window), GTK_WINDOW(projet->list_gtk.comp.window));
    
    return TRUE;
}


void _1993_1_1_gtk_materiaux_ajout(GtkMenuItem *menuitem, Projet *projet)
/* Description : Lance la fenêtre permettant d'ajouter un matériau acier.
 * Paramètres : GtkMenuItem *menuitem : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 *   Echec : projet == NULL,
 *           interface graphique non initialisée.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_materiaux.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Matériaux");
    
    BUG(_1993_1_1_gtk_materiaux(projet, NULL), );
}


#endif
