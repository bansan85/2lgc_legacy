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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>

#include "1990_actions.h"
#include "1990_groupes.h"
#include "1990_ponderations.h"
#include "1990_combinaisons.h"
#include "common_erreurs.h"
#include "common_projet.h"
#include "common_tooltip.h"
#include "common_gtk.h"

const GtkTargetEntry drag_targets_groupes_1[] = { {(gchar*)PACKAGE"1_SAME_PROC", GTK_TARGET_SAME_APP, 0}};
const GtkTargetEntry drag_targets_groupes_2[] = { {(gchar*)PACKAGE"2_SAME_PROC", GTK_TARGET_SAME_APP, 0}};
const GtkTargetEntry drag_targets_groupes_3[] = { {(gchar*)PACKAGE"1_SAME_PROC", GTK_TARGET_SAME_APP, 0},
                                                  {(gchar*)PACKAGE"2_SAME_PROC", GTK_TARGET_SAME_APP, 0}};
 
unsigned int _1990_gtk_get_groupe(GtkTreeModel *tree_model, GtkTreeIter *iter)
/* Description : Renvoie le numéro du groupe de l'élément iter.
 * Paramètres : GtkTreeModel *tree_model : tree_model contenant la liste des éléments
 *            : GtkTreeIter *iter : contient la ligne à étudier.
 * Valeur renvoyée : le numéro du groupe
 */
{
    unsigned int    numero;
    GtkTreeIter     iter_parent;
    
    if (gtk_tree_model_iter_parent(tree_model, &iter_parent, iter))
    {
        gtk_tree_model_get(tree_model, &iter_parent, 0, &numero, -1);
        return numero;
    }
    else
    {
        gtk_tree_model_get(tree_model, iter, 0, &numero, -1);
        return numero;
    }
}


G_MODULE_EXPORT void _1990_gtk_tree_view_etat_cursor_changed(__attribute__((unused)) GtkTreeView *tree_view, Projet *projet)
/* Description : Evènement lorsqu'il y a un changement de ligne sélectionnée
 *             : Permet de mettre à jour si le groupe sélectionné est de type OR, XOR ou AND
 * Paramètres : GtkTreeView *tree_view : composant tree_view à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    List_Gtk_1990_Groupes   *list_gtk_1990_groupes;
    GtkTreeModel    *model;
    GtkTreeIter     iter;
    unsigned int    ngroupe;
    Niveau_Groupe   *niveau_groupe;
    Groupe          *groupe;
    
    BUGMSG(projet, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->niveaux_groupes, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk._1990_groupes.builder, , gettext("Paramètre incorrect\n"));
    
    // window_groupe peut être NULL lorsque la fenêtre est fermée.
    if (projet->list_gtk._1990_groupes.window_groupe == NULL)
        return;
    
    list_gtk_1990_groupes = &projet->list_gtk._1990_groupes;
    
    /* Détermine le groupe sélectionné */
    if (!gtk_tree_selection_get_selected(list_gtk_1990_groupes->tree_select_etat, &model, &iter))
        return;
    ngroupe = _1990_gtk_get_groupe(model, &iter);
    BUG(niveau_groupe = _1990_groupe_positionne_niveau(projet, GTK_COMMON_SPINBUTTON_AS_UINT(GTK_SPIN_BUTTON(list_gtk_1990_groupes->spin_button_niveau))), );
    BUG(groupe = _1990_groupe_positionne_groupe(niveau_groupe, ngroupe), );
    
    /* On active le toggle_button correspondant au type de combinaison du groupe */
    switch (groupe->type_combinaison)
    {
        case GROUPE_COMBINAISON_OR :
        {
            gtk_toggle_tool_button_set_active(GTK_TOGGLE_TOOL_BUTTON(list_gtk_1990_groupes->item_groupe_or), TRUE);
            break;
        }
        case GROUPE_COMBINAISON_XOR :
        {
            gtk_toggle_tool_button_set_active(GTK_TOGGLE_TOOL_BUTTON(list_gtk_1990_groupes->item_groupe_xor), TRUE);
            break;
        }
        case GROUPE_COMBINAISON_AND :
        {
            gtk_toggle_tool_button_set_active(GTK_TOGGLE_TOOL_BUTTON(list_gtk_1990_groupes->item_groupe_and), TRUE);
            break;
        }
        default :
        {
            break;
        }
    }
    
    return;
}


G_MODULE_EXPORT gboolean _1990_gtk_groupe_window_key_press(GtkWidget *widget __attribute__((unused)), GdkEvent *event, Projet *projet)
{
    BUGMSG(projet, TRUE, gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk._1990_groupes.builder, TRUE, gettext("Paramètre incorrect\n"));
    
    if (event->key.keyval == GDK_KEY_Escape)
    {
        gtk_widget_destroy(projet->list_gtk._1990_groupes.window_groupe);
        return TRUE;
    }
    else
        return FALSE;
}


G_MODULE_EXPORT gboolean _1990_gtk_groupe_option_window_key_press(GtkWidget *widget __attribute__((unused)), GdkEvent *event, Projet *projet)
{
    BUGMSG(projet, TRUE, gettext("Paramètre incorrect\n"));
    
    if (event->key.keyval == GDK_KEY_Escape)
    {
        gtk_widget_destroy(widget);
        return TRUE;
    }
    else
        return FALSE;
}


