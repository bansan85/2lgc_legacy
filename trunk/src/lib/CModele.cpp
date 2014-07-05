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
#include <memory>
#include <iostream>
#include <functional>

#include "MErreurs.hh"
#include "CModele.hpp"

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
 * \brief Duplication d'une classe CModele.
 * \param other (in) La classe à dupliquer.
 */
CModele::CModele (const CModele & other) :
  appuis (),
  noeuds (),
  sections (),
  materiaux (),
  relachements (),
  barres (),
  actions (),
  niveaux_groupes ()
{
  std::cout << __func__ ;
  //TODO : A FINIR.
}


/**
 * \brief Assignment operator de CModele.
 * \param other (in) La classe à dupliquer.
 */
CModele &
CModele::operator = (const CModele & other)
{
  std::cout << __func__ ;
  //TODO : A FINIR.
  return *this;
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
 * \brief Ajout d'une action.
 * \param action (in) L'action à ajouter.
 * \return false en cas de problème.
 */
bool
CModele::addAction (CAction * action)
{
  BUGPARAMCRIT (action, "%p", action, false)
  
  BUGCRIT (actions.max_size () != actions.size (),
           false,
           (gettext ("Le programme est arrivé au boût de ces limites. Contactez le concepteur de la librairie.\n"));
             if (action->getUndoManager ().getEtat () != UNDO_REVERT)
             {
               delete action;
             } )
  
  //TODO : Empêcher les actions avec le même nom.
  BUG (action->emptyCharges (),
       false,
       (gettext ("L'action doit être ajoutée sans action. Les charges doivent être ajoutées ensuite.\n"));
         if (action->getUndoManager ().getEtat () != UNDO_REVERT)
         {
           delete action;
         } )
  
  action->getUndoManager().ref ();
  
  actions.push_back (action);
  action->getUndoManager().push (
    std::bind (static_cast <bool (CModele::*)
                                             (CAction *)> (&CModele::rmAction),
               this,
               action),
    std::bind (&CModele::addAction, this, action),
    std::bind (std::default_delete <CAction> (), action),
    NULL);
  
  action->getUndoManager().unref ();
  
  return true;
}


/**
 * \brief Renvoie le nombre d'actions.
 */
size_t
CModele::getActionCount ()
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
  BUGPARAMCRIT (action, "%p", action, false)
  
  BUG (action->emptyCharges (),
       false,
       (gettext ("L'action doit être supprimée sans action. Les charges doivent être supprimées avant.\n")); )
  
  action->getUndoManager().ref ();
  
  actions.remove (action);
  action->getUndoManager().push (
    std::bind (&CModele::addAction, this, action),
    std::bind (static_cast <bool (CModele::*) (CAction *)>
                                                          (&CModele::rmAction),
               this,
               action),
    NULL,
    NULL);
  
  action->getUndoManager().unref ();
  
  return true;
}


/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
