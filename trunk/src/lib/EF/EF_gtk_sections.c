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
#include <math.h>

#include "common_m3d.hpp"

#include "common_projet.h"
#include "common_erreurs.h"
#include "common_gtk.h"
#include "common_math.h"
#include "common_text.h"
#include "common_selection.h"
#include "EF_sections.h"
#include "EF_gtk_section_rectangulaire.h"
#include "EF_gtk_section_T.h"
#include "EF_gtk_section_carree.h"
#include "EF_gtk_section_circulaire.h"
#include "EF_gtk_section_personnalisee.h"
#include "1992_1_1_barres.h"


GTK_WINDOW_CLOSE (ef, sections);


GTK_WINDOW_DESTROY (ef, sections, );


GTK_WINDOW_KEY_PRESS (ef, sections);


gboolean
EF_gtk_sections_treeview_key_press (GtkTreeView *treeview,
                                    GdkEvent    *event,
                                    Projet      *p)
/**
 * \brief Supprime une section sans dépendance si la touche SUPPR est appuyée.
 * \param treeview : composant à l'origine de l'évènement,
 * \param event : Caractéristique de l'évènement,
 * \param p : la variable projet.
 * \return
 *   Succès : TRUE si la touche SUPPR est pressée, FALSE sinon.\n
 *   Echec : FALSE
 *     - p == NULL,
 *     - interface graphique non initialisée.
 *  
 */
{
  BUGPARAMCRIT (p, "%p", p, FALSE)
  BUGCRIT (UI_SEC.builder,
           FALSE,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Sections");)
  
  if (event->key.keyval == GDK_KEY_Delete)
  {
    GtkTreeIter   Iter;
    GtkTreeModel *model;
    Section      *section;
    GList        *liste_sections = NULL;
    
    if (!gtk_tree_selection_get_selected (
           gtk_tree_view_get_selection (treeview),
           &model,
           &Iter))
      return FALSE;
    
    gtk_tree_model_get (model, &Iter, 0, &section, -1);
    
    liste_sections = g_list_append (liste_sections, section);
    
    if (_1992_1_1_barres_cherche_dependances (p,
                                              NULL,
                                              NULL,
                                              liste_sections,
                                              NULL,
                                              NULL,
                                              NULL,
                                              NULL,
                                              NULL,
                                              NULL,
                                              FALSE,
                                              FALSE) == FALSE)
      BUG (EF_sections_supprime (section, TRUE, p),
           FALSE,
           g_list_free (liste_sections);)
    
    g_list_free (liste_sections);
    
    return TRUE;
  }
  else
    return FALSE;
}


void
EF_gtk_sections_select_changed (GtkTreeSelection *treeselection,
                                Projet           *p)
/**
 * \brief En fonction de la sélection, active ou désactive le bouton supprimer.
 * \param treeselection : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
{
  GtkTreeModel *model;
  GtkTreeIter   Iter;
  
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_SEC.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Section");)
  
  // Si aucune section n'est sélectionnée, il n'est pas possible d'en supprimer
  // ou d'en éditer une.
  if (!gtk_tree_selection_get_selected (GTK_TREE_SELECTION (
                                        gtk_builder_get_object (UI_SEC.builder,
                                               "EF_sections_treeview_select")),
                                        &model,
                                        &Iter))
  {
    gtk_widget_set_sensitive (GTK_WIDGET ( gtk_builder_get_object (
                      UI_SEC.builder, "EF_sections_boutton_supprimer_direct")),
                              FALSE);
    gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
                        UI_SEC.builder, "EF_sections_boutton_supprimer_menu")),
                              FALSE);
    gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
                              UI_SEC.builder, "EF_sections_boutton_modifier")),
                              FALSE);
    gtk_widget_set_visible (GTK_WIDGET (gtk_builder_get_object (UI_SEC.builder,
                                      "EF_sections_boutton_supprimer_direct")),
                            TRUE);
    gtk_widget_set_visible (GTK_WIDGET (gtk_builder_get_object (UI_SEC.builder,
                                        "EF_sections_boutton_supprimer_menu")),
                            FALSE);
  }
  else
  {
    Section *section;
    GList   *liste_sections = NULL;
    
    gtk_tree_model_get (model, &Iter, 0, &section, -1);
    
    liste_sections = g_list_append (liste_sections, section);
    gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
                              UI_SEC.builder, "EF_sections_boutton_modifier")),
                              TRUE);
    if (_1992_1_1_barres_cherche_dependances (p,
                                              NULL,
                                              NULL,
                                              liste_sections,
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
                      UI_SEC.builder, "EF_sections_boutton_supprimer_direct")),
                                FALSE);
      gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
                        UI_SEC.builder, "EF_sections_boutton_supprimer_menu")),
                                TRUE);
      gtk_widget_set_visible (GTK_WIDGET (gtk_builder_get_object (
                      UI_SEC.builder, "EF_sections_boutton_supprimer_direct")),
                              FALSE);
      gtk_widget_set_visible (GTK_WIDGET (gtk_builder_get_object (
                        UI_SEC.builder, "EF_sections_boutton_supprimer_menu")),
                              TRUE);
    }
    else
    {
      gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
                      UI_SEC.builder, "EF_sections_boutton_supprimer_direct")),
                                TRUE);
      gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
                        UI_SEC.builder, "EF_sections_boutton_supprimer_menu")),
                                FALSE);
      gtk_widget_set_visible (GTK_WIDGET (gtk_builder_get_object (
                      UI_SEC.builder, "EF_sections_boutton_supprimer_direct")),
                              TRUE);
      gtk_widget_set_visible (GTK_WIDGET (gtk_builder_get_object (
                        UI_SEC.builder, "EF_sections_boutton_supprimer_menu")),
                              FALSE);
    }
    
    g_list_free (liste_sections);
  }
  
  return;
}


void
EF_gtk_sections_boutton_supprimer_menu (GtkButton *widget,
                                        Projet    *p)
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
{
  GtkTreeModel *model;
  GtkTreeIter   Iter;
  Section      *section;
  GList        *liste_sections = NULL;
  GList        *liste_noeuds_dep, *liste_barres_dep, *liste_charges_dep;
  
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_SEC.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Section");)
  
  // Si aucune section n'est sélectionnée
  BUGCRIT (gtk_tree_selection_get_selected (GTK_TREE_SELECTION (
       gtk_builder_get_object (UI_SEC.builder, "EF_sections_treeview_select")),
                                        &model,
                                        &Iter),
           ,
           (gettext ("Aucun élément n'est sélectionné.\n"));)
  
  gtk_tree_model_get (model, &Iter, 0, &section, -1);
  
  liste_sections = g_list_append (liste_sections, section);
  BUG (_1992_1_1_barres_cherche_dependances (p,
                                             NULL,
                                             NULL,
                                             liste_sections,
                                             NULL,
                                             NULL,
                                             NULL,
                                             &liste_noeuds_dep,
                                             &liste_barres_dep,
                                             &liste_charges_dep,
                                             FALSE,
                                             FALSE),
      ,
      g_list_free (liste_sections);)
  g_list_free (liste_sections);
  
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
                         UI_SEC.builder, "EF_sections_supprimer_menu_barres")),
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
EF_gtk_sections_edit_nom (GtkCellRendererText *cell,
                          gchar               *path_string,
                          gchar               *new_text,
                          Projet              *p)
/**
 * \brief Modification du nom d'une section.
 * \param cell : cellule en cours,
 * \param path_string : path de la ligne en cours,
 * \param new_text : le nouveau nom,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée.
*/
{
  GtkTreeModel *model;
  GtkTreeIter   iter;
  GtkTreePath  *path;
  Section      *section;
  
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_SEC.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Section");)
  
  model = GTK_TREE_MODEL (UI_SEC.sections);
  path = gtk_tree_path_new_from_string (path_string);
  gtk_tree_model_get_iter (model, &iter, path);
  gtk_tree_path_free (path);
  gtk_tree_model_get (model, &iter, 0, &section, -1);
  if ((strcmp (section->nom, new_text) == 0) || (strcmp (new_text, "") == 0))
    return;
  if (EF_sections_cherche_nom (p, new_text, FALSE))
    return;

  switch (section->type)
  {
    case SECTION_RECTANGULAIRE :
    {
      BUG (EF_sections_rectangulaire_modif (p,
                                            section,
                                            new_text,
                                            m_f (NAN, FLOTTANT_ORDINATEUR),
                                            m_f (NAN, FLOTTANT_ORDINATEUR)),
          )
      break;
    }
    case SECTION_T :
    {
      BUG (EF_sections_T_modif (p,
                                section,
                                new_text,
                                m_f (NAN, FLOTTANT_ORDINATEUR),
                                m_f (NAN, FLOTTANT_ORDINATEUR),
                                m_f (NAN, FLOTTANT_ORDINATEUR),
                                m_f (NAN, FLOTTANT_ORDINATEUR)),
          )
      break;
    }
    case SECTION_CARREE :
    {
      BUG (EF_sections_carree_modif (p,
                                     section,
                                     new_text,
                                     m_f (NAN, FLOTTANT_ORDINATEUR)),
          )
      break;
    }
    case SECTION_CIRCULAIRE :
    {
      BUG (EF_sections_circulaire_modif (p,
                                         section,
                                         new_text,
                                         m_f (NAN, FLOTTANT_ORDINATEUR)),
          )
      break;
    }
    case SECTION_PERSONNALISEE :
    {
      BUG (EF_sections_personnalisee_modif (p,
                                            section,
                                            new_text,
                                            NULL,
                                            m_f (NAN, FLOTTANT_ORDINATEUR),
                                            m_f (NAN, FLOTTANT_ORDINATEUR),
                                            m_f (NAN, FLOTTANT_ORDINATEUR),
                                            m_f (NAN, FLOTTANT_ORDINATEUR),
                                            m_f (NAN, FLOTTANT_ORDINATEUR),
                                            m_f (NAN, FLOTTANT_ORDINATEUR),
                                            m_f (NAN, FLOTTANT_ORDINATEUR),
                                            m_f (NAN, FLOTTANT_ORDINATEUR),
                                            NULL),
          )
      break;
    }
    default :
    {
      FAILCRIT ( , (gettext ("Type de section %d inconnu.\n"), section->type);)
      break;
    }
  }
  
  return;
}


void
EF_gtk_sections_supprimer_direct (GtkButton *button,
                                  Projet    *p)
/**
 * \brief Supprime la section sélectionnée dans le treeview.
 * \param button : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
{
  GtkTreeIter   iter;
  GtkTreeModel *model;
  Section      *section;
  
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_SEC.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Section");)
  
  if (!gtk_tree_selection_get_selected (GTK_TREE_SELECTION (
       gtk_builder_get_object (UI_SEC.builder, "EF_sections_treeview_select")),
                                        &model,
                                        &iter))
    return;
  
  gtk_tree_model_get (model, &iter, 0, &section, -1);
  
  BUG (EF_sections_supprime (section, TRUE, p), )
  
  return;
}


void
EF_gtk_sections_supprimer_menu_barres (GtkButton *button,
                                       Projet    *p)
/**
 * \brief Supprime la section sélectionnée dans le treeview, y compris les
 *        barres l'utilisant.
 * \param button : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
{
  GtkTreeIter   iter;
  GtkTreeModel *model;
  Section      *section;
  
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_SEC.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Section");)
  
  if (!gtk_tree_selection_get_selected (GTK_TREE_SELECTION (
       gtk_builder_get_object (UI_SEC.builder, "EF_sections_treeview_select")),
                                        &model,
                                        &iter))
    return;
  
  gtk_tree_model_get (model, &iter, 0, &section, -1);
  
  BUG (EF_sections_supprime (section, FALSE, p), )
  
  BUG (m3d_rafraichit (p), )
  
  return;
}


GdkPixbuf *
EF_gtk_sections_dessin (Section *section,
                        int      width,
                        int      height)
/**
 * \brief Renvoie un dessin représentant la section.
 * \param section : la section à dessiner,
 * \param width : la largeur du dessin,
 * \param height : la hauteur du dessin.
 * \return Le dessin au format GdkPixbuf.\n
 *   Echec :
 *     - section == NULL,
 *     - width == 0,
 *     - height == 0,
 *     - en cas d'erreur d'allocation mémoire.
 */
{
  int              rowstride, n_channels;
  int              x, y;
  guchar          *pixels, *p;
  GdkPixbuf       *pixbuf;
  double           a;
  cairo_surface_t *surface;
  cairo_t         *cr;
  double           convert;
  cairo_path_t    *save_path;
  
  BUGPARAMCRIT (section, "%p", section, NULL)
  INFO (width,
        NULL,
        (gettext ("La largeur du dessin ne peut être nulle.\n"));)
  INFO (height,
        NULL,
        (gettext ("La hauteur du dessin ne peut être nulle.\n"));)
  
  surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, width, height);
  BUGCRIT (cairo_surface_status (surface) == CAIRO_STATUS_SUCCESS,
           NULL,
           (gettext ("Erreur d'allocation mémoire.\n"));)
  cr = cairo_create (surface);
  
  a = (double) width / height;
  pixbuf = gdk_pixbuf_new (GDK_COLORSPACE_RGB, TRUE, 8, width, height);
  pixels = gdk_pixbuf_get_pixels (pixbuf);
  rowstride = gdk_pixbuf_get_rowstride (pixbuf);
  n_channels = gdk_pixbuf_get_n_channels (pixbuf);
  
  cairo_set_antialias (cr, CAIRO_ANTIALIAS_NONE);
  cairo_set_source_rgba (cr, 1., 1., 1., 0.);
  cairo_rectangle (cr, 0, 0, width, height);
  cairo_fill (cr);
    
  // On replie tout avec un fond blanc
  for (y = 0; y < height; y++)
    for (x = 0; x < width; x++)
    {
      p = pixels + y * rowstride + x * n_channels;
      p[0] = 255;
      p[1] = 255;
      p[2] = 255;
      if (n_channels == 4)
        p[3] = 0;
    }
  
  switch (section->type)
  {
    case SECTION_RECTANGULAIRE :
    case SECTION_T :
    {
      Section_T *data = (Section_T *)section->data;
      
      double     lt = m_g (data->largeur_table);
      double     lr = m_g (data->largeur_retombee);
      double     ht = m_g (data->hauteur_table);
      double     hr = m_g (data->hauteur_retombee);
      double     aa = MAX (lt, lr) / (ht + hr);
      
      cairo_set_source_rgba (cr, 0.8, 0.8, 0.8, 1.);
      cairo_new_path (cr);
      
      // Le schéma prend toute la largeur
      if (aa > a)
        convert = (width - 1) / MAX (lt, lr);
      else
        convert = (height - 1) / (ht + hr);
      cairo_move_to (cr,
                     width / 2. - lt / 2. * convert,
                     height / 2. - (ht + hr) / 2. * convert);
      cairo_rel_line_to (cr, 0.,                       ht * convert);
      cairo_rel_line_to (cr, (lt - lr) / 2. * convert, 0.);
      cairo_rel_line_to (cr, 0.,                       hr * convert);
      cairo_rel_line_to (cr, lr * convert,             0.);
      cairo_rel_line_to (cr, 0.,                       -hr * convert);
      cairo_rel_line_to (cr, (lt - lr) / 2. * convert, 0.);
      cairo_rel_line_to (cr, 0.,                       -ht * convert);
      cairo_close_path (cr);
      save_path = cairo_copy_path (cr);
      cairo_fill (cr);
      cairo_set_source_rgba (cr, 0., 0., 0., 1.);
      cairo_set_line_width (cr, 1.);
      cairo_new_path (cr);
      cairo_append_path (cr, save_path);
      cairo_stroke (cr);
      
      cairo_path_destroy (save_path);
      
      break;
    }
    case SECTION_CARREE :
    {
      cairo_set_source_rgba (cr, 0.8, 0.8, 0.8, 1.);
      
      if (a < 1)
      {
        double y_h = height / 2. - width / 2.;
        double y_b = height / 2. + width / 2.;
        
        cairo_rectangle (cr, 1, y_h + 1, width - 1, y_b - y_h - 1);
        cairo_fill (cr);
        cairo_set_source_rgba (cr, 0., 0., 0., 1.);
        cairo_set_line_width (cr, 1.);
        cairo_rectangle (cr, 1, y_h + 1, width - 1, y_b - y_h - 1);
      }
      else
      {
        double x_g = width / 2. - height / 2.;
        double x_d = width / 2. + height / 2.;
        
        cairo_rectangle (cr, x_g + 1., 1., x_d - x_g - 1., height - 1.);
        cairo_fill (cr);
        cairo_set_source_rgba (cr, 0., 0., 0., 1.);
        cairo_set_line_width (cr, 1.);
        cairo_rectangle (cr, x_g + 1., 1., x_d - x_g - 1., height - 1.);
      }
      cairo_stroke (cr);
      
      break;
    }
    case SECTION_CIRCULAIRE :
    {
      cairo_set_source_rgba (cr, 0.8, 0.8, 0.8, 1.);
      
      if (a < 1)
      {
        double y_h = height / 2. - width / 2.;
        double y_b = height / 2. + width / 2.;
        
        cairo_arc (cr,
                   width / 2., y_h + (y_b - y_h) / 2.,
                   width / 2. - 1.,
                   0,
                   2. * M_PI);
        cairo_fill (cr);
        cairo_set_source_rgba (cr, 0., 0., 0., 1.);
        cairo_set_line_width (cr, 1.);
        cairo_arc (cr,
                   width / 2.,
                   y_h + (y_b - y_h) / 2.,
                   width / 2. - 1.,
                   0,
                   2. * M_PI);
      }
      else
      {
        double x_g = width / 2. - height / 2.;
        double x_d = width / 2. + height / 2.;
        
        cairo_arc (cr,
                   x_g + (x_d - x_g) / 2.,
                   height / 2.,
                   height / 2. - 1.,
                   0,
                   2. * M_PI);
        cairo_fill (cr);
        cairo_set_source_rgba (cr, 0., 0., 0., 1.);
        cairo_set_line_width (cr, 1.);
        cairo_arc (cr,
                   x_g + (x_d - x_g) / 2.,
                   height / 2.,
                   height / 2. - 1.,
                   0,
                   2. * M_PI);
      }
      cairo_stroke (cr);
      
      break;
    }
    case SECTION_PERSONNALISEE :
    {
      Section_Personnalisee *data = section->data;
      
      double  aa;
      GList   *list_parcours;
      GList   *list_parcours2;
      double  xmin = NAN, xmax = NAN, ymin = NAN, ymax = NAN;
      double  decalagex, decalagey;
      
      // On commence par calculer la largeur et la hauteur de la section.
      list_parcours = data->forme;
      while (list_parcours != NULL)
      {
        list_parcours2 = list_parcours->data;
        
        while (list_parcours2 != NULL)
        {
          EF_Point *point = list_parcours2->data;
          
          if (isnan (xmin))
          {
            xmin = m_g (point->x);
            xmax = m_g (point->x);
            ymin = m_g (point->y);
            ymax = m_g (point->y);
          }
          else
          {
            if (m_g (point->x) < xmin)
              xmin = m_g (point->x);
            if (m_g (point->x) > xmax)
              xmax = m_g (point->x);
            if (m_g (point->y) < ymin)
              ymin = m_g (point->y);
            if (m_g (point->y) > ymax)
              ymax = m_g (point->y);
          }
          
          list_parcours2 = g_list_next (list_parcours2);
        }
        
        list_parcours = g_list_next (list_parcours);
      }
      
      aa = (xmax - xmin) / (ymax - ymin);
      
      // Le schéma prend toute la largeur
      if (aa > a)
        convert = (width - 1) / (xmax - xmin);
      else
        convert = (height - 1) / (ymax - ymin);
      decalagex = (width - (xmax - xmin) * convert) / 2.;
      decalagey = (height - (ymax - ymin) * convert) / 2.;
      
      // On dessine la section.
      list_parcours = data->forme;
      while (list_parcours != NULL)
      {
        list_parcours2 = list_parcours->data;
        
        cairo_set_source_rgba (cr, 0.8, 0.8, 0.8, 1.);
        cairo_new_path (cr);
        
        while (list_parcours2 != NULL)
        {
          EF_Point *point = list_parcours2->data;
          
          if (list_parcours2 == list_parcours->data)
            cairo_move_to (cr,
                           decalagex + ((m_g (point->x) - xmin) * convert),
                           decalagey + ((ymax - m_g (point->y)) * convert));
          else
            cairo_line_to (cr,
                           decalagex + ((m_g (point->x) - xmin) * convert),
                           decalagey + ((ymax - m_g (point->y)) * convert));
          
          list_parcours2 = g_list_next (list_parcours2);
        }
        cairo_close_path (cr);
        save_path = cairo_copy_path (cr);
        cairo_fill (cr);
        cairo_set_source_rgba (cr, 0., 0., 0., 1.);
        cairo_set_line_width (cr, 1.);
        cairo_new_path (cr);
        cairo_append_path (cr, save_path);
        cairo_stroke (cr);
        
        cairo_path_destroy (save_path);
        list_parcours = g_list_next (list_parcours);
      }
      
      break;
    }
    default :
    {
      FAILCRIT (NULL,
                (gettext ("Type de section %d inconnu.\n"), section->type);
                  cairo_destroy (cr);
                  cairo_surface_destroy (surface);)
      break;
    }
  }
  cairo_destroy (cr);
  pixbuf = gdk_pixbuf_get_from_surface (surface, 0, 0, width, height);
  cairo_surface_destroy (surface);
  
  return pixbuf;
}


