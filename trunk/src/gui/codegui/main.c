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
#include "common_text.h"
#include "common_erreurs.h"
#include "common_projet.h"
#include "1990_gtk_groupes.h"
#include "1990_groupes.h"
#include "1990_actions.h"
#include "1992_1_1_elements.h"
#include "1992_1_1_section.h"
#include "EF_noeud.h"
#include <stdio.h>
#include <stdlib.h>
#include <libintl.h>
#include <locale.h>
#include <string.h>
#include <gtk/gtk.h>

void gtk_window_destroy_event(GtkWidget *pWidget __attribute__((unused)), Projet *projet)
{
	projet_free(projet);
	gtk_main_quit();
	return;
}

/* gtk_window_option_destroy_button
 * Description : Bouton de fermeture de la fenêtre
 * Paramètres : GtkComboBox *widget : composant à l'origine de la demande
 *            : GtkWidget *fenêtre : la fenêtre d'options
 * Valeur renvoyée : Aucune
 */
void gtk_window_option_destroy_button(GtkWidget *object __attribute__((unused)), GtkWidget *fenetre __attribute__((unused)))
{
	gtk_widget_destroy(fenetre);
	return;
}

int main(int argc, char *argv[])
{
	/* Variables */
	GtkWidget * MainWindow = NULL;
	GtkWidget *pTable;
	GtkWidget *pButton;
	Projet *projet;
	
	// On charge la localisation
	setlocale( LC_ALL, "" );
	bindtextdomain(PACKAGE, LOCALEDIR);
	textdomain(PACKAGE);
	
	// On traite les arguments du programme
	switch (argc)
	{
		case 2:
		{
			if ((strcmp(argv[1], "-w") == 0) || (strcmp(argv[1], "--warranty") == 0))
			{
				show_warranty();
				return 0;
			}
			else if ((strcmp(argv[1], "-h") == 0) || (strcmp(argv[1], "--help") == 0))
			{
				show_help();
				return 0;
			}
			break;
		}
		default:
		{
			break;
		}
	}
	
	// Initialisation de GTK+
	if (gtk_init_check(&argc, &argv) == FALSE)
		BUGTEXTE(-1, gettext("Impossible d'initialiser gtk.\n"));
	
	// Création d'un projet type
	projet = projet_init();
	
	// Création des actions
	if (_1990_action_ajout(projet, 0) != 0) BUG(-1); // Poids propre
	if (_1990_action_ajout(projet, 2) != 0) BUG(-1); // 2 Exploitation
	if (_1990_action_ajout(projet, 12) != 0) BUG(-1); // 3 Neige
	if (_1990_action_ajout(projet, 13) != 0) BUG(-1); // 4 Vent
	
	// Création des groupes d'actions
	if (_1990_groupe_niveau_ajout(projet) != 0) BUG(-1);
	if (_1990_groupe_niveau_ajout(projet) != 0) BUG(-1);
	if (_1990_groupe_niveau_ajout(projet) != 0) BUG(-1);
	if (_1990_groupe_niveau_ajout(projet) != 0) BUG(-1);
	if (_1990_groupe_ajout(projet, 0, 0, GROUPE_COMBINAISON_AND) != 0) BUG(-1); // Charges permanentes
	if (_1990_groupe_ajout(projet, 0, 1, GROUPE_COMBINAISON_AND) != 0) BUG(-1); // Charges d'exploitation
	if (_1990_groupe_ajout(projet, 0, 2, GROUPE_COMBINAISON_XOR) != 0) BUG(-1); // Neige
	if (_1990_groupe_ajout(projet, 0, 3, GROUPE_COMBINAISON_XOR) != 0) BUG(-1); // Vent
	if (_1990_groupe_ajout(projet, 1, 0, GROUPE_COMBINAISON_AND) != 0) BUG(-1); // Charges permanentes
	if (_1990_groupe_ajout(projet, 1, 1, GROUPE_COMBINAISON_AND) != 0) BUG(-1); // Charges d'exploitation
	if (_1990_groupe_ajout(projet, 1, 2, GROUPE_COMBINAISON_OR) != 0) BUG(-1); // Charges climatiques
	if (_1990_groupe_ajout(projet, 2, 0, GROUPE_COMBINAISON_AND) != 0) BUG(-1); // Tout
	if (_1990_groupe_ajout(projet, 2, 1, GROUPE_COMBINAISON_OR) != 0) BUG(-1); // Tout
	if (_1990_groupe_ajout(projet, 3, 0, GROUPE_COMBINAISON_AND) != 0) BUG(-1); // Tout
	if (_1990_groupe_ajout_element(projet, 0, 0, 0) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 0, 1, 1) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 0, 2, 2) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 0, 3, 3) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 1, 0, 0) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 1, 1, 1) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 1, 2, 2) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 1, 2, 3) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 2, 0, 0) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 2, 1, 1) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 2, 1, 2) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 3, 0, 0) != 0) BUG(-1);
	if (_1990_groupe_ajout_element(projet, 3, 0, 1) != 0) BUG(-1);
	
	// Création des sections en béton
	if (_1992_1_1_sections_ajout_rectangulaire(projet, 0.1, 0.3) != 0) BUG(-1);
	
	// Création de l'appui
	if (EF_appuis_ajout(projet, EF_APPUI_BLOQUE, EF_APPUI_BLOQUE, EF_APPUI_BLOQUE, EF_APPUI_BLOQUE, EF_APPUI_BLOQUE, EF_APPUI_BLOQUE) != 0) BUG(-1);
	
	// Création des noeuds
	if (EF_noeuds_ajout(projet, 0., 0., 0., 0) != 0) BUG(-1);
	if (EF_noeuds_ajout(projet, 1., 0.5, 3., -1) != 0) BUG(-1);
	
	// Création du matériau béton
	if (_1992_1_1_materiaux_ajout(projet, 25., 0.2) != 0) BUG(-1);
	
	// Création de l'élément en béton
	if (_1992_1_1_elements_ajout(projet, BETON_ELEMENT_POUTRE, 0, 0, 0, 1, 1) != 0) BUG(-1);
	
	// Ajout de la rigidité de l'élément à la matrice globale du projet
	if (_1992_1_1_elements_rigidite_ajout(projet, 0) != 0) BUG(-1);
	
	// Création de la fenêtre principale
	MainWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	// Ne pas mettre la ligne ci-dessous sinon projet est libéré deux fois.
	//g_signal_connect(G_OBJECT(MainWindow), "delete-event", G_CALLBACK(gtk_window_delete_event), projet);
	g_signal_connect(GTK_WINDOW(MainWindow), "destroy", G_CALLBACK(gtk_window_destroy_event), projet);
	pTable=gtk_table_new(2,1,TRUE);
	gtk_container_add(GTK_CONTAINER(MainWindow), GTK_WIDGET(pTable));
	pButton= gtk_button_new_with_label("Combinaisons");
	gtk_table_attach(GTK_TABLE(pTable), pButton, 0, 1, 0, 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
	g_signal_connect (pButton, "clicked", G_CALLBACK (_1990_gtk_groupes), projet);
	
	// Ajout du bouton de fermeture de l'application
	pButton= gtk_button_new_with_label("Fermeture");
	gtk_table_attach(GTK_TABLE(pTable), pButton, 0, 1, 1, 2, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
	g_signal_connect (pButton, "clicked", G_CALLBACK (gtk_window_option_destroy_button), MainWindow);
	
	// Affichage de l'interface graphique
	gtk_widget_show_all(MainWindow);
	gtk_main();
	
	return 0;
}

