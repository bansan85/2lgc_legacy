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

void gtk_common_entry_check_double(GtkTextBuffer *textbuffer, gpointer user_data __attribute__((unused)))
{
    gchar       *texte;
    GtkTextIter start, end;
    double      nombre;
    char        *fake;
    
    gtk_text_buffer_get_iter_at_offset(textbuffer, &start, 0);
    gtk_text_buffer_get_iter_at_offset(textbuffer, &end, -1);
    texte = gtk_text_buffer_get_text(textbuffer, &start, &end, FALSE);
    fake = (char*)malloc(sizeof(char)*(strlen(texte)+1));
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
