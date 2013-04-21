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
#include "common_math.h"
#include "common_selection.h"
#include "EF_charge_noeud.h"
#include "EF_charge.h"


void EF_gtk_charge_noeud_annuler_clicked(GtkButton *button, Projet *projet)
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
    Gtk_EF_Charge_Noeud *ef_gtk;
    GList               *num_noeuds;
    GtkTextIter         start, end;
    gchar               *texte_tmp;
    GtkTextBuffer       *textbuffer;
    gint                get_active;
    gboolean            ok = TRUE;
    
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
        ok = FALSE;
    else
        *num_action = (unsigned int)get_active;
    
    *fx = common_gtk_text_buffer_double(GTK_TEXT_BUFFER(gtk_builder_get_object(projet->list_gtk.ef_charge_noeud.builder, "EF_charge_noeud_buffer_fx")), -INFINITY, FALSE, INFINITY, FALSE);
    if (isnan(*fx))
        ok = FALSE;
    
    *fy = common_gtk_text_buffer_double(GTK_TEXT_BUFFER(gtk_builder_get_object(projet->list_gtk.ef_charge_noeud.builder, "EF_charge_noeud_buffer_fy")), -INFINITY, FALSE, INFINITY, FALSE);
    if (isnan(*fy))
        ok = FALSE;
    
    *fz = common_gtk_text_buffer_double(GTK_TEXT_BUFFER(gtk_builder_get_object(projet->list_gtk.ef_charge_noeud.builder, "EF_charge_noeud_buffer_fz")), -INFINITY, FALSE, INFINITY, FALSE);
    if (isnan(*fz))
        ok = FALSE;
    
    *mx = common_gtk_text_buffer_double(GTK_TEXT_BUFFER(gtk_builder_get_object(projet->list_gtk.ef_charge_noeud.builder, "EF_charge_noeud_buffer_mx")), -INFINITY, FALSE, INFINITY, FALSE);
    if (isnan(*mx))
        ok = FALSE;
    
    *my = common_gtk_text_buffer_double(GTK_TEXT_BUFFER(gtk_builder_get_object(projet->list_gtk.ef_charge_noeud.builder, "EF_charge_noeud_buffer_my")), -INFINITY, FALSE, INFINITY, FALSE);
    if (isnan(*my))
        ok = FALSE;
    
    *mz = common_gtk_text_buffer_double(GTK_TEXT_BUFFER(gtk_builder_get_object(projet->list_gtk.ef_charge_noeud.builder, "EF_charge_noeud_buffer_mz")), -INFINITY, FALSE, INFINITY, FALSE);
    if (isnan(*mz))
        ok = FALSE;
    
    textbuffer = GTK_TEXT_BUFFER(gtk_builder_get_object(projet->list_gtk.ef_charge_noeud.builder, "EF_charge_noeud_buffer_noeuds"));
    gtk_text_buffer_get_iter_at_offset(textbuffer, &start, 0);
    gtk_text_buffer_get_iter_at_offset(textbuffer, &end, -1);
    texte_tmp = gtk_text_buffer_get_text(textbuffer, &start, &end, FALSE);
    num_noeuds = common_selection_renvoie_numeros(texte_tmp);
    if (num_noeuds == NULL)
        ok = FALSE;
    else
    {
        *noeuds = common_selection_converti_numeros_en_noeuds(num_noeuds, projet);
        g_list_free(num_noeuds);
        if (*noeuds == NULL)
            ok = FALSE;
        else
        {
            // Si tous les paramètres sont corrects
            textbuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(gtk_builder_get_object(projet->list_gtk.ef_charge_noeud.builder, "EF_charge_noeud_textview_description")));
            
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


void EF_gtk_charge_noeud_check(GtkWidget *object, Projet *projet)
/* Description : Vérifie si l'ensemble des éléments est correct pour activer le bouton add/edit.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    unsigned int    num_action;
    GList           *noeuds;
    double          fx, fy, fz, mx, my, mz;
    gchar           *nom;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_charge_noeud.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Charge Nodale");
    
    if (!EF_gtk_charge_noeud_recupere_donnees(projet, &num_action, &noeuds, &fx, &fy, &fz, &mx, &my, &mz, &nom))
        gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_charge_noeud.builder, "EF_charge_noeud_button_add_edit")), FALSE);
    else
    {
        gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_charge_noeud.builder, "EF_charge_noeud_button_add_edit")), TRUE);
        free(nom);
        g_list_free(noeuds);
    }
    return;
}


void EF_gtk_charge_noeud_ajouter_clicked(GtkButton *button, Projet *projet)
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
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_charge_noeud.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Charge Nodale");
    
    BUG(EF_gtk_charge_noeud_recupere_donnees(projet, &num_action, &noeuds, &fx, &fy, &fz, &mx, &my, &mz, &texte) == TRUE, );
    
    // Création de la nouvelle charge ponctuelle au noeud
    BUG(charge_noeud = EF_charge_noeud_ajout(projet, num_action, noeuds, fx, fy, fz, mx, my, mz, texte), );
    
    free(texte);
    
    gtk_widget_destroy(projet->list_gtk.ef_charge_noeud.window);
    
    return;
}


void EF_gtk_charge_noeud_editer_clicked(GtkButton *button, Projet *projet)
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
        gtk_widget_queue_draw(GTK_WIDGET(projet->list_gtk._1990_actions.tree_view_charges));
    
    gtk_widget_destroy(projet->list_gtk.ef_charge_noeud.window);
    
    return;
}


gboolean EF_gtk_charge_noeud_window_key_press(GtkWidget *widget, GdkEvent *event,
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
    BUGMSG(projet->list_gtk.ef_charge_noeud.builder, FALSE, gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Charge Nodale");
    
    if (event->key.keyval == GDK_KEY_Escape)
    {
        gtk_widget_destroy(projet->list_gtk.ef_charge_noeud.window);
        return TRUE;
    }
    else
        return FALSE;
}


void EF_gtk_charge_noeud_window_destroy(GtkWidget *object, Projet *projet)
/* Description : met projet->list_gtk.ef_charge_noeud.builder à NULL quand la fenêtre se ferme.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_charge_noeud.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Charge Nodale");
    
    g_object_unref(G_OBJECT(projet->list_gtk.ef_charge_noeud.builder));
    projet->list_gtk.ef_charge_noeud.builder = NULL;
    
    return;
}


gboolean EF_gtk_charge_noeud(Projet *projet, unsigned int action_defaut, unsigned int charge)
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
    if (projet->list_gtk.ef_charge_noeud.builder != NULL)
    {
        gtk_window_present(GTK_WINDOW(projet->list_gtk.ef_charge_noeud.window));
        return TRUE;
    }
    
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
    
    EF_gtk_charge_noeud_check(NULL, projet);
    
    if (projet->list_gtk._1990_actions.window == NULL)
        gtk_window_set_transient_for(GTK_WINDOW(ef_gtk->window), GTK_WINDOW(projet->list_gtk.comp.window));
    else
        gtk_window_set_transient_for(GTK_WINDOW(ef_gtk->window), GTK_WINDOW(projet->list_gtk._1990_actions.window));
    
    return TRUE;
}


#endif
