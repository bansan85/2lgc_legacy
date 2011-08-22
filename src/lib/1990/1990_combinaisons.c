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
#include <list.h>
#include <stdlib.h>
#include <stdio.h>

#include "common_projet.h"
#include "common_erreurs.h"
#include "common_maths.h"

#include "1990_actions.h"
#include "1990_groupes.h"
#include "1990_combinaisons.h"
#include "1990_ponderations.h"


int _1990_combinaisons_verifie_double(LIST *liste_combinaisons, Combinaison *comb_a_verifier)
/* Description : Vérifie si une combinaison est déjà présente dans une liste de combinaisons.
 *               FONCTION INTERNE.
 * Paramètres : LIST *combinaisons : liste de combinaisons
 *            : Combinaison *comb_a_verifier : combinaison à vérifier
 * Valeur renvoyée :
 *   Succès : 0 si la combinaison n'est pas présente
 *          : 1 si la combinaison est présente
 *          : 1 si la combinaison est vide (afin de ne pas l'ajouter)
 *   Échec : -1 en cas de paramètres invalides :
 *             (liste_combinaisons == NULL) ou
 *             (comb_a_verifier == NULL)
 */
{
    BUGMSG(liste_combinaisons, -1, "_1990_combinaisons_verifie_double\n");
    BUGMSG(comb_a_verifier, -1, "_1990_combinaisons_verifie_double\n");
    
    // NE PAS INSÉRER
    if (list_size(liste_combinaisons) == 0)
        return 0;
    if (list_size(comb_a_verifier->elements) == 0)
        return 1;
    
    list_mvfront(liste_combinaisons);
    do
    {
        int         doublon = 1;
        Combinaison *comb_en_cours = (Combinaison*)list_curr(liste_combinaisons);
        
        /* On vérifie si la combinaison en cours possède le même nombre d'éléments que la combinaison à vérifier */
        if (list_size(comb_en_cours->elements) == list_size(comb_a_verifier->elements))
        {
            list_mvfront(comb_en_cours->elements);
            list_mvfront(comb_a_verifier->elements);
            do
            {
                Combinaison_Element *elem1 = list_curr(comb_en_cours->elements);
                Combinaison_Element *elem2 = list_curr(comb_a_verifier->elements);
                /* On vérifie que chaque élément pointe vers la même action
                 *  et que les flags (paramètres de calculs) sont les mêmes */
                if ((elem1->action != elem2->action) || (elem1->flags != elem2->flags))
                    doublon = 0;
            }
            while ((list_mvnext(comb_en_cours->elements) != NULL) && (list_mvnext(comb_a_verifier->elements) != NULL) && (doublon == 1));
            if (doublon == 1)
                return 1;
        }
    }
    while (list_mvnext(liste_combinaisons) != NULL);
    
    return 0;
}

int _1990_combinaisons_duplique(LIST *liste_comb_destination, LIST *liste_comb_source,
  int sans_double)
/* Description : ajoute à une liste de combinaisons existante une liste de combinaisons.
 *                 FONCTION INTERNE.
 * Paramètres : LIST *liste_comb_destination : liste de combinaisons qui recevra les
 *                combinaisons sources
 *            : LIST *liste_comb_source : liste de combinaisons source
 *            : int sans_double : TRUE pour qu'aucune combinaison ne soit ajouté en double.
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (liste_comb_destination == NULL) ou
 *             (liste_comb_source == NULL)
 *           -2 en cas d'erreur d'allocation mémoire
 */
{
    BUGMSG(liste_comb_destination, -1, "_1990_combinaisons_duplique\n");
    BUGMSG(liste_comb_source, -1, "_1990_combinaisons_duplique\n");
    
    // NE PAS INSÉRER
    if (list_size(liste_comb_source) == 0)
        return 0;
    
    list_mvrear(liste_comb_destination);
    list_mvfront(liste_comb_source);
    do
    {
        Combinaison *combinaison_source = list_curr(liste_comb_source);
        int         verifie_double;
        if (sans_double == TRUE)
            verifie_double = _1990_combinaisons_verifie_double(liste_comb_destination, combinaison_source);
        else
            verifie_double = 0;
        
        switch (verifie_double)
        {
            /* On l'ajoute donc */
            case 0 :
            {
                Combinaison combinaison_destination;
                
                /* Duplication de la combinaison */
                combinaison_destination.elements = list_init();
                BUGMSG(combinaison_destination.elements, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "_1990_combinaisons_duplique");
                if ((combinaison_source != NULL) && (list_curr(combinaison_source->elements) != NULL))
                {
                    list_mvfront(combinaison_source->elements);
                    do
                    {
                        Combinaison_Element *element_source = list_curr(combinaison_source->elements);
                        Combinaison_Element element_destination;
                        
                        element_destination.action = element_source->action;
                        element_destination.flags = element_source->flags;
                        BUGMSG(list_insert_after(combinaison_destination.elements, (void*)&element_destination, sizeof(element_destination)), -2, gettext("%s : Erreur d'allocation mémoire.\n"), "_1990_combinaisons_duplique");
                    }
                    while (list_mvnext(combinaison_source->elements) != NULL);
                }
                /* Insertion de la combinaison dans liste_comb_destination */
                BUGMSG(list_insert_after(liste_comb_destination, (void*)&combinaison_destination, sizeof(combinaison_destination)), -2, gettext("%s : Erreur d'allocation mémoire.\n"), "_1990_combinaisons_duplique");
                
                break;
            }
            /* On ne l'ajoute pas */
            case 1 :
            {
                break;
            }
            /* BUG */
            default :
            {
                BUG(0, -5);
            }
        }
    }
    while (list_mvnext(liste_comb_source) != NULL);
    
    return 0;
}

