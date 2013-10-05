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
#include <locale.h>
#include <libintl.h>
#include <math.h>
#include <string.h>
#include <gmodule.h>

#include "common_projet.h"
#include "common_math.h"
#include "common_erreurs.h"
#include "common_selection.h"
#include "1992_1_1_barres.h"
#include "1993_1_1_materiaux.h"
#include "EF_calculs.h"
#include "EF_materiaux.h"


EF_Materiau* _1993_1_1_materiaux_ajout(Projet *projet, const char *nom, Flottant fy,
  Flottant fu)
/* Description : Ajoute un matériau en acier.
 * Paramètres : Projet *projet : la variable projet,
 *            : const char *nom : nom du nouveau matériau,
 *            : Flottant fy : limite d’élasticité en MPa,
 *            : Flottant fu : résistance à la traction en MPa.
 * Valeur renvoyée :
 *   Succès : pointeur vers le nouveau matériau.
 *   Échec : NULL :
 *             projet == NULL,
 *             en cas d'erreur d'allocation mémoire.
 */
{
    EF_Materiau     *materiau_nouveau;
    Materiau_Acier  *data_acier;
    
    // Trivial
    BUGMSG(projet, NULL, gettext("Paramètre %s incorrect.\n"), "projet")
    BUGMSG(materiau_nouveau = malloc(sizeof(EF_Materiau)), NULL, gettext("Erreur d'allocation mémoire.\n"))
    BUGMSG(data_acier = malloc(sizeof(Materiau_Acier)), NULL, gettext("Erreur d'allocation mémoire.\n"))
    
    materiau_nouveau->type = MATERIAU_ACIER;
    materiau_nouveau->data = data_acier;
    
    BUGMSG(materiau_nouveau->nom = g_strdup_printf("%s", nom), NULL, gettext("Erreur d'allocation mémoire.\n"))
    
    data_acier->fy = common_math_f(common_math_get(fy)*1000000., fy.type);
    data_acier->fu = common_math_f(common_math_get(fu)*1000000., fu.type);
    data_acier->e = common_math_f(MODULE_YOUNG_ACIER, FLOTTANT_ORDINATEUR);
    data_acier->nu = common_math_f(COEFFICIENT_NU_ACIER, FLOTTANT_ORDINATEUR);
    
    BUG(EF_materiaux_insert(projet, materiau_nouveau), NULL)
    
    return materiau_nouveau;
}


gboolean _1993_1_1_materiaux_modif(Projet *projet, EF_Materiau *materiau, char *nom,
  Flottant fy, Flottant fu, Flottant e, Flottant nu)
/* Description : Modifie un matériau acier.
 * Paramètres : Projet *projet : la variable projet,
 *            : EF_Materiau *materiau : le matériau à modifier,
 *            : Autres : caractéristiques du matériau. Pour ne pas modifier un paramètre,
 *                il suffit de mettre NULL pour le nom et NAN pour les nombres.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL,
 *             materiau == NULL.
 */
{
    Materiau_Acier  *data_acier;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet")
    BUGMSG(materiau, FALSE, gettext("Paramètre %s incorrect.\n"), "materiau")
    BUGMSG(materiau->type == MATERIAU_ACIER, FALSE, gettext("Le matériau n'est pas en acier.\n"))
    
    data_acier = materiau->data;
    
    if ((nom != NULL) && (strcmp(materiau->nom, nom) != 0))
    {
        BUGMSG(!EF_materiaux_cherche_nom(projet, nom, FALSE), FALSE, gettext("Le matériau %s existe déjà.\n"), nom)
        free(materiau->nom);
        BUGMSG(materiau->nom = g_strdup_printf("%s", nom), FALSE, gettext("Erreur d'allocation mémoire.\n"))
        BUG(EF_materiaux_repositionne(projet, materiau), FALSE)
    }
    
    if (!isnan(common_math_get(fy)))
        data_acier->fy = fy;
    if (!isnan(common_math_get(fu)))
        data_acier->fu = fu;
    if (!isnan(common_math_get(e)))
        data_acier->e = e;
    if (!isnan(common_math_get(nu)))
        data_acier->nu = nu;
    
    if ((!isnan(common_math_get(fy))) || (!isnan(common_math_get(fu))) || (!isnan(common_math_get(e))) || (!isnan(common_math_get(nu))))
    {
        GList   *liste_materiaux = NULL;
        GList   *liste_barres_dep;
        
        liste_materiaux = g_list_append(liste_materiaux, materiau);
        BUG(_1992_1_1_barres_cherche_dependances(projet, NULL, NULL, NULL, liste_materiaux, NULL, NULL, NULL, &liste_barres_dep, NULL, FALSE, FALSE), FALSE)
        g_list_free(liste_materiaux);
        
        if (liste_barres_dep != NULL)
            BUG(EF_calculs_free(projet), FALSE)
        
        g_list_free(liste_barres_dep);
    }
    
#ifdef ENABLE_GTK
    if (projet->list_gtk.ef_materiaux.builder != NULL)
        gtk_widget_queue_resize(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_materiaux.builder, "EF_materiaux_treeview")));
