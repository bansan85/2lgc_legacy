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
#include <libintl.h>
#include <locale.h>
#include <gmodule.h>
#include <string.h>

#include <algorithm>

#include "1990_action.hpp"
#include "common_projet.hpp"
#include "common_math.hpp"
#include "common_erreurs.hpp"
#include "common_fonction.hpp"
#include "EF_resultat.hpp"


/**
 * \brief Renvoie la réaction d'appui d'un noeud en fonction d'une liste de
 *        pondérations.
 * \param liste : liste d'actions préalablement créées par la fonction 
 *        #_1990_action_ponderation_resultat,
 * \param noeud : noeud à étudier,
 * \param indice : 0 si Fx,
 *                 1 si Fy,
 *                 2 si Fz,
 *                 3 si Mx, 
 *                 4 si My et 
 *                 5 si Mz,
 * \param p : la variable projet,
 * \param texte : la réaction d'appui sous forme d'un texte (peut être NULL),
 * \param mini : la valeur minimale (dans le cas d'une liste à plusieurs
 *               actions), peut être NULL,
 * \param maxi : la valeur maximale (dans le cas d'une liste à plusieurs
 *               actions), peut être NULL.
 * \return
 *   Succès : true.\n
 *   Échec : false :
 *     - liste == NULL,
 *     - noeud == NULL,
 *     - p == NULL.
 */
bool
EF_resultat_noeud_reaction_appui (std::list <Action *> *liste,
                                  EF_Noeud             *noeud,
                                  uint8_t               indice,
                                  Projet               *p,
                                  char                **texte,
                                  double               *mini,
                                  double               *maxi)
{
  std::list <Action*>::iterator it;
  
  long    i;
  double  mi, ma;
  double *x;
  Action *action;
  
  BUGPARAM (noeud, "%p", noeud, false)
  BUGPARAM (p, "%p", p, false)
  INFO (indice <= 5,
        false,
        (gettext ("Indice hors limite.\n")); )
  
  if (liste == NULL)
  {
    if (texte != NULL)
    {
      BUGCRIT (*texte = g_strdup_printf ("%.*lf",
                                         indice < 3 ?
                                           DECIMAL_FORCE :
                                           DECIMAL_MOMENT,
                                         0.),
               false,
               (gettext ("Erreur d'allocation mémoire.\n")); )
    }
    if (mini != NULL)
    {
      *mini = 0.;
    }
    if (maxi != NULL)
    {
      *maxi = 0.;
    }
    return true;
  }
  
  i = std::distance (p->modele.noeuds.begin (),
                     std::find (p->modele.noeuds.begin (), 
                                p->modele.noeuds.end (),
                                noeud));
  
  it = liste->begin ();
  action = *it;
  x = (double *) _1990_action_efforts_noeuds_renvoie (action)->x;
  mi = x[i * 6 + indice];
  ma = x[i * 6 + indice];
  ++it;
  while (it != liste->end ())
  {
    action = *it;
    
    x = (double *) _1990_action_efforts_noeuds_renvoie (action)->x;
    
    if (x[i * 6 + indice] < mi)
    {
      mi = x[i * 6 + indice];
    }
    if (x[i * 6 + indice] > ma)
    {
      ma = x[i * 6 + indice];
    }
    
    ++it;
  }
  
  if (mini != NULL)
  {
    *mini = mi;
  }
  if (maxi != NULL)
  {
    *maxi = ma;
  }
  if (texte != NULL)
  {
    if (!errrel (mi, ma))
    {
      BUGCRIT (*texte = g_strdup_printf ("%.*lf/%.*lf",
                                         indice < 3 ?
                                           DECIMAL_FORCE :
                                           DECIMAL_MOMENT,
                                         mi,
                                         indice < 3 ?
                                           DECIMAL_FORCE :
                                           DECIMAL_MOMENT,
                                         ma),
               false,
               (gettext ("Erreur d'allocation mémoire.\n")); )
    }
    else
    {
      BUGCRIT (*texte = g_strdup_printf ("%.*lf",
                                         indice < 3 ?
                                           DECIMAL_FORCE :
                                           DECIMAL_MOMENT,
                                         mi),
               false,
               (gettext ("Erreur d'allocation mémoire.\n")); )
    }
  }
  
  return true;
}


