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
#include "common_text.h"
#include "EF_noeuds.h"
#include "EF_appuis.h"
#include "EF_calculs.h"
#include "1992_1_1_barres.h"


GTK_WINDOW_CLOSE (ef, noeud);


GTK_WINDOW_KEY_PRESS (ef, noeud);


GTK_WINDOW_DESTROY (ef, noeud, );


void
EF_gtk_noeud_ajouter (GtkButton *button,
                      Projet    *p)
/**
 * \brief Ajoute un nouveau noeud libre ou intermédiaire en fonction de
 *        l'onglet en cours d'affichage.
 * \param button : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
{
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_NOE.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Noeuds");)
  
  // On ajoute un noeud libre
  if (gtk_notebook_get_current_page (GTK_NOTEBOOK (UI_NOE.notebook)) == 0)
    BUG (EF_noeuds_ajout_noeud_libre (p,
                                      m_f (0., FLOTTANT_UTILISATEUR),
                                      m_f (0., FLOTTANT_UTILISATEUR),
                                      m_f (0., FLOTTANT_UTILISATEUR),
                                      NULL,
                                      NULL),
        )
  // On ajoute un noeud intermédiaire
  else
  {
    if (p->modele.barres == NULL)
      return;
    
    BUG (EF_noeuds_ajout_noeud_barre (p,
                                      (EF_Barre *) p->modele.barres->data,
                                      m_f (0.5, FLOTTANT_UTILISATEUR),
                                      NULL),
        )
  }
  
  return;
}


void
EF_gtk_noeud_supprimer (GtkButton *button,
                        Projet    *p)
/**
 * \brief Supprime le noeud sélectionné en fonction de l'onglet en cours
 *        d'affichage.
 * \param button : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
{
  GtkTreeModel *model;
  GtkTreeIter   Iter;
  EF_Noeud     *noeud;
  GList        *list = NULL;
  
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_NOE.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Noeuds");)
  
  // On supprimer un noeud libre
  if (gtk_notebook_get_current_page (GTK_NOTEBOOK (UI_NOE.notebook)) == 0)
  {
    if (!gtk_tree_selection_get_selected (GTK_TREE_SELECTION (
                                        gtk_builder_get_object (UI_NOE.builder,
                                   "EF_noeuds_treeview_noeuds_libres_select")),
                                          &model,
                                          &Iter))
      FAILCRIT ( , (gettext ("Aucun noeud n'est sélectionné.\n"));)
  }
  // On supprimer un noeud intermédiaire
  else
  {
    if (!gtk_tree_selection_get_selected (GTK_TREE_SELECTION (
                                        gtk_builder_get_object (UI_NOE.builder,
                           "EF_noeuds_treeview_noeuds_intermediaires_select")),
                                          &model,
                                          &Iter))
      FAILCRIT ( , (gettext ("Aucun noeud n'est sélectionné.\n"));)
  }
  
  gtk_tree_model_get (model, &Iter, 0, &noeud, -1);
  
  list = g_list_append (list, noeud);
  
  BUG (_1992_1_1_barres_supprime_liste (p, list, NULL),
       ,
       g_list_free (list);)
  
  g_list_free (list);
  
  BUG (m3d_rafraichit (p), )
  
  return;
}


gboolean
EF_gtk_noeud_treeview_key_press (GtkTreeView *treeview,
                                 GdkEvent    *event,
                                 Projet      *p)
/**
 * \brief Supprime un noeud sans dépendance si la touche SUPPR est appuyée.
 * \param treeview : composant à l'origine de l'évènement,
 * \param event : Caractéristique de l'évènement,
 * \param p : la variable projet.
 * \return
 *   Succès : TRUE si la touche SUPPR est pressée, FALSE sinon.
 *   Echec : FALSE :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
{
  BUGPARAMCRIT (p, "%p", p, FALSE)
  BUGCRIT (UI_NOE.builder,
           FALSE,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Noeuds");)
  
  if (event->key.keyval == GDK_KEY_Delete)
  {
    GtkTreeIter   Iter;
    GtkTreeModel *model;
    EF_Noeud     *noeud;
    GList        *liste_noeuds = NULL;
    
    if (!gtk_tree_selection_get_selected (gtk_tree_view_get_selection (
                                                                     treeview),
                                          &model,
                                          &Iter))
      return FALSE;
    
    gtk_tree_model_get (model, &Iter, 0, &noeud, -1);
    
    liste_noeuds = g_list_append (liste_noeuds, noeud);
    if (_1992_1_1_barres_cherche_dependances (p,
                                              NULL,
                                              liste_noeuds,
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
      BUG (_1992_1_1_barres_supprime_liste (p, liste_noeuds, NULL),
           FALSE,
           g_list_free (liste_noeuds);)
      BUG (m3d_rafraichit (p),
           FALSE,
           g_list_free (liste_noeuds);)
    }
    
    g_list_free (liste_noeuds);
    
    return TRUE;
  }
  else
    return FALSE;
}


void
EF_noeuds_set_supprimer_visible (gboolean select,
                                 Projet  *p)
/**
 * \brief En fonction de la sélection, active ou désactive les boutons
 *        supprimer.
 * \param select : TRUE si le changement survient via un changement de la
 *                 sélection, FALSE par un changement de page.
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
{
  GtkTreeModel *model;
  GtkTreeIter   Iter;
  EF_Noeud     *noeud;
    
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_NOE.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Noeud");)
  
  // Noeud libre
  if (((gtk_notebook_get_current_page (GTK_NOTEBOOK (UI_NOE.notebook)) == 0) &&
       (select == TRUE)) ||
      ((gtk_notebook_get_current_page (GTK_NOTEBOOK (UI_NOE.notebook)) == 1) &&
       (select == FALSE)))
  {
    // Aucune sélection
    if (!gtk_tree_selection_get_selected (GTK_TREE_SELECTION (
                                        gtk_builder_get_object (UI_NOE.builder,
                                   "EF_noeuds_treeview_noeuds_libres_select")),
                                          &model,
                                          &Iter))
      noeud = NULL;
    else
      gtk_tree_model_get (model, &Iter, 0, &noeud, -1);
  }
  // Noeud intermédiaire
  else
  {
    if (!gtk_tree_selection_get_selected (GTK_TREE_SELECTION (
                                        gtk_builder_get_object (UI_NOE.builder,
                           "EF_noeuds_treeview_noeuds_intermediaires_select")),
                                          &model,
                                          &Iter))
      noeud = NULL;
    else
      gtk_tree_model_get (model, &Iter, 0, &noeud, -1);
  }
  
  // Aucune sélection
  if (noeud == NULL)
  {
    gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
                               UI_NOE.builder, "EF_noeuds_boutton_supprimer")),
                              FALSE);
    gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
                          UI_NOE.builder, "EF_noeuds_boutton_supprimer_menu")),
                              FALSE);
    gtk_widget_set_visible (GTK_WIDGET (gtk_builder_get_object (UI_NOE.builder,
                                               "EF_noeuds_boutton_supprimer")),
                            FALSE);
    gtk_widget_set_visible (GTK_WIDGET (gtk_builder_get_object (UI_NOE.builder,
                                          "EF_noeuds_boutton_supprimer_menu")),
                            TRUE);
  }
  else
  {
    GList *liste_noeuds = NULL;
    
    liste_noeuds = g_list_append (liste_noeuds, noeud);
    if (_1992_1_1_barres_cherche_dependances (p,
                                              NULL,
                                              liste_noeuds,
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
                               UI_NOE.builder, "EF_noeuds_boutton_supprimer")),
                                FALSE);
      gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
                          UI_NOE.builder, "EF_noeuds_boutton_supprimer_menu")),
                                TRUE);
      gtk_widget_set_visible (GTK_WIDGET (gtk_builder_get_object (
                               UI_NOE.builder, "EF_noeuds_boutton_supprimer")),
                              FALSE);
      gtk_widget_set_visible (GTK_WIDGET (gtk_builder_get_object (
                          UI_NOE.builder, "EF_noeuds_boutton_supprimer_menu")),
                              TRUE);
    }
    // Noeud non utilisé
    else
    {
      gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
                               UI_NOE.builder, "EF_noeuds_boutton_supprimer")),
                                TRUE);
      gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
                          UI_NOE.builder, "EF_noeuds_boutton_supprimer_menu")),
                                FALSE);
      gtk_widget_set_visible (GTK_WIDGET (gtk_builder_get_object (
                               UI_NOE.builder, "EF_noeuds_boutton_supprimer")),
                              TRUE);
      gtk_widget_set_visible (GTK_WIDGET (gtk_builder_get_object (
                          UI_NOE.builder, "EF_noeuds_boutton_supprimer_menu")),
                              FALSE);
    }
    g_list_free (liste_noeuds);
  }
  
  return;
}


void
EF_gtk_noeuds_boutton_supprimer_menu (GtkButton *widget,
                                      Projet    *p)
/**
 * \brief Affiche la liste des dépendances dans le menu lorsqu'on clique sur le
 *        bouton Supprimer.
 * \param widget : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
{
  GtkTreeModel *model;
  GtkTreeIter   Iter;
  EF_Noeud     *noeud;
  GList        *liste_noeuds = NULL;
  GList        *liste_noeuds_dep, *liste_barres_dep, *liste_charges_dep;
    
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_NOE.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Noeud");)
  
  // Noeud libre
  if (gtk_notebook_get_current_page (GTK_NOTEBOOK (UI_NOE.notebook)) == 0)
  {
    // Aucune sélection
    if (!gtk_tree_selection_get_selected (GTK_TREE_SELECTION (
                                        gtk_builder_get_object (UI_NOE.builder,
                                   "EF_noeuds_treeview_noeuds_libres_select")),
                                          &model,
                                          &Iter))
      FAILCRIT ( , (gettext ("Aucun élément n'est sélectionné.\n"));)
    else
      gtk_tree_model_get (model, &Iter, 0, &noeud, -1);
  }
  // Noeud intermédiaire
  else
  {
    if (!gtk_tree_selection_get_selected (GTK_TREE_SELECTION (
                                        gtk_builder_get_object (UI_NOE.builder,
                           "EF_noeuds_treeview_noeuds_intermediaires_select")),
                                          &model,
                                          &Iter))
      FAILCRIT ( , (gettext ("Aucun élément n'est sélectionné.\n"));)
    else
      gtk_tree_model_get (model, &Iter, 0, &noeud, -1);
  }
  
  liste_noeuds = g_list_append (liste_noeuds, noeud);
  BUG (_1992_1_1_barres_cherche_dependances (p,
                                             NULL,
                                             liste_noeuds,
                                             NULL,
                                             NULL,
                                             NULL,
                                             NULL,
                                             &liste_noeuds_dep,
                                             &liste_barres_dep,
                                             &liste_charges_dep,
                                             FALSE,
                                             FALSE),
      ,
      g_list_free (liste_noeuds);)
  g_list_free (liste_noeuds);
  
  // Noeud utilisé
  if ((liste_noeuds_dep != NULL) ||
      (liste_barres_dep != NULL) ||
      (liste_charges_dep != NULL))
  {
    char *desc;
    
    BUG (desc = common_text_dependances (liste_noeuds_dep,
                                         liste_barres_dep,
                                         liste_charges_dep,
                                         p),
         ,
         g_list_free (liste_noeuds_dep);
           g_list_free (liste_barres_dep);
           g_list_free (liste_charges_dep);)
    gtk_menu_item_set_label (GTK_MENU_ITEM (gtk_builder_get_object (
                           UI_NOE.builder, "EF_noeuds_supprimer_menu_barres")),
                             desc);
    free (desc);
  }
  else
    FAILCRIT ( , (gettext ("L'élément ne possède aucune dépendance.\n"));)
  
  g_list_free (liste_noeuds_dep);
  g_list_free (liste_barres_dep);
  g_list_free (liste_charges_dep);
  
  return;
}


void
EF_noeuds_treeview_select_changed (GtkTreeSelection *treeselection,
                                   Projet           *p)
/**
 * \brief En fonction de la sélection, active ou désactive le bouton supprimer.
 * \param treeselection : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.
 */
{
  EF_noeuds_set_supprimer_visible (TRUE, p);
  
  return;
}


