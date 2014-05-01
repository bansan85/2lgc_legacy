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
#include <string.h>
#include <math.h>

#include "common_projet.hpp"
#include "common_erreurs.hpp"
#include "common_math.hpp"
#include "EF_calculs.hpp"
#include "EF_noeuds.hpp"
#include "1992_1_1_barres.hpp"
#ifdef ENABLE_GTK
#include "common_gtk.hpp"
#include "EF_gtk_appuis.hpp"
#include "common_m3d.hpp"
#endif

/**
 * \brief Initialise la liste des noeuds.
 * \param p : la variable projet.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL.
 */
gboolean
EF_noeuds_init (Projet *p)
{
  BUGPARAM (p, "%p", p, FALSE)
  
  p->modele.noeuds = NULL;
  
  return TRUE;
}


/**
 * \brief Renvoie un point contenant la position du noeud.
 * \param noeud : le noeud à étudier,
 * \param point : la position du noeud.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - noeud == NULL,
 *     - en cas d'erreur d'allocation mémoire.
 */
gboolean
EF_noeuds_renvoie_position (EF_Noeud *noeud,
                            EF_Point *point)
{
  BUGPARAM (noeud, "%p", noeud, FALSE)
  BUGPARAM (point, "%p", point, FALSE)
  
  switch (noeud->type)
  {
    case NOEUD_LIBRE :
    {
      EF_Noeud_Libre *tmp = noeud->data;
      
      if (tmp->relatif != NULL)
      {
        EF_Point p;
        
        BUG (EF_noeuds_renvoie_position (tmp->relatif, &p), FALSE)
        
        point->x = m_add_f (p.x, tmp->x);
        point->y = m_add_f (p.y, tmp->y);
        point->z = m_add_f (p.z, tmp->z);
      }
      else
      {
        point->x = tmp->x;
        point->y = tmp->y;
        point->z = tmp->z;
      }
      
      break;
    }
    case NOEUD_BARRE :
    {
      EF_Noeud_Barre *data = noeud->data;
      EF_Point        point1, point2;
      
      BUG (EF_noeuds_renvoie_position (data->barre->noeud_debut, &point1),
           FALSE)
      BUG (EF_noeuds_renvoie_position (data->barre->noeud_fin, &point2), FALSE)
      
      point->x = m_add_f (point1.x, m_dot_f (m_sub_f (point2.x, point1.x),
                          data->position_relative_barre));
      point->y = m_add_f (point1.y, m_dot_f (m_sub_f (point2.y, point1.y),
                          data->position_relative_barre));
      point->z = m_add_f (point1.z, m_dot_f (m_sub_f (point2.z, point1.z),
                          data->position_relative_barre));
      
      break;
    }
    default :
    {
      FAILCRIT (FALSE,
                (gettext ("Le type de noeud %d est inconnu.\n"),
                          noeud->type); )
      break;
    }
  }
  
  return TRUE;
}


/**
 * \brief Ajouter un noeud à la liste des noeuds en lui attribuant le numéro
 *        suivant le dernier noeud existant.
 * \param p : la variable projet,
 * \param x : position en x,
 * \param y : position en y,
 * \param z : position en z,
 * \param appui : Pointeur vers l'appui, NULL si aucun,
 * \param relatif : Pointeur vers le noeud relatif, NULL si aucun.
 * \return
 *   Succès : Pointeur vers le nouveau noeud.\n
 *   Échec : NULL :
 *     - p == NULL,
 *     - en cas d'erreur d'allocation mémoire.
 */
