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
#include "MErreurs.hh"

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
 * \brief Ajout d'une action.
 * \param action (in) L'action à ajouter.
 * \return false en cas de problème.
 */
bool
CModeleActions::addAction (CAction * action)
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
  
  BUG (action->getUndoManager ().ref (), false)
  
  actions.push_back (action);
  BUG (action->getUndoManager ().push (
         std::bind (static_cast <bool (CModeleActions::*)
                                      (CAction *)> (&CModeleActions::rmAction),
                    this,
                    action),
         std::bind (&CModeleActions::addAction, this, action),
         std::bind (std::default_delete <CAction> (), action),
         std::bind (&CModeleActions::addActionXML,
                    this,
                    std::placeholders::_1,
                    action)),
       false)
  
  BUG (action->getUndoManager ().unref (), false)
  
  return true;
}


/**
 * \brief Converti la fonction d'ajout d'une action sous format XML..
 * \param root (in) Le noeud dans lequel doit être inséré l'action.
 */
bool
CModeleActions::addActionXML (xmlNodePtr root, CAction * action)
{
  BUGPARAMCRIT (action, "%p", action, false)
  
  std::unique_ptr <xmlNode, void (*)(xmlNodePtr)> node (
    xmlNewNode (NULL, reinterpret_cast <const xmlChar *> ("addAction")),
    xmlFreeNode);
  
  BUG (node.get (), false, (gettext ("Erreur d'allocation mémoire.\n")); )
  
  BUG (xmlSetProp (
         node.get (),
         reinterpret_cast <const xmlChar *> ("Nom"),
         reinterpret_cast <const xmlChar *> (action->getNom ().c_str ())),
       false)
  
  BUGCRIT (xmlAddChild (root, node.get ()),
           false,
           (gettext ("Erreur lors de la génération du fichier XML.\n")); )
  
  node.release ();
  
  return true;
}


/**
 * \brief Renvoie le nombre d'actions.
 */
size_t
CModeleActions::getActionCount ()
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
  BUGPARAMCRIT (action, "%p", action, false)
  
  BUG (action->emptyCharges (),
       false,
       (gettext ("L'action doit être supprimée sans action. Les charges doivent être supprimées avant.\n")); )
  
  BUG (action->getUndoManager().ref (), false)
  
  actions.remove (action);
  BUG (action->getUndoManager ().push (
         std::bind (&CModeleActions::addAction, this, action),
         std::bind (static_cast <bool (CModeleActions::*) (CAction *)>
                                                   (&CModeleActions::rmAction),
                    this,
                    action),
         NULL,
         NULL),
       false)
  
  BUG (action->getUndoManager ().unref (), false);
  
  return true;
}


/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
