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

#include "CUndoManager.hpp"
#include "IUndoable.hpp"


/**
 * \brief Initialise le système de gestion de l'historique et de la gestion des
 *        annuler / répéter.
 */
CUndoManager::CUndoManager () :
  liste (),
  pos (0),
  ref (0),
  tmp_liste (NULL)
{
}


/**
 * \brief Duplication d'une classe CUndoManager.
 * \param other (in) La classe à dupliquer.
 */
CUndoManager::CUndoManager (const CUndoManager & other) :
  // TODO
  liste (),
  pos (0),
  ref (0),
  tmp_liste (NULL)
{
}


/**
 * \brief Assignment operator de CUndoManager.
 * \param other (in) La classe à dupliquer.
 */
CUndoManager &
CUndoManager::operator = (const CUndoManager & other)
{
  this->pos = 0;
  this->ref = 0;
  this->tmp_liste = NULL;
  
  return *this;
}


/**
 * \brief Libère une liste de IUndoable *. Le pointeur envoyé est également
 *        "delete".
 * \param liste Liste à supprimer.
 */
static void deleteListCUndoData (std::list <CUndoData *> * liste)
{
  if (liste == NULL)
  {
    return;
  }
  
  for_each (liste->begin (),
            liste->end (),
            std::default_delete <CUndoData> ());
  
  delete liste;
}


/**
 * \brief Libère l'historique du projet.
 */
CUndoManager::~CUndoManager ()
{
  for_each (this->liste.begin (),
            this->liste.end (),
            deleteListCUndoData);
  
  deleteListCUndoData (this->tmp_liste);
}


/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
