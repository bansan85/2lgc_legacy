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
#include <locale.h>
#include <libintl.h>
#include <math.h>
#include <string.h>
#include <gmodule.h>

#include "common_projet.h"
#include "common_math.h"
#include "common_erreurs.h"
#include "common_selection.h"
#include "1992_1_1_barres.h"
#include "1993_1_1_materiaux.h"
#include "EF_calculs.h"
#include "EF_materiaux.h"

#ifdef ENABLE_GTK
#include "common_gtk.h"
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
_1993_1_1_materiaux_ajout (Projet     *p,
                           const char *nom,
                           Flottant    fy,
                           Flottant    fu)
{
  EF_Materiau    *materiau_nouveau;
  Materiau_Acier *data_acier;
  
  BUGPARAM (p, "%p", p, NULL)
  BUGCRIT (materiau_nouveau = malloc (sizeof (EF_Materiau)),
           NULL,
           (gettext ("Erreur d'allocation mémoire.\n")); )
  BUGCRIT (data_acier = malloc (sizeof (Materiau_Acier)),
           NULL,
           (gettext("Erreur d'allocation mémoire.\n"));
             free (materiau_nouveau); )
  
  materiau_nouveau->type = MATERIAU_ACIER;
  materiau_nouveau->data = data_acier;
  
  BUGCRIT (materiau_nouveau->nom = g_strdup_printf ("%s", nom),
           NULL,
           (gettext ("Erreur d'allocation mémoire.\n"));
             free (materiau_nouveau);
             free (data_acier); )
  
  data_acier->fy = m_f (m_g (fy) * 1000000., fy.type);
  data_acier->fu = m_f (m_g (fu) * 1000000., fu.type);
  data_acier->e  = m_f (MODULE_YOUNG_ACIER, FLOTTANT_ORDINATEUR);
  data_acier->nu = m_f (COEFFICIENT_NU_ACIER, FLOTTANT_ORDINATEUR);
  
  BUG (EF_materiaux_insert (p, materiau_nouveau),
       NULL,
       free (materiau_nouveau->nom);
         free (data_acier);
         free (materiau_nouveau); )
  
  return materiau_nouveau;
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
 *   Succès : TRUE.
 *   Échec : FALSE :
 *     - p == NULL,
 *     - materiau == NULL.
 */
gboolean
_1993_1_1_materiaux_modif (Projet      *p,
                           EF_Materiau *materiau,
                           char        *nom,
                           Flottant     fy,
                           Flottant     fu,
                           Flottant     e,
                           Flottant     nu)
{
  Materiau_Acier *data_acier;
  
  BUGPARAM (p, "%p", p, FALSE)
  BUGPARAM (materiau, "%p", materiau, FALSE)
  BUGCRIT (materiau->type == MATERIAU_ACIER,
           FALSE,
           (gettext ("Le matériau n'est pas en acier.\n")); )
  
  data_acier = materiau->data;
  
  if ((nom != NULL) && (strcmp (materiau->nom, nom) != 0))
  {
    char *tmp;
    
    INFO (!EF_materiaux_cherche_nom (p, nom, FALSE),
          FALSE,
          (gettext ("Le matériau %s existe déjà.\n"), nom); )
    tmp = materiau->nom;
    BUGCRIT (materiau->nom = g_strdup_printf ("%s", nom),
             FALSE,
             (gettext ("Erreur d'allocation mémoire.\n"));
               materiau->nom = tmp; )
    free (tmp);
    BUG (EF_materiaux_repositionne (p, materiau), FALSE)
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
    GList *liste_materiaux = NULL;
    GList *liste_barres_dep;
    
    liste_materiaux = g_list_append (liste_materiaux, materiau);
    BUG (_1992_1_1_barres_cherche_dependances (p,
                                               NULL,
                                               NULL,
                                               NULL,
                                               liste_materiaux,
                                               NULL,
                                               NULL,
                                               NULL,
                                               &liste_barres_dep,
                                               NULL,
                                               FALSE,
                                               FALSE),
         FALSE,
         g_list_free (liste_materiaux); )
    g_list_free (liste_materiaux);
    
    if (liste_barres_dep != NULL)
    {
      BUG (EF_calculs_free (p), FALSE, g_list_free (liste_barres_dep); )
    }
    
    g_list_free (liste_barres_dep);
  }
  
#ifdef ENABLE_GTK
  if (UI_MAT.builder != NULL)
  {
    gtk_widget_queue_resize (GTK_WIDGET (gtk_builder_get_object (
                        UI_MAT.builder, "EF_materiaux_treeview")));
  }
#endif
  
  return TRUE;
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
// coverity[+alloc]
char *
_1993_1_1_materiaux_get_description (EF_Materiau* materiau)
{
  char           *description = NULL;
  char            tmp1[30];
  char           *tmp2;
  Materiau_Acier *data_acier;
  
  BUGPARAM (materiau, "%p", materiau, NULL)
  BUGCRIT (materiau->type == MATERIAU_ACIER,
           NULL,
           (gettext ("Le matériau n'est pas en acier.\n")); )
  
  data_acier = materiau->data;
  
  conv_f_c (m_f (m_g (data_acier->fy) / 1000000., data_acier->fy.type),
            tmp1,
            DECIMAL_CONTRAINTE);
  BUGCRIT (description = g_strdup_printf ("f<sub>y</sub> : %s MPa", tmp1),
           NULL,
           (gettext ("Erreur d'allocation mémoire.\n")); )
  
  // On affiche les différences si le matériau a été personnalisé
  conv_f_c (m_f (m_g (data_acier->fu) / 1000000., data_acier->fu.type),
            tmp1,
            DECIMAL_CONTRAINTE);
  tmp2 = description;
  BUGCRIT (description = g_strdup_printf ("%s, f<sub>u</sub> : %s MPa",
                                          tmp2,
                                          tmp1),
           NULL,
           (gettext ("Erreur d'allocation mémoire.\n"));
             free (tmp2); )
  free (tmp2);
  
  if (!errrel (m_g (data_acier->e), MODULE_YOUNG_ACIER))
  {
    conv_f_c (m_f (m_g (data_acier->e) / 1000000., data_acier->e.type),
              tmp1,
              DECIMAL_CONTRAINTE);
    tmp2 = description;
    BUGCRIT (description = g_strdup_printf ("%s, E : %s MPa", tmp2, tmp1),
             NULL,
             (gettext ("Erreur d'allocation mémoire.\n"));
               free (tmp2); )
    free (tmp2);
  }
  
  if (!errrel (m_g (data_acier->nu), COEFFICIENT_NU_ACIER))
  {
    conv_f_c (data_acier->nu, tmp1, DECIMAL_SANS_UNITE);
    tmp2 = description;
    BUGCRIT (description = g_strdup_printf ("%s, &#957; : %s", tmp2, tmp1),
             NULL,
             (gettext ("Erreur d'allocation mémoire.\n"));
               free (tmp2); )
    free (tmp2);
  }
  
  return description;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
