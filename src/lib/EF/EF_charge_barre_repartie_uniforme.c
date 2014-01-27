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
#include <math.h>
#include <string.h>

#include "1990_action.h"
#include "common_projet.h"
#include "common_erreurs.h"
#include "common_math.h"
#include "common_gtk.h"
#include "common_selection.h"
#include "common_fonction.h"
#include "EF_noeuds.h"
#include "EF_calculs.h"
#include "EF_sections.h"
#include "EF_materiaux.h"
#include "EF_charge.h"
#include "EF_gtk_charge_barre_repartie_uniforme.h"

Charge *
EF_charge_barre_repartie_uniforme_ajout (Projet     *p,
                                         Action     *action,
                                         GList      *barres,
                                         gboolean    repere_local,
                                         gboolean    projection,
                                         Flottant    a,
                                         Flottant    b,
                                         Flottant    fx,
                                         Flottant    fy,
                                         Flottant    fz,
                                         Flottant    mx,
                                         Flottant    my,
                                         Flottant    mz,
                                         const char *nom)
/**
 * \brief Ajoute une charge répartie uniforme à une action le long d'une barre.
 * \param p : la variable projet,
 * \param action : l'action qui contiendra la charge,
 * \param barres : liste des barres qui supportera la charge,
 * \param repere_local : TRUE si les charges doivent être prise dans le repère
 *                       local, FALSE pour le repère global,
 * \param projection : TRUE si la charge doit être projetée sur la barre, FALSE
 *                     si aucune projection. projection = TRUE est incompatible
 *                     avec repere_local = TRUE,
 * \param a : position en mètre de la charge par rapport au début de la barre,
 * \param b : position en mètre de la charge par rapport à la fin de la barre,
 * \param fx : force suivant l'axe x en N/m,
 * \param fy : force suivant l'axe y en N/m,
 * \param fz : force suivant l'axe z en N/m,
 * \param mx : moment autour de l'axe x en N.m/m,
 * \param my : moment autour de l'axe y en N.m/m,
 * \param mz : moment autour de l'axe z en N.m/m,
 * \param nom : nom de la charge.
 * \return
 *   Succès : pointeur vers la nouvelle charge.\n
 *   Échec : NULL :
 *     - p == NULL,
 *     - action == NULL,
 *     - barre == NULL,
 *     - projection == TRUE && repere_local == TRUE,
 *     - a < 0 ou a > l,
 *     - b < 0 ou b > l,
 *     - a > l - b,
 *     - en cas d'erreur d'allocation mémoire.
 */
{
  Charge *charge;
  Charge_Barre_Repartie_Uniforme  *charge_d;
  
  BUGMSG (p, NULL, gettext ("Paramètre %s incorrect.\n"), "projet")
  BUGMSG (action, NULL, gettext ("Paramètre %s incorrect.\n"), "action")
  BUGMSG (barres, NULL, gettext ("Paramètre %s incorrect.\n"), "barres")
  BUGMSG ((projection == FALSE) || (repere_local == FALSE),
          NULL,
          gettext ("Impossible d'effectuer une projection dans un repère local.\n"))
  BUGMSG(!((m_g (a) < 0.) && (!(ERR (m_g(a), 0.)))),
         NULL,
         gettext ("Le début de la position de la charge répartie uniformément (%f) est incorrect.\n"), m_g(a))
  BUGMSG (!((m_g (b) < 0.) && (!(ERR (m_g (b), 0.)))),
          NULL,
          gettext ("La fin de la position de la charge répartie uniformément (%f) est incorrecte.\n"), m_g (b))
  BUGMSG (charge = malloc (sizeof (Charge)),
          NULL,
          gettext ("Erreur d'allocation mémoire.\n"))
  BUGMSG (charge_d = malloc (sizeof (Charge_Barre_Repartie_Uniforme)),
          NULL,
          gettext ("Erreur d'allocation mémoire.\n"))
  charge->data = charge_d;
  if (barres != NULL)
  {
    GList *list_parcours = barres;
    do
    {
      EF_Barre *barre = list_parcours->data;
      double    l = EF_noeuds_distance (barre->noeud_debut, barre->noeud_fin);
      
      BUGMSG (!((m_g (a) > l) && (!(ERR (m_g (a), l)))),
              NULL,
              gettext ("Le début de la charge répartie uniformément (%f) est incorrect. La longueur de la barre %d est de %f m.\n"), m_g (a), barre->numero, l)
      BUGMSG (!((m_g (b) > l) && (!(ERR (m_g(b), l)))),
              NULL,
              gettext ("La fin de la charge répartie uniformément (%f) est incorrecte. La longueur de la barre %d est de %f m.\n"), m_g (b), barre->numero, l)
      BUGMSG (!((m_g (a) + m_g (b) > l) && (!(ERR (m_g (a) + m_g (b), l)))),
              NULL,
              gettext ("Le début (%f) et la fin (%f) de la charge répartie uniformément sont incompatible avec la longueur de la barre %d qui est de %f m.\n"), m_g (a), m_g (b), barre->numero, l)
      
      list_parcours = g_list_next (list_parcours);
    }
    while (list_parcours != NULL);
  }
  
  charge->type = CHARGE_BARRE_REPARTIE_UNIFORME;
  charge_d->barres = barres;
  charge_d->repere_local = repere_local;
  charge_d->projection = projection;
  charge_d->a = a;
  charge_d->b = b;
  charge_d->fx = fx;
  charge_d->fy = fy;
  charge_d->fz = fz;
  charge_d->mx = mx;
  charge_d->my = my;
  charge_d->mz = mz;
  
  BUG (EF_charge_ajout (p, action, charge, nom), NULL);
  
  return charge;
}


char *
EF_charge_barre_repartie_uniforme_description (Charge *charge)
/**
 * \brief Renvoie la description d'une charge de type répartie uniforme sur
 *        barre.
 * \param charge : la charge à décrire.
 * \return
 *   Succès : une chaîne de caractère.\n
 *   Échec : NULL :
 *     - charge == NULL,
 *     - en cas d'erreur d'allocation mémoire.
 */
{
  Charge_Barre_Repartie_Uniforme  *charge_d;
  char  txt_debut[30], txt_fin[30];
  char  txt_fx[30], txt_fy[30], txt_fz[30];
  char  txt_mx[30], txt_my[30], txt_mz[30];
  char  *txt_liste_barres, *description;
  
  BUGMSG (charge, NULL, gettext ("Paramètre %s incorrect.\n"), "charge")
  charge_d = charge->data;
  
  BUG (txt_liste_barres = common_selection_barres_en_texte (charge_d->barres),
       NULL)
  conv_f_c (charge_d->a, txt_debut, DECIMAL_DISTANCE);
  conv_f_c (charge_d->b, txt_fin, DECIMAL_DISTANCE);
  conv_f_c (charge_d->fx, txt_fx, DECIMAL_FORCE);
  conv_f_c (charge_d->fy, txt_fy, DECIMAL_FORCE);
  conv_f_c (charge_d->fz, txt_fz, DECIMAL_FORCE);
  conv_f_c (charge_d->mx, txt_mx, DECIMAL_MOMENT);
  conv_f_c (charge_d->my, txt_my, DECIMAL_MOMENT);
  conv_f_c (charge_d->mz, txt_mz, DECIMAL_MOMENT);
  
  BUGMSG (description = g_strdup_printf (
            "%s : %s, %s : %s m, %s : %s m, %s, %s, Fx : %s N/m, Fy : %s N/m, Fz : %s N/m, Mx : %s N.m/m, My : %s N.m/m, Mz : %s N.m/m",
            strstr (txt_liste_barres, ";") == NULL ?
              gettext ("Barre") :
              gettext("Barres"),
            txt_liste_barres,
            gettext ("début"),
            txt_debut,
            gettext ("fin (par rapport à la fin)"),
            txt_fin,
            charge_d->projection == TRUE ?
              gettext ("projection : oui") :
              gettext ("projection : non"),
            charge_d->repere_local ?
              gettext ("repère : local") :
              gettext ("repère : global"),
            txt_fx,
            txt_fy,
            txt_fz,
            txt_mx,
            txt_my,
            txt_mz),
          NULL,
          gettext ("Erreur d'allocation mémoire.\n"))
  
  free (txt_liste_barres);
  
  return description;
}


gboolean
EF_charge_barre_repartie_uniforme_mx (EF_Barre      *barre,
                                      unsigned int   discretisation,
                                      double         a,
                                      double         b,
                                      Barre_Info_EF *infos,
                                      double         mx,
                                      double        *ma,
                                      double        *mb)
/**
 * \brief Calcule l'opposé aux moments d'encastrement pour l'élément spécifié
 *        soumis au moment de torsion uniformément répartie mx dans le repère
 *        local. Les résultats sont renvoyés par l'intermédiaire des pointeurs
 *        ma et mb qui ne peuvent être NULL.
 * \param barre : Barre à étudier,
 * \param discretisation : partie de la barre à étudier,
 * \param a : début de la charge par rapport au début de la partie de barre
 *            étudiée,
 * \param b : fin de la charge par rapport à la fin de la partie de barre
 *            étudiée,
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
 *     - a < 0 ou a > l,
 *     - b < 0 ou b > l,
 *     - a > l-b.
 */
{
  EF_Noeud *debut, *fin;
  double    l, G, J;
  
  BUGMSG (barre, FALSE, gettext ("Paramètre %s incorrect.\n"), "barre")
  BUGMSG (infos, FALSE, gettext ("Paramètre %s incorrect.\n"), "infos")
  BUGMSG (ma, FALSE, gettext ("Paramètre %s incorrect.\n"), "ma")
  BUGMSG (mb, FALSE, gettext ("Paramètre %s incorrect.\n"), "mb")
  BUGMSG (discretisation <= barre->discretisation_element,
          FALSE,
          gettext ("La discrétisation %d souhaitée est hors domaine %d.\n"), discretisation, barre->discretisation_element)
  BUGMSG (!((ERR (infos->kAx, MAXDOUBLE)) && (ERR (infos->kBx, MAXDOUBLE))),
          FALSE,
          gettext ("Impossible de relâcher rx simultanément des deux cotés de la barre.\n"))
  BUGMSG (!((a < 0.) && (!(ERR (a, 0.)))),
          FALSE,
          gettext ("Le début de la charge répartie uniformément (%f) est incorrect.\n"), a)
  BUGMSG (!((b < 0.) && (!(ERR (b, 0.)))),
          FALSE,
          gettext ("La fin de la charge répartie uniformément (%f) est incorrecte.\n"), b)
  
  // Les moments aux extrémités de la barre sont déterminés par les intégrales
  // de Mohr et valent dans le cas général :\end{verbatim}\begin{center}
  //   \includegraphics[width=8cm]{images/charge_barre_rep_uni_mx.pdf}
  //   \end{center}\begin{align*}
  // M_{Bx} = & \frac{\int_0^a \frac{m_x \cdot (L-a-b)}{J(x) \cdot G } dx +
  //          \int_a^{L-b} \frac{m_x \cdot (L-b-x)}{J(x) \cdot G } dx +
  //          k_A \cdot m_x \cdot (L-a-b)}{\int_0^l \frac{1}{J(x) \cdot G}dx +
  //          k_A + k_B}\nonumber\\
  // M_{Ax} = & m_x \cdot (L-a-b) - M_{Bx}\end{align*}\begin{verbatim}
  
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
  BUGMSG (!((a > l) && (!(ERR (a, l)))),
          FALSE,
          gettext ("Le début de la charge répartie uniformément (%f) est incorrect. La longueur de la barre %d est de %f m.\n"), a, barre->numero, l)
  BUGMSG (!((b > l) && (!(ERR (b, l)))),
          FALSE,
          gettext ("La fin de la charge répartie uniformément (%f) est incorrecte. La longueur de la barre %d est de %f m.\n"), a, barre->numero, l)
  BUGMSG (!((a + b > l) && (!(ERR (a + b, l)))),
          FALSE,
          gettext ("Le début (%f) et la fin (%f) de la charge répartie uniformément sont incompatibles avec la longueur de la barre %d qui est de %f m.\n"), a, b, barre->numero, l)
  
  G = m_g (EF_materiaux_G (barre->materiau, FALSE));
  J = m_g (EF_sections_j (barre->section));
  BUG (!isnan (G), FALSE)
  BUG (!isnan (J), FALSE)
  
  // Pour une section section constante, les moments valent :\end{verbatim}
  // \begin{displaymath}
  // M_{Bx} =\frac{(L-a-b) \cdot m_x \cdot (a-b+2 \cdot G \cdot J \cdot k_A+l)}
  //   {2 \cdot (G \cdot J \cdot (k_A+k_B)+l)}\end{displaymath}\begin{verbatim}
  
  if (ERR (infos->kAx, MAXDOUBLE))
    *mb = mx * (l - a - b);
  else if (ERR (infos->kBx, MAXDOUBLE))
    *mb = 0.;
  else
    *mb = (l - a - b) * mx * (a - b + 2. * G * J * infos->kAx + l) /
            (2. * (G * J * (infos->kAx + infos->kBx) + l));
  *ma = mx * (l - a - b) - *mb;
  
  return TRUE;
}


