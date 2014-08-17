/*
 * 2lgc_code : calcul de résistance des matériaux selon les normes Eurocodes
 * Copyright (C) 2011
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"

#include <algorithm>
#include <memory>
#include <iostream>
#include <locale>
#include <sys/types.h>

#include "CProjet.hpp"
#include "CUndoManager.hpp"
#include "IUndoable.hpp"
#include "MAbrev.hh"
#include "MErreurs.hh"


/**
 * \brief Initialise le système de gestion de l'historique et de la gestion des
 *        annuler / répéter.
 */
CUndoManager::CUndoManager (CProjet & proj) :
  liste (),
  pos (0),
  count (0),
  tmpListe (NULL),
  projet (proj),
  insertion (true)
{
}


/**
 * \brief Duplication d'une classe CUndoManager.
 * \param other (in) La classe à dupliquer.
 */
CUndoManager::CUndoManager (const CUndoManager & other) :
  liste (),
  pos (0),
  count (0),
  tmpListe (NULL),
  projet (other.projet),
  insertion (true)
{
}


/**
 * \brief Assignment operator de CUndoManager.
 * \param other (in) La classe à dupliquer.
 */
CUndoManager &
CUndoManager::operator = (const CUndoManager & other) = delete;


/**
 * \brief Libère l'historique du projet.
 */
CUndoManager::~CUndoManager ()
{
  for_each (liste.begin (), liste.end (), std::default_delete <CUndoData> ());
  
  delete (tmpListe);
}


/**
 * \brief Ajoute une modification à la liste.
 * \param annule (in) La fonction à lancer pour annuler la modification.
 * \param repete (in) La fonction à lancer pour répéter la modification.
 * \param suppr (in) La fonction à lancer pour libérer la mémoire. Cette
 *                   fonction est définie lorsqu'il y a ajout d'un élément en
 *                   mémoire uniquement.
 * \param sauve (in) La fonction à lancer pour enregistrer dans un fichier
 *                   l'historique.
 */
bool
CUndoManager::push (std::function <bool ()>           annule,
                    std::function <bool ()>           repete,
                    std::function <void ()>           suppr,
                    std::function <bool (xmlNodePtr)> sauve)
{
  if (!insertion)
  {
    return true;
  }
  
  BUGPROG (count != 0,
           false,
           this,
           gettext ("Impossible d'ajouter un évènement au gestionnaire d'annulation si aucune action n'est en cours (nécessité d'appeler la fonction ref).\n"))
  BUGPARAM (tmpListe, "%p", tmpListe, false, this)
  
  tmpListe->annule.push_front (annule);
  tmpListe->repete.push_back (repete);
  if (suppr != NULL)
  {
    tmpListe->suppr.push_back (suppr);
  }
  if (sauve != NULL)
  {
    tmpListe->sauve.push_back (sauve);
  }
  
  return true;
}


/**
 * \brief Ajoute une fonction permettant de libérer de la mémoire.
 * \param suppr (in) La fonction à lancer pour libérer la mémoire. Cette
 *                   fonction est définie lorsqu'il y a ajout d'un élément en
 *                   mémoire uniquement.
 */
bool
CUndoManager::pushSuppr (std::function <void ()> suppr)
{
  if (!insertion)
  {
    return true;
  }
  
  BUGPROG (count != 0,
           false,
           this,
           gettext ("Impossible d'ajouter un évènement au gestionnaire d'annulation si aucune action n'est en cours (nécessité d'appeler la fonction ref).\n"))
  
  if (suppr != NULL)
  {
    tmpListe->suppr.push_back (suppr);
  }
  
  return true;
}


/**
 * \brief Annule la dernière modification de la liste.
 */
bool
CUndoManager::undo ()
{
  CUndoData * undoData;
  std::list <CUndoData *>::iterator it;
  
  BUGPROG (liste.size () != pos,
           false,
           this,
           gettext ("Il n'y a plus rien à annuler.\n"))
  
  insertion = false;
  
  it = liste.end ();
  std::advance (it, -static_cast <ssize_t> (pos) - 1);
  undoData = *it;
  
  for (std::function <bool ()> f : undoData->annule)
  {
    BUGCRIT (f (),
             false,
             this,
             gettext ("Echec lors de l'opération.\nLe projet est très probablement corrompu.\n"))
  }
  
  ++pos;
  
  insertion = true;
  
  return true;
}


/**
 * \brief Rétablit la dernière modification de la liste.
 */
