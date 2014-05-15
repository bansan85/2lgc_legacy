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

#include <memory>
#include <algorithm>
#include <iterator>

#include "common_projet.hpp"
#include "common_erreurs.hpp"
#include "1990_action.hpp"
#include "1990_groupe.hpp"
#include "1990_ponderations.hpp"
#include "1990_combinaisons.hpp"

/**
 * \brief Vérifie si une combinaison est déjà présente dans une liste de
 *        combinaisons.
 * \param liste_combinaisons : liste de combinaisons,
 * \param comb_a_verifier : combinaison à vérifier.
 * \return
 *    - false si la combinaison n'est pas présente,
 *    - true si la combinaison est présente,
 *    - true si la combinaison est vide (afin de ne pas l'ajouter).
 */
bool
_1990_combinaisons_verifie_double (
  std::list <std::list <Combinaison *> *> *liste_combinaisons,
  std::list <Combinaison *>               *comb_a_verifier)
{
  std::list <std::list <Combinaison *> *>::iterator it;
  
  if (liste_combinaisons == NULL)
  {
    return false;
  }
  if (comb_a_verifier == NULL)
  {
    return true;
  }
  
  it = liste_combinaisons->begin ();
  while (it != liste_combinaisons->end ())
  {
    bool doublon = true;
    
    std::list <Combinaison *>          *comb_en_cours = *it;
    std::list <Combinaison *>::iterator it1, it2;
    
    it1 = comb_en_cours->begin ();
    it2 = comb_a_verifier->begin ();
    
    while ((it1 != comb_en_cours->end ()) &&
           (it2 != comb_a_verifier->end ()) &&
           (doublon))
    {
      Combinaison *elem1;
      Combinaison *elem2;
      
      elem1 = *it1;
      elem2 = *it2;
      
      /* On vérifie que chaque élément pointe vers la même action
       *  et que les flags (paramètres de calculs) sont les mêmes */
      if ((elem1->action != elem2->action) || (elem1->flags != elem2->flags))
      {
        doublon = false;
      }
      
      ++it1;
      ++it2;
    }
    
    if ((doublon) &&
        (it1 == comb_en_cours->end ()) &&
        (it2 == comb_a_verifier->end ()))
    {
      return true;
    }
    
    ++it;
  }
  
  return false;
}


/**
 * \brief Ajoute à une liste de combinaisons existante une liste de
 *        combinaisons.
 * \param liste_comb_destination : liste de combinaisons qui recevra les
 *                                 combinaisons sources,
 * \param liste_comb_source : liste de combinaisons source,
 * \param sans_double : true pour qu'aucune combinaison ne soit ajoutée en
 *                      double.
 * \return
 *   Succès : true.\n
 *   Échec : false :
 *     - liste_comb_destination == NULL,
 *     - erreur d'allocation mémoire.
 */
bool
_1990_combinaisons_duplique (
  std::list <std::list <Combinaison *> *> *liste_comb_destination,
  std::list <std::list <Combinaison *> *> *liste_comb_source,
  bool                                     sans_double)
{
  std::list <std::list <Combinaison *> *>::iterator it;
  
  BUGPARAMCRIT (liste_comb_destination, "%p", liste_comb_destination, false)
  
  if (liste_comb_source == NULL)
  {
    return true;
  }
  
  it = liste_comb_source->begin ();
  while (it != liste_comb_source->end ())
  {
    std::list <Combinaison *> *combinaison_source = *it;
    
    bool verifie_double;
    
    if (sans_double)
    {
      verifie_double = _1990_combinaisons_verifie_double (
                         liste_comb_destination,
                         combinaison_source);
    }
    else
    {
      verifie_double = false;
    }
    
    if (!verifie_double)
    {
      std::list <Combinaison *>          *combinaison_destination;
      std::list <Combinaison *>::iterator it2 = combinaison_source->begin ();
      
      // Duplication de la combinaison
      combinaison_destination = new std::list <Combinaison *> ();
      
      while (it2 != combinaison_source->end ())
      {
        Combinaison *element_source = *it2;
        Combinaison *element_destination;
        
        element_destination = new Combinaison;
        
        element_destination->action = element_source->action;
        element_destination->flags = element_source->flags;
        
        combinaison_destination->push_back (element_destination);
        
        ++it2;
      }
      
      // Insertion de la combinaison dans liste_comb_destination
      liste_comb_destination->push_back (combinaison_destination);
      
    }
    
    ++it;
  }
  
  return true;
}


/**
 * \brief Génère toutes les combinaisons d'un groupe possédant comme type de
 *        combinaison GROUPE_COMBINAISON_XOR dans le champ tmp_combinaison.
 * \param p : la variable projet,
 * \param niveau : niveau du groupe à analyser,
 * \param groupe : groupe à analyser.
 * \return
 *   Succès : true.\n
 *   Échec : false :
 *     - p == NULL,
 *     - p->niveaux_groupes == NULL (pas de niveau),
 *     - niveau == NULL,
 *     - niveau->groupes == NULL,
 *     - groupe->type_combinaison != GROUPE_COMBINAISON_XOR,
 *     - erreur d'allocation mémoire,
 *     - _1990_combinaisons_duplique.
 */