void
EF_gtk_sections_ajout_rectangulaire (GtkMenuItem *menuitem,
                                     Projet      *p)
/**
 * \brief Lance la fenêtre permettant d'ajouter une section rectangulaire.
 * \param menuitem : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
{
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_SEC.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Section");)
  
  BUG (EF_gtk_section_rectangulaire (p, NULL), )
}


void
EF_gtk_sections_ajout_T (GtkMenuItem *menuitem,
                         Projet      *p)
/**
 * \brief Lance la fenêtre permettant d'ajouter une section en T.
 * \param menuitem : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
{
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_SEC.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Section");)
  
  BUG (EF_gtk_section_T (p, NULL), )
}


void
EF_gtk_sections_ajout_carree (GtkMenuItem *menuitem,
                              Projet      *p)
/**
 * \brief Lance la fenêtre permettant d'ajouter une section carrée.
 * \param menuitem : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
{
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_SEC.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Section");)
  
  BUG (EF_gtk_section_carree (p, NULL), )
}


void
EF_gtk_sections_ajout_circulaire (GtkMenuItem *menuitem,
                                  Projet      *p)
/**
 * \brief Lance la fenêtre permettant d'ajouter une section circulaire.
 * \param menuitem : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
{
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_SEC.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Section");)
  
  BUG (EF_gtk_section_circulaire (p, NULL), )
}


void
EF_gtk_sections_ajout_personnalisee (GtkMenuItem *menuitem,
                                     Projet      *p)
/**
 * \brief Lance la fenêtre permettant d'ajouter une section personnalisée.
 * \param menuitem : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
{
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_SEC.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Section");)
  
  BUG (EF_gtk_section_personnalisee (p, NULL), )
}


void
EF_gtk_sections_edit_clicked (GtkWidget *widget,
                              Projet    *p)
/**
 * \brief Edite les sections sélectionnées.
 * \param widget : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
{
  GtkTreeIter   iter;
  GtkTreeModel *model;
  GList        *list, *list_parcours;
  
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_SEC.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Section");)
  
  // On récupère la liste des charges à éditer.
  list = gtk_tree_selection_get_selected_rows (GTK_TREE_SELECTION (
       gtk_builder_get_object (UI_SEC.builder, "EF_sections_treeview_select")),
                                               &model);
  list_parcours = g_list_first (list);
  for ( ; list_parcours != NULL; list_parcours = g_list_next (list_parcours))
  {
    if (gtk_tree_model_get_iter (model,
                                 &iter,
                                 (GtkTreePath *) list_parcours->data))
    {
  // Et on les édite les unes après les autres.
      Section *section;
      
      gtk_tree_model_get (model, &iter, 0, &section, -1);
      
      switch (section->type)
      {
        case SECTION_RECTANGULAIRE :
        {
          BUG (EF_gtk_section_rectangulaire (p, section), )
          break;
        }
        case SECTION_T :
        {
          BUG (EF_gtk_section_T (p, section), )
          break;
        }
        case SECTION_CARREE :
        {
          BUG (EF_gtk_section_carree (p, section), )
          break;
        }
        case SECTION_CIRCULAIRE :
        {
          BUG (EF_gtk_section_circulaire (p, section), )
          break;
        }
        case SECTION_PERSONNALISEE :
        {
          BUG (EF_gtk_section_personnalisee (p, section), )
          break;
        }
        default :
        {
          FAILCRIT ( ,
                    (gettext ("Type de section %d inconnu.\n"),
                              section->type);
                      g_list_foreach (list, (GFunc) gtk_tree_path_free, NULL);
                      g_list_free (list);)
          break;
        }
      }
    }
  }
  
  g_list_foreach (list, (GFunc) gtk_tree_path_free, NULL);
  g_list_free (list);
  
  return;
}


gboolean
EF_gtk_sections_double_clicked (GtkWidget *widget,
                                GdkEvent  *event,
                                Projet    *p)
/**
 * \brief Lance la fenêtre d'édition de la section sélectionnée en cas de
 *        double-clique dans le tree-view.
 * \param widget : composant à l'origine de l'évènement,
 * \param event : Information sur l'évènement,
 * \param p : la variable projet.
 * \return TRUE s'il y a édition via un double-clique, FALSE sinon.\n
 *   Echec : FALSE :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
{
  BUGPARAMCRIT (p, "%p", p, FALSE)
  BUGCRIT (UI_SEC.builder,
           FALSE,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Section");)
  
  if ((event->type == GDK_2BUTTON_PRESS) &&
      (gtk_widget_get_sensitive (GTK_WIDGET (gtk_builder_get_object (
                            UI_SEC.builder, "EF_sections_boutton_modifier")))))
  {
    EF_gtk_sections_edit_clicked (widget, p);
    return TRUE;
  }
  else
    return common_gtk_treeview_button_press_unselect (GTK_TREE_VIEW (widget),
                                                      (GdkEventButton *) event,
                                                      p);
}


void
EF_gtk_sections_render_0 (GtkTreeViewColumn *tree_column,
                          GtkCellRenderer   *cell,
                          GtkTreeModel      *tree_model,
                          GtkTreeIter       *iter,
                          gpointer           data2)
/**
 * \brief Affiche la section dans le graphique.
 * \param tree_column : composant à l'origine de l'évènement,
 * \param cell : la cellule en cours d'édition,
 * \param tree_model : le mode en cours d'édition,
 * \param iter : la ligne en cours d'édition,
 * \param data2 : la variable projet.
 * \return Rien.
 */
{
  Section   *section;
  GdkPixbuf *pixbuf;
  
  gtk_tree_model_get (tree_model, iter, 0, &section, -1);
  
  BUG (pixbuf = EF_gtk_sections_dessin (section, 32, 32), )
  
  g_object_set (cell, "pixbuf", pixbuf, NULL);
  
  g_object_unref (pixbuf);
  
  return;
}


