#ifndef POCO_STR_IMATERIAU__HPP
#define POCO_STR_IMATERIAU__HPP

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

#include "POCO/INb.hpp"

namespace POCO
{
  namespace str
  {
    /**
     * \brief Défini un matériau type.
     */
    class IMateriau
    {
      // Attributes
      private :
        /// Nom du matériau.
        std::string nom;
        /// Module Young.
        POCO::INb * E;
        /// Cœfficient de poisson.
        POCO::INb * nu;
      // Operations
      public :
        /**
         * \brief Constructeur d'une classe IMateriau.
         */
        IMateriau ();
        /**
         * \brief Duplication d'une classe CMatAcierEc.
         * \param other (in) La classe à dupliquer.
         */
        IMateriau (const IMateriau & other) = delete;
        /**
         * \brief Duplication d'une classe CMatAcierEc.
         * \param other (in) La classe à dupliquer.
         * \return IMateriau &
         */
        IMateriau & operator = (const IMateriau & other) = delete;
        /**
         * \brief Destructeur d'une interface IMateriau.
         */
        virtual ~IMateriau ();
        /**
         * \brief Renvoie le nom du matériau.
         * \return std::string const &
         */
        std::string const & getNom () const;
        /**
         * \brief Défini le nom du matériau.
         * \param valeur (in) La nouvelle valeur.
         * \return bool CHK
         */
        bool CHK setNom (std::string valeur);
        /**
         * \brief Renvoie le module Young en Pa.
         * \return POCO::INb const &
         */
        POCO::INb const & getE () const;
        /**
         * \brief Défini le module Young en Pa.
         * \param val (in) La nouvelle valeur.
         * \return bool CHK
         */
        bool CHK setE (POCO::INb * val);
        /**
         * \brief Renvoie le cœfficient de poisson.
         * \return POCO::INb const &
         */
        POCO::INb const & getnu () const;
        /**
         * \brief Défini le cœfficient de poisson.
         * \param val (in) La nouvelle valeur.
         * \return bool CHK
         */
        bool CHK setnu (POCO::INb * val);
    };
  }
}

#endif
