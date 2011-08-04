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

#include "common_projet.h"
#include "common_erreurs.h"
#include "1990_actions.h"
#include "1990_groupes.h"
#include "1990_combinaisons.h"

/* _1990_groupe_init
 * Description : Initialise la liste des groupes
 * Paramètres : Projet *projet : variable projet
 * Valeur renvoyée :
 *   Succès : 0 
 *   Échec : valeur négative en cas d'erreur
 */
int _1990_groupe_init(Projet *projet)
{
    if (projet == NULL)
        BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
    
    projet->niveaux_groupes = list_init();
    
    if (projet->niveaux_groupes == NULL)
        BUGTEXTE(-2, gettext("Erreur d'allocation mémoire.\n"));
    else
        return 0;
}

/* _1990_groupe_niveau_ajout
 * Description : Ajoute un niveau au projet.
 *             : le numéro du niveau est attribué automatiquement.
 * Paramètres : Projet *projet : variable projet
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : valeur négative en cas d'erreur
 */
int _1990_groupe_niveau_ajout(Projet *projet)
{
    Niveau_Groupe   niveau_nouveau;
    
    if ((projet == NULL) || (projet->niveaux_groupes == NULL))
        BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
    
    list_mvrear(projet->niveaux_groupes);
    niveau_nouveau.niveau = list_size(projet->niveaux_groupes);
    niveau_nouveau.groupes = list_init();
    if (niveau_nouveau.groupes == NULL)
        BUGTEXTE(-2, gettext("Erreur d'allocation mémoire.\n"));
    
    niveau_nouveau.pIter = NULL;
    if (list_insert_after(projet->niveaux_groupes, &(niveau_nouveau), sizeof(niveau_nouveau)) == NULL)
        BUGTEXTE(-3, gettext("Erreur d'allocation mémoire.\n"));
    
    return 0;
}

/* _1990_groupe_element_positionne
 * Description : Positionne l'élément courant d'un groupe en fonction de son numéro
 * Paramètres : Groupe *groupe : groupe à analyser
 *            : int numero : numéro de l'élément à trouver
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : valeur négative en cas d'erreur
 */
int _1990_groupe_element_positionne(Groupe *groupe, int numero)
{
    if ((groupe == NULL) || (list_size(groupe->elements) == 0))
        BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
    
    list_mvfront(groupe->elements);
    do
    {
        Element     *element_en_cours;
    
        element_en_cours = list_curr(groupe->elements);
        if (element_en_cours->numero == numero)
            return 0;
    }
    while (list_mvnext(groupe->elements) != NULL);
    
    BUGTEXTE(-2, gettext("Élément introuvable.\n"));
}

/* _1990_groupe_positionne
 * Description : Positionne le groupe courant d'un niveau en fonction de son numéro
 * Paramètres : Niveau_Groupe *niveau : niveau à analyser
 *            : int numero : numéro du groupe à trouver
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : valeur négative en cas d'erreur
 */
int _1990_groupe_positionne(Niveau_Groupe *niveau, int numero)
{
    if ((niveau == NULL) || (list_size(niveau->groupes) == 0))
        BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
    
    list_mvfront(niveau->groupes);
    do
    {
        Groupe *groupe = list_curr(niveau->groupes);
        if (groupe->numero == numero)
            return 0;
    }
    while (list_mvnext(niveau->groupes) != NULL);
    BUGTEXTE(-2, gettext("Élément introuvable.\n"));
}

/* _1990_groupe_niveau_positionne
 * Description : Positionne le niveau courant en fonction de son numéro
 * Paramètres : LIST *source : liste des niveaux
 *            : int numero : numéro du groupe à trouver
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : valeur négative en cas d'erreur
 */
int _1990_groupe_niveau_positionne(LIST *source, int numero)
{
    Niveau_Groupe *niveau;
    
    if ((source == NULL) || (list_size(source) == 0))
        BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
    
    list_mvfront(source);
    do
    {
        niveau = list_curr(source);
        if (niveau->niveau == numero)
            return 0;
    }
    while (list_mvnext(source) != NULL);
    BUGTEXTE(-2, gettext("Élément introuvable.\n"));
}

