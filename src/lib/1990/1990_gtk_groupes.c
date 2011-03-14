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
#include "common_projet.h"
#include "1990_groupes.h"
#include "1990_gtk_groupes.h"
#include "common_erreurs.h"
#include <gtk/gtk.h>

int _1990_gtk_affiche_niveau(Projet *projet, int niveau)
{
	Groupe		*groupe;
	Element		*element;
	Niveau_Groupe	*niveau_groupe;
	int		dispo_max, i, *dispos;
	List_Gtk_1990	*list_gtk_1990 = projet->list_gtk._1990;
	GtkTreeIter	Iter;
	GtkTreePath	*path;
	
	gtk_tree_store_clear(list_gtk_1990->tree_store_etat);
	gtk_tree_store_clear(list_gtk_1990->tree_store_dispo);
	if (niveau == 0)
		dispo_max = list_size(projet->actions);
	else
	{
		if (_1990_groupe_niveau_traverse_et_positionne(projet->niveaux_groupes, niveau-1) != 0)
			BUG(-1);
		niveau_groupe = list_curr(projet->niveaux_groupes);
		dispo_max = list_size(niveau_groupe->groupes);
	}
	if (dispo_max != 0)
	{
		dispos = malloc(sizeof(int)*dispo_max);
		if (dispos == NULL)
			BUG(-2);
		for (i=0;i<dispo_max;i++)
			dispos[i] = 0;
	}
	else
		dispos = NULL;
	if (_1990_groupe_niveau_traverse_et_positionne(projet->niveaux_groupes, niveau) != 0)
		BUG(-1);
	niveau_groupe = list_curr(projet->niveaux_groupes);
	if (niveau_groupe == NULL)
		BUG(-1);
	if (list_size(niveau_groupe->groupes) == 0)
		return -1;
	list_mvfront(niveau_groupe->groupes);
	do
	{
		groupe = list_curr(niveau_groupe->groupes);
		if (groupe->pIter == NULL)
		{
			groupe->pIter = malloc(sizeof(GtkTreeIter));
		}
		gtk_tree_store_append(list_gtk_1990->tree_store_etat, groupe->pIter, NULL);
		gtk_tree_store_set(list_gtk_1990->tree_store_etat, groupe->pIter, 0, groupe->numero, 1, -1, 2, groupe->nom, -1);
		if ((groupe->elements != NULL) && (list_size(groupe->elements) != 0))
		{
			list_mvfront(groupe->elements);
			do
			{
				element = list_curr(groupe->elements);
				dispos[element->numero] = 1;
				if (element->pIter == NULL)
				{
					element->pIter = malloc(sizeof(GtkTreeIter));
				}
				gtk_tree_store_append(list_gtk_1990->tree_store_etat, element->pIter, groupe->pIter);
				gtk_tree_store_set(list_gtk_1990->tree_store_etat, element->pIter, 0, groupe->numero, 1, element->numero, 2, "", -1);
			}
			while (list_mvnext(groupe->elements));
		}
		if (groupe->pIter_expand == 1)
		{
			path = gtk_tree_model_get_path(GTK_TREE_MODEL(list_gtk_1990->tree_store_etat), groupe->pIter);
			gtk_tree_view_expand_row(list_gtk_1990->tree_view_etat, path, FALSE);
			gtk_tree_path_free(path);
		}
	}
	while (list_mvnext(niveau_groupe->groupes) != NULL);
	for (i=0;i<dispo_max;i++)
	{
		if (dispos[i] == 0)
		{
			gtk_tree_store_append(list_gtk_1990->tree_store_dispo, &Iter, NULL);
			gtk_tree_store_set(list_gtk_1990->tree_store_dispo, &Iter, 0, i, 1, "", -1);
		}
	}
	if (dispos != NULL)
		free(dispos);
	return 0;
}

int _1990_gtk_spin_button_niveau_change(GtkWidget *button __attribute__((unused)), Projet *projet)
{
	List_Gtk_1990	*list_gtk_1990 = projet->list_gtk._1990;
	return _1990_gtk_affiche_niveau(projet, gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(list_gtk_1990->spin_button_niveau)));
}

