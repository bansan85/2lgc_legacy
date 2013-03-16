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
#include "1992_1_1_materiaux.h"

G_MODULE_EXPORT gboolean _1992_1_1_gtk_materiaux_window_key_press(
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
    BUGMSG(projet->list_gtk._1992_1_1_materiaux.builder, FALSE, gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Ajout Matériau Béton");
    
    if (event->key.keyval == GDK_KEY_Escape)
    {
        gtk_widget_destroy(projet->list_gtk._1992_1_1_materiaux.window);
        return TRUE;
    }
    else
        return FALSE;
}


G_MODULE_EXPORT void _1992_1_1_gtk_materiaux_window_destroy(
  GtkWidget *object __attribute__((unused)), Projet *projet)
/* Description : Met projet->list_gtk._1992_1_1_materiaux.builder à NULL quand la
 *               fenêtre se ferme.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk._1992_1_1_materiaux.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Ajout Matériau Béton");
    
    g_object_unref(G_OBJECT(projet->list_gtk._1992_1_1_materiaux.builder));
    projet->list_gtk._1992_1_1_materiaux.builder = NULL;
    
    return;
}


gboolean _1992_1_1_gtk_materiaux_recupere_donnees(Projet *projet, double *fck,
  gchar **nom)
/* Description : Récupère toutes les données de la fenêtre permettant d'ajouter ou d'éditer un
 *               matériau de type béton.
 * Paramètres : Projet *projet : la variable projet,
 *            : double *fck : le paramètre fck du matériau,
 *            : gchar **nom : le nom du matériau,
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
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(fck, FALSE, gettext("Paramètre %s incorrect.\n"), "fck");
    BUGMSG(nom, FALSE, gettext("Paramètre %s incorrect.\n"), "nom");
    BUGMSG(projet->list_gtk._1992_1_1_materiaux.builder, FALSE, gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Ajout Matériau Béton");
    
    *fck = common_gtk_text_buffer_double(GTK_TEXT_BUFFER(gtk_builder_get_object(projet->list_gtk._1992_1_1_materiaux.builder, "_1992_1_1_materiaux_beton_buffer_fck")), 0., FALSE, 90., TRUE);
    if (isnan(*fck))
        ok = FALSE;
    
    // Si tous les paramètres sont corrects
    textbuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(gtk_builder_get_object(projet->list_gtk._1992_1_1_materiaux.builder, "_1992_1_1_materiaux_beton_textview_nom")));
    
    gtk_text_buffer_get_iter_at_offset(textbuffer, &start, 0);
    gtk_text_buffer_get_iter_at_offset(textbuffer, &end, -1);
    *nom = gtk_text_buffer_get_text(textbuffer, &start, &end, FALSE);
    
    gtk_text_buffer_remove_all_tags(textbuffer, &start, &end);
    
    if (projet->list_gtk._1992_1_1_materiaux.materiau == NULL)
    {
        if ((strcmp(*nom, "") == 0) || (_1992_1_1_materiaux_cherche_nom(projet, *nom, FALSE)))
        {
            gtk_text_buffer_apply_tag_by_name(textbuffer, "mauvais", &start, &end);
            ok = FALSE;
        }
        else
            gtk_text_buffer_apply_tag_by_name(textbuffer, "OK", &start, &end);
    }
    else if ((strcmp(*nom, "") == 0) || 
      ((strcmp(projet->list_gtk._1992_1_1_materiaux.materiau->nom, *nom) != 0) && (_1992_1_1_materiaux_cherche_nom(projet, *nom, FALSE))))
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


G_MODULE_EXPORT void _1992_1_1_gtk_materiaux_check(
  GtkWidget *object __attribute__((unused)), Projet *projet)
/* Description : Vérifie si l'ensemble des éléments est correct pour activer le bouton add/edit.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    double  fck;
    char    *nom;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk._1992_1_1_materiaux.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Ajout Matériau Béton");
    
    if (!_1992_1_1_gtk_materiaux_recupere_donnees(projet, &fck, &nom))
        gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk._1992_1_1_materiaux.builder, "_1992_1_1_materiaux_beton_button_add_edit")), FALSE);
    else
    {
        gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk._1992_1_1_materiaux.builder, "_1992_1_1_materiaux_beton_button_add_edit")), TRUE);
        free(nom);
    }
    
    return;
}


G_MODULE_EXPORT void _1992_1_1_gtk_materiaux_ajouter_clicked(
  GtkButton *button __attribute__((unused)), Projet *projet)
/* Description : Ferme la fenêtre en ajoutant la charge.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    double  fck;
    gchar   *texte;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk._1992_1_1_materiaux.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Ajout Matériau Béton");
    
    if (!(_1992_1_1_gtk_materiaux_recupere_donnees(projet, &fck, &texte)))
        return;
    
    // Création de la nouvelle charge ponctuelle au noeud
    BUG(_1992_1_1_materiaux_ajout(projet, texte, fck), );
    
    free(texte);
    
    gtk_widget_destroy(projet->list_gtk._1992_1_1_materiaux.window);
    
    return;
}


G_MODULE_EXPORT void _1992_1_1_gtk_materiaux_annuler_clicked(
  GtkButton *button __attribute__((unused)), Projet *projet)
/* Description : Ferme la fenêtre sans effectuer les modifications.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk._1992_1_1_materiaux.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Ajout Matériau Béton");
    
    gtk_widget_destroy(projet->list_gtk._1992_1_1_materiaux.window);
    
    return;
}


G_MODULE_EXPORT void _1992_1_1_gtk_materiaux_modifier_clicked(
  GtkButton *button __attribute__((unused)), Projet *projet)
/* Description : Ferme la fenêtre en appliquant les modifications.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    Gtk_1992_1_1_Materiaux  *ef_gtk;
    
    double      fck;
    gchar       *texte;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk._1992_1_1_materiaux.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Ajout Matériau Béton");
    
    ef_gtk = &projet->list_gtk._1992_1_1_materiaux;
    
    if (!(_1992_1_1_gtk_materiaux_recupere_donnees(projet, &fck, &texte)))
        return;
    
    BUG(_1992_1_1_materiaux_modif(projet, ef_gtk->materiau, texte, fck*1000000., _1992_1_1_materiaux_fckcube(fck), _1992_1_1_materiaux_fcm(fck), _1992_1_1_materiaux_fctm(fck), _1992_1_1_materiaux_fctk_0_05(fck), _1992_1_1_materiaux_fctk_0_95(fck), _1992_1_1_materiaux_ecm(fck), _1992_1_1_materiaux_ec1(fck), _1992_1_1_materiaux_ecu1(fck), _1992_1_1_materiaux_ec2(fck), _1992_1_1_materiaux_ecu2(fck), _1992_1_1_materiaux_n(fck), _1992_1_1_materiaux_ec3(fck), _1992_1_1_materiaux_ecu3(fck), COEFFICIENT_NU_BETON, _1992_1_1_materiaux_gnu(fck, COEFFICIENT_NU_BETON), _1992_1_1_materiaux_gnu(fck, 0)), );
    
    gtk_widget_destroy(projet->list_gtk._1992_1_1_materiaux.window);
    
    return;
}


G_MODULE_EXPORT void _1992_1_1_gtk_materiaux_toggled(GtkCheckMenuItem *checkmenuitem,
  Projet *projet)
/* Description : Evènement pour (dés)activer la personnalisation d'une propriété du béton.
 * Paramètres : GtkCheckMenuItem *checkmenuitem : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    GtkBuilder      *builder;
    gboolean        check = gtk_check_menu_item_get_active(checkmenuitem);
    Beton_Materiau  *mat;
    char            tmp[30];
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    
    builder = projet->list_gtk._1992_1_1_materiaux.builder;
    mat = projet->list_gtk._1992_1_1_materiaux.materiau;
    
    if (checkmenuitem == GTK_CHECK_MENU_ITEM(gtk_builder_get_object(builder, "_1992_1_1_materiaux_personnaliser_menu_fck_cube")))
    {
        if (check)
        {
            common_math_double_to_char(mat->fckcube/1000000., tmp, DECIMAL_CONTRAINTE);
            gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gtk_builder_get_object(builder, "_1992_1_1_materiaux_beton_buffer_fck_cube")), tmp, -1);
        }
        
        gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(builder, "_1992_1_1_materiaux_beton_label_fck_cube")), check);
        gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(builder, "_1992_1_1_materiaux_beton_scrolledwindow_fck_cube")), check);
    }
    else if (checkmenuitem == GTK_CHECK_MENU_ITEM(gtk_builder_get_object(builder, "_1992_1_1_materiaux_personnaliser_menu_fcm")))
    {
        if (check)
        {
            common_math_double_to_char(mat->fcm/1000000., tmp, DECIMAL_CONTRAINTE);
            gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gtk_builder_get_object(builder, "_1992_1_1_materiaux_beton_buffer_fcm")), tmp, -1);
        }
        gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(builder, "_1992_1_1_materiaux_beton_label_fcm")), check);
        gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(builder, "_1992_1_1_materiaux_beton_scrolledwindow_fcm")), check);
    }
    else if (checkmenuitem == GTK_CHECK_MENU_ITEM(gtk_builder_get_object(builder, "_1992_1_1_materiaux_personnaliser_menu_fctm")))
    {
        if (check)
        {
            common_math_double_to_char(mat->fctm/1000000., tmp, DECIMAL_CONTRAINTE);
            gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gtk_builder_get_object(builder, "_1992_1_1_materiaux_beton_buffer_fctm")), tmp, -1);
        }
        gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(builder, "_1992_1_1_materiaux_beton_label_fctm")), check);
        gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(builder, "_1992_1_1_materiaux_beton_scrolledwindow_fctm")), check);
    }
    else if (checkmenuitem == GTK_CHECK_MENU_ITEM(gtk_builder_get_object(builder, "_1992_1_1_materiaux_personnaliser_menu_fctk_0_05")))
    {
        if (check)
        {
            common_math_double_to_char(mat->fctk_0_05/1000000., tmp, DECIMAL_CONTRAINTE);
            gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gtk_builder_get_object(builder, "_1992_1_1_materiaux_beton_buffer_fctk_0_05")), tmp, -1);
        }
        gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(builder, "_1992_1_1_materiaux_beton_label_fctk_0_05")), check);
        gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(builder, "_1992_1_1_materiaux_beton_scrolledwindow_fctk_0_05")), check);
    }
    else if (checkmenuitem == GTK_CHECK_MENU_ITEM(gtk_builder_get_object(builder, "_1992_1_1_materiaux_personnaliser_menu_fctk_0_95")))
    {
        if (check)
        {
            common_math_double_to_char(mat->fctk_0_95/1000000., tmp, DECIMAL_CONTRAINTE);
            gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gtk_builder_get_object(builder, "_1992_1_1_materiaux_beton_buffer_fctk_0_95")), tmp, -1);
        }
        gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(builder, "_1992_1_1_materiaux_beton_label_fctk_0_95")), check);
        gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(builder, "_1992_1_1_materiaux_beton_scrolledwindow_fctk_0_95")), check);
    }
    else if (checkmenuitem == GTK_CHECK_MENU_ITEM(gtk_builder_get_object(builder, "_1992_1_1_materiaux_personnaliser_menu_Ecm")))
    {
        if (check)
        {
            common_math_double_to_char(mat->ecm/1000000000., tmp, DECIMAL_CONTRAINTE);
            gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gtk_builder_get_object(builder, "_1992_1_1_materiaux_beton_buffer_Ecm")), tmp, -1);
        }
        gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(builder, "_1992_1_1_materiaux_beton_label_Ecm")), check);
        gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(builder, "_1992_1_1_materiaux_beton_scrolledwindow_Ecm")), check);
    }
    else if (checkmenuitem == GTK_CHECK_MENU_ITEM(gtk_builder_get_object(builder, "_1992_1_1_materiaux_personnaliser_menu_ec1")))
    {
        if (check)
        {
            common_math_double_to_char(mat->ec1*1000, tmp, DECIMAL_SANS_UNITE);
            gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gtk_builder_get_object(builder, "_1992_1_1_materiaux_beton_buffer_ec1")), tmp, -1);
        }
        gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(builder, "_1992_1_1_materiaux_beton_label_ec1")), check);
        gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(builder, "_1992_1_1_materiaux_beton_scrolledwindow_ec1")), check);
    }
    else if (checkmenuitem == GTK_CHECK_MENU_ITEM(gtk_builder_get_object(builder, "_1992_1_1_materiaux_personnaliser_menu_ecu1")))
    {
        if (check)
        {
            common_math_double_to_char(mat->ecu1*1000, tmp, DECIMAL_SANS_UNITE);
            gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gtk_builder_get_object(builder, "_1992_1_1_materiaux_beton_buffer_ecu1")), tmp, -1);
        }
        gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(builder, "_1992_1_1_materiaux_beton_label_ecu1")), check);
        gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(builder, "_1992_1_1_materiaux_beton_scrolledwindow_ecu1")), check);
    }
    else if (checkmenuitem == GTK_CHECK_MENU_ITEM(gtk_builder_get_object(builder, "_1992_1_1_materiaux_personnaliser_menu_ec2")))
    {
        if (check)
        {
            common_math_double_to_char(mat->ec2*1000, tmp, DECIMAL_SANS_UNITE);
            gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gtk_builder_get_object(builder, "_1992_1_1_materiaux_beton_buffer_ec2")), tmp, -1);
        }
        gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(builder, "_1992_1_1_materiaux_beton_label_ec2")), check);
        gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(builder, "_1992_1_1_materiaux_beton_scrolledwindow_ec2")), check);
    }
    else if (checkmenuitem == GTK_CHECK_MENU_ITEM(gtk_builder_get_object(builder, "_1992_1_1_materiaux_personnaliser_menu_ecu2")))
    {
        if (check)
        {
            common_math_double_to_char(mat->ecu2*1000, tmp, DECIMAL_SANS_UNITE);
            gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gtk_builder_get_object(builder, "_1992_1_1_materiaux_beton_buffer_ecu2")), tmp, -1);
        }
        gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(builder, "_1992_1_1_materiaux_beton_label_ecu2")), check);
        gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(builder, "_1992_1_1_materiaux_beton_scrolledwindow_ecu2")), check);
    }
    else if (checkmenuitem == GTK_CHECK_MENU_ITEM(gtk_builder_get_object(builder, "_1992_1_1_materiaux_personnaliser_menu_n")))
    {
        if (check)
        {
            common_math_double_to_char(mat->n, tmp, DECIMAL_SANS_UNITE);
            gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gtk_builder_get_object(builder, "_1992_1_1_materiaux_beton_buffer_n")), tmp, -1);
        }
        gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(builder, "_1992_1_1_materiaux_beton_label_n")), check);
        gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(builder, "_1992_1_1_materiaux_beton_scrolledwindow_n")), check);
    }
    else if (checkmenuitem == GTK_CHECK_MENU_ITEM(gtk_builder_get_object(builder, "_1992_1_1_materiaux_personnaliser_menu_ec3")))
    {
        if (check)
        {
            common_math_double_to_char(mat->ec3*1000, tmp, DECIMAL_SANS_UNITE);
            gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gtk_builder_get_object(builder, "_1992_1_1_materiaux_beton_buffer_ec3")), tmp, -1);
        }
        gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(builder, "_1992_1_1_materiaux_beton_label_ec3")), check);
        gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(builder, "_1992_1_1_materiaux_beton_scrolledwindow_ec3")), check);
    }
    else if (checkmenuitem == GTK_CHECK_MENU_ITEM(gtk_builder_get_object(builder, "_1992_1_1_materiaux_personnaliser_menu_ecu3")))
    {
        if (check)
        {
            common_math_double_to_char(mat->ecu3*1000, tmp, DECIMAL_SANS_UNITE);
            gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gtk_builder_get_object(builder, "_1992_1_1_materiaux_beton_buffer_ecu3")), tmp, -1);
        }
        gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(builder, "_1992_1_1_materiaux_beton_label_ecu3")), check);
        gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(builder, "_1992_1_1_materiaux_beton_scrolledwindow_ecu3")), check);
    }
    else if (checkmenuitem == GTK_CHECK_MENU_ITEM(gtk_builder_get_object(builder, "_1992_1_1_materiaux_personnaliser_menu_nu")))
    {
        if (check)
        {
            common_math_double_to_char(mat->nu, tmp, DECIMAL_SANS_UNITE);
            gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gtk_builder_get_object(builder, "_1992_1_1_materiaux_beton_buffer_nu")), tmp, -1);
        }
        gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(builder, "_1992_1_1_materiaux_beton_label_nu")), check);
        gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(builder, "_1992_1_1_materiaux_beton_scrolledwindow_nu")), check);
    }
    else
        BUGMSG(NULL, , gettext("Paramètre %s incorrect.\n"), "checkmenuitem");
    
    return;
}


G_MODULE_EXPORT gboolean _1992_1_1_gtk_materiaux(Projet *projet, Beton_Materiau *materiau)
/* Description : Affichage de la fenêtre permettant de créer ou modifier un matériau de type
 *               béton.
 * Paramètres : Projet *projet : la variable projet,
 *            : Beton_Materiau *materiau : materiau à modifier. NULL si nouveau matériau,
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Echec : FALSE :
 *             projet == NULL,
 *             Fenêtre graphique déjà initialisée.
 */
{
    Gtk_1992_1_1_Materiaux  *ef_gtk;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    
    ef_gtk = &projet->list_gtk._1992_1_1_materiaux;
    if (projet->list_gtk._1992_1_1_materiaux.builder != NULL)
    {
        gtk_window_present(GTK_WINDOW(projet->list_gtk._1992_1_1_materiaux.window));
        if (projet->list_gtk._1992_1_1_materiaux.materiau == materiau)
            return TRUE;
    }
    else
    {
        ef_gtk->builder = gtk_builder_new();
        BUGMSG(gtk_builder_add_from_file(ef_gtk->builder, DATADIR"/ui/1992_1_1_materiaux_beton.ui", NULL) != 0, FALSE, gettext("Builder Failed\n"));
        gtk_builder_connect_signals(ef_gtk->builder, projet);
        ef_gtk->window = GTK_WIDGET(gtk_builder_get_object(ef_gtk->builder, "_1992_1_1_materiaux_beton_window"));
    }
    
    if (materiau == NULL)
    {
        gtk_window_set_title(GTK_WINDOW(ef_gtk->window), gettext("Ajout d'un matériau béton"));
        ef_gtk->materiau = NULL;
        
        gtk_button_set_label(GTK_BUTTON(gtk_builder_get_object(ef_gtk->builder, "_1992_1_1_materiaux_beton_button_add_edit")), "gtk-add");
        g_signal_connect(gtk_builder_get_object(ef_gtk->builder, "_1992_1_1_materiaux_beton_button_add_edit"), "clicked", G_CALLBACK(_1992_1_1_gtk_materiaux_ajouter_clicked), projet);
        _1992_1_1_gtk_materiaux_check(NULL, projet);
    }
    else
    {
        gchar           tmp[30];
        
        gtk_window_set_title(GTK_WINDOW(ef_gtk->window), gettext("Modification d'un matériau béton"));
        ef_gtk->materiau = materiau;
        
        gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(gtk_builder_get_object(ef_gtk->builder, "_1992_1_1_materiaux_beton_textview_nom"))), ef_gtk->materiau->nom, -1);
        common_math_double_to_char(ef_gtk->materiau->fck/1000000., tmp, DECIMAL_DISTANCE);
        gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder, "_1992_1_1_materiaux_beton_buffer_fck")), tmp, -1);
        
        gtk_button_set_label(GTK_BUTTON(gtk_builder_get_object(ef_gtk->builder, "_1992_1_1_materiaux_beton_button_add_edit")), "gtk-edit");
        g_signal_connect(gtk_builder_get_object(ef_gtk->builder, "_1992_1_1_materiaux_beton_button_add_edit"), "clicked", G_CALLBACK(_1992_1_1_gtk_materiaux_modifier_clicked), projet);
    }
    
    gtk_window_set_transient_for(GTK_WINDOW(ef_gtk->window), GTK_WINDOW(projet->list_gtk.comp.window));
    
    return TRUE;
}


