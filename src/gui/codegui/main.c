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
#include "common_text.h"
#include "common_erreurs.h"
#include "common_projet.h"
#include "1990_groupes.h"
#include "1990_gtk_groupes.h"
#include "1990_actions.h"
#include "1992_1_1_barres.h"
#include "1992_1_1_section.h"
#include "EF_appuis.h"
#include "EF_relachement.h"
#include "EF_noeud.h"
#include "EF_calculs.h"
#include "EF_charge_noeud.h"
#include "EF_charge_barre_ponctuelle.h"
#include "EF_charge_barre_repartie_uniforme.h"
#include <stdio.h>
#include <stdlib.h>
#include <libintl.h>
#include <locale.h>
#include <string.h>
#include <gtk/gtk.h>
#include <time.h>

void gtk_window_destroy_event(GtkWidget *pWidget __attribute__((unused)), Projet *projet)
{
    projet_free(projet);
    gtk_main_quit();
    return;
}

void gtk_window_option_destroy_button(GtkWidget *object __attribute__((unused)), GtkWidget *fenetre __attribute__((unused)))
/* Description : Bouton de fermeture de la fenêtre
 * Paramètres : GtkComboBox *widget : composant à l'origine de la demande
 *            : GtkWidget *fenêtre : la fenêtre d'options
 * Valeur renvoyée : Aucune
 */
{
    gtk_widget_destroy(fenetre);
    return;
}