void
EF_gtk_sections_render_1 (GtkTreeViewColumn *tree_column,
                          GtkCellRenderer   *cell,
                          GtkTreeModel      *tree_model,
                          GtkTreeIter       *iter,
                          gpointer           data2)
/**
 * \brief Affiche le nom de la section.
 * \param tree_column : composant à l'origine de l'évènement,
 * \param cell : la cellule en cours d'édition,
 * \param tree_model : le mode en cours d'édition,
 * \param iter : la ligne en cours d'édition,
 * \param data2 : la variable projet.
 * \return Rien.
 */
{
  Section *section;
  
  gtk_tree_model_get (tree_model, iter, 0, &section, -1);
  
  g_object_set (cell, "text", section->nom, NULL);
  
  return;
}


void
EF_gtk_sections_render_2 (GtkTreeViewColumn *tree_column,
                          GtkCellRenderer   *cell,
                          GtkTreeModel      *tree_model,
                          GtkTreeIter       *iter,
                          gpointer           data2)
/**
 * \brief Affiche la description de la section.
 * \param tree_column : composant à l'origine de l'évènement,
 * \param cell : la cellule en cours d'édition,
 * \param tree_model : le mode en cours d'édition,
 * \param iter : la ligne en cours d'édition,
 * \param data2 : la variable projet.
 * \return Rien.
 */
{
  Section *section;
  char    *description;
  
  gtk_tree_model_get (tree_model, iter, 0, &section, -1);
  
  BUG (description = EF_sections_get_description (section), )
  
  g_object_set (cell, "text", description, NULL);
  
  free (description);
  
  return;
}


