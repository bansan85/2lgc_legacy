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

#include "CModele.hpp"

int
main ()
{
  /* Variables */
  CModele projet (ENorme::EUROCODE);
  std::shared_ptr <POCO::sol::CAction> action;
  bool retour;
  
  assert (projet.getActionCount () == 0);
  // 0 Poids propre
  action = std::make_shared <POCO::sol::CAction> (
                           std::make_shared <std::string> ("Poids propre"), 0);
  retour = projet.fAction.doAdd (action);
  assert (retour);
  assert (projet.getActionCount () == 1);
  // 2 Exploitation
  action = std::make_shared <POCO::sol::CAction> (
                             std::make_shared <std::string> ("Chargement"), 2);
  retour = projet.fAction.doAdd (action);
  assert (retour);
  assert (projet.getActionCount () == 2);
  // 18 Neige
  action = std::make_shared <POCO::sol::CAction> (
                                 std::make_shared <std::string> ("Neige"), 18);
  retour = projet.fAction.doAdd (action);
  assert (retour);
  assert (projet.getActionCount () == 3);
  // 19 Vent
  action = std::make_shared <POCO::sol::CAction> (
                                  std::make_shared <std::string> ("Vent"), 19);
  retour = projet.fAction.doAdd (action);
  assert (retour);
  assert (projet.getActionCount () == 4);
  retour = projet.getUndoManager ().undo ();
  assert (retour);
  assert (projet.getActionCount () == 3);
  retour = projet.getUndoManager ().undo ();
  assert (retour);
  assert (projet.getActionCount () == 2);
  retour = projet.getUndoManager ().undo ();
  assert (retour);
  assert (projet.getActionCount () == 1);
  retour = projet.getUndoManager ().undo ();
  assert (retour);
  assert (projet.getActionCount () == 0);
  
  retour = projet.enregistre ("test.xml");
  assert (retour);
  
  return 0;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
