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
#include "common_math.hpp"
#include "common_selection.hpp"
#include "common_text.hpp"
#include "EF_relachement.hpp"
#include "1992_1_1_barres.hpp"
#include "EF_gtk_relachement.hpp"


GTK_WINDOW_CLOSE (ef, relachements);


GTK_WINDOW_DESTROY (ef, relachements, );


GTK_WINDOW_KEY_PRESS (ef, relachements);


/**
 * \brief En fonction de la sélection, active ou désactive le bouton supprimer.
 *        Rend également éditable ou non les cellules des tableaux en fonction
 *        de type de relâchement.
 * \param treeselection : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
extern "C"
void
EF_gtk_relachements_select_changed (GtkTreeSelection *treeselection,
                                    Projet           *p)
{
  GtkTreeModel *model;
  GtkTreeIter   Iter;
  
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_REL.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Relachement"); )
  
  // Si aucun relâchenement n'est sélectionné, il n'est pas possible d'en
  // supprimer ou d'en éditer un.
  if (!gtk_tree_selection_get_selected (GTK_TREE_SELECTION (
   gtk_builder_get_object (UI_REL.builder, "EF_relachements_treeview_select")),
                                        &model,
                                        &Iter))
  {
    gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
                  UI_REL.builder, "EF_relachements_boutton_supprimer_direct")),
                              FALSE);
    gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
                    UI_REL.builder, "EF_relachements_boutton_supprimer_menu")),
                              FALSE);
    gtk_widget_set_visible (GTK_WIDGET (gtk_builder_get_object (UI_REL.builder,
                                  "EF_relachements_boutton_supprimer_direct")),
                            TRUE);
    gtk_widget_set_visible (GTK_WIDGET (gtk_builder_get_object (UI_REL.builder,
                                    "EF_relachements_boutton_supprimer_menu")),
                            FALSE);
  }
  else
  {
    EF_Relachement      *relachement;
    GtkCellRendererText *cell;
    
    std::list <EF_Relachement *> liste_relachements;
    
    gtk_tree_model_get (model, &Iter, 0, &relachement, -1);
    
    liste_relachements.push_back (relachement);
    if (_1992_1_1_barres_cherche_dependances (p,
                                              NULL,
                                              NULL,
                                              NULL,
                                              NULL,
                                              &liste_relachements,
                                              NULL,
                                              NULL,
                                              NULL,
                                              NULL,
                                              NULL,
                                              NULL,
                                              FALSE))
    {
      gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
                  UI_REL.builder, "EF_relachements_boutton_supprimer_direct")),
                                FALSE);
      gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
                    UI_REL.builder, "EF_relachements_boutton_supprimer_menu")),
                                TRUE);
      gtk_widget_set_visible (GTK_WIDGET (gtk_builder_get_object (
                  UI_REL.builder, "EF_relachements_boutton_supprimer_direct")),
                              FALSE);
      gtk_widget_set_visible (GTK_WIDGET (gtk_builder_get_object (
                    UI_REL.builder, "EF_relachements_boutton_supprimer_menu")),
                              TRUE);
    }
    else
    {
      gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
                  UI_REL.builder, "EF_relachements_boutton_supprimer_direct")),
                                TRUE);
      gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
                    UI_REL.builder, "EF_relachements_boutton_supprimer_menu")),
                                FALSE);
      gtk_widget_set_visible (GTK_WIDGET (gtk_builder_get_object (
                  UI_REL.builder, "EF_relachements_boutton_supprimer_direct")),
                              TRUE);
      gtk_widget_set_visible (GTK_WIDGET (gtk_builder_get_object (
                    UI_REL.builder, "EF_relachements_boutton_supprimer_menu")),
                              FALSE);
    }
    
    cell = GTK_CELL_RENDERER_TEXT (gtk_builder_get_object (UI_REL.builder,
                                            "EF_relachements_treeview_cell2"));
    switch (relachement->rx_debut)
    {
      case EF_RELACHEMENT_BLOQUE :
      case EF_RELACHEMENT_LIBRE :
      {
        g_object_set (cell, "editable", FALSE, NULL);
        break;
      }
      case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
      {
        g_object_set (cell, "editable", TRUE, NULL);
        break;
      }
      case EF_RELACHEMENT_UNTOUCH :
      default :
      {
        FAILCRIT ( ,
                  (gettext ("Relachement %d inconnu."),
                            relachement->rx_debut); )
        break;
      }
    }
    cell = GTK_CELL_RENDERER_TEXT (gtk_builder_get_object (UI_REL.builder,
                                            "EF_relachements_treeview_cell4"));
    switch (relachement->ry_debut)
    {
      case EF_RELACHEMENT_BLOQUE :
      case EF_RELACHEMENT_LIBRE :
      {
        g_object_set (cell, "editable", FALSE, NULL);
        break;
      }
      case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
      {
        g_object_set (cell, "editable", TRUE, NULL);
        break;
      }
      case EF_RELACHEMENT_UNTOUCH :
      default :
      {
        FAILCRIT ( ,
                  (gettext ("Relachement %d inconnu."),
                            relachement->ry_debut); )
        break;
      }
    }
    cell = GTK_CELL_RENDERER_TEXT (gtk_builder_get_object (UI_REL.builder,
                                            "EF_relachements_treeview_cell6"));
    switch (relachement->rz_debut)
    {
      case EF_RELACHEMENT_BLOQUE :
      case EF_RELACHEMENT_LIBRE :
      {
        g_object_set (cell, "editable", FALSE, NULL);
        break;
      }
      case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
      {
        g_object_set (cell, "editable", TRUE, NULL);
        break;
      }
      case EF_RELACHEMENT_UNTOUCH :
      default :
      {
        FAILCRIT ( ,
                  (gettext ("Relachement %d inconnu."),
                            relachement->rz_debut); )
        break;
      }
    }
    cell = GTK_CELL_RENDERER_TEXT (gtk_builder_get_object (UI_REL.builder,
                                            "EF_relachements_treeview_cell8"));
    switch (relachement->rx_fin)
    {
      case EF_RELACHEMENT_BLOQUE :
      case EF_RELACHEMENT_LIBRE :
      {
        g_object_set (cell, "editable", FALSE, NULL);
        break;
      }
      case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
      {
        g_object_set (cell, "editable", TRUE, NULL);
        break;
      }
      case EF_RELACHEMENT_UNTOUCH :
      default :
      {
        FAILCRIT ( ,
                  (gettext ("Relachement %d inconnu."), relachement->rx_fin); )
        break;
      }
    }
    cell = GTK_CELL_RENDERER_TEXT (gtk_builder_get_object (UI_REL.builder,
                                           "EF_relachements_treeview_cell10"));
    switch (relachement->ry_fin)
    {
      case EF_RELACHEMENT_BLOQUE :
      case EF_RELACHEMENT_LIBRE :
      {
        g_object_set (cell, "editable", FALSE, NULL);
        break;
      }
      case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
      {
        g_object_set (cell, "editable", TRUE, NULL);
        break;
      }
      case EF_RELACHEMENT_UNTOUCH :
      default :
      {
        FAILCRIT ( ,
                  (gettext ("Relachement %d inconnu."), relachement->ry_fin); )
        break;
      }
    }
    cell = GTK_CELL_RENDERER_TEXT (gtk_builder_get_object (UI_REL.builder,
                                           "EF_relachements_treeview_cell12"));
    switch (relachement->rz_fin)
    {
      case EF_RELACHEMENT_BLOQUE :
      case EF_RELACHEMENT_LIBRE :
      {
        g_object_set (cell, "editable", FALSE, NULL);
        break;
      }
      case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
      {
        g_object_set (cell, "editable", TRUE, NULL);
        break;
      }
      case EF_RELACHEMENT_UNTOUCH :
      default :
      {
        FAILCRIT ( ,
                  (gettext ("Relachement %d inconnu."), relachement->rz_fin); )
        break;
      }
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
EF_gtk_relachements_boutton_supprimer_menu (GtkButton *widget,
                                            Projet    *p)
{
  GtkTreeModel   *model;
  GtkTreeIter     Iter;
  EF_Relachement *relachement;
  
  std::list <EF_Relachement *>  liste_relachements;
  std::list <EF_Noeud       *> *liste_noeuds_dep;
  std::list <EF_Barre       *> *liste_barres_dep;
  std::list <Charge         *> *liste_charges_dep;
  
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_REL.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Relachement"); )
  
  // Si aucun relâchenement n'est sélectionné, il n'est pas possible d'en
  // supprimer ou d'en éditer un.
  if (!gtk_tree_selection_get_selected (GTK_TREE_SELECTION (
                                        gtk_builder_get_object (UI_REL.builder,
                                           "EF_relachements_treeview_select")),
                                        &model,
                                        &Iter))
  {
    FAILCRIT ( , (gettext ("Aucun élément n'est sélectionné.\n")); )
  }
  
  gtk_tree_model_get (model, &Iter, 0, &relachement, -1);
  
  liste_relachements.push_back (relachement);
  BUG (_1992_1_1_barres_cherche_dependances (p,
                                             NULL,
                                             NULL,
                                             NULL,
                                             NULL,
                                             &liste_relachements,
                                             NULL,
                                             &liste_noeuds_dep,
                                             NULL,
                                             &liste_barres_dep,
                                             NULL,
                                             &liste_charges_dep,
                                             FALSE),
      )
  
  if ((!liste_noeuds_dep->empty ()) ||
      (!liste_barres_dep->empty ()) ||
      (!liste_charges_dep->empty ()))
  {
    char *desc;
    
    BUG (desc = common_text_dependances (liste_noeuds_dep,
                                         liste_barres_dep,
                                         liste_charges_dep,
                                         p),
         ,
         delete liste_noeuds_dep;
           delete liste_barres_dep;
           delete liste_charges_dep; )
    gtk_menu_item_set_label (GTK_MENU_ITEM (gtk_builder_get_object (
                     UI_REL.builder, "EF_relachements_supprimer_menu_barres")),
                             desc);
    free (desc);
    delete liste_noeuds_dep;
    delete liste_barres_dep;
    delete liste_charges_dep;
  }
  else
  {
    FAILCRIT ( , (gettext ("L'élément ne possède aucune dépendance.\n")); )
  }
  
  delete liste_noeuds_dep;
  delete liste_barres_dep;
  delete liste_charges_dep;
  
  return;
}


/**
 * \brief Modification du nom d'un relâchement.
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
EF_gtk_relachements_edit_nom (GtkCellRendererText *cell,
                              gchar               *path_string,
                              gchar               *new_text,
                              Projet              *p)
{
  GtkTreeModel   *model;
  GtkTreeIter     iter;
  GtkTreePath    *path;
  EF_Relachement *relachement;
  
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_REL.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Relachement"); )
  
  model = GTK_TREE_MODEL (UI_REL.relachements);
  path = gtk_tree_path_new_from_string (path_string);
  gtk_tree_model_get_iter (model, &iter, path);
  gtk_tree_path_free (path);
  gtk_tree_model_get (model, &iter, 0, &relachement, -1);
  if ((strcmp (relachement->nom, new_text) == 0) ||
      (strcmp (new_text, "") == 0))
  {
    return;
  }
  if (EF_relachement_cherche_nom (p, new_text, FALSE))
  {
    return;
  }

  BUG (EF_relachement_modif (p,
                             relachement,
                             new_text,
                             EF_RELACHEMENT_UNTOUCH,
                             NULL,
                             EF_RELACHEMENT_UNTOUCH,
                             NULL,
                             EF_RELACHEMENT_UNTOUCH,
                             NULL,
                             EF_RELACHEMENT_UNTOUCH,
                             NULL,
                             EF_RELACHEMENT_UNTOUCH,
                             NULL,
                             EF_RELACHEMENT_UNTOUCH,
                             NULL),
      )
  
  return;
}


/**
 * \brief Supprime le relâchement sélectionné dans le treeview.
 * \param button : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
extern "C"
void
EF_gtk_relachements_supprimer_direct (GtkButton *button,
                                      Projet    *p)
{
  GtkTreeIter     iter;
  GtkTreeModel   *model;
  EF_Relachement *relachement;
  
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_REL.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Relachement"); )
  
  if (!gtk_tree_selection_get_selected (GTK_TREE_SELECTION (
                                        gtk_builder_get_object (UI_REL.builder,
                                           "EF_relachements_treeview_select")),
                                        &model,
                                        &iter))
  {
    return;
  }
  
  gtk_tree_model_get (model, &iter, 0, &relachement, -1);
  
  BUG (EF_relachement_supprime (relachement, TRUE, p), )
  
  BUG (m3d_rafraichit (p), )
  
  return;
}


/**
 * \brief Supprime un relâchement sans dépendance si la touche SUPPR est
 *        appuyée.
 * \param widget : composant à l'origine de l'évènement,
 * \param event : Caractéristique de l'évènement,
 * \param p : la variable projet.
 * \return TRUE si la touche SUPPR est pressée, FALSE sinon.\n
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
extern "C"
gboolean
EF_gtk_relachements_treeview_key_press (GtkWidget *widget,
                                        GdkEvent  *event,
                                        Projet    *p)
{
  BUGPARAMCRIT (p, "%p", p, FALSE)
  BUGCRIT (UI_REL.builder,
           FALSE,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Relachement"); )
  
  if (event->key.keyval == GDK_KEY_Delete)
  {
    GtkTreeIter   Iter;
    GtkTreeModel *model;
    
    if (gtk_tree_selection_get_selected (GTK_TREE_SELECTION (
                                        gtk_builder_get_object (UI_REL.builder,
                                           "EF_relachements_treeview_select")),
                                         &model,
                                         &Iter))
    {
      EF_Relachement *relachement;
      
      std::list <EF_Relachement *> liste_relachements;
      
      gtk_tree_model_get (model, &Iter, 0, &relachement, -1);
      
      liste_relachements.push_back (relachement);
      if (_1992_1_1_barres_cherche_dependances (p,
                                                NULL,
                                                NULL,
                                                NULL,
                                                NULL,
                                                &liste_relachements,
                                                NULL,
                                                NULL,
                                                NULL,
                                                NULL,
                                                NULL,
                                                NULL,
                                                FALSE) == FALSE)
      {
        EF_gtk_relachements_supprimer_direct (NULL, p);
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
 * \brief Supprime le relachement sélectionné dans le treeview, y compris les
 *        barres l'utilisant.
 * \param button : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
extern "C"
void
EF_gtk_relachements_supprimer_menu_barres (GtkButton *button,
                                           Projet    *p)
{
  GtkTreeIter     iter;
  GtkTreeModel   *model;
  EF_Relachement *relachement;
  
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_REL.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Relachement"); )
  
  if (!gtk_tree_selection_get_selected (GTK_TREE_SELECTION (
                                        gtk_builder_get_object (UI_REL.builder,
                                           "EF_relachements_treeview_select")),
                                        &model,
                                        &iter))
  {
    return;
  }
  
  gtk_tree_model_get (model, &iter, 0, &relachement, -1);
  
  BUG (EF_relachement_supprime (relachement, FALSE, p), )
  
  BUG (m3d_rafraichit (p), )
  
  return;
}


/**
 * \brief Ajoute un nouveau relâchement vierge au treeview d'affichage.
 * \param button : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
extern "C"
void
EF_gtk_relachements_ajouter (GtkButton *button,
                             Projet    *p)
{
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_REL.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Relâchement"); )
  
  if (EF_relachement_cherche_nom (p, gettext ("Sans nom"), FALSE) == NULL)
  {
    BUG (EF_relachement_ajout (p,
                               gettext ("Sans nom"),
                               EF_RELACHEMENT_BLOQUE,
                               NULL,
                               EF_RELACHEMENT_BLOQUE,
                               NULL,
                               EF_RELACHEMENT_BLOQUE,
                               NULL,
                               EF_RELACHEMENT_BLOQUE,
                               NULL,
                               EF_RELACHEMENT_BLOQUE,
                               NULL,
                               EF_RELACHEMENT_BLOQUE,
                               NULL),
        )
  }
  else
  {
    char    *nom;
    uint16_t i = 2;
    
    BUGCRIT (nom = g_strdup_printf ("%s (%d)", gettext ("Sans nom"), i),
             ,
             (gettext ("Erreur d'allocation mémoire.\n")); )
    while (EF_relachement_cherche_nom (p, nom, FALSE) != NULL)
    {
      i++;
      free (nom);
      BUGCRIT (nom = g_strdup_printf ("%s (%d)", gettext ("Sans nom"), i),
               ,
               (gettext ("Erreur d'allocation mémoire.\n")); )
    }
    BUG (EF_relachement_ajout (p,
                               nom,
                               EF_RELACHEMENT_BLOQUE,
                               NULL,
                               EF_RELACHEMENT_BLOQUE,
                               NULL,
                               EF_RELACHEMENT_BLOQUE,
                               NULL,
                               EF_RELACHEMENT_BLOQUE,
                               NULL,
                               EF_RELACHEMENT_BLOQUE,
                               NULL,
                               EF_RELACHEMENT_BLOQUE,
                               NULL),
        ,
        free (nom); )
    free (nom);
  }
  
  return;
}


/**
 * \brief Edite le relâchement sélectionné.
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
EF_gtk_relachements_edit_clicked (GtkCellRendererText *cell,
                                  gchar               *path_string,
                                  gchar               *new_text,
                                  Projet              *p)
{
  GtkTreeModel   *model;
  GtkTreeIter     iter;
  GtkTreePath    *path;
  gint            column;
  EF_Relachement *relachement;
  double          conversion;
  Flottant        conversion2;
  
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_REL.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Relachement"); )
  
  column = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (cell), "column"));
  model = GTK_TREE_MODEL (UI_REL.relachements);
  path = gtk_tree_path_new_from_string (path_string);
  gtk_tree_model_get_iter (model, &iter, path);
  gtk_tree_path_free (path);
  gtk_tree_model_get (model, &iter, 0, &relachement, -1);
  
  conversion = common_text_str_to_double (new_text, 0., TRUE, INFINITY, FALSE);
  
  if (isnan (conversion))
  {
    return;
  }
  
  conversion2 = m_f (conversion, FLOTTANT_UTILISATEUR);
  
  if (column == 0)
  {
    switch (relachement->rx_debut)
    {
      case EF_RELACHEMENT_BLOQUE :
      case EF_RELACHEMENT_LIBRE :
      {
        FAILCRIT ( ,
                  (gettext ("Impossible d'éditer ce type de relâchement.\n")); )
        break;
      }
      case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
      {
        EF_Relachement_Donnees_Elastique_Lineaire data;
        
        data.raideur = conversion2;
        BUG (EF_relachement_modif (p,
                                   relachement,
                                   NULL,
                                   EF_RELACHEMENT_UNTOUCH,
                                   &data,
                                   EF_RELACHEMENT_UNTOUCH,
                                   NULL,
                                   EF_RELACHEMENT_UNTOUCH,
                                   NULL,
                                   EF_RELACHEMENT_UNTOUCH,
                                   NULL,
                                   EF_RELACHEMENT_UNTOUCH,
                                   NULL,
                                   EF_RELACHEMENT_UNTOUCH,
                                   NULL),
            )
        
        break;
      }
      case EF_RELACHEMENT_UNTOUCH :
      default :
      {
        FAILCRIT ( ,
                  (gettext ("Le type de relâchement est inconnu.\n")); )
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
        FAILCRIT ( ,
                  (gettext ("Impossible d'éditer ce type de relâchement.\n")); )
        break;
      }
      case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
      {
        EF_Relachement_Donnees_Elastique_Lineaire data;
        
        data.raideur = conversion2;
        BUG (EF_relachement_modif (p,
                                   relachement,
                                   NULL,
                                   EF_RELACHEMENT_UNTOUCH,
                                   NULL,
                                   EF_RELACHEMENT_UNTOUCH,
                                   &data,
                                   EF_RELACHEMENT_UNTOUCH,
                                   NULL,
                                   EF_RELACHEMENT_UNTOUCH,
                                   NULL,
                                   EF_RELACHEMENT_UNTOUCH,
                                   NULL,
                                   EF_RELACHEMENT_UNTOUCH,
                                   NULL),
            )
        
        break;
      }
      case EF_RELACHEMENT_UNTOUCH :
      default :
      {
        FAILCRIT ( , (gettext ("Le type de relâchement est inconnu.\n")); )
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
        FAILCRIT ( ,
                  (gettext ("Impossible d'éditer ce type de relâchement.\n")); )
        break;
      }
      case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
      {
        EF_Relachement_Donnees_Elastique_Lineaire data;
        
        data.raideur = conversion2;
        BUG (EF_relachement_modif (p,
                                   relachement,
                                   NULL,
                                   EF_RELACHEMENT_UNTOUCH,
                                   NULL,
                                   EF_RELACHEMENT_UNTOUCH,
                                   NULL,
                                   EF_RELACHEMENT_UNTOUCH,
                                   &data,
                                   EF_RELACHEMENT_UNTOUCH,
                                   NULL,
                                   EF_RELACHEMENT_UNTOUCH,
                                   NULL,
                                   EF_RELACHEMENT_UNTOUCH,
                                   NULL),
            )
        
        break;
      }
      case EF_RELACHEMENT_UNTOUCH :
      default :
      {
        FAILCRIT ( , (gettext ("Le type de relâchement est inconnu.\n")); )
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
        FAILCRIT ( ,
                  (gettext ("Impossible d'éditer ce type de relâchement.\n")); )
        break;
      }
      case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
      {
        EF_Relachement_Donnees_Elastique_Lineaire data;
        
        data.raideur = conversion2;
        BUG (EF_relachement_modif (p,
                                   relachement,
                                   NULL,
                                   EF_RELACHEMENT_UNTOUCH,
                                   NULL,
                                   EF_RELACHEMENT_UNTOUCH,
                                   NULL,
                                   EF_RELACHEMENT_UNTOUCH,
                                   NULL,
                                   EF_RELACHEMENT_UNTOUCH,
                                   &data,
                                   EF_RELACHEMENT_UNTOUCH,
                                   NULL,
                                   EF_RELACHEMENT_UNTOUCH,
                                   NULL),
            )
        
        break;
      }
      case EF_RELACHEMENT_UNTOUCH :
      default :
      {
        FAILCRIT ( , (gettext ("Le type de relâchement est inconnu.\n")); )
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
        FAILCRIT ( ,
                  (gettext ("Impossible d'éditer ce type de relâchement.\n")); )
        break;
      }
      case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
      {
        EF_Relachement_Donnees_Elastique_Lineaire data;
        
        data.raideur = conversion2;
        BUG (EF_relachement_modif (p,
                                   relachement,
                                   NULL,
                                   EF_RELACHEMENT_UNTOUCH,
                                   NULL,
                                   EF_RELACHEMENT_UNTOUCH,
                                   NULL,
                                   EF_RELACHEMENT_UNTOUCH,
                                   NULL,
                                   EF_RELACHEMENT_UNTOUCH,
                                   NULL,
                                   EF_RELACHEMENT_UNTOUCH,
                                   &data,
                                   EF_RELACHEMENT_UNTOUCH,
                                   NULL),
            )
        
        break;
      }
      case EF_RELACHEMENT_UNTOUCH :
      default :
      {
        FAILCRIT ( , (gettext ("Le type de relâchement est inconnu.\n")); )
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
        FAILCRIT ( ,
                  (gettext ("Impossible d'éditer ce type de relâchement.\n")); )
        break;
      }
      case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
      {
        EF_Relachement_Donnees_Elastique_Lineaire data;
        
        data.raideur = conversion2;
        BUG (EF_relachement_modif (p,
                                   relachement,
                                   NULL,
                                   EF_RELACHEMENT_UNTOUCH,
                                   NULL,
                                   EF_RELACHEMENT_UNTOUCH,
                                   NULL,
                                   EF_RELACHEMENT_UNTOUCH,
                                   NULL,
                                   EF_RELACHEMENT_UNTOUCH,
                                   NULL,
                                   EF_RELACHEMENT_UNTOUCH,
                                   NULL,
                                   EF_RELACHEMENT_UNTOUCH,
                                   &data),
             
             )
        
        break;
      }
      case EF_RELACHEMENT_UNTOUCH :
      default :
      {
        FAILCRIT ( , (gettext ("Le type de relâchement est inconnu.\n")); )
        break;
      }
    }
  }
  
  return;
}


/**
 * \brief Édite le type du relâchement sélectionné.
 * \param cell : cellule en cours,
 * \param path_string : path de la ligne en cours,
 * \param new_text : le nouveau type du relâchement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
extern "C"
void
EF_gtk_relachements_edit_type (GtkCellRendererText *cell,
                               gchar               *path_string,
                               gchar               *new_text,
                               Projet              *p)
{
  GtkTreeModel       *model;
  GtkTreeIter         iter;
  GtkTreePath        *path;
  gint                column;
  EF_Relachement     *relachement;
  EF_Relachement_Type type;
  EF_Relachement_Donnees_Elastique_Lineaire elastique_lineaire;
  
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_REL.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Relachement"); )
  
  column = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (cell), "column"));
  model = GTK_TREE_MODEL (UI_REL.relachements);
  path = gtk_tree_path_new_from_string (path_string);
  gtk_tree_model_get_iter (model, &iter, path);
  gtk_tree_path_free (path);
  gtk_tree_model_get (model, &iter, 0, &relachement, -1);
  
  if (strcmp (gettext ("Bloqué"), new_text) == 0)
  {
    type = EF_RELACHEMENT_BLOQUE;
  }
  else if (strcmp (gettext ("Libre"), new_text) == 0)
  {
    type = EF_RELACHEMENT_LIBRE;
  }
  else if (strcmp (gettext ("Linéaire"), new_text) == 0)
  {
    type = EF_RELACHEMENT_ELASTIQUE_LINEAIRE;
  }
  else
  {
    FAILCRIT ( , (gettext ("Le type de relâchement est inconnu.\n")); )
  }
  
  elastique_lineaire.raideur = m_f (0., FLOTTANT_UTILISATEUR);
  
  switch (column)
  {
    case 0 :
    {
      BUG (EF_relachement_modif (p,
                                 relachement,
                                 NULL,
                                 type,
                                 (type == EF_RELACHEMENT_ELASTIQUE_LINEAIRE) &&
                                   (type != relachement->rx_debut) ?
                                     &elastique_lineaire :
                                     NULL,
                                 EF_RELACHEMENT_UNTOUCH,
                                 NULL,
                                 EF_RELACHEMENT_UNTOUCH,
                                 NULL,
                                 EF_RELACHEMENT_UNTOUCH,
                                 NULL,
                                 EF_RELACHEMENT_UNTOUCH,
                                 NULL,
                                 EF_RELACHEMENT_UNTOUCH,
                                 NULL),
          )
      break;
    }
    case 1 :
    {
      BUG (EF_relachement_modif (p,
                                 relachement,
                                 NULL,
                                 EF_RELACHEMENT_UNTOUCH,
                                 NULL,
                                 type,
                                 (type == EF_RELACHEMENT_ELASTIQUE_LINEAIRE) &&
                                   (type != relachement->ry_debut) ?
                                     &elastique_lineaire :
                                     NULL,
                                 EF_RELACHEMENT_UNTOUCH,
                                 NULL,
                                 EF_RELACHEMENT_UNTOUCH,
                                 NULL,
                                 EF_RELACHEMENT_UNTOUCH,
                                 NULL,
                                 EF_RELACHEMENT_UNTOUCH,
                                 NULL),
          )
      break;
    }
    case 2 :
    {
      BUG (EF_relachement_modif (p,
                                 relachement,
                                 NULL,
                                 EF_RELACHEMENT_UNTOUCH,
                                 NULL,
                                 EF_RELACHEMENT_UNTOUCH,
                                 NULL,
                                 type,
                                 (type == EF_RELACHEMENT_ELASTIQUE_LINEAIRE) &&
                                   (type != relachement->rz_debut) ?
                                     &elastique_lineaire :
                                     NULL,
                                 EF_RELACHEMENT_UNTOUCH,
                                 NULL,
                                 EF_RELACHEMENT_UNTOUCH,
                                 NULL,
                                 EF_RELACHEMENT_UNTOUCH,
                                 NULL),
          )
      break;
    }
    case 3 :
    {
      BUG (EF_relachement_modif (p,
                                 relachement,
                                 NULL,
                                 EF_RELACHEMENT_UNTOUCH,
                                 NULL,
                                 EF_RELACHEMENT_UNTOUCH,
                                 NULL,
                                 EF_RELACHEMENT_UNTOUCH,
                                 NULL,
                                 type,
                                 (type == EF_RELACHEMENT_ELASTIQUE_LINEAIRE) &&
                                   (type != relachement->rx_fin) ?
                                     &elastique_lineaire :
                                     NULL,
                                 EF_RELACHEMENT_UNTOUCH,
                                 NULL,
                                 EF_RELACHEMENT_UNTOUCH,
                                 NULL),
          )
      break;
    }
    case 4 :
    {
      BUG (EF_relachement_modif (p,
                                 relachement,
                                 NULL,
                                 EF_RELACHEMENT_UNTOUCH,
                                 NULL,
                                 EF_RELACHEMENT_UNTOUCH,
                                 NULL,
                                 EF_RELACHEMENT_UNTOUCH,
                                 NULL,
                                 EF_RELACHEMENT_UNTOUCH,
                                 NULL,
                                 type,
                                 (type == EF_RELACHEMENT_ELASTIQUE_LINEAIRE) &&
                                   (type != relachement->ry_fin) ?
                                     &elastique_lineaire :
                                     NULL,
                                 EF_RELACHEMENT_UNTOUCH,
                                 NULL),
          )
      break;
    }
    case 5 :
    {
      BUG (EF_relachement_modif (p,
                                 relachement,
                                 NULL,
                                 EF_RELACHEMENT_UNTOUCH,
                                 NULL,
                                 EF_RELACHEMENT_UNTOUCH,
                                 NULL,
                                 EF_RELACHEMENT_UNTOUCH,
                                 NULL,
                                 EF_RELACHEMENT_UNTOUCH,
                                 NULL,
                                 EF_RELACHEMENT_UNTOUCH,
                                 NULL,
                                 type,
                                 (type == EF_RELACHEMENT_ELASTIQUE_LINEAIRE) &&
                                   (type != relachement->rz_fin) ?
                                     &elastique_lineaire :
                                     NULL),
          )
      break;
    }
    default :
    {
      BUGPARAMCRIT (column, "%d", column, )
      break;
    }
  }
  
  return;
}


/**
 * \brief Affiche le nom du relâchement.
 * \param tree_column : composant à l'origine de l'évènement,
 * \param cell : la cellule en cours d'édition,
 * \param tree_model : le mode en cours d'édition,
 * \param iter : la ligne en cours d'édition,
 * \param data2 : la variable projet.
 * \return Rien.
 */
