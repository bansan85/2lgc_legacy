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
#include <locale.h>
#include <libintl.h>
#include <math.h>
#include <string.h>
#include <gmodule.h>

#include "common_projet.h"
#include "common_math.h"
#ifdef ENABLE_GTK
#include "common_gtk.h"
#endif
#include "common_erreurs.h"
#include "common_selection.h"
#include "1992_1_1_barres.h"
#include "1992_1_1_materiaux.h"
#include "1993_1_1_materiaux.h"
#include "EF_calculs.h"


gboolean
EF_materiaux_init (Projet *p)
/**
 * \brief Initialise la liste des matériaux.
 * \param p : la variable projet.
 * \return
 *   Succès : TRUE.
 *   Échec : FALSE :
 *     - p == NULL.
 */
{
  BUGMSG (p, FALSE, gettext ("Paramètre %s incorrect.\n"), "projet")
  
  p->modele.materiaux = NULL;
  
#ifdef ENABLE_GTK
  UI_MAT.liste_materiaux = gtk_list_store_new (1, G_TYPE_STRING);
#endif
  
  return TRUE;
}


gboolean
EF_materiaux_insert (Projet      *p,
                     EF_Materiau *materiau)
/**
 * \brief Insère un materiau en triant la liste en fonction du nom. Procédure
 *        commune à tous les matériaux.
 * \param p : la variable projet,
 * \param materiau : le matériau à insérer.
 * \return
 *   Succès : TRUE.
 *   Échec : FALSE :
 *     - p == NULL,
 *     - materiau == NULL.
 */
{
  GList       *list_parcours;
  EF_Materiau *materiau_tmp;
  
  BUGMSG (p, FALSE, gettext ("Paramètre %s incorrect.\n"), "projet")
  BUGMSG (materiau, FALSE, gettext ("Paramètre %s incorrect.\n"), "materiau")
  
  list_parcours = p->modele.materiaux;
  while (list_parcours != NULL)
  {
    materiau_tmp = list_parcours->data;
    
    if (strcmp (materiau->nom, materiau_tmp->nom) < 0)
      break;
    
    list_parcours = g_list_next (list_parcours);
  }
  if (list_parcours == NULL)
  {
    p->modele.materiaux = g_list_append (p->modele.materiaux, materiau);
#ifdef ENABLE_GTK
    gtk_list_store_append (UI_MAT.liste_materiaux,
                           &materiau->Iter_liste);
    if (UI_MAT.builder != NULL)
      gtk_tree_store_append (UI_MAT.materiaux,
                             &materiau->Iter_fenetre,
                             NULL);
#endif
  }
  else
  {
    p->modele.materiaux = g_list_insert_before (p->modele.materiaux,
                                                list_parcours,
                                                materiau);
#ifdef ENABLE_GTK
    gtk_list_store_insert_before (UI_MAT.liste_materiaux,
                                  &materiau->Iter_liste,
                                  &materiau_tmp->Iter_liste);
    if (UI_MAT.builder != NULL)
      gtk_tree_store_insert_before (UI_MAT.materiaux,
                                    &materiau->Iter_fenetre,
                                    NULL,
                                    &materiau_tmp->Iter_fenetre);
#endif
  }
  
#ifdef ENABLE_GTK
  gtk_list_store_set (UI_MAT.liste_materiaux,
                      &materiau->Iter_liste,
                      0, materiau->nom,
                      -1);
  if (UI_MAT.builder != NULL)
    gtk_tree_store_set (UI_MAT.materiaux,
                        &materiau->Iter_fenetre,
                        0, materiau,
                        -1);
#endif
  
  return TRUE;
}


gboolean
EF_materiaux_repositionne (Projet      *p,
                           EF_Materiau *materiau)
