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
#include "common_selection.h"
#include "EF_sections.h"


GTK_WINDOW_KEY_PRESS (ef, section_carree);


GTK_WINDOW_DESTROY (ef, section_carree, );


GTK_WINDOW_CLOSE (ef, section_carree);


gboolean
EF_gtk_section_carree_recupere_donnees (Projet *p,
                                        double *cote,
                                        gchar **nom)
/**
 * \brief Récupère toutes les données de la fenêtre permettant d'ajouter ou
 *        d'éditer une section carrée.
 * \param p : la variable projet,
 * \param cote : le coté de la section,
 * \param nom : le nom de la section,
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL,
 *     - cote == NULL,
 *     - nom == NULL,
 *     - en cas d'erreur d'allocation mémoire.
 */
{
  GtkTextIter    start, end;
  GtkTextBuffer *textbuffer;
  gboolean       ok = TRUE;
  
  BUGMSG (p, FALSE, gettext ("Paramètre %s incorrect.\n"), "projet")
  BUGMSG (cote, FALSE, gettext ("Paramètre %s incorrect.\n"), "cote")
  BUGMSG (nom, FALSE, gettext ("Paramètre %s incorrect.\n"), "nom")
  BUGMSG (UI_SEC_CA.builder,
          FALSE,
          gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                   "Ajout Section Carrée")
  
  *cote = conv_buff_d (GTK_TEXT_BUFFER (gtk_builder_get_object (
                          UI_SEC_CA.builder, "EF_section_carree_buffer_cote")),
                       0,
                       FALSE,
                       INFINITY,
                       FALSE);
  if (isnan (*cote))
    ok = FALSE;
  
  // Si tous les paramètres sont corrects
  textbuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (
                                     gtk_builder_get_object (UI_SEC_CA.builder,
                                           "EF_section_carree_textview_nom")));
  
  gtk_text_buffer_get_iter_at_offset (textbuffer, &start, 0);
  gtk_text_buffer_get_iter_at_offset (textbuffer, &end, -1);
  *nom = gtk_text_buffer_get_text (textbuffer, &start, &end, FALSE);
  
  gtk_text_buffer_remove_all_tags (textbuffer, &start, &end);
  
  if (UI_SEC_CA.section == NULL)
  {
    if ((strcmp (*nom, "") == 0) || (EF_sections_cherche_nom (p, *nom, FALSE)))
    {
      gtk_text_buffer_apply_tag_by_name (textbuffer, "mauvais", &start, &end);
      ok = FALSE;
    }
    else
      gtk_text_buffer_apply_tag_by_name (textbuffer, "OK", &start, &end);
  }
  else if ((strcmp (*nom, "") == 0) ||
           ((strcmp (UI_SEC_CA.section->nom, *nom) != 0) &&
            (EF_sections_cherche_nom(p, *nom, FALSE))))
  {
    gtk_text_buffer_apply_tag_by_name (textbuffer, "mauvais", &start, &end);
    ok = FALSE;
  }
  else
    gtk_text_buffer_apply_tag_by_name (textbuffer, "OK", &start, &end);
  
  if (ok == FALSE)
  {
    free (*nom);
    *nom = NULL;
  }
  
  return ok;
}


void
EF_gtk_section_carree_check (GtkWidget *button,
                             Projet    *p)
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
{
  double cote;
  char  *nom;
  
  BUGMSG (p, , gettext ("Paramètre %s incorrect.\n"), "projet")
  BUGMSG (UI_SEC_CA.builder,
          ,
          gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                   "Ajout Section Carrée")
  
  if (!EF_gtk_section_carree_recupere_donnees (p, &cote, &nom))
    gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
                      UI_SEC_CA.builder, "EF_section_carree_button_add_edit")),
                              FALSE);
  else
  {
    gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
                      UI_SEC_CA.builder, "EF_section_carree_button_add_edit")),
                              TRUE);
    free (nom);
  }
  
  return;
}


void
EF_gtk_section_carree_ajouter_clicked (GtkButton *button,
                                       Projet    *p)