gboolean
EF_charge_barre_repartie_uniforme_def_ang_iso_y (EF_Barre    *barre,
                                                 unsigned int discretisation,
                                                 double       a,
                                                 double       b,
                                                 double       fz,
                                                 double       my,
                                                 double      *phia,
                                                 double      *phib)
/**
 * \brief Calcule les angles de rotation pour un élément bi-articulé soumis à
 *        une charge uniformément répartie fz, my dans le repère local. Les
 *        résultats sont renvoyés par l'intermédiaire des pointeurs phia et
 *        phib qui ne peuvent être NULL.
 * \param *barre : Barre à étudier,
 * \param discretisation : partie de la barre à étudier,
 * \param a : début et fin de la charge uniformément répartie respectivement
 * \param b : par rapport au début et la fin de la partie de barre étudiée,
 * \param fz : charge linéaire suivant l'axe z,
 * \param my : moment linéaire autour de l'axe y,
 * \param phia : pointeur qui contiendra l'angle au début de la barre,
 * \param *phib : pointeur qui contiendra l'angle à la fin de la barre.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - barre == NULL,
 *     - phia == NULL,
 *     - phib == NULL,
 *     - discretisation>barre->discretisation_element,
 *     - a < 0 ou a > l,
 *     - b < 0 ou b > l,
 *     - a > l-b.
 */
{
  EF_Noeud *debut, *fin;
  double    l;
  double    E, I;
  
  BUGMSG (barre, FALSE, gettext ("Paramètre %s incorrect.\n"), "barre")
  BUGMSG (phia, FALSE, gettext ("Paramètre %s incorrect.\n"), "phia")
  BUGMSG (phib, FALSE, gettext ("Paramètre %s incorrect.\n"), "phib")
  BUGMSG (discretisation <= barre->discretisation_element,
          FALSE,
          gettext ("La discrétisation %d souhaitée est hors domaine %d.\n"), discretisation, barre->discretisation_element)
  
  // Les angles phi_A et phi_B sont déterminés par les intégrales de Mohr et
  // valent dans le cas général :\end{verbatim}\begin{center}
  // \includegraphics[width=8cm]{images/charge_barre_rep_uni_fz.pdf}
  // \includegraphics[width=8cm]{images/charge_barre_rep_uni_my.pdf}\par
  // \includegraphics[width=8cm]{images/charge_barre_ponctuelle_phiay.pdf}
  // \includegraphics[width=8cm]{images/charge_barre_ponctuelle_phiby.pdf}
  // \end{center}\begin{align*}
  // \varphi_A = & \int_0^a \frac{Mf_{11} \cdot Mf_0}{E \cdot I_y} dx +
  //             \int_a^{L-b} \frac{Mf_{12} \cdot Mf_0}{E \cdot I_y} dx +
  //             \int_{L-b}^L \frac{Mf_{13} \cdot Mf_0}{E \cdot I_y} dx
  //             \nonumber\\
  //             & + \int_0^a \frac{Mf_{21} \cdot Mf_0 }{E \cdot I_y} dx +
  //             \int_a^{L-b} \frac{Mf_{22} \cdot Mf_0}{E \cdot I_y} dx +
  //             \int_{L-b}^L \frac{Mf_{23} \cdot Mf_0}{E \cdot I_y} dx
  //             \nonumber\\
  // \varphi_B = & \int_0^a \frac{Mf_{11} \cdot Mf_3}{E \cdot I_y} dx +
  //             \int_a^{L-b} \frac{Mf_{12} \cdot Mf_3}{E \cdot I_y} dx +
  //             \int_{L-b}^L \frac{Mf_{13} \cdot Mf_3}{E \cdot I_y} dx
  //             \nonumber\\
  //             & + \int_0^a \frac{Mf_{21} \cdot Mf_3 }{E \cdot I_y} dx +
  //             \int_a^{L-b} \frac{Mf_{22} \cdot Mf_3}{E \cdot I_y} dx +
  //             \int_{L-b}^L \frac{Mf_{23} \cdot Mf_3}{E \cdot I_y} dx
  //             \texttt{ avec :}\end{align*}\begin{align*}
  // Mf_0 = & \frac{L-x}{L} &
  // Mf_{11} = & A_2 \cdot x\nonumber\\
  // Mf_{12} = & B_2 \cdot (L-x) + f_z \cdot \frac{(L-b-x)^2}{2} &
  // Mf_{13} = & B_2 \cdot (L-x) \nonumber\\
  // Mf_{21} = & A_1 \cdot x &
  // Mf_{22} = & \frac{(-A_1 \cdot a + B_1 \cdot b) \cdot x -
  //             a \cdot (b \cdot (A_1 + B_1)-L \cdot A_1)}{L-a-b} \nonumber\\
  // Mf_{23} = & B_1 \cdot (L-x) &
  // Mf_3 = & -\frac{x}{L} \nonumber\\
  // A_1 = & -m_y \cdot \frac{L-a-b}{L} &
  // B_1 = & m_y \cdot \frac{L-a-b}{L} \nonumber\\
  // A_2 = & -\frac{f_z \cdot (L-a-b)(L-a+b)}{2 \cdot L} &
  // B_2 = & -\frac{f_z \cdot (L-a-b)(L+a-b)}{2 \cdot L}
  // \end{align*}\begin{verbatim}
  
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
  BUGMSG (!((a < 0.) && (!(ERR (a, 0.)))),
          FALSE,
          gettext ("Le début de la position de la charge répartie uniformément (%f) est incorrect.\n"), a)
  BUGMSG (!((b < 0.) && (!(ERR (b, 0.)))),
          FALSE,
          gettext ("La fin de la position de la charge répartie uniformément (%f) est incorrecte.\n"), b)
  BUGMSG (!((a > l) && (!(ERR (a, l)))),
          FALSE,
          gettext ("Le début de la charge répartie uniformément (%f) est incorrect. La longueur de la barre %d est de %f m.\n"), a, barre->numero, l)
  BUGMSG (!((b > l) && (!(ERR (b, l)))),
          FALSE,
          gettext ("La fin de la charge répartie uniformément (%f) est incorrecte. La longueur de la barre %d est de %f m.\n"), b, barre->numero, l)
  BUGMSG (!((a + b > l) && (!(ERR (a + b, l)))),
          FALSE,
          gettext ("Le début (%f) et la fin (%f) de la charge répartie uniformément sont incorrecte avec la longueur de la barre %d qui est de %f m.\n"), a, b, barre->numero, l)
  
  E = m_g (EF_materiaux_E (barre->materiau));
  I = m_g (EF_sections_iy (barre->section));
  BUG (!isnan (E), FALSE)
  BUG (!isnan (I), FALSE)
  
  // Pour une section constante, les angles valent :\end{verbatim}
  // \begin{displaymath}
  // \varphi_A = \frac{[a^2-a \cdot (b+2 \cdot L) + b \cdot (b+L)] \cdot m_y
  //             \cdot (L-a-b)}{6 \cdot E \cdot I_y \cdot L} -
  //             \frac{f_z \cdot (L-a-b) \cdot (L-a+b) \cdot (L^2-a^2-b^2+2
  //             \cdot a \cdot L)}{24 \cdot E \cdot I_y \cdot L}
  // \end{displaymath}\begin{displaymath}
  // \varphi_B = \frac{[a^2+a \cdot (L-b) - b \cdot (2L-b)] \cdot m_y
  //             \cdot (L-a-b)}{6 \cdot E \cdot I_y \cdot L} +
  //             \frac{F \cdot (L-a-b) \cdot (L+a-b) \cdot (L^2-a^2-b^2+2
  //             \cdot b \cdot L)}{24 \cdot E \cdot I_y \cdot L}
  // \end{displaymath}\begin{verbatim}
  
  *phia = (a * a - a * (b + 2 * l) + b * (b + l)) * my * (l - a - b) /
            (6 * E * I * l) - fz * (l - a - b) * (l - a + b) *
            (l * l - a * a - b * b +  2 *a * l) / (24 * E * I * l);
  *phib = (a * a - b * (2 * l - b) + a * (l - b)) * my * (l - a - b) /
            (6 * E * I * l) + fz * (l - a - b) * (l + a - b) *
            (l * l - a * a - b * b + 2 * b * l) / (24 * E * I * l);
  
  return TRUE;
}


gboolean
EF_charge_barre_repartie_uniforme_def_ang_iso_z (EF_Barre    *barre,
                                                 unsigned int discretisation,
                                                 double       a,
                                                 double       b,
                                                 double       fy,
                                                 double       mz,
                                                 double      *phia,
                                                 double      *phib)
