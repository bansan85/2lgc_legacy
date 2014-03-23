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

#ifdef ENABLE_GTK
#include <libintl.h>
#include <locale.h>
#include <gtk/gtk.h>
#include <string.h>

#include "1990_action.h"
#include "1990_ponderations.h"
#include "common_projet.h"
#include "common_erreurs.h"
#include "common_fonction.h"
#include "common_gtk.h"
#include "common_math.h"
#include "EF_noeuds.h"
#include "EF_resultat.h"
#include "EF_gtk_sections.h"


GTK_WINDOW_CLOSE (ef, resultats);


GTK_WINDOW_DESTROY (ef, resultats, );


GTK_WINDOW_KEY_PRESS (ef, resultats);


void
EF_gtk_resultats_notebook_switch (GtkNotebook *notebook,
                                  GtkWidget   *page,
                                  gint         page_num,
                                  Projet      *p)
/**
 * \brief Empêche le changement de page si on clique sur le dernier onglet sans
 *        cliquer sur le bouton d'ajout d'onglet.
 * \param notebook : le composant notebook,
 * \param page : composant désignant la page,
 * \param page_num : numéro de la page,
 * \param p : la variable projet.
 * \return Rien.
 */
{
  if (page_num == gtk_notebook_get_n_pages (notebook) - 1)
    g_signal_stop_emission_by_name (notebook, "switch-page");
  
  return;
}


gboolean
EF_gtk_resultats_remplit_page (Gtk_EF_Resultats_Tableau *res,
                               Projet                   *p)
