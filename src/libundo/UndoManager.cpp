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

#include "Algo.hpp"
#include "UndoManager.hpp"
#include "IUndoable.hpp"
#include "MErreurs.hpp"
#include "SString.hpp"

UndoManager::UndoManager () :
  ISujet (),
  liste (),
  pos (0),
  count (0),
  tmpListe (nullptr),
  memory (SIZE_MAX),
  sauveDesc (1),
  insertion (true)
{
}

UndoManager::~UndoManager ()
{
  for_each (liste.begin (), liste.end (), std::default_delete <UndoData> ());
  
  delete (tmpListe);
}

bool CHK
UndoManager::push (std::function <bool ()>           annule,
                   std::function <bool ()>           repete,
                   std::shared_ptr <void>            suppr,
                   std::function <bool (xmlNodePtr)> sauve,
                   const std::string               & description)
{
  if (!insertion)
  {
    return true;
  }
  
  BUGPROG (count != 0,
           false,
           this,
           "Impossible d'ajouter un évènement au gestionnaire d'annulation si aucune modification n'est en cours (nécessité d'appeler la fonction ref).\n")
  BUGPARAM (static_cast <void *> (tmpListe), "%p", tmpListe, false, this)
  
  tmpListe->annule.push_front (annule);
  tmpListe->repete.push_back (repete);
  if (suppr != nullptr)
  {
    tmpListe->suppr.push_back (suppr);
  }
  if (sauve != nullptr)
  {
    tmpListe->sauve.push_back (sauve);
  }
  
  // Seules les modifications du sauveDesc-ième ordre sont mémorisées dans la
  // description.
  if (count <= sauveDesc)
  {
    if (!tmpListe->description.empty ())
    {
      tmpListe->description.append ("\n");
    }
    tmpListe->description.append (description);
  }
  
  return true;
}

bool CHK
UndoManager::pushSuppr (std::shared_ptr <void> suppr)
{
  if (!insertion)
  {
    return true;
  }
  
  BUGPROG (count != 0,
           false,
           this,
           "Impossible d'ajouter un évènement au gestionnaire d'annulation si aucune modification n'est en cours (nécessité d'appeler la fonction ref).\n")
  
  if (suppr != NULL)
  {
    tmpListe->suppr.push_back (suppr);
  }
  
  return true;
}

bool
UndoManager::undo ()
{
  UndoData                       * undoData;
  std::list <UndoData *>::iterator it;
  
  BUGPROG (liste.size () > pos,
           false,
           this,
           "Il n'y a plus rien à annuler.\n")
  
  insertion = false;
  
  it = liste.end ();
  NCALL (pos + 1, --it;);
  undoData = *it;
  
  for (std::function <bool ()> f : undoData->annule)
  {
    BUGCRIT (f (),
             false,
             this,
             "Échec lors de l'opération.\n"
               "Le projet est très probablement corrompu.\n")
  }
  
  ++pos;
  
  insertion = true;

  if (count == 0)
  {
    notify (EEvent::UNDO_NB, nullptr);
    notify (EEvent::REDO_NB, nullptr);
  }
  
  return true;
}

bool CHK
UndoManager::undoN (uint32_t nb)
{
  // Pour éviter l'émission des signaux à chaque émission de undo ().
  ++count;
  NCALL (nb, BUGCONT (undo (), false, this))
  --count;

  notify (EEvent::UNDO_NB, nullptr);
  notify (EEvent::REDO_NB, nullptr);

  return true;
}

size_t
UndoManager::undoNb () const
{
  return liste.size () - pos;
}

const std::string *
UndoManager::undoDesc (size_t n) const
{
  UndoData * undoData;
  std::list <UndoData *>::const_iterator it;

  BUGPROG (n + pos <= liste.size (),
           nullptr,
           UNDO_MANAGER_NULL,
           "Indice hors limite (%zu+%zu). La taille de la pile est de %zu.\n",
             n,
             pos,
             liste.size ())

  it = liste.end ();
  NCALL (n + pos + 1, --it;);
  undoData = *it;

  return &undoData->description;
}

bool CHK
UndoManager::redo ()
{
  UndoData * undoData;
  std::list <UndoData *>::iterator it;
  
  BUGPROG (pos != 0,
           false,
           this,
           "Il n'y a plus rien à rétablir.\n")
  
  insertion = false;
  
  it = liste.end ();
  NCALL (pos, --it;);
  undoData = *it;
  
  for (std::function <bool ()> f : undoData->repete)
  {
    BUGCRIT (f (),
             false,
             this,
             "Echec lors de l'opération.\n"
               "Le projet est très probablement corrompu.\n")
  }
  
  --pos;
  
  insertion = true;

  // Utile si le paramètre memory est changé alors que pos n'est pas nul.
  if (liste.size () - pos > memory)
  {
    size_t iend = liste.size () - pos - memory;

    NCALL (iend,
           delete *liste.begin ();
           liste.pop_front ();)
  }
  
  if (count == 0)
  {
    notify (EEvent::UNDO_NB, nullptr);
    notify (EEvent::REDO_NB, nullptr);
  }
  
  return true;
}

bool CHK
UndoManager::redoN (uint32_t nb)
{
  // Pour éviter l'émission des signaux à chaque émission de redo ().
  ++count;
  NCALL (nb, BUGCONT (redo (), false, this))
  --count;

  notify (EEvent::UNDO_NB, nullptr);
  notify (EEvent::REDO_NB, nullptr);

  return true;
}

