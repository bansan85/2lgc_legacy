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
#include <memory>
#include <string.h>
#include <cmath>
#include <algorithm>

#include "1990_ponderations.hpp"
#include "common_projet.hpp"
#include "common_erreurs.hpp"
#include "common_math.hpp"
#include "common_fonction.hpp"
#include "common_text.hpp"

/**
 * \brief Divise un tronçon en deux à la position coupure. Si la coupure est
 *        en dehors de la borne de validité actuelle de la fonction, les bornes
 *        s'en trouvent modifiées. Si la coupure correspond déjà à une jonction
 *        entre deux tronçons, la fonction ne fait rien et renvoie true. Si la
 *        coupure à lieu en dehors des bornes de la fonction, le nouvel
 *        intervalle est remplit par une fonction vide.
 * \param fonction : la variable contenant la fonction,
 * \param coupure : position de la coupure.
 * \return
 *   Succès : true.\n
 *   Échec : false :
 *     - fonction == NULL,
 *     - fonction->nb_troncons == 0,
 *     - en cas d'erreur d'allocation mémoire.
 */
bool
common_fonction_scinde_troncon (Fonction *fonction,
                                double    coupure)
{
  BUGPARAM (fonction, "%p", fonction, false)
  INFO (fonction->t.size () != 0,
        false,
        (gettext ("Impossible de scinder une fonction vide\n")); )
  
  // Si la coupure est égale au début du premier tronçon Alors
  //   Fin.
  if (errrel ((*fonction->t.begin ())->debut_troncon, coupure))
  {
    return true;
  }
  // Sinon Si la coupure est inférieure au début du premier troncon Alors
  //   Insertion d'un tronçon en première position.
  //   Initialisation de tous les coefficients à 0.
  else if (coupure < (*fonction->t.begin ())->debut_troncon)
  {
    Troncon *t_new = new Troncon;
    
    t_new->debut_troncon = coupure;
    t_new->fin_troncon = (*fonction->t.begin ())->debut_troncon;
    t_new->x0 = 0.;
    t_new->x1 = 0.;
    t_new->x2 = 0.;
    t_new->x3 = 0.;
    t_new->x4 = 0.;
    t_new->x5 = 0.;
    t_new->x6 = 0.;
    
    fonction->t.push_front (t_new);
    
    return true;
  }
  else
  {
  // Sinon
  //   Pour chaque tronçon
  //     Si la coupure correspond exactement à la borne supérieure
  //       Fin.
  //     Sinon si la coupure est à l'intérieur du tronçon étudié
  //       Scindage du tronçon en deux.
  //       Attribution des mêmes coefficients pour les deux tronçons.
  //       Fin.
  //     FinSi
  //   FinPour
  // FinSi
    std::list <Troncon *>::iterator it = fonction->t.begin ();
    Troncon *t_new;
    
    while (it != fonction->t.end ())
    {
      if (errrel ((*it)->fin_troncon, coupure))
      {
        return true;
      }
      else if ((*it)->fin_troncon > coupure)
      {
        t_new = new Troncon;
        
        t_new->debut_troncon = (*it)->debut_troncon;
        t_new->fin_troncon = coupure;
        t_new->x0 = (*it)->x0;
        t_new->x1 = (*it)->x1;
        t_new->x2 = (*it)->x2;
        t_new->x3 = (*it)->x3;
        t_new->x4 = (*it)->x4;
        t_new->x5 = (*it)->x5;
        t_new->x6 = (*it)->x6;
        
        fonction->t.insert (it, t_new);
        
        return true;
      }
      
      ++it;
    }
  // Si la position de la coupure est au-delà à la borne supérieure du dernier
  // tronçon
  //   Ajout d'un nouveau tronçon en dernière position.
  //   Initialisation de tous les coefficients à 0.
  //   Fin.
  // FinSi
    
    t_new = new Troncon;
    
    --it;
    
    t_new->debut_troncon = (*it)->fin_troncon;
    t_new->fin_troncon = coupure;
    t_new->x0 = 0.;
    t_new->x1 = 0.;
    t_new->x2 = 0.;
    t_new->x3 = 0.;
    t_new->x4 = 0.;
    t_new->x5 = 0.;
    
    fonction->t.push_back (t_new);
    
    return true;
  }
}


/**
 * \brief Additionne une fonction à une fonction existante dont le domaine de
 *        validité est compris entre debut_troncon et fin_troncon. Si la
 *        fonction ne possède pas un tronçon commençant à debut_troncon ou un
 *        tronçon finissant à fin_troncon, une scission sera effectuée par la
 *        fonction #common_fonction_scinde_troncon.\n
 *        Si le domaine de validité du nouveau tronçon est totalement en dehors
 *        du domaine de validité de l'actuelle fonction, un tronçon
 *        intermédiaire sera créé et possèdera une fonction f(x)=0.
 * \param fonction : la variable contenant la fonction
 * \param debut_troncon : début du tronçon de validité de la fonction
 * \param fin_troncon : fin du tronçon de validité de la fonction
 * \param x0 : coefficients de la formule x0 +
 * \param x1 : x1*x +
 * \param x2 : x2*x^2 +
 * \param x3 : x3*x^3 +
 * \param x4 : x4*x^4 +
 * \param x5 : x5*x^5 +
 * \param x6 : x6*x^6
 * \param t : modifie les coefficients ci-dessus afin d'effectuer une
 *                 translation de la fonction de 0 à t.
 *\return
 *   Succès : true.\n
 *   Échec : false :
 *     - fonction == NULL,
 *     - fin_troncon < debut_troncon,
 *     - en cas d'erreur d'allocation mémoire,
 *     - en cas d'erreur due à une fonction interne.
 */
bool
common_fonction_ajout_poly (Fonction *fonction,
                            double    debut_troncon,
                            double    fin_troncon,
                            double    x0,
                            double    x1,
                            double    x2,
                            double    x3,
                            double    x4,
                            double    x5,
                            double    x6,
                            double    t)
{
  double x0_t, x1_t, x2_t, x3_t, x4_t, x5_t, x6_t;
  
  BUGPARAM (fonction, "%p", fonction, false)
  // Si fin_troncon == debut_troncon Alors
  //   Fin.
  // FinSi
  if (errrel (fin_troncon, debut_troncon))
  {
    return true;
  }
  
  INFO (fin_troncon > debut_troncon,
        false,
        (gettext ("Le début du tronçon (%.20f) est supérieur à la fin (%.20f).\n"),
                  debut_troncon,
                  fin_troncon); )
  
  debut_troncon = debut_troncon + t;
  fin_troncon = fin_troncon + t;
  
  // Détermination les nouveaux coefficients à partir du résultat de f(x-t).
  x6_t = x6;
  x5_t = - 6. * x6 * t
         + x5;
  x4_t =  15. * x6 * t * t
         - 5. * x5 * t
         + x4;
  x3_t = - 20. * x6 * t * t * t
         + 10. * x5 * t * t
         -  4. * x4 * t
         + x3;
  x2_t =   15. * x6 * t * t * t * t
         - 10. * x5 * t * t * t
         +  6. * x4 * t * t
         -  3. * x3 * t
         + x2;
  x1_t = - 6. * x6 * t * t * t * t * t
         + 5. * x5 * t * t * t * t
         - 4. * x4 * t * t * t
         + 3. * x3 * t * t
         - 2. * x2 * t
         + x1;
  x0_t =   x6 * t * t * t * t * t * t
         - x5 * t * t * t * t * t
         + x4 * t * t * t * t
         - x3 * t * t * t
         + x2 * t * t
         - x1 * t
         + x0;
  
  // Si aucun troncon n'est présent (fonction vide) Alors
  //   Création d'un tronçon avec pour borne debut_troncon .. fin_troncon.
  //   Attribution des coefficients de la fonction.
  if (fonction->t.size () == 0)
  {
    Troncon *t_new;
    
    t_new = new Troncon;
    
    t_new->debut_troncon = debut_troncon;
    t_new->fin_troncon = fin_troncon;
    t_new->x0 = x0_t;
    t_new->x1 = x1_t;
    t_new->x2 = x2_t;
    t_new->x3 = x3_t;
    t_new->x4 = x4_t;
    t_new->x5 = x5_t;
    t_new->x6 = x6_t;
    
    fonction->t.push_back (t_new);

    return true;
  }
  // Sinon
  //   Scission de la fonction à debut_troncon. Pour rappel, si la scission existe déjà
  //     aucun nouveau tronçon n'est créé.
  //   Scission de la fonction à fin_troncon.
  //   Pour chaque tronçon compris entre debut_troncon et fin_troncon Faire
  //     Addition de la fonction existante avec les nouveaux coefficients.
  //   FinPour
  // FinSi
  else
  {
    std::list <Troncon *>::iterator it = fonction->t.begin ();
    
    BUG (common_fonction_scinde_troncon (fonction, debut_troncon), false)
    BUG (common_fonction_scinde_troncon (fonction, fin_troncon), false)
    while (it != fonction->t.end ())
    {
      if (errrel ((*it)->debut_troncon, fin_troncon))
      {
        return true;
      }
      else if ((errrel ((*it)->debut_troncon, debut_troncon)) ||
               ((*it)->debut_troncon > debut_troncon))
      {
        (*it)->x0 += x0_t;
        (*it)->x1 += x1_t;
        (*it)->x2 += x2_t;
        (*it)->x3 += x3_t;
        (*it)->x4 += x4_t;
        (*it)->x5 += x5_t;
        (*it)->x6 += x6_t;
      }
      
      ++it;
    }
    return true;
  }
}


/**
 * \brief Additionne une fonction à une fonction existante.
 * \param fonction : fonction à modifier,
 * \param fonction_a_ajouter : fonction à ajouter,
 * \param multi : coefficient multiplicateur de la fonction à ajouter.
 * \return
 *   Succès : true.\n
 *   Échec : false :
 *     - fonction == NULL,
 *     - fonction_a_ajouter == NULL,
 *     - en cas d'erreur d'allocation mémoire.
 */
