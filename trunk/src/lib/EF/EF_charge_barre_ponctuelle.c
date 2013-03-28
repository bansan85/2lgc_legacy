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

#include "1990_action.h"
#include "common_projet.h"
#include "common_erreurs.h"
#include "common_math.h"
#include "common_fonction.h"
#include "EF_noeuds.h"
#include "EF_calculs.h"
#include "EF_sections.h"
#include "EF_gtk_charge_barre_ponctuelle.h"

Charge_Barre_Ponctuelle *EF_charge_barre_ponctuelle_ajout(Projet *projet,
  unsigned int num_action, GList *barres, gboolean repere_local, double a, double fx,
  double fy, double fz, double mx, double my, double mz, const char* nom)
/* Description : Ajoute une charge ponctuelle à une action et à l'intérieur d'une barre en lui
 *               attribuant le numéro suivant la dernière charge de l'action.
 * Paramètres : Projet *projet : la variable projet,
 *            : int num_action : numero de l'action qui contiendra la charge,
 *            : GList *barres : liste des barres qui supportera la charge,
 *            : int repere_local : TRUE si les charges doivent être prise dans le repère local,
 *                                 FALSE pour le repère global,
 *            : double a : position en mètre de la charge par rapport au début de la barre,
 *            : double fx : force suivant l'axe x,
 *            : double fy : force suivant l'axe y,
 *            : double fz : force suivant l'axe z,
 *            : double mx : moment autour de l'axe x,
 *            : double my : moment autour de l'axe y,
 *            : double mz : moment autour de l'axe z,
 *            : const char* nom : nom de la charge.
 * Valeur renvoyée :
 *   Succès : pointeur vers la nouvelle charge
 *   Échec : NULL :
 *             projet == NULL,
 *             action introuvable,
 *             barre == NULL,
 *             _1990_action_cherche_numero(projet, num_action) == NULL,
 *             a < 0 ou a > l,
 *             en cas d'erreur d'allocation mémoire
 */
{
    Action                  *action_en_cours;
    Charge_Barre_Ponctuelle *charge_nouveau;
#ifdef ENABLE_GTK
    GtkTreeIter             iter_action;
    unsigned int            numero_action;
    GtkTreeModel            *model_action;
#endif
    
    // Trivial
    BUGMSG(projet, NULL, gettext("Paramètre %s incorrect.\n"), "projet");
    BUG(action_en_cours = _1990_action_cherche_numero(projet, num_action), NULL);
    BUGMSG(!((a < 0.) && (!(ERREUR_RELATIVE_EGALE(a, 0.)))), NULL, gettext("La position de la charge ponctuelle (%f) est incorrecte.\n"), a);
    BUGMSG(charge_nouveau = malloc(sizeof(Charge_Barre_Ponctuelle)), NULL, gettext("Erreur d'allocation mémoire.\n"));
    if (barres != NULL)
    {
        GList   *list_parcours = barres;
        do
        {
            Beton_Barre *barre = list_parcours->data;
            
            double distance = EF_noeuds_distance(barre->noeud_debut, barre->noeud_fin);
            BUGMSG(!((a > distance) && (!(ERREUR_RELATIVE_EGALE(a, distance)))), NULL, gettext("La position de la charge ponctuelle (%f) est incorrecte. La longueur de la barre %d est de %f m.\n"), a, barre->numero, distance);
            
            list_parcours = g_list_next(list_parcours);
        }
        while (list_parcours != NULL);
    }
    
    charge_nouveau->type = CHARGE_BARRE_PONCTUELLE;
    BUGMSG(charge_nouveau->nom = g_strdup_printf("%s", nom), NULL, gettext("Erreur d'allocation mémoire.\n"));
    charge_nouveau->barres = barres;
    charge_nouveau->repere_local = repere_local;
    charge_nouveau->position = a;
    charge_nouveau->fx = fx;
    charge_nouveau->fy = fy;
    charge_nouveau->fz = fz;
    charge_nouveau->mx = mx;
    charge_nouveau->my = my;
    charge_nouveau->mz = mz;
    
    charge_nouveau->numero = g_list_length(action_en_cours->charges);
    
    action_en_cours->charges = g_list_append(action_en_cours->charges, charge_nouveau);
    
    BUG(EF_calculs_free(projet), FALSE);
    
#ifdef ENABLE_GTK
    if ((projet->list_gtk._1990_actions.builder != NULL) && (gtk_tree_selection_get_selected(projet->list_gtk._1990_actions.tree_select_actions, &model_action, &iter_action)))
    {
        gtk_tree_model_get(model_action, &iter_action, 0, &numero_action, -1);
        if (numero_action == num_action)
            BUG(EF_gtk_charge_barre_ponctuelle_ajout_affichage(charge_nouveau, projet, TRUE), NULL);
    }
#endif
    
    return charge_nouveau;
}


gboolean EF_charge_barre_ponctuelle_mx(Beton_Barre *barre, unsigned int discretisation,
  double a, Barre_Info_EF *infos, double mx, double *ma, double *mb)
/* Description : Calcule l'opposé aux moments d'encastrement pour l'élément spécifié soumis
 *               au moment de torsion mx dans le repère local. Les résultats sont renvoyés
 *               par l'intermédiaire des pointeurs ma et mb qui ne peuvent être NULL.
 * Paramètres : Beton_Barre *barre : Barre à étudier,
 *            : unsigned int discretisation : partie de la barre à étudier,
 *            : double a : position de la charge par rapport au début de la partie de barre à
 *                         étudier,
 *            : Barre_Info_EF *infos : infos de la partie de barre concerné,
 *            : double mx : moment autour de l'axe x,
 *            : double *ma : pointeur qui contiendra le moment au début de la barre,
 *            : double *mb : pointeur qui contiendra le moment à la fin de la barre.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             barre == NULL,
 *             infos == NULL,
 *             barre->section == NULL,
 *             barre->materiau == NULL,
 *             barre->noeud_debut == NULL,
 *             barre->noeud_fin == NULL,
 *             discretisation>barre->discretisation_element,
 *             ma == NULL,
 *             mb == NULL,
 *             kAx == kBx == MAXDOUBLE,
 *             a < 0. ou a > l
 */
{
    EF_Noeud    *debut, *fin;
    double      l, G;

    BUGMSG(barre, FALSE, gettext("Paramètre %s incorrect.\n"), "barre");
    BUGMSG(infos, FALSE, gettext("Paramètre %s incorrect.\n"), "infos");
    BUGMSG(barre->section, FALSE, gettext("Section indéfinie.\n"));
    BUGMSG(barre->materiau, FALSE, gettext("Matériau indéfini.\n"));
    BUGMSG(barre->noeud_debut, FALSE, gettext("Noeud 1 indéfini.\n"));
    BUGMSG(barre->noeud_fin, FALSE, gettext("Noeud 2 indéfini.\n"));
    BUGMSG(discretisation<=barre->discretisation_element, FALSE, gettext("La discrétisation %d souhaitée est hors domaine %d.\n"), discretisation, barre->discretisation_element);
    BUGMSG(ma, FALSE, gettext("Paramètre %s incorrect.\n"), "ma");
    BUGMSG(mb, FALSE, gettext("Paramètre %s incorrect.\n"), "mb");
    BUGMSG(!((ERREUR_RELATIVE_EGALE(infos->kAx, MAXDOUBLE)) && (ERREUR_RELATIVE_EGALE(infos->kBx, MAXDOUBLE))), FALSE, gettext("Impossible de relâcher rx simultanément des deux cotés de la barre.\n"));
    BUGMSG(!((a < 0.) && (!(ERREUR_RELATIVE_EGALE(a, 0.)))), FALSE, gettext("La position de la charge ponctuelle (%f) est incorrecte.\n"), a);

    // Les moments aux extrémités de la barre sont déterminés par les intégrales de Mohr
    //   et valent dans le cas général :\end{verbatim}\begin{center}
    // \includegraphics[width=8cm]{images/charge_barre_ponctuelle_mx.pdf}\end{center}
    // \begin{align*}
    // M_{Bx} = & \frac{\int_0^a \frac{1}{J(x) \cdot G}dx + k_A}{\int_0^L \frac{1}{J(x) \cdot G} dx + k_A + k_B} \cdot M_x\nonumber\\
    // M_{Ax} = & M_x - M_{Bx}\end{align*}\begin{verbatim}
    
    if (discretisation == 0)
        debut = barre->noeud_debut;
    else
        debut = g_list_nth_data(barre->noeuds_intermediaires, discretisation-1);
    if (discretisation == barre->discretisation_element)
        fin = barre->noeud_fin;
    else
        fin = g_list_nth_data(barre->noeuds_intermediaires, discretisation);
    
    l = EF_noeuds_distance(debut, fin);
    BUG(!isnan(l), FALSE);
    BUGMSG(!((a > l) && (!(ERREUR_RELATIVE_EGALE(a, l)))), FALSE, gettext("La position de la charge ponctuelle (%f) est incorrecte. La longueur de la barre est de %f m.\n"), a, l);
    
    G = EF_calculs_G(barre->materiau, FALSE);
    
    switch (barre->section->type)
    {
        case SECTION_RECTANGULAIRE :
        case SECTION_T :
        case SECTION_CARREE :
        case SECTION_CIRCULAIRE :
        {
            double      J = EF_sections_j(barre->section);
            
            BUG(!isnan(J), FALSE);
            
    // Pour une section section constante, les moments valent :\end{verbatim}\begin{displaymath}
    // M_{Bx} = \frac{\frac{a}{G \cdot J} +k_{Ax}}{\frac{L}{G \cdot J} +k_{Ax}+k_{Bx}} \cdot M_x\end{displaymath}\begin{verbatim}
            if (ERREUR_RELATIVE_EGALE(infos->kAx, MAXDOUBLE))
                *mb = mx;
            else if (ERREUR_RELATIVE_EGALE(infos->kBx, MAXDOUBLE))
                *mb = 0.;
            else
                *mb = (a/(G*J)+infos->kAx)/(l/(G*J)+infos->kAx+infos->kBx)*mx;
            *ma = mx - *mb;
            return TRUE;
            break;
        }
        default :
        {
            BUGMSG(0, FALSE, gettext("Type de section %d inconnu.\n"), barre->section->type);
            break;
        }
    }
}


