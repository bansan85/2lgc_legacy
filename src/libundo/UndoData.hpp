#ifndef UNDODATA__HPP
#define UNDODATA__HPP

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
#include <functional>
#include <libxml/tree.h>
#include <memory>
#include <string>

/**
 * \brief Ensemble de données nécessaire pour annuler, répéter et enregistrer une action.
 */
class UndoData
{
  // Attributes
  public :
    /// Fonctions à exécuter dans l'ordre pour annuler les modifications.
    std::list <std::function <bool ()> > annule;
    /// Fonctions à exécuter dans l'ordre pour rétablir les modifications.
    std::list <std::function <bool ()> > repete;
    /// Liste des variables à libérer la mémoire lors de la libération du CProjet ou si l'historique est plein.
    std::list <std::shared_ptr <void> > suppr;
    /// Fonctions à exécuter pour pouvoir enregistrer cette classe dans un fichier.
    std::list <std::function <bool (xmlNodePtr)> > sauve;
    /// Date de la modification.
    time_t heure;
    /// Description de l'opération.
    std::string description;
  // Operations
    /**
     * \brief Constructeur d'une classe UndoData.
     */
    UndoData ();
    /**
     * \brief Duplication d'une classe UndoData.
     * \param other (in) La classe à dupliquer.
     */
    UndoData (const UndoData & other) = delete;
    /**
     * \brief Duplication d'une classe UndoData.
     * \param other (in) La classe à dupliquer.
     * \return UndoData &
     */
    UndoData & operator = (const UndoData & other) = delete;
    /**
     * \brief Destructeur d'une classe UndoData.
     */
    ~UndoData ();
};

#endif