bool
common_fonction_ajout_fonction (Fonction *fonction,
                                Fonction *fonction_a_ajouter,
                                double    multi)
{
  std::list <Troncon *>::iterator it;
  
  BUGPARAM (fonction, "%p", fonction, false)
  BUGPARAM (fonction_a_ajouter, "%p", fonction_a_ajouter, false)
  
  it = fonction_a_ajouter->t.begin ();
  while (it != fonction_a_ajouter->t.end ())
  {
    BUG (common_fonction_ajout_poly (fonction,
                                     (*it)->debut_troncon,
                                     (*it)->fin_troncon,
                                     multi*(*it)->x0,
                                     multi*(*it)->x1,
                                     multi*(*it)->x2,
                                     multi*(*it)->x3,
                                     multi*(*it)->x4,
                                     multi*(*it)->x5,
                                     multi*(*it)->x6,
                                     0.),
         false)
    ++it;
  }
  
  return true;
}


/**
 * \brief Fusionne les tronçons voisins ayant une fonction identique.
 * \param fonction : fonction à afficher,
 * \param index : index de la fonction fonction. Peut être NULL.
 * \return
 *   Succès : true.\n
 *   Échec : false :
 *     - fonction == NULL,
 *     - en cas d'erreur d'allocation mémoire.
 */
bool
common_fonction_compacte (Fonction *fonction,
                          Fonction *index)
{
  std::list <Troncon *>::iterator it_f_1;
  std::list <Troncon *>::iterator it_f;
  std::list <Troncon *>::iterator it_i;
  
  BUGPARAM (fonction, "%p", fonction, false)
  
  if (fonction->t.size () <= 1)
  {
    return true;
  }
  
  it_f_1 = fonction->t.begin ();
  it_f = std::next (it_f_1);
  if (index != NULL)
  {
    it_i = std::next (index->t.begin ());
  }
  
  while (it_f != fonction->t.end ())
  {
    if ((errrel ((*it_f)->x0, (*it_f_1)->x0)) &&
        (errrel ((*it_f)->x1, (*it_f_1)->x1)) &&
        (errrel ((*it_f)->x2, (*it_f_1)->x2)) &&
        (errrel ((*it_f)->x3, (*it_f_1)->x3)) &&
        (errrel ((*it_f)->x4, (*it_f_1)->x4)) &&
        (errrel ((*it_f)->x5, (*it_f_1)->x5)) &&
        (errrel ((*it_f)->x6, (*it_f_1)->x6)) &&
        ((index == NULL) ||
         (errrel ((*it_i)->x0, (*std::prev (it_i))->x0))))
    {
      (*it_f_1)->fin_troncon = (*it_f)->fin_troncon;
      if (index != NULL)
      {
        (*std::prev (it_i))->fin_troncon = (*it_i)->fin_troncon;
      }
      it_f = fonction->t.erase (it_f);
      it_i = fonction->t.erase (it_i);
    }
    else
    {
      ++it_f_1;
      ++it_f;
      if (index != NULL)
      {
        ++it_i;
      }
    }
  }
  
  return true;
}


/**
 * \brief Renvoie a, b, et c en fonction de f(x1)=y1, ....
 * \param x1 : abscisse du permier point,
 * \param y1 : ordonnée du permier point,
 * \param x2 : abscisse du deuxième point,
 * \param y2 : ordonnée du deuxième point,
 * \param x3 : abscisse du troisième point,
 * \param y3 : ordonnée du troisième point,
 * \param a : valeur de a de la fonction a*x²+b*x+c,
 * \param b : valeur de b de la fonction a*x²+b*x+c,
 * \param c : valeur de c de la fonction a*x²+b*x+c.
 * \return Rien.
 */
void
common_fonction_ax2_bx_c (double      x1,
                          long double y1,
                          double      x2,
                          long double y2,
                          double      x3,
                          long double y3,
                          double     *a,
                          double     *b,
                          double     *c)
{
  // maxima : solve([a*x1^2 + b*x1 + c = y1,
  //                 a*x2^2 + b*x2 + c = y2,
  //                 a*x3^2 + b*x3 + c = y3],
  //                [a,b,c]
  //               );
  *a = (double) ((x1 * (y3 - y2) - x2 * y3 + x3 * y2 + (x2 - x3) * y1) /
                 (x1 * (x3 * x3 - x2 * x2) - x2 * x3 * x3 + x2 * x2 * x3 +
                  x1 * x1 * (x2 - x3)));
  *b = (double) (- (x1 * x1 * (y3 - y2) - x2 * x2 * y3 + x3 * x3 * y2 +
                    (x2 * x2 - x3 * x3) * y1) /
                   (x1 * (x3 * x3 - x2 * x2) - x2 * x3 * x3 + x2 * x2 * x3 +
                    x1 * x1 * (x2 - x3)));
  *c = (double) ((x1 * (x3 * x3 * y2 - x2 * x2 * y3) +
                  x1 * x1 * (x2 * y3 - x3 * y2) +
                  (x2 * x2 * x3 - x2 * x3 * x3) * y1) /
                 (x1 * (x3 * x3 - x2 * x2) - x2 * x3 * x3 + x2 * x2 * x3 +
                  x1 * x1 * (x2 - x3)));
  
  return;
}


/**
 * \brief Renvoie la valeur f(x). Un ordinateur étant ce qu'il est, lorsqu'on
 *        recherche par approximation successive un zéro, il est nécessaire
 *        d'avoir accès au maximum de décimales disponible. Le type double ne
 *        permet pas toujours d'atteindre une précision de 1e-8. Le type long
 *        double permet environ 1e-10.
 * \param fonction : fonction à afficher,
 * \param x_ : abscisse de la fonction à renvoyer,
 * \param position : cette variable est utilisé dans un cas particulier. Une
 *        abscisse peut posséder renvoyer à deux valeurs différentes. Dans le
 *        cas où l'abscisse demandée est exactement celle séparant deux
 *        tronçons, une discontinuité peut apparaître. Ainsi, si position vaut
 *        -1 la valeur du tronçon inférieure sera renvoyée (et NAN si position
 *        vaut l'abscisse inférieure de la fonction). Si position vaut 1, la
 *        valeur du tronçon supérieure est renvoyée (et NAN si la position
 *        vaut l'abscisse supérieure de la fonction). Si position vaut 0,
 *        l'abscisse inférieure sera renvoyée mais sans NAN en cas d'erreur.
 * \return
 *   Succès : La valeur souhaitée.\n
 *   Échec : NAN :
 *     - fonction == NULL,
 *     - x hors domaine,
 *     - position < -1 ou position > 1
 */
double
common_fonction_y (Fonction *fonction,
                   double    x_,
                   int8_t    position)
{
  long double x = x_; // NS (nsiqcppstyle)
  long double y; // NS (nsiqcppstyle)
  
  std::list <Troncon *>::iterator it;
  
  BUGPARAM (fonction, "%p", fonction, NAN)
  BUGPARAM (position, "%d", ((-1 <= position) && (position <= 1)), NAN)
  
  if (fonction->t.size () == 0)
  {
    return NAN;
  }
  
  if ((errrel ((*fonction->t.begin ())->debut_troncon, x_)) &&
      (position == -1))
  {
    return NAN;
  }
  
  it = fonction->t.begin ();
  while (it != fonction->t.end ())
  {
    if (x <= (*it)->fin_troncon)
    {
      if (x < (*it)->debut_troncon)
      {
        return NAN;
      }
      else
      {
        if ((errrel ((*it)->fin_troncon, x_)) &&
            (position == 1))
        {
          if (std::next (it) == fonction->t.end ())
          {
            return NAN;
          }
          else
          {
            ++it;
          }
        }
        y = (*it)->x0 +
            (*it)->x1 * x +
            (*it)->x2 * x * x +
            (*it)->x3 * x * x * x +
            (*it)->x4 * x * x * x * x +
            (*it)->x5 * x * x * x * x * x +
            (*it)->x6 * x * x * x * x * x * x;
        return (double) y;
      }
    }
    ++it;
  }
  
  return NAN;
}


/**
 * \brief Cherche l'abscisse dont l'ordonnée vaut 0.
 * \param fonction : fonction à analyser,
 * \param mini : valeur basse du domaine de recherche,
 * \param maxi : valeur haute du domaine de recherche,
 * \param zero_1 : première abscisse où l'ordonnée vaut 0,
 * \param zero_2 : deuxième abscisse où l'ordonnée vaut 0. Si un seul zéro est
 *                trouvé, il sera toujours dans zero1 et zero2 vaudra NAN.
 * \return
 *   Succès : false si aucun zéro n'est trouvé,
 *            true si au moins est trouvé.
 *   Échec : false :
 *     - fonction == NULL,
 *     - maxi <= mini.
 */