/**
 * \brief Remplit/actualise la page du treeview via la variable res.
 * \param res : caractéristiques de la page à remplir,
 * \param p : la variable projet.
 * \return TRUE.\n
 *   Echec : FALSE,
 *     - p == NULL,
 *     - res == FALSE,
 *     - interface graphique non initialisée.
 */
{
  unsigned int i;
  GList       *actions = NULL;
  Action      *action;
  GList       *comb = NULL;
  
  BUGPARAMCRIT (p, "%p", p, FALSE)
  BUGPARAMCRIT (res, "%p", res, FALSE)
  BUGCRIT (UI_RES.builder,
           FALSE,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Résultats");)
  
  switch (gtk_combo_box_get_active (UI_RES.combobox))
  {
    // Actions élémentaires
    case 0 :
    {
      if (gtk_combo_box_get_active (UI_RES.combobox_cas) == -1)
      {
        gtk_list_store_clear (res->list_store);
        return TRUE;
      }
      BUG (action = g_list_nth_data (p->actions,
                                     gtk_combo_box_get_active (
                                                         UI_RES.combobox_cas)),
           FALSE)
      actions = g_list_append (actions, action);
      break;
    }
    // Combinaisons
    case 1 :
    {
      if ((gtk_combo_box_get_active (UI_RES.combobox_cas) == -1) ||
          (gtk_combo_box_get_active (UI_RES.combobox_ponderations) == -1))
      {
        gtk_list_store_clear (res->list_store);
        return TRUE;
      }
      
      // On cherche la combinaison à afficher.
      switch (gtk_combo_box_get_active (UI_RES.combobox_cas))
      {
        case 0 :
        {
          comb = p->combinaisons.elu_equ;
          break;
        }
        case 1 :
        {
          comb = p->combinaisons.elu_str;
          break;
        }
        case 2 :
        {
          comb = p->combinaisons.elu_geo;
          break;
        }
        case 3 :
        {
          comb = p->combinaisons.elu_fat;
          break;
        }
        case 4 :
        {
          comb = p->combinaisons.elu_acc;
          break;
        }
        case 5 :
        {
          comb = p->combinaisons.elu_sis;
          break;
        }
        case 6 :
        {
          comb = p->combinaisons.els_car;
          break;
        }
        case 7 :
        {
          comb = p->combinaisons.els_freq;
          break;
        }
        case 8 :
        {
          comb = p->combinaisons.els_perm;
          break;
        }
        default :
        {
          FAILCRIT (FALSE,
                    (gettext ("Paramètre %s incorrect.\n"), "combobox_cas");)
          break;
        }
      }
      comb = g_list_nth (comb,
                      gtk_combo_box_get_active (UI_RES.combobox_ponderations));
      
      BUG (action = _1990_action_ponderation_resultat (comb->data, p), FALSE)
      actions = g_list_append (actions, action);
      break;
    }
    case 2 :
    {
      if ((gtk_combo_box_get_active (UI_RES.combobox_cas) == -1) ||
          (gtk_combo_box_get_active (UI_RES.combobox_ponderations) == -1))
      {
        gtk_list_store_clear (res->list_store);
        return TRUE;
      }
      
      // On cherche la combinaison à afficher.
      switch (gtk_combo_box_get_active (UI_RES.combobox_cas))
      {
        case 0 :
        {
          comb = p->combinaisons.elu_equ;
          break;
        }
        case 1 :
        {
          comb = p->combinaisons.elu_str;
          break;
        }
        case 2 :
        {
          comb = p->combinaisons.elu_geo;
          break;
        }
        case 3 :
        {
          comb = p->combinaisons.elu_fat;
          break;
        }
        case 4 :
        {
          comb = p->combinaisons.elu_acc;
          break;
        }
        case 5 :
        {
          comb = p->combinaisons.elu_sis;
          break;
        }
        case 6 :
        {
          comb = p->combinaisons.els_car;
          break;
        }
        case 7 :
        {
          comb = p->combinaisons.els_freq;
          break;
        }
        case 8 :
        {
          comb = p->combinaisons.els_perm;
          break;
        }
        default :
        {
          FAILCRIT (FALSE,
                    (gettext ("Paramètre %s incorrect.\n"), "combobox_cas");)
          break;
        }
      }
      
      if (gtk_combo_box_get_active (UI_RES.combobox_ponderations) == 0)
      {
        GList *list_parcours;
        
        list_parcours = comb;
        while (list_parcours != NULL)
        {
          BUG (action = _1990_action_ponderation_resultat (list_parcours->data,
                                                           p),
               FALSE,
               g_list_free (actions);)
          actions = g_list_append (actions, action);
          list_parcours = g_list_next (list_parcours);
        }
      }
      break;
    }
    default :
    {
      FAILCRIT (FALSE,
                (gettext ("Paramètre %s incorrect.\n"), "combobox");)
      break;
    }
  }
  
  #define FREE_ALL { \
    if (gtk_combo_box_get_active (UI_RES.combobox) == 0) \
      g_list_free (actions); \
    else \
    { \
      GList *list_parcours3 = actions; \
      \
      while (list_parcours3 != NULL) \
      { \
        action = list_parcours3->data; \
        BUG (_1990_action_fonction_free (p, action), FALSE) \
        BUG (_1990_action_ponderation_resultat_free_calculs (action), FALSE) \
        free (action); \
        \
        list_parcours3 = g_list_next (list_parcours3); \
      } \
    } \
  }
  
  gtk_list_store_clear (res->list_store);
  
  switch (res->col_tab[1])
  {
    case COLRES_NUM_NOEUDS :
    {
      GList *list_parcours = p->modele.noeuds;
      
      i = 0;
      
      while (list_parcours != NULL)
      {
        EF_Noeud *noeud = list_parcours->data;
        gboolean  ok;
        
        switch (res->filtre)
        {
          case FILTRE_AUCUN :
          {
            ok = TRUE;
            break;
          }
          case FILTRE_NOEUD_APPUI :
          {
            if (noeud->appui == NULL)
              ok = FALSE;
            else
              ok = TRUE;
            break;
          }
          default :
          {
            FAILCRIT (FALSE,
                      (gettext ("Le filtre %d est inconnu.\n"), res->filtre);
                        FREE_ALL)
            break;
          }
        }
        
        if (ok)
        {
          GtkTreeIter  Iter;
          unsigned int j;
          char        *tmp_double;
          char         tmp_double30[30];
          
          gtk_list_store_append (res->list_store, &Iter);
          for (j = 1; j <= res->col_tab[0]; j++)
          {
            switch (res->col_tab[j])
            {
              case COLRES_NUM_NOEUDS :
              {
                gtk_list_store_set (res->list_store,
                                    &Iter,
                                    j - 1, noeud->numero,
                                    -1);
                break;
              }
              case COLRES_NOEUDS_X :
              {
                EF_Point point;
                
                BUG (EF_noeuds_renvoie_position (noeud, &point), FALSE, FREE_ALL)
                conv_f_c (point.x, tmp_double30, DECIMAL_DISTANCE);
                
                gtk_list_store_set (res->list_store,
                                    &Iter,
                                    j - 1, tmp_double30,
                                    -1);
                
                break;
              }
              case COLRES_NOEUDS_Y :
              {
                EF_Point point;
                
                BUG (EF_noeuds_renvoie_position (noeud, &point), FALSE, FREE_ALL)
                conv_f_c (point.y, tmp_double30, DECIMAL_DISTANCE);
                
                gtk_list_store_set (res->list_store,
                                    &Iter,
                                    j - 1, tmp_double30,
                                    -1);
                
                break;
              }
              case COLRES_NOEUDS_Z :
              {
                EF_Point point;
                
                BUG (EF_noeuds_renvoie_position (noeud, &point), FALSE, FREE_ALL)
                conv_f_c (point.z, tmp_double30, DECIMAL_DISTANCE);
                
                gtk_list_store_set (res->list_store,
                                    &Iter,
                                    j - 1, tmp_double30,
                                    -1);
                
                break;
              }
              case COLRES_REACTION_APPUI_FX :
              case COLRES_REACTION_APPUI_FY:
              case COLRES_REACTION_APPUI_FZ :
              case COLRES_REACTION_APPUI_MX :
              case COLRES_REACTION_APPUI_MY :
              case COLRES_REACTION_APPUI_MZ :
              {
                BUG (EF_resultat_noeud_reaction_appui (actions,
                                                       noeud,
                                                       res->col_tab[j] -
                                                        COLRES_REACTION_APPUI_FX,
                                                       p,
                                                       &tmp_double,
                                                       NULL,
                                                       NULL),
                     FALSE,
                     FREE_ALL)
                gtk_list_store_set (res->list_store,
                                    &Iter,
                                    j - 1, tmp_double,
                                    -1);
                free (tmp_double);
                break;
              }
              case COLRES_DEPLACEMENT_UX :
              case COLRES_DEPLACEMENT_UY :
              case COLRES_DEPLACEMENT_UZ :
              case COLRES_DEPLACEMENT_RX :
              case COLRES_DEPLACEMENT_RY :
              case COLRES_DEPLACEMENT_RZ :
              {
                BUG (EF_resultat_noeud_deplacement (actions,
                                                    noeud,
                                                    res->col_tab[j] -
                                                      COLRES_DEPLACEMENT_UX,
                                                    p,
                                                    &tmp_double,
                                                    NULL,
                                                    NULL),
                     FALSE,
                     FREE_ALL)
                gtk_list_store_set (res->list_store,
                                    &Iter,
                                    j - 1, tmp_double,
                                    -1);
                free (tmp_double);
                break;
              }
              case COLRES_NUM_BARRES :
              case COLRES_BARRES_LONGUEUR :
              case COLRES_BARRES_PIXBUF_N :
              case COLRES_BARRES_PIXBUF_TY :
              case COLRES_BARRES_PIXBUF_TZ :
              case COLRES_BARRES_PIXBUF_MX :
              case COLRES_BARRES_PIXBUF_MY :
              case COLRES_BARRES_PIXBUF_MZ :
              case COLRES_BARRES_DESC_N :
              case COLRES_BARRES_DESC_TY :
              case COLRES_BARRES_DESC_TZ :
              case COLRES_BARRES_DESC_MX :
              case COLRES_BARRES_DESC_MY :
              case COLRES_BARRES_DESC_MZ :
              case COLRES_BARRES_EQ_N :
              case COLRES_BARRES_EQ_TY :
              case COLRES_BARRES_EQ_TZ :
              case COLRES_BARRES_EQ_MX :
              case COLRES_BARRES_EQ_MY :
              case COLRES_BARRES_EQ_MZ :
              case COLRES_DEF_PIXBUF_UX :
              case COLRES_DEF_PIXBUF_UY :
              case COLRES_DEF_PIXBUF_UZ :
              case COLRES_DEF_PIXBUF_RX :
              case COLRES_DEF_PIXBUF_RY :
              case COLRES_DEF_PIXBUF_RZ :
              case COLRES_DEF_DESC_UX :
              case COLRES_DEF_DESC_UY :
              case COLRES_DEF_DESC_UZ :
              case COLRES_DEF_DESC_RX :
              case COLRES_DEF_DESC_RY :
              case COLRES_DEF_DESC_RZ :
              case COLRES_DEF_UX :
              case COLRES_DEF_UY :
              case COLRES_DEF_UZ :
              case COLRES_DEF_RX :
              case COLRES_DEF_RY :
              case COLRES_DEF_RZ :
              {
                FAILCRIT (FALSE,
                          (gettext ("La colonne des résultats %d ne peut être appliquée aux noeuds."),
                                    res->col_tab[j]);
                            FREE_ALL)
                break;
              }
              default :
              {
                FAILCRIT (FALSE,
                          (gettext ("La colonne des résultats %d est inconnue.\n"),
                                    res->col_tab[j]);
                            FREE_ALL)
                break;
              }
            }
          }
          gtk_list_store_set (res->list_store, &Iter, res->col_tab[0], "", -1);
        }
        
        i++;
        list_parcours = g_list_next (list_parcours);
      }
      break;
    }
     
    case COLRES_NUM_BARRES :
    {
      GList *list_parcours = p->modele.barres;
      
      i = 0;
      
      while (list_parcours != NULL)
      {
        EF_Barre *barre = list_parcours->data;
        gboolean  ok;
        
        switch (res->filtre)
        {
          case FILTRE_AUCUN :
          {
            ok = TRUE;
            break;
          }
          case FILTRE_NOEUD_APPUI :
          {
            FAILCRIT (FALSE,
                      (gettext ("Le filtre %d ne peut être appliqué aux barres.\n"),
                                res->filtre);
                        FREE_ALL)
            break;
          }
          default :
          {
            FAILCRIT (FALSE,
                      (gettext ("Le filtre %d est inconnu.\n"),
                                res->filtre);
                        FREE_ALL)
            break;
          }
        }
        
        if (ok)
        {
          GtkTreeIter  Iter;
          unsigned int j;
          
          gtk_list_store_append (res->list_store, &Iter);
          
          for (j = 1; j <= res->col_tab[0]; j++)
          {
            Fonction comb_min, comb_max;
            Fonction f_min, f_max;
            char    *tmp1 = NULL, *tmp2 = NULL;
            char    *tmp = NULL;
            GList   *liste = NULL, *list_parcours2;
            GList   *converti = NULL;
            
    #define FREE_ALL2 free (comb_min.troncons); \
    free (comb_max.troncons); \
    free (f_min.troncons); \
    free (f_max.troncons); \
    free (tmp); \
    free (tmp1); \
    free (tmp2); \
    g_list_free (liste); \
    g_list_free (converti);
            
            comb_min.troncons = NULL;
            comb_max.troncons = NULL;
            f_min.troncons = NULL;
            f_max.troncons = NULL;
            
            switch (res->col_tab[j])
            {
              case COLRES_NUM_NOEUDS :
              case COLRES_NOEUDS_X :
              case COLRES_NOEUDS_Y :
              case COLRES_NOEUDS_Z :
              case COLRES_REACTION_APPUI_FX :
              case COLRES_REACTION_APPUI_FY:
              case COLRES_REACTION_APPUI_FZ :
              case COLRES_REACTION_APPUI_MX :
              case COLRES_REACTION_APPUI_MY :
              case COLRES_REACTION_APPUI_MZ :
              case COLRES_DEPLACEMENT_UX :
              case COLRES_DEPLACEMENT_UY :
              case COLRES_DEPLACEMENT_UZ :
              case COLRES_DEPLACEMENT_RX :
              case COLRES_DEPLACEMENT_RY :
              case COLRES_DEPLACEMENT_RZ :
              {
                FAILCRIT (FALSE,
                          (gettext ("La colonne des résultats %d ne peut être appliquée aux barres."),
                                     res->col_tab[j]);
                            FREE_ALL
                              FREE_ALL2)
                break;
              }
              case COLRES_NUM_BARRES :
              {
                gtk_list_store_set (res->list_store,
                                    &Iter,
                                    j - 1, barre->numero,
                                    -1);
                break;
              }
              case COLRES_BARRES_LONGUEUR :
              {
                gtk_list_store_set (res->list_store,
                                    &Iter,
                                    j - 1,
                                    EF_noeuds_distance (barre->noeud_debut,
                                                        barre->noeud_fin),
                                    -1);
                break;
              }
              case COLRES_BARRES_PIXBUF_N :
              case COLRES_BARRES_PIXBUF_TY :
              case COLRES_BARRES_PIXBUF_TZ :
              case COLRES_BARRES_PIXBUF_MX :
              case COLRES_BARRES_PIXBUF_MY :
              case COLRES_BARRES_PIXBUF_MZ :
              {
                GdkPixbuf *pixbuf;
                
                list_parcours2 = actions;
                while (list_parcours2 != NULL)
                {
                  Action *action2 = list_parcours2->data;
                  
                  liste = g_list_append (
                            liste,
                            _1990_action_efforts_renvoie (action2,
                                                          res->col_tab[j] -
                                                          COLRES_BARRES_PIXBUF_N,
                                                          i));
                  
                  list_parcours2 = g_list_next (list_parcours2);
                }
                
                pixbuf = common_fonction_dessin (
                           liste,
                           200,
                           50,
                           res->col_tab[j] < COLRES_BARRES_PIXBUF_MX ?
                             DECIMAL_FORCE :
                             DECIMAL_MOMENT);
                gtk_list_store_set (res->list_store, &Iter, j - 1, pixbuf, -1);
                
                g_object_unref (pixbuf);
                
                break;
              }
              case COLRES_BARRES_DESC_N :
              case COLRES_BARRES_DESC_TY :
              case COLRES_BARRES_DESC_TZ :
              case COLRES_BARRES_DESC_MX :
              case COLRES_BARRES_DESC_MY :
              case COLRES_BARRES_DESC_MZ :
              {
                list_parcours2 = actions;
                
                if (g_list_next(actions) != NULL)
                {
                  while (list_parcours2 != NULL)
                  {
                    Action *action2 = list_parcours2->data;
                    
                    liste = g_list_append (
                              liste,
                              _1990_action_efforts_renvoie (action2,
                                                            res->col_tab[j] -
                                                            COLRES_BARRES_DESC_N,
                                                            i));
                    
                    list_parcours2 = g_list_next (list_parcours2);
                  }
                  
                  if (common_fonction_renvoie_enveloppe (liste,
                                                         &f_min,
                                                         &f_max,
                                                         &comb_min,
                                                         &comb_max) == FALSE)
                    BUGCRIT (tmp = g_strdup_printf (gettext ("Erreur")),
                             FALSE,
                             (gettext ("Erreur d'allocation mémoire.\n"));
                               FREE_ALL
                               FREE_ALL2)
                  else
                  {
                    tmp1 = common_fonction_affiche_caract(
                             &f_min,
                             DECIMAL_DISTANCE,
                             res->col_tab[j] < COLRES_BARRES_DESC_MX ?
                               DECIMAL_FORCE :
                               DECIMAL_MOMENT);
                    tmp2 = common_fonction_affiche_caract (
                             &f_max,
                             DECIMAL_DISTANCE,
                             res->col_tab[j] < COLRES_BARRES_DESC_MX ?
                               DECIMAL_FORCE :
                               DECIMAL_MOMENT);
                    
                    BUGCRIT (tmp = g_strdup_printf (gettext ("Enveloppe supérieure :\n%s\nEnveloppe inférieure :\n%s"),
                                                    tmp2,
                                                    tmp1),
                             FALSE,
                             (gettext ("Erreur d'allocation mémoire.\n"));
                               FREE_ALL
                               FREE_ALL2)
                  }
                }
                else
                {
                  Action *action2 = list_parcours2->data;
                  
                  tmp = common_fonction_affiche_caract (
                          _1990_action_efforts_renvoie (
                            action2,
                            res->col_tab[j] - COLRES_BARRES_DESC_N,
                            i),
                          DECIMAL_DISTANCE,
                          res->col_tab[j] < COLRES_BARRES_DESC_MX ?
                            DECIMAL_FORCE :
                            DECIMAL_MOMENT);
                }
                gtk_list_store_set (res->list_store, &Iter, j - 1, tmp, -1);
                
                break;
              }
              case COLRES_BARRES_EQ_N :
              case COLRES_BARRES_EQ_TY :
              case COLRES_BARRES_EQ_TZ :
              case COLRES_BARRES_EQ_MX :
              case COLRES_BARRES_EQ_MY :
              case COLRES_BARRES_EQ_MZ :
              {
                list_parcours2 = actions;
                
                if (g_list_next (actions) != NULL)
                {
                  while (list_parcours2 != NULL)
                  {
                    Action *action2 = list_parcours2->data;
                    
                    liste = g_list_append (
                              liste,
                              _1990_action_efforts_renvoie (
                                action2,
                                res->col_tab[j] - COLRES_BARRES_EQ_N,
                                i));
                    
                    list_parcours2 = g_list_next (list_parcours2);
                  }
                  
                  if (common_fonction_renvoie_enveloppe (liste,
                                                         &f_min,
                                                         &f_max,
                                                         &comb_min,
                                                         &comb_max) == FALSE)
                    BUGCRIT (tmp = g_strdup_printf (gettext ("Erreur")),
                             FALSE,
                             (gettext ("Erreur d'allocation mémoire.\n"));
                               FREE_ALL
                               FREE_ALL2)
                  else
                  {
                    BUG (common_fonction_conversion_combinaisons (&comb_min,
                                                                  comb,
                                                                  &converti),
                         FALSE,
                         FREE_ALL
                           FREE_ALL2)
                    tmp1 = common_fonction_renvoie (
                             &f_min,
                             converti,
                             res->col_tab[j] < COLRES_BARRES_EQ_MX ?
                               DECIMAL_FORCE :
                               DECIMAL_MOMENT);
                    g_list_free (converti);
                    converti = NULL;
                    BUG (common_fonction_conversion_combinaisons (&comb_max,
                                                                  comb,
                                                                  &converti),
                         FALSE,
                         FREE_ALL
                           FREE_ALL2)
                    tmp2 = common_fonction_renvoie (
                             &f_max,
                             converti,
                             res->col_tab[j] < COLRES_BARRES_EQ_MX ?
                               DECIMAL_FORCE :
                               DECIMAL_MOMENT);
                    
                    BUGCRIT (tmp = g_strdup_printf (gettext ("Enveloppe supérieure :\n%s\nEnveloppe inférieure :\n%s"),
                                                             tmp2,
                                                             tmp1),
                             FALSE,
                             (gettext ("Erreur d'allocation mémoire.\n"));
                               FREE_ALL
                               FREE_ALL2)
                  }
                }
                else
                {
                  Action *action2 = list_parcours2->data;
                  tmp = common_fonction_renvoie (
                          _1990_action_efforts_renvoie (
                            action2,
                            res->col_tab[j] - COLRES_BARRES_EQ_N,
                            i),
                          NULL,
                          res->col_tab[j] < COLRES_BARRES_EQ_MX ?
                            DECIMAL_FORCE :
                            DECIMAL_MOMENT);
                }
                gtk_list_store_set (res->list_store, &Iter, j - 1, tmp, -1);
                
                break;
              }
              case COLRES_DEF_PIXBUF_UX :
              case COLRES_DEF_PIXBUF_UY :
              case COLRES_DEF_PIXBUF_UZ :
              case COLRES_DEF_PIXBUF_RX :
              case COLRES_DEF_PIXBUF_RY :
              case COLRES_DEF_PIXBUF_RZ :
              {
                GdkPixbuf *pixbuf;
                
                list_parcours2 = actions;
                
                while (list_parcours2 != NULL)
                {
                  Action *action2 = list_parcours2->data;
                  
                  if (res->col_tab[j] < COLRES_DEF_PIXBUF_RX)
                    liste = g_list_append (
                              liste,
                              _1990_action_deformation_renvoie (
                                action2,
                                res->col_tab[j] - COLRES_DEF_PIXBUF_UX,
                                i));
                  else
                    liste = g_list_append (
                              liste,
                              _1990_action_rotation_renvoie (
                                action2,
                                res->col_tab[j] - COLRES_DEF_PIXBUF_RX,
                                i));
                  
                  list_parcours2 = g_list_next (list_parcours2);
                }
                
                pixbuf = common_fonction_dessin (
                           liste,
                           200,
                           50,
                           res->col_tab[j] < COLRES_DEF_PIXBUF_RX ?
                             DECIMAL_DEPLACEMENT :
                             DECIMAL_ROTATION);
                gtk_list_store_set (res->list_store, &Iter, j - 1, pixbuf, -1);
                
                g_object_unref (pixbuf);
                
                break;
              }
              case COLRES_DEF_DESC_UX :
              case COLRES_DEF_DESC_UY :
              case COLRES_DEF_DESC_UZ :
              case COLRES_DEF_DESC_RX :
              case COLRES_DEF_DESC_RY :
              case COLRES_DEF_DESC_RZ :
              {
                list_parcours2 = actions;
                
                if (g_list_next (actions) != NULL)
                {
                  while (list_parcours2 != NULL)
                  {
                    Action *action2 = list_parcours2->data;
                    
                    if (res->col_tab[j] < COLRES_DEF_DESC_RX)
                      liste = g_list_append (
                                liste,
                                _1990_action_deformation_renvoie (
                                  action2,
                                  res->col_tab[j] - COLRES_DEF_DESC_UX,
                                  i));
                    else
                      liste = g_list_append (
                                liste,
                                _1990_action_rotation_renvoie (
                                  action2,
                                  res->col_tab[j] - COLRES_DEF_DESC_RX,
                                  i));
                    
                    list_parcours2 = g_list_next (list_parcours2);
                  }
                  
                  if (common_fonction_renvoie_enveloppe (liste,
                                                         &f_min,
                                                         &f_max,
                                                         &comb_min,
                                                         &comb_max) == FALSE)
                    BUGCRIT (tmp = g_strdup_printf (gettext ("Erreur")),
                             FALSE,
                             (gettext ("Erreur d'allocation mémoire.\n"));
                               FREE_ALL
                               FREE_ALL2)
                  else
                  {
                    tmp1 = common_fonction_affiche_caract (
                             &f_min,
                             DECIMAL_DISTANCE,
                             res->col_tab[j] < COLRES_DEF_DESC_RX ?
                               DECIMAL_DEPLACEMENT :
                               DECIMAL_ROTATION);
                    tmp2 = common_fonction_affiche_caract (
                             &f_max,
                             DECIMAL_DISTANCE,
                             res->col_tab[j] < COLRES_DEF_DESC_RX ?
                               DECIMAL_DEPLACEMENT :
                               DECIMAL_ROTATION);
                    
                    BUGCRIT (tmp = g_strdup_printf (gettext ("Enveloppe supérieure :\n%s\nEnveloppe inférieure :\n%s"),
                                                    tmp2,
                                                    tmp1),
                             FALSE,
                             (gettext ("Erreur d'allocation mémoire.\n"));
                               FREE_ALL
                               FREE_ALL2)
                  }
                }
                else
                {
                  Action *action2 = list_parcours2->data;
                  
                  if (res->col_tab[j] < COLRES_DEF_DESC_RX)
                    tmp = common_fonction_affiche_caract (
                            _1990_action_deformation_renvoie (
                              action2,
                              res->col_tab[j] - COLRES_DEF_DESC_UX,
                              i),
                            DECIMAL_DISTANCE,
                            DECIMAL_DEPLACEMENT);
                  else
                    tmp = common_fonction_affiche_caract (
                            _1990_action_rotation_renvoie (
                              action2,
                              res->col_tab[j] - COLRES_DEF_DESC_RX,
                              i),
                            DECIMAL_DISTANCE,
                            DECIMAL_ROTATION);
                }
                gtk_list_store_set (res->list_store, &Iter, j - 1, tmp, -1);
                
                break;
              }
              case COLRES_DEF_UX :
              case COLRES_DEF_UY :
              case COLRES_DEF_UZ :
              case COLRES_DEF_RX :
              case COLRES_DEF_RY :
              case COLRES_DEF_RZ :
              {
                list_parcours2 = actions;
                
                if (g_list_next (actions) != NULL)
                {
                  while (list_parcours2 != NULL)
                  {
                    Action *action2 = list_parcours2->data;
                    
                    if (res->col_tab[j] < COLRES_DEF_RX)
                      liste = g_list_append (
                                liste,
                                _1990_action_deformation_renvoie (
                                  action2,
                                  res->col_tab[j] - COLRES_DEF_UX,
                                  i));
                    else
                      liste = g_list_append (
                                liste,
                                _1990_action_rotation_renvoie (
                                  action2,
                                  res->col_tab[j] - COLRES_DEF_RX,
                                  i));
                    
                    list_parcours2 = g_list_next (list_parcours2);
                  }
                  
                  if (common_fonction_renvoie_enveloppe (liste,
                                                         &f_min,
                                                         &f_max,
                                                         &comb_min,
                                                         &comb_max) == FALSE)
                    BUGCRIT (tmp = g_strdup_printf (gettext ("Erreur")),
                             FALSE,
                             (gettext ("Erreur d'allocation mémoire.\n"));
                               FREE_ALL
                               FREE_ALL2)
                  else
                  {
                    BUG (common_fonction_conversion_combinaisons (&comb_min,
                                                                  comb,
                                                                  &converti),
                         FALSE,
                         FREE_ALL
                           FREE_ALL2)
                    tmp1 = common_fonction_renvoie (
                             &f_min,
                             converti,
                             res->col_tab[j] < COLRES_DEF_RX ?
                               DECIMAL_DEPLACEMENT :
                               DECIMAL_ROTATION);
                    g_list_free (converti);
                    converti = NULL;
                    BUG (common_fonction_conversion_combinaisons (&comb_max,
                                                                  comb,
                                                                  &converti),
                         FALSE,
                         FREE_ALL
                           FREE_ALL2)
                    tmp2 = common_fonction_renvoie (
                             &f_max,
                             converti,
                             res->col_tab[j] < COLRES_DEF_RX ?
                               DECIMAL_DEPLACEMENT :
                               DECIMAL_ROTATION);
                    
                    BUGCRIT (tmp = g_strdup_printf (gettext ("Enveloppe supérieure :\n%s\nEnveloppe inférieure :\n%s"),
                                                             tmp2,
                                                             tmp1),
                             FALSE,
                             (gettext ("Erreur d'allocation mémoire.\n"));
                               FREE_ALL
                               FREE_ALL2)
                  }
                }
                else
                {
                  Action *action2 = list_parcours2->data;
                  
                  if (res->col_tab[j] < COLRES_DEF_RX)
                    tmp = common_fonction_renvoie (
                            _1990_action_deformation_renvoie (
                              action2,
                              res->col_tab[j] - COLRES_DEF_UX,
                              i),
                            NULL,
                            DECIMAL_DEPLACEMENT);
                  else
                    tmp = common_fonction_renvoie (
                            _1990_action_rotation_renvoie (
                              action2,
                              res->col_tab[j] - COLRES_DEF_RX,
                              i),
                            NULL,
                            DECIMAL_ROTATION);
                }
                gtk_list_store_set (res->list_store, &Iter, j - 1, tmp, -1);
                
                break;
              }
              default :
              {
                FAILINFO (FALSE,
                          (gettext("La colonne des résultats %d est inconnue.\n"),
                                   res->col_tab[j]);
                            FREE_ALL
                            FREE_ALL2)
                break;
              }
            }
            
            FREE_ALL2
          }
          gtk_list_store_set (res->list_store, &Iter, res->col_tab[0], "", -1);
        }
        
        i++;
        list_parcours = g_list_next (list_parcours);
      }
      break;
    }
    
    case COLRES_NOEUDS_X :
    case COLRES_NOEUDS_Y :
    case COLRES_NOEUDS_Z :
    case COLRES_REACTION_APPUI_FX :
    case COLRES_REACTION_APPUI_FY :
    case COLRES_REACTION_APPUI_FZ :
    case COLRES_REACTION_APPUI_MX :
    case COLRES_REACTION_APPUI_MY :
    case COLRES_REACTION_APPUI_MZ :
    case COLRES_DEPLACEMENT_UX :
    case COLRES_DEPLACEMENT_UY :
    case COLRES_DEPLACEMENT_UZ :
    case COLRES_DEPLACEMENT_RX :
    case COLRES_DEPLACEMENT_RY :
    case COLRES_DEPLACEMENT_RZ :
    case COLRES_BARRES_LONGUEUR :
    case COLRES_BARRES_PIXBUF_N :
    case COLRES_BARRES_PIXBUF_TY :
    case COLRES_BARRES_PIXBUF_TZ :
    case COLRES_BARRES_PIXBUF_MX :
    case COLRES_BARRES_PIXBUF_MY :
    case COLRES_BARRES_PIXBUF_MZ :
    case COLRES_BARRES_DESC_N :
    case COLRES_BARRES_DESC_TY :
    case COLRES_BARRES_DESC_TZ :
    case COLRES_BARRES_DESC_MX :
    case COLRES_BARRES_DESC_MY :
    case COLRES_BARRES_DESC_MZ :
    case COLRES_BARRES_EQ_N :
    case COLRES_BARRES_EQ_TY :
    case COLRES_BARRES_EQ_TZ :
    case COLRES_BARRES_EQ_MX :
    case COLRES_BARRES_EQ_MY :
    case COLRES_BARRES_EQ_MZ :
    case COLRES_DEF_PIXBUF_UX :
    case COLRES_DEF_PIXBUF_UY :
    case COLRES_DEF_PIXBUF_UZ :
    case COLRES_DEF_PIXBUF_RX :
    case COLRES_DEF_PIXBUF_RY :
    case COLRES_DEF_PIXBUF_RZ :
    case COLRES_DEF_DESC_UX :
    case COLRES_DEF_DESC_UY :
    case COLRES_DEF_DESC_UZ :
    case COLRES_DEF_DESC_RX :
    case COLRES_DEF_DESC_RY :
    case COLRES_DEF_DESC_RZ :
    case COLRES_DEF_UX :
    case COLRES_DEF_UY :
    case COLRES_DEF_UZ :
    case COLRES_DEF_RX :
    case COLRES_DEF_RY :
    case COLRES_DEF_RZ :
    default :
    {
      FAILCRIT (FALSE,
                (gettext ("La première colonne ne peut contenir que la liste des nœuds ou des barres.\n"));)
      break;
    }
  }
  
  FREE_ALL
  
  return TRUE;
#undef FREE_ALL2
#undef FREE_ALL
}


