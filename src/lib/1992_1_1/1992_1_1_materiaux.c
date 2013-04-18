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


gboolean _1992_1_1_materiaux_init(Projet *projet)
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
    projet->modele.materiaux = NULL;
    
#ifdef ENABLE_GTK
    projet->list_gtk.ef_materiaux.liste_materiaux = gtk_list_store_new(1, G_TYPE_STRING);
#endif
    
    return TRUE;
}


double _1992_1_1_materiaux_fckcube(double fck)
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


double _1992_1_1_materiaux_fcm(double fck)
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


double _1992_1_1_materiaux_fctm(double fck, double fcm)
/* Description : Renvoie la valeur moyenne de la résistance en traction directe du béton,
 *               mesurée sur cylindre en unité SI (Pa).
 * Paramètres : double fck : Résistance caractéristique en compression du béton, mesurée sur
 *                           cylindre à 28 jours, en MPa.
 *            : double fcm : Valeur moyenne de la résistance en compression du béton, en MPa.
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
        return 2.12*log(1.+(fcm/10.))*1000000.;
}


double _1992_1_1_materiaux_fctk_0_05(double fctm)
/* Description : Renvoie la valeur moyenne de la résistance en traction directe du béton,
 *               (fractile 5%) en unité SI (Pa).
 * Paramètres : double fctm : Valeur moyenne de la résistance en traction directe du béton,
 *                            en MPa.
 * Valeur renvoyée :
 *   Succès : le résultat.
 */
{
    return 0.7*fctm*1000000.;
}


double _1992_1_1_materiaux_fctk_0_95(double fctm)
/* Description : Renvoie la valeur moyenne de la résistance en traction directe du béton,
 *               (fractile 95%) en unité SI (Pa).
 * Paramètres : double fctm : Valeur moyenne de la résistance en traction directe du béton,
 *                            en MPa.
 * Valeur renvoyée :
 *   Succès : le résultat.
 */
{
    return 1.3*fctm*1000000.;
}


double _1992_1_1_materiaux_ecm(double fcm)
/* Description : Renvoie le module d'élasticité sécant du béton en unité SI (Pa).
 * Paramètres : double fcm : valeur moyenne de la résistance en compression du béton, en MPa.
 * Valeur renvoyée :
 *   Succès : le résultat.
 */
{
    return 22.*pow(fcm/10., 0.3)*1000000000.;
}


double _1992_1_1_materiaux_ec1(double fcm)
/* Description : Renvoie la déformation relative en compression du béton au point 1.
 * Paramètres : double fcm : valeur moyenne de la résistance en compression du béton, en MPa.
 * Valeur renvoyée :
 *   Succès : le résultat.
 */
{
    return MIN(0.7*pow(fcm, 0.31), 2.8)/1000.;
}


double _1992_1_1_materiaux_ecu1(double fcm, double fck)
/* Description : Renvoie la déformation relative ultime en compression du béton au point 1.
 * Paramètres : double fcm : valeur moyenne de la résistance en compression du béton, en MPa.
 *            : double fck : Résistance caractéristique en compression du béton, mesurée sur
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
        return (2.8 + 27.*pow((98.-fcm)/100.,4.))/1000.;
}


double _1992_1_1_materiaux_ec2(double fck)
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


double _1992_1_1_materiaux_ecu2(double fck)
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


double _1992_1_1_materiaux_ec3(double fck)
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


double _1992_1_1_materiaux_ecu3(double fck)
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


double _1992_1_1_materiaux_n(double fck)
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
        return 2.;
    else
        return (1.4 + 23.4*pow((90.-fck)/100., 4.));
}


double _1992_1_1_materiaux_gnu(double ecm, double nu)
/* Description : Renvoie le module de cisallement en unité SI (Pa).
 * Paramètres : double fck : module d'élasticité sécant du béton en GPa,
 *              double nu : coefficient de poisson.
 * Valeur renvoyée :
 *   Succès : le résultat.
 */
{
    return ecm/(2.*(1.+nu))*1000000000.;
}


