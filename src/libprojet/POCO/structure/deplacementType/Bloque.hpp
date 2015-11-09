#ifndef POCO_STRUCTURE_DEPLACEMENTTYPE_BLOQUE__HPP
#define POCO_STRUCTURE_DEPLACEMENTTYPE_BLOQUE__HPP

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

#include "POCO/structure/IDeplacementType.hpp"

namespace POCO
{
  namespace structure
  {
    namespace deplacementType
    {
      /**
       * \brief Type de déplacement bloqué.
       */
      class Bloque : public POCO::structure::IDeplacementType
      {
        // Operations
        public :
          /**
           * \brief Constructeur d'une classe Bloque.
           * \param undo (in) Le gestionnaire des modifications.
           */
          Bloque (UndoManager & undo);
          /**
           * \brief Duplication d'une classe Bloque.
           * \param other (in) La classe à dupliquer.
           */
          Bloque (const Bloque & other) = delete;
          /**
           * \brief Assignment operator de Bloque.
           * \param other (in) La classe à dupliquer.
           * \return Bloque &
           */
          Bloque & operator = (const Bloque & other) = delete;
          /**
           * \brief Destructeur d'une classe Bloque.
           */
          virtual ~Bloque ();
      };
    }
  }
}

#endif
