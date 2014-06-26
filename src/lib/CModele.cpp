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

#include <algorithm>
#include <memory>
#include <iostream>

#include "CModele.hpp"

/**
 * \brief Initialise le modèle de calcul.
 */
CModele::CModele () :
  appuis (),
  noeuds (),
  sections (),
  materiaux (),
  relachements (),
  barres (),
  actions (),
  niveaux_groupes ()
{
}


/**
 * \brief Duplication d'une classe CModele.
 * \param other (in) La classe à dupliquer.
 */
CModele::CModele (const CModele & other) :
  appuis (),
  noeuds (),
  sections (),
  materiaux (),
  relachements (),
  barres (),
  actions (),
  niveaux_groupes ()
{
  std::cout << __func__ ;
  //TODO : A FINIR.
}


/**
 * \brief Assignment operator de CModele.
 * \param other (in) La classe à dupliquer.
 */
CModele &
CModele::operator = (const CModele & other)
{
  std::cout << __func__ ;
  //TODO : A FINIR.
  return *this;
}


/**
 * \brief Libère le modèle de calcul.
 */
CModele::~CModele ()
{
  for_each (niveaux_groupes.begin (),
            niveaux_groupes.end (),
            std::default_delete <CNiveauGroupe> ());
  for_each (actions.begin (),
            actions.end (),
            std::default_delete <CAction> ());
  for_each (barres.begin (),
            barres.end (),
            std::default_delete <CBarre> ());
  for_each (relachements.begin (),
            relachements.end (),
            std::default_delete <CRelachement> ());
  for_each (materiaux.begin (),
            materiaux.end (),
            std::default_delete <IMateriau> ());
  for_each (sections.begin (),
            sections.end (),
            std::default_delete <ISection> ());
  for_each (noeuds.begin (),
            noeuds.end (),
            std::default_delete <INoeud> ());
  for_each (appuis.begin (),
            appuis.end (),
            std::default_delete <CAppui> ());
}


/**
 * \brief Ajout d'une action.
 * \param action (in) L'action à ajouter.
 */
bool
CModele::addAction (CAction * action)
{
  actions.push_back (action);
  
  return true;
}


/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
