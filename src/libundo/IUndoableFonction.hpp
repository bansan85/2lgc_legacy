#ifndef IUNDOABLEFONCTION__HPP
#define IUNDOABLEFONCTION__HPP

/*
2lgc_code : calcul de résistance des matériaux selon les normes Eurocodes
Copyright (C) 2011-2015

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

#include <libxml/tree.h>
#include <string>

class IUndoableFonction
{
  // Attributes
  private :
    /// Flag permettant de savoir si la modification est importante ou non.
    bool important : 1;
  // Operations
  public :
    /**
     * \brief Constructeur d'une interface IUndoableFonction.
     * \param importance (in) Indique si la modification est importante ou non.
     */
    IUndoableFonction (bool importance);
    /**
     * \brief Duplication d'une interface IUndoableFonction.
     * \param other (in) La classe à dupliquer.
     */
    IUndoableFonction (const IUndoableFonction & other) = delete;
    /**
     * \brief Duplication d'une interface IUndoableFonction.
     * \param other (in) La classe à dupliquer.
     * \return IUndoableFonction &
     */
    IUndoableFonction & operator = (const IUndoableFonction & other) = delete;
    /**
     * \brief Destructeur d'une interface IUndoableFonction.
     */
    virtual ~IUndoableFonction ();
    /**
     * \brief La description de la modification.
     * \return const std::string
     */
    virtual const std::string doDescription () = 0;
    /**
     * \brief La description de l'annulation.
     * \return const std::string
     */
    virtual const std::string undoDescription () = 0;
    /**
     * \brief La fonction renvoie true si la classe modifie le modèle et false si elle ne modifie rien de significatif.
     * \return bool CHK
     */
    bool CHK getImportant () const;
};

#endif
