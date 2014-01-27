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
#include <string.h>
#include <gtk/gtk.h>
#include <math.h>

#include "common_projet.h"
#include "common_erreurs.h"
#include "common_selection.h"
#include "common_gtk.h"
#include "common_math.h"
#include "EF_charge.h"
#include "EF_charge_barre_ponctuelle.h"
#include "EF_calculs.h"


GTK_WINDOW_CLOSE (ef, charge_barre_ponctuelle);


GTK_WINDOW_KEY_PRESS (ef, charge_barre_ponctuelle);


GTK_WINDOW_DESTROY (ef, charge_barre_ponctuelle, );


gboolean EF_gtk_charge_barre_ponctuelle_recupere (Projet   *p,
                                                  Action  **action,
                                                  GList   **barres,
                                                  double   *fx,
                                                  double   *fy,
                                                  double   *fz,
                                                  double   *mx,
                                                  double   *my,
                                                  double   *mz,
                                                  gchar   **nom,
                                                  gboolean *repere_local,
                                                  double   *position)
/**
 * \brief Récupère toutes les données de la fenêtre permettant d'ajouter ou
 *        d'éditer une charge ponctuelle sur barre.
 * \param p : la variable projet,
 * \param action : l'action où sera ajoutée la charge,
 * \param barres : liste des barres qui supportera la charge,
 * \param fx : force selon x,
 * \param fy : force selon y,
 * \param fz : force selon z,
 * \param mx : moment selon x,
 * \param my : moment selon y,
 * \param mz : moment selon z,
 * \param nom : nom de l'action,
 * \param repere_local : si utilisation du repère local,
 * \param position : position de la charge sur la barre.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL,
 *     - action == NULL,
 *     - barres == NULL,
 *     - fx == NULL,
 *     - fy == NULL,
 *     - fz == NULL,
 *     - mx == NULL,
 *     - my == NULL,
 *     - mz == NULL,
 *     - nom == NULL,
 *     - repere_local == NULL,
 *     - position == NULL,
 *     - en cas d'erreur d'allocation mémoire,
 *     - interface graphique non initialisée.
 */
{
  GList         *num_barres;
  GtkTextIter    start, end;
  gchar         *texte_tmp;
  GtkTextBuffer *textbuffer;
  gboolean       ok = TRUE;
  
  BUGMSG (p, FALSE, gettext ("Paramètre %s incorrect.\n"), "projet")
  BUGMSG (action, FALSE, gettext ("Paramètre %s incorrect.\n"), "action")
  BUGMSG (barres, FALSE, gettext ("Paramètre %s incorrect.\n"), "barres")
  BUGMSG (fx, FALSE, gettext ("Paramètre %s incorrect.\n"), "fx")
  BUGMSG (fy, FALSE, gettext ("Paramètre %s incorrect.\n"), "fy")
  BUGMSG (fz, FALSE, gettext ("Paramètre %s incorrect.\n"), "fz")
  BUGMSG (mx, FALSE, gettext ("Paramètre %s incorrect.\n"), "mx")
  BUGMSG (my, FALSE, gettext ("Paramètre %s incorrect.\n"), "my")
  BUGMSG (mz, FALSE, gettext ("Paramètre %s incorrect.\n"), "mz")
  BUGMSG (nom, FALSE, gettext ("Paramètre %s incorrect.\n"), "nom")
  BUGMSG (repere_local,
          FALSE,
          gettext ("Paramètre %s incorrect.\n"), "repere_local")
  BUGMSG (position, FALSE, gettext ("Paramètre %s incorrect.\n"), "position")
  BUGMSG (UI_CHBARP.builder,
          FALSE,
          gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                   "Charge Barre Ponctuelle")
  
  if (gtk_combo_box_get_active (UI_CHBARP.combobox_charge) < 0)
    ok = FALSE;
  else
    *action = g_list_nth_data (p->actions,
                               (unsigned int) gtk_combo_box_get_active (
                                                   UI_CHBARP.combobox_charge));
  
  *fx = conv_buff_d (GTK_TEXT_BUFFER (gtk_builder_get_object (
                        UI_CHBARP.builder, "EF_charge_barre_ponct_buffer_fx")),
                     -INFINITY,
                     FALSE,
                     INFINITY,
                     FALSE);
  if (isnan (*fx))
    ok = FALSE;
  
  *fy = conv_buff_d (GTK_TEXT_BUFFER (gtk_builder_get_object (
                        UI_CHBARP.builder, "EF_charge_barre_ponct_buffer_fy")),
                     -INFINITY,
                     FALSE,
                     INFINITY,
                     FALSE);
  if (isnan (*fy))
    ok = FALSE;
  
  *fz = conv_buff_d (GTK_TEXT_BUFFER (gtk_builder_get_object (
                        UI_CHBARP.builder, "EF_charge_barre_ponct_buffer_fz")),
                     -INFINITY,
                     FALSE,
                     INFINITY,
                     FALSE);
  if (isnan (*fz))
    ok = FALSE;
  
  *mx = conv_buff_d (GTK_TEXT_BUFFER (gtk_builder_get_object (
                        UI_CHBARP.builder, "EF_charge_barre_ponct_buffer_mx")),
                     -INFINITY,
                     FALSE,
                     INFINITY,
                     FALSE);
  if (isnan (*mx))
    ok = FALSE;
  
  *my = conv_buff_d (GTK_TEXT_BUFFER (gtk_builder_get_object (
                        UI_CHBARP.builder, "EF_charge_barre_ponct_buffer_my")),
                     -INFINITY,
                     FALSE,
                     INFINITY,
                     FALSE);
  if (isnan (*my))
    ok = FALSE;
  
  *mz = conv_buff_d (GTK_TEXT_BUFFER (gtk_builder_get_object (
                        UI_CHBARP.builder, "EF_charge_barre_ponct_buffer_mz")),
                     -INFINITY,
                     FALSE,
                     INFINITY,
                     FALSE);
  if (isnan (*mz))
    ok = FALSE;
  
  *position = conv_buff_d (GTK_TEXT_BUFFER (gtk_builder_get_object (
                       UI_CHBARP.builder, "EF_charge_barre_ponct_buffer_pos")),
                           0,
                           TRUE,
                           INFINITY,
                           FALSE);
  if (isnan (*position))
    ok = FALSE;
  
  *repere_local = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (
                                     gtk_builder_get_object (UI_CHBARP.builder,
                                        "EF_charge_barre_ponct_radio_local")));
  
  textbuffer = GTK_TEXT_BUFFER (gtk_builder_get_object (UI_CHBARP.builder,
                                       "EF_charge_barre_ponct_buffer_barres"));
  gtk_text_buffer_get_iter_at_offset (textbuffer, &start, 0);
  gtk_text_buffer_get_iter_at_offset (textbuffer, &end, -1);
  texte_tmp = gtk_text_buffer_get_text (textbuffer, &start, &end, FALSE);
  num_barres = common_selection_renvoie_numeros (texte_tmp);
  if (num_barres == NULL)
    ok = FALSE;
  else
  {
    *barres = common_selection_numeros_en_barres (num_barres, p);
    g_list_free (num_barres);
    if (*barres == NULL)
      ok = FALSE;
    else
    {
      // Si tous les paramètres sont corrects
      textbuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (
                                     gtk_builder_get_object (UI_CHBARP.builder,
                               "EF_charge_barre_ponct_textview_description")));
      
      gtk_text_buffer_get_iter_at_offset (textbuffer, &start, 0);
      gtk_text_buffer_get_iter_at_offset (textbuffer, &end, -1);
      *nom = gtk_text_buffer_get_text (textbuffer, &start, &end, FALSE);
      
      if (strcmp (*nom, "") == 0)
      {
        free (*nom);
        *nom = NULL;
        ok = FALSE;
      }
    }
  }
  
  free (texte_tmp);
  
  return ok;
}


