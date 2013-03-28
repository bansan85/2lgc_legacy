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
#include <gtk/gtk.h>
#include <math.h>
#include <string.h>
#include "common_selection.h"
#include "common_erreurs.h"
#include "common_math.h"
#include "common_gtk.h"

gboolean common_gtk_treeview_button_press_unselect(GtkTreeView *widget,
  GdkEvent *event, Projet *projet)
/* Description : Déselectionne la ligne sélectionnée si on clique sur une zone vide du treeview.
 * Paramètres : GtkTreeView *widget : composant à l'origine de l'évènement,
 *            : GdkEvent *event : Caractéristique de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : FALSE.
 *   Echec : projet == NULL.
 *  
 */
{
    BUGMSG(projet, TRUE, gettext("Paramètre %s incorrect.\n"), "projet");
    
    if (event->type == GDK_BUTTON_PRESS)
    {
        GdkEventButton *event_button = (GdkEventButton *)event;
        
        if (!gtk_tree_view_get_path_at_pos(widget, (gint) event_button->x, (gint) event_button->y, NULL, NULL, NULL, NULL))
        {
            GtkTreeIter Iter;
            if (gtk_tree_selection_get_mode(gtk_tree_view_get_selection(widget)) == GTK_SELECTION_MULTIPLE)
                gtk_tree_selection_unselect_all(gtk_tree_view_get_selection(widget));
            else if (gtk_tree_selection_get_selected(gtk_tree_view_get_selection(widget), NULL, &Iter))
                gtk_tree_selection_unselect_iter(gtk_tree_view_get_selection(widget), &Iter);
        }
    }
    
    return FALSE;
}


double common_gtk_text_buffer_double(GtkTextBuffer *textbuffer, double val_min,
  gboolean min_include, double val_max, gboolean max_include)
/* Description : Vérifie en temps réel si le GtkTextBuffer contient bien un nombre flottant
 *               compris entre les valeurs val_min et val_max.
 *               S'il ne contient pas de nombre ou hors domaine, le texte passe en rouge.
 * Paramètres : GtkTextBuffer *textbuffer : composant à l'origine de l'évènement,
 *            : double val_min : borne inférieure,
 *            : gboolean min_include : le nombre de la borne inférieure est-il autorisé ?,
 *            : double val_max : borne supérieure
 *            : gboolean max_include : le nombre de la borne supérieure est-il autorisé ?.
 * Valeur renvoyée :
 *   Succès : la valeur du nombre,
 *   Echec : NAN.
 */
{
    char        *texte;
    GtkTextIter start, end;
    double      nombre;
    char        *fake;
    gboolean    min_check;
    gboolean    max_check;
    
    gtk_text_buffer_get_iter_at_offset(textbuffer, &start, 0);
    gtk_text_buffer_get_iter_at_offset(textbuffer, &end, -1);
    texte = gtk_text_buffer_get_text(textbuffer, &start, &end, FALSE);
    BUGMSG(fake = (char*)malloc(sizeof(char)*(strlen(texte)+1)), NAN, gettext("Erreur d'allocation mémoire.\n"));
    
    gtk_text_buffer_remove_all_tags(textbuffer, &start, &end);
    if (sscanf(texte, "%lf%s", &nombre, fake) != 1)
    {
        min_check = FALSE;
        max_check = FALSE;
    }
    else
    {
        if (isinf(val_min) == -1)
            min_check = TRUE;
        else if ((min_include) && (ERREUR_RELATIVE_EGALE(nombre, val_min)))
            min_check = TRUE;
        else if (nombre > val_min)
            min_check = TRUE;
        else
            min_check = FALSE;
            
        if (isinf(val_max) == 1)
            max_check = TRUE;
        else if ((max_include) && (ERREUR_RELATIVE_EGALE(nombre, val_max)))
            max_check = TRUE;
        else if (nombre < val_max)
            max_check = TRUE;
        else
            max_check = FALSE;
    }
    
    free(texte);
    free(fake);
    
    if ((min_check) && (max_check))
    {
        gtk_text_buffer_apply_tag_by_name(textbuffer, "OK", &start, &end);
        return nombre;
    }
    else
    {
        gtk_text_buffer_apply_tag_by_name(textbuffer, "mauvais", &start, &end);
        return NAN;
    }
}


unsigned int common_gtk_text_buffer_uint(GtkTextBuffer *textbuffer, unsigned int val_min,
  gboolean min_include, unsigned int val_max, gboolean max_include)
/* Description : Vérifie en temps réel si le GtkTextBuffer contient bien un nombre entier non
 *               signé compris entre les valeurs val_min et val_max.
 *               S'il ne contient pas de nombre ou hors domaine, le texte passe en rouge.
 * Paramètres : GtkTextBuffer *textbuffer : composant à l'origine de l'évènement,
 *            : unsigned int val_min : borne inférieure,
 *            : gboolean min_include : borne inférieure autorisée ? (min 0),
 *            : unsigned int val_max : borne supérieure
 *            : gboolean max_include : borne supérieure autorisée ? (max UINT_MAX).
 * Valeur renvoyée :
 *   Succès : la valeur du nombre,
 *   Echec : UINT_MAX.
 */
{
    char            *texte;
    GtkTextIter     start, end;
    unsigned int    nombre;
    char            *fake;
    gboolean        min_check;
    gboolean        max_check;
    
    gtk_text_buffer_get_iter_at_offset(textbuffer, &start, 0);
    gtk_text_buffer_get_iter_at_offset(textbuffer, &end, -1);
    texte = gtk_text_buffer_get_text(textbuffer, &start, &end, FALSE);
    BUGMSG(fake = (char*)malloc(sizeof(char)*(strlen(texte)+1)), UINT_MAX, gettext("Erreur d'allocation mémoire.\n"));
    
    gtk_text_buffer_remove_all_tags(textbuffer, &start, &end);
    if (sscanf(texte, "%u%s", &nombre, fake) != 1)
    {
        min_check = FALSE;
        max_check = FALSE;
    }
    else
    {
        if (min_include)
            min_check = nombre >= val_min;
        else
            min_check = nombre > val_min;
            
        if (max_include)
            max_check = nombre <= val_max;
        else
            max_check = nombre < val_max;
    }
    
    free(texte);
    free(fake);
    
    if ((min_check) && (max_check))
    {
        gtk_text_buffer_apply_tag_by_name(textbuffer, "OK", &start, &end);
        return nombre;
    }
    else
    {
        gtk_text_buffer_apply_tag_by_name(textbuffer, "mauvais", &start, &end);
        return UINT_MAX;
    }
}


