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

#ifndef __SSTRING_HPP
#define __SSTRING_HPP

#include "config.h"

/**
 * \brief Équivalent de sprintf mais sécurisé ou encore de g_strdup_printf mais
 *        en version std::string. Honteusement volé de
 *        http://stackoverflow.com/questions/2342162#3742999.
 * \param fmt : le texte à formater,
 * \param ... : les divers paramètres.
 * \return Le texte formaté en format std::string.
 */
inline std::string &
format (const std::string fmt,
        ...)
{
  size_t      size = 1024;
  bool        b = false;
  va_list     marker;
  static std::string s;
  
  while (!b)
  {
    int32_t n;
    
    s.resize (size);
    va_start (marker, fmt);
    n = vsnprintf (const_cast <char *> (s.c_str ()),
                   size,
                   fmt.c_str (),
                   marker);
    va_end (marker);
    if ((n > 0) && (b = (static_cast <size_t> (n) < size)))
    {
      s.resize (static_cast <size_t> (n));
    }
    else
    {
      size = size * 2;
    }
  }
  return s;
}

#define BAD_CAST2 reinterpret_cast <const xmlChar *>
#define BAD_TSAC2 reinterpret_cast <const char *>

#endif

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
