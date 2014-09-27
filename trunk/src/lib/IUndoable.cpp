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

#include <iostream>

#include "IUndoable.hpp"

/**
 * \brief Constructeur d'une classe IUndoable.
 * \param undo_ (in) Le gestionnaire des modifications.
 */
IUndoable::IUndoable (CUndoManager & undo_) :
  undoManager (undo_)
{
}


/**
 * \brief Duplication d'une classe IUndoable.
 * \param other (in) La classe à dupliquer.
 */
IUndoable::IUndoable (const IUndoable & other) :
  undoManager (other.undoManager)
{
}


/**
 * \brief Assignment operator de IUndoable.
 * \param other (in) La classe à dupliquer.
 */
//IUndoable &
//IUndoable::operator = (const IUndoable & other) = delete;


/**
 * \brief Destructeur d'une classe IUndoable.
 */
IUndoable::~IUndoable ()
{
}


/**
 * \brief Renvoie le gestionnaire d'annulations.
 */
CUndoManager &
IUndoable::getUndoManager () const
{
  return undoManager;
}


/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
