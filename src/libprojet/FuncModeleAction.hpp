#ifndef FUNCMODELEACTION__HPP
#define FUNCMODELEACTION__HPP

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

class CPonderation;
class CPonderations;
class CCalculs;
class CAction;
class CModele;

#include <IUndoableFonction.hpp>
#include "POCO/sol/CAction.hpp"

class FuncModeleAction : public IUndoableFonction
{
  // Attributes
  private :
    /// Le modèle rattaché à la classe.
    CModele & modele;
  // Operations
  public :
    /**
     * \brief Constructeur d'une classe FuncModeleAction.
     * \param modele_ (in) Le modèle rattaché à l'ajout de l'action.
     */
    FuncModeleAction (CModele & modele_);
    /**
     * \brief Duplication d'une classe FuncModeleAction.
     * \param other (in) La classe à dupliquer.
     */
    FuncModeleAction (const FuncModeleAction & other) = delete;
    /**
     * \brief Duplication d'une classe FuncModeleAction.
     * \param other (in) La classe à dupliquer.
     * \return FuncModeleAction &
     */
    FuncModeleAction & operator = (const FuncModeleAction & other) = delete;
    /**
     * \brief Destructeur d'une classe FuncModeleAction.
     */
    virtual ~FuncModeleAction ();
    /**
     * \brief Exécute la fonction.
     * \param action (???)
     * \return bool CHK
     */
    bool CHK doAdd (std::shared_ptr <POCO::sol::CAction> & action);
    /**
     * \brief Format XML pour ajouter une action.
     * \param id (in) L'identifiant interne de l'action.
     * \param nom_ (in) Le nom de l'action.
     * \param type_ (in) Le type d'action.
     * \param nb0 (in) Le cœfficient psi0.
     * \param nb1 (in) Le cœfficient psi1.
     * \param nb2 (in) Le cœfficient psi2.
     * \param node (in) Le noeud où ajouter les informations.
     * \return bool CHK
     */
    bool CHK doXMLAdd (uint32_t id, std::shared_ptr <const std::string> & nom_, uint8_t type_, std::shared_ptr <POCO::INb> & nb0, std::shared_ptr <POCO::INb> & nb1, std::shared_ptr <POCO::INb> & nb2, xmlNodePtr node) const;
    /**
     * \brief Annule la fonction.
     * \return bool CHK
     */
    bool CHK undoAdd ();
    /**
     * \brief Format XML pour annuler les modifications.
     * \param node (in) Le noeud où ajouter les informations.
     * \return bool CHK
     */
    bool CHK undoXMLAdd (xmlNodePtr node) const;
    /**
     * \brief Modifie le coefficient psi d'une action.
     * \param action (in) L'action à modifier.
     * \param psi (in) Le numéro du coefficient psi à modifier (0, 1 ou 2).
     * \param val (in) Le nouveau cœfficient.
     * \return bool CHK
     */
    bool CHK doSetPsi (std::shared_ptr <POCO::sol::CAction> & action, uint8_t psi, std::shared_ptr <POCO::INb> & val);
    /**
     * \brief Format XML pour modifier le cœfficient psi d'une action.
     * \param id (in) L'identifiant de l'action.
     * \param psi (in) Le numéro du coefficient psi à modifier (0, 1 ou 2).
     * \param val (in) Le nouveau cœfficient.
     * \param root (in) Le noeud où ajouter les informations.
     * \return bool CHK
     */
    bool CHK doXMLSetPsi (uint32_t id, uint8_t psi, std::shared_ptr <POCO::INb> & val, xmlNodePtr root) const;
};

#endif
