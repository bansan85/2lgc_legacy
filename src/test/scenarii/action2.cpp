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

#include <stdint.h>
#include <libintl.h>
#include <string.h>
#include <memory>
#include <iostream>
#include <cassert>

#include "codegui.hpp"
#include "Math.hpp"

int
main (int32_t,
      char  *[])
{
  CProjet projet (ENorme::EUROCODE);
  std::shared_ptr <CAction> action;
  CAction *action_;
  std::shared_ptr <INb> nb0, nb1, nb2;
  
  // On charge la localisation
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE_NAME, LOCALEDIR);
  bind_textdomain_codeset (PACKAGE_NAME, "UTF-8");
  textdomain (PACKAGE_NAME);
  
  assert (projet.getActionCount () == 0);
  // 2 Exploitation
  action = std::make_shared <CAction> (std::make_shared <std::string> (
                                                                 "Chargement"),
                                       2,
                                       projet);
  assert (projet.addAction (action));
  action_ = projet.getAction ("Chargement");
  assert (action_ != NULL);
  assert (projet.getActionCount () == 1);

  nb0 = std::make_shared <NbUser> (0.0, EUnite::U_);
  nb1 = std::make_shared <NbUser> (0.1, EUnite::U_);
  nb2 = std::make_shared <NbUser> (0.2, EUnite::U_);

  assert (action_->setPsi (0, nb0));
  assert (action_->setPsi (1, nb1));
  assert (action_->setPsi (2, nb2));

  assert (projet.getActionCount () == 1);

  assert (doublesAreEqual (action_->getPsi (0)->getVal (), 0.0, 1., ERR_REL));
  assert (doublesAreEqual (action_->getPsi (1)->getVal (), 0.1, 1., ERR_REL));
  assert (doublesAreEqual (action_->getPsi (2)->getVal (), 0.2, 1., ERR_REL));

  assert (projet.undoDesc (0)->compare (
                    "Cœfficient ψ₂ de l'action “Chargement” (0,200000)") == 0);
  assert (projet.undo ());
  assert (doublesAreEqual (action_->getPsi (0)->getVal (), 0.0, 1., ERR_REL));
  assert (doublesAreEqual (action_->getPsi (1)->getVal (), 0.1, 1., ERR_REL));
  assert (doublesAreEqual (action_->getPsi (2)->getVal (), 0.3, 1., ERR_REL));

  assert (projet.undoDesc (0)->compare (
                    "Cœfficient ψ₁ de l'action “Chargement” (0,100000)") == 0);
  assert (projet.undo ());
  assert (doublesAreEqual (action_->getPsi (0)->getVal (), 0.0, 1., ERR_REL));
  assert (doublesAreEqual (action_->getPsi (1)->getVal (), 0.5, 1., ERR_REL));
  assert (doublesAreEqual (action_->getPsi (2)->getVal (), 0.3, 1., ERR_REL));
  
  assert (projet.undoDesc (0)->compare (
                    "Cœfficient ψ₀ de l'action “Chargement” (0,000000)") == 0);
  assert (projet.undo ());
  assert (doublesAreEqual (action_->getPsi (0)->getVal (), 0.7, 1., ERR_REL));
  assert (doublesAreEqual (action_->getPsi (1)->getVal (), 0.5, 1., ERR_REL));
  assert (doublesAreEqual (action_->getPsi (2)->getVal (), 0.3, 1., ERR_REL));
  
  assert (projet.redoDesc (0)->compare (
                    "Cœfficient ψ₀ de l'action “Chargement” (0,000000)") == 0);
  assert (projet.redo ());
  assert (doublesAreEqual (action_->getPsi (0)->getVal (), 0.0, 1., ERR_REL));
  assert (doublesAreEqual (action_->getPsi (1)->getVal (), 0.5, 1., ERR_REL));
  assert (doublesAreEqual (action_->getPsi (2)->getVal (), 0.3, 1., ERR_REL));
  
  assert (projet.redoDesc (0)->compare (
                    "Cœfficient ψ₁ de l'action “Chargement” (0,100000)") == 0);
  assert (projet.redo ());
  assert (doublesAreEqual (action_->getPsi (0)->getVal (), 0.0, 1., ERR_REL));
  assert (doublesAreEqual (action_->getPsi (1)->getVal (), 0.1, 1., ERR_REL));
  assert (doublesAreEqual (action_->getPsi (2)->getVal (), 0.3, 1., ERR_REL));
  
  assert (projet.redoDesc (0)->compare (
                    "Cœfficient ψ₂ de l'action “Chargement” (0,200000)") == 0);
  assert (projet.redo ());
  assert (doublesAreEqual (action_->getPsi (0)->getVal (), 0.0, 1., ERR_REL));
  assert (doublesAreEqual (action_->getPsi (1)->getVal (), 0.1, 1., ERR_REL));
  assert (doublesAreEqual (action_->getPsi (2)->getVal (), 0.2, 1., ERR_REL));
  
  assert (projet.enregistre ("action2.xml"));
  
  return 0;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
