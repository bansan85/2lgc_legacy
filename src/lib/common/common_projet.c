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

#include "common_projet.h"
#include "common_erreurs.h"
#include "common_math.h"

#ifdef ENABLE_GTK
#include "common_m3d.hpp"
#include "1990_gtk_groupes.h"
#include "1990_gtk_actions.h"
#include "EF_gtk_noeud.h"
#include "EF_gtk_appuis.h"
#include "EF_gtk_calculs.h"
#include "EF_gtk_sections.h"
#include "EF_gtk_barres.h"
#include "EF_gtk_materiaux.h"
#include "EF_gtk_relachement.h"
#include "EF_gtk_resultats.h"
#endif

#include "ressources.h"
#include "common_ville.h"
#include "common_gtk_informations.h"
#include "EF_appuis.h"
#include "EF_noeuds.h"
#include "EF_rigidite.h"
#include "EF_relachement.h"
#include "EF_sections.h"
#include "EF_calculs.h"
#include "1990_action.h"
#include "1990_groupe.h"
#include "1990_combinaisons.h"
#include "1992_1_1_barres.h"
#include "1992_1_1_materiaux.h"

Projet* projet_init(Type_Pays pays)
/* Description : Initialise la variable projet.
 * Paramètres : Type_Pays pays : pays du calcul.
 * Valeur renvoyée :
 *   Succès : Un pointeur vers une zone mémoire projet.
 *   Échec : NULL :
 *             en cas d'erreur d'allocation mémoire.
 */
{
    Projet      *projet;
    
    _2lgc_register_resource();
    
    // Alloue toutes les zones mémoires du projet à savoir (par module) :
    BUGMSG(projet = (Projet*)malloc(sizeof(Projet)), NULL, gettext("Erreur d'allocation mémoire.\n"));
    
    projet->parametres.pays = pays;
    
    BUG(common_ville_init(projet), NULL);
    
    //     - 1990 : la liste des actions, des groupes et des combinaisons,
    BUG(_1990_action_init(projet), NULL);
    BUG(_1990_groupe_init(projet), NULL);
    BUG(_1990_combinaisons_init(projet), NULL);
    //     - 1992-1-1 : la liste des sections, des barres et des matériaux
    BUG(EF_sections_init(projet), NULL);
    BUG(_1992_1_1_barres_init(projet), NULL);
    BUG(_1992_1_1_materiaux_init(projet), NULL);
    //     - EF : la liste des appuis, des relâchements et des noeuds ainsi que les éléments
    //              nécessaire pour les calculs aux éléments finis.
    BUG(EF_appuis_init(projet), NULL);
    BUG(EF_rigidite_init(projet), NULL);
    BUG(EF_relachement_init(projet), NULL);
    BUG(EF_noeuds_init(projet), NULL);
#ifdef ENABLE_GTK
    BUG(m3d_init(projet), NULL);
    projet->list_gtk._1990_groupes.builder = NULL;
    projet->list_gtk._1990_groupes.builder_options = NULL;
    projet->list_gtk.ef_charge_noeud.builder = NULL;
    projet->list_gtk.ef_charge_barre_ponctuelle.builder = NULL;
    projet->list_gtk.ef_charge_barre_repartie_uniforme.builder = NULL;
    projet->list_gtk.common_informations.builder = NULL;
    projet->list_gtk.ef_noeud.builder = NULL;
    projet->list_gtk.ef_barres.builder = NULL;
    projet->list_gtk.ef_barres.builder_add = NULL;
    projet->list_gtk.ef_appuis.builder = NULL;
    projet->list_gtk.ef_sections.builder = NULL;
    projet->list_gtk.ef_sections_rectangulaire.builder = NULL;
    projet->list_gtk.ef_sections_T.builder = NULL;
    projet->list_gtk.ef_sections_carree.builder = NULL;
    projet->list_gtk.ef_sections_circulaire.builder = NULL;
    projet->list_gtk.ef_materiaux.builder = NULL;
    projet->list_gtk._1992_1_1_materiaux.builder = NULL;
    projet->list_gtk.ef_relachements.builder = NULL;
    projet->list_gtk.ef_rapport.builder = NULL;
    projet->list_gtk.ef_resultats.builder = NULL;
    projet->list_gtk.ef_resultats.tableaux = NULL;
#endif
    
    BUG(common_ville_set(projet, "30", 59, FALSE), NULL);
    
    projet->calculs.c = &(projet->calculs.Common);
    cholmod_start(projet->calculs.c);
    
    return projet;
}


