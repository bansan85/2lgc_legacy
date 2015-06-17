#ifndef ISUJET__HPP
#define ISUJET__HPP

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

#include <list>
#include <memory>
#include "EEvent.hpp"
#include "IObserveur.hpp"

/**
 * \brief Sujet pouvant envoyer des notifications aux observateurs.
 */
class DllExport ISujet
{
  // Attributes
  private :
    /// Liste des observeurs.
    std::list <std::shared_ptr <IObserveur> > observeurs;
  // Operations
  public :
    /**
     * \brief Constructeur d'une interface ISujet.
     */
    ISujet ();
    /**
     * \brief Duplication d'une interface ISujet.
     * \param other (in) La classe à dupliquer.
     */
    ISujet (const ISujet & other) = delete;
    /**
     * \brief Duplication d'une interface ISujet.
     * \param other (in) La classe à dupliquer.
     * \return ISujet &
     */
    ISujet & operator = (const ISujet & other);
    /**
     * \brief Destructeur d'une interface ISujet.
     */
    virtual ~ISujet ();
    /**
     * \brief Envoie une notification à l'ensemble des observeurs.
     * \param event (in) L'évènement à notifier.
     * \param param (in) Paramètre éventuel.
     * \return void
     */
    virtual void notify (EEvent event, void * param);
    /**
     * \brief Ajout d'un observateur.
     * \param obs (in) L'observateur à ajouter.
     * \return void
     */
    virtual void addObserver (std::shared_ptr <IObserveur> obs);
};

#endif
