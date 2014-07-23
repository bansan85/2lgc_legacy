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

#include "config.h"

#include <cstdio>
#include <libintl.h>

#include "CUndoManager.hpp"

#define PRINTF(...) printf (__VA_ARGS__)

/*#define NOTE(X, ...) { \
  if (!(X)) \
  { \
    PRINTF (gettext ("fichier %s, fonction %s, ligne %d, test : %s\n"), \
            __FILE__, \
            __FUNCTION__, \
            __LINE__, \
            #X); \
    PRINTF (__VA_ARGS__); \
  } \
}*/


#define NOWARNING _Pragma("GCC diagnostic push")\
  _Pragma("GCC diagnostic ignored \"-Wunused-result\"")
#define POPWARNING _Pragma("GCC diagnostic pop")

#define FAILINFO(Y, MANAGER, ...) { \
  PRINTF (gettext ("fichier %s, fonction %s, ligne %d, texte : "), \
          __FILE__, \
          __FUNCTION__, \
          __LINE__); \
  if ((MANAGER) != NULL) \
    (static_cast <CUndoManager *> (MANAGER))->rollback (); \
  printf __VA_ARGS__ \
  return Y; \
}

#define FAILPARAM(PARAM, TYPE, Y, MANAGER, ...) { \
  PRINTF ("Paramètre incorrect : " #PARAM " = " TYPE ". ", PARAM); \
  FAILINFO (Y, MANAGER, ("\n"); __VA_ARGS__) \
}

#define FAILCRIT(Y, MANAGER, ...) { \
  PRINTF (gettext ("Erreur critique. ")); \
  FAILINFO (Y, MANAGER, __VA_ARGS__) \
}

#define BUG(X, Y, MANAGER, ...) { \
  try \
  { \
    bool tmp_x = (X); \
    if (!tmp_x) \
    { \
      PRINTF (gettext ("fichier %s, fonction %s, ligne %d, test : %s\n"), \
              __FILE__, \
              __FUNCTION__, \
              __LINE__, \
              #X); \
      if ((MANAGER) != NULL) \
        (static_cast <CUndoManager *> (MANAGER))->rollback (); \
      __VA_ARGS__ \
      return Y; \
    } \
  } \
  catch (...) \
  { \
    PRINTF (gettext ("fichier %s, fonction %s, ligne %d, test : %s\n"), \
            __FILE__, \
            __FUNCTION__, \
            __LINE__, \
            #X); \
    if ((MANAGER) != NULL) \
      (static_cast <CUndoManager *> (MANAGER))->rollback (); \
    __VA_ARGS__ \
    return Y; \
  } \
}
/**
 * \def BUG(X, Y, ...)
 * \brief La macro est l'équivalent d'un "return Y; ..." si la condition X
 *        n'est pas vérifiée.
 * \param X : condition à vérifier,
 * \param Y : renvoie Y si la condition X n'est pas vérifiée,
 * \param ... : Actions à mener avant l'application de return (libération de la
 *              la mémoire, ...). Ne pas oublier le ; final.
 */

/*#define INFO(X, Y, ...) { \
  if (!(X)) \
  { \
    PRINTF (gettext ("fichier %s, fonction %s, ligne %d, test : %s, texte : "), \
            __FILE__, \
            __FUNCTION__, \
            __LINE__, \
            #X); \
    printf __VA_ARGS__ \
    return Y; \
  } \
}*/
/**
 * \def INFO(X, Y, ...)
 * \brief Cette macro est identique à la commande #BUG mais ajoute un message
 *        d'erreur avant l'arrêt de la fonction. D'une manière générale, cette 
 *        macro doit être utilisée dès que l'erreur arrive et la macro BUG doit
 *        être utilisée pour indiquer une erreur par la valeur retour des
 *        fonctions utilisant déjà la macro INFO.\n
 *        Il existe 3 types de message d'erreur : INFO, BUGPARAM, BUGCRIT.
 *        INFO indique un problème à cause de l'utilisateur de l'interface
 *        graphique, BUGPARAM une erreur due à un mauvais argument d'une
 *        fonction accessible par le programmeur, mais ce mauvais argument ne
 *        peut pas être la faute de l'utilisateur via l'interface graphique,
 *        BUGCRIT lorsque une fonction a un mauvais paramètre alors que c'est
 *        impossible (toutes les fonctions non inclus dans les .h disponibles
 *        par l'utilisateur, par exemple les fonctions gérant les évènements
 *        des composants graphiques) ou encore une fonction qui ne renvoie pas
 *        la valeur prévue pour une raison inconnue (allocation mémoire,
 *        default d'un switch, …).
 * \param X : condition à vérifier,
 * \param Y : renvoie Y si la condition X n'est pas vérifiée,
 * \param ... : message à afficher sous la forme "(__format, arg); suite;" Il
 *              est possible de lancer des actions à mener avant l'exécution
 *              de return (libération de la la mémoire, ...) en remplaçant
 *              "suite" par les instructions.
 */


#define BUGPARAM(PARAM, TYPE, X, Y, MANAGER, ...) { \
  if (!(X)) \
  { \
    FAILPARAM (PARAM, TYPE, Y, MANAGER, __VA_ARGS__) \
  } \
}
/**
 * \def BUGPARAM(PARAM, TYPE, X, Y, ...)
 * \brief Cette macro est identique à la commande #INFO mais ajoute un
 *        message d'erreur qui indique le mauvais paramètre et sa valeur.
 * \param PARAM : paramètre (variable) invalide,
 * \param TYPE : type du paramètre sous forme de texte ("%s", "%d", …),
 * \param X : condition à vérifier,
 * \param Y : renvoie Y si la condition X n'est pas vérifiée,
 * \param ... : Il est possible de lancer des actions à mener avant
 *              l'exécution de return (libération de la la mémoire, ...).
 */


#define BUGPARAMCRIT(PARAM, TYPE, X, Y, MANAGER, ...) { \
  if (!(X)) \
  { \
    PRINTF (gettext ("Erreur critique. ")); \
    FAILPARAM (PARAM, TYPE, Y, MANAGER, __VA_ARGS__) \
  } \
}
/**
 * \def BUGPARAM(PARAM, TYPE, X, Y, ...)
 * \brief Cette macro est identique à la commande #INFO mais ajoute un
 *        message d'erreur qui indique le mauvais paramètre et sa valeur.
 * \param PARAM : paramètre (variable) invalide,
 * \param TYPE : type du paramètre sous forme de texte ("%s", "%d", …),
 * \param X : condition à vérifier,
 * \param Y : renvoie Y si la condition X n'est pas vérifiée,
 * \param ... : Il est possible de lancer des actions à mener avant
 *              l'exécution de return (libération de la la mémoire, ...).
 */


#define BUGCRIT(X, Y, MANAGER, ...) { \
  if (!(X)) \
  { \
    PRINTF (gettext ("Erreur critique. ")); \
    FAILINFO (Y, MANAGER, __VA_ARGS__) \
  } \
}
/**
 * \def BUGCRIT(X, Y, ...)
 * \brief Cette macro est identique à la commande #BUG mais ajoute un message
 *        d'erreur "Erreur critique" avant l'arrêt de la fonction.
 * \param X : condition à vérifier,
 * \param Y : renvoie Y si la condition X n'est pas vérifiée,
 * \param ... : message à afficher sous la forme "(__format, arg); suite;" Il
 *              est possible de lancer des actions à mener avant l'exécution
 *              de return (libération de la la mémoire, ...) en remplaçant
 *              "suite" par les instructions.
 */


#endif

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
