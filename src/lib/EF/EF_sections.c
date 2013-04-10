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
#include <math.h>
#include <string.h>
#include <gmodule.h>

#include "common_projet.h"
#include "common_erreurs.h"
#include "common_math.h"
#include "common_selection.h"
#include "EF_noeuds.h"
#include "EF_calculs.h"
#include "1992_1_1_barres.h"
#include "EF_sections.h"

#ifdef ENABLE_GTK
#include "EF_gtk_sections.h"
#include "common_m3d.hpp"
#endif

gboolean EF_sections_init(Projet *projet)
/* Description : Initialise la liste des section en béton.
 * Paramètres : Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL.
 */
{
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    
    // Trivial
    projet->modele.sections = NULL;
    
#ifdef ENABLE_GTK
    projet->list_gtk.ef_sections.liste_sections = gtk_list_store_new(1, G_TYPE_STRING);
#endif
    
    return TRUE;
}


gboolean EF_sections_insert(Projet *projet, EF_Section *section)
/* Description : Insère une section dans projet->modele.sections. Procédure commune à toutes les
 *               sections.
 * Paramètres : Projet *projet : la variable projet,
 *            : EF_Section *section : la section à insérer.
 * Valeur renvoyée : Aucune.
 */
{
    GList       *list_parcours;
    EF_Section  *section_tmp;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(section, FALSE, gettext("Paramètre %s incorrect.\n"), "section");
    
    list_parcours = projet->modele.sections;
    while (list_parcours != NULL)
    {
        section_tmp = list_parcours->data;
        
        if (strcmp(section->nom, section_tmp->nom) < 0)
            break;
        
        list_parcours = g_list_next(list_parcours);
    }
    if (list_parcours == NULL)
    {
        projet->modele.sections = g_list_append(projet->modele.sections, section);
#ifdef ENABLE_GTK
        gtk_list_store_append(projet->list_gtk.ef_sections.liste_sections, &section->Iter_liste);
        if (projet->list_gtk.ef_sections.builder != NULL)
            gtk_tree_store_append(projet->list_gtk.ef_sections.sections, &section->Iter_fenetre, NULL);
#endif
    }
    else
    {
        projet->modele.sections = g_list_insert_before(projet->modele.sections, list_parcours, section);
#ifdef ENABLE_GTK
        gtk_list_store_insert_before(projet->list_gtk.ef_sections.liste_sections, &section->Iter_liste, &section_tmp->Iter_liste);
        if (projet->list_gtk.ef_sections.builder != NULL)
            gtk_tree_store_insert_before(projet->list_gtk.ef_sections.sections, &section->Iter_fenetre, NULL, &section_tmp->Iter_fenetre);
#endif
    }
    
#ifdef ENABLE_GTK
    gtk_list_store_set(projet->list_gtk.ef_sections.liste_sections, &section->Iter_liste, 0, section->nom, -1);
    if (projet->list_gtk.ef_sections.builder != NULL)
        gtk_tree_store_set(projet->list_gtk.ef_sections.sections, &section->Iter_fenetre, 0, section, -1);
#endif
    
    return TRUE;
}


gboolean EF_sections_repositionne(Projet *projet, EF_Section *section)
/* Description : Repositionne une section après un renommage. Procédure commune à toutes les
 *               sections.
 * Paramètres : Projet *projet : la variable projet,
 *            : EF_Section *section : la section à repositionner.
 * Valeur renvoyée : Aucune.
 */
{
    GList   *list_parcours;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(section, FALSE, gettext("Paramètre %s incorrect.\n"), "section");
    
    // On réinsère la section au bon endroit
    projet->modele.sections = g_list_remove(projet->modele.sections, section);
    list_parcours = projet->modele.sections;
    while (list_parcours != NULL)
    {
        EF_Section    *section_parcours = list_parcours->data;
        
        if (strcmp(section->nom, section_parcours->nom) < 0)
        {
            projet->modele.sections = g_list_insert_before(projet->modele.sections, list_parcours, section);
            
#ifdef ENABLE_GTK
            gtk_list_store_move_before(projet->list_gtk.ef_sections.liste_sections, &section->Iter_liste, &section_parcours->Iter_liste);
            if (projet->list_gtk.ef_sections.builder != NULL)
                gtk_tree_store_move_before(projet->list_gtk.ef_sections.sections, &section->Iter_fenetre, &section_parcours->Iter_fenetre);
            break;
#endif
        }
        
        list_parcours = g_list_next(list_parcours);
    }
    if (list_parcours == NULL)
    {
        projet->modele.sections = g_list_append(projet->modele.sections, section);
        
#ifdef ENABLE_GTK
        gtk_list_store_move_before(projet->list_gtk.ef_sections.liste_sections, &section->Iter_liste, NULL);
        if (projet->list_gtk.ef_sections.builder != NULL)
            gtk_tree_store_move_before(projet->list_gtk.ef_sections.sections, &section->Iter_fenetre, NULL);
#endif
    }
    
#ifdef ENABLE_GTK
    if (projet->list_gtk.ef_barres.builder != NULL)
    {
        list_parcours = projet->modele.barres;
        
        while (list_parcours != NULL)
        {
            Beton_Barre *barre = list_parcours->data;
            
            if (barre->section == section)
                gtk_tree_store_set(GTK_TREE_STORE(gtk_builder_get_object(projet->list_gtk.ef_barres.builder, "EF_barres_treestore")), &barre->Iter, 2, barre->section->nom, -1);
            
            list_parcours = g_list_next(list_parcours);
        }
    }
#endif
    
    return TRUE;
}


gboolean EF_sections_rectangulaire_ajout(Projet *projet, const char* nom, Flottant l,
  Flottant h)
/* Description : Ajouter une nouvelle section rectangulaire à la liste des sections en béton.
 * Paramètres : Projet *projet : la variable projet,
 *            : Flottant l : la largeur,
 *            : Flottant h : la hauteur.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL,
 *             en cas d'erreur d'allocation mémoire.
 */
{
    Section_T   *section_data;
    EF_Section  *section_nouvelle;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(!EF_sections_cherche_nom(projet, nom, FALSE), FALSE, gettext("La section %s existe déjà.\n"), nom);
    BUGMSG(section_nouvelle = malloc(sizeof(EF_Section)), FALSE, gettext("Erreur d'allocation mémoire.\n"));
    BUGMSG(section_data = malloc(sizeof(Section_T)), FALSE, gettext("Erreur d'allocation mémoire.\n"));
    section_nouvelle->data = section_data;
    
    // Trivial
    section_nouvelle->type = SECTION_RECTANGULAIRE;
    BUGMSG(section_nouvelle->nom = g_strdup_printf("%s", nom), FALSE, gettext("Erreur d'allocation mémoire.\n"));
    section_data->largeur_retombee = l;
    section_data->hauteur_retombee = h;
    section_data->largeur_table = l;
    section_data->hauteur_table = common_math_f(0., FLOTTANT_ORDINATEUR);
    
    BUG(EF_sections_insert(projet, section_nouvelle), FALSE);
    
    return TRUE;
}


gboolean EF_sections_rectangulaire_modif(Projet *projet, EF_Section *section, const char* nom,
  Flottant l, Flottant h)
/* Description : Modifie une section rectangulaire.
 * Paramètres : Projet *projet : la variable projet,
 *            : EF_Section *section : la section à modifier,
 *            : const char* nom : son nouveau nom, NULL si aucun changement,
 *            : Flottant l : sa nouvelle largeur, NAN si aucun changement,
 *            : Flottant h : sa nouvelle hauteur, NAN si aucun changement.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL,
 *             section == NULL,
 *             section->type != RECTANGULAIRE.
 */
{
    Section_T   *section_data = section->data;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(section, FALSE, gettext("Paramètre %s incorrect.\n"), "section");
    BUGMSG(section->type == SECTION_RECTANGULAIRE, FALSE, gettext("La section doit être de type rectangulaire.\n"));
    
    // Trivial
    if ((nom != NULL) && (strcmp(section->nom, nom) != 0))
    {
        BUGMSG(!EF_sections_cherche_nom(projet, nom, FALSE), FALSE, gettext("La section %s existe déjà.\n"), nom);
        free(section->nom);
        BUGMSG(section->nom = g_strdup_printf("%s", nom), FALSE, gettext("Erreur d'allocation mémoire.\n"));
        BUG(EF_sections_repositionne(projet, section), FALSE);
#ifdef ENABLE_GTK
        if ((projet->list_gtk.ef_sections_rectangulaire.builder != NULL) && (projet->list_gtk.ef_sections_rectangulaire.section == section))
            gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(gtk_builder_get_object(projet->list_gtk.ef_sections_rectangulaire.builder, "EF_section_rectangulaire_textview_nom"))), nom, -1);