bool
_1990_combinaisons_genere_xor (Projet        *p,
                               Niveau_Groupe *niveau,
                               Groupe        *groupe)
{
  std::list <void *>::iterator it2;
  
  BUGPARAMCRIT (p, "%p", p, false)
  INFO (!p->niveaux_groupes.empty (),
        false,
        (gettext ("Le projet ne possède pas de niveaux de groupes.\n")); )
  BUGPARAMCRIT (niveau, "%p", niveau, false)
  INFO (!niveau->groupes.empty (),
        false,
        (gettext ("Le niveau %zu est vide. Veuillez soit le remplir, soit le supprimer.\n"),
                  std::distance (p->niveaux_groupes.begin (),
                                 std::find (p->niveaux_groupes.begin (),
                                            p->niveaux_groupes.end (),
                                            niveau))); )
  BUGPARAMCRIT (groupe->type_combinaison,
                "%d",
                groupe->type_combinaison == GROUPE_COMBINAISON_XOR,
                false,
                (gettext ("Seuls les groupes possédant un type de combinaison XOR peuvent appeler _1990_combinaisons_genere_xor.\n")); )
  
  // Si le nombre d'éléments est 0 Alors
  //   Fin.
  // FinSi
  if (groupe->elements.empty ())
  {
    return true;
  }
  
  it2 = groupe->elements.begin ();
  
  // Si l'élément courant de niveaux_groupes est le premier de la liste Alors
  //   L'opération XOR consiste à ajouter les actions listées dans le groupe
  //   courant du premier élément de niveaux_groupes à la suite des autres sans
  //   aucune combinaison entre elles. On crée donc autant de combinaisons
  //   contenant une seule action qu'il y a d'éléments.
  if (niveau == *p->niveaux_groupes.begin ())
  {
    do
    {
      Action *action = static_cast <Action *> (*it2);
      
      // On vérifie si l'action possède une charge. Si non, on ignore l'action.
      if (!_1990_action_charges_vide (action))
      {
        std::list <Combinaison *> *nouvelle_combinaison;
        Combinaison *element = new Combinaison;
        
        nouvelle_combinaison = new std::list <Combinaison *> ();
        element->action = action;
        element->flags = _1990_action_flags_action_predominante_renvoie (
                                                         element->action) & 1U;
        nouvelle_combinaison->push_back (element);
        groupe->tmp_combinaison.push_back (nouvelle_combinaison);
      }
      
      ++it2;
    }
    while (it2 != groupe->elements.end ());
  }
  // Sinon
  //   Un XOR pour les autres étages consiste à ajouter toutes les combinaisons
  //   des groupes de l'étage n-1 qui sont indiquées dans le groupe de l'étage
  //   n.
  else
  {
    std::list <Niveau_Groupe *>::iterator it;
    
    it = std::find (p->niveaux_groupes.begin (),
                    p->niveaux_groupes.end (),
                    niveau);
    BUGCRIT (it != p->niveaux_groupes.end (),
             false,
             (gettext ("Impossible de trouver le niveau dans la liste des niveaux de groupes.\n")); )
    --it;
    
    groupe->tmp_combinaison.clear ();
    
    do
    {
      Groupe *groupe_n_1 = static_cast <Groupe *> (*it2);
      
      BUG (_1990_combinaisons_duplique (&groupe->tmp_combinaison,
                                        &groupe_n_1->tmp_combinaison,
                                        true),
           false)
      
      ++it2;
    }
    while (it2 != groupe->elements.end ());
  }
  // FinSi
  
  return true;
}


/**
 * \brief Fusionne deux combinaisons. La liste des combinaisons source est
 *        copiée à la fin de la liste des combinaisons destination. Il est donc
 *        toujours nécessaire de libérer la liste source.
 * \param destination : combinaison de destination,
 * \param source : combinaison source.
 * \return
 *   Succès : true.\n
 *   Échec : false :
 *     - destination == NULL,
 *     - erreur d'allocation mémoire.
 */
bool
_1990_combinaisons_fusion (std::list <Combinaison *> **destination,
                           std::list <Combinaison *>  *source)
{
  std::list <Combinaison *>::iterator it;
  
  BUGPARAMCRIT (destination, "%p", destination, false)
  BUGPARAMCRIT (source, "%p", source, false)
  
  it = source->begin ();
  while (it != source->end ())
  {
    Combinaison *element_source = *it;
    Combinaison *element_destination;
    
    element_destination = new Combinaison;
    element_destination->action = element_source->action;
    element_destination->flags = element_source->flags;
    (*destination)->push_back (element_destination);
    
    ++it;
  }
  
  return true;
}


/**
 * \brief Génère toutes les combinaisons d'un groupe possédant comme type de
 *        combinaison GROUPE_COMBINAISON_AND.
 * \param p : la variable projet,
 * \param niveau : niveau du groupe à analyser,
 * \param groupe : groupe à analyser.
 * \return
 *   Succès : true.\n
 *   Échec : false :
 *     - p == NULL,
 *     - p->niveaux_groupes == NULL,
 *     - niveau == NULL,
 *     - niveau->groupes == NULL,
 *     - groupe == NULL,
 *     - groupe->type_combinaison != GROUPE_COMBINAISON_AND,
 *     - erreur d'allocation mémoire,
 *     - #_1990_combinaisons_duplique,
 *     - #_1990_combinaisons_fusion.
 */
