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
#include <gmodule.h>
#include <math.h>

#include "common_projet.h"
#include "common_erreurs.h"
#include "1990_action.h"

#ifdef ENABLE_GTK
#include "1990_gtk_groupes.h"
#include "common_gtk.h"
#endif

gboolean _1990_groupe_init(Projet *projet)
/* Description : Initialise la liste des groupes.
 * Paramètres : Projet *projet : variable projet.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL.
 */
{
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    
    // Trivial
    projet->niveaux_groupes = NULL;
    
    return TRUE;
}


gboolean _1990_groupe_ajout_niveau(Projet *projet)
/* Description : Ajoute un niveau au projet en lui attribuant le numéro suivant le dernier
 *                 niveau existant.
 * Paramètres : Projet *projet : variable projet.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL,
 *             erreur d'allocation mémoire.
 */
{
    Niveau_Groupe   *niveau_nouveau = malloc(sizeof(Niveau_Groupe));
    
    // Trivial
    BUGMSG(niveau_nouveau, FALSE, gettext("Erreur d'allocation mémoire.\n"));
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    
    niveau_nouveau->numero = g_list_length(projet->niveaux_groupes);
    niveau_nouveau->groupes = NULL;
    
    projet->niveaux_groupes = g_list_append(projet->niveaux_groupes, niveau_nouveau);
    
    /* Mise à jour des limites du composant spin_button */
#ifdef ENABLE_GTK
    if (projet->list_gtk._1990_groupes.builder != NULL)
    {
        gtk_tree_store_clear(projet->list_gtk._1990_groupes.tree_store_etat);
        gtk_spin_button_set_range(GTK_SPIN_BUTTON(projet->list_gtk._1990_groupes.spin_button_niveau), 0, g_list_length(projet->niveaux_groupes)-1);
        
        /* La modification de la valeur du composant spin_button execute automatiquement
         * _1990_gtk_spin_button_niveau_change qui met à jour l'interface graphique */
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(projet->list_gtk._1990_groupes.spin_button_niveau), g_list_length(projet->niveaux_groupes)-1);
    }
#endif
    
    return TRUE;
}


Element *_1990_groupe_positionne_element(Groupe *groupe, unsigned int numero)
/* Description : Renvoie l'élément recherché en fonction de son numéro.
 * Paramètres : Groupe *groupe : groupe à analyser,
 *            : unsigned int numero : numéro de l'élément à trouver.
 * Valeur renvoyée :
 *   Succès : Pointeur vers l'élément recherché.
 *   Échec : NULL :
 *             groupe == NULL,
 *             groupe->elements == NULL,
 *             élément introuvable.
 */
{
    GList   *list_parcours;
    
    BUGMSG(groupe, NULL, gettext("Paramètre %s incorrect.\n"), "groupe");
    
    // Trivial
    list_parcours = groupe->elements;
    while (list_parcours != NULL)
    {
        Element     *element_en_cours = list_parcours->data;
        
        if (element_en_cours->numero == numero)
            return element_en_cours;
        
        list_parcours = g_list_next(list_parcours);
    }
    
    BUGMSG(0, NULL, gettext("Élément %u du groupe %u introuvable.\n"), numero, groupe->numero);
}


Groupe *_1990_groupe_positionne_groupe(Niveau_Groupe *niveau, unsigned int numero)
/* Description : Renvoie le groupe d'un niveau en fonction de son numéro.
 * Paramètres : Niveau_Groupe *niveau : niveau à analyser,
 *            : unsigned int numero : numéro du groupe à trouver.
 * Valeur renvoyée :
 *   Succès : pointeur vers le groupe.
 *   Échec : NULL :
 *             niveau == NULL,
 *             groupe introuvable.
 */
{
    GList   *list_parcours;
    
    BUGMSG(niveau, NULL, gettext("Paramètre %s incorrect.\n"), "niveau");
    
    // Trivial
    list_parcours = niveau->groupes;
    while (list_parcours != NULL)
    {
        Groupe *groupe = list_parcours->data;
        if (groupe->numero == numero)
            return groupe;
         
        list_parcours = g_list_next(list_parcours);
    }
    
    BUGMSG(0, NULL, gettext("Groupe %u du niveau %u introuvable.\n"), numero, niveau->numero);
}


Niveau_Groupe *_1990_groupe_positionne_niveau(Projet *projet, unsigned int numero)
/* Description : Renvoie le niveau en fonction de son numéro.
 * Paramètres : Projet *projet : la variable projet,
 *            : unsigned int numero : numéro du groupe à trouver.
 * Valeur renvoyée :
 *   Succès : Pointeur vers le niveau recherché.
 *   Échec : NULL :
 *             projet == NULL,
 *             niveau introuvable.
 */
{
    GList           *list_parcours;
    Niveau_Groupe   *niveau;
    
    BUGMSG(projet, NULL, gettext("Paramètre %s incorrect.\n"), "projet");
    
    // Trivial
    list_parcours = projet->niveaux_groupes;
    while (list_parcours != NULL)
    {
        niveau = list_parcours->data;
        if (niveau->numero == numero)
            return niveau;
        
        list_parcours = g_list_next(list_parcours);
    }
    
    BUGMSG(0, NULL, gettext("Niveau de groupes %u introuvable.\n"), numero);
}


