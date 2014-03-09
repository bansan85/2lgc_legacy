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
#include <math.h>
#include <gmodule.h>
#include <string.h>

#include "1990_action.h"
#include "common_projet.h"
#include "common_erreurs.h"
#include "common_math.h"
#ifdef ENABLE_GTK
#include "common_gtk.h"
#endif
#include "common_fonction.h"
#include "common_selection.h"
#include "EF_noeuds.h"
#include "EF_calculs.h"
#include "EF_charge.h"
#include "EF_sections.h"
#include "EF_materiaux.h"
#include "EF_gtk_charge_barre_ponctuelle.h"

Charge *
EF_charge_barre_ponctuelle_ajout (Projet     *p,
                                  Action     *action,
                                  GList      *barres,
                                  gboolean    repere_local,
                                  Flottant    a,
                                  Flottant    fx,
                                  Flottant    fy,
                                  Flottant    fz,
                                  Flottant    mx,
                                  Flottant    my,
                                  Flottant    mz,
                                  const char *nom)
/**
 * \brief Ajoute une charge ponctuelle à une action à l'intérieur d'une barre.
 * \param p : la variable projet,
 * \param action : l'action qui contiendra la charge,
 * \param barres : liste des barres qui supportera la charge,
 * \param repere_local : TRUE si les charges doivent être prises dans le repère
 *                       local, FALSE pour le repère global,
 * \param a : position en mètre de la charge par rapport au début de la barre,
 * \param fx : force suivant l'axe x en N,
 * \param fy : force suivant l'axe y en N,
 * \param fz : force suivant l'axe z en N,
 * \param mx : moment autour de l'axe x en N.m,
 * \param my : moment autour de l'axe y en N.m,
 * \param mz : moment autour de l'axe z en N.m,
 * \param nom : nom de la charge.
 * \return
 *   Succès : pointeur vers la nouvelle charge.\n
 *   Échec : NULL :
 *     - p == NULL,
 *     - action == NULL,
 *     - barre == NULL,
 *     - a < 0 ou a > l,
 *     - en cas d'erreur d'allocation mémoire
 */
{
  Charge                  *charge;
  Charge_Barre_Ponctuelle *charge_d;
  
  BUGPARAM (p, "%p", p, NULL)
  BUGPARAM (action, "%p", action, NULL)
  BUGPARAM (barres, "%p", barres, NULL)
  INFO (!((m_g (a) < 0.) && (!(ERR (m_g (a), 0.)))),
        NULL,
        (gettext ("La position de la charge ponctuelle %f est incorrecte.\n"),
                  m_g (a));)
  BUGCRIT (charge = malloc (sizeof (Charge)),
           NULL,
           (gettext ("Erreur d'allocation mémoire.\n"));)
  BUGCRIT (charge_d = malloc (sizeof (Charge_Barre_Ponctuelle)),
           NULL,
           (gettext ("Erreur d'allocation mémoire.\n"));
             free (charge);)
  charge->data = charge_d;
  if (barres != NULL)
  {
    GList *list_parcours = barres;
    
    do
    {
      EF_Barre *barre = list_parcours->data;
      double    distance = EF_noeuds_distance (barre->noeud_debut,
                                               barre->noeud_fin);
      
      INFO (!((m_g (a) > distance) && (!(ERR (m_g (a), distance)))),
            NULL,
            (gettext ("La position de la charge ponctuelle %f est incorrecte.\nLa longueur de la barre %d est de %f m.\n"),
                      m_g (a),
                      barre->numero,
                      distance);
              free (charge_d);
              free (charge);)
      
      list_parcours = g_list_next (list_parcours);
    }
    while (list_parcours != NULL);
  }
  
  charge->type = CHARGE_BARRE_PONCTUELLE;
  charge_d->barres = barres;
  charge_d->repere_local = repere_local;
  charge_d->position = a;
  charge_d->fx = fx;
  charge_d->fy = fy;
  charge_d->fz = fz;
  charge_d->mx = mx;
  charge_d->my = my;
  charge_d->mz = mz;
  
  BUG (EF_charge_ajout (p, action, charge, nom),
       NULL,
       free (charge_d);
         free (charge);)
  
  return charge;
}


char *
EF_charge_barre_ponctuelle_description (Charge *charge)
/**
 * \brief Renvoie la description d'une charge de type ponctuelle sur noeud.
 * \param charge : la charge à décrire.
 * \return
 *   Succès : une chaîne de caractère.\n
 *   Échec : NULL :
 *     - charge == NULL,
 *     - en cas d'erreur d'allocation mémoire.
 */
{
  Charge_Barre_Ponctuelle *charge_d;
  char                     txt_pos[30];
  char                     txt_fx[30], txt_fy[30], txt_fz[30];
  char                     txt_mx[30], txt_my[30], txt_mz[30];
  char                    *txt_liste_barres, *description;
  
  BUGPARAM (charge, "%p", charge, FALSE)
  charge_d = charge->data;
  
  BUG (txt_liste_barres = common_selection_noeuds_en_texte (charge_d->barres),
       NULL)
  conv_f_c (charge_d->position, txt_pos, DECIMAL_DISTANCE);
  conv_f_c (charge_d->fx, txt_fx, DECIMAL_FORCE);
  conv_f_c (charge_d->fx, txt_fx, DECIMAL_FORCE);
  conv_f_c (charge_d->fy, txt_fy, DECIMAL_FORCE);
  conv_f_c (charge_d->fz, txt_fz, DECIMAL_FORCE);
  conv_f_c (charge_d->mx, txt_mx, DECIMAL_MOMENT);
  conv_f_c (charge_d->my, txt_my, DECIMAL_MOMENT);
  conv_f_c (charge_d->mz, txt_mz, DECIMAL_MOMENT);
  
  BUGCRIT (description = g_strdup_printf (
             "%s : %s, %s : %s m, %s, Fx : %s N, Fy : %s N, Fz : %s N, Mx : %s N.m, My : %s N.m, Mz : %s N.m",
             strstr (txt_liste_barres, ";") == NULL ?
               gettext ("Barre") :
               gettext ("Barres"),
             txt_liste_barres,
             gettext ("position"),
             txt_pos,
             charge_d->repere_local ?
               gettext ("repère : local") :
               gettext ("repère : global"),
             txt_fx,
             txt_fy,
             txt_fz,
             txt_mx,
             txt_my,
             txt_mz),
           FALSE,
           (gettext ("Erreur d'allocation mémoire.\n"));
             free (txt_liste_barres);)
  
  free (txt_liste_barres);
  
  return description;
}


gboolean
EF_charge_barre_ponctuelle_mx (EF_Barre      *barre,
                               unsigned int   discretisation,
                               double         a,
                               Barre_Info_EF *infos,
                               double         mx,
                               double        *ma,
                               double        *mb)
