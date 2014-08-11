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

// Test les fonctionnalités de base de gestionnaire d'annulation.

#include "config.h"

#include <stdint.h>
#include <libintl.h>
#include <string>
#include <iostream>

#include "codegui.hpp"
#include "MErreurs.hh"

int
main (int32_t argc,
      char   *argv[])
{
  CProjet projet (NORME_EC);
  std::unique_ptr <CAction> action;
  
  // On charge la localisation
  setlocale (LC_ALL, "" );
  bindtextdomain (PACKAGE, LOCALEDIR);
  bind_textdomain_codeset (PACKAGE, "UTF-8");
  textdomain (PACKAGE);
  
  std::cout << projet.getActionCount () << "\n";
  BUGCONT (projet.getActionCount () == 0, -1, NULL)
  // 0 Poids propre
  action.reset (new CAction ("Poids propre",
                             0,
                             dynamic_cast <CUndoManager &> (projet)));
  BUGCONT (projet.addAction (action.get ()), -1, NULL)
  action.release ();
  BUGCONT (projet.getActionCount () == 1, -1, NULL)
  // 2 Exploitation
  action.reset (new CAction ("Chargement",
                             2,
                             dynamic_cast <CUndoManager &> (projet)));
  BUGCONT (projet.addAction (action.get ()), -1, NULL)
  action.release ();
  BUGCONT (projet.getActionCount () == 2, -1, NULL)
  // 18 Neige
  action.reset (new CAction ("Neige",
                             18,
                             dynamic_cast <CUndoManager &> (projet)));
  BUGCONT (projet.addAction (action.get ()), -1, NULL)
  action.release ();
  BUGCONT (projet.getActionCount () == 3, -1, NULL)
  // 19 Vent
  action.reset (new CAction ("Vent",
                             19,
                             dynamic_cast <CUndoManager &> (projet)));
  BUGCONT (projet.addAction (action.get ()), -1, NULL)
  action.release ();
  BUGCONT (projet.getActionCount () == 4, -1, NULL)
  BUGCONT (projet.undo (), -1, NULL)
  BUGCONT (projet.getActionCount () == 3, -1, NULL)
  BUGCONT (projet.undo (), -1, NULL)
  BUGCONT (projet.getActionCount () == 2, -1, NULL)
  BUGCONT (projet.undo (), -1, NULL)
  BUGCONT (projet.getActionCount () == 1, -1, NULL)
  BUGCONT (projet.undo (), -1, NULL)
  BUGCONT (projet.getActionCount () == 0, -1, NULL)
  BUGCONT (projet.redo (), -1, NULL)
  BUGCONT (projet.getActionCount () == 1, -1, NULL)
  BUGCONT (projet.redo (), -1, NULL)
  BUGCONT (projet.getActionCount () == 2, -1, NULL)
  BUGCONT (projet.redo (), -1, NULL)
  BUGCONT (projet.getActionCount () == 3, -1, NULL)
  BUGCONT (projet.redo (), -1, NULL)
  BUGCONT (projet.getActionCount () == 4, -1, NULL)
  
  BUGCONT (projet.enregistre ("test.xml"), -1, NULL)
  
  return 0;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
