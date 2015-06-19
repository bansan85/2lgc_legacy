#ifndef CBARREINFOEF__HPP
#define CBARREINFOEF__HPP

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

#include <cholmod.h>

struct CBarreInfoEF
{
  /// Matrice de rigidite locale
  cholmod_sparse * m_rig_loc;
  /// Matrice de rotation de la barre.
  cholmod_sparse * m_rot;
  /// Matrice de rotation transposée de la barre.
  cholmod_sparse * m_rot_t;
  /// Paramètre de souplesse a de la poutre selon l'axe y.
  double ay;
  /// Paramètre de souplesse b de la poutre selon l'axe y.
  double by;
  /// Paramètre de souplesse c de la poutre selon l'axe y.
  double cy;
  /// Paramètre de souplesse a de la poutre selon l'axe z.
  double az;
  /// Paramètre de souplesse b de la poutre selon l'axe z.
  double bz;
  /// Paramètre de souplesse c de la poutre selon l'axe z.
  double cz;
  /// Inverse de la raideur au début du tronçon en rx.
  double kAx;
  /// Inverse de la raideur au début du tronçon en ry.
  double kAy;
  /// Inverse de la raideur au début du tronçon en rz.
  double kAz;
  /// Inverse de la raideur à la fin du tronçon en rx.
  double kBx;
  /// Inverse de la raideur à la fin du tronçon en ry.
  double kBy;
  /// Inverse de la raideur à la fin du tronçon en rz.
  double kBz;
  /**
   * \brief Constructeur d'une classe CBarreInfoEF.
   */
  CBarreInfoEF ();
  /**
   * \brief Duplication d'une classe CBarreInfoEF.
   * \param other (in) La classe à dupliquer.
   */
  CBarreInfoEF (const CBarreInfoEF & other) = delete;
  /**
   * \brief Duplication d'une classe CBarreInfoEF.
   * \param other (in) La classe à dupliquer.
   * \return CBarreInfoEF &
   */
  CBarreInfoEF & operator = (const CBarreInfoEF & other) = delete;
  /**
   * \brief Destructeur d'une classe CBarreInfoEF.
   */
  virtual ~CBarreInfoEF ();
};

#endif
