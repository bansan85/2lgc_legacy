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
#include <string.h>
#include <gtk/gtk.h>
#include <math.h>

#include "common_projet.h"
#include "common_erreurs.h"
#include "common_gtk.h"
#include "common_selection.h"
#include "common_math.h"
#include "EF_charge.h"
#include "EF_charge_barre_repartie_uniforme.h"

void EF_gtk_charge_barre_rep_uni_window_destroy(GtkWidget *object, Projet *projet)
/* Description : Met projet->list_gtk.ef_charge_barre_repartie_uniforme.builder à NULL quand la
 *               fenêtre se ferme.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_charge_barre_repartie_uniforme.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Charge Barre Répartie Uniforme");
    
    g_object_unref(G_OBJECT(projet->list_gtk.ef_charge_barre_repartie_uniforme.builder));
    projet->list_gtk.ef_charge_barre_repartie_uniforme.builder = NULL;
    
    return;
}


gboolean EF_gtk_charge_barre_rep_uni_window_key_press(GtkWidget *widget, GdkEvent *event,
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
    BUGMSG(projet->list_gtk.ef_charge_barre_repartie_uniforme.builder, FALSE, gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Charge Barre Répartie Uniforme");
    
    if (event->key.keyval == GDK_KEY_Escape)
    {
        gtk_widget_destroy(projet->list_gtk.ef_charge_barre_repartie_uniforme.window);
        return TRUE;
    }
    else
        return FALSE;
}


void EF_gtk_charge_barre_repartie_uniforme_annuler_clicked(GtkButton *button, Projet *projet)
/* Description : Ferme la fenêtre sans effectuer les modifications.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_charge_barre_repartie_uniforme.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Charge Barre Répartie Uniforme");
    
    gtk_widget_destroy(projet->list_gtk.ef_charge_barre_repartie_uniforme.window);
    
    return;
}


gboolean EF_gtk_charge_barre_repartie_uniforme_recupere_donnees(Projet *projet,
  unsigned int *num_action, GList **barres, double *fx, double *fy, double *fz, double *mx,
  double *my, double *mz, gchar **nom, gboolean *repere_local, gboolean *projection,
  double *a, double *b)
/* Description : Récupère toutes les données de la fenêtre permettant d'ajouter ou d'éditer une
 *               charge répartie uniformément.
 * Paramètres : Projet *projet : la variable projet,
 *            : unsigned int *num_action : numéro de l'action où sera ajoutée la charge,
 *            : GList **barres : liste des barres qui supportera la charge,
 *            : double *fx : force selon x,
 *            : double *fy : force selon y,
 *            : double *fz : force selon z,
 *            : double *mx : moment selon x,
 *            : double *my : moment selon y,
 *            : double *mz : moment selon z,
 *            : gchar **nom : nom de l'action,
 *            : gboolean *repere_local : si utilisation du repère local,
 *            : gboolean *projection : si utilisatation d'un projection sur la barre,
 *            : double *a : début de la charge par rapport au début de la barre,
 *            : double *b : fin de la charge par rapport à la fin de la charge.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL, num_action == NULL, barres == NULL, fx == NULL, fy == NULL,
 *             fz == NULL, mx == NULL, my == NULL, mz == NULL, nom == NULL,
 *             repere_local == NULL, projection == NULL, a == NULL, b == NULL,
 *             en cas d'erreur d'allocation mémoire.
 */
{
    Gtk_EF_Charge_Barre_Repartie_Uniforme   *ef_gtk;
    GList                                   *num_barres;
    GtkTextIter                             start, end;
    gchar                                   *texte_tmp;
    GtkTextBuffer                           *textbuffer;
    gboolean                                ok = TRUE;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_charge_barre_repartie_uniforme.builder, FALSE, gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Charge Barre Répartie Uniforme");
    
    ef_gtk = &projet->list_gtk.ef_charge_barre_repartie_uniforme;
    
    if (gtk_combo_box_get_active(GTK_COMBO_BOX(ef_gtk->combobox_charge)) < 0)
        ok = FALSE;
    else
        *num_action = (unsigned int)gtk_combo_box_get_active(GTK_COMBO_BOX(ef_gtk->combobox_charge));
    
    *fx = common_gtk_text_buffer_double(GTK_TEXT_BUFFER(gtk_builder_get_object(projet->list_gtk.ef_charge_barre_repartie_uniforme.builder, "EF_charge_barre_rep_uni_buffer_fx")), -INFINITY, FALSE, INFINITY, FALSE);
    if (isnan(*fx))
        ok = FALSE;
    
    *fy = common_gtk_text_buffer_double(GTK_TEXT_BUFFER(gtk_builder_get_object(projet->list_gtk.ef_charge_barre_repartie_uniforme.builder, "EF_charge_barre_rep_uni_buffer_fy")), -INFINITY, FALSE, INFINITY, FALSE);
    if (isnan(*fy))
        ok = FALSE;
    
    *fz = common_gtk_text_buffer_double(GTK_TEXT_BUFFER(gtk_builder_get_object(projet->list_gtk.ef_charge_barre_repartie_uniforme.builder, "EF_charge_barre_rep_uni_buffer_fz")), -INFINITY, FALSE, INFINITY, FALSE);
    if (isnan(*fz))
        ok = FALSE;
    
    *mx = common_gtk_text_buffer_double(GTK_TEXT_BUFFER(gtk_builder_get_object(projet->list_gtk.ef_charge_barre_repartie_uniforme.builder, "EF_charge_barre_rep_uni_buffer_mx")), -INFINITY, FALSE, INFINITY, FALSE);
    if (isnan(*mx))
        ok = FALSE;
    
    *my = common_gtk_text_buffer_double(GTK_TEXT_BUFFER(gtk_builder_get_object(projet->list_gtk.ef_charge_barre_repartie_uniforme.builder, "EF_charge_barre_rep_uni_buffer_my")), -INFINITY, FALSE, INFINITY, FALSE);
    if (isnan(*my))
        ok = FALSE;
    
    *mz = common_gtk_text_buffer_double(GTK_TEXT_BUFFER(gtk_builder_get_object(projet->list_gtk.ef_charge_barre_repartie_uniforme.builder, "EF_charge_barre_rep_uni_buffer_mz")), -INFINITY, FALSE, INFINITY, FALSE);
    if (isnan(*mz))
        ok = FALSE;
    
    *a = common_gtk_text_buffer_double(GTK_TEXT_BUFFER(gtk_builder_get_object(projet->list_gtk.ef_charge_barre_repartie_uniforme.builder, "EF_charge_barre_rep_uni_buffer_a")), 0, TRUE, INFINITY, FALSE);
    if (isnan(*a))
        ok = FALSE;
    
    *b = common_gtk_text_buffer_double(GTK_TEXT_BUFFER(gtk_builder_get_object(projet->list_gtk.ef_charge_barre_repartie_uniforme.builder, "EF_charge_barre_rep_uni_buffer_b")), 0, TRUE, INFINITY, FALSE);
    if (isnan(*b))
        ok = FALSE;
    
    *repere_local = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_rep_uni_radio_local")));
    
    *projection = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_rep_uni_check_projection")));
    
    textbuffer = GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_rep_uni_buffer_barres"));
    gtk_text_buffer_get_iter_at_offset(textbuffer, &start, 0);
    gtk_text_buffer_get_iter_at_offset(textbuffer, &end, -1);
    texte_tmp = gtk_text_buffer_get_text(textbuffer, &start, &end, FALSE);
    num_barres = common_selection_renvoie_numeros(texte_tmp);
    if (num_barres == NULL)
        ok = FALSE;
    else
    {
        *barres = common_selection_converti_numeros_en_barres(num_barres, projet);
        if (*barres == NULL)
            ok = FALSE;
        else
        {
            // Si tous les paramètres sont corrects
            textbuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_rep_uni_textview_description")));
            
            gtk_text_buffer_get_iter_at_offset(textbuffer, &start, 0);
            gtk_text_buffer_get_iter_at_offset(textbuffer, &end, -1);
            *nom = gtk_text_buffer_get_text(textbuffer, &start, &end, FALSE);
            
            if (strcmp(*nom, "") == 0)
            {
                free(*nom);
                ok = FALSE;
            }
        }
    }
    
    free(texte_tmp);
    
    return ok;
}


