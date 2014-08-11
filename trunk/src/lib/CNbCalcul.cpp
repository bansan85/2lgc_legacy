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
#include "EUnite.hh"
#include "MErreurs.hh"


/**
 * \brief Constructeur d'une classe CNbCalcul.
 * \param valeur (in) La valeur initiale.
 * \param unit (in) L'unité du nombre.
 */
CNbCalcul::CNbCalcul (double   valeur,
                      EUnite   unit,
                      uint8_t *decimales_) :
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
 * \brief Assignment operator de CProjet.
 * \param other La classe à dupliquer.
 */
CNbCalcul &
CNbCalcul::operator = (const CNbCalcul & other)
{
  this->val = other.val;
  this->unite = other.unite;
  this->decimales = other.decimales;
  
  return *this;
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
  return this->val;
}


/**
 * \brief Renvoie l'unité du nombre.
 */
EUnite
CNbCalcul::getUnite () const
{
  return this->unite;
}


/**
 * \brief Renvoie le nombre sous forme de texte en respectant le nombre de
 *        décimales..
 */
std::string
CNbCalcul::toString () const
{
  std::ostringstream oss;
  
  oss.precision (*decimales);
  oss << std::fixed << val;
  
  return oss.str ();
}


bool CHK
CNbCalcul::newXML (xmlNodePtr root) const
{
  std::unique_ptr <xmlNode, void (*)(xmlNodePtr)> node (
    xmlNewNode (NULL, reinterpret_cast <const xmlChar *> ("NbCalcul")),
    xmlFreeNode);
  
  BUGCRIT (node.get (),
           false,
           NULL,
           gettext ("Erreur d'allocation mémoire.\n"))
  
  std::ostringstream oss;
  
  oss << std::scientific << this->val;
  
  BUGCRIT (xmlSetProp (
             node.get (),
             reinterpret_cast <const xmlChar *> ("valeur"),
             reinterpret_cast <const xmlChar *> (oss.str ().c_str ())),
           false,
           NULL,
           gettext ("Problème depuis la librairie : %s\n"), "xml2")
  
  BUGCRIT (xmlSetProp (
             node.get (),
             reinterpret_cast <const xmlChar *> ("unite"),
             reinterpret_cast <const xmlChar *> (EUniteConst[unite].c_str ())),
           false,
           NULL,
           gettext ("Problème depuis la librairie : %s\n"), "xml2")
  
  BUGCRIT (xmlAddChild (root, node.get ()),
           false,
           NULL,
           gettext ("Problème depuis la librairie : %s\n"), "xml2")
  
  node.release ();
  
  return true;
}


/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
