#ifndef POCO_STRUCTURE_SECTION_PERSONNALISEE__HPP
#define POCO_STRUCTURE_SECTION_PERSONNALISEE__HPP

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

#include "POCO/structure/ISection.hpp"
#include "POCO/structure/CPoint.hpp"

namespace POCO
{
  namespace structure
  {
    namespace section
    {
      /**
       * \brief Défini une section personnalisée.
       */
      class Personnalisee : public POCO::structure::ISection
      {
        // Attributes
        private :
          /// Sa description (plus longue que son nom).
          std::string description;
          /// Inertie de torsion J en m⁴.
          INb * j;
          /// Inertie I selon l'axe y en m⁴.
          INb * iy;
          /// Inertie I selon l'axe z en m⁴.
          INb * iz;
          /// vy en m.
          INb * vy;
          /// vy' en m.
          INb * vyp;
          /// vz en m.
          INb * vz;
          /// vz' en m.
          INb * vzp;
          /// Surface en m².
          INb * s;
          /// Une forme est une liste de forme élémentaire. Chaque forme élémentaire est une liste de points. Cette forme est automatiquement refermée. Il n'est pas nécessaire que le premier point soit égal au dernier.
          std::list <std::list <CPoint *> *> forme;
        // Operations
        public :
          /**
           * \brief Constructeur d'une classe Personnalisee.
           * \param description (in) Sa description (plus longue que son nom).
           * \param j (in) Inertie de torsion J en m⁴.
           * \param iy (in) Inertie I selon l'axe y en m⁴.
           * \param iz (in) Inertie I selon l'axe z en m⁴.
           * \param vy (in) vy en m.
           * \param vyp (in) vy' en m.
           * \param vz (in) vz en m.
           * \param vzp (in) vz' en m.
           * \param s (in) Surface en m².
           * \param forme (in) Forme de la section.
           * \param undo (in) Le gestionnaire des modifications.
           */
          Personnalisee (std::string description, INb * j, INb * iy, INb * iz, INb * vy, INb * vyp, INb * vz, INb * vzp, INb * s, std::list <std::list <CPoint *> *> * forme, UndoManager & undo);
          /**
           * \brief Duplication d'une classe Personnalisee.
           * \param other (in) La classe à dupliquer.
           */
          Personnalisee (const Personnalisee & other);
          /**
           * \brief Assignment operator de Personnalisee.
           * \param other (in) La classe à dupliquer.
           * \return Personnalisee &
           */
          Personnalisee & operator = (const Personnalisee & other);
          /**
           * \brief Destructeur d'une classe Personnalisee.
           */
          virtual ~Personnalisee ();
          /**
           * \brief Renvoie la description longue de la section.
           * \return std::string const &
           */
          std::string const & getDescription () const;
          /**
           * \brief Défini la description longue de la section.
           * \param desc (in) La nouvelle description.
           * \return bool CHK
           */
          bool CHK setDescription (std::string desc);
          /**
           * \brief Renvoie l'inertie de torsion.
           * \return INb const &
           */
          INb const & getj () const;
          /**
           * \brief Défini l'inertie de torsion J.
           * \param val (in) La nouvelle valeur.
           * \return bool CHK
           */
          bool CHK setj (INb * val);
          /**
           * \brief Renvoie l'inertie I selon l'axe y.
           * \return INb const &
           */
          INb const & getiy () const;
          /**
           * \brief Défini l'inertie I selon l'axe y.
           * \param val (in) La nouvelle valeur.
           * \return bool CHK
           */
          bool CHK setiy (INb * val);
          /**
           * \brief Renvoie l'inertie I selon l'axe z.
           * \return INb const &
           */
          INb const & getiz () const;
          /**
           * \brief Défini l'inertie I selon l'axe z.
           * \param val (in) La nouvelle valeur.
           * \return bool CHK
           */
          bool CHK setiz (INb * val);
          /**
           * \brief Renvoie vy.
           * \return INb const &
           */
          INb const & getvy () const;
          /**
           * \brief Défini vy.
           * \param val (in) La nouvelle valeur.
           * \return bool CHK
           */
          bool CHK setvy (INb * val);
          /**
           * \brief Renvoie vy'.
           * \return INb const &
           */
          INb const & getvyp () const;
          /**
           * \brief Défini vy'.
           * \param val (in) La nouvelle valeur.
           * \return bool CHK
           */
          bool CHK setvyp (INb * val);
          /**
           * \brief Renvoie vz.
           * \return INb const &
           */
          INb const & getvz () const;
          /**
           * \brief Défini vz.
           * \param val (in) La nouvelle valeur.
           * \return bool CHK
           */
          bool CHK setvz (INb * val);
          /**
           * \brief Renvoie vz'.
           * \return INb const &
           */
          INb const & getvzp () const;
          /**
           * \brief Défini vz'.
           * \param val (in) La nouvelle valeur.
           * \return bool CHK
           */
          bool CHK setvzp (INb * val);
          /**
           * \brief Renvoie la surface.
           * \return INb const &
           */
          INb const & gets () const;
          /**
           * \brief Défini la surface.
           * \param val (in) La nouvelle valeur.
           * \return bool CHK
           */
          bool CHK sets (INb * val);
          /**
           * \brief Défini la forme de la section.
           * \param forme (in) La forme de la section.
           * \return bool CHK
           */
          bool CHK setForme (std::list <std::list <CPoint *> *> * forme);
      };
    }
  }
}

#endif