int _1990_gtk_affiche_niveau(Projet *projet, unsigned int niveau)
/* Description : Affiche le niveau souhaité dans l'interface graphique
 * Paramètres : Projet *projet : variable projet
 *            : int niveau : niveau à afficher
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec :  -1 en cas de paramètres invalides :
 *              (projet == NULL) ou
 *              (projet->list_gtk._1990_groupes == NULL) ou
 *              (list_size(projet->niveaux_groupes)-1 < niveau)
 *            -2 en cas d'erreur d'allocation mémoire
 *            -3 en cas d'erreur due à une fonction interne
 */
{
    Niveau_Groupe           *niveau_groupe;
    unsigned int            dispo_max, i;
    char                    *dispos;
    gboolean                premier = TRUE;
    List_Gtk_1990_Groupes   *list_gtk_1990_groupes;
    GtkTreePath             *path;
    
    BUGMSG(projet, -1, gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk._1990_groupes.builder, -1, gettext("Paramètre incorrect\n"));
    
    list_gtk_1990_groupes = &projet->list_gtk._1990_groupes;
    
    // Il convient de bloquer le signal. Sinon, des erreurs peuvent apparaitre si une ligne a été sélectionnée (par exemple la première) à la souris et qu'on demande ensuite de changer de niveau via le spin_button.
    g_signal_handler_block(list_gtk_1990_groupes->tree_view_etat, g_signal_handler_find(G_OBJECT(list_gtk_1990_groupes->tree_view_etat),G_SIGNAL_MATCH_FUNC,0,0,NULL,_1990_gtk_tree_view_etat_cursor_changed,NULL));
    /* On supprime le contenu des deux composants tree_view */
    gtk_tree_store_clear(list_gtk_1990_groupes->tree_store_etat);
    g_signal_handler_unblock(list_gtk_1990_groupes->tree_view_etat, g_signal_handler_find(G_OBJECT(list_gtk_1990_groupes->tree_view_etat),G_SIGNAL_MATCH_FUNC,0,0,NULL,_1990_gtk_tree_view_etat_cursor_changed,NULL));
    gtk_tree_store_clear(list_gtk_1990_groupes->tree_store_dispo);
    
    /* dispo_max contient le nombre d'éléments maximum pouvant être disponible depuis le 
     * niveau 'niveau'-1 */
    if (niveau == 0)
        dispo_max = g_list_length(projet->actions);
    else
    {
        BUG(niveau_groupe = _1990_groupe_positionne_niveau(projet, niveau-1), -3);
        BUGMSG(niveau_groupe->groupes, -1, gettext("Paramètre incorrect\n"));
        dispo_max = g_list_length(niveau_groupe->groupes);
    }
    
    /* Ensuite, on initialise un tableau contenant une liste de boolean
     * pour déterminer au fur et à mesure de l'avancement de l'algorithme
     * quelques sont les éléments du niveau n-1 encore non placés */
    if (dispo_max != 0)
    {
        BUGMSG(dispos = (char*)malloc(sizeof(char)*dispo_max), -2, gettext("Erreur d'allocation mémoire.\n"));
        for (i=0;i<dispo_max;i++)
            dispos[i] = 0;
    }
    else
        dispos = NULL;
    
    BUG(niveau_groupe = _1990_groupe_positionne_niveau(projet, niveau), -3);
    if (niveau_groupe->groupes != NULL)
    {
        GList   *list_parcours = niveau_groupe->groupes;
        
        /* Parcours le niveau à afficher */
        do
        {
            Groupe      *groupe = list_parcours->data;
            
            /* Ajoute de la ligne dans le tree_store */
            gtk_tree_store_append(list_gtk_1990_groupes->tree_store_etat, &groupe->Iter, NULL);
            gtk_tree_store_set(list_gtk_1990_groupes->tree_store_etat, &groupe->Iter, 0, groupe->numero, 1, groupe->nom, -1);
            
            /* Sélection de la première ligne du tree_view_etat */
            if (list_parcours->data == list_parcours->data)
            {
                path = gtk_tree_model_get_path(GTK_TREE_MODEL(list_gtk_1990_groupes->tree_store_etat), &groupe->Iter);
                gtk_tree_selection_select_path(list_gtk_1990_groupes->tree_select_etat, path);
                gtk_tree_path_free(path);
            }
            
            if (groupe->elements != NULL)
            {
                GList *list_parcours2 = groupe->elements;
                
                do
                {
                    Element     *element = list_parcours2->data;
                    
                    /* On signale que l'élément a déjà été inséré */
                    dispos[element->numero] = 1;
                    /* puis ajout de la ligne dans le tree_store */
                    gtk_tree_store_append(list_gtk_1990_groupes->tree_store_etat, &element->Iter, &groupe->Iter);
                    if (niveau == 0)
                    {
                        Action *action;
                        
                        BUG(action = _1990_action_cherche_numero(projet, element->numero), -3);
                        gtk_tree_store_set(list_gtk_1990_groupes->tree_store_etat, &element->Iter, 0, element->numero, 1, action->description, -1);
                    }
                    else
                    {
                        Niveau_Groupe *groupe_niveau_moins_1;
                        Groupe *groupe_moins_1;
                        BUG(groupe_niveau_moins_1 = _1990_groupe_positionne_niveau(projet, niveau-1), -3);
                        BUG(groupe_moins_1 = _1990_groupe_positionne_groupe(groupe_niveau_moins_1, element->numero), -3);
                        gtk_tree_store_set(list_gtk_1990_groupes->tree_store_etat, &element->Iter, 0, element->numero, 1, groupe_moins_1->nom, -1);
                    }
                    
                    list_parcours2 = g_list_next(list_parcours2);
                }
                while (list_parcours2 != NULL);
            }
            /* Si la dernière fois que la ligne était affiché, elle était 'expand',
             * on rétablit son attribut */
            if (groupe->Iter_expand == 1)
            {
                path = gtk_tree_model_get_path(GTK_TREE_MODEL(list_gtk_1990_groupes->tree_store_etat), &groupe->Iter);
                gtk_tree_view_expand_row(list_gtk_1990_groupes->tree_view_etat, path, FALSE);
                gtk_tree_path_free(path);
            }
            list_parcours = g_list_next(list_parcours);
        }
        while (list_parcours != NULL);
    }
    
    /* On affiche tous les éléments disponibles dans le tree_view_dispo */
    for (i=0;i<dispo_max;i++)
    {
        if (dispos[i] == 0)
        {
            GtkTreeIter Iter;
            gtk_tree_store_append(list_gtk_1990_groupes->tree_store_dispo, &Iter, NULL);
            
            /* Sélection de la première ligne du tree_view_dispo */
            if (premier == TRUE)
            {
                path = gtk_tree_model_get_path(GTK_TREE_MODEL(list_gtk_1990_groupes->tree_store_dispo), &Iter);
                gtk_tree_selection_select_path(list_gtk_1990_groupes->tree_select_dispo, path);
                gtk_tree_path_free(path);
                premier = FALSE;
            }
            
            if (niveau == 0)
            {
                Action *action;
                
                BUG(action = _1990_action_cherche_numero(projet, i), -3);
                gtk_tree_store_set(list_gtk_1990_groupes->tree_store_dispo, &Iter, 0, i, 1, action->description, -1);
            }
            else
            {
                Niveau_Groupe *groupe_niveau_moins_1;
                Groupe *groupe_moins_1;
                BUG(groupe_niveau_moins_1 = _1990_groupe_positionne_niveau(projet, niveau-1), -3);
                BUG(groupe_moins_1 = _1990_groupe_positionne_groupe(groupe_niveau_moins_1, i), -3);
                gtk_tree_store_set(list_gtk_1990_groupes->tree_store_dispo, &Iter, 0, i, 1, groupe_moins_1->nom, -1);
            }
        }
    }

    free(dispos);
    
    return 0;
}


G_MODULE_EXPORT void _1990_gtk_spin_button_niveau_change(GtkWidget *button __attribute__((unused)), Projet *projet)
/* Description : Met à jour l'affichage graphique en fonction de la valeur du spin_button
 * Paramètres : GtkWidget *button : composant spin_button ayant réalisé l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    List_Gtk_1990_Groupes   *list_gtk_1990_groupes;
    
    BUGMSG(projet, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk._1990_groupes.builder, , gettext("Paramètre incorrect\n"));
    
    list_gtk_1990_groupes = &projet->list_gtk._1990_groupes;
    BUG(_1990_gtk_affiche_niveau(projet, GTK_COMMON_SPINBUTTON_AS_UINT(GTK_SPIN_BUTTON(list_gtk_1990_groupes->spin_button_niveau))) == 0, );
    return;
}


G_MODULE_EXPORT void _1990_gtk_button_niveau_suppr_clicked(GtkWidget *button __attribute__((unused)), Projet *projet)
/* Description : Supprime le niveau en cours
 * Paramètres : GtkWidget *button : composant spin_button ayant réalisé l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    List_Gtk_1990_Groupes   *list_gtk_1990_groupes;
    
    BUGMSG(projet, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk._1990_groupes.builder, , gettext("Paramètre incorrect\n"));
    
    list_gtk_1990_groupes = &projet->list_gtk._1990_groupes;
    BUG(_1990_groupe_free_niveau(projet, GTK_COMMON_SPINBUTTON_AS_UINT(GTK_SPIN_BUTTON(list_gtk_1990_groupes->spin_button_niveau))) == 0, );
    
    /* On oblige la liste des niveaux à posséder au moins un niveau vide */
    if (projet->niveaux_groupes == NULL)
    {
        BUG(_1990_groupe_ajout_niveau(projet) == 0, );
        gtk_tree_store_clear(list_gtk_1990_groupes->tree_store_etat);
        BUG(_1990_gtk_affiche_niveau(projet, 0) == 0, );
    }
    
    /* On réajuste les limites du spin_button */
    gtk_spin_button_set_range(GTK_SPIN_BUTTON(list_gtk_1990_groupes->spin_button_niveau), 0, g_list_length(projet->niveaux_groupes)-1);
    
    return;
}


G_MODULE_EXPORT void _1990_gtk_button_niveau_ajout_clicked(GtkWidget *button __attribute__((unused)), Projet *projet)
/* Description : Ajoute un niveau
 * Paramètres : GtkWidget *button : composant spin_button ayant réalisé l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    List_Gtk_1990_Groupes   *list_gtk_1990_groupes;
    
    BUGMSG(projet, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk._1990_groupes.builder, , gettext("Paramètre incorrect\n"));
    
    list_gtk_1990_groupes = &projet->list_gtk._1990_groupes;
    BUG(_1990_groupe_ajout_niveau(projet) == 0, );
    
    /* Mise à jour des limites du composant spin_button */
    gtk_tree_store_clear(list_gtk_1990_groupes->tree_store_etat);
    gtk_spin_button_set_range(GTK_SPIN_BUTTON(list_gtk_1990_groupes->spin_button_niveau), 0, g_list_length(projet->niveaux_groupes)-1);
    
    /* La modification de la valeur du composant spin_button execute automatiquement
     * _1990_gtk_spin_button_niveau_change qui met à jour l'interface graphique */
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(list_gtk_1990_groupes->spin_button_niveau), g_list_length(projet->niveaux_groupes)-1);
    
    return;
}


G_MODULE_EXPORT void _1990_gtk_button_groupe_ajout_clicked(GtkWidget *button __attribute__((unused)), Projet *projet)
/* Description : Ajoute un groupe dans le niveau en cours
 * Paramètres : GtkWidget *button : composant spin_button ayant réalisé l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    List_Gtk_1990_Groupes   *list_gtk_1990_groupes;
    Niveau_Groupe   *niveau_groupe;
    Groupe          *groupe;
    GtkTreePath     *path;
    
    BUGMSG(projet, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk._1990_groupes.builder, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->niveaux_groupes, , gettext("Paramètre incorrect\n"));
    
    list_gtk_1990_groupes = &projet->list_gtk._1990_groupes;
    
    /* On ajoute un niveau */
    BUG(niveau_groupe = _1990_groupe_positionne_niveau(projet, GTK_COMMON_SPINBUTTON_AS_UINT(GTK_SPIN_BUTTON(list_gtk_1990_groupes->spin_button_niveau))), );
    BUG(groupe = _1990_groupe_ajout_groupe(projet, niveau_groupe->niveau, GROUPE_COMBINAISON_AND, gettext("Sans nom")), );
    
    /* Et on met à jour l'affichage */
    gtk_tree_store_append(list_gtk_1990_groupes->tree_store_etat, &groupe->Iter, NULL);
    gtk_tree_store_set(list_gtk_1990_groupes->tree_store_etat, &groupe->Iter, 0, groupe->numero, 1, "", -1);
    path = gtk_tree_model_get_path(GTK_TREE_MODEL(list_gtk_1990_groupes->tree_store_etat), &groupe->Iter);
    gtk_tree_view_set_cursor(GTK_TREE_VIEW(list_gtk_1990_groupes->tree_view_etat), path, gtk_tree_view_get_column(GTK_TREE_VIEW(list_gtk_1990_groupes->tree_view_etat), 1), TRUE);
    gtk_tree_path_free(path);
    
    return;
}