gboolean _1992_1_1_materiaux_insert(Projet *projet, EF_Materiau *materiau)
/* Description : Insère un materiau dans projet->modele.materiaux. Procédure commune à tous les
 *               matériaux.
 * Paramètres : Projet *projet : la variable projet,
 *            : EF_Materiau *materiau : le matériau à insérer.
 * Valeur renvoyée : Aucune.
 */
{
    GList       *list_parcours;
    EF_Materiau *materiau_tmp;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(materiau, FALSE, gettext("Paramètre %s incorrect.\n"), "materiau");
    
    list_parcours = projet->modele.materiaux;
    while (list_parcours != NULL)
    {
        materiau_tmp = list_parcours->data;
        
        if (strcmp(materiau->nom, materiau_tmp->nom) < 0)
            break;
        
        list_parcours = g_list_next(list_parcours);
    }
    if (list_parcours == NULL)
    {
        projet->modele.materiaux = g_list_append(projet->modele.materiaux, materiau);
#ifdef ENABLE_GTK
        gtk_list_store_append(projet->list_gtk.ef_materiaux.liste_materiaux, &materiau->Iter_liste);
        if (projet->list_gtk.ef_materiaux.builder != NULL)
            gtk_tree_store_append(projet->list_gtk.ef_materiaux.materiaux, &materiau->Iter_fenetre, NULL);
#endif
    }
    else
    {
        projet->modele.materiaux = g_list_insert_before(projet->modele.materiaux, list_parcours, materiau);
#ifdef ENABLE_GTK
        gtk_list_store_insert_before(projet->list_gtk.ef_materiaux.liste_materiaux, &materiau->Iter_liste, &materiau_tmp->Iter_liste);
        if (projet->list_gtk.ef_materiaux.builder != NULL)
            gtk_tree_store_insert_before(projet->list_gtk.ef_materiaux.materiaux, &materiau->Iter_fenetre, NULL, &materiau_tmp->Iter_fenetre);
#endif
    }
    
#ifdef ENABLE_GTK
    gtk_list_store_set(projet->list_gtk.ef_materiaux.liste_materiaux, &materiau->Iter_liste, 0, materiau->nom, -1);
    if (projet->list_gtk.ef_materiaux.builder != NULL)
        gtk_tree_store_set(projet->list_gtk.ef_materiaux.materiaux, &materiau->Iter_fenetre, 0, materiau, -1);
#endif
    
    return TRUE;
}