gboolean EF_charge_barre_ponctuelle_def_ang_iso_y(Beton_Barre *barre,
  unsigned int discretisation, double a, double fz, double my, double *phia, double *phib)
/* Description : Calcule les angles de rotation autour de l'axe y pour un élément bi-articulé
 *               soumis au chargement fz, my dans le repère local. Les résultats sont renvoyés
 *               par l'intermédiaire des pointeurs phia et phib qui ne peuvent être NULL.
 * Paramètres : Beton_Barre *barre : Barre à étudier,
 *            : unsigned int discretisation : partie de la barre à étudier,
 *            : double a : position de la charge par rapport au début de la partie de
 *                         barre à étudier,
 *            : double fz : force suivant l'axe z,
 *            : double my : moment autour de l'axe y,
 *            : double *phia : pointeur qui contiendra l'angle au début de la barre,
 *            : double *phib : pointeur qui contiendra l'angle à la fin de la barre.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             barre == NULL,
 *             barre->section == NULL,
 *             barre->materiau == NULL,
 *             barre->noeud_debut == NULL,
 *             barre->noeud_fin == NULL,
 *             discretisation>barre->discretisation_element,
 *             phia == NULL,
 *             phib == NULL,
 *             a < 0. ou a > l.
 */
{
    EF_Noeud    *debut, *fin;
    double      l, b, E;

    BUGMSG(barre, FALSE, gettext("Paramètre %s incorrect.\n"), "barre");
    BUGMSG(barre->section, FALSE, gettext("Section indéfinie.\n"));
    BUGMSG(barre->materiau, FALSE, gettext("Matériau indéfini.\n"));
    BUGMSG(barre->noeud_debut, FALSE, gettext("Noeud 1 indéfini.\n"));
    BUGMSG(barre->noeud_fin, FALSE, gettext("Noeud 2 indéfini.\n"));
    BUGMSG(discretisation<=barre->discretisation_element, FALSE, gettext("La discrétisation %d souhaitée est hors domaine %d.\n"), discretisation, barre->discretisation_element);
    BUGMSG(phia, FALSE, gettext("Paramètre %s incorrect.\n"), "phia");
    BUGMSG(phib, FALSE, gettext("Paramètre %s incorrect.\n"), "phib");
    BUGMSG(!((a < 0.) && (!(ERREUR_RELATIVE_EGALE(a, 0.)))), FALSE, gettext("La position de la charge ponctuelle (%f) est incorrecte.\n"), a);
    
    // Les rotations aux extrémités de la barre sont déterminées par les intégrales de Mohr
    //   et valent dans le cas général :\end{verbatim}\begin{center}
    // \includegraphics[width=8cm]{images/charge_barre_ponctuelle_fz.pdf}\includegraphics[width=8cm]{images/charge_barre_ponctuelle_my.pdf}\par
    // \includegraphics[width=8cm]{images/charge_barre_ponctuelle_phiay.pdf}\includegraphics[width=8cm]{images/charge_barre_ponctuelle_phiby.pdf}\end{center}\begin{align*}
    // \varphi_A = & \int_0^a \frac{Mf_{11} \cdot Mf_0}{E \cdot I_y(x)} + \int_a^L \frac{Mf_{12} \cdot Mf_0}{E \cdot I_y(x)} + \int_0^a \frac{Mf_{21} \cdot Mf_0}{E \cdot I_y(x)} + \int_a^L \frac{Mf_{22} \cdot Mf_0}{E \cdot I_y(x)} \nonumber\\
    // \varphi_B = & \int_0^a \frac{Mf_{11} \cdot Mf_3}{E \cdot I_y(x)} + \int_a^L \frac{Mf_{12} \cdot Mf_3}{E \cdot I_y(x)} + \int_0^a \frac{Mf_{21} \cdot Mf_3}{E \cdot I_y(x)} + \int_a^L \frac{Mf_{22} \cdot Mf_3}{E \cdot I_y(x)}\end{align*}\begin{align*}
    // \texttt{avec :} Mf_0 = &\frac{L-x}{L} & Mf_{11} = &-\frac{F_z \cdot b \cdot x}{L}\nonumber\\
    //                 Mf_{12} = & -\frac{F_z \cdot a \cdot (L-x)}{L} & Mf_{21} = & -\frac{M_y \cdot x}{L}\nonumber\\
    //                 Mf_{22} = & \frac{M_y \cdot (L-x)}{L} & Mf_3 = & -\frac{x}{L}\end{align*}\begin{verbatim}
    
    if (discretisation == 0)
        debut = barre->noeud_debut;
    else
        debut = g_list_nth_data(barre->noeuds_intermediaires, discretisation-1);
    if (discretisation == barre->discretisation_element)
        fin = barre->noeud_fin;
    else
        fin = g_list_nth_data(barre->noeuds_intermediaires, discretisation);
    
    l = EF_noeuds_distance(debut, fin);
    BUG(!isnan(l), FALSE);
    BUGMSG(!((a > l) && (!(ERREUR_RELATIVE_EGALE(a, l)))), FALSE, gettext("La position de la charge ponctuelle (%f) est incorrecte. La longueur de la barre est de %f m.\n"), a, l);
    b = l-a;
    
    E = EF_calculs_E(barre->materiau);
    
    switch (barre->section->type)
    {
        case SECTION_RECTANGULAIRE :
        case SECTION_T :
        case SECTION_CARREE :
        case SECTION_CIRCULAIRE :
        {
            double      I = EF_sections_iy(barre->section);
            
            BUG(!isnan(l), FALSE);
    // Pour une section constante, les angles valent :\end{verbatim}\begin{align*}
    // \varphi_A = &-\frac{F_z \cdot a}{6 \cdot E \cdot I_y \cdot L} \cdot b \cdot (2 \cdot L-a)-\frac{M_y}{6 \cdot E \cdot I_y \cdot L} \cdot (L^2-3 \cdot b^2)\nonumber\\
    // \varphi_B = &\frac{F_z \cdot a}{6 \cdot E \cdot I_y \cdot L} \cdot (L^2-a^2)-\frac{M_y}{6 \cdot E \cdot I_y \cdot L} \cdot (L^2-3 \cdot a^2)\end{align*}\begin{verbatim}
            *phia = -fz*a/(6*E*I*l)*b*(2*l-a)-my/(6*E*I*l)*(l*l-3*b*b);
            *phib = fz*a/(6*E*I*l)*(l*l-a*a)-my/(6*E*I*l)*(l*l-3*a*a);
            return TRUE;
            break;
        }
        default :
        {
            BUGMSG(0, FALSE, gettext("Type de section %d inconnu.\n"), barre->section->type);
            break;
        }
    }
}