#ifdef ENABLE_GTK
void gui_window_destroy_event(GtkWidget *pWidget, Projet *projet)
/* Description : Evènement lors de la fermeture de la fenêtre principale.
 * Paramètres : GtkWidget *pWidget : composant à l'origine de la demande,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    
    BUG(projet_free(projet), );
    gtk_widget_destroy(pWidget);
    gtk_main_quit();
    
    return;
}


void gui_window_option_destroy_button(GtkWidget *fenetre)
/* Description : Bouton de fermeture de la fenêtre.
 * Paramètres : GtkWidget *object : composant à l'origine de la demande,
 *            : GtkWidget *fenetre : la fenêtre d'options.
 * Valeur renvoyée : Aucune.
 */
{
    gtk_widget_destroy(fenetre);
    
    return;
}


gboolean projet_init_graphique(Projet *projet)
/* Description : Crée une fenêtre graphique avec toute l'interface (menu, vue 3D, ...).
 * Paramètres : Projet *projet : variable projet.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL.
 */
{
    Comp_Gtk    *comps;
    Gtk_m3d     *m3d;
    GtkWidget   *menu_separator;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    
    comps = &(projet->list_gtk.comp);
    
    comps->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_resize(GTK_WINDOW(comps->window), 800, 600);
    gtk_window_set_position(GTK_WINDOW(comps->window), GTK_WIN_POS_CENTER);
    g_signal_connect(GTK_WINDOW(comps->window), "destroy", G_CALLBACK(gui_window_destroy_event), projet);
    comps->main_grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(comps->window), GTK_WIDGET(comps->main_grid));
    
    m3d = &projet->list_gtk.m3d;
    gtk_grid_attach(GTK_GRID(comps->main_grid), m3d->drawing, 0, 1, 1, 1);
    gtk_widget_set_hexpand(m3d->drawing, TRUE);
    gtk_widget_set_vexpand(m3d->drawing, TRUE);
    g_signal_connect(GTK_WINDOW(comps->window), "key-press-event", G_CALLBACK(m3d_key_press), projet);
    
    comps->menu = gtk_menu_bar_new();
    gtk_grid_attach(GTK_GRID(comps->main_grid), comps->menu, 0, 0, 1, 1);
    gtk_widget_set_hexpand(comps->menu, TRUE);

    comps->menu_fichier_list = gtk_menu_new();
    comps->menu_fichier = gtk_menu_item_new_with_label(gettext("Fichier"));
    gtk_menu_shell_append(GTK_MENU_SHELL(comps->menu), comps->menu_fichier);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(comps->menu_fichier), comps->menu_fichier_list);
    
    comps->menu_fichier_informations = gtk_menu_item_new_with_label(gettext("Informations"));
    gtk_menu_shell_append(GTK_MENU_SHELL(comps->menu_fichier_list), comps->menu_fichier_informations);
    g_signal_connect_swapped(comps->menu_fichier_informations, "activate", G_CALLBACK(common_gtk_informations), projet);
    
    comps->menu_fichier_quitter = gtk_menu_item_new_with_label(gettext("Quitter"));
    gtk_menu_shell_append(GTK_MENU_SHELL(comps->menu_fichier_list), comps->menu_fichier_quitter);
    g_signal_connect_swapped(comps->menu_fichier_quitter, "activate", G_CALLBACK(gui_window_option_destroy_button), comps->window);
    
    comps->menu_affichage_list = gtk_menu_new();
    comps->menu_affichage = gtk_menu_item_new_with_label(gettext("Affichage"));
    gtk_menu_shell_append(GTK_MENU_SHELL(comps->menu), comps->menu_affichage);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(comps->menu_affichage), comps->menu_affichage_list);
    
    comps->menu_affichage_vues_list = gtk_menu_new();
    comps->menu_affichage_vues = gtk_menu_item_new_with_label(gettext("Vues"));
    gtk_menu_shell_append(GTK_MENU_SHELL(comps->menu_affichage_list), comps->menu_affichage_vues);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(comps->menu_affichage_vues), comps->menu_affichage_vues_list);
    
    comps->menu_affichage_xzy = gtk_menu_item_new_with_label(gettext("XZ vers Y+"));
    gtk_menu_shell_append(GTK_MENU_SHELL(comps->menu_affichage_vues_list), comps->menu_affichage_xzy);
    g_signal_connect_swapped(comps->menu_affichage_xzy, "activate", G_CALLBACK(m3d_camera_axe_x_z_y), projet);
    
    comps->menu_affichage_xz_y = gtk_menu_item_new_with_label(gettext("XZ vers Y-"));
    gtk_menu_shell_append(GTK_MENU_SHELL(comps->menu_affichage_vues_list), comps->menu_affichage_xz_y);
    g_signal_connect_swapped(comps->menu_affichage_xz_y, "activate", G_CALLBACK(m3d_camera_axe_x_z__y), projet);
    
    comps->menu_affichage_yzx = gtk_menu_item_new_with_label(gettext("YZ vers X+"));
    gtk_menu_shell_append(GTK_MENU_SHELL(comps->menu_affichage_vues_list), comps->menu_affichage_yzx);
    g_signal_connect_swapped(comps->menu_affichage_yzx, "activate", G_CALLBACK(m3d_camera_axe_y_z_x), projet);
    
    comps->menu_affichage_yz_x = gtk_menu_item_new_with_label(gettext("YZ vers X-"));
    gtk_menu_shell_append(GTK_MENU_SHELL(comps->menu_affichage_vues_list), comps->menu_affichage_yz_x);
    g_signal_connect_swapped(comps->menu_affichage_yz_x, "activate", G_CALLBACK(m3d_camera_axe_y_z__x), projet);
    
    comps->menu_affichage_xyz = gtk_menu_item_new_with_label(gettext("XY vers Z+"));
    gtk_menu_shell_append(GTK_MENU_SHELL(comps->menu_affichage_vues_list), comps->menu_affichage_xyz);
    g_signal_connect_swapped(comps->menu_affichage_xyz, "activate", G_CALLBACK(m3d_camera_axe_x_y_z), projet);
    
    comps->menu_affichage_xy_z = gtk_menu_item_new_with_label(gettext("XY vers Z-"));
    gtk_menu_shell_append(GTK_MENU_SHELL(comps->menu_affichage_vues_list), comps->menu_affichage_xy_z);
    g_signal_connect_swapped(comps->menu_affichage_xy_z, "activate", G_CALLBACK(m3d_camera_axe_x_y__z), projet);
    
    comps->menu_modelisation_list = gtk_menu_new();
    comps->menu_modelisation = gtk_menu_item_new_with_label(gettext("Modélisation"));
    gtk_menu_shell_append(GTK_MENU_SHELL(comps->menu), comps->menu_modelisation);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(comps->menu_modelisation), comps->menu_modelisation_list);
    
    comps->menu_modelisation_noeud_list = gtk_menu_new();
    comps->menu_modelisation_noeud = gtk_menu_item_new_with_label(gettext("Noeuds"));
    gtk_menu_shell_append(GTK_MENU_SHELL(comps->menu_modelisation_list), comps->menu_modelisation_noeud);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(comps->menu_modelisation_noeud), comps->menu_modelisation_noeud_list);
    
    comps->menu_modelisation_noeud_appui = gtk_menu_item_new_with_label(gettext("Appuis..."));
    gtk_menu_shell_append(GTK_MENU_SHELL(comps->menu_modelisation_noeud_list), comps->menu_modelisation_noeud_appui);
    g_signal_connect_swapped(comps->menu_modelisation_noeud_appui, "activate", G_CALLBACK(EF_gtk_appuis), projet);
    
    comps->menu_modelisation_noeud_ajout = gtk_menu_item_new_with_label(gettext("Gestion des noeuds..."));
    gtk_menu_shell_append(GTK_MENU_SHELL(comps->menu_modelisation_noeud_list), comps->menu_modelisation_noeud_ajout);
    g_signal_connect_swapped(comps->menu_modelisation_noeud_ajout, "activate", G_CALLBACK(EF_gtk_noeud), projet);
    
    comps->menu_modelisation_barres_list = gtk_menu_new();
    comps->menu_modelisation_barres = gtk_menu_item_new_with_label(gettext("Barres"));
    gtk_menu_shell_append(GTK_MENU_SHELL(comps->menu_modelisation_list), comps->menu_modelisation_barres);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(comps->menu_modelisation_barres), comps->menu_modelisation_barres_list);
    
    comps->menu_modelisation_barres_section = gtk_menu_item_new_with_label(gettext("Sections..."));
    gtk_menu_shell_append(GTK_MENU_SHELL(comps->menu_modelisation_barres_list), comps->menu_modelisation_barres_section);
    g_signal_connect_swapped(comps->menu_modelisation_barres_section, "activate", G_CALLBACK(EF_gtk_sections), projet);
    
    comps->menu_modelisation_barres_materiau = gtk_menu_item_new_with_label(gettext("Matériaux..."));
    gtk_menu_shell_append(GTK_MENU_SHELL(comps->menu_modelisation_barres_list), comps->menu_modelisation_barres_materiau);
    g_signal_connect_swapped(comps->menu_modelisation_barres_materiau, "activate", G_CALLBACK(EF_gtk_materiaux), projet);
    
    comps->menu_modelisation_barres_relachement = gtk_menu_item_new_with_label(gettext("Relâchements..."));
    gtk_menu_shell_append(GTK_MENU_SHELL(comps->menu_modelisation_barres_list), comps->menu_modelisation_barres_relachement);
    g_signal_connect_swapped(comps->menu_modelisation_barres_relachement, "activate", G_CALLBACK(EF_gtk_relachement), projet);
    
    comps->menu_modelisation_barres_ajout = gtk_menu_item_new_with_label(gettext("Gestion des barres..."));
    gtk_menu_shell_append(GTK_MENU_SHELL(comps->menu_modelisation_barres_list), comps->menu_modelisation_barres_ajout);
    g_signal_connect_swapped(comps->menu_modelisation_barres_ajout, "activate", G_CALLBACK(EF_gtk_barres), projet);
    
    menu_separator = gtk_separator_menu_item_new();
    gtk_menu_shell_append(GTK_MENU_SHELL(comps->menu_modelisation_list), menu_separator);
    
    comps->menu_modelisation_charges_list = gtk_menu_new();
    comps->menu_modelisation_charges = gtk_menu_item_new_with_label(gettext("Charges"));
    gtk_menu_shell_append(GTK_MENU_SHELL(comps->menu_modelisation_list), comps->menu_modelisation_charges);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(comps->menu_modelisation_charges), comps->menu_modelisation_charges_list);
    
    comps->menu_modelisation_charges_actions = gtk_menu_item_new_with_label(gettext("Actions..."));
    gtk_menu_shell_append(GTK_MENU_SHELL(comps->menu_modelisation_charges_list), comps->menu_modelisation_charges_actions);
    g_signal_connect_swapped(comps->menu_modelisation_charges_actions, "activate", G_CALLBACK(_1990_gtk_actions), projet);
    
    comps->menu_modelisation_charges_groupes = gtk_menu_item_new_with_label(gettext("Groupes..."));
    gtk_menu_shell_append(GTK_MENU_SHELL(comps->menu_modelisation_charges_list), comps->menu_modelisation_charges_groupes);
    g_signal_connect_swapped(comps->menu_modelisation_charges_groupes, "activate", G_CALLBACK(_1990_gtk_groupes), projet);
    
    comps->menu_resultats_list = gtk_menu_new();
    comps->menu_resultats = gtk_menu_item_new_with_label(gettext("Résultats"));
    gtk_menu_shell_append(GTK_MENU_SHELL(comps->menu), comps->menu_resultats);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(comps->menu_resultats), comps->menu_resultats_list);
    
    comps->menu_resultats_calculer = gtk_menu_item_new_with_label(gettext("Calculer"));
    gtk_menu_shell_append(GTK_MENU_SHELL(comps->menu_resultats_list), comps->menu_resultats_calculer);
    g_signal_connect(comps->menu_resultats_calculer, "activate", G_CALLBACK(EF_gtk_calculs_calculer), projet);
    
    comps->menu_resultats_afficher = gtk_menu_item_new_with_label(gettext("Afficher"));
    gtk_menu_shell_append(GTK_MENU_SHELL(comps->menu_resultats_list), comps->menu_resultats_afficher);
    g_signal_connect_swapped(comps->menu_resultats_afficher, "activate", G_CALLBACK(EF_gtk_resultats), projet);
    
    projet->list_gtk._1990_actions.window = NULL;
    projet->list_gtk._1990_groupes.window_groupe = NULL;
    
    return TRUE;
}
#endif