EF_Materiau* _1992_1_1_materiaux_ajout(Projet *projet, const char *nom, double fck)
/* Description : Ajoute un matériau en béton et calcule ses caractéristiques mécaniques.
 *               Les propriétés du béton sont déterminées conformément au tableau 3.1 de
 *               l'Eurocode 2-1-1 les valeurs de fckcube est déterminée par interpolation
 *               linéaire si nécessaire.
 * Paramètres : Projet *projet : la variable projet,
 *            : double fck : résistance à la compression du béton à 28 jours en MPa,
 *            : double nu : coefficient de poisson pour un béton non fissuré.
 * Valeur renvoyée :
 *   Succès : pointeur vers le nouveau matériau.
 *   Échec : NULL :
 *             projet == NULL,
 *             projet->modele.materiaux == NULL,
 *             fck > 90.,
 *             en cas d'erreur d'allocation mémoire.
 */
{
    EF_Materiau     *materiau_nouveau;
    Materiau_Beton  *data_beton;
    
    // Trivial
    BUGMSG(projet, NULL, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG((fck > ERREUR_RELATIVE_MIN) && (fck <= 90.*(1+ERREUR_RELATIVE_MIN)), NULL, gettext("La résistance caractéristique à la compression du béton doit être inférieure ou égale à 90 MPa.\n"));
    BUGMSG(materiau_nouveau = malloc(sizeof(EF_Materiau)), NULL, gettext("Erreur d'allocation mémoire.\n"));
    BUGMSG(data_beton = malloc(sizeof(Materiau_Beton)), NULL, gettext("Erreur d'allocation mémoire.\n"));
    
    materiau_nouveau->type = MATERIAU_BETON;
    materiau_nouveau->data = data_beton;
    
    BUGMSG(materiau_nouveau->nom = g_strdup_printf("%s", nom), NULL, gettext("Erreur d'allocation mémoire.\n"));
    
    data_beton->fck = common_math_f(fck*1000000., FLOTTANT_UTILISATEUR);
    data_beton->fckcube = common_math_f(_1992_1_1_materiaux_fckcube(fck), FLOTTANT_ORDINATEUR);
    BUG(!isnan(common_math_get(data_beton->fckcube)), NULL);
    data_beton->fcm = common_math_f(_1992_1_1_materiaux_fcm(fck), FLOTTANT_ORDINATEUR);
    BUG(!isnan(common_math_get(data_beton->fcm)), NULL);
    data_beton->fctm = common_math_f(_1992_1_1_materiaux_fctm(fck, common_math_get(data_beton->fcm)/1000000.), FLOTTANT_ORDINATEUR);
    BUG(!isnan(common_math_get(data_beton->fctm)), NULL);
    data_beton->fctk_0_05 = common_math_f(_1992_1_1_materiaux_fctk_0_05(common_math_get(data_beton->fctm)/1000000.), FLOTTANT_ORDINATEUR);
    BUG(!isnan(common_math_get(data_beton->fctk_0_05)), NULL);
    data_beton->fctk_0_95 = common_math_f(_1992_1_1_materiaux_fctk_0_95(common_math_get(data_beton->fctm)/1000000.), FLOTTANT_ORDINATEUR);
    BUG(!isnan(common_math_get(data_beton->fctk_0_95)), NULL);
    data_beton->ecm = common_math_f(_1992_1_1_materiaux_ecm(common_math_get(data_beton->fcm)/1000000.), FLOTTANT_ORDINATEUR);
    BUG(!isnan(common_math_get(data_beton->ecm)), NULL);
    data_beton->ec1 = common_math_f(_1992_1_1_materiaux_ec1(common_math_get(data_beton->fcm)/1000000.), FLOTTANT_ORDINATEUR);
    BUG(!isnan(common_math_get(data_beton->ec1)), NULL);
    data_beton->ecu1 = common_math_f(_1992_1_1_materiaux_ecu1(common_math_get(data_beton->fcm)/1000000., fck), FLOTTANT_ORDINATEUR);
    BUG(!isnan(common_math_get(data_beton->ecu1)), NULL);
    data_beton->ec2 = common_math_f(_1992_1_1_materiaux_ec2(fck), FLOTTANT_ORDINATEUR);
    BUG(!isnan(common_math_get(data_beton->ec2)), NULL);
    data_beton->ecu2 = common_math_f(_1992_1_1_materiaux_ecu2(fck), FLOTTANT_ORDINATEUR);
    BUG(!isnan(common_math_get(data_beton->ecu2)), NULL);
    data_beton->ec3 = common_math_f(_1992_1_1_materiaux_ec3(fck), FLOTTANT_ORDINATEUR);
    BUG(!isnan(common_math_get(data_beton->ec3)), NULL);
    data_beton->ecu3 = common_math_f(_1992_1_1_materiaux_ecu3(fck), FLOTTANT_ORDINATEUR);
    BUG(!isnan(common_math_get(data_beton->ecu3)), NULL);
    data_beton->n = common_math_f(_1992_1_1_materiaux_n(fck), FLOTTANT_ORDINATEUR);
    BUG(!isnan(common_math_get(data_beton->n)), NULL);
    data_beton->nu = common_math_f(COEFFICIENT_NU_BETON, FLOTTANT_ORDINATEUR);
    
    BUG(_1992_1_1_materiaux_insert(projet, materiau_nouveau), NULL);
    
    return materiau_nouveau;
}


gboolean _1992_1_1_materiaux_repositionne(Projet *projet, EF_Materiau *materiau)
/* Description : Repositionne un matériau après un renommage. Procédure commune à toutes les
 *               matériaux.
 * Paramètres : Projet *projet : la variable projet,
 *            : EF_Materiau *materiau : le matériau à repositionner.
 * Valeur renvoyée : Aucune.
 */
{
    GList   *list_parcours;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(materiau, FALSE, gettext("Paramètre %s incorrect.\n"), "materiau");
    
    // On réinsère le matériau au bon endroit
    projet->modele.materiaux = g_list_remove(projet->modele.materiaux, materiau);
    list_parcours = projet->modele.materiaux;
    while (list_parcours != NULL)
    {
        EF_Materiau  *materiau_parcours = list_parcours->data;
        
        if (strcmp(materiau->nom, materiau_parcours->nom) < 0)
        {
            projet->modele.materiaux = g_list_insert_before(projet->modele.materiaux, list_parcours, materiau);
            
#ifdef ENABLE_GTK
            gtk_list_store_move_before(projet->list_gtk.ef_materiaux.liste_materiaux, &materiau->Iter_liste, &materiau_parcours->Iter_liste);
            if (projet->list_gtk.ef_materiaux.builder != NULL)
                gtk_tree_store_move_before(projet->list_gtk.ef_materiaux.materiaux, &materiau->Iter_fenetre, &materiau_parcours->Iter_fenetre);
#endif
            break;
        }
        
        list_parcours = g_list_next(list_parcours);
    }
    if (list_parcours == NULL)
    {
        projet->modele.materiaux = g_list_append(projet->modele.materiaux, materiau);
        
#ifdef ENABLE_GTK
        gtk_list_store_move_before(projet->list_gtk.ef_materiaux.liste_materiaux, &materiau->Iter_liste, NULL);
        if (projet->list_gtk.ef_materiaux.builder != NULL)
            gtk_tree_store_move_before(projet->list_gtk.ef_materiaux.materiaux, &materiau->Iter_fenetre, NULL);
#endif
    }
    
#ifdef ENABLE_GTK
    if ((projet->list_gtk._1992_1_1_materiaux.builder != NULL) && (projet->list_gtk._1992_1_1_materiaux.materiau == materiau))
        gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(gtk_builder_get_object(projet->list_gtk._1992_1_1_materiaux.builder, "_1992_1_1_materiaux_beton_textview_nom"))), materiau->nom, -1);
    if (projet->list_gtk.ef_barres.builder != NULL)
        gtk_widget_queue_draw(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_barres.builder, "EF_barres_treeview")));
    gtk_list_store_set(projet->list_gtk.ef_materiaux.liste_materiaux, &materiau->Iter_liste, 0, materiau->nom, -1);
