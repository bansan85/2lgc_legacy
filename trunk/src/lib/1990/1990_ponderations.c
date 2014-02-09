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
#include "common_math.h"
#include "1990_action.h"

int
_1990_ponderations_verifie_double (GList *liste_ponderations,
                                   GList *pond_a_verifier)
/**
 * \brief Vérifie dans la liste des ponderations si la ponderation à vérifier
 *        est déjà présente.
 * \param liste_ponderations : liste des pondérations,
 * \param pond_a_verifier : pondération à vérifier.
 * \return
 *   Succès :
 *     - FALSE si la pondération n'existe pas,
 *     - TRUE si la pondération existe.
 */
{
  GList *list_parcours;
  
  if (liste_ponderations == NULL)
    return FALSE;
  
  // En renvoyant ici 1, la fonction fait croire que la pondération existe.
  // En vérité, c'est surtout qu'une pondération sans élément n'est pas
  // intéressante à conserver, à la différence des combinaisons vides qui
  // peuvent être utilisées par les niveaux supérieurs.
  if (pond_a_verifier == NULL)
    return TRUE;
  
  list_parcours = liste_ponderations;
  do
  {
    int    doublon;
    GList *ponderation = list_parcours->data;
    GList *list_parcours2 = ponderation;
    GList *list_parcours3 = pond_a_verifier;
    
    // On pense que la pondération est identique jusqu'à preuve du contraire
    doublon = 1;
    
    while ((list_parcours2 != NULL) && (list_parcours3 != NULL) &&
           (doublon == 1))
    {
      Ponderation *elem1, *elem2;
      
      elem1 = list_parcours2->data;
      elem2 = list_parcours3->data;
      
      if ((elem1->action != elem2->action) || (elem1->psi != elem2->psi) ||
          (!(ERR (elem1->ponderation, elem2->ponderation))))
        doublon = 0;
      
      list_parcours2 = g_list_next (list_parcours2);
      list_parcours3 = g_list_next (list_parcours3);
    }
    
    if (doublon == 1)
      return TRUE;
    
    list_parcours = g_list_next (list_parcours);
  }
  while (list_parcours != NULL);
  
  return FALSE;
}


gboolean
_1990_ponderations_duplique_sans_double (GList **liste_dest,
                                         GList  *liste_source)
/**
 * \brief Ajoute à une liste de pondérations existante une liste de
 *        pondérations. Une vérification est effectuée pour s'assurer que la
 *        liste source ne possède pas une ou plusieurs pondérations identiques
 *        que la liste de destination.
 * \param liste_dest : liste de ponderations qui recevra les ponderations
 *                     sources,
 * \param liste_source : liste de ponderations source.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - liste_dest == NULL,
 *     - en cas d'erreur d'allocation mémoire.
 */
{
  GList *list_parcours;
  
  BUGMSG (liste_dest,
          FALSE,
          gettext ("Paramètre %s incorrect.\n"), "liste_dest")
  
  if (liste_source == NULL)
    return 0;
  
  list_parcours = liste_source;
  do
  {
    GList *ponderation_source;
    
    ponderation_source = list_parcours->data;
    /* Si la ponderation n'existe pas, on l'ajoute à la fin */
    if (_1990_ponderations_verifie_double (*liste_dest, ponderation_source) ==
                                                                         FALSE)
    {
      GList *ponderation_destination;
      GList *list_parcours2 = ponderation_source;
      
      ponderation_destination = NULL;
      
      do
      {
        Ponderation *element_source;
        Ponderation *element_destination = malloc (sizeof (Ponderation));
        
        BUGMSG (element_destination,
                FALSE,
                gettext ("Erreur d'allocation mémoire.\n"))
        element_source = list_parcours2->data;
        element_destination->action = element_source->action;
        element_destination->flags = element_source->flags;
        element_destination->psi = element_source->psi;
        element_destination->ponderation = element_source->ponderation;
        ponderation_destination = g_list_append (ponderation_destination,
                                                 element_destination);
        
        list_parcours2 = g_list_next (list_parcours2);
      }
      while (list_parcours2 != NULL);
      
      *liste_dest = g_list_append (*liste_dest, ponderation_destination);
    }
    list_parcours = g_list_next (list_parcours);
  }
  while (list_parcours != NULL);
  
  return TRUE;
}


gboolean
_1990_ponderations_genere_un (Projet *p,
                              GList **ponderations_destination,
                              double *coef_min,
                              double *coef_max,
                              int     dim_coef,
                              int     psi_dominante,
                              int     psi_accompagnement)
/**
 * \brief Génère l'ensemble des pondérations en fonction des paramètres
 *        d'entrées. Pour une génération exaustive conformément à une norme, il
 *        est nécessaire d'appeler directement la fonction
 *        #_1990_ponderations_genere qui se chargera d'appeler
 *        #_1990_ponderations_genere_un autant de fois que nécessaire avec des
 *        paramètres adaptés.
 * \param p : la variable projet,
 * \param ponderations_destination : liste dans laquelle sera stockés les
 *        ponderations générées. Les résultats seront filtrés pour éviter les
 *        doublons,
 * \param coef_min : coefficients psi en situation favorable (min),
 * \param coef_max : coefficients psi en situation défavorable (max).
 *                   L'indice du tableau à utiliser est celui renvoyé par
 *                   #_1990_action_categorie_bat,
 * \param dim_coef : nombre d'incides dans le tableau de double coef_max et
 *                   coef_min,
 * \param psi_dominante : indice du coefficient psi à utiliser pour l'action
 *        variable prédominante: 0 = psi0, 1 = psi1, 2 = psi et -1 = prendre la
 *        valeur 1.0,
 * \param psi_accompagnement : indice du coefficient psi à utiliser pour les
 *        actions variables d'accompagnement : 0 = psi0, 1 = psi1, 2 = psi et
 *        -1 = prendre la valeur 1.0,
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL,
 *     - p->niveaux_groupes == NULL,
 *     - en cas d'erreur d'allocation mémoire.
 */
{
  int            nbboucle, j;
  Groupe        *groupe;
  Niveau_Groupe *niveau;
  
  BUGMSG (p, FALSE, gettext ("Paramètre %s incorrect.\n"), "projet")
  BUGMSG (p->niveaux_groupes,
          FALSE,
          gettext ("Le projet ne possède pas de niveaux de groupes.\n"))
  
  // Si le dernier niveau ne possède pas un seul et unique groupe Alors
  //   Fin.
  // FinSi
  niveau = g_list_last (p->niveaux_groupes)->data;
  BUGMSG (g_list_length (niveau->groupes) == 1,
          FALSE,
          gettext ("La génération des pondérations est impossible.\nLe dernier niveau ne peut possèder qu'un seul groupe.\n"))
  groupe = niveau->groupes->data;
  
  // Si le groupe du dernier niveau ne possède pas de combinaison Alors
  //   Fin.
  // FinSi
  BUGMSG (groupe->tmp_combinaison,
          FALSE,
          gettext ("Le dernier niveau ne possède aucune combinaison permettant la génération des pondérations.\n"))
  
  // Génération d'une boucle contenant 2^dim_coef permettant ainsi à chaque
  // passage de déterminer si le coefficient min ou max doit être pris.
  // Chaque bit correspond à une ligne des tableaux coef_min et coef_max.
  // Lorsqu'un bit vaut 0, il est utilisé coef_min dans la pondération.
  // Lorsqu'un bit vaut 1, il est utilisé coef_max dans la pondération.
  // Pour chaque itération j, définissant chacune une combinaison différente
  // des coefficients coef_min et coef_max.
  nbboucle = 1 << dim_coef;
  for (j = 0; j < nbboucle; j++)
  {
    GList *list_parcours = groupe->tmp_combinaison;
    // Pour chaque combinaison dans le groupe final Faire
    do
    {
      // Déterminer si la pondération générée doit être prise en compte. Elle
      // n'est valable que si :
      //   - Premièrement, une pondération ne peut posséder une action variable
      //     d'accompagnement sans action variable prédominante.
      //   - Deuxièmement, lorsqu'une action possède coef_min = 0 et
      //     coef_max = 0, il convient de ne pas prendre la pondération en
      //     compte. Par exemple, lorsque les actions à ELU STR sont en cours
      //     de génération, il convient de ne pas prendre les pondérations
      //     possédant des actions accidentelles.
      int    suivant = 0, variable_accompagnement = 0, variable_dominante = 0;
      GList *combinaison;
      GList *ponderation;
      
      combinaison = list_parcours->data;
      ponderation = NULL;
      
      if (combinaison != NULL)
      {
        GList *list_parcours2 = combinaison;
        // Pour chaque élément de la combinaison Faire
        do
        {
          Combinaison *combinaison_element;
          unsigned int categorie;
          double       pond;
          
          combinaison_element = list_parcours2->data;
          categorie = _1990_action_categorie_bat (_1990_action_type_renvoie (
                                                  combinaison_element->action),
                                                  p->parametres.norme);
          BUG (categorie != ACTION_INCONNUE, FALSE)
          // Vérification si le coefficient min et max de la catégorie vaut 0.
          //  Si oui, pondération ignorée.
          if ((ERR (0., coef_min[categorie])) &&
              (ERR (0., coef_max[categorie])))
            suivant = 1;
          else
          {
          // On affecte le coefficient min/max à la combinaison pour obtenir la
          // pondération
            if ((j & (1 << categorie)) != 0)
              pond = coef_max[categorie];
            else
              pond = coef_min[categorie];
            
            if (!(ERR (0., pond)))
            {
              Ponderation *ponderation_element = malloc (sizeof (Ponderation));
              
              BUGMSG (ponderation_element,
                      FALSE,
                      gettext ("Erreur d'allocation mémoire.\n"))
              ponderation_element->action = combinaison_element->action;
              ponderation_element->flags = combinaison_element->flags;
              
              // Vérifier la présente d'une action variable prédominante et
              // d'une action variable d'accompagnement. Si oui, pondération
              // ignorée.
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
              // psi vaut toujours -1 s'il ne s'agit pas d'une action variable.
              else
                ponderation_element->psi = -1;
              
              ponderation_element->ponderation = pond;
          
              ponderation = g_list_append (ponderation, ponderation_element);
            }
          }
          list_parcours2 = g_list_next (list_parcours2);
        }
        while ((list_parcours2 != NULL) && (suivant != 1));
        // FinPour
      }
  // Si la pondération n'est pas ignorée Alors
  //   Ajout à la liste des pondérations existante.
  // FinSi
      if ((variable_accompagnement == 1) && (variable_dominante == 0))
        suivant = 1;
      if ((suivant == 0) &&
          (_1990_ponderations_verifie_double (*ponderations_destination,
                                              ponderation) == FALSE))
        *ponderations_destination = g_list_append (*ponderations_destination,
                                                   ponderation);
      else
        g_list_free_full (ponderation, g_free);
      
      list_parcours = g_list_next (list_parcours);
    }
    while (list_parcours != NULL);
    // FinPour
  }
  // FinPour
  
  return TRUE;
}


