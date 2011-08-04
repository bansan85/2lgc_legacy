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


/* _1990_ponderations_verifie_double
 * Description : Vérifie dans la liste des ponderations si la ponderation à vérifier est déjà présente
 * Paramètres : LIST *ponderations : liste des pondérations
 *            : Ponderation* ponderation_a_verifier : pondération à vérifier
 * Valeur renvoyée :
 *   Succès : 0 si la pondération n'existe pas
 *          : 1 si la pondération existe
 *   Échec : valeur négative en cas d'erreur
 */
int _1990_ponderations_verifie_double(LIST *liste_ponderations, Ponderation* ponderation_a_verifier)
{
    if ((liste_ponderations == NULL) || (ponderation_a_verifier == NULL))
        BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
        
    if (list_size(liste_ponderations) == 0)
        return 0;
    
    // En renvoyant ici 1, la fonction fait croire que la pondération existe.
    // En vérité, c'est surtout qu'une pondération sans élément n'est pas intéressante à conserver,
    // à la différence des combinaisons vides qui peuvent être utilisées par les niveaux supérieurs
    if (list_size(ponderation_a_verifier->elements) == 0)
        return 1;
    
    list_mvfront(liste_ponderations);
    do
    {
        int     doublon;
        Ponderation *ponderation;
        
        // On pense que la pondération est identique jusqu'à preuve du contraire
        doublon = 1;
        ponderation = (Ponderation*)list_curr(liste_ponderations);
        if (list_size(ponderation->elements) == list_size(ponderation_a_verifier->elements))
        {
            list_mvfront(ponderation->elements);
            list_mvfront(ponderation_a_verifier->elements);
            do
            {
                Ponderation_Element *elem1, *elem2;
                
                elem1 = list_curr(ponderation->elements);
                elem2 = list_curr(ponderation_a_verifier->elements);
                // Preuve ici
                if ((elem1->action != elem2->action) || (elem1->psi != elem2->psi) || (!(ERREUR_RELATIVE_EGALE(elem1->ponderation, elem2->ponderation))))
                    doublon = 0;
            }
            while ((list_mvnext(ponderation->elements) != NULL) && (list_mvnext(ponderation_a_verifier->elements) != NULL) && (doublon == 1));
            if (doublon == 1)
                return 1;
        }
    }
    while (list_mvnext(liste_ponderations) != NULL);
    
    return 0;
}

/* _1990_ponderations_duplique_sans_double
 * Description : ajoute à une liste de pondérations existante une liste de pondérations.
 *             : une vérification est effectuée pour s'assurer que la liste source ne
 *               possède pas une ou plusieurs pondérations identiques que la liste de destination
 * Paramètres : LIST *liste_pond_destination : liste de ponderations qui recevra les ponderations sources
 *            : LIST *liste_pond_source : liste de ponderations source
 * Valeur renvoyée :
 *   Succès : 0 si les combinaisons sont ajoutées avec succès
 *   Échec : valeur négative en cas d'erreur
 */
int _1990_ponderations_duplique_sans_double(LIST *liste_pond_destination, LIST *liste_pond_source)
{
    if ((liste_pond_destination == NULL) || (liste_pond_source == NULL))
        BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
    
    if (list_size(liste_pond_source) == 0)
        return 0;
    
    list_mvrear(liste_pond_destination);
    list_mvfront(liste_pond_source);
    do
    {
        Ponderation     *ponderation_source;
        
        ponderation_source = list_curr(liste_pond_source);
        // Si la ponderation n'existe pas, on l'ajoute à la fin
        if (_1990_ponderations_verifie_double(liste_pond_destination, ponderation_source) == 0)
        {
            Ponderation     ponderation_destination;
            
            ponderation_destination.elements = list_init();
            if (ponderation_destination.elements == NULL)
                BUGTEXTE(-2, gettext("Erreur d'allocation mémoire.\n"));
            
            if ((ponderation_source != NULL) && (list_curr(ponderation_source->elements) != NULL))
            {
                list_mvfront(ponderation_source->elements);
                do
                {
                    Ponderation_Element *element_source;
                    Ponderation_Element element_destination;
                    
                    element_source = list_curr(ponderation_source->elements);
                    element_destination.action = element_source->action;
                    element_destination.flags = element_source->flags;
                    element_destination.psi = element_source->psi;
                    element_destination.ponderation = element_source->ponderation;
                    if (list_insert_after(ponderation_destination.elements, (void*)&element_destination, sizeof(element_destination)) == NULL)
                        BUGTEXTE(-3, gettext("Erreur d'allocation mémoire.\n"));
                }
                while (list_mvnext(ponderation_source->elements) != NULL);
            }
            if (list_insert_after(liste_pond_destination, (void*)&ponderation_destination, sizeof(ponderation_destination)) == NULL)
                BUGTEXTE(-4, gettext("Erreur d'allocation mémoire.\n"));
        }
    }
    while (list_mvnext(liste_pond_source) != NULL);
    
    return 0;
}