G_MODULE_EXPORT void _1992_1_1_gtk_materiaux_ajout(
  GtkMenuItem *menuitem __attribute__((unused)), Projet *projet)
/* Description : Lance la fenêtre permettant d'ajouter un matériau béton.
 * Paramètres : GtkMenuItem *menuitem : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 *   Echec : projet == NULL,
 *           interface graphique non initialisée.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_materiaux.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Matériaux");
    
    BUG(_1992_1_1_gtk_materiaux(projet, NULL), );
}


G_MODULE_EXPORT void EF_gtk_materiaux_edit_clicked(GtkWidget *widget  __attribute__((unused)),
  Projet *projet)
/* Description : Edite les matériaux sélectionnés.
 * Paramètres : GtkToolButton *toolbutton : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 *   Echec : projet == NULL,
 *           interface graphique non initialisée.
 */
{
    GtkTreeIter     iter;
    GtkTreeModel    *model;
    char            *nom;
    GList           *list, *list_parcours;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_materiaux.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Matériaux");
    
    // On récupère la liste des matériaux à éditer.
    list = gtk_tree_selection_get_selected_rows(GTK_TREE_SELECTION(gtk_builder_get_object(projet->list_gtk.ef_materiaux.builder, "EF_materiaux_treeview_select")), &model);
    list_parcours = g_list_first(list);
    for(;list_parcours != NULL; list_parcours = g_list_next(list_parcours))
    {
        if (gtk_tree_model_get_iter(model, &iter, (GtkTreePath*)list_parcours->data))
        {
    // Et on les édite les unes après les autres.
            Beton_Materiau *materiau;
            
            gtk_tree_model_get(model, &iter, 0, &nom, -1);
            BUG(materiau = _1992_1_1_materiaux_cherche_nom(projet, nom, TRUE), );
            free(nom);
            
            BUG(_1992_1_1_gtk_materiaux(projet, materiau), );
        }
    }
    g_list_foreach(list, (GFunc)gtk_tree_path_free, NULL);
    g_list_free(list);
    
    return;
}


G_MODULE_EXPORT gboolean _1992_1_1_gtk_materiaux_double_clicked(GtkWidget *widget,
  GdkEvent *event, Projet *projet)
/* Description : Lance la fenêtre d'édition du matériau sélectionné en cas de double-clique
 *               dans le tree-view.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : GdkEvent *event : Information sur l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : TRUE s'il y a édition via un double-clique, FALSE sinon.
 *   Echec : FALSE :
 *             projet == NULL,
 *             interface graphique non initialisée.
 */
{
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_materiaux.builder, FALSE, gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Matériaux");
    
    if ((event->type == GDK_2BUTTON_PRESS) && (gtk_widget_get_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_materiaux.builder, "EF_materiaux_boutton_modifier")))))
    {
        EF_gtk_materiaux_edit_clicked(widget, projet);
        return TRUE;
    }
    else
        return common_gtk_treeview_button_press_unselect(GTK_TREE_VIEW(widget), event, projet);
}


#endif