EF_Noeud *
EF_noeuds_ajout_noeud_libre (Projet   *p,
                             Flottant  x,
                             Flottant  y,
                             Flottant  z,
                             EF_Appui *appui,
                             EF_Noeud *relatif)
{
  EF_Noeud       *noeud_nouveau;
  EF_Noeud_Libre *data;
  
  BUGPARAM (p, "%p", p, NULL)
  BUGCRIT (noeud_nouveau = malloc (sizeof (EF_Noeud)),
           NULL,
           (gettext ("Erreur d'allocation mémoire.\n")); )
  
  BUGCRIT (data = malloc (sizeof (EF_Noeud_Libre)),
           NULL,
           (gettext ("Erreur d'allocation mémoire.\n"));
             free (noeud_nouveau); )
  
  // Trivial
  noeud_nouveau->type = NOEUD_LIBRE;
  noeud_nouveau->data = data;
  data->x = x;
  data->y = y;
  data->z = z;
  data->relatif = relatif;
  
  noeud_nouveau->appui = appui;
    
  if (p->modele.noeuds == NULL)
  {
    noeud_nouveau->numero = 0;
  }
  else
  {
    noeud_nouveau->numero = ((EF_Noeud *) g_list_last (p->modele.noeuds)->data)
                              ->numero + 1;
  }
  
  BUG (EF_calculs_free (p),
       NULL,
       free (data);
         free (noeud_nouveau); )
  
  p->modele.noeuds = g_list_append (p->modele.noeuds, noeud_nouveau);
  
#ifdef ENABLE_GTK
  BUG (m3d_noeud (&UI_M3D, noeud_nouveau),
       NULL,
       free (data);
         free (noeud_nouveau); )
  if (UI_NOE.builder != NULL)
  {
    gtk_tree_store_append (UI_NOE.tree_store_libre,
                           &noeud_nouveau->Iter,
                           NULL);
    gtk_tree_store_set (UI_NOE.tree_store_libre,
                        &noeud_nouveau->Iter,
                        0, noeud_nouveau,
                        -1);
  }
  BUG (m3d_rafraichit (p),
       NULL,
       free (data);
         free (noeud_nouveau); )
#endif
  
  return noeud_nouveau;
}


/**
 * \brief Ajouter un noeud à la liste des noeuds en lui attribuant le numéro
 *        suivant le dernier noeud existant. Ce noeud se situe à l'intérieur
 *        d'une barre et permet la discrétisation.
 * \param p : la variable projet,
 * \param barre : barre qui contiendra le noeud intermédiaire,
 * \param position_relative_barre : position relative à l'intérieur de la barre
 *        (compris entre 0.0 et 1.0),
 * \param appui : Pointeur vers l'appui, NULL si aucun.
 * \return
 *   Succès : Pointeur vers le nouveau noeud.\n
 *   Échec : NULL :
 *     - p == NULL,
 *     - barre == NULL,
 *     - en cas d'erreur d'allocation mémoire.
 */
EF_Noeud *
EF_noeuds_ajout_noeud_barre (Projet   *p,
                             EF_Barre *barre,
                             Flottant  position_relative_barre,
                             EF_Appui *appui)
{
  EF_Noeud       *noeud_nouveau;
  EF_Noeud_Barre *data;
  GList          *liste;
  
  BUGPARAM (p, "%p", p, NULL)
  BUGPARAM (barre, "%p", barre, NULL)
  BUGCRIT (noeud_nouveau = malloc (sizeof (EF_Noeud)),
           NULL,
           (gettext ("Erreur d'allocation mémoire.\n")); )
  
  BUGCRIT (data = malloc (sizeof (EF_Noeud_Barre)),
           NULL,
           (gettext ("Erreur d'allocation mémoire.\n"));
             free (noeud_nouveau); )
  
  noeud_nouveau->type = NOEUD_BARRE;
  noeud_nouveau->data = data;
  data->barre = barre;
  data->position_relative_barre = position_relative_barre;
  
  noeud_nouveau->appui = appui;
    
  if (p->modele.noeuds == NULL)
  {
    noeud_nouveau->numero = 0;
  }
  else
  {
    noeud_nouveau->numero = ((EF_Noeud *) g_list_last (p->modele.noeuds)->data)
                              ->numero + 1;
  }
  
  BUG (EF_calculs_free (p),
       NULL,
       free (data);
         free (noeud_nouveau); )
  
  p->modele.noeuds = g_list_append (p->modele.noeuds, noeud_nouveau);
  
  barre->discretisation_element++;
  
  liste = barre->nds_inter;
  while (liste != NULL)
  {
    EF_Noeud       *noeud = liste->data;
    EF_Noeud_Barre *data2 = noeud->data;
    
    if (m_g (data2->position_relative_barre) < m_g (position_relative_barre))
    {
      liste = g_list_next (liste);
    }
    else
    {
      break;
    }
  }
  
  barre->nds_inter = g_list_insert_before (barre->nds_inter,
                                           liste,
                                           noeud_nouveau);
  
  BUGCRIT (barre->info_EF = realloc (barre->info_EF, sizeof (Barre_Info_EF) * 
                                         (barre->discretisation_element + 1U)),
           NULL,
           (gettext ("Erreur d'allocation mémoire.\n"));
             free (data);
             free (noeud_nouveau); )
  memset (barre->info_EF,
          0,
          sizeof (Barre_Info_EF) * (barre->discretisation_element + 1U));
  
#ifdef ENABLE_GTK
  BUG (m3d_noeud (&UI_M3D, noeud_nouveau),
       NULL,
       free (data);
         free (noeud_nouveau); )
  if (UI_NOE.builder != NULL)
  {
    gtk_tree_store_append (UI_NOE.tree_store_barre,
                           &noeud_nouveau->Iter,
                           NULL);
    gtk_tree_store_set (UI_NOE.tree_store_barre,
                        &noeud_nouveau->Iter,
                        0, noeud_nouveau,
                        -1);
  }
  BUG (m3d_rafraichit (p),
       NULL,
       free (data);
         free (noeud_nouveau); )
#endif
  
  return noeud_nouveau;
}


