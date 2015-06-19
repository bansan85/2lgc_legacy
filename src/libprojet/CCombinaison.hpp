#ifndef CCOMBINAISON__HPP
#define CCOMBINAISON__HPP

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

#include "CAction.hpp"

/**
 * \brief Contient une action et un flag pour savoir si les actions variables doivent être considérées comme prédominante. Les combinaisons sont ensuite regroupées en liste et servent ensuite à générer des pondérations.
 */
class CCombinaison
{
  // Attributes
  private :
    /// Action à manipuler.
    CAction * action;
    /// Le flag vaut 1 si les actions variables sont prédominantes.
    uint8_t flags : 1;
  // Operations
  public :
    /**
     * \brief Constructeur d'une classe CCombinaison.
     * \param action (in) L'action à manipuler
     */
    CCombinaison (CAction * action);
    /**
     * \brief Duplication d'une classe CCombinaison.
     * \param other (in) La classe à dupliquer.
     */
    CCombinaison (const CCombinaison & other) = delete;
    /**
     * \brief Assignment operator de CCombinaison.
     * \param other (in) La classe à dupliquer.
     * \return CCombinaison &
     */
    CCombinaison & operator = (const CCombinaison & other) = delete;
    /**
     * \brief Destructeur d'une classe CCombinaison.
     */
    virtual ~CCombinaison ();
    /**
     * \brief Renvoie l'action de la pondération.
     * \return CAction const &
     */
    CAction const & getAction () const;
    /**
     * \brief Défini l'action de la pondération.
     * \param action (in) La nouvelle action.
     * \return bool CHK
     */
    bool CHK setAction (CAction * action);
};

#endif
