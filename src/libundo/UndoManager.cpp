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

#include "config.hpp"

#include <algorithm>
#include <memory>
#include <iostream>
#include <locale>
#include <sys/types.h>

#include "UndoManager.hpp"
#include "IUndoable.hpp"
#include "MErreurs.hpp"
#include "SString.hpp"

UndoManager::UndoManager () :
  liste (),
  pos (0),
  count (0),
  tmpListe (nullptr),
  insertion (true)
{
}

UndoManager::~UndoManager ()
{
  for_each (liste.begin (), liste.end (), std::default_delete <UndoData> ());
  
  delete (tmpListe);
}

bool
UndoManager::push (std::function <bool ()>           annule,
                   std::function <bool ()>           repete,
                   std::function <void ()>           suppr,
                   std::function <bool (xmlNodePtr)> sauve,
                   const std::string                 description)
{
  if (!insertion)
  {
    return true;
  }
  
  BUGPROG (count != 0,
           false,
           this,
           gettext ("Impossible d'ajouter un évènement au gestionnaire d'annulation si aucune modification n'est en cours (nécessité d'appeler la fonction ref).\n"))
  BUGPARAM (static_cast <void*> (tmpListe), "%p", tmpListe, false, this)
  
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
  
  // Seules les modifications de premier ordre sont mémorisées dans la
  // description.
  if (count == 1)
  {
    if (!tmpListe->description.empty ())
    {
      tmpListe->description.append ("\n");
    }
    tmpListe->description.assign (description);
  }
  
  return true;
}

bool
UndoManager::pushSuppr (std::function <void ()> suppr)
{
  if (!insertion)
  {
    return true;
  }
  
  BUGPROG (count != 0,
           false,
           this,
           gettext ("Impossible d'ajouter un évènement au gestionnaire d'annulation si aucune modification n'est en cours (nécessité d'appeler la fonction ref).\n"))
  
  if (suppr != NULL)
  {
    tmpListe->suppr.push_back (suppr);
  }
  
  return true;
}

bool
UndoManager::undo ()
{
  UndoData * undoData;
  std::list <UndoData *>::iterator it;
  
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
             gettext ("Échec lors de l'opération.\nLe projet est très probablement corrompu.\n"))
  }
  
  ++pos;
  
  insertion = true;
  
  return true;
}

bool
UndoManager::undoN (uint32_t nb)
{
  for (uint32_t i = 1; i <= nb; i++)
  {
    BUGCRIT (undo (),
             false,
             this,
             gettext ("Echec lors de l'opération.\nLe projet est très probablement corrompu.\n"))
  }
  return true;
}

bool
UndoManager::redo ()
{
  UndoData * undoData;
  std::list <UndoData *>::iterator it;
  
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

bool
UndoManager::redoN (uint32_t nb)
{
  for (uint32_t i = 1; i <= nb; i++)
  {
    BUGCRIT (redo (),
             false,
             this,
             gettext ("Echec lors de l'opération.\nLe projet est très probablement corrompu.\n"))
  }
  return true;
}

EUndoEtat
UndoManager::getEtat () const
{
  if ((insertion) && (count != 0))
  {
    return EUndoEtat::UNDO_MODIF;
  }
  else
  {
    return EUndoEtat::UNDO_NONE_OR_REVERT;
  }
}

bool
UndoManager::ref ()
{
  BUGCRIT (count != UINT16_MAX,
           false,
           this,
           gettext ("Le programme est à ses limites.\n"))
  
  if (!insertion)
  {
    return true;
  }
  
  if ((count == 0) && (pos != 0))
  {
    std::list <UndoData *>::iterator it = liste.end ();
    
    std::advance (it, -static_cast <ssize_t> (pos));
    
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

bool
UndoManager::unref ()
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
    tmpListe = nullptr;
  }
  
  return true;
}

bool
UndoManager::undoToXML (xmlNodePtr root)
{
  std::unique_ptr <xmlNode, void (*)(xmlNodePtr)> node (
                 xmlNewNode (nullptr, BAD_CAST2 ("UndoManager")), xmlFreeNode);
  
  BUGCRIT (node.get (),
           false,
           this,
           gettext ("Erreur d'allocation mémoire.\n"))
  
  for (UndoData * data : liste)
  {
    std::unique_ptr <xmlNode, void (*)(xmlNodePtr)> node0 (
                                               xmlNewNode (nullptr,
                                                           BAD_CAST2 ("Bloc")),
                                               xmlFreeNode);
    
    BUGCRIT (node0.get (),
             false,
             this,
             gettext ("Erreur d'allocation mémoire.\n"))
    
    BUGCRIT (xmlSetProp (node0.get (),
                         BAD_CAST2 ("Heure"),
                         BAD_CAST2 (std::to_string (data->heure).c_str ())),
             false,
             this,
             gettext ("Problème depuis la librairie : %s\n"), "xml2")
    
    BUGCRIT (xmlSetProp (node0.get (),
                         BAD_CAST2 ("Description"),
                         BAD_CAST2 (data->description.c_str ())),
             false,
             this,
             gettext ("Problème depuis la librairie : %s\n"), "xml2")
    
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

void
UndoManager::rollback ()
{
  count = 0;
  
  if (tmpListe == NULL)
  {
    return;
  }
  
  insertion = false;
  
  for (std::function <bool ()> f : tmpListe->annule)
  {
    BUGCRIT (f (),
             ,
             static_cast <UndoManager *> (nullptr),
             gettext ("Impossible de faire marche arrière suite à l'erreur détectée.\nLe projet est très probablement corrompu.\n"))
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

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