bool
_1990_combinaisons_genere_and (Projet        *p,
                               Niveau_Groupe *niveau,
                               Groupe        *groupe)
{
  std::list <void *>::iterator it4;
  Groupe *groupe_n_1;
  
  BUGPARAMCRIT (p, "%p", p, false)
  INFO (!p->niveaux_groupes.empty (),
        false,
        (gettext ("Le projet ne possède pas de niveaux de groupes.\n")); )
  BUGPARAMCRIT (niveau, "%p", niveau, false)
  INFO (!niveau->groupes.empty (),
        false,
        (gettext ("Le niveau %zu est vide. Veuillez soit le remplir, soit le supprimer.\n"),
                  std::distance (p->niveaux_groupes.begin (),
                                 std::find (p->niveaux_groupes.begin (),
                                            p->niveaux_groupes.end (),
                                            niveau))); )
  BUGPARAMCRIT (groupe, "%p", groupe, false)
  INFO (groupe->type_combinaison == GROUPE_COMBINAISON_AND,
        false,
        (gettext ("Seuls les groupes possédant un type de combinaison AND peuvent appeler _1990_combinaisons_genere_and.\n")); )
  
  if (groupe->elements.empty ())
  {
    return true;
  }
  
  it4 = groupe->elements.begin ();
  
  // Si l'élément courant de niveaux_groupes est le premier de la liste Alors
  //   L'opération AND consiste à créer une combinaison contenant l'ensemble
  //   des actions listées dans le groupe courant du premier élément de
  //   niveaux_groupes. Si une seule des actions variables est prédominante,
  //   par l'action de l'opérateur AND, toutes les actions variables deviennent
  //   prédominantes.
  if (niveau == *p->niveaux_groupes.begin ())
  {
    std::list <Combinaison *> *nouvelle_combinaison;
    
    nouvelle_combinaison = new std::list <Combinaison *> ();
    
    do
    {
      Action *action = static_cast <Action *> (*it4);
      
      // On ajoute l'action que si elle possède des charges
      if (!_1990_action_charges_vide (action))
      {
        Combinaison *element = new Combinaison;
        
        element->action = action;
        element->flags = _1990_action_flags_action_predominante_renvoie (
                                                         element->action) & 1U;
        nouvelle_combinaison->push_back (element);
      }
      
      ++it4;
    }
    while (it4 != groupe->elements.end ());
    
    if (!nouvelle_combinaison->empty ())
    {
      groupe->tmp_combinaison.push_back (nouvelle_combinaison);
    }
    else
    {
      delete nouvelle_combinaison;
    }
  }
  // Sinon
  //   La génération consiste à créer un nombre de combinaisons égal au produit
  //   du nombre de combinaisons de chaque élément du groupe. Par exemple, s'il
  //   y a trois groupes contenant respectivement 1, 2 et 3 éléments, il y aura
  //   ainsi 1*2*3 combinaisons différentes. Dans le cas où l'un des groupes ne
  //   possède pas d'élément, le nombre de combinaisons généré sera alors nul.
  //   Si une des actions variables est prédominante, cela n'a aucune influence
  //   sur les autres actions variables avec lesquelles elle peut être
  //   combinée. Les combinaisons sont réalisées de la façon suivante :
  //     - Première passe : on ajoute l'ensemble des combinaisons du premier
  //       groupe.
  //       soit (dans le cas de notre exemple) : 1
  //     - Deuxième passe (1) : on duplique l'ensemble des combinaisons du
  //       premier groupe autant de fois qu'il y a d'éléments dans le groupe 2.
  //       soit : 1, 1
  //     - Deuxième passe (2) : on ajoute à toutes les combinaisons créées les
  //       éléments du groupe 2.
  //       soit : 1 2_1, 1 2_2
  //     - Troisième passe : on recommence au niveau de la deuxième passe :
  //       soit : 1 2_1, 1 2_2, 1 2_1, 1 2_2, 1 2_1, 1 2_2
  //       soit : 1 2_1 3_1, 1 2_2 3_1, 1 2_1 3_2, 1 2_2 3_2, 1 2_1 3_3,
  //              1 2_2 3_3
  else
  {
    std::list <Niveau_Groupe *>::iterator it;
    
    std::list <std::list <Combinaison *> *> *nouvelles_combinaisons;
    
    nouvelles_combinaisons = new std::list <std::list <Combinaison *> *> ();
    
    it = std::find (p->niveaux_groupes.begin (),
                    p->niveaux_groupes.end (),
                    niveau);
    BUGCRIT (it != p->niveaux_groupes.end (),
             false,
             (gettext ("Impossible de trouver le niveau dans la liste des niveaux de groupes.\n")); )
    --it;
    
    do
    {
      // On se positionne sur l'élément en cours du groupe.
      groupe_n_1 = static_cast <Groupe *> (*it4);
      
      // Alors, il s'agit de la première passe. On duplique donc simplement.
      if (static_cast <Groupe *> (*groupe->elements.begin ()) == groupe_n_1)
      {
        BUG (_1990_combinaisons_duplique (nouvelles_combinaisons,
                                          &groupe_n_1->tmp_combinaison,
                                          false),
             false,
             _1990_groupe_free_combinaisons (nouvelles_combinaisons); )
      }
      else
      {
        // transition est utilisée de façon temporaire pour dupliquer
        // nouvelles_combinaisons en cas de besoin.
        std::list <std::list <Combinaison *> *>          *transition;
        std::list <std::list <Combinaison *> *>::iterator it2, it3;
        
        size_t j, i, nbboucle;
        
        transition = new std::list <std::list <Combinaison *> *> ();
        
        // On duplique les combinaisons actuellement
        // dans nouvelles_combinaisons autant de fois (moins 1) qu'il y a
        // d'éléments dans le groupe de la passe actuelle (première partie de
        // la passe 2).
        BUG (_1990_combinaisons_duplique (transition,
                                          nouvelles_combinaisons,
                                          false),
             false,
             _1990_groupe_free_combinaisons (transition);
               _1990_groupe_free_combinaisons (nouvelles_combinaisons); )
        nbboucle = groupe_n_1->tmp_combinaison.size ();
        for (i = 1; i < nbboucle; i++)
        {
          BUG (_1990_combinaisons_duplique (nouvelles_combinaisons,
                                            transition,
                                            false),
               false,
               _1990_groupe_free_combinaisons (transition);
                 _1990_groupe_free_combinaisons (nouvelles_combinaisons); )
        }
        
        // On ajoute à la fin de toutes les combinaisons dupliquées les
        // combinaisons contenues dans le groupe en cours (deuxième partie de
        // la passe 2).
        it2 = groupe_n_1->tmp_combinaison.begin ();
        it3 = nouvelles_combinaisons->begin ();
        for (i = 0; i < nbboucle; i++)
        {
          std::list <Combinaison *> *combinaison2 = *it2;
          
          for (j = 0; j < transition->size (); j++)
          {
            std::list <Combinaison *> *combinaison1 = *it3;
            
            BUG (_1990_combinaisons_fusion (&combinaison1, combinaison2),
                 false,
                 _1990_groupe_free_combinaisons (transition);
                   _1990_groupe_free_combinaisons (nouvelles_combinaisons); )
            ++it3;
          }
          
          ++it2;
        }
        
        _1990_groupe_free_combinaisons (transition);
      }
      
      ++it4;
    }
    while (it4 != groupe->elements.end ());
    
    // On ajoute définitivement les nouvelles combinaisons.
    BUG (_1990_combinaisons_duplique (&groupe->tmp_combinaison,
                                      nouvelles_combinaisons,
                                      true),
         false,
         _1990_groupe_free_combinaisons (nouvelles_combinaisons); )
    _1990_groupe_free_combinaisons (nouvelles_combinaisons);
  }
  // FinSi
  
  return true;
}