int _1990_gtk_insert_dispo(Projet *projet, unsigned int numero, Niveau_Groupe *niveau)
/* Description : Ajoute dans le tree_view_dispo le numéro de l'élément disponible.
 *             : Cette fonction ne gère que l'interface graphique de tree_view_dispo
 * Paramètres : Projet *projet : la variable projet
 *            : int numero : numero de l'élément
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet == NULL) ou
 *             (projet->list_gtk._1990_groupes == NULL)
 */
{
    List_Gtk_1990_Groupes   *list_gtk_1990_groupes;
    gboolean        retour;
    GtkTreeModel    *model;
    GtkTreeIter     iter, iter2;
    unsigned int    nombre;
    char            *nom;
    GtkTreePath     *path;
    
    BUGMSG(projet, -1, gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk._1990_groupes.builder, -1, gettext("Paramètre incorrect\n"));
    
    list_gtk_1990_groupes = &projet->list_gtk._1990_groupes;
    model = gtk_tree_view_get_model(list_gtk_1990_groupes->tree_view_dispo);
    
    /* On parcours la liste des éléments disponibles à la recherche du numéro */
    retour = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(list_gtk_1990_groupes->tree_store_dispo), &iter2);
    if (retour == TRUE)
    {
        gtk_tree_model_get(model, &iter2, 0, &nombre, 1, &nom, -1);
        BUGMSG(nombre != numero, -1, gettext("Élément %d existant.\n"), numero);
    }
    while ((retour == TRUE) && (nombre < numero))
    {
        retour = gtk_tree_model_iter_next(GTK_TREE_MODEL(list_gtk_1990_groupes->tree_store_dispo), &iter2);
        if (retour == TRUE)
        {
            gtk_tree_model_get(model, &iter2, 0, &nombre, 1, &nom, -1);
            BUGMSG(nombre != numero, -1, gettext("Élément %d existant.\n"), numero);
        }
    }
    
    /* Si aucun élément avec un numéro supérieur est trouvé, on l'ajoute à la fin */
    if (retour == FALSE)
        gtk_tree_store_append(list_gtk_1990_groupes->tree_store_dispo, &iter, NULL);
    /* Sinon, on l'ajoute avant le dernier numéro trouvé */
    else
        gtk_tree_store_insert_before(list_gtk_1990_groupes->tree_store_dispo, &iter, NULL, &iter2);
    
    path = gtk_tree_model_get_path(GTK_TREE_MODEL(list_gtk_1990_groupes->tree_store_dispo), &iter);
    gtk_tree_selection_select_path(list_gtk_1990_groupes->tree_select_dispo, path);
    gtk_tree_path_free(path);
    
    if (projet->niveaux_groupes->data == niveau)
    {
        Action *action;
        
        BUG(action = _1990_action_cherche_numero(projet, numero), -3);
        gtk_tree_store_set(list_gtk_1990_groupes->tree_store_dispo, &iter, 0, numero, 1, action->description, -1);
    }
    else
    {
        Niveau_Groupe *groupe_niveau_moins_1;
        Groupe *groupe_moins_1;
        
        BUG(groupe_niveau_moins_1 = _1990_groupe_positionne_niveau(projet, niveau->niveau-1), -3);
        BUG(groupe_moins_1 = _1990_groupe_positionne_groupe(groupe_niveau_moins_1, numero), -3);
        gtk_tree_store_set(list_gtk_1990_groupes->tree_store_dispo, &iter, 0, numero, 1, groupe_moins_1->nom, -1);
    }
    
    return 0;
}


G_MODULE_EXPORT void _1990_gtk_button_groupe_suppr_clicked(GtkWidget *button __attribute__((unused)), Projet *projet)
/* Description : Supprime le groupe dans le niveau en cours
 * Paramètres : GtkWidget *button : composant spin_button ayant réalisé l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    List_Gtk_1990_Groupes       *list_gtk_1990_groupes;
    GtkTreeModel        *model;
    GtkTreePath         *path;
    GtkTreeIter         iter, iter_tmp;
    unsigned int        niveau;
    unsigned int        numero, ngroupe;
    char                *nom;
    
    BUGMSG(projet, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->niveaux_groupes, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk._1990_groupes.builder, , gettext("Paramètre incorrect\n"));
    
    list_gtk_1990_groupes = &projet->list_gtk._1990_groupes;
    if (!gtk_tree_selection_get_selected(list_gtk_1990_groupes->tree_select_etat, &model, &iter))
        return;
    
    /* On récupère les informations de la ligne sélectionnée */
    gtk_tree_model_get(model, &iter, 0, &numero, 1, &nom, -1);
    ngroupe = _1990_gtk_get_groupe(model, &iter);

    path = gtk_tree_model_get_path(GTK_TREE_MODEL(list_gtk_1990_groupes->tree_store_etat), &iter);
    gtk_tree_path_next(path);
    gtk_tree_selection_select_path(list_gtk_1990_groupes->tree_select_etat, path);
    if (!gtk_tree_selection_path_is_selected(list_gtk_1990_groupes->tree_select_etat, path))
    {
        gtk_tree_path_prev(path);
        if (gtk_tree_path_prev(path))
            gtk_tree_selection_select_path(list_gtk_1990_groupes->tree_select_etat, path);
    }
    gtk_tree_path_free(path);
    
    gtk_tree_selection_unselect_all(list_gtk_1990_groupes->tree_select_dispo);
    niveau = GTK_COMMON_SPINBUTTON_AS_UINT(GTK_SPIN_BUTTON(list_gtk_1990_groupes->spin_button_niveau));
    /* Il s'agit d'un groupe */
    if (!gtk_tree_model_iter_parent(model, &iter_tmp, &iter))
    {
        Niveau_Groupe   *niveau_groupe;
        Groupe          *groupe;
        
        /* On supprimer la ligne */
        gtk_tree_store_remove(list_gtk_1990_groupes->tree_store_etat, &iter);
        /* On positionne le groupe en cours */
        BUG(niveau_groupe = _1990_groupe_positionne_niveau(projet, niveau), );
        BUG(groupe = _1990_groupe_positionne_groupe(niveau_groupe, ngroupe), );
        
  /* On ajoute tous les éléments associés au groupe dans la liste des éléments disponibles */
        if (groupe->elements != NULL)
        {
            GList   *list_parcours = groupe->elements;
            
            do
            {
                Element     *element = list_parcours->data;
                
                BUG(_1990_gtk_insert_dispo(projet, element->numero, niveau_groupe) == 0, );
                
                list_parcours = g_list_next(list_parcours);
            }
            while (list_parcours != NULL);
        }
        
        /* On supprime le groupe */
        BUG(_1990_groupe_free_groupe(projet, niveau, ngroupe) == 0, );
        
        /* Cette fonction est pour éviter d'avoir à utiliser la fonction
         * _1990_gtk_affiche_niveau (optimisation). Une mise à jour du composant graphique
         * est nécessaire car, en supprimant le groupe en cours, tous les groupes avec un
         * numéro supérieur se retrouvent avec leur numéro diminué de 1. */
        if (niveau_groupe->groupes != NULL)
        {
            GList   *list_parcours  = niveau_groupe->groupes;
            
            do
            {
                groupe = list_parcours->data;
                if (groupe->numero >= ngroupe)
                {
                    GValue      nouvelle_valeur;
                    /* Il est INDISPENSABLE de faire un memset avant g_value_init !!! */
                    memset(&nouvelle_valeur, 0, sizeof(nouvelle_valeur));
                    
                    g_value_init(&nouvelle_valeur, G_TYPE_UINT);
        
                    /* On récupère le numéro du groupe */
                    g_value_set_uint(&nouvelle_valeur, groupe->numero);
                    /* Et on modifie la ligne */
                    gtk_tree_store_set_value(list_gtk_1990_groupes->tree_store_etat, &groupe->Iter, 0, &nouvelle_valeur);
                }
                list_parcours = g_list_next(list_parcours);
            }
            while (list_parcours);
        }
    }
    /* Si c'est un élément, on l'ajoute à la liste des éléments disponibles
     * et on le supprime du groupe */
    else
    {
        /* On supprimer la ligne */
        gtk_tree_store_remove(list_gtk_1990_groupes->tree_store_etat, &iter);
        BUG(_1990_gtk_insert_dispo(projet, numero, _1990_groupe_positionne_niveau(projet, niveau)) == 0, );
        BUG(_1990_groupe_free_element(projet, niveau, ngroupe, numero) == 0, );
    }
    
    return;
}