void
EF_gtk_relachements_render_0 (GtkTreeViewColumn *tree_column,
                              GtkCellRenderer   *cell,
                              GtkTreeModel      *tree_model,
                              GtkTreeIter       *iter,
                              gpointer           data2)
{
  EF_Relachement *relachement;
  
  gtk_tree_model_get (tree_model, iter, 0, &relachement, -1);
  BUGPARAM (relachement, "%p", relachement, )
  
  g_object_set (cell, "text", relachement->nom, NULL);
  
  return;
}


/**
 * \brief Affiche le type du relachement selon r_x au début de la barre.
 * \param tree_column : composant à l'origine de l'évènement,
 * \param cell : la cellule en cours d'édition,
 * \param tree_model : le mode en cours d'édition,
 * \param iter : la ligne en cours d'édition,
 * \param data2 : la variable projet.
 * \return Rien.
 */
void
EF_gtk_relachements_render_1 (GtkTreeViewColumn *tree_column,
                              GtkCellRenderer   *cell,
                              GtkTreeModel      *tree_model,
                              GtkTreeIter       *iter,
                              gpointer           data2)
{
  EF_Relachement *relachement;
  
  gtk_tree_model_get (tree_model, iter, 0, &relachement, -1);
  BUGPARAM (relachement, "%p", relachement, )
  
  switch (relachement->rx_debut)
  {
    case EF_RELACHEMENT_BLOQUE :
    {
      g_object_set (cell, "text", gettext ("Bloqué"), NULL);
      break;
    }
    case EF_RELACHEMENT_LIBRE :
    {
      g_object_set (cell, "text", gettext ("Libre"), NULL);
      break;
    }
    case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
    {
      g_object_set (cell, "text", gettext ("Linéaire"), NULL);
      break;
    }
    case EF_RELACHEMENT_UNTOUCH :
    default :
    {
      FAILCRIT ( , (gettext ("Le type de relâchement est inconnu.\n")); )
      break;
    }
  }
  
  return;
}


