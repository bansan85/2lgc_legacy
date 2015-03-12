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

#include <stdint.h>
#include <libintl.h>
#include <string.h>
#include <memory>

#include "codegui.hpp"
#include "MErreurs.hpp"

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
  CProjet projet (ENorme::NORME_EC);
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
  
  BUGCONT (projet.ref (), -1, static_cast <CUndoManager *> (nullptr))
  
  std::unique_ptr <CAction> action;
  for (uint8_t i = 0; i < 22; i++)
  {
    action.reset (new CAction (new std::string (projet.getParametres ()->
                                                        getpsiDescription (i)),
                               0,
                               projet));
    
    BUGCONT (projet.addAction (action.get ()), -1, static_cast <CUndoManager *> (nullptr))
    
    action.release ();
  }
  
  BUGCONT (projet.getEtat () == EUndoEtat::UNDO_MODIF, -1, static_cast <CUndoManager *> (nullptr))
  BUGCONT (projet.unref (), -1, static_cast <CUndoManager *> (nullptr))
  
  BUGCONT (projet.ref (), -1, static_cast <CUndoManager *> (nullptr))
  CAction *action2;
  BUGCONT (action2 = projet.getAction (projet.getParametres ()->
                                                        getpsiDescription (5)),
           -1,
           static_cast <CUndoManager *> (nullptr))
  action2->getpsi0 ().getVal ();
  action2->getpsi1 ().getUnite ();
  action2->getpsi2 ().toString ();
  BUGCONT (action2->setpsi0 (new NbUser (0.5, EUnite::U_)), -1, static_cast <CUndoManager *> (nullptr))
  BUGCONT (action2->setpsi1 (new NbUser (0.5, EUnite::U_)), -1, static_cast <CUndoManager *> (nullptr))
  BUGCONT (action2->setpsi2 (new NbUser (0.5, EUnite::U_)), -1, static_cast <CUndoManager *> (nullptr))
  action2->getpsi0 ().getVal ();
  action2->getpsi1 ().getUnite ();
  action2->getpsi2 ().toString ();
  BUGCONT (projet.unref (), -1, static_cast <CUndoManager *> (nullptr))
  BUGCONT (projet.undo (), -1, static_cast <CUndoManager *> (nullptr))
  BUGCONT (projet.redo (), -1, static_cast <CUndoManager *> (nullptr))
  
  BUGCONT (projet.ref (), -1, static_cast <CUndoManager *> (nullptr))
  
  std::unique_ptr <std::string> nom (new std::string (projet.getParametres ()->
                                                      getpsiDescription (22)));
  action.reset (new CAction (nom.get (),
                             22,
                             projet));
  // Ici, il y a un traitement volontaire de l'erreur.
  if (!projet.addAction (action.get ()))
  {
    action.reset ();
    nom.reset ();
    projet.rollback ();
  }
  else
    BUGCONT (static_cast <CUndoManager *> (nullptr), -1, static_cast <CUndoManager *> (nullptr))
  
  BUGCONT (projet.getParametres ()->getpsiDescription (22).empty (), -1, static_cast <CUndoManager *> (nullptr))
  BUGCONT (projet.getActionCount () == 22, -1, static_cast <CUndoManager *> (nullptr))
  BUGCONT (projet.undo (), -1, static_cast <CUndoManager *> (nullptr))
  BUGCONT (projet.undo (), -1, static_cast <CUndoManager *> (nullptr))
  BUGCONT (projet.getActionCount () == 0, -1, static_cast <CUndoManager *> (nullptr))
  BUGCONT (projet.redo (), -1, static_cast <CUndoManager *> (nullptr))
  BUGCONT (projet.redo (), -1, static_cast <CUndoManager *> (nullptr))
  BUGCONT (projet.getActionCount () == 22, -1, static_cast <CUndoManager *> (nullptr))
  BUGCONT (projet.getEtat () == EUndoEtat::UNDO_NONE_OR_REVERT, -1, static_cast <CUndoManager *> (nullptr))
  
  BUGCONT (projet.undo (), -1, static_cast <CUndoManager *> (nullptr))
  BUGCONT (projet.ref (), -1, static_cast <CUndoManager *> (nullptr))
  BUGCONT (projet.getParametres ()->setNom (new std::string ("nom")), -1, static_cast <CUndoManager *> (nullptr))
  BUGCONT (projet.getParametres ()->setVariante (0), -1, static_cast <CUndoManager *> (nullptr))
  BUGCONT (projet.unref (), -1, static_cast <CUndoManager *> (nullptr))
  
  BUGCONT (projet.enregistre ("coverage.xml"), -1, static_cast <CUndoManager *> (nullptr))
  
  return 0;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
