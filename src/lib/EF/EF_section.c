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
#include "common_erreurs.h"
#include "common_maths.h"
#include "common_selection.h"
#include "EF_noeud.h"
#include "EF_calculs.h"
#include "1992_1_1_barres.h"
#include "EF_section.h"

#ifdef ENABLE_GTK
#include "EF_gtk_sections.hpp"
#endif

G_MODULE_EXPORT gboolean EF_sections_init(Projet *projet)
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
    projet->beton.sections = NULL;
    
#ifdef ENABLE_GTK
    projet->list_gtk.ef_sections.liste_sections = gtk_list_store_new(1, G_TYPE_STRING);
#endif
    
    return TRUE;
}


G_MODULE_EXPORT gboolean EF_sections_update_ligne_treeview(Projet *projet, EF_Section *section)
/* Description : Met à jour les données dans le treeview de la fenêtre section.
 * Paramètres : Projet *projet : la variable projet,
 *            : EF_Section *section : la section à mettre à jour.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL,
 *             section == NULL,
 *             fenetre section non initialisée.
 */
{
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(section, FALSE, gettext("Paramètre %s incorrect.\n"), "section");
    
    if (projet->list_gtk.ef_sections.builder != NULL)
    {
        char        *description;
        GdkPixbuf   *pixbuf = EF_gtk_sections_dessin(section, 32, 32);
        char        j[30], iy[30], iz[30], s[30], vy[30], vyp[30], vz[30], vzp[30];
        
        BUG(description = EF_sections_get_description(section), FALSE);
        common_math_double_to_char(EF_sections_j(section), j, DECIMAL_M4);
        common_math_double_to_char(EF_sections_iy(section), iy, DECIMAL_M4);
        common_math_double_to_char(EF_sections_iz(section), iz, DECIMAL_M4);
        common_math_double_to_char(EF_sections_s(section), s, DECIMAL_SURFACE);
        common_math_double_to_char(EF_sections_vy(section), vy, DECIMAL_DISTANCE);
        common_math_double_to_char(EF_sections_vyp(section), vyp, DECIMAL_DISTANCE);
        common_math_double_to_char(EF_sections_vz(section), vz, DECIMAL_DISTANCE);
        common_math_double_to_char(EF_sections_vzp(section), vzp, DECIMAL_DISTANCE);
        
        gtk_tree_store_set(projet->list_gtk.ef_sections.sections, &section->Iter_fenetre, 0, pixbuf, 1, section->nom, 2, description, 3, j, 4, iy, 5, iz, 6, s, 7, vy, 8, vyp, 9, vz, 10, vzp, -1);
        free(description);
        g_object_unref(pixbuf);
    }
    
    return TRUE;
}


G_MODULE_EXPORT gboolean EF_sections_ajout_rectangulaire(Projet *projet, const char* nom,
  double l, double h)
/* Description : Ajouter une nouvelle section rectangulaire à la liste des sections en béton.
 * Paramètres : Projet *projet : la variable projet,
 *            : double l : la largeur,
 *            : double h : la hauteur.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL,
 *             en cas d'erreur d'allocation mémoire.
 */
{
    Section_T   *section_data = malloc(sizeof(Section_T));
    EF_Section  *section_nouvelle = malloc(sizeof(EF_Section));
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(section_nouvelle, FALSE, gettext("Erreur d'allocation mémoire.\n"));
    BUGMSG(section_data, FALSE, gettext("Erreur d'allocation mémoire.\n"));
    section_nouvelle->data = section_data;
    
    // Trivial
    section_nouvelle->type = SECTION_RECTANGULAIRE;
    BUGMSG(section_nouvelle->nom = g_strdup_printf("%s", nom), FALSE, gettext("Erreur d'allocation mémoire.\n"));
    section_data->largeur_ame = l;
    section_data->hauteur_ame = h;
    section_data->largeur_table = 0.;
    section_data->hauteur_table = 0.;
    
    projet->beton.sections = g_list_append(projet->beton.sections, section_nouvelle);
    
#ifdef ENABLE_GTK
    gtk_list_store_append(projet->list_gtk.ef_sections.liste_sections, &section_nouvelle->Iter_liste);
    gtk_list_store_set(projet->list_gtk.ef_sections.liste_sections, &section_nouvelle->Iter_liste, 0, nom, -1);
    BUG(EF_sections_update_ligne_treeview(projet, section_nouvelle), FALSE);
#endif
    
    return TRUE;
}


G_MODULE_EXPORT gboolean EF_sections_ajout_T(Projet *projet, const char* nom, double lt,
  double la, double ht, double ha)
