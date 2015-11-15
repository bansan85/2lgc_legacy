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
  
  assert (projet.getUndoManager ().undoNb () == 1);
  // Un pour le projet,
  // Un pour l'historique.
  assert (projet.getNorme ().use_count () == 2);
  assert (projet.getNorme ()->getNom ().use_count () == 2);
  projet.getUndoManager ().setMemory (0);
  assert (projet.getNorme ().use_count () == 1);
  assert (projet.getNorme ()->getNom ().use_count () == 1);
  projet.getUndoManager ().setMemory (1000);
  assert (projet.getActionCount () == 0);
  // 0 Poids propre
  action = std::make_shared <POCO::sol::CAction> (
                           std::make_shared <std::string> ("Poids propre"), 0);
  assert (action.use_count () == 1);
  assert (action->getNom ().use_count () == 1);
  retour = projet.fAction.doAdd (action);
  assert (retour);
  assert (projet.getActionCount () == 1);
  // 2 Exploitation
  action = std::make_shared <POCO::sol::CAction> (
                             std::make_shared <std::string> ("Chargement"), 2);
  retour = projet.fAction.doAdd (action);
  assert (retour);
  // Un pour la variable action.
  // Un pour l'insertion dans la variable actions du modèle,
  // Deux par les std::bind permettant l'annulation et la répétition.
  assert (action.use_count () == 4);
  // Un par la variable action.
  // Un par le std::bind permettant la conversion au format XML.
  assert (action->getNom ().use_count () == 2);
  assert (projet.getActionCount () == 2);
  projet.getUndoManager ().setMemory (0);
  // Les 2 bind permettant l'annulation et la répétition disparaissent.
  assert (action.use_count () == 2);
  // Le std::bind permettant la conversion au format XML disparait.
  assert (action->getNom ().use_count () == 1);
  projet.getUndoManager ().setMemory (1000);
  
  retour = projet.enregistre ("undomanager4.xml");
  assert (retour);
  
  return 0;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
