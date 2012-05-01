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

#include "common_projet.h"
#include "common_erreurs.h"
#include "common_gtk.h"
#include "common_maths.h"
#include "1992_1_1_barres.h"
#include "EF_charge_barre_ponctuelle.h"
#include "1990_actions.h"
#include "1990_gtk_actions.h"
#include "common_selection.h"

G_MODULE_EXPORT int EF_gtk_charge_barre_ponctuelle_ajout_affichage(Charge_Barre_Ponctuelle *charge, Projet *projet, gboolean nouvelle_ligne)
{
    char                    *description, txt_pos[30], txt_fx[30], txt_fy[30], txt_fz[30], txt_mx[30], txt_my[30], txt_mz[30], *txt_liste_barres;
    List_Gtk_1990_Actions   *list_gtk_1990_actions;
    
    BUGMSG(projet, -1, gettext("Paramètre incorrect\n"));
    
    list_gtk_1990_actions = &projet->list_gtk._1990_actions;
    if (list_gtk_1990_actions->window == NULL)
        return -1;
    
    BUG(txt_liste_barres = common_selection_converti_barres_en_texte(charge->barres), -3);
    common_math_double_to_char(charge->position, txt_pos, GTK_DECIMAL_DISTANCE);
    common_math_double_to_char(charge->fx, txt_fx, GTK_DECIMAL_FORCE);
    common_math_double_to_char(charge->fy, txt_fy, GTK_DECIMAL_FORCE);
    common_math_double_to_char(charge->fz, txt_fz, GTK_DECIMAL_FORCE);
    common_math_double_to_char(charge->mx, txt_mx, GTK_DECIMAL_MOMENT);
    common_math_double_to_char(charge->my, txt_my, GTK_DECIMAL_MOMENT);
    common_math_double_to_char(charge->mz, txt_mz, GTK_DECIMAL_MOMENT);
    
    BUGMSG(description = g_strdup_printf("%s : %s, %s : %s m, %s, Fx : %s N, Fy : %s N, Fz : %s N, Mx : %s N.m, My : %s N.m, Mz : %s N.m", strstr(txt_liste_barres, ";") == NULL ? gettext("Barre") : gettext("Barres"), txt_liste_barres, gettext("position"), txt_pos, charge->repere_local ? gettext("repère : local") : gettext("repère : global"), txt_fx, txt_fy, txt_fz, txt_mx, txt_my, txt_mz), -2, gettext("Erreur d'allocation mémoire.\n"));
    
    free(txt_liste_barres);
    
    if (nouvelle_ligne == TRUE)
        gtk_tree_store_append(list_gtk_1990_actions->tree_store_charges, &charge->Iter, NULL);
    gtk_tree_store_set(list_gtk_1990_actions->tree_store_charges, &charge->Iter, 0, charge->numero, 1, charge->description, 2, gettext("Ponctuelle sur barre"), 3, description, -1);
    free(description);
    
    return 0;
}


/* DEBUT DE LA FENETRE GRAPHIQUE*/

