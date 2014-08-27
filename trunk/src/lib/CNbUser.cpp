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
#include <iostream>

#include "CNbUser.hpp"
#include "EUnite.hh"
#include "MAbrev.hh"
#include "MErreurs.hh"


std::string
format (const std::string fmt,
        ...);


/**
 * \brief Constructeur d'une classe CNbUser.
 * \param valeur (in) La valeur initiale.
 * \param unit (in) L'unité du nombre.
 */
CNbUser::CNbUser (double valeur,
                  EUnite unit) :
  val (valeur),
  unite (unit)
{
}


/**
 * \brief Constructeur d'une classe CNbUser.
 * \param nb Le nombre à copier.
 */
CNbUser::CNbUser (CNbUser & nb) :
  val (nb.val),
  unite (nb.unite)
{
}


/**
 * \brief Libère une classe CNbUser.
 */
CNbUser::~CNbUser ()
{
}


/**
 * \brief Renvoie la valeur du nombre.
 */
double
CNbUser::getVal () const
{
  return val;
}


/**
 * \brief Renvoie l'unité du nombre.
 */
EUnite
CNbUser::getUnite () const
{
  return unite;
}


/**
 * \brief Équivalent de sprintf mais sécurisé ou encore de g_strdup_printf mais
 *        en version std::string. Honteusement volé de
 *        http://stackoverflow.com/questions/2342162#3742999.
 * \param fmt : le texte à formater,
 * \param ... : les divers paramètres.
 * \return Le texte formaté en format std::string.
 */
std::string
format (const std::string fmt,
        ...)
{
  uint32_t    size = 1024;
  bool        b = false;
  va_list     marker;
  std::string s;
  
  while (!b)
  {
    uint32_t n;
    
    s.resize (size);
    va_start (marker, fmt);
    n = vsnprintf (const_cast <char *> (s.c_str ()),
                   size,
                   fmt.c_str (),
                   marker);
    va_end (marker);
    if ((n > 0) && ((b = (n < size)) == true))
    {
      s.resize (n);
    }
    else
    {
      size = size * 2;
    }
  }
  return s;
}


/**
 * \brief Renvoie le nombre sous forme de texte sans respecter le nombre de
 *        décimales..
 */
std::string
CNbUser::toString () const
{
  std::string retour;
  uint8_t     width;
  double      test;
  
  if (fabs (val) > 1e15)
  {
    for (width = 0; width <= 15; width++)
    {
      retour = format ("%.*le", width, val);
      sscanf (retour.c_str (), "%le", &test);
      if ((fabs (val) * 0.999999999999999 <= fabs (test)) &&
          (fabs (test) <= fabs (val) * 1.000000000000001))
      {
        break;
      }
    }
  }
  // Sinon on affiche sous forme normale
  else
  {
    for (width = 0; width <= 15; width++)
    {
      retour = format ("%.*lf", width, val);
      sscanf (retour.c_str (), "%lf", &test);
      if ((fabs (val) * 0.999999999999999 <= fabs (test)) &&
          (fabs (test) <= fabs (val) * 1.000000000000001))
      {
        break;
      }
    }
  }
  
  return format ("%.*lf", width, val);
}


bool CHK
CNbUser::newXML (xmlNodePtr root) const
{
  std::unique_ptr <xmlNode, void (*)(xmlNodePtr)> node (
                         xmlNewNode (NULL, BAD_CAST2 ("NbUser")), xmlFreeNode);
  
  BUGCRIT (node.get (),
           false,
           NULL,
           gettext ("Erreur d'allocation mémoire.\n"))
  
  std::ostringstream oss;
  
  oss << std::scientific << val;
  
  BUGCRIT (xmlSetProp (node.get (),
                       BAD_CAST2 ("valeur"),
                       BAD_CAST2 (oss.str ().c_str ())),
           false,
           NULL,
           gettext ("Problème depuis la librairie : %s\n"), "xml2")
  
  BUGCRIT (xmlSetProp (node.get (),
                       BAD_CAST2 ("unite"),
                       BAD_CAST2 (EUniteConst[unite].c_str ())),
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