Groupe *_1990_groupe_ajout_groupe(Projet *projet, unsigned int niveau,
  Type_Groupe_Combinaison type_combinaison, const char* nom)
/* Description : Ajoute un groupe au niveau choisi avec le type de combinaison spécifié.
 * Paramètres : Projet *projet : la variable projet,
 *            : unsigned int niveau : le niveau où le groupe doit être inséré,
 *            : Type_Groupe_Combinaison type_combinaison : combinaison du nouveau groupe.
 *            : const char* nom : nom du groupe.
 * Valeur renvoyée :
 *   Succès : Pointeur vers le nouveau groupe.
 *   Échec : NULL :
 *             projet == NULL,
 *             erreur d'allocation mémoire,
 *             _1990_groupe_positionne_niveau.
 */
{
    Groupe          *groupe_nouveau = malloc(sizeof(Groupe));
    Niveau_Groupe   *niveau_groupe;
    
    BUGMSG(projet, NULL, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(groupe_nouveau, NULL, gettext("Erreur d'allocation mémoire.\n"));
    
    // Trivial
    
    BUG(niveau_groupe = _1990_groupe_positionne_niveau(projet, niveau), NULL);
    
    groupe_nouveau->numero = g_list_length(niveau_groupe->groupes);
    BUGMSG(groupe_nouveau->nom = g_strdup_printf("%s", nom), NULL, gettext("Erreur d'allocation mémoire.\n"));
    groupe_nouveau->type_combinaison = type_combinaison;
#ifdef ENABLE_GTK
    groupe_nouveau->Iter_expand = 1;
#endif
    
    groupe_nouveau->tmp_combinaison = NULL;
    
    groupe_nouveau->elements = NULL;
    niveau_groupe->groupes = g_list_append(niveau_groupe->groupes, groupe_nouveau);
    
#ifdef ENABLE_GTK
    if ((projet->list_gtk._1990_groupes.builder != NULL) && (GTK_COMMON_SPINBUTTON_AS_UINT(GTK_SPIN_BUTTON(projet->list_gtk._1990_groupes.spin_button_niveau)) == niveau))
    {
        GtkTreePath     *path;
        
        /* Et on met à jour l'affichage */
        gtk_tree_store_append(projet->list_gtk._1990_groupes.tree_store_etat, &groupe_nouveau->Iter, NULL);
        gtk_tree_store_set(projet->list_gtk._1990_groupes.tree_store_etat, &groupe_nouveau->Iter, 0, groupe_nouveau->numero, 1, "", -1);
        path = gtk_tree_model_get_path(GTK_TREE_MODEL(projet->list_gtk._1990_groupes.tree_store_etat), &groupe_nouveau->Iter);
        gtk_tree_view_set_cursor(GTK_TREE_VIEW(projet->list_gtk._1990_groupes.tree_view_etat), path, gtk_tree_view_get_column(GTK_TREE_VIEW(projet->list_gtk._1990_groupes.tree_view_etat), 1), TRUE);
        gtk_tree_path_free(path);

    }
#endif
    
    return groupe_nouveau;
}


gboolean _1990_groupe_ajout_element(Projet *projet, unsigned int niveau, unsigned int groupe_n,
  unsigned int num_element)
/* Description : Ajoute l'élément 'num_element' au groupe 'groupe_n' du niveau 'niveau'.
 *               L'élément appartient au niveau directement inférieur à 'niveau' et possède le
 *               numéro 'num_element'.
 *               Ainsi si 'niveau' est égal à 0, 'num_element' fait référence à une action de
 *               projet->actions.
 *               Si 'niveau' est supérieur à 0, 'num_element' fait référence à une groupe du
 *               niveau 'niveau'-1.
 *               Le dernier niveau ne doit contenir qu'un seul groupe.
 * Paramètres : Projet *projet : la variable projet,
 *            : unsigned int niveau : le niveau où le groupe doit être inséré,
 *            : unsigned int groupe_n : numéro du groupe où ajouter l'élément,
 *            : unsigned int num_element : numéro de l'élément à ajouter.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL,
 *             projet->niveaux_groupes == NULL,
 *             si l'élément num_element, est déjà présentant dans le groupe groupe_n.
 *             erreur d'allocation mémoire,
 *             _1990_action_cherche_numero,
 *             _1990_groupe_positionne_niveau,
 *             _1990_groupe_positionne_groupe.
 */
{
    Niveau_Groupe   *niveau_groupe;
    Groupe          *groupe;
    Element         *element_nouveau = malloc(sizeof(Element));
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->niveaux_groupes, FALSE, gettext("Le projet ne possède pas de niveaux de groupes permettant de regrouper plusieurs groupes d'actions.\n"));
    BUGMSG(element_nouveau, FALSE, gettext("Erreur d'allocation mémoire.\n"));
    
    // Trivial
    /* On commence par positionner le numéro num_element de l'étage n-1
     * puis on positionne le niveau en cours au niveau 'niveau' */
    if (niveau == 0)
    {
        // On vérifie si l'action num_element existe.
        BUG(_1990_action_cherche_numero(projet, num_element), FALSE);
        niveau_groupe = projet->niveaux_groupes->data;
    }
    else
    {
        // On vérifie si le groupe du niveau n-1 existe;
        BUG(niveau_groupe = _1990_groupe_positionne_niveau(projet, niveau-1), FALSE);
        BUG(_1990_groupe_positionne_groupe(niveau_groupe, num_element), FALSE);
        // Et on positionne le niveau en cours
        BUG(niveau_groupe = _1990_groupe_positionne_niveau(projet, niveau), FALSE);
    }
    
    BUG(groupe = _1990_groupe_positionne_groupe(niveau_groupe, groupe_n), FALSE);
    element_nouveau->numero = num_element;
    #ifdef ENABLE_GTK
    element_nouveau->Iter_expand = 1;
    #endif
    
    /* On ajoute le nouvel élément au groupe */
    if (groupe->elements == NULL)
        groupe->elements = g_list_append(groupe->elements, element_nouveau);
    else
    {
        int     element_ajoute = 0;
        GList   *list_parcours;
        Element         *element;
        
        list_parcours = groupe->elements;
        element_ajoute = 0;
        /* On l'ajoute en triant pour faire plus joli */
        do
        {
            element = list_parcours->data;
            if (element->numero == num_element)
            {
                free(element_nouveau);
                printf(gettext("Le numéro %u est déjà présent dans le groupe %u du niveau %u.\n"), num_element, groupe_n, niveau);
                return FALSE;
            }
            if (element->numero > num_element)
            {
                groupe->elements = g_list_insert_before(groupe->elements, list_parcours, element_nouveau);
                element_ajoute = 1;
            }
            
            list_parcours = g_list_next(list_parcours);
        }
        while ((list_parcours != NULL) && (element_ajoute == 0));
        /* Si pas encore ajouté, on l'ajoute à la fin de la liste */
        if (element_ajoute == 0)
            groupe->elements = g_list_append(groupe->elements, element_nouveau);
    }
    
    #ifdef ENABLE_GTK
    if ((projet->list_gtk._1990_groupes.builder != NULL) && (GTK_COMMON_SPINBUTTON_AS_UINT(GTK_SPIN_BUTTON(projet->list_gtk._1990_groupes.spin_button_niveau)) == niveau))
    {
        GtkTreeIter         iter;
        unsigned int        numero;
        Gtk_1990_Groupes    *gtk_1990_groupes = &projet->list_gtk._1990_groupes;
        GtkTreePath         *path; // Pour développer une ligne du TreeView
        Element        	    *element, *element2;
        
        // On supprime l'élément à ajouter dans le groupe de la liste des éléments disponibles.
        if (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(gtk_1990_groupes->tree_store_dispo), &iter) == TRUE)
        {
            do
            {
                gtk_tree_model_get(GTK_TREE_MODEL(gtk_1990_groupes->tree_store_dispo), &iter, 0, &numero, -1);
                if (numero == num_element)
                {
                    gtk_tree_store_remove(gtk_1990_groupes->tree_store_dispo, &iter);
                    break;
                }
            }
            while (gtk_tree_model_iter_next(GTK_TREE_MODEL(gtk_1990_groupes->tree_store_dispo), &iter) != FALSE);
        }
        
        // On positionne l'élément en cours
        BUG(element = _1990_groupe_positionne_element(groupe, num_element), FALSE);
        
        // On ajoute la ligne dans l'interface graphique
        if (groupe->elements->data == element)
            gtk_tree_store_prepend(gtk_1990_groupes->tree_store_etat, &element->Iter, &groupe->Iter);
        else
        {
            element2 = g_list_previous(g_list_find(groupe->elements, element))->data;
            gtk_tree_store_insert_after(gtk_1990_groupes->tree_store_etat, &element->Iter, &groupe->Iter, &element2->Iter);
        }
        
        if (projet->niveaux_groupes->data == niveau_groupe)
        {
            Action *action;
            
            BUG(action = _1990_action_cherche_numero(projet, num_element), FALSE);
            gtk_tree_store_set(gtk_1990_groupes->tree_store_etat, &element->Iter, 0, num_element, 1, action->nom, -1);
        }
        else
        {
            Niveau_Groupe *groupe_niveau_moins_1;
            Groupe *groupe_moins_1;
            
            BUG(groupe_niveau_moins_1 = _1990_groupe_positionne_niveau(projet, niveau-1), FALSE);
            BUG(groupe_moins_1 = _1990_groupe_positionne_groupe(groupe_niveau_moins_1, num_element), FALSE);
            gtk_tree_store_set(gtk_1990_groupes->tree_store_etat, &element->Iter, 0, num_element, 1, groupe_moins_1->nom, -1);
        }
        
        // On développe la ligne dans le groupe où l'élément vient d'être mis.
        groupe->Iter_expand = 1;
        path = gtk_tree_model_get_path(GTK_TREE_MODEL(gtk_1990_groupes->tree_store_etat), &groupe->Iter);
        gtk_tree_view_expand_row(gtk_1990_groupes->tree_view_etat, path, FALSE);
        gtk_tree_path_free(path);
    }
    #endif
    
    return TRUE;
}