#endif
    
    return TRUE;
}


char *_1993_1_1_materiaux_get_description(EF_Materiau* materiau)
/* Description : Renvoie la description d'un matériau acier sous forme d'un texte.
 *               Il convient de libérer le texte renvoyée par la fonction free.
 * Paramètres : EF_Materiau* materiau : matériau à décrire.
 * Valeur renvoyée :
 *   Succès : Résultat
 *   Échec : NULL :
 *             (materiau == NULL),
 *             erreur d'allocation mémoire.
 */
{
    char            *description = NULL;
    char            tmp1[30];
    char            *tmp2;
    Materiau_Acier  *data_acier;
    
    BUGMSG(materiau, NULL, gettext("Paramètre %s incorrect.\n"), "sect")
    BUGMSG(materiau->type == MATERIAU_ACIER, FALSE, gettext("Le matériau n'est pas en acier.\n"))
    
    data_acier = materiau->data;
    
    common_math_double_to_char2(common_math_f(common_math_get(data_acier->fy)/1000000., data_acier->fy.type), tmp1, DECIMAL_CONTRAINTE);
    BUGMSG(description = g_strdup_printf("f<sub>y</sub> : %s MPa", tmp1), NULL, gettext("Erreur d'allocation mémoire.\n"))
    
    // On affiche les différences si le matériau a été personnalisé
    common_math_double_to_char2(common_math_f(common_math_get(data_acier->fu)/1000000., data_acier->fu.type), tmp1, DECIMAL_CONTRAINTE);
    tmp2 = description;
    BUGMSG(description = g_strdup_printf("%s, f<sub>u</sub> : %s MPa", tmp2, tmp1), NULL, gettext("Erreur d'allocation mémoire.\n"))
    free(tmp2);
    
    if (!ERREUR_RELATIVE_EGALE(common_math_get(data_acier->e), MODULE_YOUNG_ACIER))
    {
        common_math_double_to_char2(common_math_f(common_math_get(data_acier->e)/1000000., data_acier->e.type), tmp1, DECIMAL_CONTRAINTE);
        tmp2 = description;
        BUGMSG(description = g_strdup_printf("%s, E : %s MPa", tmp2, tmp1), NULL, gettext("Erreur d'allocation mémoire.\n"))
        free(tmp2);
    }
    
    if (!ERREUR_RELATIVE_EGALE(common_math_get(data_acier->nu), COEFFICIENT_NU_ACIER))
    {
        common_math_double_to_char2(data_acier->nu, tmp1, DECIMAL_SANS_UNITE);
        tmp2 = description;
        BUGMSG(description = g_strdup_printf("%s, &#957; : %s", tmp2, tmp1), NULL, gettext("Erreur d'allocation mémoire.\n"))
        free(tmp2);
    }
    
    return description;
}
