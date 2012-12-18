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
#include <string.h>

#include "common_m3d.hpp"

#include "common_projet.h"
#include "common_erreurs.h"
#include "common_gtk.h"
#include "common_math.h"
#include "common_selection.h"
#include "EF_relachement.h"

G_MODULE_EXPORT void EF_gtk_relachements_fermer(GtkButton *button __attribute__((unused)),
  Projet *projet)
/* Description : Ferme la fenêtre.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_relachements.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Relachement");
    
    gtk_widget_destroy(projet->list_gtk.ef_relachements.window);
    
    return;
}


G_MODULE_EXPORT void EF_gtk_relachements_window_destroy(GtkWidget *object __attribute__((unused)),
  Projet *projet)
/* Description : met projet->list_gtk.ef_relachements.builder à NULL quand la fenêtre se ferme,
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_relachements.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Relachement");
    
    projet->list_gtk.ef_relachements.builder = NULL;
    
    return;
}


G_MODULE_EXPORT gboolean EF_gtk_relachements_window_key_press(
  GtkWidget *widget __attribute__((unused)), GdkEvent *event, Projet *projet)
/* Description : Ferme la fenêtre si la touche ECHAP est pressée.
 * Paramètres : GtkWidget *widget : composant à l'origine de l'évènement,
 *            : GdkEvent *event : Caractéristique de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : TRUE si la touche DELETE est pressée, FALSE sinon.
 *   Echec : projet == NULL,
 *           interface graphique non initialisée.
 *  
 */
{
    BUGMSG(projet, TRUE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_relachements.builder, TRUE, gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Relachement");
    
    if (event->key.keyval == GDK_KEY_Escape)
    {
        gtk_widget_destroy(projet->list_gtk.ef_relachements.window);
        return TRUE;
    }
    else
        return FALSE;
}


G_MODULE_EXPORT void EF_gtk_relachements_select_changed(
  GtkTreeSelection *treeselection __attribute__((unused)), Projet *projet)
/* Description : En fonction de la sélection, active ou désactive le bouton supprimer.
 * Paramètres : GtkTreeSelection *treeselection : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 *   Echec : projet == NULL,
 *           interface graphique non initialisée.
 */
{
    GtkTreeModel    *model;
    GtkTreeIter     Iter;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_relachements.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Relachement");
    
    // Si aucun relâchenement n'est sélectionné, il n'est pas possible d'en supprimer ou d'en
    // éditer un.
    if (!gtk_tree_selection_get_selected(GTK_TREE_SELECTION(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_treeview_select")), &model, &Iter))
    {
        gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_boutton_supprimer_direct")), FALSE);
        gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_boutton_supprimer_menu")), FALSE);
        gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_boutton_supprimer_direct")), TRUE);
        gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_boutton_supprimer_menu")), FALSE);
    }
    else
    {
        char                *nom;
        EF_Relachement      *relachement;
        GtkCellRendererText *cell;
        
        gtk_tree_model_get(model, &Iter, 0, &nom, -1);
        
        BUG(relachement = EF_relachement_cherche_nom(projet, nom, TRUE), );
        
        if (EF_relachement_verifie_dependances(projet, relachement))
        {
            gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_boutton_supprimer_direct")), FALSE);
            gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_boutton_supprimer_menu")), TRUE);
            gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_boutton_supprimer_direct")), FALSE);
            gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_boutton_supprimer_menu")), TRUE);
        }
        else
        {
            gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_boutton_supprimer_direct")), TRUE);
            gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_boutton_supprimer_menu")), FALSE);
            gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_boutton_supprimer_direct")), TRUE);
            gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_boutton_supprimer_menu")), FALSE);
        }
        
        cell = GTK_CELL_RENDERER_TEXT(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_treeview_cell2"));
        switch (relachement->rx_debut)
        {
            case EF_RELACHEMENT_BLOQUE :
            case EF_RELACHEMENT_LIBRE :
            {
                g_object_set(cell, "editable", FALSE, NULL);
                break;
            }
            case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
            {
                g_object_set(cell, "editable", TRUE, NULL);
                break;
            }
            default :
            {
                BUGMSG(NULL, , gettext("Relachement %d inconnu."), relachement->rx_debut);
                break;
            }
        }
        cell = GTK_CELL_RENDERER_TEXT(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_treeview_cell4"));
        switch (relachement->ry_debut)
        {
            case EF_RELACHEMENT_BLOQUE :
            case EF_RELACHEMENT_LIBRE :
            {
                g_object_set(cell, "editable", FALSE, NULL);
                break;
            }
            case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
            {
                g_object_set(cell, "editable", TRUE, NULL);
                break;
            }
            default :
            {
                BUGMSG(NULL, , gettext("Relachement %d inconnu."), relachement->ry_debut);
                break;
            }
        }
        cell = GTK_CELL_RENDERER_TEXT(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_treeview_cell6"));
        switch (relachement->rz_debut)
        {
            case EF_RELACHEMENT_BLOQUE :
            case EF_RELACHEMENT_LIBRE :
            {
                g_object_set(cell, "editable", FALSE, NULL);
                break;
            }
            case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
            {
                g_object_set(cell, "editable", TRUE, NULL);
                break;
            }
            default :
            {
                BUGMSG(NULL, , gettext("Relachement %d inconnu."), relachement->rz_debut);
                break;
            }
        }
        cell = GTK_CELL_RENDERER_TEXT(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_treeview_cell8"));
        switch (relachement->rx_fin)
        {
            case EF_RELACHEMENT_BLOQUE :
            case EF_RELACHEMENT_LIBRE :
            {
                g_object_set(cell, "editable", FALSE, NULL);
                break;
            }
            case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
            {
                g_object_set(cell, "editable", TRUE, NULL);
                break;
            }
            default :
            {
                BUGMSG(NULL, , gettext("Relachement %d inconnu."), relachement->rx_fin);
                break;
            }
        }
        cell = GTK_CELL_RENDERER_TEXT(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_treeview_cell10"));
        switch (relachement->ry_fin)
        {
            case EF_RELACHEMENT_BLOQUE :
            case EF_RELACHEMENT_LIBRE :
            {
                g_object_set(cell, "editable", FALSE, NULL);
                break;
            }
            case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
            {
                g_object_set(cell, "editable", TRUE, NULL);
                break;
            }
            default :
            {
                BUGMSG(NULL, , gettext("Relachement %d inconnu."), relachement->ry_fin);
                break;
            }
        }
        cell = GTK_CELL_RENDERER_TEXT(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_treeview_cell12"));
        switch (relachement->rz_fin)
        {
            case EF_RELACHEMENT_BLOQUE :
            case EF_RELACHEMENT_LIBRE :
            {
                g_object_set(cell, "editable", FALSE, NULL);
                break;
            }
            case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
            {
                g_object_set(cell, "editable", TRUE, NULL);
                break;
            }
            default :
            {
                BUGMSG(NULL, , gettext("Relachement %d inconnu."), relachement->rz_fin);
                break;
            }
        }
        
        free(nom);
    }
    
    return;
}


G_MODULE_EXPORT void EF_gtk_relachements_edit_nom(
  GtkCellRendererText *cell __attribute__((unused)), gchar *path_string, gchar *new_text,
  Projet *projet)
/* Description : Modification du nom d'un relâchement.
 * Paramètres : GtkCellRendererText *cell : cellule en cours,
 *            : gchar *path_string : path de la ligne en cours,
 *            : gchar *new_text : le nouveau nom,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
*/
{
    Gtk_EF_Relachements *ef_gtk;
    GtkTreeModel        *model;
    GtkTreeIter         iter;
    GtkTreePath         *path;
    char                *nom;
    EF_Relachement      *relachement;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_relachements.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Relachement");
    
    ef_gtk = &projet->list_gtk.ef_relachements;
    model = GTK_TREE_MODEL(ef_gtk->relachements);
    path = gtk_tree_path_new_from_string(path_string);
    gtk_tree_model_get_iter(model, &iter, path);
    gtk_tree_path_free(path);
    gtk_tree_model_get(model, &iter, 0, &nom, -1);
    if (strcmp(nom, new_text) == 0)
    {
        free(nom);
        return;
    }
    BUG(relachement = EF_relachement_cherche_nom(projet, nom, TRUE), );
    free(nom);
    if (strcmp(relachement->nom, new_text) == 0)
        return;

    BUG(EF_relachement_renomme(relachement, new_text, projet), );
    
    return;
}


G_MODULE_EXPORT void EF_gtk_relachements_supprimer_direct(
  GtkButton *button __attribute__((unused)), Projet *projet)
/* Description : Supprime le relâchement sélectionné dans le treeview.
 * Paramètres : GtkWidget *widget : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    GtkTreeIter     iter;
    GtkTreeModel    *model;
    char            *nom;
    EF_Relachement  *relachement;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_relachements.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Relachement");
    
    if (!gtk_tree_selection_get_selected(GTK_TREE_SELECTION(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_treeview_select")), &model, &iter))
        return;
    
    gtk_tree_model_get(model, &iter, 0, &nom, -1);
    
    BUG(relachement = EF_relachement_cherche_nom(projet, nom, TRUE), );
    BUG(EF_relachement_supprime(relachement, TRUE, projet), );
    
    BUG(m3d_rafraichit(projet), );
    
    free(nom);
    
    return;
}


G_MODULE_EXPORT void EF_gtk_relachements_supprimer_menu_barres(
  GtkButton *button __attribute__((unused)), Projet *projet)
/* Description : Supprime le relachement sélectionné dans le treeview, y compris les barres
 *               l'utilisant.
 * Paramètres : GtkWidget *widget : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    GtkTreeIter     iter;
    GtkTreeModel    *model;
    char            *nom;
    EF_Relachement  *relachement;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_relachements.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Relachement");
    
    if (!gtk_tree_selection_get_selected(GTK_TREE_SELECTION(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_treeview_select")), &model, &iter))
        return;
    
    gtk_tree_model_get(model, &iter, 0, &nom, -1);
    
    BUG(relachement = EF_relachement_cherche_nom(projet, nom, TRUE), );
    BUG(EF_relachement_supprime(relachement, FALSE, projet), );
    
    BUG(m3d_rafraichit(projet), );
    
    free(nom);
    
    return;
}


G_MODULE_EXPORT gboolean EF_relachements_update_ligne_treeview(Projet *projet,
  EF_Relachement *relachement)
/* Description : Actualise la ligne du treeview affichant le relâchement.
 * Paramètres : Projet *projet : la variable projet,
 *            : EF_Relachement *relachement : le relâchement à actualiser.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL,
 *             relachement == NULL,
 *             Interface graphique non initialisée.
 */
{
    Gtk_EF_Relachements  *ef_gtk;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(relachement, FALSE, gettext("Paramètre %s incorrect.\n"), "relachement");
    BUGMSG(projet->list_gtk.ef_relachements.builder, FALSE, gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Relâchement");
    
    ef_gtk = &projet->list_gtk.ef_relachements;
    gtk_tree_store_set(ef_gtk->relachements, &relachement->Iter_fenetre, 0, relachement->nom, -1);
    switch (relachement->rx_debut)
    {
        case EF_RELACHEMENT_BLOQUE :
        {
            gtk_tree_store_set(ef_gtk->relachements, &relachement->Iter_fenetre, 1, gettext("Bloqué"), 2, "-", -1);
            break;
        }
        case EF_RELACHEMENT_LIBRE :
        {
            gtk_tree_store_set(ef_gtk->relachements, &relachement->Iter_fenetre, 1, gettext("Libre"), 2, "-", -1);
            break;
        }
        case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
        {
            EF_Relachement_Donnees_Elastique_Lineaire *data;
            char    tmp[30];
            
            data = (EF_Relachement_Donnees_Elastique_Lineaire *)relachement->rx_d_data;
            common_math_double_to_char(data->raideur, tmp, DECIMAL_NEWTON_PAR_METRE);
            gtk_tree_store_set(ef_gtk->relachements, &relachement->Iter_fenetre, 1, gettext("Linéaire"), 2, tmp, -1);
            break;
        }
        default :
        {
            BUGMSG(NULL, FALSE, "Le type de relâchement est inconnu.\n");
            break;
        }
    }
    switch (relachement->ry_debut)
    {
        case EF_RELACHEMENT_BLOQUE :
        {
            gtk_tree_store_set(ef_gtk->relachements, &relachement->Iter_fenetre, 3, gettext("Bloqué"), 4, "-", -1);
            break;
        }
        case EF_RELACHEMENT_LIBRE :
        {
            gtk_tree_store_set(ef_gtk->relachements, &relachement->Iter_fenetre, 3, gettext("Libre"), 4, "-", -1);
            break;
        }
        case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
        {
            EF_Relachement_Donnees_Elastique_Lineaire *data;
            char    tmp[30];
            
            data = (EF_Relachement_Donnees_Elastique_Lineaire *)relachement->ry_d_data;
            common_math_double_to_char(data->raideur, tmp, DECIMAL_NEWTON_PAR_METRE);
            gtk_tree_store_set(ef_gtk->relachements, &relachement->Iter_fenetre, 3, gettext("Linéaire"), 4, tmp, -1);
            break;
        }
        default :
        {
            BUGMSG(NULL, FALSE, "Le type de relâchement est inconnu.\n");
            break;
        }
    }
    switch (relachement->rz_debut)
    {
        case EF_RELACHEMENT_BLOQUE :
        {
            gtk_tree_store_set(ef_gtk->relachements, &relachement->Iter_fenetre, 5, gettext("Bloqué"), 6, "-", -1);
            break;
        }
        case EF_RELACHEMENT_LIBRE :
        {
            gtk_tree_store_set(ef_gtk->relachements, &relachement->Iter_fenetre, 5, gettext("Libre"), 6, "-", -1);
            break;
        }
        case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
        {
            EF_Relachement_Donnees_Elastique_Lineaire *data;
            char    tmp[30];
            
            data = (EF_Relachement_Donnees_Elastique_Lineaire *)relachement->rz_d_data;
            common_math_double_to_char(data->raideur, tmp, DECIMAL_NEWTON_PAR_METRE);
            gtk_tree_store_set(ef_gtk->relachements, &relachement->Iter_fenetre, 5, gettext("Linéaire"), 6, tmp, -1);
            break;
        }
        default :
        {
            BUGMSG(NULL, FALSE, "Le type de relâchement est inconnu.\n");
            break;
        }
    }
    switch (relachement->rx_fin)
    {
        case EF_RELACHEMENT_BLOQUE :
        {
            gtk_tree_store_set(ef_gtk->relachements, &relachement->Iter_fenetre, 7, gettext("Bloqué"), 8, "-", -1);
            break;
        }
        case EF_RELACHEMENT_LIBRE :
        {
            gtk_tree_store_set(ef_gtk->relachements, &relachement->Iter_fenetre, 7, gettext("Libre"), 8, "-", -1);
            break;
        }
        case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
        {
            EF_Relachement_Donnees_Elastique_Lineaire *data;
            char    tmp[30];
            
            data = (EF_Relachement_Donnees_Elastique_Lineaire *)relachement->rx_f_data;
            common_math_double_to_char(data->raideur, tmp, DECIMAL_NEWTON_PAR_METRE);
            gtk_tree_store_set(ef_gtk->relachements, &relachement->Iter_fenetre, 7, gettext("Linéaire"), 8, tmp, -1);
            break;
        }
        default :
        {
            BUGMSG(NULL, FALSE, "Le type de relâchement est inconnu.\n");
            break;
        }
    }
    switch (relachement->ry_fin)
    {
        case EF_RELACHEMENT_BLOQUE :
        {
            gtk_tree_store_set(ef_gtk->relachements, &relachement->Iter_fenetre, 9, gettext("Bloqué"), 10, "-", -1);
            break;
        }
        case EF_RELACHEMENT_LIBRE :
        {
            gtk_tree_store_set(ef_gtk->relachements, &relachement->Iter_fenetre, 9, gettext("Libre"), 10, "-", -1);
            break;
        }
        case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
        {
            EF_Relachement_Donnees_Elastique_Lineaire *data;
            char    tmp[30];
            
            data = (EF_Relachement_Donnees_Elastique_Lineaire *)relachement->ry_f_data;
            common_math_double_to_char(data->raideur, tmp, DECIMAL_NEWTON_PAR_METRE);
            gtk_tree_store_set(ef_gtk->relachements, &relachement->Iter_fenetre, 9, gettext("Linéaire"), 10, tmp, -1);
            break;
        }
        default :
        {
            BUGMSG(NULL, FALSE, "Le type de relâchement est inconnu.\n");
            break;
        }
    }
    switch (relachement->rz_fin)
    {
        case EF_RELACHEMENT_BLOQUE :
        {
            gtk_tree_store_set(ef_gtk->relachements, &relachement->Iter_fenetre, 11, gettext("Bloqué"), 12, "-", -1);
            break;
        }
        case EF_RELACHEMENT_LIBRE :
        {
            gtk_tree_store_set(ef_gtk->relachements, &relachement->Iter_fenetre, 11, gettext("Libre"), 12, "-", -1);
            break;
        }
        case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
        {
            EF_Relachement_Donnees_Elastique_Lineaire *data;
            char    tmp[30];
            
            data = (EF_Relachement_Donnees_Elastique_Lineaire *)relachement->rz_f_data;
            common_math_double_to_char(data->raideur, tmp, DECIMAL_NEWTON_PAR_METRE);
            gtk_tree_store_set(ef_gtk->relachements, &relachement->Iter_fenetre, 11, gettext("Linéaire"), 12, tmp, -1);
            break;
        }
        default :
        {
            BUGMSG(NULL, FALSE, "Le type de relâchement est inconnu.\n");
            break;
        }
    }
    
    return TRUE;
}


G_MODULE_EXPORT void EF_gtk_relachements_ajouter(GtkButton *button __attribute__((unused)),
  Projet *projet)
/* Description : Ajoute un nouveau relâchement vierge au treeview
 *               d'affichage.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    Gtk_EF_Relachements *ef_gtk;
    EF_Relachement      *relachement;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_relachements.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Relâchement");
    
    ef_gtk = &projet->list_gtk.ef_relachements;
    
    if (EF_relachement_cherche_nom(projet, gettext("Sans nom"), FALSE) == NULL)
        BUG(relachement = EF_relachement_ajout(projet, gettext("Sans nom"), EF_RELACHEMENT_BLOQUE, NULL, EF_RELACHEMENT_BLOQUE, NULL, EF_RELACHEMENT_BLOQUE, NULL, EF_RELACHEMENT_BLOQUE, NULL, EF_RELACHEMENT_BLOQUE, NULL, EF_RELACHEMENT_BLOQUE, NULL), );
    else
    {
        char    *nom;
        int     i = 2;
        
        nom = g_strdup_printf("%s (%d)", gettext("Sans nom"), i);
        while (EF_relachement_cherche_nom(projet, nom, FALSE) != NULL)
        {
            i++;
            free(nom);
            nom = g_strdup_printf("%s (%d)", gettext("Sans nom"), i);
        }
        BUG(relachement = EF_relachement_ajout(projet, nom, EF_RELACHEMENT_BLOQUE, NULL, EF_RELACHEMENT_BLOQUE, NULL, EF_RELACHEMENT_BLOQUE, NULL, EF_RELACHEMENT_BLOQUE, NULL, EF_RELACHEMENT_BLOQUE, NULL, EF_RELACHEMENT_BLOQUE, NULL), );
        free(nom);
    }
    
    gtk_tree_store_append(ef_gtk->relachements, &relachement->Iter_fenetre, NULL);
    EF_relachements_update_ligne_treeview(projet, relachement);
    
    return;
}


G_MODULE_EXPORT void EF_gtk_relachements_edit_clicked(GtkCellRendererText *cell,
  gchar *path_string, gchar *new_text, Projet *projet)
/* Description : Edite le relâchement sélectionné.
 * Paramètres : GtkCellRendererText *cell : cellule en cours,
 *            : gchar *path_string : path de la ligne en cours,
 *            : gchar *new_text : nouvelle valeur,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 *   Echec : projet == NULL,
 *           interface graphique non initialisée.
 */
{
    Gtk_EF_Relachements *ef_gtk;
    GtkTreeModel        *model;
    GtkTreeIter         iter;
    GtkTreePath         *path;
    gint                column = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(cell), "column"));
    char                *nom;
    EF_Relachement      *relachement;
    double              conversion;
    char                *fake = (char*)malloc(sizeof(char)*(strlen(new_text)+1));
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_relachements.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Relachement");
    
    ef_gtk = &projet->list_gtk.ef_relachements;
    model = GTK_TREE_MODEL(ef_gtk->relachements);
    path = gtk_tree_path_new_from_string(path_string);
    gtk_tree_model_get_iter(model, &iter, path);
    gtk_tree_path_free(path);
    gtk_tree_model_get(model, &iter, 0, &nom, -1);
    
    BUG(relachement = EF_relachement_cherche_nom(projet, nom, TRUE), );
    free(nom);
    
    if (sscanf(new_text, "%lf%s", &conversion, fake) != 1)
    {
        free(fake);
        return;
    }
    
    if (column == 0)
    {
        switch (relachement->rx_debut)
        {
            case EF_RELACHEMENT_BLOQUE :
            case EF_RELACHEMENT_LIBRE :
            {
                BUGMSG(NULL, , "Impossible d'éditer ce type de relâchement.\n");
                break;
            }
            case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
            {
                EF_Relachement_Donnees_Elastique_Lineaire *data;
                char    tmp[30];
                
                data = (EF_Relachement_Donnees_Elastique_Lineaire *)relachement->rx_d_data;
                data->raideur = conversion;
                common_math_double_to_char(data->raideur, tmp, DECIMAL_NEWTON_PAR_METRE);
                gtk_tree_store_set(ef_gtk->relachements, &iter, column*2+2, tmp, -1);
                
                break;
            }
            default :
            {
                BUGMSG(NULL, , "Le type de relâchement est inconnu.\n");
                break;
            }
        }
    }
    else if (column == 1)
    {
        switch (relachement->ry_debut)
        {
            case EF_RELACHEMENT_BLOQUE :
            case EF_RELACHEMENT_LIBRE :
            {
                BUGMSG(NULL, , "Impossible d'éditer ce type de relâchement.\n");
                break;
            }
            case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
            {
                EF_Relachement_Donnees_Elastique_Lineaire *data;
                char    tmp[30];
                
                data = (EF_Relachement_Donnees_Elastique_Lineaire *)relachement->ry_d_data;
                data->raideur = conversion;
                common_math_double_to_char(data->raideur, tmp, DECIMAL_NEWTON_PAR_METRE);
                gtk_tree_store_set(ef_gtk->relachements, &iter, column*2+2, tmp, -1);
                
                break;
            }
            default :
            {
                BUGMSG(NULL, , "Le type de relâchement est inconnu.\n");
                break;
            }
        }
    }
    else if (column == 2)
    {
        switch (relachement->rz_debut)
        {
            case EF_RELACHEMENT_BLOQUE :
            case EF_RELACHEMENT_LIBRE :
            {
                BUGMSG(NULL, , "Impossible d'éditer ce type de relâchement.\n");
                break;
            }
            case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
            {
                EF_Relachement_Donnees_Elastique_Lineaire *data;
                char    tmp[30];
                
                data = (EF_Relachement_Donnees_Elastique_Lineaire *)relachement->rz_d_data;
                data->raideur = conversion;
                common_math_double_to_char(data->raideur, tmp, DECIMAL_NEWTON_PAR_METRE);
                gtk_tree_store_set(ef_gtk->relachements, &iter, column*2+2, tmp, -1);
                
                break;
            }
            default :
            {
                BUGMSG(NULL, , "Le type de relâchement est inconnu.\n");
                break;
            }
        }
    }
    else if (column == 3)
    {
        switch (relachement->rx_fin)
        {
            case EF_RELACHEMENT_BLOQUE :
            case EF_RELACHEMENT_LIBRE :
            {
                BUGMSG(NULL, , "Impossible d'éditer ce type de relâchement.\n");
                break;
            }
            case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
            {
                EF_Relachement_Donnees_Elastique_Lineaire *data;
                char    tmp[30];
                
                data = (EF_Relachement_Donnees_Elastique_Lineaire *)relachement->rx_f_data;
                data->raideur = conversion;
                common_math_double_to_char(data->raideur, tmp, DECIMAL_NEWTON_PAR_METRE);
                gtk_tree_store_set(ef_gtk->relachements, &iter, column*2+2, tmp, -1);
                
                break;
            }
            default :
            {
                BUGMSG(NULL, , "Le type de relâchement est inconnu.\n");
                break;
            }
        }
    }
    else if (column == 4)
    {
        switch (relachement->ry_fin)
        {
            case EF_RELACHEMENT_BLOQUE :
            case EF_RELACHEMENT_LIBRE :
            {
                BUGMSG(NULL, , "Impossible d'éditer ce type de relâchement.\n");
                break;
            }
            case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
            {
                EF_Relachement_Donnees_Elastique_Lineaire *data;
                char    tmp[30];
                
                data = (EF_Relachement_Donnees_Elastique_Lineaire *)relachement->ry_f_data;
                data->raideur = conversion;
                common_math_double_to_char(data->raideur, tmp, DECIMAL_NEWTON_PAR_METRE);
                gtk_tree_store_set(ef_gtk->relachements, &iter, column*2+2, tmp, -1);
                
                break;
            }
            default :
            {
                BUGMSG(NULL, , "Le type de relâchement est inconnu.\n");
                break;
            }
        }
    }
    else if (column == 5)
    {
        switch (relachement->rz_fin)
        {
            case EF_RELACHEMENT_BLOQUE :
            case EF_RELACHEMENT_LIBRE :
            {
                BUGMSG(NULL, , "Impossible d'éditer ce type de relâchement.\n");
                break;
            }
            case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
            {
                EF_Relachement_Donnees_Elastique_Lineaire *data;
                char    tmp[30];
                
                data = (EF_Relachement_Donnees_Elastique_Lineaire *)relachement->rz_f_data;
                data->raideur = conversion;
                common_math_double_to_char(data->raideur, tmp, DECIMAL_NEWTON_PAR_METRE);
                gtk_tree_store_set(ef_gtk->relachements, &iter, column*2+2, tmp, -1);
                
                break;
            }
            default :
            {
                BUGMSG(NULL, , "Le type de relâchement est inconnu.\n");
                break;
            }
        }
    }
    
    return;
}


G_MODULE_EXPORT void EF_gtk_relachements_edit_type(GtkCellRendererText *cell,
  gchar *path_string, gchar *new_text, Projet *projet)
/* Description : Edite le type du relâchement sélectionné.
 * Paramètres : GtkCellRendererText *cell : cellule en cours,
 *            : gchar *path_string : path de la ligne en cours,
 *            : gchar *new_text : le nouveau type du relâchement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 *   Echec : projet == NULL,
 *           interface graphique non initialisée.
 */
{
    Gtk_EF_Relachements *ef_gtk;
    GtkTreeModel        *model;
    GtkTreeIter         iter;
    GtkTreePath         *path;
    gint                column = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(cell), "column"));
    char                *nom;
    EF_Relachement      *relachement;
    EF_Relachement_Type type;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_relachements.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Relachement");
    
    ef_gtk = &projet->list_gtk.ef_relachements;
    model = GTK_TREE_MODEL(ef_gtk->relachements);
    path = gtk_tree_path_new_from_string(path_string);
    gtk_tree_model_get_iter(model, &iter, path);
    gtk_tree_path_free(path);
    gtk_tree_model_get(model, &iter, 0, &nom, -1);
    
    BUG(relachement = EF_relachement_cherche_nom(projet, nom, TRUE), );
    free(nom);
    
    if (strcmp(gettext("Bloqué"), new_text) == 0)
        type = EF_RELACHEMENT_BLOQUE;
    else if (strcmp(gettext("Libre"), new_text) == 0)
        type = EF_RELACHEMENT_LIBRE;
    else if (strcmp(gettext("Linéaire"), new_text) == 0)
        type = EF_RELACHEMENT_ELASTIQUE_LINEAIRE;
    else
        BUGMSG(NULL, , gettext("Le type de relâchement est inconnu.\n"));
    
    switch (column)
    {
        case 0 :
        {
            if (relachement->rx_debut == type)
                return;
            
            switch (relachement->rx_debut)
            {
                case EF_RELACHEMENT_BLOQUE :
                case EF_RELACHEMENT_LIBRE :
                    break;
                case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
                {
                    free(relachement->rx_d_data);
                    break;
                }
                default :
                {
                    BUGMSG(NULL, , "Le type de relâchement est inconnu.\n");
                    break;
                }
            }
            
            relachement->rx_debut = type;
            
            switch (type)
            {
                case EF_RELACHEMENT_BLOQUE :
                {
                    relachement->rx_d_data = NULL;
                    gtk_tree_store_set(ef_gtk->relachements, &iter, column*2+1, gettext("Bloqué"), column*2+2, "-", -1);
                    g_object_set(GTK_CELL_RENDERER_TEXT(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_treeview_cell2")), "editable", FALSE, NULL);
                    break;
                }
                case EF_RELACHEMENT_LIBRE :
                {
                    relachement->rx_d_data = NULL;
                    gtk_tree_store_set(ef_gtk->relachements, &iter, column*2+1, gettext("Libre"), column*2+2, "-", -1);
                    g_object_set(GTK_CELL_RENDERER_TEXT(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_treeview_cell2")), "editable", FALSE, NULL);
                    break;
                }
                case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
                {
                    EF_Relachement_Donnees_Elastique_Lineaire   *data;
                    
                    BUGMSG(data = malloc(sizeof(EF_Relachement_Donnees_Elastique_Lineaire)), , gettext("Erreur d'allocation mémoire.\n"));
                    data->raideur = 0;
                    relachement->rx_d_data = data;
                    gtk_tree_store_set(ef_gtk->relachements, &iter, column*2+1, gettext("Linéaire"), column*2+2, "0", -1);
                    g_object_set(GTK_CELL_RENDERER_TEXT(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_treeview_cell2")), "editable", TRUE, NULL);
                    
                    break;
                }
                default :
                {
                    BUGMSG(NULL, , "Le type de relâchement est inconnu.\n");
                    break;
                }
            }
            break;
        }
        case 1 :
        {
            if (relachement->ry_debut == type)
                return;
            
            switch (relachement->ry_debut)
            {
                case EF_RELACHEMENT_BLOQUE :
                case EF_RELACHEMENT_LIBRE :
                    break;
                case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
                {
                    free(relachement->ry_d_data);
                    break;
                }
                default :
                {
                    BUGMSG(NULL, , "Le type de relâchement est inconnu.\n");
                    break;
                }
            }
            
            relachement->ry_debut = type;
            
            switch (type)
            {
                case EF_RELACHEMENT_BLOQUE :
                {
                    relachement->ry_d_data = NULL;
                    gtk_tree_store_set(ef_gtk->relachements, &iter, column*2+1, gettext("Bloqué"), column*2+2, "-", -1);
                    g_object_set(GTK_CELL_RENDERER_TEXT(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_treeview_cell4")), "editable", FALSE, NULL);
                    break;
                }
                case EF_RELACHEMENT_LIBRE :
                {
                    relachement->ry_d_data = NULL;
                    gtk_tree_store_set(ef_gtk->relachements, &iter, column*2+1, gettext("Libre"), column*2+2, "-", -1);
                    g_object_set(GTK_CELL_RENDERER_TEXT(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_treeview_cell4")), "editable", FALSE, NULL);
                    break;
                }
                case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
                {
                    EF_Relachement_Donnees_Elastique_Lineaire   *data;
                    
                    BUGMSG(data = malloc(sizeof(EF_Relachement_Donnees_Elastique_Lineaire)), , gettext("Erreur d'allocation mémoire.\n"));
                    data->raideur = 0;
                    relachement->ry_d_data = data;
                    gtk_tree_store_set(ef_gtk->relachements, &iter, column*2+1, gettext("Linéaire"), column*2+2, "0", -1);
                    g_object_set(GTK_CELL_RENDERER_TEXT(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_treeview_cell4")), "editable", TRUE, NULL);
                    
                    break;
                }
                default :
                {
                    BUGMSG(NULL, , "Le type de relâchement est inconnu.\n");
                    break;
                }
            }
            break;
        }
        case 2 :
        {
            if (relachement->rz_debut == type)
                return;
            
            switch (relachement->rz_debut)
            {
                case EF_RELACHEMENT_BLOQUE :
                case EF_RELACHEMENT_LIBRE :
                    break;
                case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
                {
                    free(relachement->rz_d_data);
                    break;
                }
                default :
                {
                    BUGMSG(NULL, , "Le type de relâchement est inconnu.\n");
                    break;
                }
            }
            
            relachement->rz_debut = type;
            
            switch (type)
            {
                case EF_RELACHEMENT_BLOQUE :
                {
                    relachement->rz_d_data = NULL;
                    gtk_tree_store_set(ef_gtk->relachements, &iter, column*2+1, gettext("Bloqué"), column*2+2, "-", -1);
                    g_object_set(GTK_CELL_RENDERER_TEXT(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_treeview_cell6")), "editable", FALSE, NULL);
                    break;
                }
                case EF_RELACHEMENT_LIBRE :
                {
                    relachement->rz_d_data = NULL;
                    gtk_tree_store_set(ef_gtk->relachements, &iter, column*2+1, gettext("Libre"), column*2+2, "-", -1);
                    g_object_set(GTK_CELL_RENDERER_TEXT(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_treeview_cell6")), "editable", FALSE, NULL);
                    break;
                }
                case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
                {
                    EF_Relachement_Donnees_Elastique_Lineaire   *data;
                    
                    BUGMSG(data = malloc(sizeof(EF_Relachement_Donnees_Elastique_Lineaire)), , gettext("Erreur d'allocation mémoire.\n"));
                    data->raideur = 0;
                    relachement->rz_d_data = data;
                    gtk_tree_store_set(ef_gtk->relachements, &iter, column*2+1, gettext("Linéaire"), column*2+2, "0", -1);
                    g_object_set(GTK_CELL_RENDERER_TEXT(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_treeview_cell6")), "editable", TRUE, NULL);
                    
                    break;
                }
                default :
                {
                    BUGMSG(NULL, , "Le type de relâchement est inconnu.\n");
                    break;
                }
            }
            break;
        }
        case 3 :
        {
            if (relachement->rx_fin == type)
                return;
            
            switch (relachement->rx_fin)
            {
                case EF_RELACHEMENT_BLOQUE :
                case EF_RELACHEMENT_LIBRE :
                    break;
                case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
                {
                    free(relachement->rx_f_data);
                    break;
                }
                default :
                {
                    BUGMSG(NULL, , "Le type de relâchement est inconnu.\n");
                    break;
                }
            }
            
            relachement->rx_fin = type;
            
            switch (type)
            {
                case EF_RELACHEMENT_BLOQUE :
                {
                    relachement->rx_f_data = NULL;
                    gtk_tree_store_set(ef_gtk->relachements, &iter, column*2+1, gettext("Bloqué"), column*2+2, "-", -1);
                    g_object_set(GTK_CELL_RENDERER_TEXT(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_treeview_cell8")), "editable", FALSE, NULL);
                    break;
                }
                case EF_RELACHEMENT_LIBRE :
                {
                    relachement->rx_f_data = NULL;
                    gtk_tree_store_set(ef_gtk->relachements, &iter, column*2+1, gettext("Libre"), column*2+2, "-", -1);
                    g_object_set(GTK_CELL_RENDERER_TEXT(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_treeview_cell8")), "editable", FALSE, NULL);
                    break;
                }
                case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
                {
                    EF_Relachement_Donnees_Elastique_Lineaire   *data;
                    
                    BUGMSG(data = malloc(sizeof(EF_Relachement_Donnees_Elastique_Lineaire)), , gettext("Erreur d'allocation mémoire.\n"));
                    data->raideur = 0;
                    relachement->rx_f_data = data;
                    gtk_tree_store_set(ef_gtk->relachements, &iter, column*2+1, gettext("Linéaire"), column*2+2, "0", -1);
                    g_object_set(GTK_CELL_RENDERER_TEXT(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_treeview_cell8")), "editable", TRUE, NULL);
                    
                    break;
                }
                default :
                {
                    BUGMSG(NULL, , "Le type de relâchement est inconnu.\n");
                    break;
                }
            }
            break;
        }
        case 4 :
        {
            if (relachement->ry_fin == type)
                return;
            
            switch (relachement->ry_fin)
            {
                case EF_RELACHEMENT_BLOQUE :
                case EF_RELACHEMENT_LIBRE :
                    break;
                case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
                {
                    free(relachement->ry_f_data);
                    break;
                }
                default :
                {
                    BUGMSG(NULL, , "Le type de relâchement est inconnu.\n");
                    break;
                }
            }
            
            relachement->ry_fin = type;
            
            switch (type)
            {
                case EF_RELACHEMENT_BLOQUE :
                {
                    relachement->ry_f_data = NULL;
                    gtk_tree_store_set(ef_gtk->relachements, &iter, column*2+1, gettext("Bloqué"), column*2+2, "-", -1);
                    g_object_set(GTK_CELL_RENDERER_TEXT(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_treeview_cell10")), "editable", FALSE, NULL);
                    break;
                }
                case EF_RELACHEMENT_LIBRE :
                {
                    relachement->ry_f_data = NULL;
                    gtk_tree_store_set(ef_gtk->relachements, &iter, column*2+1, gettext("Libre"), column*2+2, "-", -1);
                    g_object_set(GTK_CELL_RENDERER_TEXT(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_treeview_cell10")), "editable", FALSE, NULL);
                    break;
                }
                case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
                {
                    EF_Relachement_Donnees_Elastique_Lineaire   *data;
                    
                    BUGMSG(data = malloc(sizeof(EF_Relachement_Donnees_Elastique_Lineaire)), , gettext("Erreur d'allocation mémoire.\n"));
                    data->raideur = 0;
                    relachement->ry_f_data = data;
                    gtk_tree_store_set(ef_gtk->relachements, &iter, column*2+1, gettext("Linéaire"), column*2+2, "0", -1);
                    g_object_set(GTK_CELL_RENDERER_TEXT(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_treeview_cell10")), "editable", TRUE, NULL);
                    
                    break;
                }
                default :
                {
                    BUGMSG(NULL, , "Le type de relâchement est inconnu.\n");
                    break;
                }
            }
            break;
        }
        case 5 :
        {
            if (relachement->rz_fin == type)
                return;
            
            switch (relachement->rz_fin)
            {
                case EF_RELACHEMENT_BLOQUE :
                case EF_RELACHEMENT_LIBRE :
                    break;
                case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
                {
                    free(relachement->rz_f_data);
                    break;
                }
                default :
                {
                    BUGMSG(NULL, , "Le type de relâchement est inconnu.\n");
                    break;
                }
            }
            
            relachement->rz_fin = type;
            
            switch (type)
            {
                case EF_RELACHEMENT_BLOQUE :
                {
                    relachement->rz_f_data = NULL;
                    gtk_tree_store_set(ef_gtk->relachements, &iter, column*2+1, gettext("Bloqué"), column*2+2, "-", -1);
                    g_object_set(GTK_CELL_RENDERER_TEXT(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_treeview_cell12")), "editable", FALSE, NULL);
                    break;
                }
                case EF_RELACHEMENT_LIBRE :
                {
                    relachement->rz_f_data = NULL;
                    gtk_tree_store_set(ef_gtk->relachements, &iter, column*2+1, gettext("Libre"), column*2+2, "-", -1);
                    g_object_set(GTK_CELL_RENDERER_TEXT(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_treeview_cell12")), "editable", FALSE, NULL);
                    break;
                }
                case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
                {
                    EF_Relachement_Donnees_Elastique_Lineaire   *data;
                    
                    BUGMSG(data = malloc(sizeof(EF_Relachement_Donnees_Elastique_Lineaire)), , gettext("Erreur d'allocation mémoire.\n"));
                    data->raideur = 0;
                    relachement->rz_f_data = data;
                    gtk_tree_store_set(ef_gtk->relachements, &iter, column*2+1, gettext("Linéaire"), column*2+2, "0", -1);
                    g_object_set(GTK_CELL_RENDERER_TEXT(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_treeview_cell12")), "editable", TRUE, NULL);
                    
                    break;
                }
                default :
                {
                    BUGMSG(NULL, , "Le type de relâchement est inconnu.\n");
                    break;
                }
            }
            break;
        }
        default :
        {
            BUGMSG(NULL, , "Paramètre %s incorrect.\n", "column");
            break;
        }
    }
    
    return;
}


G_MODULE_EXPORT void EF_gtk_relachement(Projet *projet)
/* Description : Création de la fenêtre permettant d'afficher et d'éditer les relâchements sous
 *               forme d'un tableau.
 * Paramètres : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 *   Echec : projet == NULL,
 *           interface graphique impossible à générer.
 */
{
    Gtk_EF_Relachements *ef_gtk;
    GList               *list_parcours;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    if (projet->list_gtk.ef_relachements.builder != NULL)
    {
        gtk_window_present(GTK_WINDOW(projet->list_gtk.ef_relachements.window));
        return;
    }
    
    ef_gtk = &projet->list_gtk.ef_relachements;
    
    ef_gtk->builder = gtk_builder_new();
    BUGMSG(gtk_builder_add_from_file(ef_gtk->builder, DATADIR"/ui/EF_relachements.ui", NULL) != 0, , gettext("Builder Failed\n"));
    gtk_builder_connect_signals(ef_gtk->builder, projet);
    
    g_object_set_data(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_treeview_cell1"), "column", GINT_TO_POINTER(0));
    g_object_set_data(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_treeview_cell2"), "column", GINT_TO_POINTER(0));
    g_object_set_data(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_treeview_cell3"), "column", GINT_TO_POINTER(1));
    g_object_set_data(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_treeview_cell4"), "column", GINT_TO_POINTER(1));
    g_object_set_data(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_treeview_cell5"), "column", GINT_TO_POINTER(2));
    g_object_set_data(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_treeview_cell6"), "column", GINT_TO_POINTER(2));
    g_object_set_data(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_treeview_cell7"), "column", GINT_TO_POINTER(3));
    g_object_set_data(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_treeview_cell8"), "column", GINT_TO_POINTER(3));
    g_object_set_data(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_treeview_cell9"), "column", GINT_TO_POINTER(4));
    g_object_set_data(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_treeview_cell10"), "column", GINT_TO_POINTER(4));
    g_object_set_data(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_treeview_cell11"), "column", GINT_TO_POINTER(5));
    g_object_set_data(gtk_builder_get_object(projet->list_gtk.ef_relachements.builder, "EF_relachements_treeview_cell12"), "column", GINT_TO_POINTER(5));
    
    ef_gtk->window = GTK_WIDGET(gtk_builder_get_object(ef_gtk->builder, "EF_relachements_window"));
    ef_gtk->relachements = GTK_TREE_STORE(gtk_builder_get_object(ef_gtk->builder, "EF_relachements_treestore"));
    
    list_parcours = projet->ef_donnees.relachements;
    while (list_parcours != NULL)
    {
        EF_Relachement  *relachement = (EF_Relachement *)list_parcours->data;
        
        gtk_tree_store_append(ef_gtk->relachements, &relachement->Iter_fenetre, NULL);
        EF_relachements_update_ligne_treeview(projet, relachement);
        
        list_parcours = g_list_next(list_parcours);
    }
    
    gtk_window_set_transient_for(GTK_WINDOW(ef_gtk->window), GTK_WINDOW(projet->list_gtk.comp.window));
}

#endif
