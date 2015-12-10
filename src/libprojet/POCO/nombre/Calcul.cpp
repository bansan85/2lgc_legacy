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

#include <sstream>

#include "Calcul.hpp"
#include "SString.hpp"
#include "MErreurs.hpp"
#include "EUniteTxt.hpp"

POCO::nombre::Calcul::Calcul (
  double  valeur,
  EUnite  unit) :
  val (valeur),
  unite (unit)
{
}

POCO::nombre::Calcul::Calcul (const Calcul & nb) :
  INb (),
  val (nb.val),
  unite (nb.unite)
{
}


POCO::nombre::Calcul::~Calcul ()
{
}

double
POCO::nombre::Calcul::getVal () const
{
  return val;
}

EUnite
POCO::nombre::Calcul::getUnite () const
{
  return unite;
}

std::string
POCO::nombre::Calcul::toString (
   std::array <uint8_t, static_cast <size_t> (EUnite::LAST)> & decimales) const
{
  std::ostringstream oss;
  
  oss.precision (decimales[static_cast <size_t> (unite)]);
  oss << std::fixed << val;
  
  return oss.str ();
}

bool CHK
POCO::nombre::Calcul::newXML (xmlNodePtr root) const
{
  std::unique_ptr <xmlNode, void (*)(xmlNodePtr)> node (
                                    xmlNewNode (nullptr, BAD_CAST2 ("calcul")),
                                    xmlFreeNode);
  
  BUGCRIT (node.get () != nullptr,
           false,
           UNDO_MANAGER_NULL,
           "Erreur d'allocation mémoire.\n")
  
  std::ostringstream oss;
  
  oss << std::scientific << val;
  
  BUGCRIT (xmlSetProp (node.get (),
                       BAD_CAST2 ("valeur"),
                       BAD_CAST2 (oss.str ().c_str ())) != nullptr,
           false,
           UNDO_MANAGER_NULL,
           "Problème depuis la librairie : %s\n", "xml2")
  
  BUGCRIT (xmlSetProp (node.get (),
                       BAD_CAST2 ("unite"),
                       BAD_CAST2 (EUniteConst[static_cast <size_t> (unite)].
                                                         c_str ())) != nullptr,
           false,
           UNDO_MANAGER_NULL,
           "Problème depuis la librairie : %s\n", "xml2")
  
  BUGCRIT (xmlAddChild (root, node.get ()) != nullptr,
           false,
           UNDO_MANAGER_NULL,
           "Problème depuis la librairie : %s\n", "xml2")
  
  node.release ();
  
  return true;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