/* _1990_ponderations_genere_un
 * Description : Génère l'ensemble des pondérations en fonction des paramètres d'entrées.
 *             : Pour une génération exaustive conformément à une norme, il est nécessaire d'appeler directement
 *               la fonction _1990_ponderations_genere qui se chargera d'appeler _1990_ponderations_genere_un 
 *               autant de fois que nécessaire avec des paramètres adaptés
 * Paramètres : Projet *projet : la variable projet
 *            : LIST* combinaisons_destination : liste dans laquelle sera stocké les ponderations générées.
 *              Elles seront ajoutées en appelant la fonction _1990_ponderations_duplique_sans_double
 *            : double* coef_min : détermine les coefficients delta en situation favorable (min) et défavorable (max)
 *            : double* coef_max : l'indice du tableau à utiliser correspond à celui de la fonction _1990_action_type_combinaison_bat
 *            : int dim_coef : nombre d'incides dans le tableau de double coef_max et coef_min
 *            : int psi_dominante : indice du coefficient psi à utiliser pour l'action variable prédominante: 0 = psi0, 1 = psi1, 2 = psi et -1 = prendre la valeur 1.0.
 *            : int psi_accompagnement : indice du coefficient psi à utiliser pour les actions variables d'accompagnement : 0 = psi0, 1 = psi1, 2 = psi et -1 = prendre la valeur 1.0.
 * Valeur renvoyée :
 *   Succès : 0 si les pondérations sont ajoutées avec succès
 *   Échec : valeur négative en cas d'erreur
 */
int _1990_ponderations_genere_un(Projet *projet, LIST* ponderations_destination, double* coef_min, double* coef_max, int dim_coef, int psi_dominante, int psi_accompagnement)
{
    int     nbboucle=1, j;
    Groupe      *groupe;
    Niveau_Groupe   *niveau;
    
    if ((projet == NULL) || (projet->niveaux_groupes == NULL))
        BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
    
    niveau = list_rear(projet->niveaux_groupes);
    if (list_size(niveau->groupes) == 0)
    {
        printf(gettext("La génération des pondérations est impossible.\nLe dernier niveau ne possède pas qu'un seul groupe.\n"));
        return 0;
    }
    groupe = list_front(niveau->groupes);
    
    // Si il n'y a aucune combinaison
    if (list_size(groupe->tmp_combinaison.combinaisons) == 0)
    {
        printf(gettext("Le dernier niveau ne possède aucune combinaison permettant la génération des pondérations.\n"));
        return 0;
    }
    
    // Ainsi, on génère détermine à chaque passage si le coefficient min ou max doit être pris.
    // Chaque bit correspond à une ligne des tableaux coef_min et coef_max.
    // Lorsqu'un bit vaut 0, il est utilisé coef_min dans la pondération.
    // Lorsqu'un bit vaut 1, il est utilisé coef_max dans la pondération.
    // Ainsi, il y a bien 2^(nom de ligne dans coef_min et coef_max)
    nbboucle = nbboucle << dim_coef;
    for (j=0;j<nbboucle;j++)
    {
        list_mvfront(groupe->tmp_combinaison.combinaisons);
        do
        {
            // Suivant doit permettre de déterminer si la pondération générée doit être prise en compte.
            // En effet, la pondération pour être valable doit posséder un certain nombre de critères.
            // Premièrement, une pondération ne peut posséder une action variable d'accompagnement
            // sans action variable dominante.
            // Deuxièmement, il a été décidé que lorsqu'une action possède un coef_min = 0 et un coef_max = 0,
            // il convient de ne pas prendre la pondération en compte. Par exemple, lorsque les actions à ELU STR
            // sont en cours de génération, il convient de ne pas prendre les pondérations possédant des
            // actions accidentelles.
            int     suivant = 0, variable_accompagnement = 0, variable_dominante = 0;
            Combinaison *combinaison;
            Ponderation ponderation;
            
            combinaison = (Combinaison*) list_curr(groupe->tmp_combinaison.combinaisons);
            ponderation.elements = list_init();
            list_mvfront(combinaison->elements);
            if (list_size(combinaison->elements) != 0)
            {
                do
                {
                    // On parcourt tous les éléments de l'actuel combinaison pour la convertir en pondération
                    Ponderation_Element ponderation_element;
                    Combinaison_Element *combinaison_element;
                    int         categorie;
                    
                    combinaison_element = (Combinaison_Element*)list_curr(combinaison->elements);
                    ponderation_element.action = combinaison_element->action;
                    ponderation_element.flags = combinaison_element->flags;
                    // On regarde s'il s'agit d'une action variable
                    if (_1990_action_type_combinaison_bat(ponderation_element.action->categorie, projet->pays) == 2)
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
                    // psi vaut toujours -1 s'il ne s'agit pas d'une action variable
                    else
                        ponderation_element.psi = -1;
                    categorie = _1990_action_type_combinaison_bat(ponderation_element.action->categorie, projet->pays);
                    if ((ERREUR_RELATIVE_EGALE(0., coef_min[categorie])) && (ERREUR_RELATIVE_EGALE(0., coef_max[categorie])))
                        suivant = 1;
                    else
                    {
                        // On affecte le coefficient min/max à la combinaison pour obtenir la pondération
                        if ((j & (1 << categorie)) != 0)
                            ponderation_element.ponderation = coef_max[categorie];
                        else
                            ponderation_element.ponderation = coef_min[categorie];
                        if (!(ERREUR_RELATIVE_EGALE(0., ponderation_element.ponderation)))
                        {
                            if (list_insert_after(ponderation.elements, &ponderation_element, sizeof(ponderation_element)) == NULL)
                                BUGTEXTE(-2, gettext("Erreur d'allocation mémoire.\n"));
                        }
                    }
                }
                while ((list_mvnext(combinaison->elements) != NULL) && (suivant != 1));
            }
            if ((variable_accompagnement == 1) && (variable_dominante == 0))
                suivant = 1;
            if ((suivant == 0) && (list_size(ponderation.elements) != 0) && (_1990_ponderations_verifie_double(ponderations_destination, &ponderation) == 0))
            {
                if (list_insert_after(ponderations_destination, &ponderation, sizeof(ponderation)) == NULL)
                    BUGTEXTE(-3, gettext("Erreur d'allocation mémoire.\n"));
            }
            else
                list_free(ponderation.elements, LIST_DEALLOC);
        }
        while (list_mvnext(groupe->tmp_combinaison.combinaisons) != NULL);
    }
    
    return 0;
}

