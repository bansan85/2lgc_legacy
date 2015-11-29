#ifndef POCO_STR_SECTION_CARREE__HPP
#define POCO_STR_SECTION_CARREE__HPP

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
       * \brief Défini une section carrée.
       */
      class Carree : public POCO::str::ISection
      {
        // Attributes
        private :
          /// Largeur et hauteur de la section.
          INb * cote;
        // Operations
        public :
          /**
           * \brief Constructeur d'une classe Carree.
           * \param cote (in) Largeur et hauteur de la section.
           */
          Carree (INb * cote);
          /**
           * \brief Duplication d'une classe Carree.
           * \param other (in) La classe à dupliquer.
           */
          Carree (const Carree & other) = delete;
          /**
           * \brief Assignment operator de Carree.
           * \param other (in) La classe à dupliquer.
           * \return Carree &
           */
          Carree & operator = (const Carree & other) = delete;
          /**
           * \brief Destructeur d'une classe Carree.
           */
          virtual ~Carree ();
          /**
           * \brief Renvoie la dimension du coté de la section.
           * \return INb const &
           */
          INb const & getCote () const;
          /**
           * \brief Défini la dimension du coté de la section.
           * \param val (in) La nouvelle valeur.
           * \return bool CHK
           */
          bool CHK setCote (INb * val);
      };
    }
  }
}

#endif