gboolean _1990_groupe_modifie_combinaison(Groupe *groupe,
  Type_Groupe_Combinaison type_combinaison)
/* Description : Modifie le type d'une combinaison (OR, XOR ou AND)
 * Paramètres : Groupe *groupe : le groupe à modifier,
 *            : Type_Groupe_Combinaison type_combinaison : le nouveau type de combinaison.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             groupe == NULL,
 *             type_combinaison inconnu.
 */
{
    BUGMSG(groupe, FALSE, gettext("Paramètre %s incorrect.\n"), "groupe");
    
    switch (type_combinaison)
    {
        case GROUPE_COMBINAISON_AND :
        case GROUPE_COMBINAISON_OR :
        case GROUPE_COMBINAISON_XOR :
        {
            groupe->type_combinaison = type_combinaison;
            break;
        }
        default :
        {
            BUGMSG(0, FALSE, gettext("Le type de combinaison %d est inconnu.\n"), type_combinaison);
            break;
        }
    }
    
    return TRUE;
}


gboolean _1990_groupe_modifie_nom(unsigned int niveau, unsigned int numero, const char *nom,
  Projet* projet)
/* Description : Modifie le nom d'un groupe
 * Paramètres : unsigned int niveau : niveau où se trouve le groupe,
 *            : unsigned int numero : numéro du groupe à modifier,
 *            : const char *nom : le nouveau nom,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             groupe == NULL,
 *             erreur d'allocation mémoire.
 */
{
    Niveau_Groupe   *groupe_niveau;
    Groupe          *groupe;
    
    BUG(groupe_niveau = _1990_groupe_positionne_niveau(projet, niveau), FALSE);
    BUG(groupe = _1990_groupe_positionne_groupe(groupe_niveau, numero), FALSE);
    
    free(groupe->nom);
    BUGMSG(groupe->nom = g_strdup_printf("%s", nom), FALSE, gettext("Erreur d'allocation mémoire.\n"));
    
#ifdef ENABLE_GTK
    if (projet->list_gtk._1990_groupes.builder != NULL)
    {
        // Si on est dans le niveau en cours, il n'y a que l'Iter du groupe à modifier.
        if (GTK_COMMON_SPINBUTTON_AS_UINT(GTK_SPIN_BUTTON(projet->list_gtk._1990_groupes.spin_button_niveau)) == niveau)
            gtk_tree_store_set(projet->list_gtk._1990_groupes.tree_store_etat, &groupe->Iter, 1, nom, -1);
        // Si on se trouve dans le niveau supérieur, il y a la ligne de l'élément du groupe qui
        // contient le groupe en cours et, à défaut, la ligne dans la liste des éléments
        // disponibles.
        else if (GTK_COMMON_SPINBUTTON_AS_UINT(GTK_SPIN_BUTTON(projet->list_gtk._1990_groupes.spin_button_niveau)) == niveau+1)
        {
            GtkTreeIter iter, iter_child;
            
            // On parcours toutes les lignes pour chercher si le groupe se trouve dans
            // tree_store_etat et pour modifier la ligne.
            // Récupération de la première ligne
            if (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(projet->list_gtk._1990_groupes.tree_store_etat), &iter))
            {
                // Récupération du premier enfant
                if (gtk_tree_model_iter_children(GTK_TREE_MODEL(projet->list_gtk._1990_groupes.tree_store_etat), &iter_child, &iter))
                {
                    gtk_tree_model_get(GTK_TREE_MODEL(projet->list_gtk._1990_groupes.tree_store_etat), &iter_child, 0, &numero, -1);
                    if (numero == groupe->numero)
                    {
                        gtk_tree_store_set(projet->list_gtk._1990_groupes.tree_store_etat, &iter_child, 1, nom, -1);
                        return TRUE;
                    }
                    else
                    {
                        // Récupération des suivants.
                        while (gtk_tree_model_iter_next(GTK_TREE_MODEL(projet->list_gtk._1990_groupes.tree_store_etat), &iter_child))
                        {
                            gtk_tree_model_get(GTK_TREE_MODEL(projet->list_gtk._1990_groupes.tree_store_etat), &iter_child, 0, &numero, -1);
                            if (numero == groupe->numero)
                            {
                                gtk_tree_store_set(projet->list_gtk._1990_groupes.tree_store_etat, &iter_child, 1, nom, -1);
                                return TRUE;
                            }
                        }
                    }
                }
                    
                // On parcours les lignes suivantes.
                while (gtk_tree_model_iter_next(GTK_TREE_MODEL(projet->list_gtk._1990_groupes.tree_store_etat), &iter))
                {
                    if (gtk_tree_model_iter_children(GTK_TREE_MODEL(projet->list_gtk._1990_groupes.tree_store_etat), &iter_child, &iter))
                    {
                        gtk_tree_model_get(GTK_TREE_MODEL(projet->list_gtk._1990_groupes.tree_store_etat), &iter_child, 0, &numero, -1);
                        if (numero == groupe->numero)
                        {
                            gtk_tree_store_set(projet->list_gtk._1990_groupes.tree_store_etat, &iter_child, 1, nom, -1);
                            return TRUE;
                        }
                        else
                        {
                            while (gtk_tree_model_iter_next(GTK_TREE_MODEL(projet->list_gtk._1990_groupes.tree_store_etat), &iter_child))
                            {
                                gtk_tree_model_get(GTK_TREE_MODEL(projet->list_gtk._1990_groupes.tree_store_etat), &iter_child, 0, &numero, -1);
                                if (numero == groupe->numero)
                                {
                                    gtk_tree_store_set(projet->list_gtk._1990_groupes.tree_store_etat, &iter_child, 1, nom, -1);
                                    return TRUE;
                                }
                            }
                        }
                    }
                }
            }
            
            // Si on arrive ici, c'est que le groupe se trouve dans les éléments dispos.
            if (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(projet->list_gtk._1990_groupes.tree_store_dispo), &iter))
            {
                gtk_tree_model_get(GTK_TREE_MODEL(projet->list_gtk._1990_groupes.tree_store_dispo), &iter, 0, &numero, -1);
                if (numero == groupe->numero)
                {
                    gtk_tree_store_set(projet->list_gtk._1990_groupes.tree_store_dispo, &iter, 1, nom, -1);
                    return TRUE;
                }
                else
                {
                    // Récupération des suivants.
                    while (gtk_tree_model_iter_next(GTK_TREE_MODEL(projet->list_gtk._1990_groupes.tree_store_dispo), &iter))
                    {
                        gtk_tree_model_get(GTK_TREE_MODEL(projet->list_gtk._1990_groupes.tree_store_dispo), &iter, 0, &numero, -1);
                        if (numero == groupe->numero)
                        {
                            gtk_tree_store_set(projet->list_gtk._1990_groupes.tree_store_dispo, &iter, 1, nom, -1);
                            return TRUE;
                        }
                    }
                }
            }
            
            // Sécurité...
            BUGMSG(NULL, FALSE, gettext("Erreur lors de l'actualisation de la fenêtre graphique.\n"));
        }
    }
