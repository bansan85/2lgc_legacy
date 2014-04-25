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
#include "common_projet.h"
#include "common_selection.h"
#include "common_erreurs.h"
#include "common_math.h"
#include "common_text.h"
#include <libintl.h>
#include <locale.h>
#include <gmodule.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <wchar.h>


/**
 * \brief Affiche les limites de la garantie (articles 15, 16 et 17 de la
 *        licence GPL).
 * \return Rien.
 */
void
show_warranty ()
{
  printf (gettext ("15. Disclaimer of Warranty.\n"));
  printf (gettext ("\n"));
  printf (gettext ("THERE IS NO WARRANTY FOR THE PROGRAM, TO THE EXTENT PERMITTED BY\n"));
  printf (gettext ("APPLICABLE LAW.  EXCEPT WHEN OTHERWISE STATED IN WRITING THE COPYRIGHT\n"));
  printf (gettext ("HOLDERS AND/OR OTHER PARTIES PROVIDE THE PROGRAM \"AS IS\" WITHOUT WARRANTY\n"));
  printf (gettext ("OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO,\n"));
  printf (gettext ("THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR\n"));
  printf (gettext ("PURPOSE.  THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE PROGRAM\n"));
  printf (gettext ("IS WITH YOU.  SHOULD THE PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF\n"));
  printf (gettext ("ALL NECESSARY SERVICING, REPAIR OR CORRECTION.\n"));
  printf (gettext ("\n"));
  printf (gettext ("16. Limitation of Liability.\n"));
  printf (gettext ("\n"));
  printf (gettext ("IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN WRITING\n"));
  printf (gettext ("WILL ANY COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO MODIFIES AND/OR CONVEYS\n"));
  printf (gettext ("THE PROGRAM AS PERMITTED ABOVE, BE LIABLE TO YOU FOR DAMAGES, INCLUDING ANY\n"));
  printf (gettext ("GENERAL, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE\n"));
  printf (gettext ("USE OR INABILITY TO USE THE PROGRAM (INCLUDING BUT NOT LIMITED TO LOSS OF\n"));
  printf (gettext ("DATA OR DATA BEING RENDERED INACCURATE OR LOSSES SUSTAINED BY YOU OR THIRD\n"));
  printf (gettext ("PARTIES OR A FAILURE OF THE PROGRAM TO OPERATE WITH ANY OTHER PROGRAMS),\n"));
  printf (gettext ("EVEN IF SUCH HOLDER OR OTHER PARTY HAS BEEN ADVISED OF THE POSSIBILITY OF\n"));
  printf (gettext ("SUCH DAMAGES.\n"));
  printf (gettext ("\n"));
  printf (gettext ("17. Interpretation of Sections 15 and 16.\n"));
  printf (gettext ("\n"));
  printf (gettext ("If the disclaimer of warranty and limitation of liability provided\n"));
  printf (gettext ("above cannot be given local legal effect according to their terms,\n"));
  printf (gettext ("reviewing courts shall apply local law that most closely approximates\n"));
  printf (gettext ("an absolute waiver of all civil liability in connection with the\n"));
  printf (gettext ("Program, unless a warranty or assumption of liability accompanies a\n"));
  printf (gettext ("copy of the Program in return for a fee.\n"));
  return;
}

/**
 * \brief Affiche l'aide lorsque l'utilisateur lance le programme avec l'option
 *        -h.
 * \return Rien.
 */
void
show_help ()
{
  printf (gettext ("Utilisation : codegui [OPTION]... [FILE]...\n"));
  printf (gettext ("Options :\n"));
  printf (gettext ("\t-h, --help : affiche le présent menu\n"));
  printf (gettext ("\t-w, --warranty : affiche les limites de garantie du logiciel\n"));
  return;
}


/**
 * \brief Sur la base d'une chaîne de caractères, renvoie un nombre flottant
 *        compris entre les valeurs val_min et val_max. S'il ne contient pas de
 *        nombre ou hors domaine, la valeur renvoyée est NAN.
 * \param texte : le texte à étudier,
 * \param val_min : borne inférieure,
 * \param min_include : le nombre de la borne inférieure est-il autorisé ?,
 * \param val_max : borne supérieure
 * \param max_include : le nombre de la borne supérieure est-il autorisé ?.
 * \return
 *   Succès : la valeur du nombre.\n
 *   Echec : NAN.
 */
