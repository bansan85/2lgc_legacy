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

#include <gtk/gtk.h>

#include <algorithm>
#include <locale>

#include "common_projet.hpp"
#include "common_erreurs.hpp"
#include "common_gtk.hpp"
#include "common_selection.hpp"
#include "common_math.hpp"
#include "EF_charge.hpp"
#include "EF_charge_barre_repartie_uniforme.hpp"
#include "EF_calculs.hpp"
#include "EF_gtk_charge_barre_repartie_uniforme.hpp"


GTK_WINDOW_CLOSE (ef, charge_barre_repartie_uniforme);


GTK_WINDOW_KEY_PRESS (ef, charge_barre_repartie_uniforme);


GTK_WINDOW_DESTROY (ef, charge_barre_repartie_uniforme, );


/**
 * \brief Récupère toutes les données de la fenêtre permettant d'ajouter ou
 *        d'éditer une charge répartie uniformément sur barre.
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
 * \param projection : si utilisatation d'un projection sur la barre,
 * \param a : début de la charge par rapport au début de la barre,
 * \param b : fin de la charge par rapport à la fin de la charge.
 * \return
 *   Succès : true.\n
 *   Échec : false :
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
 *     - projection == NULL,
 *     - a == NULL,
 *     - b == NULL,
 *     - en cas d'erreur d'allocation mémoire,
 *     - interface graphique non initialisée.
 */