gboolean
_1990_ponderations_genere_eu (Projet *p)
/**
 * \brief Génération de l'ensemble des pondérations selon la norme européenne.
 *        La fonction #_1990_ponderations_genere_un est appelée autant de fois
 *        que nécessaire avec les coefficients min/max ajustées en fonction des
 *        valeur de la norme européenne et de la nature de l'état limite
 *        recherché. Les options de calculs sont définies dans la variable
 *        p->combinaisons.flags et doivent être définies.
 * \param p : la variable projet.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL,
 *     - #_1990_ponderations_genere_un,
 *     - #_1990_ponderations_duplique_sans_double.
 */
{
  double coef_min[ACTION_INCONNUE], coef_max[ACTION_INCONNUE];
  
  BUGMSG (p, FALSE, gettext ("Paramètre %s incorrect.\n"), "projet")
  
 // Les indices ont les définitions suivantes : pp = poids propre,
 // p = précontrainte, var = variable, acc = accidentelle et
 // sis = sismique.
  
  // Pour ELU_EQU, générer les pondérations suivantes :
  // Si à l'équilibre seulement Alors
  //   coefficient charges variables prédominante : 1,
  //   coefficient charges variables d'accompagnement : psi0,
  //   min_pp = 0.9, min_p = 1.0, min_var = 0.0, min_acc = 0.0, min_sis = 0.0,
  //   max_pp = 1.1, max_p = 1.3, max_var = 1.5, max_acc = 0.0, max_sis = 0.0.
  if ((p->combinaisons.flags & 1) == 0)
  {
    coef_min[ACTION_POIDS_PROPRE]  = 0.9;
    coef_max[ACTION_POIDS_PROPRE]  = 1.1;
    coef_min[ACTION_PRECONTRAINTE] = 1.0;
    coef_max[ACTION_PRECONTRAINTE] = 1.3;
    coef_min[ACTION_VARIABLE]      = 0.0;
    coef_max[ACTION_VARIABLE]      = 1.5;
    coef_min[ACTION_ACCIDENTELLE]  = 0.0;
    coef_max[ACTION_ACCIDENTELLE]  = 0.0;
    coef_min[ACTION_SISMIQUE]      = 0.0;
    coef_max[ACTION_SISMIQUE]      = 0.0;
    BUG (_1990_ponderations_genere_un (p,
                                       &p->combinaisons.elu_equ,
                                       coef_min,
                                       coef_max,
                                       5,
                                       -1,
                                       0),
         FALSE)
  }
  // Sinon (à l'équilibre et à la résistance structurelle)
  //   coefficient charges variables prédominante : 1,
  //   coefficient charges variables d'accompagnement : psi0,
  //   min_pp = 1.15, min_p = 1.0, min_var = 0.0, min_acc = 0.0, min_sis = 0.0,
  //   max_pp = 1.35, max_p = 1.3, max_var = 1.5, max_acc = 0.0, max_sis = 0.0.
  //   et
  //   coefficient charges variables prédominante : 1,
  //   coefficient charges variables d'accompagnement : psi0,
  //   min_pp = 1.00, min_p = 1.0, min_var = 0.0, min_acc = 0.0, min_sis = 0.0,
  //   max_pp = 1.00, max_p = 1.3, max_var = 1.5, max_acc = 0.0, max_sis = 0.0.
  // FinSinon
  // FinPour
  else
  {
    coef_min[ACTION_POIDS_PROPRE]  = 1.15;
    coef_max[ACTION_POIDS_PROPRE]  = 1.35;
    coef_min[ACTION_PRECONTRAINTE] = 1.0;
    coef_max[ACTION_PRECONTRAINTE] = 1.3;
    coef_min[ACTION_VARIABLE]      = 0.0;
    coef_max[ACTION_VARIABLE]      = 1.5;
    coef_min[ACTION_ACCIDENTELLE]  = 0.0;
    coef_max[ACTION_ACCIDENTELLE]  = 0.0;
    coef_min[ACTION_SISMIQUE]      = 0.0;
    coef_max[ACTION_SISMIQUE]      = 0.0;
    BUG (_1990_ponderations_genere_un (p,
                                       &p->combinaisons.elu_equ,
                                       coef_min,
                                       coef_max,
                                       5,
                                       -1,
                                       0),
         FALSE)
    
    coef_min[ACTION_POIDS_PROPRE]  = 1.0;
    coef_max[ACTION_POIDS_PROPRE]  = 1.0;
    coef_min[ACTION_PRECONTRAINTE] = 1.0;
    coef_max[ACTION_PRECONTRAINTE] = 1.3;
    coef_min[ACTION_VARIABLE]      = 0.0;
    coef_max[ACTION_VARIABLE]      = 1.5;
    coef_min[ACTION_ACCIDENTELLE]  = 0.0;
    coef_max[ACTION_ACCIDENTELLE]  = 0.0;
    coef_min[ACTION_SISMIQUE]      = 0.0;
    coef_max[ACTION_SISMIQUE]      = 0.0;
    BUG (_1990_ponderations_genere_un (p,
                                       &p->combinaisons.elu_equ,
                                       coef_min,
                                       coef_max,
                                       5,
                                       -1,
                                       0),
         FALSE)
  }
  // Si utilisation des formules 6.10a et 6.10b de l'Eurocode 0 Alors
  if ((p->combinaisons.flags & 8) == 0)
  {
    switch (p->combinaisons.flags & 6)
    {
  // Si selon l'approche 1 Alors
  // Pour ELU_STR ET ELU_GEO, générer les pondérations suivantes :
  //   coefficient charges variables prédominante : psi0,
  //   coefficient charges variables d'accompagnement : psi0,
  //   min_pp = 1.00, min_p = 1.0, min_var = 0.0, min_acc = 0.0, min_sis = 0.0,
  //   max_pp = 1.35, max_p = 1.3, max_var = 1.5, max_acc = 0.0, max_sis = 0.0.
  //   et
  //   coefficient charges variables prédominante : 1,
  //   coefficient charges variables d'accompagnement : psi0,
  //   min_pp = 1.00, min_p = 1.0, min_var = 0.0, min_acc = 0.0, min_sis = 0.0,
  //   max_pp = 1.15, max_p = 1.3, max_var = 1.5, max_acc = 0.0, max_sis = 0.0.
  //   et
  //   coefficient charges variables prédominante : 1,
  //   coefficient charges variables d'accompagnement : psi0,
  //   min_pp = 1.00, min_p = 1.0, min_var = 0.0, min_acc = 0.0, min_sis = 0.0,
  //   max_pp = 1.00, max_p = 1.3, max_var = 1.3, max_acc = 0.0, max_sis = 0.0.
      case 0:
      {
        coef_min[ACTION_POIDS_PROPRE]  = 1.0;
        coef_max[ACTION_POIDS_PROPRE]  = 1.35;
        coef_min[ACTION_PRECONTRAINTE] = 1.0;
        coef_max[ACTION_PRECONTRAINTE] = 1.3;
        coef_min[ACTION_VARIABLE]      = 0.0;
        coef_max[ACTION_VARIABLE]      = 1.5;
        coef_min[ACTION_ACCIDENTELLE]  = 0.0;
        coef_max[ACTION_ACCIDENTELLE]  = 0.0;
        coef_min[ACTION_SISMIQUE]      = 0.0;
        coef_max[ACTION_SISMIQUE]      = 0.0;
        BUG (_1990_ponderations_genere_un (p,
                                           &p->combinaisons.elu_str,
                                           coef_min,
                                           coef_max,
                                           5,
                                           0,
                                           0),
             FALSE)
        
        coef_min[ACTION_POIDS_PROPRE]  = 1.0;
        coef_max[ACTION_POIDS_PROPRE]  = 1.15;
        coef_min[ACTION_PRECONTRAINTE] = 1.0;
        coef_max[ACTION_PRECONTRAINTE] = 1.3;
        coef_min[ACTION_VARIABLE]      = 0.0;
        coef_max[ACTION_VARIABLE]      = 1.5;
        coef_min[ACTION_ACCIDENTELLE]  = 0.0;
        coef_max[ACTION_ACCIDENTELLE]  = 0.0;
        coef_min[ACTION_SISMIQUE]      = 0.0;
        coef_max[ACTION_SISMIQUE]    = 0.0;
        BUG (_1990_ponderations_genere_un (p,
                                           &p->combinaisons.elu_str,
                                           coef_min,
                                           coef_max,
                                           5,
                                           -1,
                                           0),
             FALSE)
        
        coef_min[ACTION_POIDS_PROPRE]  = 1.0;
        coef_max[ACTION_POIDS_PROPRE]  = 1.0;
        coef_min[ACTION_PRECONTRAINTE] = 1.0;
        coef_max[ACTION_PRECONTRAINTE] = 1.3;
        coef_min[ACTION_VARIABLE]      = 0.0;
        coef_max[ACTION_VARIABLE]      = 1.3;
        coef_min[ACTION_ACCIDENTELLE]  = 0.0;
        coef_max[ACTION_ACCIDENTELLE]  = 0.0;
        coef_min[ACTION_SISMIQUE]      = 0.0;
        coef_max[ACTION_SISMIQUE]      = 0.0;
        BUG (_1990_ponderations_genere_un (p,
                                           &p->combinaisons.elu_str,
                                           coef_min,
                                           coef_max,
                                           5,
                                           -1,
                                           0),
             FALSE)
        
        BUG (_1990_ponderations_duplique_sans_double (&p->combinaisons.elu_geo,
                                                      p->combinaisons.elu_str),
             FALSE)
        break;
      }
  // Sinon Si selon l'approche 2 Alors
  // Pour ELU_STR ET ELU_GEO, générer les pondérations suivantes :
  //   coefficient charges variables prédominante : psi0,
  //   coefficient charges variables d'accompagnement : psi0,
  //   min_pp = 1.00, min_p = 1.0, min_var = 0.0, min_acc = 0.0, min_sis = 0.0,
  //   max_pp = 1.35, max_p = 1.3, max_var = 1.5, max_acc = 0.0, max_sis = 0.0.
  //   et
  //   coefficient charges variables prédominante : 1,
  //   coefficient charges variables d'accompagnement : psi0,
  //   min_pp = 1.00, min_p = 1.0, min_var = 0.0, min_acc = 0.0, min_sis = 0.0,
  //   max_pp = 1.15, max_p = 1.3, max_var = 1.5, max_acc = 0.0, max_sis = 0.0.
      case 2:
      {
        coef_min[ACTION_POIDS_PROPRE]  = 1.0;
        coef_max[ACTION_POIDS_PROPRE]  = 1.35;
        coef_min[ACTION_PRECONTRAINTE] = 1.0;
        coef_max[ACTION_PRECONTRAINTE] = 1.3;
        coef_min[ACTION_VARIABLE]      = 0.0;
        coef_max[ACTION_VARIABLE]      = 1.5;
        coef_min[ACTION_ACCIDENTELLE]  = 0.0;
        coef_max[ACTION_ACCIDENTELLE]  = 0.0;
        coef_min[ACTION_SISMIQUE]      = 0.0;
        coef_max[ACTION_SISMIQUE]      = 0.0;
        BUG (_1990_ponderations_genere_un (p,
                                           &p->combinaisons.elu_str,
                                           coef_min,
                                           coef_max,
                                           5,
                                           0,
                                           0),
             FALSE)
        coef_min[ACTION_POIDS_PROPRE]  = 1.0;
        coef_max[ACTION_POIDS_PROPRE]  = 1.15;
        coef_min[ACTION_PRECONTRAINTE] = 1.0;
        coef_max[ACTION_PRECONTRAINTE] = 1.3;
        coef_min[ACTION_VARIABLE]      = 0.0;
        coef_max[ACTION_VARIABLE]      = 1.5;
        coef_min[ACTION_ACCIDENTELLE]  = 0.0;
        coef_max[ACTION_ACCIDENTELLE]  = 0.0;
        coef_min[ACTION_SISMIQUE]      = 0.0;
        coef_max[ACTION_SISMIQUE]      = 0.0;
        BUG (_1990_ponderations_genere_un (p,
                                           &p->combinaisons.elu_str,
                                           coef_min,
                                           coef_max,
                                           5,
                                           -1,
                                           0),
             FALSE)
        BUG (_1990_ponderations_duplique_sans_double (&p->combinaisons.elu_geo,
                                                      p->combinaisons.elu_str),
             FALSE)
        break;
      }
  // Si selon l'approche 3 Alors
  // Pour ELU_STR, générer les pondérations suivantes :
  //   coefficient charges variables prédominante : psi0,
  //   coefficient charges variables d'accompagnement : psi0,
  //   min_pp = 1.00, min_p = 1.0, min_var = 0.0, min_acc = 0.0, min_sis = 0.0,
  //   max_pp = 1.35, max_p = 1.3, max_var = 1.5, max_acc = 0.0, max_sis = 0.0.
  //   et
  //   coefficient charges variables prédominante : 1,
  //   coefficient charges variables d'accompagnement : psi0,
  //   min_pp = 1.00, min_p = 1.0, min_var = 0.0, min_acc = 0.0, min_sis = 0.0,
  //   max_pp = 1.15, max_p = 1.3, max_var = 1.5, max_acc = 0.0, max_sis = 0.0.
      case 4:
      {
        coef_min[ACTION_POIDS_PROPRE]  = 1.0;
        coef_max[ACTION_POIDS_PROPRE]  = 1.35;
        coef_min[ACTION_PRECONTRAINTE] = 1.0;
        coef_max[ACTION_PRECONTRAINTE] = 1.3;
        coef_min[ACTION_VARIABLE]      = 0.0;
        coef_max[ACTION_VARIABLE]      = 1.5;
        coef_min[ACTION_ACCIDENTELLE]  = 0.0;
        coef_max[ACTION_ACCIDENTELLE]  = 0.0;
        coef_min[ACTION_SISMIQUE]      = 0.0;
        coef_max[ACTION_SISMIQUE]      = 0.0;
        BUG (_1990_ponderations_genere_un (p,
                                           &p->combinaisons.elu_str,
                                           coef_min,
                                           coef_max,
                                           5,
                                           0,
                                           0),
             FALSE)
        
        coef_min[ACTION_POIDS_PROPRE]  = 1.0;
        coef_max[ACTION_POIDS_PROPRE]  = 1.15;
        coef_min[ACTION_PRECONTRAINTE] = 1.0;
        coef_max[ACTION_PRECONTRAINTE] = 1.3;
        coef_min[ACTION_VARIABLE]      = 0.0;
        coef_max[ACTION_VARIABLE]      = 1.5;
        coef_min[ACTION_ACCIDENTELLE]  = 0.0;
        coef_max[ACTION_ACCIDENTELLE]  = 0.0;
        coef_min[ACTION_SISMIQUE]      = 0.0;
        coef_max[ACTION_SISMIQUE]      = 0.0;
        BUG (_1990_ponderations_genere_un (p,
                                           &p->combinaisons.elu_str,
                                           coef_min,
                                           coef_max,
                                           5,
                                           -1,
                                           0),
             FALSE)
        
  // Pour ELU_GEO, générer les pondérations suivantes :
  //   coefficient charges variables prédominante : 1,
  //   coefficient charges variables d'accompagnement : psi0,
  //   min_pp = 1.00, min_p = 1.0, min_var = 0.0, min_acc = 0.0, min_sis = 0.0,
  //   max_pp = 1.00, max_p = 1.3, max_var = 1.3, max_acc = 0.0, max_sis = 0.0.
        coef_min[ACTION_POIDS_PROPRE]  = 1.0;
        coef_max[ACTION_POIDS_PROPRE]  = 1.0;
        coef_min[ACTION_PRECONTRAINTE] = 1.0;
        coef_max[ACTION_PRECONTRAINTE] = 1.3;
        coef_min[ACTION_VARIABLE]      = 0.0;
        coef_max[ACTION_VARIABLE]      = 1.3;
        coef_min[ACTION_ACCIDENTELLE]  = 0.0;
        coef_max[ACTION_ACCIDENTELLE]  = 0.0;
        coef_min[ACTION_SISMIQUE]      = 0.0;
        coef_max[ACTION_SISMIQUE]      = 0.0;
        BUG (_1990_ponderations_genere_un (p,
                                           &p->combinaisons.elu_geo,
                                           coef_min,
                                           coef_max,
                                           5,
                                           -1,
                                           0),
             FALSE)
        break;
  // FinSi
      }
      default:
      {
        BUGMSG (0,
                FALSE,
                gettext ("Flag %d inconnu.\n"), p->combinaisons.flags)
        break;
      }
    }
  }
  // Si utilisation de la formule 6.10 de l'Eurocode 0 Alors
  else
  {
    switch (p->combinaisons.flags & 6)
    {
  // Si selon l'approche 1 Alors
  // Pour ELU_STR ET ELU_GEO, générer les pondérations suivantes :
  //   coefficient charges variables prédominante : 1,
  //   coefficient charges variables d'accompagnement : psi0,
  //   min_pp = 1.00, min_p = 1.0, min_var = 0.0, min_acc = 0.0, min_sis = 0.0,
  //   max_pp = 1.35, max_p = 1.3, max_var = 1.5, max_acc = 0.0, max_sis = 0.0.
  //   et
  //   coefficient charges variables prédominante : 1,
  //   coefficient charges variables d'accompagnement : psi0,
  //   min_pp = 1.00, min_p = 1.0, min_var = 0.0, min_acc = 0.0, min_sis = 0.0,
  //   max_pp = 1.00, max_p = 1.3, max_var = 1.3, max_acc = 0.0, max_sis = 0.0.
      case 0:
      {
        coef_min[ACTION_POIDS_PROPRE]  = 1.0;
        coef_max[ACTION_POIDS_PROPRE]  = 1.35;
        coef_min[ACTION_PRECONTRAINTE] = 1.0;
        coef_max[ACTION_PRECONTRAINTE] = 1.3;
        coef_min[ACTION_VARIABLE]      = 0.0;
        coef_max[ACTION_VARIABLE]      = 1.5;
        coef_min[ACTION_ACCIDENTELLE]  = 0.0;
        coef_max[ACTION_ACCIDENTELLE]  = 0.0;
        coef_min[ACTION_SISMIQUE]      = 0.0;
        coef_max[ACTION_SISMIQUE]      = 0.0;
        BUG (_1990_ponderations_genere_un (p,
                                           &p->combinaisons.elu_str,
                                           coef_min,
                                           coef_max,
                                           5,
                                           -1,
                                           0),
             FALSE)
        
        coef_min[ACTION_POIDS_PROPRE]  = 1.0;
        coef_max[ACTION_POIDS_PROPRE]  = 1.0;
        coef_min[ACTION_PRECONTRAINTE] = 1.0;
        coef_max[ACTION_PRECONTRAINTE] = 1.3;
        coef_min[ACTION_VARIABLE]      = 0.0;
        coef_max[ACTION_VARIABLE]      = 1.3;
        coef_min[ACTION_ACCIDENTELLE]  = 0.0;
        coef_max[ACTION_ACCIDENTELLE]  = 0.0;
        coef_min[ACTION_SISMIQUE]      = 0.0;
        coef_max[ACTION_SISMIQUE]      = 0.0;
        BUG (_1990_ponderations_genere_un (p,
                                           &p->combinaisons.elu_str,
                                           coef_min,
                                           coef_max,
                                           5,
                                           -1,
                                           0),
             FALSE)
        
        BUG (_1990_ponderations_duplique_sans_double (&p->combinaisons.elu_geo,
                                                      p->combinaisons.elu_str),
             FALSE)
        break;
      }
  // Sinon Si selon l'approche 2 Alors
  // Pour ELU_STR ET ELU_GEO, générer les pondérations suivantes :
  //   coefficient charges variables prédominante : 1,
  //   coefficient charges variables d'accompagnement : psi0,
  //   min_pp = 1.00, min_p = 1.0, min_var = 0.0, min_acc = 0.0, min_sis = 0.0,
  //   max_pp = 1.35, max_p = 1.3, max_var = 1.5, max_acc = 0.0, max_sis = 0.0.
      case 2:
      {
        coef_min[ACTION_POIDS_PROPRE]  = 1.0;
        coef_max[ACTION_POIDS_PROPRE]  = 1.35;
        coef_min[ACTION_PRECONTRAINTE] = 1.0;
        coef_max[ACTION_PRECONTRAINTE] = 1.3;
        coef_min[ACTION_VARIABLE]      = 0.0;
        coef_max[ACTION_VARIABLE]      = 1.5;
        coef_min[ACTION_ACCIDENTELLE]  = 0.0;
        coef_max[ACTION_ACCIDENTELLE]  = 0.0;
        coef_min[ACTION_SISMIQUE]      = 0.0;
        coef_max[ACTION_SISMIQUE]      = 0.0;
        BUG (_1990_ponderations_genere_un (p,
                                           &p->combinaisons.elu_str,
                                           coef_min,
                                           coef_max,
                                           5,
                                           -1,
                                           0),
             FALSE)
        BUG (_1990_ponderations_duplique_sans_double (&p->combinaisons.elu_geo,
                                                      p->combinaisons.elu_str),
             FALSE)
        break;
      }
  // Si selon l'approche 3 Alors
  // Pour ELU_STR, générer les pondérations suivantes :
  //   coefficient charges variables prédominante : 1,
  //   coefficient charges variables d'accompagnement : psi0,
  //   min_pp = 1.00, min_p = 1.0, min_var = 0.0, min_acc = 0.0, min_sis = 0.0,
  //   max_pp = 1.35, max_p = 1.3, max_var = 1.5, max_acc = 0.0, max_sis = 0.0.
      case 4:
      {
        coef_min[ACTION_POIDS_PROPRE]  = 1.0;
        coef_max[ACTION_POIDS_PROPRE]  = 1.35;
        coef_min[ACTION_PRECONTRAINTE] = 1.0;
        coef_max[ACTION_PRECONTRAINTE] = 1.3;
        coef_min[ACTION_VARIABLE]      = 0.0;
        coef_max[ACTION_VARIABLE]      = 1.5;
        coef_min[ACTION_ACCIDENTELLE]  = 0.0;
        coef_max[ACTION_ACCIDENTELLE]  = 0.0;
        coef_min[ACTION_SISMIQUE]      = 0.0;
        coef_max[ACTION_SISMIQUE]      = 0.0;
        BUG (_1990_ponderations_genere_un (p,
                                           &p->combinaisons.elu_str,
                                           coef_min,
                                           coef_max,
                                           5,
                                           -1,
                                           0),
             FALSE)
  // Pour ELU_GEO, générer les pondérations suivantes :
  //   coefficient charges variables prédominante : 1,
  //   coefficient charges variables d'accompagnement : psi0,
  //   min_pp = 1.00, min_p = 1.0, min_var = 0.0, min_acc = 0.0, min_sis = 0.0,
  //   max_pp = 1.00, max_p = 1.3, max_var = 1.3, max_acc = 0.0, max_sis = 0.0.
        coef_min[ACTION_POIDS_PROPRE]  = 1.0;
        coef_max[ACTION_POIDS_PROPRE]  = 1.0;
        coef_min[ACTION_PRECONTRAINTE] = 1.0;
        coef_max[ACTION_PRECONTRAINTE] = 1.3;
        coef_min[ACTION_VARIABLE]      = 0.0;
        coef_max[ACTION_VARIABLE]      = 1.3;
        coef_min[ACTION_ACCIDENTELLE]  = 0.0;
        coef_max[ACTION_ACCIDENTELLE]  = 0.0;
        coef_min[ACTION_SISMIQUE]      = 0.0;
        coef_max[ACTION_SISMIQUE]      = 0.0;
        BUG (_1990_ponderations_genere_un (p,
                                           &p->combinaisons.elu_geo,
                                           coef_min,
                                           coef_max,
                                           5,
                                           -1,
                                           0),
             FALSE)
        break;
      }
  // FinSi
      default :
      {
        BUGMSG (0,
                FALSE,
                gettext ("Flag %d inconnu.\n"), p->combinaisons.flags)
        break;
      }
    }
  }
  // FinSi
  
  coef_min[ACTION_POIDS_PROPRE]  = 1.0;
  coef_max[ACTION_POIDS_PROPRE]  = 1.0;
  coef_min[ACTION_PRECONTRAINTE] = 1.0;
  coef_max[ACTION_PRECONTRAINTE] = 1.0;
  coef_min[ACTION_VARIABLE]      = 0.0;
  coef_max[ACTION_VARIABLE]      = 1.0;
  coef_min[ACTION_ACCIDENTELLE]  = 1.0;
  coef_max[ACTION_ACCIDENTELLE]  = 1.0;
  coef_min[ACTION_SISMIQUE]      = 0.0;
  coef_max[ACTION_SISMIQUE]      = 0.0;
  
  // Pour ELU_ACC, générer les pondérations suivantes :
  //   min_pp = 1.00, min_p = 1.0, min_var = 0.0, min_acc = 1.0, min_sis = 0.0,
  //   max_pp = 1.00, max_p = 1.0, max_var = 1.0, max_acc = 1.0, max_sis = 0.0,
  //   Si coefficient psi1 pour les actions accidentelles Alors
  //     coefficient charges variables prédominante : psi1,
  //   Sinon
  //     coefficient charges variables prédominante : psi2,
  //   FinSi
  //   coefficient charges variables d'accompagnement : psi2.
  // FinPour
  if ((p->combinaisons.flags & 16) == 0)
    BUG (_1990_ponderations_genere_un (p,
                                       &p->combinaisons.elu_acc,
                                       coef_min,
                                       coef_max,
                                       5,
                                       1,
                                       2),
         FALSE)
  else
    BUG (_1990_ponderations_genere_un (p,
                                       &p->combinaisons.elu_acc,
                                       coef_min,
                                       coef_max,
                                       5,
                                       2,
                                       2),
         FALSE)
  
  // Pour ELU_SIS, générer les pondérations suivantes :
  //   min_pp = 1.00, min_p = 1.0, min_var = 0.0, min_acc = 0.0, min_sis = 1.0,
  //   max_pp = 1.00, max_p = 1.0, max_var = 1.0, max_acc = 0.0, max_sis = 1.0,
  //   coefficient charges variables prédominante : psi2,
  //   coefficient charges variables d'accompagnement : psi2.
  // FinPour
  coef_min[ACTION_POIDS_PROPRE]  = 1.0;
  coef_max[ACTION_POIDS_PROPRE]  = 1.0;
  coef_min[ACTION_PRECONTRAINTE] = 1.0;
  coef_max[ACTION_PRECONTRAINTE] = 1.0;
  coef_min[ACTION_VARIABLE]      = 0.0;
  coef_max[ACTION_VARIABLE]      = 1.0;
  coef_min[ACTION_ACCIDENTELLE]  = 0.0;
  coef_max[ACTION_ACCIDENTELLE]  = 0.0;
  coef_min[ACTION_SISMIQUE]      = 1.0;
  coef_max[ACTION_SISMIQUE]      = 1.0;
  BUG (_1990_ponderations_genere_un (p,
                                     &p->combinaisons.elu_sis,
                                     coef_min,
                                     coef_max,
                                     5,
                                     2,
                                     2),
       FALSE)
  
  // Pour ELU_CAR, générer les pondérations suivantes :
  //   min_pp = 1.00, min_p = 1.0, min_var = 0.0, min_acc = 0.0, min_sis = 1.0,
  //   max_pp = 1.00, max_p = 1.0, max_var = 1.0, max_acc = 0.0, max_sis = 1.0,
  //   coefficient charges variables prédominante : 1,
  //   coefficient charges variables d'accompagnement : psi0.
  // FinPour
  coef_min[ACTION_POIDS_PROPRE]  = 1.0;
  coef_max[ACTION_POIDS_PROPRE]  = 1.0;
  coef_min[ACTION_PRECONTRAINTE] = 1.0;
  coef_max[ACTION_PRECONTRAINTE] = 1.0;
  coef_min[ACTION_VARIABLE]      = 0.0;
  coef_max[ACTION_VARIABLE]      = 1.0;
  coef_min[ACTION_ACCIDENTELLE]  = 0.0;
  coef_max[ACTION_ACCIDENTELLE]  = 0.0;
  coef_min[ACTION_SISMIQUE]      = 0.0;
  coef_max[ACTION_SISMIQUE]      = 0.0;
  BUG (_1990_ponderations_genere_un (p,
                                     &p->combinaisons.els_car,
                                     coef_min,
                                     coef_max,
                                     5,
                                     -1,
                                     0),
       FALSE)
  
  // Pour ELU_FREQ, générer les pondérations suivantes :
  //   min_pp = 1.00, min_p = 1.0, min_var = 0.0, min_acc = 0.0, min_sis = 1.0,
  //   max_pp = 1.00, max_p = 1.0, max_var = 1.0, max_acc = 0.0, max_sis = 1.0,
  //   coefficient charges variables prédominante : psi1,
  //   coefficient charges variables d'accompagnement : psi2.
  // FinPour
  coef_min[ACTION_POIDS_PROPRE]  = 1.0;
  coef_max[ACTION_POIDS_PROPRE]  = 1.0;
  coef_min[ACTION_PRECONTRAINTE] = 1.0;
  coef_max[ACTION_PRECONTRAINTE] = 1.0;
  coef_min[ACTION_VARIABLE]      = 0.0;
  coef_max[ACTION_VARIABLE]      = 1.0;
  coef_min[ACTION_ACCIDENTELLE]  = 0.0;
  coef_max[ACTION_ACCIDENTELLE]  = 0.0;
  coef_min[ACTION_SISMIQUE]      = 0.0;
  coef_max[ACTION_SISMIQUE]      = 0.0;
  BUG (_1990_ponderations_genere_un (p,
                                     &p->combinaisons.els_freq,
                                     coef_min,
                                     coef_max,
                                     5,
                                     1,
                                     2),
       FALSE)
  
  // Pour ELU_PERM, générer les pondérations suivantes :
  //   min_pp = 1.00, min_p = 1.0, min_var = 0.0, min_acc = 0.0, min_sis = 1.0,
  //   max_pp = 1.00, max_p = 1.0, max_var = 1.0, max_acc = 0.0, max_sis = 1.0,
  //   coefficient charges variables prédominante : psi2,
  //   coefficient charges variables d'accompagnement : psi2.
  // FinPour
  coef_min[ACTION_POIDS_PROPRE]  = 1.0;
  coef_max[ACTION_POIDS_PROPRE]  = 1.0;
  coef_min[ACTION_PRECONTRAINTE] = 1.0;
  coef_max[ACTION_PRECONTRAINTE] = 1.0;
  coef_min[ACTION_VARIABLE]      = 0.0;
  coef_max[ACTION_VARIABLE]      = 1.0;
  coef_min[ACTION_ACCIDENTELLE]  = 0.0;
  coef_max[ACTION_ACCIDENTELLE]  = 0.0;
  coef_min[ACTION_SISMIQUE]      = 0.0;
  coef_max[ACTION_SISMIQUE]      = 0.0;
  BUG (_1990_ponderations_genere_un (p,
                                     &p->combinaisons.els_perm,
                                     coef_min,
                                     coef_max,
                                     5,
                                     2,
                                     2),
       FALSE)
  
  return TRUE;
}