int _1990_combinaisons_action_predominante(Combinaison *combinaison, Type_Pays pays)
/* Description : modifie le flag de toutes les actions variables d'une combinaison afin de les
 *                 considérer comme action prédominante.
 *                 FONCTION INTERNE.
 * Paramètres : Combinaison *combinaison : combinaison à modifier
 *            : Type_Pays pays : le pays
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (combinaison == NULL)
 */
{
    BUGMSG(combinaison, -1, "_1990_combinaisons_action_predominante\n");
    // NE PAS INSÉRER
    if (list_size(combinaison->elements) == 0)
        return 0;
    
    list_mvfront(combinaison->elements);
    do
    {
        Combinaison_Element *combinaison_element = list_curr(combinaison->elements);
        if (_1990_action_categorie_bat(combinaison_element->action->type, pays) == ACTION_VARIABLE)
            combinaison_element->flags = 1;
    }
    while (list_mvnext(combinaison->elements));
    
    return 0;
}

int _1990_combinaisons_genere_xor(Projet *projet)
/* Description : Génère toutes les combinaisons d'un groupe possédant comme type de combinaison
 *                 GROUPE_COMBINAISON_XOR. Il convient auparavant que l'élément courant dans la
 *                 liste niveaux_groupes soit correctement positionné et que le groupe dans
 *                 l'élément de niveaux_groupes soit également correctement positionné.
 *               FONCTION INTERNE.
 * Paramètres : Projet  *projet : la variable projet
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet == NULL) ou
 *             (projet->niveaux_groupes == NULL) ou
 *             (list_size(projet->niveaux_groupes) == 0) ou
 *             (niveau->groupes == NULL)
 *               avec niveau = list_curr(projet->niveaux_groupes); ou
 *             (list_size(niveau->groupes) == 0) ou
 *             (groupe->type_combinaison != GROUPE_COMBINAISON_XOR)
 *               avec groupe = list_curr(niveau->groupes);
 *           -2 en cas d'erreur d'allocation mémoire
 *           -3 en cas d'erreur due à une fonction interne
 */
{
    Groupe              *groupe;
    Niveau_Groupe       *niveau;
    
    BUGMSG(projet, -1, "_1990_combinaisons_genere_xor\n");
    BUGMSG(projet->niveaux_groupes, -1, "_1990_combinaisons_genere_xor\n");
    BUGMSG(list_size(projet->niveaux_groupes), -1, "_1990_combinaisons_genere_xor\n");
    
    niveau = list_curr(projet->niveaux_groupes);
    
    BUGMSG(niveau->groupes, -1, "_1990_combinaisons_genere_xor\n");
    BUGMSG(list_size(niveau->groupes), -1, "_1990_combinaisons_genere_xor\n");
    
    groupe = list_curr(niveau->groupes);
    BUGMSG(groupe->type_combinaison == GROUPE_COMBINAISON_XOR, -1, "_1990_combinaisons_genere_xor\n");
    
    // Si le nombre d'éléments est nul Alors
    //     Fin.
    // FinSi
    if (list_empty(groupe->elements) == TRUE)
        return 0;
    
    list_mvfront(groupe->elements);
    
    // Si l'élément courant de niveaux_groupes est le premier de la liste Alors
    //     L'opération XOR consiste à ajouter les actions listées dans le groupe courant du
    //       premier élément de niveaux_groupes à la suite des autres sans aucune combinaison
    //       entre elles. On crée donc autant de combinaisons contenant une seule action qu'il
    //       y a d'éléments.
    if (list_curr(projet->niveaux_groupes) == list_front(projet->niveaux_groupes))
    {
        
        do
        {
            Element         *element_en_cours;
            Combinaison     nouvelle_combinaison;
            Combinaison_Element nouveau_element;
            
            element_en_cours = (Element*)list_curr(groupe->elements);
            BUG(_1990_action_cherche_numero(projet, element_en_cours->numero) == 0, -3);
            nouvelle_combinaison.elements = list_init();
            BUGMSG(nouvelle_combinaison.elements, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "_1990_combinaisons_genere_xor");
            nouveau_element.action = list_curr(projet->actions);
            nouveau_element.flags = nouveau_element.action->flags;
            BUGMSG(list_insert_after(nouvelle_combinaison.elements, (void*)&nouveau_element, sizeof(nouveau_element)), -2, gettext("%s : Erreur d'allocation mémoire.\n"), "_1990_combinaisons_genere_xor");
            BUGMSG(list_insert_after(groupe->tmp_combinaison.combinaisons, &(nouvelle_combinaison), sizeof(nouvelle_combinaison)), -2, gettext("%s : Erreur d'allocation mémoire.\n"), "_1990_combinaisons_genere_xor");
        }
        while (list_mvnext(groupe->elements) != NULL);
    }
    // Sinon
    //     Un XOR pour les autres étages consiste à ajouter toutes les combinaisons des groupes
    //       de l'étage n-1 qui sont indiquées dans le groupe de l'étage n.
    else
    {
        list_mvprev(projet->niveaux_groupes);
        niveau = list_curr(projet->niveaux_groupes);
        do
        {
            Element     *element_tmp = (Element*)list_curr(groupe->elements);
            Groupe      *groupe_n_1;
            
            BUG(_1990_groupe_positionne_groupe(niveau, element_tmp->numero) == 0, -3);
            groupe_n_1 = list_curr(niveau->groupes);
            BUGMSG(_1990_combinaisons_duplique(groupe->tmp_combinaison.combinaisons, groupe_n_1->tmp_combinaison.combinaisons, TRUE) == 0, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "_1990_combinaisons_genere_xor");
        }
        while (list_mvnext(groupe->elements) != NULL);
        
        list_mvnext(projet->niveaux_groupes);
    }
    // FinSi
    
    return 0;
}

