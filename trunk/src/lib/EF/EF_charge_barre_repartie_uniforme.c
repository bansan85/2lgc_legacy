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
#include <stdio.h>
#include <stdlib.h>
#include <cholmod.h>
#include <string.h>

#include "1990_actions.h"
#include "1992_1_1_section.h"
#include "common_projet.h"
#include "common_erreurs.h"
#include "common_maths.h"
#include "common_fonction.h"
#include "EF_noeud.h"

Charge_Barre_Repartie_Uniforme *EF_charge_barre_repartie_uniforme_ajout(Projet *projet,
  int num_action, LIST *barres, int repere_local, int projection, double a, double b,
  double fx, double fy, double fz, double mx, double my, double mz, const char* nom)
/* Description : Ajoute une charge répartie uniforme à une action et le long d'une barre en
 *               lui attribuant le numéro suivant la dernière charge de l'action.
 * Paramètres : Projet *projet : la variable projet,
 *            : int num_action : numero de l'action qui contiendra la charge,
 *            : void *barre : barre qui supportera la charge,
 *            : int repere_local : TRUE si les charges doivent être prise dans le repère local,
 *                                 FALSE pour le repère global,
 *            : int projection : TRUE si la charge doit être projetée sur la barre,
 *                               FALSE si aucune projection,
 *                               projection = TRUE est incompatible avec repere_local = TRUE,
 *            : double a : position en mètre de la charge par rapport au début de la barre,
 *            : double b : position en mètre de la charge par rapport à la fin de la barre,
 *            : double fx : force suivant l'axe x,
 *            : double fy : force suivant l'axe y,
 *            : double fz : force suivant l'axe z,
 *            : double mx : moment autour de l'axe x,
 *            : double my : moment autour de l'axe y,
 *            : double mz : moment autour de l'axe z.
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : NULL en cas de paramètres invalides :
 *             (projet == NULL) ou
 *             (projet->actions == NULL) ou
 *             (list_size(projet->actions) == 0) ou
 *             (barre == NULL) ou
 *             (projection == TRUE) && (repere_local == TRUE) ou
 *             (_1990_action_cherche_numero(projet, num_action) != 0) ou
 *             (a < 0) ou (a > l) ou
 *             (b < 0) ou (b > l) ou
 *             (a > l-b)
 *           NULL en cas d'erreur d'allocation mémoire
 */
{
    Action          *action_en_cours;
    Charge_Barre_Repartie_Uniforme *charge_dernier, charge_nouveau;
    
    // Trivial
    BUGMSG(projet, NULL, "EF_charge_barre_repartie_uniforme_ajout\n");
    BUGMSG(projet->actions, NULL, "EF_charge_barre_repartie_uniforme_ajout\n");
    BUGMSG(list_size(projet->actions), NULL, "EF_charge_barre_repartie_uniforme_ajout\n");
    BUGMSG(barres, NULL, "EF_charge_barre_repartie_uniforme_ajout\n");
    BUGMSG((projection == FALSE) || (repere_local == FALSE), NULL, "EF_charge_barre_repartie_uniforme_ajout\n");
    BUGMSG(!((a < 0.) && (!(ERREUR_RELATIVE_EGALE(a, 0.)))), NULL, "%s : a = %.20f\n", "EF_charge_barre_repartie_uniforme_ajout", a);
    BUGMSG(!((b < 0.) && (!(ERREUR_RELATIVE_EGALE(b, 0.)))), NULL, "%s : b = %.20f\n", "EF_charge_barre_repartie_uniforme_ajout", b);
    if (list_size(barres) != 0)
    {
        list_mvfront(barres);
        do
        {
            Beton_Barre **barre_p = list_curr(barres);
            Beton_Barre *barre = *barre_p;
            double l = EF_noeuds_distance(barre->noeud_debut, barre->noeud_fin);
            BUGMSG(!((a > l) && (!(ERREUR_RELATIVE_EGALE(a, l)))), NULL, "%s : a = %.20f\n", "EF_charge_barre_repartie_uniforme_ajout", a);
            BUGMSG(!((b > l) && (!(ERREUR_RELATIVE_EGALE(b, l)))), NULL, "%s : b = %.20f\n", "EF_charge_barre_repartie_uniforme_ajout", b);
            BUGMSG(!((a > l-b) && (!(ERREUR_RELATIVE_EGALE(a, l-b)))), NULL, "%s : a = %.20f, l = %.20f, b = %.20f\n", "EF_charge_barre_repartie_uniforme_ajout", a, l, b);
        }
        while (list_mvnext(barres) != NULL);
    }
    
    BUG(_1990_action_cherche_numero(projet, num_action) == 0, NULL);
    action_en_cours = (Action*)list_curr(projet->actions);
    
    charge_nouveau.type = CHARGE_BARRE_REPARTIE_UNIFORME;
    charge_nouveau.description = (char*)malloc(sizeof(char)*(strlen(nom)+1));
    BUGMSG(charge_nouveau.description, NULL, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_charge_barre_repartie_uniforme_ajout");
    strcpy(charge_nouveau.description, nom);
    charge_nouveau.barres = barres;
    charge_nouveau.repere_local = repere_local;
    charge_nouveau.projection = projection;
    charge_nouveau.a = a;
    charge_nouveau.b = b;
    charge_nouveau.fx = fx;
    charge_nouveau.fy = fy;
    charge_nouveau.fz = fz;
    charge_nouveau.mx = mx;
    charge_nouveau.my = my;
    charge_nouveau.mz = mz;
    
    charge_dernier = (Charge_Barre_Repartie_Uniforme *)list_rear(action_en_cours->charges);
    if (charge_dernier == NULL)
        charge_nouveau.numero = 0;
    else
        charge_nouveau.numero = charge_dernier->numero+1;
    
    list_mvrear(action_en_cours->charges);
    BUGMSG(list_insert_after(action_en_cours->charges, &(charge_nouveau), sizeof(charge_nouveau)), NULL, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_charge_barre_repartie_uniforme_ajout");
    
    return list_curr(action_en_cours->charges);
}


int EF_charge_barre_repartie_uniforme_mx(Beton_Barre *barre, unsigned int discretisation,
  double a, double b, Barre_Info_EF *infos, double mx, double *ma, double *mb)
/* Description : Calcule l'opposé aux moments d'encastrements pour l'élément spécifié soumis
 *               au moment de torsion uniformément répartie mx dans le repère local. Les
 *               résultats sont renvoyés par l'intermédiaire des pointeurs ma et mb qui ne
 *               peuvent être NULL.
 * Paramètres : Beton_Barre *barre : Barre à étudier,
 *            : unsigned int discretisation : partie de la barre à étudier,
 *            : double a : début et fin de la charge uniformément répartie respectivement
 *            : double b : par rapport au début et la fin de la partie de barre étudiée,
 *            : Barre_Info_EF *infos : infos de la partie de barre concerné,
 *            : double mx : moment autour de l'axe x,
 *            : double *ma : pointeur qui contiendra le moment au début de la barre,
 *            : double *mb : pointeur qui contiendra le moment à la fin de la barre.
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (barre == NULL) ou
 *             (infos == NULL) ou
 *             (barre->section == NULL) ou
 *             (barre->materiau == 0) ou
 *             (barre->noeud_debut == NULL) ou
 *             (barre->noeud_fin == NULL) ou
 *             (discretisation>barre->discretisation_element) ou
 *             (ma == NULL) ou
 *             (mb == NULL) ou
 *             (kAx == kBx == MAXDOUBLE) ou
 *             (a < 0) ou (a > l) ou
 *             (b < 0) ou (b > l) ou
 *             (a > l-b)
 */
{
    EF_Noeud    *debut, *fin;
    double      l, G;
    
    BUGMSG(barre, -1, "EF_charge_barre_repartie_uniforme_mx\n");
    BUGMSG(infos, -1, "EF_charge_barre_repartie_uniforme_mx\n");
    BUGMSG(barre->section, -1, "EF_charge_barre_repartie_uniforme_mx\n");
    BUGMSG(barre->materiau, -1, "EF_charge_barre_repartie_uniforme_mx\n");
    BUGMSG(barre->noeud_debut, -1, "EF_charge_barre_repartie_uniforme_mx\n");
    BUGMSG(barre->noeud_fin, -1, "EF_charge_barre_repartie_uniforme_mx\n");
    BUGMSG(discretisation<=barre->discretisation_element, -1, "EF_charge_barre_repartie_uniforme_mx\n");
    BUGMSG(ma, -1, "EF_charge_barre_repartie_uniforme_mx\n");
    BUGMSG(mb, -1, "EF_charge_barre_repartie_uniforme_mx\n");
    BUGMSG(!((ERREUR_RELATIVE_EGALE(infos->kAx, MAXDOUBLE)) && (ERREUR_RELATIVE_EGALE(infos->kBx, MAXDOUBLE))), -1, "EF_charge_barre_repartie_uniforme_mx\n");
    
    // Les moments aux extrémités de la barre sont déterminés par les intégrales de Mohr
    //   et valent dans le cas général :\end{verbatim}\begin{center}
    //   \includegraphics[width=8cm]{images/charge_barre_rep_uni_mx.pdf}\end{center}\begin{align*}
    // M_{Bx} = & \frac{\int_0^a \frac{m_x \cdot (L-a-b)}{J(x) \cdot G } dx + \int_a^{L-b} \frac{m_x \cdot (L-b-x)}{J(x) \cdot G } dx + k_A \cdot m_x \cdot (L-a-b)}{\int_0^l \frac{1}{J(x) \cdot G}dx + k_A + k_B}\nonumber\\
    // M_{Ax} = & m_x \cdot (L-a-b) - M_{Bx}\end{align*}\begin{verbatim}
    
    if (discretisation == 0)
        debut = barre->noeud_debut;
    else
        debut = barre->noeuds_intermediaires[discretisation-1];
    if (discretisation == barre->discretisation_element)
        fin = barre->noeud_fin;
    else
        fin = barre->noeuds_intermediaires[discretisation];
    
    l = EF_noeuds_distance(debut, fin);
    BUGMSG(!((a < 0.) && (!(ERREUR_RELATIVE_EGALE(a, 0.)))), -1, "%s : a = %.20f, l = %.20f\n", "EF_charge_barre_repartie_uniforme_mx", a, l);
    BUGMSG(!((a > l) && (!(ERREUR_RELATIVE_EGALE(a, l)))), -1, "%s : a = %.20f, l=%.20f\n", "EF_charge_barre_repartie_uniforme_mx", a, l);
    BUGMSG(!((b < 0.) && (!(ERREUR_RELATIVE_EGALE(b, 0.)))), -1, "%s : b = %.20f, l=%.20f\n", "EF_charge_barre_repartie_uniforme_mx", b, l);
    BUGMSG(!((b > l) && (!(ERREUR_RELATIVE_EGALE(b, l)))), -1, "%s : b = %.20f, l=%.20f\n", "EF_charge_barre_repartie_uniforme_mx", b, l);
    BUGMSG(!((a > l-b) && (!(ERREUR_RELATIVE_EGALE(a, l-b)))), -1, "%s : a = %.20f, l = %.20f, b = %.20f\n", "EF_charge_barre_repartie_uniforme_mx", a, l, b);
    
    G = barre->materiau->gnu_0_2;
    
    switch(((Beton_Section_Rectangulaire*)(barre->section))->type)
    {
        case BETON_SECTION_RECTANGULAIRE :
        case BETON_SECTION_T :
        case BETON_SECTION_CARRE :
        case BETON_SECTION_CIRCULAIRE :
        {
            double      J = _1992_1_1_sections_j(barre->section);
            
    // Pour une section section constante, les moments valent :\end{verbatim}\begin{displaymath}
    // M_{Bx} = \frac{(L-a-b) \cdot m_x \cdot (a-b+2 \cdot G \cdot J \cdot k_A+l)}{2 \cdot (G \cdot J \cdot (k_A+k_B)+l)}\end{displaymath}\begin{verbatim}
            if (ERREUR_RELATIVE_EGALE(infos->kAx, MAXDOUBLE))
                *mb = mx*(l-a-b);
            else if (ERREUR_RELATIVE_EGALE(infos->kBx, MAXDOUBLE))
                *mb = 0.;
            else
                *mb = (l-a-b)*mx*(a-b+2.*G*J*infos->kAx+l)/(2.*(G*J*(infos->kAx+infos->kBx)+l));
            *ma = mx*(l-a-b) - *mb;
            return 0;
            break;
        }
        default :
        {
            BUGMSG(0, 0., "EF_charge_barre_repartie_uniforme_mx\n");
            break;
        }
    }
}


int EF_charge_barre_repartie_uniforme_def_ang_iso_y(Beton_Barre *barre,
  unsigned int discretisation, double a, double b, double fz, double my,
  double *phia, double *phib)
/* Description : Calcule les angles de rotation pour un élément bi-articulé soumis à une
 *                 charge uniformément répartie fz, my dans le repère local. Les résultats
 *                 sont renvoyés par l'intermédiaire des pointeurs phia et phib qui ne
 *                 peuvent être NULL.
 * Paramètres : Beton_Barre *barre : Barre à étudier,
 *            : unsigned int discretisation : partie de la barre à étudier,
 *            : double a : début et fin de la charge uniformément répartie respectivement
 *            : double b : par rapport au début et la fin de la partie de barre étudiée,
 *            : double fz : charge linéaire suivant l'axe z
 *            : double my : moment linéaire autour de l'axe y
 *            : double *phia : pointeur qui contiendra l'angle au début de la barre
 *            : double *phib : pointeur qui contiendra l'angle à la fin de la barre
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (barre == NULL) ou
 *             (barre->section == NULL) ou
 *             (barre->materiau == 0) ou
 *             (barre->noeud_debut == NULL) ou
 *             (barre->noeud_fin == NULL) ou
 *             (discretisation>barre->discretisation_element) ou
 *             (phia == NULL) ou
 *             (phib == NULL) ou
 *             (a < 0) ou (a > l) ou
 *             (b < 0) ou (b > l) ou
 *             (a > l-b)
 */
{
    EF_Noeud    *debut, *fin;
    double      l;
    double      E;
    
    BUGMSG(barre, -1, "EF_charge_barre_repartie_uniforme_def_ang_iso_y\n");
    BUGMSG(barre->section, -1, "EF_charge_barre_repartie_uniforme_def_ang_iso_y\n");
    BUGMSG(barre->materiau, -1, "EF_charge_barre_repartie_uniforme_def_ang_iso_y\n");
    BUGMSG(barre->noeud_debut, -1, "EF_charge_barre_repartie_uniforme_def_ang_iso_y\n");
    BUGMSG(barre->noeud_fin, -1, "EF_charge_barre_repartie_uniforme_def_ang_iso_y\n");
    BUGMSG(discretisation<=barre->discretisation_element, -1, "EF_charge_barre_repartie_uniforme_def_ang_iso_y\n");
    BUGMSG(phia, -1, "EF_charge_barre_repartie_uniforme_def_ang_iso_y\n");
    BUGMSG(phib, -1, "EF_charge_barre_repartie_uniforme_def_ang_iso_y\n");
    
    // Les angles phi_A et phi_B sont déterminés par les intégrales de Mohr
    //   et valent dans le cas général :\end{verbatim}\begin{center}
    // \includegraphics[width=8cm]{images/charge_barre_rep_uni_fz.pdf}\includegraphics[width=8cm]{images/charge_barre_rep_uni_my.pdf}\par
    // \includegraphics[width=8cm]{images/charge_barre_ponctuelle_phiay.pdf}\includegraphics[width=8cm]{images/charge_barre_ponctuelle_phiby.pdf}\end{center}\begin{align*}
    // \varphi_A = & \int_0^a \frac{Mf_{11} \cdot Mf_0}{E \cdot I_y} dx + \int_a^{L-b} \frac{Mf_{12} \cdot Mf_0}{E \cdot I_y} dx + \int_{L-b}^L \frac{Mf_{13} \cdot Mf_0}{E \cdot I_y} dx \nonumber\\
    //             & + \int_0^a \frac{Mf_{21} \cdot Mf_0 }{E \cdot I_y} dx + \int_a^{L-b} \frac{Mf_{22} \cdot Mf_0}{E \cdot I_y} dx+ \int_{L-b}^L \frac{Mf_{23} \cdot Mf_0}{E \cdot I_y} dx \nonumber\\
    // \varphi_B = & \int_0^a \frac{Mf_{11} \cdot Mf_3}{E \cdot I_y} dx + \int_a^{L-b} \frac{Mf_{12} \cdot Mf_3}{E \cdot I_y} dx + \int_{L-b}^L \frac{Mf_{13} \cdot Mf_3}{E \cdot I_y} dx \nonumber\\
    //             & + \int_0^a \frac{Mf_{21} \cdot Mf_3 }{E \cdot I_y} dx + \int_a^{L-b} \frac{Mf_{22} \cdot Mf_3}{E \cdot I_y} dx+ \int_{L-b}^L \frac{Mf_{23} \cdot Mf_3}{E \cdot I_y} dx \texttt{ avec :}\end{align*}\begin{align*}
    // Mf_0 = & \frac{L-x}{L} &                 Mf_{11} = & A_2 \cdot x\nonumber\\
    // Mf_{12} = & B_2 \cdot (L-x) + f_z \cdot \frac{(L-b-x)^2}{2} & Mf_{13} = & B_2 \cdot (L-x) \nonumber\\
    // Mf_{21} = & A_1 \cdot x &                Mf_{22} = & \frac{(-A_1 \cdot a + B_1 \cdot b) \cdot x - a \cdot (b \cdot (A_1 + B_1)-L \cdot A_1)}{L-a-b} \nonumber\\
    // Mf_{23} = & B_1 \cdot (L-x) &            Mf_3 = & -\frac{x}{L} \nonumber\\
    // A_1 = & -m_y \cdot \frac{L-a-b}{L}                  & B_1 = & m_y \cdot \frac{L-a-b}{L} \nonumber\\
    // A_2 = & -\frac{f_z \cdot (L-a-b)(L-a+b)}{2 \cdot L} & B_2 = & -\frac{f_z \cdot (L-a-b)(L+a-b)}{2 \cdot L}\end{align*}\begin{verbatim}
    if (discretisation == 0)
        debut = barre->noeud_debut;
    else
        debut = barre->noeuds_intermediaires[discretisation-1];
    if (discretisation == barre->discretisation_element)
        fin = barre->noeud_fin;
    else
        fin = barre->noeuds_intermediaires[discretisation];
    
    l = EF_noeuds_distance(debut, fin);
    BUGMSG(!((a < 0.) && (!(ERREUR_RELATIVE_EGALE(a, 0.)))), -1, "%s : a = %.20f, l = %.20f\n", "EF_charge_barre_repartie_uniforme_def_ang_iso_y", a, l);
    BUGMSG(!((a > l) && (!(ERREUR_RELATIVE_EGALE(a, l)))), -1, "%s : a = %.20f, l=%.20f\n", "EF_charge_barre_repartie_uniforme_def_ang_iso_y", a, l);
    BUGMSG(!((b < 0.) && (!(ERREUR_RELATIVE_EGALE(b, 0.)))), -1, "%s : b = %.20f, l=%.20f\n", "EF_charge_barre_repartie_uniforme_def_ang_iso_y", b, l);
    BUGMSG(!((b > l) && (!(ERREUR_RELATIVE_EGALE(b, l)))), -1, "%s : b = %.20f, l=%.20f\n", "EF_charge_barre_repartie_uniforme_def_ang_iso_y", b, l);
    BUGMSG(!((a > l-b) && (!(ERREUR_RELATIVE_EGALE(a, l-b)))), -1, "%s : a = %.20f, l = %.20f, b = %.20f\n", "EF_charge_barre_repartie_uniforme_def_ang_iso_y", a, l, b);
    
    E = barre->materiau->ecm;
    
    switch(((Beton_Section_Rectangulaire*)(barre->section))->type)
    {
        case BETON_SECTION_RECTANGULAIRE :
        case BETON_SECTION_T :
        case BETON_SECTION_CARRE :
        case BETON_SECTION_CIRCULAIRE :
        {
            double      I = _1992_1_1_sections_iy(barre->section);
    // Pour une section constante, les angles valent :\end{verbatim}\begin{displaymath}
    // \varphi_A = \frac{[a^2-a \cdot (b+2 \cdot L) + b \cdot (b+L)] \cdot m_y \cdot (L-a-b)}{6 \cdot E \cdot I_y \cdot L} - \frac{f_z \cdot (L-a-b) \cdot (L-a+b) \cdot (L^2-a^2-b^2+2 \cdot a \cdot L)}{24 \cdot E \cdot I_y \cdot L}\end{displaymath}\begin{displaymath}
    // \varphi_B = \frac{[a^2+a \cdot (L-b) - b \cdot (2L-b)] \cdot m_y \cdot (L-a-b)}{6 \cdot E \cdot I_y \cdot L} + \frac{F \cdot (L-a-b) \cdot (L+a-b) \cdot (L^2-a^2-b^2+2 \cdot b \cdot L)}{24 \cdot E \cdot I_y \cdot L}\end{displaymath}\begin{verbatim}
            *phia = (a*a-a*(b+2*l)+b*(b+l))*my*(l-a-b)/(6*E*I*l)-fz*(l-a-b)*(l-a+b)*(l*l-a*a-b*b+2*a*l)/(24*E*I*l);
            *phib = (a*a-b*(2*l-b)+a*(l-b))*my*(l-a-b)/(6*E*I*l)+fz*(l-a-b)*(l+a-b)*(l*l-a*a-b*b+2*b*l)/(24*E*I*l);
            return 0;
            break;
        }
        default :
        {
            BUGMSG(0, 0, "EF_charge_barre_repartie_uniforme_def_ang_iso_y\n");
            break;
        }
    }
}


int EF_charge_barre_repartie_uniforme_def_ang_iso_z(Beton_Barre *barre,
  unsigned int discretisation, double a, double b, double fy, double mz,
  double *phia, double *phib)
/* Description : Calcule les angles de rotation pour un élément bi-articulé soumis au
 *                 chargement fy et mz dans le repère local. Les résultats sont renvoyés par
 *                 l'intermédiaire des pointeurs phia et phib qui ne peuvent être NULL.
 * Paramètres : Beton_Barre *barre : Barre à étudier,
 *            : unsigned int discretisation : partie de la barre à étudier,
 *            : double debut : début et fin de la charge uniformément répartie par rapport au
 *            : double fin : début de la partie de barre à étudier,
 *            : double fy : charge linéaire suivant l'axe y
 *            : double mz : moment linéaire autour de l'axe z
 *            : double *phia : pointeur qui contiendra l'angle au début de la barre,
 *            : double *phib : pointeur qui contiendra l'angle à la fin de la barre.
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (barre == NULL) ou
 *             (barre->section == NULL) ou
 *             (barre->materiau == 0) ou
 *             (barre->noeud_debut == NULL) ou
 *             (barre->noeud_fin == NULL) ou
 *             (discretisation>barre->discretisation_element) ou
 *             (phia == NULL) ou
 *             (phib == NULL) ou
 *             (a < 0) ou (a > l) ou
 *             (b < 0) ou (b > l) ou
 *             (a > l-b)
 */
{
    EF_Noeud    *debut, *fin;
    double      l;
    double      E;
    
    BUGMSG(barre, -1, "EF_charge_barre_repartie_uniforme_def_ang_iso_z\n");
    BUGMSG(barre->section, -1, "EF_charge_barre_repartie_uniforme_def_ang_iso_z\n");
    BUGMSG(barre->materiau, -1, "EF_charge_barre_repartie_uniforme_def_ang_iso_z\n");
    BUGMSG(barre->noeud_debut, -1, "EF_charge_barre_repartie_uniforme_def_ang_iso_z\n");
    BUGMSG(barre->noeud_fin, -1, "EF_charge_barre_repartie_uniforme_def_ang_iso_z\n");
    BUGMSG(discretisation<=barre->discretisation_element, -1, "EF_charge_barre_repartie_uniforme_def_ang_iso_z\n");
    BUGMSG(phia, -1, "EF_charge_barre_repartie_uniforme_def_ang_iso_z\n");
    BUGMSG(phib, -1, "EF_charge_barre_repartie_uniforme_def_ang_iso_z\n");
    
    if (discretisation == 0)
        debut = barre->noeud_debut;
    else
        debut = barre->noeuds_intermediaires[discretisation-1];
    if (discretisation == barre->discretisation_element)
        fin = barre->noeud_fin;
    else
        fin = barre->noeuds_intermediaires[discretisation];
    
    // Les angles phi_A et phi_B sont déterminés par les intégrales de Mohr
    //   et valent dans le cas général :\end{verbatim}\begin{center}
    // \includegraphics[width=8cm]{images/charge_barre_rep_uni_fy.pdf}\includegraphics[width=8cm]{images/charge_barre_rep_uni_mz.pdf}\par
    // \includegraphics[width=8cm]{images/charge_barre_ponctuelle_phiaz.pdf}\includegraphics[width=8cm]{images/charge_barre_ponctuelle_phibz.pdf}\end{center}\begin{align*}
    // \varphi_A = & \int_0^a \frac{Mf_{11} \cdot Mf_0}{E \cdot I_z} dx + \int_a^{L-b} \frac{Mf_{12} \cdot Mf_0}{E \cdot I_z} dx + \int_{L-b}^L \frac{Mf_{13} \cdot Mf_0}{E \cdot I_z} dx \nonumber\\
    //             & + \int_0^a \frac{Mf_{21} \cdot Mf_0 }{E \cdot I_z} dx + \int_a^{L-b} \frac{Mf_{22} \cdot Mf_0}{E \cdot I_z} dx+ \int_{L-b}^L \frac{Mf_{23} \cdot Mf_0}{E \cdot I_z} dx \nonumber\\
    // \varphi_B = & \int_0^a \frac{Mf_{11} \cdot Mf_3}{E \cdot I_z} dx + \int_a^{L-b} \frac{Mf_{12} \cdot Mf_3}{E \cdot I_z} dx + \int_{L-b}^L \frac{Mf_{13} \cdot Mf_3}{E \cdot I_z} dx \nonumber\\
    //             & + \int_0^a \frac{Mf_{21} \cdot Mf_3 }{E \cdot I_z} dx + \int_a^{L-b} \frac{Mf_{22} \cdot Mf_3}{E \cdot I_z} dx+ \int_{L-b}^L \frac{Mf_{23} \cdot Mf_3}{E \cdot I_z} dx \texttt{ avec :}\end{align*}\begin{align*}
    // Mf_0 = & \frac{L-x}{L} &                 Mf_{11} = & A_2 \cdot x\nonumber\\
    // Mf_{12} = & B_2 \cdot (L-x) - f_z \cdot \frac{(L-b-x)^2}{2} & Mf_{13} = & B_2 \cdot (L-x) \nonumber\\
    // Mf_{21} = & A_1 \cdot x &                Mf_{22} = & \frac{(-A_1 \cdot a + B_1 \cdot b) \cdot x - a \cdot (b \cdot (A_1 + B_1)-L \cdot A_1)}{L-a-b} \nonumber\\
    // Mf_{23} = & B_1 \cdot (L-x) &            Mf_3 = & -\frac{x}{L} \nonumber\\
    // A_1 = & -m_z \cdot \frac{L-a-b}{L}                  & B_1 = & m_z \cdot \frac{L-a-b}{L} \nonumber\\
    // A_2 = & \frac{f_y \cdot (L-a-b)(L-a+b)}{2 \cdot L} & B_2 = & \frac{f_y \cdot (L-a-b)(L+a-b)}{2 \cdot L}\end{align*}\begin{verbatim}
    l = EF_noeuds_distance(debut, fin);
    BUGMSG(!((a < 0.) && (!(ERREUR_RELATIVE_EGALE(a, 0.)))), -1, "%s : a = %.20f, l = %.20f\n", "EF_charge_barre_repartie_uniforme_def_ang_iso_y", a, l);
    BUGMSG(!((a > l) && (!(ERREUR_RELATIVE_EGALE(a, l)))), -1, "%s : a = %.20f, l=%.20f\n", "EF_charge_barre_repartie_uniforme_def_ang_iso_y", a, l);
    BUGMSG(!((b < 0.) && (!(ERREUR_RELATIVE_EGALE(b, 0.)))), -1, "%s : b = %.20f, l=%.20f\n", "EF_charge_barre_repartie_uniforme_def_ang_iso_y", b, l);
    BUGMSG(!((b > l) && (!(ERREUR_RELATIVE_EGALE(b, l)))), -1, "%s : b = %.20f, l=%.20f\n", "EF_charge_barre_repartie_uniforme_def_ang_iso_y", b, l);
    BUGMSG(!((a > l-b) && (!(ERREUR_RELATIVE_EGALE(a, l-b)))), -1, "%s : a = %.20f, l = %.20f, b = %.20f\n", "EF_charge_barre_repartie_uniforme_def_ang_iso_y", a, l, b);
    
    E = barre->materiau->ecm;
    
    switch(((Beton_Section_Rectangulaire*)(barre->section))->type)
    {
        case BETON_SECTION_RECTANGULAIRE :
        case BETON_SECTION_T :
        case BETON_SECTION_CARRE :
        case BETON_SECTION_CIRCULAIRE :
        {
            double      I = _1992_1_1_sections_iz(barre->section);
    // Pour une section constante, les angles valent :\end{verbatim}\begin{displaymath}
    // \varphi_A = \frac{[a^2-a \cdot (b+2 \cdot L) + b \cdot (b+L)] \cdot m_z \cdot (L-a-b)}{6 \cdot E \cdot I_z \cdot L} + \frac{f_y \cdot (L-a-b) \cdot (L-a+b) \cdot (L^2-a^2-b^2+2 \cdot a \cdot L)}{24 \cdot E \cdot I_z \cdot L}\end{displaymath}\begin{displaymath}
    // \varphi_B = \frac{[a^2+a \cdot (L-b) - b \cdot (2L-b)] \cdot m_z \cdot (L-a-b)}{6 \cdot E \cdot I_z \cdot L} - \frac{f_y \cdot (L-a-b) \cdot (L+a-b) \cdot (L^2-a^2-b^2+2 \cdot b \cdot L)}{24 \cdot E \cdot I_z \cdot L}\end{displaymath}\begin{verbatim}
            *phia = (a*a-a*(b+2*l)+b*(b+l))*mz*(l-a-b)/(6*E*I*l)+fy*(l-a-b)*(l-a+b)*(l*l-a*a-b*b+2*a*l)/(24*E*I*l);
            *phib = (a*a-b*(2*l-b)+a*(l-b))*mz*(l-a-b)/(6*E*I*l)-fy*(l-a-b)*(l+a-b)*(l*l-a*a-b*b+2*b*l)/(24*E*I*l);
            return 0;
            break;
        }
        default :
        {
            BUGMSG(0, 0, "EF_charge_barre_repartie_uniforme_def_ang_iso_z\n");
            break;
        }
    }
}


double EF_charge_barre_repartie_uniforme_position_resultante_x(void *section, double a,
  double b, double l)
/* Description : Renvoie la position de la résultante pour une charge uniformément répartie
 *               vers l'axe x.
 * Paramètres : section : nature de la section
 *              double a : position du début de la charge par rapport au début de la barre,
 *              double b : position de la fin de la charge par rapport à la fin de la barre,
 *              double l : longueur de la barre.
 * Valeur renvoyée :
 *   Succès : Résultat
 *   Échec : 0. en cas de paramètres invalides :
 *             (section == NULL) ou
 *             (a < 0) ou (a > l) ou
 *             (b < 0) ou (b > l) ou
 *             (a > l-b)
 */
{
    
    BUGMSG(section, 0., "EF_charge_barre_repartie_uniforme_position_resultante_x\n");
    BUGMSG(!((a < 0.) && (!(ERREUR_RELATIVE_EGALE(a, 0.)))), -1, "%s : a = %.20f, l = %.20f\n", "EF_charge_barre_repartie_uniforme_position_resultante_x", a, l);
    BUGMSG(!((a > l) && (!(ERREUR_RELATIVE_EGALE(a, l)))), -1, "%s : a = %.20f, l=%.20f\n", "EF_charge_barre_repartie_uniforme_position_resultante_x", a, l);
    BUGMSG(!((b < 0.) && (!(ERREUR_RELATIVE_EGALE(b, 0.)))), -1, "%s : b = %.20f, l=%.20f\n", "EF_charge_barre_repartie_uniforme_position_resultante_x", b, l);
    BUGMSG(!((b > l) && (!(ERREUR_RELATIVE_EGALE(b, l)))), -1, "%s : b = %.20f, l=%.20f\n", "EF_charge_barre_repartie_uniforme_position_resultante_x", b, l);
    BUGMSG(!((a > l-b) && (!(ERREUR_RELATIVE_EGALE(a, l-b)))), -1, "%s : a = %.20f, l = %.20f, b = %.20f\n", "EF_charge_barre_repartie_uniforme_position_resultante_x", a, l, b);
    
    // La position de la résultante de la charge est obtenue en résolvant X dans la formule :\end{verbatim}\begin{center}
    // \includegraphics[width=8cm]{images/charge_barre_rep_uni_n.pdf}\end{center}\begin{displaymath}
    // \int_a^{L-b} \frac{x-a}{S(x)} dx + \int_{L-b}^X \frac{L-b-a}{S(x)} dx = 0\end{displaymath}\begin{verbatim}
    
    switch(((Beton_Section_Rectangulaire*)section)->type)
    {
        case BETON_SECTION_RECTANGULAIRE :
        case BETON_SECTION_T :
        case BETON_SECTION_CARRE :
        case BETON_SECTION_CIRCULAIRE :
        {
            
    // Pour une section rectantulaire de section constante, X vaut :\end{verbatim}\begin{displaymath}
    // X = \frac{a+L-b}{2} \end{displaymath}\begin{verbatim}
            return (a+l-b)/2.;
            break;
        }
        default :
        {
            BUGMSG(0, 0., "EF_charge_barre_repartie_uniforme_position_resultante_x\n");
            break;
        }
    }
}

int EF_charge_barre_repartie_uniforme_fonc_rx(Fonction *fonction, Beton_Barre *barre,
  unsigned int discretisation, double a, double b, double max, double mbx)
/* Description : Calcule les déplacements d'une barre en rotation autour de l'axe x en fonction
 *                  des efforts aux extrémités de la poutre.
 * Paramètres : Fonction *fonction : fonction où sera ajoutée la déformée,
 *              Beton_Barre *barre : barre à étudier,
 *              unsigned int discretisation : partie de la barre à étudier,
 *              double a : position du début de la charge par rapport au début de la barre,
 *              double b : position de la fin de la charge par rapport à la fin de la barre,
 *              double max : moment au début de la barre,
 *              double mbx : moment à la fin de la barre.
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (fonction == NULL) ou
 *             (barre == NULL) ou
 *             (barre->section == NULL) ou
 *             (barre->materiau == 0) ou
 *             (barre->noeud_debut == NULL) ou
 *             (barre->noeud_fin == NULL) ou
 *             (discretisation>barre->discretisation_element) ou
 *             (a < 0) ou (a > l) ou
 *             (b < 0) ou (b > l) ou
 *             (a > l-b)
 *           -3 en cas d'erreur due à une fonction interne
 */
{
    EF_Noeud    *debut, *fin;
    Barre_Info_EF *infos;
    double      l;
    double      G, debut_barre;

    BUGMSG(fonction, -1, "EF_charge_barre_repartie_uniforme_fonc_rx\n");
    BUGMSG(barre, -1, "EF_charge_barre_repartie_uniforme_fonc_rx\n");
    BUGMSG(barre->section, -1, "EF_charge_barre_repartie_uniforme_fonc_rx\n");
    BUGMSG(barre->materiau, -1, "EF_charge_barre_repartie_uniforme_fonc_rx\n");
    BUGMSG(barre->noeud_debut, -1, "EF_charge_barre_repartie_uniforme_fonc_rx\n");
    BUGMSG(barre->noeud_fin, -1, "EF_charge_barre_repartie_uniforme_fonc_rx\n");
    BUGMSG(discretisation<=barre->discretisation_element, -1, "EF_charge_barre_repartie_uniforme_fonc_rx\n");
    infos = &(barre->info_EF[discretisation]);
    BUGMSG(!((ERREUR_RELATIVE_EGALE(infos->kAx, MAXDOUBLE)) && (ERREUR_RELATIVE_EGALE(infos->kBx, MAXDOUBLE))), -1, "EF_charge_barre_repartie_uniforme_fonc_rx\n");
    
    // La déformation d'une barre soumise à un effort de torsion est défini par les formules :\end{verbatim}\begin{center}
    //   \includegraphics[width=8cm]{images/charge_barre_rep_uni_mx2.pdf}\end{center}\begin{verbatim}
    // Si le noeud B est relaché en rotation Alors\end{verbatim}\begin{align*}
    // r_x(x) = & M_{Ax} \cdot \left( k_{Ax} + \int_0^x \frac{1}{J \cdot G} dx \right) & & \textrm{ pour x variant de 0 à a}\nonumber\\
    // r_x(x) = & k_{Ax} \cdot M_{Ax} + \int_0^a \frac{M_{Ax}}{J \cdot G} dx + \int_a^x \frac{Mf_1(X)}{J \cdot G} dX & & \textrm{ pour x variant de a à L-b} \nonumber\\
    // r_x(x) = & k_{Ax} \cdot M_{Ax} + \int_0^a \frac{M_{Ax}}{J \cdot G} dx + \int_a^{L-b} \frac{Mf_1(X)}{J \cdot G} dX - \int_{L-b}^x \frac{M_{Bx}}{J \cdot G} dX & & \textrm{ pour x variant de L-b à L} \end{align*}\begin{verbatim}
    // Sinon \end{verbatim}\begin{align*}
    // r_x(x) = & k_{Bx} \cdot M_{Bx} + \int_{L-b}^L \frac{M_{Bx}}{J \cdot G} dx - \int_a^{L-b} \frac{Mf_1(x)}{J \cdot G} dX - \int_x^a \frac{M_{Ax}}{J \cdot G} dx & & \textrm{ pour x variant de 0 à a} \nonumber\\
    // r_x(x) = & k_{Bx} \cdot M_{Bx} + \int_{L-b}^L \frac{M_{Bx}}{J \cdot G} dx - \int_x^{L-b} \frac{Mf_1(x)}{J \cdot G} dX & & \textrm{ pour x variant de a à L-b} \nonumber\\
    // r_x(x) = & M_{Bx} \cdot \left( k_{Bx} + \int_x^L \frac{1}{J \cdot G} dx \right) & & \textrm{ pour x variant de L-b à L}\end{align*}\begin{verbatim}
    // FinSi\end{verbatim}\begin{align*}
    // Mf_1(X) = & \frac{X \cdot (M_{Ax}+M_{Bx}) -a \cdot M_{Bx} - M_{Ax} \cdot (L-b)}{-L+b+a}\end{align*}\begin{verbatim}
    
    if (discretisation == 0)
        debut = barre->noeud_debut;
    else
        debut = barre->noeuds_intermediaires[discretisation-1];
    if (discretisation == barre->discretisation_element)
        fin = barre->noeud_fin;
    else
        fin = barre->noeuds_intermediaires[discretisation];
    
    debut_barre = EF_noeuds_distance(debut, barre->noeud_debut);
    l = EF_noeuds_distance(debut, fin);
    BUGMSG(!((a < 0.) && (!(ERREUR_RELATIVE_EGALE(a, 0.)))), -1, "%s : a = %.20f, l = %.20f\n", "EF_charge_barre_repartie_uniforme_fonc_rx", a, l);
    BUGMSG(!((a > l) && (!(ERREUR_RELATIVE_EGALE(a, l)))), -1, "%s : a = %.20f, l=%.20f\n", "EF_charge_barre_repartie_uniforme_fonc_rx", a, l);
    BUGMSG(!((b < 0.) && (!(ERREUR_RELATIVE_EGALE(b, 0.)))), -1, "%s : b = %.20f, l=%.20f\n", "EF_charge_barre_repartie_uniforme_fonc_rx", b, l);
    BUGMSG(!((b > l) && (!(ERREUR_RELATIVE_EGALE(b, l)))), -1, "%s : b = %.20f, l=%.20f\n", "EF_charge_barre_repartie_uniforme_fonc_rx", b, l);
    BUGMSG(!((a > l-b) && (!(ERREUR_RELATIVE_EGALE(a, l-b)))), -1, "%s : a = %.20f, l = %.20f, b = %.20f\n", "EF_charge_barre_repartie_uniforme_fonc_rx", a, l, b);
    
    G = barre->materiau->gnu_0_2;
    
    switch(((Beton_Section_Rectangulaire*)(barre->section))->type)
    {
        case BETON_SECTION_RECTANGULAIRE :
        case BETON_SECTION_T :
        case BETON_SECTION_CARRE :
        case BETON_SECTION_CIRCULAIRE :
        {
            double      J = _1992_1_1_sections_j(barre->section);
            
            if (ERREUR_RELATIVE_EGALE(infos->kBx, MAXDOUBLE))
            {
                BUG(common_fonction_ajout(fonction, 0., a, max*infos->kAx, max/(G*J), 0., 0., 0., 0., 0., debut_barre) == 0, -3);
                BUG(common_fonction_ajout(fonction, a, l-b, max*infos->kAx+(a*a*(max+mbx))/(2.*(a+b-l)*G*J), (-2*a*mbx+2*b*max-2*l*max)/(2.*(a+b-l)*G*J), (max+mbx)/(2.*(a+b-l)*G*J), 0., 0., 0., 0., debut_barre) == 0, -3);
                BUG(common_fonction_ajout(fonction, l-b, l, max*infos->kAx+(l+a-b)*(max+mbx)/(2.*G*J), (-2*mbx)/(2.*G*J), 0., 0., 0., 0., 0., debut_barre) == 0, -3);
            }
            else
            {
                BUG(common_fonction_ajout(fonction, 0., a, mbx*infos->kBx+((max+mbx)*(-a+b)-l*(max-mbx))/(2.*G*J), (2.*max)/(2.*G*J), 0., 0., 0., 0., 0., debut_barre) == 0, -3);
                BUG(common_fonction_ajout(fonction, a, l-b, mbx*infos->kBx+(2.*a*l*mbx+b*b*max+b*b*mbx-2.*b*l*max+l*l*max-l*l*mbx)/(2.*(a+b-l)*G*J), (-2.*a*mbx+2.*b*max-2.*l*max)/(2.*(a+b-l)*G*J), (max+mbx)/(2.*(a+b-l)*G*J), 0., 0., 0., 0., debut_barre) == 0, -3);
                BUG(common_fonction_ajout(fonction, l-b, l, mbx*(infos->kBx + l/(G*J)), -mbx/(G*J), 0., 0., 0., 0., 0., debut_barre) == 0, -3);
            }
            
    // Pour une section section constante, les moments valent :
    // Si le noeud B est relaché en rotation Alors\end{verbatim}\begin{align*}
    // r_x(x) = & M_{Ax} \cdot \left( k_{Ax} + \frac{x}{J \cdot G} \right) & & \textrm{ pour x variant de 0 à a}\nonumber\\
    // r_x(x) = & k_{Ax} \cdot M_{Ax} + \frac{(M_{Ax} + M_{Bx}) \cdot (a^2-2 \cdot x \cdot (L-b) + x^2)}{2 \cdot (a+b-L) \cdot G \cdot J} - \frac{2 \cdot M_{Bx} \cdot x}{2 \cdot G \cdot J} & & \textrm{ pour x variant de a à L-b} \nonumber\\
    // r_x(x) = & k_{Ax} \cdot M_{Ax} + \frac{(M_{Ax} + M_{Bx}) \cdot (a-b+L) - 2 \cdot M_{Bx} \cdot x}{2 \cdot G \cdot J} & & \textrm{ pour x variant de L-b à L} \end{align*}\begin{verbatim}
    // Sinon \end{verbatim}\begin{align*}
    // r_x(x) = & k_{Bx} \cdot M_{Bx} + \frac{(M_{Ax} + M_{Bx}) \cdot (-a+b) - (M_{Ax} - M_{Bx}) \cdot L - 2 \cdot M_{Ax} \cdot x}{2 \cdot G \cdot J} & & \textrm{ pour x variant de 0 à a} \nonumber\\
    // r_x(x) = & k_{Bx} \cdot M_{Bx} + \frac{(M_{Ax} + M_{Bx}) \cdot (L-b-x)^2}{2 \cdot (a+b-L) \cdot G \cdot J} + \frac{(L-x) \cdot M_{Bx}}{J \cdot G} & & \textrm{ pour x variant de a à L-b} \nonumber\\
    // r_x(x) = & M_{Bx} \cdot \left( k_{Bx} + \frac{L-x}{J \cdot G} \right) & & \textrm{ pour x variant de L-b à L}\end{align*}\begin{verbatim}
    // FinSi
            return 0;
            break;
        }
        default :
        {
            BUGMSG(0, 0., "EF_charge_barre_repartie_uniforme_fonc_rx\n");
            break;
        }
    }
}


int EF_charge_barre_repartie_uniforme_fonc_ry(Fonction *f_rotation, Fonction* f_deform,
  Beton_Barre *barre, unsigned int discretisation, double a, double b, double fz, double my,
  double may, double mby)
/* Description : Calcule les déplacements d'une barre en rotation autour de l'axe y et en
 *                 déformation selon l'axe z en fonction de la charge linéaire (fz et my) et
 *                 des efforts aux extrémités de la poutre.
 * Paramètres : Fonction *fonction : fonction où sera ajoutée la déformée,
 *              Beton_Barre *barre : Barre à étudier,
 *              unsigned int discretisation : partie de la barre à étudier,
 *              double a : position du début de la charge par rapport au début de la barre,
 *              double b : position de la fin de la charge par rapport à la fin de la barre,
 *              double fz : charge linéaire suivant l'axe z
 *              double my : moment linéaire  autour de l'axe y
 *              double may : moment au début de la barre,
 *              double mby : moment à la fin de la barre.
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (f_rotation == NULL) ou
 *             (f_deform == NULL) ou
 *             (barre == NULL) ou
 *             (barre->section == NULL) ou
 *             (barre->materiau == 0) ou
 *             (barre->noeud_debut == NULL) ou
 *             (barre->noeud_fin == NULL) ou
 *             (discretisation>barre->discretisation_element) ou
 *             (a < 0) ou (a > l) ou
 *             (b < 0) ou (b > l) ou
 *             (a > l-b)
 */
{
    EF_Noeud    *debut, *fin;
    double      l;
    double      E, debut_barre;
    
    BUGMSG(f_rotation, -1, "EF_charge_barre_repartie_uniforme_fonc_ry\n");
    BUGMSG(f_deform, -1, "EF_charge_barre_repartie_uniforme_fonc_ry\n");
    BUGMSG(barre, -1, "EF_charge_barre_repartie_uniforme_fonc_ry\n");
    BUGMSG(barre->section, -1, "EF_charge_barre_repartie_uniforme_fonc_ry\n");
    BUGMSG(barre->materiau, -1, "EF_charge_barre_repartie_uniforme_fonc_ry\n");
    BUGMSG(barre->noeud_debut, -1, "EF_charge_barre_repartie_uniforme_fonc_ry\n");
    BUGMSG(barre->noeud_fin, -1, "EF_charge_barre_repartie_uniforme_fonc_ry\n");
    BUGMSG(discretisation<=barre->discretisation_element, -1, "EF_charge_barre_repartie_uniforme_fonc_ry\n");
    
    // La déformation et la  rotation d'une barre soumise à un effort de flexion autour de
    // l'axe y est calculée selon le principe des intégrales de Mohr et est définie par les
    // formules :\end{verbatim}\begin{center}
    // \includegraphics[width=8cm]{images/charge_barre_rep_uni_fz.pdf}\includegraphics[width=8cm]{images/charge_barre_rep_uni_my.pdf}\par
    // \includegraphics[width=8cm]{images/charge_barre_ponctuelle_my_1.pdf}\includegraphics[width=8cm]{images/charge_barre_ponctuelle_fz_1.pdf}\par
    // \includegraphics[width=8cm]{images/charge_barre_ponctuelle_may_mby.pdf}\end{center}\begin{align*}
    // r_y(x) = & \int_0^x \frac{Mf_{11}(X) \cdot Mf_{31}(X)}{E \cdot I_y(X)} dX + \int_x^a \frac{Mf_{11}(X) \cdot Mf_{32}(X)}{E \cdot I_y(X)} dX + \int_a^{L-b} \frac{Mf_{12}(X) \cdot Mf_{32}(X)}{E \cdot I_y(X)} dX\nonumber\\
    //          & + \int_{L-b}^L \frac{Mf_{13}(X) \cdot Mf_{32}(X)}{E \cdot I_y(X)} dX + \int_0^x \frac{Mf_{21}(X) \cdot Mf_{31}(X)}{E \cdot I_y(X)} dX + \int_x^a \frac{Mf_{21}(X) \cdot Mf_{32}(X)}{E \cdot I_y(X)} dX\nonumber\\
    //          & + \int_a^{L-b} \frac{Mf_{22}(X) \cdot Mf_{32}(X)}{E \cdot I_y(X)} dX + \int_{L-b}^L \frac{Mf_{23}(X) \cdot Mf_{32}(X)}{E \cdot I_y(X)} dX\nonumber\\
    //          & + \int_0^x \frac{Mf_{5}(X) \cdot Mf_{31}(X)}{E \cdot I_y(X)} dX + \int_x^L \frac{Mf_{5}(X) \cdot Mf_{32}(X)}{E \cdot I_y(X)} dX \textrm{ pour x variant de 0 à a}\end{align*}\begin{align*}
    // r_y(x) = & \int_0^a \frac{Mf_{11}(X) \cdot Mf_{31}(X)}{E \cdot I_y(X)} dX + \int_a^x \frac{Mf_{12}(X) \cdot Mf_{31}(X)}{E \cdot I_y(X)} dX + \int_x^{L-b} \frac{Mf_{12}(X) \cdot Mf_{32}(X)}{E \cdot I_y(X)} dX\nonumber\\
    //          & + \int_{L-b}^L \frac{Mf_{13}(X) \cdot Mf_{32}(X)}{E \cdot I_y(X)} dX + \int_0^a \frac{Mf_{21}(X) \cdot Mf_{31}(X)}{E \cdot I_y(X)} dX + \int_a^x \frac{Mf_{22}(X) \cdot Mf_{31}(X)}{E \cdot I_y(X)} dX\nonumber\\
    //          & + \int_x^{L-b} \frac{Mf_{22}(X) \cdot Mf_{32}(X)}{E \cdot I_y(X)} dX + \int_{L-b}^L \frac{Mf_{23}(X) \cdot Mf_{32}(X)}{E \cdot I_y(X)} dX\nonumber\\
    //          & + \int_0^x \frac{Mf_{5}(X) \cdot Mf_{31}(X)}{E \cdot I_y(X)} dX + \int_x^L \frac{Mf_{5}(X) \cdot Mf_{32}(X)}{E \cdot I_y(X)} dX \textrm{ pour x variant de a à L-b}\end{align*}\begin{align*}
    // r_y(x) = & \int_0^a \frac{Mf_{11}(X) \cdot Mf_{31}(X)}{E \cdot I_y(X)} dX + \int_a^{L-b} \frac{Mf_{12}(X) \cdot Mf_{31}(X)}{E \cdot I_y(X)} dX + \int_{L-b}^x \frac{Mf_{13}(X) \cdot Mf_{31}(X)}{E \cdot I_y(X)} dX\nonumber\\
    //          & + \int_x^L \frac{Mf_{13}(X) \cdot Mf_{32}(X)}{E \cdot I_y(X)} dX + \int_0^a \frac{Mf_{21}(X) \cdot Mf_{31}(X)}{E \cdot I_y(X)} dX + \int_a^{L-b} \frac{Mf_{22}(X) \cdot Mf_{31}(X)}{E \cdot I_y(X)} dX\nonumber\\
    //          & + \int_{L-b}^x \frac{Mf_{23}(X) \cdot Mf_{31}(X)}{E \cdot I_y(X)} dX + \int_x^L \frac{Mf_{23}(X) \cdot Mf_{32}(X)}{E \cdot I_y(X)} dX\nonumber\\
    //          & + \int_0^x \frac{Mf_{5}(X) \cdot Mf_{31}(X)}{E \cdot I_y(X)} dX + \int_x^L \frac{Mf_{5}(X) \cdot Mf_{32}(X)}{E \cdot I_y(X)} dX \textrm{ pour x variant de L-b à L}\end{align*}\begin{align*}
    // f_z(x) = & \int_0^x \frac{Mf_{11}(X) \cdot Mf_{41}(X)}{E \cdot I_y(X)} dX + \int_x^a \frac{Mf_{11}(X) \cdot Mf_{42}(X)}{E \cdot I_y(X)} dX + \int_a^{L-b} \frac{Mf_{12}(X) \cdot Mf_{42}(X)}{E \cdot I_y(X)} dX\nonumber\\
    //          & + \int_{L-b}^L \frac{Mf_{13}(X) \cdot Mf_{42}(X)}{E \cdot I_y(X)} dX + \int_0^x \frac{Mf_{21}(X) \cdot Mf_{41}(X)}{E \cdot I_y(X)} dX + \int_x^a \frac{Mf_{21}(X) \cdot Mf_{42}(X)}{E \cdot I_y(X)} dX\nonumber\\
    //          & + \int_a^{L-b} \frac{Mf_{22}(X) \cdot Mf_{42}(X)}{E \cdot I_y(X)} dX + \int_{L-b}^L \frac{Mf_{23}(X) \cdot Mf_{42}(X)}{E \cdot I_y(X)} dX\nonumber\\
    //          & + \int_0^x \frac{Mf_{5}(X) \cdot Mf_{41}(X)}{E \cdot I_y(X)} dX + \int_x^L \frac{Mf_{5}(X) \cdot Mf_{42}(X)}{E \cdot I_y(X)} dX \textrm{ pour x variant de 0 à a}\end{align*}\begin{align*}
    // f_z(x) = & \int_0^a \frac{Mf_{11}(X) \cdot Mf_{41}(X)}{E \cdot I_y(X)} dX + \int_a^x \frac{Mf_{12}(X) \cdot Mf_{41}(X)}{E \cdot I_y(X)} dX + \int_x^{L-b} \frac{Mf_{12}(X) \cdot Mf_{42}(X)}{E \cdot I_y(X)} dX\nonumber\\
    //          & + \int_{L-b}^L \frac{Mf_{13}(X) \cdot Mf_{42}(X)}{E \cdot I_y(X)} dX + \int_0^a \frac{Mf_{21}(X) \cdot Mf_{41}(X)}{E \cdot I_y(X)} dX + \int_a^x \frac{Mf_{22}(X) \cdot Mf_{41}(X)}{E \cdot I_y(X)} dX\nonumber\\
    //          & + \int_x^{L-b} \frac{Mf_{22}(X) \cdot Mf_{42}(X)}{E \cdot I_y(X)} dX + \int_{L-b}^L \frac{Mf_{23}(X) \cdot Mf_{42}(X)}{E \cdot I_y(X)} dX\nonumber\\
    //          & + \int_0^x \frac{Mf_{5}(X) \cdot Mf_{41}(X)}{E \cdot I_y(X)} dX + \int_x^L \frac{Mf_{5}(X) \cdot Mf_{42}(X)}{E \cdot I_y(X)} dX \textrm{ pour x variant de a à L-b}\end{align*}\begin{align*}
    // f_z(x) = & \int_0^a \frac{Mf_{11}(X) \cdot Mf_{41}(X)}{E \cdot I_y(X)} dX + \int_a^{L-b} \frac{Mf_{12}(X) \cdot Mf_{41}(X)}{E \cdot I_y(X)} dX + \int_{L-b}^x \frac{Mf_{13}(X) \cdot Mf_{41}(X)}{E \cdot I_y(X)} dX\nonumber\\
    //          & + \int_x^L \frac{Mf_{13}(X) \cdot Mf_{42}(X)}{E \cdot I_y(X)} dX + \int_0^a \frac{Mf_{21}(X) \cdot Mf_{41}(X)}{E \cdot I_y(X)} dX + \int_a^{L-b} \frac{Mf_{22}(X) \cdot Mf_{41}(X)}{E \cdot I_y(X)} dX\nonumber\\
    //          & + \int_{L-b}^x \frac{Mf_{23}(X) \cdot Mf_{41}(X)}{E \cdot I_y(X)} dX + \int_x^L \frac{Mf_{23}(X) \cdot Mf_{42}(X)}{E \cdot I_y(X)} dX\nonumber\\
    //          & + \int_0^x \frac{Mf_{5}(X) \cdot Mf_{41}(X)}{E \cdot I_y(X)} dX + \int_x^L \frac{Mf_{5}(X) \cdot Mf_{42}(X)}{E \cdot I_y(X)} dX \textrm{ pour x variant de L-b à L}\end{align*}\begin{align*}
    // \textrm{ avec } Mf_{11}(X) = &\frac{-f_z \cdot (L-a-b) \cdot (L-a+b) \cdot X}{2 \cdot L} \nonumber\\
    //                 Mf_{12}(X) = & \frac{-f_z \cdot (L-a-b) \cdot (L+a-b) \cdot (L-X)}{2 \cdot L} + \frac{f_z \cdot (L-b-X)^2}{2}\nonumber\\
    //                 Mf_{13}(X) = &-\frac{f_z \cdot (L-a-b) \cdot (L+a-b) \cdot (L-X)}{2 \cdot L} \nonumber\\
    //                 Mf_{21}(X) = &-\frac{m_y \cdot (L-a-b) \cdot X}{L}\nonumber\\
    //                 Mf_{22}(X) = &\frac{m_y \cdot (L-a-b) \cdot (L-X)}{L} - m_y \cdot (L-b-X) \nonumber\\
    //                 Mf_{23}(X) = &\frac{m_y \cdot (L-a-b) \cdot (L-X)}{L}\nonumber\\
    //                 Mf_{31}(X) = &-\frac{X}{L} \nonumber\\
    //                 Mf_{32}(X) = &\frac{L-X}{L} \nonumber\\
    //                 Mf_{41}(X) = &-\frac{(L-x) \cdot X}{L} \nonumber\\
    //                 Mf_{42}(X) = &-\frac{x \cdot (L-X)}{L} \nonumber\\
    //                 Mf_{5}(X)  = &-\frac{M_{Ay} \cdot (L-X)}{L}+\frac{M_{By} \cdot X}{L} \end{align*}\begin{verbatim}
    
    if (discretisation == 0)
        debut = barre->noeud_debut;
    else
        debut = barre->noeuds_intermediaires[discretisation-1];
    if (discretisation == barre->discretisation_element)
        fin = barre->noeud_fin;
    else
        fin = barre->noeuds_intermediaires[discretisation];
    
    debut_barre = EF_noeuds_distance(debut, barre->noeud_debut);
    l = EF_noeuds_distance(debut, fin);
    BUGMSG(!((a < 0.) && (!(ERREUR_RELATIVE_EGALE(a, 0.)))), -1, "%s : a = %.20f, l = %.20f\n", "EF_charge_barre_repartie_uniforme_fonc_ry", a, l);
    BUGMSG(!((a > l) && (!(ERREUR_RELATIVE_EGALE(a, l)))), -1, "%s : a = %.20f, l=%.20f\n", "EF_charge_barre_repartie_uniforme_fonc_ry", a, l);
    BUGMSG(!((b < 0.) && (!(ERREUR_RELATIVE_EGALE(b, 0.)))), -1, "%s : b = %.20f, l=%.20f\n", "EF_charge_barre_repartie_uniforme_fonc_ry", b, l);
    BUGMSG(!((b > l) && (!(ERREUR_RELATIVE_EGALE(b, l)))), -1, "%s : b = %.20f, l=%.20f\n", "EF_charge_barre_repartie_uniforme_fonc_ry", b, l);
    BUGMSG(!((a > l-b) && (!(ERREUR_RELATIVE_EGALE(a, l-b)))), -1, "%s : a = %.20f, l = %.20f, b = %.20f\n", "EF_charge_barre_repartie_uniforme_fonc_ry", a, l, b);
    
    E = barre->materiau->ecm;
    
    switch(((Beton_Section_Rectangulaire*)(barre->section))->type)
    {
        case BETON_SECTION_RECTANGULAIRE :
        case BETON_SECTION_T :
        case BETON_SECTION_CARRE :
        case BETON_SECTION_CIRCULAIRE :
        {
            double      I = _1992_1_1_sections_iy(barre->section);
            BUG(common_fonction_ajout(f_rotation, 0., a, -fz*(l-a-b)*(l-a+b)*(l*l-a*a-b*b+2*a*l)/(24.*l*E*I), 0., fz*(l-a-b)*(l-a+b)*(6.)/(24.*l*E*I), 0., 0., 0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_rotation, a, l-b, fz*(a*a*a*a+4.*a*a*l*l-b*b*b*b+2.*b*b*l*l-l*l*l*l)/(24.*l*E*I), fz*(-12.*a*a*l)/(24.*l*E*I), fz*(6.*a*a-6.*b*b+6.*l*l)/(24.*l*E*I), fz*(-4.*l)/(24.*l*E*I), 0., 0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_rotation, l-b, l, fz*(a*a*a*a+4.*a*a*l*l-b*b*b*b+4.*b*b*b*l-10.*b*b*l*l+12.*b*l*l*l-5.*l*l*l*l)/(24.*l*E*I), fz*(-12.*a*a*l+12.*b*b*l-24.*b*l*l+12.*l*l*l)/(24.*l*E*I), fz*(6.*a*a-6.*b*b+12.*b*l-6.*l*l)/(24.*l*E*I), 0., 0., 0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_rotation, 0., a, my*(-a*a*a+3.*a*a*l-2.*a*l*l-b*b*b+b*l*l)/(6.*l*E*I), 0., my*(-3.*a-3.*b+3.*l)/(6.*l*E*I), 0., 0., 0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_rotation, a, l-b, my*(-a*a*a-2.*a*l*l-b*b*b+b*l*l)/(6.*l*E*I), my*(6.*a*l)/(6.*l*E*I), my*(-3.*a-3.*b)/(6.*l*E*I), 0., 0., 0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_rotation, l-b, l, my*(-a*a*a-2.*a*l*l-b*b*b+3.*b*b*l-5.*b*l*l+3.*l*l*l)/(6.*l*E*I), my*(6.*a*l+6*b*l-6*l*l)/(6.*l*E*I), my*(-3.*a-3.*b+3.*l)/(6.*l*E*I), 0., 0., 0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_rotation, 0., l, -l/(6.*E*I)*(2.*may-mby), may/(E*I), -(may+mby)/(2.*E*I*l), 0., 0., 0., 0., debut_barre) == 0, -3);
            
            BUG(common_fonction_ajout(f_deform, 0., a, 0., fz/(24.*E*I*l)*(l-a-b)*(l-a+b)*(l*l-a*a+2.*a*l-b*b), 0., fz/(24.*E*I*l)*(l-a-b)*(l-a+b)*(-2.), 0., 0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_deform, a, l-b, fz/(24.*E*I*l)*(a*a*a*a*l), fz/(24.*E*I*l)*(-a*a*a*a-4.*a*a*l*l+b*b*b*b-2.*b*b*l*l+l*l*l*l), fz/(24.*E*I*l)*(6.*a*a*l), fz/(24.*E*I*l)*(-2.*a*a+2.*b*b-2.*l*l), fz/(24.*E*I*l)*(l), 0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_deform, l-b, l, fz/(24.*E*I*l)*(l-a-b)*(l+a-b)*(-a*a*l-b*b*l+2*b*l*l-l*l*l), fz/(24.*E*I*l)*(l-a-b)*(l+a-b)*(a*a+b*b-2*b*l+5*l*l), fz/(24.*E*I*l)*(l-a-b)*(l+a-b)*(-6.*l), fz/(24.*E*I*l)*(l-a-b)*(l+a-b)*(2.), 0., 0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_deform, 0., a, 0., my/(6.*E*I*l)*(l-a-b)*(-a*a+a*b+2.*a*l-b*b-b*l), 0., my/(6.*E*I*l)*(l-a-b)*(-1.), 0., 0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_deform, a, l-b, my/(6.*E*I*l)*(-a*a*a*l), my/(6.*E*I*l)*(a*a*a+2.*a*l*l+b*b*b-b*l*l), my/(6.*E*I*l)*(-3.*a*l), my/(6.*E*I*l)*(a+b), 0., 0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_deform, l-b, l, my/(6.*E*I*l)*(l-a-b)*(a*a*l-a*b*l+a*l*l+b*b*l-2.*b*l*l+l*l*l), my/(6.*E*I*l)*(l-a-b)*(-a*a+a*b-a*l-b*b+2.*b*l-3.*l*l), my/(6.*E*I*l)*(l-a-b)*(3.*l), my/(6.*E*I*l)*(l-a-b)*(-1.), 0., 0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_deform, 0., l, 0., l/(6*E*I)*(2*may-mby), -may/(2*E*I), (mby+may)/(6*E*I*l), 0., 0., 0., debut_barre) == 0, -3);
    // Pour une section constante, les formules valent :\end{verbatim}\begin{align*}
    // r_y(x) = & \frac{f_z \cdot (L-a-b) \cdot (L-a+b) \cdot (-L^2 + a^2 + b^2 - 2 \cdot a \cdot L + 6 \cdot x^2)}{24 \cdot E \cdot I_y \cdot L}\nonumber\\
    //          & + \frac{m_y \cdot (L-a-b) \cdot (a^2-a \cdot (b + 2 \cdot L) + b^2 + b \cdot L + 3 \cdot x^2)}{6 \cdot E \cdot I_y \cdot L}\nonumber\\
    //          & - \frac{L^2 \cdot (2 \cdot M_{Ay} + m_{By}) - 6 \cdot L \cdot M_{Ay} \cdot x + 3 \cdot (M_{Ay} + M_{By} \cdot x^2)}{6 \cdot E \cdot I_y \cdot L}\textrm{ pour x variant de 0 à a}\nonumber\\
    // r_y(x) = & - f_z \cdot \frac{4 \cdot L \cdot x^3 - 6 \cdot x^2 \cdot (L^2+a^2-b^2)+12 \cdot a^2 \cdot L \cdot x+L^4-2 \cdot b^2 \cdot L^2-4 \cdot a^2 \cdot L^2+b^4-a^4}{24 \cdot L \cdot E \cdot I_y}\nonumber\\
    //          & +\frac{m_y \cdot (-a^3-a \cdot(2 \cdot L^2 - 6 \cdot L \cdot x + 3 \cdot x^2) + b \cdot (b^2 - L^2 + 3 \cdot x^2))}{6 \cdot E \cdot I_y \cdot L}\nonumber\\
    //          & - \frac{L^2 \cdot (2 \cdot M_{Ay} + m_{By}) - 6 \cdot L \cdot M_{Ay} \cdot x + 3 \cdot (M_{Ay} + M_{By} \cdot x^2)}{6 \cdot E \cdot I_y \cdot L}\textrm{ pour x variant de a à L-b}\nonumber\\
    // r_y(x) = & -f_z \cdot \frac{(L-b-a) \cdot (L-b+a) \cdot (6 \cdot x^2-12 \cdot L \cdot x+5 \cdot L^2-2 \cdot b \cdot L+b^2+a^2)}{24 \cdot L \cdot E \cdot I_y})\nonumber\\
    //          & +\frac{m_y \cdot (L-a-b) \cdot (a^2 + a \cdot (L-b) + b^2 - 2 \cdot b \cdot L + 3 \cdot (L-x)^2)}{6 \cdot E \cdot I_y \cdot L}\nonumber\\
    //          & - \frac{L^2 \cdot (2 \cdot M_{Ay} + m_{By}) - 6 \cdot L \cdot M_{Ay} \cdot x + 3 \cdot (M_{Ay} + M_{By} \cdot x^2)}{6 \cdot E \cdot I_y \cdot L}\textrm{ pour x variant de L-b à L}\end{align*}\begin{align*}
    // f_z(x) = & \frac{f_z \cdot (L-b-a) \cdot (L+b-a) \cdot x \cdot (-2 \cdot x^2+L^2+2 \cdot a \cdot L-b^2-a^2)}{24 \cdot L \cdot E \cdot I_y}\nonumber\\
    //          & -\frac{(L-b-a) \cdot my \cdot x \cdot (x^2+b \cdot L-2 \cdot a \cdot L+b^2-a \cdot b+a^2)}{6 \cdot L \cdot E \cdot I_y} \nonumber\\
    //          & x \cdot \frac{L^2 \cdot (2 \cdot M_{Ay} - M_{By}) - 3 \cdot L \cdot M_{Ay} \cdot x + (M_{Ay} + M_{By}) \cdot x^2}{6 \cdot E \cdot I_y} \textrm{ pour x variant de 0 à a}\nonumber\\
    // f_z(x) = & \frac{f_z \cdot (L \cdot x^4+(-2 L^2+2 b^2-2 a^2) \cdot x^3+6 a^2 \cdot L \cdot x^2+(L^4-(2 b^2+4 a^2) \cdot L^2+b^4-a^4) \cdot x+a^4 \cdot L)}{24 \cdot L \cdot E \cdot I_y} \nonumber\\
    //          & + \frac{my \cdot (b \cdot x^3+a \cdot x^3-3 \cdot a \cdot L \cdot x^2-b \cdot L^2 \cdot x+2 \cdot a \cdot L^2 \cdot x+b^3 \cdot x+a^3 \cdot x-a^3 \cdot L)}{6 \cdot L \cdot E \cdot I_y} \nonumber\\
    //          & x \cdot \frac{L^2 \cdot (2 \cdot M_{Ay} - M_{By}) - 3 \cdot L \cdot M_{Ay} \cdot x + (M_{Ay} + M_{By}) \cdot x^2}{6 \cdot E \cdot I_y} \textrm{ pour x variant de a à L-b}\nonumber\\
    // f_z(x) = & \frac{f_z \cdot (L-b-a) \cdot (L-b+a) \cdot (x-L) \cdot (2 \cdot x^2-4 \cdot L \cdot x+L^2-2 \cdot b \cdot L+b^2+a^2)}{24 \cdot L \cdot E \cdot I_y} \nonumber\\
    //          & -\frac{(L-b-a) \cdot m_y \cdot (x-L) \cdot (x^2-2 \cdot L \cdot x+L^2-2 \cdot b \cdot L+a \cdot L+b^2-a \cdot b+a^2)}{6 \cdot L \cdot E \cdot I_y} \nonumber\\
    //          & x \cdot \frac{L^2 \cdot (2 \cdot M_{Ay} - M_{By}) - 3 \cdot L \cdot M_{Ay} \cdot x + (M_{Ay} + M_{By}) \cdot x^2}{6 \cdot E \cdot I_y} \textrm{ pour x variant de L-b à L}\end{align*}\begin{verbatim}
            return 0;
            break;
        }
        default :
        {
            BUGMSG(0, 0, "EF_charge_barre_repartie_uniforme_fonc_ry\n");
            break;
        }
    }
}


int EF_charge_barre_repartie_uniforme_fonc_rz(Fonction *f_rotation, Fonction* f_deform,
  Beton_Barre *barre, unsigned int discretisation, double a, double b, double fy, double mz,
  double maz, double mbz)
/* Description : Calcule les déplacements d'une barre en rotation autour de l'axe z et en
 *                 déformation selon l'axe y en fonction de la charge linéaire (fy et mz) et
 *                 des efforts aux extrémités de la poutre.
 * Paramètres : Fonction *fonction : fonction où sera ajoutée la déformée,
 *              Beton_Barre *barre : Barre à étudier,
 *              unsigned int discretisation : partie de la barre à étudier,
 *              double a : position du début de la charge par rapport au début de la barre,
 *              double b : position de la fin de la charge par rapport à la fin de la barre,
 *              double fy : charge linéaire suivant l'axe y
 *              double mz : moment linéaire  autour de l'axe z
 *              double maz : moment au début de la barre,
 *              double mbz : moment à la fin de la barre.
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (f_rotation == NULL) ou
 *             (f_deform == NULL) ou
 *             (barre == NULL) ou
 *             (barre->section == NULL) ou
 *             (barre->materiau == 0) ou
 *             (barre->noeud_debut == NULL) ou
 *             (barre->noeud_fin == NULL) ou
 *             (discretisation>barre->discretisation_element) ou
 *             (a < 0) ou (a > l) ou
 *             (b < 0) ou (b > l) ou
 *             (a > l-b)
 */
{
    EF_Noeud    *debut, *fin;
    double      l;
    double      E, debut_barre;
    
    BUGMSG(f_rotation, -1, "EF_charge_barre_repartie_uniforme_fonc_rz\n");
    BUGMSG(f_deform, -1, "EF_charge_barre_repartie_uniforme_fonc_rz\n");
    BUGMSG(barre, -1, "EF_charge_barre_repartie_uniforme_fonc_rz\n");
    BUGMSG(barre->section, -1, "EF_charge_barre_repartie_uniforme_fonc_rz\n");
    BUGMSG(barre->materiau, -1, "EF_charge_barre_repartie_uniforme_fonc_rz\n");
    BUGMSG(barre->noeud_debut, -1, "EF_charge_barre_repartie_uniforme_fonc_rz\n");
    BUGMSG(barre->noeud_fin, -1, "EF_charge_barre_repartie_uniforme_fonc_rz\n");
    BUGMSG(discretisation<=barre->discretisation_element, -1, "EF_charge_barre_repartie_uniforme_fonc_rz\n");
    
    // La déformation et la rotation d'une barre soumise à un effort de flexion autour de
    //   l'axe y est calculée selon le principe des intégrales de Mohr et est définie par les
    //   mêmes formules que la fonction EF_charge_barre_repartie_uniforme_fonc_rz à ceci près
    //   que fz est remplacé par fy (il y également un changement de signe de fy lors du calcul
    //   des rotations dû au changement de repère), my par mz, may et mby par maz et mbz (il y
    //   également un changement de signe de mz, maz et mbz lors du calcul des déformations
    //   toujours dû au changement de repère) et Iy par Iz.
    
    if (discretisation == 0)
        debut = barre->noeud_debut;
    else
        debut = barre->noeuds_intermediaires[discretisation-1];
    if (discretisation == barre->discretisation_element)
        fin = barre->noeud_fin;
    else
        fin = barre->noeuds_intermediaires[discretisation];
    
    debut_barre = EF_noeuds_distance(debut, barre->noeud_debut);
    l = EF_noeuds_distance(debut, fin);
    BUGMSG(!((a < 0.) && (!(ERREUR_RELATIVE_EGALE(a, 0.)))), -1, "%s : a = %.20f, l = %.20f\n", "EF_charge_barre_repartie_uniforme_fonc_rz", a, l);
    BUGMSG(!((a > l) && (!(ERREUR_RELATIVE_EGALE(a, l)))), -1, "%s : a = %.20f, l=%.20f\n", "EF_charge_barre_repartie_uniforme_fonc_rz", a, l);
    BUGMSG(!((b < 0.) && (!(ERREUR_RELATIVE_EGALE(b, 0.)))), -1, "%s : b = %.20f, l=%.20f\n", "EF_charge_barre_repartie_uniforme_fonc_rz", b, l);
    BUGMSG(!((b > l) && (!(ERREUR_RELATIVE_EGALE(b, l)))), -1, "%s : b = %.20f, l=%.20f\n", "EF_charge_barre_repartie_uniforme_fonc_rz", b, l);
    BUGMSG(!((a > l-b) && (!(ERREUR_RELATIVE_EGALE(a, l-b)))), -1, "%s : a = %.20f, l = %.20f, b = %.20f\n", "EF_charge_barre_repartie_uniforme_fonc_rz", a, l, b);
    
    E = barre->materiau->ecm;
    
    switch(((Beton_Section_Rectangulaire*)(barre->section))->type)
    {
        case BETON_SECTION_RECTANGULAIRE :
        case BETON_SECTION_T :
        case BETON_SECTION_CARRE :
        case BETON_SECTION_CIRCULAIRE :
        {
            double      I = _1992_1_1_sections_iz(barre->section);
            BUG(common_fonction_ajout(f_rotation, 0., a, fy*(l-a-b)*(l-a+b)*(l*l-a*a-b*b+2*a*l)/(24.*l*E*I), 0., -fy*(l-a-b)*(l-a+b)*(6.)/(24.*l*E*I), 0., 0., 0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_rotation, a, l-b, -fy*(a*a*a*a+4.*a*a*l*l-b*b*b*b+2.*b*b*l*l-l*l*l*l)/(24.*l*E*I), -fy*(-12.*a*a*l)/(24.*l*E*I), -fy*(6.*a*a-6.*b*b+6.*l*l)/(24.*l*E*I), -fy*(-4.*l)/(24.*l*E*I), 0., 0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_rotation, l-b, l, -fy*(a*a*a*a+4.*a*a*l*l-b*b*b*b+4.*b*b*b*l-10.*b*b*l*l+12.*b*l*l*l-5.*l*l*l*l)/(24.*l*E*I), -fy*(-12.*a*a*l+12.*b*b*l-24.*b*l*l+12.*l*l*l)/(24.*l*E*I), -fy*(6.*a*a-6.*b*b+12.*b*l-6.*l*l)/(24.*l*E*I), 0., 0., 0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_rotation, 0., a, mz*(-a*a*a+3.*a*a*l-2.*a*l*l-b*b*b+b*l*l)/(6.*l*E*I), 0., mz*(-3.*a-3.*b+3.*l)/(6.*l*E*I), 0., 0., 0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_rotation, a, l-b, mz*(-a*a*a-2.*a*l*l-b*b*b+b*l*l)/(6.*l*E*I), mz*(6.*a*l)/(6.*l*E*I), mz*(-3.*a-3.*b)/(6.*l*E*I), 0., 0., 0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_rotation, l-b, l, mz*(-a*a*a-2.*a*l*l-b*b*b+3.*b*b*l-5.*b*l*l+3.*l*l*l)/(6.*l*E*I), mz*(6.*a*l+6*b*l-6*l*l)/(6.*l*E*I), mz*(-3.*a-3.*b+3.*l)/(6.*l*E*I), 0., 0., 0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_rotation, 0., l, -l/(6.*E*I)*(2.*maz-mbz), maz/(E*I), -(maz+mbz)/(2.*E*I*l), 0., 0., 0., 0., debut_barre) == 0, -3);
            
            BUG(common_fonction_ajout(f_deform, 0., a, 0., fy/(24.*E*I*l)*(l-a-b)*(l-a+b)*(l*l-a*a+2.*a*l-b*b), 0., fy/(24.*E*I*l)*(l-a-b)*(l-a+b)*(-2.), 0., 0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_deform, a, l-b, fy/(24.*E*I*l)*(a*a*a*a*l), fy/(24.*E*I*l)*(-a*a*a*a-4.*a*a*l*l+b*b*b*b-2.*b*b*l*l+l*l*l*l), fy/(24.*E*I*l)*(6.*a*a*l), fy/(24.*E*I*l)*(-2.*a*a+2.*b*b-2.*l*l), fy/(24.*E*I*l)*(l), 0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_deform, l-b, l, fy/(24.*E*I*l)*(l-a-b)*(l+a-b)*(-a*a*l-b*b*l+2*b*l*l-l*l*l), fy/(24.*E*I*l)*(l-a-b)*(l+a-b)*(a*a+b*b-2*b*l+5*l*l), fy/(24.*E*I*l)*(l-a-b)*(l+a-b)*(-6.*l), fy/(24.*E*I*l)*(l-a-b)*(l+a-b)*(2.), 0., 0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_deform, 0., a, 0., -mz/(6.*E*I*l)*(l-a-b)*(-a*a+a*b+2.*a*l-b*b-b*l), 0., -mz/(6.*E*I*l)*(l-a-b)*(-1.), 0., 0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_deform, a, l-b, -mz/(6.*E*I*l)*(-a*a*a*l), -mz/(6.*E*I*l)*(a*a*a+2.*a*l*l+b*b*b-b*l*l), -mz/(6.*E*I*l)*(-3.*a*l), -mz/(6.*E*I*l)*(a+b), 0., 0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_deform, l-b, l, -mz/(6.*E*I*l)*(l-a-b)*(a*a*l-a*b*l+a*l*l+b*b*l-2.*b*l*l+l*l*l), -mz/(6.*E*I*l)*(l-a-b)*(-a*a+a*b-a*l-b*b+2.*b*l-3.*l*l), -mz/(6.*E*I*l)*(l-a-b)*(3.*l), -mz/(6.*E*I*l)*(l-a-b)*(-1.), 0., 0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_deform, 0., l, 0., -l/(6*E*I)*(2*maz-mbz), maz/(2*E*I), -(mbz+maz)/(6*E*I*l), 0., 0., 0., debut_barre) == 0, -3);
            return 0;
            break;
        }
        default :
        {
            BUGMSG(0, 0, "EF_charge_barre_repartie_uniforme_fonc_rz\n");
            break;
        }
    }
}


int EF_charge_barre_repartie_uniforme_n(Fonction *fonction, Beton_Barre *barre,
  unsigned int discretisation, double a, double b, double fax, double fbx)
/* Description : Calcule les déplacements d'une barre selon l'axe x en fonction de l'effort
 *                 normal ponctuel n et des réactions d'appuis fax et fbx.
 * Paramètres : Fonction *fonction : fonction où sera ajoutée la déformée,
 *              Beton_Barre *barre : Barre à étudier,
 *              unsigned int discretisation : partie de la barre à étudier,
 *              double a : position du début de la charge par rapport au début de la barre,
 *              double b : position de la fin de la charge par rapport à la fin de la barre,
 *              double n : effort normal de la charge ponctuelle,
 *              double fax : effort normal au début de la barre,
 *              double fbx : effort normal à la fin de la barre.
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (barre == NULL) ou
 *             (fonction == NULL) ou
 *             (barre->section == NULL) ou
 *             (barre->materiau == 0) ou
 *             (barre->noeud_debut == NULL) ou
 *             (barre->noeud_fin == NULL) ou
 *             (discretisation>barre->discretisation_element)
 */
{
    EF_Noeud    *debut, *fin;
    double      l, debut_barre;
    double      E;
    
    BUGMSG(fonction, 0., "EF_charge_barre_repartie_uniforme_n\n");
    BUGMSG(barre, 0., "EF_charge_barre_repartie_uniforme_n\n");
    BUGMSG(barre->section, 0., "EF_charge_barre_repartie_uniforme_n\n");
    BUGMSG(barre->materiau, 0., "EF_charge_barre_repartie_uniforme_n\n");
    BUGMSG(barre->noeud_debut, 0., "EF_charge_barre_repartie_uniforme_n\n");
    BUGMSG(barre->noeud_fin, 0., "EF_charge_barre_repartie_uniforme_n\n");
    BUGMSG(discretisation<=barre->discretisation_element, 0., "EF_charge_barre_repartie_uniforme_n\n");
    
    // La déformation selon l'axe x est par la formule :\end{verbatim}\begin{center}
    // \includegraphics[width=8cm]{images/charge_barre_rep_uni_n2.pdf}\includegraphics[width=8cm]{images/charge_barre_ponctuelle_n2.pdf}\end{center}\begin{align*}
    // f_x(x) = & \int_0^x \frac{N_{11} \cdot N_{01}}{E \cdot S} dX + \int_x^a \frac{N_{11} \cdot N_{02}}{E \cdot S} dX + \int_a^{L-b} \frac{N_{12} \cdot N_{02}}{E \cdot S} dX + \int_{L-b}^L \frac{N_{13} \cdot N_{02}}{E \cdot S} dX \textrm{ pour x variant de 0 à a} \nonumber\\
    // f_x(x) = & \int_0^a \frac{N_{11} \cdot N_{01}}{E \cdot S} dX + \int_a^x \frac{N_{12} \cdot N_{01}}{E \cdot S} dX + \int_x^{L-b} \frac{N_{12} \cdot N_{02}}{E \cdot S} dX + \int_{L-b}^L \frac{N_{13} \cdot N_{02}}{E \cdot S} dX \textrm{ pour x variant a de L-b} \nonumber\\
    // f_x(x) = & \int_0^a \frac{N_{11} \cdot N_{01}}{E \cdot S} dX + \int_a^{L-b} \frac{N_{12} \cdot N_{01}}{E \cdot S} dX + \int_{L-b}^x \frac{N_{13} \cdot N_{01}}{E \cdot S} dX + \int_x^L \frac{N_{13} \cdot N_{02}}{E \cdot S} dX \textrm{ pour x variant L-b de L} \end{align*}\begin{align*}
    // N_{01} = &-\frac{L-x}{L} & N_{02} = & \frac{x}{L} \nonumber\\
    // N_{11} = & -F_{Ax} & N_{12} = & \frac{(F_{Ax} + F_{Bx}) \cdot X - a \cdot F_{Bx} - F_{Ax} \cdot (L-b)}{L-a-b} \nonumber\\
    // N_{13} = & F_{Bx} & & \nonumber\\
    // F_{A1} = & -\frac{L-x}{L} & F_{A2} = & \frac{x}{L} \end{align*}\begin{verbatim}
    
    if (discretisation == 0)
        debut = barre->noeud_debut;
    else
        debut = barre->noeuds_intermediaires[discretisation-1];
    if (discretisation == barre->discretisation_element)
        fin = barre->noeud_fin;
    else
        fin = barre->noeuds_intermediaires[discretisation];
    
    debut_barre = EF_noeuds_distance(debut, barre->noeud_debut);
    l = EF_noeuds_distance(debut, fin);
    BUGMSG(!((a < 0.) && (!(ERREUR_RELATIVE_EGALE(a, 0.)))), -1, "%s : a = %.20f, l = %.20f\n", "EF_charge_barre_repartie_uniforme_n", a, l);
    BUGMSG(!((a > l) && (!(ERREUR_RELATIVE_EGALE(a, l)))), -1, "%s : a = %.20f, l=%.20f\n", "EF_charge_barre_repartie_uniforme_n", a, l);
    BUGMSG(!((b < 0.) && (!(ERREUR_RELATIVE_EGALE(b, 0.)))), -1, "%s : b = %.20f, l=%.20f\n", "EF_charge_barre_repartie_uniforme_n", b, l);
    BUGMSG(!((b > l) && (!(ERREUR_RELATIVE_EGALE(b, l)))), -1, "%s : b = %.20f, l=%.20f\n", "EF_charge_barre_repartie_uniforme_n", b, l);
    BUGMSG(!((a > l-b) && (!(ERREUR_RELATIVE_EGALE(a, l-b)))), -1, "%s : a = %.20f, l = %.20f, b = %.20f\n", "EF_charge_barre_repartie_uniforme_n", a, l, b);
    
    E = barre->materiau->ecm;
    
    switch(((Beton_Section_Rectangulaire*)(barre->section))->type)
    {
        case BETON_SECTION_RECTANGULAIRE :
        case BETON_SECTION_T :
        case BETON_SECTION_CARRE :
        case BETON_SECTION_CIRCULAIRE :
        {
            double      S = _1992_1_1_sections_s(barre->section);
            
    // Pour une section constante, les déformations valent :\end{verbatim}\begin{align*}
    // f_x(x) = &\frac{F_{Ax} \cdot (L-a+b) \cdot (F_{Ax} + F_{Bx}) x}{2 \cdot L \cdot E \cdot S} & & \textrm{ pour x variant de 0 à a}\nonumber\\
    // f_x(x) = &\frac{(fbx+fax) \cdot (L \cdot x^2-L^2 \cdot x+b^2 \cdot x-a^2 \cdot x+a^2 \cdot L)}{2 \cdot L \cdot (L-b-a) \cdot E \cdot S}) & & \textrm{ pour x variant de a à L-b}\nonumber\\
    // f_x(x) = &\frac{(fbx+fax) \cdot (L-b+a) \cdot (L-x)}{2 \cdot L \cdot E \cdot S} & & \textrm{ pour x variant de L-b à L}\end{align*}\begin{verbatim}
            BUG(common_fonction_ajout(fonction, 0., a, 0., (l-a+b)*(fax+fbx)/(2*l*E*S), 0., 0., 0., 0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(fonction, a, l-b, (fax+fbx)/(2*l*E*S)/(a+b-l)*(a*a*l), (fax+fbx)/(2*l*E*S)/(a+b-l)*(-a*a+b*b-l*l), (fax+fbx)/(2*l*E*S)/(a+b-l)*(l), 0., 0., 0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(fonction, l-b, l, (fax+fbx)/(2*l*E*S)*l*(a-b+l), -(fax+fbx)/(2*l*E*S)*(a-b+l), 0., 0., 0., 0., 0., debut_barre) == 0, -3);
            return 0;
            break;
        }
        default :
        {
            BUGMSG(0, 0., "EF_charge_barre_repartie_uniforme_n\n");
            break;
        }
    }
}


void EF_charge_barre_repartie_uniforme_free(Charge_Barre_Repartie_Uniforme *charge)
{
/* Description : Libère le contenu alloué dans une charge répartie uniforme sur barre.
 * Paramètres : Charge_Barre_Repartie_Uniforme *charge : la charge à libérer.
 * Valeur renvoyée : void
 */
    free(charge->description);
    list_free(charge->barres, LIST_DEALLOC);
    return;
}