/* Description : Ajouter une nouvelle section en T à la liste des sections en béton.
 * Paramètres : Projet *projet : la variable projet,
 *            : double lt : la largeur de la table,
 *            : double la : la largeur de l'âme,
 *            : double ht : la hauteur de la table,
 *            : double ha : la hauteur de l'âme.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL,
 *             en cas d'erreur d'allocation mémoire.
 */
{
    Section_T *section_data = malloc(sizeof(Section_T));
    EF_Section      *section_nouvelle = malloc(sizeof(EF_Section));
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(section_nouvelle, FALSE, gettext("Erreur d'allocation mémoire.\n"));
    BUGMSG(section_data, FALSE, gettext("Erreur d'allocation mémoire.\n"));
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
    section_data->largeur_ame = la;
    section_data->hauteur_table = ht;
    section_data->hauteur_ame = ha;
    
    projet->beton.sections = g_list_append(projet->beton.sections, section_nouvelle);
    
#ifdef ENABLE_GTK
    gtk_list_store_append(projet->list_gtk.ef_sections.liste_sections, &section_nouvelle->Iter_liste);
    gtk_list_store_set(projet->list_gtk.ef_sections.liste_sections, &section_nouvelle->Iter_liste, 0, nom, -1);
    BUG(EF_sections_update_ligne_treeview(projet, section_nouvelle), FALSE);
#endif
    
    return TRUE;
}


G_MODULE_EXPORT gboolean EF_sections_ajout_carre(Projet *projet, const char* nom,
  double cote)
/* Description : Ajouter une nouvelle section carrée à la liste des sections en béton.
 * Paramètres : Projet *projet : la variable projet,
 *            : double cote : le coté.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL,
 *             en cas d'erreur d'allocation mémoire.
 */
{
    Section_Carree  *section_data = malloc(sizeof(Section_Carree));
    EF_Section      *section_nouvelle = malloc(sizeof(EF_Section));
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(section_nouvelle, FALSE, gettext("Erreur d'allocation mémoire.\n"));
    BUGMSG(section_data, FALSE, gettext("Erreur d'allocation mémoire.\n"));
    section_nouvelle->data = section_data;
    
    // Les caractéristiques de la section sont les suivantes :\end{verbatim}\begin{displaymath}
    //   S = cote^2\texttt{  et  }cdg_{haut} = \frac{cote}{2}\texttt{  et  }cdg_{bas} = \frac{cote}{2}\texttt{  et  }cdg_{droite} = \frac{cote}{2}\texttt{  et  }cdg_{gauche} = \frac{cote}{2}\end{displaymath}\begin{displaymath}
    //   I_y = \frac{cote^4}{12}\texttt{  et  }I_z = I_y\texttt{  et  }J = \frac{cote^4}{16} \cdot \left[\frac{16}{3}-3.364 \cdot \left(1-\frac{1}{12}\right)\right]\end{displaymath}\begin{verbatim}
    section_nouvelle->type = SECTION_CARREE;
    BUGMSG(section_nouvelle->nom = g_strdup_printf("%s", nom), FALSE, gettext("Erreur d'allocation mémoire.\n"));
    section_data->cote = cote;
    
    projet->beton.sections = g_list_append(projet->beton.sections, section_nouvelle);
    
#ifdef ENABLE_GTK
    gtk_list_store_append(projet->list_gtk.ef_sections.liste_sections, &section_nouvelle->Iter_liste);
    gtk_list_store_set(projet->list_gtk.ef_sections.liste_sections, &section_nouvelle->Iter_liste, 0, nom, -1);
    BUG(EF_sections_update_ligne_treeview(projet, section_nouvelle), FALSE);
#endif
    
    return TRUE;
}


G_MODULE_EXPORT gboolean EF_sections_ajout_circulaire(Projet *projet, const char* nom,
  double diametre)
