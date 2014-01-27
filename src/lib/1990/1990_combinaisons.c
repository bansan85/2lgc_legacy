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


gboolean
_1990_combinaisons_verifie_double (GList *liste_combinaisons,
                                   GList *comb_a_verifier)
/**
 * \brief Vérifie si une combinaison est déjà présente dans une liste de
 *        combinaisons.
 * \param liste_combinaisons : liste de combinaisons,
 * \param comb_a_verifier : combinaison à vérifier.
 * \return
 *    - FALSE si la combinaison n'est pas présente,
 *    - TRUE si la combinaison est présente,
 *    - TRUE si la combinaison est vide (afin de ne pas l'ajouter).
 */
{
  GList *list_parcours;
  
  if (liste_combinaisons == NULL)
    return FALSE;
  if (comb_a_verifier == NULL)
    return TRUE;
  
  list_parcours = liste_combinaisons;
  do
  {
    gboolean  doublon = TRUE;
    GList    *comb_en_cours = list_parcours->data;
    GList    *list_1, *list_2;
    
    list_1 = comb_en_cours;
    list_2 = comb_a_verifier;
    
    while ((list_1 != NULL) && (list_2 != NULL) && (doublon == TRUE))
    {
      Combinaison *elem1;
      Combinaison *elem2;
      
      elem1 = list_1->data;
      elem2 = list_2->data;
      
      /* On vérifie que chaque élément pointe vers la même action
       *  et que les flags (paramètres de calculs) sont les mêmes */
      if ((elem1->action != elem2->action) || (elem1->flags != elem2->flags))
        doublon = FALSE;
      
      list_1 = g_list_next(list_1);
      list_2 = g_list_next(list_2);
    }
    
    if (doublon == TRUE)
      return TRUE;
    
    list_parcours = g_list_next(list_parcours);
  }
  while (list_parcours != NULL);
  
  return FALSE;
}


gboolean
_1990_combinaisons_duplique (GList  **liste_comb_destination,
                             GList   *liste_comb_source,
                             gboolean sans_double)
/**
 * \brief Ajoute à une liste de combinaisons existante une liste de
 *        combinaisons.
 * \param liste_comb_destination : liste de combinaisons qui recevra les
 *                                 combinaisons sources,
 * \param liste_comb_source : liste de combinaisons source,
 * \param sans_double : TRUE pour qu'aucune combinaison ne soit ajoutée en
 *                      double.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - liste_comb_destination == NULL,
 *     - erreur d'allocation mémoire.
 */
{
  GList *list_parcours;
  
  BUGMSG (liste_comb_destination,
          FALSE,
          gettext ("Paramètre %s incorrect.\n"), "liste_comb_destination")
  
  if (liste_comb_source == NULL)
    return TRUE;
  
  list_parcours = liste_comb_source;
  do
  {
    GList    *combinaison_source = list_parcours->data;
    gboolean  verifie_double;
    
    if (sans_double == TRUE)
      verifie_double = _1990_combinaisons_verifie_double (
                                  *liste_comb_destination, combinaison_source);
    else
      verifie_double = FALSE;
    
    if (verifie_double == FALSE)
    {
      GList *combinaison_destination;
      
      // Duplication de la combinaison
      combinaison_destination = NULL;
      if (combinaison_source != NULL)
      {
        GList *list_parcours2 = combinaison_source;
        
        do
        {
          Combinaison *element_source = list_parcours2->data;
          Combinaison *element_destination;
          
          BUGMSG (element_destination = malloc (sizeof (Combinaison)),
                  FALSE,
                  gettext ("Erreur d'allocation mémoire.\n"))
          
          element_destination->action = element_source->action;
          element_destination->flags = element_source->flags;
          
          combinaison_destination = g_list_append (combinaison_destination,
                                                   element_destination);
          
          list_parcours2 = g_list_next (list_parcours2);
        }
        while (list_parcours2 != NULL);
      }
      
      // Insertion de la combinaison dans liste_comb_destination
      *liste_comb_destination = g_list_append (*liste_comb_destination,
                                               combinaison_destination);
      
    }
    
    list_parcours = g_list_next (list_parcours);
  }
  while (list_parcours != NULL);
  
  return TRUE;
}


gboolean
_1990_combinaisons_action_predominante (GList *combinaison,
                                        Norme  norme)
/**
 * \brief Modifie le flag de toutes les actions variables d'une combinaison
 *        afin de les considérer comme action prédominante.
 * \param combinaison : combinaison à modifier,
 * \param norme : la norme.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - une des actions est de type inconnue.
 */
{
  GList   *list_parcours;
  
  if (combinaison == NULL)
    return TRUE;
  
  list_parcours = combinaison;
  do
  {
    Combinaison     *combinaison_element = list_parcours->data;
    Action_Categorie categorie;
    
    categorie = _1990_action_categorie_bat( _1990_action_type_renvoie(
                                          combinaison_element->action), norme);
    BUG (categorie != ACTION_INCONNUE, FALSE)
    if (categorie == ACTION_VARIABLE)
      combinaison_element->flags = 1;
    
    list_parcours = g_list_next(list_parcours);
  }
  while (list_parcours != NULL);
  
  return TRUE;
}


