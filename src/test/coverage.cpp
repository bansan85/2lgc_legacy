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

#include "codegui.hpp"

int
main (int32_t,
      char   *[])
{
  /* Variables */
  CProjet projet (ENorme::EUROCODE);
//  GList   *tmp1, *tmp2;
  
//  EF_Relachement_Donnees_Elastique_Lineaire *ry_d, *rz_d, *ry_f, *rz_f;
  
  // On charge la localisation
  setlocale (LC_ALL, "" );
  bindtextdomain (PACKAGE_NAME, LOCALEDIR);
  bind_textdomain_codeset (PACKAGE_NAME, "UTF-8");
  textdomain (PACKAGE_NAME);
  
  // Initialisation de GTK+, gtk doit être initialisé avant m3dlib.
/*  INFO (gtk_init_check (&argc, &argv),
        -1,
        (gettext ("Impossible d'initialiser gtk.\n")); )*/
  
  assert (projet.ref ());
  
  std::shared_ptr <CAction> action;
  for (uint8_t i = 0; i < 22; i++)
  {
    action.reset (new CAction (std::shared_ptr <std::string> (
             new std::string (projet.getParametres ()->getpsiDescription (i))),
                              0,
                              projet));
    
    assert (projet.addAction (action));
  }
  
  assert (projet.getEtat () == EUndoEtat::MODIF);
  assert (projet.unref ());
  
  assert (projet.ref ());
  CAction *action2;
  assert (action2 = projet.getAction (projet.getParametres ()->
                                                       getpsiDescription (5)));
  action2->getPsi (0)->getVal ();
  action2->getPsi (1)->getUnite ();
  action2->getPsi (2)->toString ();
  assert (action2->setPsi (0, std::shared_ptr <INb> (
                                               new NbUser (0.5, EUnite::U_))));
  assert (action2->setPsi (1, std::shared_ptr <INb> (
                                               new NbUser (0.5, EUnite::U_))));
  assert (action2->setPsi (2, std::shared_ptr <INb> (
                                               new NbUser (0.5, EUnite::U_))));
  action2->getPsi (0)->getVal ();
  action2->getPsi (1)->getUnite ();
  action2->getPsi (2)->toString ();
  assert (projet.unref ());
  assert (projet.undo ());
  assert (projet.redo ());
  
  assert (projet.ref ());
  
  std::unique_ptr <std::string> nom (new std::string (projet.getParametres ()->
                                                      getpsiDescription (22)));
  action.reset (new CAction (std::shared_ptr <std::string> (
            new std::string (projet.getParametres ()->getpsiDescription (22))),
                             22,
                             projet));
  // Ici, il y a un traitement volontaire de l'erreur.
  if (!projet.addAction (action))
  {
    action.reset ();
    nom.reset ();
    projet.rollback ();
  }
  else
    assert (nullptr);
  
  assert (projet.getParametres ()->getpsiDescription (22).empty ());
  assert (projet.getActionCount () == 22);
  assert (projet.undo ());
  assert (projet.undo ());
  assert (projet.getActionCount () == 0);
  assert (projet.redo ());
  assert (projet.redo ());
  assert (projet.getActionCount () == 22);
  assert (projet.getEtat () == EUndoEtat::NONE_OR_REVERT);
  
  assert (projet.undo ());
  assert (projet.ref ());
  assert (projet.getParametres ()->setNom (
                     std::shared_ptr <std::string> (new std::string ("nom"))));
  assert (projet.getParametres ()->setVariante (0));
  assert (projet.unref ());
  
  assert (projet.enregistre ("coverage.xml"));
  
  return 0;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