/* Description : Ajouter une nouvelle section circulaire à la liste des sections en béton.
 * Paramètres : Projet *projet : la variable projet,
 *            : double diametre : le diamètre.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL,
 *             en cas d'erreur d'allocation mémoire.
 */
{
    Section_Circulaire    *section_data = malloc(sizeof(Section_Circulaire));
    EF_Section                  *section_nouvelle = malloc(sizeof(EF_Section));
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(section_nouvelle, FALSE, gettext("Erreur d'allocation mémoire.\n"));
    BUGMSG(section_data, FALSE, gettext("Erreur d'allocation mémoire.\n"));
    section_nouvelle->data = section_data;
    
    // Les caractéristiques de la section sont les suivantes :\end{verbatim}\begin{displaymath}
    //   S = \frac{\pi \cdot diametre^2}{4}\end{displaymath}\begin{displaymath}
    //   cdg_{haut} = \frac{diametre}{2}\texttt{  et  }cdg_{bas} = \frac{diametre}{2}\texttt{  et  }cdg_{droite} = \frac{diametre}{2}\texttt{  et  }cdg_{gauche} = \frac{diametre}{2}\end{displaymath}\begin{displaymath}
    //   I_y = \frac{\pi \cdot diametre^4}{64}\texttt{  et  }I_z = I_y\texttt{  et  }J = \frac{\pi \cdot diametre^4}{32}\end{displaymath}\begin{verbatim}
    section_nouvelle->type = SECTION_CIRCULAIRE;
    BUGMSG(section_nouvelle->nom = g_strdup_printf("%s", nom), FALSE, gettext("Erreur d'allocation mémoire.\n"));
    section_data->diametre = diametre;
    
    projet->beton.sections = g_list_append(projet->beton.sections, section_nouvelle);
    
#ifdef ENABLE_GTK
    gtk_list_store_append(projet->list_gtk.ef_sections.liste_sections, &section_nouvelle->Iter_liste);
    gtk_list_store_set(projet->list_gtk.ef_sections.liste_sections, &section_nouvelle->Iter_liste, 0, nom, -1);
    BUG(EF_sections_update_ligne_treeview(projet, section_nouvelle), FALSE);
#endif
    
    return TRUE;
}


G_MODULE_EXPORT EF_Section* EF_sections_cherche_nom(Projet *projet, const char *nom,
  gboolean critique)
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
    list_parcours = projet->beton.sections;
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


G_MODULE_EXPORT char* EF_sections_get_description(EF_Section *sect)
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
            
            common_math_double_to_char(section->largeur_ame, larg, DECIMAL_DISTANCE);
            common_math_double_to_char(section->hauteur_ame, haut, DECIMAL_DISTANCE);
            BUGMSG(description = g_strdup_printf("%s : %s m, %s : %s m", gettext("Largeur"), larg, gettext("Hauteur"), haut), NULL, gettext("Erreur d'allocation mémoire.\n"));
            
            return description;
        }
        case SECTION_T :
        {
            char    larg_t[30], haut_t[30], larg_a[30], haut_a[30];
            Section_T *section = sect->data;
            
            common_math_double_to_char(section->largeur_table, larg_t, DECIMAL_DISTANCE);
            common_math_double_to_char(section->largeur_ame, larg_a, DECIMAL_DISTANCE);
            common_math_double_to_char(section->hauteur_table, haut_t, DECIMAL_DISTANCE);
            common_math_double_to_char(section->hauteur_ame, haut_a, DECIMAL_DISTANCE);
            BUGMSG(description = g_strdup_printf("%s : %s m, %s : %s m, %s : %s m, %s : %s m", gettext("Largeur table"), larg_t, gettext("Hauteur table"), haut_t, gettext("Largeur âme"), larg_a, gettext("Hauteur âme"), haut_a), NULL, gettext("Erreur d'allocation mémoire.\n"));
            
            return description;
        }
        case SECTION_CARREE :
        {
            char            cote[30];
            Section_Carree  *section = sect->data;
            
            common_math_double_to_char(section->cote, cote, DECIMAL_DISTANCE);
            BUGMSG(description = g_strdup_printf("%s : %s m", gettext("Coté"), cote), NULL, gettext("Erreur d'allocation mémoire.\n"));
            
            return description;
        }
        case SECTION_CIRCULAIRE :
        {
            char    diam[30];
            Section_Circulaire *section = sect->data;
            
            common_math_double_to_char(section->diametre, diam, DECIMAL_DISTANCE);
            BUGMSG(description = g_strdup_printf("%s : %s m", gettext("Diamètre"), diam), NULL, gettext("Erreur d'allocation mémoire.\n"));
            
            return description;
        }
        default :
        {
            BUGMSG(0, NULL, gettext("Type de section %d inconnu."), sect->type);
            break;
        }
    }
}


G_MODULE_EXPORT gboolean EF_sections_cherche_dependances(Projet *projet, EF_Section* section,
  GList** barres_dep)
