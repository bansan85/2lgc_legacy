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
#include "EF_calculs.h"
#include "EF_gtk_charge_noeud.h"


Charge_Noeud*  EF_charge_noeud_ajout(Projet *projet, unsigned int num_action, GList *noeuds,
  Flottant fx, Flottant fy, Flottant fz, Flottant mx, Flottant my, Flottant mz, const char* nom)
/* Description : Ajoute une charge ponctuelle à une action et à un noeud de la structure en
 *               lui attribuant le numéro suivant la dernière charge de l'action.
 * Paramètres : Projet *projet : la variable projet,
 *            : int num_action : numero de l'action qui contiendra la charge,
 *            : GList *noeuds : liste des noeuds qui supportera la charge,
 *            : Flottant fx : force suivant l'axe global x,
 *            : Flottant fy : force suivant l'axe global y,
 *            : Flottant fz : force suivant l'axe global z,
 *            : Flottant mx : moment autour de l'axe global x,
 *            : Flottant my : moment autour de l'axe global y,
 *            : Flottant mz : moment autour de l'axe global z.
 * Valeur renvoyée :
 *   Succès : un pointeur vers la nouvelle charge
 *   Échec : NULL :
 *             projet == NULL,
 *             action introuvable,
 *             en cas d'erreur d'allocation mémoire.
 */
{
    Action          *action_en_cours;
    Charge_Noeud    *charge_nouveau;
#ifdef ENABLE_GTK
    GtkTreeIter     iter_action;
    Action          *action;
    GtkTreeModel    *model_action;
#endif
    
    // Trivial
    BUGMSG(projet, NULL, gettext("Paramètre %s incorrect.\n"), "projet");
    BUG(action_en_cours = _1990_action_cherche_numero(projet, num_action), NULL);
    BUGMSG(charge_nouveau = malloc(sizeof(Charge_Noeud)), NULL, gettext("Erreur d'allocation mémoire.\n"));
    
    charge_nouveau->type = CHARGE_NOEUD;
    BUGMSG(charge_nouveau->nom = g_strdup_printf("%s", nom), NULL, gettext("Erreur d'allocation mémoire.\n"));
    charge_nouveau->noeuds = noeuds;
    charge_nouveau->fx = fx;
    charge_nouveau->fy = fy;
    charge_nouveau->fz = fz;
    charge_nouveau->mx = mx;
    charge_nouveau->my = my;
    charge_nouveau->mz = mz;
    
    charge_nouveau->numero = g_list_length(action_en_cours->charges);
    
    action_en_cours->charges = g_list_append(action_en_cours->charges, charge_nouveau);
    
    BUG(EF_calculs_free(projet), FALSE);
    
#ifdef ENABLE_GTK
    if ((projet->list_gtk._1990_actions.builder != NULL) && (gtk_tree_selection_get_selected(projet->list_gtk._1990_actions.tree_select_actions, &model_action, &iter_action)))
    {
        gtk_tree_model_get(model_action, &iter_action, 0, &action, -1);
        if (action->numero == num_action)
        {
            gtk_tree_store_append(projet->list_gtk._1990_actions.tree_store_charges, &charge_nouveau->Iter, NULL);
            gtk_tree_store_set(projet->list_gtk._1990_actions.tree_store_charges, &charge_nouveau->Iter, 0, charge_nouveau, -1);
        }
    }
#endif
    
    return charge_nouveau;
}


char* EF_charge_noeud_description(Charge_Noeud *charge)
/* Description : Renvoie la description d'une charge de type ponctuelle sur noeud.
 * Paramètres : Charge_Noeud *charge : la charge à décrire.
 * Valeur renvoyée :
 *   Succès : une chaîne de caractère.
 *   Échec : NULL :
 *             charge == NULL,
 *             en cas d'erreur d'allocation mémoire.
 */
{
    char    txt_fx[30], txt_fy[30], txt_fz[30], txt_mx[30], txt_my[30], txt_mz[30];
    char    *txt_liste_noeuds, *description;
    
    BUGMSG(charge, FALSE, gettext("Paramètre %s incorrect.\n"), "charge");
    
    BUG(txt_liste_noeuds = common_selection_converti_noeuds_en_texte(charge->noeuds), NULL);
    common_math_double_to_char2(charge->fx, txt_fx, DECIMAL_FORCE);
    common_math_double_to_char2(charge->fy, txt_fy, DECIMAL_FORCE);
    common_math_double_to_char2(charge->fz, txt_fz, DECIMAL_FORCE);
    common_math_double_to_char2(charge->mx, txt_mx, DECIMAL_MOMENT);
    common_math_double_to_char2(charge->my, txt_my, DECIMAL_MOMENT);
    common_math_double_to_char2(charge->mz, txt_mz, DECIMAL_MOMENT);
    
    BUGMSG(description = g_strdup_printf("%s : %s, Fx : %s N, Fy : %s N, Fz : %s N, Mx : %s N.m, My : %s N.m, Mz : %s N.m", strstr(txt_liste_noeuds, ";") == NULL ? gettext("Noeud") : gettext("Noeuds"), txt_liste_noeuds, txt_fx, txt_fy, txt_fz, txt_mx, txt_my, txt_mz), FALSE, gettext("Erreur d'allocation mémoire.\n"));
    
    free(txt_liste_noeuds);
    
    return description;
}


gboolean EF_charge_noeud_enleve_noeuds(Charge_Noeud *charge, GList *noeuds, Projet *projet)
/* Description : Enlève à la charge une liste de noeuds pouvant être utilisés. Dans le cas où
 *               un noeud de la liste n'est pas dans la charge, ce point ne sera pas considéré
 *               comme une erreur mais le noeud sera simplement ignoré.
 * Paramètres : Charge_Noeud *charge : la charge à modifier,
 *              GList *noeuds : la liste de pointers de type EF_Noeud devant être retirés,
 *              Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             charge == NULL.
 */
{
    GList   *list_parcours = noeuds;
    
    BUGMSG(charge, FALSE, gettext("Paramètre %s incorrect.\n"), "charge");
    
    while (list_parcours != NULL)
    {
        EF_Noeud *noeud = list_parcours->data;
        
        charge->noeuds = g_list_remove(charge->noeuds, noeud);
        
        list_parcours = g_list_next(list_parcours);
    }
    
#ifdef ENABLE_GTK
    if (projet->list_gtk._1990_actions.builder != NULL)
    {
        GtkTreeModel    *model;
        GtkTreeIter     Iter;
        
        if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION(gtk_builder_get_object(projet->list_gtk._1990_actions.builder, "1990_actions_treeview_select_action")), &model, &Iter))
        {
            Action          *action;
            
            gtk_tree_model_get(model, &Iter, 0, &action, -1);
            
            if (g_list_find(action->charges, charge))
                gtk_widget_queue_draw(GTK_WIDGET(projet->list_gtk._1990_actions.tree_view_charges));
        }
    }
#endif
    
    BUG(EF_calculs_free(projet), FALSE);
    
    return TRUE;
}


gboolean EF_charge_noeud_free(Charge_Noeud *charge)
/* Description : Libère une charge nodale.
 * Paramètres : Charge_Noeud *charge : la charge à libérer.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             charge == NULL.
 */
{
    BUGMSG(charge, FALSE, gettext("Paramètre %s incorrect.\n"), "charge");
    
    free(charge->nom);
    g_list_free(charge->noeuds);
    free(charge);
    
    return TRUE;
}
