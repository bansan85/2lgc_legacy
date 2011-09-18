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

#include "1990_actions.h"
#include "1992_1_1_section.h"
#include "common_projet.h"
#include "common_erreurs.h"
#include "common_maths.h"
#include "common_fonction.h"
#include "EF_noeud.h"
#include "EF_charge_noeud.h"
#include "EF_charge_barre_repartie_uniforme.h"


int EF_charge_barre_repartie_uniforme_ajout(Projet *projet, int num_action, void *barre, 
  int repere_local, int projection, double debut, double fin, double fx, double fy, double fz,
  double mx, double my, double mz)
/* Description : Ajoute une charge répartie uniforme à une action et le long d'une barre
 *                 en lui attribuant le numéro suivant la dernière charge de l'action.
 * Paramètres : Projet *projet : la variable projet,
 *            : int num_action : numero de l'action qui contiendra la charge,
 *            : void *barre : barre qui supportera la charge,
 *            : int repere_local : TRUE si les charges doivent être prise dans le repère local,
 *                                 FALSE pour le repère global,
 *            : int projection : TRUE si la charge doit être projetée sur la barre,
 *                               FALSE si aucune projection,
 *                               projection = TRUE est incompatible avec repere_local = TRUE,
 *            : double debut : position en mètre de la charge par rapport au début de la barre,
 *            : double fin : position en mètre de la charge par rapport à la fin de la barre,
 *            : double fx : force suivant l'axe x,
 *            : double fy : force suivant l'axe y,
 *            : double fz : force suivant l'axe z,
 *            : double mx : moment autour de l'axe x,
 *            : double my : moment autour de l'axe y,
 *            : double mz : moment autour de l'axe z.
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet == NULL) ou
 *             (projet->actions == NULL) ou
 *             (list_size(projet->actions) == 0) ou
 *             (barre == NULL) ou
 *             (projection == TRUE) && (repere_local == TRUE) ou
 *             (_1990_action_cherche_numero(projet, num_action) != 0)
 *           -2 en cas d'erreur d'allocation mémoire
 */
{
    Action          *action_en_cours;
    Charge_Barre_Repartie_Uniforme *charge_dernier, charge_nouveau;
    
    // Trivial
    BUGMSG(projet, -1, "EF_charge_barre_repartie_uniforme_ajout\n");
    BUGMSG(projet->actions, -1, "EF_charge_barre_repartie_uniforme_ajout\n");
    BUGMSG(list_size(projet->actions), -1, "EF_charge_barre_repartie_uniforme_ajout\n");
    BUGMSG(barre, -1, "EF_charge_barre_repartie_uniforme_ajout\n");
    BUGMSG((projection == FALSE) || (repere_local == FALSE), -1, "EF_charge_barre_repartie_uniforme_ajout\n");
    
    BUG(_1990_action_cherche_numero(projet, num_action) == 0, -1);
    action_en_cours = list_curr(projet->actions);
    
    charge_nouveau.type = CHARGE_BARRE_REPARTIE_UNIFORME;
    charge_nouveau.nom = NULL;
    charge_nouveau.description = NULL;
    charge_nouveau.barre = barre;
    charge_nouveau.repere_local = repere_local;
    charge_nouveau.projection = projection;
    charge_nouveau.debut = debut;
    charge_nouveau.fin = fin;
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
    BUGMSG(list_insert_after(action_en_cours->charges, &(charge_nouveau), sizeof(charge_nouveau)), -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_charge_barre_repartie_uniforme_ajout");
    
    return 0;
}


int EF_charge_barre_repartie_uniforme_mx(Beton_Barre *barre, unsigned int discretisation,
  double a, double b, Barre_Info_EF *infos, double mx, double *ma, double *mb)
/* Description : Calcule les moments aux encastrements pour l'élément spécifié soumis à un
 *                 moment de torsion uniformément répartie mx dans le repère local. Les
 *                 résultats sont renvoyés par l'intermédiaire des pointeurs ma et mb qui ne
 *                 peuvent être NULL.
 * Paramètres : Beton_Barre *barre : Barre à étudier,
 *            : unsigned int discretisation : partie de la barre à étudier,
 *            : double debut : début et fin de la charge uniformément répartie respectivement
 *            : double fin : par rapport au début et la fin de la partie de barre étudiée,
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
 *             (kAx == kBx == MAXDOUBLE)
 */
{
    Beton_Section_Rectangulaire *section_tmp;
    EF_Noeud    *debut, *fin;
    double      xx, yy, zz, l;
    double      G;

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
    
    section_tmp = barre->section;
    // Les moments aux extrémités de la barre sont définis par les formules :\end{verbatim}\begin{displaymath}
    // M_{Bx} = \frac{\int_0^a \frac{C \cdot (L-a-b)}{J(x) \cdot G } dx + \int_a^{L-b} \frac{-C \cdot x + C \cdot (L-b)}{J(x) \cdot G } dx + k_A \cdot C \cdot (L-a-b)}{\int_0^l \frac{1}{J(x) \cdot G}dx + k_A + k_B}\end{displaymath}\begin{displaymath}
    // M_{Ax} = M_x - M_{Bx} \cdot (L-a-b)\end{displaymath}\begin{verbatim}
    
    if (discretisation == 0)
        debut = barre->noeud_debut;
    else
        debut = barre->noeuds_intermediaires[discretisation-1];
    if (discretisation == barre->discretisation_element)
        fin = barre->noeud_fin;
    else
        fin = barre->noeuds_intermediaires[discretisation];
    
    xx = fin->position.x - debut->position.x;
    yy = fin->position.y - debut->position.y;
    zz = fin->position.z - debut->position.z;
    l = sqrt(xx*xx+yy*yy+zz*zz);
    
    G = barre->materiau->gnu_0_2;
    
    switch(section_tmp->type)
    {
        case BETON_SECTION_RECTANGULAIRE :
        case BETON_SECTION_T :
        case BETON_SECTION_CARRE :
        case BETON_SECTION_CIRCULAIRE :
        {
            double      J = _1992_1_1_sections_j(barre->section);
            
    // Pour une section section constante, les moments valent :\end{verbatim}\begin{displaymath}
    // M_{Bx} = \frac{(L-a-b) \cdot C \cdot (a-b+2*G*J*k_A+l)}{2*(G*J*(k_A+k_B)+l}\end{displaymath}\begin{verbatim}
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
 *             (phib == NULL)
 */
{
    Beton_Section_Rectangulaire *section_tmp;
    EF_Noeud    *debut, *fin;
    double      xx, yy, zz, l;
    double      E;
    
    BUGMSG(barre, -1, "EF_charge_barre_ponctuelle_def_ang_iso_y\n");
    BUGMSG(barre->section, -1, "EF_charge_barre_ponctuelle_def_ang_iso_y\n");
    BUGMSG(barre->materiau, -1, "EF_charge_barre_ponctuelle_def_ang_iso_y\n");
    BUGMSG(barre->noeud_debut, -1, "EF_charge_barre_ponctuelle_def_ang_iso_y\n");
    BUGMSG(barre->noeud_fin, -1, "EF_charge_barre_ponctuelle_def_ang_iso_y\n");
    BUGMSG(discretisation<=barre->discretisation_element, -1, "EF_charge_barre_ponctuelle_def_ang_iso_y\n");
    BUGMSG(phia, -1, "EF_charge_barre_ponctuelle_def_ang_iso_y\n");
    BUGMSG(phib, -1, "EF_charge_barre_ponctuelle_def_ang_iso_y\n");
    
    section_tmp = barre->section;
    
    // Les angles phi_A et phi_B sont déterminées par les intégrales de Mohr :\end{verbatim}\begin{align*}
    // \varphi_A = & \int_0^a \frac{F_{A1} \cdot x \cdot \left( 1-\frac{x}{L}\right)}{E \cdot I} dx + \int_a^{L-b} \frac{(-F_{A1} \cdot a + F_{B1} \cdot b) \cdot x - a \cdot (b \cdot (F_{A1}+F_{B1})-L \cdot F_{A1})}{L-a-b}\cdot \frac{1-\frac{x}{L}}{E \cdot I} dx \nonumber\\
    // & + \int_{L-b}^L \frac{L \cdot F_{B1} - F_{B1} \cdot x}{E \cdot I} \cdot \left( 1-\frac{x}{L} \right) dx + \int_0^a \frac{F_{A2} \cdot x \cdot \left( 1-\frac{x}{L}\right)}{E \cdot I} dx \nonumber\\
    // & + \int_a^{L-b} \frac{\left( F_{B2} \cdot (L-x) +F_z \cdot \frac{(L-b-x)^2}{2} \right) \cdot \left( 1-\frac{x}{L}\right)}{E \cdot I} dx+ \int_{L-b}^L \frac{F_{B2} \cdot (L-x) \cdot \left( 1-\frac{x}{L}\right)}{E \cdot I} dx \end{align*}\begin{align*}
    // \varphi_B = & \int_0^a \frac{F_{A1} \cdot x^2}{E \cdot I \cdot L} dx + \int_a^{L-b} \frac{(-F_{A1} \cdot a + F_{B1} \cdot b) \cdot x - a \cdot (b \cdot (F_{A1}+F_{B1})-L \cdot F_{A1})}{L-a-b}\cdot \frac{x}{E \cdot I \cdot L} dx \nonumber\\
    // & + \int_{L-b}^L \frac{L \cdot F_{B1} - F_{B1} \cdot x}{E \cdot I} \cdot \frac{x}{L} dx + \int_0^a \frac{F_{A2} \cdot x^2}{E \cdot I \cdot L} dx \nonumber\\
    // & + \int_a^{L-b} \frac{\left( F_{B2} \cdot (L-x) +F_z \cdot \frac{(L-b-x)^2}{2} \right) \cdot x}{E \cdot I \cdot L} dx+ \int_{L-b}^L \frac{F_{B2} \cdot (L-x) \cdot x}{E \cdot I \cdot L} dx \texttt{ avec :}\end{align*}\begin{align*}
    // F_{A1} = & -C \cdot \frac{L-a-b}{L} & F_{B1} = & C \cdot \frac{L-a-b}{L} \nonumber\\
    // F_{A2} = & -\frac{F \cdot (L-a-b)(L-a+b)}{2 \cdot L} & F_{B2} = & -\frac{F \cdot (L-a-b)(L+a-b)}{2 \cdot L}\end{align*}\begin{verbatim}
    if (discretisation == 0)
        debut = barre->noeud_debut;
    else
        debut = barre->noeuds_intermediaires[discretisation-1];
    if (discretisation == barre->discretisation_element)
        fin = barre->noeud_fin;
    else
        fin = barre->noeuds_intermediaires[discretisation];
    
    xx = fin->position.x - debut->position.x;
    yy = fin->position.y - debut->position.y;
    zz = fin->position.z - debut->position.z;
    l = sqrt(xx*xx+yy*yy+zz*zz);
    
    E = barre->materiau->ecm;
    
    switch(section_tmp->type)
    {
        case BETON_SECTION_RECTANGULAIRE :
        case BETON_SECTION_T :
        case BETON_SECTION_CARRE :
        case BETON_SECTION_CIRCULAIRE :
        {
            double      I = _1992_1_1_sections_iy(barre->section);
    // Pour une section constante, les angles valent :\end{verbatim}\begin{displaymath}
    // \varphi_A = \frac{[a^2-a \cdot (b+2 \cdot L) + b \cdot (b+L)] \cdot C \cdot (L-a-b)}{6 \cdot E \cdot I_y \cdot L} + \frac{F \cdot (a^4-4\cdot a^3 \cdot L + 4 \cdot a^2 \cdot L^2 - b^4 + 2 \cdot b^2\cdot L^2 - L^4)}{24 \cdot E \cdot I_y \cdot L}\end{displaymath}\begin{displaymath}
    // \varphi_B = \frac{[a^2-a \cdot (b-L) + b \cdot (b-2 L)] \cdot C \cdot (L-a-b)}{6 \cdot E \cdot I_y \cdot L} + \frac{F \cdot (a^4+4 b^3  L - 4 b^2 L^2 - b^4 - 2 a^2 L^2 + L^4)}{24 \cdot E \cdot I_y \cdot L}\end{displaymath}\begin{verbatim}
            *phia = (a*a-a*(b+2*l)+b*(b+l))*my*(l-a-b)/(6*E*I*l)+fz*(a*a*a*a-4*a*a*a*l+4*a*a*l*l-b*b*b*b+2*b*b*l*l-l*l*l*l)/(24*E*I*l);
            *phib = (a*a+b*(b-2*l)-a*(b-l))*my*(l-a-b)/(6*E*I*l)+fz*(a*a*a*a+4*b*b*b*l-4*b*b*l*l-b*b*b*b-2*a*a*l*l+l*l*l*l)/(24*E*I*l);
            return 0;
            break;
        }
        default :
        {
            BUGMSG(0, 0, "EF_charge_barre_ponctuelle_def_ang_iso_y\n");
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
 *             (phib == NULL)
 */
{
    Beton_Section_Rectangulaire *section_tmp;
    EF_Noeud    *debut, *fin;
    double      xx, yy, zz, l;
    double      E;

    BUGMSG(barre, -1, "EF_charge_barre_ponctuelle_def_ang_iso_y\n");
    BUGMSG(barre->section, -1, "EF_charge_barre_ponctuelle_def_ang_iso_y\n");
    BUGMSG(barre->materiau, -1, "EF_charge_barre_ponctuelle_def_ang_iso_y\n");
    BUGMSG(barre->noeud_debut, -1, "EF_charge_barre_ponctuelle_def_ang_iso_y\n");
    BUGMSG(barre->noeud_fin, -1, "EF_charge_barre_ponctuelle_def_ang_iso_y\n");
    BUGMSG(discretisation<=barre->discretisation_element, -1, "EF_charge_barre_ponctuelle_def_ang_iso_y\n");
    BUGMSG(phia, -1, "EF_charge_barre_ponctuelle_def_ang_iso_y\n");
    BUGMSG(phib, -1, "EF_charge_barre_ponctuelle_def_ang_iso_y\n");
    
    section_tmp = barre->section;
    
    // Les angles phi_A et phi_B sont déterminées par les intégrales de Mohr :\end{verbatim}\begin{align*}
    // \varphi_A = & \int_0^a \frac{F_{A1} \cdot x \cdot \left( 1-\frac{x}{L}\right)}{E \cdot I} dx + \int_a^{L-b} \frac{(-F_{A1} \cdot a + F_{B1} \cdot b) \cdot x - a \cdot (b \cdot (F_{A1}+F_{B1})-L \cdot F_{A1})}{L-a-b}\cdot \frac{1-\frac{x}{L}}{E \cdot I} dx \nonumber\\
    // & + \int_{L-b}^L \frac{L \cdot F_{B1} - F_{B1} \cdot x}{E \cdot I} \cdot \left( 1-\frac{x}{L} \right) dx - \int_0^a \frac{F_{A2} \cdot x \cdot \left( 1-\frac{x}{L}\right)}{E \cdot I} dx \nonumber\\
    // & - \int_a^{L-b} \frac{\left( F_{B2} \cdot (L-x) +F_z \cdot \frac{(L-b-x)^2}{2} \right) \cdot \left( 1-\frac{x}{L}\right)}{E \cdot I} dx- \int_{L-b}^L \frac{F_{B2} \cdot (L-x) \cdot \left( 1-\frac{x}{L}\right)}{E \cdot I} dx \end{align*}\begin{align*}
    // \varphi_B = & \int_0^a \frac{F_{A1} \cdot x^2}{E \cdot I \cdot L} dx + \int_a^{L-b} \frac{(-F_{A1} \cdot a + F_{B1} \cdot b) \cdot x - a \cdot (b \cdot (F_{A1}+F_{B1})-L \cdot F_{A1})}{L-a-b}\cdot \frac{x}{E \cdot I \cdot L} dx \nonumber\\
    // & + \int_{L-b}^L \frac{L \cdot F_{B1} - F_{B1} \cdot x}{E \cdot I} \cdot \frac{x}{L} dx - \int_0^a \frac{F_{A2} \cdot x^2}{E \cdot I \cdot L} dx \nonumber\\
    // & - \int_a^{L-b} \frac{\left( F_{B2} \cdot (L-x) +F_z \cdot \frac{(L-b-x)^2}{2} \right) \cdot x}{E \cdot I \cdot L} dx- \int_{L-b}^L \frac{F_{B2} \cdot (L-x) \cdot x}{E \cdot I \cdot L} dx \texttt{ avec :}\end{align*}\begin{align*}
    // F_{A1} = & -C \cdot \frac{L-a-b}{L} & F_{B1} = & C \cdot \frac{L-a-b}{L} \nonumber\\
    // F_{A2} = & -\frac{F \cdot (L-a-b)(L-a+b)}{2 \cdot L} & F_{B2} = & -\frac{F \cdot (L-a-b)(L+a-b)}{2 \cdot L}\end{align*}\begin{verbatim}
    if (discretisation == 0)
        debut = barre->noeud_debut;
    else
        debut = barre->noeuds_intermediaires[discretisation-1];
    if (discretisation == barre->discretisation_element)
        fin = barre->noeud_fin;
    else
        fin = barre->noeuds_intermediaires[discretisation];
    
    xx = fin->position.x - debut->position.x;
    yy = fin->position.y - debut->position.y;
    zz = fin->position.z - debut->position.z;
    l = sqrt(xx*xx+yy*yy+zz*zz);
    
    E = barre->materiau->ecm;
    
    switch(section_tmp->type)
    {
        case BETON_SECTION_RECTANGULAIRE :
        case BETON_SECTION_T :
        case BETON_SECTION_CARRE :
        case BETON_SECTION_CIRCULAIRE :
        {
            double      I = _1992_1_1_sections_iz(barre->section);
    // Pour une section constante, les angles valent :\end{verbatim}\begin{displaymath}
    // \varphi_A = \frac{[a^2-a \cdot (b+2 \cdot L) + b \cdot (b+L)] \cdot C \cdot (L-a-b)}{6 \cdot E \cdot I_z \cdot L} - \frac{F \cdot (a^4-4\cdot a^3 \cdot L + 4 \cdot a^2 \cdot L^2 - b^4 + 2 \cdot b^2\cdot L^2 - L^4)}{24 \cdot E \cdot I_z \cdot L}\end{displaymath}\begin{displaymath}
    // \varphi_B = \frac{[a^2-a \cdot (b-L) + b \cdot (b-2 L)] \cdot C \cdot (L-a-b)}{6 \cdot E \cdot I_z \cdot L} - \frac{F \cdot (a^4+4 b^3  L - 4 b^2 L^2 - b^4 - 2 a^2 L^2 + L^4)}{24 \cdot E \cdot I_z \cdot L}\end{displaymath}\begin{verbatim}
            *phia = (a*a-a*(b+2*l)+b*(b+l))*mz*(l-a-b)/(6*E*I*l)-fy*(a*a*a*a-4*a*a*a*l+4*a*a*l*l-b*b*b*b+2*b*b*l*l-l*l*l*l)/(24*E*I*l);
            *phib = (a*a+b*(b-2*l)-a*(b-l))*mz*(l-a-b)/(6*E*I*l)-fy*(a*a*a*a+4*b*b*b*l-4*b*b*l*l-b*b*b*b-2*a*a*l*l+l*l*l*l)/(24*E*I*l);
            return 0;
            break;
        }
        default :
        {
            BUGMSG(0, 0, "EF_charge_barre_ponctuelle_def_ang_iso_y\n");
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
 */
{
    Beton_Section_Rectangulaire *section_tmp;
    
    BUGMSG(section, 0., "_1992_1_1_sections_es_l\n");
    
    // La position de la résultante de la charge est obtenue en résolvant X dans la formule :\end{verbatim}\begin{displaymath}
    // \int_a^{L-b} \frac{x-a}{S(x)} dx + \int_{L-b}^X \frac{L-b-a}{S(x)} dx = 0\end{displaymath}\begin{verbatim}
    section_tmp = section;
    
    switch(section_tmp->type)
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
            BUGMSG(0, 0., "_1992_1_1_sections_es_l\n");
            break;
        }
    }
}

int EF_charge_barre_repartie_uniforme_ajout_fonc_rx(Fonction *fonction, Beton_Barre *barre,
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
 *           -3 en cas d'erreur due à une fonction interne
 */
{
    Beton_Section_Rectangulaire *section_tmp;
    EF_Noeud    *debut, *fin;
    Barre_Info_EF *infos;
    double      xx, yy, zz, ll;
    double      G, debut_barre;

    BUGMSG(fonction, -1, "EF_charge_barre_repartie_uniforme_ajout_fonc_rx\n");
    BUGMSG(barre, -1, "EF_charge_barre_repartie_uniforme_ajout_fonc_rx\n");
    BUGMSG(barre->section, -1, "EF_charge_barre_repartie_uniforme_ajout_fonc_rx\n");
    BUGMSG(barre->materiau, -1, "EF_charge_barre_repartie_uniforme_ajout_fonc_rx\n");
    BUGMSG(barre->noeud_debut, -1, "EF_charge_barre_repartie_uniforme_ajout_fonc_rx\n");
    BUGMSG(barre->noeud_fin, -1, "EF_charge_barre_repartie_uniforme_ajout_fonc_rx\n");
    BUGMSG(discretisation<=barre->discretisation_element, -1, "EF_charge_barre_repartie_uniforme_ajout_fonc_rx\n");
    infos = &(barre->info_EF[discretisation]);
    BUGMSG(!((ERREUR_RELATIVE_EGALE(infos->kAx, MAXDOUBLE)) && (ERREUR_RELATIVE_EGALE(infos->kBx, MAXDOUBLE))), -1, "EF_charge_barre_repartie_uniforme_ajout_fonc_rx\n");
    
    section_tmp = barre->section;
    // La déformation d'une barre soumise à un effort de torsion est défini par les formules :\end{verbatim}\begin{align*}
    // r_x(x) = & M_{Ax} \cdot \left( k_{Ax} + \frac{\int_0^x \frac{1}{J} dx}{G} \right) & & \textrm{ pour x variant de 0 à a}\nonumber\\
    // r_x(x) = & \left( k_{Ax} \cdot M_{Ax} + \frac{\int_0^a \frac{M_{Ax}}{J} dx}{G} - \frac{\int_a^x \frac{x \cdot (M_{Ax}-M_{Bx}) +a \cdot M_{Bx} - M_{Ax} \cdot (L-b)}{(L-b-a) \cdot J} dx}{G} \right) & & \textrm{ pour x variant de a à L-b et $M_{Ax}$ != 0.} \nonumber\\
    // r_x(x) = & \left( k_{Bx} \cdot M_{Bx} + \frac{\int_{L-b}^L \frac{M_{Bx}}{J} dx}{G} - \frac{\int_x^{L-b} \frac{x \cdot (M_{Ax}-M_{Bx}) +a \cdot M_{Bx} - M_{Ax} \cdot (L-b)}{(L-b-a) \cdot J} dx}{G} \right) & & \textrm{ pour x variant de a à L-b et $M_{Bx}$ != 0.} \nonumber\\
    // r_x(x) = & M_{Bx} \cdot \left( k_{Bx} + \frac{\int_x^L \frac{1}{J} dx}{G} \right) & & \textrm{ pour x variant de a à L}\end{align*}\begin{verbatim}
    
    if (discretisation == 0)
        debut = barre->noeud_debut;
    else
        debut = barre->noeuds_intermediaires[discretisation-1];
    if (discretisation == barre->discretisation_element)
        fin = barre->noeud_fin;
    else
        fin = barre->noeuds_intermediaires[discretisation];
    
    xx = barre->noeud_debut->position.x - debut->position.x;
    yy = barre->noeud_debut->position.y - debut->position.y;
    zz = barre->noeud_debut->position.z - debut->position.z;
    debut_barre = sqrt(xx*xx+yy*yy+zz*zz);
    xx = fin->position.x - debut->position.x;
    yy = fin->position.y - debut->position.y;
    zz = fin->position.z - debut->position.z;
    ll = sqrt(xx*xx+yy*yy+zz*zz);
    
    G = barre->materiau->gnu_0_2;
    
    switch(section_tmp->type)
    {
        case BETON_SECTION_RECTANGULAIRE :
        case BETON_SECTION_T :
        case BETON_SECTION_CARRE :
        case BETON_SECTION_CIRCULAIRE :
        {
            double      J = _1992_1_1_sections_j(barre->section);
            
            if (ERREUR_RELATIVE_EGALE(infos->kAx, MAXDOUBLE))
                BUG(common_fonction_ajout(fonction, 0., a, 0., 0., 0., 0., 0., 0., 0., debut_barre) == 0, -3);
            else
                BUG(common_fonction_ajout(fonction, 0., a, max*infos->kAx, max/(G*J), 0., 0., 0., 0., 0., debut_barre) == 0, -3);
            if (ERREUR_RELATIVE_EGALE(infos->kAx, MAXDOUBLE))
                BUG(common_fonction_ajout(fonction, a, ll-b, mbx*infos->kBx-(-2*a*ll*mbx+b*b*max-b*b*mbx-2*b*ll*max+ll*ll*max+ll*ll*mbx)/(2.*(a+b-ll)*G*J), (-2*a*mbx-2*b*max+2*ll*max)/(2.*(a+b-ll)*G*J), -(max-mbx)/(2.*(a+b-ll)*G*J), 0., 0., 0., 0., debut_barre) == 0, -3);
            else
                BUG(common_fonction_ajout(fonction, a, ll-b, max*infos->kAx+(a*a*(max+mbx))/(2.*(a+b-ll)*G*J), (-2.*a*mbx+2.*b*max-2.*ll*max)/(2.*(a+b-ll)*G*J), (max+mbx)/(2.*(a+b-ll)*G*J), 0., 0., 0., 0., debut_barre) == 0, -3);
            if (ERREUR_RELATIVE_EGALE(infos->kBx, MAXDOUBLE))
                BUG(common_fonction_ajout(fonction, ll-b, ll, 0., 0., 0., 0., 0., 0., 0., debut_barre) == 0, -3);
            else
                BUG(common_fonction_ajout(fonction, ll-b, ll, mbx*(infos->kBx + ll/(G*J)), -mbx/(G*J), 0., 0., 0., 0., 0., debut_barre) == 0, -3);
            
    // Pour une section section constante, les moments valent :\end{verbatim}\begin{align*}
    // r_x(x) = & M_{Ax} \cdot \left( k_{Ax} + \frac{x}{J \cdot G} \right) & &\textrm{ pour x variant de 0 à a}\nonumber\\
    // r_x(x) = & M_{Bx} \cdot \left( k_{Bx} + \frac{L-x}{J \cdot G} \right) & &\textrm{ pour x variant de a à L}\end{align*}\begin{verbatim}
            return 0;
            break;
        }
        default :
        {
            BUGMSG(0, 0., "EF_charge_barre_repartie_uniforme_ajout_fonc_rx\n");
            break;
        }
    }
}

int EF_charge_barre_repartie_uniforme_ajout_fonc_ry(Fonction *f_rotation, Fonction* f_deform,
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
 *             (discretisation>barre->discretisation_element)
 */
{
    Beton_Section_Rectangulaire *section_tmp;
    EF_Noeud    *debut, *fin;
    double      xx, yy, zz, ll;
    double      E, debut_barre;
    
    BUGMSG(f_rotation, -1, "EF_charge_barre_repartie_uniforme_ajout_fonc_ry\n");
    BUGMSG(f_deform, -1, "EF_charge_barre_repartie_uniforme_ajout_fonc_ry\n");
    BUGMSG(barre, -1, "EF_charge_barre_repartie_uniforme_ajout_fonc_ry\n");
    BUGMSG(barre->section, -1, "EF_charge_barre_repartie_uniforme_ajout_fonc_ry\n");
    BUGMSG(barre->materiau, -1, "EF_charge_barre_repartie_uniforme_ajout_fonc_ry\n");
    BUGMSG(barre->noeud_debut, -1, "EF_charge_barre_repartie_uniforme_ajout_fonc_ry\n");
    BUGMSG(barre->noeud_fin, -1, "EF_charge_barre_repartie_uniforme_ajout_fonc_ry\n");
    BUGMSG(discretisation<=barre->discretisation_element, -1, "EF_charge_barre_repartie_uniforme_ajout_fonc_ry\n");
    
    section_tmp = barre->section;
    
    // La déformation en rotation d'une barre soumise à un effort de flexion autour de l'axe y
    // est calculée selon le principe des intégrales de Mohr et est définie par les formules :\end{verbatim}\begin{align*}
    // r_y(x) = & \int_0^x \frac{Mf_{11}(X) \cdot Mf_{01}(X)}{E \cdot I_y(X)} dX + \int_x^a \frac{Mf_{11}(X) \cdot Mf_{02}(X)}{E \cdot I_y(X)} dX + \int_a^{L-b} \frac{Mf_{12}(X) \cdot Mf_{02}(X)}{E \cdot I_y(X)} dX\nonumber\\
    //          & + \int_{L-b}^L \frac{Mf_{13}(X) \cdot Mf_{02}(X)}{E \cdot I_y(X)} dX + \int_0^x \frac{Mf_{21}(X) \cdot Mf_{01}(X)}{E \cdot I_y(X)} dX + \int_x^a \frac{Mf_{21}(X) \cdot Mf_{02}(X)}{E \cdot I_y(X)} dX\nonumber\\
    //          & + \int_a^{L-b} \frac{Mf_{22}(X) \cdot Mf_{02}(X)}{E \cdot I_y(X)} dX + \int_{L-b}^L \frac{Mf_{23}(X) \cdot Mf_{02}(X)}{E \cdot I_y(X)} dX\nonumber\\
    //          & + \int_0^x \frac{Mf_{3}(X) \cdot Mf_{01}(X)}{E \cdot I_y(X)} dX + \int_x^L \frac{Mf_{3}(X) \cdot Mf_{02}(X)}{E \cdot I_y(X)} dX\nonumber\\
    //          & \textrm{ pour x variant de 0 à a}\end{align*}\begin{align*}
    // r_y(x) = & \int_0^a \frac{Mf_{11}(X) \cdot Mf_{01}(X)}{E \cdot I_y(X)} dX + \int_a^x \frac{Mf_{12}(X) \cdot Mf_{01}(X)}{E \cdot I_y(X)} dX + \int_x^{L-b} \frac{Mf_{12}(X) \cdot Mf_{02}(X)}{E \cdot I_y(X)} dX\nonumber\\
    //          & + \int_{L-b}^L \frac{Mf_{13}(X) \cdot Mf_{02}(X)}{E \cdot I_y(X)} dX + \int_0^a \frac{Mf_{21}(X) \cdot Mf_{01}(X)}{E \cdot I_y(X)} dX + \int_a^x \frac{Mf_{22}(X) \cdot Mf_{01}(X)}{E \cdot I_y(X)} dX\nonumber\\
    //          & + \int_x^{L-b} \frac{Mf_{22}(X) \cdot Mf_{02}(X)}{E \cdot I_y(X)} dX + \int_{L-b}^L \frac{Mf_{23}(X) \cdot Mf_{02}(X)}{E \cdot I_y(X)} dX\nonumber\\
    //          & + \int_0^x \frac{Mf_{3}(X) \cdot Mf_{01}(X)}{E \cdot I_y(X)} dX + \int_x^L \frac{Mf_{3}(X) \cdot Mf_{02}(X)}{E \cdot I_y(X)} dX\nonumber\\
    //          & \textrm{ pour x variant de a à L-b}\end{align*}\begin{align*}
    // r_y(x) = & \int_0^a \frac{Mf_{11}(X) \cdot Mf_{01}(X)}{E \cdot I_y(X)} dX + \int_a^{L-b} \frac{Mf_{12}(X) \cdot Mf_{01}(X)}{E \cdot I_y(X)} dX + \int_{L-b}^x \frac{Mf_{13}(X) \cdot Mf_{01}(X)}{E \cdot I_y(X)} dX\nonumber\\
    //          & + \int_x^L \frac{Mf_{13}(X) \cdot Mf_{02}(X)}{E \cdot I_y(X)} dX + \int_0^a \frac{Mf_{21}(X) \cdot Mf_{01}(X)}{E \cdot I_y(X)} dX + \int_a^{L-b} \frac{Mf_{22}(X) \cdot Mf_{01}(X)}{E \cdot I_y(X)} dX\nonumber\\
    //          & + \int_{L-b}^x \frac{Mf_{23}(X) \cdot Mf_{01}(X)}{E \cdot I_y(X)} dX + \int_x^L \frac{Mf_{23}(X) \cdot Mf_{02}(X)}{E \cdot I_y(X)} dX\nonumber\\
    //          & + \int_0^x \frac{Mf_{3}(X) \cdot Mf_{01}(X)}{E \cdot I_y(X)} dX + \int_x^L \frac{Mf_{3}(X) \cdot Mf_{02}(X)}{E \cdot I_y(X)} dX\nonumber\\
    //          & \textrm{ pour x variant de a à L-b}\end{align*}\begin{align*}
    // f_z(x) = & \int_0^x \frac{Mf_{11}(X) \cdot Mf_{41}(X)}{E \cdot I_y(X)} dX + \int_x^a \frac{Mf_{11}(X) \cdot Mf_{42}(X)}{E \cdot I_y(X)} dX + \int_a^{L-b} \frac{Mf_{12}(X) \cdot Mf_{42}(X)}{E \cdot I_y(X)} dX\nonumber\\
    //          & + \int_{L-b}^L \frac{Mf_{13}(X) \cdot Mf_{42}(X)}{E \cdot I_y(X)} dX + \int_0^x \frac{Mf_{21}(X) \cdot Mf_{41}(X)}{E \cdot I_y(X)} dX + \int_x^a \frac{Mf_{21}(X) \cdot Mf_{42}(X)}{E \cdot I_y(X)} dX\nonumber\\
    //          & + \int_a^{L-b} \frac{Mf_{22}(X) \cdot Mf_{42}(X)}{E \cdot I_y(X)} dX + \int_{L-b}^L \frac{Mf_{23}(X) \cdot Mf_{42}(X)}{E \cdot I_y(X)} dX\nonumber\\
    //          & + \int_0^x \frac{Mf_{3}(X) \cdot Mf_{41}(X)}{E \cdot I_y(X)} dX + \int_x^L \frac{Mf_{3}(X) \cdot Mf_{42}(X)}{E \cdot I_y(X)} dX\nonumber\\
    //          & \textrm{ pour x variant de 0 à a}\end{align*}\begin{align*}
    // f_z(x) = & \int_0^a \frac{Mf_{11}(X) \cdot Mf_{41}(X)}{E \cdot I_y(X)} dX + \int_a^x \frac{Mf_{12}(X) \cdot Mf_{41}(X)}{E \cdot I_y(X)} dX + \int_x^{L-b} \frac{Mf_{12}(X) \cdot Mf_{42}(X)}{E \cdot I_y(X)} dX\nonumber\\
    //          & + \int_{L-b}^L \frac{Mf_{13}(X) \cdot Mf_{42}(X)}{E \cdot I_y(X)} dX + \int_0^a \frac{Mf_{21}(X) \cdot Mf_{41}(X)}{E \cdot I_y(X)} dX + \int_a^x \frac{Mf_{22}(X) \cdot Mf_{41}(X)}{E \cdot I_y(X)} dX\nonumber\\
    //          & + \int_x^{L-b} \frac{Mf_{22}(X) \cdot Mf_{42}(X)}{E \cdot I_y(X)} dX + \int_{L-b}^L \frac{Mf_{23}(X) \cdot Mf_{42}(X)}{E \cdot I_y(X)} dX\nonumber\\
    //          & + \int_0^x \frac{Mf_{3}(X) \cdot Mf_{41}(X)}{E \cdot I_y(X)} dX + \int_x^L \frac{Mf_{3}(X) \cdot Mf_{42}(X)}{E \cdot I_y(X)} dX\nonumber\\
    //          & \textrm{ pour x variant de a à L-b}\end{align*}\begin{align*}
    // f_z(x) = & \int_0^a \frac{Mf_{11}(X) \cdot Mf_{41}(X)}{E \cdot I_y(X)} dX + \int_a^{L-b} \frac{Mf_{12}(X) \cdot Mf_{41}(X)}{E \cdot I_y(X)} dX + \int_{L-b}^x \frac{Mf_{13}(X) \cdot Mf_{41}(X)}{E \cdot I_y(X)} dX\nonumber\\
    //          & + \int_x^L \frac{Mf_{13}(X) \cdot Mf_{42}(X)}{E \cdot I_y(X)} dX + \int_0^a \frac{Mf_{21}(X) \cdot Mf_{41}(X)}{E \cdot I_y(X)} dX + \int_a^{L-b} \frac{Mf_{22}(X) \cdot Mf_{41}(X)}{E \cdot I_y(X)} dX\nonumber\\
    //          & + \int_{L-b}^x \frac{Mf_{23}(X) \cdot Mf_{41}(X)}{E \cdot I_y(X)} dX + \int_x^L \frac{Mf_{23}(X) \cdot Mf_{42}(X)}{E \cdot I_y(X)} dX\nonumber\\
    //          & + \int_0^x \frac{Mf_{3}(X) \cdot Mf_{41}(X)}{E \cdot I_y(X)} dX + \int_x^L \frac{Mf_{3}(X) \cdot Mf_{42}(X)}{E \cdot I_y(X)} dX\nonumber\\
    //          & \textrm{ pour x variant de a à L-b}\end{align*}\begin{align*}
    // \textrm{ avec } Mf_{01}(X) = &-\frac{X}{L} \nonumber\\
    //                 Mf_{02}(X) = &\frac{L-X}{L} \nonumber\\
    //                 Mf_{11}(X) = &\frac{-F_z \cdot (L-a-b) \cdot (L-a+b) \cdot X}{2 \cdot L} \nonumber\\
    //                 Mf_{12}(X) = & \frac{-F_z \cdot (L-a-b) \cdot (L+a-b) \cdot (L-X)}{2 \cdot L} + \frac{F_z \cdot (L-b-X)^2}{2}\nonumber\\
    //                 Mf_{13}(X) = &-\frac{F_z \cdot (L-a-b) \cdot (L+a-b) \cdot (L-X)}{2 \cdot L} \nonumber\\
    //                 Mf_{21}(X) = &-\frac{C_y \cdot (L-a-b) \cdot X}{L}\nonumber\\
    //                 Mf_{22}(X) = &\frac{C_y \cdot (L-a-b) \cdot (L-X)}{L} - C_y \cdot (L-b-X) \nonumber\\
    //                 Mf_{23}(X) = &\frac{C_y \cdot (L-a-b) \cdot (L-X)}{L}\nonumber\\
    //                 Mf_{3}(X)  = &-\frac{M_{Ay} \cdot (L-X)}{L}+\frac{M_{By} \cdot X}{L} \nonumber\\
    //                 Mf_{41}(X) = &-\frac{(L-x) \cdot X}{L} \nonumber\\
    //                 Mf_{42}(X) = &-\frac{x \cdot (L-X)}{L} \end{align*}\begin{verbatim}
    
    if (discretisation == 0)
        debut = barre->noeud_debut;
    else
        debut = barre->noeuds_intermediaires[discretisation-1];
    if (discretisation == barre->discretisation_element)
        fin = barre->noeud_fin;
    else
        fin = barre->noeuds_intermediaires[discretisation];
    
    xx = barre->noeud_debut->position.x - debut->position.x;
    yy = barre->noeud_debut->position.y - debut->position.y;
    zz = barre->noeud_debut->position.z - debut->position.z;
    debut_barre = sqrt(xx*xx+yy*yy+zz*zz);
    xx = fin->position.x - debut->position.x;
    yy = fin->position.y - debut->position.y;
    zz = fin->position.z - debut->position.z;
    ll = sqrt(xx*xx+yy*yy+zz*zz);
    
    E = barre->materiau->ecm;
    
    switch(section_tmp->type)
    {
        case BETON_SECTION_RECTANGULAIRE :
        case BETON_SECTION_T :
        case BETON_SECTION_CARRE :
        case BETON_SECTION_CIRCULAIRE :
        {
            double      I = _1992_1_1_sections_iy(barre->section);
            BUG(common_fonction_ajout(f_rotation, 0., a, fz*(a*a*a*a-4.*a*a*a*ll+4.*a*a*ll*ll-b*b*b*b+2.*b*b*ll*ll-ll*ll*ll*ll)/(24.*ll*E*I), 0., fz*(6.*a*a-12.*a*ll-6.*b*b+6.*ll*ll)/(24.*ll*E*I), 0., 0., 0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_rotation, a, ll-b, fz*(a*a*a*a+4.*a*a*ll*ll-b*b*b*b+2.*b*b*ll*ll-ll*ll*ll*ll)/(24.*ll*E*I), fz*(-12.*a*a*ll)/(24.*ll*E*I), fz*(6.*a*a-6.*b*b+6.*ll*ll)/(24.*ll*E*I), fz*(-4.*ll)/(24.*ll*E*I), 0., 0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_rotation, ll-b, ll, fz*(a*a*a*a+4.*a*a*ll*ll-b*b*b*b+4.*b*b*b*ll-10.*b*b*ll*ll+12.*b*ll*ll*ll-5.*ll*ll*ll*ll)/(24.*ll*E*I), fz*(-12.*a*a*ll+12.*b*b*ll-24.*b*ll*ll+12.*ll*ll*ll)/(24.*ll*E*I), fz*(6.*a*a-6.*b*b+12.*b*ll-6.*ll*ll)/(24.*ll*E*I), 0., 0., 0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_rotation, 0., a, my*(-a*a*a+3.*a*a*ll-2.*a*ll*ll-b*b*b+b*ll*ll)/(6.*ll*E*I), 0., my*(-3.*a-3.*b+3.*ll)/(6.*ll*E*I), 0., 0., 0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_rotation, a, ll-b, my*(-a*a*a-2.*a*ll*ll-b*b*b+b*ll*ll)/(6.*ll*E*I), my*(6.*a*ll)/(6.*ll*E*I), my*(-3.*a-3.*b)/(6.*ll*E*I), 0., 0., 0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_rotation, ll-b, ll, my*(-a*a*a-2.*a*ll*ll-b*b*b+3.*b*b*ll-5.*b*ll*ll+3.*ll*ll*ll)/(6.*ll*E*I), my*(6.*a*ll+6*b*ll-6*ll*ll)/(6.*ll*E*I), my*(-3.*a-3.*b+3.*ll)/(6.*ll*E*I), 0., 0., 0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_rotation, 0., ll, -ll/(6.*E*I)*(2.*may-mby), may/(E*I), -(may+mby)/(2.*E*I*ll), 0., 0., 0., 0., debut_barre) == 0, -3);
            
            BUG(common_fonction_ajout(f_deform, 0., a, 0., fz/(24.*E*I*ll)*(-a*a*a*a+4.*a*a*a*ll-4.*a*a*ll*ll+b*b*b*b-2.*b*b*ll*ll+ll*ll*ll*ll), 0., fz/(24.*E*I*ll)*(-2.*a*a+4.*a*ll+2.*b*b-2.*ll*ll), 0., 0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_deform, a, ll-b, fz/(24.*E*I*ll)*(a*a*a*a*ll), fz/(24.*E*I*ll)*(-a*a*a*a-4.*a*a*ll*ll+b*b*b*b-2.*b*b*ll*ll+ll*ll*ll*ll), fz/(24.*E*I*ll)*(6.*a*a*ll), fz/(24.*E*I*ll)*(-2.*a*a+2.*b*b-2.*ll*ll), fz/(24.*E*I*ll)*(ll), 0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_deform, ll-b, ll, fz/(24.*E*I*ll)*(a*a*a*a*ll-b*b*b*b*ll+4*b*b*b*ll*ll-6*b*b*ll*ll*ll+4*b*ll*ll*ll*ll-ll*ll*ll*ll*ll), fz/(24.*E*I*ll)*(-a*a*a*a-4*a*a*ll*ll+b*b*b*b-4*b*b*b*ll+10*b*b*ll*ll-12*b*ll*ll*ll+5*ll*ll*ll*ll), fz/(24.*E*I*ll)*(6*a*a*ll-6*b*b*ll+12*b*ll*ll-6*ll*ll*ll), fz/(24.*E*I*ll)*(-2*a*a+2*b*b-4*b*ll+2*ll*ll), 0., 0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_deform, 0., a, 0., my/(6*E*I*ll)*(a*a*a-3*a*a*ll+2*a*ll*ll+b*b*b-b*ll*ll), 0., my/(6*E*I*ll)*(a+b-ll), 0., 0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_deform, a, ll-b, my/(6.*E*I*ll)*(-a*a*a*ll), my/(6.*E*I*ll)*(a*a*a+2.*a*ll*ll+b*b*b-b*ll*ll), my/(6.*E*I*ll)*(-3.*a*ll), my/(6.*E*I*ll)*(a+b), 0., 0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_deform, ll-b, ll, my/(6*E*I*ll)*(-a*a*a*ll-b*b*b*ll+3*b*b*ll*ll-3*b*ll*ll*ll+ll*ll*ll*ll), my/(6*E*I*ll)*(a*a*a+2*a*ll*ll+b*b*b-3*b*b*ll+5*b*ll*ll-3*ll*ll*ll), my/(6*E*I*ll)*(-3*a*ll-3*b*ll+3*ll*ll), my/(6*E*I*ll)*(a+b-ll), 0., 0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_deform, 0., ll, 0., ll/(6*E*I)*(2*may-mby), -may/(2*E*I), (mby+may)/(6*E*I*ll), 0., 0., 0., debut_barre) == 0, -3);
    // FAUX : Pour une section constante, les rotations valent :\end{verbatim}\begin{align*}
    // r_y(x) = & -\frac{F_z \cdot b}{6 \cdot E \cdot I_y \cdot L} [a \cdot (L+b) -3 \cdot x^2] & &\\
    //          & +\frac{M_y}{6 \cdot E \cdot I_y \cdot L} (-L^2+3 \cdot b^2 + 3 \cdot x^2) & &\\
    //          & +\frac{L}{6 \cdot E \cdot I_y} \cdot \left(-2 \cdot M_{Ay}-M_{By} + \frac{6 \cdot M_{Ay}}{L} \cdot x - 3 \cdot \frac{M_{Ay}+M_{By}}{L^2} \cdot x^2 \right) & &\textrm{ pour x variant de 0 à a}\nonumber\\
    // r_y(x) = & \frac{F_z \cdot a}{6 \cdot E \cdot I_y \cdot L}(-2 \cdot L^2-a^2 +6 \cdot L \cdot x - 3 \cdot x^2) & &\\
    //          & +\frac{M_y}{6 \cdot E \cdot I_y \cdot L} \cdot (2 \cdot L^2+3 \cdot a^2 -6 \cdot L \cdot x + 3 \cdot x^2) & &\\
    //          & +\frac{L}{6 \cdot E \cdot I_y} \cdot \left(-2 \cdot M_{Ay}-M_{By} + \frac{6 \cdot M_{Ay}}{L} \cdot x - 3 \cdot \frac{M_{Ay}+M_{By}}{L^2} \cdot x^2 \right) & &\textrm{ pour x variant de a à L}\end{align*}\begin{align*}
    // f_z(x) = & \frac{F_z \cdot b \cdot x}{6 \cdot E \cdot I_y \cdot L} \cdot \left( L^2-b^2 - x^2 \right) & &\\
    //          & + \frac{M_y \cdot x}{6 \cdot E \cdot I_y \cdot L} \cdot \left( L^2-3 \cdot b^2 - x^2 \right) & &\\
    //          & + \frac{x}{6 \cdot E \cdot I_y} \cdot \left( L \cdot (2 \cdot M_{Ay}-M_{By}) - 3 \cdot M_{Ay} \cdot x + \frac{M_{By}+M_{Ay}}{L} \cdot x^2 \right) & &\textrm{ pour x variant de 0 à a}\nonumber\\
    // f_z(x) = & \frac{F_z \cdot a}{6 \cdot E \cdot I_y \cdot L} \cdot \left( -a^2 \cdot L + (a^2+2 \cdot L^2) \cdot x - 3 \cdot L \cdot x^2 + x^3 \right) & &\\
    //          & + \frac{M_y}{6 \cdot E \cdot I_y \cdot L} \left(3 \cdot a^2 \cdot L - (2 \cdot L^2+3 \cdot a^2) \cdot x + 3 \cdot L \cdot x^2 - x^3 \right) & &\\
    //          & + \frac{x}{6 \cdot E \cdot I_y} \cdot \left( L \cdot (2 \cdot M_{Ay}-M_{By}) - 3 \cdot M_{Ay} \cdot x + \frac{M_{By}+M_{Ay}}{L} \cdot x^2 \right) & &\textrm{ pour x variant de a à L}\end{align*}\begin{verbatim}
            return 0;
            break;
        }
        default :
        {
            BUGMSG(0, 0, "EF_charge_barre_repartie_uniforme_ajout_fonc_ry\n");
            break;
        }
    }
}


int EF_charge_barre_repartie_uniforme_ajout_fonc_rz(Fonction *f_rotation, Fonction* f_deform,
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
 *             (discretisation>barre->discretisation_element)
 */
{
    Beton_Section_Rectangulaire *section_tmp;
    EF_Noeud    *debut, *fin;
    double      xx, yy, zz, ll;
    double      E, debut_barre;
    
    BUGMSG(f_rotation, -1, "EF_charge_barre_repartie_uniforme_ajout_fonc_rz\n");
    BUGMSG(f_deform, -1, "EF_charge_barre_repartie_uniforme_ajout_fonc_rz\n");
    BUGMSG(barre, -1, "EF_charge_barre_repartie_uniforme_ajout_fonc_rz\n");
    BUGMSG(barre->section, -1, "EF_charge_barre_repartie_uniforme_ajout_fonc_rz\n");
    BUGMSG(barre->materiau, -1, "EF_charge_barre_repartie_uniforme_ajout_fonc_rz\n");
    BUGMSG(barre->noeud_debut, -1, "EF_charge_barre_repartie_uniforme_ajout_fonc_rz\n");
    BUGMSG(barre->noeud_fin, -1, "EF_charge_barre_repartie_uniforme_ajout_fonc_rz\n");
    BUGMSG(discretisation<=barre->discretisation_element, -1, "EF_charge_barre_repartie_uniforme_ajout_fonc_rz\n");
    
    section_tmp = barre->section;
    
    // La déformation en rotation d'une barre soumise à un effort de flexion autour de l'axe y
    //   est calculée selon le principe des intégrales de Mohr et est définie par les mêmes
    //   formules que la fonction EF_charge_barre_ponctuelle_ajout_fonc_rz à ceci près que fz
    //   est remplacé par fy (il y également un changement de signe de fy lors du calcul des
    //   rotations dû au changement de repère), my par mz, may et mby par maz et mbz (il y
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
    
    xx = barre->noeud_debut->position.x - debut->position.x;
    yy = barre->noeud_debut->position.y - debut->position.y;
    zz = barre->noeud_debut->position.z - debut->position.z;
    debut_barre = sqrt(xx*xx+yy*yy+zz*zz);
    xx = fin->position.x - debut->position.x;
    yy = fin->position.y - debut->position.y;
    zz = fin->position.z - debut->position.z;
    ll = sqrt(xx*xx+yy*yy+zz*zz);
    
    E = barre->materiau->ecm;
    
    switch(section_tmp->type)
    {
        case BETON_SECTION_RECTANGULAIRE :
        case BETON_SECTION_T :
        case BETON_SECTION_CARRE :
        case BETON_SECTION_CIRCULAIRE :
        {
            double      I = _1992_1_1_sections_iz(barre->section);
            BUG(common_fonction_ajout(f_rotation, 0., a, -fy*(a*a*a*a-4.*a*a*a*ll+4.*a*a*ll*ll-b*b*b*b+2.*b*b*ll*ll-ll*ll*ll*ll)/(24.*ll*E*I), 0., -fy*(6.*a*a-12.*a*ll-6.*b*b+6.*ll*ll)/(24.*ll*E*I), 0., 0., 0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_rotation, a, ll-b, -fy*(a*a*a*a+4.*a*a*ll*ll-b*b*b*b+2.*b*b*ll*ll-ll*ll*ll*ll)/(24.*ll*E*I), -fy*(-12.*a*a*ll)/(24.*ll*E*I), -fy*(6.*a*a-6.*b*b+6.*ll*ll)/(24.*ll*E*I), -fy*(-4.*ll)/(24.*ll*E*I), 0., 0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_rotation, ll-b, ll, -fy*(a*a*a*a+4.*a*a*ll*ll-b*b*b*b+4.*b*b*b*ll-10.*b*b*ll*ll+12.*b*ll*ll*ll-5.*ll*ll*ll*ll)/(24.*ll*E*I), -fy*(-12.*a*a*ll+12.*b*b*ll-24.*b*ll*ll+12.*ll*ll*ll)/(24.*ll*E*I), -fy*(6.*a*a-6.*b*b+12.*b*ll-6.*ll*ll)/(24.*ll*E*I), 0., 0., 0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_rotation, 0., a, mz*(-a*a*a+3.*a*a*ll-2.*a*ll*ll-b*b*b+b*ll*ll)/(6.*ll*E*I), 0., mz*(-3.*a-3.*b+3.*ll)/(6.*ll*E*I), 0., 0., 0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_rotation, a, ll-b, mz*(-a*a*a-2.*a*ll*ll-b*b*b+b*ll*ll)/(6.*ll*E*I), mz*(6.*a*ll)/(6.*ll*E*I), mz*(-3.*a-3.*b)/(6.*ll*E*I), 0., 0., 0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_rotation, ll-b, ll, mz*(-a*a*a-2.*a*ll*ll-b*b*b+3.*b*b*ll-5.*b*ll*ll+3.*ll*ll*ll)/(6.*ll*E*I), mz*(6.*a*ll+6*b*ll-6*ll*ll)/(6.*ll*E*I), mz*(-3.*a-3.*b+3.*ll)/(6.*ll*E*I), 0., 0., 0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_rotation, 0., ll, -ll/(6.*E*I)*(2.*maz-mbz), maz/(E*I), -(maz+mbz)/(2.*E*I*ll), 0., 0., 0., 0., debut_barre) == 0, -3);
            
            BUG(common_fonction_ajout(f_deform, 0., a, 0., fy/(24.*E*I*ll)*(-a*a*a*a+4.*a*a*a*ll-4.*a*a*ll*ll+b*b*b*b-2.*b*b*ll*ll+ll*ll*ll*ll), 0., fy/(24.*E*I*ll)*(-2.*a*a+4.*a*ll+2.*b*b-2.*ll*ll), 0., 0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_deform, a, ll-b, fy/(24.*E*I*ll)*(a*a*a*a*ll), fy/(24.*E*I*ll)*(-a*a*a*a-4.*a*a*ll*ll+b*b*b*b-2.*b*b*ll*ll+ll*ll*ll*ll), fy/(24.*E*I*ll)*(6.*a*a*ll), fy/(24.*E*I*ll)*(-2.*a*a+2.*b*b-2.*ll*ll), fy/(24.*E*I*ll)*(ll), 0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_deform, ll-b, ll, fy/(24.*E*I*ll)*(a*a*a*a*ll-b*b*b*b*ll+4*b*b*b*ll*ll-6*b*b*ll*ll*ll+4*b*ll*ll*ll*ll-ll*ll*ll*ll*ll), fy/(24.*E*I*ll)*(-a*a*a*a-4*a*a*ll*ll+b*b*b*b-4*b*b*b*ll+10*b*b*ll*ll-12*b*ll*ll*ll+5*ll*ll*ll*ll), fy/(24.*E*I*ll)*(6*a*a*ll-6*b*b*ll+12*b*ll*ll-6*ll*ll*ll), fy/(24.*E*I*ll)*(-2*a*a+2*b*b-4*b*ll+2*ll*ll), 0., 0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_deform, 0., a, 0., -mz/(6*E*I*ll)*(a*a*a-3*a*a*ll+2*a*ll*ll+b*b*b-b*ll*ll), 0., -mz/(6*E*I*ll)*(a+b-ll), 0., 0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_deform, a, ll-b, -mz/(6.*E*I*ll)*(-a*a*a*ll), -mz/(6.*E*I*ll)*(a*a*a+2.*a*ll*ll+b*b*b-b*ll*ll), -mz/(6.*E*I*ll)*(-3.*a*ll), -mz/(6.*E*I*ll)*(a+b), 0., 0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_deform, ll-b, ll, -mz/(6*E*I*ll)*(-a*a*a*ll-b*b*b*ll+3*b*b*ll*ll-3*b*ll*ll*ll+ll*ll*ll*ll), -mz/(6*E*I*ll)*(a*a*a+2*a*ll*ll+b*b*b-3*b*b*ll+5*b*ll*ll-3*ll*ll*ll), -mz/(6*E*I*ll)*(-3*a*ll-3*b*ll+3*ll*ll), -mz/(6*E*I*ll)*(a+b-ll), 0., 0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_deform, 0., ll, 0., -ll/(6*E*I)*(2*maz-mbz), -maz/(-2*E*I), -(mbz+maz)/(6*E*I*ll), 0., 0., 0., debut_barre) == 0, -3);
            return 0;
            break;
        }
        default :
        {
            BUGMSG(0, 0, "EF_charge_barre_repartie_uniforme_ajout_fonc_rz\n");
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
    Beton_Section_Rectangulaire *section_tmp;
    EF_Noeud    *debut, *fin;
    double      xx, yy, zz, ll, debut_barre;
    double      E;
    
    BUGMSG(fonction, 0., "EF_charge_barre_ponctuelle_n\n");
    BUGMSG(barre, 0., "EF_charge_barre_ponctuelle_n\n");
    BUGMSG(barre->section, 0., "EF_charge_barre_ponctuelle_n\n");
    BUGMSG(barre->materiau, 0., "EF_charge_barre_ponctuelle_n\n");
    BUGMSG(barre->noeud_debut, 0., "EF_charge_barre_ponctuelle_n\n");
    BUGMSG(barre->noeud_fin, 0., "EF_charge_barre_ponctuelle_n\n");
    BUGMSG(discretisation<=barre->discretisation_element, 0., "EF_charge_barre_ponctuelle_n\n");
    
    // La déformation selon l'axe x est par la formule :\end{verbatim}\begin{align*}
    // f_x(x) = \int_0^x \frac{N_{11} \cdot N_{01}}{E \cdot S} dX + \int_x^a \frac{N_{11} \cdot N_{02}}{E \cdot S} dX + \int_a^{L-b} \frac{N_{12} \cdot N_{02}}{E \cdot S} dX + \int_{L-b}^L \frac{N_{13} \cdot N_{02}}{E \cdot S} dX \textrm{ pour x variant de 0 à a} \nonumber\\
    // f_x(x) = \int_0^a \frac{N_{11} \cdot N_{01}}{E \cdot S} dX + \int_a^x \frac{N_{12} \cdot N_{01}}{E \cdot S} dX + \int_x^{L-b} \frac{N_{12} \cdot N_{02}}{E \cdot S} dX + \int_{L-b}^L \frac{N_{13} \cdot N_{02}}{E \cdot S} dX \textrm{ pour x variant a de L-b}{E \cdot S} \nonumber\\
    // f_x(x) = \int_0^a \frac{N_{11} \cdot N_{01}}{E \cdot S} dX + \int_a^{L-b} \frac{N_{12} \cdot N_{01}}{E \cdot S} dX + \int_{L-b}^x \frac{N_{13} \cdot N_{01}}{E \cdot S} dX + \int_x^L \frac{N_{13} \cdot N_{02}}{E \cdot S} dX \textrm{ pour x variant a de L-b}{E \cdot S} \end{align*}\begin{verbatim}
    section_tmp = barre->section;
    
    if (discretisation == 0)
        debut = barre->noeud_debut;
    else
        debut = barre->noeuds_intermediaires[discretisation-1];
    if (discretisation == barre->discretisation_element)
        fin = barre->noeud_fin;
    else
        fin = barre->noeuds_intermediaires[discretisation];
    
    xx = barre->noeud_debut->position.x - debut->position.x;
    yy = barre->noeud_debut->position.y - debut->position.y;
    zz = barre->noeud_debut->position.z - debut->position.z;
    debut_barre = sqrt(xx*xx+yy*yy+zz*zz);
    xx = fin->position.x - debut->position.x;
    yy = fin->position.y - debut->position.y;
    zz = fin->position.z - debut->position.z;
    ll = sqrt(xx*xx+yy*yy+zz*zz);
    
    E = barre->materiau->ecm;
    
    switch(section_tmp->type)
    {
        case BETON_SECTION_RECTANGULAIRE :
        case BETON_SECTION_T :
        case BETON_SECTION_CARRE :
        case BETON_SECTION_CIRCULAIRE :
        {
            double      S = _1992_1_1_sections_s(barre->section);
            
    // Pour une section constante, les déformations valent :\end{verbatim}\begin{displaymath}
    // f_x(x) = \frac{F_{Ax} \cdot x}{E \cdot S} \textrm{ pour x variant de 0 à a}\end{displaymath}\begin{displaymath}
    // f_x(x) = \frac{a \cdot (F_{Ax} + F_{Bx}) - F_{Bx} \cdot x}{E \cdot S} \textrm{ pour x variant de a à l}\end{displaymath}\begin{verbatim}
            BUG(common_fonction_ajout(fonction, 0., a, 0., (ll-a+b)*(fax+fbx)/(2*ll*E*S), 0., 0., 0., 0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(fonction, a, ll-b, (fax+fbx)/(2*ll*E*S)/(a+b-ll)*(a*a*ll), (fax+fbx)/(2*ll*E*S)/(a+b-ll)*(-a*a+b*b-ll*ll), (fax+fbx)/(2*ll*E*S)/(a+b-ll)*(ll), 0., 0., 0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(fonction, ll-b, ll, (fax+fbx)/(2*ll*E*S)*ll*(a-b+ll), -(fax+fbx)/(2*ll*E*S)*(a-b+ll), 0., 0., 0., 0., 0., debut_barre) == 0, -3);
            return 0;
            break;
        }
        default :
        {
            BUGMSG(0, 0., "EF_charge_barre_ponctuelle_n\n");
            break;
        }
    }
}
