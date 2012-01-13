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
#include "EF_noeud.h"
#include "EF_charge_noeud.h"


void EF_gtk_charge_noeud_ajout_affichage(Charge_Noeud *charge, Projet *projet)
{
    char                    *description, tmp[30];
    List_Gtk_1990_Actions   *list_gtk_1990_actions = &projet->list_gtk._1990_actions;
    
    if (list_gtk_1990_actions->window == NULL)
        return;
    description = malloc(sizeof(char)*(strlen(gettext("Noeud : "))+1));
    strcpy(description, gettext("Noeud : "));
    sprintf(tmp, "%d", charge->noeud->numero);
    description = realloc(description, (strlen(description) + strlen(tmp)+1)*sizeof(char));
    strcat(description, tmp);
    common_math_double_to_char(charge->fx, tmp);
    description = realloc(description, (strlen(description) + strlen(", Fx :  N") + strlen(tmp)+1)*sizeof(char));
    strcat(description, ", Fx : ");
    strcat(description, tmp);
    strcat(description, " N");
    common_math_double_to_char(charge->fy, tmp);
    description = realloc(description, (strlen(description) + strlen(", Fy :  N") + strlen(tmp)+1)*sizeof(char));
    strcat(description, ", Fy : ");
    strcat(description, tmp);
    strcat(description, " N");
    common_math_double_to_char(charge->fz, tmp);
    description = realloc(description, (strlen(description) + strlen(", Fz :  N") + strlen(tmp)+1)*sizeof(char));
    strcat(description, ", Fz : ");
    strcat(description, tmp);
    strcat(description, " N");
    common_math_double_to_char(charge->mx, tmp);
    description = realloc(description, (strlen(description) + strlen(", Mx :  N") + strlen(tmp)+1)*sizeof(char));
    strcat(description, ", Mx : ");
    strcat(description, tmp);
    strcat(description, " N");
    common_math_double_to_char(charge->my, tmp);
    description = realloc(description, (strlen(description) + strlen(", My :  N") + strlen(tmp)+1)*sizeof(char));
    strcat(description, ", My : ");
    strcat(description, tmp);
    strcat(description, " N");
    common_math_double_to_char(charge->mz, tmp);
    description = realloc(description, (strlen(description) + strlen(", Mz :  N") + strlen(tmp)+1)*sizeof(char));
    strcat(description, ", Mz : ");
    strcat(description, tmp);
    strcat(description, " N");
    
    gtk_tree_store_append(list_gtk_1990_actions->tree_store_charges, &charge->Iter, NULL);
    gtk_tree_store_set(list_gtk_1990_actions->tree_store_charges, &charge->Iter, 0, charge->numero, 1, charge->description, 2, gettext("Ponctuelle sur noeud"), 3, description, -1);
    free(description);
}


/* DEBUT DE LA FENETRE GRAPHIQUE*/


