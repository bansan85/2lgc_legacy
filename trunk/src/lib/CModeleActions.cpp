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

#include <cmath>
#include <iostream>
#include <memory>

#include "CModeleActions.hpp"
#include "CNbUser.hpp"
#include "CProjet.hpp"
#include "MErreurs.hh"

#include "CParamEC.hpp"

/**
 * \brief Constructeur d'une classe CModeleActions.
 */
CModeleActions::CModeleActions () :
  actions ()
{
}


/**
 * \brief Duplication d'une classe CModeleActions.
 * \param other (in) La classe à dupliquer.
 */
CModeleActions::CModeleActions (const CModeleActions & other) = delete;


/**
 * \brief Assignment operator de CModeleActions.
 * \param other (in) La classe à dupliquer.
 */
CModeleActions &
CModeleActions::operator = (const CModeleActions & other) = delete;


/**
 * \brief Destructeur d'une classe CModeleActions.
 */
CModeleActions::~CModeleActions ()
{
}


/**
 * \brief Ajout d'une action. La fonction XML est CAction::addXML.
 * \param action (in) L'action à ajouter.
 * \return false en cas de problème.
 */
bool
CModeleActions::addAction (CAction * action)
{
  BUGPARAM (action, "%p", action, false, &action->getUndoManager ())
  
  BUGCRIT (actions.max_size () != actions.size (),
           false,
           &action->getUndoManager (),
           gettext ("Le programme est à ses limites.\n"))
  
  BUGUSER (this->getAction (*action->getNom ()) == NULL,
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
  BUGCONT (action->getUndoManager ().push (
             std::bind (static_cast <bool (CModeleActions::*) (CAction *)>
                                                   (&CModeleActions::rmAction),
                        this,
                        action),
             std::bind (&CModeleActions::addAction, this, action),
             std::bind (std::default_delete <CAction> (), action),
             std::bind (&CAction::addXML,
                        action,
                        action->getNom (),
                        action->getType (),
                        std::placeholders::_1)),
           false,
           &action->getUndoManager ())
  BUGCONT (action->getUndoManager ().pushSuppr (std::bind
                    (std::default_delete <std::string> (), action->getNom ())),
           false,
           &action->getUndoManager ())
  
  BUGCONT (action->setParam (dynamic_cast <CProjet *> (this)->getParametres (),
                             dynamic_cast <CProjet *> (this)->getDecimales ()),
           false,
           &action->getUndoManager ())
  
  BUGCONT (action->getUndoManager ().unref (),
           false,
           &action->getUndoManager ())
  
  return true;
}


/**
 * \brief Recherche une action.
 * \param nom (in) Le nom de l'action.
 */
CAction *
CModeleActions::getAction (std::string nom)
{
  for (CAction * action : this->actions)
  {
    if (nom.compare (*action->getNom ()) == 0)
    {
      return action;
    }
  }
  
  return NULL;
}


/**
 * \brief Renvoie le nombre d'actions.
 */
size_t
CModeleActions::getActionCount () const
{
  return actions.size ();
}


/**
 * \brief Supprime d'une action.
 * \param action (in) L'action à supprimer.
 * \return false en cas de problème.
 */
bool
CModeleActions::rmAction (CAction * action)
{
  BUGPARAM (action, "%p", action, false, &action->getUndoManager ())
  
  BUGPROG (action->emptyCharges (),
           false,
           &action->getUndoManager (),
           gettext ("L'action doit être supprimée sans charge. Elles doivent être supprimées avant.\n"))
  
  BUGCONT (action->getUndoManager ().ref (),
           false,
           &action->getUndoManager ())
  
  actions.remove (action);
  BUGCONT (action->getUndoManager ().push (
             std::bind (&CModeleActions::addAction, this, action),
             std::bind (static_cast <bool (CModeleActions::*) (CAction *)>
                                                   (&CModeleActions::rmAction),
                        this,
                        action),
             NULL,
             NULL),
           false,
           &action->getUndoManager ())
  
  BUGCONT (action->getUndoManager ().unref (),
           false,
           &action->getUndoManager ());
  
  return true;
}


/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