#endif
    }
    if ((!isnan(common_math_get(l))) && (!ERREUR_RELATIVE_EGALE(common_math_get(section_data->largeur_retombee), common_math_get(l))))
    {
        section_data->largeur_retombee = l;
        section_data->largeur_table = l;
    }
    else
        l = common_math_f(NAN, FLOTTANT_ORDINATEUR);
    if ((!isnan(common_math_get(h))) && (!ERREUR_RELATIVE_EGALE(common_math_get(section_data->hauteur_retombee), common_math_get(h))))
        section_data->hauteur_retombee = h;
    else
        h = common_math_f(NAN, FLOTTANT_ORDINATEUR);
    section_data->hauteur_table = common_math_f(0., FLOTTANT_ORDINATEUR);
    
    if ((!isnan(common_math_get(l))) || (!isnan(common_math_get(h))))
    {
        GList   *liste_sections = NULL, *liste_barres_dep;
        
        liste_sections = g_list_append(liste_sections, section);
        BUG(_1992_1_1_barres_cherche_dependances(projet, NULL, NULL, liste_sections, NULL, NULL, NULL, NULL, &liste_barres_dep, NULL, FALSE, FALSE), FALSE);
        g_list_free(liste_sections);
        
        if (liste_barres_dep != NULL)
        {
#ifdef ENABLE_GTK
            BUG(m3d_actualise_graphique(projet, NULL, liste_barres_dep), FALSE);
            BUG(m3d_rafraichit(projet), FALSE);
#endif
            BUG(EF_calculs_free(projet), FALSE);
            g_list_free(liste_barres_dep);
        }
    }
    
#ifdef ENABLE_GTK
    if (projet->list_gtk.ef_sections.builder != NULL)
        gtk_widget_queue_draw(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_sections.builder, "EF_sections_treeview")));
#endif
    
    return TRUE;
}


gboolean EF_sections_T_ajout(Projet *projet, const char* nom, Flottant lt, Flottant lr,
  Flottant ht, Flottant hr)
/* Description : Ajouter une nouvelle section en T à la liste des sections en béton.
 * Paramètres : Projet *projet : la variable projet,
 *            : chat *nom : nom de la section,
 *            : Flottant lt : la largeur de la table,
 *            : Flottant lr : la largeur de la retombée,
 *            : Flottant ht : la hauteur de la table,
 *            : Flottant hr : la hauteur de la retombée.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL,
 *             en cas d'erreur d'allocation mémoire.
 */
{
    Section_T   *section_data;
    EF_Section  *section_nouvelle;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(!EF_sections_cherche_nom(projet, nom, FALSE), FALSE, gettext("La section %s existe déjà.\n"), nom);
    BUGMSG(section_nouvelle = malloc(sizeof(EF_Section)), FALSE, gettext("Erreur d'allocation mémoire.\n"));
    BUGMSG(section_data = malloc(sizeof(Section_T)), FALSE, gettext("Erreur d'allocation mémoire.\n"));
    section_nouvelle->data = section_data;
    
    // Les caractéristiques de la section sont les suivantes :\end{verbatim}\begin{displaymath}
    //   S = lt \cdot ht+la \cdot ha\end{displaymath}\begin{displaymath}
    //   cdg_{haut} = \frac{\frac{lt \cdot ht^2}{2}+la \cdot ha \cdot (ht+\frac{ha}{2})}{S}\texttt{  et  }cdg_{bas} = (ht+ha)-cdg_{haut}\texttt{  et  }cdg_{droite} = \frac{lt}{2}\texttt{  et  }cdg_{gauche} = \frac{lt}{2}\end{displaymath}\begin{displaymath}
    //   I_y = \frac{lt \cdot ht^3}{12}+\frac{la \cdot ha^3}{12}+lt \cdot ht \cdot \left(\frac{ht}{2}-cdg_{haut} \right)^2+la \cdot ha \cdot \left(\frac{ha}{2}-cdg_{bas} \right)^2\texttt{  et  }I_z = \frac{ht \cdot lt^3}{12}+\frac{ha \cdot la^3}{12}\end{displaymath}\begin{displaymath}
    //   J = \frac{a \cdot b^3}{16} \cdot \left[\frac{16}{3}-3.364 \cdot \frac{b}{a} \cdot \left(1-\frac{b^4}{12 \cdot a^4}\right)\right]+\frac{aa \cdot bb^3}{16} \cdot \left[\frac{16}{3}-3.364 \cdot \frac{bb}{aa} \cdot \left(1-\frac{bb^4}{12 \cdot aa^4}\right)\right]\texttt{ avec }\substack{a=max(ht,lt)\\b=min(ht,lt)\\aa=max(ha,la)\\bb=min(ha,la)}
    //   \end{displaymath}\begin{verbatim}
    section_nouvelle->type = SECTION_T;
    BUGMSG(section_nouvelle->nom = g_strdup_printf("%s", nom), FALSE, gettext("Erreur d'allocation mémoire.\n"));
    section_data->largeur_table = lt;
    section_data->largeur_retombee = lr;
    section_data->hauteur_table = ht;
    section_data->hauteur_retombee = hr;
    
    BUG(EF_sections_insert(projet, section_nouvelle), FALSE);
    
    return TRUE;
}


gboolean EF_sections_T_modif(Projet *projet, EF_Section *section, const char* nom, Flottant lt,
  Flottant lr, Flottant ht, Flottant hr)
/* Description : Modifie une section en T.
 * Paramètres : Projet *projet : la variable projet,
 *            : EF_Section *section : la section à modifier,
 *            : char* nom : le nouveau nom de la section, NULL si aucun changement,
 *            : Flottant lt : la nouvelle largeur de la table, NAN si aucun changement,
 *            : Flottant lr : la nouvelle largeur de la retombée, NAN si aucun changement,
 *            : Flottant ht : la nouvelle hauteur de la table, NAN si aucun changement,
 *            : Flottant hr : la nouvelle hauteur de la retombée, NAN si aucun changement.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL,
 *             section == NULL, 
 *             section->type != T.
 */
{
    Section_T   *section_data = section->data;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(section, FALSE, gettext("Paramètre %s incorrect.\n"), "section");
    BUGMSG(section->type == SECTION_T, FALSE, gettext("La section doit être de type en T.\n"));
    
    if ((nom != NULL) && (strcmp(section->nom, nom) != 0))
    {
        BUGMSG(!EF_sections_cherche_nom(projet, nom, FALSE), FALSE, gettext("La section %s existe déjà.\n"), nom);
        free(section->nom);
        BUGMSG(section->nom = g_strdup_printf("%s", nom), FALSE, gettext("Erreur d'allocation mémoire.\n"));
        BUG(EF_sections_repositionne(projet, section), FALSE);
#ifdef ENABLE_GTK
        if ((projet->list_gtk.ef_sections_T.builder != NULL) && (projet->list_gtk.ef_sections_T.section == section))
            gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(gtk_builder_get_object(projet->list_gtk.ef_sections_T.builder, "EF_section_T_textview_nom"))), nom, -1);