/* Description : Liste l'ensemble des barres utilisant la section.
 * Paramètres : Projet *projet : la variable projet,
 *            : EF_Section *section : la section à analyser,
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
    BUGMSG(section, FALSE, gettext("Paramètre %s incorrect.\n"), "section");
    
    *barres_dep = NULL;
    
    list_parcours = projet->beton.barres;
    while (list_parcours != NULL)
    {
        Beton_Barre *barre = list_parcours->data;
        
        if (barre->section == section)
            *barres_dep = g_list_append(*barres_dep, barre);
        
        list_parcours = g_list_next(list_parcours);
    }
    
    return TRUE;
}


G_MODULE_EXPORT gboolean EF_sections_verifie_dependances(Projet *projet, EF_Section* section)
/* Description : Vérifie si la section est utilisée.
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
    BUGMSG(section, FALSE, gettext("Paramètre %s incorrect.\n"), "section");
    
    list_parcours = projet->beton.barres;
    while (list_parcours != NULL)
    {
        Beton_Barre *barre = list_parcours->data;
        
        if (barre->section == section)
            return TRUE;
        
        list_parcours = g_list_next(list_parcours);
    }
    
    return FALSE;
}


G_MODULE_EXPORT gboolean EF_sections_renomme(EF_Section *section, gchar *nom, Projet *projet)
/* Description : Renomme un appui.
 * Paramètres : EF_Section *section : section à renommer,
 *            : const char *nom : le nouveau nom,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL,
 *             section == NULL,
 *             appui possédant le nouveau nom est déjà existant.
 */
{
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(section, FALSE, gettext("Paramètre %s incorrect.\n"), "section");
    BUGMSG(EF_sections_cherche_nom(projet, nom, FALSE) == NULL, FALSE, gettext("La section '%s' existe déjà.\n"), nom);
    
    free(section->nom);
    BUGMSG(section->nom = g_strdup_printf("%s", nom), FALSE, gettext("Erreur d'allocation mémoire.\n"));
    
#ifdef ENABLE_GTK
    if (projet->list_gtk.ef_sections.builder != NULL)
        gtk_tree_store_set(projet->list_gtk.ef_sections.sections, &section->Iter_fenetre, 1, nom, -1);
    gtk_list_store_set(projet->list_gtk.ef_sections.liste_sections, &section->Iter_liste, 0, nom, -1);
#endif
    
    return TRUE;
}


G_MODULE_EXPORT gboolean EF_sections_supprime(EF_Section *section, gboolean annule_si_utilise,
  Projet *projet)
/* Description : Supprime la section spécifiée.
 * Paramètres : EF_Section *section : la section à supprimer,
 *            : gboolean annule_si_utilise : possibilité d'annuler la suppression si la section
 *              est attribuée à une barre. Si l'option est désactivée, les barres (et les
 *              barres et noeuds intermédiaires dépendants) utilisant la section seront
 *              supprimées.
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL,
 *             section == NULL.
 */
{
    GList   *list_barres;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(section, FALSE, gettext("Paramètre %s incorrect.\n"), "section");
    
    // On vérifie les dépendances.
    BUG(EF_sections_cherche_dependances(projet, section, &list_barres), FALSE);
    
    if ((annule_si_utilise) && (list_barres != NULL))
    {
        char *liste;
        
        liste = common_selection_converti_barres_en_texte(list_barres);
        if (g_list_next(list_barres) == NULL)
            printf("Impossible de supprimer la section car elle est utilisée par la barre %s.\n", liste);
        else
            printf("Impossible de supprimer la section car elle est utilisée par les barres %s.\n", liste);
        g_list_free(list_barres);
        free(liste);
        
        return TRUE;
    }
    
    BUG(_1992_1_1_barres_supprime_liste(projet, NULL, list_barres), TRUE);
    g_list_free(list_barres);
    
    free(section->nom);
    free(section->data);
    projet->beton.sections = g_list_remove(projet->beton.sections, section);
    
#ifdef ENABLE_GTK
    gtk_list_store_remove(projet->list_gtk.ef_sections.liste_sections, &section->Iter_liste);
    if (projet->list_gtk.ef_sections.builder != NULL)
        gtk_tree_store_remove(projet->list_gtk.ef_sections.sections, &section->Iter_fenetre);
#endif
    
    return TRUE;
}