gboolean
_1990_combinaisons_genere_xor (Projet        *p,
                               Niveau_Groupe *niveau,
                               Groupe        *groupe)
/**
 * \brief Génère toutes les combinaisons d'un groupe possédant comme type de
 *        combinaison GROUPE_COMBINAISON_XOR dans le champ tmp_combinaison.
 * \param p : la variable projet,
 * \param niveau : niveau du groupe à analyser,
 * \param groupe : groupe à analyser.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL,
 *     - p->niveaux_groupes == NULL (pas de niveau),
 *     - niveau == NULL,
 *     - niveau->groupes == NULL,
 *     - groupe->type_combinaison != GROUPE_COMBINAISON_XOR,
 *     - erreur d'allocation mémoire,
 *     - _1990_combinaisons_duplique.
 */
{
  GList *list_parcours;
  
  BUGMSG (p, FALSE, gettext ("Paramètre %s incorrect.\n"), "projet")
  BUGMSG (p->niveaux_groupes,
          FALSE,
          gettext ("Le projet ne possède pas de niveaux de groupes.\n"))
  BUGMSG (niveau,
          FALSE,
          gettext ("Paramètre %s incorrect.\n"), "niveau")
  BUGMSG (niveau->groupes,
          FALSE,
          gettext ("Le niveau %u est vide. Veuillez soit le remplir, soit le supprimer.\n"),
                   g_list_index (p->niveaux_groupes, niveau))
  BUGMSG (groupe->type_combinaison == GROUPE_COMBINAISON_XOR,
          FALSE,
          gettext("Seuls les groupes possédant un type de combinaison XOR peuvent appeler _1990_combinaisons_genere_xor.\n"))
  
  // Si le nombre d'éléments est 0 Alors
  //   Fin.
  // FinSi
  if (groupe->elements == NULL)
    return TRUE;
  
  list_parcours = groupe->elements;
  
  // Si l'élément courant de niveaux_groupes est le premier de la liste Alors
  //   L'opération XOR consiste à ajouter les actions listées dans le groupe
  //   courant du premier élément de niveaux_groupes à la suite des autres sans
  //   aucune combinaison entre elles. On crée donc autant de combinaisons
  //   contenant une seule action qu'il y a d'éléments.
  if (niveau == p->niveaux_groupes->data)
  {
    
    do
    {
      GList       *nouvelle_combinaison;
      Combinaison *element;
      Action      *action = list_parcours->data;
      
      // On vérifie si l'action possède une charge. Si non, on ignore l'action.
      if (!_1990_action_charges_vide (action))
      {
        BUGMSG (element = malloc (sizeof (Combinaison)),
                FALSE,
                gettext ("Erreur d'allocation mémoire.\n"))
        nouvelle_combinaison = NULL;
        element->action = action;
        element->flags = _1990_action_flags_action_predominante_renvoie (
                                                              element->action);
        nouvelle_combinaison = g_list_append (nouvelle_combinaison, element);
        groupe->tmp_combinaison = g_list_append (groupe->tmp_combinaison,
                                                 nouvelle_combinaison);
      }
      
      list_parcours = g_list_next (list_parcours);
    }
    while (list_parcours != NULL);
  }
  // Sinon
  //   Un XOR pour les autres étages consiste à ajouter toutes les combinaisons
  //   des groupes de l'étage n-1 qui sont indiquées dans le groupe de l'étage
  //   n.
  else
  {
    GList *list_groupe_n_1;
    
    list_groupe_n_1 = g_list_find (p->niveaux_groupes, niveau);
    BUGMSG (list_groupe_n_1,
            FALSE,
            gettext ("Impossible de trouver le niveau dans la liste des niveaux de groupes.\n"))
    list_groupe_n_1 = g_list_previous (list_groupe_n_1);
    BUGMSG (list_groupe_n_1,
            FALSE,
            gettext("Impossible de trouver le niveau précédent le niveau.\n"))
    
    niveau = list_groupe_n_1->data;
    do
    {
      Groupe *groupe_n_1 = list_parcours->data;
      
      BUG (_1990_combinaisons_duplique (&(groupe->tmp_combinaison),
                                        groupe_n_1->tmp_combinaison, TRUE),
           FALSE)
      
      list_parcours = g_list_next (list_parcours);
    }
    while (list_parcours != NULL);
  }
  // FinSi
  
  return TRUE;
}


gboolean
_1990_combinaisons_fusion (GList *destination,
                           GList *source)
/**
 * \brief Fusionne deux combinaisons. La liste des combinaisons source est
 *        copiée à la fin de la liste des combinaisons destination. Il est donc
 *        toujours nécessaire de libérer la liste source.
 * \param destination : combinaison de destination,
 * \param source : combinaison source.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - destination == NULL,
 *     - erreur d'allocation mémoire.
 */
{
  GList   *list_parcours;
  
  BUGMSG (destination,
          FALSE,
          gettext ("Paramètre %s incorrect.\n"), "destination")
  
  list_parcours = source;
  while (list_parcours != NULL)
  {
    Combinaison *element_source = list_parcours->data;
    Combinaison *element_destination = malloc (sizeof (Combinaison));
    
    BUGMSG (element_destination,
            FALSE,
            gettext ("Erreur d'allocation mémoire.\n"))
    element_destination->action = element_source->action;
    element_destination->flags = element_source->flags;
    destination = g_list_append (destination, element_destination);
    
    list_parcours = g_list_next (list_parcours);
  }
  
  return TRUE;
}


