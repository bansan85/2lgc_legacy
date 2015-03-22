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

#include <memory>

#include "CModele.hpp"
#include "NbCalcul.hpp"
#include "CProjet.hpp"
#include "MErreurs.hpp"
#include "SString.hpp"

CModele::CModele () :
  appuis (),
  noeuds (),
  sections (),
  materiaux (),
  relachements (),
  barres (),
  actions (),
  niveaux_groupes ()
{
}

CModele::~CModele ()
{
}

bool
CModele::addAction (std::shared_ptr <CAction> action)
{
  BUGPARAM (static_cast <void *> (action.get ()),
            "%p",
            action,
            false,
            &action->getUndoManager ())
  
  BUGUSER (getAction (*action->getNom ()) == NULL,
           false,
           &action->getUndoManager (),
           gettext ("L'action '%s' existe déjà.\nImpossible de l'ajouter.\n"),
             action->getNom ()->c_str ())
  
  BUGPROG (action->emptyCharges (),
           false,
           &action->getUndoManager (),
           gettext ("L'action doit être ajoutée sans charge. Elles doivent être ajoutées ensuite.\n"))
  
  BUGPROG (action->getType () < static_cast <CProjet *> (this)
                                                ->getParametres ()->getpsiN (),
           false,
           &action->getUndoManager (),
           gettext ("Le type d'action %d est inconnu.\n"), action->getType ())
  
  BUGCONT (action->getUndoManager ().ref (),
           false,
           &action->getUndoManager ())
  
  actions.push_back (action);

  std::shared_ptr <IParametres> param = static_cast <CProjet *> (this)->
                                                              getParametres ();

  std::shared_ptr <std::string> type_ (new std::string (
                 param.get ()->getpsiDescription (action.get ()->getType ())));

  BUGCONT (action->getUndoManager ().push (
             std::bind (static_cast <bool (CModele::*)
                             (std::shared_ptr <CAction>)> (&CModele::rmAction),
                        this,
                        action),
             std::bind (&CModele::addAction, this, action),
             nullptr,
             std::bind (&CAction::addXML,
                        action.get (),
                        action->getNom ().get (),
                        type_.get (),
                        std::placeholders::_1),
             format (gettext ("Ajout de l'action “%s”"),
                     action->getNom ().get ()->c_str ())),
           false,
           &action->getUndoManager ())
  BUGCONT (action->getUndoManager ().pushSuppr (action->getNom ()),
           false,
           &action->getUndoManager ())
  BUGCONT (action->getUndoManager ().pushSuppr (type_),
           false,
           &action->getUndoManager ())
  
  uint8_t type = action->getType ();
  std::array <uint8_t, static_cast <size_t> (EUnite::LAST)> decimales =
                              dynamic_cast <CProjet *> (this)->getDecimales ();
  
  if (action->getUndoManager ().getEtat () != EUndoEtat::NONE_OR_REVERT)
  {
    BUGCONT (action->setParam (
               param,
               std::shared_ptr <INb> (new NbCalcul (param.get ()->getpsi0
                                                                        (type),
                                                    EUnite::U_,
                             decimales)),
               std::shared_ptr <INb> (new NbCalcul (param.get ()->getpsi1
                                                                        (type),
                                                    EUnite::U_,
                                                    decimales)),
               std::shared_ptr <INb> (new NbCalcul (param.get ()->getpsi2
                                                                        (type),
                                                    EUnite::U_,
                                                    decimales))),
             false,
             &action->getUndoManager ())
  }
  
  BUGCONT (action->getUndoManager ().unref (),
           false,
           &action->getUndoManager ())
  
  return true;
}

CAction *
CModele::getAction (std::string nom) const
{
  for (std::shared_ptr <CAction> action : actions)
  {
    if (nom.compare (*action.get ()->getNom ()) == 0)
    {
      return action.get ();
    }
  }
  
  return NULL;
}

size_t
CModele::getActionCount () const
{
  return actions.size ();
}

bool
CModele::rmAction (std::shared_ptr <CAction> action)
{
  BUGPARAM (static_cast <void *> (action.get ()),
            "%p",
            action,
            false,
            &action->getUndoManager ())
  
  BUGPROG (action.get ()->emptyCharges (),
           false,
           &action.get ()->getUndoManager (),
           gettext ("L'action doit être supprimée sans charge. Elles doivent être supprimées avant.\n"))
  
  BUGCONT (action.get ()->getUndoManager ().ref (),
           false,
           &action.get ()->getUndoManager ())

  std::list <std::shared_ptr <CAction> >::iterator it;

  for (it = actions.begin (); it != actions.end (); ++it)
  {
    if ((*it).get () == action.get ())
    {
      actions.erase (it);
      break;
    }
  }
  BUGCONT (action.get ()->getUndoManager ().push (
             std::bind (&CModele::addAction,
                        this,
                        action),
             std::bind (static_cast <bool (CModele::*)
                             (std::shared_ptr <CAction>)> (&CModele::rmAction),
                        this,
                        action),
             nullptr,
             nullptr,
             format (gettext ("Suppression de l'action “%s”"),
                     action.get ()->getNom ()->c_str ())),
           false,
           &action.get ()->getUndoManager ())
  
  BUGCONT (action.get ()->getUndoManager ().unref (),
           false,
           &action.get ()->getUndoManager ());
  
  return true;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
