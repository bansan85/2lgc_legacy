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
#include <libintl.h>
#include <locale.h>
#include <gtk/gtk.h>
#include <math.h>
#include <string.h>
#include "common_selection.h"
#include "common_erreurs.h"
#include "common_math.h"
#include "common_gtk.h"
#include "common_text.h"

/**
 * \brief Déselectionne la ligne sélectionnée si on clique sur une zone vide du
 *        treeview.
 * \param widget : composant à l'origine de l'évènement,
 * \param event : Caractéristique de l'évènement,
 * \param p : la variable projet.
 * \return FALSE.n
 *   Echec :
 *     - p == NULL.
 *  
 */
gboolean
common_gtk_treeview_button_press_unselect (GtkTreeView    *widget,
                                           GdkEventButton *event,
                                           Projet         *p)
{
  BUGPARAMCRIT (p, "%p", p, FALSE)
  
  if (event->type == GDK_BUTTON_PRESS)
  {
    if (!gtk_tree_view_get_path_at_pos (widget,
                                        (gint) event->x,
                                        (gint) event->y,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL))
    {
      GtkTreeIter Iter;
      GtkTreeSelection *select;
      
      select = gtk_tree_view_get_selection (widget);
      if (gtk_tree_selection_get_mode (select) == GTK_SELECTION_MULTIPLE)
      {
        gtk_tree_selection_unselect_all (select);
      }
      else if (gtk_tree_selection_get_selected (select, NULL, &Iter))
      {
        gtk_tree_selection_unselect_iter (select, &Iter);
      }
    }
  }
  
  return FALSE;
}


/**
 * \brief Vérifie en temps réel si le GtkTextBuffer contient bien un nombre
 *        flottant compris entre les valeurs val_min et val_max.
 *        S'il ne contient pas de nombre ou hors domaine, le texte passe en
 *        rouge.
 * \param textbuffer : composant à l'origine de l'évènement,
 * \param val_min : borne inférieure,
 * \param min_include : le nombre de la borne inférieure est-il autorisé ?,
 * \param val_max : borne supérieure,
 * \param max_include : le nombre de la borne supérieure est-il autorisé ?.
 * \return
 *   Succès : la valeur du nombre.\n
 *   Echec : NAN :
 *     - textbuffer == NULL,
 *     - textbuffer ne contient pas de nombre flottant.
 */
double
conv_buff_d (GtkTextBuffer *textbuffer,
             double         val_min,
             gboolean       min_include,
             double         val_max,
             gboolean       max_include)
{
  char       *texte;
  GtkTextIter start, end;
  double      nombre;
  
  BUGPARAMCRIT (textbuffer, "%p", textbuffer, NAN)
  
  gtk_text_buffer_get_iter_at_offset (textbuffer, &start, 0);
  gtk_text_buffer_get_iter_at_offset (textbuffer, &end, -1);
  texte = gtk_text_buffer_get_text (textbuffer, &start, &end, FALSE);
  
  gtk_text_buffer_remove_all_tags (textbuffer, &start, &end);
  
  nombre = common_text_str_to_double (texte,
                                      val_min,
                                      min_include,
                                      val_max,
                                      max_include);
  
  free (texte);
  
  if (!isnan (nombre))
  {
    gtk_text_buffer_apply_tag_by_name (textbuffer, "OK", &start, &end);
  }
  else
  {
    gtk_text_buffer_apply_tag_by_name (textbuffer, "mauvais", &start, &end);
  }
  
  return nombre;
}


/**
 * \brief Vérifie en temps réel si le GtkTextBuffer contient bien un nombre
 *        entier non signé compris entre les valeurs val_min et val_max.
 *        S'il ne contient pas de nombre ou hors domaine, le texte passe en
 *        rouge.
 * \param textbuffer : composant à l'origine de l'évènement,
 * \param val_min : borne inférieure,
 * \param min_include : borne inférieure autorisée ? (min 0),
 * \param val_max : borne supérieure,
 * \param max_include : borne supérieure autorisée ? (max UINT_MAX).
 * \return
 *   Succès : la valeur du nombre.\n
 *   Echec : UINT_MAX :
 *     - textbuffer == NULL,
 *     - textbuffer ne contient pas de nombre entier non signé.
 */