/* _1990_ponderations_genere_eu
 * Description : Génération de l'ensemble des pondérations selon la norme européenne
 *             : la fonction _1990_ponderations_genere_un est appelé autant de fois que nécessaire
 *               avec les coefficients min/max ajustées en fonction des valeur de la norme européenne
 *               et de la nature de l'état limite recherché.
 *             : Les options de calculs sont définies dans la variable projet->combinaisons.flags et doivent être définies.
 * Paramètres : Projet *projet : la variable projet
 * Valeur renvoyée :
 *   Succès : 0 si les pondérations sont générées avec succès
 *   Échec : valeur négative en cas d'erreur
 */
int _1990_ponderations_genere_eu(Projet *projet)
{
    double      *coef_min, *coef_max;
    
    if (projet == NULL)
        BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
    
    coef_min = (double*)malloc(5*sizeof(double));
    if (coef_min == NULL)
        BUGTEXTE(-2, gettext("Erreur d'allocation mémoire.\n"));
    coef_max = (double*)malloc(5*sizeof(double));
    if (coef_max == NULL)
    {
        free(coef_min);
        BUGTEXTE(-3, gettext("Erreur d'allocation mémoire.\n"));
    }
    
    // Pour ELU_EQU
    // Equilibre seulement
    if ((projet->combinaisons.flags & 1) == 0)
    {
        coef_min[0] = 0.9; coef_max[0] = 1.1; // poids propre
        coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
        coef_min[2] = 0.0; coef_max[2] = 1.5; // variable
        coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
        coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
        if (_1990_ponderations_genere_un(projet, projet->combinaisons.elu_equ, coef_min, coef_max, 5, -1, 0) != 0)
            BUG(-4);
    }
    // Equilibre + Résistance structurelle
    else
    {
        coef_min[0] = 1.15; coef_max[0] = 1.35; // poids propre
        coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
        coef_min[2] = 0.0; coef_max[2] = 1.5; // variable
        coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
        coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
        if (_1990_ponderations_genere_un(projet, projet->combinaisons.elu_equ, coef_min, coef_max, 5, -1, 0) != 0)
            BUG(-5);
        coef_min[0] = 1.0; coef_max[0] = 1.0; // poids propre
        coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
        coef_min[2] = 0.0; coef_max[2] = 1.5; // variable
        coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
        coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
        if (_1990_ponderations_genere_un(projet, projet->combinaisons.elu_equ, coef_min, coef_max, 5, -1, 0) != 0)
            BUG(-6);
    }
    // Equation 6.10a et 6.10b
    if ((projet->combinaisons.flags & 8) == 0)
    {
        switch (projet->combinaisons.flags & 6)
        {
            // Approche 1
            case 0:
            {
                coef_min[0] = 1.0; coef_max[0] = 1.35; // poids propre
                coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
                coef_min[2] = 0.0; coef_max[2] = 1.5; // variable
                coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
                coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
                if (_1990_ponderations_genere_un(projet, projet->combinaisons.elu_str, coef_min, coef_max, 5, 0, 0) != 0)
                    BUG(-7);
                coef_min[0] = 1.0; coef_max[0] = 1.15; // poids propre
                coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
                coef_min[2] = 0.0; coef_max[2] = 1.5; // variable
                coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
                coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
                if (_1990_ponderations_genere_un(projet, projet->combinaisons.elu_str, coef_min, coef_max, 5, -1, 0) != 0)
                    BUG(-8);
                coef_min[0] = 1.0; coef_max[0] = 1.0; // poids propre
                coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
                coef_min[2] = 0.0; coef_max[2] = 1.3; // variable
                coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
                coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
                if (_1990_ponderations_genere_un(projet, projet->combinaisons.elu_str, coef_min, coef_max, 5, -1, 0) != 0)
                    BUG(-9);
                if (_1990_ponderations_duplique_sans_double(projet->combinaisons.elu_geo, projet->combinaisons.elu_str) != 0)
                    BUG(-10);
                break;
            }
            // Approche 2
            case 2:
            {
                coef_min[0] = 1.0; coef_max[0] = 1.35; // poids propre
                coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
                coef_min[2] = 0.0; coef_max[2] = 1.5; // variable
                coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
                coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
                if (_1990_ponderations_genere_un(projet, projet->combinaisons.elu_str, coef_min, coef_max, 5, 0, 0) != 0)
                    BUG(-11);
                coef_min[0] = 1.0; coef_max[0] = 1.15; // poids propre
                coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
                coef_min[2] = 0.0; coef_max[2] = 1.5; // variable
                coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
                coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
                if (_1990_ponderations_genere_un(projet, projet->combinaisons.elu_str, coef_min, coef_max, 5, -1, 0) != 0)
                    BUG(-12);
                if (_1990_ponderations_duplique_sans_double(projet->combinaisons.elu_geo, projet->combinaisons.elu_str) != 0)
                    BUG(-13);
                break;
            }
            case 4:
            {
                coef_min[0] = 1.0; coef_max[0] = 1.35; // poids propre
                coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
                coef_min[2] = 0.0; coef_max[2] = 1.5; // variable
                coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
                coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
                if (_1990_ponderations_genere_un(projet, projet->combinaisons.elu_str, coef_min, coef_max, 5, 0, 0) != 0)
                    BUG(-14);
                coef_min[0] = 1.0; coef_max[0] = 1.15; // poids propre
                coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
                coef_min[2] = 0.0; coef_max[2] = 1.5; // variable
                coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
                coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
                if (_1990_ponderations_genere_un(projet, projet->combinaisons.elu_str, coef_min, coef_max, 5, -1, 0) != 0)
                    BUG(-15);
                coef_min[0] = 1.0; coef_max[0] = 1.0; // poids propre
                coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
                coef_min[2] = 0.0; coef_max[2] = 1.3; // variable
                coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
                coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
                if (_1990_ponderations_genere_un(projet, projet->combinaisons.elu_geo, coef_min, coef_max, 5, -1, 0) != 0)
                    BUG(-16);
                break;
            }
            default:
            {
                BUGTEXTE(-17, gettext("Paramètres invalides.\n"));
                break;
            }
        }
    }
    // Equation 6.10
    else
    {
        switch (projet->combinaisons.flags & 6)
        {
            // Approche 1
            case 0:
            {
                coef_min[0] = 1.0; coef_max[0] = 1.35; // poids propre
                coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
                coef_min[2] = 0.0; coef_max[2] = 1.5; // variable
                coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
                coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
                if (_1990_ponderations_genere_un(projet, projet->combinaisons.elu_str, coef_min, coef_max, 5, -1, 0) != 0)
                    BUG(-18);
                coef_min[0] = 1.0; coef_max[0] = 1.0; // poids propre
                coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
                coef_min[2] = 0.0; coef_max[2] = 1.3; // variable
                coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
                coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
                if (_1990_ponderations_genere_un(projet, projet->combinaisons.elu_str, coef_min, coef_max, 5, -1, 0) != 0)
                    BUG(-19);
                if (_1990_ponderations_duplique_sans_double(projet->combinaisons.elu_geo, projet->combinaisons.elu_str) != 0)
                    BUG(-20);
                break;
            }
            // Approche 2
            case 2:
            {
                coef_min[0] = 1.0; coef_max[0] = 1.35; // poids propre
                coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
                coef_min[2] = 0.0; coef_max[2] = 1.5; // variable
                coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
                coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
                if (_1990_ponderations_genere_un(projet, projet->combinaisons.elu_str, coef_min, coef_max, 5, -1, 0) != 0)
                    BUG(-21);
                if (_1990_ponderations_duplique_sans_double(projet->combinaisons.elu_geo, projet->combinaisons.elu_str) != 0)
                    BUG(-22);
                break;
            }
            case 4:
            {
                coef_min[0] = 1.0; coef_max[0] = 1.35; // poids propre
                coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
                coef_min[2] = 0.0; coef_max[2] = 1.5; // variable
                coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
                coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
                if (_1990_ponderations_genere_un(projet, projet->combinaisons.elu_str, coef_min, coef_max, 5, -1, 0) != 0)
                    BUG(-23);
                coef_min[0] = 1.0; coef_max[0] = 1.0; // poids propre
                coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
                coef_min[2] = 0.0; coef_max[2] = 1.3; // variable
                coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
                coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
                if (_1990_ponderations_genere_un(projet, projet->combinaisons.elu_geo, coef_min, coef_max, 5, -1, 0) != 0)
                    BUG(-24);
                break;
            }
            default :
            {
                BUGTEXTE(-25, gettext("Paramètres invalides.\n"));
                break;
            }
        }
    }
    if ((projet->combinaisons.flags & 16) == 0)
    {
        coef_min[0] = 1.0; coef_max[0] = 1.0; // poids propre
        coef_min[1] = 1.0; coef_max[1] = 1.0; // précontrainte (1992-1)
        coef_min[2] = 0.0; coef_max[2] = 1.0; // variable
        coef_min[3] = 1.0; coef_max[3] = 1.0; // Accidentelle
        coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
        if (_1990_ponderations_genere_un(projet, projet->combinaisons.elu_acc, coef_min, coef_max, 5, 1, 2) != 0)
            BUG(-26);
    }
    else
    {
        coef_min[0] = 1.0; coef_max[0] = 1.0; // poids propre
        coef_min[1] = 1.0; coef_max[1] = 1.0; // précontrainte (1992-1)
        coef_min[2] = 0.0; coef_max[2] = 1.0; // variable
        coef_min[3] = 1.0; coef_max[3] = 1.0; // Accidentelle
        coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
        if (_1990_ponderations_genere_un(projet, projet->combinaisons.elu_acc, coef_min, coef_max, 5, 2, 2) != 0)
            BUG(-27);
    }
    
    coef_min[0] = 1.0; coef_max[0] = 1.0; // poids propre
    coef_min[1] = 1.0; coef_max[1] = 1.0; // précontrainte (1992-1)
    coef_min[2] = 0.0; coef_max[2] = 1.0; // variable
    coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
    coef_min[4] = 1.0; coef_max[4] = 1.0; // Sismique
    if (_1990_ponderations_genere_un(projet, projet->combinaisons.elu_sis, coef_min, coef_max, 5, 2, 2) != 0)
        BUG(-28);
    
    coef_min[0] = 1.0; coef_max[0] = 1.0; // poids propre
    coef_min[1] = 1.0; coef_max[1] = 1.0; // précontrainte (1992-1)
    coef_min[2] = 0.0; coef_max[2] = 1.0; // variable
    coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
    coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
    if (_1990_ponderations_genere_un(projet, projet->combinaisons.els_car, coef_min, coef_max, 5, -1, 0) != 0)
        BUG(-29);
    
    coef_min[0] = 1.0; coef_max[0] = 1.0; // poids propre
    coef_min[1] = 1.0; coef_max[1] = 1.0; // précontrainte (1992-1)
    coef_min[2] = 0.0; coef_max[2] = 1.0; // variable
    coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
    coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
    if (_1990_ponderations_genere_un(projet, projet->combinaisons.els_freq, coef_min, coef_max, 5, 1, 2) != 0)
        BUG(-30);
    
    coef_min[0] = 1.0; coef_max[0] = 1.0; // poids propre
    coef_min[1] = 1.0; coef_max[1] = 1.0; // précontrainte (1992-1)
    coef_min[2] = 0.0; coef_max[2] = 1.0; // variable
    coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
    coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
    if (_1990_ponderations_genere_un(projet, projet->combinaisons.els_perm, coef_min, coef_max, 5, 2, 2) != 0)
        BUG(-31);
    
    free(coef_min);
    free(coef_max);
    
    return 0;
}

