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

/**
 * \brief Constructeur d'une classe CPonderations.
 * \param eluequ (in) ELU_EQU : méthode 1 si 0, méthode 2 si 1.
 * \param elugeostr (in) ELU_GEO/STR : si 0 approche 1, 1 : approche 2 et
 *                       2 : approche 3.
 * \param eluacc (in) ELU_ACC : 0 si psi1,1 et 1 si psi2,1.
 * \param form6_10 (in) Si 0, utilisation des formules 6.10a et b.
 *                       Si 1 alors formule 6.10.
 */
CPonderations::CPonderations (uint8_t eluequ,
                              uint8_t elugeostr,
                              uint8_t eluacc,
                              uint8_t form6_10) :
  elu_equ_methode (eluequ & 1)
  , elu_geo_str_methode (elugeostr & 3)
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
#ifdef ENABLE_GTK
  , list_el_desc (NULL)
  // TODO
#endif
{
#ifdef ENABLE_GTK
  std::cout << __func__ ;
#endif
}


/**
 * \brief Duplication d'une classe CPonderations.
 * \param other (in) La classe à dupliquer.
 */
CPonderations::CPonderations (const CPonderations & other) = delete;


/**
 * \brief Assignment operator de CPonderations.
 * \param other (in) La classe à dupliquer.
 */
CPonderations &
CPonderations::operator = (const CPonderations & other) = delete;


/**
 * \brief Destructeur d'une classe CPonderations.
 */
CPonderations::~CPonderations ()
{
}


/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