G_MODULE_EXPORT void _1990_gtk_tree_view_drag_begin (GtkWidget *widget, 
  GdkDragContext *drag_context __attribute__((unused)), Projet *projet)
{
    projet->list_gtk._1990_groupes.drag_from = widget;
    return;
}


G_MODULE_EXPORT gboolean _1990_gtk_tree_view_dispo_drag(GtkWidget *widget __attribute__((unused)),
  GdkDragContext *drag_context __attribute__((unused)),
  gint x __attribute__((unused)), gint y __attribute__((unused)),
  guint tim __attribute__((unused)), Projet *projet)
/* Description : Supprimer les éléments sélectionnées du tree_view_etat.
 * Paramètres : GtkWidget *button : composant spin_button ayant réalisé l'évènement,
 *            : GdkDragContext *drag_context : inutile,
 *            : GtkSelectionData *data : inutile,
 *            : guint info : inutile,
 *            : guint time : inutile,
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : FALSE
 */
{
    _1990_gtk_button_groupe_suppr_clicked(widget, projet);
    return FALSE;
}


int _1990_gtk_button_ajout_dispo_proc(unsigned int ngroupe, Projet *projet)
{
    GtkTreeModel   	        *model1;
    GtkTreeIter    	        iter1;
    unsigned int            numero, niveau;
    char           	        *nom;
    Niveau_Groupe  	        *niveau_groupe;
    Groupe         	        *groupe;
    Element        	        *element, *element2;
    GList                   *list, *list_orig;
    List_Gtk_1990_Groupes   *list_gtk_1990_groupes;
    
    BUGMSG(projet, -1, gettext("Paramètre incorrect\n"));
    BUGMSG(projet->niveaux_groupes, -1, gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk._1990_groupes.builder, -1, gettext("Paramètre incorrect\n"));
    
    list_gtk_1990_groupes = &projet->list_gtk._1990_groupes;
    niveau = GTK_COMMON_SPINBUTTON_AS_UINT(GTK_SPIN_BUTTON(list_gtk_1990_groupes->spin_button_niveau));
    BUG(niveau_groupe = _1990_groupe_positionne_niveau(projet, niveau), -3);
    BUG(groupe = _1990_groupe_positionne_groupe(niveau_groupe, ngroupe), -3);
    list_orig = gtk_tree_selection_get_selected_rows(list_gtk_1990_groupes->tree_select_dispo, &model1);
    list = g_list_last(list_orig);
    
    for(;list != NULL; list = g_list_previous(list))
    {
        GtkTreePath *path;
        
        gtk_tree_model_get_iter(model1, &iter1, (GtkTreePath*)list->data);
        
        /* On récupère les informations des lignes sélectionnées */
        gtk_tree_model_get(model1, &iter1, 0, &numero, 1, &nom, -1);
        
        /* Comme on ajoute un élément disponible au groupe,
         * on supprime la ligne contenant l'élément anciennement disponible */
        gtk_tree_store_remove(list_gtk_1990_groupes->tree_store_dispo, &iter1);
        
        /* On ajoute l'élément au groupe */
        BUG(_1990_groupe_ajout_element(projet, niveau, ngroupe, numero) == 0, -3);
        
        /* On positionne l'élément en cours */
        BUG(element = _1990_groupe_positionne_element(groupe, numero), -3);
        
        /* On ajoute la ligne dans l'interface graphique */
        if (groupe->elements->data == element)
            gtk_tree_store_prepend(list_gtk_1990_groupes->tree_store_etat, &element->Iter, &groupe->Iter);
        else
        {
            element2 = g_list_previous(g_list_find(groupe->elements, element))->data;
            gtk_tree_store_insert_after(list_gtk_1990_groupes->tree_store_etat, &element->Iter, &groupe->Iter, &element2->Iter);
        }
        
        if (projet->niveaux_groupes->data == niveau_groupe)
        {
            Action *action;
            
            BUG(action = _1990_action_cherche_numero(projet, numero), -3);
            gtk_tree_store_set(list_gtk_1990_groupes->tree_store_etat, &element->Iter, 0, numero, 1, action->description, -1);
        }
        else
        {
            Niveau_Groupe *groupe_niveau_moins_1;
            Groupe *groupe_moins_1;
            
            BUG(groupe_niveau_moins_1 = _1990_groupe_positionne_niveau(projet, niveau-1), -3);
            BUG(groupe_moins_1 = _1990_groupe_positionne_groupe(groupe_niveau_moins_1, numero), -3);
            gtk_tree_store_set(list_gtk_1990_groupes->tree_store_etat, &element->Iter, 0, numero, 1, groupe_moins_1->nom, -1);
        }
       
        groupe->Iter_expand = 1;
        path = gtk_tree_model_get_path(GTK_TREE_MODEL(list_gtk_1990_groupes->tree_store_etat), &groupe->Iter);
        gtk_tree_view_expand_row(list_gtk_1990_groupes->tree_view_etat, path, FALSE);
        gtk_tree_path_free(path);
    }
    g_list_foreach(list_orig, (GFunc)gtk_tree_path_free, NULL);
    g_list_free(list_orig);
    
    return 0;
}


G_MODULE_EXPORT void _1990_gtk_button_ajout_dispo_clicked(GtkWidget *button __attribute__((unused)), Projet *projet)
/* Description : Ajoute l'élément disponible sélectionné dans le groupe sélectionné
 * Paramètres : GtkWidget *button : composant spin_button ayant réalisé l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    // Pour toutes les variables, les indices 1 désignent 'dispo' et les indices 2 désignent 'etat'
    List_Gtk_1990_Groupes   *list_gtk_1990_groupes;
    GtkTreeIter     iter2;
    GtkTreeModel    *model2;
    unsigned int    ngroupe;
    
    BUGMSG(projet, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->niveaux_groupes, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk._1990_groupes.builder, , gettext("Paramètre incorrect\n"));
    
    list_gtk_1990_groupes = &projet->list_gtk._1990_groupes;
    
    /* On vérifie s'il y a des éléments sélectionnés */
    if (!gtk_tree_selection_get_selected(list_gtk_1990_groupes->tree_select_etat, &model2, &iter2))
        return;
    ngroupe = _1990_gtk_get_groupe(model2, &iter2);
    
    BUG(_1990_gtk_button_ajout_dispo_proc(ngroupe, projet) == 0, );
    
    return;
}


G_MODULE_EXPORT void _1990_gtk_button_ajout_tout_dispo_clicked(GtkWidget *button __attribute__((unused)), Projet *projet)
/* Description : Ajoute l'élément disponible sélectionné dans le groupe sélectionné
 * Paramètres : GtkWidget *button : composant spin_button ayant réalisé l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    // Pour toutes les variables, les indices 1 désignent 'dispo' et les indices 2 désignent 'etat'
    List_Gtk_1990_Groupes   *list_gtk_1990_groupes = &projet->list_gtk._1990_groupes;
    GtkTreeIter     iter2;
    GtkTreeModel    *model2;
    unsigned int    ngroupe;
    
    BUGMSG(projet, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->niveaux_groupes, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk._1990_groupes.builder, , gettext("Paramètre incorrect\n"));
    
    gtk_tree_selection_select_all(list_gtk_1990_groupes->tree_select_dispo);
    
    /* On vérifie s'il y a des éléments sélectionnés */
    if (!gtk_tree_selection_get_selected(list_gtk_1990_groupes->tree_select_etat, &model2, &iter2))
        return;
    ngroupe = _1990_gtk_get_groupe(model2, &iter2);
    
    BUG(_1990_gtk_button_ajout_dispo_proc(ngroupe, projet) == 0, );
    
    return;
}


G_MODULE_EXPORT void _1990_gtk_tree_view_etat_drag(GtkWidget *widget __attribute__((unused)), GdkDragContext *drag_context __attribute__((unused)),
                        gint x, gint y, guint tim __attribute__((unused)), Projet *projet)
