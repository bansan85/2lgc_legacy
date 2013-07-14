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


gboolean EF_materiaux_init(Projet *projet)
/* Description : Initialise la liste des matériaux.
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


gboolean EF_materiaux_insert(Projet *projet, EF_Materiau *materiau)
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


gboolean EF_materiaux_repositionne(Projet *projet, EF_Materiau *materiau)
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
    switch (materiau->type)
    {
        case MATERIAU_BETON :
        {
            if ((projet->list_gtk._1992_1_1_materiaux.builder != NULL) && (projet->list_gtk._1992_1_1_materiaux.materiau == materiau))
                gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(gtk_builder_get_object(projet->list_gtk._1992_1_1_materiaux.builder, "_1992_1_1_materiaux_beton_textview_nom"))), materiau->nom, -1);
            break;
        }
        default :
        {
            BUGMSG(NULL, FALSE, gettext("Le type de matériau %d n'existe pas.\n"), materiau->type);
            break;
        }
    }
    if (projet->list_gtk.ef_barres.builder != NULL)
        gtk_widget_queue_resize(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_barres.builder, "EF_barres_treeview")));
    gtk_list_store_set(projet->list_gtk.ef_materiaux.liste_materiaux, &materiau->Iter_liste, 0, materiau->nom, -1);
#endif
    
    return TRUE;
}


EF_Materiau* EF_materiaux_cherche_nom(Projet *projet, const char *nom, gboolean critique)
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


char *EF_materiaux_get_description(EF_Materiau* materiau)
/* Description : Renvoie la description d'un matériau sous forme d'un texte.
 *               Il convient de libérer le texte renvoyée par la fonction free.
 * Paramètres : EF_Materiau* materiau : matériau à décrire.
 * Valeur renvoyée :
 *   Succès : Résultat
 *   Échec : NULL :
 *             (materiau == NULL),
 *             erreur d'allocation mémoire.
 */
{
    switch (materiau->type)
    {
        case MATERIAU_BETON :
        {
            return _1992_1_1_materiaux_get_description(materiau);
            break;
        }
        default :
        {
            BUGMSG(NULL, FALSE, gettext("Le type de matériau %d n'existe pas.\n"), materiau->type);
            break;
        }
    }
}


void EF_materiaux_free_un(EF_Materiau *materiau)
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


gboolean EF_materiaux_supprime(EF_Materiau* materiau, Projet *projet)
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
    
    EF_materiaux_free_un(materiau);
    projet->modele.materiaux = g_list_remove(projet->modele.materiaux, materiau);
    
    return TRUE;
}


gboolean EF_materiaux_free(Projet *projet)
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
        g_list_free_full(projet->modele.materiaux, (GDestroyNotify)&EF_materiaux_free_un);
        projet->modele.materiaux = NULL;
    }
    
    BUG(EF_calculs_free(projet), TRUE);
    
#ifdef ENABLE_GTK
    g_object_unref(projet->list_gtk.ef_materiaux.liste_materiaux);
#endif
    
    return TRUE;
}
