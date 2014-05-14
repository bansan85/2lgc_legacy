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

#include <algorithm>
#include <memory>

#include "common_projet.hpp"
#include "common_erreurs.hpp"
#include "1990_action.hpp"
#include "1990_combinaisons.hpp"
#include "1990_groupe.hpp"

#ifdef ENABLE_GTK
#include "1990_gtk_groupes.hpp"
#include "common_gtk.hpp"
#endif

/**
 * \brief Initialise la liste des groupes.
 * \param p : variable projet.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL.
 */
gboolean
_1990_groupe_init (Projet *p)
{
  BUGPARAM (p, "%p", p, FALSE)
  
  p->niveaux_groupes.clear ();
  
  return TRUE;
}


/**
 * \brief Ajoute un niveau au projet en lui attribuant le numéro suivant le
 *        dernier niveau existant.
 * \param p : variable projet.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL,
 *     - erreur d'allocation mémoire.
 */
gboolean
_1990_groupe_ajout_niveau (Projet *p)
{
  Niveau_Groupe *niveau_nouveau;
  
  BUGPARAM (p, "%p", p, FALSE)
  niveau_nouveau = new Niveau_Groupe;
  
  niveau_nouveau->groupes.clear ();
  
  p->niveaux_groupes.push_back (niveau_nouveau);
  
#ifdef ENABLE_GTK
  // Mise à jour des limites du composant spin_button
  if (UI_GRO.builder != NULL)
  {
    gtk_tree_store_clear (UI_GRO.tree_store_etat);
    gtk_spin_button_set_range (GTK_SPIN_BUTTON (UI_GRO.spin_button_niveau),
                               0,
                               p->niveaux_groupes.size () - 1);
    
    // La modification de la valeur du composant spin_button execute
    // automatiquement #_1990_gtk_spin_button_niveau_change qui met à jour
    // l'interface graphique
    gtk_spin_button_set_value (GTK_SPIN_BUTTON (UI_GRO.spin_button_niveau),
                               p->niveaux_groupes.size () - 1);
  }
#endif
  
  return TRUE;
}


/**
 * \brief Ajoute un groupe au niveau choisi avec le type de combinaison
 *        spécifié.
 * \param p : la variable projet,
 * \param niveau_groupe : le niveau où le groupe doit être inséré,
 * \param type_combinaison : combinaison du nouveau groupe.
 * \param nom : nom du groupe.
 * \return
 *   Succès : Pointeur vers le nouveau groupe.\n
 *   Échec : NULL :
 *     - p == NULL,
 *     - niveau_groupe == NULL,
 *     - erreur d'allocation mémoire,
 */
Groupe *
_1990_groupe_ajout_groupe (Projet                 *p,
                           Niveau_Groupe          *niveau_groupe,
                           Type_Groupe_Combinaison type_combinaison,
                           const char             *nom)
{
  Groupe *groupe_nouveau;
  
  BUGPARAM (p, "%p", p, NULL)
  BUGPARAM (niveau_groupe, "%p", niveau_groupe, NULL)
  groupe_nouveau = new Groupe;
  
  BUGCRIT (groupe_nouveau->nom = g_strdup_printf ("%s", nom),
           NULL,
           (gettext ("Erreur d'allocation mémoire.\n"));
             delete groupe_nouveau; )
  groupe_nouveau->type_combinaison = type_combinaison;
#ifdef ENABLE_GTK
  groupe_nouveau->Iter_expand = 1;
#endif
  
  groupe_nouveau->tmp_combinaison.clear ();
  
  niveau_groupe->groupes.push_back (groupe_nouveau);
  
#ifdef ENABLE_GTK
  if ((UI_GRO.builder != NULL) &&
      (GTK_COMMON_SPINBUTTON_AS_INT (GTK_SPIN_BUTTON (
                                                 UI_GRO.spin_button_niveau)) ==
         std::distance (p->niveaux_groupes.begin (),
                        std::find (p->niveaux_groupes.begin (),
                                   p->niveaux_groupes.end (),
                                   niveau_groupe))))
  {
    GtkTreePath *path;
    
    // Et on met à jour l'affichage
    gtk_tree_store_append (UI_GRO.tree_store_etat,
                           &groupe_nouveau->Iter_groupe,
                           NULL);
    gtk_tree_store_set (UI_GRO.tree_store_etat,
                        &groupe_nouveau->Iter_groupe,
                        0, groupe_nouveau,
                        -1);
    path = gtk_tree_model_get_path (GTK_TREE_MODEL (UI_GRO.tree_store_etat),
                                    &groupe_nouveau->Iter_groupe);
    gtk_tree_view_set_cursor (GTK_TREE_VIEW (UI_GRO.tree_view_etat),
                              path,
                              gtk_tree_view_get_column (GTK_TREE_VIEW (
                                                    UI_GRO.tree_view_etat),
                                                        1),
                              TRUE);
    gtk_tree_path_free (path);
  }
#endif
  
  return groupe_nouveau;
}


