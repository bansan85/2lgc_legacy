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

#include <algorithm>
#include <locale>
#include <string.h>

#include "1990_action.hpp"
#include "common_projet.hpp"
#include "common_erreurs.hpp"
#include "common_math.hpp"
#include "common_fonction.hpp"
#include "EF_charge_barre_ponctuelle.hpp"
#include "EF_charge_barre_repartie_uniforme.hpp"
#include "EF_noeuds.hpp"
#include "EF_rigidite.hpp"
#include "EF_sections.hpp"
#include "EF_materiaux.hpp"
#include "EF_calculs.hpp"

#ifdef ENABLE_GTK
#include "common_gtk.hpp"
#include "EF_gtk_resultats.hpp"
#endif


/**
 * \brief Libère la mémoire allouée pour les calculs, et pour les résultats.
 *        Cette fonction doit être appelée à chaque fois qu'une donnée ayant
 *        une influence sur les résultats des calculs est modifiée (ajout d'un
 *        noeud ou d'une barre, modification d'une action, ...).
 * \param p : la variable projet.
 * \return
 *   Succès : true.\n
 *   Échec : false :
 *     - p == NULL
 */
bool
EF_calculs_free (Projet *p)
{
  std::list <Action   *>::iterator it;
  std::list <EF_Barre *>::iterator it2;
  
  BUGPARAM (p, "%p", p, false)
  
  BUG (EF_rigidite_free (p), false)
  
  it = p->actions.begin ();
  while (it != p->actions.end ())
  {
    Action *action = *it;
    
    BUG (_1990_action_free_calculs (p, action), false)
    
    ++it;
  }
  
  it2 = p->modele.barres.begin ();
  while (it2 != p->modele.barres.end ())
  {
    EF_Barre *barre = *it2;
    uint16_t  i;
    
    for (i = 0; i <= barre->discretisation_element; i++)
    {
      if (barre->info_EF[i].m_rig_loc != NULL)
      {
        cholmod_free_sparse (&barre->info_EF[i].m_rig_loc, p->calculs.c);
        barre->info_EF[i].m_rig_loc = NULL;
      }
    }
    
    if (barre->m_rot != NULL)
    {
      cholmod_free_sparse (&barre->m_rot, p->calculs.c);
      barre->m_rot = NULL;
    }
    if (barre->m_rot_t != NULL)
    {
      cholmod_free_sparse (&barre->m_rot_t, p->calculs.c);
      barre->m_rot_t = NULL;
    }
    
    ++it2;
  }
  
#ifdef ENABLE_GTK
  gtk_widget_set_sensitive (UI_GTK.menu_resultats_afficher, FALSE);
  
  if (UI_RES.builder != NULL)
  {
    std::list <Gtk_EF_Resultats_Tableau *>::iterator it3;
    
    it3 = UI_RES.tableaux.begin ();
    
    while (it3 != UI_RES.tableaux.end ())
    {
      Gtk_EF_Resultats_Tableau *res = *it3;
      
      gtk_list_store_clear (res->list_store);
      
      ++it3;
    }
  }
  if (UI_RAP.builder != NULL)
  {
    gtk_list_store_clear (UI_RAP.liste);
  }
#endif
  
  return true;
}


/**
 * \brief Initialise les diverses variables nécessaires à l'ajout des matrices
 *        de rigidité élémentaires.
 * \param p : la variable projet.
 * \return
 *   Succès : true.\n
 *   Échec : false :
 *     - p == NULL,
 *     - Aucun nœud n'est présent,
 *     - Aucune barre n'est présente,
 *     - en cas d'erreur d'allocation mémoire.
 */
bool
EF_calculs_initialise (Projet *p)
{
  uint32_t i, nnz_max;
  uint32_t nb_col_partielle, nb_col_complete;
  size_t   nb_noeuds;
  
  std::list <EF_Noeud *>::iterator it;
  std::list <EF_Barre *>::iterator it2;
  
  BUGPARAM (p, "%p", p, false)
  INFO (!p->modele.barres.empty (),
        false,
        (gettext ("Impossible de réaliser un calcul sans barre existante.\n")); )
  
  nb_noeuds = p->modele.noeuds.size ();
  INFO (nb_noeuds != 0,
        false,
        (gettext ("Impossible de réaliser un calcul sans noeud existant.\n")); )
  
  // Allocation de la mémoire nécessaire pour contenir la position de chaque
  // degré de liberté des noeuds (via n_part et
  // n_comp) dans la matrice de rigidité globale partielle et
  // complète.
  BUGCRIT (p->calculs.n_part = (uint32_t **)
             malloc (sizeof (uint32_t *) * nb_noeuds),
           false,
           (gettext ("Erreur d'allocation mémoire.\n")); )
  memset (p->calculs.n_part, 0, sizeof (uint32_t *) * nb_noeuds);
  for (i = 0; i < nb_noeuds; i++)
  {
    BUGCRIT (p->calculs.n_part[i] = (uint32_t *)
               malloc (6 * sizeof (uint32_t)),
             false,
             (gettext ("Erreur d'allocation mémoire.\n"));
               EF_calculs_free (p); )
  }
  BUGCRIT (p->calculs.n_comp = (uint32_t **)
             malloc (sizeof (uint32_t *) * nb_noeuds),
           false,
           (gettext ("Erreur d'allocation mémoire.\n"));
             EF_calculs_free (p); )
  memset (p->calculs.n_comp, 0, sizeof (uint32_t *) * nb_noeuds);
  for (i = 0; i < nb_noeuds; i++)
  {
    BUGCRIT (p->calculs.n_comp[i] = (uint32_t *)
               malloc (6 * sizeof (uint32_t)),
             false,
             (gettext ("Erreur d'allocation mémoire.\n"));
               EF_calculs_free (p); )
  }
  // Détermination du nombre de colonnes pour la matrice de rigidité complète et
  // partielle :
  // nb_col_partielle = 0.
  // nb_col_complete = 0.
  // Pour chaque noeud, y compris ceux dus à la discrétisation
  //   Pour chaque degré de liberté du noeud (x, y, z, rx, ry, rz)
  //     n_comp[numero_noeud][degre] = nb_col_complete.
  //     nb_col_complete = nb_col_complete + 1.
  //     Si le degré de liberté de liberté est libre Alors
  //       n_part[numero_noeud][degre] = nb_col_partielle.
  //       nb_col_partielle = nb_col_partielle + 1.
  //     Sinon
  //       n_part[numero_noeud][degre] = -1.
  //     FinSi
  //   FinPour
  // FinPour
  nb_col_partielle = 0;
  nb_col_complete = 0;
  it = p->modele.noeuds.begin ();
  i = 0;
  while (it != p->modele.noeuds. end ())
  {
    EF_Noeud *noeud = *it;
    
    p->calculs.n_comp[i][0] = nb_col_complete;
    nb_col_complete++;
    p->calculs.n_comp[i][1] = nb_col_complete;
    nb_col_complete++;
    p->calculs.n_comp[i][2] = nb_col_complete;
    nb_col_complete++;
    p->calculs.n_comp[i][3] = nb_col_complete;
    nb_col_complete++;
    p->calculs.n_comp[i][4] = nb_col_complete;
    nb_col_complete++;
    p->calculs.n_comp[i][5] = nb_col_complete;
    nb_col_complete++;
    
    if (noeud->appui == NULL)
    {
      p->calculs.n_part[i][0] = nb_col_partielle;
      nb_col_partielle++;
      p->calculs.n_part[i][1] = nb_col_partielle;
      nb_col_partielle++;
      p->calculs.n_part[i][2] = nb_col_partielle;
      nb_col_partielle++;
      p->calculs.n_part[i][3] = nb_col_partielle;
      nb_col_partielle++;
      p->calculs.n_part[i][4] = nb_col_partielle;
      nb_col_partielle++;
      p->calculs.n_part[i][5] = nb_col_partielle;
      nb_col_partielle++;
    }
    else
    {
      EF_Appui *appui = noeud->appui;
      
      if (appui->ux == EF_APPUI_LIBRE)
      {
        p->calculs.n_part[i][0] = nb_col_partielle;
        nb_col_partielle++;
      }
      else
      {
        p->calculs.n_part[i][0] = UINT32_MAX;
      }
      
      if (appui->uy == EF_APPUI_LIBRE)
      {
        p->calculs.n_part[i][1] = nb_col_partielle;
        nb_col_partielle++;
      }
      else
      {
        p->calculs.n_part[i][1] = UINT32_MAX;
      }
      
      if (appui->uz == EF_APPUI_LIBRE)
      {
        p->calculs.n_part[i][2] = nb_col_partielle;
        nb_col_partielle++;
      }
      else
      {
        p->calculs.n_part[i][2] = UINT32_MAX;
      }
      
      if (appui->rx == EF_APPUI_LIBRE)
      {
        p->calculs.n_part[i][3] = nb_col_partielle;
        nb_col_partielle++;
      }
      else
      {
        p->calculs.n_part[i][3] = UINT32_MAX;
      }
      
      if (appui->ry == EF_APPUI_LIBRE)
      {
        p->calculs.n_part[i][4] = nb_col_partielle;
        nb_col_partielle++;
      }
      else
      {
        p->calculs.n_part[i][4] = UINT32_MAX;
      }
      
      if (appui->rz == EF_APPUI_LIBRE)
      {
        p->calculs.n_part[i][5] = nb_col_partielle;
        nb_col_partielle++;
      }
      else
      {
        p->calculs.n_part[i][5] = UINT32_MAX;
      }
    }
    
    i++;
    
    ++it;
  }
  
  // Détermination du nombre de matrices de rigidité globale élémentaire du
  // système (y compris la discrétisation).
  // Détermination du nombre de triplets, soit 12*12*nombre_de_matrices.
  nnz_max = 0;
  it2 = p->modele.barres.begin ();
  while (it2 != p->modele.barres.end ())
  {
    EF_Barre *element = *it2;
    
    nnz_max += 12 * 12U * (element->discretisation_element + 1U);
    
    ++it2;
  }
  
  // Allocation des triplets de la matrice de rigidité partielle (t_part) et la
  // matrice de rigidité globale (triplet_rigidite_globale).
  BUGCRIT (p->calculs.t_part = cholmod_allocate_triplet (nb_col_partielle,
                                                         nb_col_partielle,
                                                         nnz_max,
                                                         0,
                                                         CHOLMOD_REAL,
                                                         p->calculs.c),
           false,
           (gettext ("Erreur d'allocation mémoire.\n"));
             EF_calculs_free (p); )
  p->calculs.t_part->nnz = nnz_max;
  BUGCRIT (p->calculs.t_comp = cholmod_allocate_triplet (nb_col_complete,
                                                         nb_col_complete,
                                                         nnz_max,
                                                         0,
                                                         CHOLMOD_REAL,
                                                         p->calculs.c),
           false,
           (gettext ("Erreur d'allocation mémoire.\n"));
             EF_calculs_free (p); )
  p->calculs.t_comp->nnz = nnz_max;
  
  // Initialisation de l'indice du triplet en cours à 0 pour la matrice de
  // rigidité partielle (t_part_en_cours) et globale (t_comp_en_cours).
  p->calculs.t_part_en_cours = 0;
  p->calculs.t_comp_en_cours = 0;
  
  return true;
}


/**
 * \brief Factorisation de la matrice de rigidité.
 * \param p : la variable projet.
 * \return
 *   Succès : true.\n
 *   Échec : false :
 *     - p == NULL,
 *     - p->calculs.t_part == NULL,
 *     - p->calculs.t_comp == NULL,
 *     - en cas d'erreur d'allocation mémoire.
 */
