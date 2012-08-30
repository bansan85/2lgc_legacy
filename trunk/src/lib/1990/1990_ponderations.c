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
#include <gmodule.h>

#include "common_projet.h"
#include "common_erreurs.h"
#include "common_maths.h"
#include "1990_actions.h"


int _1990_ponderations_verifie_double(GList *liste_ponderations, Ponderation* pond_a_verifier)
/* Description : Vérifie dans la liste des ponderations si la ponderation à vérifier est déjà
 *                 présente.
 * Paramètres : GList *ponderations : liste des pondérations,
 *            : Ponderation* pond_a_verifier : pondération à vérifier.
 * Valeur renvoyée :
 *   Succès : 0 si la pondération n'existe pas,
 *          : 1 si la pondération existe.
 *   Échec : -1 :
 *             liste_ponderations == NULL,
 *             pond_a_verifier == NULL.
 */
{
    GList   *list_parcours;
    
    BUGMSG(pond_a_verifier, -1, gettext("Paramètre %s incorrect.\n"), "pond_a_verifier");
    
    if (liste_ponderations == NULL)
        return 0;
    
    /* En renvoyant ici 1, la fonction fait croire que la pondération existe.
     * En vérité, c'est surtout qu'une pondération sans élément n'est pas intéressante
     * à conserver, à la différence des combinaisons vides qui peuvent être utilisées
     * par les niveaux supérieurs */
    if (pond_a_verifier->elements == NULL)
        return 1;
    
    list_parcours = liste_ponderations;
    do
    {
        int         doublon;
        Ponderation *ponderation;
        
        /* On pense que la pondération est identique jusqu'à preuve du contraire */
        doublon = 1;
        ponderation = list_parcours->data;
        if (g_list_length(ponderation->elements) == g_list_length(pond_a_verifier->elements))
        {
            GList   *list_parcours2 = ponderation->elements;
            GList   *list_parcours3 = pond_a_verifier->elements;
            
            do
            {
                Ponderation_Element *elem1, *elem2;
                
                elem1 = list_parcours2->data;
                elem2 = list_parcours3->data;
                /* Preuve ici */
                if ((elem1->action != elem2->action) || (elem1->psi != elem2->psi) || (!(ERREUR_RELATIVE_EGALE(elem1->ponderation, elem2->ponderation))))
                    doublon = 0;
                
                list_parcours2 = g_list_next(list_parcours2);
                list_parcours3 = g_list_next(list_parcours3);
            }
            while ((list_parcours2 != NULL) && (list_parcours3 != NULL) && (doublon == 1));
            if (doublon == 1)
                return 1;
        }
        list_parcours = g_list_next(list_parcours);
    }
    while (list_parcours != NULL);
    
    return 0;
}


gboolean _1990_ponderations_duplique_sans_double(GList **liste_pond_destination,
  GList *liste_pond_source)
/* Description : Ajoute à une liste de pondérations existante une liste de pondérations.
 *               Une vérification est effectuée pour s'assurer que la liste source ne possède
 *               pas une ou plusieurs pondérations identiques que la liste de destination.
 * Paramètres : GList **liste_pond_destination : liste de ponderations qui recevra les
 *                ponderations sources,
 *            : GList *liste_pond_source : liste de ponderations source.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             liste_pond_destination == NULL,
 *             liste_pond_source == NULL.
 */
{
    GList   *list_parcours;
    
    BUGMSG(liste_pond_destination, FALSE, gettext("Paramètre %s incorrect.\n"), "liste_pond_destination");
    
    if (liste_pond_source == NULL)
        return 0;

    
    list_parcours = liste_pond_source;
    do
    {
        Ponderation *ponderation_source;
        
        ponderation_source = list_parcours->data;
        /* Si la ponderation n'existe pas, on l'ajoute à la fin */
        switch (_1990_ponderations_verifie_double(*liste_pond_destination, ponderation_source))
        {
            case 0 :
            {
                Ponderation *ponderation_destination = malloc(sizeof(Ponderation));
                GList       *list_parcours2 = ponderation_source->elements;
                
                BUGMSG(ponderation_destination, FALSE, gettext("Erreur d'allocation mémoire.\n"));
                ponderation_destination->elements = NULL;
                
                do
                {
                    Ponderation_Element *element_source;
                    Ponderation_Element *element_destination = malloc(sizeof(Ponderation_Element));
                    
                    BUGMSG(element_destination, FALSE, gettext("Erreur d'allocation mémoire.\n"));
                    element_source = list_parcours2->data;
                    element_destination->action = element_source->action;
                    element_destination->flags = element_source->flags;
                    element_destination->psi = element_source->psi;
                    element_destination->ponderation = element_source->ponderation;
                    ponderation_destination->elements = g_list_append(ponderation_destination->elements, element_destination);
                    
                    list_parcours2 = g_list_next(list_parcours2);
                }
                while (list_parcours2 != NULL);
                
                *liste_pond_destination = g_list_append(*liste_pond_destination, ponderation_destination);
                break;
            }
            case 1 :
            {
                break;
            }
            default :
            {
                BUG(0, FALSE);
                break;
            }
        }
        list_parcours = g_list_next(list_parcours);
    }
    while (list_parcours != NULL);
    
    return TRUE;
}


G_MODULE_EXPORT gboolean _1990_ponderations_genere_un(Projet *projet,
  GList **ponderations_destination, double* coef_min, double* coef_max, int dim_coef,
  int psi_dominante, int psi_accompagnement)
/* Description : Génère l'ensemble des pondérations en fonction des paramètres d'entrées. Pour
 *                 une génération exaustive conformément à une norme, il est nécessaire
 *                 d'appeler directement la fonction _1990_ponderations_genere qui se chargera
 *                 d'appeler _1990_ponderations_genere_un autant de fois que nécessaire avec
 *                 des paramètres adaptés.
 * Paramètres : Projet *projet : la variable projet,
 *            : GList* combinaisons_destination : liste dans laquelle sera stockés les
 *                ponderations générées. Les résultats seront filtrés pour éviter les doublons,
 *            : double* coef_min : coefficients psi en situation favorable (min),
 *            : double* coef_max : coefficients psi en situation défavorable (max). L'indice du
 *                tableau à utiliser est celui renvoyé par _1990_action_categorie_bat,
 *            : int dim_coef : nombre d'incides dans le tableau de double coef_max et coef_min,
 *            : int psi_dominante : indice du coefficient psi à utiliser pour l'action variable
 *                prédominante: 0 = psi0, 1 = psi1, 2 = psi et -1 = prendre la valeur 1.0,
 *            : int psi_accompagnement : indice du coefficient psi à utiliser pour les actions
 *                variables d'accompagnement : 0 = psi0, 1 = psi1, 2 = psi et
 *                -1 = prendre la valeur 1.0,
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL) ou
 *             projet->niveaux_groupes == NULL,
 *             groupe->tmp_combinaison.combinaisons == NULL,
 *             en cas d'erreur d'allocation mémoire
 */
{
    int             nbboucle, j;
    Groupe          *groupe;
    Niveau_Groupe   *niveau;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(projet->niveaux_groupes, FALSE, gettext("Le projet ne possède pas de niveaux de groupes.\n"));
    
    // Si le dernier niveau ne possède pas un seul et unique groupe Alors
    //     Fin.
    // FinSi
    niveau = g_list_last(projet->niveaux_groupes)->data;
    BUGMSG(g_list_length(niveau->groupes) == 1, FALSE, gettext("La génération des pondérations est impossible.\nLe dernier niveau ne peut possèder qu'un seul groupe.\n"));
    groupe = niveau->groupes->data;
    
    // Si le groupe du dernier niveau ne possède pas de combinaison Alors
    //     Fin.
    // FinSi
    BUGMSG(groupe->tmp_combinaison.combinaisons, FALSE, gettext("Le dernier niveau ne possède aucune combinaison permettant la génération des pondérations.\n"));
    
    // Génération d'une boucle contenant 2^(nom de ligne dans coef_min et coef_max) permettant
    //   ainsi à à chaque passage de déterminer si le coefficient min ou max doit être pris.
    //   Chaque bit correspond à une ligne des tableaux coef_min et coef_max.
    //   Lorsqu'un bit vaut 0, il est utilisé coef_min dans la pondération.
    //   Lorsqu'un bit vaut 1, il est utilisé coef_max dans la pondération.
    // Pour chaque itération j, définissant chacune une combinaison différente des coefficients
    //   coef_min et coef_max.
    nbboucle = 1 << dim_coef;
    for (j=0;j<nbboucle;j++)
    {
        GList   *list_parcours = groupe->tmp_combinaison.combinaisons;
    //     Pour chaque combinaison dans le groupe final Faire
        do
        {
    //         Déterminer si la pondération générée doit être prise en compte. Elle n'est
    //           valable que si :
    //             - Premièrement, une pondération ne peut posséder une action variable
    //                 d'accompagnement sans action variable prédominante.
    //             - Deuxièmement, lorsqu'une action possède coef_min = 0 et coef_max = 0, il
    //                 convient de ne pas prendre la pondération en compte. Par exemple,
    //                 lorsque les actions à ELU STR sont en cours de génération, il convient
    //                 de ne pas prendre les pondérations possédant des actions accidentelles.
            int         suivant = 0, variable_accompagnement = 0, variable_dominante = 0;
            Combinaison *combinaison;
            Ponderation *ponderation = malloc(sizeof(Ponderation));
            
            BUGMSG(ponderation, FALSE, gettext("Erreur d'allocation mémoire.\n"));
            combinaison = list_parcours->data;
            ponderation->elements = NULL;
            if (combinaison->elements != NULL)
            {
                GList   *list_parcours2 = combinaison->elements;
    //         Pour chaque élément de la combinaison Faire
                do
                {
                    Combinaison_Element *combinaison_element;
                    unsigned int        categorie;
                    double              pond;
                    
                    combinaison_element = list_parcours2->data;
                    categorie = _1990_action_categorie_bat(combinaison_element->action->type, projet->pays);
                    BUG(categorie != ACTION_INCONNUE, FALSE);
    //             Vérification si le coefficient min et max de la catégorie vaut 0.
    //               Si oui, pondération ignorée.
                    if ((ERREUR_RELATIVE_EGALE(0., coef_min[categorie])) && (ERREUR_RELATIVE_EGALE(0., coef_max[categorie])))
                        suivant = 1;
                    else
                    {
       /* On affecte le coefficient min/max à la combinaison pour obtenir la pondération */
                        if ((j & (1 << categorie)) != 0)
                            pond = coef_max[categorie];
                        else
                            pond = coef_min[categorie];
                        
                        if (!(ERREUR_RELATIVE_EGALE(0., pond)))
                        {
                            Ponderation_Element *ponderation_element = malloc(sizeof(Ponderation_Element));
                            
                            BUGMSG(ponderation_element, FALSE, gettext("Erreur d'allocation mémoire.\n"));
                            ponderation_element->action = combinaison_element->action;
                            ponderation_element->flags = combinaison_element->flags;
                            
    //             Vérifier la présente d'une action variable prédominante et d'une action
    //               variable d'accompagnement. Si oui, pondération ignorée.
                            if (categorie == ACTION_VARIABLE)
                            {
                                variable_accompagnement = 1;
                                if ((ponderation_element->flags & 1) != 0)
                                {
                                    variable_dominante = 1;
                                    ponderation_element->psi = psi_dominante;
                                }
                                else
                                    ponderation_element->psi = psi_accompagnement;
                            }
                            /* psi vaut toujours -1 s'il ne s'agit pas d'une action variable */
                            else
                                ponderation_element->psi = -1;
                            
                            ponderation_element->ponderation = pond;
                    
                            ponderation->elements = g_list_append(ponderation->elements, ponderation_element);
                        }
                    }
                    list_parcours2 = g_list_next(list_parcours2);
                }
                while ((list_parcours2 != NULL) && (suivant != 1));
    //         FinPour
            }
    //         Si la pondération n'est pas ignorée Alors
    //             Ajout à la liste des pondérations existante.
    //         FinSi
            if ((variable_accompagnement == 1) && (variable_dominante == 0))
                suivant = 1;
            if ((suivant == 0) && (_1990_ponderations_verifie_double(*ponderations_destination, ponderation) == 0))
                *ponderations_destination = g_list_append(*ponderations_destination, ponderation);
            else
            {
                g_list_free(ponderation->elements);
                free(ponderation);
            }
            list_parcours = g_list_next(list_parcours);
        }
        while (list_parcours != NULL);
    //     FinPour
    }
    // FinPour
    
    return TRUE;
}


