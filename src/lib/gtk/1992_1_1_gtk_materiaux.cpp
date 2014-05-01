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
#include "1992_1_1_materiaux.hpp"
#include "1993_1_1_gtk_materiaux.hpp"
#include "EF_materiaux.hpp"
#include "1992_1_1_gtk_materiaux.hpp"


GTK_WINDOW_KEY_PRESS (_1992_1_1, materiaux);


GTK_WINDOW_DESTROY (_1992_1_1, materiaux, );


GTK_WINDOW_CLOSE (_1992_1_1, materiaux);


/**
 * \brief Récupère toutes les données de la fenêtre permettant d'ajouter ou
 *        d'éditer un matériau de type béton.
 * \param p : la variable projet,
 * \param nom : nom du matériau,
 * \param fck : Résistance caractéristique en compression du béton, mesurée sur
 *              cylindre à 28 jours en MPa,
 * \param fckcube : Résistance caractéristique en compression du béton, mesurée
 *                  sur cube à 28 jours en Pa,
 * \param fcm : Valeur moyenne de la résistance en compression du béton,
 *              mesurée sur cylindre en Pa,
 * \param fctm : Valeur moyenne de la résistance en traction directe du béton
 *               en Pa,
 * \param fctk_0_05 : fractile 5% en Pa,
 * \param fctk_0_95 : fractile 95% en Pa,
 * \param ecm : Module d'élasticité sécant du béton en Pa,
 * \param ec1 : Déformation relative en compression du béton au pic de
 *              contrainte f_c en valeur unitaire,
 * \param ecu1 : Déformation relative ultime du béton en compression en valeur
 *               unitaire
 * \param ec2 : Déformation relative en compression du béton au point 2
 *              en valeur unitaire,
 * \param ecu2 : Déformation relative ultime du béton en compression au point 2
 *               en valeur unitaire,
 * \param n : coefficient n utilisé dans la courbe parabole rectangle,
 * \param ec3 : Déformation relative en compression du béton au point 3
 *              en valeur unitaire,
 * \param ecu3 : Déformation relative ultime du béton en compression au point 3
 *               en valeur unitaire,
 * \param nu : coefficient de poisson.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - l'un des arguments == NULL,
 *     - interface graphique non initialisée,
 *     - en cas d'erreur d'allocation mémoire.
 */
