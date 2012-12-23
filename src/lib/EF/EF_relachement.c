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
#include <string.h>
#include <gmodule.h>

#include "1992_1_1_barres.h"
#include "common_projet.h"
#include "common_erreurs.h"
#include "common_selection.h"
#include "EF_calculs.h"
#ifdef ENABLE_GTK
#include <gtk/gtk.h>
#endif

G_MODULE_EXPORT gboolean EF_relachement_init(Projet *projet)
/* Description : Initialise la liste des relachements.
 * Paramètres : Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL,
 *             en cas d'erreur d'allocation mémoire.
 */
{
#ifdef ENABLE_GTK
    GtkTreeIter     iter;
#endif
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    
    // Trivial
    projet->ef_donnees.relachements = NULL;
    
#ifdef ENABLE_GTK
    projet->list_gtk.ef_relachements.liste_relachements = gtk_list_store_new(1, G_TYPE_STRING);
    gtk_list_store_append(projet->list_gtk.ef_relachements.liste_relachements, &iter);
    gtk_list_store_set(projet->list_gtk.ef_relachements.liste_relachements, &iter, 0, gettext("Aucun"), -1);
#endif
    
    return TRUE;
}


G_MODULE_EXPORT EF_Relachement *EF_relachement_ajout(Projet *projet, const char *nom,
  EF_Relachement_Type rx_debut, void* rx_d_data, EF_Relachement_Type ry_debut, void* ry_d_data,
  EF_Relachement_Type rz_debut, void* rz_d_data, EF_Relachement_Type rx_fin, void* rx_f_data,
  EF_Relachement_Type ry_fin, void* ry_f_data, EF_Relachement_Type rz_fin, void* rz_f_data)
