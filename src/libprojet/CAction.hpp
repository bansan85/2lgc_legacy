#ifndef CACTION__HPP
#define CACTION__HPP

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

#include <ICharge.hpp>
#include <cholmod.h>
#include <vector>
#include <array>
#include "IActionGroupe.hpp"
#include "IParametres.hpp"
#include "CFonction.hpp"

/**
 * \brief Objet définissant une action avec sa liste de charges.
 */
class DllExport CAction : public IActionGroupe
{
  // Associations
  // Attributes
  private :
    /// La description de type est donnée par IParametres::getpsiDescription.
    uint8_t type;
    /// Flag utilisé temporairement lors des calculs.
    bool action_predominante;
    /// Liste des charges (ponctuelle, répartie, …).
    std::list <std::shared_ptr <ICharge> > charges;
    /// Valeur de combinaison d'une charge variable.
    std::shared_ptr <INb> psi0;
    /// Valeur fréquente d'une action variable.
    std::shared_ptr <INb> psi1;
    /// Valeur quasi-permanente d'une action variable.
    std::shared_ptr <INb> psi2;
    /// Lien vers les paramètres de calculs.
    std::shared_ptr <IParametres> parametres;
    /// Déplacement des nœuds de la structure sous l'effet de l'action.
    cholmod_sparse * deplacement;
    /// Efforts équivalents des charges dans les nœuds de la structure.
    cholmod_sparse * forces;
    /// Efforts (y compris les réactions d'appui) dans les nœuds.
    cholmod_sparse * efforts_noeuds;
    /// Les 6 fonctions (N, Ty, Tz, Mx, My, Mz) par barre.
    std::array <std::vector <CFonction *>, 6> efforts;
    /// Les 3 déformations x, y, z pour chaque barre dans le repère local.
    std::array <std::vector <CFonction *>, 3> deformation;
    /// Les 3 rotations rx, ry, rz pour chaque barre dans le repère local.
    std::array <std::vector <CFonction *>, 3> rotation;
  // Operations
  public :
    /**
     * \brief Constructeur d'une classe CAction. Par défaut, les coefficients psi sont défini à NAN. Les valeurs sont automatiquement déterminés lors de l'insertion dans le projet en fonction de la norme du projet.
     * \param nom_ (in) Le nom de l'action.
     * \param type_ (in) Le type d'action, cf. _1990_action_bat_txt_type.
     * \param undo_ (in) Le système de gestion des annulations.
     */
    CAction (std::shared_ptr <std::string> nom_, uint8_t type_, UndoManager & undo_);
    /**
     * \brief Duplication d'une classe CAction.
     * \param other (in) La classe à dupliquer.
     */
    CAction (const CAction & other) = delete;
    /**
     * \brief Duplication d'une classe CAction.
     * \param other (in) La classe à dupliquer.
     * \return CAction &
     */
    CAction & operator = (const CAction & other) = delete;
    /**
     * \brief Destructeur d'une classe CAction.
     */
    virtual ~CAction ();
    /**
     * \brief Converti la fonction d'ajout d'une action sous format XML.
     * \param nom_ (in) Le nom de l'action.
     * \param type_ (in) Le type d'action, cf. _1990_action_bat_txt_type.
     * \param root (in) Le noeud dans lequel doit être inséré l'action.
     * \return bool CHK
     */
    static bool CHK addXML (std::string * nom_, std::string * type_, xmlNodePtr root);
    /**
     * \brief Renvoie le type de l'action.
     * \return uint8_t
     */
    uint8_t getType () const;
    /**
     * \brief Défini le type de l'action.
     * \param type_ (in) Le nouveau type.
     * \return bool CHK
     */
    bool CHK setType (uint8_t type_);
    /**
     * \brief Converti la fonction de modification du psi d'une action sous format XML.
     * \param nom_ (in) Le nom de l'action.
     * \param psi (in) Le coefficient psi à changer (0, 1 ou 2).
     * \param psin (in) Le coefficient psi à convertir.
     * \param root (in) Le noeud dans lequel doit être inséré la branche.
     * \return bool CHK
     */
    bool CHK setpsiXML (std::string * const nom_, uint8_t psi, INb * psin, xmlNodePtr root) const;
    /**
     * \brief Renvoie le cœfficient psi.
     * \param psi (in) Le numéro du coefficient à renvoyer (0, 1 ou 2).
     * \return INb const *
     */
    INb const * getPsi (uint8_t psi) const;
    /**
     * \brief Défini le cœfficient psi0.
     * \param psi (in) Le numéro du coefficient à renvoyer (0, 1 ou 2).
     * \param val (in) Le nouveau cœfficient. Vaut NULL lors de la création de l'action.
     * \return bool CHK
     */
    bool CHK setPsi (uint8_t psi, std::shared_ptr <INb> val);
    /**
     * \brief Renvoie true si aucune charge n'est présente.
     * \return bool CHK
     */
    bool CHK emptyCharges () const;
    /**
     * \brief Ajoute plusieurs charges à l'action.
     * \param charges_ (in) La liste de charges à ajouter.
     * \return bool CHK
     */
    bool CHK addCharges (std::list <std::shared_ptr <ICharge> > & charges_);
    /**
     * \brief Enlève plusieurs charges à l'action.
     * \param charges_ (in) La liste de charges à enlever.
     * \return bool CHK
     */
    bool CHK rmCharges (std::list <ICharge *> * charges_);
    /**
     * \brief Renvoie le type de l'action sous forme de texte.
     * \param type_ (in) Le type de l'action à décrire.
     * \return std::string const
     */
    std::string const getDescription (uint8_t type_) const;
    /**
     * \brief Défini la norme que doit utiliser l'action. Ne nécessite pas de fonction XML puisqu'elle ne doit être appelée que depuis la fonction CProjet::setParametres.
     * \param param (in) Le type IParametres.
     * \param psi0_ (in) Le coefficient psi0.
     * \param psi1_ (in) Le coefficient psi1.
     * \param psi2_ (in) Le coefficient psi2.
     * \return CHK bool
     */
    CHK bool setParam (std::shared_ptr <IParametres> param, std::shared_ptr <INb> psi0_, std::shared_ptr <INb> psi1_, std::shared_ptr <INb> psi2_);
    /**
     * \brief Converti la fonction setParam en format XML.
     * \param action (in) Le nom de l'action.
     * \param param (in) Le nom des paramètres.
     * \param psi0_ (in) Le coefficient psi0.
     * \param psi1_ (in) Le coefficient psi1.
     * \param psi2_ (in) Le coefficient psi2.
     * \param root (in) Le noeud dans lequel doit être inséré la branche.
     * \return bool CHK
     */
    bool CHK setParamXML (std::string * action, std::string * param, INb * psi0_, INb * psi1_, INb * psi2_, xmlNodePtr root) const;
};

#endif
