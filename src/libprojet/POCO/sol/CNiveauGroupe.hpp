#ifndef POCO_SOL_CNIVEAUGROUPE__HPP
#define POCO_SOL_CNIVEAUGROUPE__HPP

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

#include <list>
#include "POCO/sol/action/IActionGroupe.hpp"

namespace POCO
{
  namespace sol
  {
    /**
     * \brief Contient une liste de groupe. Si le niveau vaut 0, le niveau contiendra une liste d'actions. Si le niveau est supérieur à 0, le niveau contiendra une liste de groupes du niveau inférieur.
     */
    class CNiveauGroupe
    {
      // Associations
      // Attributes
      private :
        /// La liste.
        std::list <POCO::sol::action::IActionGroupe *> groupes;
      // Operations
      public :
        /**
         * \brief Constructeur d'une classe CNiveauGroupe.
         * \param liste (in) La liste des groupes ou des actions.
         */
        CNiveauGroupe (std::list <POCO::sol::action::IActionGroupe *> * liste);
        /**
         * \brief Duplication d'une classe CNiveauGroupe.
         * \param other (in) La classe à dupliquer.
         */
        CNiveauGroupe (const CNiveauGroupe & other) = delete;
        /**
         * \brief Duplication d'une classe CNiveauGroupe.
         * \param other (in) La classe à dupliquer.
         * \return CNiveauGroupe &
         */
        CNiveauGroupe & operator = (const CNiveauGroupe & other) = delete;
        /**
         * \brief Destructeur d'une classe CNiveauGroupe.
         */
        virtual ~CNiveauGroupe ();
        /**
         * \brief Ajoute plusieurs groupes au niveau.
         * \param liste  (in) La liste des groupes à ajouter.
         * \return bool CHK
         */
        bool CHK addGroupes (std::list <POCO::sol::action::IActionGroupe *> * liste );
        /**
         * \brief Enlève plusieurs groupes au niveau.
         * \param liste (in) La liste des groupes à enlever.
         * \return bool CHK
         */
        bool CHK rmGroupes (std::list <POCO::sol::action::IActionGroupe *> * liste);
    };
  }
}

#endif
