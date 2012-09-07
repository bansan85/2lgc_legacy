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
#include "EF_noeud.h"
#include "EF_calculs.h"

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
    projet->list_gtk.ef_barres.liste_sections = gtk_list_store_new(1, G_TYPE_STRING);
#endif
    
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
    Beton_Section_Rectangulaire *section_data = malloc(sizeof(Beton_Section_Rectangulaire));
    EF_Section *section_nouvelle = malloc(sizeof(EF_Section));
#ifdef ENABLE_GTK
    GtkTreeIter     iter;
#endif
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(section_nouvelle, FALSE, gettext("Erreur d'allocation mémoire.\n"));
    BUGMSG(section_data, FALSE, gettext("Erreur d'allocation mémoire.\n"));
    section_nouvelle->data = section_data;
    
    // Trivial
    section_nouvelle->type = SECTION_RECTANGULAIRE;
    BUGMSG(section_nouvelle->nom = g_strdup_printf("%s", nom), FALSE, gettext("Erreur d'allocation mémoire.\n"));
    section_data->largeur = l;
    section_data->hauteur = h;
    
    projet->beton.sections = g_list_append(projet->beton.sections, section_nouvelle);
    
#ifdef ENABLE_GTK
    gtk_list_store_append(projet->list_gtk.ef_barres.liste_sections, &iter);
    gtk_list_store_set(projet->list_gtk.ef_barres.liste_sections, &iter, 0, nom, -1);
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
    Beton_Section_T *section_data = malloc(sizeof(Beton_Section_T));
    EF_Section      *section_nouvelle = malloc(sizeof(EF_Section));
    
#ifdef ENABLE_GTK
    GtkTreeIter     iter;
#endif
    
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
    gtk_list_store_append(projet->list_gtk.ef_barres.liste_sections, &iter);
    gtk_list_store_set(projet->list_gtk.ef_barres.liste_sections, &iter, 0, nom, -1);
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
    Beton_Section_Carre *section_data = malloc(sizeof(Beton_Section_Carre));
    EF_Section          *section_nouvelle = malloc(sizeof(EF_Section));
    
#ifdef ENABLE_GTK
    GtkTreeIter     iter;
#endif
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG(section_nouvelle, FALSE, gettext("Erreur d'allocation mémoire.\n"));
    BUGMSG(section_data, FALSE, gettext("Erreur d'allocation mémoire.\n"));
    section_nouvelle->data = section_data;
    
    // Les caractéristiques de la section sont les suivantes :\end{verbatim}\begin{displaymath}
    //   S = cote^2\texttt{  et  }cdg_{haut} = \frac{cote}{2}\texttt{  et  }cdg_{bas} = \frac{cote}{2}\texttt{  et  }cdg_{droite} = \frac{cote}{2}\texttt{  et  }cdg_{gauche} = \frac{cote}{2}\end{displaymath}\begin{displaymath}
    //   I_y = \frac{cote^4}{12}\texttt{  et  }I_z = I_y\texttt{  et  }J = \frac{cote^4}{16} \cdot \left[\frac{16}{3}-3.364 \cdot \left(1-\frac{1}{12}\right)\right]\end{displaymath}\begin{verbatim}
    section_nouvelle->type = SECTION_CARRE;
    BUGMSG(section_nouvelle->nom = g_strdup_printf("%s", nom), FALSE, gettext("Erreur d'allocation mémoire.\n"));
    section_data->cote = cote;
    
    projet->beton.sections = g_list_append(projet->beton.sections, section_nouvelle);
    
#ifdef ENABLE_GTK
    gtk_list_store_append(projet->list_gtk.ef_barres.liste_sections, &iter);
    gtk_list_store_set(projet->list_gtk.ef_barres.liste_sections, &iter, 0, nom, -1);
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
    Beton_Section_Circulaire    *section_data = malloc(sizeof(Beton_Section_Circulaire));
    EF_Section                  *section_nouvelle = malloc(sizeof(EF_Section));
    
#ifdef ENABLE_GTK
    GtkTreeIter     iter;
#endif
    
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
    gtk_list_store_append(projet->list_gtk.ef_barres.liste_sections, &iter);
    gtk_list_store_set(projet->list_gtk.ef_barres.liste_sections, &iter, 0, nom, -1);
#endif
    
    return TRUE;
}