/**
 * \brief Ajoute l'élément 'element_add' au groupe 'groupe' du niveau
 *        'niveau_groupe'. L'élément appartient au niveau directement inférieur
 *        à 'niveau'. Ainsi si 'niveau_groupe' est le niveau 0, 'element_add'
 *        fait référence à une action de projet->actions. Si 'niveau_groupe'
 *        est supérieur à 0, 'element_add' fait référence à une groupe du
 *        niveau 'niveau'-1.
 *        Le dernier niveau ne doit contenir qu'un seul groupe.
 * \param p : la variable projet,
 * \param niveau_groupe : le niveau où le groupe est,
 * \param groupe : groupe où ajouter l'élément,
 * \param element_add : l'élément à ajouter.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL,
 *     - p->niveaux_groupes == NULL,
 *     - niveau_groupe == NULL,
 *     - groupe == NULL,
 *     - element_add == NULL,
 *     - si l'élément num_element, est déjà présentant dans le groupe groupe_n,
 *     - erreur d'allocation mémoire.
 */
gboolean
_1990_groupe_ajout_element (Projet        *p,
                            Niveau_Groupe *niveau_groupe,
                            Groupe        *groupe,
                            void          *element_add)
{
  BUGPARAM (p, "%p", p, FALSE)
  INFO (!p->niveaux_groupes.empty (),
        FALSE,
        (gettext ("Le projet ne possède pas de niveaux de groupes permettant de regrouper plusieurs groupes d'actions.\n")); )
  BUGPARAM (niveau_groupe, "%p", niveau_groupe, FALSE)
  BUGPARAM (groupe, "%p", groupe, FALSE)
  BUGPARAM (element_add, "%p", element_add, FALSE)
  
  // On ajoute le nouvel élément au groupe.
  if (!groupe->elements.empty ())
  {
    std::list <void *>::iterator it;
    
    it = groupe->elements.begin ();
    do
    {
      if (*it == element_add)
      {
        printf (gettext ("L'élément est déjà présent dans le groupe.\n"));
        
        return FALSE;
      }
      
      ++it;
    }
    while (it != groupe->elements.end ());
  }
  
  groupe->elements.push_back (element_add);
  
  #ifdef ENABLE_GTK
  if ((UI_GRO.builder != NULL) &&
      (GTK_COMMON_SPINBUTTON_AS_INT (GTK_SPIN_BUTTON (
                                                 UI_GRO.spin_button_niveau)) ==
         std::distance (p->niveaux_groupes.begin (),
                        std::find (p->niveaux_groupes.begin (),
                                   p->niveaux_groupes.end (),
                                   niveau_groupe))))
  {
    GtkTreeIter  iter;
    void        *data;
    GtkTreePath *path; // Pour développer une ligne du TreeView
    Groupe      *groupe2 = (Groupe *) element_add;
    
    // On supprime l'élément à ajouter dans le groupe de la liste des éléments
    // disponibles.
    if (gtk_tree_model_get_iter_first (GTK_TREE_MODEL (
                                                      UI_GRO.tree_store_dispo),
                                       &iter) == TRUE)
    {
      do
      {
        gtk_tree_model_get (GTK_TREE_MODEL (UI_GRO.tree_store_dispo),
                            &iter,
                            0, &data,
                            -1);
        if (data == element_add)
        {
          gtk_tree_store_remove (UI_GRO.tree_store_dispo, &iter);
          break;
        }
      }
      while (gtk_tree_model_iter_next (GTK_TREE_MODEL (
                                    UI_GRO.tree_store_dispo), &iter) != FALSE);
    }
    
    // On ajoute la ligne dans l'interface graphique
    gtk_tree_store_append (UI_GRO.tree_store_etat,
                           &groupe2->Iter_groupe,
                           &groupe->Iter_groupe);
    gtk_tree_store_set (UI_GRO.tree_store_etat,
                        &groupe2->Iter_groupe,
                        0, element_add,
                        -1);
    
    // On développe la ligne dans le groupe où l'élément vient d'être mis.
    groupe->Iter_expand = 1;
    path = gtk_tree_model_get_path (GTK_TREE_MODEL (UI_GRO.tree_store_etat),
                                    &groupe->Iter_groupe);
    gtk_tree_view_expand_row (UI_GRO.tree_view_etat, path, FALSE);
    gtk_tree_path_free (path);
  }
  #endif
  
  return TRUE;
}


