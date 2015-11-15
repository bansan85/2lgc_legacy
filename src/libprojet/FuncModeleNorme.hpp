#ifndef FUNCMODELENORME__HPP
#define FUNCMODELENORME__HPP

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

class CModele;
class INorme;

#include <libxml/tree.h>
#include "ENorme.hpp"
#include "INorme.hpp"

/**
 * \brief Classe regroupant la manipulation des normes par le modèle.
 */
class FuncModeleNorme
{
  // Attributes
  private :
    /// Le modèle rattaché à la classe.
    CModele & modele;
  // Operations
  public :
    /**
     * \brief Constructeur d'une classe FuncModeleNorme.
     * \param modele_ (in) Le modèle rattaché à l'ajout de l'action.
     */
    FuncModeleNorme (CModele & modele_);
    /**
     * \brief Duplication d'une classe FuncModeleNorme.
     * \param other (in) La classe à dupliquer.
     */
    FuncModeleNorme (const FuncModeleNorme & other) = delete;
    /**
     * \brief Duplication d'une classe FuncModeleNorme.
     * \param other (in) La classe à dupliquer.
     * \return FuncModeleNorme &
     */
    FuncModeleNorme & operator = (const FuncModeleNorme & other) = delete;
    /**
     * \brief Destructeur d'une classe FuncModeleNorme.
     */
    virtual ~FuncModeleNorme ();
    /**
     * \brief Exécute la fonction d'attribuer d'une norme par défaut.
     * \param norme (???) La norme à ajouter.
     * \return bool CHK
     */
    bool CHK doSet (std::shared_ptr <INorme> & norme);
    /**
     * \brief Format XML pour définir la norme d'un projet.
     * \param id (in) L'identifiant interne de la norme.
     * \param type (in) La norme correspondante.
     * \param variante (in) La variante de la norme.
     * \param options (in) Les options de la norme.
     * \param nom (in) Le nom de la norme.
     * \param root (in) Le noeud où ajouter les informations.
     * \return bool CHK
     */
    bool CHK doXMLSet (uint32_t id, ENorme type, uint32_t variante, uint32_t options, std::shared_ptr <const std::string> & nom, xmlNodePtr root) const;
};

#endif
