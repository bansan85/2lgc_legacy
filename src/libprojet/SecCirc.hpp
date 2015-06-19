#ifndef SECCIRC__HPP
#define SECCIRC__HPP

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
 * \brief Défini une section circulaire.
 */
class SecCirc : public ISection
{
  // Attributes
  private :
    /// Diamètre de la section.
    INb * diametre;
  // Operations
  public :
    /**
     * \brief Constructeur d'une classe SecCirc.
     * \param diametre (in) Diamètre de la section.
     * \param undo (in) Le gestionnaire des modifications.
     */
    SecCirc (INb * diametre, UndoManager & undo);
    /**
     * \brief Duplication d'une classe SecCirc.
     * \param other (in) La classe à dupliquer.
     */
    SecCirc (const SecCirc & other) = delete;
    /**
     * \brief Assignment operator de SecCirc.
     * \param other (in) La classe à dupliquer.
     * \return SecCirc &
     */
    SecCirc & operator = (const SecCirc & other) = delete;
    /**
     * \brief Destructeur d'une classe SecCirc.
     */
    virtual ~SecCirc ();
    /**
     * \brief Renvoie la dimension du diamètre de la section.
     * \return INb const &
     */
    INb const & getDiametre () const;
    /**
     * \brief Défini la dimension du diamètre de la section.
     * \param val (in) La nouvelle valeur.
     * \return bool CHK
     */
    bool CHK setDiametre (INb * val);
};

#endif