/**
 * \brief Modifie le type d'une combinaison (OR, XOR ou AND).
 * \param groupe : le groupe à modifier,
 * \param type_combinaison : le nouveau type de combinaison.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - groupe == NULL,
 *     - type_combinaison inconnu.
 */
gboolean
_1990_groupe_modifie_combinaison (Groupe                 *groupe,
                                  Type_Groupe_Combinaison type_combinaison)
{
  BUGPARAM (groupe, "%p", groupe, FALSE)
  
  switch (type_combinaison)
  {
    case GROUPE_COMBINAISON_AND :
    case GROUPE_COMBINAISON_OR :
    case GROUPE_COMBINAISON_XOR :
    {
      groupe->type_combinaison = type_combinaison;
      break;
    }
    default :
    {
      FAILPARAM (type_combinaison, "%d", FALSE)
      break;
    }
  }
  
  return TRUE;
}


/**
 * \brief Modifie le nom d'un groupe. L'ancienne valeur est libérée.
 * \param groupe_niveau : niveau où se trouve le groupe,
 * \param groupe : groupe à modifier,
 * \param nom : le nouveau nom,
 * \param p : la variable projet.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - groupe == NULL,
 *     - erreur d'allocation mémoire.
 */
gboolean
_1990_groupe_modifie_nom (Niveau_Groupe *groupe_niveau,
                          Groupe        *groupe,
                          const char    *nom,
                          Projet        *p)
{
  char *tmp;
  
  BUGPARAM (groupe_niveau, "%p", groupe_niveau, FALSE, )
  BUGPARAM (groupe, "%p", groupe, FALSE)
  BUGPARAM (nom, "%p", groupe, FALSE)
  BUGPARAM (p, "%p", p, FALSE)
  
  tmp = groupe->nom;
  BUGCRIT (groupe->nom = g_strdup_printf ("%s", nom),
           FALSE,
           (gettext ("Erreur d'allocation mémoire.\n"));
             groupe->nom = tmp; )
  free (tmp);
  
#ifdef ENABLE_GTK
  if (UI_GRO.builder != NULL)
  {
    // Si on est dans le niveau en cours, il n'y a que le treeview etat à
    // modifier.
    if (GTK_COMMON_SPINBUTTON_AS_INT (GTK_SPIN_BUTTON (
                                                 UI_GRO.spin_button_niveau)) ==
          std::distance (p->niveaux_groupes.begin (),
                         std::find (p->niveaux_groupes.begin (),
                                    p->niveaux_groupes.end (),
                                    groupe_niveau)))
    {
      gtk_widget_queue_resize (GTK_WIDGET (UI_GRO.tree_view_etat));
    }
    // Si on se trouve dans le niveau supérieur, il y a la ligne de l'élément
    // du groupe qui contient le groupe en cours et, à défaut, la ligne dans la
    // liste des éléments disponibles.
    else if (GTK_COMMON_SPINBUTTON_AS_INT (GTK_SPIN_BUTTON 
                                                (UI_GRO.spin_button_niveau)) ==
          std::distance (p->niveaux_groupes.begin (),
                         std::find (p->niveaux_groupes.begin (),
                                    p->niveaux_groupes.end (),
                                    groupe_niveau)) + 1)
    {
      gtk_widget_queue_resize (GTK_WIDGET (UI_GRO.tree_view_etat));
      gtk_widget_queue_resize (GTK_WIDGET (UI_GRO.tree_view_dispo));
    }
  }
#endif
  
  return TRUE;
}


/**
 * \brief Affiche tous les groupes y compris les combinaisons temporaires de
 *        tous les niveaux. La valeur entre parenthèses 0 ou 1 représente si
 *        l'action est prédominante (1) ou pas (0).
 * \param p : la variable projet.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL.
 */