/**
 * \brief Calcule l'opposé aux moments d'encastrement pour l'élément spécifié
 *        soumis au moment de torsion mx dans le repère local. Les résultats
 *        sont renvoyés par l'intermédiaire des pointeurs ma et mb qui ne
 *        peuvent être NULL.
 * \param barre : Barre à étudier,
 * \param discretisation : partie de la barre à étudier,
 * \param a : position de la charge par rapport au début de la partie de barre
 *            à étudier,
 * \param infos : infos de la partie de barre concernée,
 * \param mx : moment autour de l'axe x,
 * \param ma : pointeur qui contiendra le moment au début de la barre,
 * \param mb : pointeur qui contiendra le moment à la fin de la barre.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - barre == NULL,
 *     - infos == NULL,
 *     - ma == NULL,
 *     - mb == NULL,
 *     - discretisation>barre->discretisation_element,
 *     - kAx == kBx == MAXDOUBLE,
 *     - a < 0. ou a > l
 */
{
  EF_Noeud *debut, *fin;
  double    l, G, J;

  BUGPARAM (barre, "%p", barre, FALSE)
  BUGPARAM (infos, "%p", infos, FALSE)
  BUGPARAM (ma, "%p", ma, FALSE)
  BUGPARAM (mb, "%p", mb, FALSE)
  INFO (discretisation <= barre->discretisation_element,
        FALSE,
        (gettext ("La discrétisation %d souhaitée est hors domaine %d.\n"),
                  discretisation,
                  barre->discretisation_element);)
  INFO (!((ERR (infos->kAx, MAXDOUBLE)) && (ERR (infos->kBx, MAXDOUBLE))),
        FALSE,
        (gettext ("Impossible de relâcher rx simultanément des deux cotés de la barre.\n"));)
  INFO (!((a < 0.) && (!(ERR (a, 0.)))),
        FALSE,
        (gettext ("La position de la charge ponctuelle %f est incorrecte.\n"),
                  a);)

  // Les moments aux extrémités de la barre sont déterminés par les intégrales
  // de Mohr et valent dans le cas général :\end{verbatim}\begin{center}
  // \includegraphics[width=8cm]{images/charge_barre_ponctuelle_mx.pdf}
  // \end{center}\begin{align*}
  // M_{Bx} = & \frac{\int_0^a \frac{1}{J(x) \cdot G}dx + k_A}
  //      {\int_0^L \frac{1}{J(x) \cdot G} dx + k_A + k_B} \cdot M_x\nonumber\\
  // M_{Ax} = & M_x - M_{Bx}\end{align*}\begin{verbatim}
  
  if (discretisation == 0)
    debut = barre->noeud_debut;
  else
    debut = g_list_nth_data (barre->nds_inter, discretisation-1);
  if (discretisation == barre->discretisation_element)
    fin = barre->noeud_fin;
  else
    fin = g_list_nth_data (barre->nds_inter, discretisation);
  
  l = EF_noeuds_distance (debut, fin);
  BUG (!isnan (l), FALSE)
  INFO (!((a > l) && (!(ERR (a, l)))),
        FALSE,
        (gettext ("La position de la charge ponctuelle %f est incorrecte.\nLa longueur de la barre est de %f m.\n"),
                  a,
                  l);)
  
  G = m_g (EF_materiaux_G (barre->materiau, FALSE));
  J = m_g (EF_sections_j (barre->section));
  
  BUG (!isnan (G), FALSE)
  BUG (!isnan (J), FALSE)
  
  // Pour une section section constante, les moments valent :\end{verbatim}
  // \begin{displaymath}
  // M_{Bx} = \frac{\frac{a}{G \cdot J} +k_{Ax}}{\frac{L}{G \cdot J} + k_{Ax} +
  //          k_{Bx}} \cdot M_x\end{displaymath}\begin{verbatim}
  
  if (ERR (infos->kAx, MAXDOUBLE))
    *mb = mx;
  else if (ERR (infos->kBx, MAXDOUBLE))
    *mb = 0.;
  else
    *mb = (a / (G * J) + infos->kAx) /
            (l / (G * J) + infos->kAx + infos->kBx) * mx;
  *ma = mx - *mb;
  
  return TRUE;
}


gboolean
EF_charge_barre_ponctuelle_def_ang_iso_y (EF_Barre    *barre,
                                          unsigned int discretisation,
                                          double       a,
                                          double       fz,
                                          double       my,
                                          double      *phia,
                                          double      *phib)
/**
 * \brief Calcule les angles de rotation autour de l'axe y pour un élément
 *        bi-articulé soumis au chargement fz, my dans le repère local. Les
 *        résultats sont renvoyés par l'intermédiaire des pointeurs phia et
 *        phib qui ne peuvent être NULL.
 * \param barre : Barre à étudier,
 * \param discretisation : partie de la barre à étudier,
 * \param a : position de la charge par rapport au début de la partie de barre
 *            à étudier,
 * \param fz : force suivant l'axe z,
 * \param my : moment autour de l'axe y,
 * \param phia : pointeur qui contiendra l'angle au début de la barre,
 *               ne peut être NULL,
 * \param phib : pointeur qui contiendra l'angle à la fin de la barre,
 *               ne peut être NULL.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - barre == NULL,
 *     - phia == NULL,
 *     - phib == NULL,
 *     - discretisation>barre->discretisation_element,
 *     - a < 0. ou a > l.
 */
{
  EF_Noeud *debut, *fin;
  double    l, b, E, I;

  BUGPARAM (barre, "%p", barre, FALSE)
  BUGPARAM (phia, "%p", phia, FALSE)
  BUGPARAM (phib, "%p", phib, FALSE)
  INFO (discretisation <= barre->discretisation_element,
        FALSE,
        (gettext ("La discrétisation %d souhaitée est hors domaine %d.\n"),
                  discretisation,
                  barre->discretisation_element);)
  INFO (!((a < 0.) && (!(ERR (a, 0.)))),
        FALSE,
        (gettext ("La position de la charge ponctuelle %f est incorrecte.\n"),
                  a);)
  
  // Les rotations aux extrémités de la barre sont déterminées par les
  // intégrales de Mohr et valent dans le cas général :\end{verbatim}
  // \begin{center}
  // \includegraphics[width=8cm]{images/charge_barre_ponctuelle_fz.pdf}
  // \includegraphics[width=8cm]{images/charge_barre_ponctuelle_my.pdf}\par
  // \includegraphics[width=8cm]{images/charge_barre_ponctuelle_phiay.pdf}
  // \includegraphics[width=8cm]{images/charge_barre_ponctuelle_phiby.pdf}
  // \end{center}\begin{align*}
  // \varphi_A = & \int_0^a \frac{Mf_{11} \cdot Mf_0}{E \cdot I_y(x)} +
  //             \int_a^L \frac{Mf_{12} \cdot Mf_0}{E \cdot I_y(x)} +
  //             \int_0^a \frac{Mf_{21} \cdot Mf_0}{E \cdot I_y(x)} +
  //             \int_a^L \frac{Mf_{22} \cdot Mf_0}{E \cdot I_y(x)} \nonumber\\
  // \varphi_B = & \int_0^a \frac{Mf_{11} \cdot Mf_3}{E \cdot I_y(x)} +
  //             \int_a^L \frac{Mf_{12} \cdot Mf_3}{E \cdot I_y(x)} +
  //             \int_0^a \frac{Mf_{21} \cdot Mf_3}{E \cdot I_y(x)} +
  //             \int_a^L \frac{Mf_{22} \cdot Mf_3}{E \cdot I_y(x)}
  // \end{align*}\begin{align*}
  // \texttt{avec :}
  // Mf_0 = &\frac{L-x}{L} &
  // Mf_{11} = &-\frac{F_z \cdot b \cdot x}{L}\nonumber\\
  // Mf_{12} = & -\frac{F_z \cdot a \cdot (L-x)}{L} &
  // Mf_{21} = & -\frac{M_y \cdot x}{L}\nonumber\\
  // Mf_{22} = & \frac{M_y \cdot (L-x)}{L} &
  // Mf_3 = & -\frac{x}{L}\end{align*}\begin{verbatim}
  
  if (discretisation == 0)
    debut = barre->noeud_debut;
  else
    debut = g_list_nth_data (barre->nds_inter, discretisation-1);
  if (discretisation == barre->discretisation_element)
    fin = barre->noeud_fin;
  else
    fin = g_list_nth_data (barre->nds_inter, discretisation);
  
  l = EF_noeuds_distance (debut, fin);
  BUG (!isnan (l), FALSE)
  INFO (!((a > l) && (!(ERR (a, l)))),
        FALSE,
        (gettext ("La position de la charge ponctuelle %f est incorrecte.\nLa longueur de la barre est de %f m.\n"),
                  a,
                  l);)
  b = l - a;
  
  E = m_g (EF_materiaux_E (barre->materiau));
  I = m_g (EF_sections_iy (barre->section));
  BUG (!isnan (I), FALSE)
  BUG (!isnan (E), FALSE)
  
  // Pour une section constante, les angles valent :\end{verbatim}
  // \begin{align*}
  // \varphi_A = &-\frac{F_z \cdot a}{6 \cdot E \cdot I_y \cdot L} \cdot b
  //             \cdot (2 \cdot L-a)-\frac{M_y}{6 \cdot E \cdot I_y \cdot L}
  //             \cdot (L^2-3 \cdot b^2)\nonumber\\
  // \varphi_B = &\frac{F_z \cdot a}{6 \cdot E \cdot I_y \cdot L} \cdot
  //             (L^2-a^2)-\frac{M_y}{6 \cdot E \cdot I_y \cdot L} \cdot
  //             (L^2-3 \cdot a^2)\end{align*}\begin{verbatim}
  
  *phia = -fz * a / (6 * E * I * l) * b * (2 * l - a) -
            my / (6 * E * I * l) * (l * l - 3 * b * b);
  *phib = fz * a / (6 * E * I * l) * (l * l - a * a) -
            my / (6 * E * I * l) * (l * l - 3 * a * a);
  
  return TRUE;
}