/**
 * \brief Affiche les paramètres (si nécessaire) du relachement selon r_x au
 *        début de la barre.
 * \param tree_column : composant à l'origine de l'évènement,
 * \param cell : la cellule en cours d'édition,
 * \param tree_model : le mode en cours d'édition,
 * \param iter : la ligne en cours d'édition,
 * \param data2 : la variable projet.
 * \return Rien.
 */
void
EF_gtk_relachements_render_2 (GtkTreeViewColumn *tree_column,
                              GtkCellRenderer   *cell,
                              GtkTreeModel      *tree_model,
                              GtkTreeIter       *iter,
                              gpointer           data2)
{
  EF_Relachement *relachement;
  
  gtk_tree_model_get (tree_model, iter, 0, &relachement, -1);
  BUGPARAM (relachement, "%p", relachement, )
  
  switch (relachement->rx_debut)
  {
    case EF_RELACHEMENT_LIBRE :
    case EF_RELACHEMENT_BLOQUE :
    {
      g_object_set (cell, "text", "-", NULL);
      break;
    }
    case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
    {
      EF_Relachement_Donnees_Elastique_Lineaire *data;
      char tmp[30];
      
      data = (EF_Relachement_Donnees_Elastique_Lineaire *)
                                                        relachement->rx_d_data;
      conv_f_c (data->raideur, tmp, DECIMAL_NEWTON_PAR_METRE);
      g_object_set (cell, "text", tmp, NULL);
      
      break;
    }
    case EF_RELACHEMENT_UNTOUCH :
    default :
    {
      FAILCRIT ( , (gettext ("Le type de relâchement est inconnu.\n")); )
      break;
    }
  }
  
  return;
}


