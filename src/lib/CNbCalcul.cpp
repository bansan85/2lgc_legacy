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
#include <cstdarg>
#include <memory>
#include <locale>
#include <sstream>

#include "CNbCalcul.hpp"
#include "SString.hpp"
#include "MErreurs.hpp"
#include "EUniteTxt.hpp"

/**
 * \brief Constructeur d'une classe CNbCalcul.
 * \param valeur (in) La valeur initiale.
 * \param unit (in) L'unité du nombre.
 */
CNbCalcul::CNbCalcul (
  double                                                        valeur,
  EUnite                                                        unit,
  std::array <uint8_t, static_cast <size_t> (EUnite::U_LAST)> & decimales_) :
  val (valeur),
  unite (unit),
  decimales (decimales_)
{
}

/**
 * \brief Constructeur d'une classe CNbCalcul.
 * \param nb Le nombre à copier.
 */
CNbCalcul::CNbCalcul (CNbCalcul & nb) :
  val (nb.val),
  unite (nb.unite),
  decimales (nb.decimales)
{
}

/**
 * \brief Libère une classe CNbCalcul.
 */
CNbCalcul::~CNbCalcul ()
{
}

/**
 * \brief Renvoie la valeur du nombre.
 */
double
CNbCalcul::getVal () const
{
  return val;
}

/**
 * \brief Renvoie l'unité du nombre.
 */
EUnite
CNbCalcul::getUnite () const
{
  return unite;
}

/**
 * \brief Renvoie le nombre sous forme de texte en respectant le nombre de
 *        décimales..
 */
std::string
CNbCalcul::toString () const
{
  std::ostringstream oss;
  
  oss.precision (decimales[static_cast <size_t> (unite)]);
  oss << std::fixed << val;
  
  return oss.str ();
}

bool CHK
CNbCalcul::newXML (xmlNodePtr root) const
{
  std::unique_ptr <xmlNode, void (*)(xmlNodePtr)> node (
                    xmlNewNode (nullptr, BAD_CAST2 ("NbCalcul")), xmlFreeNode);
  
  BUGCRIT (node.get (),
           false,
           static_cast <CUndoManager *> (nullptr),
           gettext ("Erreur d'allocation mémoire.\n"))
  
  std::ostringstream oss;
  
  oss << std::scientific << val;
  
  BUGCRIT (xmlSetProp (node.get (),
                       BAD_CAST2 ("valeur"),
                       BAD_CAST2 (oss.str ().c_str ())),
           false,
           static_cast <CUndoManager *> (nullptr),
           gettext ("Problème depuis la librairie : %s\n"), "xml2")
  
  BUGCRIT (xmlSetProp (node.get (),
                       BAD_CAST2 ("unite"),
                       BAD_CAST2 (EUniteConst[static_cast <size_t> (unite)].
                                                                    c_str ())),
           false,
           static_cast <CUndoManager *> (nullptr),
           gettext ("Problème depuis la librairie : %s\n"), "xml2")
  
  BUGCRIT (xmlAddChild (root, node.get ()),
           false,
           static_cast <CUndoManager *> (nullptr),
           gettext ("Problème depuis la librairie : %s\n"), "xml2")
  
  node.release ();
  
  return true;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