bool
common_fonction_cherche_zero (Fonction *fonction,
                              double    mini,
                              double    maxi,
                              double   *zero_1,
                              double   *zero_2)
{
  double xx1_2, xx2_2, xx3_2;
  double yy1, yy2, yy3;
  double a, b, c;
  double zero1, zero2;
  double ecart_x;
  
  BUGPARAM (fonction, "%p", fonction, false)
  INFO (maxi > mini,
        false,
        (gettext("Borne [%lf,%lf] incorrecte.\n"), mini, maxi); )
  
  xx1_2 = mini;
  xx3_2 = maxi;
  xx2_2 = (xx1_2 + xx3_2) / 2.;
  BUG (!std::isnan (yy1 = common_fonction_y (fonction, xx1_2,  1)), false)
  BUG (!std::isnan (yy2 = common_fonction_y (fonction, xx2_2,  0)), false)
  BUG (!std::isnan (yy3 = common_fonction_y (fonction, xx3_2, -1)), false)
  
  common_fonction_ax2_bx_c (xx1_2, yy1, xx2_2, yy2, xx3_2, yy3, &a, &b, &c);
  
  if (errmoy (a, ERRMOY_DIST))
  {
    if (errmoy (b, ERRMOY_DIST))
    {
      zero1 = NAN;
      zero2 = NAN;
    }
    else
    {
      zero1 = -c / b;
      if ((zero1 < mini) || (zero1 > maxi))
      {
        zero1 = NAN;
      }
      zero2 = NAN;
    }
  }
  else
  {
    if (b * b - 4 * a * c < 0.)
    {
      zero1 = NAN;
      zero2 = NAN;
    }
    else
    {
      zero1 = ( sqrt (b * b - 4 * a * c) - b) / (2 * a);
      zero2 = (-sqrt (b * b - 4 * a * c) - b) / (2 * a);
    }
  }
  
  if ((zero1 < mini) || (zero1 > maxi))
  {
    zero1 = NAN;
  }
  if ((zero2 < mini) || (zero2 > maxi))
  {
    zero2 = NAN;
  }
  // Idéalement, il faudrait faire une boucle ci-dessous mais les limitations
  // de la précision de la virgule flottante va attendre ces limites en
  // quelques itérations. On utilise cette méthode 2 fois pour déterminer une
  // première estimation précise des zéro avec l'écart. Pour la suite, on le
  // fait par le biais d'une recherche dicotomique.
  ecart_x = (maxi - mini) / 4.;
  if (!std::isnan (zero1))
  {
    double zero1_old = zero1;
    
    if (zero1 - ecart_x < mini)
    {
      xx1_2 = mini;
    }
    else
    {
      xx1_2 = zero1 - ecart_x;
    }
    if (zero1 + ecart_x > maxi)
    {
      xx3_2 = maxi;
    }
    else
    {
      xx3_2 = zero1 + ecart_x;
    }
    xx2_2 = (xx1_2 + xx3_2) / 2.;
    
    common_fonction_ax2_bx_c (xx1_2,
                              common_fonction_y (fonction, xx1_2, 1),
                              xx2_2,
                              common_fonction_y (fonction, xx2_2, 0),
                              xx3_2,
                              common_fonction_y (fonction, xx3_2, -1),
                              &a,
                              &b,
                              &c);
    
    if (errmoy (a, ERRMOY_DIST))
    {
      if (errmoy (b, ERRMOY_DIST))
      {
        zero1 = NAN;
      }
      else
      {
        zero1 = - c / b;
      }
    }
    else
    {
      if (b * b - 4 * a * c < 0.)
      {
        zero1 = NAN;
      }
      else
      {
        zero1 = (sqrt (b * b - 4 * a * c) - b) / (2 * a);
      }
    }
    
    ecart_x = fabs (zero1_old - zero1) / 2.;
  }
  if ((zero1 < mini) || (zero1 > maxi))
  {
    zero1 = NAN;
  }
  
  // Recherche dicotomique du zero1
  // Première vérification d'usage.
  if ((!std::isnan (zero1)) && (!(errmoy (ecart_x, ERRMOY_DIST))))
  {
    xx1_2 = std::max (zero1 - ecart_x, mini);
    xx3_2 = std::min (zero1 + ecart_x, maxi);
    xx2_2 = (xx1_2 + xx3_2) / 2.;
    while (! (errmoy (xx3_2 - xx1_2, ERRMOY_DIST)))
    {
      if (std::signbit (common_fonction_y (fonction, xx1_2, 1)) ==
                         std::signbit (common_fonction_y (fonction, xx2_2, 0)))
      {
        xx1_2 = xx2_2;
      }
      else
      {
        xx3_2 = xx2_2;
      }
      xx2_2 = (xx1_2 + xx3_2) / 2.;
    }
    zero1 = (xx3_2 + xx1_2) / 2.;
  }
  
  // On passe à la recherche du zero2 par la même méthode de travail
  ecart_x = (maxi - mini) / 4.;
  if (!std::isnan (zero2))
  {
    double zero2_old = zero2;
    
    if (zero2 - ecart_x < mini)
    {
      xx1_2 = mini;
    }
    else
    {
      xx1_2 = zero2 - ecart_x;
    }
    if (zero2 + ecart_x > maxi)
    {
      xx3_2 = maxi;
    }
    else
    {
      xx3_2 = zero2 + ecart_x;
    }
    xx2_2 = (xx1_2 + xx3_2) / 2.;
    
    common_fonction_ax2_bx_c (xx1_2,
                              common_fonction_y (fonction, xx1_2, 1),
                              xx2_2,
                              common_fonction_y (fonction, xx2_2, 0),
                              xx3_2,
                              common_fonction_y (fonction, xx3_2, -1),
                              &a,
                              &b,
                              &c);
    
    if (errmoy (a, ERRMOY_DIST))
    {
      if (errmoy (b, ERRMOY_DIST))
      {
        zero2 = NAN;
      }
      else
      {
        zero2 = - c / b;
      }
    }
    else
    {
      if (b * b - 4 * a * c < 0.)
      {
        zero2 = NAN;
      }
      else
      {
        zero2 = (- sqrt(b * b - 4 * a * c) - b) / (2 * a);
      }
    }
    
    ecart_x = fabs (zero2_old - zero2) / 2.;
  }
  // Dicotomie
  if ((!std::isnan (zero2)) && (!(errmoy (ecart_x, ERRMOY_DIST))))
  {
    xx1_2 = std::max (zero2 - ecart_x, mini);
    xx3_2 = std::min (zero2 + ecart_x, maxi);
    xx2_2 = (xx1_2 + xx3_2) / 2.;
    while (! (errmoy (xx3_2 - xx1_2, ERRMOY_DIST)))
    {
      if (std::signbit (common_fonction_y (fonction, xx1_2, 1)) ==
                         std::signbit (common_fonction_y (fonction, xx2_2, 0)))
      {
        xx1_2 = xx2_2;
      }
      else
      {
        xx3_2 = xx2_2;
      }
      xx2_2 = (xx1_2 + xx3_2) / 2.;
    }
    zero2 = (xx3_2 + xx1_2) / 2.;
  }
  
  if ((zero2 < mini) || (zero2 > maxi))
  {
    zero2 = NAN;
  }
  
  if ((std::isnan (zero1)) && (std::isnan (zero2)))
  {
    *zero_1 = NAN;
    *zero_2 = NAN;
  }
  else if ((std::isnan (zero1)) && (!std::isnan (zero2)))
  {
    *zero_1 = zero2;
    *zero_2 = NAN;
  }
  else if ((!std::isnan (zero1)) && (std::isnan (zero2)))
  {
    *zero_1 = zero1;
    *zero_2 = NAN;
  }
  else if (errrel (zero1, zero2))
  {
    *zero_1 = zero1;
    *zero_2 = NAN;
  }
  else
  {
    *zero_1 = zero1;
    *zero_2 = zero2;
  }
  
  if ((std::isnan (*zero_1)) || (!std::isnan (*zero_2)))
  {
    return false;
  }
  
  return true;
}


/**
 * \brief Affiche les points caractéristiques de la fonction.
 * \param fonction : fonction à afficher,
 * \param pos : position des points caractéristiques,
 * \param val : valeur des points caractéristiques.
 * \return
 *   Succès : le nombre de points caractéristiques.\n
 *   Échec : 0 :
 *     - fonction == NULL,
 *     - pos == NULL,
 *     - val == NULL,
 *     - erreur d'allocation mémoire.
 */