/**
 * \brief Positionne dans la liste des noeuds le noeud souhaité et le renvoie.
 * \param p : la variable projet,
 * \param numero : le numéro du noeud.
 * \param critique : si TRUE alors BUGMSG, si FALSE alors return
 * \return
 *   Succès : pointeur vers le noeud recherché.\n
 *   Échec : NULL :
 *     - p == NULL,
 *     - noeud introuvable.
 */
EF_Noeud *
EF_noeuds_cherche_numero (Projet      *p,
                          unsigned int numero,
                          gboolean critique)
{
  GList *list_parcours;
  
  BUGPARAM (p, "%p", p, NULL)
  
  list_parcours = p->modele.noeuds;
  while (list_parcours != NULL)
  {
    EF_Noeud *noeud = list_parcours->data;
    
    if (noeud->numero == numero)
    {
      return noeud;
    }
    
    list_parcours = g_list_next (list_parcours);
  }
  
  if (critique)
  {
    FAILCRIT (NULL, (gettext ("Noeud n°%u introuvable.\n"), numero); )
  }
  else
  {
    return NULL;
  }
}


/**
 * \brief Change les coordonnées d'un noeud libre.
 * \param p : la variable projet,
 * \param noeud : noeud à modifier,
 * \param x : la nouvelle coordonnée en x, NAN si pas de modification,
 * \param y : la nouvelle coordonnée en y, NAN si pas de modification,
 * \param z : la nouvelle coordonnée en z, NAN si pas de modification.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL,
 *     - noeud == NULL,
 *     - noeud->type != NOEUD_LIBRE.
 */
gboolean
EF_noeuds_change_pos_abs (Projet   *p,
                          EF_Noeud *noeud,
                          Flottant  x,
                          Flottant  y,
                          Flottant  z)
{
  EF_Noeud_Libre *point;
  
  BUGPARAM (p, "%p", p, FALSE)
  BUGPARAM (noeud, "%p", noeud, FALSE)
  INFO (noeud->type == NOEUD_LIBRE,
        FALSE,
        (gettext ("Le type du noeud est incorrect.\n")); )
  
  point = noeud->data;
  
  if (!isnan (m_g (x)))
  {
    point->x = x;
  }
  if (!isnan (m_g (y)))
  {
    point->y = y;
  }
  if (!isnan (m_g (z)))
  {
    point->z = z;
  }
  
#ifdef ENABLE_GTK
  BUG (m3d_noeud (&UI_M3D, noeud), FALSE)
  BUG (m3d_rafraichit (p), FALSE)
  
  if (UI_NOE.builder != NULL)
  {
    gtk_widget_queue_resize (GTK_WIDGET (gtk_builder_get_object (
                         UI_NOE.builder, "EF_noeuds_treeview_noeuds_libres")));
  }
#endif
  
  BUG (EF_calculs_free (p), FALSE)
  
  return TRUE;
}