gboolean
EF_charge_barre_ponctuelle_def_ang_iso_z (EF_Barre    *barre,
                                          unsigned int discretisation,
                                          double       a,
                                          double       fy,
                                          double       mz,
                                          double      *phia,
                                          double      *phib)
/**
 * \brief Calcule les angles de rotation autour de l'axe z pour un élément
 *        bi-articulé soumis au chargement fy, mz dans le repère local. Les
 *        résultats sont renvoyés par l'intermédiaire des pointeurs phia et
 *        phib qui ne peuvent être NULL.
 * \param barre : Barre à étudier,
 * \param discretisation : partie de la barre à étudier,
 * \param a : position de la charge par rapport au début de la partie de barre
 *            à étudier,
 * \param fy : force suivant l'axe y,
 * \param mz : moment autour de l'axe z,
 * \param phia : pointeur qui contiendra l'angle au début de la barre,
 * \param phib : pointeur qui contiendra l'angle à la fin de la barre.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - barre == NULL,
 *     - phia == NULL,
 *     - phib == NULL,
 *     - discretisation>barre->discretisation_element,
 *     - a < 0. ou a > l.
 */
{
  EF_Noeud *debut, *fin;
  double    l, b, E, I;
  
  BUGPARAM (barre, "%p", barre, FALSE)
  INFO (discretisation <= barre->discretisation_element,
        FALSE,
        (gettext ("La discrétisation %d souhaitée est hors domaine %d.\n"),
                  discretisation,
                  barre->discretisation_element);)
  BUGPARAM (phia, "%p", phia, FALSE)
  BUGPARAM (phib, "%p", phib, FALSE)
  INFO (!((a < 0.) && (!(ERR (a, 0.)))),
        FALSE,
        (gettext ("La position de la charge ponctuelle %f est incorrecte.\n"),
                  a);)
  
  // Les rotations aux extrémités de la barre sont déterminées par les
  // intégrales de Mohr et valent dans le cas général :\end{verbatim}
  // \begin{center}
  // \includegraphics[width=8cm]{images/charge_barre_ponctuelle_fy.pdf}
  // \includegraphics[width=8cm]{images/charge_barre_ponctuelle_mz.pdf}\par
  // \includegraphics[width=8cm]{images/charge_barre_ponctuelle_phiaz.pdf}
  // \includegraphics[width=8cm]{images/charge_barre_ponctuelle_phibz.pdf}
  // \end{center}\begin{align*}
  // \varphi_A = & \int_0^a \frac{Mf_{11} \cdot Mf_0}{E \cdot I_z(x)} +
  //             \int_a^L \frac{Mf_{12} \cdot Mf_0}{E \cdot I_z(x)} +
  //             \int_0^a \frac{Mf_{21} \cdot Mf_0}{E \cdot I_z(x)} +
  //             \int_a^L \frac{Mf_{22} \cdot Mf_0}{E \cdot I_z(x)} \nonumber\\
  // \varphi_B = & \int_0^a \frac{Mf_{11} \cdot Mf_3}{E \cdot I_z(x)} +
  //             \int_a^L \frac{Mf_{12} \cdot Mf_3}{E \cdot I_z(x)} +
  //             \int_0^a \frac{Mf_{21} \cdot Mf_3}{E \cdot I_z(x)} +
  //             \int_a^L \frac{Mf_{22} \cdot Mf_3}{E \cdot I_z(x)}\end{align*}
  // \begin{align*}
  // \texttt{avec :} Mf_0 = &\frac{L-x}{L} &
  // Mf_{11} = &\frac{F_y \cdot b \cdot x}{L}\nonumber\\
  // Mf_{12} = & \frac{F_y \cdot a \cdot (L-x)}{L} &
  // Mf_{21} = & -\frac{M_z \cdot x}{L}\nonumber\\
  // Mf_{22} = & \frac{M_z \cdot (L-x)}{L} &
  // Mf_3 = & -\frac{x}{L}\end{align*}\begin{verbatim}
  
  if (discretisation == 0)
    debut = barre->noeud_debut;
  else
    debut = g_list_nth_data (barre->nds_inter, discretisation-1);
  if (discretisation == barre->discretisation_element)
    fin = barre->noeud_fin;
  else
    fin = g_list_nth_data (barre->nds_inter, discretisation);
  
  l = EF_noeuds_distance (debut, fin);
  BUG (!isnan (l), FALSE)
  INFO (!((a > l) && (!(ERR (a, l)))),
        FALSE,
        (gettext ("La position de la charge ponctuelle %f est incorrecte.\nLa longueur de la barre est de %f m.\n"),
                  a,
                  l);)
  b = l - a;
  
  E = m_g (EF_materiaux_E (barre->materiau));
  I = m_g (EF_sections_iz (barre->section));
  BUG (!isnan (E), FALSE)
  BUG (!isnan (I), FALSE)
  
  // Pour une section constante, les angles valent :\end{verbatim}
  // \begin{displaymath}
  // \varphi_A = \frac{ F_y \cdot a}{6 \cdot E \cdot I_z \cdot L} b \cdot
  //             (2 \cdot L-a) - \frac{M_z}{6 \cdot E \cdot I_z \cdot L}
  //             \left(L^2-3 \cdot b^2 \right)\end{displaymath}
  // \begin{displaymath}
  // \varphi_B = \frac{-F_y \cdot a}{6 \cdot E \cdot I_z \cdot L} (L^2-a^2) -
  //             \frac{M_z}{6 \cdot E \cdot I_z \cdot L} \left(L^2-3 \cdot
  //             a^2 \right)\end{displaymath}\begin{verbatim}
  *phia = fy * a / (6 * E * I * l) * b * (2 * l - a) -
            mz / (6 * E * I * l) * (l * l - 3 * b * b);
  *phib = -fy * a / (6 * E * I * l) * (l * l - a * a) -
            mz / (6 * E * I * l) * (l * l - 3 * a * a);
  
  return TRUE;
}


