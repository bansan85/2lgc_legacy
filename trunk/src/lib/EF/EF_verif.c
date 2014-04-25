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
#include "common_selection.h"
#include "1992_1_1_barres.h"
#include "EF_noeuds.h"
#include "EF_verif.h"


/**
 * \brief Renvoie l'ensemble des noeuds et barres étant connectés aux noeuds et
 *        barres.
 * \param noeuds : noeuds définissant l'ensemble de départ,
 * \param barres : barres définissant l'ensemble de départ,
 * \param noeuds_dep : noeuds de l'ensemble du bloc,
 * \param barres_dep : barres de l'ensemble du bloc,
 * \param p : la variable projet.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE
 *     - noeuds_dep = NULL,
 *     - barres_dep = NULL,
 *     - p = NULL.
 */
static
gboolean
EF_verif_bloc (GList  *noeuds,
               GList  *barres,
               GList **noeuds_dep,
               GList **barres_dep,
               Projet *p)
{
  GList *noeuds_todo = NULL, *barres_todo = NULL;
  GList *list_parcours;
  
  BUGPARAM (noeuds_dep, "%p", noeuds_dep, FALSE)
  BUGPARAM (barres_dep, "%p", barres_dep, FALSE)
  BUGPARAM (p, "%p", p, FALSE)
  
  *noeuds_dep = NULL;
  *barres_dep = NULL;
  
  list_parcours = noeuds;
  while (list_parcours != NULL)
  {
    if (g_list_find (noeuds_todo, list_parcours->data) == NULL)
    {
      noeuds_todo = g_list_append (noeuds_todo, list_parcours->data);
    }
    list_parcours = g_list_next (list_parcours);
  }
  
  list_parcours = barres;
  while (list_parcours != NULL)
  {
    if (g_list_find (barres_todo, list_parcours->data) == NULL)
    {
      barres_todo = g_list_append (barres_todo, list_parcours->data);
    }
    list_parcours = g_list_next (list_parcours);
  }
  
  while ((noeuds_todo != NULL) || (barres_todo != NULL))
  {
    while (noeuds_todo != NULL)
    {
      if (g_list_find (*noeuds_dep, noeuds_todo->data) == NULL)
      {
        *noeuds_dep = g_list_append (*noeuds_dep, noeuds_todo->data);
      }
      
      list_parcours = p->modele.barres;
      while (list_parcours != NULL)
      {
        EF_Barre *barre = list_parcours->data;
        
        if ((barre->noeud_debut == noeuds_todo->data) ||
            (barre->noeud_fin == noeuds_todo->data) ||
            (g_list_find (barre->nds_inter, noeuds_todo->data) != NULL))
        {
          if ((g_list_find (*barres_dep, barre) == NULL) &&
              (g_list_find (barres_todo, barre) == NULL))
          {
            barres_todo = g_list_append (barres_todo, barre);
          }
        }
        
        list_parcours = g_list_next (list_parcours);
      }
      
      noeuds_todo = g_list_delete_link (noeuds_todo, noeuds_todo);
    }
    while (barres_todo != NULL)
    {
      EF_Barre *barre_parcours = barres_todo->data;
      
      if (g_list_find (*barres_dep, barres_todo->data) == NULL)
      {
        *barres_dep = g_list_append (*barres_dep, barres_todo->data);
      }
      
      if ((g_list_find (*noeuds_dep, barre_parcours->noeud_debut) == NULL) &&
          (g_list_find (noeuds_todo, barre_parcours->noeud_debut) == NULL))
      {
        noeuds_todo = g_list_append (noeuds_todo, barre_parcours->noeud_debut);
      }
      if ((g_list_find (*noeuds_dep, barre_parcours->noeud_fin) == NULL) &&
          (g_list_find (noeuds_todo, barre_parcours->noeud_fin) == NULL))
      {
        noeuds_todo = g_list_append (noeuds_todo, barre_parcours->noeud_fin);
      }
      
      list_parcours = barre_parcours->nds_inter;
      while (list_parcours != NULL)
      {
        if ((g_list_find (*noeuds_dep, list_parcours->data) == NULL) &&
            (g_list_find (noeuds_todo, list_parcours->data) == NULL))
        {
          noeuds_todo = g_list_append (noeuds_todo, list_parcours->data);
        }
        
        list_parcours = g_list_next (list_parcours);
      }
      
      barres_todo = g_list_delete_link (barres_todo, barres_todo);
    }
  }
  
  return TRUE;
}


