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
#include "MErreurs.hpp"
#include "SString.hpp"

UndoManager::UndoManager () :
  ISujet (),
  undoDataFort (),
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
  for_each (undoDataFort.begin (),
            undoDataFort.end (),
            std::default_delete <POCO::UndoData> ());
  
  delete (tmpListe);
}

bool
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
  
  BUGCONT (tmpListe->addAnnuler (annule), false, this)
  BUGCONT (tmpListe->addRepeter (repete), false, this)
  if (suppr != nullptr)
  {
    BUGCONT (tmpListe->addSupprimer (suppr), false, this)
  }
  if (sauve != nullptr)
  {
    BUGCONT (tmpListe->addToXML (sauve), false, this)
  }
  
  // Seules les modifications du sauveDesc-ième ordre sont mémorisées dans la
  // description.
  if (count <= sauveDesc)
  {
    BUGCONT (tmpListe->appendDescription(description), false, this)
  }
  
  return true;
}

bool
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
    BUGCONT (tmpListe->addSupprimer (suppr), false, this)
  }
  
  return true;
}

bool
UndoManager::undo ()
{
  POCO::UndoData                       * undoData;
  std::list <POCO::UndoData *>::iterator it;
  
  BUGPROG (undoDataFort.size () > pos,
           false,
           this,
           "Il n'y a plus rien à annuler.\n")
  
  insertion = false;
  
  it = undoDataFort.end ();
  NCALL (pos + 1, --it;);
  undoData = *it;
  
  BUGCRIT (undoData->execAnnuler(),
           false,
           this,
           "Impossible d'annuler l'opération");
  
  ++pos;
  
  insertion = true;

  if (count == 0)
  {
    notify (EEvent::UNDO_NB, nullptr);
    notify (EEvent::REDO_NB, nullptr);
  }
  
  return true;
}

bool
UndoManager::undoN (uint32_t nb)
{
  if (nb == 0)
  {
    return true;
  }

  // Pour éviter l'émission des signaux à chaque émission de undo ().
  ++count;
  NCALL (nb, BUGCONT (undo (), false, UNDO_MANAGER_NULL))
  --count;

  notify (EEvent::UNDO_NB, nullptr);
  notify (EEvent::REDO_NB, nullptr);

  return true;
}

size_t
UndoManager::undoNb () const
{
  return undoDataFort.size () - pos;
}

const std::string *
UndoManager::undoDesc (size_t n) const
{
  POCO::UndoData                             * undoData;
  std::list <POCO::UndoData *>::const_iterator it;

  BUGPROG (n + pos <= undoDataFort.size (),
           nullptr,
           UNDO_MANAGER_NULL,
           "Indice hors limite (%zu+%zu). La taille de la pile est de %zu.\n",
             n,
             pos,
             undoDataFort.size ())

  it = undoDataFort.end ();
  NCALL (n + pos + 1, --it;);
  undoData = *it;

  return &undoData->getDescription ();
}

bool
UndoManager::redo ()
{
  POCO::UndoData                       * undoData;
  std::list <POCO::UndoData *>::iterator it;
  
  BUGPROG (pos != 0,
           false,
           this,
           "Il n'y a plus rien à rétablir.\n")
  
  insertion = false;
  
  it = undoDataFort.end ();
  NCALL (pos, --it;);
  undoData = *it;
  
  BUGCRIT (undoData->execRepeter (),
           false,
           this,
           "Impossible d'annuler l'opération");
  
  
  --pos;
  
  insertion = true;

  // Utile si le paramètre memory est changé alors que pos n'est pas nul.
  if (undoDataFort.size () - pos > memory)
  {
    size_t iend = undoDataFort.size () - pos - memory;

    NCALL (iend,
           delete *undoDataFort.begin ();
           undoDataFort.pop_front ();)
  }
  
  if (count == 0)
  {
    notify (EEvent::UNDO_NB, nullptr);
    notify (EEvent::REDO_NB, nullptr);
  }
  
  return true;
}

