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

#include "common_projet.h"
#include "common_erreurs.h"
#include "1990_action.h"
#include "EF_charge_noeud.h"
#include "EF_charge_barre_ponctuelle.h"
#include "EF_charge_barre_repartie_uniforme.h"
#include "EF_calculs.h"


void *EF_charge_cherche(Projet *projet, unsigned int num_action, unsigned int num_charge)
/* Description : Cherche et renvoie la charge demandée.
 * Paramètres : Projet *projet : la variable projet,
 *            : unsigned int num_action : le numéro de l'action,
 *            : unsigned int num_charge : le numéro de la charge.
 * Valeur renvoyée :
 *   Succès : Pointeur vers la charge recherchée.
 *   Échec : NULL :
 *             projet == NULL,
 *             action introuvable,
 *             charge introuvable.
 */
{
    Action  *action;
    GList   *list_parcours;
    
    BUGMSG(projet, NULL, gettext("Paramètre %s incorrect.\n"), "projet");
    
    BUG(action = _1990_action_cherche_numero(projet, num_action), NULL);
    
    list_parcours = action->charges;
    while (list_parcours != NULL)
    {
        Charge_Noeud *charge = list_parcours->data;
        
        if (charge->numero == num_charge)
            return charge;
        
        list_parcours = g_list_next(list_parcours);
    }
    
    BUGMSG(0, NULL, gettext("Charge %u de l'action %u introuvable.\n"), num_charge, num_action);
}


Action *EF_charge_action(Projet *projet, void *charge)
/* Description : Envoie l'action possédant la charge.
 * Paramètres : Projet *projet : la variable projet,
 *            : void *charge : la charge dont on souhaite connaitre l'action.
 * Valeur renvoyée :
 *   Succès : pointeur vers l'action
 *   Échec : NULL :
 *             projet == NULL,
 *             charge == NULL,
 *             la charge n'est pas dans une action.
 */
{
    GList   *list_parcours;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(charge, FALSE, gettext("Paramètre %s incorrect.\n"), "charge");
    
    list_parcours = projet->actions;
    while (list_parcours != NULL)
    {
        Action  *action = list_parcours->data;
        
        if (g_list_find(action->charges, charge) != NULL)
            return action;
        
        list_parcours = g_list_next(list_parcours);
    }
    
    BUGMSG(0, NULL, gettext("La charge n'est dans aucune action.\n"));
}


gboolean EF_charge_renomme(Projet *projet, unsigned int numero_action,
  unsigned int numero_charge, const char *nom)
/* Description : Renomme une charge.
 * Paramètres : Projet *projet : la variable projet,
 *            : unsigned int num_action : le numéro de l'action,
 *            : unsigned int num_charge : le numéro de la charge,
 *            : const char *nom : le nouveau nom.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL,
 *             action introuvable,
 *             charge introuvable,
 *             erreur d'allocation mémoire.
 */
{
    Charge_Noeud    *charge;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    
    BUG(charge = EF_charge_cherche(projet, numero_action, numero_charge), -1);
    
    free(charge->nom);
    BUGMSG(charge->nom = g_strdup_printf("%s", nom), -1, gettext("Erreur d'allocation mémoire.\n"));
    
#ifdef ENABLE_GTK
    if (projet->list_gtk._1990_actions.builder != NULL)
        gtk_widget_queue_draw(GTK_WIDGET(projet->list_gtk._1990_actions.tree_view_charges));
#endif
    
    return TRUE;
}


gboolean EF_charge_deplace(Projet *projet, unsigned int action_src, unsigned int charge_src,
  unsigned int action_dest)
