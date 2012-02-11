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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "common_projet.h"
#include "common_erreurs.h"
#include "1990_actions.h"
#include "1990_groupes.h"
#include "1990_combinaisons.h"

int _1990_groupe_init(Projet *projet)
/* Description : Initialise la liste des groupes
 * Paramètres : Projet *projet : variable projet
 * Valeur renvoyée :
 *   Succès : 0 
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet == NULL)
 *           -2 en cas d'erreur d'allocation mémoire
 */
{
    BUGMSG(projet, -1, gettext("Paramètre incorrect\n"));
    
    // Trivial
    projet->niveaux_groupes = NULL;
    
    return 0;
}


int _1990_groupe_ajout_niveau(Projet *projet)
/* Description : Ajoute un niveau au projet en lui attribuant le numéro suivant le dernier
 *                 niveau existant
 * Paramètres : Projet *projet : variable projet
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet == NULL) ou
 *             (projet->niveaux_groupes == NULL)
 *           -2 en cas d'erreur d'allocation mémoire
 */
{
    Niveau_Groupe   *niveau_nouveau = malloc(sizeof(Niveau_Groupe));
    
    // Trivial
    BUGMSG(niveau_nouveau, -2, gettext("Erreur d'allocation mémoire.\n"));
    BUGMSG(projet, -1, gettext("Paramètre incorrect\n"));
    
    niveau_nouveau->niveau = g_list_length(projet->niveaux_groupes);
    niveau_nouveau->groupes = NULL;
    
    projet->niveaux_groupes = g_list_append(projet->niveaux_groupes, niveau_nouveau);
    
    return 0;
}


Element *_1990_groupe_positionne_element(Groupe *groupe, int numero)
/* Description : Positionne l'élément courant d'un groupe en fonction de son numéro
 * Paramètres : Groupe *groupe : groupe à analyser
 *            : int numero : numéro de l'élément à trouver
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (groupe == NULL) ou
 *             (list_size(groupe->elements) == 0) ou
 *             élément introuvable.
 */
{
    GList   *list_parcours;
    
    BUGMSG(groupe, NULL, gettext("Paramètre incorrect\n"));
    BUGMSG(groupe->elements, NULL, gettext("Paramètre incorrect\n"));
    
    // Trivial
    list_parcours = groupe->elements;
    do
    {
        Element     *element_en_cours = list_parcours->data;
        
        if (element_en_cours->numero == numero)
            return element_en_cours;
        
        list_parcours = g_list_next(list_parcours);
    }
    while (list_parcours != NULL);
    
    BUGMSG(0, NULL, gettext("Élément %d introuvable.\n"), numero);
}


Groupe *_1990_groupe_positionne_groupe(Niveau_Groupe *niveau, int numero)
/* Description : Renvoie le groupe d'un niveau en fonction de son numéro
 * Paramètres : Niveau_Groupe *niveau : niveau à analyser
 *            : int numero : numéro du groupe à trouver
 * Valeur renvoyée :
 *   Succès : pointeur vers le groupe
 *   Échec : NULL en cas de paramètres invalides :
 *             (niveau == NULL) ou
 *             (niveau->groupes == NULL) ou
 *             groupe introuvable.
 */
{
    GList   *list_parcours;
    
    BUGMSG(niveau, NULL, gettext("Paramètre incorrect\n"));
    BUGMSG(niveau->groupes, NULL, gettext("Paramètre incorrect\n"));
    
    // Trivial
    list_parcours = niveau->groupes;
    do
    {
        Groupe *groupe = list_parcours->data;
        if (groupe->numero == numero)
            return groupe;
         
        list_parcours = g_list_next(list_parcours);
    }
    while (list_parcours != NULL);
    
    BUGMSG(0, NULL, gettext("Élément %d introuvable.\n"), numero);
}