bool
CUndoManager::redo ()
{
  CUndoData * undoData;
  std::list <CUndoData *>::iterator it;
  
  BUGPROG (pos != 0,
           false,
           this,
           gettext ("Il n'y a plus rien à rétablir.\n"))
  
  insertion = false;
  
  it = liste.end ();
  std::advance (it, -static_cast <ssize_t> (pos));
  undoData = *it;
  
  for (std::function <bool ()> f : undoData->repete)
  {
    BUGCRIT (f (),
             false,
             this,
             gettext ("Echec lors de l'opération.\nLe projet est très probablement corrompu.\n"))
  }
  
  --pos;
  
  insertion = true;
  
  return true;
}


/**
 * \brief Renvoie l'état du gestionnaire des annulations.
 */
EUndoEtat
CUndoManager::getEtat () const
{
  if (count == 0)
  {
    return UNDO_NONE;
  }
  else if (insertion)
  {
    return UNDO_MODIF;
  }
  else
  {
    return UNDO_REVERT;
  }
}


/**
 * \brief Augmente le count de 1.
 */
bool
CUndoManager::ref ()
{
  BUGCRIT (count != 255,
           false,
           this,
           gettext ("Le programme est à ses limites.\n"))
  
  if (!insertion)
  {
    return true;
  }
  
  if ((count == 0) && (pos != 0))
  {
    std::list <CUndoData *>::iterator it = liste.end ();
    
    std::advance (it, -static_cast <ssize_t> (pos));
    
    for (; it != liste.end (); )
    {
      for (std::function <void ()> f : (*it)->suppr)
      {
        f ();
      }
      liste.erase (it++);
    }
    
    pos = 0;
  }
  
  if (count == 0)
  {
    tmpListe = new CUndoData ();
  }
  
  ++count;
  
  return true;
}


/**
 * \brief Diminue le count de 1.
 */
bool
CUndoManager::unref ()
{
  if (!insertion)
  {
    return true;
  }
  
  BUGPROG (count != 0,
           false,
           this,
           gettext ("Impossible d'appeler unref alors que le compteur vaut 0.\n"))
  
  --count;
  
  if (count == 0)
  {
    time (&tmpListe->heure);
    liste.push_back (tmpListe);
    tmpListe = NULL;
  }
  
  return true;
}


/**
 * \brief Renvoie les informations du gestionnaire d'annulation sous forme XML.
 */
bool
CUndoManager::undoToXML (xmlNodePtr root)
{
  std::unique_ptr <xmlNode, void (*)(xmlNodePtr)> node (
                    xmlNewNode (NULL, BAD_CAST2 ("UndoManager")), xmlFreeNode);
  
  BUGCRIT (node.get (),
           false,
           this,
           gettext ("Erreur d'allocation mémoire.\n"))
  
  for (CUndoData * data : liste)
  {
    std::unique_ptr <xmlNode, void (*)(xmlNodePtr)> node0 (
                           xmlNewNode (NULL, BAD_CAST2 ("Bloc")), xmlFreeNode);
    
    BUGCRIT (node0.get (),
             false,
             this,
             gettext ("Erreur d'allocation mémoire.\n"))
    
    for (std::function <bool (xmlNodePtr)> f : data->sauve)
    {
      BUGCRIT (f (node0.get ()),
               false,
               this,
               gettext ("Erreur lors de la génération du fichier XML.\n"))
    }
    
    BUGCRIT (xmlAddChild (node.get (), node0.get ()),
             false,
             this,
             gettext ("Erreur lors de la génération du fichier XML.\n"))
    
    node0.release ();
  }
  
  BUGCRIT (xmlAddChild (root, node.get ()),
           false,
           this,
           gettext ("Erreur lors de la génération du fichier XML.\n"))
  
  node.release ();
  
  return true;
}


/**
 * \brief Annule les modifications en cours sur la base de ceux dans la liste
 *        tmpListe.
 */
bool
CUndoManager::rollback ()
{
  count = 0;
  
  if (tmpListe == NULL)
  {
    return true;
  }
  
  insertion = false;
  
  for (std::function <bool ()> f : tmpListe->annule)
  {
    BUGCRIT (f (),
             false,
             NULL,
             gettext ("Impossible de faire marche arrière suite à l'erreur détectée.\nLe projet est très probablement corrompu.\n"))
  }
  
  delete tmpListe;
  tmpListe = NULL;
  
  insertion = true;
  
  return true;
}


/**
 * \brief Défini si l'insertion via push est autorisée.
 * \param insert La nouvelle valeur.
 */
void
CUndoManager::setInsertion (bool insert)
{
  insertion = insert;
}


/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
