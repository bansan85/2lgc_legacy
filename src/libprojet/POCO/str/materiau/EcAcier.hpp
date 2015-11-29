#ifndef POCO_STR_MATERIAU_ECACIER__HPP
#define POCO_STR_MATERIAU_ECACIER__HPP

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

#include <POCO/str/IMateriau.hpp>

namespace POCO
{
  namespace str
  {
    namespace materiau
    {
      /**
       * \brief Défini un matériau acier selon l'Eurocode 3.
       */
      class EcAcier : public IMateriau
      {
        // Attributes
        private :
          /// Limite d’élasticité en Pa.
          INb * fy;
          /// Résistance à la traction en Pa.
          INb * fu;
        // Operations
        public :
          /**
           * \brief Constructeur d'une classe EcAcier.
           * \param fy (in) Limite d’élasticité en MPa.
           * \param fu (in) Résistance à la traction en MPa.
           */
          EcAcier (INb * fy, INb * fu);
          /**
           * \brief Duplication d'une classe EcAcier.
           * \param other (in) La classe à dupliquer.
           */
          EcAcier (const EcAcier & other) = delete;
          /**
           * \brief Assignment operator de EcAcier.
           * \param other (in) La classe à dupliquer.
           * \return EcAcier &
           */
          EcAcier & operator = (const EcAcier & other) = delete;
          /**
           * \brief Destructeur d'une classe CMatAcierEc.
           */
          virtual ~EcAcier ();
          /**
           * \brief Renvoie la limite d’élasticité en Pa.
           * \return INb const &
           */
          INb const & getfy () const;
          /**
           * \brief Défini la limite d’élasticité en Pa.
           * \param val (in) La nouvelle valeur.
           * \return bool CHK
           */
          bool CHK setfy (INb * val);
          /**
           * \brief Renvoie la résistance à la traction en Pa.
           * \return INb const &
           */
          INb const & getfu () const;
          /**
           * \brief Défini la résistance à la traction en Pa.
           * \param val (in) La nouvelle valeur.
           * \return bool CHK
           */
          bool CHK setfu (INb * val);
      };
    }
  }
}

#endif