G_MODULE_EXPORT double EF_sections_j(EF_Section* sect)
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
        {
            Section_T *section = sect->data;
            double      lt = section->largeur_table;
            double      la = section->largeur_ame;
            double      ht = section->hauteur_table;
            double      ha = section->hauteur_ame;
            double      a, b, aa, bb;
            
            if (lt > ht)
                { a = lt; b = ht; }
            else
                { a = ht; b = lt; }
            if (la > ha)
                { aa = la; bb = ha; }
            else
                { aa = ha; bb = la; }
            if (sect->type == SECTION_RECTANGULAIRE)
                return aa*bb*bb*bb/16.*(16./3.-3.364*bb/aa*(1.-bb*bb*bb*bb/(12.*aa*aa*aa*aa)));
            else
                return a*b*b*b/16.*(16./3.-3.364*b/a*(1.-b*b*b*b/(12.*a*a*a*a)))+aa*bb*bb*bb/16.*(16./3.-3.364*bb/aa*(1-bb*bb*bb*bb/(12.*aa*aa*aa*aa)));
            
    // Pour une section en T de section constante (lt : largeur de la table, la : largeur de
    //   l'âme, ht : hauteur de la table, ha : hauteur de l'âme), J vaut :\end{verbatim}\begin{displaymath}
    // J = \frac{a \cdot b^3}{16} \left[\frac{16}{3}-3.364 \frac{b}{a} \left(1-\frac{b^4}{12 a^4}\right)\right]+\frac{aa \cdot bb^3}{16} \left[\frac{16}{3}-3.364 \frac{bb}{aa} \left(1-\frac{bb^4}{12 aa^4}\right)\right]\texttt{ avec }\substack{a=max(h_t,l_t)\\b=min(h_t,l_t)\\aa=max(h_a,l_a)\\bb=min(h_a,l_a)}\end{displaymath}\begin{verbatim}
            break;
        }
        case SECTION_CARREE :
        {
            Section_Carree *section = sect->data;
            return section->cote*section->cote*section->cote*section->cote/16.*(16./3.-3.364*(1.-1./12.));
            
    // Pour une section carrée de section constante, J vaut :\end{verbatim}\begin{displaymath}
    // J = \frac{cote^4}{16} \cdot \left[\frac{16}{3}-3.364 \cdot \left(1-\frac{1}{12}\right)\right]\end{displaymath}\begin{verbatim}
            break;
        }
        case SECTION_CIRCULAIRE :
        {
            Section_Circulaire *section = sect->data;
            return M_PI*section->diametre*section->diametre*section->diametre*section->diametre/32.;
    // Pour une section circulaire de section constante, J vaut :\end{verbatim}\begin{displaymath}
    // J = \frac{\pi \cdot \phi^4}{32}\end{displaymath}\begin{verbatim}
            break;
        }
        default :
        {
            BUGMSG(0, NAN, gettext("Type de section %d inconnu."), sect->type);
            break;
        }
    }
}


G_MODULE_EXPORT double EF_sections_iy(EF_Section* sect)
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
        {
            Section_T *section = sect->data;
            double      lt = section->largeur_table;
            double      la = section->largeur_ame;
            double      ht = section->hauteur_table;
            double      ha = section->hauteur_ame;
    // Pour une section en T de section constante (lt : largeur de la table, la : largeur de
    //   l'âme, ht : hauteur de la table, ha : hauteur de l'âme), Iy vaut :\end{verbatim}\begin{displaymath}
    // I_y = \frac{l_t \cdot h_t^3}{12}+\frac{l_a \cdot h_a^3}{12}+l_t \cdot h_t \cdot \left(\frac{h_t}{2}-cdg_h \right)^2+l_a \cdot h_a \cdot \left(\frac{h_a}{2}-cdg_b \right)^2 \texttt{, }\end{displaymath}\begin{displaymath}
    // cdg_h = \frac{\frac{l_t \cdot h_t^2}{2}+l_a \cdot h_a \cdot \left(h_t+\frac{h_a}{2} \right)}{S}  \texttt{, } cdg_b = h_t+h_a-cdg_h \texttt{ et } S = l_t \cdot h_t+l_a \cdot h_a \end{displaymath}\begin{verbatim}
            double      S = lt*ht+la*ha;
            double      cdgh = (lt*ht*ht/2.+la*ha*(ht+ha/2.))/S;
            double      cdgb = (ht+ha)-cdgh;
            return lt*ht*ht*ht/12.+la*ha*ha*ha/12.+lt*ht*(ht/2.-cdgh)*(ht/2.-cdgh)+la*ha*(ha/2.-cdgb)*(ha/2.-cdgb);
            
            break;
        }
        case SECTION_CARREE :
        {
            Section_Carree *section = sect->data;
            return section->cote*section->cote*section->cote*section->cote/12.;
            
    // Pour une section carrée de section constante, Iy vaut :\end{verbatim}\begin{displaymath}
    // I_y = \frac{c^4}{12} \end{displaymath}\begin{verbatim}
            break;
        }
        case SECTION_CIRCULAIRE :
        {
            Section_Circulaire *section = sect->data;
            return M_PI*section->diametre*section->diametre*section->diametre*section->diametre/64.;
    // Pour une section circulaire de section constante, Iy vaut :\end{verbatim}\begin{displaymath}
    // I_y = \frac{\pi \cdot \phi^4}{64} \end{displaymath}\begin{verbatim}
            break;
        }
        default :
        {
            BUGMSG(0, NAN, gettext("Type de section %d inconnu."), sect->type);
            break;
        }
    }
}


