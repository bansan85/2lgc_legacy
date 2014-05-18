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

#include <memory>
#include <locale>
#include <math.h>

#include "common_projet.hpp"
#include "common_math.hpp"
#include "common_erreurs.hpp"
#include "common_selection.hpp"
#include "common_text.hpp"
#include "1992_1_1_barres.hpp"
#include "1992_1_1_materiaux.hpp"
#include "EF_calculs.hpp"
#include "EF_materiaux.hpp"

#ifdef ENABLE_GTK
#include "common_gtk.hpp"
#endif


/**
 * \brief Renvoie la résistance caractéristique en compression du béton,
 *        mesurée sur cube à 28 jours en fonction de fck en unité SI (Pa).
 * \param fck : Résistance caractéristique en compression du béton, mesurée sur
 *              cylindre à 28 jours, en MPa.
 * \return
 *   Succès : le résultat.\n
 *   Échec : NAN :
 *     - fck > 90.,
 *     - fck < 1.
 */
double
_1992_1_1_materiaux_fckcube (double fck)
{
  INFO ((fck >= 1.) && (fck <= 90.),
        NAN,
        (gettext ("La résistance caractéristique à la compression du béton doit être inférieure ou égale à 90 MPa.\n")); )
  
  // On effectue une interpolation linéaire entre les valeurs fournies par la
  // norme
  if (fck < 12.)
  {
    return fck * 1.25 * 1000000.;
  }
  else if (fck < 16.)
  {
    return 5. * fck / 4. * 1000000.;
  }
  else if (fck < 20.)
  {
    return 5. * fck / 4. * 1000000.;
  }
  else if (fck < 25.)
  {
    return (fck + 5.) * 1000000.;
  }
  else if (fck < 30.)
  {
    return (7. * fck / 5. - 5.) * 1000000.;
  }
  else if (fck < 35.)
  {
    return (8. * fck / 5. - 11.) * 1000000.;
  }
  else if (fck < 40.)
  {
    return (fck + 10.) * 1000000.;
  }
  else if (fck < 45.)
  {
    return (fck + 10.) * 1000000.;
  }
  else if (fck < 50.)
  {
    return (fck + 10.) * 1000000.;
  }
  else if (fck < 55.)
  {
    return (7. * fck / 5. - 10.) * 1000000.;
  }
  else if (fck < 60.)
  {
    return (8. * fck / 5. - 21.) * 1000000.;
  }
  else if (fck < 70.)
  {
    return (fck + 15.) * 1000000.;
  }
  else if (fck < 80.)
  {
    return (fck + 15.) * 1000000.;
  }
  else if (fck <= 90.)
  {
    return (fck + 15.) * 1000000.;
  }
  else
  {
    return NAN;
  }
}


/**
 * \brief Renvoie la valeur moyenne de la résistance en compression du béton,
 *        mesurée sur cylindre en unité SI (Pa).
 * \param fck : Résistance caractéristique en compression du béton, mesurée sur
 *              cylindre à 28 jours, en MPa.
 * \return
 *   Succès : le résultat.\n
 *   Échec : NAN :
 *     - fck > 90.,
 *     - fck < 1.
 */
double
_1992_1_1_materiaux_fcm (double fck)
{
  INFO ((fck >= 1.) && (fck <= 90.),
        NAN,
        (gettext ("La résistance caractéristique à la compression du béton doit être inférieure ou égale à 90 MPa.\n")); )
  
  return (fck + 8.) * 1000000.;
}


/**
 * \brief Renvoie la valeur moyenne de la résistance en traction directe du
 *        béton, mesurée sur cylindre en unité SI (Pa).
 * \param fck : Résistance caractéristique en compression du béton, mesurée sur
 *              cylindre à 28 jours, en MPa.
 * \param fcm : Valeur moyenne de la résistance en compression du béton, en
 *              MPa.
 * \return
 *   Succès : le résultat.
 *   Échec : NAN :
 *     - fck > 90.,
 *     - fck <= 0.
 */