int _1990_combinaisons_fusion(Combinaison *destination, Combinaison *source)
/* Description : fusionne deux combinaisons. Les éléments de la combinaison source sont
 *                 ajoutée à la fin de la combinaison destination.
 *               FONCTION INTERNE.
 * Paramètres : Combinaison *destination : combinaison de destination
 *            : Combinaison *source : combinaison source
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (destination == NULL) ou
 *             (source == NULL) ou
 *             (destination->elements == NULL) ou
 *             (source->elements == NULL)
 *           -2 en cas d'erreur d'allocation mémoire
 */
{
    BUGMSG(destination, -1, "_1990_combinaisons_fusion\n");
    BUGMSG(source, -1, "_1990_combinaisons_fusion\n");
    BUGMSG(destination->elements, -1, "_1990_combinaisons_fusion\n");
    BUGMSG(source->elements, -1, "_1990_combinaisons_fusion\n");
    
    // NE PAS INSÉRER
    if (list_size(source->elements) == 0)
        return 0;
    
    list_mvrear(destination->elements);
    list_mvfront(source->elements);
    do
    {
        Combinaison_Element *element_source = list_curr(source->elements);
        Combinaison_Element element_destination;
        
        element_destination.action = element_source->action;
        element_destination.flags = element_source->flags;
        BUGMSG(list_insert_after(destination->elements, (void*)&element_destination, sizeof(element_destination)), -2, gettext("%s : Erreur d'allocation mémoire.\n"), "_1990_combinaisons_fusion");
    }
    while (list_mvnext(source->elements) != NULL);
    
    return 0;
}

void _1990_groupe_free_groupe_tmp_combinaison(void *data)
/* Description : Permet de supprimer toutes les combinaisons temporaires contenues dans les
 *                 groupes. À utiliser avec la fonction list_traverse
 *               FONCTION INTERNE.
 * Paramètres : void *data : donnée à libérer
 * Valeur renvoyée : Aucune
 */
{
    // NE PAS INSÉRER
    Combinaison *combinaison = (Combinaison*)data;
    if (combinaison->elements != NULL)
        list_free(combinaison->elements, LIST_DEALLOC);
    free(data);
    return;
}