/* Description : Supprimer les éléments sélectionnées du tree_view_etat.
 * Paramètres : GtkWidget *button : composant spin_button ayant réalisé l'évènement,
 *            : GdkDragContext *drag_context : inutile,
 *            : GtkSelectionData *data : inutile,
 *            : guint info : inutile,
 *            : guint time : inutile,
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    gint                    cx, cy;
    List_Gtk_1990_Groupes   *list_gtk_1990_groupes = &projet->list_gtk._1990_groupes;
    GtkTreePath             *path;
    GtkTreeIter             iter, iter_tmp;
    GtkTreeModel            *list_store2;
    
    BUGMSG(projet, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->niveaux_groupes, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk._1990_groupes.builder, , gettext("Paramètre incorrect\n"));
    
    gdk_window_get_geometry(gtk_tree_view_get_bin_window(GTK_TREE_VIEW(list_gtk_1990_groupes->tree_view_etat)), &cx, &cy, NULL, NULL);
    gtk_tree_view_get_path_at_pos(GTK_TREE_VIEW(widget), x -=cx, y -=cy, &path, NULL, &cx, &cy);
    list_store2 = gtk_tree_view_get_model(GTK_TREE_VIEW(list_gtk_1990_groupes->tree_view_etat));
    if (path != NULL)
    {
        unsigned int    ngroupe_dest;
        
        gtk_tree_model_get_iter(list_store2, &iter, path);
        ngroupe_dest = _1990_gtk_get_groupe(list_store2, &iter);
        if (list_gtk_1990_groupes->drag_from == GTK_WIDGET(list_gtk_1990_groupes->tree_view_dispo))
            BUG(_1990_gtk_button_ajout_dispo_proc(ngroupe_dest, projet) == 0, );
        else
        {
            unsigned int    ngroupe_source;
            GtkTreeModel    *model;
            unsigned int    numero;
            unsigned int    niveau;
            char            *nom;
            Niveau_Groupe   *niveau_groupe;
            Groupe          *groupe;
            Element         *element;
            
            niveau = GTK_COMMON_SPINBUTTON_AS_UINT(GTK_SPIN_BUTTON(list_gtk_1990_groupes->spin_button_niveau));
            gtk_tree_selection_get_selected(list_gtk_1990_groupes->tree_select_etat, &model, &iter);
            if (gtk_tree_model_iter_parent(list_store2, &iter_tmp, &iter))
            {
                ngroupe_source = _1990_gtk_get_groupe(model, &iter);
                gtk_tree_model_get(model, &iter, 0, &numero, 1, &nom, -1);
                BUG(_1990_groupe_free_element(projet, niveau, ngroupe_source, numero) == 0, );
                BUG(_1990_groupe_ajout_element(projet, niveau, ngroupe_dest, numero) == 0, );
                gtk_tree_store_remove(list_gtk_1990_groupes->tree_store_etat, &iter);
                BUG(niveau_groupe = _1990_groupe_positionne_niveau(projet, niveau), );
                BUG(groupe = _1990_groupe_positionne_groupe(niveau_groupe, ngroupe_dest), );
                BUG(element = _1990_groupe_positionne_element(groupe, numero), );
                
                /* On ajoute la ligne dans l'interface graphique */
                if (groupe->elements->data == element)
                    gtk_tree_store_prepend(list_gtk_1990_groupes->tree_store_etat, &element->Iter, &groupe->Iter);
                else
                {
                    Element*    element2;
                    
                    BUG(element2 = _1990_groupe_positionne_element(groupe, numero-1), );
                    gtk_tree_store_insert_after(list_gtk_1990_groupes->tree_store_etat, &element->Iter, &groupe->Iter, &element2->Iter);
                }
                
                if (projet->niveaux_groupes->data == niveau_groupe)
                {
                    Action *action;
                    
                    BUG(action = _1990_action_cherche_numero(projet, numero), );
                    gtk_tree_store_set(list_gtk_1990_groupes->tree_store_etat, &element->Iter, 0, numero, 1, action->description, -1);
                }
                else
                {
                    Niveau_Groupe *groupe_niveau_moins_1;
                    Groupe *groupe_moins_1;
                    
                    BUG(groupe_niveau_moins_1 = _1990_groupe_positionne_niveau(projet, niveau-1), );
                    BUG(groupe_moins_1 = _1990_groupe_positionne_groupe(groupe_niveau_moins_1, numero), );
                    gtk_tree_store_set(list_gtk_1990_groupes->tree_store_etat, &element->Iter, 0, numero, 1, groupe_moins_1->nom, -1);
                }
                
                groupe->Iter_expand = 1;
                path = gtk_tree_model_get_path(GTK_TREE_MODEL(list_gtk_1990_groupes->tree_store_etat), &groupe->Iter);
                gtk_tree_view_expand_row(list_gtk_1990_groupes->tree_view_etat, path, FALSE);
                gtk_tree_path_free(path);
                path = gtk_tree_model_get_path(GTK_TREE_MODEL(list_gtk_1990_groupes->tree_store_etat), &element->Iter);
                gtk_tree_selection_select_path(list_gtk_1990_groupes->tree_select_etat, path);
                gtk_tree_path_free(path);
            }
        }
    }
    
    return;
}


G_MODULE_EXPORT void _1990_gtk_tree_view_etat_row_expanded(GtkTreeView *tree_view, GtkTreeIter *iter, __attribute__((unused)) GtkTreePath *path, Projet *projet)
/* Description : Evènement lorsqu'on ouvre une ligne contenant un enfant
 *             : Est utilisé pour se souvenir des lignes qui sont ouvertes et fermées
 * Paramètres : GtkTreeView *tree_view : composant tree_view à l'origine de l'évènement
 *            : GtkTreeIter *iter : ligne s'"ouvrant"
 *            : GtkTreePath *path : chemin vers la ligne
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    List_Gtk_1990_Groupes   *list_gtk_1990_groupes;
    GtkTreeModel    *model = gtk_tree_view_get_model(tree_view);
    unsigned int    ngroupe;
    Niveau_Groupe   *niveau_groupe;
    Groupe          *groupe;
    
    BUGMSG(projet, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->niveaux_groupes, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk._1990_groupes.builder, , gettext("Paramètre incorrect\n"));
    
    list_gtk_1990_groupes = &projet->list_gtk._1990_groupes;
    
    // On détermine le groupe ayant entraîné une ouverture
    // En effet, seul un groupe dans le tree_view_etat peut entraîner une ouverture
    ngroupe = _1990_gtk_get_groupe(model, iter);
    BUG(niveau_groupe = _1990_groupe_positionne_niveau(projet, GTK_COMMON_SPINBUTTON_AS_UINT(GTK_SPIN_BUTTON(list_gtk_1990_groupes->spin_button_niveau))), );
    BUG(groupe = _1990_groupe_positionne_groupe(niveau_groupe, ngroupe), );
    
    // On le marque comme ouvert.
    groupe->Iter_expand = 1;
    
    return;
}


/* _1990_gtk_tree_view_etat_row_collapsed
 * Description : Evènement lorsqu'on ferme une ligne contenant un enfant
 *             : Est utilisé pour se souvenir des lignes qui sont ouvertes et fermées
 * Paramètres : GtkTreeView *tree_view : composant tree_view à l'origine de l'évènement
 *            : GtkTreeIter *iter : ligne se "fermant"
 *            : GtkTreePath *path : chemin vers la ligne
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
G_MODULE_EXPORT void _1990_gtk_tree_view_etat_row_collapsed(GtkTreeView *tree_view, GtkTreeIter *iter, __attribute__((unused)) GtkTreePath *path, Projet *projet)
{
    List_Gtk_1990_Groupes   *list_gtk_1990_groupes;
    GtkTreeModel    *model = gtk_tree_view_get_model(tree_view);
    unsigned int    ngroupe;
    Niveau_Groupe   *niveau_groupe;
    Groupe          *groupe;
    
    BUGMSG(projet, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->niveaux_groupes, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk._1990_groupes.builder, , gettext("Paramètre incorrect\n"));
    
    list_gtk_1990_groupes = &projet->list_gtk._1990_groupes;
    
    // On détermine le groupe ayant entraîné une fermeture
    // En effet, seul un groupe dans le tree_view_etat peut entraîner une fermeture
    ngroupe = _1990_gtk_get_groupe(model, iter);
    BUG(niveau_groupe = _1990_groupe_positionne_niveau(projet, GTK_COMMON_SPINBUTTON_AS_UINT(GTK_SPIN_BUTTON(list_gtk_1990_groupes->spin_button_niveau))), );
    BUG(groupe = _1990_groupe_positionne_groupe(niveau_groupe, ngroupe), );
    
    // On le marque comme fermé.
    groupe->Iter_expand = 0;
    
    return;
}


G_MODULE_EXPORT void _1990_gtk_button_groupe_toggled(GtkRadioToolButton *radiobutton, Projet *projet)
/* Description : Evènement lorsqu'il y a un changement de type de combinaison (OR, XOR ou AND)
 * Paramètres : GtkRadioButton *radiobutton : composant radiobutton à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    List_Gtk_1990_Groupes    *list_gtk_1990_groupes;
    GtkTreeModel     *model;
    GtkTreeIter      iter;
    unsigned int     ngroupe;
    Niveau_Groupe    *niveau_groupe;
    Groupe           *groupe;
    
    BUGMSG(projet, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->niveaux_groupes, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk._1990_groupes.builder, , gettext("Paramètre incorrect\n"));
    
    list_gtk_1990_groupes = &projet->list_gtk._1990_groupes;
    
    /* On détermine le groupe sélectionné */
    if (!gtk_tree_selection_get_selected(list_gtk_1990_groupes->tree_select_etat, &model, &iter))
        return;
    ngroupe = _1990_gtk_get_groupe(model, &iter);
    BUG(niveau_groupe = _1990_groupe_positionne_niveau(projet, GTK_COMMON_SPINBUTTON_AS_UINT(GTK_SPIN_BUTTON(list_gtk_1990_groupes->spin_button_niveau))), );
    BUG(groupe = _1990_groupe_positionne_groupe(niveau_groupe, ngroupe), );
    
    /* On attribue le nouveau type de combinaison */
    if (radiobutton == (void*)list_gtk_1990_groupes->item_groupe_and)
        groupe->type_combinaison = GROUPE_COMBINAISON_AND;
    else if (radiobutton == (void*)list_gtk_1990_groupes->item_groupe_or)
        groupe->type_combinaison = GROUPE_COMBINAISON_OR;
    else if (radiobutton == (void*)list_gtk_1990_groupes->item_groupe_xor)
        groupe->type_combinaison = GROUPE_COMBINAISON_XOR;
    
    return;
}


