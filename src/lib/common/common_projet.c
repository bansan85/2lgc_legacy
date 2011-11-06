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
#include <stdio.h>
#include <stdlib.h>
#include <libintl.h>
#include <locale.h>
#include <cholmod.h>
#include <cholmod_internal.h>

#include "common_projet.h"
#include "common_erreurs.h"
#include "common_maths.h"
#include "common_m3d.hpp"
#include "1990_actions.h"
#include "1990_groupes.h"
#include "1990_gtk_groupes.h"
#include "1990_combinaisons.h"
#include "EF_appuis.h"
#include "EF_noeud.h"
#include "EF_rigidite.h"
#include "1992_1_1_barres.h"
#include "1992_1_1_section.h"
#include "1992_1_1_materiaux.h"

Projet* projet_init(Type_Pays pays)
/* Description : Initialise la variable projet
 * Paramètres : Type_Pays pays : pays du calcul
 * Valeur renvoyée :
 *   Succès : Un pointeur vers une zone mémoire projet
 *   Échec : NULL en cas d'erreur d'allocation mémoire.
 */
{
    Projet      *projet;
    // Alloue toutes les zones mémoires du projet à savoir (par module) :
    projet = (Projet*)malloc(sizeof(Projet));
    BUGMSG(projet, NULL, gettext("%s : Erreur d'allocation mémoire.\n"), "projet_init");
    //     - 1990 : la liste des actions, des groupes et des combinaisons,
    BUGMSG(_1990_action_init(projet) == 0, NULL, gettext("%s : Erreur d'allocation mémoire.\n"), "projet_init");
    BUGMSG(_1990_groupe_init(projet) == 0, NULL, gettext("%s : Erreur d'allocation mémoire.\n"), "projet_init");
    BUGMSG(_1990_combinaisons_init(projet) == 0, NULL, gettext("%s : Erreur d'allocation mémoire.\n"), "projet_init");
    //     - 1992-1-1 : la liste des sections, des barres et des matériaux
    BUGMSG(_1992_1_1_sections_init(projet) == 0, NULL, gettext("%s : Erreur d'allocation mémoire.\n"), "projet_init");
    BUGMSG(_1992_1_1_barres_init(projet) == 0, NULL, gettext("%s : Erreur d'allocation mémoire.\n"), "projet_init");
    BUGMSG(_1992_1_1_materiaux_init(projet) == 0, NULL, gettext("%s : Erreur d'allocation mémoire.\n"), "projet_init");
    //     - EF : la liste des appuis, des relâchements et des noeuds ainsi que les éléments
    //              nécessaire pour les calculs aux éléments finis.
    BUGMSG(EF_appuis_init(projet) == 0, NULL, gettext("%s : Erreur d'allocation mémoire.\n"), "projet_init");
    BUGMSG(EF_rigidite_init(projet) == 0, NULL, gettext("%s : Erreur d'allocation mémoire.\n"), "projet_init");
    BUGMSG(EF_relachement_init(projet) == 0, NULL, gettext("%s : Erreur d'allocation mémoire.\n"), "projet_init");
    BUGMSG(EF_noeuds_init(projet) == 0, NULL, gettext("%s : Erreur d'allocation mémoire.\n"), "projet_init");
    BUGMSG(m3d_init(projet) == 0, NULL, gettext("%s : Erreur d'allocation mémoire.\n"), "projet_init");
    
    projet->ef_donnees.c = &(projet->ef_donnees.Common);
    cholmod_l_start(projet->ef_donnees.c);
    
    projet->list_gtk._1990 = NULL;
    
    projet->pays = pays;
    return projet;
}


void gui_window_destroy_event(GtkWidget *pWidget __attribute__((unused)), Projet *projet)
{
    projet_free(projet);
    gtk_widget_destroy(pWidget);
    gtk_main_quit();
    return;
}


void gui_window_option_destroy_button(GtkWidget *fenetre)
/* Description : Bouton de fermeture de la fenêtre
 * Paramètres : GtkWidget *object : composant à l'origine de la demande
 *            : GtkWidget *fenetre : la fenêtre d'options
 * Valeur renvoyée : Aucune
 */
{
    gtk_widget_destroy(fenetre);
    return;
}


void gui_affiche_combinaisons(Projet *projet)
/* Description : Affiche la fenêtre des combinaisons
 * Paramètres : GtkComboBox *widget : composant à l'origine de la demande,
 *            : gpointer *data : donnée.
 * Valeur renvoyée : Aucune
 */
{
    _1990_gtk_groupes(NULL, projet);
    return;
}