G_MODULE_EXPORT void EF_gtk_charge_barre_ponctuelle_annuler_clicked(GtkButton *button __attribute__((unused)), Projet *projet)
/* Description : Ferme la fenêtre sans effectuer les modifications
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    BUGMSG(projet, , gettext("Paramètre incorrect\n"));
    
    gtk_widget_destroy(projet->list_gtk.ef_charge_barre_ponctuelle.window);
    
    return;
}


gboolean EF_gtk_charge_barre_ponctuelle_recupere_donnees(Projet *projet, unsigned int *num_action, GList **barres, double *fx, double *fy, double *fz, double *mx, double *my, double *mz, gchar **description, gboolean *repere_local, double *position)
{
    GtkWidget                           *dialog;
    List_Gtk_EF_Charge_Barre_Ponctuelle *ef_gtk;
    GList                               *num_barres;
    GtkTextIter                         start, end;
    gchar                               *texte_tmp;
    GtkTextBuffer                       *textbuffer;
    
    BUGMSG(projet, FALSE, gettext("Paramètre incorrect\n"));
    BUGMSG(num_action, FALSE, gettext("Paramètre incorrect\n"));
    BUGMSG(barres, FALSE, gettext("Paramètre incorrect\n"));
    BUGMSG(fx, FALSE, gettext("Paramètre incorrect\n"));
    BUGMSG(fy, FALSE, gettext("Paramètre incorrect\n"));
    BUGMSG(fz, FALSE, gettext("Paramètre incorrect\n"));
    BUGMSG(mx, FALSE, gettext("Paramètre incorrect\n"));
    BUGMSG(my, FALSE, gettext("Paramètre incorrect\n"));
    BUGMSG(mz, FALSE, gettext("Paramètre incorrect\n"));
    BUGMSG(description, FALSE, gettext("Paramètre incorrect\n"));
    BUGMSG(repere_local, FALSE, gettext("Paramètre incorrect\n"));
    BUGMSG(position, FALSE, gettext("Paramètre incorrect\n"));
    
    ef_gtk = &projet->list_gtk.ef_charge_barre_ponctuelle;
    
    if (gtk_combo_box_get_active(ef_gtk->combobox_charge) < 0)
        return FALSE;
    else
        *num_action = (unsigned int)gtk_combo_box_get_active(ef_gtk->combobox_charge);
    
    *fx = gtk_common_entry_renvoie_double(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_ponct_buffer_fx")));
    if (isnan(*fx))
    {
        dialog = gtk_message_dialog_new(GTK_WINDOW(ef_gtk->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, gettext("La valeur de %s est incorrecte."), "Fx");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return FALSE;
    }
    *fy = gtk_common_entry_renvoie_double(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_ponct_buffer_fy")));
    if (isnan(*fy))
    {
        dialog = gtk_message_dialog_new(GTK_WINDOW(ef_gtk->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, gettext("La valeur de %s est incorrecte."), "Fy");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return FALSE;
    }
    *fz = gtk_common_entry_renvoie_double(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_ponct_buffer_fz")));
    if (isnan(*fz))
    {
        dialog = gtk_message_dialog_new(GTK_WINDOW(ef_gtk->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, gettext("La valeur de %s est incorrecte."), "Fz");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return FALSE;
    }
    *mx = gtk_common_entry_renvoie_double(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_ponct_buffer_mx")));
    if (isnan(*mx))
    {
        dialog = gtk_message_dialog_new(GTK_WINDOW(ef_gtk->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, gettext("La valeur de %s est incorrecte."), "Mx");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return FALSE;
    }
    *my = gtk_common_entry_renvoie_double(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_ponct_buffer_my")));
    if (isnan(*my))
    {
        dialog = gtk_message_dialog_new(GTK_WINDOW(ef_gtk->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, gettext("La valeur de %s est incorrecte."), "My");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return FALSE;
    }
    *mz = gtk_common_entry_renvoie_double(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_ponct_buffer_mz")));
    if (isnan(*mz))
    {
        dialog = gtk_message_dialog_new(GTK_WINDOW(ef_gtk->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, gettext("La valeur de %s est incorrecte."), "Mz");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return FALSE;
    }
    *position = gtk_common_entry_renvoie_double(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_ponct_buffer_pos")));
    if (isnan(*position))
    {
        dialog = gtk_message_dialog_new(GTK_WINDOW(ef_gtk->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, gettext("La valeur de la position est incorrecte."));
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return FALSE;
    }
    *repere_local = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_ponct_radio_local")));
    textbuffer = GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_ponct_buffer_barres"));
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
            dialog = gtk_message_dialog_new(GTK_WINDOW(ef_gtk->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, gettext("La liste des barres %s n'existe pas."), texte_tmp);
            gtk_dialog_run(GTK_DIALOG(dialog));
            gtk_widget_destroy(dialog);
            free(texte_tmp);
            return FALSE;
        }
        else
        {
            // Si tous les paramètres sont corrects
            textbuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_ponct_textview_description")));
            
            gtk_text_buffer_get_iter_at_offset(textbuffer, &start, 0);
            gtk_text_buffer_get_iter_at_offset(textbuffer, &end, -1);
            *description = gtk_text_buffer_get_text(textbuffer, &start, &end, FALSE);
            free(texte_tmp);
            return TRUE;
        }
    }
}


void EF_gtk_charge_barre_ponctuelle_ajouter_clicked(GtkButton *button __attribute__((unused)), Projet *projet)
/* Description : Ferme la fenêtre en ajoutant la charge
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    double                      fx, fy, fz, mx, my, mz, position;
    unsigned int                num_action;
    gboolean                    repere_local;
    GList                       *barres;
    gchar                       *texte;
    Charge_Barre_Ponctuelle     *charge;
    GtkTreeModel                *model_action;
    GtkTreeIter                 iter_action;
    unsigned int                numero_action;
    
    BUGMSG(projet, , gettext("Paramètre incorrect\n"));
    
    BUG(EF_gtk_charge_barre_ponctuelle_recupere_donnees(projet, &num_action, &barres, &fx, &fy, &fz, &mx, &my, &mz, &texte, &repere_local, &position) == TRUE, );
    
    // Création de la nouvelle charge ponctuelle sur barre
    BUG(charge = EF_charge_barre_ponctuelle_ajout(projet, num_action, barres, repere_local, position, fx, fy, fz, mx, my, mz, texte), );
    
    free(texte);
    
    // Actualisation de l'affichage graphique
    if (!gtk_tree_selection_get_selected(projet->list_gtk._1990_actions.tree_select_actions, &model_action, &iter_action))
        return;
    gtk_tree_model_get(model_action, &iter_action, 0, &numero_action, -1);
    if (numero_action == num_action)
        BUG(EF_gtk_charge_barre_ponctuelle_ajout_affichage(charge, projet, TRUE) == 0, );
    
    gtk_widget_destroy(projet->list_gtk.ef_charge_barre_ponctuelle.window);
    
    return;
}


void EF_gtk_charge_barre_ponctuelle_editer_clicked(GtkButton *button __attribute__((unused)), Projet *projet)
/* Description : Ferme la fenêtre en appliquant les modifications
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    List_Gtk_EF_Charge_Barre_Ponctuelle    *ef_gtk;
    double                      fx, fy, fz, mx, my, mz, position;
    unsigned int                num_action;
    gboolean                    repere_local;
    GList                       *barres;
    gchar                       *texte;
    Charge_Barre_Ponctuelle     *charge;
    
    BUGMSG(projet, , gettext("Paramètre incorrect\n"));
    
    ef_gtk = &projet->list_gtk.ef_charge_barre_ponctuelle;
    
    BUG(EF_gtk_charge_barre_ponctuelle_recupere_donnees(projet, &num_action, &barres, &fx, &fy, &fz, &mx, &my, &mz, &texte, &repere_local, &position) == TRUE, );
    
    // Création de la nouvelle charge ponctuelle sur barre
    charge = _1990_action_cherche_charge(projet, ef_gtk->action, ef_gtk->charge);
    BUG(charge, );
    free(charge->description);
    charge->description = texte;
    charge->fx = fx;
    charge->fy = fy;
    charge->fz = fz;
    charge->mx = mx;
    charge->my = my;
    charge->mz = mz;
    g_list_free(charge->barres);
    charge->barres = barres;
    charge->position = position;
    charge->repere_local = repere_local;
    if (num_action != ef_gtk->action)
        BUG(_1990_action_deplace_charge(projet, ef_gtk->action, ef_gtk->charge, num_action) == 0, );
    else
        BUG(EF_gtk_charge_barre_ponctuelle_ajout_affichage(charge, projet, FALSE) == 0, );
    
    gtk_widget_destroy(projet->list_gtk.ef_charge_barre_ponctuelle.window);
    
    return;
}


G_MODULE_EXPORT gboolean EF_gtk_charge_barre_ponct_window_key_press(GtkWidget *widget __attribute__((unused)), GdkEvent *event, Projet *projet)
{
    BUGMSG(projet, TRUE, gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk.ef_charge_barre_ponctuelle.builder, TRUE, gettext("Paramètre incorrect\n"));
    
    if (event->key.keyval == GDK_KEY_Escape)
    {
        gtk_widget_destroy(projet->list_gtk.ef_charge_barre_ponctuelle.window);
        return TRUE;
    }
    else
        return FALSE;
}


G_MODULE_EXPORT void EF_gtk_charge_barre_ponct_window_destroy(GtkWidget *object __attribute__((unused)), Projet *projet)
/* Description : met projet->list_gtk._1990_actions.window à NULL quand la fenêtre se ferme
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    BUGMSG(projet, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk.ef_charge_barre_ponctuelle.builder, , gettext("Paramètre incorrect\n"));
    
    projet->list_gtk.ef_charge_barre_ponctuelle.builder = NULL;
    
    return;
}


G_MODULE_EXPORT int EF_gtk_charge_barre_ponctuelle(Projet *projet, unsigned int action_defaut, unsigned int charge)
/* Description : Affichage de la fenêtre permettant de créer ou modifier une action de type
 *               charge ponctuelle sur barre
 * Paramètres : Projet *projet : la variable projet
 *              gint action_defaut : action par défaut dans la fenêtre,
 *              gint charge : vaut -1 si une nouvelle charge doit être ajoutée,
 *                            vaut le numéro de la charge si elle doit être modifiée
 * Valeur renvoyée : Aucune
 */
{
    List_Gtk_EF_Charge_Barre_Ponctuelle *ef_gtk;
    Charge_Barre_Ponctuelle             *charge_barre;
    
    ef_gtk = &projet->list_gtk.ef_charge_barre_ponctuelle;
    ef_gtk->builder = gtk_builder_new();
    BUGMSG(gtk_builder_add_from_file(ef_gtk->builder, DATADIR"/ui/EF_gtk_charge_barre_ponctuelle.ui", NULL) != 0, -1, gettext("Builder Failed\n"));
    gtk_builder_connect_signals(ef_gtk->builder, projet);
    
    ef_gtk->window = GTK_WIDGET(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_ponct_window"));
    ef_gtk->combobox_charge = GTK_COMBO_BOX(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_ponct_combo_box_charge"));
    
    if (charge == G_MAXUINT)
    {
        ef_gtk->action = 0;
        ef_gtk->charge = 0;
        gtk_window_set_title(GTK_WINDOW(ef_gtk->window), gettext("Ajout d'une charge ponctuelle sur barre"));
        charge_barre = NULL;
    }
    else
    {
        ef_gtk->action = action_defaut;
        ef_gtk->charge = charge;
        gtk_window_set_title(GTK_WINDOW(ef_gtk->window), gettext("Modification d'une charge ponctuelle sur barre"));
        BUG(charge_barre = _1990_action_cherche_charge(projet, action_defaut, charge), -1);
    }
    
    _1990_gtk_actions_genere_liste(projet);
    gtk_combo_box_set_model(GTK_COMBO_BOX(ef_gtk->combobox_charge), GTK_TREE_MODEL(projet->list_gtk._1990_actions.list_actions_pour_combobox));
    gtk_combo_box_set_active(GTK_COMBO_BOX(ef_gtk->combobox_charge), (gint)action_defaut);
    
    if (charge_barre != NULL)
    {
        gchar   tmp[30], *tmp2;
        gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_ponct_textview_description"))), charge_barre->description, -1);
        common_math_double_to_char(charge_barre->fx, tmp, GTK_DECIMAL_FORCE);
        gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_ponct_buffer_fx")), tmp, -1);
        common_math_double_to_char(charge_barre->fy, tmp, GTK_DECIMAL_FORCE);
        gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_ponct_buffer_fy")), tmp, -1);
        common_math_double_to_char(charge_barre->fz, tmp, GTK_DECIMAL_FORCE);
        gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_ponct_buffer_fz")), tmp, -1);
        common_math_double_to_char(charge_barre->mx, tmp, GTK_DECIMAL_MOMENT);
        gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_ponct_buffer_mx")), tmp, -1);
        common_math_double_to_char(charge_barre->my, tmp, GTK_DECIMAL_MOMENT);
        gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_ponct_buffer_my")), tmp, -1);
        common_math_double_to_char(charge_barre->mz, tmp, GTK_DECIMAL_MOMENT);
        gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_ponct_buffer_mz")), tmp, -1);
        common_math_double_to_char(charge_barre->position, tmp, GTK_DECIMAL_DISTANCE);
        gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_ponct_buffer_pos")), tmp, -1);
        if (charge_barre->repere_local)
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_ponct_radio_local")), TRUE);
        else
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_ponct_radio_global")), TRUE);
        BUG(tmp2 = common_selection_converti_barres_en_texte(charge_barre->barres), -3);
        gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_ponct_buffer_barres")), tmp2, -1);
        free(tmp2);
    }
    
    if (charge == G_MAXUINT)
    {
        gtk_button_set_label(GTK_BUTTON(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_ponct_button_add_edit")), "gtk-add");
        g_signal_connect(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_ponct_button_add_edit"), "clicked", G_CALLBACK(EF_gtk_charge_barre_ponctuelle_ajouter_clicked), projet);
    }
    else
    {
        gtk_button_set_label(GTK_BUTTON(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_ponct_button_add_edit")), "gtk-edit");
        g_signal_connect(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_ponct_button_add_edit"), "clicked", G_CALLBACK(EF_gtk_charge_barre_ponctuelle_editer_clicked), projet);
    }
    
    if (projet->list_gtk._1990_actions.window == NULL)
        gtk_window_set_transient_for(GTK_WINDOW(ef_gtk->window), GTK_WINDOW(projet->list_gtk.comp.window));
    else
        gtk_window_set_transient_for(GTK_WINDOW(ef_gtk->window), GTK_WINDOW(projet->list_gtk._1990_actions.window));
    return 0;
}


#endif