gboolean
_1992_1_1_gtk_materiaux_recupere_donnees (Projet *p,
                                          char  **nom,
                                          double *fck,
                                          double *fckcube,
                                          double *fcm,
                                          double *fctm,
                                          double *fctk_0_05,
                                          double *fctk_0_95,
                                          double *ecm,
                                          double *ec1,
                                          double *ecu1,
                                          double *ec2,
                                          double *ecu2,
                                          double *n,
                                          double *ec3,
                                          double *ecu3,
                                          double *nu)
{
  GtkTextIter    start, end;
  GtkTextBuffer *textbuffer;
  gboolean       ok = TRUE;
  GtkBuilder    *builder;
  
  BUGPARAM (p, "%p", p, FALSE)
  BUGPARAM (nom, "%p", nom, FALSE)
  BUGPARAM (fck, "%p", fck, FALSE)
  BUGPARAM (fckcube, "%p", fckcube, FALSE)
  BUGPARAM (fcm, "%p", fcm, FALSE)
  BUGPARAM (fctm, "%p", fctm, FALSE)
  BUGPARAM (fctk_0_05, "%p", fctk_0_05, FALSE)
  BUGPARAM (fctk_0_95, "%p", fctk_0_95, FALSE)
  BUGPARAM (ecm, "%p", ecm, FALSE)
  BUGPARAM (ec1, "%p", ec1, FALSE)
  BUGPARAM (ecu1, "%p", ecu1, FALSE)
  BUGPARAM (ec2, "%p", ec2, FALSE)
  BUGPARAM (ecu2, "%p", ecu2, FALSE)
  BUGPARAM (n, "%p", n, FALSE)
  BUGPARAM (ec3, "%p", ec3, FALSE)
  BUGPARAM (ecu3, "%p", ecu3, FALSE)
  BUGPARAM (nu, "%p", nu, FALSE)
  BUGCRIT (UI_BET.builder,
           FALSE,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Ajout Matériau Béton"); )
  
  builder = UI_BET.builder;
  
  *fck = conv_buff_d (GTK_TEXT_BUFFER (gtk_builder_get_object (builder,
                                            "_1992_1_1_materiaux_buffer_fck")),
                                        0., FALSE,
                                        90., TRUE);
  if (isnan (*fck))
  {
    ok = FALSE;
  }
  
  if (ok)
  {
    if (gtk_widget_get_visible (GTK_WIDGET (gtk_builder_get_object (builder,
                              "_1992_1_1_materiaux_scrolledwindow_fck_cube"))))
    {
      *fckcube = conv_buff_d (GTK_TEXT_BUFFER (gtk_builder_get_object (builder,
                                       "_1992_1_1_materiaux_buffer_fck_cube")),
                                                0., FALSE,
                                                INFINITY, FALSE)
                 * 1000000.;
      if (isnan (*fckcube))
      {
        ok = FALSE;
      }
    }
    else
    {
      *fckcube = _1992_1_1_materiaux_fckcube (*fck);
    }
  }
  
  if (ok)
  {
    if (gtk_widget_get_visible (GTK_WIDGET (gtk_builder_get_object (builder,
                                   "_1992_1_1_materiaux_scrolledwindow_fcm"))))
    {
      *fcm = conv_buff_d (GTK_TEXT_BUFFER (gtk_builder_get_object (builder,
                                            "_1992_1_1_materiaux_buffer_fcm")),
                                            0., FALSE,
                                            INFINITY, FALSE)
             * 1000000.;
      if (isnan (*fcm))
      {
        ok = FALSE;
      }
    }
    else
    {
      *fcm = _1992_1_1_materiaux_fcm (*fck);
    }
  }
  
  if (ok)
  {
    if (gtk_widget_get_visible (GTK_WIDGET (gtk_builder_get_object (builder,
                                  "_1992_1_1_materiaux_scrolledwindow_fctm"))))
    {
      *fctm = conv_buff_d (GTK_TEXT_BUFFER (gtk_builder_get_object (builder,
                                           "_1992_1_1_materiaux_buffer_fctm")),
                                             0., FALSE,
                                             INFINITY, FALSE)
              * 1000000.;
      if (isnan (*fctm))
      {
        ok = FALSE;
      }
    }
    else
    {
      *fctm = _1992_1_1_materiaux_fctm (*fck, *fcm / 1000000.);
    }
  }
  
  if (ok)
  {
    if (gtk_widget_get_visible (GTK_WIDGET (gtk_builder_get_object (builder,
                             "_1992_1_1_materiaux_scrolledwindow_fctk_0_05"))))
    {
      *fctk_0_05 = conv_buff_d (GTK_TEXT_BUFFER (
              gtk_builder_get_object (builder,
                                      "_1992_1_1_materiaux_buffer_fctk_0_05")),
                                                  0., FALSE,
                                                  INFINITY, FALSE)
                   * 1000000.;
      if (isnan (*fctk_0_05))
      {
        ok = FALSE;
      }
    }
    else
    {
      *fctk_0_05 = _1992_1_1_materiaux_fctk_0_05 (*fctm / 1000000.);
    }
  }
  
  if (ok)
  {
    if (gtk_widget_get_visible (GTK_WIDGET (gtk_builder_get_object (builder,
                             "_1992_1_1_materiaux_scrolledwindow_fctk_0_95"))))
    {
      *fctk_0_95 = conv_buff_d (GTK_TEXT_BUFFER (
              gtk_builder_get_object (builder,
                                      "_1992_1_1_materiaux_buffer_fctk_0_95")),
                                                  0., FALSE,
                                                  INFINITY, FALSE)
                   * 1000000.;
      if (isnan (*fctk_0_95))
      {
        ok = FALSE;
      }
    }
    else
    {
      *fctk_0_95 = _1992_1_1_materiaux_fctk_0_95 (*fctm / 1000000.);
    }
  }
  
  if (ok)
  {
    if (gtk_widget_get_visible (GTK_WIDGET (gtk_builder_get_object (builder,
                                   "_1992_1_1_materiaux_scrolledwindow_Ecm"))))
    {
      *ecm = conv_buff_d (GTK_TEXT_BUFFER (gtk_builder_get_object (builder,
                                            "_1992_1_1_materiaux_buffer_Ecm")),
                                            0., FALSE,
                                            INFINITY, FALSE)
             * 1000000000.;
      if (isnan (*ecm))
      {
        ok = FALSE;
      }
    }
    else
    {
      *ecm = _1992_1_1_materiaux_ecm (*fcm / 1000000.);
    }
  }
  
  if (ok)
  {
    if (gtk_widget_get_visible (GTK_WIDGET (gtk_builder_get_object (builder,
                                   "_1992_1_1_materiaux_scrolledwindow_ec1"))))
    {
      *ec1 = conv_buff_d (GTK_TEXT_BUFFER (gtk_builder_get_object (builder,
                                            "_1992_1_1_materiaux_buffer_ec1")),
                                            0., FALSE,
                                            INFINITY, FALSE)
             / 1000.;
      if (isnan (*ec1))
      {
        ok = FALSE;
      }
    }
    else
    {
      *ec1 = _1992_1_1_materiaux_ec1 (*fcm / 1000000.);
    }
  }
  
  if (ok)
  {
    if (gtk_widget_get_visible (GTK_WIDGET (gtk_builder_get_object (builder,
                                  "_1992_1_1_materiaux_scrolledwindow_ecu1"))))
    {
      *ecu1 = conv_buff_d (GTK_TEXT_BUFFER (gtk_builder_get_object (builder,
                                           "_1992_1_1_materiaux_buffer_ecu1")),
                                             0., FALSE,
                                             INFINITY, FALSE)
              / 1000.;
      if (isnan (*ecu1))
      {
        ok = FALSE;
      }
    }
    else
    {
      *ecu1 = _1992_1_1_materiaux_ecu1 (*fcm / 1000000., *fck / 1000000.);
    }
  }
  
  if (ok)
  {
    if (gtk_widget_get_visible (GTK_WIDGET (gtk_builder_get_object (builder,
                                   "_1992_1_1_materiaux_scrolledwindow_ec2"))))
    {
      *ec2 = conv_buff_d (GTK_TEXT_BUFFER (gtk_builder_get_object (builder,
                                            "_1992_1_1_materiaux_buffer_ec2")),
                                            0., FALSE,
                                            INFINITY, FALSE)
             / 1000.;
      if (isnan (*ec2))
      {
        ok = FALSE;
      }
    }
    else
    {
      *ec2 = _1992_1_1_materiaux_ec2 (*fck / 1000000.);
    }
  }
  
  if (ok)
  {
    if (gtk_widget_get_visible (GTK_WIDGET (gtk_builder_get_object (builder,
                                  "_1992_1_1_materiaux_scrolledwindow_ecu2"))))
    {
      *ecu2 = conv_buff_d (GTK_TEXT_BUFFER (gtk_builder_get_object (builder,
                                           "_1992_1_1_materiaux_buffer_ecu2")),
                                             0., FALSE,
                                             INFINITY, FALSE)
              / 1000.;
      if (isnan (*ecu2))
      {
        ok = FALSE;
      }
    }
    else
    {
      *ecu2 = _1992_1_1_materiaux_ecu2 (*fck / 1000000.);
    }
  }
  
  if (ok)
  {
    if (gtk_widget_get_visible (GTK_WIDGET (gtk_builder_get_object (builder,
                                     "_1992_1_1_materiaux_scrolledwindow_n"))))
    {
      *n = conv_buff_d (GTK_TEXT_BUFFER (gtk_builder_get_object (builder,
                                              "_1992_1_1_materiaux_buffer_n")),
                                          0., FALSE,
                                          INFINITY, FALSE);
      if (isnan (*n))
      {
        ok = FALSE;
      }
    }
    else
    {
      *n = _1992_1_1_materiaux_n (*fck / 1000000.);
    }
  }
  
  if (ok)
  {
    if (gtk_widget_get_visible (GTK_WIDGET (gtk_builder_get_object (builder,
                                   "_1992_1_1_materiaux_scrolledwindow_ec3"))))
    {
      *ec3 = conv_buff_d (GTK_TEXT_BUFFER (gtk_builder_get_object (builder,
                                            "_1992_1_1_materiaux_buffer_ec3")),
                                            0., FALSE,
                                            INFINITY, FALSE)
             / 1000.;
      if (isnan (*ec3))
      {
        ok = FALSE;
      }
    }
    else
    {
      *ec3 = _1992_1_1_materiaux_ec3 (*fck / 1000000.);
    }
  }
  
  if (ok)
  {
    if (gtk_widget_get_visible (GTK_WIDGET (gtk_builder_get_object (builder,
                                  "_1992_1_1_materiaux_scrolledwindow_ecu3"))))
    {
      *ecu3 = conv_buff_d (GTK_TEXT_BUFFER (gtk_builder_get_object (builder,
                                           "_1992_1_1_materiaux_buffer_ecu3")),
                                             0., FALSE,
                                             INFINITY, FALSE)
              / 1000.;
      if (isnan (*ecu3))
      {
        ok = FALSE;
      }
    }
    else
    {
      *ecu3 = _1992_1_1_materiaux_ecu3 (*fck / 1000000.);
    }
  }
  
  if (ok)
  {
    if (gtk_widget_get_visible (GTK_WIDGET (gtk_builder_get_object (builder,
                                    "_1992_1_1_materiaux_scrolledwindow_nu"))))
    {
      *nu = conv_buff_d (GTK_TEXT_BUFFER (gtk_builder_get_object (builder,
                                             "_1992_1_1_materiaux_buffer_nu")),
                                           0., TRUE,
                                           0.5, FALSE);
      if (isnan (*nu))
      {
        ok = FALSE;
      }
    }
    else
    {
      *nu = COEFFICIENT_NU_BETON;
    }
  }
  
  // Si tous les paramètres sont corrects
  textbuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (
                 gtk_builder_get_object (UI_BET.builder,
                                         "_1992_1_1_materiaux_textview_nom")));
  
  gtk_text_buffer_get_iter_at_offset (textbuffer, &start, 0);
  gtk_text_buffer_get_iter_at_offset (textbuffer, &end, -1);
  *nom = gtk_text_buffer_get_text (textbuffer, &start, &end, FALSE);
  
  gtk_text_buffer_remove_all_tags (textbuffer, &start, &end);
  
  if (UI_BET.materiau == NULL)
  {
    if ((strcmp (*nom, "") == 0) ||
        (EF_materiaux_cherche_nom (p, *nom, FALSE)))
    {
      gtk_text_buffer_apply_tag_by_name (textbuffer, "mauvais", &start, &end);
      ok = FALSE;
    }
    else
    {
      gtk_text_buffer_apply_tag_by_name (textbuffer, "OK", &start, &end);
    }
  }
  else if ((strcmp(*nom, "") == 0) || 
    ((strcmp (UI_BET.materiau->nom, *nom) != 0) &&
     (EF_materiaux_cherche_nom (p, *nom, FALSE))))
  {
    gtk_text_buffer_apply_tag_by_name (textbuffer, "mauvais", &start, &end);
    ok = FALSE;
  }
  else
  {
    gtk_text_buffer_apply_tag_by_name (textbuffer, "OK", &start, &end);
  }
  
  if (ok == FALSE)
  {
    free (*nom);
  }
  
  return ok;
}


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
extern "C"
void
_1992_1_1_gtk_materiaux_check (GtkWidget *object,
                               Projet    *p)
{
  char  *nom;
  double fck, fckcube, fcm, fctm, fctk_0_05, fctk_0_95;
  double ecm;
  double ec1, ecu1, ec2, ecu2, n, ec3, ecu3, nu;
  
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_BET.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Ajout Matériau Béton"); )
  
  if (!_1992_1_1_gtk_materiaux_recupere_donnees (p,
                                                 &nom,
                                                 &fck,
                                                 &fckcube,
                                                 &fcm,
                                                 &fctm,
                                                 &fctk_0_05,
                                                 &fctk_0_95,
                                                 &ecm,
                                                 &ec1,
                                                 &ecu1,
                                                 &ec2,
                                                 &ecu2,
                                                 &n,
                                                 &ec3,
                                                 &ecu3,
                                                 &nu))
  {
    gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
                       UI_BET.builder, "_1992_1_1_materiaux_button_add_edit")),
                              FALSE);
  }
  else
  {
    gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
                       UI_BET.builder, "_1992_1_1_materiaux_button_add_edit")),
                              TRUE);
    free (nom);
  }
  
  return;
}