unsigned int common_gtk_entry_uint(GtkEntry *entry, unsigned int val_min, gboolean min_include,
  unsigned int val_max, gboolean max_include)
/* Description : Renvoie le nombre entier non signé de l'entry, valeur comprise entre les
 *               valeurs val_min et val_max.
 * Paramètres : GtkEntry *entry : composant à l'origine de l'évènement,
 *            : unsigned int val_min : borne inférieure,
 *            : gboolean min_include : borne inférieure autorisée ? (min 0),
 *            : unsigned int val_max : borne supérieure
 *            : gboolean max_include : borne supérieure autorisée ? (max UINT_MAX).
 * Valeur renvoyée :
 *   Succès : la valeur du nombre,
 *   Echec : UINT_MAX.
 */
{
    const char      *texte;
    unsigned int    nombre;
    char            *fake;
    gboolean        min_check;
    gboolean        max_check;
    
    texte = gtk_entry_get_text(entry);
    BUGMSG(fake = (char*)malloc(sizeof(char)*(strlen(texte)+1)), UINT_MAX, gettext("Erreur d'allocation mémoire.\n"));
    
    if (sscanf(texte, "%u%s", &nombre, fake) != 1)
    {
        min_check = FALSE;
        max_check = FALSE;
    }
    else
    {
        if (min_include)
            min_check = nombre >= val_min;
        else
            min_check = nombre > val_min;
            
        if (max_include)
            max_check = nombre <= val_max;
        else
            max_check = nombre < val_max;
    }
    
    free(fake);
    
    if ((min_check) && (max_check))
        return nombre;
    else
        return UINT_MAX;
}


void common_gtk_render_double(GtkTreeViewColumn *tree_column, GtkCellRenderer *cell,
  GtkTreeModel *tree_model, GtkTreeIter *iter, gpointer data)
/* Description : Personnalise l'affichage des nombres de type double dans un treeview.
 * Paramètres : GtkTreeViewColumn *tree_column : la colonne,
 *            : GtkCellRenderer *cell : la cellule,
 *            : GtkTreeModel *tree_model : le tree_model,
 *            : GtkTreeIter *iter : et le paramètre iter,
 *            : gpointer data : le nombre de décimale.
 * Valeur renvoyée : Aucune.
 */
{
    gchar   texte[30];
    gint    colonne;
    double  nombre;
    gint    decimales = GPOINTER_TO_INT(data);
    
    colonne = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(cell), "column"));
    gtk_tree_model_get(tree_model, iter, colonne, &nombre, -1);
    
    common_math_double_to_char(nombre, texte, decimales);
    g_object_set(GTK_CELL_RENDERER_TEXT(cell), "text", texte, NULL);
    
    return;
}


GtkTreeViewColumn* common_gtk_cree_colonne(char* nom, GType type, int num_colonne,
  double xalign, int num_decimales)
/* Description : Permet de créer une colonne pour un treeview.
 * Paramètres : char* nom : nom de la colonne,
 *            : GType type : type d'information que recevra la colonne,
 *            : int num_colonne : numéro de la colonne.
 *            : double xalign : alignement horizontal du contenu de la cellule dans la colonne,
 *            : int num_decimales : si le type est G_TYPE_DOUBLE, défini le nombre de décimales
 *                                  devant être affiché.
 * Valeur renvoyée : un pointeur vers la nouvelle colonne.
 */
{
    GtkCellRenderer     *cell;
    GtkTreeViewColumn   *column;
    GtkWidget           *label;
    
    if (type == G_TYPE_OBJECT)
        cell = gtk_cell_renderer_pixbuf_new();
    else
        cell = gtk_cell_renderer_text_new();
    
    // Pour les nombres flottants, l'alignement à droite permet aux virgules d'être alignées.
    gtk_cell_renderer_set_alignment(cell, xalign, 0.5);
    
    label = gtk_label_new("");
    gtk_label_set_markup(GTK_LABEL(label), nom);
    gtk_widget_set_visible(label, TRUE);
    
    column = gtk_tree_view_column_new();
    gtk_tree_view_column_pack_start(column, cell, TRUE);
    if (type == G_TYPE_OBJECT)
        gtk_tree_view_column_set_attributes(column, cell, "pixbuf", num_colonne, NULL);
    else
        gtk_tree_view_column_set_attributes(column, cell, "text", num_colonne, NULL);
    gtk_tree_view_column_set_widget(column, label);
    gtk_tree_view_column_set_alignment(column, 0.5);
    
    if (type == G_TYPE_DOUBLE)
    {
        g_object_set_data(G_OBJECT(cell), "column", GINT_TO_POINTER(num_colonne));
        gtk_tree_view_column_set_cell_data_func(column, cell, common_gtk_render_double, GINT_TO_POINTER(num_decimales), NULL);
    }
    
    return column;
}
