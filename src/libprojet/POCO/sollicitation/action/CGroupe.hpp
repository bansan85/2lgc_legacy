#ifndef POCO_SOLLICITATION_ACTION_CGROUPE__HPP
#define POCO_SOLLICITATION_ACTION_CGROUPE__HPP

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

#include "POCO/sollicitation/action/ECombinaison.hpp"
#include "POCO/sollicitation/action/IActionGroupe.hpp"
#include "POCO/sollicitation/CCombinaison.hpp"

namespace POCO
{
  namespace sollicitation
  {
    namespace action
    {
      /**
       * \brief Contient une liste d'actions ou de groupes avec la méthode pour les combiner (AND, OR ou XOR).
       */
      class CGroupe : public POCO::sollicitation::action::IActionGroupe
      {
        // Associations
        // Attributes
        private :
          /// Le type de combinaisons des éléments.
          POCO::sollicitation::action::ECombinaison combinaison;
          /// Contient des Action si le groupe est au niveau 0, des CGroupe sinon.
          std::list <IActionGroupe *> elements;
          /// Combinaisons temporaires. Ces combinaisons sont utilisées pour générer les groupes du niveau supérieur jusqu'au dernier.
          std::list <std::list <CCombinaison *> *> tmpCombinaison;
          /// Pour préserver l'affichage graphique lors d'un changement de niveau. Cela permet de conserver les groupes développés.
          bool Iter_expand;
        // Operations
        public :
          /**
           * \brief Constructeur d'une classe CGroupe.
           * \param type (in) Le type de combinaison.
           * \param elements (???) Les Action ou CGroupe.
           * \param undo (in) Le gestionnaire des modifications.
           */
          CGroupe (ECombinaison type, std::list <IActionGroupe *> * elements, UndoManager & undo);
          /**
           * \brief Duplication d'une classe CGroupe.
           * \param other (in) La classe à dupliquer.
           */
          CGroupe (const CGroupe & other) = delete;
          /**
           * \brief Duplication d'une classe CGroupe.
           * \param other (in) La classe à dupliquer.
           * \return CGroupe &
           */
          CGroupe & operator = (const CGroupe & other) = delete;
          /**
           * \brief Destructeur d'une classe CGroupe.
           */
          virtual ~CGroupe ();
          /**
           * \brief Renvoie le type de combinaisons.
           * \return ECombinaison
           */
          ECombinaison getCombinaison () const;
          /**
           * \brief Défini le type de combinaisons.
           * \param comb (in) Le type de combinaisons.
           * \return bool CHK
           */
          bool CHK setCombinaison (ECombinaison comb);
          /**
           * \brief Ajoute plusieurs groupes/actions au groupe.
           * \param liste (in) La liste de actions/groupes à ajouter.
           * \return bool CHK
           */
          bool CHK addElements (std::list <IActionGroupe *> * liste);
          /**
           * \brief Enlève plusieurs groupes/actions au groupe.
           * \param liste (in) La liste de actions/groupes à enlever.
           * \return bool CHK
           */
          bool CHK rmElements (std::list <IActionGroupe *> * liste);
      };
    }
  }
}

#endif