/**
 * \brief Affiche le type du relachement selon r_y au début de la barre.
 * \param tree_column : composant à l'origine de l'évènement,
 * \param cell : la cellule en cours d'édition,
 * \param tree_model : le mode en cours d'édition,
 * \param iter : la ligne en cours d'édition,
 * \param data2 : la variable projet.
 * \return Rien.
 */
void
EF_gtk_relachements_render_3 (GtkTreeViewColumn *tree_column,
                              GtkCellRenderer   *cell,
                              GtkTreeModel      *tree_model,
                              GtkTreeIter       *iter,
                              gpointer           data2)
{
  EF_Relachement *relachement;
  
  gtk_tree_model_get (tree_model, iter, 0, &relachement, -1);
  BUGPARAM (relachement, "%p", relachement, )
  
  switch (relachement->ry_debut)
  {
    case EF_RELACHEMENT_BLOQUE :
    {
      g_object_set (cell, "text", gettext ("Bloqué"), NULL);
      break;
    }
    case EF_RELACHEMENT_LIBRE :
    {
      g_object_set (cell, "text", gettext ("Libre"), NULL);
      break;
    }
    case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
    {
      g_object_set (cell, "text", gettext ("Linéaire"), NULL);
      break;
    }
    case EF_RELACHEMENT_UNTOUCH :
    default :
    {
      FAILCRIT ( , (gettext ("Le type de relâchement est inconnu.\n")); )
      break;
    }
  }
  
  return;
}


