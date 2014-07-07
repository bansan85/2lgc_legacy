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

#include "IActionGroupe.hpp"


/**
 * \brief Constructeur d'une interface IActionGroupe.
 * \param nom_ (in) Le nom de l'action / groupe.
 * \param undo_ (in) Le gestionnaire des modifications.
 */
IActionGroupe::IActionGroupe (std::string    nom_,
                              CUndoManager & undo_) :
  IUndoable (undo_)
  , nom (nom_)
#ifdef ENABLE_GTK
  , Iter_fenetre_gr (NULL)
#endif
{
}


/**
 * \brief Destructeur d'une classe IActionGroupe.
 */
IActionGroupe::~IActionGroupe ()
{
}


/**
 * \brief Renvoie le nom.
 */
std::string const &
IActionGroupe::getNom () const
{
  return nom;
}


/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
