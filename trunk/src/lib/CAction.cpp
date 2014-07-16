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
#include <locale>

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
CAction::CAction (std::string    nom_,
                  uint8_t        type_,
                  CUndoManager & undo_) :
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
CAction::CAction (const CAction & other) = delete;


/**
 * \brief Assignment operator de CAction.
 * \param other (in) La classe à dupliquer.
 */
CAction &
CAction::operator = (const CAction & other) = delete;


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


/**
 * \brief Renvoie le type de l'action sous forme de texte.
 */
std::string
CAction::getDescription ()
{
  switch (type)
  {
    case 0 :
    {
      return gettext ("Permanente");
    }
    case 1 :
    {
      return gettext ("Précontrainte");
    }
    case 2 :
    {
      return gettext ("Exploitation : Catégorie A : habitation, zones résidentielles");
    }
    case 3 :
    {
      return gettext ("Exploitation : Catégorie B : bureaux");
    }
    case 4 :
    {
      return gettext ("Exploitation : Catégorie C : lieux de réunion");
    }
    case 5 :
    {
      return gettext ("Exploitation : Catégorie D : commerces");
    }
    case 6 :
    {
      return gettext ("Exploitation : Catégorie E : stockage");
    }
    case 7 :
    {
      return gettext ("Exploitation : Catégorie F : zone de trafic, véhicules de poids inférieur à 30 kN");
    }
    case 8 :
    {
      return gettext ("Exploitation : Catégorie G : zone de trafic, véhicules de poids entre 30 kN et 160 kN");
    }
    case 9 :
    {
      return gettext ("Exploitation : Catégorie H : toits");
    }
    case 10 :
    {
      return gettext ("Exploitation : Catégorie H : toits d'un bâtiment de catégorie A ou B");
    }
    case 11 :
    {
      return gettext ("Exploitation : Catégorie I : toitures accessibles avec locaux des catégories A ou B");
    }
    case 12 :
    {
      return gettext ("Exploitation : Catégorie I : toitures accessibles avec locaux des catégories C ou D");
    }
    case 13 :
    {
      return gettext ("Exploitation : Catégorie K : Hélicoptère sur la toiture");
    }
    case 14 :
    {
      return gettext ("Exploitation : Catégorie K : Hélicoptère sur la toiture, autres charges (fret, personnel, accessoires ou équipements divers)");
    }
    case 15 :
    {
      return gettext ("Neige : Finlande, Islande, Norvège, Suède");
    }
    case 16 :
    {
      return gettext ("Neige : Saint-Pierre-et-Miquelon");
    }
    case 17 :
    {
      return gettext ("Neige : Autres états membres CEN, altitude > 1000 m");
    }
    case 18 :
    {
      return gettext ("Neige : Autres états membres CEN, altitude <= 1000 m");
    }
    case 19 :
    {
      return gettext ("Vent");
    }
    case 20 :
    {
      return gettext ("Température (hors incendie)");
    }
    case 21 :
    {
      return gettext ("Accidentelle");
    }
    case 22 :
    {
      return gettext ("Sismique");
    }
    case 23 :
    {
      return gettext ("Eaux souterraines");
    }
    default :
    {
      return NULL;
    }
  }
}


/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
