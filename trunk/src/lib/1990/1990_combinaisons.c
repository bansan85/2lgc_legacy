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
#include "1990_ponderations.h"


int _1990_combinaisons_verifie_double(GList *liste_combinaisons, Combinaison *comb_a_verifier)
/* Description : Vérifie si une combinaison est déjà présente dans une liste de combinaisons.
 *               FONCTION INTERNE.
 * Paramètres : GList *liste_combinaisons : liste de combinaisons,
 *            : Combinaison *comb_a_verifier : combinaison à vérifier.
 * Valeur renvoyée :
 *   Succès : 0 si la combinaison n'est pas présente,
 *          : 1 si la combinaison est présente,
 *          : 1 si la combinaison est vide (afin de ne pas l'ajouter).
 *   Échec : -1 :
 *             comb_a_verifier == NULL.
 */
{
    GList   *list_parcours;
    
    BUGMSG(comb_a_verifier, -1, gettext("Paramètre %s incorrect.\n"), "comb_a_verifier");
    
    if (liste_combinaisons == NULL)
        return 0;
    if (comb_a_verifier->elements == NULL)
        return 1;
    
    list_parcours = liste_combinaisons;
    do
    {
        int         doublon = 1;
        Combinaison *comb_en_cours = list_parcours->data;
        
        /* On vérifie si la combinaison en cours possède le même nombre d'éléments que la combinaison à vérifier */
        if (g_list_length(comb_en_cours->elements) == g_list_length(comb_a_verifier->elements))
        {
            GList   *list_1, *list_2;
            
            list_1 = comb_en_cours->elements;
            list_2 = comb_a_verifier->elements;
            
            do
            {
                Combinaison_Element *elem1 = list_1->data;
                Combinaison_Element *elem2 = list_2->data;
                /* On vérifie que chaque élément pointe vers la même action
                 *  et que les flags (paramètres de calculs) sont les mêmes */
                if ((elem1->action != elem2->action) || (elem1->flags != elem2->flags))
                    doublon = 0;
                
                list_1 = g_list_next(list_1);
                list_2 = g_list_next(list_2);
            }
            while ((list_1 != NULL) && (list_2 != NULL) && (doublon == 1));
            
            if (doublon == 1)
                return 1;
        }
        list_parcours = g_list_next(list_parcours);
    }
    while (list_parcours != NULL);
    
    return 0;
}


int _1990_combinaisons_duplique(GList **liste_comb_destination, GList *liste_comb_source,
  gboolean sans_double)
/* Description : ajoute à une liste de combinaisons existante une liste de combinaisons.
 *               FONCTION INTERNE.
 * Paramètres : GList **liste_comb_destination : liste de combinaisons qui recevra les
 *                combinaisons sources
 *            : GList *liste_comb_source : liste de combinaisons source
 *            : int sans_double : TRUE pour qu'aucune combinaison ne soit ajouté en double.
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 :
 *             liste_comb_destination == NULL,
 *             erreur d'allocation mémoire.
 */
{
    GList   *list_parcours;
    
    BUGMSG(liste_comb_destination, -1, gettext("Paramètre %s incorrect.\n"), "liste_comb_destination");
    
    if (liste_comb_source == NULL)
        return 0;
    
    list_parcours = liste_comb_source;
    do
    {
        Combinaison *combinaison_source = list_parcours->data;
        int         verifie_double;
        
        if (sans_double == TRUE)
            verifie_double = _1990_combinaisons_verifie_double(*liste_comb_destination, combinaison_source);
        else
            verifie_double = 0;
        
        switch (verifie_double)
        {
            /* On l'ajoute donc */
            case 0 :
            {
                Combinaison *combinaison_destination = malloc(sizeof(Combinaison));
                
                BUGMSG(combinaison_destination, -1, gettext("Erreur d'allocation mémoire.\n"));
                
                /* Duplication de la combinaison */
                combinaison_destination->elements = NULL;
                if (combinaison_source->elements != NULL)
                {
                    GList   *list_parcours2 = combinaison_source->elements;
                    
                    do
                    {
                        Combinaison_Element *element_source = list_parcours2->data;
                        Combinaison_Element *element_destination = malloc(sizeof(Combinaison_Element));
                        
                        BUGMSG(element_destination, -1, gettext("Erreur d'allocation mémoire.\n"));
                        
                        element_destination->action = element_source->action;
                        element_destination->flags = element_source->flags;
                        combinaison_destination->elements = g_list_append(combinaison_destination->elements, element_destination);
                        
                        list_parcours2 = g_list_next(list_parcours2);
                    }
                    while (list_parcours2 != NULL);
                }
                /* Insertion de la combinaison dans liste_comb_destination */
                *liste_comb_destination = g_list_append(*liste_comb_destination, combinaison_destination);
                
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
                BUG(0, -3);
            }
        }
        
        list_parcours = g_list_next(list_parcours);
    }
    while (list_parcours != NULL);
    
    return 0;
}