/* _1990_ponderations_genere_fr
 * Description : Génération de l'ensemble des pondérations selon la norme française
 *             : la fonction _1990_ponderations_genere_un est appelé autant de fois que nécessaire
 *               avec les coefficients min/max ajustées en fonction des valeur de la norme française
 *               et de la nature de l'état limite recherché.
 *             : Les options de calculs sont définies dans la variable projet->combinaisons.flags et doivent être définies.
 * Paramètres : Projet *projet : la variable projet
 * Valeur renvoyée :
 *   Succès : 0 si les pondérations sont générées avec succès
 *   Échec : valeur négative en cas d'erreur
 */
int _1990_ponderations_genere_fr(Projet *projet)
{
    double      *coef_min, *coef_max;
    
    if (projet == NULL)
        BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
    
    coef_min = (double*)malloc(6*sizeof(double));
    if (coef_min == NULL)
        BUGTEXTE(-2, gettext("Erreur d'allocation mémoire.\n"));
    coef_max = (double*)malloc(6*sizeof(double));
    if (coef_max == NULL)
    {
        free(coef_min);
        BUGTEXTE(-3, gettext("Erreur d'allocation mémoire.\n"));
    }
    
    // Pour ELU_EQU
    if ((projet->combinaisons.flags & 1) == 0)
    {
        coef_min[0] = 0.9; coef_max[0] = 1.1; // poids propre
        coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
        coef_min[2] = 0.0; coef_max[2] = 1.5; // variable
        coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
        coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
        coef_min[5] = 0.0; coef_max[5] = 1.2; // Eaux souterraines
        if (_1990_ponderations_genere_un(projet, projet->combinaisons.elu_equ, coef_min, coef_max, 6, -1, 0) != 0)
            BUG(-4);
    }
    else
    {
        coef_min[0] = 1.15; coef_max[0] = 1.35; // poids propre
        coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
        coef_min[2] = 0.0; coef_max[2] = 1.5; // variable
        coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
        coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
        coef_min[5] = 0.0; coef_max[5] = 1.2; // Eaux souterraines
        if (_1990_ponderations_genere_un(projet, projet->combinaisons.elu_equ, coef_min, coef_max, 6, -1, 0) != 0)
            BUG(-5);
        coef_min[0] = 1.0; coef_max[0] = 1.0; // poids propre
        coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
        coef_min[2] = 0.0; coef_max[2] = 1.5; // variable
        coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
        coef_min[4] = 0.0; coef_max[4] = 1.0; // Sismique
        coef_min[5] = 0.0; coef_max[5] = 1.2; // Eaux souterraines
        if (_1990_ponderations_genere_un(projet, projet->combinaisons.elu_equ, coef_min, coef_max, 6, -1, 0) != 0)
            BUG(-6);
    }
    // On utilise l'équation 6.10a et 6.10b
    if ((projet->combinaisons.flags & 8) == 0)
    {
        switch (projet->combinaisons.flags & 6)
        {
            case 0:
            {
                coef_min[0] = 1.0; coef_max[0] = 1.35; // poids propre
                coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
                coef_min[2] = 0.0; coef_max[2] = 1.5; // variable
                coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
                coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
                coef_min[5] = 0.0; coef_max[5] = 1.2; // Eaux souterraines
                if (_1990_ponderations_genere_un(projet, projet->combinaisons.elu_str, coef_min, coef_max, 6, 0, 0) != 0)
                    BUG(-7);
                coef_min[0] = 1.0; coef_max[0] = 1.15; // poids propre
                coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
                coef_min[2] = 0.0; coef_max[2] = 1.5; // variable
                coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
                coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
                coef_min[5] = 0.0; coef_max[5] = 1.2; // Eaux souterraines
                if (_1990_ponderations_genere_un(projet, projet->combinaisons.elu_str, coef_min, coef_max, 6, -1, 0) != 0)
                    BUG(-8);
                coef_min[0] = 1.0; coef_max[0] = 1.0; // poids propre
                coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
                coef_min[2] = 0.0; coef_max[2] = 1.3; // variable
                coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
                coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
                coef_min[5] = 0.0; coef_max[5] = 1.2; // Eaux souterraines
                if (_1990_ponderations_genere_un(projet, projet->combinaisons.elu_str, coef_min, coef_max, 6, -1, 0) != 0)
                    BUG(-9);
                if (_1990_ponderations_duplique_sans_double(projet->combinaisons.elu_geo, projet->combinaisons.elu_str) != 0)
                    BUG(-10);
                break;
            }
            case 2:
            {
                coef_min[0] = 1.0; coef_max[0] = 1.35; // poids propre
                coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
                coef_min[2] = 0.0; coef_max[2] = 1.5; // variable
                coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
                coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
                coef_min[5] = 0.0; coef_max[5] = 1.2; // Eaux souterraines
                if (_1990_ponderations_genere_un(projet, projet->combinaisons.elu_str, coef_min, coef_max, 6, 0, 0) != 0)
                    BUG(-11);
                coef_min[0] = 1.0; coef_max[0] = 1.15; // poids propre
                coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
                coef_min[2] = 0.0; coef_max[2] = 1.5; // variable
                coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
                coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
                coef_min[5] = 0.0; coef_max[5] = 1.2; // Eaux souterraines
                if (_1990_ponderations_genere_un(projet, projet->combinaisons.elu_str, coef_min, coef_max, 6, -1, 0) != 0)
                    BUG(-12);
                if (_1990_ponderations_duplique_sans_double(projet->combinaisons.elu_geo, projet->combinaisons.elu_str) != 0)
                    BUG(-13);
                break;
            }
            case 4:
            {
                coef_min[0] = 1.0; coef_max[0] = 1.35; // poids propre
                coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
                coef_min[2] = 0.0; coef_max[2] = 1.5; // variable
                coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
                coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
                coef_min[5] = 0.0; coef_max[5] = 1.2; // Eaux souterraines
                if (_1990_ponderations_genere_un(projet, projet->combinaisons.elu_str, coef_min, coef_max, 6, 0, 0) != 0)
                    BUG(-14);
                coef_min[0] = 1.0; coef_max[0] = 1.15; // poids propre
                coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
                coef_min[2] = 0.0; coef_max[2] = 1.5; // variable
                coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
                coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
                coef_min[5] = 0.0; coef_max[5] = 1.2; // Eaux souterraines
                if (_1990_ponderations_genere_un(projet, projet->combinaisons.elu_str, coef_min, coef_max, 6, -1, 0) != 0)
                    BUG(-15);
                coef_min[0] = 1.0; coef_max[0] = 1.0; // poids propre
                coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
                coef_min[2] = 0.0; coef_max[2] = 1.3; // variable
                coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
                coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
                coef_min[5] = 0.0; coef_max[5] = 1.2; // Eaux souterraines
                if (_1990_ponderations_genere_un(projet, projet->combinaisons.elu_geo, coef_min, coef_max, 6, -1, 0) != 0)
                    BUG(-16);
                break;
            }
            default :
            {
                BUGTEXTE(-17, gettext("Paramètres invalides.\n"));
                break;
            }
        }
    }
    else
    // équation 6.10
    {
        switch (projet->combinaisons.flags & 6)
        {
            case 0:
            {
                coef_min[0] = 1.0; coef_max[0] = 1.35; // poids propre
                coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
                coef_min[2] = 0.0; coef_max[2] = 1.5; // variable
                coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
                coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
                coef_min[5] = 0.0; coef_max[5] = 1.2; // Eaux souterraines
                if (_1990_ponderations_genere_un(projet, projet->combinaisons.elu_str, coef_min, coef_max, 6, -1, 0) != 0)
                    BUG(-18);
                coef_min[0] = 1.0; coef_max[0] = 1.0; // poids propre
                coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
                coef_min[2] = 0.0; coef_max[2] = 1.3; // variable
                coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
                coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
                coef_min[5] = 0.0; coef_max[5] = 1.2; // Eaux souterraines
                if (_1990_ponderations_genere_un(projet, projet->combinaisons.elu_str, coef_min, coef_max, 6, -1, 0) != 0)
                    BUG(-19);
                if (_1990_ponderations_duplique_sans_double(projet->combinaisons.elu_geo, projet->combinaisons.elu_str) != 0)
                    BUG(-20);
                break;
            }
            case 2:
            {
                coef_min[0] = 1.0; coef_max[0] = 1.35; // poids propre
                coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
                coef_min[2] = 0.0; coef_max[2] = 1.5; // variable
                coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
                coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
                coef_min[5] = 0.0; coef_max[5] = 1.2; // Eaux souterraines
                if (_1990_ponderations_genere_un(projet, projet->combinaisons.elu_str, coef_min, coef_max, 6, -1, 0) != 0)
                    BUG(-21);
                if (_1990_ponderations_duplique_sans_double(projet->combinaisons.elu_geo, projet->combinaisons.elu_str) != 0)
                    BUG(-22);
                break;
            }
            case 4:
            {
                coef_min[0] = 1.0; coef_max[0] = 1.35; // poids propre
                coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
                coef_min[2] = 0.0; coef_max[2] = 1.5; // variable
                coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
                coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
                coef_min[5] = 0.0; coef_max[5] = 1.2; // Eaux souterraines
                if (_1990_ponderations_genere_un(projet, projet->combinaisons.elu_str, coef_min, coef_max, 6, -1, 0) != 0)
                    BUG(-23);
                coef_min[0] = 1.0; coef_max[0] = 1.0; // poids propre
                coef_min[1] = 1.0; coef_max[1] = 1.3; // précontrainte (1992-1)
                coef_min[2] = 0.0; coef_max[2] = 1.3; // variable
                coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
                coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
                coef_min[5] = 0.0; coef_max[5] = 1.2; // Eaux souterraines
                if (_1990_ponderations_genere_un(projet, projet->combinaisons.elu_geo, coef_min, coef_max, 6, -1, 0) != 0)
                    BUG(-24);
                break;
            }
            default :
            {
                BUGTEXTE(-25, gettext("Paramètres invalides.\n"));
                break;
            }
        }
    }
    if ((projet->combinaisons.flags & 16) == 0)
    {
        coef_min[0] = 1.0; coef_max[0] = 1.0; // poids propre
        coef_min[1] = 1.0; coef_max[1] = 1.0; // précontrainte (1992-1)
        coef_min[2] = 0.0; coef_max[2] = 1.0; // variable
        coef_min[3] = 1.0; coef_max[3] = 1.0; // Accidentelle
        coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
        coef_min[5] = 0.0; coef_max[5] = 1.0; // Eaux souterraines
        if (_1990_ponderations_genere_un(projet, projet->combinaisons.elu_acc, coef_min, coef_max, 6, 1, 2) != 0)
            BUG(-26);
    }
    else
    {
        coef_min[0] = 1.0; coef_max[0] = 1.0; // poids propre
        coef_min[1] = 1.0; coef_max[1] = 1.0; // précontrainte (1992-1)
        coef_min[2] = 0.0; coef_max[2] = 1.0; // variable
        coef_min[3] = 1.0; coef_max[3] = 1.0; // Accidentelle
        coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
        coef_min[5] = 0.0; coef_max[5] = 1.0; // Eaux souterraines
        if (_1990_ponderations_genere_un(projet, projet->combinaisons.elu_acc, coef_min, coef_max, 6, 2, 2) != 0)
            BUG(-27);
    }
    
    coef_min[0] = 1.0; coef_max[0] = 1.0; // poids propre
    coef_min[1] = 1.0; coef_max[1] = 1.0; // précontrainte (1992-1)
    coef_min[2] = 0.0; coef_max[2] = 1.0; // variable
    coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
    coef_min[4] = 1.0; coef_max[4] = 1.0; // Sismique
    coef_min[5] = 0.0; coef_max[5] = 1.0; // Eaux souterraines
    if (_1990_ponderations_genere_un(projet, projet->combinaisons.elu_acc, coef_min, coef_max, 6, 2, 2) != 0)
        BUG(-28);
    
    coef_min[0] = 1.0; coef_max[0] = 1.0; // poids propre
    coef_min[1] = 1.0; coef_max[1] = 1.0; // précontrainte (1992-1)
    coef_min[2] = 0.0; coef_max[2] = 1.0; // variable
    coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
    coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
    coef_min[5] = 0.0; coef_max[5] = 1.2; // Eaux souterraines
    if (_1990_ponderations_genere_un(projet, projet->combinaisons.els_car, coef_min, coef_max, 6, -1, 0) != 0)
        BUG(-29);

    coef_min[0] = 1.0; coef_max[0] = 1.0; // poids propre
    coef_min[1] = 1.0; coef_max[1] = 1.0; // précontrainte (1992-1)
    coef_min[2] = 0.0; coef_max[2] = 1.0; // variable
    coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
    coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
    coef_min[5] = 0.0; coef_max[5] = 1.0; // Eaux souterraines
    if (_1990_ponderations_genere_un(projet, projet->combinaisons.els_freq, coef_min, coef_max, 6, 1, 2) != 0)
        BUG(-30);

    coef_min[0] = 1.0; coef_max[0] = 1.0; // poids propre
    coef_min[1] = 1.0; coef_max[1] = 1.0; // précontrainte (1992-1)
    coef_min[2] = 0.0; coef_max[2] = 1.0; // variable
    coef_min[3] = 0.0; coef_max[3] = 0.0; // Accidentelle
    coef_min[4] = 0.0; coef_max[4] = 0.0; // Sismique
    coef_min[5] = 0.0; coef_max[5] = 1.0; // Eaux souterraines
    if (_1990_ponderations_genere_un(projet, projet->combinaisons.els_perm, coef_min, coef_max, 6, 2, 2) != 0)
        BUG(-31);
    
    free(coef_min);
    free(coef_max);
    
    return 0;
}