gboolean _1990_ponderations_genere_eu(Projet *projet)
/* Description : Génération de l'ensemble des pondérations selon la norme européenne. La
 *                 fonction _1990_ponderations_genere_un est appelé autant de fois que
 *                 nécessaire avec les coefficients min/max ajustées en fonction des valeur de
 *                 la norme européenne et de la nature de l'état limite recherché. Les options
 *                 de calculs sont définies dans la variable projet->combinaisons.flags et
 *                 doivent être définies.
 *                 Les indices ont les définitions suivantes : pp = poids propre,
 *                 p = précontrainte, var = variable, acc = accidentelle et sis = sismique.
 * Paramètres : Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL,
 *             en cas d'erreur due à une fonction interne.
 */
{
    double      coef_min[ACTION_INCONNUE], coef_max[ACTION_INCONNUE];
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    
    // Pour ELU_EQU, générer les pondérations suivantes :
    //     Si à l'équilibre seulement Alors
    //         coefficient charges variables prédominante : 1.
    //         coefficient charges variables d'accompagnement : psi0.
    //         min_pp = 0.9, min_p = 1.0, min_var = 0.0, min_acc = 0.0, min_sis = 0.0
    //         max_pp = 1.1, max_p = 1.3, max_var = 1.5, max_acc = 0.0, max_sis = 0.0
    if ((projet->combinaisons.flags & 1) == 0)
    {
        coef_min[ACTION_POIDS_PROPRE]  = 0.9;  coef_max[ACTION_POIDS_PROPRE]  = 1.1;
        coef_min[ACTION_PRECONTRAINTE] = 1.0;  coef_max[ACTION_PRECONTRAINTE] = 1.3;
        coef_min[ACTION_VARIABLE]      = 0.0;  coef_max[ACTION_VARIABLE]      = 1.5;
        coef_min[ACTION_ACCIDENTELLE]  = 0.0;  coef_max[ACTION_ACCIDENTELLE]  = 0.0;
        coef_min[ACTION_SISMIQUE]      = 0.0;  coef_max[ACTION_SISMIQUE]      = 0.0;
        BUG(_1990_ponderations_genere_un(projet, &projet->combinaisons.elu_equ, coef_min, coef_max, 5, -1, 0), FALSE);
    }
    //     Sinon (à l'équilibre et à la résistance structurelle)
    //         coefficient charges variables prédominante : 1.
    //         coefficient charges variables d'accompagnement : psi0.
    //         min_pp = 1.15, min_p = 1.0, min_var = 0.0, min_acc = 0.0, min_sis = 0.0
    //         max_pp = 1.35, max_p = 1.3, max_var = 1.5, max_acc = 0.0, max_sis = 0.0
    //         et
    //         coefficient charges variables prédominante : 1.
    //         coefficient charges variables d'accompagnement : psi0.
    //         min_pp = 1.00, min_p = 1.0, min_var = 0.0, min_acc = 0.0, min_sis = 0.0
    //         max_pp = 1.00, max_p = 1.3, max_var = 1.5, max_acc = 0.0, max_sis = 0.0
    //     FinSinon
    // FinPour
    else
    {
        coef_min[ACTION_POIDS_PROPRE]  = 1.15; coef_max[ACTION_POIDS_PROPRE]  = 1.35;
        coef_min[ACTION_PRECONTRAINTE] = 1.0;  coef_max[ACTION_PRECONTRAINTE] = 1.3;
        coef_min[ACTION_VARIABLE]      = 0.0;  coef_max[ACTION_VARIABLE]      = 1.5;
        coef_min[ACTION_ACCIDENTELLE]  = 0.0;  coef_max[ACTION_ACCIDENTELLE]  = 0.0;
        coef_min[ACTION_SISMIQUE]      = 0.0;  coef_max[ACTION_SISMIQUE]      = 0.0;
        BUG(_1990_ponderations_genere_un(projet, &projet->combinaisons.elu_equ, coef_min, coef_max, 5, -1, 0), FALSE);
        coef_min[ACTION_POIDS_PROPRE]  = 1.0;  coef_max[ACTION_POIDS_PROPRE]  = 1.0;
        coef_min[ACTION_PRECONTRAINTE] = 1.0;  coef_max[ACTION_PRECONTRAINTE] = 1.3;
        coef_min[ACTION_VARIABLE]      = 0.0;  coef_max[ACTION_VARIABLE]      = 1.5;
        coef_min[ACTION_ACCIDENTELLE]  = 0.0;  coef_max[ACTION_ACCIDENTELLE]  = 0.0;
        coef_min[ACTION_SISMIQUE]      = 0.0;  coef_max[ACTION_SISMIQUE]      = 0.0;
        BUG(_1990_ponderations_genere_un(projet, &projet->combinaisons.elu_equ, coef_min, coef_max, 5, -1, 0), FALSE);
    }
    // Si utilisation des formules 6.10a et 6.10b de l'Eurocode 0 Alors
    if ((projet->combinaisons.flags & 8) == 0)
    {
        switch (projet->combinaisons.flags & 6)
        {
    //     Si selon l'approche 1 Alors
    //         Pour ELU_STR ET ELU_GEO, générer les pondérations suivantes :
    //             coefficient charges variables prédominante : psi0.
    //             coefficient charges variables d'accompagnement : psi0.
    //             min_pp = 1.00, min_p = 1.0, min_var = 0.0, min_acc = 0.0, min_sis = 0.0
    //             max_pp = 1.35, max_p = 1.3, max_var = 1.5, max_acc = 0.0, max_sis = 0.0
    //             et
    //             coefficient charges variables prédominante : 1.
    //             coefficient charges variables d'accompagnement : psi0.
    //             min_pp = 1.00, min_p = 1.0, min_var = 0.0, min_acc = 0.0, min_sis = 0.0
    //             max_pp = 1.15, max_p = 1.3, max_var = 1.5, max_acc = 0.0, max_sis = 0.0
    //             et
    //             coefficient charges variables prédominante : 1.
    //             coefficient charges variables d'accompagnement : psi0.
    //             min_pp = 1.00, min_p = 1.0, min_var = 0.0, min_acc = 0.0, min_sis = 0.0
    //             max_pp = 1.00, max_p = 1.3, max_var = 1.3, max_acc = 0.0, max_sis = 0.0
            case 0:
            {
                coef_min[ACTION_POIDS_PROPRE]  = 1.0;  coef_max[ACTION_POIDS_PROPRE]  = 1.35;
                coef_min[ACTION_PRECONTRAINTE] = 1.0;  coef_max[ACTION_PRECONTRAINTE] = 1.3;
                coef_min[ACTION_VARIABLE]      = 0.0;  coef_max[ACTION_VARIABLE]      = 1.5;
                coef_min[ACTION_ACCIDENTELLE]  = 0.0;  coef_max[ACTION_ACCIDENTELLE]  = 0.0;
                coef_min[ACTION_SISMIQUE]      = 0.0;  coef_max[ACTION_SISMIQUE]      = 0.0;
                BUG(_1990_ponderations_genere_un(projet, &projet->combinaisons.elu_str, coef_min, coef_max, 5, 0, 0), FALSE);
                coef_min[ACTION_POIDS_PROPRE]  = 1.0;  coef_max[ACTION_POIDS_PROPRE]  = 1.15;
                coef_min[ACTION_PRECONTRAINTE] = 1.0;  coef_max[ACTION_PRECONTRAINTE] = 1.3;
                coef_min[ACTION_VARIABLE]      = 0.0;  coef_max[ACTION_VARIABLE]      = 1.5;
                coef_min[ACTION_ACCIDENTELLE]  = 0.0;  coef_max[ACTION_ACCIDENTELLE]  = 0.0;
                coef_min[ACTION_SISMIQUE]      = 0.0;  coef_max[ACTION_SISMIQUE]      = 0.0;
                BUG(_1990_ponderations_genere_un(projet, &projet->combinaisons.elu_str, coef_min, coef_max, 5, -1, 0), FALSE);
                coef_min[ACTION_POIDS_PROPRE]  = 1.0;  coef_max[ACTION_POIDS_PROPRE]  = 1.0;
                coef_min[ACTION_PRECONTRAINTE] = 1.0;  coef_max[ACTION_PRECONTRAINTE] = 1.3;
                coef_min[ACTION_VARIABLE]      = 0.0;  coef_max[ACTION_VARIABLE]      = 1.3;
                coef_min[ACTION_ACCIDENTELLE]  = 0.0;  coef_max[ACTION_ACCIDENTELLE]  = 0.0;
                coef_min[ACTION_SISMIQUE]      = 0.0;  coef_max[ACTION_SISMIQUE]      = 0.0;
                BUG(_1990_ponderations_genere_un(projet, &projet->combinaisons.elu_str, coef_min, coef_max, 5, -1, 0), FALSE);
                BUG(_1990_ponderations_duplique_sans_double(&projet->combinaisons.elu_geo, projet->combinaisons.elu_str), FALSE);
                break;
            }
    //     Sinon Si selon l'approche 2 Alors
    //         Pour ELU_STR ET ELU_GEO, générer les pondérations suivantes :
    //             coefficient charges variables prédominante : psi0.
    //             coefficient charges variables d'accompagnement : psi0.
    //             min_pp = 1.00, min_p = 1.0, min_var = 0.0, min_acc = 0.0, min_sis = 0.0
    //             max_pp = 1.35, max_p = 1.3, max_var = 1.5, max_acc = 0.0, max_sis = 0.0
    //             et
    //             coefficient charges variables prédominante : 1.
    //             coefficient charges variables d'accompagnement : psi0.
    //             min_pp = 1.00, min_p = 1.0, min_var = 0.0, min_acc = 0.0, min_sis = 0.0
    //             max_pp = 1.15, max_p = 1.3, max_var = 1.5, max_acc = 0.0, max_sis = 0.0
            case 2:
            {
                coef_min[ACTION_POIDS_PROPRE]  = 1.0;  coef_max[ACTION_POIDS_PROPRE]  = 1.35;
                coef_min[ACTION_PRECONTRAINTE] = 1.0;  coef_max[ACTION_PRECONTRAINTE] = 1.3;
                coef_min[ACTION_VARIABLE]      = 0.0;  coef_max[ACTION_VARIABLE]      = 1.5;
                coef_min[ACTION_ACCIDENTELLE]  = 0.0;  coef_max[ACTION_ACCIDENTELLE]  = 0.0;
                coef_min[ACTION_SISMIQUE]      = 0.0;  coef_max[ACTION_SISMIQUE]      = 0.0;
                BUG(_1990_ponderations_genere_un(projet, &projet->combinaisons.elu_str, coef_min, coef_max, 5, 0, 0), FALSE);
                coef_min[ACTION_POIDS_PROPRE]  = 1.0;  coef_max[ACTION_POIDS_PROPRE]  = 1.15;
                coef_min[ACTION_PRECONTRAINTE] = 1.0;  coef_max[ACTION_PRECONTRAINTE] = 1.3;
                coef_min[ACTION_VARIABLE]      = 0.0;  coef_max[ACTION_VARIABLE]      = 1.5;
                coef_min[ACTION_ACCIDENTELLE]  = 0.0;  coef_max[ACTION_ACCIDENTELLE]  = 0.0;
                coef_min[ACTION_SISMIQUE]      = 0.0;  coef_max[ACTION_SISMIQUE]      = 0.0;
                BUG(_1990_ponderations_genere_un(projet, &projet->combinaisons.elu_str, coef_min, coef_max, 5, -1, 0), FALSE);
                BUG(_1990_ponderations_duplique_sans_double(&projet->combinaisons.elu_geo, projet->combinaisons.elu_str), FALSE);
                break;
            }
    //     Si selon l'approche 3 Alors
    //         Pour ELU_STR, générer les pondérations suivantes :
    //             coefficient charges variables prédominante : psi0.
    //             coefficient charges variables d'accompagnement : psi0.
    //             min_pp = 1.00, min_p = 1.0, min_var = 0.0, min_acc = 0.0, min_sis = 0.0
    //             max_pp = 1.35, max_p = 1.3, max_var = 1.5, max_acc = 0.0, max_sis = 0.0
    //             et
    //             coefficient charges variables prédominante : 1.
    //             coefficient charges variables d'accompagnement : psi0.
    //             min_pp = 1.00, min_p = 1.0, min_var = 0.0, min_acc = 0.0, min_sis = 0.0
    //             max_pp = 1.15, max_p = 1.3, max_var = 1.5, max_acc = 0.0, max_sis = 0.0
            case 4:
            {
                coef_min[ACTION_POIDS_PROPRE]  = 1.0;  coef_max[ACTION_POIDS_PROPRE]  = 1.35;
                coef_min[ACTION_PRECONTRAINTE] = 1.0;  coef_max[ACTION_PRECONTRAINTE] = 1.3;
                coef_min[ACTION_VARIABLE]      = 0.0;  coef_max[ACTION_VARIABLE]      = 1.5;
                coef_min[ACTION_ACCIDENTELLE]  = 0.0;  coef_max[ACTION_ACCIDENTELLE]  = 0.0;
                coef_min[ACTION_SISMIQUE]      = 0.0;  coef_max[ACTION_SISMIQUE]      = 0.0;
                BUG(_1990_ponderations_genere_un(projet, &projet->combinaisons.elu_str, coef_min, coef_max, 5, 0, 0), FALSE);
                coef_min[ACTION_POIDS_PROPRE]  = 1.0;  coef_max[ACTION_POIDS_PROPRE]  = 1.15;
                coef_min[ACTION_PRECONTRAINTE] = 1.0;  coef_max[ACTION_PRECONTRAINTE] = 1.3;
                coef_min[ACTION_VARIABLE]      = 0.0;  coef_max[ACTION_VARIABLE]      = 1.5;
                coef_min[ACTION_ACCIDENTELLE]  = 0.0;  coef_max[ACTION_ACCIDENTELLE]  = 0.0;
                coef_min[ACTION_SISMIQUE]      = 0.0;  coef_max[ACTION_SISMIQUE]      = 0.0;
                BUG(_1990_ponderations_genere_un(projet, &projet->combinaisons.elu_str, coef_min, coef_max, 5, -1, 0), FALSE);
    //         Pour ELU_GEO, générer les pondérations suivantes :
    //             coefficient charges variables prédominante : 1.
    //             coefficient charges variables d'accompagnement : psi0.
    //             min_pp = 1.00, min_p = 1.0, min_var = 0.0, min_acc = 0.0, min_sis = 0.0
    //             max_pp = 1.00, max_p = 1.3, max_var = 1.3, max_acc = 0.0, max_sis = 0.0
                coef_min[ACTION_POIDS_PROPRE]  = 1.0;  coef_max[ACTION_POIDS_PROPRE]  = 1.0;
                coef_min[ACTION_PRECONTRAINTE] = 1.0;  coef_max[ACTION_PRECONTRAINTE] = 1.3;
                coef_min[ACTION_VARIABLE]      = 0.0;  coef_max[ACTION_VARIABLE]      = 1.3;
                coef_min[ACTION_ACCIDENTELLE]  = 0.0;  coef_max[ACTION_ACCIDENTELLE]  = 0.0;
                coef_min[ACTION_SISMIQUE]      = 0.0;  coef_max[ACTION_SISMIQUE]      = 0.0;
                BUG(_1990_ponderations_genere_un(projet, &projet->combinaisons.elu_geo, coef_min, coef_max, 5, -1, 0), FALSE);
                break;
    //     FinSi
            }
            default:
            {
                BUGMSG(0, FALSE, gettext("Flag %d inconnu.\n"), projet->combinaisons.flags);
                break;
            }
        }
    }
    // Si utilisation de la formule 6.10 de l'Eurocode 0 Alors
    else
    {
        switch (projet->combinaisons.flags & 6)
        {
    //     Si selon l'approche 1 Alors
    //         Pour ELU_STR ET ELU_GEO, générer les pondérations suivantes :
    //             coefficient charges variables prédominante : 1.
    //             coefficient charges variables d'accompagnement : psi0.
    //             min_pp = 1.00, min_p = 1.0, min_var = 0.0, min_acc = 0.0, min_sis = 0.0
    //             max_pp = 1.35, max_p = 1.3, max_var = 1.5, max_acc = 0.0, max_sis = 0.0
    //             et
    //             coefficient charges variables prédominante : 1.
    //             coefficient charges variables d'accompagnement : psi0.
    //             min_pp = 1.00, min_p = 1.0, min_var = 0.0, min_acc = 0.0, min_sis = 0.0
    //             max_pp = 1.00, max_p = 1.3, max_var = 1.3, max_acc = 0.0, max_sis = 0.0
            case 0:
            {
                coef_min[ACTION_POIDS_PROPRE]  = 1.0;  coef_max[ACTION_POIDS_PROPRE]  = 1.35;
                coef_min[ACTION_PRECONTRAINTE] = 1.0;  coef_max[ACTION_PRECONTRAINTE] = 1.3;
                coef_min[ACTION_VARIABLE]      = 0.0;  coef_max[ACTION_VARIABLE]      = 1.5;
                coef_min[ACTION_ACCIDENTELLE]  = 0.0;  coef_max[ACTION_ACCIDENTELLE]  = 0.0;
                coef_min[ACTION_SISMIQUE]      = 0.0;  coef_max[ACTION_SISMIQUE]      = 0.0;
                BUG(_1990_ponderations_genere_un(projet, &projet->combinaisons.elu_str, coef_min, coef_max, 5, -1, 0), FALSE);
                coef_min[ACTION_POIDS_PROPRE]  = 1.0;  coef_max[ACTION_POIDS_PROPRE]  = 1.0;
                coef_min[ACTION_PRECONTRAINTE] = 1.0;  coef_max[ACTION_PRECONTRAINTE] = 1.3;
                coef_min[ACTION_VARIABLE]      = 0.0;  coef_max[ACTION_VARIABLE]      = 1.3;
                coef_min[ACTION_ACCIDENTELLE]  = 0.0;  coef_max[ACTION_ACCIDENTELLE]  = 0.0;
                coef_min[ACTION_SISMIQUE]      = 0.0;  coef_max[ACTION_SISMIQUE]      = 0.0;
                BUG(_1990_ponderations_genere_un(projet, &projet->combinaisons.elu_str, coef_min, coef_max, 5, -1, 0), FALSE);
                BUG(_1990_ponderations_duplique_sans_double(&projet->combinaisons.elu_geo, projet->combinaisons.elu_str), FALSE);
                break;
            }
    //     Sinon Si selon l'approche 2 Alors
    //         Pour ELU_STR ET ELU_GEO, générer les pondérations suivantes :
    //             coefficient charges variables prédominante : 1.
    //             coefficient charges variables d'accompagnement : psi0.
    //             min_pp = 1.00, min_p = 1.0, min_var = 0.0, min_acc = 0.0, min_sis = 0.0
    //             max_pp = 1.35, max_p = 1.3, max_var = 1.5, max_acc = 0.0, max_sis = 0.0
            case 2:
            {
                coef_min[ACTION_POIDS_PROPRE]  = 1.0;  coef_max[ACTION_POIDS_PROPRE]  = 1.35;
                coef_min[ACTION_PRECONTRAINTE] = 1.0;  coef_max[ACTION_PRECONTRAINTE] = 1.3;
                coef_min[ACTION_VARIABLE]      = 0.0;  coef_max[ACTION_VARIABLE]      = 1.5;
                coef_min[ACTION_ACCIDENTELLE]  = 0.0;  coef_max[ACTION_ACCIDENTELLE]  = 0.0;
                coef_min[ACTION_SISMIQUE]      = 0.0;  coef_max[ACTION_SISMIQUE]      = 0.0;
                BUG(_1990_ponderations_genere_un(projet, &projet->combinaisons.elu_str, coef_min, coef_max, 5, -1, 0), FALSE);
                BUG(_1990_ponderations_duplique_sans_double(&projet->combinaisons.elu_geo, projet->combinaisons.elu_str), FALSE);
                break;
            }
    //     Si selon l'approche 3 Alors
    //         Pour ELU_STR, générer les pondérations suivantes :
    //             coefficient charges variables prédominante : 1.
    //             coefficient charges variables d'accompagnement : psi0.
    //             min_pp = 1.00, min_p = 1.0, min_var = 0.0, min_acc = 0.0, min_sis = 0.0
    //             max_pp = 1.35, max_p = 1.3, max_var = 1.5, max_acc = 0.0, max_sis = 0.0
            case 4:
            {
                coef_min[ACTION_POIDS_PROPRE]  = 1.0;  coef_max[ACTION_POIDS_PROPRE]  = 1.35;
                coef_min[ACTION_PRECONTRAINTE] = 1.0;  coef_max[ACTION_PRECONTRAINTE] = 1.3;
                coef_min[ACTION_VARIABLE]      = 0.0;  coef_max[ACTION_VARIABLE]      = 1.5;
                coef_min[ACTION_ACCIDENTELLE]  = 0.0;  coef_max[ACTION_ACCIDENTELLE]  = 0.0;
                coef_min[ACTION_SISMIQUE]      = 0.0;  coef_max[ACTION_SISMIQUE]      = 0.0;
                BUG(_1990_ponderations_genere_un(projet, &projet->combinaisons.elu_str, coef_min, coef_max, 5, -1, 0), FALSE);
    //         Pour ELU_GEO, générer les pondérations suivantes :
    //             coefficient charges variables prédominante : 1.
    //             coefficient charges variables d'accompagnement : psi0.
    //             min_pp = 1.00, min_p = 1.0, min_var = 0.0, min_acc = 0.0, min_sis = 0.0
    //             max_pp = 1.00, max_p = 1.3, max_var = 1.3, max_acc = 0.0, max_sis = 0.0
                coef_min[ACTION_POIDS_PROPRE]  = 1.0;  coef_max[ACTION_POIDS_PROPRE]  = 1.0;
                coef_min[ACTION_PRECONTRAINTE] = 1.0;  coef_max[ACTION_PRECONTRAINTE] = 1.3;
                coef_min[ACTION_VARIABLE]      = 0.0;  coef_max[ACTION_VARIABLE]      = 1.3;
                coef_min[ACTION_ACCIDENTELLE]  = 0.0;  coef_max[ACTION_ACCIDENTELLE]  = 0.0;
                coef_min[ACTION_SISMIQUE]      = 0.0;  coef_max[ACTION_SISMIQUE]      = 0.0;
                BUG(_1990_ponderations_genere_un(projet, &projet->combinaisons.elu_geo, coef_min, coef_max, 5, -1, 0), FALSE);
                break;
            }
    //     FinSi
            default :
            {
                BUGMSG(0, FALSE, gettext("Flag %d inconnu.\n"), projet->combinaisons.flags);
                break;
            }
        }
    }
    // FinSi
    
    coef_min[ACTION_POIDS_PROPRE]  = 1.0;  coef_max[ACTION_POIDS_PROPRE]  = 1.0;
    coef_min[ACTION_PRECONTRAINTE] = 1.0;  coef_max[ACTION_PRECONTRAINTE] = 1.0;
    coef_min[ACTION_VARIABLE]      = 0.0;  coef_max[ACTION_VARIABLE]      = 1.0;
    coef_min[ACTION_ACCIDENTELLE]  = 1.0;  coef_max[ACTION_ACCIDENTELLE]  = 1.0;
    coef_min[ACTION_SISMIQUE]      = 0.0;  coef_max[ACTION_SISMIQUE]      = 0.0;
    // Pour ELU_ACC, générer les pondérations suivantes :
    //     min_pp = 1.00, min_p = 1.0, min_var = 0.0, min_acc = 1.0, min_sis = 0.0
    //     max_pp = 1.00, max_p = 1.0, max_var = 1.0, max_acc = 1.0, max_sis = 0.0
    //     Si coefficient psi1 pour les actions accidentelles Alors
    //         coefficient charges variables prédominante : psi1.
    //     Sinon
    //         coefficient charges variables prédominante : psi2.
    //     FinSi
    //     coefficient charges variables d'accompagnement : psi2.
    // FinPour
    if ((projet->combinaisons.flags & 16) == 0)
        BUG(_1990_ponderations_genere_un(projet, &projet->combinaisons.elu_acc, coef_min, coef_max, 5, 1, 2), FALSE);
    else
        BUG(_1990_ponderations_genere_un(projet, &projet->combinaisons.elu_acc, coef_min, coef_max, 5, 2, 2), FALSE);
    
    // Pour ELU_SIS, générer les pondérations suivantes :
    //     min_pp = 1.00, min_p = 1.0, min_var = 0.0, min_acc = 0.0, min_sis = 1.0
    //     max_pp = 1.00, max_p = 1.0, max_var = 1.0, max_acc = 0.0, max_sis = 1.0
    //     coefficient charges variables prédominante : psi2.
    //     coefficient charges variables d'accompagnement : psi2.
    // FinPour
    coef_min[ACTION_POIDS_PROPRE]  = 1.0;  coef_max[ACTION_POIDS_PROPRE]  = 1.0;
    coef_min[ACTION_PRECONTRAINTE] = 1.0;  coef_max[ACTION_PRECONTRAINTE] = 1.0;
    coef_min[ACTION_VARIABLE]      = 0.0;  coef_max[ACTION_VARIABLE]      = 1.0;
    coef_min[ACTION_ACCIDENTELLE]  = 0.0;  coef_max[ACTION_ACCIDENTELLE]  = 0.0;
    coef_min[ACTION_SISMIQUE]      = 1.0;  coef_max[ACTION_SISMIQUE]      = 1.0;
    BUG(_1990_ponderations_genere_un(projet, &projet->combinaisons.elu_sis, coef_min, coef_max, 5, 2, 2), FALSE);
    
    // Pour ELU_CAR, générer les pondérations suivantes :
    //     min_pp = 1.00, min_p = 1.0, min_var = 0.0, min_acc = 0.0, min_sis = 1.0
    //     max_pp = 1.00, max_p = 1.0, max_var = 1.0, max_acc = 0.0, max_sis = 1.0
    //     coefficient charges variables prédominante : 1.
    //     coefficient charges variables d'accompagnement : psi0.
    // FinPour
    coef_min[ACTION_POIDS_PROPRE]  = 1.0;  coef_max[ACTION_POIDS_PROPRE]  = 1.0;
    coef_min[ACTION_PRECONTRAINTE] = 1.0;  coef_max[ACTION_PRECONTRAINTE] = 1.0;
    coef_min[ACTION_VARIABLE]      = 0.0;  coef_max[ACTION_VARIABLE]      = 1.0;
    coef_min[ACTION_ACCIDENTELLE]  = 0.0;  coef_max[ACTION_ACCIDENTELLE]  = 0.0;
    coef_min[ACTION_SISMIQUE]      = 0.0;  coef_max[ACTION_SISMIQUE]      = 0.0;
    BUG(_1990_ponderations_genere_un(projet, &projet->combinaisons.els_car, coef_min, coef_max, 5, -1, 0), FALSE);
    
    // Pour ELU_FREQ, générer les pondérations suivantes :
    //     min_pp = 1.00, min_p = 1.0, min_var = 0.0, min_acc = 0.0, min_sis = 1.0
    //     max_pp = 1.00, max_p = 1.0, max_var = 1.0, max_acc = 0.0, max_sis = 1.0
    //     coefficient charges variables prédominante : psi1.
    //     coefficient charges variables d'accompagnement : psi2.
    // FinPour
    coef_min[ACTION_POIDS_PROPRE]  = 1.0;  coef_max[ACTION_POIDS_PROPRE]  = 1.0;
    coef_min[ACTION_PRECONTRAINTE] = 1.0;  coef_max[ACTION_PRECONTRAINTE] = 1.0;
    coef_min[ACTION_VARIABLE]      = 0.0;  coef_max[ACTION_VARIABLE]      = 1.0;
    coef_min[ACTION_ACCIDENTELLE]  = 0.0;  coef_max[ACTION_ACCIDENTELLE]  = 0.0;
    coef_min[ACTION_SISMIQUE]      = 0.0;  coef_max[ACTION_SISMIQUE]      = 0.0;
    BUG(_1990_ponderations_genere_un(projet, &projet->combinaisons.els_freq, coef_min, coef_max, 5, 1, 2), FALSE);
    
    // Pour ELU_PERM, générer les pondérations suivantes :
    //     min_pp = 1.00, min_p = 1.0, min_var = 0.0, min_acc = 0.0, min_sis = 1.0
    //     max_pp = 1.00, max_p = 1.0, max_var = 1.0, max_acc = 0.0, max_sis = 1.0
    //     coefficient charges variables prédominante : psi2.
    //     coefficient charges variables d'accompagnement : psi2.
    // FinPour
    coef_min[ACTION_POIDS_PROPRE]  = 1.0;  coef_max[ACTION_POIDS_PROPRE]  = 1.0;
    coef_min[ACTION_PRECONTRAINTE] = 1.0;  coef_max[ACTION_PRECONTRAINTE] = 1.0;
    coef_min[ACTION_VARIABLE]      = 0.0;  coef_max[ACTION_VARIABLE]      = 1.0;
    coef_min[ACTION_ACCIDENTELLE]  = 0.0;  coef_max[ACTION_ACCIDENTELLE]  = 0.0;
    coef_min[ACTION_SISMIQUE]      = 0.0;  coef_max[ACTION_SISMIQUE]      = 0.0;
    BUG(_1990_ponderations_genere_un(projet, &projet->combinaisons.els_perm, coef_min, coef_max, 5, 2, 2), FALSE);
    
    return TRUE;
}