/**
 * \brief Génère toutes les combinaisons d'un groupe possédant comme type de
 *        combinaison GROUPE_COMBINAISON_OR.
 * \param p : la variable projet,
 * \param niveau : niveau du groupe à analyser,
 * \param groupe : groupe à analyser.
 * \return
 *   Succès : true.\n
 *   Échec : false :
 *     - p == NULL,
 *     - p->niveaux_groupes == NULL,
 *     - niveau,
 *     - niveau->groupes == NULL,
 *     - groupe,
 *     - groupe->type_combinaison != GROUPE_COMBINAISON_OR,
 *     - erreur d'allocation mémoire,
 *     - #_1990_combinaisons_duplique,
 *     - #_1990_combinaisons_fusion.
 */
bool
_1990_combinaisons_genere_or (Projet        *p,
                              Niveau_Groupe *niveau,
                              Groupe        *groupe)
{
  std::list <void *>::iterator it4;
  uint32_t boucle, i;
  
  BUGPARAMCRIT (p, "%p", p, false)
  INFO (!p->niveaux_groupes.empty (),
        false,
        (gettext ("Le projet ne possède pas de niveaux de groupes.\n")); )
  BUGPARAMCRIT (niveau, "%p", niveau, false)
  INFO (!niveau->groupes.empty (),
        false,
        (gettext ("Le niveau %zu est vide. Veuillez soit le remplir, soit le supprimer.\n"),
                  std::distance (p->niveaux_groupes.begin (),
                                 std::find (p->niveaux_groupes.begin (),
                                            p->niveaux_groupes.end (),
                                            niveau))); )
  BUGPARAMCRIT (groupe, "%p", groupe, false)
  INFO (groupe->type_combinaison == GROUPE_COMBINAISON_OR,
        false,
        (gettext ("Seuls les groupes possédant un type de combinaison OR peuvent appeler _1990_combinaisons_genere_or.\n")); )
  
  if (groupe == NULL)
  {
    return true;
  }
  
  boucle = 1 << groupe->elements.size ();
  
  // Si l'élément courant de niveaux_groupes est le premier de la liste Alors
  //   Afin de générer l'ensemble des combinaisons, il va être nécessaire de
  //   réaliser une boucle de 2^(g_list_length(groupe)). Le principe consiste à
  //   générer toutes les combinaisons possibles avec une combinaison de type
  //   OR. Ainsi, dans le cas où il y a trois éléments dans un groupe, il est
  //   donc possible de générer les combinaisons suivantes :
  //   000 001 010 011 100 101 110 111 Soit bien 2^3 = 8 possibilités.
  //   Chaque bit correspondant à un élément.
  //   Lorsque le bit vaut 0, l'action n'est pas prise en compte dans la
  //   combinaison
  //   Lorsque le bit vaut 1, l'action est prise en compte dans la combinaison.
  //   La nouvelle combinaison est prise en compte uniquement si au moins une
  //   action est prédominante.
  if (niveau == *p->niveaux_groupes.begin ())
  {
    for (i = 0; i < boucle; i++)
    {
      uint32_t parcours_bits = i;
      
      std::list <Combinaison *> *nouvelle_combinaison;
      
      nouvelle_combinaison = new std::list <Combinaison *> ();
      it4 = groupe->elements.begin ();
      
      do
      {
        if ((parcours_bits & 1) == 1)
        {
          Action *action = static_cast <Action *> (*it4);
          
          // On ajoute l'action que si elle possède des charges
          if (!_1990_action_charges_vide (action))
          {
            Combinaison *element;
            
            element = new Combinaison;
            element->action = action;
            element->flags = _1990_action_flags_action_predominante_renvoie (
                                                         element->action) & 1U;
            nouvelle_combinaison->push_back (element);
          }
        }
        parcours_bits = parcours_bits >> 1;
        
        ++it4;
      }
      while (parcours_bits != 0);
      
      if (nouvelle_combinaison != NULL)
      {
        groupe->tmp_combinaison.push_back (nouvelle_combinaison);
      }
    }
  }
  // Sinon
  //   Création d'une boucle de 2^(g_list_length(groupe)) combinaisons.
  //   Pour chaque itération (i variant de 0 à nb_boucle-1)
  //     Pour chaque bit de i valant 1 (chaque bit de i représente si l'élément
  //     doit être pris en compte (1) ou non (0))
  //       Si c'est la première fois dans l'itération qu'un bit vaut 1 Alors
  //         Duplication de l'ensemble des combinaisons contenu dans le groupe.
  //       Sinon
  //         Duplication de l'ensemble des combinaisons déjà généré autant de
  //         fois qu'il y a de combinaisons dans le groupe.
  //         Fusion de chaque série de doublons avec une combinaison du groupe.
  //       FinSi
  //     FinPour
  //     Si une des actions variables est prédominante, cela n'a aucune
  //     influence sur les autres actions variables avec lesquelles elle peut
  //     être combinée.
  //   FinPour
  // FinSi
  else
  {
    std::list <Niveau_Groupe *>::iterator it;
    
    it = std::find (p->niveaux_groupes.begin (),
                    p->niveaux_groupes.end (),
                    niveau);
    BUGCRIT (it != p->niveaux_groupes.end (),
             false,
             (gettext ("Impossible de trouver le niveau dans la liste des niveaux de groupes.\n")); )
    ++it;
    
    for (i = 0; i < boucle; i++)
    {
      uint32_t parcours_bits = i;
      std::list <std::list <Combinaison *> *> *nouvelles_combinaisons;
      
      nouvelles_combinaisons = new std::list <std::list <Combinaison *> *> ();
      
      it4 = groupe->elements.begin ();
      do
      {
        if ((parcours_bits & 1) == 1)
        {
          Groupe *groupe_n_1 = static_cast <Groupe *> (*it4);
          
          if (!groupe_n_1->tmp_combinaison.empty ())
          {
            // Il s'agit de la première passe. On duplique donc simplement.
            if (nouvelles_combinaisons == NULL)
            {
              BUG (_1990_combinaisons_duplique (nouvelles_combinaisons,
                                                &groupe_n_1->tmp_combinaison,
                                                false),
                   false,
                   _1990_groupe_free_combinaisons (nouvelles_combinaisons); )
            }
            else
            {
              // transition est utilisée de façon temporaire pour dupliquer
              // nouvelles_combinaisons en cas de besoin.
              std::list <std::list <Combinaison *> *>          *transition;
              std::list <std::list <Combinaison *> *>::iterator it2, it3;
              uint32_t j;
              
              transition = new std::list <std::list <Combinaison *> *> ();
              
              BUG (_1990_combinaisons_duplique (transition,
                                                nouvelles_combinaisons,
                                                false),
                   false,
                   _1990_groupe_free_combinaisons (nouvelles_combinaisons);
                     _1990_groupe_free_combinaisons (transition); )
              
              // On duplique les combinaisons actuellement dans
              // nouvelles_combinaisons autant de fois (moins 1) qu'il y a
              // d'éléments dans le groupe de la passe actuelle.
              for (j = 1; j < groupe_n_1->tmp_combinaison.size (); j++)
              {
                BUG (_1990_combinaisons_duplique (nouvelles_combinaisons,
                                                  transition,
                                                  false),
                     false,
                     _1990_groupe_free_combinaisons (nouvelles_combinaisons);
                       _1990_groupe_free_combinaisons (transition); )
              }
              
              // Ensuite on fusionne chaque série de doublon créée avec une
              // combinaison provenant de groupe_n_1.
              it2 = groupe_n_1->tmp_combinaison.begin ();
              it3 = nouvelles_combinaisons->begin ();
              for (j = 0; j < groupe_n_1->tmp_combinaison.size (); j++)
              {
                std::list <Combinaison *> *combinaison2 = *it2;
                uint32_t k;
                
                for (k = 0; k < transition->size (); k++)
                {
                  std::list <Combinaison *> *combinaison1 = *it3;
                  
                  BUG (_1990_combinaisons_fusion (&combinaison1,
                                                  combinaison2),
                       false,
                       _1990_groupe_free_combinaisons (nouvelles_combinaisons);
                         _1990_groupe_free_combinaisons (transition); )
                  
                  ++it3;
                }
                
                ++it2;
              }
              _1990_groupe_free_combinaisons (transition);
            }
          }
        }
        parcours_bits = parcours_bits >> 1;
        
        ++it4;
      }
      while (parcours_bits != 0);
      
      BUG (_1990_combinaisons_duplique (&groupe->tmp_combinaison,
                                        nouvelles_combinaisons,
                                        true),
           false,
           _1990_groupe_free_combinaisons (nouvelles_combinaisons); )
      _1990_groupe_free_combinaisons (nouvelles_combinaisons);
    }
  }
  
  return true;
}


