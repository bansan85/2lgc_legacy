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
#include <string.h>
#include "common_projet.h"
#include "EF_noeud.h"
#include "1992_1_1_barres.h"
#include "common_erreurs.h"

int common_selection_ajout_nombre(unsigned int nombre, GList **liste)
/* Description : ajoute un nombre à la liste chainée.
 * Paramètres : unsigned int nombre : nombre à ajouter,
 *              GList **liste : liste où le nombre doit être ajouter.
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas d'échec.
 */
{
    GList           *list_parcours;
    unsigned int    nombre_liste;
    
    if (*liste == NULL)
    {
        *liste = g_list_append(*liste, GUINT_TO_POINTER(nombre));
        return 0;
    }
    
    list_parcours = *liste;
    do
    {
        nombre_liste = GPOINTER_TO_UINT(list_parcours->data);
        
        if (nombre_liste == nombre)
            return 0;
        else if (nombre_liste > nombre)
        {
            *liste = g_list_insert_before(*liste, list_parcours, GUINT_TO_POINTER(nombre));
            return 0;
        }
        list_parcours = g_list_next(list_parcours);
    } while (list_parcours != NULL);
    
    *liste = g_list_append(*liste, GUINT_TO_POINTER(nombre));
    
    return 0;
}


// Le format de texte est le suivant : 
// 1;2;3-4;6-9;10-20/2 donne les numéros :
// 1 2 3 4 6 7 8 9 10 12 14 16 18 20
GList *common_selection_renvoie_numeros(const char *texte)
/* Description : Renvoie sous forme d'une liste de numéros les numéros sous forme de texte.
 * Paramètres : const char *texte : le texte à convertir en numéros
 * Valeur renvoyée :
 *   Succès : Un pointeur vers une liste de numéros triée
 *   Échec : NULL en cas de problème.
 */
{
    char            *texte_clean;
    GList           *list;
    unsigned int    i, j;
    
    list = NULL;
    if (texte == NULL)
        return list;
    
    BUGMSG(texte_clean = malloc(sizeof(char)*(strlen(texte)+1)), NULL, gettext("Erreur d'allocation mémoire.\n"));
    
    // On vérifie si le texte contient bien une liste correcte de numéros
    i=0;
    for (j=0;j<strlen(texte);j++)
    {
        // Les caractères autorisées sont ;-/0123456789
        if ((texte[j] == ';') || (texte[j] == '-') || (texte[j] == '/') || (('0' <= texte[j]) && (texte[j] <= '9')))
        {
            texte_clean[i] = texte[j];
            i++;
        }
        // On remplace par défaut les espaces par des ;
        else if (texte[j] == ' ')
        {
            texte_clean[i] = ';';
            i++;
        }
        else
        {
            g_list_free(list);
            free(texte_clean);
            return NULL;
        }
    }
    texte_clean[i] = 0;
    if (i == 0)
    {
        free(texte_clean);
        return list;
    }
    
    // On parcours chaque numéro et on les ajoute à la liste.
    i = 0;
    do
    {
        while ((texte_clean[i] == ';') && (texte_clean[i] != 0))
            i++;
        if (texte_clean[i] != 0)
        {
            j = i+1;
            while ((texte_clean[j] != ';') && (texte_clean[j] != 0))
                j++;
            j--;
            // Il y a quelque chose à faire
            if ((j > i) || (texte_clean[i] != ';'))
            {
                char            *tmp = malloc(sizeof(char)*(j-i+2));
                char            *fake = malloc(sizeof(char)*(j-i+2));
                unsigned int    debut, fin, pas;
                
                BUGMSG(tmp, NULL, gettext("Erreur d'allocation mémoire.\n"));
                BUGMSG(fake, NULL, gettext("Erreur d'allocation mémoire.\n"));
                
                strncpy(tmp, texte_clean+i, j-i+1);
                tmp[j-i+1] = 0;
                
                // Si c'est du format debut-fin/pas
                if (sscanf(tmp, "%u-%u/%u%s", &debut, &fin, &pas, fake) == 3)
                {
                    for (i=debut;i<=fin;i=i+pas)
                        BUG(common_selection_ajout_nombre(i, &list) == 0, NULL);
                }
                // Si c'est du format debut-fin
                else if (sscanf(tmp, "%u-%u%s", &debut, &fin, fake) == 2)
                {
                    for (i=debut;i<=fin;i++)
                        BUG(common_selection_ajout_nombre(i, &list) == 0, NULL);
                }
                // Si c'est du format nombre simple
                else if (sscanf(tmp, "%u%s", &debut, fake) == 1)
                    BUG(common_selection_ajout_nombre(debut, &list) == 0, NULL);
                else
                {
                    free(tmp);
                    free(fake);
                    g_list_free(list);
                    free(texte_clean);
                    return NULL;
                }
                
                free(tmp);
                free(fake);
            }
            i = j+1;
        }
    }
    while (i < strlen(texte_clean));
    
    free(texte_clean);
    return list;
}