gboolean
EF_charge_barre_ponctuelle_fonc_rx (Fonction    *fonction,
                                    EF_Barre    *barre,
                                    unsigned int discretisation,
                                    double       a,
                                    double       max,
                                    double       mbx)
/**
 * \brief Calcule les déplacements d'une barre en rotation autour de l'axe x en
 *        fonction des efforts aux extrémités de la poutre soumise à un moment
 *        de torsion ponctuel à la position a.
 * \param fonction : fonction où sera ajoutée la déformée,
 * \param barre : Barre à étudier,
 * \param discretisation : partie de la barre à étudier,
 * \param a : position du moment ponctuel autour de l'axe x par rapport au
 *            début de la partie de barre à étudier,
 * \param max : moment au début de la barre,
 * \param mbx : moment à la fin de la barre.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - fonction == NULL,
 *     - barre == NULL,
 *     - discretisation>barre->discretisation_element,
 *     - kAx == kBx == MAXDOUBLE,
 *     - a < 0. ou a > l.
 */
{
  EF_Noeud      *debut, *fin;
  Barre_Info_EF *infos;
  double         l;
  double         G, debut_barre, J;
  
  BUGPARAM (fonction, "%p", fonction, FALSE)
  BUGPARAM (barre, "%p", barre, FALSE)
  INFO (discretisation <= barre->discretisation_element,
        FALSE,
        (gettext ("La discrétisation %d souhaitée est hors domaine %d.\n"),
                  discretisation,
                  barre->discretisation_element);)
  infos = &(barre->info_EF[discretisation]);
  INFO (!((ERR (infos->kAx, MAXDOUBLE)) && (ERR (infos->kBx, MAXDOUBLE))),
        FALSE,
        (gettext ("Impossible de relâcher rx simultanément des deux cotés de la barre.\n"));)
  INFO (!((a < 0.) && (!(ERR (a, 0.)))),
        FALSE,
        (gettext ("La position de la charge ponctuelle %f est incorrecte.\n"),
                  a);)
  
  // La déformation d'une barre soumise à un effort de torsion est défini par
  // les formules :\end{verbatim}\begin{center}
  // \includegraphics[width=8cm]{images/charge_barre_ponctuelle_mx.pdf}
  // \end{center}\begin{verbatim}
  // Si le noeud B est relaché en rotation Alors\end{verbatim}\begin{align*}
  // r_x(x) = & M_{Ax} \cdot \left( k_{Ax} + \int_0^x \frac{1}{J \cdot G} dX
  //          \right) & & \textrm{ pour x variant de 0 à a}\nonumber\\
  // r_x(x) = & M_{Ax} \cdot k_{Ax} + \int_0^a \frac{M_{Ax}}{J \cdot G} dX -
  //          \int_a^x \frac{M_{Bx}}{J \cdot G} dX & &
  //          \textrm{ pour x variant de a à L}\end{align*}\begin{verbatim}
  // Sinon \end{verbatim}\begin{align*}
  // r_x(x) = & M_{Bx} \cdot k_{Bx} + \int_a^L \frac{M_{Bx}}{J \cdot G} dX -
  //          \int_x^a \frac{M_{Ax}}{J \cdot G} dX & &
  //          \textrm{ pour x variant de 0 à a}\nonumber\\
  // r_x(x) = & M_{Bx} \cdot \left( k_{Bx} + \int_x^L \frac{1}{J \cdot G} dX
  //          \right) & & \textrm{ pour x variant de a à L}\end{align*}
  // \begin{verbatim}
  // FinSi
  
  if (discretisation == 0)
    debut = barre->noeud_debut;
  else
    debut = g_list_nth_data (barre->nds_inter, discretisation-1);
  if (discretisation == barre->discretisation_element)
    fin = barre->noeud_fin;
  else
    fin = g_list_nth_data (barre->nds_inter, discretisation);
  
  debut_barre = EF_noeuds_distance (barre->noeud_debut, debut);
  l = EF_noeuds_distance (debut, fin);
  BUG (!isnan (l), FALSE)
  INFO (!((a > l) && (!(ERR (a, l)))),
        FALSE,
        (gettext ("La position de la charge ponctuelle %f est incorrecte.\nLa longueur de la barre est de %f m.\n"), a, l);)
  
  G = m_g (EF_materiaux_G (barre->materiau, FALSE));
  J = m_g (EF_sections_j (barre->section));
  BUG (!isnan (G), FALSE)
  BUG (!isnan (J), FALSE)
  
  if (ERR (infos->kBx, MAXDOUBLE))
  {
    BUG (common_fonction_ajout_poly (
           fonction,
           0.,
           a,
           max * infos->kAx,
           max / (G * J),
           0., 0., 0., 0., 0.,
           debut_barre),
         FALSE)
    BUG (common_fonction_ajout_poly (
           fonction,
           a,
           l,
           max * infos->kAx + a * (max + mbx) / (G * J),
           -mbx / (G * J),
           0., 0., 0., 0., 0.,
           debut_barre),
         FALSE)
  }
  else
  {
    BUG (common_fonction_ajout_poly (
           fonction,
           0.,
           a,
           mbx * infos->kBx - (a * (max + mbx) - l * mbx) / (G * J),
           max / (G * J),
           0., 0., 0., 0., 0.,
           debut_barre),
         FALSE)
    BUG (common_fonction_ajout_poly (
           fonction,
           a,
           l,
           mbx * (infos->kBx + l / (G * J)),
           -mbx / (G * J),
           0., 0., 0., 0., 0.,
           debut_barre),
         FALSE)
  }
  
  // Pour une section section constante, les moments valent :
  // Si le noeud B est relaché en rotation Alors\end{verbatim}\begin{align*}
  // r_x(x) = & M_{Ax} \cdot \left( k_{Ax} + \frac{x}{J \cdot G} \right) & &
  //          \textrm{ pour x variant de 0 à a}\nonumber\\
  // r_x(x) = & M_{Ax} \cdot k_{Ax} + \frac{a \cdot (M_{Ax}+M_{Bx})-M_{Bx}
  //          \cdot x}{J \cdot G} & & \textrm{ pour x variant de a à L}
  // \end{align*}\begin{verbatim}
  // Sinon \end{verbatim}\begin{align*}
  // r_x(x) = & M_{Bx} \cdot k_{Bx} - \frac{a \cdot (M_{Ax}+M_{Bx})-l \cdot
  //          M_{Bx} - M_{Ax} \cdot x}{J \cdot G} & &
  // \textrm{ pour x variant de 0 à a}\nonumber\\
  // r_x(x) = & M_{Bx} \cdot \left( k_{Bx} + \frac{L-x}{J \cdot G} \right) & &
  // \textrm{ pour x variant de a à L}\end{align*}\begin{verbatim}
  // FinSi
  
  return TRUE;
}