bool
UndoManager::redoN (uint32_t nb)
{
  if (nb == 0)
  {
    return true;
  }

  // Pour éviter l'émission des signaux à chaque émission de redo ().
  ++count;
  NCALL (nb, BUGCONT (redo (), false, UNDO_MANAGER_NULL))
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
  POCO::UndoData                             * undoData;
  std::list <POCO::UndoData *>::const_iterator it;

  BUGPROG (n <= pos,
           nullptr,
           UNDO_MANAGER_NULL,
           "Indice hors limite (%zu+%zu). La taille de la pile est de %zu.\n",
             n,
             pos,
             undoDataFort.size ())

  it = undoDataFort.end ();
  NCALL (pos - n, --it;);
  undoData = *it;

  return &undoData->getDescription ();
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

bool
UndoManager::ref (bool undoable)
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
    std::list <POCO::UndoData *>::iterator it = undoDataFort.end ();
    
    NCALL (pos, --it;);
    
    for (; it != undoDataFort.end (); )
    {
      delete *it;
      undoDataFort.erase (it++);
    }
    
    pos = 0;
  }
  
  if (count == 0)
  {
    tmpListe = new POCO::UndoData (undoable);
  }
  else
  {
    BUGPROG (undoable,
             false,
             this,
             "Impossible de définir undoable à false sauf au début de la modification.\n")
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
    BUGCONT (tmpListe->setHeure (), false, this)
    undoDataFort.push_back (tmpListe);
    tmpListe = nullptr;

    if (undoDataFort.size () > memory)
    {
      size_t iend = undoDataFort.size () - memory - 1;

      NCALL (iend,
             delete *undoDataFort.begin ();
             undoDataFort.pop_front ();)
    }
  }
  
  return true;
}

bool
UndoManager::undoToXML (xmlNodePtr root) const
{
  std::unique_ptr <xmlNode, void (*)(xmlNodePtr)> node (
                 xmlNewNode (nullptr, BAD_CAST2 ("undoManager")), xmlFreeNode);
  
  BUGCRIT (node.get () != nullptr,
           false,
           UNDO_MANAGER_NULL,
           "Erreur d'allocation mémoire.\n")
  
  for (POCO::UndoData * data : undoDataFort)
  {
    std::unique_ptr <xmlNode, void (*)(xmlNodePtr)> node0 (
                                      xmlNewNode (nullptr, BAD_CAST2 ("bloc")),
                                      xmlFreeNode);
    
    BUGCRIT (node0.get () != nullptr,
             false,
             UNDO_MANAGER_NULL,
             "Erreur d'allocation mémoire.\n")
    
    BUGCRIT (xmlSetProp (
               node0.get (),
               BAD_CAST2 ("heure"),
               BAD_CAST2 (std::to_string (data->getHeure ()).c_str ()))
                                                                    != nullptr,
             false,
             UNDO_MANAGER_NULL,
             "Problème depuis la librairie : %s\n", "xml2")
    
    BUGCRIT (xmlSetProp (node0.get (),
                         BAD_CAST2 ("description"),
                         BAD_CAST2 (
                           data->getDescription ().c_str ())) != nullptr,
             false,
             UNDO_MANAGER_NULL,
             "Problème depuis la librairie : %s\n", "xml2")
    
    BUGCONT (data->execToXML(node0.get ()), false, UNDO_MANAGER_NULL)
    
    BUGCRIT (xmlAddChild (node.get (), node0.get ()) != nullptr,
             false,
             UNDO_MANAGER_NULL,
             "Erreur lors de la génération du fichier XML.\n")
    
    node0.release ();
  }
  
  BUGCRIT (xmlAddChild (root, node.get ()) != nullptr,
           false,
           UNDO_MANAGER_NULL,
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
  
  BUGCRIT (tmpListe->execAnnuler(),
           ,
           UNDO_MANAGER_NULL,
           "Impossible de faire marche arrière suite à l'erreur détectée.\n"
             "Le projet est très probablement corrompu.\n");
  
  delete tmpListe;
  tmpListe = nullptr;
  
  insertion = true;
  
  return;
}

bool
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

  if (undoDataFort.size () - pos > taille)
  {
    size_t iend = undoDataFort.size () - pos - taille;

    NCALL (iend,
           delete *undoDataFort.begin ();
           undoDataFort.pop_front ();)
  }
}

void
UndoManager::setSauveDesc (uint16_t val)
{
  sauveDesc = val;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
