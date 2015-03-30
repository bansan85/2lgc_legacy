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

#include <stdint.h>
#include <libintl.h>
#include <string.h>
#include <memory>
#include <iostream>
#include <cassert>

#include "codegui.hpp"


int
main (int32_t argc,
      char   *argv[])
{
  /* Variables */
  CProjet projet (ENorme::EUROCODE);
  std::shared_ptr <CAction> action;
  
  // On charge la localisation
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE_NAME, LOCALEDIR);
  bind_textdomain_codeset (PACKAGE_NAME, "UTF-8");
  textdomain (PACKAGE_NAME);
  
  // On traite les arguments du programme
  switch (argc)
  {
    case 2:
    {
      if ((strcmp (argv[1], "-w") == 0) ||
          (strcmp (argv[1], "--warranty") == 0))
      {
        CProjet::showWarranty ();
        return 0;
      }
      else if ((strcmp (argv[1], "-h") == 0) ||
               (strcmp (argv[1], "--help") == 0))
      {
        CProjet::showHelp ();
        return 0;
      }
      break;
    }
    default:
    {
      break;
    }
  }

  assert (projet.getActionCount () == 0);
  // 0 Poids propre
  action = std::make_shared <CAction> (std::make_shared <std::string>
                                                              ("Poids propre"),
                             0,
                             projet);
  assert (projet.addAction (action));
  assert (projet.getActionCount () == 1);
  // 2 Exploitation
  action = std::make_shared <CAction> (std::make_shared <std::string>
                                                                ("Chargement"),
                             2,
                             projet);
  assert (projet.addAction (action));
  assert (projet.getActionCount () == 2);
  // 18 Neige
  action = std::make_shared <CAction> (std::make_shared <std::string>
                                                                     ("Neige"),
                             18,
                             projet);
  assert (projet.addAction (action));
  assert (projet.getActionCount () == 3);
  // 19 Vent
  action = std::make_shared <CAction> (std::make_shared <std::string> ("Vent"),
                             19,
                             projet);
  assert (projet.addAction (action));
  assert (projet.getActionCount () == 4);
  assert (projet.undo ());
  assert (projet.getActionCount () == 3);
  assert (projet.undo ());
  assert (projet.getActionCount () == 2);
  assert (projet.undo ());
  assert (projet.getActionCount () == 1);
  assert (projet.undo ());
  assert (projet.getActionCount () == 0);
  
  assert (projet.enregistre ("test.xml"));
  
  // Création des groupes d'actions