uint8_t
common_fonction_caracteristiques (Fonction *fonction,
                                  double  **pos,
                                  double  **val)
{
  uint8_t  nb = 0;
  uint8_t  j;
  double  *pos_tmp = NULL, *val_tmp = NULL;
  void    *tmp;
  
  std::list <Troncon *>::iterator it;
  
  BUGPARAM (fonction, "%p", fonction, 0)
  BUGPARAM (pos, "%p", pos, 0)
  BUGPARAM (val, "%p", val, 0)
  
  if (fonction->t.size () == 0)
  {
    *pos = NULL;
    *val = NULL;
    return 0;
  }
  
  it = fonction->t.begin ();
  while (it != fonction->t.end ())
  {
    // On commence par s'occuper du début du tronçon. 
    
    // On ajoute si c'est le début de la fonction
    if (it == fonction->t.begin ())
    {
      BUGCRIT (pos_tmp = (double *) malloc (sizeof (double)),
               0,
               (gettext ("Erreur d'allocation mémoire.\n")); )
      pos_tmp[0] = (*it)->debut_troncon;
      BUGCRIT (val_tmp = (double *) malloc (sizeof (double)),
               0,
               (gettext ("Erreur d'allocation mémoire.\n"));
                 free (pos_tmp); )
      val_tmp[0] = common_fonction_y (fonction,
                                      (*it)->debut_troncon,
                                      1);
      nb = 1;
    }
    else
    {
      // On vérifie si la fonction est discontinue en x
      if ((!errrel ((*it)->debut_troncon,
                    (*std::prev (it))->fin_troncon)) ||
      // ou si elle est discontinue en y
          (!errrel (common_fonction_y (fonction,
                                       (*it)->debut_troncon,
                                       -1),
                    common_fonction_y (fonction,
                                       (*it)->debut_troncon,
                                       1))))
      {
        nb++;
        tmp = pos_tmp;
        BUGCRIT (pos_tmp = (double *) realloc (pos_tmp, sizeof (double) * nb),
                 0,
                 (gettext ("Erreur d'allocation mémoire.\n"));
                   free (tmp);
                   free (val_tmp); )
        pos_tmp[nb - 1] = (*it)->debut_troncon;
        tmp = val_tmp;
        BUGCRIT (val_tmp = (double *) realloc (val_tmp, sizeof (double) * nb),
                 0,
                 (gettext ("Erreur d'allocation mémoire.\n"));
                   free (pos_tmp);
                   free (tmp); )
        val_tmp[nb - 1] = common_fonction_y (fonction,
                                             (*it)->debut_troncon,
                                             -1);
        nb++;
        tmp = pos_tmp;
        BUGCRIT (pos_tmp = (double *) realloc (pos_tmp, sizeof (double) * nb),
                 0,
                 (gettext ("Erreur d'allocation mémoire.\n"));
                   free (tmp);
                   free (val_tmp); )
        pos_tmp[nb - 1] = (*it)->debut_troncon;
        tmp = val_tmp;
        BUGCRIT (val_tmp = (double *) realloc (val_tmp, sizeof (double) * nb),
                 0,
                 (gettext ("Erreur d'allocation mémoire.\n"));
                   free (pos_tmp);
                   free (tmp); )
        val_tmp[nb - 1] = common_fonction_y (fonction,
                                             (*it)->debut_troncon,
                                             1);
      }
      // Si elle est continue, elle est un point caractéristique si sa dérivée
      // change de signe. Rappel : f'(x)=(f(x+Dx)-f(x))/Dx. Dans notre cas, on
      // prendra Dx = 1/1000 du troncon.
      else
      {
        double dx;
        double fprim1, fprim2;
        
        dx = ((*std::prev (it))->fin_troncon -
                                     (*std::prev (it))->debut_troncon) / 1000.;
        fprim1 = (common_fonction_y (fonction, (*it)->debut_troncon, -1) -
                  common_fonction_y (fonction, (*it)->debut_troncon - dx, 0)) /
                 dx;
        
        dx = ((*it)->fin_troncon - (*it)->debut_troncon) / 1000.;
        fprim2 = (common_fonction_y (fonction, (*it)->debut_troncon + dx, 0) -
                  common_fonction_y (fonction, (*it)->debut_troncon, 1)) / dx;
        
        if (std::signbit (fprim1) != std::signbit (fprim2))
        {
          nb++;
          tmp = pos_tmp;
          BUGCRIT (pos_tmp = (double *) realloc (pos_tmp,
                                                 sizeof (double) * nb),
                   0,
                   (gettext ("Erreur d'allocation mémoire.\n"));
                     free (tmp);
                     free (val_tmp); )
          pos_tmp[nb - 1] = (*it)->debut_troncon;
          tmp = val_tmp;
          BUGCRIT (val_tmp = (double *) realloc (val_tmp,
                                                 sizeof (double) * nb),
                   0,
                   (gettext ("Erreur d'allocation mémoire.\n"));
                     free (pos_tmp);
                     free (tmp); )
          val_tmp[nb - 1] = common_fonction_y (fonction,
                                               (*it)->debut_troncon,
                                               -1);
        }
      }
    }
    
    // On étudie l'intérieur du tronçon
    for (j = 0; j < 9; j++)
    {
      double xx1, xx2, xx3;
      double xx1_2, xx2_2, xx3_2;
      double a, b, c; // forme de l'interpolation : a*x²+b*x+c
      double zero1, zero2, deriv_zero;
      double zero2_old = NAN, deriv_zero_old = NAN;
      double ecart_x, ecart_old;
      
      // On commence par calculer une interpolation hyperbolique
      // On s'arrange pour que la deuxième moitié du tronçon à analyser soit la
      // première moitié du tronçon suivant. C'est une sécurité. En effet, on
      // ne connait pas l'erreur de l'approximation faite. Il se pourrait
      // qu'avec l'approximation de base x soit dans l'intervalle mais que,
      // suite à l'affinement x en sorte.
      xx1 = (*it)->debut_troncon +
              ((*it)->fin_troncon - (*it)->debut_troncon) / 10. * j;
      xx2 = (*it)->debut_troncon +
              ((*it)->fin_troncon - (*it)->debut_troncon) / 10. * (j + 1);
      xx3 = (*it)->debut_troncon +
              ((*it)->fin_troncon - (*it)->debut_troncon) / 10. * (j + 2.);
      
      common_fonction_ax2_bx_c (xx1,
                                common_fonction_y (fonction, xx1, 1),
                                xx2,
                                common_fonction_y (fonction, xx2, 0),
                                xx3,
                                common_fonction_y (fonction, xx3, -1),
                                &a,
                                &b,
                                &c);
      
      if (errmoy (a, ERRMOY_DIST))
      {
        if (errmoy (b, ERRMOY_DIST))
        {
          zero1 = NAN;
          zero2 = NAN;
        }
        else
        {
          zero1 = - c / b;
          zero2 = NAN;
        }
        deriv_zero = NAN;
      }
      else
      {
        if (b * b - 4 * a * c < 0.)
        {
          zero1 = NAN;
          zero2 = NAN;
        }
        else
        {
          // maxima : solve(a*x^2+b*x+c=0,x);
          zero1 = ( sqrt (b * b - 4 * a * c) - b) / (2 * a);
          zero2 = (-sqrt (b * b - 4 * a * c) - b) / (2 * a);
        }
        deriv_zero = - b / (2 * a);
      }
      
      if ((deriv_zero < xx1) || (deriv_zero > xx3))
      {
        deriv_zero = NAN;
      }
      if ((zero1 < xx1) || (zero1 > xx3))
      {
        zero1 = NAN;
      }
      if ((zero2 < xx1) || (zero2 > xx3))
      {
        zero2 = NAN;
      }
      
      ecart_old = xx3 - xx1;
      ecart_x = (xx3 - xx1) / 4.;
      while ((!std::isnan (zero1)) && (!errmoy (ecart_x, ERRMOY_DIST)))
      {
        double zero1_old = zero1;
        
        if (zero1 - ecart_x < xx1)
        {
          xx1_2 = xx1;
        }
        else
        {
          xx1_2 = zero1 - ecart_x;
        }
        if (zero1 + ecart_x > xx3)
        {
          xx3_2 = xx3;
        }
        else
        {
          xx3_2 = zero1 + ecart_x;
        }
        xx2_2 = (xx1_2 + xx3_2) / 2.;
        
        common_fonction_ax2_bx_c (xx1_2,
                                  common_fonction_y (fonction, xx1_2, 1),
                                  xx2_2,
                                  common_fonction_y (fonction, xx2_2, 0),
                                  xx3_2,
                                  common_fonction_y (fonction, xx3_2, -1),
                                  &a,
                                  &b,
                                  &c);
        
        if (errmoy (a, ERRMOY_DIST))
        {
          if (errmoy (b, ERRMOY_DIST))
          {
            zero1 = NAN;
          }
          else
          {
            zero1 = - c / b;
            if ((zero1 < xx1) || (zero1 > xx3))
            {
              zero1 = NAN;
            }
          }
        }
        else
        {
          if (b * b - 4 * a * c < 0.)
          {
            zero1 = NAN;
          }
          else
          {
            zero1 = (sqrt (b * b - 4 * a * c) - b) / (2 * a);
            if ((zero1 < xx1) || (zero1 > xx3))
            {
              zero1 = NAN;
            }
          }
        }
        ecart_x = fabs (zero1_old - zero1) / 4.;
        if (ecart_x > ecart_old)
        {
          // Si on arrive ici, c'est que la méthode ci-dessus ne marche plus à
          // cause des imprécisions dûes aux virgules flottantes. On passe donc
          // en mode recherche dicotomique en calculant le signe aux points
          // xx1_2, xx2_2 et xx3_2. Si le signe de xx1_2 est le même que xx2_2,
          // xx1_2 devient égal à xx2_2. Si le signe de xx3_2 est le même que
          // xx2_2, xx3_2 devient égal à xx2_2.
          // La méthode est un peu plus longue mais est moins problématique.
          while (true)
          {
            if (std::signbit (common_fonction_y (fonction, xx1_2, 1)) ==
                         std::signbit (common_fonction_y (fonction, xx2_2, 0)))
            {
              xx1_2 = xx2_2;
            }
            else
            {
              xx3_2 = xx2_2;
            }
            if (errmoy (xx3_2 - xx1_2, ERRMOY_DIST))
            {
              zero1 = (xx3_2 + xx1_2) / 2.;
              break;
            }
            xx2_2 = (xx3_2 + xx1_2) / 2.;
          }
          
          break;
        }
        else
        {
          if ((zero1 < xx1) || (zero1 > xx3))
          {
            zero1 = NAN;
          }
        }
        ecart_old = ecart_x;
      }
      
      ecart_old = xx3 - xx1;
      ecart_x = (xx3 - xx1) / 4.;
      while ((!std::isnan (zero2)) && (!errmoy (ecart_x, ERRMOY_DIST)))
      {
        zero2_old = zero2;
        if (zero2 - ecart_x < xx1)
        {
          xx1_2 = xx1;
        }
        else
        {
          xx1_2 = zero2 - ecart_x;
        }
        if (zero2 + ecart_x > xx3)
        {
          xx3_2 = xx3;
        }
        else
        {
          xx3_2 = zero2 + ecart_x;
        }
        xx2_2 = (xx1_2 + xx3_2) / 2.;
        
        common_fonction_ax2_bx_c (xx1_2,
                                  common_fonction_y (fonction, xx1_2, 1),
                                  xx2_2,
                                  common_fonction_y (fonction, xx2_2, 0),
                                  xx3_2,
                                  common_fonction_y (fonction, xx3_2, -1),
                                  &a,
                                  &b,
                                  &c);
        
        if (errmoy (a, ERRMOY_DIST))
        {
          if (errmoy (b, ERRMOY_DIST))
          {
            zero2 = NAN;
          }
          else
          {
            zero2 = - c / b;
            if ((zero2 < xx1) || (zero2 > xx3))
            {
              zero2 = NAN;
            }
          }
        }
        else
        {
          if (b * b - 4 * a * c < 0.)
          {
            zero2 = NAN;
          }
          else
          {
            zero2 = (-sqrt (b * b - 4 * a * c) - b) / (2 * a);
            if ((zero2 < xx1) || (zero2 > xx3))
            {
              zero2 = NAN;
            }
          }
        }
        ecart_x = fabs (zero2_old - zero2) / 4.;
        if (ecart_x > ecart_old)
        {
          // Si on arrive ici, c'est que la méthode ci-dessus ne marche plus à
          // cause des imprécisions dues aux virgules flottantes. On passe donc
          // en mode recherche dicotomique en calculant le signe aux points
          // xx1_2, xx2_2 et xx3_2. Si le signe de xx1_2 est le même que xx2_2,
          // xx1_2 devient égal à xx2_2. Si le signe de xx3_2 est le même que
          // xx2_2, xx3_2 devient égal à xx2_2.
          // La méthode est un peu plus longue mais est moins problématique.
          while (true)
          {
            if (std::signbit (common_fonction_y (fonction, xx1_2, 1)) ==
                         std::signbit (common_fonction_y (fonction, xx2_2, 0)))
            {
              xx1_2 = xx2_2;
            }
            else
            {
              xx3_2 = xx2_2;
            }
            if (errmoy (xx3_2 - xx1_2, ERRMOY_DIST))
            {
              zero2 = (xx3_2 + xx1_2) / 2.;
              break;
            }
            xx2_2 = (xx3_2 + xx1_2) / 2.;
          }
          
          break;
        }
        else
        {
          if ((zero2 < xx1) || (zero2 > xx3))
          {
            zero2 = NAN;
          }
        }
      }
      
      ecart_old = xx3 - xx1;
      ecart_x = (xx3 - xx1) / 4.;
      while ((!std::isnan (deriv_zero)) && (!errmoy (ecart_x, ERRMOY_DIST)))
      {
        deriv_zero_old = deriv_zero;
        if (deriv_zero - ecart_x < xx1)
        {
          xx1_2 = xx1;
        }
        else
        {
          xx1_2 = deriv_zero - ecart_x;
        }
        if (deriv_zero + ecart_x > xx3)
        {
          xx3_2 = xx3;
        }
        else
        {
          xx3_2 = deriv_zero + ecart_x;
        }
        xx2_2 = (xx1_2 + xx3_2) / 2.;
        
        common_fonction_ax2_bx_c (xx1_2,
                                  common_fonction_y (fonction, xx1_2, 1),
                                  xx2_2,
                                  common_fonction_y (fonction, xx2_2, 0),
                                  xx3_2,
                                  common_fonction_y (fonction, xx3_2, -1),
                                  &a,
                                  &b,
                                  &c);
        
        if (errmoy (a, ERRMOY_DIST))
        {
          deriv_zero = NAN;
        }
        else
        {
          deriv_zero = - b / (2 * a);
        }
        ecart_x = fabs (deriv_zero_old - deriv_zero) / 4.;
        if (ecart_x > ecart_old)
        {
          // Si on arrive ici, c'est que la méthode ci-dessus ne marche plus à
          // cause des imprécisions dues aux virgules flottantes. On passe donc
          // en mode recherche dicotomique en calculant le signe de la dérivée
          // aux points xx1_2, xx2_2 et xx3_2. Si le signe de xx1_2 est le même
          // que xx2_2, xx1_2 devient égal à xx2_2. Si le signe de xx3_2 est le
          // même que xx2_2, xx3_2 devient égal à xx2_2.
          // La méthode est un peu plus longue mais est moins problématique.
          while (true)
          {
            ecart_old = xx3_2 - xx1_2;
            a = common_fonction_y (fonction, xx1_2 + ecart_old / 10., 1) -
                                        common_fonction_y (fonction, xx1_2, 1);
            b = common_fonction_y (fonction, xx2_2 + ecart_old / 10., 0) -
                                        common_fonction_y (fonction, xx2_2, 0);
            c = common_fonction_y (fonction, xx3_2, -1) -
                     common_fonction_y (fonction, xx3_2 - ecart_old / 10., -1);
            if (std::signbit (a) == std::signbit (b))
            {
              xx1_2 = xx2_2;
            }
            else
            {
              xx3_2 = xx2_2;
            }
            if (errmoy (xx3_2 - xx1_2, ERRMOY_DIST))
            {
              deriv_zero = (xx3_2 + xx1_2) / 2.;
              break;
            }
            xx2_2 = (xx3_2 + xx1_2) / 2.;
          }
          
          break;
        }
        else
        {
          if ((deriv_zero < xx1) || (deriv_zero > xx3))
          {
            deriv_zero = NAN;
          }
        }
        ecart_old = ecart_x;
      }
      
      // On tri les résultats.
      a = zero1;
      b = zero2;
      c = deriv_zero;
      
      if (a > b)
      {
        a = zero2;
        b = zero1;
      }
      if (b > c)
      {
        ecart_x = b;
        b = c;
        c = ecart_x;
      }
      if (a > c)
      {
        ecart_x = a;
        a = c;
        c = ecart_x;
      }
      
      if ((!std::isnan (a)) && (!errmoy (pos_tmp [nb - 1] - a, ERRMOY_DIST)))
      {
        nb++;
        tmp = pos_tmp;
        BUGCRIT (pos_tmp = (double *) realloc (pos_tmp, sizeof (double) * nb),
                 0,
                 (gettext ("Erreur d'allocation mémoire.\n"));
                   free (tmp);
                   free (val_tmp); )
        pos_tmp[nb - 1] = a;
        tmp = val_tmp;
        BUGCRIT (val_tmp = (double *) realloc (val_tmp, sizeof (double) * nb),
                 0,
                 (gettext ("Erreur d'allocation mémoire.\n"));
                   free (pos_tmp);
                   free (tmp); )
        val_tmp[nb - 1] = common_fonction_y (fonction, a, 0);
      }
      if ((!std::isnan (b)) && (!errmoy (pos_tmp[nb - 1] - b, ERRMOY_DIST)))
      {
        nb++;
        tmp = pos_tmp;
        BUGCRIT (pos_tmp = (double *) realloc (pos_tmp, sizeof (double) * nb),
                 0,
                 (gettext ("Erreur d'allocation mémoire.\n"));
                   free (tmp);
                   free (val_tmp); )
        pos_tmp[nb - 1] = b;
        tmp = val_tmp;
        BUGCRIT (val_tmp = (double *) realloc (val_tmp, sizeof (double) * nb),
                 0,
                 (gettext ("Erreur d'allocation mémoire.\n"));
                   free (pos_tmp);
                   free (tmp); )
        val_tmp[nb - 1] = common_fonction_y (fonction, b, 0);
      }
      if ((!std::isnan (c)) && (!errmoy (pos_tmp [nb - 1] - c, ERRMOY_DIST)))
      {
        nb++;
        tmp = pos_tmp;
        BUGCRIT (pos_tmp = (double *) realloc (pos_tmp, sizeof (double) * nb),
                 0,
                 (gettext ("Erreur d'allocation mémoire.\n"));
                   free (tmp);
                   free (val_tmp); )
        pos_tmp[nb - 1] = c;
        tmp = val_tmp;
        BUGCRIT (val_tmp = (double *) realloc (val_tmp, sizeof (double) * nb),
                 0,
                 (gettext ("Erreur d'allocation mémoire.\n"));
                   free (pos_tmp);
                   free (tmp); )
        val_tmp[nb - 1] = common_fonction_y (fonction, c, 0);
      }
    }
    
    ++it;
  }
  
  it = std::prev (fonction->t.end ());
  if (!errrel (pos_tmp[nb - 1],
               (*it)->fin_troncon))
  {
    nb++;
    tmp = pos_tmp;
    BUGCRIT (pos_tmp = (double *) realloc (pos_tmp, sizeof (double) * nb),
             0,
             (gettext ("Erreur d'allocation mémoire.\n"));
               free (tmp);
               free (val_tmp); )
    pos_tmp[nb - 1] = (*it)->fin_troncon;
    tmp = val_tmp;
    BUGCRIT (val_tmp = (double *) realloc (val_tmp, sizeof (double) * nb),
             0,
             (gettext ("Erreur d'allocation mémoire.\n"));
               free (pos_tmp);
               free (tmp); )
    val_tmp[nb - 1] = common_fonction_y (fonction, (*it)->fin_troncon, -1);
  }
  
  *pos = pos_tmp;
  *val = val_tmp;
  
  return nb;
}


