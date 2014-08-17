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

#include <iostream>

#include "CPreferences.hpp"

/**
 * \brief Constructeur d'une classe CPreferences.
 */
CPreferences::CPreferences () :
  decimales
  {
    3, // U_
    3, 3, 3, 3, // U_MM
    3, 3, 3, 3, // U_MM2
    3, 3, 3, 3, // U_MM3
    4, 4, 4, 4
  } // U_MM4
{
}


/**
 * \brief Duplication d'une classe CPreferences.
 * \param other (in) La classe à dupliquer.
 */
CPreferences::CPreferences (const CPreferences & other)
{
  std::copy (std::begin (other.decimales),
             std::end (other.decimales),
             std::begin (this->decimales));
}


/**
 * \brief Assignment operator de CPreferences.
 * \param other (in) La classe à dupliquer.
 */
CPreferences &
CPreferences::operator = (const CPreferences & other)
{
  std::copy (std::begin (other.decimales),
             std::end (other.decimales),
             std::begin (this->decimales));
  return *this;
}


/**
 * \brief Destructeur d'une classe CPreferences.
 */
CPreferences::~CPreferences ()
{
}


/**
 * \brief Renvoie la liste des décimales applicables aux unités.
 */
uint8_t *
CPreferences::getDecimales () const
{
  return const_cast <uint8_t *> (decimales);
}


/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