gboolean _1990_ponderations_genere_fr(Projet *projet)
/* Description : Génération de l'ensemble des pondérations selon la norme française. La
 *                 fonction _1990_ponderations_genere_un est appelé autant de fois que
 *                 nécessaire avec les coefficients min/max ajustées en fonction des valeur de
 *                 la norme française et de la nature de l'état limite recherché. Les options
 *                 de calculs sont définies dans la variable projet->combinaisons.flags et
 *                 doivent être définies.
 *                 Les indices ont les définitions suivantes : pp = poids propre,
 *                 p = précontrainte, var = variable, acc = accidentelle et sis = sismique,
 *                 es = eaux souterraines.
 * Paramètres : Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : FALSE
 *   Échec : TRUE :
 *             projet == NULL,
 *             en cas d'erreur due à une fonction interne.
 */
{
    double      coef_min[ACTION_INCONNUE], coef_max[ACTION_INCONNUE];
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    
    // Pour ELU_EQU, générer les pondérations suivantes :
    //     Si à l'équilibre seulement Alors
    //         coefficient charges variables prédominante : 1.
    //         coefficient charges variables d'accompagnement : psi0.
    //         min_pp = 0.9 min_p = 1.0 min_var = 0.0 min_acc = 0.0 min_sis = 0.0 min_es = 0.0
    //         max_pp = 1.1 max_p = 1.3 max_var = 1.5 max_acc = 0.0 max_sis = 0.0 max_es = 1.2
    if ((projet->combinaisons.flags & 1) == 0)
    {
        coef_min[ACTION_POIDS_PROPRE]  = 0.9;  coef_max[ACTION_POIDS_PROPRE]  = 1.1;
        coef_min[ACTION_PRECONTRAINTE] = 1.0;  coef_max[ACTION_PRECONTRAINTE] = 1.3;
        coef_min[ACTION_VARIABLE]      = 0.0;  coef_max[ACTION_VARIABLE]      = 1.5;
        coef_min[ACTION_ACCIDENTELLE]  = 0.0;  coef_max[ACTION_ACCIDENTELLE]  = 0.0;
        coef_min[ACTION_SISMIQUE]      = 0.0;  coef_max[ACTION_SISMIQUE]      = 0.0;
        coef_min[ACTION_EAUX_SOUTERRAINES] = 0.0;  coef_max[ACTION_EAUX_SOUTERRAINES] = 1.2;
        BUG(_1990_ponderations_genere_un(projet, &projet->combinaisons.elu_equ, coef_min, coef_max, 6, -1, 0), FALSE);
    }
    else
    //     Sinon (à l'équilibre et à la résistance structurelle)
    //         coefficient charges variables prédominante : 1.
    //         coefficient charges variables d'accompagnement : psi0.
    //         min_pp = 1.15 min_p = 1.0 min_var = 0.0 min_acc = 0.0 min_sis = 0.0 min_es = 0.0
    //         max_pp = 1.35 max_p = 1.3 max_var = 1.5 max_acc = 0.0 max_sis = 0.0 max_es = 1.2
    //         et
    //         coefficient charges variables prédominante : 1.
    //         coefficient charges variables d'accompagnement : psi0.
    //         min_pp = 1.00 min_p = 1.0 min_var = 0.0 min_acc = 0.0 min_sis = 0.0 min_es = 0.0
    //         max_pp = 1.00 max_p = 1.3 max_var = 1.5 max_acc = 0.0 max_sis = 0.0 max_es = 1.2
    //     FinSinon
    // FinPour
    {
        coef_min[ACTION_POIDS_PROPRE]  = 1.15;  coef_max[ACTION_POIDS_PROPRE]  = 1.35;
        coef_min[ACTION_PRECONTRAINTE] = 1.0;  coef_max[ACTION_PRECONTRAINTE] = 1.3;
        coef_min[ACTION_VARIABLE]      = 0.0;  coef_max[ACTION_VARIABLE]      = 1.5;
        coef_min[ACTION_ACCIDENTELLE]  = 0.0;  coef_max[ACTION_ACCIDENTELLE]  = 0.0;
        coef_min[ACTION_SISMIQUE]      = 0.0;  coef_max[ACTION_SISMIQUE]      = 0.0;
        coef_min[ACTION_EAUX_SOUTERRAINES] = 0.0;  coef_max[ACTION_EAUX_SOUTERRAINES] = 1.2;
        BUG(_1990_ponderations_genere_un(projet, &projet->combinaisons.elu_equ, coef_min, coef_max, 6, -1, 0), FALSE);
        coef_min[ACTION_POIDS_PROPRE]  = 1.0;  coef_max[ACTION_POIDS_PROPRE]  = 1.0;
        coef_min[ACTION_PRECONTRAINTE] = 1.0;  coef_max[ACTION_PRECONTRAINTE] = 1.3;
        coef_min[ACTION_VARIABLE]      = 0.0;  coef_max[ACTION_VARIABLE]      = 1.5;
        coef_min[ACTION_ACCIDENTELLE]  = 0.0;  coef_max[ACTION_ACCIDENTELLE]  = 0.0;
        coef_min[ACTION_SISMIQUE]      = 0.0;  coef_max[ACTION_SISMIQUE]      = 1.0;
        coef_min[ACTION_EAUX_SOUTERRAINES] = 0.0;  coef_max[ACTION_EAUX_SOUTERRAINES] = 1.2;
        BUG(_1990_ponderations_genere_un(projet, &projet->combinaisons.elu_equ, coef_min, coef_max, 6, -1, 0), FALSE);
    }
    // Si utilisation des formules 6.10a et 6.10b de l'Eurocode 0 Alors
    if ((projet->combinaisons.flags & 8) == 0)
    {
        switch (projet->combinaisons.flags & 6)
        {
    //     Si selon l'approche 1 Alors
    //         Pour ELU_STR ET ELU_GEO, générer les pondérations suivantes :
    //         coefficient charges variables prédominante : psi0.
    //         coefficient charges variables d'accompagnement : psi0.
    //         min_pp = 1.00 min_p = 1.0 min_var = 0.0 min_acc = 0.0 min_sis = 0.0 min_es = 0.0
    //         max_pp = 1.35 max_p = 1.3 max_var = 1.5 max_acc = 0.0 max_sis = 0.0 max_es = 1.2
    //         et
    //         coefficient charges variables prédominante : 1.
    //         coefficient charges variables d'accompagnement : psi0.
    //         min_pp = 1.00 min_p = 1.0 min_var = 0.0 min_acc = 0.0 min_sis = 0.0 min_es = 0.0
    //         max_pp = 1.15 max_p = 1.3 max_var = 1.5 max_acc = 0.0 max_sis = 0.0 max_es = 1.2
    //         et
    //         coefficient charges variables prédominante : 1.
    //         coefficient charges variables d'accompagnement : psi0.
    //         min_pp = 1.00 min_p = 1.0 min_var = 0.0 min_acc = 0.0 min_sis = 0.0 min_es = 0.0
    //         max_pp = 1.00 max_p = 1.3 max_var = 1.5 max_acc = 0.0 max_sis = 0.0 max_es = 1.2
            case 0:
            {
                coef_min[ACTION_POIDS_PROPRE]  = 1.0;  coef_max[ACTION_POIDS_PROPRE]  = 1.35;
                coef_min[ACTION_PRECONTRAINTE] = 1.0;  coef_max[ACTION_PRECONTRAINTE] = 1.3;
                coef_min[ACTION_VARIABLE]      = 0.0;  coef_max[ACTION_VARIABLE]      = 1.5;
                coef_min[ACTION_ACCIDENTELLE]  = 0.0;  coef_max[ACTION_ACCIDENTELLE]  = 0.0;
                coef_min[ACTION_SISMIQUE]      = 0.0;  coef_max[ACTION_SISMIQUE]      = 0.0;
                coef_min[ACTION_EAUX_SOUTERRAINES] = 0.0;  coef_max[ACTION_EAUX_SOUTERRAINES] = 1.2;
                BUG(_1990_ponderations_genere_un(projet, &projet->combinaisons.elu_str, coef_min, coef_max, 6, 0, 0), FALSE);
                coef_min[ACTION_POIDS_PROPRE]  = 1.0;  coef_max[ACTION_POIDS_PROPRE]  = 1.15;
                coef_min[ACTION_PRECONTRAINTE] = 1.0;  coef_max[ACTION_PRECONTRAINTE] = 1.3;
                coef_min[ACTION_VARIABLE]      = 0.0;  coef_max[ACTION_VARIABLE]      = 1.5;
                coef_min[ACTION_ACCIDENTELLE]  = 0.0;  coef_max[ACTION_ACCIDENTELLE]  = 0.0;
                coef_min[ACTION_SISMIQUE]      = 0.0;  coef_max[ACTION_SISMIQUE]      = 0.0;
                coef_min[ACTION_EAUX_SOUTERRAINES] = 0.0;  coef_max[ACTION_EAUX_SOUTERRAINES] = 1.2;
                BUG(_1990_ponderations_genere_un(projet, &projet->combinaisons.elu_str, coef_min, coef_max, 6, -1, 0), FALSE);
                coef_min[ACTION_POIDS_PROPRE]  = 1.0;  coef_max[ACTION_POIDS_PROPRE]  = 1.0;
                coef_min[ACTION_PRECONTRAINTE] = 1.0;  coef_max[ACTION_PRECONTRAINTE] = 1.3;
                coef_min[ACTION_VARIABLE]      = 0.0;  coef_max[ACTION_VARIABLE]      = 1.3;
                coef_min[ACTION_ACCIDENTELLE]  = 0.0;  coef_max[ACTION_ACCIDENTELLE]  = 0.0;
                coef_min[ACTION_SISMIQUE]      = 0.0;  coef_max[ACTION_SISMIQUE]      = 0.0;
                coef_min[ACTION_EAUX_SOUTERRAINES] = 0.0;  coef_max[ACTION_EAUX_SOUTERRAINES] = 1.2;
                BUG(_1990_ponderations_genere_un(projet, &projet->combinaisons.elu_str, coef_min, coef_max, 6, -1, 0), FALSE);
                BUG(_1990_ponderations_duplique_sans_double(&projet->combinaisons.elu_geo, projet->combinaisons.elu_str), FALSE);
                break;
            }
    //     Sinon Si selon l'approche 2 Alors
    //         Pour ELU_STR ET ELU_GEO, générer les pondérations suivantes :
    //         coefficient charges variables prédominante : psi0.
    //         coefficient charges variables d'accompagnement : psi0.
    //         min_pp = 1.00 min_p = 1.0 min_var = 0.0 min_acc = 0.0 min_sis = 0.0 min_es = 0.0
    //         max_pp = 1.35 max_p = 1.3 max_var = 1.5 max_acc = 0.0 max_sis = 0.0 max_es = 1.2
    //         et
    //         coefficient charges variables prédominante : 1.
    //         coefficient charges variables d'accompagnement : psi0.
    //         min_pp = 1.00 min_p = 1.0 min_var = 0.0 min_acc = 0.0 min_sis = 0.0 min_es = 0.0
    //         max_pp = 1.15 max_p = 1.3 max_var = 1.5 max_acc = 0.0 max_sis = 0.0 max_es = 1.2
            case 2:
            {
                coef_min[ACTION_POIDS_PROPRE]  = 1.0;  coef_max[ACTION_POIDS_PROPRE]  = 1.35;
                coef_min[ACTION_PRECONTRAINTE] = 1.0;  coef_max[ACTION_PRECONTRAINTE] = 1.3;
                coef_min[ACTION_VARIABLE]      = 0.0;  coef_max[ACTION_VARIABLE]      = 1.5;
                coef_min[ACTION_ACCIDENTELLE]  = 0.0;  coef_max[ACTION_ACCIDENTELLE]  = 0.0;
                coef_min[ACTION_SISMIQUE]      = 0.0;  coef_max[ACTION_SISMIQUE]      = 0.0;
                coef_min[ACTION_EAUX_SOUTERRAINES] = 0.0;  coef_max[ACTION_EAUX_SOUTERRAINES] = 1.2;
                BUG(_1990_ponderations_genere_un(projet, &projet->combinaisons.elu_str, coef_min, coef_max, 6, 0, 0), FALSE);
                coef_min[ACTION_POIDS_PROPRE]  = 1.0;  coef_max[ACTION_POIDS_PROPRE]  = 1.15;
                coef_min[ACTION_PRECONTRAINTE] = 1.0;  coef_max[ACTION_PRECONTRAINTE] = 1.3;
                coef_min[ACTION_VARIABLE]      = 0.0;  coef_max[ACTION_VARIABLE]      = 1.5;
                coef_min[ACTION_ACCIDENTELLE]  = 0.0;  coef_max[ACTION_ACCIDENTELLE]  = 0.0;
                coef_min[ACTION_SISMIQUE]      = 0.0;  coef_max[ACTION_SISMIQUE]      = 0.0;
                coef_min[ACTION_EAUX_SOUTERRAINES] = 0.0;  coef_max[ACTION_EAUX_SOUTERRAINES] = 1.2;
                BUG(_1990_ponderations_genere_un(projet, &projet->combinaisons.elu_str, coef_min, coef_max, 6, -1, 0), FALSE);
                BUG(_1990_ponderations_duplique_sans_double(&projet->combinaisons.elu_geo, projet->combinaisons.elu_str), FALSE);
                break;
            }
    //     Si selon l'approche 3 Alors
    //         Pour ELU_STR, générer les pondérations suivantes :
    //         coefficient charges variables prédominante : psi0.
    //         coefficient charges variables d'accompagnement : psi0.
    //         min_pp = 1.00 min_p = 1.0 min_var = 0.0 min_acc = 0.0 min_sis = 0.0 min_es = 0.0
    //         max_pp = 1.35 max_p = 1.3 max_var = 1.5 max_acc = 0.0 max_sis = 0.0 max_es = 1.2
    //         et
    //         coefficient charges variables prédominante : 1.
    //         coefficient charges variables d'accompagnement : psi0.
    //         min_pp = 1.00 min_p = 1.0 min_var = 0.0 min_acc = 0.0 min_sis = 0.0 min_es = 0.0
    //         max_pp = 1.15 max_p = 1.3 max_var = 1.5 max_acc = 0.0 max_sis = 0.0 max_es = 1.2
            case 4:
            {
                coef_min[ACTION_POIDS_PROPRE]  = 1.0;  coef_max[ACTION_POIDS_PROPRE]  = 1.35;
                coef_min[ACTION_PRECONTRAINTE] = 1.0;  coef_max[ACTION_PRECONTRAINTE] = 1.3;
                coef_min[ACTION_VARIABLE]      = 0.0;  coef_max[ACTION_VARIABLE]      = 1.5;
                coef_min[ACTION_ACCIDENTELLE]  = 0.0;  coef_max[ACTION_ACCIDENTELLE]  = 0.0;
                coef_min[ACTION_SISMIQUE]      = 0.0;  coef_max[ACTION_SISMIQUE]      = 0.0;
                coef_min[ACTION_EAUX_SOUTERRAINES] = 0.0;  coef_max[ACTION_EAUX_SOUTERRAINES] = 1.2;
                BUG(_1990_ponderations_genere_un(projet, &projet->combinaisons.elu_str, coef_min, coef_max, 6, 0, 0), FALSE);
                coef_min[ACTION_POIDS_PROPRE]  = 1.0;  coef_max[ACTION_POIDS_PROPRE]  = 1.15;
                coef_min[ACTION_PRECONTRAINTE] = 1.0;  coef_max[ACTION_PRECONTRAINTE] = 1.3;
                coef_min[ACTION_VARIABLE]      = 0.0;  coef_max[ACTION_VARIABLE]      = 1.5;
                coef_min[ACTION_ACCIDENTELLE]  = 0.0;  coef_max[ACTION_ACCIDENTELLE]  = 0.0;
                coef_min[ACTION_SISMIQUE]      = 0.0;  coef_max[ACTION_SISMIQUE]      = 0.0;
                coef_min[ACTION_EAUX_SOUTERRAINES] = 0.0;  coef_max[ACTION_EAUX_SOUTERRAINES] = 1.2;
                BUG(_1990_ponderations_genere_un(projet, &projet->combinaisons.elu_str, coef_min, coef_max, 6, -1, 0), FALSE);
    //         Pour ELU_GEO, générer les pondérations suivantes :
    //         coefficient charges variables prédominante : 1.
    //         coefficient charges variables d'accompagnement : psi0.
    //         min_pp = 1.00 min_p = 1.0 min_var = 0.0 min_acc = 0.0 min_sis = 0.0 min_es = 0.0
    //         max_pp = 1.00 max_p = 1.3 max_var = 1.3 max_acc = 0.0 max_sis = 0.0 max_es = 1.2
                coef_min[ACTION_POIDS_PROPRE]  = 1.0;  coef_max[ACTION_POIDS_PROPRE]  = 1.0;
                coef_min[ACTION_PRECONTRAINTE] = 1.0;  coef_max[ACTION_PRECONTRAINTE] = 1.3;
                coef_min[ACTION_VARIABLE]      = 0.0;  coef_max[ACTION_VARIABLE]      = 1.3;
                coef_min[ACTION_ACCIDENTELLE]  = 0.0;  coef_max[ACTION_ACCIDENTELLE]  = 0.0;
                coef_min[ACTION_SISMIQUE]      = 0.0;  coef_max[ACTION_SISMIQUE]      = 0.0;
                coef_min[ACTION_EAUX_SOUTERRAINES] = 0.0;  coef_max[ACTION_EAUX_SOUTERRAINES] = 1.2;
                BUG(_1990_ponderations_genere_un(projet, &projet->combinaisons.elu_geo, coef_min, coef_max, 6, -1, 0), FALSE);
                break;
    //     FinSi
            }
            default :
            {
                BUGMSG(0, FALSE, gettext("Flag %d inconnu.\n"), projet->combinaisons.flags);
                break;
            }
        }
    }
    // Si utilisation de la formule 6.10 de l'Eurocode 0 Alors
    else
    // équation 6.10
    {
        switch (projet->combinaisons.flags & 6)
        {
    //     Si selon l'approche 1 Alors
    //         Pour ELU_STR ET ELU_GEO, générer les pondérations suivantes :
    //         coefficient charges variables prédominante : 1.
    //         coefficient charges variables d'accompagnement : psi0.
    //         min_pp = 1.00 min_p = 1.0 min_var = 0.0 min_acc = 0.0 min_sis = 0.0 min_es = 0.0
    //         max_pp = 1.35 max_p = 1.3 max_var = 1.5 max_acc = 0.0 max_sis = 0.0 max_es = 1.2
    //         et
    //         coefficient charges variables prédominante : 1.
    //         coefficient charges variables d'accompagnement : psi0.
    //         min_pp = 1.00 min_p = 1.0 min_var = 0.0 min_acc = 0.0 min_sis = 0.0 min_es = 0.0
    //         max_pp = 1.00 max_p = 1.3 max_var = 1.3 max_acc = 0.0 max_sis = 0.0 max_es = 1.2
            case 0:
            {
                coef_min[ACTION_POIDS_PROPRE]  = 1.0;  coef_max[ACTION_POIDS_PROPRE]  = 1.35;
                coef_min[ACTION_PRECONTRAINTE] = 1.0;  coef_max[ACTION_PRECONTRAINTE] = 1.3;
                coef_min[ACTION_VARIABLE]      = 0.0;  coef_max[ACTION_VARIABLE]      = 1.5;
                coef_min[ACTION_ACCIDENTELLE]  = 0.0;  coef_max[ACTION_ACCIDENTELLE]  = 0.0;
                coef_min[ACTION_SISMIQUE]      = 0.0;  coef_max[ACTION_SISMIQUE]      = 0.0;
                coef_min[ACTION_EAUX_SOUTERRAINES] = 0.0;  coef_max[ACTION_EAUX_SOUTERRAINES] = 1.2;
                BUG(_1990_ponderations_genere_un(projet, &projet->combinaisons.elu_str, coef_min, coef_max, 6, -1, 0), FALSE);
                coef_min[ACTION_POIDS_PROPRE]  = 1.0;  coef_max[ACTION_POIDS_PROPRE]  = 1.0;
                coef_min[ACTION_PRECONTRAINTE] = 1.0;  coef_max[ACTION_PRECONTRAINTE] = 1.3;
                coef_min[ACTION_VARIABLE]      = 0.0;  coef_max[ACTION_VARIABLE]      = 1.3;
                coef_min[ACTION_ACCIDENTELLE]  = 0.0;  coef_max[ACTION_ACCIDENTELLE]  = 0.0;
                coef_min[ACTION_SISMIQUE]      = 0.0;  coef_max[ACTION_SISMIQUE]      = 0.0;
                coef_min[ACTION_EAUX_SOUTERRAINES] = 0.0;  coef_max[ACTION_EAUX_SOUTERRAINES] = 1.2;
                BUG(_1990_ponderations_genere_un(projet, &projet->combinaisons.elu_str, coef_min, coef_max, 6, -1, 0), FALSE);
                BUG(_1990_ponderations_duplique_sans_double(&projet->combinaisons.elu_geo, projet->combinaisons.elu_str), FALSE);
                break;
            }
    //     Sinon Si selon l'approche 2 Alors
    //         Pour ELU_STR ET ELU_GEO, générer les pondérations suivantes :
    //         coefficient charges variables prédominante : 1.
    //         coefficient charges variables d'accompagnement : psi0.
    //         min_pp = 1.00 min_p = 1.0 min_var = 0.0 min_acc = 0.0 min_sis = 0.0 min_es = 0.0
    //         max_pp = 1.35 max_p = 1.3 max_var = 1.5 max_acc = 0.0 max_sis = 0.0 max_es = 1.2
            case 2:
            {
                coef_min[ACTION_POIDS_PROPRE]  = 1.0;  coef_max[ACTION_POIDS_PROPRE]  = 1.35;
                coef_min[ACTION_PRECONTRAINTE] = 1.0;  coef_max[ACTION_PRECONTRAINTE] = 1.3;
                coef_min[ACTION_VARIABLE]      = 0.0;  coef_max[ACTION_VARIABLE]      = 1.5;
                coef_min[ACTION_ACCIDENTELLE]  = 0.0;  coef_max[ACTION_ACCIDENTELLE]  = 0.0;
                coef_min[ACTION_SISMIQUE]      = 0.0;  coef_max[ACTION_SISMIQUE]      = 0.0;
                coef_min[ACTION_EAUX_SOUTERRAINES] = 0.0;  coef_max[ACTION_EAUX_SOUTERRAINES] = 1.2;
                BUG(_1990_ponderations_genere_un(projet, &projet->combinaisons.elu_str, coef_min, coef_max, 6, -1, 0), FALSE);
                BUG(_1990_ponderations_duplique_sans_double(&projet->combinaisons.elu_geo, projet->combinaisons.elu_str), FALSE);
                break;
            }
    //     Si selon l'approche 3 Alors
    //         Pour ELU_STR, générer les pondérations suivantes :
    //         coefficient charges variables prédominante : 1.
    //         coefficient charges variables d'accompagnement : psi0.
    //         min_pp = 1.00 min_p = 1.0 min_var = 0.0 min_acc = 0.0 min_sis = 0.0 min_es = 0.0
    //         max_pp = 1.35 max_p = 1.3 max_var = 1.5 max_acc = 0.0 max_sis = 0.0 max_es = 1.2
            case 4:
            {
                coef_min[ACTION_POIDS_PROPRE]  = 1.0;  coef_max[ACTION_POIDS_PROPRE]  = 1.35;
                coef_min[ACTION_PRECONTRAINTE] = 1.0;  coef_max[ACTION_PRECONTRAINTE] = 1.3;
                coef_min[ACTION_VARIABLE]      = 0.0;  coef_max[ACTION_VARIABLE]      = 1.5;
                coef_min[ACTION_ACCIDENTELLE]  = 0.0;  coef_max[ACTION_ACCIDENTELLE]  = 0.0;
                coef_min[ACTION_SISMIQUE]      = 0.0;  coef_max[ACTION_SISMIQUE]      = 0.0;
                coef_min[ACTION_EAUX_SOUTERRAINES] = 0.0;  coef_max[ACTION_EAUX_SOUTERRAINES] = 1.2;
                BUG(_1990_ponderations_genere_un(projet, &projet->combinaisons.elu_str, coef_min, coef_max, 6, -1, 0), FALSE);
    //         Pour ELU_GEO, générer les pondérations suivantes :
    //         coefficient charges variables prédominante : 1.
    //         coefficient charges variables d'accompagnement : psi0.
    //         min_pp = 1.00 min_p = 1.0 min_var = 0.0 min_acc = 0.0 min_sis = 0.0 min_es = 0.0
    //         max_pp = 1.00 max_p = 1.3 max_var = 1.3 max_acc = 0.0 max_sis = 0.0 max_es = 1.2
                coef_min[ACTION_POIDS_PROPRE]  = 1.0;  coef_max[ACTION_POIDS_PROPRE]  = 1.0;
                coef_min[ACTION_PRECONTRAINTE] = 1.0;  coef_max[ACTION_PRECONTRAINTE] = 1.3;
                coef_min[ACTION_VARIABLE]      = 0.0;  coef_max[ACTION_VARIABLE]      = 1.3;
                coef_min[ACTION_ACCIDENTELLE]  = 0.0;  coef_max[ACTION_ACCIDENTELLE]  = 0.0;
                coef_min[ACTION_SISMIQUE]      = 0.0;  coef_max[ACTION_SISMIQUE]      = 0.0;
                coef_min[ACTION_EAUX_SOUTERRAINES] = 0.0;  coef_max[ACTION_EAUX_SOUTERRAINES] = 1.2;
                BUG(_1990_ponderations_genere_un(projet, &projet->combinaisons.elu_geo, coef_min, coef_max, 6, -1, 0), FALSE);
                break;
            }
    //     FinSi
            default :
            {
                BUGMSG(0, FALSE, gettext("Flag %d inconnu.\n"), projet->combinaisons.flags);
                break;
            }
        }
    }
    // FinSi
    
    coef_min[ACTION_POIDS_PROPRE]  = 1.0;  coef_max[ACTION_POIDS_PROPRE]  = 1.0;
    coef_min[ACTION_PRECONTRAINTE] = 1.0;  coef_max[ACTION_PRECONTRAINTE] = 1.0;
    coef_min[ACTION_VARIABLE]      = 0.0;  coef_max[ACTION_VARIABLE]      = 1.0;
    coef_min[ACTION_ACCIDENTELLE]  = 1.0;  coef_max[ACTION_ACCIDENTELLE]  = 1.0;
    coef_min[ACTION_SISMIQUE]      = 0.0;  coef_max[ACTION_SISMIQUE]      = 0.0;
    coef_min[ACTION_EAUX_SOUTERRAINES] = 0.0;  coef_max[ACTION_EAUX_SOUTERRAINES] = 1.0;
    // Pour ELU_ACC, générer les pondérations suivantes :
    //     min_pp = 1.00 min_p = 1.0 min_var = 0.0 min_acc = 1.0 min_sis = 0.0 min_es = 0.0
    //     max_pp = 1.00 max_p = 1.0 max_var = 1.0 max_acc = 1.0 max_sis = 0.0 max_es = 1.0
    //     Si coefficient psi1 pour les actions accidentelles Alors
    //         coefficient charges variables prédominante : psi1.
    //     Sinon
    //         coefficient charges variables prédominante : psi2.
    //     FinSi
    //     coefficient charges variables d'accompagnement : psi2.
    // FinPour
    if ((projet->combinaisons.flags & 16) == 0)
        BUG(_1990_ponderations_genere_un(projet, &projet->combinaisons.elu_acc, coef_min, coef_max, 6, 1, 2), FALSE);
    else
        BUG(_1990_ponderations_genere_un(projet, &projet->combinaisons.elu_acc, coef_min, coef_max, 6, 2, 2), FALSE);
    
    // Pour ELU_SIS, générer les pondérations suivantes :
    //     coefficient charges variables prédominante : psi2.
    //     coefficient charges variables d'accompagnement : psi2.
    //     min_pp = 1.00 min_p = 1.0 min_var = 0.0 min_acc = 0.0 min_sis = 1.0 min_es = 0.0
    //     max_pp = 1.00 max_p = 1.0 max_var = 1.0 max_acc = 0.0 max_sis = 1.0 max_es = 1.0
    // FinPour
    coef_min[ACTION_POIDS_PROPRE]  = 1.0;  coef_max[ACTION_POIDS_PROPRE]  = 1.0;
    coef_min[ACTION_PRECONTRAINTE] = 1.0;  coef_max[ACTION_PRECONTRAINTE] = 1.0;
    coef_min[ACTION_VARIABLE]      = 0.0;  coef_max[ACTION_VARIABLE]      = 1.0;
    coef_min[ACTION_ACCIDENTELLE]  = 0.0;  coef_max[ACTION_ACCIDENTELLE]  = 0.0;
    coef_min[ACTION_SISMIQUE]      = 1.0;  coef_max[ACTION_SISMIQUE]      = 1.0;
    coef_min[ACTION_EAUX_SOUTERRAINES] = 0.0;  coef_max[ACTION_EAUX_SOUTERRAINES] = 1.0;
    BUG(_1990_ponderations_genere_un(projet, &projet->combinaisons.elu_acc, coef_min, coef_max, 6, 2, 2), FALSE);
    
    // Pour ELS_CAR, générer les pondérations suivantes :
    //     coefficient charges variables prédominante : 1.
    //     coefficient charges variables d'accompagnement : psi0.
    //     min_pp = 1.00 min_p = 1.0 min_var = 0.0 min_acc = 0.0 min_sis = 0.0 min_es = 0.0
    //     max_pp = 1.00 max_p = 1.0 max_var = 1.0 max_acc = 0.0 max_sis = 0.0 max_es = 1.2
    // FinPour
    coef_min[ACTION_POIDS_PROPRE]  = 1.0;  coef_max[ACTION_POIDS_PROPRE]  = 1.0;
    coef_min[ACTION_PRECONTRAINTE] = 1.0;  coef_max[ACTION_PRECONTRAINTE] = 1.0;
    coef_min[ACTION_VARIABLE]      = 0.0;  coef_max[ACTION_VARIABLE]      = 1.0;
    coef_min[ACTION_ACCIDENTELLE]  = 0.0;  coef_max[ACTION_ACCIDENTELLE]  = 0.0;
    coef_min[ACTION_SISMIQUE]      = 0.0;  coef_max[ACTION_SISMIQUE]      = 0.0;
    coef_min[ACTION_EAUX_SOUTERRAINES] = 0.0;  coef_max[ACTION_EAUX_SOUTERRAINES] = 1.2;
    BUG(_1990_ponderations_genere_un(projet, &projet->combinaisons.els_car, coef_min, coef_max, 6, -1, 0), FALSE);

    // Pour ELS_FREQ, générer les pondérations suivantes :
    //     coefficient charges variables prédominante : psi1.
    //     coefficient charges variables d'accompagnement : psi2.
    //     min_pp = 1.00 min_p = 1.0 min_var = 0.0 min_acc = 0.0 min_sis = 0.0 min_es = 0.0
    //     max_pp = 1.00 max_p = 1.0 max_var = 1.0 max_acc = 0.0 max_sis = 0.0 max_es = 1.0
    // FinPour
    coef_min[ACTION_POIDS_PROPRE]  = 1.0;  coef_max[ACTION_POIDS_PROPRE]  = 1.0;
    coef_min[ACTION_PRECONTRAINTE] = 1.0;  coef_max[ACTION_PRECONTRAINTE] = 1.0;
    coef_min[ACTION_VARIABLE]      = 0.0;  coef_max[ACTION_VARIABLE]      = 1.0;
    coef_min[ACTION_ACCIDENTELLE]  = 0.0;  coef_max[ACTION_ACCIDENTELLE]  = 0.0;
    coef_min[ACTION_SISMIQUE]      = 0.0;  coef_max[ACTION_SISMIQUE]      = 0.0;
    coef_min[ACTION_EAUX_SOUTERRAINES] = 0.0;  coef_max[ACTION_EAUX_SOUTERRAINES] = 1.0;
    BUG(_1990_ponderations_genere_un(projet, &projet->combinaisons.els_freq, coef_min, coef_max, 6, 1, 2), FALSE);

    // Pour ELS_PERM, générer les pondérations suivantes :
    //     coefficient charges variables prédominante : psi2.
    //     coefficient charges variables d'accompagnement : psi2.
    //     min_pp = 1.00 min_p = 1.0 min_var = 0.0 min_acc = 0.0 min_sis = 0.0 min_es = 0.0
    //     max_pp = 1.00 max_p = 1.0 max_var = 1.0 max_acc = 0.0 max_sis = 0.0 max_es = 1.0
    // FinPour
    coef_min[ACTION_POIDS_PROPRE]  = 1.0;  coef_max[ACTION_POIDS_PROPRE]  = 1.0;
    coef_min[ACTION_PRECONTRAINTE] = 1.0;  coef_max[ACTION_PRECONTRAINTE] = 1.0;
    coef_min[ACTION_VARIABLE]      = 0.0;  coef_max[ACTION_VARIABLE]      = 1.0;
    coef_min[ACTION_ACCIDENTELLE]  = 0.0;  coef_max[ACTION_ACCIDENTELLE]  = 0.0;
    coef_min[ACTION_SISMIQUE]      = 0.0;  coef_max[ACTION_SISMIQUE]      = 0.0;
    coef_min[ACTION_EAUX_SOUTERRAINES] = 0.0;  coef_max[ACTION_EAUX_SOUTERRAINES] = 1.0;
    BUG(_1990_ponderations_genere_un(projet, &projet->combinaisons.els_perm, coef_min, coef_max, 6, 2, 2), FALSE);
    
    return TRUE;
}


