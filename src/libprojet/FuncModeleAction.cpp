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

#include <algorithm>

#include "FuncModeleAction.hpp"
#include "CModele.hpp"
#include "MErreurs.hpp"
#include "SString.hpp"
#include "POCO/nombre/Calcul.hpp"
#include "EUniteTxt.hpp"

FuncModeleAction::FuncModeleAction (CModele & modele_) :
  modele (modele_)
{
}

FuncModeleAction::~FuncModeleAction ()
{
}

bool
FuncModeleAction::doAdd (std::shared_ptr <POCO::sol::CAction> & action)
{
  BUGPARAM (static_cast <void *> (action.get ()),
            "%p", action, false, &modele.undoManager)
  
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
  
  uint8_t type = action->getType ();
  
  std::list <std::shared_ptr <POCO::sol::CAction> >::iterator it;

  // Première insertion. Ce n'est pas une action du gestionnaire d'annulation.
  if (action->id == 0xFFFFFFFF)
  {
    if (modele.actions.empty ())
    {
      action->id = 0;
    }
    else
    {
      action->id = modele.actions.back ()->id + 1;
    }

    std::shared_ptr <POCO::INb> newPsi =
      std::make_shared <POCO::nombre::Calcul> (modele.norme->getPsi0 (type),
                                               EUnite::U_);
    action->psi0 = newPsi;

    newPsi = std::make_shared <POCO::nombre::Calcul> (
               modele.norme->getPsi1 (type), EUnite::U_);
    action->psi1 = newPsi;

    newPsi = std::make_shared <POCO::nombre::Calcul> (
               modele.norme->getPsi2 (type), EUnite::U_);
    action->psi2 = newPsi;

    it = modele.actions.end ();
  }
  else
  {
    it = std::find_if (modele.actions.begin (),
                       modele.actions.end (),
                       [&action]
                         (const std::shared_ptr <POCO::sol::CAction> & action_)
                       {
                         return action->id <= action_->id;
                       });
    if (it != modele.actions.end ())
    {
      BUGPROG ((*it)->id == action->id,
               false,
               &modele.undoManager,
               "L'action avec l'identifiant %u existe déjà.", action->id)
    }
  }

  BUGCONT (modele.undoManager.push (
             std::bind (&FuncModeleAction::doRemove, this, action),
             std::bind (&FuncModeleAction::doAdd, this, action),
             nullptr,
             std::bind (&FuncModeleAction::doXMLAdd,
                        this,
                        action->id,
                        action->getNom (),
                        action->getType (),
                        action->psi0,
                        action->psi1,
                        action->psi2,
                        std::placeholders::_1),
             format (gettext ("Ajout de l'action “%s”"),
                     action->getNom ().get ()->c_str ())),
           false,
           &modele.undoManager)
  
  modele.actions.insert (it, action);

  BUGCONT (modele.undoManager.unref (), false, &modele.undoManager)
  
  return true;
}

bool
FuncModeleAction::doXMLAdd (uint32_t                              id,
                            std::shared_ptr <const std::string> & nom_,
                            uint8_t                               type_,
                            std::shared_ptr <POCO::INb>         & nb0,
                            std::shared_ptr <POCO::INb>         & nb1,
                            std::shared_ptr <POCO::INb>         & nb2,
                            xmlNodePtr                            root) const
{
  BUGPARAM (static_cast <const void *> (nom_.get ()),
            "%p", nom_.get (), false, UNDO_MANAGER_NULL)
  BUGPARAM (static_cast <void *> (root),
            "%p", root, false, UNDO_MANAGER_NULL)
  
  std::unique_ptr <xmlNode, void (*)(xmlNodePtr)> node (
                                 xmlNewNode (nullptr, BAD_CAST2 ("addAction")),
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
                       BAD_CAST2 ("nom"),
                       BAD_CAST2 (nom_->c_str ())) != nullptr,
           false,
           UNDO_MANAGER_NULL,
           "Problème depuis la librairie : %s\n", "xml2")
  
  BUGCRIT (xmlSetProp (node.get (),
                       BAD_CAST2 ("type"),
                       BAD_CAST2 (std::to_string (type_).c_str ())) != nullptr,
           false,
           UNDO_MANAGER_NULL,
           "Problème depuis la librairie : %s\n", "xml2")

  BUGCONT (nb0->newXML (node.get ()), false, UNDO_MANAGER_NULL)
  BUGCONT (nb1->newXML (node.get ()), false, UNDO_MANAGER_NULL)
  BUGCONT (nb2->newXML (node.get ()), false, UNDO_MANAGER_NULL)
  
  BUGCRIT (xmlAddChild (root, node.get ()) != nullptr,
           false,
           UNDO_MANAGER_NULL,
           "Problème depuis la librairie : %s\n", "xml2")
  
  node.release ();
  
  return true;
}

