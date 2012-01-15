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
#include "common_projet.h"
#include "common_erreurs.h"
#include "common_maths.h"

int _1992_1_1_sections_init(Projet *projet)
/* Description : Initialise la liste des section en béton
 * Paramètres : Projet *projet : la variable projet
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet == NULL)
 *           -2 en cas d'erreur d'allocation mémoire
 */
{
    BUGMSG(projet, -1, "_1992_1_1_sections_init\n");
    
    // Trivial
    projet->beton.sections = list_init();
    BUGMSG(projet->beton.sections, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "_1992_1_1_sections_init");
    
    return 0;
}

int _1992_1_1_sections_ajout_rectangulaire(Projet *projet, double l, double h)
/* Description : ajouter une nouvelle section rectangulaire à la liste des sections en béton
 * Paramètres : Projet *projet : la variable projet
 *            : double l : la largeur
 *            : double h : la hauteur
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet == NULL) ou
 *             (projet->beton.sections == NULL)
 *           -2 en cas d'erreur d'allocation mémoire
 */
{
    Beton_Section_Rectangulaire     *section_en_cours;
    Beton_Section_Rectangulaire     section_nouvelle;
    
    BUGMSG(projet, -1, "_1992_1_1_sections_ajout_rectangulaire\n");
    BUGMSG(projet->beton.sections, -1, "_1992_1_1_sections_ajout_rectangulaire\n");
    
    list_mvrear(projet->beton.sections);
    
    // Trivial
    section_nouvelle.type = BETON_SECTION_RECTANGULAIRE;
    section_nouvelle.largeur = l;
    section_nouvelle.hauteur = h;
    
    section_en_cours = (Beton_Section_Rectangulaire*)list_rear(projet->beton.sections);
    if (section_en_cours == NULL)
        section_nouvelle.numero = 0;
    else
        section_nouvelle.numero = section_en_cours->numero+1;
    
    if (list_insert_after(projet->beton.sections, &(section_nouvelle), sizeof(section_nouvelle)) == NULL)
        BUGMSG(0, -3, gettext("Erreur d'allocation mémoire.\n"));
    
    return 0;
}

int _1992_1_1_sections_ajout_T(Projet *projet, double lt, double la, double ht, double ha)
/* Description : ajouter une nouvelle section en T à la liste des sections en béton
 * Paramètres : Projet *projet : la variable projet
 *            : double lt : la largeur de la table
 *            : double la : la largeur de l'âme
 *            : double ht : la hauteur de la table
 *            : double ha : la hauteur de l'âme
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet == NULL) ou
 *             (projet->beton.sections == NULL)
 *           -2 en cas d'erreur d'allocation mémoire
 */
{
    Beton_Section_T     *section_en_cours;
    Beton_Section_T     section_nouvelle;
    
    BUGMSG(projet, -1, "_1992_1_1_sections_ajout_T\n");
    BUGMSG(projet->beton.sections, -1, "_1992_1_1_sections_ajout_T\n");
    
    list_mvrear(projet->beton.sections);
    
    // Les caractéristiques de la section sont les suivantes :\end{verbatim}\begin{displaymath}
    //   S = lt \cdot ht+la \cdot ha\end{displaymath}\begin{displaymath}
    //   cdg_{haut} = \frac{\frac{lt \cdot ht^2}{2}+la \cdot ha \cdot (ht+\frac{ha}{2})}{S}\texttt{  et  }cdg_{bas} = (ht+ha)-cdg_{haut}\texttt{  et  }cdg_{droite} = \frac{lt}{2}\texttt{  et  }cdg_{gauche} = \frac{lt}{2}\end{displaymath}\begin{displaymath}
    //   I_y = \frac{lt \cdot ht^3}{12}+\frac{la \cdot ha^3}{12}+lt \cdot ht \cdot \left(\frac{ht}{2}-cdg_{haut} \right)^2+la \cdot ha \cdot \left(\frac{ha}{2}-cdg_{bas} \right)^2\texttt{  et  }I_z = \frac{ht \cdot lt^3}{12}+\frac{ha \cdot la^3}{12}\end{displaymath}\begin{displaymath}
    //   J = \frac{a \cdot b^3}{16} \cdot \left[\frac{16}{3}-3.364 \cdot \frac{b}{a} \cdot \left(1-\frac{b^4}{12 \cdot a^4}\right)\right]+\frac{aa \cdot bb^3}{16} \cdot \left[\frac{16}{3}-3.364 \cdot \frac{bb}{aa} \cdot \left(1-\frac{bb^4}{12 \cdot aa^4}\right)\right]\texttt{ avec }\substack{a=max(ht,lt)\\b=min(ht,lt)\\aa=max(ha,la)\\bb=min(ha,la)}
    //   \end{displaymath}\begin{verbatim}
    section_nouvelle.type = BETON_SECTION_T;
    section_nouvelle.largeur_table = lt;
    section_nouvelle.largeur_ame = la;
    section_nouvelle.hauteur_table = ht;
    section_nouvelle.hauteur_ame = ha;
    
    section_en_cours = (Beton_Section_T*)list_rear(projet->beton.sections);
    if (section_en_cours == NULL)
        section_nouvelle.numero = 0;
    else
        section_nouvelle.numero = section_en_cours->numero+1;
    
    BUGMSG(list_insert_after(projet->beton.sections, &(section_nouvelle), sizeof(section_nouvelle)), -2, gettext("%s : Erreur d'allocation mémoire.\n"), "_1992_1_1_sections_ajout_T");
    
    return 0;
}