/* _1990_ponderations_genere
 * Description : Génération de l'ensemble des pondérations selon la norme du pays spécifié
 * Paramètres : Projet *projet : la variable projet
 * Valeur renvoyée :
 *   Succès : 0 si les pondérations sont générées avec succès
 *   Échec : valeur négative en cas d'erreur
 */
int _1990_ponderations_genere(Projet *projet)
{
    switch (projet->pays)
    {
        case PAYS_EU : { return _1990_ponderations_genere_eu(projet); break; }
        case PAYS_FR : { return _1990_ponderations_genere_fr(projet); break; }
        default : { BUGTEXTE(-1, gettext("Paramètres invalides.\n")); break; }
    }
}

/* _1990_ponderations_affiche
 * Description : Affiche les pondérations de la liste fournie en argument
 * Paramètres : LIST *ponderations : la liste des pondérations
 * Valeur renvoyée : Aucun
 */
void _1990_ponderations_affiche(LIST *ponderations)
{
    if ((ponderations != NULL) && (list_size(ponderations) != 0))
    {
        list_mvfront(ponderations);
        do
        {
            Ponderation *ponderation = list_curr(ponderations);
            
            if (list_size(ponderation->elements) != 0)
            {
                list_mvfront(ponderation->elements);
                do
                {
                    Ponderation_Element *ponderation_element = list_curr(ponderation->elements);
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

/* _1990_ponderations_affiche_tout
 * Description : Affiche toutes les pondérations du projet
 * Paramètres : Projet *projet : la variable projet
 * Valeur renvoyée : Aucun
 */
void _1990_ponderations_affiche_tout(Projet *projet)
{
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