/**
 * \brief Affiche les points caractéristiques d'une fonction.
 * \param fonction : fonction à afficher,
 * \param decimales_x : nombre de décimales à afficher pour l'abscisse.
 * \param decimales_y : nombre de décimales à afficher pour l'ordonnée.
 * \return
 *   Succès : true.\n
 *   Échec : false :
 *     - fonction == NULL,
 *     - Echec de la fonction #common_fonction_caracteristiques
 */
std::string
common_fonction_affiche_caract (Fonction *fonction,
                                uint8_t   decimales_x,
                                uint8_t   decimales_y)
{
  double     *pos, *val;
  uint8_t     nb_val, i;
  std::string retour;
  
  BUGPARAM (fonction, "%p", fonction, NULL)
  
  BUG (nb_val = common_fonction_caracteristiques (fonction, &pos, &val), NULL);
  
  retour = format ("%.*lf : %.*lf", decimales_x, pos[0], decimales_y, val[0]);
  
  for (i = 1; i < nb_val; i++)
  {
    retour += "\n" + format ("%.*lf : %.*lf",
                             decimales_x,
                             pos[i],
                             decimales_y,
                             val[i]);
  }
  
  free (pos);
  free (val);
  
  return retour;
}


/**
 * \brief Affiche la fonction (coefficients pour chaque tronçon) ainsi que la
 *        valeur de la fonction pour chaque extrémité du tronçon.
 * \param fonction : fonction à afficher.
 * \return
 *   Succès : true.\n
 *   Échec : false :
 *     - fonction == NULL.
 */
bool
common_fonction_affiche (Fonction *fonction)
{
  std::list <Troncon *>::iterator it;
  
  BUGPARAM (fonction, "%p", fonction, false)
  
  INFO (fonction->t.size () != 0, false, (gettext ("Fonction indéfinie.\n")); )
  
  it = fonction->t.begin ();
  while (it != fonction->t.end ())
  {
    printf (gettext ("debut_troncon : %.5f\tfin_troncon : %.5f\t0 : %.20f\tx : %.20f\tx2 : %.20f\tx3 : %.20f\tx4 : %.20f\tx5 : %.20f\tx6 : %.20f\tsoit f(%.5f) = %.20f\tf(%.5f) = %.20f\n"),
      (*it)->debut_troncon,
      (*it)->fin_troncon,
      (*it)->x0,
      (*it)->x1,
      (*it)->x2,
      (*it)->x3,
      (*it)->x4,
      (*it)->x5,
      (*it)->x6,
      (*it)->debut_troncon,
      (*it)->x0 +
        (*it)->x1 * (*it)->debut_troncon +
        (*it)->x2 * (*it)->debut_troncon * (*it)->debut_troncon +
        (*it)->x3 * (*it)->debut_troncon * (*it)->debut_troncon *
                    (*it)->debut_troncon +
        (*it)->x4 * (*it)->debut_troncon * (*it)->debut_troncon *
                    (*it)->debut_troncon * (*it)->debut_troncon +
        (*it)->x5 * (*it)->debut_troncon * (*it)->debut_troncon *
                    (*it)->debut_troncon * (*it)->debut_troncon *
                    (*it)->debut_troncon +
        (*it)->x6 * (*it)->debut_troncon * (*it)->debut_troncon *
                    (*it)->debut_troncon * (*it)->debut_troncon *
                    (*it)->debut_troncon * (*it)->debut_troncon,
      (*it)->fin_troncon,
      (*it)->x0 +
        (*it)->x1 * (*it)->fin_troncon +
        (*it)->x2 * (*it)->fin_troncon * (*it)->fin_troncon +
        (*it)->x3 * (*it)->fin_troncon * (*it)->fin_troncon *
                    (*it)->fin_troncon +
        (*it)->x4 * (*it)->fin_troncon * (*it)->fin_troncon *
                    (*it)->fin_troncon * (*it)->fin_troncon +
        (*it)->x5 * (*it)->fin_troncon * (*it)->fin_troncon *
                    (*it)->fin_troncon * (*it)->fin_troncon *
                    (*it)->fin_troncon +
        (*it)->x6 * (*it)->fin_troncon * (*it)->fin_troncon *
                    (*it)->fin_troncon * (*it)->fin_troncon *
                    (*it)->fin_troncon * (*it)->fin_troncon);
    ++it;
  }
  
  return true;
}


