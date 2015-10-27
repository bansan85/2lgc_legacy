#ifndef CMODELESETPARAMETRES__HPP
#define CMODELESETPARAMETRES__HPP

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

class CModele;
class IUndoFunc;

#include "IUndoFunc.hpp"

/**
 * \brief Classe annulable permettant de définir les normes de calculs.
 */
class CModeleSetParametres : public IUndoFunc
{
  // Operations
  public :
    /**
     * \brief Constructeur d'une classe CModeleSetParametres.
     */
    CModeleSetParametres ();
    /**
     * \brief Duplication d'une classe CModeleSetParametres.
     * \param other (in) La classe à dupliquer.
     */
    CModeleSetParametres (const CModeleSetParametres & other) = delete;
    /**
     * \brief Duplication d'une classe CModeleSetParametres.
     * \param other (in) La classe à dupliquer.
     * \return CModeleSetParametres &
     */
    CModeleSetParametres & operator = (const CModeleSetParametres & other) = delete;
    /**
     * \brief Destructeur d'une classe CModeleSetParametres.
     */
    virtual ~CModeleSetParametres ();
    /**
     * \brief Exécute la fonction.
     * \return bool CHK
     */
    bool CHK execute ();
    /**
     * \brief Format XML pour appliquer les modifications.
     * \param node (in) Le noeud où ajouter les informations.
     * \return bool CHK
     */
    bool CHK doXML (xmlNodePtr node);
    /**
     * \brief La description de la modification.
     * \return const std::string &
     */
    const std::string & doDescription ();
    /**
     * \brief Annule la fonction.
     * \return bool CHK
     */
    bool CHK undo ();
    /**
     * \brief Format XML  pour annuler les modifications.
     * \param node (in) Le noeud où ajouter les informations.
     * \return bool CHK
     */
    bool CHK undoXML (xmlNodePtr node);
    /**
     * \brief La description de l'annulation.
     * \return const std::string &
     */
    const std::string & undoDescription ();
    /**
     * \brief La fonction renvoie true si la classe modifie le modèle et false si elle ne modifie rien de significatif.
     * \return bool CHK
     */
    virtual bool CHK getModification () const;
};

#endif
