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
#include "common_math.hpp"
#include "common_selection.hpp"
#include "EF_charge_noeud.hpp"
#include "EF_charge.hpp"
#include "EF_calculs.hpp"
#include "EF_gtk_charge_noeud.hpp"


GTK_WINDOW_CLOSE (ef, charge_noeud);


GTK_WINDOW_KEY_PRESS (ef, charge_noeud);


GTK_WINDOW_DESTROY (ef, charge_noeud, );


/**
 * \brief Récupère toutes les données de la fenêtre permettant d'ajouter ou
 *        d'éditer une charge nodale.
 * \param p : la variable projet,
 * \param action : numéro de l'action où sera ajoutée la charge,
 * \param noeuds : liste des noeuds qui supportera la charge,
 * \param fx : force selon x,
 * \param fy : force selon y,
 * \param fz : force selon z,
 * \param mx : moment selon x,
 * \param my : moment selon y,
 * \param mz : moment selon z,
 * \param nom : nom de l'action.
 * \return
 *   Succès : true.\n
 *   Échec : false :
 *     - p == NULL,
 *     - action == NULL,
 *     - noeuds == NULL,
 *     - fx == NULL,
 *     - fy == NULL,
 *     - fz == NULL,
 *     - mx == NULL,
 *     - my == NULL,
 *     - mz == NULL,
 *     - nom == NULL,
 *     - en cas d'erreur d'allocation mémoire,
 *     - interface graphique non initialisée.
 */
