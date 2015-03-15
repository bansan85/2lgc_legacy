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

#include <cmath>
#include <umfpack.h>

#include "CCalculs.hpp"

CCalculs::CCalculs () :
  c (new cholmod_common),
  n_part ({{{}, {}, {}, {}, {}, {}}}),
  n_comp ({{{}, {}, {}, {}, {}, {}}}),
  t_part (nullptr),
  t_comp (nullptr),
  t_part_en_cours (0),
  t_comp_en_cours (0),
  m_part (nullptr),
  m_comp (nullptr),
  numeric (nullptr),
  ap (nullptr),
  ai (nullptr),
  ax (nullptr),
  residu (NAN),
  rapport (),
  info_EF (),
  ponderations (1, 1, 1, 1)
{
  cholmod_start (c);
}

CCalculs::~CCalculs ()
{
  cholmod_free_triplet (&t_part, c);
  cholmod_free_triplet (&t_comp, c);
  
  cholmod_free_sparse (&m_comp, c);
  cholmod_free_sparse (&m_part, c);
  
  cholmod_finish (c);
  delete c;
  
  umfpack_di_free_numeric (&numeric);
  
  delete ap;
  delete ai;
  delete ax;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