G_MODULE_EXPORT void _1990_gtk_groupes_button_generer_clicked(GtkWidget *button __attribute__((unused)), Projet *projet)
/* Description : Génère les combinaisons
 * Paramètres : GtkWidget *button : composant bouton à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    BUGMSG(projet, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk._1990_groupes.builder, , gettext("Paramètre incorrect\n"));
    
    BUG(_1990_combinaisons_genere(projet) == 0, );
    BUG(_1990_groupe_affiche_tout(projet) == 0, );
    BUG(_1990_ponderations_affiche_tout(projet) == 0, );
    return;
}


G_MODULE_EXPORT void _1990_gtk_radio_button_eluequ_equ_seul(GtkRadioButton *radiobutton __attribute__((unused)), Projet *projet)
/* Description : Modifie les options de combinaison pour que l'ELU EQU calcule à l'équilibre seulement
 * Paramètres : GtkRadioButton *radiobutton : composant radiobutton à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    BUGMSG(projet, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk._1990_groupes.builder, , gettext("Paramètre incorrect\n"));
    
    if ((projet->combinaisons.flags & 1) != 0)
        projet->combinaisons.flags ^= 1;
    
    return;
}


G_MODULE_EXPORT void _1990_gtk_radio_button_eluequ_equ_resist(GtkRadioButton *radiobutton __attribute__((unused)), Projet *projet)
/* Description : Modifie les options de combinaison pour que l'ELU EQU calcule à l'équilibre et à la résistance structurelle
 * Paramètres : GtkRadioButton *radiobutton : composant radiobutton à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    BUGMSG(projet, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk._1990_groupes.builder, , gettext("Paramètre incorrect\n"));
    
    if ((projet->combinaisons.flags & 1) == 0)
        projet->combinaisons.flags++;
    
    return;
}


G_MODULE_EXPORT void _1990_gtk_radio_button_elustrgeo_1(GtkRadioButton *radiobutton __attribute__((unused)), Projet *projet)
/* Description : Modifie les options de combinaison pour que l'ELU STR/GEO calcule selon l'approche 1
 * Paramètres : GtkRadioButton *radiobutton : composant radiobutton à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    BUGMSG(projet, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk._1990_groupes.builder, , gettext("Paramètre incorrect\n"));
    
    projet->combinaisons.flags = projet->combinaisons.flags - (projet->combinaisons.flags & 6);
    projet->combinaisons.flags = projet->combinaisons.flags + 0;
    
    return;
}


G_MODULE_EXPORT void _1990_gtk_radio_button_elustrgeo_2(GtkRadioButton *radiobutton __attribute__((unused)), Projet *projet)
/* Description : Modifie les options de combinaison pour que l'ELU STR/GEO calcule selon l'approche 2
 * Paramètres : GtkRadioButton *radiobutton : composant radiobutton à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    BUGMSG(projet, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk._1990_groupes.builder, , gettext("Paramètre incorrect\n"));
    
    projet->combinaisons.flags = projet->combinaisons.flags - (projet->combinaisons.flags & 6);
    projet->combinaisons.flags = projet->combinaisons.flags + 2;
    
    return;
}


G_MODULE_EXPORT void _1990_gtk_radio_button_elustrgeo_3(GtkRadioButton *radiobutton __attribute__((unused)), Projet *projet)
/* Description : Modifie les options de combinaison pour que l'ELU STR/GEO calcule selon l'approche 3
 * Paramètres : GtkRadioButton *radiobutton : composant radiobutton à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    BUGMSG(projet, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk._1990_groupes.builder, , gettext("Paramètre incorrect\n"));
    
    projet->combinaisons.flags = projet->combinaisons.flags - (projet->combinaisons.flags & 6);
    projet->combinaisons.flags = projet->combinaisons.flags + 4;
    
    return;
}


G_MODULE_EXPORT void _1990_gtk_radio_button_elustrgeo_6_10(GtkRadioButton *radiobutton __attribute__((unused)), Projet *projet)
/* Description : Modifie les options de combinaison pour que l'ELU STR/GEO calcule selon la formule 6.10 de l'EN 1990
 * Paramètres : GtkRadioButton *radiobutton : composant radiobutton à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    BUGMSG(projet, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk._1990_groupes.builder, , gettext("Paramètre incorrect\n"));
    
    projet->combinaisons.flags = projet->combinaisons.flags - (projet->combinaisons.flags & 8);
    projet->combinaisons.flags = projet->combinaisons.flags + 8;
    
    return;
}


G_MODULE_EXPORT void _1990_gtk_radio_button_elustrgeo_6_10ab(GtkRadioButton *radiobutton __attribute__((unused)), Projet *projet)
/* Description : Modifie les options de combinaison pour que l'ELU STR/GEO calcule selon la formule 6.10 (a) et (b) de l'EN 1990
 * Paramètres : GtkRadioButton *radiobutton : composant radiobutton à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    BUGMSG(projet, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk._1990_groupes.builder, , gettext("Paramètre incorrect\n"));
    
    projet->combinaisons.flags = projet->combinaisons.flags - (projet->combinaisons.flags & 8);
    projet->combinaisons.flags = projet->combinaisons.flags + 0; 
    
    return;
}


G_MODULE_EXPORT void _1990_gtk_radio_button_eluacc_frequente(GtkRadioButton *radiobutton __attribute__((unused)), Projet *projet)
/* Description : Modifie les options de combinaison pour que l'ELU ACC calcule avec les valeurs fréquentes des actions variables
 * Paramètres : GtkRadioButton *radiobutton : composant radiobutton à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    BUGMSG(projet, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk._1990_groupes.builder, , gettext("Paramètre incorrect\n"));
    
    projet->combinaisons.flags = projet->combinaisons.flags - (projet->combinaisons.flags & 16);
    projet->combinaisons.flags = projet->combinaisons.flags + 0;
    
    return;
}


G_MODULE_EXPORT void _1990_gtk_radio_button_eluacc_quasi_permanente(GtkRadioButton *radiobutton __attribute__((unused)), Projet *projet)
/* Description : Modifie les options de combinaison pour que l'ELU ACC calcule avec les valeurs quasi_permanente des actions variables
 * Paramètres : GtkRadioButton *radiobutton : composant radiobutton à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    BUGMSG(projet, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk._1990_groupes.builder, , gettext("Paramètre incorrect\n"));
    
    projet->combinaisons.flags = projet->combinaisons.flags - (projet->combinaisons.flags & 16);
    projet->combinaisons.flags = projet->combinaisons.flags + 16;
    
    return;
}


G_MODULE_EXPORT void _1990_gtk_tooltip(GtkWidget *widget __attribute__((unused)), gint x __attribute__((unused)), gint y __attribute__((unused)), gboolean keyboard_mode __attribute__((unused)), GtkTooltip *tooltip __attribute__((unused)), gpointer user_data __attribute__((unused)))
/* Description : Cette fonction doit obligatoirement être relié à l'évènement "query-tooltip" pour qu'apparaisse la fenêtre tooltip
 * Paramètres : GtkWidget *widget : composant à l'origine de l'évènement
 *            : gint x, gint y, gboolean keyboard_mode, GtkTooltip *tooltip : paramètre de l'évènement query-tooltip
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    return;
}


G_MODULE_EXPORT void _1990_gtk_groupes_window_quitter_button(GtkWidget *object __attribute__((unused)), Projet *projet)
/* Description : Bouton de fermeture de la fenêtre
 * Paramètres : GtkComboBox *widget : composant à l'origine de la demande
 *            : GtkWidget *fenêtre : la fenêtre d'options
 * Valeur renvoyée : Aucune
 */
{
    BUGMSG(projet->list_gtk._1990_groupes.window_groupe, , gettext("Paramètre incorrect\n"));
    
    gtk_widget_destroy(projet->list_gtk._1990_groupes.window_groupe);
    
    return;
}


