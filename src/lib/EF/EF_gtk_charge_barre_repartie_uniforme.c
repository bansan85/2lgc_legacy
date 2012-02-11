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
#include "EF_charge_barre_repartie_uniforme.h"
#include "1990_actions.h"
#include "common_selection.h"

int EF_gtk_charge_barre_repartie_uniforme_ajout_affichage(Charge_Barre_Repartie_Uniforme *charge, Projet *projet, gboolean nouvelle_ligne)
{
    char                    *description, *txt_liste_barres, txt_debut[30], txt_fin[30];
    char                    txt_fx[30], txt_fy[30], txt_fz[30], txt_mx[30], txt_my[30], txt_mz[30];
    List_Gtk_1990_Actions   *list_gtk_1990_actions;
    
    BUGMSG(projet, -1, gettext("Paramètre incorrect\n"));
    
    list_gtk_1990_actions = &projet->list_gtk._1990_actions;
    
    if (list_gtk_1990_actions->window == NULL)
        return -1;
    
    BUG(txt_liste_barres = common_selection_converti_barres_en_texte(charge->barres), -3);
    common_math_double_to_char(charge->a, txt_debut, GTK_DECIMAL_DISTANCE);
    common_math_double_to_char(charge->b, txt_fin, GTK_DECIMAL_DISTANCE);
    common_math_double_to_char(charge->fx, txt_fx, GTK_DECIMAL_FORCE);
    common_math_double_to_char(charge->fy, txt_fy, GTK_DECIMAL_FORCE);
    common_math_double_to_char(charge->fz, txt_fz, GTK_DECIMAL_FORCE);
    common_math_double_to_char(charge->mx, txt_mx, GTK_DECIMAL_MOMENT);
    common_math_double_to_char(charge->my, txt_my, GTK_DECIMAL_MOMENT);
    common_math_double_to_char(charge->mz, txt_mz, GTK_DECIMAL_MOMENT);
    
    BUGMSG(description = g_strdup_printf("%s : %s, %s : %s m, %s : %s m, %s, %s, Fx : %s N/m, Fy : %s N/m, Fz : %s N/m, Mx : %s N.m/m, My : %s N.m/m, Mz : %s N.m/m", strstr(txt_liste_barres, ";") == NULL ? gettext("Barre") : gettext("Barres"), txt_liste_barres, gettext("début"), txt_debut, gettext("fin (par rapport à la fin)"), txt_fin, charge->projection == TRUE ? gettext("projection : oui") : gettext("projection : non"), charge->repere_local ? gettext("repère : local") : gettext("repère : global"), txt_fx, txt_fy, txt_fz, txt_mx, txt_my, txt_mz), -2, gettext("Erreur d'allocation mémoire.\n"));
    
    free(txt_liste_barres);
    if (nouvelle_ligne == TRUE)
        gtk_tree_store_append(list_gtk_1990_actions->tree_store_charges, &charge->Iter, NULL);
    gtk_tree_store_set(list_gtk_1990_actions->tree_store_charges, &charge->Iter, 0, charge->numero, 1, charge->description, 2, gettext("Répartie uniforme sur barre"), 3, description, -1);
    free(description);
    
    return 0;
}


/* DEBUT DE LA FENETRE GRAPHIQUE*/