gboolean
_1990_groupe_affiche_tout (Projet *p)
{
  std::list<Niveau_Groupe*>::iterator it;
  
  uint8_t nniveau = 0;
  
  BUGPARAM (p, "%p", p, FALSE)
  
  it = p->niveaux_groupes.begin ();
  while (it != p->niveaux_groupes.end ())
  {
    Niveau_Groupe *niveau = *it;
    
    std::list<Groupe*>::iterator it2 = niveau->groupes.begin ();
    
    printf (gettext ("niveau : %d\n"), nniveau);
    
    while (it2 != niveau->groupes.end ())
    {
      Groupe *groupe = *it2;
      std::list <void *>::iterator it5 = groupe->elements.begin ();
      
      std::list <std::list <Combinaison *> *>::iterator it3;
      
      printf (gettext ("\tgroupe : '%s', combinaison : "), groupe->nom);
      switch (groupe->type_combinaison)
      {
        case GROUPE_COMBINAISON_OR :
        {
          printf ("OR\n");
          break;
        }
        case GROUPE_COMBINAISON_XOR :
        {
          printf ("XOR\n");
          break;
        }
        case GROUPE_COMBINAISON_AND :
        {
          printf ("AND\n");
          break;
        }
        default :
        {
          FAILCRIT (FALSE,
                    (gettext ("Combinaison %d inconnue"),
                              groupe->type_combinaison); )
          break;
        }
      }
      if (p->niveaux_groupes.begin () == it)
      {
        printf (gettext ("\t\tActions contenus dans ce groupe : "));
      }
      else
      {
        printf (gettext ("\t\tGroupes du niveau %d contenus dans ce groupe : "),
                nniveau - 1);
      }
      
      while (it5 != groupe->elements.end ())
      {
        if (p->niveaux_groupes.begin () == it)
        {
          Action *action = static_cast <Action *> (*it5);
          
          printf("'%s' ", _1990_action_nom_renvoie (action));
        }
        else
        {
          Groupe *groupe_n_1 = static_cast <Groupe *> (*it5);
          
          printf("'%s' ", groupe_n_1->nom);
        }
        
        ++it5;
      }
      printf ("\n");
      printf (gettext ("\t\tCombinaisons :\n"));
      
      it3 = groupe->tmp_combinaison.begin ();
      
      while (it3 != groupe->tmp_combinaison.end ())
      {
        std::list <Combinaison *> *combinaison = *it3;
        std::list <Combinaison *>::iterator it4 = combinaison->begin ();
        
        printf ("\t\t\t");
        
        while (it4 != combinaison->end ())
        {
          Combinaison *comb_element = *it4;
          Action      *action = (Action *) comb_element->action;
          
          printf("'%s'(%u) ",
                 _1990_action_nom_renvoie (action),
                 comb_element->flags);
          
          ++it4;
        }
        
        printf ("\n");
        ++it3;
      }
      ++it2;
    }
    nniveau++;
    ++it;
  }
  
  return TRUE;
}


/**
 * \brief Libère l'élément désigné par les paramètres.
 * \param p : variable projet,
 * \param niveau_groupe : le niveau contenant l'élément,
 * \param groupe : le groupe contenant l'élément,
 * \param element : l'élément à retirer.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL,
 *     - niveau_groupe == NULL,
 *     - element == NULL,
 *     - groupe == NULL.
 */
gboolean
_1990_groupe_retire_element (Projet        *p,
                             Niveau_Groupe *niveau_groupe,
                             Groupe        *groupe,
                             void          *element)
{
#ifdef ENABLE_GTK
  Groupe  *groupe2 = (Groupe *) element;
#endif
  
  BUGPARAM (p, "%p", p, FALSE)
  BUGPARAM (niveau_groupe, "%p", niveau_groupe, FALSE)
  BUGPARAM (groupe, "%p", groupe, FALSE)
  BUGPARAM (element, "%p", element, FALSE)
  
#ifdef ENABLE_GTK
  // On sélectionne dans la liste des groupes la ligne suivante. Et si elle
  // n'existe pas, on sélectionne la ligne précédente.
  // Si c'est le bon niveau qui est affiché
  if ((UI_GRO.builder != NULL) &&
      (GTK_COMMON_SPINBUTTON_AS_INT (GTK_SPIN_BUTTON (
                                                 UI_GRO.spin_button_niveau)) ==
         std::distance (p->niveaux_groupes.begin (),
                        std::find (p->niveaux_groupes.begin (),
                                   p->niveaux_groupes.end (),
                                   niveau_groupe))))
  {
    if (gtk_tree_selection_iter_is_selected (UI_GRO.tree_select_etat,
                                             &groupe2->Iter_groupe))
    {
      GtkTreePath *path;
      
      // On décale la sélection
      path = gtk_tree_model_get_path (GTK_TREE_MODEL (UI_GRO.tree_store_etat),
                                      &groupe2->Iter_groupe);
      gtk_tree_path_next (path);
      gtk_tree_selection_select_path (UI_GRO.tree_select_etat,
                                      path);
      if (!gtk_tree_selection_path_is_selected (UI_GRO.tree_select_etat, path))
      {
        gtk_tree_path_prev (path);
        if (gtk_tree_path_prev (path))
        {
          gtk_tree_selection_select_path (UI_GRO.tree_select_etat, path);
        }
      }
      gtk_tree_path_free (path);
    }
    gtk_tree_store_remove (UI_GRO.tree_store_etat, &groupe2->Iter_groupe);
  }
#endif
  
  groupe->elements.remove (element);
  
  return TRUE;
}


