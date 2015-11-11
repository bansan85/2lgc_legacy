#ifndef POCO_STR_DEPLACEMENTTYPE_LIBRE__HPP
#define POCO_STR_DEPLACEMENTTYPE_LIBRE__HPP

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

#include "POCO/str/IDeplacementType.hpp"

namespace POCO
{
  namespace str
  {
    namespace deplacementType
    {
      /**
       * \brief Type de déplacement libre.
       */
      class Libre : public POCO::str::IDeplacementType
      {
        // Operations
        public :
          /**
           * \brief Constructeur d'une classe Libre.
           * \param undo (in) Le gestionnaire des modifications.
           */
          Libre (UndoManager & undo);
          /**
           * \brief Duplication d'une classe Libre.
           * \param other (in) La classe à dupliquer.
           */
          Libre (const Libre & other) = delete;
          /**
           * \brief Assignment operator de Libre.
           * \param other (in) La classe à dupliquer.
           * \return Libre &
           */
          Libre & operator = (const Libre & other) = delete;
          /**
           * \brief Destructeur d'une classe Libre.
           */
          virtual ~Libre ();
      };
    }
  }
}

#endif