#ifdef ENABLE_GTK
/**
 * \brief Renvoie un dessin représentant la courbe enveloppe.
 * \param fonctions : la liste contenant les fonctions à dessiner,
 * \param width : la largeur du dessin,
 * \param height : la hauteur du dessin.
 * \param decimales : les valeurs inférieurs à pow(10, -decimales) sont
 *                    négligées.
 * \return
 *   Succès : pointeur vers le dessin.\n
 *   Echec :
 *     - fonction == NULL,
 *     - width == 0,
 *     - height == 0,
 *     - si toutes les fonctions ne commencent et ne finissent pas à la même
 *       longueur.
 */
GdkPixbuf *
common_fonction_dessin (std::list <Fonction *> *fonctions,
                        uint16_t                width,
                        uint16_t                height,
                        int8_t                  decimales)
{
  gint             rowstride, n_channels;
  uint16_t         x, y;
  guchar          *pixels, *p;
  GdkPixbuf       *pixbuf;
  cairo_surface_t *surface;
  cairo_t         *cr;
  double           fy_min = 0., fy_max = 0., echelle;
  double          *mi, *ma;
  Fonction        *fonction;
  
  std::list <Fonction *>::iterator it;
  
  BUGPARAM (fonctions, "%p", fonctions, NULL)
  BUGPARAM (decimales, "%d", decimales > 0, NULL)
  INFO (width,
        NULL,
        (gettext ("La largeur du dessin ne peut être nulle.\n")); )
  INFO (height,
        NULL,
        (gettext ("La hauteur du dessin ne peut être nulle.\n")); )
  surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, width, height);
  BUGCRIT (cairo_surface_status (surface) == CAIRO_STATUS_SUCCESS,
           NULL,
           (gettext ("Erreur d'allocation mémoire.\n")); )
  cr = cairo_create (surface);
  BUGCRIT (cairo_status (cr) == CAIRO_STATUS_SUCCESS,
           NULL,
           (gettext ("Erreur d'allocation mémoire.\n"));
             cairo_surface_destroy (surface); )
  BUGCRIT (mi = (double *) malloc (sizeof (double) * width),
           NULL,
           (gettext ("Erreur d'allocation mémoire.\n"));
             cairo_destroy (cr);
             cairo_surface_destroy (surface); )
  BUGCRIT (ma = (double *) malloc (sizeof (double) * width),
           NULL,
           (gettext ("Erreur d'allocation mémoire.\n"));
             cairo_destroy (cr);
             cairo_surface_destroy (surface);
             free (mi); )
  
  pixbuf = gdk_pixbuf_new (GDK_COLORSPACE_RGB, TRUE, 8, width, height);
  pixels = gdk_pixbuf_get_pixels (pixbuf);
  rowstride = gdk_pixbuf_get_rowstride (pixbuf);
  n_channels = gdk_pixbuf_get_n_channels (pixbuf);
  
  cairo_set_antialias (cr, CAIRO_ANTIALIAS_NONE);
  cairo_set_source_rgba (cr, 1., 1., 1., 0.);
  cairo_rectangle (cr, 0, 0, width, height);
  cairo_fill (cr);
    
  // On remplie tout avec un fond blanc
  for (y = 0; y < height; y++)
  {
    for (x = 0; x < width; x++)
    {
      p = pixels +
          y * (unsigned int) rowstride +
          x * (unsigned int) n_channels;
      p[0] = 255;
      p[1] = 255;
      p[2] = 255;
      if (n_channels == 4)
      {
        p[3] = 0;
      }
    }
  }
  
  // On détermine les valeurs de la courbe pour la première fonction.
  it = fonctions->begin ();
  fonction = *it;
  for (x = 0; x < width; x++)
  {
    mi[x] = common_fonction_y (
      fonction,
      (*fonction->t.begin ())->debut_troncon +
        x * ((*std::prev (fonction->t.end ()))->fin_troncon -
                         (*fonction->t.begin ())->debut_troncon) / (width - 1),
      0);
    ma[x] = mi[x];
    
    if (fy_max < mi[x])
    {
      fy_max = mi[x];
    }
    if (fy_min > mi[x])
    {
      fy_min = mi[x];
    }
  }
  
  ++it;
  while (it != fonctions->end ())
  {
    fonction = *it;
    for (x = 0; x < width; x++)
    {
      echelle = common_fonction_y (
        fonction,
        (*fonction->t.begin ())->debut_troncon +
          x * ((*std::prev (fonction->t.end ()))->fin_troncon -
                         (*fonction->t.begin ())->debut_troncon) / (width - 1),
        0);
      if (echelle > ma[x])
      {
        ma[x] = echelle;
      }
      if (echelle < mi[x])
      {
        mi[x] = echelle;
      }
      
      if (fy_max < echelle)
      {
        fy_max = echelle;
      }
      if (fy_min > echelle)
      {
        fy_min = echelle;
      }
    }
    
    ++it;
  }

  if (fabs (fy_max) < pow (10, -decimales))
  {
    fy_max = 0.;
  }
  if (fabs (fy_min) < pow (10, -decimales))
  {
    fy_min = 0.;
  }
  
  cairo_new_path (cr);
  cairo_set_line_width (cr, 1.);
  if ((fabs (fy_max) > pow (10, -decimales)) ||
      (fabs (fy_min) > pow (10, -decimales)))
  {
    cairo_path_t *save_path;
    
    echelle = ((height - 1.) / 2.) / std::max (fabs (fy_max), fabs (fy_min));
    
    cairo_set_source_rgba (cr, 0.8, 0.8, 0.8, 1.);
    // On inverse le signe en y car au milieu, le fait d'ajouter fait descendre
    // la position.
    cairo_move_to (cr, 1., height / 2. - mi[0] * echelle);
    
    for (x = 1; x < width; x++)
    {
      cairo_rel_line_to (cr, 1., - (mi[x] - mi[x - 1]) * echelle);
    }
    
    if (fonctions->size () == 1)
    {
      cairo_rel_line_to (cr, 0., - (ma[width - 1] - mi[width - 1]) * echelle);
      for (x = (uint16_t) (width - 1U); x > 0; x--)
      {
        cairo_rel_line_to (cr, -1., - (ma[x - 1] - ma[x]) * echelle);
      }
    }
    else
    {
      cairo_line_to (cr, width, height / 2.);
      cairo_line_to (cr, 1., height / 2.);
    }
    
    cairo_close_path (cr);
    save_path = cairo_copy_path (cr);
    cairo_fill (cr);
    cairo_set_source_rgba (cr, 0., 0., 0., 1.);
    cairo_new_path (cr);
    cairo_append_path (cr, save_path);
    cairo_stroke (cr);
    cairo_path_destroy (save_path);
  }
  
  cairo_set_source_rgba (cr, 1., 1., 1., 1.);
  cairo_move_to (cr, 1., height / 2.);
  cairo_line_to (cr, width, height / 2.);
  cairo_stroke (cr);
  cairo_set_source_rgba (cr, 0., 0., 0., 1.);
  echelle = 5.;
  cairo_set_dash (cr, &echelle, 1, 0);
  cairo_move_to (cr, 1., height / 2.);
  cairo_line_to (cr, width, height / 2.);
  cairo_stroke (cr);
  
  cairo_destroy (cr);
  
  pixbuf = gdk_pixbuf_get_from_surface (surface, 0, 0, width, height);
  cairo_surface_destroy (surface);
  free (mi);
  free (ma);
  
  return pixbuf;
}
#endif


/**
 * \brief Renvoie une liste de ponderations (liste) sur la base des x0 des
 *        tronçons de la variable fonction en cherchant dans la variable
 *        ponderations.
 * \param fonction : fonction à convertir.
 * \param ponderations : liste des ponderations possibles,
 * \param liste : liste des ponderations sur la base de la variable fonction.
 * \return
 *   Succès : true.\n
 *   Échec : NULL :
 *     - fonction == NULL,
 *     - ponderations == NULL,
 *     - liste == NULL.
 */
bool
common_fonction_conversion_combinaisons (
  Fonction                                 *fonction,
  std::list <std::list <Ponderation *> *>  *ponderations,
  std::list <std::list <Ponderation *> *> **liste)
{
  std::list <std::list <Ponderation *> *> *list_tmp;
  std::list <Troncon *>::iterator          it;
  
  BUGPARAM (fonction, "%p", fonction, false)
  BUGPARAM (ponderations, "%p", ponderations, false)
  BUGPARAM (liste, "%p", liste, false)
  
  list_tmp = new std::list <std::list <Ponderation *> *> ();
  
  it = fonction->t.begin ();
  while (it != fonction->t.end ())
  {
    uint16_t numero = (uint16_t) (*it)->x0;
    
    list_tmp->push_back (*std::next (ponderations->begin (), numero));
    
    ++it;
  }
  
  *liste = list_tmp;
  
  return true;
}

/**
 * \brief Renvoie la fonction sous forme de texte (coefficients pour chaque
 *        tronçon).
 * \param fonction : fonction à afficher.
 * \param index : liste de pondérations. La première pondération décrit le
 *                premier tronçon, la deuxième pondération décrit le deuxième
 *                tronçon, etc... Peut être NULL.
 * \param decimales : nombre de décimales à afficher.
 * \return
 *   Succès : pointeur contenant la fonction sous forme de texte à libérer.
 *   Échec : NULL :
 *     - fonction == NULL,
 *     - si index != NULL et que le nombre d'éléments dans la liste index est
 *       différent du nombre de tronçons dans la fonction,
 *     - erreur d'allocation mémoire.
 */