/**
 * \brief Change la coordonnée d'un noeud relatif.
 * \param p : la variable projet,
 * \param noeud : noeud à modifier,
 * \param pos : la nouvelle position relative,
 * \return
 *   Succès : TRUE.
 *   Échec : FALSE :
 *     - p == NULL,
 *     - noeud == NULL,
 *     - noeud->type != NOEUD_BARRE,
 *     - pos > 1.0 ou pos < 0.
 */
gboolean
EF_noeuds_change_pos_relat (Projet   *p,
                            EF_Noeud *noeud,
                            Flottant  pos)
{
  EF_Noeud_Barre *info;
  GList          *liste;
  double          avant, apres;
  
  BUGPARAM (p, "%p", p, FALSE)
  BUGPARAM (noeud, "%p", noeud, FALSE)
  INFO (noeud->type == NOEUD_BARRE,
        FALSE,
        (gettext ("Le type du noeud est incorrect.\n")); )
  BUGPARAM (m_g (pos), "%lf", (0. <= m_g (pos)) && (m_g (pos) <= 1.), FALSE)
  
  info = noeud->data;
  
  // On vérifie s'il y a besoin de réorganiser l'ordre des noeuds
  // intermédiaires. On commence par récupérer les bornes qui ne nécessite pas
  // de réorganisation.
  liste = g_list_find (info->barre->nds_inter, noeud);
  BUGCRIT (liste,
           FALSE,
           (gettext ("Impossible\n")); )
  if (g_list_previous (liste) == NULL)
  {
    avant = 0.;
  }
  else
  {
    EF_Noeud       *noeud2 = g_list_previous (liste)->data;
    EF_Noeud_Barre *info2 = noeud2->data;
    
    avant = m_g (info2->position_relative_barre);
  }
  if (g_list_next (liste) == NULL)
  {
    apres = 1.;
  }
  else
  {
    EF_Noeud       *noeud2 = g_list_next (liste)->data;
    EF_Noeud_Barre *info2 = noeud2->data;
    
    apres = m_g (info2->position_relative_barre);
  }
  if ((avant <= m_g (pos)) && (m_g (pos) <= apres))
  {
    info->position_relative_barre = pos;
  }
  // Il est nécessaire de réorganiser
  else
  {
    GList *list_parcours;
    // On enlève l'élément de la liste
    info->barre->nds_inter = g_list_remove_link (info->barre->nds_inter,
                                                 liste);
    
    // On parcours la liste pour l'insérer au bon endroit
    list_parcours = info->barre->nds_inter;
    while (list_parcours != NULL)
    {
      EF_Noeud       *noeud_tmp = list_parcours->data;
      EF_Noeud_Barre *data2 = noeud_tmp->data;
      
      if (m_g (data2->position_relative_barre) < m_g (pos))
      {
        list_parcours = g_list_next (list_parcours);
      }
      else
      {
        break;
      }
    }
    
    info->barre->nds_inter = g_list_insert_before (info->barre->nds_inter,
                                                   list_parcours,
                                                   liste->data);
    
    g_list_free (liste);
  }
  
  info->position_relative_barre = pos;
  
#ifdef ENABLE_GTK
  liste = g_list_append (NULL, noeud);
  
  BUG (m3d_actualise_graphique (p, liste, NULL),
       FALSE,
       g_list_free (liste); )
  g_list_free (liste);
  BUG (m3d_rafraichit (p), FALSE)
  
  
  if (UI_NOE.builder != NULL)
  {
    gtk_widget_queue_resize (GTK_WIDGET (gtk_builder_get_object
                            (UI_NOE.builder, "EF_noeuds_treeview_nds_inter")));
  }
#endif
  
  BUG (EF_calculs_free (p), FALSE)
  
  return TRUE;
}


