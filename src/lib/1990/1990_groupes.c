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
    projet->niveaux_groupes = list_init();
    
    BUGMSG(projet->niveaux_groupes, -2, gettext("Erreur d'allocation mémoire.\n"));
    
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
    Niveau_Groupe   niveau_nouveau;
    
    // Trivial
    BUGMSG(projet, -1, gettext("Paramètre incorrect\n"));
    BUGMSG(projet->niveaux_groupes, -1, gettext("Paramètre incorrect\n"));
    
    list_mvrear(projet->niveaux_groupes);
    niveau_nouveau.niveau = list_size(projet->niveaux_groupes);
    niveau_nouveau.groupes = list_init();
    BUGMSG(niveau_nouveau.groupes, -2, gettext("Erreur d'allocation mémoire.\n"));
    
    BUGMSG(list_insert_after(projet->niveaux_groupes, &(niveau_nouveau), sizeof(niveau_nouveau)) , -2, gettext("Erreur d'allocation mémoire.\n"));
    
    return 0;
}


int _1990_groupe_positionne_element(Groupe *groupe, int numero)
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
    BUGMSG(groupe, -1, gettext("Paramètre incorrect\n"));
    BUGMSG(list_size(groupe->elements), -1, gettext("Paramètre incorrect\n"));
    
    // Trivial
    list_mvfront(groupe->elements);
    do
    {
        Element     *element_en_cours;
    
        element_en_cours = (Element*)list_curr(groupe->elements);
        if (element_en_cours->numero == numero)
            return 0;
    }
    while (list_mvnext(groupe->elements) != NULL);
    
    BUGMSG(0, -1, gettext("Élément %d introuvable.\n"), numero);
}


int _1990_groupe_positionne_groupe(Niveau_Groupe *niveau, int numero)
/* Description : Positionne le groupe courant d'un niveau en fonction de son numéro
 * Paramètres : Niveau_Groupe *niveau : niveau à analyser
 *            : int numero : numéro du groupe à trouver
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (niveau == NULL) ou
 *             (niveau->groupes == NULL) ou
 *             (list_size(niveau->groupes) == 0) ou
 *             groupe introuvable.
 */
{
    BUGMSG(niveau, -1, gettext("Paramètre incorrect\n"));
    BUGMSG(niveau->groupes, -1, gettext("Paramètre incorrect\n"));
    BUGMSG(list_size(niveau->groupes), -1, gettext("Paramètre incorrect\n"));
    
    // Trivial
    list_mvfront(niveau->groupes);
    do
    {
        Groupe *groupe = (Groupe*)list_curr(niveau->groupes);
        if (groupe->numero == numero)
            return 0;
    }
    while (list_mvnext(niveau->groupes) != NULL);
    BUGMSG(0, -2, gettext("Élément %d introuvable.\n"), numero);
}


int _1990_groupe_positionne_niveau(LIST *source, int numero)
/* Description : Positionne le niveau courant en fonction de son numéro
 * Paramètres : LIST *source : liste des niveaux
 *            : int numero : numéro du groupe à trouver
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (source == NULL) ou
 *             (list_size(source) == 0) ou
 *             niveau introuvable.
 */
{
    Niveau_Groupe *niveau;
    
    BUGMSG(source, -1, gettext("Paramètre incorrect\n"));
    BUGMSG(list_size(source), -1, gettext("Paramètre incorrect\n"));
    
    // Trivial
    list_mvfront(source);
    do
    {
        niveau = (Niveau_Groupe*)list_curr(source);
        if (niveau->niveau == numero)
            return 0;
    }
    while (list_mvnext(source) != NULL);
    BUGMSG(0, -1, gettext("Niveau %d introuvable.\n"), numero);
}


int _1990_groupe_ajout_groupe(Projet *projet, int niveau,
  Type_Groupe_Combinaison type_combinaison, const char* nom)