std::string
common_fonction_renvoie (Fonction                                *fonction,
                         std::list <std::list <Ponderation *> *> *index,
                         int8_t                                   decimales)
{
  std::string retour;
  double      minimum = pow (10, -decimales);
  
  std::list <std::list <Ponderation *> *>::iterator it;
  std::list <Troncon *>::iterator                   it2;
  
  BUGPARAM (fonction, "%p", fonction, NULL)
  BUGPARAM (index,
            "%p",
            (index == NULL) ||
            ((index != NULL) && (fonction->t.size () == index->size ())),
            NULL)
  
  if (fonction->t.size () == 0)
  {
    retour = format ("%.*lf", decimales, 0.);
  }
  
  if (index != NULL)
  {
    it = index->begin ();
  }
  
  it2 = fonction->t.begin ();
  while (it2 != fonction->t.end ())
  {
    std::string ajout;
    
    if (it2 != fonction->t.begin ())
    {
      retour += "\n";
    }
    
    if (fonction->t.size () != 1)
    {
      retour += format (gettext ("de %.*lfm à %.*lfm : "),
                        DECIMAL_DISTANCE,
                        (*it2)->debut_troncon,
                        DECIMAL_DISTANCE,
                        (*it2)->fin_troncon);
    }
    
    if (fabs ((*it2)->x0) > minimum)
    {
      ajout += format ("%.*lf",
                       decimales,
                       (*it2)->x0);
    }
    
    if (fabs ((*it2)->x1) > minimum)
    {
      ajout += format ("%s%.*lf*x",
                       (*it2)->x1 > 0 ?
                         (ajout == "" ? "" : "+") :
                         "",
                       decimales,
                       (*it2)->x1);
    }
    
    if (fabs ((*it2)->x2) > minimum)
    {
      ajout += format ("%s%.*lf*x²",
                       (*it2)->x2 > 0 ?
                         (ajout == "" ? "" : "+") :
                         "",
                       decimales,
                       (*it2)->x2);
    }
    
    if (fabs ((*it2)->x3) > minimum)
    {
      ajout += format ("%s%.*lf*x³",
                       (*it2)->x3 > 0 ?
                         (ajout == "" ? "" : "+") :
                         "",
                       decimales,
                       (*it2)->x3);
    }
    
    if (fabs ((*it2)->x4) > minimum)
    {
      ajout += format ("%s%.*lf*x⁴",
                       (*it2)->x4 > 0 ?
                         (ajout == "" ? "" : "+") :
                         "",
                       decimales,
                       (*it2)->x4);
    }
    
    if (fabs ((*it2)->x5) > minimum)
    {
      ajout += format ("%s%.*lf*x⁵",
                       (*it2)->x5 > 0 ?
                         (ajout == "" ? "" : "+") :
                         "",
                       decimales,
                       (*it2)->x5);
    }
    
    if (fabs ((*it2)->x6) > minimum)
    {
      ajout += format ("%s%.*lf*x⁶",
                       (*it2)->x6 > 0 ?
                         (ajout == "" ? "" : "+") :
                         "",
                       decimales,
                       (*it2)->x6);
    }
    
    if (ajout == "")
    {
      retour += format ("%.*lf", decimales, 0.);
    }
    else
    {
      retour += ajout;
    }
    
    if (index != NULL)
    {
      std::string tmp2;
      
      tmp2 = _1990_ponderations_description (*it);
      retour += " (" + tmp2 + ")";
      
      ++it;
    }
    ++it2;
  }
  
  return retour;
}


/**
 * \brief Renvoie deux fonctions qui enveloppent la liste des fonctions. 
 * \param fonctions : liste des fonctions à envelopper,
 * \param fonction_min : courbe enveloppe inférieure (en valeur algébrique),
 * \param fonction_max : courbe enveloppe supérieure (en valeur algébrique),
 * \param comb_min : le numéro de la combinaison (dans x0) prépondérante,
 * \param comb_max : le numéro de la combinaison (dans x0) prépondérante.
 * \return
 *   Succès : true.\n
 *   Échec : false :
 *     - fonctions == NULL,
 *     - fonction_min == NULL,
 *     - fonction_max == NULL,
 *     - comb_min,
 *     - comb_max,
 *     - erreur d'allocation mémoire.
 */
bool
common_fonction_renvoie_enveloppe (std::list <Fonction *> *fonctions,
                                   Fonction               *fonction_min,
                                   Fonction               *fonction_max,
                                   Fonction               *comb_min,
                                   Fonction               *comb_max)
{
  Fonction *fonction;
  uint16_t  num;
  
  std::list <Fonction *>::iterator it;
  
  BUGPARAM (fonctions, "%p", fonctions, false)
  BUGPARAM (fonction_min, "%p", fonction_min, false)
  BUGPARAM (fonction_min->t.size () == 0,
            "%zu",
            fonction_min->t.size (),
            false)
  BUGPARAM (fonction_max, "%p", fonction_max, false)
  BUGPARAM (fonction_max->t.size () == 0,
            "%zu",
            fonction_max->t.size (),
            false)
  BUGPARAM (comb_min, "%p", comb_min, false)
  BUGPARAM (comb_min->t.size () == 0,
            "%zu",
            comb_min->t.size (),
            false)
  BUGPARAM (comb_max, "%p", comb_max, false)
  BUGPARAM (comb_max->t.size () == 0,
            "%zu",
            comb_max->t.size (),
            false)
  
#define FREE_ALL { \
  for_each (fonction_min->t.begin (), \
            fonction_min->t.end (), \
            std::default_delete <Troncon> ()); \
  for_each (fonction_max->t.begin (), \
            fonction_max->t.end (), \
            std::default_delete <Troncon> ()); \
  for_each (comb_min->t.begin (), \
            comb_min->t.end (), \
            std::default_delete <Troncon> ()); \
  for_each (comb_max->t.begin (), \
            comb_max->t.end (), \
            std::default_delete <Troncon> ()); \
}
  
  it = fonctions->begin ();
  fonction = *it;
  
  BUG (common_fonction_ajout_fonction (fonction_min, fonction, 1.),
       false,
       FREE_ALL)
  BUG (common_fonction_ajout_fonction (fonction_max, fonction, 1.),
       false,
       FREE_ALL)
  BUG (common_fonction_ajout_fonction (comb_min, fonction, 0.),
       false,
       FREE_ALL)
  BUG (common_fonction_ajout_fonction (comb_max, fonction, 0.),
       false,
       FREE_ALL)
  