/* Description : Ajoute un relachement en lui attribuant le numéro suivant le dernier
 *               relachement existant. Les données fournis dans les paramètres additionnels
 *               doivent avoir été stockées en mémoire par l'utilisation de malloc.
 * Paramètres : Projet *projet : la variable projet,
 *            : const char *nom : nom du relâchement,
 *            : Type_EF_Appui rx_debut : relachement de la rotation autour de l'axe x au début,
 *            : void* rx_d_data : paramètre additionnel de la rotation si nécessaire,
 *            : Type_EF_Appui ry_debut : relachement de la rotation autour de l'axe y au début,
 *            : void* ry_d_data : paramètre additionnel de la rotation si nécessaire,
 *            : Type_EF_Appui rz_debut : relachement de la rotation autour de l'axe z au début,
 *            : void* rz_d_data : paramètre additionnel de la rotation si nécessaire,
 *            : Type_EF_Appui rx_fin : relachement de la rotation autour de l'axe x à la fin,
 *            : void* rx_f_data : paramètre additionnel de la rotation si nécessaire,
 *            : Type_EF_Appui ry_fin : relachement de la rotation autour de l'axe y à la fin,
 *            : void* ry_f_data : paramètre additionnel de la rotation si nécessaire,
 *            : Type_EF_Appui rz_fin : relachement de la rotation autour de l'axe z à la fin,
 *            : void* rz_f_data : paramètre additionnel de la rotation si nécessaire.
 * Valeur renvoyée :
 *   Succès : pointeur vers le nouveau relâchement.
 *   Échec : NULL :
 *             projet == NULL,
 *             rx_debut == EF_RELACHEMENT_LIBRE && rx_fin == EF_RELACHEMENT_LIBRE,
 *             en cas d'erreur d'allocation mémoire.
 */
{
    EF_Relachement  *relachement_nouveau = malloc(sizeof(EF_Relachement));
    
    // Trivial
    
    BUGMSG(projet, NULL, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(relachement_nouveau, NULL, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(!((rx_debut == EF_RELACHEMENT_LIBRE) && (rx_fin == EF_RELACHEMENT_LIBRE)), NULL, "Impossible de relâcher rx simultanément des deux cotés de la barre.\n");
    BUGMSG(strcmp(gettext("Aucun"), nom), NULL, "Impossible d'utiliser comme nom 'Aucun'.\n");
    
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
    BUGMSG(relachement_nouveau->nom = g_strdup_printf("%s", nom), NULL, gettext("Erreur d'allocation mémoire.\n"));
    
    projet->ef_donnees.relachements = g_list_append(projet->ef_donnees.relachements, relachement_nouveau);
    
#ifdef ENABLE_GTK
    gtk_list_store_append(projet->list_gtk.ef_relachements.liste_relachements, &relachement_nouveau->Iter_liste);
    gtk_list_store_set(projet->list_gtk.ef_relachements.liste_relachements, &relachement_nouveau->Iter_liste, 0, nom, -1);
#endif
    
    return relachement_nouveau;
}


G_MODULE_EXPORT EF_Relachement* EF_relachement_cherche_nom(Projet *projet, const char *nom,
  gboolean critique)
/* Description : Renvoie le relachement cherché.
 * Paramètres : Projet *projet : la variable projet,
 *            : const char *nom : le nom du relachement.
 *            : gboolean critique : si critique = TRUE, BUG est utilisé, return sinon.
 * Valeur renvoyée :
 *   Succès : pointeur vers le relachement recherché
 *   Échec : NULL :
 *             projet == NULL,
 *             relachement introuvable.
 */
{
    GList   *list_parcours;
    
    BUGMSG(projet, NULL, gettext("Paramètre %s incorrect.\n"), "projet");
    
    // Trivial
    list_parcours = projet->ef_donnees.relachements;
    while (list_parcours != NULL)
    {
        EF_Relachement *relachement = list_parcours->data;
        
        if (strcmp(relachement->nom, nom) == 0)
            return relachement;
        
        list_parcours = g_list_next(list_parcours);
    }
    
    if (critique)
        BUGMSG(0, NULL, gettext("Relachement '%s' introuvable.\n"), nom);
    else
        return NULL;
}


G_MODULE_EXPORT gboolean EF_relachement_renomme(EF_Relachement *relachement, gchar *nom,
  Projet *projet)
/* Description : Renomme un relachement.
 * Paramètres : EF_Relachement *relachement : relâchement à renommer,
 *            : const char *nom : le nouveau nom,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL,
 *             relâchement == NULL,
 *             relâchement possédant le nouveau nom est déjà existant.
 */
{
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(relachement, FALSE, gettext("Paramètre %s incorrect.\n"), "relachement");
    BUGMSG(EF_relachement_cherche_nom(projet, nom, FALSE) == NULL, FALSE, gettext("Le relachement '%s' existe déjà.\n"), nom);
    
    free(relachement->nom);
    BUGMSG(relachement->nom = g_strdup_printf("%s", nom), FALSE, gettext("Erreur d'allocation mémoire.\n"));
    
#ifdef ENABLE_GTK
    if (projet->list_gtk.ef_relachements.builder != NULL)
        gtk_tree_store_set(projet->list_gtk.ef_relachements.relachements, &relachement->Iter_fenetre, 0, nom, -1);
    gtk_list_store_set(projet->list_gtk.ef_relachements.liste_relachements, &relachement->Iter_liste, 0, nom, -1);
#endif
    
    return TRUE;
}


G_MODULE_EXPORT gboolean EF_relachement_cherche_dependances(Projet *projet,
  EF_Relachement *relachement, GList** barres_dep)
/* Description : Liste l'ensemble des barres utilisant le relâchement.
 * Paramètres : Projet *projet : la variable projet,
 *            : EF_Relachement *relachement : le relâchement à analyser,
 *            : GList** barres_dep : la liste des barres dépendantes.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL,
 *             relachement == NULL.
 */
{
    GList   *list_parcours;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(relachement, FALSE, gettext("Paramètre %s incorrect.\n"), "relachement");
    
    *barres_dep = NULL;
    
    list_parcours = projet->ef_donnees.relachements;
    while (list_parcours != NULL)
    {
        Beton_Barre *barre = list_parcours->data;
        
        if (barre->relachement == relachement)
            *barres_dep = g_list_append(*barres_dep, barre);
        
        list_parcours = g_list_next(list_parcours);
    }
    
    return TRUE;
}


G_MODULE_EXPORT gboolean EF_relachement_supprime(EF_Relachement *relachement,
  gboolean annule_si_utilise, Projet *projet)
/* Description : Supprime le relâchement spécifié.
 * Paramètres : EF_Relachement *relachement : le relâchement à supprimer,
 *            : gboolean annule_si_utilise : possibilité d'annuler la suppression si le
 *              relâchement est attribuée à une barre. Si l'option est désactivée, les barres
 *              (et les barres et noeuds intermédiaires dépendants) utilisant le relâchement
 *              seront supprimées.
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL,
 *             section == NULL.
 */
{
    GList   *list_barres;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(relachement, FALSE, gettext("Paramètre %s incorrect.\n"), "relachement");
    
    // On vérifie les dépendances.
    BUG(EF_relachement_cherche_dependances(projet, relachement, &list_barres), FALSE);
    
    if ((annule_si_utilise) && (list_barres != NULL))
    {
        char *liste;
        
        liste = common_selection_converti_barres_en_texte(list_barres);
        if (g_list_next(list_barres) == NULL)
            printf("Impossible de supprimer la section car elle est utilisée par la barre %s.\n", liste);
        else
            printf("Impossible de supprimer la section car elle est utilisée par les barres %s.\n", liste);
        g_list_free(list_barres);
        free(liste);
        
        return TRUE;
    }
    
    BUG(_1992_1_1_barres_supprime_liste(projet, NULL, list_barres), TRUE);
    g_list_free(list_barres);
    
    free(relachement->nom);
    free(relachement->rx_d_data);
    free(relachement->ry_d_data);
    free(relachement->rz_d_data);
    free(relachement->rx_f_data);
    free(relachement->ry_f_data);
    free(relachement->rz_f_data);
    projet->ef_donnees.relachements = g_list_remove(projet->ef_donnees.relachements, relachement);
    
#ifdef ENABLE_GTK
    gtk_list_store_remove(projet->list_gtk.ef_relachements.liste_relachements, &relachement->Iter_liste);
    if (projet->list_gtk.ef_relachements.builder != NULL)
        gtk_tree_store_remove(projet->list_gtk.ef_relachements.relachements, &relachement->Iter_fenetre);
#endif
    
    return TRUE;
}


G_MODULE_EXPORT gboolean EF_relachement_free(Projet *projet)
/* Description : Libère l'ensemble des relachements et la liste les contenant.
 * Paramètres : Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL.
 */
{
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    
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
    
    BUG(EF_calculs_free(projet), FALSE);
    
#ifdef ENABLE_GTK
    g_object_unref(projet->list_gtk.ef_relachements.liste_relachements);
#endif
    
    return TRUE;
}
