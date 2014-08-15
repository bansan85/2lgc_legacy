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

// Test les fonctions de base d'une classe CAction.

#include "config.h"

#include "codegui.hpp"
#include "MErreurs.hh"

#include <stdint.h>
#include <libintl.h>
#include <string.h>


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
  
  BUGCONT (projet.getActionCount () == 0, -1, NULL)
  // 0 Poids propre
  action.reset (new CAction (new std::string ("Poids propre"),
                             0,
                             dynamic_cast <CUndoManager &> (projet)));
  BUGCONT (projet.addAction (action.get ()), -1, NULL)
  action.release ();
  BUGCONT (projet.getActionCount () == 1, -1, NULL)
  // 2 Exploitation
  action.reset (new CAction (new std::string ("Chargement"),
                             2,
                             dynamic_cast <CUndoManager &> (projet)));
  BUGCONT (projet.addAction (action.get ()), -1, NULL)
  action.release ();
  BUGCONT (projet.getActionCount () == 2, -1, NULL)
  // 18 Neige
  action.reset (new CAction (new std::string ("Neige"),
                             18,
                             dynamic_cast <CUndoManager &> (projet)));
  BUGCONT (projet.addAction (action.get ()), -1, NULL)
  action.release ();
  BUGCONT (projet.getActionCount () == 3, -1, NULL)
  // 19 Vent
  action.reset (new CAction (new std::string ("Vent"),
                             19,
                             dynamic_cast <CUndoManager &> (projet)));
  BUGCONT (projet.addAction (action.get ()), -1, NULL)
  action.release ();
  
  BUGCONT (projet.enregistre ("test.xml"), -1, NULL)
  
  return 0;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
