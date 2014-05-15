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
#include <math.h>
#include <glib.h>

#include <algorithm>
#include <memory>

#include "common_projet.hpp"
#include "common_erreurs.hpp"
#include "common_math.hpp"
#include "1990_action.hpp"
#include "1990_ponderations.hpp"

/**
 * \brief Vérifie dans la liste des ponderations si la ponderation à vérifier
 *        est déjà présente.
 * \param liste_ponderations : liste des pondérations,
 * \param pond_a_verifier : pondération à vérifier.
 * \return
 *   Succès :
 *     - false si la pondération n'existe pas,
 *     - true si la pondération existe.
 */
bool
_1990_ponderations_verifie_double (
  std::list <std::list <Ponderation *> *> *liste_ponderations,
  std::list <Ponderation *>               *pond_a_verifier)
{
  std::list <std::list <Ponderation *> *>::iterator it;
  
  if (liste_ponderations == NULL)
  {
    return false;
  }
  
  // En renvoyant ici 1, la fonction fait croire que la pondération existe.
  // En vérité, c'est surtout qu'une pondération sans élément n'est pas
  // intéressante à conserver, à la différence des combinaisons vides qui
  // peuvent être utilisées par les niveaux supérieurs.
  if (pond_a_verifier == NULL)
  {
    return true;
  }
  
  it = liste_ponderations->begin ();
  while (it != liste_ponderations->end ())
  {
    // On pense que la pondération est identique jusqu'à preuve du contraire
    bool doublon = true;
    
    std::list <Ponderation *> *ponderation = *it;
    std::list <Ponderation *>::iterator it2 = ponderation->begin ();
    std::list <Ponderation *>::iterator it3 = pond_a_verifier->begin ();
    
    while ((it2 != ponderation->end ()) &&
           (it3 != pond_a_verifier->end ()) &&
           (doublon))
    {
      Ponderation *elem1, *elem2;
      
      elem1 = *it2;
      elem2 = *it3;
      
      if ((elem1->action != elem2->action) || (elem1->psi != elem2->psi) ||
          (!(errrel (elem1->ponderation, elem2->ponderation))))
      {
        doublon = false;
      }
      
      ++it2;
      ++it3;
    }
    
    if ((it2 == ponderation->end ()) &&
        (it3 == pond_a_verifier->end ()) &&
        (doublon))
    {
      return true;
    }
    
    ++it;
  }
  
  return false;
}


/**
 * \brief Ajoute à une liste de pondérations existante une liste de
 *        pondérations. Une vérification est effectuée pour s'assurer que la
 *        liste source ne possède pas une ou plusieurs pondérations identiques
 *        que la liste de destination.
 * \param liste_dest : liste de ponderations qui recevra les ponderations
 *                     sources,
 * \param liste_source : liste de ponderations source.
 * \return
 *   Succès : true.\n
 *   Échec : false :
 *     - liste_dest == NULL,
 *     - en cas d'erreur d'allocation mémoire.
 */
bool
_1990_ponderations_duplique_sans_double (
  std::list <std::list <Ponderation *> *> *liste_dest,
  std::list <std::list <Ponderation *> *> *liste_source)
{
  std::list <std::list <Ponderation *> *>::iterator it;
  
  BUGPARAM (liste_dest, "%p", liste_dest, false)
  BUGPARAM (liste_source, "%p", liste_dest, false)
  
  if (liste_source->empty ())
  {
    return 0;
  }
  
  it = liste_source->begin ();
  while (it != liste_source->end ())
  {
    std::list <Ponderation *> *ponderation_source;
    
    ponderation_source = *it;
    /* Si la ponderation n'existe pas, on l'ajoute à la fin */
    if (!_1990_ponderations_verifie_double (liste_dest, ponderation_source))
    {
      std::list <Ponderation *>::iterator it2 = ponderation_source->begin ();
      std::list <Ponderation *>          *ponderation_destination;
      
      ponderation_destination = new std::list <Ponderation *> ();
      
      while (it2 != ponderation_source->end ())
      {
        Ponderation *element_source;
        Ponderation *element_destination = new Ponderation;
        
        element_source = *it2;
        element_destination->action = element_source->action;
        element_destination->flags = element_source->flags;
        element_destination->psi = element_source->psi;
        element_destination->ponderation = element_source->ponderation;
        ponderation_destination->push_back (element_destination);
        
        ++it2;
      }
      
      liste_dest->push_back (ponderation_destination);
    }
    
    it++;
  }
  
  return true;
}


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
 *        actions variables d'accompagnement : 0 = psi0, 1 = psi1, 2 = psi2 et
 *        -1 = prendre la valeur 1.0,
 * \return
 *   Succès : true.\n
 *   Échec : false :
 *     - p == NULL,
 *     - p->niveaux_groupes == NULL,
 *     - en cas d'erreur d'allocation mémoire.
 */
