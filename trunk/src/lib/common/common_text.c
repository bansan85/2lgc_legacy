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
#include <libintl.h>
#include <locale.h>
#include <gmodule.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>


void
show_warranty ()
/**
 * \brief Affiche les limites de la garantie (articles 15, 16 et 17 de la
 *        licence GPL).
 * \return Rien.
 */
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

void
show_help ()
/**
 * \brief Affiche l'aide lorsque l'utilisateur lance le programme avec l'option
 *        -h.
 * \return Rien.
 */
{
  printf (gettext ("Utilisation : codegui [OPTION]... [FILE]...\n"));
  printf (gettext ("Options :\n"));
  printf (gettext ("\t-h, --help : affiche le présent menu\n"));
  printf (gettext ("\t-w, --warranty : affiche les limites de garantie du logiciel\n"));
  return;
}


double
common_text_str_to_double (char    *texte,
                           double   val_min,
                           gboolean min_include,
                           double   val_max,
                           gboolean max_include)
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
{
  gboolean      min_check, max_check;
  char         *fake, *textebis;
  double        nombre;
  struct lconv *locale_conv;
  char         *retour;
  
  BUGMSG (fake = (char *) malloc (sizeof (char) * (strlen (texte) + 1)),
          NAN,
          gettext ("Erreur d'allocation mémoire.\n"))
  BUGMSG (textebis = g_strdup (texte),
          NAN,
          gettext ("Erreur d'allocation mémoire.\n"))
  
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
    BUGMSG (NULL,
            NAN,
            gettext ("Impossible de déterminer le caractère décimal.\n"))
  
  if (sscanf (textebis, "%lf%s", &nombre, fake) != 1)
  {
    min_check = FALSE;
    max_check = FALSE;
  }
  else
  {
    if (isinf (val_min) == -1)
      min_check = TRUE;
    else if ((min_include) && (ERR (nombre, val_min)))
      min_check = TRUE;
    else if (nombre > val_min)
      min_check = TRUE;
    else
      min_check = FALSE;
      
    if (isinf (val_max) == 1)
      max_check = TRUE;
    else if ((max_include) && (ERR (nombre, val_max)))
      max_check = TRUE;
    else if (nombre < val_max)
      max_check = TRUE;
    else
      max_check = FALSE;
  }
  free (fake);
  free (textebis);
  
  if ((min_check) && (max_check))
    return nombre;
  else
    return NAN;
}


char *
strcasestr_internal (const char *haystack,
                     const char *needle)
/**
 * \brief Est un équivalent de strcasestr avec gestion des accents et ne
 *        recherche l'aiguille qu'en début de chaque mot.
 * \param haystack : moule de foin,
 * \param needle : aiguille.
 * \return
 *   Succès : pointeur vers le texte.\n
 *   Échec : NULL.
 */
{
  #define CAR_UP   "ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÑÐÒÓÔÕÖØÙÚÛÜÝàáâãäåèçéêëìíîïðñòóôõöøùúûüýÿ"
  #define CAR_DOWN "aaaaaaæceeeeiiiindoooooouuuuyaaaaaaeceeeiiiionoooooouuuuyy"
  
  unsigned int i;
  char        *meule, *aiguille;
  char        *tmp;
  
  BUGMSG (haystack, NULL, gettext ("Paramètre %s incorrect.\n"), "haystack")
  BUGMSG (needle, NULL, gettext ("Paramètre %s incorrect.\n"), "needle")
  
  BUGMSG (meule = g_strdup (haystack),
          NULL,
          gettext ("Erreur d'allocation mémoire.\n"))
  BUGMSG (aiguille = g_strdup (needle),
          NULL,
          gettext ("Erreur d'allocation mémoire.\n"))
  
  for (i = 0; meule[i]; i++)
    meule[i] = tolower (meule[i]);
  for (i = 0; aiguille[i]; i++)
    aiguille[i] = tolower (aiguille[i]);
  
  for (i = 0; i < strlen (CAR_UP); i++)
  {
    char *retour;
    
    retour = strchr (meule, CAR_UP[i]);
    
    while (retour != NULL)
    {
      *retour = CAR_DOWN[i];
      retour = strchr (meule, CAR_UP[i]);
    }
    
    retour = strchr (aiguille, CAR_UP[i]);
    
    while (retour != NULL)
    {
      *retour = CAR_DOWN[i];
      retour = strchr (meule, CAR_UP[i]);
    }
  }
  
  tmp = strstr (meule, aiguille);
  // Si ce n'est pas au début, il faut vérifier que c'est au début d'un mot
  while ((tmp != NULL) &&
         (tmp != meule) &&
         (tmp[-1] != ' ') &&
         (tmp[-1] != '-'))
    tmp = strstr (tmp + 1, aiguille);
  
  free (meule);
  free (aiguille);
  
  return tmp;
  
}