/**
 * \brief Change l'appui d'un noeud.
 * \param p : la variable projet,
 * \param noeud : noeud à modifier,
 * \param appui : le nouvel appui. NULL signifie aucun appui.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL,
 *     - noeud == NULL.
 */
gboolean
EF_noeuds_change_appui (Projet   *p,
                        EF_Noeud *noeud,
                        EF_Appui *appui)
{
#ifdef ENABLE_GTK
  EF_Appui *appui_old = noeud->appui;
#endif
  
  BUGPARAM (p, "%p", p, FALSE)
  BUGPARAM (noeud, "%p", noeud, FALSE)
  
  noeud->appui = appui;
  
#ifdef ENABLE_GTK
  if (UI_NOE.builder != NULL)
  {
    switch (noeud->type)
    {
      case NOEUD_LIBRE :
      {
        gtk_widget_queue_resize (GTK_WIDGET (gtk_builder_get_object (
                         UI_NOE.builder, "EF_noeuds_treeview_noeuds_libres")));
        break;
      }
      case NOEUD_BARRE :
      {
        gtk_widget_queue_resize (GTK_WIDGET (gtk_builder_get_object (
                             UI_NOE.builder, "EF_noeuds_treeview_nds_inter")));
        break;
      }
      default :
      {
        FAILCRIT (FALSE,
                  (gettext ("Le type de noeud %d est inconnu.\n"),
                            noeud->type); )
        break;
      }
    }
  }
  if (UI_APP.builder != NULL)
  {
    GtkTreeModel *model;
    GtkTreeIter   Iter;
    
    if (gtk_tree_selection_get_selected (GTK_TREE_SELECTION (
                                        gtk_builder_get_object (UI_APP.builder,
                                                 "EF_appuis_treeview_select")),
                                         &model,
                                         &Iter))
    {
      EF_Appui *appui2;
      
      gtk_tree_model_get (model, &Iter, 0, &appui2, -1);
      
      if (((noeud->appui != NULL) &&
           (strcmp (appui2->nom, noeud->appui->nom) == 0)) ||
          ((appui_old != NULL) && (strcmp (appui2->nom, appui_old->nom) == 0)))
      {
        EF_gtk_appuis_select_changed (NULL, p);
      }
    }
  }
#endif
  
  BUG (EF_calculs_free (p), FALSE)
  
  return TRUE;
}


/**
 * \brief Change le noeud relatif d'un noeud.
 * \param p : la variable projet,
 * \param noeud : noeud à modifier,
 * \param relatif : le nouveau noeud relatif, NULL si aucun.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL,
 *     - noeud == NULL,
 *     - noeud->type != NOEUD_LIBRE.
 */
