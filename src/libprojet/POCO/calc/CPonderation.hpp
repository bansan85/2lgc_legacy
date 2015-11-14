#ifndef POCO_CALC_CPONDERATION__HPP
#define POCO_CALC_CPONDERATION__HPP

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

class CPonderations;
class CCalculs;
class CModele;
class FuncModeleAction;
class CAction;

#include "POCO/sol/CAction.hpp"

namespace POCO
{
  namespace calc
  {
    /**
     * \brief Une pondération est calculée comme le produit de l'action par sa pondération et, si nécessaire, par le coefficient psi.
     */
    class CPonderation
    {
      // Attributes
      private :
        /// Action à manipuler.
        POCO::sol::CAction * action;
        /// La pondération à appliquer.
        double ponderation;
        /// psi vaut toujours -1 s'il ne s'agit pas d'une action variable sinon, psi vaut 0, 1 ou 2 pour que cette action soit pondérée avec soit psi0, psi1 ou psi2 en combinaison d'accompagnement.
        int8_t psi;
        /// Ce flag vaut 1 si cette action est une action dominante. 0 s'il s'agit d'une action d'accompagnement.
        uint8_t flags : 1;
      // Operations
      public :
        /**
         * \brief Constructeur d'une classe CPonderation.
         * \param action (in) L'action à manipuler
         * \param ponderation (in) La pondération à appliquer.
         * \param psi (in) Le choix du coefficient psi à appliquer.
         */
        CPonderation (POCO::sol::CAction * action, double ponderation, int8_t psi);
        /**
         * \brief Duplication d'une classe CPonderation.
         * \param other (in) La classe à dupliquer.
         */
        CPonderation (const CPonderation & other) = delete;
        /**
         * \brief Assignment operator de CPonderation.
         * \param other (in) La classe à dupliquer.
         * \return CPonderation &
         */
        CPonderation & operator = (const CPonderation & other) = delete;
        /**
         * \brief Destructeur d'une classe CPonderation.
         */
        virtual ~CPonderation ();
        /**
         * \brief Renvoie l'action de la pondération.
         * \return POCO::sol::CAction const &
         */
        POCO::sol::CAction const & getAction () const;
        /**
         * \brief Défini l'action de la pondération.
         * \param action_ (in) La nouvelle action.
         * \return bool CHK
         */
        bool CHK setAction (POCO::sol::CAction * action_);
        /**
         * \brief Renvoie la pondération.
         * \return double
         */
        double getPonderation () const;
        /**
         * \brief Défini la pondération.
         * \param pond (in) La nouvelle pondération.
         * \return bool CHK
         */
        bool CHK setPonderation (double pond);
        /**
         * \brief Renvoie le choix du coefficient psi.
         * \return uint8_t
         */
        uint8_t getpsi () const;
        /**
         * \brief Renvoie le coefficient psi.
         * \return double
         */
        double getpsif () const;
        /**
         * \brief Défini le choix du coefficient psi.
         * \param psi_ (in) Le nouveau coefficient psi.
         * \return bool CHK
         */
        bool CHK setpsi (uint8_t psi_);
    };
  }
}

#endif