/**
 * \brief Ferme la fenêtre en ajoutant la charge.
 * \param button : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 * Echec :
 *   - p == NULL,
 *   - interface graphique non initialisée,
 *   - #_1992_1_1_materiaux_ajout,
 *   - #_1992_1_1_materiaux_modif.
 */
void
_1992_1_1_gtk_materiaux_ajouter_clicked (GtkButton *button,
                                         Projet    *p)
{
  char        *nom;
  double       fck, fckcube, fcm, fctm, fctk_0_05, fctk_0_95;
  double       ecm;
  double       ec1, ecu1, ec2, ecu2, n, ec3, ecu3, nu;
  EF_Materiau *materiau;
  
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_BET.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Ajout Matériau Béton"); )
  
  if (!_1992_1_1_gtk_materiaux_recupere_donnees (p,
                                                 &nom,
                                                 &fck,
                                                 &fckcube,
                                                 &fcm,
                                                 &fctm,
                                                 &fctk_0_05,
                                                 &fctk_0_95,
                                                 &ecm,
                                                 &ec1,
                                                 &ecu1,
                                                 &ec2,
                                                 &ecu2,
                                                 &n,
                                                 &ec3,
                                                 &ecu3,
                                                 &nu))
  {
    return;
  }
  
  // Création de la nouvelle charge ponctuelle au noeud
  BUG (materiau = _1992_1_1_materiaux_ajout (p,
                                             nom,
                                             m_f (fck, FLOTTANT_UTILISATEUR)),
      ,
      free (nom); )
  free (nom);
  
  BUG (_1992_1_1_materiaux_modif (
         p,
         materiau,
         NULL,
         m_f (NAN,       FLOTTANT_UTILISATEUR),
         m_f (fckcube,   FLOTTANT_UTILISATEUR),
         m_f (fcm,       FLOTTANT_UTILISATEUR),
         m_f (fctm,      FLOTTANT_UTILISATEUR),
         m_f (fctk_0_05, FLOTTANT_UTILISATEUR),
         m_f (fctk_0_95, FLOTTANT_UTILISATEUR),
         m_f (ecm,       FLOTTANT_UTILISATEUR),
         m_f (ec1,       FLOTTANT_UTILISATEUR),
         m_f (ecu1,      FLOTTANT_UTILISATEUR),
         m_f (ec2,       FLOTTANT_UTILISATEUR),
         m_f (ecu2,      FLOTTANT_UTILISATEUR),
         m_f (n,         FLOTTANT_UTILISATEUR),
         m_f (ec3,       FLOTTANT_UTILISATEUR),
         m_f (ecu3,      FLOTTANT_UTILISATEUR),
         m_f (nu,        FLOTTANT_UTILISATEUR)),
      )
  
  gtk_widget_destroy (UI_BET.window);
  
  return;
}