/**
 * \brief Initialise la mémoire pour les combinaisons à l'ELU et l'ELS.
 * \param p : la variable projet.
 * \return
 *   Succès : true.\n
 *   Échec : false :
 *     - p == NULL.
 */
bool
_1990_combinaisons_init (Projet *p)
{
#ifdef ENABLE_GTK
  GtkTreeIter Iter;
#endif
  
  BUGPARAM (p, "%p", p, false)
  
  p->ponderations.elu_equ_methode = 1;
  p->ponderations.elu_geo_str_methode = 1;
  p->ponderations.elu_acc_psi = 1;
  p->ponderations.form_6_10 = 1;
  p->ponderations.elu_equ.clear ();
  p->ponderations.elu_str.clear ();
  p->ponderations.elu_geo.clear ();
  p->ponderations.elu_fat.clear ();
  p->ponderations.elu_acc.clear ();
  p->ponderations.elu_sis.clear ();
  p->ponderations.els_car.clear ();
  p->ponderations.els_freq.clear ();
  p->ponderations.els_perm.clear ();
  
#ifdef ENABLE_GTK
  p->ponderations.list_el_desc = gtk_list_store_new (1, G_TYPE_STRING);
  gtk_list_store_append (p->ponderations.list_el_desc, &Iter);
  gtk_list_store_set (p->ponderations.list_el_desc,
                      &Iter,
                      0, gettext ("ELU équilibre"),
                      -1);
  gtk_list_store_append (p->ponderations.list_el_desc, &Iter);
  gtk_list_store_set (p->ponderations.list_el_desc,
                      &Iter,
                      0, gettext ("ELU structure"),
                      -1);
  gtk_list_store_append (p->ponderations.list_el_desc, &Iter);
  gtk_list_store_set (p->ponderations.list_el_desc,
                      &Iter,
                      0, gettext ("ELU géotechnique"),
                      -1);
  gtk_list_store_append (p->ponderations.list_el_desc, &Iter);
  gtk_list_store_set (p->ponderations.list_el_desc,
                      &Iter,
                      0, gettext ("ELU fatigue"),
                      -1);
  gtk_list_store_append (p->ponderations.list_el_desc, &Iter);
  gtk_list_store_set (p->ponderations.list_el_desc,
                      &Iter,
                      0, gettext ("ELU accidentel"),
                      -1);
  gtk_list_store_append (p->ponderations.list_el_desc, &Iter);
  gtk_list_store_set (p->ponderations.list_el_desc,
                      &Iter,
                      0, gettext ("ELU sismique"),
                      -1);
  gtk_list_store_append (p->ponderations.list_el_desc, &Iter);
  gtk_list_store_set (p->ponderations.list_el_desc,
                      &Iter,
                      0, gettext ("ELS caractéristique"),
                      -1);
  gtk_list_store_append (p->ponderations.list_el_desc, &Iter);
  gtk_list_store_set (p->ponderations.list_el_desc,
                      &Iter,
                      0, gettext ("ELS fréquent"),
                      -1);
  gtk_list_store_append (p->ponderations.list_el_desc, &Iter);
  gtk_list_store_set (p->ponderations.list_el_desc,
                      &Iter,
                      0, gettext ("ELS permanent"),
                      -1);
#endif
  
  return true;
}