bool
FuncModeleAction::doSetPsi (std::shared_ptr <POCO::sol::CAction> & action,
                            uint8_t                                psi,
                            std::shared_ptr <POCO::INb> &          val)
{
  BUGPARAM (static_cast <void *> (action.get ()),
            "%p", action, false, &modele.undoManager)
  BUGPARAM (psi, "%u", psi <= 2, false, &modele.undoManager)
  BUGPARAM (static_cast <void *> (val.get ()),
            "%p", val, false, &modele.undoManager)

  BUGPROG (val.get ()->getUnite () == EUnite::U_,
           false,
           &modele.undoManager,
           "L'unité est de type [%s] à la place de [%s].\n",
             EUniteConst[static_cast <size_t> (val.get ()->getUnite ())]
                                                                     .c_str (),
                             
             EUniteConst[static_cast <size_t> (EUnite::U_)].c_str ())

  BUGCONT (modele.undoManager.ref (), false, &modele.undoManager)

  BUGCONT (modele.undoManager.push (
             psi == 0 ? std::bind (&FuncModeleAction::doSetPsi,
                                   this, action, psi, action->psi0) :
               psi == 1 ? std::bind (&FuncModeleAction::doSetPsi,
                                     this, action, psi, action->psi1) :
                 std::bind (&FuncModeleAction::doSetPsi,
                            this, action, psi, action->psi2),
             std::bind (&FuncModeleAction::doSetPsi, this, action, psi, val),
             nullptr,
             std::bind (&FuncModeleAction::doXMLSetPsi,
                        this,
                        action->id,
                        psi,
                        val,
                        std::placeholders::_1),
             format (gettext ("Cœfficient ψ%s de l'action “%s” (%s)"),
                     psi == 0 ? "₀" : psi == 1 ? "₁" : "₂",
                     action->nom->c_str (),
                     val->toString (modele.preferences.getDecimales ()).
                                                                    c_str ())),
             false,
             &modele.undoManager)

  if (psi == 0)
  {
    action->psi0 = val;
  }
  else if (psi == 1)
  {
    action->psi1 = val;
  }
  else
  {
    action->psi2 = val;
  }

  BUGCONT (modele.undoManager.unref (), false, &modele.undoManager)

  return true;
}

bool
FuncModeleAction::doXMLSetPsi (uint32_t                      id,
                               uint8_t                       psi,
                               std::shared_ptr <POCO::INb> & val,
                               xmlNodePtr                    root) const
{
  BUGPARAM (psi, "%u", psi <= 2, false, UNDO_MANAGER_NULL)
  BUGPARAM (static_cast <void *> (val.get ()),
            "%p", val, false, UNDO_MANAGER_NULL)
  BUGPARAM (static_cast <void *> (root), "%p", root, false, UNDO_MANAGER_NULL)
  
  std::unique_ptr <xmlNode, void (*)(xmlNodePtr)> node (
                              xmlNewNode (nullptr, BAD_CAST2 ("actionSetPsi")),
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
  
  BUGCRIT (xmlSetProp (
             node.get (),
             BAD_CAST2 ("psi"),
             BAD_CAST2 (psi == 0 ? "0" : psi == 1 ? "1" : "2")) != nullptr,
           false,
           UNDO_MANAGER_NULL,
           "Problème depuis la librairie : %s\n", "xml2")
  
  BUGCONT (val->newXML (node.get ()), false, UNDO_MANAGER_NULL)
  
  BUGCRIT (xmlAddChild (root, node.get ()) != nullptr,
           false,
           UNDO_MANAGER_NULL,
           "Problème depuis la librairie : %s\n", "xml2")
  node.release ();
  
  return true;
}

bool
FuncModeleAction::doRemove (std::shared_ptr <POCO::sol::CAction> & action)
{
  BUGPARAM (static_cast <void *> (action.get ()),
            "%p", action, false, &modele.undoManager)
  BUGPROG (action.get ()->emptyCharges (),
           false, &modele.undoManager,
           "Les charges doivent être supprimées avant la charge.")

  BUGCONT (modele.undoManager.ref (), false, &modele.undoManager)

  std::list <std::shared_ptr <POCO::sol::CAction> >::iterator it;
     
  it = std::find_if (modele.actions.begin (),
                     modele.actions.end (),
                     [&action]
                       (const std::shared_ptr <POCO::sol::CAction> & action_)
                     {
                       return action->id == action_->id;
                     });

  BUGPROG (it != modele.actions.end (),
           false,
           &modele.undoManager,
           "Impossible de trouver l'action “%s” dans le projet.",
             action->nom->c_str ())

       
  modele.actions.erase (it);

  BUGCONT (modele.undoManager.push (
             std::bind (&FuncModeleAction::doAdd, this, action),
             std::bind (&FuncModeleAction::doRemove, this, action),
             nullptr,
             nullptr,
             format (gettext ("Suppression de l'action “%s”"),
               action->nom->c_str ())),
           false,
           &modele.undoManager);

  BUGCONT (modele.undoManager.unref (), false, &modele.undoManager)
  
  return true;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
