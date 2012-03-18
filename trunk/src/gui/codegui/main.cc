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

#include "common_m3d.hpp"

#ifdef __cplusplus
extern "C" {
#endif
#include "common_text.h"
#include "common_erreurs.h"
#include "common_maths.h"
#include "common_selection.h"
#include "common_projet.h"
#include "1990_groupes.h"
#include "1990_actions.h"
#include "1992_1_1_barres.h"
#include "1992_1_1_section.h"
#include "1992_1_1_materiaux.h"
#include "EF_appuis.h"
#include "EF_relachement.h"
#include "EF_noeud.h"
#include "EF_calculs.h"
#include "EF_charge_noeud.h"
#include "EF_charge_barre_ponctuelle.h"
#include "EF_charge_barre_repartie_uniforme.h"
#ifdef __cplusplus
}
#endif

#include <stdio.h>
#include <stdlib.h>
#include <libintl.h>
#include <locale.h>
#include <string.h>
#include <gtk/gtk.h>
#include <time.h>
#include <M3d++.hpp>


int main(int argc, char *argv[])
{
    /* Variables */
    Projet *projet;

    
    EF_Relachement_Donnees_Elastique_Lineaire *ry_d, *rz_d, *ry_f, *rz_f;
    
    // On charge la localisation
    setlocale( LC_ALL, "" );
    bindtextdomain(PACKAGE, LOCALEDIR);
    bind_textdomain_codeset(PACKAGE, "UTF-8");
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
    
    // Initialisation de GTK+, gtk doit être initialisé avant m3dlib.
    BUGMSG(gtk_init_check(&argc, &argv) == TRUE, -1, gettext("Impossible d'initialiser gtk.\n"));
    
    // Création d'un projet type
    BUG(projet = projet_init(PAYS_FR), -1);
    
    // Création des actions
    BUG(_1990_action_ajout(projet, 0, "Poids propre"), -1); // Poids propre
    BUG(_1990_action_ajout(projet, 2, "Chargement"), -1); // 2 Exploitation
    BUG(_1990_action_ajout(projet, 16, "Neige"), -1); // 3 Neige
    BUG(_1990_action_ajout(projet, 17, "Vent"), -1); // 4 Vent
    
    // Création des groupes d'actions
    BUG(_1990_groupe_ajout_niveau(projet) == 0, -1);
    BUG(_1990_groupe_ajout_niveau(projet) == 0, -1);
    BUG(_1990_groupe_ajout_niveau(projet) == 0, -1);
    BUG(_1990_groupe_ajout_niveau(projet) == 0, -1);
    BUG(_1990_groupe_ajout_groupe(projet, 0, GROUPE_COMBINAISON_AND, _1990_action_type_bat_txt(0, projet->pays)), -1); // Charges permanentes
    BUG(_1990_groupe_ajout_groupe(projet, 0, GROUPE_COMBINAISON_AND, _1990_action_type_bat_txt(3, projet->pays)), -1); // Charges d'exploitation
    BUG(_1990_groupe_ajout_groupe(projet, 0, GROUPE_COMBINAISON_XOR, _1990_action_type_bat_txt(14, projet->pays)), -1); // Neige
    BUG(_1990_groupe_ajout_groupe(projet, 0, GROUPE_COMBINAISON_XOR, _1990_action_type_bat_txt(17, projet->pays)), -1); // Vent
    BUG(_1990_groupe_ajout_groupe(projet, 1, GROUPE_COMBINAISON_AND, _1990_action_type_bat_txt(0, projet->pays)), -1); // Charges permanentes
    BUG(_1990_groupe_ajout_groupe(projet, 1, GROUPE_COMBINAISON_AND, _1990_action_type_bat_txt(3, projet->pays)), -1); // Charges d'exploitation
    BUG(_1990_groupe_ajout_groupe(projet, 1, GROUPE_COMBINAISON_OR, "Charges climatiques"), -1); // Charges climatiques
    BUG(_1990_groupe_ajout_groupe(projet, 2, GROUPE_COMBINAISON_AND, "Charges permanentes"), -1); // Tout
    BUG(_1990_groupe_ajout_groupe(projet, 2, GROUPE_COMBINAISON_OR, "Charges variables"), -1); // Tout
    BUG(_1990_groupe_ajout_groupe(projet, 3, GROUPE_COMBINAISON_AND, "Combinaisons complètes"), -1); // Tout
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
    BUG(EF_appuis_ajout(projet, "Encastrement", EF_APPUI_BLOQUE, EF_APPUI_BLOQUE, EF_APPUI_BLOQUE, EF_APPUI_BLOQUE, EF_APPUI_BLOQUE, EF_APPUI_BLOQUE) == 0, -1);
    
    // Création des noeuds
    BUG(EF_noeuds_ajout_noeud_libre(projet, 0.0, 0.0, 0.0, EF_appuis_cherche_numero(projet, 0)), -1);
    BUG(EF_noeuds_ajout_noeud_libre(projet, 0.0, 0.0, 8.0, NULL), -1);
    BUG(EF_noeuds_ajout_noeud_libre(projet, 8.0, 0.0, 8.0, NULL), -1);
    BUG(EF_noeuds_ajout_noeud_libre(projet, 8.0, 0.0, 0.0, EF_appuis_cherche_numero(projet, 0)), -1);
    
    // Création des sections en béton
    BUG(_1992_1_1_sections_ajout_rectangulaire(projet, 0.3, 0.5) == 0, -1);
    
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
    BUG(EF_relachement_ajout(projet, "test123", EF_RELACHEMENT_BLOQUE, NULL, EF_RELACHEMENT_ELASTIQUE_LINEAIRE, ry_d, EF_RELACHEMENT_ELASTIQUE_LINEAIRE, rz_d, EF_RELACHEMENT_BLOQUE, NULL, EF_RELACHEMENT_ELASTIQUE_LINEAIRE, ry_f, EF_RELACHEMENT_ELASTIQUE_LINEAIRE, rz_f) == 0, -1);
//    BUG(EF_relachement_ajout(projet, EF_RELACHEMENT_BLOQUE, NULL, EF_RELACHEMENT_BLOQUE, NULL, EF_RELACHEMENT_BLOQUE, NULL, EF_RELACHEMENT_LIBRE, NULL, EF_RELACHEMENT_LIBRE, NULL, EF_RELACHEMENT_LIBRE, NULL) == 0, -1);
//    BUG(EF_relachement_ajout(projet, EF_RELACHEMENT_LIBRE, NULL, EF_RELACHEMENT_LIBRE, NULL, EF_RELACHEMENT_LIBRE, NULL, EF_RELACHEMENT_BLOQUE, NULL, EF_RELACHEMENT_BLOQUE, NULL, EF_RELACHEMENT_BLOQUE, NULL) == 0, -1);
    
    // Création de l'élément en béton
    BUG(_1992_1_1_barres_ajout(projet, BETON_ELEMENT_POUTRE, 0, 0, 0, 1, NULL, 9) == 0, -1);
    BUG(_1992_1_1_barres_ajout(projet, BETON_ELEMENT_POUTRE, 0, 0, 1, 2, NULL, 9) == 0, -1);
    BUG(_1992_1_1_barres_ajout(projet, BETON_ELEMENT_POUTRE, 0, 0, 2, 3, NULL, 9) == 0, -1);
//    BUG(_1992_1_1_barres_ajout(projet, BETON_ELEMENT_POUTRE, 0, 0, 1, 2, NULL, 0) == 0, -1);
    
    // Ajout de l'action ponctuelle
    GList   *tmp1, *tmp2;
    BUG(tmp1 = common_selection_renvoie_numeros("1;3-5"), -1);
    BUG(tmp2 = common_selection_converti_numeros_en_noeuds(tmp1, projet), -1);
    BUG(EF_charge_noeud_ajout(projet, 0, tmp2, 1000., 500., 1000., 3000., 5000., 5000., "test1"), -1);
    g_list_free(tmp1);
    BUG(tmp1 = common_selection_renvoie_numeros("0-2"), -1);
    BUG(tmp2 = common_selection_converti_numeros_en_barres(tmp1, projet), -1);
    BUG(EF_charge_barre_ponctuelle_ajout(projet, 0, tmp2, FALSE, 2.3, 10000., 9000., 8000., 7000., 6000., 5000., "test2"), -1);
    g_list_free(tmp1);
    BUG(tmp1 = common_selection_renvoie_numeros("0-2/2"), -1);
    BUG(tmp2 = common_selection_converti_numeros_en_barres(tmp1, projet), -1);
    BUG(EF_charge_barre_repartie_uniforme_ajout(projet, 0, tmp2, FALSE, FALSE, 1.5, 1.0, 10000., 9000., 8000., 7000., 6000., 5000., "test3"), -1);
    g_list_free(tmp1);
    
    // Initialise les éléments nécessaire pour l'ajout des rigidités
    BUG(EF_calculs_initialise(projet) == 0, -1);
    
    // Ajout de la rigidité de l'élément à la matrice globale du projet
    BUG(_1992_1_1_barres_rigidite_ajout_tout(projet) == 0, -1);
    
    BUG(EF_calculs_genere_mat_rig(projet) == 0, -1);
    BUG(EF_calculs_resoud_charge(projet, 0) == 0, -1);
    BUG(_1990_action_affiche_resultats(projet, 0) == 0, -1);
    
    // Création de la fenêtre principale
    BUG(projet_init_graphique(projet) == 0, -1);
//    g_signal_connect (pButton, "clicked", G_CALLBACK (_1990_gtk_groupes), projet);
    
    BUG(m3d_camera_axe_x_z(projet) == 0, -1);
    BUG(m3d_genere_graphique(projet) == 0, -1);
    
    // Affichage de l'interface graphique
    gtk_widget_show_all(projet->list_gtk.comp.window);
    gtk_main();
    
    return 0;
}