Niveau_Groupe *_1990_groupe_positionne_niveau(Projet *projet, int numero)
/* Description : Positionne le niveau courant en fonction de son numéro
 * Paramètres : Projet *projet : la variable projet
 *            : int numero : numéro du groupe à trouver
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet == NULL) ou
 *             (projet->niveaux_groupes == NULL) ou
 *             niveau introuvable.
 */
{
    GList           *list_parcours;
    Niveau_Groupe   *niveau;
    
    BUGMSG(projet, NULL, gettext("Paramètre incorrect\n"));
    BUGMSG(projet->niveaux_groupes, NULL, gettext("Paramètre incorrect\n"));
    
    // Trivial
    list_parcours = projet->niveaux_groupes;
    do
    {
        niveau = list_parcours->data;
        if (niveau->niveau == numero)
            return niveau;
        
        list_parcours = g_list_next(list_parcours);
    }
    while (list_parcours != NULL);
    
    BUGMSG(0, NULL, gettext("Niveau %d introuvable.\n"), numero);
}


Groupe *_1990_groupe_ajout_groupe(Projet *projet, int niveau,
  Type_Groupe_Combinaison type_combinaison, const char* nom)
/* Description : Ajoute un groupe au niveau choisi avec le type de combinaison spécifié.
 * Paramètres : Projet *projet : la variable projet
 *            : int niveau : le niveau où le groupe doit être inséré
 *            : Type_Groupe_Combinaison type_combinaison : combinaison du nouveau groupe
 * Valeur renvoyée :
 *   Succès : Pointeur vers le nouveau groupe.
 *   Échec : NULL en cas de paramètres invalides :
 *             (projet == NULL) ou
 *             (projet->niveaux_groupes == NULL) ou
 *             (list_size(projet->niveaux_groupes) == 0)
 *           NULL en cas d'erreur d'allocation mémoire
 *           NULL en cas d'erreur due à une fonction interne
 */
{
    Groupe          *groupe_nouveau = malloc(sizeof(Groupe));
    Niveau_Groupe   *niveau_groupe;
    
    BUGMSG(projet, NULL, gettext("Paramètre incorrect\n"));
    BUGMSG(projet->niveaux_groupes, NULL, gettext("Paramètre incorrect\n"));
    BUGMSG(groupe_nouveau, NULL, gettext("Erreur d'allocation mémoire.\n"));
    
    // Trivial
    
    BUG(niveau_groupe = _1990_groupe_positionne_niveau(projet, niveau), NULL);
    
    groupe_nouveau->numero = g_list_length(niveau_groupe->groupes);
    BUGMSG(groupe_nouveau->nom = g_strdup_printf("%s", nom), NULL, gettext("Erreur d'allocation mémoire.\n"));
    groupe_nouveau->type_combinaison = type_combinaison;
#ifdef ENABLE_GTK
    groupe_nouveau->Iter_expand = 1;
#endif
    
    groupe_nouveau->tmp_combinaison.combinaisons = NULL;
    
    groupe_nouveau->elements = NULL;
    niveau_groupe->groupes = g_list_append(niveau_groupe->groupes, groupe_nouveau);
    
    return groupe_nouveau;
}


int _1990_groupe_ajout_element(Projet *projet, unsigned int niveau, int groupe_n,
  int num_element)
