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

#include "common_projet.h"
#include "common_erreurs.h"
#include "1990_action.h"
#include "1990_groupe.h"
#include "1990_ponderations.h"


int _1990_combinaisons_verifie_double(GList *liste_combinaisons, GList *comb_a_verifier)
/* Description : Vérifie si une combinaison est déjà présente dans une liste de combinaisons.
 *               FONCTION INTERNE.
 * Paramètres : GList *liste_combinaisons : liste de combinaisons,
 *            : GList *comb_a_verifier : combinaison à vérifier.
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
    if (comb_a_verifier == NULL)
        return 1;
    
    list_parcours = liste_combinaisons;
    do
    {
        int     doublon = 1;
        GList   *comb_en_cours = list_parcours->data;
        
        /* On vérifie si la combinaison en cours possède le même nombre d'éléments que la combinaison à vérifier */
        if (g_list_length(comb_en_cours) == g_list_length(comb_a_verifier))
        {
            GList   *list_1, *list_2;
            
            list_1 = comb_en_cours;
            list_2 = comb_a_verifier;
            
            do
            {
                Combinaison *elem1 = list_1->data;
                Combinaison *elem2 = list_2->data;
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


gboolean _1990_combinaisons_duplique(GList **liste_comb_destination, GList *liste_comb_source,
  gboolean sans_double)
/* Description : Ajoute à une liste de combinaisons existante une liste de combinaisons.
 *               FONCTION INTERNE.
 * Paramètres : GList **liste_comb_destination : liste de combinaisons qui recevra les
 *              combinaisons sources
 *            : GList *liste_comb_source : liste de combinaisons source
 *            : gboolean sans_double : TRUE pour qu'aucune combinaison ne soit ajoutée en
 *              double.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             liste_comb_destination == NULL,
 *             erreur d'allocation mémoire.
 */
{
    GList   *list_parcours;
    
    BUGMSG(liste_comb_destination, FALSE, gettext("Paramètre %s incorrect.\n"), "liste_comb_destination");
    
    if (liste_comb_source == NULL)
        return TRUE;
    
    list_parcours = liste_comb_source;
    do
    {
        GList   *combinaison_source = list_parcours->data;
        int     verifie_double;
        
        if (sans_double == TRUE)
            verifie_double = _1990_combinaisons_verifie_double(*liste_comb_destination, combinaison_source);
        else
            verifie_double = 0;
        
        switch (verifie_double)
        {
            /* On l'ajoute donc */
            case 0 :
            {
                GList   *combinaison_destination;
                
                /* Duplication de la combinaison */
                combinaison_destination = NULL;
                if (combinaison_source != NULL)
                {
                    GList   *list_parcours2 = combinaison_source;
                    
                    do
                    {
                        Combinaison *element_source = list_parcours2->data;
                        Combinaison *element_destination = malloc(sizeof(Combinaison));
                        
                        BUGMSG(element_destination, FALSE, gettext("Erreur d'allocation mémoire.\n"));
                        
                        element_destination->action = element_source->action;
                        element_destination->flags = element_source->flags;
                        combinaison_destination = g_list_append(combinaison_destination, element_destination);
                        
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
                BUG(0, FALSE);
            }
        }
        
        list_parcours = g_list_next(list_parcours);
    }
    while (list_parcours != NULL);
    
    return TRUE;
}


gboolean _1990_combinaisons_action_predominante(GList *combinaison, Type_Pays pays)
/* Description : Modifie le flag de toutes les actions variables d'une combinaison afin de les
 *               considérer comme action prédominante.
 *               FONCTION INTERNE.
 * Paramètres : GList *combinaison : combinaison à modifier,
 *            : Type_Pays pays : le pays.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             combinaison == NULL,
 *             _1990_action_categorie_bat.
 */
{
    GList   *list_parcours;
    
    BUGMSG(combinaison, FALSE, gettext("Paramètre %s incorrect.\n"), "combinaison");
    
    if (combinaison == NULL)
        return 0;
    
    list_parcours = combinaison;
    do
    {
        Combinaison         *combinaison_element = list_parcours->data;
        Action_Categorie    categorie = _1990_action_categorie_bat(combinaison_element->action->type, pays);
        
        BUG(categorie != ACTION_INCONNUE, FALSE);
        if (categorie == ACTION_VARIABLE)
            combinaison_element->flags = 1;
        
        list_parcours = g_list_next(list_parcours);
    }
    while (list_parcours != NULL);
    
    return TRUE;
}


gboolean _1990_combinaisons_genere_xor(Projet *projet, Niveau_Groupe *niveau, Groupe *groupe)
/* Description : Génère toutes les combinaisons d'un groupe possédant comme type de combinaison
 *               GROUPE_COMBINAISON_XOR.
 *               FONCTION INTERNE.
 * Paramètres : Projet *projet : la variable projet,
 *            : Niveau_Groupe *niveau : niveau du groupe à analyser,
 *            : Groupe *groupe : groupe à analyser.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
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
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->niveaux_groupes, FALSE, gettext("Le projet ne possède pas de niveaux de groupes.\n"));
    BUGMSG(niveau, FALSE, gettext("Paramètre %s incorrect.\n"), "niveau");
    BUGMSG(niveau->groupes, FALSE, gettext("Le niveau %u est vide. Veuillez soit le remplir, soit le supprimer.\n"), niveau->numero);
    BUGMSG(groupe->type_combinaison == GROUPE_COMBINAISON_XOR, FALSE, gettext("Seuls les groupes possédant un type de combinaison XOR peuvent appeler _1990_combinaisons_genere_xor.\n"));
    
    // Si le nombre d'éléments est nul Alors
    //     Fin.
    // FinSi
    if (groupe->elements == NULL)
        return TRUE;
    
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
            Element     *element_en_cours = list_parcours->data;
            GList       *nouvelle_combinaison;
            Combinaison *nouveau_element;
            Action      *action;
            
            // On vérifie si l'action possède une charge. Si non, on ignore l'action.
            BUG(action = _1990_action_cherche_numero(projet, element_en_cours->numero), FALSE);
            if (action->charges != NULL)
            {
                BUGMSG(nouveau_element = malloc(sizeof(Combinaison)), FALSE, gettext("Erreur d'allocation mémoire.\n"));
                nouvelle_combinaison = NULL;
                nouveau_element->action = action;
                nouveau_element->flags = nouveau_element->action->flags;
                nouvelle_combinaison = g_list_append(nouvelle_combinaison, nouveau_element);
                groupe->tmp_combinaison = g_list_append(groupe->tmp_combinaison, nouvelle_combinaison);
            }
            
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
        BUGMSG(list_groupe_n_1, FALSE, gettext("Impossible de trouver le niveau %u dans la liste des niveaux de groupes.\n"), niveau->numero);
        list_groupe_n_1 = g_list_previous(list_groupe_n_1);
        BUGMSG(list_groupe_n_1, FALSE, gettext("Impossible de trouver le niveau précédent le niveau %u.\n"), niveau->numero);
        
        niveau = list_groupe_n_1->data;
        do
        {
            Element     *element_tmp = list_parcours->data;
            Groupe      *groupe_n_1;
            
            BUG(groupe_n_1 = _1990_groupe_positionne_groupe(niveau, element_tmp->numero), FALSE);
            BUG(_1990_combinaisons_duplique(&(groupe->tmp_combinaison), groupe_n_1->tmp_combinaison, TRUE), FALSE);
            
            list_parcours = g_list_next(list_parcours);
        }
        while (list_parcours != NULL);
    }
    // FinSi
    
    return TRUE;
}


gboolean _1990_combinaisons_fusion(GList *destination, GList *source)
/* Description : Fusionne deux combinaisons. Les éléments de la combinaison source sont ajoutés
 *               à la fin de la combinaison destination.
 *               FONCTION INTERNE.
 * Paramètres : GList *destination : combinaison de destination,
 *            : GList *source : combinaison source.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             destination == NULL,
 *             source == NULL,
 *             erreur d'allocation mémoire.
 */
{
    GList   *list_parcours;
    
    BUGMSG(destination, FALSE, gettext("Paramètre %s incorrect.\n"), "destination");
    BUGMSG(source, FALSE, gettext("Paramètre %s incorrect.\n"), "source");
    
    list_parcours = source;
    while (list_parcours != NULL)
    {
        Combinaison *element_source = list_parcours->data;
        Combinaison *element_destination = malloc(sizeof(Combinaison));
        
        BUGMSG(element_destination, FALSE, gettext("Erreur d'allocation mémoire.\n"));
        element_destination->action = element_source->action;
        element_destination->flags = element_source->flags;
        destination = g_list_append(destination, element_destination);
        
        list_parcours = g_list_next(list_parcours);
    }
    
    return TRUE;
}


void _1990_combinaisons_free_groupe_tmp_combinaison(void *data)
/* Description : Permet de supprimer toutes les combinaisons temporaires contenues dans les
 *               groupes. À utiliser avec la fonction list_traverse.
 *               FONCTION INTERNE.
 * Paramètres : void *data : donnée à libérer.
 * Valeur renvoyée : void.
 */
{
    GList   *combinaison = (GList*)data;
    
    BUGMSG(data, , gettext("Paramètre %s incorrect.\n"), "data");
    
    if (combinaison != NULL)
        g_list_free_full(combinaison, g_free);
    
    return;
}


gboolean _1990_combinaisons_genere_and(Projet *projet, Niveau_Groupe *niveau, Groupe *groupe)
/* Description : Génère toutes les combinaisons d'un groupe possédant comme type de combinaison
 *               GROUPE_COMBINAISON_AND.
 *               FONCTION INTERNE.
 * Paramètres : Projet *projet : la variable projet,
 *            : Niveau_Groupe *niveau : niveau du groupe à analyser,
 *            : Groupe *groupe : groupe à analyser.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
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
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->niveaux_groupes, FALSE, gettext("Le projet ne possède pas de niveaux de groupes.\n"));
    BUGMSG(niveau, FALSE, gettext("Paramètre %s incorrect.\n"), "niveau");
    BUGMSG(niveau->groupes, FALSE, gettext("Le niveau %u est vide. Veuillez soit le remplir, soit le supprimer.\n"), niveau->numero);
    BUGMSG(groupe->type_combinaison == GROUPE_COMBINAISON_AND, FALSE, gettext("Seuls les groupes possédant un type de combinaison AND peuvent appeler _1990_combinaisons_genere_and.\n"));
    
    if (groupe->elements == NULL)
        return TRUE;
    
    list_parcours = groupe->elements;
    
    // Si l'élément courant de niveaux_groupes est le premier de la liste Alors
    //     L'opération AND consiste à créer une combinaison contenant l'ensemble des actions
    //       listées dans le groupe courant du premier élément de niveaux_groupes. Si une seule
    //       des actions variables est prédominante, par l'action de l'opérateur AND, toutes
    //       les actions variables deviennent prédominantes.
    if (niveau == projet->niveaux_groupes->data)
    {
        GList   *comb;
        int     action_predominante = 0;
        
        comb = NULL;
        
        do
        {
            Element     *element_en_cours = list_parcours->data;
            Combinaison *nouveau_element;
            Action      *action;
            
            BUG(action = _1990_action_cherche_numero(projet, element_en_cours->numero), FALSE);
            // On ajoute l'action que si elle possède des charges
            if (action->charges != NULL)
            {
                BUGMSG(nouveau_element = malloc(sizeof(Combinaison)), FALSE, gettext("Erreur d'allocation mémoire.\n"));
                nouveau_element->action = action;
                nouveau_element->flags = nouveau_element->action->flags;
                if ((nouveau_element->flags & 1) != 0)
                    action_predominante = 1;
                comb = g_list_append(comb, nouveau_element);
            }
            
            list_parcours = g_list_next(list_parcours);
        }
        while (list_parcours != NULL);
        
        if (comb != NULL)
        {
            if (action_predominante == 1)
                BUG(_1990_combinaisons_action_predominante(comb, projet->parametres.pays), FALSE);
            groupe->tmp_combinaison = g_list_append(groupe->tmp_combinaison, comb);
        }
        else
            free(comb);
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
        BUGMSG(list_groupe_n_1, FALSE, gettext("Impossible de trouver le niveau %u dans la liste des niveaux de groupes.\n"), niveau->numero);
        list_groupe_n_1 = g_list_previous(list_groupe_n_1);
        BUGMSG(list_groupe_n_1, FALSE, gettext("Impossible de trouver le niveau précédent le niveau %u.\n"), niveau->numero);
        niveau = list_groupe_n_1->data;
        
        do
        {
            Element     *element_en_cours = list_parcours->data;
            
            /* On se positionne sur l'élément en cours du groupe */
            BUG(groupe_n_1 = _1990_groupe_positionne_groupe(niveau, element_en_cours->numero), FALSE);
            
            /* Alors, il s'agit de la première passe. On duplique donc simplement. */
            if (groupe->elements->data == element_en_cours)
                BUG(_1990_combinaisons_duplique(&nouvelles_combinaisons, groupe_n_1->tmp_combinaison, FALSE), FALSE);
            else
            {
                /* transition est utilisée de façon temporaire pour dupliquer nouvelles_combinaisons en cas de besoin */
                GList           *transition = NULL;
                GList           *list_parcours2, *list_parcours3;
                unsigned int    j, i, nbboucle;
                
                /* On duplique les combinaisons actuellement dans nouvelles_combinaisons autant
                 * de fois (moins 1) qu'il y a d'éléments dans le groupe de la passe actuelle.
                 * (première partie de la passe 2) */
                BUG(_1990_combinaisons_duplique(&transition, nouvelles_combinaisons, FALSE), FALSE);
                nbboucle = g_list_length(groupe_n_1->tmp_combinaison);
                for (i=2;i<=nbboucle;i++)
                    BUG(_1990_combinaisons_duplique(&nouvelles_combinaisons, transition, FALSE), FALSE);
                
                /* On ajoute à la fin de toutes les combinaisons dupliquées les combinaisons
                 * contenues dans le groupe en cours (deuxième partie de la passe 2) */
                list_parcours2 = groupe_n_1->tmp_combinaison;
                list_parcours3 = nouvelles_combinaisons;
                for (i=1;i<=nbboucle;i++)
                {
                    GList   *combinaison2;
                    
                    combinaison2 = list_parcours2->data;
                    for (j=1;j<=g_list_length(transition);j++)
                    {
                        GList   *combinaison1;
                        
                        combinaison1 = list_parcours3->data;
                        BUG(_1990_combinaisons_fusion(combinaison1, combinaison2), FALSE);
                        list_parcours3 = g_list_next(list_parcours3);
                    }
                    
                    list_parcours2 = g_list_next(list_parcours2);
                }
                
                g_list_free_full(transition, &(_1990_combinaisons_free_groupe_tmp_combinaison));
            }
            
            list_parcours = g_list_next(list_parcours);
        }
        while (list_parcours != NULL);
        
        /* On ajoute définitivement les nouvelles combinaisons */
        BUG(_1990_combinaisons_duplique(&(groupe->tmp_combinaison), nouvelles_combinaisons, TRUE), FALSE);
        g_list_free_full(nouvelles_combinaisons, &_1990_combinaisons_free_groupe_tmp_combinaison);
    }
    // FinSi
    
    return TRUE;
}


gboolean _1990_combinaisons_genere_or(Projet *projet, Niveau_Groupe *niveau, Groupe *groupe)
/* Description : Génère toutes les combinaisons d'un groupe possédant comme type de combinaison
 *               GROUPE_COMBINAISON_OR.
 *               FONCTION INTERNE.
 * Paramètres : Projet *projet : la variable projet,
 *            : Niveau_Groupe *niveau : niveau du groupe à analyser,
 *            : Groupe *groupe : groupe à analyser.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
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
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->niveaux_groupes, FALSE, gettext("Le projet ne possède pas de niveaux de groupes.\n"));
    BUGMSG(niveau, FALSE, gettext("Paramètre %s incorrect.\n"), "niveau");
    BUGMSG(niveau->groupes, FALSE, gettext("Le niveau %u est vide. Veuillez soit le remplir, soit le supprimer.\n"), niveau->numero);
    BUGMSG(groupe->type_combinaison == GROUPE_COMBINAISON_OR, FALSE, gettext("Seuls les groupes possédant un type de combinaison OR peuvent appeler _1990_combinaisons_genere_or.\n"));
    
    if (groupe == NULL)
        return TRUE;
    
    boucle = 2;
    for (i=2;i<=g_list_length(groupe->elements);i++)
        boucle = boucle*2;
    
    // Si l'élément courant de niveaux_groupes est le premier de la liste Alors
    //     Afin de générer l'ensemble des combinaisons, il va être nécessaire de réaliser une
    //       boucle de 2^(g_list_length(groupe)).
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
            GList           *nouvelle_combinaison;
            
            nouvelle_combinaison = NULL;
            
            list_parcours = groupe->elements;
            
            do
            {
                if ((parcours_bits & 1) == 1)
                {
                    Combinaison *element;
                    Element     *element_en_cours = list_parcours->data;
                    Action      *action;
                    
                    BUG(action = _1990_action_cherche_numero(projet, element_en_cours->numero), FALSE);
                    // On ajoute l'action que si elle possède des charges
                    if (action->charges != NULL)
                    {
                        BUGMSG(element = malloc(sizeof(Combinaison)), FALSE, gettext("Erreur d'allocation mémoire.\n"));
                        element->action = action;
                        element->flags = element->action->flags;
                        if ((element->flags & 1) != 0)
                            action_predominante = 1;
                        nouvelle_combinaison = g_list_append(nouvelle_combinaison, element);
                    }
                }
                parcours_bits = parcours_bits >> 1;
                list_parcours = g_list_next(list_parcours);
            }
            while (parcours_bits != 0);
            
            if (nouvelle_combinaison != NULL)
            {
                if (action_predominante == 1)
                    BUG(_1990_combinaisons_action_predominante(nouvelle_combinaison, projet->parametres.pays), FALSE);
                groupe->tmp_combinaison = g_list_append(groupe->tmp_combinaison, nouvelle_combinaison);
            }
            else
                free(nouvelle_combinaison);
        }
    }
    // Sinon
    //     Création d'une boucle pour générer 2^(g_list_length(groupe)) combinaisons
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
        BUGMSG(list_groupe_n_1, FALSE, gettext("Impossible de trouver le niveau %u dans la liste des niveaux de groupes.\n"), niveau->numero);
        list_groupe_n_1 = g_list_previous(list_groupe_n_1);
        BUGMSG(list_groupe_n_1, FALSE, gettext("Impossible de trouver le niveau précédent le niveau %u.\n"), niveau->numero);
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
                    
                    BUG(groupe_n_1 = _1990_groupe_positionne_groupe(niveau, element_en_cours->numero), FALSE);
                    
                    if (groupe_n_1->tmp_combinaison != NULL)
                    {
  /* Il s'agit de la première passe. On duplique donc simplement. */
                        if (nouvelles_combinaisons == NULL)
                            BUG(_1990_combinaisons_duplique(&nouvelles_combinaisons, groupe_n_1->tmp_combinaison, FALSE), FALSE);
                        else
                        {
  /* transition est utilisée de façon temporaire pour dupliquer nouvelles_combinaisons en cas
   * de besoin */
                            GList   *transition = NULL;
                            GList   *list_parcours2, *list_parcours3;
                            
                            BUG(_1990_combinaisons_duplique(&transition, nouvelles_combinaisons, FALSE), FALSE);
                            
  /* On duplique les combinaisons actuellement dans nouvelles_combinaisons autant de fois
   * (moins 1) qu'il y a d'éléments dans le groupe de la passe actuelle. */
                            for (j=2;j<=g_list_length(groupe_n_1->tmp_combinaison);j++)
                                BUG(_1990_combinaisons_duplique(&nouvelles_combinaisons, transition, FALSE), FALSE);
                            
  /* Ensuite on fusionne chaque série de doublon créée avec une combinaison provenant de
   * groupe_n_1 */
                            list_parcours2 = groupe_n_1->tmp_combinaison;
                            list_parcours3 = nouvelles_combinaisons;
                            for (j=1;j<=g_list_length(groupe_n_1->tmp_combinaison);j++)
                            {
                                GList   *combinaison2 = list_parcours2->data;
                                
                                for (k=1;k<=g_list_length(transition);k++)
                                {
                                    GList   *combinaison1 = list_parcours3->data;
                                    
                                    BUG(_1990_combinaisons_fusion(combinaison1, combinaison2), FALSE);
                                    list_parcours3 = g_list_next(list_parcours3);
                                }
                                list_parcours2 = g_list_next(list_parcours2);
                            }
                            g_list_free_full(transition, &_1990_combinaisons_free_groupe_tmp_combinaison);
                        }
                    }
                }
                parcours_bits = parcours_bits >> 1;
                list_parcours = g_list_next(list_parcours);
            }
            while (parcours_bits != 0);
            
            BUG(_1990_combinaisons_duplique(&(groupe->tmp_combinaison), nouvelles_combinaisons, TRUE), FALSE);
            g_list_free_full(nouvelles_combinaisons, &_1990_combinaisons_free_groupe_tmp_combinaison);
        }
    }
    
    return TRUE;
}