gboolean EF_charge_barre_ponctuelle_def_ang_iso_z(Beton_Barre *barre,
  unsigned int discretisation, double a, double fy, double mz, double *phia, double *phib)
/* Description : Calcule les angles de rotation autour de l'axe z pour un élément bi-articulé
 *               soumis au chargement fy, mz dans le repère local. Les résultats sont renvoyés
 *               par l'intermédiaire des pointeurs phia et phib qui ne peuvent être NULL.
 * Paramètres : Beton_Barre *barre : Barre à étudier,
 *            : unsigned int discretisation : partie de la barre à étudier,
 *            : double a : position de la charge par rapport au début de la partie de barre
 *                         à étudier,
 *            : double fy : force suivant l'axe y,
 *            : double mz : moment autour de l'axe z,
 *            : double *phia : pointeur qui contiendra l'angle au début de la barre,
 *            : double *phib : pointeur qui contiendra l'angle à la fin de la barre.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             barre == NULL,
 *             barre->section == NULL,
 *             barre->materiau == NULL,
 *             barre->noeud_debut == NULL,
 *             barre->noeud_fin == NULL,
 *             discretisation>barre->discretisation_element,
 *             phia == NULL,
 *             phib == NULL,
 *             a < 0. ou a > l.
 */
{
    EF_Noeud    *debut, *fin;
    double      l, b, E;
    
    BUGMSG(barre, FALSE, gettext("Paramètre %s incorrect.\n"), "barre");
    BUGMSG(barre->section, FALSE, gettext("Section indéfinie.\n"));
    BUGMSG(barre->materiau, FALSE, gettext("Matériau indéfini.\n"));
    BUGMSG(barre->noeud_debut, FALSE, gettext("Noeud 1 indéfini.\n"));
    BUGMSG(barre->noeud_fin, FALSE, gettext("Noeud 2 indéfini.\n"));
    BUGMSG(discretisation<=barre->discretisation_element, FALSE, gettext("La discrétisation %d souhaitée est hors domaine %d.\n"), discretisation, barre->discretisation_element);
    BUGMSG(phia, FALSE, gettext("Paramètre %s incorrect.\n"), "phia");
    BUGMSG(phib, FALSE, gettext("Paramètre %s incorrect.\n"), "phib");
    BUGMSG(!((a < 0.) && (!(ERREUR_RELATIVE_EGALE(a, 0.)))), FALSE, gettext("La position de la charge ponctuelle (%f) est incorrecte.\n"), a);
    
    // Les rotations aux extrémités de la barre sont déterminées par les intégrales de Mohr
    //   et valent dans le cas général :\end{verbatim}\begin{center}
    // \includegraphics[width=8cm]{images/charge_barre_ponctuelle_fy.pdf}\includegraphics[width=8cm]{images/charge_barre_ponctuelle_mz.pdf}\par
    // \includegraphics[width=8cm]{images/charge_barre_ponctuelle_phiaz.pdf}\includegraphics[width=8cm]{images/charge_barre_ponctuelle_phibz.pdf}\end{center}\begin{align*}
    // \varphi_A = & \int_0^a \frac{Mf_{11} \cdot Mf_0}{E \cdot I_z(x)} + \int_a^L \frac{Mf_{12} \cdot Mf_0}{E \cdot I_z(x)} + \int_0^a \frac{Mf_{21} \cdot Mf_0}{E \cdot I_z(x)} + \int_a^L \frac{Mf_{22} \cdot Mf_0}{E \cdot I_z(x)} \nonumber\\
    // \varphi_B = & \int_0^a \frac{Mf_{11} \cdot Mf_3}{E \cdot I_z(x)} + \int_a^L \frac{Mf_{12} \cdot Mf_3}{E \cdot I_z(x)} + \int_0^a \frac{Mf_{21} \cdot Mf_3}{E \cdot I_z(x)} + \int_a^L \frac{Mf_{22} \cdot Mf_3}{E \cdot I_z(x)}\end{align*}\begin{align*}
    // \texttt{avec :} Mf_0 = &\frac{L-x}{L} & Mf_{11} = &\frac{F_y \cdot b \cdot x}{L}\nonumber\\
    //                 Mf_{12} = & \frac{F_y \cdot a \cdot (L-x)}{L} & Mf_{21} = & -\frac{M_z \cdot x}{L}\nonumber\\
    //                 Mf_{22} = & \frac{M_z \cdot (L-x)}{L} & Mf_3 = & -\frac{x}{L}\end{align*}\begin{verbatim}
    
    if (discretisation == 0)
        debut = barre->noeud_debut;
    else
        debut = g_list_nth_data(barre->noeuds_intermediaires, discretisation-1);
    if (discretisation == barre->discretisation_element)
        fin = barre->noeud_fin;
    else
        fin = g_list_nth_data(barre->noeuds_intermediaires, discretisation);
    
    l = EF_noeuds_distance(debut, fin);
    BUG(!isnan(l), FALSE);
    BUGMSG(!((a > l) && (!(ERREUR_RELATIVE_EGALE(a, l)))), FALSE, gettext("La position de la charge ponctuelle (%f) est incorrecte. La longueur de la barre est de %f m.\n"), a, l);
    b = l-a;
    
    E = EF_calculs_E(barre->materiau);
    
    switch (barre->section->type)
    {
        case SECTION_RECTANGULAIRE :
        case SECTION_T :
        case SECTION_CARREE :
        case SECTION_CIRCULAIRE :
        {
            double      I = EF_sections_iz(barre->section);
            
            BUG(!isnan(I), FALSE);
            
    // Pour une section constante, les angles valent :\end{verbatim}\begin{displaymath}
    // \varphi_A = \frac{ F_y \cdot a}{6 \cdot E \cdot I_z \cdot L} b \cdot (2 \cdot L-a) - \frac{M_z}{6 \cdot E \cdot I_z \cdot L} \left(L^2-3 \cdot b^2 \right)\end{displaymath}\begin{displaymath}
    // \varphi_B = \frac{-F_y \cdot a}{6 \cdot E \cdot I_z \cdot L} (L^2-a^2) - \frac{M_z}{6 \cdot E \cdot I_z \cdot L} \left(L^2-3 \cdot a^2 \right)\end{displaymath}\begin{verbatim}
            *phia = fy*a/(6*E*I*l)*b*(2*l-a)-mz/(6*E*I*l)*(l*l-3*b*b);
            *phib = -fy*a/(6*E*I*l)*(l*l-a*a)-mz/(6*E*I*l)*(l*l-3*a*a);
            return TRUE;
            break;
        }
        default :
        {
            BUGMSG(0, FALSE, gettext("Type de section %d inconnu.\n"), barre->section->type);
            break;
        }
    }
}