/**
 * \brief Repositionne un matériau après un renommage. Procédure commune à
 *        toutes les matériaux.
 * \param p : la variable projet,
 * \param materiau : le matériau à repositionner.
 * \return
 *   Succès : TRUE.
 *   Échec : FALSE :
 *     - p == NULL,
 *     - materiau == NULL.
 */
{
  GList *list_parcours;
  
  BUGMSG (p, FALSE, gettext ("Paramètre %s incorrect.\n"), "projet")
  BUGMSG (materiau, FALSE, gettext ("Paramètre %s incorrect.\n"), "materiau")
  
  // On réinsère le matériau au bon endroit
  p->modele.materiaux = g_list_remove (p->modele.materiaux, materiau);
  list_parcours = p->modele.materiaux;
  while (list_parcours != NULL)
  {
    EF_Materiau *materiau_parcours = list_parcours->data;
    
    if (strcmp (materiau->nom, materiau_parcours->nom) < 0)
    {
      p->modele.materiaux = g_list_insert_before (p->modele.materiaux,
                                                  list_parcours,
                                                  materiau);
      
#ifdef ENABLE_GTK
      gtk_list_store_move_before (UI_MAT.liste_materiaux,
                                  &materiau->Iter_liste,
                                  &materiau_parcours->Iter_liste);
      if (UI_MAT.builder != NULL)
        gtk_tree_store_move_before (UI_MAT.materiaux,
                                    &materiau->Iter_fenetre,
                                    &materiau_parcours->Iter_fenetre);
#endif
      break;
    }
    
    list_parcours = g_list_next (list_parcours);
  }
  if (list_parcours == NULL)
  {
    p->modele.materiaux = g_list_append (p->modele.materiaux, materiau);
    
#ifdef ENABLE_GTK
    gtk_list_store_move_before (UI_MAT.liste_materiaux,
                                &materiau->Iter_liste,
                                NULL);
    if (UI_MAT.builder != NULL)
      gtk_tree_store_move_before (UI_MAT.materiaux,
                                  &materiau->Iter_fenetre,
                                  NULL);
#endif
  }
  
#ifdef ENABLE_GTK
  switch (materiau->type)
  {
    case MATERIAU_BETON :
    {
      if ((UI_BET.builder != NULL) && (UI_BET.materiau == materiau))
        gtk_text_buffer_set_text (gtk_text_view_get_buffer (GTK_TEXT_VIEW (
                                        gtk_builder_get_object (UI_BET.builder,
                                         "_1992_1_1_materiaux_textview_nom"))),
                                  materiau->nom,
                                  -1);
      break;
    }
    case MATERIAU_ACIER :
    {
      if ((UI_ACI.builder != NULL) && (UI_ACI.materiau == materiau))
        gtk_text_buffer_set_text (gtk_text_view_get_buffer (GTK_TEXT_VIEW (
                                        gtk_builder_get_object (UI_ACI.builder,
                                         "_1993_1_1_materiaux_textview_nom"))),
                                  materiau->nom,
                                  -1);
      break;
    }
    default :
    {
      BUGMSG (NULL,
              FALSE,
              gettext ("Le type de matériau %d n'existe pas.\n"), materiau->type)
      break;
    }
  }
  if (p->ui.ef_barres.builder != NULL)
    gtk_widget_queue_resize (GTK_WIDGET (gtk_builder_get_object (
                              p->ui.ef_barres.builder, "EF_barres_treeview")));
  gtk_list_store_set (UI_MAT.liste_materiaux,
                      &materiau->Iter_liste,
                      0, materiau->nom,
                      -1);
#endif
  
  return TRUE;
}


EF_Materiau *
EF_materiaux_cherche_nom (Projet     *p,
                          const char *nom,
                          gboolean    critique)
