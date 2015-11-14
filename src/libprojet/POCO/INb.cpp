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
#include "nombre/Calcul.hpp"
#include "nombre/Utilisateur.hpp"

POCO::INb::INb ()
{
}

POCO::INb *
POCO::INb::newINb (INb * nb)
{
  POCO::nombre::Calcul * nbCalc = dynamic_cast <POCO::nombre::Calcul *> (nb);
  if (nbCalc != nullptr)
  {
    return new POCO::nombre::Calcul (*nbCalc);
  }

  POCO::nombre::Utilisateur * nbUser;
  nbUser = dynamic_cast <POCO::nombre::Utilisateur *> (nb);
  if (nbUser != nullptr)
  {
    return new POCO::nombre::Utilisateur (*nbUser);
  }

  return nullptr;
}

/**
 * \brief Destructeur d'une classe INb.
 */
POCO::INb::~INb ()
{
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
