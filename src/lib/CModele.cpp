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


#include "CModele.hpp"
#include "CNbCalcul.hpp"
#include "CProjet.hpp"
#include "MErreurs.hh"

/**
 * \brief Initialise le modèle de calcul.
 */
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


/**
 * \brief Libère le modèle de calcul.
 */
CModele::~CModele ()
{
  // Ce n'est pas modèle qui libère la mémoire, c'est le gestionnaire
  // d'annulation.
}


/**
 * \brief Ajout d'une action. La fonction XML est CAction::addXML.
 * \param action (in) L'action à ajouter.
 * \return false en cas de problème.
 */
bool
CModele::addAction (CAction * action)
{
  BUGPARAM (action, "%p", action, false, &action->getUndoManager ())
  
  BUGCRIT (actions.max_size () != actions.size (),
           false,
           &action->getUndoManager (),
           gettext ("Le programme est à ses limites.\n"))
  
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
  BUGCONT (action->getUndoManager ().push (
             std::bind (static_cast <bool (CModele::*) (CAction *)>
                                                          (&CModele::rmAction),
                        this,
                        action),
             std::bind (&CModele::addAction, this, action),
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
  
  IParametres *param = dynamic_cast <CProjet *> (this)->getParametres ();
  uint8_t type = action->getType ();
  std::array <uint8_t, U_LAST> decimales = dynamic_cast <CProjet *>
                                                       (this)->getDecimales ();
  
  if (action->getUndoManager ().getEtat () != UNDO_NONE_OR_REVERT)
  {
    BUGCONT (action->setParam (
               param,
               new CNbCalcul (param->getpsi0 (type), U_, decimales),
               new CNbCalcul (param->getpsi1 (type), U_, decimales),
               new CNbCalcul (param->getpsi2 (type), U_, decimales)),
             false,
             &action->getUndoManager ())
  }
  
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
CModele::getAction (std::string nom)
{
  for (CAction * action : actions)
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
CModele::getActionCount () const
{
  return actions.size ();
}


/**
 * \brief Supprime d'une action.
 * \param action (in) L'action à supprimer.
 * \return false en cas de problème.
 */
bool
CModele::rmAction (CAction * action)
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
             std::bind (&CModele::addAction, this, action),
             std::bind (static_cast <bool (CModele::*) (CAction *)>
                                                          (&CModele::rmAction),
                        this,
                        action),
             nullptr,
             nullptr),
           false,
           &action->getUndoManager ())
  
  BUGCONT (action->getUndoManager ().unref (),
           false,
           &action->getUndoManager ());
  
  return true;
}


/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
