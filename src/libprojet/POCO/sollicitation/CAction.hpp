#ifndef POCO_SOLLICITATION_CACTION__HPP
#define POCO_SOLLICITATION_CACTION__HPP

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

class CPonderation;
class CPonderations;
class CCalculs;
class CModele;
class IUndoFunc;
class CActionSetPsi;

#include <cholmod.h>
#include <vector>
#include <array>
#include "POCO/sollicitation/action/CActionSetPsi.hpp"
#include "POCO/sollicitation/action/IActionGroupe.hpp"
#include "POCO/calcul/CFonction.hpp"
#include "POCO/sollicitation/ICharge.hpp"

namespace POCO
{
  namespace sollicitation
  {
    /**
     * \brief Objet définissant une action avec sa liste de charges.
     */
    class DllExport CAction : public POCO::sollicitation::action::CActionSetPsi, public POCO::sollicitation::action::IActionGroupe
    {
      // Associations
      // Attributes
      private :
        /// La description de type est donnée par IParametres::getpsiDescription.
        uint8_t type;
        /// Flag utilisé temporairement lors des calculs.
        bool action_predominante;
        /// Liste des charges (ponctuelle, répartie, …).
        std::list <std::shared_ptr <POCO::sollicitation::ICharge> > charges;
        /// Valeur de combinaison d'une charge variable.
        std::shared_ptr <POCO::INb> psi0;
        /// Valeur fréquente d'une action variable.
        std::shared_ptr <POCO::INb> psi1;
        /// Valeur quasi-permanente d'une action variable.
        std::shared_ptr <POCO::INb> psi2;
        /// Déplacement des noeuds de la structure sous la sollicitation de l'action.
        cholmod_sparse * deplacement;
        /// Efforts équivalents des charges dans les nœuds de la structure.
        cholmod_sparse * forces;
        /// Efforts (y compris les réactions d'appui) dans les nœuds.
        cholmod_sparse * efforts_noeuds;
        /// Les 6 fonctions (N, Ty, Tz, Mx, My, Mz) par barre.
        std::array <std::vector <POCO::calcul::CFonction *>, 6> efforts;
        /// Les 3 déformations x, y, z pour chaque barre dans le repère local.
        std::array <std::vector <POCO::calcul::CFonction *>, 3> deformation;
        /// Les 3 rotations rx, ry, rz pour chaque barre dans le repère local.
        std::array <std::vector <POCO::calcul::CFonction *>, 3> rotation;
      // Operations
      public :
        /**
         * \brief Constructeur d'une classe CAction. Par défaut, les coefficients psi sont défini à NAN. Les valeurs sont automatiquement déterminés lors de l'insertion dans le projet en fonction de la norme du projet.
         * \param nom_ (in) Le nom de l'action.
         * \param type_ (in) Le type d'action, cf. _1990_action_bat_txt_type.
         */
        CAction (std::shared_ptr <std::string> nom_, uint8_t type_);
        /**
         * \brief Duplication d'une classe CAction.
         * \param other (in) La classe à dupliquer.
         */
        CAction (const CAction & other) = delete;
        /**
         * \brief Duplication d'une classe CAction.
         * \param other (in) La classe à dupliquer.
         * \return CAction &
         */
        CAction & operator = (const CAction & other) = delete;
        /**
         * \brief Destructeur d'une classe CAction.
         */
        virtual ~CAction ();
        /**
         * \brief Renvoie le type de l'action.
         * \return uint8_t
         */
        uint8_t getType () const;
        /**
         * \brief Défini le type de l'action.
         * \param type_ (in) Le nouveau type.
         * \return bool CHK
         */
        bool CHK setType (uint8_t type_);
        /**
         * \brief Renvoie le cœfficient psi.
         * \param psi (in) Le numéro du coefficient à renvoyer (0, 1 ou 2).
         * \return POCO::INb const *
         */
        POCO::INb const * getPsi (uint8_t psi) const;
        /**
         * \brief Renvoie true si aucune charge n'est présente.
         * \return bool CHK
         */
        bool CHK emptyCharges () const;
        /**
         * \brief Ajoute plusieurs charges à l'action.
         * \param charges_ (in) La liste de charges à ajouter.
         * \return bool CHK
         */
        bool CHK addCharges (std::list <std::shared_ptr <POCO::sollicitation::ICharge> > & charges_);
        /**
         * \brief Enlève plusieurs charges à l'action.
         * \param charges_ (in) La liste de charges à enlever.
         * \return bool CHK
         */
        bool CHK rmCharges (std::list <POCO::sollicitation::ICharge *> * charges_);
        /**
         * \brief Renvoie le type de l'action sous forme de texte.
         * \param type_ (in) Le type de l'action à décrire.
         * \return std::string const
         */
        std::string const getDescription (uint8_t type_) const;
    };
  }
}

#endif
