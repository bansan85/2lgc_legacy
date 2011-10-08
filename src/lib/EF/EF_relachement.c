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
#include "common_projet.h"
#include "common_erreurs.h"
#include "EF_relachement.h"

int EF_relachement_init(Projet *projet)
/* Description : Initialise la liste des relachements
 * Paramètres : Projet *projet : la variable projet
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet == NULL)
 *           -2 en cas d'erreur d'allocation mémoire
 */
{
    BUGMSG(projet, -1, "EF_relachement_init\n");
    
    // Trivial
    projet->ef_donnees.relachements = list_init();
    BUGMSG(projet->ef_donnees.relachements, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_relachement_init");
    
    return 0;
}


int EF_relachement_ajout(Projet *projet, EF_Relachement_Type rx_debut, void* rx_d_data,
  EF_Relachement_Type ry_debut, void* ry_d_data, EF_Relachement_Type rz_debut, void* rz_d_data,
  EF_Relachement_Type rx_fin, void* rx_f_data, EF_Relachement_Type ry_fin, void* ry_f_data,
  EF_Relachement_Type rz_fin, void* rz_f_data)
/* Description : Ajoute un relachement en lui attribuant le numéro suivant le dernier
 *                 relachement existant. Les données fournis dans les paramètres additionnels
 *                 doivent avoir été stockées en mémoire par l'utilisation de malloc.
 * Paramètres : Projet *projet : la variable projet
 *            : Type_EF_Appui rx_debut : relachement de la rotation autour de l'axe x au début
 *            : void* rx_d_data : paramètre additionnel de la rotation si nécessaire
 *            : Type_EF_Appui ry_debut : relachement de la rotation autour de l'axe y au début
 *            : void* ry_d_data : paramètre additionnel de la rotation si nécessaire
 *            : Type_EF_Appui rz_debut : relachement de la rotation autour de l'axe z au début
 *            : void* rz_d_data : paramètre additionnel de la rotation si nécessaire
 *            : Type_EF_Appui rx_fin : relachement de la rotation autour de l'axe x à la fin
 *            : void* rx_f_data : paramètre additionnel de la rotation si nécessaire
 *            : Type_EF_Appui ry_fin : relachement de la rotation autour de l'axe y à la fin
 *            : void* ry_f_data : paramètre additionnel de la rotation si nécessaire
 *            : Type_EF_Appui rz_fin : relachement de la rotation autour de l'axe z à la fin
 *            : void* rz_f_data : paramètre additionnel de la rotation si nécessaire
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet == NULL)
 *             (projet->ef_donnees.relachements == NULL)
 *             (rx_debut == EF_RELACHEMENT_LIBRE) && (rx_fin == EF_RELACHEMENT_LIBRE)
 */
{
    EF_Relachement     *relachement_en_cours, relachement_nouveau;
    
    // Trivial
    
    BUGMSG(projet, -1, "EF_relachement_ajout\n");
    BUGMSG(projet->ef_donnees.relachements, -1, "EF_relachement_ajout\n");
    BUG(!((rx_debut == EF_RELACHEMENT_LIBRE) && (rx_fin == EF_RELACHEMENT_LIBRE)), -1);
    
    list_mvrear(projet->ef_donnees.relachements);
    relachement_nouveau.rx_debut = rx_debut;
    relachement_nouveau.rx_d_data = rx_d_data;
    relachement_nouveau.ry_debut = ry_debut;
    relachement_nouveau.ry_d_data = ry_d_data;
    relachement_nouveau.rz_debut = rz_debut;
    relachement_nouveau.rz_d_data = rz_d_data;
    relachement_nouveau.rx_fin = rx_fin;
    relachement_nouveau.rx_f_data = rx_f_data;
    relachement_nouveau.ry_fin = ry_fin;
    relachement_nouveau.ry_f_data = ry_f_data;
    relachement_nouveau.rz_fin = rz_fin;
    relachement_nouveau.rz_f_data = rz_f_data;
    
    relachement_en_cours = (EF_Relachement *)list_rear(projet->ef_donnees.relachements);
    if (relachement_en_cours == NULL)
        relachement_nouveau.numero = 0;
    else
        relachement_nouveau.numero = relachement_en_cours->numero+1;
    
    BUGMSG(list_insert_after(projet->ef_donnees.relachements, &(relachement_nouveau), sizeof(relachement_nouveau)), -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_relachement_ajout");
    
    return 0;
}


EF_Relachement* EF_relachement_cherche_numero(Projet *projet, unsigned int numero)
/* Description : Renvoie le relachement cherché
 * Paramètres : Projet *projet : la variable projet
 *            : unsigned int numero : le numéro du relachement
 * Valeur renvoyée :
 *   Succès : pointeur vers le relachement recherché
 *   Échec : NULL en cas de paramètres invalides :
 *             (projet == NULL) ou
 *             (projet->ef_donnees.relachements == NULL) ou
 *             (list_size(projet->ef_donnees.relachements) == 0) ou
 *             relachement introuvable.
 */
{
    BUGMSG(projet, NULL, "EF_relachement_cherche_numero\n");
    BUGMSG(projet->ef_donnees.relachements, NULL, "EF_relachement_cherche_numero\n");
    BUGMSG(list_size(projet->ef_donnees.relachements), NULL, "EF_relachement_cherche_numero\n");
    
    // Trivial
    list_mvfront(projet->ef_donnees.relachements);
    do
    {
        EF_Relachement *relachement = (EF_Relachement*)list_curr(projet->ef_donnees.relachements);
        
        if (relachement->numero == numero)
            return relachement;
    }
    while (list_mvnext(projet->ef_donnees.relachements) != NULL);
    
    BUGMSG(0, NULL, gettext("Relachement n°%d introuvable.\n"), numero);
}


int EF_relachement_free(Projet *projet)
/* Description : Libère l'ensemble des relachements et la liste les contenant
 * Paramètres : Projet *projet : la variable projet
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet == NULL) ou
 *             (projet->ef_donnees.relachements == NULL)
 */
{
    BUGMSG(projet, -1, "EF_relachement_free\n");
    BUGMSG(projet->ef_donnees.relachements, -1, "EF_relachement_free\n");
    
    // Trivial
    while (!list_empty(projet->ef_donnees.relachements))
    {
        EF_Relachement *relachement = (EF_Relachement*)list_remove_front(projet->ef_donnees.relachements);
        free(relachement->rx_d_data);
        free(relachement->ry_d_data);
        free(relachement->rz_d_data);
        free(relachement->rx_f_data);
        free(relachement->ry_f_data);
        free(relachement->rz_f_data);
        
        free(relachement);
    }
    
    free(projet->ef_donnees.relachements);
    projet->ef_donnees.relachements = NULL;
    
    return 0;
}