#endif
    
    return TRUE;
}


gboolean _1990_groupe_affiche_tout(Projet *projet)
/* Description : Affiche tous les groupes y compris les combinaisons temporaires de tous les
 *               niveaux. La valeur entre parenthèses 0 ou 1 représente si l'action est
 *               prédominante (1) ou pas (0).
 * Paramètres : Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet != NULL.
 */
{
    GList   *list_parcours;
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    
    // Trivial
    list_parcours = projet->niveaux_groupes;
    while (list_parcours != NULL)
    {
        Niveau_Groupe   *niveau = list_parcours->data;
        GList           *list_parcours2 = niveau->groupes;
        
        printf(gettext("niveau : %d\n"), niveau->numero);
        
        while (list_parcours2 != NULL)
        {
            Groupe  *groupe = list_parcours2->data;
            GList   *list_parcours3 = groupe->elements;
            
            printf(gettext("\tgroupe : %d, combinaison : "), groupe->numero);
            switch (groupe->type_combinaison)
            {
                case GROUPE_COMBINAISON_OR :
                {
                    printf("OR\n");
                    break;
                }
                case GROUPE_COMBINAISON_XOR :
                {
                    printf("XOR\n");
                    break;
                }
                case GROUPE_COMBINAISON_AND :
                {
                    printf("AND\n");
                    break;
                }
                default :
                {
                    BUGMSG(0, FALSE, gettext("Combinaison %d inconnue"), groupe->type_combinaison);
                    break;
                }
            }
            if (projet->niveaux_groupes->data == list_parcours->data)
                printf(gettext("\t\tActions contenus dans ce groupe : "));
            else
                printf(gettext("\t\tGroupes du niveau %d contenus dans ce groupe : "), niveau->numero-1);
            
            while (list_parcours3 != NULL)
            {
                Element *element = list_parcours3->data;
                printf("%d ", element->numero);
                
                list_parcours3 = g_list_next(list_parcours3);
            }
            printf("\n");
            printf(gettext("\t\tCombinaisons :\n"));
            if (groupe->tmp_combinaison != NULL)
            {
                list_parcours3 = groupe->tmp_combinaison;
                
                do
                {
                    GList   *combinaison = list_parcours3->data;
                    
                    printf("\t\t\t");
                    if (combinaison != NULL)
                    {
                        GList   *list_parcours4 = combinaison;
                        do
                        {
                            Combinaison_Element *comb_element = list_parcours4->data;
                            Action          *action = (Action*)comb_element->action;
                            
                            printf("%u(%d) ", action->numero, comb_element->flags);
                            
                            list_parcours4 = g_list_next(list_parcours4);
                        }
                        while (list_parcours4 != NULL);
                    }
                    printf("\n");
                    list_parcours3 = g_list_next(list_parcours3);
                }
                while (list_parcours3 != NULL);
            }
            list_parcours2 = g_list_next(list_parcours2);
        }
        list_parcours = g_list_next(list_parcours);
    }
    
    return TRUE;
}


