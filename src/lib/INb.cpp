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
#include "CNbCalcul.hpp"
#include "CNbUser.hpp"

/**
 * \brief Constructeur d'une classe INb.
 * \param nb Le nombre à dupliquer.
 */
INb *
INb::newINb (INb * nb)
{
  if (dynamic_cast <CNbCalcul *> (nb) != NULL)
  {
    return new CNbCalcul (*dynamic_cast <CNbCalcul *> (nb));
  }
  else if (dynamic_cast <CNbUser *> (nb) != NULL)
  {
    return new CNbUser (*dynamic_cast <CNbUser *> (nb));
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