void
EF_gtk_noeuds_notebook_change (GtkNotebook *notebook,
                               GtkWidget   *page,
                               guint        page_num,
                               Projet      *p)
/* \brief Le changement de la page en cours nécessite l'actualisation de la
 *        disponibilité du bouton supprimer.
 * \param notebook : le composant notebook,
 * \param page : page affichée,
 * \param page_num : numéro de la page,
 * \param p : la variable projet.
 * \return Rien.
 */
{
  EF_noeuds_set_supprimer_visible (FALSE, p);
  
  return;
}


void
EF_gtk_noeud_edit_pos_abs (GtkCellRendererText *cell,
                           gchar               *path_string,
                           gchar               *new_text,
                           Projet              *p)
/* \brief Changement de la position absolue d'un noeud.
 * \param cell : cellule en cours,
 * \param path_string : path de la ligne en cours,
 * \param new_text : nouvelle valeur,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - new_text == NULL,
 *     - interface graphique non initialisée,
 *     - liste des noeuds vide.
 */
{
  GtkTreeModel *model;
  GtkTreePath  *path;
  GtkTreeIter   iter;
  double        conversion;
  gint          column;
  
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_NOE.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Noeuds");)
  BUGCRIT (p->modele.noeuds, , (gettext ("Aucun noeud n'est existant.\n"));)
  BUGPARAMCRIT (new_text, "%p", new_text, )
  
  column = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (cell), "column"));
  model = GTK_TREE_MODEL (UI_NOE.tree_store_libre);
  path = gtk_tree_path_new_from_string (path_string);
  
  gtk_tree_model_get_iter (model, &iter, path);
  gtk_tree_path_free (path);
  
  // On vérifie si le texte contient bien un nombre flottant
  conversion = common_text_str_to_double (new_text,
                                          -INFINITY,
                                          FALSE,
                                          INFINITY,
                                          FALSE);
  
  if (!isnan (conversion))
  {
    EF_Noeud *noeud;
    
    // On modifie l'action
    gtk_tree_model_get (model, &iter, 0, &noeud, -1);
    
    switch (column)
    {
      case 1:
      {
        BUG (EF_noeuds_change_pos_abs (p,
                                       noeud,
                                       m_f (conversion, FLOTTANT_UTILISATEUR),
                                       m_f (NAN, FLOTTANT_UTILISATEUR),
                                       m_f (NAN, FLOTTANT_UTILISATEUR)),
            )
        break;
      }
      case 2:
      {
        BUG (EF_noeuds_change_pos_abs (p,
                                       noeud,
                                       m_f (NAN, FLOTTANT_UTILISATEUR),
                                       m_f (conversion, FLOTTANT_UTILISATEUR),
                                       m_f (NAN, FLOTTANT_UTILISATEUR)),
            )
        break;
      }
      case 3:
      {
        BUG (EF_noeuds_change_pos_abs (p,
                                       noeud,
                                       m_f (NAN, FLOTTANT_UTILISATEUR),
                                       m_f (NAN, FLOTTANT_UTILISATEUR),
                                       m_f (conversion, FLOTTANT_UTILISATEUR)),
            )
        break;
      }
      default :
      {
        FAILCRIT ( ,
                  (gettext ("La colonne d'où provient l'édition est incorrecte.\n"));)
        break;
      }
    }
  }
  
  return;
}