G_MODULE_EXPORT double EF_sections_iz(EF_Section* sect)
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
        {
            Section_T *section = sect->data;
            double      lt = section->largeur_table;
            double      la = section->largeur_ame;
            double      ht = section->hauteur_table;
            double      ha = section->hauteur_ame;
    // Pour une section en T de section constante (lt : largeur de la table, la : largeur de
    //   l'âme, ht : hauteur de la table, ha : hauteur de l'âme), I vaut :\end{verbatim}\begin{displaymath}
    // I = \frac{h_t \cdot l_t^3}{12}+\frac{h_a \cdot l_a^3}{12}\end{displaymath}\begin{verbatim}
            return ht*lt*lt*lt/12.+ha*la*la*la/12.;
            break;
        }
        case SECTION_CARREE :
        {
            Section_Carree *section = sect->data;
            return section->cote*section->cote*section->cote*section->cote/12.;
    // Pour une section carrée de section constante, I vaut :\end{verbatim}\begin{displaymath}
    // I = \frac{c^4}{12} \end{displaymath}\begin{verbatim}
            break;
        }
        case SECTION_CIRCULAIRE :
        {
            Section_Circulaire *section = sect->data;
            return M_PI*section->diametre*section->diametre*section->diametre*section->diametre/64.;
    // Pour une section circulaire de section constante, I vaut :\end{verbatim}\begin{displaymath}
    // I = \frac{\pi \cdot \phi^4}{64} \end{displaymath}\begin{verbatim}
            break;
        }
        default :
        {
            BUGMSG(0, NAN, gettext("Type de section %d inconnu."), sect->type);
            break;
        }
    }
}


G_MODULE_EXPORT double EF_sections_vy(EF_Section* sect)
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
        {
            Section_T *section = sect->data;
            
            return MAX(section->largeur_table, section->largeur_ame)/2.;
            
            break;
        }
        case SECTION_CARREE :
        {
            Section_Carree *section = sect->data;
            
            return section->cote/2.;
            
            break;
        }
        case SECTION_CIRCULAIRE :
        {
            Section_Circulaire *section = sect->data;
            
            return section->diametre/2.;
            
            break;
        }
        default :
        {
            BUGMSG(0, NAN, gettext("Type de section %d inconnu."), sect->type);
            break;
        }
    }
}


G_MODULE_EXPORT double EF_sections_vyp(EF_Section* sect)
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
        {
            Section_T *section = sect->data;
            
            return MAX(section->largeur_table, section->largeur_ame)/2.;
            
            break;
        }
        case SECTION_CARREE :
        {
            Section_Carree *section = sect->data;
            
            return section->cote/2.;
            
            break;
        }
        case SECTION_CIRCULAIRE :
        {
            Section_Circulaire *section = sect->data;
            
            return section->diametre/2.;
            
            break;
        }
        default :
        {
            BUGMSG(0, NAN, gettext("Type de section %d inconnu."), sect->type);
            break;
        }
    }
}


G_MODULE_EXPORT double EF_sections_vz(EF_Section* sect)
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
        {
            Section_T *section = sect->data;
            
            return (section->largeur_table*section->hauteur_table*section->hauteur_table/2.+section->largeur_ame*section->hauteur_ame*(section->hauteur_ame/2.+section->hauteur_table))/EF_sections_s(sect);
            
            break;
        }
        case SECTION_CARREE :
        {
            Section_Carree *section = sect->data;
            
            return section->cote/2.;
            
            break;
        }
        case SECTION_CIRCULAIRE :
        {
            Section_Circulaire *section = sect->data;
            
            return section->diametre/2.;
            
            break;
        }
        default :
        {
            BUGMSG(0, NAN, gettext("Type de section %d inconnu."), sect->type);
            break;
        }
    }
}


G_MODULE_EXPORT double EF_sections_vzp(EF_Section* sect)
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
        {
            Section_T *section = sect->data;
            
            return (section->largeur_table*section->hauteur_table*(section->hauteur_ame+section->hauteur_table/2.)+section->largeur_ame*section->hauteur_ame*(section->hauteur_ame/2.))/EF_sections_s(sect);
            
            break;
        }
        case SECTION_CARREE :
        {
            Section_Carree *section = sect->data;
            
            return section->cote/2.;
            
            break;
        }
        case SECTION_CIRCULAIRE :
        {
            Section_Circulaire *section = sect->data;
            
            return section->diametre/2.;
            
            break;
        }
        default :
        {
            BUGMSG(0, NAN, gettext("Type de section %d inconnu."), sect->type);
            break;
        }
    }
}


G_MODULE_EXPORT double EF_sections_ay(Beton_Barre *barre, unsigned int discretisation)
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
    
    E = barre->materiau->ecm;
    
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
            BUGMSG(0, NAN, gettext("Type de section %d inconnu."), barre->section->type);
            break;
        }
    }
}


G_MODULE_EXPORT double EF_sections_by(Beton_Barre *barre, unsigned int discretisation)
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
    
    E = barre->materiau->ecm;
    
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
            BUGMSG(0, NAN, gettext("Type de section %d inconnu."), barre->section->type);
            break;
        }
    }
}


