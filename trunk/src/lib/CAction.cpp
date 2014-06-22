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

#include "CAction.hpp"
#include "CNbUser.hpp"


/**
 * \brief Renvoie la description de l'annulation / répétition.
 */
std::string
CAction::undoToString ()
{
  // TODO
  return "";
}


/**
 * \brief Constructeur d'une classe CAction. Par défaut, les coefficients psi
 *        sont défini à NAN. Les valeurs sont automatiquement déterminés lors
 *        de l'insertion dans le projet en fonction de la norme du projet.
 * \param nom (in) Le nom de l'action.
 * \param type (in) Le type d'action, cf. _1990_action_bat_txt_type.
 * \param charges (in) Les charges à classer dans l'action.
 * \param undo (in) Le gestionnaire des modifications.
 */
CAction::CAction (std::string             nom_,
                  uint8_t                 type_,
                  std::list <ICharge *> & charges_,
                  CUndoManager          * undo_) :
  IActionGroupe (nom_, undo_)
  , type (type_)
  , action_predominante (false)
  , charges (charges_)
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
  , charges (other.charges)
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
}


/**
 * \brief Assignment operator de CAction.
 * \param other (in) La classe à dupliquer.
 */
CAction &
CAction::operator = (const CAction & other)
{
  // TODO
  
  return *this;
}


/**
 * \brief Libère une classe CAction.
 */
CAction::~CAction ()
{
  for_each (this->charges.begin (),
            this->charges.end (),
            std::default_delete <ICharge> ());
  
  delete this->psi0;
  delete this->psi1;
  delete this->psi2;
}


/**
 * \brief Annule/Rétabli la modification. L'objet doit être l'ancienne valeur
 *        et l'argument est la nouvelle.
 * \param apres (in) Nouvelle valeur.
 */
bool
CAction::change (IUndoable * apres)
{
  // TODO
  return true;
}


/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
