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

#include <gtk/gtk.h>
#include <string.h>

#include <algorithm>
#include <locale>

#include "1990_action.hpp"
#include "1990_ponderations.hpp"
#include "common_projet.hpp"
#include "common_erreurs.hpp"
#include "common_fonction.hpp"
#include "common_gtk.hpp"
#include "common_math.hpp"
#include "common_text.hpp"
#include "EF_noeuds.hpp"
#include "EF_resultat.hpp"
#include "EF_gtk_sections.hpp"
#include "EF_gtk_resultats.hpp"


GTK_WINDOW_CLOSE (ef, resultats);


GTK_WINDOW_DESTROY (ef, resultats, );


GTK_WINDOW_KEY_PRESS (ef, resultats);


/**
 * \brief Empêche le changement de page si on clique sur le dernier onglet sans
 *        cliquer sur le bouton d'ajout d'onglet.
 * \param notebook : le composant notebook,
 * \param page : composant désignant la page,
 * \param page_num : numéro de la page,
 * \param p : la variable projet.
 * \return Rien.
 */
extern "C"
void
EF_gtk_resultats_notebook_switch (GtkNotebook *notebook,
                                  GtkWidget   *page,
                                  gint         page_num,
                                  Projet      *p)
{
// En suspend pour le moment afin que les flèches permettant de faire défiler
// les onglets affichent cette dernière page.
//  if (page_num == gtk_notebook_get_n_pages (notebook) - 1)
//    g_signal_stop_emission_by_name (notebook, "switch-page");
  
  return;
}


/**
 * \brief Remplit/actualise la page du treeview via la variable res.
 * \param res : caractéristiques de la page à remplir,
 * \param p : la variable projet.
 * \return true.\n
 *   Echec : false,
 *     - p == NULL,
 *     - res == false,
 *     - interface graphique non initialisée.
 */
