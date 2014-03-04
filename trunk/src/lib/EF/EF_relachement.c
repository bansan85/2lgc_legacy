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

#include "1992_1_1_barres.h"
#include "common_projet.h"
#include "common_erreurs.h"
#include "common_selection.h"
#include "common_math.h"
#include "EF_calculs.h"
#ifdef ENABLE_GTK
#include <gtk/gtk.h>
#include "EF_gtk_relachement.h"
#include "common_gtk.h"
#endif

gboolean
EF_relachement_init (Projet *p)
/**
 * \brief Initialise la liste des relachements.
 * \param p : la variable projet.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL,
 *     - en cas d'erreur d'allocation mémoire.
 */
{
#ifdef ENABLE_GTK
  GtkTreeIter iter;
#endif
  
  BUGMSG (p, FALSE, gettext ("Paramètre %s incorrect.\n"), "projet")
  
  p->modele.relachements = NULL;
  
#ifdef ENABLE_GTK
  UI_REL.liste_relachements = gtk_list_store_new (1, G_TYPE_STRING);
  gtk_list_store_append (UI_REL.liste_relachements, &iter);
  gtk_list_store_set (UI_REL.liste_relachements,
                      &iter,
                      0, gettext ("Aucun"),
                      -1);
#endif
  
  return TRUE;
}


