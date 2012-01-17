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

#ifndef __COMMON_GTK_H
#define __COMMON_GTK_H

#include "config.h"
#include "common_projet.h"
#include <gtk/gtk.h>

#define GTK_NOUVELLE_FENETRE(window, titre, taillex, tailley) {\
            window = gtk_window_new(GTK_WINDOW_TOPLEVEL); \
            gtk_window_set_title(GTK_WINDOW(window), titre); \
            gtk_window_resize(GTK_WINDOW(window), taillex, tailley); \
            gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER); \
            }

#define GTK_NOUVEAU_TEXT_VIEW_AVEC_SCROLLED_WINDOW(textview, scrolled) {\
            textview = gtk_text_view_new(); \
            gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(textview), GTK_WRAP_WORD); \
            gtk_text_view_set_accepts_tab(GTK_TEXT_VIEW(textview), FALSE); \
            gtk_container_set_border_width(GTK_CONTAINER(textview), 2); \
            scrolled = gtk_scrolled_window_new (NULL, NULL); \
            gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC); \
            gtk_container_add(GTK_CONTAINER(scrolled), textview); \
            }

#define GTK_TEXT_VIEW_VERIFIE_DOUBLE(textview) {\
            gtk_text_buffer_create_tag(gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview)), "mauvais", "foreground", "red", "weight", PANGO_WEIGHT_BOLD, NULL); \
            gtk_text_buffer_create_tag(gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview)), "OK", "foreground", "black", "weight", PANGO_WEIGHT_NORMAL, NULL); \
            g_signal_connect(gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview)), "changed", G_CALLBACK(gtk_common_entry_check_double), NULL); \
            }

#define GTK_TEXT_VIEW_VERIFIE_INT(textview) {\
            gtk_text_buffer_create_tag(gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview)), "mauvais", "foreground", "red", "weight", PANGO_WEIGHT_BOLD, NULL); \
            gtk_text_buffer_create_tag(gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview)), "OK", "foreground", "black", "weight", PANGO_WEIGHT_NORMAL, NULL); \
            g_signal_connect(gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview)), "changed", G_CALLBACK(gtk_common_entry_check_int), NULL); \
            }

#define GTK_TEXT_VIEW_VERIFIE_LISTE(textview) {\
            gtk_text_buffer_create_tag(gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview)), "mauvais", "foreground", "red", "weight", PANGO_WEIGHT_BOLD, NULL); \
            gtk_text_buffer_create_tag(gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview)), "OK", "foreground", "black", "weight", PANGO_WEIGHT_NORMAL, NULL); \
            g_signal_connect(gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview)), "changed", G_CALLBACK(gtk_common_entry_check_liste), NULL); \
            }

void gtk_common_entry_check_double(GtkEntry *entry, gchar *preedit, gpointer user_data);
double gtk_common_entry_renvoie_double(GtkTextBuffer *textbuffer);
void gtk_common_entry_check_int(GtkEntry *entry, gchar *preedit, gpointer user_data);
int gtk_common_entry_renvoie_int(GtkTextBuffer *textbuffer);
void gtk_common_entry_check_liste(GtkEntry *entry, gchar *preedit, gpointer user_data);

#endif
