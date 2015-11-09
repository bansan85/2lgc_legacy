#ifndef POCO_CALCUL_CFONCTION__HPP
#define POCO_CALCUL_CFONCTION__HPP

/*
2lgc_code : calcul de résistance des matériaux selon les normes Eurocodes
Copyright (C) 2011-2015

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

#include <list>
#include "POCO/calcul/CTroncon.hpp"

namespace POCO
{
  namespace calcul
  {
    /**
     * \brief Une fonction décrit une courbe sous forme d'une liste de troncons. Les fonctions n'étant pas forcément continues le long de la barre (par exemple de part et d'une charge ponctuelle). Il est nécessaire de définir plusieurs tronçons avec pour chaque tronçon sa fonction.
     */
    class CFonction
    {
      // Associations
      // Attributes
      private :
        /// Tableau dynamique contenant les fonctions continues par tronçon.
        std::list <CTroncon *> t;
    };
  }
}

#endif
