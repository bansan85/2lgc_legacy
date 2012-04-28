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
#include <stdio.h>
#include <stdlib.h>
#include <libintl.h>
#include <locale.h>
#include <string.h>
#include <gtk/gtk.h>
#include <math.h>
#include "common_selection.h"
#include "common_erreurs.h"
#include "common_maths.h"

G_MODULE_EXPORT void gtk_common_entry_check_double(GtkTextBuffer *textbuffer, gpointer user_data __attribute__((unused)))
/* Description : Vérifie en temps réel si le GtkTextBuffer contient bien un nombre flottant.
 *               S'il ne contient pas de nombre, le texte passe en rouge.
 * Paramètres : GtkTextBuffer *textbuffer : composant à l'origine de l'évènement
 *            : gpointer user_data : ne sert à rien
 * Valeur renvoyée : Aucune
 */
{
    gchar       *texte;
    GtkTextIter start, end;
    double      nombre;
    char        *fake;
    
    gtk_text_buffer_get_iter_at_offset(textbuffer, &start, 0);
    gtk_text_buffer_get_iter_at_offset(textbuffer, &end, -1);
    texte = gtk_text_buffer_get_text(textbuffer, &start, &end, FALSE);
    BUGMSG(fake = (char*)malloc(sizeof(char)*(strlen(texte)+1)), , gettext("Erreur d'allocation mémoire.\n"));
    
    if (sscanf(texte, "%lf%s", &nombre, fake) != 1)
    {
        gtk_text_buffer_remove_all_tags(textbuffer, &start, &end);
        gtk_text_buffer_apply_tag_by_name(textbuffer, "mauvais", &start, &end);
    }
    else
    {
        gtk_text_buffer_remove_all_tags(textbuffer, &start, &end);
        gtk_text_buffer_apply_tag_by_name(textbuffer, "OK", &start, &end);
    }
    free(texte);
    free(fake);
    
    return;
}


G_MODULE_EXPORT double gtk_common_entry_renvoie_double(GtkTextBuffer *textbuffer)
/* Description : Renvoie le nombre flottant si le GtkTextBuffer en contient bien un.
 *               Renvoie nan sinon.
 * Paramètres : GtkTextBuffer *textbuffer : composant à vérifier
 * Valeur renvoyée : Aucune
 */
{
    gchar       *texte;
    GtkTextIter start, end;
    double      nombre;
    char        *fake;
    
    gtk_text_buffer_get_iter_at_offset(textbuffer, &start, 0);
    gtk_text_buffer_get_iter_at_offset(textbuffer, &end, -1);
    texte = gtk_text_buffer_get_text(textbuffer, &start, &end, FALSE);
    BUGMSG(fake = (char*)malloc(sizeof(char)*(strlen(texte)+1)), -2, gettext("Erreur d'allocation mémoire.\n"));
    
    if (sscanf(texte, "%lf%s", &nombre, fake) != 1)
    {
        free(texte);
        free(fake);
        return NAN;
    }
    else
    {
        free(texte);
        free(fake);
        return nombre;
    }
}

G_MODULE_EXPORT void gtk_common_entry_check_int(GtkTextBuffer *textbuffer, gpointer user_data __attribute__((unused)))
/* Description : Vérifie en temps réel si le GtkTextBuffer contient bien un nombre entier.
 *               S'il ne contient pas de nombre, le texte passe en rouge.
 * Paramètres : GtkTextBuffer *textbuffer : composant à l'origine de l'évènement
 *            : gpointer user_data : ne sert à rien
 * Valeur renvoyée : Aucune
 */
{
    gchar       *texte;
    GtkTextIter start, end;
    int         nombre;
    char        *fake;
    
    gtk_text_buffer_get_iter_at_offset(textbuffer, &start, 0);
    gtk_text_buffer_get_iter_at_offset(textbuffer, &end, -1);
    texte = gtk_text_buffer_get_text(textbuffer, &start, &end, FALSE);
    BUGMSG(fake = (char*)malloc(sizeof(char)*(strlen(texte)+1)), , gettext("Erreur d'allocation mémoire.\n"));
    
    if (sscanf(texte, "%d%s", &nombre, fake) != 1)
    {
        gtk_text_buffer_remove_all_tags(textbuffer, &start, &end);
        gtk_text_buffer_apply_tag_by_name(textbuffer, "mauvais", &start, &end);
    }
    else
    {
        gtk_text_buffer_remove_all_tags(textbuffer, &start, &end);
        gtk_text_buffer_apply_tag_by_name(textbuffer, "OK", &start, &end);
    }
    free(texte);
    free(fake);
    
    return;
}