/* _1990_groupe_ajout
 * Description : Ajoute un groupe au niveau choisi avec le numéro et le type de combinaison spécifié
 * Paramètres : Projet *projet : la variable projet
 *            : int niveau : le niveau où le groupe doit être inséré
 *            : int numero : numéro du nouveau groupe
 *            : Type_Groupe_Combinaison type_combinaison : type de combinaison du nouveau groupe
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : valeur négative en cas d'erreur
 */
int _1990_groupe_ajout(Projet *projet, int niveau, int numero, Type_Groupe_Combinaison type_combinaison)
{
    Groupe      groupe_nouveau;
    Niveau_Groupe   *niveau_groupe;
    
    if ((projet == NULL) || (projet->niveaux_groupes == NULL) || (list_size(projet->niveaux_groupes) == 0))
        BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
    
    if (_1990_groupe_niveau_positionne(projet->niveaux_groupes, niveau) != 0)
        BUG(-2);
    
    niveau_groupe = list_curr(projet->niveaux_groupes);
    list_mvrear(niveau_groupe->groupes);
    groupe_nouveau.numero = numero;
    groupe_nouveau.nom = NULL;
    groupe_nouveau.type_combinaison = type_combinaison;
    groupe_nouveau.pIter = NULL;
    groupe_nouveau.pIter_expand = 0;
    
    groupe_nouveau.tmp_combinaison.combinaisons = list_init();
    if (groupe_nouveau.tmp_combinaison.combinaisons == NULL)
        BUGTEXTE(-3, gettext("Erreur d'allocation mémoire.\n"));
    
    groupe_nouveau.elements = list_init();
    if (groupe_nouveau.elements == NULL)
        BUGTEXTE(-4, gettext("Erreur d'allocation mémoire.\n"));
    if (list_insert_after(niveau_groupe->groupes, &(groupe_nouveau), sizeof(groupe_nouveau)) == NULL)
        BUGTEXTE(-5, gettext("Erreur d'allocation mémoire.\n"));
    
    return 0;
}

/* _1990_groupe_ajout_element
 * Description : Ajoute un élément au groupe 'groupe_n' du niveau 'niveau'
 *             : L'élément appartient au niveau directement inférieur à 'niveau' et possède le numéro 'num_element'
 *             : Ainsi si 'niveau' = 0, 'num_element' faire référence à une action de projet->actions
 *             : Si 'niveau' est supérieur à 0, 'num_element' fait référence à une groupe du niveau 'niveau'-1
 *             : Le dernier niveau ne doit contenir qu'un seul groupe
 * Paramètres : Projet *projet : la variable projet
 *            : int niveau : le niveau où le groupe doit être inséré
 *            : int groupe_n : numéro du groupe où ajouter l'élément
 *            : int num_element : numéro de l'élément à ajouter.
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : valeur négative en cas d'erreur
 */
int _1990_groupe_ajout_element(Projet *projet, unsigned int niveau, int groupe_n, int num_element)
{
    Niveau_Groupe   *niveau_groupe;
    Groupe      *groupe;
    Element     *element, element_nouveau;
    
    if ((projet == NULL) || (projet->niveaux_groupes == NULL) || (list_size(projet->niveaux_groupes)-1 < niveau))
        BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
    
    // On commence par positionner le numéro num_element de l'étage n-1
    // puis on positionne le niveau en cours au niveau 'niveau'
    if (niveau == 0)
    {
        if (_1990_action_cherche_numero(projet, num_element) != 0)
            BUG(-2);
        list_mvfront(projet->niveaux_groupes);
    }
    else
    {
        if (_1990_groupe_niveau_positionne(projet->niveaux_groupes, niveau-1) != 0)
            BUG(-3);
        niveau_groupe = list_curr(projet->niveaux_groupes);
        if (_1990_groupe_positionne(niveau_groupe, num_element) != 0)
            BUG(-4);
        list_mvnext(projet->niveaux_groupes);
    }
    
    niveau_groupe = list_curr(projet->niveaux_groupes);
    if (_1990_groupe_positionne(niveau_groupe, groupe_n) != 0)
        BUG(-5);
    groupe = list_curr(niveau_groupe->groupes);
    element_nouveau.numero = num_element;
    #ifdef ENABLE_GTK
    element_nouveau.pIter = NULL;
    element_nouveau.pIter_expand = 0;
    #endif
    // On ajoute le nouvel élément au groupe
    if (list_size(groupe->elements) == 0)
    {
        if (list_insert_after(groupe->elements, &(element_nouveau), sizeof(element_nouveau)) == NULL)
            BUGTEXTE(-6, gettext("Erreur d'allocation mémoire.\n"));
    }
    else
    {
        int element_ajoute = 0;
        
        list_mvfront(groupe->elements);
        element_ajoute = 0;
        // On l'ajoute en triant
        do
        {
            element = list_curr(groupe->elements);
            if (element->numero == num_element)
                BUGTEXTE(-7, gettext("Erreur d'allocation mémoire.\n"));
            else if (element->numero > num_element)
            {
                if (list_insert_before(groupe->elements, &(element_nouveau), sizeof(element_nouveau)) == NULL)
                    BUG(-9);
                element_ajoute = 1;
            }
        }
        while (list_mvnext(groupe->elements) && (element_ajoute == 0));
        // Si pas encore ajouté, on l'ajoute à la fin de la liste
        if (element_ajoute == 0)
        {
            if (list_insert_after(groupe->elements, &(element_nouveau), sizeof(element_nouveau)) == NULL)
                BUG(-10);
        }
    }
    return 0;
}

