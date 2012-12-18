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
#include "common_maths.h"
#include "EF_charge.h"
#include "EF_charge_barre_repartie_uniforme.h"

G_MODULE_EXPORT void EF_gtk_charge_barre_rep_uni_window_destroy(
  GtkWidget *object __attribute__((unused)), Projet *projet)
/* Description : Met projet->list_gtk.ef_charge_barre_repartie_uniforme.builder à NULL quand la
 *               fenêtre se ferme.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_charge_barre_repartie_uniforme.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Charge Barre Répartie Uniforme");
    
    projet->list_gtk.ef_charge_barre_repartie_uniforme.builder = NULL;
    
    return;
}


G_MODULE_EXPORT gboolean EF_gtk_charge_barre_rep_uni_window_key_press(
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
    BUGMSG(projet->list_gtk.ef_charge_barre_repartie_uniforme.builder, FALSE, gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Charge Barre Répartie Uniforme");
    
    if (event->key.keyval == GDK_KEY_Escape)
    {
        gtk_widget_destroy(projet->list_gtk.ef_charge_barre_repartie_uniforme.window);
        return TRUE;
    }
    else
        return FALSE;
}


G_MODULE_EXPORT gboolean EF_gtk_charge_barre_repartie_uniforme_ajout_affichage(
  Charge_Barre_Repartie_Uniforme *charge, Projet *projet, gboolean nouvelle_ligne)
/* Description : Ajoute une ligne contenant les informations sur une charge répartie
 *               uniformément au tableau dans la fenêtre des actions.
 * Paramètres : Charge_Barre_Repartie_Uniforme *charge : charge à ajouter,
 *            : Projet *projet : la variable projet,
 *            : gboolean nouvelle_ligne : Ajoute une ligne ou modifie la ligne existante.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL,
 *             en cas d'erreur d'allocation mémoire.
 */
{
    char                *description, *txt_liste_barres, txt_debut[30], txt_fin[30];
    char                txt_fx[30], txt_fy[30], txt_fz[30], txt_mx[30], txt_my[30], txt_mz[30];
    Gtk_1990_Actions    *list_gtk_1990_actions;
    
    BUGMSG(projet, TRUE, gettext("Paramètre %s incorrect.\n"), "projet");
    
    list_gtk_1990_actions = &projet->list_gtk._1990_actions;
    
    if (list_gtk_1990_actions->builder == NULL)
        return TRUE;
    
    BUG(txt_liste_barres = common_selection_converti_barres_en_texte(charge->barres), FALSE);
    common_math_double_to_char(charge->a, txt_debut, DECIMAL_DISTANCE);
    common_math_double_to_char(charge->b, txt_fin, DECIMAL_DISTANCE);
    common_math_double_to_char(charge->fx, txt_fx, DECIMAL_FORCE);
    common_math_double_to_char(charge->fy, txt_fy, DECIMAL_FORCE);
    common_math_double_to_char(charge->fz, txt_fz, DECIMAL_FORCE);
    common_math_double_to_char(charge->mx, txt_mx, DECIMAL_MOMENT);
    common_math_double_to_char(charge->my, txt_my, DECIMAL_MOMENT);
    common_math_double_to_char(charge->mz, txt_mz, DECIMAL_MOMENT);
    
    BUGMSG(description = g_strdup_printf("%s : %s, %s : %s m, %s : %s m, %s, %s, Fx : %s N/m, Fy : %s N/m, Fz : %s N/m, Mx : %s N.m/m, My : %s N.m/m, Mz : %s N.m/m", strstr(txt_liste_barres, ";") == NULL ? gettext("Barre") : gettext("Barres"), txt_liste_barres, gettext("début"), txt_debut, gettext("fin (par rapport à la fin)"), txt_fin, charge->projection == TRUE ? gettext("projection : oui") : gettext("projection : non"), charge->repere_local ? gettext("repère : local") : gettext("repère : global"), txt_fx, txt_fy, txt_fz, txt_mx, txt_my, txt_mz), FALSE, gettext("Erreur d'allocation mémoire.\n"));
    
    free(txt_liste_barres);
    if (nouvelle_ligne == TRUE)
        gtk_tree_store_append(list_gtk_1990_actions->tree_store_charges, &charge->Iter, NULL);
    gtk_tree_store_set(list_gtk_1990_actions->tree_store_charges, &charge->Iter, 0, charge->numero, 1, charge->nom, 2, gettext("Répartie uniforme sur barre"), 3, description, -1);
    free(description);
    
    return TRUE;
}