uint32_t
conv_buff_u (GtkTextBuffer *textbuffer,
             uint32_t       val_min,
             gboolean       min_include,
             uint32_t       val_max,
             gboolean       max_include)
{
  char       *texte;
  GtkTextIter start, end;
  uint32_t    nombre;
  char       *fake;
  gboolean    min_check;
  gboolean    max_check;
  
  BUGPARAMCRIT (textbuffer, "%p", textbuffer, UINT_MAX)
  
  gtk_text_buffer_get_iter_at_offset (textbuffer, &start, 0);
  gtk_text_buffer_get_iter_at_offset (textbuffer, &end, -1);
  texte = gtk_text_buffer_get_text (textbuffer, &start, &end, FALSE);
  BUGCRIT (fake = (char *) malloc (sizeof (char) * (strlen (texte) + 1)),
           UINT_MAX,
           (gettext ("Erreur d'allocation mémoire.\n"));
             free (texte); )
  
  gtk_text_buffer_remove_all_tags (textbuffer, &start, &end);
  if (sscanf (texte, "%u%s", &nombre, fake) != 1)
  {
    min_check = FALSE;
    max_check = FALSE;
  }
  else
  {
    if (min_include)
    {
      min_check = nombre >= val_min;
    }
    else
    {
      min_check = nombre > val_min;
    }
      
    if (max_include)
    {
      max_check = nombre <= val_max;
    }
    else
    {
      max_check = nombre < val_max;
    }
  }
  
  free (texte);
  free (fake);
  
  if ((min_check) && (max_check))
  {
    gtk_text_buffer_apply_tag_by_name (textbuffer, "OK", &start, &end);
    return nombre;
  }
  else
  {
    gtk_text_buffer_apply_tag_by_name (textbuffer, "mauvais", &start, &end);
    return UINT_MAX;
  }
}


/**
 * \brief Vérifie en temps réel si le GtkTextBuffer contient bien un nombre
 *        entier non signé en 16 bits compris entre les valeurs val_min et
 *        val_max. S'il ne contient pas de nombre ou hors domaine, le texte
 *        passe en rouge.
 * \param textbuffer : composant à l'origine de l'évènement,
 * \param val_min : borne inférieure,
 * \param min_include : borne inférieure autorisée ? (min 0),
 * \param val_max : borne supérieure,
 * \param max_include : borne supérieure autorisée ? (max UINT_MAX).
 * \return
 *   Succès : la valeur du nombre.\n
 *   Echec : UINT16_MAX :
 *     - textbuffer == NULL,
 *     - textbuffer ne contient pas de nombre entier non signé en 16 bits.
 */
uint16_t
conv_buff_hu (GtkTextBuffer *textbuffer,
              uint16_t       val_min,
              gboolean       min_include,
              uint16_t       val_max,
              gboolean       max_include)
{
  char       *texte;
  GtkTextIter start, end;
  uint16_t    nombre;
  char       *fake;
  gboolean    min_check;
  gboolean    max_check;
  
  BUGPARAMCRIT (textbuffer, "%p", textbuffer, UINT16_MAX)
  
  gtk_text_buffer_get_iter_at_offset (textbuffer, &start, 0);
  gtk_text_buffer_get_iter_at_offset (textbuffer, &end, -1);
  texte = gtk_text_buffer_get_text (textbuffer, &start, &end, FALSE);
  BUGCRIT (fake = (char *) malloc (sizeof (char) * (strlen (texte) + 1)),
           UINT16_MAX,
           (gettext ("Erreur d'allocation mémoire.\n"));
             free (texte); )
  
  gtk_text_buffer_remove_all_tags (textbuffer, &start, &end);
  if (sscanf (texte, "%hu%s", &nombre, fake) != 1)
  {
    min_check = FALSE;
    max_check = FALSE;
  }
  else
  {
    if (min_include)
    {
      min_check = nombre >= val_min;
    }
    else
    {
      min_check = nombre > val_min;
    }
      
    if (max_include)
    {
      max_check = nombre <= val_max;
    }
    else
    {
      max_check = nombre < val_max;
    }
  }
  
  free (texte);
  free (fake);
  
  if ((min_check) && (max_check))
  {
    gtk_text_buffer_apply_tag_by_name (textbuffer, "OK", &start, &end);
    return nombre;
  }
  else
  {
    gtk_text_buffer_apply_tag_by_name (textbuffer, "mauvais", &start, &end);
    return UINT16_MAX;
  }
}


/**
 * \brief Renvoie le nombre entier non signé de l'entry, valeur comprise entre
 *        les valeurs val_min et val_max.
 * \param entry : composant à l'origine de l'évènement,
 * \param val_min : borne inférieure,
 * \param min_include : borne inférieure autorisée ? (min 0),
 * \param val_max : borne supérieure,
 * \param max_include : borne supérieure autorisée ? (max UINT_MAX).
 * \return
 *   Succès : la valeur du nombre.\n
 *   Echec : UINT_MAX.
 *     - entry == NULL,
 *     - entry ne contient pas de nombre entier non signé.
 */
uint32_t
common_gtk_entry_uint (GtkEntry *entry,
                       uint32_t  val_min,
                       gboolean  min_include,
                       uint32_t  val_max,
                       gboolean  max_include)
{
  const char  *texte;
  uint32_t     nombre;
  char        *fake;
  gboolean     min_check;
  gboolean     max_check;
  
  BUGPARAMCRIT (entry, "%p", entry, UINT_MAX)
  
  texte = gtk_entry_get_text (entry);
  BUGCRIT (fake = (char *) malloc (sizeof (char) * (strlen (texte) + 1)),
           UINT_MAX,
           (gettext ("Erreur d'allocation mémoire.\n")); )
  
  if (sscanf (texte, "%u%s", &nombre, fake) != 1)
  {
    min_check = FALSE;
    max_check = FALSE;
  }
  else
  {
    if (min_include)
    {
      min_check = nombre >= val_min;
    }
    else
    {
      min_check = nombre > val_min;
    }
      
    if (max_include)
    {
      max_check = nombre <= val_max;
    }
    else
    {
      max_check = nombre < val_max;
    }
  }
  
  free (fake);
  
  if ((min_check) && (max_check))
  {
    return nombre;
  }
  else
  {
    return UINT_MAX;
  }
}