int projet_init_graphique(Projet *projet)
/* Description : Crée une fenêtre graphique avec toute l'interface (menu, vue 3D, ...)
 * Paramètres : Projet *projet : variable projet
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet == NULL)
 */
{
    Comp_Gtk        *comps;
    List_Gtk_m3d    *m3d;
    
    BUGMSG(projet, -1, "projet_init_graphique\n");
    
    comps = &(projet->list_gtk.comp);
    
    comps->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(GTK_WINDOW(comps->window), "destroy", G_CALLBACK(gui_window_destroy_event), projet);
    comps->main_table = gtk_table_new(3,1,FALSE);
    gtk_container_add(GTK_CONTAINER(comps->window), GTK_WIDGET(comps->main_table));
    
    m3d = (List_Gtk_m3d*)projet->list_gtk.m3d;
    gtk_table_attach(GTK_TABLE(comps->main_table), m3d->drawing, 0, 1, 1, 2, (GtkAttachOptions)(GTK_EXPAND | GTK_FILL), (GtkAttachOptions)(GTK_EXPAND | GTK_FILL), 0, 0);
    
    comps->menu = gtk_menu_bar_new();
    gtk_table_attach(GTK_TABLE(comps->main_table), comps->menu, 0, 1, 0, 1, (GtkAttachOptions)(GTK_SHRINK | GTK_FILL), (GtkAttachOptions)(GTK_SHRINK | GTK_FILL), 0, 0);
    comps->menu_fichier_list = gtk_menu_new();
    
    comps->menu_fichier = gtk_menu_item_new_with_label(gettext("Fichier"));
    gtk_menu_shell_append(GTK_MENU_SHELL(comps->menu), comps->menu_fichier);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(comps->menu_fichier), comps->menu_fichier_list);
    
    comps->menu_fichier_quitter = gtk_menu_item_new_with_label(gettext("Quitter"));
    gtk_menu_shell_append(GTK_MENU_SHELL(comps->menu_fichier_list), comps->menu_fichier_quitter);
    g_signal_connect_swapped(comps->menu_fichier_quitter, "activate", G_CALLBACK(gui_window_option_destroy_button), comps->window);
    
    comps->menu_actions_list = gtk_menu_new();
    comps->menu_actions = gtk_menu_item_new_with_label(gettext("Actions"));
    gtk_menu_shell_append(GTK_MENU_SHELL(comps->menu), comps->menu_actions);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(comps->menu_actions), comps->menu_actions_list);
    
    comps->menu_actions_combinaisons = gtk_menu_item_new_with_label(gettext("Combinaisons"));
    gtk_menu_shell_append(GTK_MENU_SHELL(comps->menu_actions_list), comps->menu_actions_combinaisons);
    g_signal_connect_swapped(comps->menu_actions_combinaisons, "activate", G_CALLBACK(gui_affiche_combinaisons), projet);
    
    return 0;
}


void projet_free(Projet *projet)
/* Description : Libère les allocations mémoires de l'ensemble de la variable projet
 * Paramètres : Projet *projet : variable projet
 * Valeur renvoyée : void
 */
{
    /* Action doit être libéré avant projet->beton.barres */
    // Trivial
    if (projet->actions != NULL)
        _1990_action_free(projet);
    if (projet->niveaux_groupes != NULL)
        _1990_groupe_free(projet);
    if (projet->combinaisons.elu_equ != NULL)
        _1990_combinaisons_free(projet);
    /* Rigidite doit être libéré avant noeud car pour libérer toute la mémoire, il est nécessaire d'avoir accès aux informations contenues dans les noeuds */
    EF_rigidite_free(projet);
    if (projet->ef_donnees.noeuds != NULL)
        EF_noeuds_free(projet);
    if (projet->beton.sections != NULL)
        _1992_1_1_sections_free(projet);
    if (projet->beton.barres != NULL)
        _1992_1_1_barres_free(projet);
    if (projet->ef_donnees.appuis != NULL)
        EF_appuis_free(projet);
    if (projet->beton.materiaux != NULL)
        _1992_1_1_materiaux_free(projet);
    if (projet->ef_donnees.relachements != NULL)
        EF_relachement_free(projet);
    if (projet->list_gtk.m3d != NULL)
        m3d_free(projet);
    free(projet->list_gtk._1990);
    
    cholmod_l_finish(projet->ef_donnees.c);
    
    free(projet);
    
    return;
}