double
_1992_1_1_materiaux_fctm (double fck,
                          double fcm)
{
  INFO ((fck >= 1.) && (fck <= 90.),
        NAN,
        (gettext ("La résistance caractéristique à la compression du béton doit être inférieure ou égale à 90 MPa.\n")); )
  
  if (fck <= 50.)
  {
    return 0.3 * pow (fck, 2. / 3.) * 1000000.;
  }
  else
  {
    return 2.12 * log (1. + (fcm / 10.)) * 1000000.;
  }
}


/**
 * \brief Renvoie la valeur moyenne de la résistance en traction directe du
 *        béton, (fractile 5%) en unité SI (Pa).
 * \param fctm : Valeur moyenne de la résistance en traction directe du béton,
 *               en MPa.
 * \return
 *   Succès : le résultat.
 */
double
_1992_1_1_materiaux_fctk_0_05 (double fctm)
{
  return 0.7 * fctm * 1000000.;
}


/**
 * \brief Renvoie la valeur moyenne de la résistance en traction directe du
 *        béton, (fractile 95%) en unité SI (Pa).
 *\param fctm : Valeur moyenne de la résistance en traction directe du béton,
 *              en MPa.
 * \returnç
 *   Succès : le résultat.
 */
double
_1992_1_1_materiaux_fctk_0_95 (double fctm)
{
  return 1.3 * fctm * 1000000.;
}


/**
 * \brief Renvoie le module d'élasticité sécant du béton en unité SI (Pa).
 * \param fcm : valeur moyenne de la résistance en compression du béton, en
 *              MPa.
 * \return
 *   Succès : le résultat.
 */
double
_1992_1_1_materiaux_ecm (double fcm)
{
  return 22. * pow (fcm / 10., 0.3) * 1000000000.;
}


/**
 * \brief Renvoie la déformation relative en compression du béton au point 1.
 * \param fcm : valeur moyenne de la résistance en compression du béton, en
 *              MPa.
 * \return
 *   Succès : le résultat.
 */
double
_1992_1_1_materiaux_ec1 (double fcm)
{
  return std::min (0.7 * pow (fcm, 0.31), 2.8) / 1000.;
}


/**
 * \brief Renvoie la déformation relative ultime en compression du béton au
 *        point 1.
 * \param fcm : valeur moyenne de la résistance en compression du béton, en
 *              MPa.
 * \param fck : Résistance caractéristique en compression du béton, mesurée sur
 *              cylindre à 28 jours, en MPa.
 * \return
 *   Succès : le résultat.\n
 *   Échec : NAN :
 *     - fck > 90.,
 *     - fck <= 0.
 */
double
_1992_1_1_materiaux_ecu1 (double fcm,
                          double fck)
{
  INFO ((fck >= 1.) && (fck <= 90.),
        NAN,
        (gettext ("La résistance caractéristique à la compression du béton doit être inférieure ou égale à 90 MPa.\n")); )
  
  if (fck < 50.)
  {
    return 3.5 / 1000.;
  }
  else
  {
    return (2.8 + 27. * pow ((98. - fcm) / 100., 4.)) / 1000.;
  }
}


/**
 * \brief Renvoie la déformation relative en compression du béton au point 2.
 * \param fck : Résistance caractéristique en compression du béton, mesurée sur
 *              cylindre à 28 jours, en MPa.
 * \return
 *   Succès : le résultat.\n
 *   Échec : NAN :
 *     - fck > 90.,
 *     - fck <= 0.
 */
double
_1992_1_1_materiaux_ec2 (double fck)
{
  INFO ((fck >= 1.) && (fck <= 90.),
        NAN,
        (gettext ("La résistance caractéristique à la compression du béton doit être inférieure ou égale à 90 MPa.\n")); )
  
  if (fck < 50.)
  {
    return 2. / 1000.;
  }
  else
  {
    return (2. + 0.085 * pow (fck - 50., 0.53)) / 1000.;
  }
}


