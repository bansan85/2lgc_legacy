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

#include "1990_groupes.h"
#include "1990_gtk_groupes.h"
#include "1990_ponderations.h"
#include "common_erreurs.h"
#include "common_projet.h"
#include "common_tooltip.h"

/* _1990_gtk_affiche_niveau
 * Description : Affiche le niveau souhaité dans l'interface graphique
 * Paramètres : Projet *projet : variable projet
 *            : int niveau : niveau à afficher
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : valeur négative en cas d'erreur
 */
int _1990_gtk_affiche_niveau(Projet *projet, unsigned int niveau)
{
	Niveau_Groupe	*niveau_groupe;
	int		dispo_max, i, *dispos;
	List_Gtk_1990	*list_gtk_1990 = projet->list_gtk._1990;
	
	if ((projet == NULL) || (projet->list_gtk._1990 == NULL)  || (list_size(projet->niveaux_groupes)-1 < niveau))
		BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
	
	list_gtk_1990 = projet->list_gtk._1990;
	// On supprime le contenu des deux composants tree_view
	gtk_tree_store_clear(list_gtk_1990->tree_store_etat);
	gtk_tree_store_clear(list_gtk_1990->tree_store_dispo);
	
	// dispo_max contient le nombre d'éléments maximum pouvant être disponible
	if (niveau == 0)
		dispo_max = list_size(projet->actions);
	else
	{
		if (_1990_groupe_niveau_positionne(projet->niveaux_groupes, niveau-1) != 0)
			BUG(-2);
		niveau_groupe = list_curr(projet->niveaux_groupes);
		dispo_max = list_size(niveau_groupe->groupes);
	}
	// Ensuite, on initialise un tableau contenant une liste de boolean
	// pour déterminer au fûr et à mesure de l'avancement de l'algorithme
	// quelques sont les éléments du niveau n-1 encore non placés
	if (dispo_max != 0)
	{
		dispos = malloc(sizeof(int)*dispo_max);
		if (dispos == NULL)
			BUGTEXTE(-3, gettext("Erreur d'allocation mémoire.\n"));
		for (i=0;i<dispo_max;i++)
			dispos[i] = 0;
	}
	else
		dispos = NULL;
	
	
	if (_1990_groupe_niveau_positionne(projet->niveaux_groupes, niveau) != 0)
		BUG(-4);
	niveau_groupe = list_curr(projet->niveaux_groupes);
	if (list_size(niveau_groupe->groupes) == 0)
	{
		free(dispos);
		return 0;
	}
	
	// On parcours le niveau à afficher
	list_mvfront(niveau_groupe->groupes);
	do
	{
		Groupe		*groupe = list_curr(niveau_groupe->groupes);
		
		// On vérifie si la ligne existe
		if (groupe->pIter == NULL)
		{
			groupe->pIter = malloc(sizeof(GtkTreeIter));
			if (groupe->pIter == NULL)
				BUGTEXTE(-3, gettext("Erreur d'allocation mémoire.\n"));
		}
		
		// On ajoute la ligne
		gtk_tree_store_append(list_gtk_1990->tree_store_etat, groupe->pIter, NULL);
		gtk_tree_store_set(list_gtk_1990->tree_store_etat, groupe->pIter, 0, groupe->numero, 1, -1, 2, groupe->nom, -1);
		
		if ((groupe->elements != NULL) && (list_size(groupe->elements) != 0))
		{
			list_mvfront(groupe->elements);
			do
			{
				Element		*element = list_curr(groupe->elements);
				
				// On signale que l'élément a déjà été inséré
				dispos[element->numero] = 1;
				// On ajoute la ligne
				if (element->pIter == NULL)
				{
					element->pIter = malloc(sizeof(GtkTreeIter));
					if (element->pIter == NULL)
						BUGTEXTE(-3, gettext("Erreur d'allocation mémoire.\n"));
				}
				gtk_tree_store_append(list_gtk_1990->tree_store_etat, element->pIter, groupe->pIter);
				gtk_tree_store_set(list_gtk_1990->tree_store_etat, element->pIter, 0, groupe->numero, 1, element->numero, 2, "", -1);
			}
			while (list_mvnext(groupe->elements));
		}
		// Si la dernière fois que la ligne était affiché, elle était 'expand', on rétablit son attribut
		if (groupe->pIter_expand == 1)
		{
			GtkTreePath	*path;
			
			path = gtk_tree_model_get_path(GTK_TREE_MODEL(list_gtk_1990->tree_store_etat), groupe->pIter);
			gtk_tree_view_expand_row(list_gtk_1990->tree_view_etat, path, FALSE);
			gtk_tree_path_free(path);
		}
	}
	while (list_mvnext(niveau_groupe->groupes) != NULL);
	
	// On affiche tous les éléments disponibles dans le tree_view_dispo
	for (i=0;i<dispo_max;i++)
	{
		if (dispos[i] == 0)
		{
			GtkTreeIter	Iter;
			gtk_tree_store_append(list_gtk_1990->tree_store_dispo, &Iter, NULL);
			gtk_tree_store_set(list_gtk_1990->tree_store_dispo, &Iter, 0, i, 1, "", -1);
		}
	}
	
	if (dispos != NULL)
		free(dispos);
	
	return 0;
}