bool
EF_gtk_resultats_remplit_page (Gtk_EF_Resultats_Tableau *res,
                               Projet                   *p)
{
  uint8_t i;
  Action *action;
  
  std::list <Action *> actions;
  
  std::list <std::list <Ponderation *> *> *comb;
  
  BUGPARAMCRIT (p, "%p", p, false)
  BUGPARAMCRIT (res, "%p", res, false)
  BUGCRIT (UI_RES.builder,
           false,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Résultats"); )
  
  switch (gtk_combo_box_get_active (UI_RES.combobox))
  {
    // Actions élémentaires
    case 0 :
    {
      if (gtk_combo_box_get_active (UI_RES.combobox_cas) == -1)
      {
        gtk_list_store_clear (res->list_store);
        return true;
      }
      BUG (action = *std::next (p->actions.begin (),
                                gtk_combo_box_get_active (
                                                         UI_RES.combobox_cas)),
           false)
      actions.push_back (action);
      break;
    }
    // Combinaisons
    case 1 :
    {
      if ((gtk_combo_box_get_active (UI_RES.combobox_cas) == -1) ||
          (gtk_combo_box_get_active (UI_RES.combobox_ponderations) == -1))
      {
        gtk_list_store_clear (res->list_store);
        return true;
      }
      
      // On cherche la combinaison à afficher.
      switch (gtk_combo_box_get_active (UI_RES.combobox_cas))
      {
        case 0 :
        {
          comb = &p->ponderations.elu_equ;
          break;
        }
        case 1 :
        {
          comb = &p->ponderations.elu_str;
          break;
        }
        case 2 :
        {
          comb = &p->ponderations.elu_geo;
          break;
        }
        case 3 :
        {
          comb = &p->ponderations.elu_fat;
          break;
        }
        case 4 :
        {
          comb = &p->ponderations.elu_acc;
          break;
        }
        case 5 :
        {
          comb = &p->ponderations.elu_sis;
          break;
        }
        case 6 :
        {
          comb = &p->ponderations.els_car;
          break;
        }
        case 7 :
        {
          comb = &p->ponderations.els_freq;
          break;
        }
        case 8 :
        {
          comb = &p->ponderations.els_perm;
          break;
        }
        default :
        {
          FAILCRIT (false,
                    (gettext ("Paramètre %s incorrect.\n"), "combobox_cas"); )
          break;
        }
      }
      
      BUG (action = _1990_action_ponderation_resultat (
                      *std::next (comb->begin (),
                                  gtk_combo_box_get_active (
                                      UI_RES.combobox_ponderations)),
                    p),
           false)
      actions.push_back (action);
      break;
    }
    case 2 :
    {
      if ((gtk_combo_box_get_active (UI_RES.combobox_cas) == -1) ||
          (gtk_combo_box_get_active (UI_RES.combobox_ponderations) == -1))
      {
        gtk_list_store_clear (res->list_store);
        return true;
      }
      
      // On cherche la combinaison à afficher.
      switch (gtk_combo_box_get_active (UI_RES.combobox_cas))
      {
        case 0 :
        {
          comb = &p->ponderations.elu_equ;
          break;
        }
        case 1 :
        {
          comb = &p->ponderations.elu_str;
          break;
        }
        case 2 :
        {
          comb = &p->ponderations.elu_geo;
          break;
        }
        case 3 :
        {
          comb = &p->ponderations.elu_fat;
          break;
        }
        case 4 :
        {
          comb = &p->ponderations.elu_acc;
          break;
        }
        case 5 :
        {
          comb = &p->ponderations.elu_sis;
          break;
        }
        case 6 :
        {
          comb = &p->ponderations.els_car;
          break;
        }
        case 7 :
        {
          comb = &p->ponderations.els_freq;
          break;
        }
        case 8 :
        {
          comb = &p->ponderations.els_perm;
          break;
        }
        default :
        {
          FAILCRIT (false,
                    (gettext ("Paramètre %s incorrect.\n"), "combobox_cas"); )
          break;
        }
      }
      
      if (gtk_combo_box_get_active (UI_RES.combobox_ponderations) == 0)
      {
        std::list <std::list <Ponderation *> *>::iterator it;
        
        it = comb->begin ();
        while (it != comb->end ())
        {
          BUG (action = _1990_action_ponderation_resultat (*it, p), false)
          actions.push_back (action);
          
          ++it;
        }
      }
      break;
    }
    default :
    {
      FAILCRIT (false,
                (gettext ("Paramètre %s incorrect.\n"), "combobox"); )
      break;
    }
  }
  BUGCRIT (!actions.empty (),
           false,
           (gettext ("Impossible\n")); )
  
  #define FREE_ALL { \
    if (gtk_combo_box_get_active (UI_RES.combobox) != 0) \
    { \
      std::list <Action *>::iterator it_ = actions.begin (); \
      \
      while (it_ != actions.end ()) \
      { \
        action = *it_; \
        BUG (_1990_action_fonction_free (action), false) \
        BUG (_1990_action_ponderation_resultat_free_calculs (action), false) \
        delete action; \
        \
        ++it_; \
      } \
    } \
  }
  
  gtk_list_store_clear (res->list_store);
  
  switch (res->col_tab[1])
  {
    case COLRES_NUM_NOEUDS :
    {
      std::list <EF_Noeud *>::iterator it;
      
      it = p->modele.noeuds.begin ();
      
      i = 0;
      
      while (it != p->modele.noeuds.end ())
      {
        EF_Noeud *noeud = *it;
        bool      ok;
        
        switch (res->filtre)
        {
          case FILTRE_AUCUN :
          {
            ok = true;
            break;
          }
          case FILTRE_NOEUD_APPUI :
          {
            if (noeud->appui == NULL)
            {
              ok = false;
            }
            else
            {
              ok = true;
            }
            break;
          }
          default :
          {
            FAILCRIT (false,
                      (gettext ("Le filtre %d est inconnu.\n"), res->filtre);
                        FREE_ALL)
            break;
          }
        }
        
        if (ok)
        {
          GtkTreeIter Iter;
          uint8_t     j;
          std::string tmp_double;
          std::string tmp_double30;
          
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
                
                BUG (EF_noeuds_renvoie_position (noeud, &point),
                     false,
                     FREE_ALL)
                conv_f_c (point.x, &tmp_double30, DECIMAL_DISTANCE);
                
                gtk_list_store_set (res->list_store,
                                    &Iter,
                                    j - 1, tmp_double30.c_str (),
                                    -1);
                
                break;
              }
              case COLRES_NOEUDS_Y :
              {
                EF_Point point;
                
                BUG (EF_noeuds_renvoie_position (noeud, &point),
                     false,
                     FREE_ALL)
                conv_f_c (point.y, &tmp_double30, DECIMAL_DISTANCE);
                
                gtk_list_store_set (res->list_store,
                                    &Iter,
                                    j - 1, tmp_double30.c_str (),
                                    -1);
                
                break;
              }
              case COLRES_NOEUDS_Z :
              {
                EF_Point point;
                
                BUG (EF_noeuds_renvoie_position (noeud, &point),
                     false,
                     FREE_ALL)
                conv_f_c (point.z, &tmp_double30, DECIMAL_DISTANCE);
                
                gtk_list_store_set (res->list_store,
                                    &Iter,
                                    j - 1, tmp_double30.c_str (),
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
                BUG (EF_resultat_noeud_reaction_appui (
                       &actions,
                       noeud,
                       (uint8_t) (res->col_tab[j] - COLRES_REACTION_APPUI_FX),
                       p,
                       &tmp_double,
                       NULL,
                       NULL),
                     false,
                     FREE_ALL)
                gtk_list_store_set (res->list_store,
                                    &Iter,
                                    j - 1, tmp_double.c_str (),
                                    -1);
                break;
              }
              case COLRES_DEPLACEMENT_UX :
              case COLRES_DEPLACEMENT_UY :
              case COLRES_DEPLACEMENT_UZ :
              case COLRES_DEPLACEMENT_RX :
              case COLRES_DEPLACEMENT_RY :
              case COLRES_DEPLACEMENT_RZ :
              {
                BUG (EF_resultat_noeud_deplacement (
                       &actions,
                       noeud,
                       (uint8_t) (res->col_tab[j] - COLRES_DEPLACEMENT_UX),
                       p,
                       &tmp_double,
                       NULL,
                       NULL),
                     false,
                     FREE_ALL)
                gtk_list_store_set (res->list_store,
                                    &Iter,
                                    j - 1, tmp_double.c_str (),
                                    -1);
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
                FAILCRIT (false,
                          (gettext ("La colonne des résultats %d ne peut être appliquée aux noeuds."),
                                    res->col_tab[j]);
                            FREE_ALL)
                break;
              }
              default :
              {
                FAILCRIT (false,
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
        ++it;
      }
      break;
    }
     
    case COLRES_NUM_BARRES :
    {
      std::list <EF_Barre *>::iterator it;
      
      it = p->modele.barres.begin ();
      
      i = 0;
      
      while (it != p->modele.barres.end ())
      {
        EF_Barre *barre = *it;
        bool      ok;
        
        switch (res->filtre)
        {
          case FILTRE_AUCUN :
          {
            ok = true;
            break;
          }
          case FILTRE_NOEUD_APPUI :
          {
            FAILCRIT (false,
                      (gettext ("Le filtre %d ne peut être appliqué aux barres.\n"),
                                res->filtre);
                        FREE_ALL)
            break;
          }
          default :
          {
            FAILCRIT (false,
                      (gettext ("Le filtre %d est inconnu.\n"),
                                res->filtre);
                        FREE_ALL)
            break;
          }
        }
        
        if (ok)
        {
          GtkTreeIter Iter;
          uint8_t     j;
          
          gtk_list_store_append (res->list_store, &Iter);
          
          for (j = 1; j <= res->col_tab[0]; j++)
          {
            Fonction comb_min, comb_max;
            Fonction f_min, f_max;
            std::string tmp1, tmp2;
            std::string tmp;
            std::list <Fonction *> liste;
            std::list <std::list <Ponderation *> *> *converti = NULL;
            
            std::list <Action *>::iterator it2;
            
    #define FREE_ALL2 free (comb_min.troncons); \
    free (comb_max.troncons); \
    free (f_min.troncons); \
    free (f_max.troncons); \
    delete converti;
            
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
                FAILCRIT (false,
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
                
                it2 = actions.begin ();
                while (it2 != actions.end ())
                {
                  Action *action2 = *it2;
                  
                  liste.push_back (_1990_action_efforts_renvoie (
                                     action2,
                                       (uint8_t) (res->col_tab[j] -
                                                       COLRES_BARRES_PIXBUF_N),
                                     i));
                  
                  ++it2;
                }
                
                pixbuf = common_fonction_dessin (
                           &liste,
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
                it2 = actions.begin ();
                
                if (std::next (it2) != actions.end ())
                {
                  while (it2 != actions.end ())
                  {
                    Action *action2 = *it2;
                    
                    liste.push_back (_1990_action_efforts_renvoie (
                                       action2,
                                       (uint8_t) (res->col_tab[j] -
                                                         COLRES_BARRES_DESC_N),
                                       i));
                    
                    ++it2;
                  }
                  
                  if (!common_fonction_renvoie_enveloppe (&liste,
                                                          &f_min,
                                                          &f_max,
                                                          &comb_min,
                                                          &comb_max))
                  {
                    tmp = gettext ("Erreur");
                  }
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
                    
                    tmp = format (gettext ("Enveloppe supérieure :\n%s\nEnveloppe inférieure :\n%s"),
                                           tmp2.c_str (),
                                           tmp1.c_str ());
                  }
                }
                else
                {
                  Action *action2 = *it2;
                  
                  tmp = common_fonction_affiche_caract (
                          _1990_action_efforts_renvoie (
                            action2,
                            (uint8_t) (res->col_tab[j] - COLRES_BARRES_DESC_N),
                            i),
                          DECIMAL_DISTANCE,
                          res->col_tab[j] < COLRES_BARRES_DESC_MX ?
                            DECIMAL_FORCE :
                            DECIMAL_MOMENT);
                }
                gtk_list_store_set (res->list_store,
                                    &Iter,
                                    j - 1, tmp.c_str (),
                                    -1);
                
                break;
              }
              case COLRES_BARRES_EQ_N :
              case COLRES_BARRES_EQ_TY :
              case COLRES_BARRES_EQ_TZ :
              case COLRES_BARRES_EQ_MX :
              case COLRES_BARRES_EQ_MY :
              case COLRES_BARRES_EQ_MZ :
              {
                it2 = actions.begin ();
                
                if (std::next (it2) != actions.end ())
                {
                  while (it2 != actions.end ())
                  {
                    Action *action2 = *it2;
                    
                    liste.push_back (_1990_action_efforts_renvoie (
                                       action2,
                                       (uint8_t) (res->col_tab[j] -
                                                           COLRES_BARRES_EQ_N),
                                       i));
                    
                    ++it2;
                  }
                  
                  if (!common_fonction_renvoie_enveloppe (&liste,
                                                          &f_min,
                                                          &f_max,
                                                          &comb_min,
                                                          &comb_max))
                  {
                    tmp = gettext ("Erreur");
                  }
                  else
                  {
                    BUG (common_fonction_conversion_combinaisons (&comb_min,
                                                                  comb,
                                                                  &converti),
                         false,
                         FREE_ALL
                           FREE_ALL2)
                    tmp1 = common_fonction_renvoie (
                             &f_min,
                             converti,
                             res->col_tab[j] < COLRES_BARRES_EQ_MX ?
                               DECIMAL_FORCE :
                               DECIMAL_MOMENT);
                    delete converti;
                    converti = NULL;
                    BUG (common_fonction_conversion_combinaisons (&comb_max,
                                                                  comb,
                                                                  &converti),
                         false,
                         FREE_ALL
                           FREE_ALL2)
                    tmp2 = common_fonction_renvoie (
                             &f_max,
                             converti,
                             res->col_tab[j] < COLRES_BARRES_EQ_MX ?
                               DECIMAL_FORCE :
                               DECIMAL_MOMENT);
                    
                    tmp = format  (gettext ("Enveloppe supérieure :\n%s\nEnveloppe inférieure :\n%s"),
                                            tmp2.c_str (),
                                            tmp1.c_str ());
                  }
                }
                else
                {
                  Action *action2 = *it2;
                  tmp = common_fonction_renvoie (
                          _1990_action_efforts_renvoie (
                            action2,
                            (uint8_t) (res->col_tab[j] - COLRES_BARRES_EQ_N),
                            i),
                          NULL,
                          res->col_tab[j] < COLRES_BARRES_EQ_MX ?
                            DECIMAL_FORCE :
                            DECIMAL_MOMENT);
                }
                gtk_list_store_set (res->list_store,
                                    &Iter,
                                    j - 1, tmp.c_str (),
                                    -1);
                
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
                
                it2 = actions.begin ();
                
                while (it2 != actions.end ())
                {
                  Action *action2 = *it2;
                  
                  if (res->col_tab[j] < COLRES_DEF_PIXBUF_RX)
                  {
                    liste.push_back (_1990_action_deformation_renvoie (
                                       action2,
                                       (uint8_t) (res->col_tab[j] -
                                                         COLRES_DEF_PIXBUF_UX),
                                       i));
                  }
                  else
                  {
                    liste.push_back (_1990_action_rotation_renvoie (
                                       action2,
                                       (uint8_t) (res->col_tab[j] -
                                                         COLRES_DEF_PIXBUF_RX),
                                       i));
                  }
                  
                  ++it2;
                }
                
                pixbuf = common_fonction_dessin (
                           &liste,
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
                it2 = actions.begin ();
                
                if (std::next (it2) != actions.end ())
                {
                  while (it2 != actions.end ())
                  {
                    Action *action2 = *it2;
                    
                    if (res->col_tab[j] < COLRES_DEF_DESC_RX)
                    {
                      liste.push_back (_1990_action_deformation_renvoie (
                                         action2,
                                         (uint8_t) (res->col_tab[j] -
                                                           COLRES_DEF_DESC_UX),
                                         i));
                    }
                    else
                    {
                      liste.push_back (_1990_action_rotation_renvoie (
                                         action2,
                                         (uint8_t) (res->col_tab[j] -
                                                           COLRES_DEF_DESC_RX),
                                         i));
                    }
                    
                    ++it2;
                  }
                  
                  if (!common_fonction_renvoie_enveloppe (&liste,
                                                          &f_min,
                                                          &f_max,
                                                          &comb_min,
                                                          &comb_max))
                  {
                    tmp = gettext ("Erreur");
                  }
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
                    
                    tmp = format (gettext ("Enveloppe supérieure :\n%s\nEnveloppe inférieure :\n%s"),
                                           tmp2.c_str (),
                                           tmp1.c_str ());
                  }
                }
                else
                {
                  Action *action2 = *it2;
                  
                  if (res->col_tab[j] < COLRES_DEF_DESC_RX)
                  {
                    tmp = common_fonction_affiche_caract (
                            _1990_action_deformation_renvoie (
                              action2,
                              (uint8_t) (res->col_tab[j] - COLRES_DEF_DESC_UX),
                              i),
                            DECIMAL_DISTANCE,
                            DECIMAL_DEPLACEMENT);
                  }
                  else
                  {
                    tmp = common_fonction_affiche_caract (
                            _1990_action_rotation_renvoie (
                              action2,
                              (uint8_t) (res->col_tab[j] - COLRES_DEF_DESC_RX),
                              i),
                            DECIMAL_DISTANCE,
                            DECIMAL_ROTATION);
                  }
                }
                gtk_list_store_set (res->list_store,
                                    &Iter,
                                    j - 1, tmp.c_str (),
                                    -1);
                
                break;
              }
              case COLRES_DEF_UX :
              case COLRES_DEF_UY :
              case COLRES_DEF_UZ :
              case COLRES_DEF_RX :
              case COLRES_DEF_RY :
              case COLRES_DEF_RZ :
              {
                it2 = actions.begin ();
                
                if (std::next (it2) != actions.end ())
                {
                  while (it2 != actions.end ())
                  {
                    Action *action2 = *it2;
                    
                    if (res->col_tab[j] < COLRES_DEF_RX)
                    {
                      liste.push_back (_1990_action_deformation_renvoie (
                                         action2,
                                         (uint8_t) (res->col_tab[j] -
                                                                COLRES_DEF_UX),
                                         i));
                    }
                    else
                    {
                      liste.push_back (_1990_action_rotation_renvoie (
                                         action2,
                                         (uint8_t) (res->col_tab[j] -
                                                                COLRES_DEF_RX),
                                         i));
                    }
                    
                    ++it2;
                  }
                  
                  if (!common_fonction_renvoie_enveloppe (&liste,
                                                          &f_min,
                                                          &f_max,
                                                          &comb_min,
                                                          &comb_max))
                  {
                    tmp = gettext ("Erreur");
                  }
                  else
                  {
                    BUG (common_fonction_conversion_combinaisons (&comb_min,
                                                                  comb,
                                                                  &converti),
                         false,
                         FREE_ALL
                           FREE_ALL2)
                    tmp1 = common_fonction_renvoie (
                             &f_min,
                             converti,
                             res->col_tab[j] < COLRES_DEF_RX ?
                               DECIMAL_DEPLACEMENT :
                               DECIMAL_ROTATION);
                    delete converti;
                    converti = NULL;
                    BUG (common_fonction_conversion_combinaisons (&comb_max,
                                                                  comb,
                                                                  &converti),
                         false,
                         FREE_ALL
                           FREE_ALL2)
                    tmp2 = common_fonction_renvoie (
                             &f_max,
                             converti,
                             res->col_tab[j] < COLRES_DEF_RX ?
                               DECIMAL_DEPLACEMENT :
                               DECIMAL_ROTATION);
                    
                    tmp = format (gettext ("Enveloppe supérieure :\n%s\nEnveloppe inférieure :\n%s"),
                                           tmp2.c_str (),
                                           tmp1.c_str ());
                  }
                }
                else
                {
                  Action *action2 = *it2;
                  
                  if (res->col_tab[j] < COLRES_DEF_RX)
                  {
                    tmp = common_fonction_renvoie (
                            _1990_action_deformation_renvoie (
                              action2,
                              (uint8_t) (res->col_tab[j] - COLRES_DEF_UX),
                              i),
                            NULL,
                            DECIMAL_DEPLACEMENT);
                  }
                  else
                  {
                    tmp = common_fonction_renvoie (
                            _1990_action_rotation_renvoie (
                              action2,
                              (uint8_t) (res->col_tab[j] - COLRES_DEF_RX),
                              i),
                            NULL,
                            DECIMAL_ROTATION);
                  }
                }
                gtk_list_store_set (res->list_store,
                                    &Iter,
                                    j - 1, tmp.c_str (),
                                    -1);
                
                break;
              }
              default :
              {
                FAILINFO (false,
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
        ++it;
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
      FAILCRIT (false,
                (gettext ("La première colonne ne peut contenir que la liste des nœuds ou des barres.\n")); )
      break;
    }
  }
  
  FREE_ALL
  
  return true;
#undef FREE_ALL2
#undef FREE_ALL
}


/**
 * \brief Ajoute une page au treeview de la fenêtre affichant les résultats en
 *        fonction de la description fournie via la variable res.
 * \param res : caractéristiques de la page à ajouter,
 * \param p : la variable projet.
 * \return true.\n
 *   Echec : false .
 *     - p == NULL,
 *     - res == NULL,
 *     - interface graphique non initialisée.
 */
bool
EF_gtk_resultats_add_page (Gtk_EF_Resultats_Tableau *res,
                           Projet                   *p)
{
  GtkWidget         *p_scrolled_window; 
  uint8_t            i;
  GType             *col_type;
  GtkCellRenderer   *cell;
  GtkTreeViewColumn *column;
  float              xalign;
  
  BUGPARAMCRIT (p, "%p", p, false)
  BUGPARAMCRIT (res, "%p", res, false)
  BUGCRIT (UI_RES.builder,
           false,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Résultats"); )
  
  p_scrolled_window = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (p_scrolled_window),
                                  GTK_POLICY_AUTOMATIC,
                                  GTK_POLICY_AUTOMATIC);
  
  gtk_notebook_insert_page (UI_RES.notebook,
                            p_scrolled_window,
                            GTK_WIDGET (gtk_label_new (res->nom.c_str ())),
                            gtk_notebook_get_n_pages (UI_RES.notebook) - 1);
   gtk_notebook_set_tab_reorderable (GTK_NOTEBOOK (UI_RES.notebook),
                                     p_scrolled_window,
                                     TRUE);
  
  // La variable xalign n'est utilisée que pour les résultats à virgule
  // flottante. Elle centre le texte si on affiche une enveloppe de résultats
  // (affichage sous forme ***/***) ou aligne à droite si elle affiche un
  // nombre à virgule flottante (pour aligner les virgules)
  if (gtk_combo_box_get_active (UI_RES.combobox) == 2)
  {
    xalign = 0.5;
  }
  else
  {
    xalign = 1.;
  }
  
  res->treeview = GTK_TREE_VIEW (gtk_tree_view_new ());
  gtk_container_add (GTK_CONTAINER (p_scrolled_window),
                     GTK_WIDGET (res->treeview));
  
  col_type = new GType[res->col_tab[0] + 1];
  
  for (i = 1; i <= res->col_tab[0]; i++)
  {
    std::string str_tmp;
    
    switch (res->col_tab[i])
    {
      case COLRES_NUM_NOEUDS :
      {
        BUGCRIT (i == 1,
                 false,
                 (gettext ("La liste des noeuds doit être spécifiée en tant que première colonne.\n"));
                   delete [] col_type; )
        col_type[i - 1] = G_TYPE_INT;
        str_tmp.assign (gettext ("Noeuds"));
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (&str_tmp,
                                   col_type[i - 1],
                                   i - 1U,
                                   0.5,
                                   0));
        break;
      }
      case COLRES_NUM_BARRES :
      {
        BUGCRIT (i == 1,
                 false,
                 (gettext ("La liste des barres doit être spécifiée en tant que première colonne.\n"));
                   delete [] col_type; )
        col_type[i - 1] = G_TYPE_INT;
        str_tmp.assign (gettext ("Barres"));
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (&str_tmp,
                                   col_type[i - 1],
                                   i - 1U,
                                   0.5,
                                   0));
        break;
      }
      case COLRES_NOEUDS_X :
      {
        BUGCRIT (i != 1,
                 false,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   delete [] col_type; )
        BUGCRIT (res->col_tab[1] == COLRES_NUM_NOEUDS,
                 false,
                 (gettext ("La position en %s ne peut être affichée que si la première colonne affiche les numéros des noeuds.\n"),
                          "x");
                   delete [] col_type; )
        col_type[i - 1] = G_TYPE_STRING;
        str_tmp.assign (gettext ("x [m]"));
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (&str_tmp,
                                   col_type[i - 1],
                                   i - 1U,
                                   1.,
                                   0));
        break;
      }
      case COLRES_NOEUDS_Y :
      {
        BUGCRIT (i != 1,
                 false,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   delete [] col_type; )
        BUGCRIT (res->col_tab[1] == COLRES_NUM_NOEUDS,
                 false,
                 (gettext ("La position en %s ne peut être affichée que si la première colonne affiche les numéros des noeuds.\n"),
                           "y");
                   delete [] col_type; )
        col_type[i - 1] = G_TYPE_STRING;
        str_tmp.assign (gettext ("y [m]"));
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (&str_tmp,
                                   col_type[i - 1],
                                   i - 1U,
                                   1.,
                                   0));
        break;
      }
      case COLRES_NOEUDS_Z :
      {
        BUGCRIT (i != 1,
                 false,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   delete [] col_type; )
        BUGCRIT (res->col_tab[1] == COLRES_NUM_NOEUDS,
                 false,
                 (gettext ("La position en %s ne peut être affichée que si la première colonne affiche les numéros des noeuds.\n"),
                           "z");
                   delete [] col_type; )
        col_type[i - 1] = G_TYPE_STRING;
        str_tmp.assign (gettext ("z [m]"));
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (&str_tmp,
                                   col_type[i - 1],
                                   i - 1U,
                                   1.,
                                   0));
        break;
      }
      case COLRES_REACTION_APPUI_FX :
      {
        BUGCRIT (i != 1,
                 false,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   delete [] col_type; )
        BUGCRIT (res->col_tab[1] == COLRES_NUM_NOEUDS,
                 false,
                 (gettext ("La réaction d'appui %s ne peut être affichée que si la première colonne affiche les numéros des noeuds.\n"),
                           "Fx");
                   delete [] col_type; )
        col_type[i - 1] = G_TYPE_STRING;
        str_tmp.assign (gettext ("F<sub>x</sub> [N]"));
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (&str_tmp,
                                   col_type[i - 1],
                                   i - 1U,
                                   xalign,
                                   0));
        break;
      }
      case COLRES_REACTION_APPUI_FY:
      {
        BUGCRIT (i != 1,
                 false,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   delete [] col_type; )
        BUGCRIT (res->col_tab[1] == COLRES_NUM_NOEUDS,
                 false,
                 (gettext ("La réaction d'appui %s ne peut être affichée que si la première colonne affiche les numéros des noeuds.\n"),
                           "Fy");
                   delete [] col_type; )
        col_type[i - 1] = G_TYPE_STRING;
        str_tmp.assign (gettext ("F<sub>y</sub> [N]"));
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (&str_tmp,
                                   col_type[i - 1],
                                   i - 1U,
                                   xalign,
                                   0));
        break;
      }
      case COLRES_REACTION_APPUI_FZ :
      {
        BUGCRIT (i != 1,
                 false,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   delete [] col_type; )
        BUGCRIT (res->col_tab[1] == COLRES_NUM_NOEUDS,
                 false,
                 (gettext ("La réaction d'appui %s ne peut être affichée que si la première colonne affiche les numéros des noeuds.\n"),
                           "Fz");
                   delete [] col_type; )
        col_type[i - 1] = G_TYPE_STRING;
        str_tmp.assign (gettext ("F<sub>z</sub> [N]"));
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (&str_tmp,
                                   col_type[i - 1],
                                   i - 1U,
                                   xalign,
                                   0));
        break;
      }
      case COLRES_REACTION_APPUI_MX :
      {
        BUGCRIT (i != 1,
                 false,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   delete [] col_type; )
        BUGCRIT (res->col_tab[1] == COLRES_NUM_NOEUDS,
                 false,
                 (gettext ("La réaction d'appui %s ne peut être affichée que si la première colonne affiche les numéros des noeuds.\n"),
                           "Mx");
                   delete [] col_type; )
        col_type[i - 1] = G_TYPE_STRING;
        str_tmp.assign (gettext ("M<sub>x</sub> [N.m]"));
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (&str_tmp,
                                   col_type[i - 1],
                                   i - 1U,
                                   xalign,
                                   0));
        break;
      }
      case COLRES_REACTION_APPUI_MY :
      {
        BUGCRIT (i != 1,
                 false,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   delete [] col_type; )
        BUGCRIT (res->col_tab[1] == COLRES_NUM_NOEUDS,
                 false,
                 (gettext ("La réaction d'appui %s ne peut être affichée que si la première colonne affiche les numéros des noeuds.\n"),
                           "My");
                   delete [] col_type; )
        col_type[i - 1] = G_TYPE_STRING;
        str_tmp.assign (gettext ("M<sub>y</sub> [N.m]"));
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (&str_tmp,
                                   col_type[i - 1],
                                   i - 1U,
                                   xalign,
                                   0));
        break;
      }
      case COLRES_REACTION_APPUI_MZ :
      {
        BUGCRIT (i != 1,
                 false,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   delete [] col_type; )
        BUGCRIT (res->col_tab[1] == COLRES_NUM_NOEUDS,
                 false,
                 (gettext ("La réaction d'appui %s ne peut être affichée que si la première colonne affiche les numéros des noeuds.\n"),
                           "Mz");
                   delete [] col_type; )
        col_type[i - 1] = G_TYPE_STRING;
        str_tmp.assign (gettext ("M<sub>z</sub> [N.m]"));
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (&str_tmp,
                                   col_type[i - 1],
                                   i - 1U,
                                   xalign,
                                   0));
        break;
      }
      case COLRES_DEPLACEMENT_UX :
      {
        BUGCRIT (i != 1,
                 false,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   delete [] col_type; )
        BUGCRIT (res->col_tab[1] == COLRES_NUM_NOEUDS,
                 false,
                 (gettext ("Le déplacement des noeuds %s ne peut être affichée que si la première colonne affiche les numéros des noeuds.\n"),
                           "Ux");
                   delete [] col_type; )
        col_type[i - 1] = G_TYPE_STRING;
        str_tmp.assign (gettext ("u<sub>x</sub> [m]"));
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (&str_tmp,
                                   col_type[i - 1],
                                   i - 1U,
                                   xalign,
                                   0));
        break;
      }
      case COLRES_DEPLACEMENT_UY :
      {
        BUGCRIT (i != 1,
                 false,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   delete [] col_type; )
        BUGCRIT (res->col_tab[1] == COLRES_NUM_NOEUDS,
                 false,
                 (gettext ("Le déplacement des noeuds %s ne peut être affichée que si la première colonne affiche les numéros des noeuds.\n"),
                           "Uy");
                   delete [] col_type; )
        col_type[i - 1] = G_TYPE_STRING;
        str_tmp.assign (gettext ("u<sub>y</sub> [m]"));
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (&str_tmp,
                                   col_type[i - 1],
                                   i - 1U,
                                   xalign,
                                   0));
        break;
      }
      case COLRES_DEPLACEMENT_UZ :
      {
        BUGCRIT (i != 1,
                 false,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   delete [] col_type; )
        BUGCRIT (res->col_tab[1] == COLRES_NUM_NOEUDS,
                 false,
                 (gettext ("Le déplacement des noeuds %s ne peut être affichée que si la première colonne affiche les numéros des noeuds.\n"),
                           "Uz");
                   delete [] col_type; )
        col_type[i - 1] = G_TYPE_STRING;
        str_tmp.assign (gettext ("u<sub>z</sub> [m]"));
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (&str_tmp,
                                   col_type[i - 1],
                                   i - 1U,
                                   xalign,
                                   0));
        break;
      }
      case COLRES_DEPLACEMENT_RX :
      {
        BUGCRIT (i != 1,
                 false,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   delete [] col_type; )
        BUGCRIT (res->col_tab[1] == COLRES_NUM_NOEUDS,
                 false,
                 (gettext ("Le déplacement des noeuds %s ne peut être affichée que si la première colonne affiche les numéros des noeuds.\n"),
                           "Rx");
                   delete [] col_type; )
        col_type[i - 1] = G_TYPE_STRING;
        str_tmp.assign (gettext ("r<sub>x</sub> [rad]"));
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (&str_tmp,
                                   col_type[i - 1],
                                   i - 1U,
                                   xalign,
                                   0));
        break;
      }
      case COLRES_DEPLACEMENT_RY :
      {
        BUGCRIT (i != 1,
                 false,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   delete [] col_type; )
        BUGCRIT (res->col_tab[1] == COLRES_NUM_NOEUDS,
                 false,
                 (gettext ("Le déplacement des noeuds %s ne peut être affichée que si la première colonne affiche les numéros des noeuds.\n"),
                           "Ry");
                   delete [] col_type; )
        col_type[i - 1] = G_TYPE_STRING;
        str_tmp.assign (gettext ("r<sub>y</sub> [rad]"));
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (&str_tmp,
                                   col_type[i - 1],
                                   i - 1U,
                                   xalign,
                                   0));
        break;
      }
      case COLRES_DEPLACEMENT_RZ :
      {
        BUGCRIT (i != 1,
                 false,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   delete [] col_type; )
        BUGCRIT (res->col_tab[1] == COLRES_NUM_NOEUDS,
                 false,
                 (gettext ("Le déplacement des noeuds %s ne peut être affichée que si la première colonne affiche les numéros des noeuds.\n"),
                           "Rx");
                   delete [] col_type; )
        col_type[i - 1] = G_TYPE_STRING;
        str_tmp.assign (gettext ("r<sub>z</sub> [rad]"));
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (&str_tmp,
                                   col_type[i - 1],
                                   i - 1U,
                                   xalign,
                                   0));
        break;
      }
      case COLRES_BARRES_LONGUEUR :
      {
        BUGCRIT (i != 1,
                 false,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   delete [] col_type; )
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 false,
                 (gettext ("La longueur des barres ne peut être affichée que si la première colonne affiche les numéros des barres.\n"));
                   delete [] col_type; )
        col_type[i - 1] = G_TYPE_DOUBLE;
        str_tmp.assign (gettext ("L [m]"));
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (&str_tmp,
                                   col_type[i - 1],
                                   i - 1U,
                                   1.,
                                   DECIMAL_DISTANCE));
        break;
      }
      case COLRES_BARRES_PIXBUF_N :
      {
        BUGCRIT (i != 1,
                 false,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   delete [] col_type; )
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 false,
                 (gettext ("Les efforts dans les barres ne peuvent être affichés que si la première colonne affiche les numéros des barres.\n"));
                   delete [] col_type; )
        col_type[i - 1] = G_TYPE_OBJECT;
        str_tmp.assign (gettext ("Effort normal"));
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (&str_tmp,
                                   col_type[i - 1],
                                   i - 1U,
                                   0.5,
                                   0));
        break;
      }
      case COLRES_BARRES_PIXBUF_TY :
      {
        BUGCRIT (i != 1,
                 false,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   delete [] col_type; )
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 false,
                 (gettext ("Les efforts dans les barres ne peuvent être affichés que si la première colonne affiche les numéros des barres.\n"));
                   delete [] col_type; )
        col_type[i - 1] = G_TYPE_OBJECT;
        str_tmp.assign (gettext ("Effort tranchant selon Y"));
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (&str_tmp,
                                   col_type[i - 1],
                                   i - 1U,
                                   0.5,
                                   0));
        break;
      }
      case COLRES_BARRES_PIXBUF_TZ :
      {
        BUGCRIT (i != 1,
                 false,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   delete [] col_type; )
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 false,
                 (gettext ("Les efforts dans les barres ne peuvent être affichés que si la première colonne affiche les numéros des barres.\n"));
                   delete [] col_type; )
        col_type[i - 1] = G_TYPE_OBJECT;
        str_tmp.assign (gettext ("Effort tranchant selon Z"));
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (&str_tmp,
                                   col_type[i - 1],
                                   i - 1U,
                                   0.5,
                                   0));
        break;
      }
      case COLRES_BARRES_PIXBUF_MX :
      {
        BUGCRIT (i != 1,
                 false,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   delete [] col_type; )
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 false,
                 (gettext ("Les efforts dans les barres ne peuvent être affichés que si la première colonne affiche les numéros des barres.\n"));
                   delete [] col_type; )
        col_type[i - 1] = G_TYPE_OBJECT;
        str_tmp.assign (gettext ("Moment de torsion"));
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (&str_tmp,
                                   col_type[i - 1],
                                   i - 1U,
                                   0.5,
                                   0));
        break;
      }
      case COLRES_BARRES_PIXBUF_MY :
      {
        BUGCRIT (i != 1,
                 false,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   delete [] col_type; )
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 false,
                 (gettext ("Les efforts dans les barres ne peuvent être affichés que si la première colonne affiche les numéros des barres.\n"));
                   delete [] col_type; )
        col_type[i - 1] = G_TYPE_OBJECT;
        str_tmp.assign (gettext ("Moment fléchissant selon y"));
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (&str_tmp,
                                   col_type[i - 1],
                                   i - 1U,
                                   0.5,
                                   0));
        break;
      }
      case COLRES_BARRES_PIXBUF_MZ :
      {
        BUGCRIT (i != 1,
                 false,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   delete [] col_type; )
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 false,
                 (gettext ("Les efforts dans les barres ne peuvent être affichés que si la première colonne affiche les numéros des barres.\n"));
                   delete [] col_type; )
        col_type[i - 1] = G_TYPE_OBJECT;
        str_tmp.assign (gettext ("Moment fléchissant selon z"));
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (&str_tmp,
                                   col_type[i - 1],
                                   i - 1U,
                                   0.5,
                                   0));
        break;
      }
      case COLRES_BARRES_DESC_N :
      {
        BUGCRIT (i != 1,
                 false,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   delete [] col_type; )
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 false,
                 (gettext ("Les efforts dans les barres ne peuvent être affichés que si la première colonne affiche les numéros des barres.\n"));
                   delete [] col_type; )
        col_type[i - 1] = G_TYPE_STRING;
        str_tmp.assign (gettext ("Points caractéristiques de N [N]"));
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (&str_tmp,
                                   col_type[i - 1],
                                   i - 1U,
                                   0.,
                                   0));
        break;
      }
      case COLRES_BARRES_DESC_TY :
      {
        BUGCRIT (i != 1,
                 false,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   delete [] col_type; )
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 false,
                 (gettext ("Les efforts dans les barres ne peuvent être affichés que si la première colonne affiche les numéros des barres.\n"));
                   delete [] col_type; )
        col_type[i - 1] = G_TYPE_STRING;
        str_tmp.assign (gettext ("Points caractéristiques de T<sub>y</sub> [N]"));
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (&str_tmp,
                                   col_type[i - 1],
                                   i - 1U,
                                   0.,
                                   0));
        break;
      }
      case COLRES_BARRES_DESC_TZ :
      {
        BUGCRIT (i != 1,
                 false,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   delete [] col_type; )
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 false,
                 (gettext ("Les efforts dans les barres ne peuvent être affichés que si la première colonne affiche les numéros des barres.\n"));
                   delete [] col_type; )
        col_type[i - 1] = G_TYPE_STRING;
        str_tmp.assign (gettext ("Points caractéristiques de T<sub>z</sub> [N]"));
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (&str_tmp,
                                   col_type[i - 1],
                                   i - 1U,
                                   0.,
                                   0));
        break;
      }
      case COLRES_BARRES_DESC_MX :
      {
        BUGCRIT (i != 1,
                 false,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   delete [] col_type; )
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 false,
                 (gettext ("Les efforts dans les barres ne peuvent être affichés que si la première colonne affiche les numéros des barres.\n"));
                   delete [] col_type; )
        col_type[i - 1] = G_TYPE_STRING;
        str_tmp.assign (gettext ("Points caractéristiques de M<sub>x</sub> [N.m]"));
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (&str_tmp,
                                   col_type[i - 1],
                                   i - 1U,
                                   0.,
                                   0));
        break;
      }
      case COLRES_BARRES_DESC_MY :
      {
        BUGCRIT (i != 1,
                 false,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   delete [] col_type; )
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 false,
                 (gettext ("Les efforts dans les barres ne peuvent être affichés que si la première colonne affiche les numéros des barres.\n"));
                   delete [] col_type; )
        col_type[i - 1] = G_TYPE_STRING;
        str_tmp.assign (gettext ("Points caractéristiques de M<sub>y</sub> [N.m]"));
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (&str_tmp,
                                   col_type[i - 1],
                                   i - 1U,
                                   0.,
                                   0));
        break;
      }
      case COLRES_BARRES_DESC_MZ :
      {
        BUGCRIT (i != 1,
                 false,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   delete [] col_type; )
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 false,
                 (gettext ("Les efforts dans les barres ne peuvent être affichés que si la première colonne affiche les numéros des barres.\n"));
                   delete [] col_type; )
        col_type[i - 1] = G_TYPE_STRING;
        str_tmp.assign (gettext ("Points caractéristiques de M<sub>z</sub> [N.m]"));
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (&str_tmp,
                                   col_type[i - 1],
                                   i - 1U,
                                   0.,
                                   0));
        break;
      }
      case COLRES_BARRES_EQ_N :
      {
        BUGCRIT (i != 1,
                 false,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   delete [] col_type; )
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 false,
                 (gettext ("Les efforts dans les barres ne peuvent être affichés que si la première colonne affiche les numéros des barres.\n"));
                   delete [] col_type; )
        col_type[i - 1] = G_TYPE_STRING;
        str_tmp.assign (gettext ("N [N]"));
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (&str_tmp,
                                   col_type[i - 1],
                                   i - 1U,
                                   0.,
                                   0));
        break;
      }
      case COLRES_BARRES_EQ_TY :
      {
        BUGCRIT (i != 1,
                 false,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   delete [] col_type; )
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 false,
                 (gettext ("Les efforts dans les barres ne peuvent être affichés que si la première colonne affiche les numéros des barres.\n"));
                   delete [] col_type; )
        col_type[i - 1] = G_TYPE_STRING;
        str_tmp.assign (gettext ("T<sub>y</sub> [N]"));
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (&str_tmp,
                                   col_type[i - 1],
                                   i - 1U,
                                   0.,
                                   0));
        break;
      }
      case COLRES_BARRES_EQ_TZ :
      {
        BUGCRIT (i != 1,
                 false,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   delete [] col_type; )
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 false,
                 (gettext ("Les efforts dans les barres ne peuvent être affichés que si la première colonne affiche les numéros des barres.\n"));
                   delete [] col_type; )
        col_type[i - 1] = G_TYPE_STRING;
        str_tmp.assign (gettext ("T<sub>z</sub> [N]"));
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (&str_tmp,
                                   col_type[i - 1],
                                   i - 1U,
                                   0.,
                                   0));
        break;
      }
      case COLRES_BARRES_EQ_MX :
      {
        BUGCRIT (i != 1,
                 false,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   delete [] col_type; )
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 false,
                 (gettext ("Les efforts dans les barres ne peuvent être affichés que si la première colonne affiche les numéros des barres.\n"));
                   delete [] col_type; )
        col_type[i - 1] = G_TYPE_STRING;
        str_tmp.assign (gettext ("M<sub>x</sub> [N.m]"));
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (&str_tmp,
                                   col_type[i - 1],
                                   i - 1U,
                                   0.,
                                   0));
        break;
      }
      case COLRES_BARRES_EQ_MY :
      {
        BUGCRIT (i != 1,
                 false,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   delete [] col_type; )
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 false,
                 (gettext ("Les efforts dans les barres ne peuvent être affichés que si la première colonne affiche les numéros des barres.\n"));
                   delete [] col_type; )
        col_type[i - 1] = G_TYPE_STRING;
        str_tmp.assign (gettext ("M<sub>y</sub> [N.m]"));
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (&str_tmp,
                                   col_type[i - 1],
                                   i - 1U,
                                   0.,
                                   0));
        break;
      }
      case COLRES_BARRES_EQ_MZ :
      {
        BUGCRIT (i != 1,
                 false,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   delete [] col_type; )
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 false,
                 (gettext ("Les efforts dans les barres ne peuvent être affichés que si la première colonne affiche les numéros des barres.\n"));
                   delete [] col_type; )
        col_type[i - 1] = G_TYPE_STRING;
        str_tmp.assign (gettext ("M<sub>z</sub> [N.m]"));
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (&str_tmp,
                                   col_type[i - 1],
                                   i - 1U,
                                   0.,
                                   0));
        break;
      }
      case COLRES_DEF_PIXBUF_UX :
      {
        BUGCRIT (i != 1,
                 false,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   delete [] col_type; )
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 false,
                 (gettext ("Les efforts dans les barres ne peuvent être affichés que si la première colonne affiche les numéros des barres.\n"));
                   delete [] col_type; )
        col_type[i - 1] = G_TYPE_OBJECT;
        str_tmp.assign (gettext ("Déformation de la barre selon x"));
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (&str_tmp,
                                   col_type[i - 1],
                                   i - 1U,
                                   0.5,
                                   0));
        break;
      }
      case COLRES_DEF_PIXBUF_UY :
      {
        BUGCRIT (i != 1,
                 false,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   delete [] col_type; )
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 false,
                 (gettext ("Les efforts dans les barres ne peuvent être affichés que si la première colonne affiche les numéros des barres.\n"));
                   delete [] col_type; )
        col_type[i - 1] = G_TYPE_OBJECT;
        str_tmp.assign (gettext ("Déformation de la barre selon y"));
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (&str_tmp,
                                   col_type[i - 1],
                                   i - 1U,
                                   0.5,
                                   0));
        break;
      }
      case COLRES_DEF_PIXBUF_UZ :
      {
        BUGCRIT (i != 1,
                 false,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   delete [] col_type; )
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 false,
                 (gettext ("Les efforts dans les barres ne peuvent être affichés que si la première colonne affiche les numéros des barres.\n"));
                   delete [] col_type; )
        col_type[i - 1] = G_TYPE_OBJECT;
        str_tmp.assign (gettext ("Déformation de la barre selon z"));
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (&str_tmp,
                                   col_type[i - 1],
                                   i - 1U,
                                   0.5,
                                   0));
        break;
      }
      case COLRES_DEF_PIXBUF_RX :
      {
        BUGCRIT (i != 1,
                 false,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   delete [] col_type; )
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 false,
                 (gettext ("Les efforts dans les barres ne peuvent être affichés que si la première colonne affiche les numéros des barres.\n"));
                   delete [] col_type; )
        col_type[i - 1] = G_TYPE_OBJECT;
        str_tmp.assign (gettext ("Rotation de la barre selon x"));
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (&str_tmp,
                                   col_type[i - 1],
                                   i - 1U,
                                   0.5,
                                   0));
        break;
      }
      case COLRES_DEF_PIXBUF_RY :
      {
        BUGCRIT (i != 1,
                 false,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   delete [] col_type; )
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 false,
                 (gettext ("Les efforts dans les barres ne peuvent être affichés que si la première colonne affiche les numéros des barres.\n"));
                   delete [] col_type; )
        col_type[i - 1] = G_TYPE_OBJECT;
        str_tmp.assign (gettext ("Rotation de la barre selon y"));
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (&str_tmp,
                                   col_type[i - 1],
                                   i - 1U,
                                   0.5,
                                   0));
        break;
      }
      case COLRES_DEF_PIXBUF_RZ :
      {
        BUGCRIT (i != 1,
                 false,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   delete [] col_type; )
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 false,
                 (gettext ("Les efforts dans les barres ne peuvent être affichés que si la première colonne affiche les numéros des barres.\n"));
                   delete [] col_type; )
        col_type[i - 1] = G_TYPE_OBJECT;
        str_tmp.assign (gettext ("Rotation de la barre selon z"));
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (&str_tmp,
                                   col_type[i - 1],
                                   i - 1U,
                                   0.5,
                                   0));
        break;
      }
      case COLRES_DEF_DESC_UX :
      {
        BUGCRIT (i != 1,
                 false,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   delete [] col_type; )
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 false,
                 (gettext ("Les déformations dans les barres ne peuvent être affichées que si la première colonne affiche les numéros des barres.\n"));
                   delete [] col_type; )
        col_type[i - 1] = G_TYPE_STRING;
        str_tmp.assign (gettext ("Points caractéristiques de u<sub>x</sub> [m]"));
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (&str_tmp,
                                   col_type[i - 1],
                                   i - 1U,
                                   0.,
                                   0));
        break;
      }
      case COLRES_DEF_DESC_UY :
      {
        BUGCRIT (i != 1,
                 false,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   delete [] col_type; )
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 false,
                 (gettext ("Les déformations dans les barres ne peuvent être affichées que si la première colonne affiche les numéros des barres.\n"));
                   delete [] col_type; )
        col_type[i - 1] = G_TYPE_STRING;
        str_tmp.assign (gettext ("Points caractéristiques de u<sub>y</sub> [m]"));
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (&str_tmp,
                                   col_type[i - 1],
                                   i - 1U,
                                   0.,
                                   0));
        break;
      }
      case COLRES_DEF_DESC_UZ :
      {
        BUGCRIT (i != 1,
                 false,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   delete [] col_type; )
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 false,
                 (gettext ("Les déformations dans les barres ne peuvent être affichées que si la première colonne affiche les numéros des barres.\n"));
                   delete [] col_type; )
        col_type[i - 1] = G_TYPE_STRING;
        str_tmp.assign (gettext ("Points caractéristiques de u<sub>z</sub> [m]"));
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (&str_tmp,
                                   col_type[i - 1],
                                   i - 1U,
                                   0.,
                                   0));
        break;
      }
      case COLRES_DEF_DESC_RX :
      {
        BUGCRIT (i != 1,
                 false,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   delete [] col_type; )
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 false,
                 (gettext ("Les déformations dans les barres ne peuvent être affichées que si la première colonne affiche les numéros des barres.\n"));
                   delete [] col_type; )
        col_type[i - 1] = G_TYPE_STRING;
        str_tmp.assign (gettext ("Points caractéristiques de r<sub>x</sub> [rad]"));
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (&str_tmp,
                                   col_type[i - 1],
                                   i - 1U,
                                   0.,
                                   0));
        break;
      }
      case COLRES_DEF_DESC_RY :
      {
        BUGCRIT (i != 1,
                 false,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   delete [] col_type; )
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 false,
                 (gettext ("Les déformations dans les barres ne peuvent être affichées que si la première colonne affiche les numéros des barres.\n"));
                   delete [] col_type; )
        col_type[i - 1] = G_TYPE_STRING;
        str_tmp.assign (gettext ("Points caractéristiques de r<sub>y</sub> [rad]"));
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (&str_tmp,
                                   col_type[i - 1],
                                   i - 1U,
                                   0.,
                                   0));
        break;
      }
      case COLRES_DEF_DESC_RZ :
      {
        BUGCRIT (i != 1,
                 false,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   delete [] col_type; )
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 false,
                 (gettext ("Les déformations dans les barres ne peuvent être affichées que si la première colonne affiche les numéros des barres.\n"));
                   delete [] col_type; )
        col_type[i - 1] = G_TYPE_STRING;
        str_tmp.assign (gettext ("Points caractéristiques de r<sub>z</sub> [rad]"));
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (&str_tmp,
                                   col_type[i - 1],
                                   i - 1U,
                                   0.,
                                   0));
        break;
      }
      case COLRES_DEF_UX :
      {
        BUGCRIT (i != 1,
                 false,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   delete [] col_type; )
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 false,
                 (gettext ("Les déformations dans les barres ne peuvent être affichées que si la première colonne affiche les numéros des barres.\n"));
                   delete [] col_type; )
        col_type[i - 1] = G_TYPE_STRING;
        str_tmp.assign (gettext ("u<sub>x</sub> [m]"));
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (&str_tmp,
                                   col_type[i - 1],
                                   i - 1U,
                                   0.,
                                   0));
        break;
      }
      case COLRES_DEF_UY :
      {
        BUGCRIT (i != 1,
                 false,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   delete [] col_type; )
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 false,
                 (gettext ("Les déformations dans les barres ne peuvent être affichées que si la première colonne affiche les numéros des barres.\n"));
                   delete [] col_type; )
        col_type[i - 1] = G_TYPE_STRING;
        str_tmp.assign (gettext ("u<sub>y</sub> [m]"));
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (&str_tmp,
                                   col_type[i - 1],
                                   i - 1U,
                                   0.,
                                   0));
        break;
      }
      case COLRES_DEF_UZ :
      {
        BUGCRIT (i != 1,
                 false,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   delete [] col_type; )
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 false,
                 (gettext ("Les déformations dans les barres ne peuvent être affichées que si la première colonne affiche les numéros des barres.\n"));
                   delete [] col_type; )
        col_type[i - 1] = G_TYPE_STRING;
        str_tmp.assign (gettext ("u<sub>z</sub> [m]"));
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (&str_tmp,
                                   col_type[i - 1],
                                   i - 1U,
                                   0.,
                                   0));
        break;
      }
      case COLRES_DEF_RX :
      {
        BUGCRIT (i != 1,
                 false,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   delete [] col_type; )
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 false,
                 (gettext ("Les déformations dans les barres ne peuvent être affichées que si la première colonne affiche les numéros des barres.\n"));
                   delete [] col_type; )
        col_type[i - 1] = G_TYPE_STRING;
        str_tmp.assign (gettext ("r<sub>x</sub> [rad]"));
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (&str_tmp,
                                   col_type[i - 1],
                                   i - 1U,
                                   0.,
                                   0));
        break;
      }
      case COLRES_DEF_RY :
      {
        BUGCRIT (i != 1,
                 false,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   delete [] col_type; )
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 false,
                 (gettext ("Les déformations dans les barres ne peuvent être affichées que si la première colonne affiche les numéros des barres.\n"));
                   delete [] col_type; )
        col_type[i - 1] = G_TYPE_STRING;
        str_tmp.assign (gettext ("r<sub>y</sub> [rad]"));
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (&str_tmp,
                                   col_type[i - 1],
                                   i - 1U,
                                   0.,
                                   0));
        break;
      }
      case COLRES_DEF_RZ :
      {
        BUGCRIT (i != 1,
                 false,
                 (gettext ("La première colonne est réservée à la liste des noeuds et des barres.\n"));
                   delete [] col_type; )
        BUGCRIT (res->col_tab[1] == COLRES_NUM_BARRES,
                 false,
                 (gettext ("Les déformations dans les barres ne peuvent être affichées que si la première colonne affiche les numéros des barres.\n"));
                   delete [] col_type; )
        col_type[i - 1] = G_TYPE_STRING;
        str_tmp.assign (gettext ("r<sub>z</sub> [rad]"));
        gtk_tree_view_append_column (
          res->treeview,
          common_gtk_cree_colonne (&str_tmp,
                                   col_type[i - 1],
                                   i - 1U,
                                   0.,
                                   0));
        break;
      }
      default :
      {
        FAILCRIT (false,
                  (gettext ("La colonne des résultats %d est inconnue.\n"),
                            res->col_tab[i]);
                    delete [] col_type; )
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
  delete [] col_type;
  gtk_tree_view_set_model (res->treeview, GTK_TREE_MODEL (res->list_store));

  BUG (EF_gtk_resultats_remplit_page (res, p), false)
  
  gtk_widget_show_all (p_scrolled_window);
  
  gtk_notebook_set_current_page (
    UI_RES.notebook,
    gtk_notebook_get_n_pages (UI_RES.notebook) - 2);
  
  return true;
}