int _1990_combinaisons_genere_and(Projet *projet)
/* Description : Génère toutes les combinaisons d'un groupe possédant comme type de combinaison
 *                 GROUPE_COMBINAISON_AND. Il convient auparavant que l'élément courant dans la
 *                 liste niveaux_groupes soit correctement positionné et que le groupe dans
 *                 l'élément de niveaux_groupes soit également correctement positionné.
 *               FONCTION INTERNE.
 * Paramètres : Projet  *projet : la variable projet
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet == NULL) ou
 *             (projet->niveaux_groupes == NULL) ou
 *             (list_size(projet->niveaux_groupes) == 0) ou
 *             (niveau->groupes == NULL)
 *               avec niveau = list_curr(projet->niveaux_groupes); ou
 *             (list_size(niveau->groupes) == 0) ou
 *             (groupe->type_combinaison != GROUPE_COMBINAISON_AND)
 *               avec groupe = list_curr(niveau->groupes);
 *           -2 en cas d'erreur d'allocation mémoire
 *           -3 en cas d'erreur due à une fonction interne
 */
{
    Niveau_Groupe       *niveau;
    Groupe              *groupe_n_1, *groupe;
    
    BUGMSG(projet, -1, "_1990_combinaisons_genere_and\n");
    BUGMSG(projet->niveaux_groupes, -1, "_1990_combinaisons_genere_and\n");
    BUGMSG(list_size(projet->niveaux_groupes), -1, "_1990_combinaisons_genere_and\n");
    
    niveau = list_curr(projet->niveaux_groupes);
    
    BUGMSG(niveau->groupes, -1, "_1990_combinaisons_genere_and\n");
    BUGMSG(list_size(niveau->groupes), -1, "_1990_combinaisons_genere_and\n");
    
    groupe = list_curr(niveau->groupes);
    BUGMSG(groupe->type_combinaison == GROUPE_COMBINAISON_AND, -1, "_1990_combinaisons_genere_and\n");
    
    if (list_empty(groupe->elements) == TRUE)
        return 0;
    
    // Si l'élément courant de niveaux_groupes est le premier de la liste Alors
    //     L'opération AND consiste à créer une combinaison contenant l'ensemble des actions
    //       listées dans le groupe courant du premier élément de niveaux_groupes. Si une seule
    //       des actions variables est prédominante, par l'action de l'opérateur AND, toutes
    //       les actions variables deviennent prédominantes.
    if (list_curr(projet->niveaux_groupes) == list_front(projet->niveaux_groupes))
    {
        Combinaison comb;
        int         action_predominante = 0;
        
        comb.elements = list_init();
        BUGMSG(comb.elements, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "_1990_combinaisons_genere_and");
        
        list_mvfront(groupe->elements);
        do
        {
            Element             *element_en_cours = (Element*)list_curr(groupe->elements);
            Combinaison_Element nouveau_element;
            
            BUG(_1990_action_cherche_numero(projet, element_en_cours->numero) == 0, -3);
            nouveau_element.action = list_curr(projet->actions);
            nouveau_element.flags = nouveau_element.action->flags;
            if ((nouveau_element.flags & 1) != 0)
                action_predominante = 1;
            BUGMSG(list_insert_after(comb.elements, (void*)&nouveau_element, sizeof(nouveau_element)), -2, gettext("%s : Erreur d'allocation mémoire.\n"), "_1990_combinaisons_genere_and");
        }
        while (list_mvnext(groupe->elements) != NULL);
        
        if (action_predominante == 1)
            _1990_combinaisons_action_predominante(&(comb), projet->pays);
        BUGMSG(list_insert_after(groupe->tmp_combinaison.combinaisons, &(comb), sizeof(comb)), -2, gettext("%s : Erreur d'allocation mémoire.\n"), "_1990_combinaisons_genere_and");
    }
    // Sinon
    //     La génération consiste à créer un nombre de combinaisons égal au produit du nombre
    //       de combinaisons de chaque élément du groupe. Par exemple, s'il y a trois groupes
    //       contenant respectivement 1, 2 et 3 éléments, il y aura ainsi 1*2*3 combinaisons
    //       différentes. Dans le cas où l'un des groupes ne possède pas d'élément, le nombre
    //       de combinaisons généré sera alors nul. Si une des actions variables est
    //       prédominante, cela n'a aucune influence sur les autres actions variables avec 
    //       lesquelles elle peut être combinée.
    //     Les combinaisons sont réalisées de la façon suivante :
    //       - Première passe : on ajoute l'ensemble des combinaisons du premier groupe.
    //           soit (dans le cas de notre exemple) : 1
    //       - Deuxième passe (1) : on duplique l'ensemble des combinaisons du premier groupe
    //           autant de fois qu'il y a d'éléments dans le groupe 2.
    //           soit : 1, 1
    //       - Deuxième passe (2) : on ajoute à toutes les combinaisons créées les éléments du
    //           groupe 2.
    //           soit : 1 2_1, 1 2_2
    //       - Troisième passe : on recommence au niveau de la deuxième passe :
    //           soit : 1 2_1, 1 2_2, 1 2_1, 1 2_2, 1 2_1, 1 2_2
    //           soit : 1 2_1 3_1, 1 2_2 3_1, 1 2_1 3_2, 1 2_2 3_2, 1 2_1 3_3, 1 2_2 3_3
    else
    {
        LIST        *nouvelles_combinaisons;
        
        nouvelles_combinaisons = list_init();
        BUGMSG(nouvelles_combinaisons, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "_1990_combinaisons_genere_and");
        
        list_mvprev(projet->niveaux_groupes);
        niveau = list_curr(projet->niveaux_groupes);
        list_mvfront(groupe->elements);
        do
        {
            Element     *element_en_cours = (Element*)list_curr(groupe->elements);
            
            /* On se positionne sur l'élément en cours du groupe */
            BUG(_1990_groupe_positionne_groupe(niveau, element_en_cours->numero) == 0, -3);
            groupe_n_1 = list_curr(niveau->groupes);
            
            /* Alors, il s'agit de la première passe. On duplique donc simplement. */
            if (list_front(groupe->elements) == element_en_cours)
            {
                BUG(_1990_combinaisons_duplique(nouvelles_combinaisons, groupe_n_1->tmp_combinaison.combinaisons, FALSE) == 0, -3);
            }
            else
            {
                /* transition est utilisée de façon temporaire pour dupliquer nouvelles_combinaisons en cas de besoin */
                LIST        *transition = list_init();
                unsigned int    j;
                int     i, nbboucle;
                
                BUGMSG(transition, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "_1990_combinaisons_genere_and");
                
                /* On duplique les combinaisons actuellement dans nouvelles_combinaisons autant
                 * de fois (moins 1) qu'il y a d'éléments dans le groupe de la passe actuelle.
                 * (première partie de la passe 2) */
                BUG(_1990_combinaisons_duplique(transition, nouvelles_combinaisons, FALSE) == 0, -3);
                nbboucle = list_size(groupe_n_1->tmp_combinaison.combinaisons);
                for (i=2;i<=nbboucle;i++)
                    BUG(_1990_combinaisons_duplique(nouvelles_combinaisons, transition, FALSE) == 0, -12);
                
                /* On ajoute à la fin de toutes les combinaisons dupliquées les combinaisons
                 * contenues dans le groupe en cours (deuxième partie de la passe 2) */
                list_mvfront(nouvelles_combinaisons);
                list_mvfront(groupe_n_1->tmp_combinaison.combinaisons);
                for (i=1;i<=nbboucle;i++)
                {
                    Combinaison *combinaison2;
                    
                    combinaison2 = (Combinaison*)list_curr(groupe_n_1->tmp_combinaison.combinaisons);
                    for (j=1;j<=list_size(transition);j++)
                    {
                        Combinaison *combinaison1;
                        
                        combinaison1 = (Combinaison*)list_curr(nouvelles_combinaisons);
                        BUG(_1990_combinaisons_fusion(combinaison1, combinaison2) == 0, -3);
                        list_mvnext(nouvelles_combinaisons);
                    }
                    list_mvnext(groupe_n_1->tmp_combinaison.combinaisons);
                }
                
                list_free(transition, &(_1990_groupe_free_groupe_tmp_combinaison));
            }
        }
        while (list_mvnext(groupe->elements) != NULL);
        
        /* On ajoute définitivement les nouvelles combinaisons */
        BUG(_1990_combinaisons_duplique(groupe->tmp_combinaison.combinaisons, nouvelles_combinaisons, TRUE) == 0, -3);
        list_free(nouvelles_combinaisons, &(_1990_groupe_free_groupe_tmp_combinaison));
        
        list_mvnext(projet->niveaux_groupes);
    }
    // FinSi
    
    return 0;
}