gboolean
EF_noeuds_change_noeud_relatif (Projet   *p,
                                EF_Noeud *noeud,
                                EF_Noeud *relatif)
{
  EF_Noeud_Libre *point;
  GList          *liste_noeuds = NULL, *noeuds_dep = NULL;
  
  BUGPARAM (p, "%p", p, FALSE)
  BUGPARAM (noeud, "%p", noeud, FALSE)
  INFO (noeud->type == NOEUD_LIBRE,
        FALSE,
        (gettext ("Le type du noeud est incorrect.\n")); )
  
  point = noeud->data;
  
  if (point->relatif == relatif)
  {
    return TRUE;
  }
  
  if (relatif != NULL)
  {
    liste_noeuds = g_list_append (NULL, relatif);
    BUG (_1992_1_1_barres_cherche_dependances (p,
                                               NULL,
                                               liste_noeuds,
                                               NULL,
                                               NULL,
                                               NULL,
                                               NULL,
                                               &noeuds_dep,
                                               NULL,
                                               NULL,
                                               FALSE,
                                               TRUE),
         FALSE,
         g_list_free (liste_noeuds); )
    g_list_free (liste_noeuds);
    
    if (g_list_find (noeuds_dep, noeud) != NULL)
    {
      g_list_free (noeuds_dep);
      FAILINFO (FALSE,
            (gettext ("Le noeud %d est déjà dépendant du noeud %d.\n"),
                      relatif->numero,
                      noeud->numero); )
    }
    
    g_list_free (noeuds_dep);
    
    liste_noeuds = g_list_append (NULL, noeud);
    BUG (_1992_1_1_barres_cherche_dependances (p,
                                               NULL,
                                               liste_noeuds,
                                               NULL,
                                               NULL,
                                               NULL,
                                               NULL,
                                               &noeuds_dep,
                                               NULL,
                                               NULL,
                                               FALSE,
                                               TRUE),
         FALSE,
         g_list_free (liste_noeuds); )
    g_list_free (liste_noeuds);
    
    if (g_list_find (noeuds_dep, relatif) != NULL)
    {
      g_list_free (noeuds_dep);
      FAILINFO (FALSE,
            (gettext ("Le noeud %d est déjà dépendant du noeud %d.\n"),
                      noeud->numero,
                      relatif->numero); )
    }
    
    g_list_free (noeuds_dep);
  }
  
  point->relatif = relatif;
  
#ifdef ENABLE_GTK
  liste_noeuds = g_list_append (NULL, noeud);
  BUG (m3d_actualise_graphique (p, liste_noeuds, NULL),
       FALSE,
       g_list_free (liste_noeuds); )
  g_list_free (liste_noeuds);
  BUG(m3d_rafraichit (p), FALSE)
  
  if (UI_NOE.builder != NULL)
  {
    gtk_widget_queue_resize (GTK_WIDGET (gtk_builder_get_object (
                         UI_NOE.builder, "EF_noeuds_treeview_noeuds_libres")));
  }
#endif
  
  BUG (EF_calculs_free (p), FALSE)
  
  return TRUE;
}


/**
 * \brief Renvoie la distance entre deux noeuds.
 * \param p1 : point de départ,
 * \param p2 : point de fin.
 * \return
 *   Succès : distance entre les deux points.\n
 *   Échec : NAN :
 *     - p1 == NULL,
 *     - p2 == NULL.
 */
double
EF_points_distance (EF_Point *p1,
                    EF_Point *p2)
{
  double x, y, z;
  
  // \end{verbatim}\texttt{distance }$= \sqrt{x^2+y^2+z^2}$\begin{verbatim}
  BUGPARAM (p1, "%p", p1, NAN)
  BUGPARAM (p2, "%p", p2, NAN)
  
  x = m_g (p2->x) - m_g (p1->x);
  y = m_g (p2->y) - m_g (p1->y);
  z = m_g (p2->z) - m_g (p1->z);
  
  return sqrt (x * x + y * y + z * z);
}


/**
 * \brief Renvoie la distance entre deux noeuds.
 * \param n1 : noeud de départ,
 * \param n2 : noeud de fin.
 * \return
 *   Succès : distance entre les deux noeuds.\n
 *   Échec : NAN :
 *     - n1 == NULL,
 *     - n2 == NULL.
 */
double
EF_noeuds_distance (EF_Noeud *n1,
                    EF_Noeud *n2)
{
  EF_Point p1, p2;
  double   x, y, z;
  
  // \end{verbatim}\texttt{distance }$= \sqrt{x^2+y^2+z^2}$\begin{verbatim}
  BUGPARAM (n1, "%p", n1, NAN)
  BUGPARAM (n2, "%p", n2, NAN)
  
  BUG (EF_noeuds_renvoie_position (n1, &p1), NAN)
  BUG (EF_noeuds_renvoie_position (n2, &p2), NAN)
  
  x = m_g (p2.x) - m_g (p1.x);
  y = m_g (p2.y) - m_g (p1.y);
  z = m_g (p2.z) - m_g (p1.z);
  
  return sqrt (x * x + y * y + z * z);
}


