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
#include <string.h>
#include <gmodule.h>

#include <memory>

#include "common_projet.hpp"
#include "common_erreurs.hpp"
#include "common_selection.hpp"
#include "EF_noeuds.hpp"
#include "EF_calculs.hpp"
#include "EF_appuis.hpp"
#include "1992_1_1_barres.hpp"

#ifdef ENABLE_GTK
#include <gtk/gtk.h>
#include "common_m3d.hpp"
#include "common_gtk.hpp"
#endif

/**
 * \brief Initialise la liste des types d'appuis.
 * \param p : la variable projet.
 * \return
 *   Succès : true.\n
 *   Échec : false :
 *     - p == NULL.
 */
bool
EF_appuis_init (Projet *p)
{
#ifdef ENABLE_GTK
  GtkTreeIter iter;
#endif
  
  BUGPARAM (p, "%p", p, false)

  p->modele.appuis.clear ();
  
#ifdef ENABLE_GTK
  UI_APP.liste_appuis = gtk_list_store_new (1, G_TYPE_STRING);
  gtk_list_store_append (UI_APP.liste_appuis, &iter);
  gtk_list_store_set (UI_APP.liste_appuis, &iter, 0, gettext ("Aucun"), -1);
  UI_APP.liste_type_appui = gtk_list_store_new (1, G_TYPE_STRING);
  gtk_list_store_append (UI_APP.liste_type_appui, &iter);
  gtk_list_store_set (UI_APP.liste_type_appui,
                      &iter,
                      0, gettext ("Libre"),
                      -1);
  gtk_list_store_append (UI_APP.liste_type_appui, &iter);
  gtk_list_store_set (UI_APP.liste_type_appui,
                      &iter,
                      0, gettext ("Bloqué"),
                      -1);
#endif  
  return true;
}


/**
 * \brief Renvoie l'appui correspondant au nom demandé.
 * \param p : la variable projet,
 * \param nom : le nom de l'appui,
 * \param critique : true si en cas d'échec, la fonction BUG est utilisée.
 * \return
 *   Succès : pointeur vers l'appui.\n
 *   Échec : NULL :
 *     - p == NULL,
 *     - l'appui n'existe pas.
 */
EF_Appui *
EF_appuis_cherche_nom (Projet     *p,
                       const char *nom,
                       bool        critique)
{
  std::list <EF_Appui *>::iterator it;
  
  BUGPARAM (p, "%p", p, NULL)
  
  it = p->modele.appuis.begin ();
  while (it != p->modele.appuis.end ())
  {
    EF_Appui *appui = *it;
    
    if (strcmp (appui->nom, nom) == 0)
    {
      return appui;
    }
    
    ++it;
  }
  
  if (critique)
  {
    FAILINFO (NULL, (gettext ("Appui '%s' est introuvable.\n"), nom); )
  }
  else
  {
    return NULL;
  }
}


/**
 * \brief Ajoute un type d'appui et trie la liste en fonction du nom.
 * \param p : la variable projet,
 * \param nom : le nom du nouvel appui,
 * \param x : définition du déplacement en x,
 * \param y : définition du déplacement en y,
 * \param z : définition du déplacement en z,
 * \param rx : définition de la rotation autour de l'axe x,
 * \param ry : définition de la rotation autour de l'axe y,
 * \param rz : définition de la rotation autour de l'axe z.
 * \return
 *   Succès : pointeur vers le nouvel appui.\n
 *   Échec : NULL :
 *     - p == NULL,
 *     - le nom est déjà utilisé,
 *     - x, y, z, rx, ry, rz sont de type inconnu,
 *     - en cas d'erreur d'allocation mémoire.
 */
