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
#include <memory>
#include <cassert>

#include "CModele.hpp"

int
main (int32_t,
      char   *[])
{
  CModele projet (ENorme::EUROCODE);
  std::shared_ptr <POCO::sol::CAction> action;
  bool retour;
  
  projet.getUndoManager ().setSauveDesc (2);
  retour = projet.getUndoManager ().ref ();
  assert (retour);
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
  retour = projet.getUndoManager ().unref ();
  assert (retour);

  assert (projet.getUndoManager ().undoDesc (0)->compare (
                                           "Ajout de l'action “Poids propre”\n"
                                           "Ajout de l'action “Chargement”\n"
                                           "Ajout de l'action “Neige”\n"
                                           "Ajout de l'action “Vent”") == 0);
  assert (projet.getUndoManager ().undoDesc (1)->compare (
                                      "Paramètres du projet (Eurocode)") == 0);

  retour = projet.enregistre ("undomanager5.xml");
  assert (retour);
  
  return 0;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