#endif
    
    return TRUE;
}


gboolean _1992_1_1_materiaux_modif(Projet *projet, EF_Materiau *materiau, char *nom,
  Flottant fck, Flottant fckcube, Flottant fcm, Flottant fctm, Flottant fctk_0_05,
  Flottant fctk_0_95, Flottant ecm, Flottant ec1, Flottant ecu1, Flottant ec2, Flottant ecu2,
  Flottant n, Flottant ec3, Flottant ecu3, Flottant nu)
/* Description : Modifie un matériau béton.
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
    Materiau_Beton  *data_beton;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(materiau, FALSE, gettext("Paramètre %s incorrect.\n"), "materiau");
    BUGMSG(materiau->type == MATERIAU_BETON, FALSE, gettext("Le matériau n'est pas en béton.\n"));
    
    data_beton = materiau->data;
    
    if ((nom != NULL) && (strcmp(materiau->nom, nom) != 0))
    {
        BUGMSG(!_1992_1_1_materiaux_cherche_nom(projet, nom, FALSE), FALSE, gettext("Le matériau %s existe déjà.\n"), nom);
        free(materiau->nom);
        BUGMSG(materiau->nom = g_strdup_printf("%s", nom), FALSE, gettext("Erreur d'allocation mémoire.\n"));
        BUG(_1992_1_1_materiaux_repositionne(projet, materiau), FALSE);
    }
    
    if (!isnan(common_math_get(fck)))
        data_beton->fck = fck;
    if (!isnan(common_math_get(fckcube)))
        data_beton->fckcube = fckcube;
    if (!isnan(common_math_get(fcm)))
        data_beton->fcm = fcm;
    if (!isnan(common_math_get(fctm)))
        data_beton->fctm = fctm;
    if (!isnan(common_math_get(fctk_0_05)))
        data_beton->fctk_0_05 = fctk_0_05;
    if (!isnan(common_math_get(fctk_0_95)))
        data_beton->fctk_0_95 = fctk_0_95;
    if (!isnan(common_math_get(ecm)))
        data_beton->ecm = ecm;
    if (!isnan(common_math_get(ec1)))
        data_beton->ec1 = ec1;
    if (!isnan(common_math_get(ecu1)))
        data_beton->ecu1 = ecu1;
    if (!isnan(common_math_get(ec2)))
        data_beton->ec2 = ec2;
    if (!isnan(common_math_get(ecu2)))
        data_beton->ecu2 = ecu2;
    if (!isnan(common_math_get(ec3)))
        data_beton->ec3 = ec3;
    if (!isnan(common_math_get(ecu3)))
        data_beton->ecu3 = ecu3;
    if (!isnan(common_math_get(n)))
        data_beton->n = n;
    if (!isnan(common_math_get(nu)))
        data_beton->nu = nu;
    
    if ((!isnan(common_math_get(fck))) || (!isnan(common_math_get(fckcube))) || (!isnan(common_math_get(fcm))) || (!isnan(common_math_get(fctm))) || (!isnan(common_math_get(fctk_0_05))) || (!isnan(common_math_get(fctk_0_95))) || (!isnan(common_math_get(ecm))) || (!isnan(common_math_get(ec1))) || (!isnan(common_math_get(ecu1))) || (!isnan(common_math_get(ec2))) || (!isnan(common_math_get(ecu2))) || (!isnan(common_math_get(ec3))) || (!isnan(common_math_get(ecu3))) || (!isnan(common_math_get(n))) || (!isnan(common_math_get(nu))))
    {
        GList   *liste_materiaux = NULL;
        GList   *liste_barres_dep;
        
        liste_materiaux = g_list_append(liste_materiaux, materiau);
        BUG(_1992_1_1_barres_cherche_dependances(projet, NULL, NULL, NULL, liste_materiaux, NULL, NULL, NULL, &liste_barres_dep, NULL, FALSE, FALSE), FALSE);
        g_list_free(liste_materiaux);
        
        if (liste_barres_dep != NULL)
            BUG(EF_calculs_free(projet), FALSE);
        
        g_list_free(liste_barres_dep);
    }
    
#ifdef ENABLE_GTK
    if (projet->list_gtk.ef_materiaux.builder != NULL)
        gtk_widget_queue_draw(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_materiaux.builder, "EF_materiaux_treeview")));
#endif
    
    return TRUE;
}


EF_Materiau* _1992_1_1_materiaux_cherche_nom(Projet *projet, const char *nom, gboolean critique)
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
    list_parcours = projet->modele.materiaux;
    while (list_parcours != NULL)
    {
        EF_Materiau  *materiau = list_parcours->data;
        
        if (strcmp(materiau->nom, nom) == 0)
            return materiau;
        
        list_parcours = g_list_next(list_parcours);
    }
    
    if (critique)
        BUGMSG(0, NULL, gettext("Matériau en béton '%s' introuvable.\n"), nom);
    else
        return NULL;
}


char *_1992_1_1_materiaux_get_description(EF_Materiau* materiau)
/* Description : Renvoie la description d'un matériau béton sous forme d'un texte.
 *               Il convient de libérer le texte renvoyée par la fonction free.
 * Paramètres : EF_Materiau* materiau : section à étudier.
 * Valeur renvoyée :
 *   Succès : Résultat
 *   Échec : NULL :
 *             (materiau == NULL),
 *             erreur d'allocation mémoire.
 */
{
    char            *description;
    char            fck[30], tmp1[30];
    char            *complement = NULL, *tmp2;
    Materiau_Beton  *data_beton;
    
    BUGMSG(materiau, NULL, gettext("Paramètre %s incorrect.\n"), "sect");
    BUGMSG(materiau->type == MATERIAU_BETON, FALSE, gettext("Le matériau n'est pas en béton.\n"));
    
    data_beton = materiau->data;
    
    common_math_double_to_char2(common_math_f(common_math_get(data_beton->fck)/1000000., data_beton->fck.type), fck, DECIMAL_CONTRAINTE);
    
    // On affiche les différences si le matériau a été personnalisé
    if (!ERREUR_RELATIVE_EGALE(common_math_get(data_beton->fckcube), _1992_1_1_materiaux_fckcube(common_math_get(data_beton->fck)/1000000.)))
    {
        common_math_double_to_char2(common_math_f(common_math_get(data_beton->fckcube)/1000000., data_beton->fckcube.type), tmp1, DECIMAL_CONTRAINTE);
        if (complement == NULL)
            BUGMSG(complement = g_strdup_printf("f<sub>ck,cube</sub> : %s MPa", tmp1), NULL, gettext("Erreur d'allocation mémoire.\n"));
        else
        {
            tmp2 = complement;
            BUGMSG(complement = g_strdup_printf("%s, f<sub>ck,cube</sub> : %s MPa", tmp2, tmp1), NULL, gettext("Erreur d'allocation mémoire.\n"));
            free(tmp2);
        }
    }
    if (!ERREUR_RELATIVE_EGALE(common_math_get(data_beton->fcm), _1992_1_1_materiaux_fcm(common_math_get(data_beton->fck)/1000000.)))
    {
        common_math_double_to_char2(common_math_f(common_math_get(data_beton->fcm)/1000000., data_beton->fcm.type), tmp1, DECIMAL_CONTRAINTE);
        if (complement == NULL)
            BUGMSG(complement = g_strdup_printf("f<sub>cm</sub> : %s MPa", tmp1), NULL, gettext("Erreur d'allocation mémoire.\n"));
        else
        {
            tmp2 = complement;
            BUGMSG(complement = g_strdup_printf("%s, f<sub>cm</sub> : %s MPa", tmp2, tmp1), NULL, gettext("Erreur d'allocation mémoire.\n"));
            free(tmp2);
        }
    }
    if (!ERREUR_RELATIVE_EGALE(common_math_get(data_beton->fctm), _1992_1_1_materiaux_fctm(common_math_get(data_beton->fck)/1000000., common_math_get(data_beton->fcm)/1000000.)))
    {
        common_math_double_to_char2(common_math_f(common_math_get(data_beton->fctm)/1000000., data_beton->fctm.type), tmp1, DECIMAL_CONTRAINTE);
        if (complement == NULL)
            BUGMSG(complement = g_strdup_printf("f<sub>ctm</sub> : %s MPa", tmp1), NULL, gettext("Erreur d'allocation mémoire.\n"));
        else
        {
            tmp2 = complement;
            BUGMSG(complement = g_strdup_printf("%s, f<sub>ctm</sub> : %s MPa", tmp2, tmp1), NULL, gettext("Erreur d'allocation mémoire.\n"));
            free(tmp2);
        }
    }
    if (!ERREUR_RELATIVE_EGALE(common_math_get(data_beton->fctk_0_05), _1992_1_1_materiaux_fctk_0_05(common_math_get(data_beton->fctm)/1000000.)))
    {
        common_math_double_to_char2(common_math_f(common_math_get(data_beton->fctk_0_05)/1000000., data_beton->fctk_0_05.type), tmp1, DECIMAL_CONTRAINTE);
        if (complement == NULL)
            BUGMSG(complement = g_strdup_printf("f<sub>ctk,0.05</sub> : %s MPa", tmp1), NULL, gettext("Erreur d'allocation mémoire.\n"));
        else
        {
            tmp2 = complement;
            BUGMSG(complement = g_strdup_printf("%s, f<sub>ctk,0.05</sub> : %s MPa", tmp2, tmp1), NULL, gettext("Erreur d'allocation mémoire.\n"));
            free(tmp2);
        }
    }
    if (!ERREUR_RELATIVE_EGALE(common_math_get(data_beton->fctk_0_95), _1992_1_1_materiaux_fctk_0_95(common_math_get(data_beton->fctm)/1000000.)))
    {
        common_math_double_to_char2(common_math_f(common_math_get(data_beton->fctk_0_95)/1000000., data_beton->fctk_0_95.type), tmp1, DECIMAL_CONTRAINTE);
        if (complement == NULL)
            BUGMSG(complement = g_strdup_printf("f<sub>ctk,0.95</sub> : %s MPa", tmp1), NULL, gettext("Erreur d'allocation mémoire.\n"));
        else
        {
            tmp2 = complement;
            BUGMSG(complement = g_strdup_printf("%s, f<sub>ctk,0.95</sub> : %s MPa", tmp2, tmp1), NULL, gettext("Erreur d'allocation mémoire.\n"));
            free(tmp2);
        }
    }
    if (!ERREUR_RELATIVE_EGALE(common_math_get(data_beton->ecm), _1992_1_1_materiaux_ecm(common_math_get(data_beton->fcm)/1000000.)))
    {
        common_math_double_to_char2(common_math_f(common_math_get(data_beton->ecm)/1000000., data_beton->ecm.type), tmp1, DECIMAL_CONTRAINTE);
        if (complement == NULL)
            BUGMSG(complement = g_strdup_printf("E<sub>cm</sub> : %s MPa", tmp1), NULL, gettext("Erreur d'allocation mémoire.\n"));
        else
        {
            tmp2 = complement;
            BUGMSG(complement = g_strdup_printf("%s, E<sub>cm</sub> : %s MPa", tmp2, tmp1), NULL, gettext("Erreur d'allocation mémoire.\n"));
            free(tmp2);
        }
    }
    if (!ERREUR_RELATIVE_EGALE(common_math_get(data_beton->ec1), _1992_1_1_materiaux_ec1(common_math_get(data_beton->fcm)/1000000.)))
    {
        common_math_double_to_char2(common_math_f(common_math_get(data_beton->ec1)*1000., data_beton->ec1.type), tmp1, DECIMAL_SANS_UNITE);
        if (complement == NULL)
            BUGMSG(complement = g_strdup_printf("&#949;<sub>c1</sub> : %s ‰", tmp1), NULL, gettext("Erreur d'allocation mémoire.\n"));
        else
        {
            tmp2 = complement;
            BUGMSG(complement = g_strdup_printf("%s, &#949;<sub>c1</sub> : %s ‰", tmp2, tmp1), NULL, gettext("Erreur d'allocation mémoire.\n"));
            free(tmp2);
        }
    }
    if (!ERREUR_RELATIVE_EGALE(common_math_get(data_beton->ecu1), _1992_1_1_materiaux_ecu1(common_math_get(data_beton->fcm)/1000000., common_math_get(data_beton->fck)/1000000.)))
    {
        common_math_double_to_char2(common_math_f(common_math_get(data_beton->ecu1)*1000., data_beton->ecu1.type), tmp1, DECIMAL_SANS_UNITE);
        if (complement == NULL)
            BUGMSG(complement = g_strdup_printf("&#949;<sub>cu1</sub> : %s ‰", tmp1), NULL, gettext("Erreur d'allocation mémoire.\n"));
        else
        {
            tmp2 = complement;
            BUGMSG(complement = g_strdup_printf("%s, &#949;<sub>cu1</sub> : %s ‰", tmp2, tmp1), NULL, gettext("Erreur d'allocation mémoire.\n"));
            free(tmp2);
        }
    }
    if (!ERREUR_RELATIVE_EGALE(common_math_get(data_beton->ec2), _1992_1_1_materiaux_ec2(common_math_get(data_beton->fck)/1000000.)))
    {
        common_math_double_to_char2(common_math_f(common_math_get(data_beton->ec2)*1000., data_beton->ec2.type), tmp1, DECIMAL_SANS_UNITE);
        if (complement == NULL)
            BUGMSG(complement = g_strdup_printf("&#949;<sub>c2</sub> : %s ‰", tmp1), NULL, gettext("Erreur d'allocation mémoire.\n"));
        else
        {
            tmp2 = complement;
            BUGMSG(complement = g_strdup_printf("%s, &#949;<sub>c2</sub> : %s ‰", tmp2, tmp1), NULL, gettext("Erreur d'allocation mémoire.\n"));
            free(tmp2);
        }
    }
    if (!ERREUR_RELATIVE_EGALE(common_math_get(data_beton->ecu2), _1992_1_1_materiaux_ecu2(common_math_get(data_beton->fck)/1000000.)))
    {
        common_math_double_to_char2(common_math_f(common_math_get(data_beton->ecu2)*1000., data_beton->ecu2.type), tmp1, DECIMAL_SANS_UNITE);
        if (complement == NULL)
            BUGMSG(complement = g_strdup_printf("&#949;<sub>cu2</sub> : %s ‰", tmp1), NULL, gettext("Erreur d'allocation mémoire.\n"));
        else
        {
            tmp2 = complement;
            BUGMSG(complement = g_strdup_printf("%s, &#949;<sub>cu2</sub> : %s ‰", tmp2, tmp1), NULL, gettext("Erreur d'allocation mémoire.\n"));
            free(tmp2);
        }
    }
    if (!ERREUR_RELATIVE_EGALE(common_math_get(data_beton->ec3), _1992_1_1_materiaux_ec3(common_math_get(data_beton->fck)/1000000.)))
    {
        common_math_double_to_char2(common_math_f(common_math_get(data_beton->ec3)*1000., data_beton->ec3.type), tmp1, DECIMAL_SANS_UNITE);
        if (complement == NULL)
            BUGMSG(complement = g_strdup_printf("&#949;<sub>c3</sub> : %s ‰", tmp1), NULL, gettext("Erreur d'allocation mémoire.\n"));
        else
        {
            tmp2 = complement;
            BUGMSG(complement = g_strdup_printf("%s, &#949;<sub>c3</sub> : %s ‰", tmp2, tmp1), NULL, gettext("Erreur d'allocation mémoire.\n"));
            free(tmp2);
        }
    }
    if (!ERREUR_RELATIVE_EGALE(common_math_get(data_beton->ecu3), _1992_1_1_materiaux_ecu3(common_math_get(data_beton->fck)/1000000.)))
    {
        common_math_double_to_char2(common_math_f(common_math_get(data_beton->ecu3)*1000., data_beton->ecu3.type), tmp1, DECIMAL_SANS_UNITE);
        if (complement == NULL)
            BUGMSG(complement = g_strdup_printf("&#949;<sub>cu3</sub> : %s ‰", tmp1), NULL, gettext("Erreur d'allocation mémoire.\n"));
        else
        {
            tmp2 = complement;
            BUGMSG(complement = g_strdup_printf("%s, &#949;<sub>cu3</sub> : %s ‰", tmp2, tmp1), NULL, gettext("Erreur d'allocation mémoire.\n"));
            free(tmp2);
        }
    }
    if (!ERREUR_RELATIVE_EGALE(common_math_get(data_beton->n), _1992_1_1_materiaux_n(common_math_get(data_beton->fck)/1000000.)))
    {
        common_math_double_to_char2(data_beton->n, tmp1, DECIMAL_SANS_UNITE);
        if (complement == NULL)
            BUGMSG(complement = g_strdup_printf("n : %s", tmp1), NULL, gettext("Erreur d'allocation mémoire.\n"));
        else
        {
            tmp2 = complement;
            BUGMSG(complement = g_strdup_printf("%s, n : %s", tmp2, tmp1), NULL, gettext("Erreur d'allocation mémoire.\n"));
            free(tmp2);
        }
    }
    if (!ERREUR_RELATIVE_EGALE(common_math_get(data_beton->nu), COEFFICIENT_NU_BETON))
    {
        common_math_double_to_char2(data_beton->nu, tmp1, DECIMAL_SANS_UNITE);
        if (complement == NULL)
            BUGMSG(complement = g_strdup_printf("&#957; : %s", tmp1), NULL, gettext("Erreur d'allocation mémoire.\n"));
        else
        {
            tmp2 = complement;
            BUGMSG(complement = g_strdup_printf("%s, &#957; : %s", tmp2, tmp1), NULL, gettext("Erreur d'allocation mémoire.\n"));
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


void _1992_1_1_materiaux_free_un(EF_Materiau *materiau)
/* Description : Fonction permettant de libérer un matériau.
 * Paramètres : EF_Materiau *materiau : matériau à libérer.
 * Valeur renvoyée : Aucun.
 */
{
    free(materiau->nom);
    free(materiau->data);
    free(materiau);
    
    return;
}


gboolean _1992_1_1_materiaux_supprime(EF_Materiau* materiau, Projet *projet)
/* Description : Supprime le matériau spécifié. Impossible si le matériau est utilisé.
 * Paramètres : EF_Materiau* materiau : le matériau à supprimer,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL,
 *             materiau == NULL.
 */
{
    GList   *liste_materiaux = NULL, *liste_barres_dep;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(materiau, FALSE, gettext("Paramètre %s incorrect.\n"), "materiau");
   
    // On vérifie les dépendances.
    liste_materiaux = g_list_append(liste_materiaux, materiau);
    BUG(_1992_1_1_barres_cherche_dependances(projet, NULL, NULL, NULL, liste_materiaux, NULL, NULL, NULL, &liste_barres_dep, NULL, FALSE, FALSE), FALSE);
    g_list_free(liste_materiaux);
    
    if (liste_barres_dep != NULL)
    {
        char *liste;
        
        liste = common_selection_converti_barres_en_texte(liste_barres_dep);
        if (g_list_next(liste_barres_dep) == NULL)
            BUGMSG(NULL, FALSE, gettext("Impossible de supprimer le matériau car il est utilisé par la barre %s.\n"), liste);
        else
            BUGMSG(NULL, FALSE, gettext("Impossible de supprimer le matériau car il est utilisé par les barres %s.\n"), liste);
    }
    
    BUG(_1992_1_1_barres_supprime_liste(projet, NULL, liste_barres_dep), TRUE);
    g_list_free(liste_barres_dep);
    
#ifdef ENABLE_GTK
    gtk_list_store_remove(projet->list_gtk.ef_materiaux.liste_materiaux, &materiau->Iter_liste);
    if (projet->list_gtk.ef_materiaux.builder != NULL)
        gtk_tree_store_remove(projet->list_gtk.ef_materiaux.materiaux, &materiau->Iter_fenetre);
#endif
    
    _1992_1_1_materiaux_free_un(materiau);
    projet->modele.materiaux = g_list_remove(projet->modele.materiaux, materiau);
    
    return TRUE;
}


gboolean _1992_1_1_materiaux_free(Projet *projet)
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
    while (projet->modele.materiaux != NULL)
    {
        g_list_free_full(projet->modele.materiaux, (GDestroyNotify)&_1992_1_1_materiaux_free_un);
        projet->modele.materiaux = NULL;
    }
    
    BUG(EF_calculs_free(projet), TRUE);
    
#ifdef ENABLE_GTK
    g_object_unref(projet->list_gtk.ef_materiaux.liste_materiaux);
#endif
    
    return TRUE;
}