/* _1990_gtk_spin_button_niveau_change
 * Description : Met à jour l'affichage graphique en fonction de la valeur du spin_button
 * Paramètres : GtkWidget *button : composant spin_button ayant réalisé l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
void _1990_gtk_spin_button_niveau_change(GtkWidget *button __attribute__((unused)), Projet *projet)
{
	List_Gtk_1990	*list_gtk_1990 = projet->list_gtk._1990;
	_1990_gtk_affiche_niveau(projet, gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(list_gtk_1990->spin_button_niveau)));
	return;
}

/* _1990_gtk_button_niveau_suppr_clicked
 * Description : Supprime le niveau en cours
 * Paramètres : GtkWidget *button : composant spin_button ayant réalisé l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
void _1990_gtk_button_niveau_suppr_clicked(GtkWidget *button __attribute__((unused)), Projet *projet)
{
	List_Gtk_1990	*list_gtk_1990 = projet->list_gtk._1990;
	
	_1990_groupe_free_niveau_numero(projet, gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(list_gtk_1990->spin_button_niveau)));
	
	// On oblige la liste des niveaux à posséder au moins un niveau vide
	if (list_size(projet->niveaux_groupes) == 0)
	{
		if (_1990_groupe_niveau_ajout(projet) != 0)
			BUG();
		gtk_tree_store_clear(list_gtk_1990->tree_store_etat);
	}
	
	// On réajuste les limites du spin_button
	gtk_spin_button_set_range(GTK_SPIN_BUTTON(list_gtk_1990->spin_button_niveau), 0, list_size(projet->niveaux_groupes)-1);
	return;
}

/* _1990_gtk_button_niveau_ajout_clicked
 * Description : Ajoute un niveau
 * Paramètres : GtkWidget *button : composant spin_button ayant réalisé l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
void _1990_gtk_button_niveau_ajout_clicked(GtkWidget *button __attribute__((unused)), Projet *projet)
{
	List_Gtk_1990	*list_gtk_1990 = projet->list_gtk._1990;
	
	if (_1990_groupe_niveau_ajout(projet) != 0)
		BUG();
	
	// Mise à jour des limites du composant spin_button
	gtk_tree_store_clear(list_gtk_1990->tree_store_etat);
	gtk_spin_button_set_range(GTK_SPIN_BUTTON(list_gtk_1990->spin_button_niveau), 0, list_size(projet->niveaux_groupes)-1);
	
	// La modification de la valeur du composant spin_button execute automatiquement _1990_gtk_spin_button_niveau_change
	// qui met à jour l'interface graphique
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(list_gtk_1990->spin_button_niveau), list_size(projet->niveaux_groupes)-1);
}

/* _1990_gtk_button_groupe_ajout_clicked
 * Description : Ajoute un groupe dans le niveau en cours
 * Paramètres : GtkWidget *button : composant spin_button ayant réalisé l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
void _1990_gtk_button_groupe_ajout_clicked(GtkWidget *button __attribute__((unused)), Projet *projet)
{
	List_Gtk_1990	*list_gtk_1990 = projet->list_gtk._1990;
	Niveau_Groupe	*niveau_groupe;
	Groupe		*groupe;
	
	// On ajoute un niveau
	_1990_groupe_niveau_positionne(projet->niveaux_groupes, gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(list_gtk_1990->spin_button_niveau)));
	niveau_groupe = list_curr(projet->niveaux_groupes);
	if (_1990_groupe_ajout(projet, niveau_groupe->niveau, list_size(niveau_groupe->groupes), GROUPE_COMBINAISON_AND) != 0)
		BUG();
	groupe = list_rear(niveau_groupe->groupes);
	
	// Et on met à jour l'affichage
	if (groupe->pIter == NULL)
	{
		groupe->pIter = malloc(sizeof(GtkTreeIter));
		if (groupe->pIter == NULL)
			BUGTEXTE(, gettext("Erreur d'allocation mémoire.\n"));
	}
	gtk_tree_store_append(list_gtk_1990->tree_store_etat, groupe->pIter, NULL);
	gtk_tree_store_set(list_gtk_1990->tree_store_etat, groupe->pIter, 0, groupe->numero, 1, -1, 2, "", -1);
}

/* _1990_gtk_insert_dispo
 * Description : Ajoute dans le tree_view_dispo le numéro de l'élément disponible.
 *             : Cette fonction ne gère que l'interface graphique de tree_view_dispo
 * Paramètres : Projet *projet : la variable projet
 *            : int numero : numero de l'élément
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : valeur négative en cas d'erreur
 */
int _1990_gtk_insert_dispo(Projet *projet, int numero)
{
	List_Gtk_1990	*list_gtk_1990 = projet->list_gtk._1990;
	gboolean	retour;
	GtkTreeModel	*model;
	GtkTreeIter	iter, iter2;
	int		nombre;
	char		*nom;
	
	model = gtk_tree_view_get_model(list_gtk_1990->tree_view_dispo);
	
	// On parcours la liste des éléments disponibles à la recherche du numéro
	retour = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(list_gtk_1990->tree_store_dispo), &iter2);
	if (retour == TRUE)
	{
		gtk_tree_model_get(model, &iter2, 0, &nombre, 1, &nom, -1);
		if (nombre == numero)
			BUGTEXTE(-1, "Élément existant.\n");
	}
	while ((retour == TRUE) && (nombre < numero))
	{
		retour = gtk_tree_model_iter_next(GTK_TREE_MODEL(list_gtk_1990->tree_store_dispo), &iter2);
		if (retour == TRUE)
		{
			gtk_tree_model_get(model, &iter2, 0, &nombre, 1, &nom, -1);
			if (nombre == numero)
				BUGTEXTE(-2, "Élément existant.\n");
		}
	}
	// Si aucun élément avec un numéro supérieur est trouvé, on l'ajoute à la fin
	if (retour == FALSE)
		gtk_tree_store_append(list_gtk_1990->tree_store_dispo, &iter, NULL);
	// Sinon, on l'ajoute avant le dernier numéro trouvé
	else
		gtk_tree_store_insert_before(list_gtk_1990->tree_store_dispo, &iter, NULL, &iter2);
	
	gtk_tree_store_set(list_gtk_1990->tree_store_dispo, &iter, 0, numero, 1, "", -1);
	
	return 0;
}

