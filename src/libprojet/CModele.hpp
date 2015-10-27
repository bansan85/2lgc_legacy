#ifndef CMODELE__HPP
#define CMODELE__HPP

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

class CPonderation;
class CPonderations;
class CCalculs;
class CActionSetPsi;
class CAction;
class CModeleAddAction;
class CModeleRmAction;
class CModeleSetParametres;
class IUndoFunc;

#include <IMateriau.hpp>
#include <ISection.hpp>
#include <INoeud.hpp>
#include <UndoManager.hpp>
#include <ISujet.hpp>
#include "CModeleSetParametres.hpp"
#include "CModeleRmAction.hpp"
#include "CModeleAddAction.hpp"
#include "CCalculs.hpp"
#include "CPreferences.hpp"
#include "ENorme.hpp"
#include "IParametres.hpp"
#include "CAppui.hpp"
#include "CRelachement.hpp"
#include "CBarre.hpp"
#include "CNiveauGroupe.hpp"
#include "CAction.hpp"

/**
 * \brief Contient toutes les données définissant la structure à analyser.
 */
class DllExport CModele : public CModeleSetParametres, public CModeleRmAction, public CModeleAddAction, public ISujet, public CCalculs
{
  // Associations
  // Attributes
  private :
    /// Liste des types d'appuis.
    std::list <std::shared_ptr <CAppui> > appuis;
    /// Liste des noeuds.
    std::list <std::shared_ptr <INoeud> > noeuds;
    /// Liste des sections.
    std::list <std::shared_ptr <ISection> > sections;
    /// Liste des matériaux.
    std::list <std::shared_ptr <IMateriau> > materiaux;
    /// Liste des relâchements.
    std::list <std::shared_ptr <CRelachement> > relachements;
    /// Liste des barres.
    std::list <std::shared_ptr <CBarre> > barres;
    /// Liste des actions.
    std::list <std::shared_ptr <CAction> > actions;
    /// Compatibilités entres actions.
    std::list <std::shared_ptr <CNiveauGroupe> > niveaux_groupes;
    /// Le gestionnaire d'annulation
    UndoManager undoManager;
  // Operations
  public :
    /**
     * \brief Constructeur d'une classe CModele.
     */
    CModele ();
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
    bool CHK addAppui (std::shared_ptr <CAppui> appui);
    /**
     * \brief Recherche un appui.
     * \param nom (in) Le nom de l'appui.
     * \return CAppui *
     */
    CAppui * getAppui (std::string nom) const;
    /**
     * \brief Supprime un appui.
     * \param appui (in) L'appui à supprimer.
     * \return bool CHK
     */
    bool CHK rmAppui (CAppui * appui);
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
    bool CHK addNoeud (std::shared_ptr <INoeud> noeud);
    /**
     * \brief Recherche un nœud.
     * \param num (in) Le numéro du nœud.
     * \return INoeud *
     */
    INoeud * getNoeud (uint32_t num) const;
    /**
     * \brief Supprime un nœud.
     * \param noeud (in) Le nœud à supprimer.
     * \return bool CHK
     */
    bool CHK rmNoeud (INoeud * noeud);
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
    bool CHK addSection (std::shared_ptr <ISection> section);
    /**
     * \brief Recherche une section.
     * \param nom (in) Le nom de la section.
     * \return ISection *
     */
    ISection * getSection (std::string nom) const;
    /**
     * \brief Supprime une section.
     * \param section (in) La section à supprimer.
     * \return bool CHK
     */
    bool CHK rmSection (ISection * section);
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
    bool CHK addMateriau (std::shared_ptr <IMateriau> materiau);
    /**
     * \brief Recherche un matériau.
     * \param nom (in) Le nom du matériau.
     * \return IMateriau *
     */
    IMateriau * getMateriau (std::string nom) const;
    /**
     * \brief Supprime un matériau.
     * \param materiau (in) Le matériau à supprimer.
     * \return bool CHK
     */
    bool CHK rmMateriau (IMateriau * materiau);
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
    bool CHK addRelachement (std::shared_ptr <CRelachement> relachement);
    /**
     * \brief Recherche un relâchement.
     * \param nom (in) Le nom du relâchement.
     * \return CRelachement *
     */
    CRelachement * getRelachement (std::string nom) const;
    /**
     * \brief Supprime un relâchement.
     * \param relachement (in) Le relâchement à supprimer.
     * \return bool CHK
     */
    bool CHK rmRelachement (CRelachement * relachement);
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
    bool CHK addBarre (std::shared_ptr <CBarre> barre);
    /**
     * \brief Recherche une barre.
     * \param num (in) Le numéro de la barre.
     * \return CBarre *
     */
    CBarre * getBarre (uint32_t num) const;
    /**
     * \brief Supprime une barre.
     * \param barre (in) La barre à supprimer.
     * \return bool CHK
     */
    bool CHK rmBarre (CBarre * barre);
    /**
     * \brief Supprime une barre.
     * \param num (in) Le numéro de la barre à supprimer.
     * \return bool CHK
     */
    bool CHK rmBarre (uint32_t num);
    /**
     * \brief Ajoute une action au modèle.
     * \param action (in) L'action à ajouter.
     * \return bool CHK
     */
    bool CHK addAction (std::shared_ptr <CAction> action);
    /**
     * \brief Recherche une action.
     * \param nom (in) Le nom de l'action.
     * \return CAction *
     */
    CAction * getAction (const std::string & nom) const;
    /**
     * \brief Renvoie le nombre d'actions.
     * \return size_t
     */
    size_t getActionCount () const;
    /**
     * \brief Supprime une action.
     * \param action (in) L'action à supprimer.
     * \return bool CHK
     */
    bool CHK rmAction (std::shared_ptr <CAction> action);
    /**
     * \brief Renvoie les paramètres de calculs.
     * \return std::shared_ptr <IParametres> &
     */
    std::shared_ptr <IParametres> & getParametres () const;
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