int _1990_combinaisons_action_predominante(Combinaison *combinaison, Type_Pays pays)
/* Description : modifie le flag de toutes les actions variables d'une combinaison afin de les
 *               considérer comme action prédominante.
 *               FONCTION INTERNE.
 * Paramètres : Combinaison *combinaison : combinaison à modifier,
 *            : Type_Pays pays : le pays.
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 :
 *             combinaison == NULL,
 *             _1990_action_categorie_bat.
 */
{
    GList   *list_parcours;
    
    BUGMSG(combinaison, -1, gettext("Paramètre %s incorrect.\n"), "combinaison");
    
    if (combinaison->elements == NULL)
        return 0;
    
    list_parcours = combinaison->elements;
    do
    {
        Combinaison_Element *combinaison_element = list_parcours->data;
        Action_Categorie    categorie = _1990_action_categorie_bat(combinaison_element->action->type, pays);
        
        BUG(categorie != ACTION_INCONNUE, -3);
        if (categorie == ACTION_VARIABLE)
            combinaison_element->flags = 1;
        
        list_parcours = g_list_next(list_parcours);
    }
    while (list_parcours != NULL);
    
    return 0;
}


int _1990_combinaisons_genere_xor(Projet *projet, Niveau_Groupe *niveau, Groupe *groupe)
/* Description : Génère toutes les combinaisons d'un groupe possédant comme type de combinaison
 *               GROUPE_COMBINAISON_XOR.
 *               FONCTION INTERNE.
 * Paramètres : Projet *projet : la variable projet,
 *              Niveau_Groupe *niveau : niveau du groupe à analyser,
 *              Groupe *groupe : groupe à analyser.
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 :
 *             projet == NULL,
 *             projet->niveaux_groupes == NULL,
 *             niveau == NULL,
 *             niveau->groupes == NULL,
 *             groupe->type_combinaison != GROUPE_COMBINAISON_XOR,
 *             erreur d'allocation mémoire,
 *             _1990_action_cherche_numero,
 *             _1990_groupe_positionne_groupe,
 *             _1990_combinaisons_duplique.
 */
{
    GList   *list_parcours;
    
    BUGMSG(projet, -1, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->niveaux_groupes, -1, gettext("Le projet ne possède pas de niveaux de groupes.\n"));
    BUGMSG(niveau, -1, gettext("Paramètre %s incorrect.\n"), "niveau");
    BUGMSG(niveau->groupes, -1, gettext("Le niveau %u est vide. Veuillez soit le remplir, soit le supprimer.\n"), niveau->numero);
    BUGMSG(groupe->type_combinaison == GROUPE_COMBINAISON_XOR, -1, gettext("Seuls les groupes possédant un type de combinaison XOR peuvent appeler _1990_combinaisons_genere_xor.\n"));
    
    // Si le nombre d'éléments est nul Alors
    //     Fin.
    // FinSi
    if (groupe->elements == NULL)
        return 0;
    
    list_parcours = groupe->elements;
    
    // Si l'élément courant de niveaux_groupes est le premier de la liste Alors
    //     L'opération XOR consiste à ajouter les actions listées dans le groupe courant du
    //       premier élément de niveaux_groupes à la suite des autres sans aucune combinaison
    //       entre elles. On crée donc autant de combinaisons contenant une seule action qu'il
    //       y a d'éléments.
    if (niveau == projet->niveaux_groupes->data)
    {
        
        do
        {
            Element             *element_en_cours;
            Combinaison         *nouvelle_combinaison = malloc(sizeof(Combinaison));
            Combinaison_Element *nouveau_element = malloc(sizeof(Combinaison_Element));
            
            BUGMSG(nouvelle_combinaison, -1, gettext("Erreur d'allocation mémoire.\n"));
            BUGMSG(nouveau_element, -1, gettext("Erreur d'allocation mémoire.\n"));
            element_en_cours = list_parcours->data;
            nouvelle_combinaison->elements = NULL;
            BUG(nouveau_element->action = _1990_action_cherche_numero(projet, element_en_cours->numero), -1);
            nouveau_element->flags = nouveau_element->action->flags;
            nouvelle_combinaison->elements = g_list_append(nouvelle_combinaison->elements, nouveau_element);
            groupe->tmp_combinaison.combinaisons = g_list_append(groupe->tmp_combinaison.combinaisons, nouvelle_combinaison);
            
            list_parcours = g_list_next(list_parcours);
        }
        while (list_parcours != NULL);
    }
    // Sinon
    //     Un XOR pour les autres étages consiste à ajouter toutes les combinaisons des groupes
    //       de l'étage n-1 qui sont indiquées dans le groupe de l'étage n.
    else
    {
        GList   *list_groupe_n_1;
        
        list_groupe_n_1 = g_list_find(projet->niveaux_groupes, niveau);
        BUGMSG(list_groupe_n_1, -1, gettext("Impossible de trouver le niveau %u dans la liste des niveaux de groupes.\n"), niveau->numero);
        list_groupe_n_1 = g_list_previous(list_groupe_n_1);
        BUGMSG(list_groupe_n_1, -1, gettext("Impossible de trouver le niveau précédent le niveau %u.\n"), niveau->numero);
        
        niveau = list_groupe_n_1->data;
        do
        {
            Element     *element_tmp = list_parcours->data;
            Groupe      *groupe_n_1;
            
            BUG(groupe_n_1 = _1990_groupe_positionne_groupe(niveau, element_tmp->numero), -1);
            BUG(_1990_combinaisons_duplique(&(groupe->tmp_combinaison.combinaisons), groupe_n_1->tmp_combinaison.combinaisons, TRUE) == 0, -1);
            
            list_parcours = g_list_next(list_parcours);
        }
        while (list_parcours != NULL);
    }
    // FinSi
    
    return 0;
}


