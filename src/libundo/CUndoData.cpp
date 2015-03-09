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

#include "CUndoData.hpp"


/**
 * \brief Constructeur d'une classe CUndoData.
 */
CUndoData::CUndoData () :
  annule (),
  repete (),
  suppr (),
  sauve (),
  heure (0),
  description ()
{
}


/*
 * \brief Assignment operator de CUndoData. Non copiable.
 * \param other (in) La classe à dupliquer.
 */
//CUndoData &
//CUndoData::operator = (const CUndoData & other) = delete;


/**
 * \brief Destructeur d'une classe CUndoData.
 */
CUndoData::~CUndoData ()
{
  for (std::function <void ()> f : suppr)
  {
    f ();
  }
}


/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
