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

#include <iostream>

#include "UndoData.hpp"
#include "MErreurs.hpp"

POCO::UndoData::UndoData () :
  annule (),
  repete (),
  suppr (),
  sauve (),
  heure (0),
  description ()
{
}

POCO::UndoData::~UndoData ()
{
  // Les pointeurs contenus dans suppr sont automatiquement supprimés.
}

bool
POCO::UndoData::addAnnuler (std::function <bool ()> fonction)
{
  annule.push_front (fonction);
  return true;
}

bool
POCO::UndoData::execAnnuler ()
{
  for (std::function <bool ()> f : annule)
  {
    BUGCRIT (f (),
             false,
             UNDO_MANAGER_NULL,
             "Échec lors de l'opération d'annulation.\n"
               "Le projet est très probablement corrompu.\n")
  }
  return true;
}

bool
POCO::UndoData::addRepeter (std::function <bool ()> fonction)
{
  repete.push_back (fonction);
  return true;
}

bool
POCO::UndoData::execRepeter ()
{
  for (std::function <bool ()> f : repete)
  {
    BUGCRIT (f (),
             false,
             UNDO_MANAGER_NULL,
             "Echec lors de l'opération de répétition.\n"
               "Le projet est très probablement corrompu.\n")
  }
  return true;
}

bool
POCO::UndoData::addSupprimer (std::shared_ptr <void> objet)
{
  suppr.push_back (objet);
  return true;
}

bool
POCO::UndoData::addToXML (std::function <bool (xmlNodePtr)> fonction)
{
  sauve.push_back(fonction);
  return true;
}

bool
POCO::UndoData::execToXML (xmlNodePtr noeud) const
{
  for (std::function <bool (xmlNodePtr)> f : sauve)
  {
    BUGCRIT (f (noeud),
             false,
             UNDO_MANAGER_NULL,
             "Erreur lors de la génération du fichier XML.\n")
  }
  return true;
}

const std::string &
POCO::UndoData::getDescription () const
{
  return description;
}

time_t
POCO::UndoData::getHeure ()
{
  return heure;
}

bool
POCO::UndoData::setHeure ()
{
  time (&heure);
  return true;
}

bool
POCO::UndoData::appendDescription (const std::string & description_)
{
  if (!description.empty()) {
    description.append ("\n");
  }
  description.append (description_);
  return true;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