gboolean _1990_groupe_free_element(Projet *projet, unsigned int niveau, unsigned int groupe,
  unsigned int element)
/* Description : Libère l'élément désigné par les paramètres.
 * Paramètres : Projet *projet : variable projet,
 *            : unsigned int niveau : le numéro du niveau contenant l'élément,
 *            : unsigned int groupe : le numéro du groupe contenant l'élément,
 *            : unsigned int element : le numéro de l'élément.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet != NULL,
 *             _1990_groupe_positionne_niveau,
 *             _1990_groupe_positionne_groupe,
 *             _1990_groupe_positionne_element.
 */
{
    Niveau_Groupe   *niveau_groupe;
    Groupe          *groupe_en_cours;
    Element         *element_en_cours;
    
    // Trivial
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    
    BUG(niveau_groupe = _1990_groupe_positionne_niveau(projet, niveau), FALSE);
    BUG(groupe_en_cours = _1990_groupe_positionne_groupe(niveau_groupe, groupe), FALSE);
    BUG(element_en_cours = _1990_groupe_positionne_element(groupe_en_cours, element), FALSE);
    
    // On sélectionne dans la liste des groupes la ligne suivante. Et si elle n'existe pas,
    // on sélectionne la ligne précédente.
#ifdef ENABLE_GTK
    // Si c'est le bon niveau qui est affiché
    if ((projet->list_gtk._1990_groupes.builder != NULL) && (GTK_COMMON_SPINBUTTON_AS_UINT(GTK_SPIN_BUTTON(projet->list_gtk._1990_groupes.spin_button_niveau)) == niveau))
    {
        GtkTreePath     *path;
        
        // Si il y a une sélection
        if (gtk_tree_selection_iter_is_selected(projet->list_gtk._1990_groupes.tree_select_etat, &element_en_cours->Iter))
        {
            // On décale la sélection
            path = gtk_tree_model_get_path(GTK_TREE_MODEL(projet->list_gtk._1990_groupes.tree_store_etat), &element_en_cours->Iter);
            gtk_tree_path_next(path);
            gtk_tree_selection_select_path(projet->list_gtk._1990_groupes.tree_select_etat, path);
            if (!gtk_tree_selection_path_is_selected(projet->list_gtk._1990_groupes.tree_select_etat, path))
            {
                gtk_tree_path_prev(path);
                if (gtk_tree_path_prev(path))
                    gtk_tree_selection_select_path(projet->list_gtk._1990_groupes.tree_select_etat, path);
            }
            gtk_tree_path_free(path);
        }
        gtk_tree_store_remove(projet->list_gtk._1990_groupes.tree_store_etat, &element_en_cours->Iter);
    }
#endif
    
    groupe_en_cours->elements = g_list_remove(groupe_en_cours->elements, element_en_cours);
    free(element_en_cours);
    
    return TRUE;
}