double
common_text_str_to_double (char    *texte,
                           double   val_min,
                           gboolean min_include,
                           double   val_max,
                           gboolean max_include)
{
  gboolean      min_check, max_check;
  char         *fake, *textebis;
  double        nombre;
  struct lconv *locale_conv;
  char         *retour;
  
  BUGCRIT (fake = (char *) malloc (sizeof (char) * (strlen (texte) + 1)),
           NAN,
           (gettext ("Erreur d'allocation mémoire.\n")); )
  BUGCRIT (textebis = g_strdup (texte),
           NAN,
           (gettext ("Erreur d'allocation mémoire.\n"));
             free (fake); )
  
  // On remplace par la bonne décimale.
  locale_conv = localeconv ();
  if (strcmp (locale_conv->decimal_point, ".") == 0)
  {
    retour = strchr (textebis, ',');
    while (retour != NULL)
    {
      *retour = '.';
      retour = strchr (textebis, ',');
    }
  }
  else if (strcmp (locale_conv->decimal_point, ",") == 0)
  {
    retour = strchr (textebis, '.');
    while (retour != NULL)
    {
      *retour = ',';
      retour = strchr (textebis, '.');
    }
  }
  else
  {
    FAILCRIT (NAN,
              (gettext ("Impossible de déterminer le caractère décimal.\n"));
                free (fake);
                free (textebis); )
  }
  
  if (sscanf (textebis, "%lf%s", &nombre, fake) != 1)
  {
    min_check = FALSE;
    max_check = FALSE;
  }
  else
  {
    if (isinf (val_min) == -1)
    {
      min_check = TRUE;
    }
    else if ((min_include) && (errrel (nombre, val_min)))
    {
      min_check = TRUE;
    }
    else if (nombre > val_min)
    {
      min_check = TRUE;
    }
    else
    {
      min_check = FALSE;
    }
      
    if (isinf (val_max) == 1)
    {
      max_check = TRUE;
    }
    else if ((max_include) && (errrel (nombre, val_max)))
    {
      max_check = TRUE;
    }
    else if (nombre < val_max)
    {
      max_check = TRUE;
    }
    else
    {
      max_check = FALSE;
    }
  }
  free (fake);
  free (textebis);
  
  if ((min_check) && (max_check))
  {
    return nombre;
  }
  else
  {
    return NAN;
  }
}


/**
 * \brief Est un équivalent de strcasestr avec gestion des accents et ne
 *        recherche l'aiguille qu'en début de chaque mot.
 * \param haystack : moule de foin,
 * \param needle : aiguille.
 * \return
 *   Succès : pointeur vers le texte.\n
 *   Échec : NULL.
 */
wchar_t *
strcasestr_internal (const wchar_t *haystack,
                     const wchar_t *needle)
{
#define CAR_UP   L"ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÑÐÒÓÔÕÖØÙÚÛÜÝàáâãäåèçéêëìíîïðñòóôõöøùúûüýÿ" // NS (nsiqcppstyle)
#define CAR_DOWN L"aaaaaaæceeeeiiiindoooooouuuuyaaaaaaeceeeiiiionoooooouuuuyy"
  
  uint8_t  i;
  wchar_t *meule, *aiguille;
  wchar_t *tmp;
  
  BUGPARAM (haystack, "%p", haystack, NULL)
  BUGPARAM (needle, "%p", needle, NULL)
  
  BUGCRIT (meule = malloc (sizeof (wchar_t) * (wcslen (haystack) + 1)),
           NULL,
           (gettext ("Erreur d'allocation mémoire.\n")); )
  wcscpy (meule, haystack);
  BUGCRIT (aiguille = malloc (sizeof (wchar_t) * (wcslen (needle) + 1)),
           NULL,
           (gettext ("Erreur d'allocation mémoire.\n"));
             free (meule); )
  wcscpy (aiguille, needle);
  
  for (i = 0; meule[i]; i++)
  {
    meule[i] = tolower (meule[i]);
  }
  for (i = 0; aiguille[i]; i++)
  {
    aiguille[i] = tolower (aiguille[i]);
  }
  
  for (i = 0; i < wcslen (CAR_UP); i++)
  {
    wchar_t *retour;
    
    retour = wcschr (meule, CAR_UP[i]);
    
    while (retour != NULL)
    {
      *retour = CAR_DOWN[i];
      retour = wcschr (meule, CAR_UP[i]);
    }
    
    retour = wcschr (aiguille, CAR_UP[i]);
    
    while (retour != NULL)
    {
      *retour = CAR_DOWN[i];
      retour = wcschr (meule, CAR_UP[i]);
    }
  }
  
  tmp = wcsstr (meule, aiguille);
  // Si ce n'est pas au début, il faut vérifier que c'est au début d'un mot
  while ((tmp != NULL) &&
         (tmp != meule) &&
         (tmp[-1] != ' ') &&
         (tmp[-1] != '-'))
  {
    tmp = wcsstr (tmp + 1, aiguille);
  }
  
  free (meule);
  free (aiguille);
  
  return tmp;
  
}