/**
 * \brief Fonction à utiliser avec for_each pour libérer une combinaison.
 * \param comb : une combinaison à libérer.
 * \return Rien.
 */
void
_1990_combinaisons_free_1 (std::list <Combinaison *> *comb)
{
  for_each (comb->begin (),
            comb->end (),
            std::default_delete <Combinaison> ());
  delete comb;
  
  return;
}


/**
 * \brief Libère l'ensemble des combinaisons à l'ELU et l'ELS.
 * \param p : la variable projet.
 * \return
 *   Succès : true.\n
 *   Échec : false :
 *     - p == NULL.
 */
bool
_1990_combinaisons_free (Projet *p)
{
  BUGPARAM (p, "%p", p, false)
  
  if (!p->ponderations.elu_equ.empty ())
  {
    for_each (p->ponderations.elu_equ.begin (),
              p->ponderations.elu_equ.end (),
              _1990_ponderations_free_1);
    p->ponderations.elu_equ.clear ();
  }
  if (!p->ponderations.elu_str.empty ())
  {
    for_each (p->ponderations.elu_str.begin (),
              p->ponderations.elu_str.end (),
              _1990_ponderations_free_1);
    p->ponderations.elu_str.clear ();
  }
  if (!p->ponderations.elu_geo.empty ())
  {
    for_each (p->ponderations.elu_geo.begin (),
              p->ponderations.elu_geo.end (),
              _1990_ponderations_free_1);
    p->ponderations.elu_geo.clear ();
  }
  if (!p->ponderations.elu_fat.empty ())
  {
    for_each (p->ponderations.elu_fat.begin (),
              p->ponderations.elu_fat.end (),
              _1990_ponderations_free_1);
    p->ponderations.elu_fat.clear ();
  }
  if (!p->ponderations.elu_acc.empty ())
  {
    for_each (p->ponderations.elu_acc.begin (),
              p->ponderations.elu_acc.end (),
              _1990_ponderations_free_1);
    p->ponderations.elu_acc.clear ();
  }
  if (!p->ponderations.elu_sis.empty ())
  {
    for_each (p->ponderations.elu_sis.begin (),
              p->ponderations.elu_sis.end (),
              _1990_ponderations_free_1);
    p->ponderations.elu_sis.clear ();
  }
  if (!p->ponderations.els_car.empty ())
  {
    for_each (p->ponderations.els_car.begin (),
              p->ponderations.els_car.end (),
              _1990_ponderations_free_1);
    p->ponderations.els_car.clear ();
  }
  if (!p->ponderations.els_freq.empty ())
  {
    for_each (p->ponderations.els_freq.begin (),
              p->ponderations.els_freq.end (),
              _1990_ponderations_free_1);
    p->ponderations.els_freq.clear ();
  }
  if (!p->ponderations.els_perm.empty ())
  {
    for_each (p->ponderations.els_perm.begin (),
              p->ponderations.els_perm.end (),
              _1990_ponderations_free_1);
    p->ponderations.els_perm.clear ();
  }
  
#ifdef ENABLE_GTK
  g_object_unref (p->ponderations.list_el_desc);
#endif
  
  return true;
}