gboolean
EF_charge_barre_ponctuelle_fonc_ry (Fonction    *f_rotation,
                                    Fonction    *f_deform,
                                    EF_Barre    *barre,
                                    unsigned int discretisation,
                                    double       a,
                                    double       fz,
                                    double       my,
                                    double       may,
                                    double       mby)
/**
 * \brief Calcule les déplacements d'une barre en rotation autour de l'axe y et
 *        en déformation selon l'axe z en fonction de la charge ponctuelle (fz
 *        et my) et des efforts aux extrémités de la poutre.
 * \param f_rotation : fonction où sera ajoutée la rotation,
 * \param f_deform : fonction où sera ajoutée la déformation,
 * \param barre : Barre à étudier,
 * \param discretisation : partie de la barre à étudier,
 * \param a : position de la charge par rapport au début de la partie de barre
 *            à étudier,
 * \param fz : force suivant l'axe z,
 * \param my : moment autour de l'axe y,
 * \param may : moment au début de la barre,
 * \param mby : moment à la fin de la barre.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - f_rotation == NULL,
 *     - f_deform == NULL,
 *     - barre == NULL,
 *     - discretisation>barre->discretisation_element,
 *     - a < 0. ou a > l,
 *     - en cas d'erreur due à une fonction interne.
 */
{
  EF_Noeud *debut, *fin;
  double    l, b;
  double    E, debut_barre, I;
  
  BUGPARAM (f_rotation, "%p", f_rotation, FALSE)
  BUGPARAM (f_deform, "%p", f_deform, FALSE)
  BUGPARAM (barre, "%p", barre, FALSE)
  INFO (discretisation <= barre->discretisation_element,
        FALSE,
        (gettext ("La discrétisation %d souhaitée est hors domaine %d.\n"),
                  discretisation,
                  barre->discretisation_element);)
  INFO (!((a < 0.) && (!(ERR (a, 0.)))),
        FALSE,
        (gettext ("La position de la charge ponctuelle %f est incorrecte.\n"),
                  a);)
  
  // La déformation en rotation d'une barre soumise à un effort de flexion
  // autour de l'axe y est calculée selon le principe des intégrales de Mohr et
  // est définie par les formules :\end{verbatim}\begin{center}
  // \includegraphics[width=8cm]{images/charge_barre_ponctuelle_fz.pdf}
  // \includegraphics[width=8cm]{images/charge_barre_ponctuelle_my.pdf}
  // \includegraphics[width=8cm]{images/charge_barre_ponctuelle_my_1.pdf}
  // \includegraphics[width=8cm]{images/charge_barre_ponctuelle_fz_1.pdf}
  // \includegraphics[width=8cm]{images/charge_barre_ponctuelle_may_mby.pdf}
  // \end{center}\begin{align*}
  // r_y(x) = & \int_0^x \frac{Mf_{11}(X) \cdot Mf_{31}(X)}{E \cdot I_y(X)} dX
  //          + \int_x^a \frac{Mf_{11}(X) \cdot Mf_{32}(X)}{E \cdot I_y(X)} dX
  //          + \int_a^L \frac{Mf_{12}(X) \cdot Mf_{32}(X)}{E \cdot I_y(X)} dX
  //          \nonumber\\
  //          &+\int_0^x \frac{Mf_{21}(X) \cdot Mf_{31}(X)}{E \cdot I_y(X)} dX+
  //          \int_x^a \frac{Mf_{21}(X) \cdot Mf_{32}(X)}{E \cdot I_y(X)} dX +
  //          \int_a^L \frac{Mf_{22}(X) \cdot Mf_{32}(X)}{E \cdot I_y(X)} dX
  //          \nonumber\\
  //          &+\int_0^x \frac{Mf_{5}(X) \cdot Mf_{31}(X)}{E \cdot I_y(X)} dX +
  //          \int_x^L \frac{Mf_{5}(X) \cdot Mf_{32}(X)}{E \cdot I_y(X)} dX
  //          \nonumber\\
  //          & \textrm{ pour x variant de 0 à a}\end{align*}\begin{align*}
  // r_y(x) = & \int_0^a \frac{Mf_{11}(X) \cdot Mf_{31}(X)}{E \cdot I_y(X)} dX+
  //          \int_a^x \frac{Mf_{12}(X) \cdot Mf_{31}(X)}{E \cdot I_y(X)} dX +
  //          \int_x^L \frac{Mf_{12}(X) \cdot Mf_{32}(X)}{E \cdot I_y(X)} dX
  //          \nonumber\\
  //          &+\int_0^a \frac{Mf_{21}(X) \cdot Mf_{31}(X)}{E \cdot I_y(X)} dX+
  //          \int_a^x \frac{Mf_{22}(X) \cdot Mf_{31}(X)}{E \cdot I_y(X)} dX +
  //          \int_x^L \frac{Mf_{22}(X) \cdot Mf_{32}(X)}{E \cdot I_y(X)} dX
  //          \nonumber\\
  //          &+\int_0^x \frac{Mf_{5}(X) \cdot Mf_{31}(X)}{E \cdot I_y(X)} dX +
  //          \int_x^L \frac{Mf_{5}(X) \cdot Mf_{32}(X)}{E \cdot I_y(X)} dX
  //          \nonumber\\
  //          &\textrm{ pour x variant de a à L}\end{align*}\begin{align*}
  // f_z(x) = & \int_0^x \frac{Mf_{11}(X) \cdot Mf_{41}(X)}{E \cdot I_y(X)} dX+
  //          \int_x^a \frac{Mf_{11}(X) \cdot Mf_{42}(X)}{E \cdot I_y(X)} dX +
  //          \int_a^L \frac{Mf_{12}(X) \cdot Mf_{42}(X)}{E \cdot I_y(X)} dX
  //          \nonumber\\
  //          &+\int_0^x \frac{Mf_{21}(X) \cdot Mf_{41}(X)}{E \cdot I_y(X)} dX+
  //          \int_x^a \frac{Mf_{21}(X) \cdot Mf_{42}(X)}{E \cdot I_y(X)} dX +
  //          \int_a^L \frac{Mf_{22}(X) \cdot Mf_{42}(X)}{E \cdot I_y(X)} dX
  //          \nonumber\\
  //          &+\int_0^x \frac{Mf_{5}(X) \cdot Mf_{41}(X)}{E \cdot I_y(X)} dX +
  //          \int_x^L \frac{Mf_{5}(X) \cdot Mf_{42}(X)}{E \cdot I_y(X)} dX
  //          \nonumber\\
  //          & \textrm{ pour x variant de 0 à a}\end{align*}\begin{align*}
  // f_z(x) = & \int_0^a \frac{Mf_{11}(X) \cdot Mf_{41}(X)}{E \cdot I_y(X)} dX+
  //          \int_a^x \frac{Mf_{12}(X) \cdot Mf_{41}(X)}{E \cdot I_y(X)} dX +
  //          \int_x^L \frac{Mf_{12}(X) \cdot Mf_{42}(X)}{E \cdot I_y(X)} dX
  //          \nonumber\\
  //          &+\int_0^a \frac{Mf_{21}(X) \cdot Mf_{41}(X)}{E \cdot I_y(X)} dX+
  //          \int_a^x \frac{Mf_{22}(X) \cdot Mf_{41}(X)}{E \cdot I_y(X)} dX +
  //          \int_x^L \frac{Mf_{22}(X) \cdot Mf_{42}(X)}{E \cdot I_y(X)} dX
  //          \nonumber\\
  //          &+\int_0^x \frac{Mf_{5}(X) \cdot Mf_{41}(X)}{E \cdot I_y(X)} dX +
  //          \int_x^L \frac{Mf_{5}(X) \cdot Mf_{42}(X)}{E \cdot I_y(X)} dX
  //          \nonumber\\
  //          &\textrm{ pour x variant de a à L}\end{align*}\begin{align*}
  // \textrm{ avec }
  //         Mf_{11}(X) = &-\frac{F_z \cdot b \cdot X}{L} &
  //         Mf_{12}(X) = & -F_z \cdot a \cdot \left( \frac{L-X}{L} \right)
  //         \nonumber\\
  //         Mf_{21}(X) = &-\frac{M_y \cdot X}{L} &
  //         Mf_{22}(X) = &\frac{M_y \cdot (L-X)}{L}\nonumber\\
  //         Mf_{31}(X) = &-\frac{X}{L} &
  //         Mf_{32}(X) = &\frac{L-X}{L}\nonumber\\
  //         Mf_{41}(X) = &-\frac{(L-x) \cdot X}{L} &
  //         Mf_{42}(X) = &-\frac{x \cdot (L-X)}{L}\nonumber\\
  //         Mf_{5}(X) = &\frac{M_{Ay} \cdot (L-X)}{L}-\frac{M_{By} \cdot X}{L}
  //         & & \end{align*}\begin{verbatim}
  
  if (discretisation == 0)
    debut = barre->noeud_debut;
  else
    debut = g_list_nth_data (barre->nds_inter, discretisation-1);
  if (discretisation == barre->discretisation_element)
    fin = barre->noeud_fin;
  else
    fin = g_list_nth_data (barre->nds_inter, discretisation);
  
  debut_barre = EF_noeuds_distance (barre->noeud_debut, debut);
  l = EF_noeuds_distance (debut, fin);
  BUG (!isnan (l), FALSE)
  INFO (!((a > l) && (!(ERR (a, l)))),
        FALSE,
        (gettext ("La position de la charge ponctuelle %f est incorrecte.\nLa longueur de la barre est de %f m.\n"),
                  a,
                  l);)
  
  b = l - a;
  E = m_g (EF_materiaux_E (barre->materiau));
  I = m_g (EF_sections_iy (barre->section));
  BUG (!isnan (E), FALSE)
  BUG (!isnan (I), FALSE)
  
  BUG (common_fonction_ajout_poly (
         f_rotation,
         0.,
         a,
         fz * b / (6 * E * I * l) * (-l * l + b * b),
         0.,
         fz * b / (2 * E * I * l),
         0., 0., 0., 0.,
         debut_barre),
       FALSE)
  BUG (common_fonction_ajout_poly (
         f_rotation,
         a,
         l,
         -fz * a / (6 * E * I * l) * (2 * l * l + a * a),
         fz * a / (E * I),
         -fz * a / (2 * E* I * l),
         0., 0., 0., 0.,
         debut_barre),
       FALSE)
  BUG (common_fonction_ajout_poly (
         f_rotation,
         0.,
         a,
         my / (6 * E * I * l) * (-l * l + 3 * b * b),
         0.,
         my / (2 * E * I * l),
         0., 0., 0., 0.,
         debut_barre),
       FALSE)
  BUG (common_fonction_ajout_poly (
         f_rotation,
         a,
         l,
         my / (6 * E * I * l) * (2 * l * l + 3 * a * a),
         -my / (E * I),
         my / (2 * E * I * l),
         0., 0., 0., 0.,
         debut_barre),
       FALSE)
  BUG (common_fonction_ajout_poly (
         f_rotation,
         0.,
         l,
         l / (6 * E * I) * (2 * may - mby),
         -may / (E * I),
         (may + mby) / (2 * E * I * l),
         0., 0., 0., 0.,
         debut_barre),
       FALSE)
  
  BUG (common_fonction_ajout_poly (
         f_deform,
         0.,
         a,
         0.,
         fz * b / (6 * E * I * l) * (l * l - b * b),
         0.,
         -fz * b / (6 * E * I * l),
         0., 0., 0.,
         debut_barre),
       FALSE)
  BUG (common_fonction_ajout_poly (
         f_deform,
         a,
         l,
         -fz * a * a * a / (6 * E * I),
         fz * a / (6 * E * I * l) * (a * a + 2 * l * l),
         -fz * a / (2 * E * I),
         fz * a / (6 * E * I * l),
         0., 0., 0.,
         debut_barre),
       FALSE)
  BUG (common_fonction_ajout_poly (
         f_deform,
         0.,
         a,
         0.,
         my / (6 * E * I * l) * (l * l - 3 * b * b),
         0.,
         -my / (6 * E * I * l),
         0., 0., 0.,
         debut_barre),
       FALSE)
  BUG (common_fonction_ajout_poly (
         f_deform,
         a,
         l,
         my / (6 * E * I) * (3 * a * a),
         -my / (6 * E * I * l) * (2 * l * l + 3 * a * a),
         my / (6 * E * I * l) * (3 * l),
         -my / (6 * E * I * l),
         0., 0., 0.,
         debut_barre),
       FALSE)
  BUG (common_fonction_ajout_poly (
         f_deform,
         0.,
         l,
         0.,
         l / (6 * E * I) * (-2 * may + mby),
         may / (2 * E * I),
         -(mby + may) / (6 * E * I * l),
         0., 0., 0.,
         debut_barre),
       FALSE)
  
  // Pour une section constante, les rotations valent :\end{verbatim}
  // \begin{align*}
  // r_y(x) = & \frac{F_z \cdot b}{6 \cdot E \cdot I_y \cdot L} [-L^2+b^2 +
  //          3 \cdot x^2] & &\\  & +\frac{M_y}{6 \cdot E \cdot I_y \cdot L}
  //          (-L^2+3 \cdot b^2 + 3 \cdot x^2) & &\\
  //          & +\frac{L}{6 \cdot E \cdot I_y} \cdot \left(2 \cdot M_{Ay}-
  //          M_{By} - \frac{6 \cdot M_{Ay}}{L} \cdot x + 3 \cdot \frac{M_{Ay}+
  //          M_{By}}{L^2} \cdot x^2 \right) &
  //          &\textrm{ pour x variant de 0 à a}\nonumber\\
  // r_y(x) = & -\frac{F_z \cdot a}{6 \cdot E \cdot I_y \cdot L}(2 \cdot
  //          L^2+a^2 -6 \cdot L \cdot x + 3 \cdot x^2) & &\\
  //          & +\frac{M_y}{6 \cdot E \cdot I_y \cdot L} \cdot (2 \cdot L^2+3
  //          \cdot a^2 -6 \cdot L \cdot x + 3 \cdot x^2) & &\\
  //          & +\frac{L}{6 \cdot E \cdot I_y} \cdot \left(2 \cdot M_{Ay}-
  //          M_{By} - \frac{6 \cdot M_{Ay}}{L} \cdot x + 3 \cdot \frac{M_{Ay}+
  //          M_{By}}{L^2} \cdot x^2 \right) &
  //          &\textrm{ pour x variant de a à L}\end{align*}\begin{align*}
  // f_z(x) = & \frac{F_z \cdot b \cdot x}{6 \cdot E \cdot I_y \cdot L} \cdot
  //          \left( L^2-b^2 - x^2 \right) & &\\
  //          & + \frac{M_y \cdot x}{6 \cdot E \cdot I_y \cdot L} \cdot
  //          \left( L^2-3 \cdot b^2 - x^2 \right) & &\\
  //          & + \frac{x}{6 \cdot E \cdot I_y} \cdot \left( L \cdot
  //          (-2 \cdot M_{Ay}+M_{By}) + 3 \cdot M_{Ay} \cdot x - \frac{M_{By}+
  //          M_{Ay}}{L} \cdot x^2 \right) &
  //          &\textrm{ pour x variant de 0 à a}\nonumber\\
  // f_z(x) = & \frac{F_z \cdot a}{6 \cdot E \cdot I_y \cdot L} \cdot
  //          \left( -a^2 \cdot L + (a^2+2 \cdot L^2) \cdot x - 3 \cdot L
  //          \cdot x^2 + x^3 \right) & &\\
  //          & + \frac{M_y}{6 \cdot E \cdot I_y \cdot L} \left(3 \cdot a^2
  //          \cdot L - (2 \cdot L^2+3 \cdot a^2) \cdot x + 3 \cdot L \cdot
  //          x^2 - x^3 \right) & &\\
  //          & + \frac{x}{6 \cdot E \cdot I_y} \cdot \left( L \cdot
  //          (-2 \cdot M_{Ay}+M_{By}) + 3 \cdot M_{Ay} \cdot x - \frac{M_{By}+
  //          M_{Ay}}{L} \cdot x^2 \right) &
  //          &\textrm{ pour x variant de a à L}\end{align*}\begin{verbatim}
  
  return TRUE;
}


