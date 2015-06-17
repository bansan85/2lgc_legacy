#ifndef NDBARRE__HPP
#define NDBARRE__HPP

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

#include <INb.hpp>
#include <CBarre.hpp>
#include "INoeud.hpp"

/**
 * \brief Nœud intermédiaire dont la position est le long d'une barre.
 */
class NdBarre : public INoeud
{
  // Attributes
  private :
    /// La barre qui contiendra le nœud.
    CBarre * barre;
    /// La position le long de la barre (entre 0. et 1.).
    INb * pos_rel;
  // Operations
  public :
    /**
     * \brief Constructeur d'une classe NdBarre.
     * \param barre (in) La barre qui contiendra le nœud.
     * \param pos (in) La position relative sur la barre.
     * \param undo (in) Le gestionnaire des modifications.
     */
    NdBarre (CBarre * barre, INb * pos, UndoManager & undo);
    /**
     * \brief Duplication d'une classe NdBarre.
     * \param other (in) La classe à dupliquer.
     */
    NdBarre (const NdBarre & other) = delete;
    /**
     * \brief Assignment operator de NdBarre.
     * \param other (in) La classe à dupliquer.
     * \return NdBarre &
     */
    NdBarre & operator = (const NdBarre & other) = delete;
    /**
     * \brief Destructeur d'une classe NdBarre.
     */
    virtual ~NdBarre ();
    /**
     * \brief Renvoie la barre où repose le nœud.
     * \return CBarre const &
     */
    CBarre const & getBarre () const;
    /**
     * \brief Défini la barre où repose le nœud.
     * \param val (in) La nouvelle barre.
     * \return bool CHK
     */
    bool CHK setBarre (CBarre * val);
    /**
     * \brief Renvoie la position du nœud sur la barre.
     * \return INb const &
     */
    INb const & getPosRel () const;
    /**
     * \brief Défini la position du nœud sur la barre (entre 0 et 1).
     * \param val (in) La nouvelle valeur.
     * \return bool CHK
     */
    bool CHK setPosRel (INb * val);
    /**
     * \brief Renvoie la coordonnée en x.
     * \return double
     */
    virtual double getx ();
    /**
     * \brief Renvoie la coordonnée en y.
     * \return double
     */
    virtual double gety ();
    /**
     * \brief Renvoie la coordonnée en z.
     * \return double
     */
    virtual double getz ();
};

#endif