/**
 * \brief Libère le niveau ainsi que tous les niveaux supérieurs.
 * \param p : la variable projet,
 * \param niveau_groupe : le niveau à libérer,
 * \param accept_vide : si FALSE et que tous les niveaux sont libérés alors un
 *                      niveau vide est créé.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL.
 */
gboolean
_1990_groupe_free_niveau (Projet        *p,
                          Niveau_Groupe *niveau_groupe,
                          gboolean       accept_vide)
{
  std::list <Niveau_Groupe *>::iterator it;
  
  BUGPARAM (p, "%p", p, FALSE)
  
  it = std::find (p->niveaux_groupes.begin (),
                  p->niveaux_groupes.end (),
                  niveau_groupe);
  
  
  while (it != p->niveaux_groupes.end ())
  {
    niveau_groupe = *it;
    
    std::list <Groupe *>::iterator it2 = niveau_groupe->groupes.begin ();
    // Il peut être possible d'utiliser la fonction #_1990_groupe_free_groupe
    // mais cette dernier analyse également le niveau supérieur pour supprimer
    // les références devenues obsolète, ce qui est inutile ici puisque tous
    // les niveaux supérieurs vont également être supprimés.
    while (it2 != niveau_groupe->groupes.end ())
    {
      Groupe *groupe = *it2;
      
      free (groupe->nom);
      
      // On libère tous les éléments contenus dans le groupe
      groupe->elements.clear ();
      
      // On libère toutes les combinaisons temporaires
      for_each (groupe->tmp_combinaison.begin (),
                groupe->tmp_combinaison.end (),
                _1990_combinaisons_free_1);
      
      delete groupe;
      
      // Et enfin, on supprime l'élément courant.
      ++it2;
    }
    
    it = p->niveaux_groupes.erase (it);
    
    delete niveau_groupe;
  }
  
  // On oblige la liste des niveaux à posséder au moins un niveau vide.
  if ((p->niveaux_groupes.empty ()) && (accept_vide == FALSE))
  {
    BUG (_1990_groupe_ajout_niveau (p), FALSE)
#ifdef ENABLE_GTK
    if (UI_GRO.builder != NULL)
    {
      gtk_tree_store_clear (UI_GRO.tree_store_etat);
      BUG (_1990_gtk_groupes_affiche_niveau (p, 0), FALSE)
    }
#endif
  }
  
#ifdef ENABLE_GTK
  if (UI_GRO.builder != NULL)
  {
    // On réajuste les limites du spin_button
    gtk_spin_button_set_range (GTK_SPIN_BUTTON (UI_GRO.spin_button_niveau),
                               0,
                               p->niveaux_groupes.size () - 1);
  }
#endif
  
  return TRUE;
}


/**
 * \brief Libère les combinaisons temporaires d'un groupe.
 * \param liste : liste contenant les combinaisons à libérer.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - groupe == NULL,
 */
gboolean
_1990_groupe_free_combinaisons (
  std::list <std::list <Combinaison *> *> *liste)
{
  std::list <std::list <Combinaison *> *>::iterator it;
  
  BUGPARAM (liste, "%p", liste, FALSE)
  
  it = liste->begin ();
  // On libère toutes les combinaisons temporaires.
  while (it != liste->end ())
  {
    std::list <Combinaison *> *combinaison = *it;
    
    for_each (combinaison->begin (),
              combinaison->end (),
              std::default_delete <Combinaison> ());
    delete combinaison;
    
    it = liste->erase (it);
  }

  return TRUE;
}


