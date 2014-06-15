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

#include "CBarreInfoEF.hpp"

/**
 * \brief Constructeur d'une classe CBarreInfoEF.
 */
CBarreInfoEF::CBarreInfoEF () :
  m_rig_loc (NULL),
  m_rot (NULL),
  m_rot_t (NULL),
  ay (NAN),
  by (NAN),
  cy (NAN),
  az (NAN),
  bz (NAN),
  cz (NAN),
  kAx (NAN),
  kAy (NAN),
  kAz (NAN),
  kBx (NAN),
  kBy (NAN),
  kBz (NAN)
{
}


/**
 * \brief Duplication d'une classe CBarreInfoEF.
 * \param other (in) La classe à dupliquer.
 */
CBarreInfoEF::CBarreInfoEF (const CBarreInfoEF & other) :
  // TODO
  m_rig_loc (NULL),
  m_rot (NULL),
  m_rot_t (NULL),
  ay (NAN),
  by (NAN),
  cy (NAN),
  az (NAN),
  bz (NAN),
  cz (NAN),
  kAx (NAN),
  kAy (NAN),
  kAz (NAN),
  kBx (NAN),
  kBy (NAN),
  kBz (NAN)
{
}


/**
 * \brief Assignment operator de CBarreInfoEF.
 * \param other (in) La classe à dupliquer.
 */
CCalculs &
CCalculs::operator = (const CCalculs & other)
{
  // TODO
  this->m_rig_loc = NULL;
  this->m_rot = NULL;
  this->m_rot_t = NULL;
  this->ay = NAN;
  this->by = NAN;
  this->cy = NAN;
  this->az = NAN;
  this->bz = NAN;
  this->cz = NAN;
  this->kAx = NAN;
  this->kAy = NAN;
  this->kAz = NAN;
  this->kBx = NAN;
  this->kBy = NAN;
  this->kBz = NAN;
  
  return *this;
}


/**
 * \brief Destructeur d'une classe CBarreInfoEF.
 */
CBarreInfoEF::~CBarreInfoEF ()
{
  // TODO
}


/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
