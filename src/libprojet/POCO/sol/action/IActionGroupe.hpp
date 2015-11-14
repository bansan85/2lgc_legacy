#ifndef POCO_SOL_ACTION_IACTIONGROUPE__HPP
#define POCO_SOL_ACTION_IACTIONGROUPE__HPP

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

#include <memory>

namespace POCO
{
  namespace sol
  {
    namespace action
    {
      /**
       * \brief Un groupe possède soit une liste d'actions ou un liste de groupes. Action et Groupe d'actions possèdent le même point commun : un nom et un iter pour l'affichage graphique dans la fenêtre permettant de réaliser des combinaisons de Groupe et d'Action.
       */
      class IActionGroupe
      {
        // Attributes
        private :
          /// Nom du groupe ou de l'action.
          std::shared_ptr <const std::string> nom;
        // Operations
        public :
          /**
           * \brief Constructeur d'une interface IActionGroupe.
           * \param nom_ (in) Le nom de l'action / groupe.
           */
          IActionGroupe (std::shared_ptr <std::string> nom_);
          /**
           * \brief Duplication d'une classe IActionGroupe.
           * \param other (in) La classe à dupliquer.
           */
          IActionGroupe (const IActionGroupe & other) = delete;
          /**
           * \brief Duplication d'une classe IActionGroupe.
           * \param other (in) La classe à dupliquer.
           * \return IActionGroupe &
           */
          IActionGroupe & operator = (const IActionGroupe & other) = delete;
          /**
           * \brief Destructeur d'une interface IActionGroupe.
           */
          virtual ~IActionGroupe ();
          /**
           * \brief Renvoie le nom.
           * \return std::shared_ptr <const std::string>
           */
          std::shared_ptr <const std::string> getNom () const;
          /**
           * \brief Défini le nom.
           * \param nom_ (in) Le nouveau nom.
           * \return bool CHK
           */
          bool CHK setNom (std::shared_ptr <std::string> nom_);
      };
    }
  }
}

#endif
