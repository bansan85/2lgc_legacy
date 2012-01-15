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
#include "1990_gtk_groupes.h"
#include "1990_ponderations.h"
#include "1990_combinaisons.h"
#include "common_erreurs.h"
#include "common_projet.h"
#include "common_tooltip.h"
#include "common_maths.h"
#include "common_gtk.h"
#include "EF_charge_barre_ponctuelle.h"
#include "EF_charge_noeud.h"
#include "EF_charge_barre_repartie_uniforme.h"

const GtkTargetEntry drag_targets_groupes_1[] = { {(gchar*)PACKAGE"1_SAME_PROC", GTK_TARGET_SAME_APP, 0}}; 
const GtkTargetEntry drag_targets_groupes_2[] = { {(gchar*)PACKAGE"2_SAME_PROC", GTK_TARGET_SAME_APP, 0}}; 
const GtkTargetEntry drag_targets_groupes_3[] = { {(gchar*)PACKAGE"1_SAME_PROC", GTK_TARGET_SAME_APP, 0},
                                                  {(gchar*)PACKAGE"2_SAME_PROC", GTK_TARGET_SAME_APP, 0}}; 

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
    int                     dispo_max, i, *dispos;
    gboolean                premier = TRUE;
    List_Gtk_1990_Groupes   *list_gtk_1990_groupes;
    GtkTreePath             *path;
    
    BUGMSG(projet, -1, "_1990_gtk_affiche_niveau\n");
    BUGMSG(list_size(projet->niveaux_groupes)-1 >= niveau, -1, "_1990_gtk_affiche_niveau\n");
    
    list_gtk_1990_groupes = &projet->list_gtk._1990_groupes;
    /* On supprime le contenu des deux composants tree_view */
    gtk_tree_store_clear(list_gtk_1990_groupes->tree_store_etat);
    gtk_tree_store_clear(list_gtk_1990_groupes->tree_store_dispo);
    
    /* dispo_max contient le nombre d'éléments maximum pouvant être disponible depuis le 
     * niveau 'niveau'-1 */
    if (niveau == 0)
        dispo_max = list_size(projet->actions);
    else
    {
        BUG(_1990_groupe_positionne_niveau(projet->niveaux_groupes, niveau-1) == 0, -3);
        niveau_groupe = (Niveau_Groupe*)list_curr(projet->niveaux_groupes);
        BUGMSG(niveau_groupe->groupes, -1, "_1990_gtk_affiche_niveau");
        dispo_max = list_size(niveau_groupe->groupes);
    }
    
    /* Ensuite, on initialise un tableau contenant une liste de boolean
     * pour déterminer au fur et à mesure de l'avancement de l'algorithme
     * quelques sont les éléments du niveau n-1 encore non placés */
    if (dispo_max != 0)
    {
        dispos = (int*)malloc(sizeof(int)*dispo_max);
        BUGMSG(dispos, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "_1990_gtk_affiche_niveau");
        for (i=0;i<dispo_max;i++)
            dispos[i] = 0;
    }
    else
        dispos = NULL;
    
    BUG(_1990_groupe_positionne_niveau(projet->niveaux_groupes, niveau) == 0, -3);
    niveau_groupe = (Niveau_Groupe*)list_curr(projet->niveaux_groupes);
    if (list_size(niveau_groupe->groupes) != 0)
    {
        /* Parcours le niveau à afficher */
        list_mvfront(niveau_groupe->groupes);
        do
        {
            Groupe      *groupe = (Groupe*)list_curr(niveau_groupe->groupes);
            
            /* Ajoute de la ligne dans le tree_store */
            gtk_tree_store_append(list_gtk_1990_groupes->tree_store_etat, &groupe->Iter, NULL);
            gtk_tree_store_set(list_gtk_1990_groupes->tree_store_etat, &groupe->Iter, 0, groupe->numero, 1, groupe->nom, -1);
            
            /* Sélection de la première ligne du tree_view_etat */
            if (list_curr(niveau_groupe->groupes) == list_front(niveau_groupe->groupes))
            {
                path = gtk_tree_model_get_path(GTK_TREE_MODEL(list_gtk_1990_groupes->tree_store_etat), &groupe->Iter);
                gtk_tree_selection_select_path(list_gtk_1990_groupes->tree_select_etat, path);
                gtk_tree_path_free(path);
            }
            
            if ((groupe->elements != NULL) && (list_size(groupe->elements) != 0))
            {
                list_mvfront(groupe->elements);
                do
                {
                    Element     *element = (Element*)list_curr(groupe->elements);
                    
                    /* On signale que l'élément a déjà été inséré */
                    dispos[element->numero] = 1;
                    /* puis ajout de la ligne dans le tree_store */
                    gtk_tree_store_append(list_gtk_1990_groupes->tree_store_etat, &element->Iter, &groupe->Iter);
                    if (niveau == 0)
                    {
                        Action *action;
                        
                        BUG(_1990_action_cherche_numero(projet, element->numero) == 0, -3);
                        action = (Action*)list_curr(projet->actions);
                        gtk_tree_store_set(list_gtk_1990_groupes->tree_store_etat, &element->Iter, 0, element->numero, 1, action->description, -1);
                    }
                    else
                    {
                        Niveau_Groupe *groupe_niveau_moins_1;
                        Groupe *groupe_moins_1;
                        BUG(_1990_groupe_positionne_niveau(projet->niveaux_groupes, niveau-1) == 0, -3);
                        groupe_niveau_moins_1 = (Niveau_Groupe*)list_curr(projet->niveaux_groupes);
                        BUG(_1990_groupe_positionne_groupe(groupe_niveau_moins_1, element->numero) == 0, -3);
                        groupe_moins_1 = (Groupe*)list_curr(groupe_niveau_moins_1->groupes);
                        gtk_tree_store_set(list_gtk_1990_groupes->tree_store_etat, &element->Iter, 0, element->numero, 1, groupe_moins_1->nom, -1);
                    }
                }
                while (list_mvnext(groupe->elements));
            }
            /* Si la dernière fois que la ligne était affiché, elle était 'expand',
             * on rétablit son attribut */
            if (groupe->Iter_expand == 1)
            {
                path = gtk_tree_model_get_path(GTK_TREE_MODEL(list_gtk_1990_groupes->tree_store_etat), &groupe->Iter);
                gtk_tree_view_expand_row(list_gtk_1990_groupes->tree_view_etat, path, FALSE);
                gtk_tree_path_free(path);
            }
        }
        while (list_mvnext(niveau_groupe->groupes) != NULL);
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
                
                BUG(_1990_action_cherche_numero(projet, i) == 0, -3);
                action = (Action*)list_curr(projet->actions);
                gtk_tree_store_set(list_gtk_1990_groupes->tree_store_dispo, &Iter, 0, i, 1, action->description, -1);
            }
            else
            {
                Niveau_Groupe *groupe_niveau_moins_1;
                Groupe *groupe_moins_1;
                BUG(_1990_groupe_positionne_niveau(projet->niveaux_groupes, niveau-1) == 0, -3);
                groupe_niveau_moins_1 = (Niveau_Groupe*)list_curr(projet->niveaux_groupes);
                BUG(_1990_groupe_positionne_groupe(groupe_niveau_moins_1, i) == 0, -3);
                groupe_moins_1 = (Groupe*)list_curr(groupe_niveau_moins_1->groupes);
                gtk_tree_store_set(list_gtk_1990_groupes->tree_store_dispo, &Iter, 0, i, 1, groupe_moins_1->nom, -1);
            }
        }
    }

    free(dispos);
    
    return 0;
}

void _1990_gtk_spin_button_niveau_change(GtkWidget *button __attribute__((unused)), Projet *projet)
/* Description : Met à jour l'affichage graphique en fonction de la valeur du spin_button
 * Paramètres : GtkWidget *button : composant spin_button ayant réalisé l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    List_Gtk_1990_Groupes   *list_gtk_1990_groupes;
    
    BUGMSG(projet, , "_1990_gtk_spin_button_niveau_change\n");
    
    list_gtk_1990_groupes = &projet->list_gtk._1990_groupes;
    _1990_gtk_affiche_niveau(projet, gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(list_gtk_1990_groupes->spin_button_niveau)));
    return;
}

void _1990_gtk_button_niveau_suppr_clicked(GtkWidget *button __attribute__((unused)), Projet *projet)
/* Description : Supprime le niveau en cours
 * Paramètres : GtkWidget *button : composant spin_button ayant réalisé l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    List_Gtk_1990_Groupes   *list_gtk_1990_groupes;
    
    BUGMSG(projet, , "_1990_gtk_button_niveau_suppr_clicked\n");
    
    list_gtk_1990_groupes = &projet->list_gtk._1990_groupes;
    _1990_groupe_free_niveau(projet, gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(list_gtk_1990_groupes->spin_button_niveau)));
    
    /* On oblige la liste des niveaux à posséder au moins un niveau vide */
    if (list_size(projet->niveaux_groupes) == 0)
    {
        BUG(_1990_groupe_ajout_niveau(projet) == 0, );
        gtk_tree_store_clear(list_gtk_1990_groupes->tree_store_etat);
        _1990_gtk_affiche_niveau(projet, 0);
    }
    
    /* On réajuste les limites du spin_button */
    gtk_spin_button_set_range(GTK_SPIN_BUTTON(list_gtk_1990_groupes->spin_button_niveau), 0, list_size(projet->niveaux_groupes)-1);
    return;
}

void _1990_gtk_button_niveau_ajout_clicked(GtkWidget *button __attribute__((unused)), Projet *projet)
/* Description : Ajoute un niveau
 * Paramètres : GtkWidget *button : composant spin_button ayant réalisé l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    List_Gtk_1990_Groupes   *list_gtk_1990_groupes;
    
    BUGMSG(projet, , "_1990_gtk_button_niveau_ajout_clicked\n");
    
    list_gtk_1990_groupes = &projet->list_gtk._1990_groupes;
    BUG(_1990_groupe_ajout_niveau(projet) == 0, );
    
    /* Mise à jour des limites du composant spin_button */
    gtk_tree_store_clear(list_gtk_1990_groupes->tree_store_etat);
    gtk_spin_button_set_range(GTK_SPIN_BUTTON(list_gtk_1990_groupes->spin_button_niveau), 0, list_size(projet->niveaux_groupes)-1);
    
    /* La modification de la valeur du composant spin_button execute automatiquement
     * _1990_gtk_spin_button_niveau_change qui met à jour l'interface graphique */
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(list_gtk_1990_groupes->spin_button_niveau), list_size(projet->niveaux_groupes)-1);
}