void EF_gtk_charge_noeud_annuler_clicked(GtkButton *button __attribute__((unused)), GtkWidget *fenetre)
/* Description : Ferme la fenêtre sans effectuer les modifications
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    BUGMSG(fenetre, , "_EF_gtk_charge_noeud\n");
    gtk_widget_destroy(fenetre);
    return;
}



void EF_gtk_charge_noeud_ajouter_clicked(GtkButton *button __attribute__((unused)), Projet *projet)
/* Description : Ferme la fenêtre sans effectuer les modifications
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    List_Gtk_EF_Charge_Noeud    *ef_gtk;
    GtkWidget                   *dialog;
    double                      fx, fy, fz, mx, my, mz;
    int                         num_noeud;
    EF_Noeud                    *noeud;
    
    BUGMSG(projet, , "_EF_gtk_charge_noeud\n");
    
    ef_gtk = &projet->list_gtk.ef_charge_noeud;
    
    fx = gtk_common_entry_renvoie_double(gtk_text_view_get_buffer(GTK_TEXT_VIEW(ef_gtk->text_view_fx)));
    if (isnan(fx))
    {
        dialog = gtk_message_dialog_new(GTK_WINDOW(ef_gtk->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, gettext("La valeur de %s est incorrecte."), "Fx");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }
    fy = gtk_common_entry_renvoie_double(gtk_text_view_get_buffer(GTK_TEXT_VIEW(ef_gtk->text_view_fy)));
    if (isnan(fy))
    {
        dialog = gtk_message_dialog_new(GTK_WINDOW(ef_gtk->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, gettext("La valeur de %s est incorrecte."), "Fy");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }
    fz = gtk_common_entry_renvoie_double(gtk_text_view_get_buffer(GTK_TEXT_VIEW(ef_gtk->text_view_fz)));
    if (isnan(fz))
    {
        dialog = gtk_message_dialog_new(GTK_WINDOW(ef_gtk->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, gettext("La valeur de %s est incorrecte."), "Fz");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }
    mx = gtk_common_entry_renvoie_double(gtk_text_view_get_buffer(GTK_TEXT_VIEW(ef_gtk->text_view_mx)));
    if (isnan(mx))
    {
        dialog = gtk_message_dialog_new(GTK_WINDOW(ef_gtk->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, gettext("La valeur de %s est incorrecte."), "Mx");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }
    my = gtk_common_entry_renvoie_double(gtk_text_view_get_buffer(GTK_TEXT_VIEW(ef_gtk->text_view_my)));
    if (isnan(my))
    {
        dialog = gtk_message_dialog_new(GTK_WINDOW(ef_gtk->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, gettext("La valeur de %s est incorrecte."), "My");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }
    mz = gtk_common_entry_renvoie_double(gtk_text_view_get_buffer(GTK_TEXT_VIEW(ef_gtk->text_view_mz)));
    if (isnan(mz))
    {
        dialog = gtk_message_dialog_new(GTK_WINDOW(ef_gtk->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, gettext("La valeur de %s est incorrecte."), "Mz");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }
    num_noeud = gtk_common_entry_renvoie_int(gtk_text_view_get_buffer(GTK_TEXT_VIEW(ef_gtk->text_view_noeud)));
    if (num_noeud == -1)
    {
        dialog = gtk_message_dialog_new(GTK_WINDOW(ef_gtk->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, gettext("La valeur du noeud est incorrecte."));
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }
    else
    {
        noeud = EF_noeuds_cherche_numero(projet, num_noeud);
        if (noeud == NULL)
        {
            dialog = gtk_message_dialog_new(GTK_WINDOW(ef_gtk->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, gettext("Le noeud %d n'existe pas."), num_noeud);
            gtk_dialog_run(GTK_DIALOG(dialog));
            gtk_widget_destroy(dialog);
            return;
        }
        else
        {
            // Si tous les paramètres sont corrects
            GtkTextIter     start, end;
            GtkTextBuffer   *textbuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(ef_gtk->text_view_description));
            gchar           *texte;
            gint            num_action;
            Charge_Noeud    *charge_noeud;
            
            gtk_text_buffer_get_iter_at_offset(textbuffer, &start, 0);
            gtk_text_buffer_get_iter_at_offset(textbuffer, &end, -1);
            texte = gtk_text_buffer_get_text(textbuffer, &start, &end, FALSE);
            
            num_action = gtk_combo_box_get_active(GTK_COMBO_BOX(ef_gtk->combobox_charge));
            
            // Création de la nouvelle charge ponctuelle au noeud
            charge_noeud = EF_charge_noeud_ajout(projet, num_action, noeud, fx, fy, fz, mx, my, mz, texte);
            BUG(charge_noeud, );
            
            free(texte);
            
            // Actualisation de l'affichage graphique
            EF_gtk_charge_noeud_ajout_affichage(charge_noeud, projet);
            //if ()
            
            return;
        }
    }
}


void EF_gtk_charge_noeud(Projet *projet, gboolean nouveau, gint action_defaut)
/* Description : Affichage de la fenêtre permettant de créer ou modifier une action de type
 *               charge ponctuelle au noeud
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 *            : gboolean nouveau : vaut TRUE si une nouvelle charge doit être ajoutée,
 *                                 vaut FALSE si la charge en cours doit être modifiée
 * Valeur renvoyée : Aucune
 */
{
    List_Gtk_EF_Charge_Noeud *ef_gtk;
    
    BUGMSG(projet, , "_EF_gtk_charge_noeud\n");
    BUGMSG(projet->actions, , "_EF_gtk_charge_noeud\n");
    
    BUGMSG(list_size(projet->actions) > 0, , "_EF_gtk_charge_noeud\n");
    
    ef_gtk = &projet->list_gtk.ef_charge_noeud;
    
    if (nouveau == TRUE)
        GTK_NOUVELLE_FENETRE(ef_gtk->window, gettext("Ajout d'une charge au noeud"), 400, 1)
    else
        GTK_NOUVELLE_FENETRE(ef_gtk->window, gettext("Modification d'une charge au noeud"), 400, 1)
    
    ef_gtk->table = gtk_table_new(7, 4, FALSE);
    gtk_container_add(GTK_CONTAINER(ef_gtk->window), ef_gtk->table);
    
    ef_gtk->label_charge = gtk_label_new(gettext("Charge :"));
    gtk_table_attach(GTK_TABLE(ef_gtk->table), ef_gtk->label_charge, 0, 1, 0, 1, GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
    
    ef_gtk->combobox_charge = gtk_combo_box_text_new();
    list_mvfront(projet->actions);
    do
    {
        Action *action = list_curr(projet->actions);
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(ef_gtk->combobox_charge), action->description);
    }
    while (list_mvnext(projet->actions) != NULL);
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
    
    ef_gtk->label_noeud = gtk_label_new(gettext("Noeud :"));
    gtk_table_attach(GTK_TABLE(ef_gtk->table), ef_gtk->label_noeud, 0, 1, 5, 6, GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
    GTK_NOUVEAU_TEXT_VIEW_AVEC_SCROLLED_WINDOW(ef_gtk->text_view_noeud, ef_gtk->sw_noeud)
    GTK_TEXT_VIEW_VERIFIE_INT(ef_gtk->text_view_noeud)
    gtk_table_attach(GTK_TABLE(ef_gtk->table), ef_gtk->sw_noeud, 1, 4, 5, 6, GTK_EXPAND | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
    
    ef_gtk->table_buttons = gtk_table_new(1, 2, FALSE);
    gtk_table_attach(GTK_TABLE(ef_gtk->table), ef_gtk->table_buttons, 0, 4, 6, 7, GTK_EXPAND | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
    ef_gtk->button_ajouter = gtk_button_new_with_label(gettext("Ajouter"));
    g_signal_connect(ef_gtk->button_ajouter, "clicked", G_CALLBACK(EF_gtk_charge_noeud_ajouter_clicked), projet);
    gtk_table_attach(GTK_TABLE(ef_gtk->table_buttons), ef_gtk->button_ajouter, 0, 1, 0, 1, GTK_EXPAND | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
    ef_gtk->button_annuler = gtk_button_new_with_label(gettext("Annuler"));
    g_signal_connect(ef_gtk->button_annuler, "clicked", G_CALLBACK(EF_gtk_charge_noeud_annuler_clicked), ef_gtk->window);
    gtk_table_attach(GTK_TABLE(ef_gtk->table_buttons), ef_gtk->button_annuler, 1, 2, 0, 1, GTK_EXPAND | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
    
    if (projet->list_gtk._1990_actions.window == NULL)
        gtk_window_set_transient_for(GTK_WINDOW(ef_gtk->window), GTK_WINDOW(projet->list_gtk.comp.window));
    else
        gtk_window_set_transient_for(GTK_WINDOW(ef_gtk->window), GTK_WINDOW(projet->list_gtk._1990_actions.window));
    gtk_window_set_modal(GTK_WINDOW(ef_gtk->window), TRUE);
    gtk_widget_show_all(ef_gtk->window);
    
    return;
}


#endif
