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
#include <math.h>

#include "common_projet.h"
#include "common_erreurs.h"
#include "common_maths.h"
#include "common_fonction.h"
#include "EF_noeud.h"
#include "EF_charge_noeud.h"
#include "EF_charge_barre_ponctuelle.h"
#include "EF_charge_barre_repartie_uniforme.h"
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
    // Pour chaque noeud, y compris ceux dus à la discrétisation
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
        EF_Noeud    *noeud = (EF_Noeud*)list_curr(projet->ef_donnees.noeuds);
        
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
        Beton_Barre   *element = (Beton_Barre*)list_curr(projet->beton.barres);
        
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
/* Description : Factorisation de la matrice de rigidité.
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
    void *symbolic;
    
    BUGMSG(projet, -1, "EF_calculs_genere_mat_rig\n");
    BUGMSG(projet->ef_donnees.triplet_rigidite_partielle, -1, "EF_calculs_genere_mat_rig\n");
    BUGMSG(projet->ef_donnees.triplet_rigidite_complete, -1, "EF_calculs_genere_mat_rig\n");
    
    ai = (long*)projet->ef_donnees.triplet_rigidite_partielle->i;
    aj = (long*)projet->ef_donnees.triplet_rigidite_partielle->j;
    ax = (double*)projet->ef_donnees.triplet_rigidite_partielle->x;
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
    projet->ef_donnees.triplet_rigidite_complete->nnz = projet->ef_donnees.triplet_rigidite_complete_en_cours;
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
        cholmod_l_free_triplet(&triplet_rigidite, projet->ef_donnees.c);
        umfpack_dl_symbolic(0., 0., NULL, NULL, NULL, &symbolic, NULL, NULL);
        umfpack_dl_numeric(NULL, NULL, NULL, symbolic, &projet->ef_donnees.numeric, NULL, NULL);
        
        return 0;
    }
    
    projet->ef_donnees.rigidite_matrice_partielle = cholmod_l_triplet_to_sparse(projet->ef_donnees.triplet_rigidite_partielle, 0, projet->ef_donnees.c);
    BUGMSG(projet->ef_donnees.rigidite_matrice_partielle, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_genere_mat_rig");
    projet->ef_donnees.rigidite_matrice_complete = cholmod_l_triplet_to_sparse(projet->ef_donnees.triplet_rigidite_complete, 0, projet->ef_donnees.c);
    BUGMSG(projet->ef_donnees.rigidite_matrice_complete, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_genere_mat_rig");
    
    // Factorisation de la matrice de rigidité partielle.
    projet->ef_donnees.ap = (long*)malloc(sizeof(long)*(projet->ef_donnees.triplet_rigidite_partielle->ncol+1));
    BUGMSG(projet->ef_donnees.ap, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_genere_mat_rig");
    projet->ef_donnees.ai = (long*)malloc(sizeof(long)*projet->ef_donnees.triplet_rigidite_partielle->nnz);
    BUGMSG(projet->ef_donnees.ai, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_genere_mat_rig");
    projet->ef_donnees.ax = (double*)malloc(sizeof(double)*projet->ef_donnees.triplet_rigidite_partielle->nnz);
    BUGMSG(projet->ef_donnees.ax, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_genere_mat_rig");
    ai = (long*)projet->ef_donnees.triplet_rigidite_partielle->i;
    aj = (long*)projet->ef_donnees.triplet_rigidite_partielle->j;
    ax = (double*)projet->ef_donnees.triplet_rigidite_partielle->x;
    umfpack_dl_triplet_to_col(projet->ef_donnees.triplet_rigidite_partielle->nrow, projet->ef_donnees.triplet_rigidite_partielle->ncol, projet->ef_donnees.triplet_rigidite_partielle->nnz, ai, aj, ax, projet->ef_donnees.ap, projet->ef_donnees.ai, projet->ef_donnees.ax, NULL);
    umfpack_dl_symbolic(projet->ef_donnees.triplet_rigidite_partielle->nrow, projet->ef_donnees.triplet_rigidite_partielle->ncol, projet->ef_donnees.ap, projet->ef_donnees.ai, projet->ef_donnees.ax, &symbolic, NULL, NULL);
    BUGMSG(symbolic, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_genere_mat_rig");
    umfpack_dl_numeric(projet->ef_donnees.ap, projet->ef_donnees.ai, projet->ef_donnees.ax, symbolic, &projet->ef_donnees.numeric, NULL, NULL);
    BUGMSG(projet->ef_donnees.numeric, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_genere_mat_rig");
    umfpack_dl_free_symbolic(&symbolic);
    
    return 0;
}


int EF_calculs_moment_hyper_y(Barre_Info_EF *infos, double phia, double phib,
  double *ma, double *mb)
/* Description : Calcul le moment hyperstatique correspondant à l'opposé du moment de la
 *               réaction à partir de la rotation au point A et B autour de l'axe Y.
 * Paramètres : Barre_Info_EF *infos : contient les paramètres ay, by, cy et kAy et kBy.
 *            : double phia : rotation au noeud A.
 *            : double phib : rotation au noeud B.
 *            : double *ma : moment au noeud A. Peut être NULL.
 *            : double *mb : moment au noeud B. Peut être NULL.
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (infos == NULL)
 */
{
    BUGMSG(infos, -1, "EF_calculs_moment_hyper_y\n");
    // Calcul des moments créés par les raideurs :\end{verbatim}\texttt{Référence : \cite{RDM_articulation}}\begin{align*}
    //               M_{Ay} & = \frac{b_y \cdot \varphi_{By}+(c_y+k_{By}) \cdot \varphi_{Ay}}{(a_y+k_{Ay}) \cdot (c_y+k_{By})-b_y^2} \nonumber\\
    //               M_{By} & = \frac{b_y \cdot \varphi_{Ay}+(a_y+k_{Ay}) \cdot \varphi_{By}}{(a_y+k_{Ay}) \cdot (c_y+k_{By})-b_y^2} \end{align*}\begin{verbatim}
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
            *mb = phib/(infos->cy+infos->kBy);
    }
    else if (ERREUR_RELATIVE_EGALE(infos->kBy, MAXDOUBLE))
    {
        if (ma != NULL)
            *ma = phia/(infos->ay+infos->kAy);
        if (mb != NULL)
            *mb = 0.;
    }
    else
    {
        if (ma != NULL)
            *ma = (infos->by*phib+(infos->cy+infos->kBy)*phia)/((infos->ay+infos->kAy)*(infos->cy+infos->kBy)-infos->by*infos->by);
        if (mb != NULL)
            *mb = (infos->by*phia+(infos->ay+infos->kAy)*phib)/((infos->ay+infos->kAy)*(infos->cy+infos->kBy)-infos->by*infos->by);
    }
    return 0;
}


int EF_calculs_moment_hyper_z(Barre_Info_EF *infos, double phia, double phib,
  double *ma, double *mb)
