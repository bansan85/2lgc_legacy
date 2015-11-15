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

#include "INorme.hpp"

INorme::INorme (ENorme                              type_,
                uint32_t                            variante_,
                uint32_t                            options_,
                std::shared_ptr <const std::string> nom_) :
  id (0xFFFFFFFF),
  type (type_),
  variante (variante_),
  options (options_),
  nom (nom_)
{
}

INorme::~INorme ()
{
}

ENorme
INorme::getType () const
{
  return type;
}

uint32_t
INorme::getVariante () const
{
  return variante;
}

uint32_t
INorme::getOptions () const
{
  return options;
}

const std::shared_ptr <const std::string> &
INorme::getNom () const
{
  return nom;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
