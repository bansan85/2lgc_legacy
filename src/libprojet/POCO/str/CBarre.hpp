#ifndef POCO_STR_CBARRE__HPP
#define POCO_STR_CBARRE__HPP

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
#include "POCO/str/INoeud.hpp"
#include "POCO/str/IMateriau.hpp"
#include "POCO/str/CRelachement.hpp"
#include "POCO/str/ISection.hpp"

namespace POCO
{
  namespace str
  {
    /**
     * \brief Données d'une barre.
     */
    class CBarre
    {
      // Attributes
      private :
        /// Numéro de la barre. Est attribué automatiquement. Le numéro est attribué automatiquement comme celui du numéro de la dernière barre + 1.
        uint32_t numero;
        /// La section de la barre.
        POCO::str::ISection * section;
        /// Le matériau de la barre.
        POCO::str::IMateriau * materiau;
        /// Le nœud définissant le début de la barre.
        POCO::str::INoeud * noeud_debut;
        /// Le nœud définissant le fin de la barre.
        POCO::str::INoeud * noeud_fin;
        /// Angle de rotation de la barre autour de l'axe x local.
        POCO::INb * angle;
        /// Relâchement de la barre. NULL si aucun.
        CRelachement * relachement;
        /// Nœuds intermédiaires le long de la barre. Lorsque des nœuds intermédiaires sont ajoutés, ils le sont dans p->modele.noeuds. La liste de nœuds nds_inter contient uniquement un pointeur vers chaque nœud de la liste principale.
        std::list <POCO::str::INoeud *> nds_inter;
      // Operations
      public :
        /**
         * \brief Constructeur d'une classe CBarre.
         * \param section (in) La section de la barre.
         * \param materiau (in) Le matériau de la barre.
         * \param noeudd (in) Le nœud définissant le début de la barre.
         * \param noeudf (in) Le nœud définissant la fin de la barre.
         * \param angle (in) Angle de rotation de la barre autour de l'axe x local.
         * \param relachement (in) Relâchement de la barre. NULL si aucun.
         * \param discretisation (in) Nombre de découpage de la barre.
         */
        CBarre (POCO::str::ISection * section, POCO::str::IMateriau * materiau, POCO::str::INoeud * noeudd, POCO::str::INoeud * noeudf, POCO::INb * angle, CRelachement * relachement, uint16_t discretisation);
        /**
         * \brief Duplication d'une classe CBarre.
         * \param other (in) La classe à dupliquer.
         */
        CBarre (const CBarre & other) = delete;
        /**
         * \brief Assignment operator de CBarre.
         * \param other (in) La classe à dupliquer.
         * \return CBarre &
         */
        CBarre & operator = (const CBarre & other) = delete;
        /**
         * \brief Destructeur d'une classe CBarre.
         */
        virtual ~CBarre ();
        /**
         * \brief Renvoie la section de la barre.
         * \return POCO::str::ISection const &
         */
        POCO::str::ISection const & getSection () const;
        /**
         * \brief Défini la section de la barre.
         * \param section_ (in) La nouvelle section.
         * \return bool CHK
         */
        bool CHK setSection (POCO::str::ISection * section_);
        /**
         * \brief Renvoie le matériau de la barre.
         * \return POCO::str::IMateriau const &
         */
        POCO::str::IMateriau const & getMateriau () const;
        /**
         * \brief Défini le matériau de la barre.
         * \param mat (in) Le nouveau matériau.
         * \return bool CHK
         */
        bool CHK setMateriau (POCO::str::IMateriau * mat);
        /**
         * \brief Renvoie le nœud de début de la barre.
         * \return POCO::str::INoeud const &
         */
        POCO::str::INoeud const & getNoeudDebut () const;
        /**
         * \brief Défini le nœud de début de la barre.
         * \param noeud (in) Le nouveau nœud.
         * \return bool CHK
         */
        bool CHK setNoeudDebut (POCO::str::INoeud * noeud);
        /**
         * \brief Renvoie le nœud de fin de la barre.
         * \return POCO::str::INoeud const &
         */
        POCO::str::INoeud const & getNoeudFin () const;
        /**
         * \brief Défini le nœud de fin de la barre.
         * \param noeud (in) Le nouveau nœud.
         * \return bool CHK
         */
        bool CHK setNoeudFin (POCO::str::INoeud * noeud);
        /**
         * \brief Renvoie l'angle de rotation de la barre.
         * \return POCO::INb const &
         */
        POCO::INb const & getAngle () const;
        /**
         * \brief Défini l'angle de rotation de la barre.
         * \param angle_ (in) Le nouvel angle.
         * \return bool CHK
         */
        bool CHK setAngle (POCO::INb * angle_);
        /**
         * \brief Renvoie le relâchement de la barre.
         * \return CRelachement const &
         */
        CRelachement const & getRelachement () const;
        /**
         * \brief Défini le relâchement de la barre.
         * \param relachement_ (in) Le nouveau relâchement.
         * \return bool CHK
         */
        bool CHK setRelachement (CRelachement * relachement_);
        /**
         * \brief Ajoute plusieurs nœuds à la charge.
         * \param noeuds (in) La liste de nœuds à ajouter.
         * \return bool CHK
         */
        bool CHK addNoeud (std::list <POCO::str::INoeud *> * noeuds);
        /**
         * \brief Enlève plusieurs nœuds à la charge.
         * \param noeuds (in) La liste de nœuds à enlever.
         * \return bool CHK
         */
        bool CHK rmNoeud (std::list <POCO::str::INoeud *> * noeuds);
    };
  }
}

#endif