/* DEBUT DE LA FENETRE GRAPHIQUE*/

G_MODULE_EXPORT void EF_gtk_charge_barre_repartie_uniforme_annuler_clicked(GtkButton *button __attribute__((unused)), Projet *projet)
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
    GtkWidget                               *dialog;
    Gtk_EF_Charge_Barre_Repartie_Uniforme   *ef_gtk;
    GList                                   *num_barres;
    GtkTextIter                             start, end;
    gchar                                   *texte_tmp;
    GtkTextBuffer                           *textbuffer;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_charge_barre_repartie_uniforme.builder, FALSE, gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Charge Barre Répartie Uniforme");
    
    ef_gtk = &projet->list_gtk.ef_charge_barre_repartie_uniforme;
    
    if (gtk_combo_box_get_active(GTK_COMBO_BOX(ef_gtk->combobox_charge)) < 0)
        return FALSE;
    else
        *num_action = (unsigned int)gtk_combo_box_get_active(GTK_COMBO_BOX(ef_gtk->combobox_charge));
    
    *fx = common_gtk_entry_renvoie_double(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_rep_uni_buffer_fx")));
    if (isnan(*fx))
    {
        dialog = gtk_message_dialog_new(GTK_WINDOW(ef_gtk->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, gettext("La valeur de %s est incorrecte."), "Fx");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return FALSE;
    }
    
    *fy = common_gtk_entry_renvoie_double(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_rep_uni_buffer_fy")));
    if (isnan(*fy))
    {
        dialog = gtk_message_dialog_new(GTK_WINDOW(ef_gtk->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, gettext("La valeur de %s est incorrecte."), "Fy");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return FALSE;
    }
    
    *fz = common_gtk_entry_renvoie_double(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_rep_uni_buffer_fz")));
    if (isnan(*fz))
    {
        dialog = gtk_message_dialog_new(GTK_WINDOW(ef_gtk->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, gettext("La valeur de %s est incorrecte."), "Fz");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return FALSE;
    }
    
    *mx = common_gtk_entry_renvoie_double(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_rep_uni_buffer_mx")));
    if (isnan(*mx))
    {
        dialog = gtk_message_dialog_new(GTK_WINDOW(ef_gtk->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, gettext("La valeur de %s est incorrecte."), "Mx");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return FALSE;
    }
    
    *my = common_gtk_entry_renvoie_double(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_rep_uni_buffer_my")));
    if (isnan(*my))
    {
        dialog = gtk_message_dialog_new(GTK_WINDOW(ef_gtk->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, gettext("La valeur de %s est incorrecte."), "My");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return FALSE;
    }
    
    *mz = common_gtk_entry_renvoie_double(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_rep_uni_buffer_mz")));
    if (isnan(*mz))
    {
        dialog = gtk_message_dialog_new(GTK_WINDOW(ef_gtk->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, gettext("La valeur de %s est incorrecte."), "Mz");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return FALSE;
    }
    
    *a = common_gtk_entry_renvoie_double(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_rep_uni_buffer_a")));
    if (isnan(*a))
    {
        dialog = gtk_message_dialog_new(GTK_WINDOW(ef_gtk->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, gettext("La valeur de a est incorrecte."));
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return FALSE;
    }
    
    *b = common_gtk_entry_renvoie_double(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_rep_uni_buffer_b")));
    if (isnan(*b))
    {
        dialog = gtk_message_dialog_new(GTK_WINDOW(ef_gtk->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, gettext("La valeur de b est incorrecte."));
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return FALSE;
    }
    
    *repere_local = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_rep_uni_radio_local")));
    
    *projection = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_rep_uni_check_projection")));
    
    textbuffer = GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_rep_uni_buffer_barres"));
    gtk_text_buffer_get_iter_at_offset(textbuffer, &start, 0);
    gtk_text_buffer_get_iter_at_offset(textbuffer, &end, -1);
    texte_tmp = gtk_text_buffer_get_text(textbuffer, &start, &end, FALSE);
    num_barres = common_selection_renvoie_numeros(texte_tmp);
    if (num_barres == NULL)
    {
        dialog = gtk_message_dialog_new(GTK_WINDOW(ef_gtk->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, gettext("La valeur des barres est incorrecte."));
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        free(texte_tmp);
        return FALSE;
    }
    else
    {
        *barres = common_selection_converti_numeros_en_barres(num_barres, projet);
        if (*barres == NULL)
        {
            dialog = gtk_message_dialog_new(GTK_WINDOW(ef_gtk->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, gettext("La barre %s n'existe pas."), texte_tmp);
            gtk_dialog_run(GTK_DIALOG(dialog));
            gtk_widget_destroy(dialog);
            free(texte_tmp);
            return FALSE;
        }
        else
        {
            // Si tous les paramètres sont corrects
            textbuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_rep_uni_textview_description")));
            
            gtk_text_buffer_get_iter_at_offset(textbuffer, &start, 0);
            gtk_text_buffer_get_iter_at_offset(textbuffer, &end, -1);
            *nom = gtk_text_buffer_get_text(textbuffer, &start, &end, FALSE);
            free(texte_tmp);
            
            return TRUE;
        }
    }
}


void EF_gtk_charge_barre_repartie_uniforme_ajouter_clicked(
  GtkButton *button __attribute__((unused)), Projet *projet)
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
    GtkTreeModel                    *model_action;
    GtkTreeIter                     iter_action;
    unsigned int                    numero_action;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_charge_barre_repartie_uniforme.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Charge Barre Répartie Uniforme");
    
    BUG(EF_gtk_charge_barre_repartie_uniforme_recupere_donnees(projet, &num_action, &barres, &fx, &fy, &fz, &mx, &my, &mz, &texte, &repere_local, &projection, &a, &b) == TRUE, );
    
    // Création de la nouvelle charge ponctuelle sur barre
    BUG(charge = EF_charge_barre_repartie_uniforme_ajout(projet, num_action, barres, repere_local, projection, a, b, fx, fy, fz, mx, my, mz, texte), );
    
    free(texte);
    
    // Actualisation de l'affichage graphique
    if (!gtk_tree_selection_get_selected(projet->list_gtk._1990_actions.tree_select_actions, &model_action, &iter_action))
        return;
    gtk_tree_model_get(model_action, &iter_action, 0, &numero_action, -1);
    if (numero_action == num_action)
        BUG(EF_gtk_charge_barre_repartie_uniforme_ajout_affichage(charge, projet, TRUE), );
    
    gtk_widget_destroy(projet->list_gtk.ef_charge_barre_repartie_uniforme.window);
    
    return;
}


void EF_gtk_charge_barre_repartie_uniforme_editer_clicked(
  GtkButton *button __attribute__((unused)), Projet *projet)
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
        BUG(EF_gtk_charge_barre_repartie_uniforme_ajout_affichage(charge, projet, FALSE), );
    
    gtk_widget_destroy(projet->list_gtk.ef_charge_barre_repartie_uniforme.window);
    
    return;
}


G_MODULE_EXPORT void EF_gtk_charge_barre_repartie_uniforme_toggled(
  GtkToggleButton *togglebutton __attribute__((unused)), Projet *projet)
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


G_MODULE_EXPORT gboolean EF_gtk_charge_barre_repartie_uniforme(Projet *projet,
  unsigned int action_defaut, unsigned int charge)
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
    
    if (projet->list_gtk._1990_actions.window == NULL)
        gtk_window_set_transient_for(GTK_WINDOW(ef_gtk->window), GTK_WINDOW(projet->list_gtk.comp.window));
    else
        gtk_window_set_transient_for(GTK_WINDOW(ef_gtk->window), GTK_WINDOW(projet->list_gtk._1990_actions.window));
    
    return TRUE;
}


#endif