G_MODULE_EXPORT double EF_sections_cy(Beton_Barre *barre, unsigned int discretisation)
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
    
    E = barre->materiau->ecm;
    
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
            BUGMSG(0, NAN, gettext("Type de section %d inconnu."), barre->section->type);
            break;
        }
    }
}


G_MODULE_EXPORT double EF_sections_az(Beton_Barre *barre, unsigned int discretisation)
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
    
    E = barre->materiau->ecm;
    
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
            BUGMSG(0, NAN, gettext("Type de section %d inconnu."), barre->section->type);
            break;
        }
    }
}


G_MODULE_EXPORT double EF_sections_bz(Beton_Barre *barre, unsigned int discretisation)
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
    
    E = barre->materiau->ecm;
    
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
            BUGMSG(0, NAN, gettext("Type de section %d inconnu."), barre->section->type);
            break;
        }
    }
}


G_MODULE_EXPORT double EF_sections_cz(Beton_Barre *barre, unsigned int discretisation)
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
    
    E = barre->materiau->ecm;
    
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
            BUGMSG(0, NAN, gettext("Type de section %d inconnu."), barre->section->type);
            break;
        }
    }
}


G_MODULE_EXPORT double EF_sections_s(EF_Section *sect)
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
        {
            Section_T *section = sect->data;
            return section->hauteur_table*section->largeur_table+section->hauteur_ame*section->largeur_ame;
            
    // Pour une section en T de section constante (lt : largeur de la table, la : largeur de
    //   l'âme, ht : hauteur de la table, ha : hauteur de l'âme), S vaut :\end{verbatim}\begin{displaymath}
    // S = h_t \cdot l_t+h_a \cdot l_a\end{displaymath}\begin{verbatim}
            break;
        }
        case SECTION_CARREE :
        {
            Section_Carree *section = sect->data;
            return section->cote*section->cote;
            
    // Pour une section carrée de section constante, S vaut :\end{verbatim}\begin{displaymath}
    // S = c^2\end{displaymath}\begin{verbatim}
            break;
        }
        case SECTION_CIRCULAIRE :
        {
            Section_Circulaire *section = sect->data;
            return M_PI*section->diametre*section->diametre/4.;
    // Pour une section circulaire de section constante, S vaut :\end{verbatim}\begin{displaymath}
    // S = \frac{\pi \cdot \phi^2}{4} \end{displaymath}\begin{verbatim}
            break;
        }
        default :
        {
            BUGMSG(0, NAN, gettext("Type de section %d inconnu."), sect->type);
            break;
        }
    }
}


G_MODULE_EXPORT double EF_sections_es_l(Beton_Barre *barre, unsigned int discretisation,
  double d, double f)
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
    double      E;
    
    BUGMSG(barre, NAN, gettext("Paramètre %s incorrect.\n"), "barre");
    BUGMSG(discretisation<=barre->discretisation_element, NAN, gettext("La discrétisation %d souhaitée est hors domaine %d.\n"), discretisation, barre->discretisation_element);
    BUGMSG(!((d>f) && (!(ERREUR_RELATIVE_EGALE(d, f)))), NAN, gettext("\n"));
    
    // Le facteur ES/L est défini par la formule :\end{verbatim}\begin{displaymath}
    // \frac{E \cdot S}{L} = \frac{E}{\int_d^f \frac{1}{S(x)} dx}\end{displaymath}\begin{verbatim}
    
    E = barre->materiau->ecm;
    
    switch (barre->section->type)
    {
        case SECTION_RECTANGULAIRE :
        case SECTION_T :
        {
            Section_T *section = barre->section->data;
            double      lt = section->largeur_table;
            double      la = section->largeur_ame;
            double      ht = section->hauteur_table;
            double      ha = section->hauteur_ame;
            double      S = ht*lt+ha*la;
            
    // Pour une section en T de section constante (lt : largeur de la table, la : largeur de
    //   l'âme, ht : hauteur de la table, ha : hauteur de l'âme), ES/L vaut :\end{verbatim}\begin{displaymath}
    // \frac{E \cdot S}{L} = \frac{E \cdot (h_t \cdot l_t+h_a \cdot l_a)}{L}\end{displaymath}\begin{verbatim}
            return E*S/(f-d);
            break;
        }
        case SECTION_CARREE :
        {
            Section_Carree *section = barre->section->data;
            double      S = section->cote*section->cote;
            
    // Pour une section carrée de section constante, ES/L vaut :\end{verbatim}\begin{displaymath}
    // \frac{E \cdot S}{L} = \frac{E \cdot c^2}{L}\end{displaymath}\begin{verbatim}
            return E*S/(f-d);
            break;
        }
        case SECTION_CIRCULAIRE :
        {
            Section_Circulaire *section = barre->section->data;
            double      S = M_PI*section->diametre*section->diametre/4.;
    // Pour une section circulaire de section constante, ES/L vaut :\end{verbatim}\begin{displaymath}
    // \frac{E \cdot S}{L} = \frac{E \cdot \pi \cdot \phi^2}{4 \cdot L} \end{displaymath}\begin{verbatim}
            return E*S/(f-d);
            break;
        }
        default :
        {
            BUGMSG(0, NAN, gettext("Type de section %d inconnu."), barre->section->type);
            break;
        }
    }
}


