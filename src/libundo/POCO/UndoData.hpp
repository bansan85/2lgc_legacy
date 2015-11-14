#ifndef POCO_UNDODATA__HPP
#define POCO_UNDODATA__HPP

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

#include <list>
#include <functional>
#include <libxml/tree.h>
#include <memory>
#include <string>

namespace POCO
{
  /**
   * \brief Ensemble de données nécessaire pour annuler, répéter et enregistrer une action.
   */
  class UndoData
  {
    // Attributes
    private :
      /// Fonctions à exécuter dans l'ordre pour annuler les modifications.
      std::list <std::function <bool ()> > annule;
      /// Fonctions à exécuter dans l'ordre pour rétablir les modifications.
      std::list <std::function <bool ()> > repete;
      /// Liste des variables à libérer de la mémoire lors de la libération du CProjet ou si l'historique est plein.
      std::list <std::shared_ptr <void> > suppr;
      /// Fonctions à exécuter pour pouvoir enregistrer cette classe dans un fichier.
      std::list <std::function <bool (xmlNodePtr)> > sauve;
      /// Date de la modification.
      time_t heure;
      /// Description de l'opération.
      std::string description;
    // Operations
    public :
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
      virtual ~UndoData ();
      /**
       * \brief Ajoute une méthode à appeler en cas d'annulation.
       * \param fonction (in) La fonction sans paramètre convertie grâce à std::bind.
       * \return bool CHK
       */
      bool CHK addAnnuler (std::function <bool ()> fonction);
      /**
       * \brief Exécute l'ensemble des fonctions d'annulation dans l'ordre LIFO.
       * \return bool CHK
       */
      bool CHK execAnnuler ();
      /**
       * \brief Ajoute une méthode à appeler en cas de répétition.
       * \param fonction (in) La fonction sans paramètre convertie grâce à std::bind.
       * \return bool CHK
       */
      bool CHK addRepeter (std::function <bool ()> fonction);
      /**
       * \brief Exécute l'ensemble des fonctions de répétition dans l'ordre FIFO.
       * \return bool CHK
       */
      bool CHK execRepeter ();
      /**
       * \brief Ajoute une donnée à stocker en mémoire dans le cas d'une annulation ou d'une répétition.
       * \param objet (in) La donnée à conserver.
       * \return bool CHK
       */
      bool CHK addSupprimer (std::shared_ptr <void> objet);
      /**
       * \brief Ajoute une fonction permettant de sauvegarder la struture au format XML.
       * \param fonction (in) La fonction créant les informations au format XML.
       * \return bool CHK
       */
      bool CHK addToXML (std::function <bool (xmlNodePtr)> fonction);
      /**
       * \brief Exécute l'ensemble des fonctions permettant de sauvegarder la struture au format XML dans l'ordre FIFO.
       * \param noeud (in) Noeud XML d'enregistrement des modifications.
       * \return bool CHK
       */
      bool CHK execToXML (xmlNodePtr noeud) const;
      /**
       * \brief Renvoie l'heure de la modification des données.
       * \return time_t
       */
      time_t getHeure ();
      /**
       * \brief Définit l'heure de la modification par l'heure en cours.
       * \return bool CHK
       */
      bool CHK setHeure ();
      /**
       * \brief Renvoie la description des données.
       * \return const std::string &
       */
      const std::string & getDescription () const;
      /**
       * \brief Complète la description des données.
       * \param description_ (in) La description complémentaire.
       * \return bool CHK
       */
      bool CHK appendDescription (const std::string & description_);
  };
}

#endif
