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

#include "MErreurs.hh"
#include "CUndoManager.hpp"
#include "IUndoable.hpp"


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
  modif (true)
{
}


/**
 * \brief Duplication d'une classe CUndoManager.
 * \param other (in) La classe à dupliquer.
 */
CUndoManager::CUndoManager (const CUndoManager & other) = delete;


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
  for_each (this->liste.begin (),
            this->liste.end (),
            std::default_delete <CUndoData> ());
  
  delete (this->tmpListe);
}


/**
 * \brief Ajoute une modification à la liste.
 * \param annule (in) La fonction à lancer pour annuler la modification.
 * \param repete (in) La fonction à lancer pour répéter la modification.
 */
bool
CUndoManager::push (std::function <bool ()>           annule,
                    std::function <bool ()>           repete,
                    std::function <void ()>           suppr,
                    std::function <void (xmlNodePtr)> sauve)
{
  if (modif)
  {
    BUGPARAMCRIT (tmpListe, "%p", tmpListe, false);
    
    tmpListe->annule.push_back (annule);
    tmpListe->repete.push_back (repete);
    if (suppr != NULL)
      tmpListe->suppr.push_back (suppr);
    if (sauve != NULL)
      tmpListe->sauve.push_back (sauve);
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
  
  BUG (liste.size () != pos,
       false,
       (gettext ("Il n'y a plus rien à annuler.\n")); )
  
  modif = false;
  
  it = liste.end ();
  std::advance (it, -static_cast<ssize_t>(pos) - 1);
  undoData = *it;
  
  for (std::function <void ()> f: undoData->annule)
  {
    f ();
  }
  
  ++pos;
  
  modif = true;
  
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
  
  BUG (pos != 0,
       false,
       (gettext ("Il n'y a plus rien à rétablir.\n")); )
  
  modif = false;
  
  it = liste.end ();
  std::advance (it, -static_cast<ssize_t>(pos));
  undoData = *it;
  
  for (std::function <void ()> f: undoData->repete)
  {
    f ();
  }
  
  --pos;
  
  modif = true;
  
  return true;
}


/**
 * \brief Renvoie l'état du gestionnaire des annulations.
 */
EUndoEtat
CUndoManager::getEtat ()
{
  if (count == 0)
  {
    return UNDO_NONE;
  }
  else if (modif)
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
           (gettext ("Le programme est arrivé au boût de ces limites. Contactez le concepteur de la librairie ou réduisez la complexité des modifications.\n")); )
  
  if (!modif)
    return true;
  
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
  if (!modif)
    return true;
  
  BUGCRIT (count != 0,
           false,
           (gettext ("Impossible d'appeler unref alors que le compteur vaut 0.\n")); )
  
  --count;
  
  if (count == 0)
  {
    liste.push_back (tmpListe);
    tmpListe = NULL;
  }
  
  return true;
}


/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