#endif
    }
    if ((!isnan(common_math_get(lt))) && (!ERREUR_RELATIVE_EGALE(common_math_get(section_data->largeur_table), common_math_get(lt))))
        section_data->largeur_table = lt;
    else
        lt = common_math_f(NAN, FLOTTANT_ORDINATEUR);
    if ((!isnan(common_math_get(lr))) && (!ERREUR_RELATIVE_EGALE(common_math_get(section_data->largeur_retombee), common_math_get(lr))))
        section_data->largeur_retombee = lr;
    else
        lr = common_math_f(NAN, FLOTTANT_ORDINATEUR);
    if ((!isnan(common_math_get(ht))) && (!ERREUR_RELATIVE_EGALE(common_math_get(section_data->hauteur_table), common_math_get(ht))))
        section_data->hauteur_table = ht;
    else
        ht = common_math_f(NAN, FLOTTANT_ORDINATEUR);
    if ((!isnan(common_math_get(hr))) && (!ERREUR_RELATIVE_EGALE(common_math_get(section_data->hauteur_retombee), common_math_get(hr))))
        section_data->hauteur_retombee = hr;
    else
        hr = common_math_f(NAN, FLOTTANT_ORDINATEUR);
    
    if ((!isnan(common_math_get(lt))) || (!isnan(common_math_get(lr))) || (!isnan(common_math_get(ht))) || (!isnan(common_math_get(hr))))
    {
        GList   *liste_sections = NULL, *liste_barres_dep;
        
        liste_sections = g_list_append(liste_sections, section);
        BUG(_1992_1_1_barres_cherche_dependances(projet, NULL, NULL, liste_sections, NULL, NULL, NULL, NULL, &liste_barres_dep, NULL, FALSE, FALSE), FALSE);
        g_list_free(liste_sections);
        
        if (liste_barres_dep != NULL)
        {
#ifdef ENABLE_GTK
            BUG(m3d_actualise_graphique(projet, NULL, liste_barres_dep), FALSE);
            BUG(m3d_rafraichit(projet), FALSE);
#endif
            BUG(EF_calculs_free(projet), FALSE);
            g_list_free(liste_barres_dep);
        }
    }
    
#ifdef ENABLE_GTK
    if (projet->list_gtk.ef_sections.builder != NULL)
        gtk_widget_queue_draw(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_sections.builder, "EF_sections_treeview")));
#endif
    
    return TRUE;
}


gboolean EF_sections_carree_ajout(Projet *projet, const char* nom, Flottant cote)
/* Description : Ajouter une nouvelle section carrée à la liste des sections en béton.
 * Paramètres : Projet *projet : la variable projet,
 *            : char *nom : nom de la section,
 *            : Flottant cote : le coté.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL,
 *             en cas d'erreur d'allocation mémoire.
 */
{
    Section_T   *section_data;
    EF_Section  *section_nouvelle;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(!EF_sections_cherche_nom(projet, nom, FALSE), FALSE, gettext("La section %s existe déjà.\n"), nom);
    BUGMSG(section_nouvelle = malloc(sizeof(EF_Section)), FALSE, gettext("Erreur d'allocation mémoire.\n"));
    BUGMSG(section_data = malloc(sizeof(Section_T)), FALSE, gettext("Erreur d'allocation mémoire.\n"));
    section_nouvelle->data = section_data;
    
    section_nouvelle->type = SECTION_CARREE;
    BUGMSG(section_nouvelle->nom = g_strdup_printf("%s", nom), FALSE, gettext("Erreur d'allocation mémoire.\n"));
    section_data->largeur_retombee = cote;
    section_data->hauteur_retombee = cote;
    section_data->largeur_table = cote;
    section_data->hauteur_table = common_math_f(0., FLOTTANT_ORDINATEUR);
    
    BUG(EF_sections_insert(projet, section_nouvelle), FALSE);
    
    return TRUE;
}


gboolean EF_sections_carree_modif(Projet *projet, EF_Section *section, const char* nom,
  Flottant cote)
/* Description : Modifie une section carrée.
 * Paramètres : Projet *projet : la variable projet,
 *            : EF_Section *section : la section à modifier,
 *            : char *nom : nouveau nom de la section, NULL si aucun changement,
 *            : Flottant cote : le nouveau coté, NAN si aucun changement.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL,
 *             section == NULL,
 *             section->type != CARREE.
 */
{
    Section_T   *section_data = section->data;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(section, FALSE, gettext("Paramètre %s incorrect.\n"), "section");
    BUGMSG(section->type == SECTION_CARREE, FALSE, gettext("La section doit être de type carrée.\n"));
    
    if ((nom != NULL) && (strcmp(section->nom, nom) != 0))
    {
        BUGMSG(!EF_sections_cherche_nom(projet, nom, FALSE), FALSE, gettext("La section %s existe déjà.\n"), nom);
        free(section->nom);
        BUGMSG(section->nom = g_strdup_printf("%s", nom), FALSE, gettext("Erreur d'allocation mémoire.\n"));
        BUG(EF_sections_repositionne(projet, section), FALSE);
#ifdef ENABLE_GTK
        if ((projet->list_gtk.ef_sections_carree.builder != NULL) && (projet->list_gtk.ef_sections_carree.section == section))
            gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(gtk_builder_get_object(projet->list_gtk.ef_sections_carree.builder, "EF_section_carree_textview_nom"))), nom, -1);
#endif
    }
    if ((!isnan(common_math_get(cote))) && (!ERREUR_RELATIVE_EGALE(common_math_get(section_data->largeur_retombee), common_math_get(cote))))
    {
        GList   *liste_sections = NULL, *liste_barres_dep;
        
        section_data->largeur_retombee = cote;
        section_data->hauteur_retombee = cote;
        section_data->largeur_table = cote;
        section_data->hauteur_table = common_math_f(0., FLOTTANT_ORDINATEUR);
        
        liste_sections = g_list_append(liste_sections, section);
        BUG(_1992_1_1_barres_cherche_dependances(projet, NULL, NULL, liste_sections, NULL, NULL, NULL, NULL, &liste_barres_dep, NULL, FALSE, FALSE), FALSE);
        g_list_free(liste_sections);
        
        if (liste_barres_dep != NULL)
        {
#ifdef ENABLE_GTK
            BUG(m3d_actualise_graphique(projet, NULL, liste_barres_dep), FALSE);
            BUG(m3d_rafraichit(projet), FALSE);
#endif
            BUG(EF_calculs_free(projet), FALSE);
            g_list_free(liste_barres_dep);
        }
        
    }
    
#ifdef ENABLE_GTK
    if (projet->list_gtk.ef_sections.builder != NULL)
        gtk_widget_queue_draw(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_sections.builder, "EF_sections_treeview")));
#endif
    
    return TRUE;
}


gboolean EF_sections_circulaire_ajout(Projet *projet, const char* nom, Flottant diametre)
/* Description : Ajouter une nouvelle section circulaire à la liste des sections en béton.
 * Paramètres : Projet *projet : la variable projet,
 *            : char *nom : nom de la section,
 *            : Flottant diametre : le diamètre.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL,
 *             en cas d'erreur d'allocation mémoire.
 */
{
    Section_Circulaire  *section_data;
    EF_Section          *section_nouvelle;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(!EF_sections_cherche_nom(projet, nom, FALSE), FALSE, gettext("La section %s existe déjà.\n"), nom);
    BUGMSG(section_nouvelle = malloc(sizeof(EF_Section)), FALSE, gettext("Erreur d'allocation mémoire.\n"));
    BUGMSG(section_data = malloc(sizeof(Section_Circulaire)), FALSE, gettext("Erreur d'allocation mémoire.\n"));
    section_nouvelle->data = section_data;
    
    // Les caractéristiques de la section sont les suivantes :\end{verbatim}\begin{displaymath}
    //   S = \frac{\pi \cdot diametre^2}{4}\end{displaymath}\begin{displaymath}
    //   cdg_{haut} = \frac{diametre}{2}\texttt{  et  }cdg_{bas} = \frac{diametre}{2}\texttt{  et  }cdg_{droite} = \frac{diametre}{2}\texttt{  et  }cdg_{gauche} = \frac{diametre}{2}\end{displaymath}\begin{displaymath}
    //   I_y = \frac{\pi \cdot diametre^4}{64}\texttt{  et  }I_z = I_y\texttt{  et  }J = \frac{\pi \cdot diametre^4}{32}\end{displaymath}\begin{verbatim}
    section_nouvelle->type = SECTION_CIRCULAIRE;
    BUGMSG(section_nouvelle->nom = g_strdup_printf("%s", nom), FALSE, gettext("Erreur d'allocation mémoire.\n"));
    section_data->diametre = diametre;
    
    BUG(EF_sections_insert(projet, section_nouvelle), FALSE);
    
    return TRUE;
}


gboolean EF_sections_circulaire_modif(Projet *projet, EF_Section *section, const char* nom,
  Flottant diametre)