bool
_1990_ponderations_genere_un (
  Projet *p,
  std::list <std::list <Ponderation *> *> *ponderations_destination,
  double *coef_min,
  double *coef_max,
  uint8_t dim_coef,
  int8_t  psi_dominante,
  int8_t  psi_accompagnement)
{
  uint32_t       nbboucle, j;
  Groupe        *groupe;
  Niveau_Groupe *niveau;
  
  std::list <Niveau_Groupe *>::iterator it_tmp;
  
  BUGPARAMCRIT (p, "%p", p, false)
  INFO (!p->niveaux_groupes.empty (),
        false,
        (gettext ("Le projet ne possède pas de niveaux de groupes.\n")); )
  
  // Si le dernier niveau ne possède pas un seul et unique groupe Alors
  //   Fin.
  // FinSi
  it_tmp = p->niveaux_groupes.end ();
  --it_tmp;
  niveau = *it_tmp;
  INFO (niveau->groupes.size () == 1,
        false,
        (gettext ("La génération des pondérations est impossible.\nLe dernier niveau ne peut possèder qu'un seul groupe.\n")); )
  groupe = *niveau->groupes.begin ();
  
  // Si le groupe du dernier niveau ne possède pas de combinaison Alors
  //   Fin.
  // FinSi
  INFO (!groupe->tmp_combinaison.empty (),
        false,
        (gettext ("Le dernier niveau ne possède aucune combinaison permettant la génération des pondérations.\n")); )
  
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
    std::list <std::list <Combinaison *> *>::iterator it;
    
    it = groupe->tmp_combinaison.begin ();
    // Pour chaque combinaison dans le groupe final Faire
    while (it != groupe->tmp_combinaison.end ())
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
      bool suivant = false;
      bool variable_accompagnement = false, variable_dominante = false;
      
      std::list <Combinaison *>          *combinaison;
      std::list <Combinaison *>::iterator it2;
      std::list <Ponderation *>          *ponderation;
      
      combinaison = *it;
      ponderation = new std::list <Ponderation *> ();
      
      it2 = combinaison->begin ();
      // Pour chaque élément de la combinaison Faire
      while ((it2 != combinaison->end ()) && (!suivant))
      {
        Combinaison     *combinaison_element;
        Action_Categorie categorie;
        
        combinaison_element = *it2;
        categorie = _1990_action_categorie_bat (_1990_action_type_renvoie (
                                                combinaison_element->action),
                                                p->parametres.norme);
        BUG (categorie != ACTION_INCONNUE, false)
        // Vérification si le coefficient min et max de la catégorie vaut 0.
        //  Si oui, pondération ignorée.
        if ((errmax (coef_min[categorie], ERRMAX_POND)) &&
            (errmax (coef_max[categorie], ERRMAX_POND)))
        {
          suivant = true;
        }
        else
        {
          double pond;
        // On affecte le coefficient min/max à la combinaison pour obtenir la
        // pondération
          if ((j & (1 << categorie)) != 0)
          {
            pond = coef_max[categorie];
          }
          else
          {
            pond = coef_min[categorie];
          }
          
          if (!(errmax (pond, ERRMAX_POND)))
          {
            Ponderation *ponderation_element = new Ponderation;
            
            ponderation_element->action = combinaison_element->action;
            ponderation_element->flags = combinaison_element->flags;
            
            // Vérifier la présente d'une action variable prédominante et
            // d'une action variable d'accompagnement. Si oui, pondération
            // ignorée.
            if (categorie == ACTION_VARIABLE)
            {
              variable_accompagnement = true;
              if ((ponderation_element->flags & 1) != 0)
              {
                variable_dominante = true;
                ponderation_element->psi = psi_dominante;
              }
              else
              {
                ponderation_element->psi = psi_accompagnement;
              }
            }
            // psi vaut toujours -1 s'il ne s'agit pas d'une action variable.
            else
            {
              ponderation_element->psi = -1;
            }
            
            ponderation_element->ponderation = pond;
        
            ponderation->push_back (ponderation_element);
          }
        }
        ++it2;
      }
      // FinPour
  // Si la pondération n'est pas ignorée Alors
  //   Ajout à la liste des pondérations existante.
  // FinSi
      if ((variable_accompagnement) && (!variable_dominante))
      {
        suivant = true;
      }
      if ((!suivant) &&
          (!_1990_ponderations_verifie_double (ponderations_destination,
                                              ponderation)))
      {
        ponderations_destination->push_back (ponderation);
      }
      else
      {
        _1990_ponderations_free_1 (ponderation);
      }
      
      ++it;
    }
    // FinPour
  }
  // FinPour
  
  return true;
}


