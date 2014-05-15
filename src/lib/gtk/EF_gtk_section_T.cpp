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

#include "common_projet.hpp"
#include "common_erreurs.hpp"
#include "common_gtk.hpp"
#include "common_math.hpp"
#include "common_selection.hpp"
#include "EF_sections.hpp"
#include "EF_gtk_section_T.hpp"


GTK_WINDOW_KEY_PRESS (ef, section_T);


GTK_WINDOW_DESTROY (ef, section_T, );


GTK_WINDOW_CLOSE (ef, section_T);


/**
 * \brief Récupère toutes les données de la fenêtre permettant d'ajouter ou
 *        d'éditer une section en T.
 * \param p : la variable projet,
 * \param lt : la largeur de la table de la section,
 * \param ht : la hauteur de la table de la section,
 * \param lr : la largeur de la retombée de la section,
 * \param hr : la hauteur de la retombée de la section,
 * \param nom : le nom de la section,
 * \return
 *   Succès : true.\n
 *   Échec : false :
 *     - p == NULL,
 *     - lt == NULL,
 *     - ht == NULL,
 *     - la == NULL,
 *     - ha == NULL,
 *     - nom == NULL,
 *     - en cas d'erreur d'allocation mémoire.
 */
bool
EF_gtk_section_T_recupere_donnees (Projet *p,
                                   double *lt,
                                   double *ht,
                                   double *lr,
                                   double *hr,
                                   gchar **nom)
{
  GtkTextIter    start, end;
  GtkTextBuffer *textbuffer;
  bool           ok = true;
  
  BUGPARAMCRIT (p, "%p", p, false)
  BUGPARAMCRIT (lt, "%p", lt, false)
  BUGPARAMCRIT (ht, "%p", ht, false)
  BUGPARAMCRIT (lr, "%p", lr, false)
  BUGPARAMCRIT (hr, "%p", hr, false)
  BUGPARAMCRIT (nom, "%p", nom, false)
  BUGCRIT (UI_SEC_T.builder,
           false,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Ajout Section T"); )
  
  *lr = conv_buff_d (GTK_TEXT_BUFFER (gtk_builder_get_object (UI_SEC_T.builder,
                                                    "EF_section_T_buffer_lr")),
                     0., false,
                     INFINITY, false);
  if (isnan (*lr))
  {
    ok = false;
  }
  
  *hr = conv_buff_d (GTK_TEXT_BUFFER (gtk_builder_get_object (UI_SEC_T.builder,
                                                    "EF_section_T_buffer_hr")),
                     0., false,
                     INFINITY, false);
  if (isnan (*hr))
  {
    ok = false;
  }
  
  *lt = conv_buff_d (GTK_TEXT_BUFFER (gtk_builder_get_object (UI_SEC_T.builder,
                                                    "EF_section_T_buffer_lt")),
                     0., false,
                     INFINITY, false);
  if (isnan (*lt))
  {
    ok = false;
  }
  
  *ht = conv_buff_d (GTK_TEXT_BUFFER (gtk_builder_get_object (UI_SEC_T.builder,
                                                    "EF_section_T_buffer_ht")),
                     0., false,
                     INFINITY, false);
  if (isnan (*ht))
  {
    ok = false;
  }
  
  // Si tous les paramètres sont corrects
  textbuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (
      gtk_builder_get_object (UI_SEC_T.builder, "EF_section_T_textview_nom")));
  
  gtk_text_buffer_get_iter_at_offset (textbuffer, &start, 0);
  gtk_text_buffer_get_iter_at_offset (textbuffer, &end, -1);
  *nom = gtk_text_buffer_get_text (textbuffer, &start, &end, FALSE);
  
  gtk_text_buffer_remove_all_tags (textbuffer, &start, &end);
  
  if (UI_SEC_T.section == NULL)
  {
    if ((strcmp (*nom, "") == 0) ||
        (EF_sections_cherche_nom (p, *nom, false)))
    {
      gtk_text_buffer_apply_tag_by_name (textbuffer, "mauvais", &start, &end);
      ok = false;
    }
    else
    {
      gtk_text_buffer_apply_tag_by_name (textbuffer, "OK", &start, &end);
    }
  }
  else if ((strcmp (*nom, "") == 0) ||
           ((strcmp (UI_SEC_T.section->nom, *nom) != 0) &&
            (EF_sections_cherche_nom (p, *nom, false))))
  {
    gtk_text_buffer_apply_tag_by_name (textbuffer, "mauvais", &start, &end);
    ok = false;
  }
  else
  {
    gtk_text_buffer_apply_tag_by_name (textbuffer, "OK", &start, &end);
  }
  
  if (!ok)
  {
    free (*nom);
    *nom = NULL;
  }
  
  return ok;
}