/**
 * \brief Ferme la fenêtre en appliquant les modifications.
 * \param button : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 * Echec :
 *   - p == NULL,
 *   - interface graphique non initialisée,
 *   - #_1992_1_1_materiaux_modif.
 */
void
_1992_1_1_gtk_materiaux_modifier_clicked (GtkButton *button,
                                          Projet    *p)
{
  char  *nom;
  double fck, fckcube, fcm, fctm, fctk_0_05, fctk_0_95;
  double ecm;
  double ec1, ecu1, ec2, ecu2, n, ec3, ecu3, nu;
  
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_BET.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Ajout Matériau Béton"); )
  
  if (!_1992_1_1_gtk_materiaux_recupere_donnees (p,
                                                 &nom,
                                                 &fck,
                                                 &fckcube,
                                                 &fcm,
                                                 &fctm,
                                                 &fctk_0_05,
                                                 &fctk_0_95,
                                                 &ecm,
                                                 &ec1,
                                                 &ecu1,
                                                 &ec2,
                                                 &ecu2,
                                                 &n,
                                                 &ec3,
                                                 &ecu3,
                                                 &nu))
  {
    return;
  }
  
  BUG (_1992_1_1_materiaux_modif (
         p,
         UI_BET.materiau,
         nom,
         m_f (fck * 1000000., FLOTTANT_UTILISATEUR),
         m_f (fckcube,        FLOTTANT_UTILISATEUR),
         m_f (fcm,            FLOTTANT_UTILISATEUR),
         m_f (fctm,           FLOTTANT_UTILISATEUR),
         m_f (fctk_0_05,      FLOTTANT_UTILISATEUR),
         m_f (fctk_0_95,      FLOTTANT_UTILISATEUR),
         m_f (ecm,            FLOTTANT_UTILISATEUR),
         m_f (ec1,            FLOTTANT_UTILISATEUR),
         m_f (ecu1,           FLOTTANT_UTILISATEUR),
         m_f (ec2,            FLOTTANT_UTILISATEUR),
         m_f (ecu2,           FLOTTANT_UTILISATEUR),
         m_f (n,              FLOTTANT_UTILISATEUR),
         m_f (ec3,            FLOTTANT_UTILISATEUR),
         m_f (ecu3,           FLOTTANT_UTILISATEUR),
         m_f (nu,             FLOTTANT_UTILISATEUR)),
      free (nom); )
  
  free (nom);
  
  gtk_widget_destroy (UI_BET.window);
  
  return;
}


/**
 * \brief Evènement pour (dés)activer la personnalisation d'une propriété du
 *        béton.
 * \param checkmenuitem : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 * Echec :
 *   - p == NULL,
 *   - interface graphique non initialisée.
 */