gboolean EF_charge_barre_ponctuelle_fonc_rx(Fonction *fonction, Beton_Barre *barre,
  unsigned int discretisation, double a, double max, double mbx)
/* Description : Calcule les déplacements d'une barre en rotation autour de l'axe x en fonction
 *                  des efforts aux extrémités de la poutre soumise à un moment de torsion
 *                  ponctuel à la position a.
 * Paramètres : Fonction *fonction : fonction où sera ajoutée la déformée,
 *            : Beton_Barre *barre : Barre à étudier,
 *            : unsigned int discretisation : partie de la barre à étudier,
 *            : double a : position du moment ponctuel autour de l'axe x par rapport au début
 *                         de la partie de barre à étudier,
 *            : double max : moment au début de la barre,
 *            : double mbx : moment à la fin de la barre.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             fonction == NULL,
 *             barre == NULL,
 *             barre->section == NULL,
 *             barre->materiau == NULL,
 *             barre->noeud_debut == NULL,
 *             barre->noeud_fin == NULL,
 *             discretisation>barre->discretisation_element,
 *             a < 0. ou a > l,
 *             en cas d'erreur due à une fonction interne.
 */
{
    EF_Noeud    *debut, *fin;
    Barre_Info_EF *infos;
    double      l;
    double      G, debut_barre;
    
    BUGMSG(fonction, FALSE, gettext("Paramètre %s incorrect.\n"), "fonction");
    BUGMSG(barre, FALSE, gettext("Paramètre %s incorrect.\n"), "barre");
    BUGMSG(barre->section, FALSE, gettext("Section indéfinie.\n"));
    BUGMSG(barre->materiau, FALSE, gettext("Matériau indéfini.\n"));
    BUGMSG(barre->noeud_debut, FALSE, gettext("Noeud 1 indéfini.\n"));
    BUGMSG(barre->noeud_fin, FALSE, gettext("Noeud 2 indéfini.\n"));
    BUGMSG(discretisation<=barre->discretisation_element, FALSE, gettext("La discrétisation %d souhaitée est hors domaine %d.\n"), discretisation, barre->discretisation_element);
    infos = &(barre->info_EF[discretisation]);
    BUGMSG(!((ERREUR_RELATIVE_EGALE(infos->kAx, MAXDOUBLE)) && (ERREUR_RELATIVE_EGALE(infos->kBx, MAXDOUBLE))), FALSE, gettext("Impossible de relâcher rx simultanément des deux cotés de la barre.\n")); 
    BUGMSG(!((a < 0.) && (!(ERREUR_RELATIVE_EGALE(a, 0.)))), FALSE, gettext("La position de la charge ponctuelle (%f) est incorrecte.\n"), a);
    
    // La déformation d'une barre soumise à un effort de torsion est défini par les formules :\end{verbatim}\begin{center}
    // \includegraphics[width=8cm]{images/charge_barre_ponctuelle_mx.pdf}\end{center}\begin{verbatim}
    // Si le noeud B est relaché en rotation Alors\end{verbatim}\begin{align*}
    // r_x(x) = & M_{Ax} \cdot \left( k_{Ax} + \int_0^x \frac{1}{J \cdot G} dX \right) & & \textrm{ pour x variant de 0 à a}\nonumber\\
    // r_x(x) = & M_{Ax} \cdot k_{Ax} + \int_0^a \frac{M_{Ax}}{J \cdot G} dX - \int_a^x \frac{M_{Bx}}{J \cdot G} dX & & \textrm{ pour x variant de a à L}\end{align*}\begin{verbatim}
    // Sinon \end{verbatim}\begin{align*}
    // r_x(x) = & M_{Bx} \cdot k_{Bx} + \int_a^L \frac{M_{Bx}}{J \cdot G} dX - \int_x^a \frac{M_{Ax}}{J \cdot G} dX & & \textrm{ pour x variant de 0 à a}\nonumber\\
    // r_x(x) = & M_{Bx} \cdot \left( k_{Bx} + \int_x^L \frac{1}{J \cdot G} dX \right) & & \textrm{ pour x variant de a à L}\end{align*}\begin{verbatim}
    // FinSi
    
    if (discretisation == 0)
        debut = barre->noeud_debut;
    else
        debut = g_list_nth_data(barre->noeuds_intermediaires, discretisation-1);
    if (discretisation == barre->discretisation_element)
        fin = barre->noeud_fin;
    else
        fin = g_list_nth_data(barre->noeuds_intermediaires, discretisation);
    
    debut_barre = EF_noeuds_distance(barre->noeud_debut, debut);
    l = EF_noeuds_distance(debut, fin);
    BUG(!isnan(l), FALSE);
    BUGMSG(!((a > l) && (!(ERREUR_RELATIVE_EGALE(a, l)))), FALSE, gettext("La position de la charge ponctuelle (%f) est incorrecte. La longueur de la barre est de %f m.\n"), a, l);
    
    G = EF_calculs_G(barre->materiau, FALSE);
    
    switch (barre->section->type)
    {
        case SECTION_RECTANGULAIRE :
        case SECTION_T :
        case SECTION_CARREE :
        case SECTION_CIRCULAIRE :
        {
            double      J = EF_sections_j(barre->section);
            
            BUG(!isnan(J), FALSE);
            
            if (ERREUR_RELATIVE_EGALE(infos->kBx, MAXDOUBLE))
            {
                BUG(common_fonction_ajout_poly(fonction, 0., a, max*infos->kAx, max/(G*J), 0., 0., 0., 0., 0., debut_barre), FALSE);
                BUG(common_fonction_ajout_poly(fonction, a, l, max*infos->kAx+a*(max+mbx)/(G*J), -mbx/(G*J), 0., 0., 0., 0., 0., debut_barre), FALSE);
            }
            else
            {
                BUG(common_fonction_ajout_poly(fonction, 0., a, mbx*infos->kBx - (a*(max+mbx)-l*mbx)/(G*J), +max/(G*J), 0., 0., 0., 0., 0., debut_barre), FALSE);
                BUG(common_fonction_ajout_poly(fonction, a, l, mbx*(infos->kBx + l/(G*J)), -mbx/(G*J), 0., 0., 0., 0., 0., debut_barre), FALSE);
            }
            
    // Pour une section section constante, les moments valent :
    // Si le noeud B est relaché en rotation Alors\end{verbatim}\begin{align*}
    // r_x(x) = & M_{Ax} \cdot \left( k_{Ax} + \frac{x}{J \cdot G} \right) & & \textrm{ pour x variant de 0 à a}\nonumber\\
    // r_x(x) = & M_{Ax} \cdot k_{Ax} + \frac{a \cdot (M_{Ax}+M_{Bx})-M_{Bx} \cdot x}{J \cdot G} & & \textrm{ pour x variant de a à L}\end{align*}\begin{verbatim}
    // Sinon \end{verbatim}\begin{align*}
    // r_x(x) = & M_{Bx} \cdot k_{Bx} - \frac{a \cdot (M_{Ax}+M_{Bx})-l \cdot M_{Bx} - M_{Ax} \cdot x}{J \cdot G} & & \textrm{ pour x variant de 0 à a}\nonumber\\
    // r_x(x) = & M_{Bx} \cdot \left( k_{Bx} + \frac{L-x}{J \cdot G} \right) & & \textrm{ pour x variant de a à L}\end{align*}\begin{verbatim}
    // FinSi
            return TRUE;
            break;
        }
        default :
        {
            BUGMSG(0, FALSE, gettext("Type de section %d inconnu.\n"), barre->section->type);
            break;
        }
    }
}


