#ifndef IPARAMETRES__HPP
#define IPARAMETRES__HPP

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

#include <IUndoable.hpp>
#include "EAction.hpp"

/**
 * \brief Contient les options de calculs du projet.
 */
class DllExport IParametres : public IUndoable
{
  // Operations
  public :
    /**
     * \brief Constructeur d'une classe IParametres.
     * \param undo_ (in) Le gestionnaire des modifications.
     */
    IParametres (UndoManager & undo_);
    /**
     * \brief Constructeur d'une classe IParametres.
     * \param other (in) La classe à dupliquer.
     */
    IParametres (const IParametres & other) = delete;
    /**
     * \brief Constructeur d'une classe IParametres.
     * \param other (in) La classe à dupliquer.
     * \return IParametres &
     */
    IParametres & operator = (const IParametres & other) = delete;
    /**
     * \brief Destructeur d'une classe IParametres.
     */
    virtual ~IParametres ();
    /**
     * \brief Renvoie le nom des paramètres de calcul.
     * \return const std::shared_ptr <std::string> &
     */
    virtual const std::shared_ptr <std::string> & getNom () const = 0;
    /**
     * \brief Définit le nom des paramètres de calcul.
     * \param nom_ (in) Le nouveau nom.
     * \return bool CHK
     */
    virtual bool CHK setNom (std::shared_ptr <std::string> nom_) = 0;
    /**
     * \brief Renvoie la variante de la norme.
     * \return uint32_t
     */
    virtual uint32_t getVariante () const = 0;
    /**
     * \brief Défini la variante de la norme.
     * \param variante_ (in) La nouvelle variante.
     * \return bool CHK
     */
    virtual bool CHK setVariante (uint32_t variante_) = 0;
    /**
     * \brief Renvoie le nombre de différents types d'actions.
     * \return uint8_t
     */
    virtual uint8_t getpsiN () const = 0;
    /**
     * \brief Renvoie la catégorie du type d'action.
     * \param type (in) Le type d'action.
     * \return EAction
     */
    virtual EAction getpsiAction (uint8_t type) const = 0;
    /**
     * \brief Renvoie la description du type d'actions.
     * \param type (in) Le type d'action à décrire.
     * \return std::string const
     */
    virtual std::string const getpsiDescription (uint8_t type) const = 0;
    /**
     * \brief Renvoie la valeur par défaut du coefficient psi0.
     * \param type (in) Le type de l'action.
     * \return double
     */
    virtual double getpsi0 (uint8_t type) const = 0;
    /**
     * \brief Renvoie la valeur par défaut du coefficient psi1.
     * \param type (in) Le type de l'action.
     * \return double
     */
    virtual double getpsi1 (uint8_t type) const = 0;
    /**
     * \brief Renvoie la valeur par défaut du coefficient psi2.
     * \param type (in) Le type de l'action.
     * \return double
     */
    virtual double getpsi2 (uint8_t type) const = 0;
};

#endif