int _1990_gtk_button_niveau_suppr_clicked(GtkWidget *button __attribute__((unused)), Projet *projet)
{
	List_Gtk_1990	*list_gtk_1990 = projet->list_gtk._1990;
	if (_1990_groupe_free_niveau_numero(projet, gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(list_gtk_1990->spin_button_niveau))) != 0)
		BUG(-1);
	if (list_size(projet->niveaux_groupes) == 0)
	{
		if (_1990_groupe_niveau_ajout(projet, 0) != 0)
			BUG(-2);
		gtk_tree_store_clear(list_gtk_1990->tree_store_etat);
	}
	gtk_spin_button_set_range(GTK_SPIN_BUTTON(list_gtk_1990->spin_button_niveau), 0, list_size(projet->niveaux_groupes)-1);
	return 0;
}

int _1990_gtk_button_niveau_ajout_clicked(GtkWidget *button __attribute__((unused)), Projet *projet)
{
	List_Gtk_1990	*list_gtk_1990 = projet->list_gtk._1990;
	if (_1990_groupe_niveau_ajout(projet, list_size(projet->niveaux_groupes)) != 0)
		BUG(-1);
	gtk_tree_store_clear(list_gtk_1990->tree_store_etat);
	gtk_spin_button_set_range(GTK_SPIN_BUTTON(list_gtk_1990->spin_button_niveau), 0, list_size(projet->niveaux_groupes)-1);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(list_gtk_1990->spin_button_niveau), list_size(projet->niveaux_groupes)-1);
	return 0;
}

int _1990_gtk_button_groupe_ajout_clicked(GtkWidget *button __attribute__((unused)), Projet *projet)
{
	List_Gtk_1990	*list_gtk_1990 = projet->list_gtk._1990;
	Niveau_Groupe	*niveau_groupe = list_curr(projet->niveaux_groupes);
	if (_1990_groupe_ajout(projet, gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(list_gtk_1990->spin_button_niveau)), list_size(niveau_groupe->groupes), GROUPE_COMBINAISON_AND) != 0)
		BUG(-1);
	if (_1990_gtk_affiche_niveau(projet, gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(list_gtk_1990->spin_button_niveau))) != 0)
		BUG(-2);
	return 0;
}

int _1990_gtk_insert_dispo(Projet *projet, int numero)
{
	List_Gtk_1990	*list_gtk_1990 = projet->list_gtk._1990;
	gboolean	retour;
	GtkTreeModel	*model;
	GtkTreeIter	iter, iter2;
	int		nombre;
	char		*nom;
	
	retour = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(list_gtk_1990->tree_store_dispo), &iter2);
	model = gtk_tree_view_get_model(list_gtk_1990->tree_view_dispo);
	if (retour == TRUE)
		gtk_tree_model_get(model, &iter2, 0, &nombre, 1, &nom, -1);
	while ((retour == TRUE) && (nombre < numero))
	{
		retour = gtk_tree_model_iter_next(GTK_TREE_MODEL(list_gtk_1990->tree_store_dispo), &iter2);
		if (retour == TRUE)
			gtk_tree_model_get(model, &iter2, 0, &nombre, 1, &nom, -1);
	}
	if (retour == FALSE)
		gtk_tree_store_append(list_gtk_1990->tree_store_dispo, &iter, NULL);
	else
		gtk_tree_store_insert_before(list_gtk_1990->tree_store_dispo, &iter, NULL, &iter2);
	gtk_tree_store_set(list_gtk_1990->tree_store_dispo, &iter, 0, numero, 1, "", -1);
	return 0;
}