/**
 * \brief Ferme la fenêtre en ajoutant la section.
 * \param button : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
{
  double cote;
  gchar *texte;
  
  BUGMSG (p, , gettext ("Paramètre %s incorrect.\n"), "projet")
  BUGMSG (UI_SEC_CA.builder,
          ,
          gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                   "Ajout Section Carrée")
  
  if (!EF_gtk_section_carree_recupere_donnees (p, &cote, &texte))
    return;
  
  gtk_widget_destroy (UI_SEC_CA.window);
  
  BUG (EF_sections_carree_ajout (p, texte, m_f (cote, FLOTTANT_UTILISATEUR)), )
  
  free (texte);
  
  return;
}


void
EF_gtk_section_carree_modifier_clicked (GtkButton *button,
                                        Projet    *p)
/**
 * \brief Ferme la fenêtre en appliquant les modifications.
 * \param button : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
{
  double cote;
  gchar *texte;
  
  BUGMSG (p, , gettext ("Paramètre %s incorrect.\n"), "projet")
  BUGMSG (UI_SEC_CA.builder,
          ,
          gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                   "Ajout Section Carrée")
  
  if (!EF_gtk_section_carree_recupere_donnees (p, &cote, &texte))
    return;
  
  BUG (EF_sections_carree_modif (p,
                                 UI_SEC_CA.section,
                                 texte,
                                 m_f (cote, FLOTTANT_UTILISATEUR)),
      )
  
  free (texte);
  
  gtk_widget_destroy (UI_SEC_CA.window);
  
  return;
}


gboolean
EF_gtk_section_carree (Projet  *p,
                       Section *section)
/**
 * \brief Affichage de la fenêtre permettant de créer ou modifier une section
 *        de type carré.
 * \param p : la variable projet,
 * \param section : section à modifier. NULL si nouvelle section.
 * \return
 *   Succès : TRUE.\n
 *   Echec : FALSE :
 *     - p == NULL,
 *     - interface graphique impossible à générer.
 */
{
  BUGMSG (p, FALSE, gettext ("Paramètre %s incorrect.\n"), "projet")
  
  if (UI_SEC_CA.builder != NULL)
  {
    gtk_window_present (GTK_WINDOW (UI_SEC_CA.window));
    if (UI_SEC_CA.section == section)
      return TRUE;
  }
  else
  {
    UI_SEC_CA.builder = gtk_builder_new ();
    BUGMSG (gtk_builder_add_from_resource (UI_SEC_CA.builder,
                                  "/org/2lgc/codegui/ui/EF_sections_carree.ui",
                                           NULL) != 0,
            FALSE,
            gettext ("Builder Failed\n"))
    gtk_builder_connect_signals (UI_SEC_CA.builder, p);
    UI_SEC_CA.window = GTK_WIDGET (gtk_builder_get_object (UI_SEC_CA.builder,
                                                  "EF_section_carree_window"));
  }
  
  if (section == NULL)
  {
    gtk_window_set_title (GTK_WINDOW (UI_SEC_CA.window),
                          gettext ("Ajout d'une section carrée"));
    UI_SEC_CA.section = NULL;
    
    gtk_button_set_label (GTK_BUTTON (gtk_builder_get_object (
                      UI_SEC_CA.builder, "EF_section_carree_button_add_edit")),
                          gettext ("_Ajouter"));
    g_signal_connect (gtk_builder_get_object (UI_SEC_CA.builder,
                                          "EF_section_carree_button_add_edit"),
                      "clicked",
                      G_CALLBACK (EF_gtk_section_carree_ajouter_clicked),
                      p);
    EF_gtk_section_carree_check (NULL, p);
  }
  else
  {
    gchar      tmp[30];
    Section_T *data;
    
    gtk_window_set_title (GTK_WINDOW (UI_SEC_CA.window),
                          gettext ("Modification d'une section carrée"));
    UI_SEC_CA.section = section;
    BUGMSG (UI_SEC_CA.section->type == SECTION_CARREE,
            FALSE,
            gettext ("La section à modifier n'est pas carrée.\n"))
    data = UI_SEC_CA.section->data;
    
    gtk_text_buffer_set_text (gtk_text_view_get_buffer (GTK_TEXT_VIEW (
                                     gtk_builder_get_object (UI_SEC_CA.builder,
                                           "EF_section_carree_textview_nom"))),
                              UI_SEC_CA.section->nom,
                              -1);
    conv_f_c (data->largeur_table, tmp, DECIMAL_DISTANCE);
    gtk_text_buffer_set_text (GTK_TEXT_BUFFER (gtk_builder_get_object (
                          UI_SEC_CA.builder, "EF_section_carree_buffer_cote")),
                              tmp,
                              -1);
    
    gtk_button_set_label (GTK_BUTTON (gtk_builder_get_object (
                      UI_SEC_CA.builder, "EF_section_carree_button_add_edit")),
                          gettext ("_Modifier"));
    g_signal_connect (gtk_builder_get_object (UI_SEC_CA.builder,
                                          "EF_section_carree_button_add_edit"),
                      "clicked",
                      G_CALLBACK (EF_gtk_section_carree_modifier_clicked),
                      p);
  }
  
  gtk_window_set_transient_for (GTK_WINDOW (UI_SEC_CA.window),
                                GTK_WINDOW (p->ui.comp.window));
  
  return TRUE;
}


#endif
