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
#include "1993_1_1_materiaux.h"
#include "EF_materiaux.h"


GTK_WINDOW_KEY_PRESS (_1993_1_1, materiaux);


GTK_WINDOW_DESTROY (_1993_1_1, materiaux, );


GTK_WINDOW_CLOSE (_1993_1_1, materiaux);


gboolean
_1993_1_1_gtk_materiaux_recupere_donnees (Projet *p,
                                          char  **nom,
                                          double *fy,
                                          double *fu,
                                          double *e,
                                          double *nu)
/**
 * \brief Récupère toutes les données de la fenêtre permettant d'ajouter ou
 *        d'éditer un matériau de type acier.
 * \param p : la variable projet,
 * \param nom : nom du matériau,
 * \param fy : limite d’élasticité en Pa,
 * \param fu : résistance à la traction en Pa,
 * \param e : Module d'élasticité de l'acier en Pa,
 * \param nu : coefficient de poisson
 * \return 
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - l'un des arguments == NULL,
 *     - interface graphique non initialisée,
 *     - en cas d'erreur d'allocation mémoire.
 */
{
  GtkTextIter    start, end;
  GtkTextBuffer *textbuffer;
  gboolean       ok = TRUE;
  GtkBuilder    *builder;
  
  BUGPARAM (p, "%p", p, FALSE)
  BUGPARAM (nom, "%p", nom, FALSE)
  BUGPARAM (fy, "%p", fy, FALSE)
  BUGPARAM (fu, "%p", fu, FALSE)
  BUGPARAM (e, "%p", e, FALSE)
  BUGPARAM (nu, "%p", nu, FALSE)
  BUGCRIT (UI_ACI.builder,
           FALSE,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Ajout Matériau Acier");)
  
  builder = UI_ACI.builder;
  
  *fy = conv_buff_d (GTK_TEXT_BUFFER (gtk_builder_get_object (builder,
                                             "_1993_1_1_materiaux_buffer_fy")),
                     0.,
                     FALSE,
                     INFINITY,
                     FALSE) * 1000000.;
  if (isnan (*fy))
    ok = FALSE;
  
  if (ok)
  {
    *fu = conv_buff_d (GTK_TEXT_BUFFER (gtk_builder_get_object (builder,
                                             "_1993_1_1_materiaux_buffer_fu")),
                       0.,
                       FALSE,
                       INFINITY,
                       FALSE) * 1000000.;
    if (isnan (*fu))
      ok = FALSE;
  }
  
  if (ok)
  {
    if (gtk_widget_get_visible (GTK_WIDGET (gtk_builder_get_object (builder,
                                     "_1993_1_1_materiaux_scrolledwindow_e"))))
    {
      *e = conv_buff_d (GTK_TEXT_BUFFER (gtk_builder_get_object (builder,
                                              "_1993_1_1_materiaux_buffer_e")),
                        0.,
                        FALSE,
                        INFINITY,
                        FALSE) * 1000000000.;
      if (isnan (*e))
        ok = FALSE;
    }
    else
      *e = MODULE_YOUNG_ACIER;
  }
  
  if (ok)
  {
    if (gtk_widget_get_visible (GTK_WIDGET (gtk_builder_get_object (builder,
                                    "_1993_1_1_materiaux_scrolledwindow_nu"))))
    {
      *nu = conv_buff_d (GTK_TEXT_BUFFER (gtk_builder_get_object (builder,
                                             "_1993_1_1_materiaux_buffer_nu")),
                         0.,
                         TRUE,
                         0.5,
                         FALSE);
      if (isnan (*nu))
        ok = FALSE;
    }
    else
      *nu = COEFFICIENT_NU_ACIER;
  }
  
  // Si tous les paramètres sont corrects
  textbuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (
                 gtk_builder_get_object (UI_ACI.builder,
                                         "_1993_1_1_materiaux_textview_nom")));
  
  gtk_text_buffer_get_iter_at_offset (textbuffer, &start, 0);
  gtk_text_buffer_get_iter_at_offset (textbuffer, &end, -1);
  *nom = gtk_text_buffer_get_text (textbuffer, &start, &end, FALSE);
  
  gtk_text_buffer_remove_all_tags (textbuffer, &start, &end);
  
  if (UI_ACI.materiau == NULL)
  {
    if ((strcmp (*nom, "") == 0) ||
        (EF_materiaux_cherche_nom (p, *nom, FALSE)))
    {
      gtk_text_buffer_apply_tag_by_name (textbuffer, "mauvais", &start, &end);
      ok = FALSE;
    }
    else
      gtk_text_buffer_apply_tag_by_name (textbuffer, "OK", &start, &end);
  }
  else if ((strcmp (*nom, "") == 0) || 
    ((strcmp (UI_ACI.materiau->nom, *nom) != 0) &&
     (EF_materiaux_cherche_nom (p, *nom, FALSE))))
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
_1993_1_1_gtk_materiaux_check (GtkWidget *object,
                               Projet    *p)
/**
 * \brief Vérifie si l'ensemble des éléments est correct pour activer le bouton
 *        add/edit.
 * \param object : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 * Echec :
 *   - p == NULL,
 *   - interface graphique non initialisée.
 */
{
  char  *nom;
  double fy, fu, e, nu;
  
  BUGPARAM (p, "%p", p, )
  BUGCRIT (UI_ACI.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Ajout Matériau Acier");)
  
  if (!_1993_1_1_gtk_materiaux_recupere_donnees (p, &nom, &fy, &fu, &e, &nu))
    gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
                       UI_ACI.builder, "_1993_1_1_materiaux_button_add_edit")),
                              FALSE);
  else
  {
    gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
                       UI_ACI.builder, "_1993_1_1_materiaux_button_add_edit")),
                              TRUE);
    free (nom);
  }
  
  return;
}


