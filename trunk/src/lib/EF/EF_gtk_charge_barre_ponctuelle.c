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

void EF_gtk_charge_barre_ponctuelle_ajout_affichage(Charge_Barre_Ponctuelle *charge, Projet *projet, gboolean nouvelle_ligne)
{
    char                    *description, tmp[30];
    List_Gtk_1990_Actions   *list_gtk_1990_actions = &projet->list_gtk._1990_actions;
    
    if (list_gtk_1990_actions->window == NULL)
        return;
    
    description = malloc(sizeof(char)*(strlen(gettext("Barre : "))+1));
    strcpy(description, gettext("Barre : "));
    sprintf(tmp, "%d", charge->barre->numero);
    description = realloc(description, (strlen(description) + strlen(tmp)+1)*sizeof(char));
    strcat(description, tmp);
    common_math_double_to_char(charge->position, tmp);
    description = realloc(description, (strlen(description) + strlen(",  :  m") + strlen(gettext("position")) + strlen(tmp) + 1)*sizeof(char));
    strcat(description, ", ");
    strcat(description, gettext("position"));
    strcat(description, " : ");
    strcat(description, tmp);
    strcat(description, " m");
    if (charge->repere_local)
    {
        description = realloc(description, (strlen(description) + strlen(", ") + strlen(gettext("repère : local")) + strlen(tmp) + 1)*sizeof(char));
        strcat(description, ", ");
        strcat(description, gettext("repère : local"));
    }
    else
    {
        description = realloc(description, (strlen(description) + strlen(", ") + strlen(gettext("repère : global")) + strlen(tmp) + 1)*sizeof(char));
        strcat(description, ", ");
        strcat(description, gettext("repère : global"));
    }
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
    
    if (nouvelle_ligne == TRUE)
        gtk_tree_store_append(list_gtk_1990_actions->tree_store_charges, &charge->Iter, NULL);
    gtk_tree_store_set(list_gtk_1990_actions->tree_store_charges, &charge->Iter, 0, charge->numero, 1, charge->description, 2, gettext("Ponctuelle sur barre"), 3, description, -1);
    free(description);
}


/* DEBUT DE LA FENETRE GRAPHIQUE*/