bool
EF_calculs_genere_mat_rig (Projet *p)
{
  uint32_t  i;
  uint32_t *ai, *aj;
  double   *ax;
  void     *symbolic;
  int       status; // NS
  
  BUGPARAM (p, "%p", p, false)
  BUGPARAM (p->calculs.t_part, "%p", p->calculs.t_part, false)
  BUGPARAM (p->calculs.t_comp, "%p", p->calculs.t_comp, false)
  
  ai = (uint32_t *) p->calculs.t_part->i;
  aj = (uint32_t *) p->calculs.t_part->j;
  ax = (double *) p->calculs.t_part->x;
  // On initialise à 0 les valeurs non utilisées dans le triplet rigidite
  // partiel.
  for (i = p->calculs.t_part_en_cours; i < p->calculs.t_part->nzmax; i++)
  {
    ai[i] = 0;
    aj[i] = 0;
    ax[i] = 0.;
  }
    
  // Si le nombre de lignes du triplet rigidité partielle == 0, cela signifie
  // que tous les noeuds sont bloqués (cas d'une poutre sur deux appuis sans
  // discrétisation) Alors
  //   Initialisation d'un matrice de rigidité partielle vide.
  //   Convertion du triplet de rigidité complète en matrice.
  //   Factorisation de la matrice de rigidité partielle vide.
  //   Fin.
  // FinSi
  p->calculs.t_comp->nnz = p->calculs.t_comp_en_cours;
  if (p->calculs.t_part->nrow == 0)
  {
    cholmod_triplet *triplet_rigidite;
    
    BUGCRIT (triplet_rigidite = cholmod_allocate_triplet (0,
                                                          0,
                                                          0,
                                                          0,
                                                          CHOLMOD_REAL,
                                                          p->calculs.c),
             false,
             (gettext ("Erreur d'allocation mémoire.\n")); )
    BUGCRIT (p->calculs.m_part = cholmod_triplet_to_sparse (triplet_rigidite,
                                                            0,
                                                            p->calculs.c),
             false,
             (gettext ("Erreur d'allocation mémoire.\n"));
               cholmod_free_triplet (&triplet_rigidite, p->calculs.c); )
    cholmod_free_triplet (&triplet_rigidite, p->calculs.c);
    p->calculs.m_part->stype = 0;
    BUGCRIT (p->calculs.m_comp = cholmod_triplet_to_sparse (p->calculs.t_comp,
                                                            0,
                                                            p->calculs.c),
             false,
             (gettext ("Erreur d'allocation mémoire.\n")); )
    status = umfpack_di_symbolic (0.,
                                  0.,
                                  NULL,
                                  NULL,
                                  NULL,
                                  &symbolic,
                                  NULL,
                                  NULL);
    BUGCRIT (status == UMFPACK_OK,
             false,
             (gettext ("Erreur de calcul : %d\n"), status); )
    status = umfpack_di_numeric (NULL,
                                 NULL,
                                 NULL,
                                 symbolic,
                                 &p->calculs.numeric,
                                 NULL,
                                 NULL);
    BUGCRIT (status == UMFPACK_OK,
             false,
             (gettext ("Erreur de calcul : %d\n"), status);
               umfpack_di_free_symbolic (&symbolic); )
    umfpack_di_free_symbolic (&symbolic);
    
    return true;
  }
  
  BUGCRIT (p->calculs.m_part = cholmod_triplet_to_sparse (p->calculs.t_part,
                                                          0,
                                                          p->calculs.c),
           false,
           (gettext ("Erreur d'allocation mémoire.\n")); )
  BUGCRIT (p->calculs.m_comp = cholmod_triplet_to_sparse (p->calculs.t_comp,
                                                          0,
                                                          p->calculs.c),
           false,
           (gettext ("Erreur d'allocation mémoire.\n")); )
  
  // Factorisation de la matrice de rigidité partielle.
  BUGCRIT (p->calculs.ap = (int *) malloc (sizeof (int) *
                                                (p->calculs.t_part->ncol + 1)),
           false,
           (gettext ("Erreur d'allocation mémoire.\n")); )
  BUGCRIT (p->calculs.ai = (int *) malloc (sizeof (int) *
                                                       p->calculs.t_part->nnz),
           false,
           (gettext ("Erreur d'allocation mémoire.\n")); )
  BUGCRIT (p->calculs.ax = (double *) malloc (sizeof (double) *
                                                       p->calculs.t_part->nnz),
           false,
           (gettext ("Erreur d'allocation mémoire.\n")); )
  ai = (uint32_t *) p->calculs.t_part->i;
  aj = (uint32_t *) p->calculs.t_part->j;
  ax = (double *) p->calculs.t_part->x;
  status = umfpack_di_triplet_to_col ((int) p->calculs.t_part->nrow,
                                      (int) p->calculs.t_part->ncol,
                                      (int) p->calculs.t_part->nnz,
                                      (int *) ai,
                                      (int *) aj,
                                      ax,
                                      p->calculs.ap,
                                      p->calculs.ai,
                                      p->calculs.ax,
                                      NULL);
  BUGCRIT (status == UMFPACK_OK,
           false,
           (gettext ("Erreur de calcul : %d\n"), status); )
  status = umfpack_di_symbolic ((int) p->calculs.t_part->nrow,
                                (int) p->calculs.t_part->ncol,
                                p->calculs.ap,
                                p->calculs.ai,
                                p->calculs.ax,
                                &symbolic,
                                NULL,
                                NULL);
  BUGCRIT (status == UMFPACK_OK,
           false,
           (gettext ("Erreur de calcul : %d\n"), status); )
  status = umfpack_di_numeric (p->calculs.ap,
                               p->calculs.ai,
                               p->calculs.ax,
                               symbolic,
                               &p->calculs.numeric,
                               NULL,
                               NULL);
  if (status == UMFPACK_WARNING_singular_matrix)
  {
    printf (gettext ("Attention, matrice singulière.\n"
                "Il est possible que la modélisation ne soit pas stable.\n"));
  }
  else
  {
    BUGCRIT (status == UMFPACK_OK,
             false,
             (gettext ("Erreur de calcul : %d\n"), status);
               umfpack_di_free_symbolic (&symbolic); )
  }
  umfpack_di_free_symbolic (&symbolic);
  
  return true;
}


/**
 * \brief Calcul le moment hyperstatique correspondant à l'opposé du moment de
 *        la réaction à partir de la rotation au point A et B autour de l'axe
 *        Y.
 * \param infos : contient les paramètres ay, by, cy, kAy et kBy,
 * \param phia : rotation au noeud A,
 * \param phib : rotation au noeud B,
 * \param ma : moment au noeud A. Peut être NULL,
 * \param mb : moment au noeud B. Peut être NULL.
 * \return
 *   Succès : true.\n
 *   Échec : false :
 *     - infos == NULL.
 */
bool
EF_calculs_moment_hyper_y (Barre_Info_EF *infos,
                           double         phia,
                           double         phib,
                           double        *ma,
                           double        *mb)
{
  BUGPARAM (infos, "%p", infos, false)
  
  // Calcul des moments créés par les raideurs :\end{verbatim}
  // \texttt{Référence : \cite{RDM_articulation}}\begin{align*}
  //   M_{Ay} & = \frac{b_y \cdot \varphi_{By}+(c_y+k_{By}) \cdot
  //           \varphi_{Ay}}{(a_y+k_{Ay}) \cdot (c_y+k_{By})-b_y^2} \nonumber\\
  //   M_{By} & = \frac{b_y \cdot \varphi_{Ay}+(a_y+k_{Ay}) \cdot
  //          \varphi_{By}}{(a_y+k_{Ay}) \cdot (c_y+k_{By})-b_y^2} \end{align*}
  // \begin{verbatim}
  if ((errrel (infos->kAy, MAXDOUBLE)) && (errrel (infos->kBy, MAXDOUBLE)))
  {
    if (ma != NULL)
    {
      *ma = 0.;
    }
    if (mb != NULL)
    {
      *mb = 0.;
    }
  }
  else if (errrel (infos->kAy, MAXDOUBLE))
  {
    if (ma != NULL)
    {
      *ma = 0.;
    }
    if (mb != NULL)
    {
      *mb = phib / (infos->cy + infos->kBy);
    }
  }
  else if (errrel (infos->kBy, MAXDOUBLE))
  {
    if (ma != NULL)
    {
      *ma = phia / (infos->ay + infos->kAy);
    }
    if (mb != NULL)
    {
      *mb = 0.;
    }
  }
  else
  {
    if (ma != NULL)
    {
      *ma = (infos->by * phib + (infos->cy + infos->kBy) * phia) /
              ((infos->ay + infos->kAy) * (infos->cy + infos->kBy) -
               infos->by * infos->by);
    }
    if (mb != NULL)
    {
      *mb = (infos->by * phia + (infos->ay + infos->kAy) * phib) /
              ((infos->ay + infos->kAy) * (infos->cy + infos->kBy) -
               infos->by * infos->by);
    }
  }
  
  return true;
}


/**
 * \brief Calcul le moment hyperstatique correspondant à l'opposé du moment de
 *        la réaction à partir de la rotation au point A et B autour de l'axe
 *        Z.
 * \param infos : contient les paramètres az, bz, cz et kAz et kBz,
 * \param phia : rotation au noeud A,
 * \param phib : rotation au noeud B,
 * \param ma : moment au noeud A. Peut être NULL,
 * \param mb : moment au noeud B. Peut être NULL.
 * \return
 *   Succès : true.\n
 *   Échec : false :
 *     - infos == NULL
 */
bool
EF_calculs_moment_hyper_z (Barre_Info_EF *infos,
                           double         phia,
                           double         phib,
                           double        *ma,
                           double        *mb)
{
  BUGPARAM (infos, "%p", infos, false)
  
  // Calcul des moments créés par les raideurs :\end{verbatim}
  // \texttt{Référence : \cite{RDM_articulation}}\begin{align*}
  //   M_{Az} & = \frac{b_z \cdot \varphi_{Bz}+(c_z+k_{Bz}) \cdot
  //           \varphi_{Az}}{(a_z+k_{Az}) \cdot (c_z+k_{Bz})-b_z^2} \nonumber\\
  //   M_{Bz} & = \frac{b_z \cdot \varphi_{Az}+(a_z+k_{Az}) \cdot
  //   \varphi_{Bz}}{(a_z+k_{Az}) \cdot (c_z+k_{Bz})-b_z^2}\end{align*}
  //   \begin{verbatim}
  if ((errrel (infos->kAz, MAXDOUBLE)) && (errrel (infos->kBz, MAXDOUBLE)))
  {
    if (ma != NULL)
    {
      *ma = 0.;
    }
    if (mb != NULL)
    {
      *mb = 0.;
    }
  }
  else if (errrel (infos->kAz, MAXDOUBLE))
  {
    if (ma != NULL)
    {
      *ma = 0.;
    }
    if (mb != NULL)
    {
      *mb = phib / (infos->cz + infos->kBz);
    }
  }
  else if (errrel (infos->kBz, MAXDOUBLE))
  {
    if (ma != NULL)
    {
      *ma = phia / (infos->az + infos->kAz);
    }
    if (mb != NULL)
    {
      *mb = 0.;
    }
  }
  else
  {
    if (ma != NULL)
    {
      *ma = (infos->bz * phib + (infos->cz + infos->kBz) * phia) /
              ((infos->az + infos->kAz) * (infos->cz + infos->kBz) -
               infos->bz * infos->bz);
    }
    if (mb != NULL)
    {
      *mb = (infos->bz * phia + (infos->az + infos->kAz) * phib) /
              ((infos->az + infos->kAz) * (infos->cz + infos->kBz) -
               infos->bz * infos->bz);
    }
  }
  
  return true;
}


/**
 * \brief Détermine le résidu lors de la résolution du système matriciel pour
 *        obtenir les déplacements aux noeuds (A.x = b).
 * \param Ap : Matrice de rigidité,
 * \param Ai : Matrice de rigidité,
 * \param Ax : Matrice de rigidité,
 * \param b : Résultat à obtenir,
 * \param n : Nombre de ligne dans le vecteur b,
 * \param x : Résultat obtenu.
 * \return
 *   Succès : valeur du résidu.\n
 *   Échec : NAN :
 *     - Ap == NULL,
 *     - Ai == NULL,
 *     - Ax == NULL,
 *     - b == NULL,
 *     - x == NULL,
 *     - en cas d'erreur d'allocation mémoire.
 */
double
EF_calculs_resid (int    *Ap,
                  int    *Ai,
                  double *Ax,
                  double *b,
                  size_t  n,
                  double *x)
{
  int     p; // NS
  size_t  j, k;
  double  norm ;
  double *r;
  
  // Fonction tirée de la librarie UMFPACK, du fichier umfpack_di_demo.c
  BUGPARAM (Ap, "%p", Ap, NAN)
  BUGPARAM (Ai, "%p", Ai, NAN)
  BUGPARAM (Ax, "%p", Ax, NAN)
  BUGPARAM (b, "%p", b, NAN)
  BUGPARAM (x, "%p", x, NAN)
  BUGCRIT (r = (double *) malloc (sizeof (double) * n),
           NAN,
           (gettext ("Erreur d'allocation mémoire.\n")); )
  
  for (k = 0; k < n; k++)
  {
  	r[k] = -b[k];
  }
  for (j = 0; j < n; j++)
  {
    for (p = Ap[j]; p < Ap[j + 1]; p++)
    {
  		r[Ai[p]] += Ax[p] * x[j];
    }
  }
  norm = 0.;
  for (k = 0; k < n; k++)
  {
  	norm = std::max (fabs (r[k]), norm);
  }
  free (r);
  
  return norm;
}


/**
 * \brief Détermine à partir de la matrice de rigidité partielle factorisée les
 *        déplacements et les efforts dans les noeuds pour l'action demandée
 *        ainsi que la courbe des sollicitations dans les barres.
 * \param p : la variable projet,
 * \param action : l'action à résoudre.
 * \return
 *   Succès : true.\n
 *   Échec : false :
 *     - p == NULL,
 *     - action == NULL,
 *     - p->calculs.numeric == NULL && matrice_partielle->nrow != 0,
 *     - en cas d'erreur d'allocation mémoire.
 */
