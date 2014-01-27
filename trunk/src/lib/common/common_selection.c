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
#include <gmodule.h>

#include "1990_action.h"
#include "common_projet.h"
#include "EF_noeuds.h"
#include "EF_charge.h"
#include "1992_1_1_barres.h"
#include "common_erreurs.h"

gboolean
common_selection_ajout_nombre (void      *data,
                               GList    **liste,
                               Type_Liste type,
                               Projet    *p)
/**
 * \brief Ajoute un nombre à la liste chainée.
 * \param data : donnée à ajouter,
 * \param liste : liste où la donnée doit être ajoutée,
 * \param type : nature de la donnée data,
 * \param p : la variable projet, doit être différent de NULL uniquement si
 *            le type vaut #LISTE_CHARGES.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - liste == NULL,
 *     - p == NULL && type == LISTE_CHARGES
 */
{
  GList  *list_parcours;
  Action *action = NULL;
  
  BUGMSG (liste, FALSE, gettext ("Paramètre %s incorrect.\n"), "liste")
  BUGMSG ((p) || (type != LISTE_CHARGES),
          FALSE,
          gettext ("Paramètre %s incorrect.\n"), "projet")
  
  if (type == LISTE_CHARGES)
    BUG (action = EF_charge_action (p, data), FALSE)
  
  if (*liste == NULL)
  {
    switch (type)
    {
      case LISTE_UINT :
      case LISTE_NOEUDS :
      case LISTE_BARRES :
      case LISTE_CHARGES :
      {
        *liste = g_list_append (*liste, data);
        return TRUE;
      }
      default :
      {
        BUGMSG (NULL,
                FALSE,
                gettext ("Le type %d de la liste est inconnu.\n"), type)
      }
    }
  }
  
  list_parcours = *liste;
  do
  {
    switch (type)
    {
      case LISTE_UINT :
      {
        unsigned int nombre_liste, nombre;
        
        nombre_liste = GPOINTER_TO_UINT (list_parcours->data);
        nombre = GPOINTER_TO_UINT (data);
        
        if (nombre_liste == nombre)
          return TRUE;
        else if (nombre_liste > nombre)
        {
          *liste = g_list_insert_before (*liste, list_parcours, data);
          return TRUE;
        }
        break;
      }
      case LISTE_NOEUDS :
      {
        EF_Noeud *noeud_liste, *noeud;
        
        noeud_liste = list_parcours->data;
        noeud = data;
        
        if (noeud_liste->numero == noeud->numero)
          return TRUE;
        else if (noeud_liste->numero > noeud->numero)
        {
          *liste = g_list_insert_before (*liste, list_parcours, data);
          return TRUE;
        }
        break;
      }
      case LISTE_BARRES :
      {
        EF_Barre *barre_liste, *barre;
        
        barre_liste = list_parcours->data;
        barre = data;
        
        if (barre_liste->numero == barre->numero)
          return TRUE;
        else if (barre_liste->numero > barre->numero)
        {
          *liste = g_list_insert_before (*liste, list_parcours, data);
          return TRUE;
        }
        break;
      }
      case LISTE_CHARGES :
      {
        Charge *charge_liste, *charge;
        Action *action_en_cours;
        
        charge_liste = list_parcours->data;
        BUG (action_en_cours = EF_charge_action (p, charge_liste), FALSE)
        charge = data;
        
        if ((action_en_cours == action) && (charge_liste == charge))
          return TRUE;
        else if ((g_list_index (p->actions, action_en_cours) >
                                          g_list_index (p->actions, action)) ||
                 ((action_en_cours == action) &&
                 (g_list_index (_1990_action_charges_renvoie (action_en_cours),
                                                               charge_liste) >=
                  g_list_index (_1990_action_charges_renvoie (action_en_cours),
                                                                     charge))))
        {
          *liste = g_list_insert_before (*liste, list_parcours, data);
          return TRUE;
        }
        break;
      }
      default :
      {
        BUGMSG (NULL,
                FALSE,
                gettext ("Le type %d de la liste est inconnu.\n"), type)
      }
    }
    list_parcours = g_list_next (list_parcours);
  } while (list_parcours != NULL);
  
  switch (type)
  {
    case LISTE_UINT :
    {
      *liste = g_list_append (*liste, GUINT_TO_POINTER (data));
      return TRUE;
    }
    case LISTE_NOEUDS :
    case LISTE_BARRES :
    case LISTE_CHARGES :
    {
      *liste = g_list_append (*liste, data);
      return TRUE;
    }
    default :
    {
      BUGMSG (NULL,
              FALSE,
              gettext ("Le type %d de la liste est inconnu.\n"), type)
    }
  }
}