void
_1993_1_1_gtk_materiaux_ajouter_clicked (GtkButton *button,
                                         Projet    *p)
/**
 * \brief Ferme la fenêtre en ajoutant la charge.
 * \param button : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 * Echec :
 *   - p == NULL,
 *   - interface graphique non initialisée,
 *   - #_1993_1_1_materiaux_ajout,
 *   - #_1993_1_1_materiaux_modif.
 */
{
  char        *nom;
  double       fy, fu, e, nu;
  EF_Materiau *materiau;
  
  BUGPARAM (p, "%p", p, )
  BUGCRIT (UI_ACI.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Ajout Matériau Acier");)
  
  if (!_1993_1_1_gtk_materiaux_recupere_donnees (p, &nom, &fy, &fu, &e, &nu))
    return;
  
  // Création de la nouvelle charge ponctuelle au noeud
  BUG (materiau = _1993_1_1_materiaux_ajout (
         p,
         nom,
         m_f (fy / 1000000., FLOTTANT_UTILISATEUR),
         m_f (fu / 1000000., FLOTTANT_UTILISATEUR)),
      ,
      free (nom);)
  free (nom);
  BUG (_1993_1_1_materiaux_modif (p,
                                  materiau,
                                  NULL,
                                  m_f (NAN, FLOTTANT_UTILISATEUR),
                                  m_f (NAN, FLOTTANT_UTILISATEUR),
                                  m_f (e, FLOTTANT_UTILISATEUR),
                                  m_f (nu, FLOTTANT_UTILISATEUR)),
      )
  
  gtk_widget_destroy (UI_ACI.window);
  
  return;
}


void
_1993_1_1_gtk_materiaux_modifier_clicked (GtkButton *button,
                                          Projet    *p)
/**
 * \brief Ferme la fenêtre en appliquant les modifications.
 * \param button : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 * Echec :
 *   - p == NULL,
 *   - interface graphique non initialisée,
 *   - #_1993_1_1_materiaux_modif.
 */
{
  char  *nom;
  double fy, fu, e, nu;
  
  BUGPARAM (p, "%p", p, )
  BUGCRIT (UI_ACI.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Ajout Matériau Acier");)
  
  if (!_1993_1_1_gtk_materiaux_recupere_donnees (p, &nom, &fy, &fu, &e, &nu))
    return;
  
  BUG (_1993_1_1_materiaux_modif (p,
                                  UI_ACI.materiau,
                                  nom,
                                  m_f (fy, FLOTTANT_UTILISATEUR),
                                  m_f (fu, FLOTTANT_UTILISATEUR),
                                  m_f (e, FLOTTANT_UTILISATEUR),
                                  m_f (nu, FLOTTANT_UTILISATEUR)),
       ,
       free (nom);)
  
  free (nom);
  
  gtk_widget_destroy (UI_ACI.window);
  
  return;
}


void
_1993_1_1_gtk_materiaux_toggled (GtkCheckMenuItem *checkmenuitem,
                                 Projet           *p)
/**
 * \brief Evènement pour (dés)activer la personnalisation d'une propriété de
 *        l'acier.
 * \param checkmenuitem : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 * Echec :
 *   - p == NULL,
 *   - interface graphique non initialisée.
 */
{
  GtkBuilder     *builder;
  gboolean        check = gtk_check_menu_item_get_active(checkmenuitem);
  EF_Materiau    *mat;
  Materiau_Acier *acier_data;
  char            tmp[30];
  
  BUGPARAM (p, "%p", p, )
  
  builder = UI_ACI.builder;
  mat = UI_ACI.materiau;
  if (mat != NULL)
    acier_data = UI_ACI.materiau->data;
  else
    acier_data = NULL;
  
  if (checkmenuitem == GTK_CHECK_MENU_ITEM (gtk_builder_get_object (builder,
                                 "_1993_1_1_materiaux_personnaliser_menu_fu")))
  {
    if (check && mat)
    {
      conv_f_c (m_f (m_g (acier_data->fu) / 1000000., acier_data->fu.type),
                tmp,
                DECIMAL_CONTRAINTE);
      gtk_text_buffer_set_text (GTK_TEXT_BUFFER (gtk_builder_get_object (
                                    builder, "_1993_1_1_materiaux_buffer_fu")),
                                tmp,
                                -1);
    }
    
    gtk_widget_set_visible (GTK_WIDGET (gtk_builder_get_object (builder,
                                              "_1993_1_1_materiaux_label_fu")),
                            check);
    gtk_widget_set_visible (GTK_WIDGET (gtk_builder_get_object (builder,
                                     "_1993_1_1_materiaux_scrolledwindow_fu")),
                            check);
  }
  else if (checkmenuitem == GTK_CHECK_MENU_ITEM (gtk_builder_get_object (
                         builder, "_1993_1_1_materiaux_personnaliser_menu_e")))
  {
    if (check && mat)
    {
      conv_f_c (m_f (m_g (acier_data->e) / 1000000000., acier_data->e.type),
                tmp,
                DECIMAL_CONTRAINTE);
      gtk_text_buffer_set_text (GTK_TEXT_BUFFER (gtk_builder_get_object (
                                     builder, "_1993_1_1_materiaux_buffer_e")),
                                tmp,
                                -1);
    }
    gtk_widget_set_visible (GTK_WIDGET (gtk_builder_get_object (builder,
                                               "_1993_1_1_materiaux_label_e")),
                            check);
    gtk_widget_set_visible (GTK_WIDGET (gtk_builder_get_object (builder,
                                      "_1993_1_1_materiaux_scrolledwindow_e")),
                            check);
  }
  else if (checkmenuitem == GTK_CHECK_MENU_ITEM (gtk_builder_get_object (
                        builder, "_1993_1_1_materiaux_personnaliser_menu_nu")))
  {
    if (check && mat)
    {
      conv_f_c (acier_data->nu, tmp, DECIMAL_SANS_UNITE);
      gtk_text_buffer_set_text (GTK_TEXT_BUFFER (gtk_builder_get_object (
                                    builder, "_1993_1_1_materiaux_buffer_nu")),
                                tmp,
                                -1);
    }
    gtk_widget_set_visible (GTK_WIDGET (gtk_builder_get_object (builder,
                                              "_1993_1_1_materiaux_label_nu")),
                            check);
    gtk_widget_set_visible (GTK_WIDGET (gtk_builder_get_object (builder,
                                     "_1993_1_1_materiaux_scrolledwindow_nu")),
                            check);
  }
  else
    BUGMSG (NULL, , gettext ("Paramètre %s incorrect.\n"), "checkmenuitem")
  
  _1993_1_1_gtk_materiaux_check (NULL, p);
  
  return;
}


gboolean
_1993_1_1_gtk_materiaux (Projet      *p,
                         EF_Materiau *materiau)
/**
 * \brief Affichage de la fenêtre permettant de créer ou modifier un matériau
 *        de type béton.
 * \param p : la variable projet,
 * \param materiau : materiau à modifier. NULL si nouveau matériau,
 * \return
 *   Succès : TRUE.\n
 *   Echec : FALSE :
 *     - p == NULL.
 */
{
  Materiau_Acier *acier_data;
  
  BUGPARAM (p, "%p", p, FALSE)
  
  if (UI_ACI.builder != NULL)
  {
    gtk_window_present (GTK_WINDOW (UI_ACI.window));
    if (UI_ACI.materiau == materiau)
      return TRUE;
  }
  else
  {
    UI_ACI.builder = gtk_builder_new ();
    BUGCRIT (gtk_builder_add_from_resource (UI_ACI.builder,
                                  "/org/2lgc/codegui/ui/1993_1_1_materiaux.ui",
                                           NULL) != 0,
             FALSE,
             (gettext ("La génération de la fenêtre %s a échouée.\n"),
                       "Ajout Matériau Acier");)
    gtk_builder_connect_signals (UI_ACI.builder, p);
    UI_ACI.window = GTK_WIDGET (gtk_builder_get_object (UI_ACI.builder,
                                                "_1993_1_1_materiaux_window"));
  }
  
  if (materiau == NULL)
  {
    gtk_window_set_title (GTK_WINDOW (UI_ACI.window),
                          gettext ("Ajout d'un matériau acier"));
    UI_ACI.materiau = NULL;
    acier_data = NULL;
    gtk_text_buffer_set_text (gtk_text_view_get_buffer (GTK_TEXT_VIEW (
                 gtk_builder_get_object (UI_ACI.builder,
                                         "_1993_1_1_materiaux_textview_nom"))),
                              "",
                              -1);
    gtk_text_buffer_set_text (gtk_text_view_get_buffer (GTK_TEXT_VIEW (
                  gtk_builder_get_object (UI_ACI.builder,
                                          "_1993_1_1_materiaux_textview_fy"))),
                              "",
                              -1);
    gtk_text_buffer_set_text (gtk_text_view_get_buffer (GTK_TEXT_VIEW (
                  gtk_builder_get_object (UI_ACI.builder,
                                          "_1993_1_1_materiaux_textview_fu"))),
                              "",
                              -1);
    
    gtk_button_set_label (GTK_BUTTON (gtk_builder_get_object (UI_ACI.builder,
                                       "_1993_1_1_materiaux_button_add_edit")),
                          gettext("_Ajouter"));
    g_signal_connect (gtk_builder_get_object (UI_ACI.builder,
                                        "_1993_1_1_materiaux_button_add_edit"),
                      "clicked",
                      G_CALLBACK (_1993_1_1_gtk_materiaux_ajouter_clicked),
                      p);
    _1993_1_1_gtk_materiaux_check (NULL, p);
  }
  else
  {
    gchar tmp[30];
    
    BUGCRIT (materiau->type == MATERIAU_ACIER,
             FALSE,
             (gettext ("Le matériau n'est pas en acier.\n"));)
    gtk_window_set_title (GTK_WINDOW (UI_ACI.window),
                          gettext ("Modification d'un matériau acier"));
    UI_ACI.materiau = materiau;
    acier_data = materiau->data;
    
    gtk_text_buffer_set_text (gtk_text_view_get_buffer (GTK_TEXT_VIEW (
                 gtk_builder_get_object (UI_ACI.builder,
                                         "_1993_1_1_materiaux_textview_nom"))),
                              materiau->nom,
                              -1);
    conv_f_c (m_f (m_g (acier_data->fy) / 1000000., acier_data->fy.type),
              tmp,
              DECIMAL_CONTRAINTE);
    gtk_text_buffer_set_text (GTK_TEXT_BUFFER (gtk_builder_get_object (
                            UI_ACI.builder, "_1993_1_1_materiaux_buffer_fy")),
                              tmp,
                              -1);
    conv_f_c (m_f (m_g (acier_data->fu) / 1000000., acier_data->fu.type),
              tmp,
              DECIMAL_CONTRAINTE);
    gtk_text_buffer_set_text (GTK_TEXT_BUFFER (gtk_builder_get_object (
                            UI_ACI.builder, "_1993_1_1_materiaux_buffer_fu")),
                              tmp,
                              -1);
    
    gtk_button_set_label (GTK_BUTTON (gtk_builder_get_object (UI_ACI.builder,
                                       "_1993_1_1_materiaux_button_add_edit")),
                          gettext ("_Modifier"));
    g_signal_connect (gtk_builder_get_object (UI_ACI.builder,
                                        "_1993_1_1_materiaux_button_add_edit"),
                      "clicked",
                      G_CALLBACK (_1993_1_1_gtk_materiaux_modifier_clicked),
                      p);
  }
  
  // On affiche les propriétés qui ne sont pas égale à celle par défaut.
  gtk_check_menu_item_set_active (
    GTK_CHECK_MENU_ITEM (gtk_builder_get_object (UI_ACI.builder,
                                  "_1993_1_1_materiaux_personnaliser_menu_e")),
    acier_data && !ERR (m_g (acier_data->e), MODULE_YOUNG_ACIER));
  gtk_check_menu_item_set_active (
    GTK_CHECK_MENU_ITEM (gtk_builder_get_object (UI_ACI.builder,
                                 "_1993_1_1_materiaux_personnaliser_menu_nu")),
    acier_data && !ERR (m_g (acier_data->nu), COEFFICIENT_NU_ACIER));
  
  gtk_window_set_transient_for (GTK_WINDOW (UI_ACI.window),
                                GTK_WINDOW (p->ui.comp.window));
  
  return TRUE;
}


void
_1993_1_1_gtk_materiaux_ajout (GtkMenuItem *menuitem,
                               Projet      *p)
/**
 * \brief Lance la fenêtre permettant d'ajouter un matériau acier.
 * \param menuitem : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 * Echec :
 *   - p == NULL,
 *   - interface graphique non initialisée.
 */
{
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (p->ui.ef_materiaux.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Ajout Matériau Acier");)
  
  BUG (_1993_1_1_gtk_materiaux (p, NULL), )
}


#endif