/*  assert(_1990_groupe_ajout_niveau(projet), -1);
  assert(_1990_groupe_ajout_niveau(projet), -1);
  assert(_1990_groupe_ajout_niveau(projet), -1);
  assert(_1990_groupe_ajout_niveau(projet), -1);
  assert(_1990_groupe_ajout_groupe(projet, 0, GROUPE_COMBINAISON_AND, _1990_action_type_bat_txt(0, projet->parametres.pays)), -1) // Charges permanentes
  assert(_1990_groupe_ajout_groupe(projet, 0, GROUPE_COMBINAISON_AND, _1990_action_type_bat_txt(3, projet->parametres.pays)), -1) // Charges d'exploitation
  assert(_1990_groupe_ajout_groupe(projet, 0, GROUPE_COMBINAISON_XOR, _1990_action_type_bat_txt(14, projet->parametres.pays)), -1) // Neige
  assert(_1990_groupe_ajout_groupe(projet, 0, GROUPE_COMBINAISON_XOR, _1990_action_type_bat_txt(17, projet->parametres.pays)), -1) // Vent
  assert(_1990_groupe_ajout_groupe(projet, 1, GROUPE_COMBINAISON_AND, _1990_action_type_bat_txt(0, projet->parametres.pays)), -1) // Charges permanentes
  assert(_1990_groupe_ajout_groupe(projet, 1, GROUPE_COMBINAISON_AND, _1990_action_type_bat_txt(3, projet->parametres.pays)), -1) // Charges d'exploitation
  assert(_1990_groupe_ajout_groupe(projet, 1, GROUPE_COMBINAISON_OR, "Charges climatiques"), -1) // Charges climatiques
  assert(_1990_groupe_ajout_groupe(projet, 2, GROUPE_COMBINAISON_AND, "Charges permanentes"), -1) // Tout
  assert(_1990_groupe_ajout_groupe(projet, 2, GROUPE_COMBINAISON_OR, "Charges variables"), -1) // Tout
  assert(_1990_groupe_ajout_groupe(projet, 3, GROUPE_COMBINAISON_AND, "Combinaisons complètes"), -1) // Tout
  assert(_1990_groupe_ajout_element(projet, 0, 0, 0), -1);
  assert(_1990_groupe_ajout_element(projet, 0, 1, 1), -1);
  assert(_1990_groupe_ajout_element(projet, 0, 2, 2), -1);
  assert(_1990_groupe_ajout_element(projet, 0, 3, 3), -1);
  assert(_1990_groupe_ajout_element(projet, 1, 0, 0), -1);
  assert(_1990_groupe_ajout_element(projet, 1, 1, 1), -1);
  assert(_1990_groupe_ajout_element(projet, 1, 2, 2), -1);
  assert(_1990_groupe_ajout_element(projet, 1, 2, 3), -1);
  assert(_1990_groupe_ajout_element(projet, 2, 0, 0), -1);
  assert(_1990_groupe_ajout_element(projet, 2, 1, 1), -1);
  assert(_1990_groupe_ajout_element(projet, 2, 1, 2), -1);
  assert(_1990_groupe_ajout_element(projet, 3, 0, 0), -1);
  assert(_1990_groupe_ajout_element(projet, 3, 0, 1), -1);
  
  // Création de l'appui
  assert(EF_appuis_ajout(projet, "Rotule", EF_APPUI_BLOQUE, EF_APPUI_BLOQUE, EF_APPUI_BLOQUE, EF_APPUI_BLOQUE, EF_APPUI_LIBRE, EF_APPUI_LIBRE), -1);
  assert(EF_appuis_ajout(projet, "Encastrement", EF_APPUI_BLOQUE, EF_APPUI_BLOQUE, EF_APPUI_BLOQUE, EF_APPUI_BLOQUE, EF_APPUI_BLOQUE, EF_APPUI_BLOQUE), -1);
  assert(EF_appuis_ajout(projet, "Appui simple x", EF_APPUI_BLOQUE, EF_APPUI_LIBRE, EF_APPUI_LIBRE, EF_APPUI_LIBRE, EF_APPUI_LIBRE, EF_APPUI_LIBRE), -1);
  assert(EF_appuis_ajout(projet, "Appui simple z", EF_APPUI_LIBRE, EF_APPUI_LIBRE, EF_APPUI_BLOQUE, EF_APPUI_LIBRE, EF_APPUI_LIBRE, EF_APPUI_LIBRE), -1);
  
  // Création des noeuds
  assert(EF_noeuds_ajout_noeud_libre(projet, common_math_f(0.0, FLOTTANT_UTILISATEUR), common_math_f(0.0, FLOTTANT_UTILISATEUR), common_math_f(0.0, FLOTTANT_UTILISATEUR), EF_appuis_cherche_nom(projet, "Encastrement", TRUE), NULL), -1);
  assert(EF_noeuds_ajout_noeud_libre(projet, common_math_f(0.0, FLOTTANT_UTILISATEUR), common_math_f(0.0, FLOTTANT_UTILISATEUR), common_math_f(5.0, FLOTTANT_UTILISATEUR), EF_appuis_cherche_nom(projet, "Appui simple x", TRUE), NULL), -1);
  assert(EF_noeuds_ajout_noeud_libre(projet, common_math_f(4.0, FLOTTANT_UTILISATEUR), common_math_f(0.0, FLOTTANT_UTILISATEUR), common_math_f(5.0, FLOTTANT_UTILISATEUR), NULL, NULL), -1);
  assert(EF_noeuds_ajout_noeud_libre(projet, common_math_f(4.0, FLOTTANT_UTILISATEUR), common_math_f(0.0, FLOTTANT_UTILISATEUR), common_math_f(0.0, FLOTTANT_UTILISATEUR), EF_appuis_cherche_nom(projet, "Rotule", TRUE), NULL), -1);
  assert(EF_noeuds_ajout_noeud_libre(projet, common_math_f(-2.0, FLOTTANT_UTILISATEUR), common_math_f(0.0, FLOTTANT_UTILISATEUR), common_math_f(2.0, FLOTTANT_UTILISATEUR), EF_appuis_cherche_nom(projet, "Appui simple z", TRUE), EF_noeuds_cherche_numero(projet, 3, TRUE)), -1);
  
  // Création des sections en béton
  assert(EF_sections_rectangulaire_ajout(projet, "Rect_0.3*0.5ht", common_math_f(0.3, FLOTTANT_UTILISATEUR), common_math_f(0.5, FLOTTANT_UTILISATEUR)), -1);
  assert(EF_sections_T_ajout(projet, "T_1.00*0.20*0.10*0.50", common_math_f(1.00, FLOTTANT_UTILISATEUR), common_math_f(0.20, FLOTTANT_UTILISATEUR), common_math_f(0.10, FLOTTANT_UTILISATEUR), common_math_f(0.50, FLOTTANT_UTILISATEUR)), -1);
  assert(EF_sections_carree_ajout(projet, "Carre_0.50", common_math_f(0.5, FLOTTANT_UTILISATEUR)), -1);
  assert(EF_sections_circulaire_ajout(projet, "Circulaire_0.05", common_math_f(0.05, FLOTTANT_UTILISATEUR)), -1);
  assert(EF_sections_rectangulaire_ajout(projet, "Rect_0.3*0.1ht", common_math_f(0.3, FLOTTANT_UTILISATEUR), common_math_f(0.1, FLOTTANT_UTILISATEUR)), -1);
  assert(EF_sections_T_ajout(projet, "T_0.20*0.60*0.30*0.20", common_math_f(0.20, FLOTTANT_UTILISATEUR), common_math_f(0.60, FLOTTANT_UTILISATEUR), common_math_f(0.30, FLOTTANT_UTILISATEUR), common_math_f(0.20, FLOTTANT_UTILISATEUR)), -1);
  assert(EF_sections_carree_ajout(projet, "Carre_0.20", common_math_f(0.2, FLOTTANT_UTILISATEUR)), -1);
  assert(EF_sections_circulaire_ajout(projet, "Circulaire_0.4", common_math_f(0.4, FLOTTANT_UTILISATEUR)), -1);
  
  // Création du matériau béton
  assert(_1992_1_1_materiaux_ajout(projet, "B20", common_math_f(20., FLOTTANT_UTILISATEUR)), -1);
  assert(_1992_1_1_materiaux_ajout(projet, "B30", common_math_f(30., FLOTTANT_UTILISATEUR)), -1);
  assert(_1992_1_1_materiaux_ajout(projet, "B40", common_math_f(40., FLOTTANT_UTILISATEUR)), -1);
  assert(_1992_1_1_materiaux_ajout(projet, "B50", common_math_f(50., FLOTTANT_UTILISATEUR)), -1);
  assert(_1992_1_1_materiaux_ajout(projet, "B60", common_math_f(60., FLOTTANT_UTILISATEUR)), -1);
  assert(_1992_1_1_materiaux_ajout(projet, "B25", common_math_f(25., FLOTTANT_UTILISATEUR)), -1);
  assert(_1992_1_1_materiaux_ajout(projet, "B35", common_math_f(35., FLOTTANT_UTILISATEUR)), -1);
  assert(_1992_1_1_materiaux_ajout(projet, "B45", common_math_f(45., FLOTTANT_UTILISATEUR)), -1);
  assert(_1992_1_1_materiaux_ajout(projet, "B55", common_math_f(55., FLOTTANT_UTILISATEUR)), -1);
  assert(_1993_1_1_materiaux_ajout(projet, "S235", common_math_f(235., FLOTTANT_UTILISATEUR), common_math_f(360., FLOTTANT_UTILISATEUR)), -1);
  
  // Création du relâchment
  ry_d = (EF_Relachement_Donnees_Elastique_Lineaire*)malloc(sizeof(EF_Relachement_Donnees_Elastique_Lineaire));
  ry_d->raideur = common_math_f(3000000., FLOTTANT_UTILISATEUR);
  rz_d = (EF_Relachement_Donnees_Elastique_Lineaire*)malloc(sizeof(EF_Relachement_Donnees_Elastique_Lineaire));
  rz_d->raideur =  common_math_f(400000., FLOTTANT_UTILISATEUR);
  ry_f = (EF_Relachement_Donnees_Elastique_Lineaire*)malloc(sizeof(EF_Relachement_Donnees_Elastique_Lineaire));
  ry_f->raideur = common_math_f(2300000., FLOTTANT_UTILISATEUR);
  rz_f = (EF_Relachement_Donnees_Elastique_Lineaire*)malloc(sizeof(EF_Relachement_Donnees_Elastique_Lineaire));
  rz_f->raideur =  common_math_f(340000., FLOTTANT_UTILISATEUR);
  assert(EF_relachement_ajout(projet, "Articulation", EF_RELACHEMENT_BLOQUE, NULL, EF_RELACHEMENT_BLOQUE, NULL, EF_RELACHEMENT_BLOQUE, NULL, EF_RELACHEMENT_LIBRE, NULL, EF_RELACHEMENT_LIBRE, NULL, EF_RELACHEMENT_LIBRE, NULL), -1);
  assert(EF_relachement_ajout(projet, "Rotation nulle", EF_RELACHEMENT_LIBRE, NULL, EF_RELACHEMENT_LIBRE, NULL, EF_RELACHEMENT_LIBRE, NULL, EF_RELACHEMENT_BLOQUE, NULL, EF_RELACHEMENT_BLOQUE, NULL, EF_RELACHEMENT_BLOQUE, NULL), -1);
  assert(EF_relachement_ajout(projet, "Partiel", EF_RELACHEMENT_BLOQUE, NULL, EF_RELACHEMENT_ELASTIQUE_LINEAIRE, ry_d, EF_RELACHEMENT_ELASTIQUE_LINEAIRE, rz_d, EF_RELACHEMENT_BLOQUE, NULL, EF_RELACHEMENT_ELASTIQUE_LINEAIRE, ry_f, EF_RELACHEMENT_ELASTIQUE_LINEAIRE, rz_f), -1);
  
  // Création de l'élément en béton
  assert(_1992_1_1_barres_ajout(projet, BETON_ELEMENT_POUTRE, EF_sections_cherche_nom(projet, "Rect_0.3*0.5ht", TRUE), EF_materiaux_cherche_nom(projet, "B25", TRUE), 0, 1, common_math_f(0., FLOTTANT_UTILISATEUR), NULL, 1), -1);
  assert(_1992_1_1_barres_ajout(projet, BETON_ELEMENT_POUTRE, EF_sections_cherche_nom(projet, "T_1.00*0.20*0.10*0.50", TRUE), EF_materiaux_cherche_nom(projet, "B25", TRUE), 1, 2, common_math_f(0., FLOTTANT_UTILISATEUR), EF_relachement_cherche_nom(projet, "Articulation", TRUE), 2), -1);
  assert(_1992_1_1_barres_ajout(projet, BETON_ELEMENT_POUTRE, EF_sections_cherche_nom(projet, "Carre_0.50", TRUE), EF_materiaux_cherche_nom(projet, "B25", TRUE), 2, 3, common_math_f(0., FLOTTANT_UTILISATEUR), NULL, 3), -1);
  assert(_1992_1_1_barres_ajout(projet, BETON_ELEMENT_POUTRE, EF_sections_cherche_nom(projet, "Circulaire_0.05", TRUE), EF_materiaux_cherche_nom(projet, "B25", TRUE), 3, 4, common_math_f(0., FLOTTANT_UTILISATEUR), NULL, 0), -1);
  
  // Ajout de l'action ponctuelle
  assert(tmp1 = common_selection_renvoie_numeros("1;3-5"), -1);
  assert(tmp2 = common_selection_converti_numeros_en_noeuds(tmp1, projet), -1);
  assert(EF_charge_noeud_ajout(projet, 0, tmp2, 1000., 500., 1000., 3000., 5000., 5000., "test1"), -1);
  g_list_free(tmp1);
  assert(tmp1 = common_selection_renvoie_numeros("2"), -1);
  assert(tmp2 = common_selection_converti_numeros_en_noeuds(tmp1, projet), -1);
  assert(EF_charge_noeud_ajout(projet, 0, tmp2, common_math_f(0., FLOTTANT_UTILISATEUR), common_math_f(0., FLOTTANT_UTILISATEUR), common_math_f(0., FLOTTANT_UTILISATEUR), common_math_f(0., FLOTTANT_UTILISATEUR), common_math_f(0., FLOTTANT_UTILISATEUR), common_math_f(0., FLOTTANT_UTILISATEUR), "test1"), -1);
  g_list_free(tmp1);
  assert(tmp1 = common_selection_renvoie_numeros("1"), -1);
  assert(tmp2 = common_selection_converti_numeros_en_barres(tmp1, projet), -1);
  assert(EF_charge_barre_ponctuelle_ajout(projet, 2, tmp2, FALSE, common_math_f(1.0, FLOTTANT_UTILISATEUR), common_math_f(0., FLOTTANT_UTILISATEUR), common_math_f(0., FLOTTANT_UTILISATEUR), common_math_f(-10000., FLOTTANT_UTILISATEUR), common_math_f(0., FLOTTANT_UTILISATEUR), common_math_f(0., FLOTTANT_UTILISATEUR), common_math_f(0., FLOTTANT_UTILISATEUR), "test2"), -1);
  assert(tmp2 = common_selection_converti_numeros_en_barres(tmp1, projet), -1);
  assert(EF_charge_barre_repartie_uniforme_ajout(projet, 3, tmp2, FALSE, FALSE, common_math_f(0., FLOTTANT_UTILISATEUR), common_math_f(0., FLOTTANT_UTILISATEUR), common_math_f(0., FLOTTANT_UTILISATEUR), common_math_f(0., FLOTTANT_UTILISATEUR), common_math_f(-10000., FLOTTANT_UTILISATEUR), common_math_f(0., FLOTTANT_UTILISATEUR), common_math_f(0., FLOTTANT_UTILISATEUR), common_math_f(0., FLOTTANT_UTILISATEUR), "test3"), -1);
  g_list_free(tmp1);
  assert(tmp1 = common_selection_renvoie_numeros("0-2/2"), -1);
  assert(tmp2 = common_selection_converti_numeros_en_barres(tmp1, projet), -1);
  assert(EF_charge_barre_repartie_uniforme_ajout(projet, 0, tmp2, FALSE, FALSE, 1.5, 1.0, 10000., 9000., 8000., 7000., 6000., 5000., "test3"), -1);
  g_list_free(tmp1);*/
  
  // Affichage de l'interface graphique
//  gtk_widget_show_all (projet->ui.comp.window);
//  gtk_main ();
  
  return 0;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
