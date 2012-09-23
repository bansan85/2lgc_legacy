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
#include "EF_charge.h"
#include "1990_actions.h"
#include "1990_gtk_actions.h"
#include "common_selection.h"

G_MODULE_EXPORT gboolean EF_gtk_charge_noeud_ajout_affichage(Charge_Noeud *charge,
  Projet *projet, gboolean nouvelle_ligne)
/* Description : Ajoute une ligne contenant les informations sur une charge ponctuelle au
 *               tableau dans la fenêtre des actions.
 * Paramètres : Charge_Barre_Ponctuelle *charge : charge à ajouter,
 *            : Projet *projet : la variable projet,
 *            : gboolean nouvelle_ligne : Ajoute une ligne ou modifie la ligne existante.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL,
 *             en cas d'erreur d'allocation mémoire.
 */
{
    char    *description, txt_fx[30], txt_fy[30], txt_fz[30], txt_mx[30], txt_my[30], txt_mz[30], *txt_liste_noeuds;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    
    if (projet->list_gtk._1990_actions.builder == NULL)
        return TRUE;
     
    BUG(txt_liste_noeuds = common_selection_converti_noeuds_en_texte(charge->noeuds), FALSE);
    common_math_double_to_char(charge->fx, txt_fx, DECIMAL_FORCE);
    common_math_double_to_char(charge->fy, txt_fy, DECIMAL_FORCE);
    common_math_double_to_char(charge->fz, txt_fz, DECIMAL_FORCE);
    common_math_double_to_char(charge->mx, txt_mx, DECIMAL_MOMENT);
    common_math_double_to_char(charge->my, txt_my, DECIMAL_MOMENT);
    common_math_double_to_char(charge->mz, txt_mz, DECIMAL_MOMENT);
    
    BUGMSG(description = g_strdup_printf("%s : %s, Fx : %s N, Fy : %s N, Fz : %s N, Mx : %s N.m, My : %s N.m, Mz : %s N.m", strstr(txt_liste_noeuds, ";") == NULL ? gettext("Noeud") : gettext("Noeuds"), txt_liste_noeuds, txt_fx, txt_fy, txt_fz, txt_mx, txt_my, txt_mz), FALSE, gettext("Erreur d'allocation mémoire.\n"));
    
    free(txt_liste_noeuds);
    
    if (nouvelle_ligne == TRUE)
        gtk_tree_store_append(projet->list_gtk._1990_actions.tree_store_charges, &charge->Iter, NULL);
    gtk_tree_store_set(projet->list_gtk._1990_actions.tree_store_charges, &charge->Iter, 0, charge->numero, 1, charge->nom, 2, gettext("Ponctuelle sur noeud"), 3, description, -1);
    
    free(description);
    
    return TRUE;
}


/* DEBUT DE LA FENETRE GRAPHIQUE*/


G_MODULE_EXPORT void EF_gtk_charge_noeud_annuler_clicked(
  GtkButton *button __attribute__((unused)), Projet *projet)