void EF_gtk_charge_barre_ponctuelle_annuler_clicked(GtkButton *button __attribute__((unused)), GtkWidget *fenetre)
/* Description : Ferme la fenêtre sans effectuer les modifications
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    BUGMSG(fenetre, , "EF_gtk_charge_barre_ponctuelle_annuler_clicked\n");
    gtk_widget_destroy(fenetre);
    return;
}


gboolean EF_gtk_charge_barre_ponctuelle_recupere_donnees(Projet *projet, int *num_action, Beton_Barre **barre, double *fx, double *fy, double *fz, double *mx, double *my, double *mz, gchar **description, int *repere_local, double *position)
{
    GtkWidget                           *dialog;
    List_Gtk_EF_Charge_Barre_Ponctuelle *ef_gtk;
    int                                 num_barre;
    
    BUGMSG(projet, FALSE, "EF_gtk_charge_barre_ponctuelle_recupere_donnees\n");
    
    ef_gtk = &projet->list_gtk.ef_charge_barre_ponctuelle;
    
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
    *position = gtk_common_entry_renvoie_double(gtk_text_view_get_buffer(GTK_TEXT_VIEW(ef_gtk->text_view_position)));
    if (isnan(*position))
    {
        dialog = gtk_message_dialog_new(GTK_WINDOW(ef_gtk->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, gettext("La valeur de la position est incorrecte."));
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return FALSE;
    }
    *repere_local = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ef_gtk->check_button_repere_local));
    num_barre = gtk_common_entry_renvoie_int(gtk_text_view_get_buffer(GTK_TEXT_VIEW(ef_gtk->text_view_barre)));
    if (num_barre == -1)
    {
        dialog = gtk_message_dialog_new(GTK_WINDOW(ef_gtk->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, gettext("La valeur de la barre est incorrecte."));
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return FALSE;
    }
    else
    {
        *barre = _1992_1_1_barres_cherche_numero(projet, num_barre);
        if (*barre == NULL)
        {
            dialog = gtk_message_dialog_new(GTK_WINDOW(ef_gtk->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, gettext("La barre %d n'existe pas."), num_barre);
            gtk_dialog_run(GTK_DIALOG(dialog));
            gtk_widget_destroy(dialog);
            return FALSE;
        }
        else
        {
            // Si tous les paramètres sont corrects
            GtkTextIter     start, end;
            GtkTextBuffer   *textbuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(ef_gtk->text_view_description));
            
            gtk_text_buffer_get_iter_at_offset(textbuffer, &start, 0);
            gtk_text_buffer_get_iter_at_offset(textbuffer, &end, -1);
            *description = gtk_text_buffer_get_text(textbuffer, &start, &end, FALSE);
            
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
    int                         num_action, repere_local;
    Beton_Barre                 *barre;
    gchar                       *texte;
    Charge_Barre_Ponctuelle     *charge;
    GtkTreeModel                *model_action;
    GtkTreeIter                 iter_action;
    int                         numero_action;
    
    BUGMSG(projet, , "EF_gtk_charge_barre_ponctuelle_ajouter_clicked\n");
    
    if (!EF_gtk_charge_barre_ponctuelle_recupere_donnees(projet, &num_action, &barre, &fx, &fy, &fz, &mx, &my, &mz, &texte, &repere_local, &position))
        return;
    
    // Création de la nouvelle charge ponctuelle sur barre
    charge = EF_charge_barre_ponctuelle_ajout(projet, num_action, barre, repere_local, position, fx, fy, fz, mx, my, mz, texte);
    BUG(charge, );
    
    free(texte);
    
    // Actualisation de l'affichage graphique
    if (!gtk_tree_selection_get_selected(projet->list_gtk._1990_actions.tree_select_actions, &model_action, &iter_action))
        return;
    gtk_tree_model_get(model_action, &iter_action, 0, &numero_action, -1);
    if (numero_action == num_action)
        EF_gtk_charge_barre_ponctuelle_ajout_affichage(charge, projet, TRUE);
    
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
    int                         num_action, repere_local;
    Beton_Barre                 *barre;
    gchar                       *texte;
    Charge_Barre_Ponctuelle     *charge;
    
    BUGMSG(projet, , "_EF_gtk_charge_barre_ponctuelle_editer_clicked\n");
    
    ef_gtk = &projet->list_gtk.ef_charge_barre_ponctuelle;
    
    if (!EF_gtk_charge_barre_ponctuelle_recupere_donnees(projet, &num_action, &barre, &fx, &fy, &fz, &mx, &my, &mz, &texte, &repere_local, &position))
        return;
    
    // Création de la nouvelle charge ponctuelle sur barre
    charge = _1990_action_cherche_charge(projet, ef_gtk->action, ef_gtk->charge);
    free(charge->description);
    charge->description = texte;
    charge->fx = fx;
    charge->fy = fy;
    charge->fz = fz;
    charge->mx = mx;
    charge->my = my;
    charge->mz = mz;
    charge->barre = barre;
    charge->position = position;
    charge->repere_local = repere_local;
    if (num_action != ef_gtk->action)
        _1990_action_deplace_charge(projet, ef_gtk->action, ef_gtk->charge, num_action);
    else
        EF_gtk_charge_barre_ponctuelle_ajout_affichage(charge, projet, FALSE);
    
    return;
}


void EF_gtk_charge_barre_ponctuelle(Projet *projet, gint action_defaut, gint charge)
/* Description : Affichage de la fenêtre permettant de créer ou modifier une action de type
 *               charge ponctuelle sur barre
 * Paramètres : Projet *projet : la variable projet
 *              gint action_defaut : action par défaut dans la fenêtre,
 *              gint charge : vaut -1 si une nouvelle charge doit être ajoutée,
 *                            vaut le numéro de la charge si elle doit être modifiée
 * Valeur renvoyée : Aucune
 */
{
    List_Gtk_EF_Charge_Barre_Ponctuelle    *ef_gtk;
    Charge_Barre_Ponctuelle                *charge_barre;
    
    BUGMSG(projet, , "EF_gtk_charge_barre_ponctuelle\n");
    BUGMSG(projet->actions, , "EF_gtk_charge_barre_ponctuelle\n");
    
    BUGMSG(list_size(projet->actions) > 0, , "EF_gtk_charge_barre_ponctuelle\n");
    
    ef_gtk = &projet->list_gtk.ef_charge_barre_ponctuelle;
    
    if (charge == -1)
    {
        GTK_NOUVELLE_FENETRE(ef_gtk->window, gettext("Ajout d'une charge ponctuelle sur barre"), 400, 1)
        charge_barre = NULL;
    }
    else
    {
        ef_gtk->action = action_defaut;
        ef_gtk->charge = charge;
        GTK_NOUVELLE_FENETRE(ef_gtk->window, gettext("Modification d'une charge ponctuelle sur barre"), 400, 1)
        charge_barre = _1990_action_cherche_charge(projet, action_defaut, charge);
        BUG(charge_barre, );
    }
    
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
    
    ef_gtk->check_button_repere_local = gtk_check_button_new_with_label(gettext("Repère local"));
    gtk_table_attach(GTK_TABLE(ef_gtk->table), ef_gtk->check_button_repere_local, 0, 2, 5, 6, GTK_EXPAND | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
    
    ef_gtk->label_position = gtk_label_new(gettext("Position :"));
    gtk_table_attach(GTK_TABLE(ef_gtk->table), ef_gtk->label_position, 2, 3, 5, 6, GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 10, 0);
    GTK_NOUVEAU_TEXT_VIEW_AVEC_SCROLLED_WINDOW(ef_gtk->text_view_position, ef_gtk->sw_position)
    GTK_TEXT_VIEW_VERIFIE_DOUBLE(ef_gtk->text_view_position)
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(ef_gtk->text_view_position)), "0", -1);
    gtk_table_attach(GTK_TABLE(ef_gtk->table), ef_gtk->sw_position, 3, 4, 5, 6, GTK_EXPAND | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
    
    ef_gtk->label_barre = gtk_label_new(gettext("Barre :"));
    gtk_table_attach(GTK_TABLE(ef_gtk->table), ef_gtk->label_barre, 0, 1, 6, 7, GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
    GTK_NOUVEAU_TEXT_VIEW_AVEC_SCROLLED_WINDOW(ef_gtk->text_view_barre, ef_gtk->sw_barre)
    GTK_TEXT_VIEW_VERIFIE_INT(ef_gtk->text_view_barre)
    gtk_table_attach(GTK_TABLE(ef_gtk->table), ef_gtk->sw_barre, 1, 4, 6, 7, GTK_EXPAND | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
    
    if (charge_barre != NULL)
    {
        gchar   tmp[30];
        gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(ef_gtk->text_view_description)), charge_barre->description, -1);
        common_math_double_to_char(charge_barre->fx, tmp);
        gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(ef_gtk->text_view_fx)), tmp, -1);
        common_math_double_to_char(charge_barre->fy, tmp);
        gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(ef_gtk->text_view_fy)), tmp, -1);
        common_math_double_to_char(charge_barre->fz, tmp);
        gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(ef_gtk->text_view_fz)), tmp, -1);
        common_math_double_to_char(charge_barre->mx, tmp);
        gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(ef_gtk->text_view_mx)), tmp, -1);
        common_math_double_to_char(charge_barre->my, tmp);
        gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(ef_gtk->text_view_my)), tmp, -1);
        common_math_double_to_char(charge_barre->mz, tmp);
        gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(ef_gtk->text_view_mz)), tmp, -1);
        common_math_double_to_char(charge_barre->position, tmp);
        gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(ef_gtk->text_view_position)), tmp, -1);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ef_gtk->check_button_repere_local), charge_barre->repere_local);
        sprintf(tmp, "%d", charge_barre->barre->numero);
        gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(ef_gtk->text_view_barre)), tmp, -1);
    }
    ef_gtk->table_buttons = gtk_table_new(1, 2, FALSE);
    gtk_table_attach(GTK_TABLE(ef_gtk->table), ef_gtk->table_buttons, 0, 4, 7, 8, GTK_EXPAND | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
    if (charge == -1)
    {
        ef_gtk->button_ajouter = gtk_button_new_from_stock(GTK_STOCK_ADD);
        g_signal_connect(ef_gtk->button_ajouter, "clicked", G_CALLBACK(EF_gtk_charge_barre_ponctuelle_ajouter_clicked), projet);
    }
    else
    {
        ef_gtk->button_ajouter = gtk_button_new_from_stock(GTK_STOCK_EDIT);
        g_signal_connect(ef_gtk->button_ajouter, "clicked", G_CALLBACK(EF_gtk_charge_barre_ponctuelle_editer_clicked), projet);
    }
    gtk_table_attach(GTK_TABLE(ef_gtk->table_buttons), ef_gtk->button_ajouter, 0, 1, 0, 1, GTK_EXPAND | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
    ef_gtk->button_annuler = gtk_button_new_from_stock(GTK_STOCK_CANCEL);
    g_signal_connect(ef_gtk->button_annuler, "clicked", G_CALLBACK(EF_gtk_charge_barre_ponctuelle_annuler_clicked), ef_gtk->window);
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
