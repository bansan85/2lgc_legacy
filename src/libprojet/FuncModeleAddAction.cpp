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

#include "FuncModeleAddAction.hpp"
#include "CModele.hpp"
#include "MErreurs.hpp"
#include "SString.hpp"
#include "POCO/nombre/Calcul.hpp"

FuncModeleAddAction::FuncModeleAddAction (CModele & modele_) :
  IUndoableFonction (true),
  modele (modele_)
{
}

FuncModeleAddAction::~FuncModeleAddAction ()
{
}

bool
FuncModeleAddAction::execute (std::shared_ptr <POCO::sol::CAction> & action)
{
  BUGPARAM (static_cast <void *> (action.get ()),
            "%p",
            action,
            false,
            &modele.undoManager)
  
  BUGUSER (modele.getAction (*action->getNom ()) == nullptr,
           false,
           &modele.undoManager,
           gettext ("L'action “%s” existe déjà.\nImpossible de l'ajouter.\n"),
             action->getNom ()->c_str ())
  
  BUGPROG (action->emptyCharges (),
           false,
           &modele.undoManager,
           "L'action doit être ajoutée sans charge. Elles doivent être ajoutées ensuite.\n")
  
  BUGPROG (action->getType () < modele.getNorme ()->getPsiN (),
           false,
           &modele.undoManager,
           "Le type d'action %d est inconnu.\n", action->getType ())
  
  BUGCONT (modele.undoManager.ref (), false, &modele.undoManager)
  
  modele.actions.push_back (action);

  BUGCONT (modele.undoManager.push (
             std::bind (&CModele::rmAction, &modele, action),
             std::bind (&CModele::addAction, &modele, action),
             nullptr,
             std::bind (&FuncModeleAddAction::doXML,
                        this,
                        action->getNom (),
                        action->getType (),
                        std::placeholders::_1),
             format (gettext ("Ajout de l'action “%s”"),
                     action->getNom ().get ()->c_str ())),
           false,
           &modele.undoManager)
  
  uint8_t type = action->getType ();
  std::array <uint8_t, static_cast <size_t> (EUnite::LAST)> & decimales =
                                            modele.preferences.getDecimales ();
  
  if (modele.undoManager.getEtat () != EUndoEtat::NONE_OR_REVERT)
  {
    std::shared_ptr <POCO::INb> newPsi =
      std::make_shared <POCO::nombre::Calcul> (
        modele.norme->getPsi0 (type), EUnite::U_, decimales);
    action->psi0 = newPsi;
    BUGCONT (modele.undoManager.pushSuppr(newPsi), false, &modele.undoManager)

    newPsi = std::make_shared <POCO::nombre::Calcul> (
      modele.norme->getPsi1 (type), EUnite::U_, decimales);
    action->psi1 = newPsi;
    BUGCONT (modele.undoManager.pushSuppr(newPsi), false, &modele.undoManager)

    newPsi = std::make_shared <POCO::nombre::Calcul> (
      modele.norme->getPsi2 (type), EUnite::U_, decimales);
    action->psi2 = newPsi;
    BUGCONT (modele.undoManager.pushSuppr(newPsi), false, &modele.undoManager)
  }

  BUGCONT (modele.undoManager.unref (), false, &modele.undoManager)
  
  return true;
}

bool
FuncModeleAddAction::doXML (std::shared_ptr<const std::string> & nom_,
                            uint8_t type_,
                            xmlNodePtr root)
{
  BUGPARAM (static_cast <void *> (root),
            "%p",
            root,
            false,
            &modele.undoManager)
  
  std::unique_ptr <xmlNode, void (*)(xmlNodePtr)> node (
                                 xmlNewNode (nullptr, BAD_CAST2 ("addAction")),
                                 xmlFreeNode);
  
  BUGCRIT (node.get () != nullptr,
           false,
           &modele.undoManager,
           "Erreur d'allocation mémoire.\n")
  
  BUGCRIT (xmlSetProp (node.get (),
                       BAD_CAST2 ("Nom"),
                       BAD_CAST2 (nom_->c_str ())) != nullptr,
           false,
           &modele.undoManager,
           "Problème depuis la librairie : %s\n", "xml2")
  
  BUGCRIT (xmlSetProp (node.get (),
                       BAD_CAST2 ("Type"),
                       BAD_CAST2 (std::to_string (type_).c_str ())) != nullptr,
           false,
           &modele.undoManager,
           "Problème depuis la librairie : %s\n", "xml2")
  
  BUGCRIT (xmlAddChild (root, node.get ()) != nullptr,
           false,
           &modele.undoManager,
           "Problème depuis la librairie : %s\n", "xml2")
  
  node.release ();
  
  return true;
}

const std::string
FuncModeleAddAction::doDescription ()
{
  return "teisuan";
}

const std::string
FuncModeleAddAction::undoDescription ()
{
  return "teisuan";
}

bool
FuncModeleAddAction::undo ()
{
  return false;
}

bool
FuncModeleAddAction::undoXML (xmlNodePtr node)
{
  (void) node;
  return false;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