void
EF_gtk_noeud_edit_pos_relat (GtkCellRendererText *cell,
                             gchar               *path_string,
                             gchar               *new_text,
                             Projet              *p)
/* \brief Changement de la position relative d'un noeud de type barre.
 * \param cell : cellule en cours,
 * \param path_string : path de la ligne en cours,
 * \param new_text : nouvelle valeur,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - new_text == NULL,
 *     - interface graphique non initialisée,
 *     - liste des noeuds vide.
 */
{
  GtkTreeModel *model;
  GtkTreePath  *path;
  GtkTreeIter   iter;
  double        conversion;
  gint          column;
  
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_NOE.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Noeuds");)
  BUGCRIT (p->modele.noeuds, , (gettext ("Aucun noeud n'est existant.\n"));)
  BUGPARAMCRIT (new_text, "%p", new_text, )
  
  column = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (cell), "column"));
  model = GTK_TREE_MODEL (UI_NOE.tree_store_barre);
  path = gtk_tree_path_new_from_string (path_string);
  
  gtk_tree_model_get_iter (model, &iter, path);
  gtk_tree_path_free (path);
  
  // On vérifie si le texte contient bien un nombre flottant
  conversion = common_text_str_to_double (new_text, 0, TRUE, 1., TRUE);
  if (!isnan (conversion))
  {
    EF_Noeud *noeud;
    
    gtk_tree_model_get (model, &iter, 0, &noeud, -1);
    
    if ((noeud->type == NOEUD_BARRE) && (column == 6))
      BUG (EF_noeuds_change_pos_relat (p,
                                       noeud,
                                       m_f (conversion, FLOTTANT_UTILISATEUR)),
          )
    else
      FAILCRIT ( ,
                (gettext ("Le type du noeud ou la colonne d'édition est incorrect.\n"));)
  }
  
  return;
}


