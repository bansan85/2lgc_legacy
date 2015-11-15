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
  /* Variables */
  CModele projet (ENorme::EUROCODE);
//  GList   *tmp1, *tmp2;
  
//  EF_Relachement_Donnees_Elastique_Lineaire *ry_d, *rz_d, *ry_f, *rz_f;
  
  // Initialisation de GTK+, gtk doit être initialisé avant m3dlib.
/*  INFO (gtk_init_check (&argc, &argv),
        -1,
        (gettext ("Impossible d'initialiser gtk.\n")); )*/
  
  assert (projet.getUndoManager ().ref ());
  
  std::shared_ptr <POCO::sol::CAction> action;
  for (uint8_t i = 0; i < 22; i++)
  {
    action = std::make_shared <POCO::sol::CAction> (
               std::make_shared <std::string> (
                               projet.getNorme ()->getPsiDescription (i)),
               0);
    
    assert (projet.fAction.doAdd (action));
  }
  
  assert (projet.getUndoManager ().getEtat () == EUndoEtat::MODIF);
  assert (projet.getUndoManager ().unref ());
  
  assert (projet.getUndoManager ().ref ());
  std::shared_ptr <POCO::sol::CAction> *action2;
  assert (action2 = projet.getAction (projet.getNorme ()->
                                                       getPsiDescription (5)));
  assert (action2);
  action = *action2;
  action->getPsi (0)->getVal ();
  action->getPsi (1)->getUnite ();
  action->getPsi (2)->toString (projet.getPreferences ().getDecimales ());
  assert (projet.fAction.doSetPsi (action, 0, std::make_shared <POCO::nombre::Utilisateur> (0.5, EUnite::U_)));
  assert (projet.fAction.doSetPsi (action, 1, std::make_shared <POCO::nombre::Utilisateur> (0.5, EUnite::U_)));
  assert (projet.fAction.doSetPsi (action, 2, std::make_shared <POCO::nombre::Utilisateur> (0.5, EUnite::U_)));
  action->getPsi (0)->getVal ();
  action->getPsi (1)->getUnite ();
  action->getPsi (2)->toString (projet.getPreferences ().getDecimales ());
  assert (projet.getUndoManager ().unref ());
  assert (projet.getUndoManager ().undo ());
  assert (projet.getUndoManager ().redo ());
  
  assert (projet.getUndoManager ().ref ());
  
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
  assert (projet.getUndoManager ().undo ());
  assert (projet.getUndoManager ().undo ());
  assert (projet.getActionCount () == 0);
  assert (projet.getUndoManager ().redo ());
  assert (projet.getUndoManager ().redo ());
  assert (projet.getActionCount () == 22);
  assert (projet.getUndoManager ().getEtat () == EUndoEtat::NONE_OR_REVERT);
  
  assert (projet.getUndoManager ().undo ());
/*  assert (projet.getUndoManager ().ref ());
  assert (projet.getNorme ()->setNom (std::make_shared <std::string>
                                                                     ("nom")));
  assert (projet.getNorme ()->setVariante (0));
  assert (projet.getUndoManager ().unref ());*/
  
  assert (projet.enregistre ("coverage.xml"));
  
  return 0;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
