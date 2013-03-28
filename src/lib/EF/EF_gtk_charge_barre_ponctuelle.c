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
#include "common_selection.h"
#include "common_gtk.h"
#include "common_math.h"
#include "EF_charge.h"
#include "EF_charge_barre_ponctuelle.h"

gboolean EF_gtk_charge_barre_ponctuelle_ajout_affichage(Charge_Barre_Ponctuelle *charge,
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
    char    *description, txt_pos[30], txt_fx[30], txt_fy[30], txt_fz[30], txt_mx[30], txt_my[30], txt_mz[30], *txt_liste_barres;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    
    if (projet->list_gtk._1990_actions.builder == NULL)
        return TRUE;
    
    BUG(txt_liste_barres = common_selection_converti_barres_en_texte(charge->barres), FALSE);
    common_math_double_to_char(charge->position, txt_pos, DECIMAL_DISTANCE);
    common_math_double_to_char(charge->fx, txt_fx, DECIMAL_FORCE);
    common_math_double_to_char(charge->fy, txt_fy, DECIMAL_FORCE);
    common_math_double_to_char(charge->fz, txt_fz, DECIMAL_FORCE);
    common_math_double_to_char(charge->mx, txt_mx, DECIMAL_MOMENT);
    common_math_double_to_char(charge->my, txt_my, DECIMAL_MOMENT);
    common_math_double_to_char(charge->mz, txt_mz, DECIMAL_MOMENT);
    
    BUGMSG(description = g_strdup_printf("%s : %s, %s : %s m, %s, Fx : %s N, Fy : %s N, Fz : %s N, Mx : %s N.m, My : %s N.m, Mz : %s N.m", strstr(txt_liste_barres, ";") == NULL ? gettext("Barre") : gettext("Barres"), txt_liste_barres, gettext("position"), txt_pos, charge->repere_local ? gettext("repère : local") : gettext("repère : global"), txt_fx, txt_fy, txt_fz, txt_mx, txt_my, txt_mz), FALSE, gettext("Erreur d'allocation mémoire.\n"));
    
    free(txt_liste_barres);
    
    if (nouvelle_ligne == TRUE)
        gtk_tree_store_append(projet->list_gtk._1990_actions.tree_store_charges, &charge->Iter, NULL);
    gtk_tree_store_set(projet->list_gtk._1990_actions.tree_store_charges, &charge->Iter, 0, charge->numero, 1, charge->nom, 2, gettext("Ponctuelle sur barre"), 3, description, -1);
    free(description);
    
    return TRUE;
}


/* DEBUT DE LA FENETRE GRAPHIQUE*/