extern "C"
void
_1992_1_1_gtk_materiaux_toggled (GtkCheckMenuItem *checkmenuitem,
                                 Projet           *p)
{
  GtkBuilder     *builder;
  gboolean        check = gtk_check_menu_item_get_active (checkmenuitem);
  EF_Materiau    *mat;
  Materiau_Beton *beton_data;
  char            tmp[30];
  
  BUGPARAMCRIT (p, "%p", p, )
  
  builder = UI_BET.builder;
  mat = UI_BET.materiau;
  if (mat != NULL)
  {
    beton_data = UI_BET.materiau->data;
  }
  else
  {
    beton_data = NULL;
  }
  
  if (checkmenuitem == GTK_CHECK_MENU_ITEM (gtk_builder_get_object (builder,
                           "_1992_1_1_materiaux_personnaliser_menu_fck_cube")))
  {
    if (check && mat)
    {
      conv_f_c (m_f (m_g (beton_data->fckcube) / 1000000.,
                     beton_data->fckcube.type),
                tmp,
                DECIMAL_CONTRAINTE);
      gtk_text_buffer_set_text (GTK_TEXT_BUFFER (gtk_builder_get_object (
                              builder, "_1992_1_1_materiaux_buffer_fck_cube")),
                                tmp,
                                -1);
    }
    
    gtk_widget_set_visible (GTK_WIDGET (gtk_builder_get_object (builder,
                                        "_1992_1_1_materiaux_label_fck_cube")),
                            check);
    gtk_widget_set_visible (GTK_WIDGET (gtk_builder_get_object (builder,
                               "_1992_1_1_materiaux_scrolledwindow_fck_cube")),
                            check);
  }
  else if (checkmenuitem == GTK_CHECK_MENU_ITEM (gtk_builder_get_object (
                       builder, "_1992_1_1_materiaux_personnaliser_menu_fcm")))
  {
    if (check && mat)
    {
      conv_f_c (m_f (m_g (beton_data->fcm) / 1000000., beton_data->fcm.type),
                tmp,
                DECIMAL_CONTRAINTE);
      gtk_text_buffer_set_text (GTK_TEXT_BUFFER (gtk_builder_get_object (
                                   builder, "_1992_1_1_materiaux_buffer_fcm")),
                                tmp,
                                -1);
    }
    gtk_widget_set_visible (GTK_WIDGET (gtk_builder_get_object (builder,
                                             "_1992_1_1_materiaux_label_fcm")),
                            check);
    gtk_widget_set_visible (GTK_WIDGET (gtk_builder_get_object (builder,
                                    "_1992_1_1_materiaux_scrolledwindow_fcm")),
                            check);
  }
  else if (checkmenuitem == GTK_CHECK_MENU_ITEM (gtk_builder_get_object (
                      builder, "_1992_1_1_materiaux_personnaliser_menu_fctm")))
  {
    if (check && mat)
    {
      conv_f_c (m_f (m_g (beton_data->fctm) / 1000000., beton_data->fctm.type),
                tmp,
                DECIMAL_CONTRAINTE);
      gtk_text_buffer_set_text (GTK_TEXT_BUFFER (gtk_builder_get_object (
                                  builder, "_1992_1_1_materiaux_buffer_fctm")),
                                tmp,
                                -1);
    }
    gtk_widget_set_visible (GTK_WIDGET (gtk_builder_get_object (builder,
                                            "_1992_1_1_materiaux_label_fctm")),
                            check);
    gtk_widget_set_visible (GTK_WIDGET (gtk_builder_get_object (builder,
                                   "_1992_1_1_materiaux_scrolledwindow_fctm")),
                            check);
  }
  else if (checkmenuitem == GTK_CHECK_MENU_ITEM (gtk_builder_get_object (
                 builder, "_1992_1_1_materiaux_personnaliser_menu_fctk_0_05")))
  {
    if (check && mat)
    {
      conv_f_c (m_f (m_g (beton_data->fctk_0_05) / 1000000.,
                     beton_data->fctk_0_05.type),
                tmp,
                DECIMAL_CONTRAINTE);
      gtk_text_buffer_set_text (GTK_TEXT_BUFFER (gtk_builder_get_object (
                             builder, "_1992_1_1_materiaux_buffer_fctk_0_05")),
                                tmp,
                                -1);
    }
    gtk_widget_set_visible (GTK_WIDGET (gtk_builder_get_object (builder,
                                       "_1992_1_1_materiaux_label_fctk_0_05")),
                            check);
    gtk_widget_set_visible (GTK_WIDGET (gtk_builder_get_object (builder,
                              "_1992_1_1_materiaux_scrolledwindow_fctk_0_05")),
                            check);
  }
  else if (checkmenuitem == GTK_CHECK_MENU_ITEM (gtk_builder_get_object (
                 builder, "_1992_1_1_materiaux_personnaliser_menu_fctk_0_95")))
  {
    if (check && mat)
    {
      conv_f_c (m_f (m_g (beton_data->fctk_0_95) / 1000000.,
                     beton_data->fctk_0_95.type),
                tmp,
                DECIMAL_CONTRAINTE);
      gtk_text_buffer_set_text (GTK_TEXT_BUFFER (gtk_builder_get_object (
                             builder, "_1992_1_1_materiaux_buffer_fctk_0_95")),
                                tmp,
                                -1);
    }
    gtk_widget_set_visible (GTK_WIDGET (gtk_builder_get_object (builder,
                                       "_1992_1_1_materiaux_label_fctk_0_95")),
                            check);
    gtk_widget_set_visible (GTK_WIDGET (gtk_builder_get_object (builder,
                              "_1992_1_1_materiaux_scrolledwindow_fctk_0_95")),
                            check);
  }
  else if (checkmenuitem == GTK_CHECK_MENU_ITEM (gtk_builder_get_object (
                       builder, "_1992_1_1_materiaux_personnaliser_menu_Ecm")))
  {
    if (check && mat)
    {
      conv_f_c (m_f (m_g (beton_data->ecm) / 1000000000.,
                     beton_data->ecm.type),
                tmp,
                DECIMAL_CONTRAINTE);
      gtk_text_buffer_set_text (GTK_TEXT_BUFFER (gtk_builder_get_object (
                                   builder, "_1992_1_1_materiaux_buffer_Ecm")),
                                tmp,
                                -1);
    }
    gtk_widget_set_visible (GTK_WIDGET (gtk_builder_get_object (builder,
                                             "_1992_1_1_materiaux_label_Ecm")),
                            check);
    gtk_widget_set_visible (GTK_WIDGET (gtk_builder_get_object (builder,
                                    "_1992_1_1_materiaux_scrolledwindow_Ecm")),
                            check);
  }
  else if (checkmenuitem == GTK_CHECK_MENU_ITEM (gtk_builder_get_object (
                       builder, "_1992_1_1_materiaux_personnaliser_menu_ec1")))
  {
    if (check && mat)
    {
      conv_f_c (m_f (m_g (beton_data->ec1) * 1000, beton_data->ec1.type),
                tmp,
                DECIMAL_SANS_UNITE);
      gtk_text_buffer_set_text (GTK_TEXT_BUFFER (gtk_builder_get_object (
                                   builder, "_1992_1_1_materiaux_buffer_ec1")),
                                tmp,
                                -1);
    }
    gtk_widget_set_visible (GTK_WIDGET (gtk_builder_get_object (builder,
                                             "_1992_1_1_materiaux_label_ec1")),
                            check);
    gtk_widget_set_visible (GTK_WIDGET (gtk_builder_get_object (builder,
                                    "_1992_1_1_materiaux_scrolledwindow_ec1")),
                            check);
  }
  else if (checkmenuitem == GTK_CHECK_MENU_ITEM (gtk_builder_get_object (
                      builder, "_1992_1_1_materiaux_personnaliser_menu_ecu1")))
  {
    if (check && mat)
    {
      conv_f_c (m_f (m_g (beton_data->ecu1) * 1000, beton_data->ecu1.type),
                tmp,
                DECIMAL_SANS_UNITE);
      gtk_text_buffer_set_text (GTK_TEXT_BUFFER (gtk_builder_get_object (
                                  builder, "_1992_1_1_materiaux_buffer_ecu1")),
                                tmp,
                                -1);
    }
    gtk_widget_set_visible (GTK_WIDGET (gtk_builder_get_object (builder,
                                            "_1992_1_1_materiaux_label_ecu1")),
                            check);
    gtk_widget_set_visible (GTK_WIDGET (gtk_builder_get_object (builder,
                                   "_1992_1_1_materiaux_scrolledwindow_ecu1")),
                            check);
  }
  else if (checkmenuitem == GTK_CHECK_MENU_ITEM (gtk_builder_get_object (
                       builder, "_1992_1_1_materiaux_personnaliser_menu_ec2")))
  {
    if (check && mat)
    {
      conv_f_c (m_f (m_g (beton_data->ec2) * 1000, beton_data->ec2.type),
                tmp,
                DECIMAL_SANS_UNITE);
      gtk_text_buffer_set_text (GTK_TEXT_BUFFER (gtk_builder_get_object (
                                   builder, "_1992_1_1_materiaux_buffer_ec2")),
                                tmp,
                                -1);
    }
    gtk_widget_set_visible (GTK_WIDGET (gtk_builder_get_object (builder,
                                             "_1992_1_1_materiaux_label_ec2")),
                            check);
    gtk_widget_set_visible (GTK_WIDGET (gtk_builder_get_object (builder,
                                    "_1992_1_1_materiaux_scrolledwindow_ec2")),
                            check);
  }
  else if (checkmenuitem == GTK_CHECK_MENU_ITEM (gtk_builder_get_object (
                      builder, "_1992_1_1_materiaux_personnaliser_menu_ecu2")))
  {
    if (check && mat)
    {
      conv_f_c (m_f (m_g (beton_data->ecu2) * 1000, beton_data->ecu2.type),
                tmp,
                DECIMAL_SANS_UNITE);
      gtk_text_buffer_set_text (GTK_TEXT_BUFFER (gtk_builder_get_object (
                                  builder, "_1992_1_1_materiaux_buffer_ecu2")),
                                tmp,
                                -1);
    }
    gtk_widget_set_visible (GTK_WIDGET (gtk_builder_get_object (builder,
                                            "_1992_1_1_materiaux_label_ecu2")),
                            check);
    gtk_widget_set_visible (GTK_WIDGET (gtk_builder_get_object (builder,
                                   "_1992_1_1_materiaux_scrolledwindow_ecu2")),
                            check);
  }
  else if (checkmenuitem == GTK_CHECK_MENU_ITEM (gtk_builder_get_object (
                         builder, "_1992_1_1_materiaux_personnaliser_menu_n")))
  {
    if (check && mat)
    {
      conv_f_c (beton_data->n, tmp, DECIMAL_SANS_UNITE);
      gtk_text_buffer_set_text (GTK_TEXT_BUFFER (gtk_builder_get_object (
                                     builder, "_1992_1_1_materiaux_buffer_n")),
                                tmp,
                                -1);
    }
    gtk_widget_set_visible (GTK_WIDGET (gtk_builder_get_object (builder,
                                               "_1992_1_1_materiaux_label_n")),
                            check);
    gtk_widget_set_visible (GTK_WIDGET (gtk_builder_get_object (builder,
                                      "_1992_1_1_materiaux_scrolledwindow_n")),
                            check);
  }
  else if (checkmenuitem == GTK_CHECK_MENU_ITEM (gtk_builder_get_object (
                       builder, "_1992_1_1_materiaux_personnaliser_menu_ec3")))
  {
    if (check && mat)
    {
      conv_f_c (m_f (m_g (beton_data->ec3) * 1000, beton_data->ec3.type),
                tmp,
                DECIMAL_SANS_UNITE);
      gtk_text_buffer_set_text (GTK_TEXT_BUFFER (gtk_builder_get_object (
                                   builder, "_1992_1_1_materiaux_buffer_ec3")),
                                tmp,
                                -1);
    }
    gtk_widget_set_visible (GTK_WIDGET (gtk_builder_get_object (builder,
                                             "_1992_1_1_materiaux_label_ec3")),
                            check);
    gtk_widget_set_visible (GTK_WIDGET (gtk_builder_get_object (builder,
                                    "_1992_1_1_materiaux_scrolledwindow_ec3")),
                            check);
  }
  else if (checkmenuitem == GTK_CHECK_MENU_ITEM (gtk_builder_get_object (
                      builder, "_1992_1_1_materiaux_personnaliser_menu_ecu3")))
  {
    if (check && mat)
    {
      conv_f_c (m_f (m_g (beton_data->ecu3) * 1000, beton_data->ecu3.type),
                tmp,
                DECIMAL_SANS_UNITE);
      gtk_text_buffer_set_text (GTK_TEXT_BUFFER (gtk_builder_get_object (
                                  builder, "_1992_1_1_materiaux_buffer_ecu3")),
                                tmp,
                                -1);
    }
    gtk_widget_set_visible (GTK_WIDGET (gtk_builder_get_object (builder,
                                            "_1992_1_1_materiaux_label_ecu3")),
                            check);
    gtk_widget_set_visible (GTK_WIDGET (gtk_builder_get_object (builder,
                                   "_1992_1_1_materiaux_scrolledwindow_ecu3")),
                            check);
  }
  else if (checkmenuitem == GTK_CHECK_MENU_ITEM (gtk_builder_get_object (
                        builder, "_1992_1_1_materiaux_personnaliser_menu_nu")))
  {
    if (check && mat)
    {
      conv_f_c (beton_data->nu, tmp, DECIMAL_SANS_UNITE);
      gtk_text_buffer_set_text (GTK_TEXT_BUFFER (gtk_builder_get_object (
                                    builder, "_1992_1_1_materiaux_buffer_nu")),
                                tmp,
                                -1);
    }
    gtk_widget_set_visible (GTK_WIDGET (gtk_builder_get_object (builder,
                                              "_1992_1_1_materiaux_label_nu")),
                            check);
    gtk_widget_set_visible (GTK_WIDGET (gtk_builder_get_object (builder,
                                     "_1992_1_1_materiaux_scrolledwindow_nu")),
                            check);
  }
  else
  {
    FAILPARAM (checkmenuitem, "%p", )
  }
  
  _1992_1_1_gtk_materiaux_check (NULL, p);
  
  return;
}


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
gboolean
_1992_1_1_gtk_materiaux (Projet      *p,
                         EF_Materiau *materiau)
{
  Materiau_Beton *beton_data;
  
  BUGPARAMCRIT (p, "%p", p, FALSE)
  
  if (UI_BET.builder != NULL)
  {
    gtk_window_present (GTK_WINDOW (UI_BET.window));
    if (UI_BET.materiau == materiau)
    {
      return TRUE;
    }
  }
  else
  {
    UI_BET.builder = gtk_builder_new ();
    BUGCRIT (gtk_builder_add_from_resource (UI_BET.builder,
                                  "/org/2lgc/codegui/ui/1992_1_1_materiaux.ui",
                                           NULL) != 0,
            FALSE,
            (gettext ("La génération de la fenêtre %s a échouée.\n"),
                      "Ajout Matériau Béton"); )
    gtk_builder_connect_signals (UI_BET.builder, p);
    UI_BET.window = GTK_WIDGET (gtk_builder_get_object (UI_BET.builder,
                                                "_1992_1_1_materiaux_window"));
  }
  
  if (materiau == NULL)
  {
    gtk_window_set_title (GTK_WINDOW (UI_BET.window),
                          gettext ("Ajout d'un matériau béton"));
    UI_BET.materiau = NULL;
    beton_data = NULL;
    gtk_text_buffer_set_text (gtk_text_view_get_buffer (GTK_TEXT_VIEW (
                 gtk_builder_get_object (UI_BET.builder,
                                         "_1992_1_1_materiaux_textview_nom"))),
                              "",
                              -1);
    gtk_text_buffer_set_text (gtk_text_view_get_buffer (GTK_TEXT_VIEW (
                 gtk_builder_get_object (UI_BET.builder,
                                         "_1992_1_1_materiaux_textview_fck"))),
                              "",
                              -1);
    
    gtk_button_set_label (GTK_BUTTON (gtk_builder_get_object (UI_BET.builder,
                                       "_1992_1_1_materiaux_button_add_edit")),
                          gettext ("_Ajouter"));
    g_signal_connect (gtk_builder_get_object (UI_BET.builder,
                                        "_1992_1_1_materiaux_button_add_edit"),
                      "clicked",
                      G_CALLBACK (_1992_1_1_gtk_materiaux_ajouter_clicked),
                      p);
    _1992_1_1_gtk_materiaux_check (NULL, p);
  }
  else
  {
    gchar tmp[30];
    
    BUGCRIT (materiau->type == MATERIAU_BETON,
             FALSE,
             (gettext ("Le matériau n'est pas en béton.\n")); )
    gtk_window_set_title (GTK_WINDOW (UI_BET.window),
                          gettext ("Modification d'un matériau béton"));
    UI_BET.materiau = materiau;
    beton_data = materiau->data;
    
    gtk_text_buffer_set_text (gtk_text_view_get_buffer (GTK_TEXT_VIEW (
                 gtk_builder_get_object (UI_BET.builder,
                                         "_1992_1_1_materiaux_textview_nom"))),
                              materiau->nom,
                              -1);
    conv_f_c (m_f (m_g (beton_data->fck) / 1000000., beton_data->fck.type),
              tmp,
              DECIMAL_CONTRAINTE);
    gtk_text_buffer_set_text (GTK_TEXT_BUFFER (gtk_builder_get_object (
                            UI_BET.builder, "_1992_1_1_materiaux_buffer_fck")),
                              tmp,
                              -1);
    
    gtk_button_set_label (GTK_BUTTON (gtk_builder_get_object (UI_BET.builder,
                                       "_1992_1_1_materiaux_button_add_edit")),
                          gettext ("_Modifier"));
    g_signal_connect (gtk_builder_get_object (UI_BET.builder,
                                        "_1992_1_1_materiaux_button_add_edit"),
                      "clicked",
                      G_CALLBACK (_1992_1_1_gtk_materiaux_modifier_clicked),
                      p);
  }
  
  // On affiche les propriétés qui ne sont pas égale à celle par défaut.
  gtk_check_menu_item_set_active (
    GTK_CHECK_MENU_ITEM (gtk_builder_get_object (UI_BET.builder,
                           "_1992_1_1_materiaux_personnaliser_menu_fck_cube")),
    beton_data &&
    !errrel (m_g (beton_data->fckcube),
             _1992_1_1_materiaux_fckcube (m_g (beton_data->fck) / 1000000.)));
  gtk_check_menu_item_set_active (
    GTK_CHECK_MENU_ITEM (gtk_builder_get_object (UI_BET.builder,
                                "_1992_1_1_materiaux_personnaliser_menu_fcm")),
    beton_data &&
    !errrel (m_g (beton_data->fcm),
             _1992_1_1_materiaux_fcm (m_g (beton_data->fck) / 1000000.)));
  gtk_check_menu_item_set_active (
    GTK_CHECK_MENU_ITEM (gtk_builder_get_object (UI_BET.builder,
                               "_1992_1_1_materiaux_personnaliser_menu_fctm")),
    beton_data &&
    !errrel (m_g (beton_data->fctm),
             _1992_1_1_materiaux_fctm (m_g (beton_data->fck) / 1000000.,
                                       m_g (beton_data->fcm) / 1000000.)));
  gtk_check_menu_item_set_active (
    GTK_CHECK_MENU_ITEM (gtk_builder_get_object (UI_BET.builder,
                          "_1992_1_1_materiaux_personnaliser_menu_fctk_0_05")),
    beton_data &&
    !errrel (m_g (beton_data->fctk_0_05),
           _1992_1_1_materiaux_fctk_0_05 (m_g (beton_data->fctm) / 1000000.)));
  gtk_check_menu_item_set_active (
    GTK_CHECK_MENU_ITEM (gtk_builder_get_object (UI_BET.builder,
                          "_1992_1_1_materiaux_personnaliser_menu_fctk_0_95")),
    beton_data &&
    !errrel (m_g (beton_data->fctk_0_95),
           _1992_1_1_materiaux_fctk_0_95 (m_g (beton_data->fctm) / 1000000.)));
  gtk_check_menu_item_set_active (
    GTK_CHECK_MENU_ITEM (gtk_builder_get_object (UI_BET.builder,
                                "_1992_1_1_materiaux_personnaliser_menu_Ecm")),
    beton_data &&
    !errrel (m_g (beton_data->ecm),
             _1992_1_1_materiaux_ecm (m_g (beton_data->fcm) / 1000000.)));
  gtk_check_menu_item_set_active (
    GTK_CHECK_MENU_ITEM (gtk_builder_get_object (UI_BET.builder,
                                "_1992_1_1_materiaux_personnaliser_menu_ec1")),
    beton_data &&
    !errrel (m_g (beton_data->ec1),
             _1992_1_1_materiaux_ec1 (m_g (beton_data->fcm) / 1000000.)));
  gtk_check_menu_item_set_active (
    GTK_CHECK_MENU_ITEM (gtk_builder_get_object (UI_BET.builder,
                               "_1992_1_1_materiaux_personnaliser_menu_ecu1")),
    beton_data &&
    !errrel (m_g (beton_data->ecu1),
             _1992_1_1_materiaux_ecu1 (m_g (beton_data->fcm) / 1000000.,
                                       m_g (beton_data->fck) / 1000000.)));
  gtk_check_menu_item_set_active (
    GTK_CHECK_MENU_ITEM (gtk_builder_get_object (UI_BET.builder,
                                "_1992_1_1_materiaux_personnaliser_menu_ec2")),
    beton_data &&
    !errrel (m_g (beton_data->ec2),
             _1992_1_1_materiaux_ec2 (m_g (beton_data->fck) / 1000000.)));
  gtk_check_menu_item_set_active (
    GTK_CHECK_MENU_ITEM (gtk_builder_get_object (UI_BET.builder,
                               "_1992_1_1_materiaux_personnaliser_menu_ecu2")),
    beton_data &&
    !errrel (m_g (beton_data->ecu2),
             _1992_1_1_materiaux_ecu2 (m_g (beton_data->fck) / 1000000.)));
  gtk_check_menu_item_set_active (
    GTK_CHECK_MENU_ITEM (gtk_builder_get_object (UI_BET.builder,
                                  "_1992_1_1_materiaux_personnaliser_menu_n")),
    beton_data &&
    !errrel (m_g (beton_data->n),
             _1992_1_1_materiaux_n (m_g (beton_data->fck) / 1000000.)));
  gtk_check_menu_item_set_active(
    GTK_CHECK_MENU_ITEM ( gtk_builder_get_object (UI_BET.builder,
                                "_1992_1_1_materiaux_personnaliser_menu_ec3")),
    beton_data &&
    !errrel (m_g (beton_data->ec3),
             _1992_1_1_materiaux_ec3 (m_g (beton_data->fck) / 1000000.)));
  gtk_check_menu_item_set_active (
    GTK_CHECK_MENU_ITEM (gtk_builder_get_object (UI_BET.builder,
                               "_1992_1_1_materiaux_personnaliser_menu_ecu3")),
    beton_data &&
    !errrel (m_g (beton_data->ecu3),
             _1992_1_1_materiaux_ecu3 (m_g (beton_data->fck) / 1000000.)));
  gtk_check_menu_item_set_active (
    GTK_CHECK_MENU_ITEM (gtk_builder_get_object (UI_BET.builder,
                                 "_1992_1_1_materiaux_personnaliser_menu_nu")),
    beton_data &&
    !errrel (m_g (beton_data->nu), COEFFICIENT_NU_BETON));
  
  gtk_window_set_transient_for (GTK_WINDOW (UI_BET.window),
                                GTK_WINDOW (UI_GTK.window));
  
  return TRUE;
}


/**
 * \brief Lance la fenêtre permettant d'ajouter un matériau béton.
 * \param menuitem : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 * Echec :
 *   - p == NULL,
 *   - interface graphique non initialisée.
 */
extern "C"
void
_1992_1_1_gtk_materiaux_ajout (GtkMenuItem *menuitem,
                               Projet      *p)
{
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_MAT.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Ajout Matériau Béton"); )
  
  BUG (_1992_1_1_gtk_materiaux (p, NULL), )
}


#endif

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