int _1990_combinaisons_fusion(Combinaison *destination, Combinaison *source)
/* Description : Fusionne deux combinaisons. Les éléments de la combinaison source sont ajoutés
 *               à la fin de la combinaison destination.
 *               FONCTION INTERNE.
 * Paramètres : Combinaison *destination : combinaison de destination,
 *            : Combinaison *source : combinaison source.
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 :
 *             destination == NULL,
 *             source == NULL,
 *             erreur d'allocation mémoire.
 */
{
    GList   *list_parcours;
    
    BUGMSG(destination, -1, gettext("Paramètre %s incorrect.\n"), "destination");
    BUGMSG(source, -1, gettext("Paramètre %s incorrect.\n"), "source");
    
    list_parcours = source->elements;
    while (list_parcours != NULL)
    {
        Combinaison_Element *element_source = list_parcours->data;
        Combinaison_Element *element_destination = malloc(sizeof(Combinaison_Element));
        
        BUGMSG(element_destination, -1, gettext("Erreur d'allocation mémoire.\n"));
        element_destination->action = element_source->action;
        element_destination->flags = element_source->flags;
        destination->elements = g_list_append(destination->elements, element_destination);
        
        list_parcours = g_list_next(list_parcours);
    }
    
    return 0;
}


void _1990_groupe_free_groupe_tmp_combinaison(void *data)
/* Description : Permet de supprimer toutes les combinaisons temporaires contenues dans les
 *               groupes. À utiliser avec la fonction list_traverse.
 *               FONCTION INTERNE.
 * Paramètres : void *data : donnée à libérer.
 * Valeur renvoyée : void
 */
{
    Combinaison *combinaison = (Combinaison*)data;
    
    BUGMSG(data, , gettext("Paramètre %s incorrect.\n"), "data");
    
    if (combinaison->elements != NULL)
        g_list_free(combinaison->elements);
    free(data);
    
    return;
}