/**
 * \brief Modifie les options de combinaison pour que l'ELU EQU calcule à
 *        l'équilibre seulement.
 * \param p : la variable projet.
 * \return
 *   Succès : true.\n
 *   Échec : false :
 *     - p == NULL.
 */
bool
_1990_combinaisons_eluequ_equ_seul (Projet *p)
{
  BUGPARAM (p, "%p", p, false)
  
  p->ponderations.elu_equ_methode = 0;
  
  return true;
}


/**
 * \brief Modifie les options de combinaison pour que l'ELU EQU calcule à
 *        l'équilibre et à la résistance structurelle.
 * \param p : la variable projet.
 * \return
 *   Succès : true.\n
 *   Échec : false :
 *     - p == NULL.
 */
bool
_1990_combinaisons_eluequ_equ_resist (Projet *p)
{
  BUGPARAM (p, "%p", p, false)
  
  p->ponderations.elu_equ_methode = 1;
  
  return true;
}


/**
 * \brief Modifie les options de combinaison pour que l'ELU STR/GEO calcule
 *        selon l'approche 1.
 * \param p : la variable projet.
 * \return
 *   Succès : true.\n
 *   Échec : false :
 *     - p == NULL.
 */
bool
_1990_combinaisons_elustrgeo_1 (Projet *p)
{
  BUGPARAM (p, "%p", p, false)
  
  p->ponderations.elu_geo_str_methode = 0;
  
  return true;
}


/**
 * \brief Modifie les options de combinaison pour que l'ELU STR/GEO calcule
 *        selon l'approche 2.
 * \param p : la variable projet.
 * \return
 *   Succès : true.\n
 *   Échec : false :
 *     - p == NULL.
 */
bool
_1990_combinaisons_elustrgeo_2 (Projet *p)
{
  BUGPARAM (p, "%p", p, false)
  
  p->ponderations.elu_geo_str_methode = 1;
  
  return true;
}


/**
 * \brief Modifie les options de combinaison pour que l'ELU STR/GEO calcule
 *        selon l'approche 3.
 * \param p : la variable projet.
 * \return
 *   Succès : true.\n
 *   Échec : false :
 *     - p == NULL.
 */
bool
_1990_combinaisons_elustrgeo_3 (Projet *p)
{
  BUGPARAM (p, "%p", p, false)
  
  p->ponderations.elu_geo_str_methode = 2;
  
  return true;
}


/**
 * \brief Modifie les options de combinaison pour que l'ELU STR/GEO calcule
 *        selon la formule 6.10 (a) et (b) de l'EN 1990.
 * \param p : la variable projet.
 * \return
 *   Succès : true.\n
 *   Échec : false :
 *     - p == NULL.
 */
bool
_1990_combinaisons_elustrgeo_6_10ab (Projet *p)
{
  BUGPARAM (p, "%p", p, false)
  
  p->ponderations.form_6_10 = 0;
  
  return true;
}


/**
 * \brief Modifie les options de combinaison pour que l'ELU STR/GEO calcule
 *        selon la formule 6.10 de l'EN 1990.
 * \param p : la variable projet.
 * \return
 *   Succès : true.\n
 *   Échec : false :
 *     - p == NULL.
 */
bool
_1990_combinaisons_elustrgeo_6_10 (Projet *p)
{
  BUGPARAM (p, "%p", p, false)
  
  p->ponderations.form_6_10 = 1;
  
  return true;
}


/**
 * \brief Modifie les options de combinaison pour que l'ELU ACC calcule avec
 *        les valeurs fréquentes des actions variables.
 * \param p : la variable projet.
 * \return
 *   Succès : true.\n
 *   Échec : false :
 *     - p == NULL.
 */