EF_Relachement *
EF_relachement_ajout (Projet             *p,
                      const char         *nom,
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
{
  EF_Relachement *relachement_nouveau;
  EF_Relachement *relachement_tmp;
  GList          *list_parcours;
  
  BUGMSG (p, NULL, gettext ("Paramètre %s incorrect.\n"), "projet")
  BUGMSG(!((rx_debut == EF_RELACHEMENT_LIBRE) &&
           (rx_fin == EF_RELACHEMENT_LIBRE)),
         NULL,
         gettext ("Impossible de relâcher rx simultanément des deux cotés de la barre.\n"))
  BUGMSG (strcmp (gettext ("Aucun"), nom),
          NULL,
          gettext ("Impossible d'utiliser comme nom 'Aucun'.\n"))
  BUGMSG (rx_debut != EF_RELACHEMENT_ELASTIQUE_LINEAIRE || rx_d_data != NULL,
          NULL,
          gettext ("Un relachement élastique linéaire doit être défini avec ses paramètres."))
  BUGMSG (ry_debut != EF_RELACHEMENT_ELASTIQUE_LINEAIRE || ry_d_data != NULL,
          NULL,
          gettext ("Un relachement élastique linéaire doit être défini avec ses paramètres."))
  BUGMSG (rz_debut != EF_RELACHEMENT_ELASTIQUE_LINEAIRE || rz_d_data != NULL,
          NULL,
          gettext ("Un relachement élastique linéaire doit être défini avec ses paramètres."))
  BUGMSG (rx_fin != EF_RELACHEMENT_ELASTIQUE_LINEAIRE || rx_f_data != NULL,
          NULL,
          gettext ("Un relachement élastique linéaire doit être défini avec ses paramètres."))
  BUGMSG (ry_fin != EF_RELACHEMENT_ELASTIQUE_LINEAIRE || ry_f_data != NULL,
          NULL,
          gettext ("Un relachement élastique linéaire doit être défini avec ses paramètres."))
  BUGMSG (rz_fin != EF_RELACHEMENT_ELASTIQUE_LINEAIRE || rz_f_data != NULL,
          NULL,
          gettext ("Un relachement élastique linéaire doit être défini avec ses paramètres."))
  BUGMSG (relachement_nouveau = malloc (sizeof (EF_Relachement)),
          NULL,
          gettext ("Paramètre %s incorrect.\n"), "p")
  
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
  BUGMSG (relachement_nouveau->nom = g_strdup_printf ("%s", nom),
          NULL,
          gettext ("Erreur d'allocation mémoire.\n"))
  
  list_parcours = p->modele.relachements;
  while (list_parcours != NULL)
  {
    relachement_tmp = list_parcours->data;
    
    if (strcmp (relachement_nouveau->nom, relachement_tmp->nom) < 0)
      break;
    
    list_parcours = g_list_next (list_parcours);
  }
  if (list_parcours == NULL)
  {
    p->modele.relachements = g_list_append (p->modele.relachements,
                                            relachement_nouveau);
#ifdef ENABLE_GTK
    gtk_list_store_append (UI_REL.liste_relachements,
                           &relachement_nouveau->Iter_liste);
    if (UI_REL.builder != NULL)
      gtk_tree_store_append (UI_REL.relachements,
                             &relachement_nouveau->Iter_fenetre,
                             NULL);
#endif
  }
  else
  {
    p->modele.relachements = g_list_insert_before (p->modele.relachements,
                                                   list_parcours,
                                                   relachement_nouveau);
#ifdef ENABLE_GTK
    gtk_list_store_insert_before (UI_REL.liste_relachements,
                                  &relachement_nouveau->Iter_liste,
                                  &relachement_tmp->Iter_liste);
    if (UI_REL.builder != NULL)
      gtk_tree_store_insert_before (UI_REL.relachements,
                                    &relachement_nouveau->Iter_fenetre,
                                    NULL,
                                    &relachement_tmp->Iter_fenetre);
#endif
  }
  
#ifdef ENABLE_GTK
  gtk_list_store_set (UI_REL.liste_relachements,
                      &relachement_nouveau->Iter_liste,
                      0, nom,
                      -1);
  if (UI_REL.builder != NULL)
    gtk_tree_store_set (UI_REL.relachements,
                        &relachement_nouveau->Iter_fenetre,
                        0, relachement_nouveau,
                        -1);
#endif
  
  return relachement_nouveau;
}


EF_Relachement *
EF_relachement_cherche_nom (Projet     *p,
                            const char *nom,
                            gboolean    critique)
/**
 * \brief Renvoie le relachement cherché.
 * \param p : la variable projet,
 * \param nom : le nom du relachement.
 * \param critique : si critique = TRUE, BUG est utilisé, return sinon
 * \return
 *   Succès : pointeur vers le relachement recherché.\n
 *   Échec : NULL :
 *     - p == NULL,
 *     - relachement introuvable.
 */
{
  GList *list_parcours;
  
  BUGMSG (p, NULL, gettext ("Paramètre %s incorrect.\n"), "projet")
  
  list_parcours = p->modele.relachements;
  while (list_parcours != NULL)
  {
    EF_Relachement *relachement = list_parcours->data;
    
    if (strcmp (relachement->nom, nom) == 0)
      return relachement;
    
    list_parcours = g_list_next (list_parcours);
  }
  
  if (critique)
    BUGMSG (0, NULL, gettext ("Relachement '%s' introuvable.\n"), nom)
  else
    return NULL;
}


gboolean
EF_relachement_modif (Projet             *p,
                      EF_Relachement     *relachement,
                      const char         *nom,
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
/**
 * \brief Modifie un relâchement.
 * \param p : la variable projet,
 * \param relachement : le relâchement à modifier,
 * \param nom : nouveau nom. mettre NULL pour le nom pour ne pas le modifier,
 * \param rx_debut : relachement de la rotation autour de l'axe x au début,
 *                   UINT_MAX pour ne pas modifier,
 * \param rx_d_data : paramètre additionnel de la rotation en x si nécessaire,
 *                    NULL pour ne pas modifier,
 * \param ry_debut : relachement de la rotation autour de l'axe y au début,
 *                   UINT_MAX pour ne pas modifier,
 * \param ry_d_data : paramètre additionnel de la rotation en y si nécessaire,
 *                    NULL pour ne pas modifier,
 * \param rz_debut : relachement de la rotation autour de l'axe z au début,
 *                   UINT_MAX pour ne pas modifier,
 * \param rz_d_data : paramètre additionnel de la rotation en z si nécessaire,
 *                    NULL pour ne pas modifier,
 * \param rx_fin : relachement de la rotation autour de l'axe x à la fin,
 *                   UINT_MAX pour ne pas modifier,
 * \param rx_f_data : paramètre additionnel de la rotation en x si nécessaire,
 *                    NULL pour ne pas modifier,
 * \param ry_fin : relachement de la rotation autour de l'axe y à la fin,
 *                   UINT_MAX pour ne pas modifier,
 * \param ry_f_data : paramètre additionnel de la rotation en y si nécessaire,
 *                    NULL pour ne pas modifier,
 * \param rz_fin : relachement de la rotation autour de l'axe z à la fin,
 *                   UINT_MAX pour ne pas modifier,
 * \param rz_f_data : paramètre additionnel de la rotation en z si nécessaire.
 *                    NULL pour ne pas modifier,
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL,
 *     - materiau == NULL.
 */
{
  GList *liste_relachement = NULL, *liste_barres_dep;
  
  BUGMSG (p, FALSE, gettext ("Paramètre %s incorrect.\n"), "projet")
  BUGMSG (relachement,
          FALSE,
          gettext ("Paramètre %s incorrect.\n"), "relachement")
  BUGMSG(!(((rx_debut == UINT_MAX ? relachement->rx_debut : rx_debut) ==
             EF_RELACHEMENT_LIBRE) &&
           ((rx_fin == UINT_MAX ? relachement->rx_fin : rx_fin) ==
             EF_RELACHEMENT_LIBRE)),
         FALSE,
         gettext ("Impossible de relâcher rx simultanément des deux cotés de la barre.\n"))
  BUGMSG (((rx_debut == UINT_MAX ? relachement->rx_debut : rx_debut) !=
            EF_RELACHEMENT_ELASTIQUE_LINEAIRE) ||
          ((rx_d_data == NULL ? relachement->rx_d_data : rx_d_data) != NULL),
          FALSE,
          gettext ("Un relachement élastique linéaire doit être défini avec ses paramètres."))
  BUGMSG (((ry_debut == UINT_MAX ? relachement->ry_debut : ry_debut) !=
            EF_RELACHEMENT_ELASTIQUE_LINEAIRE) ||
          ((ry_d_data == NULL ? relachement->ry_d_data : ry_d_data) != NULL),
          FALSE,
          gettext ("Un relachement élastique linéaire doit être défini avec ses paramètres."))
  BUGMSG (((rz_debut == UINT_MAX ? relachement->rz_debut : rz_debut) !=
            EF_RELACHEMENT_ELASTIQUE_LINEAIRE) ||
          ((rz_d_data == NULL ? relachement->rz_d_data : rz_d_data) != NULL),
          FALSE,
          gettext ("Un relachement élastique linéaire doit être défini avec ses paramètres."))
  BUGMSG (((rx_fin == UINT_MAX ? relachement->rx_fin : rx_fin) !=
            EF_RELACHEMENT_ELASTIQUE_LINEAIRE) ||
          ((rx_f_data == NULL ? relachement->rx_f_data : rx_f_data) != NULL),
          FALSE,
          gettext ("Un relachement élastique linéaire doit être défini avec ses paramètres."))
  BUGMSG (((ry_fin == UINT_MAX ? relachement->ry_fin : ry_fin) !=
            EF_RELACHEMENT_ELASTIQUE_LINEAIRE) ||
          ((ry_f_data == NULL ? relachement->ry_f_data : ry_f_data) != NULL),
          FALSE,
          gettext ("Un relachement élastique linéaire doit être défini avec ses paramètres."))
  BUGMSG (((rz_fin == UINT_MAX ? relachement->rz_fin : rz_fin) !=
            EF_RELACHEMENT_ELASTIQUE_LINEAIRE) ||
          ((rz_f_data == NULL ? relachement->rz_f_data : rz_f_data) != NULL),
          FALSE,
          gettext ("Un relachement élastique linéaire doit être défini avec ses paramètres."))
  
  BUGMSG (!((((rx_debut == UINT_MAX ? relachement->rx_debut : rx_debut) ==
               EF_RELACHEMENT_BLOQUE) ||
             ((rx_debut == UINT_MAX ? relachement->rx_debut : rx_debut) ==
               EF_RELACHEMENT_LIBRE)) &&
            (rx_d_data != NULL)),
          FALSE,
          gettext ("Un relachement libre ou bloqué ne doit pas avoir de paramètres."))
  BUGMSG (!((((ry_debut == UINT_MAX ? relachement->ry_debut : ry_debut) ==
               EF_RELACHEMENT_BLOQUE) ||
             ((ry_debut == UINT_MAX ? relachement->ry_debut : ry_debut) ==
               EF_RELACHEMENT_LIBRE)) &&
            (ry_d_data != NULL)),
          FALSE,
          gettext ("Un relachement libre ou bloqué ne doit pas avoir de paramètres."))
  BUGMSG (!((((rz_debut == UINT_MAX ? relachement->rz_debut : rz_debut) ==
               EF_RELACHEMENT_BLOQUE) ||
             ((rz_debut == UINT_MAX ? relachement->rz_debut : rz_debut) ==
               EF_RELACHEMENT_LIBRE)) &&
            (rz_d_data != NULL)),
          FALSE,
          gettext ("Un relachement libre ou bloqué ne doit pas avoir de paramètres."))
  BUGMSG (!((((rx_fin == UINT_MAX ? relachement->rx_fin : rx_fin) ==
               EF_RELACHEMENT_BLOQUE) ||
             ((rx_fin == UINT_MAX ? relachement->rx_fin : rx_fin) ==
               EF_RELACHEMENT_LIBRE)) &&
            (rx_f_data != NULL)),
          FALSE,
          gettext ("Un relachement libre ou bloqué ne doit pas avoir de paramètres."))
  BUGMSG (!((((ry_fin == UINT_MAX ? relachement->ry_fin : ry_fin) ==
               EF_RELACHEMENT_BLOQUE) ||
             ((ry_fin == UINT_MAX ? relachement->ry_fin : ry_fin) ==
               EF_RELACHEMENT_LIBRE)) &&
            (ry_f_data != NULL)),
          FALSE,
          gettext ("Un relachement libre ou bloqué ne doit pas avoir de paramètres."))
  BUGMSG (!((((rz_fin == UINT_MAX ? relachement->rz_fin : rz_fin) ==
               EF_RELACHEMENT_BLOQUE) ||
             ((rz_fin == UINT_MAX ? relachement->rz_fin : rz_fin) ==
               EF_RELACHEMENT_LIBRE)) &&
            (rz_f_data != NULL)),
          FALSE,
          gettext ("Un relachement libre ou bloqué ne doit pas avoir de paramètres."))
  
  liste_relachement = g_list_append (liste_relachement, relachement);
  BUG (_1992_1_1_barres_cherche_dependances (p,
                                             NULL,
                                             NULL,
                                             NULL,
                                             NULL,
                                             liste_relachement,
                                             NULL,
                                             NULL,
                                             &liste_barres_dep,
                                             NULL,
                                             FALSE,
                                             FALSE),
       FALSE)
  g_list_free (liste_relachement);
  if (liste_barres_dep != NULL)
    BUG (EF_calculs_free (p), FALSE)
  g_list_free (liste_barres_dep);
  
  if ((nom != NULL) && (strcmp (relachement->nom, nom) != 0))
  {
    GList *list_parcours;
    
    BUGMSG (!EF_relachement_cherche_nom (p, nom, FALSE),
            FALSE,
            gettext ("Le relâchement %s existe déjà.\n"), nom)
    free (relachement->nom);
    BUGMSG (relachement->nom = g_strdup_printf ("%s", nom),
            FALSE,
            gettext ("Erreur d'allocation mémoire.\n"))
#ifdef ENABLE_GTK
    gtk_list_store_set (UI_REL.liste_relachements,
                        &relachement->Iter_liste,
                        0, relachement->nom,
                        -1);
#endif
    
    // On réinsère le relâchement au bon endroit
    p->modele.relachements = g_list_remove (p->modele.relachements,
                                            relachement);
    list_parcours = p->modele.relachements;
    while (list_parcours != NULL)
    {
      EF_Relachement *relachement_parcours = list_parcours->data;
      
      if (strcmp (relachement->nom, relachement_parcours->nom) < 0)
      {
        p->modele.relachements = g_list_insert_before (p->modele.relachements,
                                                       list_parcours,
                                                       relachement);
        
#ifdef ENABLE_GTK
        gtk_list_store_move_before (UI_REL.liste_relachements,
                                    &relachement->Iter_liste,
                                    &relachement_parcours->Iter_liste);
        if (UI_REL.builder != NULL)
          gtk_tree_store_move_before (UI_REL.relachements,
                                      &relachement->Iter_fenetre,
                                      &relachement_parcours->Iter_fenetre);
#endif
        break;
      }
      
      list_parcours = g_list_next (list_parcours);
    }
    if (list_parcours == NULL)
    {
      p->modele.relachements = g_list_append (p->modele.relachements,
                                              relachement);
      
#ifdef ENABLE_GTK
      gtk_list_store_move_before (UI_REL.liste_relachements,
                                  &relachement->Iter_liste,
                                  NULL);
      if (UI_REL.builder != NULL)
        gtk_tree_store_move_before (UI_REL.relachements,
                                    &relachement->Iter_fenetre,
                                    NULL);
#endif
    }
  
#ifdef ENABLE_GTK
    if (UI_BAR.builder != NULL)
      gtk_widget_queue_resize (GTK_WIDGET (gtk_builder_get_object (
                                       UI_BAR.builder, "EF_barres_treeview")));
#endif
  }
  
#ifdef ENABLE_GTK
#define APPLY_REL(RELA, RELA_DATA, COL) \
  if (RELA != UINT_MAX) \
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
        BUGMSG (relachement->RELA_DATA = \
                 malloc (sizeof (EF_Relachement_Donnees_Elastique_Lineaire)), \
                FALSE, \
                gettext ("Erreur d'allocation mémoire.\n")) \
        memcpy (relachement->RELA_DATA, \
                RELA_DATA, \
                sizeof (EF_Relachement_Donnees_Elastique_Lineaire)); \
      } \
      break; \
    } \
    default : \
    { \
      BUGMSG (NULL, FALSE, gettext ("Le type de relâchement est inconnu.\n")) \
      break; \
    } \
  }