/* Description : Calcul le moment hyperstatique correspondant à l'opposé du moment de la
 *               réaction à partir de la rotation au point A et B autour de l'axe Z.
 * Paramètres : Barre_Info_EF *infos : contient les paramètres az, bz, cz et kAz et kBz.
 *            : double phia : rotation au noeud A.
 *            : double phib : rotation au noeud B.
 *            : double *ma : moment au noeud A. Peut être NULL.
 *            : double *mb : moment au noeud B. Peut être NULL.
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (infos == NULL)
 */
{
    BUGMSG(infos, -1, "EF_calculs_moment_hyper_z\n");
    // Calcul des moments créés par les raideurs :\end{verbatim}\texttt{Référence : \cite{RDM_articulation}}\begin{align*}
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


double EF_calculs_resid(long *Ap, long *Ai, double *Ax, double *b, int n, double *x)
/* Description : Détermine le résidu lors de la résolution du système matriciel pour obtenir
 *               les déplacements aux noeuds (A.x = b).
 * Paramètres : long *Ap   : paramètres représentant la matrice
 *            : long *Ai   : de rigidité sous une forme
 *            : double *Ax : compatible avec umfpack,
 *            : double *b : Résultat à obtenir,
 *            : int n : nombre de ligne dans le vecteur b,
 *            : double x : résultat obtenu.
 * Valeur renvoyée :
 *   Succès : valeur du résidu
 *   Échec : NAN en cas de paramètres invalides :
 *             (Ap == NULL) ou
 *             (Ai == NULL) ou
 *             (Ax == NULL) ou
 *             (b == NULL) ou
 *             (x == NULL)
 *           NAN en cas d'erreur d'allocation mémoire
 */
{
    UF_long i, j, p ;
    double norm ;
    double *r = (double*)malloc(sizeof(double)*n);
    
    // Fonction tirée de la librarie UMFPACK, du fichier umfpack_dl_demo.c
    BUGMSG(Ap, NAN, "EF_calculs_resid\n");
    BUGMSG(Ai, NAN, "EF_calculs_resid\n");
    BUGMSG(Ax, NAN, "EF_calculs_resid\n");
    BUGMSG(b, NAN, "EF_calculs_resid\n");
    BUGMSG(x, NAN, "EF_calculs_resid\n");
    BUGMSG(r, NAN, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_resid");
    for (i=0;i<n;i++)
    	r[i] = -b[i] ;
    for (j=0;j<n;j++)
    {
        for (p=Ap[j];p<Ap[j+1];p++)
        {
        	i = Ai[p];
    		r[i] += Ax[p]*x[j];
        }
    }
    norm = 0.;
    for (i=0;i<n;i++)
    	norm = MAX(ABS(r[i]), norm);
    free(r);
    return (norm);
}


int EF_calculs_resoud_charge(Projet *projet, int num_action)
/* Description : Détermine à partir de la matrice de rigidité partielle factorisée les
 *               déplacements et les efforts dans les noeuds pour l'action demandée ainsi
 *               que la courbe des sollicitations dans les barres.
 * Paramètres : Projet *projet : la variable projet,
 *            : int num_action : numéro de l'action à résourdre.
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet == NULL) ou
 *             (projet->actions == NULL) ou
 *             (list_size(projet->actions) == 0) ou
 *             (_1990_action_cherche_numero(projet, num_action) != 0) ou
 *             (projet->ef_donnees.numeric == NULL) && (matrice_partielle->nrow != 0)
 *           -2 en cas d'erreur d'allocation mémoire
 *           -3 en cas d'erreur due à une fonction interne
 */
{
    Action          *action_en_cours;
    cholmod_triplet *triplet_deplacements_totaux, *triplet_deplacements_partiels;
    cholmod_triplet *triplet_force_partielle, *triplet_force_complete;
    cholmod_triplet *triplet_efforts_locaux_finaux, *triplet_efforts_globaux_initiaux, *triplet_efforts_locaux_initiaux, *triplet_efforts_globaux_finaux;
    cholmod_sparse  *sparse_efforts_locaux_finaux,  *sparse_efforts_globaux_initiaux,  *sparse_efforts_locaux_initiaux,  *sparse_efforts_globaux_finaux;
    long            *ai, *aj;
    double          *ax;
    long            *ai2, *aj2;
    double          *ax2;
    long            *ai3, *aj3;
    double          *ax3;
    unsigned int    i, j, k;
    cholmod_sparse  *sparse_tmp;
    double minusone[2] = {-1., 0.}, one[2] = {1., 0.};
    
    BUGMSG(projet, -1, "EF_calculs_resoud_charge\n");
    BUGMSG(projet->actions, -1, "EF_calculs_resoud_charge\n");
    BUGMSG(list_size(projet->actions), -1, "EF_calculs_resoud_charge\n");
    BUGMSG(_1990_action_cherche_numero(projet, num_action) == 0, -1, "EF_calculs_resoud_charge : num_action %d\n", num_action);
    BUGMSG(!((projet->ef_donnees.numeric == NULL) && (projet->ef_donnees.rigidite_matrice_partielle->nrow != 0)), -1, "EF_calculs_resoud_charge\n");
    
    /* Création du triplet partiel et complet contenant les forces extérieures
     * sur les noeuds et initialisation des valeurs à 0. Le vecteur partiel sera 
     * utilisé dans l'équation finale : {F} = [K]{D}*/
    action_en_cours = (Action*)list_curr(projet->actions);
    BUG(common_fonction_init(projet, action_en_cours) == 0, -3);
    triplet_force_partielle = cholmod_l_allocate_triplet(projet->ef_donnees.rigidite_matrice_partielle->nrow, 1, projet->ef_donnees.rigidite_matrice_partielle->nrow, 0, CHOLMOD_REAL, projet->ef_donnees.c);
    BUGMSG(triplet_force_partielle, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_resoud_charge");
    ai = (long*)triplet_force_partielle->i;
    aj = (long*)triplet_force_partielle->j;
    ax = (double*)triplet_force_partielle->x;
    triplet_force_partielle->nnz = projet->ef_donnees.rigidite_matrice_partielle->nrow;
    for (i=0;i<triplet_force_partielle->nnz;i++)
    {
        ai[i] = i;
        aj[i] = 0;
        ax[i] = 0.;
    }
    triplet_force_complete = cholmod_l_allocate_triplet(projet->ef_donnees.rigidite_matrice_complete->nrow, 1, projet->ef_donnees.rigidite_matrice_complete->nrow, 0, CHOLMOD_REAL, projet->ef_donnees.c);
    BUGMSG(triplet_force_complete, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_resoud_charge");
    ai3 = (long*)triplet_force_complete->i;
    aj3 = (long*)triplet_force_complete->j;
    ax3 = (double*)triplet_force_complete->x;
    triplet_force_complete->nnz = projet->ef_donnees.rigidite_matrice_complete->nrow;
    for (i=0;i<triplet_force_complete->nnz;i++)
    {
        ai3[i] = i;
        aj3[i] = 0;
        ax3[i] = 0.;
    }
    
    // Détermination des charges aux noeuds :
    //   Pour chaque charge dans l'action
    if (list_size(action_en_cours->charges) != 0)
    {
        list_mvfront(action_en_cours->charges);
        do
        {
            switch (((Charge_Barre_Ponctuelle *)list_curr(action_en_cours->charges))->type)
            {
                case CHARGE_NOEUD :
                {
    //     Si la charge en cours est une charge au noeud Alors
    //         On ajoute au vecteur des efforts les efforts aux noeuds directement saisis par
    //           l'utilisateur dans le repère global.
                    Charge_Noeud *charge_noeud = (Charge_Noeud*)list_curr(action_en_cours->charges);
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
                    break;
                }
    //     Sinon Si la charge est une charge ponctuelle sur la barre Alors
                case CHARGE_BARRE_PONCTUELLE :
                {
                    Charge_Barre_Ponctuelle *charge_barre = (Charge_Barre_Ponctuelle*)list_curr(action_en_cours->charges);
                    double       xx, yy, zz, l;
                    double       a, b;                         /* Position de la charge par rapport au début et à la fin de l'élément discrétisé */
                    double       debut_barre, fin_barre;       /* Début et fin de la barre discrétisée par rapport à la barre complète */
                    double       phiAy, phiBy, phiAz, phiBz;   /* Rotation sur appui lorsque la barre est isostatique */
                    double       MAx, MBx, MAy, MBy, MAz, MBz; /* Moment opposé à la réaction d'appui */
                    double       FAx, FBx;
                    double       FAy_i, FAy_h, FBy_i, FBy_h;   /* Force opposé à la réaction d'appui */
                    double       FAz_i, FAz_h, FBz_i, FBz_h;
                    EF_Noeud    *noeud_debut, *noeud_fin;
                    Beton_Barre *element_en_beton = (Beton_Barre*)charge_barre->barre;
                    unsigned int num = element_en_beton->numero;
                    unsigned int pos;                          /* numéro de l'élément dans la discrétisation */
                    
    //         Convertion des efforts globaux en efforts locaux si nécessaire :\end{verbatim}\begin{center}
    //         $\{ F \}_{local} = [R]^T \cdot \{ F \}_{global}$\end{center}\begin{verbatim}
                    if (charge_barre->repere_local == FALSE)
                    {
                        triplet_efforts_globaux_initiaux = cholmod_l_allocate_triplet(12, 1, 12, 0, CHOLMOD_REAL, projet->ef_donnees.c);
                        BUGMSG(triplet_efforts_globaux_initiaux, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_resoud_charge");
                        ai2 = (long*)triplet_efforts_globaux_initiaux->i;
                        aj2 = (long*)triplet_efforts_globaux_initiaux->j;
                        ax2 = (double*)triplet_efforts_globaux_initiaux->x;
                        triplet_efforts_globaux_initiaux->nnz = 12;
                    }
                    else
                    {
                        triplet_efforts_locaux_initiaux = cholmod_l_allocate_triplet(12, 1, 12, 0, CHOLMOD_REAL, projet->ef_donnees.c);
                        BUGMSG(triplet_efforts_locaux_initiaux, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_resoud_charge");
                        ai2 = (long*)triplet_efforts_locaux_initiaux->i;
                        aj2 = (long*)triplet_efforts_locaux_initiaux->j;
                        ax2 = (double*)triplet_efforts_locaux_initiaux->x;
                        triplet_efforts_locaux_initiaux->nnz = 12;
                    }
                    ai2[0] = 0;     aj2[0] = 0;     ax2[0] = charge_barre->fx;
                    ai2[1] = 1;     aj2[1] = 0;     ax2[1] = charge_barre->fy;
                    ai2[2] = 2;     aj2[2] = 0;     ax2[2] = charge_barre->fz;
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
                        cholmod_l_free_sparse(&sparse_efforts_globaux_initiaux, projet->ef_donnees.c);
                        triplet_efforts_locaux_initiaux = cholmod_l_sparse_to_triplet(sparse_efforts_locaux_initiaux, projet->ef_donnees.c);
                        BUGMSG(triplet_efforts_locaux_initiaux, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_resoud_charge");
                        ai2 = (long*)triplet_efforts_locaux_initiaux->i;
                        aj2 = (long*)triplet_efforts_locaux_initiaux->j;
                        ax2 = (double*)triplet_efforts_locaux_initiaux->x;
                        cholmod_l_free_sparse(&(sparse_efforts_locaux_initiaux), projet->ef_donnees.c);
                    }
                    /* A ce stade ax2 pointent vers les charges dans le repère local*/
                    
    //         Détermination des deux noeuds se situant directement avant et après la
    //           charge ponctuelle (est différent des deux noeuds définissant la barre
    //           si elle est discrétisée).
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
                    
    //         Détermination des moments mx de rotation (EF_charge_barre_ponctuelle_mx) :
                    BUG(EF_charge_barre_ponctuelle_mx(element_en_beton, pos, a, &(element_en_beton->info_EF[pos]), ax2[3], &MAx, &MBx) == 0, -3);
                    
    //         Détermination de la rotation y et z aux noeuds de l'élément discrétisé en le
    //           supposant isostatique (EF_charge_barre_ponctuelle_def_ang_iso_y et z):
                    BUG(EF_charge_barre_ponctuelle_def_ang_iso_y(element_en_beton, pos, a, ax2[2], ax2[4], &phiAy, &phiBy) == 0, -3);
                    BUG(EF_charge_barre_ponctuelle_def_ang_iso_z(element_en_beton, pos, a, ax2[1], ax2[5], &phiAz, &phiBz) == 0, -3);
                    
    //         Calcul des moments créés par les raideurs (EF_calculs_moment_hyper_y et z) :
                    BUG(EF_calculs_moment_hyper_y(&(element_en_beton->info_EF[pos]), phiAy, phiBy, &MAy, &MBy) == 0, -3);
                    BUG(EF_calculs_moment_hyper_z(&(element_en_beton->info_EF[pos]), phiAz, phiBz, &MAz, &MBz) == 0, -3);
                    
    //         Réaction d'appui sur les noeuds :\end{verbatim}\begin{align*}
                    // F_{Ax} & = F_x \cdot \frac{\int_a^l \frac{1}{E \cdot S(x)}}{\int_0^l \frac{1}{E \cdot S(x)}}\nonumber\\
                    // F_{Bx} & = F_x - F_{Ax}\nonumber\\
                    // F_{Ay_i}   & = \frac{F_y \cdot b}{l}-\frac{M_z}{l}\nonumber\\
                    // F_{Ay_h} & = \frac{M_{Bz}+M_{Az}}{l}\nonumber\\
                    // F_{By_i}   & = \frac{F_y \cdot a}{l}+\frac{M_z}{l}\nonumber\\
                    // F_{By_h} & = -\frac{M_{Bz}+M_{Az}}{l}\nonumber\\
                    // F_{Az_i}   & = \frac{F_z \cdot b}{l}+\frac{M_y}{l}\nonumber\\
                    // F_{Az_h} & = -\frac{M_{By}+M_{Ay}}{l}\nonumber\\
                    // F_{Bz_i}   & = \frac{F_z \cdot a}{l}-\frac{M_y}{l}\nonumber\\
                    // F_{Bz_h} & = \frac{M_{By}+M_{Ay}}{l}\end{align*}\begin{verbatim}
                    FAx = ax2[0]*_1992_1_1_sections_es_l(element_en_beton, pos, 0, l)/_1992_1_1_sections_es_l(element_en_beton, pos, a, l);
                    FBx = ax2[0] - FAx;
                    FAy_i = ax2[1]*b/l-ax2[5]/l;
                    FAy_h = (MBz+MAz)/l;
                    FBy_i = ax2[1]*a/l+ax2[5]/l;
                    FBy_h = -(MBz+MAz)/l;
                    FAz_i = ax2[2]*b/l+ax2[4]/l;
                    FAz_h = -(MBy+MAy)/l;
                    FBz_i = ax2[2]*a/l-ax2[4]/l;
                    FBz_h = (MBy+MAy)/l;
                    
    //         Détermination des fonctions des efforts dus à la charge (x, a et l sont
    //           calculés par rapport à l'élément discrétisé et non pour toute la barre).
    //           Pour cela on calcule la sollicitation due au cas isostatique puis on ajoute
    //           la sollicitation due à l'éventuel encastrement (MAx, MBx, MAy, MAz, MBy, MBz) :\end{verbatim}\begin{align*}
                    // N(x) & = -F_{Ax} & &\textrm{ pour x de 0 à a}\nonumber\\
                    // N(x) & = F_{Bx} & &\textrm{ pour x de a à l}\nonumber\\
                    
                    BUG(common_fonction_ajout(action_en_cours->fonctions_efforts[0][num], 0., a, -FAx, 0., 0., 0., 0., 0., 0., debut_barre) == 0, -3);
                    BUG(common_fonction_ajout(action_en_cours->fonctions_efforts[0][num], a,  l,  FBx, 0., 0., 0., 0., 0., 0., debut_barre) == 0, -3);
                    
                    // T_y(x) & = -F_{Ay_i} - F_{Ay_h} & &\textrm{ pour x de 0 à a}\nonumber\\
                    // T_y(x) & =  F_{By_i} + F_{By_h} & &\textrm{ pour x de a à l}\nonumber\\
                    
                    BUG(common_fonction_ajout(action_en_cours->fonctions_efforts[1][num], 0., a, -FAy_i-FAy_h, 0., 0., 0., 0., 0., 0., debut_barre) == 0, -3);
                    BUG(common_fonction_ajout(action_en_cours->fonctions_efforts[1][num], a,  l,  FBy_i+FBy_h, 0., 0., 0., 0., 0., 0., debut_barre) == 0, -3);
                    
                    // T_z(x) & = -F_{Az_i} - F_{Az_h} & &\textrm{ pour x de 0 à a}\nonumber\\
                    // T_z(x) & =  F_{Bz_i} + F_{Bz_h} & &\textrm{ pour x de a à l}\nonumber\\
                    
                    BUG(common_fonction_ajout(action_en_cours->fonctions_efforts[2][num], 0., a, -FAz_i-FAz_h, 0., 0., 0., 0., 0., 0., debut_barre) == 0, -3);
                    BUG(common_fonction_ajout(action_en_cours->fonctions_efforts[2][num], a,  l,  FBz_i+FBz_h, 0., 0., 0., 0., 0., 0., debut_barre) == 0, -3);
                    
                    // M_x(x) & = -M_{Ax} & &\textrm{ pour x de 0 à a}\nonumber\\
                    // M_x(x) & = M_{Bx} & &\textrm{ pour x de a à l}\nonumber\\
                    
                    BUG(common_fonction_ajout(action_en_cours->fonctions_efforts[3][num], 0., a, -MAx, 0., 0., 0., 0., 0., 0., debut_barre) == 0, -3);
                    BUG(common_fonction_ajout(action_en_cours->fonctions_efforts[3][num], a,  l,  MBx, 0., 0., 0., 0., 0., 0., debut_barre) == 0, -3);
                    
                    // M_y(x) & = -M_{Ay} - (F_{Az_i}+F_{Az_h}) \cdot x & &\textrm{ pour x de 0 à a}\nonumber\\
                    // M_y(x) & = F_{Bz_i} \cdot L - M_{Ay} + (F_{Bz_i}+F_{Bz_h}) \cdot x & &\textrm{ pour x de a à l}\nonumber\\
                    
                    BUG(common_fonction_ajout(action_en_cours->fonctions_efforts[4][num], 0., a, -MAy,         -FAz_i-FAz_h, 0., 0., 0., 0., 0., debut_barre) == 0, -3);
                    BUG(common_fonction_ajout(action_en_cours->fonctions_efforts[4][num], a,  l, -FBz_i*l-MAy,  FBz_i+FBz_h, 0., 0., 0., 0., 0., debut_barre) == 0, -3);
                    
                    // M_z(x) & = -M_{Az} + (F_{Ay_i}+F_{Ay_h}) \cdot x & &\textrm{ pour x de 0 à a}\nonumber\\
                    // M_z(x) & = F_{By_i} \cdot L - M_{Az} - (F_{By_i}+F_{By_h}) \cdot x & &\textrm{ pour x de a à l}
                    
                    BUG(common_fonction_ajout(action_en_cours->fonctions_efforts[5][num], 0., a, -MAz,         FAy_i+FAy_h, 0., 0., 0., 0., 0., debut_barre) == 0, -3);
                    BUG(common_fonction_ajout(action_en_cours->fonctions_efforts[5][num], a,  l, FBy_i*l-MAz, -FBy_i-FBy_h, 0., 0., 0., 0., 0., debut_barre) == 0, -3);
                    // \end{align*}\begin{verbatim}
                    
                    
    //         Détermination des fonctions de déformation et rotation de la même façon que
    //           pour les sollicitations (cas isostatique + encastrement) :
    //           EF_charge_barre_ponctuelle_fonc_rx,
    //           EF_charge_barre_ponctuelle_fonc_ry,
    //           EF_charge_barre_ponctuelle_fonc_rz,
    //           EF_charge_barre_ponctuelle_n)\begin{align*}
                    BUG(EF_charge_barre_ponctuelle_fonc_rx(action_en_cours->fonctions_rotation[0][num], element_en_beton, pos, a, MAx, MBx) == 0, -3);
                    BUG(EF_charge_barre_ponctuelle_fonc_ry(action_en_cours->fonctions_rotation[1][num], action_en_cours->fonctions_deformation[2][num], element_en_beton, pos, a, ax2[2], ax2[4], -MAy, -MBy) == 0, -3);
                    BUG(EF_charge_barre_ponctuelle_fonc_rz(action_en_cours->fonctions_rotation[2][num], action_en_cours->fonctions_deformation[1][num], element_en_beton, pos, a, ax2[1], ax2[5], -MAz, -MBz) == 0, -3);
                    BUG(EF_charge_barre_ponctuelle_n(action_en_cours->fonctions_deformation[0][num], element_en_beton, pos, a, FAx, FBx) == 0, -3);
                    
                    cholmod_l_free_triplet(&triplet_efforts_locaux_initiaux, projet->ef_donnees.c);
                    
    //         Convertion des réactions d'appuis locales dans le repère global :\end{verbatim}\begin{center}
    //           $\{ R \}_{global} = [K] \cdot \{ F \}_{local}$\end{center}\begin{verbatim}
                    triplet_efforts_locaux_finaux = cholmod_l_allocate_triplet(12, 1, 12, 0, CHOLMOD_REAL, projet->ef_donnees.c);
                    BUGMSG(triplet_efforts_locaux_finaux, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_resoud_charge");
                    ai2 = (long*)triplet_efforts_locaux_finaux->i;
                    aj2 = (long*)triplet_efforts_locaux_finaux->j;
                    ax2 = (double*)triplet_efforts_locaux_finaux->x;
                    triplet_efforts_locaux_finaux->nnz = 12;
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
                    sparse_efforts_locaux_finaux = cholmod_l_triplet_to_sparse(triplet_efforts_locaux_finaux, 0, projet->ef_donnees.c);
                    BUGMSG(sparse_efforts_locaux_finaux, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_resoud_charge");
                    cholmod_l_free_triplet(&triplet_efforts_locaux_finaux, projet->ef_donnees.c);
                    sparse_efforts_globaux_finaux = cholmod_l_ssmult(element_en_beton->matrice_rotation, sparse_efforts_locaux_finaux, 0, 1, 0, projet->ef_donnees.c);
                    BUGMSG(sparse_efforts_globaux_finaux, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_resoud_charge");
                    cholmod_l_free_sparse(&(sparse_efforts_locaux_finaux), projet->ef_donnees.c);
                    triplet_efforts_globaux_finaux = cholmod_l_sparse_to_triplet(sparse_efforts_globaux_finaux, projet->ef_donnees.c);
                    BUGMSG(triplet_efforts_globaux_finaux, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_resoud_charge");
                    ai2 = (long*)triplet_efforts_globaux_finaux->i;
                    aj2 = (long*)triplet_efforts_globaux_finaux->j;
                    ax2 = (double*)triplet_efforts_globaux_finaux->x;
                    cholmod_l_free_sparse(&(sparse_efforts_globaux_finaux), projet->ef_donnees.c);
                    
    //         Ajout des moments et les efforts dans le vecteur des forces aux noeuds {F}
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
                    break;
                }
    //     Sinon Si la charge est une charge répartie uniforme sur la barre Alors
                case CHARGE_BARRE_REPARTIE_UNIFORME :
                {
                    double       xx, yy, zz, l, ll;
                    unsigned int j_d, j_f;                  /* numéro de l'élément dans la discrétisation */
                    Charge_Barre_Repartie_Uniforme *charge_barre = (Charge_Barre_Repartie_Uniforme*)list_curr(action_en_cours->charges);
                    Beton_Barre  *element_en_beton = (Beton_Barre*)charge_barre->barre;
                    
    //         Convertion des efforts globaux en efforts locaux si nécessaire :\end{verbatim}\begin{center}
    //           $\{ F \}_{local} = [R_e]^T \cdot \{ F \}_{global}$\end{center}\begin{verbatim}
                    if (charge_barre->repere_local == FALSE)
                    {
                        triplet_efforts_globaux_initiaux = cholmod_l_allocate_triplet(12, 1, 12, 0, CHOLMOD_REAL, projet->ef_donnees.c);
                        BUGMSG(triplet_efforts_globaux_initiaux, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_resoud_charge");
                        ai2 = (long*)triplet_efforts_globaux_initiaux->i;
                        aj2 = (long*)triplet_efforts_globaux_initiaux->j;
                        ax2 = (double*)triplet_efforts_globaux_initiaux->x;
                        triplet_efforts_globaux_initiaux->nnz = 12;
                    }
                    else
                    {
                        triplet_efforts_locaux_initiaux = cholmod_l_allocate_triplet(12, 1, 12, 0, CHOLMOD_REAL, projet->ef_donnees.c);
                        BUGMSG(triplet_efforts_locaux_initiaux, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_resoud_charge");
                        ai2 = (long*)triplet_efforts_locaux_initiaux->i;
                        aj2 = (long*)triplet_efforts_locaux_initiaux->j;
                        ax2 = (double*)triplet_efforts_locaux_initiaux->x;
                        triplet_efforts_locaux_initiaux->nnz = 12;
                    }
                    xx = element_en_beton->noeud_fin->position.x - element_en_beton->noeud_debut->position.x;
                    yy = element_en_beton->noeud_fin->position.y - element_en_beton->noeud_debut->position.y;
                    zz = element_en_beton->noeud_fin->position.z - element_en_beton->noeud_debut->position.z;
                    ll = sqrt(xx*xx+yy*yy+zz*zz);
                    if (charge_barre->projection == TRUE)
                    {
                        ai2[0] = 0;     aj2[0] = 0;     ax2[0] = charge_barre->fx*sqrt(yy*yy+zz*zz)/ll;
                        ai2[1] = 1;     aj2[1] = 0;     ax2[1] = charge_barre->fy*sqrt(xx*xx+zz*zz)/ll;
                        ai2[2] = 2;     aj2[2] = 0;     ax2[2] = charge_barre->fz*sqrt(xx*xx+yy*yy)/ll;
                        ai2[3] = 3;     aj2[3] = 0;     ax2[3] = charge_barre->mx*sqrt(yy*yy+zz*zz)/ll;
                        ai2[4] = 4;     aj2[4] = 0;     ax2[4] = charge_barre->my*sqrt(xx*xx+zz*zz)/ll;
                        ai2[5] = 5;     aj2[5] = 0;     ax2[5] = charge_barre->mz*sqrt(xx*xx+yy*yy)/ll;
                    }
                    else
                    {
                        ai2[0] = 0;     aj2[0] = 0;     ax2[0] = charge_barre->fx;
                        ai2[1] = 1;     aj2[1] = 0;     ax2[1] = charge_barre->fy;
                        ai2[2] = 2;     aj2[2] = 0;     ax2[2] = charge_barre->fz;
                        ai2[3] = 3;     aj2[3] = 0;     ax2[3] = charge_barre->mx;
                        ai2[4] = 4;     aj2[4] = 0;     ax2[4] = charge_barre->my;
                        ai2[5] = 5;     aj2[5] = 0;     ax2[5] = charge_barre->mz;
                    }
                    ai2[6]  = 6;    aj2[6]  = 0;    ax2[6]  = 0.;
                    ai2[7]  = 7;    aj2[7]  = 0;    ax2[7]  = 0.;
                    ai2[8]  = 8;    aj2[8]  = 0;    ax2[8]  = 0.;
                    ai2[9]  = 9;    aj2[9]  = 0;    ax2[9]  = 0.;
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
                        ai2 = (long*)triplet_efforts_locaux_initiaux->i;
                        aj2 = (long*)triplet_efforts_locaux_initiaux->j;
                        ax2 = (double*)triplet_efforts_locaux_initiaux->x;
                        cholmod_l_free_sparse(&(sparse_efforts_locaux_initiaux), projet->ef_donnees.c);
                    }
                    /* A ce stade ax2 pointent vers les charges dans le repère local*/
                    
    //         Détermination des deux barres discrétisées (j_d et j_f) qui entoure la charge
    //           répartie.
                    if (element_en_beton->discretisation_element == 0)
                    /* Pas de discrétisation */
                    {
                        j_d = 0;
                        j_f = 0;
                    }
                    else
                    /* On cherche le noeud de départ et le noeud de fin */
                    {
                        j_d = 0;
                        l = -1.;
                        /* On regarde pour chaque noeud intermédiaire si la position de la charge devient inférieur à la distance entre le noeud de départ et le noeud intermédiaire */
                        while ((j_d<=element_en_beton->discretisation_element) && (l < charge_barre->a))
                        {
                            if (j_d==element_en_beton->discretisation_element)
                            {
                                xx = element_en_beton->noeud_fin->position.x - element_en_beton->noeud_debut->position.x;
                                yy = element_en_beton->noeud_fin->position.y - element_en_beton->noeud_debut->position.y;
                                zz = element_en_beton->noeud_fin->position.z - element_en_beton->noeud_debut->position.z;
                            }
                            else
                            {
                                xx = element_en_beton->noeuds_intermediaires[j_d]->position.x - element_en_beton->noeud_debut->position.x;
                                yy = element_en_beton->noeuds_intermediaires[j_d]->position.y - element_en_beton->noeud_debut->position.y;
                                zz = element_en_beton->noeuds_intermediaires[j_d]->position.z - element_en_beton->noeud_debut->position.z;
                            }
                            l = sqrt(xx*xx+yy*yy+zz*zz);
                            j_d++;
                        }
                        j_d--;
                        j_f = j_d;
                        l = -1.;
                        /* On regarde pour chaque noeud intermédiaire si la position de la charge devient inférieur à la distance entre le noeud de départ et le noeud intermédiaire */
                        while ((j_f<=element_en_beton->discretisation_element) && (l < ll-charge_barre->b))
                        {
                            if (j_f==element_en_beton->discretisation_element)
                            {
                                xx = element_en_beton->noeud_fin->position.x - element_en_beton->noeud_debut->position.x;
                                yy = element_en_beton->noeud_fin->position.y - element_en_beton->noeud_debut->position.y;
                                zz = element_en_beton->noeud_fin->position.z - element_en_beton->noeud_debut->position.z;
                            }
                            else
                            {
                                xx = element_en_beton->noeuds_intermediaires[j_f]->position.x - element_en_beton->noeud_debut->position.x;
                                yy = element_en_beton->noeuds_intermediaires[j_f]->position.y - element_en_beton->noeud_debut->position.y;
                                zz = element_en_beton->noeuds_intermediaires[j_f]->position.z - element_en_beton->noeud_debut->position.z;
                            }
                            l = sqrt(xx*xx+yy*yy+zz*zz);
                            j_f++;
                        }
                        j_f--;
                    }
                    
    //         Pour chaque barre comprise entre j_d et j_f inclus Faire
                    for (i=j_d;i<=j_f;i++)
                    {
                        double       a, b;                         /* Position de la charge par rapport au début et à la fin de l'élément discrétisé */
                        double       debut_barre, fin_barre;       /* Début et fin de la barre discrétisée par rapport à la barre complète */
                        double       phiAy, phiBy, phiAz, phiBz;   /* Rotation sur appui lorsque la barre est isostatique */
                        double       MAx, MBx, MAy, MBy, MAz, MBz; /* Moments créés par la raideur */
                        double       FAx, FBx;
                        double       FAy_i, FAy_h, FBy_i, FBy_h; /* Réactions d'appui */
                        double       FAz_i, FAz_h, FBz_i, FBz_h;
                        long         *ai4, *aj4;
                        double       *ax4;
                        EF_Noeud     *noeud_debut, *noeud_fin;
                        unsigned int num = element_en_beton->numero;
                        
                        if (i == 0)
                            noeud_debut = element_en_beton->noeud_debut;
                        else
                            noeud_debut = element_en_beton->noeuds_intermediaires[i-1];
                        if (i == element_en_beton->discretisation_element)
                            noeud_fin = element_en_beton->noeud_fin;
                        else
                            noeud_fin = element_en_beton->noeuds_intermediaires[i];
                        xx = noeud_debut->position.x - element_en_beton->noeud_debut->position.x;
                        yy = noeud_debut->position.y - element_en_beton->noeud_debut->position.y;
                        zz = noeud_debut->position.z - element_en_beton->noeud_debut->position.z;
                        debut_barre = sqrt(xx*xx+yy*yy+zz*zz);
                        if (i == j_d)
                            a = charge_barre->a-debut_barre;
                        else
                            a = 0.;
                        xx = noeud_fin->position.x - element_en_beton->noeud_debut->position.x;
                        yy = noeud_fin->position.y - element_en_beton->noeud_debut->position.y;
                        zz = noeud_fin->position.z - element_en_beton->noeud_debut->position.z;
                        fin_barre = sqrt(xx*xx+yy*yy+zz*zz);
                        l = ABS(fin_barre-debut_barre);
                        if (i == j_f)
                        {
                            xx = noeud_fin->position.x - element_en_beton->noeud_fin->position.x;
                            yy = noeud_fin->position.y - element_en_beton->noeud_fin->position.y;
                            zz = noeud_fin->position.z - element_en_beton->noeud_fin->position.z;
                            fin_barre = sqrt(xx*xx+yy*yy+zz*zz);
                            b = charge_barre->b-fin_barre;
                        }
                        else
                            b = 0.;
                        
    //             Détermination des moments mx de rotation
    //               (EF_charge_barre_repartie_uniforme_mx):
                        BUG(EF_charge_barre_repartie_uniforme_mx(element_en_beton, i, a, b, &(element_en_beton->info_EF[i]), ax2[3], &MAx, &MBx) == 0, -3);
                        
    //             Détermination de la rotation y et z aux noeuds de l'élément discrétisé en
    //               le supposant isostatique 
    //               (EF_charge_barre_repartie_uniforme_def_ang_iso_y et z):
                        BUG(EF_charge_barre_repartie_uniforme_def_ang_iso_y(element_en_beton, i, a, b, ax2[2], ax2[4], &phiAy, &phiBy) == 0, -3);
                        BUG(EF_charge_barre_repartie_uniforme_def_ang_iso_z(element_en_beton, i, a, b, ax2[1], ax2[5], &phiAz, &phiBz) == 0, -3);
                        
    //             Calcul des moments créés par les raideurs (EF_calculs_moment_hyper_y et z):
                        BUG(EF_calculs_moment_hyper_y(&(element_en_beton->info_EF[i]), phiAy, phiBy, &MAy, &MBy) == 0, -3);
                        BUG(EF_calculs_moment_hyper_z(&(element_en_beton->info_EF[i]), phiAz, phiBz, &MAz, &MBz) == 0, -3);
                    
    //             Réaction d'appui sur les noeuds (X représente la position de la résultante
    //               pour une force équivalente, déterminée par la fonction
    //               EF_charge_barre_repartie_uniforme_position_resultante_x) :\end{verbatim}\begin{align*}
          // F_{Ax} & = F_x \cdot (L-a-b) \cdot \frac{\int_0^l \frac{1}{E \cdot S(x)}}{\int_X^l \frac{1}{E \cdot S(x)}}\nonumber\\
          // F_{Bx} & = F_x \cdot (L-a-b) - F_{Ax}\nonumber\\
          // F_{Ay_i}   & = \frac{F_y \cdot (L-a-b) \cdot (L-a+b)}{2 \cdot L}-\frac{M_z \cdot (L-a+b)}{l}\nonumber\\
          // F_{Ay_h} & = \frac{M_{Bz}+M_{Az}}{l}\nonumber\\
          // F_{By_i}   & = \frac{F_y \cdot (L-a-b) \cdot (L+a-b)}{2 \cdot L}+\frac{M_z \cdot (L-a+b)}{l}\nonumber\\
          // F_{By_h} & = -\frac{M_{Bz}+M_{Az}}{l}\nonumber\\
          // F_{Az_i}   & = \frac{F_z \cdot (L-a-b) \cdot (L-a+b)}{2 \cdot L}+\frac{M_y \cdot (L-a+b)}{l}\nonumber\\
          // F_{Az_h} & = -\frac{M_{By}+M_{Ay}}{l}\nonumber\\
          // F_{Bz_i}   & = \frac{F_z \cdot (L-a-b) \cdot (L+a-b)}{2 \cdot L}-\frac{M_y \cdot (L-a+b)}{l}\nonumber\\
          // F_{Bz_h} & = \frac{M_{By}+M_{Ay}}{l}\end{align*}\begin{verbatim}
                        FAx = ax2[0]*(l-a-b)*_1992_1_1_sections_es_l(element_en_beton, i, 0, l)/_1992_1_1_sections_es_l(element_en_beton, i, EF_charge_barre_repartie_uniforme_position_resultante_x(element_en_beton->section, a, b, l), l);
                        FBx = ax2[0]*(l-a-b) - FAx;
                        FAy_i = ax2[1]*(l-a-b)*(l-a+b)/(2.*l)-ax2[5]*(l-a-b)/l;
                        FAy_h = (MBz+MAz)/l;
                        FBy_i = ax2[1]*(l-a-b)*(l+a-b)/(2.*l)+ax2[5]*(l-a-b)/l;
                        FBy_h = -(MBz+MAz)/l;
                        FAz_i = ax2[2]*(l-a-b)*(l-a+b)/(2.*l)+ax2[4]*(l-a-b)/l;
                        FAz_h = -(MBy+MAy)/l;
                        FBz_i = ax2[2]*(l-a-b)*(l+a-b)/(2.*l)-ax2[4]*(l-a-b)/l;
                        FBz_h = (MBy+MAy)/l;
                        
    //             Détermination des fonctions des efforts dus à la charge (x, a et l sont
    //               calculés par rapport à l'élément discrétisé et non pour toute la barre).
    //               Pour cela, on calcule la sollicitation due au cas isostatique puis on
    //               ajoute la sollicitation due à l'éventuel encastrement (MAx, MBx, MAy,
    //               MAz, MBy, MBz) :\end{verbatim}\begin{align*}
    //               N(x) & = -F_{Ax} & &\textrm{ pour x de 0 à a}\nonumber\\
    //               N(x) & = \frac{(F_{Ax}+F_{Bx}) \cdot x - a \cdot F_{Bx} - (L-b) \cdot F_{Ax}}{L-a-b} & &\textrm{ pour x de a à L-b}\nonumber\\
    //               N(x) & = F_{Bx} & &\textrm{ pour x de L-b à L}\nonumber\\
                        
                        BUG(common_fonction_ajout(action_en_cours->fonctions_efforts[0][num], 0., a, -FAx, 0., 0., 0., 0., 0., 0., debut_barre) == 0, -3);
                        BUG(common_fonction_ajout(action_en_cours->fonctions_efforts[0][num], a, l-b, (-a*FBx-(l-b)*FAx)/(l-a-b), (FAx+FBx)/(l-a-b), 0., 0., 0., 0., 0., debut_barre) == 0, -3);
                        BUG(common_fonction_ajout(action_en_cours->fonctions_efforts[0][num], l-b, l, FBx,  0., 0., 0., 0., 0., 0., debut_barre) == 0, -3);
                        
                    // T_y(x) & = -F_{Ay_i} - F_{Ay_h} & &\textrm{ pour x de 0 à a}\nonumber\\
                    // T_y(x) & = \frac{-a (F_{By_i}+F_{By_h})-(L-b)(F_{Ay_i}+F_{Ay_h})}{L-a-b} + \frac{F_{Ay_i}+F_{Ay_h}+F_{By_i}+F_{By_h}}{L-a-b} x & &\textrm{ pour x de a à L-b}\nonumber\\
                    // T_y(x) & =  F_{By_i} + F_{By_h} & &\textrm{ pour x de L-b à L}\nonumber\\
                        
                        BUG(common_fonction_ajout(action_en_cours->fonctions_efforts[1][num], 0., a,  -FAy_i-FAy_h, 0., 0., 0., 0., 0., 0., debut_barre) == 0, -3);
                        BUG(common_fonction_ajout(action_en_cours->fonctions_efforts[1][num], a, l-b, (-a*(FBy_i+FBy_h)-(l-b)*(FAy_i+FAy_h))/(l-a-b), (FAy_i+FAy_h+FBy_i+FBy_h)/(l-a-b), 0., 0., 0., 0., 0., debut_barre) == 0, -3);
                        BUG(common_fonction_ajout(action_en_cours->fonctions_efforts[1][num], l-b, l, FBy_i+FBy_h, 0., 0., 0., 0., 0., 0., debut_barre) == 0, -3);
                        
                    // T_z(x) & = -F_{Az_i} - F_{Az_h} & &\textrm{ pour x de 0 à a}\nonumber\\
                    // T_z(x) & = \frac{-a (F_{Bz_i}+F_{Bz_h})-(L-b) (F_{Az_i}+F_{Az_h})}{L-a-b} + \frac{F_{Az_i}+F_{Az_h}+F_{Bz_i}+F_{Bz_h}}{L-a-b} x & &\textrm{ pour x de a à L-b}\nonumber\\
                    // T_z(x) & =  F_{Bz_i} + F_{Bz_h} & &\textrm{ pour x de L-b à L}\nonumber\\
                        
                        BUG(common_fonction_ajout(action_en_cours->fonctions_efforts[2][num], 0., a, -FAz_i-FAz_h, 0., 0., 0., 0., 0., 0., debut_barre) == 0, -3);
                        BUG(common_fonction_ajout(action_en_cours->fonctions_efforts[2][num], a, l-b, (-a*(FBz_i+FBz_h)-(l-b)*(FAz_i+FAz_h))/(l-a-b), (FAz_i+FAz_h+FBz_i+FBz_h)/(l-a-b), 0., 0., 0., 0., 0., debut_barre) == 0, -3);
                        BUG(common_fonction_ajout(action_en_cours->fonctions_efforts[2][num], l-b,  l,  FBz_i+FBz_h, 0., 0., 0., 0., 0., 0., debut_barre) == 0, -3);
                        
                    // M_x(x) & = -M_{Ax} & &\textrm{ pour x de 0 à a}\nonumber\\
                    // M_x(x) & = \frac{-a \cdot M_{Bx}-(L-b) \cdot M_{Ax}}{L-a-b} + \frac{M_{Ax}+M_{Bx}}{L-a-b} \cdot x & &\textrm{ pour x de a à L-b}\nonumber\\
                    // M_x(x) & = M_{Bx} & &\textrm{ pour x de L-b à L}\nonumber\\
                        
                        BUG(common_fonction_ajout(action_en_cours->fonctions_efforts[3][num], 0., a,  -MAx, 0., 0., 0., 0., 0., 0., debut_barre) == 0, -3);
                        BUG(common_fonction_ajout(action_en_cours->fonctions_efforts[3][num], a, l-b, (-a*MBx-(l-b)*MAx)/(l-a-b), (MAx+MBx)/(l-a-b), 0., 0., 0., 0., 0., debut_barre) == 0, -3);
                        BUG(common_fonction_ajout(action_en_cours->fonctions_efforts[3][num], l-b,  l, MBx, 0., 0., 0., 0., 0., 0., debut_barre) == 0, -3);
                        
                    // M_y(x) & = -M_{Ay} - (F_{Az_i}+F_{Az_h}) \cdot x & &\textrm{ pour x de 0 à a}\nonumber\\
                    // M_y(x) & = -(F_{Bz_i}+F_{Bz_h}) \cdot (L-x) + M_{By} - M_y \cdot (L-b-x) + F_z \cdot \frac{(L-b-x)^2}{2} & &\textrm{ pour x de a à L-b}\nonumber\\
                    // M_y(x) & =  M_{By} - L \cdot (F_{Bz_i}+F_{Bz_h}) + (F_{Bz_i}+F_{Bz_h}) \cdot x & &\textrm{ pour x de L-b à L}\nonumber\\
                        
                        BUG(common_fonction_ajout(action_en_cours->fonctions_efforts[4][num], 0., a, -MAy, -FAz_i-FAz_h, 0., 0., 0., 0., 0., debut_barre) == 0, -3);
                        BUG(common_fonction_ajout(action_en_cours->fonctions_efforts[4][num], a, l-b, b*b*ax2[2]/2.+b*(ax2[4]-ax2[2]*l)+ax2[2]*l*l/2.-l*(ax2[4]+FBz_i+FBz_h)+MBy, -ax2[2]*(l-b)+ax2[4]+FBz_i+FBz_h, ax2[2]/2., 0., 0., 0., 0., debut_barre) == 0, -3);
                        BUG(common_fonction_ajout(action_en_cours->fonctions_efforts[4][num], l-b,  l, MBy-l*(FBz_i+FBz_h), FBz_i+FBz_h, 0., 0., 0., 0., 0., debut_barre) == 0, -3);
                        
                    // M_z(x) & = -M_{Az} + (F_{Ay_i}+F_{Ay_h}) \cdot x & &\textrm{ pour x de 0 à a}\nonumber\\
                    // M_z(x) & = (F_{By_i}+F_{By_h}) \cdot (L-x) - M_{Bz} - M_z \cdot (L-b-x) - F_y \cdot \frac{(L-b-x)^2}{2} & &\textrm{ pour x de a à L-b}\nonumber\\
                    // M_z(x) & =  M_{Bz} + L \cdot (F_{By_i}+F_{By_h}) - (F_{By_i}+F_{By_h}) \cdot x & &\textrm{ pour x de L-b à L}
                        
                        BUG(common_fonction_ajout(action_en_cours->fonctions_efforts[5][num], 0., a, -MAz, +FAy_i+FAy_h, 0., 0., 0., 0., 0., debut_barre) == 0, -3);
                        BUG(common_fonction_ajout(action_en_cours->fonctions_efforts[5][num], a, l-b, -b*b*ax2[1]/2.+b*(ax2[1]*l+ax2[5])-ax2[1]*l*l/2.+l*(FBy_i+FBy_h-ax2[5])+MBz, ax2[1]*(l-b)+ax2[5]-FBy_i-FBy_h, -ax2[1]/2., 0., 0., 0., 0., debut_barre) == 0, -3);
                        BUG(common_fonction_ajout(action_en_cours->fonctions_efforts[5][num], l-b,  l, MBz+l*(FBy_i+FBy_h), -FBy_i-FBy_h, 0., 0., 0., 0., 0., debut_barre) == 0, -3);
                    // \end{align*}\begin{verbatim}
                        
    //             Détermination des fonctions de déformation et rotation de la même façon que
    //             pour les sollicitations (cas isostatique + encastrement) :
    //               EF_charge_barre_repartie_uniforme_fonc_rx,
    //               EF_charge_barre_repartie_uniforme_fonc_ry,
    //               EF_charge_barre_repartie_uniforme_fonc_rz,
    //               EF_charge_barre_repartie_uniforme_n) :
                        BUG(EF_charge_barre_repartie_uniforme_fonc_rx(action_en_cours->fonctions_rotation[0][num], element_en_beton, i, a, b, MAx, MBx) == 0, -3);
                        BUG(EF_charge_barre_repartie_uniforme_fonc_ry(action_en_cours->fonctions_rotation[1][num], action_en_cours->fonctions_deformation[2][num], element_en_beton, i, a, b, ax2[2], ax2[4], MAy, MBy) == 0, -3);
                        BUG(EF_charge_barre_repartie_uniforme_fonc_rz(action_en_cours->fonctions_rotation[2][num], action_en_cours->fonctions_deformation[1][num], element_en_beton, i, a, b, ax2[1], ax2[5], MAz, MBz) == 0, -3);
                        BUG(EF_charge_barre_repartie_uniforme_n(action_en_cours->fonctions_deformation[0][num], element_en_beton, i, a, b, FAx, FBx) == 0, -3);
                    
    //             Convertion des réactions d'appuis locales dans le repère global :\end{verbatim}\begin{center}
    //           $\{ R \}_{global} = [K] \cdot \{ F \}_{local}$\end{center}\begin{verbatim}
                        triplet_efforts_locaux_finaux = cholmod_l_allocate_triplet(12, 1, 12, 0, CHOLMOD_REAL, projet->ef_donnees.c);
                        BUGMSG(triplet_efforts_locaux_finaux, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_resoud_charge");
                        ai4 = (long*)triplet_efforts_locaux_finaux->i;
                        aj4 = (long*)triplet_efforts_locaux_finaux->j;
                        ax4 = (double*)triplet_efforts_locaux_finaux->x;
                        triplet_efforts_locaux_finaux->nnz = 12;
                        ai4[0] = 0;   aj4[0] = 0;  ax4[0] = FAx;
                        ai4[1] = 1;   aj4[1] = 0;  ax4[1] = FAy_i+FAy_h;
                        ai4[2] = 2;   aj4[2] = 0;  ax4[2] = FAz_i+FAz_h;
                        ai4[3] = 3;   aj4[3] = 0;  ax4[3] = MAx;
                        ai4[4] = 4;   aj4[4] = 0;  ax4[4] = MAy;
                        ai4[5] = 5;   aj4[5] = 0;  ax4[5] = MAz;
                        ai4[6] = 6;   aj4[6] = 0;  ax4[6] = FBx;
                        ai4[7] = 7;   aj4[7] = 0;  ax4[7] = FBy_i+FBy_h;
                        ai4[8] = 8;   aj4[8] = 0;  ax4[8] = FBz_i+FBz_h;
                        ai4[9] = 9;   aj4[9] = 0;  ax4[9] = MBx;
                        ai4[10] = 10; aj4[10] = 0; ax4[10] = MBy;
                        ai4[11] = 11; aj4[11] = 0; ax4[11] = MBz;
                        sparse_efforts_locaux_finaux = cholmod_l_triplet_to_sparse(triplet_efforts_locaux_finaux, 0, projet->ef_donnees.c);
                        BUGMSG(sparse_efforts_locaux_finaux, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_resoud_charge");
                        cholmod_l_free_triplet(&triplet_efforts_locaux_finaux, projet->ef_donnees.c);
                        sparse_efforts_globaux_finaux = cholmod_l_ssmult(element_en_beton->matrice_rotation, sparse_efforts_locaux_finaux, 0, 1, 0, projet->ef_donnees.c);
                        BUGMSG(sparse_efforts_globaux_finaux, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_resoud_charge");
                        cholmod_l_free_sparse(&(sparse_efforts_locaux_finaux), projet->ef_donnees.c);
                        triplet_efforts_globaux_finaux = cholmod_l_sparse_to_triplet(sparse_efforts_globaux_finaux, projet->ef_donnees.c);
                        BUGMSG(triplet_efforts_globaux_finaux, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_resoud_charge");
                        ai4 = (long*)triplet_efforts_globaux_finaux->i;
                        aj4 = (long*)triplet_efforts_globaux_finaux->j;
                        ax4 = (double*)triplet_efforts_globaux_finaux->x;
                        cholmod_l_free_sparse(&(sparse_efforts_globaux_finaux), projet->ef_donnees.c);
                    
    //             Ajout des moments et les efforts dans le vecteur des forces aux noeuds {F}
                        for (j=0;j<12;j++)
                        {
                            if (ai4[j] < 6)
                            {
                                if (projet->ef_donnees.noeuds_pos_partielle[noeud_debut->numero][ai4[j]] != -1)
                                    ax[projet->ef_donnees.noeuds_pos_partielle[noeud_debut->numero][ai4[j]]] += ax4[j];
                                ax3[projet->ef_donnees.noeuds_pos_complete[noeud_debut->numero][ai4[j]]] += ax4[j];
                            }
                            else
                            {
                                if (projet->ef_donnees.noeuds_pos_partielle[noeud_fin->numero][ai4[j]-6] != -1)
                                    ax[projet->ef_donnees.noeuds_pos_partielle[noeud_fin->numero][ai4[j]-6]] += ax4[j];
                                ax3[projet->ef_donnees.noeuds_pos_complete[noeud_fin->numero][ai4[j]-6]] += ax4[j];
                            }
                        }
                        cholmod_l_free_triplet(&triplet_efforts_globaux_finaux, projet->ef_donnees.c);
                    }
    //         FinPour
                    cholmod_l_free_triplet(&triplet_efforts_locaux_initiaux, projet->ef_donnees.c);
                    break;
                }
    //     FinSi
                /* Charge inconnue */
                default :
                {
                    BUG(0, -1);
                    break;
                }
            }
        }
        while (list_mvnext(action_en_cours->charges) != NULL);
    }
    // FinPour
    
    /* On converti les données dans des structures permettant les calculs via les libraries */
    action_en_cours->forces_complet = cholmod_l_triplet_to_sparse(triplet_force_complete, 0, projet->ef_donnees.c);
    BUGMSG(action_en_cours->forces_complet, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_resoud_charge");
    cholmod_l_free_triplet(&triplet_force_complete, projet->ef_donnees.c);
    
    // Calcul des déplacements des noeuds :\end{verbatim}\begin{align*}
    // \{ \Delta \}_{global} = [K]^{-1} \cdot \{ F \}_{global}\end{align*}\begin{verbatim}
    
    triplet_deplacements_partiels = cholmod_l_allocate_triplet(projet->ef_donnees.rigidite_matrice_partielle->nrow, 1, projet->ef_donnees.rigidite_matrice_partielle->nrow, 0, CHOLMOD_REAL, projet->ef_donnees.c);
    BUGMSG(triplet_deplacements_partiels, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_resoud_charge");
    triplet_deplacements_partiels->nnz = projet->ef_donnees.rigidite_matrice_partielle->nrow;
    ai = (long*)triplet_deplacements_partiels->i;
    aj = (long*)triplet_deplacements_partiels->j;
    ax = (double*)triplet_deplacements_partiels->x;
    if (projet->ef_donnees.rigidite_matrice_partielle->nrow != 0)
    {
        ax2 = (double*)triplet_force_partielle->x;
        BUGMSG(umfpack_dl_solve(UMFPACK_A, projet->ef_donnees.ap, projet->ef_donnees.ai, projet->ef_donnees.ax, ax, ax2, projet->ef_donnees.numeric, NULL, NULL) == UMFPACK_OK, -3, gettext("Erreur lors de la résolution de la matrice.\n"));
        projet->ef_donnees.residu = EF_calculs_resid (projet->ef_donnees.ap, projet->ef_donnees.ai, projet->ef_donnees.ax, ax2, projet->ef_donnees.triplet_rigidite_partielle->nrow, ax);
        printf("Residu sur les déplacements : %g\n", projet->ef_donnees.residu);
        for (i=0;i<projet->ef_donnees.rigidite_matrice_partielle->nrow;i++)
        {
            ai[i] = i;
            aj[i] = 0;
        }
    }
    else
        projet->ef_donnees.residu = 0.;
    cholmod_l_free_triplet(&triplet_force_partielle, projet->ef_donnees.c);
    
    /* Création du vecteur déplacement complet */
    triplet_deplacements_totaux = cholmod_l_allocate_triplet(action_en_cours->forces_complet->nrow, 1, action_en_cours->forces_complet->nrow, 0, CHOLMOD_REAL, projet->ef_donnees.c);
    BUGMSG(triplet_deplacements_totaux, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_resoud_charge");
    triplet_deplacements_totaux->nnz = action_en_cours->forces_complet->nrow;
    ai2 = (long*)triplet_deplacements_totaux->i;
    aj2 = (long*)triplet_deplacements_totaux->j;
    ax2 = (double*)triplet_deplacements_totaux->x;
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
    
    // Calcule des réactions d'appuis :\end{verbatim}\begin{displaymath}
    // \{F\} = [K] \cdot \{\Delta\} - \{F_0\} \end{displaymath}\begin{verbatim}
    sparse_tmp = cholmod_l_ssmult(projet->ef_donnees.rigidite_matrice_complete, action_en_cours->deplacement_complet, 0, TRUE, TRUE, projet->ef_donnees.c);
    BUGMSG(sparse_tmp, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_resoud_charge");
    action_en_cours->efforts_noeuds = cholmod_l_add(sparse_tmp, action_en_cours->forces_complet, one, minusone, TRUE, TRUE, projet->ef_donnees.c);
    BUGMSG(action_en_cours->efforts_noeuds, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_resoud_charge");
    cholmod_l_free_sparse(&sparse_tmp, projet->ef_donnees.c);
    
    // Pour chaque barre, ajout des efforts et déplacement dus au mouvement de l'ensemble de
    //   la structure.
    list_mvfront(projet->beton.barres);
    do
    {
        Beton_Barre                 *element_en_beton = (Beton_Barre*)list_curr(projet->beton.barres);
        Beton_Section_Rectangulaire *section_tmp = (Beton_Section_Rectangulaire*)element_en_beton->section;
        double                      S = _1992_1_1_sections_s(section_tmp);
        
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
            switch (element_en_beton->type)
            {
                case BETON_ELEMENT_POTEAU :
                case BETON_ELEMENT_POUTRE :
                {
                    E = element_en_beton->materiau->ecm;
                    G = element_en_beton->materiau->gnu_0_2;
                    break;
                }
                default :
                {
                    BUG(0, -1);
                }
            }
            
    //         Récupération des déplacements du noeud de départ et du noeud final de l'élément
            triplet_deplacement_globaux = cholmod_l_allocate_triplet(12, 1, 12, 0, CHOLMOD_REAL, projet->ef_donnees.c);
            BUGMSG(triplet_deplacement_globaux, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_resoud_charge");
            ai = (long*)triplet_deplacement_globaux->i;
            aj = (long*)triplet_deplacement_globaux->j;
            ax = (double*)triplet_deplacement_globaux->x;
            triplet_deplacement_globaux->nnz = 12;
            ax2 = (double*)triplet_deplacements_totaux->x;
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
            
    //         Conversion des déplacements globaux en déplacement locaux (u_A, v_A, w_A,
    //           theta_{Ax}, theta_{Ay}, theta_{Az}, u_B, v_B, w_B, theta_{Bx}, theta_{By} et
    //           theta_{Bz}) : \end{verbatim}\begin{align*}
            // \{ \Delta \}_{local} = [R]^T \cdot \{ \Delta \}_{global}\end{align*}\begin{verbatim}
            sparse_deplacement_globaux = cholmod_l_triplet_to_sparse(triplet_deplacement_globaux, 0, projet->ef_donnees.c);
            BUGMSG(sparse_deplacement_globaux, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_resoud_charge");
            cholmod_l_free_triplet(&triplet_deplacement_globaux, projet->ef_donnees.c);
            sparse_deplacement_locaux = cholmod_l_ssmult(element_en_beton->matrice_rotation_transpose, sparse_deplacement_globaux, 0, 1, TRUE, projet->ef_donnees.c);
            BUGMSG(sparse_deplacement_locaux, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_resoud_charge");
            cholmod_l_free_sparse(&sparse_deplacement_globaux, projet->ef_donnees.c);
    //         Détermination des efforts (F_{Ax}, F_{Bx}, F_{Ay}, F_{By}, F_{Az}, F_{Bz},
    //           M_{Ax}, M_{Bx}, M_{Ay}, M_{By}, M_{Az} et M_{Bz}) dans le repère local : \end{verbatim}\begin{align*}
            // \{ F \}_{local} = [K] \cdot \{ \Delta \}_{local}\end{align*}\begin{verbatim}
            sparse_effort_locaux = cholmod_l_ssmult(element_en_beton->info_EF[j].matrice_rigidite_locale, sparse_deplacement_locaux, 0, 1, TRUE, projet->ef_donnees.c);
            BUGMSG(sparse_effort_locaux, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_calculs_resoud_charge");
            
            ax = (double*)sparse_deplacement_locaux->x;
            ax2 = (double*)sparse_effort_locaux->x;
            xx = noeud_debut->position.x - element_en_beton->noeud_debut->position.x;
            yy = noeud_debut->position.y - element_en_beton->noeud_debut->position.y;
            zz = noeud_debut->position.z - element_en_beton->noeud_debut->position.z;
            l_debut = sqrt(xx*xx+yy*yy+zz*zz);
            xx = noeud_fin->position.x - element_en_beton->noeud_debut->position.x;
            yy = noeud_fin->position.y - element_en_beton->noeud_debut->position.y;
            zz = noeud_fin->position.z - element_en_beton->noeud_debut->position.z;
            l_fin = sqrt(xx*xx+yy*yy+zz*zz);
            l = ABS(l_fin-l_debut);
            
    //         Ajout des efforts entre deux noeuds dus à leur déplacement relatif, la courbe
    //           vient s'ajouter à la courbe (si existante) déja définie précédemment.
    //           L'indice A correspond au noeud de départ et l'indice B correspond au noeud
    //           final :\end{verbatim}\begin{align*}
            // N(x) & = N(x) + F_{Ax} - (F_{Ax}+F_{Bx}) \cdot x/L\nonumber\\
            // T_y(x) & = T_y(x) + F_{Ay} - (F_{Ay}+F_{By}) \cdot x/L\nonumber\\
            // T_z(x) & = T_z(x) + F_{Az} - (F_{Az}+F_{Bz}) \cdot x/L\nonumber\\
            // M_x(x) & = M_x(x) + M_{Ax} - (M_{Ax}+M_{Bx}) \cdot x/L\nonumber\\
            // M_y(x) & = M_y(x) + M_{Ay} - (M_{Ay}+M_{By}) \cdot x/L\nonumber\\
            // M_z(x) & = M_z(x) + M_{Az} - (M_{Az}+M_{Bz}) \cdot x/L
            // \end{align*}\begin{verbatim}
            BUG(common_fonction_ajout(action_en_cours->fonctions_efforts[0][element_en_beton->numero], 0., l, ax2[0], -(ax2[0]+ax2[6])/l,  0., 0., 0., 0., 0., l_debut) == 0, -3);
            BUG(common_fonction_ajout(action_en_cours->fonctions_efforts[1][element_en_beton->numero], 0., l, ax2[1], -(ax2[1]+ax2[7])/l,  0., 0., 0., 0., 0., l_debut) == 0, -3);
            BUG(common_fonction_ajout(action_en_cours->fonctions_efforts[2][element_en_beton->numero], 0., l, ax2[2], -(ax2[2]+ax2[8])/l,  0., 0., 0., 0., 0., l_debut) == 0, -3);
            BUG(common_fonction_ajout(action_en_cours->fonctions_efforts[3][element_en_beton->numero], 0., l, ax2[3], -(ax2[3]+ax2[9])/l,  0., 0., 0., 0., 0., l_debut) == 0, -3);
            BUG(common_fonction_ajout(action_en_cours->fonctions_efforts[4][element_en_beton->numero], 0., l, ax2[4], -(ax2[4]+ax2[10])/l, 0., 0., 0., 0., 0., l_debut) == 0, -3);
            BUG(common_fonction_ajout(action_en_cours->fonctions_efforts[5][element_en_beton->numero], 0., l, ax2[5], -(ax2[5]+ax2[11])/l, 0., 0., 0., 0., 0., l_debut) == 0, -3);
            
    //         Ajout des déplacements & rotations entre deux noeuds dus à leur déplacement
    //           relatif, la courbe vient s'ajouter à la courbe (si existante) déja définie
    //           précédemment.
    //         Dans le cas d'une section constante, la déformée en x est obtenue par
    //           intégration de la courbe d'effort normal et la déformée en y et z est
    //           obtenue par double intégration de la courbe des moments. Les deux constantes
    //           sont obtenues par la connaissance des déplacements en x=0 et x=l. Les
    //           rotations (ry et rz) sont obtenues par la dérivée de la flèche. La rotation
    //           rx est obtenue par intégration du moment en x. La constante est déterminée
    //           pour f(0) égal à la rotation au noeud à gauche s'il n'y a pas de relachement
    //           (sinon f(l) égal à la rotation à droite).\end{verbatim}\begin{align*}
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
                    
    //              f_x(x) = & u_A - \frac{F_{Ax}}{E \cdot S} \cdot x + \frac{F_{Ax}+F_{Bx}}{2 \cdot E \cdot S \cdot L} \cdot x^2 \nonumber\\
    //              f_y(x) = & v_A + A \cdot x - \frac{M_{Az}}{2 \cdot E \cdot I_z} \cdot x^2 + \frac{M_{Az}+M_{Bz}}{6 \cdot L \cdot E \cdot I_z} \cdot x^3 \text{ avec } \nonumber\\
    //                       & \text{A tel que } f_y(l)=v_B \text{ soit } A = \frac{M_{Az} \cdot L}{2*E*Iz}-\frac{(M_{Az}+M_{Bz}) \cdot L}{6*E*Iz}+\frac{-v_A+v_B}{L} \nonumber\\
    //              f_z(x) = & w_A + B \cdot x + \frac{M_{Ay}}{2 \cdot E \cdot I_y} \cdot x^2 - \frac{M_{Ay}+M_{By}}{6 \cdot L \cdot E \cdot I_y} \cdot x^3 \text{ avec } \nonumber\\
    //                       & \text{B tel que } f_z(l)=w_B \text{ soit } B = -\frac{M_{Ay} \cdot L}{2*E*I_y}+\frac{(M_{Ay}+M_{By}) \cdot L}{6*E*I_y}+\frac{-w_A+w_B}{L} \nonumber\\
    //              r_x(x) = & C - \frac{M_{Ax}}{G \cdot J} \cdot x + \frac{M_{Ax}+M_{Bx}}{2 \cdot G \cdot J \cdot L} \cdot x^2 \text{ avec }\nonumber\\
    //                       & \text{C tel que } r_x(l)=\theta_{Bx}  \text{ soit } C = \frac{M_{Ax} \cdot L}{G \cdot J}-\frac{(M_{Ax}+M_{Bx}) \cdot L}{2 \cdot G \cdot J}+\theta_{Bx} \nonumber\\
    //                       & \text{C tel que } r_x(0)=\theta_{Ax}  \text{ soit } C = \theta_{Ax} \nonumber\\
    //                       & \text{Solution 1 si le noeud A n'est pas encastrée et solution 2 si le noeud B n'est pas encastrée} \nonumber\\
    //              r_y(x) = & -B - \frac{M_{Ay}}{E \cdot I_y} \cdot x + \frac{M_{Ay}+M_{By}}{2 \cdot L \cdot E \cdot I_y} \cdot x^2 \nonumber\\
    //              r_z(x) = & A - \frac{M_{Az}}{E \cdot I_z} \cdot x + \frac{M_{Az}+M_{Bz}}{2 \cdot L \cdot E \cdot I_z} \cdot x^2

                    BUG(common_fonction_ajout(action_en_cours->fonctions_deformation[0][element_en_beton->numero], 0., l, ax[0], -ax2[0]/(E*S), (ax2[0]+ax2[6])/(2*E*S*l), 0., 0., 0., 0., l_debut) == 0, -3);
                    BUG(common_fonction_ajout(action_en_cours->fonctions_deformation[1][element_en_beton->numero], 0., l, ax[1], (ax2[5]/(2*E*Iz)*l*l-(ax2[5]+ax2[11])/(6*E*Iz)*l*l-ax[1]+ax[7])/l, -ax2[5]/(2*E*Iz),  (ax2[5]+ax2[11])/(6*l*E*Iz), 0., 0., 0., l_debut) == 0, -3);
                    BUG(common_fonction_ajout(action_en_cours->fonctions_deformation[2][element_en_beton->numero], 0., l, ax[2], (-ax2[4]/(2*E*Iy)*l*l+(ax2[4]+ax2[10])/(6*E*Iy)*l*l-ax[2]+ax[8])/l,  ax2[4]/(2*E*Iy), -(ax2[4]+ax2[10])/(6*l*E*Iy), 0., 0., 0., l_debut) == 0, -3);
                    if ((j == 0) && (element_en_beton->relachement != NULL) && (element_en_beton->relachement->rx_debut != EF_RELACHEMENT_BLOQUE))
                        BUG(common_fonction_ajout(action_en_cours->fonctions_rotation[0][element_en_beton->numero], 0., l, ax2[3]/(G*J)*l-(ax2[3]+ax2[9])/(2*G*J*l)*l*l+ax[9], -ax2[3]/(G*J), (ax2[3]+ax2[9])/(2*G*J*l), 0., 0., 0., 0., l_debut) == 0, -3);
                    else
                        BUG(common_fonction_ajout(action_en_cours->fonctions_rotation[0][element_en_beton->numero], 0., l, ax[3], -ax2[3]/(G*J), (ax2[3]+ax2[9])/(2*G*J*l), 0., 0., 0., 0., l_debut) == 0, -3);
                    BUG(common_fonction_ajout(action_en_cours->fonctions_rotation[1][element_en_beton->numero], 0., l, -(-ax2[4]/(2*E*Iy)*l*l+(ax2[4]+ax2[10])/(6*E*Iy)*l*l-ax[2]+ax[8])/l, -ax2[4]/(E*Iy), (ax2[4]+ax2[10])/(2*l*E*Iy), 0., 0., 0., 0., l_debut) == 0, -3);
                    BUG(common_fonction_ajout(action_en_cours->fonctions_rotation[2][element_en_beton->numero], 0., l,  (ax2[5]/(2*E*Iz)*l*l+(-ax2[5]-ax2[11])/(6*E*Iz)*l*l-ax[1]+ax[7])/l, -ax2[5]/(E*Iz), (ax2[5]+ax2[11])/(2*l*E*Iz), 0., 0., 0., 0., l_debut) == 0, -3);
    //              \end{align*}\begin{verbatim}
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
