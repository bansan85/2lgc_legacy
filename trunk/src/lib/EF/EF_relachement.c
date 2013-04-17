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
#include "common_math.h"
#include "EF_calculs.h"
#ifdef ENABLE_GTK
#include <gtk/gtk.h>
#include "EF_gtk_relachement.h"
#endif

gboolean EF_relachement_init(Projet *projet)
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
    projet->modele.relachements = NULL;
    
#ifdef ENABLE_GTK
    projet->list_gtk.ef_relachements.liste_relachements = gtk_list_store_new(1, G_TYPE_STRING);
    gtk_list_store_append(projet->list_gtk.ef_relachements.liste_relachements, &iter);
    gtk_list_store_set(projet->list_gtk.ef_relachements.liste_relachements, &iter, 0, gettext("Aucun"), -1);
#endif
    
    return TRUE;
}


#ifdef ENABLE_GTK
gboolean EF_relachements_update_ligne_treeview(Projet *projet, EF_Relachement *relachement)
/* Description : Actualise la ligne du treeview affichant le relâchement.
 * Paramètres : Projet *projet : la variable projet,
 *            : EF_Relachement *relachement : le relâchement à actualiser.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL,
 *             relachement == NULL,
 *             Interface graphique non initialisée.
 */
{
    Gtk_EF_Relachements  *ef_gtk;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(relachement, FALSE, gettext("Paramètre %s incorrect.\n"), "relachement");
    BUGMSG(projet->list_gtk.ef_relachements.builder, FALSE, gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Relâchement");
    
    ef_gtk = &projet->list_gtk.ef_relachements;
    gtk_tree_store_set(ef_gtk->relachements, &relachement->Iter_fenetre, 0, relachement->nom, -1);
    switch (relachement->rx_debut)
    {
        case EF_RELACHEMENT_BLOQUE :
        {
            gtk_tree_store_set(ef_gtk->relachements, &relachement->Iter_fenetre, 1, gettext("Bloqué"), 2, "-", -1);
            break;
        }
        case EF_RELACHEMENT_LIBRE :
        {
            gtk_tree_store_set(ef_gtk->relachements, &relachement->Iter_fenetre, 1, gettext("Libre"), 2, "-", -1);
            break;
        }
        case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
        {
            EF_Relachement_Donnees_Elastique_Lineaire *data;
            char    tmp[30];
            
            data = (EF_Relachement_Donnees_Elastique_Lineaire *)relachement->rx_d_data;
            common_math_double_to_char(data->raideur, tmp, DECIMAL_NEWTON_PAR_METRE);
            gtk_tree_store_set(ef_gtk->relachements, &relachement->Iter_fenetre, 1, gettext("Linéaire"), 2, tmp, -1);
            break;
        }
        default :
        {
            BUGMSG(NULL, FALSE, "Le type de relâchement est inconnu.\n");
            break;
        }
    }
    switch (relachement->ry_debut)
    {
        case EF_RELACHEMENT_BLOQUE :
        {
            gtk_tree_store_set(ef_gtk->relachements, &relachement->Iter_fenetre, 3, gettext("Bloqué"), 4, "-", -1);
            break;
        }
        case EF_RELACHEMENT_LIBRE :
        {
            gtk_tree_store_set(ef_gtk->relachements, &relachement->Iter_fenetre, 3, gettext("Libre"), 4, "-", -1);
            break;
        }
        case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
        {
            EF_Relachement_Donnees_Elastique_Lineaire *data;
            char    tmp[30];
            
            data = (EF_Relachement_Donnees_Elastique_Lineaire *)relachement->ry_d_data;
            common_math_double_to_char(data->raideur, tmp, DECIMAL_NEWTON_PAR_METRE);
            gtk_tree_store_set(ef_gtk->relachements, &relachement->Iter_fenetre, 3, gettext("Linéaire"), 4, tmp, -1);
            break;
        }
        default :
        {
            BUGMSG(NULL, FALSE, "Le type de relâchement est inconnu.\n");
            break;
        }
    }
    switch (relachement->rz_debut)
    {
        case EF_RELACHEMENT_BLOQUE :
        {
            gtk_tree_store_set(ef_gtk->relachements, &relachement->Iter_fenetre, 5, gettext("Bloqué"), 6, "-", -1);
            break;
        }
        case EF_RELACHEMENT_LIBRE :
        {
            gtk_tree_store_set(ef_gtk->relachements, &relachement->Iter_fenetre, 5, gettext("Libre"), 6, "-", -1);
            break;
        }
        case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
        {
            EF_Relachement_Donnees_Elastique_Lineaire *data;
            char    tmp[30];
            
            data = (EF_Relachement_Donnees_Elastique_Lineaire *)relachement->rz_d_data;
            common_math_double_to_char(data->raideur, tmp, DECIMAL_NEWTON_PAR_METRE);
            gtk_tree_store_set(ef_gtk->relachements, &relachement->Iter_fenetre, 5, gettext("Linéaire"), 6, tmp, -1);
            break;
        }
        default :
        {
            BUGMSG(NULL, FALSE, "Le type de relâchement est inconnu.\n");
            break;
        }
    }
    switch (relachement->rx_fin)
    {
        case EF_RELACHEMENT_BLOQUE :
        {
            gtk_tree_store_set(ef_gtk->relachements, &relachement->Iter_fenetre, 7, gettext("Bloqué"), 8, "-", -1);
            break;
        }
        case EF_RELACHEMENT_LIBRE :
        {
            gtk_tree_store_set(ef_gtk->relachements, &relachement->Iter_fenetre, 7, gettext("Libre"), 8, "-", -1);
            break;
        }
        case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
        {
            EF_Relachement_Donnees_Elastique_Lineaire *data;
            char    tmp[30];
            
            data = (EF_Relachement_Donnees_Elastique_Lineaire *)relachement->rx_f_data;
            common_math_double_to_char(data->raideur, tmp, DECIMAL_NEWTON_PAR_METRE);
            gtk_tree_store_set(ef_gtk->relachements, &relachement->Iter_fenetre, 7, gettext("Linéaire"), 8, tmp, -1);
            break;
        }
        default :
        {
            BUGMSG(NULL, FALSE, "Le type de relâchement est inconnu.\n");
            break;
        }
    }
    switch (relachement->ry_fin)
    {
        case EF_RELACHEMENT_BLOQUE :
        {
            gtk_tree_store_set(ef_gtk->relachements, &relachement->Iter_fenetre, 9, gettext("Bloqué"), 10, "-", -1);
            break;
        }
        case EF_RELACHEMENT_LIBRE :
        {
            gtk_tree_store_set(ef_gtk->relachements, &relachement->Iter_fenetre, 9, gettext("Libre"), 10, "-", -1);
            break;
        }
        case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
        {
            EF_Relachement_Donnees_Elastique_Lineaire *data;
            char    tmp[30];
            
            data = (EF_Relachement_Donnees_Elastique_Lineaire *)relachement->ry_f_data;
            common_math_double_to_char(data->raideur, tmp, DECIMAL_NEWTON_PAR_METRE);
            gtk_tree_store_set(ef_gtk->relachements, &relachement->Iter_fenetre, 9, gettext("Linéaire"), 10, tmp, -1);
            break;
        }
        default :
        {
            BUGMSG(NULL, FALSE, "Le type de relâchement est inconnu.\n");
            break;
        }
    }
    switch (relachement->rz_fin)
    {
        case EF_RELACHEMENT_BLOQUE :
        {
            gtk_tree_store_set(ef_gtk->relachements, &relachement->Iter_fenetre, 11, gettext("Bloqué"), 12, "-", -1);
            break;
        }
        case EF_RELACHEMENT_LIBRE :
        {
            gtk_tree_store_set(ef_gtk->relachements, &relachement->Iter_fenetre, 11, gettext("Libre"), 12, "-", -1);
            break;
        }
        case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
        {
            EF_Relachement_Donnees_Elastique_Lineaire *data;
            char    tmp[30];
            
            data = (EF_Relachement_Donnees_Elastique_Lineaire *)relachement->rz_f_data;
            common_math_double_to_char(data->raideur, tmp, DECIMAL_NEWTON_PAR_METRE);
            gtk_tree_store_set(ef_gtk->relachements, &relachement->Iter_fenetre, 11, gettext("Linéaire"), 12, tmp, -1);
            break;
        }
        default :
        {
            BUGMSG(NULL, FALSE, "Le type de relâchement est inconnu.\n");
            break;
        }
    }
    
    return TRUE;
}
#endif


EF_Relachement *EF_relachement_ajout(Projet *projet, const char *nom,
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
    EF_Relachement  *relachement_tmp;
    GList           *list_parcours;
    
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
    
    list_parcours = projet->modele.relachements;
    while (list_parcours != NULL)
    {
        relachement_tmp = list_parcours->data;
        
        if (strcmp(relachement_nouveau->nom, relachement_tmp->nom) < 0)
            break;
        
        list_parcours = g_list_next(list_parcours);
    }
    if (list_parcours == NULL)
    {
        projet->modele.relachements = g_list_append(projet->modele.relachements, relachement_nouveau);
#ifdef ENABLE_GTK
        gtk_list_store_append(projet->list_gtk.ef_relachements.liste_relachements, &relachement_nouveau->Iter_liste);
        if (projet->list_gtk.ef_relachements.builder != NULL)
            gtk_tree_store_append(projet->list_gtk.ef_relachements.relachements, &relachement_nouveau->Iter_fenetre, NULL);
#endif
    }
    else
    {
        projet->modele.relachements = g_list_insert_before(projet->modele.relachements, list_parcours, relachement_nouveau);
#ifdef ENABLE_GTK
        gtk_list_store_insert_before(projet->list_gtk.ef_relachements.liste_relachements, &relachement_nouveau->Iter_liste, &relachement_tmp->Iter_liste);
        if (projet->list_gtk.ef_relachements.builder != NULL)
            gtk_tree_store_insert_before(projet->list_gtk.ef_relachements.relachements, &relachement_nouveau->Iter_fenetre, NULL, &relachement_tmp->Iter_fenetre);
#endif
    }
    
#ifdef ENABLE_GTK
    gtk_list_store_set(projet->list_gtk.ef_relachements.liste_relachements, &relachement_nouveau->Iter_liste, 0, nom, -1);
    if (projet->list_gtk.ef_relachements.builder != NULL)
        BUG(EF_relachements_update_ligne_treeview(projet, relachement_nouveau), NULL);
#endif
    
    return relachement_nouveau;
}


EF_Relachement* EF_relachement_cherche_nom(Projet *projet, const char *nom, gboolean critique)
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
    list_parcours = projet->modele.relachements;
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


gboolean EF_relachement_modif(Projet *projet, EF_Relachement *relachement, const char *nom,
  EF_Relachement_Type rx_debut, void* rx_d_data, EF_Relachement_Type ry_debut, void* ry_d_data,
  EF_Relachement_Type rz_debut, void* rz_d_data, EF_Relachement_Type rx_fin, void* rx_f_data,
  EF_Relachement_Type ry_fin, void* ry_f_data, EF_Relachement_Type rz_fin, void* rz_f_data)
/* Description : Modifie un relâchement.
 * Paramètres : Projet *projet : la variable projet,
 *            : EF_Relachement *relachement : le relâchement à modifier,
 *            : Autres : caractéristiques du relachement. Pour ne pas modifier un paramètre,
 *            :   il suffit de lui de mettre NULL pour le nom, UINT_MAX pour les
 *                EF_Relachement_Type et NULL pour les arguments. Attention, aucune vérification
 *                n'est effectuée entre la compatibilité de EF_Relachement_Type et son
 *                paramètre.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL,
 *             materiau == NULL.
 */
{
    GList   *liste_relachement = NULL, *liste_barres_dep;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(relachement, FALSE, gettext("Paramètre %s incorrect.\n"), "relachement");
    
    liste_relachement = g_list_append(liste_relachement, relachement);
    BUG(_1992_1_1_barres_cherche_dependances(projet, NULL, NULL, NULL, NULL, liste_relachement, NULL, NULL, &liste_barres_dep, NULL, FALSE, FALSE), FALSE);
    g_list_free(liste_relachement);
    if (liste_barres_dep != NULL)
        BUG(EF_calculs_free(projet), FALSE);
    g_list_free(liste_barres_dep);
    
    if ((nom != NULL) && (strcmp(relachement->nom, nom) != 0))
    {
        GList   *list_parcours;
        
        BUGMSG(!EF_relachement_cherche_nom(projet, nom, FALSE), FALSE, gettext("Le relâchement %s existe déjà.\n"), nom);
        free(relachement->nom);
        BUGMSG(relachement->nom = g_strdup_printf("%s", nom), FALSE, gettext("Erreur d'allocation mémoire.\n"));
        gtk_list_store_set(projet->list_gtk.ef_relachements.liste_relachements, &relachement->Iter_liste, 0, relachement->nom, -1);
        
        // On réinsère le relâchement au bon endroit
        projet->modele.relachements = g_list_remove(projet->modele.relachements, relachement);
        list_parcours = projet->modele.relachements;
        while (list_parcours != NULL)
        {
            EF_Relachement  *relachement_parcours = list_parcours->data;
            
            if (strcmp(relachement->nom, relachement_parcours->nom) < 0)
            {
                projet->modele.relachements = g_list_insert_before(projet->modele.relachements, list_parcours, relachement);
                
#ifdef ENABLE_GTK
                gtk_list_store_move_before(projet->list_gtk.ef_relachements.liste_relachements, &relachement->Iter_liste, &relachement_parcours->Iter_liste);
                if (projet->list_gtk.ef_relachements.builder != NULL)
                    gtk_tree_store_move_before(projet->list_gtk.ef_relachements.relachements, &relachement->Iter_fenetre, &relachement_parcours->Iter_fenetre);
#endif
                break;
            }
            
            list_parcours = g_list_next(list_parcours);
        }
        if (list_parcours == NULL)
        {
            projet->modele.relachements = g_list_append(projet->modele.relachements, relachement);
            
#ifdef ENABLE_GTK
            gtk_list_store_move_before(projet->list_gtk.ef_relachements.liste_relachements, &relachement->Iter_liste, NULL);
            if (projet->list_gtk.ef_relachements.builder != NULL)
                gtk_tree_store_move_before(projet->list_gtk.ef_relachements.relachements, &relachement->Iter_fenetre, NULL);
#endif
        }
    
#ifdef ENABLE_GTK
        if (projet->list_gtk.ef_barres.builder != NULL)
        {
            GList   *list_parcours2 = projet->modele.barres;
            
            while (list_parcours2 != NULL)
            {
                Beton_Barre *barre = list_parcours2->data;
                
                if (barre->relachement == relachement)
                    gtk_tree_store_set(GTK_TREE_STORE(gtk_builder_get_object(projet->list_gtk.ef_barres.builder, "EF_barres_treestore")), &barre->Iter, 6, barre->relachement->nom, -1);
                list_parcours2 = g_list_next(list_parcours2);
            }
        }
#endif
    }
    
    if ((rx_debut != UINT_MAX) && (relachement->rx_debut != rx_debut))
    {
        switch (rx_debut)
        {
            free(relachement->rx_d_data);
            case EF_RELACHEMENT_BLOQUE :
            case EF_RELACHEMENT_LIBRE :
            {
                relachement->rx_d_data = NULL;
                break;
            }
            case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
            {
                BUGMSG(relachement->rx_d_data = malloc(sizeof(EF_Relachement_Donnees_Elastique_Lineaire)), FALSE, gettext("Erreur d'allocation mémoire.\n"));
                memset(relachement->rx_d_data, 0, sizeof(EF_Relachement_Donnees_Elastique_Lineaire));
                break;
            }
            default :
            {
                BUGMSG(NULL, FALSE, "Le type de relâchement est inconnu.\n");
                break;
            }
        }
        relachement->rx_debut = rx_debut;
    }
    if (rx_d_data != NULL)
    {
        switch (relachement->rx_debut)
        {
            case EF_RELACHEMENT_BLOQUE :
            case EF_RELACHEMENT_LIBRE :
            {
                BUGMSG(NULL, FALSE, "Impossible d'éditer ce type de relâchement.\n");
                break;
            }
            case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
            {
                memcpy(relachement->rx_d_data, rx_d_data, sizeof(EF_Relachement_Donnees_Elastique_Lineaire));
                break;
            }
            default :
            {
                BUGMSG(NULL, FALSE, "Le type de relâchement est inconnu.\n");
                break;
            }
        }
    }
    if ((ry_debut != UINT_MAX) && (relachement->ry_debut != ry_debut))
    {
        switch (ry_debut)
        {
            free(relachement->ry_d_data);
            case EF_RELACHEMENT_BLOQUE :
            case EF_RELACHEMENT_LIBRE :
            {
                relachement->ry_d_data = NULL;
                break;
            }
            case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
            {
                BUGMSG(relachement->ry_d_data = malloc(sizeof(EF_Relachement_Donnees_Elastique_Lineaire)), FALSE, gettext("Erreur d'allocation mémoire.\n"));
                memset(relachement->ry_d_data, 0, sizeof(EF_Relachement_Donnees_Elastique_Lineaire));
                break;
            }
            default :
            {
                BUGMSG(NULL, FALSE, "Le type de relâchement est inconnu.\n");
                break;
            }
        }
        relachement->ry_debut = ry_debut;
    }
    if (ry_d_data != NULL)
    {
        switch (relachement->ry_debut)
        {
            case EF_RELACHEMENT_BLOQUE :
            case EF_RELACHEMENT_LIBRE :
            {
                BUGMSG(NULL, FALSE, "Impossible d'éditer ce type de relâchement.\n");
                break;
            }
            case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
            {
                memcpy(relachement->ry_d_data, ry_d_data, sizeof(EF_Relachement_Donnees_Elastique_Lineaire));
                break;
            }
            default :
            {
                BUGMSG(NULL, FALSE, "Le type de relâchement est inconnu.\n");
                break;
            }
        }
    }
    if ((rz_debut != UINT_MAX) && (relachement->rz_debut != rz_debut))
    {
        switch (rz_debut)
        {
            free(relachement->rz_d_data);
            case EF_RELACHEMENT_BLOQUE :
            case EF_RELACHEMENT_LIBRE :
            {
                relachement->rz_d_data = NULL;
                break;
            }
            case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
            {
                BUGMSG(relachement->rz_d_data = malloc(sizeof(EF_Relachement_Donnees_Elastique_Lineaire)), FALSE, gettext("Erreur d'allocation mémoire.\n"));
                memset(relachement->rz_d_data, 0, sizeof(EF_Relachement_Donnees_Elastique_Lineaire));
                break;
            }
            default :
            {
                BUGMSG(NULL, FALSE, "Le type de relâchement est inconnu.\n");
                break;
            }
        }
        relachement->rz_debut = rz_debut;
    }
    if (rz_d_data != NULL)
    {
        switch (relachement->rz_debut)
        {
            case EF_RELACHEMENT_BLOQUE :
            case EF_RELACHEMENT_LIBRE :
            {
                BUGMSG(NULL, FALSE, "Impossible d'éditer ce type de relâchement.\n");
                break;
            }
            case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
            {
                memcpy(relachement->rz_d_data, rz_d_data, sizeof(EF_Relachement_Donnees_Elastique_Lineaire));
                break;
            }
            default :
            {
                BUGMSG(NULL, FALSE, "Le type de relâchement est inconnu.\n");
                break;
            }
        }
    }
    if ((rx_fin != UINT_MAX) && (relachement->rx_fin != rx_fin))
    {
        switch (rx_fin)
        {
            free(relachement->rx_f_data);
            case EF_RELACHEMENT_BLOQUE :
            case EF_RELACHEMENT_LIBRE :
            {
                relachement->rx_f_data = NULL;
                break;
            }
            case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
            {
                BUGMSG(relachement->rx_f_data = malloc(sizeof(EF_Relachement_Donnees_Elastique_Lineaire)), FALSE, gettext("Erreur d'allocation mémoire.\n"));
                memset(relachement->rx_f_data, 0, sizeof(EF_Relachement_Donnees_Elastique_Lineaire));
                break;
            }
            default :
            {
                BUGMSG(NULL, FALSE, "Le type de relâchement est inconnu.\n");
                break;
            }
        }
        relachement->rx_fin = rx_fin;
    }
    if (rx_f_data != NULL)
    {
        switch (relachement->rx_fin)
        {
            case EF_RELACHEMENT_BLOQUE :
            case EF_RELACHEMENT_LIBRE :
            {
                BUGMSG(NULL, FALSE, "Impossible d'éditer ce type de relâchement.\n");
                break;
            }
            case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
            {
                memcpy(relachement->rx_f_data, rx_f_data, sizeof(EF_Relachement_Donnees_Elastique_Lineaire));
                break;
            }
            default :
            {
                BUGMSG(NULL, FALSE, "Le type de relâchement est inconnu.\n");
                break;
            }
        }
    }
    if ((ry_fin != UINT_MAX) && (relachement->ry_fin != ry_fin))
    {
        switch (ry_fin)
        {
            free(relachement->ry_f_data);
            case EF_RELACHEMENT_BLOQUE :
            case EF_RELACHEMENT_LIBRE :
            {
                relachement->ry_f_data = NULL;
                break;
            }
            case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
            {
                BUGMSG(relachement->ry_f_data = malloc(sizeof(EF_Relachement_Donnees_Elastique_Lineaire)), FALSE, gettext("Erreur d'allocation mémoire.\n"));
                memset(relachement->ry_f_data, 0, sizeof(EF_Relachement_Donnees_Elastique_Lineaire));
                break;
            }
            default :
            {
                BUGMSG(NULL, FALSE, "Le type de relâchement est inconnu.\n");
                break;
            }
        }
        relachement->ry_fin = ry_fin;
    }
    if (ry_f_data != NULL)
    {
        switch (relachement->ry_fin)
        {
            case EF_RELACHEMENT_BLOQUE :
            case EF_RELACHEMENT_LIBRE :
            {
                BUGMSG(NULL, FALSE, "Impossible d'éditer ce type de relâchement.\n");
                break;
            }
            case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
            {
                memcpy(relachement->ry_f_data, ry_f_data, sizeof(EF_Relachement_Donnees_Elastique_Lineaire));
                break;
            }
            default :
            {
                BUGMSG(NULL, FALSE, "Le type de relâchement est inconnu.\n");
                break;
            }
        }
    }
    if ((rz_fin != UINT_MAX) && (relachement->rz_fin != rz_fin))
    {
        switch (rz_fin)
        {
            free(relachement->rz_f_data);
            case EF_RELACHEMENT_BLOQUE :
            case EF_RELACHEMENT_LIBRE :
            {
                relachement->rz_f_data = NULL;
                break;
            }
            case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
            {
                BUGMSG(relachement->rz_f_data = malloc(sizeof(EF_Relachement_Donnees_Elastique_Lineaire)), FALSE, gettext("Erreur d'allocation mémoire.\n"));
                memset(relachement->rz_f_data, 0, sizeof(EF_Relachement_Donnees_Elastique_Lineaire));
                break;
            }
            default :
            {
                BUGMSG(NULL, FALSE, "Le type de relâchement est inconnu.\n");
                break;
            }
        }
        relachement->rz_fin = rz_fin;
    }
    if (rz_f_data != NULL)
    {
        switch (relachement->rz_fin)
        {
            case EF_RELACHEMENT_BLOQUE :
            case EF_RELACHEMENT_LIBRE :
            {
                BUGMSG(NULL, FALSE, "Impossible d'éditer ce type de relâchement.\n");
                break;
            }
            case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
            {
                memcpy(relachement->rz_f_data, rz_f_data, sizeof(EF_Relachement_Donnees_Elastique_Lineaire));
                break;
            }
            default :
            {
                BUGMSG(NULL, FALSE, "Le type de relâchement est inconnu.\n");
                break;
            }
        }
    }
            
#ifdef ENABLE_GTK
    if (projet->list_gtk.ef_relachements.builder != NULL)
    {
        BUG(EF_relachements_update_ligne_treeview(projet, relachement), FALSE);
        EF_gtk_relachements_select_changed(NULL, projet);
    }
#endif
    
    return TRUE;
}


gboolean EF_relachement_supprime(EF_Relachement *relachement, gboolean annule_si_utilise,
  Projet *projet)
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
    GList   *liste_relachements = NULL, *liste_barres_dep;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(relachement, FALSE, gettext("Paramètre %s incorrect.\n"), "relachement");
    
    // On vérifie les dépendances.
    liste_relachements = g_list_append(liste_relachements, relachement);
    BUG(_1992_1_1_barres_cherche_dependances(projet, NULL, NULL, NULL, NULL, liste_relachements, NULL, NULL, &liste_barres_dep, NULL, FALSE, FALSE), FALSE);
    
    if ((annule_si_utilise) && (liste_barres_dep != NULL))
    {
        char *liste;
        
        liste = common_selection_converti_barres_en_texte(liste_relachements);
        if (g_list_next(liste_relachements) == NULL)
            BUGMSG(NULL, FALSE, gettext("Impossible de supprimer la section car elle est utilisée par la barre %s.\n"), liste);
        else
            BUGMSG(NULL, FALSE, gettext("Impossible de supprimer la section car elle est utilisée par les barres %s.\n"), liste);
    }
    
    g_list_free(liste_relachements);
    BUG(_1992_1_1_barres_supprime_liste(projet, NULL, liste_barres_dep), TRUE);
    g_list_free(liste_barres_dep);
    
    free(relachement->nom);
    free(relachement->rx_d_data);
    free(relachement->ry_d_data);
    free(relachement->rz_d_data);
    free(relachement->rx_f_data);
    free(relachement->ry_f_data);
    free(relachement->rz_f_data);
    projet->modele.relachements = g_list_remove(projet->modele.relachements, relachement);
    
#ifdef ENABLE_GTK
    gtk_list_store_remove(projet->list_gtk.ef_relachements.liste_relachements, &relachement->Iter_liste);
    if (projet->list_gtk.ef_relachements.builder != NULL)
        gtk_tree_store_remove(projet->list_gtk.ef_relachements.relachements, &relachement->Iter_fenetre);
#endif
    
    free(relachement);
    
    return TRUE;
}


gboolean EF_relachement_free(Projet *projet)
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
    while (projet->modele.relachements != NULL)
    {
        EF_Relachement *relachement = projet->modele.relachements->data;
        
        projet->modele.relachements = g_list_delete_link(projet->modele.relachements, projet->modele.relachements);
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
