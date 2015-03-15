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

#include <algorithm>
#include <locale>

#include "common_projet.hpp"
#include "common_math.hpp"
#ifdef ENABLE_GTK
#include "common_gtk.hpp"
#endif
#include "common_erreurs.hpp"
#include "common_selection.hpp"
#include "1992_1_1_barres.hpp"
#include "1992_1_1_materiaux.hpp"
#include "1993_1_1_materiaux.hpp"
#include "EF_calculs.hpp"
#include "EF_materiaux.hpp"


/**
 * \brief Initialise la liste des matériaux.
 * \param p : la variable projet.
 * \return
 *   Succès : true.
 *   Échec : false :
 *     - p == NULL.
 */
bool
EF_materiaux_init (Projet *p)
{
  BUGPARAM (p, "%p", p, false)
  
  p->modele.materiaux.clear ();
  
#ifdef ENABLE_GTK
  UI_MAT.liste_materiaux = gtk_list_store_new (1, G_TYPE_STRING);
#endif
  
  return true;
}


/**
 * \brief Insère un materiau en triant la liste en fonction du nom. Procédure
 *        commune à tous les matériaux.
 * \param p : la variable projet,
 * \param materiau : le matériau à insérer.
 * \return
 *   Succès : true.
 *   Échec : false :
 *     - p == NULL,
 *     - materiau == NULL.
 */
bool
EF_materiaux_insert (Projet      *p,
                     EF_Materiau *materiau)
{
  std::list <EF_Materiau *>::iterator it;
  EF_Materiau *materiau_tmp;
  
  BUGPARAM (p, "%p", p, false)
  BUGPARAM (materiau, "%p", materiau, false)
  
  it = p->modele.materiaux.begin ();
  while (it != p->modele.materiaux.end ())
  {
    materiau_tmp = *it;
    
    if (materiau->nom.compare (materiau_tmp->nom) < 0)
    {
      break;
    }
    
    ++it;
  }
  if (it == p->modele.materiaux.end ())
  {
    p->modele.materiaux.push_back (materiau);
#ifdef ENABLE_GTK
    gtk_list_store_append (UI_MAT.liste_materiaux,
                           &materiau->Iter_liste);
    if (UI_MAT.builder != NULL)
    {
      gtk_tree_store_append (UI_MAT.materiaux,
                             &materiau->Iter_fenetre,
                             NULL);
    }
#endif
  }
  else
  {
    p->modele.materiaux.insert (it, materiau);
#ifdef ENABLE_GTK
    gtk_list_store_insert_before (UI_MAT.liste_materiaux,
                                  &materiau->Iter_liste,
                                  &materiau_tmp->Iter_liste);
    if (UI_MAT.builder != NULL)
    {
      gtk_tree_store_insert_before (UI_MAT.materiaux,
                                    &materiau->Iter_fenetre,
                                    NULL,
                                    &materiau_tmp->Iter_fenetre);
    }
#endif
  }
  
#ifdef ENABLE_GTK
  gtk_list_store_set (UI_MAT.liste_materiaux,
                      &materiau->Iter_liste,
                      0, materiau->nom.c_str (),
                      -1);
  if (UI_MAT.builder != NULL)
  {
    gtk_tree_store_set (UI_MAT.materiaux,
                        &materiau->Iter_fenetre,
                        0, materiau,
                        -1);
  }
#endif
  
  return true;
}


/**
 * \brief Repositionne un matériau après un renommage. Procédure commune à
 *        toutes les matériaux.
 * \param p : la variable projet,
 * \param materiau : le matériau à repositionner.
 * \return
 *   Succès : true.
 *   Échec : false :
 *     - p == NULL,
 *     - materiau == NULL.
 */
