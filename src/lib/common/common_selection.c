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
            free(list);
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
                    free(list);
                    return NULL;
                }
                
                free(tmp);
                free(fake);
            }
            i = j+1;
        }
    }
    while (i < strlen(texte_clean));
    
    return list;
}