char *
common_text_dependances (GList  *liste_noeuds,
                         GList  *liste_barres,
                         GList  *liste_charges,
                         Projet *p)
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
{
  char *retour = NULL;
  char *tmp;
  
  if (liste_noeuds != NULL)
  {
    tmp = common_selection_noeuds_en_texte (liste_noeuds);
    if (g_list_length (liste_noeuds) == 1)
      retour = g_strdup_printf ("%s : %s", gettext ("noeud"), tmp);
    else
      retour = g_strdup_printf ("%s : %s", gettext ("noeuds"), tmp);
    free(tmp);
  }
  if (liste_barres != NULL)
  {
    tmp = common_selection_barres_en_texte (liste_barres);
    if (retour == NULL)
    {
      if (g_list_length (liste_barres) == 1)
        retour = g_strdup_printf ("%s : %s", gettext ("barre"), tmp);
      else
        retour = g_strdup_printf ("%s : %s", gettext ("barres"), tmp);
    }
    else
    {
      char *tmp2;
      
      tmp2 = retour;
      if (g_list_length (liste_barres) == 1)
        retour = g_strdup_printf ("%s, %s : %s", tmp2, gettext ("barre"), tmp);
      else
        retour = g_strdup_printf ("%s, %s : %s",
                                  tmp2,
                                  gettext ("barres"),
                                  tmp);
      free (tmp2);
    }
    free (tmp);
  }
  if (liste_charges != NULL)
  {
    tmp = common_selection_charges_en_texte (liste_charges, p);
    if (retour == NULL)
    {
      if (g_list_length (liste_charges) == 1)
        retour = g_strdup_printf ("%s : %s", gettext ("charge"), tmp);
      else
        retour = g_strdup_printf ("%s : %s", gettext ("charges"), tmp);
    }
    else
    {
      char *tmp2;
      
      tmp2 = retour;
      if (g_list_length (liste_charges) == 1)
        retour = g_strdup_printf ("%s, %s : %s",
                                  tmp2,
                                  gettext ("charge"),
                                  tmp);
      else
        retour = g_strdup_printf ("%s, %s : %s",
                                  tmp2,
                                  gettext ("charges"),
                                  tmp);
      free (tmp2);
    }
    free (tmp);
  }
  
  tmp = retour;
  retour = g_strdup_printf (gettext ("et ces dépendances (%s)"), tmp);
  free (tmp);
  
  return retour;
}


char *
common_text_get_line (FILE *fichier)
/**
 * \brief Renvoie la ligne en cours de la variable fichier.
 * \param fichier : la variable fichier.
 * \return
 *   Succès : pointeur vers la ligne de texte.\n
 *   Échec : NULL :
 *     - fichier == NULL,
 *     - Erreur d'allocation mémoire.
 */
{
#define CUR_MAX 256
  char *buffer, *retour = NULL;
  
  BUGMSG (buffer = malloc (sizeof (char) * CUR_MAX),
          FALSE,
          gettext ("Erreur d'allocation mémoire.\n"))
  
  do
  {
    char *ligne_tmp;
    
    if ((fgets (buffer, CUR_MAX, fichier) == NULL) && (retour == NULL))
    {
      free (buffer);
      return NULL;
    }
    ligne_tmp = retour;
    if (ligne_tmp == NULL)
      retour = g_strconcat (buffer, NULL);
    else
    {
      retour = g_strconcat (ligne_tmp, buffer, NULL);
      free (ligne_tmp);
    }
    
    // On a atteint la fin de la ligne
    if (buffer[strlen (buffer) - 1] == '\n')
    {
      free (buffer);
      // Suppression du retour chariot
      retour[strlen (retour) - 1] = 0;
      return retour;
    }
  } while (TRUE);
#undef CUR_MAX
}