gboolean
_1990_ponderations_genere_fr (Projet *p)
/**
 * \brief Génération de l'ensemble des pondérations selon la norme française.
 *        La fonction #_1990_ponderations_genere_un est appelé autant de fois
 *        que nécessaire avec les coefficients min/max ajustées en fonction des
 *        valeur de la norme française et de la nature de l'état limite
 *        recherché. Les options de calculs sont définies dans la variable
 *        p->combinaisons.flags et doivent être définies.
 * \brief p : la variable projet.
 * \return
 *   Succès : FALSE.\n
 *   Échec : TRUE :
 *     - p == NULL,
 *     - #_1990_ponderations_genere_un,
 *     - #_1990_ponderations_duplique_sans_double.
 */
{
  double  coef_min[ACTION_INCONNUE], coef_max[ACTION_INCONNUE];
  
  // Les indices ont les définitions suivantes : pp = poids propre,
  // p = précontrainte, var = variable, acc = accidentelle,
  // sis = sismique et es = eaux souterraines.
  
  BUGMSG (p, FALSE, gettext ("Paramètre %s incorrect.\n"), "projet")
  
  // Pour ELU_EQU, générer les pondérations suivantes :
  //   Si à l'équilibre seulement Alors
  //     coefficient charges variables prédominante : 1.
  //     coefficient charges variables d'accompagnement : psi0.
  //     min_pp = 0.9, min_p = 1.0, min_var = 0.0, min_acc = 0.0,
  //     min_sis = 0.0, min_es = 0.0,
  //     max_pp = 1.1, max_p = 1.3, max_var = 1.5, max_acc = 0.0,
  //     max_sis = 0.0, max_es = 1.2.
  if ((p->combinaisons.flags & 1) == 0)
  {
    coef_min[ACTION_POIDS_PROPRE]      = 0.9;
    coef_max[ACTION_POIDS_PROPRE]      = 1.1;
    coef_min[ACTION_PRECONTRAINTE]     = 1.0;
    coef_max[ACTION_PRECONTRAINTE]     = 1.3;
    coef_min[ACTION_VARIABLE]          = 0.0;
    coef_max[ACTION_VARIABLE]          = 1.5;
    coef_min[ACTION_ACCIDENTELLE]      = 0.0;
    coef_max[ACTION_ACCIDENTELLE]      = 0.0;
    coef_min[ACTION_SISMIQUE]          = 0.0;
    coef_max[ACTION_SISMIQUE]          = 0.0;
    coef_min[ACTION_EAUX_SOUTERRAINES] = 0.0;
    coef_max[ACTION_EAUX_SOUTERRAINES] = 1.2;
    BUG (_1990_ponderations_genere_un (p,
                                       &p->combinaisons.elu_equ,
                                       coef_min,
                                       coef_max,
                                       6,
                                       -1,
                                       0),
         FALSE)
  }
  else
  //   Sinon (à l'équilibre et à la résistance structurelle)
  //     coefficient charges variables prédominante : 1,
  //     coefficient charges variables d'accompagnement : psi0,
  //     min_pp = 1.15, min_p = 1.0, min_var = 0.0, min_acc = 0.0,
  //     min_sis = 0.0, min_es = 0.0,
  //     max_pp = 1.35, max_p = 1.3, max_var = 1.5, max_acc = 0.0,
  //     max_sis = 0.0, max_es = 1.2.
  //     et
  //     coefficient charges variables prédominante : 1,
  //     coefficient charges variables d'accompagnement : psi0,
  //     min_pp = 1.00, min_p = 1.0, min_var = 0.0, min_acc = 0.0,
  //     min_sis = 0.0, min_es = 0.0,
  //     max_pp = 1.00, max_p = 1.3, max_var = 1.5, max_acc = 0.0,
  //     max_sis = 0.0, max_es = 1.2.
  //   FinSinon
  // FinPour
  {
    coef_min[ACTION_POIDS_PROPRE]      = 1.15;
    coef_max[ACTION_POIDS_PROPRE]      = 1.35;
    coef_min[ACTION_PRECONTRAINTE]     = 1.0;
    coef_max[ACTION_PRECONTRAINTE]     = 1.3;
    coef_min[ACTION_VARIABLE]          = 0.0;
    coef_max[ACTION_VARIABLE]          = 1.5;
    coef_min[ACTION_ACCIDENTELLE]      = 0.0;
    coef_max[ACTION_ACCIDENTELLE]      = 0.0;
    coef_min[ACTION_SISMIQUE]          = 0.0;
    coef_max[ACTION_SISMIQUE]          = 0.0;
    coef_min[ACTION_EAUX_SOUTERRAINES] = 0.0;
    coef_max[ACTION_EAUX_SOUTERRAINES] = 1.2;
    BUG (_1990_ponderations_genere_un (p,
                                       &p->combinaisons.elu_equ,
                                       coef_min,
                                       coef_max,
                                       6,
                                       -1,
                                       0),
         FALSE)
    
    coef_min[ACTION_POIDS_PROPRE]      = 1.0;
    coef_max[ACTION_POIDS_PROPRE]      = 1.0;
    coef_min[ACTION_PRECONTRAINTE]     = 1.0;
    coef_max[ACTION_PRECONTRAINTE]     = 1.3;
    coef_min[ACTION_VARIABLE]          = 0.0;
    coef_max[ACTION_VARIABLE]          = 1.5;
    coef_min[ACTION_ACCIDENTELLE]      = 0.0;
    coef_max[ACTION_ACCIDENTELLE]      = 0.0;
    coef_min[ACTION_SISMIQUE]          = 0.0;
    coef_max[ACTION_SISMIQUE]          = 1.0;
    coef_min[ACTION_EAUX_SOUTERRAINES] = 0.0;
    coef_max[ACTION_EAUX_SOUTERRAINES] = 1.2;
    BUG (_1990_ponderations_genere_un (p,
                                       &p->combinaisons.elu_equ,
                                       coef_min,
                                       coef_max,
                                       6,
                                       -1,
                                       0),
         FALSE)
  }
  
  // Si utilisation des formules 6.10a et 6.10b de l'Eurocode 0 Alors
  if ((p->combinaisons.flags & 8) == 0)
  {
    switch (p->combinaisons.flags & 6)
    {
  //   Si selon l'approche 1 Alors
  //     Pour ELU_STR ET ELU_GEO, générer les pondérations suivantes :
  //     coefficient charges variables prédominante : psi0,
  //     coefficient charges variables d'accompagnement : psi0,
  //     min_pp = 1.00, min_p = 1.0, min_var = 0.0, min_acc = 0.0,
  //     min_sis = 0.0, min_es = 0.0,
  //     max_pp = 1.35, max_p = 1.3, max_var = 1.5, max_acc = 0.0,
  //     max_sis = 0.0, max_es = 1.2.
  //     et
  //     coefficient charges variables prédominante : 1,
  //     coefficient charges variables d'accompagnement : psi0,
  //     min_pp = 1.00, min_p = 1.0, min_var = 0.0, min_acc = 0.0,
  //     min_sis = 0.0, min_es = 0.0,
  //     max_pp = 1.15, max_p = 1.3, max_var = 1.5, max_acc = 0.0,
  //     max_sis = 0.0, max_es = 1.2.
  //     et
  //     coefficient charges variables prédominante : 1,
  //     coefficient charges variables d'accompagnement : psi0,
  //     min_pp = 1.00, min_p = 1.0, min_var = 0.0, min_acc = 0.0,
  //     min_sis = 0.0, min_es = 0.0,
  //     max_pp = 1.00, max_p = 1.3, max_var = 1.5, max_acc = 0.0,
  //     max_sis = 0.0, max_es = 1.2.
      case 0:
      {
        coef_min[ACTION_POIDS_PROPRE]      = 1.0;
        coef_max[ACTION_POIDS_PROPRE]      = 1.35;
        coef_min[ACTION_PRECONTRAINTE]     = 1.0;
        coef_max[ACTION_PRECONTRAINTE]     = 1.3;
        coef_min[ACTION_VARIABLE]          = 0.0;
        coef_max[ACTION_VARIABLE]          = 1.5;
        coef_min[ACTION_ACCIDENTELLE]      = 0.0;
        coef_max[ACTION_ACCIDENTELLE]      = 0.0;
        coef_min[ACTION_SISMIQUE]          = 0.0;
        coef_max[ACTION_SISMIQUE]          = 0.0;
        coef_min[ACTION_EAUX_SOUTERRAINES] = 0.0;
        coef_max[ACTION_EAUX_SOUTERRAINES] = 1.2;
        BUG (_1990_ponderations_genere_un (p,
                                           &p->combinaisons.elu_str,
                                           coef_min,
                                           coef_max,
                                           6,
                                           0,
                                           0),
             FALSE)
        
        coef_min[ACTION_POIDS_PROPRE]      = 1.0;
        coef_max[ACTION_POIDS_PROPRE]      = 1.15;
        coef_min[ACTION_PRECONTRAINTE]     = 1.0;
        coef_max[ACTION_PRECONTRAINTE]     = 1.3;
        coef_min[ACTION_VARIABLE]          = 0.0;
        coef_max[ACTION_VARIABLE]          = 1.5;
        coef_min[ACTION_ACCIDENTELLE]      = 0.0;
        coef_max[ACTION_ACCIDENTELLE]      = 0.0;
        coef_min[ACTION_SISMIQUE]          = 0.0;
        coef_max[ACTION_SISMIQUE]          = 0.0;
        coef_min[ACTION_EAUX_SOUTERRAINES] = 0.0;
        coef_max[ACTION_EAUX_SOUTERRAINES] = 1.2;
        BUG (_1990_ponderations_genere_un (p,
                                           &p->combinaisons.elu_str,
                                           coef_min,
                                           coef_max,
                                           6,
                                           -1,
                                           0),
             FALSE)
        
        coef_min[ACTION_POIDS_PROPRE]      = 1.0;
        coef_max[ACTION_POIDS_PROPRE]      = 1.0;
        coef_min[ACTION_PRECONTRAINTE]     = 1.0;
        coef_max[ACTION_PRECONTRAINTE]     = 1.3;
        coef_min[ACTION_VARIABLE]          = 0.0;
        coef_max[ACTION_VARIABLE]          = 1.3;
        coef_min[ACTION_ACCIDENTELLE]      = 0.0;
        coef_max[ACTION_ACCIDENTELLE]      = 0.0;
        coef_min[ACTION_SISMIQUE]          = 0.0;
        coef_max[ACTION_SISMIQUE]          = 0.0;
        coef_min[ACTION_EAUX_SOUTERRAINES] = 0.0;
        coef_max[ACTION_EAUX_SOUTERRAINES] = 1.2;
        BUG (_1990_ponderations_genere_un (p,
                                           &p->combinaisons.elu_str,
                                           coef_min,
                                           coef_max,
                                           6,
                                           -1,
                                           0),
             FALSE)
        
        BUG (_1990_ponderations_duplique_sans_double (&p->combinaisons.elu_geo,
                                                      p->combinaisons.elu_str),
             FALSE)
        break;
      }
  //   Sinon Si selon l'approche 2 Alors
  //     Pour ELU_STR ET ELU_GEO, générer les pondérations suivantes :
  //     coefficient charges variables prédominante : psi0,
  //     coefficient charges variables d'accompagnement : psi0,
  //     min_pp = 1.00, min_p = 1.0, min_var = 0.0, min_acc = 0.0,
  //     min_sis = 0.0, min_es = 0.0,
  //     max_pp = 1.35, max_p = 1.3, max_var = 1.5, max_acc = 0.0,
  //     max_sis = 0.0, max_es = 1.2.
  //     et
  //     coefficient charges variables prédominante : 1,
  //     coefficient charges variables d'accompagnement : psi0,
  //     min_pp = 1.00, min_p = 1.0, min_var = 0.0, min_acc = 0.0,
  //     min_sis = 0.0, min_es = 0.0,
  //     max_pp = 1.15, max_p = 1.3, max_var = 1.5, max_acc = 0.0,
  //     max_sis = 0.0, max_es = 1.2.
      case 2:
      {
        coef_min[ACTION_POIDS_PROPRE]      = 1.0;
        coef_max[ACTION_POIDS_PROPRE]      = 1.35;
        coef_min[ACTION_PRECONTRAINTE]     = 1.0;
        coef_max[ACTION_PRECONTRAINTE]     = 1.3;
        coef_min[ACTION_VARIABLE]          = 0.0;
        coef_max[ACTION_VARIABLE]          = 1.5;
        coef_min[ACTION_ACCIDENTELLE]      = 0.0;
        coef_max[ACTION_ACCIDENTELLE]      = 0.0;
        coef_min[ACTION_SISMIQUE]          = 0.0;
        coef_max[ACTION_SISMIQUE]          = 0.0;
        coef_min[ACTION_EAUX_SOUTERRAINES] = 0.0;
        coef_max[ACTION_EAUX_SOUTERRAINES] = 1.2;
        BUG (_1990_ponderations_genere_un (p,
                                           &p->combinaisons.elu_str,
                                           coef_min,
                                           coef_max,
                                           6,
                                           0,
                                           0),
             FALSE)
        
        coef_min[ACTION_POIDS_PROPRE]      = 1.0;
        coef_max[ACTION_POIDS_PROPRE]      = 1.15;
        coef_min[ACTION_PRECONTRAINTE]     = 1.0;
        coef_max[ACTION_PRECONTRAINTE]     = 1.3;
        coef_min[ACTION_VARIABLE]          = 0.0;
        coef_max[ACTION_VARIABLE]          = 1.5;
        coef_min[ACTION_ACCIDENTELLE]      = 0.0;
        coef_max[ACTION_ACCIDENTELLE]      = 0.0;
        coef_min[ACTION_SISMIQUE]          = 0.0;
        coef_max[ACTION_SISMIQUE]          = 0.0;
        coef_min[ACTION_EAUX_SOUTERRAINES] = 0.0;
        coef_max[ACTION_EAUX_SOUTERRAINES] = 1.2;
        BUG (_1990_ponderations_genere_un (p,
                                           &p->combinaisons.elu_str,
                                           coef_min,
                                           coef_max,
                                           6,
                                           -1,
                                           0),
             FALSE)
        
        BUG (_1990_ponderations_duplique_sans_double (&p->combinaisons.elu_geo,
                                                      p->combinaisons.elu_str),
             FALSE)
        break;
      }
  //   Si selon l'approche 3 Alors
  //     Pour ELU_STR, générer les pondérations suivantes :
  //     coefficient charges variables prédominante : psi0,
  //     coefficient charges variables d'accompagnement : psi0,
  //     min_pp = 1.00, min_p = 1.0, min_var = 0.0, min_acc = 0.0,
  //     min_sis = 0.0, min_es = 0.0,
  //     max_pp = 1.35, max_p = 1.3, max_var = 1.5, max_acc = 0.0,
  //     max_sis = 0.0, max_es = 1.2.
  //     et
  //     coefficient charges variables prédominante : 1,
  //     coefficient charges variables d'accompagnement : psi0,
  //     min_pp = 1.00, min_p = 1.0, min_var = 0.0, min_acc = 0.0,
  //     min_sis = 0.0, min_es = 0.0,
  //     max_pp = 1.15, max_p = 1.3, max_var = 1.5, max_acc = 0.0,
  //     max_sis = 0.0, max_es = 1.2.
      case 4:
      {
        coef_min[ACTION_POIDS_PROPRE]      = 1.0;
        coef_max[ACTION_POIDS_PROPRE]      = 1.35;
        coef_min[ACTION_PRECONTRAINTE]     = 1.0;
        coef_max[ACTION_PRECONTRAINTE]     = 1.3;
        coef_min[ACTION_VARIABLE]          = 0.0;
        coef_max[ACTION_VARIABLE]          = 1.5;
        coef_min[ACTION_ACCIDENTELLE]      = 0.0;
        coef_max[ACTION_ACCIDENTELLE]      = 0.0;
        coef_min[ACTION_SISMIQUE]          = 0.0;
        coef_max[ACTION_SISMIQUE]          = 0.0;
        coef_min[ACTION_EAUX_SOUTERRAINES] = 0.0;
        coef_max[ACTION_EAUX_SOUTERRAINES] = 1.2;
        BUG (_1990_ponderations_genere_un (p,
                                           &p->combinaisons.elu_str,
                                           coef_min,
                                           coef_max,
                                           6,
                                           0,
                                           0),
             FALSE)
        
        coef_min[ACTION_POIDS_PROPRE]      = 1.0;
        coef_max[ACTION_POIDS_PROPRE]      = 1.15;
        coef_min[ACTION_PRECONTRAINTE]     = 1.0;
        coef_max[ACTION_PRECONTRAINTE]     = 1.3;
        coef_min[ACTION_VARIABLE]          = 0.0;
        coef_max[ACTION_VARIABLE]          = 1.5;
        coef_min[ACTION_ACCIDENTELLE]      = 0.0;
        coef_max[ACTION_ACCIDENTELLE]      = 0.0;
        coef_min[ACTION_SISMIQUE]          = 0.0;
        coef_max[ACTION_SISMIQUE]          = 0.0;
        coef_min[ACTION_EAUX_SOUTERRAINES] = 0.0;
        coef_max[ACTION_EAUX_SOUTERRAINES] = 1.2;
        BUG (_1990_ponderations_genere_un (p,
                                           &p->combinaisons.elu_str,
                                           coef_min,
                                           coef_max,
                                           6,
                                           -1,
                                           0),
             FALSE)
        
  //     Pour ELU_GEO, générer les pondérations suivantes :
  //     coefficient charges variables prédominante : 1,
  //     coefficient charges variables d'accompagnement : psi0,
  //     min_pp = 1.00, min_p = 1.0, min_var = 0.0, min_acc = 0.0,
  //     min_sis = 0.0, min_es = 0.0,
  //     max_pp = 1.00, max_p = 1.3, max_var = 1.3, max_acc = 0.0,
  //     max_sis = 0.0, max_es = 1.2.
        coef_min[ACTION_POIDS_PROPRE]      = 1.0;
        coef_max[ACTION_POIDS_PROPRE]      = 1.0;
        coef_min[ACTION_PRECONTRAINTE]     = 1.0;
        coef_max[ACTION_PRECONTRAINTE]     = 1.3;
        coef_min[ACTION_VARIABLE]          = 0.0;
        coef_max[ACTION_VARIABLE]          = 1.3;
        coef_min[ACTION_ACCIDENTELLE]      = 0.0;
        coef_max[ACTION_ACCIDENTELLE]      = 0.0;
        coef_min[ACTION_SISMIQUE]          = 0.0;
        coef_max[ACTION_SISMIQUE]          = 0.0;
        coef_min[ACTION_EAUX_SOUTERRAINES] = 0.0;
        coef_max[ACTION_EAUX_SOUTERRAINES] = 1.2;
        BUG (_1990_ponderations_genere_un (p,
                                           &p->combinaisons.elu_geo,
                                           coef_min,
                                           coef_max,
                                           6,
                                           -1,
                                           0),
             FALSE)
        break;
  //   FinSi
      }
      default :
      {
        BUGMSG (0, 
                FALSE,
                gettext ("Flag %d inconnu.\n"), p->combinaisons.flags)
        break;
      }
    }
  }
  // Si utilisation de la formule 6.10 de l'Eurocode 0 Alors
  else
  // équation 6.10
  {
    switch (p->combinaisons.flags & 6)
    {
  //   Si selon l'approche 1 Alors
  //     Pour ELU_STR ET ELU_GEO, générer les pondérations suivantes :
  //     coefficient charges variables prédominante : 1,
  //     coefficient charges variables d'accompagnement : psi0,
  //     min_pp = 1.00, min_p = 1.0, min_var = 0.0, min_acc = 0.0,
  //     min_sis = 0.0, min_es = 0.0,
  //     max_pp = 1.35, max_p = 1.3, max_var = 1.5, max_acc = 0.0,
  //     max_sis = 0.0, max_es = 1.2.
  //     et
  //     coefficient charges variables prédominante : 1,
  //     coefficient charges variables d'accompagnement : psi0,
  //     min_pp = 1.00, min_p = 1.0, min_var = 0.0, min_acc = 0.0,
  //     min_sis = 0.0, min_es = 0.0,
  //     max_pp = 1.00, max_p = 1.3, max_var = 1.3, max_acc = 0.0,
  //     max_sis = 0.0, max_es = 1.2.
      case 0:
      {
        coef_min[ACTION_POIDS_PROPRE]      = 1.0;
        coef_max[ACTION_POIDS_PROPRE]      = 1.35;
        coef_min[ACTION_PRECONTRAINTE]     = 1.0;
        coef_max[ACTION_PRECONTRAINTE]     = 1.3;
        coef_min[ACTION_VARIABLE]          = 0.0;
        coef_max[ACTION_VARIABLE]          = 1.5;
        coef_min[ACTION_ACCIDENTELLE]      = 0.0;
        coef_max[ACTION_ACCIDENTELLE]      = 0.0;
        coef_min[ACTION_SISMIQUE]          = 0.0;
        coef_max[ACTION_SISMIQUE]          = 0.0;
        coef_min[ACTION_EAUX_SOUTERRAINES] = 0.0;
        coef_max[ACTION_EAUX_SOUTERRAINES] = 1.2;
        BUG (_1990_ponderations_genere_un (p,
                                           &p->combinaisons.elu_str,
                                           coef_min,
                                           coef_max,
                                           6,
                                           -1,
                                           0),
             FALSE)
        
        coef_min[ACTION_POIDS_PROPRE]      = 1.0;
        coef_max[ACTION_POIDS_PROPRE]      = 1.0;
        coef_min[ACTION_PRECONTRAINTE]     = 1.0;
        coef_max[ACTION_PRECONTRAINTE]     = 1.3;
        coef_min[ACTION_VARIABLE]          = 0.0;
        coef_max[ACTION_VARIABLE]          = 1.3;
        coef_min[ACTION_ACCIDENTELLE]      = 0.0;
        coef_max[ACTION_ACCIDENTELLE]      = 0.0;
        coef_min[ACTION_SISMIQUE]          = 0.0;
        coef_max[ACTION_SISMIQUE]          = 0.0;
        coef_min[ACTION_EAUX_SOUTERRAINES] = 0.0;
        coef_max[ACTION_EAUX_SOUTERRAINES] = 1.2;
        BUG (_1990_ponderations_genere_un (p,
                                           &p->combinaisons.elu_str,
                                           coef_min,
                                           coef_max,
                                           6,
                                           -1,
                                           0),
             FALSE)
        
        BUG (_1990_ponderations_duplique_sans_double (&p->combinaisons.elu_geo,
                                                      p->combinaisons.elu_str),
             FALSE)
        break;
      }
  //   Sinon Si selon l'approche 2 Alors
  //     Pour ELU_STR ET ELU_GEO, générer les pondérations suivantes :
  //     coefficient charges variables prédominante : 1,
  //     coefficient charges variables d'accompagnement : psi0,
  //     min_pp = 1.00, min_p = 1.0, min_var = 0.0, min_acc = 0.0,
  //     min_sis = 0.0, min_es = 0.0,
  //     max_pp = 1.35, max_p = 1.3, max_var = 1.5, max_acc = 0.0,
  //     max_sis = 0.0, max_es = 1.2.
      case 2:
      {
        coef_min[ACTION_POIDS_PROPRE]      = 1.0;
        coef_max[ACTION_POIDS_PROPRE]      = 1.35;
        coef_min[ACTION_PRECONTRAINTE]     = 1.0;
        coef_max[ACTION_PRECONTRAINTE]     = 1.3;
        coef_min[ACTION_VARIABLE]          = 0.0;
        coef_max[ACTION_VARIABLE]          = 1.5;
        coef_min[ACTION_ACCIDENTELLE]      = 0.0;
        coef_max[ACTION_ACCIDENTELLE]      = 0.0;
        coef_min[ACTION_SISMIQUE]          = 0.0;
        coef_max[ACTION_SISMIQUE]          = 0.0;
        coef_min[ACTION_EAUX_SOUTERRAINES] = 0.0;
        coef_max[ACTION_EAUX_SOUTERRAINES] = 1.2;
        BUG (_1990_ponderations_genere_un (p,
                                           &p->combinaisons.elu_str,
                                           coef_min,
                                           coef_max,
                                           6,
                                           -1,
                                           0),
             FALSE)
        
        BUG (_1990_ponderations_duplique_sans_double (&p->combinaisons.elu_geo,
                                                      p->combinaisons.elu_str),
             FALSE)
        break;
      }
  //   Si selon l'approche 3 Alors
  //     Pour ELU_STR, générer les pondérations suivantes :
  //     coefficient charges variables prédominante : 1,
  //     coefficient charges variables d'accompagnement : psi0,
  //     min_pp = 1.00, min_p = 1.0, min_var = 0.0, min_acc = 0.0,
  //     min_sis = 0.0, min_es = 0.0,
  //     max_pp = 1.35, max_p = 1.3, max_var = 1.5, max_acc = 0.0,
  //     max_sis = 0.0, max_es = 1.2.
      case 4:
      {
        coef_min[ACTION_POIDS_PROPRE]      = 1.0;
        coef_max[ACTION_POIDS_PROPRE]      = 1.35;
        coef_min[ACTION_PRECONTRAINTE]     = 1.0;
        coef_max[ACTION_PRECONTRAINTE]     = 1.3;
        coef_min[ACTION_VARIABLE]          = 0.0;
        coef_max[ACTION_VARIABLE]          = 1.5;
        coef_min[ACTION_ACCIDENTELLE]      = 0.0;
        coef_max[ACTION_ACCIDENTELLE]      = 0.0;
        coef_min[ACTION_SISMIQUE]          = 0.0;
        coef_max[ACTION_SISMIQUE]          = 0.0;
        coef_min[ACTION_EAUX_SOUTERRAINES] = 0.0;
        coef_max[ACTION_EAUX_SOUTERRAINES] = 1.2;
        BUG (_1990_ponderations_genere_un (p,
                                           &p->combinaisons.elu_str,
                                           coef_min,
                                           coef_max,
                                           6,
                                           -1,
                                           0),
             FALSE)
        
  //     Pour ELU_GEO, générer les pondérations suivantes :
  //     coefficient charges variables prédominante : 1,
  //     coefficient charges variables d'accompagnement : psi0,
  //     min_pp = 1.00, min_p = 1.0, min_var = 0.0, min_acc = 0.0,
  //     min_sis = 0.0, min_es = 0.0,
  //     max_pp = 1.00, max_p = 1.3, max_var = 1.3, max_acc = 0.0,
  //     max_sis = 0.0, max_es = 1.2.
        coef_min[ACTION_POIDS_PROPRE]      = 1.0;
        coef_max[ACTION_POIDS_PROPRE]      = 1.0;
        coef_min[ACTION_PRECONTRAINTE]     = 1.0;
        coef_max[ACTION_PRECONTRAINTE]     = 1.3;
        coef_min[ACTION_VARIABLE]          = 0.0;
        coef_max[ACTION_VARIABLE]          = 1.3;
        coef_min[ACTION_ACCIDENTELLE]      = 0.0;
        coef_max[ACTION_ACCIDENTELLE]      = 0.0;
        coef_min[ACTION_SISMIQUE]          = 0.0;
        coef_max[ACTION_SISMIQUE]          = 0.0;
        coef_min[ACTION_EAUX_SOUTERRAINES] = 0.0;
        coef_max[ACTION_EAUX_SOUTERRAINES] = 1.2;
        BUG (_1990_ponderations_genere_un (p,
                                           &p->combinaisons.elu_geo,
                                           coef_min,
                                           coef_max,
                                           6,
                                           -1,
                                           0),
             FALSE)
        break;
      }
  //   FinSi
      default :
      {
        BUGMSG (0,
                FALSE,
                gettext ("Flag %d inconnu.\n"), p->combinaisons.flags)
        break;
      }
    }
  }
  // FinSi
  
  coef_min[ACTION_POIDS_PROPRE]      = 1.0;
  coef_max[ACTION_POIDS_PROPRE]      = 1.0;
  coef_min[ACTION_PRECONTRAINTE]     = 1.0;
  coef_max[ACTION_PRECONTRAINTE]     = 1.0;
  coef_min[ACTION_VARIABLE]          = 0.0;
  coef_max[ACTION_VARIABLE]          = 1.0;
  coef_min[ACTION_ACCIDENTELLE]      = 1.0;
  coef_max[ACTION_ACCIDENTELLE]      = 1.0;
  coef_min[ACTION_SISMIQUE]          = 0.0;
  coef_max[ACTION_SISMIQUE]          = 0.0;
  coef_min[ACTION_EAUX_SOUTERRAINES] = 0.0;
  coef_max[ACTION_EAUX_SOUTERRAINES] = 1.0;
  // Pour ELU_ACC, générer les pondérations suivantes :
  //   min_pp = 1.00, min_p = 1.0, min_var = 0.0, min_acc = 1.0,
  //   min_sis = 0.0, min_es = 0.0,
  //   max_pp = 1.00, max_p = 1.0, max_var = 1.0, max_acc = 1.0,
  //   max_sis = 0.0, max_es = 1.0.
  //   Si coefficient psi1 pour les actions accidentelles Alors
  //     coefficient charges variables prédominante : psi1,
  //   Sinon
  //     coefficient charges variables prédominante : psi2.
  //   FinSi
  //   coefficient charges variables d'accompagnement : psi2.
  // FinPour
  if ((p->combinaisons.flags & 16) == 0)
    BUG (_1990_ponderations_genere_un (p,
                                       &p->combinaisons.elu_acc,
                                       coef_min,
                                       coef_max,
                                       6,
                                       1,
                                       2),
         FALSE)
  else
    BUG (_1990_ponderations_genere_un (p,
                                       &p->combinaisons.elu_acc,
                                       coef_min,
                                       coef_max,
                                       6,
                                       2,
                                       2),
         FALSE)
  
  // Pour ELU_SIS, générer les pondérations suivantes :
  //   coefficient charges variables prédominante : psi2,
  //   coefficient charges variables d'accompagnement : psi2,
  //   min_pp = 1.00, min_p = 1.0, min_var = 0.0, min_acc = 0.0,
  //   min_sis = 1.0, min_es = 0.0,
  //   max_pp = 1.00, max_p = 1.0, max_var = 1.0, max_acc = 0.0,
  //   max_sis = 1.0, max_es = 1.0.
  // FinPour
  coef_min[ACTION_POIDS_PROPRE]      = 1.0;
  coef_max[ACTION_POIDS_PROPRE]      = 1.0;
  coef_min[ACTION_PRECONTRAINTE]     = 1.0;
  coef_max[ACTION_PRECONTRAINTE]     = 1.0;
  coef_min[ACTION_VARIABLE]          = 0.0;
  coef_max[ACTION_VARIABLE]          = 1.0;
  coef_min[ACTION_ACCIDENTELLE]      = 0.0;
  coef_max[ACTION_ACCIDENTELLE]      = 0.0;
  coef_min[ACTION_SISMIQUE]          = 1.0;
  coef_max[ACTION_SISMIQUE]          = 1.0;
  coef_min[ACTION_EAUX_SOUTERRAINES] = 0.0;
  coef_max[ACTION_EAUX_SOUTERRAINES] = 1.0;
  BUG (_1990_ponderations_genere_un (p,
                                     &p->combinaisons.elu_acc,
                                     coef_min,
                                     coef_max,
                                     6,
                                     2,
                                     2),
       FALSE)
  
  // Pour ELS_CAR, générer les pondérations suivantes :
  //   coefficient charges variables prédominante : 1,
  //   coefficient charges variables d'accompagnement : psi0,
  //   min_pp = 1.00, min_p = 1.0, min_var = 0.0, min_acc = 0.0,
  //   min_sis = 0.0, min_es = 0.0,
  //   max_pp = 1.00, max_p = 1.0, max_var = 1.0, max_acc = 0.0,
  //   max_sis = 0.0, max_es = 1.2.
  // FinPour
  coef_min[ACTION_POIDS_PROPRE]      = 1.0;
  coef_max[ACTION_POIDS_PROPRE]      = 1.0;
  coef_min[ACTION_PRECONTRAINTE]     = 1.0;
  coef_max[ACTION_PRECONTRAINTE]     = 1.0;
  coef_min[ACTION_VARIABLE]          = 0.0;
  coef_max[ACTION_VARIABLE]          = 1.0;
  coef_min[ACTION_ACCIDENTELLE]      = 0.0;
  coef_max[ACTION_ACCIDENTELLE]      = 0.0;
  coef_min[ACTION_SISMIQUE]          = 0.0;
  coef_max[ACTION_SISMIQUE]          = 0.0;
  coef_min[ACTION_EAUX_SOUTERRAINES] = 0.0;
  coef_max[ACTION_EAUX_SOUTERRAINES] = 1.2;
  BUG (_1990_ponderations_genere_un (p,
                                     &p->combinaisons.els_car,
                                     coef_min,
                                     coef_max,
                                     6,
                                     -1,
                                     0),
       FALSE)

  // Pour ELS_FREQ, générer les pondérations suivantes :
  //   coefficient charges variables prédominante : psi1,
  //   coefficient charges variables d'accompagnement : psi2,
  //   min_pp = 1.00, min_p = 1.0, min_var = 0.0, min_acc = 0.0,
  //   min_sis = 0.0, min_es = 0.0,
  //   max_pp = 1.00, max_p = 1.0, max_var = 1.0, max_acc = 0.0,
  //   max_sis = 0.0, max_es = 1.0.
  // FinPour
  coef_min[ACTION_POIDS_PROPRE]      = 1.0;
  coef_max[ACTION_POIDS_PROPRE]      = 1.0;
  coef_min[ACTION_PRECONTRAINTE]     = 1.0;
  coef_max[ACTION_PRECONTRAINTE]     = 1.0;
  coef_min[ACTION_VARIABLE]          = 0.0;
  coef_max[ACTION_VARIABLE]          = 1.0;
  coef_min[ACTION_ACCIDENTELLE]      = 0.0;
  coef_max[ACTION_ACCIDENTELLE]      = 0.0;
  coef_min[ACTION_SISMIQUE]          = 0.0;
  coef_max[ACTION_SISMIQUE]          = 0.0;
  coef_min[ACTION_EAUX_SOUTERRAINES] = 0.0;
  coef_max[ACTION_EAUX_SOUTERRAINES] = 1.0;
  BUG (_1990_ponderations_genere_un (p,
                                     &p->combinaisons.els_freq,
                                     coef_min,
                                     coef_max,
                                     6,
                                     1,
                                     2),
       FALSE)

  // Pour ELS_PERM, générer les pondérations suivantes :
  //   coefficient charges variables prédominante : psi2,
  //   coefficient charges variables d'accompagnement : psi2,
  //   min_pp = 1.00, min_p = 1.0, min_var = 0.0, min_acc = 0.0,
  //   min_sis = 0.0, min_es = 0.0,
  //   max_pp = 1.00, max_p = 1.0, max_var = 1.0, max_acc = 0.0,
  //   max_sis = 0.0, max_es = 1.0.
  // FinPour
  coef_min[ACTION_POIDS_PROPRE]      = 1.0;
  coef_max[ACTION_POIDS_PROPRE]      = 1.0;
  coef_min[ACTION_PRECONTRAINTE]     = 1.0;
  coef_max[ACTION_PRECONTRAINTE]     = 1.0;
  coef_min[ACTION_VARIABLE]          = 0.0;
  coef_max[ACTION_VARIABLE]          = 1.0;
  coef_min[ACTION_ACCIDENTELLE]      = 0.0;
  coef_max[ACTION_ACCIDENTELLE]      = 0.0;
  coef_min[ACTION_SISMIQUE]          = 0.0;
  coef_max[ACTION_SISMIQUE]          = 0.0;
  coef_min[ACTION_EAUX_SOUTERRAINES] = 0.0;
  coef_max[ACTION_EAUX_SOUTERRAINES] = 1.0;
  BUG (_1990_ponderations_genere_un (p,
                                     &p->combinaisons.els_perm,
                                     coef_min,
                                     coef_max,
                                     6,
                                     2,
                                     2),
       FALSE)
  
  return TRUE;
}