void _1990_gtk_button_groupe_ajout_clicked(GtkWidget *button __attribute__((unused)), Projet *projet)
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
    
    BUGMSG(projet, , "_1990_gtk_button_groupe_ajout_clicked\n");
    
    list_gtk_1990_groupes = &projet->list_gtk._1990_groupes;
    
    BUGMSG(projet->niveaux_groupes, , "_1990_gtk_button_groupe_ajout_clicked\n");
    BUGMSG(list_size(projet->niveaux_groupes), , "_1990_gtk_button_groupe_ajout_clicked\n");
    /* On ajoute un niveau */
    BUG(_1990_groupe_positionne_niveau(projet->niveaux_groupes, gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(list_gtk_1990_groupes->spin_button_niveau))) == 0, );
    niveau_groupe = (Niveau_Groupe*)list_curr(projet->niveaux_groupes);
    BUG(_1990_groupe_ajout_groupe(projet, niveau_groupe->niveau, GROUPE_COMBINAISON_AND, gettext("Sans nom")) == 0, );
    groupe = (Groupe*)list_rear(niveau_groupe->groupes);
    
    /* Et on met à jour l'affichage */
    gtk_tree_store_append(list_gtk_1990_groupes->tree_store_etat, &groupe->Iter, NULL);
    gtk_tree_store_set(list_gtk_1990_groupes->tree_store_etat, &groupe->Iter, 0, groupe->numero, 1, "", -1);
    path = gtk_tree_model_get_path(GTK_TREE_MODEL(list_gtk_1990_groupes->tree_store_etat), &groupe->Iter);
    gtk_tree_view_set_cursor(GTK_TREE_VIEW(list_gtk_1990_groupes->tree_view_etat), path, gtk_tree_view_get_column(GTK_TREE_VIEW(list_gtk_1990_groupes->tree_view_etat), 1), TRUE);
    gtk_tree_path_free(path);
}

int _1990_gtk_insert_dispo(Projet *projet, int numero)
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
    int             nombre;
    char            *nom;
    GtkTreePath     *path;
    
    BUGMSG(projet, -1, "_1990_gtk_insert_dispo\n");
    
    list_gtk_1990_groupes = &projet->list_gtk._1990_groupes;
    model = gtk_tree_view_get_model(list_gtk_1990_groupes->tree_view_dispo);
    
    /* On parcours la liste des éléments disponibles à la recherche du numéro */
    retour = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(list_gtk_1990_groupes->tree_store_dispo), &iter2);
    if (retour == TRUE)
    {
        gtk_tree_model_get(model, &iter2, 0, &nombre, 1, &nom, -1);
        BUGMSG(nombre != numero, -1, gettext("%s : Élément %d existant.\n"), "_1990_gtk_insert_dispo", numero);
    }
    while ((retour == TRUE) && (nombre < numero))
    {
        retour = gtk_tree_model_iter_next(GTK_TREE_MODEL(list_gtk_1990_groupes->tree_store_dispo), &iter2);
        if (retour == TRUE)
        {
            gtk_tree_model_get(model, &iter2, 0, &nombre, 1, &nom, -1);
            BUGMSG(nombre != numero, -1, gettext("%s : Élément %d existant.\n"), "_1990_gtk_insert_dispo", numero);
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
    
    if (list_front(projet->niveaux_groupes) == list_curr(projet->niveaux_groupes))
    {
        Action *action;
        
        BUG(_1990_action_cherche_numero(projet, numero) == 0, -3);
        action = (Action*)list_curr(projet->actions);
        gtk_tree_store_set(list_gtk_1990_groupes->tree_store_dispo, &iter, 0, numero, 1, action->description, -1);
    }
    else
    {
        Niveau_Groupe *groupe_niveau_moins_1;
        Groupe *groupe_moins_1;
        
        list_mvprev(projet->niveaux_groupes);
        groupe_niveau_moins_1 = (Niveau_Groupe*)list_curr(projet->niveaux_groupes);
        BUG(_1990_groupe_positionne_groupe(groupe_niveau_moins_1, numero) == 0, -3);
        groupe_moins_1 = (Groupe*)list_curr(groupe_niveau_moins_1->groupes);
        gtk_tree_store_set(list_gtk_1990_groupes->tree_store_dispo, &iter, 0, numero, 1, groupe_moins_1->nom, -1);
    }
    
    return 0;
}


int _1990_gtk_get_groupe(GtkTreeModel *tree_model, GtkTreeIter *iter)
{
    int     numero;
    char*   nom;
    GtkTreeIter iter_parent;
    
    if (gtk_tree_model_iter_parent(tree_model, &iter_parent, iter))
    {
        gtk_tree_model_get(tree_model, &iter_parent, 0, &numero, 1, &nom, -1);
        return numero;
    }
    else
    {
        gtk_tree_model_get(tree_model, iter, 0, &numero, 1, &nom, -1);
        return numero;
    }
}


void _1990_gtk_button_groupe_suppr_clicked(GtkWidget *button __attribute__((unused)), Projet *projet)
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
    int                 niveau, numero, ngroupe;
    char                *nom;
    
    BUGMSG(projet, , "_1990_gtk_button_groupe_suppr_clicked\n");
    BUGMSG(projet->niveaux_groupes, , "_1990_gtk_button_groupe_suppr_clicked\n");
    
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
    niveau = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(list_gtk_1990_groupes->spin_button_niveau));
    /* Il s'agit d'un groupe */
    if (!gtk_tree_model_iter_parent(model, &iter_tmp, &iter))
    {
        Niveau_Groupe   *niveau_groupe;
        Groupe          *groupe;
        
        /* On supprimer la ligne */
        gtk_tree_store_remove(list_gtk_1990_groupes->tree_store_etat, &iter);
        /* On positionne le groupe en cours */
        BUG(_1990_groupe_positionne_niveau(projet->niveaux_groupes, niveau) == 0, );
        niveau_groupe = (Niveau_Groupe*)list_curr(projet->niveaux_groupes);
        BUG(_1990_groupe_positionne_groupe(niveau_groupe, ngroupe) == 0, );
        groupe = (Groupe*)list_curr(niveau_groupe->groupes);
        
  /* On ajoute tous les éléments associés au groupe dans la liste des éléments disponibles */
        if (list_size(groupe->elements) != 0)
        {
            list_mvfront(groupe->elements);
            do
            {
                Element     *element;
                
                element = (Element*)list_curr(groupe->elements);
                BUG(_1990_gtk_insert_dispo(projet, element->numero) == 0, );
            }
            while (list_mvnext(groupe->elements));
        }
        
        /* On supprime le groupe */
        _1990_groupe_free_groupe(projet, niveau, ngroupe);
        
        /* Cette fonction est pour éviter d'avoir à utiliser la fonction
         * _1990_gtk_affiche_niveau (optimisation). Une mise à jour du composant graphique
         * est nécessaire car, en supprimant le groupe en cours, tous les groupes avec un
         * numéro supérieur se retrouvent avec leur numéro diminué de 1. */
        if (list_curr(niveau_groupe->groupes) != NULL)
        {
            list_mvfront(niveau_groupe->groupes);
            do
            {
                groupe = (Groupe*)list_curr(niveau_groupe->groupes);
                if (groupe->numero >= ngroupe)
                {
                    GValue      nouvelle_valeur;
                    /* Il est INDISPENSABLE de faire un memset avant g_value_init !!! */
                    memset(&nouvelle_valeur, 0, sizeof(nouvelle_valeur));
                    
                    g_value_init (&nouvelle_valeur, G_TYPE_INT);
        
                    /* On récupère le numéro du groupe */
                    g_value_set_int (&nouvelle_valeur, groupe->numero);
                    /* Et on modifie la ligne */
                    gtk_tree_store_set_value(list_gtk_1990_groupes->tree_store_etat, &groupe->Iter, 0, &nouvelle_valeur);
                }
            }
            while (list_mvnext(niveau_groupe->groupes));
        }
    }
    /* Si c'est un élément, on l'ajoute à la liste des éléments disponibles
     * et on le supprime du groupe */
    else
    {
        /* On supprimer la ligne */
        gtk_tree_store_remove(list_gtk_1990_groupes->tree_store_etat, &iter);
        BUG(_1990_gtk_insert_dispo(projet, numero) == 0, );
        _1990_groupe_free_element(projet, niveau, ngroupe, numero);
    }
    
    return;
}


void _1990_gtk_tree_view_drag_begin (GtkWidget *widget, 
  GdkDragContext *drag_context __attribute__((unused)), List_Gtk_1990_Groupes *gtk_1990)
{
    gtk_1990->drag_from = widget;
}


