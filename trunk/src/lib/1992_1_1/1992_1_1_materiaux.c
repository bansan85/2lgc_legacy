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
#include <stdlib.h>
#include <libintl.h>
#include <math.h>
#include <string.h>
#include <gmodule.h>

#include "common_projet.h"
#include "common_maths.h"
#include "common_erreurs.h"
#include "EF_calculs.h"


G_MODULE_EXPORT gboolean _1992_1_1_materiaux_init(Projet *projet)
/* Description : Initialise la liste des matériaux en béton.
 * Paramètres : Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL.
 */
{
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    
    // Trivial
    projet->beton.materiaux = NULL;
    
#ifdef ENABLE_GTK
    projet->list_gtk.ef_barres.liste_materiaux = gtk_list_store_new(1, G_TYPE_STRING);
#endif
    
    return TRUE;
}


G_MODULE_EXPORT gboolean _1992_1_1_materiaux_ajout(Projet *projet, const char *nom, double fck,
  double nu)
/* Description : Ajoute un matériau en béton et calcule ses caractéristiques mécaniques.
 *               Les propriétés du béton sont déterminées conformément au tableau 3.1 de
 *               l'Eurocode 2-1-1 les valeurs de fckcube est déterminée par interpolation
 *               linéaire si nécessaire.
 * Paramètres : Projet *projet : la variable projet,
 *            : double fck : résistance à la compression du béton à 28 jours en MPa,
 *            : double nu : coefficient de poisson pour un béton non fissuré.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL,
 *             projet->beton.materiaux == NULL,
 *             fck > 90.,
 *             en cas d'erreur d'allocation mémoire.
 */
{
    Beton_Materiau  *materiau_nouveau = malloc(sizeof(Beton_Materiau));
#ifdef ENABLE_GTK
    GtkTreeIter     iter;
#endif
    
    // Trivial
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG((fck > ERREUR_RELATIVE_MIN) && (fck <= 90.*(1+ERREUR_RELATIVE_MIN)), FALSE, gettext("La résistance caractéristique à la compression du béton doit être inférieure ou égale à 90 MPa.\n"));
    BUGMSG(materiau_nouveau, FALSE, gettext("Erreur d'allocation mémoire.\n"));

    materiau_nouveau->fck = fck*1000000.;
    BUGMSG(materiau_nouveau->nom = g_strdup_printf("%s", nom), FALSE, gettext("Erreur d'allocation mémoire.\n"));
    if (fck < 12.)
        materiau_nouveau->fckcube = fck*1.25*1000000.;
    else if (fck < 16.)
        materiau_nouveau->fckcube = 5.*fck/4.*1000000.;
    else if (fck < 20.)
        materiau_nouveau->fckcube = 5.*fck/4.*1000000.;
    else if (fck < 25.)
        materiau_nouveau->fckcube = (fck+5.)*1000000.;
    else if (fck < 30.)
        materiau_nouveau->fckcube = (7.*fck/5.-5.)*1000000.;
    else if (fck < 35.)
        materiau_nouveau->fckcube = (8.*fck/5.-11.)*1000000.;
    else if (fck < 40.)
        materiau_nouveau->fckcube = (fck+10.)*1000000.;
    else if (fck < 45.)
        materiau_nouveau->fckcube = (fck+10.)*1000000.;
    else if (fck < 50.)
        materiau_nouveau->fckcube = (fck+10.)*1000000.;
    else if (fck < 55.)
        materiau_nouveau->fckcube = (7.*fck/5.-10.)*1000000.;
    else if (fck < 60.)
        materiau_nouveau->fckcube = (8.*fck/5.-21.)*1000000.;
    else if (fck < 70.)
        materiau_nouveau->fckcube = (fck+15.)*1000000.;
    else if (fck < 80.)
        materiau_nouveau->fckcube = (fck+15.)*1000000.;
    else if (fck <= 90.)
        materiau_nouveau->fckcube = (fck+15.)*1000000.;
    materiau_nouveau->fcm = (fck+8.)*1000000.;
    if (fck <= 50.)
        materiau_nouveau->fctm = 0.3*pow(fck,2./3.)*1000000.;
    else
        materiau_nouveau->fctm = 2.12*log(1.+(materiau_nouveau->fcm/10./1000000.))*1000000.;
    materiau_nouveau->fctk_0_05 = 0.7*materiau_nouveau->fctm;
    materiau_nouveau->fctk_0_95 = 1.3*materiau_nouveau->fctm;
    materiau_nouveau->ecm = 22.*pow(materiau_nouveau->fcm/10./1000000., 0.3)*1000000000.;
    materiau_nouveau->ec1 = MIN(0.7*pow(materiau_nouveau->fcm/1000000., 0.31), 2.8)/1000.;
    if (fck < 50.)
        materiau_nouveau->ecu1 = 3.5/1000.;
    else
        materiau_nouveau->ecu1 = (2.8 + 27.*pow((98.-materiau_nouveau->fcm/1000000.)/100.,4.))/1000.;
    if (fck < 50.)
        materiau_nouveau->ec2 = 2./1000.;
    else
        materiau_nouveau->ec2 = (2. + 0.085*pow(fck-50., 0.53))/1000.;
    if (fck < 50.)
        materiau_nouveau->ecu2 = 3.5/1000.;
    else
        materiau_nouveau->ecu2 = (2.6 + 35.*pow((90.-fck)/100.,4.))/1000.;
    if (fck < 50.)
        materiau_nouveau->n = 2./1000.;
    else
        materiau_nouveau->n = (1.4 + 23.4*pow((90.-fck)/100., 4.))/1000.;
    if (fck < 50.)
        materiau_nouveau->ec3 = 1.75/1000.;
    else
        materiau_nouveau->ec3 = (1.75 + 0.55*(fck-50.)/40.)/1000.;
    if (fck < 50.)
        materiau_nouveau->ecu3 = 3.5/1000.;
    else
        materiau_nouveau->ecu3 = (2.6 + 35*pow((90.-fck)/100., 4.))/1000.;
    materiau_nouveau->nu = nu;
    materiau_nouveau->gnu_0_2 = materiau_nouveau->ecm/(2.*(1.+nu));
    materiau_nouveau->gnu_0_0 = materiau_nouveau->ecm/2.;
    
    projet->beton.materiaux = g_list_append(projet->beton.materiaux, materiau_nouveau);
    
#ifdef ENABLE_GTK
    gtk_list_store_append(projet->list_gtk.ef_barres.liste_materiaux, &iter);
    gtk_list_store_set(projet->list_gtk.ef_barres.liste_materiaux, &iter, 0, nom, -1);
#endif
    
    return TRUE;
}


