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
#include <stdio.h>
#include <stdlib.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <gtk/gtk.h>
#include <string.h>
#include "common_erreurs.h"

/* wrapped_label_size_allocate_callback
 * Description : Permet un redimensionnement en temps réel du composant label
 * Paramètres : GtkWidget *label : composant label à redimensionner
 *            : GtkAllocation *allocation : nouvelle dimension
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
void wrapped_label_size_allocate_callback(GtkWidget *label, GtkAllocation *allocation, gpointer data __attribute__((unused)))
{
	gtk_widget_set_size_request(label, allocation->width, -1);
	return;
}

/* common_generation_tooltip
 * Description : Génère une fenêtre de type tooltip contenant les informations spécifiées par le nom 'nom'.
 *             : les informations sont contenues dans le fichier DATADIR"/tooltips.xml" sous format XML.
 *             : L'ouverture de ce fichier permet de comprendre de la façon la plus rapide son format
 * Paramètres : const char *nom : nom de la fenêtre
 * Valeur renvoyée : 
 */
GtkWidget* common_generation_tooltip(const char *nom)
{
	xmlDocPtr	doc;
	xmlNodePtr	racine;
	xmlNodePtr	n0;
	xmlChar		*nom1;
	int i = 0;
	
	doc = xmlParseFile(DATADIR"/tooltips.xml");
	if (doc == NULL)
		BUGTEXTE(NULL, gettext("Le fichier '%s' est introuvable ou corrompu.\n"), DATADIR"/tooltips.xml");
	
	racine = xmlDocGetRootElement(doc);
	if (racine == NULL)
		BUGTEXTE(NULL, gettext("Le fichier '%s' est vide.\n"), DATADIR"/tooltips.xml");
	
	for (n0 = racine; n0 != NULL; n0 = n0->next)
	{
		if ((strcmp((char*)n0->name, "liste") == 0) && (n0->type == XML_ELEMENT_NODE) && (n0->children != NULL))
		{
			xmlNodePtr	n1;
			for (n1 = n0->children; n1 != NULL; n1 = n1->next)
			{
				if ((strcmp((char*)n1->name, "tooltip") == 0) && (n1->type == XML_ELEMENT_NODE) && (n1->children != NULL))
				{
					nom1 = xmlGetProp(n1, BAD_CAST "reference");
					// Si le nom du noeud correspond au noeud recherché
					if (strcmp((char *) nom1, nom) == 0)
					{
						GtkWidget	*pwindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
						GtkWidget	*pvbox = gtk_vbox_new(FALSE, 0);
						GdkRGBA		color;
						xmlNodePtr	n2;
						
						// Initialisation de la fenêtre graphique
						gtk_widget_show(pvbox);
						gtk_window_set_position(GTK_WINDOW(pwindow), GTK_WIN_POS_CENTER);
						gtk_window_set_title(GTK_WINDOW(pwindow), (char *) nom1);
						gtk_container_add(GTK_CONTAINER(pwindow), pvbox);
						gtk_window_set_decorated(GTK_WINDOW(pwindow), FALSE);
						color.red = 1.; color.green = 1.; color.blue = 0.75; color.alpha = 1.;
						gtk_widget_override_background_color(pwindow, GTK_STATE_FLAG_NORMAL, &color);
						
						for (n2 = n1->children; n2 != NULL; n2 = n2->next)
						{
							if (n2->type == XML_ELEMENT_NODE)
							{
								xmlChar *contenu = xmlNodeGetContent(n2);
								// Contenu de type image
								if (strcmp((char *) n2->name, "image") == 0)
								{
									char		*nom_fichier = malloc(sizeof(char)*(strlen(DATADIR)+strlen((char *) contenu)+2));
									GtkWidget	*element;
									
									strcpy(nom_fichier, DATADIR);
									strcat(nom_fichier, "/");
									strcat(nom_fichier, (char *)contenu);
									element = gtk_image_new_from_file(nom_fichier);
									free(nom_fichier);
									gtk_misc_set_alignment(GTK_MISC(element), 0., 0.5);
									gtk_box_pack_start(GTK_BOX(pvbox), element, FALSE, FALSE, 0);
									gtk_widget_show(element);
									i++;
								}
								// Contenu de type texte
								else if (strcmp((char *) n2->name, "texte") == 0)
								{
									GtkWidget	*element;
									element = gtk_label_new((char *)contenu);
									
									gtk_label_set_line_wrap(GTK_LABEL(element), TRUE);
									gtk_label_set_justify(GTK_LABEL(element), GTK_JUSTIFY_FILL);
									g_signal_connect(G_OBJECT(element), "size-allocate", G_CALLBACK(wrapped_label_size_allocate_callback), NULL);
									gtk_misc_set_alignment(GTK_MISC(element), 0., 0.5);
									gtk_box_pack_start(GTK_BOX(pvbox), element, TRUE, TRUE, 0);
									gtk_widget_show(element);
									i++;
								}
								// Les dimensions de la fenêtre sont indiquées explicitement
								else if (strcmp((char *) n2->name, "dimensions") == 0)
								{
									char	*fake = malloc(sizeof(char)*(strlen((char *)contenu)+1));
									int	largeur, hauteur;
									
									if (sscanf((char *)contenu, "%dx%d %s", &largeur, &hauteur, fake) != 2)
										printf("'%s' n'est pas de la forme 'largeurxhauteur'.\n", (char *)contenu);
									else
										gtk_window_set_default_size(GTK_WINDOW(pwindow), largeur, hauteur);
									free(fake);
								}
								xmlFree(contenu);
							}
						}
						return pwindow;
					}
				}
			}
		}
	}
	
	xmlFreeDoc(doc);
	
	return NULL;
}
#endif
