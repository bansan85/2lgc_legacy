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

#include "CProjet.hpp"

int
main (int32_t,
      char   *[])
{
  CProjet projet (ENorme::EUROCODE);
  std::shared_ptr <CAction> action;
  
  // On charge la localisation
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE_NAME, LOCALEDIR);
  bind_textdomain_codeset (PACKAGE_NAME, "UTF-8");
  textdomain (PACKAGE_NAME);
  
  assert (projet.undoNb () == 1);
  // Un pour le projet,
  // Un pour l'historique.
  assert (projet.getParametres ().use_count () == 2);
  assert (projet.getParametres ()->getNom ().use_count () == 2);
  projet.setMemory (0);
  assert (projet.getParametres ().use_count () == 1);
  assert (projet.getParametres ()->getNom ().use_count () == 1);
  projet.setMemory (1000);
  assert (projet.getActionCount () == 0);
  // 0 Poids propre
  action = std::make_shared <CAction> (std::make_shared <std::string>
                                                              ("Poids propre"),
                                       0,
                                       projet);
  assert (action.use_count () == 1);
  assert (action->getNom ().use_count () == 1);
  assert (projet.addAction (action));
  assert (projet.getActionCount () == 1);
  // 2 Exploitation
  action = std::make_shared <CAction> (std::make_shared <std::string>
                                                                ("Chargement"),
                                       2,
                                       projet);
  assert (projet.addAction (action));
  // Un pour la variable action.
  // Un pour l'insertion dans le projet,
  // Deux pour les std::bind permettant l'annulation et la répétition.
  assert (action.use_count () == 4);
  assert (action->getNom ().use_count () == 2);
  assert (projet.getActionCount () == 2);
  projet.setMemory (0);
  assert (action.use_count () == 2);
  assert (action->getNom ().use_count () == 1);
  projet.setMemory (1000);
  
  assert (projet.enregistre ("undomanager4.xml"));
  
  return 0;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
