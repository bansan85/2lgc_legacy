#ifndef CPARAMEC__HPP
#define CPARAMEC__HPP

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

#include <IParametres.hpp>
#include "ENormeEcAc.hpp"

/**
 * \brief Contient les options de calculs selon les Eurocodes.
 */
class DllExport CParamEC : public IParametres
{
  // Attributes
  private :
    /// Le nom décrivant la norme et les paramètres.
    std::shared_ptr <std::string> nom;
    /// L'annexe nationale à utiliser.
    ENormeEcAc annexe;
    /// La variante de la norme.
    uint32_t variante;
  // Operations
  public :
    /**
     * \brief Constructeur d'une classe CParamEC.
     * \param nom_ (in) Le nom décrivant la norme et ses différents options.
     * \param annexe_ (in) Le choix de l'annexe nationale.
     * \param variante_ (in) Le numéro de la variante de la norme.
     */
    CParamEC (std::shared_ptr <std::string> nom_, ENormeEcAc annexe_, uint16_t variante_);
    /**
     * \brief Constructeur d'une classe CParamEC.
     * \param other (in) La classe à dupliquer.
     */
    CParamEC (const CParamEC & other) = delete;
    /**
     * \brief Constructeur d'une classe CParamEC.
     * \param other (in) La classe à dupliquer.
     * \return CParamEC &
     */
    CParamEC & operator = (const CParamEC & other) = delete;
    /**
     * \brief Destructeur d'une classe CParamEC.
     */
    virtual ~CParamEC ();
    /**
     * \brief Renvoie le nom des paramètres de calcul.
     * \return const std::shared_ptr <std::string> &
     */
    const std::shared_ptr <std::string> & getNom () const;
    /**
     * \brief Renvoie la variante de la norme.
     * \return uint32_t
     */
    uint32_t getVariante () const;
    /**
     * \brief Renvoie le nombre de différents types d'actions.
     * \return uint8_t
     */
    uint8_t getpsiN () const;
    /**
     * \brief Renvoie la catégorie du type d'action.
     * \param type (in) Le type d'action.
     * \return EAction
     */
    EAction getpsiAction (uint8_t type) const;
    /**
     * \brief Renvoie la description du type d'actions.
     * \param type (in) Le type d'action à décrire.
     * \return std::string const
     */
    std::string const getpsiDescription (uint8_t type) const;
    /**
     * \brief Renvoie la valeur par défaut du coefficient psi0.
     * \param type (in) Le type de l'action.
     * \return double
     */
    double getpsi0 (uint8_t type) const;
    /**
     * \brief Renvoie la valeur par défaut du coefficient psi1.
     * \param type (in) Le type de l'action.
     * \return double
     */
    double getpsi1 (uint8_t type) const;
    /**
     * \brief Renvoie la valeur par défaut du coefficient psi2.
     * \param type (in) Le type de l'action.
     * \return double
     */
    double getpsi2 (uint8_t type) const;
};

#endif