bool
EF_gtk_charge_noeud_recupere (Projet                  *p,
                              Action                 **action,
                              std::list <EF_Noeud *> **noeuds,
                              double                  *fx,
                              double                  *fy,
                              double                  *fz,
                              double                  *mx,
                              double                  *my,
                              double                  *mz,
                              std::string             *nom)
{
  std::list <uint32_t> *num_noeuds;
  std::string           str_tmp;
  
  GtkTextIter    start, end;
  gchar         *texte_tmp;
  GtkTextBuffer *textbuffer;
  gint           get_active;
  bool           ok = true;
  
  BUGPARAM (p, "%p", p, false)
  BUGPARAM (action, "%p", action, false)
  BUGPARAM (noeuds, "%p", noeuds, false)
  BUGPARAM (fx, "%p", fx, false)
  BUGPARAM (fy, "%p", fy, false)
  BUGPARAM (fz, "%p", fz, false)
  BUGPARAM (mx, "%p", mx, false)
  BUGPARAM (my, "%p", my, false)
  BUGPARAM (mz, "%p", mz, false)
  BUGPARAM (nom, "%p", nom, false)
  BUGCRIT (UI_CHNO.builder,
           false,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Charge Nodale"); )
  
  get_active = gtk_combo_box_get_active (GTK_COMBO_BOX (
                                                     UI_CHNO.combobox_charge));
  if (get_active < 0)
  {
    ok = false;
  }
  else
  {
    *action = *std::next (p->actions.begin (), get_active);
  }
  
  *fx = conv_buff_d (GTK_TEXT_BUFFER (gtk_builder_get_object (UI_CHNO.builder,
                                                 "EF_charge_noeud_buffer_fx")),
                     -INFINITY, false,
                     INFINITY, false);
  if (isnan (*fx))
  {
    ok = false;
  }
  
  *fy = conv_buff_d (GTK_TEXT_BUFFER (gtk_builder_get_object (UI_CHNO.builder,
                                                 "EF_charge_noeud_buffer_fy")),
                     -INFINITY, false,
                     INFINITY, false);
  if (isnan (*fy))
  {
    ok = false;
  }
  
  *fz = conv_buff_d (GTK_TEXT_BUFFER (gtk_builder_get_object (UI_CHNO.builder,
                                                 "EF_charge_noeud_buffer_fz")),
                     -INFINITY, false,
                     INFINITY, false);
  if (isnan (*fz))
  {
    ok = false;
  }
  
  *mx = conv_buff_d (GTK_TEXT_BUFFER (gtk_builder_get_object (UI_CHNO.builder,
                                                 "EF_charge_noeud_buffer_mx")),
                     -INFINITY, false,
                     INFINITY, false);
  if (isnan (*mx))
  {
    ok = false;
  }
  
  *my = conv_buff_d (GTK_TEXT_BUFFER (gtk_builder_get_object (UI_CHNO.builder,
                                                 "EF_charge_noeud_buffer_my")),
                     -INFINITY, false,
                     INFINITY, false);
  if (isnan (*my))
  {
    ok = false;
  }
  
  *mz = conv_buff_d (GTK_TEXT_BUFFER (gtk_builder_get_object (UI_CHNO.builder,
                                                 "EF_charge_noeud_buffer_mz")),
                     -INFINITY, false,
                     INFINITY, false);
  if (isnan (*mz))
  {
    ok = false;
  }
  
  textbuffer = GTK_TEXT_BUFFER (gtk_builder_get_object (UI_CHNO.builder,
                                             "EF_charge_noeud_buffer_noeuds"));
  gtk_text_buffer_get_iter_at_offset (textbuffer, &start, 0);
  gtk_text_buffer_get_iter_at_offset (textbuffer, &end, -1);
  texte_tmp = gtk_text_buffer_get_text (textbuffer, &start, &end, FALSE);
  str_tmp.assign (texte_tmp);
  num_noeuds = common_selection_renvoie_numeros (&str_tmp);
  free (texte_tmp);
  if (num_noeuds == NULL)
  {
    ok = false;
  }
  else
  {
    *noeuds = common_selection_numeros_en_noeuds (num_noeuds, p);
    delete num_noeuds;
    if (*noeuds == NULL)
    {
      ok = false;
    }
    else
    {
      // Si tous les paramètres sont corrects
      textbuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (
                                       gtk_builder_get_object (UI_CHNO.builder,
                                     "EF_charge_noeud_textview_description")));
      
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
EF_gtk_charge_noeud_check (GtkWidget *button,
                           Projet    *p)
{
  Action     *action;
  double      fx, fy, fz, mx, my, mz;
  std::string nom;
  
  std::list <EF_Noeud *> *noeuds;
  
  BUGPARAM (p, "%p", p, )
  BUGCRIT (UI_CHNO.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Charge Nodale"); )
  
  if (!EF_gtk_charge_noeud_recupere (p,
                                     &action,
                                     &noeuds,
                                     &fx,
                                     &fy,
                                     &fz,
                                     &mx,
                                     &my,
                                     &mz,
                                     &nom))
  {
    gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
                          UI_CHNO.builder, "EF_charge_noeud_button_add_edit")),
                              FALSE);
  }
  else
  {
    gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
                          UI_CHNO.builder, "EF_charge_noeud_button_add_edit")),
                              TRUE);
    delete noeuds;
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
EF_gtk_charge_noeud_ajouter (GtkButton *button,
                             Projet    *p)
{
  double      fx, fy, fz, mx, my, mz;
  Action     *action;
  std::string texte;
  
  std::list <EF_Noeud *> *noeuds;
  
  BUGPARAM (p, "%p", p, )
  BUGCRIT (UI_CHNO.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Charge Nodale"); )
  
  BUG (EF_gtk_charge_noeud_recupere (p,
                                     &action,
                                     &noeuds,
                                     &fx,
                                     &fy,
                                     &fz,
                                     &mx,
                                     &my,
                                     &mz,
                                     &texte),
      )
  
  // Création de la nouvelle charge nodale
  BUG (EF_charge_noeud_ajout (p,
                              action,
                              noeuds,
                              m_f (fx, FLOTTANT_UTILISATEUR),
                              m_f (fy, FLOTTANT_UTILISATEUR),
                              m_f (fz, FLOTTANT_UTILISATEUR),
                              m_f (mx, FLOTTANT_UTILISATEUR),
                              m_f (my, FLOTTANT_UTILISATEUR),
                              m_f (mz, FLOTTANT_UTILISATEUR),
                              &texte),
      , )
  
  gtk_widget_destroy (UI_CHNO.window);
  
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
EF_gtk_charge_noeud_editer (GtkButton *button,
                            Projet    *p)
{
  double        fx, fy, fz, mx, my, mz;
  Action       *action;
  std::string   texte;
  Charge_Noeud *charge_d;
  
  std::list <EF_Noeud *> *noeuds;
  
  BUGPARAM (p, "%p", p, )
  BUGCRIT (UI_CHNO.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Charge Nodale"); )
  
  BUG (EF_gtk_charge_noeud_recupere (p,
                                     &action,
                                     &noeuds,
                                     &fx,
                                     &fy,
                                     &fz,
                                     &mx,
                                     &my,
                                     &mz,
                                     &texte),
       )
  
  // Création de la nouvelle charge nodale
  UI_CHNO.charge->nom.assign (texte);
  charge_d = (Charge_Noeud *) UI_CHNO.charge->data;
  charge_d->fx = m_f (fx, FLOTTANT_UTILISATEUR);
  charge_d->fy = m_f (fy, FLOTTANT_UTILISATEUR);
  charge_d->fz = m_f (fz, FLOTTANT_UTILISATEUR);
  charge_d->mx = m_f (mx, FLOTTANT_UTILISATEUR);
  charge_d->my = m_f (my, FLOTTANT_UTILISATEUR);
  charge_d->mz = m_f (mz, FLOTTANT_UTILISATEUR);
  charge_d->noeuds.assign (noeuds->begin (), noeuds->end ());
  if (action != UI_CHNO.action)
  {
    BUG (EF_charge_deplace (p, UI_CHNO.action, UI_CHNO.charge, action), )
  }
  else
  {
    gtk_widget_queue_resize (GTK_WIDGET (UI_ACT.tree_view_charges));
  }
  
  gtk_widget_destroy (UI_CHNO.window);
  
  BUG (EF_calculs_free (p), )
  
  return;
}


/**
 * \brief Affichage de la fenêtre permettant de créer ou modifier une action de
 *        type charge nodale.
 * \param p : la variable projet
 * \param action_defaut : action par défaut dans la fenêtre,
 * \param charge : vaut NULL si une nouvelle charge doit être ajoutée,
 *                 vaut le numéro de la charge si elle doit être modifiée.
 * \return
 *   Succès : true.\n
 *   Echec : false :
 *     - p == NULL,
 *     - interface graphique impossible à générer.
 */
bool
EF_gtk_charge_noeud (Projet *p,
                     Action *action_defaut,
                     Charge *charge)
{
  BUGPARAM (p, "%p", p, false)
  
  if (UI_CHNO.builder != NULL)
  {
    gtk_window_present (GTK_WINDOW (UI_CHNO.window));
    return true;
  }
  
  UI_CHNO.builder = gtk_builder_new ();
  BUGCRIT (gtk_builder_add_from_resource (UI_CHNO.builder,
                                     "/org/2lgc/codegui/ui/EF_charge_noeud.ui",
                                          NULL) != 0,
           false,
           (gettext ("La génération de la fenêtre %s a échouée.\n"),
                     "Charge Nodale"); )
  gtk_builder_connect_signals (UI_CHNO.builder, p);
  
  UI_CHNO.window = GTK_WIDGET (gtk_builder_get_object (UI_CHNO.builder,
                                                    "EF_charge_noeud_window"));
  UI_CHNO.combobox_charge = GTK_WIDGET (gtk_builder_get_object (
                         UI_CHNO.builder, "EF_charge_noeud_combo_box_charge"));
  
  if (charge == NULL)
  {
    UI_CHNO.action = 0;
    UI_CHNO.charge = 0;
    gtk_window_set_title (GTK_WINDOW (UI_CHNO.window),
                          gettext ("Ajout d'une charge au noeud"));
  }
  else
  {
    UI_CHNO.action = action_defaut;
    UI_CHNO.charge = charge;
    gtk_window_set_title (GTK_WINDOW (UI_CHNO.window),
                          gettext ("Modification d'une charge au noeud"));
  }
  
  gtk_combo_box_set_model (GTK_COMBO_BOX (UI_CHNO.combobox_charge),
                           GTK_TREE_MODEL (UI_ACT.liste));
  gtk_combo_box_set_active (GTK_COMBO_BOX (UI_CHNO.combobox_charge),
                            (gint) std::distance (p->actions.begin (),
                                     std::find (p->actions.begin (),
                                                p->actions.end (),
                                                action_defaut)));
  
  if (charge != NULL)
  {
    Charge_Noeud *charge_d;
    std::string   tmp;
    
    charge_d = (Charge_Noeud *) charge->data;
    gtk_text_buffer_set_text (gtk_text_view_get_buffer (GTK_TEXT_VIEW (
                                       gtk_builder_get_object (UI_CHNO.builder,
                                     "EF_charge_noeud_textview_description"))),
                              charge->nom.c_str (),
                              -1);
    conv_f_c (charge_d->fx, &tmp, DECIMAL_FORCE);
    gtk_text_buffer_set_text (GTK_TEXT_BUFFER (gtk_builder_get_object (
                                UI_CHNO.builder, "EF_charge_noeud_buffer_fx")),
                              tmp.c_str (),
                              -1);
    conv_f_c (charge_d->fy, &tmp, DECIMAL_FORCE);
    gtk_text_buffer_set_text (GTK_TEXT_BUFFER (gtk_builder_get_object (
                                UI_CHNO.builder, "EF_charge_noeud_buffer_fy")),
                              tmp.c_str (),
                              -1);
    conv_f_c (charge_d->fz, &tmp, DECIMAL_FORCE);
    gtk_text_buffer_set_text (GTK_TEXT_BUFFER (gtk_builder_get_object (
                                UI_CHNO.builder, "EF_charge_noeud_buffer_fz")),
                              tmp.c_str (),
                              -1);
    conv_f_c (charge_d->mx, &tmp, DECIMAL_MOMENT);
    gtk_text_buffer_set_text (GTK_TEXT_BUFFER (gtk_builder_get_object (
                                UI_CHNO.builder, "EF_charge_noeud_buffer_mx")),
                              tmp.c_str (),
                              -1);
    conv_f_c (charge_d->my, &tmp, DECIMAL_MOMENT);
    gtk_text_buffer_set_text (GTK_TEXT_BUFFER (gtk_builder_get_object (
                                UI_CHNO.builder, "EF_charge_noeud_buffer_my")),
                              tmp.c_str (),
                              -1);
    conv_f_c (charge_d->mz, &tmp, DECIMAL_MOMENT);
    gtk_text_buffer_set_text (GTK_TEXT_BUFFER (gtk_builder_get_object (
                                UI_CHNO.builder, "EF_charge_noeud_buffer_mz")),
                              tmp.c_str (),
                              -1);
    tmp = common_selection_noeuds_en_texte (&charge_d->noeuds);
    gtk_text_buffer_set_text (GTK_TEXT_BUFFER (gtk_builder_get_object (
                            UI_CHNO.builder, "EF_charge_noeud_buffer_noeuds")),
                              tmp.c_str (),
                              -1);
    gtk_button_set_label (GTK_BUTTON (gtk_builder_get_object (UI_CHNO.builder,
                                           "EF_charge_noeud_button_add_edit")),
                          gettext ("_Modifier"));
    g_signal_connect (gtk_builder_get_object (UI_CHNO.builder,
                                            "EF_charge_noeud_button_add_edit"),
                      "clicked",
                      G_CALLBACK (EF_gtk_charge_noeud_editer),
                      p);
  }
  else
  {
    gtk_button_set_label (GTK_BUTTON (gtk_builder_get_object (UI_CHNO.builder,
                                           "EF_charge_noeud_button_add_edit")),
                          gettext ("_Ajouter"));
    g_signal_connect (gtk_builder_get_object (UI_CHNO.builder,
                                            "EF_charge_noeud_button_add_edit"),
                      "clicked",
                      G_CALLBACK (EF_gtk_charge_noeud_ajouter),
                      p);
  }
  
  EF_gtk_charge_noeud_check (NULL, p);
  
  if (UI_ACT.window == NULL)
  {
    gtk_window_set_transient_for (GTK_WINDOW (UI_CHNO.window),
                                  GTK_WINDOW (UI_GTK.window));
  }
  else
  {
    gtk_window_set_transient_for (GTK_WINDOW (UI_CHNO.window),
                                  GTK_WINDOW (UI_ACT.window));
  }
  
  return true;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