bool
EF_gtk_charge_barre_repartie_uniforme_recupere (
  Projet                  *p,
  Action                 **action,
  std::list <EF_Barre *> **barres,
  double                  *fx,
  double                  *fy,
  double                  *fz,
  double                  *mx,
  double                  *my,
  double                  *mz,
  std::string             *nom,
  bool                    *repere_local,
  bool                    *projection,
  double                  *a,
  double                  *b)
{
  GtkTextIter    start, end;
  gchar         *texte_tmp;
  GtkTextBuffer *textbuffer;
  bool           ok = true;
  std::string    str_tmp;
  
  std::list <uint32_t> *num_barres;
  
  BUGPARAM (p, "%p", p, false)
  BUGPARAM (action, "%p", action, false)
  BUGPARAM (barres, "%p", barres, false)
  BUGPARAM (fx, "%p", fx, false)
  BUGPARAM (fy, "%p", fy, false)
  BUGPARAM (fz, "%p", fz, false)
  BUGPARAM (mx, "%p", mx, false)
  BUGPARAM (my, "%p", my, false)
  BUGPARAM (mz, "%p", mz, false)
  BUGPARAM (nom, "%p", nom, false)
  BUGPARAM (repere_local, "%p", repere_local, false)
  BUGPARAM (projection, "%p", projection, false)
  BUGPARAM (a, "%p", a, false)
  BUGPARAM (b, "%p", b, false)
  BUGCRIT (UI_CHBARR.builder,
           false,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Charge Barre Répartie Uniforme"); )
  
  if (gtk_combo_box_get_active (UI_CHBARR.combobox_charge) < 0)
  {
    ok = false;
  }
  else
  {
    *action = *std::next (p->actions.begin (),
                          gtk_combo_box_get_active (
                                                   UI_CHBARR.combobox_charge));
  }
  
  *fx = conv_buff_d (GTK_TEXT_BUFFER (gtk_builder_get_object (
                      UI_CHBARR.builder, "EF_charge_barre_rep_uni_buffer_fx")),
                     -INFINITY, false,
                     INFINITY, false);
  if (std::isnan (*fx))
  {
    ok = false;
  }
  
  *fy = conv_buff_d (GTK_TEXT_BUFFER (gtk_builder_get_object (
                      UI_CHBARR.builder, "EF_charge_barre_rep_uni_buffer_fy")),
                     -INFINITY, false,
                     INFINITY, false);
  if (std::isnan (*fy))
  {
    ok = false;
  }
  
  *fz = conv_buff_d (GTK_TEXT_BUFFER (gtk_builder_get_object (
                      UI_CHBARR.builder, "EF_charge_barre_rep_uni_buffer_fz")),
                     -INFINITY, false,
                     INFINITY, false);
  if (std::isnan (*fz))
  {
    ok = false;
  }
  
  *mx = conv_buff_d (GTK_TEXT_BUFFER (gtk_builder_get_object (
                      UI_CHBARR.builder, "EF_charge_barre_rep_uni_buffer_mx")),
                     -INFINITY, false,
                     INFINITY, false);
  if (std::isnan (*mx))
  {
    ok = false;
  }
  
  *my = conv_buff_d (GTK_TEXT_BUFFER (gtk_builder_get_object (
                      UI_CHBARR.builder, "EF_charge_barre_rep_uni_buffer_my")),
                     -INFINITY, false,
                     INFINITY, false);
  if (std::isnan (*my))
  {
    ok = false;
  }
  
  *mz = conv_buff_d (GTK_TEXT_BUFFER (gtk_builder_get_object (
                      UI_CHBARR.builder, "EF_charge_barre_rep_uni_buffer_mz")),
                     -INFINITY, false,
                     INFINITY, false);
  if (std::isnan (*mz))
  {
    ok = false;
  }
  
  *a = conv_buff_d (GTK_TEXT_BUFFER (gtk_builder_get_object (UI_CHBARR.builder,
                                          "EF_charge_barre_rep_uni_buffer_a")),
                    0, true,
                    INFINITY, false);
  if (std::isnan (*a))
  {
    ok = false;
  }
  
  *b = conv_buff_d (GTK_TEXT_BUFFER (gtk_builder_get_object (UI_CHBARR.builder,
                                          "EF_charge_barre_rep_uni_buffer_b")),
                    0, true,
                    INFINITY, false);
  if (std::isnan (*b))
  {
    ok = false;
  }
  
  *repere_local = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (
                                     gtk_builder_get_object (UI_CHBARR.builder,
                                      "EF_charge_barre_rep_uni_radio_local")));
  
  *projection = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (
                                     gtk_builder_get_object (UI_CHBARR.builder,
                                 "EF_charge_barre_rep_uni_check_projection")));
  
  textbuffer = GTK_TEXT_BUFFER (gtk_builder_get_object (UI_CHBARR.builder,
                                     "EF_charge_barre_rep_uni_buffer_barres"));
  gtk_text_buffer_get_iter_at_offset (textbuffer, &start, 0);
  gtk_text_buffer_get_iter_at_offset (textbuffer, &end, -1);
  texte_tmp = gtk_text_buffer_get_text (textbuffer, &start, &end, FALSE);
  str_tmp.assign (texte_tmp);
  num_barres = common_selection_renvoie_numeros (&str_tmp);
  free (texte_tmp);
  
  if (num_barres == NULL)
  {
    ok = false;
  }
  else
  {
    *barres = common_selection_numeros_en_barres (num_barres, p);
    if (*barres == NULL)
    {
      ok = false;
    }
    else
    {
      // Si tous les paramètres sont corrects
      textbuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (
                                     gtk_builder_get_object (UI_CHBARR.builder,
                             "EF_charge_barre_rep_uni_textview_description")));
      
      gtk_text_buffer_get_iter_at_offset (textbuffer, &start, 0);
      gtk_text_buffer_get_iter_at_offset (textbuffer, &end, -1);
      *nom = gtk_text_buffer_get_text (textbuffer, &start, &end, FALSE);
      
      if (nom->length () == 0)
      {
        ok = false;
      }
    }
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
EF_gtk_charge_barre_rep_uni_check (GtkWidget *button,
                                   Projet    *p)
{
  Action     *action;
  double      fx, fy, fz, mx, my, mz;
  std::string nom;
  bool        repere_local, projection;
  double      a, b;
  
  std::list <EF_Barre *> *barres;
  
  BUGPARAM (p, "%p", p, )
  BUGCRIT (UI_CHBARR.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Charge Barre Répartie Uniforme"); )
  
  if (!EF_gtk_charge_barre_repartie_uniforme_recupere (p,
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
                                                       &projection,
                                                       &a,
                                                       &b))
  {
    gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
                UI_CHBARR.builder, "EF_charge_barre_rep_uni_button_add_edit")),
                              FALSE);
  }
  else
  {
    gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
                UI_CHBARR.builder, "EF_charge_barre_rep_uni_button_add_edit")),
                              TRUE);
    delete barres;
  }
  
  return;
}