int main(int argc, char *argv[])
{
    /* Variables */
    GtkWidget * MainWindow = NULL;
    GtkWidget *pTable;
    GtkWidget *pButton;
    Projet *projet;
    
    EF_Relachement_Donnees_Elastique_Lineaire *ry_d, *rz_d, *ry_f, *rz_f;
    
    // On charge la localisation
    setlocale( LC_ALL, "" );
    bindtextdomain(PACKAGE, LOCALEDIR);
    textdomain(PACKAGE);
    
    // On traite les arguments du programme
    switch (argc)
    {
        case 2:
        {
            if ((strcmp(argv[1], "-w") == 0) || (strcmp(argv[1], "--warranty") == 0))
            {
                show_warranty();
                return 0;
            }
            else if ((strcmp(argv[1], "-h") == 0) || (strcmp(argv[1], "--help") == 0))
            {
                show_help();
                return 0;
            }
            break;
        }
        default:
        {
            break;
        }
    }
    
    // Création d'un projet type
    projet = projet_init(PAYS_FR);
    
    // Création des actions
    BUG(_1990_action_ajout(projet, 0) == 0, -1); // Poids propre
    BUG(_1990_action_ajout(projet, 2) == 0, -1); // 2 Exploitation
    BUG(_1990_action_ajout(projet, 16) == 0, -1); // 3 Neige
    BUG(_1990_action_ajout(projet, 17) == 0, -1); // 4 Vent
    
    // Création des groupes d'actions
    BUG(_1990_groupe_ajout_niveau(projet) == 0, -1);
    BUG(_1990_groupe_ajout_niveau(projet) == 0, -1);
    BUG(_1990_groupe_ajout_niveau(projet) == 0, -1);
    BUG(_1990_groupe_ajout_niveau(projet) == 0, -1);
    BUG(_1990_groupe_ajout_groupe(projet, 0, GROUPE_COMBINAISON_AND, _1990_action_type_bat_txt(0, projet->pays)) == 0, -1); // Charges permanentes
    BUG(_1990_groupe_ajout_groupe(projet, 0, GROUPE_COMBINAISON_AND, _1990_action_type_bat_txt(3, projet->pays)) == 0, -1); // Charges d'exploitation
    BUG(_1990_groupe_ajout_groupe(projet, 0, GROUPE_COMBINAISON_XOR, _1990_action_type_bat_txt(14, projet->pays)) == 0, -1); // Neige
    BUG(_1990_groupe_ajout_groupe(projet, 0, GROUPE_COMBINAISON_XOR, _1990_action_type_bat_txt(17, projet->pays)) == 0, -1); // Vent
    BUG(_1990_groupe_ajout_groupe(projet, 1, GROUPE_COMBINAISON_AND, _1990_action_type_bat_txt(0, projet->pays)) == 0, -1); // Charges permanentes
    BUG(_1990_groupe_ajout_groupe(projet, 1, GROUPE_COMBINAISON_AND, _1990_action_type_bat_txt(3, projet->pays)) == 0, -1); // Charges d'exploitation
    BUG(_1990_groupe_ajout_groupe(projet, 1, GROUPE_COMBINAISON_OR, (char*)"Charges climatiques") == 0, -1); // Charges climatiques
    BUG(_1990_groupe_ajout_groupe(projet, 2, GROUPE_COMBINAISON_AND, (char*)"Charges permanentes") == 0, -1); // Tout
    BUG(_1990_groupe_ajout_groupe(projet, 2, GROUPE_COMBINAISON_OR, (char*)"Charges variables") == 0, -1); // Tout
    BUG(_1990_groupe_ajout_groupe(projet, 3, GROUPE_COMBINAISON_AND, (char*)"Combinaisons complètes") == 0, -1); // Tout
    BUG(_1990_groupe_ajout_element(projet, 0, 0, 0) == 0, -1);
    BUG(_1990_groupe_ajout_element(projet, 0, 1, 1) == 0, -1);
    BUG(_1990_groupe_ajout_element(projet, 0, 2, 2) == 0, -1);
    BUG(_1990_groupe_ajout_element(projet, 0, 3, 3) == 0, -1);
    BUG(_1990_groupe_ajout_element(projet, 1, 0, 0) == 0, -1);
    BUG(_1990_groupe_ajout_element(projet, 1, 1, 1) == 0, -1);
    BUG(_1990_groupe_ajout_element(projet, 1, 2, 2) == 0, -1);
    BUG(_1990_groupe_ajout_element(projet, 1, 2, 3) == 0, -1);
    BUG(_1990_groupe_ajout_element(projet, 2, 0, 0) == 0, -1);
    BUG(_1990_groupe_ajout_element(projet, 2, 1, 1) == 0, -1);
    BUG(_1990_groupe_ajout_element(projet, 2, 1, 2) == 0, -1);
    BUG(_1990_groupe_ajout_element(projet, 3, 0, 0) == 0, -1);
    BUG(_1990_groupe_ajout_element(projet, 3, 0, 1) == 0, -1);
    
    // Création de l'appui
    BUG(EF_appuis_ajout(projet, EF_APPUI_BLOQUE, EF_APPUI_BLOQUE, EF_APPUI_BLOQUE, EF_APPUI_BLOQUE, EF_APPUI_BLOQUE, EF_APPUI_BLOQUE) == 0, -1);
    
    // Création des noeuds
    BUG(EF_noeuds_ajout(projet, 0.0, 0.0, 0.0, 0) == 0, -1);
    BUG(EF_noeuds_ajout(projet, 8.0, 0.0, 0.0, 0) == 0, -1);
    
    // Création des sections en béton
    BUG(_1992_1_1_sections_ajout_rectangulaire(projet, 0.1, 0.3) == 0, -1);
    
    // Création du matériau béton
    BUG(_1992_1_1_materiaux_ajout(projet, 25., 0.2) == 0, -1);
    
    // Création du relâchment
    ry_d = (EF_Relachement_Donnees_Elastique_Lineaire*)malloc(sizeof(EF_Relachement_Donnees_Elastique_Lineaire));
    ry_d->raideur = 3000000.;
    rz_d = (EF_Relachement_Donnees_Elastique_Lineaire*)malloc(sizeof(EF_Relachement_Donnees_Elastique_Lineaire));
    rz_d->raideur =  400000.;
    ry_f = (EF_Relachement_Donnees_Elastique_Lineaire*)malloc(sizeof(EF_Relachement_Donnees_Elastique_Lineaire));
    ry_f->raideur = 2300000.;
    rz_f = (EF_Relachement_Donnees_Elastique_Lineaire*)malloc(sizeof(EF_Relachement_Donnees_Elastique_Lineaire));
    rz_f->raideur =  340000.;
    BUG(EF_relachement_ajout(projet, EF_RELACHEMENT_BLOQUE, NULL, EF_RELACHEMENT_ELASTIQUE_LINEAIRE, ry_d, EF_RELACHEMENT_ELASTIQUE_LINEAIRE, rz_d, EF_RELACHEMENT_BLOQUE, NULL, EF_RELACHEMENT_ELASTIQUE_LINEAIRE, ry_f, EF_RELACHEMENT_ELASTIQUE_LINEAIRE, rz_f) == 0, -1);
//    BUG(EF_relachement_ajout(projet, EF_RELACHEMENT_BLOQUE, NULL, EF_RELACHEMENT_BLOQUE, NULL, EF_RELACHEMENT_BLOQUE, NULL, EF_RELACHEMENT_LIBRE, NULL, EF_RELACHEMENT_LIBRE, NULL, EF_RELACHEMENT_LIBRE, NULL) == 0, -1);
//    BUG(EF_relachement_ajout(projet, EF_RELACHEMENT_LIBRE, NULL, EF_RELACHEMENT_LIBRE, NULL, EF_RELACHEMENT_LIBRE, NULL, EF_RELACHEMENT_BLOQUE, NULL, EF_RELACHEMENT_BLOQUE, NULL, EF_RELACHEMENT_BLOQUE, NULL) == 0, -1);
    
    // Création de l'élément en béton
    BUG(_1992_1_1_barres_ajout(projet, BETON_ELEMENT_POUTRE, 0, 0, 0, 1, -1, 0) == 0, -1);
//    BUG(_1992_1_1_barres_ajout(projet, BETON_ELEMENT_POUTRE, 0, 0, 1, 2, -1, 0) == 0, -1);
    
    // Ajout de l'action ponctuelle
//    BUG(EF_charge_noeud_ajout(projet, 0, EF_noeuds_cherche_numero(projet, 1), 1000., 500., 1000., 3000., 5000., 5000.) == 0, -1);
//    BUG(EF_charge_barre_ponctuelle_ajout(projet, 0, _1992_1_1_barres_cherche_numero(projet, 0), FALSE, 2.3, 10000., 9000., 8000., 7000., 6000., 5000.) == 0, -1);
    BUG(EF_charge_barre_repartie_uniforme_ajout(projet, 0, _1992_1_1_barres_cherche_numero(projet, 0), FALSE, FALSE, 1.5, 1.0, 10000., 9000., 8000., 7000., 6000., 5000.) == 0, -1);
    
    // Initialise les éléments nécessaire pour l'ajout des rigidités
    BUG(EF_calculs_initialise(projet) == 0, -1);
    
    // Ajout de la rigidité de l'élément à la matrice globale du projet
    BUG(_1992_1_1_barres_rigidite_ajout_tout(projet) == 0, -1);
    
    BUG(EF_calculs_genere_mat_rig(projet) == 0, -1);
    BUG(EF_calculs_resoud_charge(projet, 0) == 0, -1);
    BUG(_1990_action_affiche_resultats(projet, 0) == 0, -1);
    
    // Initialisation de GTK+
    BUGMSG(gtk_init_check(&argc, &argv) == TRUE, -1, gettext("Impossible d'initialiser gtk.\n"));
    
    // Création de la fenêtre principale
    MainWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(GTK_WINDOW(MainWindow), "destroy", G_CALLBACK(gtk_window_destroy_event), projet);
    pTable=gtk_table_new(2,1,TRUE);
    gtk_container_add(GTK_CONTAINER(MainWindow), GTK_WIDGET(pTable));
    pButton= gtk_button_new_with_label("Combinaisons");
    gtk_table_attach(GTK_TABLE(pTable), pButton, 0, 1, 0, 1, (GtkAttachOptions)(GTK_EXPAND | GTK_FILL), (GtkAttachOptions)(GTK_EXPAND | GTK_FILL), 0, 0);
    g_signal_connect (pButton, "clicked", G_CALLBACK (_1990_gtk_groupes), projet);
    
    // Ajout du bouton de fermeture de l'application
    pButton= gtk_button_new_with_label("Fermeture");
    gtk_table_attach(GTK_TABLE(pTable), pButton, 0, 1, 1, 2, (GtkAttachOptions)(GTK_EXPAND | GTK_FILL), (GtkAttachOptions)(GTK_EXPAND | GTK_FILL), 0, 0);
    g_signal_connect (pButton, "clicked", G_CALLBACK (gtk_window_option_destroy_button), MainWindow);
    
    // Affichage de l'interface graphique
    gtk_widget_show_all(MainWindow);
    gtk_main();
    
    return 0;
}