EF_Appui *
EF_appuis_ajout (Projet       *p,
                 const char   *nom,
                 Type_EF_Appui x,
                 Type_EF_Appui y,
                 Type_EF_Appui z,
                 Type_EF_Appui rx,
                 Type_EF_Appui ry,
                 Type_EF_Appui rz)
{
  std::unique_ptr <EF_Appui> appui_nouveau (new EF_Appui);
  EF_Appui *appui_parcours, *app;
  std::list <EF_Appui *>::iterator it;
  
  BUGPARAM (p, "%p", p, NULL)
  INFO (strcmp (nom, gettext ("Aucun")),
        NULL,
        (gettext ("Impossible d'utiliser comme nom 'Aucun'.\n")); )
  
  INFO (EF_appuis_cherche_nom (p, nom, false) == NULL,
        NULL,
        (gettext ("L'appui '%s' existe déjà.\n"), nom); )
   
  appui_nouveau.get ()->ux = x;
  switch (x)
  {
    case EF_APPUI_LIBRE :
    case EF_APPUI_BLOQUE :
    {
      appui_nouveau.get ()->ux_donnees = NULL;
      break;
    }
    default:
    {
      FAILINFO (NULL,
                (gettext ("Type d'appui %d inconnu.\n"), x); )
      break;
    }
  }
  appui_nouveau.get ()->uy = y;
  switch (y)
  {
    case EF_APPUI_LIBRE :
    case EF_APPUI_BLOQUE :
    {
      appui_nouveau.get ()->uy_donnees = NULL;
      break;
    }
    default:
    {
      FAILINFO (NULL,
                (gettext ("Type d'appui %d inconnu.\n"), y); )
      break;
    }
  }
  appui_nouveau.get ()->uz = z;
  switch (z)
  {
    case EF_APPUI_LIBRE :
    case EF_APPUI_BLOQUE :
    {
      appui_nouveau.get ()->uz_donnees = NULL;
      break;
    }
    default:
    {
      FAILINFO (NULL,
                (gettext ("Type d'appui %d inconnu.\n"), z); )
      break;
    }
  }
  appui_nouveau.get ()->rx = rx;
  switch (rx)
  {
    case EF_APPUI_LIBRE :
    case EF_APPUI_BLOQUE :
    {
      appui_nouveau.get ()->rx_donnees = NULL;
      break;
    }
    default:
    {
      FAILINFO (NULL,
                (gettext ("Type d'appui %d inconnu.\n"), rx); )
      break;
    }
  }
  appui_nouveau.get ()->ry = ry;
  switch (ry)
  {
    case EF_APPUI_LIBRE :
    case EF_APPUI_BLOQUE :
    {
      appui_nouveau.get ()->ry_donnees = NULL;
      break;
    }
    default:
    {
      FAILINFO (NULL,
                (gettext ("Type d'appui %d inconnu.\n"), ry); )
      break;
    }
  }
  appui_nouveau.get ()->rz = rz;
  switch (rz)
  {
    case EF_APPUI_LIBRE :
    case EF_APPUI_BLOQUE :
    {
      appui_nouveau.get ()->rz_donnees = NULL;
      break;
    }
    default:
    {
      FAILINFO (NULL,
                (gettext ("Type d'appui %d inconnu.\n"), rz); )
      break;
    }
  }
  
  BUGCRIT (appui_nouveau.get ()->nom = g_strdup_printf ("%s", nom),
           NULL,
           (gettext ("Erreur d'allocation mémoire.\n")); )
  
  it = p->modele.appuis.begin ();
  while (it != p->modele.appuis.end ())
  {
    appui_parcours = *it;
    if (strcmp (nom, appui_parcours->nom) < 0)
    {
      break;
    }
    
    ++it;
  }
  
  app = appui_nouveau.release ();
  if (it == p->modele.appuis.end ())
  {
    p->modele.appuis.push_back (app);
#ifdef ENABLE_GTK
    gtk_list_store_append (UI_APP.liste_appuis, &app->Iter_liste);
    if (UI_APP.builder != NULL)
    {
      gtk_tree_store_append (GTK_TREE_STORE (gtk_builder_get_object (
                                       UI_APP.builder, "EF_appuis_treestore")),
                             &app->Iter_fenetre,
                             NULL);
    }
#endif
  }
  else
  {
    p->modele.appuis.insert (it, app);
#ifdef ENABLE_GTK
    gtk_list_store_insert_before (UI_APP.liste_appuis,
                                  &app->Iter_liste,
                                  &appui_parcours->Iter_liste);
    if (UI_APP.builder != NULL)
    {
      gtk_tree_store_insert_before (GTK_TREE_STORE (gtk_builder_get_object (
                                       UI_APP.builder, "EF_appuis_treestore")),
                                    &app->Iter_fenetre,
                                    NULL,
                                    &appui_parcours->Iter_fenetre);
    }
#endif
  }
#ifdef ENABLE_GTK
  gtk_list_store_set (UI_APP.liste_appuis,
                      &app->Iter_liste,
                      0, nom,
                      -1);
  if (UI_APP.builder != NULL)
  {
    gtk_tree_store_set (GTK_TREE_STORE (gtk_builder_get_object (
                                       UI_APP.builder, "EF_appuis_treestore")),
                        &app->Iter_fenetre,
                        0, app,
                        -1);
  }
#endif
  
  return app;
}