/**
 * \brief Renvoie le déplacement d'un noeud en fonction d'une liste de
 *        pondérations.
 * \param liste : liste d'actions préalablement créées par la fonction 
 *        #_1990_action_ponderation_resultat,
 * \param noeud : noeud à étudier,
 * \param indice : 0 si Ux,
 *                 1 si Uy, 
 *                 2 si Uz, 
 *                 3 si rx, 
 *                 4 si ry et 
 *                 5 si rz,
 * \param p : la variable projet,
 * \param texte : le déplacement sous forme d'un texte (peut être NULL),
 * \param mini : la valeur minimale (dans le cas d'une liste à plusieurs
 *               actions), peut être NULL,
 * \param maxi : la valeur maximale (dans le cas d'une liste à plusieurs
 *               actions), peut être NULL.
 * \return
 *   Succès : true.\n
 *   Échec : false :
 *     - liste == NULL,
 *     - noeud == NULL,
 *     - p == NULL.
 */
bool
EF_resultat_noeud_deplacement (std::list <Action *> *liste,
                               EF_Noeud             *noeud,
                               uint8_t               indice,
                               Projet               *p,
                               char                **texte,
                               double               *mini,
                               double               *maxi)
{
  std::list <Action*>::iterator it;
  
  long    i;
  double  mi, ma;
  double *x;
  Action *action;
  
  BUGPARAM (noeud, "%p", noeud, false)
  BUGPARAM (p, "%p", p, false)
  INFO (indice <= 5,
        false,
        (gettext ("Indice hors limite.\n")); )
  
  if (liste == NULL)
  {
    if (texte != NULL)
    {
      BUGCRIT (*texte = g_strdup_printf ("%.*lf",
                                         indice < 3 ?
                                           DECIMAL_DEPLACEMENT :
                                           DECIMAL_ROTATION,
                                         0.),
               false,
               (gettext ("Erreur d'allocation mémoire.\n")); )
    }
    if (mini != NULL)
    {
      *mini = 0.;
    }
    if (maxi != NULL)
    {
      *maxi = 0.;
    }
    return true;
  }
  
  i = std::distance (p->modele.noeuds.begin (),
                     std::find (p->modele.noeuds.begin (), 
                                p->modele.noeuds.end (), 
                                noeud));
  
  it = liste->begin ();
  action = *it;
  x = (double *) _1990_action_deplacement_renvoie (action)->x;
  mi = x[i * 6 + indice];
  ma = x[i * 6 + indice];
  ++it;
  while (it != liste->end ())
  {
    action = *it;
    
    x = (double *) _1990_action_deplacement_renvoie (action)->x;
    
    if (x[i * 6 + indice] < mi)
    {
      mi = x[i * 6 + indice];
    }
    if (x[i * 6 + indice] > ma)
    {
      ma = x[i * 6 + indice];
    }
    
    ++it;
  }
  
  if (mini != NULL)
  {
    *mini = mi;
  }
  if (maxi != NULL)
  {
    *maxi = ma;
  }
  if (texte != NULL)
  {
    if (!errrel (mi, ma))
    {
      BUGCRIT (*texte = g_strdup_printf ("%.*lf/%.*lf",
                                         indice < 3 ?
                                           DECIMAL_DEPLACEMENT :
                                           DECIMAL_ROTATION,
                                         mi,
                                         indice < 3 ?
                                           DECIMAL_DEPLACEMENT :
                                           DECIMAL_ROTATION,
                                         ma),
               false,
               (gettext ("Erreur d'allocation mémoire.\n")); )
    }
    else
    {
      BUGCRIT (*texte = g_strdup_printf ("%.*lf",
                                         indice < 3 ?
                                           DECIMAL_DEPLACEMENT :
                                           DECIMAL_ROTATION,
                                         mi),
               false,
               (gettext ("Erreur d'allocation mémoire.\n")); )
    }
  }
  
  return true;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