/**
 * \brief Libère le groupe demandé. La suppression d'un groupe entraine sa
 *        suppression dans le niveau supérieur (si existant) lorsqu'il est
 *        présent dans une combinaison.
 * \param p : la variable projet,
 * \param niveau_groupe : niveau contenant le groupe,
 * \param groupe : groupe à libérer.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL,
 */
gboolean
_1990_groupe_free_groupe (Projet        *p,
                          Niveau_Groupe *niveau_groupe,
                          Groupe        *groupe)
{
  std::list <Niveau_Groupe *>::iterator it;
  
  BUGPARAM (p, "%p", p, FALSE)
  
#ifdef ENABLE_GTK
  if ((UI_GRO.builder != NULL) &&
      (GTK_COMMON_SPINBUTTON_AS_INT (GTK_SPIN_BUTTON (
                                                 UI_GRO.spin_button_niveau)) ==
         std::distance (p->niveaux_groupes.begin (),
                        std::find (p->niveaux_groupes.begin (),
                                   p->niveaux_groupes.end (),
                                   niveau_groupe))))
  {
    std::list <void *>::iterator it2;
    GtkTreePath *path;
    
    // On sélectionne dans la liste des groupes la ligne suivante. Et si elle
    // n'existe pas, on sélectionne la ligne précédente.
    path = gtk_tree_model_get_path (GTK_TREE_MODEL (UI_GRO.tree_store_etat),
                                    &groupe->Iter_groupe);
    gtk_tree_path_next (path);
    gtk_tree_selection_select_path (UI_GRO.tree_select_etat, path);
    if (!gtk_tree_selection_path_is_selected (UI_GRO.tree_select_etat, path))
    {
      gtk_tree_path_prev (path);
      if (gtk_tree_path_prev (path))
      {
        gtk_tree_selection_select_path (UI_GRO.tree_select_etat, path);
      }
    }
    gtk_tree_path_free (path);
    
    gtk_tree_selection_unselect_all (UI_GRO.tree_select_dispo);
    
    // On supprimer la ligne
    gtk_tree_store_remove (UI_GRO.tree_store_etat, &groupe->Iter_groupe);
    
    // On ajoute tous les éléments associés au groupe dans la liste des
    // éléments disponibles.
    it2 = groupe->elements.begin ();
    while (it2 != groupe->elements.end ())
    {
      BUG (_1990_gtk_insert_dispo (p, *it2, niveau_groupe),
           FALSE)
      
      ++it2;
    }
  }
#endif
  
  free (groupe->nom);
  
  // On libère tous les éléments contenus dans le groupe.
  groupe->elements.clear ();
  
  // On libère toutes les combinaisons temporaires.
  _1990_groupe_free_combinaisons (&groupe->tmp_combinaison);
  
  // Et enfin, on supprime l'élément courant.
  niveau_groupe->groupes.erase (std::find (niveau_groupe->groupes.begin (),
                                           niveau_groupe->groupes.end (),
                                           groupe));
  delete groupe;
  
  it = std::find (p->niveaux_groupes.begin (),
                  p->niveaux_groupes.end (),
                  niveau_groupe);
  ++it;
  
  // On passe au niveau suivant (s'il existe).
  if (it != p->niveaux_groupes.end ())
  {
    std::list <Groupe *>::iterator it2;
    
    niveau_groupe = *it;
    it2 = niveau_groupe->groupes.begin ();
    
    while (it2 != niveau_groupe->groupes.end ())
    {
      std::list <void *>::iterator it3 = groupe->elements.begin ();
      
      // On parcours tous les groupes pour vérifier si l'un possède l'élément
      // qui a été supprimé.
      // On ne s'arrête pas volontairement au premier élément qu'on trouve.
      // Il est possible que quelqu'un trouve utile de pouvoir insérer un
      // même élément dans plusieurs groupes.
      groupe = *it2;
      while (it3 != groupe->elements.end ())
      {
        if (*it3 == groupe)
        {
          groupe->elements.remove (*it3);
        }
        
        ++it3;
      }
      
      ++it2;
    }
  }
  
  return TRUE;
}


/**
 * \brief Libère l'ensemble des groupes et niveaux.
 * \param p : la variable projet.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL,
 *     - #_1990_groupe_free_niveau.
 */
gboolean
_1990_groupe_free (Projet *p)
{
  BUGPARAM (p, "%p", p, FALSE)
  
  if (!p->niveaux_groupes.empty ())
  {
    BUG (_1990_groupe_free_niveau (p,
                                   *p->niveaux_groupes.begin (),
                                   TRUE),
         FALSE)
  }
  
  return TRUE;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