G_MODULE_EXPORT void _1990_gtk_groupes_options_window_destroy(GtkWidget *object __attribute__((unused)), Projet *projet)
/* Description : met projet->list_gtk._1990_groupes.window à NULL quand la fenêtre se ferme
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    BUGMSG(projet, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk._1990_groupes.builder_options, , gettext("Paramètre incorrect\n"));
    
    projet->list_gtk._1990_groupes.builder_options = NULL;
    return;
}


G_MODULE_EXPORT void _1990_gtk_groupes_option_window_quitter_button(GtkWidget *object __attribute__((unused)), Projet *projet)
/* Description : Bouton de fermeture de la fenêtre
 * Paramètres : GtkComboBox *widget : composant à l'origine de la demande
 *            : GtkWidget *fenêtre : la fenêtre d'options
 * Valeur renvoyée : Aucune
 */
{
    BUGMSG(projet, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk._1990_groupes.builder_options, , gettext("Paramètre incorrect\n"));
    
    gtk_widget_destroy(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk._1990_groupes.builder_options, "1990_groupes_options_window")));
    
    return;
}


G_MODULE_EXPORT void _1990_gtk_groupes_button_options_clicked(GtkWidget *button __attribute__((unused)), Projet *projet)
{
    GtkSettings *settings;
    
    BUGMSG(projet, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk._1990_groupes.builder, , gettext("Paramètre incorrect\n"));
    
    projet->list_gtk._1990_groupes.builder_options = gtk_builder_new();
    BUGMSG(gtk_builder_add_from_file(projet->list_gtk._1990_groupes.builder_options, DATADIR"/ui/1990_gtk_groupes_options.ui", NULL) != 0, , gettext("Builder Failed\n"));
    gtk_builder_connect_signals(projet->list_gtk._1990_groupes.builder_options, projet);
    
    if ((projet->combinaisons.flags & 1) == 0)
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(projet->list_gtk._1990_groupes.builder_options, "1990_groupes_options_radio_button_EQU")), TRUE);
    else
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(projet->list_gtk._1990_groupes.builder_options, "1990_groupes_options_radio_button_EQU_RES")), TRUE);
    
    if ((projet->combinaisons.flags & 8) == 0)
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(projet->list_gtk._1990_groupes.builder_options, "1990_groupes_options_radio_button_6_10a_b")), TRUE);
    else
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(projet->list_gtk._1990_groupes.builder_options, "1990_groupes_options_radio_button_6_10")), TRUE);
    gtk_widget_set_tooltip_window(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk._1990_groupes.builder_options, "1990_groupes_options_radio_button_6_10")), GTK_WINDOW(common_tooltip_generation("1990_6_10")));
    gtk_widget_set_tooltip_window(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk._1990_groupes.builder_options, "1990_groupes_options_radio_button_6_10a_b")), GTK_WINDOW(common_tooltip_generation("1990_6_10a_b")));
    settings = gtk_widget_get_settings(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk._1990_groupes.builder_options, "1990_groupes_options_radio_button_6_10")));
    g_object_set(settings, "gtk-tooltip-timeout", 0, NULL);
    settings = gtk_widget_get_settings(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk._1990_groupes.builder_options, "1990_groupes_options_radio_button_6_10a_b")));
    g_object_set(settings, "gtk-tooltip-timeout", 0, NULL);
    
    if ((projet->combinaisons.flags & 6) == 4)
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(projet->list_gtk._1990_groupes.builder_options, "1990_groupes_options_radio_button_appr3")), TRUE);
    else if ((projet->combinaisons.flags & 6) == 2)
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(projet->list_gtk._1990_groupes.builder_options, "1990_groupes_options_radio_button_appr2")), TRUE);
    else
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(projet->list_gtk._1990_groupes.builder_options, "1990_groupes_options_radio_button_appr1")), TRUE);
    
    if ((projet->combinaisons.flags & 16) == 0)
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(projet->list_gtk._1990_groupes.builder_options, "1990_groupes_options_radio_button_freq")), TRUE);
    else
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(projet->list_gtk._1990_groupes.builder_options, "1990_groupes_options_radio_button_quasi_perm")), TRUE);
    
    gtk_window_set_transient_for(GTK_WINDOW(gtk_builder_get_object(projet->list_gtk._1990_groupes.builder_options, "1990_groupes_options_window")), GTK_WINDOW(gtk_builder_get_object(projet->list_gtk._1990_groupes.builder, "1990_groupes_options_window")));
    
    return;
}


G_MODULE_EXPORT void _1990_gtk_tree_view_etat_cell_edited(GtkCellRendererText *cell __attribute__((unused)), gchar *path_string, gchar *new_text, Projet *projet)
/* Description : Edition du nom dans le tree-view-etat. On édite donc soit le nom d'un niveau, soit le nom d'une action.
 * Paramètres : GtkCellRendererText *cell : composant à l'origine de l'évènement,
 *            : gchar *path_string : ligne en cours d'édition,
 *            : gchar *new_text : nouveau nom,
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    List_Gtk_1990_Groupes *list_gtk_1990_groupes;
    GtkTreePath   *path;
    GtkTreeIter   iter;
    GtkTreeModel  *model;
    GValue        nouvelle_valeur;
    unsigned int  numero, niveau;
    GtkTreeIter   iter_parent;
    
    BUGMSG(projet, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk._1990_groupes.builder, , gettext("Paramètre incorrect\n"));
    
    list_gtk_1990_groupes = &projet->list_gtk._1990_groupes;
    path = gtk_tree_path_new_from_string(path_string);
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(list_gtk_1990_groupes->tree_view_etat));
    gtk_tree_model_get_iter (model, &iter, path);
    gtk_tree_model_get(model, &iter, 0, &numero, -1);
    
    niveau = GTK_COMMON_SPINBUTTON_AS_UINT(GTK_SPIN_BUTTON(list_gtk_1990_groupes->spin_button_niveau));
    // On prend le niveau correspondant à niveau
    if (!gtk_tree_model_iter_parent(model, &iter_parent, &iter))
    {
        Niveau_Groupe *groupe_niveau;
        Groupe        *groupe;
        
        BUG(groupe_niveau = _1990_groupe_positionne_niveau(projet, niveau), );
        BUG(groupe = _1990_groupe_positionne_groupe(groupe_niveau, numero), );
        free(groupe->nom);
        BUGMSG(groupe->nom = g_strdup_printf("%s", new_text), , gettext("Erreur d'allocation mémoire.\n"));
    }
    else // On prend niveau-1
    {
        // Le nom est celui d'une action
        if (niveau == 0)
        {
            Action      *action;
            
            BUG(action = _1990_action_cherche_numero(projet, numero), );
            free(action->description);
            BUGMSG(action->description = g_strdup_printf("%s", new_text), , gettext("Erreur d'allocation mémoire.\n"));
        }
        else // Le nom est celui d'un groupe du niveau n-1
        {
            Niveau_Groupe *groupe_niveau;
            Groupe        *groupe;
            BUG(groupe_niveau = _1990_groupe_positionne_niveau(projet, niveau-1), );
            BUG(groupe = _1990_groupe_positionne_groupe(groupe_niveau, numero), );
            free(groupe->nom);
            BUGMSG(groupe->nom = g_strdup_printf("%s", new_text), , gettext("Erreur d'allocation mémoire.\n"));
        }
    }
    
    memset(&nouvelle_valeur, 0, sizeof(nouvelle_valeur));
    g_value_init (&nouvelle_valeur, G_TYPE_STRING);
    g_value_set_string (&nouvelle_valeur, new_text);
    gtk_tree_store_set_value(list_gtk_1990_groupes->tree_store_etat, &iter, 1, &nouvelle_valeur);
    
    gtk_tree_path_free (path);
}


G_MODULE_EXPORT gboolean _1990_gtk_tree_view_etat_key_press_event(GtkWidget *widget __attribute__((unused)), GdkEvent *event, Projet *projet)
{
    BUGMSG(projet, FALSE, gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk._1990_groupes.builder, FALSE, gettext("Paramètre incorrect\n"));
    
    if (event->key.keyval == GDK_KEY_Delete)
        _1990_gtk_button_groupe_suppr_clicked(widget, projet);
    return FALSE; /* Pour permettre aux autres touches d'être fonctionnelles  */
}


