#ifndef POCO_SOL_ICHARGE__HPP
#define POCO_SOL_ICHARGE__HPP

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

#include <string>
#include "POCO/INb.hpp"

namespace POCO
{
  namespace sol
  {
    /**
     * \brief Défini une charge type.
     */
    class ICharge
    {
      // Attributes
      private :
        /// Nom de la charge.
        std::string nom;
      // Operations
      public :
        /**
         * \brief Constructeur d'une interface ICharge.
         */
        ICharge ();
        /**
         * \brief Duplication d'une classe ICharge.
         * \param other (in) La classe à dupliquer.
         */
        ICharge (const ICharge & other) = delete;
        /**
         * \brief Assignment operator de ICharge.
         * \param other (in) La classe à dupliquer.
         * \return ICharge &
         */
        ICharge & operator = (const ICharge & other) = delete;
        /**
         * \brief Destructeur d'une interface ICharge.
         */
        virtual ~ICharge ();
        /**
         * \brief Renvoie le nom de la charge.
         * \return std::string const &
         */
        std::string const & getNom () const;
        /**
         * \brief Défini le nom de la charge.
         * \param val (in) La nouvelle valeur.
         * \return bool CHK
         */
        bool CHK setNom (std::string val);
    };
  }
}

#endif