int _1990_combinaisons_genere_and(Projet *projet, Niveau_Groupe *niveau, Groupe *groupe)
/* Description : Génère toutes les combinaisons d'un groupe possédant comme type de combinaison
 *               GROUPE_COMBINAISON_AND.
 *               FONCTION INTERNE.
 * Paramètres : Projet *projet : la variable projet,
 *              Niveau_Groupe *niveau : niveau du groupe à analyser,
 *              Groupe *groupe : groupe à analyser.
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 :
 *             projet == NULL,
 *             projet->niveaux_groupes == NULL,
 *             niveau == NULL,
 *             niveau->groupes == NULL,
 *             groupe->type_combinaison != GROUPE_COMBINAISON_AND,
 *             erreur d'allocation mémoire,
 *             _1990_action_cherche_numero,
 *             _1990_combinaisons_action_predominante,
 *             _1990_groupe_positionne_groupe,
 *             _1990_combinaisons_duplique,
 *             _1990_combinaisons_fusion.
 */
{
    GList               *list_parcours;
    Groupe              *groupe_n_1;
    
    BUGMSG(projet, -1, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->niveaux_groupes, -1, gettext("Le projet ne possède pas de niveaux de groupes.\n"));
    BUGMSG(niveau, -1, gettext("Paramètre %s incorrect.\n"), "niveau");
    BUGMSG(niveau->groupes, -1, gettext("Le niveau %u est vide. Veuillez soit le remplir, soit le supprimer.\n"), niveau->numero);
    BUGMSG(groupe->type_combinaison == GROUPE_COMBINAISON_AND, -1, gettext("Seuls les groupes possédant un type de combinaison AND peuvent appeler _1990_combinaisons_genere_and.\n"));
    
    if (groupe->elements == NULL)
        return 0;
    
    list_parcours = groupe->elements;
    
    // Si l'élément courant de niveaux_groupes est le premier de la liste Alors
    //     L'opération AND consiste à créer une combinaison contenant l'ensemble des actions
    //       listées dans le groupe courant du premier élément de niveaux_groupes. Si une seule
    //       des actions variables est prédominante, par l'action de l'opérateur AND, toutes
    //       les actions variables deviennent prédominantes.
    if (niveau == projet->niveaux_groupes->data)
    {
        Combinaison *comb = malloc(sizeof(Combinaison));
        int         action_predominante = 0;
        
        BUGMSG(comb, -2, gettext("Erreur d'allocation mémoire.\n"));
        comb->elements = NULL;
        
        do
        {
            Element             *element_en_cours = list_parcours->data;
            Combinaison_Element *nouveau_element = malloc(sizeof(Combinaison_Element));
            
            BUGMSG(nouveau_element, -1, gettext("Erreur d'allocation mémoire.\n"));
            BUG(nouveau_element->action = _1990_action_cherche_numero(projet, element_en_cours->numero), -1);
            nouveau_element->flags = nouveau_element->action->flags;
            if ((nouveau_element->flags & 1) != 0)
                action_predominante = 1;
            comb->elements = g_list_append(comb->elements, nouveau_element);
            
            list_parcours = g_list_next(list_parcours);
        }
        while (list_parcours != NULL);
        
        if (action_predominante == 1)
            BUG(_1990_combinaisons_action_predominante(comb, projet->pays) == 0, -1);
        groupe->tmp_combinaison.combinaisons = g_list_append(groupe->tmp_combinaison.combinaisons, comb);
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
        GList   *list_groupe_n_1;
        GList   *nouvelles_combinaisons;
        
        nouvelles_combinaisons = NULL;
        
        list_groupe_n_1 = g_list_find(projet->niveaux_groupes, niveau);
        BUGMSG(list_groupe_n_1, -1, gettext("Impossible de trouver le niveau %u dans la liste des niveaux de groupes.\n"), niveau->numero);
        list_groupe_n_1 = g_list_previous(list_groupe_n_1);
        BUGMSG(list_groupe_n_1, -1, gettext("Impossible de trouver le niveau précédent le niveau %u.\n"), niveau->numero);
        niveau = list_groupe_n_1->data;
        
        do
        {
            Element     *element_en_cours = list_parcours->data;
            
            /* On se positionne sur l'élément en cours du groupe */
            BUG(groupe_n_1 = _1990_groupe_positionne_groupe(niveau, element_en_cours->numero), -1);
            
            /* Alors, il s'agit de la première passe. On duplique donc simplement. */
            if (groupe->elements->data == element_en_cours)
                BUG(_1990_combinaisons_duplique(&nouvelles_combinaisons, groupe_n_1->tmp_combinaison.combinaisons, FALSE) == 0, -1);
            else
            {
                /* transition est utilisée de façon temporaire pour dupliquer nouvelles_combinaisons en cas de besoin */
                GList           *transition = NULL;
                GList           *list_parcours2, *list_parcours3;
                unsigned int    j, i, nbboucle;
                
                /* On duplique les combinaisons actuellement dans nouvelles_combinaisons autant
                 * de fois (moins 1) qu'il y a d'éléments dans le groupe de la passe actuelle.
                 * (première partie de la passe 2) */
                BUG(_1990_combinaisons_duplique(&transition, nouvelles_combinaisons, FALSE) == 0, -1);
                nbboucle = g_list_length(groupe_n_1->tmp_combinaison.combinaisons);
                for (i=2;i<=nbboucle;i++)
                    BUG(_1990_combinaisons_duplique(&nouvelles_combinaisons, transition, FALSE) == 0, -1);
                
                /* On ajoute à la fin de toutes les combinaisons dupliquées les combinaisons
                 * contenues dans le groupe en cours (deuxième partie de la passe 2) */
                list_parcours2 = groupe_n_1->tmp_combinaison.combinaisons;
                list_parcours3 = nouvelles_combinaisons;
                for (i=1;i<=nbboucle;i++)
                {
                    Combinaison *combinaison2;
                    
                    combinaison2 = list_parcours2->data;
                    for (j=1;j<=g_list_length(transition);j++)
                    {
                        Combinaison *combinaison1;
                        
                        combinaison1 = list_parcours3->data;
                        BUG(_1990_combinaisons_fusion(combinaison1, combinaison2) == 0, -1);
                        list_parcours3 = g_list_next(list_parcours3);
                    }
                    
                    list_parcours2 = g_list_next(list_parcours2);
                }
                
                g_list_free_full(transition, &(_1990_groupe_free_groupe_tmp_combinaison));
            }
            
            list_parcours = g_list_next(list_parcours);
        }
        while (list_parcours != NULL);
        
        /* On ajoute définitivement les nouvelles combinaisons */
        BUG(_1990_combinaisons_duplique(&(groupe->tmp_combinaison.combinaisons), nouvelles_combinaisons, TRUE) == 0, -3);
        g_list_free_full(nouvelles_combinaisons, &_1990_groupe_free_groupe_tmp_combinaison);
    }
    // FinSi
    
    return 0;
}