/* Description : Modifie une section circulaire.
 * Paramètres : Projet *projet : la variable projet,
 *            : EF_Section *section : la section à modifier,
 *            : char *nom : nom de la section, NULL si aucun changement,
 *            : Flottant diametre : le diamètre, NAN si aucun changement.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL,
 *             section == NULL,
 *             section->type != CIRCULAIRE.
 */
{
    Section_Circulaire    *section_data = section->data;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(section, FALSE, gettext("Paramètre %s incorrect.\n"), "section");
    BUGMSG(section->type == SECTION_CIRCULAIRE, FALSE, gettext("La section doit être de type circulaire.\n"));
    
    if ((nom != NULL) && (strcmp(section->nom, nom) != 0))
    {
        BUGMSG(!EF_sections_cherche_nom(projet, nom, FALSE), FALSE, gettext("La section %s existe déjà.\n"), nom);
        free(section->nom);
        BUGMSG(section->nom = g_strdup_printf("%s", nom), FALSE, gettext("Erreur d'allocation mémoire.\n"));
        BUG(EF_sections_repositionne(projet, section), FALSE);
#ifdef ENABLE_GTK
        if ((projet->list_gtk.ef_sections_circulaire.builder != NULL) && (projet->list_gtk.ef_sections_circulaire.section == section))
            gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(gtk_builder_get_object(projet->list_gtk.ef_sections_circulaire.builder, "EF_section_circulaire_textview_nom"))), nom, -1);
#endif
    }
    if ((!isnan(common_math_get(diametre))) && (!ERREUR_RELATIVE_EGALE(common_math_get(section_data->diametre), common_math_get(diametre))))
    {
        GList   *liste_sections = NULL, *liste_barres_dep;
        
        section_data->diametre = diametre;
        
        liste_sections = g_list_append(liste_sections, section);
        BUG(_1992_1_1_barres_cherche_dependances(projet, NULL, NULL, liste_sections, NULL, NULL, NULL, NULL, &liste_barres_dep, NULL, FALSE, FALSE), FALSE);
        g_list_free(liste_sections);
        
        if (liste_barres_dep != NULL)
        {
#ifdef ENABLE_GTK
            BUG(m3d_actualise_graphique(projet, NULL, liste_barres_dep), FALSE);
            BUG(m3d_rafraichit(projet), FALSE);
#endif
            BUG(EF_calculs_free(projet), FALSE);
            g_list_free(liste_barres_dep);
        }
    }
    
#ifdef ENABLE_GTK
    if (projet->list_gtk.ef_sections.builder != NULL)
        gtk_widget_queue_draw(GTK_WIDGET(gtk_builder_get_object(projet->list_gtk.ef_sections.builder, "EF_sections_treeview")));
#endif
    
    return TRUE;
}


EF_Section* EF_sections_cherche_nom(Projet *projet, const char *nom, gboolean critique)
/* Description : Positionne dans la liste des sections en béton l'élément courant au numéro
 *               souhaité.
 * Paramètres : Projet *projet : la variable projet,
 *            : const char *nom : le nom de la section.
 *            : gboolean critique : TRUE si en cas d'echec, la fonction BUG est utilisée.
 * Valeur renvoyée :
 *   Succès : pointeur vers la section
 *   Échec : NULL :
 *             projet == NULL,
 *             section introuvable.
 */
{
    GList   *list_parcours;
    
    BUGMSG(projet, NULL, gettext("Paramètre %s incorrect.\n"), "projet");
    
    // Trivial
    list_parcours = projet->modele.sections;
    while (list_parcours != NULL)
    {
        EF_Section  *section = list_parcours->data;
        
        if (strcmp(section->nom, nom) == 0)
            return section;
        
        list_parcours = g_list_next(list_parcours);
    }
    
    if (critique)
        BUGMSG(0, NULL, gettext("Section en béton '%s' introuvable.\n"), nom);
    else
        return NULL;
}


char* EF_sections_get_description(EF_Section *sect)
/* Description : Renvoie la description d'une section sous forme d'un texte.
 *               Il convient de libérer le texte renvoyée par la fonction free.
 * Paramètres : EF_Section* sect : section à étudier.
 * Valeur renvoyée :
 *   Succès : Résultat
 *   Échec : NULL :
 *             (sect == NULL),
 *             erreur d'allocation mémoire.
 */
{
    char    *description;
    
    BUGMSG(sect, NULL, gettext("Paramètre %s incorrect.\n"), "sect");
    
    switch (sect->type)
    {
        case SECTION_RECTANGULAIRE :
        {
            char        larg[30], haut[30];
            Section_T   *section = sect->data;
            
            common_math_double_to_char2(section->largeur_retombee, larg, DECIMAL_DISTANCE);
            common_math_double_to_char2(section->hauteur_retombee, haut, DECIMAL_DISTANCE);
            BUGMSG(description = g_strdup_printf("%s : %s m, %s : %s m", gettext("Largeur"), larg, gettext("Hauteur"), haut), NULL, gettext("Erreur d'allocation mémoire.\n"));
            
            return description;
        }
        case SECTION_T :
        {
            char    larg_t[30], haut_t[30], larg_r[30], haut_r[30];
            Section_T *section = sect->data;
            
            common_math_double_to_char2(section->largeur_table, larg_t, DECIMAL_DISTANCE);
            common_math_double_to_char2(section->largeur_retombee, larg_r, DECIMAL_DISTANCE);
            common_math_double_to_char2(section->hauteur_table, haut_t, DECIMAL_DISTANCE);
            common_math_double_to_char2(section->hauteur_retombee, haut_r, DECIMAL_DISTANCE);
            BUGMSG(description = g_strdup_printf("%s : %s m, %s : %s m, %s : %s m, %s : %s m", gettext("Largeur table"), larg_t, gettext("Hauteur table"), haut_t, gettext("Largeur retombée"), larg_r, gettext("Hauteur retombée"), haut_r), NULL, gettext("Erreur d'allocation mémoire.\n"));
            
            return description;
        }
        case SECTION_CARREE :
        {
            char         cote[30];
            Section_T   *section = sect->data;
            
            common_math_double_to_char2(section->largeur_table, cote, DECIMAL_DISTANCE);
            BUGMSG(description = g_strdup_printf("%s : %s m", gettext("Coté"), cote), NULL, gettext("Erreur d'allocation mémoire.\n"));
            
            return description;
        }
        case SECTION_CIRCULAIRE :
        {
            char    diam[30];
            Section_Circulaire *section = sect->data;
            
            common_math_double_to_char2(section->diametre, diam, DECIMAL_DISTANCE);
            BUGMSG(description = g_strdup_printf("%s : %s m", gettext("Diamètre"), diam), NULL, gettext("Erreur d'allocation mémoire.\n"));
            
            return description;
        }
        default :
        {
            BUGMSG(0, NULL, gettext("Type de section %d inconnu.\n"), sect->type);
            break;
        }
    }
}


