#ifndef POCO_INB__HPP
#define POCO_INB__HPP

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

#include <libxml/tree.h>
#include <string>
#include <array>
#include "EUnite.hpp"

namespace POCO
{
  /**
   * \brief Défini un nombre flottant de type double en fonction de différents types de données initiales.
   */
  class INb
  {
    // Operations
    public :
      /**
       * \brief Constructeur d'une interface INb.
       */
      INb ();
      /**
       * \brief Constructeur d'une classe NbCalcul.
       * \param other (in) Le nombre à dupliquer.
       */
      INb (const INb & other) = delete;
      /**
       * \brief Constructeur d'une classe INb.
       * \param other (in) Le nombre à dupliquer.
       * \return INb &
       */
      INb & operator = (const INb & other) = delete;
      /**
       * \brief Destructeur d'une classe Nombre.
       */
      virtual ~INb ();
      /**
       * \brief Constructeur d'une classe INb.
       * \param nb (in) Le nombre à dupliquer.
       * \return INb *
       */
      static INb * newINb (INb * nb);
      /**
       * \brief Renvoie la valeur du nombre.
       * \return double
       */
      virtual double getVal () const = 0;
      /**
       * \brief Renvoie l'unité du nombre.
       * \return EUnite
       */
      virtual EUnite getUnite () const = 0;
      /**
       * \brief Renvoie le nombre sous forme de texte.
       * \param decimales (in) Le nombre de décimales à afficher.
       * \return std::string
       */
      virtual std::string toString (std::array <uint8_t, static_cast <size_t> (EUnite::LAST)> & decimales) const = 0;
      /**
       * \brief Converti la fonction de création d'un nombre sous format XML.
       * \param root (in) Le noeud dans lequel doit être inséré le nombre.
       * \return bool CHK
       */
      virtual bool CHK newXML (xmlNodePtr root) const = 0;
  };
}

#endif
