#ifndef POCO_STR_CPOINT__HPP
#define POCO_STR_CPOINT__HPP

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
     * \brief Défini un point en 3D.
     */
    class CPoint
    {
      // Attributes
      private :
        /// Coordonnée en x.
        POCO::INb * x;
        /// Coordonnée en y.
        POCO::INb * y;
        /// Coordonnée en z.
        POCO::INb * z;
      // Operations
      public :
        /**
         * \brief Constructeur d'une classe CPoint.
         * \param x (in) Coordonnée en x.
         * \param y (in) Coordonnée en y.
         * \param z (in) Coordonnée en z.
         */
        CPoint (POCO::INb * x, POCO::INb * y, POCO::INb * z);
        /**
         * \brief Duplication d'une classe CPoint.
         * \param other (in) La classe à dupliquer.
         */
        CPoint (const CPoint & other) = delete;
        /**
         * \brief Assignment operator de CPoint.
         * \param other (in) La classe à dupliquer.
         * \return CPoint &
         */
        CPoint & operator = (const CPoint & other) = delete;
        /**
         * \brief Destructeur d'une classe CPoint.
         */
        virtual ~CPoint ();
        /**
         * \brief Renvoie la coordonnée en x.
         * \return POCO::INb const &
         */
        POCO::INb const & getX () const;
        /**
         * \brief Modifie la coordonnée en x.
         * \param valeur (in) La nouvelle valeur en x.
         * \return bool CHK
         */
        bool CHK setX (POCO::INb * valeur);
        /**
         * \brief Renvoie la coordonnée en y.
         * \return POCO::INb const &
         */
        POCO::INb const & getY () const;
        /**
         * \brief Modifie la coordonnée en y.
         * \param valeur (in) La nouvelle valeur en y.
         * \return bool CHK
         */
        bool CHK setY (POCO::INb * valeur);
        /**
         * \brief Renvoie la coordonnée en z.
         * \return POCO::INb const &
         */
        POCO::INb const & getZ () const;
        /**
         * \brief Modifie la coordonnée en z.
         * \param valeur (in) La nouvelle valeur en z.
         * \return bool CHK
         */
        bool CHK setZ (POCO::INb * valeur);
        /**
         * \brief Modifie les coordonnées en x, y et z.
         * \param x (in) La nouvelle valeur en x.
         * \param y (in) La nouvelle valeur en y.
         * \param z (in) La nouvelle valeur en z.
         * \return bool CHK
         */
        bool CHK setXYZ (POCO::INb * x, POCO::INb * y, POCO::INb * z);
    };
  }
}

#endif
