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

#ifndef __MATH_HPP
#define __MATH_HPP

#include "config.hpp"

#include <cmath>

#define ERR_REL 1.E-14

inline bool
doublesAreEqual (double a, double b, double moyAbs, double maxRel)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-equal"
  if (a == b)
#pragma GCC diagnostic pop
  {
    return true;
  }

  double absError = fabs (a - b);
  a = std::fabs (a);
  b = std::fabs (b);
  
  return (absError <= moyAbs * std::sqrt (maxRel)) ||
         ((absError / (a > b ? a : b)) <= maxRel);
}

#endif

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