/**
 * \brief Renvoie la liste des types d'éléments dépendants sous forme de texte.
 *        liste_noeuds et liste_barres viennent de
 *        #_1992_1_1_barres_cherche_dependances.
 * \param liste_noeuds : liste des noeuds,
 * \param liste_barres : liste des barres,
 * \param liste_charges : liste des charges.
 * \param p : la variable projet.
 * \return
 *   Succès : pointeur vers le texte.\n
 *   Échec : NULL :
 *     - Erreur d'allocation mémoire.
 */
// coverity[+alloc]
char *
common_text_dependances (GList  *liste_noeuds,
                         GList  *liste_barres,
                         GList  *liste_charges,
                         Projet *p)
{
  char *retour = NULL;
  char *tmp;
  
  if (liste_noeuds != NULL)
  {
    BUG (tmp = common_selection_noeuds_en_texte (liste_noeuds), NULL)
    if (g_list_length (liste_noeuds) == 1)
    {
      retour = g_strdup_printf ("%s : %s", gettext ("noeud"), tmp);
    }
    else
    {
      retour = g_strdup_printf ("%s : %s", gettext ("noeuds"), tmp);
    }
    BUGCRIT (retour,
             NULL,
             (gettext ("Erreur d'allocation mémoire.\n"));
               free (tmp); )
    free(tmp);
  }
  if (liste_barres != NULL)
  {
    BUG (tmp = common_selection_barres_en_texte (liste_barres), NULL)
    if (retour == NULL)
    {
      if (g_list_length (liste_barres) == 1)
      {
        retour = g_strdup_printf ("%s : %s", gettext ("barre"), tmp);
      }
      else
      {
        retour = g_strdup_printf ("%s : %s", gettext ("barres"), tmp);
      }
      BUGCRIT (retour, 
               NULL, 
               (gettext ("Erreur d'allocation mémoire.\n"));
                 free (tmp); )
    }
    else
    {
      char *tmp2;
      
      tmp2 = retour;
      if (g_list_length (liste_barres) == 1)
      {
        retour = g_strdup_printf ("%s, %s : %s", tmp2, gettext ("barre"), tmp);
      }
      else
      {
        retour = g_strdup_printf ("%s, %s : %s",
                                  tmp2,
                                  gettext ("barres"),
                                  tmp);
      }
      BUGCRIT (retour,
               NULL,
               (gettext ("Erreur d'allocation mémoire.\n"));
                 free (tmp);
                 free (tmp2); )
      free (tmp2);
    }
    free (tmp);
  }
  if (liste_charges != NULL)
  {
    BUG (tmp = common_selection_charges_en_texte (liste_charges, p), NULL)
    if (retour == NULL)
    {
      if (g_list_length (liste_charges) == 1)
      {
        retour = g_strdup_printf ("%s : %s", gettext ("charge"), tmp);
      }
      else
      {
        retour = g_strdup_printf ("%s : %s", gettext ("charges"), tmp);
      }
      BUGCRIT (retour,
               NULL,
               (gettext ("Erreur d'allocation mémoire.\n"));
                 free (tmp); )
    }
    else
    {
      char *tmp2;
      
      tmp2 = retour;
      if (g_list_length (liste_charges) == 1)
      {
        retour = g_strdup_printf ("%s, %s : %s",
                                  tmp2,
                                  gettext ("charge"),
                                  tmp);
      }
      else
      {
        retour = g_strdup_printf ("%s, %s : %s",
                                  tmp2,
                                  gettext ("charges"),
                                  tmp);
      }
      BUGCRIT (retour,
               NULL,
               (gettext ("Erreur d'allocation mémoire.\n"));
                 free (tmp);
                 free (tmp2); )
      free (tmp2);
    }
    free (tmp);
  }
  
  tmp = retour;
  BUGCRIT (retour = g_strdup_printf (gettext ("et ces dépendances (%s)"), tmp),
           NULL,
           (gettext ("Erreur d'allocation mémoire.\n"));
             free (tmp); )
  free (tmp);
  
  return retour;
}