gboolean _1990_combinaisons_init(Projet *projet)
/* Description : Initialise la mémoire pour les combinaisons à l'ELU et l'ELS.
 * Paramètres : Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL.
 */
{
#ifdef ENABLE_GTK
    GtkTreeIter Iter;
#endif
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    // Trivial
    
    projet->combinaisons.flags = 1 | 4;
    projet->combinaisons.elu_equ = NULL;
    projet->combinaisons.elu_str = NULL;
    projet->combinaisons.elu_geo = NULL;
    projet->combinaisons.elu_fat = NULL;
    projet->combinaisons.elu_acc = NULL;
    projet->combinaisons.elu_sis = NULL;
    projet->combinaisons.els_car = NULL;
    projet->combinaisons.els_freq = NULL;
    projet->combinaisons.els_perm = NULL;
    
#ifdef ENABLE_GTK
    projet->combinaisons.list_el_desc = gtk_list_store_new(1, G_TYPE_STRING);
    gtk_list_store_append(projet->combinaisons.list_el_desc, &Iter);
    gtk_list_store_set(projet->combinaisons.list_el_desc, &Iter, 0, gettext("ELU équilibre"), -1);
    gtk_list_store_append(projet->combinaisons.list_el_desc, &Iter);
    gtk_list_store_set(projet->combinaisons.list_el_desc, &Iter, 0, gettext("ELU structure"), -1);
    gtk_list_store_append(projet->combinaisons.list_el_desc, &Iter);
    gtk_list_store_set(projet->combinaisons.list_el_desc, &Iter, 0, gettext("ELU géotechnique"), -1);
    gtk_list_store_append(projet->combinaisons.list_el_desc, &Iter);
    gtk_list_store_set(projet->combinaisons.list_el_desc, &Iter, 0, gettext("ELU fatigue"), -1);
    gtk_list_store_append(projet->combinaisons.list_el_desc, &Iter);
    gtk_list_store_set(projet->combinaisons.list_el_desc, &Iter, 0, gettext("ELU accidentel"), -1);
    gtk_list_store_append(projet->combinaisons.list_el_desc, &Iter);
    gtk_list_store_set(projet->combinaisons.list_el_desc, &Iter, 0, gettext("ELU sismique"), -1);
    gtk_list_store_append(projet->combinaisons.list_el_desc, &Iter);
    gtk_list_store_set(projet->combinaisons.list_el_desc, &Iter, 0, gettext("ELS caractéristique"), -1);
    gtk_list_store_append(projet->combinaisons.list_el_desc, &Iter);
    gtk_list_store_set(projet->combinaisons.list_el_desc, &Iter, 0, gettext("ELS fréquent"), -1);
    gtk_list_store_append(projet->combinaisons.list_el_desc, &Iter);
    gtk_list_store_set(projet->combinaisons.list_el_desc, &Iter, 0, gettext("ELS permanent"), -1);
#endif
    
    return TRUE;
}


