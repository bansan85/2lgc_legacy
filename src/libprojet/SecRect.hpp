#ifndef SECRECT__HPP
#define SECRECT__HPP

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
#include "ISection.hpp"

/**
 * \brief Défini une section rectangulaire.
 */
class SecRect : public ISection
{
  // Attributes
  private :
    /// Largeur de la section.
    INb * largeur;
    /// Hauteur de la section.
    INb * hauteur;
  // Operations
  public :
    /**
     * \brief Constructeur d'une classe SecRect.
     * \param largeur (in) Largeur de la section.
     * \param hauteur (in) Hauteur de la section.
     * \param undo (in) Le gestionnaire des modifications.
     */
    SecRect (INb * largeur, INb * hauteur, UndoManager & undo);
    /**
     * \brief Duplication d'une classe SecRect.
     * \param other (in) La classe à dupliquer.
     */
    SecRect (const SecRect & other) = delete;
    /**
     * \brief Assignment operator de SecRect.
     * \param other (in) La classe à dupliquer.
     * \return SecRect &
     */
    SecRect & operator = (const SecRect & other) = delete;
    /**
     * \brief Destructeur d'une classe SecRect.
     */
    virtual ~SecRect ();
    /**
     * \brief Renvoie la largeur de la section.
     * \return INb const &
     */
    INb const & getLargeur () const;
    /**
     * \brief Défini la largeur de la section.
     * \param val (in) La nouvelle valeur.
     * \return bool CHK
     */
    bool CHK setLargeur (INb * val);
    /**
     * \brief Renvoie la hauteur de la section.
     * \return INb const &
     */
    INb const & getHauteur () const;
    /**
     * \brief Défini la hauteur de la section.
     * \param val (in) La nouvelle valeur.
     * \return bool CHK
     */
    bool CHK setHauteur (INb * val);
};

#endif