bool
EF_calculs_resoud_charge (Projet *p,
                          Action *action)
{
  cholmod_triplet *t_dep_tot, *t_dep_part;
  cholmod_triplet *t_for_part, *t_for_comp;
  cholmod_triplet *t_eff_loc_f, *t_eff_glo_i;
  cholmod_triplet *t_eff_loc_i, *t_eff_glo_f;
  uint32_t        *ai, *aj;
  double          *ax;
  uint32_t        *ai2, *aj2;
  double          *ax2;
  uint32_t        *ai3, *aj3;
  double          *ax3;
  uint32_t         i, j;
  uint8_t          k;
  cholmod_sparse  *sparse_tmp;
  double           minusone[2] = {-1., 0.}, one[2] = {1., 0.};
  
  std::list <EF_Barre *>::iterator it;
  
  BUGPARAM (p, "%p", p, false)
  BUGPARAM (action, "%p", action, false)
  BUGPARAM (p->calculs.numeric, "%p", p->calculs.numeric, false)
  BUGPARAM (p->calculs.m_part, "%p", p->calculs.m_part, false)
  BUGPARAM (p->calculs.m_part->nrow,
            "%zu",
            p->calculs.m_part->nrow != 0,
            false)
  
  // Création du triplet partiel et complet contenant les forces extérieures
  // sur les noeuds et initialisation des valeurs à 0. Le vecteur partiel sera 
  // utilisé dans l'équation finale : {F} = [K]{D}
  BUG (_1990_action_fonction_init (p, action), false)
  BUGCRIT (t_for_part = cholmod_allocate_triplet (p->calculs.m_part->nrow,
                                                  1,
                                                  p->calculs.m_part->nrow,
                                                  0,
                                                  CHOLMOD_REAL,
                                                  p->calculs.c),
           false,
           (gettext ("Erreur d'allocation mémoire.\n")); )
  ai = (uint32_t *) t_for_part->i;
  aj = (uint32_t *) t_for_part->j;
  ax = (double *) t_for_part->x;
  t_for_part->nnz = p->calculs.m_part->nrow;
  for (i = 0; i < t_for_part->nnz; i++)
  {
    ai[i] = i;
    aj[i] = 0;
    ax[i] = 0.;
  }
  BUGCRIT (t_for_comp = cholmod_allocate_triplet (p->calculs.m_comp->nrow,
                                                  1,
                                                  p->calculs.m_comp->nrow,
                                                  0,
                                                  CHOLMOD_REAL,
                                                  p->calculs.c),
           false,
           (gettext ("Erreur d'allocation mémoire.\n"));
             cholmod_free_triplet (&t_for_part, p->calculs.c); )
  ai3 = (uint32_t *) t_for_comp->i;
  aj3 = (uint32_t *) t_for_comp->j;
  ax3 = (double *) t_for_comp->x;
  t_for_comp->nnz = p->calculs.m_comp->nrow;
  for (i = 0; i < t_for_comp->nnz; i++)
  {
    ai3[i] = i;
    aj3[i] = 0;
    ax3[i] = 0.;
  }
  
  // Détermination des charges aux noeuds :
  //   Pour chaque charge dans l'action
  if (!_1990_action_charges_vide (action))
  {
    std::list <Charge *>::iterator it3;
    
    it3 = _1990_action_charges_renvoie (action)->begin ();
    
    while (it3 != _1990_action_charges_renvoie (action)->end ())
    {
      Charge         *charge = *it3;
      cholmod_sparse *s_eff_loc_i, *s_eff_glo_f;
      cholmod_sparse *s_eff_loc_f;
      
      switch (charge->type)
      {
        case CHARGE_NOEUD :
        {
  //   Si la charge en cours est une charge au noeud Alors
  //     On ajoute au vecteur des efforts les efforts aux noeuds directement
  //     saisis par l'utilisateur dans le repère global.
          Charge_Noeud *charge_d = (Charge_Noeud *) charge->data;
          
          std::list <EF_Noeud *>::iterator it2  = charge_d->noeuds.begin ();
          
          while (it2 != charge_d->noeuds.end ())
          {
            EF_Noeud *noeud = *it2;
            size_t    num = std::distance (
                              p->modele.noeuds.begin (),
                              std::find (p->modele.noeuds.begin (), 
                                         p->modele.noeuds.end (), 
                                         noeud));
            
            if (p->calculs.n_part[num][0] != UINT32_MAX)
            {
              ax[p->calculs.n_part[num][0]] += m_g (charge_d->fx);
            }
            if (p->calculs.n_part[num][1] != UINT32_MAX)
            {
              ax[p->calculs.n_part[num][1]] += m_g (charge_d->fy);
            }
            if (p->calculs.n_part[num][2] != UINT32_MAX)
            {
              ax[p->calculs.n_part[num][2]] += m_g (charge_d->fz);
            }
            if (p->calculs.n_part[num][3] != UINT32_MAX)
            {
              ax[p->calculs.n_part[num][3]] += m_g (charge_d->mx);
            }
            if (p->calculs.n_part[num][4] != UINT32_MAX)
            {
              ax[p->calculs.n_part[num][4]] += m_g (charge_d->my);
            }
            if (p->calculs.n_part[num][5] != UINT32_MAX)
            {
              ax[p->calculs.n_part[num][5]] += m_g (charge_d->mz);
            }
            ax3[p->calculs.n_comp[num][0]] += m_g (charge_d->fx);
            ax3[p->calculs.n_comp[num][1]] += m_g (charge_d->fy);
            ax3[p->calculs.n_comp[num][2]] += m_g (charge_d->fz);
            ax3[p->calculs.n_comp[num][3]] += m_g (charge_d->mx);
            ax3[p->calculs.n_comp[num][4]] += m_g (charge_d->my);
            ax3[p->calculs.n_comp[num][5]] += m_g (charge_d->mz);
            
            ++it2;
          }
          
          break;
        }
  //   Sinon Si la charge est une charge ponctuelle sur la barre Alors
        case CHARGE_BARRE_PONCTUELLE :
        {
          Charge_Barre_Ponctuelle *charge_d =
                                      (Charge_Barre_Ponctuelle *) charge->data;
          
          double       l;
          double       phiAy, phiBy, phiAz, phiBz; // Rotation sur appui
                       // lorsque la barre est isostatique
          double       MAx, MBx, MAy, MBy, MAz, MBz; // Moment opposé à la
                       // réaction d'appui
          EF_Noeud    *noeud_debut, *noeud_fin;
          uint16_t     pos; // numéro de l'élément dans la discrétisation
          
          std::list <EF_Barre *>::iterator it2 = charge_d->barres.begin ();
          
          while (it2 != charge_d->barres.end ())
          {
            double    a, b; // Position de la charge par rapport au début
                      // et à la fin de l'élément discrétisé
            double    debut_barre, fin_barre; // Début et fin de la barre
                      // discrétisée par rapport à la barre complète
            double    FAx, FBx; // Effort de compression au début et fin
                      // de l'élément discrétisé
            double    FAy_i, FAy_h, FBy_i, FBy_h; // Force opposée à la
                      // réaction d'appui
            double    FAz_i, FAz_h, FBz_i, FBz_h;
            EF_Barre *element = *it2;
            uint32_t  num;
            uint32_t  num_d, num_f;
          
            num = std::distance (p->modele.barres.begin (),
                                 std::find (p->modele.barres.begin (),
                                            p->modele.barres.end (),
                                            element));
      //   Convertion des efforts globaux en efforts locaux si nécessaire :
      //   \end{verbatim}\begin{center}
      //   $\{ F \}_{local} = [R]^T \cdot \{ F \}_{global}$\end{center}
      //   \begin{verbatim}
            if (!charge_d->repere_local)
            {
              BUGCRIT (t_eff_glo_i = cholmod_allocate_triplet (12,
                                                               1,
                                                               12,
                                                               0,
                                                               CHOLMOD_REAL,
                                                               p->calculs.c),
                       false,
                       (gettext ("Erreur d'allocation mémoire.\n"));
                         cholmod_free_triplet (&t_for_part, p->calculs.c);
                         cholmod_free_triplet (&t_for_comp, p->calculs.c); )
              ai2 = (uint32_t *) t_eff_glo_i->i;
              aj2 = (uint32_t *) t_eff_glo_i->j;
              ax2 = (double *) t_eff_glo_i->x;
              t_eff_glo_i->nnz = 12;
            }
            else
            {
              BUGCRIT (t_eff_loc_i = cholmod_allocate_triplet (12,
                                                               1,
                                                               12,
                                                               0,
                                                               CHOLMOD_REAL,
                                                               p->calculs.c),
                       false,
                       (gettext ("Erreur d'allocation mémoire.\n"));
                         cholmod_free_triplet (&t_for_part, p->calculs.c);
                         cholmod_free_triplet (&t_for_comp, p->calculs.c); )
              ai2 = (uint32_t *) t_eff_loc_i->i;
              aj2 = (uint32_t *) t_eff_loc_i->j;
              ax2 = (double *) t_eff_loc_i->x;
              t_eff_loc_i->nnz = 12;
            }
            ai2[0] = 0;   aj2[0] = 0;  ax2[0] = m_g (charge_d->fx);
            ai2[1] = 1;   aj2[1] = 0;  ax2[1] = m_g (charge_d->fy);
            ai2[2] = 2;   aj2[2] = 0;  ax2[2] = m_g (charge_d->fz);
            ai2[3] = 3;   aj2[3] = 0;  ax2[3] = m_g (charge_d->mx);
            ai2[4] = 4;   aj2[4] = 0;  ax2[4] = m_g (charge_d->my);
            ai2[5] = 5;   aj2[5] = 0;  ax2[5] = m_g (charge_d->mz);
            ai2[6] = 6;   aj2[6] = 0;  ax2[6] = 0.;
            ai2[7] = 7;   aj2[7] = 0;  ax2[7] = 0.;
            ai2[8] = 8;   aj2[8] = 0;  ax2[8] = 0.;
            ai2[9] = 9;   aj2[9] = 0;  ax2[9] = 0.;
            ai2[10] = 10; aj2[10] = 0; ax2[10] = 0.;
            ai2[11] = 11; aj2[11] = 0; ax2[11] = 0.;
            if (!charge_d->repere_local)
            {
              cholmod_sparse *s_eff_glo_i;
              
              BUGCRIT (s_eff_glo_i = cholmod_triplet_to_sparse (t_eff_glo_i,
                                                                0,
                                                                p->calculs.c),
                       false,
                       (gettext ("Erreur d'allocation mémoire.\n"));
                         cholmod_free_triplet (&t_for_part, p->calculs.c);
                         cholmod_free_triplet (&t_for_comp, p->calculs.c);
                         cholmod_free_triplet (&t_eff_glo_i, p->calculs.c); )
              cholmod_free_triplet (&t_eff_glo_i, p->calculs.c);
              BUGCRIT (s_eff_loc_i = cholmod_ssmult (element->m_rot_t,
                                                     s_eff_glo_i,
                                                     0,
                                                     1,
                                                     0,
                                                     p->calculs.c),
                       false,
                       (gettext ("Erreur d'allocation mémoire.\n"));
                         cholmod_free_triplet (&t_for_part, p->calculs.c);
                         cholmod_free_triplet (&t_for_comp, p->calculs.c);
                         cholmod_free_sparse (&s_eff_glo_i, p->calculs.c); )
              cholmod_free_sparse (&s_eff_glo_i, p->calculs.c);
              BUGCRIT (t_eff_loc_i = cholmod_sparse_to_triplet (s_eff_loc_i,
                                                                p->calculs.c),
                       false,
                       (gettext ("Erreur d'allocation mémoire.\n"));
                         cholmod_free_triplet (&t_for_part, p->calculs.c);
                         cholmod_free_triplet (&t_for_comp, p->calculs.c);
                         cholmod_free_sparse (&s_eff_loc_i, p->calculs.c); )
              cholmod_free_sparse (&s_eff_loc_i, p->calculs.c);
              ax2 = (double *) t_eff_loc_i->x;
            }
            // A ce stade ax2 pointent vers les charges dans le repère local
            
#define FREE_ALL cholmod_free_triplet (&t_for_part, p->calculs.c); \
  cholmod_free_triplet (&t_for_comp, p->calculs.c); \
  cholmod_free_triplet (&t_eff_loc_i, p->calculs.c);
           
      //   Détermination des deux noeuds se situant directement avant et
      //   après la charge ponctuelle (est différent des deux noeuds
      //   définissant la barre si elle est discrétisée).
            if (element->discretisation_element == 0)
            /* Pas de discrétisation */
            {
              pos = 0;
              noeud_debut = element->noeud_debut;
              noeud_fin = element->noeud_fin;
            }
            else
            // On cherche le noeud de départ et le noeud de fin
            {
              pos = 0;
              l = -1.;
              // On regarde pour chaque noeud intermédiaire si la position de
              // la charge devient inférieure à la distance entre le noeud de
              // départ et le noeud intermédiaire.
              while ((pos <= element->discretisation_element) &&
                     (l < m_g (charge_d->position)))
              {
                if (pos == element->discretisation_element)
                {
                  l = EF_noeuds_distance (element->noeud_fin,
                                          element->noeud_debut);
                }
                else
                {
                  std::list <EF_Noeud *>::iterator it_t;
                  
                  it_t = element->nds_inter.begin ();
                  std::advance (it_t, pos);
                  l = EF_noeuds_distance (*it_t, element->noeud_debut);
                }
                BUG (!isnan (l), false, FREE_ALL)
                pos++;
              }
              pos--;
              // Alors la position de la charge est compris entre le début du
              // noeud et le premier noeud intermédiaire.
              if (pos == 0)
              {
                noeud_debut = element->noeud_debut;
                noeud_fin = *element->nds_inter.begin ();
              }
              // Alors la position de la charge est compris entre le dernier
              // noeud intermédiaire et le noeud de fin de la barre
              else if (pos == element->discretisation_element)
              {
                std::list <EF_Noeud *>::iterator it_t;
                
                it_t = element->nds_inter.begin ();
                std::advance (it_t, pos - 1U);
                noeud_debut = *it_t;
                noeud_fin = element->noeud_fin;
              }
              else
              {
                std::list <EF_Noeud *>::iterator it_t;
                
                it_t = element->nds_inter.begin ();
                std::advance (it_t, pos - 1U);
                noeud_debut = *it_t;
                std::next (it_t);
                noeud_fin = *it_t;
              }
            }
            num_d = std::distance (p->modele.noeuds.begin (),
                                   std::find (p->modele.noeuds.begin (), 
                                              p->modele.noeuds.end (), 
                                              noeud_debut));
            num_f = std::distance (p->modele.noeuds.begin (), 
                                   std::find (p->modele.noeuds.begin (),
                                              p->modele.noeuds.end (),     
                                              noeud_fin));
            debut_barre = EF_noeuds_distance (noeud_debut,
                                              element->noeud_debut);
            BUG (!isnan (debut_barre), false, FREE_ALL)
            a = m_g (charge_d->position) - debut_barre;
            fin_barre = EF_noeuds_distance (noeud_fin, element->noeud_debut);
            BUG (!isnan (fin_barre), false, FREE_ALL)
            l = fabs (fin_barre - debut_barre);
            b = l - a;
            
      //   Détermination des moments mx de rotation :
            BUG (EF_charge_barre_ponctuelle_mx (element,
                                                pos,
                                                a,
                                                &(element->info_EF[pos]),
                                                ax2[3],
                                                &MAx,
                                                &MBx),
                 false,
                 FREE_ALL)
            
      //   Détermination de la rotation y et z aux noeuds de l'élément
      //   discrétisé en le supposant isostatique :
            BUG (EF_charge_barre_ponctuelle_def_ang_iso_y (element,
                                                           pos,
                                                           a,
                                                           ax2[2],
                                                           ax2[4],
                                                           &phiAy,
                                                           &phiBy),
                 false,
                 FREE_ALL)
            BUG (EF_charge_barre_ponctuelle_def_ang_iso_z (element,
                                                           pos,
                                                           a,
                                                           ax2[1],
                                                           ax2[5],
                                                           &phiAz,
                                                           &phiBz),
                 false,
                 FREE_ALL)
            
      //   Calcul des moments créés par les raideurs :
            BUG (EF_calculs_moment_hyper_y (&(element->info_EF[pos]),
                                            phiAy,
                                            phiBy,
                                            &MAy,
                                            &MBy),
                 false,
                 FREE_ALL)
            BUG (EF_calculs_moment_hyper_z (&(element->info_EF[pos]),
                                            phiAz,
                                            phiBz,
                                            &MAz,
                                            &MBz),
                 false,
                 FREE_ALL)
            
      //   Réaction d'appui sur les noeuds :\end{verbatim}\begin{align*}
            // F_{Ax} & = F_x \cdot \frac{\int_a^l \frac{1}{E \cdot S(x)}}
            //            {\int_0^l \frac{1}{E \cdot S(x)}}\nonumber\\
            // F_{Bx} & = F_x - F_{Ax}\nonumber\\
            // F_{Ay_i}   & = \frac{F_y \cdot b}{l}-\frac{M_z}{l}\nonumber\\
            // F_{Ay_h} & = \frac{M_{Bz}+M_{Az}}{l}\nonumber\\
            // F_{By_i}   & = \frac{F_y \cdot a}{l}+\frac{M_z}{l}\nonumber\\
            // F_{By_h} & = -\frac{M_{Bz}+M_{Az}}{l}\nonumber\\
            // F_{Az_i}   & = \frac{F_z \cdot b}{l}+\frac{M_y}{l}\nonumber\\
            // F_{Az_h} & = -\frac{M_{By}+M_{Ay}}{l}\nonumber\\
            // F_{Bz_i}   & = \frac{F_z \cdot a}{l}-\frac{M_y}{l}\nonumber\\
            // F_{Bz_h} & = \frac{M_{By}+M_{Ay}}{l}\end{align*}
            // \begin{verbatim}
            FAx = ax2[0] * EF_sections_es_l (element, pos, 0, l) /
                    EF_sections_es_l (element, pos, a, l);
            BUG (!isnan (FAx), false, FREE_ALL)
            FBx = ax2[0] - FAx;
            FAy_i = ax2[1] * b / l - ax2[5] / l;
            FAy_h = (MBz + MAz) / l;
            FBy_i = ax2[1] * a / l + ax2[5] / l;
            FBy_h = -(MBz + MAz) / l;
            FAz_i = ax2[2] * b / l + ax2[4] / l;
            FAz_h = -(MBy + MAy) / l;
            FBz_i = ax2[2] * a / l - ax2[4] / l;
            FBz_h = (MBy + MAy) / l;
            
      //   Détermination des fonctions des efforts dus à la charge (x, a et
      //   l sont calculés par rapport à l'élément discrétisé et non pour
      //   toute la barre). Pour cela on calcule la sollicitation due au cas
      //   isostatique puis on ajoute la sollicitation due à l'éventuel
      //   encastrement (MAx, MBx, MAy, MAz, MBy, MBz) :\end{verbatim}
      //   \begin{align*}
            // N(x) & = -F_{Ax} & &\textrm{ pour x de 0 à a}\nonumber\\
            // N(x) & = F_{Bx} & &\textrm{ pour x de a à l}\nonumber\\
            
            BUG (common_fonction_ajout_poly (
                   _1990_action_efforts_renvoie (action, 0, num),
                   0.,
                   a,
                   -FAx,
                   0., 0., 0., 0., 0., 0.,
                   debut_barre),
                 false,
                 FREE_ALL)
            BUG (common_fonction_ajout_poly (
                   _1990_action_efforts_renvoie (action, 0, num),
                   a,
                   l,
                   FBx,
                   0., 0., 0., 0., 0., 0.,
                   debut_barre),
                 false,
                 FREE_ALL)
            
            // T_y(x) & = -F_{Ay_i} - F_{Ay_h} & &\textrm{ pour x de 0 à a}
            // \nonumber\\
            // T_y(x) & =  F_{By_i} + F_{By_h} & &\textrm{ pour x de a à l}
            // \nonumber\\
            
            BUG (common_fonction_ajout_poly (
                   _1990_action_efforts_renvoie (action, 1, num),
                   0.,
                   a,
                   -FAy_i - FAy_h,
                   0., 0., 0., 0., 0., 0.,
                   debut_barre),
                 false,
                 FREE_ALL)
            BUG (common_fonction_ajout_poly (
                   _1990_action_efforts_renvoie (action, 1, num),
                   a,
                   l,
                   FBy_i + FBy_h,
                   0., 0., 0., 0., 0., 0.,
                   debut_barre),
                 false,
                 FREE_ALL)
            
            // T_z(x) & = -F_{Az_i} - F_{Az_h} & &\textrm{ pour x de 0 à a}
            // \nonumber\\
            // T_z(x) & =  F_{Bz_i} + F_{Bz_h} & &\textrm{ pour x de a à l}
            // \nonumber\\
            
            BUG (common_fonction_ajout_poly (
                   _1990_action_efforts_renvoie (action, 2, num),
                   0.,
                   a,
                   -FAz_i - FAz_h,
                   0., 0., 0., 0., 0., 0.,
                   debut_barre),
                 false,
                 FREE_ALL)
            BUG (common_fonction_ajout_poly (
                   _1990_action_efforts_renvoie (action, 2, num),
                   a,
                   l,
                   FBz_i + FBz_h,
                   0., 0., 0., 0., 0., 0.,
                   debut_barre),
                 false,
                 FREE_ALL)
            
            // M_x(x) & = -M_{Ax} & &\textrm{ pour x de 0 à a}\nonumber\\
            // M_x(x) & = M_{Bx} & &\textrm{ pour x de a à l}\nonumber\\
            
            BUG (common_fonction_ajout_poly (
                   _1990_action_efforts_renvoie (action, 3, num),
                   0.,
                   a,
                   -MAx,
                   0., 0., 0., 0., 0., 0.,
                   debut_barre),
                 false,
                 FREE_ALL)
            BUG (common_fonction_ajout_poly (
                   _1990_action_efforts_renvoie (action, 3, num),
                   a,
                   l,
                   MBx,
                   0., 0., 0., 0., 0., 0.,
                   debut_barre),
                 false,
                 FREE_ALL)
            
            // M_y(x) & = -M_{Ay} - (F_{Az_i}+F_{Az_h}) \cdot x & 
            //            &\textrm{ pour x de 0 à a}\nonumber\\
            // M_y(x) & = F_{Bz_i} \cdot L - M_{Ay} + (F_{Bz_i} + F_{Bz_h})
            //            \cdot x & &\textrm{ pour x de a à l}\nonumber\\
            
            BUG (common_fonction_ajout_poly (
                   _1990_action_efforts_renvoie (action, 4, num),
                   0.,
                   a,
                   -MAy,
                   -FAz_i - FAz_h,
                   0., 0., 0., 0., 0.,
                   debut_barre),
                 false,
                 FREE_ALL)
            BUG (common_fonction_ajout_poly (
                   _1990_action_efforts_renvoie (action, 4, num),
                   a,
                   l,
                   -FBz_i * l - MAy,
                   FBz_i + FBz_h,
                   0., 0., 0., 0., 0.,
                   debut_barre),
                 false,
                 FREE_ALL)
            
            // M_z(x) & = -M_{Az} + (F_{Ay_i}+F_{Ay_h}) \cdot x &
            //            &\textrm{ pour x de 0 à a}\nonumber\\
            // M_z(x) & = F_{By_i} \cdot L - M_{Az} - (F_{By_i} + F_{By_h})
            //            \cdot x & &\textrm{ pour x de a à l}
            
            BUG (common_fonction_ajout_poly (
                   _1990_action_efforts_renvoie (action, 5, num),
                   0.,
                   a,
                   -MAz,
                   FAy_i + FAy_h,
                   0., 0., 0., 0., 0.,
                   debut_barre),
                 false,
                 FREE_ALL)
            BUG (common_fonction_ajout_poly (
                   _1990_action_efforts_renvoie (action, 5, num),
                   a,
                   l,
                   FBy_i * l - MAz,
                   -FBy_i - FBy_h,
                   0., 0., 0., 0., 0.,
                   debut_barre),
                 false,
                 FREE_ALL)
            // \end{align*}\begin{verbatim}
            
            
      //   Détermination des fonctions de déformation et rotation (cas
      //   isostatique + encastrement) :
      //   \begin{align*}
            BUG (EF_charge_barre_ponctuelle_fonc_rx (
                   _1990_action_rotation_renvoie (action, 0, num),
                   element,
                   pos,
                   a,
                   MAx,
                   MBx),
                 false,
                 FREE_ALL)
            BUG (EF_charge_barre_ponctuelle_fonc_ry (
                   _1990_action_rotation_renvoie (action, 1, num),
                   _1990_action_deformation_renvoie (action, 2, num),
                   element,
                   pos,
                   a,
                   ax2[2],
                   ax2[4],
                   -MAy,
                   -MBy),
                 false,
                 FREE_ALL)
            BUG (EF_charge_barre_ponctuelle_fonc_rz (
                   _1990_action_rotation_renvoie (action, 2, num),
                   _1990_action_deformation_renvoie (action, 1, num),
                   element,
                   pos,
                   a,
                   ax2[1],
                   ax2[5],
                   -MAz,
                   -MBz),
                 false,
                 FREE_ALL)
            BUG (EF_charge_barre_ponctuelle_n (
                   _1990_action_deformation_renvoie (action, 0, num),
                   element,
                   pos,
                   a,
                   FAx,
                   FBx),
                 false,
                 FREE_ALL)
            
            cholmod_free_triplet (&t_eff_loc_i, p->calculs.c);
            
#undef FREE_ALL
            
      //   Convertion des réactions d'appuis locales dans le repère global :
      //   \end{verbatim}\begin{center}
      //     $\{ R \}_{global} = [K] \cdot \{ F \}_{local}$\end{center}
      //     \begin{verbatim}
            BUGCRIT (t_eff_loc_f = cholmod_allocate_triplet (12,
                                                             1,
                                                             12,
                                                             0,
                                                             CHOLMOD_REAL,
                                                             p->calculs.c),
                     false,
                     (gettext ("Erreur d'allocation mémoire.\n"));
                       cholmod_free_triplet (&t_for_part, p->calculs.c);
                       cholmod_free_triplet (&t_for_comp, p->calculs.c); )
            ai2 = (uint32_t *) t_eff_loc_f->i;
            aj2 = (uint32_t *) t_eff_loc_f->j;
            ax2 = (double *) t_eff_loc_f->x;
            t_eff_loc_f->nnz = 12;
            ai2[0] = 0;   aj2[0] = 0;  ax2[0] = FAx;
            ai2[1] = 1;   aj2[1] = 0;  ax2[1] = FAy_i + FAy_h;
            ai2[2] = 2;   aj2[2] = 0;  ax2[2] = FAz_i + FAz_h;
            ai2[3] = 3;   aj2[3] = 0;  ax2[3] = MAx;
            ai2[4] = 4;   aj2[4] = 0;  ax2[4] = MAy;
            ai2[5] = 5;   aj2[5] = 0;  ax2[5] = MAz;
            ai2[6] = 6;   aj2[6] = 0;  ax2[6] = FBx;
            ai2[7] = 7;   aj2[7] = 0;  ax2[7] = FBy_i + FBy_h;
            ai2[8] = 8;   aj2[8] = 0;  ax2[8] = FBz_i + FBz_h;
            ai2[9] = 9;   aj2[9] = 0;  ax2[9] = MBx;
            ai2[10] = 10; aj2[10] = 0; ax2[10] = MBy;
            ai2[11] = 11; aj2[11] = 0; ax2[11] = MBz;
            BUGCRIT (s_eff_loc_f = cholmod_triplet_to_sparse (t_eff_loc_f,
                                                              0,
                                                              p->calculs.c),
                     false,
                     (gettext ("Erreur d'allocation mémoire.\n"));
                       cholmod_free_triplet (&t_for_part, p->calculs.c);
                       cholmod_free_triplet (&t_for_comp, p->calculs.c);
                       cholmod_free_triplet (&t_eff_loc_f, p->calculs.c); )
            cholmod_free_triplet (&t_eff_loc_f, p->calculs.c);
            BUGCRIT (s_eff_glo_f = cholmod_ssmult (element->m_rot,
                                                   s_eff_loc_f,
                                                   0,
                                                   1,
                                                   0,
                                                   p->calculs.c),
                     false,
                     (gettext ("Erreur d'allocation mémoire.\n"));
                       cholmod_free_triplet (&t_for_part, p->calculs.c);
                       cholmod_free_triplet (&t_for_comp, p->calculs.c);
                       cholmod_free_sparse (&s_eff_loc_f, p->calculs.c); )
            cholmod_free_sparse (&s_eff_loc_f, p->calculs.c);
            BUGCRIT (t_eff_glo_f = cholmod_sparse_to_triplet (s_eff_glo_f,
                                                              p->calculs.c),
                     false,
                     (gettext ("Erreur d'allocation mémoire.\n"));
                       cholmod_free_triplet (&t_for_part, p->calculs.c);
                       cholmod_free_triplet (&t_for_comp, p->calculs.c);
                       cholmod_free_sparse (&(s_eff_glo_f), p->calculs.c); )
            cholmod_free_sparse (&(s_eff_glo_f), p->calculs.c);
            ai2 = (uint32_t *) t_eff_glo_f->i;
            ax2 = (double *) t_eff_glo_f->x;
            
      //   Ajout des moments et les efforts dans le vecteur des forces aux
      //   noeuds {F}
            for (i = 0; i < 12; i++)
            {
              if (ai2[i] < 6)
              {
                if (p->calculs.n_part[num_d][ai2[i]] != UINT32_MAX)
                {
                  ax[p->calculs.n_part[num_d][ai2[i]]] += ax2[i];
                }
                ax3[p->calculs.n_comp[num_d][ai2[i]]] += ax2[i];
              }
              else
              {
                if (p->calculs.n_part[num_f][ai2[i] - 6] != UINT32_MAX)
                {
                  ax[p->calculs.n_part[num_f][ai2[i] - 6]] += ax2[i];
                }
                ax3[p->calculs.n_comp[num_f][ai2[i] - 6]] += ax2[i];
              }
            }
            cholmod_free_triplet (&t_eff_glo_f, p->calculs.c);
            
            ++it2;
          }
          
          break;
        }
  //   Sinon Si la charge est une charge répartie uniforme sur la barre Alors
        case CHARGE_BARRE_REPARTIE_UNIFORME :
        {
          double   xx, yy, zz, l;
          uint16_t j_d, j_f; // Numéro de l'élément dans la discrétisation
          
          Charge_Barre_Repartie_Uniforme *charge_d =
                               (Charge_Barre_Repartie_Uniforme *) charge->data;
          std::list <EF_Barre *>::iterator it2 = charge_d->barres.begin ();
          
          while (it2 != charge_d->barres.end ())
          {
            double    ll;
            EF_Barre *element = *it2;
            uint16_t pos;
            
            uint32_t num = std::distance (
                             p->modele.barres.begin (),
                             std::find (p->modele.barres.begin (), 
                                        p->modele.barres.end (), 
                                        element));
            
      //   Convertion des efforts globaux en efforts locaux si nécessaire :
      //   \end{verbatim}\begin{center}
      //     $\{ F \}_{local} = [R_e]^T \cdot \{ F \}_{global}$\end{center}
      //   \begin{verbatim}
            ll = EF_noeuds_distance_x_y_z (element->noeud_debut,
                                           element->noeud_fin,
                                           &xx,
                                           &yy,
                                           &zz);
            BUG (!isnan (ll),
                 false,
                 cholmod_free_triplet (&t_for_part, p->calculs.c);
                   cholmod_free_triplet (&t_for_comp, p->calculs.c); )
            if (!charge_d->repere_local)
            {
              BUGCRIT (t_eff_glo_i = cholmod_allocate_triplet (12,
                                                               1,
                                                               12,
                                                               0,
                                                               CHOLMOD_REAL,
                                                               p->calculs.c),
                       false,
                       (gettext ("Erreur d'allocation mémoire.\n"));
                         cholmod_free_triplet (&t_for_part, p->calculs.c);
                         cholmod_free_triplet (&t_for_comp, p->calculs.c); )
              ai2 = (uint32_t *) t_eff_glo_i->i;
              aj2 = (uint32_t *) t_eff_glo_i->j;
              ax2 = (double *) t_eff_glo_i->x;
              t_eff_glo_i->nnz = 12;
            }
            else
            {
              BUGCRIT (t_eff_loc_i = cholmod_allocate_triplet (12,
                                                               1,
                                                               12,
                                                               0,
                                                               CHOLMOD_REAL,
                                                               p->calculs.c),
                       false,
                       (gettext ("Erreur d'allocation mémoire.\n")); )
              ai2 = (uint32_t *) t_eff_loc_i->i;
              aj2 = (uint32_t *) t_eff_loc_i->j;
              ax2 = (double *) t_eff_loc_i->x;
              t_eff_loc_i->nnz = 12;
            }
            if (charge_d->projection)
            {
              ai2[0] = 0; aj2[0] = 0;
              ax2[0] = m_g (charge_d->fx) * sqrt (yy * yy + zz * zz) / ll;
              ai2[1] = 1; aj2[1] = 0;
              ax2[1] = m_g (charge_d->fy) * sqrt (xx * xx + zz * zz) / ll;
              ai2[2] = 2; aj2[2] = 0;
              ax2[2] = m_g (charge_d->fz) * sqrt (xx * xx + yy * yy) / ll;
              ai2[3] = 3; aj2[3] = 0;
              ax2[3] = m_g (charge_d->mx) * sqrt (yy * yy + zz * zz) / ll;
              ai2[4] = 4; aj2[4] = 0;
              ax2[4] = m_g (charge_d->my) * sqrt (xx * xx + zz * zz) / ll;
              ai2[5] = 5; aj2[5] = 0;
              ax2[5] = m_g (charge_d->mz) * sqrt (xx * xx + yy * yy) / ll;
            }
            else
            {
              ai2[0] = 0; aj2[0] = 0; ax2[0] = m_g (charge_d->fx);
              ai2[1] = 1; aj2[1] = 0; ax2[1] = m_g (charge_d->fy);
              ai2[2] = 2; aj2[2] = 0; ax2[2] = m_g (charge_d->fz);
              ai2[3] = 3; aj2[3] = 0; ax2[3] = m_g (charge_d->mx);
              ai2[4] = 4; aj2[4] = 0; ax2[4] = m_g (charge_d->my);
              ai2[5] = 5; aj2[5] = 0; ax2[5] = m_g (charge_d->mz);
            }
            ai2[6]  = 6;  aj2[6]  = 0; ax2[6]  = 0.;
            ai2[7]  = 7;  aj2[7]  = 0; ax2[7]  = 0.;
            ai2[8]  = 8;  aj2[8]  = 0; ax2[8]  = 0.;
            ai2[9]  = 9;  aj2[9]  = 0; ax2[9]  = 0.;
            ai2[10] = 10; aj2[10] = 0; ax2[10] = 0.;
            ai2[11] = 11; aj2[11] = 0; ax2[11] = 0.;
            if (!charge_d->repere_local)
            {
              cholmod_sparse *s_eff_glo_i;
              
              BUGCRIT (s_eff_glo_i = cholmod_triplet_to_sparse (t_eff_glo_i,
                                                                0,
                                                                p->calculs.c),
                       false,
                       (gettext ("Erreur d'allocation mémoire.\n"));
                         cholmod_free_triplet (&t_for_part, p->calculs.c);
                         cholmod_free_triplet (&t_for_comp, p->calculs.c);
                         cholmod_free_triplet (&t_eff_glo_i, p->calculs.c); )
              cholmod_free_triplet (&t_eff_glo_i, p->calculs.c);
              BUGCRIT (s_eff_loc_i = cholmod_ssmult (element->m_rot_t,
                                                     s_eff_glo_i,
                                                     0,
                                                     1,
                                                     0,
                                                     p->calculs.c),
                       false,
                       (gettext ("Erreur d'allocation mémoire.\n"));
                         cholmod_free_triplet (&t_for_part, p->calculs.c);
                         cholmod_free_triplet (&t_for_comp, p->calculs.c);
                         cholmod_free_sparse (&s_eff_glo_i, p->calculs.c); )
              cholmod_free_sparse (&(s_eff_glo_i), p->calculs.c);
              BUGCRIT (t_eff_loc_i = cholmod_sparse_to_triplet (s_eff_loc_i,
                                                                p->calculs.c),
                       false,
                       (gettext ("Erreur d'allocation mémoire.\n"));
                         cholmod_free_triplet (&t_for_part, p->calculs.c);
                         cholmod_free_triplet (&t_for_comp, p->calculs.c);
                         cholmod_free_sparse (&s_eff_loc_i, p->calculs.c); )
              cholmod_free_sparse (&(s_eff_loc_i), p->calculs.c);
              ax2 = (double *) t_eff_loc_i->x;
            }
            // A ce stade ax2 pointent vers les charges dans le repère local
            
#define FREE_ALL cholmod_free_triplet (&t_for_part, p->calculs.c); \
  cholmod_free_triplet (&t_for_comp, p->calculs.c); \
  cholmod_free_triplet (&t_eff_loc_i, p->calculs.c);
      
      //   Détermination des deux barres discrétisées (j_d et j_f) qui
      //   entoure la charge répartie.
            if (element->discretisation_element == 0)
            // Pas de discrétisation
            {
              j_d = 0;
              j_f = 0;
            }
            else
            // On cherche le noeud de départ et le noeud de fin
            {
              j_d = 0;
              l = -1.;
              // On regarde pour chaque noeud intermédiaire si la position de
              // la charge devient inférieure à la distance entre le noeud de
              // départ et le noeud intermédiaire.
              while ((j_d <= element->discretisation_element) && 
                     (l < m_g (charge_d->a)))
              {
                if (j_d == element->discretisation_element)
                {
                  l = EF_noeuds_distance (element->noeud_fin,
                                          element->noeud_debut);
                }
                else
                {
                  std::list <EF_Noeud *>::iterator it_t;
                  
                  it_t = element->nds_inter.begin ();
                  std::advance (it_t, j_d);
                  l = EF_noeuds_distance (*it_t, element->noeud_debut);
                }
                BUG (!isnan (l), false, FREE_ALL)
                j_d++;
              }
              j_d--;
              j_f = j_d;
              l = -1.;
              // On regarde pour chaque noeud intermédiaire si la position de
              // la charge devient inférieur à la distance entre le noeud de
              // départ et le noeud intermédiaire.
              while ((j_f <= element->discretisation_element) &&
                     (l < ll - m_g (charge_d->b)))
              {
                if (j_f == element->discretisation_element)
                {
                  l = EF_noeuds_distance (element->noeud_fin,
                                          element->noeud_debut);
                }
                else
                {
                  std::list <EF_Noeud *>::iterator it_t;
                  
                  it_t = element->nds_inter.begin ();       
                  std::advance (it_t, j_f);
                  l = EF_noeuds_distance (*it_t, element->noeud_debut);
                }
                BUG (!isnan (l), false, FREE_ALL)
                j_f++;
              }
              j_f--;
            }
            
      //   Pour chaque barre comprise entre j_d et j_f inclus Faire
            for (pos = j_d; pos <= j_f; pos++)
            {
              double    a, b; // Position de la charge par rapport au
                        // début et à la fin de l'élément discrétisé
              double    debut_barre, fin_barre; // Début et fin de la
                        // barre discrétisée par rapport à la barre complète
              double    phiAy, phiBy, phiAz, phiBz; // Rotation sur appui
                        // lorsque la barre est isostatique
              double    MAx, MBx, MAy, MBy, MAz, MBz; // Moments créés par
                        // la raideur
              double    FAx, FBx;
              double    FAy_i, FAy_h, FBy_i, FBy_h; // Réactions d'appui
              double    FAz_i, FAz_h, FBz_i, FBz_h;
              uint32_t *ai4, *aj4;
              double   *ax4;
              EF_Noeud *noeud_debut, *noeud_fin;
              uint32_t  num_d, num_f;
              
              if (pos == 0)
              {
                noeud_debut = element->noeud_debut;
              }
              else
              {
                std::list <EF_Noeud *>::iterator it_t;
                
                it_t = element->nds_inter.begin ();
                std::advance (it_t, pos - 1U);
                noeud_debut = *it_t;
              }
              if (pos == element->discretisation_element)
              {
                noeud_fin = element->noeud_fin;
              }
              else
              {
                std::list <EF_Noeud *>::iterator it_t;
                
                it_t = element->nds_inter.begin ();
                std::advance (it_t, pos);
                noeud_fin = *it_t;
              }
              debut_barre = EF_noeuds_distance (noeud_debut,
                                                element->noeud_debut);
              BUG (!isnan (debut_barre), false, FREE_ALL)
              if (pos == j_d)
              {
                a = m_g (charge_d->a) - debut_barre;
              }
              else
              {
                a = 0.;
              }
              fin_barre = EF_noeuds_distance (noeud_fin,
                                              element->noeud_debut);
              BUG (!isnan (fin_barre), false, FREE_ALL)
              l = fabs (fin_barre - debut_barre);
              if (pos == j_f)
              {
                fin_barre = EF_noeuds_distance (noeud_fin,
                                                element->noeud_fin);
                b = m_g (charge_d->b) - fin_barre;
              }
              else
              {
                b = 0.;
              }
              
              num_d = std::distance (p->modele.noeuds.begin (),
                                     std::find (p->modele.noeuds.begin (), 
                                                p->modele.noeuds.end (),
                                                noeud_debut));
              num_f = std::distance (p->modele.noeuds.begin (),
                                     std::find (p->modele.noeuds.begin (),
                                                p->modele.noeuds.end (),
                                                noeud_fin));
              
      //     Détermination des moments mx de rotation :
              BUG (EF_charge_barre_repartie_uniforme_mx (element,
                                                         pos,
                                                         a,
                                                         b,
                                                      &(element->info_EF[pos]),
                                                         ax2[3],
                                                         &MAx,
                                                         &MBx),
                   false,
                   FREE_ALL)
              
      //     Détermination de la rotation y et z aux noeuds de l'élément
      //     discrétisé en le supposant isostatique :
              BUG (EF_charge_barre_repartie_uniforme_def_ang_iso_y (element,
                                                                    pos,
                                                                    a,
                                                                    b,
                                                                    ax2[2],
                                                                    ax2[4],
                                                                    &phiAy,
                                                                    &phiBy),
                   false,
                   FREE_ALL)
              BUG (EF_charge_barre_repartie_uniforme_def_ang_iso_z (element,
                                                                    pos,
                                                                    a,
                                                                    b,
                                                                    ax2[1],
                                                                    ax2[5],
                                                                    &phiAz,
                                                                    &phiBz),
                   false,
                   FREE_ALL)
              
      //     Calcul des moments créés par les raideurs :
              BUG (EF_calculs_moment_hyper_y (&(element->info_EF[pos]),
                                              phiAy,
                                              phiBy,
                                              &MAy,
                                              &MBy),
                   false,
                   FREE_ALL)
              BUG (EF_calculs_moment_hyper_z (&(element->info_EF[pos]),
                                              phiAz,
                                              phiBz,
                                              &MAz,
                                              &MBz),
                   false,
                   FREE_ALL)
            
      //     Réaction d'appui sur les noeuds (X représente la position de la
      //     résultante pour une force équivalente :\end{verbatim}
      //     \begin{align*}
        // F_{Ax} & = F_x \cdot (L-a-b) \cdot \frac{\int_0^l \frac{1}
        //        {E \cdot S(x)}}{\int_X^l \frac{1}{E \cdot S(x)}}\nonumber\\
        // F_{Bx} & = F_x \cdot (L-a-b) - F_{Ax}\nonumber\\
        // F_{Ay_i}   & = \frac{F_y \cdot (L-a-b) \cdot (L-a+b)}{2 \cdot L}-
        //                             \frac{M_z \cdot (L-a+b)}{l}\nonumber\\
        // F_{Ay_h} & = \frac{M_{Bz}+M_{Az}}{l}\nonumber\\
        // F_{By_i}   & = \frac{F_y \cdot (L-a-b) \cdot (L+a-b)}{2 \cdot L}+
        //                             \frac{M_z \cdot (L-a+b)}{l}\nonumber\\
        // F_{By_h} & = -\frac{M_{Bz}+M_{Az}}{l}\nonumber\\
        // F_{Az_i}   & = \frac{F_z \cdot (L-a-b) \cdot (L-a+b)}{2 \cdot L}+
        //                             \frac{M_y \cdot (L-a+b)}{l}\nonumber\\
        // F_{Az_h} & = -\frac{M_{By}+M_{Ay}}{l}\nonumber\\
        // F_{Bz_i}   & = \frac{F_z \cdot (L-a-b) \cdot (L+a-b)}{2 \cdot L}-
        //                             \frac{M_y \cdot (L-a+b)}{l}\nonumber\\
        // F_{Bz_h} & = \frac{M_{By}+M_{Ay}}{l}\end{align*}\begin{verbatim}
              FAx = ax2[0] * (l - a - b) *
                    EF_sections_es_l (element, pos, 0, l) /
                    EF_sections_es_l (
                      element,
                      pos,
                      EF_charge_barre_repartie_uniforme_position_resultante_x (
                        element->section,
                        a,
                        b,
                        l),
                      l);
              BUG (!isnan (FAx), false, FREE_ALL)
              FBx = ax2[0] * (l - a - b) - FAx;
              FAy_i = ax2[1] * (l - a - b) * (l - a + b) / (2. * l) -
                      ax2[5] * (l - a - b) / l;
              FAy_h = (MBz + MAz) / l;
              FBy_i = ax2[1] * (l - a - b) * (l + a - b) / (2. * l) +
                      ax2[5] * (l - a - b) / l;
              FBy_h = -(MBz + MAz) / l;
              FAz_i = ax2[2] * (l - a - b) * (l - a + b) / (2. * l) +
                      ax2[4] * (l - a - b) / l;
              FAz_h = -(MBy + MAy) / l;
              FBz_i = ax2[2] * (l - a - b) * (l + a - b) / (2. * l) -
                      ax2[4] * (l - a - b) / l;
              FBz_h = (MBy + MAy) / l;
              
      //     Détermination des fonctions des efforts dus à la charge (x, a et
      //     l sont calculés par rapport à l'élément discrétisé et non pour
      //     toute la barre). Pour cela, on calcule la sollicitation due au
      //     cas isostatique puis on ajoute la sollicitation due à l'éventuel
      //     encastrement (MAx, MBx, MAy,
      //       MAz, MBy, MBz) :\end{verbatim}\begin{align*}
      //       N(x) & = -F_{Ax} & &\textrm{ pour x de 0 à a}\nonumber\\
      //       N(x) & = \frac{(F_{Ax}+F_{Bx}) \cdot x - a \cdot F_{Bx} -
      //                (L-b) \cdot F_{Ax}}{L-a-b} &
      //                &\textrm{ pour x de a à L-b}\nonumber\\
      //       N(x) & = F_{Bx} & &\textrm{ pour x de L-b à L}\nonumber\\
              
              BUG (common_fonction_ajout_poly (
                     _1990_action_efforts_renvoie (action, 0, num),
                     0.,
                     a,
                     -FAx,
                     0., 0., 0., 0., 0., 0.,
                     debut_barre),
                   false,
                   FREE_ALL)
              BUG (common_fonction_ajout_poly (
                     _1990_action_efforts_renvoie (action, 0, num),
                     a,
                     l - b,
                     (-a * FBx - (l - b) * FAx) / (l - a - b),
                     (FAx + FBx) / (l - a - b),
                     0., 0., 0., 0., 0.,
                     debut_barre),
                   false,
                   FREE_ALL)
              BUG (common_fonction_ajout_poly (
                     _1990_action_efforts_renvoie (action, 0, num),
                     l - b,
                     l,
                     FBx,
                     0., 0., 0., 0., 0., 0.,
                     debut_barre),
                   false,
                   FREE_ALL)
              
            // T_y(x) & = -F_{Ay_i} - F_{Ay_h} & &\textrm{ pour x de 0 à a}
            // \nonumber\\
            // T_y(x) & = \frac{-a (F_{By_i}+F_{By_h})-(L-b)(F_{Ay_i}+
            //            F_{Ay_h})}{L-a-b} + \frac{F_{Ay_i}+F_{Ay_h}+
            //            F_{By_i}+F_{By_h}}{L-a-b} x &
            //            &\textrm{ pour x de a à L-b}\nonumber\\
            // T_y(x) & =  F_{By_i} + F_{By_h} &
            // &\textrm{ pour x de L-b à L}\nonumber\\
              
              BUG (common_fonction_ajout_poly (
                     _1990_action_efforts_renvoie (action, 1, num),
                     0.,
                     a,
                     -FAy_i - FAy_h,
                     0., 0., 0., 0., 0., 0.,
                     debut_barre),
                   false,
                   FREE_ALL)
              BUG (common_fonction_ajout_poly (
                     _1990_action_efforts_renvoie (action, 1, num),
                     a,
                     l - b,
                     (-a * (FBy_i + FBy_h) - (l - b) * (FAy_i + FAy_h)) /
                                                                 (l - a - b),
                     (FAy_i + FAy_h + FBy_i + FBy_h) / (l - a - b),
                     0., 0., 0., 0., 0.,
                     debut_barre),
                   false,
                   FREE_ALL)
              BUG (common_fonction_ajout_poly (
                     _1990_action_efforts_renvoie (action, 1, num),
                     l - b,
                     l,
                     FBy_i + FBy_h,
                     0., 0., 0., 0., 0., 0.,
                     debut_barre),
                   false,
                   FREE_ALL)
              
            // T_z(x) & = -F_{Az_i} - F_{Az_h} &
            //            &\textrm{ pour x de 0 à a}\nonumber\\
            // T_z(x) & = \frac{-a (F_{Bz_i}+F_{Bz_h})-(L-b) (F_{Az_i}+
            //            F_{Az_h})}{L-a-b} + \frac{F_{Az_i}+F_{Az_h}+
            //            F_{Bz_i}+F_{Bz_h}}{L-a-b} x &
            //            &\textrm{ pour x de a à L-b}\nonumber\\
            // T_z(x) & = F_{Bz_i} + F_{Bz_h} &
            //            &\textrm{ pour x de L-b à L}\nonumber\\
              
              BUG (common_fonction_ajout_poly (
                     _1990_action_efforts_renvoie (action, 2, num),
                     0.,
                     a,
                     -FAz_i - FAz_h,
                     0., 0., 0., 0., 0., 0.,
                     debut_barre),
                   false,
                   FREE_ALL)
              BUG (common_fonction_ajout_poly (
                     _1990_action_efforts_renvoie (action, 2, num),
                     a,
                     l - b,
                     (-a * (FBz_i + FBz_h) - (l - b) * (FAz_i + FAz_h)) / 
                                                                 (l - a - b),
                     (FAz_i + FAz_h + FBz_i + FBz_h) / (l - a - b),
                     0., 0., 0., 0., 0.,
                     debut_barre),
                   false,
                   FREE_ALL)
              BUG (common_fonction_ajout_poly (
                     _1990_action_efforts_renvoie (action, 2, num),
                     l - b,
                     l,
                     FBz_i + FBz_h,
                     0., 0., 0., 0., 0., 0.,
                     debut_barre),
                   false,
                   FREE_ALL)
              
            // M_x(x) & = -M_{Ax} & &\textrm{ pour x de 0 à a}\nonumber\\
            // M_x(x) & = \frac{-a \cdot M_{Bx}-(L-b) \cdot M_{Ax}}{L-a-b} +
            //            \frac{M_{Ax}+M_{Bx}}{L-a-b} \cdot x &
            //            &\textrm{ pour x de a à L-b}\nonumber\\
            // M_x(x) & = M_{Bx} & &\textrm{ pour x de L-b à L}\nonumber\\
              
              BUG (common_fonction_ajout_poly (
                     _1990_action_efforts_renvoie (action, 3, num),
                     0.,
                     a,
                     -MAx,
                     0., 0., 0., 0., 0., 0.,
                     debut_barre),
                   false,
                   FREE_ALL)
              BUG (common_fonction_ajout_poly (
                     _1990_action_efforts_renvoie (action, 3, num),
                     a,
                     l - b,
                     (-a * MBx - (l - b) * MAx) / (l - a - b),
                     (MAx + MBx) / (l - a - b),
                     0., 0., 0., 0., 0.,
                     debut_barre),
                   false,
                   FREE_ALL)
              BUG (common_fonction_ajout_poly (
                     _1990_action_efforts_renvoie (action, 3, num),
                     l - b,
                     l,
                     MBx,
                     0., 0., 0., 0., 0., 0.,
                     debut_barre),
                   false,
                   FREE_ALL)
              
            // M_y(x) & = -M_{Ay} - (F_{Az_i}+F_{Az_h}) \cdot x &
            //            &\textrm{ pour x de 0 à a}\nonumber\\
            // M_y(x) & = -(F_{Bz_i}+F_{Bz_h}) \cdot (L-x) + M_{By} -
            //            M_y \cdot (L-b-x) + F_z \cdot \frac{(L-b-x)^2}{2} &
            //            &\textrm{ pour x de a à L-b}\nonumber\\
            // M_y(x) & = M_{By} - L \cdot (F_{Bz_i}+F_{Bz_h}) + (F_{Bz_i} +
            //            F_{Bz_h}) \cdot x & &\textrm{ pour x de L-b à L}
            // \nonumber\\
              
              BUG (common_fonction_ajout_poly (
                     _1990_action_efforts_renvoie (action, 4, num),
                     0.,
                     a,
                     -MAy,
                     -FAz_i - FAz_h,
                     0., 0., 0., 0., 0.,
                     debut_barre),
                   false,
                   FREE_ALL)
              BUG (common_fonction_ajout_poly (
                     _1990_action_efforts_renvoie (action, 4, num),
                     a,
                     l - b,
                     b * b * ax2[2] / 2. + b * (ax2[4] - ax2[2] * l) +
                       ax2[2] * l * l / 2. - l * (ax2[4] + FBz_i + FBz_h) +
                       MBy,
                     -ax2[2] * (l - b) + ax2[4] + FBz_i + FBz_h,
                     ax2[2] / 2.,
                     0., 0., 0., 0.,
                     debut_barre),
                   false,
                   FREE_ALL)
              BUG (common_fonction_ajout_poly (
                     _1990_action_efforts_renvoie (action, 4, num),
                     l - b,
                     l,
                     MBy - l * (FBz_i + FBz_h),
                     FBz_i + FBz_h,
                     0., 0., 0., 0., 0.,
                     debut_barre),
                   false,
                   FREE_ALL)
              
            // M_z(x) & = -M_{Az} + (F_{Ay_i}+F_{Ay_h}) \cdot x &
            //            &\textrm{ pour x de 0 à a}\nonumber\\
            // M_z(x) & = (F_{By_i}+F_{By_h}) \cdot (L-x) - M_{Bz} -
            //            M_z \cdot (L-b-x) - F_y \cdot \frac{(L-b-x)^2}{2} &
            //            &\textrm{ pour x de a à L-b}\nonumber\\
            // M_z(x) & = M_{Bz} + L \cdot (F_{By_i}+F_{By_h}) - (F_{By_i} +
            //            F_{By_h}) \cdot x & &\textrm{ pour x de L-b à L}
              
              BUG (common_fonction_ajout_poly (
                     _1990_action_efforts_renvoie (action, 5, num),
                     0.,
                     a,
                     -MAz,
                     FAy_i + FAy_h,
                     0., 0., 0., 0., 0.,
                     debut_barre),
                   false,
                   FREE_ALL)
              BUG (common_fonction_ajout_poly (
                     _1990_action_efforts_renvoie (action, 5, num),
                     a,
                     l - b,
                     -b * b * ax2[1] / 2. + b * (ax2[1] * l + ax2[5]) -
                       ax2[1] * l * l / 2. + l * (FBy_i + FBy_h - ax2[5]) +
                       MBz,
                     ax2[1] * (l - b) + ax2[5] - FBy_i - FBy_h,
                     -ax2[1] / 2.,
                     0., 0., 0., 0.,
                     debut_barre),
                   false,
                   FREE_ALL)
              BUG (common_fonction_ajout_poly (
                     _1990_action_efforts_renvoie (action, 5, num),
                     l - b,
                     l,
                     MBz + l * (FBy_i + FBy_h),
                     -FBy_i - FBy_h,
                     0., 0., 0., 0., 0.,
                     debut_barre),
                   false,
                   FREE_ALL)
            // \end{align*}\begin{verbatim}
              
      //     Détermination des fonctions de déformation et rotation (cas
      //     isostatique + encastrement) :
              BUG (EF_charge_barre_repartie_uniforme_fonc_rx (
                     _1990_action_rotation_renvoie (action, 0, num),
                     element,
                     pos,
                     a,
                     b,
                     MAx,
                     MBx),
                   false,
                   FREE_ALL)
              BUG (EF_charge_barre_repartie_uniforme_fonc_ry (
                     _1990_action_rotation_renvoie (action, 1, num),
                     _1990_action_deformation_renvoie (action, 2, num),
                     element,
                     pos,
                     a,
                     b,
                     ax2[2],
                     ax2[4],
                     MAy,
                     MBy),
                   false,
                   FREE_ALL)
              BUG (EF_charge_barre_repartie_uniforme_fonc_rz (
                     _1990_action_rotation_renvoie (action, 2, num),
                     _1990_action_deformation_renvoie (action, 1, num),
                     element,
                     pos,
                     a,
                     b,
                     ax2[1],
                     ax2[5],
                     MAz,
                     MBz),
                   false,
                   FREE_ALL)
              BUG (EF_charge_barre_repartie_uniforme_n (
                     _1990_action_deformation_renvoie (action, 0, num),
                     element,
                     pos,
                     a,
                     b,
                     FAx,
                     FBx),
                   false,
                   FREE_ALL)
            
      //     Convertion des réactions d'appuis locales dans le repère global
      //     :\end{verbatim}\begin{center}
      //     $\{ R \}_{global} = [K] \cdot \{ F \}_{local}$\end{center}
      //     \begin{verbatim}
              BUGCRIT (t_eff_loc_f = cholmod_allocate_triplet (12,
                                                               1,
                                                               12,
                                                               0,
                                                               CHOLMOD_REAL,
                                                               p->calculs.c),
                       false,
                       (gettext ("Erreur d'allocation mémoire.\n"));
                         FREE_ALL)
              ai4 = (uint32_t *) t_eff_loc_f->i;
              aj4 = (uint32_t *) t_eff_loc_f->j;
              ax4 = (double *) t_eff_loc_f->x;
              t_eff_loc_f->nnz = 12;
              ai4[0] = 0;   aj4[0] = 0;  ax4[0] = FAx;
              ai4[1] = 1;   aj4[1] = 0;  ax4[1] = FAy_i + FAy_h;
              ai4[2] = 2;   aj4[2] = 0;  ax4[2] = FAz_i + FAz_h;
              ai4[3] = 3;   aj4[3] = 0;  ax4[3] = MAx;
              ai4[4] = 4;   aj4[4] = 0;  ax4[4] = MAy;
              ai4[5] = 5;   aj4[5] = 0;  ax4[5] = MAz;
              ai4[6] = 6;   aj4[6] = 0;  ax4[6] = FBx;
              ai4[7] = 7;   aj4[7] = 0;  ax4[7] = FBy_i + FBy_h;
              ai4[8] = 8;   aj4[8] = 0;  ax4[8] = FBz_i + FBz_h;
              ai4[9] = 9;   aj4[9] = 0;  ax4[9] = MBx;
              ai4[10] = 10; aj4[10] = 0; ax4[10] = MBy;
              ai4[11] = 11; aj4[11] = 0; ax4[11] = MBz;
              BUGCRIT (s_eff_loc_f = cholmod_triplet_to_sparse (t_eff_loc_f,
                                                                0,
                                                                p->calculs.c),
                       false,
                       (gettext ("Erreur d'allocation mémoire.\n"));
                         cholmod_free_triplet (&t_eff_loc_f, p->calculs.c);
                         FREE_ALL)
              cholmod_free_triplet (&t_eff_loc_f, p->calculs.c);
              BUGCRIT (s_eff_glo_f = cholmod_ssmult (element->m_rot,
                                                     s_eff_loc_f,
                                                     0,
                                                     1,
                                                     0,
                                                     p->calculs.c),
                       false,
                       (gettext ("Erreur d'allocation mémoire.\n"));
                         cholmod_free_sparse (&s_eff_loc_f, p->calculs.c);
                         FREE_ALL)
              cholmod_free_sparse (&s_eff_loc_f, p->calculs.c);
              BUGCRIT (t_eff_glo_f = cholmod_sparse_to_triplet (s_eff_glo_f,
                                                                p->calculs.c),
                       false,
                       (gettext ("Erreur d'allocation mémoire.\n"));
                         cholmod_free_sparse (&s_eff_glo_f, p->calculs.c);
                         FREE_ALL)
              cholmod_free_sparse (&s_eff_glo_f, p->calculs.c);
              ai4 = (uint32_t *) t_eff_glo_f->i;
              ax4 = (double *) t_eff_glo_f->x;
            
      //     Ajout des moments et les efforts dans le vecteur des forces aux
      //     noeuds {F}
              for (j = 0; j < 12; j++)
              {
                if (ai4[j] < 6)
                {
                  if (p->calculs.n_part[num_d][ai4[j]] != UINT32_MAX)
                  {
                    ax[p->calculs.n_part[num_d][ai4[j]]] += ax4[j];
                  }
                  ax3[p->calculs.n_comp[num_d][ai4[j]]] += ax4[j];
                }
                else
                {
                  if (p->calculs.n_part[num_f][ai4[j] - 6] != UINT32_MAX)
                  {
                    ax[p->calculs.n_part[num_f][ai4[j] - 6]] += ax4[j];
                  }
                  ax3[p->calculs.n_comp[num_f][ai4[j] - 6]] += ax4[j];
                }
              }
              cholmod_free_triplet (&t_eff_glo_f, p->calculs.c);
            }
      //   FinPour
            cholmod_free_triplet (&t_eff_loc_i, p->calculs.c);
#undef FREE_ALL
            
            ++it2;
          }
          
          break;
        }
  //   FinSi
        /* Charge inconnue */
        default :
        {
          FAILCRIT (false,
                    (gettext ("Type de charge %d inconnu.\n"), charge->type); )
          break;
        }
      }
      
      ++it3;
    }
  }
  // FinPour
  
  // On converti les données dans des structures permettant les calculs via les
  // libraries.
  BUGCRIT (_1990_action_forces_change (action,
                                       cholmod_triplet_to_sparse (t_for_comp,
                                                                  0,
                                                                p->calculs.c)),
           false,
           (gettext ("Erreur d'allocation mémoire.\n"));
             cholmod_free_triplet (&t_for_part, p->calculs.c);
             cholmod_free_triplet (&t_for_comp, p->calculs.c); )
  cholmod_free_triplet (&t_for_comp, p->calculs.c);
  
  // Calcul des déplacements des noeuds :\end{verbatim}\begin{align*}
  // \{ \Delta \}_{global} = [K]^{-1} \cdot \{ F \}_{global}\end{align*}
  // \begin{verbatim}
  
  BUGCRIT (t_dep_part = cholmod_allocate_triplet (p->calculs.m_part->nrow,
                                                  1,
                                                  p->calculs.m_part->nrow,
                                                  0,
                                                  CHOLMOD_REAL,
                                                  p->calculs.c),
           false,
           (gettext ("Erreur d'allocation mémoire.\n"));
             cholmod_free_triplet (&t_for_part, p->calculs.c); )
  t_dep_part->nnz = p->calculs.m_part->nrow;
  ai = (uint32_t *) t_dep_part->i;
  aj = (uint32_t *) t_dep_part->j;
  ax = (double *) t_dep_part->x;
  if (p->calculs.m_part->nrow != 0)
  {
    int status; //NS
    
    ax2 = (double *) t_for_part->x;
    status = umfpack_di_solve (UMFPACK_A,
                               p->calculs.ap,
                               p->calculs.ai,
                               p->calculs.ax,
                               ax,
                               ax2,
                               p->calculs.numeric,
                               NULL,
                               NULL);
    if (status == UMFPACK_WARNING_singular_matrix)
    {
      printf (gettext ("Attention, matrice singulière.\n"
                "Il est possible que la modélisation ne soit pas stable.\n"));
    }
    else
    {
      BUGCRIT (status == UMFPACK_OK,
               false,
               (gettext ("Erreur de calcul : %d\n"), status);
                 cholmod_free_triplet (&t_for_part, p->calculs.c);
                 cholmod_free_triplet (&t_dep_part, p->calculs.c); )
    }
    p->calculs.residu = EF_calculs_resid (p->calculs.ap,
                                          p->calculs.ai,
                                          p->calculs.ax,
                                          ax2,
                                          p->calculs.t_part->nrow,
                                          ax);
    BUG (!isnan (p->calculs.residu),
         false,
         cholmod_free_triplet (&t_for_part, p->calculs.c);
           cholmod_free_triplet (&t_dep_part, p->calculs.c); )
    printf ("Residu sur les déplacements : %g\n", p->calculs.residu);
    for (i = 0; i <p->calculs.m_part->nrow; i++)
    {
      ai[i] = i;
      aj[i] = 0;
    }
  }
  else
  {
    p->calculs.residu = 0.;
  }
  cholmod_free_triplet (&t_for_part, p->calculs.c);
  
  // Création du vecteur déplacement complet
  BUGCRIT (t_dep_tot = cholmod_allocate_triplet (
                         _1990_action_forces_renvoie(action)->nrow,
                         1,
                         _1990_action_forces_renvoie (action)->nrow,
                         0,
                         CHOLMOD_REAL,
                         p->calculs.c),
           false,
           (gettext ("Erreur d'allocation mémoire.\n"));
             cholmod_free_triplet (&t_dep_part, p->calculs.c); )
  t_dep_tot->nnz = _1990_action_forces_renvoie (action)->nrow;
  ai2 = (uint32_t *) t_dep_tot->i;
  aj2 = (uint32_t *) t_dep_tot->j;
  ax2 = (double *) t_dep_tot->x;
  j = 0;
  for (i = 0; i < p->modele.noeuds.size (); i++)
  {
    for (k = 0; k < 6; k++)
    {
      ai2[i * 6 + k] = i * 6 + k; aj2[i * 6 + k] = 0;
      if (p->calculs.n_part[i][k] == UINT32_MAX)
      {
        ax2[i * 6 + k] = 0.;
      }
      else
      {
        if (ai[j] == p->calculs.n_part[i][k])
        {
          ax2[i * 6 + k] = ax[j];
          j++;
        }
        else
        {
          ax2[i * 6 + k] = 0.;
        }
      }
    }
  }
  cholmod_free_triplet (&t_dep_part, p->calculs.c);
  BUGCRIT (_1990_action_deplacement_change (action,
                                          cholmod_triplet_to_sparse (t_dep_tot,
                                                                     0,
                                                                p->calculs.c)),
           false,
           (gettext ("Erreur d'allocation mémoire.\n"));
             cholmod_free_triplet (&t_dep_tot, p->calculs.c); )
  
  // Calcule des réactions d'appuis :\end{verbatim}\begin{displaymath}
  // \{F\} = [K] \cdot \{\Delta\} - \{F_0\} \end{displaymath}\begin{verbatim}
  BUGCRIT (sparse_tmp = cholmod_ssmult (p->calculs.m_comp,
                                      _1990_action_deplacement_renvoie(action),
                                        0,
                                        true,
                                        true,
                                        p->calculs.c),
           false,
           (gettext ("Erreur d'allocation mémoire.\n"));
             cholmod_free_triplet (&t_dep_tot, p->calculs.c); )
  BUGCRIT (_1990_action_efforts_noeuds_change (
             action,
             cholmod_add (sparse_tmp,
                          _1990_action_forces_renvoie (action),
                          one,
                          minusone,
                          true,
                          true,
                          p->calculs.c)),
           false,
           (gettext ("Erreur d'allocation mémoire.\n"));
             cholmod_free_triplet (&t_dep_tot, p->calculs.c);
             cholmod_free_sparse (&sparse_tmp, p->calculs.c); )
  cholmod_free_sparse (&sparse_tmp, p->calculs.c);
  
  // Pour chaque barre, ajout des efforts et déplacement dus au mouvement de
  // l'ensemble de la structure.
  it = p->modele.barres.begin ();
  while (it != p->modele.barres.end ())
  {
    EF_Barre *element = *it;
    double    S = m_g (EF_sections_s (element->section));
    size_t    num = std::distance (p->modele.barres.begin (),
                                   std::find (p->modele.barres.begin (),
                                              p->modele.barres.end (),
                                              element));
    
  //   Pour chaque discrétisation de la barre
    for (j = 0; j <= element->discretisation_element; j++)
    {
      EF_Noeud        *noeud_debut, *noeud_fin; // Le noeud de départ et le
      // noeud de fin, nécessaire en cas de discrétisation
      cholmod_sparse  *s_eff_loc;
      double           l_debut, l_fin;
      double           E, G;
      cholmod_triplet *t_dep_glo;
      cholmod_sparse  *s_dep_glo, *s_dep_loc;
      double           l;
      uint32_t         num_d, num_f;
      double           J = m_g (EF_sections_j (element->section));
      double           Iy = m_g (EF_sections_iy (element->section));
      double           Iz = m_g (EF_sections_iz (element->section));
      
      BUG (!isnan (J),
           false,
           cholmod_free_triplet (&t_dep_tot, p->calculs.c); )
      BUG (!isnan (Iy),
           false,
           cholmod_free_triplet (&t_dep_tot, p->calculs.c); )
      BUG (!isnan (Iz),
           false,
           cholmod_free_triplet (&t_dep_tot, p->calculs.c); )
      
      
      // Récupération du noeud de départ et de fin de la partie discrétisée
      if (j == 0)
      {
        noeud_debut = element->noeud_debut;
      }
      else
      {
        std::list <EF_Noeud *>::iterator it_t;
        
        it_t = element->nds_inter.begin ();
        std::advance (it_t, j - 1);
        noeud_debut = *it_t;
      }
      if (j == element->discretisation_element)
      {
        noeud_fin = element->noeud_fin;
      }
      else
      {
        std::list <EF_Noeud *>::iterator it_t;
        
        it_t = element->nds_inter.begin ();
        std::advance (it_t, j);
        noeud_fin = *it_t;
      }
      
      num_d = std::distance (p->modele.noeuds.begin (),
                             std::find (p->modele.noeuds.begin (), 
                                        p->modele.noeuds.end (),
                                        noeud_debut));
      num_f = std::distance (p->modele.noeuds.begin (),
                             std::find (p->modele.noeuds.begin (),
                                        p->modele.noeuds.end (),
                                        noeud_fin));
      
      // Récupération des caractéristiques de la barre en fonction du matériau
      switch (element->type)
      {
        case BETON_ELEMENT_POTEAU :
        case BETON_ELEMENT_POUTRE :
        {
          E = m_g (EF_materiaux_E (element->materiau));
          G = m_g (EF_materiaux_G (element->materiau, false));
          break;
        }
        default :
        {
          FAILCRIT (false,
                    (gettext ("Type d'élément %d inconnu.\n"), element->type);
                      cholmod_free_triplet (&t_dep_tot, p->calculs.c); )
          break;
        }
      }
      
  //     Récupération des déplacements du noeud de départ et du noeud final de
  //     l'élément
      BUGCRIT (t_dep_glo = cholmod_allocate_triplet (12,
                                                     1,
                                                     12,
                                                     0,
                                                     CHOLMOD_REAL,
                                                     p->calculs.c),
               false,
               (gettext ("Erreur d'allocation mémoire.\n"));
                 cholmod_free_triplet (&t_dep_tot, p->calculs.c); )
      ai = (uint32_t *) t_dep_glo->i;
      aj = (uint32_t *) t_dep_glo->j;
      ax = (double *) t_dep_glo->x;
      t_dep_glo->nnz = 12;
      ax2 = (double *) t_dep_tot->x;
      for (i = 0; i < 6; i++)
      {
        ai[i] = i;
        aj[i] = 0;
        ax[i] = ax2[p->calculs.n_comp[num_d][i]];
      }
      for (i = 0; i < 6; i++)
      {
        ai[i + 6] = i + 6;
        aj[i + 6] = 0;
        ax[i + 6] = ax2[p->calculs.n_comp[num_f][i]];
      }
      
  //     Conversion des déplacements globaux en déplacement locaux (u_A, v_A,
  //     w_A, theta_{Ax}, theta_{Ay}, theta_{Az}, u_B, v_B, w_B, theta_{Bx},
  //     theta_{By} et theta_{Bz}) : \end{verbatim}\begin{align*}
      // \{ \Delta \}_{local} = [R]^T \cdot \{ \Delta \}_{global}\end{align*}
      // \begin{verbatim}
      BUGCRIT (s_dep_glo = cholmod_triplet_to_sparse (t_dep_glo,
                                                      0,
                                                      p->calculs.c),
               false,
               (gettext ("Erreur d'allocation mémoire.\n"));
                 cholmod_free_triplet (&t_dep_tot, p->calculs.c);
                 cholmod_free_triplet (&t_dep_glo, p->calculs.c); )
      cholmod_free_triplet (&t_dep_glo, p->calculs.c);
      BUGCRIT (s_dep_loc = cholmod_ssmult (element->m_rot_t,
                                           s_dep_glo,
                                           0,
                                           1,
                                           true,
                                           p->calculs.c),
               false,
               (gettext ("Erreur d'allocation mémoire.\n"));
                 cholmod_free_triplet (&t_dep_tot, p->calculs.c);
                 cholmod_free_sparse (&s_dep_glo, p->calculs.c); )
      cholmod_free_sparse (&s_dep_glo, p->calculs.c);
  //     Détermination des efforts (F_{Ax}, F_{Bx}, F_{Ay}, F_{By}, F_{Az},
  //     F_{Bz}, M_{Ax}, M_{Bx}, M_{Ay}, M_{By}, M_{Az} et M_{Bz}) dans le
  //     repère local : \end{verbatim}\begin{align*}
      // \{ F \}_{local} = [K] \cdot \{ \Delta \}_{local}\end{align*}
      // \begin{verbatim}
      BUGCRIT (s_eff_loc = cholmod_ssmult (element->info_EF[j].m_rig_loc,
                                           s_dep_loc,
                                           0,
                                           1,
                                           true,
                                           p->calculs.c),
               false,
               (gettext ("Erreur d'allocation mémoire.\n"));
                 cholmod_free_triplet (&t_dep_tot, p->calculs.c);
                 cholmod_free_sparse (&s_dep_loc, p->calculs.c); )
      
#define FREE_ALL cholmod_free_triplet (&t_dep_tot, p->calculs.c); \
  cholmod_free_sparse (&s_dep_loc, p->calculs.c); \
  cholmod_free_sparse (&s_eff_loc, p->calculs.c);
      
      ax = (double *) s_dep_loc->x;
      ax2 = (double *) s_eff_loc->x;
      l_debut = EF_noeuds_distance (noeud_debut, element->noeud_debut);
      l_fin = EF_noeuds_distance (noeud_fin, element->noeud_debut);
      BUG (!isnan (l_debut), false, FREE_ALL)
      BUG (!isnan (l_fin), false, FREE_ALL)
      l = fabs (l_fin - l_debut);
      
  //     Ajout des efforts entre deux noeuds dus à leur déplacement relatif,
  //     la courbe vient s'ajouter à la courbe (si existante) déja définie
  //     précédemment. L'indice A correspond au noeud de départ et l'indice B
  //     correspond au noeud final :\end{verbatim}\begin{align*}
      // N(x) & = N(x) + F_{Ax} - (F_{Ax}+F_{Bx}) \cdot x/L\nonumber\\
      // T_y(x) & = T_y(x) + F_{Ay} - (F_{Ay}+F_{By}) \cdot x/L\nonumber\\
      // T_z(x) & = T_z(x) + F_{Az} - (F_{Az}+F_{Bz}) \cdot x/L\nonumber\\
      // M_x(x) & = M_x(x) + M_{Ax} - (M_{Ax}+M_{Bx}) \cdot x/L\nonumber\\
      // M_y(x) & = M_y(x) + M_{Ay} - (M_{Ay}+M_{By}) \cdot x/L\nonumber\\
      // M_z(x) & = M_z(x) + M_{Az} - (M_{Az}+M_{Bz}) \cdot x/L
      // \end{align*}\begin{verbatim}
      BUG (common_fonction_ajout_poly (_1990_action_efforts_renvoie (action,
                                                                     0,
                                                                     num),
                                       0.,
                                       l,
                                       ax2[0],
                                       -(ax2[0] + ax2[6]) / l,
                                       0., 0., 0., 0., 0.,
                                       l_debut),
           false,
           FREE_ALL)
      BUG (common_fonction_ajout_poly (_1990_action_efforts_renvoie (action,
                                                                     1,
                                                                     num),
                                       0.,
                                       l,
                                       ax2[1],
                                       -(ax2[1] + ax2[7]) / l,
                                       0., 0., 0., 0., 0.,
                                       l_debut),
           false,
           FREE_ALL)
      BUG (common_fonction_ajout_poly (_1990_action_efforts_renvoie (action,
                                                                     2,
                                                                     num),
                                       0.,
                                       l,
                                       ax2[2],
                                       -(ax2[2] + ax2[8]) / l,
                                       0., 0., 0., 0., 0.,
                                       l_debut),
           false,
           FREE_ALL)
      BUG (common_fonction_ajout_poly (_1990_action_efforts_renvoie (action,
                                                                     3,
                                                                     num),
                                       0.,
                                       l,
                                       ax2[3],
                                       -(ax2[3] + ax2[9]) / l,
                                       0., 0., 0., 0., 0.,
                                       l_debut),
           false,
           FREE_ALL)
      BUG (common_fonction_ajout_poly (_1990_action_efforts_renvoie (action,
                                                                     4,
                                                                     num),
                                       0.,
                                       l,
                                       ax2[4],
                                       -(ax2[4] + ax2[10]) / l,
                                       0., 0., 0., 0., 0.,
                                       l_debut),
           false,
           FREE_ALL)
      BUG (common_fonction_ajout_poly (_1990_action_efforts_renvoie (action,
                                                                     5,
                                                                     num),
                                       0.,
                                       l,
                                       ax2[5],
                                       -(ax2[5] + ax2[11]) / l,
                                       0., 0., 0., 0., 0.,
                                       l_debut),
           false,
           FREE_ALL)
      
  //     Ajout des déplacements & rotations entre deux noeuds dus à leur
  //     déplacement relatif, la courbe vient s'ajouter à la courbe (si
  //     existante) déja définie précédemment.
  //     Dans le cas d'une section constante, la déformée en x est obtenue par
  //     intégration de la courbe d'effort normal et la déformée en y et z est
  //     obtenue par double intégration de la courbe des moments. Les deux
  //     constantes sont obtenues par la connaissance des déplacements en x=0
  //     et x=l. Les rotations (ry et rz) sont obtenues par la dérivée de la
  //     flèche. La rotation rx est obtenue par intégration du moment en x. La
  //     constante est déterminée pour f(0) égal à la rotation au noeud à
  //     gauche s'il n'y a pas de relachement (sinon f(l) égal à la rotation à
  //     droite).\end{verbatim}\begin{align*}
  //        f_x(x) = & u_A - \frac{F_{Ax}}{E \cdot S} \cdot x + \frac{F_{Ax} +
  //                   F_{Bx}}{2 \cdot E \cdot S \cdot L} \cdot x^2 \nonumber\\
  //        f_y(x) = & v_A + A \cdot x - \frac{M_{Az}}{2 \cdot E \cdot I_z}
  //                 \cdot x^2 + \frac{M_{Az}+M_{Bz}}{6 \cdot L \cdot E \cdot
  //                                   I_z} \cdot x^3 \text{ avec } \nonumber\\
  //             & \text{A tel que } f_y(l)=v_B \text{ soit } A = \frac{M_{Az}
  //             \cdot L}{2*E*Iz}-\frac{(M_{Az}+M_{Bz}) \cdot L}{6*E*Iz} +
  //                                             \frac{-v_A+v_B}{L} \nonumber\\
  //        f_z(x) = & w_A + B \cdot x + \frac{M_{Ay}}{2 \cdot E \cdot I_y}
  //                 \cdot x^2 - \frac{M_{Ay}+M_{By}}{6 \cdot L \cdot E \cdot
  //                 I_y} \cdot x^3 \text{ avec } \nonumber\\
  //             & \text{B tel que } f_z(l)=w_B \text{ soit } B = -\frac{M_{Ay}
  //             \cdot L}{2*E*I_y}+\frac{(M_{Ay}+M_{By}) \cdot L}{6*E*I_y} +
  //                                             \frac{-w_A+w_B}{L} \nonumber\\
  //        r_x(x) = & C - \frac{M_{Ax}}{G \cdot J} \cdot x + \frac{M_{Ax} +
  //                 M_{Bx}}{2 \cdot G \cdot J \cdot L} \cdot x^2 \text{ avec }
  //                 \nonumber\\ & \text{C tel que } r_x(l)=\theta_{Bx}  
  //                 \text{ soit } C = \frac{M_{Ax} \cdot L}{G \cdot J} -
  //                 \frac{(M_{Ax}+M_{Bx}) \cdot L}{2 \cdot G \cdot J}+
  //                 \theta_{Bx} \nonumber\\
  //             & \text{C tel que } r_x(0)=\theta_{Ax}  \text{ soit }
  //             C = \theta_{Ax} \nonumber\\
  //             & \text{Solution 1 si le noeud A n'est pas encastrée et solution 2 si le noeud B n'est pas encastrée} \nonumber\\
  //        r_y(x) = & -B - \frac{M_{Ay}}{E \cdot I_y} \cdot x + \frac{M_{Ay} +
  //                 M_{By}}{2 \cdot L \cdot E \cdot I_y} \cdot x^2 \nonumber\\
  //        r_z(x) = & A - \frac{M_{Az}}{E \cdot I_z} \cdot x + \frac{M_{Az} +
  //                             M_{Bz}}{2 \cdot L \cdot E \cdot I_z} \cdot x^2

          BUG (common_fonction_ajout_poly (
                 _1990_action_deformation_renvoie (action, 0, num),
                 0.,
                 l,
                 ax[0],
                 -ax2[0] / (E * S),
                 (ax2[0] + ax2[6]) / (2 * E * S * l),
                 0., 0., 0., 0.,
                 l_debut),
               false,
               FREE_ALL)
          BUG (common_fonction_ajout_poly (
                 _1990_action_deformation_renvoie (action, 1, num),
                 0.,
                 l,
                 ax[1],
                 (ax2[5] / (2 * E * Iz) * l * l -
                   (ax2[5] + ax2[11]) / (6 * E * Iz) * l * l -
                   ax[1] + ax[7]) / l,
                 -ax2[5] / (2 * E * Iz),
                 (ax2[5] + ax2[11]) / (6 * l * E * Iz),
                 0., 0., 0.,
                 l_debut),
               false,
               FREE_ALL)
          BUG (common_fonction_ajout_poly (
                 _1990_action_deformation_renvoie (action, 2, num),
                 0.,
                 l,
                 ax[2],
                 (-ax2[4] / (2 * E * Iy) * l * l +
                   (ax2[4] + ax2[10]) / (6 * E * Iy) * l * l -
                   ax[2] + ax[8]) / l,
                 ax2[4] / (2 * E * Iy),
                 -(ax2[4] + ax2[10]) / (6 * l * E * Iy),
                 0., 0., 0.,
                 l_debut),
               false,
               FREE_ALL)
          if ((j == 0) &&
              (element->relachement != NULL) &&
              (element->relachement->rx_debut != EF_RELACHEMENT_BLOQUE))
          {
            BUG (common_fonction_ajout_poly (
                   _1990_action_rotation_renvoie (action, 0, num),
                   0.,
                   l,
                   ax2[3] / (G * J) * l -
                     (ax2[3] + ax2[9]) / (2 * G * J * l) * l * l + ax[9],
                   -ax2[3] / (G * J),
                   (ax2[3] + ax2[9]) / (2 * G * J * l),
                   0., 0., 0., 0.,
                   l_debut),
                 false,
                 FREE_ALL)
          }
          else
          {
            BUG (common_fonction_ajout_poly (
                   _1990_action_rotation_renvoie (action, 0, num),
                   0.,
                   l,
                   ax[3],
                   -ax2[3] / (G * J),
                   (ax2[3] + ax2[9]) / (2 * G * J * l),
                   0., 0., 0., 0.,
                   l_debut),
                 false,
                 FREE_ALL)
          }
          BUG (common_fonction_ajout_poly (
                 _1990_action_rotation_renvoie (action, 1, num),
                 0.,
                 l,
                 -(-ax2[4] / (2 * E * Iy) * l * l +
                   (ax2[4] + ax2[10]) / (6 * E * Iy) * l * l -
                   ax[2] + ax[8]) / l,
                 -ax2[4] / (E * Iy),
                 (ax2[4] + ax2[10]) / (2 * l * E * Iy),
                 0., 0., 0., 0.,
                 l_debut),
               false,
               FREE_ALL)
          BUG (common_fonction_ajout_poly (
                 _1990_action_rotation_renvoie (action, 2, num),
                 0.,
                 l,
                 (ax2[5] / (2 * E * Iz) * l * l +
                   (-ax2[5] - ax2[11]) / (6 * E * Iz) * l * l -
                   ax[1] + ax[7]) / l,
                 -ax2[5] / (E * Iz),
                 (ax2[5] + ax2[11]) / (2 * l * E * Iz),
                 0., 0., 0., 0.,
                 l_debut),
               false,
               FREE_ALL)
  //        \end{align*}\begin{verbatim}
      
      cholmod_free_sparse (&s_dep_loc, p->calculs.c);
      cholmod_free_sparse (&s_eff_loc, p->calculs.c);
    }
  //   FinPour
    ++it;
  }
  // FinPour
  cholmod_free_triplet (&t_dep_tot, p->calculs.c);
  
  return true;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