extern "C"
void
EF_gtk_resultats_cas_change (GtkWidget *widget,
                             Projet    *p)
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
  std::list <Gtk_EF_Resultats_Tableau *>::iterator it;
  uint8_t indice_combo;
  
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_RES.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Résultats"); )
  
  indice_combo = (uint8_t) gtk_combo_box_get_active (UI_RES.combobox);
  
  // Pour forcer le combobox ponderation à être vierge.
  if (gtk_combo_box_get_model (UI_RES.combobox_ponderations) != NULL)
  {
    gtk_combo_box_set_active (UI_RES.combobox_ponderations, -1);
    gtk_combo_box_set_model (UI_RES.combobox_ponderations, NULL);
  }
  
  it = UI_RES.tableaux.begin ();
  
  while (it != UI_RES.tableaux.end ())
  {
    BUG (EF_gtk_resultats_remplit_page (*it, p), )
    
    ++it;
  }
  
  if (indice_combo == 1)
  {
    GtkListStore *list_pond;
    GtkTreeIter   Iter;
    uint16_t      i;
    
    std::list <std::list <Ponderation *> *> *comb;
    std::list <std::list <Ponderation *> *>::iterator it2;
    
    switch (gtk_combo_box_get_active (UI_RES.combobox_cas))
    {
      case -1 :
        return;
      case 0 :
      {
        comb = &p->ponderations.elu_equ;
        break;
      }
      case 1 :
      {
        comb = &p->ponderations.elu_str;
        break;
      }
      case 2 :
      {
        comb = &p->ponderations.elu_geo;
        break;
      }
      case 3 :
      {
        comb = &p->ponderations.elu_fat;
        break;
      }
      case 4 :
      {
        comb = &p->ponderations.elu_acc;
        break;
      }
      case 5 :
      {
        comb = &p->ponderations.elu_sis;
        break;
      }
      case 6 :
      {
        comb = &p->ponderations.els_car;
        break;
      }
      case 7 :
      {
        comb = &p->ponderations.els_freq;
        break;
      }
      case 8 :
      {
        comb = &p->ponderations.els_perm;
        break;
      }
      default :
      {
        FAILCRIT ( ,
                  (gettext ("Paramètre %s incorrect.\n"), "combobox_cas"); )
        break;
      }
    }
    
    list_pond = gtk_list_store_new (1, G_TYPE_STRING);
    
    it2 = comb->begin ();
    i = 0;
    while (it2 != comb->end ())
    {
      std::string tmp2;
      
      gtk_list_store_append (list_pond, &Iter);
      tmp2 = std::to_string (i) + " : " +
                                         _1990_ponderations_description (*it2);
      gtk_list_store_set (list_pond, &Iter, 0, tmp2.c_str (), -1);
      
      i++;
      ++it2;
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
extern "C"
void
EF_gtk_resultats_ponderations_change (GtkWidget *widget,
                                      Projet    *p)
{
  std::list<Gtk_EF_Resultats_Tableau*>::iterator it;
  
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_RES.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Résultats"); )
  
  it = UI_RES.tableaux.begin ();
  
  while (it != UI_RES.tableaux.end ())
  {
    BUG (EF_gtk_resultats_remplit_page (*it, p), )
    
    ++it;
  }
  
  return;
}


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
extern "C"
void
EF_gtk_resultats_combobox_changed (GtkComboBox *combobox,
                                   Projet      *p)
{
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_RES.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Résultats"); )
  
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
    g_object_ref (p->ponderations.list_el_desc);
    gtk_combo_box_set_model (UI_RES.combobox_cas,
                             GTK_TREE_MODEL (p->ponderations.list_el_desc));
    gtk_widget_set_visible (GTK_WIDGET (UI_RES.combobox_ponderations), TRUE);
    gtk_widget_set_hexpand (GTK_WIDGET (UI_RES.combobox_cas), FALSE);
    EF_gtk_resultats_cas_change (NULL, p);
  }
  
  return;
}


/**
 * \brief Ajoute une page au Notebook sur la base de divers tableaux "de base".
 * \param menuitem : le composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
extern "C"
void
EF_gtk_resultats_add_page_type (GtkMenuItem *menuitem,
                                Projet      *p)
{
  Gtk_EF_Resultats_Tableau *res;
  
  BUGPARAMCRIT (p, "%p", p, )
  BUGPARAMCRIT (menuitem, "%p", menuitem, )
  BUGCRIT (UI_RES.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Résultats"); )
  res = new Gtk_EF_Resultats_Tableau;
  
  if (strcmp (gtk_menu_item_get_label (menuitem), gettext ("Noeuds")) == 0)
  {
    res->col_tab = new Colonne_Resultats[5];
    res->col_tab[0] = (Colonne_Resultats) 4;
    res->col_tab[1] = COLRES_NUM_NOEUDS;
    res->col_tab[2] = COLRES_NOEUDS_X;
    res->col_tab[3] = COLRES_NOEUDS_Y;
    res->col_tab[4] = COLRES_NOEUDS_Z;
    
    res->filtre = FILTRE_AUCUN;
    
    res->nom = gtk_menu_item_get_label (menuitem);
    
    BUG (EF_gtk_resultats_add_page (res, p), )
    
    UI_RES.tableaux.push_back (res);
  }
  else if (strcmp (gtk_menu_item_get_label (menuitem),
                   gettext("Réactions d'appuis (repère global)")) == 0)
  {
    res->col_tab = new Colonne_Resultats[8];
    res->col_tab[0] = (Colonne_Resultats) 7;
    res->col_tab[1] = COLRES_NUM_NOEUDS;
    res->col_tab[2] = COLRES_REACTION_APPUI_FX;
    res->col_tab[3] = COLRES_REACTION_APPUI_FY;
    res->col_tab[4] = COLRES_REACTION_APPUI_FZ;
    res->col_tab[5] = COLRES_REACTION_APPUI_MX;
    res->col_tab[6] = COLRES_REACTION_APPUI_MY;
    res->col_tab[7] = COLRES_REACTION_APPUI_MZ;
    
    res->filtre = FILTRE_NOEUD_APPUI;
    
    res->nom = gtk_menu_item_get_label (menuitem);
    
    BUG (EF_gtk_resultats_add_page (res, p), )
    
    UI_RES.tableaux.push_back (res);
  }
  else if (strcmp (gtk_menu_item_get_label (menuitem),
                   gettext ("Déplacements (repère global)")) == 0)
  {
    res->col_tab = new Colonne_Resultats[8];
    res->col_tab[0] = (Colonne_Resultats) 7;
    res->col_tab[1] = COLRES_NUM_NOEUDS;
    res->col_tab[2] = COLRES_DEPLACEMENT_UX;
    res->col_tab[3] = COLRES_DEPLACEMENT_UY;
    res->col_tab[4] = COLRES_DEPLACEMENT_UZ;
    res->col_tab[5] = COLRES_DEPLACEMENT_RX;
    res->col_tab[6] = COLRES_DEPLACEMENT_RY;
    res->col_tab[7] = COLRES_DEPLACEMENT_RZ;
    
    res->filtre = FILTRE_AUCUN;
    
    res->nom = gtk_menu_item_get_label (menuitem);
    
    BUG (EF_gtk_resultats_add_page (res, p), )
    
    UI_RES.tableaux.push_back (res);
  }
  else if (strcmp (gtk_menu_item_get_label (menuitem),
                   gettext ("Barres")) == 0)
  {
    res->col_tab = new Colonne_Resultats[3];
    res->col_tab[0] = (Colonne_Resultats) 2;
    res->col_tab[1] = COLRES_NUM_BARRES;
    res->col_tab[2] = COLRES_BARRES_LONGUEUR;
    
    res->filtre = FILTRE_AUCUN;
    
    res->nom = gtk_menu_item_get_label (menuitem);
    
    BUG (EF_gtk_resultats_add_page (res, p), )
    
    UI_RES.tableaux.push_back (res);
  }
  else if (strcmp (gtk_menu_item_get_label (menuitem),
                   gettext ("Efforts dans les barres (repère local)")) == 0)
  {
    res->col_tab = new Colonne_Resultats[21];
    res->col_tab[0] = (Colonne_Resultats) 20;
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
    
    res->nom = gtk_menu_item_get_label (menuitem);
    
    BUG (EF_gtk_resultats_add_page (res, p), )
    
    UI_RES.tableaux.push_back (res);
  }
  else if (strcmp (gtk_menu_item_get_label (menuitem),
                   gettext ("Déformations des barres (repère local)")) == 0)
  {
    res->col_tab = new Colonne_Resultats[21];
    res->col_tab[0] = (Colonne_Resultats) 20;
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
    
    res->nom = gtk_menu_item_get_label (menuitem);
    
    BUG (EF_gtk_resultats_add_page (res, p), )
    
    UI_RES.tableaux.push_back (res);
  }
  else
  {
    FAILCRIT ( ,
              (gettext ("La page est inconnue.\n"));
                delete res; )
  }
  
  return;
}


/**
 * \brief Création de la fenêtre permettant d'afficher les résultats sous forme
 *        d'un tableau.
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique impossible à générer.
 */
void
EF_gtk_resultats (Projet *p)
{
  std::list <Gtk_EF_Resultats_Tableau *>::iterator it;
  
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
                     "Résultats"); )
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
  
  it = UI_RES.tableaux.begin ();
  
  while (it != UI_RES.tableaux.end ())
  {
    BUG (EF_gtk_resultats_add_page (*it, p), )
    
    ++it;
  }
  
  gtk_window_set_transient_for (GTK_WINDOW (UI_RES.window),
                                GTK_WINDOW (UI_GTK.window));
  
  return;
}


/**
 * \brief Libère l'ensemble des éléments utilisés pour l'affichage des
 *        résultats.
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 */
void
EF_gtk_resultats_free (Projet *p)
{
  std::list <Gtk_EF_Resultats_Tableau *>::iterator it;
  
  it = UI_RES.tableaux.begin ();
  
  while (it != UI_RES.tableaux.end ())
  {
    Gtk_EF_Resultats_Tableau *res = *it;
    
    g_object_unref (res->list_store);
    free (res->col_tab);
    delete res;
    
    ++it;
  }
  UI_RES.tableaux.clear ();
  
  return;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