gboolean
EF_charge_barre_ponctuelle_fonc_rz (Fonction    *f_rotation,
                                    Fonction    *f_deform,
                                    EF_Barre    *barre,
                                    unsigned int discretisation,
                                    double       a,
                                    double       fy,
                                    double       mz,
                                    double       maz,
                                    double       mbz)
/**
 * \brief Calcule les déplacements d'une barre en rotation autour de l'axe z et
 *        en déformation selon l'axe y en fonction de la charge ponctuelle (fy
 *        et mz) et des efforts aux extrémités de la poutre.
 * \param f_rotation : fonction où sera ajoutée la rotation,
 * \param f_deform : fonction où sera ajoutée la déformation,
 * \param barre : Barre à étudier,
 * \param discretisation : partie de la barre à étudier,
 * \param a : position de la charge par rapport au début de la partie de barre
 *            à étudier,
 * \param fy : force suivant l'axe y,
 * \param mz : moment autour de l'axe z,
 * \param maz : moment au début de la barre,
 * \param mbz : moment à la fin de la barre.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - f_rotation == NULL,
 *     - f_deform == NULL,
 *     - barre == NULL,
 *     - discretisation>barre->discretisation_element,
 *     - a < 0. ou a > l,
 *     - en cas d'erreur due à une fonction interne.
 */
{
  EF_Noeud *debut, *fin;
  double    l, b;
  double    E, debut_barre, I;
  
  BUGPARAM (f_rotation, "%p", f_rotation, FALSE)
  BUGPARAM (f_deform, "%p", f_deform, FALSE)
  BUGPARAM (barre, "%p", barre, FALSE)
  INFO (discretisation <= barre->discretisation_element,
        FALSE,
        (gettext ("La discrétisation %d souhaitée est hors domaine %d.\n"),
                  discretisation,
                  barre->discretisation_element);)
  INFO (!((a < 0.) && (!(ERR (a, 0.)))),
        FALSE,
        (gettext ("La position de la charge ponctuelle %f est incorrecte.\n"),
                  a);)
  
  // La déformation en rotation d'une barre soumise à un effort de flexion
  // autour de l'axe y est calculée selon le principe des intégrales de Mohr et
  // est définie par les mêmes formules que la fonction
  // #EF_charge_barre_ponctuelle_fonc_ry à ceci près que fz est remplacé par fy
  // (il y également un changement de signe de fy lors du calcul des rotations
  // dû au changement de repère), my par mz, may et mby par maz et mbz (il y
  // également un changement de signe de mz, maz et mbz lors du calcul des
  // déformations toujours dû au changement de repère) et Iy par Iz.
  
  if (discretisation == 0)
    debut = barre->noeud_debut;
  else
    debut = g_list_nth_data (barre->nds_inter, discretisation-1);
  if (discretisation == barre->discretisation_element)
    fin = barre->noeud_fin;
  else
    fin = g_list_nth_data (barre->nds_inter, discretisation);
  
  debut_barre = EF_noeuds_distance (barre->noeud_debut, debut);
  l = EF_noeuds_distance (debut, fin);
  BUG (!isnan (l), FALSE)
  INFO (!((a > l) && (!(ERR (a, l)))),
        FALSE,
        (gettext ("La position de la charge ponctuelle %f est incorrecte.\nLa longueur de la barre est de %f m.\n"),
                  a,
                  l);)
  
  b = l - a;
  E = m_g (EF_materiaux_E (barre->materiau));
  I = m_g (EF_sections_iz (barre->section));
  BUG (!isnan (E), FALSE)
  BUG (!isnan (I), FALSE)
  
  BUG (common_fonction_ajout_poly (
         f_rotation,
         0.,
         a,
         fy * b / (6 * E * I * l) * a * (l + b),
         0.,
         -fy * b / (2 * E * I * l),
         0., 0., 0., 0.,
         debut_barre),
       FALSE)
  BUG (common_fonction_ajout_poly (
         f_rotation,
         a,
         l,
         fy * a / (6 * E * I * l) * (2 * l * l + a * a),
         -fy * a / (E * I),
         fy * a / (2 * E * I * l),
         0., 0., 0., 0.,
         debut_barre),
       FALSE)
  BUG (common_fonction_ajout_poly (
         f_rotation,
         0.,
         a,
         mz / (6 * E * I * l) * (-l * l + 3 * b * b),
         0.,
         mz / (2 * E * I * l),
         0., 0., 0., 0.,
         debut_barre),
       FALSE)
  BUG (common_fonction_ajout_poly (
         f_rotation,
         a,
         l,
         mz / (6 * E * I * l) * (2 * l * l + 3 * a * a),
         -mz / (E * I),
         mz / (2 * E * I * l),
         0., 0., 0., 0.,
         debut_barre),
       FALSE)
  BUG (common_fonction_ajout_poly (
         f_rotation,
         0.,
         l,
         l / (6 * E * I) * (2 * maz - mbz),
         -maz / (E * I),
         (maz + mbz) / (2 * E * I * l),
         0., 0., 0., 0.,
         debut_barre),
       FALSE)
  
  BUG (common_fonction_ajout_poly (
         f_deform,
         0.,
         a,
         0.,
         fy * b / (6 * E * I * l) * (l * l - b * b),
         0.,
         -fy * b / (6 * E * I * l),
         0., 0., 0.,
         debut_barre),
       FALSE)
  BUG (common_fonction_ajout_poly (
         f_deform,
         a,
         l,
         -fy * a * a * a / (6 * E * I),
         fy * a / (6 * E * I * l) * (a * a + 2 * l * l),
         -fy * a / (2 * E * I),
         fy * a / (6 * E * I * l),
         0., 0., 0.,
         debut_barre),
       FALSE)
  BUG (common_fonction_ajout_poly (
         f_deform,
         0.,
         a,
         0.,
         mz / (6 * E * I * l) * (-l * l + 3 * b * b),
         0.,
         mz / (6 * E * I * l),
         0., 0., 0.,
         debut_barre),
       FALSE)
  BUG (common_fonction_ajout_poly (
         f_deform,
         a,
         l,
         -mz / (6 * E * I) * (3 * a * a),
         mz / (6 * E * I * l) * (2 * l * l + 3 * a * a),
         -mz / (6 * E * I * l) * (3 * l),
         mz / (6 * E * I * l),
         0., 0., 0.,
         debut_barre),
       FALSE)
  BUG (common_fonction_ajout_poly (
         f_deform,
         0.,
         l,
         0.,
         l / (6 * E * I) * (2 * maz - mbz),
         -maz / (2 * E * I),
         (mbz + maz) / (6 * E * I * l),
         0., 0., 0.,
         debut_barre),
       FALSE)
  
  return TRUE;
}


