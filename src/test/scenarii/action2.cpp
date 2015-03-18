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
#include "MErreurs.hpp"
#include "Math.hpp"

#include <stdint.h>
#include <libintl.h>
#include <string.h>
#include <memory>
#include <iostream>


int
main (int32_t,
      char  *[])
{
  CProjet projet (ENorme::EUROCODE);
  std::unique_ptr <CAction> action;
  CAction *action_;
  std::unique_ptr <INb> nb0, nb1, nb2;
  
  // On charge la localisation
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  bind_textdomain_codeset (PACKAGE, "UTF-8");
  textdomain (PACKAGE);
  
  BUGCONT (projet.getActionCount () == 0, -1, UNDO_MANAGER_NULL)
  // 2 Exploitation
  action.reset (new CAction (new std::string ("Chargement"), 2, projet));
  BUGCONT (projet.addAction (action.get ()), -1, UNDO_MANAGER_NULL)
  action.release ();
  action_ = projet.getAction ("Chargement");
  BUGCONT (action_ != NULL, -1, UNDO_MANAGER_NULL)
  BUGCONT (projet.getActionCount () == 1, -1, UNDO_MANAGER_NULL)

  nb0.reset (new NbUser (0.0, EUnite::U_));
  nb1.reset (new NbUser (0.1, EUnite::U_));
  nb2.reset (new NbUser (0.2, EUnite::U_));

  BUGCONT (action_->setPsi (0, nb0.get ()), -1, UNDO_MANAGER_NULL)
  nb0.release ();
  BUGCONT (action_->setPsi (1, nb1.get ()), -1, UNDO_MANAGER_NULL)
  nb1.release ();
  BUGCONT (action_->setPsi (2, nb2.get ()), -1, UNDO_MANAGER_NULL)
  nb2.release ();

  BUGCONT (projet.getActionCount () == 1, -1, UNDO_MANAGER_NULL)

  BUGCONT (doublesAreEqual (action_->getPsi (0)->getVal (), 0.0, 1., ERR_REL),
           -1,
           UNDO_MANAGER_NULL)
  BUGCONT (doublesAreEqual (action_->getPsi (1)->getVal (), 0.1, 1., ERR_REL),
           -1,
           UNDO_MANAGER_NULL)
  BUGCONT (doublesAreEqual (action_->getPsi (2)->getVal (), 0.2, 1., ERR_REL),
           -1,
           UNDO_MANAGER_NULL)

  BUGCONT (projet.undoDesc (0)->compare (
                     "Cœfficient ψ₂ de l'action “Chargement” (0,200000)") == 0,
           -1,
           UNDO_MANAGER_NULL);
  BUGCONT (projet.undo (), -1, UNDO_MANAGER_NULL)
  BUGCONT (doublesAreEqual (action_->getPsi (0)->getVal (), 0.0, 1., ERR_REL),
           -1,
           UNDO_MANAGER_NULL)
  BUGCONT (doublesAreEqual (action_->getPsi (1)->getVal (), 0.1, 1., ERR_REL),
           -1,
           UNDO_MANAGER_NULL)
  BUGCONT (doublesAreEqual (action_->getPsi (2)->getVal (), 0.3, 1., ERR_REL),
           -1,
           UNDO_MANAGER_NULL)

  BUGCONT (projet.undoDesc (0)->compare (
                     "Cœfficient ψ₁ de l'action “Chargement” (0,100000)") == 0,
           -1,
           UNDO_MANAGER_NULL);
  BUGCONT (projet.undo (), -1, UNDO_MANAGER_NULL)
  BUGCONT (doublesAreEqual (action_->getPsi (0)->getVal (), 0.0, 1., ERR_REL),
           -1,
           UNDO_MANAGER_NULL)
  BUGCONT (doublesAreEqual (action_->getPsi (1)->getVal (), 0.5, 1., ERR_REL),
           -1,
           UNDO_MANAGER_NULL)
  BUGCONT (doublesAreEqual (action_->getPsi (2)->getVal (), 0.3, 1., ERR_REL),
           -1,
           UNDO_MANAGER_NULL)
  
  BUGCONT (projet.undoDesc (0)->compare (
                     "Cœfficient ψ₀ de l'action “Chargement” (0,000000)") == 0,
           -1,
           UNDO_MANAGER_NULL);
  BUGCONT (projet.undo (), -1, UNDO_MANAGER_NULL)
  BUGCONT (doublesAreEqual (action_->getPsi (0)->getVal (), 0.7, 1., ERR_REL),
           -1,
           UNDO_MANAGER_NULL)
  BUGCONT (doublesAreEqual (action_->getPsi (1)->getVal (), 0.5, 1., ERR_REL),
           -1,
           UNDO_MANAGER_NULL)
  BUGCONT (doublesAreEqual (action_->getPsi (2)->getVal (), 0.3, 1., ERR_REL),
           -1,
           UNDO_MANAGER_NULL)
  
  BUGCONT (projet.redoDesc (0)->compare (
                     "Cœfficient ψ₀ de l'action “Chargement” (0,000000)") == 0,
           -1,
           UNDO_MANAGER_NULL);
  BUGCONT (projet.redo (), -1, UNDO_MANAGER_NULL)
  BUGCONT (doublesAreEqual (action_->getPsi (0)->getVal (), 0.0, 1., ERR_REL),
           -1,
           UNDO_MANAGER_NULL)
  BUGCONT (doublesAreEqual (action_->getPsi (1)->getVal (), 0.5, 1., ERR_REL),
           -1,
           UNDO_MANAGER_NULL)
  BUGCONT (doublesAreEqual (action_->getPsi (2)->getVal (), 0.3, 1., ERR_REL),
           -1,
           UNDO_MANAGER_NULL)
  
  BUGCONT (projet.redoDesc (0)->compare (
                     "Cœfficient ψ₁ de l'action “Chargement” (0,100000)") == 0,
           -1,
           UNDO_MANAGER_NULL);
  BUGCONT (projet.redo (), -1, UNDO_MANAGER_NULL)
  BUGCONT (doublesAreEqual (action_->getPsi (0)->getVal (), 0.0, 1., ERR_REL),
           -1,
           UNDO_MANAGER_NULL)
  BUGCONT (doublesAreEqual (action_->getPsi (1)->getVal (), 0.1, 1., ERR_REL),
           -1,
           UNDO_MANAGER_NULL)
  BUGCONT (doublesAreEqual (action_->getPsi (2)->getVal (), 0.3, 1., ERR_REL),
           -1,
           UNDO_MANAGER_NULL)
  
  BUGCONT (projet.redoDesc (0)->compare (
                     "Cœfficient ψ₂ de l'action “Chargement” (0,200000)") == 0,
           -1,
           UNDO_MANAGER_NULL);
  BUGCONT (projet.redo (), -1, UNDO_MANAGER_NULL)
  BUGCONT (doublesAreEqual (action_->getPsi (0)->getVal (), 0.0, 1., ERR_REL),
           -1,
           UNDO_MANAGER_NULL)
  BUGCONT (doublesAreEqual (action_->getPsi (1)->getVal (), 0.1, 1., ERR_REL),
           -1,
           UNDO_MANAGER_NULL)
  BUGCONT (doublesAreEqual (action_->getPsi (2)->getVal (), 0.2, 1., ERR_REL),
           -1,
           UNDO_MANAGER_NULL)
  
  BUGCONT (projet.enregistre ("test.xml"), -1, UNDO_MANAGER_NULL)
  
  return 0;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
