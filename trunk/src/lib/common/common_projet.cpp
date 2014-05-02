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
#include <libintl.h>
#include <locale.h>
#include <gmodule.h>
#include <cholmod.h>
#include <string.h>

#include "common_erreurs.hpp"
#include "common_math.hpp"

#ifdef ENABLE_GTK
#include "common_m3d.hpp"
#include "common_gtk.hpp"
#include "1990_gtk_groupes.hpp"
#include "1990_gtk_actions.hpp"
#include "EF_gtk_noeud.hpp"
#include "EF_gtk_appuis.hpp"
#include "EF_gtk_calculs.hpp"
#include "EF_gtk_sections.hpp"
#include "EF_gtk_barres.hpp"
#include "EF_gtk_materiaux.hpp"
#include "EF_gtk_relachement.hpp"
#include "EF_gtk_resultats.hpp"
#include "ressources.h"
#include "common_gtk_informations.hpp"
#endif

#include "common_ville.hpp"
#include "EF_appuis.hpp"
#include "EF_materiaux.hpp"
#include "EF_noeuds.hpp"
#include "EF_rigidite.hpp"
#include "EF_relachement.hpp"
#include "EF_sections.hpp"
#include "EF_calculs.hpp"
#include "1990_action.hpp"
#include "1990_action_private.hpp"
#include "1990_groupe.hpp"
#include "1990_combinaisons.hpp"
#include "1992_1_1_barres.hpp"
#include "1992_1_1_materiaux.hpp"


/**
 * \brief Libère les allocations mémoires de l'ensemble de la variable projet.
 * \param p : la variable projet.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL.
 */
gboolean
projet_free (Projet *p)
{
  // Action doit être libéré avant p->modele.barres
  
  BUGPARAM (p, "%p", p, FALSE)
  
  BUG (common_ville_free (p), FALSE)
  if (p->actions != NULL)
  {
    BUG (_1990_action_free (p), FALSE)
  }
  if (p->niveaux_groupes != NULL)
  {
    BUG (_1990_groupe_free (p), FALSE)
  }
  if (p->combinaisons.elu_equ != NULL)
  {
    BUG (_1990_combinaisons_free (p), FALSE)
  }
  // Rigidite doit être libéré avant noeud car pour libérer toute la mémoire,
  // il est nécessaire d'avoir accès aux informations contenues dans les
  // noeuds.
  BUG (EF_calculs_free (p), FALSE)
  if (p->modele.sections != NULL)
  {
    BUG (EF_sections_free (p), FALSE)
  }
  if (p->modele.noeuds != NULL)
  {
    BUG (EF_noeuds_free (p), FALSE)
  }
  if (p->modele.barres != NULL)
  {
    BUG (_1992_1_1_barres_free (p), FALSE)
  }
  if (p->modele.appuis != NULL)
  {
    BUG (EF_appuis_free (p), FALSE)
  }
  if (p->modele.materiaux != NULL)
  {
    BUG (EF_materiaux_free (p), FALSE)
  }
  if (p->modele.relachements != NULL)
  {
    BUG (EF_relachement_free (p), FALSE)
  }
#ifdef ENABLE_GTK
  if (UI_M3D.data != NULL)
  {
    BUG (m3d_free (p), FALSE)
  }
  EF_gtk_resultats_free (p);
  
  _2lgc_unregister_resource ();
#endif
  
  cholmod_finish (p->calculs.c);
  
  free (p);
  
  return TRUE;
}


/**
 * \brief Initialise la variable projet.
 * \param norme : norme de calcul.
 * \return
 *   Succès : Un pointeur vers une zone mémoire projet.
 *   Échec : NULL :
 *     - en cas d'erreur d'allocation mémoire.
 */
