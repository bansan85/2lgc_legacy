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

#include "FuncModeleNorme.hpp"
#include "CModele.hpp"
#include "MErreurs.hpp"
#include "SString.hpp"

FuncModeleNorme::FuncModeleNorme (CModele & modele_) :
  modele (modele_)
{
}

FuncModeleNorme::~FuncModeleNorme ()
{
}

bool
FuncModeleNorme::doSet (std::shared_ptr <INorme> & norme)
{
  BUGPARAM (static_cast <void *> (norme.get ()),
            "%p", norme, false, &modele.undoManager)

  BUGCONT (modele.undoManager.ref (), false, &modele.undoManager)

  norme->id = 0;
  
  BUGCONT (modele.undoManager.push (
    std::bind (&FuncModeleNorme::doSet, this, modele.norme),
    std::bind (&FuncModeleNorme::doSet, this, norme),
    nullptr,
    std::bind (&FuncModeleNorme::doXMLSet,
               this,
               norme->id,
               norme->type,
               norme->variante,
               norme->options,
               norme->nom,
               std::placeholders::_1),
    format (gettext ("Paramètres du projet (%s)"), norme->nom->c_str ())),
    false,
    &modele.undoManager)


  modele.norme = norme;

  BUGCONT (modele.undoManager.unref (), false, &modele.undoManager)

  return true;
}

bool
FuncModeleNorme::doXMLSet (uint32_t                              id,
                           ENorme                                type,
                           uint32_t                              variante,
                           uint32_t                              options,
                           std::shared_ptr <const std::string> & nom,
                           xmlNodePtr                            root) const
{
  BUGPARAM (static_cast <const void *> (nom.get ()),
            "%p", nom.get (), false, UNDO_MANAGER_NULL)
  BUGPARAM (static_cast <void *> (root),
            "%p", root, false, UNDO_MANAGER_NULL)

  std::unique_ptr <xmlNode, void (*)(xmlNodePtr)> node (
                                  xmlNewNode (nullptr, BAD_CAST2 ("setNorme")),
                                  xmlFreeNode);

  BUGCRIT (node.get () != nullptr,
           false,
           UNDO_MANAGER_NULL,
           "Erreur d'allocation mémoire.\n")

  BUGCRIT (xmlSetProp (node.get (),
                       BAD_CAST2 ("id"),
                       BAD_CAST2 (std::to_string (id).c_str ())) != nullptr,
           false,
           UNDO_MANAGER_NULL,
           "Problème depuis la librairie : %s\n", "xml2")

  BUGCRIT (xmlSetProp (node.get (),
                       BAD_CAST2 ("type"),
                       BAD_CAST2 (std::to_string (static_cast <uint32_t>
                                                 (type)).c_str ())) != nullptr,
           false,
           UNDO_MANAGER_NULL,
           "Problème depuis la librairie : %s\n", "xml2")

  BUGCRIT (xmlSetProp (node.get (),
                       BAD_CAST2 ("variante"),
                       BAD_CAST2 (std::to_string (variante).c_str ()))
                                                                    != nullptr,
           false,
           UNDO_MANAGER_NULL,
           "Problème depuis la librairie : %s\n", "xml2")

  BUGCRIT (xmlSetProp (node.get (),
                       BAD_CAST2 ("options"),
                       BAD_CAST2 (std::to_string (options).c_str ()))
                                                                    != nullptr,
           false,
           UNDO_MANAGER_NULL,
           "Problème depuis la librairie : %s\n", "xml2")

  BUGCRIT (xmlSetProp (node.get (),
                       BAD_CAST2 ("nom"),
                       BAD_CAST2 (nom->c_str ())) != nullptr,
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