/**
 * \brief Renvoie le matériau en fonction de son nom.
 * \param p : la variable projet,
 * \param nom : le nom du matériau,
 * \param critique : utilise BUG si TRUE, return sinon.
 * \return
 *   Succès : pointeur vers le matériau en béton.\n
 *   Échec : NULL :
 *     - p == NULL,
 *     - materiau introuvable.
 */
{
  GList *list_parcours;
  
  BUGMSG (p, NULL, gettext ("Paramètre %s incorrect.\n"), "projet")
  BUGMSG (nom, NULL, gettext ("Paramètre %s incorrect.\n"), "nom")
  
  list_parcours = p->modele.materiaux;
  while (list_parcours != NULL)
  {
    EF_Materiau *materiau = list_parcours->data;
    
    if (strcmp (materiau->nom, nom) == 0)
      return materiau;
    
    list_parcours = g_list_next (list_parcours);
  }
  
  if (critique)
    BUGMSG (0, NULL, gettext ("Matériau '%s' introuvable.\n"), nom)
  else
    return NULL;
}


char *
EF_materiaux_get_description (EF_Materiau *materiau)
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
{
  switch (materiau->type)
  {
    case MATERIAU_BETON :
      return _1992_1_1_materiaux_get_description (materiau);
    case MATERIAU_ACIER :
      return _1993_1_1_materiaux_get_description (materiau);
    default :
    {
      BUGMSG (NULL,
              FALSE,
              gettext ("Le type de matériau %d n'existe pas.\n"), materiau->type)
      break;
    }
  }
}


Flottant
EF_materiaux_E (EF_Materiau *materiau)
/**
 * \brief Renvoie le module d'Young du matériau.
 * \param materiau : le matériau à analyser.
 * \return
 *   Succès : le module d'Young.\n
 *   Échec : NAN :
 *     - materiau == NULL,
 *     - materiau inconnu.
 */
{
  BUGMSG (materiau,
          m_f (NAN, FLOTTANT_ORDINATEUR),
          gettext ("Paramètre %s incorrect.\n"), "materiau")
  
  switch (materiau->type)
  {
    case MATERIAU_BETON :
    {
      Materiau_Beton *data_beton = materiau->data;
      
      return data_beton->ecm;
    }
    case MATERIAU_ACIER :
    {
      Materiau_Acier *data_acier = materiau->data;
      
      return data_acier->e;
    }
    default :
    {
      BUGMSG (NULL,
              m_f (NAN, FLOTTANT_ORDINATEUR),
              gettext ("Matériau %d inconnu.\n"), materiau->type)
      break;
    }
  }
  
  return m_f (NAN, FLOTTANT_ORDINATEUR);
}


Flottant
EF_materiaux_G (EF_Materiau *materiau,
                gboolean     nu_null)
/**
 * \brief Renvoie le module de cisaillement du matériau.
 * \param materiau : le matériau à analyser,
 * \param nu_null : TRUE si on force le coefficient de poisson à 0,
 *                  FALSE si on prend la valeur définie dans le matériau.
 * \return
 *   Succès : le module de cisaillement.\n
 *   Échec : NAN :
 *     - materiau == NULL,
 *     - materiau inconnu.
 */
{
  BUGMSG (materiau,
          m_f (NAN, FLOTTANT_ORDINATEUR),
          gettext ("Paramètre %s incorrect.\n"), "materiau")
  
  switch (materiau->type)
  {
    case MATERIAU_BETON :
    {
      Materiau_Beton *data_beton = materiau->data;
      
      if (nu_null)
        return m_f (m_g (data_beton->ecm) / 2., FLOTTANT_ORDINATEUR);
      else
        return m_f (m_g (data_beton->ecm) / (2. * (1. + m_g (data_beton->nu))),
                    FLOTTANT_ORDINATEUR);
    }
    case MATERIAU_ACIER :
    {
      Materiau_Acier *data_acier = materiau->data;
      
      BUGMSG (!nu_null,
              m_f (NAN, FLOTTANT_ORDINATEUR),
              gettext ("Seul le matériau béton supporte nu à 0.\n"))
      return m_f (m_g (data_acier->e) / (2. * (1. + m_g (data_acier->nu))),
                  FLOTTANT_ORDINATEUR);
    }
    default :
    {
      BUGMSG (NULL,
              m_f (NAN, FLOTTANT_ORDINATEUR),
              gettext ("Matériau %d inconnu.\n"), materiau->type)
      break;
    }
  }
  
  return m_f (NAN, FLOTTANT_ORDINATEUR);
}


