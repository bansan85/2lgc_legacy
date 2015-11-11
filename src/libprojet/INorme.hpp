#ifndef INORME__HPP
#define INORME__HPP

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
#include <cstdint>
#include "EAction.hpp"

/**
 * \brief Contient les options de calculs du projet, ceux dépendant de la norme sélectionnée.
 */
class DllExport INorme
{
  // Operations
  public :
    /**
     * \brief Constructeur d'une classe INorme.
     */
    INorme ();
    /**
     * \brief Constructeur d'une classe INorme.
     * \param other (in) La classe à dupliquer.
     */
    INorme (const INorme & other) = delete;
    /**
     * \brief Constructeur d'une classe INorme.
     * \param other (in) La classe à dupliquer.
     * \return INorme &
     */
    INorme & operator = (const INorme & other) = delete;
    /**
     * \brief Destructeur d'une classe INorme.
     */
    virtual ~INorme ();
    /**
     * \brief Renvoie le nom des paramètres de calcul.
     * \return const std::shared_ptr <std::string> &
     */
    virtual const std::shared_ptr <std::string> & getNom () const = 0;
    /**
     * \brief Renvoie la variante de la norme.
     * \return uint32_t
     */
    virtual uint32_t getVariante () const = 0;
    /**
     * \brief Renvoie le nombre de différents types d'actions.
     * \return uint8_t
     */
    virtual uint8_t getPsiN () const = 0;
    /**
     * \brief Renvoie la catégorie du type d'action.
     * \param type (in) Le type d'action.
     * \return EAction
     */
    virtual EAction getPsiAction (uint8_t type) const = 0;
    /**
     * \brief Renvoie la description du type d'actions.
     * \param type (in) Le type d'action à décrire.
     * \return std::string const
     */
    virtual std::string const getPsiDescription (uint8_t type) const = 0;
    /**
     * \brief Renvoie la valeur par défaut du cœfficient psi0.
     * \param type (in) Le type de l'action.
     * \return double
     */
    virtual double getPsi0 (uint8_t type) const = 0;
    /**
     * \brief Renvoie la valeur par défaut du cœfficient psi1.
     * \param type (in) Le type de l'action.
     * \return double
     */
    virtual double getPsi1 (uint8_t type) const = 0;
    /**
     * \brief Renvoie la valeur par défaut du cœfficient psi2.
     * \param type (in) Le type de l'action.
     * \return double
     */
    virtual double getPsi2 (uint8_t type) const = 0;
};

#endif
