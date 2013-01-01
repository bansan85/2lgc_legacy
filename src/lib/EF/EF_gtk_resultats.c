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

#include "1990_action.h"
#include "common_projet.h"
#include "common_erreurs.h"
#include "common_gtk.h"
#include "EF_noeuds.h"

G_MODULE_EXPORT void EF_gtk_resultats_fermer(GtkButton *button __attribute__((unused)),
  Projet *projet)
/* Description : Ferme la fenêtre.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_resultats.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Résultats");
    
    gtk_widget_destroy(projet->list_gtk.ef_resultats.window);
    
    return;
}


G_MODULE_EXPORT void EF_gtk_resultats_window_destroy(GtkWidget *object __attribute__((unused)),
  Projet *projet)
/* Description : met projet->list_gtk.ef_resultats.builder à NULL quand la fenêtre se ferme,
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->list_gtk.ef_resultats.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Résultats");
    
    g_object_unref(G_OBJECT(projet->list_gtk.ef_resultats.builder));
    projet->list_gtk.ef_resultats.builder = NULL;
    
    return;
}


G_MODULE_EXPORT gboolean EF_gtk_resultats_window_key_press(
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
    BUGMSG(projet->list_gtk.ef_resultats.builder, TRUE, gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Résultats");
    
    if (event->key.keyval == GDK_KEY_Escape)
    {
        gtk_widget_destroy(projet->list_gtk.ef_resultats.window);
        return TRUE;
    }
    else
        return FALSE;
}


G_MODULE_EXPORT void EF_gtk_resultats_notebook_switch(GtkNotebook *notebook,
  GtkWidget *page __attribute__((unused)), gint page_num,
  Projet *projet __attribute__((unused)))
/* Description : Le changement de la page en cours nécessite l'actualisation de la disponibilité
 *               du bouton supprimer.
 * Paramètres : GtkNotebook *notebook : le composant notebook,
 *            : GtkWidget *page : composant désignant la page,
 *            : guint page_num : numéro de la page,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    if (page_num == gtk_notebook_get_n_pages(notebook)-1)
        g_signal_stop_emission_by_name(notebook, "switch-page");
    
    return;
}


gboolean EF_gtk_resultats_remplit_page(Gtk_EF_Resultats_Tableau *res, Projet *projet)
/* Description : Remplit/actualise la page du treeview via la variable res.
 * Paramètres : Gtk_EF_Resultats_Tableau *res : caractéristiques de la page à remplir,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : TRUE si pas de problème, FALSE sinon.
 *   Echec : projet == NULL,
 */
{
    unsigned int        i;
    Action              *action_en_cours = _1990_action_cherche_numero(projet, 0);
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(res, FALSE, gettext("Paramètre %s incorrect.\n"), "res");
    BUGMSG(projet->list_gtk.ef_resultats.builder, FALSE, gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Résultats");
    
    gtk_list_store_clear(res->list_store);
    
    if (res->col_tab[1] == COLRES_NUM_NOEUDS)
    {
        GList   *list_parcours = projet->ef_donnees.noeuds;
        
        i = 0;
        
        while (list_parcours != NULL)
        {
            EF_Noeud    *noeud = list_parcours->data;
            gboolean    ok;
            
            switch (res->filtre)
            {
                case FILTRE_AUCUN :
                {
                    ok = TRUE;
                    break;
                }
                case FILTRE_NOEUD_APPUI :
                {
                    if (noeud->appui == NULL)
                        ok = FALSE;
                    else
                        ok = TRUE;
                    break;
                }
                default :
                {
                    BUGMSG(NULL, FALSE, gettext("Le filtre %d est inconnu.\n"), res->filtre);
                    break;
                }
            }
            
            if (ok)
            {
                GtkTreeIter     Iter;
                double          *x = action_en_cours->efforts_noeuds->x;
                double          *y = action_en_cours->deplacement_complet->x;
                unsigned int    j;
                
                gtk_list_store_append(res->list_store, &Iter);
                for (j=1;j<=res->col_tab[0];j++)
                {
                    switch (res->col_tab[j])
                    {
                        case COLRES_NUM_NOEUDS :
                        {
                            gtk_list_store_set(res->list_store, &Iter, j-1, noeud->numero, -1);
                            break;
                        }
                        case COLRES_NOEUDS_X :
                        {
                            EF_Point    *point = EF_noeuds_renvoie_position(noeud);
                            
                            gtk_list_store_set(res->list_store, &Iter, j-1, point->x, -1);
                            
                            free(point);
                            break;
                        }
                        case COLRES_NOEUDS_Y :
                        {
                            EF_Point    *point = EF_noeuds_renvoie_position(noeud);
                            
                            gtk_list_store_set(res->list_store, &Iter, j-1, point->y, -1);
                            
                            free(point);
                            break;
                        }
                        case COLRES_NOEUDS_Z :
                        {
                            EF_Point    *point = EF_noeuds_renvoie_position(noeud);
                            
                            gtk_list_store_set(res->list_store, &Iter, j-1, point->z, -1);
                            
                            free(point);
                            break;
                        }
                        case COLRES_REACTION_APPUI_FX :
                        {
                            gtk_list_store_set(res->list_store, &Iter, j-1, x[i*6+0], -1);
                            break;
                        }
                        case COLRES_REACTION_APPUI_FY:
                        {
                            gtk_list_store_set(res->list_store, &Iter, j-1, x[i*6+1], -1);
                            break;
                        }
                        case COLRES_REACTION_APPUI_FZ :
                        {
                            gtk_list_store_set(res->list_store, &Iter, j-1, x[i*6+2], -1);
                            break;
                        }
                        case COLRES_REACTION_APPUI_MX :
                        {
                            gtk_list_store_set(res->list_store, &Iter, j-1, x[i*6+3], -1);
                            break;
                        }
                        case COLRES_REACTION_APPUI_MY :
                        {
                            gtk_list_store_set(res->list_store, &Iter, j-1, x[i*6+4], -1);
                            break;
                        }
                        case COLRES_REACTION_APPUI_MZ :
                        {
                            gtk_list_store_set(res->list_store, &Iter, j-1, x[i*6+5], -1);
                            break;
                        }
                        case COLRES_DEPLACEMENT_UX :
                        {
                            gtk_list_store_set(res->list_store, &Iter, j-1, y[i*6+0], -1);
                            break;
                        }
                        case COLRES_DEPLACEMENT_UY :
                        {
                            gtk_list_store_set(res->list_store, &Iter, j-1, y[i*6+1], -1);
                            break;
                        }
                        case COLRES_DEPLACEMENT_UZ :
                        {
                            gtk_list_store_set(res->list_store, &Iter, j-1, y[i*6+2], -1);
                            break;
                        }
                        case COLRES_DEPLACEMENT_RX :
                        {
                            gtk_list_store_set(res->list_store, &Iter, j-1, y[i*6+3], -1);
                            break;
                        }
                        case COLRES_DEPLACEMENT_RY :
                        {
                            gtk_list_store_set(res->list_store, &Iter, j-1, y[i*6+4], -1);
                            break;
                        }
                        case COLRES_DEPLACEMENT_RZ :
                        {
                            gtk_list_store_set(res->list_store, &Iter, j-1, y[i*6+5], -1);
                            break;
                        }
                        case COLRES_NUM_BARRES :
                        case COLRES_BARRES_LONGUEUR :
                        {
                            BUGMSG(NULL, FALSE, gettext("La colonne des résultats %d ne peut être appliquée aux noeuds."), res->col_tab[j]);
                            break;
                        }
                        default :
                        {
                            BUGMSG(NULL, FALSE, gettext("La colonne des résultats %d est inconnue.\n"), res->col_tab[j]);
                            break;
                        }
                    }
                }
                gtk_list_store_set(res->list_store, &Iter, res->col_tab[0], "", -1);
            }
            
            i++;
            list_parcours = g_list_next(list_parcours);
        }
    }
    
    if (res->col_tab[1] == COLRES_NUM_BARRES)
    {
        GList   *list_parcours = projet->beton.barres;
        
        i = 0;
        
        while (list_parcours != NULL)
        {
            Beton_Barre *barre = list_parcours->data;
            gboolean    ok;
            
            switch (res->filtre)
            {
                case FILTRE_AUCUN :
                {
                    ok = TRUE;
                    break;
                }
                case FILTRE_NOEUD_APPUI :
                {
                    BUGMSG(NULL, FALSE, gettext("Le filtre %d ne peut être appliqué aux barres.\n"), res->filtre);
                    break;
                }
                default :
                {
                    BUGMSG(NULL, FALSE, gettext("Le filtre %d est inconnu.\n"), res->filtre);
                    break;
                }
            }
            
            if (ok)
            {
                GtkTreeIter     Iter;
                unsigned int    j;
                
                gtk_list_store_append(res->list_store, &Iter);
                for (j=1;j<=res->col_tab[0];j++)
                {
                    switch (res->col_tab[j])
                    {
                        case COLRES_NUM_NOEUDS :
                        case COLRES_NOEUDS_X :
                        case COLRES_NOEUDS_Y :
                        case COLRES_NOEUDS_Z :
                        case COLRES_REACTION_APPUI_FX :
                        case COLRES_REACTION_APPUI_FY:
                        case COLRES_REACTION_APPUI_FZ :
                        case COLRES_REACTION_APPUI_MX :
                        case COLRES_REACTION_APPUI_MY :
                        case COLRES_REACTION_APPUI_MZ :
                        case COLRES_DEPLACEMENT_UX :
                        case COLRES_DEPLACEMENT_UY :
                        case COLRES_DEPLACEMENT_UZ :
                        case COLRES_DEPLACEMENT_RX :
                        case COLRES_DEPLACEMENT_RY :
                        case COLRES_DEPLACEMENT_RZ :
                        {
                            BUGMSG(NULL, FALSE, gettext("La colonne des résultats %d ne peut être appliquée aux barres."), res->col_tab[j]);
                            break;
                        }
                        case COLRES_NUM_BARRES :
                        {
                            gtk_list_store_set(res->list_store, &Iter, j-1, barre->numero, -1);
                            break;
                        }
                        case COLRES_BARRES_LONGUEUR :
                        {
                            gtk_list_store_set(res->list_store, &Iter, j-1, EF_noeuds_distance(barre->noeud_debut, barre->noeud_fin), -1);
                            break;
                        }
                        default :
                        {
                            BUGMSG(NULL, FALSE, gettext("La colonne des résultats %d est inconnue.\n"), res->col_tab[j]);
                            break;
                        }
                    }
                }
                gtk_list_store_set(res->list_store, &Iter, res->col_tab[0], "", -1);
            }
            
            i++;
            list_parcours = g_list_next(list_parcours);
        }
    }
    
    return TRUE;
}


gboolean EF_gtk_resultats_add_page(Gtk_EF_Resultats_Tableau *res, Projet *projet)
/* Description : Ajoute une page au treeview de la fenêtre affichant les résultats en fonction
 *               de la description fournie via la variable res.
 * Paramètres : Gtk_EF_Resultats_Tableau *res : caractéristiques de la page à ajouter,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : TRUE si pas de problème, FALSE sinon.
 *   Echec : projet == NULL,
 */
{
    GtkWidget           *p_scrolled_window; 
    unsigned int        i;
    GType               *col_type;
    GtkCellRenderer     *cell;
    GtkTreeViewColumn   *column;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(res, FALSE, gettext("Paramètre %s incorrect.\n"), "res");
    BUGMSG(projet->list_gtk.ef_resultats.builder, FALSE, gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Résultats");
    
    p_scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(p_scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    
    gtk_notebook_insert_page(projet->list_gtk.ef_resultats.notebook, p_scrolled_window, GTK_WIDGET(gtk_label_new(res->nom)), gtk_notebook_get_n_pages(projet->list_gtk.ef_resultats.notebook)-1);
    
    res->treeview = GTK_TREE_VIEW(gtk_tree_view_new());
    gtk_container_add(GTK_CONTAINER(p_scrolled_window), GTK_WIDGET(res->treeview));
    
    BUGMSG(col_type = malloc((res->col_tab[0]+1)*sizeof(GType)), FALSE, gettext("Erreur d'allocation mémoire.\n"));
    
    for (i=1;i<=res->col_tab[0];i++)
    {
        switch (res->col_tab[i])
        {
            case COLRES_NUM_NOEUDS :
            {
                BUGMSG(i==1, FALSE, gettext("La liste des noeuds doit être spécifiée en tant que première colonne.\n"));
                cell = gtk_cell_renderer_text_new();
                gtk_cell_renderer_set_alignment(cell, 0.5, 0.5);
                column = gtk_tree_view_column_new_with_attributes(gettext("Noeuds"), cell, "text", i-1, NULL);
                gtk_tree_view_column_set_alignment(column, 0.5);
                gtk_tree_view_append_column(res->treeview, column);
                col_type[i-1] = G_TYPE_INT;
                break;
            }
            case COLRES_NUM_BARRES :
            {
                BUGMSG(i==1, FALSE, gettext("La liste des barres doit être spécifiée en tant que première colonne.\n"));
                cell = gtk_cell_renderer_text_new();
                gtk_cell_renderer_set_alignment(cell, 0.5, 0.5);
                column = gtk_tree_view_column_new_with_attributes(gettext("Barres"), cell, "text", i-1, NULL);
                gtk_tree_view_column_set_alignment(column, 0.5);
                gtk_tree_view_append_column(res->treeview, column);
                col_type[i-1] = G_TYPE_INT;
                break;
            }
            case COLRES_NOEUDS_X :
            {
                BUGMSG(i!=1, FALSE, gettext("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                BUGMSG(res->col_tab[1] == COLRES_NUM_NOEUDS, FALSE, gettext("La position en %s ne peut être affichée que si la première colonne affiche les numéros des noeuds.\n"), "x");
                cell = gtk_cell_renderer_text_new();
                gtk_cell_renderer_set_alignment(cell, 1.0, 0.5);
                column = gtk_tree_view_column_new_with_attributes(gettext("x [m]"), cell, "text", i-1, NULL);
                gtk_tree_view_column_set_alignment(column, 0.5);
                gtk_tree_view_append_column(res->treeview, column);
                g_object_set_data(G_OBJECT(cell), "column", GINT_TO_POINTER(i-1));
                gtk_tree_view_column_set_cell_data_func(column, cell, common_gtk_render_double, GINT_TO_POINTER(DECIMAL_DISTANCE), NULL);
                col_type[i-1] = G_TYPE_DOUBLE;
                break;
            }
            case COLRES_NOEUDS_Y :
            {
                BUGMSG(i!=1, FALSE, gettext("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                BUGMSG(res->col_tab[1] == COLRES_NUM_NOEUDS, FALSE, gettext("La position en %s ne peut être affichée que si la première colonne affiche les numéros des noeuds.\n"), "y");
                cell = gtk_cell_renderer_text_new();
                gtk_cell_renderer_set_alignment(cell, 1.0, 0.5);
                column = gtk_tree_view_column_new_with_attributes(gettext("y [m]"), cell, "text", i-1, NULL);
                gtk_tree_view_column_set_alignment(column, 0.5);
                gtk_tree_view_append_column(res->treeview, column);
                g_object_set_data(G_OBJECT(cell), "column", GINT_TO_POINTER(i-1));
                gtk_tree_view_column_set_cell_data_func(column, cell, common_gtk_render_double, GINT_TO_POINTER(DECIMAL_DISTANCE), NULL);
                col_type[i-1] = G_TYPE_DOUBLE;
                break;
            }
            case COLRES_NOEUDS_Z :
            {
                BUGMSG(i!=1, FALSE, gettext("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                BUGMSG(res->col_tab[1] == COLRES_NUM_NOEUDS, FALSE, gettext("La position en %s ne peut être affichée que si la première colonne affiche les numéros des noeuds.\n"), "z");
                cell = gtk_cell_renderer_text_new();
                gtk_cell_renderer_set_alignment(cell, 1.0, 0.5);
                column = gtk_tree_view_column_new_with_attributes(gettext("z [m]"), cell, "text", i-1, NULL);
                gtk_tree_view_column_set_alignment(column, 0.5);
                gtk_tree_view_append_column(res->treeview, column);
                g_object_set_data(G_OBJECT(cell), "column", GINT_TO_POINTER(i-1));
                gtk_tree_view_column_set_cell_data_func(column, cell, common_gtk_render_double, GINT_TO_POINTER(DECIMAL_DISTANCE), NULL);
                col_type[i-1] = G_TYPE_DOUBLE;
                break;
            }
            case COLRES_REACTION_APPUI_FX :
            {
                BUGMSG(i!=1, FALSE, gettext("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                BUGMSG(res->col_tab[1] == COLRES_NUM_NOEUDS, FALSE, gettext("La réaction d'appui %s ne peut être affichée que si la première colonne affiche les numéros des noeuds.\n"), "Fx");
                cell = gtk_cell_renderer_text_new();
                gtk_cell_renderer_set_alignment(cell, 1.0, 0.5);
                column = gtk_tree_view_column_new_with_attributes(gettext("Fx [N]"), cell, "text", i-1, NULL);
                gtk_tree_view_column_set_alignment(column, 0.5);
                gtk_tree_view_append_column(res->treeview, column);
                g_object_set_data(G_OBJECT(cell), "column", GINT_TO_POINTER(i-1));
                gtk_tree_view_column_set_cell_data_func(column, cell, common_gtk_render_double, GINT_TO_POINTER(DECIMAL_FORCE), NULL);
                col_type[i-1] = G_TYPE_DOUBLE;
                break;
            }
            case COLRES_REACTION_APPUI_FY:
            {
                BUGMSG(i!=1, FALSE, gettext("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                BUGMSG(res->col_tab[1] == COLRES_NUM_NOEUDS, FALSE, gettext("La réaction d'appui %s ne peut être affichée que si la première colonne affiche les numéros des noeuds.\n"), "Fy");
                cell = gtk_cell_renderer_text_new();
                gtk_cell_renderer_set_alignment(cell, 1.0, 0.5);
                column = gtk_tree_view_column_new_with_attributes(gettext("Fy [N]"), cell, "text", i-1, NULL);
                gtk_tree_view_column_set_alignment(column, 0.5);
                gtk_tree_view_append_column(res->treeview, column);
                g_object_set_data(G_OBJECT(cell), "column", GINT_TO_POINTER(i-1));
                gtk_tree_view_column_set_cell_data_func(column, cell, common_gtk_render_double, GINT_TO_POINTER(DECIMAL_FORCE), NULL);
                col_type[i-1] = G_TYPE_DOUBLE;
                break;
            }
            case COLRES_REACTION_APPUI_FZ :
            {
                BUGMSG(i!=1, FALSE, gettext("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                BUGMSG(res->col_tab[1] == COLRES_NUM_NOEUDS, FALSE, gettext("La réaction d'appui %s ne peut être affichée que si la première colonne affiche les numéros des noeuds.\n"), "Fz");
                cell = gtk_cell_renderer_text_new();
                gtk_cell_renderer_set_alignment(cell, 1.0, 0.5);
                column = gtk_tree_view_column_new_with_attributes(gettext("Fz [N]"), cell, "text", i-1, NULL);
                gtk_tree_view_column_set_alignment(column, 0.5);
                gtk_tree_view_append_column(res->treeview, column);
                g_object_set_data(G_OBJECT(cell), "column", GINT_TO_POINTER(i-1));
                gtk_tree_view_column_set_cell_data_func(column, cell, common_gtk_render_double, GINT_TO_POINTER(DECIMAL_FORCE), NULL);
                col_type[i-1] = G_TYPE_DOUBLE;
                break;
            }
            case COLRES_REACTION_APPUI_MX :
            {
                BUGMSG(i!=1, FALSE, gettext("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                BUGMSG(res->col_tab[1] == COLRES_NUM_NOEUDS, FALSE, gettext("La réaction d'appui %s ne peut être affichée que si la première colonne affiche les numéros des noeuds.\n"), "Mx");
                cell = gtk_cell_renderer_text_new();
                gtk_cell_renderer_set_alignment(cell, 1.0, 0.5);
                column = gtk_tree_view_column_new_with_attributes(gettext("Mx [N.m]"), cell, "text", i-1, NULL);
                gtk_tree_view_column_set_alignment(column, 0.5);
                gtk_tree_view_append_column(res->treeview, column);
                g_object_set_data(G_OBJECT(cell), "column", GINT_TO_POINTER(i-1));
                gtk_tree_view_column_set_cell_data_func(column, cell, common_gtk_render_double, GINT_TO_POINTER(DECIMAL_MOMENT), NULL);
                col_type[i-1] = G_TYPE_DOUBLE;
                break;
            }
            case COLRES_REACTION_APPUI_MY :
            {
                BUGMSG(i!=1, FALSE, gettext("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                BUGMSG(res->col_tab[1] == COLRES_NUM_NOEUDS, FALSE, gettext("La réaction d'appui %s ne peut être affichée que si la première colonne affiche les numéros des noeuds.\n"), "My");
                cell = gtk_cell_renderer_text_new();
                gtk_cell_renderer_set_alignment(cell, 1.0, 0.5);
                column = gtk_tree_view_column_new_with_attributes(gettext("My [N.m]"), cell, "text", i-1, NULL);
                gtk_tree_view_column_set_alignment(column, 0.5);
                gtk_tree_view_append_column(res->treeview, column);
                g_object_set_data(G_OBJECT(cell), "column", GINT_TO_POINTER(i-1));
                gtk_tree_view_column_set_cell_data_func(column, cell, common_gtk_render_double, GINT_TO_POINTER(DECIMAL_MOMENT), NULL);
                col_type[i-1] = G_TYPE_DOUBLE;
                break;
            }
            case COLRES_REACTION_APPUI_MZ :
            {
                BUGMSG(i!=1, FALSE, gettext("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                BUGMSG(res->col_tab[1] == COLRES_NUM_NOEUDS, FALSE, gettext("La réaction d'appui %s ne peut être affichée que si la première colonne affiche les numéros des noeuds.\n"), "Mz");
                cell = gtk_cell_renderer_text_new();
                gtk_cell_renderer_set_alignment(cell, 1.0, 0.5);
                column = gtk_tree_view_column_new_with_attributes(gettext("Mz [N.m]"), cell, "text", i-1, NULL);
                gtk_tree_view_column_set_alignment(column, 0.5);
                gtk_tree_view_append_column(res->treeview, column);
                g_object_set_data(G_OBJECT(cell), "column", GINT_TO_POINTER(i-1));
                gtk_tree_view_column_set_cell_data_func(column, cell, common_gtk_render_double, GINT_TO_POINTER(DECIMAL_MOMENT), NULL);
                col_type[i-1] = G_TYPE_DOUBLE;
                break;
            }
            case COLRES_DEPLACEMENT_UX :
            {
                BUGMSG(i!=1, FALSE, gettext("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                BUGMSG(res->col_tab[1] == COLRES_NUM_NOEUDS, FALSE, gettext("Le déplacement des noeuds %s ne peut être affichée que si la première colonne affiche les numéros des noeuds.\n"), "Ux");
                cell = gtk_cell_renderer_text_new();
                gtk_cell_renderer_set_alignment(cell, 1.0, 0.5);
                column = gtk_tree_view_column_new_with_attributes(gettext("Ux [m]"), cell, "text", i-1, NULL);
                gtk_tree_view_column_set_alignment(column, 0.5);
                gtk_tree_view_append_column(res->treeview, column);
                g_object_set_data(G_OBJECT(cell), "column", GINT_TO_POINTER(i-1));
                gtk_tree_view_column_set_cell_data_func(column, cell, common_gtk_render_double, GINT_TO_POINTER(DECIMAL_DEPLACEMENT), NULL);
                col_type[i-1] = G_TYPE_DOUBLE;
                break;
            }
            case COLRES_DEPLACEMENT_UY :
            {
                BUGMSG(i!=1, FALSE, gettext("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                BUGMSG(res->col_tab[1] == COLRES_NUM_NOEUDS, FALSE, gettext("Le déplacement des noeuds %s ne peut être affichée que si la première colonne affiche les numéros des noeuds.\n"), "Uy");
                cell = gtk_cell_renderer_text_new();
                gtk_cell_renderer_set_alignment(cell, 1.0, 0.5);
                column = gtk_tree_view_column_new_with_attributes(gettext("Uy [m]"), cell, "text", i-1, NULL);
                gtk_tree_view_column_set_alignment(column, 0.5);
                gtk_tree_view_append_column(res->treeview, column);
                g_object_set_data(G_OBJECT(cell), "column", GINT_TO_POINTER(i-1));
                gtk_tree_view_column_set_cell_data_func(column, cell, common_gtk_render_double, GINT_TO_POINTER(DECIMAL_DEPLACEMENT), NULL);
                col_type[i-1] = G_TYPE_DOUBLE;
                break;
            }
            case COLRES_DEPLACEMENT_UZ :
            {
                BUGMSG(i!=1, FALSE, gettext("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                BUGMSG(res->col_tab[1] == COLRES_NUM_NOEUDS, FALSE, gettext("Le déplacement des noeuds %s ne peut être affichée que si la première colonne affiche les numéros des noeuds.\n"), "Uz");
                cell = gtk_cell_renderer_text_new();
                gtk_cell_renderer_set_alignment(cell, 1.0, 0.5);
                column = gtk_tree_view_column_new_with_attributes(gettext("Uz [m]"), cell, "text", i-1, NULL);
                gtk_tree_view_column_set_alignment(column, 0.5);
                gtk_tree_view_append_column(res->treeview, column);
                g_object_set_data(G_OBJECT(cell), "column", GINT_TO_POINTER(i-1));
                gtk_tree_view_column_set_cell_data_func(column, cell, common_gtk_render_double, GINT_TO_POINTER(DECIMAL_DEPLACEMENT), NULL);
                col_type[i-1] = G_TYPE_DOUBLE;
                break;
            }
            case COLRES_DEPLACEMENT_RX :
            {
                BUGMSG(i!=1, FALSE, gettext("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                BUGMSG(res->col_tab[1] == COLRES_NUM_NOEUDS, FALSE, gettext("Le déplacement des noeuds %s ne peut être affichée que si la première colonne affiche les numéros des noeuds.\n"), "Rx");
                cell = gtk_cell_renderer_text_new();
                gtk_cell_renderer_set_alignment(cell, 1.0, 0.5);
                column = gtk_tree_view_column_new_with_attributes(gettext("Rx [rad]"), cell, "text", i-1, NULL);
                gtk_tree_view_column_set_alignment(column, 0.5);
                gtk_tree_view_append_column(res->treeview, column);
                g_object_set_data(G_OBJECT(cell), "column", GINT_TO_POINTER(i-1));
                gtk_tree_view_column_set_cell_data_func(column, cell, common_gtk_render_double, GINT_TO_POINTER(DECIMAL_ROTATION), NULL);
                col_type[i-1] = G_TYPE_DOUBLE;
                break;
            }
            case COLRES_DEPLACEMENT_RY :
            {
                BUGMSG(i!=1, FALSE, gettext("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                BUGMSG(res->col_tab[1] == COLRES_NUM_NOEUDS, FALSE, gettext("Le déplacement des noeuds %s ne peut être affichée que si la première colonne affiche les numéros des noeuds.\n"), "Ry");
                cell = gtk_cell_renderer_text_new();
                gtk_cell_renderer_set_alignment(cell, 1.0, 0.5);
                column = gtk_tree_view_column_new_with_attributes(gettext("Ry [rad]"), cell, "text", i-1, NULL);
                gtk_tree_view_column_set_alignment(column, 0.5);
                gtk_tree_view_append_column(res->treeview, column);
                g_object_set_data(G_OBJECT(cell), "column", GINT_TO_POINTER(i-1));
                gtk_tree_view_column_set_cell_data_func(column, cell, common_gtk_render_double, GINT_TO_POINTER(DECIMAL_ROTATION), NULL);
                col_type[i-1] = G_TYPE_DOUBLE;
                break;
            }
            case COLRES_DEPLACEMENT_RZ :
            {
                BUGMSG(i!=1, FALSE, gettext("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                BUGMSG(res->col_tab[1] == COLRES_NUM_NOEUDS, FALSE, gettext("Le déplacement des noeuds %s ne peut être affichée que si la première colonne affiche les numéros des noeuds.\n"), "Rx");
                cell = gtk_cell_renderer_text_new();
                gtk_cell_renderer_set_alignment(cell, 1.0, 0.5);
                column = gtk_tree_view_column_new_with_attributes(gettext("Rz [rad]"), cell, "text", i-1, NULL);
                gtk_tree_view_column_set_alignment(column, 0.5);
                gtk_tree_view_append_column(res->treeview, column);
                g_object_set_data(G_OBJECT(cell), "column", GINT_TO_POINTER(i-1));
                gtk_tree_view_column_set_cell_data_func(column, cell, common_gtk_render_double, GINT_TO_POINTER(DECIMAL_ROTATION), NULL);
                col_type[i-1] = G_TYPE_DOUBLE;
                break;
            }
            case COLRES_BARRES_LONGUEUR :
            {
                BUGMSG(i!=1, FALSE, gettext("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                BUGMSG(res->col_tab[1] == COLRES_NUM_BARRES, FALSE, gettext("La longueur des barres ne peut être affichée que si la première colonne affiche les numéros des barres.\n"));
                cell = gtk_cell_renderer_text_new();
                gtk_cell_renderer_set_alignment(cell, 1.0, 0.5);
                column = gtk_tree_view_column_new_with_attributes(gettext("L [m]"), cell, "text", i-1, NULL);
                gtk_tree_view_column_set_alignment(column, 0.5);
                gtk_tree_view_append_column(res->treeview, column);
                g_object_set_data(G_OBJECT(cell), "column", GINT_TO_POINTER(i-1));
                gtk_tree_view_column_set_cell_data_func(column, cell, common_gtk_render_double, GINT_TO_POINTER(DECIMAL_DISTANCE), NULL);
                col_type[i-1] = G_TYPE_DOUBLE;
                break;
            }
            default :
            {
                BUGMSG(NULL, FALSE, gettext("La colonne des résultats %d est inconnue.\n"), res->col_tab[i]);
                break;
            }
        }
    }
    
    // On insère une colonne vide à la fin pour éviter le redimensionnement automatique de la
    // dernière colonne.
    cell = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("", cell, "text", res->col_tab[0], NULL);
    gtk_tree_view_append_column(res->treeview, column);
    col_type[res->col_tab[0]] = G_TYPE_STRING;
    
    res->list_store = gtk_list_store_newv((gint)res->col_tab[0]+1, col_type);
    free(col_type);
    gtk_tree_view_set_model(res->treeview, GTK_TREE_MODEL(res->list_store));
    g_object_unref(res->list_store);

    BUG(EF_gtk_resultats_remplit_page(res, projet), FALSE);
    
    gtk_widget_show_all(p_scrolled_window);
    
    gtk_notebook_set_current_page(projet->list_gtk.ef_resultats.notebook, gtk_notebook_get_n_pages(projet->list_gtk.ef_resultats.notebook)-2);
    
    return TRUE;
}


G_MODULE_EXPORT void EF_gtk_resultats_add_page_type(GtkMenuItem *menuitem, Projet *projet)
/* Description : Ajoute une page au Notebook sur la base de divers tableaux "de base".
 * Paramètres : GtkMenuItem *menuitem : le composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    Gtk_EF_Resultats_Tableau    *res;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(menuitem, , gettext("Paramètre %s incorrect.\n"), "menuitem");
    BUGMSG(projet->list_gtk.ef_resultats.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Résultats");
    BUGMSG(res = malloc(sizeof(Gtk_EF_Resultats_Tableau)), , gettext("Erreur d'allocation mémoire.\n"));
    
    if (strcmp(gtk_menu_item_get_label(menuitem), gettext("Noeuds")) == 0)
    {
        BUGMSG(res->col_tab = malloc(sizeof(Colonne_Resultats)*5), , gettext("Erreur d'allocation mémoire.\n"));
        res->col_tab[0] = 4;
        res->col_tab[1] = COLRES_NUM_NOEUDS;
        res->col_tab[2] = COLRES_NOEUDS_X;
        res->col_tab[3] = COLRES_NOEUDS_Y;
        res->col_tab[4] = COLRES_NOEUDS_Z;
        
        res->filtre = FILTRE_AUCUN;
        
        BUGMSG(res->nom = g_strdup_printf("%s", gtk_menu_item_get_label(menuitem)), , gettext("Erreur d'allocation mémoire.\n"));
        
        BUG(EF_gtk_resultats_add_page(res, projet), );
        
        projet->list_gtk.ef_resultats.tableaux = g_list_append(projet->list_gtk.ef_resultats.tableaux, res);
    }
    else if (strcmp(gtk_menu_item_get_label(menuitem), gettext("Réactions d'appuis")) == 0)
    {
        BUGMSG(res->col_tab = malloc(sizeof(Colonne_Resultats)*8), , gettext("Erreur d'allocation mémoire.\n"));
        res->col_tab[0] = 7;
        res->col_tab[1] = COLRES_NUM_NOEUDS;
        res->col_tab[2] = COLRES_REACTION_APPUI_FX;
        res->col_tab[3] = COLRES_REACTION_APPUI_FY;
        res->col_tab[4] = COLRES_REACTION_APPUI_FZ;
        res->col_tab[5] = COLRES_REACTION_APPUI_MX;
        res->col_tab[6] = COLRES_REACTION_APPUI_MY;
        res->col_tab[7] = COLRES_REACTION_APPUI_MZ;
        
        res->filtre = FILTRE_NOEUD_APPUI;
        
        BUGMSG(res->nom = g_strdup_printf("%s", gtk_menu_item_get_label(menuitem)), , gettext("Erreur d'allocation mémoire.\n"));
        
        BUG(EF_gtk_resultats_add_page(res, projet), );
        
        projet->list_gtk.ef_resultats.tableaux = g_list_append(projet->list_gtk.ef_resultats.tableaux, res);
    }
    else if (strcmp(gtk_menu_item_get_label(menuitem), gettext("Déplacements")) == 0)
    {
        BUGMSG(res->col_tab = malloc(sizeof(Colonne_Resultats)*8), , gettext("Erreur d'allocation mémoire.\n"));
        res->col_tab[0] = 7;
        res->col_tab[1] = COLRES_NUM_NOEUDS;
        res->col_tab[2] = COLRES_DEPLACEMENT_UX;
        res->col_tab[3] = COLRES_DEPLACEMENT_UY;
        res->col_tab[4] = COLRES_DEPLACEMENT_UZ;
        res->col_tab[5] = COLRES_DEPLACEMENT_RX;
        res->col_tab[6] = COLRES_DEPLACEMENT_RY;
        res->col_tab[7] = COLRES_DEPLACEMENT_RZ;
        
        res->filtre = FILTRE_AUCUN;
        
        BUGMSG(res->nom = g_strdup_printf("%s", gtk_menu_item_get_label(menuitem)), , gettext("Erreur d'allocation mémoire.\n"));
        
        BUG(EF_gtk_resultats_add_page(res, projet), );
        
        projet->list_gtk.ef_resultats.tableaux = g_list_append(projet->list_gtk.ef_resultats.tableaux, res);
    }
    else if (strcmp(gtk_menu_item_get_label(menuitem), gettext("Barres")) == 0)
    {
        BUGMSG(res->col_tab = malloc(sizeof(Colonne_Resultats)*3), , gettext("Erreur d'allocation mémoire.\n"));
        res->col_tab[0] = 2;
        res->col_tab[1] = COLRES_NUM_BARRES;
        res->col_tab[2] = COLRES_BARRES_LONGUEUR;
        
        res->filtre = FILTRE_AUCUN;
        
        BUGMSG(res->nom = g_strdup_printf("%s", gtk_menu_item_get_label(menuitem)), , gettext("Erreur d'allocation mémoire.\n"));
        
        BUG(EF_gtk_resultats_add_page(res, projet), );
        
        projet->list_gtk.ef_resultats.tableaux = g_list_append(projet->list_gtk.ef_resultats.tableaux, res);
    }
    
    return;
}


G_MODULE_EXPORT void EF_gtk_resultats(Projet *projet)
/* Description : Création de la fenêtre permettant d'afficher les résultats sous forme d'un
 *               tableau.
 * Paramètres : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 *   Echec : projet == NULL,
 *           interface graphique impossible à générer.
 */
{
    Gtk_EF_Resultats    *ef_gtk;
    GList               *list_parcours;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    
    BUG(_1990_action_affiche_resultats(projet, 0), );
    
    if (projet->list_gtk.ef_resultats.builder != NULL)
    {
        gtk_window_present(GTK_WINDOW(projet->list_gtk.ef_resultats.window));
        return;
    }
    
    ef_gtk = &projet->list_gtk.ef_resultats;
    
    ef_gtk->builder = gtk_builder_new();
    BUGMSG(gtk_builder_add_from_file(ef_gtk->builder, DATADIR"/ui/EF_resultats.ui", NULL) != 0, , gettext("Builder Failed\n"));
    gtk_builder_connect_signals(ef_gtk->builder, projet);
    
    ef_gtk->window = GTK_WIDGET(gtk_builder_get_object(ef_gtk->builder, "EF_resultats_window"));
    ef_gtk->notebook = GTK_NOTEBOOK(gtk_builder_get_object(ef_gtk->builder, "EF_resultats_notebook"));
    
    list_parcours = ef_gtk->tableaux;
    while (list_parcours != NULL)
    {
        EF_gtk_resultats_add_page(list_parcours->data, projet);
        
        list_parcours = g_list_next(list_parcours);
    }
    
    gtk_window_set_transient_for(GTK_WINDOW(ef_gtk->window), GTK_WINDOW(projet->list_gtk.comp.window));
}


G_MODULE_EXPORT void EF_gtk_resultats_free(Projet *projet)
/* Description : Libère l'ensemble des éléments utilisés pour l'affichage des résultats.
 * Paramètres : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 *   Echec : projet == NULL,
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    
    while (projet->list_gtk.ef_resultats.tableaux != NULL)
    {
        Gtk_EF_Resultats_Tableau    *res = projet->list_gtk.ef_resultats.tableaux->data;
        
        free(res->col_tab);
        free(res->nom);
        free(res);
        
        projet->list_gtk.ef_resultats.tableaux = g_list_delete_link(projet->list_gtk.ef_resultats.tableaux, projet->list_gtk.ef_resultats.tableaux);
    }
    
    return;
}
#endif