/**
 * \brief Modifie un appui.
 * \param appui : appui à modifier,
 * \param x : désigne le paramètre à modifier, 0 pour ux à 5 pour rz,
 * \param type_x : le nouveau type,
 * \param p : la variable projet.
 * \return
 *   Succès : true.\n
 *   Échec : false :
 *     - p == NULL,
 *     - appui == NULL,
 *     - type_x inconnu.
 */
bool
EF_appuis_edit (EF_Appui     *appui,
                uint8_t       x,
                Type_EF_Appui type_x,
                Projet       *p)
{
  std::list <EF_Appui *> list_appuis;
  
  std::list <EF_Noeud *> *list_noeuds;
  
  BUGPARAM (p, "%p", p, false)
  BUGPARAM (appui, "%p", appui, false)
  INFO ((type_x == EF_APPUI_LIBRE) || (type_x == EF_APPUI_BLOQUE),
        false,
        (gettext ("Type d'appui %d inconnu.\n"), type_x); )
  
  switch (x)
  {
    case 0 :
    {
      appui->ux = type_x;
      break;
    }
    case 1 :
    {
      appui->uy = type_x;
      break;
    }
    case 2 :
    {
      appui->uz = type_x;
      break;
    }
    case 3 :
    {
      appui->rx = type_x;
      break;
    }
    case 4 :
    {
      appui->ry = type_x;
      break;
    }
    case 5 :
    {
      appui->rz = type_x;
      break;
    }
    default :
    {
      FAILINFO (false,
                (gettext ("Afin de modifier un appui, le paramètre x doit être compris entre 0 et 5 inclu.\n")); )
      return false;
    }
  }
  
#ifdef ENABLE_GTK
  if (UI_APP.builder != NULL)
  {
    gtk_widget_queue_resize (GTK_WIDGET (gtk_builder_get_object (
                                       UI_APP.builder, "EF_appuis_treeview")));
  }
#endif
  
  list_appuis.push_back (appui);
  BUG (_1992_1_1_barres_cherche_dependances (p,
                                             &list_appuis,
                                             NULL,
                                             NULL,
                                             NULL,
                                             NULL,
                                             NULL,
                                             &list_noeuds,
                                             NULL,
                                             NULL,
                                             NULL,
                                             NULL,
                                             false),
       false)
  list_appuis.clear ();
  
  if (list_noeuds != NULL)
  {
    delete list_noeuds;
    BUG (EF_calculs_free (p), false)
  }
  
  return true;
}


/**
 * \brief Renomme un appui.
 * \param appui : appui à renommer,
 * \param nom : le nouveau nom,
 * \param p : la variable projet,
 * \param critique : si true alors BUGMSG, si false alors return.
 * \return
 *   Succès : true.\n
 *   Échec : false :
 *     - p == NULL,
 *     - nom == NULL,
 *     - appui == NULL,
 *     - appui possédant le nouveau nom est déjà existant.
 */