void
EF_materiaux_free_un (EF_Materiau *materiau)
/**
 * \brief Fonction permettant de libérer un matériau.
 * \param materiau : matériau à libérer.
 * \return : Rien.
 *   Échec : 
 *       materiau == NULL,
 */
{
  BUGMSG (materiau, , gettext("Paramètre %s incorrect.\n"), "materiau")
  
  free (materiau->nom);
  free (materiau->data);
  free (materiau);
  
  return;
}


gboolean
EF_materiaux_supprime (EF_Materiau *materiau,
                       Projet      *p)
/**
 * \brief Supprime le matériau spécifié. Impossible si le matériau est utilisé.
 * \param materiau : le matériau à supprimer,
 * \param p : la variable projet.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *       p == NULL,
 *       materiau == NULL,
 *       Le materiau est utilisé.
 */
{
  GList *liste_materiaux = NULL, *liste_barres_dep;
  
  BUGMSG (p, FALSE, gettext ("Paramètre %s incorrect.\n"), "projet")
  BUGMSG (materiau, FALSE, gettext ("Paramètre %s incorrect.\n"), "materiau")
   
  // On vérifie les dépendances.
  liste_materiaux = g_list_append (liste_materiaux, materiau);
  BUG (_1992_1_1_barres_cherche_dependances (p,
                                             NULL,
                                             NULL,
                                             NULL,
                                             liste_materiaux,
                                             NULL,
                                             NULL,
                                             NULL,
                                             &liste_barres_dep,
                                             NULL,
                                             FALSE,
                                             FALSE),
       FALSE)
  g_list_free (liste_materiaux);
  
  if (liste_barres_dep != NULL)
  {
    char *liste;
    
    liste = common_selection_barres_en_texte (liste_barres_dep);
    if (g_list_next (liste_barres_dep) == NULL)
      BUGMSG (NULL, FALSE, gettext ("Impossible de supprimer le matériau car il est utilisé par la barre %s.\n"), liste)
    else
      BUGMSG (NULL, FALSE, gettext ("Impossible de supprimer le matériau car il est utilisé par les barres %s.\n"), liste)
  }
  
  BUG (_1992_1_1_barres_supprime_liste (p, NULL, liste_barres_dep), TRUE)
  g_list_free (liste_barres_dep);
  
#ifdef ENABLE_GTK
  gtk_list_store_remove (UI_MAT.liste_materiaux, &materiau->Iter_liste);
  if (UI_MAT.builder != NULL)
    gtk_tree_store_remove (UI_MAT.materiaux, &materiau->Iter_fenetre);
#endif
  
  EF_materiaux_free_un (materiau);
  p->modele.materiaux = g_list_remove (p->modele.materiaux, materiau);
  
  return TRUE;
}


gboolean
EF_materiaux_free (Projet *p)
/**
 * \brief Libère l'ensemble des matériaux en béton.
 * \param p : la variable projet.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL.
 */
{
  BUGMSG (p, FALSE, gettext ("Paramètre %s incorrect.\n"), "projet")
  
  while (p->modele.materiaux != NULL)
  {
    g_list_free_full (p->modele.materiaux,
                      (GDestroyNotify) &EF_materiaux_free_un);
    p->modele.materiaux = NULL;
  }
  
  BUG (EF_calculs_free (p), TRUE)
  
#ifdef ENABLE_GTK
  g_object_unref (UI_MAT.liste_materiaux);
#endif
  
  return TRUE;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
