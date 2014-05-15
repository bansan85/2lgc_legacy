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
#include "common_math.hpp"
#include "common_erreurs.hpp"
#include "common_gtk.hpp"
#include "common_selection.hpp"
#include "common_text.hpp"
#include "EF_materiaux.hpp"
#include "EF_noeuds.hpp"
#include "EF_relachement.hpp"
#include "EF_sections.hpp"
#include "1992_1_1_barres.hpp"
#include "1992_1_1_materiaux.hpp"
#include "EF_gtk_barres.hpp"


GTK_WINDOW_CLOSE (ef, barres);


GTK_WINDOW_DESTROY (ef, barres, );


GTK_WINDOW_KEY_PRESS (ef, barres);


/**
 * \brief Changement du type d'une barre.
 * \param cell : cellule en cours,
 * \param path_string : path de la ligne en cours,
 * \param new_text : nom du type de barre,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée,
 *     - liste des types de barre est indéfinie.
 */
extern "C"
void
EF_gtk_barres_edit_type (GtkCellRendererText *cell,
                         const gchar         *path_string,
                         const gchar         *new_text,
                         Projet              *p)
{
  GtkTreeModel *model;
  GtkTreeIter   iter, iter2;
  EF_Barre     *barre = NULL;
  char         *nom_type;
  
  BUGPARAM (p, "%p", p, )
  BUGCRIT (UI_BAR.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Barres"); )
  BUGCRIT (UI_BAR.liste_types,
           ,
           (gettext ("La liste des types de barre est indéfinie.\n")); )
  
  model = GTK_TREE_MODEL (gtk_builder_get_object (UI_BAR.builder,
                                                  "EF_barres_treestore"));
  
  gtk_tree_model_get_iter_from_string (model, &iter, path_string);
  gtk_tree_model_get (model, &iter, 0, &barre, -1);
  
  BUGCRIT (gtk_tree_model_get_iter_first (GTK_TREE_MODEL (UI_BAR.liste_types),
                                          &iter2),
           ,
           (gettext ("Aucun type de barre n'est défini.\n")); )
  gtk_tree_model_get (GTK_TREE_MODEL (UI_BAR.liste_types),
                      &iter2,
                      0, &nom_type,
                      -1);
  if (strcmp (nom_type, new_text) == 0)
  {
    free (nom_type);
    BUG (_1992_1_1_barres_change_type (barre, (Type_Element) 0, p), )
  }
  else
  {
    gint parcours;
    
    free (nom_type);
    parcours = 1;
    while (gtk_tree_model_iter_next (GTK_TREE_MODEL (UI_BAR.liste_types),
                                     &iter2))
    {
      gtk_tree_model_get (GTK_TREE_MODEL (UI_BAR.liste_types),
                          &iter2,
                          0, &nom_type,
                          -1);
      if (strcmp (nom_type, new_text) == 0)
      {
        free (nom_type);
        BUG (_1992_1_1_barres_change_type (barre,
                                           (Type_Element) parcours,
                                           p),
             )
        return;
      }
      free (nom_type);
      parcours++;
    }
    // Impossible (normalement)
    FAILCRIT ( , (gettext ("Impossible de trouver le type de l'élément.\n")); )
  }
  
  return;
}


/**
 * \brief Changement la section d'une barre.
 * \param cell : cellule en cours,
 * \param path_string : path de la ligne en cours,
 * \param new_text : nom du type de barre,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
extern "C"
void
EF_gtk_barres_edit_section (GtkCellRendererText *cell,
                            const gchar         *path_string,
                            const gchar         *new_text,
                            Projet              *p)
{
  GtkTreeModel *model;
  GtkTreeIter   iter;
  EF_Barre     *barre = NULL;
  Section      *section;
  
  BUGPARAM (p, "%p", p, )
  BUGCRIT (UI_BAR.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Barres"); )
  BUGCRIT (UI_SEC.liste_sections,
           ,
           (gettext ("La liste des sections est indéfinie.\n")); )
  
  model = GTK_TREE_MODEL (gtk_builder_get_object (UI_BAR.builder,
                                                  "EF_barres_treestore"));
  
  gtk_tree_model_get_iter_from_string (model, &iter, path_string);
  gtk_tree_model_get (model, &iter, 0, &barre, -1);
  
  BUG (section = EF_sections_cherche_nom (p, new_text, true), )
  BUG (_1992_1_1_barres_change_section (barre, section, p), )
  
  return;
}


/**
 * \brief Changement le matériau d'une barre.
 * \param cell : cellule en cours,
 * \param path_string : path de la ligne en cours,
 * \param new_text : nom du type de barre,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
extern "C"
void
EF_gtk_barres_edit_materiau (GtkCellRendererText *cell,
                             const gchar         *path_string,
                             const gchar         *new_text,
                             Projet              *p)
{
  GtkTreeModel *model;
  GtkTreeIter   iter;
  EF_Barre     *barre = NULL;
  EF_Materiau  *materiau;
  
  BUGPARAM (p, "%p", p, )
  BUGCRIT (UI_BAR.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Barres"); )
  BUGCRIT (UI_MAT.liste_materiaux,
           ,
           (gettext ("La liste des matériaux est indéfinie.\n")); )
  
  model = GTK_TREE_MODEL (gtk_builder_get_object (UI_BAR.builder,
                                                  "EF_barres_treestore"));
  
  gtk_tree_model_get_iter_from_string (model, &iter, path_string);
  gtk_tree_model_get (model, &iter, 0, &barre, -1);
  
  BUG (materiau = EF_materiaux_cherche_nom (p, new_text, true), )
  BUG (_1992_1_1_barres_change_materiau (barre, materiau, p), )
  
  return;
}


/**
 * \brief Changement du relachement d'une barre.
 * \param cell : cellule en cours,
 * \param path_string : path de la ligne en cours,
 * \param new_text : nom du type de barre,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
extern "C"
void
EF_gtk_barres_edit_relachement (GtkCellRendererText *cell,
                                const gchar         *path_string,
                                const gchar         *new_text,
                                Projet              *p)
{
  GtkTreeModel   *model;
  GtkTreeIter     iter;
  EF_Barre       *barre = NULL;
  
  BUGPARAM (p, "%p", p, )
  BUGCRIT (UI_BAR.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Barres"); )
  BUGCRIT (UI_REL.liste_relachements,
           ,
           (gettext ("La liste des relâchements de barre est indéfinie.\n")); )
  
  model = GTK_TREE_MODEL (gtk_builder_get_object (UI_BAR.builder,
                                                  "EF_barres_treestore"));
  
  gtk_tree_model_get_iter_from_string (model, &iter, path_string);
  gtk_tree_model_get (model, &iter, 0, &barre, -1);
  
  if (strcmp (gettext ("Aucun"), new_text) == 0)
  {
    BUG (_1992_1_1_barres_change_relachement (barre, NULL, p), )
  }
  else
  {
    EF_Relachement *relachement;
    
    BUG (relachement = EF_relachement_cherche_nom (p, new_text, true), )
    BUG (_1992_1_1_barres_change_relachement (barre, relachement, p), )
  }
  
  return;
}


/**
 * \brief Change le noeud initial de la barre.
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
EF_gtk_barres_edit_noeud (GtkCellRendererText *cell,
                          gchar               *path_string,
                          gchar               *new_text,
                          Projet              *p)
{
  GtkTreeModel *model;
  GtkTreeIter   iter;
  char         *fake;
  uint32_t      conversion;
  gint          column;
  EF_Barre     *barre;
  
  BUGPARAM (p, "%p", p, )
  BUGCRIT (UI_BAR.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Barres"); )
  
  BUGCRIT (fake = (char *) malloc (sizeof (char) * (strlen (new_text) + 1)),
           ,
           (gettext ("Erreur d'allocation mémoire.\n")); )
  column = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (cell), "column"));
  
  model = GTK_TREE_MODEL (gtk_builder_get_object (UI_BAR.builder,
                                                  "EF_barres_treestore"));
  
  gtk_tree_model_get_iter_from_string (model, &iter, path_string);
  gtk_tree_model_get (model, &iter, 0, &barre, -1);
  
  // On vérifie si le texte contient bien un nombre entier
  if (sscanf (new_text, "%u%s", &conversion, fake) == 1)
  {
    EF_Noeud *noeud;
    
    free (fake);
    
    BUG (noeud = EF_noeuds_cherche_numero (p, conversion, true), )
    
    if (column == 4)
    {
      BUG (_1992_1_1_barres_change_noeud (barre, noeud, true, p), )
    }
    else
    {
      BUG (_1992_1_1_barres_change_noeud (barre, noeud, false, p), )
    }
  }
  else
  {
    free (fake);
  }
   
  return;
}


/**
 * \brief Change l'angle de la barre.
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
EF_gtk_barres_edit_angle (GtkCellRendererText *cell,
                          gchar               *path_string,
                          gchar               *new_text,
                          Projet              *p)
{
  GtkTreeModel *model;
  GtkTreeIter   iter;
  double        conversion;
  EF_Barre     *barre;
  
  BUGPARAM (p, "%p", p, )
  BUGCRIT (UI_BAR.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Barres"); )
  
  model = GTK_TREE_MODEL (gtk_builder_get_object (UI_BAR.builder,
                                                  "EF_barres_treestore"));
  
  gtk_tree_model_get_iter_from_string (model, &iter, path_string);
  gtk_tree_model_get (model, &iter, 0, &barre, -1);
  
  conversion = common_text_str_to_double (new_text, -360., false, 360., false);
  if (isnan (conversion))
  {
    return;
  }
  
  BUG (_1992_1_1_barres_change_angle (barre,
                                      m_f (conversion, FLOTTANT_UTILISATEUR),
                                      p), )
   
  return;
}


/**
 * \brief Supprime la barre sélectionnée en fonction de l'onglet en cours
 *        d'affichage.
 * \param button : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
extern "C"
void
EF_gtk_barres_supprimer (GtkButton *button,
                         Projet    *p)
{
  GtkTreeModel *model;
  GtkTreeIter   Iter;
  EF_Barre     *barre;
  
  std::list <EF_Barre *> list;
  
  BUGPARAM (p, "%p", p, )
  BUGCRIT (UI_BAR.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Barres"); )
  
  BUGCRIT ( gtk_tree_selection_get_selected (GTK_TREE_SELECTION (
         gtk_builder_get_object (UI_BAR.builder, "EF_barres_treeview_select")),
                                             &model,
                                             &Iter),
           ,
           (gettext ("Aucune barre n'est sélectionnée.\n")); )
  
  gtk_tree_model_get (model, &Iter, 0, &barre, -1);
  
  list.push_back (barre);
  
  BUG (_1992_1_1_barres_supprime_liste (p, NULL, &list), )
  
  BUG (m3d_rafraichit (p), )
  
  return;
}


/**
 * \brief Supprime une barre sans dépendance si la touche SUPPR est appuyée.
 * \param widget : composant à l'origine de l'évènement,
 * \param event : Caractéristique de l'évènement,
 * \param p : la variable projet.
 * \return TRUE si la touche SUPPR est pressée, FALSE sinon.\n
 *   Echec : FALSE.
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
extern "C"
gboolean
EF_gtk_barres_treeview_key_press (GtkWidget *widget,
                                  GdkEvent  *event,
                                  Projet    *p)
{
  BUGPARAM (p, "%p", p, FALSE)
  BUGCRIT (UI_BAR.builder,
           FALSE,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Barres"); )
  
  if (event->key.keyval == GDK_KEY_Delete)
  {
    GtkTreeIter   Iter;
    GtkTreeModel  *model;
    
    if (gtk_tree_selection_get_selected (GTK_TREE_SELECTION (
         gtk_builder_get_object (UI_BAR.builder, "EF_barres_treeview_select")),
                                         &model,
                                         &Iter))
    {
      EF_Barre *barre;
      
      std::list <EF_Barre *> liste_barres;
      
      gtk_tree_model_get (model, &Iter, 0, &barre, -1);
      
      liste_barres.push_back (barre);
      if (!_1992_1_1_barres_cherche_dependances (p,
                                                 NULL,
                                                 NULL,
                                                 NULL,
                                                 NULL,
                                                 NULL,
                                                 &liste_barres,
                                                 NULL,
                                                 NULL,
                                                 NULL,
                                                 NULL,
                                                 NULL,
                                                 false))
      {
        BUG (_1992_1_1_barres_supprime_liste (p, NULL, &liste_barres),
             FALSE)
        BUG (m3d_rafraichit (p),
             FALSE)
      }
    }
    return TRUE;
  }
  else
  {
    return FALSE;
  }
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
EF_gtk_barres_select_changed (GtkTreeSelection *treeselection,
                              Projet           *p)
{
  GtkTreeModel *model;
  GtkTreeIter   Iter;
  
  BUGPARAM (p, "%p", p, )
  BUGCRIT (UI_BAR.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Barres"); )
  
  // Si aucune barre n'est sélectionnée, il n'est pas possible de supprimer.
  if (!gtk_tree_selection_get_selected (GTK_TREE_SELECTION (
         gtk_builder_get_object (UI_BAR.builder, "EF_barres_treeview_select")),
                                        &model,
                                        &Iter))
  {
    gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
                        UI_BAR.builder, "EF_barres_boutton_supprimer_direct")),
                              FALSE);
    gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
                          UI_BAR.builder, "EF_barres_boutton_supprimer_menu")),
                              FALSE);
    gtk_widget_set_visible (GTK_WIDGET (gtk_builder_get_object (UI_BAR.builder,
                                        "EF_barres_boutton_supprimer_direct")),
                            FALSE);
    gtk_widget_set_visible (GTK_WIDGET (gtk_builder_get_object (UI_BAR.builder,
                                          "EF_barres_boutton_supprimer_menu")),
                            TRUE);
  }
  else
  {
    EF_Barre *barre;
    std::list <EF_Barre *> liste_barres;
    
    gtk_tree_model_get (model, &Iter, 0, &barre, -1);
    
    liste_barres.push_back (barre);
    if (_1992_1_1_barres_cherche_dependances (p,
                                              NULL,
                                              NULL,
                                              NULL,
                                              NULL,
                                              NULL,
                                              &liste_barres,
                                              NULL,
                                              NULL,
                                              NULL,
                                              NULL,
                                              NULL,
                                              false))
    {
      gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
                        UI_BAR.builder, "EF_barres_boutton_supprimer_direct")),
                                FALSE);
      gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
                          UI_BAR.builder, "EF_barres_boutton_supprimer_menu")),
                                TRUE);
      gtk_widget_set_visible (GTK_WIDGET (gtk_builder_get_object (
                        UI_BAR.builder, "EF_barres_boutton_supprimer_direct")),
                              FALSE);
      gtk_widget_set_visible (GTK_WIDGET (gtk_builder_get_object (
                          UI_BAR.builder, "EF_barres_boutton_supprimer_menu")),
                              TRUE);
    }
    else
    {
      gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
                        UI_BAR.builder, "EF_barres_boutton_supprimer_direct")),
                                TRUE);
      gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
                          UI_BAR.builder, "EF_barres_boutton_supprimer_menu")),
                                FALSE);
      gtk_widget_set_visible (GTK_WIDGET (gtk_builder_get_object (
                        UI_BAR.builder, "EF_barres_boutton_supprimer_direct")),
                              TRUE);
      gtk_widget_set_visible (GTK_WIDGET (gtk_builder_get_object (
                          UI_BAR.builder, "EF_barres_boutton_supprimer_menu")),
                              FALSE);
    }
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
EF_gtk_barres_boutton_supprimer_menu (GtkButton *widget,
                                      Projet    *p)
{
  GtkTreeModel *model;
  GtkTreeIter   Iter;
  EF_Barre     *barre;
  char         *desc;
  
  std::list <EF_Barre *>  liste_barres;
  std::list <EF_Noeud *> *liste_noeuds_dep;
  std::list <EF_Barre *> *liste_barres_dep;
  std::list <Charge   *> *liste_charges_dep;
  
  BUGPARAM (p, "%p", p, )
  BUGCRIT (UI_BAR.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Barres"); )
  
  // Si aucune barre n'est sélectionnée, il n'est pas possible de supprimer.
  BUGCRIT (gtk_tree_selection_get_selected (GTK_TREE_SELECTION (
         gtk_builder_get_object (UI_BAR.builder, "EF_barres_treeview_select")),
                                            &model,
                                            &Iter),
           ,
           (gettext ("Aucun élément n'est sélectionné.\n")); )
  
  gtk_tree_model_get (model, &Iter, 0, &barre, -1);
  
  liste_barres.push_back (barre);
  BUG (_1992_1_1_barres_cherche_dependances (p,
                                             NULL,
                                             NULL,
                                             NULL,
                                             NULL,
                                             NULL,
                                             &liste_barres,
                                             &liste_noeuds_dep,
                                             NULL,
                                             &liste_barres_dep,
                                             NULL,
                                             &liste_charges_dep,
                                             false),
      , )
  
  BUGCRIT ((!liste_noeuds_dep->empty ()) ||
           (!liste_barres_dep->empty ()) ||
           (!liste_charges_dep->empty ()),
           ,
           (gettext ("L'élément ne possède aucune dépendance.\n")); )

  BUG (desc = common_text_dependances (liste_noeuds_dep,
                                       liste_barres_dep,
                                       liste_charges_dep,
                                       p),
       ,
       delete liste_noeuds_dep;
         delete liste_barres_dep;
         delete liste_charges_dep; )
  gtk_menu_item_set_label (GTK_MENU_ITEM (gtk_builder_get_object (
                         UI_BAR.builder, "EF_barres_supprimer_menu_barres")),
                           desc);
  free (desc);
  
  delete liste_noeuds_dep;
  delete liste_barres_dep;
  delete liste_charges_dep;
  
  return;
}


/**
 * \brief Affiche la distance vz de la section.
 * \param tree_column : composant à l'origine de l'évènement,
 * \param cell : la cellule en cours d'édition,
 * \param tree_model : le mode en cours d'édition,
 * \param iter : la ligne en cours d'édition,
 * \param data2 : la variable projet.
 * \return Rien.
 */
void
EF_gtk_barres_render_0 (GtkTreeViewColumn *tree_column,
                        GtkCellRenderer   *cell,
                        GtkTreeModel      *tree_model,
                        GtkTreeIter       *iter,
                        gpointer           data2)
{
  EF_Barre *barre;
  char     *tmp;
  
  gtk_tree_model_get (tree_model, iter, 0, &barre, -1);
  BUGPARAM (barre, "%p", barre, )
  
  BUGCRIT (tmp = g_strdup_printf ("%d", barre->numero),
           ,
           (gettext ("Erreur d'allocation mémoire.\n")); )
  
  g_object_set (cell, "text", tmp, NULL);
  
  free (tmp);
  
  return;
}


/**
 * \brief Affiche la distance vz de la section.
 * \param tree_column : composant à l'origine de l'évènement,
 * \param cell : la cellule en cours d'édition,
 * \param tree_model : le mode en cours d'édition,
 * \param iter : la ligne en cours d'édition,
 * \param data2 : la variable projet.
 * \return Rien.
 */
void
EF_gtk_barres_render_1 (GtkTreeViewColumn *tree_column,
                        GtkCellRenderer   *cell,
                        GtkTreeModel      *tree_model,
                        GtkTreeIter       *iter,
                        gpointer           data2)
{
  EF_Barre   *barre;
  char       *tmp;
  GtkTreeIter iter2;
  Projet     *p = (Projet *) data2;
  
  BUGPARAM (p, "%p", p, )
  
  gtk_tree_model_get (tree_model, iter, 0, &barre, -1);
  BUGPARAM (barre, "%p", barre, )
  
  BUGCRIT (tmp = g_strdup_printf ("%d", (int) barre->type),
           ,
           (gettext ("Erreur d'allocation mémoire.\n")); )
  gtk_tree_model_get_iter_from_string (GTK_TREE_MODEL (UI_BAR.liste_types),
                                       &iter2,
                                       tmp);
  free (tmp);
  gtk_tree_model_get (GTK_TREE_MODEL (UI_BAR.liste_types),
                      &iter2,
                      0,
                      &tmp,
                      -1);
  
  g_object_set (cell, "text", tmp, NULL);
  
  free (tmp);
  
  return;
}


/**
 * \brief Affiche la distance vz de la section.
 * \param tree_column : composant à l'origine de l'évènement,
 * \param cell : la cellule en cours d'édition,
 * \param tree_model : le mode en cours d'édition,
 * \param iter : la ligne en cours d'édition,
 * \param data2 : la variable projet.
 * \return Rien.
 */
void
EF_gtk_barres_render_2 (GtkTreeViewColumn *tree_column,
                        GtkCellRenderer   *cell,
                        GtkTreeModel      *tree_model,
                        GtkTreeIter       *iter,
                        gpointer           data2)
{
  EF_Barre *barre;
  
  gtk_tree_model_get (tree_model, iter, 0, &barre, -1);
  BUGPARAM (barre, "%p", barre, )
  
  g_object_set (cell, "text", barre->section->nom, NULL);
  
  return;
}


/**
 * \brief Affiche la distance vz de la section.
 * \param tree_column : composant à l'origine de l'évènement,
 * \param cell : la cellule en cours d'édition,
 * \param tree_model : le mode en cours d'édition,
 * \param iter : la ligne en cours d'édition,
 * \param data2 : la variable projet.
 * \return Rien.
 */
void
EF_gtk_barres_render_3 (GtkTreeViewColumn *tree_column,
                        GtkCellRenderer   *cell,
                        GtkTreeModel      *tree_model,
                        GtkTreeIter       *iter,
                        gpointer           data2)
{
  EF_Barre *barre;
  
  gtk_tree_model_get (tree_model, iter, 0, &barre, -1);
  BUGPARAM (barre, "%p", barre, )
  
  g_object_set (cell, "text", barre->materiau->nom, NULL);
  
  return;
}


/**
 * \brief Affiche la distance vz de la section.
 * \param tree_column : composant à l'origine de l'évènement,
 * \param cell : la cellule en cours d'édition,
 * \param tree_model : le mode en cours d'édition,
 * \param iter : la ligne en cours d'édition,
 * \param data2 : la variable projet.
 * \return Rien.
 */
void
EF_gtk_barres_render_4 (GtkTreeViewColumn *tree_column,
                        GtkCellRenderer   *cell,
                        GtkTreeModel      *tree_model,
                        GtkTreeIter       *iter,
                        gpointer           data2)
{
  EF_Barre *barre;
  char     *tmp;
  
  gtk_tree_model_get (tree_model, iter, 0, &barre, -1);
  BUGPARAM (barre, "%p", barre, )
  
  BUGCRIT (tmp = g_strdup_printf ("%d", barre->noeud_debut->numero),
           ,
           (gettext ("Erreur d'allocation mémoire.\n")); )
  
  g_object_set (cell, "text", tmp, NULL);
  
  free (tmp);
  
  return;
}


/**
 * \brief Affiche la distance vz de la section.
 * \param tree_column : composant à l'origine de l'évènement,
 * \param cell : la cellule en cours d'édition,
 * \param tree_model : le mode en cours d'édition,
 * \param iter : la ligne en cours d'édition,
 * \param data2 : la variable projet.
 * \return Rien.
 */
void
EF_gtk_barres_render_5 (GtkTreeViewColumn *tree_column,
                        GtkCellRenderer   *cell,
                        GtkTreeModel      *tree_model,
                        GtkTreeIter       *iter,
                        gpointer           data2)
{
  EF_Barre *barre;
  char     *tmp;
  
  gtk_tree_model_get (tree_model, iter, 0, &barre, -1);
  BUGPARAM (barre, "%p", barre, )
  
  BUGCRIT (tmp = g_strdup_printf ("%d", barre->noeud_fin->numero),
           ,
           (gettext ("Erreur d'allocation mémoire.\n")); )
  
  g_object_set (cell, "text", tmp, NULL);
  
  free (tmp);
  
  return;
}


/**
 * \brief Affiche la distance vz de la section.
 * \param tree_column : composant à l'origine de l'évènement,
 * \param cell : la cellule en cours d'édition,
 * \param tree_model : le mode en cours d'édition,
 * \param iter : la ligne en cours d'édition,
 * \param data2 : la variable projet.
 * \return Rien.
 */
void
EF_gtk_barres_render_6 (GtkTreeViewColumn *tree_column,
                        GtkCellRenderer   *cell,
                        GtkTreeModel      *tree_model,
                        GtkTreeIter       *iter,
                        gpointer           data2)
{
  EF_Barre *barre;
  char      tmp[30];
  
  gtk_tree_model_get (tree_model, iter, 0, &barre, -1);
  BUGPARAM (barre, "%p", barre, )
  
  conv_f_c (barre->angle, tmp, DECIMAL_ANGLE);
  
  g_object_set (cell, "text", tmp, NULL);
  
  return;
}


/**
 * \brief Affiche la distance vz de la section.
 * \param tree_column : composant à l'origine de l'évènement,
 * \param cell : la cellule en cours d'édition,
 * \param tree_model : le mode en cours d'édition,
 * \param iter : la ligne en cours d'édition,
 * \param data2 : la variable projet.
 * \return Rien.
 */
void
EF_gtk_barres_render_7 (GtkTreeViewColumn *tree_column,
                        GtkCellRenderer   *cell,
                        GtkTreeModel      *tree_model,
                        GtkTreeIter       *iter,
                        gpointer           data2)
{
  EF_Barre *barre;
  
  gtk_tree_model_get (tree_model, iter, 0, &barre, -1);
  BUGPARAM (barre, "%p", barre, )
  
  if (barre->relachement == NULL)
  {
    g_object_set (cell, "text", gettext ("Aucun"), NULL);
  }
  else
  {
    g_object_set (cell, "text", barre->relachement->nom, NULL);
  }
  
  return;
}


/**
 * \brief Création de la fenêtre permettant d'afficher les barres sous forme
 *        d'un tableau.
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique impossible à générer.
 */
void
EF_gtk_barres (Projet *p)
{
  std::list <EF_Barre *>::iterator it;
  
  BUGPARAM (p, "%p", p, )
  if (UI_BAR.builder != NULL)
  {
    gtk_window_present (GTK_WINDOW (UI_BAR.window));
    return;
  }
  
  UI_BAR.builder = gtk_builder_new ();
  BUGCRIT (gtk_builder_add_from_resource (UI_BAR.builder,
                                          "/org/2lgc/codegui/ui/EF_barres.ui",
                                          NULL) != 0,
           ,
           (gettext ("La génération de la fenêtre %s a échouée.\n"),
                     "Barres"); )
  gtk_builder_connect_signals (UI_BAR.builder, p);
  
  UI_BAR.window = GTK_WIDGET (gtk_builder_get_object (UI_BAR.builder,
                                                      "EF_barres_window"));
  
  g_object_set (gtk_builder_get_object (UI_BAR.builder,
                                        "EF_barres_treeview_cell1"),
                "model",
                UI_BAR.liste_types,
                NULL);
  g_object_set (gtk_builder_get_object (UI_BAR.builder,
                                        "EF_barres_treeview_cell2"),
                "model",
                UI_SEC.liste_sections,
                NULL);
  g_object_set (gtk_builder_get_object (UI_BAR.builder,
                                        "EF_barres_treeview_cell3"),
                "model",
                UI_MAT.liste_materiaux,
                NULL);
  g_object_set (gtk_builder_get_object (UI_BAR.builder,
                                        "EF_barres_treeview_cell7"),
                "model",
                UI_REL.liste_relachements,
                NULL);
  
  g_object_set_data (gtk_builder_get_object (UI_BAR.builder,
                                             "EF_barres_treeview_cell4"),
                     "column",
                     GINT_TO_POINTER (4));
  g_object_set_data (gtk_builder_get_object (UI_BAR.builder,
                                             "EF_barres_treeview_cell5"),
                     "column",
                     GINT_TO_POINTER (5));
  
  gtk_tree_view_column_set_cell_data_func (
    GTK_TREE_VIEW_COLUMN (gtk_builder_get_object (UI_BAR.builder,
                                                "EF_barres_treeview_column0")),
    GTK_CELL_RENDERER (gtk_builder_get_object (UI_BAR.builder,
                                               "EF_barres_treeview_cell0")),
    EF_gtk_barres_render_0,
    p,
    NULL);
  gtk_tree_view_column_set_cell_data_func (
    GTK_TREE_VIEW_COLUMN (gtk_builder_get_object (UI_BAR.builder,
                                                "EF_barres_treeview_column1")),
    GTK_CELL_RENDERER (gtk_builder_get_object (UI_BAR.builder,
                                               "EF_barres_treeview_cell1")),
    EF_gtk_barres_render_1,
    p,
    NULL);
  gtk_tree_view_column_set_cell_data_func (
    GTK_TREE_VIEW_COLUMN (gtk_builder_get_object (UI_BAR.builder,
                                                "EF_barres_treeview_column2")),
    GTK_CELL_RENDERER (gtk_builder_get_object (UI_BAR.builder,
                                               "EF_barres_treeview_cell2")),
    EF_gtk_barres_render_2,
    p,
    NULL);
  gtk_tree_view_column_set_cell_data_func (
    GTK_TREE_VIEW_COLUMN (gtk_builder_get_object (UI_BAR.builder,
                                                "EF_barres_treeview_column3")),
    GTK_CELL_RENDERER (gtk_builder_get_object (UI_BAR.builder,
                                               "EF_barres_treeview_cell3")),
    EF_gtk_barres_render_3,
    p,
    NULL);
  gtk_tree_view_column_set_cell_data_func (
    GTK_TREE_VIEW_COLUMN (gtk_builder_get_object (UI_BAR.builder,
                                                "EF_barres_treeview_column4")),
    GTK_CELL_RENDERER (gtk_builder_get_object (UI_BAR.builder,
                                               "EF_barres_treeview_cell4")),
    EF_gtk_barres_render_4,
    p,
    NULL);
  gtk_tree_view_column_set_cell_data_func (
    GTK_TREE_VIEW_COLUMN (gtk_builder_get_object (UI_BAR.builder,
                                                "EF_barres_treeview_column5")),
    GTK_CELL_RENDERER (gtk_builder_get_object (UI_BAR.builder,
                                               "EF_barres_treeview_cell5")),
    EF_gtk_barres_render_5,
    p,
    NULL);
  gtk_tree_view_column_set_cell_data_func (
    GTK_TREE_VIEW_COLUMN (gtk_builder_get_object (UI_BAR.builder,
                                                "EF_barres_treeview_column6")),
    GTK_CELL_RENDERER (gtk_builder_get_object (UI_BAR.builder,
                                                  "EF_barres_treeview_cell6")),
    EF_gtk_barres_render_6,
    p,
    NULL);
  gtk_tree_view_column_set_cell_data_func (
    GTK_TREE_VIEW_COLUMN (gtk_builder_get_object (UI_BAR.builder,
                                                "EF_barres_treeview_column7")),
    GTK_CELL_RENDERER (gtk_builder_get_object (UI_BAR.builder,
                                                  "EF_barres_treeview_cell7")),
    EF_gtk_barres_render_7,
    p,
    NULL);
  
  it = p->modele.barres.begin ();
  while (it != p->modele.barres.end ())
  {
    EF_Barre   *barre = *it;
    GtkTreeIter iter;
    char       *tmp;
    
    BUGCRIT (tmp = g_strdup_printf ("%d", (int) barre->type),
             ,
             (gettext ("Erreur d'allocation mémoire.\n")); )
    gtk_tree_model_get_iter_from_string (GTK_TREE_MODEL (UI_BAR.liste_types),
                                         &iter,
                                         tmp);
    free (tmp);
    
    gtk_tree_store_append (GTK_TREE_STORE (gtk_builder_get_object (
                                       UI_BAR.builder, "EF_barres_treestore")),
                           &barre->Iter,
                           NULL);
    gtk_tree_store_set (GTK_TREE_STORE (gtk_builder_get_object (
                                       UI_BAR.builder, "EF_barres_treestore")),
                        &barre->Iter,
                        0, barre,
                        -1);
    
    ++it;
  }
  
  gtk_window_set_transient_for (GTK_WINDOW (UI_BAR.window),
                                GTK_WINDOW (UI_GTK.window));
}

#endif

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