// coverity[+alloc]
Projet *
projet_init (Norme norme)
{
  Projet         *p;
#ifdef ENABLE_GTK
  GtkCssProvider *provider = gtk_css_provider_new ();
  GdkDisplay     *display;
  GdkScreen      *screen;
  
  _2lgc_register_resource ();
#endif
  
  // Alloue toutes les zones mémoires du projet à savoir (par module) :
  BUGCRIT (p = (Projet *) malloc (sizeof (Projet)),
           NULL,
           (gettext ("Erreur d'allocation mémoire.\n")); )
  memset (p, 0, sizeof (Projet));
  
  p->parametres.norme = norme;
  
  BUG (common_ville_init (p), NULL, free (p); )
  
  NOWARNING
  //   - 1990 : la liste des actions, des groupes et des combinaisons,
  BUG (_1990_action_init (p), NULL, projet_free (p); )
  BUG (_1990_groupe_init (p), NULL, projet_free (p); )
  BUG (_1990_combinaisons_init (p), NULL, projet_free (p); )
  //   - 1992-1-1 : la liste des sections, des barres et des matériaux
  BUG (EF_sections_init (p), NULL, projet_free (p); )
  BUG (_1992_1_1_barres_init (p), NULL, projet_free (p); )
  BUG (EF_materiaux_init (p), NULL, projet_free (p); )
  //   - EF : la liste des appuis, des relâchements et des noeuds ainsi que les
  //          éléments nécessaire pour les calculs aux éléments finis.
  BUG (EF_appuis_init (p), NULL, projet_free (p); )
  BUG (EF_rigidite_init (p), NULL, projet_free (p); )
  BUG (EF_relachement_init (p), NULL, projet_free (p); )
  BUG (EF_noeuds_init (p), NULL, projet_free (p); )
  
#ifdef ENABLE_GTK
  BUG (m3d_init (p), NULL, projet_free (p); )
  UI_GRO.builder = NULL;
  UI_GROOP.builder = NULL;
  UI_CHNO.builder = NULL;
  UI_CHBARP.builder = NULL;
  UI_CHBARR.builder = NULL;
  UI_INFO.builder = NULL;
  UI_NOE.builder = NULL;
  UI_BAR.builder = NULL;
  UI_BARADD.builder = NULL;
  UI_APP.builder = NULL;
  UI_SEC.builder = NULL;
  UI_SEC_RE.builder = NULL;
  UI_SEC_T.builder = NULL;
  UI_SEC_CA.builder = NULL;
  UI_SEC_CI.builder = NULL;
  UI_SEC_PE.builder = NULL;
  UI_MAT.builder = NULL;
  UI_BET.builder = NULL;
  UI_ACI.builder = NULL;
  UI_REL.builder = NULL;
  UI_RAP.builder = NULL;
  UI_RES.builder = NULL;
  UI_RES.tableaux = new std::list <Gtk_EF_Resultats_Tableau *> ();
  gtk_css_provider_load_from_data (provider,
     "GtkPaned GtkToolbar {\n"
     "  background-image: -gtk-gradient (linear,"
     "                    left top, left bottom,"
     "                    from (#a7aba7), to (#ededed));\n"
     "  border-color: #a7aba7;\n"
     "  border-width: 1px;"
     "  border-style: solid;"
     "}\n", -1, NULL);
  display = gdk_display_get_default ();
  screen = gdk_display_get_default_screen (display);
  gtk_style_context_add_provider_for_screen (screen,
                                             GTK_STYLE_PROVIDER (provider),
                                      GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
  g_object_unref (provider);
#endif
  
  BUG (common_ville_set (p, L"37", L"Joué-lès-Tours", FALSE),
       NULL,
       projet_free (p); )
  POPWARNING
  
  p->calculs.c = &(p->calculs.Common);
  cholmod_start (p->calculs.c);
  
  return p;
}


#ifdef ENABLE_GTK
/**
 * \brief Évenement lors de la fermeture de la fenêtre principale.
 * \param pWidget : composant à l'origine de la demande,
 * \param p : la variable projet.
 * \return Rien.
 *   Échec :
 *     - p == NULL.
 */
void
gui_window_destroy_event (GtkWidget *pWidget,
                          Projet    *p)
{
  BUGPARAM (p, "%p", p, )
  
  BUG (projet_free (p), )
  gtk_widget_destroy (pWidget);
  gtk_main_quit ();
  
  return;
}


/**
 * \brief Bouton de fermeture de la fenêtre.
 * \param fenetre : composant à détruire,
 * \param fenetre : la fenêtre d'options.
 * \return Rien.
 */
void
gui_window_option_destroy_button (GtkWidget *fenetre)
{
  gtk_widget_destroy (fenetre);
  
  return;
}


/**
 * \brief Crée une fenêtre graphique avec toute l'interface (menu, vue 3D,
 *        ...).
 * \param p : variable projet.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL.
 */
gboolean
projet_init_graphique (Projet *p)
{
  GtkWidget *menu_separator;
  
  BUGPARAM (p, "%p", p, FALSE)
  
  UI_GTK.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_resize (GTK_WINDOW (UI_GTK.window), 800, 600);
  gtk_window_set_position (GTK_WINDOW (UI_GTK.window), GTK_WIN_POS_CENTER);
  g_signal_connect (GTK_WINDOW (UI_GTK.window),
                    "destroy",
                    G_CALLBACK (gui_window_destroy_event),
                    p);
  UI_GTK.main_grid = gtk_grid_new ();
  gtk_container_add (GTK_CONTAINER (UI_GTK.window),
                     GTK_WIDGET (UI_GTK.main_grid));
  
  gtk_grid_attach (GTK_GRID (UI_GTK.main_grid), UI_M3D.drawing, 0, 1, 1, 1);
  gtk_widget_set_hexpand (UI_M3D.drawing, TRUE);
  gtk_widget_set_vexpand (UI_M3D.drawing, TRUE);
  g_signal_connect (GTK_WINDOW (UI_GTK.window),
                    "key-press-event",
                    G_CALLBACK (m3d_key_press), p);
  
  UI_GTK.menu = gtk_menu_bar_new ();
  gtk_grid_attach (GTK_GRID (UI_GTK.main_grid), UI_GTK.menu, 0, 0, 1, 1);
  gtk_widget_set_hexpand (UI_GTK.menu, TRUE);

  UI_GTK.menu_fichier_list = gtk_menu_new ();
  UI_GTK.menu_fichier = gtk_menu_item_new_with_label (gettext ("Fichier"));
  gtk_menu_shell_append (GTK_MENU_SHELL (UI_GTK.menu), UI_GTK.menu_fichier);
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (UI_GTK.menu_fichier),
                             UI_GTK.menu_fichier_list);
  
  UI_GTK.menu_fichier_informations = gtk_menu_item_new_with_label (
                                                     gettext ("Informations"));
  gtk_menu_shell_append (GTK_MENU_SHELL (UI_GTK.menu_fichier_list),
                         UI_GTK.menu_fichier_informations);
  g_signal_connect_swapped (UI_GTK.menu_fichier_informations,
                            "activate",
                            G_CALLBACK (common_gtk_informations),
                            p);
  
  UI_GTK.menu_fichier_quitter = gtk_menu_item_new_with_label (
                                                          gettext ("Quitter"));
  gtk_menu_shell_append (GTK_MENU_SHELL (UI_GTK.menu_fichier_list),
                         UI_GTK.menu_fichier_quitter);
  g_signal_connect_swapped (UI_GTK.menu_fichier_quitter,
                            "activate",
                            G_CALLBACK (gui_window_option_destroy_button),
                            UI_GTK.window);
  
  UI_GTK.menu_affichage_list = gtk_menu_new ();
  UI_GTK.menu_affichage = gtk_menu_item_new_with_label (gettext ("Affichage"));
  gtk_menu_shell_append (GTK_MENU_SHELL (UI_GTK.menu), UI_GTK.menu_affichage);
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (UI_GTK.menu_affichage),
                             UI_GTK.menu_affichage_list);
  
  UI_GTK.menu_affichage_vues_list = gtk_menu_new ();
  UI_GTK.menu_affichage_vues = gtk_menu_item_new_with_label (gettext ("Vues"));
  gtk_menu_shell_append (GTK_MENU_SHELL (UI_GTK.menu_affichage_list),
                         UI_GTK.menu_affichage_vues);
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (UI_GTK.menu_affichage_vues),
                             UI_GTK.menu_affichage_vues_list);
  
  UI_GTK.menu_affichage_xzy = gtk_menu_item_new_with_label (
                                                       gettext ("XZ vers Y+"));
  gtk_menu_shell_append (GTK_MENU_SHELL (UI_GTK.menu_affichage_vues_list),
                         UI_GTK.menu_affichage_xzy);
  g_signal_connect_swapped (UI_GTK.menu_affichage_xzy,
                            "activate",
                            G_CALLBACK (m3d_camera_axe_x_z_y),
                            p);
  
  UI_GTK.menu_affichage_xz_y = gtk_menu_item_new_with_label (
                                                       gettext ("XZ vers Y-"));
  gtk_menu_shell_append (GTK_MENU_SHELL (UI_GTK.menu_affichage_vues_list),
                         UI_GTK.menu_affichage_xz_y);
  g_signal_connect_swapped (UI_GTK.menu_affichage_xz_y,
                            "activate",
                            G_CALLBACK (m3d_camera_axe_x_z__y),
                            p);
  
  UI_GTK.menu_affichage_yzx = gtk_menu_item_new_with_label (
                                                       gettext ("YZ vers X+"));
  gtk_menu_shell_append (GTK_MENU_SHELL (UI_GTK.menu_affichage_vues_list),
                         UI_GTK.menu_affichage_yzx);
  g_signal_connect_swapped (UI_GTK.menu_affichage_yzx,
                            "activate",
                            G_CALLBACK (m3d_camera_axe_y_z_x),
                            p);
  
  UI_GTK.menu_affichage_yz_x = gtk_menu_item_new_with_label (
                                                       gettext ("YZ vers X-"));
  gtk_menu_shell_append (GTK_MENU_SHELL (UI_GTK.menu_affichage_vues_list),
                         UI_GTK.menu_affichage_yz_x);
  g_signal_connect_swapped (UI_GTK.menu_affichage_yz_x,
                            "activate",
                            G_CALLBACK (m3d_camera_axe_y_z__x),
                            p);
  
  UI_GTK.menu_affichage_xyz = gtk_menu_item_new_with_label (
                                                       gettext ("XY vers Z+"));
  gtk_menu_shell_append (GTK_MENU_SHELL (UI_GTK.menu_affichage_vues_list),
                         UI_GTK.menu_affichage_xyz);
  g_signal_connect_swapped (UI_GTK.menu_affichage_xyz,
                            "activate",
                            G_CALLBACK (m3d_camera_axe_x_y_z),
                            p);
  
  UI_GTK.menu_affichage_xy_z = gtk_menu_item_new_with_label (
                                                       gettext ("XY vers Z-"));
  gtk_menu_shell_append (GTK_MENU_SHELL (UI_GTK.menu_affichage_vues_list),
                         UI_GTK.menu_affichage_xy_z);
  g_signal_connect_swapped (UI_GTK.menu_affichage_xy_z,
                            "activate",
                            G_CALLBACK (m3d_camera_axe_x_y__z),
                            p);
  
  UI_GTK.menu_modelisation_list = gtk_menu_new ();
  UI_GTK.menu_modelisation = gtk_menu_item_new_with_label (
                                                     gettext ("Modélisation"));
  gtk_menu_shell_append (GTK_MENU_SHELL (UI_GTK.menu),
                         UI_GTK.menu_modelisation);
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (UI_GTK.menu_modelisation),
                             UI_GTK.menu_modelisation_list);
  
  UI_GTK.menu_modelisation_noeud_list = gtk_menu_new ();
  UI_GTK.menu_modelisation_noeud = gtk_menu_item_new_with_label (
                                                           gettext ("Noeuds"));
  gtk_menu_shell_append (GTK_MENU_SHELL (UI_GTK.menu_modelisation_list),
                         UI_GTK.menu_modelisation_noeud);
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (UI_GTK.menu_modelisation_noeud),
                             UI_GTK.menu_modelisation_noeud_list);
  
  UI_GTK.menu_modelisation_noeud_appui = gtk_menu_item_new_with_label
                                                       (gettext ("Appuis..."));
  gtk_menu_shell_append (GTK_MENU_SHELL (UI_GTK.menu_modelisation_noeud_list),
                         UI_GTK.menu_modelisation_noeud_appui);
  g_signal_connect_swapped (UI_GTK.menu_modelisation_noeud_appui,
                            "activate",
                            G_CALLBACK (EF_gtk_appuis),
                            p);
  
  UI_GTK.menu_modelisation_noeud_ajout = gtk_menu_item_new_with_label
                                           (gettext ("Gestion des noeuds..."));
  gtk_menu_shell_append (GTK_MENU_SHELL (UI_GTK.menu_modelisation_noeud_list),
                         UI_GTK.menu_modelisation_noeud_ajout);
  g_signal_connect_swapped (UI_GTK.menu_modelisation_noeud_ajout,
                            "activate",
                            G_CALLBACK (EF_gtk_noeud),
                            p);
  
  UI_GTK.menu_modelisation_barres_list = gtk_menu_new ();
  UI_GTK.menu_modelisation_barres = gtk_menu_item_new_with_label
                                                          (gettext ("Barres"));
  gtk_menu_shell_append (GTK_MENU_SHELL (UI_GTK.menu_modelisation_list),
                         UI_GTK.menu_modelisation_barres);
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (UI_GTK.menu_modelisation_barres),
                             UI_GTK.menu_modelisation_barres_list);
  
  UI_GTK.menu_modelisation_barres_section = gtk_menu_item_new_with_label (
                                                      gettext ("Sections..."));
  gtk_menu_shell_append (GTK_MENU_SHELL (UI_GTK.menu_modelisation_barres_list),
                         UI_GTK.menu_modelisation_barres_section);
  g_signal_connect_swapped (UI_GTK.menu_modelisation_barres_section,
                            "activate",
                            G_CALLBACK (EF_gtk_sections),
                            p);
  
  UI_GTK.menu_modelisation_barres_materiau = gtk_menu_item_new_with_label (
                                                     gettext ("Matériaux..."));
  gtk_menu_shell_append (GTK_MENU_SHELL (UI_GTK.menu_modelisation_barres_list),
                         UI_GTK.menu_modelisation_barres_materiau);
  g_signal_connect_swapped (UI_GTK.menu_modelisation_barres_materiau,
                            "activate",
                            G_CALLBACK (EF_gtk_materiaux),
                            p);
  
  UI_GTK.menu_modelisation_barres_relachement = gtk_menu_item_new_with_label (
                                                  gettext ("Relâchements..."));
  gtk_menu_shell_append (GTK_MENU_SHELL (UI_GTK.menu_modelisation_barres_list),
                         UI_GTK.menu_modelisation_barres_relachement);
  g_signal_connect_swapped (UI_GTK.menu_modelisation_barres_relachement,
                            "activate",
                            G_CALLBACK (EF_gtk_relachement),
                            p);
  
  UI_GTK.menu_modelisation_barres_ajout = gtk_menu_item_new_with_label (
                                            gettext ("Gestion des barres..."));
  gtk_menu_shell_append (GTK_MENU_SHELL (UI_GTK.menu_modelisation_barres_list),
                         UI_GTK.menu_modelisation_barres_ajout);
  g_signal_connect_swapped (UI_GTK.menu_modelisation_barres_ajout,
                            "activate",
                            G_CALLBACK (EF_gtk_barres),
                            p);
  
  menu_separator = gtk_separator_menu_item_new ();
  gtk_menu_shell_append (GTK_MENU_SHELL (UI_GTK.menu_modelisation_list),
                         menu_separator);
  
  UI_GTK.menu_modelisation_charges_list = gtk_menu_new ();
  UI_GTK.menu_modelisation_charges = gtk_menu_item_new_with_label (
                                                          gettext ("Charges"));
  gtk_menu_shell_append (GTK_MENU_SHELL (UI_GTK.menu_modelisation_list),
                         UI_GTK.menu_modelisation_charges);
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (UI_GTK.menu_modelisation_charges),
                             UI_GTK.menu_modelisation_charges_list);
  
  UI_GTK.menu_modelisation_charges_actions = gtk_menu_item_new_with_label (
                                                       gettext ("Actions..."));
  gtk_menu_shell_append (GTK_MENU_SHELL (
                                        UI_GTK.menu_modelisation_charges_list),
                         UI_GTK.menu_modelisation_charges_actions);
  g_signal_connect_swapped (UI_GTK.menu_modelisation_charges_actions,
                            "activate",
                            G_CALLBACK (_1990_gtk_actions),
                            p);
  
  UI_GTK.menu_modelisation_charges_groupes = gtk_menu_item_new_with_label (
                                                       gettext ("Groupes..."));
  gtk_menu_shell_append (GTK_MENU_SHELL (
                                        UI_GTK.menu_modelisation_charges_list),
                         UI_GTK.menu_modelisation_charges_groupes);
  g_signal_connect_swapped (UI_GTK.menu_modelisation_charges_groupes,
                            "activate",
                            G_CALLBACK (_1990_gtk_groupes),
                            p);
  
  UI_GTK.menu_resultats_list = gtk_menu_new ();
  UI_GTK.menu_resultats = gtk_menu_item_new_with_label (gettext ("Résultats"));
  gtk_menu_shell_append (GTK_MENU_SHELL (UI_GTK.menu), UI_GTK.menu_resultats);
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (UI_GTK.menu_resultats),
                             UI_GTK.menu_resultats_list);
  
  UI_GTK.menu_resultats_calculer = gtk_menu_item_new_with_label (
                                                         gettext ("Calculer"));
  gtk_menu_shell_append (GTK_MENU_SHELL (UI_GTK.menu_resultats_list),
                         UI_GTK.menu_resultats_calculer);
  g_signal_connect (UI_GTK.menu_resultats_calculer,
                    "activate",
                    G_CALLBACK (EF_gtk_calculs_calculer),
                    p);
  
  UI_GTK.menu_resultats_afficher = gtk_menu_item_new_with_label (
                                                         gettext ("Afficher"));
  gtk_menu_shell_append (GTK_MENU_SHELL (UI_GTK.menu_resultats_list),
                         UI_GTK.menu_resultats_afficher);
  g_signal_connect_swapped (UI_GTK.menu_resultats_afficher,
                            "activate",
                            G_CALLBACK (EF_gtk_resultats),
                            p);
  
  UI_ACT.window = NULL;
  UI_GRO.window = NULL;
  
  return TRUE;
}
#endif

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