/**
 * \brief Ferme la fenêtre en ajoutant la charge.
 * \param button : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
void
EF_gtk_charge_barre_repartie_uniforme_ajouter (GtkButton *button,
                                               Projet    *p)
{
  double      fx, fy, fz, mx, my, mz, a, b;
  Action     *action;
  bool        repere_local, projection;
  std::string texte;
  
  std::list <EF_Barre *> *barres;
  
  BUGPARAM (p, "%p", p, )
  BUGCRIT (UI_CHBARR.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Charge Barre Répartie Uniforme"); )
  
  BUG (EF_gtk_charge_barre_repartie_uniforme_recupere (p,
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
                                                       &projection,
                                                       &a,
                                                       &b),
      )
  
  // Création de la nouvelle charge répartie uniformément sur barre
  BUG (EF_charge_barre_repartie_uniforme_ajout (p,
                                                action,
                                                barres,
                                                repere_local,
                                                projection,
                                                m_f (a, FLOTTANT_UTILISATEUR),
                                                m_f (b, FLOTTANT_UTILISATEUR),
                                                m_f (fx, FLOTTANT_UTILISATEUR),
                                                m_f (fy, FLOTTANT_UTILISATEUR),
                                                m_f (fz, FLOTTANT_UTILISATEUR),
                                                m_f (mx, FLOTTANT_UTILISATEUR),
                                                m_f (my, FLOTTANT_UTILISATEUR),
                                                m_f (mz, FLOTTANT_UTILISATEUR),
                                                &texte),
       , )
  
  gtk_widget_destroy (UI_CHBARR.window);
  
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
EF_gtk_charge_barre_repartie_uniforme_editer (GtkButton *button,
                                              Projet    *p)
{
  double      fx, fy, fz, mx, my, mz, a, b;
  Action     *action;
  bool        repere_local, projection;
  std::string texte;
  
  Charge_Barre_Repartie_Uniforme *charge_d;
  std::list <EF_Barre *>         *barres;
  
  BUGPARAM (p, "%p", p, )
  BUGCRIT (UI_CHBARR.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Charge Barre Répartie Uniforme"); )
  
  BUG (EF_gtk_charge_barre_repartie_uniforme_recupere (p,
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
                                                       &projection,
                                                       &a,
                                                       &b),
      )
  
  // Création de la nouvelle charge répartie uniformément sur barre
  UI_CHBARR.charge->nom.assign (texte);
  charge_d = (Charge_Barre_Repartie_Uniforme *) UI_CHBARR.charge->data;
  charge_d->barres.assign (barres->begin (), barres->end ());
  charge_d->repere_local = repere_local;
  charge_d->projection = projection;
  charge_d->a = m_f (a, FLOTTANT_UTILISATEUR);
  charge_d->b = m_f (b, FLOTTANT_UTILISATEUR);
  charge_d->fx = m_f (fx, FLOTTANT_UTILISATEUR);
  charge_d->fy = m_f (fy, FLOTTANT_UTILISATEUR);
  charge_d->fz = m_f (fz, FLOTTANT_UTILISATEUR);
  charge_d->mx = m_f (mx, FLOTTANT_UTILISATEUR);
  charge_d->my = m_f (my, FLOTTANT_UTILISATEUR);
  charge_d->mz = m_f (mz, FLOTTANT_UTILISATEUR);
  if (action != UI_CHBARR.action)
  {
    BUG (EF_charge_deplace (p, UI_CHBARR.action, UI_CHBARR.charge, action), )
  }
  else
  {
    gtk_widget_queue_resize (GTK_WIDGET (UI_ACT.tree_view_charges));
  }
  
  gtk_widget_destroy (UI_CHBARR.window);
  
  BUG (EF_calculs_free (p), )
  
  return;
}


/**
 * \brief Évènement lors du cochage de projection et repère globale. En effet,
 *        il n'est pas possible de faire à la fois une projection dans le
 *        repère local.
 * \param togglebutton : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
extern "C"
void
EF_gtk_charge_barre_repartie_uniforme_toggled (GtkToggleButton *togglebutton,
                                               Projet          *p)
{
  BUGPARAM (p, "%p", p, )
  BUGCRIT (UI_CHBARR.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Charge Barre Répartie Uniforme"); )
  
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (
                                         UI_CHBARR.check_button_repere_local)))
  {
    gtk_widget_set_sensitive (UI_CHBARR.check_button_projection, FALSE);
    gtk_widget_set_sensitive (UI_CHBARR.check_button_repere_local, TRUE);
    gtk_widget_set_sensitive (UI_CHBARR.check_button_repere_global, TRUE);
  }
  else if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (
                                           UI_CHBARR.check_button_projection)))
  {
    gtk_widget_set_sensitive (UI_CHBARR.check_button_projection, TRUE);
    gtk_widget_set_sensitive (UI_CHBARR.check_button_repere_global, TRUE);
    gtk_widget_set_sensitive (UI_CHBARR.check_button_repere_local, FALSE);
  }
  else
  {
    gtk_widget_set_sensitive (UI_CHBARR.check_button_projection, TRUE);
    gtk_widget_set_sensitive (UI_CHBARR.check_button_repere_local, TRUE);
    gtk_widget_set_sensitive (UI_CHBARR.check_button_repere_global, TRUE);
  }
}


/**
 * \brief Affichage de la fenêtre permettant de créer ou modifier une action de
 *        type charge repartie uniforme sur barre.
 * \param p : la variable projet,
 * \param action_defaut : action par défaut dans la fenêtre,
 * \param charge : vaut NULL si une nouvelle charge doit être ajoutée,
 *                 vaut la charge si elle doit être modifiée.
 * \return
 *   Succès : true.\n
 *   Echec : false :
 *     - p == NULL,
 *     - interface graphique impossible à générer.
 */
