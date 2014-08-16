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

// Très gros exemple devant couvrir tout le code de la librairie en une fois.

#include "config.h"

#include "codegui.hpp"
#include "MErreurs.hh"

#include <stdint.h>
#include <libintl.h>
#include <string.h>

/*
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <time.h>

#ifdef ENABLE_GTK
#include <gtk/gtk.h>
#endif

#include "common_m3d.hpp"

#include "common_text.hpp"
#include "common_erreurs.hpp"
#include "common_math.hpp"
#include "common_selection.hpp"
#include "common_projet.hpp"
#include "1990_groupe.hpp"
#include "1990_action.hpp"
#include "1992_1_1_barres.hpp"
#include "1992_1_1_materiaux.hpp"
#include "1993_1_1_materiaux.hpp"
#include "EF_appuis.hpp"
#include "EF_relachement.hpp"
#include "EF_materiaux.hpp"
#include "EF_noeuds.hpp"
#include "EF_calculs.hpp"
#include "EF_charge_noeud.hpp"
#include "EF_charge_barre_ponctuelle.hpp"
#include "EF_charge_barre_repartie_uniforme.hpp"
#include "EF_sections.hpp"
*/

int
main (int32_t argc,
      char   *argv[])
{
  /* Variables */
  CProjet projet (NORME_EC);
//  GList   *tmp1, *tmp2;
  
//  EF_Relachement_Donnees_Elastique_Lineaire *ry_d, *rz_d, *ry_f, *rz_f;
  
  // On charge la localisation
  setlocale (LC_ALL, "" );
  bindtextdomain (PACKAGE, LOCALEDIR);
  bind_textdomain_codeset (PACKAGE, "UTF-8");
  textdomain (PACKAGE);
  
  // Initialisation de GTK+, gtk doit être initialisé avant m3dlib.
/*  INFO (gtk_init_check (&argc, &argv),
        -1,
        (gettext ("Impossible d'initialiser gtk.\n")); )*/
  
  BUGCONT (projet.ref (), -1, NULL)
  
  std::unique_ptr <CAction> action;
  for (uint8_t i = 0; i < 22; i++)
  {
    action.reset (new CAction (new std::string (projet.getParametres ()->
                                                        getpsiDescription (i)),
                               0,
                               projet));
    
    BUGCONT (projet.addAction (action.get ()), -1, NULL)
    
    action.release ();
  }
  
  BUGCONT (projet.getEtat () == UNDO_MODIF, -1, NULL)
  BUGCONT (projet.unref (), -1, NULL)
  
  BUGCONT (projet.ref (), -1, NULL)
  CAction *action2;
  BUGCONT (action2 = projet.getAction (projet.getParametres ()->
                                                        getpsiDescription (5)),
           -1,
           NULL)
  BUGCONT (action2->setpsi0 (new CNbCalcul (0.5, U_, projet.getDecimales ())),
           -1,
           NULL)
  BUGCONT (action2->setpsi1 (new CNbCalcul (0.5, U_, projet.getDecimales ())),
           -1,
           NULL)
  BUGCONT (action2->setpsi2 (new CNbCalcul (0.5, U_, projet.getDecimales ())),
           -1,
           NULL)
  BUGCONT (projet.unref (), -1, NULL)
  BUGCONT (projet.undo (), -1, NULL)
  
  BUGCONT (projet.ref (), -1, NULL)
  
  std::unique_ptr <std::string> nom (new std::string (projet.getParametres ()->
                                                      getpsiDescription (22)));
  action.reset (new CAction (nom.get (),
                             22,
                             projet));
  //Ici, il y a un traitement volontaire de l'erreur.
  if (!projet.addAction (action.get ()))
  {
    action.reset ();
    nom.reset ();
    projet.rollback ();
  }
  else
    BUGCONT (NULL, -1, NULL)
  
  
  BUGCONT (projet.getParametres ()->getpsiDescription (22).empty (), -1, NULL)
  BUGCONT (projet.getActionCount () == 22, -1, NULL)
  BUGCONT (projet.undo (), -1, NULL)
  BUGCONT (projet.getActionCount () == 0, -1, NULL)
  BUGCONT (projet.redo (), -1, NULL)
  BUGCONT (projet.getActionCount () == 22, -1, NULL)
  BUGCONT (projet.getEtat () == UNDO_NONE, -1, NULL)
  
  BUGCONT (projet.enregistre ("test.xml"), -1, NULL)
  
  // Création des groupes d'actions
/*  BUGCONT(_1990_groupe_ajout_niveau(projet), -1)
  BUGCONT(_1990_groupe_ajout_niveau(projet), -1)
  BUGCONT(_1990_groupe_ajout_niveau(projet), -1)
  BUGCONT(_1990_groupe_ajout_niveau(projet), -1)
  BUGCONT(_1990_groupe_ajout_groupe(projet, 0, GROUPE_COMBINAISON_AND, _1990_action_type_bat_txt(0, projet->parametres.pays)), -1) // Charges permanentes
  BUGCONT(_1990_groupe_ajout_groupe(projet, 0, GROUPE_COMBINAISON_AND, _1990_action_type_bat_txt(3, projet->parametres.pays)), -1) // Charges d'exploitation
  BUGCONT(_1990_groupe_ajout_groupe(projet, 0, GROUPE_COMBINAISON_XOR, _1990_action_type_bat_txt(14, projet->parametres.pays)), -1) // Neige
  BUGCONT(_1990_groupe_ajout_groupe(projet, 0, GROUPE_COMBINAISON_XOR, _1990_action_type_bat_txt(17, projet->parametres.pays)), -1) // Vent
  BUGCONT(_1990_groupe_ajout_groupe(projet, 1, GROUPE_COMBINAISON_AND, _1990_action_type_bat_txt(0, projet->parametres.pays)), -1) // Charges permanentes
  BUGCONT(_1990_groupe_ajout_groupe(projet, 1, GROUPE_COMBINAISON_AND, _1990_action_type_bat_txt(3, projet->parametres.pays)), -1) // Charges d'exploitation
  BUGCONT(_1990_groupe_ajout_groupe(projet, 1, GROUPE_COMBINAISON_OR, "Charges climatiques"), -1) // Charges climatiques
  BUGCONT(_1990_groupe_ajout_groupe(projet, 2, GROUPE_COMBINAISON_AND, "Charges permanentes"), -1) // Tout
  BUGCONT(_1990_groupe_ajout_groupe(projet, 2, GROUPE_COMBINAISON_OR, "Charges variables"), -1) // Tout
  BUGCONT(_1990_groupe_ajout_groupe(projet, 3, GROUPE_COMBINAISON_AND, "Combinaisons complètes"), -1) // Tout
  BUGCONT(_1990_groupe_ajout_element(projet, 0, 0, 0), -1)
  BUGCONT(_1990_groupe_ajout_element(projet, 0, 1, 1), -1)
  BUGCONT(_1990_groupe_ajout_element(projet, 0, 2, 2), -1)
  BUGCONT(_1990_groupe_ajout_element(projet, 0, 3, 3), -1)
  BUGCONT(_1990_groupe_ajout_element(projet, 1, 0, 0), -1)
  BUGCONT(_1990_groupe_ajout_element(projet, 1, 1, 1), -1)
  BUGCONT(_1990_groupe_ajout_element(projet, 1, 2, 2), -1)
  BUGCONT(_1990_groupe_ajout_element(projet, 1, 2, 3), -1)
  BUGCONT(_1990_groupe_ajout_element(projet, 2, 0, 0), -1)
  BUGCONT(_1990_groupe_ajout_element(projet, 2, 1, 1), -1)
  BUGCONT(_1990_groupe_ajout_element(projet, 2, 1, 2), -1)
  BUGCONT(_1990_groupe_ajout_element(projet, 3, 0, 0), -1)
  BUGCONT(_1990_groupe_ajout_element(projet, 3, 0, 1), -1)
  
  // Création de l'appui
  BUGCONT(EF_appuis_ajout(projet, "Rotule", EF_APPUI_BLOQUE, EF_APPUI_BLOQUE, EF_APPUI_BLOQUE, EF_APPUI_BLOQUE, EF_APPUI_LIBRE, EF_APPUI_LIBRE), -1)
  BUGCONT(EF_appuis_ajout(projet, "Encastrement", EF_APPUI_BLOQUE, EF_APPUI_BLOQUE, EF_APPUI_BLOQUE, EF_APPUI_BLOQUE, EF_APPUI_BLOQUE, EF_APPUI_BLOQUE), -1)
  BUGCONT(EF_appuis_ajout(projet, "Appui simple x", EF_APPUI_BLOQUE, EF_APPUI_LIBRE, EF_APPUI_LIBRE, EF_APPUI_LIBRE, EF_APPUI_LIBRE, EF_APPUI_LIBRE), -1)
  BUGCONT(EF_appuis_ajout(projet, "Appui simple z", EF_APPUI_LIBRE, EF_APPUI_LIBRE, EF_APPUI_BLOQUE, EF_APPUI_LIBRE, EF_APPUI_LIBRE, EF_APPUI_LIBRE), -1)
  
  // Création des noeuds
  BUGCONT(EF_noeuds_ajout_noeud_libre(projet, common_math_f(0.0, FLOTTANT_UTILISATEUR), common_math_f(0.0, FLOTTANT_UTILISATEUR), common_math_f(0.0, FLOTTANT_UTILISATEUR), EF_appuis_cherche_nom(projet, "Encastrement", TRUE), NULL), -1)
  BUGCONT(EF_noeuds_ajout_noeud_libre(projet, common_math_f(0.0, FLOTTANT_UTILISATEUR), common_math_f(0.0, FLOTTANT_UTILISATEUR), common_math_f(5.0, FLOTTANT_UTILISATEUR), EF_appuis_cherche_nom(projet, "Appui simple x", TRUE), NULL), -1)
  BUGCONT(EF_noeuds_ajout_noeud_libre(projet, common_math_f(4.0, FLOTTANT_UTILISATEUR), common_math_f(0.0, FLOTTANT_UTILISATEUR), common_math_f(5.0, FLOTTANT_UTILISATEUR), NULL, NULL), -1)
  BUGCONT(EF_noeuds_ajout_noeud_libre(projet, common_math_f(4.0, FLOTTANT_UTILISATEUR), common_math_f(0.0, FLOTTANT_UTILISATEUR), common_math_f(0.0, FLOTTANT_UTILISATEUR), EF_appuis_cherche_nom(projet, "Rotule", TRUE), NULL), -1)
  BUGCONT(EF_noeuds_ajout_noeud_libre(projet, common_math_f(-2.0, FLOTTANT_UTILISATEUR), common_math_f(0.0, FLOTTANT_UTILISATEUR), common_math_f(2.0, FLOTTANT_UTILISATEUR), EF_appuis_cherche_nom(projet, "Appui simple z", TRUE), EF_noeuds_cherche_numero(projet, 3, TRUE)), -1)
  
  // Création des sections en béton
  BUGCONT(EF_sections_rectangulaire_ajout(projet, "Rect_0.3*0.5ht", common_math_f(0.3, FLOTTANT_UTILISATEUR), common_math_f(0.5, FLOTTANT_UTILISATEUR)), -1)
  BUGCONT(EF_sections_T_ajout(projet, "T_1.00*0.20*0.10*0.50", common_math_f(1.00, FLOTTANT_UTILISATEUR), common_math_f(0.20, FLOTTANT_UTILISATEUR), common_math_f(0.10, FLOTTANT_UTILISATEUR), common_math_f(0.50, FLOTTANT_UTILISATEUR)), -1)
  BUGCONT(EF_sections_carree_ajout(projet, "Carre_0.50", common_math_f(0.5, FLOTTANT_UTILISATEUR)), -1)
  BUGCONT(EF_sections_circulaire_ajout(projet, "Circulaire_0.05", common_math_f(0.05, FLOTTANT_UTILISATEUR)), -1)
  BUGCONT(EF_sections_rectangulaire_ajout(projet, "Rect_0.3*0.1ht", common_math_f(0.3, FLOTTANT_UTILISATEUR), common_math_f(0.1, FLOTTANT_UTILISATEUR)), -1)
  BUGCONT(EF_sections_T_ajout(projet, "T_0.20*0.60*0.30*0.20", common_math_f(0.20, FLOTTANT_UTILISATEUR), common_math_f(0.60, FLOTTANT_UTILISATEUR), common_math_f(0.30, FLOTTANT_UTILISATEUR), common_math_f(0.20, FLOTTANT_UTILISATEUR)), -1)
  BUGCONT(EF_sections_carree_ajout(projet, "Carre_0.20", common_math_f(0.2, FLOTTANT_UTILISATEUR)), -1)
  BUGCONT(EF_sections_circulaire_ajout(projet, "Circulaire_0.4", common_math_f(0.4, FLOTTANT_UTILISATEUR)), -1)
  
  // Création du matériau béton
  BUGCONT(_1992_1_1_materiaux_ajout(projet, "B20", common_math_f(20., FLOTTANT_UTILISATEUR)), -1)
  BUGCONT(_1992_1_1_materiaux_ajout(projet, "B30", common_math_f(30., FLOTTANT_UTILISATEUR)), -1)
  BUGCONT(_1992_1_1_materiaux_ajout(projet, "B40", common_math_f(40., FLOTTANT_UTILISATEUR)), -1)
  BUGCONT(_1992_1_1_materiaux_ajout(projet, "B50", common_math_f(50., FLOTTANT_UTILISATEUR)), -1)
  BUGCONT(_1992_1_1_materiaux_ajout(projet, "B60", common_math_f(60., FLOTTANT_UTILISATEUR)), -1)
  BUGCONT(_1992_1_1_materiaux_ajout(projet, "B25", common_math_f(25., FLOTTANT_UTILISATEUR)), -1)
  BUGCONT(_1992_1_1_materiaux_ajout(projet, "B35", common_math_f(35., FLOTTANT_UTILISATEUR)), -1)
  BUGCONT(_1992_1_1_materiaux_ajout(projet, "B45", common_math_f(45., FLOTTANT_UTILISATEUR)), -1)
  BUGCONT(_1992_1_1_materiaux_ajout(projet, "B55", common_math_f(55., FLOTTANT_UTILISATEUR)), -1)
  BUGCONT(_1993_1_1_materiaux_ajout(projet, "S235", common_math_f(235., FLOTTANT_UTILISATEUR), common_math_f(360., FLOTTANT_UTILISATEUR)), -1)
  
  // Création du relâchment
  ry_d = (EF_Relachement_Donnees_Elastique_Lineaire*)malloc(sizeof(EF_Relachement_Donnees_Elastique_Lineaire));
  ry_d->raideur = common_math_f(3000000., FLOTTANT_UTILISATEUR);
  rz_d = (EF_Relachement_Donnees_Elastique_Lineaire*)malloc(sizeof(EF_Relachement_Donnees_Elastique_Lineaire));
  rz_d->raideur =  common_math_f(400000., FLOTTANT_UTILISATEUR);
  ry_f = (EF_Relachement_Donnees_Elastique_Lineaire*)malloc(sizeof(EF_Relachement_Donnees_Elastique_Lineaire));
  ry_f->raideur = common_math_f(2300000., FLOTTANT_UTILISATEUR);
  rz_f = (EF_Relachement_Donnees_Elastique_Lineaire*)malloc(sizeof(EF_Relachement_Donnees_Elastique_Lineaire));
  rz_f->raideur =  common_math_f(340000., FLOTTANT_UTILISATEUR);
  BUGCONT(EF_relachement_ajout(projet, "Articulation", EF_RELACHEMENT_BLOQUE, NULL, EF_RELACHEMENT_BLOQUE, NULL, EF_RELACHEMENT_BLOQUE, NULL, EF_RELACHEMENT_LIBRE, NULL, EF_RELACHEMENT_LIBRE, NULL, EF_RELACHEMENT_LIBRE, NULL), -1)
  BUGCONT(EF_relachement_ajout(projet, "Rotation nulle", EF_RELACHEMENT_LIBRE, NULL, EF_RELACHEMENT_LIBRE, NULL, EF_RELACHEMENT_LIBRE, NULL, EF_RELACHEMENT_BLOQUE, NULL, EF_RELACHEMENT_BLOQUE, NULL, EF_RELACHEMENT_BLOQUE, NULL), -1)
  BUGCONT(EF_relachement_ajout(projet, "Partiel", EF_RELACHEMENT_BLOQUE, NULL, EF_RELACHEMENT_ELASTIQUE_LINEAIRE, ry_d, EF_RELACHEMENT_ELASTIQUE_LINEAIRE, rz_d, EF_RELACHEMENT_BLOQUE, NULL, EF_RELACHEMENT_ELASTIQUE_LINEAIRE, ry_f, EF_RELACHEMENT_ELASTIQUE_LINEAIRE, rz_f), -1)
  
  // Création de l'élément en béton
  BUGCONT(_1992_1_1_barres_ajout(projet, BETON_ELEMENT_POUTRE, EF_sections_cherche_nom(projet, "Rect_0.3*0.5ht", TRUE), EF_materiaux_cherche_nom(projet, "B25", TRUE), 0, 1, common_math_f(0., FLOTTANT_UTILISATEUR), NULL, 1), -1)
  BUGCONT(_1992_1_1_barres_ajout(projet, BETON_ELEMENT_POUTRE, EF_sections_cherche_nom(projet, "T_1.00*0.20*0.10*0.50", TRUE), EF_materiaux_cherche_nom(projet, "B25", TRUE), 1, 2, common_math_f(0., FLOTTANT_UTILISATEUR), EF_relachement_cherche_nom(projet, "Articulation", TRUE), 2), -1)
  BUGCONT(_1992_1_1_barres_ajout(projet, BETON_ELEMENT_POUTRE, EF_sections_cherche_nom(projet, "Carre_0.50", TRUE), EF_materiaux_cherche_nom(projet, "B25", TRUE), 2, 3, common_math_f(0., FLOTTANT_UTILISATEUR), NULL, 3), -1)
  BUGCONT(_1992_1_1_barres_ajout(projet, BETON_ELEMENT_POUTRE, EF_sections_cherche_nom(projet, "Circulaire_0.05", TRUE), EF_materiaux_cherche_nom(projet, "B25", TRUE), 3, 4, common_math_f(0., FLOTTANT_UTILISATEUR), NULL, 0), -1)
  
  // Ajout de l'action ponctuelle
  BUGCONT(tmp1 = common_selection_renvoie_numeros("1;3-5"), -1)
  BUGCONT(tmp2 = common_selection_converti_numeros_en_noeuds(tmp1, projet), -1)
  BUGCONT(EF_charge_noeud_ajout(projet, 0, tmp2, 1000., 500., 1000., 3000., 5000., 5000., "test1"), -1)
  g_list_free(tmp1);
  BUGCONT(tmp1 = common_selection_renvoie_numeros("2"), -1)
  BUGCONT(tmp2 = common_selection_converti_numeros_en_noeuds(tmp1, projet), -1)
  BUGCONT(EF_charge_noeud_ajout(projet, 0, tmp2, common_math_f(0., FLOTTANT_UTILISATEUR), common_math_f(0., FLOTTANT_UTILISATEUR), common_math_f(0., FLOTTANT_UTILISATEUR), common_math_f(0., FLOTTANT_UTILISATEUR), common_math_f(0., FLOTTANT_UTILISATEUR), common_math_f(0., FLOTTANT_UTILISATEUR), "test1"), -1)
  g_list_free(tmp1);
  BUGCONT(tmp1 = common_selection_renvoie_numeros("1"), -1)
  BUGCONT(tmp2 = common_selection_converti_numeros_en_barres(tmp1, projet), -1)
  BUGCONT(EF_charge_barre_ponctuelle_ajout(projet, 2, tmp2, FALSE, common_math_f(1.0, FLOTTANT_UTILISATEUR), common_math_f(0., FLOTTANT_UTILISATEUR), common_math_f(0., FLOTTANT_UTILISATEUR), common_math_f(-10000., FLOTTANT_UTILISATEUR), common_math_f(0., FLOTTANT_UTILISATEUR), common_math_f(0., FLOTTANT_UTILISATEUR), common_math_f(0., FLOTTANT_UTILISATEUR), "test2"), -1)
  BUGCONT(tmp2 = common_selection_converti_numeros_en_barres(tmp1, projet), -1)
  BUGCONT(EF_charge_barre_repartie_uniforme_ajout(projet, 3, tmp2, FALSE, FALSE, common_math_f(0., FLOTTANT_UTILISATEUR), common_math_f(0., FLOTTANT_UTILISATEUR), common_math_f(0., FLOTTANT_UTILISATEUR), common_math_f(0., FLOTTANT_UTILISATEUR), common_math_f(-10000., FLOTTANT_UTILISATEUR), common_math_f(0., FLOTTANT_UTILISATEUR), common_math_f(0., FLOTTANT_UTILISATEUR), common_math_f(0., FLOTTANT_UTILISATEUR), "test3"), -1)
  g_list_free(tmp1);
  BUGCONT(tmp1 = common_selection_renvoie_numeros("0-2/2"), -1)
  BUGCONT(tmp2 = common_selection_converti_numeros_en_barres(tmp1, projet), -1)
  BUGCONT(EF_charge_barre_repartie_uniforme_ajout(projet, 0, tmp2, FALSE, FALSE, 1.5, 1.0, 10000., 9000., 8000., 7000., 6000., 5000., "test3"), -1)
  g_list_free(tmp1);*/
  
  // Affichage de l'interface graphique
//  gtk_widget_show_all (projet->ui.comp.window);
//  gtk_main ();
  
  return 0;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
