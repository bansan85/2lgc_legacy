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
#include "1992_1_1_materiaux.h"
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
    projet->list_gtk.ef_materiaux.liste_materiaux = gtk_list_store_new(1, G_TYPE_STRING);
#endif
    
    return TRUE;
}


G_MODULE_EXPORT double _1992_1_1_materiaux_fckcube(double fck)
/* Description : Renvoie la résistance caractéristique en compression du béton, mesurée sur
 *               cube à 28 jours en fonction de fck en unité SI (Pa).
 * Paramètres : double fck : Résistance caractéristique en compression du béton, mesurée sur
 *                           cylindre à 28 jours, en MPa.
 * Valeur renvoyée :
 *   Succès : le résultat,
 *   Échec : NAN :
 *             fck > 90.,
 *             fck <= 0.
 */
{
    BUGMSG((fck > ERREUR_RELATIVE_MIN) && (fck <= 90.*(1+ERREUR_RELATIVE_MIN)), NAN, gettext("La résistance caractéristique à la compression du béton doit être inférieure ou égale à 90 MPa.\n"));
    
    // On effectue une interpolation linéaire entre les valeurs fournies par la norme
    if (fck < 12.)
        return fck*1.25*1000000.;
    else if (fck < 16.)
        return 5.*fck/4.*1000000.;
    else if (fck < 20.)
        return 5.*fck/4.*1000000.;
    else if (fck < 25.)
        return (fck+5.)*1000000.;
    else if (fck < 30.)
        return (7.*fck/5.-5.)*1000000.;
    else if (fck < 35.)
        return (8.*fck/5.-11.)*1000000.;
    else if (fck < 40.)
        return (fck+10.)*1000000.;
    else if (fck < 45.)
        return (fck+10.)*1000000.;
    else if (fck < 50.)
        return (fck+10.)*1000000.;
    else if (fck < 55.)
        return (7.*fck/5.-10.)*1000000.;
    else if (fck < 60.)
        return (8.*fck/5.-21.)*1000000.;
    else if (fck < 70.)
        return (fck+15.)*1000000.;
    else if (fck < 80.)
        return (fck+15.)*1000000.;
    else if (fck <= 90.)
        return (fck+15.)*1000000.;
    else
        return NAN;
}


G_MODULE_EXPORT double _1992_1_1_materiaux_fcm(double fck)
/* Description : Renvoie la valeur moyenne de la résistance en compression du béton, mesurée sur
 *               cylindre en unité SI (Pa).
 * Paramètres : double fck : Résistance caractéristique en compression du béton, mesurée sur
 *                           cylindre à 28 jours, en MPa.
 * Valeur renvoyée :
 *   Succès : le résultat,
 *   Échec : NAN :
 *             fck > 90.,
 *             fck <= 0.
 */
{
    BUGMSG((fck > ERREUR_RELATIVE_MIN) && (fck <= 90.*(1+ERREUR_RELATIVE_MIN)), NAN, gettext("La résistance caractéristique à la compression du béton doit être inférieure ou égale à 90 MPa.\n"));
    
    return (fck+8.)*1000000.;
}


G_MODULE_EXPORT double _1992_1_1_materiaux_fctm(double fck)
/* Description : Renvoie la valeur moyenne de la résistance en traction directe du béton,
 *               mesurée sur cylindre en unité SI (Pa).
 * Paramètres : double fck : Résistance caractéristique en compression du béton, mesurée sur
 *                           cylindre à 28 jours, en MPa.
 * Valeur renvoyée :
 *   Succès : le résultat,
 *   Échec : NAN :
 *             fck > 90.,
 *             fck <= 0.
 */
{
    BUGMSG((fck > ERREUR_RELATIVE_MIN) && (fck <= 90.*(1+ERREUR_RELATIVE_MIN)), NAN, gettext("La résistance caractéristique à la compression du béton doit être inférieure ou égale à 90 MPa.\n"));
    
    if (fck <= 50.)
        return 0.3*pow(fck,2./3.)*1000000.;
    else
        return 2.12*log(1.+(_1992_1_1_materiaux_fcm(fck)/10./1000000.))*1000000.;
}


G_MODULE_EXPORT double _1992_1_1_materiaux_fctk_0_05(double fck)
/* Description : Renvoie la valeur moyenne de la résistance en traction directe du béton,
 *               (fractile 5%) en unité SI (Pa).
 * Paramètres : double fck : Résistance caractéristique en compression du béton, mesurée sur
 *                           cylindre à 28 jours, en MPa.
 * Valeur renvoyée :
 *   Succès : le résultat,
 *   Échec : NAN :
 *             fck > 90.,
 *             fck <= 0.
 */
{
    BUGMSG((fck > ERREUR_RELATIVE_MIN) && (fck <= 90.*(1+ERREUR_RELATIVE_MIN)), NAN, gettext("La résistance caractéristique à la compression du béton doit être inférieure ou égale à 90 MPa.\n"));
    
    return 0.7*_1992_1_1_materiaux_fctm(fck);
}