int _1990_combinaisons_genere_or(Projet *projet)
/* Description : Génère toutes les combinaisons d'un groupe possédant comme type de combinaison
 *                 GROUPE_COMBINAISON_OR. Il convient auparavant que l'élément courant dans la
 *                 liste niveaux_groupes soit correctement positionné et que le groupe dans
 *                 l'élément de niveaux_groupes soit également correctement positionné.
 *               FONCTION INTERNE.
 * Paramètres : Projet  *projet : la variable projet
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet == NULL) ou
 *             (projet->niveaux_groupes == NULL) ou
 *             (list_size(projet->niveaux_groupes) == 0) ou
 *             (niveau->groupes == NULL)
 *               avec niveau = list_curr(projet->niveaux_groupes);
 *             (list_size(niveau->groupes) == 0) ou
 *             (groupe->type_combinaison != GROUPE_COMBINAISON_OR)
 *               avec groupe = list_curr(niveau->groupes);
 *           -2 en cas d'erreur d'allocation mémoire
 *           -3 en cas d'erreur due à une fonction interne
 */
{
    Niveau_Groupe       *niveau;
    Groupe          *groupe;
    Combinaison_Element element;
    unsigned int        boucle, i, j, k;
    
    BUGMSG(projet, -1, "_1990_combinaisons_genere_or\n");
    BUGMSG(projet->niveaux_groupes, -1, "_1990_combinaisons_genere_or\n");
    BUGMSG(list_size(projet->niveaux_groupes), -1, "_1990_combinaisons_genere_or\n");
    
    niveau = list_curr(projet->niveaux_groupes);
    
    BUGMSG(niveau->groupes, -1, "_1990_combinaisons_genere_or\n");
    BUGMSG(list_size(niveau->groupes), -1, "_1990_combinaisons_genere_or\n");
    
    groupe = list_curr(niveau->groupes);
    
    BUGMSG(groupe->type_combinaison == GROUPE_COMBINAISON_OR, -1, "_1990_combinaisons_genere_or\n");
    
    if (list_size(groupe->elements) == 0)
        return 0;
    
    boucle = 2;
    for (i=2;i<=list_size(groupe->elements);i++)
        boucle = boucle*2;
    
    // Si l'élément courant de niveaux_groupes est le premier de la liste Alors
    //     Afin de générer l'ensemble des combinaisons, il va être nécessaire de réaliser une
    //       boucle de 2^(list_size(groupe->elements)).
    //     Le principe consiste à générer toutes les combinaisons possibles avec une
    //       combinaison de type OR. Ainsi, dans le cas où il y a trois éléments dans un
    //       groupe, il est donc possible de générer les combinaisons suivantes :
    //           000 001 010 011 100 101 110 111 Soit bien 2^3 = 8 possibilités.
    //       Chaque bit correspondant à un élément.
    //       Lorsque le bit vaut 0, l'action n'est pas prise en compte dans la combinaison
    //       Lorsque le bit vaut 1, l'action est prise en compte dans la combinaison.
    if (list_curr(projet->niveaux_groupes) == list_front(projet->niveaux_groupes))
    {
        for (i=0;i<boucle;i++)
        {
            int         parcours_bits = i;
            int         action_predominante = 0;
            Combinaison nouvelle_combinaison;
            
            nouvelle_combinaison.elements = list_init();

            BUGMSG(nouvelle_combinaison.elements, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "_1990_combinaisons_genere_or");
            
            list_mvfront(groupe->elements);
            do
            {
                if ((parcours_bits & 1) == 1)
                {
                    Element     *element_en_cours = (Element*)list_curr(groupe->elements);
                    
                    BUG(_1990_action_cherche_numero(projet, element_en_cours->numero) == 0, -3);
                    element.action = list_curr(projet->actions);
                    element.flags = element.action->flags;
                    if ((element.flags & 1) != 0)
                        action_predominante = 1;
                    BUGMSG(list_insert_after(nouvelle_combinaison.elements, (void*)&element, sizeof(element)), -2, gettext("%s : Erreur d'allocation mémoire.\n"), "_1990_combinaisons_genere_or");
                }
                parcours_bits = parcours_bits >> 1;
                list_mvnext(groupe->elements);
            }
            while (parcours_bits != 0);
            
            if (action_predominante == 1)
                _1990_combinaisons_action_predominante(&(nouvelle_combinaison), projet->pays);
            BUGMSG(list_insert_after(groupe->tmp_combinaison.combinaisons, &(nouvelle_combinaison), sizeof(nouvelle_combinaison)), -2, gettext("%s : Erreur d'allocation mémoire.\n"), "_1990_combinaisons_genere_or");
        }
    }
    else
    {
    // Sinon
    //     Création d'une boucle pour générer 2^(list_size(groupe->elements)) combinaisons
    //     Pour chaque itération (i variant de 0 à nb_boucle-1)
    //         Pour chaque bit de i valant 1 (chaque bit de i représente si l'élément doit être
    //           pris en compte (1) ou non (0))
    //             Si c'est la première fois dans l'itération qu'un bit de i vaut 1 Alors
    //                 Duplication de l'ensemble des combinaisons contenu dans le groupe.
    //             Sinon
    //                 Duplication de l'ensemble des combinaisons déjà généré autant de fois
    //                   qu'il y a de combinaisons dans le groupe.
    //                 Fusion de chaque série de doublons avec une combinaison du groupe.
    //             FinSi
    //         FinPour
    //         Si une des actions variables est prédominante, cela n'a aucune influence sur les
    //           autres actions variables avec lesquelles elle peut être combinée.
    //     FinPour
    // FinSi
        list_mvprev(projet->niveaux_groupes);
        niveau = list_curr(projet->niveaux_groupes);
        for (i=0;i<boucle;i++)
        {
            int     parcours_bits = i;
            LIST    *nouvelles_combinaisons;
            
            nouvelles_combinaisons = list_init();
            BUGMSG(nouvelles_combinaisons, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "_1990_combinaisons_genere_or");
            
            list_mvfront(groupe->elements);
            do
            {
                if ((parcours_bits & 1) == 1)
                {
                    Element     *element_en_cours = (Element*)list_curr(groupe->elements);
                    Groupe      *groupe_n_1;
                    
                    BUG(_1990_groupe_positionne_groupe(niveau, element_en_cours->numero) == 0, -3);
                    groupe_n_1 = list_curr(niveau->groupes);
                    if (list_size(groupe_n_1->tmp_combinaison.combinaisons) != 0)
                    {
  /* Il s'agit de la première passe. On duplique donc simplement. */
                        if (list_size(nouvelles_combinaisons) == 0)
                        {
                            BUG(_1990_combinaisons_duplique(nouvelles_combinaisons, groupe_n_1->tmp_combinaison.combinaisons, FALSE) == 0, -3);
                        }
                        else
                        {
  /* transition est utilisée de façon temporaire pour dupliquer nouvelles_combinaisons en cas
   * de besoin */
                            LIST        *transition = list_init();
                            
                            BUGMSG(transition, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "_1990_combinaisons_genere_or");
                            
                            BUG(_1990_combinaisons_duplique(transition, nouvelles_combinaisons, FALSE) == 0, -3);
                            
  /* On duplique les combinaisons actuellement dans nouvelles_combinaisons autant de fois
   * (moins 1) qu'il y a d'éléments dans le groupe de la passe actuelle. */
                            for (j=2;j<=list_size(groupe_n_1->tmp_combinaison.combinaisons);j++)
                            {
                                BUG(_1990_combinaisons_duplique(nouvelles_combinaisons, transition, FALSE) == 0, -3);
                            }
                            
  /* Ensuite on fusionne chaque série de doublon créée avec une combinaison provenant de
   * groupe_n_1 */
                            list_mvfront(nouvelles_combinaisons);
                            list_mvfront(groupe_n_1->tmp_combinaison.combinaisons);
                            for (j=1;j<=list_size(groupe_n_1->tmp_combinaison.combinaisons);j++)
                            {
                                Combinaison *combinaison2 = (Combinaison*)list_curr(groupe_n_1->tmp_combinaison.combinaisons);
                                
                                for (k=1;k<=list_size(transition);k++)
                                {
                                    Combinaison *combinaison1 = (Combinaison*)list_curr(nouvelles_combinaisons);
                                    
                                    BUG(_1990_combinaisons_fusion(combinaison1, combinaison2) == 0, -3);
                                    list_mvnext(nouvelles_combinaisons);
                                }
                                list_mvnext(groupe_n_1->tmp_combinaison.combinaisons);
                            }
                            list_free(transition, &(_1990_groupe_free_groupe_tmp_combinaison));
                        }
                    }
                }
                parcours_bits = parcours_bits >> 1;
                list_mvnext(groupe->elements);
            }
            while (parcours_bits != 0);
            
            BUG(_1990_combinaisons_duplique(groupe->tmp_combinaison.combinaisons, nouvelles_combinaisons, TRUE) == 0, -15);
            list_free(nouvelles_combinaisons, &(_1990_groupe_free_groupe_tmp_combinaison));
        }
        list_mvnext(projet->niveaux_groupes);
    }
    
    return 0;
}