G_MODULE_EXPORT double EF_sections_gj_l(Beton_Barre *barre, unsigned int discretisation)
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
    double      G;
    
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
    
    G = barre->materiau->gnu_0_2;
    
    switch (barre->section->type)
    {
        case SECTION_RECTANGULAIRE :
        case SECTION_T :
        {
            Section_T *section = barre->section->data;
            double      lt = section->largeur_table;
            double      la = section->largeur_ame;
            double      ht = section->hauteur_table;
            double      ha = section->hauteur_ame;
            double      a, b, aa, bb;
            double      J;
            
            if (lt > ht)
                { a = lt; b = ht; }
            else
                { a = ht; b = lt; }
            if (la > ha)
                { aa = la; bb = ha; }
            else
                { aa = ha; bb = la; }
            if (barre->section->type == SECTION_RECTANGULAIRE)
                J = aa*bb*bb*bb/16.*(16./3.-3.364*bb/aa*(1-bb*bb*bb*bb/(12.*aa*aa*aa*aa)));
            else
                J = a*b*b*b/16.*(16./3.-3.364*b/a*(1.-b*b*b*b/(12.*a*a*a*a)))+aa*bb*bb*bb/16.*(16./3.-3.364*bb/aa*(1-bb*bb*bb*bb/(12.*aa*aa*aa*aa)));
            
    // Pour une section en T de section constante (lt : largeur de la table, la : largeur de
    //   l'âme, ht : hauteur de la table, ha : hauteur de l'âme), GJ/L vaut :\end{verbatim}\begin{displaymath}
    // \frac{G \cdot J}{L} \texttt{ avec } J = \frac{a \cdot b^3}{16} \left[\frac{16}{3}-3.364 \frac{b}{a} \left(1-\frac{b^4}{12 a^4}\right)\right]+\frac{aa \cdot bb^3}{16} \left[\frac{16}{3}-3.364 \frac{bb}{aa} \left(1-\frac{bb^4}{12 aa^4}\right)\right]\texttt{ avec }\substack{a=max(h_t,l_t)\\b=min(h_t,l_t)\\aa=max(h_a,l_a)\\bb=min(h_a,l_a)}\end{displaymath}\begin{verbatim}
            return G*J/ll;
            break;
        }
        case SECTION_CARREE :
        {
            Section_Carree *section = barre->section->data;
            double      J = section->cote*section->cote*section->cote*section->cote/16.*(16./3.-3.364*(1.-1./12.));
            
    // Pour une section carrée de section constante, GJ/L vaut :\end{verbatim}\begin{displaymath}
    // \frac{G \cdot J}{L} \texttt{ avec } J = \frac{cote^4}{16} \cdot \left[\frac{16}{3}-3.364 \cdot \left(1-\frac{1}{12}\right)\right]\end{displaymath}\begin{verbatim}
            return G*J/ll;
            break;
        }
        case SECTION_CIRCULAIRE :
        {
            Section_Circulaire *section = barre->section->data;
            double      J = M_PI*section->diametre*section->diametre*section->diametre*section->diametre/32.;
    // Pour une section circulaire de section constante, GJ/L vaut :\end{verbatim}\begin{displaymath}
    // \frac{G \cdot J}{L} \texttt{ avec } J = \frac{\pi \cdot \phi^4}{32}\end{displaymath}\begin{verbatim}
            return G*J/ll;
            break;
        }
        default :
        {
            BUGMSG(0, NAN, gettext("Type de section %d inconnu."), barre->section->type);
            break;
        }
    }
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


G_MODULE_EXPORT gboolean EF_sections_free(Projet *projet)
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
    if (projet->beton.sections != NULL)
    {
        g_list_free_full(projet->beton.sections, (GDestroyNotify)&EF_sections_free_un);
        projet->beton.sections = NULL;
    }
    
    BUG(EF_calculs_free(projet), TRUE);
    
#ifdef ENABLE_GTK
    g_object_unref(projet->list_gtk.ef_sections.liste_sections);
#endif
    
    return TRUE;
}
