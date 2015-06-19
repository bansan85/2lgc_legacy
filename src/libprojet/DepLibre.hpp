#ifndef DEPLIBRE__HPP
#define DEPLIBRE__HPP

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

#include "IDeplacementType.hpp"

/**
 * \brief Type de déplacement libre.
 */
class DepLibre : public IDeplacementType
{
  // Operations
  public :
    /**
     * \brief Constructeur d'une classe DepLibre.
     * \param undo (in) Le gestionnaire des modifications.
     */
    DepLibre (UndoManager & undo);
    /**
     * \brief Duplication d'une classe DepLibre.
     * \param other (in) La classe à dupliquer.
     */
    DepLibre (const DepLibre & other) = delete;
    /**
     * \brief Assignment operator de DepLibre.
     * \param other (in) La classe à dupliquer.
     * \return DepLibre &
     */
    DepLibre & operator = (const DepLibre & other) = delete;
    /**
     * \brief Destructeur d'une classe DepLibre.
     */
    virtual ~DepLibre ();
};

#endif