int _1990_combinaisons_genere_or(Projet *projet, Niveau_Groupe *niveau, Groupe *groupe)
/* Description : Génère toutes les combinaisons d'un groupe possédant comme type de combinaison
 *               GROUPE_COMBINAISON_OR.
 *               FONCTION INTERNE.
 * Paramètres : Projet *projet : la variable projet,
 *              Niveau_Groupe *niveau : niveau du groupe à analyser,
 *              Groupe *groupe : groupe à analyser.
 * Valeur renvoyée :
 *   Succès : 0.
 *   Échec : -1 :
 *             projet == NULL,
 *             projet->niveaux_groupes == NULL,
 *             niveau,
 *             niveau->groupes == NULL,
 *             groupe->type_combinaison != GROUPE_COMBINAISON_OR,
 *             erreur d'allocation mémoire,
 *             _1990_action_cherche_numero,
 *             _1990_groupe_positionne_groupe,
 *             _1990_combinaisons_duplique,
 *             _1990_combinaisons_fusion.
 */
{
    GList               *list_parcours;
    unsigned int        boucle, i, j, k;
    
    BUGMSG(projet, -1, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->niveaux_groupes, -1, gettext("Le projet ne possède pas de niveaux de groupes.\n"));
    BUGMSG(niveau, -1, gettext("Paramètre %s incorrect.\n"), "niveau");
    BUGMSG(niveau->groupes, -1, gettext("Le niveau %u est vide. Veuillez soit le remplir, soit le supprimer.\n"), niveau->numero);
    BUGMSG(groupe->type_combinaison == GROUPE_COMBINAISON_OR, -1, gettext("Seuls les groupes possédant un type de combinaison OR peuvent appeler _1990_combinaisons_genere_or.\n"));
    
    if (groupe->elements == NULL)
        return 0;
    
    boucle = 2;
    for (i=2;i<=g_list_length(groupe->elements);i++)
        boucle = boucle*2;
    
    list_parcours = groupe->elements;
    
    // Si l'élément courant de niveaux_groupes est le premier de la liste Alors
    //     Afin de générer l'ensemble des combinaisons, il va être nécessaire de réaliser une
    //       boucle de 2^(g_list_length(groupe->elements)).
    //     Le principe consiste à générer toutes les combinaisons possibles avec une
    //       combinaison de type OR. Ainsi, dans le cas où il y a trois éléments dans un
    //       groupe, il est donc possible de générer les combinaisons suivantes :
    //           000 001 010 011 100 101 110 111 Soit bien 2^3 = 8 possibilités.
    //       Chaque bit correspondant à un élément.
    //       Lorsque le bit vaut 0, l'action n'est pas prise en compte dans la combinaison
    //       Lorsque le bit vaut 1, l'action est prise en compte dans la combinaison.
    if (niveau == projet->niveaux_groupes->data)
    {
        for (i=0;i<boucle;i++)
        {
            unsigned int    parcours_bits = i;
            int             action_predominante = 0;
            Combinaison     *nouvelle_combinaison = malloc(sizeof(Combinaison));
            
            BUGMSG(nouvelle_combinaison, -1, gettext("Erreur d'allocation mémoire.\n"));
            nouvelle_combinaison->elements = NULL;
            
            do
            {
                if ((parcours_bits & 1) == 1)
                {
                    Combinaison_Element *element = malloc(sizeof(Combinaison_Element));
                    Element             *element_en_cours = list_parcours->data;
                    
                    BUGMSG(element, -1, gettext("Erreur d'allocation mémoire.\n"));
                    BUG(element->action = _1990_action_cherche_numero(projet, element_en_cours->numero), -1);
                    element->flags = element->action->flags;
                    if ((element->flags & 1) != 0)
                        action_predominante = 1;
                    nouvelle_combinaison->elements = g_list_append(nouvelle_combinaison->elements, element);
                }
                parcours_bits = parcours_bits >> 1;
                list_parcours = g_list_next(list_parcours);
            }
            while (parcours_bits != 0);
            
            if (action_predominante == 1)
                BUG(_1990_combinaisons_action_predominante(nouvelle_combinaison, projet->pays) == 0, -1);
            groupe->tmp_combinaison.combinaisons = g_list_append(groupe->tmp_combinaison.combinaisons, nouvelle_combinaison);
        }
    }
    // Sinon
    //     Création d'une boucle pour générer 2^(g_list_length(groupe->elements)) combinaisons
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
    else
    {
        GList   *list_groupe_n_1;
        
        list_groupe_n_1 = g_list_find(projet->niveaux_groupes, niveau);
        BUGMSG(list_groupe_n_1, -1, gettext("Impossible de trouver le niveau %u dans la liste des niveaux de groupes.\n"), niveau->numero);
        list_groupe_n_1 = g_list_previous(list_groupe_n_1);
        BUGMSG(list_groupe_n_1, -1, gettext("Impossible de trouver le niveau précédent le niveau %u.\n"), niveau->numero);
        niveau = list_groupe_n_1->data;
        
        for (i=0;i<boucle;i++)
        {
            unsigned int    parcours_bits = i;
            GList           *nouvelles_combinaisons;
            
            nouvelles_combinaisons = NULL;
            
            list_parcours = groupe->elements;
            do
            {
                if ((parcours_bits & 1) == 1)
                {
                    Element     *element_en_cours = list_parcours->data;
                    Groupe      *groupe_n_1;
                    
                    BUG(groupe_n_1 = _1990_groupe_positionne_groupe(niveau, element_en_cours->numero), -1);
                    
                    if (groupe_n_1->tmp_combinaison.combinaisons != NULL)
                    {
  /* Il s'agit de la première passe. On duplique donc simplement. */
                        if (nouvelles_combinaisons == NULL)
                            BUG(_1990_combinaisons_duplique(&nouvelles_combinaisons, groupe_n_1->tmp_combinaison.combinaisons, FALSE) == 0, -1);
                        else
                        {
  /* transition est utilisée de façon temporaire pour dupliquer nouvelles_combinaisons en cas
   * de besoin */
                            GList   *transition = NULL;
                            GList   *list_parcours2, *list_parcours3;
                            
                            BUG(_1990_combinaisons_duplique(&transition, nouvelles_combinaisons, FALSE) == 0, -1);
                            
  /* On duplique les combinaisons actuellement dans nouvelles_combinaisons autant de fois
   * (moins 1) qu'il y a d'éléments dans le groupe de la passe actuelle. */
                            for (j=2;j<=g_list_length(groupe_n_1->tmp_combinaison.combinaisons);j++)
                                BUG(_1990_combinaisons_duplique(&nouvelles_combinaisons, transition, FALSE) == 0, -1);
                            
  /* Ensuite on fusionne chaque série de doublon créée avec une combinaison provenant de
   * groupe_n_1 */
                            list_parcours2 = groupe_n_1->tmp_combinaison.combinaisons;
                            list_parcours3 = nouvelles_combinaisons;
                            for (j=1;j<=g_list_length(groupe_n_1->tmp_combinaison.combinaisons);j++)
                            {
                                Combinaison *combinaison2 = list_parcours2->data;
                                
                                for (k=1;k<=g_list_length(transition);k++)
                                {
                                    Combinaison *combinaison1 = list_parcours3->data;
                                    
                                    BUG(_1990_combinaisons_fusion(combinaison1, combinaison2) == 0, -1);
                                    list_parcours3 = g_list_next(list_parcours3);
                                }
                                list_parcours2 = g_list_next(list_parcours2);
                            }
                            g_list_free_full(transition, &_1990_groupe_free_groupe_tmp_combinaison);
                        }
                    }
                }
                parcours_bits = parcours_bits >> 1;
                list_parcours = g_list_next(list_parcours);
            }
            while (parcours_bits != 0);
            
            BUG(_1990_combinaisons_duplique(&(groupe->tmp_combinaison.combinaisons), nouvelles_combinaisons, TRUE) == 0, -1);
            g_list_free_full(nouvelles_combinaisons, &_1990_groupe_free_groupe_tmp_combinaison);
        }
    }
    
    return 0;
}


