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
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <values.h>
#include <math.h>

#include "common_projet.h"
#include "common_erreurs.h"
#include "common_maths.h"
#include "common_fonction.h"
#include "EF_noeud.h"
#include "EF_charge_noeud.h"
#include "EF_charge_barre_ponctuelle.h"
#include "1990_actions.h"
#include "1992_1_1_barres.h"
#include "1992_1_1_section.h"

int EF_calculs_initialise(Projet *projet)
/* Description : Initialise les diverses variables nécessaires à l'ajout des matrices de
 *               rigidité élémentaires.
 * Paramètres : Projet *projet : la variable projet
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet == NULL) ou
 *             (projet->ef_donnees.noeuds == NULL) ou
 *             (list_size(projet->ef_donnees.noeuds) == 0) ou
 *             (projet->beton.barres == NULL) ou
 *             (list_size(projet->beton.barres) == 0)
 *           -2 en cas d'erreur d'allocation mémoire
 */
{
    unsigned int    i, nnz_max, nb_col_partielle, nb_col_complete;
    
    BUGMSG(projet, -1, "EF_calculs_initialise\n");
    BUGMSG(projet->ef_donnees.noeuds, -1, "EF_calculs_initialise\n");
    BUGMSG(list_size(projet->ef_donnees.noeuds), -1, "EF_calculs_initialise\n");
    BUGMSG(projet->beton.barres, -1, "EF_calculs_initialise\n");
    BUGMSG(list_size(projet->beton.barres), -1, "EF_calculs_initialise\n");
    
    // Allocation de la mémoire nécessaire pour contenir la position de chaque degré de
    //   liberté des noeuds (via noeuds_pos_partielle et noeuds_pos_complete) dans la matrice
    //   de rigidité globale partielle et complète.
    projet->ef_donnees.noeuds_pos_partielle = (int**)malloc(sizeof(int*)*list_size(projet->ef_donnees.noeuds));
    BUGMSG(projet->ef_donnees.noeuds_pos_partielle, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_initialise");
    for (i=0;i<list_size(projet->ef_donnees.noeuds);i++)
    {
        projet->ef_donnees.noeuds_pos_partielle[i] = (int*)malloc(6*sizeof(int));
        BUGMSG(projet->ef_donnees.noeuds_pos_partielle[i], -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_initialise");
    }
    projet->ef_donnees.noeuds_pos_complete = (int**)malloc(sizeof(int*)*list_size(projet->ef_donnees.noeuds));
    BUGMSG(projet->ef_donnees.noeuds_pos_complete, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_initialise");
    for (i=0;i<list_size(projet->ef_donnees.noeuds);i++)
    {
        projet->ef_donnees.noeuds_pos_complete[i] = (int*)malloc(6*sizeof(int));
        BUGMSG(projet->ef_donnees.noeuds_pos_complete[i], -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_initialise");
    }
    
    // Détermination du nombre de colonnes pour la matrice de rigidité complète et partielle :
    // nb_col_partielle = 0.
    // nb_col_complete = 0.
    // Pour chaque noeud, y compris ceux dû à la discrétisation
    //     Pour chaque degré de liberté du noeud (x, y, z, rx, ry, rz)
    //         noeuds_pos_complete[numero_noeud][degre] = nb_col_complete.
    //         nb_col_complete = nb_col_complete + 1.
    //         Si le degré de liberté de liberté est libre Alors
    //             noeuds_pos_partielle[numero_noeud][degre] = nb_col_partielle.
    //             nb_col_partielle = nb_col_partielle + 1.
    //         Sinon
    //             noeuds_pos_partielle[numero_noeud][degre] = -1.
    //         FinSi
    //     FinPour
    // FinPour
    nb_col_partielle = 0;
    nb_col_complete = 0;
    list_mvfront(projet->ef_donnees.noeuds);
    do
    {
        EF_Noeud    *noeud = list_curr(projet->ef_donnees.noeuds);
        
        projet->ef_donnees.noeuds_pos_complete[noeud->numero][0] = nb_col_complete; nb_col_complete++;
        projet->ef_donnees.noeuds_pos_complete[noeud->numero][1] = nb_col_complete; nb_col_complete++;
        projet->ef_donnees.noeuds_pos_complete[noeud->numero][2] = nb_col_complete; nb_col_complete++;
        projet->ef_donnees.noeuds_pos_complete[noeud->numero][3] = nb_col_complete; nb_col_complete++;
        projet->ef_donnees.noeuds_pos_complete[noeud->numero][4] = nb_col_complete; nb_col_complete++;
        projet->ef_donnees.noeuds_pos_complete[noeud->numero][5] = nb_col_complete; nb_col_complete++;
        
        if (noeud->appui == NULL)
        {
            projet->ef_donnees.noeuds_pos_partielle[noeud->numero][0] = nb_col_partielle; nb_col_partielle++;
            projet->ef_donnees.noeuds_pos_partielle[noeud->numero][1] = nb_col_partielle; nb_col_partielle++;
            projet->ef_donnees.noeuds_pos_partielle[noeud->numero][2] = nb_col_partielle; nb_col_partielle++;
            projet->ef_donnees.noeuds_pos_partielle[noeud->numero][3] = nb_col_partielle; nb_col_partielle++;
            projet->ef_donnees.noeuds_pos_partielle[noeud->numero][4] = nb_col_partielle; nb_col_partielle++;
            projet->ef_donnees.noeuds_pos_partielle[noeud->numero][5] = nb_col_partielle; nb_col_partielle++;
        }
        else
        {
            EF_Appui    *appui = noeud->appui;
            if (appui->x == EF_APPUI_LIBRE)
                { projet->ef_donnees.noeuds_pos_partielle[noeud->numero][0] = nb_col_partielle; nb_col_partielle++; }
            else projet->ef_donnees.noeuds_pos_partielle[noeud->numero][0] = -1;
            if (appui->y == EF_APPUI_LIBRE)
                { projet->ef_donnees.noeuds_pos_partielle[noeud->numero][1] = nb_col_partielle; nb_col_partielle++; }
            else projet->ef_donnees.noeuds_pos_partielle[noeud->numero][1] = -1;
            if (appui->z == EF_APPUI_LIBRE)
                { projet->ef_donnees.noeuds_pos_partielle[noeud->numero][2] = nb_col_partielle; nb_col_partielle++; }
            else projet->ef_donnees.noeuds_pos_partielle[noeud->numero][2] = -1;
            if (appui->rx == EF_APPUI_LIBRE)
                { projet->ef_donnees.noeuds_pos_partielle[noeud->numero][3] = nb_col_partielle; nb_col_partielle++; }
            else projet->ef_donnees.noeuds_pos_partielle[noeud->numero][3] = -1;
            if (appui->ry == EF_APPUI_LIBRE)
                { projet->ef_donnees.noeuds_pos_partielle[noeud->numero][4] = nb_col_partielle; nb_col_partielle++; }
            else projet->ef_donnees.noeuds_pos_partielle[noeud->numero][4] = -1;
            if (appui->rz == EF_APPUI_LIBRE)
                { projet->ef_donnees.noeuds_pos_partielle[noeud->numero][5] = nb_col_partielle; nb_col_partielle++; }
            else projet->ef_donnees.noeuds_pos_partielle[noeud->numero][5] = -1;
        }
    }
    while (list_mvnext(projet->ef_donnees.noeuds) != NULL);
    
    // Détermination du nombre de matrices de rigidité globale élémentaire du système
    //   (y compris la discrétisation).
    // Détermination du nombre de triplets, soit 12*12*nombre_de_matrices.
    nnz_max = 0;
    list_mvfront(projet->beton.barres);
    do
    {
        Beton_Barre   *element = list_curr(projet->beton.barres);
        
        nnz_max += 12*12*(element->discretisation_element+1);
    }
    while (list_mvnext(projet->beton.barres) != NULL);
    
    // Allocation des triplets de la matrice de rigidité partielle (triplet_rigidite_partielle)
    //   et la matrice de rigidité globale (triplet_rigidite_globale).
    projet->ef_donnees.triplet_rigidite_partielle = cholmod_l_allocate_triplet(nb_col_partielle, nb_col_partielle, nnz_max, 0, CHOLMOD_REAL, projet->ef_donnees.c);
    BUGMSG(projet->ef_donnees.triplet_rigidite_partielle, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_initialise");
    projet->ef_donnees.triplet_rigidite_partielle->nnz = nnz_max;
    projet->ef_donnees.triplet_rigidite_complete = cholmod_l_allocate_triplet(nb_col_complete, nb_col_complete, nnz_max, 0, CHOLMOD_REAL, projet->ef_donnees.c);
    BUGMSG(projet->ef_donnees.triplet_rigidite_complete, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_initialise");
    projet->ef_donnees.triplet_rigidite_complete->nnz = nnz_max;
    
    // Initialisation de l'indice du triplet en cours à 0 pour la matrice de rigidité partielle
    //   (triplet_rigidite_partielle_en_cours) et globale (triplet_rigidite_complete_en_cours).
    projet->ef_donnees.triplet_rigidite_partielle_en_cours = 0;
    projet->ef_donnees.triplet_rigidite_complete_en_cours = 0;
    
    return 0;
}


int EF_calculs_genere_mat_rig(Projet *projet)
/* Description : Convertion des triplets contenant la matrice de rigidité 
 *               partielle et complète sous forme d'une matrice sparse et
 *               factorisation de la matrice partielle.
 * Paramètres : Projet *projet : la variable projet
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet == NULL) ou
 *             (projet->ef_donnees.triplet_rigidite_partielle == NULL) ou
 *             (projet->ef_donnees.triplet_rigidite_complete == NULL)
 *           -2 en cas d'erreur d'allocation mémoire
 */
{
    unsigned int        i;
    long                *ai, *aj;
    double              *ax;
    
    BUGMSG(projet, -1, "EF_calculs_genere_mat_rig\n");
    BUGMSG(projet->ef_donnees.triplet_rigidite_partielle, -1, "EF_calculs_genere_mat_rig\n");
    BUGMSG(projet->ef_donnees.triplet_rigidite_complete, -1, "EF_calculs_genere_mat_rig\n");
    
    ai = projet->ef_donnees.triplet_rigidite_partielle->i;
    aj = projet->ef_donnees.triplet_rigidite_partielle->j;
    ax = projet->ef_donnees.triplet_rigidite_partielle->x;
    /* On initialise à 0 les valeurs non utilisée dans le triplet rigidite partiel. */
    for(i=projet->ef_donnees.triplet_rigidite_partielle_en_cours;i<projet->ef_donnees.triplet_rigidite_partielle->nzmax;i++)
    {
        ai[i] = 0;
        aj[i] = 0;
        ax[i] = 0.;
    }
        
    // Si le nombre de lignes du triplet rigidité partielle == 0, cela signifie que tous les
    //   noeuds sont bloqués (cas d'une poutre sur deux appuis sans discrétisation) Alors
    //     Initialisation d'un matrice de rigidité partielle vide.
    //     Convertion du triplet de rigidité complète en matrice.
    //     Factorisation de la matrice de rigidité partielle vide.
    //     Fin.
    // FinSi
    if (projet->ef_donnees.triplet_rigidite_partielle->nrow == 0)
    {
        cholmod_triplet     *triplet_rigidite;
        
        triplet_rigidite = cholmod_l_allocate_triplet(0, 0, 0, 0, CHOLMOD_REAL, projet->ef_donnees.c);
        BUGMSG(triplet_rigidite, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_genere_mat_rig");
        projet->ef_donnees.rigidite_matrice_partielle = cholmod_l_triplet_to_sparse(triplet_rigidite, 0, projet->ef_donnees.c);
        BUGMSG(projet->ef_donnees.rigidite_matrice_partielle, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_genere_mat_rig");
        projet->ef_donnees.rigidite_matrice_partielle->stype = 0;
        projet->ef_donnees.rigidite_matrice_complete = cholmod_l_triplet_to_sparse(projet->ef_donnees.triplet_rigidite_complete, 0, projet->ef_donnees.c);
        BUGMSG(projet->ef_donnees.rigidite_matrice_complete, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_genere_mat_rig");
/*        fhri    l
        projet->ef_donnees.QR = SuiteSparseQR_C_factorize(0, 0., projet->ef_donnees.rigidite_matrice_partielle, projet->ef_donnees.c);
        BUGMSG(projet->ef_donnees.QR, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_genere_mat_rig");*/
        cholmod_l_free_triplet(&triplet_rigidite, projet->ef_donnees.c);
        
        return 0;
    }
    
    projet->ef_donnees.rigidite_matrice_partielle = cholmod_l_triplet_to_sparse(projet->ef_donnees.triplet_rigidite_partielle, 0, projet->ef_donnees.c);
    BUGMSG(projet->ef_donnees.rigidite_matrice_partielle, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_genere_mat_rig");
    projet->ef_donnees.rigidite_matrice_complete = cholmod_l_triplet_to_sparse(projet->ef_donnees.triplet_rigidite_complete, 0, projet->ef_donnees.c);
    BUGMSG(projet->ef_donnees.rigidite_matrice_complete, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_genere_mat_rig");
    /* On force les matrices à ne pas être symétriques.*/
    if (projet->ef_donnees.rigidite_matrice_partielle->stype != 0)
    {
        cholmod_sparse *A = cholmod_l_copy(projet->ef_donnees.rigidite_matrice_partielle, 0, 1, projet->ef_donnees.c);
        BUGMSG(A, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_genere_mat_rig");
        cholmod_l_free_sparse(&projet->ef_donnees.rigidite_matrice_partielle, projet->ef_donnees.c);
        projet->ef_donnees.rigidite_matrice_partielle = A;
    }
    if (projet->ef_donnees.rigidite_matrice_complete->stype != 0)
    {
        cholmod_sparse *A = cholmod_l_copy(projet->ef_donnees.rigidite_matrice_complete, 0, 1, projet->ef_donnees.c);
        BUGMSG(A, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_genere_mat_rig");
        cholmod_l_free_sparse(&projet->ef_donnees.rigidite_matrice_complete, projet->ef_donnees.c);
        projet->ef_donnees.rigidite_matrice_complete = A;
    }
    
/*  Pour utiliser cholmod dans les calculs de matrices.
     Et on factorise la matrice
    projet->ef_donnees.factor_rigidite_matrice_partielle = cholmod_l_analyze (projet->ef_donnees.rigidite_matrice_partielle, projet->ef_donnees.c) ;
    Normalement, c'est par cette méthode qu'on résoud une matrice non symétrique. Mais en pratique, ça ne marche pas. Pourquoi ?!?
    double beta[2] = {1.e-6, 0.};
    if (cholmod_l_factorize_p(projet->ef_donnees.rigidite_matrice_partielle, beta, NULL, 0, projet->ef_donnees.factor_rigidite_matrice_partielle, projet->ef_donnees.c) == TRUE)
    {
        if (projet->ef_donnees.c->status == CHOLMOD_NOT_POSDEF)
            BUGMSG(0, -1, "Matrice non définie positive.\n");
    }*/
    
    // Factorisation de la matrice de rigidité partielle.
    void *symbolic;
    projet->ef_donnees.ap = (long*)malloc(sizeof(long)*(projet->ef_donnees.triplet_rigidite_partielle->ncol+1));
    BUGMSG(projet->ef_donnees.ap, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_genere_mat_rig");
    projet->ef_donnees.ai = (long*)malloc(sizeof(long)*projet->ef_donnees.triplet_rigidite_partielle->nnz);
    BUGMSG(projet->ef_donnees.ai, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_genere_mat_rig");
    projet->ef_donnees.map= (long*)malloc(sizeof(long)*projet->ef_donnees.triplet_rigidite_partielle->nnz);
    BUGMSG(projet->ef_donnees.map, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_genere_mat_rig");
    projet->ef_donnees.ax= (double*)malloc(sizeof(double)*projet->ef_donnees.triplet_rigidite_partielle->nnz);
    BUGMSG(projet->ef_donnees.ax, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_genere_mat_rig");
    ai=projet->ef_donnees.triplet_rigidite_partielle->i;
    aj=projet->ef_donnees.triplet_rigidite_partielle->j;
    ax=projet->ef_donnees.triplet_rigidite_partielle->x;
    umfpack_dl_triplet_to_col(projet->ef_donnees.triplet_rigidite_partielle->nrow, projet->ef_donnees.triplet_rigidite_partielle->ncol, projet->ef_donnees.triplet_rigidite_partielle->nnz, ai, aj, ax, projet->ef_donnees.ap, projet->ef_donnees.ai, projet->ef_donnees.ax, projet->ef_donnees.map);
    umfpack_dl_symbolic(projet->ef_donnees.triplet_rigidite_partielle->nrow, projet->ef_donnees.triplet_rigidite_partielle->ncol, projet->ef_donnees.ap, projet->ef_donnees.ai, projet->ef_donnees.ax, &symbolic, NULL, NULL);
    BUGMSG(symbolic, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_genere_mat_rig");
    umfpack_dl_numeric(projet->ef_donnees.ap, projet->ef_donnees.ai, projet->ef_donnees.ax, symbolic, &projet->ef_donnees.numeric, NULL, NULL);
    BUGMSG(projet->ef_donnees.numeric, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_genere_mat_rig");
    free(symbolic);
    
    return 0;
}


int EF_calculs_moment_hyper_y(Barre_Info_EF *infos, double phia, double phib,
  double *ma, double *mb)
/* Description : Calcul le moment hyperstatique à partir de la rotation au point A et B autour
 *                 de l'axe Y.
 * Paramètres : Barre_Info_EF *infos : contient les paramètres ay, by, cy et kAy et kBy.
 *              double phia : rotation au noeud A.
 *              double phib : rotation au noeud B.
 *              double *ma : moment au noeud A. Peut être NULL.
 *              double *mb : moment au noeud B. Peut être NULL.
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (infos == NULL)
 */
{
    BUGMSG(infos, -1, "EF_calculs_moment_hyper_y\n");
    // Calcul des moments créés par les raideurs :\end{verbatim}\begin{align*}
    //               M_{Ay} & = -\frac{b_y \cdot \varphi_{By}+(c_y+k_{By}) \cdot \varphi_{Ay}}{(a_y+k_{Ay}) \cdot (c_y+k_{By})-b_y^2} \nonumber\\
    //               M_{By} & = -\frac{b_y \cdot \varphi_{Ay}+(a_y+k_{Ay}) \cdot \varphi_{By}}{(a_y+k_{Ay}) \cdot (c_y+k_{By})-b_y^2} \end{align*}\begin{verbatim}
    if ((ERREUR_RELATIVE_EGALE(infos->kAy, MAXDOUBLE)) && (ERREUR_RELATIVE_EGALE(infos->kBy, MAXDOUBLE)))
    {
        if (ma != NULL)
            *ma = 0.;
        if (mb != NULL)
            *mb = 0.;
    }
    else if (ERREUR_RELATIVE_EGALE(infos->kAy, MAXDOUBLE))
    {
        if (ma != NULL)
            *ma = 0.;
        if (mb != NULL)
            *mb = -phib/(infos->cy+infos->kBy);
    }
    else if (ERREUR_RELATIVE_EGALE(infos->kBy, MAXDOUBLE))
    {
        if (ma != NULL)
            *ma = -phia/(infos->ay+infos->kAy);
        if (mb != NULL)
        *mb = 0.;
    }
    else
    {
        if (ma != NULL)
            *ma = -(infos->by*phib+(infos->cy+infos->kBy)*phia)/((infos->ay+infos->kAy)*(infos->cy+infos->kBy)-infos->by*infos->by);
        if (mb != NULL)
            *mb = -(infos->by*phia+(infos->ay+infos->kAy)*phib)/((infos->ay+infos->kAy)*(infos->cy+infos->kBy)-infos->by*infos->by);
    }
    return 0;
}


int EF_calculs_moment_hyper_z(Barre_Info_EF *infos, double phia, double phib,
  double *ma, double *mb)
/* Description : Calcul le moment hyperstatique à partir de la rotation au point A et B autour
 *                 de l'axe Z.
 * Paramètres : Barre_Info_EF *infos : contient les paramètres az, bz, cz et kAz et kBz.
 *              double phia : rotation au noeud A.
 *              double phib : rotation au noeud B.
 *              double *ma : moment au noeud A. Peut être NULL.
 *              double *mb : moment au noeud B. Peut être NULL.
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (infos == NULL)
 */
{
    BUGMSG(infos, -1, "EF_calculs_moment_hyper_z\n");
    // Calcul des moments créés par les raideurs :\end{verbatim}\begin{align*}
    //               M_{Az} & = \frac{b_z \cdot \varphi_{Bz}+(c_z+k_{Bz}) \cdot \varphi_{Az}}{(a_z+k_{Az}) \cdot (c_z+k_{Bz})-b_z^2} \nonumber\\
    //               M_{Bz} & = \frac{b_z \cdot \varphi_{Az}+(a_z+k_{Az}) \cdot \varphi_{Bz}}{(a_z+k_{Az}) \cdot (c_z+k_{Bz})-b_z^2}\end{align*}\begin{verbatim}
    if ((ERREUR_RELATIVE_EGALE(infos->kAz, MAXDOUBLE)) && (ERREUR_RELATIVE_EGALE(infos->kBz, MAXDOUBLE)))
    {
        if (ma != NULL)
            *ma = 0.;
        if (mb != NULL)
            *mb = 0.;
    }
    else if (ERREUR_RELATIVE_EGALE(infos->kAz, MAXDOUBLE))
    {
        if (ma != NULL)
            *ma = 0.;
        if (mb != NULL)
            *mb = phib/(infos->cz+infos->kBz);
    }
    else if (ERREUR_RELATIVE_EGALE(infos->kBz, MAXDOUBLE))
    {
        if (ma != NULL)
            *ma = phia/(infos->az+infos->kAz);
        if (mb != NULL)
            *mb = 0.;
    }
    else
    {
        if (ma != NULL)
            *ma = (infos->bz*phib+(infos->cz+infos->kBz)*phia)/((infos->az+infos->kAz)*(infos->cz+infos->kBz)-infos->bz*infos->bz);
        if (mb != NULL)
            *mb = (infos->bz*phia+(infos->az+infos->kAz)*phib)/((infos->az+infos->kAz)*(infos->cz+infos->kBz)-infos->bz*infos->bz);
    }
    return 0;
}


int EF_calculs_resoud_charge(Projet *projet, int num_action)
/* Description : Détermine à partir de la matrice de rigidité partielle factorisée les
 *               déplacements et les efforts dans les noeuds pour l'action demandée
 *               ainsi que la courbe des sollicitations dans les barres.
 * Paramètres : Projet *projet : la variable projet
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet == NULL) ou
 *             (projet->actions == NULL) ou
 *             (list_size(projet->actions) == 0) ou
 *             (_1990_action_cherche_numero(projet, num_action) != 0) ou
 *             (projet->ef_donnees.numeric == NULL)
 *           -2 en cas d'erreur d'allocation mémoire
 *           -3 en cas d'erreur due à une fonction interne
 */
{
    Action          *action_en_cours;
    cholmod_triplet *triplet_deplacements_totaux, *triplet_deplacements_partiels;
    cholmod_triplet *triplet_force_partielle, *triplet_force_complete;
    cholmod_dense   *dense_force;
    cholmod_triplet *triplet_efforts_locaux_finaux, *triplet_efforts_globaux_initiaux, *triplet_efforts_locaux_initiaux, *triplet_efforts_globaux_finaux;
    cholmod_sparse  *sparse_efforts_locaux_finaux,  *sparse_efforts_globaux_initiaux,  *sparse_efforts_locaux_initiaux,  *sparse_efforts_globaux_finaux;
    long            *ai, *aj;
    double          *ax;
    long            *ai2, *aj2;
    double          *ax2;
    long            *ai3, *aj3;
    double          *ax3;
    unsigned int    i, j, k;
    cholmod_dense   *X, *Y;
    cholmod_sparse  *sparse_tmp;
    
    BUGMSG(projet, -1, "EF_calculs_resoud_charge\n");
    BUGMSG(projet->actions, -1, "EF_calculs_resoud_charge\n");
    BUGMSG(list_size(projet->actions), -1, "EF_calculs_resoud_charge\n");
    BUGMSG(_1990_action_cherche_numero(projet, num_action) == 0, -1, "EF_calculs_resoud_charge : num_action %d\n", num_action);
    BUGMSG(projet->ef_donnees.numeric, -1, "EF_calculs_resoud_charge\n");
    
    // Création du triplet sparse partiel et complet contenant les forces extérieures
    //   sur les noeuds et initialisation des valeurs à 0. Le vecteur partiel sera 
    //   utilisé dans l'équation finale :
    //   {F} = [K]{D}
    action_en_cours = list_curr(projet->actions);
    BUG(common_fonction_init(projet, action_en_cours) == 0, -3);
    triplet_force_partielle = cholmod_l_allocate_triplet(projet->ef_donnees.rigidite_matrice_partielle->nrow, 1, projet->ef_donnees.rigidite_matrice_partielle->nrow, 0, CHOLMOD_REAL, projet->ef_donnees.c);
    BUGMSG(triplet_force_partielle, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_resoud_charge");
    ai = triplet_force_partielle->i;
    aj = triplet_force_partielle->j;
    ax = triplet_force_partielle->x;
    triplet_force_partielle->nnz = projet->ef_donnees.rigidite_matrice_partielle->nrow;
    for (i=0;i<triplet_force_partielle->nnz;i++)
    {
        ai[i] = i;
        aj[i] = 0;
        ax[i] = 0.;
    }
    triplet_force_complete = cholmod_l_allocate_triplet(projet->ef_donnees.rigidite_matrice_complete->nrow, 1, projet->ef_donnees.rigidite_matrice_complete->nrow, 0, CHOLMOD_REAL, projet->ef_donnees.c);
    BUGMSG(triplet_force_complete, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_resoud_charge");
    ai3 = triplet_force_complete->i;
    aj3 = triplet_force_complete->j;
    ax3 = triplet_force_complete->x;
    triplet_force_complete->nnz = projet->ef_donnees.rigidite_matrice_complete->nrow;
    for (i=0;i<triplet_force_complete->nnz;i++)
    {
        ai3[i] = i;
        aj3[i] = 0;
        ax3[i] = 0.;
    }
    
    // Détermination des charges aux noeuds (triplet_force_partielle).
    // Pour chaque charge dans l'action
    if (list_size(action_en_cours->charges) != 0)
    {
        list_mvfront(action_en_cours->charges);
        do
        {
            Charge_Barre_Ponctuelle *charge_barre = list_curr(action_en_cours->charges);
            if (charge_barre->type == CHARGE_PONCTUELLE_NOEUD)
            {
    //     Si la charge en cours est une charge ponctuelle au noeud Alors
    //         On ajoute au vecteur des efforts partiels et complets les efforts aux noeuds
    //           directement saisis par l'utilisateur dans le repère global.
                Charge_Noeud *charge_noeud = list_curr(action_en_cours->charges);
                if (projet->ef_donnees.noeuds_pos_partielle[charge_noeud->noeud->numero][0] != -1)
                    ax[projet->ef_donnees.noeuds_pos_partielle[charge_noeud->noeud->numero][0]] += charge_noeud->x;
                if (projet->ef_donnees.noeuds_pos_partielle[charge_noeud->noeud->numero][1] != -1)
                    ax[projet->ef_donnees.noeuds_pos_partielle[charge_noeud->noeud->numero][1]] += charge_noeud->y;
                if (projet->ef_donnees.noeuds_pos_partielle[charge_noeud->noeud->numero][2] != -1)
                    ax[projet->ef_donnees.noeuds_pos_partielle[charge_noeud->noeud->numero][2]] += charge_noeud->z;
                if (projet->ef_donnees.noeuds_pos_partielle[charge_noeud->noeud->numero][3] != -1)
                    ax[projet->ef_donnees.noeuds_pos_partielle[charge_noeud->noeud->numero][3]] += charge_noeud->mx;
                if (projet->ef_donnees.noeuds_pos_partielle[charge_noeud->noeud->numero][4] != -1)
                    ax[projet->ef_donnees.noeuds_pos_partielle[charge_noeud->noeud->numero][4]] += charge_noeud->my;
                if (projet->ef_donnees.noeuds_pos_partielle[charge_noeud->noeud->numero][5] != -1)
                    ax[projet->ef_donnees.noeuds_pos_partielle[charge_noeud->noeud->numero][5]] += charge_noeud->mz;
                ax3[projet->ef_donnees.noeuds_pos_complete[charge_noeud->noeud->numero][0]] += charge_noeud->x;
                ax3[projet->ef_donnees.noeuds_pos_complete[charge_noeud->noeud->numero][1]] += charge_noeud->y;
                ax3[projet->ef_donnees.noeuds_pos_complete[charge_noeud->noeud->numero][2]] += charge_noeud->z;
                ax3[projet->ef_donnees.noeuds_pos_complete[charge_noeud->noeud->numero][3]] += charge_noeud->mx;
                ax3[projet->ef_donnees.noeuds_pos_complete[charge_noeud->noeud->numero][4]] += charge_noeud->my;
                ax3[projet->ef_donnees.noeuds_pos_complete[charge_noeud->noeud->numero][5]] += charge_noeud->mz;
            }
    //     Sinon Les efforts ne sont pas aux noeuds mais dans la barre, il faut donc calculer
    //       les réactions d'appuis pour chaque charge en prenant en compte les conditions aux
    //       appuis (relachements)
            else
            {
                double       E, G;                      /* Caractéristiques du matériau de la barre */
                double       xx, yy, zz, l;
                double       a, b;                         /* Position de la charge par rapport au début de l'élément discrétisé */
                double       debut_barre, fin_barre;       /* Début et fin de la barre discrétisée par rapport à la barre complète */
                double       phiAy, phiBy, phiAz, phiBz;   /* Rotation sur appui lorsque la barre est isostatique */
                double       MAx, MBx, MAy, MBy, MAz, MBz; /* Moments créés par la raideur */
                double       FAx, FBx, FAy, FBy, FAz, FBz; /* Réactions d'appui*/
                EF_Noeud    *noeud_debut, *noeud_fin;
                Beton_Barre *element_en_beton = charge_barre->barre;
                unsigned int num = element_en_beton->numero;
                unsigned int pos;                         /* numéro de l'élément dans la discrétisation */
                
    //         Récupération des caractéristiques mécaniques de l'élément.
                switch (element_en_beton->type)
                {
                    case BETON_ELEMENT_POTEAU :
                    case BETON_ELEMENT_POUTRE :
                    {
                        E = element_en_beton->materiau->ecm;
                        G = element_en_beton->materiau->gnu_0_2;
                        break;
                    }
                    /* Type d'élément inconnu*/
                    default :
                    {
                        BUG(0, -1);
                    }
                }
                
    //         Si la charge est une charge ponctuelle Alors
                if (charge_barre->type == CHARGE_PONCTUELLE_BARRE)
                {
    //             Convertion des efforts globaux en efforts locaux si nécessaire :\end{verbatim}\begin{center}
    //               $\{ F \}_{local} = [K]^T \cdot \{ F \}_{global}$\end{center}\begin{verbatim}
                    if (charge_barre->repere_local == FALSE)
                    {
                        triplet_efforts_globaux_initiaux = cholmod_l_allocate_triplet(12, 1, 12, 0, CHOLMOD_REAL, projet->ef_donnees.c);
                        BUGMSG(triplet_efforts_globaux_initiaux, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_resoud_charge");
                        ai2 = triplet_efforts_globaux_initiaux->i;
                        aj2 = triplet_efforts_globaux_initiaux->j;
                        ax2 = triplet_efforts_globaux_initiaux->x;
                        triplet_efforts_globaux_initiaux->nnz = 12;
                    }
                    else
                    {
                        triplet_efforts_locaux_initiaux = cholmod_l_allocate_triplet(12, 1, 12, 0, CHOLMOD_REAL, projet->ef_donnees.c);
                        BUGMSG(triplet_efforts_locaux_initiaux, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_resoud_charge");
                        ai2 = triplet_efforts_locaux_initiaux->i;
                        aj2 = triplet_efforts_locaux_initiaux->j;
                        ax2 = triplet_efforts_locaux_initiaux->x;
                        triplet_efforts_locaux_initiaux->nnz = 12;
                    }
                    ai2[0] = 0;     aj2[0] = 0;     ax2[0] = charge_barre->x;
                    ai2[1] = 1;     aj2[1] = 0;     ax2[1] = charge_barre->y;
                    ai2[2] = 2;     aj2[2] = 0;     ax2[2] = charge_barre->z;
                    ai2[3] = 3;     aj2[3] = 0;     ax2[3] = charge_barre->mx;
                    ai2[4] = 4;     aj2[4] = 0;     ax2[4] = charge_barre->my;
                    ai2[5] = 5;     aj2[5] = 0;     ax2[5] = charge_barre->mz;
                    ai2[6] = 6;     aj2[6] = 0;     ax2[6] = 0.;
                    ai2[7] = 7;     aj2[7] = 0;     ax2[7] = 0.;
                    ai2[8] = 8;     aj2[8] = 0;     ax2[8] = 0.;
                    ai2[9] = 9;     aj2[9] = 0;     ax2[9] = 0.;
                    ai2[10] = 10;   aj2[10] = 0;    ax2[10] = 0.;
                    ai2[11] = 11;   aj2[11] = 0;    ax2[11] = 0.;
                    if (charge_barre->repere_local == FALSE)
                    {
                        sparse_efforts_globaux_initiaux = cholmod_l_triplet_to_sparse(triplet_efforts_globaux_initiaux, 0, projet->ef_donnees.c);
                        BUGMSG(sparse_efforts_globaux_initiaux, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_resoud_charge");
                        cholmod_l_free_triplet(&triplet_efforts_globaux_initiaux, projet->ef_donnees.c);
                        sparse_efforts_locaux_initiaux = cholmod_l_ssmult(element_en_beton->matrice_rotation_transpose, sparse_efforts_globaux_initiaux, 0, 1, 0, projet->ef_donnees.c);
                        BUGMSG(sparse_efforts_locaux_initiaux, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_resoud_charge");
                        cholmod_l_free_sparse(&(sparse_efforts_globaux_initiaux), projet->ef_donnees.c);
                        triplet_efforts_locaux_initiaux = cholmod_l_sparse_to_triplet(sparse_efforts_locaux_initiaux, projet->ef_donnees.c);
                        BUGMSG(triplet_efforts_locaux_initiaux, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_resoud_charge");
                        ai2 = triplet_efforts_locaux_initiaux->i;
                        aj2 = triplet_efforts_locaux_initiaux->j;
                        ax2 = triplet_efforts_locaux_initiaux->x;
                        cholmod_l_free_sparse(&(sparse_efforts_locaux_initiaux), projet->ef_donnees.c);
                    }
                    /* A ce stade ax2 pointent vers les charges dans le repère local*/
                    
    //             Détermination des deux noeuds se situant directement avant et après la
    //               charge ponctuelle (est différent des deux noeuds définissant la barre
    //               si la discrétisation est utilisée).
                    if (element_en_beton->discretisation_element == 0)
                    /* Pas de discrétisation */
                    {
                        pos = 0;
                        noeud_debut = element_en_beton->noeud_debut;
                        noeud_fin = element_en_beton->noeud_fin;
                    }
                    else
                    /* On cherche le noeud de départ et le noeud de fin */
                    {
                        pos = 0;
                        l = -1.;
                        /* On regarde pour chaque noeud intermédiaire si la position de la charge devient inférieur à la distance entre le noeud de départ et le noeud intermédiaire */
                        while ((pos<=element_en_beton->discretisation_element) && (l < charge_barre->position))
                        {
                            if (pos==element_en_beton->discretisation_element)
                            {
                                xx = element_en_beton->noeud_fin->position.x - element_en_beton->noeud_debut->position.x;
                                yy = element_en_beton->noeud_fin->position.y - element_en_beton->noeud_debut->position.y;
                                zz = element_en_beton->noeud_fin->position.z - element_en_beton->noeud_debut->position.z;
                            }
                            else
                            {
                                xx = element_en_beton->noeuds_intermediaires[pos]->position.x - element_en_beton->noeud_debut->position.x;
                                yy = element_en_beton->noeuds_intermediaires[pos]->position.y - element_en_beton->noeud_debut->position.y;
                                zz = element_en_beton->noeuds_intermediaires[pos]->position.z - element_en_beton->noeud_debut->position.z;
                            }
                            l = sqrt(xx*xx+yy*yy+zz*zz);
                            pos++;
                        }
                        pos--;
                        /* Alors la position de la charge est compris entre le début du noeud et le premier noeud intermédiaire */
                        if (pos==0)
                        {
                            noeud_debut = element_en_beton->noeud_debut;
                            noeud_fin = element_en_beton->noeuds_intermediaires[0];
                        }
                        /* Alors la position de la charge est compris entre le dernier noeud intermédiaire et le noeud de fin de la barre */
                        else if (pos == element_en_beton->discretisation_element)
                        {
                            noeud_debut = element_en_beton->noeuds_intermediaires[pos-1];
                            noeud_fin = element_en_beton->noeud_fin;
                        }
                        else
                        {
                            noeud_debut = element_en_beton->noeuds_intermediaires[pos-1];
                            noeud_fin = element_en_beton->noeuds_intermediaires[pos];
                        }
                    }
                    xx = noeud_debut->position.x - element_en_beton->noeud_debut->position.x;
                    yy = noeud_debut->position.y - element_en_beton->noeud_debut->position.y;
                    zz = noeud_debut->position.z - element_en_beton->noeud_debut->position.z;
                    debut_barre = sqrt(xx*xx+yy*yy+zz*zz);
                    a = charge_barre->position-debut_barre;
                    xx = noeud_fin->position.x - element_en_beton->noeud_debut->position.x;
                    yy = noeud_fin->position.y - element_en_beton->noeud_debut->position.y;
                    zz = noeud_fin->position.z - element_en_beton->noeud_debut->position.z;
                    fin_barre = sqrt(xx*xx+yy*yy+zz*zz);
                    l = ABS(fin_barre-debut_barre);
                    b = l-a;
                    
    //             Détermination des moments mx de rotation (EF_charge_barre_ponctuelle_mx) :
                    BUG(EF_charge_barre_ponctuelle_mx(element_en_beton, pos, a, &(element_en_beton->info_EF[pos]), ax2[3], &MAx, &MBx) == 0, -3);
                    
    //             Détermination de la rotation y et z aux noeuds de l'élément discrétisé en le
    //               supposant isostatique (EF_charge_barre_ponctuelle_def_ang_iso_y et z):
                    BUG(EF_charge_barre_ponctuelle_def_ang_iso_y(element_en_beton, pos, a, ax2[2], ax2[4], &phiAy, &phiBy) == 0, -3);
                    BUG(EF_charge_barre_ponctuelle_def_ang_iso_z(element_en_beton, pos, a, ax2[1], ax2[5], &phiAz, &phiBz) == 0, -3);
                    
    //             Calcul des moments créés par les raideurs (EF_calculs_moment_hyper_y et z) :
                    BUG(EF_calculs_moment_hyper_y(&(element_en_beton->info_EF[pos]), phiAy, phiBy, &MAy, &MBy) == 0, -3);
                    BUG(EF_calculs_moment_hyper_z(&(element_en_beton->info_EF[pos]), phiAz, phiBz, &MAz, &MBz) == 0, -3);
                    
    //             Réaction d'appui sur les noeuds :\end{verbatim}\begin{align*}
          // F_{Ax} & = F_x \cdot \frac{\int_0^l \frac{1}{E \cdot S(x)}}{\int_a^l \frac{1}{E \cdot S(x)}}\nonumber\\
          // F_{Bx} & = F_x - F_{Ax}\nonumber\\
          // F_{Ay} & = \frac{F_y \cdot b}{l}-\frac{M_z}{l}+\frac{M_{Bz}+M_{Az}}{l}\nonumber\\
          // F_{By} & = \frac{F_y \cdot a}{l}+\frac{M_z}{l}-\frac{M_{Bz}+M_{Az}}{l}\nonumber\\
          // F_{Az} & = \frac{F_z \cdot b}{l}+\frac{M_y}{l}-\frac{M_{By}+M_{Ay}}{l}\nonumber\\
          // F_{Bz} & = \frac{F_z \cdot a}{l}-\frac{M_y}{l}+\frac{M_{By}+M_{Ay}}{l}\end{align*}\begin{verbatim}
                    FAx = ax2[0]*_1992_1_1_sections_es_l(element_en_beton, pos, 0, l)/_1992_1_1_sections_es_l(element_en_beton, pos, a, l);
                    FBx = ax2[0] - FAx;
                    FAy = ax2[1]*b/l-ax2[5]/l+(MBz+MAz)/l;
                    FBy = ax2[1]*a/l+ax2[5]/l-(MBz+MAz)/l;
                    FAz = ax2[2]*b/l+ax2[4]/l-(MBy+MAy)/l;
                    FBz = ax2[2]*a/l-ax2[4]/l+(MBy+MAy)/l;
                    
    //             Détermination des fonctions des efforts dus à la charge (x, a et l sont
    //               calculés par rapport à l'élément discrétisé et non pour toute la barre).
    //               Pour cela on calcule la sollicitation due au cas isostatique puis on ajoute
    //               la sollicitation due à l'éventuel encastrement (MAx, MBx, MAy, MAz, MBy, MBz) :\end{verbatim}\begin{align*}
                    // N(x) & = -F_{Ax} & &\textrm{ pour x variant de 0 à a}\nonumber\\
                    // N(x) & = F_{Bx} & &\textrm{ pour x variant de a à l}\nonumber\\
                    
                    BUG(common_fonction_ajout(action_en_cours->fonctions_efforts[0][num], 0., a, -FAx, 0., 0., 0., debut_barre) == 0, -3);
                    BUG(common_fonction_ajout(action_en_cours->fonctions_efforts[0][num], a,  l, FBx,  0., 0., 0., debut_barre) == 0, -3);
                    
                    // T_y(x) & = -\frac{F_y \cdot b}{l} + \frac{M_z}{l} - \frac{M_{Az}+M_{Bz}}{l} & &\textrm{ pour x variant de 0 à a}\nonumber\\
                    // T_y(x) & = \frac{F_y \cdot a}{l} + \frac{M_z}{l} - \frac{M_{Az}+M_{Bz}}{l} & &\textrm{ pour x variant de a à l}\nonumber\\
                    
                    BUG(common_fonction_ajout(action_en_cours->fonctions_efforts[1][num], 0., a, -ax2[1]*b/l+ax2[5]/l, 0., 0., 0., debut_barre) == 0, -3);
                    BUG(common_fonction_ajout(action_en_cours->fonctions_efforts[1][num], a,  l, ax2[1]*a/l+ax2[5]/l,  0., 0., 0., debut_barre) == 0, -3);
                    BUG(common_fonction_ajout(action_en_cours->fonctions_efforts[1][num], 0., l, -(MAz+MBz)/l,         0., 0., 0., debut_barre) == 0, -3);
                    
                    // T_z(x) & = -\frac{F_z \cdot b}{l}-\frac{M_y}{l} + \frac{M_{Ay}+M_{By}}{l} & &\textrm{ pour x variant de 0 à a}\nonumber\\
                    // T_z(x) & = \frac{F_z \cdot a}{l}-\frac{M_y}{l} + \frac{M_{Ay}+M_{By}}{l} & &\textrm{ pour x variant de a à l}\nonumber\\
                    
                    BUG(common_fonction_ajout(action_en_cours->fonctions_efforts[2][num], 0., a, -ax2[2]*b/l-ax2[4]/l, 0., 0., 0., debut_barre) == 0, -3);
                    BUG(common_fonction_ajout(action_en_cours->fonctions_efforts[2][num], a,  l, ax2[2]*a/l-ax2[4]/l,  0., 0., 0., debut_barre) == 0, -3);
                    BUG(common_fonction_ajout(action_en_cours->fonctions_efforts[2][num], 0., l, (MAy+MBy)/l,          0., 0., 0., debut_barre) == 0, -3);
                    
                    // M_x(x) & = -M_{Ax} & &\textrm{ pour x variant de 0 à a}\nonumber\\
                    // M_x(x) & = M_{Bx} & &\textrm{ pour x variant de a à l}\nonumber\\
                    
                    BUG(common_fonction_ajout(action_en_cours->fonctions_efforts[3][num], 0., a, -MAx, 0., 0., 0., debut_barre) == 0, -3);
                    BUG(common_fonction_ajout(action_en_cours->fonctions_efforts[3][num], a,  l, MBx,  0., 0., 0., debut_barre) == 0, -3);
                    
                    // M_y(x) & = -\frac{F_z \cdot b \cdot x}{l}-\frac{M_y \cdot x}{l} - M_{Ay} + \frac{M_{Ay}+M_{By}}{l} \cdot x & &\textrm{ pour x variant de 0 à a}\nonumber\\
                    // M_y(x) & = -F_z \cdot a+M_y+\frac{F_z \cdot a \cdot x}{l}-\frac{M_y}{l} \cdot x - M_{Ay} + \frac{M_{Ay}+M_{By}}{l} \cdot x & &\textrm{ pour x variant de a à l}\nonumber\\
                    
                    BUG(common_fonction_ajout(action_en_cours->fonctions_efforts[4][num], 0., a,  0,               -ax2[2]*b/l-ax2[4]/l, 0., 0., debut_barre) == 0, -3);
                    BUG(common_fonction_ajout(action_en_cours->fonctions_efforts[4][num], a,  l, -ax2[2]*a+ax2[4], ax2[2]*a/l-ax2[4]/l,  0., 0., debut_barre) == 0, -3);
                    BUG(common_fonction_ajout(action_en_cours->fonctions_efforts[4][num], 0., l,  -MAy,            (MAy+MBy)/l,          0., 0., debut_barre) == 0, -3);
                    
                    // M_z(x) & = \frac{F_y \cdot b \cdot x}{l}-\frac{M_z \cdot x}{l} -M_{Az} + \frac{M_{Az}+M_{Bz}}{l} \cdot x & &\textrm{ pour x variant de 0 à a}\nonumber\\
                    // M_z(x) & = F_y \cdot a+M_z -\frac{F_y \cdot a \cdot x}{l}-\frac{M_z}{l} \cdot x -M_{Az} + \frac{M_{Az}+M_{Bz}}{l} \cdot x & &\textrm{ pour x variant de a à l}
                    
                    BUG(common_fonction_ajout(action_en_cours->fonctions_efforts[5][num], 0., a, 0.,              ax2[1]*b/l-ax2[5]/l,  0., 0., debut_barre) == 0, -3);
                    BUG(common_fonction_ajout(action_en_cours->fonctions_efforts[5][num], a,  l, ax2[1]*a+ax2[5], -ax2[1]*a/l-ax2[5]/l, 0., 0., debut_barre) == 0, -3);
                    BUG(common_fonction_ajout(action_en_cours->fonctions_efforts[5][num], 0., l, -MAz,            (MAz+MBz)/l,          0., 0., debut_barre) == 0, -3);
                    // \end{align*}\begin{verbatim}
                    
                    
    //             Détermination des fonctions de déformation et rotation de la même façon que
    //               pour les sollicitations (cas isostatique + encastrement) :
    //                 EF_charge_barre_ponctuelle_ajout_fonc_rx,
    //                 EF_charge_barre_ponctuelle_ajout_fonc_ry,
    //                 EF_charge_barre_ponctuelle_ajout_fonc_rz,
    //                 EF_charge_barre_ponctuelle_n)\begin{align*}
                    BUG(EF_charge_barre_ponctuelle_ajout_fonc_rx(action_en_cours->fonctions_rotation[0][num], element_en_beton, pos, a, MAx, MBx) == 0, -3);
                    BUG(EF_charge_barre_ponctuelle_ajout_fonc_ry(action_en_cours->fonctions_rotation[1][num], action_en_cours->fonctions_deformation[2][num], element_en_beton, pos, a, ax2[2], ax2[4], MAy, MBy) == 0, -3);
                    BUG(EF_charge_barre_ponctuelle_ajout_fonc_rz(action_en_cours->fonctions_rotation[2][num], action_en_cours->fonctions_deformation[1][num], element_en_beton, pos, a, ax2[1], ax2[5], MAz, MBz) == 0, -3);
                    
                    
                    EF_charge_barre_ponctuelle_n(action_en_cours->fonctions_deformation[0][num], element_en_beton, pos, a, FAx, FBx);
                    
                    cholmod_l_free_triplet(&triplet_efforts_locaux_initiaux, projet->ef_donnees.c);
                    
    //             Convertion des réactions d'appuis locales dans le repère global :\end{verbatim}\begin{center}
    //               $\{ R \}_{global} = [K] \cdot \{ F \}_{local}$\end{center}\begin{verbatim}
                    triplet_efforts_locaux_finaux = cholmod_l_allocate_triplet(12, 1, 12, 0, CHOLMOD_REAL, projet->ef_donnees.c);
                    BUGMSG(triplet_efforts_locaux_finaux, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_resoud_charge");
                    ai2 = triplet_efforts_locaux_finaux->i;
                    aj2 = triplet_efforts_locaux_finaux->j;
                    ax2 = triplet_efforts_locaux_finaux->x;
                    triplet_efforts_locaux_finaux->nnz = 12;
                    ai2[0] = 0;   aj2[0] = 0;  ax2[0] = FAx;
                    ai2[1] = 1;   aj2[1] = 0;  ax2[1] = FAy;
                    ai2[2] = 2;   aj2[2] = 0;  ax2[2] = FAz;
                    ai2[3] = 3;   aj2[3] = 0;  ax2[3] = MAx;
                    ai2[4] = 4;   aj2[4] = 0;  ax2[4] = MAy;
                    ai2[5] = 5;   aj2[5] = 0;  ax2[5] = MAz;
                    ai2[6] = 6;   aj2[6] = 0;  ax2[6] = FBx;
                    ai2[7] = 7;   aj2[7] = 0;  ax2[7] = FBy;
                    ai2[8] = 8;   aj2[8] = 0;  ax2[8] = FBz;
                    ai2[9] = 9;   aj2[9] = 0;  ax2[9] = MBx;
                    ai2[10] = 10; aj2[10] = 0; ax2[10] = MBy;
                    ai2[11] = 11; aj2[11] = 0; ax2[11] = MBz;
                    sparse_efforts_locaux_finaux = cholmod_l_triplet_to_sparse(triplet_efforts_locaux_finaux, 0, projet->ef_donnees.c);
                    BUGMSG(sparse_efforts_locaux_finaux, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_resoud_charge");
                    cholmod_l_free_triplet(&triplet_efforts_locaux_finaux, projet->ef_donnees.c);
                    sparse_efforts_globaux_finaux = cholmod_l_ssmult(element_en_beton->matrice_rotation, sparse_efforts_locaux_finaux, 0, 1, 0, projet->ef_donnees.c);
                    BUGMSG(sparse_efforts_globaux_finaux, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_resoud_charge");
                    cholmod_l_free_sparse(&(sparse_efforts_locaux_finaux), projet->ef_donnees.c);
                    triplet_efforts_globaux_finaux = cholmod_l_sparse_to_triplet(sparse_efforts_globaux_finaux, projet->ef_donnees.c);
                    BUGMSG(triplet_efforts_globaux_finaux, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_resoud_charge");
                    ai2 = triplet_efforts_globaux_finaux->i;
                    aj2 = triplet_efforts_globaux_finaux->j;
                    ax2 = triplet_efforts_globaux_finaux->x;
                    cholmod_l_free_sparse(&(sparse_efforts_globaux_finaux), projet->ef_donnees.c);
                    
    //             Ajout des moments et les efforts dans le vecteur des forces aux noeuds {F}
                    for (i=0;i<12;i++)
                    {
                        if (ai2[i] < 6)
                        {
                            if (projet->ef_donnees.noeuds_pos_partielle[noeud_debut->numero][ai2[i]] != -1)
                                ax[projet->ef_donnees.noeuds_pos_partielle[noeud_debut->numero][ai2[i]]] += ax2[i];
                            ax3[projet->ef_donnees.noeuds_pos_complete[noeud_debut->numero][ai2[i]]] += ax2[i];
                        }
                        else
                        {
                            if (projet->ef_donnees.noeuds_pos_partielle[noeud_fin->numero][ai2[i]-6] != -1)
                                ax[projet->ef_donnees.noeuds_pos_partielle[noeud_fin->numero][ai2[i]-6]] += ax2[i];
                            ax3[projet->ef_donnees.noeuds_pos_complete[noeud_fin->numero][ai2[i]-6]] += ax2[i];
                        }
                    }
                    cholmod_l_free_triplet(&triplet_efforts_globaux_finaux, projet->ef_donnees.c);
                }
    //         Fin Charge ponctuelle sur barre
                /* Charge inconnue */
                else
                    BUG(0, -1);
            }
    //     FinSi
        }
        while (list_mvnext(action_en_cours->charges) != NULL);
    }
    // FinPour
    
    /* On converti les données dans des structures permettant les calculs via les libraries */
    action_en_cours->forces_complet = cholmod_l_triplet_to_sparse(triplet_force_complete, 0, projet->ef_donnees.c);
    BUGMSG(action_en_cours->forces_complet, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_resoud_charge");
    dense_force = cholmod_l_sparse_to_dense(action_en_cours->forces_complet, projet->ef_donnees.c);
    BUGMSG(dense_force, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_resoud_charge");
    cholmod_l_free_triplet(&triplet_force_complete, projet->ef_donnees.c);
    
    // Calcul des déplacements des noeuds :\end{verbatim}\begin{align*}
    // \{ \Delta \}_{global} = [K]^{-1} \cdot \{ F \}_{global}\end{align*}\begin{verbatim}
    
    triplet_deplacements_partiels = cholmod_l_allocate_triplet(projet->ef_donnees.rigidite_matrice_partielle->nrow, 1, projet->ef_donnees.rigidite_matrice_partielle->nrow, 0, CHOLMOD_REAL, projet->ef_donnees.c);
    BUGMSG(triplet_deplacements_partiels, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_resoud_charge");
    triplet_deplacements_partiels->nnz = projet->ef_donnees.rigidite_matrice_partielle->nrow;
    ai = triplet_deplacements_partiels->i;
    aj = triplet_deplacements_partiels->j;
    ax = triplet_deplacements_partiels->x;
    ax2 = triplet_force_partielle->x;
    umfpack_dl_solve(UMFPACK_A, projet->ef_donnees.ap, projet->ef_donnees.ai, projet->ef_donnees.ax, ax, ax2, projet->ef_donnees.numeric, NULL, NULL) ;
    for (i=0;i<projet->ef_donnees.rigidite_matrice_partielle->nrow;i++)
    {
        ai[i] = i;
        aj[i] = 0;
    }
    
    /* Création du vecteur déplacement complet */
    triplet_deplacements_totaux = cholmod_l_allocate_triplet(action_en_cours->forces_complet->nrow, 1, action_en_cours->forces_complet->nrow, 0, CHOLMOD_REAL, projet->ef_donnees.c);
    BUGMSG(triplet_deplacements_totaux, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_resoud_charge");
    triplet_deplacements_totaux->nnz = action_en_cours->forces_complet->nrow;
    ai2 = triplet_deplacements_totaux->i;
    aj2 = triplet_deplacements_totaux->j;
    ax2 = triplet_deplacements_totaux->x;
    j = 0;
    for (i=0;i<list_size(projet->ef_donnees.noeuds);i++)
    {
        for (k=0;k<6;k++)
        {
            ai2[i*6+k] = i*6+k; aj2[i*6+k] = 0;
            if (projet->ef_donnees.noeuds_pos_partielle[i][k] == -1)
                ax2[i*6+k] = 0.;
            else
            {
                if (ai[j] == projet->ef_donnees.noeuds_pos_partielle[i][k])
                {
                    ax2[i*6+k] = ax[j];
                    j++;
                }
                else
                    ax2[i*6+k] = 0.;
            }
        }
    }
    cholmod_l_free_triplet(&triplet_deplacements_partiels, projet->ef_donnees.c);
    action_en_cours->deplacement_complet = cholmod_l_triplet_to_sparse(triplet_deplacements_totaux, 0, projet->ef_donnees.c);
    BUGMSG(action_en_cours->deplacement_complet, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_resoud_charge");
    X = cholmod_l_sparse_to_dense(action_en_cours->deplacement_complet, projet->ef_donnees.c);
    BUGMSG(X, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_resoud_charge");
    
    // Calcul du résidu :\end{verbatim}\begin{align*}
    // res = \frac{norme{\{[K]*\{\Delta\}+\{F\}\}}}{norme([K])*norme(\{\Delta\})+norme(\{F\})}\end{align*}\begin{verbatim}
    /* Méthode trouvée dans le fichier cholmod_demo.c de la source de la librairie cholmod. */
    cholmod_dense *r = cholmod_l_copy_dense(dense_force, projet->ef_donnees.c);
    BUGMSG(r, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_resoud_charge");
    double minusone[2] = {-1., 0.}, one[2] = {1., 0.};
    BUG(cholmod_l_sdmult(projet->ef_donnees.rigidite_matrice_complete, 0, minusone, one, X, r, projet->ef_donnees.c) == TRUE, -2);
    double bnorm = cholmod_l_norm_dense(dense_force, 0, projet->ef_donnees.c);
    double rnorm = cholmod_l_norm_dense(r, 0, projet->ef_donnees.c);
    double xnorm = cholmod_l_norm_dense(X, 0, projet->ef_donnees.c);
    double anorm = cholmod_l_norm_sparse(projet->ef_donnees.rigidite_matrice_complete, 0, projet->ef_donnees.c);
    double axbnorm = (anorm * xnorm + bnorm) ;
    action_en_cours->norm = rnorm / axbnorm ;
    printf("résidu : %e\n", action_en_cours->norm);
    
    // Calcule des réactions d'appuis :\end{verbatim}\begin{displaymath}
    // \{F\} = [K] \cdot \{\Delta\} - \{F_0\} \end{displaymath}\begin{verbatim}
    sparse_tmp = cholmod_l_ssmult(projet->ef_donnees.rigidite_matrice_complete, action_en_cours->deplacement_complet, 0, TRUE, TRUE, projet->ef_donnees.c);
    BUGMSG(sparse_tmp, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_resoud_charge");
    action_en_cours->efforts_noeuds = cholmod_l_add(sparse_tmp, action_en_cours->forces_complet, one, minusone, TRUE, TRUE, projet->ef_donnees.c);
    BUGMSG(action_en_cours->efforts_noeuds, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_resoud_charge");
    
    /* Libération de la mémoire */
    cholmod_l_free_dense(&Y, projet->ef_donnees.c);
    cholmod_l_free_dense(&X, projet->ef_donnees.c);
    cholmod_l_free_dense(&r, projet->ef_donnees.c);
    cholmod_l_free_sparse(&sparse_tmp, projet->ef_donnees.c);
    cholmod_l_free_dense(&dense_force, projet->ef_donnees.c);
    cholmod_l_free_triplet(&triplet_force_partielle, projet->ef_donnees.c);
    
    // Pour chaque barre, ajout des efforts dus aux déplacements
    list_mvfront(projet->beton.barres);
    do
    {
        Beton_Barre         *element_en_beton = list_curr(projet->beton.barres);
        Beton_Section_Rectangulaire *section_tmp = element_en_beton->section;
        double              S = _1992_1_1_sections_s(section_tmp);
        
    //     Pour chaque discrétisation de la barre
        for (j=0;j<=element_en_beton->discretisation_element;j++)
        {
            EF_Noeud            *noeud_debut, *noeud_fin;   /* Le noeud de départ et le noeud de fin, nécessaire en cas de discrétisation*/
            cholmod_sparse      *sparse_effort_locaux;
            double              l_debut, l_fin;
            double              E, G;
            cholmod_triplet     *triplet_deplacement_globaux;
            cholmod_sparse      *sparse_deplacement_globaux, *sparse_deplacement_locaux;
            double              xx, yy, zz, l;
            
            /* Récupération du noeud de départ et de fin de la partie discrétisée */
            if (j == 0)
                noeud_debut = element_en_beton->noeud_debut;
            else
                noeud_debut = element_en_beton->noeuds_intermediaires[j-1];
            if (j==element_en_beton->discretisation_element)
                noeud_fin = element_en_beton->noeud_fin;
            else
                noeud_fin = element_en_beton->noeuds_intermediaires[j];
            
            /* Récupération des caractéristiques de la barre en fonction du matériau */
            if ((element_en_beton->type == BETON_ELEMENT_POTEAU) || (element_en_beton->type == BETON_ELEMENT_POUTRE))
            {
                E = element_en_beton->materiau->ecm;
                G = element_en_beton->materiau->gnu_0_2;
            }
            /* Type d'élément inconnu*/
            else
                BUG(0, -1);
            
    //     Récupération des déplacements du noeud de départ et du noeud final de l'élément
            triplet_deplacement_globaux = cholmod_l_allocate_triplet(12, 1, 12, 0, CHOLMOD_REAL, projet->ef_donnees.c);
            BUGMSG(triplet_deplacement_globaux, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_resoud_charge");
            ai = triplet_deplacement_globaux->i;
            aj = triplet_deplacement_globaux->j;
            ax = triplet_deplacement_globaux->x;
            triplet_deplacement_globaux->nnz = 12;
            ax2 = triplet_deplacements_totaux->x;
            for (i=0;i<6;i++)
            {
                ai[i] = i;
                aj[i] = 0;
                ax[i] = ax2[projet->ef_donnees.noeuds_pos_complete[noeud_debut->numero][i]];
            }
            for (i=0;i<6;i++)
            {
                ai[i+6] = i+6;
                aj[i+6] = 0;
                ax[i+6] = ax2[projet->ef_donnees.noeuds_pos_complete[noeud_fin->numero][i]];
            }
            
    //     Conversion des déplacements globaux en déplacement locaux (u_A, v_A, w_A, theta_{Ax},
    //       theta_{Ay}, theta_{Az}, u_B, v_B, w_B, theta_{Bx}, theta_{By} et theta_{Bz}) : \end{verbatim}\begin{align*}
            // \{ \Delta \}_{local} = [R]^T \cdot \{ \Delta \}_{global}\end{align*}\begin{verbatim}
            sparse_deplacement_globaux = cholmod_l_triplet_to_sparse(triplet_deplacement_globaux, 0, projet->ef_donnees.c);
            BUGMSG(sparse_deplacement_globaux, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_resoud_charge");
            cholmod_l_free_triplet(&triplet_deplacement_globaux, projet->ef_donnees.c);
            sparse_deplacement_locaux = cholmod_l_ssmult(element_en_beton->matrice_rotation_transpose, sparse_deplacement_globaux, 0, 1, TRUE, projet->ef_donnees.c);
            BUGMSG(sparse_deplacement_locaux, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_resoud_charge");
            cholmod_l_free_sparse(&sparse_deplacement_globaux, projet->ef_donnees.c);
    //     Détermination des efforts (F_{Ax}, F_{Bx}, F_{Ay}, F_{By}, F_{Az}, F_{Bz}, M_{Ax},
    //       M_{Bx}, M_{Ay}, M_{By}, M_{Az} et M_{Bz}) dans le repère local : \end{verbatim}\begin{align*}
            // \{ F \}_{local} = [K] \cdot \{ \Delta \}_{local}\end{align*}\begin{verbatim}
            sparse_effort_locaux = cholmod_l_ssmult(element_en_beton->info_EF[j].matrice_rigidite_locale, sparse_deplacement_locaux, 0, 1, TRUE, projet->ef_donnees.c);
            BUGMSG(sparse_effort_locaux, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_resoud_charge");
            
            ax = sparse_deplacement_locaux->x;
            ax2 = sparse_effort_locaux->x;
            xx = noeud_debut->position.x - element_en_beton->noeud_debut->position.x;
            yy = noeud_debut->position.y - element_en_beton->noeud_debut->position.y;
            zz = noeud_debut->position.z - element_en_beton->noeud_debut->position.z;
            l_debut = sqrt(xx*xx+yy*yy+zz*zz);
            xx = noeud_fin->position.x - element_en_beton->noeud_debut->position.x;
            yy = noeud_fin->position.y - element_en_beton->noeud_debut->position.y;
            zz = noeud_fin->position.z - element_en_beton->noeud_debut->position.z;
            l_fin = sqrt(xx*xx+yy*yy+zz*zz);
            l = ABS(l_fin-l_debut);
            
    //     Ajout des efforts entre deux noeuds dus à leur déplacement relatif, la courbe vient
    //       s'ajouter à la courbe (si existante) déja définie précédemment. L'indice A
    //       correspond au noeud de départ et l'indice B correspond au noeud final :\end{verbatim}\begin{align*}
            // N(x) & = N(x) + F_{Ax} - (F_{Ax}+F_{Bx})/l\nonumber\\
            // T_y(x) & = T_y(x) + F_{Ay} - (F_{Ay}+F_{By})/l\nonumber\\
            // T_z(x) & = T_z(x) + F_{Az} - (F_{Az}+F_{Bz})/l\nonumber\\
            // M_x(x) & = M_x(x) + M_{Ax} - (M_{Ax}+M_{Bx})/l\nonumber\\
            // M_y(x) & = M_y(x) + M_{Ay} - (M_{Ay}+M_{By})/l\nonumber\\
            // M_z(x) & = M_z(x) + M_{Az} - (M_{Az}+M_{Bz})/l
            // \end{align*}\begin{verbatim}
            BUG(common_fonction_ajout(action_en_cours->fonctions_efforts[0][element_en_beton->numero], 0., l, ax2[0], -(ax2[0]+ax2[6])/l,  0., 0., l_debut) == 0, -3);
            BUG(common_fonction_ajout(action_en_cours->fonctions_efforts[1][element_en_beton->numero], 0., l, ax2[1], -(ax2[1]+ax2[7])/l,  0., 0., l_debut) == 0, -3);
            BUG(common_fonction_ajout(action_en_cours->fonctions_efforts[2][element_en_beton->numero], 0., l, ax2[2], -(ax2[2]+ax2[8])/l,  0., 0., l_debut) == 0, -3);
            BUG(common_fonction_ajout(action_en_cours->fonctions_efforts[3][element_en_beton->numero], 0., l, ax2[3], -(ax2[3]+ax2[9])/l,  0., 0., l_debut) == 0, -3);
            BUG(common_fonction_ajout(action_en_cours->fonctions_efforts[4][element_en_beton->numero], 0., l, ax2[4], -(ax2[4]+ax2[10])/l, 0., 0., l_debut) == 0, -3);
            BUG(common_fonction_ajout(action_en_cours->fonctions_efforts[5][element_en_beton->numero], 0., l, ax2[5], -(ax2[5]+ax2[11])/l, 0., 0., l_debut) == 0, -3);
            
    //     Ajout des déplacements & rotations entre deux noeuds dus à leur déplacement relatif,
    //       la courbe vient s'ajouter à la courbe (si existante) déja définie précédemment.
    //       La déformée en x est obtenue comme une variation linéaire entre les deux noeuds.
    //       La déformée en y et z est obtenue par double intégration de la courbe des moments.
    //       Les deux constantes sont obtenues par la connaissance des déplacements en x=0 et
    //       x=l. La rotation (ry et rz) est obtenue par la dérivée de la flèche.
    //       La rotation rx est obtenue par intégration du moment en x. La constante est
    //       déterminée pour f(0) égal à la rotation au noeud à gauche (s'il n'y a pas de
    //       relachement sinon f(l) égal à la rotation à droite).
            switch(section_tmp->type)
            {
                case BETON_SECTION_RECTANGULAIRE :
                case BETON_SECTION_T :
                case BETON_SECTION_CARRE :
                case BETON_SECTION_CIRCULAIRE :
                {
                    double J = _1992_1_1_sections_j(section_tmp);
                    double Iy = _1992_1_1_sections_iy(section_tmp);
                    double Iz = _1992_1_1_sections_iz(section_tmp);
                    if ((j == 0) && (element_en_beton->relachement != NULL) && (element_en_beton->relachement->rx_debut != EF_RELACHEMENT_BLOQUE))
                        BUG(common_fonction_ajout(action_en_cours->fonctions_rotation[0][element_en_beton->numero], 0., l, ax2[3]/(G*J)*l-(ax2[3]+ax2[9])/(2*G*J*l)*l*l+ax[9], -ax2[3]/(G*J), (ax2[3]+ax2[9])/(2*G*J*l), 0., l_debut) == 0, -3);
                    else
                        BUG(common_fonction_ajout(action_en_cours->fonctions_rotation[0][element_en_beton->numero], 0., l, ax[3], -ax2[3]/(G*J), (ax2[3]+ax2[9])/(2*G*J*l), 0., l_debut) == 0, -3);
                    BUG(common_fonction_ajout(action_en_cours->fonctions_rotation[1][element_en_beton->numero], 0., l, -(-ax2[4]/(2*E*Iy)*l*l+(ax2[4]+ax2[10])/(6*E*Iy)*l*l-ax[2]+ax[8])/l, -ax2[4]/(E*Iy), (ax2[4]+ax2[10])/(2*l*E*Iy), 0., l_debut) == 0, -3);
                    BUG(common_fonction_ajout(action_en_cours->fonctions_rotation[2][element_en_beton->numero], 0., l,  (ax2[5]/(2*E*Iz)*l*l+(-ax2[5]-ax2[11])/(6*E*Iz)*l*l-ax[1]+ax[7])/l, -ax2[5]/(E*Iz), (ax2[5]+ax2[11])/(2*l*E*Iz), 0., l_debut) == 0, -3);
                    BUG(common_fonction_ajout(action_en_cours->fonctions_deformation[0][element_en_beton->numero], 0., l, ax[0], -ax2[0]/(E*S), (ax2[0]+ax2[6])/l/(2*E*S), 0., l_debut) == 0, -3);
                    BUG(common_fonction_ajout(action_en_cours->fonctions_deformation[1][element_en_beton->numero], 0., l, ax[1], (ax2[5]/(2*E*Iz)*l*l+(-ax2[5]-ax2[11])/(6*E*Iz)*l*l-ax[1]+ax[7])/l, -ax2[5]/(2*E*Iz),  (ax2[5]+ax2[11])/(6*l*E*Iz), l_debut) == 0, -3);
                    BUG(common_fonction_ajout(action_en_cours->fonctions_deformation[2][element_en_beton->numero], 0., l, ax[2], (-ax2[4]/(2*E*Iy)*l*l+(ax2[4]+ax2[10])/(6*E*Iy)*l*l-ax[2]+ax[8])/l,  ax2[4]/(2*E*Iy), -(ax2[4]+ax2[10])/(6*l*E*Iy), l_debut) == 0, -3);
                    break;
                }
                default :
                {
                    BUGMSG(0, 0., "EF_charge_barre_ponctuelle_n\n");
                    break;
                }
            }
            
            cholmod_l_free_sparse(&sparse_deplacement_locaux, projet->ef_donnees.c);
            cholmod_l_free_sparse(&sparse_effort_locaux, projet->ef_donnees.c);
        }
    //     FinPour
    }
    // FinPour
    while (list_mvnext(projet->beton.barres) != NULL);
    cholmod_l_free_triplet(&triplet_deplacements_totaux, projet->ef_donnees.c);
    
    return 0;
}


int EF_calculs_affiche_resultats(Projet *projet, int num_action)
/* Description : Affiche tous les résultats d'une actio
 * Paramètres : Projet *projet : la variable projet
 *            : int num_action : numéro de l'action
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet == NULL) ou
 *             (projet->actions == NULL) ou
 *             (list_size(projet->actions) == 0) ou
 *             (_1990_action_cherche_numero(projet, num_action) != 0) ou
 *             (projet->beton.barres == NULL)
 *           -3 en cas d'erreur due à une fonction interne
 */
{
    Action          *action_en_cours;
    unsigned int        i;
    
    BUGMSG(projet, -1, "EF_calculs_affiche_resultats\n");
    BUGMSG(projet->actions, -1, "EF_calculs_affiche_resultats\n");
    BUGMSG(list_size(projet->actions), -1, "EF_calculs_affiche_resultats\n");
    BUGMSG(_1990_action_cherche_numero(projet, num_action) == 0, -1, "EF_calculs_affiche_resultats : num_action %d\n", num_action);
    BUGMSG(projet->beton.barres,  -1, "EF_calculs_affiche_resultats\n");
    
    // Affichage des efforts aux noeuds et des réactions d'appuis
    action_en_cours = list_curr(projet->actions);
    printf("Effort aux noeuds & Réactions d'appuis\n");
    common_math_arrondi_sparse(action_en_cours->efforts_noeuds);
    cholmod_l_write_sparse(stdout, action_en_cours->efforts_noeuds, NULL, NULL, projet->ef_donnees.c);
    // Affichage des déplacements des noeuds
    printf("Déplacements\n");
    common_math_arrondi_sparse(action_en_cours->deplacement_complet);
    cholmod_l_write_sparse(stdout, action_en_cours->deplacement_complet, NULL, NULL, projet->ef_donnees.c);
    // Pour chaque barre
    for (i=0;i<list_size(projet->beton.barres);i++)
    {
    //     Affichage de la courbe des sollicitations vis-à-vis de l'effort normal
        printf("Barre n°%d, Effort normal\n", i);
        BUG(common_fonction_affiche(action_en_cours->fonctions_efforts[0][i]) == 0, -3);
    //     Affichage de la courbe des sollicitations vis-à-vis de l'effort tranchant selon Y
        printf("Barre n°%d, Effort Tranchant Y\n", i);
        BUG(common_fonction_affiche(action_en_cours->fonctions_efforts[1][i]) == 0, -3);
    //     Affichage de la courbe des sollicitations vis-à-vis de l'effort tranchant selon Z
        printf("Barre n°%d, Effort Tranchant Z\n", i);
        BUG(common_fonction_affiche(action_en_cours->fonctions_efforts[2][i]) == 0, -3);
    //     Affichage de la courbe des sollicitations vis-à-vis du moment de torsion
        printf("Barre n°%d, Moment de torsion\n", i);
        BUG(common_fonction_affiche(action_en_cours->fonctions_efforts[3][i]) == 0, -3);
    //     Affichage de la courbe des sollicitations vis-à-vis du moment fléchissant selon Y
        printf("Barre n°%d, Moment de flexion Y\n", i);
        BUG(common_fonction_affiche(action_en_cours->fonctions_efforts[4][i]) == 0, -3);
    //     Affichage de la courbe des sollicitations vis-à-vis du moment fléchissant selon Z
        printf("Barre n°%d, Moment de flexion Z\n", i);
        BUG(common_fonction_affiche(action_en_cours->fonctions_efforts[5][i]) == 0, -3);
    }
    for (i=0;i<list_size(projet->beton.barres);i++)
    {
    //     Affichage de la courbe de déformation selon l'axe X
        printf("Barre n°%d, Déformation en X\n", i);
        BUG(common_fonction_affiche(action_en_cours->fonctions_deformation[0][i]) == 0, -3);
    //     Affichage de la courbe de déformation selon l'axe Y
        printf("Barre n°%d, Déformation en Y\n", i);
        BUG(common_fonction_affiche(action_en_cours->fonctions_deformation[1][i]) == 0, -3);
    //     Affichage de la courbe de déformation selon l'axe Z
        printf("Barre n°%d, Déformation en Z\n", i);
        BUG(common_fonction_affiche(action_en_cours->fonctions_deformation[2][i]) == 0, -3);
    //     Affichage de la courbe de rotation selon l'axe X
        printf("Barre n°%d, Rotation en X\n", i);
        BUG(common_fonction_affiche(action_en_cours->fonctions_rotation[0][i]) == 0, -3);
    //     Affichage de la courbe de rotation selon l'axe Y
        printf("Barre n°%d, Rotation en Y\n", i);
        BUG(common_fonction_affiche(action_en_cours->fonctions_rotation[1][i]) == 0, -3);
    //     Affichage de la courbe de rotation selon l'axe Z
        printf("Barre n°%d, Rotation en Z\n", i);
        BUG(common_fonction_affiche(action_en_cours->fonctions_rotation[2][i]) == 0, -3);
    }
    // FinPour
    
    return 0;
}