bool
EF_gtk_charge_barre_repartie_uniforme (Projet *p,
                                       Action *action_defaut,
                                       Charge *charge)
{
  BUGPARAM (p, "%p", p, false)
  if (UI_CHBARR.builder != NULL)
  {
    gtk_window_present (GTK_WINDOW (UI_CHBARR.window));
    return true;
  }
  
  UI_CHBARR.builder = gtk_builder_new ();
  BUGCRIT (gtk_builder_add_from_resource (UI_CHBARR.builder,
                   "/org/2lgc/codegui/ui/EF_charge_barre_repartie_uniforme.ui",
                                          NULL) != 0,
           false,
           (gettext ("La génération de la fenêtre %s a échouée.\n"),
                     "Charge Barre Répartie Uniforme"); )
  gtk_builder_connect_signals (UI_CHBARR.builder, p);
  
  UI_CHBARR.window = GTK_WIDGET (gtk_builder_get_object (UI_CHBARR.builder,
                                            "EF_charge_barre_rep_uni_window"));
  UI_CHBARR.combobox_charge = GTK_COMBO_BOX (gtk_builder_get_object (
               UI_CHBARR.builder, "EF_charge_barre_rep_uni_combo_box_charge"));
  UI_CHBARR.check_button_repere_local = GTK_WIDGET (gtk_builder_get_object (
                    UI_CHBARR.builder, "EF_charge_barre_rep_uni_radio_local"));
  UI_CHBARR.check_button_repere_global = GTK_WIDGET (gtk_builder_get_object (
                   UI_CHBARR.builder, "EF_charge_barre_rep_uni_radio_global"));
  UI_CHBARR.check_button_projection = GTK_WIDGET (gtk_builder_get_object (
               UI_CHBARR.builder, "EF_charge_barre_rep_uni_check_projection"));
  
  if (charge == NULL)
  {
    UI_CHBARR.action = 0;
    UI_CHBARR.charge = 0;
    gtk_window_set_title (GTK_WINDOW (UI_CHBARR.window),
                          gettext ("Ajout d'une charge répartie uniforme sur barre"));
  }
  else
  {
    UI_CHBARR.action = action_defaut;
    UI_CHBARR.charge = charge;
    gtk_window_set_title (GTK_WINDOW (UI_CHBARR.window),
                          gettext ("Modification d'une charge répartie uniforme sur barre"));
  }
  
  gtk_combo_box_set_model (GTK_COMBO_BOX (UI_CHBARR.combobox_charge),
                           GTK_TREE_MODEL (UI_ACT.liste));
  gtk_combo_box_set_active (GTK_COMBO_BOX (UI_CHBARR.combobox_charge),
                            (gint) std::distance (
                                     p->actions.begin (),
                                     std::find (p->actions.begin (),
                                                p->actions.end (),
                                                action_defaut)));
  
  if (charge != NULL)
  {
    std::string                     tmp;
    Charge_Barre_Repartie_Uniforme *charge_d;
    
    gtk_text_buffer_set_text (gtk_text_view_get_buffer (GTK_TEXT_VIEW (
                                     gtk_builder_get_object (UI_CHBARR.builder,
                             "EF_charge_barre_rep_uni_textview_description"))),
                              charge->nom.c_str (),
                              -1);
    charge_d = (Charge_Barre_Repartie_Uniforme *) charge->data;
    conv_f_c (charge_d->fx, &tmp, DECIMAL_FORCE);
    gtk_text_buffer_set_text (GTK_TEXT_BUFFER (gtk_builder_get_object (
                      UI_CHBARR.builder, "EF_charge_barre_rep_uni_buffer_fx")),
                              tmp.c_str (),
                              -1);
    conv_f_c (charge_d->fy, &tmp, DECIMAL_FORCE);
    gtk_text_buffer_set_text (GTK_TEXT_BUFFER (gtk_builder_get_object (
                      UI_CHBARR.builder, "EF_charge_barre_rep_uni_buffer_fy")),
                              tmp.c_str (),
                              -1);
    conv_f_c (charge_d->fz, &tmp, DECIMAL_FORCE);
    gtk_text_buffer_set_text (GTK_TEXT_BUFFER (gtk_builder_get_object (
                      UI_CHBARR.builder, "EF_charge_barre_rep_uni_buffer_fz")),
                              tmp.c_str (),
                              -1);
    conv_f_c (charge_d->mx, &tmp, DECIMAL_MOMENT);
    gtk_text_buffer_set_text (GTK_TEXT_BUFFER (gtk_builder_get_object (
                      UI_CHBARR.builder, "EF_charge_barre_rep_uni_buffer_mx")),
                              tmp.c_str (),
                              -1);
    conv_f_c (charge_d->my, &tmp, DECIMAL_MOMENT);
    gtk_text_buffer_set_text (GTK_TEXT_BUFFER (gtk_builder_get_object (
                      UI_CHBARR.builder, "EF_charge_barre_rep_uni_buffer_my")),
                              tmp.c_str (),
                              -1);
    conv_f_c (charge_d->mz, &tmp, DECIMAL_MOMENT);
    gtk_text_buffer_set_text (GTK_TEXT_BUFFER (gtk_builder_get_object (
                      UI_CHBARR.builder, "EF_charge_barre_rep_uni_buffer_mz")),
                              tmp.c_str (),
                              -1);
    conv_f_c (charge_d->a, &tmp, DECIMAL_DISTANCE);
    gtk_text_buffer_set_text (GTK_TEXT_BUFFER (gtk_builder_get_object (
                       UI_CHBARR.builder, "EF_charge_barre_rep_uni_buffer_a")),
                              tmp.c_str (),
                              -1);
    conv_f_c (charge_d->b, &tmp, DECIMAL_DISTANCE);
    gtk_text_buffer_set_text (GTK_TEXT_BUFFER (gtk_builder_get_object (
                       UI_CHBARR.builder, "EF_charge_barre_rep_uni_buffer_b")),
                              tmp.c_str (),
                              -1);
    if ((charge_d->repere_local) && (charge_d->projection))
    {
      GtkWidget *dialog;
      
      dialog = gtk_message_dialog_new (GTK_WINDOW (UI_CHBARR.window),
                                       GTK_DIALOG_MODAL,
                                       GTK_MESSAGE_ERROR,
                                       GTK_BUTTONS_OK,
                                       gettext ("Il n'est pas possible d'activer à la fois la projection et l'utilisation du repère local."));
      gtk_dialog_run (GTK_DIALOG (dialog));
      gtk_widget_destroy (dialog);
      
      return false;
    }
    
    if (charge_d->repere_local)
    {
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (
                                          UI_CHBARR.check_button_repere_local),
                                    TRUE);
    }
    else
    {
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (
                                         UI_CHBARR.check_button_repere_global),
                                    TRUE);
    }
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (
                                            UI_CHBARR.check_button_projection),
                                  charge_d->projection);
    tmp = common_selection_barres_en_texte (&charge_d->barres);
    gtk_text_buffer_set_text (GTK_TEXT_BUFFER (gtk_builder_get_object (
                  UI_CHBARR.builder, "EF_charge_barre_rep_uni_buffer_barres")),
                              tmp.c_str (),
                              -1);
    gtk_button_set_label (GTK_BUTTON (gtk_builder_get_object 
               (UI_CHBARR.builder, "EF_charge_barre_rep_uni_button_add_edit")),
                          gettext ("_Modifier"));
    g_signal_connect (gtk_builder_get_object (UI_CHBARR.builder,
                                    "EF_charge_barre_rep_uni_button_add_edit"),
                      "clicked",
                     G_CALLBACK (EF_gtk_charge_barre_repartie_uniforme_editer),
                      p);
  }
  else
  {
    gtk_button_set_label (GTK_BUTTON (gtk_builder_get_object (
                UI_CHBARR.builder, "EF_charge_barre_rep_uni_button_add_edit")),
                          gettext ("_Ajouter"));
    g_signal_connect (gtk_builder_get_object (UI_CHBARR.builder,
                                    "EF_charge_barre_rep_uni_button_add_edit"),
                      "clicked",
                    G_CALLBACK (EF_gtk_charge_barre_repartie_uniforme_ajouter),
                      p);
  }
  
  EF_gtk_charge_barre_rep_uni_check (NULL, p);
  
  if (UI_ACT.window == NULL)
  {
    gtk_window_set_transient_for (GTK_WINDOW (UI_CHBARR.window),
                                  GTK_WINDOW (UI_GTK.window));
  }
  else
  {
    gtk_window_set_transient_for (GTK_WINDOW (UI_CHBARR.window),
                                  GTK_WINDOW (UI_ACT.window));
  }
  
  return true;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