G_MODULE_EXPORT Beton_Materiau* _1992_1_1_materiaux_cherche_nom(Projet *projet, const char *nom)
/* Description : Renvoie le matériau en fonction de son nom.
 * Paramètres : Projet *projet : la variable projet,
 *            : const char *nom : le nom du matériau.
 * Valeur renvoyée :
 *   Succès : pointeur vers le matériau en béton
 *   Échec : NULL :
 *             projet == NULL,
 *             materiau introuvable.
 */
{
    GList   *list_parcours;
    
    BUGMSG(projet, NULL, gettext("Paramètre %s incorrect.\n"), "projet");
    
    // Trivial
    list_parcours = projet->beton.materiaux;
    while (list_parcours != NULL)
    {
        Beton_Materiau  *materiau = list_parcours->data;
        
        if (strcmp(materiau->nom, nom) == 0)
            return materiau;
        
        list_parcours = g_list_next(list_parcours);
    }
    
    BUGMSG(0, NULL, gettext("Matériau en béton '%s' introuvable.\n"), nom);
}


G_MODULE_EXPORT gboolean _1992_1_1_materiaux_free(Projet *projet)
/* Description : Libère l'ensemble des matériaux en béton.
 * Paramètres : Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL.
 */
{
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    
    // Trivial
    while (projet->beton.materiaux != NULL)
    {
        Beton_Materiau *materiau = projet->beton.materiaux->data;
        projet->beton.materiaux = g_list_delete_link(projet->beton.materiaux, projet->beton.materiaux);
        free(materiau->nom);
        
        free(materiau);
    }
    
    BUG(EF_calculs_free(projet), TRUE);
    
#ifdef ENABLE_GTK
    g_object_unref(projet->list_gtk.ef_barres.liste_materiaux);
#endif
    
    return TRUE;
}
