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
#include <gtk/gtk.h>

#include "common_projet.h"
#include "common_erreurs.h"
#include "1990_action.h"
#include "1992_1_1_barres.h"
#include "EF_rigidite.h"
#include "EF_calculs.h"

G_MODULE_EXPORT void EF_gtk_calculs_calculer(GtkMenuItem *menuitem __attribute__((unused)),
  Projet *projet)
/* Description : Exécute le calcul de la structure.
 * Paramètres : GtkMenuItem *menuitem : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    unsigned int i;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    
    BUG(EF_calculs_free(projet), );
    BUG(EF_calculs_initialise(projet), );
    BUG(_1992_1_1_barres_rigidite_ajout_tout(projet), );
    BUG(EF_calculs_genere_mat_rig(projet), );
    for (i=0;i<g_list_length(projet->actions);i++)
        BUG(EF_calculs_resoud_charge(projet, i), );
    
    gtk_widget_set_sensitive(projet->list_gtk.comp.menu_resultats_afficher, TRUE);
    
    return;
}


#endif