gboolean
EF_charge_barre_ponctuelle_n (Fonction    *fonction,
                              EF_Barre    *barre,
                              unsigned int discretisation,
                              double       a,
                              double       fax,
                              double       fbx)
/**
 * \brief Calcule les déplacements d'une barre selon l'axe x en fonction de
 *        l'effort normal ponctuel n et des réactions d'appuis fax et fbx.
 * \param fonction : fonction où sera ajoutée la déformée,
 * \param barre : Barre à étudier,
 * \param discretisation : partie de la barre à étudier,
 * \param a : position de la charge par rapport au début de la partie de barre,
 * \param fax : effort normal au début de la barre,
 * \param fbx : effort normal à la fin de la barre.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - barre == NULL,
 *     - fonction == NULL,
 *     - discretisation>barre->discretisation_element,
 *     - a < 0. ou a > l,
 *     - en cas d'erreur due à une fonction interne.
 */
{
  EF_Noeud *debut, *fin;
  double    l, debut_barre;
  double    E, S;
  
  BUGPARAM (fonction, "%p", fonction, FALSE)
  BUGPARAM (barre, "%p", barre, FALSE)
  INFO (discretisation <= barre->discretisation_element,
        FALSE,
        (gettext ("La discrétisation %d souhaitée est hors domaine %d.\n"),
                  discretisation,
                  barre->discretisation_element);)
  INFO (!((a < 0.) && (!(ERR (a, 0.)))),
        FALSE,
        (gettext ("La position de la charge ponctuelle %f est incorrecte.\n"),
                  a);)
  
  // La déformation selon l'axe x est par la formule :\end{verbatim}
  // \begin{center}
  // \includegraphics[width=8cm]{images/charge_barre_ponctuelle_n.pdf}
  // \end{center}\begin{align*}
  // f_x(x) = & F_{Ax} \cdot \int_0^x \frac{1}{E \cdot S(x)} dx &
  //          & \textrm{ pour x variant de 0 à a} \nonumber\\
  // f_x(x) = & F_{Ax} \cdot \int_0^a \frac{1}{E \cdot S(x)} dx - F_{Bx}
  //          \cdot \int_a^x \frac{1}{E \cdot S(x)} dx &
  //          & \textrm{ pour x variant de a à l} \end{align*}\begin{verbatim}
  
  if (discretisation == 0)
    debut = barre->noeud_debut;
  else
    debut = g_list_nth_data (barre->nds_inter, discretisation - 1);
  if (discretisation == barre->discretisation_element)
    fin = barre->noeud_fin;
  else
    fin = g_list_nth_data (barre->nds_inter, discretisation);
  
  debut_barre = EF_noeuds_distance (barre->noeud_debut, debut);
  l = EF_noeuds_distance (debut, fin);
  BUG (!isnan (l), FALSE)
  INFO (!((a > l) && (!(ERR (a, l)))),
        FALSE,
        (gettext ("La position de la charge ponctuelle %f est incorrecte.\nLa longueur de la barre est de %f m.\n"),
                  a,
                  l);)
  
  E = m_g (EF_materiaux_E (barre->materiau));
  S = m_g (EF_sections_s (barre->section));
  BUG (!isnan (E), FALSE)
  BUG (!isnan (S), FALSE)
  
  // Pour une section constante, les déformations valent :\end{verbatim}
  // \begin{align*}
  // f_x(x) = & \frac{F_{Ax} \cdot x}{E \cdot S} &
  //          & \textrm{ pour x variant de 0 à a}\nonumber\\
  // f_x(x) = & \frac{a \cdot (F_{Ax} + F_{Bx}) - F_{Bx} \cdot x}{E \cdot S} &
  //          & \textrm{ pour x variant de a à l}\end{align*}\begin{verbatim}
  
   BUG (common_fonction_ajout_poly (
          fonction,
          0.,
          a,
          0.,
          fax / (E * S),
          0., 0., 0., 0., 0.,
          debut_barre),
        FALSE)
   BUG (common_fonction_ajout_poly (
          fonction,
          a,
          l,
          a * (fax + fbx) / (E * S),
          -fbx / (E * S),
          0., 0., 0., 0., 0.,
          debut_barre),
        FALSE)
   
   return TRUE;
}


