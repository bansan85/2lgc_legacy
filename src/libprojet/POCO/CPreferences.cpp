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

#include "CPreferences.hpp"

POCO::CPreferences::CPreferences () :
  decimales
  (
    {
      {
        3, // U_
        3, 3, 3, 3, // U_MM
        3, 3, 3, 3, // U_MM2
        3, 3, 3, 3, // U_MM3
        4, 4, 4, 4 // U_MM4
      }
    }
  )
{
}

POCO::CPreferences::~CPreferences ()
{
}

std::array <uint8_t, static_cast <std::size_t> (EUnite::LAST)> &
POCO::CPreferences::getDecimales () const
{
  return const_cast <std::array <uint8_t,
                      static_cast <std::size_t> (EUnite::LAST)> &> (decimales);
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
