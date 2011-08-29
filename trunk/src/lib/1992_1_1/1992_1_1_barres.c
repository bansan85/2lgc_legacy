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
#include <stdlib.h>
#include <libintl.h>
#include <cholmod.h>
#include <values.h>
#include <string.h>
#include "common_projet.h"
#include "common_erreurs.h"
#include "common_maths.h"
#include "1992_1_1_barres.h"
#include "EF_calculs.h"
#include "math.h"

int _1992_1_1_barres_init(Projet *projet)
/* Description : Initialise la liste des éléments en béton
 * Paramètres : Projet *projet : la variable projet
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet == NULL)
 *           -2 en cas d'erreur d'allocation mémoire
 */
{
    BUGMSG(projet, -1, "_1992_1_1_barres_init\n");
    
    // Trivial
    projet->beton.barres = list_init();
    BUGMSG(projet->beton.barres, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "_1992_1_1_barres_init");
    
    return 0;
}

int _1992_1_1_barres_ajout(Projet *projet, Type_Element type, unsigned int section,
  unsigned int materiau, unsigned int noeud_debut, unsigned int noeud_fin, int relachement,
  unsigned int discretisation_element)
/* Description : Ajoute un élément à la liste des éléments en béton
 * Paramètres : Projet *projet : la variable projet
 *            : Type_Beton_Barre type : type de l'élément en béton
 *            : int section : numéro de la section correspondant à l'élément
 *            : int materiau : numéro du matériau en béton de l'élément
 *            : int noeud_debut : numéro de départ de l'élément
 *            : int noeud_fin : numéro de fin de l'élément
 *            : int relachement : relachement de la barre (-1 si aucun).
 *            : int discretisation_element : nombre d'élément une fois discrétisé
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet == NULL) ou
 *             (projet->beton.barres == NULL) ou
 *             (noeud_debut == noeud_fin) ou
 *             section introuvable ou
 *             materiau introuvable ou
 *             noeud_debut introuvable ou
 *             noeud_fin introuvable ou
 *             relachement introuvable
 *           -2 en cas d'erreur d'allocation mémoire
 *           -3 en cas d'erreur due à une fonction interne
 */
{
    Beton_Barre   *element_en_cours, element_nouveau;
    
    // Trivial
    BUGMSG(projet, -1, "_1992_1_1_barres_ajout\n");
    BUGMSG(projet->beton.barres, -1, "_1992_1_1_barres_ajout\n");
    BUGMSG(noeud_debut != noeud_fin, -1, "_1992_1_1_barres_ajout\n");
    
    list_mvrear(projet->beton.barres);
    element_nouveau.type = type;
    
    BUG(element_nouveau.section = _1992_1_1_sections_cherche_numero(projet, section), -1);
    BUG(element_nouveau.materiau = _1992_1_1_materiaux_cherche_numero(projet, materiau), -1);
    BUG(element_nouveau.noeud_debut = EF_noeuds_cherche_numero(projet, noeud_debut), -1);
    BUG(element_nouveau.noeud_fin = EF_noeuds_cherche_numero(projet, noeud_fin), -1);
    
    if (relachement != -1)
        BUG(element_nouveau.relachement = EF_relachement_cherche_numero(projet, relachement), -1);
    else
        element_nouveau.relachement = NULL;
    
    element_nouveau.discretisation_element = discretisation_element;
    
    element_nouveau.info_EF = (Barre_Info_EF*)malloc(sizeof(Barre_Info_EF)*(discretisation_element+1));
    BUGMSG(element_nouveau.info_EF, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "_1992_1_1_barres_ajout");
    
    if (discretisation_element != 0)
    {
        unsigned int    i;
        
        element_nouveau.noeuds_intermediaires = malloc(sizeof(EF_Noeud)*(discretisation_element));
        BUGMSG(element_nouveau.noeuds_intermediaires, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "_1992_1_1_barres_ajout");

        /* Création des noeuds intermédiaires */
        for (i=0;i<discretisation_element;i++)
        {
            double      dx, dy, dz;
            dx = (element_nouveau.noeud_fin->position.x-element_nouveau.noeud_debut->position.x)/(discretisation_element+1);
            dy = (element_nouveau.noeud_fin->position.y-element_nouveau.noeud_debut->position.y)/(discretisation_element+1);
            dz = (element_nouveau.noeud_fin->position.z-element_nouveau.noeud_debut->position.z)/(discretisation_element+1);
            BUG(EF_noeuds_ajout(projet, element_nouveau.noeud_debut->position.x+dx*(i+1), element_nouveau.noeud_debut->position.y+dy*(i+1), element_nouveau.noeud_debut->position.z+dz*(i+1), -1) == 0, -2);
            element_nouveau.noeuds_intermediaires[i] = list_rear(projet->ef_donnees.noeuds);
        }
    }
    else
        element_nouveau.noeuds_intermediaires = NULL;
    
    element_nouveau.matrice_rotation = NULL;
    element_nouveau.matrice_rotation_transpose = NULL;
    
    element_en_cours = (Beton_Barre *)list_rear(projet->beton.barres);
    if (element_en_cours == NULL)
        element_nouveau.numero = 0;
    else
        element_nouveau.numero = element_en_cours->numero+1;
    
    BUGMSG(list_insert_after(projet->beton.barres, &(element_nouveau), sizeof(element_nouveau)), -2, gettext("%s : Erreur d'allocation mémoire.\n"), "_1992_1_1_barres_ajout");
    
    return 0;
}


Beton_Barre* _1992_1_1_barres_cherche_numero(Projet *projet, unsigned int numero)
/* Description : Positionne dans la liste des éléments en béton l'élément courant au numéro
 * Paramètres : Projet *projet : la variable projet
 *            : int numero : le numéro de la section
 * Valeur renvoyée :
 *   Succès : Pointeur vers l'élément en béton
 *   Échec : NULL en cas de paramètres invalides :
 *             (projet == NULL)
 *             (projet->beton.barres == NULL) ou
 *             (list_size(projet->beton.barres) == 0) ou
 *             Barre en béton introuvable.
 */
{
    BUGMSG(projet, NULL, "_1992_1_1_barres_cherche_numero\n");
    BUGMSG(projet->beton.barres, NULL, "_1992_1_1_barres_cherche_numero\n");
    BUGMSG(list_size(projet->beton.barres), NULL, "_1992_1_1_barres_cherche_numero\n");
    
    // Trivial
    list_mvfront(projet->beton.barres);
    do
    {
        Beton_Barre   *element = list_curr(projet->beton.barres);
        
        if (element->numero == numero)
            return element;
    }
    while (list_mvnext(projet->beton.barres) != NULL);
    
    BUGMSG(0, NULL, gettext("Barre en béton %d introuvable.\n"), numero);
}