/**
 * \brief Calcule les angles de rotation pour un élément bi-articulé soumis au
 *        chargement fy et mz dans le repère local. Les résultats sont renvoyés
 *        par l'intermédiaire des pointeurs phia et phib qui ne peuvent être
 *        NULL.
 * \param barre : Barre à étudier,
 * \param discretisation : partie de la barre à étudier,
 * \param a : début de la charge par rapport au début de la partie de barre à
 *            étudier
 * \param b : fin de la charge par rapport à la fin de la partie de barre à
 *              étudier,
 * \param fy : charge linéaire suivant l'axe y,
 * \param mz : moment linéaire autour de l'axe z,
 * \param phia : pointeur qui contiendra l'angle au début de la barre,
 * \param phib : pointeur qui contiendra l'angle à la fin de la barre.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - barre == NULL,
 *     - phia == NULL,
 *     - phib == NULL,
 *     - discretisation>barre->discretisation_element,
 *     - a < 0 ou a > l,
 *     - b < 0 ou b > l,
 *     - a > l-b.
 */
{
  EF_Noeud *debut, *fin;
  double    l;
  double    E, I;
  
  BUGMSG (barre, FALSE, gettext ("Paramètre %s incorrect.\n"), "barre")
  BUGMSG (discretisation <= barre->discretisation_element,
          FALSE,
          gettext ("La discrétisation %d souhaitée est hors domaine %d.\n"), discretisation, barre->discretisation_element)
  BUGMSG (phia, FALSE, gettext ("Paramètre %s incorrect.\n"), "phia")
  BUGMSG (phib, FALSE, gettext ("Paramètre %s incorrect.\n"), "phib")
  
  if (discretisation == 0)
    debut = barre->noeud_debut;
  else
    debut = g_list_nth_data (barre->nds_inter, discretisation-1);
  if (discretisation == barre->discretisation_element)
    fin = barre->noeud_fin;
  else
    fin = g_list_nth_data (barre->nds_inter, discretisation);
  
  // Les angles phi_A et phi_B sont déterminés par les intégrales de Mohr et
  // valent dans le cas général :\end{verbatim}\begin{center}
  // \includegraphics[width=8cm]{images/charge_barre_rep_uni_fy.pdf}
  // \includegraphics[width=8cm]{images/charge_barre_rep_uni_mz.pdf}\par
  // \includegraphics[width=8cm]{images/charge_barre_ponctuelle_phiaz.pdf}
  // \includegraphics[width=8cm]{images/charge_barre_ponctuelle_phibz.pdf}
  // \end{center}\begin{align*}
  // \varphi_A = & \int_0^a \frac{Mf_{11} \cdot Mf_0}{E \cdot I_z} dx +
  //             \int_a^{L-b} \frac{Mf_{12} \cdot Mf_0}{E \cdot I_z} dx +
  //             \int_{L-b}^L \frac{Mf_{13} \cdot Mf_0}{E \cdot I_z} dx
  //             \nonumber\\
  //             & + \int_0^a \frac{Mf_{21} \cdot Mf_0 }{E \cdot I_z} dx +
  //             \int_a^{L-b} \frac{Mf_{22} \cdot Mf_0}{E \cdot I_z} dx +
  //             \int_{L-b}^L \frac{Mf_{23} \cdot Mf_0}{E \cdot I_z} dx
  //             \nonumber\\
  // \varphi_B = & \int_0^a \frac{Mf_{11} \cdot Mf_3}{E \cdot I_z} dx +
  //             \int_a^{L-b} \frac{Mf_{12} \cdot Mf_3}{E \cdot I_z} dx +
  //             \int_{L-b}^L \frac{Mf_{13} \cdot Mf_3}{E \cdot I_z} dx
  //             \nonumber\\
  //             & + \int_0^a \frac{Mf_{21} \cdot Mf_3 }{E \cdot I_z} dx +
  //             \int_a^{L-b} \frac{Mf_{22} \cdot Mf_3}{E \cdot I_z} dx +
  //             \int_{L-b}^L \frac{Mf_{23} \cdot Mf_3}{E \cdot I_z} dx
  //             \texttt{ avec :}\end{align*}\begin{align*}
  // Mf_0 = & \frac{L-x}{L} &
  // Mf_{11} = & A_2 \cdot x\nonumber\\
  // Mf_{12} = & B_2 \cdot (L-x) - f_z \cdot \frac{(L-b-x)^2}{2} &
  // Mf_{13} = & B_2 \cdot (L-x) \nonumber\\
  // Mf_{21} = & A_1 \cdot x &
  // Mf_{22} = & \frac{(-A_1 \cdot a + B_1 \cdot b) \cdot x -
  //              a \cdot (b \cdot (A_1 + B_1)-L \cdot A_1)}{L-a-b} \nonumber\\
  // Mf_{23} = & B_1 \cdot (L-x) &
  // Mf_3 = & -\frac{x}{L} \nonumber\\
  // A_1 = & -m_z \cdot \frac{L-a-b}{L} &
  // B_1 = & m_z \cdot \frac{L-a-b}{L} \nonumber\\
  // A_2 = & \frac{f_y \cdot (L-a-b)(L-a+b)}{2 \cdot L} &
  // B_2 = & \frac{f_y \cdot (L-a-b)(L+a-b)}{2 \cdot L}
  // \end{align*}\begin{verbatim}
  
  l = EF_noeuds_distance (debut, fin);
  BUG (!isnan (l), FALSE)
  BUGMSG (!((a < 0.) && (!(ERR (a, 0.)))),
          FALSE,
          gettext ("Le début de la position de la charge répartie uniformément (%f) est incorrect.\n"), a)
  BUGMSG (!((b < 0.) && (!(ERR (b, 0.)))),
          FALSE,
          gettext ("La fin de la position de la charge répartie uniformément (%f) est incorrecte.\n"), b)
  BUGMSG (!((a > l) && (!(ERR (a, l)))),
          FALSE,
          gettext ("Le début de la charge répartie uniformément (%f) est incorrect. La longueur de la barre %d est de %f m.\n"), a, barre->numero, l)
  BUGMSG (!((b > l) && (!(ERR (b, l)))),
          FALSE,
          gettext ("La fin de la charge répartie uniformément (%f) est incorrecte. La longueur de la barre %d est de %f m.\n"), b, barre->numero, l)
  BUGMSG (!((a + b > l) && (!(ERR (a + b, l)))),
          FALSE,
          gettext ("Le début (%f) et la fin (%f) de la charge répartie uniformément sont incorrecte avec la longueur de la barre %d qui est de %f m.\n"), a, b, barre->numero, l)
  
  E = m_g (EF_materiaux_E (barre->materiau));
  I = m_g (EF_sections_iz (barre->section));
  BUG (!isnan (E), FALSE)
  BUG (!isnan (I), FALSE)
  
  // Pour une section constante, les angles valent :\end{verbatim}
  // \begin{displaymath}
  // \varphi_A = \frac{[a^2-a \cdot (b+2 \cdot L) + b \cdot (b+L)] \cdot m_z
  //             \cdot (L-a-b)}{6 \cdot E \cdot I_z \cdot L} + \frac{f_y \cdot
  //             (L-a-b) \cdot (L-a+b) \cdot (L^2-a^2-b^2+2 \cdot a \cdot L)}
  //             {24 \cdot E \cdot I_z \cdot L}\end{displaymath}
  //             \begin{displaymath}
  // \varphi_B = \frac{[a^2+a \cdot (L-b) - b \cdot (2L-b)] \cdot m_z \cdot
  //             (L-a-b)}{6 \cdot E \cdot I_z \cdot L} - \frac{f_y \cdot
  //             (L-a-b) \cdot (L+a-b) \cdot (L^2-a^2-b^2+2 \cdot b \cdot L)}
  //             {24 \cdot E \cdot I_z \cdot L}\end{displaymath}
  // \begin{verbatim}
  
  *phia = (a * a - a * (b + 2 * l) + b * (b + l)) * mz * (l - a - b) /
            (6 * E * I * l) + fy * (l - a - b) * (l - a + b) *
            (l * l - a * a - b * b + 2 * a * l) / (24 * E * I * l);
  *phib = (a * a - b * (2 * l - b) + a * (l - b)) * mz * (l - a - b) /
            (6 * E * I * l) - fy * (l - a - b) * (l + a - b) *
            (l * l - a * a - b * b + 2 * b * l ) / (24 * E * I * l);
  
  return TRUE;
}


double
EF_charge_barre_repartie_uniforme_position_resultante_x (Section *section,
                                                         double   a,
                                                         double   b,
                                                         double   l)
/**
 * \brief Renvoie la position de la résultante pour une charge uniformément
 *        répartie vers l'axe x.
 * \param section : nature de la section,
 * \param a : position du début de la charge par rapport au début de la barre,
 * \param b : position de la fin de la charge par rapport à la fin de la barre,
 * \param l : longueur de la barre.
 * \return
 *   Succès : Résultat.\n
 *   Échec : NAN :
 *     - section == NULL,
 *     - a < 0 ou a > l,
 *     - b < 0 ou b > l,
 *     - a > l-b.
 */
{
  BUGMSG (section, NAN, gettext ("Paramètre %s incorrect.\n"), "section")
  BUGMSG (!((a < 0.) && (!(ERR (a, 0.)))),
          NAN,
          gettext ("Le début de la position de la charge répartie uniformément (%f) est incorrect.\n"), a)
  BUGMSG (!((b < 0.) && (!(ERR (b, 0.)))),
          NAN,
          gettext ("La fin de la position de la charge répartie uniformément (%f) est incorrecte.\n"), b)
  BUGMSG (!((a > l) && (!(ERR (a, l)))),
          NAN,
          gettext ("Le début de la charge répartie uniformément (%f) est incorrect. La longueur de la barre est de %f m.\n"), a, l)
  BUGMSG (!((b > l) && (!(ERR (b, l)))),
          NAN,
          gettext ("La fin de la charge répartie uniformément (%f) est incorrecte. La longueur de la barre est de %f m.\n"), b, l)
  BUGMSG (!((a + b > l) && (!(ERR (a + b, l)))),
          NAN,
          gettext ("Le début (%f) et la fin (%f) de la charge répartie uniformément sont incorrecte avec la longueur de la barre qui est de %f m.\n"), a, b, l)
  
  // La position de la résultante de la charge est obtenue en résolvant X dans
  // la formule :\end{verbatim}\begin{center}
  // \includegraphics[width=8cm]{images/charge_barre_rep_uni_n.pdf}
  // \end{center}\begin{displaymath}
  // \int_a^{L-b} \frac{x-a}{S(x)} dx + \int_{L-b}^X \frac{L-b-a}{S(x)} dx = 0
  // \end{displaymath}\begin{verbatim}
  
  // Pour une section rectantulaire de section constante, X vaut :
  // \end{verbatim}\begin{displaymath}
  // X = \frac{a+L-b}{2} \end{displaymath}\begin{verbatim}
  
  return (a + l - b) / 2.;
}


gboolean
EF_charge_barre_repartie_uniforme_fonc_rx (Fonction    *fonction,
                                           EF_Barre    *barre,
                                           unsigned int discretisation,
                                           double       a,
                                           double       b,
                                           double       max,
                                           double       mbx)