gboolean _1990_gtk_tree_view_dispo_drag(GtkWidget *widget __attribute__((unused)),
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
 * Valeur renvoyée : Aucune
 */
{
    _1990_gtk_button_groupe_suppr_clicked(widget, projet);
    return FALSE;
}


void _1990_gtk_button_ajout_dispo_proc(int ngroupe, Projet *projet)
{
    GtkTreeModel    *model1;
    GtkTreeIter     iter1;
    int             numero;
    char            *nom;
    Niveau_Groupe   *niveau_groupe;
    Groupe          *groupe;
    Element         *element, *element2;
    GList           *list, *list_orig;
    List_Gtk_1990_Groupes   *list_gtk_1990_groupes = &projet->list_gtk._1990_groupes;
    
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
        BUG(_1990_groupe_ajout_element(projet, gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(list_gtk_1990_groupes->spin_button_niveau)), ngroupe, numero) == 0, );
        
        /* On positionne l'élément en cours */
        niveau_groupe = (Niveau_Groupe*)list_curr(projet->niveaux_groupes);
        groupe = (Groupe*)list_curr(niveau_groupe->groupes);
        BUG(_1990_groupe_positionne_element(groupe, numero) == 0, );
        element = (Element*)list_curr(groupe->elements);
        
        /* On ajoute la ligne dans l'interface graphique */
        if (list_front(groupe->elements) == element)
            gtk_tree_store_prepend(list_gtk_1990_groupes->tree_store_etat, &element->Iter, &groupe->Iter);
        else
        {
            list_mvprev(groupe->elements);
            element2 = (Element*)list_curr(groupe->elements);
            gtk_tree_store_insert_after(list_gtk_1990_groupes->tree_store_etat, &element->Iter, &groupe->Iter, &element2->Iter);
        }
        
        if (list_front(projet->niveaux_groupes) == list_curr(projet->niveaux_groupes))
        {
            Action *action;
            
            BUG(_1990_action_cherche_numero(projet, numero) == 0, );
            action = (Action*)list_curr(projet->actions);
            gtk_tree_store_set(list_gtk_1990_groupes->tree_store_etat, &element->Iter, 0, numero, 1, action->description, -1);
        }
        else
        {
            Niveau_Groupe *groupe_niveau_moins_1;
            Groupe *groupe_moins_1;
            
            list_mvprev(projet->niveaux_groupes);
            groupe_niveau_moins_1 = (Niveau_Groupe*)list_curr(projet->niveaux_groupes);
            BUG(_1990_groupe_positionne_groupe(groupe_niveau_moins_1, numero) == 0, );
            groupe_moins_1 = (Groupe*)list_curr(groupe_niveau_moins_1->groupes);
            gtk_tree_store_set(list_gtk_1990_groupes->tree_store_etat, &element->Iter, 0, numero, 1, groupe_moins_1->nom, -1);
        }
       
        groupe->Iter_expand = 1;
        path = gtk_tree_model_get_path(GTK_TREE_MODEL(list_gtk_1990_groupes->tree_store_etat), &groupe->Iter);
        gtk_tree_view_expand_row(list_gtk_1990_groupes->tree_view_etat, path, FALSE);
        gtk_tree_path_free(path);
    }
    g_list_foreach(list_orig, (GFunc)gtk_tree_path_free, NULL);
    g_list_free(list_orig);
}


void _1990_gtk_button_ajout_dispo_clicked(GtkWidget *button __attribute__((unused)), Projet *projet)
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
    int             ngroupe;
    
    BUGMSG(projet, , "_1990_gtk_button_ajout_dispo_clicked\n");
    BUGMSG(projet->niveaux_groupes, , "_1990_gtk_button_ajout_dispo_clicked\n");
    
    
    /* On vérifie s'il y a des éléments sélectionnés */
    if (!gtk_tree_selection_get_selected(list_gtk_1990_groupes->tree_select_etat, &model2, &iter2))
        return;
    ngroupe = _1990_gtk_get_groupe(model2, &iter2);
    
    _1990_gtk_button_ajout_dispo_proc(ngroupe, projet);
    
    return;
}


void _1990_gtk_button_ajout_tout_dispo_clicked(GtkWidget *button __attribute__((unused)), Projet *projet)
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
    int             ngroupe;
    
    BUGMSG(projet, , "_1990_gtk_button_ajout_dispo_clicked\n");
    BUGMSG(projet->niveaux_groupes, , "_1990_gtk_button_ajout_dispo_clicked\n");
    
    gtk_tree_selection_select_all(list_gtk_1990_groupes->tree_select_dispo);
    
    /* On vérifie s'il y a des éléments sélectionnés */
    if (!gtk_tree_selection_get_selected(list_gtk_1990_groupes->tree_select_etat, &model2, &iter2))
        return;
    ngroupe = _1990_gtk_get_groupe(model2, &iter2);
    
    _1990_gtk_button_ajout_dispo_proc(ngroupe, projet);
    
    return;
}


void _1990_gtk_tree_view_etat_drag(GtkWidget *widget __attribute__((unused)), GdkDragContext *drag_context __attribute__((unused)),
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
    gint            cx, cy;
    List_Gtk_1990_Groupes   *list_gtk_1990_groupes = &projet->list_gtk._1990_groupes;
    GtkTreePath     *path;
    GtkTreeIter     iter, iter_tmp;
    GtkTreeModel    *list_store2;
    
    gdk_window_get_geometry (gtk_tree_view_get_bin_window (GTK_TREE_VIEW(list_gtk_1990_groupes->tree_view_etat)), &cx, &cy, NULL, NULL);
    gtk_tree_view_get_path_at_pos(GTK_TREE_VIEW(widget), x -=cx, y -=cy, &path, NULL, &cx, &cy);
    list_store2 = gtk_tree_view_get_model(GTK_TREE_VIEW(list_gtk_1990_groupes->tree_view_etat));
    if (path != NULL)
    {
        int     ngroupe_dest;
        gtk_tree_model_get_iter(list_store2, &iter, path);
        ngroupe_dest = _1990_gtk_get_groupe(list_store2, &iter);
        if (list_gtk_1990_groupes->drag_from == GTK_WIDGET(list_gtk_1990_groupes->tree_view_dispo))
            _1990_gtk_button_ajout_dispo_proc(ngroupe_dest, projet);
        else
        {
            int ngroupe_source;
            GtkTreeModel     *model;
            int              numero;
            int              niveau;
            char             *nom;
            Niveau_Groupe    *niveau_groupe;
            Groupe           *groupe;
            Element          *element;
            
            niveau = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(list_gtk_1990_groupes->spin_button_niveau));
            gtk_tree_selection_get_selected(list_gtk_1990_groupes->tree_select_etat, &model, &iter);
            if (gtk_tree_model_iter_parent(list_store2, &iter_tmp, &iter))
            {
                ngroupe_source = _1990_gtk_get_groupe(model, &iter);
                gtk_tree_model_get(model, &iter, 0, &numero, 1, &nom, -1);
                _1990_groupe_free_element(projet, niveau, ngroupe_source, numero);
                _1990_groupe_ajout_element(projet, niveau, ngroupe_dest, numero);
                gtk_tree_store_remove(list_gtk_1990_groupes->tree_store_etat, &iter);
                niveau_groupe = (Niveau_Groupe*)list_curr(projet->niveaux_groupes);
                groupe = (Groupe*)list_curr(niveau_groupe->groupes);
                BUG(_1990_groupe_positionne_element(groupe, numero) == 0, );
                element = (Element*)list_curr(groupe->elements);
                
                /* On ajoute la ligne dans l'interface graphique */
                if (list_front(groupe->elements) == element)
                    gtk_tree_store_prepend(list_gtk_1990_groupes->tree_store_etat, &element->Iter, &groupe->Iter);
                else
                {
                    Element*    element2;
                    list_mvprev(groupe->elements);
                    element2 = (Element*)list_curr(groupe->elements);
                    gtk_tree_store_insert_after(list_gtk_1990_groupes->tree_store_etat, &element->Iter, &groupe->Iter, &element2->Iter);
                }
                
                if (list_front(projet->niveaux_groupes) == list_curr(projet->niveaux_groupes))
                {
                    Action *action;
                    
                    BUG(_1990_action_cherche_numero(projet, numero) == 0, );
                    action = (Action*)list_curr(projet->actions);
                    gtk_tree_store_set(list_gtk_1990_groupes->tree_store_etat, &element->Iter, 0, numero, 1, action->description, -1);
                }
                else
                {
                    Niveau_Groupe *groupe_niveau_moins_1;
                    Groupe *groupe_moins_1;
                    
                    list_mvprev(projet->niveaux_groupes);
                    groupe_niveau_moins_1 = (Niveau_Groupe*)list_curr(projet->niveaux_groupes);
                    BUG(_1990_groupe_positionne_groupe(groupe_niveau_moins_1, numero) == 0, );
                    groupe_moins_1 = (Groupe*)list_curr(groupe_niveau_moins_1->groupes);
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


void _1990_gtk_tree_view_etat_row_expanded(GtkTreeView *tree_view, GtkTreeIter *iter, __attribute__((unused)) GtkTreePath *path, Projet *projet)
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
    int             ngroupe;
    Niveau_Groupe   *niveau_groupe;
    Groupe          *groupe;
    
    BUGMSG(projet, , "_1990_gtk_tree_view_etat_row_expanded\n");
    BUGMSG(projet->niveaux_groupes, , "_1990_gtk_tree_view_etat_row_expanded\n");
    
    list_gtk_1990_groupes = &projet->list_gtk._1990_groupes;
    
    // On détermine le groupe ayant entraîné une ouverture
    // En effet, seul un groupe dans le tree_view_etat peut entraîner une ouverture
    ngroupe = _1990_gtk_get_groupe(model, iter);
    BUG(_1990_groupe_positionne_niveau(projet->niveaux_groupes, gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(list_gtk_1990_groupes->spin_button_niveau))) == 0, );
    niveau_groupe = (Niveau_Groupe*)list_curr(projet->niveaux_groupes);
    BUG(_1990_groupe_positionne_groupe(niveau_groupe, ngroupe) == 0, );
    groupe = (Groupe*)list_curr(niveau_groupe->groupes);
    
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
void _1990_gtk_tree_view_etat_row_collapsed(GtkTreeView *tree_view, GtkTreeIter *iter, __attribute__((unused)) GtkTreePath *path, Projet *projet)
{
    List_Gtk_1990_Groupes   *list_gtk_1990_groupes;
    GtkTreeModel    *model = gtk_tree_view_get_model(tree_view);
    int             ngroupe;
    Niveau_Groupe   *niveau_groupe;
    Groupe          *groupe;
    
    BUGMSG(projet, , "_1990_gtk_tree_view_etat_row_collapsed\n");
    BUGMSG(projet->niveaux_groupes, , "_1990_gtk_tree_view_etat_row_collapsed\n");
    
    list_gtk_1990_groupes = &projet->list_gtk._1990_groupes;
    
    // On détermine le groupe ayant entraîné une fermeture
    // En effet, seul un groupe dans le tree_view_etat peut entraîner une fermeture
    ngroupe = _1990_gtk_get_groupe(model, iter);
    BUG(_1990_groupe_positionne_niveau(projet->niveaux_groupes, gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(list_gtk_1990_groupes->spin_button_niveau))) == 0, );
    niveau_groupe = (Niveau_Groupe*)list_curr(projet->niveaux_groupes);
    BUG(_1990_groupe_positionne_groupe(niveau_groupe, ngroupe) == 0, );
    groupe = (Groupe*)list_curr(niveau_groupe->groupes);
    
    // On le marque comme fermé.
    groupe->Iter_expand = 0;
    
    return;
}


void _1990_gtk_tree_view_etat_cursor_changed(__attribute__((unused)) GtkTreeView *tree_view, Projet *projet)
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
    int             tmp, ngroupe;
    char            *nom;
    Niveau_Groupe   *niveau_groupe;
    Groupe          *groupe;
    
    BUGMSG(projet, , "_1990_gtk_tree_view_etat_cursor_changed\n");
    BUGMSG(projet->niveaux_groupes, , "_1990_gtk_tree_view_etat_cursor_changed\n");
    
    list_gtk_1990_groupes = &projet->list_gtk._1990_groupes;
    
    /* Détermine le groupe sélectionné */
    if (!gtk_tree_selection_get_selected(list_gtk_1990_groupes->tree_select_etat, &model, &iter))
        return;
    gtk_tree_model_get(model, &iter, 0, &tmp, 1, &nom, -1);
    ngroupe = _1990_gtk_get_groupe(model, &iter);
    BUG(_1990_groupe_positionne_niveau(projet->niveaux_groupes, gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(list_gtk_1990_groupes->spin_button_niveau))) == 0, );
    niveau_groupe = (Niveau_Groupe*)list_curr(projet->niveaux_groupes);
    BUG(_1990_groupe_positionne_groupe(niveau_groupe, ngroupe) == 0, );
    groupe = (Groupe*)list_curr(niveau_groupe->groupes);
    
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


void _1990_gtk_button_groupe_toggled(GtkRadioToolButton *radiobutton, Projet *projet)
/* Description : Evènement lorsqu'il y a un changement de type de combinaison (OR, XOR ou AND)
 * Paramètres : GtkRadioButton *radiobutton : composant radiobutton à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    List_Gtk_1990_Groupes    *list_gtk_1990_groupes;
    GtkTreeModel     *model;
    GtkTreeIter      iter;
    int              ngroupe;
    Niveau_Groupe    *niveau_groupe;
    Groupe           *groupe;
    
    BUGMSG(projet, , "_1990_gtk_button_groupe_toggled\n");
    BUGMSG(projet->niveaux_groupes, , "_1990_gtk_button_groupe_toggled\n");
    
    list_gtk_1990_groupes = &projet->list_gtk._1990_groupes;
    
    /* On détermine le groupe sélectionné */
    if (!gtk_tree_selection_get_selected(list_gtk_1990_groupes->tree_select_etat, &model, &iter))
        return;
    ngroupe = _1990_gtk_get_groupe(model, &iter);
    BUG(_1990_groupe_positionne_niveau(projet->niveaux_groupes, gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(list_gtk_1990_groupes->spin_button_niveau))) == 0, );
    niveau_groupe = (Niveau_Groupe*)list_curr(projet->niveaux_groupes);
    BUG(_1990_groupe_positionne_groupe(niveau_groupe, ngroupe) == 0, );
    groupe = (Groupe*)list_curr(niveau_groupe->groupes);
    
    /* On attribue le nouveau type de combinaison */
    if (radiobutton == (void*)list_gtk_1990_groupes->item_groupe_and)
        groupe->type_combinaison = GROUPE_COMBINAISON_AND;
    else if (radiobutton == (void*)list_gtk_1990_groupes->item_groupe_or)
        groupe->type_combinaison = GROUPE_COMBINAISON_OR;
    else if (radiobutton == (void*)list_gtk_1990_groupes->item_groupe_xor)
        groupe->type_combinaison = GROUPE_COMBINAISON_XOR;
    
    return;
}


