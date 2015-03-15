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

#include <string.h>
#include <locale>

#include "1992_1_1_barres.hpp"
#include "common_projet.hpp"
#include "common_erreurs.hpp"
#include "common_selection.hpp"
#include "common_math.hpp"
#include "EF_calculs.hpp"
#include "EF_relachement.hpp"
#ifdef ENABLE_GTK
#include <gtk/gtk.h>
#include "EF_gtk_relachement.hpp"
#include "common_gtk.hpp"
#endif

/**
 * \brief Initialise la liste des relachements.
 * \param p : la variable projet.
 * \return
 *   Succès : true.\n
 *   Échec : false :
 *     - p == NULL,
 *     - en cas d'erreur d'allocation mémoire.
 */
bool
EF_relachement_init (Projet *p)
{
#ifdef ENABLE_GTK
  GtkTreeIter iter;
#endif
  
  BUGPARAM (p, "%p", p, false)
  
  p->modele.relachements.clear ();
  
#ifdef ENABLE_GTK
  UI_REL.liste_relachements = gtk_list_store_new (1, G_TYPE_STRING);
  gtk_list_store_append (UI_REL.liste_relachements, &iter);
  gtk_list_store_set (UI_REL.liste_relachements,
                      &iter,
                      0, gettext ("Aucun"),
                      -1);
#endif
  
  return true;
}


/**
 * \brief Ajoute un relachement. Les données fournis dans les paramètres
 *        additionnels doivent avoir été stockées en mémoire par l'utilisation
 *        de malloc.\n
 *        Si le type de relachement est libre ou broqué, data doit être NULL.\n
 *        Si le type est élastique linéaire, data doit être de type
 *          EF_Relachement_Donnees_Elastique_Lineaire.
 * \param p : la variable projet,
 * \param *nom : nom du relâchement,
 * \param rx_debut : relachement de la rotation autour de l'axe x au début,
 * \param rx_d_data : paramètre additionnel de la rotation en x si nécessaire,
 * \param ry_debut : relachement de la rotation autour de l'axe y au début,
 * \param ry_d_data : paramètre additionnel de la rotation en y si nécessaire,
 * \param rz_debut : relachement de la rotation autour de l'axe z au début,
 * \param rz_d_data : paramètre additionnel de la rotation en z si nécessaire,
 * \param rx_fin : relachement de la rotation autour de l'axe x à la fin,
 * \param rx_f_data : paramètre additionnel de la rotation en x si nécessaire,
 * \param ry_fin : relachement de la rotation autour de l'axe y à la fin,
 * \param ry_f_data : paramètre additionnel de la rotation en y si nécessaire,
 * \param rz_fin : relachement de la rotation autour de l'axe z à la fin,
 * \param rz_f_data : paramètre additionnel de la rotation en z si nécessaire.
 * \return
 *   Succès : pointeur vers le nouveau relâchement.\n
 *   Échec : NULL :
 *     - p == NULL,
 *     - rx_debut == EF_RELACHEMENT_LIBRE && rx_fin == EF_RELACHEMENT_LIBRE,
 *     - en cas d'erreur d'allocation mémoire.
 */