/* _1990_gtk_button_groupe_suppr_clicked
 * Description : Supprime le groupe dans le niveau en cours
 * Paramètres : GtkWidget *button : composant spin_button ayant réalisé l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
void _1990_gtk_button_groupe_suppr_clicked(GtkWidget *button __attribute__((unused)), Projet *projet)
{
	List_Gtk_1990	*list_gtk_1990 = projet->list_gtk._1990;
	GtkTreeSelection *selection = gtk_tree_view_get_selection(list_gtk_1990->tree_view_etat);
	GtkTreeModel	*model;
	GtkTreeIter	iter;
	int		niveau, ngroupe, numero;
	char		*nom;
	
	if (selection == NULL)
		return;
	if (!gtk_tree_selection_get_selected(selection, &model, &iter))
		return;
	
	// On récupère les informations de la ligne sélectionnée
	gtk_tree_model_get(model, &iter, 0, &ngroupe, 1, &numero, 2, &nom, -1);
	// On supprimer la ligne
	gtk_tree_store_remove(list_gtk_1990->tree_store_etat, &iter);
	
	niveau = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(list_gtk_1990->spin_button_niveau));
	// Si numero == -1 alors, il s'agit d'un groupe
	if (numero == -1)
	{
		Niveau_Groupe	*niveau_groupe;
		Groupe		*groupe;
		
		g_type_init();
		
		// On positionne le groupe en cours
		if (_1990_groupe_niveau_positionne(projet->niveaux_groupes, niveau) != 0)
			BUG();
		niveau_groupe = list_curr(projet->niveaux_groupes);
		if (_1990_groupe_positionne(niveau_groupe, ngroupe) != 0)
			BUG();
		groupe = list_curr(niveau_groupe->groupes);
		
		// On ajoute tous les éléments associés au groupe dans la liste des éléments disponibles
		if (list_size(groupe->elements) != 0)
		{
			list_mvfront(groupe->elements);
			do
			{
				Element		*element;
				
				element = list_curr(groupe->elements);
				_1990_gtk_insert_dispo(projet, element->numero);
			}
			while (list_mvnext(groupe->elements));
		}
		
		// On supprime le groupe
		_1990_groupe_free_numero(projet, niveau, ngroupe);
		
		// Cette fonction est pour éviter d'avoir à utiliser la fonction _1990_gtk_affiche_niveau (optimisation)
		// Une mise à jour du composant graphique est nécessaire car, en supprimant le groupe en cours,
		// tous les groupes avec un numéro supérieur se retrouvent avec leur numéro diminué de 1.
		if (list_curr(niveau_groupe->groupes) != NULL)
		{
			list_mvfront(niveau_groupe->groupes);
			do
			{
				groupe = list_curr(niveau_groupe->groupes);
				if (groupe->numero >= ngroupe)
				{
					GValue		nouvelle_valeur;
					memset(&nouvelle_valeur, 0, sizeof(nouvelle_valeur));
					
					g_value_init (&nouvelle_valeur, G_TYPE_INT);
		
					// On récupère le numéro du groupe
					g_value_set_int (&nouvelle_valeur, groupe->numero);
					// Et on modifie la ligne
					gtk_tree_store_set_value(list_gtk_1990->tree_store_etat, groupe->pIter, 0, &nouvelle_valeur);
					if ((groupe->elements != NULL) && (list_size(groupe->elements) != 0))
					{
						list_mvfront(groupe->elements);
						do
						{
							Element		*element;
							
							// Et ensuite, on modifie pour tous les lignes enfants le numéro du groupe où ils appartiennent
							element = list_curr(groupe->elements);
							if (element->pIter == NULL)
							{
								element->pIter = malloc(sizeof(GtkTreeIter));
								if (element->pIter == NULL)
									BUGTEXTE(, gettext("Erreur d'allocation mémoire.\n"));
							}
							gtk_tree_store_set_value(list_gtk_1990->tree_store_etat, element->pIter, 0, &nouvelle_valeur);
						}
						while (list_mvnext(groupe->elements) != NULL);
					}
				}
			}
			while (list_mvnext(niveau_groupe->groupes));
		}
	}
	// Si c'est seulement un élément, on l'ajoute à la liste des éléments disponibles
	// et on le supprime du groupe
	else
	{
		_1990_gtk_insert_dispo(projet, numero);
		_1990_groupe_free_element_numero(projet, niveau, ngroupe, numero);
	}
	return;
}

/* _1990_gtk_button_ajout_dispo_clicked
 * Description : Ajoute l'élément disponible sélectionné dans le groupe sélectionné
 * Paramètres : GtkWidget *button : composant spin_button ayant réalisé l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
void _1990_gtk_button_ajout_dispo_clicked(GtkWidget *button __attribute__((unused)), Projet *projet)
{
	// Pour toutes les variables, les indices 1 désignent 'dispo' et les indices 2 désignent 'etat'
	List_Gtk_1990	*list_gtk_1990 = projet->list_gtk._1990;
	GtkTreeSelection *selection1 = gtk_tree_view_get_selection(list_gtk_1990->tree_view_dispo);
	GtkTreeSelection *selection2 = gtk_tree_view_get_selection(list_gtk_1990->tree_view_etat);
	GtkTreeModel	*model1, *model2;
	GtkTreeIter	iter1, iter2;
	int		ngroupe, numero, tmp;
	char		*nom;
	Niveau_Groupe	*niveau_groupe;
	Groupe		*groupe;
	Element		*element, *element2;
	
	// On vérifie s'il y a des éléments sélectionnés
	if ((selection1 == NULL) || (selection2 == NULL))
		return;
	if ((!gtk_tree_selection_get_selected(selection1, &model1, &iter1)) || (!gtk_tree_selection_get_selected(selection2, &model2, &iter2)))
		return;
	
	// On récupère les informations des lignes sélectionnées
	gtk_tree_model_get(model1, &iter1, 0, &numero, 1, &nom, -1);
	gtk_tree_model_get(model2, &iter2, 0, &ngroupe, 1, &tmp, 2, &nom, -1);
	
	// Comme on ajoute un élément disponible au groupe, on supprime la ligne contenant l'élément anciennement disponible
	gtk_tree_store_remove(list_gtk_1990->tree_store_dispo, &iter1);
	
	// On ajoute l'élément au groupe
	if (_1990_groupe_ajout_element(projet, gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(list_gtk_1990->spin_button_niveau)), ngroupe, numero) != 0)
		BUG();
	
	// On positionne l'élément en cours
	niveau_groupe = list_curr(projet->niveaux_groupes);
	groupe = list_curr(niveau_groupe->groupes);
	_1990_groupe_element_positionne(groupe, numero);
	element = list_curr(groupe->elements);
	
	// On ajoute la ligne dans l'interface graphique
	if (element->pIter == NULL)
	{
		element->pIter = malloc(sizeof(GtkTreeIter));
		if (element->pIter == NULL)
			BUGTEXTE(, gettext("Erreur d'allocation mémoire.\n"));
	}
	if (list_front(groupe->elements) == element)
		gtk_tree_store_prepend(list_gtk_1990->tree_store_etat, element->pIter, groupe->pIter);
	else
	{
		list_mvprev(groupe->elements);
		element2 = list_curr(groupe->elements);
		gtk_tree_store_insert_after(list_gtk_1990->tree_store_etat, element->pIter, groupe->pIter, element2->pIter);
	}
	gtk_tree_store_set(list_gtk_1990->tree_store_etat, element->pIter, 0, ngroupe, 1, numero, 2, "", -1);
	
	return;
}

/* _1990_gtk_tree_view_etat_row_expanded
 * Description : Evènement lorsqu'on ouvre une ligne contenant un enfant
 *             : Est utilisé pour se souvenir des lignes qui sont ouvertes et fermées
 * Paramètres : GtkTreeView *tree_view : composant tree_view à l'origine de l'évènement
 *            : GtkTreeIter *iter : ligne s'"ouvrant"
 *            : GtkTreePath *path : chemin vers la ligne
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
void _1990_gtk_tree_view_etat_row_expanded(GtkTreeView *tree_view, GtkTreeIter *iter, __attribute__((unused)) GtkTreePath *path, Projet *projet)
{
	List_Gtk_1990	*list_gtk_1990 = projet->list_gtk._1990;
	GtkTreeModel	*model = gtk_tree_view_get_model(tree_view);
	int		ngroupe, tmp;
	char		*nom;
	Niveau_Groupe	*niveau_groupe;
	Groupe		*groupe;
	
	// On détermine le groupe ayant entraîné une ouverture
	// En effet, seul un groupe dans le tree_view_etat peut entraîner une ouverture
	gtk_tree_model_get(model, iter, 0, &ngroupe, 1, &tmp, 2, &nom, -1);
	if (_1990_groupe_niveau_positionne(projet->niveaux_groupes, gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(list_gtk_1990->spin_button_niveau))) != 0)
		BUG();
	niveau_groupe = list_curr(projet->niveaux_groupes);
	if (_1990_groupe_positionne(niveau_groupe, ngroupe) != 0)
		BUG();
	groupe = list_curr(niveau_groupe->groupes);
	
	// On le marque comme ouvert.
	groupe->pIter_expand = 1;
	
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
	List_Gtk_1990	*list_gtk_1990 = projet->list_gtk._1990;
	GtkTreeModel	*model = gtk_tree_view_get_model(tree_view);
	int		ngroupe, tmp;
	char		*nom;
	Niveau_Groupe	*niveau_groupe;
	Groupe		*groupe;
	
	// On détermine le groupe ayant entraîné une fermeture
	// En effet, seul un groupe dans le tree_view_etat peut entraîner une fermeture
	gtk_tree_model_get(model, iter, 0, &ngroupe, 1, &tmp, 2, &nom, -1);
	if (_1990_groupe_niveau_positionne(projet->niveaux_groupes, gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(list_gtk_1990->spin_button_niveau))) != 0)
		BUG();
	niveau_groupe = list_curr(projet->niveaux_groupes);
	if (_1990_groupe_positionne(niveau_groupe, ngroupe) != 0)
		BUG();
	groupe = list_curr(niveau_groupe->groupes);
	
	// On le marque comme fermé.
	groupe->pIter_expand = 0;
	
	return;
}

/* _1990_gtk_tree_view_etat_cursor_changed
 * Description : Evènement lorsqu'il y a un changement de ligne sélectionnée
 *             : Permet de mettre à jour si le groupe sélectionné est de type OR, XOR ou AND
 * Paramètres : GtkTreeView *tree_view : composant tree_view à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
void _1990_gtk_tree_view_etat_cursor_changed(GtkTreeView *tree_view, Projet *projet)
{
	List_Gtk_1990	*list_gtk_1990 = projet->list_gtk._1990;
	GtkTreeSelection *selection = gtk_tree_view_get_selection(tree_view);
	GtkTreeModel	*model;
	GtkTreeIter	iter;
	int		ngroupe, tmp;
	char		*nom;
	Niveau_Groupe	*niveau_groupe;
	Groupe		*groupe;
	
	// On détermine le groupe sélectionné
	if (!gtk_tree_selection_get_selected(selection, &model, &iter))
		return;
	gtk_tree_model_get(model, &iter, 0, &ngroupe, 1, &tmp, 2, &nom, -1);
	if (_1990_groupe_niveau_positionne(projet->niveaux_groupes, gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(list_gtk_1990->spin_button_niveau))) != 0)
		BUG();
	niveau_groupe = list_curr(projet->niveaux_groupes);
	if (_1990_groupe_positionne(niveau_groupe, ngroupe) != 0)
		BUG();
	groupe = list_curr(niveau_groupe->groupes);
	
	// On active le toggle_button correspondant au type de combinaison du groupe
	switch (groupe->type_combinaison)
	{
		case GROUPE_COMBINAISON_OR :
		{
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(list_gtk_1990->button_groupe_or), TRUE);
			break;
		}
		case GROUPE_COMBINAISON_XOR :
		{
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(list_gtk_1990->button_groupe_xor), TRUE);
			break;
		}
		case GROUPE_COMBINAISON_AND :
		{
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(list_gtk_1990->button_groupe_and), TRUE);
			break;
		}

	}
	return;
}

/* _1990_gtk_button_groupe_toggled
 * Description : Evènement lorsqu'il y a un changement de type de combinaison (OR, XOR ou AND)
 * Paramètres : GtkRadioButton *radiobutton : composant radiobutton à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
void _1990_gtk_button_groupe_toggled(GtkRadioButton *radiobutton, Projet *projet)
{
	List_Gtk_1990	*list_gtk_1990 = projet->list_gtk._1990;
	GtkTreeSelection *selection = gtk_tree_view_get_selection(list_gtk_1990->tree_view_etat);
	GtkTreeModel	*model;
	GtkTreeIter	iter;
	int		ngroupe, tmp;
	char		*nom;
	Niveau_Groupe	*niveau_groupe;
	Groupe		*groupe;
	
	// On détermine le groupe sélectionné
	if (!gtk_tree_selection_get_selected(selection, &model, &iter))
		return;
	gtk_tree_model_get(model, &iter, 0, &ngroupe, 1, &tmp, 2, &nom, -1);
	if (_1990_groupe_niveau_positionne(projet->niveaux_groupes, gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(list_gtk_1990->spin_button_niveau))) != 0)
		BUG();
	niveau_groupe = list_curr(projet->niveaux_groupes);
	if (_1990_groupe_positionne(niveau_groupe, ngroupe) != 0)
		BUG();
	groupe = list_curr(niveau_groupe->groupes);
	
	// On attribue le nouveau type de combinaison
	if (radiobutton == (void*)list_gtk_1990->button_groupe_and)
		groupe->type_combinaison = GROUPE_COMBINAISON_AND;
	else if (radiobutton == (void*)list_gtk_1990->button_groupe_or)
		groupe->type_combinaison = GROUPE_COMBINAISON_OR;
	else if (radiobutton == (void*)list_gtk_1990->button_groupe_xor)
		groupe->type_combinaison = GROUPE_COMBINAISON_XOR;
	
	return;
}

/* _1990_gtk_button_groupe_nom_clicked
 * Description : Change le nom du groupe en cours
 * Paramètres : GtkWidget *button : composant bouton à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
void _1990_gtk_button_groupe_nom_clicked(GtkWidget *button __attribute__((unused)), Projet *projet)
{
	List_Gtk_1990	*list_gtk_1990 = projet->list_gtk._1990;
	GtkTreeSelection *selection = gtk_tree_view_get_selection(list_gtk_1990->tree_view_etat);
	GtkTreeModel	*model;
	GtkTreeIter	iter;
	int		ngroupe, tmp;
	char		*nom;
	Niveau_Groupe	*niveau_groupe;
	Groupe		*groupe;
	const gchar	*sText;
	
	// On détermine le groupe sélectionné
	if (!gtk_tree_selection_get_selected(selection, &model, &iter))
		return;
	gtk_tree_model_get(model, &iter, 0, &ngroupe, 1, &tmp, 2, &nom, -1);
	if (_1990_groupe_niveau_positionne(projet->niveaux_groupes, gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(list_gtk_1990->spin_button_niveau))) != 0)
		BUG();
	niveau_groupe = list_curr(projet->niveaux_groupes);
	if (_1990_groupe_positionne(niveau_groupe, ngroupe) != 0)
		BUG();
	groupe = list_curr(niveau_groupe->groupes);
	
	// On attribue le nouveau type de combinaison
	sText = gtk_entry_get_text(GTK_ENTRY(list_gtk_1990->entry_groupe_nom));
	if (groupe->nom != NULL)
		free(groupe->nom);
	groupe->nom = malloc(sizeof(char)*(strlen(sText)+1));
	strcpy(groupe->nom, sText);
	gtk_tree_store_set(list_gtk_1990->tree_store_etat, groupe->pIter, 0, ngroupe, 1, -1, 2, groupe->nom, -1);
	
	return;
}

/* _1990_gtk_button_generer_clicked
 * Description : Génère les combinaisons
 * Paramètres : GtkWidget *button : composant bouton à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
void _1990_gtk_button_generer_clicked(GtkWidget *button __attribute__((unused)), Projet *projet)
{
	if (_1990_combinaisons_genere(projet) != 0)
		BUG();
	_1990_groupe_affiche_tout(projet);
	_1990_ponderations_affiche_tout(projet);
	return;
}

/* _1990_gtk_radio_button_eluequ_equ_seul
 * Description : Modifie les options de combinaison pour que l'ELU EQU calcule à l'équilibre seulement
 * Paramètres : GtkRadioButton *radiobutton : composant radiobutton à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
void _1990_gtk_radio_button_eluequ_equ_seul(GtkRadioButton *radiobutton __attribute__((unused)), Projet *projet)
{
	if ((projet->combinaisons.flags & 1) != 0)
		projet->combinaisons.flags ^= 1;
	return;
}

/* _1990_gtk_radio_button_eluequ_equ_resist
 * Description : Modifie les options de combinaison pour que l'ELU EQU calcule à l'équilibre et à la résistance structurelle
 * Paramètres : GtkRadioButton *radiobutton : composant radiobutton à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
void _1990_gtk_radio_button_eluequ_equ_resist(GtkRadioButton *radiobutton __attribute__((unused)), Projet *projet)
{
	if ((projet->combinaisons.flags & 1) == 0)
		projet->combinaisons.flags++;
	return;
}

/* _1990_gtk_radio_button_elustrgeo_1
 * Description : Modifie les options de combinaison pour que l'ELU STR/GEO calcule selon l'approche 1
 * Paramètres : GtkRadioButton *radiobutton : composant radiobutton à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
void _1990_gtk_radio_button_elustrgeo_1(GtkRadioButton *radiobutton __attribute__((unused)), Projet *projet)
{
	projet->combinaisons.flags = projet->combinaisons.flags - (projet->combinaisons.flags & 6);
	projet->combinaisons.flags = projet->combinaisons.flags + 0;
	return;
}

/* _1990_gtk_radio_button_elustrgeo_2
 * Description : Modifie les options de combinaison pour que l'ELU STR/GEO calcule selon l'approche 2
 * Paramètres : GtkRadioButton *radiobutton : composant radiobutton à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
void _1990_gtk_radio_button_elustrgeo_2(GtkRadioButton *radiobutton __attribute__((unused)), Projet *projet)
{
	projet->combinaisons.flags = projet->combinaisons.flags - (projet->combinaisons.flags & 6);
	projet->combinaisons.flags = projet->combinaisons.flags + 2;
	return;
}

/* _1990_gtk_radio_button_elustrgeo_3
 * Description : Modifie les options de combinaison pour que l'ELU STR/GEO calcule selon l'approche 3
 * Paramètres : GtkRadioButton *radiobutton : composant radiobutton à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
void _1990_gtk_radio_button_elustrgeo_3(GtkRadioButton *radiobutton __attribute__((unused)), Projet *projet)
{
	projet->combinaisons.flags = projet->combinaisons.flags - (projet->combinaisons.flags & 6);
	projet->combinaisons.flags = projet->combinaisons.flags + 4;
	return;
}

/* _1990_gtk_radio_button_elustrgeo_6_10
 * Description : Modifie les options de combinaison pour que l'ELU STR/GEO calcule selon la formule 6.10 de l'EN 1990
 * Paramètres : GtkRadioButton *radiobutton : composant radiobutton à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
void _1990_gtk_radio_button_elustrgeo_6_10(GtkRadioButton *radiobutton __attribute__((unused)), Projet *projet)
{
	projet->combinaisons.flags = projet->combinaisons.flags - (projet->combinaisons.flags & 8);
	projet->combinaisons.flags = projet->combinaisons.flags + 8;
	return;
}

/* _1990_gtk_radio_button_elustrgeo_6_10ab
 * Description : Modifie les options de combinaison pour que l'ELU STR/GEO calcule selon la formule 6.10 (a) et (b) de l'EN 1990
 * Paramètres : GtkRadioButton *radiobutton : composant radiobutton à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
void _1990_gtk_radio_button_elustrgeo_6_10ab(GtkRadioButton *radiobutton __attribute__((unused)), Projet *projet)
{
	projet->combinaisons.flags = projet->combinaisons.flags - (projet->combinaisons.flags & 8);
	projet->combinaisons.flags = projet->combinaisons.flags + 0;
	return;
}

/* _1990_gtk_radio_button_eluacc_frequente
 * Description : Modifie les options de combinaison pour que l'ELU ACC calcule avec les valeurs fréquentes des actions variables
 * Paramètres : GtkRadioButton *radiobutton : composant radiobutton à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
void _1990_gtk_radio_button_eluacc_frequente(GtkRadioButton *radiobutton __attribute__((unused)), Projet *projet)
{
	projet->combinaisons.flags = projet->combinaisons.flags - (projet->combinaisons.flags & 16);
	projet->combinaisons.flags = projet->combinaisons.flags + 0;
	return;
}

/* _1990_gtk_radio_button_eluacc_quasi_permanente
 * Description : Modifie les options de combinaison pour que l'ELU ACC calcule avec les valeurs quasi_permanente des actions variables
 * Paramètres : GtkRadioButton *radiobutton : composant radiobutton à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
void _1990_gtk_radio_button_eluacc_quasi_permanente(GtkRadioButton *radiobutton __attribute__((unused)), Projet *projet)
{
	projet->combinaisons.flags = projet->combinaisons.flags - (projet->combinaisons.flags & 16);
	projet->combinaisons.flags = projet->combinaisons.flags + 16;
	return;
}

/* _1990_gtk_tooltip
 * Description : Cette fonction doit obligatoirement être relié à l'évènement "query-tooltip" pour qu'apparaisse la fenêtre tooltip
 * Paramètres : GtkWidget *widget : composant à l'origine de l'évènement
 *            : gint x, gint y, gboolean keyboard_mode, GtkTooltip *tooltip : paramètre de l'évènement query-tooltip
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
void _1990_gtk_tooltip(GtkWidget *widget __attribute__((unused)), gint x __attribute__((unused)), gint y __attribute__((unused)), gboolean keyboard_mode __attribute__((unused)), GtkTooltip *tooltip __attribute__((unused)), gpointer user_data __attribute__((unused)))
{
	return;
}

/* _1990_gtk_window_quitter_button
 * Description : Bouton de fermeture de la fenêtre
 * Paramètres : GtkComboBox *widget : composant à l'origine de la demande
 *            : GtkWidget *fenêtre : la fenêtre d'options
 * Valeur renvoyée : Aucune
 */
