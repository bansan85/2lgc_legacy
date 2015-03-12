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

#include "INb.hpp"
#include "NbCalcul.hpp"
#include "NbUser.hpp"

/**
 * \brief Constructeur d'une classe INb.
 * \param nb Le nombre à dupliquer.
 */
INb *
INb::newINb (INb * nb)
{
  if (dynamic_cast <NbCalcul *> (nb) != NULL)
  {
    return new NbCalcul (*dynamic_cast <NbCalcul *> (nb));
  }
  else if (dynamic_cast <NbUser *> (nb) != NULL)
  {
    return new NbUser (*dynamic_cast <NbUser *> (nb));
  }
  else
  {
    return NULL;
  }
}

/**
 * \brief Destructeur d'une classe INb.
 */
INb::~INb ()
{
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