/**
 * \brief Calcule les déplacements d'une barre en rotation autour de l'axe x en
 *        fonction des efforts aux extrémités de la poutre.
 * \param fonction : fonction où sera ajoutée la déformée,
 * \param barre : barre à étudier,
 * \param discretisation : partie de la barre à étudier,
 * \param a : position du début de la charge par rapport au début de la barre,
 * \param b : position de la fin de la charge par rapport à la fin de la barre,
 * \param max : moment au début de la barre,
 * \param mbx : moment à la fin de la barre.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - fonction == NULL,
 *     - barre == NULL,
 *     - discretisation>barre->discretisation_element,
 *     - kAx == kBx == MAXDOUBLE,
 *     - a < 0 ou a > l,
 *     - b < 0 ou b > l,
 *     - a > l-b,
 *     - en cas d'erreur due à une fonction interne.
 */
{
  EF_Noeud      *debut, *fin;
  Barre_Info_EF *infos;
  double         l;
  double         G, debut_barre, J;
  
  BUGMSG (fonction, FALSE, gettext ("Paramètre %s incorrect.\n"), "fonction")
  BUGMSG (barre, FALSE, gettext ("Paramètre %s incorrect.\n"), "barre")
  BUGMSG (discretisation <= barre->discretisation_element,
          FALSE,
          gettext ("La discrétisation %d souhaitée est hors domaine %d.\n"), discretisation, barre->discretisation_element)
  infos = &(barre->info_EF[discretisation]);
  BUGMSG(!((ERR (infos->kAx, MAXDOUBLE)) && (ERR (infos->kBx, MAXDOUBLE))),
         FALSE,
         gettext ("Impossible de relâcher rx simultanément des deux cotés de la barre.\n"))
  
  // La déformation d'une barre soumise à un effort de torsion est défini par
  // les formules :\end{verbatim}\begin{center}
  //   \includegraphics[width=8cm]{images/charge_barre_rep_uni_mx2.pdf}
  //   \end{center}\begin{verbatim}
  // Si le noeud B est relaché en rotation Alors\end{verbatim}\begin{align*}
  // r_x(x) = & M_{Ax} \cdot \left( k_{Ax} + \int_0^x \frac{1}{J \cdot G} dx
  //          \right) & & \textrm{ pour x variant de 0 à a}\nonumber\\
  // r_x(x) = & k_{Ax} \cdot M_{Ax} + \int_0^a \frac{M_{Ax}}{J \cdot G} dx +
  //          \int_a^x \frac{Mf_1(X)}{J \cdot G} dX &
  //          & \textrm{ pour x variant de a à L-b} \nonumber\\
  // r_x(x) = & k_{Ax} \cdot M_{Ax} + \int_0^a \frac{M_{Ax}}{J \cdot G} dx +
  //          \int_a^{L-b} \frac{Mf_1(X)}{J \cdot G} dX - \int_{L-b}^x
  //          \frac{M_{Bx}}{J \cdot G} dX &
  //          & \textrm{ pour x variant de L-b à L}
  // \end{align*}\begin{verbatim}
  // Sinon \end{verbatim}\begin{align*}
  // r_x(x) = & k_{Bx} \cdot M_{Bx} + \int_{L-b}^L \frac{M_{Bx}}{J \cdot G} dx
  //          - \int_a^{L-b} \frac{Mf_1(x)}{J \cdot G} dX - \int_x^a
  //          \frac{M_{Ax}}{J \cdot G} dx &
  //          & \textrm{ pour x variant de 0 à a} \nonumber\\
  // r_x(x) = & k_{Bx} \cdot M_{Bx} + \int_{L-b}^L \frac{M_{Bx}}{J \cdot G} dx
  //          - \int_x^{L-b} \frac{Mf_1(x)}{J \cdot G} dX &
  //          & \textrm{ pour x variant de a à L-b} \nonumber\\
  // r_x(x) = & M_{Bx} \cdot \left( k_{Bx} + \int_x^L \frac{1}{J \cdot G} dx
  //          \right) & & \textrm{ pour x variant de L-b à L}
  // \end{align*}\begin{verbatim}
  // FinSi\end{verbatim}\begin{align*}
  // Mf_1(X) = & \frac{X \cdot (M_{Ax}+M_{Bx}) -a \cdot M_{Bx} - M_{Ax} \cdot
  //           (L-b)}{-L+b+a}\end{align*}\begin{verbatim}
  
  if (discretisation == 0)
    debut = barre->noeud_debut;
  else
    debut = g_list_nth_data (barre->nds_inter, discretisation-1);
  if (discretisation == barre->discretisation_element)
    fin = barre->noeud_fin;
  else
    fin = g_list_nth_data (barre->nds_inter, discretisation);
  
  debut_barre = EF_noeuds_distance (debut, barre->noeud_debut);
  BUG (!isnan (debut_barre), FALSE)
  l = EF_noeuds_distance (debut, fin);
  BUG (!isnan(l), FALSE)
  BUGMSG (!((a < 0.) && (!(ERR (a, 0.)))),
          FALSE,
          gettext ("Le début de la charge répartie uniformément (%f) est incorrect.\n"), a)
  BUGMSG (!((b < 0.) && (!(ERR (b, 0.)))),
          FALSE,
          gettext ("La fin de la charge répartie uniformément (%f) est incorrecte.\n"), b)
  BUGMSG (!((a > l) && (!(ERR (a, l)))),
          FALSE,
          gettext ("Le début de la charge répartie uniformément (%f) est incorrect. La longueur de la barre %d est de %f m.\n"), a, barre->numero, l)
  BUGMSG (!((b > l) && (!(ERR (b, l)))),
          FALSE,
          gettext ("La fin de la charge répartie uniformément (%f) est incorrecte. La longueur de la barre %d est de %f m.\n"), a, barre->numero, l)
  BUGMSG (!((a + b > l) && (!(ERR (a + b, l)))),
          FALSE,
          gettext ("Le début (%f) et la fin (%f) de la charge répartie uniformément sont incompatibles avec la longueur de la barre %d qui est de %f m.\n"), a, b, barre->numero, l)
  
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
           0.,
           0.,
           0.,
           0.,
           0.,
           debut_barre),
         FALSE)
    BUG (common_fonction_ajout_poly (
           fonction,
           a,
           l - b,
           max * infos->kAx +
             (a * a * (max + mbx)) / (2. * (a + b - l) * G * J),
           (-2 * a * mbx + 2 * b * max - 2 * l * max) /
             (2. * (a + b - l) * G * J),
           (max + mbx) / (2. * (a + b - l) * G * J),
           0.,
           0.,
           0.,
           0.,
           debut_barre),
         FALSE)
    BUG (common_fonction_ajout_poly (
           fonction,
           l - b,
           l,
           max * infos->kAx + (l + a - b) * (max + mbx) / (2. * G * J),
           (-2 * mbx) / (2. * G * J),
           0.,
           0.,
           0.,
           0.,
           0.,
           debut_barre),
         FALSE)
  }
  else
  {
    BUG (common_fonction_ajout_poly (
           fonction,
           0.,
           a,
           mbx * infos->kBx + ((max + mbx) * (-a + b) - l * (max - mbx)) /
             (2. * G * J),
           (2. * max) / (2. * G * J),
           0.,
           0.,
           0.,
           0.,
           0.,
           debut_barre),
          FALSE)
    BUG (common_fonction_ajout_poly (
           fonction,
           a,
           l - b,
           mbx * infos->kBx + (2. * a * l * mbx + b * b * max + b * b * mbx -
             2. * b * l * max + l * l * max - l * l * mbx) /
             (2. * (a + b - l) * G * J),
           (-2. * a * mbx + 2. * b * max - 2. * l * max) /
             (2. * (a + b - l) * G * J),
           (max + mbx) / (2. * (a + b - l) * G * J),
           0.,
           0.,
           0.,
           0.,
           debut_barre),
         FALSE)
    BUG (common_fonction_ajout_poly (
           fonction,
           l - b,
           l,
           mbx *(infos->kBx + l / (G * J)),
           -mbx / (G * J),
           0.,
           0.,
           0.,
           0.,
           0.,
           debut_barre),
         FALSE)
  }
  
  // Pour une section section constante, les moments valent :
  // Si le noeud B est relaché en rotation Alors\end{verbatim}\begin{align*}
  // r_x(x) = & M_{Ax} \cdot \left( k_{Ax} + \frac{x}{J \cdot G} \right) &
  //          & \textrm{ pour x variant de 0 à a}\nonumber\\
  // r_x(x) = & k_{Ax} \cdot M_{Ax} + \frac{(M_{Ax} + M_{Bx}) \cdot (a^2-2
  //          \cdot x \cdot (L-b) + x^2)}{2 \cdot (a+b-L) \cdot G \cdot J} -
  //          \frac{2 \cdot M_{Bx} \cdot x}{2 \cdot G \cdot J} &
  //          & \textrm{ pour x variant de a à L-b} \nonumber\\
  // r_x(x) = & k_{Ax} \cdot M_{Ax} + \frac{(M_{Ax} + M_{Bx}) \cdot (a-b+L) -
  //          2 \cdot M_{Bx} \cdot x}{2 \cdot G \cdot J} &
  //          & \textrm{ pour x variant de L-b à L}
  // \end{align*}\begin{verbatim}
  // Sinon \end{verbatim}\begin{align*}
  // r_x(x) = & k_{Bx} \cdot M_{Bx} + \frac{(M_{Ax} + M_{Bx}) \cdot (-a+b) -
  //          (M_{Ax} - M_{Bx}) \cdot L - 2 \cdot M_{Ax} \cdot x}
  //          {2 \cdot G \cdot J} &
  //          & \textrm{ pour x variant de 0 à a} \nonumber\\
  // r_x(x) = & k_{Bx} \cdot M_{Bx} + \frac{(M_{Ax} + M_{Bx}) \cdot (L-b-x)^2}
  //          {2 \cdot (a+b-L) \cdot G \cdot J} + \frac{(L-x) \cdot M_{Bx}}
  //          {J \cdot G} & & \textrm{ pour x variant de a à L-b} \nonumber\\
  // r_x(x) = & M_{Bx} \cdot \left( k_{Bx} + \frac{L-x}{J \cdot G} \right) &
  //          & \textrm{ pour x variant de L-b à L}\end{align*}\begin{verbatim}
  // FinSi
  
  return TRUE;
}


gboolean
EF_charge_barre_repartie_uniforme_fonc_ry (Fonction    *f_rotation,
                                           Fonction    *f_deform,
                                           EF_Barre    *barre,
                                           unsigned int discretisation,
                                           double       a,
                                           double       b,
                                           double       fz,
                                           double       my,
                                           double       may,
                                           double       mby)