void
EF_gtk_sections_render_3 (GtkTreeViewColumn *tree_column,
                          GtkCellRenderer   *cell,
                          GtkTreeModel      *tree_model,
                          GtkTreeIter       *iter,
                          gpointer           data2)
/**
 * \brief Affiche le module de torsion de la section.
 * \param tree_column : composant à l'origine de l'évènement,
 * \param cell : la cellule en cours d'édition,
 * \param tree_model : le mode en cours d'édition,
 * \param iter : la ligne en cours d'édition,
 * \param data2 : la variable projet.
 * \return Rien.
 */
{
  Section *section;
  char     c[30];
  
  gtk_tree_model_get (tree_model, iter, 0, &section, -1);
  
  conv_f_c (EF_sections_j (section), c, DECIMAL_M4);
  
  g_object_set (cell, "text", c, NULL);
  
  return;
}


void
EF_gtk_sections_render_4 (GtkTreeViewColumn *tree_column,
                          GtkCellRenderer   *cell,
                          GtkTreeModel      *tree_model,
                          GtkTreeIter       *iter,
                          gpointer           data2)
/**
 * \brief Affiche le module de flexion selon l'axe y de la section.
 * \param tree_column : composant à l'origine de l'évènement,
 * \param cell : la cellule en cours d'édition,
 * \param tree_model : le mode en cours d'édition,
 * \param iter : la ligne en cours d'édition,
 * \param data2 : la variable projet.
 * \return Rien.
 */
{
  Section *section;
  char     c[30];
  
  gtk_tree_model_get (tree_model, iter, 0, &section, -1);
  
  conv_f_c (EF_sections_iy (section), c, DECIMAL_M4);
  
  g_object_set (cell, "text", c, NULL);
  
  return;
}