void
EF_gtk_charge_barre_ponct_check (GtkWidget *button,
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
  Action  *action;
  GList   *barres;
  double   fx, fy, fz, mx, my, mz;
  gchar   *nom = NULL;
  gboolean repere_local;
  double   position;
  
  BUGMSG (p, , gettext ("Paramètre %s incorrect.\n"), "projet")
  BUGMSG (UI_CHBARP.builder,
          ,
          gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                   "Charge Barre Ponctuelle")
  
  if (!EF_gtk_charge_barre_ponctuelle_recupere (p,
                                                &action,
                                                &barres,
                                                &fx,
                                                &fy,
                                                &fz,
                                                &mx,
                                                &my,
                                                &mz,
                                                &nom,
                                                &repere_local,
                                                &position))
    gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
                  UI_CHBARP.builder, "EF_charge_barre_ponct_button_add_edit")),
                              FALSE);
  else
  {
    gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
                  UI_CHBARP.builder, "EF_charge_barre_ponct_button_add_edit")),
                              TRUE);
    g_list_free(barres);
  }
  free (nom);
  
  return;
}


void
EF_gtk_charge_barre_ponctuelle_ajouter (GtkButton *button,
                                        Projet    *p)
/**
 * \brief Ferme la fenêtre en ajoutant la charge.
 * \param button : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
{
  double   fx, fy, fz, mx, my, mz, position;
  Action  *action;
  gboolean repere_local;
  GList   *barres;
  gchar   *texte;
  
  BUGMSG (p, , gettext ("Paramètre %s incorrect.\n"), "projet")
  BUGMSG (UI_CHBARP.builder,
          ,
          gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                   "Charge Barre Ponctuelle")
  
  BUG (EF_gtk_charge_barre_ponctuelle_recupere (p,
                                                &action,
                                                &barres,
                                                &fx,
                                                &fy,
                                                &fz,
                                                &mx,
                                                &my,
                                                &mz,
                                                &texte,
                                                &repere_local,
                                                &position) == TRUE,
      )
  
  // Création de la nouvelle charge ponctuelle sur barre
  BUG (EF_charge_barre_ponctuelle_ajout (p,
                                         action,
                                         barres,
                                         repere_local,
                                         m_f (position, FLOTTANT_UTILISATEUR),
                                         m_f (fx, FLOTTANT_UTILISATEUR),
                                         m_f (fy, FLOTTANT_UTILISATEUR),
                                         m_f (fz, FLOTTANT_UTILISATEUR),
                                         m_f (mx, FLOTTANT_UTILISATEUR),
                                         m_f (my, FLOTTANT_UTILISATEUR),
                                         m_f (mz, FLOTTANT_UTILISATEUR),
                                         texte),
      )
  
  free (texte);
  
  gtk_widget_destroy (UI_CHBARP.window);
  
  return;
}


void
EF_gtk_charge_barre_ponctuelle_editer (GtkButton *button,
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
  double                   fx, fy, fz, mx, my, mz, position;
  gboolean                 repere_local;
  Action                  *action;
  GList                   *barres;
  gchar                   *texte;
  Charge_Barre_Ponctuelle *charge_d;
  
  BUGMSG (p, , gettext ("Paramètre %s incorrect.\n"), "projet")
  BUGMSG (UI_CHBARP.builder,
          ,
          gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                   "Charge Barre Ponctuelle")
  
  BUG (EF_gtk_charge_barre_ponctuelle_recupere (p,
                                                &action,
                                                &barres,
                                                &fx,
                                                &fy,
                                                &fz,
                                                &mx,
                                                &my,
                                                &mz,
                                                &texte,
                                                &repere_local,
                                                &position) == TRUE,
      )
  
  // Création de la nouvelle charge ponctuelle sur barre
  free (UI_CHBARP.charge->nom);
  UI_CHBARP.charge->nom = texte;
  charge_d = UI_CHBARP.charge->data;
  charge_d->fx = m_f (fx, FLOTTANT_UTILISATEUR);
  charge_d->fy = m_f (fy, FLOTTANT_UTILISATEUR);
  charge_d->fz = m_f (fz, FLOTTANT_UTILISATEUR);
  charge_d->mx = m_f (mx, FLOTTANT_UTILISATEUR);
  charge_d->my = m_f (my, FLOTTANT_UTILISATEUR);
  charge_d->mz = m_f (mz, FLOTTANT_UTILISATEUR);
  g_list_free (charge_d->barres);
  charge_d->barres = barres;
  charge_d->position = m_f (position, FLOTTANT_UTILISATEUR);
  charge_d->repere_local = repere_local;
  if (action != UI_CHBARP.action)
    BUG (EF_charge_deplace (p, UI_CHBARP.action, UI_CHBARP.charge, action), )
  else
    gtk_widget_queue_resize (GTK_WIDGET (UI_ACT.tree_view_charges));
  
  gtk_widget_destroy (UI_CHBARP.window);
  
  BUG (EF_calculs_free (p), )
  
  return;
}


gboolean
EF_gtk_charge_barre_ponctuelle (Projet *p,
                                Action *action_defaut,
                                Charge *charge)
/**
 * \brief Affichage de la fenêtre permettant de créer ou modifier une action de
 *        type charge ponctuelle sur barre.
 * \param p : la variable projet,
 * \param action_defaut : action par défaut dans la fenêtre,
 * \param charge : vaut NULL si une nouvelle charge doit être ajoutée,
 *                 vaut la charge si elle doit être modifiée.
 * \return
 *   Succès : TRUE.\n
 *   Echec : FALSE :
 *     - p == NULL,
 *     - interface graphique impossible à générer.
 */
{
  BUGMSG (p, FALSE, gettext ("Paramètre %s incorrect.\n"), "projet")
  
  if (UI_CHBARP.builder != NULL)
  {
    gtk_window_present (GTK_WINDOW (UI_CHBARP.window));
    return TRUE;
  }
  
  UI_CHBARP.builder = gtk_builder_new ();
  BUGMSG (gtk_builder_add_from_resource (UI_CHBARP.builder,
                          "/org/2lgc/codegui/ui/EF_charge_barre_ponctuelle.ui",
                                         NULL) != 0,
          FALSE,
          gettext ("Builder Failed\n"))
  gtk_builder_connect_signals (UI_CHBARP.builder, p);
  
  UI_CHBARP.window = GTK_WIDGET (gtk_builder_get_object (UI_CHBARP.builder,
                                              "EF_charge_barre_ponct_window"));
  UI_CHBARP.combobox_charge = GTK_COMBO_BOX (gtk_builder_get_object (
                 UI_CHBARP.builder, "EF_charge_barre_ponct_combo_box_charge"));
  
  if (charge == NULL)
  {
    UI_CHBARP.action = 0;
    UI_CHBARP.charge = 0;
    gtk_window_set_title (GTK_WINDOW (UI_CHBARP.window),
                          gettext ("Ajout d'une charge ponctuelle sur barre"));
  }
  else
  {
    UI_CHBARP.action = action_defaut;
    UI_CHBARP.charge = charge;
    gtk_window_set_title (GTK_WINDOW (UI_CHBARP.window),
                          gettext ("Modification d'une charge ponctuelle sur barre"));
  }
  
  gtk_combo_box_set_model (GTK_COMBO_BOX (UI_CHBARP.combobox_charge),
                           GTK_TREE_MODEL (UI_ACT.liste));
  gtk_combo_box_set_active (GTK_COMBO_BOX (UI_CHBARP.combobox_charge),
                            g_list_index (p->actions, action_defaut));
  
  if (charge != NULL)
  {
    gchar                    tmp[30], *tmp2;
    Charge_Barre_Ponctuelle *charge_d;
    
    gtk_text_buffer_set_text (gtk_text_view_get_buffer (GTK_TEXT_VIEW (
                                     gtk_builder_get_object (UI_CHBARP.builder,
                               "EF_charge_barre_ponct_textview_description"))),
                              charge->nom,
                              -1);
    charge_d = charge->data;
    conv_f_c (charge_d->fx, tmp, DECIMAL_FORCE);
    gtk_text_buffer_set_text (GTK_TEXT_BUFFER (gtk_builder_get_object (
                        UI_CHBARP.builder, "EF_charge_barre_ponct_buffer_fx")),
                              tmp,
                              -1);
    conv_f_c (charge_d->fy, tmp, DECIMAL_FORCE);
    gtk_text_buffer_set_text (GTK_TEXT_BUFFER (gtk_builder_get_object (
                        UI_CHBARP.builder, "EF_charge_barre_ponct_buffer_fy")),
                        tmp,
                        -1);
    conv_f_c (charge_d->fz, tmp, DECIMAL_FORCE);
    gtk_text_buffer_set_text (GTK_TEXT_BUFFER (gtk_builder_get_object (
                        UI_CHBARP.builder, "EF_charge_barre_ponct_buffer_fz")),
                              tmp,
                              -1);
    conv_f_c (charge_d->mx, tmp, DECIMAL_MOMENT);
    gtk_text_buffer_set_text (GTK_TEXT_BUFFER (gtk_builder_get_object (
                        UI_CHBARP.builder, "EF_charge_barre_ponct_buffer_mx")),
                              tmp,
                              -1);
    conv_f_c (charge_d->my, tmp, DECIMAL_MOMENT);
    gtk_text_buffer_set_text (GTK_TEXT_BUFFER (gtk_builder_get_object (
                        UI_CHBARP.builder, "EF_charge_barre_ponct_buffer_my")),
                              tmp,
                              -1);
    conv_f_c (charge_d->mz, tmp, DECIMAL_MOMENT);
    gtk_text_buffer_set_text (GTK_TEXT_BUFFER (gtk_builder_get_object (
                        UI_CHBARP.builder, "EF_charge_barre_ponct_buffer_mz")),
                              tmp,
                              -1);
    conv_f_c (charge_d->position, tmp, DECIMAL_DISTANCE);
    gtk_text_buffer_set_text (GTK_TEXT_BUFFER (gtk_builder_get_object (
                       UI_CHBARP.builder, "EF_charge_barre_ponct_buffer_pos")),
                              tmp,
                              -1);
    if (charge_d->repere_local)
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gtk_builder_get_object (
                      UI_CHBARP.builder, "EF_charge_barre_ponct_radio_local")),
                                    TRUE);
    else
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gtk_builder_get_object (
                     UI_CHBARP.builder, "EF_charge_barre_ponct_radio_global")),
                                    TRUE);
    BUG (tmp2 = common_selection_barres_en_texte (charge_d->barres), FALSE)
    gtk_text_buffer_set_text (GTK_TEXT_BUFFER (gtk_builder_get_object (
                    UI_CHBARP.builder, "EF_charge_barre_ponct_buffer_barres")),
                              tmp2,
                              -1);
    free (tmp2);
  }
  
  if (charge == NULL)
  {
    gtk_button_set_label (GTK_BUTTON (gtk_builder_get_object (
                  UI_CHBARP.builder, "EF_charge_barre_ponct_button_add_edit")),
                          gettext ("_Ajouter"));
    g_signal_connect (gtk_builder_get_object (UI_CHBARP.builder,
                                      "EF_charge_barre_ponct_button_add_edit"),
                      "clicked",
                      G_CALLBACK (EF_gtk_charge_barre_ponctuelle_ajouter),
                      p);
  }
  else
  {
    gtk_button_set_label (GTK_BUTTON (gtk_builder_get_object (
                  UI_CHBARP.builder, "EF_charge_barre_ponct_button_add_edit")),
                          gettext ("_Modifier"));
    g_signal_connect (gtk_builder_get_object (UI_CHBARP.builder,
                                      "EF_charge_barre_ponct_button_add_edit"),
                      "clicked",
                      G_CALLBACK (EF_gtk_charge_barre_ponctuelle_editer),
                      p);
  }
  
  EF_gtk_charge_barre_ponct_check (NULL, p);
  
  if (UI_ACT.window == NULL)
    gtk_window_set_transient_for (GTK_WINDOW (UI_CHBARP.window),
                                  GTK_WINDOW (p->ui.comp.window));
  else
    gtk_window_set_transient_for (GTK_WINDOW (UI_CHBARP.window),
                                  GTK_WINDOW (UI_ACT.window));
  return TRUE;
}


#endif