/**
 * \brief Génération de l'ensemble des pondérations selon la norme européenne.
 *        La fonction #_1990_ponderations_genere_un est appelée autant de fois
 *        que nécessaire avec les coefficients min/max ajustées en fonction des
 *        valeur de la norme européenne et de la nature de l'état limite
 *        recherché. Les options de calculs sont définies dans la variable
 *        p->ponderations.flags et doivent être définies.
 * \param p : la variable projet.
 * \return
 *   Succès : true.\n
 *   Échec : false :
 *     - p == NULL,
 *     - #_1990_ponderations_genere_un,
 *     - #_1990_ponderations_duplique_sans_double.
 */
bool
_1990_ponderations_genere_eu (Projet *p)
{
  double coef_min[ACTION_INCONNUE], coef_max[ACTION_INCONNUE];
  
  BUGPARAMCRIT (p, "%p", p, false)
  
 // Les indices ont les définitions suivantes : pp = poids propre,
 // p = précontrainte, var = variable, acc = accidentelle et
 // sis = sismique.
  
  // Pour ELU_EQU, générer les pondérations suivantes :
  // Si à l'équilibre seulement Alors
  //   coefficient charges variables prédominante : 1,
  //   coefficient charges variables d'accompagnement : psi0,
  //   min_pp = 0.9, min_p = 1.0, min_var = 0.0, min_acc = 0.0, min_sis = 0.0,
  //   max_pp = 1.1, max_p = 1.3, max_var = 1.5, max_acc = 0.0, max_sis = 0.0.
  if (p->ponderations.elu_equ_methode == 0)
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
                                       &p->ponderations.elu_equ,
                                       coef_min,
                                       coef_max,
                                       5,
                                       -1,
                                       0),
         false)
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
                                       &p->ponderations.elu_equ,
                                       coef_min,
                                       coef_max,
                                       5,
                                       -1,
                                       0),
         false)
    
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
                                       &p->ponderations.elu_equ,
                                       coef_min,
                                       coef_max,
                                       5,
                                       -1,
                                       0),
         false)
  }
  // Si utilisation des formules 6.10a et 6.10b de l'Eurocode 0 Alors
  if (p->ponderations.form_6_10 == 0)
  {
    switch (p->ponderations.elu_geo_str_methode)
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
                                           &p->ponderations.elu_str,
                                           coef_min,
                                           coef_max,
                                           5,
                                           0,
                                           0),
             false)
        
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
                                           &p->ponderations.elu_str,
                                           coef_min,
                                           coef_max,
                                           5,
                                           -1,
                                           0),
             false)
        
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
                                           &p->ponderations.elu_str,
                                           coef_min,
                                           coef_max,
                                           5,
                                           -1,
                                           0),
             false)
        
        BUG (_1990_ponderations_duplique_sans_double (
               &p->ponderations.elu_geo,
               &p->ponderations.elu_str),
             false)
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
      case 1:
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
                                           &p->ponderations.elu_str,
                                           coef_min,
                                           coef_max,
                                           5,
                                           0,
                                           0),
             false)
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
                                           &p->ponderations.elu_str,
                                           coef_min,
                                           coef_max,
                                           5,
                                           -1,
                                           0),
             false)
        BUG (_1990_ponderations_duplique_sans_double (
               &p->ponderations.elu_geo,
               &p->ponderations.elu_str),
             false)
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
                                           &p->ponderations.elu_str,
                                           coef_min,
                                           coef_max,
                                           5,
                                           0,
                                           0),
             false)
        
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
                                           &p->ponderations.elu_str,
                                           coef_min,
                                           coef_max,
                                           5,
                                           -1,
                                           0),
             false)
        
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
                                           &p->ponderations.elu_geo,
                                           coef_min,
                                           coef_max,
                                           5,
                                           -1,
                                           0),
             false)
        break;
  // FinSi
      }
      default:
      {
        FAILCRIT (false,
                  (gettext ("Flag %d inconnu.\n"),
                            p->ponderations.elu_geo_str_methode); )
        break;
      }
    }
  }
  // Si utilisation de la formule 6.10 de l'Eurocode 0 Alors
  else
  {
    switch (p->ponderations.elu_geo_str_methode)
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
                                           &p->ponderations.elu_str,
                                           coef_min,
                                           coef_max,
                                           5,
                                           -1,
                                           0),
             false)
        
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
                                           &p->ponderations.elu_str,
                                           coef_min,
                                           coef_max,
                                           5,
                                           -1,
                                           0),
             false)
        
        BUG (_1990_ponderations_duplique_sans_double (
               &p->ponderations.elu_geo,
               &p->ponderations.elu_str),
             false)
        break;
      }
  // Sinon Si selon l'approche 2 Alors
  // Pour ELU_STR ET ELU_GEO, générer les pondérations suivantes :
  //   coefficient charges variables prédominante : 1,
  //   coefficient charges variables d'accompagnement : psi0,
  //   min_pp = 1.00, min_p = 1.0, min_var = 0.0, min_acc = 0.0, min_sis = 0.0,
  //   max_pp = 1.35, max_p = 1.3, max_var = 1.5, max_acc = 0.0, max_sis = 0.0.
      case 1:
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
                                           &p->ponderations.elu_str,
                                           coef_min,
                                           coef_max,
                                           5,
                                           -1,
                                           0),
             false)
        BUG (_1990_ponderations_duplique_sans_double (
               &p->ponderations.elu_geo,
               &p->ponderations.elu_str),
             false)
        break;
      }
  // Si selon l'approche 3 Alors
  // Pour ELU_STR, générer les pondérations suivantes :
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
                                           &p->ponderations.elu_str,
                                           coef_min,
                                           coef_max,
                                           5,
                                           -1,
                                           0),
             false)
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
                                           &p->ponderations.elu_geo,
                                           coef_min,
                                           coef_max,
                                           5,
                                           -1,
                                           0),
             false)
        break;
      }
  // FinSi
      default :
      {
        FAILCRIT (false,
                  (gettext ("Flag %d inconnu.\n"),
                            p->ponderations.elu_geo_str_methode); )
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
  if (p->ponderations.elu_acc_psi == 0)
  {
    BUG (_1990_ponderations_genere_un (p,
                                       &p->ponderations.elu_acc,
                                       coef_min,
                                       coef_max,
                                       5,
                                       1,
                                       2),
         false)
  }
  else
  {
    BUG (_1990_ponderations_genere_un (p,
                                       &p->ponderations.elu_acc,
                                       coef_min,
                                       coef_max,
                                       5,
                                       2,
                                       2),
         false)
  }
  
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
                                     &p->ponderations.elu_sis,
                                     coef_min,
                                     coef_max,
                                     5,
                                     2,
                                     2),
       false)
  
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
                                     &p->ponderations.els_car,
                                     coef_min,
                                     coef_max,
                                     5,
                                     -1,
                                     0),
       false)
  
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
                                     &p->ponderations.els_freq,
                                     coef_min,
                                     coef_max,
                                     5,
                                     1,
                                     2),
       false)
  
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
                                     &p->ponderations.els_perm,
                                     coef_min,
                                     coef_max,
                                     5,
                                     2,
                                     2),
       false)
  
  return true;
}