int _1990_gtk_button_ajout_dispo_clicked(GtkWidget *button __attribute__((unused)), Projet *projet)
{
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
	
	if ((selection1 == NULL) || (selection2 == NULL))
		return 0;
	if ((!gtk_tree_selection_get_selected(selection1, &model1, &iter1)) || (!gtk_tree_selection_get_selected(selection2, &model2, &iter2)))
		return 0;
	gtk_tree_model_get(model1, &iter1, 0, &numero, 1, &nom, -1);
	gtk_tree_model_get(model2, &iter2, 0, &ngroupe, 1, &tmp, 2, &nom, -1);
	gtk_tree_store_remove(list_gtk_1990->tree_store_dispo, &iter1);
	if (_1990_groupe_ajout_element(projet, gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(list_gtk_1990->spin_button_niveau)), ngroupe, numero) != 0)
		BUG(-1);
	niveau_groupe = list_curr(projet->niveaux_groupes);
	groupe = list_curr(niveau_groupe->groupes);
	_1990_groupe_element_traverse_et_positionne(groupe, numero);
	element = list_curr(groupe->elements);
	if (element->pIter == NULL)
	{
		element->pIter = malloc(sizeof(GtkTreeIter));
		if (element->pIter == NULL)
			BUG(-2);
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
	return 0;
}

