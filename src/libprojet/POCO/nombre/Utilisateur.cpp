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

#include <complex>
#include <memory>
#include <sstream>
#include <iostream>

#include "Utilisateur.hpp"
#include "EUniteTxt.hpp"
#include "MErreurs.hpp"
#include "SString.hpp"

POCO::nombre::Utilisateur::Utilisateur (double valeur,
                EUnite unit) :
  val (valeur),
  unite (unit)
{
}

POCO::nombre::Utilisateur::Utilisateur (const Utilisateur & other) :
  INb (),
  val (other.val),
  unite (other.unite)
{
}

POCO::nombre::Utilisateur::~Utilisateur ()
{
}

double
POCO::nombre::Utilisateur::getVal () const
{
  return val;
}

EUnite
POCO::nombre::Utilisateur::getUnite () const
{
  return unite;
}

std::string
POCO::nombre::Utilisateur::toString (
  std::array <uint8_t, static_cast <size_t> (EUnite::LAST)> & decimales) const
{
  std::string retour;
  uint8_t     width;
  double      test;
  
  if (std::abs (val) > 1e15)
  {
    for (width = 0; width <= 15; width++)
    {
      retour = format ("%.*le", width, val);
      SSCANF (retour.c_str (), "%le", &test);
      if ((fabs (val) * 0.999999999999999 <= fabs (test)) &&
          (fabs (test) <= fabs (val) * 1.000000000000001))
      {
        break;
      }
    }
  }
  // Sinon on affiche sous forme normale
  else
  {
    for (width = 0; width <= 15; width++)
    {
      retour = format ("%.*lf", width, val);
      SSCANF (retour.c_str (), "%lf", &test);
      if ((fabs (val) * 0.999999999999999 <= fabs (test)) &&
          (fabs (test) <= fabs (val) * 1.000000000000001))
      {
        break;
      }
    }
  }
  
  return format ("%.*lf",
                 std::max (width, decimales[static_cast<size_t>(unite)]),
                 val);
}

bool CHK
POCO::nombre::Utilisateur::newXML (xmlNodePtr root) const
{
  std::unique_ptr <xmlNode, void (*)(xmlNodePtr)> node (
                               xmlNewNode (nullptr, BAD_CAST2 ("Utilisateur")),
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
                       BAD_CAST2 (EUniteConst[static_cast <size_t> (unite)]
                                                        .c_str ())) != nullptr,
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
