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

#ifndef __COMMON_ERREURS_H
#define __COMMON_ERREURS_H

#include "config.h"
#include <stdio.h>

//#ifdef NDEBUG
#define BUG(X, Y) {if (!(X)) \
  { \
    printf(gettext("fichier %s, fonction %s, ligne %d, test : %s\n"), __FILE__, __FUNCTION__, __LINE__, #X); \
    return Y; \
  } \
}
//#else
//#define BUG(X, Y) ({assert(X);})
//#endif
/**
 * \def BUG(X, Y)
 * \brief La macro est l'équivalent d'un "return Y;" si la condition X n'est
 * pas vérifiée. Cependant, afin de faciliter le débogage des erreurs via gdb,
 * il est rajouté, pour la version expérimentale du programme un "assert" qui
 * s'assure de créer une erreur si la condition X n'est pas respectée.
 * \param X : condition à vérifier,
 * \param Y : Renvoie Y si la condition X n'est pas vérifiée.
 */

#define BUGMSG(X, Y, ...) {if (!(X)) \
            { \
              printf(gettext("fichier %s, fonction %s, ligne %d, test : %s, texte : "), __FILE__, __FUNCTION__, __LINE__, #X); \
              printf(__VA_ARGS__); \
              return Y; \
            } \
          }
//#else
//#define BUGMSG(X, Y, ...) ({if (!(X)) \
//             { \
//               printf(__VA_ARGS__); \
//               assert(X); \
//               return Y; \
//             } \
//           })
//#endif
/**
 * \def BUGMSG(X, Y, ...)
 * \brief Cette macro est identique à la commande #BUG mais ajoute un message
 *        d'erreur avant l'arrêt du programme. D'une maniète générale, cette 
 *        macro doit être utilisée dès que l'erreur arrive et la macro BUG doit
 *        être utilisée pour indiquer une erreur par la valeur retour des
 *        fonctions utilisant déjà la macro BUGMSG.
 * \param X : condition à vérifier,
 * \param Y : Renvoie Y si la condition X n'est pas vérifiée,
 * \param ... : message à afficher sous la forme __format, arg.
 */

#endif
