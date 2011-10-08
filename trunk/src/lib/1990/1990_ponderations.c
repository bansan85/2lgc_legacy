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


int _1990_ponderations_verifie_double(LIST *liste_ponderations, Ponderation* pond_a_verifier)
/* Description : Vérifie dans la liste des ponderations si la ponderation à vérifier est déjà
 *                 présente.
 * Paramètres : LIST *ponderations : liste des pondérations
 *            : Ponderation* pond_a_verifier : pondération à vérifier
 * Valeur renvoyée :
 *   Succès : 0 si la pondération n'existe pas
 *          : 1 si la pondération existe
 *   Échec : -1 en cas de paramètres invalides :
 *             (liste_ponderations == NULL) ou
 *             (pond_a_verifier == NULL)
 */
{
    BUGMSG(liste_ponderations, -1, "_1990_ponderations_verifie_double\n");
    BUGMSG(pond_a_verifier, -1, "_1990_ponderations_verifie_double\n");
    
    if (list_size(liste_ponderations) == 0)
        return 0;
    
    /* En renvoyant ici 1, la fonction fait croire que la pondération existe.
     * En vérité, c'est surtout qu'une pondération sans élément n'est pas intéressante
     * à conserver, à la différence des combinaisons vides qui peuvent être utilisées
     * par les niveaux supérieurs */
    if (list_size(pond_a_verifier->elements) == 0)
        return 1;
    
    list_mvfront(liste_ponderations);
    do
    {
        int         doublon;
        Ponderation *ponderation;
        
        /* On pense que la pondération est identique jusqu'à preuve du contraire */
        doublon = 1;
        ponderation = (Ponderation*)list_curr(liste_ponderations);
        if (list_size(ponderation->elements) == list_size(pond_a_verifier->elements))
        {
            list_mvfront(ponderation->elements);
            list_mvfront(pond_a_verifier->elements);
            do
            {
                Ponderation_Element *elem1, *elem2;
                
                elem1 = (Ponderation_Element*)list_curr(ponderation->elements);
                elem2 = (Ponderation_Element*)list_curr(pond_a_verifier->elements);
                /* Preuve ici */
                if ((elem1->action != elem2->action) || (elem1->psi != elem2->psi) || (!(ERREUR_RELATIVE_EGALE(elem1->ponderation, elem2->ponderation))))
                    doublon = 0;
            }
            while ((list_mvnext(ponderation->elements) != NULL) && (list_mvnext(pond_a_verifier->elements) != NULL) && (doublon == 1));
            if (doublon == 1)
                return 1;
        }
    }
    while (list_mvnext(liste_ponderations) != NULL);
    
    return 0;
}

int _1990_ponderations_duplique_sans_double(LIST *liste_pond_destination, LIST *liste_pond_source)
/* Description : ajoute à une liste de pondérations existante une liste de pondérations.
 *                 une vérification est effectuée pour s'assurer que la liste source ne possède
 *                 pas une ou plusieurs pondérations identiques que la liste de destination.
 * Paramètres : LIST *liste_pond_destination : liste de ponderations qui recevra les
 *                ponderations sources
 *            : LIST *liste_pond_source : liste de ponderations source
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             
 */
{
    BUGMSG(liste_pond_destination, -1, "_1990_ponderations_duplique_sans_double\n");
    BUGMSG(liste_pond_source, -1, "_1990_ponderations_duplique_sans_double\n");
    
    if (list_size(liste_pond_source) == 0)
        return 0;
    
    list_mvrear(liste_pond_destination);
    list_mvfront(liste_pond_source);
    do
    {
        Ponderation     *ponderation_source;
        
        ponderation_source = (Ponderation*)list_curr(liste_pond_source);
        /* Si la ponderation n'existe pas, on l'ajoute à la fin */
        if (_1990_ponderations_verifie_double(liste_pond_destination, ponderation_source) == 0)
        {
            Ponderation     ponderation_destination;
            
            ponderation_destination.elements = list_init();
            if (ponderation_destination.elements == NULL)
                BUGMSG(0, -2, gettext("Erreur d'allocation mémoire.\n"));
            
            if ((ponderation_source != NULL) && (list_curr(ponderation_source->elements) != NULL))
            {
                list_mvfront(ponderation_source->elements);
                do
                {
                    Ponderation_Element *element_source;
                    Ponderation_Element element_destination;
                    
                    element_source = (Ponderation_Element*)list_curr(ponderation_source->elements);
                    element_destination.action = element_source->action;
                    element_destination.flags = element_source->flags;
                    element_destination.psi = element_source->psi;
                    element_destination.ponderation = element_source->ponderation;
                    if (list_insert_after(ponderation_destination.elements, (void*)&element_destination, sizeof(element_destination)) == NULL)
                        BUGMSG(0, -3, gettext("Erreur d'allocation mémoire.\n"));
                }
                while (list_mvnext(ponderation_source->elements) != NULL);
            }
            if (list_insert_after(liste_pond_destination, (void*)&ponderation_destination, sizeof(ponderation_destination)) == NULL)
                BUGMSG(0, -4, gettext("Erreur d'allocation mémoire.\n"));
        }
    }
    while (list_mvnext(liste_pond_source) != NULL);
    
    return 0;
}

