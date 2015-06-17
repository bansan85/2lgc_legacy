#ifndef EUNITE__HPP
#define EUNITE__HPP

/*
2lgc_code : calcul de résistance des matériaux selon les normes Eurocodes
Copyright (C) 2011

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

Fichier généré automatiquement avec dia2code 0.9.0.
 */

/// L'unité du nombre
enum class EUnite
{
  /// Sans unité.
  U_ = 0,
  /// Mètre.
  M,
  /// Décimètre.
  DM,
  /// Centimètre.
  CM,
  /// Millimètre.
  MM,
  /// Mètre carré.
  M2,
  /// Décimètre carré.
  DM2,
  /// Centimètre carré.
  CM2,
  /// Millimètre carré.
  MM2,
  /// Mètre cube.
  M3,
  /// Décimètre cube.
  DM3,
  /// Centimètre cube.
  CM3,
  /// Millimètre cube.
  MM3,
  /// Mètre ⁴.
  M4,
  /// Décimètre ⁴.
  DM4,
  /// Centimètre ⁴.
  CM4,
  /// Millimètre ⁴.
  MM4,
  LAST
};

#endif