/* _1990_groupe_affiche_tout
 * Description : Affiche tous les groupes y compris les combinaisons temporaires de tous les niveaux
 * Paramètres : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
void _1990_groupe_affiche_tout(Projet *projet)
{
    if ((projet != NULL) && (projet->niveaux_groupes != NULL) && (list_size(projet->niveaux_groupes) != 0))
    {
        list_mvfront(projet->niveaux_groupes);
        do
        {
            Niveau_Groupe   *niveau = (Niveau_Groupe*)list_curr(projet->niveaux_groupes);
            
            printf("niveau : %d\n", niveau->niveau);
            printf("\tGroupes\n");
            if ((niveau->groupes != NULL) && (list_size(niveau->groupes) != 0))
            {
                list_mvfront(niveau->groupes);
                
                do
                {
                    Groupe  *groupe = (Groupe*)list_curr(niveau->groupes);
                    
                    printf("\tgroupe : %d", groupe->numero);
                    switch(groupe->type_combinaison)
                    {
                        case GROUPE_COMBINAISON_OR :
                        {
                            printf(" OR\n");
                            break;
                        }
                        case GROUPE_COMBINAISON_XOR :
                        {
                            printf(" XOR\n");
                            break;
                        }
                        case GROUPE_COMBINAISON_AND :
                        {
                            printf(" AND\n");
                            break;
                        }
                    }
                    
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
                    printf("\tCombinaisons :\n");
                    if ((groupe->tmp_combinaison.combinaisons != NULL) && (list_size(groupe->tmp_combinaison.combinaisons) != 0))
                    {
                        list_mvfront(groupe->tmp_combinaison.combinaisons);
                        
                        do
                        {
                            Combinaison *combinaison = (Combinaison*)list_curr(groupe->tmp_combinaison.combinaisons);
                            printf("\t\tNouvelle combinaison : ");
                            if ((combinaison->elements != NULL) && (list_size(combinaison->elements) != 0))
                            {
                                list_mvfront(combinaison->elements);
                                do
                                {
                                    Combinaison_Element *comb_element = (Combinaison_Element*)list_curr(combinaison->elements);
                                    Action          *action = (Action*)comb_element->action;
                                    
                                    printf("%d(%d) ", action->numero, comb_element->flags);
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
    }
}

/* _1990_groupe_free_element_courant
 * Description : Libère l'élément en cours dans une liste d'éléments
 * Paramètres : LIST *elements : une liste d'éléments
 * Valeur renvoyée : Aucune
 */
void _1990_groupe_free_element_courant(LIST *elements)
{
    Element     *element;
    
    if ((elements == NULL) || (list_curr(elements) == NULL))
        return;
    
    element = list_curr(elements);
    
    if (element->pIter != NULL)
        free(element->pIter);
    free(list_remove_curr(elements));
    
    return;
}