/**
 * \brief Calcule les déplacements d'une barre en rotation autour de l'axe y et
 *        en déformation selon l'axe z en fonction de la charge linéaire (fz et
 *        my) et des efforts aux extrémités de la poutre.
 * \param f_rotation : fonction où sera ajoutée la rotation,
 * \param f_deform : fonction où sera ajoutée la déformée,
 * \param barre : Barre à étudier,
 * \param discretisation : partie de la barre à étudier,
 * \param a : position du début de la charge par rapport au début de la barre,
 * \param b : position de la fin de la charge par rapport à la fin de la barre,
 * \param fz : charge linéaire suivant l'axe z,
 * \param my : moment linéaire autour de l'axe y,
 * \param may : moment au début de la barre,
 * \param mby : moment à la fin de la barre.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE en cas de paramètres invalides :
 *     - f_rotation == NULL,
 *     - f_deform == NULL,
 *     - barre == NULL,
 *     - discretisation>barre->discretisation_element,
 *     - a < 0 ou a > l,
 *     - b < 0 ou b > l,
 *     - a > l-b.
 */
{
  EF_Noeud *debut, *fin;
  double    l;
  double    E, debut_barre, I;
  
  BUGMSG (f_rotation,
          FALSE,
          gettext ("Paramètre %s incorrect.\n"), "f_rotation")
  BUGMSG (f_deform, FALSE, gettext ("Paramètre %s incorrect.\n"), "f_deform")
  BUGMSG (barre, FALSE, gettext ("Paramètre %s incorrect.\n"), "barre")
  BUGMSG (discretisation <= barre->discretisation_element,
          FALSE,
          gettext ("La discrétisation %d souhaitée est hors domaine %d.\n"), discretisation, barre->discretisation_element)
  
  // La déformation et la  rotation d'une barre soumise à un effort de flexion
  // autour de l'axe y est calculée selon le principe des intégrales de Mohr et
  // est définie par les formules :\end{verbatim}\begin{center}
  // \includegraphics[width=8cm]{images/charge_barre_rep_uni_fz.pdf}
  // \includegraphics[width=8cm]{images/charge_barre_rep_uni_my.pdf}\par
  // \includegraphics[width=8cm]{images/charge_barre_ponctuelle_my_1.pdf}
  // \includegraphics[width=8cm]{images/charge_barre_ponctuelle_fz_1.pdf}\par
  // \includegraphics[width=8cm]{images/charge_barre_ponctuelle_may_mby.pdf}
  // \end{center}\begin{align*}
  // r_y(x) = & \int_0^x \frac{Mf_{11}(X) \cdot Mf_{31}(X)}{E \cdot I_y(X)} dX
  //          + \int_x^a \frac{Mf_{11}(X) \cdot Mf_{32}(X)}{E \cdot I_y(X)} dX
  //          + \int_a^{L-b} \frac{Mf_{12}(X) \cdot Mf_{32}(X)}
  //          {E \cdot I_y(X)} dX\nonumber\\
  //          & + \int_{L-b}^L \frac{Mf_{13}(X) \cdot Mf_{32}(X)}
  //          {E \cdot I_y(X)} dX + \int_0^x \frac{Mf_{21}(X) \cdot Mf_{31}(X)}
  //          {E \cdot I_y(X)} dX + \int_x^a \frac{Mf_{21}(X) \cdot Mf_{32}(X)}
  //          {E \cdot I_y(X)} dX\nonumber\\
  //          & + \int_a^{L-b} \frac{Mf_{22}(X) \cdot Mf_{32}(X)}
  //          {E \cdot I_y(X)} dX + \int_{L-b}^L \frac{Mf_{23}(X) \cdot
  //          Mf_{32}(X)}{E \cdot I_y(X)} dX\nonumber\\
  //          & + \int_0^x \frac{Mf_{5}(X) \cdot Mf_{31}(X)}{E \cdot I_y(X)} dX
  //          + \int_x^L \frac{Mf_{5}(X) \cdot Mf_{32}(X)}{E \cdot I_y(X)} dX
  //          \textrm{ pour x variant de 0 à a}\end{align*}\begin{align*}
  // r_y(x) = & \int_0^a \frac{Mf_{11}(X) \cdot Mf_{31}(X)}{E \cdot I_y(X)} dX
  //          + \int_a^x \frac{Mf_{12}(X) \cdot Mf_{31}(X)}{E \cdot I_y(X)} dX
  //          + \int_x^{L-b} \frac{Mf_{12}(X) \cdot Mf_{32}(X)}
  //          {E \cdot I_y(X)} dX\nonumber\\
  //          & + \int_{L-b}^L \frac{Mf_{13}(X) \cdot Mf_{32}(X)}
  //          {E \cdot I_y(X)} dX + \int_0^a \frac{Mf_{21}(X) \cdot Mf_{31}(X)}
  //          {E \cdot I_y(X)} dX + \int_a^x \frac{Mf_{22}(X) \cdot Mf_{31}(X)}
  //          {E \cdot I_y(X)} dX\nonumber\\
  //          & + \int_x^{L-b} \frac{Mf_{22}(X) \cdot Mf_{32}(X)}
  //          {E \cdot I_y(X)} dX + \int_{L-b}^L \frac{Mf_{23}(X)
  //          \cdot Mf_{32}(X)}{E \cdot I_y(X)} dX\nonumber\\
  //          & + \int_0^x \frac{Mf_{5}(X) \cdot Mf_{31}(X)}{E \cdot I_y(X)} dX
  //          + \int_x^L \frac{Mf_{5}(X) \cdot Mf_{32}(X)}{E \cdot I_y(X)} dX
  //          \textrm{ pour x variant de a à L-b}\end{align*}\begin{align*}
  // r_y(x) = & \int_0^a \frac{Mf_{11}(X) \cdot Mf_{31}(X)}{E \cdot I_y(X)} dX
  //          + \int_a^{L-b} \frac{Mf_{12}(X) \cdot Mf_{31}(X)}{E \cdot I_y(X)}
  //          dX + \int_{L-b}^x \frac{Mf_{13}(X) \cdot Mf_{31}(X)}
  //          {E \cdot I_y(X)} dX\nonumber\\
  //          & + \int_x^L \frac{Mf_{13}(X) \cdot Mf_{32}(X)}{E \cdot I_y(X)}
  //          dX + \int_0^a \frac{Mf_{21}(X) \cdot Mf_{31}(X)}{E \cdot I_y(X)}
  //          dX + \int_a^{L-b} \frac{Mf_{22}(X) \cdot Mf_{31}(X)}
  //          {E \cdot I_y(X)} dX\nonumber\\
  //          & + \int_{L-b}^x \frac{Mf_{23}(X) \cdot Mf_{31}(X)}
  //          {E \cdot I_y(X)} dX + \int_x^L \frac{Mf_{23}(X) \cdot Mf_{32}(X)}
  //          {E \cdot I_y(X)} dX\nonumber\\
  //          & + \int_0^x \frac{Mf_{5}(X) \cdot Mf_{31}(X)}{E \cdot I_y(X)} dX
  //          + \int_x^L \frac{Mf_{5}(X) \cdot Mf_{32}(X)}{E \cdot I_y(X)} dX
  //          \textrm{ pour x variant de L-b à L}\end{align*}\begin{align*}
  // f_z(x) = & \int_0^x \frac{Mf_{11}(X) \cdot Mf_{41}(X)}{E \cdot I_y(X)} dX
  //          + \int_x^a \frac{Mf_{11}(X) \cdot Mf_{42}(X)}{E \cdot I_y(X)} dX
  //          + \int_a^{L-b} \frac{Mf_{12}(X) \cdot Mf_{42}(X)}
  //          {E \cdot I_y(X)} dX\nonumber\\
  //          & + \int_{L-b}^L \frac{Mf_{13}(X) \cdot Mf_{42}(X)}
  //          {E \cdot I_y(X)} dX + \int_0^x \frac{Mf_{21}(X) \cdot Mf_{41}(X)}
  //          {E \cdot I_y(X)} dX + \int_x^a \frac{Mf_{21}(X) \cdot Mf_{42}(X)}
  //          {E \cdot I_y(X)} dX\nonumber\\
  //          & + \int_a^{L-b} \frac{Mf_{22}(X) \cdot Mf_{42}(X)}
  //          {E \cdot I_y(X)} dX + \int_{L-b}^L \frac{Mf_{23}(X)
  //          \cdot Mf_{42}(X)}{E \cdot I_y(X)} dX\nonumber\\
  //          & + \int_0^x \frac{Mf_{5}(X) \cdot Mf_{41}(X)}{E \cdot I_y(X)} dX
  //          + \int_x^L \frac{Mf_{5}(X) \cdot Mf_{42}(X)}{E \cdot I_y(X)} dX
  //          \textrm{ pour x variant de 0 à a}\end{align*}\begin{align*}
  // f_z(x) = & \int_0^a \frac{Mf_{11}(X) \cdot Mf_{41}(X)}{E \cdot I_y(X)} dX
  //          + \int_a^x \frac{Mf_{12}(X) \cdot Mf_{41}(X)}{E \cdot I_y(X)} dX
  //          + \int_x^{L-b} \frac{Mf_{12}(X) \cdot Mf_{42}(X)}
  //          {E \cdot I_y(X)} dX\nonumber\\
  //          & + \int_{L-b}^L \frac{Mf_{13}(X) \cdot Mf_{42}(X)}
  //          {E \cdot I_y(X)} dX + \int_0^a \frac{Mf_{21}(X) \cdot Mf_{41}(X)}
  //          {E \cdot I_y(X)} dX + \int_a^x \frac{Mf_{22}(X) \cdot Mf_{41}(X)}
  //          {E \cdot I_y(X)} dX\nonumber\\
  //          & + \int_x^{L-b} \frac{Mf_{22}(X) \cdot Mf_{42}(X)}
  //          {E \cdot I_y(X)} dX + \int_{L-b}^L \frac{Mf_{23}(X) \cdot
  //          Mf_{42}(X)}{E \cdot I_y(X)} dX\nonumber\\
  //          & + \int_0^x \frac{Mf_{5}(X) \cdot Mf_{41}(X)}{E \cdot I_y(X)} dX
  //          + \int_x^L \frac{Mf_{5}(X) \cdot Mf_{42}(X)}{E \cdot I_y(X)} dX
  //          \textrm{ pour x variant de a à L-b}\end{align*}\begin{align*}
  // f_z(x) = & \int_0^a \frac{Mf_{11}(X) \cdot Mf_{41}(X)}{E \cdot I_y(X)} dX
  //          + \int_a^{L-b} \frac{Mf_{12}(X) \cdot Mf_{41}(X)}{E \cdot I_y(X)}
  //          dX + \int_{L-b}^x \frac{Mf_{13}(X) \cdot Mf_{41}(X)}
  //          {E \cdot I_y(X)} dX\nonumber\\
  //          & + \int_x^L \frac{Mf_{13}(X) \cdot Mf_{42}(X)}{E \cdot I_y(X)}
  //          dX + \int_0^a \frac{Mf_{21}(X) \cdot Mf_{41}(X)}{E \cdot I_y(X)}
  //          dX + \int_a^{L-b} \frac{Mf_{22}(X) \cdot Mf_{41}(X)}
  //          {E \cdot I_y(X)} dX\nonumber\\
  //          & + \int_{L-b}^x \frac{Mf_{23}(X) \cdot Mf_{41}(X)}
  //          {E \cdot I_y(X)} dX + \int_x^L \frac{Mf_{23}(X) \cdot Mf_{42}(X)}
  //          {E \cdot I_y(X)} dX\nonumber\\
  //          & + \int_0^x \frac{Mf_{5}(X) \cdot Mf_{41}(X)}{E \cdot I_y(X)} dX
  //          + \int_x^L \frac{Mf_{5}(X) \cdot Mf_{42}(X)}{E \cdot I_y(X)} dX
  //          \textrm{ pour x variant de L-b à L}\end{align*}\begin{align*}
  //          \textrm{ avec } Mf_{11}(X) = &\frac{-f_z \cdot (L-a-b) \cdot
  //          (L-a+b) \cdot X}{2 \cdot L} \nonumber\\
  //          Mf_{12}(X) = & \frac{-f_z \cdot (L-a-b) \cdot (L+a-b) \cdot
  //          (L-X)}{2 \cdot L} + \frac{f_z \cdot (L-b-X)^2}{2}\nonumber\\
  //          Mf_{13}(X) = &-\frac{f_z \cdot (L-a-b) \cdot (L+a-b) \cdot
  //          (L-X)}{2 \cdot L} \nonumber\\
  //          Mf_{21}(X) = &-\frac{m_y \cdot (L-a-b) \cdot X}{L}\nonumber\\
  //          Mf_{22}(X) = &\frac{m_y \cdot (L-a-b) \cdot (L-X)}{L} -
  //          m_y \cdot (L-b-X) \nonumber\\
  //          Mf_{23}(X) = &\frac{m_y \cdot (L-a-b) \cdot (L-X)}{L}\nonumber\\
  //          Mf_{31}(X) = &-\frac{X}{L} \nonumber\\
  //          Mf_{32}(X) = &\frac{L-X}{L} \nonumber\\
  //          Mf_{41}(X) = &-\frac{(L-x) \cdot X}{L} \nonumber\\
  //          Mf_{42}(X) = &-\frac{x \cdot (L-X)}{L} \nonumber\\
  //          Mf_{5}(X)  = &-\frac{M_{Ay} \cdot (L-X)}{L}+\frac{M_{By} \cdot X}
  //          {L} \end{align*}\begin{verbatim}
  
  if (discretisation == 0)
    debut = barre->noeud_debut;
  else
    debut = g_list_nth_data (barre->nds_inter, discretisation-1);
  if (discretisation == barre->discretisation_element)
    fin = barre->noeud_fin;
  else
    fin = g_list_nth_data (barre->nds_inter, discretisation);
  
  debut_barre = EF_noeuds_distance (debut, barre->noeud_debut);
  BUG (!isnan (debut_barre), FALSE)
  l = EF_noeuds_distance (debut, fin);
  BUG (!isnan (l), FALSE)
  BUGMSG (!((a < 0.) && (!(ERR (a, 0.)))),
          FALSE,
          gettext ("Le début de la charge répartie uniformément (%f) est incorrect.\n"), a)
  BUGMSG (!((b < 0.) && (!(ERR (b, 0.)))),
          FALSE,
          gettext ("La fin de la charge répartie uniformément (%f) est incorrecte.\n"), b)
  BUGMSG (!((a > l) && (!(ERR (a, l)))),
          FALSE,
          gettext ("Le début de la charge répartie uniformément (%f) est incorrect. La longueur de la barre %d est de %f m.\n"), a, barre->numero, l)
  BUGMSG (!((b > l) && (!(ERR (b, l)))),
          FALSE,
          gettext ("La fin de la charge répartie uniformément (%f) est incorrecte. La longueur de la barre %d est de %f m.\n"), a, barre->numero, l)
  BUGMSG (!((a + b > l) && (!(ERR (a + b, l)))),
          FALSE,
          gettext ("Le début (%f) et la fin (%f) de la charge répartie uniformément sont incompatibles avec la longueur de la barre %d qui est de %f m.\n"), a, b, barre->numero, l)
  
  E = m_g (EF_materiaux_E (barre->materiau));
  I = m_g (EF_sections_iy (barre->section));
  BUG (!isnan (E), FALSE)
  BUG (!isnan (I), FALSE)
  
  BUG (common_fonction_ajout_poly (
         f_rotation,
         0.,
         a,
         -fz * (l - a - b) * (l - a + b) * (l * l - a * a - b * b + 2 * a * l)
           / (24. * l * E * I),
         0.,
         fz * (l - a - b) * (l - a + b) * (6.) / (24. * l * E * I),
         0.,
         0.,
         0.,
         0.,
         debut_barre),
       FALSE)
  BUG (common_fonction_ajout_poly (
         f_rotation,
         a,
         l - b,
         fz * (a * a * a * a + 4. * a * a * l * l - b * b * b * b +
           2. * b * b * l * l - l * l * l * l) / (24. * l * E * I),
         fz * (-12. * a * a * l) / (24. * l * E * I),
         fz * (6. * a * a - 6. * b * b + 6. * l * l) / (24. * l * E * I),
         fz * (-4. * l) / (24. * l * E * I),
         0.,
         0.,
         0.,
         debut_barre),
       FALSE)
  BUG (common_fonction_ajout_poly (
         f_rotation,
         l - b,
         l,
         fz * (a * a * a * a + 4. * a * a * l * l - b * b * b * b +
           4. * b * b * b * l - 10. * b * b * l * l + 12. * b * l * l * l -
           5. * l * l * l * l) / (24. * l * E * I),
         fz * (-12. * a * a * l + 12. * b * b * l - 24. * b * l * l +
           12. * l * l * l) / (24. * l * E * I),
         fz * (6. * a * a - 6. * b * b + 12. * b * l - 6. * l * l) /
           (24. * l * E * I),
         0.,
         0.,
         0.,
         0.,
         debut_barre),
       FALSE)
  BUG (common_fonction_ajout_poly (
         f_rotation,
         0.,
         a,
         my * (-a * a * a + 3. * a * a * l - 2. * a * l * l - b * b * b +
           b * l * l) / (6. * l * E * I),
         0.,
         my * (-3. * a - 3. * b + 3. * l) / (6. * l * E * I),
         0.,
         0.,
         0.,
         0.,
         debut_barre),
       FALSE)
  BUG (common_fonction_ajout_poly (
         f_rotation,
         a,
         l - b,
         my * (-a * a * a - 2. * a * l * l - b * b * b + b * l * l) /
           (6. * l * E * I),
         my * (6. * a * l) / (6. * l * E * I),
         my * (-3. * a - 3. * b) / (6. * l * E * I),
         0.,
         0.,
         0.,
         0.,
         debut_barre),
       FALSE)
  BUG (common_fonction_ajout_poly (
         f_rotation,
         l - b,
         l,
         my * (-a * a * a - 2. * a * l * l - b * b * b + 3. * b * b * l -
           5. * b * l *  l + 3. * l * l * l) / (6. * l * E * I),
         my * (6. * a * l + 6. * b * l - 6. * l * l) / (6. * l * E * I),
         my * (-3. * a - 3. * b + 3. * l) / (6. * l * E * I),
         0.,
         0.,
         0.,
         0.,
         debut_barre),
       FALSE)
  BUG (common_fonction_ajout_poly (
         f_rotation,
         0.,
         l,
         -l / (6. * E * I) * (2. * may - mby),
         may / (E * I),
         -(may + mby) / (2. * E * I * l),
         0.,
         0.,
         0.,
         0.,
         debut_barre),
       FALSE)
  
  BUG (common_fonction_ajout_poly (
         f_deform,
         0.,
         a,
         0.,
         fz / (24. * E * I * l) * (l - a - b) * (l - a + b) *
           (l * l - a * a + 2. * a * l - b * b),
         0.,
         fz / (24. * E * I * l) * (l - a - b) * (l - a + b) * (-2.),
         0.,
         0.,
         0.,
         debut_barre),
       FALSE)
  BUG (common_fonction_ajout_poly (
         f_deform,
         a,
         l - b,
         fz / (24. * E * I * l) * (a * a * a * a * l),
         fz / (24. * E * I * l) * (-a * a * a * a - 4. * a * a * l * l +
           b * b * b * b - 2. * b * b * l * l + l * l * l * l),
         fz / (24. * E * I * l) * (6. * a * a * l),
         fz / (24. * E * I * l) * (-2. * a * a + 2. * b * b - 2. * l * l),
         fz / (24. * E * I * l) * (l),
         0.,
         0.,
         debut_barre),
       FALSE)
  BUG (common_fonction_ajout_poly (
         f_deform,
         l - b,
         l,
         fz / (24. * E * I * l) * (l - a - b) * (l + a - b) *
           (-a * a * l - b * b * l + 2. * b * l * l - l * l * l),
         fz / (24. * E * I * l) * (l - a - b) * (l + a - b) *
           (a * a + b * b - 2. * b * l + 5. * l * l),
         fz / (24. * E * I * l) * (l - a - b) * (l + a - b) * (-6. * l),
         fz / (24. * E * I * l) * (l - a - b) * (l + a - b) * (2.),
         0.,
         0.,
         0.,
         debut_barre),
       FALSE)
  BUG (common_fonction_ajout_poly (
         f_deform,
         0.,
         a,
         0.,
         my / (6. * E * I * l) * (l - a - b) *
           (-a * a + a * b + 2. * a * l - b * b - b * l),
         0.,
         -my / (6. * E * I * l) * (l - a - b),
         0.,
         0.,
         0.,
         debut_barre),
       FALSE)
  BUG (common_fonction_ajout_poly (
         f_deform,
         a,
         l - b,
         my / (6. * E * I * l) * (-a * a * a * l),
         my / (6. * E * I * l) *
           (a * a * a + 2. * a * l * l + b * b * b - b * l * l),
         my / (6. * E * I * l) * (-3. * a * l),
         my / (6. * E * I * l) * (a + b),
         0.,
         0.,
         0.,
         debut_barre),
       FALSE)
  BUG (common_fonction_ajout_poly (
         f_deform,
         l - b,
         l,
         my / (6. * E * I * l) * (l - a - b) * (a * a * l - a * b * l +
           a * l * l + b * b * l - 2. * b * l * l + l * l * l),
         my / (6. * E * I * l) * (l - a - b) *
           (-a * a + a * b - a * l - b * b + 2. * b * l - 3. * l * l),
         my / (6. * E * I * l) * (l - a - b) * (3. * l),
         -my / (6. * E * I * l) * (l - a - b),
         0.,
         0.,
         0.,
         debut_barre),
       FALSE)
  BUG (common_fonction_ajout_poly (
         f_deform,
         0.,
         l,
         0.,
         l / (6. * E * I) * (2. * may - mby),
         -may / (2. * E * I),
         (mby + may) / (6. * E * I * l),
         0.,
         0.,
         0.,
         debut_barre),
       FALSE)
  
  // Pour une section constante, les formules valent :
  // \end{verbatim}\begin{align*}
  // r_y(x) = & \frac{f_z \cdot (L-a-b) \cdot (L-a+b) \cdot (-L^2 + a^2 + b^2 -
  //          2 \cdot a \cdot L + 6 \cdot x^2)}{24 \cdot E \cdot I_y \cdot L}
  //          \nonumber\\
  //          & + \frac{m_y \cdot (L-a-b) \cdot (a^2-a \cdot (b + 2 \cdot L) +
  //          b^2 + b \cdot L + 3 \cdot x^2)}{6 \cdot E \cdot I_y \cdot L}
  //          \nonumber\\
  //          & - \frac{L^2 \cdot (2 \cdot M_{Ay} + m_{By}) - 6 \cdot L \cdot
  //          M_{Ay} \cdot x + 3 \cdot (M_{Ay} + M_{By} \cdot x^2)}
  //          {6 \cdot E \cdot I_y \cdot L}\textrm{ pour x variant de 0 à a}
  //          \nonumber\\
  // r_y(x) = & - f_z \cdot \frac{4 \cdot L \cdot x^3 - 6 \cdot x^2 \cdot 
  //          (L^2+a^2-b^2)+12 \cdot a^2 \cdot L \cdot x+L^4-2 \cdot b^2 \cdot
  //          L^2-4 \cdot a^2 \cdot L^2+b^4-a^4}{24 \cdot L \cdot E \cdot I_y}
  //          \nonumber\\
  //          & +\frac{m_y \cdot (-a^3-a \cdot(2 \cdot L^2 - 6 \cdot L \cdot x
  //          + 3 \cdot x^2) + b \cdot (b^2 - L^2 + 3 \cdot x^2))}
  //          {6 \cdot E \cdot I_y \cdot L}\nonumber\\
  //          & - \frac{L^2 \cdot (2 \cdot M_{Ay} + m_{By}) - 6 \cdot L \cdot
  //          M_{Ay} \cdot x + 3 \cdot (M_{Ay} + M_{By} \cdot x^2)}
  //          {6 \cdot E \cdot I_y \cdot L}\textrm{ pour x variant de a à L-b}
  //          \nonumber\\
  // r_y(x) = & -f_z \cdot \frac{(L-b-a) \cdot (L-b+a) \cdot (6 \cdot x^2-12
  //          \cdot L \cdot x+5 \cdot L^2-2 \cdot b \cdot L+b^2+a^2)}
  //          {24 \cdot L \cdot E \cdot I_y})\nonumber\\
  //          & +\frac{m_y \cdot (L-a-b) \cdot (a^2 + a \cdot (L-b) + b^2 -
  //          2 \cdot b \cdot L + 3 \cdot (L-x)^2)}
  //          {6 \cdot E \cdot I_y \cdot L}\nonumber\\
  //          & - \frac{L^2 \cdot (2 \cdot M_{Ay} + m_{By}) - 6 \cdot L \cdot
  //          M_{Ay} \cdot x + 3 \cdot (M_{Ay} + M_{By} \cdot x^2)}
  //          {6 \cdot E \cdot I_y \cdot L}\textrm{ pour x variant de L-b à L}
  //          \end{align*}\begin{align*}
  // f_z(x) = & \frac{f_z \cdot (L-b-a) \cdot (L+b-a) \cdot x \cdot
  //          (-2 \cdot x^2+L^2+2 \cdot a \cdot L-b^2-a^2)}
  //          {24 \cdot L \cdot E \cdot I_y}\nonumber\\
  //          & -\frac{(L-b-a) \cdot my \cdot x \cdot (x^2+b \cdot L-2 \cdot a
  //          \cdot L+b^2-a \cdot b+a^2)}{6 \cdot L \cdot E \cdot I_y}
  //          \nonumber\\
  //          & x \cdot \frac{L^2 \cdot (2 \cdot M_{Ay} - M_{By}) -
  //          3 \cdot L \cdot M_{Ay} \cdot x + (M_{Ay} + M_{By}) \cdot x^2}
  //          {6 \cdot E \cdot I_y} \textrm{ pour x variant de 0 à a}
  //          \nonumber\\
  // f_z(x) = & \frac{f_z \cdot (L \cdot x^4+(-2 L^2+2 b^2-2 a^2) \cdot x^3+
  //          6  a^2 \cdot L \cdot x^2+(L^4-(2 b^2+4 a^2) \cdot L^2+b^4-a^4)
  //          \cdot x+a^4 \cdot L)}{24 \cdot L \cdot E \cdot I_y} \nonumber\\
  //          & + \frac{my \cdot (b \cdot x^3+a \cdot x^3-3 \cdot a \cdot L
  //          \cdot x^2-b \cdot L^2 \cdot x+2 \cdot a \cdot L^2 \cdot x+b^3
  //          \cdot x+a^3 \cdot x-a^3 \cdot L)}{6 \cdot L \cdot E \cdot I_y}
  //          \nonumber\\
  //          & x \cdot \frac{L^2 \cdot (2 \cdot M_{Ay} - M_{By}) -
  //          3 \cdot L \cdot M_{Ay} \cdot x + (M_{Ay} + M_{By}) \cdot x^2}
  //          {6 \cdot E \cdot I_y} \textrm{ pour x variant de a à L-b}
  //          \nonumber\\
  // f_z(x) = & \frac{f_z \cdot (L-b-a) \cdot (L-b+a) \cdot (x-L) \cdot
  //          (2 \cdot x^2-4 \cdot L \cdot x+L^2-2 \cdot b \cdot L+b^2+a^2)}
  //          {24 \cdot L \cdot E \cdot I_y} \nonumber\\
  //          & -\frac{(L-b-a) \cdot m_y \cdot (x-L) \cdot (x^2-2 \cdot L \cdot
  //          x+L^2-2 \cdot b \cdot L+a \cdot L+b^2-a \cdot b+a^2)}
  //          {6 \cdot L \cdot E \cdot I_y} \nonumber\\
  //          & x \cdot \frac{L^2 \cdot (2 \cdot M_{Ay} - M_{By}) -
  //          3 \cdot L \cdot M_{Ay} \cdot x + (M_{Ay} + M_{By}) \cdot x^2}
  //          {6 \cdot E \cdot I_y} \textrm{ pour x variant de L-b à L}
  //          \end{align*}\begin{verbatim}
  
  return TRUE;
}