gboolean
EF_gtk_resultats_add_page (Gtk_EF_Resultats_Tableau *res,
                           Projet                   *p)
/**
 * \brief Ajoute une page au treeview de la fenêtre affichant les résultats en
 *        fonction de la description fournie via la variable res.
 * \param res : caractéristiques de la page à ajouter,
 * \param p : la variable projet.
 * \return TRUE.\n
 *   Echec : FALSE .
 *     - p == NULL,
 *     - res == NULL,
 *     - interface graphique non initialisée.
 */
{
  GtkWidget         *p_scrolled_window; 
  unsigned int       i;
  GType             *col_type;
  GtkCellRenderer   *cell;
  GtkTreeViewColumn *column;
  double             xalign;
  
  BUGPARAMCRIT (p, "%p", p, FALSE)
  BUGPARAMCRIT (res, "%p", res, FALSE)
  BUGCRIT (UI_RES.builder,
           FALSE,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Résultats");)
  
  p_scrolled_window = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (p_scrolled_window),
                                  GTK_POLICY_AUTOMATIC,
                                  GTK_POLICY_AUTOMATIC);
  
  gtk_notebook_insert_page (UI_RES.notebook,
                            p_scrolled_window,
                            GTK_WIDGET (gtk_label_new (res->nom)),
                            gtk_notebook_get_n_pages (UI_RES.notebook) - 1);
  
  // La variable xalign n'est utilisée que pour les résultats à virgule
  // flottante. Elle centre le texte si on affiche une enveloppe de résultats
  // (affichage sous forme ***/***) ou aligne à droite si elle affiche un
  // nombre à virgule flottante (pour aligner les virgules)
  if (gtk_combo_box_get_active (UI_RES.combobox) == 2)
    xalign = 0.5;
  else
    xalign = 1.;
  
  res->treeview = GTK_TREE_VIEW (gtk_tree_view_new ());
  gtk_container_add (GTK_CONTAINER (p_scrolled_window),
                     GTK_WIDGET (res->treeview));
  
  BUGCRIT (col_type = malloc ((res->col_tab[0] + 1) * sizeof (GType)),
           FALSE,
           (gettext ("Erreur d'allocation mémoire.\n"));)
  
  for (i = 1; i <= res->col_tab[0]; i++)
  {
    switch (res->col_tab[i])
    {
      case COLRES_NUM_NOEUDS :
      {
        BUGCRIT (i == 1,
                 FALSE,
                 (gettext ("La liste des noeuds doit être spécifiée en tant que première colonne.\n"));
                   free (col_type);)
        col_type[i - 1] = G_TYPE_INT;
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (gettext ("Noeuds"),
                                   col_type[i - 1],
                                   i - 1,
                                   0.5,
                                   0));
        break;
      }
      case COLRES_NUM_BARRES :
      {
        BUGCRIT (i == 1,
                 FALSE,
                 (gettext ("La liste des barres doit être spécifiée en tant que première colonne.\n"));
                   free (col_type);)
        col_type[i - 1] = G_TYPE_INT;
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (gettext ("Barres"),
                                   col_type[i - 1],
                                   i - 1,
                                   0.5,
                                   0));
        break;
      }
      case COLRES_NOEUDS_X :
      {
        BUGCRIT (i != 1,
                 FALSE,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   free (col_type);)
        BUGCRIT (res->col_tab[1] == COLRES_NUM_NOEUDS,
                 FALSE,
                 (gettext ("La position en %s ne peut être affichée que si la première colonne affiche les numéros des noeuds.\n"),
                          "x");
                   free (col_type);)
        col_type[i - 1] = G_TYPE_STRING;
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (gettext ("x [m]"),
                                   col_type[i - 1],
                                   i - 1,
                                   1.,
                                   0));
        break;
      }
      case COLRES_NOEUDS_Y :
      {
        BUGCRIT (i != 1,
                 FALSE,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   free (col_type);)
        BUGCRIT (res->col_tab[1] == COLRES_NUM_NOEUDS,
                 FALSE,
                 (gettext ("La position en %s ne peut être affichée que si la première colonne affiche les numéros des noeuds.\n"),
                           "y");
                   free (col_type);)
        col_type[i - 1] = G_TYPE_STRING;
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (gettext ("y [m]"),
                                   col_type[i - 1],
                                   i - 1,
                                   1.,
                                   0));
        break;
      }
      case COLRES_NOEUDS_Z :
      {
        BUGCRIT (i != 1,
                 FALSE,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   free (col_type);)
        BUGCRIT (res->col_tab[1] == COLRES_NUM_NOEUDS,
                 FALSE,
                 (gettext ("La position en %s ne peut être affichée que si la première colonne affiche les numéros des noeuds.\n"),
                           "z");
                   free (col_type);)
        col_type[i - 1] = G_TYPE_STRING;
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (gettext ("z [m]"),
                                   col_type[i - 1],
                                   i - 1,
                                   1.,
                                   0));
        break;
      }
      case COLRES_REACTION_APPUI_FX :
      {
        BUGCRIT (i != 1,
                 FALSE,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   free (col_type);)
        BUGCRIT (res->col_tab[1] == COLRES_NUM_NOEUDS,
                 FALSE,
                 (gettext ("La réaction d'appui %s ne peut être affichée que si la première colonne affiche les numéros des noeuds.\n"),
                           "Fx");
                   free (col_type);)
        col_type[i - 1] = G_TYPE_STRING;
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (gettext ("F<sub>x</sub> [N]"),
                                   col_type[i - 1],
                                   i - 1,
                                   xalign,
                                   0));
        break;
      }
      case COLRES_REACTION_APPUI_FY:
      {
        BUGCRIT (i != 1,
                 FALSE,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   free (col_type);)
        BUGCRIT (res->col_tab[1] == COLRES_NUM_NOEUDS,
                 FALSE,
                 (gettext ("La réaction d'appui %s ne peut être affichée que si la première colonne affiche les numéros des noeuds.\n"),
                           "Fy");
                   free (col_type);)
        col_type[i - 1] = G_TYPE_STRING;
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (gettext ("F<sub>y</sub> [N]"),
                                   col_type[i - 1],
                                   i - 1,
                                   xalign,
                                   0));
        break;
      }
      case COLRES_REACTION_APPUI_FZ :
      {
        BUGCRIT (i != 1,
                 FALSE,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   free (col_type);)
        BUGCRIT (res->col_tab[1] == COLRES_NUM_NOEUDS,
                 FALSE,
                 (gettext ("La réaction d'appui %s ne peut être affichée que si la première colonne affiche les numéros des noeuds.\n"),
                           "Fz");
                   free (col_type);)
        col_type[i - 1] = G_TYPE_STRING;
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (gettext ("F<sub>z</sub> [N]"),
                                   col_type[i - 1],
                                   i - 1,
                                   xalign,
                                   0));
        break;
      }
      case COLRES_REACTION_APPUI_MX :
      {
        BUGCRIT (i != 1,
                 FALSE,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));free (col_type);)
        BUGCRIT (res->col_tab[1] == COLRES_NUM_NOEUDS,
                 FALSE,
                 (gettext ("La réaction d'appui %s ne peut être affichée que si la première colonne affiche les numéros des noeuds.\n"),
                           "Mx");
                   free (col_type);)
        col_type[i - 1] = G_TYPE_STRING;
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (gettext ("M<sub>x</sub> [N.m]"),
                                   col_type[i - 1],
                                   i - 1,
                                   xalign,
                                   0));
        break;
      }
      case COLRES_REACTION_APPUI_MY :
      {
        BUGCRIT (i != 1,
                 FALSE,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   free (col_type);)
        BUGCRIT (res->col_tab[1] == COLRES_NUM_NOEUDS,
                 FALSE,
                 (gettext ("La réaction d'appui %s ne peut être affichée que si la première colonne affiche les numéros des noeuds.\n"),
                           "My");
                   free (col_type);)
        col_type[i - 1] = G_TYPE_STRING;
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (gettext ("M<sub>y</sub> [N.m]"),
                                   col_type[i - 1],
                                   i - 1,
                                   xalign,
                                   0));
        break;
      }
      case COLRES_REACTION_APPUI_MZ :
      {
        BUGCRIT (i != 1,
                 FALSE,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   free (col_type);)
        BUGCRIT (res->col_tab[1] == COLRES_NUM_NOEUDS,
                 FALSE,
                 (gettext ("La réaction d'appui %s ne peut être affichée que si la première colonne affiche les numéros des noeuds.\n"),
                           "Mz");
                   free (col_type);)
        col_type[i - 1] = G_TYPE_STRING;
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (gettext ("M<sub>z</sub> [N.m]"),
                                   col_type[i - 1],
                                   i - 1,
                                   xalign,
                                   0));
        break;
      }
      case COLRES_DEPLACEMENT_UX :
      {
        BUGCRIT (i != 1,
                 FALSE,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   free (col_type);)
        BUGCRIT (res->col_tab[1] == COLRES_NUM_NOEUDS,
                 FALSE,
                 (gettext ("Le déplacement des noeuds %s ne peut être affichée que si la première colonne affiche les numéros des noeuds.\n"),
                           "Ux");
                   free (col_type);)
        col_type[i - 1] = G_TYPE_STRING;
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (gettext ("u<sub>x</sub> [m]"),
                                   col_type[i - 1],
                                   i - 1,
                                   xalign,
                                   0));
        break;
      }
      case COLRES_DEPLACEMENT_UY :
      {
        BUGCRIT (i != 1,
                 FALSE,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   free (col_type);)
        BUGCRIT (res->col_tab[1] == COLRES_NUM_NOEUDS,
                 FALSE,
                 (gettext ("Le déplacement des noeuds %s ne peut être affichée que si la première colonne affiche les numéros des noeuds.\n"),
                           "Uy");
                   free (col_type);)
        col_type[i - 1] = G_TYPE_STRING;
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (gettext ("u<sub>y</sub> [m]"),
                                   col_type[i - 1],
                                   i - 1,
                                   xalign,
                                   0));
        break;
      }
      case COLRES_DEPLACEMENT_UZ :
      {
        BUGCRIT (i != 1,
                 FALSE,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   free (col_type);)
        BUGCRIT (res->col_tab[1] == COLRES_NUM_NOEUDS,
                 FALSE,
                 (gettext ("Le déplacement des noeuds %s ne peut être affichée que si la première colonne affiche les numéros des noeuds.\n"),
                           "Uz");
                   free (col_type);)
        col_type[i - 1] = G_TYPE_STRING;
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (gettext ("u<sub>z</sub> [m]"),
                                   col_type[i - 1],
                                   i - 1,
                                   xalign,
                                   0));
        break;
      }
      case COLRES_DEPLACEMENT_RX :
      {
        BUGCRIT (i != 1,
                 FALSE,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   free (col_type);)
        BUGCRIT (res->col_tab[1] == COLRES_NUM_NOEUDS,
                 FALSE,
                 (gettext ("Le déplacement des noeuds %s ne peut être affichée que si la première colonne affiche les numéros des noeuds.\n"),
                           "Rx");
                   free (col_type);)
        col_type[i - 1] = G_TYPE_STRING;
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (gettext ("r<sub>x</sub> [rad]"),
                                   col_type[i - 1],
                                   i - 1,
                                   xalign,
                                   0));
        break;
      }
      case COLRES_DEPLACEMENT_RY :
      {
        BUGCRIT (i != 1,
                 FALSE,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   free (col_type);)
        BUGCRIT (res->col_tab[1] == COLRES_NUM_NOEUDS,
                 FALSE,
                 (gettext ("Le déplacement des noeuds %s ne peut être affichée que si la première colonne affiche les numéros des noeuds.\n"),
                           "Ry");
                   free (col_type);)
        col_type[i - 1] = G_TYPE_STRING;
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (gettext ("r<sub>y</sub> [rad]"),
                                   col_type[i - 1],
                                   i - 1,
                                   xalign,  0));
        break;
      }
      case COLRES_DEPLACEMENT_RZ :
      {
        BUGCRIT (i != 1,
                 FALSE,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   free (col_type);)
        BUGCRIT (res->col_tab[1] == COLRES_NUM_NOEUDS,
                 FALSE,
                 (gettext ("Le déplacement des noeuds %s ne peut être affichée que si la première colonne affiche les numéros des noeuds.\n"),
                           "Rx");
                   free (col_type);)
        col_type[i - 1] = G_TYPE_STRING;
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (gettext ("r<sub>z</sub> [rad]"),
                                   col_type[i - 1],
                                   i - 1,
                                   xalign,
                                   0));
        break;
      }
      case COLRES_BARRES_LONGUEUR :
      {
        BUGCRIT (i != 1,
                 FALSE,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   free (col_type);)
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 FALSE,
                 (gettext ("La longueur des barres ne peut être affichée que si la première colonne affiche les numéros des barres.\n"));
                   free (col_type);)
        col_type[i - 1] = G_TYPE_DOUBLE;
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (gettext ("L [m]"),
                                   col_type[i - 1],
                                   i - 1,
                                   1.,
                                   DECIMAL_DISTANCE));
        break;
      }
      case COLRES_BARRES_PIXBUF_N :
      {
        BUGCRIT (i != 1,
                 FALSE,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   free (col_type);)
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 FALSE,
                 (gettext ("Les efforts dans les barres ne peuvent être affichés que si la première colonne affiche les numéros des barres.\n"));
                   free (col_type);)
        col_type[i - 1] = G_TYPE_OBJECT;
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (gettext ("Effort normal"),
                                   col_type[i - 1],
                                   i - 1,
                                   0.5,
                                   0));
        break;
      }
      case COLRES_BARRES_PIXBUF_TY :
      {
        BUGCRIT (i != 1,
                 FALSE,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   free (col_type);)
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 FALSE,
                 (gettext ("Les efforts dans les barres ne peuvent être affichés que si la première colonne affiche les numéros des barres.\n"));
                   free (col_type);)
        col_type[i - 1] = G_TYPE_OBJECT;
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (gettext ("Effort tranchant selon Y"),
                                   col_type[i - 1],
                                   i - 1,
                                   0.5,
                                   0));
        break;
      }
      case COLRES_BARRES_PIXBUF_TZ :
      {
        BUGCRIT (i != 1,
                 FALSE,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   free (col_type);)
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 FALSE,
                 (gettext ("Les efforts dans les barres ne peuvent être affichés que si la première colonne affiche les numéros des barres.\n"));
                   free (col_type);)
        col_type[i - 1] = G_TYPE_OBJECT;
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (gettext ("Effort tranchant selon Z"),
                                   col_type[i - 1],
                                   i - 1,
                                   0.5,
                                   0));
        break;
      }
      case COLRES_BARRES_PIXBUF_MX :
      {
        BUGCRIT (i != 1,
                 FALSE,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   free (col_type);)
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 FALSE,
                 (gettext ("Les efforts dans les barres ne peuvent être affichés que si la première colonne affiche les numéros des barres.\n"));
                   free (col_type);)
        col_type[i - 1] = G_TYPE_OBJECT;
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (gettext ("Moment de torsion"),
                                   col_type[i - 1],
                                   i - 1,
                                   0.5,
                                   0));
        break;
      }
      case COLRES_BARRES_PIXBUF_MY :
      {
        BUGCRIT (i != 1,
                 FALSE,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   free (col_type);)
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 FALSE,
                 (gettext ("Les efforts dans les barres ne peuvent être affichés que si la première colonne affiche les numéros des barres.\n"));
                   free (col_type);)
        col_type[i - 1] = G_TYPE_OBJECT;
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (gettext ("Moment fléchissant selon y"),
                                   col_type[i - 1],
                                   i - 1,
                                   0.5,
                                   0));
        break;
      }
      case COLRES_BARRES_PIXBUF_MZ :
      {
        BUGCRIT (i != 1,
                 FALSE,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   free (col_type);)
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 FALSE,
                 (gettext ("Les efforts dans les barres ne peuvent être affichés que si la première colonne affiche les numéros des barres.\n"));
                   free (col_type);)
        col_type[i - 1] = G_TYPE_OBJECT;
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (gettext ("Moment fléchissant selon z"),
                                   col_type[i - 1],
                                   i - 1,
                                   0.5,
                                   0));
        break;
      }
      case COLRES_BARRES_DESC_N :
      {
        BUGCRIT (i != 1,
                 FALSE,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   free (col_type);)
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 FALSE,
                 (gettext ("Les efforts dans les barres ne peuvent être affichés que si la première colonne affiche les numéros des barres.\n"));
                   free (col_type);)
        col_type[i - 1] = G_TYPE_STRING;
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (gettext ("Points caractéristiques de N [N]"),
                                   col_type[i - 1],
                                   i - 1,
                                   0.,
                                   0));
        break;
      }
      case COLRES_BARRES_DESC_TY :
      {
        BUGCRIT (i != 1,
                 FALSE,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   free (col_type);)
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 FALSE,
                 (gettext ("Les efforts dans les barres ne peuvent être affichés que si la première colonne affiche les numéros des barres.\n"));
                   free (col_type);)
        col_type[i - 1] = G_TYPE_STRING;
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (gettext ("Points caractéristiques de T<sub>y</sub> [N]"),
                                   col_type[i - 1],
                                   i - 1,
                                   0.,
                                   0));
        break;
      }
      case COLRES_BARRES_DESC_TZ :
      {
        BUGCRIT (i != 1,
                 FALSE,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   free (col_type);)
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 FALSE,
                 (gettext ("Les efforts dans les barres ne peuvent être affichés que si la première colonne affiche les numéros des barres.\n"));
                   free (col_type);)
        col_type[i - 1] = G_TYPE_STRING;
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (gettext ("Points caractéristiques de T<sub>z</sub> [N]"),
                                   col_type[i - 1],
                                   i - 1,
                                   0.,
                                   0));
        break;
      }
      case COLRES_BARRES_DESC_MX :
      {
        BUGCRIT (i != 1,
                 FALSE,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   free (col_type);)
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 FALSE,
                 (gettext ("Les efforts dans les barres ne peuvent être affichés que si la première colonne affiche les numéros des barres.\n"));
                   free (col_type);)
        col_type[i - 1] = G_TYPE_STRING;
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (gettext ("Points caractéristiques de M<sub>x</sub> [N.m]"),
                                   col_type[i - 1],
                                   i - 1,
                                   0.,
                                   0));
        break;
      }
      case COLRES_BARRES_DESC_MY :
      {
        BUGCRIT (i != 1,
                 FALSE,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   free (col_type);)
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 FALSE,
                 (gettext ("Les efforts dans les barres ne peuvent être affichés que si la première colonne affiche les numéros des barres.\n"));
                   free (col_type);)
        col_type[i - 1] = G_TYPE_STRING;
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (gettext ("Points caractéristiques de M<sub>y</sub> [N.m]"),
                                   col_type[i - 1],
                                   i - 1,
                                   0.,
                                   0));
        break;
      }
      case COLRES_BARRES_DESC_MZ :
      {
        BUGCRIT (i != 1,
                 FALSE,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   free (col_type);)
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 FALSE,
                 (gettext ("Les efforts dans les barres ne peuvent être affichés que si la première colonne affiche les numéros des barres.\n"));
                   free (col_type);)
        col_type[i - 1] = G_TYPE_STRING;
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (gettext ("Points caractéristiques de M<sub>z</sub> [N.m]"),
                                   col_type[i - 1],
                                   i - 1,
                                   0.,
                                   0));
        break;
      }
      case COLRES_BARRES_EQ_N :
      {
        BUGCRIT (i != 1,
                 FALSE,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   free (col_type);)
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 FALSE,
                 (gettext ("Les efforts dans les barres ne peuvent être affichés que si la première colonne affiche les numéros des barres.\n"));
                   free (col_type);)
        col_type[i - 1] = G_TYPE_STRING;
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (gettext ("N [N]"),
                                   col_type[i - 1],
                                   i - 1,
                                   0.,
                                   0));
        break;
      }
      case COLRES_BARRES_EQ_TY :
      {
        BUGCRIT (i != 1,
                 FALSE,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   free (col_type);)
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 FALSE,
                 (gettext ("Les efforts dans les barres ne peuvent être affichés que si la première colonne affiche les numéros des barres.\n"));
                   free (col_type);)
        col_type[i - 1] = G_TYPE_STRING;
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (gettext ("T<sub>y</sub> [N]"),
                                   col_type[i - 1],
                                   i - 1,
                                   0.,
                                   0));
        break;
      }
      case COLRES_BARRES_EQ_TZ :
      {
        BUGCRIT (i != 1,
                 FALSE,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   free (col_type);)
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 FALSE,
                 (gettext ("Les efforts dans les barres ne peuvent être affichés que si la première colonne affiche les numéros des barres.\n"));
                   free (col_type);)
        col_type[i - 1] = G_TYPE_STRING;
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (gettext ("T<sub>z</sub> [N]"),
                                   col_type[i - 1],
                                   i - 1,
                                   0.,
                                   0));
        break;
      }
      case COLRES_BARRES_EQ_MX :
      {
        BUGCRIT (i != 1,
                 FALSE,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   free (col_type);)
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 FALSE,
                 (gettext ("Les efforts dans les barres ne peuvent être affichés que si la première colonne affiche les numéros des barres.\n"));
                   free (col_type);)
        col_type[i - 1] = G_TYPE_STRING;
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (gettext ("M<sub>x</sub> [N.m]"),
                                   col_type[i - 1],
                                   i - 1,
                                   0.,
                                   0));
        break;
      }
      case COLRES_BARRES_EQ_MY :
      {
        BUGCRIT (i != 1,
                 FALSE,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   free (col_type);)
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 FALSE,
                 (gettext ("Les efforts dans les barres ne peuvent être affichés que si la première colonne affiche les numéros des barres.\n"));
                   free (col_type);)
        col_type[i - 1] = G_TYPE_STRING;
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (gettext ("M<sub>y</sub> [N.m]"),
                                   col_type[i - 1],
                                   i - 1,
                                   0.,
                                   0));
        break;
      }
      case COLRES_BARRES_EQ_MZ :
      {
        BUGCRIT (i != 1,
                 FALSE,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   free (col_type);)
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 FALSE,
                 (gettext ("Les efforts dans les barres ne peuvent être affichés que si la première colonne affiche les numéros des barres.\n"));
                   free (col_type);)
        col_type[i - 1] = G_TYPE_STRING;
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (gettext ("M<sub>z</sub> [N.m]"),
                                   col_type[i - 1],
                                   i - 1,
                                   0.,
                                   0));
        break;
      }
      case COLRES_DEF_PIXBUF_UX :
      {
        BUGCRIT (i != 1,
                 FALSE,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   free (col_type);)
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 FALSE,
                 (gettext ("Les efforts dans les barres ne peuvent être affichés que si la première colonne affiche les numéros des barres.\n"));
                   free (col_type);)
        col_type[i - 1] = G_TYPE_OBJECT;
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (gettext ("Déformation de la barre selon x"),
                                   col_type[i - 1],
                                   i - 1,
                                   0.5,
                                   0));
        break;
      }
      case COLRES_DEF_PIXBUF_UY :
      {
        BUGCRIT (i != 1,
                 FALSE,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   free (col_type);)
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 FALSE,
                 (gettext ("Les efforts dans les barres ne peuvent être affichés que si la première colonne affiche les numéros des barres.\n"));
                   free (col_type);)
        col_type[i - 1] = G_TYPE_OBJECT;
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (gettext ("Déformation de la barre selon y"),
                                   col_type[i - 1],
                                   i - 1,
                                   0.5,
                                   0));
        break;
      }
      case COLRES_DEF_PIXBUF_UZ :
      {
        BUGCRIT (i != 1,
                 FALSE,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   free (col_type);)
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 FALSE,
                 (gettext ("Les efforts dans les barres ne peuvent être affichés que si la première colonne affiche les numéros des barres.\n"));
                   free (col_type);)
        col_type[i - 1] = G_TYPE_OBJECT;
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (gettext ("Déformation de la barre selon z"),
                                   col_type[i - 1],
                                   i - 1,
                                   0.5,
                                   0));
        break;
      }
      case COLRES_DEF_PIXBUF_RX :
      {
        BUGCRIT (i != 1,
                 FALSE,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   free (col_type);)
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 FALSE,
                 (gettext ("Les efforts dans les barres ne peuvent être affichés que si la première colonne affiche les numéros des barres.\n"));
                   free (col_type);)
        col_type[i - 1] = G_TYPE_OBJECT;
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (gettext ("Rotation de la barre selon x"),
                                   col_type[i - 1],
                                   i - 1,
                                   0.5,
                                   0));
        break;
      }
      case COLRES_DEF_PIXBUF_RY :
      {
        BUGCRIT (i != 1,
                 FALSE,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   free (col_type);)
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 FALSE,
                 (gettext ("Les efforts dans les barres ne peuvent être affichés que si la première colonne affiche les numéros des barres.\n"));
                   free (col_type);)
        col_type[i - 1] = G_TYPE_OBJECT;
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (gettext ("Rotation de la barre selon y"),
                                   col_type[i - 1],
                                   i - 1,
                                   0.5,
                                   0));
        break;
      }
      case COLRES_DEF_PIXBUF_RZ :
      {
        BUGCRIT (i != 1,
                 FALSE,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   free (col_type);)
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 FALSE,
                 (gettext ("Les efforts dans les barres ne peuvent être affichés que si la première colonne affiche les numéros des barres.\n"));
                   free (col_type);)
        col_type[i - 1] = G_TYPE_OBJECT;
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (gettext ("Rotation de la barre selon z"),
                                   col_type[i - 1],
                                   i - 1,
                                   0.5,
                                   0));
        break;
      }
      case COLRES_DEF_DESC_UX :
      {
        BUGCRIT (i != 1,
                 FALSE,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   free (col_type);)
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 FALSE,
                 (gettext ("Les déformations dans les barres ne peuvent être affichées que si la première colonne affiche les numéros des barres.\n"));
                   free (col_type);)
        col_type[i - 1] = G_TYPE_STRING;
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (gettext ("Points caractéristiques de u<sub>x</sub> [m]"),
                                   col_type[i - 1],
                                   i - 1,
                                   0.,
                                   0));
        break;
      }
      case COLRES_DEF_DESC_UY :
      {
        BUGCRIT (i != 1,
                 FALSE,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   free (col_type);)
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 FALSE,
                 (gettext ("Les déformations dans les barres ne peuvent être affichées que si la première colonne affiche les numéros des barres.\n"));
                   free (col_type);)
        col_type[i - 1] = G_TYPE_STRING;
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (gettext ("Points caractéristiques de u<sub>y</sub> [m]"),
                                   col_type[i - 1],
                                   i - 1,
                                   0.,
                                   0));
        break;
      }
      case COLRES_DEF_DESC_UZ :
      {
        BUGCRIT (i != 1,
                 FALSE,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   free (col_type);)
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 FALSE,
                 (gettext ("Les déformations dans les barres ne peuvent être affichées que si la première colonne affiche les numéros des barres.\n"));
                   free (col_type);)
        col_type[i - 1] = G_TYPE_STRING;
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (gettext ("Points caractéristiques de u<sub>z</sub> [m]"),
                                   col_type[i - 1],
                                   i - 1,
                                   0.,
                                   0));
        break;
      }
      case COLRES_DEF_DESC_RX :
      {
        BUGCRIT (i != 1,
                 FALSE,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   free (col_type);)
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 FALSE,
                 (gettext ("Les déformations dans les barres ne peuvent être affichées que si la première colonne affiche les numéros des barres.\n"));
                   free (col_type);)
        col_type[i - 1] = G_TYPE_STRING;
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (gettext ("Points caractéristiques de r<sub>x</sub> [rad]"),
                                   col_type[i - 1],
                                   i - 1,
                                   0.,
                                   0));
        break;
      }
      case COLRES_DEF_DESC_RY :
      {
        BUGCRIT (i != 1,
                 FALSE,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   free (col_type);)
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 FALSE,
                 (gettext ("Les déformations dans les barres ne peuvent être affichées que si la première colonne affiche les numéros des barres.\n"));
                   free (col_type);)
        col_type[i - 1] = G_TYPE_STRING;
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (gettext ("Points caractéristiques de r<sub>y</sub> [rad]"),
                                   col_type[i - 1],
                                   i - 1,
                                   0.,
                                   0));
        break;
      }
      case COLRES_DEF_DESC_RZ :
      {
        BUGCRIT (i != 1,
                 FALSE,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   free (col_type);)
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 FALSE,
                 (gettext ("Les déformations dans les barres ne peuvent être affichées que si la première colonne affiche les numéros des barres.\n"));
                   free (col_type);)
        col_type[i - 1] = G_TYPE_STRING;
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (gettext ("Points caractéristiques de r<sub>z</sub> [rad]"),
                                   col_type[i - 1],
                                   i - 1,
                                   0.,
                                   0));
        break;
      }
      case COLRES_DEF_UX :
      {
        BUGCRIT (i != 1,
                 FALSE,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   free (col_type);)
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 FALSE,
                 (gettext ("Les déformations dans les barres ne peuvent être affichées que si la première colonne affiche les numéros des barres.\n"));
                   free (col_type);)
        col_type[i - 1] = G_TYPE_STRING;
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (gettext ("u<sub>x</sub> [m]"),
                                   col_type[i - 1],
                                   i - 1,
                                   0.,
                                   0));
        break;
      }
      case COLRES_DEF_UY :
      {
        BUGCRIT (i != 1,
                 FALSE,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   free (col_type);)
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 FALSE,
                 (gettext ("Les déformations dans les barres ne peuvent être affichées que si la première colonne affiche les numéros des barres.\n"));
                   free (col_type);)
        col_type[i - 1] = G_TYPE_STRING;
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (gettext ("u<sub>y</sub> [m]"),
                                   col_type[i - 1],
                                   i - 1,
                                   0.,
                                   0));
        break;
      }
      case COLRES_DEF_UZ :
      {
        BUGCRIT (i != 1,
                 FALSE,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   free (col_type);)
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 FALSE,
                 (gettext ("Les déformations dans les barres ne peuvent être affichées que si la première colonne affiche les numéros des barres.\n"));
                   free (col_type);)
        col_type[i - 1] = G_TYPE_STRING;
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (gettext ("u<sub>z</sub> [m]"),
                                   col_type[i - 1],
                                   i - 1,
                                   0.,
                                   0));
        break;
      }
      case COLRES_DEF_RX :
      {
        BUGCRIT (i != 1,
                 FALSE,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   free (col_type);)
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 FALSE,
                 (gettext ("Les déformations dans les barres ne peuvent être affichées que si la première colonne affiche les numéros des barres.\n"));
                   free (col_type);)
        col_type[i - 1] = G_TYPE_STRING;
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (gettext ("r<sub>x</sub> [rad]"),
                                   col_type[i - 1],
                                   i - 1,
                                   0.,
                                   0));
        break;
      }
      case COLRES_DEF_RY :
      {
        BUGCRIT (i != 1,
                 FALSE,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   free (col_type);)
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 FALSE,
                 (gettext ("Les déformations dans les barres ne peuvent être affichées que si la première colonne affiche les numéros des barres.\n"));
                   free (col_type);)
        col_type[i - 1] = G_TYPE_STRING;
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (gettext ("r<sub>y</sub> [rad]"),
                                   col_type[i - 1],
                                   i - 1,
                                   0.,
                                   0));
        break;
      }
      case COLRES_DEF_RZ :
      {
        BUGCRIT (i != 1,
                 FALSE,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   free (col_type);)
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 FALSE,
                 (gettext ("Les déformations dans les barres ne peuvent être affichées que si la première colonne affiche les numéros des barres.\n"));
                   free (col_type);)
        col_type[i - 1] = G_TYPE_STRING;
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (gettext ("r<sub>z</sub> [rad]"),
                                   col_type[i - 1],
                                   i - 1,
                                   0.,
                                   0));
        break;
      }
      default :
      {
        FAILCRIT (FALSE,
                  (gettext ("La colonne des résultats %d est inconnue.\n"),
                            res->col_tab[i]);
                    free (col_type);)
        break;
      }
    }
  }
  
  // On insère une colonne vide à la fin pour éviter le redimensionnement
  // automatique de la dernière colonne.
  cell = gtk_cell_renderer_text_new ();
  column = gtk_tree_view_column_new_with_attributes ("",
                                                     cell,
                                                     "text", res->col_tab[0],
                                                     NULL);
  gtk_tree_view_append_column (res->treeview, column);
  col_type[res->col_tab[0]] = G_TYPE_STRING;
  
  res->list_store = gtk_list_store_newv ((gint) res->col_tab[0] + 1, col_type);
  free (col_type);
  gtk_tree_view_set_model (res->treeview, GTK_TREE_MODEL (res->list_store));

  BUG (EF_gtk_resultats_remplit_page (res, p), FALSE)
  
  gtk_widget_show_all (p_scrolled_window);
  
  gtk_notebook_set_current_page (
    UI_RES.notebook,
    gtk_notebook_get_n_pages (UI_RES.notebook) - 2);
  
  return TRUE;
}


