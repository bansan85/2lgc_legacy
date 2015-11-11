#ifndef POCO_STR_NOEUD_LIBRE__HPP
#define POCO_STR_NOEUD_LIBRE__HPP

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

#include "POCO/str/INoeud.hpp"

namespace POCO
{
  namespace str
  {
    namespace noeud
    {
      /**
       * \brief Nœud dont la position est dans le repère XYZ. Sa position peut-être relative avec un autre nœud.
       */
      class Libre : public POCO::str::INoeud
      {
        // Attributes
        private :
          /// Le nœud relatif vaut NULL si la position est absolue.
          INoeud * relatif;
          /// Coordonnée en x.
          INb * x;
          /// Coordonnée en y.
          INb * y;
          /// Coordonnée en z.
          INb * z;
        // Operations
        public :
          /**
           * \brief Constructeur d'une classe Libre.
           * \param x (in) La coordonnée en x.
           * \param y (in) La coordonnée en y.
           * \param z (in) La coordonnée en z.
           * \param nd (in) Le nœud relatif, NULL si aucun.
           * \param undo (in) Le gestionnaire des modifications.
           */
          Libre (INb * x, INb * y, INb * z, INoeud * nd, UndoManager & undo);
          /**
           * \brief Duplication d'une classe Libre.
           * \param other (in) La classe à dupliquer.
           */
          Libre (const Libre & other) = delete;
          /**
           * \brief Assignment operator de Libre.
           * \param other (in) La classe à dupliquer.
           * \return Libre &
           */
          Libre & operator = (const Libre & other) = delete;
          /**
           * \brief Destructeur d'une classe Libre.
           */
          virtual ~Libre ();
          /**
           * \brief Renvoie la coordonnée en x.
           * \return INb const &
           */
          INb const & getxN () const;
          /**
           * \brief Renvoie la coordonnée en x.
           * \return double
           */
          virtual double getx ();
          /**
           * \brief Défini la coordonnée en x.
           * \param val (in) La nouvelle valeur.
           * \return bool CHK
           */
          bool CHK setx (INb * val);
          /**
           * \brief Renvoie la coordonnée en y.
           * \return INb const &
           */
          INb const & getyN () const;
          /**
           * \brief Renvoie la coordonnée en y.
           * \return double
           */
          virtual double gety ();
          /**
           * \brief Défini la coordonnée en y.
           * \param val (in) La nouvelle valeur.
           * \return bool CHK
           */
          bool CHK sety (INb * val);
          /**
           * \brief Renvoie la coordonnée en z.
           * \return INb const &
           */
          INb const & getzN () const;
          /**
           * \brief Renvoie la coordonnée en z.
           * \return double
           */
          virtual double getz ();
          /**
           * \brief Défini la coordonnée en z.
           * \param val (in) La nouvelle valeur.
           * \return bool CHK
           */
          bool CHK setz (INb * val);
          /**
           * \brief Défini les coordonnées du nœud.
           * \param x (in) La nouvelle coordonnée en x.
           * \param y (in) La nouvelle coordonnée en y.
           * \param z (in) La nouvelle coordonnée en z.
           * \return bool CHK
           */
          bool CHK setxyz (INb * x, INb * y, INb * z);
          /**
           * \brief Renvoie le nœud relatif.
           * \return INoeud const &
           */
          INoeud const & getRelatif () const;
          /**
           * \brief Défini le nœud relatif.
           * \param val (in) Le nouveau nœud.
           * \return bool CHK
           */
          bool CHK setRelatif (INoeud * val);
      };
    }
  }
}

#endif
