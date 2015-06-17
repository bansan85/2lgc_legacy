#ifndef IUNDOABLE__HPP
#define IUNDOABLE__HPP

/*
2lgc_code : calcul de résistance des matériaux selon les normes Eurocodes
Copyright (C) 2011

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

Fichier généré automatiquement avec dia2code 0.9.0.
 */

#include "UndoManager.hpp"

/**
 * \brief Défini un modèle pour toutes les classes devant gérer des modifications annulables.
 */
class DllExport IUndoable
{
  // Attributes
  private :
    /// Mémorisation des modifications.
    UndoManager & undoManager;
  // Operations
  public :
    /**
     * \brief Constructeur d'une interface IUndoable.
     * \param undo_ (in) Le gestionnaire des modifications.
     */
    IUndoable (UndoManager & undo_);
    /**
     * \brief Duplication d'une interface IUndoable.
     * \param other (in) La classe à dupliquer.
     */
    IUndoable (const IUndoable & other) = delete;
    /**
     * \brief Duplication d'une interface IUndoable.
     * \param other (in) La classe à dupliquer.
     * \return IUndoable &
     */
    IUndoable & operator = (const IUndoable & other) = delete;
    /**
     * \brief Destructeur d'une interface IUndoable.
     */
    virtual ~IUndoable ();
    /**
     * \brief Renvoie le gestionnaire d'annulations.
     * \return UndoManager &
     */
    UndoManager & getUndoManager () const;
};

#endif
