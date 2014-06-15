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

/**
 * \brief Initialise les variables de calcul globales et les résultats.
 */
CCalculs::CCalculs () :
  c (new cholmod_common),
  n_part {{}, {}, {}, {}, {}, {}},
  n_comp {{}, {}, {}, {}, {}, {}},
  t_part (NULL),
  t_comp (NULL),
  t_part_en_cours (0),
  t_comp_en_cours (0),
  m_part (NULL),
  m_comp (NULL),
  numeric (NULL),
  ap (NULL),
  ai (NULL),
  ax (NULL),
  residu (NAN),
  info_EF (),
  rapport (),
  ponderations (1, 1, 1, 1)
{
  cholmod_start (c);
}


/**
 * \brief Duplication d'une classe CCalculs. Les résultats n'étant pas
 *        dupliqués, la variable cholmod_common (c) est simplement initialisée
 *        à zéro et les paramètres des pondérations est conservés.
 * \param other (in) La classe à dupliquer.
 */
CCalculs::CCalculs (const CCalculs & other) :
  c (new cholmod_common),
  n_part {{}, {}, {}, {}, {}, {}},
  n_comp {{}, {}, {}, {}, {}, {}},
  t_part (NULL),
  t_comp (NULL),
  t_part_en_cours (0),
  t_comp_en_cours (0),
  m_part (NULL),
  m_comp (NULL),
  numeric (NULL),
  ap (NULL),
  ai (NULL),
  ax (NULL),
  residu (NAN),
  info_EF (),
  rapport (),
  ponderations (other.ponderations)
{
  cholmod_start (c);
}


/**
 * \brief Assignment operator de CCalculs. Les résultats n'étant pas dupliqués,
 *        la variable cholmod_common (c) est simplement initialisée à zéro et
 *        les paramètres des pondérations est conservés.
 * \param other (in) La classe à dupliquer.
 */
CCalculs &
CCalculs::operator = (const CCalculs & other)
{
  this->c = new cholmod_common;
  cholmod_start (c);
  this->t_part = NULL;
  this->t_comp = NULL;
  this->t_part_en_cours = 0;
  this->t_comp_en_cours = 0;
  this->m_part = NULL;
  this->m_comp = NULL;
  this->numeric = NULL;
  this->ap = NULL;
  this->ai = NULL;
  this->ax = NULL;
  this->residu = NAN;
  this->ponderations = other.ponderations;
  
  return *this;
}


/**
 * \brief Libère les résultats.
 */
CCalculs::~CCalculs ()
{
  cholmod_finish (c);
  delete c;
  
  cholmod_free_triplet (&t_part, c);
  cholmod_free_triplet (&t_comp, c);
  
  cholmod_free_sparse (&m_comp, c);
  cholmod_free_sparse (&m_part, c);
  
  umfpack_di_free_numeric (&numeric);
  
  delete ap;
  delete ai;
  delete ax;
}


/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
