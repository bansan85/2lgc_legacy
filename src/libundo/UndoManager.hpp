#ifndef UNDOMANAGER__HPP
#define UNDOMANAGER__HPP

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

#include <MUndo.hpp>
#include <cstdint>
#include <config.h>
#include "EUndoEtat.hpp"
#include "ISujet.hpp"
#include "POCO/UndoData.hpp"

/**
 * \brief Le système de gestion des annulations et de mémorisation de l'historique.
 *        Important : UndoManager ne doit pas être tel quel. Le projet doit hériter de UndoManager et UndoManager doit être utilisé que par le projet par polymorphisme.
 */
class DllExport UndoManager : public ISujet
{
  // Associations
  // Attributes
  private :
    /// Historique des modifications. Fonctionne comme une pile mais il est nécessaire de pouvoir la parcourir sans dépiler.
    std::list <POCO::UndoData *> liste;
    /// Défini la position des modifications par rapport au haut de la pile.
    size_t pos;
    /// Les modifications en cours sont intégrés lorsque count descend à 0. En cas de modification, chaque méthode ref augmente de 1 et unref diminue de 1. Cela permet de prendre en compte plusieurs modifications dans un même évènement. Les fonctions undo / redo ne peuvent s'exécuter si count est différent de 0.
    uint16_t count;
    /// La liste temporaire. Créer dynaniquement lorsque ref passe de 0 à 1 et ajouté automatiquement à la liste lorsque ref passe de 1 à 0.
    POCO::UndoData * tmpListe;
    /// Taille de l'historique
    size_t memory;
    /// Définit pour quelle valeur maximale de count la description des opérations doit être enregistrée dans la description de l'opération d'annulation / de répétition. Défaut 1.
    uint16_t sauveDesc;
    /// Si true, le gestionnaire d'annulation accepte les demandes de push, ref et unref. Peut être utile dans 2 cas : 1) lorsqu'une opération undo/redo est appelée puisque ces opérations vont devoir appeler des fonctions qui devraient normalement modifiées la pile. 2) Lorsqu'une fonction qui s'enregistre dans la pile appelle d'autres fonctions qui s'enregistre également dans la pile, afin d'éviter un double enregistrement.
    bool insertion : 1;
  // Operations
  public :
    /**
     * \brief Constructeur d'une classe UndoManager.
     *        Initialise le système de gestion de l'historique et de la gestion des annuler / répéter.
     */
    UndoManager ();
    /**
     * \brief Duplication d'une classe UndoManager.
     * \param other (in) La classe à dupliquer.
     */
    UndoManager (const UndoManager & other) = delete;
    /**
     * \brief Duplication d'une classe UndoManager.
     * \param other (in) La classe à dupliquer.
     * \return UndoManager &
     */
    UndoManager & operator = (const UndoManager & other) = delete;
    /**
     * \brief Destructeur d'une classe UndoManager.
     *        Libère l'historique du projet.
     */
    virtual ~UndoManager ();
    /**
     * \brief Ajoute une modification à la liste.
     * \param annule (in) La fonction à lancer pour annuler la modification.
     * \param repete (in) La fonction à lancer pour répéter la modification.
     * \param suppr (in) Le pointer à conserver si nécessaire.
     * \param sauve (in) La fonction à lancer pour enregistrer dans un fichier l'historique.
     * \param description (in) La description de l'action.
     * \return bool CHK
     */
    bool CHK push (std::function <bool ()> annule, std::function <bool ()> repete, std::shared_ptr <void> suppr, std::function <bool (xmlNodePtr)> sauve, const std::string & description);
    /**
     * \brief Ajoute une fonction permettant de libérer de la mémoire.
     * \param suppr (in) Le pointer à conserver si nécessaire.
     * \return bool CHK
     */
    bool CHK pushSuppr (std::shared_ptr <void> suppr);
    /**
     * \brief Annule la dernière modification de la liste.
     * \return bool CHK
     */
    bool CHK undo ();
    /**
     * \brief Annule les n dernières modifications de la liste.
     * \param nb (in) Le nombre de modifications à annuler.
     * \return bool CHK
     */
    bool CHK undoN (uint32_t nb);
    /**
     * \brief Renvoie le nombre d'opérations pouvant être annulées.
     * \return size_t
     */
    size_t undoNb () const;
    /**
     * \brief Renvoie la description de la nième opération à annuler.
     * \param n (in) L'opération à décrire.
     * \return const std::string *
     */
    const std::string * undoDesc (size_t n) const;
    /**
     * \brief Rétablit la dernière modification de la liste.
     * \return bool CHK
     */
    bool CHK redo ();
    /**
     * \brief Rétablit les n dernières modifications de la liste.
     * \param nb (in) Le nombre de modifications à rétablir.
     * \return bool CHK
     */
    bool CHK redoN (uint32_t nb);
    /**
     * \brief Renvoie le nombre d'opérations pouvant être répétées.
     * \return size_t
     */
    size_t redoNb () const;
    /**
     * \brief Renvoie la description de la nième opération à répéter.
     * \param n (in) L'opération à décrire.
     * \return const std::string *
     */
    const std::string * redoDesc (size_t n) const;
    /**
     * \brief Renvoie l'état du gestionnaire des annulations.
     * \return EUndoEtat
     */
    EUndoEtat getEtat () const;
    /**
     * \brief Augmente le count de 1.
     * \return bool CHK
     */
    bool CHK ref ();
    /**
     * \brief Diminue le count de 1. Ajoute tmpListe à liste si count passe à 0.
     * \return bool CHK
     */
    bool CHK unref ();
    /**
     * \brief Renvoie les informations du gestionnaire d'annulation sous forme XML.
     * \param root (in) Le noeud dans lequel doit être inséré l'historique.
     * \return bool CHK
     */
    bool CHK undoToXML (xmlNodePtr root) const;
    /**
     * \brief Annule les modifications en cours sur la base de ceux dans la liste tmpListe. Force count à 0.
     * \return void
     */
    void rollback ();
    /**
     * \brief Renvoie si l'insertion via push est possible.
     * \return bool CHK
     */
    bool CHK getInsertion () const;
    /**
     * \brief Défini si l'insertion via push est autorisée.
     * \param insert (in) La nouvelle valeur.
     * \return void
     */
    void setInsertion (bool insert);
    /**
     * \brief Défini la taille de l'historique.
     * \param taille (in) La nouvelle taille.
     * \return void
     */
    void setMemory (size_t taille);
    /**
     * \brief Défini l'attribut sauveDesc.
     * \param val (in) La nouvelle valeur.
     * \return void
     */
    void setSauveDesc (uint16_t val);
    /**
     * \brief Converti la liste des annulations dans une liste consultable par l'interface graphique.
     *        Sous GTK+, cette liste est un GtkPopoverMenu.
     *        TODO !!!
     * \param param1 (in) Pa
     * \return bool CHK
     */
    bool CHK gUndoToList (void * param1);
};

#endif