void _1990_gtk_button_generer_clicked(GtkWidget *button __attribute__((unused)), Projet *projet)
/* Description : Génère les combinaisons
 * Paramètres : GtkWidget *button : composant bouton à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    BUG(_1990_combinaisons_genere(projet) == 0, );
    _1990_groupe_affiche_tout(projet);
    _1990_ponderations_affiche_tout(projet);
    return;
}


void _1990_gtk_radio_button_eluequ_equ_seul(GtkRadioButton *radiobutton __attribute__((unused)), Projet *projet)
/* Description : Modifie les options de combinaison pour que l'ELU EQU calcule à l'équilibre seulement
 * Paramètres : GtkRadioButton *radiobutton : composant radiobutton à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    BUGMSG(projet, , "_1990_gtk_radio_button_eluequ_equ_seul\n");
    if ((projet->combinaisons.flags & 1) != 0)
        projet->combinaisons.flags ^= 1;
    return;
}


void _1990_gtk_radio_button_eluequ_equ_resist(GtkRadioButton *radiobutton __attribute__((unused)), Projet *projet)
/* Description : Modifie les options de combinaison pour que l'ELU EQU calcule à l'équilibre et à la résistance structurelle
 * Paramètres : GtkRadioButton *radiobutton : composant radiobutton à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    BUGMSG(projet, , "_1990_gtk_radio_button_eluequ_equ_resist\n");
    if ((projet->combinaisons.flags & 1) == 0)
        projet->combinaisons.flags++;
    return;
}


void _1990_gtk_radio_button_elustrgeo_1(GtkRadioButton *radiobutton __attribute__((unused)), Projet *projet)
/* Description : Modifie les options de combinaison pour que l'ELU STR/GEO calcule selon l'approche 1
 * Paramètres : GtkRadioButton *radiobutton : composant radiobutton à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    BUGMSG(projet, , "_1990_gtk_radio_button_elustrgeo_1\n");
    projet->combinaisons.flags = projet->combinaisons.flags - (projet->combinaisons.flags & 6);
    projet->combinaisons.flags = projet->combinaisons.flags + 0;
    return;
}


void _1990_gtk_radio_button_elustrgeo_2(GtkRadioButton *radiobutton __attribute__((unused)), Projet *projet)
/* Description : Modifie les options de combinaison pour que l'ELU STR/GEO calcule selon l'approche 2
 * Paramètres : GtkRadioButton *radiobutton : composant radiobutton à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    BUGMSG(projet, , "_1990_gtk_radio_button_elustrgeo_2\n");
    projet->combinaisons.flags = projet->combinaisons.flags - (projet->combinaisons.flags & 6);
    projet->combinaisons.flags = projet->combinaisons.flags + 2;
    return;
}


void _1990_gtk_radio_button_elustrgeo_3(GtkRadioButton *radiobutton __attribute__((unused)), Projet *projet)
/* Description : Modifie les options de combinaison pour que l'ELU STR/GEO calcule selon l'approche 3
 * Paramètres : GtkRadioButton *radiobutton : composant radiobutton à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    BUGMSG(projet, , "_1990_gtk_radio_button_elustrgeo_3\n");
    projet->combinaisons.flags = projet->combinaisons.flags - (projet->combinaisons.flags & 6);
    projet->combinaisons.flags = projet->combinaisons.flags + 4;
    return;
}


void _1990_gtk_radio_button_elustrgeo_6_10(GtkRadioButton *radiobutton __attribute__((unused)), Projet *projet)
/* Description : Modifie les options de combinaison pour que l'ELU STR/GEO calcule selon la formule 6.10 de l'EN 1990
 * Paramètres : GtkRadioButton *radiobutton : composant radiobutton à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    BUGMSG(projet, , "_1990_gtk_radio_button_elustrgeo_6_10\n");
    projet->combinaisons.flags = projet->combinaisons.flags - (projet->combinaisons.flags & 8);
    projet->combinaisons.flags = projet->combinaisons.flags + 8;
    return;
}


void _1990_gtk_radio_button_elustrgeo_6_10ab(GtkRadioButton *radiobutton __attribute__((unused)), Projet *projet)
/* Description : Modifie les options de combinaison pour que l'ELU STR/GEO calcule selon la formule 6.10 (a) et (b) de l'EN 1990
 * Paramètres : GtkRadioButton *radiobutton : composant radiobutton à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    BUGMSG(projet, , "_1990_gtk_radio_button_elustrgeo_6_10ab\n");
    projet->combinaisons.flags = projet->combinaisons.flags - (projet->combinaisons.flags & 8);
    projet->combinaisons.flags = projet->combinaisons.flags + 0;
    return;
}


void _1990_gtk_radio_button_eluacc_frequente(GtkRadioButton *radiobutton __attribute__((unused)), Projet *projet)
/* Description : Modifie les options de combinaison pour que l'ELU ACC calcule avec les valeurs fréquentes des actions variables
 * Paramètres : GtkRadioButton *radiobutton : composant radiobutton à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    BUGMSG(projet, , "_1990_gtk_radio_button_eluacc_frequente\n");
    projet->combinaisons.flags = projet->combinaisons.flags - (projet->combinaisons.flags & 16);
    projet->combinaisons.flags = projet->combinaisons.flags + 0;
    return;
}


void _1990_gtk_radio_button_eluacc_quasi_permanente(GtkRadioButton *radiobutton __attribute__((unused)), Projet *projet)
/* Description : Modifie les options de combinaison pour que l'ELU ACC calcule avec les valeurs quasi_permanente des actions variables
 * Paramètres : GtkRadioButton *radiobutton : composant radiobutton à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    BUGMSG(projet, , "_1990_gtk_radio_button_eluacc_quasi_permanente\n");
    projet->combinaisons.flags = projet->combinaisons.flags - (projet->combinaisons.flags & 16);
    projet->combinaisons.flags = projet->combinaisons.flags + 16;
    return;
}


void _1990_gtk_tooltip(GtkWidget *widget __attribute__((unused)), gint x __attribute__((unused)), gint y __attribute__((unused)), gboolean keyboard_mode __attribute__((unused)), GtkTooltip *tooltip __attribute__((unused)), gpointer user_data __attribute__((unused)))
/* Description : Cette fonction doit obligatoirement être relié à l'évènement "query-tooltip" pour qu'apparaisse la fenêtre tooltip
 * Paramètres : GtkWidget *widget : composant à l'origine de l'évènement
 *            : gint x, gint y, gboolean keyboard_mode, GtkTooltip *tooltip : paramètre de l'évènement query-tooltip
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    return;
}


void _1990_gtk_window_quitter_button(GtkWidget *object __attribute__((unused)), GtkWidget *fenetre)
/* Description : Bouton de fermeture de la fenêtre
 * Paramètres : GtkComboBox *widget : composant à l'origine de la demande
 *            : GtkWidget *fenêtre : la fenêtre d'options
 * Valeur renvoyée : Aucune
 */
{
    BUGMSG(fenetre, , "_1990_gtk_window_quitter_button\n");
    gtk_widget_destroy(fenetre);
    return;
}