gboolean EF_sections_supprime(EF_Section *section, gboolean annule_si_utilise, Projet *projet)
/* Description : Supprime la section spécifiée.
 * Paramètres : EF_Section *section : la section à supprimer,
 *            : gboolean annule_si_utilise : possibilité d'annuler la suppression si la section
 *              est attribuée à une barre. Si l'option est désactivée, les barres (et les
 *              barres et noeuds intermédiaires dépendants) utilisant la section seront
 *              supprimés.
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL,
 *             section == NULL.
 */
{
    GList   *liste_sections = NULL, *liste_barres_dep;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(section, FALSE, gettext("Paramètre %s incorrect.\n"), "section");
    
    // On vérifie les dépendances.
    liste_sections = g_list_append(liste_sections, section);
    BUG(_1992_1_1_barres_cherche_dependances(projet, NULL, NULL, liste_sections, NULL, NULL, NULL, NULL, &liste_barres_dep, NULL, FALSE, FALSE), FALSE);
    
    if ((annule_si_utilise) && (liste_barres_dep != NULL))
    {
        char *liste;
        
        liste = common_selection_converti_barres_en_texte(liste_barres_dep);
        if (g_list_next(liste_barres_dep) == NULL)
            BUGMSG(NULL, FALSE, gettext("Impossible de supprimer la section car elle est utilisée par la barre %s.\n"), liste);
        else
            BUGMSG(NULL, FALSE, gettext("Impossible de supprimer la section car elle est utilisée par les barres %s.\n"), liste);
    }
    
    BUG(_1992_1_1_barres_supprime_liste(projet, NULL, liste_barres_dep), TRUE);
    g_list_free(liste_sections);
    g_list_free(liste_barres_dep);
    
    free(section->nom);
    free(section->data);
    projet->modele.sections = g_list_remove(projet->modele.sections, section);
    
#ifdef ENABLE_GTK
    gtk_list_store_remove(projet->list_gtk.ef_sections.liste_sections, &section->Iter_liste);
    if (projet->list_gtk.ef_sections.builder != NULL)
        gtk_tree_store_remove(projet->list_gtk.ef_sections.sections, &section->Iter_fenetre);
    switch (section->type)
    {
        case SECTION_RECTANGULAIRE :
        {
            if ((projet->list_gtk.ef_sections_rectangulaire.builder != NULL) && (projet->list_gtk.ef_sections_rectangulaire.section == section))
                gtk_widget_destroy(projet->list_gtk.ef_sections_rectangulaire.window);
            break;
        }
        case SECTION_T :
        {
            if ((projet->list_gtk.ef_sections_T.builder != NULL) && (projet->list_gtk.ef_sections_T.section == section))
                gtk_widget_destroy(projet->list_gtk.ef_sections_T.window);
            break;
        }
        case SECTION_CARREE :
        {
            if ((projet->list_gtk.ef_sections_carree.builder != NULL) && (projet->list_gtk.ef_sections_carree.section == section))
                gtk_widget_destroy(projet->list_gtk.ef_sections_carree.window);
            break;
        }
        case SECTION_CIRCULAIRE :
        {
            if ((projet->list_gtk.ef_sections_circulaire.builder != NULL) && (projet->list_gtk.ef_sections_circulaire.section == section))
                gtk_widget_destroy(projet->list_gtk.ef_sections_circulaire.window);
            break;
        }
        default :
        {
            BUGMSG(0, FALSE, gettext("Type de section %d inconnu.\n"), section->type);
            break;
        }
    }
#endif
    
    free(section);
    
    return TRUE;
}


double EF_sections_j(EF_Section* sect)
/* Description : Renvoie l'inertie de torsion J pour la section étudiée.
 * Paramètres : EF_Section* sect : section à étudier.
 * Valeur renvoyée :
 *   Succès : Résultat
 *   Échec : NAN :
 *             (sect == NULL),
 *             type de section inconnu.
 */
{
    BUGMSG(sect, NAN, gettext("Paramètre %s incorrect.\n"), "sect");
    
    switch (sect->type)
    {
        case SECTION_RECTANGULAIRE :
        case SECTION_T :
        case SECTION_CARREE :
        {
            Section_T *section = sect->data;
            double      lt = common_math_get(section->largeur_table);
            double      lr = common_math_get(section->largeur_retombee);
            double      ht = common_math_get(section->hauteur_table);
            double      hr = common_math_get(section->hauteur_retombee);
            double      a, b, aa, bb;
            
            if (lt > ht)
                { a = lt; b = ht; }
            else
                { a = ht; b = lt; }
            if (lr > hr)
                { aa = lr; bb = hr; }
            else
                { aa = hr; bb = lr; }
            if (sect->type == SECTION_RECTANGULAIRE)
                return aa*bb*bb*bb/16.*(16./3.-3.364*bb/aa*(1.-bb*bb*bb*bb/(12.*aa*aa*aa*aa)));
            else
                return a*b*b*b/16.*(16./3.-3.364*b/a*(1.-b*b*b*b/(12.*a*a*a*a)))+aa*bb*bb*bb/16.*(16./3.-3.364*bb/aa*(1-bb*bb*bb*bb/(12.*aa*aa*aa*aa)));
            
    // Pour une section en T de section constante (lt : largeur de la table, lr : largeur de
    //   la retombée, ht : hauteur de la table, hr : hauteur de la retombée), J vaut :\end{verbatim}\begin{displaymath}
    // J = \frac{a \cdot b^3}{16} \left[\frac{16}{3}-3.364 \frac{b}{a} \left(1-\frac{b^4}{12 a^4}\right)\right]+\frac{aa \cdot bb^3}{16} \left[\frac{16}{3}-3.364 \frac{bb}{aa} \left(1-\frac{bb^4}{12 aa^4}\right)\right]\texttt{ avec }\substack{a=max(h_t,l_t)\\b=min(h_t,l_t)\\aa=max(h_r,l_r)\\bb=min(h_r,l_r)}\end{displaymath}\begin{verbatim}
            break;
        }
        case SECTION_CIRCULAIRE :
        {
            Section_Circulaire  *section = sect->data;
            double              diametre = common_math_get(section->diametre);
            
            return M_PI*diametre*diametre*diametre*diametre/32.;
    // Pour une section circulaire de section constante, J vaut :\end{verbatim}\begin{displaymath}
    // J = \frac{\pi \cdot \phi^4}{32}\end{displaymath}\begin{verbatim}
            break;
        }
        default :
        {
            BUGMSG(0, NAN, gettext("Type de section %d inconnu.\n"), sect->type);
            break;
        }
    }
}


double EF_sections_iy(EF_Section* sect)
/* Description : Renvoie l'inertie I selon l'axe y lorsque la section est constante.
 * Paramètres : EF_Section* section : section à étudier,
 * Valeur renvoyée :
 *   Succès : Résultat.
 *   Échec : NAN :
 *             section == NULL,
 *             type de section inconnu.
 */
{
    BUGMSG(sect, NAN, gettext("Paramètre %s incorrect.\n"), "sect");
    
    switch (sect->type)
    {
        case SECTION_RECTANGULAIRE :
        case SECTION_T :
        case SECTION_CARREE :
        {
            Section_T *section = sect->data;
            double      lt = common_math_get(section->largeur_table);
            double      lr = common_math_get(section->largeur_retombee);
            double      ht = common_math_get(section->hauteur_table);
            double      hr = common_math_get(section->hauteur_retombee);
    // Pour une section en T de section constante (lt : largeur de la table, lr : largeur de
    //   la retombée, ht : hauteur de la table, hr : hauteur de la retombée), Iy vaut :\end{verbatim}\begin{displaymath}
    // I_y = \frac{l_t \cdot h_t^3}{12}+\frac{l_r \cdot h_r^3}{12}+l_t \cdot h_t \cdot \left(\frac{h_t}{2}-cdg_h \right)^2+l_r \cdot h_r \cdot \left(\frac{h_r}{2}-cdg_b \right)^2 \texttt{, }\end{displaymath}\begin{displaymath}
    // cdg_h = \frac{\frac{l_t \cdot h_t^2}{2}+l_r \cdot h_r \cdot \left(h_t+\frac{h_r}{2} \right)}{S}  \texttt{, } cdg_b = h_t+h_r-cdg_h \texttt{ et } S = l_t \cdot h_t+l_r \cdot h_r \end{displaymath}\begin{verbatim}
            double      S = lt*ht+lr*hr;
            double      cdgh = (lt*ht*ht/2.+lr*hr*(ht+hr/2.))/S;
            double      cdgb = (ht+hr)-cdgh;
            return lt*ht*ht*ht/12.+lr*hr*hr*hr/12.+lt*ht*(ht/2.-cdgh)*(ht/2.-cdgh)+lr*hr*(hr/2.-cdgb)*(hr/2.-cdgb);
            
            break;
        }
        case SECTION_CIRCULAIRE :
        {
            Section_Circulaire *section = sect->data;
            double              diametre = common_math_get(section->diametre);
            
            return M_PI*diametre*diametre*diametre*diametre/64.;
    // Pour une section circulaire de section constante, Iy vaut :\end{verbatim}\begin{displaymath}
    // I_y = \frac{\pi \cdot \phi^4}{64} \end{displaymath}\begin{verbatim}
            break;
        }
        default :
        {
            BUGMSG(0, NAN, gettext("Type de section %d inconnu.\n"), sect->type);
            break;
        }
    }
}