/**
 * \brief Personnalise l'affichage des nombres de type double dans un treeview.
 * \param tree_column : la colonne,
 * \param cell : la cellule,
 * \param tree_model : le tree_model,
 * \param iter : et le paramètre iter,
 * \param data : le nombre de décimale.
 * \return Rien.\n
 *   Échec :
 *     - cell == NULL,
 *     - tree_model == NULL,
 *     - iter == NULL.
 */
void
common_gtk_render_double (GtkTreeViewColumn *tree_column,
                          GtkCellRenderer   *cell,
                          GtkTreeModel      *tree_model,
                          GtkTreeIter       *iter,
                          gpointer           data)
{
  gchar  texte[30];
  gint   colonne;
  double nombre;
  int8_t decimales = (int8_t) GPOINTER_TO_INT (data);
  
  BUGPARAMCRIT (cell, "%p", cell, )
  BUGPARAMCRIT (tree_model, "%p", tree_model, )
  BUGPARAMCRIT (iter, "%p", iter, )
  
  colonne = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (cell), "column"));
  gtk_tree_model_get (tree_model, iter, colonne, &nombre, -1);
  
  common_math_double_to_char (nombre, texte, decimales);
  g_object_set (GTK_CELL_RENDERER_TEXT (cell), "text", texte, NULL);
  
  return;
}


/**
 * \brief Personnalise l'affichage des nombres de type double dans un treeview.
 * \param tree_column : la colonne,
 * \param cell : la cellule,
 * \param tree_model : le tree_model,
 * \param iter : et le paramètre iter,
 * \param data : le nombre de décimale.
 * \return Rien.
 */
void
common_gtk_render_flottant (GtkTreeViewColumn *tree_column,
                            GtkCellRenderer   *cell,
                            GtkTreeModel      *tree_model,
                            GtkTreeIter       *iter,
                            gpointer           data)
{
  gchar     texte[30];
  gint      colonne;
  Flottant *nombre;
  gint      decimales = GPOINTER_TO_INT(data);
  
  colonne = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (cell), "column"));
  gtk_tree_model_get (tree_model, iter, colonne, &nombre, -1);
  
  conv_f_c (*nombre, texte, (uint8_t) decimales);
  g_object_set (GTK_CELL_RENDERER_TEXT (cell), "text", texte, NULL);
  
  return;
}


/**
 * \brief Permet de créer une colonne pour un treeview.
 * \param nom : nom de la colonne,
 * \param type : type d'information que recevra la colonne,
 * \param num_colonne : numéro de la colonne.
 * \param xalign : alignement horizontal de la cellule dans la colonne,
 * \param num_decimales : si tvpe == G_TYPE_DOUBLE, défini le nombre de
 *                        décimales devant être affiché.
 * \return Un pointeur vers la nouvelle colonne.
 */
GtkTreeViewColumn *
common_gtk_cree_colonne (char    *nom,
                         GType    type,
                         uint32_t num_colonne,
                         float    xalign,
                         int8_t   num_decimales)
{
  GtkCellRenderer   *cell;
  GtkTreeViewColumn *column;
  GtkWidget         *label;
  
  if (type == G_TYPE_OBJECT)
  {
    cell = gtk_cell_renderer_pixbuf_new ();
  }
  else
  {
    cell = gtk_cell_renderer_text_new ();
  }
  
  // Pour les nombres flottants, l'alignement à droite permet aux virgules
  // d'être alignées.
  gtk_cell_renderer_set_alignment (cell, xalign, 0.5);
  
  label = gtk_label_new ("");
  gtk_label_set_markup (GTK_LABEL (label), nom);
  gtk_widget_set_visible (label, TRUE);
  
  column = gtk_tree_view_column_new ();
  gtk_tree_view_column_pack_start (column, cell, TRUE);
  if (type == G_TYPE_OBJECT)
  {
    gtk_tree_view_column_set_attributes (column,
                                         cell,
                                         "pixbuf",
                                         num_colonne,
                                         NULL);
  }
  else
  {
    gtk_tree_view_column_set_attributes (column,
                                         cell,
                                         "text",
                                         num_colonne,
                                         NULL);
  }
  gtk_tree_view_column_set_widget (column, label);
  gtk_tree_view_column_set_alignment (column, 0.5);
  
  if (type == G_TYPE_DOUBLE)
  {
    g_object_set_data (G_OBJECT (cell),
                       "column",
                       GINT_TO_POINTER (num_colonne));
    gtk_tree_view_column_set_cell_data_func (column,
                                             cell,
                                             common_gtk_render_double,
                                             GINT_TO_POINTER (num_decimales),
                                             NULL);
  }
  
  return column;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