void _1990_gtk_button_options_clicked(GtkWidget *button __attribute__((unused)), Projet *projet)
/* Description : Affiche les différentes options pour la génération des combinaisons
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    GtkWidget       *pWindow;
    GtkWidget       *grid;
    GtkWidget       *grid_i;
    GtkWidget       *frame1, *frame2;
    GtkWidget       *label;
    GtkWidget       *radio_button_maitre, *radio_button_esclave1, *radio_button_esclave2;
    GtkSettings     *settings;
    List_Gtk_1990_Groupes   *list_gtk_1990_groupes;
    
    BUGMSG(projet, , "_1990_gtk_button_options_clicked\n");
    
    list_gtk_1990_groupes = &projet->list_gtk._1990_groupes;
    g_type_init();
        
    /* Définition de la fenêtre */
    GTK_NOUVELLE_FENETRE(pWindow, gettext("Options des combinaisons"), 800, 600);
    
    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(pWindow), grid);
    
    frame1 = gtk_frame_new("ELU Équilibre :");
    gtk_grid_attach(GTK_GRID(grid), frame1, 0, 0, 1, 1);
    
    /* Définition des composants graphiques permettant la sélection entre le calcul à
     * l'ELU EQU équilibre seulement ou équilibre et résistance structurelle ensemble */
    grid_i = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(frame1), grid_i);
    label=gtk_label_new(gettext("A l'État Limite Ultime d'ÉQUilibre, il est possible de générer les combinaisons pour la vérification à l'équilibre statique seulement ou à l'équilibre statique incluant la résistance d'éléments structuraux."));
    gtk_label_set_line_wrap(GTK_LABEL(label), TRUE);
    gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_FILL);
    g_signal_connect(G_OBJECT(label), "size-allocate", G_CALLBACK(wrapped_label_size_allocate_callback), NULL);
    gtk_grid_attach(GTK_GRID(grid_i), label, 0, 0, 1, 1);
    
    radio_button_maitre = gtk_radio_button_new_with_label_from_widget(NULL, gettext("Équilibre seulement"));
    g_signal_connect(G_OBJECT(radio_button_maitre), "toggled", G_CALLBACK(_1990_gtk_radio_button_eluequ_equ_seul), projet);
    gtk_grid_attach_next_to(GTK_GRID(grid_i), radio_button_maitre, label, GTK_POS_BOTTOM, 1, 1);
    radio_button_esclave1 = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio_button_maitre), gettext("Équilibre et résistance"));
    g_signal_connect(G_OBJECT(radio_button_esclave1), "toggled", G_CALLBACK(_1990_gtk_radio_button_eluequ_equ_resist), projet);
    gtk_grid_attach_next_to(GTK_GRID(grid_i), radio_button_esclave1, radio_button_maitre, GTK_POS_BOTTOM, 1, 1);
    if ((projet->combinaisons.flags & 1) == 0)
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_button_maitre), TRUE);
    else
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_button_esclave1), TRUE);
    
    
    /* Définition des composants graphiques permettant la sélection du calcul à
     * l'ELU STR/GEO entre la formule 6.10 et 6.10(a) et (b) */
    frame2 = gtk_frame_new(gettext("ELU STR et GEO, formule de combinaison :"));
    gtk_grid_attach_next_to(GTK_GRID(grid), frame2, frame1, GTK_POS_BOTTOM, 1, 1);
    
    grid_i = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(frame2), grid_i);
    label = gtk_label_new(gettext("À l'État Limite Ultime STRucture et GÉOtechnique, deux formules de combinaison sont possibles. La première est la formule 6.10 et la deuxième est 6.10a et 6.10b"));
    gtk_label_set_line_wrap(GTK_LABEL(label), TRUE);
    gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_FILL);
    g_signal_connect(G_OBJECT(label), "size-allocate", G_CALLBACK(wrapped_label_size_allocate_callback), NULL);
    gtk_grid_attach(GTK_GRID(grid_i), label, 0, 0, 1, 1);
    
    radio_button_maitre = gtk_radio_button_new_with_label_from_widget(NULL, "6.10");
    g_signal_connect(G_OBJECT(radio_button_maitre), "toggled", G_CALLBACK(_1990_gtk_radio_button_elustrgeo_6_10), projet);
    gtk_widget_set_has_tooltip(radio_button_maitre, TRUE);
    g_signal_connect(radio_button_maitre, "query-tooltip", G_CALLBACK(_1990_gtk_tooltip), projet);
    gtk_widget_set_tooltip_window(radio_button_maitre, GTK_WINDOW(common_tooltip_generation("1990_6_10")));
    settings = gtk_widget_get_settings(GTK_WIDGET(radio_button_maitre));
    g_object_set(settings, "gtk-tooltip-timeout", 0, NULL);
    gtk_grid_attach_next_to(GTK_GRID(grid_i), radio_button_maitre, label, GTK_POS_BOTTOM, 1, 1);
    radio_button_esclave1 = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio_button_maitre), gettext("6.10a et 6.10b"));
    g_signal_connect(G_OBJECT(radio_button_esclave1), "toggled", G_CALLBACK(_1990_gtk_radio_button_elustrgeo_6_10ab), projet);
    gtk_widget_set_has_tooltip(radio_button_esclave1, TRUE);
    g_signal_connect(radio_button_esclave1, "query-tooltip", G_CALLBACK(_1990_gtk_tooltip), projet);
    gtk_widget_set_tooltip_window(radio_button_esclave1, GTK_WINDOW(common_tooltip_generation("1990_6_10a_b")));
    settings = gtk_widget_get_settings(GTK_WIDGET(radio_button_esclave1));
    g_object_set(settings, "gtk-tooltip-timeout", 0, NULL);
    gtk_grid_attach_next_to(GTK_GRID(grid_i), radio_button_esclave1, radio_button_maitre, GTK_POS_BOTTOM, 1, 1);
    if ((projet->combinaisons.flags & 8) == 0)
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_button_esclave1), TRUE);
    else
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_button_maitre), TRUE);
    
    
    /* Définition des composants graphiques permettant la sélection du calcul à
     * l'ELU STR/GEO approche 1, 2 ou 3 */
    frame1 = gtk_frame_new(gettext("ELU STR et GEO, ensemble de calcul :"));
    gtk_grid_attach_next_to(GTK_GRID(grid), frame1, frame2, GTK_POS_BOTTOM, 1, 1);
    
    grid_i = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(frame1), grid_i);
    label = gtk_label_new(gettext("Lorsqu'il est nécessaire de réaliser un calcul à l'État Limite Ultime STRucture et GÉOtechnique, trois approches sont possibles. Approche 1 : vérification de la structure et du sol via les coefficients de l'ensemble B puis les coefficients de l'ensemble C. Approche 2 : vérification de la structure et du sol via les coefficients de l'ensemble B. Approche 3 : vérification de la structure et du sol via les coefficients de l'ensemble B pour les actions géotechniques et les coefficients de l'ensemble C pour les actions appliquées à la structure."));
    gtk_label_set_line_wrap(GTK_LABEL(label), TRUE);
    gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_FILL);
    g_signal_connect(G_OBJECT(label), "size-allocate", G_CALLBACK(wrapped_label_size_allocate_callback), NULL);
    gtk_grid_attach(GTK_GRID(grid_i), label, 0, 0, 1, 1);
    
    radio_button_maitre = gtk_radio_button_new_with_label_from_widget(NULL, gettext("Approche 1"));
    g_signal_connect(G_OBJECT(radio_button_maitre), "toggled", G_CALLBACK(_1990_gtk_radio_button_elustrgeo_1), projet);
    gtk_grid_attach_next_to(GTK_GRID(grid_i), radio_button_maitre, label, GTK_POS_BOTTOM, 1, 1);
    radio_button_esclave1 = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio_button_maitre), gettext("Approche 2"));
    g_signal_connect(G_OBJECT(radio_button_esclave1), "toggled", G_CALLBACK(_1990_gtk_radio_button_elustrgeo_2), projet);
    gtk_grid_attach_next_to(GTK_GRID(grid_i), radio_button_esclave1, radio_button_maitre, GTK_POS_BOTTOM, 1, 1);
    radio_button_esclave2 = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio_button_maitre), gettext("Approche 3"));
    g_signal_connect(G_OBJECT(radio_button_esclave2), "toggled", G_CALLBACK(_1990_gtk_radio_button_elustrgeo_3), projet);
    gtk_grid_attach_next_to(GTK_GRID(grid_i), radio_button_esclave2, radio_button_esclave1, GTK_POS_BOTTOM, 1, 1);
    if ((projet->combinaisons.flags & 6) == 4)
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_button_esclave2), TRUE);
    else if ((projet->combinaisons.flags & 6) == 2)
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_button_esclave1), TRUE);
    else
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_button_maitre), TRUE);
    
    
    /* Définition des composants graphiques permettant la sélection du calcul à l'ELU ACC valeur fréquente ou quasi-permanente */
    frame2 = gtk_frame_new(gettext("ELU ACCidentel :"));
    gtk_grid_attach_next_to(GTK_GRID(grid), frame2, frame1, GTK_POS_BOTTOM, 1, 1);
    
    grid_i = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(frame2), grid_i);
    label = gtk_label_new(gettext("À l'État Limite Ultime ACCidentel, la charge variable dominante utilisée est soit la valeur fréquente, soit la valeur quasi-permanente."));
    gtk_label_set_line_wrap(GTK_LABEL(label), TRUE);
    gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_FILL);
    g_signal_connect(G_OBJECT(label), "size-allocate", G_CALLBACK(wrapped_label_size_allocate_callback), NULL);
    gtk_grid_attach(GTK_GRID(grid_i), label, 0, 0, 1, 1);
    
    radio_button_maitre = gtk_radio_button_new_with_label_from_widget(NULL, gettext("valeur fréquence"));
    g_signal_connect(G_OBJECT(radio_button_maitre), "toggled", G_CALLBACK(_1990_gtk_radio_button_eluacc_frequente), projet);
    gtk_grid_attach_next_to(GTK_GRID(grid_i), radio_button_maitre, label, GTK_POS_BOTTOM, 1, 1);
    radio_button_esclave1 = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio_button_maitre), gettext("valeur quasi-permanente"));
    g_signal_connect(G_OBJECT(radio_button_esclave1), "toggled", G_CALLBACK(_1990_gtk_radio_button_eluacc_quasi_permanente), projet);
    gtk_grid_attach_next_to(GTK_GRID(grid_i), radio_button_esclave1, radio_button_maitre, GTK_POS_BOTTOM, 1, 1);
    if ((projet->combinaisons.flags & 16) == 0)
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_button_maitre), TRUE);
    else
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_button_esclave1), TRUE);
    
    /* Affichage de la fenêtre */
    gtk_window_set_transient_for(GTK_WINDOW(pWindow), GTK_WINDOW(list_gtk_1990_groupes->window_groupe));
    gtk_window_set_modal(GTK_WINDOW(pWindow), TRUE);
    gtk_widget_show_all(pWindow);
    
    return;
}