G_MODULE_EXPORT double _1992_1_1_materiaux_fctk_0_95(double fck)
/* Description : Renvoie la valeur moyenne de la résistance en traction directe du béton,
 *               (fractile 95%) en unité SI (Pa).
 * Paramètres : double fck : Résistance caractéristique en compression du béton, mesurée sur
 *                           cylindre à 28 jours, en MPa.
 * Valeur renvoyée :
 *   Succès : le résultat,
 *   Échec : NAN :
 *             fck > 90.,
 *             fck <= 0.
 */
{
    BUGMSG((fck > ERREUR_RELATIVE_MIN) && (fck <= 90.*(1+ERREUR_RELATIVE_MIN)), NAN, gettext("La résistance caractéristique à la compression du béton doit être inférieure ou égale à 90 MPa.\n"));
    
    return 1.3*_1992_1_1_materiaux_fctm(fck);
}


G_MODULE_EXPORT double _1992_1_1_materiaux_ecm(double fck)
/* Description : Renvoie le module d'élasticité sécant du béton en unité SI (Pa).
 * Paramètres : double fck : Résistance caractéristique en compression du béton, mesurée sur
 *                           cylindre à 28 jours, en MPa.
 * Valeur renvoyée :
 *   Succès : le résultat,
 *   Échec : NAN :
 *             fck > 90.,
 *             fck <= 0.
 */
{
    BUGMSG((fck > ERREUR_RELATIVE_MIN) && (fck <= 90.*(1+ERREUR_RELATIVE_MIN)), NAN, gettext("La résistance caractéristique à la compression du béton doit être inférieure ou égale à 90 MPa.\n"));
    
    return 22.*pow(_1992_1_1_materiaux_fcm(fck)/10./1000000., 0.3)*1000000000.;
}


G_MODULE_EXPORT double _1992_1_1_materiaux_ec1(double fck)
/* Description : Renvoie la déformation relative en compression du béton au point 1.
 * Paramètres : double fck : Résistance caractéristique en compression du béton, mesurée sur
 *                           cylindre à 28 jours, en MPa.
 * Valeur renvoyée :
 *   Succès : le résultat,
 *   Échec : NAN :
 *             fck > 90.,
 *             fck <= 0.
 */
{
    BUGMSG((fck > ERREUR_RELATIVE_MIN) && (fck <= 90.*(1+ERREUR_RELATIVE_MIN)), NAN, gettext("La résistance caractéristique à la compression du béton doit être inférieure ou égale à 90 MPa.\n"));
    
    return MIN(0.7*pow(_1992_1_1_materiaux_fcm(fck)/1000000., 0.31), 2.8)/1000.;
}


G_MODULE_EXPORT double _1992_1_1_materiaux_ecu1(double fck)
/* Description : Renvoie la déformation relative ultime en compression du béton au point 1.
 * Paramètres : double fck : Résistance caractéristique en compression du béton, mesurée sur
 *                           cylindre à 28 jours, en MPa.
 * Valeur renvoyée :
 *   Succès : le résultat,
 *   Échec : NAN :
 *             fck > 90.,
 *             fck <= 0.
 */
{
    BUGMSG((fck > ERREUR_RELATIVE_MIN) && (fck <= 90.*(1+ERREUR_RELATIVE_MIN)), NAN, gettext("La résistance caractéristique à la compression du béton doit être inférieure ou égale à 90 MPa.\n"));
    
    if (fck < 50.)
        return 3.5/1000.;
    else
        return (2.8 + 27.*pow((98.-_1992_1_1_materiaux_fcm(fck)/1000000.)/100.,4.))/1000.;
}


G_MODULE_EXPORT double _1992_1_1_materiaux_ec2(double fck)
/* Description : Renvoie la déformation relative en compression du béton au point 2.
 * Paramètres : double fck : Résistance caractéristique en compression du béton, mesurée sur
 *                           cylindre à 28 jours, en MPa.
 * Valeur renvoyée :
 *   Succès : le résultat,
 *   Échec : NAN :
 *             fck > 90.,
 *             fck <= 0.
 */
{
    BUGMSG((fck > ERREUR_RELATIVE_MIN) && (fck <= 90.*(1+ERREUR_RELATIVE_MIN)), NAN, gettext("La résistance caractéristique à la compression du béton doit être inférieure ou égale à 90 MPa.\n"));
    
    if (fck < 50.)
        return 2./1000.;
    else
        return (2. + 0.085*pow(fck-50., 0.53))/1000.;
}