int _1990_combinaisons_init(Projet *projet)
/* Description : Initialise la mémoire pour les combinaisons à l'ELU et l'ELS
 * Paramètres : Projet  *projet : la variable projet
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet == NULL)
 *           -2 en cas d'erreur d'allocation mémoire
 */
{
    BUGMSG(projet, -1, "_1990_combinaisons_init\n");
    // Trivial
    
    projet->combinaisons.flags = 0;
    projet->combinaisons.elu_equ = list_init();
    BUGMSG(projet->combinaisons.elu_equ, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "_1990_combinaisons_init");
    projet->combinaisons.elu_str = list_init();
    BUGMSG(projet->combinaisons.elu_str, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "_1990_combinaisons_init");
    projet->combinaisons.elu_geo = list_init();
    BUGMSG(projet->combinaisons.elu_geo, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "_1990_combinaisons_init");
    projet->combinaisons.elu_fat = list_init();
    BUGMSG(projet->combinaisons.elu_fat, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "_1990_combinaisons_init");
    projet->combinaisons.elu_acc = list_init();
    BUGMSG(projet->combinaisons.elu_acc, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "_1990_combinaisons_init");
    projet->combinaisons.elu_sis = list_init();
    BUGMSG(projet->combinaisons.elu_sis, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "_1990_combinaisons_init");
    projet->combinaisons.els_car = list_init();
    BUGMSG(projet->combinaisons.els_car, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "_1990_combinaisons_init");
    projet->combinaisons.els_freq = list_init();
    BUGMSG(projet->combinaisons.els_freq, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "_1990_combinaisons_init");
    projet->combinaisons.els_perm = list_init();
    BUGMSG(projet->combinaisons.els_perm, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "_1990_combinaisons_init");
    
    return 0;
}

