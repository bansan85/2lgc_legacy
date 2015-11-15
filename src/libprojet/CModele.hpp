#ifndef CMODELE__HPP
#define CMODELE__HPP

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
class FuncModeleNorme;
class FuncModeleAction;

#include <UndoManager.hpp>
#include <ISujet.hpp>
#include "FuncModeleAction.hpp"
#include "FuncModeleNorme.hpp"
#include "ENorme.hpp"
#include "CCalculs.hpp"
#include "POCO/CPreferences.hpp"
#include "INorme.hpp"
#include "POCO/str/CAppui.hpp"
#include "POCO/str/INoeud.hpp"
#include "POCO/str/ISection.hpp"
#include "POCO/str/IMateriau.hpp"
#include "POCO/str/CRelachement.hpp"
#include "POCO/str/CBarre.hpp"
#include "POCO/sol/CNiveauGroupe.hpp"
#include "POCO/sol/CAction.hpp"

/**
 * \brief Contient toutes les données définissant la structure à analyser.
 */
class DllExport CModele : public ISujet, public CCalculs
{
  // Associations
  // Attributes
  private :
    friend class FuncModeleAction;
    friend class FuncModeleNorme;
    /// Liste des types d'appuis.
    std::list <std::shared_ptr <POCO::str::CAppui> > appuis;
    /// Liste des noeuds.
    std::list <std::shared_ptr <POCO::str::INoeud> > noeuds;
    /// Liste des sections.
    std::list <std::shared_ptr <POCO::str::ISection> > sections;
    /// Liste des matériaux.
    std::list <std::shared_ptr <POCO::str::IMateriau> > materiaux;
    /// Liste des relâchements.
    std::list <std::shared_ptr <POCO::str::CRelachement> > relachements;
    /// Liste des barres.
    std::list <std::shared_ptr <POCO::str::CBarre> > barres;
    /// Liste des actions.
    std::list <std::shared_ptr <POCO::sol::CAction> > actions;
    /// Compatibilités entres actions.
    std::list <std::shared_ptr <POCO::sol::CNiveauGroupe> > niveaux_groupes;
    /// Norme de calculs. Défini lors de la création du modèle.
    std::shared_ptr <INorme> norme;
    /// Les préférences du projet.
    POCO::CPreferences preferences;
    /// Le gestionnaire d'annulation
    UndoManager undoManager;
  public :
    /// Classe gérant la manipulation des actions.
    FuncModeleAction fAction;
    /// Classe gérant la manipulation des normes.
    FuncModeleNorme fNorme;
  // Operations
    /**
     * \brief Constructeur d'une classe CModele.
     * \param norme (in) Norme de calcul.
     */
    CModele (ENorme norme);
    /**
     * \brief Duplication d'une classe CModele.
     * \param other (in) La classe à dupliquer.
     */
    CModele (const CModele & other) = delete;
    /**
     * \brief Duplication d'une classe CModele.
     * \param other (in) La classe à dupliquer.
     * \return CModele &
     */
    CModele & operator = (const CModele & other) = delete;
    /**
     * \brief Destructeur d'une classe CModele.
     */
    virtual ~CModele ();
    /**
     * \brief Ajout d'un appui.
     * \param appui (in) Le nouveau appui.
     * \return bool CHK
     */
    bool CHK addAppui (std::shared_ptr <POCO::str::CAppui> appui);
    /**
     * \brief Recherche un appui.
     * \param nom (in) Le nom de l'appui.
     * \return POCO::str::CAppui *
     */
    POCO::str::CAppui * getAppui (std::string nom) const;
    /**
     * \brief Supprime un appui.
     * \param appui (in) L'appui à supprimer.
     * \return bool CHK
     */
    bool CHK rmAppui (POCO::str::CAppui * appui);
    /**
     * \brief Supprime un appui.
     * \param appui (in) Le nom de l'appui à supprimer.
     * \return bool CHK
     */
    bool CHK rmAppui (std::string appui);
    /**
     * \brief Ajout d'un nœud.
     * \param noeud (in) Le nouveau nœud.
     * \return bool CHK
     */
    bool CHK addNoeud (std::shared_ptr <POCO::str::INoeud> noeud);
    /**
     * \brief Recherche un nœud.
     * \param num (in) Le numéro du nœud.
     * \return POCO::str::INoeud *
     */
    POCO::str::INoeud * getNoeud (uint32_t num) const;
    /**
     * \brief Supprime un nœud.
     * \param noeud (in) Le nœud à supprimer.
     * \return bool CHK
     */
    bool CHK rmNoeud (POCO::str::INoeud * noeud);
    /**
     * \brief Supprime un nœud.
     * \param num (in) Le nœud à supprimer.
     * \return bool CHK
     */
    bool CHK rmNoeud (uint32_t num);
    /**
     * \brief Ajout d'une section.
     * \param section (in) Le nouveau section.
     * \return bool CHK
     */
    bool CHK addSection (std::shared_ptr <POCO::str::ISection> section);
    /**
     * \brief Recherche une section.
     * \param nom (in) Le nom de la section.
     * \return POCO::str::ISection *
     */
    POCO::str::ISection * getSection (std::string nom) const;
    /**
     * \brief Supprime une section.
     * \param section (in) La section à supprimer.
     * \return bool CHK
     */
    bool CHK rmSection (POCO::str::ISection * section);
    /**
     * \brief Supprime une section.
     * \param nom (in) La section à supprimer.
     * \return bool CHK
     */
    bool CHK rmSection (std::string nom);
    /**
     * \brief Ajout d'un matériau.
     * \param materiau (in) Le nouveau matériau.
     * \return bool CHK
     */
    bool CHK addMateriau (std::shared_ptr <POCO::str::IMateriau> materiau);
    /**
     * \brief Recherche un matériau.
     * \param nom (in) Le nom du matériau.
     * \return POCO::str::IMateriau *
     */
    POCO::str::IMateriau * getMateriau (std::string nom) const;
    /**
     * \brief Supprime un matériau.
     * \param materiau (in) Le matériau à supprimer.
     * \return bool CHK
     */
    bool CHK rmMateriau (POCO::str::IMateriau * materiau);
    /**
     * \brief Supprime un matériau.
     * \param nom (in) Le matériau à supprimer.
     * \return bool CHK
     */
    bool CHK rmMateriau (std::string nom);
    /**
     * \brief Ajout d'un relâchement.
     * \param relachement (in) Le nouveau relâchement.
     * \return bool CHK
     */
    bool CHK addRelachement (std::shared_ptr <POCO::str::CRelachement> relachement);
    /**
     * \brief Recherche un relâchement.
     * \param nom (in) Le nom du relâchement.
     * \return POCO::str::CRelachement *
     */
    POCO::str::CRelachement * getRelachement (std::string nom) const;
    /**
     * \brief Supprime un relâchement.
     * \param relachement (in) Le relâchement à supprimer.
     * \return bool CHK
     */
    bool CHK rmRelachement (POCO::str::CRelachement * relachement);
    /**
     * \brief Supprime un relâchement.
     * \param nom (in) Le relâchement à supprimer.
     * \return bool CHK
     */
    bool CHK rmRelachement (std::string nom);
    /**
     * \brief Ajout d'une barre.
     * \param barre (in) La nouvelle barre.
     * \return bool CHK
     */
    bool CHK addBarre (std::shared_ptr <POCO::str::CBarre> barre);
    /**
     * \brief Recherche une barre.
     * \param num (in) Le numéro de la barre.
     * \return POCO::str::CBarre *
     */
    POCO::str::CBarre * getBarre (uint32_t num) const;
    /**
     * \brief Supprime une barre.
     * \param barre (in) La barre à supprimer.
     * \return bool CHK
     */
    bool CHK rmBarre (POCO::str::CBarre * barre);
    /**
     * \brief Supprime une barre.
     * \param num (in) Le numéro de la barre à supprimer.
     * \return bool CHK
     */
    bool CHK rmBarre (uint32_t num);
    /**
     * \brief Recherche une action.
     *        Renvoie nullptr en cas d'échec.
     *        La fonction ne peut pas être const car l'action retour peut être modifiée.
     * \param nom (in) Le nom de l'action.
     * \return std::shared_ptr <POCO::sol::CAction> *
     */
    std::shared_ptr <POCO::sol::CAction> * getAction (const std::string & nom);
    /**
     * \brief Renvoie le nombre d'actions.
     * \return size_t
     */
    size_t getActionCount () const;
    /**
     * \brief Renvoie les paramètres de calculs.
     * \return const std::shared_ptr <INorme> &
     */
    const std::shared_ptr <INorme> & getNorme () const;
    /**
     * \brief Renvoie le gestionnaire d'annulation.
     * \return UndoManager &
     */
    UndoManager & getUndoManager ();
    /**
     * \brief Enregistre le projet.
     * \param fichier (in) Le nom du fichier.
     * \return bool CHK
     */
    bool CHK enregistre (const std::string fichier) const;
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