GList *common_selection_converti_numeros_en_noeuds(GList *liste_numeros, Projet *projet)
/* Description : Renvoie sous forme d'une liste de noeuds la liste des numéros.
 * Paramètres : GList *liste_numeros : la liste des numéros à convertir en liste de noeuds,
 *              Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : Un pointeur vers une liste des noeuds
 *   Échec : NULL en cas de problème.
 */
{
    GList   *liste_noeuds = NULL;
    GList   *list_parcours;
    
    if (liste_numeros != NULL)
    {
        list_parcours = liste_numeros;
        do
        {
            unsigned int    numero = GPOINTER_TO_UINT(list_parcours->data);
            EF_Noeud        *noeud = EF_noeuds_cherche_numero(projet, numero);
            
            if (noeud == NULL)
            {
                g_list_free(liste_noeuds);
                return NULL;
            }
            else
                liste_noeuds = g_list_append(liste_noeuds, noeud);
            
            list_parcours = g_list_next(list_parcours);
        }
        while (list_parcours != NULL);
    }
    
    return liste_noeuds;
}


GList *common_selection_converti_numeros_en_barres(GList *liste_numeros, Projet *projet)
/* Description : Renvoie sous forme d'une liste de barres la liste des numéros.
 * Paramètres : GList *liste_numeros : la liste des numéros à convertir en liste de barres,
 *              Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : Un pointeur vers une liste des barres
 *   Échec : NULL en cas de problème.
 */
{
    GList   *list_parcours;
    GList   *liste_barres = NULL;
    
    if (liste_numeros != NULL)
    {
        list_parcours = liste_numeros;
        do
        {
            unsigned int    numero = GPOINTER_TO_UINT(list_parcours->data);
            Beton_Barre     *barre = _1992_1_1_barres_cherche_numero(projet, numero);
            
            if (barre == NULL)
            {
                g_list_free(liste_barres);
                return NULL;
            }
            else
                liste_barres = g_list_append(liste_barres, barre);
            list_parcours = g_list_next(list_parcours);
        }
        while (list_parcours != NULL);
    }
    
    return liste_barres;
}


char *common_selection_converti_noeuds_en_texte(GList *liste_noeuds)
/* Description : Renvoie sous forme de texte une liste de noeuds.
 * Paramètres : GList *liste_noeuds : la liste des noeuds à convertir en texte,
 * Valeur renvoyée :
 *   Succès : le texte correspondant.
 *   Échec : NULL en cas de problème.
 */
{
    char        *tmp = NULL, *tmp2 = NULL;
    
    if (liste_noeuds != NULL)
    {
        GList       *list_parcours;
        EF_Noeud    *noeud;
        
        list_parcours = liste_noeuds;
        noeud = list_parcours->data;
        BUGMSG(tmp = g_strdup_printf("%d", noeud->numero), NULL, gettext("Erreur d'allocation mémoire.\n"));
        if (g_list_next(list_parcours) != NULL)
        {
            list_parcours = g_list_next(list_parcours);
            do
            {
                noeud = list_parcours->data;
                BUGMSG(tmp2 = g_strdup_printf("%s; %d", tmp, noeud->numero), NULL, gettext("Erreur d'allocation mémoire.\n"));
                free(tmp);
                tmp = tmp2;
                tmp2 = NULL;
                list_parcours = g_list_next(list_parcours);
            }
            while (list_parcours != NULL);
        }
        return tmp;
    }
    else
        return NULL;
}


char *common_selection_converti_barres_en_texte(GList *liste_barres)
/* Description : Renvoie sous forme de texte une liste de barres.
 * Paramètres : GList *liste_barres : la liste des barres à convertir en texte,
 * Valeur renvoyée :
 *   Succès : le texte correspondant.
 *   Échec : NULL en cas de problème.
 */
{
    char        *tmp, *tmp2;
    
    if (liste_barres != NULL)
    {
        GList       *list_parcours;
        Beton_Barre *barre;
        
        list_parcours = liste_barres;
        barre = list_parcours->data;
        BUGMSG(tmp = g_strdup_printf("%u", barre->numero), NULL, gettext("Erreur d'allocation mémoire.\n"));
        if (g_list_next(list_parcours) != NULL)
        {
            list_parcours = g_list_next(list_parcours);
            do
            {
                barre = list_parcours->data;
                BUGMSG(tmp2 = g_strdup_printf("%s; %u", tmp, barre->numero), NULL, gettext("Erreur d'allocation mémoire.\n"));
                free(tmp);
                tmp = tmp2;
                tmp2 = NULL;
                list_parcours = g_list_next(list_parcours);
            }
            while (list_parcours != NULL);
        }
        return tmp;
    }
    else
        return NULL;
}