/**
 * \brief Vérifie toutes les données utilisées dans les calculs :
 *          - Structure non vide (minimum 2 noeuds et une barre),
 *          - Structure en un seul bloc,
 *          - Structure bloquée en ux, uy et uz,
 *          - Deux noeuds sans les mêmes coordonnées,
 *          - Longueur des barres non nulle.
 * \param p : la variable projet,
 * \param rapport : liste d'Analyse_Comm,
 * \param erreur : renvoie le plus haut niveau d'erreur rencontré.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL,
 *     - rapport == NULL,
 *     - erreur == NULL,
 *     - erreur d'allocation mémoire.
 */
gboolean
EF_verif_EF (Projet   *p,
             GList   **rapport,
             uint16_t *erreur)
{
  Analyse_Comm *ligne;
  GList        *list_parcours;
  
  BUGPARAM (p, "%p", p, FALSE)
  BUGPARAM (rapport, "%p", rapport, FALSE)
  BUGPARAM (erreur, "%p", erreur, FALSE)
  
  *erreur = 0;
  *rapport = NULL;
  
  // On vérifie si la structure possède au moins une barre (et donc deux
  // noeuds).
  BUGCRIT (ligne = malloc (sizeof (Analyse_Comm)),
           FALSE,
           (gettext ("Erreur d'allocation mémoire.\n")); )
  BUGCRIT (ligne->analyse = g_strdup_printf ("%s",
             gettext ("Structure non vide (minimum 2 noeuds et une barre)")),
           FALSE,
           (gettext ("Erreur d'allocation mémoire.\n"));
             free (ligne); )
  ligne->resultat = 0;
  ligne->commentaire = NULL;
  if ((p->modele.noeuds == NULL) || (g_list_next (p->modele.noeuds) == NULL))
  {
    ligne->resultat = 2;
    if (*erreur < ligne->resultat)
    {
      *erreur = ligne->resultat;
    }
    BUGCRIT (ligne->commentaire = g_strdup_printf ("%s (%d).",
                                    gettext ("Nombre de noeuds insuffisant"),
                                    g_list_length (p->modele.noeuds)),
             FALSE,
             (gettext ("Erreur d'allocation mémoire.\n"));
               free (ligne->analyse);
               free (ligne); )
  }
  if (p->modele.barres == NULL)
  {
    ligne->resultat = 2;
    if (*erreur < ligne->resultat)
    {
      *erreur = ligne->resultat;
    }
    if (ligne->commentaire == NULL)
    {
      BUGCRIT (ligne->commentaire = g_strdup_printf ("%s",
                 gettext ("Aucune barre n'est existante.")),
               FALSE,
               (gettext ("Erreur d'allocation mémoire.\n"));
                 free (ligne->analyse);
                 free (ligne); )
    }
    else
    {
      char *tmp = ligne->commentaire;
      
      BUGCRIT (ligne->commentaire = g_strdup_printf ("%s\n%s",
                                     tmp,
                                     gettext ("Aucune barre n'est existante.")),
               FALSE,
               (gettext ("Erreur d'allocation mémoire.\n"));
                 free (ligne->analyse);
                 free (ligne);
                 free (tmp); )
      free (tmp);
    }
  }
  *rapport = g_list_append (*rapport, ligne);
  
  // On vérifie si la structure est dans un seul bloc.
  BUGCRIT (ligne = malloc (sizeof (Analyse_Comm)),
           FALSE,
           (gettext ("Erreur d'allocation mémoire.\n"));
             EF_verif_rapport_free (*rapport); )
  BUGCRIT (ligne->analyse = g_strdup_printf ("%s",
             gettext ("Structure en un seul bloc")),
           FALSE,
           (gettext ("Erreur d'allocation mémoire.\n"));
             EF_verif_rapport_free (*rapport);
             free (ligne); )
  ligne->resultat = 0;
  ligne->commentaire = NULL;
  if (p->modele.noeuds != NULL)
  {
    uint32_t i = 1;
    GList   *noeuds, *barres, *tmp = NULL;
    GList   *noeuds_tout = NULL, *barres_tout = NULL;
    
    tmp = g_list_append (tmp, p->modele.noeuds->data);
    BUG (EF_verif_bloc (tmp, NULL, &noeuds, &barres, p),
         FALSE,
         g_list_free (tmp);
         EF_verif_rapport_free (*rapport);
         free (ligne->analyse);
         free (ligne);
         g_list_free (noeuds);
         g_list_free (barres); )
    g_list_free (tmp);
    
    // Structure disjointe
    while ((g_list_length (noeuds_tout) != g_list_length (p->modele.noeuds)) ||
           (g_list_length (barres_tout) != g_list_length (p->modele.barres)))
    {
      char *tmp2;
      
      noeuds_tout = g_list_concat (noeuds_tout, noeuds);
      barres_tout = g_list_concat (barres_tout, barres);
    
      if (i == 1)
      {
        ligne->resultat = 0;
      }
      else
      {
        ligne->resultat = 2;
      }
      if (*erreur < ligne->resultat)
      {
        *erreur = ligne->resultat;
      }
      tmp2 = ligne->commentaire;
      BUGCRIT (ligne->commentaire = g_strdup_printf (gettext("%s%sBloc %d :"),
                                      tmp2 != NULL ? ligne->commentaire : "",
                                      ligne->commentaire != NULL ? "\n" : "",
                                      i),
               FALSE,
               (gettext ("Erreur d'allocation mémoire.\n"));
                 EF_verif_rapport_free (*rapport);
                 free (ligne->analyse);
                 free (ligne);
                 g_list_free (noeuds_tout);
                 g_list_free (barres_tout);
                 free (tmp2); )
      free (tmp2);
      
      if (noeuds != NULL)
      {
        char *noeuds_tmp;
        
        BUG (noeuds_tmp = common_selection_noeuds_en_texte (noeuds),
             FALSE,
             EF_verif_rapport_free (*rapport);
               free (ligne->commentaire);
               free (ligne->analyse);
               free (ligne);
               g_list_free (noeuds_tout);
               g_list_free (barres_tout); )
        tmp2 = ligne->commentaire;
        BUGCRIT (ligne->commentaire = g_strdup_printf ("%s %s : %s%s",
                                        tmp2,
                                        g_list_next (noeuds) == NULL ?
                                          gettext ("noeud") :
                                          gettext ("noeuds"),
                                        noeuds_tmp,
                                        barres != NULL ? "," : ""),
                 FALSE,
                 (gettext ("Erreur d'allocation mémoire.\n"));
                   EF_verif_rapport_free (*rapport);
                   free (tmp2);
                   free (ligne->analyse);
                   free (ligne);
                   g_list_free (noeuds_tout);
                   g_list_free (barres_tout);
                   free (noeuds_tmp); )
        free (tmp2);
        free (noeuds_tmp);
      }
      if (barres != NULL)
      {
        char *barres_tmp;
        
        BUG (barres_tmp = common_selection_barres_en_texte (barres),
             FALSE,
             EF_verif_rapport_free (*rapport);
               free (ligne->commentaire);
               free (ligne->analyse);
               free (ligne);
               g_list_free (noeuds_tout);
               g_list_free (barres_tout); )
        tmp2 = ligne->commentaire;
        BUGCRIT (ligne->commentaire = g_strdup_printf ("%s %s : %s",
                                        tmp2,
                                        g_list_next (barres) == NULL ?
                                          gettext ("barre") :
                                          gettext ("barres"),
                                        barres_tmp),
                 FALSE,
                 (gettext ("Erreur d'allocation mémoire.\n"));
                   EF_verif_rapport_free (*rapport);
                   free (tmp2);
                   free (ligne->analyse);
                   free (ligne);
                   g_list_free (noeuds_tout);
                   g_list_free (barres_tout);
                   free (barres_tmp); )
        free (tmp2);
        free (barres_tmp);
      }
      
      if (g_list_length (noeuds_tout) != g_list_length (p->modele.noeuds))
      {
        // On recherche le prochain noeud qui n'est pas dans la liste de ceux
        // étudiés.
        list_parcours = p->modele.noeuds;
        
        while (list_parcours != NULL)
        {
          if (g_list_find (noeuds_tout, list_parcours->data) == NULL)
          {
            tmp = g_list_append (NULL, list_parcours->data);
            BUG (EF_verif_bloc (tmp, NULL, &noeuds, &barres, p),
                 FALSE,
                 EF_verif_rapport_free (*rapport);
                   free (ligne->commentaire);
                   free (ligne->analyse);
                   free (ligne);
                   g_list_free (noeuds_tout);
                   g_list_free (barres_tout);
                   g_list_free (tmp); )
            g_list_free (tmp);
            break;
          }
          
          list_parcours = g_list_next (list_parcours);
        }
      }
      
      i++;
    }
    
    g_list_free (noeuds_tout);
    g_list_free (barres_tout);
    
    // Un seul bloc
    if (i == 2)
    {
      free (ligne->commentaire);
      ligne->commentaire = NULL;
    }
  }
  *rapport = g_list_append (*rapport, ligne);
  
  // On vérifie si la structure est bien bloquée en ux, uy et uz.
  BUGCRIT (ligne = malloc (sizeof (Analyse_Comm)),
           FALSE,
           (gettext ("Erreur d'allocation mémoire.\n"));
             EF_verif_rapport_free (*rapport); )
  BUGCRIT (ligne->analyse = g_strdup_printf ("%s",
             gettext ("Structure bloquée en déplacement")),
           FALSE,
           (gettext ("Erreur d'allocation mémoire.\n"));
             EF_verif_rapport_free (*rapport);
             free (ligne); )
  ligne->resultat = 0;
  ligne->commentaire = NULL;
  {
    char    *tmp;
    gboolean ux = FALSE, uy = FALSE, uz = FALSE;
    
    list_parcours = p->modele.appuis;
    while (list_parcours != NULL)
    {
      EF_Appui *appui = list_parcours->data;
      
      if (appui->ux != EF_APPUI_LIBRE)
      {
        ux = TRUE;
      }
      if (appui->uy != EF_APPUI_LIBRE)
      {
        uy = TRUE;
      }
      if (appui->uz != EF_APPUI_LIBRE)
      {
        uz = TRUE;
      }
      
      list_parcours = g_list_next (list_parcours);
    }
    
    if (ux == FALSE)
    {
      if (ligne->commentaire == NULL)
      {
        BUGCRIT (ligne->commentaire = g_strdup_printf ("%s",
                   gettext ("Aucun appui ne permet de bloquer le déplacement vers x.")),
                 FALSE,
                 (gettext ("Erreur d'allocation mémoire.\n"));
                   EF_verif_rapport_free (*rapport);
                   free (ligne->analyse);
                   free (ligne); )
      }
      else
      {
        tmp = ligne->commentaire;
        BUGCRIT (ligne->commentaire = g_strdup_printf ("%s\n%s",
                   tmp,
                   gettext ("Aucun appui ne permet de bloquer le déplacement vers x.")),
                 FALSE,
                 (gettext ("Erreur d'allocation mémoire.\n"));
                   EF_verif_rapport_free (*rapport);
                   free (tmp);
                   free (ligne->analyse);
                   free (ligne); )
        free (tmp);
      }
    }
    if (uy == FALSE)
    {
      if (ligne->commentaire == NULL)
      {
        BUGCRIT (ligne->commentaire = g_strdup_printf ("%s",
                   gettext("Aucun appui ne permet de bloquer le déplacement vers y.")),
                 FALSE,
                 (gettext ("Erreur d'allocation mémoire.\n"));
                   EF_verif_rapport_free (*rapport);
                   free (ligne->analyse);
                   free (ligne); )
      }
      else
      {
        tmp = ligne->commentaire;
        BUGCRIT (ligne->commentaire = g_strdup_printf ("%s\n%s",
                   tmp,
                   gettext ("Aucun appui ne permet de bloquer le déplacement vers y.")),
                 FALSE,
                 (gettext ("Erreur d'allocation mémoire.\n"));
                   EF_verif_rapport_free (*rapport);
                   free (tmp);
                   free (ligne->analyse);
                   free (ligne); )
        free (tmp);
      }
    }
    if (uz == FALSE)
    {
      if (ligne->commentaire == NULL)
      {
        BUGCRIT (ligne->commentaire = g_strdup_printf ("%s",
                   gettext ("Aucun appui ne permet de bloquer le déplacement vers z.")),
                 FALSE,
                 (gettext ("Erreur d'allocation mémoire.\n"));
                   EF_verif_rapport_free (*rapport);
                   free (ligne->analyse);
                   free (ligne); )
      }
      else
      {
        tmp = ligne->commentaire;
        BUGCRIT (ligne->commentaire = g_strdup_printf ("%s\n%s",
                   tmp,
                   gettext ("Aucun appui ne permet de bloquer le déplacement vers z.")),
                 FALSE,
                 (gettext ("Erreur d'allocation mémoire.\n"));
                   EF_verif_rapport_free (*rapport);
                   free (tmp);
                   free (ligne->analyse);
                   free (ligne); )
        free (tmp);
      }
    }
    
    if ((ux == TRUE) && (uy == TRUE) && (uz == TRUE))
    {
      // On vérifie si au moins un des appuis de chaque ux, uy et uz est bien
      // utilisé.
      ux = FALSE;
      uy = FALSE;
      uz = FALSE;
      
      list_parcours = p->modele.noeuds;
      while ((list_parcours != NULL) &&
             ((ux == FALSE) || (uy == FALSE) || (uz == FALSE)))
      {
        EF_Noeud *noeud = list_parcours->data;
        
        if (noeud->appui != NULL)
        {
          if (noeud->appui->ux != EF_APPUI_LIBRE)
          {
            ux = TRUE;
          }
          if (noeud->appui->uy != EF_APPUI_LIBRE)
          {
            uy = TRUE;
          }
          if (noeud->appui->uz != EF_APPUI_LIBRE)
          {
            uz = TRUE;
          }
        }
        
        list_parcours = g_list_next (list_parcours);
      }
      
      if (ux == FALSE)
      {
        if (ligne->commentaire == NULL)
        {
          BUGCRIT (ligne->commentaire = g_strdup_printf ("%s",
                     gettext ("Aucun noeud ne permet de bloquer le déplacement vers x.")),
                   FALSE,
                   (gettext ("Erreur d'allocation mémoire.\n"));
                     EF_verif_rapport_free (*rapport);
                     free (ligne->analyse);
                     free (ligne); )
        }
        else
        {
          tmp = ligne->commentaire;
          BUGCRIT (ligne->commentaire = g_strdup_printf ("%s\n%s",
                     tmp,
                     gettext ("Aucun noeud ne permet de bloquer le déplacement vers x.")),
                   FALSE,
                   (gettext ("Erreur d'allocation mémoire.\n"));
                     EF_verif_rapport_free (*rapport);
                     free (tmp);
                     free (ligne->analyse);
                     free (ligne); )
          free (tmp);
        }
      }
      if (uy == FALSE)
      {
        if (ligne->commentaire == NULL)
        {
          BUGCRIT (ligne->commentaire = g_strdup_printf ("%s",
                     gettext ("Aucun noeud ne permet de bloquer le déplacement vers y.")),
                   FALSE,
                   (gettext ("Erreur d'allocation mémoire.\n"));
                     EF_verif_rapport_free (*rapport);
                     free (ligne->analyse);
                     free (ligne); )
        }
        else
        {
          tmp = ligne->commentaire;
          BUGCRIT (ligne->commentaire = g_strdup_printf ("%s\n%s",
                     tmp,
                     gettext ("Aucun noeud ne permet de bloquer le déplacement vers y.")),
                   FALSE,
                   (gettext ("Erreur d'allocation mémoire.\n"));
                     EF_verif_rapport_free (*rapport);
                     free (tmp);
                     free (ligne->analyse);
                     free (ligne); )
          free (tmp);
        }
      }
      if (uz == FALSE)
      {
        if (ligne->commentaire == NULL)
        {
          BUGCRIT (ligne->commentaire = g_strdup_printf ("%s",
                     gettext ("Aucun noeud ne permet de bloquer le déplacement vers z.")),
                   FALSE,
                   (gettext ("Erreur d'allocation mémoire.\n"));
                     EF_verif_rapport_free (*rapport);
                     free (ligne->analyse);
                     free (ligne); )
        }
        else
        {
          tmp = ligne->commentaire;
          BUGCRIT (ligne->commentaire = g_strdup_printf ("%s\n%s",
                     tmp,
                     gettext ("Aucun noeud ne permet de bloquer le déplacement vers z.")),
                   FALSE,
                   (gettext ("Erreur d'allocation mémoire.\n"));
                     EF_verif_rapport_free (*rapport);
                     free (tmp);
                     free (ligne->analyse);
                     free (ligne); )
          free (tmp);
        }
      }
      
      if ((ux == FALSE) || (uy == FALSE) || (uz == FALSE))
      {
        ligne->resultat = 2;
        if (*erreur < ligne->resultat)
        {
          *erreur = ligne->resultat;
        }
      }
    }
    else
    {
      ligne->resultat = 2;
      if (*erreur < ligne->resultat)
      {
        *erreur = ligne->resultat;
      }
    }
  }
  *rapport = g_list_append(*rapport, ligne);
  
  // Vérification si deux noeuds ont les mêmes coordonnées
  BUGCRIT (ligne = malloc (sizeof (Analyse_Comm)),
           FALSE,
           (gettext ("Erreur d'allocation mémoire.\n"));
             EF_verif_rapport_free (*rapport); )
  BUGCRIT (ligne->analyse = g_strdup_printf ("%s",
             gettext ("Vérification des noeuds :\n\t- Noeuds ayant les mêmes coordonnées.")),
           FALSE,
           (gettext ("Erreur d'allocation mémoire.\n"));
             EF_verif_rapport_free (*rapport);
             free (ligne); )
  ligne->resultat = 0;
  ligne->commentaire = NULL;
  list_parcours = p->modele.noeuds;
  while (g_list_next (list_parcours) != NULL)
  {
    EF_Noeud *noeud1 = list_parcours->data;
    GList    *list_parcours2 = g_list_next (list_parcours);
    EF_Point  point1;
    
    BUG (EF_noeuds_renvoie_position (noeud1, &point1),
         FALSE,
         EF_verif_rapport_free (*rapport);
           free (ligne->analyse);
           free (ligne); )
    
    while (list_parcours2 != NULL)
    {
      EF_Noeud *noeud2 = list_parcours2->data;
      EF_Point  point2;
      
      BUG (EF_noeuds_renvoie_position (noeud2, &point2),
           FALSE,
           EF_verif_rapport_free (*rapport);
             free (ligne->analyse);
             free (ligne); )
      
      if ((errrel (m_g (point1.x), m_g (point2.x))) &&
          (errrel (m_g (point1.y), m_g (point2.y))) &&
          (errrel (m_g (point1.z), m_g (point2.z))))
      {
        ligne->resultat = 1;
        if (*erreur < ligne->resultat)
        {
          *erreur = ligne->resultat;
        }
        
        if (ligne->commentaire == NULL)
        {
          BUGCRIT (ligne->commentaire = g_strdup_printf (
                     gettext ("Les noeuds %d et %d ont les mêmes coordonnées."),
                     noeud1->numero,
                     noeud2->numero),
                   FALSE,
                   (gettext ("Erreur d'allocation mémoire.\n"));
                     EF_verif_rapport_free (*rapport);
                     free (ligne->analyse);
                     free (ligne); )
        }
        else
        {
          char *tmp = ligne->commentaire;
          
          BUGCRIT (ligne->commentaire = g_strdup_printf (
                     gettext ("%s\nLes noeuds %d et %d ont les mêmes coordonnées."),
                     tmp,
                     noeud1->numero,
                     noeud2->numero),
                   FALSE,
                   (gettext ("Erreur d'allocation mémoire.\n"));
                     EF_verif_rapport_free (*rapport);
                     free (tmp);
                     free (ligne->analyse);
                     free (ligne); )
          free (tmp);
        }
        
        // On affiche les mêmes noeuds par singleton
        list_parcours2 = NULL;
      }
      
      list_parcours2 = g_list_next (list_parcours2);
    }
    
    list_parcours = g_list_next (list_parcours);
  }
  *rapport = g_list_append (*rapport, ligne);
  
  // Vérification des barres
  BUGCRIT (ligne = malloc (sizeof (Analyse_Comm)),
           FALSE,
           (gettext ("Erreur d'allocation mémoire.\n"));
             EF_verif_rapport_free (*rapport); )
  BUGCRIT (ligne->analyse = g_strdup_printf ("%s",
             gettext ("Vérification des barres :\n\t- Longueur nulle.")),
           FALSE,
           (gettext ("Erreur d'allocation mémoire.\n"));
             EF_verif_rapport_free (*rapport);
             free (ligne); )
  ligne->resultat = 0;
  ligne->commentaire = NULL;
  // Détection des barres de longueur nulle.
  list_parcours = p->modele.barres;
  while (list_parcours != NULL)
  {
    EF_Barre *barre = list_parcours->data;
    
    if (errmoy (EF_noeuds_distance (barre->noeud_debut, barre->noeud_fin),
                ERRMOY_DIST))
    {
      ligne->resultat = 2;
      if (*erreur < ligne->resultat)
      {
        *erreur = ligne->resultat;
      }
      
      if (ligne->commentaire == NULL)
      {
        BUGCRIT (ligne->commentaire = g_strdup_printf (
                   gettext ("La longueur de la barre %d est nulle."),
                   barre->numero),
                 FALSE,
                 (gettext ("Erreur d'allocation mémoire.\n"));
                   EF_verif_rapport_free (*rapport);
                   free (ligne->analyse);
                   free (ligne); )
      }
      else
      {
        char *tmp = ligne->commentaire;
        
        BUGCRIT (ligne->commentaire = g_strdup_printf (
                   gettext ("%s\nLa longueur de la barre %d est nulle."),
                   tmp,
                   barre->numero),
                 FALSE,
                 (gettext ("Erreur d'allocation mémoire.\n"));
                   EF_verif_rapport_free (*rapport);
                   free (tmp);
                   free (ligne->analyse);
                   free (ligne); )
        free (tmp);
      }
    }
    
    list_parcours = g_list_next (list_parcours);
  }
  *rapport = g_list_append (*rapport, ligne);
  
  return TRUE;
}


/**
 * \brief Libère la mémoire utilisée pour un rapport.
 * \param rapport : le rapport à libérer.
 * \return Rien.
 */
void
EF_verif_rapport_free (GList *rapport)
{
  GList *list_parcours = rapport;
  
  while (list_parcours != NULL)
  {
    Analyse_Comm *ligne = list_parcours->data;
    
    free (ligne->analyse);
    free (ligne->commentaire);
    free (ligne);
    
    list_parcours = g_list_next (list_parcours);
  }
  
  g_list_free (rapport);
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
