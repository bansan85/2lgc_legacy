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


GTK_WINDOW_KEY_PRESS (ef, section_rectangulaire);


GTK_WINDOW_DESTROY (ef, section_rectangulaire, );


GTK_WINDOW_CLOSE (ef, section_rectangulaire);


gboolean
EF_gtk_section_rectangulaire_recupere_donnees (Projet *p,
                                               double *largeur,
                                               double *hauteur,
                                               gchar **nom)
/**
 * \brief Récupère toutes les données de la fenêtre permettant d'ajouter ou
 *        d'éditer une section rectangulaire.
 * \param p : la variable projet,
 * \param largeur : la largeur de la section,
 * \param hauteur : la hauteur de la section,
 * \param nom : le nom de la section,
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *   - p == NULL,
 *   - largeur == NULL,
 *   - hauteur == NULL,
 *   - nom == NULL,
 *   - en cas d'erreur d'allocation mémoire.
 */
{
  GtkTextIter    start, end;
  GtkTextBuffer *textbuffer;
  gboolean       ok = TRUE;
  
  BUGMSG (p, FALSE, gettext ("Paramètre %s incorrect.\n"), "projet")
  BUGMSG (largeur, FALSE, gettext ("Paramètre %s incorrect.\n"), "largeur")
  BUGMSG (hauteur, FALSE, gettext ("Paramètre %s incorrect.\n"), "hauteur")
  BUGMSG (nom, FALSE, gettext ("Paramètre %s incorrect.\n"), "nom")
  BUGMSG (UI_SEC_RE.builder,
          FALSE,
          gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                   "Ajout Section Rectangulaire")
  
  *largeur = conv_buff_d (GTK_TEXT_BUFFER (gtk_builder_get_object (
                UI_SEC_RE.builder, "EF_section_rectangulaire_buffer_largeur")),
                          0,
                          FALSE,
                          INFINITY,
                          FALSE);
  if (isnan (*largeur))
    ok = FALSE;
  
  *hauteur = conv_buff_d (GTK_TEXT_BUFFER (gtk_builder_get_object (
                UI_SEC_RE.builder, "EF_section_rectangulaire_buffer_hauteur")),
                          0,
                          FALSE,
                          INFINITY,
                          FALSE);
  if (isnan (*hauteur))
    ok = FALSE;
  
  // Si tous les paramètres sont corrects
  textbuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (
                                     gtk_builder_get_object (UI_SEC_RE.builder,
                                    "EF_section_rectangulaire_textview_nom")));
  
  gtk_text_buffer_get_iter_at_offset (textbuffer, &start, 0);
  gtk_text_buffer_get_iter_at_offset (textbuffer, &end, -1);
  *nom = gtk_text_buffer_get_text (textbuffer, &start, &end, FALSE);
  
  gtk_text_buffer_remove_all_tags (textbuffer, &start, &end);
  
  if (UI_SEC_RE.section == NULL)
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
           ((strcmp (UI_SEC_RE.section->nom, *nom) != 0) &&
            (EF_sections_cherche_nom (p, *nom, FALSE))))
  {
    gtk_text_buffer_apply_tag_by_name (textbuffer, "mauvais", &start, &end);
    ok = FALSE;
  }
  else
    gtk_text_buffer_apply_tag_by_name (textbuffer, "OK", &start, &end);
  
  if (ok == FALSE)
    free (*nom);
  
  return ok;
}


void
EF_gtk_section_rectangulaire_check (GtkWidget *button,
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
  double largeur, hauteur;
  char  *nom;
  
  BUGMSG (p, , gettext ("Paramètre %s incorrect.\n"), "projet")
  BUGMSG (UI_SEC_RE.builder,
          ,
          gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                   "Ajout Section Rectangulaire")
  
  if (!EF_gtk_section_rectangulaire_recupere_donnees (p,
                                                      &largeur,
                                                      &hauteur,
                                                      &nom))
    gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
               UI_SEC_RE.builder, "EF_section_rectangulaire_button_add_edit")),
                              FALSE);
  else
  {
    gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
               UI_SEC_RE.builder, "EF_section_rectangulaire_button_add_edit")),
                              TRUE);
    free (nom);
  }
  
  return;
}


void
EF_gtk_section_rectangulaire_ajouter_clicked (GtkButton *button,
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
  double largeur, hauteur;
  gchar   *texte;
  
  BUGMSG (p, , gettext ("Paramètre %s incorrect.\n"), "projet")
  BUGMSG (UI_SEC_RE.builder,
          ,
          gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                   "Ajout Section Rectangulaire")
  
  if (!(EF_gtk_section_rectangulaire_recupere_donnees (p,
                                                       &largeur,
                                                       &hauteur,
                                                       &texte)))
    return;
  
  // Création de la nouvelle charge ponctuelle au noeud
  BUG (EF_sections_rectangulaire_ajout (p,
                                        texte,
                                        m_f (largeur, FLOTTANT_UTILISATEUR),
                                        m_f (hauteur, FLOTTANT_UTILISATEUR)),
      )
  
  free (texte);
  
  gtk_widget_destroy (UI_SEC_RE.window);
  
  return;
}