G_MODULE_EXPORT double _1992_1_1_materiaux_ecu2(double fck)
/* Description : Renvoie la déformation relative ultime en compression du béton au point 2.
 * Paramètres : double fck : Résistance caractéristique en compression du béton, mesurée sur
 *                           cylindre à 28 jours, en MPa.
 * Valeur renvoyée :
 *   Succès : le résultat,
 *   Échec : NAN :
 *             fck > 90.,
 *             fck <= 0.
 */
{
    BUGMSG((fck > ERREUR_RELATIVE_MIN) && (fck <= 90.*(1+ERREUR_RELATIVE_MIN)), NAN, gettext("La résistance caractéristique à la compression du béton doit être inférieure ou égale à 90 MPa.\n"));
    
    if (fck < 50.)
        return 3.5/1000.;
    else
        return (2.6 + 35.*pow((90.-fck)/100.,4.))/1000.;
}


G_MODULE_EXPORT double _1992_1_1_materiaux_ec3(double fck)
/* Description : Renvoie la déformation relative en compression du béton au point 3.
 * Paramètres : double fck : Résistance caractéristique en compression du béton, mesurée sur
 *                           cylindre à 28 jours, en MPa.
 * Valeur renvoyée :
 *   Succès : le résultat,
 *   Échec : NAN :
 *             fck > 90.,
 *             fck <= 0.
 */
{
    BUGMSG((fck > ERREUR_RELATIVE_MIN) && (fck <= 90.*(1+ERREUR_RELATIVE_MIN)), NAN, gettext("La résistance caractéristique à la compression du béton doit être inférieure ou égale à 90 MPa.\n"));
    
    if (fck < 50.)
        return 1.75/1000.;
    else
        return (1.75 + 0.55*(fck-50.)/40.)/1000.;
}


G_MODULE_EXPORT double _1992_1_1_materiaux_ecu3(double fck)
/* Description : Renvoie la déformation relative ultime en compression du béton au point 3.
 * Paramètres : double fck : Résistance caractéristique en compression du béton, mesurée sur
 *                           cylindre à 28 jours, en MPa.
 * Valeur renvoyée :
 *   Succès : le résultat,
 *   Échec : NAN :
 *             fck > 90.,
 *             fck <= 0.
 */
{
    BUGMSG((fck > ERREUR_RELATIVE_MIN) && (fck <= 90.*(1+ERREUR_RELATIVE_MIN)), NAN, gettext("La résistance caractéristique à la compression du béton doit être inférieure ou égale à 90 MPa.\n"));
    
    if (fck < 50.)
        return 3.5/1000.;
    else
        return (2.6 + 35*pow((90.-fck)/100., 4.))/1000.;
}


G_MODULE_EXPORT double _1992_1_1_materiaux_n(double fck)
/* Description : Renvoie le coefficient n utilisé dans la courbe parabole rectangle.
 * Paramètres : double fck : Résistance caractéristique en compression du béton, mesurée sur
 *                           cylindre à 28 jours, en MPa.
 * Valeur renvoyée :
 *   Succès : le résultat,
 *   Échec : NAN :
 *             fck > 90.,
 *             fck <= 0.
 */
{
    BUGMSG((fck > ERREUR_RELATIVE_MIN) && (fck <= 90.*(1+ERREUR_RELATIVE_MIN)), NAN, gettext("La résistance caractéristique à la compression du béton doit être inférieure ou égale à 90 MPa.\n"));
    
    if (fck < 50.)
        return 2./1000.;
    else
        return (1.4 + 23.4*pow((90.-fck)/100., 4.))/1000.;
}


G_MODULE_EXPORT double _1992_1_1_materiaux_gnu(double fck, double nu)
/* Description : Renvoie le module de cisallement en unité SI (Pa).
 * Paramètres : double fck : Résistance caractéristique en compression du béton, mesurée sur
 *                           cylindre à 28 jours, en MPa.
 * Valeur renvoyée :
 *   Succès : le résultat,
 *   Échec : NAN :
 *             fck > 90.,
 *             fck <= 0.
 */
{
    BUGMSG((fck > ERREUR_RELATIVE_MIN) && (fck <= 90.*(1+ERREUR_RELATIVE_MIN)), NAN, gettext("La résistance caractéristique à la compression du béton doit être inférieure ou égale à 90 MPa.\n"));
    
    return _1992_1_1_materiaux_ecm(fck)/(2.*(1.+nu));
}


#ifdef ENABLE_GTK
G_MODULE_EXPORT gboolean _1992_1_1_materiaux_update_ligne_treeview(Projet *projet,
  Beton_Materiau *materiau)