EF_Relachement *
EF_relachement_ajout (Projet             *p,
                      std::string        *nom,
                      EF_Relachement_Type rx_debut,
                      void               *rx_d_data,
                      EF_Relachement_Type ry_debut,
                      void               *ry_d_data,
                      EF_Relachement_Type rz_debut,
                      void               *rz_d_data,
                      EF_Relachement_Type rx_fin,
                      void               *rx_f_data,
                      EF_Relachement_Type ry_fin,
                      void               *ry_f_data,
                      EF_Relachement_Type rz_fin,
                      void               *rz_f_data)
{
  EF_Relachement *relachement_nouveau;
  EF_Relachement *relachement_tmp;
  std::list <EF_Relachement *>::iterator it;
  
  BUGPARAM (p, "%p", p, NULL)
  INFO (!((rx_debut == EF_RELACHEMENT_LIBRE) &&
          (rx_fin == EF_RELACHEMENT_LIBRE)),
        NULL,
        (gettext ("Impossible de relâcher rx simultanément des deux cotés de la barre.\n")); )
  INFO (nom->compare (gettext ("Aucun")),
        NULL,
        (gettext ("Impossible d'utiliser comme nom 'Aucun'.\n")); )
  INFO (rx_debut != EF_RELACHEMENT_ELASTIQUE_LINEAIRE || rx_d_data != NULL,
        NULL,
        (gettext ("Un relachement élastique linéaire doit être défini avec ses paramètres.")); )
  INFO (ry_debut != EF_RELACHEMENT_ELASTIQUE_LINEAIRE || ry_d_data != NULL,
        NULL,
        (gettext ("Un relachement élastique linéaire doit être défini avec ses paramètres.")); )
  INFO (rz_debut != EF_RELACHEMENT_ELASTIQUE_LINEAIRE || rz_d_data != NULL,
        NULL,
        (gettext ("Un relachement élastique linéaire doit être défini avec ses paramètres.")); )
  INFO (rx_fin != EF_RELACHEMENT_ELASTIQUE_LINEAIRE || rx_f_data != NULL,
        NULL,
        (gettext ("Un relachement élastique linéaire doit être défini avec ses paramètres.")); )
  INFO (ry_fin != EF_RELACHEMENT_ELASTIQUE_LINEAIRE || ry_f_data != NULL,
        NULL,
        (gettext ("Un relachement élastique linéaire doit être défini avec ses paramètres.")); )
  INFO (rz_fin != EF_RELACHEMENT_ELASTIQUE_LINEAIRE || rz_f_data != NULL,
        NULL,
        (gettext ("Un relachement élastique linéaire doit être défini avec ses paramètres.")); )
  relachement_nouveau = new EF_Relachement;
  
  relachement_nouveau->rx_debut = rx_debut;
  relachement_nouveau->rx_d_data = rx_d_data;
  relachement_nouveau->ry_debut = ry_debut;
  relachement_nouveau->ry_d_data = ry_d_data;
  relachement_nouveau->rz_debut = rz_debut;
  relachement_nouveau->rz_d_data = rz_d_data;
  relachement_nouveau->rx_fin = rx_fin;
  relachement_nouveau->rx_f_data = rx_f_data;
  relachement_nouveau->ry_fin = ry_fin;
  relachement_nouveau->ry_f_data = ry_f_data;
  relachement_nouveau->rz_fin = rz_fin;
  relachement_nouveau->rz_f_data = rz_f_data;
  relachement_nouveau->nom.assign (*nom);
  
  it = p->modele.relachements.begin ();
  while (it != p->modele.relachements.end ())
  {
    relachement_tmp = *it;
    
    if (relachement_nouveau->nom.compare (relachement_tmp->nom) < 0)
    {
      break;
    }
    
    ++it;
  }
  if (it == p->modele.relachements.end ())
  {
    p->modele.relachements.push_back (relachement_nouveau);
#ifdef ENABLE_GTK
    gtk_list_store_append (UI_REL.liste_relachements,
                           &relachement_nouveau->Iter_liste);
    if (UI_REL.builder != NULL)
    {
      gtk_tree_store_append (UI_REL.relachements,
                             &relachement_nouveau->Iter_fenetre,
                             NULL);
    }
#endif
  }
  else
  {
    p->modele.relachements.insert (it, relachement_nouveau);
#ifdef ENABLE_GTK
    gtk_list_store_insert_before (UI_REL.liste_relachements,
                                  &relachement_nouveau->Iter_liste,
                                  &relachement_tmp->Iter_liste);
    if (UI_REL.builder != NULL)
    {
      gtk_tree_store_insert_before (UI_REL.relachements,
                                    &relachement_nouveau->Iter_fenetre,
                                    NULL,
                                    &relachement_tmp->Iter_fenetre);
    }
#endif
  }
  
#ifdef ENABLE_GTK
  gtk_list_store_set (UI_REL.liste_relachements,
                      &relachement_nouveau->Iter_liste,
                      0, nom,
                      -1);
  if (UI_REL.builder != NULL)
  {
    gtk_tree_store_set (UI_REL.relachements,
                        &relachement_nouveau->Iter_fenetre,
                        0, relachement_nouveau,
                        -1);
  }
#endif
  
  return relachement_nouveau;
}


