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

#include "CNbUser.hpp"


/**
 * \brief Constructeur d'une classe CNbUser.
 * \param valeur (in) La valeur initiale.
 * \param unit (in) L'unité du nombre.
 */
CNbUser::CNbUser (double valeur,
                  EUnite unit) :
  val (valeur)
  , unite (unit)
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
  return this->val;
}


/**
 * \brief Renvoie l'unité du nombre.
 */
EUnite
CNbUser::getUnite () const
{
  return this->unite;
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
  int         size = 1024;
  bool        b = false;
  va_list     marker;
  std::string s;
  
  while (!b)
  {
    int         n;
    
    s.resize (size);
    va_start (marker, fmt);
    n = vsnprintf (const_cast <char *> (s.c_str ()), size, fmt.c_str (), marker);
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
  
  if (fabs (this->val) > 1e15)
  {
    for (width = 0; width <= 15; width++)
    {
      retour = format ("%.*le", width, this->val);
      sscanf (retour.c_str (), "%le", &test);
      if ((fabs (this->val) * 0.999999999999999 <= fabs (test)) &&
          (fabs (test) <= fabs (this->val) * 1.000000000000001))
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
      retour = format ("%.*lf", width, this->val);
      sscanf (retour.c_str (), "%lf", &test);
      if ((fabs (this->val) * 0.999999999999999 <= fabs (test)) &&
          (fabs (test) <= fabs (this->val) * 1.000000000000001))
      {
        break;
      }
    }
  }
  
  return format ("%.*lf", width, this->val);
}


/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