gboolean
EF_charge_barre_ponctuelle_enleve_barres (Charge *charge,
                                          GList  *barres,
                                          Projet *p)
/**
 * \brief Enlève à la charge une liste de barres pouvant être utilisées. Dans
 *        le cas où une barre de la liste n'est pas dans la charge, ce point ne
 *        sera pas considéré comme une erreur mais la barre sera simplement
 *        ignorée.
 * \param charge : la charge à modifier,
 * \param barres : la liste de pointers de type EF_Barre devant être retirés,
 * \param p : la variable projet.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - charge == NULL.
 */
{
  GList                   *list_parcours = barres;
  Charge_Barre_Ponctuelle *charge_d;
  
  BUGPARAM (charge, "%p", charge, FALSE)
  charge_d = charge->data;
  
  while (list_parcours != NULL)
  {
    EF_Barre *barre = list_parcours->data;
    
    charge_d->barres = g_list_remove (charge_d->barres, barre);
    
    list_parcours = g_list_next (list_parcours);
  }
  
#ifdef ENABLE_GTK
  if (UI_ACT.builder != NULL)
  {
    GtkTreeModel *model;
    GtkTreeIter   Iter;
    
    if (gtk_tree_selection_get_selected (GTK_TREE_SELECTION (
                                        gtk_builder_get_object (UI_ACT.builder,
                                       "1990_actions_treeview_select_action")),
                                         &model,
                                         &Iter))
    {
      Action *action;
      
      gtk_tree_model_get (model, &Iter, 0, &action, -1);
      
      if (g_list_find (_1990_action_charges_renvoie (action), charge))
        gtk_widget_queue_resize (GTK_WIDGET (UI_ACT.tree_view_charges));
    }
  }
#endif
  
  BUG (EF_calculs_free (p), FALSE)
  
  return TRUE;
}


gboolean
EF_charge_barre_ponctuelle_free (Charge *charge)
/**
 * \brief Libère une charge ponctuelle sur barre.
 * \param charge : la charge à libérer.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - charge == NULL.
 */
{
  Charge_Barre_Ponctuelle *charge_d;
  
  BUGPARAM (charge, "%p", charge, FALSE)
  charge_d = charge->data;
  
  free (charge->nom);
  g_list_free (charge_d->barres);
  free (charge_d);
  free (charge);
  
  return TRUE;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
