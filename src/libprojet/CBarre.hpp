#ifndef CBARRE__HPP
#define CBARRE__HPP

/*
2lgc_code : calcul de résistance des matériaux selon les normes Eurocodes
Copyright (C) 2011

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

#include <ISection.hpp>
#include <IMateriau.hpp>
#include <INoeud.hpp>
#include <list>
#include "CRelachement.hpp"

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
    ISection * section;
    /// Le matériau de la barre.
    IMateriau * materiau;
    /// Le nœud définissant le début de la barre.
    INoeud * noeud_debut;
    /// Le nœud définissant le fin de la barre.
    INoeud * noeud_fin;
    /// Angle de rotation de la barre autour de l'axe x local.
    INb * angle;
    /// Relâchement de la barre. NULL si aucun.
    CRelachement * relachement;
    /// Nœuds intermédiaires le long de la barre. Lorsque des nœuds intermédiaires sont ajoutés, ils le sont dans p->modele.noeuds. La liste de nœuds nds_inter contient uniquement un pointeur vers chaque nœud de la liste principale.
    std::list <INoeud *> nds_inter;
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
    CBarre (ISection * section, IMateriau * materiau, INoeud * noeudd, INoeud * noeudf, INb * angle, CRelachement * relachement, uint16_t discretisation);
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
     * \return ISection const &
     */
    ISection const & getSection () const;
    /**
     * \brief Défini la section de la barre.
     * \param section_ (in) La nouvelle section.
     * \return bool CHK
     */
    bool CHK setSection (ISection * section_);
    /**
     * \brief Renvoie le matériau de la barre.
     * \return IMateriau const &
     */
    IMateriau const & getMateriau () const;
    /**
     * \brief Défini le matériau de la barre.
     * \param mat (in) Le nouveau matériau.
     * \return bool CHK
     */
    bool CHK setMateriau (IMateriau * mat);
    /**
     * \brief Renvoie le nœud de début de la barre.
     * \return INoeud const &
     */
    INoeud const & getNoeudDebut () const;
    /**
     * \brief Défini le nœud de début de la barre.
     * \param noeud (in) Le nouveau nœud.
     * \return bool CHK
     */
    bool CHK setNoeudDebut (INoeud * noeud);
    /**
     * \brief Renvoie le nœud de fin de la barre.
     * \return INoeud const &
     */
    INoeud const & getNoeudFin () const;
    /**
     * \brief Défini le nœud de fin de la barre.
     * \param noeud (in) Le nouveau nœud.
     * \return bool CHK
     */
    bool CHK setNoeudFin (INoeud * noeud);
    /**
     * \brief Renvoie l'angle de rotation de la barre.
     * \return INb const &
     */
    INb const & getAngle () const;
    /**
     * \brief Défini l'angle de rotation de la barre.
     * \param angle_ (in) Le nouvel angle.
     * \return bool CHK
     */
    bool CHK setAngle (INb * angle_);
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
    bool CHK addNoeud (std::list <INoeud *> * noeuds);
    /**
     * \brief Enlève plusieurs nœuds à la charge.
     * \param noeuds (in) La liste de nœuds à enlever.
     * \return bool CHK
     */
    bool CHK rmNoeud (std::list <INoeud *> * noeuds);
};

#endif