/**
 * \brief Renvoie la déformation relative ultime en compression du béton au
 *        point 2.
 * \param fck : Résistance caractéristique en compression du béton, mesurée sur
 *               cylindre à 28 jours, en MPa.
 * \return
 *   Succès : le résultat.\n
 *   Échec : NAN :
 *     - fck > 90.,
 *     - fck <= 0.
 */
double
_1992_1_1_materiaux_ecu2 (double fck)
{
  INFO ((fck >= 1.) && (fck <= 90.),
        NAN,
        (gettext ("La résistance caractéristique à la compression du béton doit être inférieure ou égale à 90 MPa.\n")); )
  
  if (fck < 50.)
  {
    return 3.5 / 1000.;
  }
  else
  {
    return (2.6 + 35. * pow ((90. - fck) / 100., 4.)) / 1000.;
  }
}


/**
 * \brief Renvoie la déformation relative en compression du béton au point 3.
 * \param fck : Résistance caractéristique en compression du béton, mesurée sur
 *              cylindre à 28 jours, en MPa.
 * \return
 *   Succès : le résultat,
 *   Échec : NAN :
 *     - fck > 90.,
 *     - fck <= 0.
 */
double
_1992_1_1_materiaux_ec3 (double fck)
{
  INFO ((fck >= 1.) && (fck <= 90.),
        NAN,
        (gettext ("La résistance caractéristique à la compression du béton doit être inférieure ou égale à 90 MPa.\n")); )
  
  if (fck < 50.)
  {
    return 1.75 / 1000.;
  }
  else
  {
    return (1.75 + 0.55 * (fck - 50.) / 40.) / 1000.;
  }
}


/**
 * \brief Renvoie la déformation relative ultime en compression du béton au
 *        point 3.
 * \param fck : Résistance caractéristique en compression du béton, mesurée sur
 *              cylindre à 28 jours, en MPa.
 * \return
 *   Succès : le résultat.\n
 *   Échec : NAN :
 *     - fck > 90.,
 *     - fck <= 0.
 */
double
_1992_1_1_materiaux_ecu3 (double fck)
{
  INFO ((fck >= 1.) && (fck <= 90.),
        NAN,
        (gettext ("La résistance caractéristique à la compression du béton doit être inférieure ou égale à 90 MPa.\n")); )
  
  if (fck < 50.)
  {
    return 3.5 / 1000.;
  }
  else
  {
    return (2.6 + 35 * pow ((90. - fck) / 100., 4.)) / 1000.;
  }
}


/**
 * \brief Renvoie le coefficient n utilisé dans la courbe parabole rectangle.
 * \param fck : Résistance caractéristique en compression du béton, mesurée sur
 *              cylindre à 28 jours, en MPa.
 * \return
 *   Succès : le résultat.\n
 *   Échec : NAN :
 *     - fck > 90.,
 *     - fck <= 0.
 */
double
_1992_1_1_materiaux_n (double fck)
{
  INFO ((fck >= 1.) && (fck <= 90.),
        NAN,
        (gettext ("La résistance caractéristique à la compression du béton doit être inférieure ou égale à 90 MPa.\n")); )
  
  if (fck < 50.)
  {
    return 2.;
  }
  else
  {
    return (1.4 + 23.4 * pow ((90. - fck) / 100., 4.));
  }
}


/**
 * \brief Renvoie le module de cisallement en unité SI (Pa).
 * \param ecm : module d'élasticité sécant du béton en GPa,
 * \param nu : coefficient de poisson.
 * \return
 *   Succès : le résultat.\n
 *   Échec : NAN :
 *     - nu >= 0.5,
 *     - nu <= 0.
 */
double
_1992_1_1_materiaux_gnu (double ecm,
                         double nu)
{
  INFO ((nu > 0.) && (nu < 0.5),
        NAN,
        (gettext ("Le coefficient de poisson doit être compris entre 0 et 0.5, borne exclues.\n")); )
  return ecm / (2. * (1. + nu)) * 1000000000.;
}


