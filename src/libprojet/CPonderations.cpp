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

#include "CPonderations.hpp"

CPonderations::CPonderations (bool    eluequ,
                              uint8_t elugeostr,
                              bool    eluacc,
                              bool    form6_10) :
  elu_geo_str_methode (elugeostr)
  , elu_equ_methode (eluequ & 1)
  , elu_acc_psi (eluacc & 1)
  , form_6_10 (form6_10 & 1)
  , elu_equ ()
  , elu_str ()
  , elu_geo ()
  , elu_fat ()
  , elu_acc ()
  , elu_sis ()
  , els_car ()
  , els_freq ()
  , els_perm ()
{
}

CPonderations::~CPonderations ()
{
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
