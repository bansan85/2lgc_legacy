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
#include "common_selection.h"
#include "1992_1_1_barres.h"


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
    gtk_list_store_append(projet->list_gtk.ef_materiaux.liste_materiaux, &materiau_nouveau->Iter_liste);
    gtk_list_store_set(projet->list_gtk.ef_materiaux.liste_materiaux, &materiau_nouveau->Iter_liste, 0, nom, -1);
    if (projet->list_gtk.ef_materiaux.builder != NULL)
    {
        gtk_tree_store_append(projet->list_gtk.ef_materiaux.materiaux, &materiau_nouveau->Iter_fenetre, NULL);
        gtk_tree_store_set(projet->list_gtk.ef_materiaux.materiaux, &materiau_nouveau->Iter_fenetre, 0, materiau_nouveau->nom, -1);
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
    char    fck[30];
    
    BUGMSG(materiau, NULL, gettext("Paramètre %s incorrect.\n"), "sect");
    
    common_math_double_to_char(materiau->fck/1000000., fck, DECIMAL_CONTRAINTE);
    BUGMSG(description = g_strdup_printf("fck : %s MPa", fck), NULL, gettext("Erreur d'allocation mémoire.\n"));
    
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