/**
 * \brief Renvoie le relachement cherché.
 * \param p : la variable projet,
 * \param nom : le nom du relachement.
 * \param critique : si critique = true, BUG est utilisé, return sinon
 * \return
 *   Succès : pointeur vers le relachement recherché.\n
 *   Échec : NULL :
 *     - p == NULL,
 *     - nom == NULL,
 *     - relachement introuvable.
 */
EF_Relachement *
EF_relachement_cherche_nom (Projet      *p,
                            std::string *nom,
                            bool         critique)
{
  std::list <EF_Relachement *>::iterator it;
  
  BUGPARAM (p, "%p", p, NULL)
  BUGPARAM (nom, "%p", nom, NULL)
  
  it = p->modele.relachements.begin ();
  while (it != p->modele.relachements.end ())
  {
    EF_Relachement *relachement = *it;
    
    if (relachement->nom.compare (*nom) == 0)
    {
      return relachement;
    }
    
    ++it;
  }
  
  if (critique)
  {
    FAILINFO (NULL,
              (gettext ("Relachement '%s' introuvable.\n"),
                        nom->c_str ()); )
  }
  else
  {
    return NULL;
  }
}


/**
 * \brief Modifie un relâchement.
 * \param p : la variable projet,
 * \param relachement : le relâchement à modifier,
 * \param nom : nouveau nom. mettre NULL pour le nom pour ne pas le modifier,
 * \param rx_debut : relachement de la rotation autour de l'axe x au début,
 *                   EF_RELACHEMENT_UNTOUCH pour ne pas modifier,
 * \param rx_d_data : paramètre additionnel de la rotation en x si nécessaire,
 *                    NULL pour ne pas modifier,
 * \param ry_debut : relachement de la rotation autour de l'axe y au début,
 *                   EF_RELACHEMENT_UNTOUCH pour ne pas modifier,
 * \param ry_d_data : paramètre additionnel de la rotation en y si nécessaire,
 *                    NULL pour ne pas modifier,
 * \param rz_debut : relachement de la rotation autour de l'axe z au début,
 *                   EF_RELACHEMENT_UNTOUCH pour ne pas modifier,
 * \param rz_d_data : paramètre additionnel de la rotation en z si nécessaire,
 *                    NULL pour ne pas modifier,
 * \param rx_fin : relachement de la rotation autour de l'axe x à la fin,
 *                 EF_RELACHEMENT_UNTOUCH pour ne pas modifier,
 * \param rx_f_data : paramètre additionnel de la rotation en x si nécessaire,
 *                    NULL pour ne pas modifier,
 * \param ry_fin : relachement de la rotation autour de l'axe y à la fin,
 *                 EF_RELACHEMENT_UNTOUCH pour ne pas modifier,
 * \param ry_f_data : paramètre additionnel de la rotation en y si nécessaire,
 *                    NULL pour ne pas modifier,
 * \param rz_fin : relachement de la rotation autour de l'axe z à la fin,
 *                 EF_RELACHEMENT_UNTOUCH pour ne pas modifier,
 * \param rz_f_data : paramètre additionnel de la rotation en z si nécessaire.
 *                    NULL pour ne pas modifier,
 * \return
 *   Succès : true.\n
 *   Échec : false :
 *     - p == NULL,
 *     - materiau == NULL.
 */