bool
EF_appuis_renomme (EF_Appui   *appui,
                   const char *nom,
                   Projet     *p,
                   bool        critique)
{
  std::list <EF_Appui *>::iterator it;
  
  BUGPARAM (p, "%p", p, false)
  BUGPARAM (nom, "%p", nom, false)
  BUGPARAM (appui, "%p", appui, false)
  
  if (critique)
  {
    INFO (EF_appuis_cherche_nom (p, nom, false) == NULL,
          false,
          (gettext ("L'appui '%s' existe déjà.\n"), nom); )
  }
  else if (EF_appuis_cherche_nom (p, nom, false) != NULL)
  {
    return false;
  }
  
  free (appui->nom);
  BUGCRIT (appui->nom = g_strdup_printf ("%s", nom),
           false,
           (gettext ("Erreur d'allocation mémoire.\n")); )
  
  // On réinsère l'appui au bon endroit
  p->modele.appuis.remove (appui);
  it = p->modele.appuis.begin ();
  while (it != p->modele.appuis.end ())
  {
    EF_Appui *appui_parcours = *it;
    
    if (strcmp (nom, appui_parcours->nom) < 0)
    {
      p->modele.appuis.insert (it, appui);
      
#ifdef ENABLE_GTK
      gtk_list_store_move_before (UI_APP.liste_appuis,
                                  &appui->Iter_liste,
                                  &appui_parcours->Iter_liste);
      if (UI_APP.builder != NULL)
      {
        gtk_tree_store_move_before (UI_APP.appuis,
                                    &appui->Iter_fenetre,
                                    &appui_parcours->Iter_fenetre);
      }
#endif
      break;
    }
    
    ++it;
  }
  if (it == p->modele.appuis.end ())
  {
    p->modele.appuis.push_back (appui);
    
#ifdef ENABLE_GTK
    gtk_list_store_move_before (UI_APP.liste_appuis,
                                &appui->Iter_liste,
                                NULL);
    if (UI_APP.builder != NULL)
    {
      gtk_tree_store_move_before (UI_APP.appuis, &appui->Iter_fenetre, NULL);
    }
#endif
  }
  
#ifdef ENABLE_GTK
  gtk_list_store_set (UI_APP.liste_appuis, &appui->Iter_liste, 0, nom, -1);
  if (UI_APP.builder != NULL)
  {
    GtkTreePath *path;
    
    gtk_widget_queue_resize (GTK_WIDGET (gtk_builder_get_object (
                                       UI_APP.builder, "EF_appuis_treeview")));
    
    // On modifie la position de l'ascenseur pour que la ligne reste visible
    // même si elle sort de la fenêtre.
    path = gtk_tree_model_get_path (
             gtk_tree_view_get_model (GTK_TREE_VIEW (gtk_builder_get_object (
                                       UI_APP.builder, "EF_appuis_treeview"))),
                                    &appui->Iter_fenetre);
    gtk_tree_view_scroll_to_cell (GTK_TREE_VIEW (gtk_builder_get_object (
                                        UI_APP.builder, "EF_appuis_treeview")),
                                  path,
                                  NULL,
                                  FALSE,
                                  0.,
                                  0.);
    gtk_tree_path_free (path);
  }
  
  if (UI_NOE.builder != NULL)
  {
    gtk_widget_queue_resize (GTK_WIDGET (gtk_builder_get_object (
                         UI_NOE.builder, "EF_noeuds_treeview_noeuds_libres")));
    gtk_widget_queue_resize (GTK_WIDGET (gtk_builder_get_object (
                 UI_NOE.builder, "EF_noeuds_treeview_noeuds_intermediaires")));
  }
#endif
  
  return true;
}


/**
 * \brief Supprime l'appui spécifié.
 * \param appui : l'appui à supprimer,
 * \param annule_si_utilise : possibilité d'annuler la suppression si l'appui
 *        est attribué à un noeud. Si l'option est désactivée, les noeuds
 *        possédant l'appui seront modifiés en fonction du paramètre supprime.
 * \param supprime : utilisé uniquement si annule_si_utilise == false.
 *        Si true alors, les noeuds (et les barres et noeuds intermédaires
 *        dépendants) utilisant l'appui seront supprimés. Si false alors les
 *        noeuds deviendront sans appui.
 * \param p : la variable projet.
 * \return
 *   Succès : true.\n
 *   Échec : false :
 *     - p == NULL,
 *     - appui == NULL.
 */
