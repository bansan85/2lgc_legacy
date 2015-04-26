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

#ifndef __MERREURS_HH
#define __MERREURS_HH

#include "config.hpp"

#include <cstdio>
#include <libintl.h>

#include "UndoManager.hpp"

#define PRINTF(...) printf (__VA_ARGS__)


#define FAILINFO(Y, MANAGER, MSG, ...) { \
  PRINTF (gettext ("fichier %s, fonction %s, ligne %d, texte : "), \
          __FILE__, \
          __FUNCTION__, \
          __LINE__); \
  if ((MANAGER) != nullptr) \
    (MANAGER)->rollback (); \
  PRINTF (MSG); \
  PRINTF (__VA_ARGS__); \
  return Y; \
}


#define XXX(X, Y, MANAGER, MSG, ...) { \
  try \
  { \
    bool tmp_x = (X); \
    if (!tmp_x) \
    { \
      FAILINFO (Y, MANAGER, MSG, __VA_ARGS__) \
    } \
  } \
  catch (...) \
  { \
    FAILINFO (Y, MANAGER, MSG, __VA_ARGS__) \
  } \
}
/**
 * \def XXX(X, Y, MANAGER, MSG, ...)
 * \brief La macro est l'équivalent d'un "return Y; ..." si la condition X
 *        n'est pas vérifiée.
 * \param X : condition à vérifier,
 * \param Y : renvoie Y si la condition X n'est pas vérifiée,
 * \param MANAGER : Le gestionnaire d'annulation. Peut être NULL,
 * \param MSG : Le message afficher.
 * \param ... : Message complémentaire sous une forme compatible avec fprintf.
 */


#define BUGPARAM(PARAM, TYPE, X, Y, MANAGER) { \
  if (!(X)) \
  { \
    FAILINFO (Y, \
              MANAGER, \
              gettext ("Erreur de programmation."), \
              "%s" #PARAM " = " TYPE ".\n", gettext ("Paramètre incorrect : "), PARAM) \
  } \
}
/**
 * \def BUGPARAM(PARAM, TYPE, X, Y, MANAGER)
 * \brief Macro spécialement conçu pour la vériifcation des paramètres.
 * \param PARAM : paramètre (variable) invalide,
 * \param TYPE : type du paramètre sous forme de texte ("%s", "%d", …),
 * \param X : condition à vérifier,
 * \param Y : renvoie Y si la condition X n'est pas vérifiée,
 * \param MANAGER : Le gestionnaire d'annulation. Peut être NULL,
 */


#define BUGCRIT(X, Y, MANAGER, ...) { \
  XXX (X, Y, MANAGER, gettext ("Erreur de type critique.\n"), __VA_ARGS__) \
}
/**
 * \def BUGCRIT(X, Y, ...)
 * \brief Cette macro est identique à la commande #XXX mais ajoute un message
 *        d'erreur "Erreur critique" avant l'arrêt de la fonction.
 * \param X : condition à vérifier,
 * \param Y : renvoie Y si la condition X n'est pas vérifiée,
 * \param MANAGER : Le gestionnaire d'annulation. Peut être NULL,
 */


#define BUGUSER(X, Y, MANAGER, ...) { \
  XXX (X, Y, MANAGER, gettext ("Erreur de type utilisateur.\n"), __VA_ARGS__) \
}


#define BUGPROG(X, Y, MANAGER, ...) { \
  XXX (X, Y, MANAGER, gettext ("Erreur de programmation.\n"), __VA_ARGS__) \
}


#define BUGCONT(X, Y, MANAGER) { \
  XXX (X, Y, MANAGER, gettext ("Propagation de l'erreur.\n"), "BLA") \
}


#endif

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
