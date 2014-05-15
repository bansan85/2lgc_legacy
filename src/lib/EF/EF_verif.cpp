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

#include "common_projet.hpp"
#include "common_erreurs.hpp"
#include "common_math.hpp"
#include "common_selection.hpp"
#include "1992_1_1_barres.hpp"
#include "EF_noeuds.hpp"
#include "EF_verif.hpp"


/**
 * \brief Renvoie l'ensemble des noeuds et barres étant connectés aux noeuds et
 *        barres.
 * \param noeuds : noeuds définissant l'ensemble de départ, peut être NULL,
 * \param barres : barres définissant l'ensemble de départ, peut être NULL,
 * \param noeuds_dep : noeuds de l'ensemble du bloc,
 * \param barres_dep : barres de l'ensemble du bloc,
 * \param p : la variable projet.
 * \return
 *   Succès : true.\n
 *   Échec : false
 *     - noeuds_dep = NULL,
 *     - barres_dep = NULL,
 *     - p = NULL.
 */
bool
EF_verif_bloc (std::list <EF_Noeud *> *noeuds,
               std::list <EF_Barre *> *barres,
               std::list <EF_Noeud *> *noeuds_dep,
               std::list <EF_Barre *> *barres_dep,
               Projet                  *p)
{
  std::list <EF_Noeud *> noeuds_todo;
  std::list <EF_Barre *> barres_todo;
  
  std::list <EF_Noeud *>::iterator it1;
  std::list <EF_Barre *>::iterator it2;
  
  BUGPARAM (noeuds_dep, "%p", noeuds_dep, false)
  BUGPARAM (barres_dep, "%p", barres_dep, false)
  BUGPARAM (p, "%p", p, false)
  
  if (noeuds != NULL)
  {
    it1 = noeuds->begin ();
    while (it1 != noeuds->end ())
    {
      if (std::find (noeuds_todo.begin (),
                     noeuds_todo.end (),
                     *it1) == noeuds_todo.end ())
      {
        noeuds_todo.push_back (*it1);
      }
      
      ++it1;
    }
  }
  
  if (barres != NULL)
  {
    it2 = barres->begin ();
    while (it2 != barres->end ())
    {
      if (std::find (barres_todo.begin (),
                     barres_todo.end (),
                     *it2) == barres_todo.end ())
      {
        barres_todo.push_back (*it2);
      }
      
      ++it2;
    }
  }
  
  while ((!noeuds_todo.empty ()) || (!barres_todo.empty ()))
  {
    while (!noeuds_todo.empty ())
    {
      if (std::find (noeuds_dep->begin (),
                     noeuds_dep->end (),
                     *noeuds_todo.begin ()) == noeuds_dep->end ())
      {
        noeuds_dep->push_back (*noeuds_todo.begin ());
      }
      
      it2 = p->modele.barres.begin ();
      while (it2 != p->modele.barres.end ())
      {
        EF_Barre *barre = *it2;
        
        printf ("test brre %d\n", barre->numero);
        if ((barre->noeud_debut == *noeuds_todo.begin ()) ||
            (barre->noeud_fin == *noeuds_todo.begin ()) ||
            (std::find (barre->nds_inter.begin (),
                        barre->nds_inter.end (),
                        *noeuds_todo.begin ()) != barre->nds_inter.end ()))
        {
          if ((std::find (barres_dep->begin (),
                          barres_dep->end (),
                          barre) == barres_dep->end ()) &&
              (std::find (barres_todo.begin (),
                          barres_todo.end (),
                          barre) == barres_todo.end ()))
          {
            printf ("Ajout brre %d\n", barre->numero);
            barres_todo.push_back (barre);
          }
        }
        
        ++it2;
      }
      
      noeuds_todo.erase (noeuds_todo.begin ());
    }
    while (!barres_todo.empty ())
    {
      EF_Barre *barre_parcours = *barres_todo.begin ();
      
      if (std::find (barres_dep->begin (),
                     barres_dep->end (),
                     *barres_todo.begin ()) == barres_dep->end ())
      {
        barres_dep->push_back (*barres_todo.begin ());
      }
      
      if ((std::find (noeuds_dep->begin (), 
                      noeuds_dep->end (),
                      barre_parcours->noeud_debut) == noeuds_dep->end ()) &&
          (std::find (noeuds_todo.begin (),
                      noeuds_todo.end (),
                      barre_parcours->noeud_debut) == noeuds_todo.end ()))
      {
      printf ("début %d\n", barre_parcours->noeud_debut->numero);
        noeuds_todo.push_back (barre_parcours->noeud_debut);
      }
      if ((std::find (noeuds_dep->begin (), 
                      noeuds_dep->end (),
                      barre_parcours->noeud_fin) == noeuds_dep->end ()) &&
          (std::find (noeuds_todo.begin (),
                      noeuds_todo.end (),
                      barre_parcours->noeud_fin) == noeuds_todo.end ()))
      {
      printf ("fin %d\n", barre_parcours->noeud_fin->numero);
        noeuds_todo.push_back (barre_parcours->noeud_fin);
      }
      
      it1 = barre_parcours->nds_inter.begin ();
      while (it1 != barre_parcours->nds_inter.end ())
      {
        if ((std::find (noeuds_dep->begin (),
                        noeuds_dep->end (),
                        *it1) == noeuds_dep->end ()) &&
            (std::find (noeuds_todo.begin (),
                        noeuds_todo.end (),
                        *it1) == noeuds_todo.end ()))
        {
          noeuds_todo.push_back (*it1);
        }
        
        ++it1;
      }
      
      barres_todo.erase (barres_todo.begin ());
    }
  }
  
  return true;
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
 *   Succès : true.\n
 *   Échec : false :
 *     - p == NULL,
 *     - rapport == NULL,
 *     - erreur == NULL,
 *     - erreur d'allocation mémoire.
 */
bool
EF_verif_EF (Projet                      *p,
             std::list <Analyse_Comm *> **rapport,
             uint16_t                    *erreur)
{
  Analyse_Comm *ligne;
  
  std::list <EF_Noeud *>::iterator it;
  std::list <EF_Barre *>::iterator it2;
  
  BUGPARAM (p, "%p", p, false)
  BUGPARAM (rapport, "%p", rapport, false)
  BUGPARAM (erreur, "%p", erreur, false)
  
  *erreur = 0;
  *rapport = new std::list <Analyse_Comm *> ();
  
  // On vérifie si la structure possède au moins une barre (et donc deux
  // noeuds).
  ligne = new Analyse_Comm;
  BUGCRIT (ligne->analyse = g_strdup_printf ("%s",
             gettext ("Structure non vide (minimum 2 noeuds et une barre)")),
           false,
           (gettext ("Erreur d'allocation mémoire.\n"));
             delete ligne; )
  ligne->resultat = 0;
  ligne->commentaire = NULL;
  if (p->modele.noeuds.size () < 2)
  {
    ligne->resultat = 2;
    if (*erreur < ligne->resultat)
    {
      *erreur = ligne->resultat;
    }
    BUGCRIT (ligne->commentaire = g_strdup_printf ("%s (%zu).",
                                    gettext ("Nombre de noeuds insuffisant"),
                                    p->modele.noeuds.size ()),
             false,
             (gettext ("Erreur d'allocation mémoire.\n"));
               free (ligne->analyse);
               delete ligne; )
  }
  if (p->modele.barres.empty ())
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
               false,
               (gettext ("Erreur d'allocation mémoire.\n"));
                 free (ligne->analyse);
                 delete ligne; )
    }
    else
    {
      char *tmp = ligne->commentaire;
      
      BUGCRIT (ligne->commentaire = g_strdup_printf ("%s\n%s",
                                     tmp,
                                     gettext ("Aucune barre n'est existante.")),
               false,
               (gettext ("Erreur d'allocation mémoire.\n"));
                 free (ligne->analyse);
                 delete ligne;
                 free (tmp); )
      free (tmp);
    }
  }
  (*rapport)->push_back (ligne);
  
  // On vérifie si la structure est dans un seul bloc.
  ligne = new Analyse_Comm;
  BUGCRIT (ligne->analyse = g_strdup_printf ("%s",
             gettext ("Structure en un seul bloc")),
           false,
           (gettext ("Erreur d'allocation mémoire.\n"));
             EF_verif_rapport_free (*rapport);
             delete ligne; )
  ligne->resultat = 0;
  ligne->commentaire = NULL;
  if (!p->modele.noeuds.empty ())
  {
    uint32_t i = 1;
    std::list <EF_Noeud *> noeuds;
    std::list <EF_Barre *> barres;
    std::list <EF_Noeud *> tmp;
    std::list <EF_Noeud *> noeuds_tout;
    std::list <EF_Barre *> barres_tout;
    
    tmp.push_back (*p->modele.noeuds.begin ());
    BUG (EF_verif_bloc (&tmp, NULL, &noeuds, &barres, p),
         false,
         EF_verif_rapport_free (*rapport);
         free (ligne->analyse);
         delete ligne; )
    tmp.clear ();
    printf ("%zu %zu\n", noeuds.size (), barres.size ());
    
    // Structure disjointe
    while ((noeuds_tout.size () != p->modele.noeuds.size ()) ||
           (barres_tout.size () != p->modele.barres.size ()))
    {
      char *tmp2;
      
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
               false,
               (gettext ("Erreur d'allocation mémoire.\n"));
                 EF_verif_rapport_free (*rapport);
                 free (ligne->analyse);
                 delete ligne;
                 free (tmp2); )
      free (tmp2);
      
      if (!noeuds.empty ())
      {
        char *noeuds_tmp;
        
        BUG (noeuds_tmp = common_selection_noeuds_en_texte (&noeuds),
             false,
             EF_verif_rapport_free (*rapport);
               free (ligne->commentaire);
               free (ligne->analyse);
               delete ligne; )
        tmp2 = ligne->commentaire;
        BUGCRIT (ligne->commentaire = g_strdup_printf ("%s %s : %s%s",
                                        tmp2,
                                        noeuds.size () == 1 ?
                                          gettext ("noeud") :
                                          gettext ("noeuds"),
                                        noeuds_tmp,
                                        !barres.empty () ? "," : ""),
                 false,
                 (gettext ("Erreur d'allocation mémoire.\n"));
                   EF_verif_rapport_free (*rapport);
                   free (tmp2);
                   free (ligne->analyse);
                   delete ligne;
                   delete noeuds_tmp; )
        free (tmp2);
        delete noeuds_tmp;
      }
      
      if (!barres.empty ())
      {
        char *barres_tmp;
        
        BUG (barres_tmp = common_selection_barres_en_texte (&barres),
             false,
             EF_verif_rapport_free (*rapport);
               free (ligne->commentaire);
               free (ligne->analyse);
               delete ligne; )
        tmp2 = ligne->commentaire;
        BUGCRIT (ligne->commentaire = g_strdup_printf ("%s %s : %s",
                                        tmp2,
                                        barres.size () == 1 ?
                                          gettext ("barre") :
                                          gettext ("barres"),
                                        barres_tmp),
                 false,
                 (gettext ("Erreur d'allocation mémoire.\n"));
                   EF_verif_rapport_free (*rapport);
                   free (tmp2);
                   free (ligne->analyse);
                   delete ligne;
                   delete barres_tmp; )
          free (tmp2);
        delete barres_tmp;
      }
      
      noeuds_tout.splice (noeuds_tout.end (), noeuds);
      barres_tout.splice (barres_tout.end (), barres);
      
      if (noeuds_tout.size () != p->modele.noeuds.size ())
      {
        // On recherche le prochain noeud qui n'est pas dans la liste de ceux
        // étudiés.
        it = p->modele.noeuds.begin ();
        
        while (it != p->modele.noeuds.end ())
        {
          if (std::find (noeuds_tout.begin (),
                         noeuds_tout.end (),
                         *it) == noeuds_tout.end ())
          {
            tmp.push_back (*it);
            BUG (EF_verif_bloc (&tmp, NULL, &noeuds, &barres, p),
                 false,
                 EF_verif_rapport_free (*rapport);
                   free (ligne->commentaire);
                   free (ligne->analyse);
                   delete ligne; )
            tmp.clear ();
            break;
          }
          
          ++it;
        }
      }
      
      i++;
    }
    
    // Un seul bloc
    if (i == 2)
    {
      free (ligne->commentaire);
      ligne->commentaire = NULL;
    }
  }
  (*rapport)->push_back (ligne);
  
  // On vérifie si la structure est bien bloquée en ux, uy et uz.
  ligne = new Analyse_Comm;
  BUGCRIT (ligne->analyse = g_strdup_printf ("%s",
             gettext ("Structure bloquée en déplacement")),
           false,
           (gettext ("Erreur d'allocation mémoire.\n"));
             EF_verif_rapport_free (*rapport);
             delete ligne; )
  ligne->resultat = 0;
  ligne->commentaire = NULL;
  {
    char    *tmp;
    bool     ux = false, uy = false, uz = false;
    std::list <EF_Appui *>::iterator it3;
    
    it3 = p->modele.appuis.begin ();
    while (it3 != p->modele.appuis.end ())
    {
      EF_Appui *appui = *it3;
      
      if (appui->ux != EF_APPUI_LIBRE)
      {
        ux = true;
      }
      if (appui->uy != EF_APPUI_LIBRE)
      {
        uy = true;
      }
      if (appui->uz != EF_APPUI_LIBRE)
      {
        uz = true;
      }
      
      ++it3;
    }
    
    if (!ux)
    {
      if (ligne->commentaire == NULL)
      {
        BUGCRIT (ligne->commentaire = g_strdup_printf ("%s",
                   gettext ("Aucun appui ne permet de bloquer le déplacement vers x.")),
                 false,
                 (gettext ("Erreur d'allocation mémoire.\n"));
                   EF_verif_rapport_free (*rapport);
                   free (ligne->analyse);
                   delete ligne; )
      }
      else
      {
        tmp = ligne->commentaire;
        BUGCRIT (ligne->commentaire = g_strdup_printf ("%s\n%s",
                   tmp,
                   gettext ("Aucun appui ne permet de bloquer le déplacement vers x.")),
                 false,
                 (gettext ("Erreur d'allocation mémoire.\n"));
                   EF_verif_rapport_free (*rapport);
                   free (tmp);
                   free (ligne->analyse);
                   delete ligne; )
        free (tmp);
      }
    }
    if (!uy)
    {
      if (ligne->commentaire == NULL)
      {
        BUGCRIT (ligne->commentaire = g_strdup_printf ("%s",
                   gettext("Aucun appui ne permet de bloquer le déplacement vers y.")),
                 false,
                 (gettext ("Erreur d'allocation mémoire.\n"));
                   EF_verif_rapport_free (*rapport);
                   free (ligne->analyse);
                   delete ligne; )
      }
      else
      {
        tmp = ligne->commentaire;
        BUGCRIT (ligne->commentaire = g_strdup_printf ("%s\n%s",
                   tmp,
                   gettext ("Aucun appui ne permet de bloquer le déplacement vers y.")),
                 false,
                 (gettext ("Erreur d'allocation mémoire.\n"));
                   EF_verif_rapport_free (*rapport);
                   free (tmp);
                   free (ligne->analyse);
                   delete ligne; )
        free (tmp);
      }
    }
    if (!uz)
    {
      if (ligne->commentaire == NULL)
      {
        BUGCRIT (ligne->commentaire = g_strdup_printf ("%s",
                   gettext ("Aucun appui ne permet de bloquer le déplacement vers z.")),
                 false,
                 (gettext ("Erreur d'allocation mémoire.\n"));
                   EF_verif_rapport_free (*rapport);
                   free (ligne->analyse);
                   delete ligne; )
      }
      else
      {
        tmp = ligne->commentaire;
        BUGCRIT (ligne->commentaire = g_strdup_printf ("%s\n%s",
                   tmp,
                   gettext ("Aucun appui ne permet de bloquer le déplacement vers z.")),
                 false,
                 (gettext ("Erreur d'allocation mémoire.\n"));
                   EF_verif_rapport_free (*rapport);
                   free (tmp);
                   free (ligne->analyse);
                   delete ligne; )
        free (tmp);
      }
    }
    
    if ((ux) && (uy) && (uz))
    {
      // On vérifie si au moins un des appuis de chaque ux, uy et uz est bien
      // utilisé.
      ux = false;
      uy = false;
      uz = false;
      
      it = p->modele.noeuds.begin ();
      while ((it != p->modele.noeuds.end ()) &&
             ((!ux) || (!uy) || (!uz)))
      {
        EF_Noeud *noeud = *it;
        
        if (noeud->appui != NULL)
        {
          if (noeud->appui->ux != EF_APPUI_LIBRE)
          {
            ux = true;
          }
          if (noeud->appui->uy != EF_APPUI_LIBRE)
          {
            uy = true;
          }
          if (noeud->appui->uz != EF_APPUI_LIBRE)
          {
            uz = true;
          }
        }
        
        ++it;
      }
      
      if (!ux)
      {
        if (ligne->commentaire == NULL)
        {
          BUGCRIT (ligne->commentaire = g_strdup_printf ("%s",
                     gettext ("Aucun noeud ne permet de bloquer le déplacement vers x.")),
                   false,
                   (gettext ("Erreur d'allocation mémoire.\n"));
                     EF_verif_rapport_free (*rapport);
                     free (ligne->analyse);
                     delete ligne; )
        }
        else
        {
          tmp = ligne->commentaire;
          BUGCRIT (ligne->commentaire = g_strdup_printf ("%s\n%s",
                     tmp,
                     gettext ("Aucun noeud ne permet de bloquer le déplacement vers x.")),
                   false,
                   (gettext ("Erreur d'allocation mémoire.\n"));
                     EF_verif_rapport_free (*rapport);
                     free (tmp);
                     free (ligne->analyse);
                     delete ligne; )
          free (tmp);
        }
      }
      if (!uy)
      {
        if (ligne->commentaire == NULL)
        {
          BUGCRIT (ligne->commentaire = g_strdup_printf ("%s",
                     gettext ("Aucun noeud ne permet de bloquer le déplacement vers y.")),
                   false,
                   (gettext ("Erreur d'allocation mémoire.\n"));
                     EF_verif_rapport_free (*rapport);
                     free (ligne->analyse);
                     delete ligne; )
        }
        else
        {
          tmp = ligne->commentaire;
          BUGCRIT (ligne->commentaire = g_strdup_printf ("%s\n%s",
                     tmp,
                     gettext ("Aucun noeud ne permet de bloquer le déplacement vers y.")),
                   false,
                   (gettext ("Erreur d'allocation mémoire.\n"));
                     EF_verif_rapport_free (*rapport);
                     free (tmp);
                     free (ligne->analyse);
                     delete ligne; )
          free (tmp);
        }
      }
      if (!uz)
      {
        if (ligne->commentaire == NULL)
        {
          BUGCRIT (ligne->commentaire = g_strdup_printf ("%s",
                     gettext ("Aucun noeud ne permet de bloquer le déplacement vers z.")),
                   false,
                   (gettext ("Erreur d'allocation mémoire.\n"));
                     EF_verif_rapport_free (*rapport);
                     free (ligne->analyse);
                     delete ligne; )
        }
        else
        {
          tmp = ligne->commentaire;
          BUGCRIT (ligne->commentaire = g_strdup_printf ("%s\n%s",
                     tmp,
                     gettext ("Aucun noeud ne permet de bloquer le déplacement vers z.")),
                   false,
                   (gettext ("Erreur d'allocation mémoire.\n"));
                     EF_verif_rapport_free (*rapport);
                     free (tmp);
                     free (ligne->analyse);
                     delete ligne; )
          free (tmp);
        }
      }
      
      if ((!ux) || (!uy) || (!uz))
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
  (*rapport)->push_back (ligne);
  
  // Vérification si deux noeuds ont les mêmes coordonnées
  ligne = new Analyse_Comm;
  BUGCRIT (ligne->analyse = g_strdup_printf ("%s",
             gettext ("Vérification des noeuds :\n\t- Noeuds ayant les mêmes coordonnées.")),
           false,
           (gettext ("Erreur d'allocation mémoire.\n"));
             EF_verif_rapport_free (*rapport);
             delete ligne; )
  ligne->resultat = 0;
  ligne->commentaire = NULL;
  it = p->modele.noeuds.begin ();
  while (std::next (it) != p->modele.noeuds.end ())
  {
    EF_Noeud *noeud1 = *it;
    std::list <EF_Noeud *>::iterator it_ = std::next (it);
    EF_Point  point1;
    
    BUG (EF_noeuds_renvoie_position (noeud1, &point1),
         false,
         EF_verif_rapport_free (*rapport);
           free (ligne->analyse);
           delete ligne; )
    
    while (it_ != p->modele.noeuds.end ())
    {
      EF_Noeud *noeud2 = *it_;
      EF_Point  point2;
      
      BUG (EF_noeuds_renvoie_position (noeud2, &point2),
           false,
           EF_verif_rapport_free (*rapport);
             free (ligne->analyse);
             delete ligne; )
      
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
                   false,
                   (gettext ("Erreur d'allocation mémoire.\n"));
                     EF_verif_rapport_free (*rapport);
                     free (ligne->analyse);
                     delete ligne; )
        }
        else
        {
          char *tmp = ligne->commentaire;
          
          BUGCRIT (ligne->commentaire = g_strdup_printf (
                     gettext ("%s\nLes noeuds %d et %d ont les mêmes coordonnées."),
                     tmp,
                     noeud1->numero,
                     noeud2->numero),
                   false,
                   (gettext ("Erreur d'allocation mémoire.\n"));
                     EF_verif_rapport_free (*rapport);
                     free (tmp);
                     free (ligne->analyse);
                     delete ligne; )
          free (tmp);
        }
        
        // On affiche les mêmes noeuds par singleton
        break;
      }
      
      ++it_;
    }
    
    ++it;
  }
  (*rapport)->push_back (ligne);
  
  // Vérification des barres
  ligne = new Analyse_Comm;
  BUGCRIT (ligne->analyse = g_strdup_printf ("%s",
             gettext ("Vérification des barres :\n\t- Longueur nulle.")),
           false,
           (gettext ("Erreur d'allocation mémoire.\n"));
             EF_verif_rapport_free (*rapport);
             delete ligne; )
  ligne->resultat = 0;
  ligne->commentaire = NULL;
  // Détection des barres de longueur nulle.
  it2 = p->modele.barres.begin ();
  while (it2 != p->modele.barres.end ())
  {
    EF_Barre *barre = *it2;
    
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
                 false,
                 (gettext ("Erreur d'allocation mémoire.\n"));
                   EF_verif_rapport_free (*rapport);
                   free (ligne->analyse);
                   delete ligne; )
      }
      else
      {
        char *tmp = ligne->commentaire;
        
        BUGCRIT (ligne->commentaire = g_strdup_printf (
                   gettext ("%s\nLa longueur de la barre %d est nulle."),
                   tmp,
                   barre->numero),
                 false,
                 (gettext ("Erreur d'allocation mémoire.\n"));
                   EF_verif_rapport_free (*rapport);
                   free (tmp);
                   free (ligne->analyse);
                   delete ligne; )
        free (tmp);
      }
    }
    
    ++it2;
  }
  (*rapport)->push_back (ligne);
  
  return true;
}


/**
 * \brief Libère la mémoire utilisée pour un rapport.
 * \param rapport : le rapport à libérer.
 * \return Rien.
 */
void
EF_verif_rapport_free (std::list <Analyse_Comm *> *rapport)
{
  std::list <Analyse_Comm *>::iterator it;
  
  if (rapport == NULL)
    return;
  
  it = rapport->begin ();
  
  while (it != rapport->end ())
  {
    Analyse_Comm *ligne = *it;
    
    free (ligne->analyse);
    free (ligne->commentaire);
    delete ligne;
    
    ++it;
  }
  
  delete rapport;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