bool
EF_appuis_supprime (EF_Appui *appui,
                    bool      annule_si_utilise,
                    bool      supprime,
                    Projet   *p)
{
  std::list <EF_Appui *> list_appuis;
  
  std::list <EF_Noeud *> *noeuds_suppr;
  
  BUGPARAM (p, "%p", p, false)
  BUGPARAM (appui, "%p", appui, false)
  
  // On vérifie les dépendances.
  list_appuis.push_back (appui);
  BUG (_1992_1_1_barres_cherche_dependances (p,
                                             &list_appuis,
                                             NULL,
                                             NULL,
                                             NULL,
                                             NULL,
                                             NULL,
                                             &noeuds_suppr,
                                             NULL,
                                             NULL,
                                             NULL,
                                             NULL,
                                             false),
       false)
  list_appuis.clear ();
  
  if ((annule_si_utilise) && (!noeuds_suppr->empty ()))
  {
    char *liste;
    
    BUGCRIT (liste = common_selection_noeuds_en_texte (noeuds_suppr),
             false,
             (gettext ("Erreur d'allocation mémoire.\n"));
               delete noeuds_suppr; )

    if (noeuds_suppr->size () == 1)
    {
      FAILINFO (false,
                (gettext ("Impossible de supprimer l'appui car il est utilisé par le noeud %s.\n"),
                          liste);
                  free (liste);
                  delete noeuds_suppr; )
    }
    else
    {
      FAILINFO (false,
                (gettext ("Impossible de supprimer l'appui car il est utilisé par les noeuds %s.\n"),
                          liste);
                  free (liste);
                  delete noeuds_suppr; )
    }
  }
  
  // On enlève l'appui pour les noeuds dépendants (si supprime == false).
  if (!supprime)
  {
    std::list <EF_Noeud *>::iterator it = noeuds_suppr->begin ();
    
    while (it != noeuds_suppr->end ())
    {
      EF_Noeud *noeud = *it;
      
      BUG (EF_noeuds_change_appui (p, noeud, NULL),
           true,
           delete noeuds_suppr; )
      
      ++it;
    }
  }
  else
  {
    BUG (_1992_1_1_barres_supprime_liste (p, noeuds_suppr, NULL),
         true,
         delete noeuds_suppr; )
  }
  
  delete noeuds_suppr;
  
  free (appui->nom);
  appui->nom = NULL;
  
  switch (appui->ux)
  {
    case EF_APPUI_LIBRE :
    case EF_APPUI_BLOQUE :
      break;
    default :
    {
      FAILINFO (false,
                (gettext ("Le type d'appui de %s (%d) est inconnu.\n"),
                          "ux",
                          appui->ux); )
    }
  }
  switch (appui->uy)
  {
    case EF_APPUI_LIBRE :
    case EF_APPUI_BLOQUE :
      break;
    default :
    {
      FAILINFO (false,
                (gettext ("Le type d'appui de %s (%d) est inconnu.\n"),
                          "uy",
                          appui->ux); )
    }
  }
  switch (appui->uz)
  {
    case EF_APPUI_LIBRE :
    case EF_APPUI_BLOQUE :
      break;
    default :
    {
      FAILINFO (false,
                (gettext ("Le type d'appui de %s (%d) est inconnu.\n"),
                          "uz",
                          appui->ux); )
    }
  }
  switch (appui->rx)
  {
    case EF_APPUI_LIBRE :
    case EF_APPUI_BLOQUE :
      break;
    default :
    {
      FAILINFO (false,
                (gettext ("Le type d'appui de %s (%d) est inconnu.\n"),
                          "rx",
                          appui->ux); )
    }
  }
  switch (appui->ry)
  {
    case EF_APPUI_LIBRE :
    case EF_APPUI_BLOQUE :
      break;
    default :
    {
      FAILINFO (false,
                (gettext ("Le type d'appui de %s (%d) est inconnu.\n"),
                          "ry",
                          appui->ux); )
    }
  }
  switch (appui->rz)
  {
    case EF_APPUI_LIBRE :
    case EF_APPUI_BLOQUE :
      break;
    default :
    {
      FAILINFO (false,
                (gettext ("Le type d'appui de %s (%d) est inconnu.\n"),
                          "rz",
                          appui->ux); )
    }
  }
  p->modele.appuis.remove (appui);
  
#ifdef ENABLE_GTK
  gtk_list_store_remove (UI_APP.liste_appuis, &appui->Iter_liste);
  if (UI_APP.builder != NULL)
  {
    gtk_tree_store_remove (UI_APP.appuis, &appui->Iter_fenetre);
  }
#endif
  
  delete appui;
  
  return true;
}


/**
 * \brief Libère l'ensemble des types d'appuis ainsi que la liste les
 *        contenant.
 * \param p : la variable projet.
 * \return
 *   Succès : true.\n
 *   Échec : false :
 *     - p == NULL.
 */
bool
EF_appuis_free (Projet *p)
{
  std::list <EF_Appui *>::iterator it;
  
  BUGPARAM (p, "%p", p, false)
  
  it = p->modele.appuis.begin ();
  while (it != p->modele.appuis.end ())
  {
    EF_Appui *appui = *it;
    
    free (appui->nom);
    delete appui;
    
    ++it;
  }
  p->modele.appuis.clear ();
  
#ifdef ENABLE_GTK
  g_object_unref (UI_APP.liste_appuis);
  g_object_unref (UI_APP.liste_type_appui);
#endif
  
  return true;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