void _1990_combinaisons_free_1(void *data)
/* Description : Fonction utilisable avec g_list_free_full pour libérer une combinaison.
 * Paramètres : void *data : une pondération à libérer.
 * Valeur renvoyée : Aucune.
 */
{
    GList   *pond = data;
    
    g_list_free_full(pond, free);
    
    return;
}


gboolean _1990_combinaisons_free(Projet *projet)
/* Description : libère l'ensemble des combinaisons à l'ELU et l'ELS sans libérer la liste.
 * Paramètres : Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL.
 */
{
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    
    if (projet->combinaisons.elu_equ != NULL)
    {
        g_list_free_full(projet->combinaisons.elu_equ, _1990_combinaisons_free_1);
        projet->combinaisons.elu_equ = NULL;
    }
    if (projet->combinaisons.elu_str != NULL)
    {
        g_list_free_full(projet->combinaisons.elu_str, _1990_combinaisons_free_1);
        projet->combinaisons.elu_str = NULL;
    }
    if (projet->combinaisons.elu_geo != NULL)
    {
        g_list_free_full(projet->combinaisons.elu_geo, _1990_combinaisons_free_1);
        projet->combinaisons.elu_geo = NULL;
    }
    if (projet->combinaisons.elu_fat != NULL)
    {
        g_list_free_full(projet->combinaisons.elu_fat, _1990_combinaisons_free_1);
        projet->combinaisons.elu_fat = NULL;
    }
    if (projet->combinaisons.elu_acc != NULL)
    {
        g_list_free_full(projet->combinaisons.elu_acc, _1990_combinaisons_free_1);
        projet->combinaisons.elu_acc = NULL;
    }
    if (projet->combinaisons.elu_sis != NULL)
    {
        g_list_free_full(projet->combinaisons.elu_sis, _1990_combinaisons_free_1);
        projet->combinaisons.elu_sis = NULL;
    }
    if (projet->combinaisons.els_car != NULL)
    {
        g_list_free_full(projet->combinaisons.els_car, _1990_combinaisons_free_1);
        projet->combinaisons.els_car = NULL;
    }
    if (projet->combinaisons.els_freq != NULL)
    {
        g_list_free_full(projet->combinaisons.els_freq, _1990_combinaisons_free_1);
        projet->combinaisons.els_freq = NULL;
    }
    if (projet->combinaisons.els_perm != NULL)
    {
        g_list_free_full(projet->combinaisons.els_perm, _1990_combinaisons_free_1);
        projet->combinaisons.els_perm = NULL;
    }
    
#ifdef ENABLE_GTK
    g_object_unref(projet->combinaisons.list_el_desc);
#endif
    
    return TRUE;
}