void EF_gtk_charge_barre_repartie_uniforme_annuler_clicked(GtkButton *button __attribute__((unused)), GtkWidget *fenetre)
/* Description : Ferme la fenêtre sans effectuer les modifications
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    BUGMSG(fenetre, , gettext("Paramètre incorrect\n"));
    gtk_widget_destroy(fenetre);
    return;
}


gboolean EF_gtk_charge_barre_repartie_uniforme_recupere_donnees(Projet *projet, int *num_action, GList **barres, double *fx, double *fy, double *fz, double *mx, double *my, double *mz, gchar **description, int *repere_local, int *projection, double *a, double *b)
{
    GtkWidget                                  *dialog;
    List_Gtk_EF_Charge_Barre_Repartie_Uniforme *ef_gtk;
    GList                                      *num_barres;
    GtkTextIter                                 start, end;
    gchar                                       *texte_tmp;
    GtkTextBuffer                               *textbuffer;
    
    BUGMSG(projet, FALSE, gettext("Paramètre incorrect\n"));
    
    ef_gtk = &projet->list_gtk.ef_charge_barre_repartie_uniforme;
    
    *num_action = gtk_combo_box_get_active(GTK_COMBO_BOX(ef_gtk->combobox_charge));
    
    *fx = gtk_common_entry_renvoie_double(gtk_text_view_get_buffer(GTK_TEXT_VIEW(ef_gtk->text_view_fx)));
    if (isnan(*fx))
    {
        dialog = gtk_message_dialog_new(GTK_WINDOW(ef_gtk->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, gettext("La valeur de %s est incorrecte."), "Fx");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return FALSE;
    }
    *fy = gtk_common_entry_renvoie_double(gtk_text_view_get_buffer(GTK_TEXT_VIEW(ef_gtk->text_view_fy)));
    if (isnan(*fy))
    {
        dialog = gtk_message_dialog_new(GTK_WINDOW(ef_gtk->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, gettext("La valeur de %s est incorrecte."), "Fy");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return FALSE;
    }
    *fz = gtk_common_entry_renvoie_double(gtk_text_view_get_buffer(GTK_TEXT_VIEW(ef_gtk->text_view_fz)));
    if (isnan(*fz))
    {
        dialog = gtk_message_dialog_new(GTK_WINDOW(ef_gtk->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, gettext("La valeur de %s est incorrecte."), "Fz");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return FALSE;
    }
    *mx = gtk_common_entry_renvoie_double(gtk_text_view_get_buffer(GTK_TEXT_VIEW(ef_gtk->text_view_mx)));
    if (isnan(*mx))
    {
        dialog = gtk_message_dialog_new(GTK_WINDOW(ef_gtk->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, gettext("La valeur de %s est incorrecte."), "Mx");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return FALSE;
    }
    *my = gtk_common_entry_renvoie_double(gtk_text_view_get_buffer(GTK_TEXT_VIEW(ef_gtk->text_view_my)));
    if (isnan(*my))
    {
        dialog = gtk_message_dialog_new(GTK_WINDOW(ef_gtk->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, gettext("La valeur de %s est incorrecte."), "My");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return FALSE;
    }
    *mz = gtk_common_entry_renvoie_double(gtk_text_view_get_buffer(GTK_TEXT_VIEW(ef_gtk->text_view_mz)));
    if (isnan(*mz))
    {
        dialog = gtk_message_dialog_new(GTK_WINDOW(ef_gtk->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, gettext("La valeur de %s est incorrecte."), "Mz");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return FALSE;
    }
    *a = gtk_common_entry_renvoie_double(gtk_text_view_get_buffer(GTK_TEXT_VIEW(ef_gtk->text_view_a)));
    if (isnan(*a))
    {
        dialog = gtk_message_dialog_new(GTK_WINDOW(ef_gtk->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, gettext("La valeur de a est incorrecte."));
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return FALSE;
    }
    *b = gtk_common_entry_renvoie_double(gtk_text_view_get_buffer(GTK_TEXT_VIEW(ef_gtk->text_view_b)));
    if (isnan(*b))
    {
        dialog = gtk_message_dialog_new(GTK_WINDOW(ef_gtk->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, gettext("La valeur de b est incorrecte."));
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return FALSE;
    }
    *repere_local = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ef_gtk->check_button_repere_local));
    *projection = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ef_gtk->check_button_projection));
    textbuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(ef_gtk->text_view_barre));
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
            textbuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(ef_gtk->text_view_description));
            
            gtk_text_buffer_get_iter_at_offset(textbuffer, &start, 0);
            gtk_text_buffer_get_iter_at_offset(textbuffer, &end, -1);
            *description = gtk_text_buffer_get_text(textbuffer, &start, &end, FALSE);
            free(texte_tmp);
            
            return TRUE;
        }
    }
}


void EF_gtk_charge_barre_repartie_uniforme_ajouter_clicked(GtkButton *button __attribute__((unused)), Projet *projet)
/* Description : Ferme la fenêtre en ajoutant la charge
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    double                          fx, fy, fz, mx, my, mz, a, b;
    int                             num_action, repere_local, projection;
    GList                           *barres;
    gchar                           *texte;
    Charge_Barre_Repartie_Uniforme  *charge;
    GtkTreeModel                    *model_action;
    GtkTreeIter                     iter_action;
    int                             numero_action;
    
    BUGMSG(projet, , gettext("Paramètre incorrect\n"));
    BUG(EF_gtk_charge_barre_repartie_uniforme_recupere_donnees(projet, &num_action, &barres, &fx, &fy, &fz, &mx, &my, &mz, &texte, &repere_local, &projection, &a, &b) == TRUE, );
    
    // Création de la nouvelle charge ponctuelle sur barre
    BUG(charge = EF_charge_barre_repartie_uniforme_ajout(projet, num_action, barres, repere_local, projection, a, b, fx, fy, fz, mx, my, mz, texte), );
    
    free(texte);
    
    // Actualisation de l'affichage graphique
    if (!gtk_tree_selection_get_selected(projet->list_gtk._1990_actions.tree_select_actions, &model_action, &iter_action))
        return;
    gtk_tree_model_get(model_action, &iter_action, 0, &numero_action, -1);
    if (numero_action == num_action)
        BUG(EF_gtk_charge_barre_repartie_uniforme_ajout_affichage(charge, projet, TRUE) == 0, );
    
    gtk_widget_destroy(projet->list_gtk.ef_charge_barre_repartie_uniforme.window);
    
    return;
}


void EF_gtk_charge_barre_repartie_uniforme_editer_clicked(GtkButton *button __attribute__((unused)), Projet *projet)
/* Description : Ferme la fenêtre en appliquant les modifications
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    List_Gtk_EF_Charge_Barre_Repartie_Uniforme *ef_gtk;
    double                          fx, fy, fz, mx, my, mz, a, b;
    int                             num_action, repere_local, projection;
    GList                           *barres;
    gchar                           *texte;
    Charge_Barre_Repartie_Uniforme  *charge;
    
    BUGMSG(projet, , gettext("Paramètre incorrect\n"));
    
    ef_gtk = &projet->list_gtk.ef_charge_barre_repartie_uniforme;
    
    BUG(EF_gtk_charge_barre_repartie_uniforme_recupere_donnees(projet, &num_action, &barres, &fx, &fy, &fz, &mx, &my, &mz, &texte, &repere_local, &projection, &a, &b) == TRUE, );
    
    // Création de la nouvelle charge ponctuelle sur barre
    BUG(charge = _1990_action_cherche_charge(projet, ef_gtk->action, ef_gtk->charge), );
    free(charge->description);
    charge->description = texte;
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
        BUG(_1990_action_deplace_charge(projet, ef_gtk->action, ef_gtk->charge, num_action), );
    else
        BUG(EF_gtk_charge_barre_repartie_uniforme_ajout_affichage(charge, projet, FALSE) == 0, );
    
    gtk_widget_destroy(projet->list_gtk.ef_charge_barre_repartie_uniforme.window);
    
    return;
}


void EF_gtk_charge_barre_repartie_uniforme_toggled(GtkToggleButton *togglebutton __attribute__((unused)), Projet *projet)
/* Description : Évènement lors du cochage de projection et repère globale. En effet, il n'est
 *               pas possible de faire à la fois une projection dans le repère local
 * Paramètres : GtkToggleButton *togglebutton : composant à l'origine de l'évènement
 *              Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    List_Gtk_EF_Charge_Barre_Repartie_Uniforme  *ef_gtk;
    
    BUGMSG(projet, , gettext("Paramètre incorrect\n"));
    
    ef_gtk = &projet->list_gtk.ef_charge_barre_repartie_uniforme;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ef_gtk->check_button_repere_local)))
    {
        gtk_widget_set_sensitive(ef_gtk->check_button_projection, FALSE);
        gtk_widget_set_sensitive(ef_gtk->check_button_repere_local, TRUE);
    }
    else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ef_gtk->check_button_projection)))
    {
        gtk_widget_set_sensitive(ef_gtk->check_button_projection, TRUE);
        gtk_widget_set_sensitive(ef_gtk->check_button_repere_local, FALSE);
    }
    else
    {
        gtk_widget_set_sensitive(ef_gtk->check_button_projection, TRUE);
        gtk_widget_set_sensitive(ef_gtk->check_button_repere_local, TRUE);
    }
}


int EF_gtk_charge_barre_repartie_uniforme(Projet *projet, gint action_defaut, gint charge)
/* Description : Affichage de la fenêtre permettant de créer ou modifier une action de type
 *               charge repartie uniforme sur barre
 * Paramètres : Projet *projet : la variable projet
 *              gint action_defaut : action par défaut dans la fenêtre,
 *              gint charge : vaut -1 si une nouvelle charge doit être ajoutée,
 *                            vaut le numéro de la charge si elle doit être modifiée
 * Valeur renvoyée : Aucune
 */
{
    List_Gtk_EF_Charge_Barre_Repartie_Uniforme  *ef_gtk;
    Charge_Barre_Repartie_Uniforme              *charge_barre;
    GList                                       *list_parcours;
    
    BUGMSG(projet, -1, gettext("Paramètre incorrect\n"));
    BUGMSG(projet->actions, -1, gettext("Paramètre incorrect\n"));
    
    ef_gtk = &projet->list_gtk.ef_charge_barre_repartie_uniforme;
    
    if (charge == -1)
    {
        GTK_NOUVELLE_FENETRE(ef_gtk->window, gettext("Ajout d'une charge répartie uniforme sur barre"), 400, 1)
        charge_barre = NULL;
    }
    else
    {
        ef_gtk->action = action_defaut;
        ef_gtk->charge = charge;
        GTK_NOUVELLE_FENETRE(ef_gtk->window, gettext("Modification d'une charge répartie uniforme sur barre"), 400, 1)
        BUG(charge_barre = _1990_action_cherche_charge(projet, action_defaut, charge), -3);
    }
    
    ef_gtk->table = gtk_table_new(7, 4, FALSE);
    gtk_container_add(GTK_CONTAINER(ef_gtk->window), ef_gtk->table);
    
    ef_gtk->label_charge = gtk_label_new(gettext("Charge :"));
    gtk_table_attach(GTK_TABLE(ef_gtk->table), ef_gtk->label_charge, 0, 1, 0, 1, GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
    
    ef_gtk->combobox_charge = gtk_combo_box_text_new();
    list_parcours = projet->actions;
    do
    {
        Action *action = list_parcours->data;
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(ef_gtk->combobox_charge), action->description);
        list_parcours = g_list_next(list_parcours);
    }
    while (list_parcours != NULL);
    gtk_combo_box_set_active(GTK_COMBO_BOX(ef_gtk->combobox_charge), action_defaut);
    gtk_table_attach(GTK_TABLE(ef_gtk->table), ef_gtk->combobox_charge, 1, 4, 0, 1, GTK_EXPAND | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
    
    ef_gtk->label_description = gtk_label_new(gettext("Description :"));
    gtk_table_attach(GTK_TABLE(ef_gtk->table), ef_gtk->label_description, 0, 1, 1, 2, GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
    ef_gtk->text_view_description = gtk_text_view_new();
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(ef_gtk->text_view_description), GTK_WRAP_WORD);
    gtk_text_view_set_accepts_tab(GTK_TEXT_VIEW(ef_gtk->text_view_description), FALSE);
    gtk_container_set_border_width(GTK_CONTAINER(ef_gtk->text_view_description), 3);
    gtk_table_attach(GTK_TABLE(ef_gtk->table), ef_gtk->text_view_description, 1, 4, 1, 2, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
    
    ef_gtk->label_fx = gtk_label_new(gettext("Fx :"));
    gtk_table_attach(GTK_TABLE(ef_gtk->table), ef_gtk->label_fx, 0, 1, 2, 3, GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
    GTK_NOUVEAU_TEXT_VIEW_AVEC_SCROLLED_WINDOW(ef_gtk->text_view_fx, ef_gtk->sw_fx)
    GTK_TEXT_VIEW_VERIFIE_DOUBLE(ef_gtk->text_view_fx)
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(ef_gtk->text_view_fx)), "0", -1);
    gtk_table_attach(GTK_TABLE(ef_gtk->table), ef_gtk->sw_fx, 1, 2, 2, 3, GTK_EXPAND | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
    
    ef_gtk->label_fy = gtk_label_new(gettext("Fy :"));
    gtk_table_attach(GTK_TABLE(ef_gtk->table), ef_gtk->label_fy, 0, 1, 3, 4, GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
    GTK_NOUVEAU_TEXT_VIEW_AVEC_SCROLLED_WINDOW(ef_gtk->text_view_fy, ef_gtk->sw_fy)
    GTK_TEXT_VIEW_VERIFIE_DOUBLE(ef_gtk->text_view_fy)
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(ef_gtk->text_view_fy)), "0", -1);
    gtk_table_attach(GTK_TABLE(ef_gtk->table), ef_gtk->sw_fy, 1, 2, 3, 4, GTK_EXPAND | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
    
    ef_gtk->label_fz = gtk_label_new(gettext("Fz :"));
    gtk_table_attach(GTK_TABLE(ef_gtk->table), ef_gtk->label_fz, 0, 1, 4, 5, GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
    GTK_NOUVEAU_TEXT_VIEW_AVEC_SCROLLED_WINDOW(ef_gtk->text_view_fz, ef_gtk->sw_fz)
    GTK_TEXT_VIEW_VERIFIE_DOUBLE(ef_gtk->text_view_fz)
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(ef_gtk->text_view_fz)), "0", -1);
    gtk_table_attach(GTK_TABLE(ef_gtk->table), ef_gtk->sw_fz, 1, 2, 4, 5, GTK_EXPAND | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
    
    ef_gtk->label_mx = gtk_label_new(gettext("Mx :"));
    gtk_table_attach(GTK_TABLE(ef_gtk->table), ef_gtk->label_mx, 2, 3, 2, 3, GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 10, 0);
    GTK_NOUVEAU_TEXT_VIEW_AVEC_SCROLLED_WINDOW(ef_gtk->text_view_mx, ef_gtk->sw_mx)
    GTK_TEXT_VIEW_VERIFIE_DOUBLE(ef_gtk->text_view_mx)
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(ef_gtk->text_view_mx)), "0", -1);
    gtk_table_attach(GTK_TABLE(ef_gtk->table), ef_gtk->sw_mx, 3, 4, 2, 3, GTK_EXPAND | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
    
    ef_gtk->label_my = gtk_label_new(gettext("My :"));
    gtk_table_attach(GTK_TABLE(ef_gtk->table), ef_gtk->label_my, 2, 3, 3, 4, GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 10, 0);
    GTK_NOUVEAU_TEXT_VIEW_AVEC_SCROLLED_WINDOW(ef_gtk->text_view_my, ef_gtk->sw_my)
    GTK_TEXT_VIEW_VERIFIE_DOUBLE(ef_gtk->text_view_my)
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(ef_gtk->text_view_my)), "0", -1);
    gtk_table_attach(GTK_TABLE(ef_gtk->table), ef_gtk->sw_my, 3, 4, 3, 4, GTK_EXPAND | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
    
    ef_gtk->label_mz = gtk_label_new(gettext("Mz :"));
    gtk_table_attach(GTK_TABLE(ef_gtk->table), ef_gtk->label_mz, 2, 3, 4, 5, GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 10, 0);
    GTK_NOUVEAU_TEXT_VIEW_AVEC_SCROLLED_WINDOW(ef_gtk->text_view_mz, ef_gtk->sw_mz)
    GTK_TEXT_VIEW_VERIFIE_DOUBLE(ef_gtk->text_view_mz)
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(ef_gtk->text_view_mz)), "0", -1);
    gtk_table_attach(GTK_TABLE(ef_gtk->table), ef_gtk->sw_mz, 3, 4, 4, 5, GTK_EXPAND | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
    
    ef_gtk->check_button_repere_local = gtk_check_button_new_with_label(gettext("Repère local"));
    gtk_table_attach(GTK_TABLE(ef_gtk->table), ef_gtk->check_button_repere_local, 0, 2, 5, 6, GTK_EXPAND | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
    g_signal_connect(ef_gtk->check_button_repere_local, "toggled", G_CALLBACK(EF_gtk_charge_barre_repartie_uniforme_toggled), projet);
    
    ef_gtk->check_button_projection = gtk_check_button_new_with_label(gettext("Projection"));
    gtk_table_attach(GTK_TABLE(ef_gtk->table), ef_gtk->check_button_projection, 2, 4, 5, 6, GTK_EXPAND | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
    g_signal_connect(ef_gtk->check_button_projection, "toggled", G_CALLBACK(EF_gtk_charge_barre_repartie_uniforme_toggled), projet);
    
    ef_gtk->label_a = gtk_label_new(gettext("a :"));
    gtk_table_attach(GTK_TABLE(ef_gtk->table), ef_gtk->label_a, 0, 1, 6, 7, GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 10, 0);
    GTK_NOUVEAU_TEXT_VIEW_AVEC_SCROLLED_WINDOW(ef_gtk->text_view_a, ef_gtk->sw_a)
    GTK_TEXT_VIEW_VERIFIE_DOUBLE(ef_gtk->text_view_a)
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(ef_gtk->text_view_a)), "0", -1);
    gtk_table_attach(GTK_TABLE(ef_gtk->table), ef_gtk->sw_a, 1, 2, 6, 7, GTK_EXPAND | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
    
    ef_gtk->label_b = gtk_label_new(gettext("b :"));
    gtk_table_attach(GTK_TABLE(ef_gtk->table), ef_gtk->label_b, 2, 3, 6, 7, GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 10, 0);
    GTK_NOUVEAU_TEXT_VIEW_AVEC_SCROLLED_WINDOW(ef_gtk->text_view_b, ef_gtk->sw_b)
    GTK_TEXT_VIEW_VERIFIE_DOUBLE(ef_gtk->text_view_b)
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(ef_gtk->text_view_b)), "0", -1);
    gtk_table_attach(GTK_TABLE(ef_gtk->table), ef_gtk->sw_b, 3, 4, 6, 7, GTK_EXPAND | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
    
    ef_gtk->label_barre = gtk_label_new(gettext("Barre :"));
    gtk_table_attach(GTK_TABLE(ef_gtk->table), ef_gtk->label_barre, 0, 1, 7, 8, GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
    GTK_NOUVEAU_TEXT_VIEW_AVEC_SCROLLED_WINDOW(ef_gtk->text_view_barre, ef_gtk->sw_barre)
    GTK_TEXT_VIEW_VERIFIE_LISTE(ef_gtk->text_view_barre)
    gtk_table_attach(GTK_TABLE(ef_gtk->table), ef_gtk->sw_barre, 1, 4, 7, 8, GTK_EXPAND | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
    if (charge_barre != NULL)
    {
        gchar   tmp[30], *tmp2;
        gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(ef_gtk->text_view_description)), charge_barre->description, -1);
        common_math_double_to_char(charge_barre->fx, tmp, GTK_DECIMAL_FORCE);
        gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(ef_gtk->text_view_fx)), tmp, -1);
        common_math_double_to_char(charge_barre->fy, tmp, GTK_DECIMAL_FORCE);
        gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(ef_gtk->text_view_fy)), tmp, -1);
        common_math_double_to_char(charge_barre->fz, tmp, GTK_DECIMAL_FORCE);
        gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(ef_gtk->text_view_fz)), tmp, -1);
        common_math_double_to_char(charge_barre->mx, tmp, GTK_DECIMAL_MOMENT);
        gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(ef_gtk->text_view_mx)), tmp, -1);
        common_math_double_to_char(charge_barre->my, tmp, GTK_DECIMAL_MOMENT);
        gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(ef_gtk->text_view_my)), tmp, -1);
        common_math_double_to_char(charge_barre->mz, tmp, GTK_DECIMAL_MOMENT);
        gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(ef_gtk->text_view_mz)), tmp, -1);
        common_math_double_to_char(charge_barre->a, tmp, GTK_DECIMAL_DISTANCE);
        gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(ef_gtk->text_view_a)), tmp, -1);
        common_math_double_to_char(charge_barre->b, tmp, GTK_DECIMAL_DISTANCE);
        gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(ef_gtk->text_view_b)), tmp, -1);
        if ((charge_barre->repere_local) && (charge_barre->projection))
        {
            GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(ef_gtk->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, gettext("Il n'est pas possible d'activer à la fois la projection et l'utilisation du repère local."));
            gtk_dialog_run(GTK_DIALOG(dialog));
            gtk_widget_destroy(dialog);
            return -1;
        }
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ef_gtk->check_button_repere_local), charge_barre->repere_local);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ef_gtk->check_button_projection), charge_barre->projection);
        BUG(tmp2 = common_selection_converti_barres_en_texte(charge_barre->barres), -3);
        gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(ef_gtk->text_view_barre)), tmp2, -1);
        free(tmp2);
    }
    ef_gtk->table_buttons = gtk_table_new(1, 2, FALSE);
    gtk_table_attach(GTK_TABLE(ef_gtk->table), ef_gtk->table_buttons, 0, 4, 8, 9, GTK_EXPAND | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
    if (charge == -1)
    {
        ef_gtk->button_ajouter = gtk_button_new_from_stock(GTK_STOCK_ADD);
        g_signal_connect(ef_gtk->button_ajouter, "clicked", G_CALLBACK(EF_gtk_charge_barre_repartie_uniforme_ajouter_clicked), projet);
    }
    else
    {
        ef_gtk->button_ajouter = gtk_button_new_from_stock(GTK_STOCK_EDIT);
        g_signal_connect(ef_gtk->button_ajouter, "clicked", G_CALLBACK(EF_gtk_charge_barre_repartie_uniforme_editer_clicked), projet);
    }
    gtk_table_attach(GTK_TABLE(ef_gtk->table_buttons), ef_gtk->button_ajouter, 0, 1, 0, 1, GTK_EXPAND | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
    ef_gtk->button_annuler = gtk_button_new_from_stock(GTK_STOCK_CANCEL);
    g_signal_connect(ef_gtk->button_annuler, "clicked", G_CALLBACK(EF_gtk_charge_barre_repartie_uniforme_annuler_clicked), ef_gtk->window);
    gtk_table_attach(GTK_TABLE(ef_gtk->table_buttons), ef_gtk->button_annuler, 1, 2, 0, 1, GTK_EXPAND | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
    
    if (projet->list_gtk._1990_actions.window == NULL)
        gtk_window_set_transient_for(GTK_WINDOW(ef_gtk->window), GTK_WINDOW(projet->list_gtk.comp.window));
    else
        gtk_window_set_transient_for(GTK_WINDOW(ef_gtk->window), GTK_WINDOW(projet->list_gtk._1990_actions.window));
    gtk_window_set_modal(GTK_WINDOW(ef_gtk->window), TRUE);
    gtk_widget_show_all(ef_gtk->window);
    
    return 0;
}

#endif