int _1990_gtk_button_groupe_suppr_clicked(GtkWidget *button __attribute__((unused)), Projet *projet)
{
	List_Gtk_1990	*list_gtk_1990 = projet->list_gtk._1990;
	GtkTreeSelection *selection = gtk_tree_view_get_selection(list_gtk_1990->tree_view_etat);
	GtkTreeModel	*model;
	GtkTreeIter	iter;
	int		niveau, ngroupe, numero;
	char		*nom;
	Niveau_Groupe	*niveau_groupe;
	Groupe		*groupe;
	Element		*element;
	GValue		nouvelle_valeur;
	memset(&nouvelle_valeur, 0, sizeof(nouvelle_valeur));
	
	if (selection == NULL)
		return 0;
	if (!gtk_tree_selection_get_selected(selection, &model, &iter))
		return 0;
	gtk_tree_model_get(model, &iter, 0, &ngroupe, 1, &numero, 2, &nom, -1);
	gtk_tree_store_remove(list_gtk_1990->tree_store_etat, &iter);
	niveau = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(list_gtk_1990->spin_button_niveau));
	if (numero == -1)
	{
		g_type_init();
		g_value_init (&nouvelle_valeur, G_TYPE_INT);

		if (_1990_groupe_niveau_traverse_et_positionne(projet->niveaux_groupes, niveau) != 0)
			BUG(-2);
		niveau_groupe = list_curr(projet->niveaux_groupes);
		if (_1990_groupe_traverse_et_positionne(niveau_groupe, ngroupe) != 0)
			BUG(-3);
		groupe = list_curr(niveau_groupe->groupes);
		if (list_size(groupe->elements) != 0)
		{
			list_mvfront(groupe->elements);
			do
			{
				element = list_curr(groupe->elements);
				_1990_gtk_insert_dispo(projet, element->numero);
			}
			while (list_mvnext(groupe->elements));
		}
		if (_1990_groupe_free_groupe_numero(projet, niveau, ngroupe) != 0)
			BUG(-1);
		
		if (list_curr(niveau_groupe->groupes) != NULL)
		{
			list_mvfront(niveau_groupe->groupes);
			do
			{
				groupe = list_curr(niveau_groupe->groupes);
				if (groupe->numero >= ngroupe)
				{
					g_value_set_int (&nouvelle_valeur, groupe->numero);
					gtk_tree_store_set_value(list_gtk_1990->tree_store_etat, groupe->pIter, 0, &nouvelle_valeur);
					if ((groupe->elements != NULL) && (list_size(groupe->elements) != 0))
					{
						list_mvfront(groupe->elements);
						do
						{
							element = list_curr(groupe->elements);
							if (element->pIter == NULL)
							{
								element->pIter = malloc(sizeof(GtkTreeIter));
								if (element->pIter == NULL)
									BUG(-2);
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
	else
	{
		_1990_gtk_insert_dispo(projet, numero);
		if (_1990_groupe_free_element_numero(projet, gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(list_gtk_1990->spin_button_niveau)), ngroupe, numero) != 0)
			BUG(-2);
	}
	return 0;
}

int _1990_gtk_tree_view_etat_row_expanded(GtkTreeView *tree_view, GtkTreeIter *iter, __attribute__((unused)) GtkTreePath *path, Projet *projet)
{
	List_Gtk_1990	*list_gtk_1990 = projet->list_gtk._1990;
	GtkTreeModel	*model = gtk_tree_view_get_model(tree_view);
	int		ngroupe, tmp;
	char		*nom;
	Niveau_Groupe	*niveau_groupe;
	Groupe		*groupe;
	
	gtk_tree_model_get(model, iter, 0, &ngroupe, 1, &tmp, 2, &nom, -1);
	if (_1990_groupe_niveau_traverse_et_positionne(projet->niveaux_groupes, gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(list_gtk_1990->spin_button_niveau))) != 0)
		BUG(-1);
	niveau_groupe = list_curr(projet->niveaux_groupes);
	if (_1990_groupe_traverse_et_positionne(niveau_groupe, ngroupe) != 0)
		BUG(-2);
	groupe = list_curr(niveau_groupe->groupes);
	groupe->pIter_expand = 1;
	
	return 0;
}

int _1990_gtk_tree_view_etat_row_collapsed(GtkTreeView *tree_view, GtkTreeIter *iter, __attribute__((unused)) GtkTreePath *path, Projet *projet)
{
	List_Gtk_1990	*list_gtk_1990 = projet->list_gtk._1990;
	GtkTreeModel	*model = gtk_tree_view_get_model(tree_view);
	int		ngroupe, tmp;
	char		*nom;
	Niveau_Groupe	*niveau_groupe;
	Groupe		*groupe;
	
	gtk_tree_model_get(model, iter, 0, &ngroupe, 1, &tmp, 2, &nom, -1);
	if (_1990_groupe_niveau_traverse_et_positionne(projet->niveaux_groupes, gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(list_gtk_1990->spin_button_niveau))) != 0)
		BUG(-1);
	niveau_groupe = list_curr(projet->niveaux_groupes);
	if (_1990_groupe_traverse_et_positionne(niveau_groupe, ngroupe) != 0)
		BUG(-2);
	groupe = list_curr(niveau_groupe->groupes);
	groupe->pIter_expand = 0;
	
	return 0;
}

int _1990_gtk_tree_view_etat_cursor_changed(GtkTreeView *tree_view, Projet *projet)
{
	List_Gtk_1990	*list_gtk_1990 = projet->list_gtk._1990;
	GtkTreeSelection *selection = gtk_tree_view_get_selection(tree_view);
	GtkTreeModel	*model;
	GtkTreeIter	iter;
	int		ngroupe, tmp;
	char		*nom;
	Niveau_Groupe	*niveau_groupe;
	Groupe		*groupe;

	if (!gtk_tree_selection_get_selected(selection, &model, &iter))
		return 0;
	gtk_tree_model_get(model, &iter, 0, &ngroupe, 1, &tmp, 2, &nom, -1);
	if (_1990_groupe_niveau_traverse_et_positionne(projet->niveaux_groupes, gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(list_gtk_1990->spin_button_niveau))) != 0)
		BUG(-1);
	niveau_groupe = list_curr(projet->niveaux_groupes);
	if (_1990_groupe_traverse_et_positionne(niveau_groupe, ngroupe) != 0)
		BUG(-2);
	groupe = list_curr(niveau_groupe->groupes);
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
	return 0;
}

int _1990_gtk_button_groupe_toggled(GtkRadioButton *radiobutton, Projet *projet)
{
	List_Gtk_1990	*list_gtk_1990 = projet->list_gtk._1990;
	GtkTreeSelection *selection = gtk_tree_view_get_selection(list_gtk_1990->tree_view_etat);
	GtkTreeModel	*model;
	GtkTreeIter	iter;
	int		ngroupe, tmp;
	char		*nom;
	Niveau_Groupe	*niveau_groupe;
	Groupe		*groupe;
	
	if (!gtk_tree_selection_get_selected(selection, &model, &iter))
		return 0;
	gtk_tree_model_get(model, &iter, 0, &ngroupe, 1, &tmp, 2, &nom, -1);
	if (_1990_groupe_niveau_traverse_et_positionne(projet->niveaux_groupes, gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(list_gtk_1990->spin_button_niveau))) != 0)
		BUG(-1);
	niveau_groupe = list_curr(projet->niveaux_groupes);
	if (_1990_groupe_traverse_et_positionne(niveau_groupe, ngroupe) != 0)
		BUG(-2);
	groupe = list_curr(niveau_groupe->groupes);
	if (radiobutton == (void*)list_gtk_1990->button_groupe_and)
		groupe->type_combinaison = GROUPE_COMBINAISON_AND;
	else if (radiobutton == (void*)list_gtk_1990->button_groupe_or)
		groupe->type_combinaison = GROUPE_COMBINAISON_OR;
	else if (radiobutton == (void*)list_gtk_1990->button_groupe_xor)
		groupe->type_combinaison = GROUPE_COMBINAISON_XOR;
	else
		BUG(-1);
	return 0;
}

int _1990_gtk_button_groupe_ok_clicked(GtkWidget *button __attribute__((unused)), Projet *projet)
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
	
	if (!gtk_tree_selection_get_selected(selection, &model, &iter))
		return 0;
	gtk_tree_model_get(model, &iter, 0, &ngroupe, 1, &tmp, 2, &nom, -1);
	if (_1990_groupe_niveau_traverse_et_positionne(projet->niveaux_groupes, gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(list_gtk_1990->spin_button_niveau))) != 0)
		BUG(-1);
	niveau_groupe = list_curr(projet->niveaux_groupes);
	if (_1990_groupe_traverse_et_positionne(niveau_groupe, ngroupe) != 0)
		BUG(-2);
	groupe = list_curr(niveau_groupe->groupes);
	sText = gtk_entry_get_text(GTK_ENTRY(list_gtk_1990->entry_groupe_nom));
	if (groupe->nom != NULL)
		free(groupe->nom);
	groupe->nom = malloc(sizeof(char)*(strlen(sText)+1));
	strcpy(groupe->nom, sText);
	gtk_tree_store_set(list_gtk_1990->tree_store_etat, groupe->pIter, 0, ngroupe, 1, -1, 2, groupe->nom, -1);
	return 0;
}