G_MODULE_EXPORT EF_Section* EF_sections_cherche_nom(Projet *projet, const char *nom)
/* Description : Positionne dans la liste des sections en béton l'élément courant au numéro
 *               souhaité.
 * Paramètres : Projet *projet : la variable projet,
 *            : const char *nom : le nom de la section.
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
    
    BUGMSG(0, NULL, gettext("Section en béton '%s' introuvable.\n"), nom);
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
        {
            Beton_Section_Rectangulaire *section = sect->data;
            double      l = section->largeur;
            double      h = section->hauteur;
            double      a, b;
            
            if (l > h)
                { a = l; b = h; }
            else
                { a = h; b = l; }
            return a*b*b*b/16.*(16./3.-3.364*b/a*(1.-b*b*b*b/(12.*a*a*a*a)));
            
    // Pour une section rectantulaire de section constante, J vaut :\end{verbatim}\begin{displaymath}
    // J = \frac{a \cdot b^3}{16} \cdot \left[\frac{16}{3}-3.364 \cdot \frac{b}{a} \cdot \left( 1-\frac{b^4}{12 \cdot a^4} \right) \right]\texttt{ avec }\substack{a=max(h,l)\\b=min(h,l)} \end{displaymath}\begin{verbatim}
            break;
        }
        case SECTION_T :
        {
            Beton_Section_T *section = sect->data;
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
            return a*b*b*b/16.*(16./3.-3.364*b/a*(1.-b*b*b*b/(12.*a*a*a*a)))+aa*bb*bb*bb/16.*(16./3.-3.364*bb/aa*(1-bb*bb*bb*bb/(12.*aa*aa*aa*aa)));
            
    // Pour une section en T de section constante (lt : largeur de la table, la : largeur de
    //   l'âme, ht : hauteur de la table, ha : hauteur de l'âme), J vaut :\end{verbatim}\begin{displaymath}
    // J = \frac{a \cdot b^3}{16} \left[\frac{16}{3}-3.364 \frac{b}{a} \left(1-\frac{b^4}{12 a^4}\right)\right]+\frac{aa \cdot bb^3}{16} \left[\frac{16}{3}-3.364 \frac{bb}{aa} \left(1-\frac{bb^4}{12 aa^4}\right)\right]\texttt{ avec }\substack{a=max(h_t,l_t)\\b=min(h_t,l_t)\\aa=max(h_a,l_a)\\bb=min(h_a,l_a)}\end{displaymath}\begin{verbatim}
            break;
        }
        case SECTION_CARRE :
        {
            Beton_Section_Carre *section = sect->data;
            return section->cote*section->cote*section->cote*section->cote/16.*(16./3.-3.364*(1.-1./12.));
            
    // Pour une section carrée de section constante, J vaut :\end{verbatim}\begin{displaymath}
    // J = \frac{cote^4}{16} \cdot \left[\frac{16}{3}-3.364 \cdot \left(1-\frac{1}{12}\right)\right]\end{displaymath}\begin{verbatim}
            break;
        }
        case SECTION_CIRCULAIRE :
        {
            Beton_Section_Circulaire *section = sect->data;
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
        {
            Beton_Section_Rectangulaire *section = sect->data;
            return section->largeur*section->hauteur*section->hauteur*section->hauteur/12.;
    // Pour une section rectantulaire de section constante, Iy vaut :\end{verbatim}\begin{displaymath}
    // I_y = \frac{l \cdot h^3}{12} \end{displaymath}\begin{verbatim}
            break;
        }
        case SECTION_T :
        {
            Beton_Section_T *section = sect->data;
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
        case SECTION_CARRE :
        {
            Beton_Section_Carre *section = sect->data;
            return section->cote*section->cote*section->cote*section->cote/12.;
            
    // Pour une section carrée de section constante, Iy vaut :\end{verbatim}\begin{displaymath}
    // I_y = \frac{c^4}{12} \end{displaymath}\begin{verbatim}
            break;
        }
        case SECTION_CIRCULAIRE :
        {
            Beton_Section_Circulaire *section = sect->data;
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
        {
            Beton_Section_Rectangulaire *section = sect->data;
            return section->hauteur*section->largeur*section->largeur*section->largeur/12.;
            
    // Pour une section rectantulaire de section constante, I vaut :\end{verbatim}\begin{displaymath}
    // I = \frac{h \cdot l^3}{12} \end{displaymath}\begin{verbatim}
            break;
        }
        case SECTION_T :
        {
            Beton_Section_T *section = sect->data;
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
        case SECTION_CARRE :
        {
            Beton_Section_Carre *section = sect->data;
            return section->cote*section->cote*section->cote*section->cote/12.;
    // Pour une section carrée de section constante, I vaut :\end{verbatim}\begin{displaymath}
    // I = \frac{c^4}{12} \end{displaymath}\begin{verbatim}
            break;
        }
        case SECTION_CIRCULAIRE :
        {
            Beton_Section_Circulaire *section = sect->data;
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
        case SECTION_CARRE :
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
        case SECTION_CARRE :
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
        case SECTION_CARRE :
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
        case SECTION_CARRE :
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
        case SECTION_CARRE :
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
        case SECTION_CARRE :
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
        {
            Beton_Section_Rectangulaire *section = sect->data;
            return section->hauteur*section->largeur;
            
    // Pour une section rectantulaire de section constante, S vaut :\end{verbatim}\begin{displaymath}
    // S = h \cdot l\end{displaymath}\begin{verbatim}
            break;
        }
        case SECTION_T :
        {
            Beton_Section_T *section = sect->data;
            return section->hauteur_table*section->largeur_table+section->hauteur_ame*section->largeur_ame;
            
    // Pour une section en T de section constante (lt : largeur de la table, la : largeur de
    //   l'âme, ht : hauteur de la table, ha : hauteur de l'âme), S vaut :\end{verbatim}\begin{displaymath}
    // S = h_t \cdot l_t+h_a \cdot l_a\end{displaymath}\begin{verbatim}
            break;
        }
        case SECTION_CARRE :
        {
            Beton_Section_Carre *section = sect->data;
            return section->cote*section->cote;
            
    // Pour une section carrée de section constante, S vaut :\end{verbatim}\begin{displaymath}
    // S = c^2\end{displaymath}\begin{verbatim}
            break;
        }
        case SECTION_CIRCULAIRE :
        {
            Beton_Section_Circulaire *section = sect->data;
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
        {
            Beton_Section_Rectangulaire *section = barre->section->data;
            double      S = section->hauteur*section->largeur;
            
    // Pour une section rectantulaire de section constante, ES/L vaut :\end{verbatim}\begin{displaymath}
    // \frac{E \cdot S}{L} = \frac{E \cdot h \cdot l}{L} \end{displaymath}\begin{verbatim}
            return E*S/(f-d);
            break;
        }
        case SECTION_T :
        {
            Beton_Section_T *section = barre->section->data;
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
        case SECTION_CARRE :
        {
            Beton_Section_Carre *section = barre->section->data;
            double      S = section->cote*section->cote;
            
    // Pour une section carrée de section constante, ES/L vaut :\end{verbatim}\begin{displaymath}
    // \frac{E \cdot S}{L} = \frac{E \cdot c^2}{L}\end{displaymath}\begin{verbatim}
            return E*S/(f-d);
            break;
        }
        case SECTION_CIRCULAIRE :
        {
            Beton_Section_Circulaire *section = barre->section->data;
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
        {
            Beton_Section_Rectangulaire *section = barre->section->data;
            double      l = section->largeur;
            double      h = section->hauteur;
            double      J;
            double      a, b;
            
            if (l > h)
                { a = l; b = h; }
            else
                { a = h; b = l; }
            J = a*b*b*b/16.*(16./3.-3.364*b/a*(1.-b*b*b*b/(12.*a*a*a*a)));
            
    // Pour une section rectantulaire de section constante, GJ/L vaut :\end{verbatim}\begin{displaymath}
    // \frac{G \cdot J}{L} \texttt{ avec } J = \frac{a \cdot b^3}{16} \cdot \left[\frac{16}{3}-3.364 \cdot \frac{b}{a} \cdot \left( 1-\frac{b^4}{12 \cdot a^4} \right) \right]\texttt{ avec }\substack{a=max(h,l)\\b=min(h,l)} \end{displaymath}\begin{verbatim}
            return G*J/ll;
            break;
        }
        case SECTION_T :
        {
            Beton_Section_T *section = barre->section->data;
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
            J = a*b*b*b/16.*(16./3.-3.364*b/a*(1.-b*b*b*b/(12.*a*a*a*a)))+aa*bb*bb*bb/16.*(16./3.-3.364*bb/aa*(1-bb*bb*bb*bb/(12.*aa*aa*aa*aa)));
            
    // Pour une section en T de section constante (lt : largeur de la table, la : largeur de
    //   l'âme, ht : hauteur de la table, ha : hauteur de l'âme), GJ/L vaut :\end{verbatim}\begin{displaymath}
    // \frac{G \cdot J}{L} \texttt{ avec } J = \frac{a \cdot b^3}{16} \left[\frac{16}{3}-3.364 \frac{b}{a} \left(1-\frac{b^4}{12 a^4}\right)\right]+\frac{aa \cdot bb^3}{16} \left[\frac{16}{3}-3.364 \frac{bb}{aa} \left(1-\frac{bb^4}{12 aa^4}\right)\right]\texttt{ avec }\substack{a=max(h_t,l_t)\\b=min(h_t,l_t)\\aa=max(h_a,l_a)\\bb=min(h_a,l_a)}\end{displaymath}\begin{verbatim}
            return G*J/ll;
            break;
        }
        case SECTION_CARRE :
        {
            Beton_Section_Carre *section = barre->section->data;
            double      J = section->cote*section->cote*section->cote*section->cote/16.*(16./3.-3.364*(1.-1./12.));
            
    // Pour une section carrée de section constante, GJ/L vaut :\end{verbatim}\begin{displaymath}
    // \frac{G \cdot J}{L} \texttt{ avec } J = \frac{cote^4}{16} \cdot \left[\frac{16}{3}-3.364 \cdot \left(1-\frac{1}{12}\right)\right]\end{displaymath}\begin{verbatim}
            return G*J/ll;
            break;
        }
        case SECTION_CIRCULAIRE :
        {
            Beton_Section_Circulaire *section = barre->section->data;
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
    g_object_unref(projet->list_gtk.ef_barres.liste_sections);
#endif
    
    return TRUE;
}
