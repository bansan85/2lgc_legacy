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
#include <cmath>

#include "common_projet.hpp"
#include "common_math.hpp"
#include "common_erreurs.hpp"
#include "common_selection.hpp"
#include "common_text.hpp"
#include "1992_1_1_barres.hpp"
#include "1993_1_1_materiaux.hpp"
#include "EF_calculs.hpp"
#include "EF_materiaux.hpp"

#ifdef ENABLE_GTK
#include "common_gtk.hpp"
#endif


/**
 * \brief Ajoute un matériau en acier.
 * \param p : la variable projet,
 * \param nom : nom du nouveau matériau,
 * \param fy : limite d’élasticité en MPa,
 * \param fu : résistance à la traction en MPa.
 * \return
 *   Succès : pointeur vers le nouveau matériau.
 *   Échec : NULL :
 *     - p == NULL,
 *     - en cas d'erreur d'allocation mémoire.
 */
EF_Materiau *
_1993_1_1_materiaux_ajout (Projet      *p,
                           std::string *nom,
                           Flottant     fy,
                           Flottant     fu)
{
  std::unique_ptr <EF_Materiau>    materiau_nouveau (new EF_Materiau);
  std::unique_ptr <Materiau_Acier> data_acier (new Materiau_Acier);
  
  EF_Materiau *mat;
  
  BUGPARAM (p, "%p", p, NULL)
  
  materiau_nouveau->type = MATERIAU_ACIER;
  
  materiau_nouveau->nom.assign (*nom);
  
  data_acier->fy = m_f (m_g (fy) * 1000000., fy.type);
  data_acier->fu = m_f (m_g (fu) * 1000000., fu.type);
  data_acier->e  = m_f (MODULE_YOUNG_ACIER, FLOTTANT_ORDINATEUR);
  data_acier->nu = m_f (COEFFICIENT_NU_ACIER, FLOTTANT_ORDINATEUR);
  
  mat = materiau_nouveau.release ();
  mat->data = data_acier.release ();
  
  BUG (EF_materiaux_insert (p, mat),
       NULL,
         delete (Materiau_Acier *) mat->data;
         delete mat; )
  
  return mat;
}


/**
 * \brief Modifie un matériau acier.
 * \param p : la variable projet,
 * \param materiau : le matériau à modifier,
 * \param nom : nouvelle valeur de nom, peut être NULL.
 * \param fy : nouvelle valeur de fy, peut être NAN.
 * \param fu : nouvelle valeur de fu, peut être NAN.
 * \param e : nouvelle valeur de e, peut être NAN.
 * \param nu : nouvelle valeur de nu, peut être NAN.
 * \return
 *   Succès : true.
 *   Échec : false :
 *     - p == NULL,
 *     - materiau == NULL.
 */
bool
_1993_1_1_materiaux_modif (Projet      *p,
                           EF_Materiau *materiau,
                           std::string *nom,
                           Flottant     fy,
                           Flottant     fu,
                           Flottant     e,
                           Flottant     nu)
{
  Materiau_Acier *data_acier;
  
  BUGPARAM (p, "%p", p, false)
  BUGPARAM (materiau, "%p", materiau, false)
  BUGCRIT (materiau->type == MATERIAU_ACIER,
           false,
           (gettext ("Le matériau n'est pas en acier.\n")); )
  
  data_acier = (Materiau_Acier *) materiau->data;
  
  if ((nom != NULL) && (materiau->nom.compare (*nom) != 0))
  {
    INFO (!EF_materiaux_cherche_nom (p, nom, false),
          false,
          (gettext ("Le matériau %s existe déjà.\n"), nom->c_str ()); )
    materiau->nom.assign (*nom);
    
    BUG (EF_materiaux_repositionne (p, materiau), false)
  }
  
  if (!isnan (m_g (fy)))
  {
    data_acier->fy = fy;
  }
  if (!isnan (m_g (fu)))
  {
    data_acier->fu = fu;
  }
  if (!isnan (m_g (e)))
  {
    data_acier->e = e;
  }
  if (!isnan (m_g (nu)))
  {
    data_acier->nu = nu;
  }
  
  if ((!isnan (m_g (fy))) || (!isnan (m_g (fu))) ||
      (!isnan (m_g (e))) || (!isnan (m_g (nu))))
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
 * \brief Renvoie la description d'un matériau acier sous forme d'un texte.
 *        Il convient de libérer le texte renvoyée par la fonction free.
 * \param materiau : matériau à décrire.
 * \return
 *   Succès : Résultat.\n
 *   Échec : NULL :
 *     - materiau == NULL,
 *     - erreur d'allocation mémoire.
 */
std::string
_1993_1_1_materiaux_get_description (EF_Materiau* materiau)
{
  std::string     description;
  std::string     tmp1;
  Materiau_Acier *data_acier;
  
  BUGPARAM (materiau, "%p", materiau, NULL)
  BUGCRIT (materiau->type == MATERIAU_ACIER,
           NULL,
           (gettext ("Le matériau n'est pas en acier.\n")); )
  
  data_acier = (Materiau_Acier *) materiau->data;
  
  conv_f_c (m_f (m_g (data_acier->fy) / 1000000., data_acier->fy.type),
            &tmp1,
            DECIMAL_CONTRAINTE);
  description = format (gettext ("f<sub>y</sub> : %s MPa"), tmp1.c_str ());
  
  // On affiche les différences si le matériau a été personnalisé
  conv_f_c (m_f (m_g (data_acier->fu) / 1000000., data_acier->fu.type),
            &tmp1,
            DECIMAL_CONTRAINTE);
  description += ", " + format (gettext ("f<sub>u</sub> : %s MPa"),
                                         tmp1.c_str ());
  
  if (!errrel (m_g (data_acier->e), MODULE_YOUNG_ACIER))
  {
    conv_f_c (m_f (m_g (data_acier->e) / 1000000., data_acier->e.type),
              &tmp1,
              DECIMAL_CONTRAINTE);
    description += ", " + format (gettext ("E : %s MPa"), tmp1.c_str ());
  }
  
  if (!errrel (m_g (data_acier->nu), COEFFICIENT_NU_ACIER))
  {
    conv_f_c (data_acier->nu, &tmp1, DECIMAL_SANS_UNITE);
    description += ", " + format (gettext ("%s, &#957; : %s"), tmp1.c_str ());
  }
  
  return description;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
