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
#include <cassert>

#include "CModele.hpp"
#include "POCO/nombre/Utilisateur.hpp"

int
main (int32_t,
      char   *[])
{
  CModele projet (ENorme::EUROCODE);
  bool retour;
  
  retour = projet.getUndoManager ().ref ();
  assert (retour);
  
  std::shared_ptr <POCO::sol::CAction> action;
  for (uint8_t i = 0; i < 22; i++)
  {
    action = std::make_shared <POCO::sol::CAction> (
               std::make_shared <std::string> (
                               projet.getNorme ()->getPsiDescription (i)),
               0);
    
    retour = projet.fAction.doAdd (action);
    assert (retour);
  }
  
  assert (projet.getUndoManager ().getEtat () == EUndoEtat::MODIF);
  retour = projet.getUndoManager ().unref ();
  assert (retour);
  
  retour = projet.getUndoManager ().ref ();
  assert (retour);
  std::shared_ptr <POCO::sol::CAction> *action2;
  action2 = projet.getAction (projet.getNorme ()->getPsiDescription (5));
  assert (action2);
  action = *action2;
  action->getPsi (0)->getVal ();
  action->getPsi (1)->getUnite ();
  action->getPsi (2)->toString (projet.getPreferences ().getDecimales ());
  retour = projet.fAction.doSetPsi (action, 0, std::make_shared <POCO::nombre::Utilisateur> (0.5, EUnite::U_));
  assert (retour);
  retour = projet.fAction.doSetPsi (action, 1, std::make_shared <POCO::nombre::Utilisateur> (0.5, EUnite::U_));
  assert (retour);
  retour = projet.fAction.doSetPsi (action, 2, std::make_shared <POCO::nombre::Utilisateur> (0.5, EUnite::U_));
  assert (retour);
  action->getPsi (0)->getVal ();
  action->getPsi (1)->getUnite ();
  action->getPsi (2)->toString (projet.getPreferences ().getDecimales ());
  retour = projet.getUndoManager ().unref ();
  assert (retour);
  retour = projet.getUndoManager ().undo ();
  assert (retour);
  retour = projet.getUndoManager ().redo ();
  assert (retour);
  
  retour = projet.getUndoManager ().ref ();
  assert (retour);
  
  std::unique_ptr <std::string> nom (new std::string (projet.getNorme ()->
                                                      getPsiDescription (22)));
  action = std::make_shared <POCO::sol::CAction> (
             std::make_shared <std::string> (
                              projet.getNorme ()->getPsiDescription (22)),
             22);
  // Ici, il y a un traitement volontaire de l'erreur.
  if (!projet.fAction.doAdd (action))
  {
    action = nullptr;
    nom = nullptr;
    projet.getUndoManager ().rollback ();
  }
  else
    assert (false);
  
  assert (projet.getNorme ()->getPsiDescription (22).empty ());
  assert (projet.getActionCount () == 22);
  retour = projet.getUndoManager ().undo ();
  assert (retour);
  retour = projet.getUndoManager ().undo ();
  assert (retour);
  assert (projet.getActionCount () == 0);
  retour = projet.getUndoManager ().redo ();
  assert (retour);
  retour = projet.getUndoManager ().redo ();
  assert (retour);
  assert (projet.getActionCount () == 22);
  assert (projet.getUndoManager ().getEtat () == EUndoEtat::NONE_OR_REVERT);
  
  retour = projet.getUndoManager ().undo ();
  assert (retour);
  
  retour = projet.enregistre ("coverage.xml");
  assert (retour);
  
  return 0;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