bool
EF_materiaux_repositionne (Projet      *p,
                           EF_Materiau *materiau)
{
  std::list <EF_Materiau *>::iterator it;
  
  BUGPARAM (p, "%p", p, false)
  BUGPARAM (materiau, "%p", materiau, false)
  
  // On réinsère le matériau au bon endroit
  p->modele.materiaux.remove (materiau);
  it = p->modele.materiaux.begin ();
  while (it != p->modele.materiaux.end ())
  {
    EF_Materiau *materiau_parcours = *it;
    
    if (materiau->nom.compare (materiau_parcours->nom) < 0)
    {
      p->modele.materiaux.insert (it, materiau);
      
#ifdef ENABLE_GTK
      gtk_list_store_move_before (UI_MAT.liste_materiaux,
                                  &materiau->Iter_liste,
                                  &materiau_parcours->Iter_liste);
      if (UI_MAT.builder != NULL)
      {
        gtk_tree_store_move_before (UI_MAT.materiaux,
                                    &materiau->Iter_fenetre,
                                    &materiau_parcours->Iter_fenetre);
      }
#endif
      break;
    }
    
    ++it;
  }
  if (it == p->modele.materiaux.end ())
  {
    p->modele.materiaux.push_back (materiau);
    
#ifdef ENABLE_GTK
    gtk_list_store_move_before (UI_MAT.liste_materiaux,
                                &materiau->Iter_liste,
                                NULL);
    if (UI_MAT.builder != NULL)
    {
      gtk_tree_store_move_before (UI_MAT.materiaux,
                                  &materiau->Iter_fenetre,
                                  NULL);
    }
#endif
  }
  
#ifdef ENABLE_GTK
  switch (materiau->type)
  {
    case MATERIAU_BETON :
    {
      if ((UI_BET.builder != NULL) && (UI_BET.materiau == materiau))
      {
        gtk_text_buffer_set_text (gtk_text_view_get_buffer (GTK_TEXT_VIEW (
                                        gtk_builder_get_object (UI_BET.builder,
                                         "_1992_1_1_materiaux_textview_nom"))),
                                  materiau->nom.c_str (),
                                  -1);
      }
      break;
    }
    case MATERIAU_ACIER :
    {
      if ((UI_ACI.builder != NULL) && (UI_ACI.materiau == materiau))
      {
        gtk_text_buffer_set_text (gtk_text_view_get_buffer (GTK_TEXT_VIEW (
                                        gtk_builder_get_object (UI_ACI.builder,
                                         "_1993_1_1_materiaux_textview_nom"))),
                                  materiau->nom.c_str (),
                                  -1);
      }
      break;
    }
    default :
    {
      FAILCRIT (false,
                (gettext ("Le type de matériau %d n'existe pas.\n"),
                          materiau->type); )
      break;
    }
  }
  if (UI_BAR.builder != NULL)
  {
    gtk_widget_queue_resize (GTK_WIDGET (gtk_builder_get_object (
                                       UI_BAR.builder, "EF_barres_treeview")));
  }
  gtk_list_store_set (UI_MAT.liste_materiaux,
                      &materiau->Iter_liste,
                      0, materiau->nom.c_str (),
                      -1);
#endif
  
  return true;
}


/**
 * \brief Renvoie le matériau en fonction de son nom.
 * \param p : la variable projet,
 * \param nom : le nom du matériau,
 * \param critique : utilise BUG si true, return sinon.
 * \return
 *   Succès : pointeur vers le matériau en béton.\n
 *   Échec : NULL :
 *     - p == NULL,
 *     - materiau introuvable.
 */
EF_Materiau *
EF_materiaux_cherche_nom (Projet      *p,
                          std::string *nom,
                          bool         critique)
{
  std::list <EF_Materiau *>::iterator it;
  
  BUGPARAM (p, "%p", p, NULL)
  BUGPARAM (nom, "%p", nom, NULL)
  
  it = p->modele.materiaux.begin ();
  while (it != p->modele.materiaux.end ())
  {
    EF_Materiau *materiau = *it;
    
    if (materiau->nom.compare (*nom) == 0)
    {
      return materiau;
    }
    
    ++it;
  }
  
  if (critique)
  {
    FAILCRIT (NULL,
              (gettext ("Matériau '%s' introuvable.\n"),
                        nom->c_str ()); )
  }
  else
  {
    return NULL;
  }
}


/**
 * \brief Renvoie la description d'un matériau sous forme d'un texte. Il
 *        convient de libérer le texte renvoyée par la fonction free.
 * \param materiau : matériau à décrire.
 * \return
 *   Succès : Résultat.\n
 *   Échec : NULL :
 *     - materiau == NULL,
 *     - erreur d'allocation mémoire.
 */
std::string
EF_materiaux_get_description (EF_Materiau *materiau)
{
  switch (materiau->type)
  {
    case MATERIAU_BETON :
      return _1992_1_1_materiaux_get_description (materiau);
    case MATERIAU_ACIER :
      return _1993_1_1_materiaux_get_description (materiau);
    default :
    {
      FAILCRIT (NULL,
                (gettext ("Le type de matériau %d n'existe pas.\n"),
                          materiau->type); )
      break;
    }
  }
}


/**
 * \brief Renvoie le module d'Young du matériau.
 * \param materiau : le matériau à analyser.
 * \return
 *   Succès : le module d'Young.\n
 *   Échec : NAN :
 *     - materiau == NULL,
 *     - materiau inconnu.
 */
Flottant
EF_materiaux_E (EF_Materiau *materiau)
{
  BUGPARAM (materiau, "%p", materiau, m_f (NAN, FLOTTANT_ORDINATEUR))
  
  switch (materiau->type)
  {
    case MATERIAU_BETON :
    {
      Materiau_Beton *data_beton = (Materiau_Beton *) materiau->data;
      
      return data_beton->ecm;
    }
    case MATERIAU_ACIER :
    {
      Materiau_Acier *data_acier = (Materiau_Acier *) materiau->data;
      
      return data_acier->e;
    }
    default :
    {
      FAILCRIT (m_f (NAN, FLOTTANT_ORDINATEUR),
                (gettext ("Matériau %d inconnu.\n"), materiau->type); )
      break;
    }
  }
}


/**
 * \brief Renvoie le module de cisaillement du matériau.
 * \param materiau : le matériau à analyser,
 * \param nu_null : true si on force le coefficient de poisson à 0,
 *                  false si on prend la valeur définie dans le matériau.
 * \return
 *   Succès : le module de cisaillement.\n
 *   Échec : NAN :
 *     - materiau == NULL,
 *     - materiau inconnu.
 */