/* Description : Met à jour les données dans le treeview de la fenêtre matériau.
 * Paramètres : Projet *projet : la variable projet,
 *            : Beton_Materiau *materiau : le matériau à mettre à jour.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL,
 *             section == NULL,
 *             fenetre matériau non initialisée.
 */
{
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(materiau, FALSE, gettext("Paramètre %s incorrect.\n"), "materiau");
    
    if (projet->list_gtk.ef_materiaux.builder != NULL)
    {
        char        *description;
        
        BUG(description = _1992_1_1_materiaux_get_description(materiau), FALSE);
        
        gtk_tree_store_set(projet->list_gtk.ef_materiaux.materiaux, &materiau->Iter_fenetre, 0, materiau->nom, 1, gettext("Béton"), 2, description, -1);
        free(description);
    }
    
    return TRUE;
}
#endif


G_MODULE_EXPORT gboolean _1992_1_1_materiaux_ajout(Projet *projet, const char *nom, double fck)
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
    
    // Trivial
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG((fck > ERREUR_RELATIVE_MIN) && (fck <= 90.*(1+ERREUR_RELATIVE_MIN)), FALSE, gettext("La résistance caractéristique à la compression du béton doit être inférieure ou égale à 90 MPa.\n"));
    BUGMSG(materiau_nouveau, FALSE, gettext("Erreur d'allocation mémoire.\n"));

    materiau_nouveau->fck = fck*1000000.;
    BUGMSG(materiau_nouveau->nom = g_strdup_printf("%s", nom), FALSE, gettext("Erreur d'allocation mémoire.\n"));
    
    BUG(!isnan(materiau_nouveau->fckcube = _1992_1_1_materiaux_fckcube(fck)), FALSE);
    BUG(!isnan(materiau_nouveau->fcm = _1992_1_1_materiaux_fcm(fck)), FALSE);
    BUG(!isnan(materiau_nouveau->fctm = _1992_1_1_materiaux_fctm(fck)), FALSE);
    BUG(!isnan(materiau_nouveau->fctk_0_05 = _1992_1_1_materiaux_fctk_0_05(fck)), FALSE);
    BUG(!isnan(materiau_nouveau->fctk_0_95 = _1992_1_1_materiaux_fctk_0_95(fck)), FALSE);
    BUG(!isnan(materiau_nouveau->ecm = _1992_1_1_materiaux_ecm(fck)), FALSE);
    BUG(!isnan(materiau_nouveau->ec1 = _1992_1_1_materiaux_ec1(fck)), FALSE);
    BUG(!isnan(materiau_nouveau->ecu1 = _1992_1_1_materiaux_ecu1(fck)), FALSE);
    BUG(!isnan(materiau_nouveau->ec2 = _1992_1_1_materiaux_ec2(fck)), FALSE);
    BUG(!isnan(materiau_nouveau->ecu2 = _1992_1_1_materiaux_ecu2(fck)), FALSE);
    BUG(!isnan(materiau_nouveau->ec3 = _1992_1_1_materiaux_ec3(fck)), FALSE);
    BUG(!isnan(materiau_nouveau->ecu3 = _1992_1_1_materiaux_ecu3(fck)), FALSE);
    BUG(!isnan(materiau_nouveau->n = _1992_1_1_materiaux_n(fck)), FALSE);
    materiau_nouveau->nu = COEFFICIENT_NU_BETON;
    BUG(!isnan(materiau_nouveau->gnu_0_2 = _1992_1_1_materiaux_gnu(fck, COEFFICIENT_NU_BETON)), FALSE);
    BUG(!isnan(materiau_nouveau->gnu_0_0 = _1992_1_1_materiaux_gnu(fck, 0)), FALSE);
    
    projet->beton.materiaux = g_list_append(projet->beton.materiaux, materiau_nouveau);
    
#ifdef ENABLE_GTK
    gtk_list_store_append(projet->list_gtk.ef_materiaux.liste_materiaux, &materiau_nouveau->Iter_liste);
    gtk_list_store_set(projet->list_gtk.ef_materiaux.liste_materiaux, &materiau_nouveau->Iter_liste, 0, nom, -1);
    if (projet->list_gtk.ef_materiaux.builder != NULL)
    {
        char        *description;
        
        BUG(description = _1992_1_1_materiaux_get_description(materiau_nouveau), FALSE);
        
        gtk_tree_store_append(projet->list_gtk.ef_materiaux.materiaux, &materiau_nouveau->Iter_fenetre, NULL);
        gtk_tree_store_set(projet->list_gtk.ef_materiaux.materiaux, &materiau_nouveau->Iter_fenetre, 0, materiau_nouveau->nom, 1, gettext("Béton"), 2, description, -1);
        
        free(description);
    }
#endif
    
    return TRUE;
}


G_MODULE_EXPORT Beton_Materiau* _1992_1_1_materiaux_cherche_nom(Projet *projet, const char *nom,
  gboolean critique)
