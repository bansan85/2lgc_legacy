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

#include "common_m3d.hpp"

#include "common_projet.h"
#include "common_math.h"
#include "common_erreurs.h"
#include "common_gtk.h"
#include "common_selection.h"
#include "EF_materiaux.h"
#include "EF_noeuds.h"
#include "EF_relachement.h"
#include "EF_sections.h"
#include "1992_1_1_barres.h"
#include "1992_1_1_materiaux.h"


GTK_WINDOW_CLOSE(ef, barres_add);


GTK_WINDOW_DESTROY (ef, barres_add, );


GTK_WINDOW_KEY_PRESS (ef, barres_add);


void
EF_gtk_barres_add_add_clicked (GtkButton *button,
                               Projet    *p)
/**
 * \brief Aoute une nouvelle barre.
 * \param button : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
{
  int             type;
  char           *nom;
  Section        *section;
  EF_Materiau    *materiau;
  unsigned int    noeud_debut;
  unsigned int    noeud_fin;
  double          angle;
  EF_Relachement *relachement;
  unsigned int    nb_noeuds;
  GtkTreeModel   *model;
  GtkTreeIter     Iter;
  
  BUGMSG (p, , gettext ("Paramètre %s incorrect.\n"), "projet")
  BUGMSG (UI_BARADD.builder,
          ,
          gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                   "Ajout Appui")
  
  model = gtk_combo_box_get_model (GTK_COMBO_BOX (gtk_builder_get_object (
                    UI_BARADD.builder, "EF_gtk_barres_add_section_combobox")));
  type = gtk_combo_box_get_active (GTK_COMBO_BOX (gtk_builder_get_object (
                    UI_BARADD.builder, "EF_gtk_barres_add_section_combobox")));
  if (type == -1)
    return;
  gtk_tree_model_iter_nth_child (model, &Iter, NULL, type);
  gtk_tree_model_get (model, &Iter, 0, &nom, -1);
  BUG (section = EF_sections_cherche_nom (p, nom, TRUE), )
  free (nom);
  
  model = gtk_combo_box_get_model (GTK_COMBO_BOX (gtk_builder_get_object (
                   UI_BARADD.builder, "EF_gtk_barres_add_materiau_combobox")));
  type = gtk_combo_box_get_active (GTK_COMBO_BOX (gtk_builder_get_object (
                   UI_BARADD.builder, "EF_gtk_barres_add_materiau_combobox")));
  if (type == -1)
    return;
  gtk_tree_model_iter_nth_child (model, &Iter, NULL, type);
  gtk_tree_model_get (model, &Iter, 0, &nom, -1);
  BUG (materiau = EF_materiaux_cherche_nom (p, nom, TRUE), )
  free (nom);
  
  model = gtk_combo_box_get_model (GTK_COMBO_BOX (gtk_builder_get_object (
                UI_BARADD.builder, "EF_gtk_barres_add_relachement_combobox")));
  type = gtk_combo_box_get_active (GTK_COMBO_BOX (gtk_builder_get_object (
                UI_BARADD.builder, "EF_gtk_barres_add_relachement_combobox")));
  if (type == -1)
    return;
  else if (type == 0)
    relachement = NULL;
  else
  {
    gtk_tree_model_iter_nth_child (model, &Iter, NULL, type);
    gtk_tree_model_get (model, &Iter, 0, &nom, -1);
    BUG (relachement = EF_relachement_cherche_nom (p, nom, TRUE), )
    free (nom);
  }
  
  noeud_debut = conv_buff_u (GTK_TEXT_BUFFER (gtk_builder_get_object (
                        UI_BARADD.builder, "EF_gtk_barres_add_noeud1_buffer")),
                             0,
                             TRUE,
                             UINT_MAX,
                             FALSE);
  noeud_fin = conv_buff_u (GTK_TEXT_BUFFER (gtk_builder_get_object (
                        UI_BARADD.builder, "EF_gtk_barres_add_noeud2_buffer")),
                           0,
                           TRUE,
                           UINT_MAX,
                           FALSE);
  if ((EF_noeuds_cherche_numero (p, noeud_debut, TRUE) == NULL) ||
      (EF_noeuds_cherche_numero (p, noeud_fin, TRUE) == NULL))
    return;
  
  angle = conv_buff_d (GTK_TEXT_BUFFER (gtk_builder_get_object (
                         UI_BARADD.builder, "EF_gtk_barres_add_angle_buffer")),
                       -360.,
                       FALSE,
                       360.,
                       FALSE);
  if (isnan (angle))
    return;
  
  type = gtk_combo_box_get_active (GTK_COMBO_BOX (gtk_builder_get_object (
                       UI_BARADD.builder, "EF_gtk_barres_add_type_combobox")));
  
  nb_noeuds = conv_buff_u (GTK_TEXT_BUFFER (gtk_builder_get_object (
       UI_BARADD.builder, "EF_gtk_barres_add_nb_noeuds_intermediaire_buffer")),
                           0,
                           TRUE,
                           UINT_MAX,
                           FALSE);
  BUG (nb_noeuds != UINT_MAX, )
  
  BUG (_1992_1_1_barres_ajout (p,
                               (Type_Element) type,
                               section,
                               materiau,
                               EF_noeuds_cherche_numero (p, noeud_debut, TRUE),
                               EF_noeuds_cherche_numero (p, noeud_fin, TRUE),
                               m_f (angle, FLOTTANT_UTILISATEUR),
                               relachement,
                               nb_noeuds),
      )
  BUG (m3d_rafraichit (p), )
  
  return;
}


void
EF_gtk_barres_add_check_add (GtkWidget *widget,
                             Projet    *p)
/**
 * \brief Vérifie à chaque modification d'un champ si la fenêtre possède toutes
 *        les informations correctes pour créer une barre et active / désactive
 *        en fonction le bouton ajouter.
 * \param widget : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
{
  gboolean       ok = FALSE;
  EF_Noeud      *noeud1, *noeud2;
  GtkTextIter    start, end;
  GtkTextBuffer *buff;
  
  BUGMSG (p, , gettext ("Paramètre %s incorrect.\n"), "projet")
  BUGMSG (UI_BARADD.builder,
          ,
          gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                   "Ajout Appui")
  
  noeud1 = EF_noeuds_cherche_numero (
             p,
             conv_buff_u (GTK_TEXT_BUFFER (gtk_builder_get_object (
                        UI_BARADD.builder, "EF_gtk_barres_add_noeud1_buffer")),
             0,
             TRUE,
             UINT_MAX,
             FALSE),
           FALSE);
  buff = GTK_TEXT_BUFFER (gtk_builder_get_object (UI_BARADD.builder,
                                           "EF_gtk_barres_add_noeud1_buffer"));
  gtk_text_buffer_get_iter_at_offset (buff, &start, 0);
  gtk_text_buffer_get_iter_at_offset (buff, &end, -1);
  gtk_text_buffer_remove_all_tags (buff, &start, &end);
  if (noeud1 == NULL)
    gtk_text_buffer_apply_tag_by_name (buff, "mauvais", &start, &end);
  else
    gtk_text_buffer_apply_tag_by_name (buff, "OK", &start, &end);
  noeud2 = EF_noeuds_cherche_numero (
             p,
             conv_buff_u (GTK_TEXT_BUFFER (gtk_builder_get_object (
                        UI_BARADD.builder, "EF_gtk_barres_add_noeud2_buffer")),
             0,
             TRUE,
             UINT_MAX,
             FALSE),
           FALSE);
  buff = GTK_TEXT_BUFFER (gtk_builder_get_object (UI_BARADD.builder,
                                           "EF_gtk_barres_add_noeud2_buffer"));
  gtk_text_buffer_get_iter_at_offset (buff, &start, 0);
  gtk_text_buffer_get_iter_at_offset (buff, &end, -1);
  gtk_text_buffer_remove_all_tags (buff, &start, &end);
  if (noeud2 == NULL)
    gtk_text_buffer_apply_tag_by_name (buff, "mauvais", &start, &end);
  else
    gtk_text_buffer_apply_tag_by_name (buff, "OK", &start, &end);
  
  if ((gtk_combo_box_get_active (GTK_COMBO_BOX (gtk_builder_get_object (
              UI_BARADD.builder, "EF_gtk_barres_add_type_combobox"))) != -1) &&
      (gtk_combo_box_get_active (GTK_COMBO_BOX (gtk_builder_get_object (
           UI_BARADD.builder, "EF_gtk_barres_add_section_combobox"))) != -1) &&
      (gtk_combo_box_get_active (GTK_COMBO_BOX (gtk_builder_get_object (
          UI_BARADD.builder, "EF_gtk_barres_add_materiau_combobox"))) != -1) &&
      (gtk_combo_box_get_active (GTK_COMBO_BOX (gtk_builder_get_object (
       UI_BARADD.builder, "EF_gtk_barres_add_relachement_combobox"))) != -1) &&
      (noeud1 != NULL) &&
      (noeud2 != NULL) &&
      (noeud1 != noeud2))
    ok = TRUE;
  
  if (conv_buff_u (GTK_TEXT_BUFFER (gtk_builder_get_object (UI_BARADD.builder,
                          "EF_gtk_barres_add_nb_noeuds_intermediaire_buffer")),
                   0,
                   TRUE,
                   UINT_MAX,
                   FALSE) == UINT_MAX)
    ok = FALSE;
  if (isnan (conv_buff_d (GTK_TEXT_BUFFER (gtk_builder_get_object (
                         UI_BARADD.builder, "EF_gtk_barres_add_angle_buffer")),
             -360.,
             FALSE,
             360.,
             FALSE)))
    ok = FALSE;
  
  gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
                           UI_BARADD.builder, "EF_gtk_barres_add_button_add")),
                            ok);
  
  return;
}


void
EF_gtk_barres_ajouter (GtkButton *button,
                       Projet    *p)
/**
 * \brief Création de la fenêtre permettant d'ajouter des barres.
 * \param button : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique impossible à générer.
 */
{
  BUGMSG (p, , gettext ("Paramètre %s incorrect.\n"), "projet")
  if (UI_BARADD.builder != NULL)
  {
    gtk_window_present (GTK_WINDOW (UI_BARADD.window));
    return;
  }
  
  UI_BARADD.builder = gtk_builder_new ();
  BUGMSG (gtk_builder_add_from_resource (UI_BARADD.builder,
                                       "/org/2lgc/codegui/ui/EF_barres_add.ui",
                                         NULL) != 0,
          ,
          gettext ("Builder Failed\n"))
  gtk_builder_connect_signals (UI_BARADD.builder, p);
  
  UI_BARADD.window = GTK_WIDGET (gtk_builder_get_object (UI_BARADD.builder,
                                                      "EF_barres_add_window"));
  
  g_object_set (gtk_builder_get_object (UI_BARADD.builder,
                                        "EF_gtk_barres_add_type_combobox"),
                "model", UI_BAR.liste_types,
                NULL);
  g_object_set (gtk_builder_get_object (UI_BARADD.builder,
                                        "EF_gtk_barres_add_section_combobox"),
                "model", UI_SEC.liste_sections,
                NULL);
  g_object_set (gtk_builder_get_object (UI_BARADD.builder,
                                        "EF_gtk_barres_add_materiau_combobox"),
                "model", UI_MAT.liste_materiaux,
                NULL);
  g_object_set (gtk_builder_get_object (UI_BARADD.builder,
                                     "EF_gtk_barres_add_relachement_combobox"),
                "model", UI_REL.liste_relachements,
                NULL);
  
  if (p->modele.barres != NULL)
  {
    char *nb_barres;
    
    BUGMSG (nb_barres = g_strdup_printf ("%d",
              ((EF_Barre *) g_list_last (p->modele.barres)->data)->numero + 1),
            ,
            gettext ("Erreur d'allocation mémoire.\n"))
    gtk_label_set_text (GTK_LABEL (gtk_builder_get_object (UI_BARADD.builder,
                                           "EF_gtk_barres_add_numero_label2")),
                        nb_barres);
    free (nb_barres);
  }
  else
    gtk_label_set_text (GTK_LABEL (gtk_builder_get_object (UI_BARADD.builder,
                                           "EF_gtk_barres_add_numero_label2")),
                        "0");
  
  gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
                           UI_BARADD.builder, "EF_gtk_barres_add_button_add")),
                            FALSE);
  
  gtk_window_set_transient_for (GTK_WINDOW (UI_BARADD.window),
                                GTK_WINDOW (UI_GTK.window));
}

#endif

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