Flottant
EF_materiaux_G (EF_Materiau *materiau,
                bool         nu_null)
{
  BUGPARAM (materiau, "%p", materiau, m_f (NAN, FLOTTANT_ORDINATEUR))
  
  switch (materiau->type)
  {
    case MATERIAU_BETON :
    {
      Materiau_Beton *data_beton = (Materiau_Beton *) materiau->data;
      
      if (nu_null)
      {
        return m_f (m_g (data_beton->ecm) / 2., FLOTTANT_ORDINATEUR);
      }
      else
      {
        return m_f (m_g (data_beton->ecm) / (2. * (1. + m_g (data_beton->nu))),
                    FLOTTANT_ORDINATEUR);
      }
    }
    case MATERIAU_ACIER :
    {
      Materiau_Acier *data_acier = (Materiau_Acier *) materiau->data;
      
      INFO (!nu_null,
            m_f (NAN, FLOTTANT_ORDINATEUR),
            (gettext ("Seul le matériau béton supporte nu à 0.\n")); )
      return m_f (m_g (data_acier->e) / (2. * (1. + m_g (data_acier->nu))),
                  FLOTTANT_ORDINATEUR);
    }
    default :
    {
      FAILCRIT (m_f (NAN, FLOTTANT_ORDINATEUR),
                (gettext ("Matériau %d inconnu.\n"), materiau->type); )
      break;
    }
  }
}


/**
 * \brief Fonction permettant de libérer un matériau.
 * \param materiau : matériau à libérer.
 * \return : Rien.
 *   Échec : 
 *       materiau == NULL,
 */
void
EF_materiaux_free_un (EF_Materiau *materiau)
{
  BUGPARAM (materiau, "%p", materiau, )
  
  switch (materiau->type)
  {
    case MATERIAU_BETON :
    {
      delete (Materiau_Beton *) materiau->data;
      break;
    }
    case MATERIAU_ACIER :
    {
      delete (Materiau_Acier *) materiau->data;
      break;
    }
    default :
    {
      FAILCRIT ( ,
                (gettext ("Matériau %d inconnu.\n"), materiau->type); )
      break;
    }
  }
  delete materiau;
  
  return;
}


/**
 * \brief Supprime le matériau spécifié. Impossible si le matériau est utilisé.
 * \param materiau : le matériau à supprimer,
 * \param p : la variable projet.
 * \return
 *   Succès : true.\n
 *   Échec : false :
 *       p == NULL,
 *       materiau == NULL,
 *       Le materiau est utilisé.
 */
bool
EF_materiaux_supprime (EF_Materiau *materiau,
                       Projet      *p)
{
  std::list <EF_Materiau *> liste_materiaux;
  std::list <EF_Barre *>   *liste_barres_dep;
  
  BUGPARAM (p, "%p", p, false)
  BUGPARAM (materiau, "%p", materiau, false)
   
  // On vérifie les dépendances.
  liste_materiaux.push_back (materiau);
  BUG (_1992_1_1_barres_cherche_dependances (p,
                                             NULL,
                                             NULL,
                                             NULL,
                                             &liste_materiaux,
                                             NULL,
                                             NULL,
                                             NULL,
                                             NULL,
                                             &liste_barres_dep,
                                             NULL,
                                             NULL,
                                             false),
       false)
  liste_materiaux.clear ();
  
  if (!liste_barres_dep->empty ())
  {
    std::string liste;
    
    liste = common_selection_barres_en_texte (liste_barres_dep);
    
    if (liste_barres_dep->size () == 1)
    {
      FAILINFO (false,
                (gettext ("Impossible de supprimer le matériau car il est utilisé par la barre %s.\n"),
                          liste.c_str ());
                  delete liste_barres_dep; )
    }
    else
    {
      FAILINFO (false,
                (gettext ("Impossible de supprimer le matériau car il est utilisé par les barres %s.\n"),
                          liste.c_str ());
                  delete liste_barres_dep; )
    }
  }
  delete liste_barres_dep;
  
#ifdef ENABLE_GTK
  gtk_list_store_remove (UI_MAT.liste_materiaux, &materiau->Iter_liste);
  if (UI_MAT.builder != NULL)
  {
    gtk_tree_store_remove (UI_MAT.materiaux, &materiau->Iter_fenetre);
  }
#endif
  
  p->modele.materiaux.remove (materiau);
  EF_materiaux_free_un (materiau);
  
  return true;
}


/**
 * \brief Libère l'ensemble des matériaux en béton.
 * \param p : la variable projet.
 * \return
 *   Succès : true.\n
 *   Échec : false :
 *     - p == NULL.
 */
bool
EF_materiaux_free (Projet *p)
{
  BUGPARAM (p, "%p", p, false)
  
  for_each (p->modele.materiaux.begin (),
            p->modele.materiaux.end (),
            EF_materiaux_free_un);
  p->modele.materiaux.clear ();
  
#ifdef ENABLE_GTK
  g_object_unref (UI_MAT.liste_materiaux);
#endif
  
  return true;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
