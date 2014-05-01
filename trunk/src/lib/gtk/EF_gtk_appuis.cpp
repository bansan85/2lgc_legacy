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

#include "common_projet.hpp"
#include "common_erreurs.hpp"
#include "common_gtk.hpp"
#include "common_selection.hpp"
#include "common_text.hpp"
#include "EF_appuis.hpp"
#include "1992_1_1_barres.hpp"
#include "EF_gtk_appuis.hpp"


GTK_WINDOW_CLOSE (ef, appuis);


GTK_WINDOW_DESTROY (ef, appuis, );


GTK_WINDOW_KEY_PRESS (ef, appuis);


/**
 * \brief Ajoute un nouvel appui.
 * \param button : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée,
 *     - erreur d'allocation mémoire.
 */
extern "C"
void
EF_gtk_appuis_ajouter (GtkButton *button,
                       Projet    *p)
{
  EF_Appui     *appui;
  GtkTreePath  *path;
  GtkTreeModel *model;
  
  BUGPARAM (p, "%p", p, )
  BUGCRIT (UI_APP.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Appui"); )
  
  if (EF_appuis_cherche_nom (p, gettext ("Sans nom"), FALSE) == NULL)
  {
    BUG (appui = EF_appuis_ajout (p,
                                  gettext ("Sans nom"),
                                  EF_APPUI_LIBRE,
                                  EF_APPUI_LIBRE,
                                  EF_APPUI_LIBRE,
                                  EF_APPUI_LIBRE,
                                  EF_APPUI_LIBRE,
                                  EF_APPUI_LIBRE),
         )
  }
  else
  {
    char    *nom;
    uint16_t i = 2;
    
    BUGCRIT (nom = g_strdup_printf ("%s (%d)", gettext ("Sans nom"), i),
             ,
             (gettext ("Erreur d'allocation mémoire.\n")); )
    while (EF_appuis_cherche_nom (p, nom, FALSE) != NULL)
    {
      i++;
      free (nom);
      BUGCRIT (nom = g_strdup_printf ("%s (%d)", gettext ("Sans nom"), i),
               ,
               (gettext ("Erreur d'allocation mémoire.\n")); )
    }
    BUG (appui = EF_appuis_ajout (p,
                                  nom,
                                  EF_APPUI_LIBRE,
                                  EF_APPUI_LIBRE,
                                  EF_APPUI_LIBRE,
                                  EF_APPUI_LIBRE,
                                  EF_APPUI_LIBRE,
                                  EF_APPUI_LIBRE),
         ,
         free (nom); )
    free (nom);
  }
  
  model = GTK_TREE_MODEL (gtk_builder_get_object (UI_APP.builder,
                                                  "EF_appuis_treestore"));
  path = gtk_tree_model_get_path (model, &appui->Iter_fenetre);
  gtk_tree_view_set_cursor (GTK_TREE_VIEW (gtk_builder_get_object (
                                        UI_APP.builder, "EF_appuis_treeview")),
                            path,
                            gtk_tree_view_get_column (GTK_TREE_VIEW (
                                        gtk_builder_get_object (UI_APP.builder,
                                                        "EF_appuis_treeview")),
                                                      0),
                            TRUE);
  gtk_tree_path_free (path);
}


/**
 * \brief Supprime l'appui sélectionné dans le treeview.
 * \param button : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
extern "C"
void
EF_gtk_appuis_supprimer (GtkButton *button,
                         Projet    *p)
{
  GtkTreeIter   iter;
  GtkTreeModel *model;
  EF_Appui     *appui;
  
  BUGPARAM (p, "%p", p, )
  BUGCRIT (UI_APP.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Appui"); )
  
  if (!gtk_tree_selection_get_selected (GTK_TREE_SELECTION (
         gtk_builder_get_object (UI_APP.builder, "EF_appuis_treeview_select")),
                                        &model,
                                        &iter))
  {
    return;
  }
  
  gtk_tree_model_get (model, &iter, 0, &appui, -1);
  
  BUG (EF_appuis_supprime (appui, TRUE, FALSE, p), )
  
  BUG (m3d_rafraichit (p), )
  
  return;
}


/**
 * \brief Supprime un appui sans dépendance si la touche SUPPR est appuyée.
 * \param widget : composant à l'origine de l'évènement,
 * \param event : Caractéristique de l'évènement,
 * \param p : la variable projet.
 * \return TRUE si la touche SUPPR est pressée, FALSE sinon.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 *  
 */
extern "C"
gboolean
EF_gtk_appuis_treeview_key_press (GtkWidget *widget,
                                  GdkEvent  *event,
                                  Projet    *p)
{
  BUGPARAM (p, "%p", p, FALSE)
  BUGCRIT (UI_APP.builder,
           FALSE,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Appui"); )
  
  if (event->key.keyval == GDK_KEY_Delete)
  {
    GtkTreeIter   Iter;
    GtkTreeModel *model;
    
    if (gtk_tree_selection_get_selected (GTK_TREE_SELECTION (
         gtk_builder_get_object (UI_APP.builder, "EF_appuis_treeview_select")),
                                         &model,
                                         &Iter))
    {
      EF_Appui *appui;
      GList    *liste_appuis = NULL;
      
      gtk_tree_model_get (model, &Iter, 0, &appui, -1);
      
      liste_appuis = g_list_append (liste_appuis, appui);
      if (_1992_1_1_barres_cherche_dependances (p,
                                                liste_appuis,
                                                NULL,
                                                NULL,
                                                NULL,
                                                NULL,
                                                NULL,
                                                NULL,
                                                NULL,
                                                NULL,
                                                FALSE,
                                                FALSE) == FALSE)
      {
        EF_gtk_appuis_supprimer (NULL, p);
      }
      
      g_list_free (liste_appuis);
    }
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}


/**
 * \brief Supprime l'appui sélectionné dans le treeview et ainsi que les noeuds
 *        utilisant l'appui.
 * \param button : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
extern "C"
void
EF_gtk_appuis_supprimer_menu_suppr_noeud (GtkButton *button,
                                          Projet    *p)
{
  GtkTreeIter   iter;
  GtkTreeModel *model;
  EF_Appui     *appui;
  
  BUGPARAM (p, "%p", p, )
  BUGCRIT (UI_APP.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Appui"); )
  
  if (!gtk_tree_selection_get_selected (GTK_TREE_SELECTION (
         gtk_builder_get_object (UI_APP.builder, "EF_appuis_treeview_select")),
                                        &model,
                                        &iter))
  {
    return;
  }
  
  gtk_tree_model_get (model, &iter, 0, &appui, -1);
  
  BUG (EF_appuis_supprime (appui, FALSE, TRUE, p), )
  
  BUG (m3d_rafraichit (p), )
  
  return;
}


/**
 * \brief Supprime l'appui sélectionné dans le treeview et les noeuds le
 *        possédant deviennent sans appui.
 * \param button : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
extern "C"
void
EF_gtk_appuis_supprimer_menu_modif_noeud (GtkButton *button,
                                          Projet    *p)
{
  GtkTreeIter   iter;
  GtkTreeModel *model;
  EF_Appui     *appui;
  
  BUGPARAM (p, "%p", p, )
  BUGCRIT (UI_APP.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Appui"); )
  
  if (!gtk_tree_selection_get_selected (GTK_TREE_SELECTION (
         gtk_builder_get_object (UI_APP.builder, "EF_appuis_treeview_select")),
                                        &model,
                                        &iter))
  {
    return;
  }
  
  gtk_tree_model_get (model, &iter, 0, &appui, -1);
  
  BUG (EF_appuis_supprime (appui, FALSE, FALSE, p), )
  
  return;
}


/**
 * \brief Modification d'un appui.
 * \param cell : cellule en cours,
 * \param path_string : path de la ligne en cours,
 * \param new_text : nouvelle valeur,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
extern "C"
void
EF_gtk_appuis_edit_type (GtkCellRendererText *cell,
                         gchar               *path_string,
                         gchar               *new_text,
                         Projet              *p)
{
  GtkTreeModel  *model;
  GtkTreeIter    iter;
  GtkTreePath   *path;
  EF_Appui      *appui;
  uint8_t        column;
  
  BUGPARAM (p, "%p", p, )
  BUGCRIT (UI_APP.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Appui"); )
  
  column = (uint8_t) (GPOINTER_TO_INT (g_object_get_data (G_OBJECT (cell),
                                                          "column")) - 1);
  
  model = GTK_TREE_MODEL (UI_APP.appuis);
  path = gtk_tree_path_new_from_string (path_string);
  gtk_tree_model_get_iter (model, &iter, path);
  gtk_tree_path_free (path);
  gtk_tree_model_get (model, &iter, 0, &appui, -1);

  if (strcmp (new_text, gettext ("Libre")) == 0)
  {
    BUG (EF_appuis_edit (appui, column, EF_APPUI_LIBRE, p), )
  }
  else if (strcmp (new_text, gettext ("Bloqué")) == 0)
  {
    BUG (EF_appuis_edit (appui, column, EF_APPUI_BLOQUE, p), )
  }
  else
  {
    FAILINFO ( ,
              (gettext ("Type d'appui %s inconnu.\n"), new_text); )
  }
  
  return;
}


/**
 * \brief Modification du nom d'un appui.
 * \param cell : cellule en cours,
 * \param path_string : path de la ligne en cours,
 * \param new_text : le nouveau nom,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
extern "C"
void
EF_gtk_appuis_edit_nom (GtkCellRendererText *cell,
                        gchar               *path_string,
                        gchar               *new_text,
                        Projet              *p)
{
  GtkTreeModel *model;
  GtkTreeIter   iter;
  GtkTreePath  *path;
  EF_Appui     *appui;
  
  BUGPARAM (p, "%p", p, )
  BUGCRIT (UI_APP.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Appui"); )
  
  model = GTK_TREE_MODEL (UI_APP.appuis);
  path = gtk_tree_path_new_from_string (path_string);
  gtk_tree_model_get_iter (model, &iter, path);
  gtk_tree_path_free (path);
  gtk_tree_model_get (model, &iter, 0, &appui, -1);
  if (strcmp (appui->nom, new_text) == 0)
  {
    return;
  }

  if (EF_appuis_renomme (appui, new_text, p, FALSE))
  {
    return;
  }
  
  return;
}


/**
 * \brief En fonction de la sélection, active ou désactive le bouton supprimer.
 * \param treeselection : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
extern "C"
void
EF_gtk_appuis_select_changed (GtkTreeSelection *treeselection,
                              Projet           *p)
{
  GtkTreeModel *model;
  GtkTreeIter   Iter;
  
  BUGPARAM (p, "%p", p, )
  BUGCRIT (UI_APP.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Appui"); )
  
  // Si aucun appui n'est sélectionné, il n'est pas possible d'en supprimer un.
  if (!gtk_tree_selection_get_selected (GTK_TREE_SELECTION (
         gtk_builder_get_object (UI_APP.builder, "EF_appuis_treeview_select")),
                                        &model,
                                        &Iter))
  {
    gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
                        UI_APP.builder, "EF_appuis_boutton_supprimer_direct")),
                              FALSE);
    gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
                          UI_APP.builder, "EF_appuis_boutton_supprimer_menu")),
                              FALSE);
    gtk_widget_set_visible (GTK_WIDGET (gtk_builder_get_object (UI_APP.builder,
                                        "EF_appuis_boutton_supprimer_direct")),
                            FALSE);
    gtk_widget_set_visible (GTK_WIDGET (gtk_builder_get_object (UI_APP.builder,
                                          "EF_appuis_boutton_supprimer_menu")),
                            TRUE);
  }
  else
  {
    EF_Appui *appui;
    GList    *liste_appuis = NULL;
    
    gtk_tree_model_get (model, &Iter, 0, &appui, -1);
    
    liste_appuis = g_list_append (liste_appuis, appui);
    if (_1992_1_1_barres_cherche_dependances (p,
                                              liste_appuis,
                                              NULL,
                                              NULL,
                                              NULL,
                                              NULL,
                                              NULL,
                                              NULL,
                                              NULL,
                                              NULL,
                                              FALSE,
                                              FALSE))
    {
      gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
                        UI_APP.builder, "EF_appuis_boutton_supprimer_direct")),
                                FALSE);
      gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
                          UI_APP.builder, "EF_appuis_boutton_supprimer_menu")),
                                TRUE);
      gtk_widget_set_visible (GTK_WIDGET (gtk_builder_get_object (
                        UI_APP.builder, "EF_appuis_boutton_supprimer_direct")),
                              FALSE);
      gtk_widget_set_visible (GTK_WIDGET (gtk_builder_get_object (
                          UI_APP.builder, "EF_appuis_boutton_supprimer_menu")),
                              TRUE);
    }
    else
    {
      gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
                        UI_APP.builder, "EF_appuis_boutton_supprimer_direct")),
                                TRUE);
      gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
                          UI_APP.builder, "EF_appuis_boutton_supprimer_menu")),
                                FALSE);
      gtk_widget_set_visible (GTK_WIDGET (gtk_builder_get_object (
                        UI_APP.builder, "EF_appuis_boutton_supprimer_direct")),
                              TRUE);
      gtk_widget_set_visible (GTK_WIDGET (gtk_builder_get_object (
                          UI_APP.builder, "EF_appuis_boutton_supprimer_menu")),
                              FALSE);
    }
    
    g_list_free (liste_appuis);
  }
  
  return;
}


/**
 * \brief Affiche la liste des dépendances dans le menu lorsqu'on clique sur le
 *        bouton.
 * \param widget : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
extern "C"
void
EF_gtk_appuis_boutton_supprimer_menu (GtkButton *widget,
                                      Projet    *p)
{
  GtkTreeModel *model;
  GtkTreeIter   Iter;
  EF_Appui     *appui;
  GList        *liste_appuis = NULL;
  GList        *liste_noeuds_dep, *liste_barres_dep, *liste_charges_dep;
  
  BUGPARAM (p, "%p", p, )
  BUGCRIT (UI_APP.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Appui"); )
  
  if (!gtk_tree_selection_get_selected (GTK_TREE_SELECTION (
         gtk_builder_get_object (UI_APP.builder, "EF_appuis_treeview_select")),
                                        &model,
                                        &Iter))
  {
    FAILINFO ( , (gettext ("Aucun élément n'est sélectionné.\n")); )
  }
  
  gtk_tree_model_get (model, &Iter, 0, &appui, -1);
  
  liste_appuis = g_list_append (liste_appuis, appui);
  BUG (_1992_1_1_barres_cherche_dependances (p,
                                             liste_appuis,
                                             NULL,
                                             NULL,
                                             NULL,
                                             NULL,
                                             NULL,
                                             &liste_noeuds_dep,
                                             &liste_barres_dep,
                                             &liste_charges_dep,
                                             FALSE,
                                             FALSE),
       )
  g_list_free (liste_appuis);
  
  if ((liste_noeuds_dep != NULL) ||
      (liste_barres_dep != NULL) ||
      (liste_charges_dep != NULL))
  {
    char *desc;
    
    BUGCRIT (desc = common_text_dependances (liste_noeuds_dep,
                                             liste_barres_dep,
                                             liste_charges_dep,
                                             p),
             ,
             (gettext ("Erreur d'allocation mémoire.\n"));
               g_list_free (liste_noeuds_dep);
               g_list_free (liste_barres_dep);
               g_list_free (liste_charges_dep); )
    gtk_menu_item_set_label (GTK_MENU_ITEM (gtk_builder_get_object (
                      UI_APP.builder, "EF_appuis_supprimer_menu_suppr_noeud")),
                             desc);
    free (desc);
  }
  else
  {
    FAILINFO ( ,
              (gettext ("L'élément ne possède aucune dépendance.\n"));
                g_list_free (liste_noeuds_dep);
                g_list_free (liste_barres_dep);
                g_list_free (liste_charges_dep); )
  }
  
  g_list_free (liste_noeuds_dep);
  g_list_free (liste_barres_dep);
  g_list_free (liste_charges_dep);
  
  return;
}


/**
 * \brief Affiche le nom de l'appui.
 * \param tree_column : composant à l'origine de l'évènement,
 * \param cell : la cellule en cours d'édition,
 * \param tree_model : le mode en cours d'édition,
 * \param iter : la ligne en cours d'édition,
 * \param data2 : la variable projet.
 * \return Rien.
 */
void
EF_gtk_appuis_render_0 (GtkTreeViewColumn *tree_column,
                        GtkCellRenderer   *cell,
                        GtkTreeModel      *tree_model,
                        GtkTreeIter       *iter,
                        gpointer           data2)
{
  EF_Appui *appui;
  
  gtk_tree_model_get (tree_model, iter, 0, &appui, -1);
  BUGPARAM (appui, "%p", appui, )
  
  g_object_set (cell, "text", appui->nom, NULL);
}


#define EF_GTK_APPUIS_RENDER(NUM, DATA) void \
EF_gtk_appuis_render_##NUM (GtkTreeViewColumn *tree_column, \
                            GtkCellRenderer   *cell,        \
                            GtkTreeModel      *tree_model,  \
                            GtkTreeIter       *iter,        \
                            gpointer           data2) \
{ \
  EF_Appui *appui; \
  char     *txt; \
  \
  gtk_tree_model_get (tree_model, iter, 0, &appui, -1); \
  BUGPARAM (appui, "%p", appui, ) \
  \
  switch (appui->DATA) \
  { \
    case EF_APPUI_LIBRE : \
    { \
      txt = gettext ("Libre"); \
      BUGCRIT (appui->DATA##_donnees == NULL, \
               , \
               (gettext ("Le type d'appui de %s (%s) n'a pas à posséder de données.\n"), \
                         #DATA, \
                         gettext ("Libre")); ) \
      break; \
    } \
    case EF_APPUI_BLOQUE : \
    { \
      txt = gettext ("Bloqué"); \
      BUGCRIT (appui->DATA##_donnees == NULL, \
               , \
               (gettext ("Le type d'appui de %s (%s) n'a pas à posséder de données.\n"), \
                         #DATA, \
                         gettext ("Bloqué")); ) \
      break; \
    } \
    default : \
    { \
      FAILINFO (, \
                (gettext("Le type d'appui de %s (%d) est inconnu.\n"), \
                         #DATA, \
                         appui->DATA); ) \
    } \
  } \
  \
  g_object_set (cell, "text", txt, NULL); \
}
EF_GTK_APPUIS_RENDER (1, ux);
EF_GTK_APPUIS_RENDER (2, uy);
EF_GTK_APPUIS_RENDER (3, uz);
EF_GTK_APPUIS_RENDER (4, rx);
EF_GTK_APPUIS_RENDER (5, ry);
EF_GTK_APPUIS_RENDER (6, rz);
/**
 * \def EF_GTK_APPUIS_RENDER(NUM, DATA)
 * \brief Crée la fonction affichant le type d'appui en fonction de la demande.
 * \param NUM : numéro de la colonne, conformément au fichier .ui,
 * \param DATA : information à afficher.
 */


/**
 * \brief Création de la fenêtre permettant d'afficher les appuis sous forme
 *        d'un tableau.
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique impossible à générer.
 */
void
EF_gtk_appuis (Projet *p)
{
  GList *list_parcours;
  
  BUGPARAM (p, "%p", p, )
  if (UI_APP.builder != NULL)
  {
    gtk_window_present (GTK_WINDOW (UI_APP.window));
    return;
  }
  
  UI_APP.builder = gtk_builder_new ();
  INFO (gtk_builder_add_from_resource (UI_APP.builder,
                                       "/org/2lgc/codegui/ui/EF_appuis.ui",
                                       NULL) != 0,
        ,
        (gettext ("La génération de la fenêtre %s a échouée.\n"),
                  "Appui"); )
  gtk_builder_connect_signals (UI_APP.builder, p);
  
  UI_APP.window = GTK_WIDGET (gtk_builder_get_object (UI_APP.builder,
                                                      "EF_appuis_window"));
  UI_APP.appuis = GTK_TREE_STORE (gtk_builder_get_object (UI_APP.builder,
                                                       "EF_appuis_treestore"));
  
  g_object_set (gtk_builder_get_object (UI_APP.builder,
                                        "EF_appuis_treeview_cell1"),
                "model", UI_APP.liste_type_appui,
                NULL);
  g_object_set (gtk_builder_get_object (UI_APP.builder,
                                        "EF_appuis_treeview_cell2"),
                "model", UI_APP.liste_type_appui,
                NULL);
  g_object_set (gtk_builder_get_object (UI_APP.builder,
                                        "EF_appuis_treeview_cell3"),
                "model", UI_APP.liste_type_appui,
                NULL);
  g_object_set (gtk_builder_get_object (UI_APP.builder,
                                        "EF_appuis_treeview_cell4"),
                "model", UI_APP.liste_type_appui,
                NULL);
  g_object_set (gtk_builder_get_object (UI_APP.builder,
                                        "EF_appuis_treeview_cell5"),
                "model", UI_APP.liste_type_appui,
                NULL);
  g_object_set (gtk_builder_get_object (UI_APP.builder,
                                        "EF_appuis_treeview_cell6"),
                "model", UI_APP.liste_type_appui,
                NULL);
  
  gtk_tree_view_column_set_cell_data_func (
    GTK_TREE_VIEW_COLUMN (gtk_builder_get_object (UI_APP.builder,
                                                "EF_appuis_treeview_column0")),
    GTK_CELL_RENDERER (gtk_builder_get_object (UI_APP.builder,
                                                  "EF_appuis_treeview_cell0")),
    EF_gtk_appuis_render_0,
    p,
    NULL);
  gtk_tree_view_column_set_cell_data_func (
    GTK_TREE_VIEW_COLUMN (gtk_builder_get_object (UI_APP.builder,
                                                "EF_appuis_treeview_column1")),
    GTK_CELL_RENDERER (gtk_builder_get_object (UI_APP.builder,
                                                  "EF_appuis_treeview_cell1")),
    EF_gtk_appuis_render_1,
    p,
    NULL);
  gtk_tree_view_column_set_cell_data_func (
    GTK_TREE_VIEW_COLUMN (gtk_builder_get_object (UI_APP.builder,
                                                "EF_appuis_treeview_column2")),
    GTK_CELL_RENDERER (gtk_builder_get_object (UI_APP.builder,
                                                  "EF_appuis_treeview_cell2")),
    EF_gtk_appuis_render_2,
    p,
    NULL);
  gtk_tree_view_column_set_cell_data_func (
    GTK_TREE_VIEW_COLUMN (gtk_builder_get_object (UI_APP.builder,
                                                "EF_appuis_treeview_column3")),
    GTK_CELL_RENDERER (gtk_builder_get_object (UI_APP.builder,
                                                  "EF_appuis_treeview_cell3")),
    EF_gtk_appuis_render_3,
    p,
    NULL);
  gtk_tree_view_column_set_cell_data_func (
    GTK_TREE_VIEW_COLUMN (gtk_builder_get_object (UI_APP.builder,
                                                "EF_appuis_treeview_column4")),
    GTK_CELL_RENDERER (gtk_builder_get_object (UI_APP.builder,
                                                  "EF_appuis_treeview_cell4")),
    EF_gtk_appuis_render_4,
    p,
    NULL);
  gtk_tree_view_column_set_cell_data_func (
    GTK_TREE_VIEW_COLUMN (gtk_builder_get_object (UI_APP.builder,
                                                "EF_appuis_treeview_column5")),
    GTK_CELL_RENDERER (gtk_builder_get_object (UI_APP.builder,
                                                  "EF_appuis_treeview_cell5")),
    EF_gtk_appuis_render_5,
    p,
    NULL);
  gtk_tree_view_column_set_cell_data_func (
    GTK_TREE_VIEW_COLUMN (gtk_builder_get_object (UI_APP.builder,
                                                "EF_appuis_treeview_column6")),
    GTK_CELL_RENDERER (gtk_builder_get_object (UI_APP.builder,
                                                  "EF_appuis_treeview_cell6")),
    EF_gtk_appuis_render_6,
    p,
    NULL);
  
  list_parcours = p->modele.appuis;
  while (list_parcours != NULL)
  {
    EF_Appui *appui = (EF_Appui *) list_parcours->data;
    
    gtk_tree_store_append (UI_APP.appuis, &appui->Iter_fenetre, NULL);
    gtk_tree_store_set (UI_APP.appuis, &appui->Iter_fenetre, 0, appui, -1);
    
    list_parcours = g_list_next (list_parcours);
  }
  
  g_object_set_data (gtk_builder_get_object (UI_APP.builder,
                                                   "EF_appuis_treeview_cell1"),
                     "column", GINT_TO_POINTER (1));
  g_object_set_data (gtk_builder_get_object (UI_APP.builder,
                                                   "EF_appuis_treeview_cell2"),
                     "column", GINT_TO_POINTER (2));
  g_object_set_data (gtk_builder_get_object (UI_APP.builder,
                                                   "EF_appuis_treeview_cell3"),
                     "column", GINT_TO_POINTER (3));
  g_object_set_data (gtk_builder_get_object (UI_APP.builder,
                                                   "EF_appuis_treeview_cell4"),
                     "column", GINT_TO_POINTER (4));
  g_object_set_data (gtk_builder_get_object (UI_APP.builder,
                                                   "EF_appuis_treeview_cell5"),
                     "column", GINT_TO_POINTER (5));
  g_object_set_data (gtk_builder_get_object (UI_APP.builder,
                                                   "EF_appuis_treeview_cell6"),
                     "column", GINT_TO_POINTER (6));
  
  gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (UI_APP.builder,
                                        "EF_appuis_boutton_supprimer_direct")),
                            FALSE);
  gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (UI_APP.builder,
                                          "EF_appuis_boutton_supprimer_menu")),
                            FALSE);
  
  gtk_window_set_transient_for (GTK_WINDOW (UI_APP.window),
                                GTK_WINDOW (UI_GTK.window));
}

#endif

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