/**
 * \brief Renvoie la ligne en cours de la variable fichier.
 * \param fichier : la variable fichier.
 * \return
 *   Succès : pointeur vers la ligne de texte.\n
 *   Échec : NULL :
 *     - fichier == NULL,
 *     - Erreur d'allocation mémoire.
 */
// coverity[+alloc]
wchar_t *
common_text_get_line (FILE *fichier)
{
  const uint16_t CUR_MAX = 256;
  wchar_t *buffer, *retour = NULL;
  
  BUGCRIT (buffer = malloc (sizeof (wchar_t) * (CUR_MAX + 1U)),
           NULL,
           (gettext ("Erreur d'allocation mémoire.\n")); )
  
  do
  {
    wchar_t *ligne_tmp;
    
    if ((fgetws (buffer, CUR_MAX, fichier) == NULL) && (retour == NULL))
    {
      free (buffer);
      return NULL;
    }
    ligne_tmp = retour;
    if (ligne_tmp == NULL)
    {
      BUGCRIT (retour = malloc (sizeof (wchar_t) * (wcslen (buffer) + 1)),
               NULL,
               (gettext ("Erreur d'allocation mémoire.\n"));
                 free (buffer); )
      wcscpy (retour, buffer);
    }
    else
    {
      BUGCRIT (retour = malloc (sizeof (wchar_t) *
                                   (wcslen (ligne_tmp) + wcslen (buffer) + 1)),
               NULL,
               (gettext ("Erreur d'allocation mémoire.\n"));
                 free (buffer);
                 free (ligne_tmp); )
      wcscpy (retour, ligne_tmp);
      wcscat (retour, buffer);
      free (ligne_tmp);
    }
    
    // On a atteint la fin de la ligne
    if (buffer[wcslen (buffer) - 1] == '\n')
    {
      free (buffer);
      // Suppression du retour chariot
      retour[wcslen (retour) - 1] = 0;
      
      return retour;
    }
  } while (TRUE);
}


/**
 * \brief Renvoie le texte sous format char *, y compris l'allocation mémoire.
 * \param texte : le texte à convertir.
 * \return
 *   Succès : pointeur vers le texte converti ou NULL si texte == NULL.\n
 *   Échec : NULL :
 *     - Erreur d'allocation mémoire.
 */
// coverity[+alloc]
char *
common_text_wcstostr_dup (const wchar_t *texte)
{
  char *tmp;
  
  if (texte == NULL)
  {
    return NULL;
  }
  
  BUGCRIT (tmp = malloc (sizeof (char) * (wcstombs (NULL, texte, 0) + 1)),
           NULL,
           (gettext ("Erreur d'allocation mémoire.\n")); )
  wcstombs (tmp, texte, wcstombs (NULL, texte, 0) + 1);
  
  return tmp;
}


/**
 * \brief Renvoie le texte sous format wchar_t *, y compris l'allocation
 *        mémoire.
 * \param texte : le texte à convertir.
 * \return
 *   Succès : pointeur vers le texte converti ou NULL si texte == NULL.\n
 *   Échec : NULL :
 *     - Erreur d'allocation mémoire.
 */
// coverity[+alloc]
wchar_t *
common_text_strtowcs_dup (const char *texte)
{
  wchar_t *tmp;
  
  if (texte == NULL)
  {
    return NULL;
  }
  
  BUGCRIT (tmp = malloc (sizeof (wchar_t) * (mbstowcs (NULL, texte, 0) + 1)),
           NULL,
           (gettext ("Erreur d'allocation mémoire.\n")); )
  mbstowcs (tmp, texte, mbstowcs (NULL, texte, 0) + 1);
  
  return tmp;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