/* _1990_groupe_free_element_numero
 * Description : Libère l'élément spécifié par les paramètres
 * Paramètres : Projet *projet : variable projet
 *            : int niveau : le numéro du niveau contenant l'élément
 *            : int groupe : le numéro du groupe contenant l'élément
 *            : int element : le numéro de l'élément
 * Valeur renvoyée : Aucune
 */
void _1990_groupe_free_element_numero(Projet *projet, int niveau, int groupe, int element)
{
    Niveau_Groupe   *niveau_groupe;
    Groupe      *groupe_en_cours;
    
    if ((projet == NULL) || (projet->niveaux_groupes == NULL) || (list_size(projet->niveaux_groupes) == 0))
        return;
    if (_1990_groupe_niveau_positionne(projet->niveaux_groupes, niveau) != 0)
        return;
    niveau_groupe = list_curr(projet->niveaux_groupes);
    if ((niveau_groupe->groupes == NULL) || (list_size(niveau_groupe->groupes) == 0))
        return;
    if (_1990_groupe_positionne(niveau_groupe, groupe) != 0)
        return;
    groupe_en_cours = list_curr(niveau_groupe->groupes);
    if ((groupe_en_cours->elements == NULL) || (list_size(groupe_en_cours->elements) == 0))
        return;
    if (_1990_groupe_element_positionne(groupe_en_cours, element) != 0)
        return;
    _1990_groupe_free_element_courant(groupe_en_cours->elements);
    
    return;
}

/* _1990_groupe_free_niveau_numero
 * Description : Libère le niveau en cours ainsi que tous les niveaux supérieurs
 * Paramètres : Projet *projet : la variable projet
 *            : numéro du niveau à libérer
 * Valeur renvoyée : Aucune
 */