gboolean
EF_charge_barre_repartie_uniforme_fonc_rz (Fonction    *f_rotation,
                                           Fonction    *f_deform,
                                           EF_Barre    *barre,
                                           unsigned int discretisation,
                                           double       a,
                                           double       b,
                                           double       fy,
                                           double       mz,
                                           double       maz,
                                           double       mbz)
/**
 * \brief Calcule les déplacements d'une barre en rotation autour de l'axe z et
 *        en déformation selon l'axe y en fonction de la charge linéaire (fy et
 *        mz) et des efforts aux extrémités de la poutre.
 * \param f_rotation : fonction où sera ajoutée la rotation,
 * \param f_deform : fonction où sera ajoutée la déformée,
 * \param barre : Barre à étudier,
 * \param discretisation : partie de la barre à étudier,
 * \param a : position du début de la charge par rapport au début de la barre,
 * \param b : position de la fin de la charge par rapport à la fin de la barre,
 * \param fy : charge linéaire suivant l'axe y,
 * \param mz : moment linéaire  autour de l'axe z,
 * \param maz : moment au début de la barre,
 * \param mbz : moment à la fin de la barre.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - f_rotation == NULL,
 *     - f_deform == NULL,
 *     - barre == NULL,
 *     - discretisation>barre->discretisation_element,
 *     - a < 0 ou a > l,
 *     - b < 0 ou b > l,
 *     - a > l-b.
 */
{
  EF_Noeud *debut, *fin;
  double    l;
  double    E, debut_barre, I;
  
  BUGMSG (f_rotation,
          FALSE,
          gettext ("Paramètre %s incorrect.\n"), "f_rotation")
  BUGMSG (f_deform, FALSE, gettext ("Paramètre %s incorrect.\n"), "f_deform")
  BUGMSG (barre, FALSE, gettext ("Paramètre %s incorrect.\n"), "barre")
  BUGMSG (discretisation <= barre->discretisation_element,
          FALSE,
          gettext ("La discrétisation %d souhaitée est hors domaine %d.\n"), discretisation, barre->discretisation_element)
  
  // La déformation et la rotation d'une barre soumise à un effort de flexion
  // autour de l'axe y est calculée selon le principe des intégrales de Mohr et
  // est définie par les mêmes formules que la fonction
  // #EF_charge_barre_repartie_uniforme_fonc_ry à ceci près que fz est remplacé
  // par fy (il y également un changement de signe de fy lors du calcul des
  // rotations dû au changement de repère), my par mz, may et mby par maz et
  // mbz (il y également un changement de signe de mz, maz et mbz lors du
  // calcul des déformations toujours dû au changement de repère) et Iy par Iz.
  
  if (discretisation == 0)
    debut = barre->noeud_debut;
  else
    debut = g_list_nth_data (barre->nds_inter, discretisation-1);
  if (discretisation == barre->discretisation_element)
    fin = barre->noeud_fin;
  else
    fin = g_list_nth_data (barre->nds_inter, discretisation);
  
  debut_barre = EF_noeuds_distance (debut, barre->noeud_debut);
  BUG (!isnan (debut_barre), FALSE)
  l = EF_noeuds_distance (debut, fin);
  BUG (!isnan(l), FALSE)
  BUGMSG (!((a < 0.) && (!(ERR (a, 0.)))),
          FALSE,
          gettext ("Le début de la charge répartie uniformément (%f) est incorrect.\n"), a)
  BUGMSG (!((b < 0.) && (!(ERR (b, 0.)))),
          FALSE,
          gettext ("La fin de la charge répartie uniformément (%f) est incorrecte.\n"), b)
  BUGMSG (!((a > l) && (!(ERR (a, l)))),
          FALSE,
          gettext ("Le début de la charge répartie uniformément (%f) est incorrect. La longueur de la barre %d est de %f m.\n"), a, barre->numero, l)
  BUGMSG (!((b > l) && (!(ERR (b, l)))),
          FALSE,
          gettext ("La fin de la charge répartie uniformément (%f) est incorrecte. La longueur de la barre %d est de %f m.\n"), a, barre->numero, l)
  BUGMSG (!((a + b > l) && (!(ERR (a + b, l)))),
          FALSE,
          gettext ("Le début (%f) et la fin (%f) de la charge répartie uniformément sont incompatibles avec la longueur de la barre %d qui est de %f m.\n"), a, b, barre->numero, l)
  
  E = m_g (EF_materiaux_E (barre->materiau));
  I = m_g (EF_sections_iz (barre->section));
  BUG (!isnan (E), FALSE)
  BUG (!isnan (I), FALSE)
  
  BUG (common_fonction_ajout_poly (
         f_rotation,
         0.,
         a,
         fy * (l - a - b) * (l - a + b) * (l * l - a * a - b * b + 2 * a * l) /
           (24. * l * E * I),
         0.,
         -fy * (l - a - b) * (l - a + b) * (6.) / (24. * l * E * I),
         0.,
         0.,
         0.,
         0.,
         debut_barre),
       FALSE)
  BUG (common_fonction_ajout_poly (
         f_rotation,
         a,
         l - b,
         -fy * (a * a * a * a + 4. * a * a * l * l - b * b * b * b +
           2. * b * b * l * l - l * l * l * l) / (24. * l * E * I),
         -fy * (-12. * a * a * l) / (24. * l * E * I),
         -fy * (6. * a * a - 6. * b * b + 6. * l * l) / (24. * l * E * I),
         -fy * (-4. * l) / (24. * l * E * I),
         0.,
         0.,
         0.,
         debut_barre),
       FALSE)
  BUG (common_fonction_ajout_poly (
         f_rotation,
         l - b,
         l,
         -fy * (a * a * a * a + 4. * a * a * l * l - b * b * b * b +
           4. * b * b * b * l - 10. * b * b * l * l + 12. * b * l * l * l -
           5. * l * l * l * l) / (24. * l * E * I),
         -fy * (-12. * a * a * l + 12. * b * b * l - 24. * b * l * l +
           12. * l * l * l) / (24. * l * E * I),
         -fy * (6. * a * a - 6. * b * b + 12. * b * l - 6. * l * l) /
           (24. * l * E * I),
         0.,
         0.,
         0.,
         0.,
         debut_barre),
       FALSE)
  BUG (common_fonction_ajout_poly (
         f_rotation,
         0.,
         a,
         mz * (-a * a * a + 3. * a * a * l - 2. * a * l * l - b * b * b +
           b * l * l) / (6. * l * E * I),
         0.,
         mz * (-3. * a - 3. * b + 3. * l) / (6. * l * E * I),
         0.,
         0.,
         0.,
         0.,
         debut_barre),
       FALSE)
  BUG (common_fonction_ajout_poly (
         f_rotation,
         a,
         l - b,
         mz * (-a * a * a - 2. * a * l * l - b * b * b + b * l * l) /
           (6. * l * E * I),
         mz * (6. * a * l) / (6. * l * E * I),
         mz * (-3. * a - 3. * b) / (6. * l * E * I),
         0.,
         0.,
         0.,
         0.,
         debut_barre),
       FALSE)
  BUG (common_fonction_ajout_poly (
         f_rotation,
         l - b,
         l,
         mz * (-a * a * a - 2. * a * l * l - b * b * b + 3. * b * b * l -
           5. * b * l * l + 3. * l * l * l) / (6. * l * E * I),
         mz * (6. * a * l + 6. * b * l - 6. * l * l) / (6. * l * E * I),
         mz * (-3. * a - 3. * b + 3. * l) / (6. * l * E * I),
         0.,
         0.,
         0.,
         0.,
         debut_barre),
       FALSE)
  BUG (common_fonction_ajout_poly (
         f_rotation,
         0.,
         l,
         -l / (6. * E * I) * (2. * maz - mbz),
         maz / (E * I),
         -(maz + mbz) / (2. * E * I * l),
         0.,
         0.,
         0.,
         0.,
         debut_barre),
       FALSE)
  
  BUG (common_fonction_ajout_poly (
         f_deform,
         0.,
         a,
         0.,
         fy / (24. * E * I * l) * (l - a - b) * (l - a + b) *
           (l * l - a * a + 2. * a * l - b * b),
         0.,
         fy / (24. * E * I * l) * (l - a - b) * (l - a + b) * (-2.),
         0.,
         0.,
         0.,
         debut_barre),
       FALSE)
  BUG (common_fonction_ajout_poly (
         f_deform,
         a,
         l - b,
         fy / (24. * E * I * l) * (a * a * a * a * l),
         fy / (24. * E * I * l) * (-a * a * a * a - 4. * a * a * l * l +
           b * b * b * b - 2. * b * b * l * l + l * l * l * l),
         fy / (24. * E * I * l) * (6. * a * a * l),
         fy / (24. * E * I * l) * (-2. * a * a + 2. * b * b - 2. * l * l),
         fy / (24. * E * I * l) * (l),
         0.,
         0.,
         debut_barre),
       FALSE)
  BUG (common_fonction_ajout_poly (
         f_deform,
         l - b,
         l,
         fy / (24. * E * I * l) * (l - a - b) * (l + a - b) *
           (-a * a * l - b * b * l + 2. * b * l * l - l * l * l),
         fy / (24. * E * I * l) * (l - a - b) * (l + a - b) *
           (a * a + b * b - 2. * b * l + 5. * l * l),
         fy / (24. * E * I * l) * (l - a - b) * (l + a - b) * (-6. * l),
         fy / (24. * E * I * l) * (l - a - b) * (l + a - b) * (2.),
         0.,
         0.,
         0.,
         debut_barre),
       FALSE)
  BUG (common_fonction_ajout_poly (
         f_deform,
         0.,
         a,
         0.,
         -mz / (6. * E * I * l) * (l - a - b) *
           (-a * a + a * b + 2. * a * l - b * b - b * l),
         0.,
         mz / (6. * E * I * l) * (l - a - b),
         0.,
         0.,
         0.,
         debut_barre),
       FALSE)
  BUG (common_fonction_ajout_poly (
         f_deform,
         a,
         l - b,
         -mz / (6. * E * I * l) * (-a * a * a * l),
         -mz / (6. * E * I * l) *
           (a * a * a + 2. * a * l * l + b * b * b - b * l * l),
         -mz / (6. * E * I * l) * (-3. * a * l),
         -mz / (6. * E * I * l) * (a + b),
         0.,
         0.,
         0.,
         debut_barre),
       FALSE)
  BUG (common_fonction_ajout_poly (
         f_deform,
         l - b,
         l,
         -mz / (6. * E * I * l) * (l - a - b) * (a * a * l - a * b * l +
           a * l * l + b * b * l - 2. * b * l * l + l * l * l),
         -mz / (6. * E * I * l) * (l - a - b) *
           (-a * a + a * b - a * l - b * b + 2. * b * l - 3. * l * l),
         -mz / (6. * E * I * l) * (l - a - b) * (3. * l),
         mz / (6. * E * I * l) * (l - a - b),
         0.,
         0.,
         0.,
         debut_barre),
       FALSE)
  BUG (common_fonction_ajout_poly (
         f_deform,
         0.,
         l,
         0.,
         -l / (6. * E * I) * (2. * maz - mbz),
         maz / (2. * E * I),
         -(mbz + maz) / (6. * E * I * l),
         0.,
         0.,
         0.,
         debut_barre),
       FALSE)
  
  return TRUE;
}


