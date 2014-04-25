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

#include "1990_action.h"
#include "common_projet.h"
#include "common_erreurs.h"
#include "common_selection.h"
#include "common_math.h"
#ifdef ENABLE_GTK
#include "common_gtk.h"
#include "EF_gtk_charge_noeud.h"
#endif
#include "EF_calculs.h"
#include "EF_charge.h"


/**
 * \brief Ajoute une charge ponctuelle à une action et à un noeud de la
 *        structure.
 * \param p : la variable projet,
 * \param action : l'action qui contiendra la charge,
 * \param noeuds : liste des noeuds qui supportera la charge,
 * \param fx : force suivant l'axe global x en N,
 * \param fy : force suivant l'axe global y en N,
 * \param fz : force suivant l'axe global z en N,
 * \param mx : moment autour de l'axe global x en N.m,
 * \param my : moment autour de l'axe global y en N.m,
 * \param mz : moment autour de l'axe global z en N.m.
 * \param nom : nom de la charge.
 * \return
 *   Succès : un pointeur vers la nouvelle charge.\n
 *   Échec : NULL :
 *     - p == NULL,
 *     - action == NULL,
 *     - noeuds == NULL,
 *     - en cas d'erreur d'allocation mémoire.
 */
Charge *
EF_charge_noeud_ajout (Projet     *p,
                       Action     *action,
                       GList      *noeuds,
                       Flottant    fx,
                       Flottant    fy,
                       Flottant    fz,
                       Flottant    mx,
                       Flottant    my,
                       Flottant    mz,
                       const char *nom)
{
  Charge       *charge;
  Charge_Noeud *charge_d;
  
  BUGPARAM (p, "%p", p, NULL)
  BUGPARAM (action, "%p", action, NULL)
  BUGPARAM (noeuds, "%p", noeuds, NULL)
  BUGCRIT (charge = malloc (sizeof (Charge)),
           NULL,
           (gettext ("Erreur d'allocation mémoire.\n")); )
  BUGCRIT (charge_d = malloc (sizeof (Charge_Noeud)),
           NULL,
           (gettext ("Erreur d'allocation mémoire.\n"));
             free (charge); )
  charge->data = charge_d;
  
  charge->type = CHARGE_NOEUD;
  charge_d->noeuds = noeuds;
  charge_d->fx = fx;
  charge_d->fy = fy;
  charge_d->fz = fz;
  charge_d->mx = mx;
  charge_d->my = my;
  charge_d->mz = mz;
  
  BUG (EF_charge_ajout (p, action, charge, nom),
       NULL,
       free (charge);
         free (charge_d); )
  
  return charge;
}


/**
 * \brief Renvoie la description d'une charge de type ponctuelle sur noeud.
 * \param charge : la charge à décrire.
 * \return
 *   Succès : une chaîne de caractère.\n
 *   Échec : NULL :
 *     - charge == NULL,
 *     - en cas d'erreur d'allocation mémoire.
 */
// coverity[+alloc]
char *
EF_charge_noeud_description (Charge *charge)
{
  Charge_Noeud *charge_d;
  char  txt_fx[30], txt_fy[30], txt_fz[30];
  char  txt_mx[30], txt_my[30], txt_mz[30];
  char  *txt_liste_noeuds, *description;
  
  BUGPARAM (charge, "%p", charge, NULL)
  
  charge_d = charge->data;
  
  BUG (txt_liste_noeuds = common_selection_noeuds_en_texte (charge_d->noeuds),
       NULL)
  conv_f_c (charge_d->fx, txt_fx, DECIMAL_FORCE);
  conv_f_c (charge_d->fy, txt_fy, DECIMAL_FORCE);
  conv_f_c (charge_d->fz, txt_fz, DECIMAL_FORCE);
  conv_f_c (charge_d->mx, txt_mx, DECIMAL_MOMENT);
  conv_f_c (charge_d->my, txt_my, DECIMAL_MOMENT);
  conv_f_c (charge_d->mz, txt_mz, DECIMAL_MOMENT);
  
  BUGCRIT (description = g_strdup_printf (
             "%s : %s, Fx : %s N, Fy : %s N, Fz : %s N, Mx : %s N.m, My : %s N.m, Mz : %s N.m", // NS
             strstr (txt_liste_noeuds, ";") == NULL ?
               gettext ("Noeud") :
               gettext ("Noeuds"),
             txt_liste_noeuds,
             txt_fx,
             txt_fy,
             txt_fz,
             txt_mx,
             txt_my,
             txt_mz),
           NULL,
           (gettext ("Erreur d'allocation mémoire.\n"));
             free (txt_liste_noeuds); )
  
  free (txt_liste_noeuds);
  
  return description;
}


/**
 * \brief Enlève à la charge une liste de noeuds pouvant être utilisés. Dans le
 *        cas où un noeud de la liste n'est pas dans la charge, ce point ne
 *        sera pas considéré comme une erreur mais le noeud sera simplement
 *        ignoré.
 * \param charge : la charge à modifier,
 * \param noeuds : la liste de pointers de type EF_Noeud devant être retirés,
 * \param  p : la variable projet.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - charge == NULL.
 */
gboolean
EF_charge_noeud_enleve_noeuds (Charge *charge,
                               GList  *noeuds,
                               Projet *p)
{
  GList        *list_parcours = noeuds;
  Charge_Noeud *charge_d;
  
  BUGPARAM (charge, "%p", charge, FALSE)
  
  charge_d = charge->data;
  
  while (list_parcours != NULL)
  {
    EF_Noeud *noeud = list_parcours->data;
    
    charge_d->noeuds = g_list_remove (charge_d->noeuds, noeud);
    
    list_parcours = g_list_next (list_parcours);
  }
  
#ifdef ENABLE_GTK
  if (UI_ACT.builder != NULL)
  {
    GtkTreeModel *model;
    GtkTreeIter   Iter;
    
    if (gtk_tree_selection_get_selected (GTK_TREE_SELECTION (
                                        gtk_builder_get_object (UI_ACT.builder,
                                       "1990_actions_treeview_select_action")),
                                         &model,
                                         &Iter))
    {
      Action *action;
      
      gtk_tree_model_get (model, &Iter, 0, &action, -1);
      
      if (g_list_find (_1990_action_charges_renvoie (action), charge))
      {
        gtk_widget_queue_resize (GTK_WIDGET (UI_ACT.tree_view_charges));
      }
    }
  }
#endif
  
  BUG (EF_calculs_free (p), FALSE)
  
  return TRUE;
}


/**
 * \brief Libère une charge nodale.
 * \param charge : la charge à libérer.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - charge == NULL.
 */
gboolean
EF_charge_noeud_free (Charge *charge)
{
  Charge_Noeud *charge_d;
  
  BUGPARAM (charge, "%p", charge, FALSE)
  charge_d = charge->data;
  
  free (charge->nom);
  g_list_free (charge_d->noeuds);
  free (charge_d);
  free (charge);
  
  return TRUE;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