gboolean
_1990_ponderations_genere (Projet *p)
/**
 * \brief Génération de l'ensemble des pondérations selon la norme spécifiée.
 *        Cf. _1990_ponderations_genere_PAYS.
 * \param p : la variable projet.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL,
 *     - norme inconnue,
 *     - #_1990_ponderations_genere_eu,
 *     - #_1990_ponderations_genere_fr.
 */
{
  BUGMSG (p, FALSE, gettext ("Paramètre %s incorrect.\n"), "projet")
  
  switch (p->parametres.norme)
  {
    case NORME_EU : { return _1990_ponderations_genere_eu (p); break; }
    case NORME_FR : { return _1990_ponderations_genere_fr (p); break; }
    default : { BUGMSG (0, FALSE, gettext ("Norme %d inconnue.\n"), p->parametres.norme) break; } }
}


char *
_1990_ponderations_description (GList *ponderation)
/**
 * \brief Renvoie sous forme de texte une pondération.
 * \param ponderation : une pondération.
 * \return
 *   Succès : la description.\n
 *   Échec : NULL :
 *     - en cas d'erreur d'allocation mémoire.
 */
{
  char *retour = NULL, *tmp;
  
  BUGMSG (retour = malloc (sizeof (char)),
          NULL,
          gettext ("Erreur d'allocation mémoire.\n"))
  retour[0] = 0;
  
  if (ponderation != NULL)
  {
    GList *list_parcours = ponderation;
    do
    {
      char         psi[30];
      Ponderation *ponderation_element = list_parcours->data;
      
      tmp = retour;
      if (ponderation_element->psi == 0)
        conv_f_c (_1990_action_psi_renvoie_0 (ponderation_element->action),
                  psi,
                  DECIMAL_SANS_UNITE);
      else if (ponderation_element->psi == 1)
        conv_f_c (_1990_action_psi_renvoie_1 (ponderation_element->action),
                  psi,
                  DECIMAL_SANS_UNITE);
      else if (ponderation_element->psi == 2)
        conv_f_c (_1990_action_psi_renvoie_2 (ponderation_element->action),
                  psi,
                  DECIMAL_SANS_UNITE);
      else
        psi[0] = '\0';
      if (ponderation_element->psi != -1)
        BUGMSG (retour = g_strdup_printf ("%s%s%.*lf*%s*%s", 
                                          tmp,
                                          tmp[0] != 0 ? "+" : "",
                                          DECIMAL_SANS_UNITE,
                                          ponderation_element->ponderation,
                                          psi,
                                          _1990_action_nom_renvoie(
                                                 ponderation_element->action)),
                                          NULL,
                                          gettext ("Erreur d'allocation mémoire.\n"))
      else
        BUGMSG (retour = g_strdup_printf ("%s%s%.*lf*%s",
                                          tmp,
                                          tmp[0] != 0 ? "+" : "",
                                          DECIMAL_SANS_UNITE,
                                          ponderation_element->ponderation,
                                          _1990_action_nom_renvoie
                                                (ponderation_element->action)),
                                          NULL,
                                          gettext ("Erreur d'allocation mémoire.\n"))
      
      free (tmp);
      
      list_parcours = g_list_next (list_parcours);
    }
    while (list_parcours != NULL);
  }
  
  return retour;
}