/**
 * \brief Vérifie si l'ensemble des éléments est correct pour activer le bouton
 *        add/edit.
 * \param button : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
extern "C"
void
EF_gtk_section_T_check (GtkWidget *button,
                        Projet    *p)
{
  double lt, ht, lr, hr;
  char  *nom;
  
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_SEC_T.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Ajout Section T"); )
  
  if (!EF_gtk_section_T_recupere_donnees (p, &lt, &ht, &lr, &hr, &nom))
  {
    gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
                            UI_SEC_T.builder, "EF_section_T_button_add_edit")),
                              FALSE);
  }
  else
  {
    gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
                            UI_SEC_T.builder, "EF_section_T_button_add_edit")),
                              TRUE);
    free (nom);
  }
  
  return;
}


/**
 * \brief Ferme la fenêtre en ajoutant la section.
 * \param button : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
void
EF_gtk_section_T_ajouter_clicked (GtkButton *button,
                                  Projet    *p)
{
  double lr, hr, lt, ht;
  gchar *texte;
  
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_SEC_T.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Ajout Section T"); )
  
  if (!(EF_gtk_section_T_recupere_donnees (p, &lt, &ht, &lr, &hr, &texte)))
  {
    return;
  }
  
  // Création de la nouvelle charge ponctuelle au noeud
  BUG (EF_sections_T_ajout (p,
                            texte,
                            m_f (lt, FLOTTANT_UTILISATEUR),
                            m_f (lr, FLOTTANT_UTILISATEUR),
                            m_f (ht, FLOTTANT_UTILISATEUR),
                            m_f (hr, FLOTTANT_UTILISATEUR)),
      ,
      free (texte); )
  
  free (texte);
  
  gtk_widget_destroy (UI_SEC_T.window);
  
  return;
}


/**
 * \brief Ferme la fenêtre en appliquant les modifications.
 * \param button : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
void
EF_gtk_section_T_modifier_clicked (GtkButton *button,
                                   Projet    *p)
{
  double lt, ht, lr, hr;
  gchar *texte;
  
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_SEC_T.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Ajout Section T"); )
  
  if (!(EF_gtk_section_T_recupere_donnees (p, &lt, &ht, &lr, &hr, &texte)))
  {
    return;
  }
  
  gtk_widget_destroy (UI_SEC_T.window);
  
  BUG (EF_sections_T_modif (p,
                            UI_SEC_T.section,
                            texte,
                            m_f (lt, FLOTTANT_UTILISATEUR),
                            m_f (lr, FLOTTANT_UTILISATEUR),
                            m_f (ht, FLOTTANT_UTILISATEUR),
                            m_f (hr, FLOTTANT_UTILISATEUR)),
      ,
      free (texte); )
  
  free (texte);
  
  return;
}


/**
 * \brief Affichage de la fenêtre permettant de créer ou modifier une section
 *        de type en T.
 * \param p : la variable projet,
 * \param section : section à modifier. NULL si nouvelle section,
 * \return
 *   Succès : true.\n
 *   Echec : false :
 *     - p == NULL,
 *     - interface graphique impossible à générer.
 */
