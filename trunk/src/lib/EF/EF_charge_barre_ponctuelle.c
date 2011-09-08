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
#include <values.h>
#include <cholmod.h>

#include "1990_actions.h"
#include "1992_1_1_section.h"
#include "common_projet.h"
#include "common_erreurs.h"
#include "common_maths.h"
#include "common_fonction.h"
#include "EF_noeud.h"
#include "EF_charge_noeud.h"
#include "EF_charge_barre_ponctuelle.h"


int EF_charge_barre_ponctuelle_ajout(Projet *projet, int num_action, void *barre, 
  int repere_local, double a, double fx, double fy, double fz, double mx, double my,
  double mz)
/* Description : Ajoute une charge ponctuelle à une action et à l'intérieur d'une barre en lui
 *                 attribuant le numéro suivant la dernière charge de l'action.
 * Paramètres : Projet *projet : la variable projet,
 *            : int num_action : numero de l'action qui contiendra la charge,
 *            : void *barre : barre qui supportera la charge,
 *            : int repere_local : TRUE si les charges doivent être prise dans le repère local,
 *                                 FALSE pour le repère global,
 *            : double a : position en mètre de la charge par rapport au début de la barre,
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
 *             (_1990_action_cherche_numero(projet, num_action) != 0)
 *           -2 en cas d'erreur d'allocation mémoire
 */
{
    Action          *action_en_cours;
    Charge_Barre_Ponctuelle *charge_dernier, charge_nouveau;
    
    // Trivial
    BUGMSG(projet, -1, "EF_charge_barre_ponctuelle_ajout\n");
    BUGMSG(projet->actions, -1, "EF_charge_barre_ponctuelle_ajout\n");
    BUGMSG(list_size(projet->actions), -1, "EF_charge_barre_ponctuelle_ajout\n");
    BUGMSG(barre, -1, "EF_charge_barre_ponctuelle_ajout\n");
    
    BUG(_1990_action_cherche_numero(projet, num_action) == 0, -1);
    action_en_cours = list_curr(projet->actions);
    
    charge_nouveau.type = CHARGE_PONCTUELLE_BARRE;
    charge_nouveau.nom = NULL;
    charge_nouveau.description = NULL;
    charge_nouveau.barre = barre;
    charge_nouveau.repere_local = repere_local;
    charge_nouveau.position = a;
    charge_nouveau.x = fx;
    charge_nouveau.y = fy;
    charge_nouveau.z = fz;
    charge_nouveau.mx = mx;
    charge_nouveau.my = my;
    charge_nouveau.mz = mz;
    
    charge_dernier = (Charge_Barre_Ponctuelle *)list_rear(action_en_cours->charges);
    if (charge_dernier == NULL)
        charge_nouveau.numero = 0;
    else
        charge_nouveau.numero = charge_dernier->numero+1;
    
    list_mvrear(action_en_cours->charges);
    BUGMSG(list_insert_after(action_en_cours->charges, &(charge_nouveau), sizeof(charge_nouveau)), -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_charge_barre_ponctuelle_ajout");
    
    return 0;
}


int EF_charge_barre_ponctuelle_mx(Beton_Barre *barre, unsigned int discretisation,
  double a, Barre_Info_EF *infos, double mx, double *ma, double *mb)
/* Description : Calcule les moments aux encastrements pour l'élément spécifié soumis au
 *                 moment de torsion mx dans le repère local. Les résultats sont renvoyés par
 *                 l'intermédiaire des pointeurs ma et mb qui ne peuvent être NULL.
 * Paramètres : Beton_Barre *barre : Barre à étudier,
 *            : unsigned int discretisation : partie de la barre à étudier,
 *            : double a : position de la charge par rapport au début de la partie de barre à
 *                         étudier,
 *            : Barre_Info_EF *infos : infos de la partie de barre concerné,
 *            : double mx : moment autour de l'axe y,
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
    double      xx, yy, zz, ll;
    double      G;

    BUGMSG(barre, -1, "EF_charge_barre_ponctuelle_mx\n");
    BUGMSG(infos, -1, "EF_charge_barre_ponctuelle_mx\n");
    BUGMSG(barre->section, -1, "EF_charge_barre_ponctuelle_mx\n");
    BUGMSG(barre->materiau, -1, "EF_charge_barre_ponctuelle_mx\n");
    BUGMSG(barre->noeud_debut, -1, "EF_charge_barre_ponctuelle_mx\n");
    BUGMSG(barre->noeud_fin, -1, "EF_charge_barre_ponctuelle_mx\n");
    BUGMSG(discretisation<=barre->discretisation_element, -1, "EF_charge_barre_ponctuelle_mx\n");
    BUGMSG(ma, -1, "EF_charge_barre_ponctuelle_mx\n");
    BUGMSG(mb, -1, "EF_charge_barre_ponctuelle_mx\n");
    BUGMSG(!((ERREUR_RELATIVE_EGALE(infos->kAx, MAXDOUBLE)) && (ERREUR_RELATIVE_EGALE(infos->kBx, MAXDOUBLE))), -1, "EF_charge_barre_ponctuelle_mx\n");
    
    section_tmp = barre->section;
    // Les moments aux extrémités de la barre sont définis par les formules :\end{verbatim}\begin{displaymath}
    // M_{Bx} = \frac{\frac{\int_0^a \frac{1}{J(x)}dx}{G} + k_A}{\frac{\int_0^l \frac{1}{J(x)}dx}{G} + k_A + k_B} \cdot M_x\end{displaymath}\begin{displaymath}
    // M_{Ax} = M_x - M_{Bx}\end{displaymath}\begin{verbatim}
    
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
    ll = sqrt(xx*xx+yy*yy+zz*zz);
    
    G = barre->materiau->gnu_0_2;
    
    switch(section_tmp->type)
    {
        case BETON_SECTION_RECTANGULAIRE :
        {
            Beton_Section_Rectangulaire *section = barre->section;
            double      l = section->largeur;
            double      h = section->hauteur;
            double      J;
            double      a_, b;
            
            if (l > h)
                { a_ = l; b = h; }
            else
                { a_ = h; b = l; }
            J = a_*b*b*b/16.*(16./3.-3.364*b/a_*(1.-b*b*b*b/(12.*a_*a_*a_*a_)));
            
    // Pour une section section constante, les moments valent :\end{verbatim}\begin{displaymath}
    // M_{Bx} = \frac{\frac{a}{G \cdot J} +k_{Ax}}{\frac{l}{G \cdot J} +k_{Ax}+k_{Bx}} \cdot M_x \textrm{ et } M_{Ax} = M_x - M_{Bx}\texttt{ avec :}\end{displaymath}\begin{verbatim}
    //     pour une section rectangulaire :\end{verbatim}\begin{displaymath}
    //     J = \frac{a \cdot b^3}{16} \cdot \left[\frac{16}{3}-3.364 \cdot \frac{b}{a} \cdot \left( 1-\frac{b^4}{12 \cdot a^4} \right) \right]\textrm{ et }\substack{a=max(h,l)\\b=min(h,l)} \end{displaymath}\begin{verbatim}
            if (ERREUR_RELATIVE_EGALE(infos->kAx, MAXDOUBLE))
            {
                *mb = mx;
                *ma = 0.;
            }
            else if (ERREUR_RELATIVE_EGALE(infos->kBx, MAXDOUBLE))
            {
                *mb = 0.;
                *ma = mx;
            }
            else
            {
                *mb = (a/(G*J)+infos->kAx)/(ll/(G*J)+infos->kAx+infos->kBx)*mx;
                *ma = mx - *mb;
            }
            return 0;
            break;
        }
        case BETON_SECTION_T :
        {
            Beton_Section_T *section = barre->section;
            double      lt = section->largeur_table;
            double      la = section->largeur_ame;
            double      ht = section->hauteur_table;
            double      ha = section->hauteur_ame;
            double      a_, b, aa, bb;
            double      J;
            
            if (lt > ht)
                { a_ = lt; b = ht; }
            else
                { a_ = ht; b = lt; }
            if (la > ha)
                { aa = la; bb = ha; }
            else
                { aa = ha; bb = la; }
            J = a_*b*b*b/16.*(16./3.-3.364*b/a_*(1.-b*b*b*b/(12.*a_*a_*a_*a_)))+aa*bb*bb*bb/16.*(16./3.-3.364*bb/aa*(1-bb*bb*bb*bb/(12.*aa*aa*aa*aa)));
            
    //     pour section en T (lt : largeur de la table, la : largeur de l'âme, ht : hauteur de
    //     la table, ha : hauteur de l'âme) :\end{verbatim}\begin{displaymath}
    //     J = \frac{a \cdot b^3}{16} \left[\frac{16}{3}-3.364 \frac{b}{a} \left(1-\frac{b^4}{12 a^4}\right)\right]+\frac{aa \cdot bb^3}{16} \left[\frac{16}{3}-3.364 \frac{bb}{aa} \left(1-\frac{bb^4}{12 aa^4}\right)\right]\textrm{ et }\substack{a=max(h_t,l_t)\\b=min(h_t,l_t)\\aa=max(h_a,l_a)\\bb=min(h_a,l_a)}\end{displaymath}\begin{verbatim}
            if (ERREUR_RELATIVE_EGALE(infos->kAx, MAXDOUBLE))
            {
                *mb = mx;
                *ma = 0.;
            }
            else if (ERREUR_RELATIVE_EGALE(infos->kBx, MAXDOUBLE))
            {
                *mb = 0.;
                *ma = mx;
            }
            else
            {
                *mb = (a/(G*J)+infos->kAx)/(ll/(G*J)+infos->kAx+infos->kBx)*mx;
                *ma = mx - *mb;
            }
            return 0;
            break;
        }
        case BETON_SECTION_CARRE :
        {
            Beton_Section_Carre *section = barre->section;
            double      J = section->cote*section->cote*section->cote*section->cote/16.*(16./3.-3.364*(1.-1./12.));
            
    //     pour une section carrée :\end{verbatim}\begin{displaymath}
    //     J = \frac{cote^4}{16} \cdot \left[\frac{16}{3}-3.364 \cdot \left(1-\frac{1}{12}\right)\right]\end{displaymath}\begin{verbatim}
            if (ERREUR_RELATIVE_EGALE(infos->kAx, MAXDOUBLE))
            {
                *mb = mx;
                *ma = 0.;
            }
            else if (ERREUR_RELATIVE_EGALE(infos->kBx, MAXDOUBLE))
            {
                *mb = 0.;
                *ma = mx;
            }
            else
            {
                *mb = (a/(G*J)+infos->kAx)/(ll/(G*J)+infos->kAx+infos->kBx)*mx;
                *ma = mx - *mb;
            }
            return 0;
            break;
        }
        case BETON_SECTION_CIRCULAIRE :
        {
            Beton_Section_Circulaire *section = barre->section;
            double      J = M_PI*section->diametre*section->diametre*section->diametre*section->diametre/32.;
    //     pour une section circulaire :\end{verbatim}\begin{displaymath}
    //     J = \frac{\pi \cdot \phi^4}{32}\end{displaymath}\begin{verbatim}
            if (ERREUR_RELATIVE_EGALE(infos->kAx, MAXDOUBLE))
            {
                *mb = mx;
                *ma = 0.;
            }
            else if (ERREUR_RELATIVE_EGALE(infos->kBx, MAXDOUBLE))
            {
                *mb = 0.;
                *ma = mx;
            }
            else
            {
                *mb = (a/(G*J)+infos->kAx)/(ll/(G*J)+infos->kAx+infos->kBx)*mx;
                *ma = mx - *mb;
            }
            return 0;
            break;
        }
        default :
        {
            BUGMSG(0, 0., "EF_charge_barre_ponctuelle_mx\n");
            break;
        }
    }
}


int EF_charge_barre_ponctuelle_def_ang_iso_y(Beton_Barre *barre, unsigned int discretisation,
  double a, double fz, double my, double *phia, double *phib)
/* Description : Calcule les angles de rotation pour un élément bi-articulé soumis au
 *                 chargement fz, my dans le repère local. Les résultats sont renvoyés par
 *                 l'intermédiaire des pointeurs phia et phib qui ne peuvent être NULL.
 * Paramètres : Beton_Barre *barre : Barre à étudier,
 *            : unsigned int discretisation : partie de la barre à étudier,
 *            : double a : position de la charge par rapport au début de la partie de
 *                         barre à étudier,
 *            : double fz : force suivant l'axe z
 *            : double my : moment autour de l'axe y
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
    double      xx, yy, zz, ll, b;
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
    ll = sqrt(xx*xx+yy*yy+zz*zz);
    b = ll-a;
    
    E = barre->materiau->ecm;
    
    switch(section_tmp->type)
    {
        case BETON_SECTION_RECTANGULAIRE :
        {
            Beton_Section_Rectangulaire *section = barre->section;
            double      I = section->largeur*section->hauteur*section->hauteur*section->hauteur/12.;
    // Pour une section constante, les angles valent :\end{verbatim}\begin{displaymath}
    // \varphi_A = \frac{F_z \cdot a}{6 \cdot E \cdot I \cdot L} \cdot b \cdot (2 \cdot L-a)+\frac{M_y}{6 \cdot E \cdot I \cdot L} \cdot (L^2-3 \cdot b^2)\end{displaymath}\begin{displaymath}
    // \varphi_B = -\frac{F_z \cdot a}{6 \cdot E \cdot I \cdot L} \cdot (L^2-a^2)+\frac{M_y}{6 \cdot E \cdot I \cdot L}*(L^2-3 \cdot a^2) \texttt{ avec :}\end{displaymath}\begin{verbatim}
    //     pour une section rectantulaire :\end{verbatim}\begin{displaymath}
    //     I= \frac{l \cdot h^3}{12} \end{displaymath}\begin{verbatim}
            *phia = fz*a/(6*E*I*ll)*b*(2*ll-a)+my/(6*E*I*ll)*(ll*ll-3*b*b);
            *phib = -fz*a/(6*E*I*ll)*(ll*ll-a*a)+my/(6*E*I*ll)*(ll*ll-3*a*a);
            return 0;
            break;
        }
        case BETON_SECTION_T :
        {
            Beton_Section_T *section = barre->section;
            double      lt = section->largeur_table;
            double      la = section->largeur_ame;
            double      ht = section->hauteur_table;
            double      ha = section->hauteur_ame;
    //     pour une section en T (lt : largeur de la table, la : largeur de l'âme, ht : hauteur
    //     de la table, ha : hauteur de l'âme) :\end{verbatim}\begin{displaymath}
    //     I = \frac{l_t \cdot h_t^3}{12}+\frac{l_a \cdot h_a^3}{12}+l_t \cdot h_t \cdot \left(\frac{h_t}{2}-cdg_h \right)^2+l_a \cdot h_a \cdot \left(\frac{h_a}{2}-cdg_b \right)^2 \textrm{, }\end{displaymath}\begin{displaymath}
    //     cdg_h = \frac{\frac{l_t \cdot h_t^2}{2}+l_a \cdot h_a \cdot \left(h_t+\frac{h_a}{2} \right)}{S}  \textrm{, } cdg_b = h_t+h_a-cdg_h \textrm{ et } S = l_t \cdot h_t+l_a \cdot h_a \end{displaymath}\begin{verbatim}
            double      S = lt*ht+la*ha;
            double      cdgh = (lt*ht*ht/2+la*ha*(ht+ha/2))/S;
            double      cdgb = (ht+ha)-cdgh;
            double      I = lt*ht*ht*ht/12+la*ha*ha*ha/12+lt*ht*(ht/2-cdgh)*(ht/2-cdgh)+la*ha*(ha/2-cdgb)*(ha/2-cdgb);
            *phia = fz*a/(6*E*I*ll)*b*(2*ll-a)+my/(6*E*I*ll)*(ll*ll-3*b*b);
            *phib = -fz*a/(6*E*I*ll)*(ll*ll-a*a)+my/(6*E*I*ll)*(ll*ll-3*a*a);
            
            return 0;
            break;
        }
        case BETON_SECTION_CARRE :
        {
            Beton_Section_Carre *section = barre->section;
            double      I = section->cote*section->cote*section->cote*section->cote/12.;
            
    //     pour une section carrée :\end{verbatim}\begin{displaymath}
    //     I= \frac{c^4}{12} \end{displaymath}\begin{verbatim}
            *phia = fz*a/(6*E*I*ll)*b*(2*ll-a)+my/(6*E*I*ll)*(ll*ll-3*b*b);
            *phib = -fz*a/(6*E*I*ll)*(ll*ll-a*a)+my/(6*E*I*ll)*(ll*ll-3*a*a);
            return 0;
            break;
        }
        case BETON_SECTION_CIRCULAIRE :
        {
            Beton_Section_Circulaire *section = barre->section;
            double      I = M_PI*section->diametre*section->diametre*section->diametre*section->diametre/64.;
    //     pour une section circulaire :\end{verbatim}\begin{displaymath}
    //     I= \frac{\pi \cdot \phi^4}{64} \end{displaymath}\begin{verbatim}
            *phia = fz*a/(6*E*I*ll)*b*(2*ll-a)+my/(6*E*I*ll)*(ll*ll-3*b*b);
            *phib = -fz*a/(6*E*I*ll)*(ll*ll-a*a)+my/(6*E*I*ll)*(ll*ll-3*a*a);
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


int EF_charge_barre_ponctuelle_def_ang_iso_z(Beton_Barre *barre, unsigned int discretisation,
  double a, double fy, double mz, double *phia, double *phib)
/* Description : Calcule les angles de rotation pour un élément bi-articulé soumis au
 *                 chargement fy et mz dans le repère local. Les résultats sont renvoyés par
 *                 l'intermédiaire des pointeurs phia et phib qui ne peuvent être NULL.
 * Paramètres : Beton_Barre *barre : Barre à étudier,
 *            : unsigned int discretisation : partie de la barre à étudier,
 *            : double a : position de la charge par rapport au début de la partie de barre
 *                         à étudier,
 *            : double fy : force suivant l'axe y,
 *            : double mz : moment autour de l'axe z,
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
    double      xx, yy, zz, ll, b;
    double      E;

    BUGMSG(barre, -1, "EF_charge_barre_ponctuelle_def_ang_iso_z\n");
    BUGMSG(barre->section, -1, "EF_charge_barre_ponctuelle_def_ang_iso_z\n");
    BUGMSG(barre->materiau, -1, "EF_charge_barre_ponctuelle_def_ang_iso_z\n");
    BUGMSG(barre->noeud_debut, -1, "EF_charge_barre_ponctuelle_def_ang_iso_z\n");
    BUGMSG(barre->noeud_fin, -1, "EF_charge_barre_ponctuelle_def_ang_iso_z\n");
    BUGMSG(discretisation<=barre->discretisation_element, -1, "EF_charge_barre_ponctuelle_def_ang_iso_z\n");
    BUGMSG(phia, -1, "EF_charge_barre_ponctuelle_def_ang_iso_z\n");
    BUGMSG(phib, -1, "EF_charge_barre_ponctuelle_def_ang_iso_z\n");
    
    section_tmp = barre->section;
    
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
    ll = sqrt(xx*xx+yy*yy+zz*zz);
    b = ll-a;
    
    E = barre->materiau->ecm;
    
    switch(section_tmp->type)
    {
        case BETON_SECTION_RECTANGULAIRE :
        {
            Beton_Section_Rectangulaire *section = barre->section;
            double      I = section->hauteur*section->largeur*section->largeur*section->largeur/12.;
            
    // Pour une section constante, les angles valent :\end{verbatim}\begin{displaymath}
    // \varphi_A = \frac{ F_y \cdot a}{6 \cdot E \cdot I \cdot L} b \cdot (2 \cdot L-a) - \frac{M_z}{6 \cdot E \cdot I \cdot L} \left(L^2-3*b^2 \right)\end{displaymath}\begin{displaymath}
    // \varphi_B = \frac{-F_y \cdot a}{6 \cdot E \cdot I \cdot L} (L^2-a^2) - \frac{M_z}{6 \cdot E \cdot I \cdot L} \left(L^2-3*a^2 \right) \texttt{ avec :}\end{displaymath}\begin{verbatim}
    //     pour une section rectantulaire :\end{verbatim}\begin{displaymath}
    //     I= \frac{h \cdot l^3}{12} \end{displaymath}\begin{verbatim}
            *phia = fy*a/(6*E*I*ll)*b*(2*ll-a)-mz/(6*E*I*ll)*(ll*ll-3*b*b);
            *phib = -fy*a/(6*E*I*ll)*(ll*ll-a*a)-mz/(6*E*I*ll)*(ll*ll-3*a*a);
            return 0;
            break;
        }
        case BETON_SECTION_T :
        {
            Beton_Section_T *section = barre->section;
            double      lt = section->largeur_table;
            double      la = section->largeur_ame;
            double      ht = section->hauteur_table;
            double      ha = section->hauteur_ame;
    //     pour une section en T (lt : largeur de la table, la : largeur de l'âme, ht : hauteur
    //     de la table, ha : hauteur de l'âme) :\end{verbatim}\begin{displaymath}
    //     I = \frac{h_t \cdot l_t^3}{12}+\frac{h_a \cdot l_a^3}{12}\end{displaymath}\begin{verbatim}
            double      I = ht*lt*lt*lt/12.+ha*la*la*la/12.;
            *phia = fy*a/(6*E*I*ll)*b*(2*ll-a)-mz/(6*E*I*ll)*(ll*ll-3*b*b);
            *phib = -fy*a/(6*E*I*ll)*(ll*ll-a*a)-mz/(6*E*I*ll)*(ll*ll-3*a*a);
            return 0;
            break;
        }
        case BETON_SECTION_CARRE :
        {
            Beton_Section_Carre *section = barre->section;
            double      I = section->cote*section->cote*section->cote*section->cote/12.;
            
    //     pour une section carrée :\end{verbatim}\begin{displaymath}
    //     I= \frac{c^4}{12} \end{displaymath}\begin{verbatim}
            *phia = fy*a/(6*E*I*ll)*b*(2*ll-a)-mz/(6*E*I*ll)*(ll*ll-3*b*b);
            *phib = -fy*a/(6*E*I*ll)*(ll*ll-a*a)-mz/(6*E*I*ll)*(ll*ll-3*a*a);
            return 0;
            break;
        }
        case BETON_SECTION_CIRCULAIRE :
        {
            Beton_Section_Circulaire *section = barre->section;
            double      I = M_PI*section->diametre*section->diametre*section->diametre*section->diametre/64.;
    //     pour une section circulaire :\end{verbatim}\begin{displaymath}
    //     I= \frac{\pi \cdot \phi^4}{64} \end{displaymath}\begin{verbatim}
            *phia = fy*a/(6*E*I*ll)*b*(2*ll-a)-mz/(6*E*I*ll)*(ll*ll-3*b*b);
            *phib = -fy*a/(6*E*I*ll)*(ll*ll-a*a)-mz/(6*E*I*ll)*(ll*ll-3*a*a);
            return 0;
            break;
        }
        default :
        {
            BUGMSG(0, 0., "EF_charge_barre_ponctuelle_def_ang_iso_z\n");
            break;
        }
    }
}


int EF_charge_barre_ponctuelle_ajout_fonc_rx(Fonction *fonction, Beton_Barre *barre,
  unsigned int discretisation, double a, double max, double mbx)
/* Description : Calcule les déplacements d'une barre en rotation autour de l'axe x en fonction
 *                  des efforts aux extrémités de la poutre.
 * Paramètres : Fonction *fonction : fonction où sera ajoutée la déformée,
 *            : Beton_Barre *barre : Barre à étudier,
 *            : unsigned int discretisation : partie de la barre à étudier,
 *            : double a : position du moment ponctuel autour de l'axe x par rapport au début
 *                         de la partie de barre à étudier,
 *            : double max : moment au début de la barre,
 *            : double mbx : moment à la fin de la barre.
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

    BUGMSG(fonction, -1, "EF_charge_barre_ponctuelle_ajout_fonc_rx\n");
    BUGMSG(barre, -1, "EF_charge_barre_ponctuelle_ajout_fonc_rx\n");
    BUGMSG(barre->section, -1, "EF_charge_barre_ponctuelle_ajout_fonc_rx\n");
    BUGMSG(barre->materiau, -1, "EF_charge_barre_ponctuelle_ajout_fonc_rx\n");
    BUGMSG(barre->noeud_debut, -1, "EF_charge_barre_ponctuelle_ajout_fonc_rx\n");
    BUGMSG(barre->noeud_fin, -1, "EF_charge_barre_ponctuelle_ajout_fonc_rx\n");
    BUGMSG(discretisation<=barre->discretisation_element, -1, "EF_charge_barre_ponctuelle_ajout_fonc_rx\n");
    infos = &(barre->info_EF[discretisation]);
    BUGMSG(!((ERREUR_RELATIVE_EGALE(infos->kAx, MAXDOUBLE)) && (ERREUR_RELATIVE_EGALE(infos->kBx, MAXDOUBLE))), -1, "EF_charge_barre_ponctuelle_ajout_fonc_rx\n");
    
    section_tmp = barre->section;
    // La déformation d'une barre soumise à un effort de torsion est défini par les formules :\end{verbatim}\begin{align*}
    // r_x(x) = & M_{Ax} \cdot \left( k_{Ax} + \frac{\int_0^x \frac{1}{J} dx}{G} \right) & & \textrm{ pour x variant de 0 à a}\nonumber\\
    // r_x(x) = & -\frac{\int_0^x \frac{1}{J} dx}{G} \cdot M_{Bx} + M_{Bx} \cdot \left( k_{Bx} + \frac{\int_0^L \frac{1}{J} dx}{G} \right) & & \textrm{ pour x variant de a à L}\end{align*}\begin{verbatim}
    
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
        {
            Beton_Section_Rectangulaire *section = barre->section;
            double      l = section->largeur;
            double      h = section->hauteur;
            double      J;
            double      a_, b;
            
            if (l > h)
                { a_ = l; b = h; }
            else
                { a_ = h; b = l; }
            J = a_*b*b*b/16.*(16./3.-3.364*b/a_*(1.-b*b*b*b/(12.*a_*a_*a_*a_)));
            if (ERREUR_RELATIVE_EGALE(infos->kAx, MAXDOUBLE))
                BUG(common_fonction_ajout(fonction, 0., a, 0., 0., 0., 0., debut_barre) == 0, -3);
            else
                BUG(common_fonction_ajout(fonction, 0., a, max*infos->kAx, max/(G*J), 0., 0., debut_barre) == 0, -3);
            if (ERREUR_RELATIVE_EGALE(infos->kBx, MAXDOUBLE))
                BUG(common_fonction_ajout(fonction, a, ll, 0., 0., 0., 0., debut_barre) == 0, -3);
            else
                BUG(common_fonction_ajout(fonction, a, ll, mbx*(infos->kBx + ll/(G*J)), -mbx/(G*J), 0., 0., debut_barre) == 0, -3);
            
    // Pour une section section constante, les moments valent :\end{verbatim}\begin{align*}
    // r_x(x) = & M_{Ax} \cdot \left( k_{Ax} + \frac{x}{J \cdot G} \right) & &\textrm{ pour x variant de 0 à a}\nonumber\\
    // r_x(x) = & -\frac{x}{J \cdot G} \cdot M_{Bx} + M_{Bx} \cdot \left( k_{Bx} + \frac{L}{J \cdot G} \right) & &\textrm{ pour x variant de a à L avec :}\end{align*}\begin{verbatim}
    //     pour une section rectantulaire :\end{verbatim}\begin{displaymath}
    //     J = \frac{a \cdot b^3}{16} \cdot \left[\frac{16}{3}-3.364 \cdot \frac{b}{a} \cdot \left( 1-\frac{b^4}{12 \cdot a^4} \right) \right]\textrm{ et }\substack{a=max(h,l)\\b=min(h,l)} \end{displaymath}\begin{verbatim}
            return 0;
            break;
        }
        case BETON_SECTION_T :
        {
            Beton_Section_T *section = barre->section;
            double      lt = section->largeur_table;
            double      la = section->largeur_ame;
            double      ht = section->hauteur_table;
            double      ha = section->hauteur_ame;
            double      a_, b, aa, bb;
            double      J;
            
            if (lt > ht)
                { a_ = lt; b = ht; }
            else
                { a_ = ht; b = lt; }
            if (la > ha)
                { aa = la; bb = ha; }
            else
                { aa = ha; bb = la; }
            J = a_*b*b*b/16.*(16./3.-3.364*b/a_*(1.-b*b*b*b/(12.*a_*a_*a_*a_)))+aa*bb*bb*bb/16.*(16./3.-3.364*bb/aa*(1-bb*bb*bb*bb/(12.*aa*aa*aa*aa)));
            if (ERREUR_RELATIVE_EGALE(infos->kAx, MAXDOUBLE))
                BUG(common_fonction_ajout(fonction, 0., a, 0., 0., 0., 0., debut_barre) == 0, -3);
            else
                BUG(common_fonction_ajout(fonction, 0., a, max*infos->kAx, max/(G*J), 0., 0., debut_barre) == 0, -3);
            if (ERREUR_RELATIVE_EGALE(infos->kBx, MAXDOUBLE))
                BUG(common_fonction_ajout(fonction, a, ll, 0., 0., 0., 0., debut_barre) == 0, -3);
            else
                BUG(common_fonction_ajout(fonction, a, ll, mbx*(infos->kBx + ll/(G*J)), -mbx/(G*J), 0., 0., debut_barre) == 0, -3);
            
    //     pour une section en T (lt : largeur de la table, la : largeur de l'âme, ht : hauteur
    //     de la table, ha : hauteur de l'âme), les moments valent :\end{verbatim}\begin{displaymath}
    //     J = \frac{a \cdot b^3}{16} \left[\frac{16}{3}-3.364 \frac{b}{a} \left(1-\frac{b^4}{12 a^4}\right)\right]+\frac{aa \cdot bb^3}{16} \left[\frac{16}{3}-3.364 \frac{bb}{aa} \left(1-\frac{bb^4}{12 aa^4}\right)\right]\textrm{ et }\substack{a=max(h_t,l_t)\\b=min(h_t,l_t)\\aa=max(h_a,l_a)\\bb=min(h_a,l_a)}\end{displaymath}\begin{verbatim}
            return 0;
            break;
        }
        case BETON_SECTION_CARRE :
        {
            Beton_Section_Carre *section = barre->section;
            double      J = section->cote*section->cote*section->cote*section->cote/16.*(16./3.-3.364*(1.-1./12.));
            if (ERREUR_RELATIVE_EGALE(infos->kAx, MAXDOUBLE))
                BUG(common_fonction_ajout(fonction, 0., a, 0., 0., 0., 0., debut_barre) == 0, -3);
            else
                BUG(common_fonction_ajout(fonction, 0., a, max*infos->kAx, max/(G*J), 0., 0., debut_barre) == 0, -3);
            if (ERREUR_RELATIVE_EGALE(infos->kBx, MAXDOUBLE))
                BUG(common_fonction_ajout(fonction, a, ll, 0., 0., 0., 0., debut_barre) == 0, -3);
            else
                BUG(common_fonction_ajout(fonction, a, ll, mbx*(infos->kBx + ll/(G*J)), -mbx/(G*J), 0., 0., debut_barre) == 0, -3);
            
    //     pour une section carrée :\end{verbatim}\begin{displaymath}
    //     J = \frac{cote^4}{16} \cdot \left[\frac{16}{3}-3.364 \cdot \left(1-\frac{1}{12}\right)\right]\end{displaymath}\begin{verbatim}
            return 0;
            break;
        }
        case BETON_SECTION_CIRCULAIRE :
        {
            Beton_Section_Circulaire *section = barre->section;
            double      J = M_PI*section->diametre*section->diametre*section->diametre*section->diametre/32.;
            if (ERREUR_RELATIVE_EGALE(infos->kAx, MAXDOUBLE))
                BUG(common_fonction_ajout(fonction, 0., a, 0., 0., 0., 0., debut_barre) == 0, -3);
            else
                BUG(common_fonction_ajout(fonction, 0., a, max*infos->kAx, max/(G*J), 0., 0., debut_barre) == 0, -3);
            if (ERREUR_RELATIVE_EGALE(infos->kBx, MAXDOUBLE))
                BUG(common_fonction_ajout(fonction, a, ll, 0., 0., 0., 0., debut_barre) == 0, -3);
            else
                BUG(common_fonction_ajout(fonction, a, ll, mbx*(infos->kBx + ll/(G*J)), -mbx/(G*J), 0., 0., debut_barre) == 0, -3);
    //     pour une section circulaire :\end{verbatim}\begin{displaymath}
    //     J = \frac{\pi \cdot \phi^4}{32}\end{displaymath}\begin{verbatim}
            return 0;
            break;
        }
        default :
        {
            BUGMSG(0, 0., "EF_charge_barre_ponctuelle_ajout_fonc_rx\n");
            break;
        }
    }
}


int EF_charge_barre_ponctuelle_ajout_fonc_ry(Fonction *f_rotation, Fonction* f_deform,
  Beton_Barre *barre, unsigned int discretisation, double a, double fz, double my,
  double may, double mby)
/* Description : Calcule les déplacements d'une barre en rotation autour de l'axe y et en
 *                 déformation selon l'axe z en fonction de la charge ponctuelle (fz et my) et
 *                 des efforts aux extrémités de la poutre.
 * Paramètres : Fonction *fonction : fonction où sera ajoutée la déformée,
 *            : Beton_Barre *barre : Barre à étudier,
 *            : unsigned int discretisation : partie de la barre à étudier,
 *            : double a : position de la charge par rapport au début de la partie de barre à
 *                         étudier,
 *            : double fz : force suivant l'axe z
 *            : double my : moment autour de l'axe y
 *            : double may : moment au début de la barre,
 *            : double mby : moment à la fin de la barre.
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
    double      xx, yy, zz, ll, b;
    double      E, debut_barre;
    
    BUGMSG(f_rotation, -1, "EF_charge_barre_ponctuelle_ajout_fonc_ry\n");
    BUGMSG(f_deform, -1, "EF_charge_barre_ponctuelle_ajout_fonc_ry\n");
    BUGMSG(barre, -1, "EF_charge_barre_ponctuelle_ajout_fonc_ry\n");
    BUGMSG(barre->section, -1, "EF_charge_barre_ponctuelle_ajout_fonc_ry\n");
    BUGMSG(barre->materiau, -1, "EF_charge_barre_ponctuelle_ajout_fonc_ry\n");
    BUGMSG(barre->noeud_debut, -1, "EF_charge_barre_ponctuelle_ajout_fonc_ry\n");
    BUGMSG(barre->noeud_fin, -1, "EF_charge_barre_ponctuelle_ajout_fonc_ry\n");
    BUGMSG(discretisation<=barre->discretisation_element, -1, "EF_charge_barre_ponctuelle_ajout_fonc_ry\n");
    
    section_tmp = barre->section;
    
    // La déformation en rotation d'une barre soumise à un effort de flexion autour de l'axe y
    // est calculée selon le principe des intégrales de Mohr et est définie par les formules :\end{verbatim}\begin{align*}
    // r_y(x) = & \int_0^x \frac{Mf_{01}(X) \cdot Mf_{11}(X)}{E \cdot I_y(X)} dX + \int_x^a \frac{Mf_{01}(X) \cdot Mf_{12}(X)}{E \cdot I_y(X)} dX + \int_a^L \frac{Mf_{02}(X) \cdot Mf_{12}(X)}{E \cdot I_y(X)} dX\nonumber\\
    //          & + \int_0^x \frac{Mf_{21}(X) \cdot Mf_{11}(X)}{E \cdot I_y(X)} dX + \int_x^a \frac{Mf_{21}(X) \cdot Mf_{12}(X)}{E \cdot I_y(X)} dX + \int_a^L \frac{Mf_{22}(X) \cdot Mf_{12}(X)}{E \cdot I_y(X)} dX\nonumber\\
    //          & + \int_0^x \frac{Mf_{3}(X) \cdot Mf_{11}(X)}{E \cdot I_y(X)} dX + \int_x^L \frac{Mf_{3}(X) \cdot Mf_{12}(X)}{E \cdot I_y(X)} dX\nonumber\\
    //          & \textrm{ pour x variant de 0 à a}\end{align*}\begin{align*}
    // r_y(x) = & \int_0^a \frac{Mf_{01}(X) \cdot Mf_{11}(X)}{E \cdot I_y(X)} dX + \int_a^x \frac{Mf_{02}(X) \cdot Mf_{11}(X)}{E \cdot I_y(X)} dX + \int_x^L \frac{Mf_{02}(X) \cdot Mf_{12}(X)}{E \cdot I_y(X)} dX \nonumber\\
    //          & + \int_0^a \frac{Mf_{21}(X) \cdot Mf_{11}(X)}{E \cdot I_y(X)} dX + \int_a^x \frac{Mf_{22}(X) \cdot Mf_{11}(X)}{E \cdot I_y(X)} dX + \int_x^L \frac{Mf_{22}(X) \cdot Mf_{12}(X)}{E \cdot I_y(X)} dX \nonumber\\
    //          & + \int_0^x \frac{Mf_{3}(X) \cdot Mf_{11}(X)}{E \cdot I_y(X)} dX + \int_x^L \frac{Mf_{3}(X) \cdot Mf_{12}(X)}{E \cdot I_y(X)} dX\nonumber\\
    //          &\textrm{ pour x variant de a à L}\end{align*}\begin{align*}
    // f_z(x) = & \int_0^x \frac{Mf_{01}(X) \cdot Mf_{41}(X)}{E \cdot I_y(X)} dX + \int_x^a \frac{Mf_{01}(X) \cdot Mf_{42}(X)}{E \cdot I_y(X)} dX + \int_a^L \frac{Mf_{02}(X) \cdot Mf_{42}(X)}{E \cdot I_y(X)} dX\nonumber\\
    //          & + \int_0^x \frac{Mf_{21}(X) \cdot Mf_{41}(X)}{E \cdot I_y(X)} dX + \int_x^a \frac{Mf_{21}(X) \cdot Mf_{42}(X)}{E \cdot I_y(X)} dX + \int_a^L \frac{Mf_{22}(X) \cdot Mf_{42}(X)}{E \cdot I_y(X)} dX\nonumber\\
    //          & + \int_0^x \frac{Mf_{3}(X) \cdot Mf_{41}(X)}{E \cdot I_y(X)} dX + \int_x^L \frac{Mf_{3}(X) \cdot Mf_{42}(X)}{E \cdot I_y(X)} dX\nonumber\\
    //          & \textrm{ pour x variant de 0 à a}\end{align*}\begin{align*}
    // f_z(x) = & \int_0^a \frac{Mf_{01}(X) \cdot Mf_{41}(X)}{E \cdot I_y(X)} dX + \int_a^x \frac{Mf_{02}(X) \cdot Mf_{41}(X)}{E \cdot I_y(X)} dX + \int_x^L \frac{Mf_{02}(X) \cdot Mf_{42}(X)}{E \cdot I_y(X)} dX \nonumber\\
    //          & + \int_0^a \frac{Mf_{21}(X) \cdot Mf_{41}(X)}{E \cdot I_y(X)} dX + \int_a^x \frac{Mf_{22}(X) \cdot Mf_{41}(X)}{E \cdot I_y(X)} dX + \int_x^L \frac{Mf_{22}(X) \cdot Mf_{42}(X)}{E \cdot I_y(X)} dX \nonumber\\
    //          & + \int_0^x \frac{Mf_{3}(X) \cdot Mf_{41}(X)}{E \cdot I_y(X)} dX + \int_x^L \frac{Mf_{3}(X) \cdot Mf_{42}(X)}{E \cdot I_y(X)} dX\nonumber\\
    //          &\textrm{ pour x variant de a à L}\end{align*}\begin{align*}
    // \textrm{ avec } Mf_{01}(X) = &\frac{F_z \cdot b \cdot X}{L} & Mf_{02}(X) = &F_z \cdot a \cdot \left( 1-\frac{X}{L} \right) \nonumber\\
    //                 Mf_{11}(X) = &\frac{X}{L} & Mf_{12}(X) = &-\frac{L-X}{L}\nonumber\\
    //                 Mf_{21}(X) = &\frac{M_y \cdot X}{L} & Mf_{22}(X) = &-\frac{M_y \cdot (L-X)}{L}\nonumber\\
    //                 Mf_{3}(X) =  &\frac{M_{Ay} \cdot (L-X)}{L}-\frac{M_{By} \cdot X}{L} & Mf_{41}(X) = &\frac{(L-x) \cdot X}{L}\nonumber\\
    //                 Mf_{42}(X) = &\frac{x \cdot (L-X)}{L}\end{align*}\begin{verbatim}
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
    b = ll-a;
    
    E = barre->materiau->ecm;
    
    switch(section_tmp->type)
    {
        case BETON_SECTION_RECTANGULAIRE :
        {
            Beton_Section_Rectangulaire *section = barre->section;
            double      I = section->largeur*section->hauteur*section->hauteur*section->hauteur/12.;
            BUG(common_fonction_ajout(f_rotation, 0., a, -fz*b/(6*E*I*ll)*a*(ll+b),     0.,          fz*b/(2*E*I*ll),       0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_rotation, a,  ll, -fz*a/(6*E*I*ll)*(2*ll*ll+a*a), fz*a/(E*I), -fz*a/(2*E*I*ll),      0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_rotation, 0., a, my/(6*E*I*ll)*(-ll*ll+3*b*b),   0.,          my/(2*E*I*ll),         0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_rotation, a,  ll, my/(6*E*I*ll)*(2*ll*ll+3*a*a),  -my/(E*I),  my/(2*E*I*ll),         0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_rotation, 0., ll, -ll/(6*E*I)*(2*may-mby),      may/(E*I),  -(may+mby)/(2*E*I*ll), 0., debut_barre) == 0, -3);
            
            BUG(common_fonction_ajout(f_deform, 0., a, 0.,                  fz*b/(6*E*I*ll)*(ll*ll-b*b),    0.,                  -fz*b/(6*E*I*ll),     debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_deform, a,  ll, -fz*a*a*a/(6*E*I),  fz*a/(6*E*I*ll)*(a*a+2*ll*ll),  -fz*a/(2*E*I),      fz*a/(6*E*I*ll),      debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_deform, 0., a, 0.,                  my/(6*E*I*ll)*(ll*ll-3*b*b),    0.,                  -my/(6*E*I*ll),       debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_deform, a,  ll, my/(6*E*I)*(3*a*a), -my/(6*E*I*ll)*(2*ll*ll+3*a*a), my/(6*E*I*ll)*(3*ll), -my/(6*E*I*ll),       debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_deform, 0., ll, 0.,                  ll/(6*E*I)*(2*may-mby),       -may/(2*E*I),       (mby+may)/(6*E*I*ll), debut_barre) == 0, -3);
    // Pour une section constante, les rotations valent :\end{verbatim}\begin{align*}
    // r_y(x) = & -\frac{F_z \cdot b}{6 \cdot E \cdot I_y \cdot L} [a \cdot (L+b) -3 \cdot x^2] & &\\
    //          & +\frac{M_y}{6 \cdot E \cdot I_y \cdot L} (-L^2+3 \cdot b^2 + 3 \cdot x^2) & &\\
    //          & +\frac{L}{6 \cdot E \cdot I_y} \cdot \left(-2 \cdot M_{Ay}-M_{By} + \frac{6 \cdot M_{Ay}}{L} \cdot x - 3 \cdot \frac{M_{Ay}+M_{By}}{L^2} \cdot x^2 \right) & &\textrm{ pour x variant de 0 à a}\nonumber\\
    // r_y(x) = & \frac{F_z \cdot a}{6 \cdot E \cdot I_y \cdot L}(-2 \cdot L^2-a^2 +6 \cdot L \cdot x - 3 \cdot x^2) & &\\
    //          & +\frac{M_y}{6 \cdot E \cdot I_y \cdot L} \cdot (2 \cdot L^2+3 \cdot a^2 -6 \cdot L \cdot x + 3 \cdot x^2) & &\\
    //          & +\frac{L}{6 \cdot E \cdot I_y} \cdot \left(-2 \cdot M_{Ay}-M_{By} + \frac{6 \cdot M_{Ay}}{L} \cdot x - 3 \cdot \frac{M_{Ay}+M_{By}}{L^2} \cdot x^2 \right) & &\textrm{ pour x variant de a à L}\end{align*}\begin{align*}
    // f_z(x) = & \frac{F_z \cdot b \cdot x}{6 \cdot E \cdot I_y \cdot L}  \cdot \left( L^2-b^2 - x^2 \right) & &\\
    //          & + \frac{M_y \cdot x}{6 \cdot E \cdot I_y \cdot L} \cdot \left( L^2-3 \cdot b^2 - x^2 \right) & &\\
    //          & + \frac{x}{6 \cdot E \cdot I_y} \cdot \left( L \cdot (2 \cdot M_{Ay}-M_{By}) - 3 \cdot M_{Ay} \cdot x + \frac{M_{By}+M_{Ay}}{L} \cdot x^2 \right) & &\textrm{ pour x variant de 0 à a}\nonumber\\
    // f_z(x) = & \frac{F_z \cdot a}{6 \cdot E \cdot I_y \cdot L} \cdot \left(  -a^2 \cdot L + (a^2+2 \cdot L^2) \cdot x - 3 \cdot L \cdot x^2 + x^3 \right) & &\\
    //          & + \frac{M_y}{6 \cdot E \cdot I_y \cdot L} \left(3 \cdot a^2 \cdot L - (2 \cdot L^2+3 \cdot a^2) \cdot x + 3 \cdot L \cdot x^2 - x^3 \right) & &\\
    //          & + \frac{x}{6 \cdot E \cdot I_y} \cdot \left( L \cdot (2 \cdot M_{Ay}-M_{By}) - 3 \cdot M_{Ay} \cdot x + \frac{M_{By}+M_{Ay}}{L} \cdot x^2 \right) & &\textrm{ pour x variant de a à L}\end{align*}\begin{verbatim}
    //     pour une section rectangulaire :\end{verbatim}\begin{displaymath}
    //     I= \frac{L \cdot h^3}{12} \end{displaymath}\begin{verbatim}
            return 0;
            break;
        }
        case BETON_SECTION_T :
        {
            Beton_Section_T *section = barre->section;
            double      lt = section->largeur_table;
            double      la = section->largeur_ame;
            double      ht = section->hauteur_table;
            double      ha = section->hauteur_ame;
    //     pour une section en T (lt : largeur de la table, la : largeur de l'âme, ht : hauteur
    //     de la table, ha : hauteur de l'âme) :\end{verbatim}\begin{displaymath}
    //     I = \frac{l_t \cdot h_t^3}{12}+\frac{l_a \cdot h_a^3}{12}+l_t \cdot h_t \cdot \left(\frac{h_t}{2}-cdg_h \right)^2+l_a \cdot h_a \cdot \left(\frac{h_a}{2}-cdg_b \right)^2 \textrm{, }\end{displaymath}\begin{displaymath}
    //     cdg_h = \frac{\frac{l_t \cdot h_t^2}{2}+l_a \cdot h_a \cdot \left(h_t+\frac{h_a}{2} \right)}{S}  \textrm{, } cdg_b = h_t+h_a-cdg_h \textrm{ et } S = l_t \cdot h_t+l_a \cdot h_a \end{displaymath}\begin{verbatim}
            double      S = lt*ht+la*ha;
            double      cdgh = (lt*ht*ht/2+la*ha*(ht+ha/2))/S;
            double      cdgb = (ht+ha)-cdgh;
            double      I = lt*ht*ht*ht/12+la*ha*ha*ha/12+lt*ht*(ht/2-cdgh)*(ht/2-cdgh)+la*ha*(ha/2-cdgb)*(ha/2-cdgb);
            
            BUG(common_fonction_ajout(f_rotation, 0., a, -fz*b/(6*E*I*ll)*a*(ll+b),     0.,          fz*b/(2*E*I*ll),       0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_rotation, a,  ll, -fz*a/(6*E*I*ll)*(2*ll*ll+a*a), fz*a/(E*I), -fz*a/(2*E*I*ll),      0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_rotation, 0., a, my/(6*E*I*ll)*(-ll*ll+3*b*b),   0.,          my/(2*E*I*ll),         0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_rotation, a,  ll, my/(6*E*I*ll)*(2*ll*ll+3*a*a),  -my/(E*I),  my/(2*E*I*ll),         0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_rotation, 0., ll, -ll/(6*E*I)*(2*may-mby),      may/(E*I),  -(may+mby)/(2*E*I*ll), 0., debut_barre) == 0, -3);
            
            BUG(common_fonction_ajout(f_deform, 0., a, 0.,                  fz*b/(6*E*I*ll)*(ll*ll-b*b),    0.,                  -fz*b/(6*E*I*ll),     debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_deform, a,  ll, -fz*a*a*a/(6*E*I),  fz*a/(6*E*I*ll)*(a*a+2*ll*ll),  -fz*a/(2*E*I),      fz*a/(6*E*I*ll),      debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_deform, 0., a, 0.,                  my/(6*E*I*ll)*(ll*ll-3*b*b),    0.,                  -my/(6*E*I*ll),       debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_deform, a,  ll, my/(6*E*I)*(3*a*a), -my/(6*E*I*ll)*(2*ll*ll+3*a*a), my/(6*E*I*ll)*(3*ll), -my/(6*E*I*ll),       debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_deform, 0., ll, 0.,                  ll/(6*E*I)*(2*may-mby),       -may/(2*E*I),       (mby+may)/(6*E*I*ll), debut_barre) == 0, -3);
            
            return 0;
            break;
        }
        case BETON_SECTION_CARRE :
        {
            Beton_Section_Carre *section = barre->section;
            double      I = section->cote*section->cote*section->cote*section->cote/12.;
            
            BUG(common_fonction_ajout(f_rotation, 0., a, -fz*b/(6*E*I*ll)*a*(ll+b),     0.,          fz*b/(2*E*I*ll),       0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_rotation, a,  ll, -fz*a/(6*E*I*ll)*(2*ll*ll+a*a), fz*a/(E*I), -fz*a/(2*E*I*ll),      0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_rotation, 0., a, my/(6*E*I*ll)*(-ll*ll+3*b*b),   0.,          my/(2*E*I*ll),         0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_rotation, a,  ll, my/(6*E*I*ll)*(2*ll*ll+3*a*a),  -my/(E*I),  my/(2*E*I*ll),         0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_rotation, 0., ll, -ll/(6*E*I)*(2*may-mby),      may/(E*I),  -(may+mby)/(2*E*I*ll), 0., debut_barre) == 0, -3);
            
            BUG(common_fonction_ajout(f_deform, 0., a, 0.,                  fz*b/(6*E*I*ll)*(ll*ll-b*b),    0.,                  -fz*b/(6*E*I*ll),     debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_deform, a,  ll, -fz*a*a*a/(6*E*I),  fz*a/(6*E*I*ll)*(a*a+2*ll*ll),  -fz*a/(2*E*I),      fz*a/(6*E*I*ll),      debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_deform, 0., a, 0.,                  my/(6*E*I*ll)*(ll*ll-3*b*b),    0.,                  -my/(6*E*I*ll),       debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_deform, a,  ll, my/(6*E*I)*(3*a*a), -my/(6*E*I*ll)*(2*ll*ll+3*a*a), my/(6*E*I*ll)*(3*ll), -my/(6*E*I*ll),       debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_deform, 0., ll, 0.,                  ll/(6*E*I)*(2*may-mby),       -may/(2*E*I),       (mby+may)/(6*E*I*ll), debut_barre) == 0, -3);
            
    //     pour une section carrée :\end{verbatim}\begin{displaymath}
    //     I= \frac{c^4}{12} \end{displaymath}\begin{verbatim}
            return 0;
            break;
        }
        case BETON_SECTION_CIRCULAIRE :
        {
            Beton_Section_Circulaire *section = barre->section;
            double      I = M_PI*section->diametre*section->diametre*section->diametre*section->diametre/64.;
    //     pour une section circulaire :\end{verbatim}\begin{displaymath}
    //     I= \frac{\pi \cdot \phi^4}{64} \end{displaymath}\begin{verbatim}
            
            BUG(common_fonction_ajout(f_rotation, 0., a, -fz*b/(6*E*I*ll)*a*(ll+b),     0.,          fz*b/(2*E*I*ll),       0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_rotation, a,  ll, -fz*a/(6*E*I*ll)*(2*ll*ll+a*a), fz*a/(E*I), -fz*a/(2*E*I*ll),      0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_rotation, 0., a, my/(6*E*I*ll)*(-ll*ll+3*b*b),   0.,          my/(2*E*I*ll),         0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_rotation, a,  ll, my/(6*E*I*ll)*(2*ll*ll+3*a*a),  -my/(E*I),  my/(2*E*I*ll),         0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_rotation, 0., ll, -ll/(6*E*I)*(2*may-mby),      may/(E*I),  -(may+mby)/(2*E*I*ll), 0., debut_barre) == 0, -3);
            
            BUG(common_fonction_ajout(f_deform, 0., a, 0.,                  fz*b/(6*E*I*ll)*(ll*ll-b*b),    0.,                  -fz*b/(6*E*I*ll),     debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_deform, a,  ll, -fz*a*a*a/(6*E*I),  fz*a/(6*E*I*ll)*(a*a+2*ll*ll),  -fz*a/(2*E*I),      fz*a/(6*E*I*ll),      debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_deform, 0., a, 0.,                  my/(6*E*I*ll)*(ll*ll-3*b*b),    0.,                  -my/(6*E*I*ll),       debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_deform, a,  ll, my/(6*E*I)*(3*a*a), -my/(6*E*I*ll)*(2*ll*ll+3*a*a), my/(6*E*I*ll)*(3*ll), -my/(6*E*I*ll),       debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_deform, 0., ll, 0.,                  ll/(6*E*I)*(2*may-mby),       -may/(2*E*I),       (mby+may)/(6*E*I*ll), debut_barre) == 0, -3);
            return 0;
            break;
        }
        default :
        {
            BUGMSG(0, 0, "EF_charge_barre_ponctuelle_ajout_fonc_ry\n");
            break;
        }
    }
}


int EF_charge_barre_ponctuelle_ajout_fonc_rz(Fonction *f_rotation, Fonction* f_deform,
  Beton_Barre *barre, unsigned int discretisation, double a, double fy, double mz,
  double maz, double mbz)
/* Description : Calcule les déplacements d'une barre en rotation autour de l'axe z et en
 *                 déformation selon l'axe y en fonction de la charge ponctuelle (fy et mz) et
 *                 des efforts aux extrémités de la poutre.
 * Paramètres : Fonction *fonction : fonction où sera ajoutée la déformée,
 *            : Beton_Barre *barre : Barre à étudier,
 *            : unsigned int discretisation : partie de la barre à étudier,
 *            : double a : position de la charge par rapport au début de la partie de barre à
 *                         étudier,
 *            : double fy : force suivant l'axe y
 *            : double mz : moment autour de l'axe z
 *            : double maz : moment au début de la barre,
 *            : double mbz : moment à la fin de la barre.
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
    double      xx, yy, zz, ll, b;
    double      E, debut_barre;
    
    BUGMSG(f_rotation, -1, "EF_charge_barre_ponctuelle_ajout_fonc_rz\n");
    BUGMSG(f_deform, -1, "EF_charge_barre_ponctuelle_ajout_fonc_rz\n");
    BUGMSG(barre, -1, "EF_charge_barre_ponctuelle_ajout_fonc_rz\n");
    BUGMSG(barre->section, -1, "EF_charge_barre_ponctuelle_ajout_fonc_rz\n");
    BUGMSG(barre->materiau, -1, "EF_charge_barre_ponctuelle_ajout_fonc_rz\n");
    BUGMSG(barre->noeud_debut, -1, "EF_charge_barre_ponctuelle_ajout_fonc_rz\n");
    BUGMSG(barre->noeud_fin, -1, "EF_charge_barre_ponctuelle_ajout_fonc_rz\n");
    BUGMSG(discretisation<=barre->discretisation_element, -1, "EF_charge_barre_ponctuelle_ajout_fonc_rz\n");
    
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
    b = ll-a;
    
    E = barre->materiau->ecm;
    
    switch(section_tmp->type)
    {
        case BETON_SECTION_RECTANGULAIRE :
        {
            Beton_Section_Rectangulaire *section = barre->section;
            double      I = section->hauteur*section->largeur*section->largeur*section->largeur/12.;
            BUG(common_fonction_ajout(f_rotation, 0., a, fy*b/(6*E*I*ll)*a*(ll+b),     0.,          -fy*b/(2*E*I*ll),       0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_rotation, a,  ll, fy*a/(6*E*I*ll)*(2*ll*ll+a*a), -fy*a/(E*I), fy*a/(2*E*I*ll),      0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_rotation, 0., a, mz/(6*E*I*ll)*(-ll*ll+3*b*b),   0.,          mz/(2*E*I*ll),         0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_rotation, a,  ll, mz/(6*E*I*ll)*(2*ll*ll+3*a*a),  -mz/(E*I),  mz/(2*E*I*ll),         0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_rotation, 0., ll, -ll/(6*E*I)*(2*maz-mbz),      maz/(E*I),  -(maz+mbz)/(2*E*I*ll), 0., debut_barre) == 0, -3);
            
            BUG(common_fonction_ajout(f_deform, 0., a, 0.,                  fy*b/(6*E*I*ll)*(ll*ll-b*b),    0.,                  -fy*b/(6*E*I*ll),     debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_deform, a,  ll, -fy*a*a*a/(6*E*I),  fy*a/(6*E*I*ll)*(a*a+2*ll*ll),  -fy*a/(2*E*I),      fy*a/(6*E*I*ll),      debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_deform, 0., a, 0.,                  mz/(6*E*I*ll)*(-ll*ll+3*b*b),    0.,                  mz/(6*E*I*ll),       debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_deform, a,  ll, -mz/(6*E*I)*(3*a*a), mz/(6*E*I*ll)*(2*ll*ll+3*a*a), -mz/(6*E*I*ll)*(3*ll), mz/(6*E*I*ll),       debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_deform, 0., ll, 0.,                  -ll/(6*E*I)*(2*maz-mbz),       maz/(2*E*I),       -(mbz+maz)/(6*E*I*ll), debut_barre) == 0, -3);
            return 0;
            break;
        }
        case BETON_SECTION_T :
        {
            Beton_Section_T *section = barre->section;
            double      lt = section->largeur_table;
            double      la = section->largeur_ame;
            double      ht = section->hauteur_table;
            double      ha = section->hauteur_ame;
            double      I = ht*lt*lt*lt/12+ha*la*la*la/12;
            BUG(common_fonction_ajout(f_rotation, 0., a, fy*b/(6*E*I*ll)*a*(ll+b),     0.,          -fy*b/(2*E*I*ll),       0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_rotation, a,  ll, fy*a/(6*E*I*ll)*(2*ll*ll+a*a), -fy*a/(E*I), fy*a/(2*E*I*ll),      0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_rotation, 0., a, mz/(6*E*I*ll)*(-ll*ll+3*b*b),   0.,          mz/(2*E*I*ll),         0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_rotation, a,  ll, mz/(6*E*I*ll)*(2*ll*ll+3*a*a),  -mz/(E*I),  mz/(2*E*I*ll),         0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_rotation, 0., ll, -ll/(6*E*I)*(2*maz-mbz),      maz/(E*I),  -(maz+mbz)/(2*E*I*ll), 0., debut_barre) == 0, -3);
            
            BUG(common_fonction_ajout(f_deform, 0., a, 0.,                  fy*b/(6*E*I*ll)*(ll*ll-b*b),    0.,                  -fy*b/(6*E*I*ll),     debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_deform, a,  ll, -fy*a*a*a/(6*E*I),  fy*a/(6*E*I*ll)*(a*a+2*ll*ll),  -fy*a/(2*E*I),      fy*a/(6*E*I*ll),      debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_deform, 0., a, 0.,                  mz/(6*E*I*ll)*(-ll*ll+3*b*b),    0.,                  mz/(6*E*I*ll),       debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_deform, a,  ll, -mz/(6*E*I)*(3*a*a), mz/(6*E*I*ll)*(2*ll*ll+3*a*a), -mz/(6*E*I*ll)*(3*ll), mz/(6*E*I*ll),       debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_deform, 0., ll, 0.,                  -ll/(6*E*I)*(2*maz-mbz),       maz/(2*E*I),       -(mbz+maz)/(6*E*I*ll), debut_barre) == 0, -3);
            
            return 0;
            break;
        }
        case BETON_SECTION_CARRE :
        {
            Beton_Section_Carre *section = barre->section;
            double      I = section->cote*section->cote*section->cote*section->cote/12.;
            BUG(common_fonction_ajout(f_rotation, 0., a, fy*b/(6*E*I*ll)*a*(ll+b),     0.,          -fy*b/(2*E*I*ll),       0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_rotation, a,  ll, fy*a/(6*E*I*ll)*(2*ll*ll+a*a), -fy*a/(E*I), fy*a/(2*E*I*ll),      0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_rotation, 0., a, mz/(6*E*I*ll)*(-ll*ll+3*b*b),   0.,          mz/(2*E*I*ll),         0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_rotation, a,  ll, mz/(6*E*I*ll)*(2*ll*ll+3*a*a),  -mz/(E*I),  mz/(2*E*I*ll),         0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_rotation, 0., ll, -ll/(6*E*I)*(2*maz-mbz),      maz/(E*I),  -(maz+mbz)/(2*E*I*ll), 0., debut_barre) == 0, -3);
            
            BUG(common_fonction_ajout(f_deform, 0., a, 0.,                  fy*b/(6*E*I*ll)*(ll*ll-b*b),    0.,                  -fy*b/(6*E*I*ll),     debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_deform, a,  ll, -fy*a*a*a/(6*E*I),  fy*a/(6*E*I*ll)*(a*a+2*ll*ll),  -fy*a/(2*E*I),      fy*a/(6*E*I*ll),      debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_deform, 0., a, 0.,                  mz/(6*E*I*ll)*(-ll*ll+3*b*b),    0.,                  mz/(6*E*I*ll),       debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_deform, a,  ll, -mz/(6*E*I)*(3*a*a), mz/(6*E*I*ll)*(2*ll*ll+3*a*a), -mz/(6*E*I*ll)*(3*ll), mz/(6*E*I*ll),       debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_deform, 0., ll, 0.,                  -ll/(6*E*I)*(2*maz-mbz),       maz/(2*E*I),       -(mbz+maz)/(6*E*I*ll), debut_barre) == 0, -3);
            
            return 0;
            break;
        }
        case BETON_SECTION_CIRCULAIRE :
        {
            Beton_Section_Circulaire *section = barre->section;
            double      I = M_PI*section->diametre*section->diametre*section->diametre*section->diametre/64.;
            BUG(common_fonction_ajout(f_rotation, 0., a, fy*b/(6*E*I*ll)*a*(ll+b),     0.,          -fy*b/(2*E*I*ll),       0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_rotation, a,  ll, fy*a/(6*E*I*ll)*(2*ll*ll+a*a), -fy*a/(E*I), fy*a/(2*E*I*ll),      0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_rotation, 0., a, mz/(6*E*I*ll)*(-ll*ll+3*b*b),   0.,          mz/(2*E*I*ll),         0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_rotation, a,  ll, mz/(6*E*I*ll)*(2*ll*ll+3*a*a),  -mz/(E*I),  mz/(2*E*I*ll),         0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_rotation, 0., ll, -ll/(6*E*I)*(2*maz-mbz),      maz/(E*I),  -(maz+mbz)/(2*E*I*ll), 0., debut_barre) == 0, -3);
            
            BUG(common_fonction_ajout(f_deform, 0., a, 0.,                  fy*b/(6*E*I*ll)*(ll*ll-b*b),    0.,                  -fy*b/(6*E*I*ll),     debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_deform, a,  ll, -fy*a*a*a/(6*E*I),  fy*a/(6*E*I*ll)*(a*a+2*ll*ll),  -fy*a/(2*E*I),      fy*a/(6*E*I*ll),      debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_deform, 0., a, 0.,                  mz/(6*E*I*ll)*(-ll*ll+3*b*b),    0.,                  mz/(6*E*I*ll),       debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_deform, a,  ll, -mz/(6*E*I)*(3*a*a), mz/(6*E*I*ll)*(2*ll*ll+3*a*a), -mz/(6*E*I*ll)*(3*ll), mz/(6*E*I*ll),       debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_deform, 0., ll, 0.,                  -ll/(6*E*I)*(2*maz-mbz),       maz/(2*E*I),       -(mbz+maz)/(6*E*I*ll), debut_barre) == 0, -3);
            
            return 0;
            break;
        }
        default :
        {
            BUGMSG(0, 0, "EF_charge_barre_ponctuelle_ajout_fonc_rz\n");
            break;
        }
    }
}


int EF_charge_barre_ponctuelle_n(Fonction *fonction, Beton_Barre *barre,
  unsigned int discretisation, double a, double fax, double fbx)
/* Description : Calcule les déplacements d'une barre selon l'axe x en fonction de l'effort
 *                 normal ponctuel n et des réactions d'appuis fax et fbx.
 * Paramètres : Fonction *fonction : fonction où sera ajoutée la déformée,
 *            : Beton_Barre *barre : Barre à étudier,
 *            : unsigned int discretisation : partie de la barre à étudier,
 *            : double a : position de la charge par rapport au début de la partie de barre,
 *            : double n : effort normal de la charge ponctuelle,
 *            : double fax : effort normal au début de la barre,
 *            : double fbx : effort normal à la fin de la barre.
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
    
    // La déformation selon l'axe x est par la formule :\end{verbatim}\begin{displaymath}
    // f_x(x) = F_{Ax} \cdot \int_0^x \frac{1}{E \cdot S(x)} dx \textrm{ pour x variant de 0 à a} \end{displaymath}\begin{displaymath}
    // f_x(x) = F_{Ax} \cdot \int_0^a \frac{1}{E \cdot S(x)} dx - F_{Bx} \cdot \int_a^x \frac{1}{E \cdot S(x)} dx \textrm{ pour x variant de a à l} \end{displaymath}\begin{verbatim}
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
        {
            Beton_Section_Rectangulaire *section = barre->section;
            double      S = section->hauteur*section->largeur;
            
    // Pour une section constante, les déformations valent :\end{verbatim}\begin{displaymath}
    // f_x(x) = \frac{F_{Ax} \cdot x}{E \cdot S} \textrm{ pour x variant de 0 à a}\end{displaymath}\begin{displaymath}
    // f_x(x) = \frac{a \cdot (F_{Ax} + F_{Bx}) - F_{Bx} \cdot x}{E \cdot S} \textrm{ pour x variant de a à l}\end{displaymath}\begin{verbatim}
            BUG(common_fonction_ajout(fonction, 0., a, 0.,                fax/(E*S),  0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(fonction, a,  ll, a*(fax+fbx)/(E*S), -fbx/(E*S), 0., 0., debut_barre) == 0, -3);
    //     pour une section rectangulaire :\end{verbatim}\begin{displaymath}
    //     S= b \cdot h \end{displaymath}\begin{verbatim}
            return 0;
            break;
        }
        case BETON_SECTION_T :
        {
            Beton_Section_T *section = barre->section;
            double      lt = section->largeur_table;
            double      la = section->largeur_ame;
            double      ht = section->hauteur_table;
            double      ha = section->hauteur_ame;
            double      S = ht*lt+ha*la;
            
            BUG(common_fonction_ajout(fonction, 0., a, 0.,                fax/(E*S),  0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(fonction, a,  ll, a*(fax+fbx)/(E*S), -fbx/(E*S), 0., 0., debut_barre) == 0, -3);
    //     pour une section en T (lt : largeur de la table, la : largeur de l'âme, ht : hauteur
    //       de la table, ha : hauteur de l'âme), S vaut :\end{verbatim}\begin{displaymath}
    //     S = h_t \cdot l_t+h_a \cdot l_a\end{displaymath}\begin{verbatim}
            return E*S/ll;
            break;
        }
        case BETON_SECTION_CARRE :
        {
            Beton_Section_Carre *section = barre->section;
            double      S = section->cote*section->cote;
            
            BUG(common_fonction_ajout(fonction, 0., a, 0.,                fax/(E*S),  0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(fonction, a,  ll, a*(fax+fbx)/(E*S), -fbx/(E*S), 0., 0., debut_barre) == 0, -3);
    //     pour une section carrée, S vaut :\end{verbatim}\begin{displaymath}
    //     S = cote^2\end{displaymath}\begin{verbatim}
            return E*S/ll;
            break;
        }
        case BETON_SECTION_CIRCULAIRE :
        {
            Beton_Section_Circulaire *section = barre->section;
            double      S = M_PI*section->diametre*section->diametre/4.;
            BUG(common_fonction_ajout(fonction, 0., a, 0.,                fax/(E*S),  0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(fonction, a,  ll, a*(fax+fbx)/(E*S), -fbx/(E*S), 0., 0., debut_barre) == 0, -3);
    //     pour une section circulaire, S vaut :\end{verbatim}\begin{displaymath}
    //     S = \frac{\pi \cdot \phi^2}{4} \end{displaymath}\begin{verbatim}
            return E*S/ll;
            break;
        }
        default :
        {
            BUGMSG(0, 0., "EF_charge_barre_ponctuelle_n\n");
            break;
        }
    }
}