void
EF_gtk_noeud_edit_noeud_relatif (GtkCellRendererText *cell,
                                 gchar               *path_string,
                                 gchar               *new_text,
                                 Projet              *p)
/**
 * \brief Changement du noeud relatif d'un noeud.
 * \param cell : cellule en cours,
 * \param path_string : path de la ligne en cours,
 * \param new_text : numero du noeud,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - new_text == NULL,
 *     - interface graphique non initialisée,
 *     - liste des noeuds vide,
 *     - en cas d'erreur d'allocation mémoire.
 */
{
  GtkTreeModel *model;
  GtkTreePath  *path;
  GtkTreeIter   iter;
  char         *fake;
  unsigned int  conversion;
  EF_Noeud     *noeud;
  
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_NOE.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Noeuds");)
  BUGCRIT (p->modele.noeuds, , (gettext ("Aucun noeud n'est existant.\n"));)
  BUGPARAMCRIT (new_text, "%p", new_text, )
  
  BUGCRIT (fake = (char *) malloc (sizeof (char) * (strlen (new_text) + 1)),
           ,
           (gettext ("Erreur d'allocation mémoire.\n"));)
  
  model = GTK_TREE_MODEL (UI_NOE.tree_store_libre);
  path = gtk_tree_path_new_from_string (path_string);
  
  gtk_tree_model_get_iter (model, &iter, path);
  gtk_tree_path_free (path);
  gtk_tree_model_get (model, &iter, 0, &noeud, -1);
  
  // On vérifie si le texte contient bien un nombre flottant
  if (strcmp (new_text, "") == 0)
    BUG (EF_noeuds_change_noeud_relatif (p, noeud, NULL),
         ,
         free (fake);)
  else if (sscanf (new_text, "%u%s", &conversion, fake) == 1)
  {
    EF_Noeud *noeud2;
    
    BUG (noeud2 = EF_noeuds_cherche_numero (p, conversion, TRUE),
         ,
         free (fake);)
    
    BUG (EF_noeuds_change_noeud_relatif (p, noeud, noeud2),
         ,
         free (fake);)
  }
  
  free (fake);
   
  return;
}


void
EF_gtk_noeud_edit_noeud_appui (GtkCellRendererText *cell,
                               const gchar         *path_string,
                               const gchar         *new_text,
                               Projet              *p)