void _1990_gtk_tree_view_etat_cell_edited(GtkCellRendererText *cell __attribute__((unused)), gchar *path_string, gchar *new_text, Projet *projet)
{
    List_Gtk_1990_Groupes *list_gtk_1990_groupes = &projet->list_gtk._1990_groupes;
    GtkTreePath   *path = gtk_tree_path_new_from_string (path_string);
    GtkTreeIter   iter;
    GtkTreeModel  *model = gtk_tree_view_get_model(GTK_TREE_VIEW(list_gtk_1990_groupes->tree_view_etat));
    GValue        nouvelle_valeur;
    int           numero, niveau;
    char          *nom;
    GtkTreeIter   iter_parent;
    
    gtk_tree_model_get_iter (model, &iter, path);
    gtk_tree_model_get(model, &iter, 0, &numero, 1, &nom, -1);
    
    niveau = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(list_gtk_1990_groupes->spin_button_niveau));
    // On prend le niveau correspondant à niveau
    if (!gtk_tree_model_iter_parent(model, &iter_parent, &iter))
    {
        Niveau_Groupe *groupe_niveau;
        Groupe        *groupe;
        BUG(_1990_groupe_positionne_niveau(projet->niveaux_groupes, niveau) == 0, );
        groupe_niveau = (Niveau_Groupe*)list_curr(projet->niveaux_groupes);
        BUG(_1990_groupe_positionne_groupe(groupe_niveau, numero) == 0, );
        groupe = (Groupe*)list_curr(groupe_niveau->groupes);
        free(groupe->nom);
        groupe->nom = (char*)malloc(sizeof(char)*(strlen(new_text)+1));
        BUGMSG(groupe->nom, , gettext("%s : Erreur d'allocation mémoire.\n"), "_1990_gtk_tree_view_etat_cell_edited");
        strcpy(groupe->nom, new_text);
    }
    else // On prend niveau-1
    {
        // Le nom est celui d'une action
        if (niveau == 0)
        {
            Action      *action;
            _1990_action_cherche_numero(projet, numero);
            action = (Action*)list_curr(projet->actions);
            free(action->description);
            action->description = (char*)malloc(sizeof(char)*(strlen(new_text)+1));
            BUGMSG(action->description, , gettext("%s : Erreur d'allocation mémoire.\n"), "_1990_gtk_tree_view_etat_cell_edited");
            strcpy(action->description, new_text);
        }
        else // Le nom est celui d'un groupe du niveau n-1
        {
            Niveau_Groupe *groupe_niveau;
            Groupe        *groupe;
            BUG(_1990_groupe_positionne_niveau(projet->niveaux_groupes, niveau-1) == 0, );
            groupe_niveau = (Niveau_Groupe*)list_curr(projet->niveaux_groupes);
            BUG(_1990_groupe_positionne_groupe(groupe_niveau, numero) == 0, );
            groupe = (Groupe*)list_curr(groupe_niveau->groupes);
            free(groupe->nom);
            groupe->nom = (char*)malloc(sizeof(char)*(strlen(new_text)+1));
            BUGMSG(groupe->nom, , gettext("%s : Erreur d'allocation mémoire.\n"), "_1990_gtk_tree_view_etat_cell_edited");
            strcpy(groupe->nom, new_text);
        }
    }
    
    memset(&nouvelle_valeur, 0, sizeof(nouvelle_valeur));
    g_value_init (&nouvelle_valeur, G_TYPE_STRING);
    g_value_set_string (&nouvelle_valeur, new_text);
    gtk_tree_store_set_value(list_gtk_1990_groupes->tree_store_etat, &iter, 1, &nouvelle_valeur);
    
    gtk_tree_path_free (path);
}


gboolean _1990_gtk_tree_view_etat_key_press_event(GtkWidget *widget __attribute__((unused)), GdkEvent *event, Projet *projet)
{
    BUGMSG(projet, FALSE, "_1990_gtk_tree_view_etat_key_press_event\n");
    if (event->key.keyval == GDK_KEY_Delete)
        _1990_gtk_button_groupe_suppr_clicked(widget, projet);
    return FALSE; /* Pour permettre aux autres touches d'être fonctionnelles  */
}