/**
 * \brief Renvoie la distance entre deux noeuds par retour de fonction et
 *        renvoie la distance entre deux noeuds selon les 3 axes par argument.
 * \param n1 : noeud de départ,
 * \param n2 : noeud de fin,
 * \param x : distance selon l'axe x,
 * \param y : distance selon l'axe y,
 * \param z : distance selon l'axe z.
 * \return
 *   Succès : distance entre les deux points.\n
 *   Échec : NAN :
 *     - n1 == NULL,
 *     - n2 == NULL,
 *     - x == NULL,
 *     - y == NULL,
 *     - z == NULL.
 */
double
EF_noeuds_distance_x_y_z (EF_Noeud *n1,
                          EF_Noeud *n2,
                          double   *x,
                          double   *y,
                          double   *z)
{
  EF_Point p1, p2;
  
  // \end{verbatim}\texttt{distance }$= \sqrt{x^2+y^2+z^2}$\begin{verbatim}
  BUGPARAM (n1, "%p", n1, NAN)
  BUGPARAM (n2, "%p", n2, NAN)
  
  BUG (EF_noeuds_renvoie_position (n1, &p1), NAN)
  BUG (EF_noeuds_renvoie_position (n2, &p2), NAN)

  *x = m_g (p2.x) - m_g (p1.x);
  *y = m_g (p2.y) - m_g (p1.y);
  *z = m_g (p2.z) - m_g (p1.z);
  
  return sqrt ((*x) * (*x) + (*y) * (*y) + (*z) * (*z));
}


/**
 * \brief Fonction permettant de libérer un noeud contenu dans une liste.
 * \param noeud : le noeud à libérer,
 * \param p : la variable projet.
 * \return Rien.
 */
void
EF_noeuds_free_foreach (EF_Noeud *noeud,
                        Projet   *p)
{
  if (noeud == NULL)
  {
    return;
  }
  
  if (noeud->type == NOEUD_BARRE)
  {
    EF_Noeud_Barre *infos = noeud->data;
    uint16_t        i;
    
    infos->barre->nds_inter = g_list_remove (infos->barre->nds_inter, noeud);
    for (i = 0; i <= infos->barre->discretisation_element; i++)
    {
      if (infos->barre->info_EF[i].m_rig_loc != NULL)
      {
        cholmod_free_sparse (&infos->barre->info_EF[i].m_rig_loc,
                             p->calculs.c);
      }
    }
    infos->barre->discretisation_element--;
    BUGPARAM (infos->barre->discretisation_element + 1,
              "%d",
              infos->barre->discretisation_element + 1,
              )
    BUGCRIT (infos->barre->info_EF = realloc (infos->barre->info_EF,
         sizeof (Barre_Info_EF) * (infos->barre->discretisation_element + 1U)),
             ,
             (gettext("Erreur d'allocation mémoire.\n")); )
    memset (infos->barre->info_EF,
            0,
            sizeof (Barre_Info_EF) *
                                  (infos->barre->discretisation_element + 1U));
  }
  
#ifdef ENABLE_GTK
  if (UI_NOE.builder != NULL)
  {
    GtkTreeModel *model;
    
    switch(noeud->type)
    {
      case NOEUD_LIBRE :
      {
        model = GTK_TREE_MODEL (UI_NOE.tree_store_libre);
        break;
      }
      case NOEUD_BARRE :
      {
        model = GTK_TREE_MODEL (UI_NOE.tree_store_barre);
        break;
      }
      default :
      {
        FAILCRIT ( ,
                  (gettext ("Le type de noeud %d est inconnu.\n"),
                            noeud->type); )
        break;
      }
    }
    
    gtk_tree_store_remove (GTK_TREE_STORE (model), &noeud->Iter);
  }
  m3d_noeud_free (&UI_M3D, noeud);
#endif
  
  free (noeud->data);
  free (noeud);
  
  return;
}


/**
 * \brief Libère l'ensemble des noeuds et la liste les contenant.
 * \param p : la variable projet.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL.
 */
gboolean
EF_noeuds_free (Projet *p)
{
  BUGPARAM (p, "%p", p, FALSE)
  
  BUG (EF_calculs_free (p), FALSE)
  
  g_list_foreach (p->modele.noeuds, (GFunc) EF_noeuds_free_foreach, p);
  g_list_free (p->modele.noeuds);
  
  return TRUE;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
