#ifndef POCO_STR_CRELACHEMENT__HPP
#define POCO_STR_CRELACHEMENT__HPP

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

namespace POCO
{
  namespace str
  {
    /**
     * \brief Données définissant un relâchement.
     */
    class CRelachement
    {
      // Attributes
      private :
        /// Nom de l'appui.
        std::string nom;
        /// Degré de liberté en rotation autour de l'axe x en début de la barre.
        POCO::str::IDeplacementType * rxd;
        /// Degré de liberté en rotation autour de l'axe y en début de la barre.
        POCO::str::IDeplacementType * ryd;
        /// Degré de liberté en rotation autour de l'axe z en début de la barre.
        POCO::str::IDeplacementType * rzd;
        /// Degré de liberté en rotation autour de l'axe x en fin de la barre.
        POCO::str::IDeplacementType * rxf;
        /// Degré de liberté en rotation autour de l'axe y en fin de la barre.
        POCO::str::IDeplacementType * ryf;
        /// Degré de liberté en rotation autour de l'axe z en fin de la barre.
        POCO::str::IDeplacementType * rzf;
      // Operations
      public :
        /**
         * \brief Constructeur d'une classe CRelachement.
         * \param nom (in) Nom du relâchement.
         * \param rxd (in) Le type de relâchement autour de l'axe x en début de barre.
         * \param ryd (in) Le type de relâchement autour de l'axe y en début de barre.
         * \param rzd (in) Le type de relâchement autour de l'axe z en début de barre.
         * \param rxf (in) Le type de relâchement autour de l'axe x en fin de barre.
         * \param ryf (in) Le type de relâchement autour de l'axe y en fin de barre.
         * \param rzf (in) Le type de relâchement autour de l'axe z en fin de barre.
         */
        CRelachement (std::string nom, POCO::str::IDeplacementType * rxd, POCO::str::IDeplacementType * ryd, POCO::str::IDeplacementType * rzd, POCO::str::IDeplacementType * rxf, POCO::str::IDeplacementType * ryf, POCO::str::IDeplacementType * rzf);
        /**
         * \brief Duplication d'une classe CRelachement.
         * \param other (in) La classe à dupliquer.
         */
        CRelachement (const CRelachement & other) = delete;
        /**
         * \brief Assignment operator de CRelachement.
         * \param other (in) La classe à dupliquer.
         * \return CRelachement &
         */
        CRelachement & operator = (const CRelachement & other) = delete;
        /**
         * \brief Destructeur d'une classe CRelachement.
         */
        virtual ~CRelachement ();
        /**
         * \brief Renvoie le nom du relâchement.
         * \return std::string const &
         */
        std::string const & getNom () const;
        /**
         * \brief Défini le nom du relâchement.
         * \param val (in) La nouvelle valeur.
         * \return bool CHK
         */
        bool CHK setNom (std::string val);
        /**
         * \brief Renvoie le type de relâchement autour de l'axe x en début de barre.
         * \return POCO::str::IDeplacementType const &
         */
        POCO::str::IDeplacementType const & getrxd () const;
        /**
         * \brief Modifie le type de relâchement autour de l'axe x en début de barre.
         * \param val (in) La nouvelle valeur.
         * \return bool CHK
         */
        bool CHK setrxd (POCO::str::IDeplacementType * val);
        /**
         * \brief Renvoie le type de relâchement autour de l'axe y en début de barre.
         * \return POCO::str::IDeplacementType const &
         */
        POCO::str::IDeplacementType const & getryd () const;
        /**
         * \brief Modifie le type de relâchement autour de l'axe y en début de barre.
         * \param val (in) La nouvelle valeur.
         * \return bool CHK
         */
        bool CHK setryd (POCO::str::IDeplacementType * val);
        /**
         * \brief Renvoie le type de relâchement autour de l'axe z en début de barre.
         * \return POCO::str::IDeplacementType const &
         */
        POCO::str::IDeplacementType const & getrzd () const;
        /**
         * \brief Modifie le type de relâchement autour de l'axe z en début de barre.
         * \param val (in) La nouvelle valeur.
         * \return bool CHK
         */
        bool CHK setrzd (POCO::str::IDeplacementType * val);
        /**
         * \brief Renvoie le type de relâchement autour de l'axe x en fin de barre.
         * \return POCO::str::IDeplacementType const &
         */
        POCO::str::IDeplacementType const & getrxf () const;
        /**
         * \brief Modifie le type de relâchement autour de l'axe x en fin de barre.
         * \param val (in) La nouvelle valeur.
         * \return bool CHK
         */
        bool CHK setrxf (POCO::str::IDeplacementType * val);
        /**
         * \brief Renvoie le type de relâchement autour de l'axe y en fin de barre.
         * \return POCO::str::IDeplacementType const &
         */
        POCO::str::IDeplacementType const & getryf () const;
        /**
         * \brief Modifie le type de relâchement autour de l'axe y en fin de barre.
         * \param val (in) La nouvelle valeur.
         * \return bool CHK
         */
        bool CHK setryf (POCO::str::IDeplacementType * val);
        /**
         * \brief Renvoie le type de relâchement autour de l'axe z en fin de barre.
         * \return POCO::str::IDeplacementType const &
         */
        POCO::str::IDeplacementType const & getrzf () const;
        /**
         * \brief Modifie le type de relâchement autour de l'axe z en fin de barre.
         * \param val (in) La nouvelle valeur.
         * \return bool CHK
         */
        bool CHK setrzf (POCO::str::IDeplacementType * val);
    };
  }
}

#endif