int _1992_1_1_sections_ajout_carre(Projet *projet, double cote)
/* Description : ajouter une nouvelle section carrée à la liste des sections en béton
 * Paramètres : Projet *projet : la variable projet
 *            : double cote : le coté
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet == NULL) ou
 *             (projet->beton.sections == NULL)
 *           -2 en cas d'erreur d'allocation mémoire
 */
{
    Beton_Section_Carre     *section_en_cours;
    Beton_Section_Carre     section_nouvelle;
    
    BUGMSG(projet, -1, "_1992_1_1_sections_ajout_carre\n");
    BUGMSG(projet->beton.sections, -1, "_1992_1_1_sections_ajout_carre\n");
    
    list_mvrear(projet->beton.sections);
    
    // Les caractéristiques de la section sont les suivantes :\end{verbatim}\begin{displaymath}
    //   S = cote^2\texttt{  et  }cdg_{haut} = \frac{cote}{2}\texttt{  et  }cdg_{bas} = \frac{cote}{2}\texttt{  et  }cdg_{droite} = \frac{cote}{2}\texttt{  et  }cdg_{gauche} = \frac{cote}{2}\end{displaymath}\begin{displaymath}
    //   I_y = \frac{cote^4}{12}\texttt{  et  }I_z = I_y\texttt{  et  }J = \frac{cote^4}{16} \cdot \left[\frac{16}{3}-3.364 \cdot \left(1-\frac{1}{12}\right)\right]\end{displaymath}\begin{verbatim}
    section_nouvelle.type = BETON_SECTION_CARRE;
    section_nouvelle.cote = cote;
    
    section_en_cours = (Beton_Section_Carre*)list_rear(projet->beton.sections);
    if (section_en_cours == NULL)
        section_nouvelle.numero = 0;
    else
        section_nouvelle.numero = section_en_cours->numero+1;
    
    BUGMSG(list_insert_after(projet->beton.sections, &(section_nouvelle), sizeof(section_nouvelle)), -2, gettext("%s : Erreur d'allocation mémoire.\n"), "_1992_1_1_sections_ajout_carre");
    
    return 0;
}

int _1992_1_1_sections_ajout_circulaire(Projet *projet, double diametre)
/* Description : ajouter une nouvelle section circulaire à la liste des sections en béton
 * Paramètres : Projet *projet : la variable projet
 *            : double diametre : le diamètre
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet == NULL) ou
 *             (projet->beton.sections == NULL)
 *           -2 en cas d'erreur d'allocation mémoire
 */
{
    Beton_Section_Circulaire    *section_en_cours;
    Beton_Section_Circulaire    section_nouvelle;
    
    BUGMSG(projet, -1, "_1992_1_1_sections_ajout_circulaire\n");
    BUGMSG(projet->beton.sections, -1, "_1992_1_1_sections_ajout_circulaire\n");
    
    list_mvrear(projet->beton.sections);
    
    // Les caractéristiques de la section sont les suivantes :\end{verbatim}\begin{displaymath}
    //   S = \frac{\pi \cdot diametre^2}{4}\end{displaymath}\begin{displaymath}
    //   cdg_{haut} = \frac{diametre}{2}\texttt{  et  }cdg_{bas} = \frac{diametre}{2}\texttt{  et  }cdg_{droite} = \frac{diametre}{2}\texttt{  et  }cdg_{gauche} = \frac{diametre}{2}\end{displaymath}\begin{displaymath}
    //   I_y = \frac{\pi \cdot diametre^4}{64}\texttt{  et  }I_z = I_y\texttt{  et  }J = \frac{\pi \cdot diametre^4}{32}\end{displaymath}\begin{verbatim}
    section_nouvelle.type = BETON_SECTION_CIRCULAIRE;
    section_nouvelle.diametre = diametre;
    
    section_en_cours = (Beton_Section_Circulaire*)list_rear(projet->beton.sections);
    if (section_en_cours == NULL)
        section_nouvelle.numero = 0;
    else
        section_nouvelle.numero = section_en_cours->numero+1;
    
    BUGMSG(list_insert_after(projet->beton.sections, &(section_nouvelle), sizeof(section_nouvelle)), -2, gettext("%s : Erreur d'allocation mémoire.\n"), "_1992_1_1_sections_ajout_circulaire");
    
    return 0;
}


void* _1992_1_1_sections_cherche_numero(Projet *projet, unsigned int numero)
/* Description : Positionne dans la liste des sections en béton l'élément courant au numéro
 *                 souhaité.
 * Paramètres : Projet *projet : la variable projet
 *            : unsigned int numero : le numéro de la section
 * Valeur renvoyée :
 *   Succès : pointeur vers la section
 *   Échec : NULL en cas de paramètres invalides :
 *             (projet == NULL) ou
 *             (projet->beton.sections == NULL) ou
 *             (list_size(projet->beton.sections) == 0) ou
 *             numéro introuvable.
 */
{
    BUGMSG(projet, NULL, "_1992_1_1_sections_cherche_numero\n");
    BUGMSG(projet->beton.sections, NULL, "_1992_1_1_sections_cherche_numero\n");
    BUGMSG(list_size(projet->beton.sections), NULL, "_1992_1_1_sections_cherche_numero\n");
    
    // Trivial
    list_mvfront(projet->beton.sections);
    do
    {
        Beton_Section_Circulaire    *section = (Beton_Section_Circulaire*)list_curr(projet->beton.sections);
        
        if (section->numero == numero)
            return section;
    }
    while (list_mvnext(projet->beton.sections) != NULL);
    
    BUGMSG(0, NULL, gettext("%s : Section en béton n°%d introuvable.\n"), "_1992_1_1_sections_cherche_numero", numero);
}