void
_1990_combinaisons_free_groupe_tmp_combinaison (void *data)
/**
 * \brief Permet de supprimer toutes les combinaisons temporaires contenues
 *        dans les groupes. À utiliser avec la fonction list_traverse.
 * \param data : donnée à libérer.
 * \return void.
 */
{
  GList *combinaison = (GList *) data;
  
  BUGMSG (data, , gettext ("Paramètre %s incorrect.\n"), "data")
  
  if (combinaison != NULL)
    g_list_free_full (combinaison, g_free);
  
  return;
}


gboolean
_1990_combinaisons_genere_and (Projet        *p,
                               Niveau_Groupe *niveau,
                               Groupe        *groupe)
/**
 * \brief Génère toutes les combinaisons d'un groupe possédant comme type de
 *        combinaison GROUPE_COMBINAISON_AND.
 * \param p : la variable projet,
 * \param niveau : niveau du groupe à analyser,
 * \param groupe : groupe à analyser.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL,
 *     - p->niveaux_groupes == NULL,
 *     - niveau == NULL,
 *     - niveau->groupes == NULL,
 *     - groupe == NULL,
 *     - groupe->type_combinaison != GROUPE_COMBINAISON_AND,
 *     - erreur d'allocation mémoire,
 *     - _1990_combinaisons_action_predominante,
 *     - _1990_combinaisons_duplique,
 *     - _1990_combinaisons_fusion.
 */
{
  GList  *list_parcours;
  Groupe *groupe_n_1;
  
  BUGMSG (p, FALSE, gettext ("Paramètre %s incorrect.\n"), "projet")
  BUGMSG (p->niveaux_groupes,
          FALSE,
          gettext ("Le projet ne possède pas de niveaux de groupes.\n"))
  BUGMSG (niveau, FALSE, gettext ("Paramètre %s incorrect.\n"), "niveau")
  BUGMSG (niveau->groupes,
          FALSE,
          gettext ("Le niveau %u est vide. Veuillez soit le remplir, soit le supprimer.\n"), g_list_index (p->niveaux_groupes, niveau))
  BUGMSG (groupe, FALSE, gettext ("Paramètre %s incorrect.\n"), "groupe")
  BUGMSG (groupe->type_combinaison == GROUPE_COMBINAISON_AND,
          FALSE,
          gettext ("Seuls les groupes possédant un type de combinaison AND peuvent appeler _1990_combinaisons_genere_and.\n"))
  
  if (groupe->elements == NULL)
    return TRUE;
  
  list_parcours = groupe->elements;
  
  // Si l'élément courant de niveaux_groupes est le premier de la liste Alors
  //   L'opération AND consiste à créer une combinaison contenant l'ensemble
  //   des actions listées dans le groupe courant du premier élément de
  //   niveaux_groupes. Si une seule des actions variables est prédominante,
  //   par l'action de l'opérateur AND, toutes les actions variables deviennent
  //   prédominantes.
  if (niveau == p->niveaux_groupes->data)
  {
    GList *comb;
    int    action_predominante = 0;
    
    comb = NULL;
    
    do
    {
      Combinaison *element;
      Action      *action = list_parcours->data;
      
      // On ajoute l'action que si elle possède des charges
      if (!_1990_action_charges_vide (action))
      {
        BUGMSG (element = malloc (sizeof (Combinaison)),
                FALSE,
                gettext ("Erreur d'allocation mémoire.\n"))
        element->action = action;
        element->flags = _1990_action_flags_action_predominante_renvoie (
                                                              element->action);
        if ((element->flags & 1) != 0)
          action_predominante = 1;
        comb = g_list_append (comb, element);
      }
      
      list_parcours = g_list_next (list_parcours);
    }
    while (list_parcours != NULL);
    
    if (comb != NULL)
    {
      if (action_predominante == 1)
        BUG (_1990_combinaisons_action_predominante (comb,
                                                     p->parametres.norme),
             FALSE)
      groupe->tmp_combinaison = g_list_append (groupe->tmp_combinaison, comb);
    }
    else
      free (comb);
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
    GList *list_groupe_n_1;
    GList *nouvelles_combinaisons;
    
    nouvelles_combinaisons = NULL;
    
    list_groupe_n_1 = g_list_find (p->niveaux_groupes, niveau);
    BUGMSG (list_groupe_n_1,
            FALSE,
            gettext ("Impossible de trouver le niveau dans la liste des niveaux de groupes.\n"))
    list_groupe_n_1 = g_list_previous (list_groupe_n_1);
    BUGMSG (list_groupe_n_1,
            FALSE,
            gettext ("Impossible de trouver le niveau précédent le niveau.\n"))
    niveau = list_groupe_n_1->data;
    
    do
    {
      // On se positionne sur l'élément en cours du groupe.
      groupe_n_1 = list_parcours->data;
      
      // Alors, il s'agit de la première passe. On duplique donc simplement.
      if (groupe->elements->data == groupe_n_1)
        BUG (_1990_combinaisons_duplique (&nouvelles_combinaisons,
                                          groupe_n_1->tmp_combinaison,
                                          FALSE),
             FALSE)
      else
      {
        // transition est utilisée de façon temporaire pour dupliquer
        // nouvelles_combinaisons en cas de besoin.
        GList       *transition = NULL;
        GList       *list_parcours2, *list_parcours3;
        unsigned int j, i, nbboucle;
        
        // On duplique les combinaisons actuellement
        // dans nouvelles_combinaisons autant de fois (moins 1) qu'il y a
        // d'éléments dans le groupe de la passe actuelle (première partie de
        // la passe 2).
        BUG (_1990_combinaisons_duplique (&transition,
                                          nouvelles_combinaisons,
                                          FALSE),
             FALSE)
        nbboucle = g_list_length (groupe_n_1->tmp_combinaison);
        for (i = 2; i <= nbboucle; i++)
          BUG (_1990_combinaisons_duplique (&nouvelles_combinaisons,
                                            transition,
                                            FALSE),
               FALSE)
        
        // On ajoute à la fin de toutes les combinaisons dupliquées les
        // combinaisons contenues dans le groupe en cours (deuxième partie de
        // la passe 2).
        list_parcours2 = groupe_n_1->tmp_combinaison;
        list_parcours3 = nouvelles_combinaisons;
        for (i = 1; i <= nbboucle; i++)
        {
          GList *combinaison2;
          
          combinaison2 = list_parcours2->data;
          for (j = 1; j <= g_list_length (transition); j++)
          {
            GList *combinaison1;
            
            combinaison1 = list_parcours3->data;
            BUG (_1990_combinaisons_fusion (combinaison1, combinaison2), FALSE)
            list_parcours3 = g_list_next (list_parcours3);
          }
          
          list_parcours2 = g_list_next (list_parcours2);
        }
        
        g_list_free_full (transition,
                          &(_1990_combinaisons_free_groupe_tmp_combinaison));
      }
      
      list_parcours = g_list_next (list_parcours);
    }
    while (list_parcours != NULL);
    
    // On ajoute définitivement les nouvelles combinaisons.
    BUG (_1990_combinaisons_duplique (&(groupe->tmp_combinaison),
                                      nouvelles_combinaisons,
                                      TRUE),
         FALSE)
    g_list_free_full (nouvelles_combinaisons,
                      &_1990_combinaisons_free_groupe_tmp_combinaison);
  }
  // FinSi
  
  return TRUE;
}