void
EF_gtk_sections_render_5 (GtkTreeViewColumn *tree_column,
                          GtkCellRenderer   *cell,
                          GtkTreeModel      *tree_model,
                          GtkTreeIter       *iter,
                          gpointer           data2)
/**
 * \brief Affiche le module de flexion selon l'axe z de la section.
 * \param tree_column : composant à l'origine de l'évènement,
 * \param cell : la cellule en cours d'édition,
 * \param tree_model : le mode en cours d'édition,
 * \param iter : la ligne en cours d'édition,
 * \param data2 : la variable projet.
 * \return Rien.
 */
{
  Section *section;
  char     c[30];
  
  gtk_tree_model_get (tree_model, iter, 0, &section, -1);
  
  conv_f_c (EF_sections_iz (section), c, DECIMAL_M4);
  
  g_object_set (cell, "text", c, NULL);
  
  return;
}


void
EF_gtk_sections_render_6 (GtkTreeViewColumn *tree_column,
                          GtkCellRenderer   *cell,
                          GtkTreeModel      *tree_model,
                          GtkTreeIter       *iter,
                          gpointer           data2)
/**
 * \brief Affiche la surface de la section.
 * \param tree_column : composant à l'origine de l'évènement,
 * \param cell : la cellule en cours d'édition,
 * \param tree_model : le mode en cours d'édition,
 * \param iter : la ligne en cours d'édition,
 * \param data2 : la variable projet.
 * \return Rien.
 */
{
  Section *section;
  char     c[30];
  
  gtk_tree_model_get (tree_model, iter, 0, &section, -1);
  
  conv_f_c (EF_sections_s (section), c, DECIMAL_SURFACE);
  
  g_object_set (cell, "text", c, NULL);
  
  return;
}


void
EF_gtk_sections_render_7 (GtkTreeViewColumn *tree_column,
                          GtkCellRenderer   *cell,
                          GtkTreeModel      *tree_model,
                          GtkTreeIter       *iter,
                          gpointer           data2)
/**
 * \brief Affiche la distance v<sub>y</sub> de la section.
 * \param tree_column : composant à l'origine de l'évènement,
 * \param cell : la cellule en cours d'édition,
 * \param tree_model : le mode en cours d'édition,
 * \param iter : la ligne en cours d'édition,
 * \param data2 : la variable projet.
 * \return Rien.
 */
{
  Section *section;
  char     c[30];
  
  gtk_tree_model_get (tree_model, iter, 0, &section, -1);
  
  conv_f_c (EF_sections_vy (section), c, DECIMAL_DISTANCE);
  
  g_object_set (cell, "text", c, NULL);
  
  return;
}


void
EF_gtk_sections_render_8 (GtkTreeViewColumn *tree_column,
                          GtkCellRenderer   *cell,
                          GtkTreeModel      *tree_model,
                          GtkTreeIter       *iter,
                          gpointer           data2)
/**
 * \brief Affiche la distance v<sub>y</sub>' de la section.
 * \param tree_column : composant à l'origine de l'évènement,
 * \param cell : la cellule en cours d'édition,
 * \param tree_model : le mode en cours d'édition,
 * \param iter : la ligne en cours d'édition,
 * \param data2 : la variable projet.
 * \return Rien.
 */
{
  Section *section;
  char     c[30];
  
  gtk_tree_model_get (tree_model, iter, 0, &section, -1);
  
  conv_f_c (EF_sections_vyp (section), c, DECIMAL_DISTANCE);
  
  g_object_set (cell, "text", c, NULL);
  
  return;
}


void
EF_gtk_sections_render_9 (GtkTreeViewColumn *tree_column,
                          GtkCellRenderer   *cell,
                          GtkTreeModel      *tree_model,
                          GtkTreeIter       *iter,
                          gpointer           data2)
/**
 * \brief Affiche la distance v<sub>z</sub> de la section.
 * \param tree_column : composant à l'origine de l'évènement,
 * \param cell : la cellule en cours d'édition,
 * \param tree_model : le mode en cours d'édition,
 * \param iter : la ligne en cours d'édition,
 * \param data2 : la variable projet.
 * \return Rien.
 */
{
  Section *section;
  char     c[30];
  
  gtk_tree_model_get (tree_model, iter, 0, &section, -1);
  
  conv_f_c (EF_sections_vz (section), c, DECIMAL_DISTANCE);
  
  g_object_set (cell, "text", c, NULL);
  
  return;
}


void
EF_gtk_sections_render_10 (GtkTreeViewColumn *tree_column,
                           GtkCellRenderer   *cell,
                           GtkTreeModel      *tree_model,
                           GtkTreeIter       *iter,
                           gpointer           data2)
/**
 * \brief Affiche la distance v<sub>z</sub>' de la section.
 * \param tree_column : composant à l'origine de l'évènement,
 * \param cell : la cellule en cours d'édition,
 * \param tree_model : le mode en cours d'édition,
 * \param iter : la ligne en cours d'édition,
 * \param data2 : la variable projet.
 * \return Rien.
 */
{
  Section *section;
  char     c[30];
  
  gtk_tree_model_get (tree_model, iter, 0, &section, -1);
  
  conv_f_c (EF_sections_vzp (section), c, DECIMAL_DISTANCE);
  
  g_object_set (cell, "text", c, NULL);
  
  return;
}


gboolean
EF_gtk_sections_get_section (char   *ligne,
                             char  **nom,
                             double *g,
                             double *h,
                             double *b,
                             double *tw,
                             double *tf,
                             double *r1,
                             double *r2,
                             double *A,
                             double *hi,
                             double *d,
                             int    *phi,
                             double *pmin,
                             double *pmax,
                             double *AL,
                             double *AG,
                             double *iiy,
                             double *Wely,
                             double *Wply,
                             double *iy,
                             double *Avz,
                             double *iiz,
                             double *Welz,
                             double *Wplz,
                             double *iz,
                             double *ss,
                             double *It,
                             double *Iw,
                             double *vy,
                             double *vyp,
                             double *vz,
                             double *vzp,
                             double *vty,
                             double *vtz,
                             GList **forme)