/* Description : Ferme la fenêtre sans effectuer les modifications.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_charge_noeud.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Charge Nodale");
    
    gtk_widget_destroy(projet->list_gtk.ef_charge_noeud.window);
    
    return;
}


gboolean EF_gtk_charge_noeud_recupere_donnees(Projet *projet, unsigned int *num_action,
  GList **noeuds, double *fx, double *fy, double *fz, double *mx, double *my, double *mz,
  gchar **nom)
/* Description : Récupère toutes les données de la fenêtre permettant d'ajouter ou d'éditer une
 *               charge nodale.
 * Paramètres : Projet *projet : la variable projet,
 *            : unsigned int *num_action : numéro de l'action où sera ajoutée la charge,
 *            : GList **noeuds : liste des noeuds qui supportera la charge,
 *            : double *fx : force selon x,
 *            : double *fy : force selon y,
 *            : double *fz : force selon z,
 *            : double *mx : moment selon x,
 *            : double *my : moment selon y,
 *            : double *mz : moment selon z,
 *            : gchar **nom : nom de l'action.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL, num_action == NULL, noeuds == NULL, fx == NULL, fy == NULL,
 *             fz == NULL, mx == NULL, my == NULL, mz == NULL, nom == NULL,
 *             en cas d'erreur d'allocation mémoire.
 */
{
    GtkWidget           *dialog;
    Gtk_EF_Charge_Noeud *ef_gtk;
    GList               *num_noeuds;
    GtkTextIter         start, end;
    gchar               *texte_tmp;
    GtkTextBuffer       *textbuffer;
    gint                get_active;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(num_action, FALSE, gettext("Paramètre %s incorrect.\n"), "num_action");
    BUGMSG(noeuds, FALSE, gettext("Paramètre %s incorrect.\n"), "noeuds");
    BUGMSG(fx, FALSE, gettext("Paramètre %s incorrect.\n"), "fx");
    BUGMSG(fy, FALSE, gettext("Paramètre %s incorrect.\n"), "fy");
    BUGMSG(fz, FALSE, gettext("Paramètre %s incorrect.\n"), "fz");
    BUGMSG(mx, FALSE, gettext("Paramètre %s incorrect.\n"), "mx");
    BUGMSG(my, FALSE, gettext("Paramètre %s incorrect.\n"), "my");
    BUGMSG(mz, FALSE, gettext("Paramètre %s incorrect.\n"), "mz");
    BUGMSG(nom, FALSE, gettext("Paramètre %s incorrect.\n"), "nom");
    BUGMSG(projet->list_gtk.ef_charge_noeud.builder, FALSE, gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Charge Nodale");
    
    ef_gtk = &projet->list_gtk.ef_charge_noeud;
    
    get_active = gtk_combo_box_get_active(GTK_COMBO_BOX(ef_gtk->combobox_charge));
    if (get_active < 0)
        return FALSE;
    else
        *num_action = (unsigned int)get_active;
    
    *fx = gtk_common_entry_renvoie_double(GTK_TEXT_BUFFER(gtk_builder_get_object(projet->list_gtk.ef_charge_noeud.builder, "EF_charge_noeud_buffer_fx")));
    if (isnan(*fx))
    {
        dialog = gtk_message_dialog_new(GTK_WINDOW(ef_gtk->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, gettext("La valeur de %s est incorrecte."), "Fx");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return FALSE;
    }
    
    *fy = gtk_common_entry_renvoie_double(GTK_TEXT_BUFFER(gtk_builder_get_object(projet->list_gtk.ef_charge_noeud.builder, "EF_charge_noeud_buffer_fy")));
    if (isnan(*fy))
    {
        dialog = gtk_message_dialog_new(GTK_WINDOW(ef_gtk->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, gettext("La valeur de %s est incorrecte."), "Fy");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return FALSE;
    }
    
    *fz = gtk_common_entry_renvoie_double(GTK_TEXT_BUFFER(gtk_builder_get_object(projet->list_gtk.ef_charge_noeud.builder, "EF_charge_noeud_buffer_fz")));
    if (isnan(*fz))
    {
        dialog = gtk_message_dialog_new(GTK_WINDOW(ef_gtk->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, gettext("La valeur de %s est incorrecte."), "Fz");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return FALSE;
    }
    
    *mx = gtk_common_entry_renvoie_double(GTK_TEXT_BUFFER(gtk_builder_get_object(projet->list_gtk.ef_charge_noeud.builder, "EF_charge_noeud_buffer_mx")));
    if (isnan(*mx))
    {
        dialog = gtk_message_dialog_new(GTK_WINDOW(ef_gtk->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, gettext("La valeur de %s est incorrecte."), "Mx");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return FALSE;
    }
    
    *my = gtk_common_entry_renvoie_double(GTK_TEXT_BUFFER(gtk_builder_get_object(projet->list_gtk.ef_charge_noeud.builder, "EF_charge_noeud_buffer_my")));
    if (isnan(*my))
    {
        dialog = gtk_message_dialog_new(GTK_WINDOW(ef_gtk->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, gettext("La valeur de %s est incorrecte."), "My");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return FALSE;
    }
    
    *mz = gtk_common_entry_renvoie_double(GTK_TEXT_BUFFER(gtk_builder_get_object(projet->list_gtk.ef_charge_noeud.builder, "EF_charge_noeud_buffer_mz")));
    if (isnan(*mz))
    {
        dialog = gtk_message_dialog_new(GTK_WINDOW(ef_gtk->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, gettext("La valeur de %s est incorrecte."), "Mz");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return FALSE;
    }
    
    textbuffer = GTK_TEXT_BUFFER(gtk_builder_get_object(projet->list_gtk.ef_charge_noeud.builder, "EF_charge_noeud_buffer_noeuds"));
    gtk_text_buffer_get_iter_at_offset(textbuffer, &start, 0);
    gtk_text_buffer_get_iter_at_offset(textbuffer, &end, -1);
    texte_tmp = gtk_text_buffer_get_text(textbuffer, &start, &end, FALSE);
    num_noeuds = common_selection_renvoie_numeros(texte_tmp);
    if (num_noeuds == NULL)
    {
        dialog = gtk_message_dialog_new(GTK_WINDOW(ef_gtk->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, gettext("La valeur des noeuds est incorrecte."));
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        free(texte_tmp);
        return FALSE;
    }
    else
    {
        *noeuds = common_selection_converti_numeros_en_noeuds(num_noeuds, projet);
        if (*noeuds == NULL)
        {
            dialog = gtk_message_dialog_new(GTK_WINDOW(ef_gtk->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, gettext("La liste de noeuds %s n'existe pas."), texte_tmp);
            gtk_dialog_run(GTK_DIALOG(dialog));
            gtk_widget_destroy(dialog);
            free(texte_tmp);
            return FALSE;
        }
        else
        {
            // Si tous les paramètres sont corrects
            textbuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(gtk_builder_get_object(projet->list_gtk.ef_charge_noeud.builder, "EF_charge_noeud_textview_description")));
            
            free(texte_tmp);
            gtk_text_buffer_get_iter_at_offset(textbuffer, &start, 0);
            gtk_text_buffer_get_iter_at_offset(textbuffer, &end, -1);
            *nom = gtk_text_buffer_get_text(textbuffer, &start, &end, FALSE);
            
            return TRUE;
        }
    }
}


void EF_gtk_charge_noeud_ajouter_clicked(GtkButton *button __attribute__((unused)),
  Projet *projet)
/* Description : Ferme la fenêtre en ajoutant la charge.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    double          fx, fy, fz, mx, my, mz;
    unsigned int    num_action;
    GList           *noeuds;
    gchar           *texte;
    Charge_Noeud    *charge_noeud;
    GtkTreeIter     iter_action;
    GtkTreeModel    *model_action;
    unsigned int    numero_action;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_charge_noeud.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Charge Nodale");
    
    BUG(EF_gtk_charge_noeud_recupere_donnees(projet, &num_action, &noeuds, &fx, &fy, &fz, &mx, &my, &mz, &texte) == TRUE, );
    
    // Création de la nouvelle charge ponctuelle au noeud
    BUG(charge_noeud = EF_charge_noeud_ajout(projet, num_action, noeuds, fx, fy, fz, mx, my, mz, texte), );
    
    free(texte);
    
    // Actualisation de l'affichage graphique
    if (!gtk_tree_selection_get_selected(projet->list_gtk._1990_actions.tree_select_actions, &model_action, &iter_action))
        return;
    gtk_tree_model_get(model_action, &iter_action, 0, &numero_action, -1);
    if (numero_action == num_action)
        BUG(EF_gtk_charge_noeud_ajout_affichage(charge_noeud, projet, TRUE), );
    
    gtk_widget_destroy(projet->list_gtk.ef_charge_noeud.window);
    
    return;
}


void EF_gtk_charge_noeud_editer_clicked(GtkButton *button __attribute__((unused)), Projet *projet)
/* Description : Ferme la fenêtre en appliquant les modifications.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    Gtk_EF_Charge_Noeud *ef_gtk;
    double              fx, fy, fz, mx, my, mz;
    unsigned int        num_action;
    GList               *noeuds;
    gchar               *texte;
    Charge_Noeud        *charge_noeud;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_charge_noeud.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Charge Nodale");
    
    ef_gtk = &projet->list_gtk.ef_charge_noeud;
    
    BUG(EF_gtk_charge_noeud_recupere_donnees(projet, &num_action, &noeuds, &fx, &fy, &fz, &mx, &my, &mz, &texte) == TRUE, );
    
    // Création de la nouvelle charge ponctuelle au noeud
    BUG(charge_noeud = EF_charge_cherche(projet, ef_gtk->action, ef_gtk->charge), );
    free(charge_noeud->nom);
    charge_noeud->nom = texte;
    charge_noeud->fx = fx;
    charge_noeud->fy = fy;
    charge_noeud->fz = fz;
    charge_noeud->mx = mx;
    charge_noeud->my = my;
    charge_noeud->mz = mz;
    g_list_free(charge_noeud->noeuds);
    charge_noeud->noeuds = noeuds;
    if (num_action != ef_gtk->action)
        BUG(EF_charge_deplace(projet, ef_gtk->action, ef_gtk->charge, num_action), );
    else
        BUG(EF_gtk_charge_noeud_ajout_affichage(charge_noeud, projet, FALSE), );
    
    gtk_widget_destroy(projet->list_gtk.ef_charge_noeud.window);
    
    return;
}


G_MODULE_EXPORT gboolean EF_gtk_charge_noeud_window_key_press(
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
    BUGMSG(projet->list_gtk.ef_charge_noeud.builder, FALSE, gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Charge Nodale");
    
    if (event->key.keyval == GDK_KEY_Escape)
    {
        gtk_widget_destroy(projet->list_gtk.ef_charge_noeud.window);
        return TRUE;
    }
    else
        return FALSE;
}


G_MODULE_EXPORT void EF_gtk_charge_noeud_window_destroy(
  GtkWidget *object __attribute__((unused)), Projet *projet)
/* Description : met projet->list_gtk.ef_charge_noeud.builder à NULL quand la fenêtre se ferme.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_charge_noeud.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Charge Nodale");
    
    projet->list_gtk.ef_charge_noeud.builder = NULL;
    
    return;
}


G_MODULE_EXPORT gboolean EF_gtk_charge_noeud(Projet *projet, unsigned int action_defaut, unsigned int charge)
/* Description : Affichage de la fenêtre permettant de créer ou modifier une action de type
 *               charge ponctuelle au noeud.
 * Paramètres : Projet *projet : la variable projet
 *            : unsigned int action_defaut : action par défaut dans la fenêtre,
 *            : unsigned int charge : vaut G_MAXUINT si une nouvelle charge doit être ajoutée,
 *                                    vaut le numéro de la charge si elle doit être modifiée.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Echec : FALSE :
 *             projet == NULL,
 *             Fenêtre graphique déjà initialisée.
 */
{
    Gtk_EF_Charge_Noeud *ef_gtk;
    Charge_Noeud        *charge_noeud;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_charge_noeud.builder == NULL, FALSE, gettext("La fenêtre graphique %s est déjà initialisée.\n"), "Charge Nodale");
    
    ef_gtk = &projet->list_gtk.ef_charge_noeud;
    projet->list_gtk.ef_charge_noeud.builder = gtk_builder_new();
    BUGMSG(gtk_builder_add_from_file(projet->list_gtk.ef_charge_noeud.builder, DATADIR"/ui/EF_charge_noeud.ui", NULL) != 0, FALSE, gettext("Builder Failed\n"));
    gtk_builder_connect_signals(projet->list_gtk.ef_charge_noeud.builder, projet);
    
    ef_gtk->window = GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_charge_noeud.builder, "EF_charge_noeud_window"));
    ef_gtk->combobox_charge = GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_charge_noeud.builder, "EF_charge_noeud_combo_box_charge"));
    
    if (charge == G_MAXUINT)
    {
        ef_gtk->action = 0;
        ef_gtk->charge = 0;
        gtk_window_set_title(GTK_WINDOW(ef_gtk->window), gettext("Ajout d'une charge au noeud"));
        charge_noeud = NULL;
    }
    else
    {
        ef_gtk->action = action_defaut;
        ef_gtk->charge = charge;
        gtk_window_set_title(GTK_WINDOW(ef_gtk->window), gettext("Modification d'une charge au noeud"));
        BUG(charge_noeud = EF_charge_cherche(projet, action_defaut, charge), FALSE);
    }
    
    gtk_combo_box_set_model(GTK_COMBO_BOX(ef_gtk->combobox_charge), GTK_TREE_MODEL(projet->list_gtk._1990_actions.list_actions));
    gtk_combo_box_set_active(GTK_COMBO_BOX(ef_gtk->combobox_charge), (gint)action_defaut);
    
    if (charge_noeud != NULL)
    {
        char   tmp[30], *tmp2;
        gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(gtk_builder_get_object(projet->list_gtk.ef_charge_noeud.builder, "EF_charge_noeud_textview_description"))), charge_noeud->nom, -1);
        common_math_double_to_char(charge_noeud->fx, tmp, DECIMAL_FORCE);
        gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gtk_builder_get_object(projet->list_gtk.ef_charge_noeud.builder, "EF_charge_noeud_buffer_fx")), tmp, -1);
        common_math_double_to_char(charge_noeud->fy, tmp, DECIMAL_FORCE);
        gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gtk_builder_get_object(projet->list_gtk.ef_charge_noeud.builder, "EF_charge_noeud_buffer_fy")), tmp, -1);
        common_math_double_to_char(charge_noeud->fz, tmp, DECIMAL_FORCE);
        gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gtk_builder_get_object(projet->list_gtk.ef_charge_noeud.builder, "EF_charge_noeud_buffer_fz")), tmp, -1);
        common_math_double_to_char(charge_noeud->mx, tmp, DECIMAL_MOMENT);
        gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gtk_builder_get_object(projet->list_gtk.ef_charge_noeud.builder, "EF_charge_noeud_buffer_mx")), tmp, -1);
        common_math_double_to_char(charge_noeud->my, tmp, DECIMAL_MOMENT);
        gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gtk_builder_get_object(projet->list_gtk.ef_charge_noeud.builder, "EF_charge_noeud_buffer_my")), tmp, -1);
        common_math_double_to_char(charge_noeud->mz, tmp, DECIMAL_MOMENT);
        gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gtk_builder_get_object(projet->list_gtk.ef_charge_noeud.builder, "EF_charge_noeud_buffer_mz")), tmp, -1);
        BUG(tmp2 = common_selection_converti_noeuds_en_texte(charge_noeud->noeuds), FALSE);
        gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gtk_builder_get_object(projet->list_gtk.ef_charge_noeud.builder, "EF_charge_noeud_buffer_noeuds")), tmp2, -1);
        free(tmp2);
    }
    
    if (charge == G_MAXUINT)
    {
        gtk_button_set_label(GTK_BUTTON(gtk_builder_get_object(projet->list_gtk.ef_charge_noeud.builder, "EF_charge_noeud_button_add_edit")), "gtk-add");
        g_signal_connect(gtk_builder_get_object(projet->list_gtk.ef_charge_noeud.builder, "EF_charge_noeud_button_add_edit"), "clicked", G_CALLBACK(EF_gtk_charge_noeud_ajouter_clicked), projet);
    }
    else
    {
        gtk_button_set_label(GTK_BUTTON(gtk_builder_get_object(projet->list_gtk.ef_charge_noeud.builder, "EF_charge_noeud_button_add_edit")), "gtk-edit");
        g_signal_connect(gtk_builder_get_object(projet->list_gtk.ef_charge_noeud.builder, "EF_charge_noeud_button_add_edit"), "clicked", G_CALLBACK(EF_gtk_charge_noeud_editer_clicked), projet);
    }
    
    if (projet->list_gtk._1990_actions.window == NULL)
        gtk_window_set_transient_for(GTK_WINDOW(ef_gtk->window), GTK_WINDOW(projet->list_gtk.comp.window));
    else
        gtk_window_set_transient_for(GTK_WINDOW(ef_gtk->window), GTK_WINDOW(projet->list_gtk._1990_actions.window));
    
    return TRUE;
}


#endif