void
EF_gtk_section_rectangulaire_modifier_clicked (GtkButton *button,
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
  double largeur, hauteur;
  gchar *texte;
  
  BUGMSG (p, , gettext ("Paramètre %s incorrect.\n"), "projet")
  BUGMSG (UI_SEC_RE.builder,
          ,
          gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                   "Ajout Section Rectangulaire")
  
  if (!(EF_gtk_section_rectangulaire_recupere_donnees (p,
                                                       &largeur,
                                                       &hauteur,
                                                       &texte)))
    return;
  
  gtk_widget_destroy (UI_SEC_RE.window);
  
  BUG (EF_sections_rectangulaire_modif (p,
                                        UI_SEC_RE.section,
                                        texte,
                                        m_f (largeur, FLOTTANT_UTILISATEUR),
                                        m_f (hauteur, FLOTTANT_UTILISATEUR)),
      )
  
  free (texte);
  
  return;
}


gboolean
EF_gtk_section_rectangulaire (Projet  *p,
                              Section *section)
/**
 * \brief Affichage de la fenêtre permettant de créer ou modifier une section
 *        de type rectangulaire.
 * \param p : la variable projet,
 * \param section : section à modifier. NULL si nouvelle section,
 * \return
 *   Succès : TRUE.\n
 *   Echec : FALSE :
 *     - p == NULL,
 *     - interface graphique déjà initialisée.
 */
{
  BUGMSG (p, FALSE, gettext ("Paramètre %s incorrect.\n"), "projet")
  
  if (UI_SEC_RE.builder != NULL)
  {
    gtk_window_present (GTK_WINDOW (UI_SEC_RE.window));
    if (UI_SEC_RE.section == section)
      return TRUE;
  }
  else
  {
    UI_SEC_RE.builder = gtk_builder_new ();
    BUGMSG (gtk_builder_add_from_resource (UI_SEC_RE.builder,
                           "/org/2lgc/codegui/ui/EF_sections_rectangulaire.ui",
                                           NULL) != 0,
            FALSE,
            gettext ("Builder Failed\n"))
    gtk_builder_connect_signals (UI_SEC_RE.builder, p);
    UI_SEC_RE.window = GTK_WIDGET (gtk_builder_get_object (UI_SEC_RE.builder,
                                           "EF_section_rectangulaire_window"));
  }
  
  if (section == NULL)
  {
    gtk_window_set_title (GTK_WINDOW (UI_SEC_RE.window),
                          gettext ("Ajout d'une section rectangulaire"));
    UI_SEC_RE.section = NULL;
    
    gtk_button_set_label (GTK_BUTTON (gtk_builder_get_object (
               UI_SEC_RE.builder, "EF_section_rectangulaire_button_add_edit")),
                          gettext ("_Ajouter"));
    g_signal_connect (gtk_builder_get_object (UI_SEC_RE.builder,
                                   "EF_section_rectangulaire_button_add_edit"),
                      "clicked",
                     G_CALLBACK (EF_gtk_section_rectangulaire_ajouter_clicked),
                      p);
    EF_gtk_section_rectangulaire_check (NULL, p);
  }
  else
  {
    gchar      tmp[30];
    Section_T *data;
    
    gtk_window_set_title (GTK_WINDOW (UI_SEC_RE.window),
                          gettext ("Modification d'une section rectangulaire"));
    UI_SEC_RE.section = section;
    BUGMSG (UI_SEC_RE.section->type == SECTION_RECTANGULAIRE,
            FALSE,
            gettext ("La section à modifier n'est pas rectangulaire.\n"))
    data = UI_SEC_RE.section->data;
    
    gtk_text_buffer_set_text (gtk_text_view_get_buffer (GTK_TEXT_VIEW (
                                     gtk_builder_get_object (UI_SEC_RE.builder,
                                    "EF_section_rectangulaire_textview_nom"))),
                              UI_SEC_RE.section->nom,
                              -1);
    conv_f_c (data->largeur_retombee, tmp, DECIMAL_DISTANCE);
    gtk_text_buffer_set_text (GTK_TEXT_BUFFER (gtk_builder_get_object (
                UI_SEC_RE.builder, "EF_section_rectangulaire_buffer_largeur")),
                              tmp,
                              -1);
    conv_f_c (data->hauteur_retombee, tmp, DECIMAL_DISTANCE);
    gtk_text_buffer_set_text (GTK_TEXT_BUFFER (gtk_builder_get_object (
                UI_SEC_RE.builder, "EF_section_rectangulaire_buffer_hauteur")),
                              tmp,
                              -1);
    
    gtk_button_set_label (GTK_BUTTON (gtk_builder_get_object (
               UI_SEC_RE.builder, "EF_section_rectangulaire_button_add_edit")),
                          gettext ("_Modifier"));
    g_signal_connect (gtk_builder_get_object (UI_SEC_RE.builder,
                                   "EF_section_rectangulaire_button_add_edit"),
                      "clicked",
                    G_CALLBACK (EF_gtk_section_rectangulaire_modifier_clicked),
                      p);
  }
  
  gtk_window_set_transient_for (GTK_WINDOW (UI_SEC_RE.window),
                                GTK_WINDOW (p->ui.comp.window));
  
  return TRUE;
}


#endif

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