G_MODULE_EXPORT int gtk_common_entry_renvoie_int(GtkTextBuffer *textbuffer)
/* Description : Renvoie le nombre entier si le GtkTextBuffer en contient bien un.
 *               Renvoie INT_MIN sinon.
 * Paramètres : GtkTextBuffer *textbuffer : composant à vérifier
 * Valeur renvoyée : Aucune
 */
{
    gchar       *texte;
    GtkTextIter start, end;
    int         nombre;
    char        *fake;
    
    gtk_text_buffer_get_iter_at_offset(textbuffer, &start, 0);
    gtk_text_buffer_get_iter_at_offset(textbuffer, &end, -1);
    texte = gtk_text_buffer_get_text(textbuffer, &start, &end, FALSE);
    BUGMSG(fake = (char*)malloc(sizeof(char)*(strlen(texte)+1)), -2, gettext("Erreur d'allocation mémoire.\n"));
    
    if (sscanf(texte, "%d%s", &nombre, fake) != 1)
    {
        free(texte);
        free(fake);
        return -1;
    }
    else
    {
        free(texte);
        free(fake);
        return nombre;
    }
}


G_MODULE_EXPORT void gtk_common_entry_check_liste(GtkTextBuffer *textbuffer, gpointer user_data __attribute__((unused)))
/* Description : Vérifie en temps réel si le GtkTextBuffer contient bien un nombre entier.
 *               S'il ne contient pas de nombre, le texte passe en rouge.
 * Paramètres : GtkTextBuffer *textbuffer : composant à l'origine de l'évènement
 *            : gpointer user_data : ne sert à rien
 * Valeur renvoyée : Aucune
 */
{
    gchar       *texte;
    GtkTextIter start, end;
    GList       *retour;
    
    gtk_text_buffer_get_iter_at_offset(textbuffer, &start, 0);
    gtk_text_buffer_get_iter_at_offset(textbuffer, &end, -1);
    texte = gtk_text_buffer_get_text(textbuffer, &start, &end, FALSE);
    retour = common_selection_renvoie_numeros(texte);
    if (retour == NULL)
    {
        gtk_text_buffer_remove_all_tags(textbuffer, &start, &end);
        gtk_text_buffer_apply_tag_by_name(textbuffer, "mauvais", &start, &end);
    }
    else
    {
        gtk_text_buffer_remove_all_tags(textbuffer, &start, &end);
        gtk_text_buffer_apply_tag_by_name(textbuffer, "OK", &start, &end);
        g_list_free(retour);
    }
    free(texte);
    return;
}


G_MODULE_EXPORT gboolean common_gtk_key_press(GtkWidget *widget __attribute__((unused)), GdkEvent *event __attribute__((unused)), GtkWidget *fenetre)
/* Description : Détruit la fenêtre si la touche d'échappement est appuiée.
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement,
 *            : GdkEvent *event : description de l'évènement,
 *            : GtkWidget *fenetre : la fenêtre à détruire.
 * Valeur renvoyée : TRUE pour arrêter le traitement des touches,
 *                 : FALSE si ce n'est pas la touche Escape.
 */
{
    if (event->key.keyval == GDK_KEY_Escape)
    {
        gtk_widget_destroy(fenetre);
        return TRUE;
    }
    else
        return FALSE;
}


G_MODULE_EXPORT void gtk_common_render_double(GtkTreeViewColumn *tree_column __attribute__((unused)), GtkCellRenderer *cell, GtkTreeModel *tree_model, GtkTreeIter *iter, gpointer data)
/* Description : personnalise l'affichage des nombres de type double dans un treeview.
 * Paramètres : GtkTreeViewColumn *tree_column : la colonne,
 *            : GtkCellRenderer *cell : la cellule,
 *            : GtkTreeModel *tree_model : le tree_model,
 *            : GtkTreeIter *iter : et le paramètre iter,
 *            : gpointer data : le nombre de décimale.
 * Valeur renvoyée : void
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