int _1990_ponderations_genere_un(Projet *projet, LIST* ponderations_destination,
  double* coef_min, double* coef_max, int dim_coef, int psi_dominante, int psi_accompagnement)
/* Description : Génère l'ensemble des pondérations en fonction des paramètres d'entrées. Pour
 *                 une génération exaustive conformément à une norme, il est nécessaire
 *                 d'appeler directement la fonction _1990_ponderations_genere qui se chargera
 *                 d'appeler _1990_ponderations_genere_un autant de fois que nécessaire avec
 *                 des paramètres adaptés.
 * Paramètres : Projet *projet : la variable projet
 *            : LIST* combinaisons_destination : liste dans laquelle sera stockés les
 *                ponderations générées. Les résultats seront filtrés pour éviter les doublons.
 *            : double* coef_min : coefficients psi en situation favorable (min).
 *            : double* coef_max : coefficients psi en situation défavorable (max). L'indice du
 *                tableau à utiliser est celui renvoyé par _1990_action_categorie_bat.
 *            : int dim_coef : nombre d'incides dans le tableau de double coef_max et coef_min
 *            : int psi_dominante : indice du coefficient psi à utiliser pour l'action variable
 *                prédominante: 0 = psi0, 1 = psi1, 2 = psi et -1 = prendre la valeur 1.0.
 *            : int psi_accompagnement : indice du coefficient psi à utiliser pour les actions
 *                variables d'accompagnement : 0 = psi0, 1 = psi1, 2 = psi et
 *                -1 = prendre la valeur 1.0.
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet == NULL) ou
 *             (projet->niveaux_groupes == NULL) ou
 *             (list_size(projet->niveaux_groupes) == 0) ou
 *             (list_size(niveau->groupes) != 1) avec
 *               niveau = list_rear(projet->niveaux_groupes);
 *             (list_size(groupe->tmp_combinaison.combinaisons) == 0) avec
 *               groupe = list_front(niveau->groupes);
 *           -2 en cas d'erreur d'allocation mémoire
 */
{
    int             nbboucle, j;
    Groupe          *groupe;
    Niveau_Groupe   *niveau;
    
    BUGMSG(projet, -1, "_1990_ponderations_genere_un\n");
    BUGMSG(projet->niveaux_groupes, -1, "_1990_ponderations_genere_un\n");
    BUGMSG(list_size(projet->niveaux_groupes), -1, "_1990_ponderations_genere_un\n");
    
    // Si le dernier niveau ne possède pas un seul et unique groupe Alors
    //     Fin.
    // FinSi
    niveau = (Niveau_Groupe*)list_rear(projet->niveaux_groupes);
    if (list_size(niveau->groupes) != 1)
    {
        printf(gettext("La génération des pondérations est impossible.\nLe dernier niveau ne possède pas qu'un seul groupe.\n"));
        return -1;
    }
    groupe = (Groupe*)list_front(niveau->groupes);
    
    // Si le groupe du dernier niveau ne possède pas de combinaison Alors
    //     Fin.
    // FinSi
    if (list_size(groupe->tmp_combinaison.combinaisons) == 0)
    {
        printf(gettext("Le dernier niveau ne possède aucune combinaison permettant la génération des pondérations.\n"));
        return -1;
    }
    
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
    //     Pour chaque combinaison dans le groupe final Faire
        list_mvfront(groupe->tmp_combinaison.combinaisons);
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
            Ponderation ponderation;
            
            combinaison = (Combinaison*) list_curr(groupe->tmp_combinaison.combinaisons);
            ponderation.elements = list_init();
            BUGMSG(ponderation.elements, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "_1990_ponderations_genere_un");
            list_mvfront(combinaison->elements);
            if (list_size(combinaison->elements) != 0)
            {
    //         Pour chaque élément de la combinaison Faire
                do
                {
                    Ponderation_Element ponderation_element;
                    Combinaison_Element *combinaison_element;
                    int         categorie;
                    
                    combinaison_element = (Combinaison_Element*)list_curr(combinaison->elements);
                    ponderation_element.action = combinaison_element->action;
                    ponderation_element.flags = combinaison_element->flags;
    //             Vérifier la présente d'une action variable prédominante et d'une action
    //               variable d'accompagnement. Si oui, pondération ignorée.
                    categorie = _1990_action_categorie_bat(ponderation_element.action->type, projet->pays);
                    BUG(categorie != ACTION_INCONNUE, -1);
                    if (categorie == ACTION_VARIABLE)
                    {
                        variable_accompagnement = 1;
                        if ((ponderation_element.flags & 1) != 0)
                        {
                            variable_dominante = 1;
                            ponderation_element.psi = psi_dominante;
                        }
                        else
                            ponderation_element.psi = psi_accompagnement;
                    }
                    /* psi vaut toujours -1 s'il ne s'agit pas d'une action variable */
                    else
                        ponderation_element.psi = -1;
                    
    //             Vérification si le coefficient min et max de la catégorie vaut 0.
    //               Si oui, pondération ignorée.
                    if ((ERREUR_RELATIVE_EGALE(0., coef_min[categorie])) && (ERREUR_RELATIVE_EGALE(0., coef_max[categorie])))
                        suivant = 1;
                    else
                    {
       /* On affecte le coefficient min/max à la combinaison pour obtenir la pondération */
                        if ((j & (1 << categorie)) != 0)
                            ponderation_element.ponderation = coef_max[categorie];
                        else
                            ponderation_element.ponderation = coef_min[categorie];
                        if (!(ERREUR_RELATIVE_EGALE(0., ponderation_element.ponderation)))
                            BUGMSG(list_insert_after(ponderation.elements, &ponderation_element, sizeof(ponderation_element)), -2, gettext("%s : Erreur d'allocation mémoire.\n"), "_1990_ponderations_genere_un");
                    }
                }
                while ((list_mvnext(combinaison->elements) != NULL) && (suivant != 1));
    //         FinPour
            }
    //         Si la pondération n'est pas ignorée Alors
    //             Ajout à la liste des pondérations existante.
    //         FinSi
            if ((variable_accompagnement == 1) && (variable_dominante == 0))
                suivant = 1;
            if ((suivant == 0) && (list_size(ponderation.elements) != 0) && (_1990_ponderations_verifie_double(ponderations_destination, &ponderation) == 0))
                BUGMSG(list_insert_after(ponderations_destination, &ponderation, sizeof(ponderation)), -2, gettext("%s : Erreur d'allocation mémoire.\n"), "_1990_ponderations_genere_un");
            else
                list_free(ponderation.elements, LIST_DEALLOC);
        }
        while (list_mvnext(groupe->tmp_combinaison.combinaisons) != NULL);
    //     FinPour
    }
    // FinPour
    
    return 0;
}