/**
 * \brief Ajoute un matériau en béton et calcule ses caractéristiques
 *        mécaniques. Les propriétés du béton sont déterminées conformément au
 *        tableau 3.1 de l'Eurocode 2-1-1 et la valeur de fckcube est
 *        déterminée par interpolation linéaire si nécessaire.
 * \param p : la variable projet,
 * \param nom : nom du nouveau matériau,
 * \param fck : résistance à la compression du béton à 28 jours en MPa.
 * \return
 *   Succès : pointeur vers le nouveau matériau.
 *   Échec : NULL :
 *     - p == NULL,
 *     - fck > 90. ou < 0.,
 *     - en cas d'erreur d'allocation mémoire.
 */
EF_Materiau *
_1992_1_1_materiaux_ajout (Projet      *p,
                           std::string *nom,
                           Flottant     fck)
{
  std::unique_ptr <EF_Materiau>    materiau_nouveau (new EF_Materiau);
  std::unique_ptr <Materiau_Beton> data_beton (new Materiau_Beton);
  
  EF_Materiau *mat;
  
  BUGPARAM (p, "%p", p, NULL)
  INFO ((m_g (fck) >= 1.) && (m_g (fck) <= 90.),
        NULL,
        (gettext ("La résistance caractéristique à la compression du béton doit être inférieure ou égale à 90 MPa.\n")); )
  
  materiau_nouveau->type = MATERIAU_BETON;
  
  data_beton->fck = m_f (m_g (fck) * 1000000., fck.type);
  data_beton->fckcube = m_f (_1992_1_1_materiaux_fckcube (m_g (fck)),
                             FLOTTANT_ORDINATEUR);
  BUG (!isnan (m_g (data_beton->fckcube)), NULL)
  data_beton->fcm = m_f (_1992_1_1_materiaux_fcm (m_g (fck)),
                    FLOTTANT_ORDINATEUR);
  BUG (!isnan (m_g (data_beton->fcm)), NULL)
  data_beton->fctm = m_f (_1992_1_1_materiaux_fctm (m_g (fck),
                                             m_g (data_beton->fcm) / 1000000.),
                          FLOTTANT_ORDINATEUR);
  BUG (!isnan (m_g (data_beton->fctm)), NULL)
  data_beton->fctk_0_05 = m_f (_1992_1_1_materiaux_fctk_0_05 (
                                            m_g (data_beton->fctm) / 1000000.),
                               FLOTTANT_ORDINATEUR);
  BUG (!isnan (m_g (data_beton->fctk_0_05)), NULL)
  data_beton->fctk_0_95 = m_f (_1992_1_1_materiaux_fctk_0_95 (
                                            m_g (data_beton->fctm) / 1000000.),
                               FLOTTANT_ORDINATEUR);
  BUG (!isnan (m_g (data_beton->fctk_0_95)), NULL)
  data_beton->ecm = m_f (_1992_1_1_materiaux_ecm (
                                             m_g (data_beton->fcm) / 1000000.),
                         FLOTTANT_ORDINATEUR);
  BUG (!isnan (m_g (data_beton->ecm)), NULL)
  data_beton->ec1 = m_f (_1992_1_1_materiaux_ec1 (
                                             m_g (data_beton->fcm) / 1000000.),
                         FLOTTANT_ORDINATEUR);
  BUG (!isnan (m_g (data_beton->ec1)), NULL)
  data_beton->ecu1 = m_f (_1992_1_1_materiaux_ecu1 (
                                              m_g (data_beton->fcm) / 1000000.,
                                                    m_g (fck)),
                          FLOTTANT_ORDINATEUR);
  BUG (!isnan (m_g (data_beton->ecu1)), NULL)
  data_beton->ec2 = m_f (_1992_1_1_materiaux_ec2 (m_g (fck)),
                         FLOTTANT_ORDINATEUR);
  BUG (!isnan (m_g (data_beton->ec2)), NULL)
  data_beton->ecu2 = m_f (_1992_1_1_materiaux_ecu2 (m_g (fck)),
                          FLOTTANT_ORDINATEUR);
  BUG (!isnan (m_g (data_beton->ecu2)), NULL)
  data_beton->ec3 = m_f (_1992_1_1_materiaux_ec3 (m_g (fck)),
                         FLOTTANT_ORDINATEUR);
  BUG (!isnan (m_g (data_beton->ec3)), NULL)
  data_beton->ecu3 = m_f (_1992_1_1_materiaux_ecu3 (m_g (fck)),
                          FLOTTANT_ORDINATEUR);
  BUG (!isnan (m_g (data_beton->ecu3)),
       NULL)
  data_beton->n = m_f (_1992_1_1_materiaux_n (m_g (fck)), FLOTTANT_ORDINATEUR);
  BUG (!isnan (m_g (data_beton->n)),
       NULL)
  data_beton->nu = m_f (COEFFICIENT_NU_BETON, FLOTTANT_ORDINATEUR);
  
  materiau_nouveau->nom.assign (*nom);
  
  mat = materiau_nouveau.release ();
  mat->data = data_beton.release ();
  
  BUG (EF_materiaux_insert (p, mat),
       NULL,
         delete (Materiau_Beton *) mat->data;
         delete mat; )
  
  return mat;
}


