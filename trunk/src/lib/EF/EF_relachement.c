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
#include "common_projet.h"
#include "common_erreurs.h"
#ifdef ENABLE_GTK
#include <gtk/gtk.h>
#endif

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
#ifdef ENABLE_GTK
    GtkTreeIter     iter;
#endif
    
    BUGMSG(projet, -1, gettext("Paramètre incorrect\n"));
    
    // Trivial
    projet->ef_donnees.relachements = NULL;
    
#ifdef ENABLE_GTK
    projet->list_gtk.ef_barre.liste_relachements = gtk_list_store_new(1, G_TYPE_STRING);
    gtk_list_store_append(projet->list_gtk.ef_barre.liste_relachements, &iter);
    gtk_list_store_set(projet->list_gtk.ef_barre.liste_relachements, &iter, 0, gettext("Aucun"), -1);
#endif
    
    return 0;
}


int EF_relachement_ajout(Projet *projet, const char *nom, EF_Relachement_Type rx_debut,
  void* rx_d_data, EF_Relachement_Type ry_debut, void* ry_d_data, EF_Relachement_Type rz_debut,
  void* rz_d_data, EF_Relachement_Type rx_fin, void* rx_f_data, EF_Relachement_Type ry_fin,
  void* ry_f_data, EF_Relachement_Type rz_fin, void* rz_f_data)
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
 *            : const char *nom : nom du relâchement.
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet == NULL)
 *             (projet->ef_donnees.relachements == NULL)
 *             (rx_debut == EF_RELACHEMENT_LIBRE) && (rx_fin == EF_RELACHEMENT_LIBRE)
 */
{
    EF_Relachement  *relachement_nouveau = malloc(sizeof(EF_Relachement));
#ifdef ENABLE_GTK
    GtkTreeIter     iter;
#endif
    
    // Trivial
    
    BUGMSG(projet, -1, gettext("Paramètre incorrect\n"));
    BUG(!((rx_debut == EF_RELACHEMENT_LIBRE) && (rx_fin == EF_RELACHEMENT_LIBRE)), -1);
    
    relachement_nouveau->rx_debut = rx_debut;
    relachement_nouveau->rx_d_data = rx_d_data;
    relachement_nouveau->ry_debut = ry_debut;
    relachement_nouveau->ry_d_data = ry_d_data;
    relachement_nouveau->rz_debut = rz_debut;
    relachement_nouveau->rz_d_data = rz_d_data;
    relachement_nouveau->rx_fin = rx_fin;
    relachement_nouveau->rx_f_data = rx_f_data;
    relachement_nouveau->ry_fin = ry_fin;
    relachement_nouveau->ry_f_data = ry_f_data;
    relachement_nouveau->rz_fin = rz_fin;
    relachement_nouveau->rz_f_data = rz_f_data;
    BUGMSG(relachement_nouveau->nom = g_strdup_printf("%s", nom), -2, gettext("Erreur d'allocation mémoire.\n"));
    
    relachement_nouveau->numero = g_list_length(projet->ef_donnees.relachements);
    
    projet->ef_donnees.relachements = g_list_append(projet->ef_donnees.relachements, relachement_nouveau);
    
#ifdef ENABLE_GTK
    gtk_list_store_append(projet->list_gtk.ef_barre.liste_relachements, &iter);
    gtk_list_store_set(projet->list_gtk.ef_barre.liste_relachements, &iter, 0, nom, -1);
#endif
    
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
    GList   *list_parcours;
    
    BUGMSG(projet, NULL, gettext("Paramètre incorrect\n"));
    BUGMSG(projet->ef_donnees.relachements, NULL, gettext("Paramètre incorrect\n"));
    
    // Trivial
    list_parcours = projet->ef_donnees.relachements;
    do
    {
        EF_Relachement *relachement = list_parcours->data;
        
        if (relachement->numero == numero)
            return relachement;
        
        list_parcours = g_list_next(list_parcours);
    }
    while (list_parcours != NULL);
    
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
    BUGMSG(projet, -1, gettext("Paramètre incorrect\n"));
    BUGMSG(projet->ef_donnees.relachements, -1, gettext("Paramètre incorrect\n"));
    
    // Trivial
    while (projet->ef_donnees.relachements != NULL)
    {
        EF_Relachement *relachement = projet->ef_donnees.relachements->data;
        
        projet->ef_donnees.relachements = g_list_delete_link(projet->ef_donnees.relachements, projet->ef_donnees.relachements);
        free(relachement->rx_d_data);
        free(relachement->ry_d_data);
        free(relachement->rz_d_data);
        free(relachement->rx_f_data);
        free(relachement->ry_f_data);
        free(relachement->rz_f_data);
        free(relachement->nom);
        
        free(relachement);
    }
    
#ifdef ENABLE_GTK
    g_object_unref(projet->list_gtk.ef_barre.liste_relachements);
#endif
    
    return 0;
}
