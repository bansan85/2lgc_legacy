#ifndef POCO_NOMBRE_UTILISATEUR__HPP
#define POCO_NOMBRE_UTILISATEUR__HPP

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

#include "POCO/EUnite.hpp"
#include "POCO/INb.hpp"

namespace POCO
{
  namespace nombre
  {
    /**
     * \brief Nombre défini par l'utilisateur (et donc dont la précision des décimales est voulue). L'intérêt de cette classe est de pouvoir stocker un nombre flottant et que l'affichage via toString ne respecte pas les décimales par défaut.
     */
    class Utilisateur : public POCO::INb
    {
      // Attributes
      private :
        /// Valeur du nombre.
        double val;
        /// L'unité du nombre.
        POCO::EUnite unite;
      // Operations
      public :
        /**
         * \brief Constructeur d'une classe Utilisateur.
         * \param valeur (in) La valeur initiale.
         * \param unit (in) L'unité du nombre.
         */
        Utilisateur (double valeur, EUnite unit);
        /**
         * \brief Constructeur d'une classe Utilisateur.
         * \param other (in) Le nombre à dupliquer.
         */
        Utilisateur (const Utilisateur & other);
        /**
         * \brief Constructeur d'une classe Utilisateur.
         * \param other (in) Le nombre à dupliquer.
         * \return Utilisateur &
         */
        Utilisateur & operator = (const Utilisateur & other) = delete;
        /**
         * \brief Destructeur d'une classe Utilisateur.
         */
        virtual ~Utilisateur ();
        /**
         * \brief Renvoie la valeur du nombre.
         * \return double
         */
        virtual double getVal () const;
        /**
         * \brief Modifie la valeur du nombre.
         * \param valeur (in) La nouvelle valeur.
         * \return bool CHK
         */
        bool CHK setVal (double valeur);
        /**
         * \brief Renvoie l'unité du nombre.
         * \return EUnite
         */
        virtual EUnite getUnite () const;
        /**
         * \brief Modifie l'unité du nombre.
         * \param unite_ (in) La nouvelle unité.
         * \return bool CHK
         */
        bool CHK setUnite (EUnite unite_);
        /**
         * \brief Renvoie le nombre sous forme de texte sans respecter le nombre de décimales.
         * \return std::string
         */
        virtual std::string toString () const;
        /**
         * \brief Converti la fonction de création d'un nombre sous format XML.
         * \param root (in) Le noeud dans lequel doit être inséré le nombre.
         * \return bool CHK
         */
        virtual bool CHK newXML (xmlNodePtr root) const;
    };
  }
}

#endif