gboolean
EF_charge_barre_repartie_uniforme_n (Fonction    *fonction,
                                     EF_Barre    *barre,
                                     unsigned int discretisation,
                                     double       a,
                                     double       b,
                                     double       fax,
                                     double       fbx)
/**
 * \brief Calcule les déplacements d'une barre selon l'axe x en fonction de
 *        l'effort normal ponctuel n et des réactions d'appuis fax et fbx.
 * \param fonction : fonction où sera ajoutée la déformée,
 * \param barre : Barre à étudier,
 * \param discretisation : partie de la barre à étudier,
 * \param a : position du début de la charge par rapport au début de la barre,
 * \param b : position de la fin de la charge par rapport à la fin de la barre,
 * \param fax : effort normal au début de la barre,
 * \param fbx : effort normal à la fin de la barre.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - barre == NULL,
 *     - fonction == NULL,
 *     - discretisation>barre->discretisation_element.
 *     - a < 0 ou a > l,
 *     - b < 0 ou b > l,
 *     - a > l-b.
 */
{
  EF_Noeud *debut, *fin;
  double    l, debut_barre;
  double    E, S;
  
  BUGMSG (fonction, FALSE, gettext ("Paramètre %s incorrect.\n"), "fonction")
  BUGMSG (barre, FALSE, gettext ("Paramètre %s incorrect.\n"), "barre")
  BUGMSG (discretisation <= barre->discretisation_element,
          FALSE,
          gettext ("La discrétisation %d souhaitée est hors domaine %d.\n"), discretisation, barre->discretisation_element)
  
  // La déformation selon l'axe x est par la formule :
  // \end{verbatim}\begin{center}
  // \includegraphics[width=8cm]{images/charge_barre_rep_uni_n2.pdf}
  // \includegraphics[width=8cm]{images/charge_barre_ponctuelle_n2.pdf}
  // \end{center}\begin{align*}
  // f_x(x) = & \int_0^x \frac{N_{11} \cdot N_{01}}{E \cdot S} dX + \int_x^a
  //          \frac{N_{11} \cdot N_{02}}{E \cdot S} dX + \int_a^{L-b}
  //          \frac{N_{12} \cdot N_{02}}{E \cdot S} dX + \int_{L-b}^L
  //          \frac{N_{13} \cdot N_{02}}{E \cdot S} dX
  //          \textrm{ pour x variant de 0 à a} \nonumber\\
  // f_x(x) = & \int_0^a \frac{N_{11} \cdot N_{01}}{E \cdot S} dX +
  //          \int_a^x \frac{N_{12} \cdot N_{01}}{E \cdot S} dX + \int_x^{L-b}
  //          \frac{N_{12} \cdot N_{02}}{E \cdot S} dX + \int_{L-b}^L
  //          \frac{N_{13} \cdot N_{02}}{E \cdot S} dX
  //          \textrm{ pour x variant a de L-b} \nonumber\\
  // f_x(x) = & \int_0^a \frac{N_{11} \cdot N_{01}}{E \cdot S} dX +
  //          \int_a^{L-b} \frac{N_{12} \cdot N_{01}}{E \cdot S} dX +
  //          \int_{L-b}^x \frac{N_{13} \cdot N_{01}}{E \cdot S} dX +
  //          \int_x^L \frac{N_{13} \cdot N_{02}}{E \cdot S} dX
  //          \textrm{ pour x variant L-b de L} \end{align*}\begin{align*}
  // N_{01} = &-\frac{L-x}{L} &
  // N_{02} = & \frac{x}{L} \nonumber\\
  // N_{11} = & -F_{Ax} &
  // N_{12} = & \frac{(F_{Ax} + F_{Bx}) \cdot X - a \cdot F_{Bx} -
  //          F_{Ax} \cdot (L-b)}{L-a-b} \nonumber\\
  // N_{13} = & F_{Bx} & & \nonumber\\
  // F_{A1} = & -\frac{L-x}{L} &
  // F_{A2} = & \frac{x}{L} \end{align*}\begin{verbatim}
  
  if (discretisation == 0)
    debut = barre->noeud_debut;
  else
    debut = g_list_nth_data (barre->nds_inter, discretisation-1);
  if (discretisation == barre->discretisation_element)
    fin = barre->noeud_fin;
  else
    fin = g_list_nth_data (barre->nds_inter, discretisation);
  
  debut_barre = EF_noeuds_distance (debut, barre->noeud_debut);
  BUG (!isnan (debut_barre), FALSE)
  l = EF_noeuds_distance (debut, fin);
  BUG (!isnan (l), FALSE)
  BUGMSG (!((a < 0.) && (!(ERR (a, 0.)))),
          FALSE,
          gettext ("Le début de la position de la charge répartie uniformément (%f) est incorrect.\n"), a)
  BUGMSG (!((b < 0.) && (!(ERR (b, 0.)))),
          FALSE,
          gettext ("La fin de la position de la charge répartie uniformément (%f) est incorrecte.\n"), b)
  BUGMSG (!((a > l) && (!(ERR (a, l)))),
          FALSE,
          gettext ("Le début de la charge répartie uniformément (%f) est incorrect. La longueur de la barre %d est de %f m.\n"), a, barre->numero, l)
  BUGMSG (!((b > l) && (!(ERR (b, l)))),
          FALSE,
          gettext ("La fin de la charge répartie uniformément (%f) est incorrecte. La longueur de la barre %d est de %f m.\n"), b, barre->numero, l)
  BUGMSG (!((a + b > l) && (!(ERR (a + b, l)))),
          FALSE,
          gettext ("Le début (%f) et la fin (%f) de la charge répartie uniformément sont incorrecte avec la longueur de la barre %d qui est de %f m.\n"), a, b, barre->numero, l)
  
  E = m_g (EF_materiaux_E (barre->materiau));
  S = m_g (EF_sections_s (barre->section));
  BUG (!isnan (E), FALSE)
  BUG (!isnan (S), FALSE)
  
  // Pour une section constante, les déformations valent :
  // \end{verbatim}\begin{align*}
  // f_x(x) = &\frac{F_{Ax} \cdot (L-a+b) \cdot (F_{Ax} + F_{Bx}) x}
  //          {2 \cdot L \cdot E \cdot S} & & \textrm{ pour x variant de 0 à a}
  //          \nonumber\\
  // f_x(x) = &\frac{(fbx+fax) \cdot (L \cdot x^2-L^2 \cdot x+b^2 \cdot x-a^2
  //          \cdot x+a^2 \cdot L)}{2 \cdot L \cdot (L-b-a) \cdot E \cdot S})
  //          & & \textrm{ pour x variant de a à L-b}\nonumber\\
  // f_x(x) = &\frac{(fbx+fax) \cdot (L-b+a) \cdot (L-x)}{2 \cdot L \cdot E
  //          \cdot S} & & \textrm{ pour x variant de L-b à L}
  //          \end{align*}\begin{verbatim}
  
  BUG (common_fonction_ajout_poly (
         fonction,
         0.,
         a,
         0.,
         (l - a + b) * (fax + fbx) / (2. * l * E * S),
         0.,
         0.,
         0.,
         0.,
         0.,
         debut_barre),
       FALSE)
  BUG (common_fonction_ajout_poly (
         fonction,
         a,
         l - b,
         (fax + fbx) / (2. * l * E * S) / (a + b - l) * (a * a *l),
         (fax + fbx) / (2. * l * E * S) / (a + b - l) *
           (-a * a + b * b - l * l),
         (fax + fbx) / (2. * l * E * S) / (a + b - l) * (l),
         0.,
         0.,
         0.,
         0.,
         debut_barre),
       FALSE)
  BUG (common_fonction_ajout_poly (
         fonction,
         l - b,
         l,
         (fax + fbx) / (2. * l * E * S) * l * (a - b + l),
         -(fax + fbx) / (2. * l * E * S) * (a - b + l),
         0.,
         0.,
         0.,
         0.,
         0.,
         debut_barre),
       FALSE)
  
  return TRUE;
}


gboolean
EF_charge_barre_repartie_uniforme_enleve_barres (Charge *charge,
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
  GList                          *list_parcours = barres;
  Charge_Barre_Repartie_Uniforme *charge_d;
  
  BUGMSG (charge, FALSE, gettext ("Paramètre %s incorrect.\n"), "charge")
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
EF_charge_barre_repartie_uniforme_free (Charge *charge)
/**
 * \brief Libère le contenu alloué dans une charge répartie uniforme sur barre.
 * \param charge : la charge à libérer.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - charge == NULL.
 */
{
  Charge_Barre_Repartie_Uniforme *charge_d;
  
  BUGMSG (charge, FALSE, gettext ("Paramètre %s incorrect.\n"), "charge")
  charge_d = charge->data;
  
  free (charge->nom);
  g_list_free (charge_d->barres);
  free (charge->data);
  free (charge);
  
  return TRUE;
}