int _1990_ponderations_genere_eu(Projet *projet)
/* Description : Génération de l'ensemble des pondérations selon la norme européenne. La
 *                 fonction _1990_ponderations_genere_un est appelé autant de fois que
 *                 nécessaire avec les coefficients min/max ajustées en fonction des valeur de
 *                 la norme européenne et de la nature de l'état limite recherché. Les options
 *                 de calculs sont définies dans la variable projet->combinaisons.flags et
 *                 doivent être définies.
 *                 Les indices ont les définitions suivantes : pp = poids propre,
 *                   p = précontrainte, var = variable, acc = accidentelle et sis = sismique.
 * Paramètres : Projet *projet : la variable projet
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet == NULL)
 *           -3 en cas d'erreur due à une fonction interne
 */
{
    double      coef_min[ACTION_INCONNUE], coef_max[ACTION_INCONNUE];
    
    BUGMSG(projet, -1, "_1990_ponderations_genere_eu\n");
    
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
        BUG(_1990_ponderations_genere_un(projet, projet->combinaisons.elu_equ, coef_min, coef_max, 5, -1, 0) == 0, -3);
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
        BUG(_1990_ponderations_genere_un(projet, projet->combinaisons.elu_equ, coef_min, coef_max, 5, -1, 0) == 0, -3);
        coef_min[ACTION_POIDS_PROPRE]  = 1.0;  coef_max[ACTION_POIDS_PROPRE]  = 1.0;
        coef_min[ACTION_PRECONTRAINTE] = 1.0;  coef_max[ACTION_PRECONTRAINTE] = 1.3;
        coef_min[ACTION_VARIABLE]      = 0.0;  coef_max[ACTION_VARIABLE]      = 1.5;
        coef_min[ACTION_ACCIDENTELLE]  = 0.0;  coef_max[ACTION_ACCIDENTELLE]  = 0.0;
        coef_min[ACTION_SISMIQUE]      = 0.0;  coef_max[ACTION_SISMIQUE]      = 0.0;
        BUG(_1990_ponderations_genere_un(projet, projet->combinaisons.elu_equ, coef_min, coef_max, 5, -1, 0) == 0, -3);
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
                BUG(_1990_ponderations_genere_un(projet, projet->combinaisons.elu_str, coef_min, coef_max, 5, 0, 0) == 0, -3);
                coef_min[ACTION_POIDS_PROPRE]  = 1.0;  coef_max[ACTION_POIDS_PROPRE]  = 1.15;
                coef_min[ACTION_PRECONTRAINTE] = 1.0;  coef_max[ACTION_PRECONTRAINTE] = 1.3;
                coef_min[ACTION_VARIABLE]      = 0.0;  coef_max[ACTION_VARIABLE]      = 1.5;
                coef_min[ACTION_ACCIDENTELLE]  = 0.0;  coef_max[ACTION_ACCIDENTELLE]  = 0.0;
                coef_min[ACTION_SISMIQUE]      = 0.0;  coef_max[ACTION_SISMIQUE]      = 0.0;
                BUG(_1990_ponderations_genere_un(projet, projet->combinaisons.elu_str, coef_min, coef_max, 5, -1, 0) == 0, -3);
                coef_min[ACTION_POIDS_PROPRE]  = 1.0;  coef_max[ACTION_POIDS_PROPRE]  = 1.0;
                coef_min[ACTION_PRECONTRAINTE] = 1.0;  coef_max[ACTION_PRECONTRAINTE] = 1.3;
                coef_min[ACTION_VARIABLE]      = 0.0;  coef_max[ACTION_VARIABLE]      = 1.3;
                coef_min[ACTION_ACCIDENTELLE]  = 0.0;  coef_max[ACTION_ACCIDENTELLE]  = 0.0;
                coef_min[ACTION_SISMIQUE]      = 0.0;  coef_max[ACTION_SISMIQUE]      = 0.0;
                BUG(_1990_ponderations_genere_un(projet, projet->combinaisons.elu_str, coef_min, coef_max, 5, -1, 0) == 0, -3);
                BUG(_1990_ponderations_duplique_sans_double(projet->combinaisons.elu_geo, projet->combinaisons.elu_str) == 0, -3);
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
                BUG(_1990_ponderations_genere_un(projet, projet->combinaisons.elu_str, coef_min, coef_max, 5, 0, 0) == 0, -3);
                coef_min[ACTION_POIDS_PROPRE]  = 1.0;  coef_max[ACTION_POIDS_PROPRE]  = 1.15;
                coef_min[ACTION_PRECONTRAINTE] = 1.0;  coef_max[ACTION_PRECONTRAINTE] = 1.3;
                coef_min[ACTION_VARIABLE]      = 0.0;  coef_max[ACTION_VARIABLE]      = 1.5;
                coef_min[ACTION_ACCIDENTELLE]  = 0.0;  coef_max[ACTION_ACCIDENTELLE]  = 0.0;
                coef_min[ACTION_SISMIQUE]      = 0.0;  coef_max[ACTION_SISMIQUE]      = 0.0;
                BUG(_1990_ponderations_genere_un(projet, projet->combinaisons.elu_str, coef_min, coef_max, 5, -1, 0) == 0, -3);
                BUG(_1990_ponderations_duplique_sans_double(projet->combinaisons.elu_geo, projet->combinaisons.elu_str) == 0, -3);
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
                BUG(_1990_ponderations_genere_un(projet, projet->combinaisons.elu_str, coef_min, coef_max, 5, 0, 0) == 0, -3);
                coef_min[ACTION_POIDS_PROPRE]  = 1.0;  coef_max[ACTION_POIDS_PROPRE]  = 1.15;
                coef_min[ACTION_PRECONTRAINTE] = 1.0;  coef_max[ACTION_PRECONTRAINTE] = 1.3;
                coef_min[ACTION_VARIABLE]      = 0.0;  coef_max[ACTION_VARIABLE]      = 1.5;
                coef_min[ACTION_ACCIDENTELLE]  = 0.0;  coef_max[ACTION_ACCIDENTELLE]  = 0.0;
                coef_min[ACTION_SISMIQUE]      = 0.0;  coef_max[ACTION_SISMIQUE]      = 0.0;
                BUG(_1990_ponderations_genere_un(projet, projet->combinaisons.elu_str, coef_min, coef_max, 5, -1, 0) == 0, -3);
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
                BUG(_1990_ponderations_genere_un(projet, projet->combinaisons.elu_geo, coef_min, coef_max, 5, -1, 0) == 0, -3);
                break;
    //     FinSi
            }
            default:
            {
                BUGMSG(0, -1, gettext("Paramètres invalides.\n"));
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
                BUG(_1990_ponderations_genere_un(projet, projet->combinaisons.elu_str, coef_min, coef_max, 5, -1, 0) == 0, -3);
                coef_min[ACTION_POIDS_PROPRE]  = 1.0;  coef_max[ACTION_POIDS_PROPRE]  = 1.0;
                coef_min[ACTION_PRECONTRAINTE] = 1.0;  coef_max[ACTION_PRECONTRAINTE] = 1.3;
                coef_min[ACTION_VARIABLE]      = 0.0;  coef_max[ACTION_VARIABLE]      = 1.3;
                coef_min[ACTION_ACCIDENTELLE]  = 0.0;  coef_max[ACTION_ACCIDENTELLE]  = 0.0;
                coef_min[ACTION_SISMIQUE]      = 0.0;  coef_max[ACTION_SISMIQUE]      = 0.0;
                BUG(_1990_ponderations_genere_un(projet, projet->combinaisons.elu_str, coef_min, coef_max, 5, -1, 0) == 0, -3);
                BUG(_1990_ponderations_duplique_sans_double(projet->combinaisons.elu_geo, projet->combinaisons.elu_str) == 0, -3);
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
                BUG(_1990_ponderations_genere_un(projet, projet->combinaisons.elu_str, coef_min, coef_max, 5, -1, 0) == 0, -3);
                BUG(_1990_ponderations_duplique_sans_double(projet->combinaisons.elu_geo, projet->combinaisons.elu_str) == 0, -3);
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
                BUG(_1990_ponderations_genere_un(projet, projet->combinaisons.elu_str, coef_min, coef_max, 5, -1, 0) == 0, -3);
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
                BUG(_1990_ponderations_genere_un(projet, projet->combinaisons.elu_geo, coef_min, coef_max, 5, -1, 0) == 0, -3);
                break;
            }
    //     FinSi
            default :
            {
                BUGMSG(0, -1, gettext("Paramètres invalides.\n"));
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
        BUG(_1990_ponderations_genere_un(projet, projet->combinaisons.elu_acc, coef_min, coef_max, 5, 1, 2) == 0, -3);
    else
        BUG(_1990_ponderations_genere_un(projet, projet->combinaisons.elu_acc, coef_min, coef_max, 5, 2, 2) == 0, -3);
    
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
    BUG(_1990_ponderations_genere_un(projet, projet->combinaisons.elu_sis, coef_min, coef_max, 5, 2, 2) == 0, -3);
    
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
    BUG(_1990_ponderations_genere_un(projet, projet->combinaisons.els_car, coef_min, coef_max, 5, -1, 0) == 0, -3);
    
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
    BUG(_1990_ponderations_genere_un(projet, projet->combinaisons.els_freq, coef_min, coef_max, 5, 1, 2) == 0, -3);
    
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
    BUG(_1990_ponderations_genere_un(projet, projet->combinaisons.els_perm, coef_min, coef_max, 5, 2, 2) == 0, -3);
    
    return 0;
}