/**
 * \brief Modifie un matériau béton.
 * \param p : la variable projet,
 * \param materiau : le matériau à modifier,
 * \param nom : nouvelle valeur de nom, peut être NULL.
 * \param fck : nouvelle valeur de fck, peut être NAN.
 * \param fckcube : nouvelle valeur de fckcube, peut être NAN.
 * \param fcm : nouvelle valeur de fcm, peut être NAN.
 * \param fctm : nouvelle valeur de fctm, peut être NAN.
 * \param fctk_0_05 : nouvelle valeur de fctk_0_05, peut être NAN.
 * \param fctk_0_95 : nouvelle valeur de fctk_0_95, peut être NAN.
 * \param ecm : nouvelle valeur de ecm, peut être NAN.
 * \param ec1 : nouvelle valeur de ec1, peut être NAN.
 * \param ecu1 : nouvelle valeur de ecu1, peut être NAN.
 * \param ec2 : nouvelle valeur de ec2, peut être NAN.
 * \param ecu2 : nouvelle valeur de ecu2, peut être NAN.
 * \param n : nouvelle valeur de n, peut être NAN.
 * \param ec3 : nouvelle valeur de ec3, peut être NAN.
 * \param ecu3 : nouvelle valeur de ecu3, peut être NAN.
 * \param nu : nouvelle valeur de nu, peut être NAN.
 * \return
 *   Succès : true.
 *   Échec : false :
 *     - p == NULL,
 *     - materiau == NULL.
 */
