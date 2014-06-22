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

#include <locale>
#include <cmath>
#include <cfloat>

#include "common_math.hpp"
#include "common_projet.hpp"
#include "common_erreurs.hpp"
#include "common_text.hpp"


/**
 * \brief Détermine si un nombre est plus petit que le nombre minumum err_min.
 *        Emet un message d'erreur si le nombre est supérieur à err_min * 1e14.
 * \param calc : le nombre 1 (nombre calculé).
 * \param err_min : le nombre minimum.
 * \return true si fabs (calc) <= err_moy * 1e-14.
 */
bool
errmin (double calc,
        double err_min)
{
  INFO (fabs (calc) <= err_min * 1e14,
        true,
        (gettext ("Comparaison hors limite : %lf > %lf.\n"),
                  calc,
                  err_min * 1e14); )
  return fabs (calc) <= err_min;
}


/**
 * \brief Détermine si un nombre est plus petit que le nombre maximum err_max
 *        * 1e-14. Emet un message d'erreur si le nombre est supérieur à
 *        err_max.
 * \param calc : le nombre 1 (nombre calculé).
 * \param err_max : le nombre maximum.
 * \return true si fabs (calc) <= err_moy * 1e-14.
 */
bool
errmax (double calc,
        double err_max)
{
  INFO (fabs (calc) <= err_max,
        true,
        (gettext ("Comparaison hors limite : %lf > %lf.\n"),
                  calc,
                  err_max); )
  return fabs (calc) <= err_max * 1e-14;
}


/**
 * \brief Détermine si un nombre est plus petit que le nombre moyen err_moy
 *        * 1e-7. Emet un message d'erreur si le nombre est supérieur à
 *        err_moy * 1e-7.
 * \param calc : le nombre 1 (nombre calculé).
 * \param err_moy : le nombre moyen.
 * \return true si fabs (calc) <= err_moy * 1e-7.
 */
bool
errmoy (double calc,
        double err_moy)
{
  INFO (fabs (calc) <= err_moy * 1e7,
        true,
        (gettext ("Comparaison hors limite : %lf > %lf.\n"),
                  calc,
                  err_moy * 1e7); )
  return fabs (calc) <= err_moy * 1e-7;
}


/**
 * \brief Détermine si 2 nombres sont identiques.
 * \param calc : le nombre 1 (nombre calculé).
 * \param theo : le nombre 2 (nombre théorique).
 * \return true si les deux nombres sont identiques à 10^-14 prêt.
 */
bool
errrel (double calc,
        double theo)
{
  double err_max = std::max (fabs (calc), fabs (theo));
  
  return fabs (calc - theo) <= err_max * 1e-14;
}


/**
 * \brief Arrondi un nombre en supprimant la partie négligeable. L'algorithme
 *        est perfectible puisque lors de l'arrondi, une nouvelle imprécision
 *        apparait et certains nombres peuvent être arrondi en 1.09999999 ou
 *        -23.000000001.
 * \param nombre : le nombre à arrondir.
 * \return Le nombre arrondi.
 */
double
common_math_arrondi_nombre (double nombre)
{
  double puissance;
  
  if (fabs (nombre) < DBL_MIN)
  {
    return 0.;
  }
  puissance = ERREUR_RELATIVE_PUISSANCE - ceil (log10 (fabs (nombre)));
  nombre = nombre * pow (10, puissance);
  modf (nombre, &nombre);
  
  return nombre / pow (10., puissance);
}


/**
 * \brief Arrondi un triplet en supprimant la partie négligeable.
 * \param triplet : la variable triplet à arrondir.
 * \return Rien.
 */
void
common_math_arrondi_triplet (cholmod_triplet *triplet)
{
  double *ax;
  size_t  i;
  
  ax = (double *) triplet->x;
  for (i = 0; i < triplet->nnz; i++)
  {
    ax[i] = common_math_arrondi_nombre (ax[i]);
  }
  
  return;
}


/**
 * \brief Arrondi un sparse en supprimant la partie négligeable.
 * \param sparse : la matrice sparse à arrondir.
 * \return Rien.
 */
void
common_math_arrondi_sparse (cholmod_sparse *sparse)
{
  double *ax;
  size_t  i;
  
  ax = (double *) sparse->x;
  for (i = 0; i < sparse->nzmax; i++)
  {
    ax[i] = common_math_arrondi_nombre (ax[i]);
  }
  
  return;
}


/**
 * \brief Converti un nombre double en std::string.
 *        Dest doit déjà être alloué. 30 caractères devrait être suffisant.
 * \param nombre : nombre à convertir,
 * \param dest : nombre converti,
 * \param decimales : nombre de décimales au maximum.
 * \return Rien.
 */
void
common_math_double_to_string (double       nombre,
                              std::string *dest,
                              int8_t       decimales)
{
/*  double  test;
  int   width;*/
  
  if (decimales > 15)
  {
    decimales = 15;
  }
  
/*  // Si le nombre est supérieur à 1e15, on affiche sous forme scientifique
  if (fabs(nombre) > 1e15)
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
  *dest = format ("%.*lf", decimales, nombre);
  
  return;
}


/**
 * \brief Converti un nombre double en std::string.
 *        Dest doit déjà être alloué. 30 caractères devrait être suffisant.
 * \param nombre : nombre à convertir,
 * \param dest : nombre converti,
 * \param decimales : nombre de décimales au maximum.
 * \return Rien.
 */
void
conv_f_c (Flottant     nombre,
          std::string *dest,
          uint8_t      decimales)
{
  double f;
  
  if (decimales > 15)
  {
    decimales = 15;
  }
  f = m_g (nombre);
  
  // Si le nombre est supérieur à 1e15, on affiche sous forme scientifique
  switch (nombre.type)
  {
    case FLOTTANT_ORDINATEUR :
    {
      *dest = format ("%.*lf", decimales, f);
      break;
    }
    default :
    {
      FAILCRIT ( , (gettext ("Type de nombre est inconnu.\n")); )
      break;
    }
  }
  
  return;
}


/**
 * \brief Renvoie la valeur d'un flottant.
 * \param f : le nombre.
 * \return
 *   Succès : le nombre.\n
 *   Échec : NAN si
 *     - le type de f est inconnu.
 */
double
m_g (Flottant f)
{
  switch (f.type)
  {
    case FLOTTANT_ORDINATEUR :
    case FLOTTANT_UTILISATEUR :
      return f.d;
    default :
    {
      FAILCRIT (NAN, (gettext("Type de nombre est inconnu.\n")); )
      break;
    }
  }
}


/**
 * \brief Crée un nombre flottant.
 * \param f : nombre flottant,
 * \param type : type du flottant.
 * \return Le résultat.
 */
Flottant
m_f (double        f,
     Type_Flottant type)
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
      FAILCRIT (retour, (gettext ("Type de nombre est inconnu.\n")); )
      break;
    }
  }
}


/** Description : Additionne deux flottants.
 * Paramètres : Flottant f1 : nombre 1,
 *        Flottant f2 : nombre 2.
 * Valeur renvoyée : Le résultat.
 */
Flottant
m_add_f (Flottant f1,
         Flottant f2)
{
  Flottant retour;
  
  if ((f1.type == FLOTTANT_UTILISATEUR) && (f2.type == FLOTTANT_UTILISATEUR))
  {
    retour.type = FLOTTANT_UTILISATEUR;
  }
  else
  {
    retour.type = FLOTTANT_ORDINATEUR;
  }
  retour.d = m_g (f1) + m_g (f2);
  
  return retour;
}


/**
 * \brief Soustrait deux flottants.
 * \param f1 : nombre 1,
 * \param f2 : nombre 2.
 * \return Le résultat,
 */
Flottant
m_sub_f (Flottant f1,
         Flottant f2)
{
  Flottant   retour;
  
  if ((f1.type == FLOTTANT_UTILISATEUR) && (f2.type == FLOTTANT_UTILISATEUR))
  {
    retour.type = FLOTTANT_UTILISATEUR;
  }
  else
  {
    retour.type = FLOTTANT_ORDINATEUR;
  }
  retour.d = m_g (f1) - m_g (f2);
  
  return retour;
}


/**
 * \brief Divise un Flottant par un double.
 * \param f : nombre 1,
 * \param d : nombre 2.
 * \return Le résultat.
 */
Flottant
m_div_d (Flottant f,
         double   d)
{
  Flottant retour;
  
  retour.type = FLOTTANT_ORDINATEUR;
  retour.d = m_g(f) / d;
  
  return retour;
}


/**
 * \brief Multiplie deux flottants.
 * \param f1 : nombre 1,
 * \param f2 : nombre 2.
 * \return Le résultat,
 */
Flottant
m_dot_f (Flottant f1,
         Flottant f2)
{
  Flottant retour;
  
  if ((f1.type == FLOTTANT_UTILISATEUR) && (f2.type == FLOTTANT_UTILISATEUR))
  {
    retour.type = FLOTTANT_UTILISATEUR;
  }
  else
  {
    retour.type = FLOTTANT_ORDINATEUR;
  }
  retour.d = m_g (f1) * m_g (f2);
  
  return retour;
}


/**
 * \brief Multiplie un Flottant avec un double.
 * \param f : nombre 1,
 * \param d : nombre 2.
 * \return Le résultat.
 */
Flottant
m_dot_d (Flottant f,
         double   d)
{
  Flottant retour;
  
  retour.type = FLOTTANT_ORDINATEUR;
  retour.d = m_g (f) * d;
  
  return retour;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
