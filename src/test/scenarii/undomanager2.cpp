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

// Lorsque des modifications sont appliquées juste après un/plusieurs undo,
// on vérifie que les modifications fraichement annulées sont bien supprimées
// du gestionnaire d'annulation avant l'insertion des nouvelles modifications.

#include "config.h"

#include <stdint.h>
#include <libintl.h>
#include <string>
#include <iostream>
#include <memory>
#include <cassert>

#include "CProjet.hpp"
#include "SString.hpp"

int
main (int32_t,
      char   *[])
{
  CProjet projet (ENorme::EUROCODE);
  std::shared_ptr <CAction> action;
  
  assert (projet.getActionCount () == 0);
  // 0 Poids propre
  action = std::make_shared <CAction> (std::make_shared <std::string>
                                                              ("Poids propre"),
                                       0,
                                       projet);
  assert (projet.addAction (action));
  assert (projet.getActionCount () == 1);
  // 2 Exploitation
  action = std::make_shared <CAction> (std::make_shared <std::string>
                                                                ("Chargement"),
                                       2,
                                       projet);
  assert (projet.addAction (action));
  assert (projet.getActionCount () == 2);
  assert (projet.undo ());
  assert (projet.getActionCount () == 1);
  // 18 Neige
  action = std::make_shared <CAction> (std::make_shared <std::string>
                                                                     ("Neige"),
                                       18,
                                       projet);
  assert (projet.addAction (action));
  assert (projet.getActionCount () == 2);
  
  assert (projet.enregistre ("undomanager2.xml"));
  
  xmlNodePtr root_node, n0, n1, n2, n3;
  
  assert ((root_node = xmlNewNode (nullptr, BAD_CAST2 ("Projet"))) != nullptr);
  
  assert (projet.undoToXML (root_node));
  
  n0 = root_node;
  assert (n0->type == XML_ELEMENT_NODE);
  assert (std::string ("Projet").compare (BAD_TSAC2 (n0->name)) == 0);
  assert ((n1 = n0->children) != nullptr);
  assert (n1->type == XML_ELEMENT_NODE);
  assert (std::string ("UndoManager").compare (BAD_TSAC2 (n1->name)) == 0);
  assert ((n2 = n1->children) != nullptr);
  assert (n2->type == XML_ELEMENT_NODE);
  assert (std::string ("Bloc").compare (BAD_TSAC2 (n2->name)) == 0);
  assert ((n3 = n2->children) != nullptr);
  assert (n3->type == XML_ELEMENT_NODE);
  assert (std::string ("projetSetParam").compare (BAD_TSAC2 (n3->name)) == 0);
  
  xmlChar *prop;
  
  assert ((n2 = n2->next) != nullptr);
  assert (n2->type == XML_ELEMENT_NODE);
  assert (std::string ("Bloc").compare (BAD_TSAC2 (n2->name)) == 0);
  assert ((prop = xmlGetProp (n2, BAD_CAST2 ("Heure"))) != nullptr);
  assert ((n3 = n2->children) != nullptr);
  assert (n3->type == XML_ELEMENT_NODE);
  assert (std::string ("addAction").compare (BAD_TSAC2 (n3->name)) == 0);
  xmlFree (prop);
  prop = xmlGetProp (n3, BAD_CAST2 ("Nom"));
  assert (std::string ("Poids propre").compare (BAD_TSAC2 (prop)) == 0);
  xmlFree (prop);
  assert ((n2 = n2->next) != nullptr);
  assert (n2->type == XML_ELEMENT_NODE);
  assert (std::string ("Bloc").compare (BAD_TSAC2 (n2->name)) == 0);
  assert ((n3 = n2->children) != nullptr);
  assert (n3->type == XML_ELEMENT_NODE);
  assert (std::string ("addAction").compare (BAD_TSAC2 (n3->name)) == 0);
  // On vérifie bien que Chargement n'est plus présent dans la liste des
  // modifications.
  prop = xmlGetProp (n3, BAD_CAST2 ("Nom"));
  assert (std::string ("Neige").compare (BAD_TSAC2 (prop)) == 0);
  xmlFree (prop);
  
  xmlFreeNode (root_node);
  
  return 0;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
