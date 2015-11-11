#ifndef POCO_STR_DEPLACEMENTTYPE_LINEAIRE__HPP
#define POCO_STR_DEPLACEMENTTYPE_LINEAIRE__HPP

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
#include "POCO/INb.hpp"

namespace POCO
{
  namespace str
  {
    namespace deplacementType
    {
      /**
       * \brief Type de déplacement linéaire.
       */
      class Lineaire : public POCO::str::IDeplacementType
      {
        // Attributes
        private :
          /// La raideur doit être indiquée en N.m/rad.
          INb * raideur;
        // Operations
        public :
          /**
           * \brief Constructeur d'une classe Lineaire.
           * \param raideur (in) La raideur du déplacement.
           * \param undo (in) Le gestionnaire des modifications.
           */
          Lineaire (INb * raideur, UndoManager & undo);
          /**
           * \brief Duplication d'une classe Lineaire.
           * \param other (in) La classe à dupliquer.
           */
          Lineaire (const Lineaire & other) = delete;
          /**
           * \brief Assignment operator de Lineaire.
           * \param other (in) La classe à dupliquer.
           * \return Lineaire &
           */
          Lineaire & operator = (const Lineaire & other) = delete;
          /**
           * \brief Destructeur d'une classe Lineaire.
           */
          virtual ~Lineaire ();
          /**
           * \brief Renvoie la raideur du déplacement.
           * \return INb const &
           */
          INb const & getRaideur () const;
          /**
           * \brief Défini la raideur du déplacement.
           * \param raideur (in) La nouvelle raideur.
           * \return bool CHK
           */
          bool CHK setRaideur (INb * raideur);
      };
    }
  }
}

#endif