int _1990_gtk_groupes(GtkWidget *button __attribute__((unused)), Projet *projet)
{
	GtkWidget	*label;
	Niveau_Groupe	*niveau;
	List_Gtk_1990	*list_gtk_1990;
	
	if (projet->list_gtk._1990 == NULL)
		projet->list_gtk._1990 = malloc(sizeof(List_Gtk_1990));
	if (projet->list_gtk._1990 == NULL)
		BUG(-2);
	list_gtk_1990 = projet->list_gtk._1990;
	list_gtk_1990->window_groupe = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	list_gtk_1990->table_groupe = gtk_table_new (2, 4, FALSE);
	list_gtk_1990->table_combinaison = gtk_table_new (1, 1, FALSE);
	list_gtk_1990->table_groupes = gtk_table_new (7, 2, FALSE);
	list_gtk_1990->table_dispo = gtk_table_new (2, 2, FALSE);
	
	if ((projet->niveaux_groupes == NULL) || (list_size(projet->niveaux_groupes) == 0))
	{
		if (_1990_groupe_niveau_ajout(projet, 0) != 0)
			BUG(-1);
	}
	
	list_gtk_1990->spin_button_niveau = gtk_spin_button_new_with_range(0, list_size(projet->niveaux_groupes)-1, 1);
	g_signal_connect (list_gtk_1990->spin_button_niveau, "value-changed", G_CALLBACK (_1990_gtk_spin_button_niveau_change), projet);
	
	list_gtk_1990->button_niveau_ajout = gtk_button_new_with_label(" + ");
	g_signal_connect(G_OBJECT(list_gtk_1990->button_niveau_ajout), "clicked", G_CALLBACK(_1990_gtk_button_niveau_ajout_clicked), projet);
	list_gtk_1990->button_niveau_suppr = gtk_button_new_with_label("  -  ");
	g_signal_connect(G_OBJECT(list_gtk_1990->button_niveau_suppr), "clicked", G_CALLBACK(_1990_gtk_button_niveau_suppr_clicked), projet);
	
	list_mvfront(projet->niveaux_groupes);
	niveau = list_curr(projet->niveaux_groupes);
	
	GtkCellRenderer	*pCellRenderer;
	GtkTreeViewColumn *pColumn;
	
	list_gtk_1990->tree_store_etat = gtk_tree_store_new(3, G_TYPE_INT, G_TYPE_INT, G_TYPE_STRING);
	list_gtk_1990->tree_view_etat = (GtkTreeView*)gtk_tree_view_new_with_model(GTK_TREE_MODEL(list_gtk_1990->tree_store_etat));
	g_signal_connect(G_OBJECT(list_gtk_1990->tree_view_etat), "row-expanded", G_CALLBACK(_1990_gtk_tree_view_etat_row_expanded), projet);
	g_signal_connect(G_OBJECT(list_gtk_1990->tree_view_etat), "row-collapsed", G_CALLBACK(_1990_gtk_tree_view_etat_row_collapsed), projet);
	g_signal_connect(G_OBJECT(list_gtk_1990->tree_view_etat), "cursor-changed", G_CALLBACK(_1990_gtk_tree_view_etat_cursor_changed), projet);
	list_gtk_1990->tree_store_dispo = gtk_tree_store_new(2, G_TYPE_INT, G_TYPE_STRING);
	list_gtk_1990->tree_view_dispo = (GtkTreeView*)gtk_tree_view_new_with_model(GTK_TREE_MODEL(list_gtk_1990->tree_store_dispo));
	
	_1990_gtk_affiche_niveau(projet, gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(list_gtk_1990->spin_button_niveau)));
	
	gtk_window_set_title (GTK_WINDOW (list_gtk_1990->window_groupe), "Noeuds");
	gtk_window_resize (GTK_WINDOW (list_gtk_1990->window_groupe), 600, 400);
	gtk_window_set_position (GTK_WINDOW (list_gtk_1990->window_groupe), GTK_WIN_POS_CENTER_ALWAYS);
	
	gtk_container_add(GTK_CONTAINER(list_gtk_1990->window_groupe), GTK_WIDGET(list_gtk_1990->table_groupe));
	
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
	list_gtk_1990->button_groupe_ok = gtk_button_new_with_label("OK");
	g_signal_connect(G_OBJECT(list_gtk_1990->button_groupe_ok), "clicked", G_CALLBACK(_1990_gtk_button_groupe_ok_clicked), projet);
	list_gtk_1990->entry_groupe_nom = gtk_entry_new();
	gtk_table_attach (GTK_TABLE (list_gtk_1990->table_groupes), list_gtk_1990->button_groupe_ajout, 1, 2, 0, 1, GTK_FILL, 0, 0, 0);
	gtk_table_attach (GTK_TABLE (list_gtk_1990->table_groupes), list_gtk_1990->button_groupe_suppr, 1, 2, 1, 2, GTK_FILL, 0, 0, 0);
	gtk_table_attach (GTK_TABLE (list_gtk_1990->table_groupes), list_gtk_1990->button_ajout_dispo, 1, 2, 2, 3, GTK_FILL, 0, 0, 0);
	gtk_table_attach (GTK_TABLE (list_gtk_1990->table_groupes), list_gtk_1990->button_groupe_and, 1, 2, 3, 4, GTK_FILL, 0, 0, 0);
	gtk_table_attach (GTK_TABLE (list_gtk_1990->table_groupes), list_gtk_1990->button_groupe_or, 1, 2, 4, 5, GTK_FILL, 0, 0, 0);
	gtk_table_attach (GTK_TABLE (list_gtk_1990->table_groupes), list_gtk_1990->button_groupe_xor, 1, 2, 5, 6, GTK_FILL, 0, 0, 0);
	gtk_table_attach (GTK_TABLE (list_gtk_1990->table_groupes), list_gtk_1990->button_groupe_ok, 1, 2, 6, 7, GTK_FILL, 0, 0, 0);
	gtk_table_attach (GTK_TABLE (list_gtk_1990->table_groupes), list_gtk_1990->entry_groupe_nom, 0, 1, 6, 7, GTK_FILL, 0, 0, 0);
	
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
	
	
	gtk_widget_show_all(list_gtk_1990->window_groupe);
	
	return 0;
}
#endif