GList *
common_selection_renvoie_numeros (const char *texte)
/**
 * \brief Renvoie une liste de numéros sous forme de texte. Par exemple, le
 *        texte "1;2;3-4;6-9;10-20/2" donne les numéros
 *        "1 2 3 4 6 7 8 9 10 12 14 16 18 20" via la fonction GUINT_TO_POINTER.
 *        La virgule "," est considérée comme un point virgule ";" et les
 *        espaces négligés.
 * \param texte : le texte à convertir en numéros.
 * \return
 *   Succès : Un pointeur vers une liste de numéros triée.\n
 *   Échec : NULL :
 *     - Erreur d'allocation mémoire,
 *     - Format de texte illisible.
 */
{
  char        *texte_clean;
  GList       *list;
  unsigned int i, j;
  
  if (texte == NULL)
    return NULL;
  
  BUGMSG (texte_clean = malloc (sizeof (char) * (strlen (texte) + 1)),
          NULL,
          gettext ("Erreur d'allocation mémoire.\n"))
  
  // On vérifie si le texte contient bien une liste correcte de numéros
  i=0;
  for (j = 0; j < strlen (texte); j++)
  {
    // Les caractères autorisées sont ,;-/0123456789
    if ((texte[j] == ';') ||
        (texte[j] == '-') ||
        (texte[j] == '/') ||
        (('0' <= texte[j]) && (texte[j] <= '9')))
    {
      texte_clean[i] = texte[j];
      i++;
    }
    else if (texte[j] == ',')
    {
      texte_clean[i] = ';';
      i++;
    }
    // On remplace par défaut les espaces par des ;
    else if (texte[j] == ' ')
    {
      texte_clean[i] = ';';
      i++;
    }
    // En cas de caractères inconnus, on ne fait rien
    else
    {
      free (texte_clean);
      return NULL;
    }
  }
  
  texte_clean[i] = 0;
  if (i == 0)
  {
    free (texte_clean);
    return NULL;
  }
  
  list = NULL;
  
  // On parcours chaque numéro et on les ajoute à la liste.
  i = 0;
  do
  {
    while ((texte_clean[i] == ';') && (texte_clean[i] != 0))
      i++;
    if (texte_clean[i] != 0)
    {
      j = i + 1;
      while ((texte_clean[j] != ';') && (texte_clean[j] != 0))
        j++;
      j--;
      // Il y a quelque chose à faire
      if ((j > i) || (texte_clean[i] != ';'))
      {
        char        *tmp = malloc (sizeof (char) * (j - i + 2));
        char        *fake = malloc (sizeof (char) * (j - i + 2));
        unsigned int debut, fin, pas;
        
        BUGMSG (tmp, NULL, gettext ("Erreur d'allocation mémoire.\n"))
        BUGMSG (fake, NULL, gettext ("Erreur d'allocation mémoire.\n"))
        
        strncpy (tmp, texte_clean + i, j - i + 1);
        tmp[j - i + 1] = 0;
        
        // Si c'est du format debut-fin/pas
        if (sscanf (tmp, "%u-%u/%u%s", &debut, &fin, &pas, fake) == 3)
        {
          for (i = debut; i <= fin; i = i + pas)
            BUG (common_selection_ajout_nombre (GUINT_TO_POINTER (i),
                                                &list,
                                                LISTE_UINT,
                                                NULL),
                 NULL)
        }
        // Si c'est du format debut-fin
        else if (sscanf (tmp, "%u-%u%s", &debut, &fin, fake) == 2)
        {
          for (i = debut; i <= fin; i++)
            BUG (common_selection_ajout_nombre (GUINT_TO_POINTER (i),
                                                &list,
                                                LISTE_UINT,
                                                NULL),
                 NULL)
        }
        // Si c'est du format nombre simple
        else if (sscanf (tmp, "%u%s", &debut, fake) == 1)
          BUG (common_selection_ajout_nombre (GUINT_TO_POINTER (debut),
                                              &list,
                                              LISTE_UINT,
                                              NULL),
               NULL)
        // Le format est inconnu.
        else
        {
          free (tmp);
          free (fake);
          g_list_free (list);
          free (texte_clean);
          return NULL;
        }
        
        free (tmp);
        free (fake);
      }
      i = j + 1;
    }
  }
  while (i < strlen (texte_clean));
  
  free (texte_clean);
  return list;
}