bool
EF_relachement_modif (Projet             *p,
                      EF_Relachement     *relachement,
                      std::string        *nom,
                      EF_Relachement_Type rx_debut,
                      void               *rx_d_data,
                      EF_Relachement_Type ry_debut,
                      void               *ry_d_data,
                      EF_Relachement_Type rz_debut,
                      void               *rz_d_data,
                      EF_Relachement_Type rx_fin,
                      void               *rx_f_data,
                      EF_Relachement_Type ry_fin,
                      void               *ry_f_data,
                      EF_Relachement_Type rz_fin,
                      void               *rz_f_data)
{
  std::list <EF_Relachement *> liste_relachement;
  std::list <EF_Barre *>      *liste_barres_dep;
  
  BUGPARAM (p, "%p", p, false)
  BUGPARAM (relachement, "%p", relachement, false)
  INFO (!(((rx_debut == EF_RELACHEMENT_UNTOUCH ?
                  relachement->rx_debut : rx_debut) == EF_RELACHEMENT_LIBRE) &&
          ((rx_fin == EF_RELACHEMENT_UNTOUCH ?
                       relachement->rx_fin : rx_fin) == EF_RELACHEMENT_LIBRE)),
        false,
        (gettext ("Impossible de relâcher rx simultanément des deux cotés de la barre.\n")); )
  INFO (((rx_debut == EF_RELACHEMENT_UNTOUCH ?
            relachement->rx_debut : rx_debut) !=
              EF_RELACHEMENT_ELASTIQUE_LINEAIRE) ||
        ((rx_d_data == NULL ? relachement->rx_d_data : rx_d_data) != NULL),
        false,
        (gettext ("Un relachement élastique linéaire doit être défini avec ses paramètres.")); )
  INFO (((ry_debut == EF_RELACHEMENT_UNTOUCH ?
            relachement->ry_debut : ry_debut) !=
              EF_RELACHEMENT_ELASTIQUE_LINEAIRE) ||
        ((ry_d_data == NULL ? relachement->ry_d_data : ry_d_data) != NULL),
        false,
        (gettext ("Un relachement élastique linéaire doit être défini avec ses paramètres.")); )
  INFO (((rz_debut == EF_RELACHEMENT_UNTOUCH ?
            relachement->rz_debut : rz_debut) !=
              EF_RELACHEMENT_ELASTIQUE_LINEAIRE) ||
        ((rz_d_data == NULL ? relachement->rz_d_data : rz_d_data) != NULL),
        false,
        (gettext ("Un relachement élastique linéaire doit être défini avec ses paramètres.")); )
  INFO (((rx_fin == EF_RELACHEMENT_UNTOUCH ?
            relachement->rx_fin : rx_fin) !=
              EF_RELACHEMENT_ELASTIQUE_LINEAIRE) ||
        ((rx_f_data == NULL ? relachement->rx_f_data : rx_f_data) != NULL),
        false,
        (gettext ("Un relachement élastique linéaire doit être défini avec ses paramètres.")); )
  INFO (((ry_fin == EF_RELACHEMENT_UNTOUCH ?
            relachement->ry_fin : ry_fin) !=
              EF_RELACHEMENT_ELASTIQUE_LINEAIRE) ||
        ((ry_f_data == NULL ? relachement->ry_f_data : ry_f_data) != NULL),
        false,
        (gettext ("Un relachement élastique linéaire doit être défini avec ses paramètres.")); )
  INFO (((rz_fin == EF_RELACHEMENT_UNTOUCH ?
            relachement->rz_fin : rz_fin) !=
              EF_RELACHEMENT_ELASTIQUE_LINEAIRE) ||
        ((rz_f_data == NULL ? relachement->rz_f_data : rz_f_data) != NULL),
        false,
        (gettext ("Un relachement élastique linéaire doit être défini avec ses paramètres.")); )
  
  INFO (!((((rx_debut == EF_RELACHEMENT_UNTOUCH ?
               relachement->rx_debut : rx_debut) ==
                 EF_RELACHEMENT_BLOQUE) ||
           ((rx_debut == EF_RELACHEMENT_UNTOUCH ?
               relachement->rx_debut : rx_debut) ==
                 EF_RELACHEMENT_LIBRE)) &&
          (rx_d_data != NULL)),
        false,
        (gettext ("Un relachement libre ou bloqué ne doit pas avoir de paramètres.")); )
  INFO (!((((ry_debut == EF_RELACHEMENT_UNTOUCH ?
               relachement->ry_debut : ry_debut) ==
                 EF_RELACHEMENT_BLOQUE) ||
           ((ry_debut == EF_RELACHEMENT_UNTOUCH ?
               relachement->ry_debut : ry_debut) ==
                 EF_RELACHEMENT_LIBRE)) &&
          (ry_d_data != NULL)),
        false,
        (gettext ("Un relachement libre ou bloqué ne doit pas avoir de paramètres.")); )
  INFO (!((((rz_debut == EF_RELACHEMENT_UNTOUCH ?
               relachement->rz_debut : rz_debut) ==
                 EF_RELACHEMENT_BLOQUE) ||
           ((rz_debut == EF_RELACHEMENT_UNTOUCH ?
               relachement->rz_debut : rz_debut) ==
                 EF_RELACHEMENT_LIBRE)) &&
          (rz_d_data != NULL)),
        false,
        (gettext ("Un relachement libre ou bloqué ne doit pas avoir de paramètres.")); )
  INFO (!((((rx_fin == EF_RELACHEMENT_UNTOUCH ?
               relachement->rx_fin : rx_fin) ==
                 EF_RELACHEMENT_BLOQUE) ||
           ((rx_fin == EF_RELACHEMENT_UNTOUCH ?
               relachement->rx_fin : rx_fin) ==
                 EF_RELACHEMENT_LIBRE)) &&
          (rx_f_data != NULL)),
        false,
        (gettext ("Un relachement libre ou bloqué ne doit pas avoir de paramètres.")); )
  INFO (!((((ry_fin == EF_RELACHEMENT_UNTOUCH ?
               relachement->ry_fin : ry_fin) ==
                 EF_RELACHEMENT_BLOQUE) ||
           ((ry_fin == EF_RELACHEMENT_UNTOUCH ?
               relachement->ry_fin : ry_fin) ==
                 EF_RELACHEMENT_LIBRE)) &&
          (ry_f_data != NULL)),
        false,
        (gettext ("Un relachement libre ou bloqué ne doit pas avoir de paramètres.")); )
  INFO (!((((rz_fin == EF_RELACHEMENT_UNTOUCH ?
               relachement->rz_fin : rz_fin) ==
                 EF_RELACHEMENT_BLOQUE) ||
           ((rz_fin == EF_RELACHEMENT_UNTOUCH ?
               relachement->rz_fin : rz_fin) ==
                 EF_RELACHEMENT_LIBRE)) &&
          (rz_f_data != NULL)),
        false,
        (gettext ("Un relachement libre ou bloqué ne doit pas avoir de paramètres.")); )
  
  liste_relachement.push_back (relachement);
  BUG (_1992_1_1_barres_cherche_dependances (p,
                                             NULL,
                                             NULL,
                                             NULL,
                                             NULL,
                                             &liste_relachement,
                                             NULL,
                                             NULL,
                                             NULL,
                                             &liste_barres_dep,
                                             NULL,
                                             NULL,
                                             false),
       false)
  if (!liste_barres_dep->empty ())
  {
    BUG (EF_calculs_free (p),
         false,
         delete liste_barres_dep; )
  }
  delete liste_barres_dep;
  
  if ((nom != NULL) && (relachement->nom.compare (*nom) != 0))
  {
    std::list <EF_Relachement *>::iterator it;
    
    INFO (!EF_relachement_cherche_nom (p, nom, false),
          false,
          (gettext ("Le relâchement %s existe déjà.\n"),
                    nom->c_str ()); )
    relachement->nom.assign (*nom);
#ifdef ENABLE_GTK
    gtk_list_store_set (UI_REL.liste_relachements,
                        &relachement->Iter_liste,
                        0, relachement->nom.c_str (),
                        -1);
#endif
    
    // On réinsère le relâchement au bon endroit
    p->modele.relachements.remove (relachement);
    it = p->modele.relachements.begin ();
    while (it != p->modele.relachements.end ())
    {
      EF_Relachement *relachement_parcours = *it;
      
      if (relachement->nom.compare (relachement_parcours->nom) < 0)
      {
        p->modele.relachements.insert (it, relachement);
        
#ifdef ENABLE_GTK
        gtk_list_store_move_before (UI_REL.liste_relachements,
                                    &relachement->Iter_liste,
                                    &relachement_parcours->Iter_liste);
        if (UI_REL.builder != NULL)
        {
          gtk_tree_store_move_before (UI_REL.relachements,
                                      &relachement->Iter_fenetre,
                                      &relachement_parcours->Iter_fenetre);
        }
#endif
        break;
      }
      
      ++it;
    }
    if (it != p->modele.relachements.end ())
    {
      p->modele.relachements.push_back (relachement);
      
#ifdef ENABLE_GTK
      gtk_list_store_move_before (UI_REL.liste_relachements,
                                  &relachement->Iter_liste,
                                  NULL);
      if (UI_REL.builder != NULL)
      {
        gtk_tree_store_move_before (UI_REL.relachements,
                                    &relachement->Iter_fenetre,
                                    NULL);
      }
#endif
    }
  
#ifdef ENABLE_GTK
    if (UI_BAR.builder != NULL)
    {
      gtk_widget_queue_resize (GTK_WIDGET (gtk_builder_get_object (
                                       UI_BAR.builder, "EF_barres_treeview")));
    }
#endif
  }
  
#ifdef ENABLE_GTK
#define APPLY_REL(RELA, RELA_DATA, COL) \
  if (RELA != EF_RELACHEMENT_UNTOUCH) \
    relachement->RELA = RELA; \
  switch (relachement->RELA) \
  { \
    case EF_RELACHEMENT_BLOQUE : \
    case EF_RELACHEMENT_LIBRE : \
    { \
      if (UI_REL.builder != NULL) \
        g_object_set (gtk_builder_get_object (UI_REL.builder, \
                                        "EF_relachements_treeview_cell"#COL), \
                      "editable", FALSE, \
                      NULL); \
      free (relachement->RELA_DATA); \
      relachement->RELA_DATA = NULL; \
      break; \
    } \
    case EF_RELACHEMENT_ELASTIQUE_LINEAIRE : \
    { \
      if (UI_REL.builder != NULL) \
        g_object_set (gtk_builder_get_object (UI_REL.builder, \
                                        "EF_relachements_treeview_cell"#COL), \
                      "editable", TRUE, \
                      NULL); \
      if (RELA_DATA != NULL) \
      { \
        free (relachement->RELA_DATA); \
        BUGCRIT (relachement->RELA_DATA = \
                 malloc (sizeof (EF_Relachement_Donnees_Elastique_Lineaire)), \
                 false, \
                 (gettext ("Erreur d'allocation mémoire.\n")); ) \
        memcpy (relachement->RELA_DATA, \
                RELA_DATA, \
                sizeof (EF_Relachement_Donnees_Elastique_Lineaire)); \
      } \
      break; \
    } \
    case EF_RELACHEMENT_UNTOUCH : \
    default : \
    { \
      FAILCRIT (false, (gettext ("Le type de relâchement est inconnu.\n")); ) \
      break; \
    } \
  }
#else
#define APPLY_REL(RELA, RELA_DATA, COL) \
  if (RELA != EF_RELACHEMENT_UNTOUCH) \
    relachement->RELA = RELA; \
  switch (relachement->RELA) \
  { \
    case EF_RELACHEMENT_BLOQUE : \
    case EF_RELACHEMENT_LIBRE : \
    { \
      free (relachement->RELA_DATA); \
      relachement->RELA_DATA = NULL; \
      break; \
    } \
    case EF_RELACHEMENT_ELASTIQUE_LINEAIRE : \
    { \
      if (RELA_DATA != NULL) \
      { \
        free (relachement->RELA_DATA); \
        BUGCRIT (relachement->RELA_DATA = \
                 malloc (sizeof (EF_Relachement_Donnees_Elastique_Lineaire)), \
                 false, \
                 (gettext ("Erreur d'allocation mémoire.\n")); ) \
        memcpy (relachement->RELA_DATA, \
                RELA_DATA, \
                sizeof (EF_Relachement_Donnees_Elastique_Lineaire)); \
      } \
      break; \
    } \
    case EF_RELACHEMENT_UNTOUCH : \
    default : \
    { \
      FAILCRIT (false, (gettext ("Le type de relâchement est inconnu.\n")); ) \
      break; \
    } \
  }
#endif
  
  APPLY_REL (rx_debut, rx_d_data, 2)
  APPLY_REL (ry_debut, ry_d_data, 4)
  APPLY_REL (rz_debut, rz_d_data, 6)
  APPLY_REL (rx_fin, rx_f_data, 8)
  APPLY_REL (ry_fin, ry_f_data, 10)
  APPLY_REL (rz_fin, rz_f_data, 12)
  
#undef APPLY_REL
  
#ifdef ENABLE_GTK
  if (UI_REL.builder != NULL)
  {
    gtk_widget_queue_resize (GTK_WIDGET (gtk_builder_get_object (
                                 UI_REL.builder, "EF_relachements_treeview")));
  }
#endif
  
  return true;
}


/**
 * \brief Supprime le relâchement spécifié.
 * \param relachement : le relâchement à supprimer,
 * \param annule_si_utilise : possibilité d'annuler la suppression si le
 *        relâchement est attribué à une barre. Si l'option est désactivée, les
 *        barres (et les barres et noeuds intermédiaires dépendants) utilisant
 *        le relâchement seront supprimées.
 * \param p : la variable projet.
 * \return
 *   Succès : true.\n
 *   Échec : false :
 *     - p == NULL,
 *     - section == NULL.
 */
bool
EF_relachement_supprime (EF_Relachement *relachement,
                         bool            annule_si_utilise,
                         Projet         *p)
{
  std::list <EF_Relachement *> liste_relachements;
  std::list <EF_Barre *>      *liste_barres_dep;
  
  BUGPARAM (p, "%p", p, false)
  BUGPARAM (relachement, "%p", relachement, false)
  
  // On vérifie les dépendances.
  liste_relachements.push_back (relachement);
  BUG (_1992_1_1_barres_cherche_dependances (p,
                                             NULL,
                                             NULL,
                                             NULL,
                                             NULL,
                                             &liste_relachements,
                                             NULL,
                                             NULL,
                                             NULL,
                                             &liste_barres_dep,
                                             NULL,
                                             NULL,
                                             false),
       false)
  
  if ((annule_si_utilise) && (!liste_barres_dep->empty ()))
  {
    std::string liste;
    
    liste = common_selection_barres_en_texte (liste_barres_dep);
    
    if (liste_barres_dep->size () == 1)
    {
      FAILINFO (false,
                (gettext ("Impossible de supprimer la section car elle est utilisée par la barre %s.\n"),
                          liste.c_str ());
                  delete liste_barres_dep; )
    }
    else
    {
      FAILINFO (false,
                (gettext ("Impossible de supprimer la section car elle est utilisée par les barres %s.\n"),
                          liste.c_str ());
                  delete liste_barres_dep; )
    }
  }
  
  BUG (_1992_1_1_barres_supprime_liste (p, NULL, liste_barres_dep),
       true,
       delete liste_barres_dep; )
  delete liste_barres_dep;
  
  delete relachement->rx_d_data;
  delete relachement->ry_d_data;
  delete relachement->rz_d_data;
  delete relachement->rx_f_data;
  delete relachement->ry_f_data;
  delete relachement->rz_f_data;
  p->modele.relachements.remove (relachement);
  
#ifdef ENABLE_GTK
  gtk_list_store_remove (UI_REL.liste_relachements, &relachement->Iter_liste);
  if (UI_REL.builder != NULL)
  {
    gtk_tree_store_remove (UI_REL.relachements, &relachement->Iter_fenetre);
  }
#endif
  
  delete relachement;
  
  return true;
}


/**
 * \brief Libère l'ensemble des relachements et la liste les contenant.
 * \param p : la variable projet.
 * \return
 *   Succès : true.\n
 *   Échec : false :
 *     - p == NULL.
 */
bool
EF_relachement_free (Projet *p)
{
  std::list <EF_Relachement *>::iterator it;
  
  BUGPARAM (p, "%p", p, false)
  
  it = p->modele.relachements.begin ();
  while (it != p->modele.relachements.end ())
  {
    EF_Relachement *relachement = *it;
    
    delete relachement->rx_d_data;
    delete relachement->ry_d_data;
    delete relachement->rz_d_data;
    delete relachement->rx_f_data;
    delete relachement->ry_f_data;
    delete relachement->rz_f_data;
    delete relachement;
    
    ++it;
  }
  p->modele.relachements.clear ();
  
#ifdef ENABLE_GTK
  g_object_unref (UI_REL.liste_relachements);
#endif
  
  return true;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