/**
 * \brief Génération de l'ensemble des pondérations selon la norme française.
 *        La fonction #_1990_ponderations_genere_un est appelé autant de fois
 *        que nécessaire avec les coefficients min/max ajustées en fonction des
 *        valeur de la norme française et de la nature de l'état limite
 *        recherché. Les options de calculs sont définies dans la variable
 *        p->ponderations.flags et doivent être définies.
 * \brief p : la variable projet.
 * \return
 *   Succès : false.\n
 *   Échec : true :
 *     - p == NULL,
 *     - #_1990_ponderations_genere_un,
 *     - #_1990_ponderations_duplique_sans_double.
 */
bool
_1990_ponderations_genere_fr (Projet *p)
{
  double  coef_min[ACTION_INCONNUE], coef_max[ACTION_INCONNUE];
  
  // Les indices ont les définitions suivantes : pp = poids propre,
  // p = précontrainte, var = variable, acc = accidentelle,
  // sis = sismique et es = eaux souterraines.
  
  BUGPARAMCRIT (p, "%p", p, false)
  
  // Pour ELU_EQU, générer les pondérations suivantes :
  //   Si à l'équilibre seulement Alors
  //     coefficient charges variables prédominante : 1.
  //     coefficient charges variables d'accompagnement : psi0.
  //     min_pp = 0.9, min_p = 1.0, min_var = 0.0, min_acc = 0.0,
  //     min_sis = 0.0, min_es = 0.0,
  //     max_pp = 1.1, max_p = 1.3, max_var = 1.5, max_acc = 0.0,
  //     max_sis = 0.0, max_es = 1.2.
  if (p->ponderations.elu_equ_methode == 0)
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
                                       &p->ponderations.elu_equ,
                                       coef_min,
                                       coef_max,
                                       6,
                                       -1,
                                       0),
         false)
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
                                       &p->ponderations.elu_equ,
                                       coef_min,
                                       coef_max,
                                       6,
                                       -1,
                                       0),
         false)
    
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
                                       &p->ponderations.elu_equ,
                                       coef_min,
                                       coef_max,
                                       6,
                                       -1,
                                       0),
         false)
  }
  
  // Si utilisation des formules 6.10a et 6.10b de l'Eurocode 0 Alors
  if (p->ponderations.form_6_10 == 0)
  {
    switch (p->ponderations.elu_geo_str_methode)
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
                                           &p->ponderations.elu_str,
                                           coef_min,
                                           coef_max,
                                           6,
                                           0,
                                           0),
             false)
        
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
                                           &p->ponderations.elu_str,
                                           coef_min,
                                           coef_max,
                                           6,
                                           -1,
                                           0),
             false)
        
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
                                           &p->ponderations.elu_str,
                                           coef_min,
                                           coef_max,
                                           6,
                                           -1,
                                           0),
             false)
        
        BUG (_1990_ponderations_duplique_sans_double (
               &p->ponderations.elu_geo,
               &p->ponderations.elu_str),
             false)
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
      case 1:
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
                                           &p->ponderations.elu_str,
                                           coef_min,
                                           coef_max,
                                           6,
                                           0,
                                           0),
             false)
        
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
                                           &p->ponderations.elu_str,
                                           coef_min,
                                           coef_max,
                                           6,
                                           -1,
                                           0),
             false)
        
        BUG (_1990_ponderations_duplique_sans_double (
               &p->ponderations.elu_geo,
               &p->ponderations.elu_str),
             false)
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
                                           &p->ponderations.elu_str,
                                           coef_min,
                                           coef_max,
                                           6,
                                           0,
                                           0),
             false)
        
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
                                           &p->ponderations.elu_str,
                                           coef_min,
                                           coef_max,
                                           6,
                                           -1,
                                           0),
             false)
        
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
                                           &p->ponderations.elu_geo,
                                           coef_min,
                                           coef_max,
                                           6,
                                           -1,
                                           0),
             false)
        break;
  //   FinSi
      }
      default :
      {
        FAILCRIT (false,
                  (gettext ("Flag %d inconnu.\n"),
                            p->ponderations.elu_geo_str_methode); )
        break;
      }
    }
  }
  // Si utilisation de la formule 6.10 de l'Eurocode 0 Alors
  else
  // équation 6.10
  {
    switch (p->ponderations.elu_geo_str_methode)
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
                                           &p->ponderations.elu_str,
                                           coef_min,
                                           coef_max,
                                           6,
                                           -1,
                                           0),
             false)
        
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
                                           &p->ponderations.elu_str,
                                           coef_min,
                                           coef_max,
                                           6,
                                           -1,
                                           0),
             false)
        
        BUG (_1990_ponderations_duplique_sans_double (
               &p->ponderations.elu_geo,
               &p->ponderations.elu_str),
             false)
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
      case 1:
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
                                           &p->ponderations.elu_str,
                                           coef_min,
                                           coef_max,
                                           6,
                                           -1,
                                           0),
             false)
        
        BUG (_1990_ponderations_duplique_sans_double (
               &p->ponderations.elu_geo,
               &p->ponderations.elu_str),
             false)
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
                                           &p->ponderations.elu_str,
                                           coef_min,
                                           coef_max,
                                           6,
                                           -1,
                                           0),
             false)
        
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
                                           &p->ponderations.elu_geo,
                                           coef_min,
                                           coef_max,
                                           6,
                                           -1,
                                           0),
             false)
        break;
      }
  //   FinSi
      default :
      {
        FAILCRIT (false,
                  (gettext ("Flag %d inconnu.\n"),
                            p->ponderations.elu_geo_str_methode); )
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
  if (p->ponderations.elu_acc_psi == 0)
  {
    BUG (_1990_ponderations_genere_un (p,
                                       &p->ponderations.elu_acc,
                                       coef_min,
                                       coef_max,
                                       6,
                                       1,
                                       2),
         false)
  }
  else
  {
    BUG (_1990_ponderations_genere_un (p,
                                       &p->ponderations.elu_acc,
                                       coef_min,
                                       coef_max,
                                       6,
                                       2,
                                       2),
         false)
  }
  
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
                                     &p->ponderations.elu_acc,
                                     coef_min,
                                     coef_max,
                                     6,
                                     2,
                                     2),
       false)
  
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
                                     &p->ponderations.els_car,
                                     coef_min,
                                     coef_max,
                                     6,
                                     -1,
                                     0),
       false)

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
                                     &p->ponderations.els_freq,
                                     coef_min,
                                     coef_max,
                                     6,
                                     1,
                                     2),
       false)

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
                                     &p->ponderations.els_perm,
                                     coef_min,
                                     coef_max,
                                     6,
                                     2,
                                     2),
       false)
  
  return true;
}