double _1992_1_1_sections_j(void* sect)
/* Description : Renvoie le paramètre J pour la section étudiée
 * Paramètres : void* sect : section à étudier
 * Valeur renvoyée :
 *   Succès : Résultat
 *   Échec : 0. en cas de paramètres invalides :
 *             (sect == NULL)
 */
{
    BUGMSG(sect, 0., "_1992_1_1_sections_j\n");
    
    switch(((Beton_Section_Rectangulaire*) sect)->type)
    {
        case BETON_SECTION_RECTANGULAIRE :
        {
            Beton_Section_Rectangulaire *section = (Beton_Section_Rectangulaire*)sect;
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
        case BETON_SECTION_T :
        {
            Beton_Section_T *section = (Beton_Section_T*)sect;
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
        case BETON_SECTION_CARRE :
        {
            Beton_Section_Carre *section = (Beton_Section_Carre*)sect;
            return section->cote*section->cote*section->cote*section->cote/16.*(16./3.-3.364*(1.-1./12.));
            
    // Pour une section carrée de section constante, J vaut :\end{verbatim}\begin{displaymath}
    // J = \frac{cote^4}{16} \cdot \left[\frac{16}{3}-3.364 \cdot \left(1-\frac{1}{12}\right)\right]\end{displaymath}\begin{verbatim}
            break;
        }
        case BETON_SECTION_CIRCULAIRE :
        {
            Beton_Section_Circulaire *section = (Beton_Section_Circulaire*)sect;
            return M_PI*section->diametre*section->diametre*section->diametre*section->diametre/32.;
    // Pour une section circulaire de section constante, J vaut :\end{verbatim}\begin{displaymath}
    // J = \frac{\pi \cdot \phi^4}{32}\end{displaymath}\begin{verbatim}
            break;
        }
        default :
        {
            BUGMSG(0, 0., "_1992_1_1_sections_j\n");
            break;
        }
    }
}


double _1992_1_1_sections_iy(void* sect)
/* Description : Renvoie l'inertie I selon l'axe y lorsque la section est constante
 * Paramètres : void* section : section à étudier
 * Valeur renvoyée :
 *   Succès : Résultat
 *   Échec : 0. en cas de paramètres invalides :
 *             (section == NULL)
 */
{
    BUGMSG(sect, 0., "_1992_1_1_sections_iy\n");
    
    switch(((Beton_Section_Rectangulaire*) sect)->type)
    {
        case BETON_SECTION_RECTANGULAIRE :
        {
            Beton_Section_Rectangulaire *section = (Beton_Section_Rectangulaire*)sect;
            return section->largeur*section->hauteur*section->hauteur*section->hauteur/12.;
    // Pour une section rectantulaire de section constante, Iy vaut :\end{verbatim}\begin{displaymath}
    // I_y = \frac{l \cdot h^3}{12} \end{displaymath}\begin{verbatim}
            break;
        }
        case BETON_SECTION_T :
        {
            Beton_Section_T *section = (Beton_Section_T*)sect;
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
        case BETON_SECTION_CARRE :
        {
            Beton_Section_Carre *section = (Beton_Section_Carre*)sect;
            return section->cote*section->cote*section->cote*section->cote/12.;
            
    // Pour une section carrée de section constante, Iy vaut :\end{verbatim}\begin{displaymath}
    // I_y = \frac{c^4}{12} \end{displaymath}\begin{verbatim}
            break;
        }
        case BETON_SECTION_CIRCULAIRE :
        {
            Beton_Section_Circulaire *section = (Beton_Section_Circulaire*)sect;
            return M_PI*section->diametre*section->diametre*section->diametre*section->diametre/64.;
    // Pour une section circulaire de section constante, Iy vaut :\end{verbatim}\begin{displaymath}
    // I_y = \frac{\pi \cdot \phi^4}{64} \end{displaymath}\begin{verbatim}
            break;
        }
        default :
        {
            BUGMSG(0, 0., "_1992_1_1_sections_iy\n");
            break;
        }
    }
}


double _1992_1_1_sections_iz(void* sect)
/* Description : Renvoie l'inertie I selon l'axe z lorsque la section est constante
 * Paramètres : void* section : section à étudier
 * Valeur renvoyée :
 *   Succès : Résultat
 *   Échec : 0. en cas de paramètres invalides :
 *             (section == NULL)
 */
{
    BUGMSG(sect, 0., "_1992_1_1_sections_iz\n");
    
    switch(((Beton_Section_Rectangulaire*) sect)->type)
    {
        case BETON_SECTION_RECTANGULAIRE :
        {
            Beton_Section_Rectangulaire *section = (Beton_Section_Rectangulaire*)sect;
            return section->hauteur*section->largeur*section->largeur*section->largeur/12.;
            
    // Pour une section rectantulaire de section constante, I vaut :\end{verbatim}\begin{displaymath}
    // I = \frac{h \cdot l^3}{12} \end{displaymath}\begin{verbatim}
            break;
        }
        case BETON_SECTION_T :
        {
            Beton_Section_T *section = (Beton_Section_T*)sect;
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
        case BETON_SECTION_CARRE :
        {
            Beton_Section_Carre *section = (Beton_Section_Carre*)sect;
            return section->cote*section->cote*section->cote*section->cote/12.;
    // Pour une section carrée de section constante, I vaut :\end{verbatim}\begin{displaymath}
    // I = \frac{c^4}{12} \end{displaymath}\begin{verbatim}
            break;
        }
        case BETON_SECTION_CIRCULAIRE :
        {
            Beton_Section_Circulaire *section = (Beton_Section_Circulaire*)sect;
            return M_PI*section->diametre*section->diametre*section->diametre*section->diametre/64.;
    // Pour une section circulaire de section constante, I vaut :\end{verbatim}\begin{displaymath}
    // I = \frac{\pi \cdot \phi^4}{64} \end{displaymath}\begin{verbatim}
            break;
        }
        default :
        {
            BUGMSG(0, 0., "_1992_1_1_sections_iz\n");
            break;
        }
    }
}


double _1992_1_1_sections_ay(Beton_Barre *barre, unsigned int discretisation)
/* Description : Renvoie le paramètre de souplesse a de la poutre selon l'axe y
 * Paramètres : Beton_Barre *barre : la barre à étudier
 *              unsigned int discretisation : partie de la barre à étudier
 * Valeur renvoyée :
 *   Succès : Résultat
 *   Échec : 0. en cas de paramètres invalides :
 *             (barre == NULL) ou
 *             (barre->section == NULL) ou
 *             (barre->materiau) ou
 *             (barre->noeud_debut) ou
 *             (barre->noeud_fin) ou
 *             (discretisation>barre->discretisation_element)
 */
{
    Beton_Section_Rectangulaire *section_tmp;
    EF_Noeud    *debut, *fin;
    double      xx, yy, zz, ll;
    double      E;
    
    BUGMSG(barre, 0., "_1992_1_1_sections_ay\n");
    BUGMSG(barre->section, 0., "_1992_1_1_sections_ay\n");
    BUGMSG(barre->materiau, 0., "_1992_1_1_sections_ay\n");
    BUGMSG(barre->noeud_debut, 0., "_1992_1_1_sections_ay\n");
    BUGMSG(barre->noeud_fin, 0., "_1992_1_1_sections_ay\n");
    BUGMSG(discretisation<=barre->discretisation_element, 0., "_1992_1_1_sections_ay\n");
    
    // Le coefficient a est défini par la formule :\end{verbatim}\begin{displaymath}
    // a_y = \frac{1}{l^2}\int_0^l \frac{(l-x)^2}{E \cdot I_y(x)} dx\end{displaymath}\begin{verbatim}
    section_tmp = (Beton_Section_Rectangulaire*)barre->section;
    
    if (discretisation == 0)
        debut = barre->noeud_debut;
    else
        debut = barre->noeuds_intermediaires[discretisation-1];
    if (discretisation == barre->discretisation_element)
        fin = barre->noeud_fin;
    else
        fin = barre->noeuds_intermediaires[discretisation];
    
    xx = fin->position.x - debut->position.x;
    yy = fin->position.y - debut->position.y;
    zz = fin->position.z - debut->position.z;
    ll = sqrt(xx*xx+yy*yy+zz*zz);
    
    E = barre->materiau->ecm;
    
    switch(section_tmp->type)
    {
        case BETON_SECTION_RECTANGULAIRE :
        case BETON_SECTION_T :
        case BETON_SECTION_CARRE :
        case BETON_SECTION_CIRCULAIRE :
        {
            return ll/(3.*E*_1992_1_1_sections_iy(section_tmp));
            break;
        }
        default :
        {
            BUGMSG(0, 0., "_1992_1_1_sections_ay\n");
            break;
        }
    }
}


double _1992_1_1_sections_by(Beton_Barre *barre, unsigned int discretisation)
/* Description : Renvoie le paramètre de souplesse b de la poutre selon l'axe y
 * Paramètres : Beton_Barre *barre : la barre à étudier
 *              unsigned int discretisation : partie de la barre à étudier
 * Valeur renvoyée :
 *   Succès : Résultat
 *   Échec : 0. en cas de paramètres invalides :
 *             (barre == NULL) ou
 *             (barre->section == NULL) ou
 *             (barre->materiau) ou
 *             (barre->noeud_debut) ou
 *             (barre->noeud_fin) ou
 *             (discretisation>barre->discretisation_element)
 */
{
    Beton_Section_Rectangulaire *section_tmp;
    EF_Noeud    *debut, *fin;
    double      xx, yy, zz, ll;
    double      E;
    
    BUGMSG(barre, 0., "_1992_1_1_sections_by\n");
    BUGMSG(barre->section, 0., "_1992_1_1_sections_by\n");
    BUGMSG(barre->materiau, 0., "_1992_1_1_sections_by\n");
    BUGMSG(barre->noeud_debut, 0., "_1992_1_1_sections_by\n");
    BUGMSG(barre->noeud_fin, 0., "_1992_1_1_sections_by\n");
    BUGMSG(discretisation<=barre->discretisation_element, 0., "_1992_1_1_sections_by\n");
    
    // Le coefficient b est défini par la formule :\end{verbatim}\begin{displaymath}
    // b_y = \frac{1}{l^2}\int_0^l \frac{x \cdot (l-x)^2}{E \cdot I_y(x)} dx\end{displaymath}\begin{verbatim}
    section_tmp = (Beton_Section_Rectangulaire*)barre->section;
    
    if (discretisation == 0)
        debut = barre->noeud_debut;
    else
        debut = barre->noeuds_intermediaires[discretisation-1];
    if (discretisation == barre->discretisation_element)
        fin = barre->noeud_fin;
    else
        fin = barre->noeuds_intermediaires[discretisation];
    
    xx = fin->position.x - debut->position.x;
    yy = fin->position.y - debut->position.y;
    zz = fin->position.z - debut->position.z;
    ll = sqrt(xx*xx+yy*yy+zz*zz);
    
    E = barre->materiau->ecm;
    
    switch(section_tmp->type)
    {
        case BETON_SECTION_RECTANGULAIRE :
        case BETON_SECTION_T :
        case BETON_SECTION_CARRE :
        case BETON_SECTION_CIRCULAIRE :
        {
            return ll/(6.*E*_1992_1_1_sections_iy(section_tmp));
            break;
        }
        default :
        {
            BUGMSG(0, 0., "_1992_1_1_sections_by\n");
            break;
        }
    }
}


double _1992_1_1_sections_cy(Beton_Barre *barre, unsigned int discretisation)
/* Description : Renvoie le paramètre de souplesse c de la poutre selon l'axe y
 * Paramètres : Beton_Barre *barre : la barre à étudier
 *              unsigned int discretisation : partie de la barre à étudier
 * Valeur renvoyée :
 *   Succès : Résultat
 *   Échec : 0. en cas de paramètres invalides :
 *             (barre == NULL) ou
 *             (barre->section == NULL) ou
 *             (barre->materiau) ou
 *             (barre->noeud_debut) ou
 *             (barre->noeud_fin) ou
 *             (discretisation>barre->discretisation_element)
 */
{
    Beton_Section_Rectangulaire *section_tmp;
    EF_Noeud    *debut, *fin;
    double      xx, yy, zz, ll;
    double      E;
    
    BUGMSG(barre, 0., "_1992_1_1_sections_cy\n");
    BUGMSG(barre->section, 0., "_1992_1_1_sections_cy\n");
    BUGMSG(barre->materiau, 0., "_1992_1_1_sections_cy\n");
    BUGMSG(barre->noeud_debut, 0., "_1992_1_1_sections_cy\n");
    BUGMSG(barre->noeud_fin, 0., "_1992_1_1_sections_cy\n");
    BUGMSG(discretisation<=barre->discretisation_element, 0., "_1992_1_1_sections_cy\n");
    
    // Le coefficient c est défini par la formule :\end{verbatim}\begin{displaymath}
    // c_y = \frac{1}{l^2}\int_0^l \frac{x^2}{E \cdot I_y(x)} dx\end{displaymath}\begin{verbatim}
    section_tmp = (Beton_Section_Rectangulaire*)barre->section;
    
    if (discretisation == 0)
        debut = barre->noeud_debut;
    else
        debut = barre->noeuds_intermediaires[discretisation-1];
    if (discretisation == barre->discretisation_element)
        fin = barre->noeud_fin;
    else
        fin = barre->noeuds_intermediaires[discretisation];
    
    xx = fin->position.x - debut->position.x;
    yy = fin->position.y - debut->position.y;
    zz = fin->position.z - debut->position.z;
    ll = sqrt(xx*xx+yy*yy+zz*zz);
    
    E = barre->materiau->ecm;
    
    switch(section_tmp->type)
    {
        case BETON_SECTION_RECTANGULAIRE :
        case BETON_SECTION_T :
        case BETON_SECTION_CARRE :
        case BETON_SECTION_CIRCULAIRE :
        {
            return ll/(3.*E*_1992_1_1_sections_iy(section_tmp));
            break;
        }
        default :
        {
            BUGMSG(0, 0., "_1992_1_1_sections_cy\n");
            break;
        }
    }
}


double _1992_1_1_sections_az(Beton_Barre *barre, unsigned int discretisation)
/* Description : Renvoie le paramètre de souplesse a de la poutre selon l'axe z
 * Paramètres : Beton_Barre *barre : la barre à étudier
 *              unsigned int discretisation : partie de la barre à étudier
 * Valeur renvoyée :
 *   Succès : Résultat
 *   Échec : 0. en cas de paramètres invalides :
 *             (barre == NULL) ou
 *             (barre->section == NULL) ou
 *             (barre->materiau) ou
 *             (barre->noeud_debut) ou
 *             (barre->noeud_fin) ou
 *             (discretisation>barre->discretisation_element)
 */
{
    Beton_Section_Rectangulaire *section_tmp;
    EF_Noeud    *debut, *fin;
    double      xx, yy, zz, ll;
    double      E;
    
    BUGMSG(barre, 0., "_1992_1_1_sections_az\n");
    BUGMSG(barre->section, 0., "_1992_1_1_sections_az\n");
    BUGMSG(barre->materiau, 0., "_1992_1_1_sections_az\n");
    BUGMSG(barre->noeud_debut, 0., "_1992_1_1_sections_az\n");
    BUGMSG(barre->noeud_fin, 0., "_1992_1_1_sections_az\n");
    BUGMSG(discretisation<=barre->discretisation_element, 0., "_1992_1_1_sections_az\n");
    
    // Le coefficient a est défini par la formule :\end{verbatim}\begin{displaymath}
    // a_z = \frac{1}{l^2}\int_0^l \frac{(l-x)^2}{E \cdot I_z(x)} dx\end{displaymath}\begin{verbatim}
    section_tmp = (Beton_Section_Rectangulaire*)barre->section;
    
    if (discretisation == 0)
        debut = barre->noeud_debut;
    else
        debut = barre->noeuds_intermediaires[discretisation-1];
    if (discretisation == barre->discretisation_element)
        fin = barre->noeud_fin;
    else
        fin = barre->noeuds_intermediaires[discretisation];
    
    xx = fin->position.x - debut->position.x;
    yy = fin->position.y - debut->position.y;
    zz = fin->position.z - debut->position.z;
    ll = sqrt(xx*xx+yy*yy+zz*zz);
    
    E = barre->materiau->ecm;
    
    switch(section_tmp->type)
    {
        case BETON_SECTION_RECTANGULAIRE :
        case BETON_SECTION_T :
        case BETON_SECTION_CARRE :
        case BETON_SECTION_CIRCULAIRE :
        {
            return ll/(3.*E*_1992_1_1_sections_iz(section_tmp));
            break;
        }
        default :
        {
            BUGMSG(0, 0., "_1992_1_1_sections_az\n");
            break;
        }
    }
}


double _1992_1_1_sections_bz(Beton_Barre *barre, unsigned int discretisation)
/* Description : Renvoie le paramètre de souplesse b de la poutre selon l'axe z
 * Paramètres : Beton_Barre *barre : la barre à étudier
 *              unsigned int discretisation : partie de la barre à étudier
 * Valeur renvoyée :
 *   Succès : Résultat
 *   Échec : 0. en cas de paramètres invalides :
 *             (barre == NULL) ou
 *             (barre->section == NULL) ou
 *             (barre->materiau) ou
 *             (barre->noeud_debut) ou
 *             (barre->noeud_fin) ou
 *             (discretisation>barre->discretisation_element)
 */
{
    Beton_Section_Rectangulaire *section_tmp;
    EF_Noeud    *debut, *fin;
    double      xx, yy, zz, ll;
    double      E;
    
    BUGMSG(barre, 0., "_1992_1_1_sections_bz\n");
    BUGMSG(barre->section, 0., "_1992_1_1_sections_bz\n");
    BUGMSG(barre->materiau, 0., "_1992_1_1_sections_bz\n");
    BUGMSG(barre->noeud_debut, 0., "_1992_1_1_sections_bz\n");
    BUGMSG(barre->noeud_fin, 0., "_1992_1_1_sections_bz\n");
    BUGMSG(discretisation<=barre->discretisation_element, 0., "_1992_1_1_sections_bz\n");
    
    // Le coefficient b est défini par la formule :\end{verbatim}\begin{displaymath}
    // b_z = \frac{1}{l^2}\int_0^l \frac{x \cdot (l-x)^2}{E \cdot I_z(x)} dx\end{displaymath}\begin{verbatim}
    section_tmp = (Beton_Section_Rectangulaire*)barre->section;
    
    if (discretisation == 0)
        debut = barre->noeud_debut;
    else
        debut = barre->noeuds_intermediaires[discretisation-1];
    if (discretisation == barre->discretisation_element)
        fin = barre->noeud_fin;
    else
        fin = barre->noeuds_intermediaires[discretisation];
    
    xx = fin->position.x - debut->position.x;
    yy = fin->position.y - debut->position.y;
    zz = fin->position.z - debut->position.z;
    ll = sqrt(xx*xx+yy*yy+zz*zz);
    
    E = barre->materiau->ecm;
    
    switch(section_tmp->type)
    {
        case BETON_SECTION_RECTANGULAIRE :
        case BETON_SECTION_T :
        case BETON_SECTION_CARRE :
        case BETON_SECTION_CIRCULAIRE :
        {
            return ll/(6.*E*_1992_1_1_sections_iz(section_tmp));
            break;
        }
        default :
        {
            BUGMSG(0, 0., "_1992_1_1_sections_bz\n");
            break;
        }
    }
}


double _1992_1_1_sections_cz(Beton_Barre *barre, unsigned int discretisation)
/* Description : Renvoie le paramètre de souplesse c de la poutre selon l'axe z
 * Paramètres : Beton_Barre *barre : la barre à étudier
 *              unsigned int discretisation : partie de la barre à étudier
 * Valeur renvoyée :
 *   Succès : Résultat
 *   Échec : 0. en cas de paramètres invalides :
 *             (barre == NULL) ou
 *             (barre->section == NULL) ou
 *             (barre->materiau) ou
 *             (barre->noeud_debut) ou
 *             (barre->noeud_fin) ou
 *             (discretisation>barre->discretisation_element)
 */
{
    Beton_Section_Rectangulaire *section_tmp;
    EF_Noeud    *debut, *fin;
    double      xx, yy, zz, ll;
    double      E;
    
    BUGMSG(barre, 0., "_1992_1_1_sections_cz\n");
    BUGMSG(barre->section, 0., "_1992_1_1_sections_cz\n");
    BUGMSG(barre->materiau, 0., "_1992_1_1_sections_cz\n");
    BUGMSG(barre->noeud_debut, 0., "_1992_1_1_sections_cz\n");
    BUGMSG(barre->noeud_fin, 0., "_1992_1_1_sections_cz\n");
    BUGMSG(discretisation<=barre->discretisation_element, 0., "_1992_1_1_sections_cz\n");
    
    // Le coefficient c est défini par la formule :\end{verbatim}\begin{displaymath}
    // c_z = \frac{1}{l^2}\int_0^l \frac{x^2}{E \cdot I_y(x)} dx\end{displaymath}\begin{verbatim}
    section_tmp = (Beton_Section_Rectangulaire*)barre->section;
    
    if (discretisation == 0)
        debut = barre->noeud_debut;
    else
        debut = barre->noeuds_intermediaires[discretisation-1];
    if (discretisation == barre->discretisation_element)
        fin = barre->noeud_fin;
    else
        fin = barre->noeuds_intermediaires[discretisation];
    
    xx = fin->position.x - debut->position.x;
    yy = fin->position.y - debut->position.y;
    zz = fin->position.z - debut->position.z;
    ll = sqrt(xx*xx+yy*yy+zz*zz);
    
    E = barre->materiau->ecm;
    
    switch(section_tmp->type)
    {
        case BETON_SECTION_RECTANGULAIRE :
        case BETON_SECTION_T :
        case BETON_SECTION_CARRE :
        case BETON_SECTION_CIRCULAIRE :
        {
            return ll/(3.*E*_1992_1_1_sections_iz(section_tmp));
            break;
        }
        default :
        {
            BUGMSG(0, 0., "_1992_1_1_sections_cz\n");
            break;
        }
    }
}


double _1992_1_1_sections_s(void *sect)
/* Description : Renvoie la surface de la section étudiée
 * Paramètres : void* section : section à étudier.
 * Valeur renvoyée :
 *   Succès : Résultat
 *   Échec : 0. en cas de paramètres invalides :
 *             (sect == NULL) ou
 */
{
    BUGMSG(sect, 0., "_1992_1_1_sections_s\n");
    
    switch(((Beton_Section_Rectangulaire*) sect)->type)
    {
        case BETON_SECTION_RECTANGULAIRE :
        {
            Beton_Section_Rectangulaire *section = (Beton_Section_Rectangulaire*)sect;
            return section->hauteur*section->largeur;
            
    // Pour une section rectantulaire de section constante, S vaut :\end{verbatim}\begin{displaymath}
    // S = h \cdot l\end{displaymath}\begin{verbatim}
            break;
        }
        case BETON_SECTION_T :
        {
            Beton_Section_T *section = (Beton_Section_T*)sect;
            return section->hauteur_table*section->largeur_table+section->hauteur_ame*section->largeur_ame;
            
    // Pour une section en T de section constante (lt : largeur de la table, la : largeur de
    //   l'âme, ht : hauteur de la table, ha : hauteur de l'âme), S vaut :\end{verbatim}\begin{displaymath}
    // S = h_t \cdot l_t+h_a \cdot l_a\end{displaymath}\begin{verbatim}
            break;
        }
        case BETON_SECTION_CARRE :
        {
            Beton_Section_Carre *section = (Beton_Section_Carre*)sect;
            return section->cote*section->cote;
            
    // Pour une section carrée de section constante, S vaut :\end{verbatim}\begin{displaymath}
    // S = c^2\end{displaymath}\begin{verbatim}
            break;
        }
        case BETON_SECTION_CIRCULAIRE :
        {
            Beton_Section_Circulaire *section = (Beton_Section_Circulaire*)sect;
            return M_PI*section->diametre*section->diametre/4.;
    // Pour une section circulaire de section constante, S vaut :\end{verbatim}\begin{displaymath}
    // S = \frac{\pi \cdot \phi^2}{4} \end{displaymath}\begin{verbatim}
            break;
        }
        default :
        {
            BUGMSG(0, 0., "_1992_1_1_sections_s\n");
            break;
        }
    }
}


double _1992_1_1_sections_es_l(Beton_Barre *barre, unsigned int discretisation,
  double d, double f)
/* Description : Renvoie l'équivalent du rapport ES/L pour la barre étudiée
 * Paramètres : Beton_Barre *barre : la barre à étudier,
 *              unsigned int discretisation : partie de la barre à étudier,
 *              double d : début de la partie à prendre en compte,
 *              double f : fin de la partie à prendre en compte.
 * Valeur renvoyée :
 *   Succès : Résultat
 *   Échec : 0. en cas de paramètres invalides :
 *             (barre == NULL) ou
 *             (barre->section == NULL) ou
 *             (barre->materiau) ou
 *             (barre->noeud_debut) ou
 *             (barre->noeud_fin) ou
 *             (discretisation>barre->discretisation_element)
 *             (debut>fin)
 */
{
    Beton_Section_Rectangulaire *section_tmp;
    double      E;
    
    BUGMSG(barre, 0., "_1992_1_1_sections_es_l\n");
    BUGMSG(barre->section, 0., "_1992_1_1_sections_es_l\n");
    BUGMSG(barre->materiau, 0., "_1992_1_1_sections_es_l\n");
    BUGMSG(barre->noeud_debut, 0., "_1992_1_1_sections_es_l\n");
    BUGMSG(barre->noeud_fin, 0., "_1992_1_1_sections_es_l\n");
    BUGMSG(discretisation<=barre->discretisation_element, 0., "_1992_1_1_sections_es_l\n");
    BUGMSG(!((d>f) && (!(ERREUR_RELATIVE_EGALE(d, f)))), 0., "_1992_1_1_sections_es_l\n");
    
    // Le facteur ES/L est défini par la formule :\end{verbatim}\begin{displaymath}
    // \frac{E \cdot S}{L} = \frac{E}{\int_d^f \frac{1}{S(x)} dx}\end{displaymath}\begin{verbatim}
    section_tmp = (Beton_Section_Rectangulaire*)barre->section;
    
    E = barre->materiau->ecm;
    
    switch(section_tmp->type)
    {
        case BETON_SECTION_RECTANGULAIRE :
        {
            Beton_Section_Rectangulaire *section = (Beton_Section_Rectangulaire*)barre->section;
            double      S = section->hauteur*section->largeur;
            
    // Pour une section rectantulaire de section constante, ES/L vaut :\end{verbatim}\begin{displaymath}
    // \frac{E \cdot S}{L} = \frac{E \cdot h \cdot l}{L} \end{displaymath}\begin{verbatim}
            return E*S/(f-d);
            break;
        }
        case BETON_SECTION_T :
        {
            Beton_Section_T *section = (Beton_Section_T*)barre->section;
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
        case BETON_SECTION_CARRE :
        {
            Beton_Section_Carre *section = (Beton_Section_Carre*)barre->section;
            double      S = section->cote*section->cote;
            
    // Pour une section carrée de section constante, ES/L vaut :\end{verbatim}\begin{displaymath}
    // \frac{E \cdot S}{L} = \frac{E \cdot c^2}{L}\end{displaymath}\begin{verbatim}
            return E*S/(f-d);
            break;
        }
        case BETON_SECTION_CIRCULAIRE :
        {
            Beton_Section_Circulaire *section = (Beton_Section_Circulaire*)barre->section;
            double      S = M_PI*section->diametre*section->diametre/4.;
    // Pour une section circulaire de section constante, ES/L vaut :\end{verbatim}\begin{displaymath}
    // \frac{E \cdot S}{L} = \frac{E \cdot \pi \cdot \phi^2}{4 \cdot L} \end{displaymath}\begin{verbatim}
            return E*S/(f-d);
            break;
        }
        default :
        {
            BUGMSG(0, 0., "_1992_1_1_sections_es_l\n");
            break;
        }
    }
}


double _1992_1_1_sections_gj_l(Beton_Barre *barre, unsigned int discretisation)
/* Description : Renvoie l'équivalent du rapport GJ/L pour la barre étudiée
 * Paramètres : Beton_Barre *barre : la barre à étudier
 *              unsigned int discretisation : partie de la barre à étudier
 * Valeur renvoyée :
 *   Succès : Résultat
 *   Échec : 0. en cas de paramètres invalides :
 *             (barre == NULL) ou
 *             (barre->section == NULL) ou
 *             (barre->materiau) ou
 *             (barre->noeud_debut) ou
 *             (barre->noeud_fin) ou
 *             (discretisation>barre->discretisation_element)
 */
{
    Beton_Section_Rectangulaire *section_tmp;
    EF_Noeud    *debut, *fin;
    double      xx, yy, zz, ll;
    double      G;
    
    BUGMSG(barre, 0., "_1992_1_1_sections_gj_l\n");
    BUGMSG(barre->section, 0., "_1992_1_1_sections_gj_l\n");
    BUGMSG(barre->materiau, 0., "_1992_1_1_sections_gj_l\n");
    BUGMSG(barre->noeud_debut, 0., "_1992_1_1_sections_gj_l\n");
    BUGMSG(barre->noeud_fin, 0., "_1992_1_1_sections_gj_l\n");
    BUGMSG(discretisation<=barre->discretisation_element, 0., "_1992_1_1_sections_gj_l\n");
    
    // Le facteur GJ/L est défini par la formule :\end{verbatim}\begin{displaymath}
    // \frac{G \cdot J}{L} = \frac{G}{\int_0^l \frac{1}{J(x)} dx}\end{displaymath}\begin{verbatim}
    section_tmp = (Beton_Section_Rectangulaire*)barre->section;
    
    if (discretisation == 0)
        debut = barre->noeud_debut;
    else
        debut = barre->noeuds_intermediaires[discretisation-1];
    if (discretisation == barre->discretisation_element)
        fin = barre->noeud_fin;
    else
        fin = barre->noeuds_intermediaires[discretisation];
    
    xx = fin->position.x - debut->position.x;
    yy = fin->position.y - debut->position.y;
    zz = fin->position.z - debut->position.z;
    ll = sqrt(xx*xx+yy*yy+zz*zz);
    
    G = barre->materiau->gnu_0_2;
    
    switch(section_tmp->type)
    {
        case BETON_SECTION_RECTANGULAIRE :
        {
            Beton_Section_Rectangulaire *section = (Beton_Section_Rectangulaire*)barre->section;
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
        case BETON_SECTION_T :
        {
            Beton_Section_T *section = (Beton_Section_T*)barre->section;
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
        case BETON_SECTION_CARRE :
        {
            Beton_Section_Carre *section = (Beton_Section_Carre*)barre->section;
            double      J = section->cote*section->cote*section->cote*section->cote/16.*(16./3.-3.364*(1.-1./12.));
            
    // Pour une section carrée de section constante, GJ/L vaut :\end{verbatim}\begin{displaymath}
    // \frac{G \cdot J}{L} \texttt{ avec } J = \frac{cote^4}{16} \cdot \left[\frac{16}{3}-3.364 \cdot \left(1-\frac{1}{12}\right)\right]\end{displaymath}\begin{verbatim}
            return G*J/ll;
            break;
        }
        case BETON_SECTION_CIRCULAIRE :
        {
            Beton_Section_Circulaire *section = (Beton_Section_Circulaire*)barre->section;
            double      J = M_PI*section->diametre*section->diametre*section->diametre*section->diametre/32.;
    // Pour une section circulaire de section constante, GJ/L vaut :\end{verbatim}\begin{displaymath}
    // \frac{G \cdot J}{L} \texttt{ avec } J = \frac{\pi \cdot \phi^4}{32}\end{displaymath}\begin{verbatim}
            return G*J/ll;
            break;
        }
        default :
        {
            BUGMSG(0, 0., "_1992_1_1_sections_gj_l\n");
            break;
        }
    }
}


int _1992_1_1_sections_free(Projet *projet)
/* Description : Libère l'ensemble des sections en béton
 * Paramètres : Projet *projet : la variable projet
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet == NULL) ou
 *             (projet->beton.sections == NULL)
 */
{
    BUGMSG(projet, -1, "_1992_1_1_sections_free\n");
    BUGMSG(projet->beton.sections, -1, "_1992_1_1_sections_free\n");
    
    // Trivial
    while (!list_empty(projet->beton.sections))
        free((Beton_Section_Circulaire*)list_remove_front(projet->beton.sections));
    
    free(projet->beton.sections);
    projet->beton.sections = NULL;
    
    return 0;
}