#else
#define APPLY_REL(RELA, RELA_DATA, COL) \
  if (RELA != UINT_MAX) \
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
        BUGMSG (relachement->RELA_DATA = \
                 malloc (sizeof (EF_Relachement_Donnees_Elastique_Lineaire)), \
                FALSE, \
                gettext ("Erreur d'allocation mémoire.\n")) \
        memcpy (relachement->RELA_DATA, \
                RELA_DATA, \
                sizeof (EF_Relachement_Donnees_Elastique_Lineaire)); \
      } \
      break; \
    } \
    default : \
    { \
      BUGMSG (NULL, FALSE, gettext ("Le type de relâchement est inconnu.\n")) \
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
    gtk_widget_queue_resize (GTK_WIDGET (gtk_builder_get_object (
                                 UI_REL.builder, "EF_relachements_treeview")));
#endif
  
  return TRUE;
}


gboolean
EF_relachement_supprime (EF_Relachement *relachement,
                         gboolean        annule_si_utilise,
                         Projet         *p)
/**
 * \brief Supprime le relâchement spécifié.
 * \param relachement : le relâchement à supprimer,
 * \param annule_si_utilise : possibilité d'annuler la suppression si le
 *        relâchement est attribué à une barre. Si l'option est désactivée, les
 *        barres (et les barres et noeuds intermédiaires dépendants) utilisant
 *        le relâchement seront supprimées.
 * \param p : la variable projet.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL,
 *     - section == NULL.
 */
{
  GList *liste_relachements = NULL, *liste_barres_dep;
  
  BUGMSG (p, FALSE, gettext ("Paramètre %s incorrect.\n"), "projet")
  BUGMSG (relachement,
          FALSE,
          gettext ("Paramètre %s incorrect.\n"), "relachement")
  
  // On vérifie les dépendances.
  liste_relachements = g_list_append (liste_relachements, relachement);
  BUG (_1992_1_1_barres_cherche_dependances (p,
                                             NULL,
                                             NULL,
                                             NULL,
                                             NULL,
                                             liste_relachements,
                                             NULL,
                                             NULL,
                                             &liste_barres_dep,
                                             NULL,
                                             FALSE,
                                             FALSE),
       FALSE)
  
  if ((annule_si_utilise) && (liste_barres_dep != NULL))
  {
    char *liste;
    
    liste = common_selection_barres_en_texte (liste_relachements);
    if (g_list_next (liste_relachements) == NULL)
      BUGMSG (NULL,
              FALSE,
              gettext ("Impossible de supprimer la section car elle est utilisée par la barre %s.\n"), liste)
    else
      BUGMSG (NULL,
              FALSE,
              gettext ("Impossible de supprimer la section car elle est utilisée par les barres %s.\n"), liste)
  }
  
  g_list_free (liste_relachements);
  BUG (_1992_1_1_barres_supprime_liste (p, NULL, liste_barres_dep), TRUE)
  g_list_free (liste_barres_dep);
  
  free (relachement->nom);
  free (relachement->rx_d_data);
  free (relachement->ry_d_data);
  free (relachement->rz_d_data);
  free (relachement->rx_f_data);
  free (relachement->ry_f_data);
  free (relachement->rz_f_data);
  p->modele.relachements = g_list_remove (p->modele.relachements, relachement);
  
#ifdef ENABLE_GTK
  gtk_list_store_remove (UI_REL.liste_relachements, &relachement->Iter_liste);
  if (UI_REL.builder != NULL)
    gtk_tree_store_remove (UI_REL.relachements, &relachement->Iter_fenetre);
#endif
  
  free (relachement);
  
  return TRUE;
}


gboolean
EF_relachement_free (Projet *p)
/**
 * \brief Libère l'ensemble des relachements et la liste les contenant.
 * \param p : la variable projet.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL.
 */
{
  BUGMSG (p, FALSE, gettext ("Paramètre %s incorrect.\n"), "projet")
  
  while (p->modele.relachements != NULL)
  {
    EF_Relachement *relachement = p->modele.relachements->data;
    
    p->modele.relachements = g_list_delete_link (p->modele.relachements,
                                                 p->modele.relachements);
    free (relachement->rx_d_data);
    free (relachement->ry_d_data);
    free (relachement->rz_d_data);
    free (relachement->rx_f_data);
    free (relachement->ry_f_data);
    free (relachement->rz_f_data);
    free (relachement->nom);
    
    free (relachement);
  }
  
  BUG (EF_calculs_free (p), FALSE)
  
#ifdef ENABLE_GTK
  g_object_unref (UI_REL.liste_relachements);
#endif
  
  return TRUE;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
