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

#include <locale>
#include <cmath>

#include <gtk/gtk.h>

#include "common_projet.hpp"
#include "common_erreurs.hpp"
#include "common_gtk.hpp"
#include "common_math.hpp"
#include "common_selection.hpp"
#include "EF_sections.hpp"
#include "EF_gtk_section_rectangulaire.hpp"


GTK_WINDOW_KEY_PRESS (ef, section_rectangulaire);


GTK_WINDOW_DESTROY (ef, section_rectangulaire, );


GTK_WINDOW_CLOSE (ef, section_rectangulaire);


/**
 * \brief Récupère toutes les données de la fenêtre permettant d'ajouter ou
 *        d'éditer une section rectangulaire.
 * \param p : la variable projet,
 * \param largeur : la largeur de la section,
 * \param hauteur : la hauteur de la section,
 * \param nom : le nom de la section,
 * \return
 *   Succès : true.\n
 *   Échec : false :
 *   - p == NULL,
 *   - largeur == NULL,
 *   - hauteur == NULL,
 *   - nom == NULL,
 *   - en cas d'erreur d'allocation mémoire.
 */
bool
EF_gtk_section_rectangulaire_recupere_donnees (Projet      *p,
                                               double      *largeur,
                                               double      *hauteur,
                                               std::string *nom)
{
  GtkTextIter    start, end;
  GtkTextBuffer *textbuffer;
  bool           ok = true;
  char           *txt;
  
  BUGPARAMCRIT (p, "%p", p, false)
  BUGPARAMCRIT (largeur, "%p", largeur, false)
  BUGPARAMCRIT (hauteur, "%p", hauteur, false)
  BUGPARAMCRIT (nom, "%p", nom, false)
  BUGCRIT (UI_SEC_RE.builder,
           false,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Ajout Section Rectangulaire"); )
  
  *largeur = conv_buff_d (GTK_TEXT_BUFFER (gtk_builder_get_object (
                UI_SEC_RE.builder, "EF_section_rectangulaire_buffer_largeur")),
                          0, false,
                          INFINITY, false);
  if (std::isnan (*largeur))
  {
    ok = false;
  }
  
  *hauteur = conv_buff_d (GTK_TEXT_BUFFER (gtk_builder_get_object (
                UI_SEC_RE.builder, "EF_section_rectangulaire_buffer_hauteur")),
                          0, false,
                          INFINITY, false);
  if (std::isnan (*hauteur))
  {
    ok = false;
  }
  
  // Si tous les paramètres sont corrects
  textbuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (
                                     gtk_builder_get_object (UI_SEC_RE.builder,
                                    "EF_section_rectangulaire_textview_nom")));
  
  gtk_text_buffer_get_iter_at_offset (textbuffer, &start, 0);
  gtk_text_buffer_get_iter_at_offset (textbuffer, &end, -1);
  txt = gtk_text_buffer_get_text (textbuffer, &start, &end, FALSE);
  *nom = txt;
  free (txt);
  
  gtk_text_buffer_remove_all_tags (textbuffer, &start, &end);
  
  if (UI_SEC_RE.section == NULL)
  {
    if ((nom->empty ()) || (EF_sections_cherche_nom (p, nom, false)))
    {
      gtk_text_buffer_apply_tag_by_name (textbuffer, "mauvais", &start, &end);
      ok = false;
    }
    else
    {
      gtk_text_buffer_apply_tag_by_name (textbuffer, "OK", &start, &end);
    }
  }
  else if ((nom->empty ()) ||
           ((UI_SEC_RE.section->nom.compare (*nom) != 0) &&
            (EF_sections_cherche_nom (p, nom, false))))
  {
    gtk_text_buffer_apply_tag_by_name (textbuffer, "mauvais", &start, &end);
    ok = false;
  }
  else
  {
    gtk_text_buffer_apply_tag_by_name (textbuffer, "OK", &start, &end);
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
EF_gtk_section_rectangulaire_check (GtkWidget *button,
                                    Projet    *p)
{
  double      largeur, hauteur;
  std::string nom;
  
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_SEC_RE.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Ajout Section Rectangulaire"); )
  
  if (!EF_gtk_section_rectangulaire_recupere_donnees (p,
                                                      &largeur,
                                                      &hauteur,
                                                      &nom))
  {
    gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
               UI_SEC_RE.builder, "EF_section_rectangulaire_button_add_edit")),
                              FALSE);
  }
  else
  {
    gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
               UI_SEC_RE.builder, "EF_section_rectangulaire_button_add_edit")),
                              TRUE);
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
EF_gtk_section_rectangulaire_ajouter_clicked (GtkButton *button,
                                              Projet    *p)
{
  double      largeur, hauteur;
  std::string texte;
  
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_SEC_RE.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Ajout Section Rectangulaire"); )
  
  if (!(EF_gtk_section_rectangulaire_recupere_donnees (p,
                                                       &largeur,
                                                       &hauteur,
                                                       &texte)))
  {
    return;
  }
  
  // Création de la nouvelle charge ponctuelle au noeud
  BUG (EF_sections_rectangulaire_ajout (p,
                                        &texte,
                                        m_f (largeur, FLOTTANT_UTILISATEUR),
                                        m_f (hauteur, FLOTTANT_UTILISATEUR)),
      , )
  
  gtk_widget_destroy (UI_SEC_RE.window);
  
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
EF_gtk_section_rectangulaire_modifier_clicked (GtkButton *button,
                                               Projet    *p)
{
  double      largeur, hauteur;
  std::string texte;
  
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_SEC_RE.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Ajout Section Rectangulaire"); )
  
  if (!(EF_gtk_section_rectangulaire_recupere_donnees (p,
                                                       &largeur,
                                                       &hauteur,
                                                       &texte)))
  {
    return;
  }
  
  gtk_widget_destroy (UI_SEC_RE.window);
  
  BUG (EF_sections_rectangulaire_modif (p,
                                        UI_SEC_RE.section,
                                        &texte,
                                        m_f (largeur, FLOTTANT_UTILISATEUR),
                                        m_f (hauteur, FLOTTANT_UTILISATEUR)),
      , )
  
  return;
}


/**
 * \brief Affichage de la fenêtre permettant de créer ou modifier une section
 *        de type rectangulaire.
 * \param p : la variable projet,
 * \param section : section à modifier. NULL si nouvelle section,
 * \return
 *   Succès : true.\n
 *   Echec : false :
 *     - p == NULL,
 *     - interface graphique déjà initialisée.
 */
bool
EF_gtk_section_rectangulaire (Projet  *p,
                              Section *section)
{
  BUGPARAM (p, "%p", p, false)
  
  if (UI_SEC_RE.builder != NULL)
  {
    gtk_window_present (GTK_WINDOW (UI_SEC_RE.window));
    if (UI_SEC_RE.section == section)
    {
      return true;
    }
  }
  else
  {
    UI_SEC_RE.builder = gtk_builder_new ();
    BUGCRIT (gtk_builder_add_from_resource (UI_SEC_RE.builder,
                           "/org/2lgc/codegui/ui/EF_sections_rectangulaire.ui",
                                            NULL) != 0,
             false,
             (gettext ("La génération de la fenêtre %s a échouée.\n"),
                       "Ajout Section Rectangulaire"); )
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
    std::string tmp;
    Section_T  *data;
    
    gtk_window_set_title (GTK_WINDOW (UI_SEC_RE.window),
                          gettext ("Modification d'une section rectangulaire"));
    UI_SEC_RE.section = section;
    BUGCRIT (UI_SEC_RE.section->type == SECTION_RECTANGULAIRE,
             false,
             (gettext ("La section à modifier n'est pas rectangulaire.\n")); )
    data = (Section_T *) UI_SEC_RE.section->data;
    
    gtk_text_buffer_set_text (gtk_text_view_get_buffer (GTK_TEXT_VIEW (
                                     gtk_builder_get_object (UI_SEC_RE.builder,
                                    "EF_section_rectangulaire_textview_nom"))),
                              UI_SEC_RE.section->nom.c_str (),
                              -1);
    conv_f_c (data->largeur_retombee, &tmp, DECIMAL_DISTANCE);
    gtk_text_buffer_set_text (GTK_TEXT_BUFFER (gtk_builder_get_object (
                UI_SEC_RE.builder, "EF_section_rectangulaire_buffer_largeur")),
                              tmp.c_str (),
                              -1);
    conv_f_c (data->hauteur_retombee, &tmp, DECIMAL_DISTANCE);
    gtk_text_buffer_set_text (GTK_TEXT_BUFFER (gtk_builder_get_object (
                UI_SEC_RE.builder, "EF_section_rectangulaire_buffer_hauteur")),
                              tmp.c_str (),
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
                                GTK_WINDOW (UI_GTK.window));
  
  return true;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