GList *
common_selection_numeros_en_noeuds (GList  *liste_numeros,
                                    Projet *p)
/**
 * \brief Renvoie sous forme d'une liste de noeuds la liste des numéros.
 * \param liste_numeros : liste des numéros (sous format GPOINTER_TO_UINT) à
 *        convertir en liste de noeuds,
 * \param p : la variable projet.
 * \return
 *   Succès : Un pointeur vers une liste des noeuds.\n
 *   Échec : NULL :
 *     - un des noeuds est introuvable.
 */
{
  GList *liste_noeuds = NULL;
  GList *list_parcours;
  
  if (liste_numeros != NULL)
  {
    list_parcours = liste_numeros;
    do
    {
      unsigned int numero = GPOINTER_TO_UINT (list_parcours->data);
      EF_Noeud    *noeud = EF_noeuds_cherche_numero (p, numero, FALSE);
      
      if (noeud == NULL)
      {
        g_list_free (liste_noeuds);
        return NULL;
      }
      else
        liste_noeuds = g_list_append (liste_noeuds, noeud);
      
      list_parcours = g_list_next (list_parcours);
    }
    while (list_parcours != NULL);
  }
  
  return liste_noeuds;
}


GList *
common_selection_numeros_en_barres (GList  *liste_numeros,
                                    Projet *p)
/**
 * \brief Renvoie sous forme d'une liste de barres la liste des numéros.
 * \param liste_numeros : la liste des numéros (sous format GPOINTER_TO_UINT) à
 *        convertir en liste de barres,
 * \param p : la variable projet.
 * \return
 *   Succès : Un pointeur vers une liste des barres.\n
 *   Échec : NULL :
 *     - une des barres est introuvable.
 */
{
  GList *list_parcours;
  GList *liste_barres = NULL;
  
  if (liste_numeros != NULL)
  {
    list_parcours = liste_numeros;
    do
    {
      unsigned int numero = GPOINTER_TO_UINT (list_parcours->data);
      EF_Barre    *barre = _1992_1_1_barres_cherche_numero (p, numero, FALSE);
      
      if (barre == NULL)
      {
        g_list_free (liste_barres);
        return NULL;
      }
      else
        liste_barres = g_list_append (liste_barres, barre);
      
      list_parcours = g_list_next (list_parcours);
    }
    while (list_parcours != NULL);
  }
  
  return liste_barres;
}


char *
common_selection_noeuds_en_texte (GList *liste_noeuds)
/**
 * \brief Renvoie sous forme de texte une liste de noeuds.
 * \param liste_noeuds : la liste des noeuds à convertir en texte.
 * \return
 *   Succès : le texte correspondant.\n
 *   Échec : NULL :
 *     - Erreur d'allocation mémoire,
 */
{
  char *tmp = NULL, *tmp2 = NULL;
  
  if (liste_noeuds != NULL)
  {
    GList    *list_parcours;
    EF_Noeud *noeud;
    
    list_parcours = liste_noeuds;
    noeud = list_parcours->data;
    BUGMSG (tmp = g_strdup_printf ("%d", noeud->numero),
            NULL,
            gettext ("Erreur d'allocation mémoire.\n"))
    if (g_list_next (list_parcours) != NULL)
    {
      list_parcours = g_list_next (list_parcours);
      do
      {
        noeud = list_parcours->data;
        BUGMSG (tmp2 = g_strdup_printf ("%s;%d", tmp, noeud->numero),
                NULL,
                gettext ("Erreur d'allocation mémoire.\n"))
        free (tmp);
        tmp = tmp2;
        tmp2 = NULL;
        list_parcours = g_list_next (list_parcours);
      }
      while (list_parcours != NULL);
    }
  }
  else
    BUGMSG (tmp = g_strdup_printf (" "),
            NULL,
            gettext ("Erreur d'allocation mémoire.\n"))
  
  return tmp;
}