void EF_gtk_charge_barre_rep_uni_check(GtkWidget *object, Projet *projet)
/* Description : Vérifie si l'ensemble des éléments est correct pour activer le bouton add/edit.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    unsigned int    num_action;
    GList           *barres;
    double          fx, fy, fz, mx, my, mz;
    gchar           *nom;
    gboolean        repere_local, projection;
    double          a, b;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_charge_barre_repartie_uniforme.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Charge Barre Répartie Uniforme");
    
    if (!EF_gtk_charge_barre_repartie_uniforme_recupere_donnees(projet, &num_action, &barres, &fx, &fy, &fz, &mx, &my, &mz, &nom, &repere_local, &projection, &a, &b))
        gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_charge_barre_repartie_uniforme.builder, "EF_charge_barre_rep_uni_button_add_edit")), FALSE);
    else
    {
        gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_charge_barre_repartie_uniforme.builder, "EF_charge_barre_rep_uni_button_add_edit")), TRUE);
        free(nom);
        g_list_free(barres);
    }
    return;
}


void EF_gtk_charge_barre_repartie_uniforme_ajouter_clicked(GtkButton *button, Projet *projet)
/* Description : Ferme la fenêtre en ajoutant la charge.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    double                          fx, fy, fz, mx, my, mz, a, b;
    unsigned int                    num_action;
    gboolean                        repere_local, projection;
    GList                           *barres;
    gchar                           *texte;
    Charge_Barre_Repartie_Uniforme  *charge;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_charge_barre_repartie_uniforme.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Charge Barre Répartie Uniforme");
    
    BUG(EF_gtk_charge_barre_repartie_uniforme_recupere_donnees(projet, &num_action, &barres, &fx, &fy, &fz, &mx, &my, &mz, &texte, &repere_local, &projection, &a, &b) == TRUE, );
    
    // Création de la nouvelle charge ponctuelle sur barre
    BUG(charge = EF_charge_barre_repartie_uniforme_ajout(projet, num_action, barres, repere_local, projection, a, b, fx, fy, fz, mx, my, mz, texte), );
    
    free(texte);
    
    gtk_widget_destroy(projet->list_gtk.ef_charge_barre_repartie_uniforme.window);
    
    return;
}


void EF_gtk_charge_barre_repartie_uniforme_editer_clicked(GtkButton *button, Projet *projet)
/* Description : Ferme la fenêtre en appliquant les modifications.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    Gtk_EF_Charge_Barre_Repartie_Uniforme   *ef_gtk;
    double                                  fx, fy, fz, mx, my, mz, a, b;
    unsigned int                            num_action;
    gboolean                                repere_local, projection;
    GList                                   *barres;
    gchar                                   *texte;
    Charge_Barre_Repartie_Uniforme          *charge;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_charge_barre_repartie_uniforme.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Charge Barre Répartie Uniforme");
    
    ef_gtk = &projet->list_gtk.ef_charge_barre_repartie_uniforme;
    
    BUG(EF_gtk_charge_barre_repartie_uniforme_recupere_donnees(projet, &num_action, &barres, &fx, &fy, &fz, &mx, &my, &mz, &texte, &repere_local, &projection, &a, &b) == TRUE, );
    
    // Création de la nouvelle charge ponctuelle sur barre
    BUG(charge = EF_charge_cherche(projet, ef_gtk->action, ef_gtk->charge), );
    free(charge->nom);
    charge->nom = texte;
    g_list_free(charge->barres);
    charge->barres = barres;
    charge->repere_local = repere_local;
    charge->projection = projection;
    charge->a = a;
    charge->b = b;
    charge->fx = fx;
    charge->fy = fy;
    charge->fz = fz;
    charge->mx = mx;
    charge->my = my;
    charge->mz = mz;
    if (num_action != ef_gtk->action)
        BUG(EF_charge_deplace(projet, ef_gtk->action, ef_gtk->charge, num_action), );
    else
        gtk_widget_queue_draw(GTK_WIDGET(projet->list_gtk._1990_actions.tree_view_charges));
    
    gtk_widget_destroy(projet->list_gtk.ef_charge_barre_repartie_uniforme.window);
    
    return;
}


void EF_gtk_charge_barre_repartie_uniforme_toggled( GtkToggleButton *togglebutton,
  Projet *projet)
/* Description : Évènement lors du cochage de projection et repère globale. En effet, il n'est
 *               pas possible de faire à la fois une projection dans le repère local.
 * Paramètres : GtkToggleButton *togglebutton : composant à l'origine de l'évènement,
 *              Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    Gtk_EF_Charge_Barre_Repartie_Uniforme  *ef_gtk;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_charge_barre_repartie_uniforme.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Charge Barre Répartie Uniforme");
    
    ef_gtk = &projet->list_gtk.ef_charge_barre_repartie_uniforme;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ef_gtk->check_button_repere_local)))
    {
        gtk_widget_set_sensitive(ef_gtk->check_button_projection, FALSE);
        gtk_widget_set_sensitive(ef_gtk->check_button_repere_local, TRUE);
        gtk_widget_set_sensitive(ef_gtk->check_button_repere_global, TRUE);
    }
    else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ef_gtk->check_button_projection)))
    {
        gtk_widget_set_sensitive(ef_gtk->check_button_projection, TRUE);
        gtk_widget_set_sensitive(ef_gtk->check_button_repere_global, TRUE);
        gtk_widget_set_sensitive(ef_gtk->check_button_repere_local, FALSE);
    }
    else
    {
        gtk_widget_set_sensitive(ef_gtk->check_button_projection, TRUE);
        gtk_widget_set_sensitive(ef_gtk->check_button_repere_local, TRUE);
        gtk_widget_set_sensitive(ef_gtk->check_button_repere_global, TRUE);
    }
}


gboolean EF_gtk_charge_barre_repartie_uniforme(Projet *projet, unsigned int action_defaut,
  unsigned int charge)
/* Description : Affichage de la fenêtre permettant de créer ou modifier une action de type
 *               charge repartie uniforme sur barre.
 * Paramètres : Projet *projet : la variable projet,
 *              unsigned int action_defaut : action par défaut dans la fenêtre,
 *              unsigned int charge : vaut G_MAXUINT si une nouvelle charge doit être ajoutée,
 *                                    vaut le numéro de la charge si elle doit être modifiée.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Echec : FALSE :
 *             projet == NULL,
 *             Fenêtre graphique déjà initialisée.
 */
{
    Gtk_EF_Charge_Barre_Repartie_Uniforme   *ef_gtk;
    Charge_Barre_Repartie_Uniforme          *charge_barre;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    if (projet->list_gtk.ef_charge_barre_repartie_uniforme.builder != NULL)
    {
        gtk_window_present(GTK_WINDOW(projet->list_gtk.ef_charge_barre_repartie_uniforme.window));
        return TRUE;
    }
    
    ef_gtk = &projet->list_gtk.ef_charge_barre_repartie_uniforme;
    ef_gtk->builder = gtk_builder_new();
    BUGMSG(gtk_builder_add_from_file(ef_gtk->builder, DATADIR"/ui/EF_charge_barre_repartie_uniforme.ui", NULL) != 0, FALSE, gettext("Builder Failed\n"));
    gtk_builder_connect_signals(ef_gtk->builder, projet);
    
    ef_gtk->window = GTK_WIDGET(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_rep_uni_window"));
    ef_gtk->combobox_charge = GTK_WIDGET(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_rep_uni_combo_box_charge"));
    ef_gtk->check_button_repere_local = GTK_WIDGET(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_rep_uni_radio_local"));
    ef_gtk->check_button_repere_global = GTK_WIDGET(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_rep_uni_radio_global"));
    ef_gtk->check_button_projection = GTK_WIDGET(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_rep_uni_check_projection"));
    
    if (charge == G_MAXUINT)
    {
        ef_gtk->action = 0;
        ef_gtk->charge = 0;
        gtk_window_set_title(GTK_WINDOW(ef_gtk->window), gettext("Ajout d'une charge répartie uniforme sur barre"));
        charge_barre = NULL;
    }
    else
    {
        ef_gtk->action = action_defaut;
        ef_gtk->charge = charge;
        gtk_window_set_title(GTK_WINDOW(ef_gtk->window), gettext("Modification d'une charge répartie uniforme sur barre"));
        BUG(charge_barre = EF_charge_cherche(projet, action_defaut, charge), FALSE);
    }
    
    gtk_combo_box_set_model(GTK_COMBO_BOX(ef_gtk->combobox_charge), GTK_TREE_MODEL(projet->list_gtk._1990_actions.list_actions));
    gtk_combo_box_set_active(GTK_COMBO_BOX(ef_gtk->combobox_charge), (gint)action_defaut);
    
    if (charge_barre != NULL)
    {
        gchar   tmp[30], *tmp2;
        gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_rep_uni_textview_description"))), charge_barre->nom, -1);
        common_math_double_to_char(charge_barre->fx, tmp, DECIMAL_FORCE);
        gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_rep_uni_buffer_fx")), tmp, -1);
        common_math_double_to_char(charge_barre->fy, tmp, DECIMAL_FORCE);
        gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_rep_uni_buffer_fy")), tmp, -1);
        common_math_double_to_char(charge_barre->fz, tmp, DECIMAL_FORCE);
        gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_rep_uni_buffer_fz")), tmp, -1);
        common_math_double_to_char(charge_barre->mx, tmp, DECIMAL_MOMENT);
        gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_rep_uni_buffer_mx")), tmp, -1);
        common_math_double_to_char(charge_barre->my, tmp, DECIMAL_MOMENT);
        gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_rep_uni_buffer_my")), tmp, -1);
        common_math_double_to_char(charge_barre->mz, tmp, DECIMAL_MOMENT);
        gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_rep_uni_buffer_mz")), tmp, -1);
        common_math_double_to_char(charge_barre->a, tmp, DECIMAL_DISTANCE);
        gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_rep_uni_buffer_a")), tmp, -1);
        common_math_double_to_char(charge_barre->b, tmp, DECIMAL_DISTANCE);
        gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_rep_uni_buffer_b")), tmp, -1);
        if ((charge_barre->repere_local) && (charge_barre->projection))
        {
            GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(ef_gtk->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, gettext("Il n'est pas possible d'activer à la fois la projection et l'utilisation du repère local."));
            gtk_dialog_run(GTK_DIALOG(dialog));
            gtk_widget_destroy(dialog);
            return -1;
        }
        if (charge_barre->repere_local)
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ef_gtk->check_button_repere_local), TRUE);
        else
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ef_gtk->check_button_repere_global), TRUE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ef_gtk->check_button_projection), charge_barre->projection);
        BUG(tmp2 = common_selection_converti_barres_en_texte(charge_barre->barres), FALSE);
        gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_rep_uni_buffer_barres")), tmp2, -1);
        free(tmp2);
    }
    
    if (charge == G_MAXUINT)
    {
        gtk_button_set_label(GTK_BUTTON(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_rep_uni_button_add_edit")), "gtk-add");
        g_signal_connect(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_rep_uni_button_add_edit"), "clicked", G_CALLBACK(EF_gtk_charge_barre_repartie_uniforme_ajouter_clicked), projet);
    }
    else
    {
        gtk_button_set_label(GTK_BUTTON(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_rep_uni_button_add_edit")), "gtk-edit");
        g_signal_connect(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_rep_uni_button_add_edit"), "clicked", G_CALLBACK(EF_gtk_charge_barre_repartie_uniforme_editer_clicked), projet);
    }
    
    EF_gtk_charge_barre_rep_uni_check(NULL, projet);
    
    if (projet->list_gtk._1990_actions.window == NULL)
        gtk_window_set_transient_for(GTK_WINDOW(ef_gtk->window), GTK_WINDOW(projet->list_gtk.comp.window));
    else
        gtk_window_set_transient_for(GTK_WINDOW(ef_gtk->window), GTK_WINDOW(projet->list_gtk._1990_actions.window));
    
    return TRUE;
}


#endif