/* Description : Ajoute un groupe au niveau choisi avec le type de combinaison spécifié.
 * Paramètres : Projet *projet : la variable projet
 *            : int niveau : le niveau où le groupe doit être inséré
 *            : Type_Groupe_Combinaison type_combinaison : combinaison du nouveau groupe
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet == NULL) ou
 *             (projet->niveaux_groupes == NULL) ou
 *             (list_size(projet->niveaux_groupes) == 0)
 *             -2 en cas d'erreur d'allocation mémoire
 *             -3 en cas d'erreur due à une fonction interne
 */
{
    Groupe          groupe_nouveau;
    Niveau_Groupe   *niveau_groupe;
    
    BUGMSG(projet, -1, gettext("Paramètre incorrect\n"));
    BUGMSG(projet->niveaux_groupes, -1, gettext("Paramètre incorrect\n"));
    BUGMSG(list_size(projet->niveaux_groupes), -1, gettext("Paramètre incorrect\n"));
    
    // Trivial
    
    BUG(_1990_groupe_positionne_niveau(projet->niveaux_groupes, niveau) == 0, -3);
    
    niveau_groupe = (Niveau_Groupe*)list_curr(projet->niveaux_groupes);
    list_mvrear(niveau_groupe->groupes);
    groupe_nouveau.numero = list_size(niveau_groupe->groupes);
    BUGMSG(groupe_nouveau.nom = g_strdup_printf("%s", nom), -2, gettext("Erreur d'allocation mémoire.\n"));
    groupe_nouveau.type_combinaison = type_combinaison;
#ifdef ENABLE_GTK
    groupe_nouveau.Iter_expand = 1;
#endif
    
    groupe_nouveau.tmp_combinaison.combinaisons = list_init();
    BUGMSG(groupe_nouveau.tmp_combinaison.combinaisons, -2, gettext("Erreur d'allocation mémoire.\n"));
    
    groupe_nouveau.elements = list_init();
    BUGMSG(groupe_nouveau.elements, -2, gettext("Erreur d'allocation mémoire.\n"));
    BUGMSG(list_insert_after(niveau_groupe->groupes, &(groupe_nouveau), sizeof(groupe_nouveau)), -2, gettext("Erreur d'allocation mémoire.\n"));
    
    return 0;
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
 *            : int niveau : le niveau où le groupe doit être inséré.
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
    Element         *element, element_nouveau;
    
    BUGMSG(projet, -1, gettext("Paramètre incorrect\n"));
    BUGMSG(projet->niveaux_groupes, -1, gettext("Paramètre incorrect\n"));
    BUGMSG(list_size(projet->niveaux_groupes)-1 >= niveau, -1, gettext("Paramètre incorrect\n"));
    
    // Trivial
    /* On commence par positionner le numéro num_element de l'étage n-1
     * puis on positionne le niveau en cours au niveau 'niveau' */
    if (niveau == 0)
    {
        BUG(_1990_action_cherche_numero(projet, num_element) == 0, -3);
        list_mvfront(projet->niveaux_groupes);
    }
    else
    {
        BUG(_1990_groupe_positionne_niveau(projet->niveaux_groupes, niveau-1) == 0, -3);
        niveau_groupe = (Niveau_Groupe*)list_curr(projet->niveaux_groupes);
        BUG(_1990_groupe_positionne_groupe(niveau_groupe, num_element) == 0, -3);
        list_mvnext(projet->niveaux_groupes);
    }
    
    niveau_groupe = (Niveau_Groupe*)list_curr(projet->niveaux_groupes);
    BUG(_1990_groupe_positionne_groupe(niveau_groupe, groupe_n) == 0, -3);
    groupe = (Groupe*)list_curr(niveau_groupe->groupes);
    element_nouveau.numero = num_element;
    #ifdef ENABLE_GTK
    element_nouveau.Iter_expand = 1;
    #endif
    
    /* On ajoute le nouvel élément au groupe */
    if (list_size(groupe->elements) == 0)
        BUGMSG(list_insert_after(groupe->elements, &(element_nouveau), sizeof(element_nouveau)), -2, gettext("Erreur d'allocation mémoire.\n"));
    else
    {
        int element_ajoute = 0;
        
        list_mvfront(groupe->elements);
        element_ajoute = 0;
        /* On l'ajoute en triant pour faire plus joli */
        do
        {
            element = (Element*)list_curr(groupe->elements);
            BUGMSG(element->numero != num_element, -1, "le numéro %d est déjà présent.\n", num_element);
            if (element->numero > num_element)
            {
                BUGMSG(list_insert_before(groupe->elements, &(element_nouveau), sizeof(element_nouveau)), -2, gettext("Erreur d'allocation mémoire.\n"));
                element_ajoute = 1;
            }
        }
        while (list_mvnext(groupe->elements) && (element_ajoute == 0));
        /* Si pas encore ajouté, on l'ajoute à la fin de la liste */
        if (element_ajoute == 0)
            BUGMSG(list_insert_after(groupe->elements, &(element_nouveau), sizeof(element_nouveau)), -2, gettext("Erreur d'allocation mémoire.\n"));
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
    BUGMSG(projet, -1, gettext("Paramètre incorrect\n"));
    BUGMSG(projet->niveaux_groupes, -1, gettext("Paramètre incorrect\n"));
    BUGMSG(list_size(projet->niveaux_groupes), -1, gettext("Paramètre incorrect\n"));
    
    // Trivial
    list_mvfront(projet->niveaux_groupes);
    do
    {
        Niveau_Groupe   *niveau = (Niveau_Groupe*)list_curr(projet->niveaux_groupes);
        
        printf(gettext("niveau : %d\n"), niveau->niveau);
        if ((niveau->groupes != NULL) && (list_size(niveau->groupes) != 0))
        {
            list_mvfront(niveau->groupes);
            
            do
            {
                Groupe  *groupe = (Groupe*)list_curr(niveau->groupes);
                
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
                if (list_front(projet->niveaux_groupes) == list_curr(projet->niveaux_groupes))
                    printf(gettext("\t\tActions contenus dans ce groupe : "));
                else
                    printf(gettext("\t\tGroupes du niveau %d contenus dans ce groupe : "), niveau->niveau-1);
                
                if ((groupe->elements != NULL) && (list_size(groupe->elements) != 0))
                {
                    list_mvfront(groupe->elements);
                    
                    do
                    {
                        Element *element = (Element*)list_curr(groupe->elements);
                        printf("%d ", element->numero);
                    }
                    while (list_mvnext(groupe->elements) != NULL);
                }
                printf("\n");
                printf(gettext("\t\tCombinaisons :\n"));
                if ((groupe->tmp_combinaison.combinaisons != NULL) && (list_size(groupe->tmp_combinaison.combinaisons) != 0))
                {
                    list_mvfront(groupe->tmp_combinaison.combinaisons);
                    
                    do
                    {
                        Combinaison *combinaison = (Combinaison*)list_curr(groupe->tmp_combinaison.combinaisons);
                        printf("\t\t\t");
                        if ((combinaison->elements != NULL) && (list_size(combinaison->elements) != 0))
                        {
                            list_mvfront(combinaison->elements);
                            do
                            {
                                Combinaison_Element *comb_element = (Combinaison_Element*)list_curr(combinaison->elements);
                                Action          *action = (Action*)comb_element->action;
                                
                                printf("%u(%d) ", action->numero, comb_element->flags);
                            }
                            while (list_mvnext(combinaison->elements) != NULL);
                        }
                        printf("\n");
                    }
                    while (list_mvnext(groupe->tmp_combinaison.combinaisons) != NULL);
                }
            }
            while (list_mvnext(niveau->groupes) != NULL);
        }
    }
    while (list_mvnext(projet->niveaux_groupes) != NULL);
    
    return 0;
}


int _1990_groupe_free_element_courant(LIST *elements)
/* Description : Libère l'élément en cours dans une liste d'éléments
 * Paramètres : LIST *elements : une liste d'éléments
 * Valeur renvoyée : Aucune.
 */
{
    // Trivial
    
    BUGMSG(elements, -1, gettext("Paramètre incorrect\n"));
    BUGMSG(list_curr(elements), -1, gettext("Paramètre incorrect\n"));
    
    free(list_remove_curr(elements));
    
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
    
    // Trivial
    
    BUGMSG(projet, -1, gettext("Paramètre incorrect\n"));
    BUGMSG(projet->niveaux_groupes, -1, gettext("Paramètre incorrect\n"));
    BUGMSG(list_size(projet->niveaux_groupes), -1, gettext("Paramètre incorrect\n"));
    BUG(_1990_groupe_positionne_niveau(projet->niveaux_groupes, niveau) == 0, -3);
    
    niveau_groupe = (Niveau_Groupe*)list_curr(projet->niveaux_groupes);
    
    BUGMSG(niveau_groupe->groupes, -1, gettext("Paramètre incorrect\n"));
    BUGMSG(list_size(niveau_groupe->groupes), -1, gettext("Paramètre incorrect\n"));
    BUG(_1990_groupe_positionne_groupe(niveau_groupe, groupe) == 0, -3);
    
    groupe_en_cours = (Groupe*)list_curr(niveau_groupe->groupes);
    
    BUGMSG(groupe_en_cours->elements, -1, gettext("Paramètre incorrect\n"));
    BUGMSG(list_size(groupe_en_cours->elements), -1, gettext("Paramètre incorrect\n"));
    BUG(_1990_groupe_positionne_element(groupe_en_cours, element) == 0, -3);
    
    BUG(_1990_groupe_free_element_courant(groupe_en_cours->elements) == 0, -3);
    
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
    Niveau_Groupe   *niveau_groupe;
    
    // Trivial
    BUGMSG(projet, -1, gettext("Paramètre incorrect\n"));
    BUGMSG(projet->niveaux_groupes, -1, gettext("Paramètre incorrect\n"));
    BUGMSG(list_size(projet->niveaux_groupes), -1, gettext("Paramètre incorrect\n"));
    BUG(_1990_groupe_positionne_niveau(projet->niveaux_groupes, niveau) == 0, -3);
    
    niveau_groupe = (Niveau_Groupe*)list_curr(projet->niveaux_groupes);
    
    do
    {
        if (niveau_groupe->niveau >= niveau)
        {
            /* Il peut être possible d'utiliser la fonction _1990_groupe_free_groupe
             * mais cette dernier analyse également le niveau supérieur pour supprimer les 
             * références devenues obsolète, ce qui est inutile ici puisque tous les niveaux
             * supérieurs vont également être supprimé. */
            if (niveau_groupe->groupes != NULL)
            {
                list_mvfront(niveau_groupe->groupes);
                while (!list_empty(niveau_groupe->groupes))
                {
                    Groupe      *groupe = (Groupe*)list_curr(niveau_groupe->groupes);
                    free(groupe->nom);
                    
                    /* On libère tous les éléments contenus dans le groupe */
                    if (groupe->elements != NULL)
                    {
                        list_mvfront(groupe->elements);
                        while (!list_empty(groupe->elements))
                        {
                            free(list_remove_front(groupe->elements));
                        }
                        free(groupe->elements);
                    }
                    
                    /* On libère toutes les combinaisons temporaires */
                    if (groupe->tmp_combinaison.combinaisons != NULL)
                    {
                        list_mvfront(groupe->tmp_combinaison.combinaisons);
                        while (!list_empty(groupe->tmp_combinaison.combinaisons))
                        {
                            Combinaison *combinaison = (Combinaison*)list_front(groupe->tmp_combinaison.combinaisons);
                            if (combinaison->elements != NULL)
                            {
                                list_mvfront(combinaison->elements);
                                while (!list_empty(combinaison->elements))
                                    free(list_remove_front(combinaison->elements));
                                free(combinaison->elements);
                            }
                            free(list_remove_front(groupe->tmp_combinaison.combinaisons));
                        }
                        free(groupe->tmp_combinaison.combinaisons);
                    }
                    
                    /* Et enfin, on supprime l'élément courant */
                    free(list_remove_curr(niveau_groupe->groupes));
                }
                free(niveau_groupe->groupes);
            }
            
            free(list_remove_curr(projet->niveaux_groupes));
            
            niveau_groupe = (Niveau_Groupe*)list_curr(projet->niveaux_groupes);
        }
        else
        {
            list_mvnext(projet->niveaux_groupes);
            niveau_groupe = (Niveau_Groupe*)list_curr(projet->niveaux_groupes);
        }
    }
    while ((list_size(projet->niveaux_groupes) != 0) && (niveau_groupe != NULL) && (niveau_groupe->niveau >= niveau));
    
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
    
    // Trivial
    BUGMSG(projet, -1, gettext("Paramètre incorrect\n"));
    BUGMSG(projet->niveaux_groupes, -1, gettext("Paramètre incorrect\n"));
    BUGMSG(list_size(projet->niveaux_groupes), -1, gettext("Paramètre incorrect\n"));
    BUG(_1990_groupe_positionne_niveau(projet->niveaux_groupes, niveau) == 0, -3);
    
    niveau_groupe = (Niveau_Groupe*)list_curr(projet->niveaux_groupes);
    
    BUGMSG(niveau_groupe->groupes, -1, gettext("Paramètre incorrect\n"));
    BUGMSG(list_size(niveau_groupe->groupes), -1, gettext("Paramètre incorrect\n"));
    BUG(_1990_groupe_positionne_groupe(niveau_groupe, groupe) == 0, -1);
    
    groupe_curr = (Groupe*)list_curr(niveau_groupe->groupes);
    
    free(groupe_curr->nom);
    
    /* On libère tous les éléments contenus dans le groupe */
    if (groupe_curr->elements != NULL)
    {
        list_mvfront(groupe_curr->elements);
        while (!list_empty(groupe_curr->elements))
            BUG(_1990_groupe_free_element_courant(groupe_curr->elements) == 0, -3);
        free(groupe_curr->elements);
    }
    
    /* On libère toutes les combinaisons temporaires */
    if (groupe_curr->tmp_combinaison.combinaisons != NULL)
    {
        list_mvfront(groupe_curr->tmp_combinaison.combinaisons);
        while (!list_empty(groupe_curr->tmp_combinaison.combinaisons))
        {
            Combinaison *combinaison = (Combinaison*)list_front(groupe_curr->tmp_combinaison.combinaisons);
            if (combinaison->elements != NULL)
            {
                list_mvfront(combinaison->elements);
                while (!list_empty(combinaison->elements))
                    free(list_remove_front(combinaison->elements));
                free(combinaison->elements);
            }
            free(list_remove_front(groupe_curr->tmp_combinaison.combinaisons));
        }
        free(groupe_curr->tmp_combinaison.combinaisons);
    }
    
    /* Et enfin, on supprime l'élément courant */
    free(list_remove_curr(niveau_groupe->groupes));
    
    /* Après la suppression du groupe, on décale d'un numéro tous les groupes supérieurs
     * afin de ne pas avoir de séparation entre les numéros */
    if (list_size(niveau_groupe->groupes) != 0)
    {
        list_mvfront(niveau_groupe->groupes);
        do
        {
            groupe_curr = (Groupe*)list_curr(niveau_groupe->groupes);
            if (groupe_curr->numero > groupe)
                groupe_curr->numero--;
        }
        while (list_mvnext(niveau_groupe->groupes));
    }
    
    /* On passe au niveau suivant (s'il existe) */
    if (list_mvnext(projet->niveaux_groupes) != NULL)
    {
        niveau_groupe = (Niveau_Groupe*)list_curr(projet->niveaux_groupes);
        if ((niveau_groupe->groupes != NULL) && (list_size(niveau_groupe->groupes) != 0))
        {
            list_mvfront(niveau_groupe->groupes);
            do
            {
                /* On parcours tous les groupes pour vérifier si l'un possède l'élément
                 * qui a été supprimé.
                 * On ne s'arrête pas volontairement au premier élément qu'on trouve.
                 * Il est possible que quelqu'un trouve utile de pouvoir insérer un même
                 * élément dans plusieurs groupes */
                groupe_curr = (Groupe*)list_curr(niveau_groupe->groupes);
                if ((groupe_curr->elements != NULL) && (list_size(groupe_curr->elements) != 0))
                {
                    list_mvfront(groupe_curr->elements);
                    do
                    {
                        int         dernier = 0;
                        Element     *element = (Element*)list_curr(groupe_curr->elements);
                        
                        if (element->numero == groupe)
                        {
                            if (element == list_rear(groupe_curr->elements))
                                dernier = 1;
                            free(list_remove_curr(groupe_curr->elements));
                            element = (Element*)list_curr(groupe_curr->elements);
                        }
                        if ((dernier == 0) && (element->numero > groupe))
                            element->numero--;
                    }
                    while (list_mvnext(groupe_curr->elements) != NULL);
                }
            }
            while (list_mvnext(niveau_groupe->groupes));
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
