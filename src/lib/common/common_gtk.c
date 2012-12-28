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

G_MODULE_EXPORT gboolean common_gtk_treeview_button_press_unselect(GtkTreeView *widget,
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


G_MODULE_EXPORT double common_gtk_text_buffer_double(GtkTextBuffer *textbuffer, double val_min,
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


G_MODULE_EXPORT void common_gtk_entry_check_int(GtkTextBuffer *textbuffer,
  gpointer user_data __attribute__((unused)))
/* Description : Vérifie en temps réel si le GtkTextBuffer contient bien un nombre entier.
 *               S'il ne contient pas de nombre, le texte passe en rouge.
 * Paramètres : GtkTextBuffer *textbuffer : composant à l'origine de l'évènement,
 *            : gpointer user_data : ne sert à rien.
 * Valeur renvoyée : Aucune.
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


G_MODULE_EXPORT int common_gtk_entry_renvoie_int(GtkTextBuffer *textbuffer)
/* Description : Renvoie le nombre entier si le GtkTextBuffer en contient bien un.
 *               Renvoie INT_MIN sinon.
 * Paramètres : GtkTextBuffer *textbuffer : composant à vérifier.
 * Valeur renvoyée :
 *   Succès : le nombre entier contenu dans un composant de type Entry
 *   Échec : INT_MIN :
 *             textbuffer == NULL,
 *             le composant Entry ne contient pas un nombre double,
 *             en cas d'erreur d'allocation mémoire,
 */
{
    gchar       *texte;
    GtkTextIter start, end;
    int         nombre;
    char        *fake;
    
    BUGMSG(textbuffer, INT_MIN, gettext("Paramètre %s incorrect.\n"), "textbuffer");
    
    gtk_text_buffer_get_iter_at_offset(textbuffer, &start, 0);
    gtk_text_buffer_get_iter_at_offset(textbuffer, &end, -1);
    texte = gtk_text_buffer_get_text(textbuffer, &start, &end, FALSE);
    BUGMSG(fake = (char*)malloc(sizeof(char)*(strlen(texte)+1)), INT_MIN, gettext("Erreur d'allocation mémoire.\n"));
    
    if (sscanf(texte, "%d%s", &nombre, fake) != 1)
    {
        free(texte);
        free(fake);
        return INT_MIN;
    }
    else
    {
        free(texte);
        free(fake);
        return nombre;
    }
}


G_MODULE_EXPORT unsigned int common_gtk_entry_renvoie_uint(GtkTextBuffer *textbuffer)
/* Description : Renvoie le nombre non signé entier si le GtkTextBuffer en contient bien un.
 *               Renvoie UINT_MAX sinon.
 * Paramètres : GtkTextBuffer *textbuffer : composant à vérifier.
 * Valeur renvoyée :
 *   Succès : le nombre entier contenu dans un composant de type Entry
 *   Échec : UINT_MAX :
 *             textbuffer == NULL,
 *             le composant Entry ne contient pas un nombre double,
 *             en cas d'erreur d'allocation mémoire,
 */
{
    gchar           *texte;
    GtkTextIter     start, end;
    unsigned int    nombre;
    char            *fake;
    
    BUGMSG(textbuffer, UINT_MAX, gettext("Paramètre %s incorrect.\n"), "textbuffer");
    
    gtk_text_buffer_get_iter_at_offset(textbuffer, &start, 0);
    gtk_text_buffer_get_iter_at_offset(textbuffer, &end, -1);
    texte = gtk_text_buffer_get_text(textbuffer, &start, &end, FALSE);
    BUGMSG(fake = (char*)malloc(sizeof(char)*(strlen(texte)+1)), UINT_MAX, gettext("Erreur d'allocation mémoire.\n"));
    
    if (sscanf(texte, "%u%s", &nombre, fake) != 1)
    {
        free(texte);
        free(fake);
        return UINT_MAX;
    }
    else
    {
        free(texte);
        free(fake);
        return nombre;
    }
}


G_MODULE_EXPORT void common_gtk_entry_check_liste(GtkTextBuffer *textbuffer,
  gpointer user_data __attribute__((unused)))
/* Description : Vérifie en temps réel si le GtkTextBuffer contient une liste de nombre entier.
 *               S'il ne contient pas de nombre, le texte passe en rouge.
 * Paramètres : GtkTextBuffer *textbuffer : composant à l'origine de l'évènement,
 *            : gpointer user_data : ne sert à rien.
 * Valeur renvoyée : Aucune.
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


G_MODULE_EXPORT gboolean common_gtk_key_press(GtkWidget *widget __attribute__((unused)),
  GdkEvent *event __attribute__((unused)), GtkWidget *fenetre)
/* Description : Détruit la fenêtre si la touche d'échappement est appuyée.
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


G_MODULE_EXPORT void common_gtk_render_double(
  GtkTreeViewColumn *tree_column __attribute__((unused)), GtkCellRenderer *cell,
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