gboolean _1990_ponderations_genere(Projet *projet)
/* Description : Génération de l'ensemble des pondérations selon la norme du pays spécifié.
 *               cf. _1990_ponderations_genere_PAYS.
 * Paramètres : Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL,
 *             pays inconnu,
 *             erreur en cas de fonction interne.
 */
{
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    
    // Trivial
    switch (projet->pays)
    {
        case PAYS_EU : { return _1990_ponderations_genere_eu(projet); break; }
        case PAYS_FR : { return _1990_ponderations_genere_fr(projet); break; }
        default : { BUGMSG(0, FALSE, gettext("Pays %d inconnu.\n"), projet->pays); break; }
    }
}

void _1990_ponderations_affiche(GList *ponderations)
/* Description : Affiche les pondérations de la liste fournie en argument.
 * Paramètres : GList *ponderations : la liste des pondérations.
 * Valeur renvoyée : Aucun.
 */
{
    // Trivial
    if (ponderations != NULL)
    {
        GList   *list_parcours = ponderations;
        do
        {
            Ponderation *ponderation = list_parcours->data;
            
            if (ponderation->elements != NULL)
            {
                GList   *list_parcours2 = ponderation->elements;
                do
                {
                    Ponderation_Element *ponderation_element = list_parcours2->data;
                    if (g_list_next(list_parcours2) != NULL)
                        printf("%u*%f(%d)+", ponderation_element->action->numero+1, ponderation_element->ponderation, ponderation_element->psi);
                    else
                        printf("%u*%f(%d)", ponderation_element->action->numero+1, ponderation_element->ponderation, ponderation_element->psi);
                    list_parcours2 = g_list_next(list_parcours2);
                }
                while (list_parcours2 != NULL);
                printf("\n");
            }
            list_parcours = g_list_next(list_parcours);
        }
        while (list_parcours != NULL);
    }

    return;
}


G_MODULE_EXPORT gboolean _1990_ponderations_affiche_tout(Projet *projet)
/* Description : Affiche toutes les pondérations du projet.
 * Paramètres : Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL.
 */
{
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    
    // Trivial
    printf("elu_equ\n");
    _1990_ponderations_affiche(projet->combinaisons.elu_equ);
    printf("elu_str\n");
    _1990_ponderations_affiche(projet->combinaisons.elu_str);
    printf("elu_geo\n");
    _1990_ponderations_affiche(projet->combinaisons.elu_geo);
    printf("elu_fat\n");
    _1990_ponderations_affiche(projet->combinaisons.elu_fat);
    printf("elu_acc\n");
    _1990_ponderations_affiche(projet->combinaisons.elu_acc);
    printf("elu_sis\n");
    _1990_ponderations_affiche(projet->combinaisons.elu_sis);
    printf("els_car\n");
    _1990_ponderations_affiche(projet->combinaisons.els_car);
    printf("els_freq\n");
    _1990_ponderations_affiche(projet->combinaisons.els_freq);
    printf("els_perm\n");
    _1990_ponderations_affiche(projet->combinaisons.els_perm);
    
    return TRUE;
}