gboolean _1990_groupe_free_niveau(Projet *projet, unsigned int niveau, gboolean accept_vide)
/* Description : Libère le niveau en cours ainsi que tous les niveaux supérieurs.
 * Paramètres : Projet *projet : la variable projet,
 *            : unsigned int niveau : numéro du niveau à libérer.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL,
 *             _1990_groupe_positionne_niveau.
 */
{
    GList           *list_parcours;
    Niveau_Groupe   *niveau_groupe;
    
    // Trivial
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUG(niveau_groupe = _1990_groupe_positionne_niveau(projet, niveau), FALSE);
    
    list_parcours = projet->niveaux_groupes;
    do
    {
        niveau_groupe = list_parcours->data;
        if (niveau_groupe->numero >= niveau)
        {
            /* Il peut être possible d'utiliser la fonction _1990_groupe_free_groupe
             * mais cette dernier analyse également le niveau supérieur pour supprimer les 
             * références devenues obsolète, ce qui est inutile ici puisque tous les niveaux
             * supérieurs vont également être supprimés. */
            while (niveau_groupe->groupes != NULL)
            {
                Groupe      *groupe = niveau_groupe->groupes->data;
                
                free(groupe->nom);
                
                /* On libère tous les éléments contenus dans le groupe */
                if (groupe->elements != NULL)
                    g_list_free_full(groupe->elements, free);
                
                /* On libère toutes les combinaisons temporaires */
                if (groupe->tmp_combinaison != NULL)
                {
                    while (groupe->tmp_combinaison != NULL)
                    {
                        GList   *combinaison = groupe->tmp_combinaison->data;
                        
                        if (combinaison != NULL)
                            g_list_free_full(combinaison, free);
                        groupe->tmp_combinaison = g_list_delete_link(groupe->tmp_combinaison, groupe->tmp_combinaison);
                    }
                    free(groupe->tmp_combinaison);
                }
                
                free(groupe);
                
                /* Et enfin, on supprime l'élément courant */
                niveau_groupe->groupes = g_list_delete_link(niveau_groupe->groupes, niveau_groupe->groupes);
            }
            
            list_parcours = g_list_next(list_parcours);
            projet->niveaux_groupes = g_list_remove(projet->niveaux_groupes, niveau_groupe);
            
            free(niveau_groupe);
        }
        else
            list_parcours = g_list_next(list_parcours);
        if (list_parcours != NULL)
            niveau_groupe = list_parcours->data;
        else
            niveau_groupe = NULL;
    }
    while ((projet->niveaux_groupes != NULL) && (list_parcours != NULL));
    
    /* On oblige la liste des niveaux à posséder au moins un niveau vide */
    if ((projet->niveaux_groupes == NULL) && (accept_vide == FALSE))
    {
        BUG(_1990_groupe_ajout_niveau(projet), FALSE);
#ifdef ENABLE_GTK
        if (projet->list_gtk._1990_groupes.builder != NULL)
        {
            gtk_tree_store_clear(projet->list_gtk._1990_groupes.tree_store_etat);
            BUG(_1990_gtk_groupes_affiche_niveau(projet, 0), FALSE);
        }
#endif
    }
    
#ifdef ENABLE_GTK
    if (projet->list_gtk._1990_groupes.builder != NULL)
    {
        /* On réajuste les limites du spin_button */
        gtk_spin_button_set_range(GTK_SPIN_BUTTON(projet->list_gtk._1990_groupes.spin_button_niveau), 0, g_list_length(projet->niveaux_groupes)-1);
    }
#endif
    
    return TRUE;
}