double EF_sections_iz(EF_Section* sect)
/* Description : Renvoie l'inertie I selon l'axe z lorsque la section est constante.
 * Paramètres : EF_Section* section : section à étudier.
 * Valeur renvoyée :
 *   Succès : Résultat.
 *   Échec : NAN :
 *             section == NULL,
 *             type de section inconnu.
 */
{
    BUGMSG(sect, NAN, gettext("Paramètre %s incorrect.\n"), "sect");
    
    switch (sect->type)
    {
        case SECTION_RECTANGULAIRE :
        case SECTION_T :
        case SECTION_CARREE :
        {
            Section_T *section = sect->data;
            double      lt = common_math_get(section->largeur_table);
            double      lr = common_math_get(section->largeur_retombee);
            double      ht = common_math_get(section->hauteur_table);
            double      hr = common_math_get(section->hauteur_retombee);
    // Pour une section en T de section constante (lt : largeur de la table, lr : largeur de
    //   la retombée, ht : hauteur de la table, hr : hauteur de la retombée), I vaut :\end{verbatim}\begin{displaymath}
    // I = \frac{h_t \cdot l_t^3}{12}+\frac{h_r \cdot l_r^3}{12}\end{displaymath}\begin{verbatim}
            return ht*lt*lt*lt/12.+hr*lr*lr*lr/12.;
            break;
        }
        case SECTION_CIRCULAIRE :
        {
            Section_Circulaire  *section = sect->data;
            double              diametre = common_math_get(section->diametre);
            return M_PI*diametre*diametre*diametre*diametre/64.;
    // Pour une section circulaire de section constante, I vaut :\end{verbatim}\begin{displaymath}
    // I = \frac{\pi \cdot \phi^4}{64} \end{displaymath}\begin{verbatim}
            break;
        }
        default :
        {
            BUGMSG(0, NAN, gettext("Type de section %d inconnu.\n"), sect->type);
            break;
        }
    }
}


double EF_sections_vy(EF_Section* sect)
/* Description : Renvoie la distance entre le centre de gravité et la partie la plus à droite
 *               de la section.
 * Paramètres : EF_Section* sect : section à étudier.
 * Valeur renvoyée :
 *   Succès : Résultat.
 *   Échec : NAN :
 *             section == NULL,
 *             type de section inconnu.
 */
{
    BUGMSG(sect, NAN, gettext("Paramètre %s incorrect.\n"), "sect");
    
    switch (sect->type)
    {
        case SECTION_RECTANGULAIRE :
        case SECTION_T :
        case SECTION_CARREE :
        {
            Section_T *section = sect->data;
            
            return MAX(common_math_get(section->largeur_table), common_math_get(section->largeur_retombee))/2.;
            
            break;
        }
        case SECTION_CIRCULAIRE :
        {
            Section_Circulaire *section = sect->data;
            
            return common_math_get(section->diametre)/2.;
            
            break;
        }
        default :
        {
            BUGMSG(0, NAN, gettext("Type de section %d inconnu.\n"), sect->type);
            break;
        }
    }
}


double EF_sections_vyp(EF_Section* sect)
/* Description : Renvoie la distance entre le centre de gravité et la partie la plus à gauche
 *               de la section.
 * Paramètres : EF_Section* sect : section à étudier.
 * Valeur renvoyée :
 *   Succès : Résultat.
 *   Échec : NAN :
 *             section == NULL,
 *             type de section inconnu.
 */
{
    BUGMSG(sect, NAN, gettext("Paramètre %s incorrect.\n"), "sect");
    
    switch (sect->type)
    {
        case SECTION_RECTANGULAIRE :
        case SECTION_T :
        case SECTION_CARREE :
        {
            Section_T *section = sect->data;
            
            return MAX(common_math_get(section->largeur_table), common_math_get(section->largeur_retombee))/2.;
            
            break;
        }
        case SECTION_CIRCULAIRE :
        {
            Section_Circulaire *section = sect->data;
            
            return common_math_get(section->diametre)/2.;
            
            break;
        }
        default :
        {
            BUGMSG(0, NAN, gettext("Type de section %d inconnu.\n"), sect->type);
            break;
        }
    }
}


double EF_sections_vz(EF_Section* sect)
/* Description : Renvoie la distance entre le centre de gravité et la partie la plus haute de
 *               la section.
 * Paramètres : EF_Section* sect : section à étudier.
 * Valeur renvoyée :
 *   Succès : Résultat.
 *   Échec : NAN :
 *             section == NULL,
 *             type de section inconnu.
 */
{
    BUGMSG(sect, NAN, gettext("Paramètre %s incorrect.\n"), "sect");
    
    switch (sect->type)
    {
        case SECTION_RECTANGULAIRE :
        case SECTION_T :
        case SECTION_CARREE :
        {
            Section_T *section = sect->data;
            
            return (common_math_get(section->largeur_table)*common_math_get(section->hauteur_table)*common_math_get(section->hauteur_table)/2.+common_math_get(section->largeur_retombee)*common_math_get(section->hauteur_retombee)*(common_math_get(section->hauteur_retombee)/2.+common_math_get(section->hauteur_table)))/EF_sections_s(sect);
            
            break;
        }
        case SECTION_CIRCULAIRE :
        {
            Section_Circulaire *section = sect->data;
            
            return common_math_get(section->diametre)/2.;
            
            break;
        }
        default :
        {
            BUGMSG(0, NAN, gettext("Type de section %d inconnu.\n"), sect->type);
            break;
        }
    }
}


double EF_sections_vzp(EF_Section* sect)
/* Description : Renvoie la distance entre le centre de gravité et la partie la plus basse de
 *               la section.
 * Paramètres : EF_Section* sect : section à étudier.
 * Valeur renvoyée :
 *   Succès : Résultat.
 *   Échec : NAN :
 *             section == NULL,
 *             type de section inconnu.
 */
{
    BUGMSG(sect, NAN, gettext("Paramètre %s incorrect.\n"), "sect");
    
    switch (sect->type)
    {
        case SECTION_RECTANGULAIRE :
        case SECTION_T :
        case SECTION_CARREE :
        {
            Section_T *section = sect->data;
            
            return (common_math_get(section->largeur_table)*common_math_get(section->hauteur_table)*(common_math_get(section->hauteur_retombee)+common_math_get(section->hauteur_table)/2.)+common_math_get(section->largeur_retombee)*common_math_get(section->hauteur_retombee)*(common_math_get(section->hauteur_retombee)/2.))/EF_sections_s(sect);
            
            break;
        }
        case SECTION_CIRCULAIRE :
        {
            Section_Circulaire *section = sect->data;
            
            return common_math_get(section->diametre)/2.;
            
            break;
        }
        default :
        {
            BUGMSG(0, NAN, gettext("Type de section %d inconnu.\n"), sect->type);
            break;
        }
    }
}


double EF_sections_ay(Beton_Barre *barre, unsigned int discretisation)
/* Description : Renvoie le paramètre de souplesse a de la poutre selon l'axe y.
 * Paramètres : Beton_Barre *barre : la barre à étudier,
 *              unsigned int discretisation : partie de la barre à étudier.
 * Valeur renvoyée :
 *   Succès : Résultat
 *   Échec : NAN :
 *             barre == NULL,
 *             discretisation>barre->discretisation_element,
 *             type de section inconnu.
 */
{
    EF_Noeud    *debut, *fin;
    double      ll;
    double      E;
    
    BUGMSG(barre, NAN, gettext("Paramètre %s incorrect.\n"), "barre");
    BUGMSG(discretisation<=barre->discretisation_element, NAN, gettext("La discrétisation %d souhaitée est hors domaine %d.\n"), discretisation, barre->discretisation_element);
    
    // Le coefficient a est défini par la formule :\end{verbatim}\begin{displaymath}
    // a_y = \frac{1}{l^2}\int_0^l \frac{(l-x)^2}{E \cdot I_y(x)} dx\end{displaymath}\begin{verbatim}
    
    if (discretisation == 0)
        debut = barre->noeud_debut;
    else
        debut = g_list_nth_data(barre->noeuds_intermediaires, discretisation-1);
    if (discretisation == barre->discretisation_element)
        fin = barre->noeud_fin;
    else
        fin = g_list_nth_data(barre->noeuds_intermediaires, discretisation);
    
    ll = EF_noeuds_distance(fin, debut);
    BUG(!isnan(ll), NAN);
    
    E = EF_calculs_E(barre->materiau);
    
    switch (barre->section->type)
    {
        case SECTION_RECTANGULAIRE :
        case SECTION_T :
        case SECTION_CARREE :
        case SECTION_CIRCULAIRE :
        {
            return ll/(3.*E*EF_sections_iy(barre->section));
            break;
        }
        default :
        {
            BUGMSG(0, NAN, gettext("Type de section %d inconnu.\n"), barre->section->type);
            break;
        }
    }
}