/* Description : Déplace une charge d'une action à l'autre.
 *               La charge une dois déplacée sera en fin de la liste et les numéros des charges
 *               dans l'action d'origine seront décrémentés afin que les numéros soit toujours
 *               continus.
 * Paramètres : Projet *projet : la variable projet,
 *              unsigned int action_src : numéro de l'action où se situe la charge à déplacer,
 *              unsigned int charge_src : numéro de la charge à déplacer,
 *              unsigned int action_dest : numéro de l'action où sera déplacer la charge.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL,
 *             action action_src introuvable,
 *             action action_dest introuvable,
 *             charge charge_src dans l'action action_src introuvable.
 */
{
    Charge_Noeud    *charge_data = NULL;
    GList           *list_parcours;
    Action          *action1, *action2;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    // On cherche l'action qui contient la charge
    BUG(action1 = _1990_action_cherche_numero(projet, action_src), -1);
    BUG(action2 = _1990_action_cherche_numero(projet, action_dest), -1);
    
    if (action_src == action_dest)
        return TRUE;
    
    // Lorsqu'elle est trouvée,
    
    list_parcours = action1->charges;
    //     Pour chaque charge de l'action en cours Faire
    while (list_parcours != NULL)
    {
        Charge_Noeud *charge = (Charge_Noeud*)list_parcours->data;
        
    //         Si la charge est celle à supprimer Alors
        if (charge->numero == charge_src)
        {
            GList   *list_next = g_list_next(list_parcours);
            
#ifdef ENABLE_GTK
    //             On la supprime du tree-view-charge
            if (projet->list_gtk._1990_actions.builder != NULL)
                gtk_tree_store_remove(GTK_TREE_STORE(gtk_builder_get_object(projet->list_gtk._1990_actions.builder, "1990_actions_tree_store_charge")), &charge->Iter);
#endif
    //             et de la liste des charges tout en conservant les données
    //               de la charge dans charge_data.
            charge_data = charge;
            action1->charges = g_list_delete_link(action1->charges, list_parcours);
            list_parcours = list_next;
            if (list_parcours != NULL)
                charge = list_parcours->data;
            else
                charge = NULL;
        }
        
    //         Sinon Si la charge possède un numéro supérieur à la charge supprimée alors
        if ((charge_data != NULL) && (charge != NULL) && (charge->numero > charge_src))
        {
    //             On décrémente son numéro dans le tree-view-charges
            charge->numero--;
#ifdef ENABLE_GTK
            if (projet->list_gtk._1990_actions.builder != NULL)
                gtk_tree_store_set(GTK_TREE_STORE(gtk_builder_get_object(projet->list_gtk._1990_actions.builder, "1990_actions_tree_store_charge")), &charge->Iter, 0, charge->numero, -1);
#endif
        }
        
        list_parcours = g_list_next(list_parcours);
    //     FinPour
    }
    
    BUGMSG(charge_data, FALSE, gettext("Charge %u de l'action %u introuvable.\n"), charge_src, action_src);
    
    // On insère la charge à la fin de la liste des charges dans l'action de destination
    //   en modifiant son numéro.
    charge_data->numero = g_list_length(action2->charges);
    action2->charges = g_list_append(action2->charges, charge_data);
    
    BUG(EF_calculs_free(projet), FALSE);
    
    return TRUE;
}


gboolean EF_charge_supprime(Projet *projet, unsigned int action_num, unsigned int charge_num)
/* Description : Supprime une charge. Décrémente également le numéro des charges possédant un
 *               numéro supérieur à la charge supprimée afin que la liste des numéros soit
 *               toujours continue.
 * Paramètres : Projet *projet : la variable projet,
 *              unsigned int action_num : numéro de l'action où se situe la charge à supprimer,
 *              unsigned int charge_num : numéro de la charge à supprimer.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL,
 *             action introuvable,
 *             charge introuvable,
 *             erreur lors de la libération de la charge.
 */
{
    Charge_Noeud            *charge_data = NULL;
    GList                   *list_parcours;
    Action                  *action;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUG(action = _1990_action_cherche_numero(projet, action_num), FALSE);
    
    list_parcours = action->charges;
    // Pour chaque charge de l'action en cours Faire
    while (list_parcours != NULL)
    {
        Charge_Noeud *charge = list_parcours->data;
    //     Si la charge est celle à supprimer Alors
        if (charge->numero == charge_num)
        {
            GList   *list_next = g_list_next(list_parcours);
#ifdef ENABLE_GTK
    //         On la supprime du tree-view-charge
             if (projet->list_gtk._1990_actions.builder != NULL)
                 gtk_tree_store_remove(GTK_TREE_STORE(gtk_builder_get_object(projet->list_gtk._1990_actions.builder, "1990_actions_tree_store_charge")), &charge->Iter);
#endif
    //         et de la liste des charges tout en conservant les données
    //           de la charge dans charge_data
            charge_data = list_parcours->data;
            action->charges = g_list_delete_link(action->charges, list_parcours);
            list_parcours = list_next;
            if (list_parcours != NULL)
                charge = list_parcours->data;
            else
                charge = NULL;
    
    //         On libère la charge charge_data
            switch (charge_data->type)
            {
                case CHARGE_NOEUD :
                {
                    BUG(EF_charge_noeud_free(charge_data), FALSE);
                    break;
                }
                case CHARGE_BARRE_PONCTUELLE :
                {
                    BUG(EF_charge_barre_ponctuelle_free((Charge_Barre_Ponctuelle*)charge_data), FALSE);
                    break;
                }
                case CHARGE_BARRE_REPARTIE_UNIFORME :
                {
                    BUG(EF_charge_barre_repartie_uniforme_free((Charge_Barre_Repartie_Uniforme*)charge_data), FALSE);
                    break;
                }
                default :
                {
                    BUGMSG(0, FALSE, gettext("Type de charge %d inconnu.\n"), charge_data->type);
                    break;
                }
            }
        }
    //     Sinon Si la charge possède un numéro supérieur à la charge supprimée alors
        if ((charge_data != NULL) && (charge != NULL) && (charge->numero > charge_num))
        {
    //         On décrémente son numéro dans le tree-view
            charge->numero--;
#ifdef ENABLE_GTK
            if (projet->list_gtk._1990_actions.builder != NULL)
                gtk_tree_store_set(GTK_TREE_STORE(gtk_builder_get_object(projet->list_gtk._1990_actions.builder, "1990_actions_tree_store_charge")), &charge->Iter, 0, charge->numero, -1);
#endif
         }
    // FinPour
        list_parcours = g_list_next(list_parcours);
    }
    
    BUGMSG(charge_data, FALSE, gettext("Charge %u de l'action %u introuvable.\n"), charge_num, action_num);
    
    BUG(EF_calculs_free(projet), FALSE);
    
    return TRUE;
}