int _1990_ponderations_genere_fr(Projet *projet)
/* Description : Génération de l'ensemble des pondérations selon la norme française. La
 *                 fonction _1990_ponderations_genere_un est appelé autant de fois que
 *                 nécessaire avec les coefficients min/max ajustées en fonction des valeur de
 *                 la norme française et de la nature de l'état limite recherché. Les options
 *                 de calculs sont définies dans la variable projet->combinaisons.flags et
 *                 doivent être définies.
 *                 Les indices ont les définitions suivantes : pp = poids propre,
 *                   p = précontrainte, var = variable, acc = accidentelle et sis = sismique,
 *                   es = eaux souterraines.
 * Paramètres : Projet *projet : la variable projet
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet == NULL)
 *           -3 en cas d'erreur due à une fonction interne
 */
{
    double      coef_min[ACTION_INCONNUE], coef_max[ACTION_INCONNUE];
    
    BUGMSG(projet, -1, "_1990_ponderations_genere_fr\n");
    
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
        BUG(_1990_ponderations_genere_un(projet, projet->combinaisons.elu_equ, coef_min, coef_max, 6, -1, 0) == 0, -3);
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
        BUG(_1990_ponderations_genere_un(projet, projet->combinaisons.elu_equ, coef_min, coef_max, 6, -1, 0) == 0, -3);
        coef_min[ACTION_POIDS_PROPRE]  = 1.0;  coef_max[ACTION_POIDS_PROPRE]  = 1.0;
        coef_min[ACTION_PRECONTRAINTE] = 1.0;  coef_max[ACTION_PRECONTRAINTE] = 1.3;
        coef_min[ACTION_VARIABLE]      = 0.0;  coef_max[ACTION_VARIABLE]      = 1.5;
        coef_min[ACTION_ACCIDENTELLE]  = 0.0;  coef_max[ACTION_ACCIDENTELLE]  = 0.0;
        coef_min[ACTION_SISMIQUE]      = 0.0;  coef_max[ACTION_SISMIQUE]      = 1.0;
        coef_min[ACTION_EAUX_SOUTERRAINES] = 0.0;  coef_max[ACTION_EAUX_SOUTERRAINES] = 1.2;
        BUG(_1990_ponderations_genere_un(projet, projet->combinaisons.elu_equ, coef_min, coef_max, 6, -1, 0) == 0, -3);
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
                BUG(_1990_ponderations_genere_un(projet, projet->combinaisons.elu_str, coef_min, coef_max, 6, 0, 0) == 0, -3);
                coef_min[ACTION_POIDS_PROPRE]  = 1.0;  coef_max[ACTION_POIDS_PROPRE]  = 1.15;
                coef_min[ACTION_PRECONTRAINTE] = 1.0;  coef_max[ACTION_PRECONTRAINTE] = 1.3;
                coef_min[ACTION_VARIABLE]      = 0.0;  coef_max[ACTION_VARIABLE]      = 1.5;
                coef_min[ACTION_ACCIDENTELLE]  = 0.0;  coef_max[ACTION_ACCIDENTELLE]  = 0.0;
                coef_min[ACTION_SISMIQUE]      = 0.0;  coef_max[ACTION_SISMIQUE]      = 0.0;
                coef_min[ACTION_EAUX_SOUTERRAINES] = 0.0;  coef_max[ACTION_EAUX_SOUTERRAINES] = 1.2;
                BUG(_1990_ponderations_genere_un(projet, projet->combinaisons.elu_str, coef_min, coef_max, 6, -1, 0) == 0, -3);
                coef_min[ACTION_POIDS_PROPRE]  = 1.0;  coef_max[ACTION_POIDS_PROPRE]  = 1.0;
                coef_min[ACTION_PRECONTRAINTE] = 1.0;  coef_max[ACTION_PRECONTRAINTE] = 1.3;
                coef_min[ACTION_VARIABLE]      = 0.0;  coef_max[ACTION_VARIABLE]      = 1.3;
                coef_min[ACTION_ACCIDENTELLE]  = 0.0;  coef_max[ACTION_ACCIDENTELLE]  = 0.0;
                coef_min[ACTION_SISMIQUE]      = 0.0;  coef_max[ACTION_SISMIQUE]      = 0.0;
                coef_min[ACTION_EAUX_SOUTERRAINES] = 0.0;  coef_max[ACTION_EAUX_SOUTERRAINES] = 1.2;
                BUG(_1990_ponderations_genere_un(projet, projet->combinaisons.elu_str, coef_min, coef_max, 6, -1, 0) == 0, -3);
                BUG(_1990_ponderations_duplique_sans_double(projet->combinaisons.elu_geo, projet->combinaisons.elu_str) == 0, -3);
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
                BUG(_1990_ponderations_genere_un(projet, projet->combinaisons.elu_str, coef_min, coef_max, 6, 0, 0) == 0, -3);
                coef_min[ACTION_POIDS_PROPRE]  = 1.0;  coef_max[ACTION_POIDS_PROPRE]  = 1.15;
                coef_min[ACTION_PRECONTRAINTE] = 1.0;  coef_max[ACTION_PRECONTRAINTE] = 1.3;
                coef_min[ACTION_VARIABLE]      = 0.0;  coef_max[ACTION_VARIABLE]      = 1.5;
                coef_min[ACTION_ACCIDENTELLE]  = 0.0;  coef_max[ACTION_ACCIDENTELLE]  = 0.0;
                coef_min[ACTION_SISMIQUE]      = 0.0;  coef_max[ACTION_SISMIQUE]      = 0.0;
                coef_min[ACTION_EAUX_SOUTERRAINES] = 0.0;  coef_max[ACTION_EAUX_SOUTERRAINES] = 1.2;
                BUG(_1990_ponderations_genere_un(projet, projet->combinaisons.elu_str, coef_min, coef_max, 6, -1, 0) == 0, -3);
                BUG(_1990_ponderations_duplique_sans_double(projet->combinaisons.elu_geo, projet->combinaisons.elu_str) == 0, -3);
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
                BUG(_1990_ponderations_genere_un(projet, projet->combinaisons.elu_str, coef_min, coef_max, 6, 0, 0) == 0, -3);
                coef_min[ACTION_POIDS_PROPRE]  = 1.0;  coef_max[ACTION_POIDS_PROPRE]  = 1.15;
                coef_min[ACTION_PRECONTRAINTE] = 1.0;  coef_max[ACTION_PRECONTRAINTE] = 1.3;
                coef_min[ACTION_VARIABLE]      = 0.0;  coef_max[ACTION_VARIABLE]      = 1.5;
                coef_min[ACTION_ACCIDENTELLE]  = 0.0;  coef_max[ACTION_ACCIDENTELLE]  = 0.0;
                coef_min[ACTION_SISMIQUE]      = 0.0;  coef_max[ACTION_SISMIQUE]      = 0.0;
                coef_min[ACTION_EAUX_SOUTERRAINES] = 0.0;  coef_max[ACTION_EAUX_SOUTERRAINES] = 1.2;
                BUG(_1990_ponderations_genere_un(projet, projet->combinaisons.elu_str, coef_min, coef_max, 6, -1, 0) == 0, -3);
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
                BUG(_1990_ponderations_genere_un(projet, projet->combinaisons.elu_geo, coef_min, coef_max, 6, -1, 0) == 0, -3);
                break;
    //     FinSi
            }
            default :
            {
                BUGMSG(0, -3, gettext("%s : Paramètres invalides.\n"), "_1990_ponderations_genere_fr");
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
                BUG(_1990_ponderations_genere_un(projet, projet->combinaisons.elu_str, coef_min, coef_max, 6, -1, 0) == 0, -3);
                coef_min[ACTION_POIDS_PROPRE]  = 1.0;  coef_max[ACTION_POIDS_PROPRE]  = 1.0;
                coef_min[ACTION_PRECONTRAINTE] = 1.0;  coef_max[ACTION_PRECONTRAINTE] = 1.3;
                coef_min[ACTION_VARIABLE]      = 0.0;  coef_max[ACTION_VARIABLE]      = 1.3;
                coef_min[ACTION_ACCIDENTELLE]  = 0.0;  coef_max[ACTION_ACCIDENTELLE]  = 0.0;
                coef_min[ACTION_SISMIQUE]      = 0.0;  coef_max[ACTION_SISMIQUE]      = 0.0;
                coef_min[ACTION_EAUX_SOUTERRAINES] = 0.0;  coef_max[ACTION_EAUX_SOUTERRAINES] = 1.2;
                BUG(_1990_ponderations_genere_un(projet, projet->combinaisons.elu_str, coef_min, coef_max, 6, -1, 0) == 0, -3);
                BUG(_1990_ponderations_duplique_sans_double(projet->combinaisons.elu_geo, projet->combinaisons.elu_str) == 0, -3);
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
                BUG(_1990_ponderations_genere_un(projet, projet->combinaisons.elu_str, coef_min, coef_max, 6, -1, 0) == 0, -3);
                BUG(_1990_ponderations_duplique_sans_double(projet->combinaisons.elu_geo, projet->combinaisons.elu_str) == 0, -3);
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
                BUG(_1990_ponderations_genere_un(projet, projet->combinaisons.elu_str, coef_min, coef_max, 6, -1, 0) == 0, -3);
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
                BUG(_1990_ponderations_genere_un(projet, projet->combinaisons.elu_geo, coef_min, coef_max, 6, -1, 0) == 0, -3);
                break;
            }
    //     FinSi
            default :
            {
                BUGMSG(0, -3, gettext("Paramètres invalides.\n"));
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
        BUG(_1990_ponderations_genere_un(projet, projet->combinaisons.elu_acc, coef_min, coef_max, 6, 1, 2) == 0, -3);
    else
        BUG(_1990_ponderations_genere_un(projet, projet->combinaisons.elu_acc, coef_min, coef_max, 6, 2, 2) == 0, -3);
    
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
    BUG(_1990_ponderations_genere_un(projet, projet->combinaisons.elu_acc, coef_min, coef_max, 6, 2, 2) == 0, -3);
    
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
    BUG(_1990_ponderations_genere_un(projet, projet->combinaisons.els_car, coef_min, coef_max, 6, -1, 0) == 0, -3);

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
    BUG(_1990_ponderations_genere_un(projet, projet->combinaisons.els_freq, coef_min, coef_max, 6, 1, 2) == 0, -3);

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
    BUG(_1990_ponderations_genere_un(projet, projet->combinaisons.els_perm, coef_min, coef_max, 6, 2, 2) == 0, -3);
    
    return 0;
}