/* Description : Renvoie le matériau en fonction de son nom.
 * Paramètres : Projet *projet : la variable projet,
 *            : const char *nom : le nom du matériau.
 *            : gboolean critique : utilise BUG si TRUE, return sinon
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
    
    if (critique)
        BUGMSG(0, NULL, gettext("Matériau en béton '%s' introuvable.\n"), nom);
    else
        return NULL;
}


G_MODULE_EXPORT gboolean _1992_1_1_materiaux_renomme(Beton_Materiau *materiau, gchar *nom,
  Projet *projet)
/* Description : Renomme un matériau.
 * Paramètres : Beton_Materiau *materiau : materiau à renommer,
 *            : const char *nom : le nouveau nom,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL,
 *             section == NULL,
 *             materiau possédant le nouveau nom est déjà existant.
 */
{
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(materiau, FALSE, gettext("Paramètre %s incorrect.\n"), "materiau");
    BUGMSG(_1992_1_1_materiaux_cherche_nom(projet, nom, FALSE) == NULL, FALSE, gettext("Le matériau '%s' existe déjà.\n"), nom);
    
    free(materiau->nom);
    BUGMSG(materiau->nom = g_strdup_printf("%s", nom), FALSE, gettext("Erreur d'allocation mémoire.\n"));
    
#ifdef ENABLE_GTK
    if (projet->list_gtk.ef_materiaux.builder != NULL)
        gtk_tree_store_set(projet->list_gtk.ef_materiaux.materiaux, &materiau->Iter_fenetre, 0, nom, -1);
    gtk_list_store_set(projet->list_gtk.ef_materiaux.liste_materiaux, &materiau->Iter_liste, 0, nom, -1);
#endif
    
    return TRUE;
}


G_MODULE_EXPORT gboolean _1992_1_1_materiaux_cherche_dependances(Projet *projet,
  Beton_Materiau* materiau, GList** barres_dep)
/* Description : Liste l'ensemble des barres utilisant le materiau.
 * Paramètres : Projet *projet : la variable projet,
 *            : Beton_Materiau* materiau : le matériau à analyser,
 *            : GList** barres_dep : la liste des barres dépendantes.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL,
 *             section == NULL.
 */
{
    GList   *list_parcours;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(materiau, FALSE, gettext("Paramètre %s incorrect.\n"), "materiau");
    
    *barres_dep = NULL;
    
    list_parcours = projet->beton.barres;
    while (list_parcours != NULL)
    {
        Beton_Barre *barre = list_parcours->data;
        
        if (barre->materiau == materiau)
            *barres_dep = g_list_append(*barres_dep, barre);
        
        list_parcours = g_list_next(list_parcours);
    }
    
    return TRUE;
}


G_MODULE_EXPORT gboolean _1992_1_1_materiaux_verifie_dependances(Projet *projet,
  Beton_Materiau* materiau)
/* Description : Vérifie si le matériau est utilisé.
 * Paramètres : Projet *projet : la variable projet,
 *            : EF_Section *section : la section à analyser,
 * Valeur renvoyée :
 *   Succès : TRUE si la section est utilisée et FALSE s'il ne l'est pas.
 *   Échec : FALSE :
 *             projet == NULL,
 *             section == NULL.
 */
{
    GList   *list_parcours;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(materiau, FALSE, gettext("Paramètre %s incorrect.\n"), "materiau");
    
    list_parcours = projet->beton.barres;
    while (list_parcours != NULL)
    {
        Beton_Barre *barre = list_parcours->data;
        
        if (barre->materiau == materiau)
            return TRUE;
        
        list_parcours = g_list_next(list_parcours);
    }
    
    return FALSE;
}