size_t
UndoManager::redoNb () const
{
  return pos;
}

const std::string *
UndoManager::redoDesc (size_t n) const
{
  UndoData                             * undoData;
  std::list <UndoData *>::const_iterator it;

  BUGPROG (n <= pos,
           nullptr,
           UNDO_MANAGER_NULL,
           "Indice hors limite (%zu+%zu). La taille de la pile est de %zu.\n",
             n,
             pos,
             liste.size ())

  it = liste.end ();
  NCALL (pos - n, --it;);
  undoData = *it;

  return &undoData->description;
}

EUndoEtat
UndoManager::getEtat () const
{
  if ((insertion) && (count != 0))
  {
    return EUndoEtat::MODIF;
  }
  else
  {
    return EUndoEtat::NONE_OR_REVERT;
  }
}

bool CHK
UndoManager::ref ()
{
  BUGCRIT (count != UINT16_MAX,
           false,
           this,
           "Le programme est à ses limites.\n")
  
  if (!insertion)
  {
    return true;
  }
  
  if ((count == 0) && (pos != 0))
  {
    std::list <UndoData *>::iterator it = liste.end ();
    
    NCALL (pos, --it;);
    
    for (; it != liste.end (); )
    {
      delete *it;
      liste.erase (it++);
    }
    
    pos = 0;
  }
  
  if (count == 0)
  {
    tmpListe = new UndoData ();
  }
  
  ++count;
  
  return true;
}

bool CHK
UndoManager::unref ()
{
  if (!insertion)
  {
    return true;
  }
  
  BUGPROG (pos == 0,
           false,
           this,
           "La position en cours de la pile devrait être au sommet.\n")
  BUGPROG (count != 0,
           false,
           this,
           "Impossible d'appeler unref alors que le compteur vaut 0.\n")
  
  --count;
  
  if (count == 0)
  {
    time (&tmpListe->heure);
    liste.push_back (tmpListe);
    tmpListe = nullptr;

    if (liste.size () > memory)
    {
      size_t iend = liste.size () - memory - 1;

      NCALL (iend,
             delete *liste.begin ();
             liste.pop_front ();)
    }
  }
  
  return true;
}

bool CHK
UndoManager::undoToXML (xmlNodePtr root)
{
  std::unique_ptr <xmlNode, void (*)(xmlNodePtr)> node (
                 xmlNewNode (nullptr, BAD_CAST2 ("UndoManager")), xmlFreeNode);
  
  BUGCRIT (node.get () != nullptr,
           false,
           this,
           "Erreur d'allocation mémoire.\n")
  
  for (UndoData * data : liste)
  {
    std::unique_ptr <xmlNode, void (*)(xmlNodePtr)> node0 (
                                      xmlNewNode (nullptr, BAD_CAST2 ("Bloc")),
                                      xmlFreeNode);
    
    BUGCRIT (node0.get () != nullptr,
             false,
             this,
             "Erreur d'allocation mémoire.\n")
    
    BUGCRIT (xmlSetProp (
               node0.get (),
               BAD_CAST2 ("Heure"),
               BAD_CAST2 (std::to_string (data->heure).c_str ())) != nullptr,
             false,
             this,
             "Problème depuis la librairie : %s\n", "xml2")
    
    BUGCRIT (xmlSetProp (node0.get (),
                         BAD_CAST2 ("Description"),
                         BAD_CAST2 (data->description.c_str ())) != nullptr,
             false,
             this,
             "Problème depuis la librairie : %s\n", "xml2")
    
    for (std::function <bool (xmlNodePtr)> f : data->sauve)
    {
      BUGCRIT (f (node0.get ()),
               false,
               this,
               "Erreur lors de la génération du fichier XML.\n")
    }
    
    BUGCRIT (xmlAddChild (node.get (), node0.get ()) != nullptr,
             false,
             this,
             "Erreur lors de la génération du fichier XML.\n")
    
    node0.release ();
  }
  
  BUGCRIT (xmlAddChild (root, node.get ()) != nullptr,
           false,
           this,
           "Erreur lors de la génération du fichier XML.\n")

  node.release ();
  
  return true;
}

void
UndoManager::rollback ()
{
  count = 0;
  
  if (tmpListe == nullptr)
  {
    return;
  }
  
  insertion = false;
  
  for (std::function <bool ()> f : tmpListe->annule)
  {
    BUGCRIT (f (),
             ,
             UNDO_MANAGER_NULL,
             "Impossible de faire marche arrière suite à l'erreur détectée.\n"
               "Le projet est très probablement corrompu.\n")
  }
  
  delete tmpListe;
  tmpListe = nullptr;
  
  insertion = true;
  
  return;
}

bool CHK
UndoManager::getInsertion () const
{
  return insertion;
}

void
UndoManager::setInsertion (bool insert)
{
  insertion = insert;
}

void
UndoManager::setMemory (size_t taille)
{
  memory = taille;

  if (liste.size () - pos > taille)
  {
    size_t iend = liste.size () - pos - taille;

    NCALL (iend,
           delete *liste.begin ();
           liste.pop_front ();)
  }
}

void
UndoManager::setSauveDesc (uint16_t val)
{
  sauveDesc = val;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