/**
 * \brief Affiche les paramètres (si nécessaire) du relachement selon r_y au
 *        début de la barre.
 * \param tree_column : composant à l'origine de l'évènement,
 * \param cell : la cellule en cours d'édition,
 * \param tree_model : le mode en cours d'édition,
 * \param iter : la ligne en cours d'édition,
 * \param data2 : la variable projet.
 * \return Rien.
 */
void
EF_gtk_relachements_render_4 (GtkTreeViewColumn *tree_column,
                              GtkCellRenderer   *cell,
                              GtkTreeModel      *tree_model,
                              GtkTreeIter       *iter,
                              gpointer           data2)
{
  EF_Relachement *relachement;
  
  gtk_tree_model_get (tree_model, iter, 0, &relachement, -1);
  BUGPARAM (relachement, "%p", relachement, )
  
  switch (relachement->ry_debut)
  {
    case EF_RELACHEMENT_LIBRE :
    case EF_RELACHEMENT_BLOQUE :
    {
      g_object_set (cell, "text", "-", NULL);
      break;
    }
    case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
    {
      EF_Relachement_Donnees_Elastique_Lineaire *data;
      char tmp[30];
      
      data = (EF_Relachement_Donnees_Elastique_Lineaire *)
                                                        relachement->ry_d_data;
      conv_f_c (data->raideur, tmp, DECIMAL_NEWTON_PAR_METRE);
      g_object_set (cell, "text", tmp, NULL);
      
      break;
    }
    case EF_RELACHEMENT_UNTOUCH :
    default :
    {
      FAILCRIT ( , (gettext ("Le type de relâchement est inconnu.\n")); )
      break;
    }
  }
  
  return;
}


