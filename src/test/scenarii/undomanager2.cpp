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

#include "codegui.hpp"
#include "MErreurs.hpp"
#include "SString.hpp"

int
main (int32_t argc,
      char   *argv[])
{
  CProjet projet (ENorme::NORME_EC);
  std::unique_ptr <CAction> action;
  
  // On charge la localisation
  setlocale (LC_ALL, "" );
  bindtextdomain (PACKAGE, LOCALEDIR);
  bind_textdomain_codeset (PACKAGE, "UTF-8");
  textdomain (PACKAGE);
  
  BUGCONT (projet.getActionCount () == 0, -1, static_cast <CUndoManager *> (nullptr))
  // 0 Poids propre
  action.reset (new CAction (new std::string ("Poids propre"), 0, projet));
  BUGCONT (projet.addAction (action.get ()), -1, static_cast <CUndoManager *> (nullptr))
  action.release ();
  BUGCONT (projet.getActionCount () == 1, -1, static_cast <CUndoManager *> (nullptr))
  // 2 Exploitation
  action.reset (new CAction (new std::string ("Chargement"), 2, projet));
  BUGCONT (projet.addAction (action.get ()), -1, static_cast <CUndoManager *> (nullptr))
  action.release ();
  BUGCONT (projet.getActionCount () == 2, -1, static_cast <CUndoManager *> (nullptr))
  BUGCONT (projet.undo (), -1, static_cast <CUndoManager *> (nullptr))
  BUGCONT (projet.getActionCount () == 1, -1, static_cast <CUndoManager *> (nullptr))
  // 18 Neige
  action.reset (new CAction (new std::string ("Neige"), 18, projet));
  BUGCONT (projet.addAction (action.get ()), -1, static_cast <CUndoManager *> (nullptr))
  action.release ();
  BUGCONT (projet.getActionCount () == 2, -1, static_cast <CUndoManager *> (nullptr))
  
  BUGCONT (projet.enregistre ("undomanager2.xml"), -1, static_cast <CUndoManager *> (nullptr))
  
  xmlNodePtr root_node, n0, n1, n2, n3;
  
  BUGCONT (root_node = xmlNewNode (nullptr, BAD_CAST2 ("Projet")), -1, static_cast <CUndoManager *> (nullptr))
  
  BUGCONT (projet.undoToXML (root_node), false, static_cast <CUndoManager *> (nullptr))
  
  n0 = root_node;
  BUGCONT (n0->type == XML_ELEMENT_NODE, -1, static_cast <CUndoManager *> (nullptr))
  BUGCONT (std::string ("Projet").compare (BAD_TSAC2 (n0->name)) == 0,
           -1,
           static_cast <CUndoManager *> (nullptr))
  BUGCONT (n1 = n0->children, -1, static_cast <CUndoManager *> (nullptr))
  BUGCONT (n1->type == XML_ELEMENT_NODE, -1, static_cast <CUndoManager *> (nullptr))
  BUGCONT (std::string ("UndoManager").compare (BAD_TSAC2 (n1->name)) == 0,
           -1,
           static_cast <CUndoManager *> (nullptr))
  BUGCONT (n2 = n1->children, -1, static_cast <CUndoManager *> (nullptr))
  BUGCONT (n2->type == XML_ELEMENT_NODE, -1, static_cast <CUndoManager *> (nullptr))
  BUGCONT (std::string ("Bloc").compare (BAD_TSAC2 (n2->name)) == 0, -1, static_cast <CUndoManager *> (nullptr))
  BUGCONT (n3 = n2->children, -1, static_cast <CUndoManager *> (nullptr))
  BUGCONT (n3->type == XML_ELEMENT_NODE, -1, static_cast <CUndoManager *> (nullptr))
  BUGCONT (std::string ("projetSetParam").compare (BAD_TSAC2 (n3->name)) == 0,
           -1,
           static_cast <CUndoManager *> (nullptr))
  
  xmlChar *prop;
  
  BUGCONT (n2 = n2->next, -1, static_cast <CUndoManager *> (nullptr))
  BUGCONT (n2->type == XML_ELEMENT_NODE, -1, static_cast <CUndoManager *> (nullptr))
  BUGCONT (std::string ("Bloc").compare (BAD_TSAC2 (n2->name)) == 0, -1, static_cast <CUndoManager *> (nullptr))
  BUGCONT (prop = xmlGetProp (n2, BAD_CAST2 ("Heure")), -1,static_cast <CUndoManager *> (nullptr))
  BUGCONT (n3 = n2->children, -1, static_cast <CUndoManager *> (nullptr))
  BUGCONT (n3->type == XML_ELEMENT_NODE, -1, static_cast <CUndoManager *> (nullptr))
  BUGCONT (std::string ("addAction").compare (BAD_TSAC2 (n3->name)) == 0,
           -1,
           static_cast <CUndoManager *> (nullptr))
  prop = xmlGetProp (n3, BAD_CAST2 ("Nom"));
  BUGCONT (std::string ("Poids propre").compare (BAD_TSAC2 (prop)) == 0,
           -1,
           static_cast <CUndoManager *> (nullptr))
  xmlFree (prop);
  BUGCONT (n2 = n2->next, -1, static_cast <CUndoManager *> (nullptr))
  BUGCONT (n2->type == XML_ELEMENT_NODE, -1, static_cast <CUndoManager *> (nullptr))
  BUGCONT (std::string ("Bloc").compare (BAD_TSAC2 (n2->name)) == 0, -1, static_cast <CUndoManager *> (nullptr))
  BUGCONT (n3 = n2->children, -1, static_cast <CUndoManager *> (nullptr))
  BUGCONT (n3->type == XML_ELEMENT_NODE, -1, static_cast <CUndoManager *> (nullptr))
  BUGCONT (std::string ("addAction").compare (BAD_TSAC2 (n3->name)) == 0,
           -1,
           static_cast <CUndoManager *> (nullptr))
  // On vérifie bien que Chargement n'est plus présent dans la liste des
  // modifications.
  prop = xmlGetProp (n3, BAD_CAST2 ("Nom"));
  BUGCONT (std::string ("Neige").compare (BAD_TSAC2 (prop)) == 0, -1, static_cast <CUndoManager *> (nullptr))
  xmlFree (prop);
  
  xmlFreeNode (root_node);
  
  return 0;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */