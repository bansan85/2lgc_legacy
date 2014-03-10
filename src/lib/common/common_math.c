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
#include <math.h>
#include <gmodule.h>
#include "common_math.h"
#include "common_projet.h"
#include "common_erreurs.h"


double
common_math_arrondi_nombre (double nombre)
/**
 * \brief Arrondi un nombre en supprimant la partie négligeable
 *        (#ERR_MIN).  L'algorithme est perfectible puisque lors de
 *        l'arrondi, une nouvelle imprécision apparait et certains nombres
 *        peuvent être arrondi en 1.09999999 ou -23.000000001.
 * \param nombre : le nombre à arrondir.
 * \return Le nombre arrondi.
 */
{
  double puissance;
  
  if (ERR (nombre, 0.))
    return 0.;
  puissance = ERREUR_RELATIVE_PUISSANCE - ceil (log10 (ABS (nombre)));
  nombre = nombre * pow (10, puissance);
  modf (nombre, &nombre);
  
  return nombre / pow (10., puissance);
}


void
common_math_arrondi_triplet (cholmod_triplet *triplet)
/**
 * \brief Arrondi un triplet en supprimant la partie négligeable
 *        (#ERR_MIN).
 * \param triplet : la variable triplet à arrondir.
 * \return Rien.
 */
{
  double       *ax;
  unsigned int  i;
  
  ax = (double *) triplet->x;
  for (i = 0; i < triplet->nnz; i++)
    ax[i] = common_math_arrondi_nombre (ax[i]);
  
  return;
}


void
common_math_arrondi_sparse (cholmod_sparse *sparse)
/**
 * \brief Arrondi un sparse en supprimant la partie négligeable
 *        (#ERR_MIN).
 * \param sparse : la matrice sparse à arrondir.
 * \return Rien.
 */
{
  double      *ax;
  unsigned int i;
  
  ax = (double *) sparse->x;
  for (i = 0; i < sparse->nzmax; i++)
    ax[i] = common_math_arrondi_nombre (ax[i]);
  
  return;
}


void
common_math_double_to_char (double nombre,
                            char  *dest,
                            int    decimales)
/**
 * \brief Converti un nombre double en char *.
 *        Dest doit déjà être alloué. 30 caractères devrait être suffisant.
 * \param nombre : nombre à convertir,
 * \param dest : nombre converti,
 * \param decimales : nombre de décimales au maximum.
 * \return Rien.
 */
{
/*  double  test;
  int   width;*/
  
  if (decimales > 15)
    decimales = 15;
  
/*  // Si le nombre est supérieur à 1e15, on affiche sous forme scientifique
  if (ABS(nombre) > 1e15)
  {
    for (width = 0; width<=decimales; width++)
    {
      sprintf(dest, "%.*le", width, nombre);
      sscanf(dest, "%le", &test);
      if ((fabs(nombre)*0.999999999999999 <= fabs(test)) && (fabs(test) <= fabs(nombre)*1.000000000000001))
        break;
    }
  }
  // Sinon on affiche sous forme normale
  else
  {
    for (width = 0; width<=decimales; width++)
    {
      sprintf(dest, "%.*lf", width, nombre);
      sscanf(dest, "%lf", &test);
      if ((fabs(nombre)*0.999999999999999 <= fabs(test)) && (fabs(test) <= fabs(nombre)*1.000000000000001))
        break;
    }
  }*/
  sprintf (dest, "%.*lf", decimales, nombre);
  
  return;
}


void
conv_f_c (Flottant nombre,
          char    *dest,
          int      decimales)
/**
 * \brief Converti un nombre double en char *.
 *        Dest doit déjà être alloué. 30 caractères devrait être suffisant.
 * \param nombre : nombre à convertir,
 * \param dest : nombre converti,
 * \param decimales : nombre de décimales au maximum.
 * \return Rien.
 */
{
  double test, f;
  
  if (decimales > 15)
    decimales = 15;
  f = m_g (nombre);
  
  // Si le nombre est supérieur à 1e15, on affiche sous forme scientifique
  switch (nombre.type)
  {
    case FLOTTANT_ORDINATEUR :
    {
      sprintf (dest, "%.*lf", decimales, f);
      break;
    }
    // Dans le cas d'un flottant utilisateur, on affiche toutes les décimales
    // afin d'afficher le nombre exact qu'il a saisi afin d'éviter qu'il pense
    // que des informations ont été tronquées.
    case FLOTTANT_UTILISATEUR :
    {
      int width;
      
      if (ABS (f) > 1e15)
      {
        for (width = 0; width <= 15; width++)
        {
          sprintf (dest, "%.*le", width, f);
          sscanf (dest, "%le", &test);
          if ((fabs (f) * 0.999999999999999 <= fabs (test)) &&
              (fabs (test) <= fabs (f) * 1.000000000000001))
            break;
        }
      }
      // Sinon on affiche sous forme normale
      else
      {
        for (width = 0; width <= 15; width++)
        {
          sprintf (dest, "%.*lf", width, f);
          sscanf (dest, "%lf", &test);
          if ((fabs (f) * 0.999999999999999 <= fabs (test)) &&
              (fabs (test) <= fabs (f) * 1.000000000000001))
            break;
        }
      }
      sprintf (dest, "%.*lf", MAX (width, decimales), f);
      break;
    }
    default :
    {
      FAILCRIT ( , (gettext ("Type de nombre est inconnu.\n"));)
      break;
    }
  }
  
  return;
}


double
m_g (Flottant f)
/**
 * \brief Renvoie la valeur d'un flottant.
 * \param f : le nombre.
 * \return
 *   Succès : le nombre.\n
 *   Échec : NAN si
 *     - le type de f est inconnu.
 */
{
  switch (f.type)
  {
    case FLOTTANT_ORDINATEUR :
    case FLOTTANT_UTILISATEUR :
      return f.d;
    default :
    {
      FAILCRIT (NAN, (gettext("Type de nombre est inconnu.\n"));)
      break;
    }
  }
}


Flottant
m_f (double        f,
     Type_Flottant type)
/**
 * \brief Crée un nombre flottant.
 * \param f : nombre flottant,
 * \param type : type du flottant.
 * \return Le résultat.
 */
{
  Flottant retour;
  
  retour.type = type;
  switch (retour.type)
  {
    case FLOTTANT_ORDINATEUR :
    case FLOTTANT_UTILISATEUR :
    {
      retour.d = f;
      return retour;
    }
    default :
    {
      retour.d = NAN;
      FAILCRIT (retour, (gettext ("Type de nombre est inconnu.\n"));)
      break;
    }
  }
}


Flottant
m_add_f (Flottant f1,
         Flottant f2)
/* Description : Additionne deux flottants.
 * Paramètres : Flottant f1 : nombre 1,
 *        Flottant f2 : nombre 2.
 * Valeur renvoyée : Le résultat.
 */
{
  Flottant retour;
  
  if ((f1.type == FLOTTANT_UTILISATEUR) && (f2.type == FLOTTANT_UTILISATEUR))
    retour.type = FLOTTANT_UTILISATEUR;
  else
    retour.type = FLOTTANT_ORDINATEUR;
  retour.d = m_g (f1) + m_g (f2);
  
  return retour;
}


Flottant
m_sub_f (Flottant f1,
         Flottant f2)
/**
 * \brief Soustrait deux flottants.
 * \param f1 : nombre 1,
 * \param f2 : nombre 2.
 * \return Le résultat,
 */
{
  Flottant   retour;
  
  if ((f1.type == FLOTTANT_UTILISATEUR) && (f2.type == FLOTTANT_UTILISATEUR))
    retour.type = FLOTTANT_UTILISATEUR;
  else
    retour.type = FLOTTANT_ORDINATEUR;
  retour.d = m_g (f1) - m_g (f2);
  
  return retour;
}


Flottant
m_div_d (Flottant f,
         double   d)
/**
 * \brief Divise un Flottant par un double.
 * \param f : nombre 1,
 * \param d : nombre 2.
 * \return Le résultat.
 */
{
  Flottant retour;
  
  retour.type = FLOTTANT_ORDINATEUR;
  retour.d = m_g(f) / d;
  
  return retour;
}


Flottant
m_dot_f (Flottant f1,
         Flottant f2)
/**
 * \brief Multiplie deux flottants.
 * \param f1 : nombre 1,
 * \param f2 : nombre 2.
 * \return Le résultat,
 */
{
  Flottant retour;
  
  if ((f1.type == FLOTTANT_UTILISATEUR) && (f2.type == FLOTTANT_UTILISATEUR))
    retour.type = FLOTTANT_UTILISATEUR;
  else
    retour.type = FLOTTANT_ORDINATEUR;
  retour.d = m_g (f1) * m_g (f2);
  
  return retour;
}


Flottant
m_dot_d (Flottant f,
         double   d)
/**
 * \brief Multiplie un Flottant avec un double.
 * \param f : nombre 1,
 * \param d : nombre 2.
 * \return Le résultat.
 */
{
  Flottant retour;
  
  retour.type = FLOTTANT_ORDINATEUR;
  retour.d = m_g (f) * d;
  
  return retour;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