/**
 * \brief Affiche le type du relachement selon r_z au début de la barre.
 * \param tree_column : composant à l'origine de l'évènement,
 * \param cell : la cellule en cours d'édition,
 * \param tree_model : le mode en cours d'édition,
 * \param iter : la ligne en cours d'édition,
 * \param data2 : la variable projet.
 * \return Rien.
 */
void
EF_gtk_relachements_render_5 (GtkTreeViewColumn *tree_column,
                              GtkCellRenderer   *cell,
                              GtkTreeModel      *tree_model,
                              GtkTreeIter       *iter,
                              gpointer           data2)
{
  EF_Relachement *relachement;
  
  gtk_tree_model_get (tree_model, iter, 0, &relachement, -1);
  BUGPARAM (relachement, "%p", relachement, )
  
  switch (relachement->rz_debut)
  {
    case EF_RELACHEMENT_BLOQUE :
    {
      g_object_set (cell, "text", gettext ("Bloqué"), NULL);
      break;
    }
    case EF_RELACHEMENT_LIBRE :
    {
      g_object_set (cell, "text", gettext ("Libre"), NULL);
      break;
    }
    case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
    {
      g_object_set (cell, "text", gettext ("Linéaire"), NULL);
      break;
    }
    case EF_RELACHEMENT_UNTOUCH :
    default :
    {
      FAILCRIT ( , (gettext ("Le type de relâchement est inconnu.\n")); )
      break;
    }
  }
  
  return;
}


/**
 * \brief Affiche les paramètres (si nécessaire) du relachement selon r_z au
 *        début de la barre.
 * \param tree_column : composant à l'origine de l'évènement,
 * \param cell : la cellule en cours d'édition,
 * \param tree_model : le mode en cours d'édition,
 * \param iter : la ligne en cours d'édition,
 * \param data2 : la variable projet.
 * \return Rien.
 */
void
EF_gtk_relachements_render_6 (GtkTreeViewColumn *tree_column,
                              GtkCellRenderer   *cell,
                              GtkTreeModel      *tree_model,
                              GtkTreeIter       *iter,
                              gpointer           data2)
{
  EF_Relachement *relachement;
  
  gtk_tree_model_get (tree_model, iter, 0, &relachement, -1);
  BUGPARAM (relachement, "%p", relachement, )
  
  switch (relachement->rz_debut)
  {
    case EF_RELACHEMENT_LIBRE :
    case EF_RELACHEMENT_BLOQUE :
    {
      g_object_set (cell, "text", "-", NULL);
      break;
    }
    case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
    {
      EF_Relachement_Donnees_Elastique_Lineaire *data;
      char tmp[30];
      
      data = (EF_Relachement_Donnees_Elastique_Lineaire *)
                                                        relachement->rz_d_data;
      conv_f_c (data->raideur, tmp, DECIMAL_NEWTON_PAR_METRE);
      g_object_set (cell, "text", tmp, NULL);
      
      break;
    }
    case EF_RELACHEMENT_UNTOUCH :
    default :
    {
      FAILCRIT ( , (gettext ("Le type de relâchement est inconnu.\n")); )
      break;
    }
  }
  
  return;
}


/**
 * \brief Affiche le type du relachement selon r_x à la fin de la barre.
 * \param tree_column : composant à l'origine de l'évènement,
 * \param cell : la cellule en cours d'édition,
 * \param tree_model : le mode en cours d'édition,
 * \param iter : la ligne en cours d'édition,
 * \param data2 : la variable projet.
 * \return Rien.
 */
void
EF_gtk_relachements_render_7 (GtkTreeViewColumn *tree_column,
                              GtkCellRenderer   *cell,
                              GtkTreeModel      *tree_model,
                              GtkTreeIter       *iter,
                              gpointer           data2)
{
  EF_Relachement *relachement;
  
  gtk_tree_model_get (tree_model, iter, 0, &relachement, -1);
  BUGPARAM (relachement, "%p", relachement, )
  
  switch (relachement->rx_fin)
  {
    case EF_RELACHEMENT_BLOQUE :
    {
      g_object_set (cell, "text", gettext ("Bloqué"), NULL);
      break;
    }
    case EF_RELACHEMENT_LIBRE :
    {
      g_object_set (cell, "text", gettext ("Libre"), NULL);
      break;
    }
    case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
    {
      g_object_set (cell, "text", gettext ("Linéaire"), NULL);
      break;
    }
    case EF_RELACHEMENT_UNTOUCH :
    default :
    {
      FAILCRIT ( , (gettext ("Le type de relâchement est inconnu.\n")); )
      break;
    }
  }
  
  return;
}


/**
 * \brief Affiche les paramètres (si nécessaire) du relachement selon r_x à la
 *        fin de la barre.
 * \param tree_column : composant à l'origine de l'évènement,
 * \param cell : la cellule en cours d'édition,
 * \param tree_model : le mode en cours d'édition,
 * \param iter : la ligne en cours d'édition,
 * \param data2 : la variable projet.
 * \return Rien.
 */
void
EF_gtk_relachements_render_8 (GtkTreeViewColumn *tree_column,
                              GtkCellRenderer   *cell,
                              GtkTreeModel      *tree_model,
                              GtkTreeIter       *iter,
                              gpointer           data2)
{
  EF_Relachement *relachement;
  
  gtk_tree_model_get (tree_model, iter, 0, &relachement, -1);
  BUGPARAM (relachement, "%p", relachement, )
  
  switch (relachement->rx_fin)
  {
    case EF_RELACHEMENT_LIBRE :
    case EF_RELACHEMENT_BLOQUE :
    {
      g_object_set (cell, "text", "-", NULL);
      break;
    }
    case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
    {
      EF_Relachement_Donnees_Elastique_Lineaire *data;
      char tmp[30];
      
      data = (EF_Relachement_Donnees_Elastique_Lineaire *)
                                                        relachement->rx_f_data;
      conv_f_c (data->raideur, tmp, DECIMAL_NEWTON_PAR_METRE);
      g_object_set (cell, "text", tmp, NULL);
      
      break;
    }
    case EF_RELACHEMENT_UNTOUCH :
    default :
    {
      FAILCRIT ( , (gettext ("Le type de relâchement est inconnu.\n")); )
      break;
    }
  }
  
  return;
}


/**
 * \brief Affiche le type du relachement selon r_y à la fin de la barre.
 * \param tree_column : composant à l'origine de l'évènement,
 * \param cell : la cellule en cours d'édition,
 * \param tree_model : le mode en cours d'édition,
 * \param iter : la ligne en cours d'édition,
 * \param data2 : la variable projet.
 * \return Rien.
 */