gboolean EF_charge_barre_ponctuelle_fonc_ry(Fonction *f_rotation, Fonction* f_deform,
  Beton_Barre *barre, unsigned int discretisation, double a, double fz, double my, double may,
  double mby)
/* Description : Calcule les déplacements d'une barre en rotation autour de l'axe y et en
 *                 déformation selon l'axe z en fonction de la charge ponctuelle (fz et my) et
 *                 des efforts aux extrémités de la poutre.
 * Paramètres : Fonction *f_rotation : fonction où sera ajoutée la rotation,
 *            : Fonction* f_deform : fonction où sera ajoutée la déformation,
 *            : Beton_Barre *barre : Barre à étudier,
 *            : unsigned int discretisation : partie de la barre à étudier,
 *            : double a : position de la charge par rapport au début de la partie de barre à
 *                         étudier,
 *            : double fz : force suivant l'axe z,
 *            : double my : moment autour de l'axe y,
 *            : double may : moment au début de la barre,
 *            : double mby : moment à la fin de la barre.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             f_rotation == NULL,
 *             f_deform == NULL,
 *             barre == NULL,
 *             barre->section == NULL,
 *             barre->materiau == NULL,
 *             barre->noeud_debut == NULL,
 *             barre->noeud_fin == NULL,
 *             discretisation>barre->discretisation_element,
 *             a < 0. ou a > l,
 *             en cas d'erreur due à une fonction interne.
 */
{
    EF_Noeud    *debut, *fin;
    double      l, b;
    double      E, debut_barre;
    
    BUGMSG(f_rotation, FALSE, gettext("Paramètre %s incorrect.\n"), "f_rotation");
    BUGMSG(f_deform, FALSE, gettext("Paramètre %s incorrect.\n"), "f_deform");
    BUGMSG(barre, FALSE, gettext("Paramètre %s incorrect.\n"), "barre");
    BUGMSG(barre->section, FALSE, gettext("Section indéfinie.\n"));
    BUGMSG(barre->materiau, FALSE, gettext("Matériau indéfini.\n"));
    BUGMSG(barre->noeud_debut, FALSE, gettext("Noeud 1 indéfini.\n"));
    BUGMSG(barre->noeud_fin, FALSE, gettext("Noeud 2 indéfini.\n"));
    BUGMSG(discretisation<=barre->discretisation_element, FALSE, gettext("La discrétisation %d souhaitée est hors domaine %d.\n"), discretisation, barre->discretisation_element);
    BUGMSG(!((a < 0.) && (!(ERREUR_RELATIVE_EGALE(a, 0.)))), FALSE, gettext("La position de la charge ponctuelle (%f) est incorrecte.\n"), a);
    
    // La déformation en rotation d'une barre soumise à un effort de flexion autour de l'axe y
    // est calculée selon le principe des intégrales de Mohr et est définie par les formules :\end{verbatim}\begin{center}
    // \includegraphics[width=8cm]{images/charge_barre_ponctuelle_fz.pdf}\includegraphics[width=8cm]{images/charge_barre_ponctuelle_my.pdf}
    // \includegraphics[width=8cm]{images/charge_barre_ponctuelle_my_1.pdf}\includegraphics[width=8cm]{images/charge_barre_ponctuelle_fz_1.pdf}
    // \includegraphics[width=8cm]{images/charge_barre_ponctuelle_may_mby.pdf}\end{center}\begin{align*}
    // r_y(x) = & \int_0^x \frac{Mf_{11}(X) \cdot Mf_{31}(X)}{E \cdot I_y(X)} dX + \int_x^a \frac{Mf_{11}(X) \cdot Mf_{32}(X)}{E \cdot I_y(X)} dX + \int_a^L \frac{Mf_{12}(X) \cdot Mf_{32}(X)}{E \cdot I_y(X)} dX\nonumber\\
    //          & + \int_0^x \frac{Mf_{21}(X) \cdot Mf_{31}(X)}{E \cdot I_y(X)} dX + \int_x^a \frac{Mf_{21}(X) \cdot Mf_{32}(X)}{E \cdot I_y(X)} dX + \int_a^L \frac{Mf_{22}(X) \cdot Mf_{32}(X)}{E \cdot I_y(X)} dX\nonumber\\
    //          & + \int_0^x \frac{Mf_{5}(X) \cdot Mf_{31}(X)}{E \cdot I_y(X)} dX + \int_x^L \frac{Mf_{5}(X) \cdot Mf_{32}(X)}{E \cdot I_y(X)} dX\nonumber\\
    //          & \textrm{ pour x variant de 0 à a}\end{align*}\begin{align*}
    // r_y(x) = & \int_0^a \frac{Mf_{11}(X) \cdot Mf_{31}(X)}{E \cdot I_y(X)} dX + \int_a^x \frac{Mf_{12}(X) \cdot Mf_{31}(X)}{E \cdot I_y(X)} dX + \int_x^L \frac{Mf_{12}(X) \cdot Mf_{32}(X)}{E \cdot I_y(X)} dX \nonumber\\
    //          & + \int_0^a \frac{Mf_{21}(X) \cdot Mf_{31}(X)}{E \cdot I_y(X)} dX + \int_a^x \frac{Mf_{22}(X) \cdot Mf_{31}(X)}{E \cdot I_y(X)} dX + \int_x^L \frac{Mf_{22}(X) \cdot Mf_{32}(X)}{E \cdot I_y(X)} dX \nonumber\\
    //          & + \int_0^x \frac{Mf_{5}(X) \cdot Mf_{31}(X)}{E \cdot I_y(X)} dX + \int_x^L \frac{Mf_{5}(X) \cdot Mf_{32}(X)}{E \cdot I_y(X)} dX\nonumber\\
    //          &\textrm{ pour x variant de a à L}\end{align*}\begin{align*}
    // f_z(x) = & \int_0^x \frac{Mf_{11}(X) \cdot Mf_{41}(X)}{E \cdot I_y(X)} dX + \int_x^a \frac{Mf_{11}(X) \cdot Mf_{42}(X)}{E \cdot I_y(X)} dX + \int_a^L \frac{Mf_{12}(X) \cdot Mf_{42}(X)}{E \cdot I_y(X)} dX\nonumber\\
    //          & + \int_0^x \frac{Mf_{21}(X) \cdot Mf_{41}(X)}{E \cdot I_y(X)} dX + \int_x^a \frac{Mf_{21}(X) \cdot Mf_{42}(X)}{E \cdot I_y(X)} dX + \int_a^L \frac{Mf_{22}(X) \cdot Mf_{42}(X)}{E \cdot I_y(X)} dX\nonumber\\
    //          & + \int_0^x \frac{Mf_{5}(X) \cdot Mf_{41}(X)}{E \cdot I_y(X)} dX + \int_x^L \frac{Mf_{5}(X) \cdot Mf_{42}(X)}{E \cdot I_y(X)} dX\nonumber\\
    //          & \textrm{ pour x variant de 0 à a}\end{align*}\begin{align*}
    // f_z(x) = & \int_0^a \frac{Mf_{11}(X) \cdot Mf_{41}(X)}{E \cdot I_y(X)} dX + \int_a^x \frac{Mf_{12}(X) \cdot Mf_{41}(X)}{E \cdot I_y(X)} dX + \int_x^L \frac{Mf_{12}(X) \cdot Mf_{42}(X)}{E \cdot I_y(X)} dX \nonumber\\
    //          & + \int_0^a \frac{Mf_{21}(X) \cdot Mf_{41}(X)}{E \cdot I_y(X)} dX + \int_a^x \frac{Mf_{22}(X) \cdot Mf_{41}(X)}{E \cdot I_y(X)} dX + \int_x^L \frac{Mf_{22}(X) \cdot Mf_{42}(X)}{E \cdot I_y(X)} dX \nonumber\\
    //          & + \int_0^x \frac{Mf_{5}(X) \cdot Mf_{41}(X)}{E \cdot I_y(X)} dX + \int_x^L \frac{Mf_{5}(X) \cdot Mf_{42}(X)}{E \cdot I_y(X)} dX\nonumber\\
    //          &\textrm{ pour x variant de a à L}\end{align*}\begin{align*}
    // \textrm{ avec }
    //                 Mf_{11}(X) = &-\frac{F_z \cdot b \cdot X}{L} & Mf_{12}(X) = & -F_z \cdot a \cdot \left( \frac{L-X}{L} \right) \nonumber\\
    //                 Mf_{21}(X) = &-\frac{M_y \cdot X}{L} & Mf_{22}(X) = &\frac{M_y \cdot (L-X)}{L}\nonumber\\
    //                 Mf_{31}(X) = &-\frac{X}{L} & Mf_{32}(X) = &\frac{L-X}{L}\nonumber\\
    //                 Mf_{41}(X) = &-\frac{(L-x) \cdot X}{L} & Mf_{42}(X) = &-\frac{x \cdot (L-X)}{L}\nonumber\\
    //                 Mf_{5}(X) =  &\frac{M_{Ay} \cdot (L-X)}{L}-\frac{M_{By} \cdot X}{L} & & \end{align*}\begin{verbatim}
    if (discretisation == 0)
        debut = barre->noeud_debut;
    else
        debut = g_list_nth_data(barre->noeuds_intermediaires, discretisation-1);
    if (discretisation == barre->discretisation_element)
        fin = barre->noeud_fin;
    else
        fin = g_list_nth_data(barre->noeuds_intermediaires, discretisation);
    
    debut_barre = EF_noeuds_distance(barre->noeud_debut, debut);
    l = EF_noeuds_distance(debut, fin);
    BUG(!isnan(l), FALSE);
    BUGMSG(!((a > l) && (!(ERREUR_RELATIVE_EGALE(a, l)))), FALSE, gettext("La position de la charge ponctuelle (%f) est incorrecte. La longueur de la barre est de %f m.\n"), a, l);
    
    b = l-a;
    E = EF_calculs_E(barre->materiau);
    
    switch (barre->section->type)
    {
        case SECTION_RECTANGULAIRE :
        case SECTION_T :
        case SECTION_CARREE :
        case SECTION_CIRCULAIRE :
        {
            double      I = EF_sections_iy(barre->section);
            
            BUG(!isnan(I), FALSE);
            
            BUG(common_fonction_ajout_poly(f_rotation, 0., a,  fz*b/(6*E*I*l)*(-l*l+b*b),  0.,          fz*b/(2*E*I*l),      0., 0., 0., 0., debut_barre), FALSE);
            BUG(common_fonction_ajout_poly(f_rotation, a,  l, -fz*a/(6*E*I*l)*(2*l*l+a*a), fz*a/(E*I), -fz*a/(2*E*I*l),      0., 0., 0., 0., debut_barre), FALSE);
            BUG(common_fonction_ajout_poly(f_rotation, 0., a,  my/(6*E*I*l)*(-l*l+3*b*b),  0.,          my/(2*E*I*l),        0., 0., 0., 0., debut_barre), FALSE);
            BUG(common_fonction_ajout_poly(f_rotation, a,  l, my/(6*E*I*l)*(2*l*l+3*a*a),  -my/(E*I),   my/(2*E*I*l),        0., 0., 0., 0., debut_barre), FALSE);
            BUG(common_fonction_ajout_poly(f_rotation, 0., l, l/(6*E*I)*(2*may-mby),       -may/(E*I),  (may+mby)/(2*E*I*l), 0., 0., 0., 0., debut_barre), FALSE);
            
            BUG(common_fonction_ajout_poly(f_deform, 0., a,  0.,                fz*b/(6*E*I*l)*(l*l-b*b),    0.,                 -fz*b/(6*E*I*l),      0., 0., 0., debut_barre), FALSE);
            BUG(common_fonction_ajout_poly(f_deform, a,  l, -fz*a*a*a/(6*E*I),  fz*a/(6*E*I*l)*(a*a+2*l*l),  -fz*a/(2*E*I),      fz*a/(6*E*I*l),       0., 0., 0., debut_barre), FALSE);
            BUG(common_fonction_ajout_poly(f_deform, 0., a,  0.,                my/(6*E*I*l)*(l*l-3*b*b),    0.,                 -my/(6*E*I*l),        0., 0., 0., debut_barre), FALSE);
            BUG(common_fonction_ajout_poly(f_deform, a,  l, my/(6*E*I)*(3*a*a), -my/(6*E*I*l)*(2*l*l+3*a*a), my/(6*E*I*l)*(3*l), -my/(6*E*I*l),        0., 0., 0., debut_barre), FALSE);
            BUG(common_fonction_ajout_poly(f_deform, 0., l, 0.,                 l/(6*E*I)*(-2*may+mby),      may/(2*E*I),        -(mby+may)/(6*E*I*l), 0., 0., 0., debut_barre), FALSE);
    // Pour une section constante, les rotations valent :\end{verbatim}\begin{align*}
    // r_y(x) = & \frac{F_z \cdot b}{6 \cdot E \cdot I_y \cdot L} [-L^2+b^2 + 3 \cdot x^2] & &\\
    //          & +\frac{M_y}{6 \cdot E \cdot I_y \cdot L} (-L^2+3 \cdot b^2 + 3 \cdot x^2) & &\\
    //          & +\frac{L}{6 \cdot E \cdot I_y} \cdot \left(2 \cdot M_{Ay}-M_{By} - \frac{6 \cdot M_{Ay}}{L} \cdot x + 3 \cdot \frac{M_{Ay}+M_{By}}{L^2} \cdot x^2 \right) & &\textrm{ pour x variant de 0 à a}\nonumber\\
    // r_y(x) = & -\frac{F_z \cdot a}{6 \cdot E \cdot I_y \cdot L}(2 \cdot L^2+a^2 -6 \cdot L \cdot x + 3 \cdot x^2) & &\\
    //          & +\frac{M_y}{6 \cdot E \cdot I_y \cdot L} \cdot (2 \cdot L^2+3 \cdot a^2 -6 \cdot L \cdot x + 3 \cdot x^2) & &\\
    //          & +\frac{L}{6 \cdot E \cdot I_y} \cdot \left(2 \cdot M_{Ay}-M_{By} - \frac{6 \cdot M_{Ay}}{L} \cdot x + 3 \cdot \frac{M_{Ay}+M_{By}}{L^2} \cdot x^2 \right) & &\textrm{ pour x variant de a à L}\end{align*}\begin{align*}
    // f_z(x) = & \frac{F_z \cdot b \cdot x}{6 \cdot E \cdot I_y \cdot L}  \cdot \left( L^2-b^2 - x^2 \right) & &\\
    //          & + \frac{M_y \cdot x}{6 \cdot E \cdot I_y \cdot L} \cdot \left( L^2-3 \cdot b^2 - x^2 \right) & &\\
    //          & + \frac{x}{6 \cdot E \cdot I_y} \cdot \left( L \cdot (-2 \cdot M_{Ay}+M_{By}) + 3 \cdot M_{Ay} \cdot x - \frac{M_{By}+M_{Ay}}{L} \cdot x^2 \right) & &\textrm{ pour x variant de 0 à a}\nonumber\\
    // f_z(x) = & \frac{F_z \cdot a}{6 \cdot E \cdot I_y \cdot L} \cdot \left( -a^2 \cdot L + (a^2+2 \cdot L^2) \cdot x - 3 \cdot L \cdot x^2 + x^3 \right) & &\\
    //          & + \frac{M_y}{6 \cdot E \cdot I_y \cdot L} \left(3 \cdot a^2 \cdot L - (2 \cdot L^2+3 \cdot a^2) \cdot x + 3 \cdot L \cdot x^2 - x^3 \right) & &\\
    //          & + \frac{x}{6 \cdot E \cdot I_y} \cdot \left( L \cdot (-2 \cdot M_{Ay}+M_{By}) + 3 \cdot M_{Ay} \cdot x - \frac{M_{By}+M_{Ay}}{L} \cdot x^2 \right) & &\textrm{ pour x variant de a à L}\end{align*}\begin{verbatim}
            return TRUE;
            break;
        }
        default :
        {
            BUGMSG(0, FALSE, gettext("Type de section %d inconnu.\n"), barre->section->type);
            break;
        }
    }
}