/* Description : Ajoute l'élément 'num_element' au groupe 'groupe_n' du niveau 'niveau'.
 *                 L'élément appartient au niveau directement inférieur à 'niveau' et possède
 *                   le numéro 'num_element'.
 *                 Ainsi si 'niveau' est égal à 0, 'num_element' fait référence à une action
 *                   de projet->actions.
 *                 Si 'niveau' est supérieur à 0, 'num_element' fait référence à une groupe du
 *                   niveau 'niveau'-1
 *                 Le dernier niveau ne doit contenir qu'un seul groupe
 * Paramètres : Projet *projet : la variable projet
 *            : unsigned int niveau : le niveau où le groupe doit être inséré.
 *            : int groupe_n : numéro du groupe où ajouter l'élément.
 *            : int num_element : numéro de l'élément à ajouter.
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet == NULL) ou
 *             (projet->niveaux_groupes == NULL) ou
 *             (list_size(projet->niveaux_groupes)-1 < niveau)
 *           -2 en cas d'erreur d'allocation mémoire
 *           -3 en cas d'erreur due à une fonction interne
 */
{
    Niveau_Groupe   *niveau_groupe;
    Groupe          *groupe;
    Element         *element;
    Element         *element_nouveau = malloc(sizeof(Element));
    
    BUGMSG(projet, -1, gettext("Paramètre incorrect\n"));
    BUGMSG(projet->niveaux_groupes, -1, gettext("Paramètre incorrect\n"));
    BUGMSG(g_list_length(projet->niveaux_groupes)-1 >= niveau, -1, gettext("Paramètre incorrect\n"));
    BUGMSG(element_nouveau, -2, gettext("Erreur d'allocation mémoire.\n"));
    
    // Trivial
    /* On commence par positionner le numéro num_element de l'étage n-1
     * puis on positionne le niveau en cours au niveau 'niveau' */
    if (niveau == 0)
    {
        // On vérifie si l'action num_element existe.
        BUG(_1990_action_cherche_numero(projet, num_element), -3);
        niveau_groupe = projet->niveaux_groupes->data;
    }
    else
    {
        // On vérifie si le groupe du niveau n-1 existe;
        BUG(niveau_groupe = _1990_groupe_positionne_niveau(projet, niveau-1), -3);
        BUG(_1990_groupe_positionne_groupe(niveau_groupe, num_element), -3);
        BUG(niveau_groupe = _1990_groupe_positionne_niveau(projet, niveau), -3);
    }
    
    BUG(groupe = _1990_groupe_positionne_groupe(niveau_groupe, groupe_n), -3);
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
        
        list_parcours = groupe->elements;
        element_ajoute = 0;
        /* On l'ajoute en triant pour faire plus joli */
        do
        {
            element = list_parcours->data;
            BUGMSG(element->numero != num_element, -1, "le numéro %d est déjà présent.\n", num_element);
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
    
    return 0;
}


int _1990_groupe_affiche_tout(Projet *projet)
/* Description : Affiche tous les groupes y compris les combinaisons temporaires de tous les
 *                 niveaux. La valeur entre parenthèses 0 ou 1 représente si l'action est
 *                 prédominante (1) ou pas (0).
 * Paramètres : Projet *projet : la variable projet
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet != NULL) ou
 *             (projet->niveaux_groupes != NULL) ou
 *             (list_size(projet->niveaux_groupes) != 0)
 */
{
    GList   *list_parcours;
    BUGMSG(projet, -1, gettext("Paramètre incorrect\n"));
    BUGMSG(projet->niveaux_groupes, -1, gettext("Paramètre incorrect\n"));
    
    // Trivial
    list_parcours = projet->niveaux_groupes;
    do
    {
        Niveau_Groupe   *niveau = list_parcours->data;
        
        printf(gettext("niveau : %d\n"), niveau->niveau);
        if (niveau->groupes != NULL)
        {
            GList   *list_parcours2 = niveau->groupes;
            
            do
            {
                Groupe  *groupe = list_parcours2->data;
                
                printf(gettext("\tgroupe : %d, combinaison : "), groupe->numero);
                switch(groupe->type_combinaison)
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
                        BUGMSG(0, -1, gettext("Combinaison %d inconnue"), groupe->type_combinaison);
                        break;
                    }
                }
                if (projet->niveaux_groupes->data == list_parcours->data)
                    printf(gettext("\t\tActions contenus dans ce groupe : "));
                else
                    printf(gettext("\t\tGroupes du niveau %d contenus dans ce groupe : "), niveau->niveau-1);
                
                if (groupe->elements != NULL)
                {
                    GList   *list_parcours3 = groupe->elements;
                    
                    do
                    {
                        Element *element = list_parcours3->data;
                        printf("%d ", element->numero);
                        
                        list_parcours3 = g_list_next(list_parcours3);
                    }
                    while (list_parcours3 != NULL);
                }
                printf("\n");
                printf(gettext("\t\tCombinaisons :\n"));
                if (groupe->tmp_combinaison.combinaisons != NULL)
                {
                    GList   *list_parcours3 = groupe->tmp_combinaison.combinaisons;
                    
                    do
                    {
                        Combinaison *combinaison = list_parcours3->data;
                        printf("\t\t\t");
                        if (combinaison->elements != NULL)
                        {
                            GList   *list_parcours4 = combinaison->elements;
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
            while (list_parcours2 != NULL);
        }
        list_parcours = g_list_next(list_parcours);
    }
    while (list_parcours != NULL);
    
    return 0;
}


int _1990_groupe_free_element(Projet *projet, int niveau, int groupe, int element)
/* Description : Libère l'élément désigné par les paramètres.
 * Paramètres : Projet *projet : variable projet
 *            : int niveau : le numéro du niveau contenant l'élément
 *            : int groupe : le numéro du groupe contenant l'élément
 *            : int element : le numéro de l'élément
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet != NULL) ou
 *             (projet->niveaux_groupes != NULL) ou
 *             (list_size(projet->niveaux_groupes) != 0)
 *
 */
{
    Niveau_Groupe   *niveau_groupe;
    Groupe          *groupe_en_cours;
    Element         *element_en_cours;
    
    // Trivial
    
    BUGMSG(projet, -1, gettext("Paramètre incorrect\n"));
    BUGMSG(projet->niveaux_groupes, -1, gettext("Paramètre incorrect\n"));
    BUG(niveau_groupe = _1990_groupe_positionne_niveau(projet, niveau), -3);
    BUGMSG(niveau_groupe->groupes, -1, gettext("Paramètre incorrect\n"));
    BUG(groupe_en_cours = _1990_groupe_positionne_groupe(niveau_groupe, groupe), -3);
    BUGMSG(groupe_en_cours->elements, -1, gettext("Paramètre incorrect\n"));
    BUG(element_en_cours = _1990_groupe_positionne_element(groupe_en_cours, element), -3);
    groupe_en_cours->elements = g_list_remove(groupe_en_cours->elements, element_en_cours);
    
    return 0;
}


int _1990_groupe_free_niveau(Projet *projet, int niveau)
/* Description : Libère le niveau en cours ainsi que tous les niveaux supérieurs
 * Paramètres : Projet *projet : la variable projet
 *            : numéro du niveau à libérer
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides
 */
{
    GList           *list_parcours;
    Niveau_Groupe   *niveau_groupe;
    
    // Trivial
    BUGMSG(projet, -1, gettext("Paramètre incorrect\n"));
    BUGMSG(projet->niveaux_groupes, -1, gettext("Paramètre incorrect\n"));
    BUG(niveau_groupe = _1990_groupe_positionne_niveau(projet, niveau), -3);
    
    list_parcours = projet->niveaux_groupes;
    do
    {
        niveau_groupe = list_parcours->data;
        if (niveau_groupe->niveau >= niveau)
        {
            /* Il peut être possible d'utiliser la fonction _1990_groupe_free_groupe
             * mais cette dernier analyse également le niveau supérieur pour supprimer les 
             * références devenues obsolète, ce qui est inutile ici puisque tous les niveaux
             * supérieurs vont également être supprimé. */
            while (niveau_groupe->groupes != NULL)
            {
                Groupe      *groupe = niveau_groupe->groupes->data;
                
                free(groupe->nom);
                
                /* On libère tous les éléments contenus dans le groupe */
                if (groupe->elements != NULL)
                    g_list_free_full(groupe->elements, free);
                
                /* On libère toutes les combinaisons temporaires */
                if (groupe->tmp_combinaison.combinaisons != NULL)
                {
                    while (groupe->tmp_combinaison.combinaisons != NULL)
                    {
                        Combinaison *combinaison = groupe->tmp_combinaison.combinaisons->data;
                        if (combinaison->elements != NULL)
                            g_list_free_full(combinaison->elements, free);
                        free(combinaison);
                        groupe->tmp_combinaison.combinaisons = g_list_delete_link(groupe->tmp_combinaison.combinaisons, groupe->tmp_combinaison.combinaisons);
                    }
                    free(groupe->tmp_combinaison.combinaisons);
                }
                
                free(groupe);
                
                /* Et enfin, on supprime l'élément courant */
                niveau_groupe->groupes = g_list_delete_link(niveau_groupe->groupes, niveau_groupe->groupes);
            }
            
            list_parcours = g_list_next(list_parcours);
            projet->niveaux_groupes = g_list_remove(projet->niveaux_groupes, niveau_groupe);
        }
        else
            list_parcours = g_list_next(list_parcours);
        free(niveau_groupe);
        if (list_parcours != NULL)
            niveau_groupe = list_parcours->data;
        else
            niveau_groupe = NULL;
    }
    while ((projet->niveaux_groupes != NULL) && (list_parcours != NULL) && (niveau_groupe->niveau >= niveau));
    
    return 0;
}


int _1990_groupe_free_groupe(Projet *projet, int niveau, int groupe)
/* Description : Libère le groupe demandé. La suppression d'un groupe entraine la modification
 *                 du numéro (moins 1) des groupes supérieurs du même niveau et sa suppression
 *                 dans le nuveau supérieur (si existant) lorsqu'il est présent dans une
 *                 combinaison.
 * Paramètres : Projet *projet : la variable projet
 *            : int niveau : niveau contenant le groupe
 *            : int groupe : numéro du groupe à libérer
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides
 */
{
    Niveau_Groupe   *niveau_groupe;
    Groupe          *groupe_curr;
    GList           *list_parcours;
    
    // Trivial
    BUGMSG(projet, -1, gettext("Paramètre incorrect\n"));
    BUGMSG(projet->niveaux_groupes, -1, gettext("Paramètre incorrect\n"));
    BUG(niveau_groupe = _1990_groupe_positionne_niveau(projet, niveau), -3);
    BUGMSG(niveau_groupe->groupes, -1, gettext("Paramètre incorrect\n"));
    BUG(groupe_curr = _1990_groupe_positionne_groupe(niveau_groupe, groupe), -1);
    
    free(groupe_curr->nom);
    
    /* On libère tous les éléments contenus dans le groupe */
    if (groupe_curr->elements != NULL)
        g_list_free_full(groupe_curr->elements, free);
    
    /* On libère toutes les combinaisons temporaires */
    if (groupe_curr->tmp_combinaison.combinaisons != NULL)
    {
        while (groupe_curr->tmp_combinaison.combinaisons != NULL)
        {
            Combinaison *combinaison = groupe_curr->tmp_combinaison.combinaisons->data;
            if (combinaison->elements != NULL)
                g_list_free_full(combinaison->elements, free);
            free(combinaison);
            groupe_curr->tmp_combinaison.combinaisons = g_list_delete_link(groupe_curr->tmp_combinaison.combinaisons, groupe_curr->tmp_combinaison.combinaisons);
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
    
    return 0;
}


int _1990_groupe_free(Projet *projet)
/* Description : Libère l'ensemble des groupes et niveaux
 * Paramètres : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    BUGMSG(projet, -1, gettext("Paramètre incorrect\n"));
    BUGMSG(projet->niveaux_groupes, -1, gettext("Paramètre incorrect\n"));
    
    // Trivial
    /* On supprime tous les niveaux */
    BUG(_1990_groupe_free_niveau(projet, 0) == 0, -3);
    
    /* Et on libère la liste */
    free(projet->niveaux_groupes);
    projet->niveaux_groupes = NULL;
    
    return 0;
}
