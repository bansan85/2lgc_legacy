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
#include <string.h>
#include <cholmod.h>
#include <gmodule.h>

#include "common_projet.h"
#include "common_erreurs.h"

gboolean
EF_rigidite_init (Projet *p)
/**
 * \brief Initialise à NULL les différentes matrices de rigidité.
 * \param p : la variable projet.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL.
 */
{
  BUGPARAM (p, "%p", p, FALSE)
  
  p->calculs.m_part = NULL;
  p->calculs.m_comp = NULL;
  p->calculs.numeric = NULL;
  p->calculs.ap = NULL;
  p->calculs.ai = NULL;
  p->calculs.ax = NULL;
  p->calculs.t_part = NULL;
  p->calculs.t_comp = NULL;
  p->calculs.n_comp = NULL;
  p->calculs.n_part = NULL;
  
  return TRUE;
}


gboolean
EF_rigidite_free (Projet *p)
/**
 * \brief Libère la liste contenant la matrice de rigidité.
 * \param p : la variable projet.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL.
 */
{
  unsigned int i;
  
  BUGPARAM (p, "%p", p, FALSE)
  
  cholmod_free_triplet (&p->calculs.t_part, p->calculs.c);
  p->calculs.t_part = NULL;
  cholmod_free_triplet (&p->calculs.t_comp, p->calculs.c);
  p->calculs.t_comp = NULL;
  
  cholmod_free_sparse (&(p->calculs.m_comp), p->calculs.c);
  p->calculs.m_comp = NULL;
  cholmod_free_sparse (&(p->calculs.m_part), p->calculs.c);
  p->calculs.m_part = NULL;
  
  umfpack_di_free_numeric (&p->calculs.numeric);
  p->calculs.numeric = NULL;
  
  free (p->calculs.ap);
  p->calculs.ap = NULL;
  free (p->calculs.ai);
  p->calculs.ai = NULL;
  free (p->calculs.ax);
  p->calculs.ax = NULL;
  
  if (p->calculs.n_comp != NULL)
  {
    for (i = 0; i < g_list_length (p->modele.noeuds); i++)
      free (p->calculs.n_comp[i]);
    free (p->calculs.n_comp);
    p->calculs.n_comp = NULL;
  }
  if (p->calculs.n_part != NULL)
  {
    for (i = 0; i < g_list_length (p->modele.noeuds); i++)
      free (p->calculs.n_part[i]);
    free (p->calculs.n_part);
    p->calculs.n_part = NULL;
  }
  
  return TRUE;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