gboolean EF_charge_barre_ponctuelle_fonc_rz(Fonction *f_rotation, Fonction* f_deform,
  Beton_Barre *barre, unsigned int discretisation, double a, double fy, double mz, double maz,
  double mbz)
/* Description : Calcule les déplacements d'une barre en rotation autour de l'axe z et en
 *                 déformation selon l'axe y en fonction de la charge ponctuelle (fy et mz) et
 *                 des efforts aux extrémités de la poutre.
 * Paramètres : Fonction *f_rotation : fonction où sera ajoutée la rotation,
 *            : Fonction *f_deform : fonction où sera ajoutée la déformation,
 *            : Beton_Barre *barre : Barre à étudier,
 *            : unsigned int discretisation : partie de la barre à étudier,
 *            : double a : position de la charge par rapport au début de la partie de barre à
 *                         étudier,
 *            : double fy : force suivant l'axe y,
 *            : double mz : moment autour de l'axe z,
 *            : double maz : moment au début de la barre,
 *            : double mbz : moment à la fin de la barre.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             f_rotation == NULL,
 *             f_deform == NULL,
 *             barre == NULL,
 *             barre->section == NULL,
 *             barre->materiau == NULL,
 *             barre->noeud_debut == NULL,
 *             barre->noeud_fin == NULL,
 *             discretisation>barre->discretisation_element,
 *             a < 0. ou a > l,
 *             en cas d'erreur due à une fonction interne.
 */
{
    EF_Noeud    *debut, *fin;
    double      l, b;
    double      E, debut_barre;
    
    BUGMSG(f_rotation, FALSE, gettext("Paramètre %s incorrect.\n"), "f_rotation");
    BUGMSG(f_deform, FALSE, gettext("Paramètre %s incorrect.\n"), "f_deform");
    BUGMSG(barre, FALSE, gettext("Paramètre %s incorrect.\n"), "barre");
    BUGMSG(barre->section, FALSE, gettext("Section indéfinie.\n"));
    BUGMSG(barre->materiau, FALSE, gettext("Matériau indéfini.\n"));
    BUGMSG(barre->noeud_debut, FALSE, gettext("Noeud 1 indéfini.\n"));
    BUGMSG(barre->noeud_fin, FALSE, gettext("Noeud 2 indéfini.\n"));
    BUGMSG(discretisation<=barre->discretisation_element, FALSE, gettext("La discrétisation %d souhaitée est hors domaine %d.\n"), discretisation, barre->discretisation_element);
    BUGMSG(!((a < 0.) && (!(ERREUR_RELATIVE_EGALE(a, 0.)))), FALSE, gettext("La position de la charge ponctuelle (%f) est incorrecte.\n"), a);
    
    // La déformation en rotation d'une barre soumise à un effort de flexion autour de l'axe y
    //   est calculée selon le principe des intégrales de Mohr et est définie par les mêmes
    //   formules que la fonction EF_charge_barre_ponctuelle_fonc_rz à ceci près que fz
    //   est remplacé par fy (il y également un changement de signe de fy lors du calcul des
    //   rotations dû au changement de repère), my par mz, may et mby par maz et mbz (il y
    //   également un changement de signe de mz, maz et mbz lors du calcul des déformations
    //   toujours dû au changement de repère) et Iy par Iz.
    
    if (discretisation == 0)
        debut = barre->noeud_debut;
    else
        debut = g_list_nth_data(barre->noeuds_intermediaires, discretisation-1);
    if (discretisation == barre->discretisation_element)
        fin = barre->noeud_fin;
    else
        fin = g_list_nth_data(barre->noeuds_intermediaires, discretisation);
    
    debut_barre = EF_noeuds_distance(barre->noeud_debut, debut);
    l = EF_noeuds_distance(debut, fin);
    BUG(!isnan(l), FALSE);
    BUGMSG(!((a > l) && (!(ERREUR_RELATIVE_EGALE(a, l)))), FALSE, gettext("La position de la charge ponctuelle (%f) est incorrecte. La longueur de la barre est de %f m.\n"), a, l);
    
    b = l-a;
    E = EF_calculs_E(barre->materiau);
    
    switch (barre->section->type)
    {
        case SECTION_RECTANGULAIRE :
        case SECTION_T :
        case SECTION_CARREE :
        case SECTION_CIRCULAIRE :
        {
            double      I = EF_sections_iz(barre->section);
            
            BUG(!isnan(I), FALSE);
            
            BUG(common_fonction_ajout_poly(f_rotation, 0., a, fy*b/(6*E*I*l)*a*(l+b),     0.,          -fy*b/(2*E*I*l),     0., 0., 0., 0., debut_barre), FALSE);
            BUG(common_fonction_ajout_poly(f_rotation, a,  l, fy*a/(6*E*I*l)*(2*l*l+a*a), -fy*a/(E*I), fy*a/(2*E*I*l),      0., 0., 0., 0., debut_barre), FALSE);
            BUG(common_fonction_ajout_poly(f_rotation, 0., a, mz/(6*E*I*l)*(-l*l+3*b*b),  0.,          mz/(2*E*I*l),        0., 0., 0., 0., debut_barre), FALSE);
            BUG(common_fonction_ajout_poly(f_rotation, a,  l, mz/(6*E*I*l)*(2*l*l+3*a*a), -mz/(E*I),   mz/(2*E*I*l),        0., 0., 0., 0., debut_barre), FALSE);
            BUG(common_fonction_ajout_poly(f_rotation, 0., l, l/(6*E*I)*(2*maz-mbz),      -maz/(E*I),  (maz+mbz)/(2*E*I*l), 0., 0., 0., 0., debut_barre), FALSE);
            
            BUG(common_fonction_ajout_poly(f_deform, 0., a, 0.,                  fy*b/(6*E*I*l)*(l*l-b*b),   0.,                  -fy*b/(6*E*I*l),     0., 0., 0., debut_barre), FALSE);
            BUG(common_fonction_ajout_poly(f_deform, a,  l, -fy*a*a*a/(6*E*I),   fy*a/(6*E*I*l)*(a*a+2*l*l), -fy*a/(2*E*I),       fy*a/(6*E*I*l),      0., 0., 0., debut_barre), FALSE);
            BUG(common_fonction_ajout_poly(f_deform, 0., a, 0.,                  mz/(6*E*I*l)*(-l*l+3*b*b),  0.,                  mz/(6*E*I*l),        0., 0., 0., debut_barre), FALSE);
            BUG(common_fonction_ajout_poly(f_deform, a,  l, -mz/(6*E*I)*(3*a*a), mz/(6*E*I*l)*(2*l*l+3*a*a), -mz/(6*E*I*l)*(3*l), mz/(6*E*I*l),        0., 0., 0., debut_barre), FALSE);
            BUG(common_fonction_ajout_poly(f_deform, 0., l, 0.,                  l/(6*E*I)*(2*maz-mbz),      -maz/(2*E*I),        (mbz+maz)/(6*E*I*l), 0., 0., 0., debut_barre), FALSE);
            return TRUE;
            break;
        }
        default :
        {
            BUGMSG(0, FALSE, gettext("Type de section %d inconnu.\n"), barre->section->type);
            break;
        }
    }
}