int _1992_1_1_barres_rigidite_ajout(Projet *projet, Beton_Barre *element)
/* Description : ajouter un élément à la matrice de rigidité partielle et complete
 * Paramètres : Projet *projet : la variable projet
 *            : Beton_Barre* element : pointeur vers l'élément en béton
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet == NULL) ou
 *             (projet->ef_donnees.triplet_rigidite_partielle == NULL) ou
 *             (element == NULL) ou
 *             (projet->ef_donnees.triplet_rigidite_complete == NULL) ou
 *             (element->section == NULL) ou
 *             (distance entre le début et l'extrémité de la barre est nulle)
 *           -2 en cas d'erreur d'allocation mémoire
 *           -3 en cas d'erreur due à une fonction interne
 */
{
    EF_Noeud            *noeud1, *noeud2;
    long                *ai, *aj;
    double              *ax;
    long                *ai2, *aj2;
    double              *ax2;
    long                *ai3, *aj3;
    double              *ax3;
    double              y, cosz, sinz;
    double              xx, yy, zz, ll;
    unsigned int        i, j;
    cholmod_triplet     *triplet;
    cholmod_sparse      *sparse_tmp, *matrice_rigidite_globale;
    Beton_Section_Carre *section_donnees;
    Beton_Section_Caracteristiques  *section_caract;
    double              E, S, Iz, Iy, J, G;
    
    BUGMSG(projet, -1, "_1992_1_1_barres_rigidite_ajout\n");
    BUGMSG(projet->ef_donnees.triplet_rigidite_partielle, -1, "_1992_1_1_barres_rigidite_ajout\n");
    BUGMSG(element, -1, "_1992_1_1_barres_rigidite_ajout\n");
    BUGMSG(projet->ef_donnees.triplet_rigidite_complete, -1, "_1992_1_1_barres_rigidite_ajout\n");
    
    BUGMSG(element->section, -1, "_1992_1_1_barres_rigidite_ajout\n");
    section_donnees = element->section;
    section_caract = section_donnees->caracteristiques;
    E = element->materiau->ecm;
    S = section_caract->s;
    Iy = section_caract->iy;
    Iz = section_caract->iz;
    J = section_caract->j;
    G = element->materiau->gnu_0_2;
    
    // Calcul de la matrice de rotation 3D qui permet de passer du repère local au repère
    //   global. Elle est déterminée par le calcul de deux angles : z faisant une rotation
    //   dans le repère local autour de l'axe z et y faisant une rotation dans le repère
    //   local autour de l'axe y. Ainsi, le système d'équation à résoudre est le suivant : \end{verbatim}\begin{displaymath}
    //   \begin{bmatrix} cos(z) & -sin(z) & 0 \\
    //                   sin(z) &  cos(z) & 0 \\
    //                     0    &    0    & 1
    //   \end{bmatrix} \cdot 
    //   \begin{bmatrix} cos(y) &    0    & -sin(y) \\
    //                     0    &    1    &    0    \\
    //                   sin(y) &    0    &  cos(y)
    //   \end{bmatrix} \cdot
    //   \begin{pmatrix} L \\
    //                   0 \\
    //                   0
    //   \end{pmatrix} =
    //   \begin{pmatrix} L_x \\
    //                   L_y \\
    //                   L_z
    //   \end{pmatrix}
    //   \end{displaymath}\begin{displaymath}
    //   \begin{bmatrix} cos(z) \cdot cos(y) & -sin(z) & -cos(z) \cdot sin(y) \\
    //                   sin(z) \cdot cos(y) &  cos(z) & -sin(z) \cdot sin(y) \\
    //                         sin(y)        &    0    &        cos(y)
    //   \end{bmatrix} \cdot
    //   \begin{pmatrix} L \\
    //                   0 \\
    //                   0
    //   \end{pmatrix} =
    //   \begin{pmatrix} L_x \\
    //                   L_y \\
    //                   L_z
    //   \end{pmatrix}
    //   \end{displaymath}\begin{displaymath}
    //   \begin{pmatrix} L \cdot cos(y) \cdot cos(z) \\
    //                   L \cdot cos(y) \cdot sin(z) \\
    //                   L \cdot sin(y)
    //   \end{pmatrix} =
    //   \begin{pmatrix} L_x \\
    //                   L_y \\
    //                   L_z
    //   \end{pmatrix}
    //   \end{displaymath}\begin{verbatim}
    //   avec L_x, L_y et L_z étant la différence, dans le repère global, entre les coordonnées
    //   des points définissant la fin et le début de la barre.
    //   À partir de ce système d'équations, il est possible de déterminer deux valeurs y.\end{verbatim}\begin{displaymath}
    //   y = arcsin \left( \frac{L_z}{L} \right) \texttt{ et }y = \pi - arcsin \left( \frac{L_z}{L} \right)
    //   \end{displaymath}\begin{verbatim}
    //   Il sera retenu la première solution pour des raisons de simplicité.
    //   Ensuite, en intégrant le résultat dans les deux autres équations restantes :\end{verbatim}\begin{displaymath}
    //   L \cdot cos \left ( arcsin \left( \frac{L_z}{L} \right) \right ) \cdot cos(z) = \sqrt{L^2-L_z^2} \cdot cos (z) = L_x\end{displaymath}\begin{displaymath}
    //   L \cdot cos \left ( arcsin \left( \frac{L_z}{L} \right) \right ) \cdot sin(z) = \sqrt{L^2-L_z^2} \cdot sin (z) = L_y
    //   \end{displaymath}\begin{verbatim}
    //   Il existe alors deux solutions :\end{verbatim}\begin{displaymath}
    //   z = arcsin \left( \frac{L_y}{\sqrt{L^2-L_z^2}} \right) \texttt{ et }z = \pi - arcsin \left( \frac{L_y}{\sqrt{L^2-L_z^2}} \right)
    //   \end{displaymath}\begin{verbatim}
    //   Après application de la première solution, les deux équations deviennent :\end{verbatim}\begin{displaymath}
    //   \left | L_x \right | = L_x \texttt{ et }L_y = L_y\end{displaymath}\begin{verbatim}
    //   et après application de la deuxième solution, les deux équations deviennent :\end{verbatim}\begin{displaymath}
    //   -\left | L_x \right | = L_x\texttt{ et }L_y = L_y\end{displaymath}\begin{verbatim}
    //   On constate que la seule différence est que la première solution doit être utilisée
    //   pour avoir un L_x positif et la deuxième solution doit être utilisée pour avoir un
    //   L_x négatif. Il est donc possible d'obtenir une seule et unique solution :\end{verbatim}\begin{displaymath}
    //   cos(z) = signe\{L_x\} \cdot \sqrt{\frac{L_x^2}{L^2-L_z^2}}\texttt{ et }sin(z) = \frac{yy}{\sqrt{L^2-L_z^2}}\end{displaymath}\begin{verbatim}
    triplet = cholmod_l_allocate_triplet(12, 12, 32, 0, CHOLMOD_REAL, projet->ef_donnees.c);
    BUGMSG(triplet, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "_1992_1_1_barres_rigidite_ajout");
    ai = triplet->i;
    aj = triplet->j;
    ax = triplet->x;
    xx = element->noeud_fin->position.x - element->noeud_debut->position.x;
    yy = element->noeud_fin->position.y - element->noeud_debut->position.y;
    zz = element->noeud_fin->position.z - element->noeud_debut->position.z;
    ll = sqrt(xx*xx+yy*yy+zz*zz);
    BUGMSG(!ERREUR_RELATIVE_EGALE(0.0, ll), -1, "_1992_1_1_barres_rigidite_ajout");
    // Détermination de l'angle de rotation autour de l'axe Y.
    y = asin(zz/ll);
    if (ERREUR_RELATIVE_EGALE(ll*ll-zz*zz, 0.))
    {
        sinz = 0.;
        cosz = 1.;
    }
    else
    {
        sinz = yy / sqrt(ll*ll-zz*zz);
        cosz = copysign(1.0, xx)*sqrt(xx*xx/(ll*ll-zz*zz));
    }
    for (i=0;i<4;i++)
    {
        ai[i*8+0] = i*3+0; aj[i*8+0] = i*3+0; ax[i*8+0] = cosz*cos(y);
        ai[i*8+1] = i*3+0; aj[i*8+1] = i*3+1; ax[i*8+1] = -sinz;
        ai[i*8+2] = i*3+0; aj[i*8+2] = i*3+2; ax[i*8+2] = -cosz*sin(y);
        ai[i*8+3] = i*3+1; aj[i*8+3] = i*3+0; ax[i*8+3] = sinz*cos(y);
        ai[i*8+4] = i*3+1; aj[i*8+4] = i*3+1; ax[i*8+4] = cosz;
        ai[i*8+5] = i*3+1; aj[i*8+5] = i*3+2; ax[i*8+5] = -sinz*sin(y);
        ai[i*8+6] = i*3+2; aj[i*8+6] = i*3+0; ax[i*8+6] = sin(y);
        ai[i*8+7] = i*3+2; aj[i*8+7] = i*3+2; ax[i*8+7] = cos(y);
    }
    triplet->nnz=32;
    element->matrice_rotation = cholmod_l_triplet_to_sparse(triplet, 0, projet->ef_donnees.c);
    BUGMSG(element->matrice_rotation, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "_1992_1_1_barres_rigidite_ajout");
    element->matrice_rotation_transpose = cholmod_l_transpose(element->matrice_rotation, 1, projet->ef_donnees.c);
    BUGMSG(element->matrice_rotation_transpose, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "_1992_1_1_barres_rigidite_ajout");
    cholmod_l_free_triplet(&triplet, projet->ef_donnees.c);
    
    // Une fois la matrice de rotation déterminée, il est nécessaire de calculer la matrice de
    //   rigidité élémentaire dans le repère local. La poutre pouvant être discrétisée, une
    //   matrice de rigidité élémentaire est déterminée pour chaque tronçon.
    // Pour chaque discrétisation
    for (j=0;j<element->discretisation_element+1;j++)
    {
        double          MA, MB;
        Barre_Info_EF   infos;
    
    //     Détermination du noeud de départ et de fin
        if (j==0)
        {
            noeud1 = element->noeud_debut;
            if (element->discretisation_element != 0)
                noeud2 = element->noeuds_intermediaires[0];
            else
                noeud2 = element->noeud_fin;
        }
        else if (j == element->discretisation_element)
        {
            noeud1 = element->noeuds_intermediaires[j-1];
            noeud2 = element->noeud_fin;
        }
        else
        {
            noeud1 = element->noeuds_intermediaires[j-1];
            noeud2 = element->noeuds_intermediaires[j];
        }
    //     Calcul des L_x, L_y, L_z et L.
        xx = noeud2->position.x - noeud1->position.x;
        yy = noeud2->position.y - noeud1->position.y;
        zz = noeud2->position.z - noeud1->position.z;
        ll = sqrt(xx*xx+yy*yy+zz*zz);
        
    //     Détermination des paramètres de souplesse de l'élément de barre :\end{verbatim}\begin{align*}
    //               a_y = c_y & = \frac{l}{3 \cdot E \cdot I_y} \nonumber\\
    //               b_y & = \frac{l}{6 \cdot E \cdot I_y} \nonumber\\
    //               a_z = c_z & = \frac{l}{3 \cdot E \cdot I_z} \nonumber\\
    //               b_z & = \frac{l}{6 \cdot E \cdot I_z}\end{align*}\begin{verbatim}
        element->info_EF[j].ay = ll/(3*E*Iy);
        element->info_EF[j].by = ll/(6*E*Iy);
        element->info_EF[j].cy = ll/(3*E*Iy);
        element->info_EF[j].az = ll/(3*E*Iz);
        element->info_EF[j].bz = ll/(6*E*Iz);
        element->info_EF[j].cz = ll/(3*E*Iz);
        
    //     Calcul des coefficients kA et kB définissant l'inverse de la raideur aux
    //       noeuds. Ainsi k = 0 en cas d'encastrement et infini en cas d'articulation.
            /* Moment en Y et Z */
        if (element->relachement == NULL)
        {
            element->info_EF[j].kAy = 0;
            element->info_EF[j].kBy = 0;
            element->info_EF[j].kAz = 0;
            element->info_EF[j].kBz = 0;
        }
        else
        {
            if (noeud1 != element->noeud_debut)
            {
                element->info_EF[j].kAy = 0;
                element->info_EF[j].kAz = 0;
            }
            else
            {
                switch (element->relachement->ry_debut)
                {
                    case EF_RELACHEMENT_BLOQUE :
                    {
                        element->info_EF[j].kAy = 0;
                        break;
                    }
                    case EF_RELACHEMENT_LIBRE :
                    {
                        element->info_EF[j].kAy = MAXDOUBLE;
                        break;
                    }
                    case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
                    {
                        EF_Relachement_Donnees_Elastique_Linaire *donnees;
                        
                        donnees = element->relachement->ry_d_data;
                        if (donnees->raideur == 0.)
                            element->info_EF[j].kAy = MAXDOUBLE;
                        else
                            element->info_EF[j].kAy = 1./donnees->raideur;
                        break;
                    }
                    default :
                    {
                        BUG(0, -1);
                        break;
                    }
                }
                
                switch (element->relachement->rz_debut)
                {
                    case EF_RELACHEMENT_BLOQUE :
                    {
                        element->info_EF[j].kAz = 0;
                        break;
                    }
                    case EF_RELACHEMENT_LIBRE :
                    {
                        element->info_EF[j].kAz = MAXDOUBLE;
                        break;
                    }
                    case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
                    {
                        EF_Relachement_Donnees_Elastique_Linaire *donnees;
                        
                        donnees = element->relachement->rz_d_data;
                        if (donnees->raideur == 0.)
                            element->info_EF[j].kAz = MAXDOUBLE;
                        else
                            element->info_EF[j].kAz = 1./donnees->raideur;
                        break;
                    }
                    default :
                    {
                        BUG(0, -1);
                        break;
                    }
                }
            }
            
            if (noeud2 != element->noeud_fin)
            {
                element->info_EF[j].kBy = 0;
                element->info_EF[j].kBz = 0;
            }
            else
            {
                switch (element->relachement->ry_fin)
                {
                    case EF_RELACHEMENT_BLOQUE :
                    {
                        element->info_EF[j].kBy = 0;
                        break;
                    }
                    case EF_RELACHEMENT_LIBRE :
                    {
                        element->info_EF[j].kBy = MAXDOUBLE;
                        break;
                    }
                    case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
                    {
                        EF_Relachement_Donnees_Elastique_Linaire *donnees;
                        
                        donnees = element->relachement->ry_f_data;
                        if (donnees->raideur == 0.)
                            element->info_EF[j].kBy = MAXDOUBLE;
                        else
                            element->info_EF[j].kBy = 1./donnees->raideur;
                        break;
                    }
                    default :
                    {
                        BUG(0, -1);
                        break;
                    }
                }
                
                switch (element->relachement->rz_fin)
                {
                    case EF_RELACHEMENT_BLOQUE :
                    {
                        element->info_EF[j].kBz = 0;
                        break;
                    }
                    case EF_RELACHEMENT_LIBRE :
                    {
                        element->info_EF[j].kBz = MAXDOUBLE;
                        break;
                    }
                    case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
                    {
                        EF_Relachement_Donnees_Elastique_Linaire *donnees;
                        
                        donnees = element->relachement->rz_f_data;
                        if (donnees->raideur == 0.)
                            element->info_EF[j].kBz = MAXDOUBLE;
                        else
                            element->info_EF[j].kBz = 1./donnees->raideur;
                        break;
                    }
                    default :
                    {
                        BUG(0, -1);
                        break;
                    }
                }
            }
        }
        
    //     Calcul des valeurs de la matrice de rigidité locale :
        triplet = cholmod_l_allocate_triplet(12, 12, 40, 0, CHOLMOD_REAL, projet->ef_donnees.c);
        BUGMSG(triplet, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "_1992_1_1_barres_rigidite_ajout");
        ai = triplet->i;
        aj = triplet->j;
        ax = triplet->x;
        triplet->nnz = 40;
        i=0;
    //       Pour un élément travaillant en compression simple (aucune variante possible dues
    //       aux relachements) :\end{verbatim}\begin{displaymath}
    // \begin{bmatrix}  \frac{E \cdot S}{L} & -\frac{E \cdot S}{L} \\
    //                 -\frac{E \cdot S}{L} &  \frac{E \cdot S}{L}
    // \end{bmatrix}\end{displaymath}\begin{verbatim}
        ai[i] = 0;  aj[i] = 0;  ax[i] =  E*S/ll; i++;
        ai[i] = 0;  aj[i] = 6;  ax[i] = -E*S/ll; i++;
        ai[i] = 6;  aj[i] = 0;  ax[i] = -E*S/ll; i++;
        ai[i] = 6;  aj[i] = 6;  ax[i] =  E*S/ll; i++;
        
    //       Détermination de la matrice de rigidité après prise en compte des relachements
    //         autour de l'axe z :
    //         Pour cela, il convient de prendre comme modèle de base une poutre reposant sur
    //         deux appuis avec encastrement élastique, kAz et kAy, et de déterminer l'effort
    //         tranchant et le moment fléchissant au droit de chaque noeud. Les quatres cas de
    //         charge à étudier sont :
    //         1) Déplacement vertical imposé au noeud A. Les résultats sont exprimés en
    //           fontion de la valeur du déplacement.
    //         2) Rotation imposée au noeud A avec relachement au noeud A ignoré pour permettre
    //           la rotation. Les résultats sont exprimés en fonction de la rotation.
    //         3) Déplacement vertical imposé au noeud B. Les résultats sont exprimés en
    //           fontion de la valeur du déplacement.
    //         4) Rotation imposée au noeud B avec relachement au noeud B ignoré pour permettre
    //           la rotation. Les résultats sont exprimés en fonction de la rotation.
    //         Pour chaque cas, il est d'abord calculé la rotation au noeud en supposant le
    //         système isostatique (relachements remplacés par des rotules). Ensuite les
    //         moments isostatiques sont déterminés sur la base des rotations isostatiques.
    //         
    //         Etude du cas 1 :\end{verbatim}\begin{displaymath}
    //         \varphi_{A_{iso}} = arctan \left( \frac{v}{L} \right)\end{displaymath}\begin{displaymath}
    //         \varphi_{B_{iso}} = arctan \left( \frac{v}{L} \right)\end{displaymath}\begin{displaymath}
    //         M_{A_{iso}} = 0\end{displaymath}\begin{displaymath}
    //         M_{B_{iso}} = 0\end{displaymath}\begin{displaymath}
    //         b = M_{A_{hyp}} = \frac{b \cdot \varphi_{B_{iso}}+(c+k_B) \cdot \varphi_{A_{iso}}}{(a+k_A)(c+k_B)-b^2} = \varphi_{iso} \frac{b+c+k_B}{(a+k_A)(c+k_B)-b^2}\end{displaymath}\begin{verbatim}
    //         Afin de pouvoir exprimer le moment en fonction de la dénivelé, il est appliqué
    //         un développement de Taylor de arctan (hypothèse des petites déplacements).
    //         Ainsi arctan(v/L) # v/L + O(v^3)\end{verbatim}\begin{displaymath}
    //         d = M_{B_{hyp}} = \frac{b \cdot \varphi_{A_{iso}}+(a+k_A) \cdot \varphi_{B_{iso}}}{(a+k_A)(c+k_B)-b^2} = \varphi_{iso} \frac{b+a+k_A}{(a+k_A)(c+k_B)-b^2}\end{displaymath}\begin{displaymath}
    //         a = \frac{b}{L}+\frac{d}{L}\end{displaymath}\begin{displaymath}
    //         c = -\frac{b}{L}-\frac{d}{L}\end{displaymath}\begin{verbatim}
        memcpy(&infos, &(element->info_EF[j]), sizeof(infos));
        EF_calculs_moment_hyper_z(&infos, 1./ll, 1./ll, &MA, &MB);
        ai[i] = 1;  aj[i] = 1;  ax[i] = +MA/ll+MB/ll; i++;
        printf("%f\n", (+MA/ll+MB/ll)/(E*Iz/ll/ll/ll));
        ai[i] = 5;  aj[i] = 1;  ax[i] = MA;           i++;
        printf("%f\n", (MA)/(E*Iz/ll/ll));
        ai[i] = 7;  aj[i] = 1;  ax[i] = -MA/ll-MB/ll; i++;
        printf("%f\n", (-MA/ll-MB/ll)/(E*Iz/ll/ll/ll));
        ai[i] = 11; aj[i] = 1;  ax[i] = MB;           i++;
        printf("%f\n", (MB)/(E*Iz/ll/ll));
        
    //         Etude du cas 2 :\end{verbatim}\begin{displaymath}
    //         \varphi_{A_{iso}} = \frac{C \cdot L}{3 \cdot E \cdot I_z}\end{displaymath}\begin{displaymath}
    //         \varphi_{B_{iso}} = -\frac{C \cdot L}{6 \cdot E \cdot I_z}\end{displaymath}\begin{displaymath}
    //         M_{A_{iso}} = C\end{displaymath}\begin{displaymath}
    //         M_{B_{iso}} = 0\end{displaymath}\begin{displaymath}
    //         M_{A_{hyp}} = 0\end{displaymath}\begin{displaymath}
    //         M_{B_{hyp}} = \frac{b \cdot \varphi_{A_{iso}}+(a+k_A) \cdot \varphi_{B_{iso}}}{(a+k_A)(c+k_B)-b^2} =  \varphi_{A_{iso}} \cdot \frac{b-(a+k_A)/2}{(a+k_A)(c+k_B)-b^2} = \frac{C \cdot L}{3 \cdot E \cdot I_z} \cdot \frac{b-(a+k_A)/2}{(a+k_A)(c+k_B)-b^2}\end{displaymath}\begin{displaymath}
    //         M_{B_{hyp}} = \lim_{k_A\rightarrow \infty} \frac{C \cdot L}{3 \cdot E \cdot I_z} \cdot \frac{b-(a+k_A)/2}{(a+k_A)(c+k_B)-b^2} = -\frac{C \cdot L}{6 \cdot E \cdot I_z \cdot (c+k_B)}\end{displaymath}\begin{displaymath}
    //         \phi_A = \varphi_{A_{iso}} + b \cdot M_{B_{hyp}} = \frac{C \cdot L}{3 \cdot E \cdot I_z} - b \cdot \frac{C \cdot L}{6 \cdot E \cdot I_z \cdot (c+k_B)} = \frac{C \cdot L}{3 \cdot E \cdot I_z} \left( 1 - \frac{b}{2 \cdot (c+k_B)} \right)\end{displaymath}\begin{displaymath}
    //         f = M_{A_{iso}} = C = \frac{3 \cdot E \cdot I_z}{L \cdot \left( 1-\frac{b}{2\cdot(c+k_B)} \right)} \cdot \phi_A \end{displaymath}\begin{displaymath}
    //         h = M_{B_{hyp}} = -\frac{C \cdot L}{6 \cdot E \cdot I_z \cdot (c+k_B)}\end{displaymath}\begin{displaymath}
    //         e = \frac{f}{L}+\frac{h}{L}\end{displaymath}\begin{displaymath}
    //         g = -\frac{f}{L}-\frac{h}{L}\end{displaymath}\begin{verbatim}
        memcpy(&infos, &(element->info_EF[j]), sizeof(infos));
        infos.kAz = MAXDOUBLE;
        if (ERREUR_RELATIVE_EGALE(infos.kBz, MAXDOUBLE))
        {
            MA = 3*E*Iz/ll;
            MB = 0.;
        }
        else
        {
            MA = 3*E*Iz/ll/(1-infos.bz/(2*(infos.cz+infos.kBz)));
            MB = MA*ll/(6*E*Iz*(infos.cz+infos.kBz));
        }
        ai[i] = 1;  aj[i] = 5;  ax[i] = MA/ll+MB/ll;  i++;
        printf("%f\n", (MA/ll+MB/ll)/(E*Iz/ll/ll));
        ai[i] = 5;  aj[i] = 5;  ax[i] = MA;           i++;
        printf("%f\n", (MA)/(E*Iz/ll));
        ai[i] = 7;  aj[i] = 5;  ax[i] = -MA/ll-MB/ll; i++;
        printf("%f\n", (-MA/ll-MB/ll)/(E*Iz/ll/ll));
        ai[i] = 11; aj[i] = 5;  ax[i] = MB;           i++;
        printf("%f\n", (MB)/(E*Iz/ll));
        
    //         Etude du cas 3 :\end{verbatim}\begin{displaymath}
    //         \varphi_{A_{iso}} = -arctan \left( \frac{v}{L} \right)\end{displaymath}\begin{displaymath}
    //         \varphi_{B_{iso}} = -arctan \left( \frac{v}{L} \right)\end{displaymath}\begin{displaymath}
    //         M_{A_{iso}} = 0\end{displaymath}\begin{displaymath}
    //         M_{B_{iso}} = 0\end{displaymath}\begin{displaymath}
    //         j = M_{A_{hyp}} = \frac{b \cdot \varphi_{B_{iso}}+(c+k_B) \cdot \varphi_{A_{iso}}}{(a+k_A)(c+k_B)-b^2} = \varphi_{iso} \frac{b+c+k_B}{(a+k_A)(c+k_B)-b^2}\end{displaymath}\begin{verbatim}
    //         Afin de pouvoir exprimer le moment en fonction de la dénivelé, il est appliqué
    //         un développement de Taylor de arctan (hypothèse des petites déplacements).
    //         Ainsi -arctan(v/L) # -v/L + O(v^3)\end{verbatim}\begin{displaymath}
    //         l = M_{B_{hyp}} = \frac{b \cdot \varphi_{A_{iso}}+(a+k_A) \cdot \varphi_{B_{iso}}}{(a+k_A)(c+k_B)-b^2} = \varphi_{iso} \frac{b+a+k_A}{(a+k_A)(c+k_B)-b^2}\end{displaymath}\begin{displaymath}
    //         i = \frac{j}{L}+\frac{l}{L}\end{displaymath}\begin{displaymath}
    //         k = -\frac{j}{L}-\frac{l}{L}\end{displaymath}\begin{verbatim}
        memcpy(&infos, &(element->info_EF[j]), sizeof(infos));
        EF_calculs_moment_hyper_z(&infos, -1./ll, -1./ll, &MA, &MB);
        ai[i] = 1;  aj[i] = 7;  ax[i] =  MA/ll+MB/ll; i++;
        printf("%f\n", (MA/ll+MB/ll)/(E*Iz/ll/ll/ll));
        ai[i] = 5;  aj[i] = 7;  ax[i] =  MA;          i++;
        printf("%f\n", (MA)/(E*Iz/ll/ll));
        ai[i] = 7;  aj[i] = 7;  ax[i] = -MA/ll-MB/ll; i++;
        printf("%f\n", (-MA/ll-MB/ll)/(E*Iz/ll/ll/ll));
        ai[i] = 11; aj[i] = 7;  ax[i] =  MB;          i++;
        printf("%f\n", (MB)/(E*Iz/ll/ll));
        
    //         Etude du cas 4 :\end{verbatim}\begin{displaymath}
    //         \varphi_{A_{iso}} = -\frac{C \cdot L}{6 \cdot E \cdot I_z}\end{displaymath}\begin{displaymath}
    //         \varphi_{B_{iso}} = \frac{C \cdot L}{3 \cdot E \cdot I_z}\end{displaymath}\begin{displaymath}
    //         M_{A_{iso}} = 0\end{displaymath}\begin{displaymath}
    //         M_{B_{iso}} = C\end{displaymath}\begin{displaymath}
    //         M_{A_{hyp}} = \frac{b \cdot \varphi_{B_{iso}}+(c+k_B) \cdot \varphi_{A_{iso}}}{(a+k_A)(c+k_B)-b^2} =  \varphi_{B_{iso}} \cdot \frac{b-(c+k_B)/2}{(a+k_A)(c+k_B)-b^2} = \frac{C \cdot L}{3 \cdot E \cdot I_z} \cdot \frac{b-(c+k_B)/2}{(a+k_A)(c+k_B)-b^2}\end{displaymath}\begin{displaymath}
    //         M_{A_{hyp}} = \lim_{k_B\rightarrow \infty} \frac{C \cdot L}{3 \cdot E \cdot I_z} \cdot \frac{b-(c+k_B)/2}{(a+k_A)(c+k_B)-b^2} = -\frac{C \cdot L}{6 \cdot E \cdot I_z \cdot (a+k_A)}\end{displaymath}\begin{displaymath}
    //         M_{B_{hyp}} = 0\end{displaymath}\begin{displaymath}
    //         \phi_B = \varphi_{B_{iso}} + b \cdot M_{A_{hyp}} = \frac{C \cdot L}{3 \cdot E \cdot I_z} - b \cdot \frac{C \cdot L}{6 \cdot E \cdot I_z \cdot (a+k_A)} = \frac{C \cdot L}{3 \cdot E \cdot I_z} \left( 1 - \frac{b}{2 \cdot (a+k_A)} \right)\end{displaymath}\begin{displaymath}
    //         p = M_{B_{iso}} = C = \frac{3 \cdot E \cdot I_z}{L \cdot \left( 1-\frac{b}{2\cdot(a+k_A)} \right)} \cdot \phi_B\end{displaymath}\begin{displaymath}
    //         n = M_{A_{hyp}} = -\frac{C \cdot L}{6 \cdot E \cdot I_z \cdot (a+k_A)}\end{displaymath}\begin{displaymath}
    //         m = \frac{n}{L}+\frac{p}{L}\end{displaymath}\begin{displaymath}
    //         o = -\frac{n}{L}-\frac{p}{L}\end{displaymath}\begin{verbatim}
    //         L'ensemble des valeurs sont à inséré dans la matrice suivante et permet
    //         d'obtenir la matrice de rigidité élémentaire.\end{verbatim}\begin{displaymath}
        memcpy(&infos, &(element->info_EF[j]), sizeof(infos));
        infos.kBz = MAXDOUBLE;
        if (ERREUR_RELATIVE_EGALE(infos.kAz, MAXDOUBLE))
        {
            /* MA et MB sont bien ici inversés ;) */
            MB = 3*E*Iz/ll;
            MA = 0.;
        }
        else
        {
            MB = 3*E*Iz/ll/(1-infos.bz/(2*(infos.az+infos.kAz)));
            MA = MB*ll/(6*E*Iz*(infos.az+infos.kAz));
        }
        ai[i] = 1;  aj[i] = 11; ax[i] =  MA/ll+MB/ll; i++;
        printf("%f\n", (MA/ll+MB/ll)/(E*Iz/ll/ll));
        ai[i] = 5;  aj[i] = 11; ax[i] =  MA;          i++;
        printf("%f\n", (MA)/(E*Iz/ll));
        ai[i] = 7;  aj[i] = 11; ax[i] = -MA/ll-MB/ll; i++;
        printf("%f\n", (-MA/ll-MB/ll)/(E*Iz/ll/ll));
        ai[i] = 11; aj[i] = 11; ax[i] =  MB;          i++;
        printf("%f\n", (MB)/(E*Iz/ll));
    // \begin{bmatrix}K_e\end{bmatrix} = 
    // \begin{bmatrix}  a & e & i & m\\
    //                  b & f & j & n\\
    //                  c & g & k & o\\
    //                  d & h & l & p
    // \end{bmatrix}\end{displaymath}\begin{verbatim}
        
    //       Détermination de la matrice de rigidité après prise en compte des relachements
    //         autour de l'axe y. Cette matrice de rigidité se calcule exactement de la même
    //         façon que la matrice précédente. Il suffit de remplacer les relachements kAz et
    //         kBz par kAy et lBy, les inerties Iz par Iy et les coefficients de souplesse az,
    //         bz et cz par ay, by et cy. Le changement repère entraine également un changement
    //         de signe des coefficients de la matrice a, c, e, g, i, k, m et o.
        memcpy(&infos, &(element->info_EF[j]), sizeof(infos));
        EF_calculs_moment_hyper_y(&infos, 1./ll, 1./ll, &MA, &MB);
        ai[i] = 2;  aj[i] = 2;  ax[i] = -MA/ll-MB/ll; i++;
        printf("%f\n", (-MA/ll-MB/ll)/(E*Iy/ll/ll/ll));
        ai[i] = 4;  aj[i] = 2;  ax[i] = MA;           i++;
        printf("%f\n", (MA)/(E*Iy/ll/ll));
        ai[i] = 8;  aj[i] = 2;  ax[i] = +MA/ll+MB/ll; i++;
        printf("%f\n", (+MA/ll+MB/ll)/(E*Iy/ll/ll/ll));
        ai[i] = 10; aj[i] = 2;  ax[i] = MB;           i++;
        printf("%f\n", (MB)/(E*Iy/ll/ll));
        
        memcpy(&infos, &(element->info_EF[j]), sizeof(infos));
        infos.kAy = MAXDOUBLE;
        if (ERREUR_RELATIVE_EGALE(infos.kBy, MAXDOUBLE))
        {
            MA = 3*E*Iy/ll;
            MB = 0.;
        }
        else
        {
            MA = 3*E*Iy/ll/(1-infos.by/(2*(infos.cy+infos.kBy)));
            MB = MA*ll/(6*E*Iy*(infos.cy+infos.kBy));
        }
        ai[i] = 2;  aj[i] = 4;  ax[i] = -MA/ll-MB/ll; i++;
        printf("%f\n", (-MA/ll-MB/ll)/(E*Iy/ll/ll));
        ai[i] = 4;  aj[i] = 4;  ax[i] = MA;           i++;
        printf("%f\n", (MA)/(E*Iy/ll));
        ai[i] = 8;  aj[i] = 4;  ax[i] = +MA/ll+MB/ll; i++;
        printf("%f\n", (+MA/ll+MB/ll)/(E*Iy/ll/ll));
        ai[i] = 10; aj[i] = 4;  ax[i] = MB;           i++;
        printf("%f\n", (MB)/(E*Iy/ll));
        
        memcpy(&infos, &(element->info_EF[j]), sizeof(infos));
        EF_calculs_moment_hyper_y(&infos, -1./ll, -1./ll, &MA, &MB);
        ai[i] = 2;  aj[i] = 8;  ax[i] = -MA/ll-MB/ll; i++;
        printf("%f\n", (-MA/ll-MB/ll)/(E*Iy/ll/ll/ll));
        ai[i] = 4;  aj[i] = 8;  ax[i] =  MA;          i++;
        printf("%f\n", (MA)/(E*Iy/ll/ll));
        ai[i] = 8;  aj[i] = 8;  ax[i] = +MA/ll+MB/ll; i++;
        printf("%f\n", (+MA/ll+MB/ll)/(E*Iy/ll/ll/ll));
        ai[i] = 10; aj[i] = 8;  ax[i] =  MB;          i++;
        printf("%f\n", (MB)/(E*Iy/ll/ll));
        
        memcpy(&infos, &(element->info_EF[j]), sizeof(infos));
        infos.kBy = MAXDOUBLE;
        if (ERREUR_RELATIVE_EGALE(infos.kAy, MAXDOUBLE))
        {
            /* MA et MB sont bien ici inversés ;) */
            MB = 3*E*Iy/ll;
            MA = 0.;
        }
        else
        {
            MB = 3*E*Iy/ll/(1-infos.by/(2*(infos.ay+infos.kAy)));
            MA = MB*ll/(6*E*Iy*(infos.ay+infos.kAy));
        }
        ai[i] = 2;  aj[i] = 10; ax[i] = -MA/ll-MB/ll; i++;
        printf("%f\n", (-MA/ll-MB/ll)/(E*Iy/ll/ll));
        ai[i] = 4;  aj[i] = 10; ax[i] = MA;           i++;
        printf("%f\n", (MA)/(E*Iy/ll));
        ai[i] = 8;  aj[i] = 10; ax[i] = MA/ll+MB/ll;  i++;
        printf("%f\n", (MA/ll+MB/ll)/(E*Iy/ll/ll));
        ai[i] = 10; aj[i] = 10; ax[i] = MB;           i++;
        printf("%f\n", (MB)/(E*Iy/ll));
        
    //       Pour un élément travaillant en torsion simple dont l'une des extrémités est
    //         relaxée :\end{verbatim}\begin{displaymath}
    // \begin{bmatrix}            0        &           0          \\
    //                            0        &           0          
    // \end{bmatrix}\end{displaymath}\begin{verbatim}
        if (((j == 0) && (element->relachement != NULL) && (element->relachement->rx_debut == EF_RELACHEMENT_LIBRE)) || ((j==element->discretisation_element) && (element->relachement != NULL) && (element->relachement->rx_fin == EF_RELACHEMENT_LIBRE)))
        {
            ai[i] = 3;  aj[i] = 3;  ax[i] = 0; i++;
            ai[i] = 3;  aj[i] = 9;  ax[i] = 0; i++;
            ai[i] = 9;  aj[i] = 3;  ax[i] = 0; i++;
            ai[i] = 9;  aj[i] = 9;  ax[i] = 0; i++;
        }
    //       Pour un élément travaillant en torsion simple dont aucune des extrémités n'est
    //         relaxée :\end{verbatim}\begin{displaymath}
    // \begin{bmatrix}  \frac{G \cdot J}{L} & -\frac{G \cdot J}{L} \\
    //                 -\frac{G \cdot J}{L} &  \frac{G \cdot J}{L}
    // \end{bmatrix}\end{displaymath}\begin{verbatim}
        else
        {
            ai[i] = 3;  aj[i] = 3;  ax[i] = G*J/ll;  i++;
            ai[i] = 3;  aj[i] = 9;  ax[i] = -G*J/ll; i++;
            ai[i] = 9;  aj[i] = 3;  ax[i] = -G*J/ll; i++;
            ai[i] = 9;  aj[i] = 9;  ax[i] = G*J/ll;  i++;
        }
        
        element->info_EF[j].matrice_rigidite_locale = cholmod_l_triplet_to_sparse(triplet, 0, projet->ef_donnees.c);
        BUGMSG(element->info_EF[j].matrice_rigidite_locale, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "_1992_1_1_barres_rigidite_ajout");
        cholmod_l_free_triplet(&triplet, projet->ef_donnees.c);
        
    //       Calcul la matrice locale dans le repère globale :\end{verbatim}\begin{displaymath}
    //       [K]_{global} = [R] \cdot [K]_{local} \cdot [R]^{-1} = [R] \cdot [K]_{local} \cdot [R]^T\end{displaymath}\begin{verbatim}
        sparse_tmp = cholmod_l_ssmult(element->matrice_rotation, element->info_EF[j].matrice_rigidite_locale, 0, 1, 0, projet->ef_donnees.c);
        BUGMSG(sparse_tmp, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "_1992_1_1_barres_rigidite_ajout");
        matrice_rigidite_globale = cholmod_l_ssmult(sparse_tmp, element->matrice_rotation_transpose, 0, 1, 0, projet->ef_donnees.c);
        BUGMSG(matrice_rigidite_globale, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "_1992_1_1_barres_rigidite_ajout");
        cholmod_l_free_sparse(&(sparse_tmp), projet->ef_donnees.c);
        triplet = cholmod_l_sparse_to_triplet(matrice_rigidite_globale, projet->ef_donnees.c);
        BUGMSG(triplet, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "_1992_1_1_barres_rigidite_ajout");
        ai = triplet->i;
        aj = triplet->j;
        ax = triplet->x;
        ai2 = projet->ef_donnees.triplet_rigidite_partielle->i;
        aj2 = projet->ef_donnees.triplet_rigidite_partielle->j;
        ax2 = projet->ef_donnees.triplet_rigidite_partielle->x;
        ai3 = projet->ef_donnees.triplet_rigidite_complete->i;
        aj3 = projet->ef_donnees.triplet_rigidite_complete->j;
        ax3 = projet->ef_donnees.triplet_rigidite_complete->x;
        
    //       Insertion de la matrice de rigidité élémentaire dans la matrice de rigidité
    //         globale partielle et complète.
        for (i=0;i<triplet->nnz;i++)
        {
            if ((ai[i] < 6) && (aj[i] < 6) && (projet->ef_donnees.noeuds_pos_partielle[noeud1->numero][ai[i]] != -1) && (projet->ef_donnees.noeuds_pos_partielle[noeud1->numero][aj[i]] != -1))
            {
                ai2[projet->ef_donnees.triplet_rigidite_partielle_en_cours] = projet->ef_donnees.noeuds_pos_partielle[noeud1->numero][ai[i]];
                aj2[projet->ef_donnees.triplet_rigidite_partielle_en_cours] = projet->ef_donnees.noeuds_pos_partielle[noeud1->numero][aj[i]];
                ax2[projet->ef_donnees.triplet_rigidite_partielle_en_cours] = ax[i];
                projet->ef_donnees.triplet_rigidite_partielle_en_cours++;
            }
            else if ((ai[i] < 6) && (aj[i] >= 6) && (projet->ef_donnees.noeuds_pos_partielle[noeud1->numero][ai[i]] != -1) && (projet->ef_donnees.noeuds_pos_partielle[noeud2->numero][aj[i]-6] != -1))
            {
                ai2[projet->ef_donnees.triplet_rigidite_partielle_en_cours] = projet->ef_donnees.noeuds_pos_partielle[noeud1->numero][ai[i]];
                aj2[projet->ef_donnees.triplet_rigidite_partielle_en_cours] = projet->ef_donnees.noeuds_pos_partielle[noeud2->numero][aj[i]-6];
                ax2[projet->ef_donnees.triplet_rigidite_partielle_en_cours] = ax[i];
                projet->ef_donnees.triplet_rigidite_partielle_en_cours++;
            }
            else if ((ai[i] >= 6) && (aj[i] < 6) && (projet->ef_donnees.noeuds_pos_partielle[noeud2->numero][ai[i]-6] != -1) && (projet->ef_donnees.noeuds_pos_partielle[noeud1->numero][aj[i]] != -1))
            {
                ai2[projet->ef_donnees.triplet_rigidite_partielle_en_cours] = projet->ef_donnees.noeuds_pos_partielle[noeud2->numero][ai[i]-6];
                aj2[projet->ef_donnees.triplet_rigidite_partielle_en_cours] = projet->ef_donnees.noeuds_pos_partielle[noeud1->numero][aj[i]];
                ax2[projet->ef_donnees.triplet_rigidite_partielle_en_cours] = ax[i];
                projet->ef_donnees.triplet_rigidite_partielle_en_cours++;
            }
            else if ((ai[i] >= 6) && (aj[i] >= 6) && (projet->ef_donnees.noeuds_pos_partielle[noeud2->numero][ai[i]-6] != -1) && (projet->ef_donnees.noeuds_pos_partielle[noeud2->numero][aj[i]-6] != -1))
            {
                ai2[projet->ef_donnees.triplet_rigidite_partielle_en_cours] = projet->ef_donnees.noeuds_pos_partielle[noeud2->numero][ai[i]-6];
                aj2[projet->ef_donnees.triplet_rigidite_partielle_en_cours] = projet->ef_donnees.noeuds_pos_partielle[noeud2->numero][aj[i]-6];
                ax2[projet->ef_donnees.triplet_rigidite_partielle_en_cours] = ax[i];
                projet->ef_donnees.triplet_rigidite_partielle_en_cours++;
            }
            
            if ((ai[i] < 6) && (aj[i] < 6))
            {
                ai3[projet->ef_donnees.triplet_rigidite_complete_en_cours] = projet->ef_donnees.noeuds_pos_complete[noeud1->numero][ai[i]];
                aj3[projet->ef_donnees.triplet_rigidite_complete_en_cours] = projet->ef_donnees.noeuds_pos_complete[noeud1->numero][aj[i]];
                ax3[projet->ef_donnees.triplet_rigidite_complete_en_cours] = ax[i];
                projet->ef_donnees.triplet_rigidite_complete_en_cours++;
            }
            else if ((ai[i] < 6) && (aj[i] >= 6))
            {
                ai3[projet->ef_donnees.triplet_rigidite_complete_en_cours] = projet->ef_donnees.noeuds_pos_complete[noeud1->numero][ai[i]];
                aj3[projet->ef_donnees.triplet_rigidite_complete_en_cours] = projet->ef_donnees.noeuds_pos_complete[noeud2->numero][aj[i]-6];
                ax3[projet->ef_donnees.triplet_rigidite_complete_en_cours] = ax[i];
                projet->ef_donnees.triplet_rigidite_complete_en_cours++;
            }
            else if ((ai[i] >= 6) && (aj[i] < 6))
            {
                ai3[projet->ef_donnees.triplet_rigidite_complete_en_cours] = projet->ef_donnees.noeuds_pos_complete[noeud2->numero][ai[i]-6];
                aj3[projet->ef_donnees.triplet_rigidite_complete_en_cours] = projet->ef_donnees.noeuds_pos_complete[noeud1->numero][aj[i]];
                ax3[projet->ef_donnees.triplet_rigidite_complete_en_cours] = ax[i];
                projet->ef_donnees.triplet_rigidite_complete_en_cours++;
            }
            else if ((ai[i] >= 6) && (aj[i] >= 6))
            {
                ai3[projet->ef_donnees.triplet_rigidite_complete_en_cours] = projet->ef_donnees.noeuds_pos_complete[noeud2->numero][ai[i]-6];
                aj3[projet->ef_donnees.triplet_rigidite_complete_en_cours] = projet->ef_donnees.noeuds_pos_complete[noeud2->numero][aj[i]-6];
                ax3[projet->ef_donnees.triplet_rigidite_complete_en_cours] = ax[i];
                projet->ef_donnees.triplet_rigidite_complete_en_cours++;
            }
        }
        cholmod_l_free_triplet(&triplet, projet->ef_donnees.c);
        cholmod_l_free_sparse(&(matrice_rigidite_globale), projet->ef_donnees.c);
    }
    // FinPour
    
    return 0;
}