gboolean projet_free(Projet *projet)
/* Description : Libère les allocations mémoires de l'ensemble de la variable projet.
 * Paramètres : Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL,
 *             erreur lors de l'utilisation d'une fonction interne.
 */
{
    /* Action doit être libéré avant projet->modele.barres */
    // Trivial
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    
    BUG(common_ville_free(projet), FALSE);
    if (projet->actions != NULL)
        BUG(_1990_action_free(projet), FALSE);
    if (projet->niveaux_groupes != NULL)
        BUG(_1990_groupe_free(projet), FALSE);
    if (projet->combinaisons.elu_equ != NULL)
        BUG(_1990_combinaisons_free(projet), FALSE);
    /* Rigidite doit être libéré avant noeud car pour libérer toute la mémoire, il est nécessaire d'avoir accès aux informations contenues dans les noeuds */
    BUG(EF_calculs_free(projet), FALSE);
    if (projet->modele.sections != NULL)
        BUG(EF_sections_free(projet), FALSE);
    if (projet->modele.noeuds != NULL)
        BUG(EF_noeuds_free(projet), FALSE);
    if (projet->modele.barres != NULL)
        BUG(_1992_1_1_barres_free(projet), FALSE);
    if (projet->modele.appuis != NULL)
        BUG(EF_appuis_free(projet), FALSE);
    if (projet->modele.materiaux != NULL)
        BUG(_1992_1_1_materiaux_free(projet), FALSE);
    if (projet->modele.relachements != NULL)
        BUG(EF_relachement_free(projet), FALSE);
#ifdef ENABLE_GTK
    if (projet->list_gtk.m3d.data != NULL)
        BUG(m3d_free(projet), FALSE);
    EF_gtk_resultats_free(projet);
#endif
    
    cholmod_finish(projet->calculs.c);
    
    free(projet);
    
    _2lgc_unregister_resource();
    
    return TRUE;
}