void EF_gtk_resultats_cas_change(GtkWidget *widget, Projet *p)
/**
 * \brief Met à jour l'affichage des résultats en cas de changement de cas.
 * \param widget : le composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
{
  GList *list_parcours;
  int    indice_combo;
  
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_RES.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Résultats");)
  
  indice_combo = gtk_combo_box_get_active (UI_RES.combobox);
  
  // Pour forcer le combobox ponderation à être vierge.
  if (gtk_combo_box_get_model (UI_RES.combobox_ponderations) != NULL)
  {
    gtk_combo_box_set_active (UI_RES.combobox_ponderations, -1);
    gtk_combo_box_set_model (UI_RES.combobox_ponderations, NULL);
  }
  
  list_parcours = UI_RES.tableaux;
  
  while (list_parcours != NULL)
  {
    BUG (EF_gtk_resultats_remplit_page (list_parcours->data, p), )
    
    list_parcours = g_list_next (list_parcours);
  }
  
  if (indice_combo == 1)
  {
    GtkListStore *list_pond;
    GtkTreeIter   Iter;
    GList        *comb;
    int           i;
    
    switch (gtk_combo_box_get_active (UI_RES.combobox_cas))
    {
      case -1 :
        return;
      case 0 :
      {
        comb = p->combinaisons.elu_equ;
        break;
      }
      case 1 :
      {
        comb = p->combinaisons.elu_str;
        break;
      }
      case 2 :
      {
        comb = p->combinaisons.elu_geo;
        break;
      }
      case 3 :
      {
        comb = p->combinaisons.elu_fat;
        break;
      }
      case 4 :
      {
        comb = p->combinaisons.elu_acc;
        break;
      }
      case 5 :
      {
        comb = p->combinaisons.elu_sis;
        break;
      }
      case 6 :
      {
        comb = p->combinaisons.els_car;
        break;
      }
      case 7 :
      {
        comb = p->combinaisons.els_freq;
        break;
      }
      case 8 :
      {
        comb = p->combinaisons.els_perm;
        break;
      }
      default :
      {
        BUGMSG (NULL,
                ,
                gettext ("Paramètre %s incorrect.\n"), "combobox_cas")
        break;
      }
    }
    
    list_pond = gtk_list_store_new (1, G_TYPE_STRING);
    
    list_parcours = comb;
    i = 0;
    while (list_parcours != NULL)
    {
      char *tmp, *tmp2;
      
      gtk_list_store_append (list_pond, &Iter);
      tmp = _1990_ponderations_description (list_parcours->data);
      BUGCRIT (tmp2 = g_strdup_printf ("%d : %s", i, tmp),
               ,
               (gettext ("Erreur d'allocation mémoire.\n"));
                 free (tmp);)
      free (tmp);
      gtk_list_store_set (list_pond, &Iter, 0, tmp2, -1);
      free (tmp2);
      
      list_parcours = g_list_next (list_parcours);
      i++;
    }
    
    gtk_combo_box_set_model (UI_RES.combobox_ponderations,
                             GTK_TREE_MODEL (list_pond));
    g_object_unref (list_pond);
  }
  else if (indice_combo == 2)
  {
    GtkListStore *list_pond;
    GtkTreeIter   Iter;
    
    list_pond = gtk_list_store_new (1, G_TYPE_STRING);
    
    gtk_list_store_append (list_pond, &Iter);
    gtk_list_store_set (list_pond, &Iter, 0, gettext ("Tout"), -1);
    
    gtk_combo_box_set_model (UI_RES.combobox_ponderations,
                             GTK_TREE_MODEL (list_pond));
    g_object_unref (list_pond);
  }
  
  return;
}


void
EF_gtk_resultats_ponderations_change (GtkWidget *widget,
                                      Projet    *p)
/**
 * \brief Met à jour l'affichage des résultats en cas de changement de
 *        combinaisons.
 * \param widget : le composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
{
  GList *list_parcours;
  
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_RES.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Résultats");)
  
  list_parcours = UI_RES.tableaux;
  
  while (list_parcours != NULL)
  {
    BUG (EF_gtk_resultats_remplit_page (list_parcours->data, p), )
    
    list_parcours = g_list_next (list_parcours);
  }
  
  return;
}


void
EF_gtk_resultats_combobox_changed (GtkComboBox *combobox,
                                   Projet      *p)
/**
 * \brief Met à jour l'affichage des résultats en cas de changement de cas /
 *        combinaison.
 * \param combobox : le composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
{
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_RES.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Résultats");)
  
  // Pour forcer le combobox cas à être vierge.
  if (gtk_combo_box_get_model (UI_RES.combobox_ponderations) != NULL)
  {
    gtk_combo_box_set_active (UI_RES.combobox_ponderations, -1);
    gtk_combo_box_set_model (UI_RES.combobox_ponderations, NULL);
  }
  if (gtk_combo_box_get_model (UI_RES.combobox_cas) != NULL)
  {
    gtk_combo_box_set_active (UI_RES.combobox_cas, -1);
    gtk_combo_box_set_model (UI_RES.combobox_cas, NULL);
  }
  
  // Actions élémentaires
  if (gtk_combo_box_get_active (combobox) == 0)
  {
    gtk_combo_box_set_model (UI_RES.combobox_cas,
                             GTK_TREE_MODEL (UI_ACT.liste));
    gtk_widget_set_visible (GTK_WIDGET (UI_RES.combobox_ponderations), FALSE);
    gtk_widget_set_hexpand (GTK_WIDGET (UI_RES.combobox_cas), TRUE);
  }
  // Combinaisons ou enveloppes
  else
  {
    g_object_ref (p->combinaisons.list_el_desc);
    gtk_combo_box_set_model (UI_RES.combobox_cas,
                             GTK_TREE_MODEL (p->combinaisons.list_el_desc));
    gtk_widget_set_visible (GTK_WIDGET (UI_RES.combobox_ponderations), TRUE);
    gtk_widget_set_hexpand (GTK_WIDGET (UI_RES.combobox_cas), FALSE);
    EF_gtk_resultats_cas_change (NULL, p);
  }
  
  return;
}


void
EF_gtk_resultats_add_page_type (GtkMenuItem *menuitem,
                                Projet      *p)
/**
 * \brief Ajoute une page au Notebook sur la base de divers tableaux "de base".
 * \param menuitem : le composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
{
  Gtk_EF_Resultats_Tableau *res;
  
  BUGPARAMCRIT (p, "%p", p, )
  BUGPARAMCRIT (menuitem, "%p", menuitem, )
  BUGCRIT (UI_RES.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Résultats");)
  BUGCRIT (res = malloc (sizeof (Gtk_EF_Resultats_Tableau)),
           ,
           (gettext ("Erreur d'allocation mémoire.\n"));)
  
  if (strcmp (gtk_menu_item_get_label (menuitem), gettext ("Noeuds")) == 0)
  {
    BUGCRIT (res->col_tab = malloc (sizeof (Colonne_Resultats) * 5),
             ,
             (gettext ("Erreur d'allocation mémoire.\n"));
               free (res);)
    res->col_tab[0] = 4;
    res->col_tab[1] = COLRES_NUM_NOEUDS;
    res->col_tab[2] = COLRES_NOEUDS_X;
    res->col_tab[3] = COLRES_NOEUDS_Y;
    res->col_tab[4] = COLRES_NOEUDS_Z;
    
    res->filtre = FILTRE_AUCUN;
    
    BUGCRIT (res->nom = g_strdup_printf ("%s",
                                         gtk_menu_item_get_label (menuitem)),
             ,
             (gettext ("Erreur d'allocation mémoire.\n"));
               free (res->col_tab);
               free (res);)
    
    BUG (EF_gtk_resultats_add_page (res, p), )
    
    UI_RES.tableaux = g_list_append (UI_RES.tableaux, res);
  }
  else if (strcmp (gtk_menu_item_get_label (menuitem),
                   gettext("Réactions d'appuis (repère global)")) == 0)
  {
    BUGCRIT (res->col_tab = malloc (sizeof (Colonne_Resultats) * 8),
             ,
             (gettext ("Erreur d'allocation mémoire.\n"));
               free (res);)
    res->col_tab[0] = 7;
    res->col_tab[1] = COLRES_NUM_NOEUDS;
    res->col_tab[2] = COLRES_REACTION_APPUI_FX;
    res->col_tab[3] = COLRES_REACTION_APPUI_FY;
    res->col_tab[4] = COLRES_REACTION_APPUI_FZ;
    res->col_tab[5] = COLRES_REACTION_APPUI_MX;
    res->col_tab[6] = COLRES_REACTION_APPUI_MY;
    res->col_tab[7] = COLRES_REACTION_APPUI_MZ;
    
    res->filtre = FILTRE_NOEUD_APPUI;
    
    BUGCRIT (res->nom = g_strdup_printf ("%s",
                                         gtk_menu_item_get_label (menuitem)),
             ,
             (gettext ("Erreur d'allocation mémoire.\n"));
               free (res->col_tab);
               free (res);)
    
    BUG (EF_gtk_resultats_add_page (res, p), )
    
    UI_RES.tableaux = g_list_append (UI_RES.tableaux, res);
  }
  else if (strcmp (gtk_menu_item_get_label (menuitem),
                   gettext ("Déplacements (repère global)")) == 0)
  {
    BUGCRIT (res->col_tab = malloc (sizeof (Colonne_Resultats) * 8),
             ,
             (gettext ("Erreur d'allocation mémoire.\n"));
               free (res);)
    res->col_tab[0] = 7;
    res->col_tab[1] = COLRES_NUM_NOEUDS;
    res->col_tab[2] = COLRES_DEPLACEMENT_UX;
    res->col_tab[3] = COLRES_DEPLACEMENT_UY;
    res->col_tab[4] = COLRES_DEPLACEMENT_UZ;
    res->col_tab[5] = COLRES_DEPLACEMENT_RX;
    res->col_tab[6] = COLRES_DEPLACEMENT_RY;
    res->col_tab[7] = COLRES_DEPLACEMENT_RZ;
    
    res->filtre = FILTRE_AUCUN;
    
    BUGCRIT (res->nom = g_strdup_printf ("%s",
                                         gtk_menu_item_get_label (menuitem)),
             ,
             (gettext ("Erreur d'allocation mémoire.\n"));
               free (res->col_tab);
               free (res);)
    
    BUG (EF_gtk_resultats_add_page (res, p), )
    
    UI_RES.tableaux = g_list_append (UI_RES.tableaux, res);
  }
  else if (strcmp (gtk_menu_item_get_label (menuitem),
                   gettext ("Barres")) == 0)
  {
    BUGCRIT (res->col_tab = malloc (sizeof (Colonne_Resultats) * 3),
             ,
             (gettext ("Erreur d'allocation mémoire.\n"));
               free (res);)
    res->col_tab[0] = 2;
    res->col_tab[1] = COLRES_NUM_BARRES;
    res->col_tab[2] = COLRES_BARRES_LONGUEUR;
    
    res->filtre = FILTRE_AUCUN;
    
    BUGCRIT (res->nom = g_strdup_printf ("%s",
                                         gtk_menu_item_get_label (menuitem)),
             ,
             (gettext ("Erreur d'allocation mémoire.\n"));
               free (res->col_tab);
               free (res);)
    
    BUG (EF_gtk_resultats_add_page (res, p), )
    
    UI_RES.tableaux = g_list_append (UI_RES.tableaux, res);
  }
  else if (strcmp (gtk_menu_item_get_label (menuitem),
                   gettext ("Efforts dans les barres (repère local)")) == 0)
  {
    BUGCRIT (res->col_tab = malloc (sizeof (Colonne_Resultats) * 21),
             ,
             (gettext ("Erreur d'allocation mémoire.\n"));
               free (res);)
    res->col_tab[0] = 20;
    res->col_tab[1] = COLRES_NUM_BARRES;
    res->col_tab[2] = COLRES_BARRES_LONGUEUR;
    res->col_tab[3] = COLRES_BARRES_PIXBUF_N;
    res->col_tab[4] = COLRES_BARRES_EQ_N;
    res->col_tab[5] = COLRES_BARRES_DESC_N;
    res->col_tab[6] = COLRES_BARRES_PIXBUF_TY;
    res->col_tab[7] = COLRES_BARRES_EQ_TY;
    res->col_tab[8] = COLRES_BARRES_DESC_TY;
    res->col_tab[9] = COLRES_BARRES_PIXBUF_TZ;
    res->col_tab[10] = COLRES_BARRES_EQ_TZ;
    res->col_tab[11] = COLRES_BARRES_DESC_TZ;
    res->col_tab[12] = COLRES_BARRES_PIXBUF_MX;
    res->col_tab[13] = COLRES_BARRES_EQ_MX;
    res->col_tab[14] = COLRES_BARRES_DESC_MX;
    res->col_tab[15] = COLRES_BARRES_PIXBUF_MY;
    res->col_tab[16] = COLRES_BARRES_EQ_MY;
    res->col_tab[17] = COLRES_BARRES_DESC_MY;
    res->col_tab[18] = COLRES_BARRES_PIXBUF_MZ;
    res->col_tab[19] = COLRES_BARRES_EQ_MZ;
    res->col_tab[20] = COLRES_BARRES_DESC_MZ;
    
    res->filtre = FILTRE_AUCUN;
    
    BUGCRIT (res->nom = g_strdup_printf ("%s",
                                         gtk_menu_item_get_label (menuitem)),
             ,
             (gettext ("Erreur d'allocation mémoire.\n"));
               free (res->col_tab);
               free (res);)
    
    BUG (EF_gtk_resultats_add_page (res, p), )
    
    UI_RES.tableaux = g_list_append (UI_RES.tableaux, res);
  }
  else if (strcmp (gtk_menu_item_get_label (menuitem),
                   gettext ("Déformations des barres (repère local)")) == 0)
  {
    BUGCRIT (res->col_tab = malloc (sizeof (Colonne_Resultats) * 21),
             ,
             (gettext ("Erreur d'allocation mémoire.\n"));
               free (res);)
    res->col_tab[0] = 20;
    res->col_tab[1] = COLRES_NUM_BARRES;
    res->col_tab[2] = COLRES_BARRES_LONGUEUR;
    res->col_tab[3] = COLRES_DEF_PIXBUF_UX;
    res->col_tab[4] = COLRES_DEF_UX;
    res->col_tab[5] = COLRES_DEF_DESC_UX;
    res->col_tab[6] = COLRES_DEF_PIXBUF_UY;
    res->col_tab[7] = COLRES_DEF_UY;
    res->col_tab[8] = COLRES_DEF_DESC_UY;
    res->col_tab[9] = COLRES_DEF_PIXBUF_UZ;
    res->col_tab[10] = COLRES_DEF_UZ;
    res->col_tab[11] = COLRES_DEF_DESC_UZ;
    res->col_tab[12] = COLRES_DEF_PIXBUF_RX;
    res->col_tab[13] = COLRES_DEF_RX;
    res->col_tab[14] = COLRES_DEF_DESC_RX;
    res->col_tab[15] = COLRES_DEF_PIXBUF_RY;
    res->col_tab[16] = COLRES_DEF_RY;
    res->col_tab[17] = COLRES_DEF_DESC_RY;
    res->col_tab[18] = COLRES_DEF_PIXBUF_RZ;
    res->col_tab[19] = COLRES_DEF_RZ;
    res->col_tab[20] = COLRES_DEF_DESC_RZ;
    
    res->filtre = FILTRE_AUCUN;
    
    BUGCRIT (res->nom = g_strdup_printf ("%s",
                                         gtk_menu_item_get_label (menuitem)),
             ,
             (gettext ("Erreur d'allocation mémoire.\n"));
               free (res->col_tab);
               free (res);)
    
    BUG (EF_gtk_resultats_add_page (res, p), )
    
    UI_RES.tableaux = g_list_append (UI_RES.tableaux, res);
  }
  
  return;
}


void
EF_gtk_resultats (Projet *p)
/**
 * \brief Création de la fenêtre permettant d'afficher les résultats sous forme
 *        d'un tableau.
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique impossible à générer.
 */
{
  GList *list_parcours;
  
  BUGPARAM (p, "%p", p, )
  
  if (UI_RES.builder != NULL)
  {
    gtk_window_present (GTK_WINDOW (UI_RES.window));
    return;
  }
  
  UI_RES.builder = gtk_builder_new ();
  BUGCRIT (gtk_builder_add_from_resource (UI_RES.builder,
                                        "/org/2lgc/codegui/ui/EF_resultats.ui",
                                          NULL) != 0,
           ,
           (gettext ("La génération de la fenêtre %s a échouée.\n"),
                     "Résultats");)
  gtk_builder_connect_signals (UI_RES.builder, p);
  
  UI_RES.window = GTK_WIDGET (gtk_builder_get_object (UI_RES.builder,
                                                      "EF_resultats_window"));
  UI_RES.notebook = GTK_NOTEBOOK (gtk_builder_get_object (UI_RES.builder,
                                                     "EF_resultats_notebook"));
  
  UI_RES.combobox = GTK_COMBO_BOX (gtk_builder_get_object (UI_RES.builder,
                                                     "EF_resultats_combobox"));
  UI_RES.combobox_cas = GTK_COMBO_BOX (gtk_builder_get_object (UI_RES.builder,
                                                "EF_resultats_combo_box_cas"));
  UI_RES.combobox_ponderations = GTK_COMBO_BOX (gtk_builder_get_object (
                       UI_RES.builder, "EF_resultats_combo_box_ponderations"));
  
  gtk_combo_box_set_active (UI_RES.combobox, 0);
  
  list_parcours = UI_RES.tableaux;
  while (list_parcours != NULL)
  {
    BUG (EF_gtk_resultats_add_page (list_parcours->data, p), )
    
    list_parcours = g_list_next (list_parcours);
  }
  
  gtk_window_set_transient_for (GTK_WINDOW (UI_RES.window),
                                GTK_WINDOW (UI_GTK.window));
  
  return;
}


void
EF_gtk_resultats_free (Projet *p)
/**
 * \brief Libère l'ensemble des éléments utilisés pour l'affichage des
 *        résultats.
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 */
{
  BUGPARAM (p, "%p", p, )
  
  while (UI_RES.tableaux != NULL)
  {
    Gtk_EF_Resultats_Tableau *res = UI_RES.tableaux->data;
    
    g_object_unref (res->list_store);
    free (res->col_tab);
    free (res->nom);
    free (res);
    
    UI_RES.tableaux = g_list_delete_link (UI_RES.tableaux, UI_RES.tableaux);
  }
  
  return;
}
#endif

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
