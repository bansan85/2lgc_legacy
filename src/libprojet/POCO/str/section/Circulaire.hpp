#ifndef POCO_STR_SECTION_CIRCULAIRE__HPP
#define POCO_STR_SECTION_CIRCULAIRE__HPP

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

#include "POCO/str/ISection.hpp"

namespace POCO
{
  namespace str
  {
    namespace section
    {
      /**
       * \brief Défini une section circulaire.
       */
      class Circulaire : public POCO::str::ISection
      {
        // Attributes
        private :
          /// Diamètre de la section.
          INb * diametre;
        // Operations
        public :
          /**
           * \brief Constructeur d'une classe Circulaire.
           * \param diametre (in) Diamètre de la section.
           */
          Circulaire (INb * diametre);
          /**
           * \brief Duplication d'une classe Circulaire.
           * \param other (in) La classe à dupliquer.
           */
          Circulaire (const Circulaire & other) = delete;
          /**
           * \brief Assignment operator de Circulaire.
           * \param other (in) La classe à dupliquer.
           * \return Circulaire &
           */
          Circulaire & operator = (const Circulaire & other) = delete;
          /**
           * \brief Destructeur d'une classe Circulaire.
           */
          virtual ~Circulaire ();
          /**
           * \brief Renvoie la dimension du diamètre de la section.
           * \return INb const &
           */
          INb const & getDiametre () const;
          /**
           * \brief Défini la dimension du diamètre de la section.
           * \param val (in) La nouvelle valeur.
           * \return bool CHK
           */
          bool CHK setDiametre (INb * val);
      };
    }
  }
}

#endif
