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
#include <algorithm>
#include <memory>
#include <iostream>

#include "CAction.hpp"
#include "CNbUser.hpp"


/**
 * \brief Constructeur d'une classe CAction. Par défaut, les coefficients psi
 *        sont défini à NAN. Les valeurs sont automatiquement déterminés lors
 *        de l'insertion dans le projet en fonction de la norme du projet.
 * \param nom_ (in) Le nom de l'action.
 * \param type_ (in) Le type d'action, cf. _1990_action_bat_txt_type.
 * \param undo_ (in) Le gestionnaire des modifications.
 */
CAction::CAction (std::string             nom_,
                  uint8_t                 type_,
                  CUndoManager          & undo_) :
  IActionGroupe (nom_, undo_)
  , type (type_)
  , action_predominante (false)
  , charges ()
  , psi0 (new CNbUser (NAN, U_))
  , psi1 (new CNbUser (NAN, U_))
  , psi2 (new CNbUser (NAN, U_))
  , deplacement (NULL)
  , forces (NULL)
  , efforts_noeuds (NULL)
  , efforts {{}, {}, {}, {}, {}, {}}
  , deformation {{}, {}, {}}
  , rotation {{}, {}, {}}
#ifdef ENABLE_GTK
  , Iter_fenetre_ac (NULL)
  , Iter_liste (NULL)
#endif
{
}


/**
 * \brief Duplication d'une classe CAction.
 * \param other (in) La classe à dupliquer.
 */
CAction::CAction (const CAction & other) :
  // TODO
  IActionGroupe (other)
  , type (other.type)
  , action_predominante (other.action_predominante)
  , charges ()
  , psi0 (other.psi0)
  , psi1 (other.psi1)
  , psi2 (other.psi2)
  , deplacement (NULL)
  , forces (NULL)
  , efforts_noeuds (NULL)
  , efforts {{}, {}, {}, {}, {}, {}}
  , deformation {{}, {}, {}}
  , rotation {{}, {}, {}}
#ifdef ENABLE_GTK
  , Iter_fenetre_ac (NULL)
  , Iter_liste (NULL)
#endif
{
  std::cout << __func__ ;
}


/**
 * \brief Assignment operator de CAction.
 * \param other (in) La classe à dupliquer.
 */
CAction &
CAction::operator = (const CAction & other)
{
  // TODO
  std::cout << __func__ ;
  
  return *this;
}


/**
 * \brief Libère une classe CAction.
 */
CAction::~CAction ()
{
  delete this->psi0;
  delete this->psi1;
  delete this->psi2;
}


/**
 * \brief Renvoie true si aucune charge n'est présente.
 */
bool
CAction::emptyCharges ()
{
  return this->charges.size () == 0;
}


/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