bool
_1992_1_1_materiaux_modif (Projet      *p,
                           EF_Materiau *materiau,
                           std::string *nom,
                           Flottant     fck,
                           Flottant     fckcube,
                           Flottant     fcm,
                           Flottant     fctm,
                           Flottant     fctk_0_05,
                           Flottant     fctk_0_95,
                           Flottant     ecm,
                           Flottant     ec1,
                           Flottant     ecu1,
                           Flottant     ec2,
                           Flottant     ecu2,
                           Flottant     n,
                           Flottant     ec3,
                           Flottant     ecu3,
                           Flottant     nu)
{
  Materiau_Beton *data_beton;
  
  BUGPARAM (p, "%p", p, false)
  BUGPARAM (materiau, "%p", materiau, false)
  BUGCRIT (materiau->type == MATERIAU_BETON,
           false,
           (gettext ("Le matériau n'est pas en béton.\n")); )
  
  data_beton = (Materiau_Beton *) materiau->data;
  
  if ((nom != NULL) && (materiau->nom.compare (*nom) != 0))
  {
    INFO (!EF_materiaux_cherche_nom (p, nom, false),
          false,
          (gettext ("Le matériau %s existe déjà.\n"), nom->c_str ()); )
    materiau->nom.assign (*nom);
    
    BUG (EF_materiaux_repositionne (p, materiau), false)
  }
  
  if (!isnan (m_g (fck)))
  {
    data_beton->fck = fck;
  }
  if (!isnan (m_g (fckcube)))
  {
    data_beton->fckcube = fckcube;
  }
  if (!isnan (m_g (fcm)))
  {
    data_beton->fcm = fcm;
  }
  if (!isnan (m_g (fctm)))
  {
    data_beton->fctm = fctm;
  }
  if (!isnan (m_g (fctk_0_05)))
  {
    data_beton->fctk_0_05 = fctk_0_05;
  }
  if (!isnan (m_g (fctk_0_95)))
  {
    data_beton->fctk_0_95 = fctk_0_95;
  }
  if (!isnan (m_g (ecm)))
  {
    data_beton->ecm = ecm;
  }
  if (!isnan (m_g (ec1)))
  {
    data_beton->ec1 = ec1;
  }
  if (!isnan (m_g (ecu1)))
  {
    data_beton->ecu1 = ecu1;
  }
  if (!isnan (m_g (ec2)))
  {
    data_beton->ec2 = ec2;
  }
  if (!isnan (m_g (ecu2)))
  {
    data_beton->ecu2 = ecu2;
  }
  if (!isnan (m_g (ec3)))
  {
    data_beton->ec3 = ec3;
  }
  if (!isnan (m_g (ecu3)))
  {
    data_beton->ecu3 = ecu3;
  }
  if (!isnan (m_g (n)))
  {
    data_beton->n = n;
  }
  if (!isnan (m_g (nu)))
  {
    data_beton->nu = nu;
  }
  
  if ((!isnan (m_g (fck)))       || (!isnan (m_g (fckcube)))   ||
      (!isnan (m_g (fcm)))       || (!isnan (m_g (fctm)))      ||
      (!isnan (m_g (fctk_0_05))) || (!isnan (m_g (fctk_0_95))) ||
      (!isnan (m_g (ecm)))       || (!isnan (m_g (ec1)))       ||
      (!isnan (m_g (ecu1)))      || (!isnan (m_g (ec2)))       ||
      (!isnan (m_g (ecu2)))      || (!isnan (m_g (ec3)))       ||
      (!isnan (m_g (ecu3)))      || (!isnan (m_g (n)))         ||
      (!isnan (m_g (nu))))
  {
    std::list <EF_Materiau *> liste_materiaux;
    std::list <EF_Barre *>   *liste_barres_dep;
    
    liste_materiaux.push_back (materiau);
    BUG (_1992_1_1_barres_cherche_dependances (p,
                                               NULL,
                                               NULL,
                                               NULL,
                                               &liste_materiaux,
                                               NULL,
                                               NULL,
                                               NULL,
                                               NULL,
                                               &liste_barres_dep,
                                               NULL,
                                               NULL,
                                               false),
         false)
    liste_materiaux.clear ();
    
    if (!liste_barres_dep->empty ())
    {
      BUG (EF_calculs_free (p), false, delete liste_barres_dep; )
    }
    
    delete liste_barres_dep;
  }
  
#ifdef ENABLE_GTK
  if (UI_MAT.builder != NULL)
  {
    gtk_widget_queue_resize (GTK_WIDGET (gtk_builder_get_object (
                                    UI_MAT.builder, "EF_materiaux_treeview")));
  }
#endif
  
  return true;
}


/**
 * \brief Renvoie la description d'un matériau béton sous forme d'un texte.
 *        Il convient de libérer le texte renvoyée par la fonction free.
 * \param materiau : matériau à décrire.
 * \return
 *   Succès : Résultat.\n
 *   Échec : NULL :
 *     - materiau == NULL,
 *     - erreur d'allocation mémoire.
 */