#undef FREE_ALL
  ++it;
  num = 1;
  while (it != fonctions->end ())
  {
    uint8_t  j;
    double   val[10], x[10];
    Fonction fonction_moins, fonction_bis;
    double   x_base, tmp;
    
    std::list <Troncon *>::iterator it_m, it_b;
    std::list <Troncon *>::iterator it_k, it_c;
    
    fonction = *it;
    
#define FREE_ALL { \
  for_each (fonction_min->t.begin (), \
            fonction_min->t.end (), \
            std::default_delete <Troncon> ()); \
  for_each (fonction_max->t.begin (), \
            fonction_max->t.end (), \
            std::default_delete <Troncon> ()); \
  for_each (comb_min->t.begin (), \
            comb_min->t.end (), \
            std::default_delete <Troncon> ()); \
  for_each (comb_max->t.begin (), \
            comb_max->t.end (), \
            std::default_delete <Troncon> ()); \
  for_each (fonction_bis.t.begin (), \
            fonction_bis.t.end (), \
            std::default_delete <Troncon> ()); \
  for_each (fonction_moins.t.begin (), \
            fonction_moins.t.end (), \
            std::default_delete <Troncon> ()); \
}
    BUG (common_fonction_ajout_fonction (&fonction_moins, fonction, 1.),
         false,
         FREE_ALL)
    BUG (common_fonction_ajout_fonction (&fonction_moins, fonction_max, -1.),
         false,
         FREE_ALL)
    BUG (common_fonction_ajout_fonction (&fonction_bis, fonction, 1.),
         false,
         FREE_ALL)
    
    it_m = fonction_moins.t.begin ();
    it_b = fonction_bis.t.begin ();
    while (it_m != fonction_moins.t.end ())
    {
      int8_t modif;
      double zero1, zero2;
      
      x_base = (*it_m)->debut_troncon;
      BUG (common_fonction_scinde_troncon (&fonction_bis, x_base),
           false,
           FREE_ALL)
      BUG (common_fonction_scinde_troncon (fonction_max, x_base),
           false,
           FREE_ALL)
      BUG (common_fonction_scinde_troncon (comb_max, x_base),
           false,
           FREE_ALL)
      BUG (common_fonction_scinde_troncon (&fonction_moins, x_base),
           false,
           FREE_ALL)
      
      x[0] = x_base;
      val[0] = common_fonction_y (&fonction_moins, x[0], 1);
      for (j = 1; j < 9; j++)
      {
        x[j] = (*it_m)->debut_troncon +
               j * ((*it_m)->fin_troncon -
                    (*it_m)->debut_troncon) / 9.;
        val[j] = common_fonction_y (&fonction_moins, x[j], 0);
      }
      x[9] = (*it_m)->fin_troncon;
      val[9] = common_fonction_y (&fonction_moins, x[9], -1);
      
      // On vérifie en 10 points si la fonction est toujours inférieure à
      // fonction_max. modif = 0 si la fonction en cours d'étude est inférieure
      // à fonction_max. Elle vaut 1 si elle est supérieure.
      if (errmoy (val[0], ERRMOY_DIST))
      {
        modif = -1;
      }
      else if (val[0] < 0.)
      {
        modif = 0;
      }
      else
      {
        modif = 1;
      }
      for (j = 1; j < 10; j++)
      {
        if ((val[j] > 0.) &&
            (!errmoy (val[j], ERRMOY_DIST)) &&
            (modif == -1))
        {
          modif = 1;
        }
        else if ((val[j] > 0.) &&
                 (!errmoy (val[j], ERRMOY_DIST)) &&
                 (modif == 0))
        {
          BUG (common_fonction_cherche_zero (&fonction_moins,
                                             x[j - 1],
                                             x[j],
                                             &zero1,
                                             &zero2),
               false,
               FREE_ALL)
          BUGCRIT ((!std::isnan (zero1)) && (std::isnan (zero2)),
                   false,
                   (gettext ("Zéro impossible à trouver.\n"));
                     FREE_ALL)
          BUG (common_fonction_scinde_troncon (fonction_max, zero1),
               false,
               FREE_ALL)
          BUG (common_fonction_scinde_troncon (comb_max, zero1),
               false,
               FREE_ALL)
          BUG (common_fonction_scinde_troncon (&fonction_bis, zero1),
               false,
               FREE_ALL)
          BUG (common_fonction_scinde_troncon (&fonction_moins, zero1),
               false,
               FREE_ALL)
          x_base = zero1;
          modif = 1;
        }
        else if ((val[j] < 0.) &&
                 (!errmoy (val[j], ERRMOY_DIST)) &&
                (modif == -1))
        {
          modif = 0;
        }
        else if ((val[j] < 0.) &&
                 (!errmoy (val[j], ERRMOY_DIST)) &&
                 (modif == 1))
        {
          BUG (common_fonction_cherche_zero (&fonction_moins,
                                             x[j - 1],
                                             x[j],
                                             &zero1,
                                             &zero2),
               false,
               FREE_ALL)
          BUGCRIT ((!std::isnan (zero1)) && (std::isnan (zero2)),
                   false,
                   (gettext ("Zéro impossible à trouver.\n"));
                     FREE_ALL)
          BUG (common_fonction_scinde_troncon (fonction_max, zero1),
               false,
               FREE_ALL)
          BUG (common_fonction_scinde_troncon (comb_max, zero1),
               false,
               FREE_ALL)
          BUG (common_fonction_scinde_troncon (&fonction_bis, zero1),
               false,
               FREE_ALL)
          BUG (common_fonction_scinde_troncon (&fonction_moins, zero1),
               false,
               FREE_ALL)
          
          it_k = fonction_max->t.begin ();
          it_c = comb_max->t.begin ();
          while (it_k != fonction_max->t.end ())
          {
            if ((*it_k)->fin_troncon > zero1)
            {
              break;
            }
            if ((*it_k)->fin_troncon > x_base)
            {
              (*it_k)->x0 = (*it_b)->x0;
              (*it_k)->x1 = (*it_b)->x1;
              (*it_k)->x2 = (*it_b)->x2;
              (*it_k)->x3 = (*it_b)->x3;
              (*it_k)->x4 = (*it_b)->x4;
              (*it_k)->x5 = (*it_b)->x5;
              (*it_k)->x6 = (*it_b)->x6;
              (*it_c)->x0 = num;
            }
            
            ++it_k;
            ++it_c;
          }
          x_base = zero1;
          modif = 0;
          break;
        }
      }
      if (modif == 1)
      {
        tmp = (*it_m)->fin_troncon;
        BUG (common_fonction_scinde_troncon (fonction_max, tmp),
             false,
             FREE_ALL)
        BUG (common_fonction_scinde_troncon (comb_max, tmp),
             false,
             FREE_ALL)
        BUG (common_fonction_scinde_troncon (&fonction_bis, tmp),
             false,
             FREE_ALL)
        BUG (common_fonction_scinde_troncon (&fonction_moins, tmp),
             false,
             FREE_ALL)
        
        it_k = fonction_max->t.begin ();
        it_c = comb_max->t.begin ();
        while (it_k != fonction_max->t.end ())
        {
          if ((*it_k)->fin_troncon > tmp)
          {
            break;
          }
          if ((*it_k)->fin_troncon > x_base)
          {
            (*it_k)->x0 = (*it_b)->x0;
            (*it_k)->x1 = (*it_b)->x1;
            (*it_k)->x2 = (*it_b)->x2;
            (*it_k)->x3 = (*it_b)->x3;
            (*it_k)->x4 = (*it_b)->x4;
            (*it_k)->x5 = (*it_b)->x5;
            (*it_k)->x6 = (*it_b)->x6;
            (*it_c)->x0 = num;
          }
          
          ++it_k;
          ++it_c;
        }
      }
      
      ++it_m;
      ++it_b;
    }
    BUG (common_fonction_compacte (fonction_max, comb_max), false, FREE_ALL)
    for_each (fonction_bis.t.begin (), \
              fonction_bis.t.end (), \
              std::default_delete <Troncon> ()); \
    for_each (fonction_moins.t.begin (), \
              fonction_moins.t.end (), \
              std::default_delete <Troncon> ()); \
    fonction_moins.t.clear ();
    fonction_bis.t.clear ();
    
    // On passe à la courbe enveloppe inférieure. On utilise exactement le même
    // code que ci-dessus sauf qu'on fait -fonction+fonction_min à la place de
    // fonction-fonction_max. On remplace les fonction_max par fonction_min.
    // C'est tout !
    BUG (common_fonction_ajout_fonction (&fonction_moins,
                                         fonction,
                                         -1.),
         false,
         FREE_ALL)
    BUG (common_fonction_ajout_fonction (&fonction_moins,
                                         fonction_min,
                                         1.),
         false,
         FREE_ALL)
    BUG (common_fonction_ajout_fonction (&fonction_bis, fonction, 1.),
         false,
         FREE_ALL)
    
    it_m = fonction_moins.t.begin ();
    it_b = fonction_bis.t.begin ();
    while (it_m != fonction_moins.t.end ())
    {
      int8_t modif;
      double zero1, zero2;
      
      x_base = (*it_m)->debut_troncon;
      BUG (common_fonction_scinde_troncon (&fonction_bis, x_base),
           false,
           FREE_ALL)
      BUG (common_fonction_scinde_troncon (fonction_min, x_base),
           false,
           FREE_ALL)
      BUG (common_fonction_scinde_troncon (comb_min, x_base), false, FREE_ALL)
      BUG (common_fonction_scinde_troncon (&fonction_moins, x_base),
           false,
           FREE_ALL)
      
      x[0] = x_base;
      val[0] = common_fonction_y (&fonction_moins, x[0], 1);
      for (j = 1; j < 9; j++)
      {
        x[j] = (*it_m)->debut_troncon +
               j * ((*it_m)->fin_troncon -
                    (*it_m)->debut_troncon) / 9.;
        val[j] = common_fonction_y (&fonction_moins, x[j], 0);
      }
      x[9] = (*it_m)->fin_troncon;
      val[9] = common_fonction_y (&fonction_moins, x[9], -1);
      
      // On vérifie en 10 points si la fonction est toujours inférieure à
      // fonction_min. modif = 0 si la fonction en cours d'étude est inférieure
      // à fonction_min. Elle vaut 1 si elle est supérieure.
      // modif vaut temporairement -1 si la valeur de y est proche de 0.
      if (errmoy (val[0], ERRMOY_DIST))
      {
        modif = -1;
      }
      else if (val[0] < 0.)
      {
        modif = 0;
      }
      else
      {
        modif = 1;
      }
      for (j = 1; j < 10; j++)
      {
        if ((val[j] > 0.) &&
            (!errmoy (val[j], ERRMOY_DIST)) &&
            (modif == -1))
        {
          modif = 1;
        }
        else if ((val[j] > 0.) &&
                 (!errmoy (val[j], ERRMOY_DIST)) &&
                 (modif == 0))
        {
          BUG (common_fonction_cherche_zero (&fonction_moins,
                                             x[j - 1],
                                             x[j],
                                             &zero1,
                                             &zero2),
               false,
               FREE_ALL)
          BUGCRIT ((!std::isnan (zero1)) && (std::isnan (zero2)),
                   false,
                   (gettext ("Zéro impossible à trouver.\n"));
                     FREE_ALL)
          BUG (common_fonction_scinde_troncon (fonction_min, zero1),
               false,
               FREE_ALL)
          BUG (common_fonction_scinde_troncon (comb_min, zero1),
               false,
               FREE_ALL)
          BUG (common_fonction_scinde_troncon (&fonction_bis, zero1),
               false,
               FREE_ALL)
          BUG (common_fonction_scinde_troncon (&fonction_moins, zero1),
               false,
               FREE_ALL)
          x_base = zero1;
          modif = 1;
        }
        else if ((val[j] < 0.) &&
                 (!errmoy (val[j], ERRMOY_DIST)) &&
                 (modif == -1))
        {
          modif = 0;
        }
        else if ((val[j] < 0.) &&
                 (!errmoy (val[j], ERRMOY_DIST)) &&
                 (modif == 1))
        {
          BUG (common_fonction_cherche_zero (&fonction_moins,
                                             x[j - 1],
                                             x[j],
                                             &zero1,
                                             &zero2),
               false,
               FREE_ALL)
          BUGCRIT ((!std::isnan (zero1)) && (std::isnan (zero2)),
                   false,
                   (gettext ("Zéro impossible à trouver.\n"));
                     FREE_ALL)
          BUG (common_fonction_scinde_troncon (fonction_min, zero1),
               false,
               FREE_ALL)
          BUG (common_fonction_scinde_troncon (comb_min, zero1),
               false,
               FREE_ALL)
          BUG (common_fonction_scinde_troncon (&fonction_bis, zero1),
               false,
               FREE_ALL)
          BUG (common_fonction_scinde_troncon (&fonction_moins, zero1),
               false,
               FREE_ALL)
          
          it_k = fonction_min->t.begin ();
          it_c = comb_min->t.begin ();
          while (it_k != fonction_min->t.end ())
          {
            if ((*it_k)->fin_troncon > zero1)
            {
              break;
            }
            if ((*it_k)->fin_troncon > x_base)
            {
              (*it_k)->x0 = (*it_b)->x0;
              (*it_k)->x1 = (*it_b)->x1;
              (*it_k)->x2 = (*it_b)->x2;
              (*it_k)->x3 = (*it_b)->x3;
              (*it_k)->x4 = (*it_b)->x4;
              (*it_k)->x5 = (*it_b)->x5;
              (*it_k)->x6 = (*it_b)->x6;
              (*it_c)->x0 = num;
            }
            
            ++it_k;
            ++it_c;
          }
          x_base = zero1;
          modif = 0;
          break;
        }
      }
      if (modif == 1)
      {
        tmp = (*it_m)->fin_troncon;
        BUG (common_fonction_scinde_troncon (fonction_min, tmp),
             false,
             FREE_ALL)
        BUG (common_fonction_scinde_troncon (comb_min, tmp), false, FREE_ALL)
        BUG (common_fonction_scinde_troncon (&fonction_bis, tmp),
             false,
             FREE_ALL)
        BUG (common_fonction_scinde_troncon (&fonction_moins, tmp),
             false,
             FREE_ALL)
        
        
        it_k = fonction_min->t.begin ();
        it_c = comb_min->t.begin ();
        while (it_k != fonction_min->t.end ())
        {
          if ((*it_k)->fin_troncon > tmp)
          {
            break;
          }
          if ((*it_k)->fin_troncon > x_base)
          {
            (*it_k)->x0 = (*it_b)->x0;
            (*it_k)->x1 = (*it_b)->x1;
            (*it_k)->x2 = (*it_b)->x2;
            (*it_k)->x3 = (*it_b)->x3;
            (*it_k)->x4 = (*it_b)->x4;
            (*it_k)->x5 = (*it_b)->x5;
            (*it_k)->x6 = (*it_b)->x6;
            (*it_c)->x0 = num;
          }
        }
      }
      
      ++it_m;
      ++it_b;
    }
    BUG (common_fonction_compacte (fonction_min, comb_min), false, FREE_ALL)
    
    for_each (fonction_bis.t.begin (), \
              fonction_bis.t.end (), \
              std::default_delete <Troncon> ()); \
    for_each (fonction_moins.t.begin (), \
              fonction_moins.t.end (), \
              std::default_delete <Troncon> ()); \
    fonction_moins.t.clear ();
    fonction_bis.t.clear ();
    
    ++it;
    num++;
  }
  
  return true;
}


/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
