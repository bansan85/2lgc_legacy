#ifndef IMATERIAU__HPP
#define IMATERIAU__HPP

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
#include <IUndoable.hpp>

/**
 * \brief Défini un matériau type.
 */
class IMateriau : public IUndoable
{
  // Attributes
  private :
    /// Nom du matériau.
    std::string nom;
    /// Module Young.
    INb * E;
    /// Cœfficient de poisson.
    INb * nu;
  // Operations
  public :
    /**
     * \brief Constructeur d'une classe IMateriau.
     */
    IMateriau ();
    /**
     * \brief Duplication d'une classe CMatAcierEc.
     * \param other (in) La classe à dupliquer.
     */
    IMateriau (const IMateriau & other) = delete;
    /**
     * \brief Duplication d'une classe CMatAcierEc.
     * \param other (in) La classe à dupliquer.
     * \return IMateriau &
     */
    IMateriau & operator = (const IMateriau & other) = delete;
    /**
     * \brief Destructeur d'une interface IMateriau.
     */
    virtual ~IMateriau ();
    /**
     * \brief Renvoie le nom du matériau.
     * \return std::string const &
     */
    std::string const & getNom () const;
    /**
     * \brief Défini le nom du matériau.
     * \param valeur (in) La nouvelle valeur.
     * \return bool CHK
     */
    bool CHK setNom (std::string valeur);
    /**
     * \brief Renvoie le module Young en Pa.
     * \return INb const &
     */
    INb const & getE () const;
    /**
     * \brief Défini le module Young en Pa.
     * \param val (in) La nouvelle valeur.
     * \return bool CHK
     */
    bool CHK setE (INb * val);
    /**
     * \brief Renvoie le cœfficient de poisson.
     * \return INb const &
     */
    INb const & getnu () const;
    /**
     * \brief Défini le cœfficient de poisson.
     * \param val (in) La nouvelle valeur.
     * \return bool CHK
     */
    bool CHK setnu (INb * val);
};

#endif