bool
EF_gtk_section_T (Projet  *p,
                  Section *section)
{
  BUGPARAM (p, "%p", p, false)
  
  if (UI_SEC_T.builder != NULL)
  {
    gtk_window_present (GTK_WINDOW (UI_SEC_T.window));
    if (UI_SEC_T.section == section)
    {
      return true;
    }
  }
  else
  {
    UI_SEC_T.builder = gtk_builder_new ();
    BUGCRIT (gtk_builder_add_from_resource (UI_SEC_T.builder,
                                       "/org/2lgc/codegui/ui/EF_sections_T.ui",
                                            NULL) != 0,
             false,
             (gettext ("La génération de la fenêtre %s a échouée.\n"),
                       "Ajout Section T"); )
    gtk_builder_connect_signals (UI_SEC_T.builder, p);
    UI_SEC_T.window = GTK_WIDGET (gtk_builder_get_object (UI_SEC_T.builder,
                                                       "EF_section_T_window"));
  }
  
  if (section == NULL)
  {
    gtk_window_set_title (GTK_WINDOW (UI_SEC_T.window),
                          gettext ("Ajout d'une section en T"));
    UI_SEC_T.section = NULL;
    
    gtk_button_set_label (GTK_BUTTON (gtk_builder_get_object (UI_SEC_T.builder,
                                              "EF_section_T_button_add_edit")),
                          gettext ("_Ajouter"));
    g_signal_connect (gtk_builder_get_object (UI_SEC_T.builder,
                                               "EF_section_T_button_add_edit"),
                      "clicked",
                      G_CALLBACK (EF_gtk_section_T_ajouter_clicked),
                      p);
    EF_gtk_section_T_check (NULL, p);
  }
  else
  {
    gchar      tmp[30];
    Section_T *data;
    
    gtk_window_set_title (GTK_WINDOW (UI_SEC_T.window),
                          gettext ("Modification d'une section en T"));
    UI_SEC_T.section = section;
    BUGCRIT (UI_SEC_T.section->type == SECTION_T,
             false,
             (gettext ("La section à modifier n'est pas en T.\n")); )
    data = (Section_T *) UI_SEC_T.section->data;
    
    gtk_text_buffer_set_text (gtk_text_view_get_buffer (GTK_TEXT_VIEW (
      gtk_builder_get_object (UI_SEC_T.builder, "EF_section_T_textview_nom"))),
                              UI_SEC_T.section->nom,
                              -1);
    conv_f_c (data->largeur_table, tmp, DECIMAL_DISTANCE);
    gtk_text_buffer_set_text (GTK_TEXT_BUFFER (gtk_builder_get_object (
                                  UI_SEC_T.builder, "EF_section_T_buffer_lt")),
                              tmp,
                              -1);
    conv_f_c (data->hauteur_table, tmp, DECIMAL_DISTANCE);
    gtk_text_buffer_set_text (GTK_TEXT_BUFFER (gtk_builder_get_object (
                                  UI_SEC_T.builder, "EF_section_T_buffer_ht")),
                              tmp,
                              -1);
    conv_f_c (data->largeur_retombee, tmp, DECIMAL_DISTANCE);
    gtk_text_buffer_set_text (GTK_TEXT_BUFFER (gtk_builder_get_object (
                                  UI_SEC_T.builder, "EF_section_T_buffer_lr")),
                              tmp,
                              -1);
    conv_f_c (data->hauteur_retombee, tmp, DECIMAL_DISTANCE);
    gtk_text_buffer_set_text (GTK_TEXT_BUFFER (gtk_builder_get_object (
                                  UI_SEC_T.builder, "EF_section_T_buffer_hr")),
                              tmp,
                              -1);
    
    gtk_button_set_label (GTK_BUTTON (gtk_builder_get_object (UI_SEC_T.builder,
                                              "EF_section_T_button_add_edit")),
                          gettext ("_Modifier"));
    g_signal_connect (gtk_builder_get_object (UI_SEC_T.builder,
                                               "EF_section_T_button_add_edit"),
                      "clicked",
                      G_CALLBACK (EF_gtk_section_T_modifier_clicked),
                      p);
  }
  
  gtk_window_set_transient_for (GTK_WINDOW (UI_SEC_T.window),
                                GTK_WINDOW (UI_GTK.window));
  
  return true;
}


#endif

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