/**
 * \brief Renvoie sous forme de variables la ligne en cours d'analyse du
 *        fichier d'import de section.
 * \param ligne : la ligne sous forme de texte, seul paramètre devant être non
 *                NULL,
 * \param nom : nom de la section. La valeur renvoyée doit être libérée
 *                     avec free,
 * \param g : masse par unité de longueur en kg/ml,
 * \param h : hauteur en m,
 * \param b : largeur en m,
 * \param tw : épaisseur d’âme en m,
 * \param tf : épaisseur d’aile en m,
 * \param r1 : rayon de congé intérieur en m,
 * \param r2 : rayon de congé extérieur en m,
 * \param A : aire de section en m²,
 * \param hi : hauteur intérieure entre les ailes en m,
 * \param d :  hauteur de la portion droite de l’âme
 * \param phi : diamètre de boulon maximal en mm
 * \param pmin : pince minimale admissible en m,
 * \param pmax : pince maximale admissible en m,
 * \param AL : surface à peindre par unité de longueur en m²/ml,
 * \param AG : surface à peindre par unité de masse en m²/t,
 * \param iiy : moment d’inertie de flexion selon l'axe y en m⁴,
 * \param Wely : module de flexion élastique selon l'axe y en m³,
 * \param Wply : module de flexion plastique selon l'axe y en m³,
 * \param iy : rayon de giration selon l'axe y en m,
 * \param Avz : aire de cisaillement, effort parallèle à l’âme, en m²,
 * \param iiz : moment d’inertie de flexion selon l'axe z en m⁴,
 * \param Welz : module de flexion élastique selon l'axe z en m³,
 * \param Wplz : module de flexion plastique selon l'axe z en m³,
 * \param iz : rayon de giration selon l'axe z en m,
 * \param ss : longueur d’appui rigide de l'aile en m,
 * \param It : moment d’inertie de torsion en m⁴,
 * \param Iw : moment d’inertie de gauchissement par rapport au centre
 *             de cisaillement en m⁶,
 * \param vy : distance horizontale entre le centre de gravité et la
 *             fibre extrême droite en m,
 * \param vyp : distance horizontale entre le centre de gravité et la
 *              fibre extrême gauche en m,
 * \param vz : distance verticale entre le centre de gravité et la
 *             fibre extrême haute en m,
 * \param vzp : distance verticale entre le centre de gravité et la
 *              fibre extrême basse en m,
 * \param vty : distance horizontale entre le centre de gravité et le
 *              centre de cisaillement en m, positif si vers la droite,
 * \param vtz : distance verticale entre le centre de gravité et le
 *              centre de cisaillement en m, positif si vers le haut,
 * \param forme : forme de la section. Contient une liste de groupe
 *                        formant chacun une section par une liste de points.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - ligne == NULL,
 *     - en cas d'erreur d'allocation mémoire.
 */
{
  char  *nom_, *ligne_tmp;
  double g_, h_, b_, tw_, tf_, r1_, r2_, A_, hi_, d_, pmin_, pmax_, AL_, AG_;
  double iiy_, Wely_, Wply_, iy_, Avz_, iiz_, Welz_, Wplz_, iz_, ss_, It_, Iw_;
  double vy_, vyp_, vz_, vzp_, vty_, vtz_;
  int    phi_;
  
  int    i;
  GList *forme_, *points;
  double x, y;
  
  BUGPARAMCRIT (ligne, "%p", ligne, FALSE)
  
  ligne_tmp = &ligne[strchr (ligne, '\t') - ligne + 1];
  INFO (sscanf (ligne_tmp,
                "%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%d\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t",
                &g_,
                &h_,
                &b_,
                &tw_,
                &tf_,
                &r1_,
                &r2_,
                &A_,
                &hi_,
                &d_,
                &phi_,
                &pmin_,
                &pmax_,
                &AL_,
                &AG_,
                &iiy_,
                &Wely_,
                &Wply_,
                &iy_,
                &Avz_,
                &iiz_,
                &Welz_,
                &Wplz_,
                &iz_,
                &ss_,
                &It_,
                &Iw_,
                &vy_,
                &vyp_,
                &vz_,
                &vzp_,
                &vty_,
                &vtz_) == 33,
        FALSE,
        (gettext ("La ligne en cours '%s' n'est pas dans un format correct pour une section.\n"),
                  ligne);)
  
  if (g != NULL)
    *g = g_;
  if (h != NULL)
    *h = h_;
  if (b != NULL)
    *b = b_;
  if (tw != NULL)
    *tw = tw_;
  if (tf != NULL)
    *tf = tf_;
  if (r1 != NULL)
    *r1 = r1_;
  if (r2 != NULL)
    *r2 = r2_;
  if (A != NULL)
    *A = A_;
  if (hi != NULL)
    *hi = hi_;
  if (d != NULL)
    *d = d_;
  if (phi != NULL)
    *phi = phi_;
  if (pmin != NULL)
    *pmin = pmin_;
  if (pmax != NULL)
    *pmax = pmax_;
  if (AL != NULL)
    *AL = AL_;
  if (AG != NULL)
    *AG = AG_;
  if (iiy != NULL)
    *iiy = iiy_;
  if (Wely != NULL)
    *Wely = Wely_;
  if (Wply != NULL)
    *Wply = Wply_;
  if (iy != NULL)
    *iy = iy_;
  if (Avz != NULL)
    *Avz = Avz_;
  if (iiz != NULL)
    *iiz = iiz_;
  if (Welz != NULL)
    *Welz = Welz_;
  if (Wplz != NULL)
    *Wplz = Wplz_;
  if (iz != NULL)
    *iz = iz_;
  if (ss != NULL)
    *ss = ss_;
  if (It != NULL)
    *It = It_;
  if (Iw != NULL)
    *Iw = Iw_;
  if (vy != NULL)
    *vy = vy_;
  if (vyp != NULL)
    *vyp = vyp_;
  if (vz != NULL)
    *vz = vz_;
  if (vzp != NULL)
    *vzp = vzp_;
  if (vty != NULL)
    *vty = vty_;
  if (vtz != NULL)
    *vtz = vtz_;
  
  ligne_tmp = ligne;
  i = 0;
  while ((i != 34) && (ligne_tmp[0] != 0))
  {
    if (ligne_tmp[0] == '\t')
      i++;
    ligne_tmp++;
  }
  forme_ = NULL;
  points = NULL;
  
  while (ligne_tmp[0] != 0)
  {
    INFO (sscanf (ligne_tmp, "%lf\t%lf", &x, &y) == 2,
          FALSE,
          (gettext ("La ligne en cours '%s' n'est pas dans un format correct pour une section.\n"),
                    ligne);
            g_list_free_full (points, free);
            g_list_free_full (forme_,
                      (GDestroyNotify) EF_sections_personnalisee_free_forme1);)
    
    // Nouveau groupe de points
    if ((isnan (x)) && (isnan (y)))
    {
      // On ajoute pas un groupe de point vide
      if (points != NULL)
        forme_ = g_list_append (forme_, points);
      points = NULL;
    }
    else
    {
      EF_Point *point;
      
      BUGCRIT (point = malloc (sizeof (EF_Point)),
               FALSE,
               (gettext ("Erreur d'allocation mémoire.\n"));
                 g_list_free_full (points, free);
                 g_list_free_full (forme_, 
                      (GDestroyNotify) EF_sections_personnalisee_free_forme1);)
      point->x = m_f (x, FLOTTANT_UTILISATEUR);
      point->y = m_f (y, FLOTTANT_UTILISATEUR);
      point->z = m_f (0., FLOTTANT_UTILISATEUR);
      
      points = g_list_append (points, point);
    }
    
    // On passe au groupe de point suivant
    i = 0;
    while ((i != 2) && (ligne_tmp[0] != 0))
    {
      if (ligne_tmp[0] == '\t')
        i++;
      ligne_tmp++;
    }
  }
  if (points != NULL)
    forme_ = g_list_append (forme_, points);
  
  if (!EF_sections_personnalisee_verif_forme (forme_, TRUE))
  {
    g_list_free_full (forme_,
                      (GDestroyNotify) EF_sections_personnalisee_free_forme1);
    
    FAILINFO (FALSE,
              (gettext ("La ligne en cours '%s' n'est pas dans un format correct pour une section.\n"),
                        ligne);)
  }
  else if (forme == NULL)
    g_list_free_full (forme_,
                      (GDestroyNotify) EF_sections_personnalisee_free_forme1);
  else
    *forme = forme_;
  
  // On le fait à la fin pour éviter d'allouer inutilement de la mémoire.
  if (ligne != NULL)
  {
    BUGCRIT (nom_ = malloc (sizeof (char) *
                                           (strchr (ligne, '\t') - ligne + 1)),
             FALSE,
             (gettext ("Erreur d'allocation mémoire.\n"));
               g_list_free_full (forme_,
                      (GDestroyNotify) EF_sections_personnalisee_free_forme1);)
    
    strncpy (nom_, ligne, strchr (ligne, '\t')-ligne);
    nom_[strchr (ligne, '\t') - ligne] = 0;
    
    *nom = nom_;
  }
  
  return TRUE;
}