void _1990_gtk_window_quitter_button(GtkWidget *object __attribute__((unused)), GtkWidget *fenetre)
{
	gtk_widget_destroy(fenetre);
	return;
}

/* _1990_gtk_button_options_clicked
 * Description : Affiche les différentes options
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
void _1990_gtk_button_options_clicked(GtkWidget *button __attribute__((unused)), Projet *projet)
{
	GtkWidget	*pWindow;
	GtkWidget	*vBox, *vBox_inside;
	GtkWidget	*frame;
	GtkWidget	*label;
	GtkWidget	*radio_button_maitre, *radio_button_esclave1, *radio_button_esclave2;
	GtkSettings	*settings;
	List_Gtk_1990	*list_gtk_1990 = projet->list_gtk._1990;
	
	// Définition de la fenêtre
	pWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_position(GTK_WINDOW(pWindow), GTK_WIN_POS_CENTER);
	gtk_window_set_default_size(GTK_WINDOW(pWindow), 600, -1);
	gtk_window_set_title(GTK_WINDOW(pWindow), "Options des combinaisons");
	
	vBox = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(pWindow), vBox);
	
	
	frame = gtk_frame_new("ELU Équilibre :");
	gtk_box_pack_start(GTK_BOX(vBox), frame, TRUE, TRUE, 0);
	
	
	// Définition des composants graphiques permettant la sélection entre le calcul à l'ELU EQU
	// équilibre seulement ou équilibre et résistance structurelle ensemble
	vBox_inside = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(frame), vBox_inside);
	label=gtk_label_new("A l'État Limite Ultime d'ÉQUilibre, il est possible de générer les combinaisons pour la vérification à l'équilibre statique seulement ou à l'équilibre statique incluant la résistance d'éléments structuraux.");
	gtk_label_set_line_wrap(GTK_LABEL(label), TRUE);
	gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_FILL);
	g_signal_connect(G_OBJECT(label), "size-allocate", G_CALLBACK(wrapped_label_size_allocate_callback), NULL);
	gtk_box_pack_start(GTK_BOX(vBox_inside), label, TRUE, FALSE, 0);
	
	radio_button_maitre = gtk_radio_button_new_with_label_from_widget(NULL, "Équilibre seulement");
	g_signal_connect(G_OBJECT(radio_button_maitre), "toggled", G_CALLBACK(_1990_gtk_radio_button_eluequ_equ_seul), projet);
	gtk_box_pack_start(GTK_BOX(vBox_inside), radio_button_maitre, TRUE, FALSE, 0);
	radio_button_esclave1 = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio_button_maitre), "Équilibre et résistance");
	g_signal_connect(G_OBJECT(radio_button_esclave1), "toggled", G_CALLBACK(_1990_gtk_radio_button_eluequ_equ_resist), projet);
	gtk_box_pack_start(GTK_BOX(vBox_inside), radio_button_esclave1, TRUE, FALSE, 0);
	if ((projet->combinaisons.flags & 1) == 0)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_button_maitre), TRUE);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_button_esclave1), TRUE);
	
	
	// Définition des composants graphiques permettant la sélection du calcul à l'ELU STR/GEO entre la formule 6.10 et 6.10(a) et (b)
	frame = gtk_frame_new("ELU STR et GEO, formule de combinaison :");
	gtk_box_pack_start(GTK_BOX(vBox), frame, TRUE, TRUE, 0);
	
	vBox_inside = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(frame), vBox_inside);
	label = gtk_label_new("À l'État Limite Ultime STRucture et GÉOtechnique, deux formules de combinaison sont possibles. La première est la formule 6.10 et la deuxième est 6.10a et 6.10b");
	gtk_label_set_line_wrap(GTK_LABEL(label), TRUE);
	gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_FILL);
	g_signal_connect(G_OBJECT(label), "size-allocate", G_CALLBACK(wrapped_label_size_allocate_callback), NULL);
	gtk_box_pack_start(GTK_BOX(vBox_inside), label, TRUE, FALSE, 0);
	
	radio_button_maitre = gtk_radio_button_new_with_label_from_widget(NULL, "6.10");
	g_signal_connect(G_OBJECT(radio_button_maitre), "toggled", G_CALLBACK(_1990_gtk_radio_button_elustrgeo_6_10), projet);
	gtk_widget_set_has_tooltip(radio_button_maitre, TRUE);
	g_signal_connect(radio_button_maitre, "query-tooltip", G_CALLBACK(_1990_gtk_tooltip), projet);
	gtk_widget_set_tooltip_window(radio_button_maitre, GTK_WINDOW(common_generation_tooltip("1990_6_10")));
	settings = gtk_widget_get_settings(GTK_WIDGET(radio_button_maitre));
	g_object_set(settings, "gtk-tooltip-timeout", 0, NULL);
	gtk_box_pack_start(GTK_BOX(vBox_inside), radio_button_maitre, TRUE, FALSE, 0);
	radio_button_esclave1 = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio_button_maitre), "6.10a et 6.10b");
	g_signal_connect(G_OBJECT(radio_button_esclave1), "toggled", G_CALLBACK(_1990_gtk_radio_button_elustrgeo_6_10ab), projet);
	gtk_widget_set_has_tooltip(radio_button_esclave1, TRUE);
	g_signal_connect(radio_button_esclave1, "query-tooltip", G_CALLBACK(_1990_gtk_tooltip), projet);
	gtk_widget_set_tooltip_window(radio_button_esclave1, GTK_WINDOW(common_generation_tooltip("1990_6_10a_b")));
	settings = gtk_widget_get_settings(GTK_WIDGET(radio_button_esclave1));
	g_object_set(settings, "gtk-tooltip-timeout", 0, NULL);
	gtk_box_pack_start(GTK_BOX(vBox_inside), radio_button_esclave1, TRUE, FALSE, 0);
	if ((projet->combinaisons.flags & 8) == 0)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_button_esclave1), TRUE);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_button_maitre), TRUE);
	
	
	// Définition des composants graphiques permettant la sélection du calcul à l'ELU STR/GEO approche 1, 2 ou 3
	frame = gtk_frame_new("ELU STR et GEO, ensemble de calcul :");
	gtk_box_pack_start(GTK_BOX(vBox), frame, TRUE, TRUE, 0);
	
	vBox_inside = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(frame), vBox_inside);
	label = gtk_label_new("Lorsqu'il est nécessaire de réaliser un calcul à l'État Limite Ultime STRucture et GÉOtechnique, trois approches sont possibles. Approche 1 : vérification de la structure et du sol via les coefficients de l'ensemble B puis les coefficients de l'ensemble C. Approche 2 : vérification de la structure et du sol via les coefficients de l'ensemble B. Approche 3 : vérification de la structure et du sol via les coefficients de l'ensemble B pour les actions géotechniques et les coefficients de l'ensemble C pour les actions appliquées à la structure.");
	gtk_label_set_line_wrap(GTK_LABEL(label), TRUE);
	gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_FILL);
	g_signal_connect(G_OBJECT(label), "size-allocate", G_CALLBACK(wrapped_label_size_allocate_callback), NULL);
	gtk_box_pack_start(GTK_BOX(vBox_inside), label, TRUE, FALSE, 0);
	
	radio_button_maitre = gtk_radio_button_new_with_label_from_widget(NULL, "Approche 1");
	g_signal_connect(G_OBJECT(radio_button_maitre), "toggled", G_CALLBACK(_1990_gtk_radio_button_elustrgeo_1), projet);
	gtk_box_pack_start(GTK_BOX(vBox_inside), radio_button_maitre, TRUE, FALSE, 0);
	radio_button_esclave1 = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio_button_maitre), "Approche 2");
	g_signal_connect(G_OBJECT(radio_button_esclave1), "toggled", G_CALLBACK(_1990_gtk_radio_button_elustrgeo_2), projet);
	gtk_box_pack_start(GTK_BOX(vBox_inside), radio_button_esclave1, TRUE, FALSE, 0);
	radio_button_esclave2 = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio_button_maitre), "Approche 3");
	g_signal_connect(G_OBJECT(radio_button_esclave2), "toggled", G_CALLBACK(_1990_gtk_radio_button_elustrgeo_3), projet);
	gtk_box_pack_start(GTK_BOX(vBox_inside), radio_button_esclave2, TRUE, FALSE, 0);
	if ((projet->combinaisons.flags & 6) == 4)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_button_esclave2), TRUE);
	else if ((projet->combinaisons.flags & 6) == 2)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_button_esclave1), TRUE);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_button_maitre), TRUE);
	
	
	// Définition des composants graphiques permettant la sélection du calcul à l'ELU ACC valeur fréquente ou quasi-permanente
	frame = gtk_frame_new("ELU ACCidentel :");
	gtk_box_pack_start(GTK_BOX(vBox), frame, TRUE, TRUE, 0);
	
	vBox_inside = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(frame), vBox_inside);
	label = gtk_label_new("À l'État Limite Ultime ACCidentel, la charge variable dominante utilisée est soit la valeur fréquente, soit la valeur quasi-permanente.");
	gtk_label_set_line_wrap(GTK_LABEL(label), TRUE);
	gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_FILL);
	g_signal_connect(G_OBJECT(label), "size-allocate", G_CALLBACK(wrapped_label_size_allocate_callback), NULL);
	gtk_box_pack_start(GTK_BOX(vBox_inside), label, TRUE, FALSE, 0);
	
	radio_button_maitre = gtk_radio_button_new_with_label_from_widget(NULL, "valeur fréquence");
	g_signal_connect(G_OBJECT(radio_button_maitre), "toggled", G_CALLBACK(_1990_gtk_radio_button_eluacc_frequente), projet);
	gtk_box_pack_start(GTK_BOX(vBox_inside), radio_button_maitre, TRUE, FALSE, 0);
	radio_button_esclave1 = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio_button_maitre), "valeur quasi-permanente");
	g_signal_connect(G_OBJECT(radio_button_esclave1), "toggled", G_CALLBACK(_1990_gtk_radio_button_eluacc_quasi_permanente), projet);
	gtk_box_pack_start(GTK_BOX(vBox_inside), radio_button_esclave1, TRUE, FALSE, 0);
	if ((projet->combinaisons.flags & 16) == 0)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_button_maitre), TRUE);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_button_esclave1), TRUE);
	
	
	// Affichage de la fenêtre
	gtk_window_set_transient_for(GTK_WINDOW(pWindow), GTK_WINDOW(list_gtk_1990->window_groupe));
	gtk_window_set_modal(GTK_WINDOW(pWindow), TRUE);
	gtk_widget_show_all(pWindow);
	
	return;
}

/* _1990_gtk_groupes
 * Description : Affichage de la fenêtre principale
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
void _1990_gtk_groupes(GtkWidget *button __attribute__((unused)), Projet *projet)
{
	GtkWidget	*label;
	Niveau_Groupe	*niveau;
	List_Gtk_1990	*list_gtk_1990;
	GtkCellRenderer	*pCellRenderer;
	GtkTreeViewColumn *pColumn;
	
	if (projet->list_gtk._1990 == NULL)
	{
		projet->list_gtk._1990 = malloc(sizeof(List_Gtk_1990));
		if (projet->list_gtk._1990 == NULL)
			BUGTEXTE(, gettext("Erreur d'allocation mémoire.\n"));
	}
	list_gtk_1990 = projet->list_gtk._1990;
	list_gtk_1990->window_groupe = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	list_gtk_1990->table_groupe = gtk_table_new (5, 4, FALSE);
	list_gtk_1990->table_combinaison = gtk_table_new (1, 1, FALSE);
	list_gtk_1990->table_groupes = gtk_table_new (7, 2, FALSE);
	list_gtk_1990->table_dispo = gtk_table_new (2, 2, FALSE);
	list_gtk_1990->table_bas = gtk_table_new (1, 3, FALSE);
	
	// Réglage de la fenêtre graphique
	gtk_window_set_title (GTK_WINDOW (list_gtk_1990->window_groupe), "Combinaisons");
	gtk_window_resize (GTK_WINDOW (list_gtk_1990->window_groupe), 600, 400);
	gtk_window_set_position (GTK_WINDOW (list_gtk_1990->window_groupe), GTK_WIN_POS_CENTER_ALWAYS);
	gtk_container_add(GTK_CONTAINER(list_gtk_1990->window_groupe), GTK_WIDGET(list_gtk_1990->table_groupe));
	
	if (projet->niveaux_groupes == NULL)
		BUGTEXTE(, gettext("Paramètres invalides.\n"));
	if (list_size(projet->niveaux_groupes) == 0)
	{
		if (_1990_groupe_niveau_ajout(projet) != 0)
			BUG();
	}
	
	// Création des composants graphiques gérant les niveaux
	list_gtk_1990->spin_button_niveau = gtk_spin_button_new_with_range(0, list_size(projet->niveaux_groupes)-1, 1);
	g_signal_connect (list_gtk_1990->spin_button_niveau, "value-changed", G_CALLBACK (_1990_gtk_spin_button_niveau_change), projet);
	
	list_gtk_1990->button_niveau_ajout = gtk_button_new_with_label(" + ");
	g_signal_connect(G_OBJECT(list_gtk_1990->button_niveau_ajout), "clicked", G_CALLBACK(_1990_gtk_button_niveau_ajout_clicked), projet);
	list_gtk_1990->button_niveau_suppr = gtk_button_new_with_label("  -  ");
	g_signal_connect(G_OBJECT(list_gtk_1990->button_niveau_suppr), "clicked", G_CALLBACK(_1990_gtk_button_niveau_suppr_clicked), projet);
	
	list_mvfront(projet->niveaux_groupes);
	niveau = list_curr(projet->niveaux_groupes);
	
	// Création des composants graphiques affichant l'état du niveau en cours
	list_gtk_1990->tree_store_etat = gtk_tree_store_new(3, G_TYPE_INT, G_TYPE_INT, G_TYPE_STRING);
	list_gtk_1990->tree_view_etat = (GtkTreeView*)gtk_tree_view_new_with_model(GTK_TREE_MODEL(list_gtk_1990->tree_store_etat));
	g_signal_connect(G_OBJECT(list_gtk_1990->tree_view_etat), "row-expanded", G_CALLBACK(_1990_gtk_tree_view_etat_row_expanded), projet);
	g_signal_connect(G_OBJECT(list_gtk_1990->tree_view_etat), "row-collapsed", G_CALLBACK(_1990_gtk_tree_view_etat_row_collapsed), projet);
	g_signal_connect(G_OBJECT(list_gtk_1990->tree_view_etat), "cursor-changed", G_CALLBACK(_1990_gtk_tree_view_etat_cursor_changed), projet);
	list_gtk_1990->tree_store_dispo = gtk_tree_store_new(2, G_TYPE_INT, G_TYPE_STRING);
	list_gtk_1990->tree_view_dispo = (GtkTreeView*)gtk_tree_view_new_with_model(GTK_TREE_MODEL(list_gtk_1990->tree_store_dispo));
	_1990_gtk_affiche_niveau(projet, gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(list_gtk_1990->spin_button_niveau)));
	
	gtk_table_attach (GTK_TABLE (list_gtk_1990->table_groupe), list_gtk_1990->spin_button_niveau, 1, 2, 0, 1, GTK_EXPAND | GTK_FILL, 0, 0, 0);
	gtk_table_attach (GTK_TABLE (list_gtk_1990->table_groupe), list_gtk_1990->button_niveau_ajout, 2, 3, 0, 1, GTK_FILL, 0, 0, 0);
	gtk_table_attach (GTK_TABLE (list_gtk_1990->table_groupe), list_gtk_1990->button_niveau_suppr, 3, 4, 0, 1, GTK_FILL, 0, 0, 0);
	label = gtk_label_new ("Niveau : ");
	gtk_table_attach (GTK_TABLE (list_gtk_1990->table_groupe), label, 0, 1, 0, 1, 0, 0, 0, 0);
	gtk_table_attach (GTK_TABLE (list_gtk_1990->table_groupe), list_gtk_1990->table_combinaison, 0, 4, 2, 3, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
	
	list_gtk_1990->frame_groupe = gtk_frame_new("Groupes");
	gtk_table_attach (GTK_TABLE (list_gtk_1990->table_combinaison), list_gtk_1990->frame_groupe, 0, 1, 0, 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
	gtk_container_add(GTK_CONTAINER(list_gtk_1990->frame_groupe), list_gtk_1990->table_groupes);
	pCellRenderer = gtk_cell_renderer_text_new();
	pColumn = gtk_tree_view_column_new_with_attributes("Groupe", pCellRenderer, "text", 0, NULL);
	gtk_tree_view_append_column(list_gtk_1990->tree_view_etat, pColumn);
	pCellRenderer = gtk_cell_renderer_text_new();
	pColumn = gtk_tree_view_column_new_with_attributes("Element", pCellRenderer, "text", 1, NULL);
	gtk_tree_view_append_column(list_gtk_1990->tree_view_etat, pColumn);
	pCellRenderer = gtk_cell_renderer_text_new();
	pColumn = gtk_tree_view_column_new_with_attributes("Descrition", pCellRenderer, "text", 2, NULL);
	gtk_tree_view_append_column(list_gtk_1990->tree_view_etat, pColumn);
	gtk_table_attach (GTK_TABLE (list_gtk_1990->table_groupes), GTK_WIDGET(list_gtk_1990->tree_view_etat), 0, 1, 0, 6, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
	
	// Composants permettant la suppression / ajout des groupes, gestion des éléments disponibles et du type de combinaison du groupe en cours
	list_gtk_1990->button_groupe_ajout = gtk_button_new_with_label(" + ");
	g_signal_connect(G_OBJECT(list_gtk_1990->button_groupe_ajout), "clicked", G_CALLBACK(_1990_gtk_button_groupe_ajout_clicked), projet);
	list_gtk_1990->button_groupe_suppr = gtk_button_new_with_label("-");
	g_signal_connect(G_OBJECT(list_gtk_1990->button_groupe_suppr), "clicked", G_CALLBACK(_1990_gtk_button_groupe_suppr_clicked), projet);
	list_gtk_1990->button_ajout_dispo = gtk_button_new_with_label("<-");
	g_signal_connect(G_OBJECT(list_gtk_1990->button_ajout_dispo), "clicked", G_CALLBACK(_1990_gtk_button_ajout_dispo_clicked), projet);
	list_gtk_1990->button_groupe_and = gtk_radio_button_new_with_label_from_widget(NULL, "AND");
	g_signal_connect(G_OBJECT(list_gtk_1990->button_groupe_and), "toggled", G_CALLBACK(_1990_gtk_button_groupe_toggled), projet);
	list_gtk_1990->button_groupe_or = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(list_gtk_1990->button_groupe_and), "OR");
	g_signal_connect(G_OBJECT(list_gtk_1990->button_groupe_or), "toggled", G_CALLBACK(_1990_gtk_button_groupe_toggled), projet);
	list_gtk_1990->button_groupe_xor = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(list_gtk_1990->button_groupe_and), "XOR");
	g_signal_connect(G_OBJECT(list_gtk_1990->button_groupe_xor), "toggled", G_CALLBACK(_1990_gtk_button_groupe_toggled), projet);
	list_gtk_1990->button_groupe_nom = gtk_button_new_with_label("OK");
	g_signal_connect(G_OBJECT(list_gtk_1990->button_groupe_nom), "clicked", G_CALLBACK(_1990_gtk_button_groupe_nom_clicked), projet);
	list_gtk_1990->entry_groupe_nom = gtk_entry_new();
	gtk_table_attach (GTK_TABLE (list_gtk_1990->table_groupes), list_gtk_1990->button_groupe_ajout, 1, 2, 0, 1, GTK_FILL, 0, 0, 0);
	gtk_table_attach (GTK_TABLE (list_gtk_1990->table_groupes), list_gtk_1990->button_groupe_suppr, 1, 2, 1, 2, GTK_FILL, 0, 0, 0);
	gtk_table_attach (GTK_TABLE (list_gtk_1990->table_groupes), list_gtk_1990->button_ajout_dispo, 1, 2, 2, 3, GTK_FILL, 0, 0, 0);
	gtk_table_attach (GTK_TABLE (list_gtk_1990->table_groupes), list_gtk_1990->button_groupe_and, 1, 2, 3, 4, GTK_FILL, 0, 0, 0);
	gtk_table_attach (GTK_TABLE (list_gtk_1990->table_groupes), list_gtk_1990->button_groupe_or, 1, 2, 4, 5, GTK_FILL, 0, 0, 0);
	gtk_table_attach (GTK_TABLE (list_gtk_1990->table_groupes), list_gtk_1990->button_groupe_xor, 1, 2, 5, 6, GTK_FILL, 0, 0, 0);
	gtk_table_attach (GTK_TABLE (list_gtk_1990->table_groupes), list_gtk_1990->button_groupe_nom, 1, 2, 6, 7, GTK_FILL, 0, 0, 0);
	gtk_table_attach (GTK_TABLE (list_gtk_1990->table_groupes), list_gtk_1990->entry_groupe_nom, 0, 1, 6, 7, GTK_FILL, 0, 0, 0);
	
	// Composants gérant les éléments disponibles
	list_gtk_1990->frame_dispo = gtk_frame_new("Disponibles :");
	gtk_table_attach (GTK_TABLE (list_gtk_1990->table_combinaison), list_gtk_1990->frame_dispo, 1, 2, 0, 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
	gtk_container_add(GTK_CONTAINER(list_gtk_1990->frame_dispo), list_gtk_1990->table_dispo);
	
	pCellRenderer = gtk_cell_renderer_text_new();
	pColumn = gtk_tree_view_column_new_with_attributes("Disponibles", pCellRenderer, "text", 0, NULL);
	gtk_tree_view_append_column(list_gtk_1990->tree_view_dispo, pColumn);
	pCellRenderer = gtk_cell_renderer_text_new();
	pColumn = gtk_tree_view_column_new_with_attributes("Descrition", pCellRenderer, "text", 1, NULL);
	gtk_tree_view_append_column(list_gtk_1990->tree_view_dispo, pColumn);
	gtk_table_attach (GTK_TABLE (list_gtk_1990->table_dispo), GTK_WIDGET(list_gtk_1990->tree_view_dispo), 0, 1, 0, 2, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
	
	// Composants gérant les options et la génération des combinaisons
	gtk_table_attach (GTK_TABLE (list_gtk_1990->table_groupe), list_gtk_1990->table_bas, 0, 4, 3, 4, GTK_EXPAND | GTK_FILL, 0, 0, 0);
	list_gtk_1990->button_generer = gtk_button_new_with_label("Générer");
	gtk_table_attach (GTK_TABLE (list_gtk_1990->table_bas), list_gtk_1990->button_generer, 0, 1, 0, 1, GTK_EXPAND | GTK_FILL, 0, 0, 0);
	g_signal_connect(G_OBJECT(list_gtk_1990->button_generer), "clicked", G_CALLBACK(_1990_gtk_button_generer_clicked), projet);
	list_gtk_1990->button_options = gtk_button_new_with_label("Options");
	gtk_table_attach (GTK_TABLE (list_gtk_1990->table_bas), list_gtk_1990->button_options, 1, 2, 0, 1, GTK_EXPAND | GTK_FILL, 0, 0, 0);
	g_signal_connect(G_OBJECT(list_gtk_1990->button_options), "clicked", G_CALLBACK(_1990_gtk_button_options_clicked), projet);
	list_gtk_1990->button_quitter = gtk_button_new_with_label("Quitter");
	gtk_table_attach (GTK_TABLE (list_gtk_1990->table_bas), list_gtk_1990->button_quitter, 2, 3, 0, 1, GTK_EXPAND | GTK_FILL, 0, 0, 0);
	g_signal_connect(G_OBJECT(list_gtk_1990->button_quitter), "clicked", G_CALLBACK(_1990_gtk_window_quitter_button), list_gtk_1990->window_groupe);
	
	gtk_window_set_modal(GTK_WINDOW(list_gtk_1990->window_groupe), TRUE);
	gtk_widget_show_all(list_gtk_1990->window_groupe);
	
	return;
}
#endif