/**
 * \brief Génération de l'ensemble des pondérations selon la norme spécifiée.
 *        Cf. _1990_ponderations_genere_PAYS.
 * \param p : la variable projet.
 * \return
 *   Succès : true.\n
 *   Échec : false :
 *     - p == NULL,
 *     - norme inconnue,
 *     - #_1990_ponderations_genere_eu,
 *     - #_1990_ponderations_genere_fr.
 */
bool
_1990_ponderations_genere (Projet *p)
{
  BUGPARAMCRIT (p, "%p", p, false)
  
  switch (p->parametres.norme)
  {
    case NORME_EU : return _1990_ponderations_genere_eu (p);
    case NORME_FR : return _1990_ponderations_genere_fr (p);
    default : {
                FAILCRIT (false,
                          (gettext ("Norme %d inconnue.\n"),
                                    p->parametres.norme); )
                break;
              }
  }
}


/**
 * \brief Renvoie sous forme de texte une pondération.
 * \param ponderation : une pondération.
 * \return
 *   Succès : la description.\n
 *   Échec : NULL :
 *     - en cas d'erreur d'allocation mémoire.
 */
// coverity[+alloc]
char *
_1990_ponderations_description (std::list <Ponderation*> *ponderation)
{
  char *retour = NULL;
  
  BUGCRIT (retour = (char *) malloc (sizeof (char)),
           NULL,
           (gettext ("Erreur d'allocation mémoire.\n")); )
  retour[0] = 0;
  
  if (ponderation != NULL)
  {
    std::list <Ponderation*>::iterator it = ponderation->begin ();
    
    while (it != ponderation->end ())
    {
      char         psi[30];
      Ponderation *ponderation_element = *it;
      char        *tmp = retour;
      
      if (ponderation_element->psi == 0)
      {
        conv_f_c (_1990_action_psi_renvoie_0 (ponderation_element->action),
                  psi,
                  DECIMAL_SANS_UNITE);
      }
      else if (ponderation_element->psi == 1)
      {
        conv_f_c (_1990_action_psi_renvoie_1 (ponderation_element->action),
                  psi,
                  DECIMAL_SANS_UNITE);
      }
      else if (ponderation_element->psi == 2)
      {
        conv_f_c (_1990_action_psi_renvoie_2 (ponderation_element->action),
                  psi,
                  DECIMAL_SANS_UNITE);
      }
      else
      {
        psi[0] = '\0';
      }
      if (ponderation_element->psi != -1)
      {
        BUGCRIT (retour = g_strdup_printf ("%s%s%.*lf*%s*%s", 
                                          tmp,
                                          tmp[0] != 0 ? "+" : "",
                                          DECIMAL_SANS_UNITE,
                                          ponderation_element->ponderation,
                                          psi,
                                          _1990_action_nom_renvoie(
                                                 ponderation_element->action)),
                 NULL,
                 (gettext ("Erreur d'allocation mémoire.\n"));
                   free (tmp); )
      }
      else
      {
        BUGCRIT (retour = g_strdup_printf ("%s%s%.*lf*%s",
                                          tmp,
                                          tmp[0] != 0 ? "+" : "",
                                          DECIMAL_SANS_UNITE,
                                          ponderation_element->ponderation,
                                          _1990_action_nom_renvoie
                                                (ponderation_element->action)),
                 NULL,
                 (gettext ("Erreur d'allocation mémoire.\n"));
                   free (tmp); )
      }
      
      free (tmp);
      
      ++it;
    }
  }
  
  return retour;
}