void
EF_gtk_sections_importe_section (GtkMenuItem *menuitem,
                                 Projet      *p)
/**
 * \brief Permet d'importer une section depuis les menus.
 * \param menuitem : l'item dont le nom contient la section à importer,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - Le fichier profiles_acier.csv est introuvable.
 */
{
  FILE    *file;
  wchar_t *ligne_tmp;
  char    *section;
  
  BUGPARAMCRIT (p, "%p", p, )
  
  INFO (file = fopen (DATADIR"/profiles_acier.csv", "r"),
        ,
        (gettext ("Le fichier des sections est introuvable.\n"));)
  
  BUGCRIT (section = g_strdup_printf ("%s\t",
                                      gtk_menu_item_get_label (menuitem)),
           ,
           (gettext ("Erreur d'allocation mémoire.\n"));
             fclose (file);)
  
  ligne_tmp = common_text_get_line (file);
  free (ligne_tmp);
  
  ligne_tmp = common_text_get_line (file);
  while (ligne_tmp != NULL)
  {
    char *ligne;
    
    BUG (ligne = common_text_wcstostr_dup (ligne_tmp),
         ,
         fclose (file);
           free (section);
           free (ligne_tmp);)
    free (ligne_tmp);
    if (strncmp (ligne, section, strlen (section)) == 0)
    {
      double j, iy, iz, vy, vyp, vz, vzp, s;
      GList *forme;
      char  *desc;
      
      BUG (EF_gtk_sections_get_section (ligne,
                                        &desc,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL,
                                        &s,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL,
                                        &iy,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL,
                                        &iz,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL,
                                        &j,
                                        NULL,
                                        &vy,
                                        &vyp,
                                        &vz,
                                        &vzp,
                                        NULL,
                                        NULL,
                                        &forme),
          ,
          fclose (file);
            free (section);
            free (ligne);)
      BUG (EF_sections_personnalisee_ajout (p,
                                            desc,
                                            desc,
                                            m_f (j, FLOTTANT_ORDINATEUR),
                                            m_f (iy, FLOTTANT_ORDINATEUR),
                                            m_f (iz, FLOTTANT_ORDINATEUR),
                                            m_f (vy, FLOTTANT_ORDINATEUR),
                                            m_f (vyp, FLOTTANT_ORDINATEUR),
                                            m_f (vz, FLOTTANT_ORDINATEUR),
                                            m_f (vzp, FLOTTANT_ORDINATEUR),
                                            m_f (s, FLOTTANT_ORDINATEUR),
                                            forme),
          ,
          fclose (file);
            free (section);
            free (ligne);
            free (desc);
            g_list_free_full (forme,
                              (GDestroyNotify) EF_sections_personnalisee_free_forme1);)
      free (desc);
    }
    free (ligne);
    ligne_tmp = common_text_get_line (file);
  }
  
  free (section);
  fclose (file);
  
  return;
}