void _1990_groupe_free_niveau_numero(Projet *projet, int numero)
{
    Niveau_Groupe   *niveau_groupe;
    
    if (_1990_groupe_niveau_positionne(projet->niveaux_groupes, numero) != 0)
        return;
    
    niveau_groupe = list_curr(projet->niveaux_groupes);
    
    do
    {
        if (niveau_groupe->niveau >= numero)
        {
            // Il aurait pu être possible d'utiliser la fonction _1990_groupe_free_groupe_numero
            // mais cette dernier analyse également le niveau supérieur pour supprimer les 
            // références devenues obsolète
            if (niveau_groupe->pIter != NULL)
                free(niveau_groupe->pIter);
            
            if (niveau_groupe->groupes != NULL)
            {
                list_mvfront(niveau_groupe->groupes);
                while (!list_empty(niveau_groupe->groupes))
                {
                    Groupe      *groupe = list_curr(niveau_groupe->groupes);
                    if (groupe->pIter != NULL)
                        free(groupe->pIter);
                    
                    // On libère tous les éléments contenus dans le groupe
                    if (groupe->elements != NULL)
                    {
                        list_mvfront(groupe->elements);
                        while (!list_empty(groupe->elements))
                        {
                            Element     *element = list_front(groupe->elements);
                            
                            free(element->pIter);
                            free(list_remove_front(groupe->elements));
                        }
                        free(groupe->elements);
                    }
                    
                    // On libère toutes les combinaisons temporaires
                    if (groupe->tmp_combinaison.combinaisons != NULL)
                    {
                        list_mvfront(groupe->tmp_combinaison.combinaisons);
                        while (!list_empty(groupe->tmp_combinaison.combinaisons))
                        {
                            Combinaison *combinaison = list_front(groupe->tmp_combinaison.combinaisons);
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
                    
                    // Et enfin, on supprime l'élément courant
                    free(list_remove_curr(niveau_groupe->groupes));
                }
                free(niveau_groupe->groupes);
            }
            
            free(list_remove_curr(projet->niveaux_groupes));
            
            niveau_groupe = list_curr(projet->niveaux_groupes);
        }
        else
        {
            list_mvnext(projet->niveaux_groupes);
            niveau_groupe = list_curr(projet->niveaux_groupes);
        }
    }
    while ((list_size(projet->niveaux_groupes) != 0) && (niveau_groupe != NULL) && (niveau_groupe->niveau >= numero));
    
    return;
}

/* _1990_groupe_free_numero
 * Description : Libère le groupe demandé
 * Paramètres : Projet *projet : la variable projet
 *            : int niveau : niveau contenant le groupe
 *            : int numero : numéro du groupe à libérer
 * Valeur renvoyée : Aucune
 */
void _1990_groupe_free_numero(Projet *projet, int niveau, int numero)
{
    Niveau_Groupe   *niveau_groupe;
    Groupe      *groupe;
    
    if ((projet->niveaux_groupes == NULL) || (list_size(projet->niveaux_groupes) == 0))
        return;
    if (_1990_groupe_niveau_positionne(projet->niveaux_groupes, niveau) != 0)
        return;
    niveau_groupe = list_curr(projet->niveaux_groupes);
    if ((niveau_groupe->groupes == NULL) || (list_size(niveau_groupe->groupes) == 0))
        return;
    if (_1990_groupe_positionne(niveau_groupe, numero) != 0)
        return;
    
    groupe = list_curr(niveau_groupe->groupes);
    
    if (groupe->pIter != NULL)
        free(groupe->pIter);
    
    // On libère tous les éléments contenus dans le groupe
    if (groupe->elements != NULL)
    {
        list_mvfront(groupe->elements);
        while (!list_empty(groupe->elements))
            _1990_groupe_free_element_courant(groupe->elements);
        free(groupe->elements);
    }
    
    // On libère toutes les combinaisons temporaires
    if (groupe->tmp_combinaison.combinaisons != NULL)
    {
        list_mvfront(groupe->tmp_combinaison.combinaisons);
        while (!list_empty(groupe->tmp_combinaison.combinaisons))
        {
            Combinaison *combinaison = list_front(groupe->tmp_combinaison.combinaisons);
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
    
    // Et enfin, on supprime l'élément courant
    free(list_remove_curr(niveau_groupe->groupes));
    
    // Après la suppression du groupe, on décale d'un numéro tous les groupes supérieurs
    // afin de ne pas avoir de séparation entre les numéros
    if (list_size(niveau_groupe->groupes) != 0)
    {
        list_mvfront(niveau_groupe->groupes);
        do
        {
            groupe = list_curr(niveau_groupe->groupes);
            if (groupe->numero > numero)
                groupe->numero--;
        }
        while (list_mvnext(niveau_groupe->groupes));
    }
    
    // On passe au niveau suivant (s'il existe)
    if (list_mvnext(projet->niveaux_groupes) != NULL)
    {
        niveau_groupe = list_curr(projet->niveaux_groupes);
        if ((niveau_groupe->groupes != NULL) && (list_size(niveau_groupe->groupes) != 0))
        {
            list_mvfront(niveau_groupe->groupes);
            do
            {
                // On parcours tous les groupes pour vérifier si l'un possède l'élément
                // qui a été supprimé
                // On ne s'arrête pas volontairement au premier élément qu'on trouve.
                // Il est possible que quelqu'un trouve utile de pouvoir insérer un même
                // élément dans plusieurs groupes
                groupe = list_curr(niveau_groupe->groupes);
                if ((groupe->elements != NULL) && (list_size(groupe->elements) != 0))
                {
                    list_mvfront(groupe->elements);
                    do
                    {
                        int     dernier = 0;
                        Element     *element = list_curr(groupe->elements);
                        
                        if (element->numero == numero)
                        {
                            if (element == list_rear(groupe->elements))
                                dernier = 1;
                            free(list_remove_curr(groupe->elements));
                            element = list_curr(groupe->elements);
                        }
                        if ((dernier == 0) && (element->numero > numero))
                            element->numero--;
                    }
                    while (list_mvnext(groupe->elements) != NULL);
                }
            }
            while (list_mvnext(niveau_groupe->groupes));
        }
    }
    
    return;
}

/* _1990_groupe_free
 * Description : Libère l'ensemble des groupes et niveaux
 * Paramètres : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
void _1990_groupe_free(Projet *projet)
{
    if ((projet == NULL) || (projet->niveaux_groupes == NULL))
        return;
    
    // On supprime tous les niveaux
    _1990_groupe_free_niveau_numero(projet, 0);
    
    // Et on libère la liste
    free(projet->niveaux_groupes);
    projet->niveaux_groupes = NULL;
    return;
}
