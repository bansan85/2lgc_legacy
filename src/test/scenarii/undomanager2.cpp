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

#include "CModele.hpp"
#include "SString.hpp"

int
main (int32_t,
      char   *[])
{
  CModele projet (ENorme::EUROCODE);
  std::shared_ptr <POCO::sol::CAction> action;
  bool retour;
  
  assert (projet.getActionCount () == 0);
  // 0 Poids propre
  action = std::make_shared <POCO::sol::CAction> (
                           std::make_shared <std::string> ("Poids propre"), 0);
  retour = projet.fAction.doAdd (action);
  assert (retour);
  assert (projet.getActionCount () == 1);
  // 2 Exploitation
  action = std::make_shared <POCO::sol::CAction> (
                             std::make_shared <std::string> ("Chargement"), 2);
  retour = projet.fAction.doAdd (action);
  assert (retour);
  assert (projet.getActionCount () == 2);
  retour = projet.getUndoManager ().undo ();
  assert (retour);
  assert (projet.getActionCount () == 1);
  // 18 Neige
  action = std::make_shared <POCO::sol::CAction> (
                                 std::make_shared <std::string> ("Neige"), 18);
  retour = projet.fAction.doAdd (action);
  assert (retour);
  assert (projet.getActionCount () == 2);
  
  retour = projet.enregistre ("undomanager2.xml");
  assert (retour);
  
  xmlNodePtr root_node, n0, n1, n2, n3, n4;
  xmlChar *prop;
  
  root_node = xmlNewNode (nullptr, BAD_CAST2 ("projet"));
  assert (root_node);
  
  retour = projet.getUndoManager ().undoToXML (root_node);
  assert (retour);
  
  n0 = root_node;
  assert (n0->type == XML_ELEMENT_NODE);
  assert (std::string ("projet").compare (BAD_TSAC2 (n0->name)) == 0);
/*  prop = xmlGetProp (n0, BAD_CAST2 ("Norme"));
  assert (std::string ("Eurocode").compare (BAD_TSAC2 (prop)) == 0);
  xmlFree (prop);
  prop = xmlGetProp (n0, BAD_CAST2 ("Annexe"));
  assert (std::string ("1").compare (BAD_TSAC2 (prop)) == 0);
  xmlFree (prop);
  prop = xmlGetProp (n0, BAD_CAST2 ("Variante"));
  assert (std::string ("0").compare (BAD_TSAC2 (prop)) == 0);
  xmlFree (prop);*/
  n1 = n0->children;
  assert (n1);
  assert (n1->type == XML_ELEMENT_NODE);
  assert (std::string ("undoManager").compare (BAD_TSAC2 (n1->name)) == 0);
  n2 = n1->children;
  assert (n2);
  n2 = n2->next;
  assert (n2);
  assert (n2->type == XML_ELEMENT_NODE);
  assert (std::string ("bloc").compare (BAD_TSAC2 (n2->name)) == 0);
  prop = xmlGetProp (n2, BAD_CAST2 ("description"));
  assert (std::string ("Ajout de l'action “Poids propre”").compare (BAD_TSAC2 (prop)) == 0);
  xmlFree (prop);
  n3 = n2->children;
  assert (n3);
  assert (n3->type == XML_ELEMENT_NODE);
  assert (std::string ("addAction").compare (BAD_TSAC2 (n3->name)) == 0);
  prop = xmlGetProp (n3, BAD_CAST2 ("id"));
  assert (std::string ("0").compare (BAD_TSAC2 (prop)) == 0);
  xmlFree (prop);
  prop = xmlGetProp (n3, BAD_CAST2 ("nom"));
  assert (std::string ("Poids propre").compare (BAD_TSAC2 (prop)) == 0);
  xmlFree (prop);
  prop = xmlGetProp (n3, BAD_CAST2 ("type"));
  assert (std::string ("0").compare (BAD_TSAC2 (prop)) == 0);
  xmlFree (prop);
  n4 = n3->children;
  assert (n4);
  assert (n4->type == XML_ELEMENT_NODE);
  assert (std::string ("calcul").compare (BAD_TSAC2 (n4->name)) == 0);
  n4 = n4->next;
  assert (n4);
  assert (n4->type == XML_ELEMENT_NODE);
  n4 = n4->next;
  assert (n4);
  assert (n4->type == XML_ELEMENT_NODE);
  n4 = n4->next;
  assert (!n4);
  n2 = n2->next;
  assert (n2);
  assert (n2->type == XML_ELEMENT_NODE);
  assert (std::string ("bloc").compare (BAD_TSAC2 (n2->name)) == 0);
  n3 = n2->children;
  assert (n3);
  assert (n3->type == XML_ELEMENT_NODE);
  assert (std::string ("addAction").compare (BAD_TSAC2 (n3->name)) == 0);
  // On vérifie bien que Chargement n'est plus présent dans la liste des
  // modifications.
  prop = xmlGetProp (n3, BAD_CAST2 ("nom"));
  assert (std::string ("Neige").compare (BAD_TSAC2 (prop)) == 0);
  xmlFree (prop);
  
  xmlFreeNode (root_node);
  
  return 0;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