void
EF_gtk_sections (Projet *p)
/**
 * \brief Création de la fenêtre permettant d'afficher les sections sous forme
 *        d'un tableau.
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique impossible à générer.
 */
{
  GList *list_parcours;
  FILE  *file;
  
  BUGPARAM (p, "%p", p, )
  
  if (UI_SEC.builder != NULL)
  {
    gtk_window_present (GTK_WINDOW (UI_SEC.window));
    return;
  }
  
  UI_SEC.builder = gtk_builder_new ();
  BUGCRIT (gtk_builder_add_from_resource (UI_SEC.builder,
                                         "/org/2lgc/codegui/ui/EF_sections.ui",
                                          NULL) != 0,
           ,
           (gettext ("La génération de la fenêtre %s a échouée.\n"),
                     "Section");)
  gtk_builder_connect_signals (UI_SEC.builder, p);
  
  UI_SEC.window = GTK_WIDGET (gtk_builder_get_object (UI_SEC.builder,
                                                        "EF_sections_window"));
  UI_SEC.sections = GTK_TREE_STORE (gtk_builder_get_object (UI_SEC.builder,
                                                     "EF_sections_treestore"));
  
  gtk_tree_view_column_set_cell_data_func (
    GTK_TREE_VIEW_COLUMN (gtk_builder_get_object (UI_SEC.builder,
                                              "EF_sections_treeview_column0")),
    GTK_CELL_RENDERER (gtk_builder_get_object (UI_SEC.builder,
                                                "EF_sections_treeview_cell0")),
    EF_gtk_sections_render_0,
    p,
    NULL);
  gtk_tree_view_column_set_cell_data_func (
    GTK_TREE_VIEW_COLUMN (gtk_builder_get_object (UI_SEC.builder,
                                              "EF_sections_treeview_column1")),
    GTK_CELL_RENDERER (gtk_builder_get_object (UI_SEC.builder,
                                                "EF_sections_treeview_cell1")),
    EF_gtk_sections_render_1,
    p,
    NULL);
  gtk_tree_view_column_set_cell_data_func (
    GTK_TREE_VIEW_COLUMN (gtk_builder_get_object (UI_SEC.builder,
                                              "EF_sections_treeview_column2")),
    GTK_CELL_RENDERER (gtk_builder_get_object (UI_SEC.builder,
                                                "EF_sections_treeview_cell2")),
    EF_gtk_sections_render_2,
    p,
    NULL);
  gtk_tree_view_column_set_cell_data_func (
    GTK_TREE_VIEW_COLUMN (gtk_builder_get_object (UI_SEC.builder,
                                              "EF_sections_treeview_column3")),
    GTK_CELL_RENDERER (gtk_builder_get_object (UI_SEC.builder,
                                                "EF_sections_treeview_cell3")),
    EF_gtk_sections_render_3,
    p,
    NULL);
  gtk_tree_view_column_set_cell_data_func (
    GTK_TREE_VIEW_COLUMN (gtk_builder_get_object (UI_SEC.builder,
                                              "EF_sections_treeview_column4")),
    GTK_CELL_RENDERER (gtk_builder_get_object (UI_SEC.builder,
                                                "EF_sections_treeview_cell4")),
    EF_gtk_sections_render_4,
    p,
    NULL);
  gtk_tree_view_column_set_cell_data_func (
    GTK_TREE_VIEW_COLUMN (gtk_builder_get_object (UI_SEC.builder,
                                              "EF_sections_treeview_column5")),
    GTK_CELL_RENDERER (gtk_builder_get_object (UI_SEC.builder,
                                                "EF_sections_treeview_cell5")),
    EF_gtk_sections_render_5,
    p,
    NULL);
  gtk_tree_view_column_set_cell_data_func (
    GTK_TREE_VIEW_COLUMN (gtk_builder_get_object (UI_SEC.builder,
                                              "EF_sections_treeview_column6")),
    GTK_CELL_RENDERER (gtk_builder_get_object (UI_SEC.builder,
                                                "EF_sections_treeview_cell6")),
    EF_gtk_sections_render_6,
    p,
    NULL);
  gtk_tree_view_column_set_cell_data_func (
    GTK_TREE_VIEW_COLUMN (gtk_builder_get_object (UI_SEC.builder,
                                              "EF_sections_treeview_column7")),
    GTK_CELL_RENDERER (gtk_builder_get_object (UI_SEC.builder,
                                                "EF_sections_treeview_cell7")),
    EF_gtk_sections_render_7,
    p,
    NULL);
  gtk_tree_view_column_set_cell_data_func (
    GTK_TREE_VIEW_COLUMN (gtk_builder_get_object (UI_SEC.builder,
                                              "EF_sections_treeview_column8")),
    GTK_CELL_RENDERER (gtk_builder_get_object (UI_SEC.builder,
                                                "EF_sections_treeview_cell8")),
    EF_gtk_sections_render_8,
    p,
    NULL);
  gtk_tree_view_column_set_cell_data_func (
    GTK_TREE_VIEW_COLUMN (gtk_builder_get_object (UI_SEC.builder,
                                              "EF_sections_treeview_column9")),
    GTK_CELL_RENDERER (gtk_builder_get_object (UI_SEC.builder,
                                                "EF_sections_treeview_cell9")),
    EF_gtk_sections_render_9,
    p,
    NULL);
  gtk_tree_view_column_set_cell_data_func (
    GTK_TREE_VIEW_COLUMN (gtk_builder_get_object (UI_SEC.builder,
                                             "EF_sections_treeview_column10")),
    GTK_CELL_RENDERER (gtk_builder_get_object (UI_SEC.builder,
                                               "EF_sections_treeview_cell10")),
    EF_gtk_sections_render_10,
    p,
    NULL);
  
  list_parcours = p->modele.sections;
  while (list_parcours != NULL)
  {
    Section *section = (Section *)list_parcours->data;
    
    gtk_tree_store_append (UI_SEC.sections, &section->Iter_fenetre, NULL);
    gtk_tree_store_set (UI_SEC.sections,
                        &section->Iter_fenetre,
                        0, section,
                        -1);
    
    list_parcours = g_list_next (list_parcours);
  }
  
  file = fopen (DATADIR"/profiles_acier.csv", "r");
  if (file != NULL)
  {
    char      *ligne;
    wchar_t   *ligne_tmp;
    GtkWidget *sous_menu;
    GList     *list_categorie = NULL;
    
    sous_menu = gtk_menu_item_get_submenu (GTK_MENU_ITEM (
                                        gtk_builder_get_object (UI_SEC.builder,
                                        "EF_sections_ajouter_menu_importee")));
    if (sous_menu == NULL)
    {
      sous_menu = gtk_menu_new ();
      gtk_menu_item_set_submenu (GTK_MENU_ITEM (gtk_builder_get_object (
                         UI_SEC.builder, "EF_sections_ajouter_menu_importee")),
                                 sous_menu);
    }
    
    // On passe la première ligne qui est l'étiquette des colonnes.
    ligne_tmp = common_text_get_line (file);
    free (ligne_tmp);
    
    ligne_tmp = common_text_get_line (file);
    while (ligne_tmp != NULL)
    {
      char      *nom_section, *categorie;
      GtkWidget *menu, *categorie_menu = NULL;
      GList     *forme;
      
      BUG (ligne = common_text_wcstostr_dup (ligne_tmp),
           ,
           free (ligne_tmp);
             fclose (file);)
      free (ligne_tmp);
      
      BUG (EF_gtk_sections_get_section (ligne,
                                        &nom_section,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL,
                                        &forme), 
          )
      BUGCRIT (categorie = malloc (sizeof (char) * (strlen (nom_section) + 1)),
               ,
               (gettext ("Erreur d'allocation mémoire.\n"));
                 fclose (file);
                 free (ligne);
                 free (nom_section);
                 g_list_free_full (forme,
                      (GDestroyNotify) EF_sections_personnalisee_free_forme1);)
      if (strchr (nom_section, ' ') == NULL)
        strcpy (categorie, nom_section);
      else
      {
        strncpy (categorie,
                 nom_section,
                 strchr (nom_section, ' ') - nom_section);
        categorie[strchr (nom_section, ' ') - nom_section] = 0;
      }
      
      list_parcours = list_categorie;
      while (list_parcours != NULL)
      {
        GtkWidget *widget = list_parcours->data;
        GtkWidget *grid = gtk_container_get_children (GTK_CONTAINER (widget))->data;
        GtkWidget *label = gtk_grid_get_child_at (GTK_GRID (grid), 1, 0);
        
        if (strcmp (categorie, gtk_label_get_text (GTK_LABEL (label))) == 0)
        {
          categorie_menu = widget;
          break;
        }
        
        list_parcours = g_list_next (list_parcours);
      }
      if (categorie_menu == NULL)
      {
        Section               section;
        Section_Personnalisee data;
        GtkWidget            *label;
        GtkWidget            *image;
        GtkWidget            *grid;
        
        section.data = &data;
        section.type = SECTION_PERSONNALISEE;
        data.forme = forme;
        
        categorie_menu = gtk_menu_item_new ();
        label = gtk_label_new (categorie);
        image = gtk_image_new_from_pixbuf (EF_gtk_sections_dessin (&section,
                                                                   32,
                                                                   32));
        grid = gtk_grid_new ();
        gtk_grid_attach (GTK_GRID (grid), image, 0, 0, 1, 1);
        gtk_grid_attach (GTK_GRID (grid), label, 1, 0, 2, 1);
        gtk_container_add (GTK_CONTAINER (categorie_menu), grid);
        
        gtk_menu_shell_append (GTK_MENU_SHELL (sous_menu), categorie_menu);
        gtk_menu_item_set_submenu (GTK_MENU_ITEM (categorie_menu),
                                   gtk_menu_new ());
        
        list_categorie = g_list_append (list_categorie, categorie_menu);
      }
      g_list_free_full (forme,
                       (GDestroyNotify) EF_sections_personnalisee_free_forme1);
      
      menu = gtk_menu_item_new_with_label (nom_section);
      gtk_menu_shell_append (GTK_MENU_SHELL (gtk_menu_item_get_submenu (
                                              GTK_MENU_ITEM (categorie_menu))),
                             menu);
      g_signal_connect (menu,
                        "activate",
                        G_CALLBACK (EF_gtk_sections_importe_section),
                        p);
      
      free (ligne);
      free (nom_section);
      free (categorie);
      ligne_tmp = common_text_get_line (file);
    }
    g_list_free (list_categorie);
    
    gtk_widget_show_all (sous_menu);
    
    fclose (file);
  }
  else
    gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
                         UI_SEC.builder, "EF_sections_ajouter_menu_importee")),
                              FALSE);
  
  gtk_window_set_transient_for (GTK_WINDOW (UI_SEC.window),
                                GTK_WINDOW (UI_GTK.window));
}

#endif

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
