#ifndef POCO_STR_SECTION_T__HPP
#define POCO_STR_SECTION_T__HPP

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
       * \brief Défini une section en T.
       */
      class T : public POCO::str::ISection
      {
        // Attributes
        private :
          /// Largeur de la table de compression.
          INb * l_table;
          /// Hauteur de la table de compression.
          INb * h_table;
          /// Largeur de la retombée de la section.
          INb * l_retombee;
          /// Hauteur de la retombée de la section.
          INb * h_retombee;
        // Operations
        public :
          /**
           * \brief Constructeur d'une classe T.
           * \param l_table (in) Largeur de la table de compression.
           * \param h_table (in) Hauteur de la table de compression.
           * \param l_retombee (in) Largeur de la retombée de la section.
           * \param h_retombee (in) Hauteur de la retombée de la section.
           */
          T (INb * l_table, INb * h_table, INb * l_retombee, INb * h_retombee);
          /**
           * \brief Duplication d'une classe T.
           * \param other (in) La classe à dupliquer.
           */
          T (const T & other) = delete;
          /**
           * \brief Assignment operator de T.
           * \param other (in) La classe à dupliquer.
           * \return T &
           */
          T & operator = (const T & other) = delete;
          /**
           * \brief Destructeur d'une classe T.
           */
          virtual ~T ();
          /**
           * \brief Renvoie la largeur de la table de compression.
           * \return INb const &
           */
          INb const & getLTable () const;
          /**
           * \brief Défini la largeur de la table de compression.
           * \param val (in) La nouvelle valeur.
           * \return bool CHK
           */
          bool CHK setLTable (INb * val);
          /**
           * \brief Renvoie la hauteur de la table de compression.
           * \return INb const &
           */
          INb const & getHTable () const;
          /**
           * \brief Défini la hauteur de la table de compression.
           * \param val (in) La nouvelle valeur.
           * \return bool CHK
           */
          bool CHK setHTable (INb * val);
          /**
           * \brief Renvoie la largeur de la retombée.
           * \return INb const &
           */
          INb const & getLRetombee () const;
          /**
           * \brief Défini la largeur de la retombée.
           * \param val (in) La nouvelle valeur.
           * \return bool CHK
           */
          bool CHK setLRetombee (INb * val);
          /**
           * \brief Renvoie la hauteur de la retombée.
           * \return INb const &
           */
          INb const & getHRetombee () const;
          /**
           * \brief Défini la hauteur de la retombée.
           * \param val (in) La nouvelle valeur.
           * \return bool CHK
           */
          bool CHK setHRetombee (INb * val);
      };
    }
  }
}

#endif
