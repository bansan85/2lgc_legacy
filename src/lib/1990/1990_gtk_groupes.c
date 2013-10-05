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
#include <math.h>

#include "1990_action.h"
#include "1990_groupe.h"
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
 * Paramètres : GtkTreeModel *tree_model : tree_model contenant la liste des éléments,
 *            : GtkTreeIter *iter : contient la ligne à étudier.
 * Valeur renvoyée : le numéro du groupe.
 */
{
    unsigned int    numero;
    GtkTreeIter     iter_parent;
    
    if (gtk_tree_model_iter_parent(tree_model, &iter_parent, iter))
        gtk_tree_model_get(tree_model, &iter_parent, 0, &numero, -1);
    else
        gtk_tree_model_get(tree_model, iter, 0, &numero, -1);
    
    return numero;
}


void _1990_gtk_groupes_tree_view_etat_cursor_changed(GtkTreeView *tree_view, Projet *projet)
/* Description : Evènement lorsqu'il y a un changement de ligne sélectionnée.
 *             : Permet de mettre à jour si le groupe sélectionné est de type OR, XOR ou AND.
 * Paramètres : GtkTreeView *tree_view : composant tree_view à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 * Echec : projet == NULL,
 *         Aucun niveau de groupes,
 *         interface graphique non initialisée,
 *         _1990_groupe_positionne_niveau,
 *         _1990_groupe_positionne_groupe.
 */
{
    GtkTreeModel    *model;
    GtkTreeIter     iter;
    unsigned int    ngroupe;
    Niveau_Groupe   *niveau_groupe;
    Groupe          *groupe;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet")
    BUGMSG(projet->niveaux_groupes, , gettext("Le projet ne possède pas de niveaux de groupes.\n"))
    BUGMSG(projet->list_gtk._1990_groupes.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Groupes")
    
    /* Détermine le groupe sélectionné */
    // Si aucune sélection.
    if (!gtk_tree_selection_get_selected(projet->list_gtk._1990_groupes.tree_select_etat, &model, &iter))
        return;
    
    ngroupe = _1990_gtk_get_groupe(model, &iter);
    BUG(niveau_groupe = _1990_groupe_positionne_niveau(projet, GTK_COMMON_SPINBUTTON_AS_UINT(GTK_SPIN_BUTTON(projet->list_gtk._1990_groupes.spin_button_niveau))), )
    BUG(groupe = _1990_groupe_positionne_groupe(niveau_groupe, ngroupe), )
    
    /* On active le toggle_button correspondant au type de combinaison du groupe */
    switch (groupe->type_combinaison)
    {
        case GROUPE_COMBINAISON_OR :
        {
            gtk_toggle_tool_button_set_active(GTK_TOGGLE_TOOL_BUTTON(projet->list_gtk._1990_groupes.item_groupe_or), TRUE);
            break;
        }
        case GROUPE_COMBINAISON_XOR :
        {
            gtk_toggle_tool_button_set_active(GTK_TOGGLE_TOOL_BUTTON(projet->list_gtk._1990_groupes.item_groupe_xor), TRUE);
            break;
        }
        case GROUPE_COMBINAISON_AND :
        {
            gtk_toggle_tool_button_set_active(GTK_TOGGLE_TOOL_BUTTON(projet->list_gtk._1990_groupes.item_groupe_and), TRUE);
            break;
        }
        default :
        {
            BUGMSG(0, , gettext("Le groupe %s (%d) n'est combiné ni de type OR, XOR ou AND.\n"), groupe->nom, groupe->numero)
            break;
        }
    }
    
    return;
}


gboolean _1990_gtk_groupe_window_key_press(GtkWidget *widget, GdkEvent *event, Projet *projet)
/* Description : Gestion des touches de l'ensemble des composants de la fenêtre.
 * Paramètres : GtkWidget *widget : composant à l'origine de l'évènement,
 *            : GdkEvent *event : description de la touche pressée,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : TRUE si la touche ECHAP est pressée, FALSE sinon.
 * Echec : FALSE :
 *           projet == NULL,
 *           interface graphique non initialisée.
 */
{
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet")
    BUGMSG(projet->list_gtk._1990_groupes.builder, FALSE, gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Groupes")
    
    if (event->key.keyval == GDK_KEY_Escape)
    {
        gtk_widget_destroy(projet->list_gtk._1990_groupes.window_groupe);
        return TRUE;
    }
    else
        return FALSE;
}


gboolean _1990_gtk_groupes_affiche_niveau(Projet *projet, unsigned int niveau)
/* Description : Affiche le niveau souhaité dans l'interface graphique.
 * Paramètres : Projet *projet : variable projet,
 *            : unsigned int niveau : niveau à afficher.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE : projet == NULL,
 *                interface graphique non initialisée,
 *                aucun niveau de groupes,
 *                erreur d'allocation mémoire,
 *                _1990_groupe_positionne_niveau,
 *                _1990_action_numero_cherche,
 *                _1990_groupe_positionne_groupe.
 */
{
    Niveau_Groupe       *niveau_groupe;
    unsigned int        dispo_max, i;
    char                *dispos;
    gboolean            premier = TRUE;
    Gtk_1990_Groupes    *gtk_1990_groupes;
    GtkTreePath         *path;
    GList               *list_parcours;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet")
    BUGMSG(projet->niveaux_groupes, FALSE, gettext("Le projet ne possède pas de niveaux de groupes.\n"))
    BUGMSG(projet->list_gtk._1990_groupes.builder, FALSE, gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Groupes")
    
    gtk_1990_groupes = &projet->list_gtk._1990_groupes;
    
    // Il convient de bloquer le signal. Sinon, des erreurs peuvent apparaitre si une ligne a été sélectionnée (par exemple la première) à la souris et qu'on demande ensuite de changer de niveau via le spin_button.
    g_signal_handler_block(gtk_1990_groupes->tree_view_etat, g_signal_handler_find(G_OBJECT(gtk_1990_groupes->tree_view_etat),G_SIGNAL_MATCH_FUNC,0,0,NULL,_1990_gtk_groupes_tree_view_etat_cursor_changed,NULL));
    /* On supprime le contenu des deux composants tree_view */
    gtk_tree_store_clear(gtk_1990_groupes->tree_store_etat);
    g_signal_handler_unblock(gtk_1990_groupes->tree_view_etat, g_signal_handler_find(G_OBJECT(gtk_1990_groupes->tree_view_etat),G_SIGNAL_MATCH_FUNC,0,0,NULL,_1990_gtk_groupes_tree_view_etat_cursor_changed,NULL));
    gtk_tree_store_clear(gtk_1990_groupes->tree_store_dispo);
    
    /* dispo_max contient le nombre d'éléments maximum pouvant être disponible depuis le 
     * niveau 'niveau'-1 */
    if (niveau == 0)
        dispo_max = g_list_length(projet->actions);
    else
    {
        BUG(niveau_groupe = _1990_groupe_positionne_niveau(projet, niveau-1), FALSE)
        BUGMSG(niveau_groupe->groupes, FALSE, gettext("Le niveau de groupes %d ne possède pas de groupe.\n"), niveau_groupe->numero)
        dispo_max = g_list_length(niveau_groupe->groupes);
    }
    
    /* Ensuite, on initialise un tableau contenant une liste de boolean
     * pour déterminer au fur et à mesure de l'avancement de l'algorithme
     * quelques sont les éléments du niveau n-1 encore non placés */
    if (dispo_max != 0)
    {
        BUGMSG(dispos = (char*)malloc(sizeof(char)*dispo_max), FALSE, gettext("Erreur d'allocation mémoire.\n"))
        for (i=0;i<dispo_max;i++)
            dispos[i] = 0;
    }
    else
        dispos = NULL;
    
    BUG(niveau_groupe = _1990_groupe_positionne_niveau(projet, niveau), FALSE)
    list_parcours = niveau_groupe->groupes;
    
    /* Parcours le niveau à afficher */
    while (list_parcours != NULL)
    {
        Groupe      *groupe = list_parcours->data;
        
        /* Ajoute de la ligne dans le tree_store */
        gtk_tree_store_append(gtk_1990_groupes->tree_store_etat, &groupe->Iter, NULL);
        gtk_tree_store_set(gtk_1990_groupes->tree_store_etat, &groupe->Iter, 0, groupe->numero, 1, groupe->nom, -1);
        
        /* Sélection de la première ligne du tree_view_etat */
        if (list_parcours->data == list_parcours->data)
        {
            path = gtk_tree_model_get_path(GTK_TREE_MODEL(gtk_1990_groupes->tree_store_etat), &groupe->Iter);
            gtk_tree_selection_select_path(gtk_1990_groupes->tree_select_etat, path);
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
                gtk_tree_store_append(gtk_1990_groupes->tree_store_etat, &element->Iter, &groupe->Iter);
                if (niveau == 0)
                {
                    Action *action;
                    
                    BUG(action = _1990_action_numero_cherche(projet, element->numero), FALSE)
                    gtk_tree_store_set(gtk_1990_groupes->tree_store_etat, &element->Iter, 0, element->numero, 1, _1990_action_nom_renvoie(action), -1);
                }
                else
                {
                    Niveau_Groupe *groupe_niveau_moins_1;
                    Groupe *groupe_moins_1;
                    BUG(groupe_niveau_moins_1 = _1990_groupe_positionne_niveau(projet, niveau-1), FALSE)
                    BUG(groupe_moins_1 = _1990_groupe_positionne_groupe(groupe_niveau_moins_1, element->numero), FALSE)
                    gtk_tree_store_set(gtk_1990_groupes->tree_store_etat, &element->Iter, 0, element->numero, 1, groupe_moins_1->nom, -1);
                }
                
                list_parcours2 = g_list_next(list_parcours2);
            }
            while (list_parcours2 != NULL);
        }
        /* Si la dernière fois que la ligne était affiché, elle était 'expand',
         * on rétablit son attribut */
        if (groupe->Iter_expand == 1)
        {
            path = gtk_tree_model_get_path(GTK_TREE_MODEL(gtk_1990_groupes->tree_store_etat), &groupe->Iter);
            gtk_tree_view_expand_row(gtk_1990_groupes->tree_view_etat, path, FALSE);
            gtk_tree_path_free(path);
        }
        list_parcours = g_list_next(list_parcours);
    }
    
    
    /* On affiche tous les éléments disponibles dans le tree_view_dispo */
    for (i=0;i<dispo_max;i++)
    {
        if (dispos[i] == 0)
        {
            GtkTreeIter Iter;
            gtk_tree_store_append(gtk_1990_groupes->tree_store_dispo, &Iter, NULL);
            
            /* Sélection de la première ligne du tree_view_dispo */
            if (premier == TRUE)
            {
                path = gtk_tree_model_get_path(GTK_TREE_MODEL(gtk_1990_groupes->tree_store_dispo), &Iter);
                gtk_tree_selection_select_path(gtk_1990_groupes->tree_select_dispo, path);
                gtk_tree_path_free(path);
                premier = FALSE;
            }
            
            if (niveau == 0)
            {
                Action *action;
                
                BUG(action = _1990_action_numero_cherche(projet, i), FALSE)
                gtk_tree_store_set(gtk_1990_groupes->tree_store_dispo, &Iter, 0, i, 1, _1990_action_nom_renvoie(action), -1);
            }
            else
            {
                Niveau_Groupe *groupe_niveau_moins_1;
                Groupe *groupe_moins_1;
                BUG(groupe_niveau_moins_1 = _1990_groupe_positionne_niveau(projet, niveau-1), FALSE)
                BUG(groupe_moins_1 = _1990_groupe_positionne_groupe(groupe_niveau_moins_1, i), FALSE)
                gtk_tree_store_set(gtk_1990_groupes->tree_store_dispo, &Iter, 0, i, 1, groupe_moins_1->nom, -1);
            }
        }
    }

    free(dispos);
    
    return TRUE;
}


void _1990_gtk_spin_button_niveau_change(GtkWidget *button, Projet *projet)
/* Description : Met à jour l'affichage graphique en fonction de la valeur du spin_button
 * Paramètres : GtkWidget *button : composant spin_button ayant réalisé l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 * Echec : projet == NULL,
 *         interface graphique non initialisée,
 *         _1990_gtk_groupes_affiche_niveau.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet")
    BUGMSG(projet->list_gtk._1990_groupes.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Groupes")
    
    BUG(_1990_gtk_groupes_affiche_niveau(projet, GTK_COMMON_SPINBUTTON_AS_UINT(GTK_SPIN_BUTTON(projet->list_gtk._1990_groupes.spin_button_niveau))), )
    return;
}


void _1990_gtk_button_niveau_suppr_clicked(GtkWidget *button, Projet *projet)
/* Description : Supprime le niveau en cours.
 * Paramètres : GtkWidget *button : composant spin_button ayant réalisé l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 * Echec : projet == NULL,
 *         interface graphique non initialisée,
 *         _1990_groupe_free_niveau.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet")
    BUGMSG(projet->list_gtk._1990_groupes.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Groupes")
    
    BUG(_1990_groupe_free_niveau(projet, GTK_COMMON_SPINBUTTON_AS_UINT(GTK_SPIN_BUTTON(projet->list_gtk._1990_groupes.spin_button_niveau)), FALSE), )
    
    return;
}


void _1990_gtk_button_niveau_ajout_clicked(GtkWidget *button, Projet *projet)
/* Description : Ajoute un niveau de groupe.
 * Paramètres : GtkWidget *button : composant spin_button ayant réalisé l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 * Echec : projet == NULL,
 *         interface graphique non initialisée,
 *         _1990_groupe_ajout_niveau.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet")
    BUGMSG(projet->list_gtk._1990_groupes.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Groupes")
    
    BUG(_1990_groupe_ajout_niveau(projet), )
    
    return;
}


void _1990_gtk_button_groupe_ajout_clicked(GtkWidget *button, Projet *projet)
/* Description : Ajoute un groupe dans le niveau en cours.
 * Paramètres : GtkWidget *button : composant spin_button ayant réalisé l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 * Echec : projet == NULL,
 *         interface graphique non initialisée,
 *         _1990_groupe_positionne_niveau,
 *         _1990_groupe_ajout_groupe.
 */
{
    Niveau_Groupe   *niveau_groupe;
    Groupe          *groupe;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet")
    BUGMSG(projet->list_gtk._1990_groupes.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Groupes")
    
    /* On ajoute un niveau */
    BUG(niveau_groupe = _1990_groupe_positionne_niveau(projet, GTK_COMMON_SPINBUTTON_AS_UINT(GTK_SPIN_BUTTON(projet->list_gtk._1990_groupes.spin_button_niveau))), )
    BUG(groupe = _1990_groupe_ajout_groupe(projet, niveau_groupe->numero, GROUPE_COMBINAISON_AND, gettext("Sans nom")), )
    
    return;
}

int _1990_gtk_insert_dispo(Projet *projet, unsigned int numero, Niveau_Groupe *niveau)
/* Description : Ajoute dans le tree_view_dispo le numéro de l'élément disponible.
 *             : Cette fonction ne gère que l'interface graphique de tree_view_dispo.
 * Paramètres : Projet *projet : la variable projet,
 *            : unsigned int numero : numero de l'élément,
 *            : Niveau_Groupe *niveau : niveau à étudier.
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             projet == NULL,
 *             interface graphique non initialisée.
 */
{
    Gtk_1990_Groupes    *gtk_1990_groupes;
    gboolean            retour;
    GtkTreeModel        *model;
    GtkTreeIter         iter, iter2;
    unsigned int        nombre;
    char                *nom;
    GtkTreePath         *path;
    
    BUGMSG(projet, -1, gettext("Paramètre %s incorrect.\n"), "projet")
    BUGMSG(projet->list_gtk._1990_groupes.builder, -1, gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Groupes")
    
    gtk_1990_groupes = &projet->list_gtk._1990_groupes;
    model = gtk_tree_view_get_model(gtk_1990_groupes->tree_view_dispo);
    
    /* On parcours la liste des éléments disponibles à la recherche du numéro */
    retour = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(gtk_1990_groupes->tree_store_dispo), &iter2);
    if (retour == TRUE)
    {
        gtk_tree_model_get(model, &iter2, 0, &nombre, 1, &nom, -1);
        BUGMSG(nombre != numero, -1, gettext("Élément %d existant.\n"), numero)
    }
    while ((retour == TRUE) && (nombre < numero))
    {
        retour = gtk_tree_model_iter_next(GTK_TREE_MODEL(gtk_1990_groupes->tree_store_dispo), &iter2);
        if (retour == TRUE)
        {
            gtk_tree_model_get(model, &iter2, 0, &nombre, 1, &nom, -1);
            BUGMSG(nombre != numero, -1, gettext("Élément %d existant.\n"), numero)
        }
    }
    
    /* Si aucun élément avec un numéro supérieur est trouvé, on l'ajoute à la fin */
    if (retour == FALSE)
        gtk_tree_store_append(gtk_1990_groupes->tree_store_dispo, &iter, NULL);
    /* Sinon, on l'ajoute avant le dernier numéro trouvé */
    else
        gtk_tree_store_insert_before(gtk_1990_groupes->tree_store_dispo, &iter, NULL, &iter2);
    
    path = gtk_tree_model_get_path(GTK_TREE_MODEL(gtk_1990_groupes->tree_store_dispo), &iter);
    gtk_tree_selection_select_path(gtk_1990_groupes->tree_select_dispo, path);
    gtk_tree_path_free(path);
    
    if (projet->niveaux_groupes->data == niveau)
    {
        Action *action;
        
        BUG(action = _1990_action_numero_cherche(projet, numero), -3)
        gtk_tree_store_set(gtk_1990_groupes->tree_store_dispo, &iter, 0, numero, 1, _1990_action_nom_renvoie(action), -1);
    }
    else
    {
        Niveau_Groupe *groupe_niveau_moins_1;
        Groupe *groupe_moins_1;
        
        BUG(groupe_niveau_moins_1 = _1990_groupe_positionne_niveau(projet, niveau->numero-1), -3)
        BUG(groupe_moins_1 = _1990_groupe_positionne_groupe(groupe_niveau_moins_1, numero), -3)
        gtk_tree_store_set(gtk_1990_groupes->tree_store_dispo, &iter, 0, numero, 1, groupe_moins_1->nom, -1);
    }
    
    return 0;
}


void _1990_gtk_button_groupe_suppr_clicked(GtkWidget *button, Projet *projet)
/* Description : Supprime le groupe dans le niveau en cours.
 * Paramètres : GtkWidget *button : composant spin_button ayant réalisé l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    GtkTreeModel        *model;
    GtkTreeIter         iter, iter_tmp;
    unsigned int        niveau;
    unsigned int        numero, ngroupe;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet")
    BUGMSG(projet->list_gtk._1990_groupes.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Groupes")
    
    if (!gtk_tree_selection_get_selected(projet->list_gtk._1990_groupes.tree_select_etat, &model, &iter))
        return;
    
    /* On récupère les informations de la ligne sélectionnée */
    gtk_tree_model_get(model, &iter, 0, &numero, -1);
    ngroupe = _1990_gtk_get_groupe(model, &iter);
    
    niveau = GTK_COMMON_SPINBUTTON_AS_UINT(GTK_SPIN_BUTTON(projet->list_gtk._1990_groupes.spin_button_niveau));
    /* Il s'agit d'un groupe */
    if (!gtk_tree_model_iter_parent(model, &iter_tmp, &iter))
    {
        Niveau_Groupe   *niveau_groupe;
        Groupe          *groupe;
        
        /* On positionne le groupe en cours */
        BUG(niveau_groupe = _1990_groupe_positionne_niveau(projet, niveau), )
        BUG(groupe = _1990_groupe_positionne_groupe(niveau_groupe, ngroupe), )
        
        /* On supprime le groupe */
        BUG(_1990_groupe_free_groupe(projet, niveau, ngroupe), )
    }
    /* Si c'est un élément, on l'ajoute à la liste des éléments disponibles
     * et on le supprime du groupe */
    else
    {
        BUG(_1990_gtk_insert_dispo(projet, numero, _1990_groupe_positionne_niveau(projet, niveau)) == 0, )
        BUG(_1990_groupe_free_element(projet, niveau, ngroupe, numero), )
    }
    
    return;
}


void _1990_gtk_tree_view_drag_begin(GtkWidget *widget, GdkDragContext *drag_context,
  Projet *projet)
/* Description : Défini le composant à l'origine de DnD
 * Paramètres : GtkWidget *button : composant widget à l'origine de l'évènement,
 *            : GdkDragContext *drag_context : description du DnD,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet")
    
    projet->list_gtk._1990_groupes.drag_from = widget;
    
    return;
}


gboolean _1990_gtk_tree_view_dispo_drag(GtkWidget *widget, GdkDragContext *drag_context, gint x,
  gint y, guint tim, Projet *projet)
/* Description : Supprimer les éléments sélectionnées du tree_view_etat.
 * Paramètres : GtkWidget *button : composant spin_button ayant réalisé l'évènement,
 *            : GdkDragContext *drag_context : inutile,
 *            : GtkSelectionData *data : inutile,
 *            : guint info : inutile,
 *            : guint time : inutile,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : FALSE
 */
{
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet")
    
    _1990_gtk_button_groupe_suppr_clicked(widget, projet);
    
    return FALSE;
}


int _1990_gtk_button_ajout_dispo_proc(unsigned int ngroupe, Projet *projet)
/* Description : Ajoute les éléments disponibles sélectionnés dans le tree_view "dispo" dans le
 *               groupe ngroupe du niveau en cours.
 * Paramètres : ngroupe : numéro du groupe du niveau en cours où sera ajouté les éléments
 *                        disponibles sélectionnés,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             projet == NULL,
 *             aucun niveau de groupes,
 *             interface graphique non initialisée.
 */
{
    GtkTreeModel   	    *model1;
    GtkTreeIter    	    iter1;
    unsigned int        numero, niveau;
    char           	    *nom;
    Niveau_Groupe  	    *niveau_groupe;
    Groupe         	    *groupe;
    GList               *list, *list_orig;
    Gtk_1990_Groupes    *gtk_1990_groupes;
    
    BUGMSG(projet, -1, gettext("Paramètre %s incorrect.\n"), "projet")
    BUGMSG(projet->niveaux_groupes, -1, gettext("Le projet ne possède pas de niveaux de groupes.\n"))
    BUGMSG(projet->list_gtk._1990_groupes.builder, -1, gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Groupes")
    
    gtk_1990_groupes = &projet->list_gtk._1990_groupes;
    niveau = GTK_COMMON_SPINBUTTON_AS_UINT(GTK_SPIN_BUTTON(gtk_1990_groupes->spin_button_niveau));
    BUG(niveau_groupe = _1990_groupe_positionne_niveau(projet, niveau), -1)
    BUG(groupe = _1990_groupe_positionne_groupe(niveau_groupe, ngroupe), -1)
    list_orig = gtk_tree_selection_get_selected_rows(gtk_1990_groupes->tree_select_dispo, &model1);
    list = g_list_last(list_orig);
    
    for(;list != NULL; list = g_list_previous(list))
    {
        gtk_tree_model_get_iter(model1, &iter1, (GtkTreePath*)list->data);
        
        // On récupère les informations des lignes sélectionnées
        gtk_tree_model_get(model1, &iter1, 0, &numero, 1, &nom, -1);
        
        // On ajoute l'élément au groupe
        BUG(_1990_groupe_ajout_element(projet, niveau, ngroupe, numero), -1)
    }
    g_list_foreach(list_orig, (GFunc)gtk_tree_path_free, NULL);
    
    return 0;
}


void _1990_gtk_button_ajout_dispo_clicked(GtkWidget *button, Projet *projet)
/* Description : Ajoute les éléments disponibles sélectionnés dans le groupe sélectionné.
 * Paramètres : GtkWidget *button : composant spin_button ayant réalisé l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    GtkTreeIter     iter;
    GtkTreeModel    *model;
    unsigned int    ngroupe;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet")
    BUGMSG(projet->niveaux_groupes, , gettext("Le projet ne possède pas de niveaux de groupes.\n"))
    BUGMSG(projet->list_gtk._1990_groupes.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Groupes")
    
    /* On vérifie s'il y a des éléments sélectionnés */
    if (!gtk_tree_selection_get_selected(projet->list_gtk._1990_groupes.tree_select_etat, &model, &iter))
        return;
    ngroupe = _1990_gtk_get_groupe(model, &iter);
    
    BUG(_1990_gtk_button_ajout_dispo_proc(ngroupe, projet) == 0, )
    
    return;
}


void _1990_gtk_button_ajout_tout_dispo_clicked(GtkWidget *button, Projet *projet)
/* Description : Ajoute tous les éléments disponibles dans le groupe sélectionné.
 * Paramètres : GtkWidget *button : composant spin_button ayant réalisé l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    // Pour toutes les variables, les indices 1 désignent 'dispo' et les indices 2 désignent 'etat'
    GtkTreeIter     iter;
    GtkTreeModel    *model;
    unsigned int    ngroupe;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet")
    BUGMSG(projet->niveaux_groupes, , gettext("Le projet ne possède pas de niveaux de groupes.\n"))
    BUGMSG(projet->list_gtk._1990_groupes.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Groupes")
    
    gtk_tree_selection_select_all(projet->list_gtk._1990_groupes.tree_select_dispo);
    
    /* On vérifie s'il y a des éléments sélectionnés */
    if (!gtk_tree_selection_get_selected(projet->list_gtk._1990_groupes.tree_select_etat, &model, &iter))
        return;
    ngroupe = _1990_gtk_get_groupe(model, &iter);
    
    BUG(_1990_gtk_button_ajout_dispo_proc(ngroupe, projet) == 0, )
    
    return;
}


void _1990_gtk_tree_view_etat_drag(GtkWidget *widget, GdkDragContext *drag_context, gint x,
  gint y, guint tim, Projet *projet)
/* Description : Modifie le groupe d'un élément via le DnD.
 * Paramètres : GtkWidget *button : composant spin_button ayant réalisé l'évènement,
 *            : GdkDragContext *drag_context : inutile,
 *            : GtkSelectionData *data : inutile,
 *            : guint info : inutile,
 *            : guint time : inutile,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    gint                cx, cy;
    Gtk_1990_Groupes    *gtk_1990_groupes = &projet->list_gtk._1990_groupes;
    GtkTreePath         *path;
    GtkTreeIter         iter, iter_tmp;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet")
    BUGMSG(projet->niveaux_groupes, , gettext("Le projet ne possède pas de niveaux de groupes.\n"))
    BUGMSG(projet->list_gtk._1990_groupes.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Groupes")
    
    gdk_window_get_geometry(gtk_tree_view_get_bin_window(GTK_TREE_VIEW(gtk_1990_groupes->tree_view_etat)), &cx, &cy, NULL, NULL);
    gtk_tree_view_get_path_at_pos(GTK_TREE_VIEW(widget), x -=cx, y -=cy, &path, NULL, &cx, &cy);
    if (path != NULL)
    {
        GtkTreeModel    *list_store;
        unsigned int    ngroupe_dest;
        
        list_store = gtk_tree_view_get_model(GTK_TREE_VIEW(gtk_1990_groupes->tree_view_etat));
        gtk_tree_model_get_iter(list_store, &iter, path);
        ngroupe_dest = _1990_gtk_get_groupe(list_store, &iter);
        // Ajout les éléments disponibles sélectionnées dans le groupe souhaité.
        if (gtk_1990_groupes->drag_from == GTK_WIDGET(gtk_1990_groupes->tree_view_dispo))
            BUG(_1990_gtk_button_ajout_dispo_proc(ngroupe_dest, projet) == 0, )
        // Déplace l'élément sélectionné dans un autre groupe.
        else
        {
            unsigned int    ngroupe_source;
            GtkTreeModel    *model;
            unsigned int    numero;
            unsigned int    niveau;
            
            niveau = GTK_COMMON_SPINBUTTON_AS_UINT(GTK_SPIN_BUTTON(gtk_1990_groupes->spin_button_niveau));
            gtk_tree_selection_get_selected(gtk_1990_groupes->tree_select_etat, &model, &iter);
            if (gtk_tree_model_iter_parent(list_store, &iter_tmp, &iter))
            {
                ngroupe_source = _1990_gtk_get_groupe(model, &iter);
                if (ngroupe_source != ngroupe_dest)
                {
                    gtk_tree_model_get(model, &iter, 0, &numero, -1);
                    BUG(_1990_groupe_free_element(projet, niveau, ngroupe_source, numero), )
                    BUG(_1990_groupe_ajout_element(projet, niveau, ngroupe_dest, numero), )
                }
            }
        }
    }
    
    return;
}


void _1990_gtk_tree_view_etat_row_expanded(GtkTreeView *tree_view, GtkTreeIter *iter,
  GtkTreePath *path, Projet *projet)
/* Description : Evènement lorsqu'on ouvre une ligne contenant un enfant.
 *             : Est utilisé pour se souvenir des lignes qui sont ouvertes et fermées et les
 *               restitué lorsqu'on change de niveau.
 * Paramètres : GtkTreeView *tree_view : composant tree_view à l'origine de l'évènement,
 *            : GtkTreeIter *iter : ligne s'"ouvrant",
 *            : GtkTreePath *path : chemin vers la ligne,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    GtkTreeModel    *model = gtk_tree_view_get_model(tree_view);
    unsigned int    ngroupe;
    Niveau_Groupe   *niveau_groupe;
    Groupe          *groupe;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet")
    BUGMSG(projet->niveaux_groupes, , gettext("Le projet ne possède pas de niveaux de groupes.\n"))
    BUGMSG(projet->list_gtk._1990_groupes.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Groupes")
    
    // On détermine le groupe ayant entraîné une ouverture
    // En effet, seul un groupe dans le tree_view_etat peut entraîner une ouverture
    ngroupe = _1990_gtk_get_groupe(model, iter);
    BUG(niveau_groupe = _1990_groupe_positionne_niveau(projet, GTK_COMMON_SPINBUTTON_AS_UINT(GTK_SPIN_BUTTON(projet->list_gtk._1990_groupes.spin_button_niveau))), )
    BUG(groupe = _1990_groupe_positionne_groupe(niveau_groupe, ngroupe), )
    
    // On le marque comme ouvert.
    groupe->Iter_expand = 1;
    
    return;
}


void _1990_gtk_tree_view_etat_row_collapsed(GtkTreeView *tree_view, GtkTreeIter *iter,
  GtkTreePath *path, Projet *projet)
/* Description : Evènement lorsqu'on ferme une ligne contenant un enfant.
 *             : Est utilisé pour se souvenir des lignes qui sont ouvertes et fermées et les
 *               restitué lorsqu'on change de niveau.
 * Paramètres : GtkTreeView *tree_view : composant tree_view à l'origine de l'évènement,
 *            : GtkTreeIter *iter : ligne se "fermant",
 *            : GtkTreePath *path : chemin vers la ligne,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune
 */
{
    GtkTreeModel    *model = gtk_tree_view_get_model(tree_view);
    unsigned int    ngroupe;
    Niveau_Groupe   *niveau_groupe;
    Groupe          *groupe;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet")
    BUGMSG(projet->niveaux_groupes, , gettext("Le projet ne possède pas de niveaux de groupes.\n"))
    BUGMSG(projet->list_gtk._1990_groupes.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Groupes")
    
    // On détermine le groupe ayant entraîné une fermeture
    // En effet, seul un groupe dans le tree_view_etat peut entraîner une fermeture
    ngroupe = _1990_gtk_get_groupe(model, iter);
    BUG(niveau_groupe = _1990_groupe_positionne_niveau(projet, GTK_COMMON_SPINBUTTON_AS_UINT(GTK_SPIN_BUTTON(projet->list_gtk._1990_groupes.spin_button_niveau))), )
    BUG(groupe = _1990_groupe_positionne_groupe(niveau_groupe, ngroupe), )
    
    // On le marque comme fermé.
    groupe->Iter_expand = 0;
    
    return;
}


void _1990_gtk_button_groupe_toggled(GtkRadioToolButton *radiobutton, Projet *projet)
/* Description : Evènement lorsqu'il y a un changement de type de combinaison (OR, XOR ou AND).
 * Paramètres : GtkRadioButton *radiobutton : composant radiobutton à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    GtkTreeModel     *model;
    GtkTreeIter      iter;
    unsigned int     ngroupe;
    Niveau_Groupe    *niveau_groupe;
    Groupe           *groupe;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet")
    BUGMSG(projet->niveaux_groupes, , gettext("Le projet ne possède pas de niveaux de groupes.\n"))
    BUGMSG(projet->list_gtk._1990_groupes.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Groupes")
    
    /* On détermine le groupe sélectionné */
    if (!gtk_tree_selection_get_selected(projet->list_gtk._1990_groupes.tree_select_etat, &model, &iter))
        return;
    ngroupe = _1990_gtk_get_groupe(model, &iter);
    BUG(niveau_groupe = _1990_groupe_positionne_niveau(projet, GTK_COMMON_SPINBUTTON_AS_UINT(GTK_SPIN_BUTTON(projet->list_gtk._1990_groupes.spin_button_niveau))), )
    BUG(groupe = _1990_groupe_positionne_groupe(niveau_groupe, ngroupe), )
    
    /* On attribue le nouveau type de combinaison */
    if (radiobutton == (void *)projet->list_gtk._1990_groupes.item_groupe_and)
        BUG(_1990_groupe_modifie_combinaison(groupe, GROUPE_COMBINAISON_AND), )
    else if (radiobutton == (void *)projet->list_gtk._1990_groupes.item_groupe_or)
        BUG(_1990_groupe_modifie_combinaison(groupe, GROUPE_COMBINAISON_OR), )
    else if (radiobutton == (void *)projet->list_gtk._1990_groupes.item_groupe_xor)
        BUG(_1990_groupe_modifie_combinaison(groupe, GROUPE_COMBINAISON_XOR), )
    else
        BUGMSG(0, , gettext("Le type de combinaison est inconnu.\n"))
    
    return;
}


void _1990_gtk_groupes_button_generer_clicked(GtkWidget *button, Projet *projet)
/* Description : Génère les combinaisons.
 * Paramètres : GtkWidget *button : composant bouton à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet")
    BUGMSG(projet->niveaux_groupes, , gettext("Le projet ne possède pas de niveaux de groupes.\n"))
    BUGMSG(projet->list_gtk._1990_groupes.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Groupes")
    
    BUG(_1990_combinaisons_genere(projet), )
    BUG(_1990_groupe_affiche_tout(projet), )
    BUG(_1990_ponderations_affiche_tout(projet), )
    return;
}


void _1990_gtk_radio_button_eluequ_equ_seul(GtkRadioButton *radiobutton, Projet *projet)
/* Description : Modifie les options de combinaison pour que l'ELU EQU calcule à l'équilibre
 *               seulement.
 * Paramètres : GtkRadioButton *radiobutton : composant radiobutton à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet")
    
    if ((projet->combinaisons.flags & 1) != 0)
        projet->combinaisons.flags ^= 1;
    
    return;
}


void _1990_gtk_radio_button_eluequ_equ_resist(GtkRadioButton *radiobutton, Projet *projet)
/* Description : Modifie les options de combinaison pour que l'ELU EQU calcule à l'équilibre et
 *               à la résistance structurelle.
 * Paramètres : GtkRadioButton *radiobutton : composant radiobutton à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet")
    
    if ((projet->combinaisons.flags & 1) == 0)
        projet->combinaisons.flags++;
    
    return;
}


void _1990_gtk_radio_button_elustrgeo_1(GtkRadioButton *radiobutton, Projet *projet)
/* Description : Modifie les options de combinaison pour que l'ELU STR/GEO calcule selon
 *               l'approche 1.
 * Paramètres : GtkRadioButton *radiobutton : composant radiobutton à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet")
    
    projet->combinaisons.flags = projet->combinaisons.flags - (projet->combinaisons.flags & 6);
    projet->combinaisons.flags = projet->combinaisons.flags + 0;
    
    return;
}


void _1990_gtk_radio_button_elustrgeo_2(GtkRadioButton *radiobutton, Projet *projet)
/* Description : Modifie les options de combinaison pour que l'ELU STR/GEO calcule selon
 *               l'approche 2.
 * Paramètres : GtkRadioButton *radiobutton : composant radiobutton à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet")
    
    projet->combinaisons.flags = projet->combinaisons.flags - (projet->combinaisons.flags & 6);
    projet->combinaisons.flags = projet->combinaisons.flags + 2;
    
    return;
}


void _1990_gtk_radio_button_elustrgeo_3(GtkRadioButton *radiobutton, Projet *projet)
/* Description : Modifie les options de combinaison pour que l'ELU STR/GEO calcule selon
 *               l'approche 3.
 * Paramètres : GtkRadioButton *radiobutton : composant radiobutton à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet")
    
    projet->combinaisons.flags = projet->combinaisons.flags - (projet->combinaisons.flags & 6);
    projet->combinaisons.flags = projet->combinaisons.flags + 4;
    
    return;
}


void _1990_gtk_radio_button_elustrgeo_6_10(GtkRadioButton *radiobutton, Projet *projet)
/* Description : Modifie les options de combinaison pour que l'ELU STR/GEO calcule selon la
 *               formule 6.10 de l'EN 1990.
 * Paramètres : GtkRadioButton *radiobutton : composant radiobutton à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet")
    
    projet->combinaisons.flags = projet->combinaisons.flags - (projet->combinaisons.flags & 8);
    projet->combinaisons.flags = projet->combinaisons.flags + 8;
    
    return;
}


void _1990_gtk_radio_button_elustrgeo_6_10ab(GtkRadioButton *radiobutton, Projet *projet)
/* Description : Modifie les options de combinaison pour que l'ELU STR/GEO calcule selon la
 *               formule 6.10 (a) et (b) de l'EN 1990.
 * Paramètres : GtkRadioButton *radiobutton : composant radiobutton à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet")
    
    projet->combinaisons.flags = projet->combinaisons.flags - (projet->combinaisons.flags & 8);
    projet->combinaisons.flags = projet->combinaisons.flags + 0; 
    
    return;
}


void _1990_gtk_radio_button_eluacc_frequente(GtkRadioButton *radiobutton, Projet *projet)
/* Description : Modifie les options de combinaison pour que l'ELU ACC calcule avec les valeurs
 *               fréquentes des actions variables.
 * Paramètres : GtkRadioButton *radiobutton : composant radiobutton à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet")
    
    projet->combinaisons.flags = projet->combinaisons.flags - (projet->combinaisons.flags & 16);
    projet->combinaisons.flags = projet->combinaisons.flags + 0;
    
    return;
}


void _1990_gtk_radio_button_eluacc_quasi_permanente(GtkRadioButton *radiobutton, Projet *projet)
/* Description : Modifie les options de combinaison pour que l'ELU ACC calcule avec les valeurs
 *               quasi_permanente des actions variables.
 * Paramètres : GtkRadioButton *radiobutton : composant radiobutton à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet")
    
    projet->combinaisons.flags = projet->combinaisons.flags - (projet->combinaisons.flags & 16);
    projet->combinaisons.flags = projet->combinaisons.flags + 16;
    
    return;
}


void _1990_gtk_tooltip(GtkWidget *widget, gint x, gint y, gboolean keyboard_mode,
  GtkTooltip *tooltip, gpointer user_data)
/* Description : Cette fonction doit obligatoirement être relié à l'évènement "query-tooltip"
 *               pour qu'apparaisse la fenêtre tooltip.
 * Paramètres : GtkWidget *widget : composant à l'origine de l'évènement,
 *            : gint x, gint y, gboolean keyboard_mode, GtkTooltip *tooltip : paramètres de
 *                l'évènement query-tooltip,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    return;
}


void _1990_gtk_groupes_window_quitter_button(GtkWidget *object, Projet *projet)
/* Description : Bouton de fermeture de la fenêtre.
 * Paramètres : GtkComboBox *widget : composant à l'origine de la demande,
 *            : GtkWidget *fenêtre : la fenêtre d'options.
 * Valeur renvoyée : Aucune.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet")
    BUGMSG(projet->list_gtk._1990_groupes.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Groupes")
    
    gtk_widget_destroy(projet->list_gtk._1990_groupes.window_groupe);
    
    return;
}


void _1990_gtk_tree_view_etat_cell_edited(GtkCellRendererText *cell, gchar *path_string,
  gchar *new_text, Projet *projet)
/* Description : Edition du nom dans le tree-view-etat. On édite donc soit le nom d'un niveau,
 *               soit le nom d'une action.
 * Paramètres : GtkCellRendererText *cell : composant à l'origine de l'évènement,
 *            : gchar *path_string : ligne en cours d'édition,
 *            : gchar *new_text : nouveau nom,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    GtkTreePath   *path;
    GtkTreeIter   iter;
    GtkTreeModel  *model;
    unsigned int  numero, niveau;
    GtkTreeIter   iter_parent;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet")
    BUGMSG(projet->list_gtk._1990_groupes.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Groupes")
    
    path = gtk_tree_path_new_from_string(path_string);
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(projet->list_gtk._1990_groupes.tree_view_etat));
    gtk_tree_model_get_iter(model, &iter, path);
    gtk_tree_model_get(model, &iter, 0, &numero, -1);
    
    gtk_tree_path_free(path);
    
    niveau = GTK_COMMON_SPINBUTTON_AS_UINT(GTK_SPIN_BUTTON(projet->list_gtk._1990_groupes.spin_button_niveau));
    // On prend le niveau correspondant à niveau
    if (!gtk_tree_model_iter_parent(model, &iter_parent, &iter))
        BUG(_1990_groupe_modifie_nom(niveau, numero, new_text, projet), )
    else // On prend niveau-1
    {
        // Le nom est celui d'une action
        if (niveau == 0)
            BUG(_1990_action_nom_change(projet, _1990_action_numero_cherche(projet, numero), new_text), )
        else // Le nom est celui d'un groupe du niveau n-1
            BUG(_1990_groupe_modifie_nom(niveau-1, numero, new_text, projet), )
    }
}


void _1990_gtk_tree_view_dispo_cell_edited(GtkCellRendererText *cell, gchar *path_string,
  gchar *new_text, Projet *projet)
/* Description : Edition du nom dans le tree-view-dispo. On édite donc soit le nom d'un niveau,
 *               soit le nom d'une action.
 * Paramètres : GtkCellRendererText *cell : composant à l'origine de l'évènement,
 *            : gchar *path_string : ligne en cours d'édition,
 *            : gchar *new_text : nouveau nom,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    GtkTreePath   *path;
    GtkTreeIter   iter;
    GtkTreeModel  *model;
    unsigned int  numero, niveau;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet")
    BUGMSG(projet->list_gtk._1990_groupes.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Groupes")
    
    path = gtk_tree_path_new_from_string(path_string);
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(projet->list_gtk._1990_groupes.tree_view_dispo));
    gtk_tree_model_get_iter(model, &iter, path);
    gtk_tree_model_get(model, &iter, 0, &numero, -1);
    
    gtk_tree_path_free(path);
    
    niveau = GTK_COMMON_SPINBUTTON_AS_UINT(GTK_SPIN_BUTTON(projet->list_gtk._1990_groupes.spin_button_niveau));
    // Le nom est celui d'une action
    if (niveau == 0)
        BUG(_1990_action_nom_change(projet, _1990_action_numero_cherche(projet, numero), new_text), )
    else // Le nom est celui d'un groupe du niveau n-1
        BUG(_1990_groupe_modifie_nom(niveau-1, numero, new_text, projet), )
}


gboolean _1990_gtk_tree_view_etat_key_press_event(GtkWidget *widget, GdkEvent *event,
  Projet *projet)
/* Description : Gère les touches appuyées du treeview etat.
 * Paramètres : GtkWidget *widget : composant à l'origine de l'évènement,
 *            : GdkEvent *event : caractéristiques de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : FALSE.
 */
{
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet")
    BUGMSG(projet->list_gtk._1990_groupes.builder, FALSE, gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Groupes")
    
    if (event->key.keyval == GDK_KEY_Delete)
        _1990_gtk_button_groupe_suppr_clicked(widget, projet);
    
    return FALSE; /* Pour permettre aux autres touches d'être fonctionnelles  */
}


void _1990_gtk_tree_select_changed(GtkTreeSelection *treeselection, Projet *projet)
/* Description : Permet de activer/désactiver les boutons ajout en fonction de la selection.
 * Paramètres : GtkTreeSelection *treeselection : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    Gtk_1990_Groupes    *gtk_1990_groupes;
    GtkTreeIter         iter;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet")
    BUGMSG(projet->list_gtk._1990_groupes.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Groupes")
    
    gtk_1990_groupes = &projet->list_gtk._1990_groupes;
    
    // Si le tree_view est vide ou qu'aucun groupe n'est sélectionné
    if ( (!gtk_tree_model_get_iter_first(GTK_TREE_MODEL(gtk_1990_groupes->tree_store_dispo), &iter)) ||
         (!gtk_tree_selection_get_selected(gtk_1990_groupes->tree_select_etat, NULL, NULL)) )
        gtk_widget_set_sensitive(GTK_WIDGET(gtk_1990_groupes->item_ajout_tout_dispo), FALSE);
    else
        gtk_widget_set_sensitive(GTK_WIDGET(gtk_1990_groupes->item_ajout_tout_dispo), TRUE);
    
    // Si aucun groupe n'est sélectionné
    if (!gtk_tree_selection_get_selected(gtk_1990_groupes->tree_select_etat, NULL, NULL))
    {
        gtk_widget_set_sensitive(GTK_WIDGET(gtk_1990_groupes->item_ajout_dispo), FALSE);
        gtk_widget_set_sensitive(GTK_WIDGET(gtk_1990_groupes->item_groupe_suppr), FALSE);
        gtk_widget_set_sensitive(GTK_WIDGET(gtk_1990_groupes->item_groupe_and), FALSE);
        gtk_widget_set_sensitive(GTK_WIDGET(gtk_1990_groupes->item_groupe_or), FALSE);
        gtk_widget_set_sensitive(GTK_WIDGET(gtk_1990_groupes->item_groupe_xor), FALSE);
    }
    else
    {
        GList   *list_orig;
        GList   *list;
        
        gtk_widget_set_sensitive(GTK_WIDGET(gtk_1990_groupes->item_groupe_suppr), TRUE);
        gtk_widget_set_sensitive(GTK_WIDGET(gtk_1990_groupes->item_groupe_and), TRUE);
        gtk_widget_set_sensitive(GTK_WIDGET(gtk_1990_groupes->item_groupe_or), TRUE);
        gtk_widget_set_sensitive(GTK_WIDGET(gtk_1990_groupes->item_groupe_xor), TRUE);
        
        // Si aucune sélection dans la liste des éléments disponibles
        list_orig = gtk_tree_selection_get_selected_rows(gtk_1990_groupes->tree_select_dispo, NULL);
        list = g_list_last(list_orig);
        
        if (list == NULL)
            gtk_widget_set_sensitive(GTK_WIDGET(gtk_1990_groupes->item_ajout_dispo), FALSE);
        else
            gtk_widget_set_sensitive(GTK_WIDGET(gtk_1990_groupes->item_ajout_dispo), TRUE);
        
        g_list_foreach(list_orig, (GFunc)gtk_tree_path_free, NULL);
        g_list_free(list_orig);
    }
}


void _1990_gtk_groupes_window_destroy(GtkWidget *object, Projet *projet)
/* Description : met projet->list_gtk._1990_groupes.window à NULL quand la fenêtre se ferme.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
              : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet")
    BUGMSG(projet->list_gtk._1990_groupes.builder, , gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "Groupes")
    
    g_signal_handler_block(projet->list_gtk._1990_groupes.tree_view_etat, g_signal_handler_find(G_OBJECT(projet->list_gtk._1990_groupes.tree_view_etat),G_SIGNAL_MATCH_FUNC,0,0,NULL,_1990_gtk_groupes_tree_view_etat_cursor_changed,NULL));
    g_object_unref(G_OBJECT(projet->list_gtk._1990_groupes.builder));
    projet->list_gtk._1990_groupes.builder = NULL;
    return;
}


void _1990_gtk_groupes(Projet *projet)
/* Description : Création de la fenêtre Groupes.
 * Paramètres : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 *   Echec : projet == NULL,
 *           interface graphique impossible à générer.
 */
{
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet")
    if (projet->list_gtk._1990_groupes.builder != NULL)
    {
        gtk_window_present(GTK_WINDOW(projet->list_gtk._1990_groupes.window_groupe));
        return;
    }
    
    if (projet->niveaux_groupes == NULL)
        BUG(_1990_groupe_ajout_niveau(projet), )
    
    projet->list_gtk._1990_groupes.builder = gtk_builder_new();
    BUGMSG(gtk_builder_add_from_resource(projet->list_gtk._1990_groupes.builder, "/org/2lgc/codegui/ui/1990_groupes.ui", NULL) != 0, , gettext("Builder Failed\n"))
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
    
    BUG(_1990_gtk_groupes_affiche_niveau(projet, GTK_COMMON_SPINBUTTON_AS_UINT(GTK_SPIN_BUTTON(gtk_builder_get_object(projet->list_gtk._1990_groupes.builder, "1990_groupes_spin_button_niveaux")))), )
    gtk_window_set_transient_for(GTK_WINDOW(projet->list_gtk._1990_groupes.window_groupe), GTK_WINDOW(projet->list_gtk.comp.window));
    
    return;
}

#endif
