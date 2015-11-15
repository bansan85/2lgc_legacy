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

class FuncModeleNorme;

#include <memory>
#include <cstdint>
#include "ENorme.hpp"
#include "EAction.hpp"

/**
 * \brief Contient les options de calculs du projet, ceux dépendant de la norme du projet.
 */
class DllExport INorme
{
  // Attributes
  private :
    friend class FuncModeleNorme;
    /// L'identifiant de la norme.
    uint32_t id;
  protected :
    /// La norme correspondante.
    ENorme type;
    /// Permet de sélectionner différentes variantes de la norme. Par exemple, l'annexe nationale dans le cas de l'Eurocode.
    uint32_t variante;
    /// Les options de la norme. Espace réservée pour chaque spécificité des normes.
    uint32_t options;
    /// Nom de la norme.
    std::shared_ptr <const std::string> nom;
  // Operations
  public :
    /**
     * \brief Constructeur d'une classe INorme.
     * \param type_ (in) La norme correspondante.
     * \param variante_ (in) La variante de la norme.
     * \param options_ (in) Les options de la norme.
     * \param nom_ (in) Le nom de la norme.
     */
    INorme (ENorme type_, uint32_t variante_, uint32_t options_, std::shared_ptr <const std::string> nom_);
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
     * \brief Renvoie le type de la norme.
     * \return ENorme
     */
    ENorme getType () const;
    /**
     * \brief Renvoie la variante de la norme.
     * \return uint32_t
     */
    uint32_t getVariante () const;
    /**
     * \brief Renvoie les options de la norme.
     * \return uint32_t
     */
    uint32_t getOptions () const;
    /**
     * \brief Renvoie le nom de la norme de calcul.
     * \return const std::shared_ptr <const std::string> &
     */
    const std::shared_ptr <const std::string> & getNom () const;
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