G_MODULE_EXPORT int _1990_combinaisons_init(Projet *projet)
/* Description : Initialise la mémoire pour les combinaisons à l'ELU et l'ELS.
 * Paramètres : Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 :
 *             projet == NULL.
 */
{
    BUGMSG(projet, -1, gettext("Paramètre %s incorrect.\n"), "projet");
    // Trivial
    
    projet->combinaisons.flags = 0;
    projet->combinaisons.elu_equ = NULL;
    projet->combinaisons.elu_str = NULL;
    projet->combinaisons.elu_geo = NULL;
    projet->combinaisons.elu_fat = NULL;
    projet->combinaisons.elu_acc = NULL;
    projet->combinaisons.elu_sis = NULL;
    projet->combinaisons.els_car = NULL;
    projet->combinaisons.els_freq = NULL;
    projet->combinaisons.els_perm = NULL;
    
    return 0;
}


G_MODULE_EXPORT int _1990_combinaisons_free(Projet *projet)
/* Description : libère l'ensemble des combinaisons à l'ELU et l'ELS sans libérer la liste.
 *               FONCTION INTERNE.
 * Paramètres : Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 :
 *             projet == NULL,
 */
{
    BUGMSG(projet, -1, gettext("Paramètre %s incorrect.\n"), "projet");
    
    while (projet->combinaisons.elu_equ != NULL)
    {
        g_list_free(((Ponderation*)projet->combinaisons.elu_equ->data)->elements);
        projet->combinaisons.elu_equ = g_list_delete_link(projet->combinaisons.elu_equ, projet->combinaisons.elu_equ);
    }
    while (projet->combinaisons.elu_str != NULL)
    {
        g_list_free(((Ponderation*)projet->combinaisons.elu_str->data)->elements);
        projet->combinaisons.elu_str = g_list_delete_link(projet->combinaisons.elu_str, projet->combinaisons.elu_str);
    }
    while (projet->combinaisons.elu_geo != NULL)
    {
        g_list_free(((Ponderation*)projet->combinaisons.elu_geo->data)->elements);
        projet->combinaisons.elu_geo = g_list_delete_link(projet->combinaisons.elu_geo, projet->combinaisons.elu_geo);
    }
    while (projet->combinaisons.elu_fat != NULL)
    {
        g_list_free(((Ponderation*)projet->combinaisons.elu_fat->data)->elements);
        projet->combinaisons.elu_fat = g_list_delete_link(projet->combinaisons.elu_fat, projet->combinaisons.elu_fat);
    }
    while (projet->combinaisons.elu_acc != NULL)
    {
        g_list_free(((Ponderation*)projet->combinaisons.elu_acc->data)->elements);
        projet->combinaisons.elu_acc = g_list_delete_link(projet->combinaisons.elu_acc, projet->combinaisons.elu_acc);
    }
    while (projet->combinaisons.elu_sis != NULL)
    {
        g_list_free(((Ponderation*)projet->combinaisons.elu_sis->data)->elements);
        projet->combinaisons.elu_sis = g_list_delete_link(projet->combinaisons.elu_sis, projet->combinaisons.elu_sis);
    }
    while (projet->combinaisons.els_car != NULL)
    {
        g_list_free(((Ponderation*)projet->combinaisons.els_car->data)->elements);
        projet->combinaisons.els_car = g_list_delete_link(projet->combinaisons.els_car, projet->combinaisons.els_car);
    }
    while (projet->combinaisons.els_freq != NULL)
    {
        g_list_free(((Ponderation*)projet->combinaisons.els_freq->data)->elements);
        projet->combinaisons.els_freq = g_list_delete_link(projet->combinaisons.els_freq, projet->combinaisons.els_freq);
    }
    while (projet->combinaisons.els_perm != NULL)
    {
        g_list_free(((Ponderation*)projet->combinaisons.els_perm->data)->elements);
        projet->combinaisons.els_perm = g_list_delete_link(projet->combinaisons.els_perm, projet->combinaisons.els_perm);
    }
    
    return 0;
}


