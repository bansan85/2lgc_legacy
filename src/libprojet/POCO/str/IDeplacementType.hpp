#ifndef POCO_STR_IDEPLACEMENTTYPE__HPP
#define POCO_STR_IDEPLACEMENTTYPE__HPP

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

namespace POCO
{
  namespace str
  {
    /**
     * \brief Défini les différents types de déplacement pour les appuis et les relâchements.
     */
    class IDeplacementType
    {
      // Attributes
      private :
        /// Nom du type d'appui.
        std::string nom;
      // Operations
      public :
        /**
         * \brief Constructeur d'une interface IDeplacementType.
         */
        IDeplacementType ();
        /**
         * \brief Duplication d'une interface IDeplacementType.
         * \param other (???) La classe à dupliquer.
         */
        IDeplacementType (const IDeplacementType & other) = delete;
        /**
         * \brief Duplication d'une interface IDeplacementType.
         * \param other (in) La classe à dupliquer.
         * \return IDeplacementType &
         */
        IDeplacementType & operator = (const IDeplacementType & other) = delete;
        /**
         * \brief Destructeur d'une interface IDeplacementType.
         */
        virtual ~IDeplacementType ();
        /**
         * \brief Renvoie le nom du type d'appui.
         * \return std::string const &
         */
        std::string const & getNom () const;
        /**
         * \brief Défini le nom du type d'appui.
         * \param val (in) La nouvelle valeur.
         * \return bool CHK
         */
        bool CHK setNom (std::string val);
    };
  }
}

#endif