void
EF_gtk_relachements_render_9 (GtkTreeViewColumn *tree_column,
                              GtkCellRenderer   *cell,
                              GtkTreeModel      *tree_model,
                              GtkTreeIter       *iter,
                              gpointer           data2)
{
  EF_Relachement *relachement;
  
  gtk_tree_model_get (tree_model, iter, 0, &relachement, -1);
  BUGPARAM (relachement, "%p", relachement, )
  
  switch (relachement->ry_fin)
  {
    case EF_RELACHEMENT_BLOQUE :
    {
      g_object_set (cell, "text", gettext ("Bloqué"), NULL);
      break;
    }
    case EF_RELACHEMENT_LIBRE :
    {
      g_object_set (cell, "text", gettext ("Libre"), NULL);
      break;
    }
    case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
    {
      g_object_set (cell, "text", gettext ("Linéaire"), NULL);
      break;
    }
    case EF_RELACHEMENT_UNTOUCH :
    default :
    {
      FAILCRIT ( , (gettext ("Le type de relâchement est inconnu.\n")); )
      break;
    }
  }
  
  return;
}


/**
 * \brief Affiche les paramètres (si nécessaire) du relachement selon r_y à la
 *        fin de la barre.
 * \param tree_column : composant à l'origine de l'évènement,
 * \param cell : la cellule en cours d'édition,
 * \param tree_model : le mode en cours d'édition,
 * \param iter : la ligne en cours d'édition,
 * \param data2 : la variable projet.
 * \return Rien.
 */
void
EF_gtk_relachements_render_10 (GtkTreeViewColumn *tree_column,
                               GtkCellRenderer   *cell,
                               GtkTreeModel      *tree_model,
                               GtkTreeIter       *iter,
                               gpointer           data2)
{
  EF_Relachement *relachement;
  
  gtk_tree_model_get (tree_model, iter, 0, &relachement, -1);
  BUGPARAM (relachement, "%p", relachement, )
  
  switch (relachement->ry_fin)
  {
    case EF_RELACHEMENT_LIBRE :
    case EF_RELACHEMENT_BLOQUE :
    {
      g_object_set (cell, "text", "-", NULL);
      break;
    }
    case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
    {
      EF_Relachement_Donnees_Elastique_Lineaire *data;
      char tmp[30];
      
      data = (EF_Relachement_Donnees_Elastique_Lineaire *)
                                                        relachement->ry_f_data;
      conv_f_c (data->raideur, tmp, DECIMAL_NEWTON_PAR_METRE);
      g_object_set (cell, "text", tmp, NULL);
      
      break;
    }
    case EF_RELACHEMENT_UNTOUCH :
    default :
    {
      FAILCRIT ( , (gettext ("Le type de relâchement est inconnu.\n")); )
      break;
    }
  }
  
  return;
}


/**
 * \brief Affiche le type du relachement selon r_z à la fin de la barre.
 * \param tree_column : composant à l'origine de l'évènement,
 * \param cell : la cellule en cours d'édition,
 * \param tree_model : le mode en cours d'édition,
 * \param iter : la ligne en cours d'édition,
 * \param data2 : la variable projet.
 * \return Rien.
 */
void
EF_gtk_relachements_render_11 (GtkTreeViewColumn *tree_column,
                               GtkCellRenderer   *cell,
                               GtkTreeModel      *tree_model,
                               GtkTreeIter       *iter,
                               gpointer           data2)
{
  EF_Relachement *relachement;
  
  gtk_tree_model_get (tree_model, iter, 0, &relachement, -1);
  BUGPARAM (relachement, "%p", relachement, )
  
  switch (relachement->rz_fin)
  {
    case EF_RELACHEMENT_BLOQUE :
    {
      g_object_set (cell, "text", gettext ("Bloqué"), NULL);
      break;
    }
    case EF_RELACHEMENT_LIBRE :
    {
      g_object_set (cell, "text", gettext ("Libre"), NULL);
      break;
    }
    case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
    {
      g_object_set (cell, "text", gettext ("Linéaire"), NULL);
      break;
    }
    case EF_RELACHEMENT_UNTOUCH :
    default :
    {
      FAILCRIT ( , (gettext ("Le type de relâchement est inconnu.\n")); )
      break;
    }
  }
  
  return;
}


/**
 * \brief Affiche les paramètres (si nécessaire) du relachement selon r_z à la
 *        fin de la barre.
 * \param tree_column : composant à l'origine de l'évènement,
 * \param cell : la cellule en cours d'édition,
 * \param tree_model : le mode en cours d'édition,
 * \param iter : la ligne en cours d'édition,
 * \param data2 : la variable projet.
 * \return Rien.
 */
void
EF_gtk_relachements_render_12 (GtkTreeViewColumn *tree_column,
                               GtkCellRenderer   *cell,
                               GtkTreeModel      *tree_model,
                               GtkTreeIter       *iter,
                               gpointer           data2)
{
  EF_Relachement *relachement;
  
  gtk_tree_model_get (tree_model, iter, 0, &relachement, -1);
  BUGPARAM (relachement, "%p", relachement, )
  
  switch (relachement->rz_fin)
  {
    case EF_RELACHEMENT_LIBRE :
    case EF_RELACHEMENT_BLOQUE :
    {
      g_object_set (cell, "text", "-", NULL);
      break;
    }
    case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
    {
      EF_Relachement_Donnees_Elastique_Lineaire *data;
      char tmp[30];
      
      data = (EF_Relachement_Donnees_Elastique_Lineaire *)
                                                        relachement->rz_f_data;
      conv_f_c (data->raideur, tmp, DECIMAL_NEWTON_PAR_METRE);
      g_object_set (cell, "text", tmp, NULL);
      
      break;
    }
    case EF_RELACHEMENT_UNTOUCH :
    default :
    {
      FAILCRIT ( , (gettext ("Le type de relâchement est inconnu.\n")); )
      break;
    }
  }
  
  return;
}


/**
 * \brief Création de la fenêtre permettant d'afficher et d'éditer les
 *        relâchements sous forme d'un tableau.
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique impossible à générer.
 */
