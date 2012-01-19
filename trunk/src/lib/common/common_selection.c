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
#include <list.h>
#include <string.h>
#include "common_projet.h"
#include "EF_noeud.h"
#include "1992_1_1_barres.h"

int common_selection_ajout_nombre(unsigned int nombre, LIST *liste)
/* Description : ajoute un nombre à la liste chainée.
 * Paramètres : unsigned int nombre : nombre à ajouter,
 *              LIST *liste : liste où le nombre doit être ajouter.
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas d'échec.
 */
{
    unsigned int    *nombre_liste;
    LIST            *liste_tmp;
    
    list_mvfront(liste);
    if (list_size(liste) == 0)
    {
        list_insert_after(liste, &nombre, sizeof(nombre));
        return 0;
    }
    nombre_liste = list_curr(liste);
    
    if (*nombre_liste == nombre)
        return 0;
    else if (*nombre_liste > nombre)
    {
        list_insert_before(liste, &nombre, sizeof(nombre));
        return 0;
    }
    
    liste_tmp = list_mvnext(liste);
    
    while (liste_tmp != NULL)
    {
        nombre_liste = list_curr(liste);
        if (*nombre_liste == nombre)
            return 0;
        else if (*nombre_liste > nombre)
        {
            list_insert_before(liste, &nombre, sizeof(nombre));
            return 0;
        }
        liste_tmp = list_mvnext(liste);
    }
    
    list_insert_after(liste, &nombre, sizeof(nombre));
    return 0;
}


// Le format de texte est le suivant : 
// 1;2;3-4;6-9;10-20/2 donne les numéros :
// 1 2 3 4 6 7 8 9 10 12 14 16 18 20
LIST *common_selection_renvoie_numeros(const char *texte)
/* Description : Renvoie sous forme d'une liste de numéros les numéros sous forme de texte.
 * Paramètres : const char *texte : le texte à convertir en numéros
 * Valeur renvoyée :
 *   Succès : Un pointeur vers une liste de numéros triée
 *   Échec : NULL en cas de problème.
 */
{
    char            *texte_clean;
    LIST            *list;
    unsigned int    i, j;
    
    list = list_init();
    if (texte == NULL)
        return list;
    
    texte_clean = malloc(sizeof(char)*(strlen(texte)+1));
    
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
            list_free(list, LIST_DEALLOC);
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
                
                strncpy(tmp, texte_clean+i, j-i+1);
                tmp[j-i+1] = 0;
                
                // Si c'est du format debut-fin/pas
                if (sscanf(tmp, "%u-%u/%u%s", &debut, &fin, &pas, fake) == 3)
                {
                    for (i=debut;i<=fin;i=i+pas)
                        common_selection_ajout_nombre(i, list);
                }
                // Si c'est du format debut-fin
                else if (sscanf(tmp, "%u-%u%s", &debut, &fin, fake) == 2)
                {
                    for (i=debut;i<=fin;i++)
                        common_selection_ajout_nombre(i, list);
                }
                // Si c'est du format nombre simple
                else if (sscanf(tmp, "%u%s", &debut, fake) == 1)
                    common_selection_ajout_nombre(debut, list);
                else
                {
                    free(tmp);
                    free(fake);
                    list_free(list, LIST_DEALLOC);
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


LIST *common_selection_converti_numeros_en_noeuds(LIST *liste_numeros, Projet *projet)
/* Description : Renvoie sous forme d'une liste de noeuds la liste des numéros.
 * Paramètres : LIST *liste_numeros : la liste des numéros à convertir en liste de noeuds,
 *              Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : Un pointeur vers une liste des noeuds
 *   Échec : NULL en cas de problème.
 */
{
    LIST    *liste_noeuds = list_init();
    
    if ((liste_numeros != NULL) && (list_size(liste_numeros) != 0))
    {
        list_mvfront(liste_numeros);
        do
        {
            unsigned int    *numero = list_curr(liste_numeros);
            EF_Noeud        *noeud = EF_noeuds_cherche_numero(projet, *numero);
            
            if (noeud == NULL)
            {
                free(liste_noeuds);
                return NULL;
            }
            else
                list_insert_after(liste_noeuds, &noeud, sizeof(EF_Noeud**));
        }
        while (list_mvnext(liste_numeros) != NULL);
    }
    
    return liste_noeuds;
}


LIST *common_selection_converti_numeros_en_barres(LIST *liste_numeros, Projet *projet)
/* Description : Renvoie sous forme d'une liste de barres la liste des numéros.
 * Paramètres : LIST *liste_numeros : la liste des numéros à convertir en liste de barres,
 *              Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : Un pointeur vers une liste des barres
 *   Échec : NULL en cas de problème.
 */
{
    LIST    *liste_barres = list_init();
    
    if ((liste_numeros != NULL) && (list_size(liste_numeros) != 0))
    {
        list_mvfront(liste_numeros);
        do
        {
            unsigned int    *numero = list_curr(liste_numeros);
            Beton_Barre     *barre = _1992_1_1_barres_cherche_numero(projet, *numero);
            
            if (barre == NULL)
            {
                free(liste_barres);
                return NULL;
            }
            else
                list_insert_after(liste_barres, &barre, sizeof(Beton_Barre**));
        }
        while (list_mvnext(liste_numeros) != NULL);
    }
    
    return liste_barres;
}


char *common_selection_converti_noeuds_en_texte(LIST *liste_noeuds)
/* Description : Renvoie sous forme de texte une liste de noeuds.
 * Paramètres : LIST *liste_noeuds : la liste des noeuds à convertir en texte,
 * Valeur renvoyée :
 *   Succès : le texte correspondant.
 *   Échec : NULL en cas de problème.
 */
{
    char        *tmp;
    
    if ((liste_noeuds != NULL) && (list_size(liste_noeuds) != 0))
    {
        EF_Noeud    **noeud;
        
        tmp = malloc(sizeof(char)*(15)*list_size(liste_noeuds));
        list_mvfront(liste_noeuds);
        noeud = list_curr(liste_noeuds);
        sprintf(tmp, "%d", (*noeud)->numero);
        if (list_size(liste_noeuds) > 1)
        {
            list_mvnext(liste_noeuds);
            do
            {
                noeud = list_curr(liste_noeuds);
                sprintf(tmp, "%s; %d", tmp, (*noeud)->numero);
            }
            while (list_mvnext(liste_noeuds) != NULL);
        }
        return tmp;
    }
    else
        return NULL;
}


char *common_selection_converti_barres_en_texte(LIST *liste_barres)
/* Description : Renvoie sous forme de texte une liste de barres.
 * Paramètres : LIST *liste_barres : la liste des barres à convertir en texte,
 * Valeur renvoyée :
 *   Succès : le texte correspondant.
 *   Échec : NULL en cas de problème.
 */
{
    char        *tmp;
    
    if ((liste_barres != NULL) && (list_size(liste_barres) != 0))
    {
        Beton_Barre **barre;
        
        tmp = malloc(sizeof(char)*(15)*list_size(liste_barres));
        list_mvfront(liste_barres);
        barre = list_curr(liste_barres);
        sprintf(tmp, "%d", (*barre)->numero);
        if (list_size(liste_barres) > 1)
        {
            list_mvnext(liste_barres);
            do
            {
                barre = list_curr(liste_barres);
                sprintf(tmp, "%s; %d", tmp, (*barre)->numero);
            }
            while (list_mvnext(liste_barres) != NULL);
        }
        return tmp;
    }
    else
        return NULL;
}