int _1992_1_1_barres_rigidite_ajout_tout(Projet *projet)
/* Description : Ajout à la matrice de rigidité tous les éléments en béton
 * Paramètres : Projet *projet : la variable projet
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet == NULL) ou
 *             (projet->beton.barres == NULL)
 */
{
    BUGMSG(projet, -1, "_1992_1_1_barres_rigidite_ajout_tout\n");
    BUGMSG(projet->beton.barres, -1, "_1992_1_1_barres_rigidite_ajout_tout\n");
    
    // Trivial
    if (list_size(projet->beton.barres) == 0)
        return 0;
    
    list_mvfront(projet->beton.barres);
    do
    {
        Beton_Barre *element = list_curr(projet->beton.barres);
        
        BUG(_1992_1_1_barres_rigidite_ajout(projet, element) == 0, -2);
    }
    while (list_mvnext(projet->beton.barres) != NULL);
    
    return 0;
}


int _1992_1_1_barres_free(Projet *projet)
/* Description : Libère l'ensemble des éléments  en béton
 * Paramètres : Projet *projet : la variable projet
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet == NULL) ou
 *             (projet->beton.barres == NULL)
 */
{
    unsigned int     i;
    
    // Trivial
    BUGMSG(projet, -1, "_1992_1_1_barres_free\n");
    BUGMSG(projet->beton.barres, -1, "_1992_1_1_barres_free\n");
    
    while (!list_empty(projet->beton.barres))
    {
        Beton_Barre *element = list_remove_front(projet->beton.barres);
        
        free(element->noeuds_intermediaires);
        cholmod_l_free_sparse(&(element->matrice_rotation), projet->ef_donnees.c);
        cholmod_l_free_sparse(&(element->matrice_rotation_transpose), projet->ef_donnees.c);
        for (i=0;i<=element->discretisation_element;i++)
            cholmod_l_free_sparse(&(element->info_EF[i].matrice_rigidite_locale), projet->ef_donnees.c);
        free(element->info_EF);
        
        free(element);
    }
    
    free(projet->beton.barres);
    projet->beton.barres = NULL;
    
    return 0;
}