double EF_sections_by(Beton_Barre *barre, unsigned int discretisation)
/* Description : Renvoie le paramètre de souplesse b de la poutre selon l'axe y.
 * Paramètres : Beton_Barre *barre : la barre à étudier,
 *              unsigned int discretisation : partie de la barre à étudier.
 * Valeur renvoyée :
 *   Succès : Résultat
 *   Échec : NAN :
 *             barre == NULL,
 *             discretisation>barre->discretisation_element,
 *             type de section inconnu.
 */
{
    EF_Noeud    *debut, *fin;
    double      ll;
    double      E;
    
    BUGMSG(barre, NAN, gettext("Paramètre %s incorrect.\n"), "barre");
    BUGMSG(discretisation<=barre->discretisation_element, NAN, gettext("La discrétisation %d souhaitée est hors domaine %d.\n"), discretisation, barre->discretisation_element);
    
    // Le coefficient b est défini par la formule :\end{verbatim}\begin{displaymath}
    // b_y = \frac{1}{l^2}\int_0^l \frac{x \cdot (l-x)^2}{E \cdot I_y(x)} dx\end{displaymath}\begin{verbatim}
    
    if (discretisation == 0)
        debut = barre->noeud_debut;
    else
        debut = g_list_nth_data(barre->noeuds_intermediaires, discretisation-1);
    if (discretisation == barre->discretisation_element)
        fin = barre->noeud_fin;
    else
        fin = g_list_nth_data(barre->noeuds_intermediaires, discretisation);
    
    ll = EF_noeuds_distance(fin, debut);
    BUG(!isnan(ll), NAN);
    
    E = EF_calculs_E(barre->materiau);
    
    switch (barre->section->type)
    {
        case SECTION_RECTANGULAIRE :
        case SECTION_T :
        case SECTION_CARREE :
        case SECTION_CIRCULAIRE :
        {
            return ll/(6.*E*EF_sections_iy(barre->section));
            break;
        }
        default :
        {
            BUGMSG(0, NAN, gettext("Type de section %d inconnu.\n"), barre->section->type);
            break;
        }
    }
}


double EF_sections_cy(Beton_Barre *barre, unsigned int discretisation)
/* Description : Renvoie le paramètre de souplesse c de la poutre selon l'axe y.
 * Paramètres : Beton_Barre *barre : la barre à étudier,
 *              unsigned int discretisation : partie de la barre à étudier.
 * Valeur renvoyée :
 *   Succès : Résultat
 *   Échec : NAN :
 *             barre == NULL,
 *             discretisation>barre->discretisation_element,
 *             type de section inconnu.
 */
{
    EF_Noeud    *debut, *fin;
    double      ll;
    double      E;
    
    BUGMSG(barre, NAN, gettext("Paramètre %s incorrect.\n"), "barre");
    BUGMSG(discretisation<=barre->discretisation_element, NAN, gettext("La discrétisation %d souhaitée est hors domaine %d.\n"), discretisation, barre->discretisation_element);
    
    // Le coefficient c est défini par la formule :\end{verbatim}\begin{displaymath}
    // c_y = \frac{1}{l^2}\int_0^l \frac{x^2}{E \cdot I_y(x)} dx\end{displaymath}\begin{verbatim}
    
    if (discretisation == 0)
        debut = barre->noeud_debut;
    else
        debut = g_list_nth_data(barre->noeuds_intermediaires, discretisation-1);
    if (discretisation == barre->discretisation_element)
        fin = barre->noeud_fin;
    else
        fin = g_list_nth_data(barre->noeuds_intermediaires, discretisation);
    
    ll = EF_noeuds_distance(fin, debut);
    BUG(!isnan(ll), NAN);
    
    E = EF_calculs_E(barre->materiau);
    
    switch (barre->section->type)
    {
        case SECTION_RECTANGULAIRE :
        case SECTION_T :
        case SECTION_CARREE :
        case SECTION_CIRCULAIRE :
        {
            return ll/(3.*E*EF_sections_iy(barre->section));
            break;
        }
        default :
        {
            BUGMSG(0, NAN, gettext("Type de section %d inconnu.\n"), barre->section->type);
            break;
        }
    }
}


double EF_sections_az(Beton_Barre *barre, unsigned int discretisation)
/* Description : Renvoie le paramètre de souplesse a de la poutre selon l'axe z.
 * Paramètres : Beton_Barre *barre : la barre à étudier,
 *              unsigned int discretisation : partie de la barre à étudier.
 * Valeur renvoyée :
 *   Succès : Résultat
 *   Échec : NAN :
 *             barre == NULL,
 *             discretisation>barre->discretisation_element,
 *             type de section inconnu.
 */
{
    EF_Noeud    *debut, *fin;
    double      ll;
    double      E;
    
    BUGMSG(barre, NAN, gettext("Paramètre %s incorrect.\n"), "barre");
    BUGMSG(discretisation<=barre->discretisation_element, NAN, gettext("La discrétisation %d souhaitée est hors domaine %d.\n"), discretisation, barre->discretisation_element);
    
    // Le coefficient a est défini par la formule :\end{verbatim}\begin{displaymath}
    // a_z = \frac{1}{l^2}\int_0^l \frac{(l-x)^2}{E \cdot I_z(x)} dx\end{displaymath}\begin{verbatim}
    
    if (discretisation == 0)
        debut = barre->noeud_debut;
    else
        debut = g_list_nth_data(barre->noeuds_intermediaires, discretisation-1);
    if (discretisation == barre->discretisation_element)
        fin = barre->noeud_fin;
    else
        fin = g_list_nth_data(barre->noeuds_intermediaires, discretisation);
    
    ll = EF_noeuds_distance(fin, debut);
    BUG(!isnan(ll), NAN);
    
    E = EF_calculs_E(barre->materiau);
    
    switch (barre->section->type)
    {
        case SECTION_RECTANGULAIRE :
        case SECTION_T :
        case SECTION_CARREE :
        case SECTION_CIRCULAIRE :
        {
            return ll/(3.*E*EF_sections_iz(barre->section));
            break;
        }
        default :
        {
            BUGMSG(0, NAN, gettext("Type de section %d inconnu.\n"), barre->section->type);
            break;
        }
    }
}


double EF_sections_bz(Beton_Barre *barre, unsigned int discretisation)
/* Description : Renvoie le paramètre de souplesse b de la poutre selon l'axe z.
 * Paramètres : Beton_Barre *barre : la barre à étudier,
 *              unsigned int discretisation : partie de la barre à étudier.
 * Valeur renvoyée :
 *   Succès : Résultat
 *   Échec : NAN :
 *             barre == NULL,
 *             discretisation>barre->discretisation_element,
 *             type de section inconnu.
 */
{
    EF_Noeud    *debut, *fin;
    double      ll;
    double      E;
    
    BUGMSG(barre, NAN, gettext("Paramètre %s incorrect.\n"), "barre");
    BUGMSG(discretisation<=barre->discretisation_element, NAN, gettext("La discrétisation %d souhaitée est hors domaine %d.\n"), discretisation, barre->discretisation_element);
    
    // Le coefficient b est défini par la formule :\end{verbatim}\begin{displaymath}
    // b_z = \frac{1}{l^2}\int_0^l \frac{x \cdot (l-x)^2}{E \cdot I_z(x)} dx\end{displaymath}\begin{verbatim}
    
    if (discretisation == 0)
        debut = barre->noeud_debut;
    else
        debut = g_list_nth_data(barre->noeuds_intermediaires, discretisation-1);
    if (discretisation == barre->discretisation_element)
        fin = barre->noeud_fin;
    else
        fin = g_list_nth_data(barre->noeuds_intermediaires, discretisation);
    
    ll = EF_noeuds_distance(fin, debut);
    BUG(!isnan(ll), NAN);
    
    E = EF_calculs_E(barre->materiau);
    
    switch (barre->section->type)
    {
        case SECTION_RECTANGULAIRE :
        case SECTION_T :
        case SECTION_CARREE :
        case SECTION_CIRCULAIRE :
        {
            return ll/(6.*E*EF_sections_iz(barre->section));
            break;
        }
        default :
        {
            BUGMSG(0, NAN, gettext("Type de section %d inconnu.\n"), barre->section->type);
            break;
        }
    }
}