G_MODULE_EXPORT void _1990_gtk_tree_select_changed(GtkTreeSelection *treeselection __attribute__((unused)), Projet *projet)
/* Description : Permet de activer/désactiver les boutons ajout en fonction de la selection
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    List_Gtk_1990_Groupes   *list_gtk_1990_groupes = &projet->list_gtk._1990_groupes;
    GtkTreeIter             iter;
    
    BUGMSG(projet, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk._1990_groupes.builder, , gettext("Paramètre incorrect\n"));
    
    // Si le tree_view est vide ou qu'aucun groupe n'est sélectionné
    if ( (!gtk_tree_model_get_iter_first(GTK_TREE_MODEL(list_gtk_1990_groupes->tree_store_dispo), &iter)) ||
         (!gtk_tree_selection_get_selected(list_gtk_1990_groupes->tree_select_etat, NULL, NULL)) )
        gtk_widget_set_sensitive(GTK_WIDGET(list_gtk_1990_groupes->item_ajout_tout_dispo), FALSE);
    else
        gtk_widget_set_sensitive(GTK_WIDGET(list_gtk_1990_groupes->item_ajout_tout_dispo), TRUE);
    
    // Si aucun groupe n'est sélectionné
    if (!gtk_tree_selection_get_selected(list_gtk_1990_groupes->tree_select_etat, NULL, NULL))
    {
        gtk_widget_set_sensitive(GTK_WIDGET(list_gtk_1990_groupes->item_ajout_dispo), FALSE);
        gtk_widget_set_sensitive(GTK_WIDGET(list_gtk_1990_groupes->item_groupe_suppr), FALSE);
        gtk_widget_set_sensitive(GTK_WIDGET(list_gtk_1990_groupes->item_groupe_and), FALSE);
        gtk_widget_set_sensitive(GTK_WIDGET(list_gtk_1990_groupes->item_groupe_or), FALSE);
        gtk_widget_set_sensitive(GTK_WIDGET(list_gtk_1990_groupes->item_groupe_xor), FALSE);
    }
    else
    {
        GList   *list_orig;
        GList   *list;
        gtk_widget_set_sensitive(GTK_WIDGET(list_gtk_1990_groupes->item_groupe_suppr), TRUE);
        gtk_widget_set_sensitive(GTK_WIDGET(list_gtk_1990_groupes->item_groupe_and), TRUE);
        gtk_widget_set_sensitive(GTK_WIDGET(list_gtk_1990_groupes->item_groupe_or), TRUE);
        gtk_widget_set_sensitive(GTK_WIDGET(list_gtk_1990_groupes->item_groupe_xor), TRUE);
        
        // Si aucune sélection dans la liste des éléments disponibles
        list_orig = gtk_tree_selection_get_selected_rows(list_gtk_1990_groupes->tree_select_dispo, NULL);
        list = g_list_last(list_orig);
        
        if (list == NULL)
            gtk_widget_set_sensitive(GTK_WIDGET(list_gtk_1990_groupes->item_ajout_dispo), FALSE);
        else
            gtk_widget_set_sensitive(GTK_WIDGET(list_gtk_1990_groupes->item_ajout_dispo), TRUE);
        
        g_list_foreach(list_orig, (GFunc)gtk_tree_path_free, NULL);
        g_list_free(list_orig);
    }
}


G_MODULE_EXPORT void _1990_gtk_groupes_window_destroy(GtkWidget *object __attribute__((unused)), Projet *projet)
/* Description : met projet->list_gtk._1990_groupes.window à NULL quand la fenêtre se ferme
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    BUGMSG(projet, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk._1990_groupes.builder, , gettext("Paramètre incorrect\n"));
    
    g_signal_handler_block(projet->list_gtk._1990_groupes.tree_view_etat, g_signal_handler_find(G_OBJECT(projet->list_gtk._1990_groupes.tree_view_etat),G_SIGNAL_MATCH_FUNC,0,0,NULL,_1990_gtk_tree_view_etat_cursor_changed,NULL));
    projet->list_gtk._1990_groupes.builder = NULL;
    return;
}


G_MODULE_EXPORT void _1990_gtk_groupes(GtkWidget *button __attribute__((unused)), Projet *projet)
{
    BUGMSG(projet, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk._1990_groupes.builder == NULL, , gettext("Paramètre incorrect\n"));
    
    if (projet->niveaux_groupes == NULL)
        BUG(_1990_groupe_ajout_niveau(projet) == 0, );
    
    projet->list_gtk._1990_groupes.builder = gtk_builder_new();
    BUGMSG(gtk_builder_add_from_file(projet->list_gtk._1990_groupes.builder, DATADIR"/ui/1990_gtk_groupes.ui", NULL) != 0, , gettext("Builder Failed\n"));
    gtk_builder_connect_signals(projet->list_gtk._1990_groupes.builder, projet);
    
    projet->list_gtk._1990_groupes.window_groupe = GTK_WIDGET(gtk_builder_get_object(projet->list_gtk._1990_groupes.builder, "1990_groupes_window"));
    projet->list_gtk._1990_groupes.tree_store_etat = GTK_TREE_STORE(gtk_builder_get_object(projet->list_gtk._1990_groupes.builder, "1990_actions_tree_store_etat"));
    projet->list_gtk._1990_groupes.tree_store_dispo = GTK_TREE_STORE(gtk_builder_get_object(projet->list_gtk._1990_groupes.builder, "1990_actions_tree_store_dispo"));
    projet->list_gtk._1990_groupes.tree_select_etat = GTK_TREE_SELECTION(gtk_builder_get_object(projet->list_gtk._1990_groupes.builder, "1990_groupes_treeview_select_etat"));
    projet->list_gtk._1990_groupes.tree_view_etat = GTK_TREE_VIEW(gtk_builder_get_object(projet->list_gtk._1990_groupes.builder, "1990_groupes_treeview_etat"));
    projet->list_gtk._1990_groupes.tree_select_dispo = GTK_TREE_SELECTION(gtk_builder_get_object(projet->list_gtk._1990_groupes.builder, "1990_groupes_treeview_select_dispo"));
    projet->list_gtk._1990_groupes.spin_button_niveau = GTK_WIDGET(gtk_builder_get_object(projet->list_gtk._1990_groupes.builder, "1990_groupes_spin_button_niveaux"));
    projet->list_gtk._1990_groupes.tree_view_dispo = GTK_TREE_VIEW(gtk_builder_get_object(projet->list_gtk._1990_groupes.builder, "1990_groupes_treeview_dispo"));
    
    projet->list_gtk._1990_groupes.item_groupe_and = GTK_TOOL_ITEM(gtk_builder_get_object(projet->list_gtk._1990_groupes.builder, "1990_charges_toolbar_etat_and"));
    projet->list_gtk._1990_groupes.item_groupe_or = GTK_TOOL_ITEM(gtk_builder_get_object(projet->list_gtk._1990_groupes.builder, "1990_charges_toolbar_etat_or"));
    projet->list_gtk._1990_groupes.item_groupe_xor = GTK_TOOL_ITEM(gtk_builder_get_object(projet->list_gtk._1990_groupes.builder, "1990_charges_toolbar_etat_xor"));
    projet->list_gtk._1990_groupes.item_groupe_suppr = GTK_TOOL_ITEM(gtk_builder_get_object(projet->list_gtk._1990_groupes.builder, "1990_charges_toolbar_etat_suppr"));
    projet->list_gtk._1990_groupes.item_ajout_dispo = GTK_TOOL_ITEM(gtk_builder_get_object(projet->list_gtk._1990_groupes.builder, "1990_charges_toolbar_dispo_ajout"));
    projet->list_gtk._1990_groupes.item_ajout_tout_dispo = GTK_TOOL_ITEM(gtk_builder_get_object(projet->list_gtk._1990_groupes.builder, "1990_charges_toolbar_dispo_ajout_tout"));
    
    gtk_drag_source_set(GTK_WIDGET(projet->list_gtk._1990_groupes.tree_view_etat), GDK_BUTTON1_MASK, drag_targets_groupes_1, 1, GDK_ACTION_MOVE); 
    gtk_drag_dest_set(GTK_WIDGET(projet->list_gtk._1990_groupes.tree_view_dispo), GTK_DEST_DEFAULT_ALL, drag_targets_groupes_1, 1, GDK_ACTION_MOVE);
    gtk_drag_source_set(GTK_WIDGET(projet->list_gtk._1990_groupes.tree_view_dispo), GDK_BUTTON1_MASK, drag_targets_groupes_2, 1, GDK_ACTION_MOVE); 
    gtk_drag_dest_set(GTK_WIDGET(projet->list_gtk._1990_groupes.tree_view_etat), GTK_DEST_DEFAULT_ALL, drag_targets_groupes_3, 2, GDK_ACTION_MOVE);
    
    gtk_adjustment_set_upper(gtk_spin_button_get_adjustment(GTK_SPIN_BUTTON(gtk_builder_get_object(projet->list_gtk._1990_groupes.builder, "1990_groupes_spin_button_niveaux"))), g_list_length(projet->niveaux_groupes)-1);
    
    BUG(_1990_gtk_affiche_niveau(projet, GTK_COMMON_SPINBUTTON_AS_UINT(GTK_SPIN_BUTTON(gtk_builder_get_object(projet->list_gtk._1990_groupes.builder, "1990_groupes_spin_button_niveaux")))) == 0, );
    gtk_window_set_transient_for(GTK_WINDOW(projet->list_gtk._1990_groupes.window_groupe), GTK_WINDOW(projet->list_gtk.comp.window));
    
    
    
    return;
}

#endif