G_MODULE_EXPORT char *_1992_1_1_materiaux_get_description(Beton_Materiau* materiau)
/* Description : Renvoie la description d'un matériau béton sous forme d'un texte.
 *               Il convient de libérer le texte renvoyée par la fonction free.
 * Paramètres : Beton_Materiau* materiau : section à étudier.
 * Valeur renvoyée :
 *   Succès : Résultat
 *   Échec : NULL :
 *             (materiau == NULL),
 *             erreur d'allocation mémoire.
 */
{
    char    *description;
    char    fck[30], tmp1[30];
    char    *complement = NULL, *tmp2;
    
    BUGMSG(materiau, NULL, gettext("Paramètre %s incorrect.\n"), "sect");
    
    common_math_double_to_char(materiau->fck/1000000., fck, DECIMAL_CONTRAINTE);
    
    // On affiche les différences si le matériau a été personnalisé
    if (!ERREUR_RELATIVE_EGALE(materiau->fckcube, _1992_1_1_materiaux_fckcube(materiau->fck/1000000.)))
    {
        common_math_double_to_char(materiau->fckcube/1000000., tmp1, DECIMAL_CONTRAINTE);
        if (complement == NULL)
            BUGMSG(complement = g_strdup_printf("f<sub>ck,cube</sub> : %s MPa", tmp1), NULL, gettext("Erreur d'allocation mémoire.\n"));
        else
        {
            tmp2 = complement;
            BUGMSG(complement = g_strdup_printf("%s, f<sub>ck,cube</sub> : %s MPa", tmp2, tmp1), NULL, gettext("Erreur d'allocation mémoire.\n"));
            free(tmp2);
        }
    }
    if (!ERREUR_RELATIVE_EGALE(materiau->fcm, _1992_1_1_materiaux_fcm(materiau->fck/1000000.)))
    {
        common_math_double_to_char(materiau->fcm/1000000., tmp1, DECIMAL_CONTRAINTE);
        if (complement == NULL)
            BUGMSG(complement = g_strdup_printf("f<sub>cm</sub> : %s MPa", tmp1), NULL, gettext("Erreur d'allocation mémoire.\n"));
        else
        {
            tmp2 = complement;
            BUGMSG(complement = g_strdup_printf("%s, f<sub>cm</sub> : %s MPa", tmp2, tmp1), NULL, gettext("Erreur d'allocation mémoire.\n"));
            free(tmp2);
        }
    }
    if (!ERREUR_RELATIVE_EGALE(materiau->fctm, _1992_1_1_materiaux_fctm(materiau->fck/1000000.)))
    {
        common_math_double_to_char(materiau->fctm/1000000., tmp1, DECIMAL_CONTRAINTE);
        if (complement == NULL)
            BUGMSG(complement = g_strdup_printf("f<sub>ctm</sub> : %s MPa", tmp1), NULL, gettext("Erreur d'allocation mémoire.\n"));
        else
        {
            tmp2 = complement;
            BUGMSG(complement = g_strdup_printf("%s, f<sub>ctm</sub> : %s MPa", tmp2, tmp1), NULL, gettext("Erreur d'allocation mémoire.\n"));
            free(tmp2);
        }
    }
    if (!ERREUR_RELATIVE_EGALE(materiau->fctk_0_05, _1992_1_1_materiaux_fctk_0_05(materiau->fck/1000000.)))
    {
        common_math_double_to_char(materiau->fctk_0_05/1000000., tmp1, DECIMAL_CONTRAINTE);
        if (complement == NULL)
            BUGMSG(complement = g_strdup_printf("f<sub>ctk,0.05</sub> : %s MPa", tmp1), NULL, gettext("Erreur d'allocation mémoire.\n"));
        else
        {
            tmp2 = complement;
            BUGMSG(complement = g_strdup_printf("%s, f<sub>ctk,0.05</sub> : %s MPa", tmp2, tmp1), NULL, gettext("Erreur d'allocation mémoire.\n"));
            free(tmp2);
        }
    }
    if (!ERREUR_RELATIVE_EGALE(materiau->fctk_0_95, _1992_1_1_materiaux_fctk_0_95(materiau->fck/1000000.)))
    {
        common_math_double_to_char(materiau->fctk_0_95/1000000., tmp1, DECIMAL_CONTRAINTE);
        if (complement == NULL)
            BUGMSG(complement = g_strdup_printf("f<sub>ctk,0.95</sub> : %s MPa", tmp1), NULL, gettext("Erreur d'allocation mémoire.\n"));
        else
        {
            tmp2 = complement;
            BUGMSG(complement = g_strdup_printf("%s, f<sub>ctk,0.95</sub> : %s MPa", tmp2, tmp1), NULL, gettext("Erreur d'allocation mémoire.\n"));
            free(tmp2);
        }
    }
    if (!ERREUR_RELATIVE_EGALE(materiau->ecm, _1992_1_1_materiaux_ecm(materiau->fck/1000000.)))
    {
        common_math_double_to_char(materiau->ecm/1000000., tmp1, DECIMAL_CONTRAINTE);
        if (complement == NULL)
            BUGMSG(complement = g_strdup_printf("E<sub>cm</sub> : %s MPa", tmp1), NULL, gettext("Erreur d'allocation mémoire.\n"));
        else
        {
            tmp2 = complement;
            BUGMSG(complement = g_strdup_printf("%s, E<sub>cm</sub> : %s MPa", tmp2, tmp1), NULL, gettext("Erreur d'allocation mémoire.\n"));
            free(tmp2);
        }
    }
    if (!ERREUR_RELATIVE_EGALE(materiau->gnu_0_2, _1992_1_1_materiaux_gnu(materiau->fck/1000000., 0.2)))
    {
        common_math_double_to_char(materiau->gnu_0_2/1000000., tmp1, DECIMAL_CONTRAINTE);
        if (complement == NULL)
            BUGMSG(complement = g_strdup_printf("G (&#957;=0.2) : %s MPa", tmp1), NULL, gettext("Erreur d'allocation mémoire.\n"));
        else
        {
            tmp2 = complement;
            BUGMSG(complement = g_strdup_printf("%s, G (&#957; = 0.2) : %s MPa", tmp2, tmp1), NULL, gettext("Erreur d'allocation mémoire.\n"));
            free(tmp2);
        }
    }
    if (!ERREUR_RELATIVE_EGALE(materiau->gnu_0_0, _1992_1_1_materiaux_gnu(materiau->fck/1000000., 0.)))
    {
        common_math_double_to_char(materiau->gnu_0_0/1000000., tmp1, DECIMAL_CONTRAINTE);
        if (complement == NULL)
            BUGMSG(complement = g_strdup_printf("G (&#957;=0) : %s MPa", tmp1), NULL, gettext("Erreur d'allocation mémoire.\n"));
        else
        {
            tmp2 = complement;
            BUGMSG(complement = g_strdup_printf("%s, G (&#957;=0) : %s MPa", tmp2, tmp1), NULL, gettext("Erreur d'allocation mémoire.\n"));
            free(tmp2);
        }
    }
    if (!ERREUR_RELATIVE_EGALE(materiau->ec1, _1992_1_1_materiaux_ec1(materiau->fck/1000000.)))
    {
        common_math_double_to_char(materiau->ec1*1000., tmp1, DECIMAL_SANS_UNITE);
        if (complement == NULL)
            BUGMSG(complement = g_strdup_printf("&#949;<sub>c1</sub> : %s ‰", tmp1), NULL, gettext("Erreur d'allocation mémoire.\n"));
        else
        {
            tmp2 = complement;
            BUGMSG(complement = g_strdup_printf("%s, &#949;<sub>c1</sub> : %s ‰", tmp2, tmp1), NULL, gettext("Erreur d'allocation mémoire.\n"));
            free(tmp2);
        }
    }
    if (!ERREUR_RELATIVE_EGALE(materiau->ecu1, _1992_1_1_materiaux_ecu1(materiau->fck/1000000.)))
    {
        common_math_double_to_char(materiau->ecu1*1000., tmp1, DECIMAL_SANS_UNITE);
        if (complement == NULL)
            BUGMSG(complement = g_strdup_printf("&#949;<sub>cu1</sub> : %s ‰", tmp1), NULL, gettext("Erreur d'allocation mémoire.\n"));
        else
        {
            tmp2 = complement;
            BUGMSG(complement = g_strdup_printf("%s, &#949;<sub>cu1</sub> : %s ‰", tmp2, tmp1), NULL, gettext("Erreur d'allocation mémoire.\n"));
            free(tmp2);
        }
    }
    if (!ERREUR_RELATIVE_EGALE(materiau->ec2, _1992_1_1_materiaux_ec2(materiau->fck/1000000.)))
    {
        common_math_double_to_char(materiau->ec2*1000., tmp1, DECIMAL_SANS_UNITE);
        if (complement == NULL)
            BUGMSG(complement = g_strdup_printf("&#949;<sub>c2</sub> : %s ‰", tmp1), NULL, gettext("Erreur d'allocation mémoire.\n"));
        else
        {
            tmp2 = complement;
            BUGMSG(complement = g_strdup_printf("%s, &#949;<sub>c2</sub> : %s ‰", tmp2, tmp1), NULL, gettext("Erreur d'allocation mémoire.\n"));
            free(tmp2);
        }
    }
    if (!ERREUR_RELATIVE_EGALE(materiau->ecu2, _1992_1_1_materiaux_ecu2(materiau->fck/1000000.)))
    {
        common_math_double_to_char(materiau->ecu2*1000., tmp1, DECIMAL_SANS_UNITE);
        if (complement == NULL)
            BUGMSG(complement = g_strdup_printf("&#949;<sub>cu2</sub> : %s ‰", tmp1), NULL, gettext("Erreur d'allocation mémoire.\n"));
        else
        {
            tmp2 = complement;
            BUGMSG(complement = g_strdup_printf("%s, &#949;<sub>cu2</sub> : %s ‰", tmp2, tmp1), NULL, gettext("Erreur d'allocation mémoire.\n"));
            free(tmp2);
        }
    }
    if (!ERREUR_RELATIVE_EGALE(materiau->ec3, _1992_1_1_materiaux_ec3(materiau->fck/1000000.)))
    {
        common_math_double_to_char(materiau->ec3*1000., tmp1, DECIMAL_SANS_UNITE);
        if (complement == NULL)
            BUGMSG(complement = g_strdup_printf("&#949;<sub>c3</sub> : %s ‰", tmp1), NULL, gettext("Erreur d'allocation mémoire.\n"));
        else
        {
            tmp2 = complement;
            BUGMSG(complement = g_strdup_printf("%s, &#949;<sub>c3</sub> : %s ‰", tmp2, tmp1), NULL, gettext("Erreur d'allocation mémoire.\n"));
            free(tmp2);
        }
    }
    if (!ERREUR_RELATIVE_EGALE(materiau->ecu3, _1992_1_1_materiaux_ecu3(materiau->fck/1000000.)))
    {
        common_math_double_to_char(materiau->ecu3*1000., tmp1, DECIMAL_SANS_UNITE);
        if (complement == NULL)
            BUGMSG(complement = g_strdup_printf("&#949;<sub>cu3</sub> : %s ‰", tmp1), NULL, gettext("Erreur d'allocation mémoire.\n"));
        else
        {
            tmp2 = complement;
            BUGMSG(complement = g_strdup_printf("%s, &#949;<sub>cu3</sub> : %s ‰", tmp2, tmp1), NULL, gettext("Erreur d'allocation mémoire.\n"));
            free(tmp2);
        }
    }
    if (!ERREUR_RELATIVE_EGALE(materiau->n, _1992_1_1_materiaux_n(materiau->fck/1000000.)))
    {
        common_math_double_to_char(materiau->n*1000., tmp1, DECIMAL_SANS_UNITE);
        if (complement == NULL)
            BUGMSG(complement = g_strdup_printf("n : %s", tmp1), NULL, gettext("Erreur d'allocation mémoire.\n"));
        else
        {
            tmp2 = complement;
            BUGMSG(complement = g_strdup_printf("%s, n : %s", tmp2, tmp1), NULL, gettext("Erreur d'allocation mémoire.\n"));
            free(tmp2);
        }
    }
    
    if (complement == NULL)
        BUGMSG(description = g_strdup_printf("f<sub>ck</sub> : %s MPa", fck), NULL, gettext("Erreur d'allocation mémoire.\n"));
    else
    {
        BUGMSG(description = g_strdup_printf("f<sub>ck</sub> : %s MPa avec %s", fck, complement), NULL, gettext("Erreur d'allocation mémoire.\n"));
        free(complement);
    }
    
    return description;
}