G_MODULE_EXPORT int _1990_combinaisons_genere(Projet *projet)
/* Description : Génère l'ensemble des combinaisons et pondérations du projet.
 * Paramètres : Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 :
 *             projet == NULL,
 *             projet->niveaux_groupes == NULL,
 *             _1990_combinaisons_empty,
 *             _1990_action_categorie_bat,
 *             _1990_combinaisons_genere_or,
 *             _1990_combinaisons_genere_xor,
 *             _1990_combinaisons_genere_and,
 *             _1990_ponderations_genere.
 */
{
    unsigned int    i;
    
    BUGMSG(projet, -1, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->niveaux_groupes, -1, gettext("Le projet ne possède pas de niveaux de groupes.\n"));
    
    BUG(_1990_combinaisons_free(projet) == 0, -1);
    
    // Pour chaque action
    //     Cette boucle permet de générer toutes les combinaisons en prenant en compte le
    //       principe d'action prédominante. Ainsi, à chaque passage de la boucle, l'action
    //       numéro i est considérée comme action prédominante.
    for (i=0;i<g_list_length(projet->actions);i++)
    {
        GList               *list_parcours;
        Action              *action;
        unsigned int        j;
        Action_Categorie    categorie;
        
 /* On supprime les combinaisons temporaires générées lors du passage de la boucle précédente*/
        list_parcours = projet->niveaux_groupes;
        do
        {
            Niveau_Groupe   *niveau = list_parcours->data;
            
            if (niveau->groupes != NULL)
            {
                GList   *list_parcours2 = niveau->groupes;
                
                do
                {
                    Groupe      *groupe = list_parcours2->data;
                    
                    while (groupe->tmp_combinaison.combinaisons != NULL)
                    {
                        g_list_free(((Combinaison*)groupe->tmp_combinaison.combinaisons->data)->elements);
                        groupe->tmp_combinaison.combinaisons = g_list_delete_link(groupe->tmp_combinaison.combinaisons, groupe->tmp_combinaison.combinaisons);
                    }
                    list_parcours2 = g_list_next(list_parcours2);
                }
                while (list_parcours2 != NULL);
            }
            list_parcours = g_list_next(list_parcours);
        }
        while (list_parcours != NULL);
        
    //     Attribution à l'action numéro i du flags&1 = 1 afin d'indiquer qu'il s'agit d'une
    //       action prédominante et mise à 0 du flags&1 pour les autres actions.
        list_parcours = projet->actions;
        for (j=0;j<i;j++)
        {
            action = list_parcours->data;
            action->flags = 0;
            list_parcours = g_list_next(list_parcours);
        }
        action = list_parcours->data;
        categorie = _1990_action_categorie_bat(action->type, projet->pays);
        BUG(categorie != ACTION_INCONNUE, -1);
        if (categorie == ACTION_VARIABLE)
            action->flags = 1;
        else
            action->flags = 0;
        list_parcours = g_list_next(list_parcours);
        for (j=i+1;j<g_list_length(projet->actions);j++)
        {
            action = list_parcours->data;
            action->flags = 0;
            list_parcours = g_list_next(list_parcours);
        }
        
    //     Générer des combinaisons de toutes les groupes pour tous les niveaux :
    //     Pour chaque niveau de niveaux_groupes
    //         Pour chaque groupe du niveau en cours
    //             Génération des combinaisons avec l'action prédominante définie précédemment.
    //         FinPour
    //     FinPour
        list_parcours = projet->niveaux_groupes;
        do
        {
            Niveau_Groupe   *niveau = list_parcours->data;
            
            if (niveau->groupes != NULL)
            {
                GList   *list_parcours2 = niveau->groupes;
                do
                {
                    Groupe  *groupe = list_parcours2->data;
                    switch (groupe->type_combinaison)
                    {
                        case GROUPE_COMBINAISON_OR :
                        {
                            BUG(_1990_combinaisons_genere_or(projet, niveau, groupe) == 0, -1);
                            break;
                        }
                        case GROUPE_COMBINAISON_XOR :
                        {
                            BUG(_1990_combinaisons_genere_xor(projet, niveau, groupe) == 0, -1);
                            break;
                        }
                        case GROUPE_COMBINAISON_AND :
                        {
                            BUG(_1990_combinaisons_genere_and(projet, niveau, groupe) == 0, -1);
                            break;
                        }
                        default :
                        {
                            BUG(0, -1);
                            break;
                        }
                    }
                    list_parcours2 = g_list_next(list_parcours2);
                }
                while (list_parcours2 != NULL);
            }
            list_parcours = g_list_next(list_parcours);
        }
        while (list_parcours != NULL);
        
    //     Génération des pondérations (avec les coefficients de sécurité partiels) à partir
    //       des combinaisons.
        BUG(_1990_ponderations_genere(projet) == 0, -1);
    }
    // FinPour
    
    return 0;
}