void _1990_gtk_tree_select_changed(GtkTreeSelection *treeselection __attribute__((unused)), Projet *projet)
/* Description : Permet de activer/désactiver les boutons ajout en fonction de la selection
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    List_Gtk_1990_Groupes   *list_gtk_1990_groupes = &projet->list_gtk._1990_groupes;
    GtkTreeIter             iter;
    
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
        gtk_widget_set_sensitive(GTK_WIDGET(list_gtk_1990_groupes->item_groupe_suppr), TRUE);
        gtk_widget_set_sensitive(GTK_WIDGET(list_gtk_1990_groupes->item_groupe_and), TRUE);
        gtk_widget_set_sensitive(GTK_WIDGET(list_gtk_1990_groupes->item_groupe_or), TRUE);
        gtk_widget_set_sensitive(GTK_WIDGET(list_gtk_1990_groupes->item_groupe_xor), TRUE);
        
        // Si aucune sélection dans la liste des éléments disponibles
        GList   *list_orig = gtk_tree_selection_get_selected_rows(list_gtk_1990_groupes->tree_select_dispo, NULL);
        GList   *list = g_list_last(list_orig);
        if (list == NULL)
            gtk_widget_set_sensitive(GTK_WIDGET(list_gtk_1990_groupes->item_ajout_dispo), FALSE);
        else
            gtk_widget_set_sensitive(GTK_WIDGET(list_gtk_1990_groupes->item_ajout_dispo), TRUE);
        g_list_foreach(list_orig, (GFunc)gtk_tree_path_free, NULL);
        g_list_free(list_orig);
    }
}


void _1990_gtk_groupes(GtkWidget *button __attribute__((unused)), Projet *projet)
/* Description : Affichage de la fenêtre permettant de gérer les groupes
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    GtkWidget               *label;
    List_Gtk_1990_Groupes   *list_gtk_1990_groupes;
    GtkCellRenderer         *pCellRenderer;
    GtkTreeViewColumn       *pColumn;
    GList                   *list;
    
    list_gtk_1990_groupes = &projet->list_gtk._1990_groupes;
    list_gtk_1990_groupes->table = gtk_table_new(3, 1, FALSE);
    list_gtk_1990_groupes->table_niveau = gtk_table_new(1, 3, FALSE);
    list_gtk_1990_groupes->table_groupes = gtk_table_new(7, 2, FALSE);
    list_gtk_1990_groupes->table_dispo = gtk_table_new(2, 2, FALSE);
    list_gtk_1990_groupes->table_bas = gtk_table_new(1, 3, FALSE);
    
    /* Réglage de la fenêtre graphique */
    GTK_NOUVELLE_FENETRE(list_gtk_1990_groupes->window_groupe, gettext("Groupes"), 800, 600);
    gtk_container_add(GTK_CONTAINER(list_gtk_1990_groupes->window_groupe), list_gtk_1990_groupes->table);
    
    BUGMSG(projet->niveaux_groupes, , "_1990_gtk_groupes\n");
    if (list_size(projet->niveaux_groupes) == 0)
        BUG(_1990_groupe_ajout_niveau(projet) == 0, );
    
    /* Création des composants graphiques gérant les niveaux */
    label = gtk_label_new(gettext("Niveau : "));
    list_gtk_1990_groupes->spin_button_niveau = gtk_spin_button_new_with_range(0, list_size(projet->niveaux_groupes)-1, 1);
    g_signal_connect(list_gtk_1990_groupes->spin_button_niveau, "value-changed", G_CALLBACK (_1990_gtk_spin_button_niveau_change), projet);
    list_gtk_1990_groupes->button_niveau_ajout = gtk_button_new_with_label(" + ");
    g_signal_connect(G_OBJECT(list_gtk_1990_groupes->button_niveau_ajout), "clicked", G_CALLBACK(_1990_gtk_button_niveau_ajout_clicked), projet);
    list_gtk_1990_groupes->button_niveau_suppr = gtk_button_new_with_label("  -  ");
    g_signal_connect(G_OBJECT(list_gtk_1990_groupes->button_niveau_suppr), "clicked", G_CALLBACK(_1990_gtk_button_niveau_suppr_clicked), projet);
    gtk_table_attach(GTK_TABLE(list_gtk_1990_groupes->table_niveau), label, 0, 1, 0, 1, (GtkAttachOptions)0, (GtkAttachOptions)0, 0, 0);
    gtk_table_attach(GTK_TABLE(list_gtk_1990_groupes->table_niveau), list_gtk_1990_groupes->spin_button_niveau, 1, 2, 0, 1, (GtkAttachOptions)(GTK_EXPAND | GTK_FILL), (GtkAttachOptions)0, 0, 0);
    gtk_table_attach(GTK_TABLE(list_gtk_1990_groupes->table_niveau), list_gtk_1990_groupes->button_niveau_ajout, 2, 3, 0, 1, (GtkAttachOptions)GTK_FILL, (GtkAttachOptions)0, 0, 0);
    gtk_table_attach(GTK_TABLE(list_gtk_1990_groupes->table_niveau), list_gtk_1990_groupes->button_niveau_suppr, 3, 4, 0, 1, (GtkAttachOptions)GTK_FILL, (GtkAttachOptions)0, 0, 0);
    gtk_table_attach(GTK_TABLE(list_gtk_1990_groupes->table), list_gtk_1990_groupes->table_niveau, 0, 1, 0, 1, (GtkAttachOptions)(GTK_EXPAND | GTK_FILL), (GtkAttachOptions)(GTK_SHRINK | GTK_FILL), 0, 0);
    
    
    /* Création des composants graphiques affichant l'état du niveau en cours */
    list_gtk_1990_groupes->scroll_etat = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(list_gtk_1990_groupes->scroll_etat), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    list_gtk_1990_groupes->tree_store_etat = gtk_tree_store_new(2, G_TYPE_INT, G_TYPE_STRING);
    list_gtk_1990_groupes->tree_view_etat = (GtkTreeView*)gtk_tree_view_new_with_model(GTK_TREE_MODEL(list_gtk_1990_groupes->tree_store_etat));
    list_gtk_1990_groupes->tree_select_etat = gtk_tree_view_get_selection(list_gtk_1990_groupes->tree_view_etat);
    gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(list_gtk_1990_groupes->scroll_etat), GTK_WIDGET(list_gtk_1990_groupes->tree_view_etat));
    gtk_tree_view_set_reorderable(GTK_TREE_VIEW(list_gtk_1990_groupes->tree_view_etat), TRUE);
    list_gtk_1990_groupes->scroll_dispo = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(list_gtk_1990_groupes->scroll_dispo), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    list_gtk_1990_groupes->tree_store_dispo = gtk_tree_store_new(2, G_TYPE_INT, G_TYPE_STRING);
    list_gtk_1990_groupes->tree_view_dispo = (GtkTreeView*)gtk_tree_view_new_with_model(GTK_TREE_MODEL(list_gtk_1990_groupes->tree_store_dispo));
    list_gtk_1990_groupes->tree_select_dispo = gtk_tree_view_get_selection(list_gtk_1990_groupes->tree_view_dispo);
    gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(list_gtk_1990_groupes->scroll_dispo), GTK_WIDGET(list_gtk_1990_groupes->tree_view_dispo));
    gtk_tree_view_set_reorderable(GTK_TREE_VIEW(list_gtk_1990_groupes->tree_view_dispo), TRUE);
    
    g_signal_connect(GTK_WIDGET(list_gtk_1990_groupes->tree_view_dispo), "drag-begin", G_CALLBACK(_1990_gtk_tree_view_drag_begin), list_gtk_1990_groupes);
    g_signal_connect(GTK_WIDGET(list_gtk_1990_groupes->tree_view_etat), "drag-begin", G_CALLBACK(_1990_gtk_tree_view_drag_begin), list_gtk_1990_groupes);
    g_signal_connect(GTK_WIDGET(list_gtk_1990_groupes->tree_view_dispo), "drag-drop", G_CALLBACK(_1990_gtk_tree_view_dispo_drag), projet);
    g_signal_connect(GTK_WIDGET(list_gtk_1990_groupes->tree_view_etat), "drag-drop", G_CALLBACK(_1990_gtk_tree_view_etat_drag), projet);
    g_signal_connect(GTK_WIDGET(list_gtk_1990_groupes->tree_view_etat), "row-expanded", G_CALLBACK(_1990_gtk_tree_view_etat_row_expanded), projet);
    g_signal_connect(GTK_WIDGET(list_gtk_1990_groupes->tree_view_etat), "row-collapsed", G_CALLBACK(_1990_gtk_tree_view_etat_row_collapsed), projet);
    g_signal_connect(GTK_WIDGET(list_gtk_1990_groupes->tree_view_etat), "cursor-changed", G_CALLBACK(_1990_gtk_tree_view_etat_cursor_changed), projet);
    g_signal_connect(GTK_WIDGET(list_gtk_1990_groupes->tree_view_etat), "key-press-event", G_CALLBACK(_1990_gtk_tree_view_etat_key_press_event), projet);
    g_signal_connect(G_OBJECT(list_gtk_1990_groupes->tree_select_dispo), "changed", G_CALLBACK(_1990_gtk_tree_select_changed), projet);
    g_signal_connect(G_OBJECT(list_gtk_1990_groupes->tree_select_etat), "changed", G_CALLBACK(_1990_gtk_tree_select_changed), projet);
    
    /* Défini le comportement du glissé etat vers dispo*/
    gtk_drag_source_set(GTK_WIDGET(list_gtk_1990_groupes->tree_view_etat), GDK_BUTTON1_MASK, drag_targets_groupes_1, 1, GDK_ACTION_MOVE); 
    gtk_drag_dest_set(GTK_WIDGET(list_gtk_1990_groupes->tree_view_dispo), GTK_DEST_DEFAULT_ALL, drag_targets_groupes_1, 1, GDK_ACTION_MOVE);
    
    gtk_drag_source_set(GTK_WIDGET(list_gtk_1990_groupes->tree_view_dispo), GDK_BUTTON1_MASK, drag_targets_groupes_2, 1, GDK_ACTION_MOVE); 
    gtk_drag_dest_set(GTK_WIDGET(list_gtk_1990_groupes->tree_view_etat), GTK_DEST_DEFAULT_ALL, drag_targets_groupes_3, 2, GDK_ACTION_MOVE);
    
    gtk_tree_selection_set_mode(list_gtk_1990_groupes->tree_select_dispo, GTK_SELECTION_MULTIPLE);
    
    list_gtk_1990_groupes->frame_groupe = gtk_frame_new(gettext("Groupes :"));
    gtk_container_add(GTK_CONTAINER(list_gtk_1990_groupes->frame_groupe), list_gtk_1990_groupes->table_groupes);
    pCellRenderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(list_gtk_1990_groupes->tree_view_etat), -1, gettext("Numéro"), pCellRenderer, "text", 0, NULL);
    pCellRenderer = gtk_cell_renderer_text_new();
    g_object_set(pCellRenderer, "editable", TRUE, NULL);
    g_signal_connect(pCellRenderer, "edited", G_CALLBACK(_1990_gtk_tree_view_etat_cell_edited), projet);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(list_gtk_1990_groupes->tree_view_etat), -1, gettext("Description"), pCellRenderer, "text", 1, NULL);
    list = gtk_tree_view_get_columns(GTK_TREE_VIEW(list_gtk_1990_groupes->tree_view_etat));
    g_list_first(list);
    g_list_foreach(list, (GFunc)gtk_tree_view_column_set_resizable, (gpointer)TRUE);
    g_list_free(list);
    // On cache la colonne avec les numéros
    pColumn = gtk_tree_view_get_column(list_gtk_1990_groupes->tree_view_etat, 0);
    gtk_tree_view_column_set_visible(pColumn, FALSE);
    
    // Génération de la toolbar pour les groupes
    list_gtk_1990_groupes->toolbar_groupe = gtk_toolbar_new();
    gtk_orientable_set_orientation(GTK_ORIENTABLE(list_gtk_1990_groupes->toolbar_groupe), GTK_ORIENTATION_HORIZONTAL);
    gtk_toolbar_set_style(GTK_TOOLBAR(list_gtk_1990_groupes->toolbar_groupe), GTK_TOOLBAR_ICONS);
    gtk_container_set_border_width(GTK_CONTAINER(list_gtk_1990_groupes->toolbar_groupe), 0);
    list_gtk_1990_groupes->img_groupe_ajout = gtk_image_new_from_stock(GTK_STOCK_ADD, GTK_ICON_SIZE_SMALL_TOOLBAR);
    list_gtk_1990_groupes->item_groupe_ajout = gtk_tool_button_new(list_gtk_1990_groupes->img_groupe_ajout, gettext("Ajouter"));
    g_signal_connect(G_OBJECT(list_gtk_1990_groupes->item_groupe_ajout), "clicked", G_CALLBACK(_1990_gtk_button_groupe_ajout_clicked), projet);
    list_gtk_1990_groupes->img_groupe_suppr = gtk_image_new_from_stock(GTK_STOCK_GO_FORWARD, GTK_ICON_SIZE_SMALL_TOOLBAR);
    list_gtk_1990_groupes->item_groupe_suppr = gtk_tool_button_new(list_gtk_1990_groupes->img_groupe_suppr, gettext("Supprimer"));
    g_signal_connect(G_OBJECT(list_gtk_1990_groupes->item_groupe_suppr), "clicked", G_CALLBACK(_1990_gtk_button_groupe_suppr_clicked), projet);

    
    // Génération des composants permettant la suppression / ajout des groupes, gestion des éléments disponibles et du type de combinaison du groupe en cours */
    list_gtk_1990_groupes->item_groupe_and = gtk_radio_tool_button_new_from_widget(NULL);
    gtk_tool_button_set_label(GTK_TOOL_BUTTON(list_gtk_1990_groupes->item_groupe_and), "AND");
    g_signal_connect(G_OBJECT(list_gtk_1990_groupes->item_groupe_and), "toggled", G_CALLBACK(_1990_gtk_button_groupe_toggled), projet);
    list_gtk_1990_groupes->item_groupe_or = gtk_radio_tool_button_new_from_widget(GTK_RADIO_TOOL_BUTTON(list_gtk_1990_groupes->item_groupe_and));
    gtk_tool_button_set_label(GTK_TOOL_BUTTON(list_gtk_1990_groupes->item_groupe_or), "OR");
    g_signal_connect(G_OBJECT(list_gtk_1990_groupes->item_groupe_or), "toggled", G_CALLBACK(_1990_gtk_button_groupe_toggled), projet);
    list_gtk_1990_groupes->item_groupe_xor = gtk_radio_tool_button_new_from_widget(GTK_RADIO_TOOL_BUTTON(list_gtk_1990_groupes->item_groupe_and));
    gtk_tool_button_set_label(GTK_TOOL_BUTTON(list_gtk_1990_groupes->item_groupe_xor), "XOR");
    g_signal_connect(G_OBJECT(list_gtk_1990_groupes->item_groupe_xor), "toggled", G_CALLBACK(_1990_gtk_button_groupe_toggled), projet);
    gtk_toolbar_insert(GTK_TOOLBAR(list_gtk_1990_groupes->toolbar_groupe), list_gtk_1990_groupes->item_groupe_ajout, -1);
    gtk_toolbar_insert(GTK_TOOLBAR(list_gtk_1990_groupes->toolbar_groupe), list_gtk_1990_groupes->item_groupe_and, -1);
    gtk_toolbar_insert(GTK_TOOLBAR(list_gtk_1990_groupes->toolbar_groupe), list_gtk_1990_groupes->item_groupe_or, -1);
    gtk_toolbar_insert(GTK_TOOLBAR(list_gtk_1990_groupes->toolbar_groupe), list_gtk_1990_groupes->item_groupe_xor, -1);
    gtk_toolbar_insert(GTK_TOOLBAR(list_gtk_1990_groupes->toolbar_groupe), list_gtk_1990_groupes->item_groupe_suppr, -1);
    
    gtk_table_attach(GTK_TABLE(list_gtk_1990_groupes->table_groupes), GTK_WIDGET(list_gtk_1990_groupes->scroll_etat), 0, 1, 0, 1, (GtkAttachOptions)(GTK_EXPAND | GTK_FILL), (GtkAttachOptions)(GTK_EXPAND | GTK_FILL), 0, 0);
    gtk_table_attach(GTK_TABLE(list_gtk_1990_groupes->table_groupes), list_gtk_1990_groupes->toolbar_groupe, 0, 1, 1, 2, (GtkAttachOptions)(GTK_EXPAND | GTK_FILL), (GtkAttachOptions)(GTK_SHRINK | GTK_FILL), 0, 0);