std::string
_1992_1_1_materiaux_get_description (EF_Materiau* materiau)
{
  std::string     description;
  std::string     complement ("");
  std::string     tmp1;
  Materiau_Beton *data_beton;
  
  BUGPARAM (materiau, "%p", materiau, NULL)
  BUGCRIT (materiau->type == MATERIAU_BETON,
           NULL,
           (gettext ("Le matériau n'est pas en béton.\n")); )
  
  data_beton = (Materiau_Beton *) materiau->data;
  
  // On affiche les différences si le matériau a été personnalisé
  if (!errrel (m_g (data_beton->fckcube),
               _1992_1_1_materiaux_fckcube (m_g (data_beton->fck) / 1000000.)))
  {
    conv_f_c (m_f (m_g (data_beton->fckcube) / 1000000.,
              data_beton->fckcube.type),
              &tmp1,
              DECIMAL_CONTRAINTE);
    complement = format (gettext ("f<sub>ck,cube</sub> : %s MPa"),
                                  tmp1.c_str ());
  }
  if (!errrel (m_g (data_beton->fcm),
               _1992_1_1_materiaux_fcm (m_g (data_beton->fck) / 1000000.)))
  {
    conv_f_c (m_f (m_g (data_beton->fcm) / 1000000., data_beton->fcm.type),
              &tmp1,
              DECIMAL_CONTRAINTE);
    
    complement = complement +
                 complement == "" ? "" : ", " +
                 format (gettext ("f<sub>cm</sub> : %s MPa"), tmp1.c_str ());
  }
  if (!errrel (m_g (data_beton->fctm),
               _1992_1_1_materiaux_fctm (m_g (data_beton->fck) / 1000000.,
                                         m_g (data_beton->fcm) / 1000000.)))
  {
    conv_f_c (m_f (m_g (data_beton->fctm) / 1000000., data_beton->fctm.type),
              &tmp1,
              DECIMAL_CONTRAINTE);
    
    complement = complement + 
                 complement == "" ? "" : ", " +
                 format (gettext ("f<sub>ctm</sub> : %s MPa"), tmp1.c_str ());
  }
  if (!errrel (m_g (data_beton->fctk_0_05),
            _1992_1_1_materiaux_fctk_0_05 (m_g (data_beton->fctm) / 1000000.)))
  {
    conv_f_c (m_f (m_g (data_beton->fctk_0_05) / 1000000.,
                   data_beton->fctk_0_05.type),
              &tmp1,
              DECIMAL_CONTRAINTE);
    
    complement = complement +
                 complement == "" ? "" : ", " +
                 format (gettext ("f<sub>ctk,0.05</sub> : %s MPa"),
                                  tmp1.c_str ());
  }
  if (!errrel (m_g (data_beton->fctk_0_95),
            _1992_1_1_materiaux_fctk_0_95 (m_g (data_beton->fctm) / 1000000.)))
  {
    conv_f_c (m_f (m_g (data_beton->fctk_0_95) / 1000000.,
                   data_beton->fctk_0_95.type),
              &tmp1,
              DECIMAL_CONTRAINTE);
    
    complement = complement +
                 complement == "" ? "" : ", " +
                 format (gettext ("f<sub>ctk,0.95</sub> : %s MPa"),
                                  tmp1.c_str ());
  }
  if (!errrel (m_g (data_beton->ecm),
               _1992_1_1_materiaux_ecm (m_g (data_beton->fcm) / 1000000.)))
  {
    conv_f_c (m_f (m_g (data_beton->ecm) / 1000000., data_beton->ecm.type),
              &tmp1,
              DECIMAL_CONTRAINTE);
    
    complement = complement +
                 complement == "" ? "" : ", " +
                 format (gettext ("E<sub>cm</sub> : %s MPa"), tmp1.c_str ());
  }
  if (!errrel (m_g (data_beton->ec1),
               _1992_1_1_materiaux_ec1 (m_g (data_beton->fcm) / 1000000.)))
  {
    conv_f_c (m_f (m_g (data_beton->ec1) * 1000., data_beton->ec1.type),
              &tmp1,
              DECIMAL_SANS_UNITE);
    
    complement = complement +
                 complement == "" ? "" : ", " +
                 format (gettext ("&#949;<sub>c1</sub> : %s ‰"),
                                  tmp1.c_str ());
  }
  if (!errrel (m_g (data_beton->ecu1),
               _1992_1_1_materiaux_ecu1 (m_g (data_beton->fcm) / 1000000.,
                                         m_g (data_beton->fck) / 1000000.)))
  {
    conv_f_c (m_f (m_g (data_beton->ecu1) * 1000., data_beton->ecu1.type),
              &tmp1,
              DECIMAL_SANS_UNITE);
    
    complement = complement +
                 complement == "" ? "" : ", " +
                 format (gettext ("&#949;<sub>cu1</sub> : %s ‰"),
                                  tmp1.c_str ());
  }
  if (!errrel (m_g (data_beton->ec2),
               _1992_1_1_materiaux_ec2 (m_g (data_beton->fck) / 1000000.)))
  {
    conv_f_c (m_f (m_g (data_beton->ec2) * 1000., data_beton->ec2.type),
              &tmp1,
              DECIMAL_SANS_UNITE);
    
    complement = complement +
                 complement == "" ? "" : ", " +
                 format (gettext ("&#949;<sub>c2</sub> : %s ‰"),
                                  tmp1.c_str ());
  }
  if (!errrel (m_g (data_beton->ecu2),
               _1992_1_1_materiaux_ecu2 (m_g (data_beton->fck) / 1000000.)))
  {
    conv_f_c (m_f (m_g (data_beton->ecu2) * 1000., data_beton->ecu2.type),
              &tmp1,
              DECIMAL_SANS_UNITE);
    
    complement = complement +
                 complement == "" ? "" : ", " +
                 format (gettext ("&#949;<sub>cu2</sub> : %s ‰"),
                                  tmp1.c_str ());
  }
  if (!errrel (m_g (data_beton->ec3),
               _1992_1_1_materiaux_ec3 (m_g (data_beton->fck) / 1000000.)))
  {
    conv_f_c (m_f (m_g (data_beton->ec3) * 1000., data_beton->ec3.type),
              &tmp1,
              DECIMAL_SANS_UNITE);
    
    complement = complement +
                 complement == "" ? "" : ", " +
                 format (gettext ("&#949;<sub>c3</sub> : %s ‰"),
                                  tmp1.c_str ());
  }
  if (!errrel (m_g (data_beton->ecu3),
               _1992_1_1_materiaux_ecu3 (m_g (data_beton->fck) / 1000000.)))
  {
    conv_f_c (m_f (m_g (data_beton->ecu3) * 1000., data_beton->ecu3.type),
              &tmp1,
              DECIMAL_SANS_UNITE);
    
    complement = complement +
                 complement == "" ? "" : ", " +
                 format (gettext ("&#949;<sub>cu3</sub> : %s ‰"),
                                  tmp1.c_str ());
  }
  if (!errrel (m_g (data_beton->n),
               _1992_1_1_materiaux_n (m_g (data_beton->fck) / 1000000.)))
  {
    conv_f_c (data_beton->n, &tmp1, DECIMAL_SANS_UNITE);
    
    complement = complement +
                 complement == "" ? "" : ", " +
                 format (gettext ("n : %s"), tmp1.c_str ());
  }
  if (!errrel (m_g (data_beton->nu), COEFFICIENT_NU_BETON))
  {
    conv_f_c (data_beton->nu, &tmp1, DECIMAL_SANS_UNITE);
    
    complement = complement +
                 complement == "" ? "" : ", " +
                 format (gettext ("&#957; : %s"), tmp1.c_str ());
  }
  
  conv_f_c (m_f (m_g (data_beton->fck) / 1000000., data_beton->fck.type),
            &tmp1,
            DECIMAL_CONTRAINTE);
  
  if (complement == "")
  {
    description = format (gettext ("f<sub>ck</sub> : %s MPa"), tmp1.c_str ());
  }
  else
  {
    description = format (gettext ("f<sub>ck</sub> : %s MPa avec "),
                                   tmp1.c_str ()) + complement;
  }
  
  return description;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
