#ifndef CPROJET__HPP
#define CPROJET__HPP

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

#include <UndoManager.hpp>
#include "ENorme.hpp"
#include "CPreferences.hpp"
#include "CCalculs.hpp"
#include "CModele.hpp"
#include "IParametres.hpp"

/**
 * \brief Variable projet contenant toutes les informations du projet.
 */
class DllExport CProjet : public CPreferences, public CCalculs, public CModele, public UndoManager
{
  // Attributes
  private :
    /// Paramètres de calculs.
    std::shared_ptr <IParametres> parametres;
  // Operations
  public :
    /**
     * \brief Constructeur d'une classe CProjet.
     * \param norme (in) La norme du projet.
     */
    CProjet (ENorme norme);
    /**
     * \brief Duplication d'une classe CProjet.
     * \param other (in) La classe à dupliquer.
     */
    CProjet (const CProjet & other) = delete;
    /**
     * \brief Duplication d'une classe CProjet.
     * \param other (in) La classe à dupliquer.
     * \return CProjet &
     */
    CProjet & operator = (const CProjet & other) = delete;
    /**
     * \brief Destructeur d'une classe CProjet.
     */
    virtual ~CProjet ();
    /**
     * \brief Renvoie les paramètres de calculs du projet.
     * \return std::shared_ptr <IParametres> &
     */
    std::shared_ptr <IParametres> & getParametres ();
    /**
     * \brief Défini les paramètres de calculs du projet.
     * \param param (in) Les nouveaux paramètres.
     * \return bool CHK
     */
    bool CHK setParametres (std::shared_ptr <IParametres> param);
    /**
     * \brief Converti la fonction définissant la norme que doit utiliser le projet sous format XML.
     * \param param (in) Le paramètre à utiliser.
     * \param nom (in) Le nom des paramètres de calcul.
     * \param variante (in) La variante de la norme de calcul.
     * \param root (in) Le noeud dans lequel doit être inséré la méthode sous format XML.
     * \return bool CHK
     */
    bool CHK setParametresXML (IParametres * param, std::string * nom, uint32_t variante, xmlNodePtr root);
    /**
     * \brief Enregistre le projet.
     * \param fichier (in) Le nom du fichier.
     * \return bool CHK
     */
    bool CHK enregistre (std::string fichier);
    /**
     * \brief Affiche les limites de la garantie (articles 15, 16 et 17 de la licence GPL).
     * \return void
     */
    static void showWarranty ();
    /**
     * \brief Affiche l'aide lorsque l'utilisateur lance le programme avec l'option -h.
     * \return void
     */
    static void showHelp ();
};

#endif