int _1990_combinaisons_empty(Projet *projet)
/* Description : libère l'ensemble des combinaisons à l'ELU et l'ELS sans libérer la liste
 *               FONCTION INTERNE.
 * Paramètres : Projet  *projet : la variable projet
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet == NULL) ou
 *             (projet->combinaisons.elu_equ == NULL) ou
 *             (projet->combinaisons.elu_str == NULL) ou
 *             (projet->combinaisons.elu_geo == NULL) ou
 *             (projet->combinaisons.elu_fat == NULL) ou
 *             (projet->combinaisons.elu_acc == NULL) ou
 *             (projet->combinaisons.elu_sis == NULL) ou
 *             (projet->combinaisons.els_car == NULL) ou
 *             (projet->combinaisons.els_freq == NULL) ou
 *             (projet->combinaisons.els_perm == NULL)
 */
{
    Ponderation *ponderation;
    
    BUGMSG(projet, -1, "_1990_combinaisons_empty\n");
    // NE PAS INSÉRER
    
    BUGMSG(projet->combinaisons.elu_equ, -1, "_1990_combinaisons_empty\n");
    list_mvfront(projet->combinaisons.elu_equ);
    while (!list_empty(projet->combinaisons.elu_equ))
    {
        ponderation = list_remove_front(projet->combinaisons.elu_equ);
        list_free(ponderation->elements, LIST_DEALLOC);
        free(ponderation);
    }
    BUGMSG(projet->combinaisons.elu_str, -1, "_1990_combinaisons_empty\n");
    list_mvfront(projet->combinaisons.elu_str);
    while (!list_empty(projet->combinaisons.elu_str))
    {
        ponderation = list_remove_front(projet->combinaisons.elu_str);
        list_free(ponderation->elements, LIST_DEALLOC);
        free(ponderation);
    }
    BUGMSG(projet->combinaisons.elu_geo, -1, "_1990_combinaisons_empty\n");
    list_mvfront(projet->combinaisons.elu_geo);
    while (!list_empty(projet->combinaisons.elu_geo))
    {
        ponderation = list_remove_front(projet->combinaisons.elu_geo);
        list_free(ponderation->elements, LIST_DEALLOC);
        free(ponderation);
    }
    BUGMSG(projet->combinaisons.elu_fat, -1, "_1990_combinaisons_empty\n");
    list_mvfront(projet->combinaisons.elu_fat);
    while (!list_empty(projet->combinaisons.elu_fat))
    {
        ponderation = list_remove_front(projet->combinaisons.elu_fat);
        list_free(ponderation->elements, LIST_DEALLOC);
        free(ponderation);
    }
    BUGMSG(projet->combinaisons.elu_acc, -1, "_1990_combinaisons_empty\n");
    list_mvfront(projet->combinaisons.elu_acc);
    while (!list_empty(projet->combinaisons.elu_acc))
    {
        ponderation = list_remove_front(projet->combinaisons.elu_acc);
        list_free(ponderation->elements, LIST_DEALLOC);
        free(ponderation);
    }
    BUGMSG(projet->combinaisons.elu_sis, -1, "_1990_combinaisons_empty\n");
    list_mvfront(projet->combinaisons.elu_sis);
    while (!list_empty(projet->combinaisons.elu_sis))
    {
        ponderation = list_remove_front(projet->combinaisons.elu_sis);
        list_free(ponderation->elements, LIST_DEALLOC);
        free(ponderation);
    }
    BUGMSG(projet->combinaisons.els_car, -1, "_1990_combinaisons_empty\n");
    list_mvfront(projet->combinaisons.els_car);
    while (!list_empty(projet->combinaisons.els_car))
    {
        ponderation = list_remove_front(projet->combinaisons.els_car);
        list_free(ponderation->elements, LIST_DEALLOC);
        free(ponderation);
    }
    BUGMSG(projet->combinaisons.els_freq, -1, "_1990_combinaisons_empty\n");
    list_mvfront(projet->combinaisons.els_freq);
    while (!list_empty(projet->combinaisons.els_freq))
    {
        ponderation = list_remove_front(projet->combinaisons.els_freq);
        list_free(ponderation->elements, LIST_DEALLOC);
        free(ponderation);
    }
    BUGMSG(projet->combinaisons.els_perm, -1, "_1990_combinaisons_empty\n");
    list_mvfront(projet->combinaisons.els_perm);
    while (!list_empty(projet->combinaisons.els_perm))
    {
        ponderation = list_remove_front(projet->combinaisons.els_perm);
        list_free(ponderation->elements, LIST_DEALLOC);
        free(ponderation);
    }
    return 0;
}

int _1990_combinaisons_free(Projet *projet)
/* Description : libère la mémoire pour les combinaisons à l'ELU et l'ELS
 * Paramètres : Projet  *projet : la variable projet
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet->combinaisons.elu_equ == NULL) ou
 *             (projet->combinaisons.elu_str == NULL) ou
 *             (projet->combinaisons.elu_geo == NULL) ou
 *             (projet->combinaisons.elu_fat == NULL) ou
 *             (projet->combinaisons.elu_acc == NULL) ou
 *             (projet->combinaisons.elu_sis == NULL) ou
 *             (projet->combinaisons.els_car == NULL) ou
 *             (projet->combinaisons.els_freq == NULL) ou
 *             (projet->combinaisons.els_perm == NULL)
 *           -3 en cas d'erreur due à une fonction interne
 */
{
    // Trivial
    BUGMSG(projet->combinaisons.elu_equ, -1, "EF_calculs_resoud_charge\n");
    BUGMSG(projet->combinaisons.elu_str, -1, "EF_calculs_resoud_charge\n");
    BUGMSG(projet->combinaisons.elu_geo, -1, "EF_calculs_resoud_charge\n");
    BUGMSG(projet->combinaisons.elu_fat, -1, "EF_calculs_resoud_charge\n");
    BUGMSG(projet->combinaisons.elu_acc, -1, "EF_calculs_resoud_charge\n");
    BUGMSG(projet->combinaisons.elu_sis, -1, "EF_calculs_resoud_charge\n");
    BUGMSG(projet->combinaisons.els_car, -1, "EF_calculs_resoud_charge\n");
    BUGMSG(projet->combinaisons.els_freq, -1, "EF_calculs_resoud_charge\n");
    BUGMSG(projet->combinaisons.els_perm, -1, "EF_calculs_resoud_charge\n");
    
    BUG(_1990_combinaisons_empty(projet) == 0, -3);
    free(projet->combinaisons.elu_equ);
    projet->combinaisons.elu_equ = NULL;
    free(projet->combinaisons.elu_str);
    projet->combinaisons.elu_str = NULL;
    free(projet->combinaisons.elu_geo);
    projet->combinaisons.elu_geo = NULL;
    free(projet->combinaisons.elu_fat);
    projet->combinaisons.elu_fat = NULL;
    free(projet->combinaisons.elu_acc);
    projet->combinaisons.elu_acc = NULL;
    free(projet->combinaisons.elu_sis);
    projet->combinaisons.elu_sis = NULL;
    free(projet->combinaisons.els_car);
    projet->combinaisons.els_car = NULL;
    free(projet->combinaisons.els_freq);
    projet->combinaisons.els_freq = NULL;
    free(projet->combinaisons.els_perm);
    projet->combinaisons.els_perm = NULL;
    
    return 0;
}

