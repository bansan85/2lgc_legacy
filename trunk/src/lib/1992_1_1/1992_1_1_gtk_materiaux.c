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
    Gtk_1992_1_1_Materiaux *ef_gtk;
    
    GtkWidget       *dialog;
    GtkTextIter     start, end;
    GtkTextBuffer   *textbuffer;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(fck, FALSE, gettext("Paramètre %s incorrect.\n"), "fck");
    BUGMSG(nom, FALSE, gettext("Paramètre %s incorrect.\n"), "nom");
    BUGMSG(projet->list_gtk._1992_1_1_materiaux.builder, FALSE, gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Ajout Matériau Béton");
    
    ef_gtk = &projet->list_gtk._1992_1_1_materiaux;
    
    *fck = common_gtk_entry_renvoie_double(GTK_TEXT_BUFFER(gtk_builder_get_object(projet->list_gtk._1992_1_1_materiaux.builder, "_1992_1_1_materiaux_beton_buffer_fck")));
    if ((isnan(*fck)) || (*fck < ERREUR_RELATIVE_MIN) || (*fck*(1+ERREUR_RELATIVE_MIN) > 90.))
    {
        dialog = gtk_message_dialog_new(GTK_WINDOW(ef_gtk->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, gettext("La valeur de fck est incorrecte."));
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return FALSE;
    }
    
    // Si tous les paramètres sont corrects
    textbuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(gtk_builder_get_object(projet->list_gtk._1992_1_1_materiaux.builder, "_1992_1_1_materiaux_beton_textview_nom")));
    
    gtk_text_buffer_get_iter_at_offset(textbuffer, &start, 0);
    gtk_text_buffer_get_iter_at_offset(textbuffer, &end, -1);
    *nom = gtk_text_buffer_get_text(textbuffer, &start, &end, FALSE);
    
    return TRUE;
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
    
    free(ef_gtk->materiau->nom);
    ef_gtk->materiau->nom = texte;

    ef_gtk->materiau->fck = fck*1000000.;
    BUG(!isnan(ef_gtk->materiau->fckcube = _1992_1_1_materiaux_fckcube(fck)), );
    BUG(!isnan(ef_gtk->materiau->fcm = _1992_1_1_materiaux_fcm(fck)), );
    BUG(!isnan(ef_gtk->materiau->fctm = _1992_1_1_materiaux_fctm(fck)), );
    BUG(!isnan(ef_gtk->materiau->fctk_0_05 = _1992_1_1_materiaux_fctk_0_05(fck)), );
    BUG(!isnan(ef_gtk->materiau->fctk_0_95 = _1992_1_1_materiaux_fctk_0_95(fck)), );
    BUG(!isnan(ef_gtk->materiau->ecm = _1992_1_1_materiaux_ecm(fck)), );
    BUG(!isnan(ef_gtk->materiau->ec1 = _1992_1_1_materiaux_ec1(fck)), );
    BUG(!isnan(ef_gtk->materiau->ecu1 = _1992_1_1_materiaux_ecu1(fck)), );
    BUG(!isnan(ef_gtk->materiau->ec2 = _1992_1_1_materiaux_ec2(fck)), );
    BUG(!isnan(ef_gtk->materiau->ecu2 = _1992_1_1_materiaux_ecu2(fck)), );
    BUG(!isnan(ef_gtk->materiau->ec3 = _1992_1_1_materiaux_ec3(fck)), );
    BUG(!isnan(ef_gtk->materiau->ecu3 = _1992_1_1_materiaux_ecu3(fck)), );
    BUG(!isnan(ef_gtk->materiau->n = _1992_1_1_materiaux_n(fck)), );
    ef_gtk->materiau->nu = COEFFICIENT_NU_BETON;
    BUG(!isnan(ef_gtk->materiau->gnu_0_2 = _1992_1_1_materiaux_gnu(fck, COEFFICIENT_NU_BETON)), );
    BUG(!isnan(ef_gtk->materiau->gnu_0_0 = _1992_1_1_materiaux_gnu(fck, 0)), );
    
    BUG(_1992_1_1_materiaux_update_ligne_treeview(projet, ef_gtk->materiau), );
    
    gtk_widget_destroy(projet->list_gtk._1992_1_1_materiaux.window);
    
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
    if (projet->list_gtk._1992_1_1_materiaux.builder != NULL)
    {
        gtk_window_present(GTK_WINDOW(projet->list_gtk._1992_1_1_materiaux.window));
        return TRUE;
    }
    
    ef_gtk = &projet->list_gtk._1992_1_1_materiaux;
    ef_gtk->builder = gtk_builder_new();
    BUGMSG(gtk_builder_add_from_file(ef_gtk->builder, DATADIR"/ui/1992_1_1_materiaux_beton.ui", NULL) != 0, FALSE, gettext("Builder Failed\n"));
    gtk_builder_connect_signals(ef_gtk->builder, projet);
    
    ef_gtk->window = GTK_WIDGET(gtk_builder_get_object(ef_gtk->builder, "_1992_1_1_materiaux_beton_window"));
    
    if (materiau == NULL)
    {
        gtk_window_set_title(GTK_WINDOW(ef_gtk->window), gettext("Ajout d'un matériau béton"));
        ef_gtk->materiau = NULL;
        
        gtk_button_set_label(GTK_BUTTON(gtk_builder_get_object(ef_gtk->builder, "_1992_1_1_materiaux_beton_button_add_edit")), "gtk-add");
        g_signal_connect(gtk_builder_get_object(ef_gtk->builder, "_1992_1_1_materiaux_beton_button_add_edit"), "clicked", G_CALLBACK(_1992_1_1_gtk_materiaux_ajouter_clicked), projet);
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
    
    // On récupère la liste des charges à éditer.
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
