#ifndef IOBSERVEUR__HPP
#define IOBSERVEUR__HPP

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

#include "EEvent.hpp"

/**
 * \brief Observateur qui recevera les évènements du sujet.
 */
class DllExport IObserveur
{
  // Operations
  public :
    /**
     * \brief Constructeur d'une interface IObserveur.
     */
    IObserveur ();
    /**
     * \brief Duplication d'une interface IObserveur.
     * \param other (in) La classe à dupliquer.
     */
    IObserveur (const IObserveur & other) = delete;
    /**
     * \brief Duplication d'une interface IObserveur.
     * \param other (in) La classe à dupliquer.
     * \return IObserveur &
     */
    IObserveur & operator = (const IObserveur & other) = delete;
    /**
     * \brief Destructeur d'une interface IObserveur.
     */
    virtual ~IObserveur ();
    /**
     * \brief Procédure recevant les signaux du sujet observé.
     * \param event (in) L'évènement du sujet.
     * \param param (in) Le paramètre éventuel.
     * \return void
     */
    virtual void signal (EEvent event, void * param) = 0;
};

#endif