bool
_1990_combinaisons_eluacc_frequente (Projet *p)
{
  BUGPARAM (p, "%p", p, false)
  
  p->ponderations.elu_acc_psi = 0;
  
  return true;
}


/**
 * \brief Modifie les options de combinaison pour que l'ELU ACC calcule avec
 *        les valeurs quasi permanente des actions variables.
 * \param p : la variable projet.
 * \return
 *   Succès : true.\n
 *   Échec : false :
 *     - p == NULL.
 */
bool
_1990_combinaisons_eluacc_quasi_permanente (Projet *p)
{
  BUGPARAM (p, "%p", p, false)
  
  p->ponderations.elu_acc_psi = 1;
  
  return true;
}


/**
 * \brief Génère l'ensemble des combinaisons et pondérations du projet.
 * \param p : la variable projet.
 * \return
 *   Succès : true.\n
 *   Échec : false :
 *     - p == NULL,
 *     - p->niveaux_groupes == NULL,
 *     - _1990_combinaisons_free,
 *     - _1990_action_categorie_bat,
 *     - _1990_combinaisons_genere_or,
 *     - _1990_combinaisons_genere_xor,
 *     - _1990_combinaisons_genere_and,
 *     - _1990_ponderations_genere.
 */
bool
_1990_combinaisons_genere (Projet *p)
{
  uint32_t i;
  
  BUGPARAM (p, "%p", p, false)
  INFO (!p->niveaux_groupes.empty (),
        false,
        (gettext ("Le projet ne possède pas de niveaux de groupes.\n")); )
  
#ifdef ENABLE_GTK
  g_object_ref (p->ponderations.list_el_desc);
#endif
  BUG (_1990_combinaisons_free (p), false)
  
  // Pour chaque action
  //   Cette boucle permet de générer toutes les combinaisons en prenant en
  //   compte le principe d'action prédominante. Ainsi, à chaque passage de la
  //   boucle, l'action numéro i est considérée comme action prédominante.
  for (i = 0; i < p->actions.size (); i++)
  {
    std::list <Niveau_Groupe *>::iterator it;
    std::list <Action        *>::iterator it2;
    std::list <Groupe        *>::iterator it3;
    
    uint32_t         j;
    Action_Categorie categorie;
    
    // On supprime les combinaisons temporaires générées lors du passage de la
    // boucle précédente.
    it = p->niveaux_groupes.begin ();
    
    while (it != p->niveaux_groupes.end ())
    {
      Niveau_Groupe *niveau = *it;
      
      it3 = niveau->groupes.begin ();
      
      while (it3 != niveau->groupes.end ())
      {
        Groupe *groupe = *it3;
         
        _1990_groupe_free_combinaisons (&groupe->tmp_combinaison);
        
        ++it3;
      }
      
      ++it;
    }
    
    // Attribution à l'action numéro i du flags "action prédominante" à 1 afin
    // d'indiquer qu'il s'agit d'une action prédominante et mise à 0 pour les
    // autres actions.
    it2 = p->actions.begin ();
    for (j = 0; j < p->actions.size (); j++)
    {
      Action *action = *it2;
      
      if (j != i)
      {
        BUG (_1990_action_flags_action_predominante_change (action, 0), false)
      }
      else
      {
        categorie = _1990_action_categorie_bat (_1990_action_type_renvoie (
                                                                       action),
                                                p->parametres.norme);
        BUG (categorie != ACTION_INCONNUE, false)
        if (categorie == ACTION_VARIABLE)
        {
          BUG (_1990_action_flags_action_predominante_change (action, 1),
               false)
        }
        else
        {
          BUG (_1990_action_flags_action_predominante_change (action, 0),
               false)
        }
      }
      
      ++it2;
    }
    
  //   Générer des combinaisons de toutes les groupes pour tous les niveaux :
  //   Pour chaque niveau de niveaux_groupes
  //     Pour chaque groupe du niveau en cours
  //       Génération des combinaisons avec l'action prédominante définie
  //       précédemment.
  //     FinPour
  //   FinPour
    it = p->niveaux_groupes.begin ();
    
    while (it != p->niveaux_groupes.end ())
    {
      Niveau_Groupe *niveau = *it;
      
      it3 = niveau->groupes.begin ();
      
      while (it3 != niveau->groupes.end ())
      {
        Groupe *groupe = *it3;
        
        switch (groupe->type_combinaison)
        {
          case GROUPE_COMBINAISON_OR :
          {
            BUG (_1990_combinaisons_genere_or (p, niveau, groupe), false)
            break;
          }
          case GROUPE_COMBINAISON_XOR :
          {
            BUG (_1990_combinaisons_genere_xor (p, niveau, groupe), false)
            break;
          }
          case GROUPE_COMBINAISON_AND :
          {
            BUG (_1990_combinaisons_genere_and (p, niveau, groupe), false)
            break;
          }
          default :
          {
            FAILCRIT (false,
                      (gettext ("Le type de combinaison %d est inconnu.\n"),
                                groupe->type_combinaison); )
            break;
          }
        }
        
        ++it3;
      }
      
      ++it;
    }
    
    // Génération des pondérations (avec les coefficients de sécurité partiels)
    // à partir des combinaisons.
    BUG (_1990_ponderations_genere (p), false)
    BUG (_1990_groupe_affiche_tout (p), false)
  }
  // FinPour
  
  return true;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
