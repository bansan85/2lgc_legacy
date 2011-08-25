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
#include "1992_1_1_section.h"

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
    double                          a, b;
    
    BUGMSG(projet, -1, "_1992_1_1_sections_ajout_rectangulaire\n");
    BUGMSG(projet->beton.sections, -1, "_1992_1_1_sections_ajout_rectangulaire\n");
    
    list_mvrear(projet->beton.sections);
    section_nouvelle.caracteristiques = (Beton_Section_Caracteristiques*)malloc(sizeof(Beton_Section_Caracteristiques));
    BUGMSG(section_nouvelle.caracteristiques, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "_1992_1_1_sections_ajout_rectangulaire");
    
    // Les caractéristiques de la section sont les suivantes :\end{verbatim}\begin{displaymath}
    //   S = l \cdot h\texttt{  et  }cdg_{haut} = \frac{h}{2}\texttt{  et  }cdg_{bas} = \frac{h}{2}\texttt{  et  }cdg_{droite} = \frac{l}{2}\texttt{  et  }cdg_{gauche} = \frac{l}{2}\end{displaymath}\begin{displaymath}
    //   I_y = \frac{l \cdot h^3}{12}\texttt{  et  }I_z = \frac{h \cdot l^3}{12}\texttt{  et  }J = \frac{a \cdot b^3}{16} \cdot \left[\frac{16}{3}-3.364 \cdot \frac{b}{a} \cdot \left( 1-\frac{b^4}{12 \cdot a^4} \right) \right]\texttt{ avec }\substack{a=max(h,l)\\b=min(h,l)}
    //   \end{displaymath}\begin{verbatim}
    section_nouvelle.caracteristiques->type = BETON_SECTION_RECTANGULAIRE;
    section_nouvelle.largeur = l;
    section_nouvelle.hauteur = h;
    section_nouvelle.caracteristiques->s = l*h;
    section_nouvelle.caracteristiques->cdgh = h/2.;
    section_nouvelle.caracteristiques->cdgb = h/2.;
    section_nouvelle.caracteristiques->cdgd = l/2.;
    section_nouvelle.caracteristiques->cdgg = l/2.;
    section_nouvelle.caracteristiques->iy = l*h*h*h/12.;
    section_nouvelle.caracteristiques->iz = h*l*l*l/12.;
    if (l > h)
        { a = l; b = h; }
    else
        { a = h; b = l; }
    section_nouvelle.caracteristiques->j = a*b*b*b/16.*(16./3.-3.364*b/a*(1.-b*b*b*b/(12.*a*a*a*a)));
    
    section_en_cours = (Beton_Section_Rectangulaire*)list_rear(projet->beton.sections);
    if (section_en_cours == NULL)
        section_nouvelle.caracteristiques->numero = 0;
    else
        section_nouvelle.caracteristiques->numero = section_en_cours->caracteristiques->numero+1;
    
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
    double              a, b, aa, bb;
    
    BUGMSG(projet, -1, "_1992_1_1_sections_ajout_T\n");
    BUGMSG(projet->beton.sections, -1, "_1992_1_1_sections_ajout_T\n");
    
    list_mvrear(projet->beton.sections);
    section_nouvelle.caracteristiques = (Beton_Section_Caracteristiques*)malloc(sizeof(Beton_Section_Caracteristiques));
    BUGMSG(section_nouvelle.caracteristiques, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "_1992_1_1_sections_ajout_T");
    
    // Les caractéristiques de la section sont les suivantes :\end{verbatim}\begin{displaymath}
    //   S = lt \cdot ht+la \cdot ha\end{displaymath}\begin{displaymath}
    //   cdg_{haut} = \frac{\frac{lt \cdot ht^2}{2}+la \cdot ha \cdot (ht+\frac{ha}{2})}{S}\texttt{  et  }cdg_{bas} = (ht+ha)-cdg_{haut}\texttt{  et  }cdg_{droite} = \frac{lt}{2}\texttt{  et  }cdg_{gauche} = \frac{lt}{2}\end{displaymath}\begin{displaymath}
    //   I_y = \frac{lt \cdot ht^3}{12}+\frac{la \cdot ha^3}{12}+lt \cdot ht \cdot \left(\frac{ht}{2}-cdg_{haut} \right)^2+la \cdot ha \cdot \left(\frac{ha}{2}-cdg_{bas} \right)^2\texttt{  et  }I_z = \frac{ht \cdot lt^3}{12}+\frac{ha \cdot la^3}{12}\end{displaymath}\begin{displaymath}
    //   J = \frac{a \cdot b^3}{16} \cdot \left[\frac{16}{3}-3.364 \cdot \frac{b}{a} \cdot \left(1-\frac{b^4}{12 \cdot a^4}\right)\right]+\frac{aa \cdot bb^3}{16} \cdot \left[\frac{16}{3}-3.364 \cdot \frac{bb}{aa} \cdot \left(1-\frac{bb^4}{12 \cdot aa^4}\right)\right]\texttt{ avec }\substack{a=max(ht,lt)\\b=min(ht,lt)\\aa=max(ha,la)\\bb=min(ha,la)}
    //   \end{displaymath}\begin{verbatim}
    section_nouvelle.caracteristiques->type = BETON_SECTION_T;
    section_nouvelle.largeur_table = lt;
    section_nouvelle.largeur_ame = la;
    section_nouvelle.hauteur_table = ht;
    section_nouvelle.hauteur_ame = ha;
    section_nouvelle.caracteristiques->s = lt*ht+la*ha;
    section_nouvelle.caracteristiques->cdgh = (lt*ht*ht/2+la*ha*(ht+ha/2))/section_nouvelle.caracteristiques->s;
    section_nouvelle.caracteristiques->cdgb = (ht+ha)-section_nouvelle.caracteristiques->cdgh;
    section_nouvelle.caracteristiques->cdgd = lt/2;
    section_nouvelle.caracteristiques->cdgg = lt/2;
    section_nouvelle.caracteristiques->iy = lt*ht*ht*ht/12+la*ha*ha*ha/12+lt*ht*(ht/2-section_nouvelle.caracteristiques->cdgh)*(ht/2-section_nouvelle.caracteristiques->cdgh)+la*ha*(ha/2-section_nouvelle.caracteristiques->cdgb)*(ha/2-section_nouvelle.caracteristiques->cdgb);
    section_nouvelle.caracteristiques->iz = ht*lt*lt*lt/12+ha*la*la*la/12;
    
    if (lt > ht)
        { a = lt; b = ht; }
    else
        { a = ht; b = lt; }
    if (la > ha)
        { aa = la; bb = ha; }
    else
        { aa = ha; bb = la; }
    
    section_nouvelle.caracteristiques->j = a*b*b*b/16*(16/3-3.364*b/a*(1-b*b*b*b/(12*a*a*a*a)))+aa*bb*bb*bb/16*(16/3-3.364*bb/aa*(1-bb*bb*bb*bb/(12*aa*aa*aa*aa)));
    
    section_en_cours = (Beton_Section_T*)list_rear(projet->beton.sections);
    if (section_en_cours == NULL)
        section_nouvelle.caracteristiques->numero = 0;
    else
        section_nouvelle.caracteristiques->numero = section_en_cours->caracteristiques->numero+1;
    
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
    section_nouvelle.caracteristiques = (Beton_Section_Caracteristiques*)malloc(sizeof(Beton_Section_Caracteristiques));
    BUGMSG(section_nouvelle.caracteristiques, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "_1992_1_1_sections_ajout_carre");
    
    // Les caractéristiques de la section sont les suivantes :\end{verbatim}\begin{displaymath}
    //   S = cote^2\texttt{  et  }cdg_{haut} = \frac{cote}{2}\texttt{  et  }cdg_{bas} = \frac{cote}{2}\texttt{  et  }cdg_{droite} = \frac{cote}{2}\texttt{  et  }cdg_{gauche} = \frac{cote}{2}\end{displaymath}\begin{displaymath}
    //   I_y = \frac{cote^4}{12}\texttt{  et  }I_z = I_y\texttt{  et  }J = \frac{cote^4}{16} \cdot \left[\frac{16}{3}-3.364 \cdot \left(1-\frac{1}{12}\right)\right]\end{displaymath}\begin{verbatim}
    section_nouvelle.caracteristiques->type = BETON_SECTION_CARRE;
    section_nouvelle.cote = cote;
    section_nouvelle.caracteristiques->s = cote*cote;
    section_nouvelle.caracteristiques->cdgh = cote/2.;
    section_nouvelle.caracteristiques->cdgb = cote/2.;
    section_nouvelle.caracteristiques->cdgd = cote/2.;
    section_nouvelle.caracteristiques->cdgg = cote/2.;
    section_nouvelle.caracteristiques->iy = cote*cote*cote*cote/12.;
    section_nouvelle.caracteristiques->iz = section_nouvelle.caracteristiques->iy;
    section_nouvelle.caracteristiques->j = cote*cote*cote*cote/16.*(16./3.-3.364*(1.-1./12.));
    
    section_en_cours = (Beton_Section_Carre*)list_rear(projet->beton.sections);
    if (section_en_cours == NULL)
        section_nouvelle.caracteristiques->numero = 0;
    else
        section_nouvelle.caracteristiques->numero = section_en_cours->caracteristiques->numero+1;
    
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
    section_nouvelle.caracteristiques = (Beton_Section_Caracteristiques*)malloc(sizeof(Beton_Section_Caracteristiques));
    BUGMSG(section_nouvelle.caracteristiques, -2, gettext("%s : Erreur d'allocation mémoire.\n"), "_1992_1_1_sections_ajout_circulaire");
    
    // Les caractéristiques de la section sont les suivantes :\end{verbatim}\begin{displaymath}
    //   S = \frac{\pi \cdot diametre^2}{4}\end{displaymath}\begin{displaymath}
    //   cdg_{haut} = \frac{diametre}{2}\texttt{  et  }cdg_{bas} = \frac{diametre}{2}\texttt{  et  }cdg_{droite} = \frac{diametre}{2}\texttt{  et  }cdg_{gauche} = \frac{diametre}{2}\end{displaymath}\begin{displaymath}
    //   I_y = \frac{\pi \cdot diametre^4}{64}\texttt{  et  }I_z = I_y\texttt{  et  }J = \frac{\pi \cdot diametre^4}{32}\end{displaymath}\begin{verbatim}
    section_nouvelle.caracteristiques->type = BETON_SECTION_CIRCULAIRE;
    section_nouvelle.diametre = diametre;
    section_nouvelle.caracteristiques->s = M_PI*diametre*diametre/4.;
    section_nouvelle.caracteristiques->cdgh = diametre/2.;
    section_nouvelle.caracteristiques->cdgb = diametre/2.;
    section_nouvelle.caracteristiques->cdgd = diametre/2.;
    section_nouvelle.caracteristiques->cdgg = diametre/2.;
    section_nouvelle.caracteristiques->iy = M_PI*diametre*diametre*diametre*diametre/64.;
    section_nouvelle.caracteristiques->iz = section_nouvelle.caracteristiques->iy;
    section_nouvelle.caracteristiques->j = M_PI*diametre*diametre*diametre*diametre/32.;
    
    section_en_cours = (Beton_Section_Circulaire*)list_rear(projet->beton.sections);
    if (section_en_cours == NULL)
        section_nouvelle.caracteristiques->numero = 0;
    else
        section_nouvelle.caracteristiques->numero = section_en_cours->caracteristiques->numero+1;
    
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
        Beton_Section_Circulaire    *section = list_curr(projet->beton.sections);
        
        if (section->caracteristiques->numero == numero)
            return section;
    }
    while (list_mvnext(projet->beton.sections) != NULL);
    
    BUGMSG(0, NULL, gettext("%s : Section en béton n°%d introuvable.\n"), "_1992_1_1_sections_cherche_numero", numero);
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
    {
        
        Beton_Section_Circulaire    *section;
        section = list_remove_front(projet->beton.sections);
        free(section->caracteristiques);
        free(section);
    }
    
    free(projet->beton.sections);
    projet->beton.sections = NULL;
    
    return 0;
}