/**
 * \brief Affiche les pondérations de la liste fournie en argument.
 * \param ponderations : la liste des pondérations.
 * \return Valeur renvoyée : Aucun.
 */
void
_1990_ponderations_affiche (
  std::list <std::list <Ponderation *> *> *ponderations)
{
  std::list <std::list <Ponderation *> *>::iterator it = ponderations->begin ();
  
  it = ponderations->begin ();
  
  while (it != ponderations->end ())
  {
    std::list <Ponderation *> *ponderation = *it;
    std::list <Ponderation *>::iterator it2 = ponderation->begin ();
    
    while (it2 != ponderation->end ())
    {
      Ponderation *ponderation_element = *it2;
      
      if (std::next (it2) != ponderation->end ())
      {
        printf ("'%s'*%f(%d)+",
                _1990_action_nom_renvoie (ponderation_element->action),
                ponderation_element->ponderation,
                ponderation_element->psi);
      }
      else
      {
        printf ("'%s'*%f(%d)",
                _1990_action_nom_renvoie (ponderation_element->action),
                ponderation_element->ponderation,
                ponderation_element->psi);
      }
      
      ++it2;
    }
    printf ("\n");
    
    ++it;
  }

  return;
}


/**
 * \brief Affiche toutes les pondérations du projet.
 * \param p : la variable projet.
 * \return
 *   Succès : true.\n
 *   Échec : false :
 *     - p == NULL.
 */
