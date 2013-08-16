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
#include <string.h>

#include "common_projet.h"
#include "common_erreurs.h"
#include "common_gtk.h"
#include "common_math.h"
#include "common_text.h"
#include "common_ville.h"

gboolean common_gtk_informations_window_key_press(GtkWidget *widget, GdkEvent *event,
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
    BUGMSG(projet->list_gtk.common_informations.builder, FALSE, gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Informations");
    
    if (event->key.keyval == GDK_KEY_Escape)
    {
        gtk_widget_destroy(projet->list_gtk.common_informations.window);
        return TRUE;
    }
    else
        return FALSE;
}


void common_gtk_informations_window_destroy(GtkWidget *object, Projet *projet)
/* Description : Met projet->list_gtk.common_informations.builder à NULL quand la
 *               fenêtre se ferme.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.common_informations.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Informations");
    
    g_object_unref(G_OBJECT(projet->list_gtk.common_informations.builder));
    projet->list_gtk.common_informations.builder = NULL;
    free(projet->list_gtk.common_informations.departement);
    
    return;
}


gboolean common_gtk_informations_recupere_donnees(Projet *projet, char **destinataire,
  char **adresse, unsigned int *code_postal, char **ville)
/* Description : Récupère toutes les données de la fenêtre permettant d'éditer l'adresse du
 *               projet.
 * Paramètres : Projet *projet : la variable projet,
 *            : char **destinataire : le nom du destinataire,
 *            : char **adresse : la rue du projet,
 *            : unsigned int *code_postal : le code postal du projet,
 *            : char **ville : la ville du projet.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL, cote == NULL, nom == NULL,
 *             en cas d'erreur d'allocation mémoire.
 */
{
    gboolean        ok = TRUE;
    GtkTextIter     start, end;
    GtkTextBuffer   *textbuffer;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(destinataire, FALSE, gettext("Paramètre %s incorrect.\n"), "destinataire");
    BUGMSG(adresse, FALSE, gettext("Paramètre %s incorrect.\n"), "adresse");
    BUGMSG(code_postal, FALSE, gettext("Paramètre %s incorrect.\n"), "code_postal");
    BUGMSG(ville, FALSE, gettext("Paramètre %s incorrect.\n"), "ville");
    BUGMSG(projet->list_gtk.common_informations.builder, FALSE, gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Informations");
    
    *code_postal = common_gtk_entry_uint(GTK_ENTRY(gtk_builder_get_object(projet->list_gtk.common_informations.builder, "common_informations_entry_code_postal")), 0, TRUE, UINT_MAX, FALSE);
    if (*code_postal == UINT_MAX)
        ok = FALSE;
    
    textbuffer = GTK_TEXT_BUFFER(gtk_builder_get_object(projet->list_gtk.common_informations.builder, "common_informations_buffer_destinataire"));
    gtk_text_buffer_get_iter_at_offset(textbuffer, &start, 0);
    gtk_text_buffer_get_iter_at_offset(textbuffer, &end, -1);
    *destinataire = gtk_text_buffer_get_text(textbuffer, &start, &end, FALSE);
    
    textbuffer = GTK_TEXT_BUFFER(gtk_builder_get_object(projet->list_gtk.common_informations.builder, "common_informations_buffer_adresse"));
    gtk_text_buffer_get_iter_at_offset(textbuffer, &start, 0);
    gtk_text_buffer_get_iter_at_offset(textbuffer, &end, -1);
    *adresse = gtk_text_buffer_get_text(textbuffer, &start, &end, FALSE);
    
    BUGMSG(*ville = g_strdup(gtk_entry_buffer_get_text(GTK_ENTRY_BUFFER(gtk_builder_get_object(projet->list_gtk.common_informations.builder, "common_informations_buffer_ville")))), FALSE, gettext("Erreur d'allocation mémoire.\n"));;
    
    return ok;
}


void common_gtk_informations_check(GtkEntryBuffer *entrybuffer, Projet *projet)
/* Description : Vérifie si l'ensemble des éléments est correct pour activer le bouton add/edit.
 * Paramètres : GtkEntryBuffer *entrybuffer : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    unsigned int    code_postal;
    char            *destinataire = NULL, *adresse = NULL, *ville = NULL;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.common_informations.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Informations");
    
    if (!common_gtk_informations_recupere_donnees(projet, &destinataire, &adresse, &code_postal, &ville))
        gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.common_informations.builder, "common_informations_button_edit")), FALSE);
    else
    {
        gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.common_informations.builder, "common_informations_button_edit")), TRUE);
    }
    free(destinataire);
    free(adresse);
    free(ville);
    
    return;
}


typedef struct __Ligne_Adresse
{
    char    *affichage;
    int     population;
    char    departement[4];
    int     commune;
    int     code_postal;
    char    *ville;
} Ligne_Adresse;


gint common_gtk_informations_compare_adresse(Ligne_Adresse *a, Ligne_Adresse *b)
{
    if (a->population > b->population)
        return 1;
    else if (a->population < b->population)
        return -1;
    else
        return 0;
}


void common_gtk_informations_entry_del_char(GtkEntryBuffer *buffer, guint position,
  guint n_chars, Projet *projet)
{
    #define         MAX_VILLES  10
    FILE            *villes;
    char            *ligne = NULL;
    GtkTreeIter     iter;
    int             i;
    const char      *code_postal, *ville;
    GList           *list = NULL, *list_parcours;
    int             f_codepostal; // Cherche sur la base du code postal.
    int             popmin; // La population minimale que doit avoir la ville pour être
                            // autorisée à entrer dans la liste des propositions.
    
    gtk_list_store_clear(projet->list_gtk.common_informations.model_completion);
    
    if (gtk_entry_get_buffer(GTK_ENTRY(gtk_builder_get_object(projet->list_gtk.common_informations.builder, "common_informations_entry_code_postal"))) == buffer)
        f_codepostal = TRUE;
    else if (gtk_entry_get_buffer(GTK_ENTRY(gtk_builder_get_object(projet->list_gtk.common_informations.builder, "common_informations_entry_ville"))) == buffer)
        f_codepostal = FALSE;
    else
        BUGMSG(NULL, , gettext("Paramètre %s incorrect.\n"), "buffer");
    
    code_postal = gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(projet->list_gtk.common_informations.builder, "common_informations_entry_code_postal")));
    ville = gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(projet->list_gtk.common_informations.builder, "common_informations_entry_ville")));
    
    if (((f_codepostal) && (strcmp(code_postal, "") == 0))
      || ((!f_codepostal) && (strcmp(ville, "") == 0)))
    {
        common_gtk_informations_check(NULL, projet);
        return;
    }
    
    BUGMSG(villes = fopen(DATADIR"/france_villes.csv", "r"), , gettext("Le fichier '%s' est introuvable.\n"), DATADIR"/france_villes.csv");
    
    ligne = common_text_get_line(villes);
    free(ligne);
    
    i = 0;
    popmin = 0;
    ligne = common_text_get_line(villes);
    while (ligne != NULL)
    {
        char    *artmaj, *ncc, *artmin, *nccenr;
        int     article, codepostal, population;
        char    *minuscule;
        char    *code_postal2;
        char    departement[4];
        int     commune;
        
        BUG(common_ville_get_ville(ligne, NULL, NULL, NULL, departement, &commune, NULL, NULL, &article, &artmaj, &ncc, &artmin, &nccenr, &codepostal, NULL, &population), );
        BUGMSG(code_postal2 = g_strdup_printf("%d", codepostal), , gettext("Erreur d'allocation mémoire.\n"));
        BUGMSG(minuscule = g_strdup_printf("%s%s%s", artmin, ((article == 5) || (article == 1) || (article == 0)) ? "" : " ", nccenr), , gettext("Erreur d'allocation mémoire.\n"));
        
        if (
          (population > popmin)
          && (
            ((f_codepostal) && (strncmp(code_postal2, code_postal, strlen(code_postal)) == 0))
            || ((!f_codepostal) && (strcasestr_internal(minuscule, ville) != NULL))
          )
        )
        {
            Ligne_Adresse   *adresse;
            
            BUGMSG(adresse = malloc(sizeof(Ligne_Adresse)), , gettext("Erreur d'allocation mémoire.\n"));
            BUGMSG(adresse->affichage = g_strdup_printf("%d %s%s%s", codepostal, artmin, ((article == 5) || (article == 1) || (article == 0)) ? "" : " ", nccenr), , gettext("Erreur d'allocation mémoire.\n"));
            adresse->population = population;
            strncpy(adresse->departement, departement, 4);
            adresse->commune = commune;
            adresse->code_postal = codepostal;
            adresse->ville = minuscule;
            list = g_list_insert_sorted(list, adresse, (GCompareFunc)common_gtk_informations_compare_adresse);
            i++;
            
            // On limite le nombre de villes dans la liste à MAX_VILLES
            if (i > MAX_VILLES)
            {
                adresse = list->data;
                free(adresse->affichage);
                free(adresse->ville);
                free(adresse);
                
                list = g_list_delete_link(list, list);
            }
            
            adresse = list->data;
            popmin = adresse->population;
        }
        else
            free(minuscule);
        free(ligne);
        free(code_postal2);
        ligne = common_text_get_line(villes);
    }
    
    // On ajoute dans le treeview la liste des villes
    list_parcours = g_list_last(list);
    while (list_parcours != NULL)
    {
        Ligne_Adresse   *adresse = list_parcours->data;
        
        char    *tmp;
        
        BUGMSG(tmp = g_strdup_printf("%d", adresse->code_postal), , gettext("Erreur d'allocation mémoire.\n"));
        gtk_list_store_append(projet->list_gtk.common_informations.model_completion, &iter);
        gtk_list_store_set(projet->list_gtk.common_informations.model_completion, &iter, 0, adresse->affichage, 1, tmp, 2, adresse->ville, 3, adresse->departement, 4, adresse->commune, -1);
        
        free(tmp);
        free(adresse->affichage);
        free(adresse->ville);
        free(adresse);
        
        list_parcours = g_list_previous(list_parcours);
    }
    g_list_free(list);
    
    common_gtk_informations_check(NULL, projet);
    
    fclose(villes);
    
    return;
    
    #undef MAX_VILLES
}


void common_gtk_informations_entry_add_char(GtkEntryBuffer *buffer, guint position,
  gchar *chars, guint n_chars, Projet *projet)
{
    common_gtk_informations_entry_del_char(buffer, position, n_chars, projet);
    return;
}


void common_gtk_informations_annuler_clicked(GtkButton *button, Projet *projet)
/* Description : Ferme la fenêtre sans effectuer les modifications.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.common_informations.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Informations");
    
    free(projet->list_gtk.common_informations.departement);
    gtk_widget_destroy(projet->list_gtk.common_informations.window);
    
    return;
}


gboolean common_gtk_informations_match_selected(GtkEntryCompletion *widget, GtkTreeModel *model,
  GtkTreeIter *iter, Projet *projet)
/* Description : Modifie le code postal et la ville en fonction de la sélection de la ville dans
 *               la liste des propositions.
 * Paramètres : GtkEntryCompletion *widget : composant à l'origine de l'évènement,
 *            : GtkTreeModel *model : le composant model contenant la liste des propositions,
 *            : GtkTreeIter *iter : la ligne sélectinnée du composant model,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    int     commune;
    char    *departement;
    char    *code_postal, *ville;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    
    gtk_tree_model_get(model, iter, 1, &code_postal, 2, &ville, 3, &departement, 4, &commune, -1);
    
    free(projet->list_gtk.common_informations.departement);
    projet->list_gtk.common_informations.departement = departement;
    projet->list_gtk.common_informations.commune = commune;
    
    BUG(common_ville_set(projet, projet->list_gtk.common_informations.departement, ville, TRUE), FALSE);
    
    common_gtk_informations_check(NULL, projet);
    
    free(code_postal);
    free(ville);
    
    return TRUE;
}


void common_gtk_informations_modifier_clicked(GtkButton *button, Projet *projet)
/* Description : Ferme la fenêtre en appliquant les modifications.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    unsigned int    code_postal;
    char            *destinataire = NULL, *adresse = NULL, *ville = NULL;
    Type_Neige      neige;
    Type_Vent       vent;
    Type_Seisme     seisme;
    char            *txt1, *txt2, *message;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.common_informations.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Informations");
    
    if (!common_gtk_informations_recupere_donnees(projet, &destinataire, &adresse, &code_postal, &ville))
    {
        free(destinataire);
        free(adresse);
        free(ville);
        return;
    }
    
    // On récupère avant common_ville_set car ce dernier réinitialise les valeurs.
    neige = gtk_combo_box_get_active(GTK_COMBO_BOX(gtk_builder_get_object(projet->list_gtk.common_informations.builder, "common_informations_neige_combobox")));
    vent = gtk_combo_box_get_active(GTK_COMBO_BOX(gtk_builder_get_object(projet->list_gtk.common_informations.builder, "common_informations_vent_combobox")));
    seisme = gtk_combo_box_get_active(GTK_COMBO_BOX(gtk_builder_get_object(projet->list_gtk.common_informations.builder, "common_informations_seisme_combobox")));
    
    BUG(common_ville_set(projet, projet->list_gtk.common_informations.departement, gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(projet->list_gtk.common_informations.builder, "common_informations_entry_ville"))), FALSE), );
    free(projet->parametres.adresse.destinataire);
    projet->parametres.adresse.destinataire = destinataire;
    free(projet->parametres.adresse.adresse);
    projet->parametres.adresse.adresse = adresse;
    projet->parametres.adresse.code_postal = code_postal;
    free(projet->parametres.adresse.ville);
    projet->parametres.adresse.ville = ville;
    
    gtk_combo_box_set_active(GTK_COMBO_BOX(gtk_builder_get_object(projet->list_gtk.common_informations.builder, "common_informations_neige_combobox")), neige);
    gtk_combo_box_set_active(GTK_COMBO_BOX(gtk_builder_get_object(projet->list_gtk.common_informations.builder, "common_informations_vent_combobox")), vent);
    gtk_combo_box_set_active(GTK_COMBO_BOX(gtk_builder_get_object(projet->list_gtk.common_informations.builder, "common_informations_seisme_combobox")), seisme);
    
    message = NULL;
    if (neige != projet->parametres.neige)
    {
        BUGMSG(message = g_strdup_printf("%s %s", gettext("Êtes-vous sûr de vouloir choisir pour"), gettext("la neige")), , gettext("Erreur d'allocation mémoire.\n"));
    }
    if (vent != projet->parametres.vent)
    {
        if (message == NULL)
            BUGMSG(message = g_strdup_printf("%s %s", gettext("Êtes-vous sûr de vouloir choisir pour"), gettext("le vent")), , gettext("Erreur d'allocation mémoire.\n"));
        else
        {
            txt1 = message;
            BUGMSG(txt2 = g_strdup_printf(", %s", gettext("le vent")), , gettext("Erreur d'allocation mémoire.\n"));
            BUGMSG(message = g_strconcat(txt1, txt2, NULL), , gettext("Erreur d'allocation mémoire.\n"));
            free(txt1);
            free(txt2);
        }
    }
    if (seisme != projet->parametres.seisme)
    {
        if (message == NULL)
            BUGMSG(message = g_strdup_printf("%s %s", gettext("Êtes-vous sûr de vouloir choisir pour"), gettext("le séisme")), , gettext("Erreur d'allocation mémoire.\n"));
        else
        {
            txt1 = message;
            BUGMSG(txt2 = g_strdup_printf(", %s", gettext("le séisme")), , gettext("Erreur d'allocation mémoire.\n"));
            BUGMSG(message = g_strconcat(txt1, txt2, NULL), , gettext("Erreur d'allocation mémoire.\n"));
            free(txt1);
            free(txt2);
        }
    }
    if (message != NULL)
    {
        GtkWidget       *dialog;
        
        txt1 = message;
        BUGMSG(message = g_strconcat(txt1, gettext(" des paramètres de calculs différents que ceux imposés par les normes ?"), NULL), , gettext("Erreur d'allocation mémoire.\n"));
        free(txt1);
        dialog = gtk_message_dialog_new(GTK_WINDOW(projet->list_gtk.common_informations.window), GTK_DIALOG_MODAL, GTK_MESSAGE_WARNING, GTK_BUTTONS_YES_NO, "%s", message);
        if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_YES)
        {
            projet->parametres.neige = neige;
            projet->parametres.vent = vent;
            projet->parametres.seisme = seisme;
            gtk_widget_destroy(projet->list_gtk.common_informations.window);
        }
        gtk_widget_destroy (dialog);
    }
    else
        gtk_widget_destroy(projet->list_gtk.common_informations.window);
    
    return;
}


gboolean common_gtk_informations_always_match(GtkEntryCompletion *completion, const gchar *key,
  GtkTreeIter *iter, gpointer user_data)
/* Description : Indique que tous les éléments de la liste des propositions correspond à la
 *               requête. En effet, la liste des propositions est générée en temps réel via la
 *               fonction common_gtk_informations_entry_del_char.
 * Paramètres : GtkEntryCompletion *completion : le composant à l'origine de l'évènement,
 *              const gchar *key : le texte du composant completion,
 *              GtkTreeIter *iter : la liste du model du composant completion à valider,
 *              gpointer user_data : la variable projet.
 * Valeur renvoyée : TRUE
 */
{
    return TRUE;
}


gboolean common_gtk_informations(Projet *projet)
/* Description : Affichage de la fenêtre permettant de personnaliser l'adresse du projet.
 * Paramètres : Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Echec : FALSE :
 *             projet == NULL,
 *             Fenêtre graphique déjà initialisée.
 */
{
    Gtk_Common_Informations *ef_gtk;
    GtkEntryCompletion *completion;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    
    ef_gtk = &projet->list_gtk.common_informations;
    if (projet->list_gtk.common_informations.builder != NULL)
    {
        gtk_window_present(GTK_WINDOW(projet->list_gtk.common_informations.window));
        return TRUE;
    }
    
    ef_gtk->builder = gtk_builder_new();
    BUGMSG(gtk_builder_add_from_resource(ef_gtk->builder, "/org/2lgc/codegui/ui/common_informations.ui", NULL) != 0, FALSE, gettext("Builder Failed\n"));
    gtk_builder_connect_signals(ef_gtk->builder, projet);
    ef_gtk->window = GTK_WIDGET(gtk_builder_get_object(ef_gtk->builder, "common_informations_window"));
    ef_gtk->model_completion = GTK_LIST_STORE(gtk_builder_get_object(ef_gtk->builder, "common_informations_completion_model"));
    
    BUGMSG(projet->list_gtk.common_informations.departement = g_strdup(projet->parametres.adresse.departement), FALSE, gettext("Erreur d'allocation mémoire.\n"));
    projet->list_gtk.common_informations.commune = projet->parametres.adresse.commune;
    common_gtk_informations_check(NULL, projet);
    
    // On ne peut pas mettre text-column directement dans le fichier ui car ça ne marche pas.
    completion = GTK_ENTRY_COMPLETION(gtk_builder_get_object(projet->list_gtk.common_informations.builder, "common_informations_completion_code_postal"));
    gtk_entry_completion_set_text_column(completion, 0);
    gtk_entry_completion_set_match_func(completion, common_gtk_informations_always_match, NULL, NULL);
    completion = GTK_ENTRY_COMPLETION(gtk_builder_get_object(projet->list_gtk.common_informations.builder, "common_informations_completion_ville"));
    gtk_entry_completion_set_text_column(completion, 0);
    gtk_entry_completion_set_match_func(completion, common_gtk_informations_always_match, NULL, NULL);
    
    if (projet->parametres.adresse.destinataire != NULL)
        gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder, "common_informations_buffer_destinataire")), projet->parametres.adresse.destinataire, -1);
    if (projet->parametres.adresse.adresse != NULL)
        gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gtk_builder_get_object(ef_gtk->builder, "common_informations_buffer_adresse")), projet->parametres.adresse.adresse, -1);
    if (projet->parametres.adresse.code_postal != 0)
    {
        char    *texte;
        
        BUGMSG(texte = g_strdup_printf("%d", projet->parametres.adresse.code_postal), FALSE, gettext("Erreur d'allocation mémoire.\n"));
        gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(ef_gtk->builder, "common_informations_entry_code_postal")), texte);
        free(texte);
    }
    if (projet->parametres.adresse.ville != NULL)
        gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(ef_gtk->builder, "common_informations_entry_ville")), projet->parametres.adresse.ville);
    
    g_object_set(gtk_builder_get_object(ef_gtk->builder, "common_informations_neige_combobox"), "model", projet->parametres.neige_desc, NULL);
    g_object_set(gtk_builder_get_object(ef_gtk->builder, "common_informations_vent_combobox"), "model", projet->parametres.vent_desc, NULL);
    g_object_set(gtk_builder_get_object(ef_gtk->builder, "common_informations_seisme_combobox"), "model", projet->parametres.seisme_desc, NULL);
    
    gtk_combo_box_set_active(GTK_COMBO_BOX(gtk_builder_get_object(ef_gtk->builder, "common_informations_neige_combobox")), projet->parametres.neige);
    gtk_combo_box_set_active(GTK_COMBO_BOX(gtk_builder_get_object(ef_gtk->builder, "common_informations_vent_combobox")), projet->parametres.vent);
    gtk_combo_box_set_active(GTK_COMBO_BOX(gtk_builder_get_object(ef_gtk->builder, "common_informations_seisme_combobox")), projet->parametres.seisme);
    
    gtk_window_set_transient_for(GTK_WINDOW(ef_gtk->window), GTK_WINDOW(projet->list_gtk.comp.window));
    
    return TRUE;
}
#endif