gboolean _1990_combinaisons_genere(Projet *projet)
/* Description : Génère l'ensemble des combinaisons et pondérations du projet.
 * Paramètres : Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
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
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->niveaux_groupes, FALSE, gettext("Le projet ne possède pas de niveaux de groupes.\n"));
    
#ifdef ENABLE_GTK
    g_object_ref(projet->combinaisons.list_el_desc);
#endif
    BUG(_1990_combinaisons_free(projet), FALSE);
    
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
                    
                    while (groupe->tmp_combinaison != NULL)
                    {
                        g_list_free_full((GList*)groupe->tmp_combinaison->data, g_free);
                        groupe->tmp_combinaison = g_list_delete_link(groupe->tmp_combinaison, groupe->tmp_combinaison);
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
        categorie = _1990_action_categorie_bat(action->type, projet->parametres.pays);
        BUG(categorie != ACTION_INCONNUE, FALSE);
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
                            BUG(_1990_combinaisons_genere_or(projet, niveau, groupe), FALSE);
                            break;
                        }
                        case GROUPE_COMBINAISON_XOR :
                        {
                            BUG(_1990_combinaisons_genere_xor(projet, niveau, groupe), FALSE);
                            break;
                        }
                        case GROUPE_COMBINAISON_AND :
                        {
                            BUG(_1990_combinaisons_genere_and(projet, niveau, groupe), FALSE);
                            break;
                        }
                        default :
                        {
                            BUG(0, FALSE);
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
        BUG(_1990_ponderations_genere(projet), FALSE);
    }
    // FinPour
    
    return TRUE;
}