gboolean EF_charge_barre_ponctuelle_n(Fonction *fonction, Beton_Barre *barre,
  unsigned int discretisation, double a, double fax, double fbx)
/* Description : Calcule les déplacements d'une barre selon l'axe x en fonction de l'effort
 *               normal ponctuel n et des réactions d'appuis fax et fbx.
 * Paramètres : Fonction *fonction : fonction où sera ajoutée la déformée,
 *            : Beton_Barre *barre : Barre à étudier,
 *            : unsigned int discretisation : partie de la barre à étudier,
 *            : double a : position de la charge par rapport au début de la partie de barre,
 *            : double n : effort normal de la charge ponctuelle,
 *            : double fax : effort normal au début de la barre,
 *            : double fbx : effort normal à la fin de la barre.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             barre == NULL,
 *             fonction == NULL,
 *             barre->section == NULL,
 *             barre->materiau == NULL,
 *             barre->noeud_debut == NULL,
 *             barre->noeud_fin == NULL,
 *             discretisation>barre->discretisation_element,
 *             a < 0. ou a > l,
 *             en cas d'erreur due à une fonction interne.
 */
{
    EF_Noeud    *debut, *fin;
    double      l, debut_barre;
    double      E;
    
    BUGMSG(fonction, FALSE, gettext("Paramètre %s incorrect.\n"), "fonction");
    BUGMSG(barre, FALSE, gettext("Paramètre %s incorrect.\n"), "barre");
    BUGMSG(barre->section, FALSE, gettext("Section indéfinie.\n"));
    BUGMSG(barre->materiau, FALSE, gettext("Matériau indéfini.\n"));
    BUGMSG(barre->noeud_debut, FALSE, gettext("Noeud 1 indéfini.\n"));
    BUGMSG(barre->noeud_fin, FALSE, gettext("Noeud 2 indéfini.\n"));
    BUGMSG(discretisation<=barre->discretisation_element, FALSE, gettext("La discrétisation %d souhaitée est hors domaine %d.\n"), discretisation, barre->discretisation_element);
    BUGMSG(!((a < 0.) && (!(ERREUR_RELATIVE_EGALE(a, 0.)))), FALSE, gettext("La position de la charge ponctuelle (%f) est incorrecte.\n"), a);
    
    // La déformation selon l'axe x est par la formule :\end{verbatim}\begin{center}
    // \includegraphics[width=8cm]{images/charge_barre_ponctuelle_n.pdf}\end{center}\begin{align*}
    // f_x(x) = & F_{Ax} \cdot \int_0^x \frac{1}{E \cdot S(x)} dx & & \textrm{ pour x variant de 0 à a} \nonumber\\
    // f_x(x) = & F_{Ax} \cdot \int_0^a \frac{1}{E \cdot S(x)} dx - F_{Bx} \cdot \int_a^x \frac{1}{E \cdot S(x)} dx & & \textrm{ pour x variant de a à l} \end{align*}\begin{verbatim}
    
    if (discretisation == 0)
        debut = barre->noeud_debut;
    else
        debut = g_list_nth_data(barre->noeuds_intermediaires, discretisation-1);
    if (discretisation == barre->discretisation_element)
        fin = barre->noeud_fin;
    else
        fin = g_list_nth_data(barre->noeuds_intermediaires, discretisation);
    
    debut_barre = EF_noeuds_distance(barre->noeud_debut, debut);
    l = EF_noeuds_distance(debut, fin);
    BUG(!isnan(l), FALSE);
    BUGMSG(!((a > l) && (!(ERREUR_RELATIVE_EGALE(a, l)))), FALSE, gettext("La position de la charge ponctuelle (%f) est incorrecte. La longueur de la barre est de %f m.\n"), a, l);
    
    E = EF_calculs_E(barre->materiau);
    
    switch (barre->section->type)
    {
        case SECTION_RECTANGULAIRE :
        case SECTION_T :
        case SECTION_CARREE :
        case SECTION_CIRCULAIRE :
        {
            double      S = EF_sections_s(barre->section);
            
            BUG(!isnan(S), FALSE);
            
    // Pour une section constante, les déformations valent :\end{verbatim}\begin{align*}
    // f_x(x) = & \frac{F_{Ax} \cdot x}{E \cdot S} & &\textrm{ pour x variant de 0 à a}\nonumber\\
    // f_x(x) = & \frac{a \cdot (F_{Ax} + F_{Bx}) - F_{Bx} \cdot x}{E \cdot S} & & \textrm{ pour x variant de a à l}\end{align*}\begin{verbatim}
            BUG(common_fonction_ajout_poly(fonction, 0., a, 0.,                 fax/(E*S), 0., 0., 0., 0., 0., debut_barre), FALSE);
            BUG(common_fonction_ajout_poly(fonction, a,  l, a*(fax+fbx)/(E*S), -fbx/(E*S), 0., 0., 0., 0., 0., debut_barre), FALSE);
            return TRUE;
            break;
        }
        default :
        {
            BUGMSG(0, FALSE, gettext("Type de section %d inconnu.\n"), barre->section->type);
            break;
        }
    }
}