void EF_gtk_charge_barre_ponctuelle_annuler_clicked(GtkButton *button, Projet *projet)
/* Description : Ferme la fenêtre sans effectuer les modifications.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_charge_barre_ponctuelle.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Charge Barre Ponctuelle");
    
    gtk_widget_destroy(projet->list_gtk.ef_charge_barre_ponctuelle.window);
    
    return;
}


gboolean EF_gtk_charge_barre_ponctuelle_recupere_donnees(Projet *projet,
  unsigned int *num_action, GList **barres, double *fx, double *fy, double *fz, double *mx,
  double *my, double *mz, gchar **nom, gboolean *repere_local, double *position)
/* Description : Récupère toutes les données de la fenêtre permettant d'ajouter ou d'éditer une
 *               charge ponctuelle.
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
 *            : double *position : position de la charge sur la barre.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL, num_action == NULL, barres == NULL, fx == NULL, fy == NULL,
 *             fz == NULL, mx == NULL, my == NULL, mz == NULL, nom == NULL,
 *             repere_local == NULL, position == NULL,
 *             en cas d'erreur d'allocation mémoire.
 */
{
    Gtk_EF_Charge_Barre_Ponctuelle  *ef_gtk;
    GList                           *num_barres;
    GtkTextIter                     start, end;
    gchar                           *texte_tmp;
    GtkTextBuffer                   *textbuffer;
    gboolean                        ok = TRUE;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(num_action, FALSE, gettext("Paramètre %s incorrect.\n"), "num_action");
    BUGMSG(barres, FALSE, gettext("Paramètre %s incorrect.\n"), "barres");
    BUGMSG(fx, FALSE, gettext("Paramètre %s incorrect.\n"), "fx");
    BUGMSG(fy, FALSE, gettext("Paramètre %s incorrect.\n"), "fy");
    BUGMSG(fz, FALSE, gettext("Paramètre %s incorrect.\n"), "fz");
    BUGMSG(mx, FALSE, gettext("Paramètre %s incorrect.\n"), "mx");
    BUGMSG(my, FALSE, gettext("Paramètre %s incorrect.\n"), "my");
    BUGMSG(mz, FALSE, gettext("Paramètre %s incorrect.\n"), "mz");
    BUGMSG(nom, FALSE, gettext("Paramètre %s incorrect.\n"), "nom");
    BUGMSG(repere_local, FALSE, gettext("Paramètre %s incorrect.\n"), "repere_local");
    BUGMSG(position, FALSE, gettext("Paramètre %s incorrect.\n"), "position");
    BUGMSG(projet->list_gtk.ef_charge_barre_ponctuelle.builder, FALSE, gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Charge Barre Ponctuelle");
    
    ef_gtk = &projet->list_gtk.ef_charge_barre_ponctuelle;
    
    if (gtk_combo_box_get_active(ef_gtk->combobox_charge) < 0)
        ok = FALSE;
    else
        *num_action = (unsigned int)gtk_combo_box_get_active(ef_gtk->combobox_charge);
    
    *fx = common_gtk_text_buffer_double(GTK_TEXT_BUFFER(gtk_builder_get_object(projet->list_gtk.ef_charge_barre_ponctuelle.builder, "EF_charge_barre_ponct_buffer_fx")), -INFINITY, FALSE, INFINITY, FALSE);
    if (isnan(*fx))
        ok = FALSE;
    
    *fy = common_gtk_text_buffer_double(GTK_TEXT_BUFFER(gtk_builder_get_object(projet->list_gtk.ef_charge_barre_ponctuelle.builder, "EF_charge_barre_ponct_buffer_fy")), -INFINITY, FALSE, INFINITY, FALSE);
    if (isnan(*fy))
        ok = FALSE;
    
    *fz = common_gtk_text_buffer_double(GTK_TEXT_BUFFER(gtk_builder_get_object(projet->list_gtk.ef_charge_barre_ponctuelle.builder, "EF_charge_barre_ponct_buffer_fz")), -INFINITY, FALSE, INFINITY, FALSE);
    if (isnan(*fz))
        ok = FALSE;
    
    *mx = common_gtk_text_buffer_double(GTK_TEXT_BUFFER(gtk_builder_get_object(projet->list_gtk.ef_charge_barre_ponctuelle.builder, "EF_charge_barre_ponct_buffer_mx")), -INFINITY, FALSE, INFINITY, FALSE);
    if (isnan(*mx))
        ok = FALSE;
    
    *my = common_gtk_text_buffer_double(GTK_TEXT_BUFFER(gtk_builder_get_object(projet->list_gtk.ef_charge_barre_ponctuelle.builder, "EF_charge_barre_ponct_buffer_my")), -INFINITY, FALSE, INFINITY, FALSE);
    if (isnan(*my))
        ok = FALSE;
    
    *mz = common_gtk_text_buffer_double(GTK_TEXT_BUFFER(gtk_builder_get_object(projet->list_gtk.ef_charge_barre_ponctuelle.builder, "EF_charge_barre_ponct_buffer_mz")), -INFINITY, FALSE, INFINITY, FALSE);
    if (isnan(*mz))
        ok = FALSE;
    
    *position = common_gtk_text_buffer_double(GTK_TEXT_BUFFER(gtk_builder_get_object(projet->list_gtk.ef_charge_barre_ponctuelle.builder, "EF_charge_barre_ponct_buffer_pos")), 0, TRUE, INFINITY, FALSE);
    if (isnan(*position))
        ok = FALSE;
    
    *repere_local = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_ponct_radio_local")));
    
    textbuffer = GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_ponct_buffer_barres"));
    gtk_text_buffer_get_iter_at_offset(textbuffer, &start, 0);
    gtk_text_buffer_get_iter_at_offset(textbuffer, &end, -1);
    texte_tmp = gtk_text_buffer_get_text(textbuffer, &start, &end, FALSE);
    num_barres = common_selection_renvoie_numeros(texte_tmp);
    if (num_barres == NULL)
        ok = FALSE;
    else
    {
        *barres = common_selection_converti_numeros_en_barres(num_barres, projet);
        g_list_free(num_barres);
        if (*barres == NULL)
            ok = FALSE;
        else
        {
            // Si tous les paramètres sont corrects
            textbuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_ponct_textview_description")));
            
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


void EF_gtk_charge_barre_ponct_check(GtkWidget *object, Projet *projet)
/* Description : Vérifie si l'ensemble des éléments est correct pour activer le bouton add/edit.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    unsigned int num_action;
    GList *barres;
    double fx, fy, fz, mx, my, mz;
    gchar *nom;
    gboolean repere_local;
    double position;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_charge_barre_ponctuelle.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Charge Barre Ponctuelle");
    
    if (!EF_gtk_charge_barre_ponctuelle_recupere_donnees(projet, &num_action, &barres, &fx, &fy, &fz, &mx, &my, &mz, &nom, &repere_local, &position))
        gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_charge_barre_ponctuelle.builder, "EF_charge_barre_ponct_button_add_edit")), FALSE);
    else
    {
        gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_charge_barre_ponctuelle.builder, "EF_charge_barre_ponct_button_add_edit")), TRUE);
        free(nom);
        g_list_free(barres);
    }
    return;
}


void EF_gtk_charge_barre_ponctuelle_ajouter_clicked(GtkButton *button, Projet *projet)
/* Description : Ferme la fenêtre en ajoutant la charge.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    double                      fx, fy, fz, mx, my, mz, position;
    unsigned int                num_action;
    gboolean                    repere_local;
    GList                       *barres;
    gchar                       *texte;
    Charge_Barre_Ponctuelle     *charge;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_charge_barre_ponctuelle.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Charge Barre Ponctuelle");
    
    BUG(EF_gtk_charge_barre_ponctuelle_recupere_donnees(projet, &num_action, &barres, &fx, &fy, &fz, &mx, &my, &mz, &texte, &repere_local, &position) == TRUE, );
    
    // Création de la nouvelle charge ponctuelle sur barre
    BUG(charge = EF_charge_barre_ponctuelle_ajout(projet, num_action, barres, repere_local, position, fx, fy, fz, mx, my, mz, texte), );
    
    free(texte);
    
    gtk_widget_destroy(projet->list_gtk.ef_charge_barre_ponctuelle.window);
    
    return;
}


void EF_gtk_charge_barre_ponctuelle_editer_clicked(GtkButton *button, Projet *projet)
/* Description : Ferme la fenêtre en appliquant les modifications.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    Gtk_EF_Charge_Barre_Ponctuelle  *ef_gtk;
    double                          fx, fy, fz, mx, my, mz, position;
    unsigned int                    num_action;
    gboolean                        repere_local;
    GList                           *barres;
    gchar                           *texte;
    Charge_Barre_Ponctuelle         *charge;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_charge_barre_ponctuelle.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Charge Barre Ponctuelle");
    
    ef_gtk = &projet->list_gtk.ef_charge_barre_ponctuelle;
    
    BUG(EF_gtk_charge_barre_ponctuelle_recupere_donnees(projet, &num_action, &barres, &fx, &fy, &fz, &mx, &my, &mz, &texte, &repere_local, &position) == TRUE, );
    
    // Création de la nouvelle charge ponctuelle sur barre
    BUG(charge = EF_charge_cherche(projet, ef_gtk->action, ef_gtk->charge), );
    free(charge->nom);
    charge->nom = texte;
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
        BUG(EF_charge_deplace(projet, ef_gtk->action, ef_gtk->charge, num_action), );
    else
        BUG(EF_gtk_charge_barre_ponctuelle_ajout_affichage(charge, projet, FALSE), );
    
    gtk_widget_destroy(projet->list_gtk.ef_charge_barre_ponctuelle.window);
    
    return;
}


gboolean EF_gtk_charge_barre_ponct_window_key_press(GtkWidget *widget, GdkEvent *event,
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
    BUGMSG(projet->list_gtk.ef_charge_barre_ponctuelle.builder, FALSE, gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Charge Barre Ponctuelle");
    
    if (event->key.keyval == GDK_KEY_Escape)
    {
        gtk_widget_destroy(projet->list_gtk.ef_charge_barre_ponctuelle.window);
        return TRUE;
    }
    else
        return FALSE;
}


void EF_gtk_charge_barre_ponct_window_destroy(GtkWidget *object, Projet *projet)
/* Description : Met projet->list_gtk.ef_charge_barre_ponctuelle.builder à NULL quand la
 *               fenêtre se ferme.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_charge_barre_ponctuelle.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Charge Barre Ponctuelle");
    
    g_object_unref(G_OBJECT(projet->list_gtk.ef_charge_barre_ponctuelle.builder));
    projet->list_gtk.ef_charge_barre_ponctuelle.builder = NULL;
    
    return;
}


gboolean EF_gtk_charge_barre_ponctuelle(Projet *projet, unsigned int action_defaut,
  unsigned int charge)
/* Description : Affichage de la fenêtre permettant de créer ou modifier une action de type
 *               charge ponctuelle sur barre.
 * Paramètres : Projet *projet : la variable projet,
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
    Gtk_EF_Charge_Barre_Ponctuelle  *ef_gtk;
    Charge_Barre_Ponctuelle         *charge_barre;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    if (projet->list_gtk.ef_charge_barre_ponctuelle.builder != NULL)
    {
        gtk_window_present(GTK_WINDOW(projet->list_gtk.ef_charge_barre_ponctuelle.window));
        return TRUE;
    }
    
    ef_gtk = &projet->list_gtk.ef_charge_barre_ponctuelle;
    ef_gtk->builder = gtk_builder_new();
    BUGMSG(gtk_builder_add_from_file(ef_gtk->builder, DATADIR"/ui/EF_charge_barre_ponctuelle.ui", NULL) != 0, FALSE, gettext("Builder Failed\n"));
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
        BUG(charge_barre = EF_charge_cherche(projet, action_defaut, charge), FALSE);
    }
    
    gtk_combo_box_set_model(GTK_COMBO_BOX(ef_gtk->combobox_charge), GTK_TREE_MODEL(projet->list_gtk._1990_actions.list_actions));
    gtk_combo_box_set_active(GTK_COMBO_BOX(ef_gtk->combobox_charge), (gint)action_defaut);
    
    if (charge_barre != NULL)
    {
        gchar   tmp[30], *tmp2;
        gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_ponct_textview_description"))), charge_barre->nom, -1);
        common_math_double_to_char(charge_barre->fx, tmp, DECIMAL_FORCE);
        gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_ponct_buffer_fx")), tmp, -1);
        common_math_double_to_char(charge_barre->fy, tmp, DECIMAL_FORCE);
        gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_ponct_buffer_fy")), tmp, -1);
        common_math_double_to_char(charge_barre->fz, tmp, DECIMAL_FORCE);
        gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_ponct_buffer_fz")), tmp, -1);
        common_math_double_to_char(charge_barre->mx, tmp, DECIMAL_MOMENT);
        gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_ponct_buffer_mx")), tmp, -1);
        common_math_double_to_char(charge_barre->my, tmp, DECIMAL_MOMENT);
        gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_ponct_buffer_my")), tmp, -1);
        common_math_double_to_char(charge_barre->mz, tmp, DECIMAL_MOMENT);
        gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_ponct_buffer_mz")), tmp, -1);
        common_math_double_to_char(charge_barre->position, tmp, DECIMAL_DISTANCE);
        gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_ponct_buffer_pos")), tmp, -1);
        if (charge_barre->repere_local)
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_ponct_radio_local")), TRUE);
        else
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(ef_gtk->builder, "EF_charge_barre_ponct_radio_global")), TRUE);
        BUG(tmp2 = common_selection_converti_barres_en_texte(charge_barre->barres), FALSE);
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
    
    EF_gtk_charge_barre_ponct_check(NULL, projet);
    
    if (projet->list_gtk._1990_actions.window == NULL)
        gtk_window_set_transient_for(GTK_WINDOW(ef_gtk->window), GTK_WINDOW(projet->list_gtk.comp.window));
    else
        gtk_window_set_transient_for(GTK_WINDOW(ef_gtk->window), GTK_WINDOW(projet->list_gtk._1990_actions.window));
    return TRUE;
}


#endif