gboolean _1990_groupe_free_groupe(Projet *projet, unsigned int niveau, unsigned int groupe)
/* Description : Libère le groupe demandé. La suppression d'un groupe entraine la modification
 *               du numéro (moins 1) des groupes supérieurs du même niveau et sa suppression
 *               dans le niveau supérieur (si existant) lorsqu'il est présent dans une 
 *               combinaison.
 * Paramètres : Projet *projet : la variable projet,
 *            : unsigned int niveau : niveau contenant le groupe,
 *            : unsigned int groupe : numéro du groupe à libérer.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL,
 *             _1990_groupe_positionne_niveau,
 *             _1990_groupe_positionne_groupe.
 */
{
    Niveau_Groupe   *niveau_groupe;
    Groupe          *groupe_curr;
#ifdef ENABLE_GTK
    Groupe          *groupe_parcours;
#endif
    GList           *list_parcours;
    
    // Trivial
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUG(niveau_groupe = _1990_groupe_positionne_niveau(projet, niveau), FALSE);
    BUG(groupe_curr = _1990_groupe_positionne_groupe(niveau_groupe, groupe), FALSE);
    
#ifdef ENABLE_GTK
    if ((projet->list_gtk._1990_groupes.builder != NULL) && (GTK_COMMON_SPINBUTTON_AS_UINT(GTK_SPIN_BUTTON(projet->list_gtk._1990_groupes.spin_button_niveau)) == niveau))
    {
        GtkTreePath *path;
        
        // On sélectionne dans la liste des groupes la ligne suivante. Et si elle n'existe pas,
        // on sélectionne la ligne précédente.
        path = gtk_tree_model_get_path(GTK_TREE_MODEL(projet->list_gtk._1990_groupes.tree_store_etat), &groupe_curr->Iter);
        gtk_tree_path_next(path);
        gtk_tree_selection_select_path(projet->list_gtk._1990_groupes.tree_select_etat, path);
        if (!gtk_tree_selection_path_is_selected(projet->list_gtk._1990_groupes.tree_select_etat, path))
        {
            gtk_tree_path_prev(path);
            if (gtk_tree_path_prev(path))
                gtk_tree_selection_select_path(projet->list_gtk._1990_groupes.tree_select_etat, path);
        }
        gtk_tree_path_free(path);
        
        gtk_tree_selection_unselect_all(projet->list_gtk._1990_groupes.tree_select_dispo);
        
        /* On supprimer la ligne */
        gtk_tree_store_remove(projet->list_gtk._1990_groupes.tree_store_etat, &groupe_curr->Iter);
        
  /* On ajoute tous les éléments associés au groupe dans la liste des éléments disponibles */
        list_parcours = groupe_curr->elements;
        while (list_parcours != NULL)
        {
            Element     *element = list_parcours->data;
            
            BUG(_1990_gtk_insert_dispo(projet, element->numero, niveau_groupe) == 0, FALSE);
            
            list_parcours = g_list_next(list_parcours);
        }
        
    /* Cette fonction est pour éviter d'avoir à utiliser la fonction
     * _1990_gtk_groupes_affiche_niveau (optimisation). Une mise à jour du composant graphique
     * est nécessaire car, en supprimant le groupe en cours, tous les groupes avec un
     * numéro supérieur se retrouvent avec leur numéro diminué de 1. */
        list_parcours = niveau_groupe->groupes;
        while (list_parcours != NULL)
        {
            groupe_parcours = list_parcours->data;
            if (groupe_parcours->numero > groupe)
                gtk_tree_store_set(projet->list_gtk._1990_groupes.tree_store_etat, &groupe_parcours->Iter, 0, groupe_parcours->numero-1, -1);
            list_parcours = g_list_next(list_parcours);
        }
    }
#endif
    
    free(groupe_curr->nom);
    
    /* On libère tous les éléments contenus dans le groupe */
    if (groupe_curr->elements != NULL)
        g_list_free_full(groupe_curr->elements, free);
    
    /* On libère toutes les combinaisons temporaires */
    if (groupe_curr->tmp_combinaison != NULL)
    {
        while (groupe_curr->tmp_combinaison != NULL)
        {
            GList   *combinaison = groupe_curr->tmp_combinaison->data;
            
            if (combinaison != NULL)
                g_list_free_full(combinaison, free);
            groupe_curr->tmp_combinaison = g_list_delete_link(groupe_curr->tmp_combinaison, groupe_curr->tmp_combinaison);
        }
    }
    
    /* Et enfin, on supprime l'élément courant */
    niveau_groupe->groupes = g_list_remove(niveau_groupe->groupes, groupe_curr);
    free(groupe_curr);
    
    /* Après la suppression du groupe, on décale d'un numéro tous les groupes supérieurs
     * afin de ne pas avoir de séparation entre les numéros */
    if (niveau_groupe->groupes != NULL)
    {
        list_parcours = niveau_groupe->groupes;
        do
        {
            groupe_curr = list_parcours->data;
            
            if (groupe_curr->numero > groupe)
                groupe_curr->numero--;
            
            list_parcours = g_list_next(list_parcours);
        }
        while (list_parcours != NULL);
    }
    
    list_parcours = g_list_find(projet->niveaux_groupes, niveau_groupe);
    
    list_parcours = g_list_next(list_parcours);
    
    /* On passe au niveau suivant (s'il existe) */
    if (list_parcours != NULL)
    {
        niveau_groupe = list_parcours->data;
        if (niveau_groupe->groupes != NULL)
        {
            GList   *list_parcours2 = niveau_groupe->groupes;
            do
            {
                /* On parcours tous les groupes pour vérifier si l'un possède l'élément
                 * qui a été supprimé.
                 * On ne s'arrête pas volontairement au premier élément qu'on trouve.
                 * Il est possible que quelqu'un trouve utile de pouvoir insérer un même
                 * élément dans plusieurs groupes */
                groupe_curr = list_parcours2->data;
                if (groupe_curr->elements != NULL)
                {
                    GList   *list_parcours3 = groupe_curr->elements;
                    do
                    {
                        Element     *element = list_parcours3->data;
                        
                        list_parcours3 = g_list_next(list_parcours3);
                        
                        if (element->numero == groupe)
                            groupe_curr->elements = g_list_remove(groupe_curr->elements, element);
                        else if (element->numero > groupe)
                            element->numero--;
                    }
                    while (list_parcours3 != NULL);
                }
                list_parcours2 = g_list_next(list_parcours2);
            }
            while (list_parcours2 != NULL);
        }
    }
    
    return TRUE;
}


gboolean _1990_groupe_free(Projet *projet)
/* Description : Libère l'ensemble des groupes et niveaux.
 * Paramètres : Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL,
 *             _1990_groupe_free_niveau.
 */
{
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    
    // Trivial
    /* On supprime tous les niveaux */
    BUG(_1990_groupe_free_niveau(projet, 0, TRUE), FALSE);
    
    return TRUE;
}