gboolean EF_charge_barre_ponctuelle_enleve_barres(Charge_Barre_Ponctuelle *charge,
  GList *barres, Projet *projet)
/* Description : Enlève à la charge une liste de barres pouvant être utilisées. Dans le cas où
 *               une barre de la liste n'est pas dans la charge, ce point ne sera pas considéré
 *               comme une erreur mais la barre sera simplement ignorée.
 * Paramètres : Charge_Barre_Ponctuelle *charge : la charge à modifier,
 *              GList *barres : la liste de pointers de type Beton_Barre devant être retirés,
 *              Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             charge == NULL.
 */
{
    GList   *list_parcours = barres;
    
    BUGMSG(charge, FALSE, gettext("Paramètre %s incorrect.\n"), "charge");
    
    while (list_parcours != NULL)
    {
        Beton_Barre *barre = list_parcours->data;
        
        charge->barres = g_list_remove(charge->barres, barre);
        
        list_parcours = g_list_next(list_parcours);
    }
    
#ifdef ENABLE_GTK
    if (projet->list_gtk._1990_actions.builder != NULL)
    {
        GtkTreeModel    *model;
        GtkTreeIter     Iter;
        
        if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION(gtk_builder_get_object(projet->list_gtk._1990_actions.builder, "1990_actions_treeview_select_action")), &model, &Iter))
        {
            unsigned int    num;
            Action          *action;
            
            gtk_tree_model_get(model, &Iter, 0, &num, -1);
            
            BUG(action = _1990_action_cherche_numero(projet, num), FALSE);
            
            if (g_list_find(action->charges, charge))
                BUG(EF_gtk_charge_barre_ponctuelle_ajout_affichage(charge, projet, FALSE), FALSE);
        }
    }
#endif
    
    BUG(EF_calculs_free(projet), FALSE);
    
    return TRUE;
}


gboolean EF_charge_barre_ponctuelle_free(Charge_Barre_Ponctuelle *charge)
/* Description : Libère une charge ponctuelle sur barre.
 * Paramètres : Charge_Barre_Ponctuelle *charge : la charge à libérer.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             charge == NULL.
 */
{
    BUGMSG(charge, FALSE, gettext("Paramètre %s incorrect.\n"), "charge");
    
    free(charge->nom);
    g_list_free(charge->barres);
    free(charge);
    
    return TRUE;
}