char *
common_selection_barres_en_texte (GList *liste_barres)
/**
 * \brief Renvoie sous forme de texte une liste de barres.
 * \param liste_barres : la liste des barres à convertir en texte.
 * \return
 *   Succès : le texte correspondant.\n
 *   Échec : NULL :
 *     - Erreur d'allocation mémoire.
 */
{
  char *tmp, *tmp2;
  
  if (liste_barres != NULL)
  {
    GList    *list_parcours;
    EF_Barre *barre;
    
    list_parcours = liste_barres;
    barre = list_parcours->data;
    BUGMSG (tmp = g_strdup_printf ("%u", barre->numero),
            NULL,
            gettext ("Erreur d'allocation mémoire.\n"))
    if (g_list_next (list_parcours) != NULL)
    {
      list_parcours = g_list_next (list_parcours);
      do
      {
        barre = list_parcours->data;
        BUGMSG (tmp2 = g_strdup_printf ("%s;%u", tmp, barre->numero),
                NULL,
                gettext ("Erreur d'allocation mémoire.\n"))
        free (tmp);
        tmp = tmp2;
        tmp2 = NULL;
        list_parcours = g_list_next (list_parcours);
      }
      while (list_parcours != NULL);
    }
  }
  else
    BUGMSG (tmp = g_strdup_printf (" "),
            NULL,
            gettext ("Erreur d'allocation mémoire.\n"))
  
  return tmp;
}


char *
common_selection_charges_en_texte (GList  *liste_charges,
                                   Projet *p)
/**
 * \brief Renvoie sous forme de texte une liste de charges.
 * \param liste_charges : la liste des charges à convertir en texte,
 * \param p : la variable projet.
 * \return
 *   Succès : le texte correspondant.\n
 *   Échec : NULL :
 *     - Erreur d'allocation mémoire.
 */
{
  char *tmp, *tmp2;
  
  if (liste_charges != NULL)
  {
    GList  *list_parcours, *list_parcours2;
    Charge *charge;
    Action *action = NULL;
    
    list_parcours = liste_charges;
    charge = list_parcours->data;
    
    BUG (action = EF_charge_action (p, charge), NULL)
    
    BUGMSG (tmp = g_strdup_printf ("%u:%u",
                                   g_list_index (p->actions, action),
                 g_list_index (_1990_action_charges_renvoie (action), charge)),
            NULL,
            gettext ("Erreur d'allocation mémoire.\n"))
    if (g_list_next (list_parcours) != NULL)
    {
      list_parcours = g_list_next(list_parcours);
      do
      {
        charge = list_parcours->data;
        // On cherche dans la liste des actions laquelle possède la charge.
        list_parcours2 = p->actions;
        while (list_parcours2 != NULL)
        {
          action = list_parcours2->data;
          
          if (g_list_find (_1990_action_charges_renvoie(action), charge) !=
                                                                          NULL)
            list_parcours2 = NULL;
           
          list_parcours2 = g_list_next (list_parcours2);
        }
        BUGMSG (tmp2 = g_strdup_printf ("%s;%u:%u",
                                        tmp,
                                        g_list_index (p->actions, action),
                 g_list_index (_1990_action_charges_renvoie (action), charge)),
                NULL,
                gettext ("Erreur d'allocation mémoire.\n"))
        free (tmp);
        tmp = tmp2;
        tmp2 = NULL;
        list_parcours = g_list_next (list_parcours);
      }
      while (list_parcours != NULL);
    }
  }
  else
    BUGMSG (tmp = g_strdup_printf (" "),
            NULL,
            gettext ("Erreur d'allocation mémoire.\n"))
  
  return tmp;
}
