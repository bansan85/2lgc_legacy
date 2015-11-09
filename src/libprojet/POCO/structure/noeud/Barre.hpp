#ifndef POCO_STRUCTURE_NOEUD_BARRE__HPP
#define POCO_STRUCTURE_NOEUD_BARRE__HPP

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

#include "POCO/structure/INoeud.hpp"
#include "POCO/structure/CBarre.hpp"

namespace POCO
{
  namespace structure
  {
    namespace noeud
    {
      /**
       * \brief Nœud intermédiaire dont la position est le long d'une barre.
       */
      class Barre : public POCO::structure::INoeud
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
           * \brief Constructeur d'une classe Barre.
           * \param barre (in) La barre qui contiendra le nœud.
           * \param pos (in) La position relative sur la barre.
           * \param undo (in) Le gestionnaire des modifications.
           */
          Barre (CBarre * barre, INb * pos, UndoManager & undo);
          /**
           * \brief Duplication d'une classe Barre.
           * \param other (in) La classe à dupliquer.
           */
          Barre (const Barre & other) = delete;
          /**
           * \brief Assignment operator de Barre.
           * \param other (in) La classe à dupliquer.
           * \return Barre &
           */
          Barre & operator = (const Barre & other) = delete;
          /**
           * \brief Destructeur d'une classe Barre.
           */
          virtual ~Barre ();
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
    }
  }
}

#endif