int _1990_ponderations_genere(Projet *projet)
/* Description : Génération de l'ensemble des pondérations selon la norme du pays spécifié.
 *               cf. _1990_ponderations_genere_PAYS
 * Paramètres : Projet *projet : la variable projet
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : valeur négative en cas d'erreur
 */
{
    // Trivial
    switch (projet->pays)
    {
        case PAYS_EU : { return _1990_ponderations_genere_eu(projet); break; }
        case PAYS_FR : { return _1990_ponderations_genere_fr(projet); break; }
        default : { BUGMSG(0, -1, gettext("Paramètres invalides.\n")); break; }
    }
}

void _1990_ponderations_affiche(LIST *ponderations)
/* Description : Affiche les pondérations de la liste fournie en argument
 * Paramètres : LIST *ponderations : la liste des pondérations
 * Valeur renvoyée : Aucun
 */
{
    if ((ponderations != NULL) && (list_size(ponderations) != 0))
    {
        list_mvfront(ponderations);
        do
        {
            Ponderation *ponderation = (Ponderation*)list_curr(ponderations);
            
            if (list_size(ponderation->elements) != 0)
            {
                list_mvfront(ponderation->elements);
                do
                {
                    Ponderation_Element *ponderation_element = (Ponderation_Element*)list_curr(ponderation->elements);
                    printf("%d*%f(%d)+", ponderation_element->action->numero+1, ponderation_element->ponderation, ponderation_element->psi);
                }
                while (list_mvnext(ponderation->elements));
                printf("\n");
            }
        }
        while (list_mvnext(ponderations));
    }

    return;
}

void _1990_ponderations_affiche_tout(Projet *projet)
/* Description : Affiche toutes les pondérations du projet
 * Paramètres : Projet *projet : la variable projet
 * Valeur renvoyée : Aucun
 */
{
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
    return;
}