//    gtk_table_attach (GTK_TABLE (list_gtk_1990_groupes->table_groupes), list_gtk_1990_groupes->button_groupe_and, 1, 2, 3, 4, (GtkAttachOptions)GTK_FILL, (GtkAttachOptions)0, 0, 0);
//    gtk_table_attach (GTK_TABLE (list_gtk_1990_groupes->table_groupes), list_gtk_1990_groupes->button_groupe_or, 1, 2, 4, 5, (GtkAttachOptions)GTK_FILL, (GtkAttachOptions)0, 0, 0);
//    gtk_table_attach (GTK_TABLE (list_gtk_1990_groupes->table_groupes), list_gtk_1990_groupes->button_groupe_xor, 1, 2, 5, 6, (GtkAttachOptions)GTK_FILL, (GtkAttachOptions)0, 0, 0);
    
    /* Composants gérant les éléments disponibles */
    list_gtk_1990_groupes->frame_dispo = gtk_frame_new(gettext("Éléments disponibles :"));
    gtk_container_add(GTK_CONTAINER(list_gtk_1990_groupes->frame_dispo), list_gtk_1990_groupes->table_dispo);
    
    pCellRenderer = gtk_cell_renderer_text_new();
    pColumn = gtk_tree_view_column_new_with_attributes(gettext("Numéro"), pCellRenderer, "text", 0, NULL);
    gtk_tree_view_append_column(list_gtk_1990_groupes->tree_view_dispo, pColumn);
    pCellRenderer = gtk_cell_renderer_text_new();
    pColumn = gtk_tree_view_column_new_with_attributes(gettext("Description"), pCellRenderer, "text", 1, NULL);
    gtk_tree_view_append_column(list_gtk_1990_groupes->tree_view_dispo, pColumn);
    list = gtk_tree_view_get_columns(GTK_TREE_VIEW(list_gtk_1990_groupes->tree_view_dispo));
    g_list_first(list);
    g_list_foreach(list, (GFunc)gtk_tree_view_column_set_resizable, (gpointer)TRUE);
    g_list_free(list);
    // On cache la colonne avec les numéros
    pColumn = gtk_tree_view_get_column(list_gtk_1990_groupes->tree_view_dispo, 0);
    gtk_tree_view_column_set_visible(pColumn, FALSE);
    gtk_table_attach (GTK_TABLE (list_gtk_1990_groupes->table_dispo), GTK_WIDGET(list_gtk_1990_groupes->scroll_dispo), 0, 1, 0, 1, (GtkAttachOptions)(GTK_EXPAND | GTK_FILL), (GtkAttachOptions)(GTK_EXPAND | GTK_FILL), 0, 0);
    
    list_gtk_1990_groupes->paned_groupe_dispo = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_paned_set_position(GTK_PANED(list_gtk_1990_groupes->paned_groupe_dispo), 400);
    gtk_table_attach(GTK_TABLE(list_gtk_1990_groupes->table), list_gtk_1990_groupes->paned_groupe_dispo, 0, 1, 1, 2, (GtkAttachOptions)(GTK_EXPAND | GTK_FILL), (GtkAttachOptions)(GTK_EXPAND | GTK_FILL), 0, 0);
    gtk_paned_add1(GTK_PANED(list_gtk_1990_groupes->paned_groupe_dispo), list_gtk_1990_groupes->frame_groupe);
    gtk_paned_add2(GTK_PANED(list_gtk_1990_groupes->paned_groupe_dispo), list_gtk_1990_groupes->frame_dispo);
    
    list_gtk_1990_groupes->toolbar_dispo = gtk_toolbar_new();
    gtk_orientable_set_orientation(GTK_ORIENTABLE(list_gtk_1990_groupes->toolbar_dispo), GTK_ORIENTATION_HORIZONTAL);
    gtk_toolbar_set_style(GTK_TOOLBAR(list_gtk_1990_groupes->toolbar_dispo), GTK_TOOLBAR_ICONS);
    gtk_container_set_border_width(GTK_CONTAINER(list_gtk_1990_groupes->toolbar_dispo), 0);
    list_gtk_1990_groupes->img_ajout_dispo = gtk_image_new_from_stock(GTK_STOCK_GO_BACK, GTK_ICON_SIZE_SMALL_TOOLBAR);
    list_gtk_1990_groupes->item_ajout_dispo = gtk_tool_button_new(list_gtk_1990_groupes->img_ajout_dispo, gettext("Ajouter"));
    g_signal_connect(G_OBJECT(list_gtk_1990_groupes->item_ajout_dispo), "clicked", G_CALLBACK(_1990_gtk_button_ajout_dispo_clicked), projet);
    gtk_toolbar_insert(GTK_TOOLBAR(list_gtk_1990_groupes->toolbar_dispo), list_gtk_1990_groupes->item_ajout_dispo, -1);
    list_gtk_1990_groupes->img_ajout_tout_dispo = gtk_image_new_from_stock(GTK_STOCK_GOTO_FIRST, GTK_ICON_SIZE_SMALL_TOOLBAR);
    list_gtk_1990_groupes->item_ajout_tout_dispo = gtk_tool_button_new(list_gtk_1990_groupes->img_ajout_tout_dispo, gettext("Ajouter tout"));
    g_signal_connect(G_OBJECT(list_gtk_1990_groupes->item_ajout_tout_dispo), "clicked", G_CALLBACK(_1990_gtk_button_ajout_tout_dispo_clicked), projet);
    gtk_toolbar_insert(GTK_TOOLBAR(list_gtk_1990_groupes->toolbar_dispo), list_gtk_1990_groupes->item_ajout_tout_dispo, -1);
    gtk_table_attach(GTK_TABLE(list_gtk_1990_groupes->table_dispo), list_gtk_1990_groupes->toolbar_dispo, 0, 1, 1, 2, (GtkAttachOptions)(GTK_EXPAND | GTK_FILL), (GtkAttachOptions)(GTK_SHRINK | GTK_FILL), 0, 0);
    gtk_widget_set_sensitive(GTK_WIDGET(list_gtk_1990_groupes->item_ajout_dispo), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(list_gtk_1990_groupes->item_ajout_tout_dispo), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(list_gtk_1990_groupes->item_groupe_suppr), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(list_gtk_1990_groupes->item_groupe_and), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(list_gtk_1990_groupes->item_groupe_or), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(list_gtk_1990_groupes->item_groupe_xor), FALSE);
    
    /* Composants gérant les options et la génération des combinaisons */
    gtk_table_attach (GTK_TABLE (list_gtk_1990_groupes->table), list_gtk_1990_groupes->table_bas, 0, 4, 3, 4, (GtkAttachOptions)(GTK_EXPAND | GTK_FILL), (GtkAttachOptions)0, 0, 0);
    list_gtk_1990_groupes->button_generer = gtk_button_new_with_label(gettext("Générer"));
    gtk_table_attach (GTK_TABLE (list_gtk_1990_groupes->table_bas), list_gtk_1990_groupes->button_generer, 0, 1, 0, 1, (GtkAttachOptions)(GTK_EXPAND | GTK_FILL), (GtkAttachOptions)0, 0, 0);
    g_signal_connect(G_OBJECT(list_gtk_1990_groupes->button_generer), "clicked", G_CALLBACK(_1990_gtk_button_generer_clicked), projet);
    list_gtk_1990_groupes->button_options = gtk_button_new_with_label(gettext("Options"));
    gtk_table_attach (GTK_TABLE (list_gtk_1990_groupes->table_bas), list_gtk_1990_groupes->button_options, 1, 2, 0, 1, (GtkAttachOptions)(GTK_EXPAND | GTK_FILL), (GtkAttachOptions)0, 0, 0);
    g_signal_connect(G_OBJECT(list_gtk_1990_groupes->button_options), "clicked", G_CALLBACK(_1990_gtk_button_options_clicked), projet);
    list_gtk_1990_groupes->button_quitter = gtk_button_new_with_label(gettext("Quitter"));
    gtk_table_attach (GTK_TABLE (list_gtk_1990_groupes->table_bas), list_gtk_1990_groupes->button_quitter, 2, 3, 0, 1, (GtkAttachOptions)(GTK_EXPAND | GTK_FILL), (GtkAttachOptions)0, 0, 0);
    g_signal_connect(G_OBJECT(list_gtk_1990_groupes->button_quitter), "clicked", G_CALLBACK(_1990_gtk_window_quitter_button), list_gtk_1990_groupes->window_groupe);
    
    _1990_gtk_affiche_niveau(projet, gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(list_gtk_1990_groupes->spin_button_niveau)));
    
    gtk_window_set_modal(GTK_WINDOW(list_gtk_1990_groupes->window_groupe), TRUE);
    gtk_window_set_transient_for(GTK_WINDOW(list_gtk_1990_groupes->window_groupe), GTK_WINDOW(projet->list_gtk.comp.window));
    gtk_widget_show_all(list_gtk_1990_groupes->window_groupe);
    
    return;
}
#endif