double EF_sections_cz(Beton_Barre *barre, unsigned int discretisation)
/* Description : Renvoie le paramètre de souplesse c de la poutre selon l'axe z.
 * Paramètres : Beton_Barre *barre : la barre à étudier,
 *              unsigned int discretisation : partie de la barre à étudier.
 * Valeur renvoyée :
 *   Succès : Résultat
 *   Échec : NAN :
 *             barre == NULL,
 *             discretisation>barre->discretisation_element,
 *             type de section inconnu.
 */
{
    EF_Noeud    *debut, *fin;
    double      ll;
    double      E;
    
    BUGMSG(barre, NAN, gettext("Paramètre %s incorrect.\n"), "barre");
    BUGMSG(discretisation<=barre->discretisation_element, NAN, gettext("La discrétisation %d souhaitée est hors domaine %d.\n"), discretisation, barre->discretisation_element);
    
    // Le coefficient c est défini par la formule :\end{verbatim}\begin{displaymath}
    // c_z = \frac{1}{l^2}\int_0^l \frac{x^2}{E \cdot I_y(x)} dx\end{displaymath}\begin{verbatim}
    
    if (discretisation == 0)
        debut = barre->noeud_debut;
    else
        debut = g_list_nth_data(barre->noeuds_intermediaires, discretisation-1);
    if (discretisation == barre->discretisation_element)
        fin = barre->noeud_fin;
    else
        fin = g_list_nth_data(barre->noeuds_intermediaires, discretisation);
    
    ll = EF_noeuds_distance(fin, debut);
    BUG(!isnan(ll), NAN);
    
    E = EF_calculs_E(barre->materiau);
    
    switch (barre->section->type)
    {
        case SECTION_RECTANGULAIRE :
        case SECTION_T :
        case SECTION_CARREE :
        case SECTION_CIRCULAIRE :
        {
            return ll/(3.*E*EF_sections_iz(barre->section));
            break;
        }
        default :
        {
            BUGMSG(0, NAN, gettext("Type de section %d inconnu.\n"), barre->section->type);
            break;
        }
    }
}


double EF_sections_s(EF_Section *sect)
/* Description : Renvoie la surface de la section étudiée.
 * Paramètres : void* section : section à étudier.
 * Valeur renvoyée :
 *   Succès : Résultat
 *   Échec : NAN en cas de paramètres invalides :
 *             sect == NULL,
 *             type de section inconnue.
 */
{
    BUGMSG(sect, NAN, gettext("Paramètre %s incorrect.\n"), "sect");
    
    switch (sect->type)
    {
        case SECTION_RECTANGULAIRE :
        case SECTION_T :
        case SECTION_CARREE :
        {
            Section_T *section = sect->data;
            return common_math_get(section->hauteur_table)*common_math_get(section->largeur_table)+common_math_get(section->hauteur_retombee)*common_math_get(section->largeur_retombee);
            
    // Pour une section en T de section constante (lt : largeur de la table, lr : largeur de
    //   la retombée, ht : hauteur de la table, hr : hauteur de la retombée), S vaut :\end{verbatim}\begin{displaymath}
    // S = h_t \cdot l_t+h_r \cdot l_r\end{displaymath}\begin{verbatim}
            break;
        }
        case SECTION_CIRCULAIRE :
        {
            Section_Circulaire *section = sect->data;
            return M_PI*common_math_get(section->diametre)*common_math_get(section->diametre)/4.;
    // Pour une section circulaire de section constante, S vaut :\end{verbatim}\begin{displaymath}
    // S = \frac{\pi \cdot \phi^2}{4} \end{displaymath}\begin{verbatim}
            break;
        }
        default :
        {
            BUGMSG(0, NAN, gettext("Type de section %d inconnu.\n"), sect->type);
            break;
        }
    }
}


double EF_sections_es_l(Beton_Barre *barre, unsigned int discretisation, double d, double f)
/* Description : Renvoie l'équivalent du rapport ES/L pour la barre étudiée.
 * Paramètres : Beton_Barre *barre : la barre à étudier,
 *              unsigned int discretisation : partie de la barre à étudier,
 *              double d : début de la partie à prendre en compte,
 *              double f : fin de la partie à prendre en compte.
 * Valeur renvoyée :
 *   Succès : Résultat
 *   Échec : NAN :
 *             barre == NULL,
 *             discretisation>barre->discretisation_element,
 *             debut>fin,
 *             type de section inconnue.
 */
{
    BUGMSG(barre, NAN, gettext("Paramètre %s incorrect.\n"), "barre");
    BUGMSG(discretisation<=barre->discretisation_element, NAN, gettext("La discrétisation %d souhaitée est hors domaine %d.\n"), discretisation, barre->discretisation_element);
    BUGMSG(!((d>f) && (!(ERREUR_RELATIVE_EGALE(d, f)))), NAN, gettext("\n"));
    
    // Le facteur ES/L est défini par la formule :\end{verbatim}\begin{displaymath}
    // \frac{E \cdot S}{L} = \frac{E}{\int_d^f \frac{1}{S(x)} dx}\end{displaymath}\begin{verbatim}
    
    return EF_calculs_E(barre->materiau)*EF_sections_s(barre->section)/(f-d);
}


double EF_sections_gj_l(Beton_Barre *barre, unsigned int discretisation)
/* Description : Renvoie l'équivalent du rapport GJ/L pour la barre étudiée.
 * Paramètres : Beton_Barre *barre : la barre à étudier,
 *              unsigned int discretisation : partie de la barre à étudier.
 * Valeur renvoyée :
 *   Succès : Résultat
 *   Échec : NAN :
 *             barre == NULL,
 *             discretisation>barre->discretisation_element,
 *             type de section inconnue.
 */
{
    EF_Noeud    *debut, *fin;
    double      ll;
    
    BUGMSG(barre, NAN, gettext("Paramètre %s incorrect.\n"), "barre");
    BUGMSG(discretisation<=barre->discretisation_element, NAN, gettext("La discrétisation %d souhaitée est hors domaine %d.\n"), discretisation, barre->discretisation_element);
    
    // Le facteur GJ/L est défini par la formule :\end{verbatim}\begin{displaymath}
    // \frac{G \cdot J}{L} = \frac{G}{\int_0^l \frac{1}{J(x)} dx}\end{displaymath}\begin{verbatim}
    
    if (discretisation == 0)
        debut = barre->noeud_debut;
    else
        debut = g_list_nth_data(barre->noeuds_intermediaires, discretisation-1);
    if (discretisation == barre->discretisation_element)
        fin = barre->noeud_fin;
    else
        fin = g_list_nth_data(barre->noeuds_intermediaires, discretisation);
    
    ll = EF_noeuds_distance(fin, debut);
    BUG(!isnan(ll), NAN);
    
    return EF_calculs_G(barre->materiau, FALSE)*EF_sections_j(barre->section)/ll;
}


void EF_sections_free_un(EF_Section *section)
/* Description : Fonction permettant de libérer une section.
 * Paramètres : EF_Section *section : section à libérer.
 * Valeur renvoyée : Aucun.
 */
{
    free(section->nom);
    free(section->data);
    free(section);
    
    return;
}


gboolean EF_sections_free(Projet *projet)
/* Description : Libère l'ensemble des sections en béton
 * Paramètres : Projet *projet : la variable projet
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL.
 */
{
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    
    // Trivial
    if (projet->modele.sections != NULL)
    {
        g_list_free_full(projet->modele.sections, (GDestroyNotify)&EF_sections_free_un);
        projet->modele.sections = NULL;
    }
    
    BUG(EF_calculs_free(projet), FALSE);
    
#ifdef ENABLE_GTK
    g_object_unref(projet->list_gtk.ef_sections.liste_sections);
#endif
    
    return TRUE;
}