gboolean
_1990_combinaisons_genere_or (Projet        *p,
                              Niveau_Groupe *niveau,
                              Groupe        *groupe)
/**
 * \brief Génère toutes les combinaisons d'un groupe possédant comme type de
 *        combinaison GROUPE_COMBINAISON_OR.
 * \param p : la variable projet,
 * \param niveau : niveau du groupe à analyser,
 * \param groupe : groupe à analyser.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL,
 *     - p->niveaux_groupes == NULL,
 *     - niveau,
 *     - niveau->groupes == NULL,
 *     - groupe,
 *     - groupe->type_combinaison != GROUPE_COMBINAISON_OR,
 *     - erreur d'allocation mémoire,
 *     - _1990_combinaisons_duplique,
 *     - _1990_combinaisons_fusion.
 */
{
  GList       *list_parcours;
  unsigned int boucle, i, j, k;
  
  BUGMSG (p, FALSE, gettext ("Paramètre %s incorrect.\n"), "projet")
  BUGMSG (p->niveaux_groupes,
          FALSE,
          gettext ("Le projet ne possède pas de niveaux de groupes.\n"))
  BUGMSG (niveau, FALSE, gettext ("Paramètre %s incorrect.\n"), "niveau")
  BUGMSG (niveau->groupes,
          FALSE,
          gettext("Le niveau %u est vide. Veuillez soit le remplir, soit le supprimer.\n"), g_list_index (p->niveaux_groupes, niveau))
  BUGMSG (groupe, FALSE, gettext ("Paramètre %s incorrect.\n"), "groupe")
  BUGMSG (groupe->type_combinaison == GROUPE_COMBINAISON_OR,
          FALSE,
          gettext ("Seuls les groupes possédant un type de combinaison OR peuvent appeler _1990_combinaisons_genere_or.\n"))
  
  if (groupe == NULL)
    return TRUE;
  
  boucle = 1 << g_list_length (groupe->elements);
  
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
  if (niveau == p->niveaux_groupes->data)
  {
    for (i = 0; i < boucle; i++)
    {
      unsigned int parcours_bits = i;
      int          action_predominante = 0;
      GList       *nouvelle_combinaison;
      
      nouvelle_combinaison = NULL;
      
      list_parcours = groupe->elements;
      
      do
      {
        if ((parcours_bits & 1) == 1)
        {
          Combinaison *element;
          Action      *action = list_parcours->data;
          
          // On ajoute l'action que si elle possède des charges
          if (!_1990_action_charges_vide (action))
          {
            BUGMSG (element = malloc (sizeof (Combinaison)),
                    FALSE,
                    gettext ("Erreur d'allocation mémoire.\n"))
            element->action = action;
            element->flags = _1990_action_flags_action_predominante_renvoie (
                                                              element->action);
            if ((element->flags & 1) != 0)
              action_predominante = 1;
            nouvelle_combinaison = g_list_append (nouvelle_combinaison,
                                                  element);
          }
        }
        parcours_bits = parcours_bits >> 1;
        list_parcours = g_list_next (list_parcours);
      }
      while (parcours_bits != 0);
      
      if (nouvelle_combinaison != NULL)
      {
        if (action_predominante == 1)
          BUG (_1990_combinaisons_action_predominante (nouvelle_combinaison,
                                                       p->parametres.norme),
               FALSE)
        groupe->tmp_combinaison = g_list_append (groupe->tmp_combinaison,
                                                 nouvelle_combinaison);
      }
      else
        free (nouvelle_combinaison);
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
    GList *list_groupe_n_1;
    
    list_groupe_n_1 = g_list_find (p->niveaux_groupes, niveau);
    BUGMSG (list_groupe_n_1,
            FALSE,
            gettext ("Impossible de trouver le niveau dans la liste des niveaux de groupes.\n"))
    list_groupe_n_1 = g_list_previous (list_groupe_n_1);
    BUGMSG (list_groupe_n_1,
            FALSE,
            gettext ("Impossible de trouver le niveau précédent le niveau.\n"))
    niveau = list_groupe_n_1->data;
    
    for (i = 0; i < boucle; i++)
    {
      unsigned int parcours_bits = i;
      GList       *nouvelles_combinaisons;
      
      nouvelles_combinaisons = NULL;
      
      list_parcours = groupe->elements;
      do
      {
        if ((parcours_bits & 1) == 1)
        {
          Groupe *groupe_n_1 = list_parcours->data;
          
          if (groupe_n_1->tmp_combinaison != NULL)
          {
            // Il s'agit de la première passe. On duplique donc simplement.
            if (nouvelles_combinaisons == NULL)
              BUG (_1990_combinaisons_duplique (&nouvelles_combinaisons,
                                                groupe_n_1->tmp_combinaison,
                                                FALSE),
                   FALSE)
            else
            {
              // transition est utilisée de façon temporaire pour dupliquer
              // nouvelles_combinaisons en cas de besoin.
              GList *transition = NULL;
              GList *list_parcours2, *list_parcours3;
              
              BUG (_1990_combinaisons_duplique (&transition,
                                                nouvelles_combinaisons,
                                                FALSE),
                   FALSE)
              
              // On duplique les combinaisons actuellement dans
              // nouvelles_combinaisons autant de fois (moins 1) qu'il y a
              // d'éléments dans le groupe de la passe actuelle.
              for (j = 2; j <= g_list_length (groupe_n_1->tmp_combinaison); j++)
                BUG (_1990_combinaisons_duplique (&nouvelles_combinaisons,
                                                  transition,
                                                  FALSE),
                     FALSE)
              
              // Ensuite on fusionne chaque série de doublon créée avec une
              // combinaison provenant de groupe_n_1.
              list_parcours2 = groupe_n_1->tmp_combinaison;
              list_parcours3 = nouvelles_combinaisons;
              for (j = 1; j <= g_list_length (groupe_n_1->tmp_combinaison); j++)
              {
                GList *combinaison2 = list_parcours2->data;
                
                for (k = 1; k <= g_list_length (transition); k++)
                {
                  GList *combinaison1 = list_parcours3->data;
                  
                  BUG (_1990_combinaisons_fusion (combinaison1,
                                                  combinaison2),
                       FALSE)
                  list_parcours3 = g_list_next (list_parcours3);
                }
                list_parcours2 = g_list_next (list_parcours2);
              }
              g_list_free_full (transition,
                              &_1990_combinaisons_free_groupe_tmp_combinaison);
            }
          }
        }
        parcours_bits = parcours_bits >> 1;
        list_parcours = g_list_next (list_parcours);
      }
      while (parcours_bits != 0);
      
      BUG (_1990_combinaisons_duplique (&(groupe->tmp_combinaison),
                                        nouvelles_combinaisons,
                                        TRUE),
           FALSE)
      g_list_free_full (nouvelles_combinaisons,
                        &_1990_combinaisons_free_groupe_tmp_combinaison);
    }
  }
  
  return TRUE;
}


gboolean
_1990_combinaisons_init (Projet *p)
/**
 * \brief Initialise la mémoire pour les combinaisons à l'ELU et l'ELS.
 * \param p : la variable projet.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL.
 */
{
#ifdef ENABLE_GTK
  GtkTreeIter Iter;
#endif
  
  BUGMSG (p, FALSE, gettext ("Paramètre %s incorrect.\n"), "projet")
  
  p->combinaisons.flags = 1 | 4 | 8;
  p->combinaisons.elu_equ = NULL;
  p->combinaisons.elu_str = NULL;
  p->combinaisons.elu_geo = NULL;
  p->combinaisons.elu_fat = NULL;
  p->combinaisons.elu_acc = NULL;
  p->combinaisons.elu_sis = NULL;
  p->combinaisons.els_car = NULL;
  p->combinaisons.els_freq = NULL;
  p->combinaisons.els_perm = NULL;
  
#ifdef ENABLE_GTK
  p->combinaisons.list_el_desc = gtk_list_store_new (1, G_TYPE_STRING);
  gtk_list_store_append (p->combinaisons.list_el_desc, &Iter);
  gtk_list_store_set (p->combinaisons.list_el_desc,
                      &Iter,
                      0, gettext ("ELU équilibre"),
                      -1);
  gtk_list_store_append (p->combinaisons.list_el_desc, &Iter);
  gtk_list_store_set (p->combinaisons.list_el_desc,
                      &Iter,
                      0, gettext ("ELU structure"),
                      -1);
  gtk_list_store_append (p->combinaisons.list_el_desc, &Iter);
  gtk_list_store_set (p->combinaisons.list_el_desc,
                      &Iter,
                      0, gettext ("ELU géotechnique"),
                      -1);
  gtk_list_store_append (p->combinaisons.list_el_desc, &Iter);
  gtk_list_store_set (p->combinaisons.list_el_desc,
                      &Iter,
                      0, gettext ("ELU fatigue"),
                      -1);
  gtk_list_store_append (p->combinaisons.list_el_desc, &Iter);
  gtk_list_store_set (p->combinaisons.list_el_desc,
                      &Iter,
                      0, gettext ("ELU accidentel"),
                      -1);
  gtk_list_store_append (p->combinaisons.list_el_desc, &Iter);
  gtk_list_store_set (p->combinaisons.list_el_desc,
                      &Iter,
                      0, gettext ("ELU sismique"),
                      -1);
  gtk_list_store_append (p->combinaisons.list_el_desc, &Iter);
  gtk_list_store_set (p->combinaisons.list_el_desc,
                      &Iter,
                      0, gettext ("ELS caractéristique"),
                      -1);
  gtk_list_store_append (p->combinaisons.list_el_desc, &Iter);
  gtk_list_store_set (p->combinaisons.list_el_desc,
                      &Iter,
                      0, gettext ("ELS fréquent"),
                      -1);
  gtk_list_store_append (p->combinaisons.list_el_desc, &Iter);
  gtk_list_store_set (p->combinaisons.list_el_desc,
                      &Iter,
                      0, gettext ("ELS permanent"),
                      -1);
#endif
  
  return TRUE;
}


void
_1990_combinaisons_free_1 (void *data)
/**
 * \brief Fonction à utiliser avec g_list_free_full pour libérer une
 *        combinaison.
 * \param data : une pondération à libérer.
 * \return void.
 */
{
  GList   *pond = data;
  
  g_list_free_full (pond, free);
  
  return;
}


gboolean
_1990_combinaisons_free (Projet *p)
/**
 * \brief Libère l'ensemble des combinaisons à l'ELU et l'ELS.
 * \param p : la variable projet.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL.
 */
{
  BUGMSG (p, FALSE, gettext ("Paramètre %s incorrect.\n"), "projet")
  
  if (p->combinaisons.elu_equ != NULL)
  {
    g_list_free_full (p->combinaisons.elu_equ, _1990_combinaisons_free_1);
    p->combinaisons.elu_equ = NULL;
  }
  if (p->combinaisons.elu_str != NULL)
  {
    g_list_free_full (p->combinaisons.elu_str, _1990_combinaisons_free_1);
    p->combinaisons.elu_str = NULL;
  }
  if (p->combinaisons.elu_geo != NULL)
  {
    g_list_free_full (p->combinaisons.elu_geo, _1990_combinaisons_free_1);
    p->combinaisons.elu_geo = NULL;
  }
  if (p->combinaisons.elu_fat != NULL)
  {
    g_list_free_full (p->combinaisons.elu_fat, _1990_combinaisons_free_1);
    p->combinaisons.elu_fat = NULL;
  }
  if (p->combinaisons.elu_acc != NULL)
  {
    g_list_free_full (p->combinaisons.elu_acc, _1990_combinaisons_free_1);
    p->combinaisons.elu_acc = NULL;
  }
  if (p->combinaisons.elu_sis != NULL)
  {
    g_list_free_full (p->combinaisons.elu_sis, _1990_combinaisons_free_1);
    p->combinaisons.elu_sis = NULL;
  }
  if (p->combinaisons.els_car != NULL)
  {
    g_list_free_full (p->combinaisons.els_car, _1990_combinaisons_free_1);
    p->combinaisons.els_car = NULL;
  }
  if (p->combinaisons.els_freq != NULL)
  {
    g_list_free_full (p->combinaisons.els_freq, _1990_combinaisons_free_1);
    p->combinaisons.els_freq = NULL;
  }
  if (p->combinaisons.els_perm != NULL)
  {
    g_list_free_full (p->combinaisons.els_perm, _1990_combinaisons_free_1);
    p->combinaisons.els_perm = NULL;
  }
  
#ifdef ENABLE_GTK
  g_object_unref (p->combinaisons.list_el_desc);
#endif
  
  return TRUE;
}


gboolean
_1990_combinaisons_eluequ_equ_seul (Projet *p)
/**
 * \brief Modifie les options de combinaison pour que l'ELU EQU calcule à
 *        l'équilibre seulement.
 * \param p : la variable projet.
 * \return TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL.
 */
{
  BUGMSG (p, FALSE, gettext ("Paramètre %s incorrect.\n"), "projet")
  
  if ((p->combinaisons.flags & 1) != 0)
    p->combinaisons.flags ^= 1;
  
  return TRUE;
}


gboolean
_1990_combinaisons_eluequ_equ_resist (Projet *p)
/**
 * \brief Modifie les options de combinaison pour que l'ELU EQU calcule à
 *        l'équilibre et à la résistance structurelle.
 * \param p : la variable projet.
 * \return TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL.
 */
{
  BUGMSG (p, FALSE, gettext ("Paramètre %s incorrect.\n"), "projet")
  
  if ((p->combinaisons.flags & 1) == 0)
    p->combinaisons.flags++;
  
  return TRUE;
}


gboolean
_1990_combinaisons_elustrgeo_1 (Projet *p)
/**
 * \brief Modifie les options de combinaison pour que l'ELU STR/GEO calcule
 *        selon l'approche 1.
 * \param p : la variable projet.
 * \return TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL.
 */
{
  BUGMSG (p, FALSE, gettext ("Paramètre %s incorrect.\n"), "projet")
  
  p->combinaisons.flags = p->combinaisons.flags - (p->combinaisons.flags & 6);
  p->combinaisons.flags = p->combinaisons.flags + 0;
  
  return TRUE;
}


gboolean
_1990_combinaisons_elustrgeo_2 (Projet *p)
/**
 * \brief Modifie les options de combinaison pour que l'ELU STR/GEO calcule
 *        selon l'approche 2.
 * \param p : la variable projet.
 * \return TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL.
 */
{
  BUGMSG (p, FALSE, gettext ("Paramètre %s incorrect.\n"), "projet")
  
  p->combinaisons.flags = p->combinaisons.flags - (p->combinaisons.flags & 6);
  p->combinaisons.flags = p->combinaisons.flags + 2;
  
  return TRUE;
}


gboolean
_1990_combinaisons_elustrgeo_3 (Projet *p)
/**
 * \brief Modifie les options de combinaison pour que l'ELU STR/GEO calcule
 *        selon l'approche 3.
 * \param p : la variable projet.
 * \return TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL.
 */
{
  BUGMSG (p, FALSE, gettext ("Paramètre %s incorrect.\n"), "projet")
  
  p->combinaisons.flags = p->combinaisons.flags - (p->combinaisons.flags & 6);
  p->combinaisons.flags = p->combinaisons.flags + 4;
  
  return TRUE;
}


gboolean
_1990_combinaisons_elustrgeo_6_10 (Projet *p)
/**
 * \brief Modifie les options de combinaison pour que l'ELU STR/GEO calcule
 *        selon la formule 6.10 de l'EN 1990.
 * \param p : la variable projet.
 * \return TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL.
 */
{
  BUGMSG (p, FALSE, gettext ("Paramètre %s incorrect.\n"), "projet")
  
  p->combinaisons.flags = p->combinaisons.flags - (p->combinaisons.flags & 8);
  p->combinaisons.flags = p->combinaisons.flags + 8;
  
  return TRUE;
}


gboolean
_1990_combinaisons_elustrgeo_6_10ab (Projet *p)
/**
 * \brief Modifie les options de combinaison pour que l'ELU STR/GEO calcule
 *        selon la formule 6.10 (a) et (b) de l'EN 1990.
 * \param p : la variable projet.
 * \return TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL.
 */
{
  BUGMSG (p, FALSE, gettext ("Paramètre %s incorrect.\n"), "projet")
  
  p->combinaisons.flags = p->combinaisons.flags - (p->combinaisons.flags & 8);
  p->combinaisons.flags = p->combinaisons.flags + 0;
  
  return TRUE;
}


gboolean
_1990_combinaisons_eluacc_frequente (Projet *p)
/**
 * \brief Modifie les options de combinaison pour que l'ELU ACC calcule avec
 *        les valeurs fréquentes des actions variables.
 * \param p : la variable projet.
 * \return TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL.
 */
{
  BUGMSG (p, FALSE, gettext ("Paramètre %s incorrect.\n"), "projet")
  
  p->combinaisons.flags = p->combinaisons.flags - (p->combinaisons.flags & 16);
  p->combinaisons.flags = p->combinaisons.flags + 0;
  
  return TRUE;
}


gboolean
_1990_combinaisons_eluacc_quasi_permanente (Projet *p)
/**
 * \brief Modifie les options de combinaison pour que l'ELU ACC calcule avec
 *        les valeurs quasi permanente des actions variables.
 * \param p : la variable projet.
 * \return TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL.
 */
{
  BUGMSG (p, FALSE, gettext ("Paramètre %s incorrect.\n"), "projet")
  
  p->combinaisons.flags = p->combinaisons.flags - (p->combinaisons.flags & 16);
  p->combinaisons.flags = p->combinaisons.flags + 16;
  
  return TRUE;
}


gboolean
_1990_combinaisons_genere (Projet *p)
/**
 * \brief Génère l'ensemble des combinaisons et pondérations du projet.
 * \param p : la variable projet.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL,
 *     - p->niveaux_groupes == NULL,
 *     - _1990_combinaisons_free,
 *     - _1990_action_categorie_bat,
 *     - _1990_combinaisons_genere_or,
 *     - _1990_combinaisons_genere_xor,
 *     - _1990_combinaisons_genere_and,
 *     - _1990_ponderations_genere.
 */
{
  unsigned int i;
  
  BUGMSG (p, FALSE, gettext ("Paramètre %s incorrect.\n"), "projet")
  BUGMSG (p->niveaux_groupes,
          FALSE,
          gettext ("Le projet ne possède pas de niveaux de groupes.\n"))
  
#ifdef ENABLE_GTK
  g_object_ref (p->combinaisons.list_el_desc);
#endif
  BUG (_1990_combinaisons_free (p), FALSE)
  
  // Pour chaque action
  //   Cette boucle permet de générer toutes les combinaisons en prenant en
  //   compte le principe d'action prédominante. Ainsi, à chaque passage de la
  //   boucle, l'action numéro i est considérée comme action prédominante.
  for (i = 0; i < g_list_length (p->actions);i++)
  {
    GList           *list_parcours;
    Action          *action;
    unsigned int     j;
    Action_Categorie categorie;
    
    // On supprime les combinaisons temporaires générées lors du passage de la
    // boucle précédente.
    list_parcours = p->niveaux_groupes;
    do
    {
      Niveau_Groupe *niveau = list_parcours->data;
      
      if (niveau->groupes != NULL)
      {
        GList *list_parcours2 = niveau->groupes;
        
        do
        {
          Groupe *groupe = list_parcours2->data;
          
          while (groupe->tmp_combinaison != NULL)
          {
            g_list_free_full ((GList *) groupe->tmp_combinaison->data, g_free);
            groupe->tmp_combinaison = g_list_delete_link (
                             groupe->tmp_combinaison, groupe->tmp_combinaison);
          }
          list_parcours2 = g_list_next (list_parcours2);
        }
        while (list_parcours2 != NULL);
      }
      list_parcours = g_list_next (list_parcours);
    }
    while (list_parcours != NULL);
    
    // Attribution à l'action numéro i du flags "action prédominante" à 1 afin
    // d'indiquer qu'il s'agit d'une action prédominante et mise à 0 pour les
    // autres actions.
    list_parcours = p->actions;
    for (j = 0; j < i; j++)
    {
      action = list_parcours->data;
      BUG (_1990_action_flags_action_predominante_change (action, 0), FALSE)
      list_parcours = g_list_next (list_parcours);
    }
    action = list_parcours->data;
    categorie = _1990_action_categorie_bat (_1990_action_type_renvoie (action),
                                            p->parametres.norme);
    BUG (categorie != ACTION_INCONNUE, FALSE)
    if (categorie == ACTION_VARIABLE)
      BUG (_1990_action_flags_action_predominante_change (action, 1), FALSE)
    else
      BUG (_1990_action_flags_action_predominante_change (action, 0), FALSE)
    list_parcours = g_list_next (list_parcours);
    for (j = i + 1; j < g_list_length (p->actions); j++)
    {
      action = list_parcours->data;
      BUG (_1990_action_flags_action_predominante_change (action, 0), FALSE)
      list_parcours = g_list_next (list_parcours);
    }
    
  //   Générer des combinaisons de toutes les groupes pour tous les niveaux :
  //   Pour chaque niveau de niveaux_groupes
  //     Pour chaque groupe du niveau en cours
  //       Génération des combinaisons avec l'action prédominante définie
  //       précédemment.
  //     FinPour
  //   FinPour
    list_parcours = p->niveaux_groupes;
    do
    {
      Niveau_Groupe *niveau = list_parcours->data;
      
      if (niveau->groupes != NULL)
      {
        GList *list_parcours2 = niveau->groupes;
        do
        {
          Groupe *groupe = list_parcours2->data;
          switch (groupe->type_combinaison)
          {
            case GROUPE_COMBINAISON_OR :
            {
              BUG (_1990_combinaisons_genere_or (p, niveau, groupe), FALSE)
              break;
            }
            case GROUPE_COMBINAISON_XOR :
            {
              BUG (_1990_combinaisons_genere_xor (p, niveau, groupe), FALSE)
              break;
            }
            case GROUPE_COMBINAISON_AND :
            {
              BUG (_1990_combinaisons_genere_and (p, niveau, groupe), FALSE)
              break;
            }
            default :
            {
              BUG (0, FALSE)
              break;
            }
          }
          list_parcours2 = g_list_next (list_parcours2);
        }
        while (list_parcours2 != NULL);
      }
      list_parcours = g_list_next (list_parcours);
    }
    while (list_parcours != NULL);
    
    // Génération des pondérations (avec les coefficients de sécurité partiels)
    // à partir des combinaisons.
    BUG (_1990_ponderations_genere (p), FALSE)
  }
  // FinPour
  
  return TRUE;
}