void
EF_gtk_relachement (Projet *p)
{
  std::list <EF_Relachement *>::iterator it;
  
  BUGPARAMCRIT (p, "%p", p, )
  
  if (UI_REL.builder != NULL)
  {
    gtk_window_present (GTK_WINDOW (UI_REL.window));
    return;
  }
  
  UI_REL.builder = gtk_builder_new ();
  BUGCRIT (gtk_builder_add_from_resource (UI_REL.builder,
                                     "/org/2lgc/codegui/ui/EF_relachements.ui",
                                          NULL) != 0,
           ,
           (gettext ("La génération de la fenêtre %s a échouée.\n"),
                     "Relachement"); )
  gtk_builder_connect_signals (UI_REL.builder, p);
  
  g_object_set_data (gtk_builder_get_object (UI_REL.builder,
                                             "EF_relachements_treeview_cell1"),
                     "column",
                     GINT_TO_POINTER (0));
  g_object_set_data (gtk_builder_get_object (UI_REL.builder,
                                             "EF_relachements_treeview_cell2"),
                     "column",
                     GINT_TO_POINTER (0));
  g_object_set_data (gtk_builder_get_object (UI_REL.builder,
                                             "EF_relachements_treeview_cell3"),
                     "column",
                     GINT_TO_POINTER (1));
  g_object_set_data (gtk_builder_get_object (UI_REL.builder,
                                             "EF_relachements_treeview_cell4"),
                     "column",
                     GINT_TO_POINTER (1));
  g_object_set_data (gtk_builder_get_object (UI_REL.builder,
                                             "EF_relachements_treeview_cell5"),
                     "column",
                     GINT_TO_POINTER (2));
  g_object_set_data (gtk_builder_get_object (UI_REL.builder,
                                             "EF_relachements_treeview_cell6"),
                     "column",
                     GINT_TO_POINTER (2));
  g_object_set_data (gtk_builder_get_object (UI_REL.builder,
                                             "EF_relachements_treeview_cell7"),
                     "column",
                     GINT_TO_POINTER (3));
  g_object_set_data (gtk_builder_get_object (UI_REL.builder,
                                             "EF_relachements_treeview_cell8"),
                     "column",
                     GINT_TO_POINTER (3));
  g_object_set_data (gtk_builder_get_object (UI_REL.builder,
                                             "EF_relachements_treeview_cell9"),
                     "column",
                     GINT_TO_POINTER (4));
  g_object_set_data (gtk_builder_get_object (UI_REL.builder,
                                            "EF_relachements_treeview_cell10"),
                     "column",
                     GINT_TO_POINTER (4));
  g_object_set_data (gtk_builder_get_object (UI_REL.builder,
                                            "EF_relachements_treeview_cell11"),
                     "column",
                     GINT_TO_POINTER (5));
  g_object_set_data (gtk_builder_get_object (UI_REL.builder,
                                            "EF_relachements_treeview_cell12"),
                     "column",
                     GINT_TO_POINTER (5));
  
  gtk_tree_view_column_set_cell_data_func (
    GTK_TREE_VIEW_COLUMN ( gtk_builder_get_object (UI_REL.builder,
                                          "EF_relachements_treeview_column0")),
    GTK_CELL_RENDERER (gtk_builder_get_object (UI_REL.builder,
                                            "EF_relachements_treeview_cell0")),
    EF_gtk_relachements_render_0,
    p,
    NULL);
  gtk_tree_view_column_set_cell_data_func (
    GTK_TREE_VIEW_COLUMN (gtk_builder_get_object (UI_REL.builder,
                                          "EF_relachements_treeview_column1")),
    GTK_CELL_RENDERER (gtk_builder_get_object (UI_REL.builder,
                                            "EF_relachements_treeview_cell1")),
    EF_gtk_relachements_render_1,
    p,
    NULL);
  gtk_tree_view_column_set_cell_data_func (
    GTK_TREE_VIEW_COLUMN ( gtk_builder_get_object (UI_REL.builder,
                                          "EF_relachements_treeview_column2")),
    GTK_CELL_RENDERER (gtk_builder_get_object (UI_REL.builder,
                                            "EF_relachements_treeview_cell2")),
    EF_gtk_relachements_render_2,
    p,
    NULL);
  gtk_tree_view_column_set_cell_data_func (
    GTK_TREE_VIEW_COLUMN (gtk_builder_get_object (UI_REL.builder,
                                          "EF_relachements_treeview_column3")),
    GTK_CELL_RENDERER (gtk_builder_get_object (UI_REL.builder,
                                            "EF_relachements_treeview_cell3")),
    EF_gtk_relachements_render_3,
    p,
    NULL);
  gtk_tree_view_column_set_cell_data_func (
    GTK_TREE_VIEW_COLUMN (gtk_builder_get_object (UI_REL.builder,
                                          "EF_relachements_treeview_column4")),
    GTK_CELL_RENDERER (gtk_builder_get_object (UI_REL.builder,
                                            "EF_relachements_treeview_cell4")),
    EF_gtk_relachements_render_4,
    p,
    NULL);
  gtk_tree_view_column_set_cell_data_func (
    GTK_TREE_VIEW_COLUMN (gtk_builder_get_object (UI_REL.builder,
                                          "EF_relachements_treeview_column5")),
    GTK_CELL_RENDERER (gtk_builder_get_object (UI_REL.builder,
                                            "EF_relachements_treeview_cell5")),
    EF_gtk_relachements_render_5,
    p,
    NULL);
  gtk_tree_view_column_set_cell_data_func (
    GTK_TREE_VIEW_COLUMN (gtk_builder_get_object (UI_REL.builder,
                                          "EF_relachements_treeview_column6")),
    GTK_CELL_RENDERER (gtk_builder_get_object (UI_REL.builder,
                                            "EF_relachements_treeview_cell6")),
    EF_gtk_relachements_render_6,
    p,
    NULL);
  gtk_tree_view_column_set_cell_data_func (
    GTK_TREE_VIEW_COLUMN (gtk_builder_get_object (UI_REL.builder,
                                          "EF_relachements_treeview_column7")),
    GTK_CELL_RENDERER (gtk_builder_get_object (UI_REL.builder,
                                            "EF_relachements_treeview_cell7")),
    EF_gtk_relachements_render_7,
    p,
    NULL);
  gtk_tree_view_column_set_cell_data_func (
    GTK_TREE_VIEW_COLUMN (gtk_builder_get_object (UI_REL.builder,
                                          "EF_relachements_treeview_column8")),
    GTK_CELL_RENDERER (gtk_builder_get_object (UI_REL.builder,
                                            "EF_relachements_treeview_cell8")),
    EF_gtk_relachements_render_8,
    p,
    NULL);
  gtk_tree_view_column_set_cell_data_func (
    GTK_TREE_VIEW_COLUMN (gtk_builder_get_object (UI_REL.builder,
                                          "EF_relachements_treeview_column9")),
    GTK_CELL_RENDERER (gtk_builder_get_object (UI_REL.builder,
                                            "EF_relachements_treeview_cell9")),
    EF_gtk_relachements_render_9,
    p,
    NULL);
  gtk_tree_view_column_set_cell_data_func (
    GTK_TREE_VIEW_COLUMN (gtk_builder_get_object (UI_REL.builder,
                                         "EF_relachements_treeview_column10")),
    GTK_CELL_RENDERER (gtk_builder_get_object (UI_REL.builder,
                                           "EF_relachements_treeview_cell10")),
    EF_gtk_relachements_render_10,
    p,
    NULL);
  gtk_tree_view_column_set_cell_data_func (
    GTK_TREE_VIEW_COLUMN (gtk_builder_get_object (UI_REL.builder,
                                         "EF_relachements_treeview_column11")),
    GTK_CELL_RENDERER (gtk_builder_get_object (UI_REL.builder,
                                           "EF_relachements_treeview_cell11")),
    EF_gtk_relachements_render_11,
    p,
    NULL);
  gtk_tree_view_column_set_cell_data_func (
    GTK_TREE_VIEW_COLUMN (gtk_builder_get_object (UI_REL.builder,
                                         "EF_relachements_treeview_column12")),
    GTK_CELL_RENDERER (gtk_builder_get_object (UI_REL.builder,
                                           "EF_relachements_treeview_cell12")),
    EF_gtk_relachements_render_12,
    p,
    NULL);
  
  UI_REL.window = GTK_WIDGET (gtk_builder_get_object (UI_REL.builder,
                                                    "EF_relachements_window"));
  UI_REL.relachements = GTK_TREE_STORE (gtk_builder_get_object (UI_REL.builder,
                                                 "EF_relachements_treestore"));
  
  it = p->modele.relachements.begin ();
  while (it != p->modele.relachements.end ())
  {
    EF_Relachement *relachement = *it;
    
    gtk_tree_store_append (UI_REL.relachements,
                           &relachement->Iter_fenetre,
                           NULL);
    gtk_tree_store_set (UI_REL.relachements,
                        &relachement->Iter_fenetre,
                        0, relachement,
                        -1);
    
    ++it;
  }
  
  gtk_window_set_transient_for (GTK_WINDOW (UI_REL.window),
                                GTK_WINDOW (UI_GTK.window));
}

#endif

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
