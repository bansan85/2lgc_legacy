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


#include "CModele.hpp"

/**
 * \brief Initialise le modèle de calcul.
 */
CModele::CModele () :
  CModeleActions (),
  CModeleBarres (),
  CModeleRelachements (),
  CModeleMateriaux (),
  CModeleSections (),
  CModeleNoeuds (),
  CModeleAppuis (),
  niveaux_groupes ()
{
}


/**
 * \brief Duplication d'une classe CModele.
 * \param other (in) La classe à dupliquer.
 */
CModele::CModele (const CModele & other) = delete;


/**
 * \brief Assignment operator de CModele.
 * \param other (in) La classe à dupliquer.
 */
CModele &
CModele::operator = (const CModele & other) = delete;


/**
 * \brief Libère le modèle de calcul.
 */
CModele::~CModele ()
{
  // Ce n'est pas modèle qui libère la mémoire, c'est le gestionnaire
  // d'annulation.
}


/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