bool
_1990_ponderations_affiche_tout (Projet *p)
{
  BUGPARAMCRIT (p, "%p", p, false)
  
  printf ("elu_equ\n");
  _1990_ponderations_affiche (&p->ponderations.elu_equ);
  printf ("elu_str\n");
  _1990_ponderations_affiche (&p->ponderations.elu_str);
  printf ("elu_geo\n");
  _1990_ponderations_affiche (&p->ponderations.elu_geo);
  printf ("elu_fat\n");
  _1990_ponderations_affiche (&p->ponderations.elu_fat);
  printf ("elu_acc\n");
  _1990_ponderations_affiche (&p->ponderations.elu_acc);
  printf ("elu_sis\n");
  _1990_ponderations_affiche (&p->ponderations.elu_sis);
  printf ("els_car\n");
  _1990_ponderations_affiche (&p->ponderations.els_car);
  printf ("els_freq\n");
  _1990_ponderations_affiche (&p->ponderations.els_freq);
  printf ("els_perm\n");
  _1990_ponderations_affiche (&p->ponderations.els_perm);
  
  return true;
}


/**
 * \brief Fonction à utiliser avec for_each pour libérer une pondération.
 * \param pond : une pondération à libérer.
 * \return Rien.
 */
void
_1990_ponderations_free_1 (std::list <Ponderation *> *pond)
{
  for_each (pond->begin (),
            pond->end (),
            std::default_delete <Ponderation> ());
  delete pond;
  
  return;
}


/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