void _1992_1_1_materiaux_free_un(Beton_Materiau *materiau)
/* Description : Fonction permettant de libérer un matériau.
 * Paramètres : Beton_Materiau *materiau : matériau à libérer.
 * Valeur renvoyée : Aucun.
 */
{
    free(materiau->nom);
    free(materiau);
    
    return;
}


G_MODULE_EXPORT gboolean _1992_1_1_materiaux_supprime(Beton_Materiau* materiau,
  gboolean annule_si_utilise, Projet *projet)
/* Description : Supprime le matériau spécifié.
 * Paramètres : Beton_Materiau* materiau : le matériau à supprimer,
 *            : gboolean annule_si_utilise : possibilité d'annuler la suppression si le matériau
 *              est attribué à une barre. Si l'option est désactivée, les barres (et les
 *              barres et noeuds intermédiaires dépendants) utilisant le matériau seront
 *              supprimés.
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL,
 *             materiau == NULL.
 */
{
    GList   *list_barres;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(materiau, FALSE, gettext("Paramètre %s incorrect.\n"), "materiau");
   
    // On vérifie les dépendances.
    BUG(_1992_1_1_materiaux_cherche_dependances(projet, materiau, &list_barres), FALSE);
    
    if ((annule_si_utilise) && (list_barres != NULL))
    {
        char *liste;
        
        liste = common_selection_converti_barres_en_texte(list_barres);
        if (g_list_next(list_barres) == NULL)
            printf("Impossible de supprimer le matériau car il est utilisé par la barre %s.\n", liste);
        else
            printf("Impossible de supprimer le matériau car il est utilisé par les barres %s.\n", liste);
        g_list_free(list_barres);
        free(liste);
        
        return TRUE;
    }
    
    BUG(_1992_1_1_barres_supprime_liste(projet, NULL, list_barres), TRUE);
    g_list_free(list_barres);
    
    _1992_1_1_materiaux_free_un(materiau);
    projet->beton.materiaux = g_list_remove(projet->beton.materiaux, materiau);
    
#ifdef ENABLE_GTK
    gtk_list_store_remove(projet->list_gtk.ef_materiaux.liste_materiaux, &materiau->Iter_liste);
    if (projet->list_gtk.ef_materiaux.builder != NULL)
        gtk_tree_store_remove(projet->list_gtk.ef_materiaux.materiaux, &materiau->Iter_fenetre);
#endif
    
    return TRUE;
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
        g_list_free_full(projet->beton.materiaux, (GDestroyNotify)&_1992_1_1_materiaux_free_un);
        projet->beton.materiaux = NULL;
    }
    
    BUG(EF_calculs_free(projet), TRUE);
    
#ifdef ENABLE_GTK
    g_object_unref(projet->list_gtk.ef_materiaux.liste_materiaux);
#endif
    
    return TRUE;
}