/**
 * \brief Changement du type d'appui d'un noeud.
 * \param cell : cellule en cours,
 * \param path_string : path de la ligne en cours,
 * \param new_text : nom de l'appui,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - new_text == NULL,
 *     - interface graphique non initialisée,
 *     - liste des noeuds vide.
*/
{
  GtkTreeModel *model;
  GtkTreePath  *path;
  GtkTreeIter   iter;
  EF_Noeud     *noeud;
  
  BUGPARAMCRIT (p, "%p", p, )
  BUGPARAMCRIT (new_text, "%p", new_text, )
  BUGCRIT (UI_NOE.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Noeuds");)
  BUGCRIT (p->modele.noeuds, , (gettext ("Aucun noeud n'est existant.\n"));)
  
  if (p->modele.appuis == NULL)
    return;
  
  // On récupère le model du treeview en cours d'édition.
  if (gtk_notebook_get_current_page (GTK_NOTEBOOK (UI_NOE.notebook)) == 0)
    model = GTK_TREE_MODEL (UI_NOE.tree_store_libre);
  else
    model = GTK_TREE_MODEL (UI_NOE.tree_store_barre);
  path = gtk_tree_path_new_from_string (path_string);
  
  gtk_tree_model_get_iter (model, &iter, path);
  gtk_tree_model_get (model, &iter, 0, &noeud, -1);
  gtk_tree_path_free (path);
  
  // Si on souhaite que l'appui ne soit plus appuyé.
  if (strcmp (new_text, gettext ("Aucun")) == 0)
    BUG (EF_noeuds_change_appui (p, noeud, NULL), )
  else
  {
    EF_Appui *appui;
    
    BUG (appui = EF_appuis_cherche_nom (p, new_text, TRUE), )
    BUG (EF_noeuds_change_appui (p, noeud, appui), )
  }
  
  BUG (EF_calculs_free (p), )
  
  return;
}


void
EF_gtk_noeud_edit_noeud_barre_barre (GtkCellRendererText *cell,
                                     const gchar         *path_string,
                                     const gchar         *new_text,
                                     Projet              *p)
/**
 * \brief Changement de barre d'un noeud intermédiaire.
 * \param cell : cellule en cours,
 * \param path_string : path de la ligne en cours,
 * \param new_text : nom de l'appui,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - new_text == NULL,
 *     - interface graphique non initialisée,
 *     - liste des noeuds vide,
 *     - en cas d'erreur d'allocation mémoire.
*/
{
  char         *fake;
  unsigned int  conversion;
  GtkTreeModel *model;
  GtkTreePath  *path;
  GtkTreeIter   iter;
  
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_NOE.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Noeuds");)
  BUGCRIT (p->modele.noeuds, , (gettext ("Aucun noeud n'est existant.\n"));)
  BUGPARAMCRIT (new_text, "%p", new_text, )
  BUGCRIT (fake = (char *) malloc (sizeof (char) * (strlen (new_text) + 1)),
           ,
           (gettext ("Erreur d'allocation mémoire.\n"));)
  
  model = GTK_TREE_MODEL (UI_NOE.tree_store_barre);
  path = gtk_tree_path_new_from_string (path_string);
  
  gtk_tree_model_get_iter (model, &iter, path);
  gtk_tree_path_free (path);
  
  // On vérifie si le texte contient bien un numéro
  if (sscanf (new_text, "%u%s", &conversion, fake) == 1)
  {
    EF_Barre *barre;
    EF_Noeud *noeud;
    GList    *liste_noeuds = NULL;
    
    // On modifie l'action
    gtk_tree_model_get (model, &iter, 0, &noeud, -1);
    BUG (barre = _1992_1_1_barres_cherche_numero (p, conversion, TRUE),
         ,
         free (fake);)
    
    if (noeud->type == NOEUD_BARRE)
    {
      EF_Noeud_Barre *info = (EF_Noeud_Barre *) noeud->data;
      GList          *liste;
      
      BUG (EF_calculs_free (p), , free (fake);)
      
      info->barre->discretisation_element--;
      info->barre->nds_inter = g_list_remove (info->barre->nds_inter, noeud);
      BUGCRIT (info->barre->info_EF = realloc (
        info->barre->info_EF,
        sizeof (Barre_Info_EF) * (info->barre->discretisation_element + 1)),
              ,
              (gettext ("Erreur d'allocation mémoire.\n"));
                free (fake);)
      info->barre = barre;
      info->barre->discretisation_element++;
      BUGCRIT (info->barre->info_EF = realloc(
        info->barre->info_EF,
        sizeof (Barre_Info_EF) * (info->barre->discretisation_element + 1)),
              ,
              (gettext ("Erreur d'allocation mémoire.\n"));
                free (fake);)
      memset (barre->info_EF,
              0,
              sizeof (Barre_Info_EF) *
                                    (info->barre->discretisation_element + 1));
      
      liste = info->barre->nds_inter;
      while ((liste != NULL) &&
             (m_g (((EF_Noeud_Barre *) liste->data)->position_relative_barre) <
                                          m_g (info->position_relative_barre)))
        liste = g_list_next (liste);
      
      info->barre->nds_inter = g_list_insert_before (info->barre->nds_inter,
                                                     liste,
                                                     noeud);
    }
    else
      FAILCRIT ( ,
                (gettext ("Le noeud doit être de type intermédiaire.\n"));
                  free (fake);)
    
    liste_noeuds = g_list_append (liste_noeuds, noeud);
    
    BUG (m3d_actualise_graphique (p, liste_noeuds, NULL),
         ,
         free (fake);
           g_list_free (liste_noeuds);)
    BUG (m3d_rafraichit (p),
         ,
         free (fake);
           g_list_free (liste_noeuds);)
    
    g_list_free (liste_noeuds);
  }
  
  free (fake);
  
  return;
}


void
EF_gtk_noeuds_render_0 (GtkTreeViewColumn *tree_column,
                        GtkCellRenderer   *cell,
                        GtkTreeModel      *tree_model,
                        GtkTreeIter       *iter,
                        gpointer           data2)
/**
 * \brief Affiche le numéro des noeuds dans les deux treeviews noeuds libre et
 *        intermédiaire.
 * \param tree_column : composant à l'origine de l'évènement,
 * \param cell : la cellule en cours d'édition,
 * \param tree_model : le mode en cours d'édition,
 * \param iter : la ligne en cours d'édition,
 * \param data2 : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - en cas d'erreur d'allocation mémoire.
 */
{
  EF_Noeud *noeud;
  char     *tmp;
  
  gtk_tree_model_get (tree_model, iter, 0, &noeud, -1);
  BUGPARAM (noeud, "%p", noeud, )
  BUGCRIT (tmp = g_strdup_printf ("%d", noeud->numero),
           ,
           (gettext ("Erreur d'allocation mémoire.\n"));)
  
  g_object_set (cell, "text", tmp, NULL);
  
  free (tmp);
}


void
EF_gtk_noeuds_render_1 (GtkTreeViewColumn *tree_column,
                        GtkCellRenderer   *cell,
                        GtkTreeModel      *tree_model,
                        GtkTreeIter       *iter,
                        gpointer           data2)
/**
 * \brief Affiche la coordonnée x des noeuds dans les deux treeviews noeuds
 *        libre et intermédiaire.
 * \param tree_column : composant à l'origine de l'évènement,
 * \param cell : la cellule en cours d'édition,
 * \param tree_model : le mode en cours d'édition,
 * \param iter : la ligne en cours d'édition,
 * \param data2 : la variable projet.
 * \return Rien.
 */
{
  EF_Noeud       *noeud;
  EF_Point        point;
  char            tmp[30];
  EF_Noeud_Libre *data;
  
  gtk_tree_model_get (tree_model, iter, 0, &noeud, -1);
  BUGPARAM (noeud, "%p", noeud, )
  
  data = noeud->data;
  if ((noeud->type == NOEUD_LIBRE) && (data->relatif != NULL))
    conv_f_c (data->x, tmp, DECIMAL_DISTANCE);
  else
  {
    BUG (EF_noeuds_renvoie_position (noeud, &point), )
    conv_f_c (point.x, tmp, DECIMAL_DISTANCE);
  }
  
  g_object_set (cell, "text", tmp, NULL);
}


void
EF_gtk_noeuds_render_2 (GtkTreeViewColumn *tree_column,
                        GtkCellRenderer   *cell,
                        GtkTreeModel      *tree_model,
                        GtkTreeIter       *iter,
                        gpointer           data2)
/**
 * \brief Affiche la coordonnée y des noeuds dans les deux treeviews noeuds
 *        libre et intermédiaire.
 * \param tree_column : composant à l'origine de l'évènement,
 * \param cell : la cellule en cours d'édition,
 * \param tree_model : le mode en cours d'édition,
 * \param iter : la ligne en cours d'édition,
 * \param data2 : la variable projet.
 * \return Rien.
 */
{
  EF_Noeud       *noeud;
  EF_Point        point;
  char            tmp[30];
  EF_Noeud_Libre *data;
  
  gtk_tree_model_get (tree_model, iter, 0, &noeud, -1);
  BUGPARAM (noeud, "%p", noeud, )
  
  data = noeud->data;
  if ((noeud->type == NOEUD_LIBRE) && (data->relatif != NULL))
    conv_f_c (data->y, tmp, DECIMAL_DISTANCE);
  else
  {
    BUG (EF_noeuds_renvoie_position (noeud, &point), )
    conv_f_c (point.y, tmp, DECIMAL_DISTANCE);
  }
  
  g_object_set (cell, "text", tmp, NULL);
}


void
EF_gtk_noeuds_render_3 (GtkTreeViewColumn *tree_column,
                        GtkCellRenderer   *cell,
                        GtkTreeModel      *tree_model,
                        GtkTreeIter       *iter,
                        gpointer           data2)
/**
 * \brief Affiche la coordonnée z des noeuds dans les deux treeviews noeuds
 *        libre et intermédiaire.
 * \param tree_column : composant à l'origine de l'évènement,
 * \param cell : la cellule en cours d'édition,
 * \param tree_model : le mode en cours d'édition,
 * \param iter : la ligne en cours d'édition,
 * \param data2 : la variable projet.
 * \return Rien.
 */
{
  EF_Noeud       *noeud;
  EF_Point        point;
  char            tmp[30];
  EF_Noeud_Libre *data;
  
  gtk_tree_model_get (tree_model, iter, 0, &noeud, -1);
  BUGPARAM (noeud, "%p", noeud, )
  
  data = noeud->data;
  if ((noeud->type == NOEUD_LIBRE) && (data->relatif != NULL))
    conv_f_c (data->z, tmp, DECIMAL_DISTANCE);
  else
  {
    BUG (EF_noeuds_renvoie_position (noeud, &point), )
    conv_f_c (point.z, tmp, DECIMAL_DISTANCE);
  }
  
  g_object_set (cell, "text", tmp, NULL);
}


void
EF_gtk_noeuds_render_4 (GtkTreeViewColumn *tree_column,
                        GtkCellRenderer   *cell,
                        GtkTreeModel      *tree_model,
                        GtkTreeIter       *iter,
                        gpointer           data2)
/**
 * \brief Affiche l'appui des noeuds dans les deux treeviews noeuds libre et
 *        ntermédiaire.
 * \param tree_column : composant à l'origine de l'évènement,
 * \param cell : la cellule en cours d'édition,
 * \param tree_model : le mode en cours d'édition,
 * \param iter : la ligne en cours d'édition,
 * \param data2 : la variable projet.
 * \return Rien.
 */
{
  EF_Noeud *noeud;
  
  gtk_tree_model_get (tree_model, iter, 0, &noeud, -1);
  BUGPARAM (noeud, "%p", noeud, )
  
  g_object_set (cell,
                "text",
                noeud->appui == NULL ? gettext ("Aucun") : noeud->appui->nom,
                NULL);
}


void
EF_gtk_noeuds_render_libre5 (GtkTreeViewColumn *tree_column,
                             GtkCellRenderer   *cell,
                             GtkTreeModel      *tree_model,
                             GtkTreeIter       *iter,
                             gpointer           data2)
/**
 * \brief Affiche le noeud relatif des noeuds dans le treeview noeud libre.
 * \param tree_column : composant à l'origine de l'évènement,
 * \param cell : la cellule en cours d'édition,
 * \param tree_model : le mode en cours d'édition,
 * \param iter : la ligne en cours d'édition,
 * \param data2 : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - en cas d'erreur d'allocation mémoire.
 */
{
  EF_Noeud       *noeud;
  EF_Noeud_Libre *data;
  char           *tmp = NULL;
  
  gtk_tree_model_get (tree_model, iter, 0, &noeud, -1);
  BUGPARAM (noeud, "%p", noeud, )
  data = noeud->data;
  if (data->relatif != NULL)
    BUGCRIT (tmp = g_strdup_printf ("%d", data->relatif->numero),
             ,
             (gettext ("Erreur d'allocation mémoire.\n"));)
  
  g_object_set (cell, "text", tmp == NULL ? gettext ("Aucun") : tmp, NULL);
  
  free (tmp);
}


void
EF_gtk_noeuds_render_intermediaire5 (GtkTreeViewColumn *tree_column,
                                     GtkCellRenderer   *cell,
                                     GtkTreeModel      *tree_model,
                                     GtkTreeIter       *iter,
                                     gpointer           data2)
/**
 * \brief Affiche la barre où est le noeud dans le treeview noeud
 *        intermédiaire.
 * \param tree_column : composant à l'origine de l'évènement,
 * \param cell : la cellule en cours d'édition,
 * \param tree_model : le mode en cours d'édition,
 * \param iter : la ligne en cours d'édition,
 * \param data2 : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - en cas d'erreur d'allocation mémoire.
 */
{
  EF_Noeud       *noeud;
  EF_Noeud_Barre *data;
  char           *tmp = NULL;
  
  gtk_tree_model_get (tree_model, iter, 0, &noeud, -1);
  BUGPARAM (noeud, "%p", noeud, )
  data = noeud->data;
  BUGCRIT (tmp = g_strdup_printf ("%d", data->barre->numero),
           ,
           (gettext ("Erreur d'allocation mémoire.\n"));)
  
  g_object_set (cell, "text", tmp, NULL);
  
  free (tmp);
}


void
EF_gtk_noeuds_render_intermediaire6 (GtkTreeViewColumn *tree_column,
                                     GtkCellRenderer   *cell,
                                     GtkTreeModel      *tree_model,
                                     GtkTreeIter       *iter,
                                     gpointer           data2)
/**
 * \brief Affiche la position relative du noeud dans la barre dans le treeview
 *        noeud intermédiaire.
 * \param tree_column : composant à l'origine de l'évènement,
 * \param cell : la cellule en cours d'édition,
 * \param tree_model : le mode en cours d'édition,
 * \param iter : la ligne en cours d'édition,
 * \param data2 : la variable projet.
 * \return Rien.
 */
{
  EF_Noeud       *noeud;
  EF_Noeud_Barre *data;
  char            tmp[30];
  
  gtk_tree_model_get (tree_model, iter, 0, &noeud, -1);
  BUGPARAM (noeud, "%p", noeud, )
  data = noeud->data;
  conv_f_c (data->position_relative_barre, tmp, DECIMAL_DISTANCE);
  
  g_object_set (cell, "text", tmp, NULL);
}


void
EF_gtk_noeud (Projet *p)
/**
 * \brief Affichage de la fenêtre permettant de créer ou modifier des noeuds.
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique impossible à générer.
 */
{
  GList *list_parcours = p->modele.noeuds;
  
  BUGPARAM (p, "%p", p, )
  
  if (UI_NOE.builder != NULL)
  {
    gtk_window_present (GTK_WINDOW (UI_NOE.window));
    return;
  }
  
  UI_NOE.builder = gtk_builder_new ();
  BUGCRIT (gtk_builder_add_from_resource (UI_NOE.builder,
                                          "/org/2lgc/codegui/ui/EF_noeud.ui",
                                          NULL) != 0,
           ,
           (gettext ("La génération de la fenêtre %s a échouée.\n"),
                     "Noeuds");)
  gtk_builder_connect_signals (UI_NOE.builder, p);
  
  UI_NOE.window = GTK_WIDGET (gtk_builder_get_object (UI_NOE.builder,
                                                      "EF_noeuds_window"));
  UI_NOE.notebook = GTK_WIDGET (gtk_builder_get_object (UI_NOE.builder,
                                                        "EF_noeuds_notebook"));
  UI_NOE.tree_store_libre = GTK_TREE_STORE (gtk_builder_get_object (
                         UI_NOE.builder, "EF_noeuds_treestore_noeuds_libres"));
  UI_NOE.tree_store_barre = GTK_TREE_STORE (gtk_builder_get_object (
                 UI_NOE.builder, "EF_noeuds_treestore_noeuds_intermediaires"));
  
  gtk_tree_view_column_set_cell_data_func (
    GTK_TREE_VIEW_COLUMN (gtk_builder_get_object (UI_NOE.builder,
                                  "EF_noeuds_treeview_noeuds_libres_column0")),
    GTK_CELL_RENDERER (gtk_builder_get_object (UI_NOE.builder,
                                    "EF_noeuds_treeview_noeuds_libres_cell0")),
    EF_gtk_noeuds_render_0,
    p,
    NULL);
  gtk_tree_view_column_set_cell_data_func (
    GTK_TREE_VIEW_COLUMN (gtk_builder_get_object (UI_NOE.builder,
                                  "EF_noeuds_treeview_noeuds_libres_column1")),
    GTK_CELL_RENDERER (gtk_builder_get_object (UI_NOE.builder,
                                    "EF_noeuds_treeview_noeuds_libres_cell1")),
    EF_gtk_noeuds_render_1,
    p,
    NULL);
  gtk_tree_view_column_set_cell_data_func (
    GTK_TREE_VIEW_COLUMN (gtk_builder_get_object (UI_NOE.builder,
                                  "EF_noeuds_treeview_noeuds_libres_column2")),
    GTK_CELL_RENDERER (gtk_builder_get_object (UI_NOE.builder,
                                    "EF_noeuds_treeview_noeuds_libres_cell2")),
    EF_gtk_noeuds_render_2,
    p,
    NULL);
  gtk_tree_view_column_set_cell_data_func (
    GTK_TREE_VIEW_COLUMN (gtk_builder_get_object (UI_NOE.builder,
                                  "EF_noeuds_treeview_noeuds_libres_column3")),
    GTK_CELL_RENDERER (gtk_builder_get_object (UI_NOE.builder,
                                    "EF_noeuds_treeview_noeuds_libres_cell3")),
    EF_gtk_noeuds_render_3,
    p,
    NULL);
  gtk_tree_view_column_set_cell_data_func (
    GTK_TREE_VIEW_COLUMN (gtk_builder_get_object (UI_NOE.builder,
                                  "EF_noeuds_treeview_noeuds_libres_column4")),
    GTK_CELL_RENDERER (gtk_builder_get_object (UI_NOE.builder,
                                    "EF_noeuds_treeview_noeuds_libres_cell4")),
    EF_gtk_noeuds_render_4,
    p,
    NULL);
  gtk_tree_view_column_set_cell_data_func (
    GTK_TREE_VIEW_COLUMN (gtk_builder_get_object (UI_NOE.builder,
                                  "EF_noeuds_treeview_noeuds_libres_column5")),
    GTK_CELL_RENDERER (gtk_builder_get_object (UI_NOE.builder,
                                    "EF_noeuds_treeview_noeuds_libres_cell5")),
    EF_gtk_noeuds_render_libre5,
    p,
    NULL);
  g_object_set_data (gtk_builder_get_object (UI_NOE.builder,
                                     "EF_noeuds_treeview_noeuds_libres_cell1"),
                     "column",
                     GINT_TO_POINTER (1));
  g_object_set_data (gtk_builder_get_object (UI_NOE.builder,
                                     "EF_noeuds_treeview_noeuds_libres_cell2"),
                     "column",
                     GINT_TO_POINTER (2));
  g_object_set_data (gtk_builder_get_object (UI_NOE.builder,
                                     "EF_noeuds_treeview_noeuds_libres_cell3"),
                     "column",
                     GINT_TO_POINTER (3));
  
  g_object_set (gtk_builder_get_object (UI_NOE.builder,
                                     "EF_noeuds_treeview_noeuds_libres_cell4"),
                "model",
                UI_APP.liste_appuis,
                NULL);
  
  gtk_tree_view_column_set_cell_data_func (
    GTK_TREE_VIEW_COLUMN (gtk_builder_get_object (UI_NOE.builder,
                          "EF_noeuds_treeview_noeuds_intermediaires_column0")),
    GTK_CELL_RENDERER (gtk_builder_get_object (UI_NOE.builder,
                            "EF_noeuds_treeview_noeuds_intermediaires_cell0")),
    EF_gtk_noeuds_render_0,
    p,
    NULL);
  gtk_tree_view_column_set_cell_data_func (
    GTK_TREE_VIEW_COLUMN (gtk_builder_get_object (UI_NOE.builder,
                          "EF_noeuds_treeview_noeuds_intermediaires_column1")),
    GTK_CELL_RENDERER (gtk_builder_get_object (UI_NOE.builder,
                            "EF_noeuds_treeview_noeuds_intermediaires_cell1")),
    EF_gtk_noeuds_render_1,
    p,
    NULL);
  gtk_tree_view_column_set_cell_data_func (
    GTK_TREE_VIEW_COLUMN (gtk_builder_get_object (UI_NOE.builder,
                          "EF_noeuds_treeview_noeuds_intermediaires_column2")),
    GTK_CELL_RENDERER (gtk_builder_get_object (UI_NOE.builder,
                            "EF_noeuds_treeview_noeuds_intermediaires_cell2")),
    EF_gtk_noeuds_render_2,
    p,
    NULL);
  gtk_tree_view_column_set_cell_data_func (
    GTK_TREE_VIEW_COLUMN (gtk_builder_get_object (UI_NOE.builder,
                          "EF_noeuds_treeview_noeuds_intermediaires_column3")),
    GTK_CELL_RENDERER (gtk_builder_get_object (UI_NOE.builder,
                            "EF_noeuds_treeview_noeuds_intermediaires_cell3")),
    EF_gtk_noeuds_render_3,
    p,
    NULL);
  gtk_tree_view_column_set_cell_data_func (
    GTK_TREE_VIEW_COLUMN (gtk_builder_get_object (UI_NOE.builder,
                          "EF_noeuds_treeview_noeuds_intermediaires_column4")),
    GTK_CELL_RENDERER (gtk_builder_get_object (UI_NOE.builder,
                            "EF_noeuds_treeview_noeuds_intermediaires_cell4")),
    EF_gtk_noeuds_render_4,
    p,
    NULL);
  gtk_tree_view_column_set_cell_data_func (
    GTK_TREE_VIEW_COLUMN (gtk_builder_get_object (UI_NOE.builder,
                          "EF_noeuds_treeview_noeuds_intermediaires_column5")),
    GTK_CELL_RENDERER (gtk_builder_get_object (UI_NOE.builder,
                            "EF_noeuds_treeview_noeuds_intermediaires_cell5")),
    EF_gtk_noeuds_render_intermediaire5,
    p,
    NULL);
  gtk_tree_view_column_set_cell_data_func (
    GTK_TREE_VIEW_COLUMN (gtk_builder_get_object (UI_NOE.builder,
                          "EF_noeuds_treeview_noeuds_intermediaires_column6")),
    GTK_CELL_RENDERER (gtk_builder_get_object (UI_NOE.builder,
                            "EF_noeuds_treeview_noeuds_intermediaires_cell6")),
    EF_gtk_noeuds_render_intermediaire6,
    p,
    NULL);
  g_object_set_data (gtk_builder_get_object (UI_NOE.builder,
                             "EF_noeuds_treeview_noeuds_intermediaires_cell1"),
                     "column",
                     GINT_TO_POINTER (1));
  g_object_set_data (gtk_builder_get_object (UI_NOE.builder,
                             "EF_noeuds_treeview_noeuds_intermediaires_cell2"),
                     "column",
                     GINT_TO_POINTER (2));
  g_object_set_data (gtk_builder_get_object (UI_NOE.builder,
                             "EF_noeuds_treeview_noeuds_intermediaires_cell3"),
                     "column",
                     GINT_TO_POINTER (3));
  g_object_set_data (gtk_builder_get_object (UI_NOE.builder,
                             "EF_noeuds_treeview_noeuds_intermediaires_cell6"),
                     "column",
                     GINT_TO_POINTER (6));
  
  g_object_set (gtk_builder_get_object (UI_NOE.builder,
                             "EF_noeuds_treeview_noeuds_intermediaires_cell4"),
                "model",
                UI_APP.liste_appuis,
                NULL);
  
  while (list_parcours != NULL)
  {
    EF_Noeud *noeud = (EF_Noeud *) list_parcours->data;
    
    if (noeud->type == NOEUD_LIBRE)
    {
      gtk_tree_store_append (UI_NOE.tree_store_libre, &noeud->Iter, NULL);
      gtk_tree_store_set (UI_NOE.tree_store_libre,
                          &noeud->Iter,
                          0, noeud,
                          -1);
    }
    else if (noeud->type == NOEUD_BARRE)
    {
      gtk_tree_store_append (UI_NOE.tree_store_barre, &noeud->Iter, NULL);
      gtk_tree_store_set (UI_NOE.tree_store_barre,
                          &noeud->Iter,
                          0, noeud,
                          -1);
      
    }
    
    list_parcours = g_list_next (list_parcours);
  }
  
  gtk_window_set_transient_for (GTK_WINDOW (UI_NOE.window),
                                GTK_WINDOW (UI_GTK.window));
}

#endif

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