int _1990_combinaisons_genere(Projet *projet)
/* Description : Génère l'ensemble des combinaisons et pondérations du projet
 * Paramètres : Projet  *projet : la variable projet
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet == NULL) ou
 *             (projet->niveaux_groupes == NULL) ou
 *             (list_size(projet->niveaux_groupes) == 0)
 *           -2 en cas d'erreur d'allocation mémoire
 *           -3 en cas d'erreur due à une fonction interne
 */
{
    unsigned int        i;
    
    BUGMSG(projet, -1, "_1990_combinaisons_genere\n");
    BUGMSG(projet->niveaux_groupes, -1, "_1990_combinaisons_genere\n");
    BUGMSG(list_size(projet->niveaux_groupes), -1, "_1990_combinaisons_genere\n");
    
    BUG(_1990_combinaisons_empty(projet) == 0, -3);
    
    // Pour chaque action
    //     Cette boucle permet de générer toutes les combinaisons en prenant en compte le
    //       principe d'action prédominante. Ainsi, à chaque passage de la boucle, l'action
    //       numéro i est considérée comme action prédominante.
    for (i=0;i<list_size(projet->actions);i++)
    {
        Action          *action;
        unsigned int    j;
        
 /* On supprime les combinaisons temporaires générées lors du passage de la boucle précédente*/
        list_mvfront(projet->niveaux_groupes);
        do
        {
            Niveau_Groupe   *niveau = list_curr(projet->niveaux_groupes);
            
            list_mvfront(niveau->groupes);
            do
            {
                Groupe      *groupe = list_curr(niveau->groupes);
                
                if (groupe->tmp_combinaison.combinaisons != NULL)
                {
                    list_mvfront(groupe->tmp_combinaison.combinaisons);
                    while (!list_empty(groupe->tmp_combinaison.combinaisons))
                    {
                        Combinaison *combinaison = list_front(groupe->tmp_combinaison.combinaisons);
                        
                        list_free(combinaison->elements, LIST_DEALLOC);
                        free(list_remove_front(groupe->tmp_combinaison.combinaisons));
                    }
                }
            }
            while (list_mvnext(niveau->groupes) != NULL);
        }
        while (list_mvnext(projet->niveaux_groupes) != NULL);
        
    //     Attribution à l'action numéro i du flags&1 = 1 afin d'indiquer qu'il s'agit d'une
    //       action prédominante et mise à 0 du flags&1 pour les autres actions.
        list_mvfront(projet->actions);
        for (j=0;j<i;j++)
        {
            action = list_curr(projet->actions);
            action->flags = 0;
            list_mvnext(projet->actions);
        }
        action = list_curr(projet->actions);
        if (_1990_action_categorie_bat(action->type, projet->pays) == ACTION_VARIABLE)
            action->flags = 1;
        else
            action->flags = 0;
        list_mvnext(projet->actions);
        for (j=i+1;j<list_size(projet->actions);j++)
        {
            action = list_curr(projet->actions);
            action->flags = 0;
            list_mvnext(projet->actions);
        }
        
    //     Générer des combinaisons de toutes les groupes pour tous les niveaux :
    //     Pour chaque niveau de niveaux_groupes
    //         Pour chaque groupe du niveau en cours
    //             Génération des combinaisons avec l'action prédominante définie précédemment.
    //         FinPour
    //     FinPour
        list_mvfront(projet->niveaux_groupes);
        do
        {
            Niveau_Groupe   *niveau = (Niveau_Groupe*)list_curr(projet->niveaux_groupes);
            
            list_mvfront(niveau->groupes);
            if (list_curr(niveau->groupes) != NULL)
            {
                do
                {
                    Groupe  *groupe = (Groupe*)list_curr(niveau->groupes);
                    switch (groupe->type_combinaison)
                    {
                        case GROUPE_COMBINAISON_OR :
                        {
                            BUG(_1990_combinaisons_genere_or(projet) == 0, -2);
                            break;
                        }
                        case GROUPE_COMBINAISON_XOR :
                        {
                            BUG(_1990_combinaisons_genere_xor(projet) == 0, -3);
                            break;
                        }
                        case GROUPE_COMBINAISON_AND :
                        {
                            BUG(_1990_combinaisons_genere_and(projet) == 0, -4);
                            break;
                        }
                    }
                }
                while (list_mvnext(niveau->groupes));
            }
        }
        while (list_mvnext(projet->niveaux_groupes) != NULL);
        
    //     Génération des pondérations (avec les coefficients de sécurité partiels) à partir
    //       des combinaisons.
        _1990_ponderations_genere(projet);
    }
    // FinPour
    
    return 0;
}