void
_1990_ponderations_affiche (GList *ponderations)
/**
 * \brief Affiche les pondérations de la liste fournie en argument.
 * \param ponderations : la liste des pondérations.
 * \return Valeur renvoyée : Aucun.
 */
{
  if (ponderations != NULL)
  {
    GList *list_parcours = ponderations;
    do
    {
      GList *ponderation = list_parcours->data;
      
      if (ponderation != NULL)
      {
        GList *list_parcours2 = ponderation;
        do
        {
          Ponderation *ponderation_element = list_parcours2->data;
          
          if (g_list_next (list_parcours2) != NULL)
            printf ("'%s'*%f(%d)+",
                    _1990_action_nom_renvoie (ponderation_element->action),
                    ponderation_element->ponderation,
                    ponderation_element->psi);
          else
            printf ("'%s'*%f(%d)",
                    _1990_action_nom_renvoie (ponderation_element->action),
                    ponderation_element->ponderation,
                    ponderation_element->psi);
          
          list_parcours2 = g_list_next (list_parcours2);
        }
        while (list_parcours2 != NULL);
        printf ("\n");
      }
      
      list_parcours = g_list_next (list_parcours);
    }
    while (list_parcours != NULL);
  }

  return;
}


gboolean
_1990_ponderations_affiche_tout (Projet *p)
/**
 * \brief Affiche toutes les pondérations du projet.
 * \param p : la variable projet.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL.
 */
{
  BUGMSG (p, FALSE, gettext ("Paramètre %s incorrect.\n"), "projet")
  
  printf ("elu_equ\n");
  _1990_ponderations_affiche (p->combinaisons.elu_equ);
  printf ("elu_str\n");
  _1990_ponderations_affiche (p->combinaisons.elu_str);
  printf ("elu_geo\n");
  _1990_ponderations_affiche (p->combinaisons.elu_geo);
  printf ("elu_fat\n");
  _1990_ponderations_affiche (p->combinaisons.elu_fat);
  printf ("elu_acc\n");
  _1990_ponderations_affiche (p->combinaisons.elu_acc);
  printf ("elu_sis\n");
  _1990_ponderations_affiche (p->combinaisons.elu_sis);
  printf ("els_car\n");
  _1990_ponderations_affiche (p->combinaisons.els_car);
  printf ("els_freq\n");
  _1990_ponderations_affiche (p->combinaisons.els_freq);
  printf ("els_perm\n");
  _1990_ponderations_affiche (p->combinaisons.els_perm);
  
  return TRUE;
}
