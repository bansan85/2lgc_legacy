#ifndef POCO_STR_CAPPUI__HPP
#define POCO_STR_CAPPUI__HPP

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
#include "POCO/str/IDeplacementType.hpp"

namespace POCO
{
  namespace str
  {
    /**
     * \brief Données définissant un appui.
     */
    class CAppui
    {
      // Attributes
      private :
        /// Nom de l'appui.
        std::string nom;
        /// Degré de liberté de la direction x.
        POCO::str::IDeplacementType * ux;
        /// Degré de liberté de la direction y.
        POCO::str::IDeplacementType * uy;
        /// Degré de liberté de la direction z.
        POCO::str::IDeplacementType * uz;
        /// Degré de liberté en rotation autour de l'axe x.
        POCO::str::IDeplacementType * rx;
        /// Degré de liberté en rotation autour de l'axe y.
        POCO::str::IDeplacementType * ry;
        /// Degré de liberté en rotation autour de l'axe z.
        POCO::str::IDeplacementType * rz;
      // Operations
      public :
        /**
         * \brief Constructeur d'une classe CAppui.
         * \param nom (in) Nom de l'appui.
         * \param ux (in) Le type d'appui selon l'axe x.
         * \param uy (in) Le type d'appui selon l'axe y.
         * \param uz (in) Le type d'appui selon l'axe z.
         * \param rx (in) Le type d'appui autour de l'axe x.
         * \param ry (in) Le type d'appui autour de l'axe y.
         * \param rz (in) Le type d'appui autour de l'axe z.
         */
        CAppui (std::string nom, POCO::str::IDeplacementType * ux, POCO::str::IDeplacementType * uy, POCO::str::IDeplacementType * uz, POCO::str::IDeplacementType * rx, POCO::str::IDeplacementType * ry, POCO::str::IDeplacementType * rz);
        /**
         * \brief Duplication d'une classe CAppui.
         * \param other (in) La classe à dupliquer.
         */
        CAppui (const CAppui & other) = delete;
        /**
         * \brief Assignment operator de CAppui.
         * \param other (in) La classe à dupliquer.
         * \return CAppui &
         */
        CAppui & operator = (const CAppui & other) = delete;
        /**
         * \brief Destructeur d'une classe CAppui.
         */
        virtual ~CAppui ();
        /**
         * \brief Renvoie le nom de l'appui.
         * \return std::string const &
         */
        std::string const & getNom () const;
        /**
         * \brief Difini le nom de l'appui.
         * \param val (in) La nouvelle valeur.
         * \return bool CHK
         */
        bool CHK setNom (std::string val);
        /**
         * \brief Renvoie le type d'appui selon l'axe x.
         * \return POCO::str::IDeplacementType const &
         */
        POCO::str::IDeplacementType const & getux () const;
        /**
         * \brief Modifie le type d'appui selon l'axe x.
         * \param val (in) La nouvelle valeur.
         * \return bool CHK
         */
        bool CHK setux (POCO::str::IDeplacementType * val);
        /**
         * \brief Renvoie le type d'appui selon l'axe y.
         * \return POCO::str::IDeplacementType const &
         */
        POCO::str::IDeplacementType const & getuy () const;
        /**
         * \brief Modifie le type d'appui selon l'axe y.
         * \param val (in) La nouvelle valeur.
         * \return bool CHK
         */
        bool CHK setuy (POCO::str::IDeplacementType * val);
        /**
         * \brief Renvoie le type d'appui selon l'axe z.
         * \return POCO::str::IDeplacementType const &
         */
        POCO::str::IDeplacementType const & getuz () const;
        /**
         * \brief Modifie le type d'appui selon l'axe z.
         * \param val (in) La nouvelle valeur.
         * \return bool CHK
         */
        bool CHK setuz (POCO::str::IDeplacementType * val);
        /**
         * \brief Renvoie le type d'appui autour de l'axe x.
         * \return POCO::str::IDeplacementType const &
         */
        POCO::str::IDeplacementType const & getrx () const;
        /**
         * \brief Modifie le type d'appui autour de l'axe x.
         * \param val (in) La nouvelle valeur.
         * \return bool CHK
         */
        bool CHK setrx (POCO::str::IDeplacementType * val);
        /**
         * \brief Renvoie le type d'appui autour de l'axe y.
         * \return POCO::str::IDeplacementType const &
         */
        POCO::str::IDeplacementType const & getry () const;
        /**
         * \brief Modifie le type d'appui autour de l'axe y.
         * \param val (in) La nouvelle valeur.
         * \return bool CHK
         */
        bool CHK setry (POCO::str::IDeplacementType * val);
        /**
         * \brief Renvoie le type d'appui autour de l'axe z.
         * \return POCO::str::IDeplacementType const &
         */
        POCO::str::IDeplacementType const & getrz () const;
        /**
         * \brief Modifie le type d'appui autour de l'axe z.
         * \param val (in) La nouvelle valeur.
         * \return bool CHK
         */
        bool CHK setrz (POCO::str::IDeplacementType * val);
    };
  }
}

#endif
