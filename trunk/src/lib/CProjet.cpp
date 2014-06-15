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

#include <iostream>
#include <locale>

#include "CProjet.hpp"

/**
 * \brief Initialise la classe CProjet.
 */
CProjet::CProjet () :
  parametres (NULL),
  modele (),
  calculs (),
  undo ()
{
#if 0
#ifdef ENABLE_GTK
  GtkCssProvider *provider = gtk_css_provider_new ();
  GdkDisplay     *display;
  GdkScreen      *screen;
  
  _2lgc_register_resource ();
#endif
  
  BUG (common_ville_init (p), NULL, delete p; )
  
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
  
  BUG (common_ville_set (p, L"37", L"Joué-lès-Tours", false),
       NULL,
       projet_free (p); )
  POPWARNING
  
#endif
}


/**
 * \brief Duplication d'une classe CProjet.
 * \param other (in) La classe à dupliquer.
 */
CProjet::CProjet (const CProjet & other) :
  parametres (other.parametres),
  modele (other.modele),
  calculs (other.calculs),
  undo (other.undo)
{
}


/**
 * \brief Assignment operator de CProjet.
 * \param other (in) La classe à dupliquer.
 */
CProjet &
CProjet::operator = (const CProjet & other)
{
  this->parametres = other.parametres;
  this->modele = other.modele;
  this->calculs = other.calculs;
  this->undo = other.undo;
  
  return *this;
}


/**
 * \brief Libère une classe CProjet avec tout le contenu.
 */
CProjet::~CProjet ()
{
  delete parametres;
  
#if 0
  BUG (EF_calculs_free (p), false)
  BUG (common_ville_free (p), false)
  BUG (_1990_action_free (p), false)
  BUG (_1990_groupe_free (p), false)
  BUG (_1990_combinaisons_free (p), false)
  // Rigidite doit être libéré avant noeud car pour libérer toute la mémoire,
  // il est nécessaire d'avoir accès aux informations contenues dans les
  // noeuds.
  BUG (EF_calculs_free (p), false)
  BUG (EF_sections_free (p), false)
  BUG (EF_noeuds_free (p), false)
  BUG (_1992_1_1_barres_free (p), false)
  BUG (EF_appuis_free (p), false)
  BUG (EF_materiaux_free (p), false)
  BUG (EF_relachement_free (p), false)
#ifdef ENABLE_GTK
  UI_RES.tableaux.clear ();
  if (UI_M3D.data != NULL)
  {
    BUG (m3d_free (p), false)
  }
  EF_gtk_resultats_free (p);
  
  _2lgc_unregister_resource ();
#endif
#endif
}


/**
 * \brief Renvoie les paramètres du projet.
 */
IParametres &
CProjet::getParametres ()
{
  return *parametres;
}


/**
 * \brief Renvoie le modèle de calcul.
 */
CModele &
CProjet::getModele ()
{
  return modele;
}


/**
 * \brief Renvoie les résultats des calculs.
 */
CCalculs &
CProjet::getCalculs ()
{
  return calculs;
}


/**
 * \brief Affiche les limites de la garantie (articles 15, 16 et 17 de la
 *        licence GPL).
 * \return Rien.
 */
void
CProjet::showWarranty ()
{
  std::cout << gettext ("15. Disclaimer of Warranty.\n")
    << gettext ("\n")
    << gettext ("THERE IS NO WARRANTY FOR THE PROGRAM, TO THE EXTENT PERMITTED BY\n")
    << gettext ("APPLICABLE LAW.  EXCEPT WHEN OTHERWISE STATED IN WRITING THE COPYRIGHT\n")
    << gettext ("HOLDERS AND/OR OTHER PARTIES PROVIDE THE PROGRAM \"AS IS\" WITHOUT WARRANTY\n")
    << gettext ("OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO,\n")
    << gettext ("THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR\n")
    << gettext ("PURPOSE.  THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE PROGRAM\n")
    << gettext ("IS WITH YOU.  SHOULD THE PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF\n")
    << gettext ("ALL NECESSARY SERVICING, REPAIR OR CORRECTION.\n")
    << gettext ("\n")
    << gettext ("16. Limitation of Liability.\n")
    << gettext ("\n")
    << gettext ("IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN WRITING\n")
    << gettext ("WILL ANY COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO MODIFIES AND/OR CONVEYS\n")
    << gettext ("THE PROGRAM AS PERMITTED ABOVE, BE LIABLE TO YOU FOR DAMAGES, INCLUDING ANY\n")
    << gettext ("GENERAL, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE\n")
    << gettext ("USE OR INABILITY TO USE THE PROGRAM (INCLUDING BUT NOT LIMITED TO LOSS OF\n")
    << gettext ("DATA OR DATA BEING RENDERED INACCURATE OR LOSSES SUSTAINED BY YOU OR THIRD\n")
    << gettext ("PARTIES OR A FAILURE OF THE PROGRAM TO OPERATE WITH ANY OTHER PROGRAMS),\n")
    << gettext ("EVEN IF SUCH HOLDER OR OTHER PARTY HAS BEEN ADVISED OF THE POSSIBILITY OF\n")
    << gettext ("SUCH DAMAGES.\n")
    << gettext ("\n")
    << gettext ("17. Interpretation of Sections 15 and 16.\n")
    << gettext ("\n")
    << gettext ("If the disclaimer of warranty and limitation of liability provided\n")
    << gettext ("above cannot be given local legal effect according to their terms,\n")
    << gettext ("reviewing courts shall apply local law that most closely approximates\n")
    << gettext ("an absolute waiver of all civil liability in connection with the\n")
    << gettext ("Program, unless a warranty or assumption of liability accompanies a\n")
    << gettext ("copy of the Program in return for a fee.\n");
  
  return;
}

/**
 * \brief Affiche l'aide lorsque l'utilisateur lance le programme avec l'option
 *        -h.
 * \return Rien.
 */
void
CProjet::showHelp ()
{
  std::cout << gettext ("Utilisation : codegui [OPTION]... [FILE]...\n")
    << gettext ("Options :\n")
    << gettext ("\t-h, --help : affiche le présent menu\n")
    << gettext ("\t-w, --warranty : affiche les limites de garantie du logiciel\n");
  
  return;
}


#if 0
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
 *   Succès : true.\n
 *   Échec : false :
 *     - p == NULL.
 */
bool
projet_init_graphique (Projet *p)
{
  GtkWidget *menu_separator;
  
  BUGPARAM (p, "%p", p, false)
  
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
  
  return true;
}
#endif

#endif

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
