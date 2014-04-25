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
#include <string.h>
#include <gmodule.h>

#include "common_projet.h"
#include "common_erreurs.h"
#include "common_math.h"
#include "common_selection.h"
#include "EF_noeuds.h"
#include "EF_calculs.h"
#include "1992_1_1_barres.h"
#include "EF_sections.h"
#include "EF_materiaux.h"

#ifdef ENABLE_GTK
#include "EF_gtk_sections.h"
#include "common_m3d.hpp"
#include "common_gtk.h"
#endif

/**
 * \brief Initialise la liste des sections.
 * \param p : la variable projet.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL.
 */
gboolean
EF_sections_init (Projet *p)
{
  BUGPARAM (p, "%p", p, FALSE)
  
  p->modele.sections = NULL;
  
#ifdef ENABLE_GTK
  UI_SEC.liste_sections = gtk_list_store_new (1, G_TYPE_STRING);
#endif
  
  return TRUE;
}


/**
 * \brief Insère une section. Procédure commune à toutes les sections.
 * \param p : la variable projet,
 * \param section : la section à insérer.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL,
 *     - section == NULL.
 */
static
gboolean
EF_sections_insert (Projet  *p,
                    Section *section)
{
  GList   *list_parcours;
  Section *section_tmp;
  
  BUGPARAM (p, "%p", p, FALSE)
  BUGPARAM (section, "%p", section, FALSE)
  
  list_parcours = p->modele.sections;
  while (list_parcours != NULL)
  {
    section_tmp = list_parcours->data;
    
    if (strcmp (section->nom, section_tmp->nom) < 0)
    {
      break;
    }
    
    list_parcours = g_list_next (list_parcours);
  }
  if (list_parcours == NULL)
  {
    p->modele.sections = g_list_append (p->modele.sections, section);
#ifdef ENABLE_GTK
    gtk_list_store_append (UI_SEC.liste_sections, &section->Iter_liste);
    if (UI_SEC.builder != NULL)
    {
      gtk_tree_store_append (UI_SEC.sections, &section->Iter_fenetre, NULL);
    }
#endif
  }
  else
  {
    p->modele.sections = g_list_insert_before (p->modele.sections,
                                               list_parcours, section);
#ifdef ENABLE_GTK
    gtk_list_store_insert_before (UI_SEC.liste_sections,
                                  &section->Iter_liste,
                                  &section_tmp->Iter_liste);
    if (UI_SEC.builder != NULL)
    {
      gtk_tree_store_insert_before (UI_SEC.sections,
                                    &section->Iter_fenetre,
                                    NULL,
                                    &section_tmp->Iter_fenetre);
    }
#endif
  }
  
#ifdef ENABLE_GTK
  gtk_list_store_set (UI_SEC.liste_sections,
                      &section->Iter_liste,
                      0, section->nom,
                      -1);
  if (UI_SEC.builder != NULL)
  {
    gtk_tree_store_set (UI_SEC.sections,
                        &section->Iter_fenetre,
                        0, section,
                        -1);
  }
#endif
  
  return TRUE;
}


/**
 * \brief Repositionne une section après un renommage. Procédure commune à
 *        toutes les sections.
 * \param p : la variable projet,
 * \param section : la section à repositionner.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL,
 *     - section == NULL.
 */
static
gboolean
EF_sections_repositionne (Projet  *p,
                          Section *section)
{
  GList *list_parcours;
  
  BUGPARAM (p, "%p", p, FALSE)
  BUGPARAM (section, "%p", section, FALSE)
  
  // On réinsère la section au bon endroit
  p->modele.sections = g_list_remove (p->modele.sections, section);
  list_parcours = p->modele.sections;
  while (list_parcours != NULL)
  {
    Section *section_parcours = list_parcours->data;
    
    if (strcmp (section->nom, section_parcours->nom) < 0)
    {
      p->modele.sections = g_list_insert_before (p->modele.sections,
                                                 list_parcours,
                                                 section);
      
#ifdef ENABLE_GTK
      gtk_list_store_move_before (UI_SEC.liste_sections,
                                  &section->Iter_liste,
                                  &section_parcours->Iter_liste);
      if (UI_SEC.builder != NULL)
      {
        gtk_tree_store_move_before (UI_SEC.sections,
                                    &section->Iter_fenetre,
                                    &section_parcours->Iter_fenetre);
      }
      break;
#endif
    }
    
    list_parcours = g_list_next (list_parcours);
  }
  if (list_parcours == NULL)
  {
    p->modele.sections = g_list_append (p->modele.sections, section);
    
#ifdef ENABLE_GTK
    gtk_list_store_move_before (UI_SEC.liste_sections,
                                &section->Iter_liste,
                                NULL);
    if (UI_SEC.builder != NULL)
    {
      gtk_tree_store_move_before (UI_SEC.sections,
                                  &section->Iter_fenetre,
                                  NULL);
    }
#endif
  }
  
#ifdef ENABLE_GTK
  if (UI_BAR.builder != NULL)
  {
    gtk_widget_queue_resize (GTK_WIDGET (gtk_builder_get_object (
                                       UI_BAR.builder, "EF_barres_treeview")));
  }
  gtk_list_store_set (UI_SEC.liste_sections,
                      &section->Iter_liste,
                      0, section->nom,
                      -1);
  if ((UI_SEC_RE.builder != NULL) && (UI_SEC_RE.section == section))
  {
    gtk_text_buffer_set_text (gtk_text_view_get_buffer (GTK_TEXT_VIEW (
                                     gtk_builder_get_object (UI_SEC_RE.builder,
                                    "EF_section_rectangulaire_textview_nom"))),
                              section->nom, -1);
  }
#endif
  
  return TRUE;
}


#define SECTION_AJOUT(TYPE) \
  TYPE    *section_data; \
  Section *section_nouvelle; \
  \
  BUGPARAM (p, "%p", p, NULL) \
  BUGPARAM (nom, "%p", nom, NULL) \
  INFO (!EF_sections_cherche_nom (p, nom, FALSE), \
        NULL, \
        (gettext ("La section %s existe déjà.\n"), nom); ) \
  BUGCRIT (section_nouvelle = malloc (sizeof (Section)), \
           NULL, \
           (gettext ("Erreur d'allocation mémoire.\n")); ) \
  BUGCRIT (section_data = malloc (sizeof (TYPE)), \
           NULL, \
           (gettext ("Erreur d'allocation mémoire.\n")); \
             free (section_nouvelle); ) \
  section_nouvelle->data = section_data; \
  BUGCRIT (section_nouvelle->nom = g_strdup (nom), \
           NULL, \
           (gettext ("Erreur d'allocation mémoire.\n")); \
             free (section_data); \
             free (section_nouvelle); )


/**
 * \brief Ajouter une nouvelle section rectangulaire à la liste des sections.
 * \param p : la variable projet,
 * \param nom : le nom de la section,
 * \param l : la largeur,
 * \param h : la hauteur.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL,
 *     - nom == NULL,
 *     - le nom de la section est déjà existant,
 *     - en cas d'erreur d'allocation mémoire.
 */
Section *
EF_sections_rectangulaire_ajout (Projet     *p,
                                 const char *nom,
                                 Flottant    l,
                                 Flottant    h)
{
  SECTION_AJOUT (Section_T)
  
  section_nouvelle->type = SECTION_RECTANGULAIRE;
  section_data->largeur_retombee = l;
  section_data->hauteur_retombee = h;
  section_data->largeur_table = l;
  section_data->hauteur_table = m_f (0., FLOTTANT_ORDINATEUR);
  
  BUG (EF_sections_insert (p, section_nouvelle),
       NULL,
       free (section_data);
         free (section_nouvelle->nom);
         free (section_nouvelle); )
  
  return section_nouvelle;
}


#define SECTION_MODIF(QUOI, PAR) \
  if ((!isnan (m_g (PAR))) && \
      (!errrel (m_g (section_data->QUOI), m_g (PAR)))) \
    section_data->QUOI = PAR; \
  else \
    PAR = m_f (NAN, FLOTTANT_ORDINATEUR);


// En cas de modification, pensez aussi à modifier
// EF_sections_personnalisee_modif.
#ifdef ENABLE_GTK
#define SECTION_MODIF2 \
  { \
    GList *liste_sections = NULL, *liste_barres_dep; \
    \
    liste_sections = g_list_append (liste_sections, section); \
    BUG (_1992_1_1_barres_cherche_dependances (p, \
                                               NULL, \
                                               NULL, \
                                               liste_sections, \
                                               NULL, \
                                               NULL, \
                                               NULL, \
                                               NULL, \
                                               &liste_barres_dep, \
                                               NULL, \
                                               FALSE, \
                                               FALSE), \
         FALSE, \
         g_list_free (liste_sections); ) \
    g_list_free (liste_sections); \
    \
    if (liste_barres_dep != NULL) \
    { \
      BUG (m3d_actualise_graphique (p, NULL, liste_barres_dep), \
           FALSE, \
           g_list_free (liste_barres_dep); ) \
      g_list_free (liste_barres_dep); \
      BUG (m3d_rafraichit (p), FALSE) \
      BUG (EF_calculs_free (p), FALSE) \
    } \
  } \
  \
  if (UI_SEC.builder != NULL) \
    gtk_widget_queue_resize (GTK_WIDGET (gtk_builder_get_object ( \
                                   UI_SEC.builder, "EF_sections_treeview")));
#else
#define SECTION_MODIF2 \
  { \
    GList *liste_sections = NULL, *liste_barres_dep; \
    \
    liste_sections = g_list_append (liste_sections, section); \
    BUG (_1992_1_1_barres_cherche_dependances (p, \
                                               NULL, \
                                               NULL, \
                                               liste_sections, \
                                               NULL, \
                                               NULL, \
                                               NULL, \
                                               NULL, \
                                               &liste_barres_dep, \
                                               NULL, \
                                               FALSE, \
                                               FALSE), \
         FALSE, \
         g_list_free (liste_sections); ) \
    g_list_free (liste_sections); \
    \
    if (liste_barres_dep != NULL) \
    { \
      g_list_free (liste_barres_dep); \
      BUG (EF_calculs_free (p), FALSE) \
    } \
  }
#endif


/**
 * \brief Modifie une section rectangulaire.
 * \param p : la variable projet,
 * \param section : la section à modifier,
 * \param nom : son nouveau nom, NULL si aucun changement,
 * \param l : sa nouvelle largeur, NAN si aucun changement,
 * \param h : sa nouvelle hauteur, NAN si aucun changement.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL,
 *     - section == NULL,
 *     - section->type != RECTANGULAIRE.
 */
gboolean
EF_sections_rectangulaire_modif (Projet     *p,
                                 Section    *section,
                                 const char *nom,
                                 Flottant    l,
                                 Flottant    h)
{
  Section_T *section_data = section->data;
  
  BUGPARAM (p, "%p", p, FALSE)
  BUGPARAM (section, "%p", section, FALSE)
  INFO (section->type == SECTION_RECTANGULAIRE,
        FALSE,
        (gettext ("La section doit être de type rectangulaire.\n")); )
  
  if ((nom != NULL) && (strcmp (section->nom, nom) != 0))
  {
    char *tmp;
    
    INFO (!EF_sections_cherche_nom (p, nom, FALSE),
          FALSE,
          (gettext ("La section %s existe déjà.\n"), nom); )
    tmp = section->nom;
    INFO (section->nom = g_strdup (nom),
          FALSE,
          (gettext ("Erreur d'allocation mémoire.\n"));
            section->nom = tmp; )
    free (tmp);
    BUG (EF_sections_repositionne (p, section), FALSE)
  }
  
  SECTION_MODIF (largeur_retombee, l)
  SECTION_MODIF (largeur_table, l)
  SECTION_MODIF (hauteur_retombee, h)
  section_data->hauteur_table = m_f (0., FLOTTANT_ORDINATEUR);
  
  if ((!isnan (m_g (l))) || (!isnan (m_g (h))))
  {
    SECTION_MODIF2
  }
  
  return TRUE;
}


/**
 * \brief Ajouter une nouvelle section en T à la liste des sections.
 * \param p : la variable projet,
 * \param nom : nom de la section,
 * \param lt : la largeur de la table,
 * \param lr : la largeur de la retombée,
 * \param ht : la hauteur de la table,
 * \param hr : la hauteur de la retombée.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL,
 *     - nom == NULL,
 *     - en cas d'erreur d'allocation mémoire.
 */
Section *
EF_sections_T_ajout (Projet     *p,
                     const char *nom,
                     Flottant    lt,
                     Flottant    lr,
                     Flottant    ht,
                     Flottant    hr)
{
  SECTION_AJOUT (Section_T)
  
  section_nouvelle->data = section_data;
  
  // Les caractéristiques de la section sont les suivantes :\end{verbatim}
  // \begin{displaymath}
  //   S = lt \cdot ht+la \cdot ha\end{displaymath}\begin{displaymath}
  //   cdg_{haut} = \frac{\frac{lt \cdot ht^2}{2}+la \cdot ha \cdot
  //                (ht+\frac{ha}{2})}{S}\texttt{  et  }
  //   cdg_{bas} = (ht+ha)-cdg_{haut}\texttt{  et  }
  //   cdg_{droite} = \frac{lt}{2}\texttt{  et  }
  //   cdg_{gauche} = \frac{lt}{2}\end{displaymath}\begin{displaymath}
  //   I_y = \frac{lt \cdot ht^3}{12}+\frac{la \cdot ha^3}{12}+
  //         lt \cdot ht \cdot \left(\frac{ht}{2}-cdg_{haut} \right)^2+
  //         la \cdot ha \cdot \left(\frac{ha}{2}-cdg_{bas} \right)^2
  //   \texttt{  et  }
  //   I_z = \frac{ht \cdot lt^3}{12}+\frac{ha \cdot la^3}{12}
  //   \end{displaymath}\begin{displaymath}
  //   J = \frac{a \cdot b^3}{16} \cdot \left[\frac{16}{3}-3.364 \cdot
  //       \frac{b}{a} \cdot \left(1-\frac{b^4}{12 \cdot a^4}\right)\right]+
  //       \frac{aa \cdot bb^3}{16} \cdot \left[\frac{16}{3}-3.364 \cdot
  //       \frac{bb}{aa} \cdot \left(1-\frac{bb^4}{12 \cdot aa^4}\right)\right]
  //   \texttt{ avec }
  //   \substack{a=max(ht,lt)\\b=min(ht,lt)\\aa=max(ha,la)\\bb=min(ha,la)}
  //   \end{displaymath}\begin{verbatim}
  section_nouvelle->type = SECTION_T;
  section_data->largeur_table = lt;
  section_data->largeur_retombee = lr;
  section_data->hauteur_table = ht;
  section_data->hauteur_retombee = hr;
  
  BUG (EF_sections_insert (p, section_nouvelle),
       NULL,
       free (section_data);
         free (section_nouvelle->nom);
         free (section_nouvelle); )
  
  return section_nouvelle;
}


/**
 * \brief Modifie une section en T.
 * \param p : la variable projet,
 * \param section : la section à modifier,
 * \param nom : le nouveau nom de la section, NULL si aucun changement,
 * \param lt : la nouvelle largeur de la table, NAN si aucun changement,
 * \param lr : la nouvelle largeur de la retombée, NAN si aucun changement,
 * \param ht : la nouvelle hauteur de la table, NAN si aucun changement,
 * \param hr : la nouvelle hauteur de la retombée, NAN si aucun changement.
 * \return
 *   Succès : TRUE.
 *   Échec : FALSE :
 *     - p == NULL,
 *     - section == NULL, 
 *     - section->type != T.
 */
gboolean EF_sections_T_modif (Projet     *p,
                              Section    *section,
                              const char *nom,
                              Flottant    lt,
                              Flottant    lr,
                              Flottant    ht,
                              Flottant    hr)
{
  Section_T *section_data = section->data;
  
  BUGPARAM (p, "%p", p, FALSE)
  BUGPARAM (section, "%p", section, FALSE)
  INFO (section->type == SECTION_T,
        FALSE,
        (gettext ("La section doit être de type en T.\n")); )
  
  if ((nom != NULL) && (strcmp (section->nom, nom) != 0))
  {
    char *tmp;
    
    INFO (!EF_sections_cherche_nom (p, nom, FALSE),
          FALSE,
          (gettext ("La section %s existe déjà.\n"), nom); )
    tmp = section->nom;
    BUGCRIT (section->nom = g_strdup (nom),
             FALSE,
             (gettext ("Erreur d'allocation mémoire.\n"));
               section->nom = tmp; )
    free (tmp);
    BUG (EF_sections_repositionne (p, section), FALSE)
#ifdef ENABLE_GTK
    gtk_list_store_set (UI_SEC.liste_sections,
                        &section->Iter_liste,
                        0, section->nom,
                        -1);
    if ((UI_SEC_T.builder != NULL) && (UI_SEC_T.section == section))
    {
      gtk_text_buffer_set_text (gtk_text_view_get_buffer (GTK_TEXT_VIEW (
                                      gtk_builder_get_object (UI_SEC_T.builder,
                                                "EF_section_T_textview_nom"))),
                                nom,
                                -1);
    }
#endif
  }
  
  SECTION_MODIF (largeur_table, lt)
  SECTION_MODIF (largeur_retombee, lr)
  SECTION_MODIF (hauteur_table, ht)
  SECTION_MODIF (hauteur_retombee, hr)
  
  if ((!isnan (m_g (lt))) ||
      (!isnan (m_g (lr))) ||
      (!isnan (m_g (ht))) ||
      (!isnan (m_g (hr))))
  {
    SECTION_MODIF2
  }
  
  return TRUE;
}


/**
 * \brief Ajouter une nouvelle section carrée à la liste des sections.
 * \param p : la variable projet,
 * \param nom : nom de la section,
 * \param cote : le coté.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL,
 *     - en cas d'erreur d'allocation mémoire.
 */
Section *
EF_sections_carree_ajout (Projet    *p,
                          const char *nom,
                          Flottant    cote)
{
  SECTION_AJOUT (Section_T)
  
  section_nouvelle->type = SECTION_CARREE;
  section_data->largeur_retombee = cote;
  section_data->hauteur_retombee = cote;
  section_data->largeur_table = cote;
  section_data->hauteur_table = m_f (0., FLOTTANT_ORDINATEUR);
  
  BUG (EF_sections_insert (p, section_nouvelle),
       NULL,
       free (section_data);
         free (section_nouvelle->nom);
         free (section_nouvelle); )
  
  return section_nouvelle;
}


/**
 * \brief Modifie une section carrée.
 * \param p : la variable projet,
 * \param section : la section à modifier,
 * \param nom : nouveau nom de la section, NULL si aucun changement,
 * \param cote : le nouveau coté, NAN si aucun changement.
 * \return
 *   Succès : TRUE.
 *   Échec : FALSE :
 *     - p == NULL,
 *     - section == NULL,
 *     - section->type != CARREE.
 */
gboolean
EF_sections_carree_modif (Projet     *p,
                          Section    *section,
                          const char *nom,
                          Flottant    cote)
{
  Section_T *section_data = section->data;
  
  BUGPARAM (p, "%p", p, FALSE)
  BUGPARAM (section, "%p", section, FALSE)
  INFO (section->type == SECTION_CARREE,
        FALSE,
        (gettext ("La section doit être de type carrée.\n")); )
  
  if ((nom != NULL) && (strcmp (section->nom, nom) != 0))
  {
    char *tmp;
    
    INFO (!EF_sections_cherche_nom (p, nom, FALSE),
          FALSE,
          (gettext ("La section %s existe déjà.\n"), nom); )
    tmp = section->nom;
    BUGCRIT (section->nom = g_strdup (nom),
             FALSE,
             (gettext ("Erreur d'allocation mémoire.\n"));
               section->nom = tmp; )
    free (section->nom);
    BUG (EF_sections_repositionne (p, section), FALSE)
#ifdef ENABLE_GTK
    gtk_list_store_set (UI_SEC.liste_sections,
                        &section->Iter_liste,
                        0, section->nom,
                        -1);
    if ((UI_SEC_CA.builder != NULL) && (UI_SEC_CA.section == section))
    {
      gtk_text_buffer_set_text (gtk_text_view_get_buffer (GTK_TEXT_VIEW (
                                     gtk_builder_get_object (UI_SEC_CA.builder,
                                           "EF_section_carree_textview_nom"))),
                                nom,
                                -1);
    }
#endif
  }
  
  SECTION_MODIF (largeur_retombee, cote)
  SECTION_MODIF (hauteur_retombee, cote)
  SECTION_MODIF (largeur_table, cote)
  section_data->hauteur_table = m_f (0., FLOTTANT_ORDINATEUR);
  
  if (!isnan (m_g (cote)))
  {
    SECTION_MODIF2
  }
  
  return TRUE;
}


/**
 * \brief Ajouter une nouvelle section circulaire à la liste des sections.
 * \param p : la variable projet,
 * \param nom : nom de la section,
 * \param diametre : le diamètre.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL,
 *     - en cas d'erreur d'allocation mémoire.
 */
Section *
EF_sections_circulaire_ajout (Projet     *p,
                              const char *nom,
                              Flottant    diametre)
{
  SECTION_AJOUT (Section_Circulaire)
  
  // Les caractéristiques de la section sont les suivantes :\end{verbatim}\begin{displaymath}
  //   S = \frac{\pi \cdot diametre^2}{4}\end{displaymath}\begin{displaymath}
  //   cdg_{haut} = \frac{diametre}{2}\texttt{  et  }
  //   cdg_{bas} = \frac{diametre}{2}\texttt{  et  }
  //   cdg_{droite} = \frac{diametre}{2}\texttt{  et  }
  //   cdg_{gauche} = \frac{diametre}{2}\end{displaymath}\begin{displaymath}
  //   I_y = \frac{\pi \cdot diametre^4}{64}\texttt{  et  }
  //   I_z = I_y\texttt{  et  }J = \frac{\pi \cdot diametre^4}{32}
  //   \end{displaymath}\begin{verbatim}
  
  section_nouvelle->type = SECTION_CIRCULAIRE;
  section_data->diametre = diametre;
  
  BUG (EF_sections_insert (p, section_nouvelle),
       NULL,
       free (section_data);
         free (section_nouvelle->nom);
         free (section_nouvelle); )
  
  return section_nouvelle;
}


/**
 * \brief Modifie une section circulaire.
 * \param p : la variable projet,
 * \param section : la section à modifier,
 * \param nom : nom de la section, NULL si aucun changement,
 * \param diametre : le diamètre, NAN si aucun changement.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL,
 *     - section == NULL,
 *     - section->type != CIRCULAIRE.
 */
gboolean
EF_sections_circulaire_modif (Projet     *p,
                              Section    *section,
                              const char *nom,
                              Flottant    diametre)
{
  Section_Circulaire *section_data = section->data;
  
  BUGPARAM (p, "%p", p, FALSE)
  BUGPARAM (section, "%p", section, FALSE)
  INFO (section->type == SECTION_CIRCULAIRE,
        FALSE,
        (gettext ("La section doit être de type circulaire.\n")); )
  
  if ((nom != NULL) && (strcmp (section->nom, nom) != 0))
  {
    char *tmp;
    
    INFO (!EF_sections_cherche_nom (p, nom, FALSE),
          FALSE,
          (gettext ("La section %s existe déjà.\n"), nom); )
    tmp = section->nom;
    BUGCRIT (section->nom = g_strdup (nom),
             FALSE,
             (gettext ("Erreur d'allocation mémoire.\n"));
               section->nom = tmp; )
    free (tmp);
    BUG (EF_sections_repositionne (p, section), FALSE)
#ifdef ENABLE_GTK
    gtk_list_store_set (UI_SEC.liste_sections,
                        &section->Iter_liste,
                        0, section->nom,
                        -1);
    if ((UI_SEC_CI.builder != NULL) && (UI_SEC_CI.section == section))
    {
      gtk_text_buffer_set_text (gtk_text_view_get_buffer (GTK_TEXT_VIEW (
                                     gtk_builder_get_object (UI_SEC_CI.builder,
                                       "EF_section_circulaire_textview_nom"))),
                                nom,
                                -1);
    }
#endif
  }
  
  SECTION_MODIF (diametre, diametre)
  
  if (!isnan (m_g (diametre)))
  {
    SECTION_MODIF2
  }
  
  return TRUE;
}


/**
 * \brief Vérifie la forme d'une section personnalisée.
 * \param forme : la forme à vérifier. Pour mémoire, la liste contient une
 *                liste de GList *points pouvant former des formes
 *                indépendantes.
 *                La forme dessinée depuis la liste des points est
 *                automatiquement fermée (le premier point rejoint le dernier).
 * \param message : TRUE si un message d'erreur doit être affiché en cas
 *                  d'erreur.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - forme == NULL,
 *     - un groupe de points contient au minimum 3 points,
 *     - deux points se suivant ne peuvent être identiques.
 *     - les segments de droite ne doivent pas se couper entre elles.
 */
gboolean
EF_sections_personnalisee_verif_forme (GList *forme,
                                       gboolean message)
{
  GList *list_parcours;
  
  // Maintenant, on va vérifier que les droites ne se coupent pas entre elles.
  list_parcours = forme;
  // Il faut un minimum d'un groupe.
  if (message)
  {
    INFO (list_parcours,
          FALSE,
          (gettext ("La forme de la section est vide.\n")); )
  }
  else if (list_parcours == NULL)
  {
    return FALSE;
  }
  
  while (list_parcours != NULL)
  {
    GList    *list_parcours2;
    EF_Point *point2 = NULL;
    
    list_parcours2 = list_parcours->data;
    
    // Il faut un minimum de trois points par groupe.
    if (message)
    {
      INFO (g_list_next (g_list_next (list_parcours2)),
            FALSE,
            (gettext ("Un groupe de points doit avoir au minimum 3 points.\n")); )
    }
    else if (g_list_next (g_list_next (list_parcours2)) == NULL)
    {
      return FALSE;
    }
    
    point2 = list_parcours2->data;
    
    while (list_parcours2 != NULL)
    {
      double    a, b;
      GList    *list_parcours3, *list_parcours4;
      EF_Point *point1, *point3 = NULL, *point4 = NULL;
      
      point1 = point2;
      if (g_list_next (list_parcours2) != NULL)
      {
        point2 = g_list_next (list_parcours2)->data;
      }
      else // On vérifie que le premier noeud est compatible avec le premier.
      {
        point2 = ((GList *) list_parcours->data)->data;
      }
      
      // On refuse le cas où point1 == point2
      if (message)
      {
        INFO ((!errrel (m_g (point1->x), m_g (point2->x))) ||
              (!errrel (m_g (point1->y), m_g (point2->y))),
              FALSE,
              (gettext ("2 points se suivant ne peuvent avoir les mêmes coordonnées.\n")); )
      }
      else if ((errrel (m_g (point1->x), m_g (point2->x))) &&
               (errrel (m_g (point1->y), m_g (point2->y))))
      {
        return FALSE;
      }
      
      // Equation de la droite passant par les deux points.
      if (errrel (m_g (point1->x), m_g (point2->x)))
      {
        a = NAN;
        b = m_g (point1->x);
      }
      else
      {
        a = (m_g (point1->y) - m_g (point2->y)) /
              (m_g (point1->x) - m_g (point2->x));
        b = -(m_g (point2->x) * m_g (point1->y) -
              m_g (point1->x) * m_g (point2->y)) /
                (m_g (point1->x) - m_g (point2->x));
      }
      
      // Maintenant, on parcours tous les points suivants afin de vérifier si
      // les segments suivants se coupent avec le segment en cours.
      list_parcours3 = list_parcours;
      point4 = point2;
      list_parcours4 = g_list_next (list_parcours2);
      
      while (list_parcours3 != NULL)
      {
        while (list_parcours4 != NULL)
        {
          double a2, b2;
          double xmin1, xmax1;
          double xmin2, xmax2;
          double ymin1, ymax1;
          double ymin2, ymax2;
          
          point3 = point4;
          if (g_list_next (list_parcours4) == NULL)
          {
            point4 = ((GList *) list_parcours3->data)->data;
          }
          else
          {
            point4 = g_list_next (list_parcours4)->data;
          }
          
          // Maintenant, on s'assure qu'il n'y a pas d'intersection
          // entre les deux segments de droite [point1, point2] et
          // [point3, point4].
          if (errrel (m_g (point3->x), m_g (point4->x)))
          {
            a2 = NAN;
            b2 = m_g(point3->x);
          }
          else
          {
            a2 = (m_g (point3->y) - m_g (point4->y)) /
                   (m_g (point3->x) - m_g (point4->x));
            b2 = -(m_g (point4->x) * m_g (point3->y) -
                   m_g (point3->x) * m_g (point4->y)) /
                     (m_g (point3->x) - m_g (point4->x));
          }
          
          xmin1 = MIN (m_g (point1->x), m_g (point2->x));
          xmax1 = MAX (m_g (point1->x), m_g (point2->x));
          xmin2 = MIN (m_g (point3->x), m_g (point4->x));
          xmax2 = MAX (m_g (point3->x), m_g (point4->x));
          ymin1 = MIN (m_g (point1->y), m_g (point2->y));
          ymax1 = MAX (m_g (point1->y), m_g (point2->y));
          ymin2 = MIN (m_g (point3->y), m_g (point4->y));
          ymax2 = MAX (m_g (point3->y), m_g (point4->y));
          
          if ((errrel (xmax1, xmin2)) || (errrel (xmax2, xmin1)))
          {
            // Alors, le seul moyen qu'il y ait une collision est que
            // les deux segments soient verticaux.
            if ((isnan (a)) && (isnan (a2)))
            {
              // Si les deux segments se suivent
              if ((errrel (ymax1, ymin2)) || (errrel (ymax2, ymin1)))
              {
                // OK
              }
              // Si l'un des deux segments commencent ou finit à
              // l'intérieur de l'autre segment.
              else
              {
                if (message)
                {
                  INFO (!(((ymin2 < ymin1) && (ymin1 < ymax2)) ||
                          ((ymin2 < ymax1) && (ymax1 < ymax2)) ||
                          ((ymin1 < ymin2) && (ymin2 < ymax1)) ||
                          ((ymin1 < ymax2) && (ymax2 < ymax1)) ||
                          ((errrel (ymin1, ymin2)) &&
                           (errrel (ymax1, ymax2)))),
                        FALSE,
                        (gettext ("Le segment défini par les points x = %lf, y = %lf et x = %lf, y = %lf se coupe avec celui défini par les points x = %lf, y = %lf et x = %lf, y = %lf.\n"),
                                  m_g (point1->x),
                                  m_g (point1->y),
                                  m_g (point2->x),
                                  m_g (point2->y),
                                  m_g (point3->x),
                                  m_g (point3->y),
                                  m_g (point4->x),
                                  m_g (point4->y)); )
                }
                else if (((ymin2 < ymin1) && (ymin1 < ymax2)) ||
                  ((ymin2 < ymax1) && (ymax1 < ymax2)) ||
                  ((ymin1 < ymin2) && (ymin2 < ymax1)) ||
                  ((ymin1 < ymax2) && (ymax2 < ymax1)) ||
                  ((errrel (ymin1, ymin2)) && (errrel (ymax1, ymax2))))
                {
                  return FALSE;
                }
              }
            }
          }
          // si le segment 1 est vertical et que le deuxième non
          else if (isnan (a))
          {
            double y_b;
            
            // Valeur y du 2ème segment en position x du segment 1
            y_b = a2 * b + b2;
            
            if ((errrel (y_b, ymin1)) || (errrel (y_b, ymax1)))
            {
              // OK
            }
            else
            {
              if (message)
              {
                INFO (!((ymin1 < y_b) &&
                        (y_b < ymax1) &&
                        (xmin1 < xmin2) &&
                        (xmin2 < xmax1)),
                      FALSE,
                      (gettext ("Le segment défini par les points x = %lf, y = %lf et x = %lf, y = %lf se coupe avec celui défini par les points x = %lf, y = %lf et x = %lf, y = %lf.\n"),
                                m_g (point1->x),
                                m_g (point1->y),
                                m_g (point2->x),
                                m_g (point2->y),
                                m_g (point3->x),
                                m_g (point3->y),
                                m_g (point4->x),
                                m_g (point4->y)); )
              }
              else if ((ymin1 < y_b) &&
                       (y_b < ymax1) &&
                       (xmin1 < xmin2) &&
                       (xmin2 < xmax1))
              {
                return FALSE;
              }
            }
          }
          // si le segment 2 est vertical et que le premier non
          else if (isnan (a2))
          {
            double y_b;
            
            // Valeur y du 1er segment en position x du segment 2
            y_b = a * b2 + b;
            
            if ((errrel (y_b, ymin2)) || (errrel (y_b, ymax2)))
            {
              // OK
            }
            else
            {
              if (message)
              {
                INFO (!((ymin2 < y_b) &&
                        (y_b < ymax2) &&
                        (xmin2 < xmin1) &&
                        (xmin1 < xmax2)),
                      FALSE,
                      (gettext ("Le segment défini par les points x = %lf, y = %lf et x = %lf, y = %lf se coupe avec celui défini par les points x = %lf, y = %lf et x = %lf, y = %lf.\n"),
                                m_g (point1->x),
                                m_g (point1->y),
                                m_g (point2->x),
                                m_g (point2->y),
                                m_g (point3->x),
                                m_g (point3->y),
                                m_g (point4->x),
                                m_g (point4->y)); )
              }
              else if ((ymin2 < y_b) &&
                       (y_b < ymax2) &&
                       (xmin2 < xmin1) &&
                       (xmin1 < xmax2))
              {
                return FALSE;
              }
            }
          }
          // Si les deux segments ne sont pas verticaux, on recherche
          // simplement l'intersection f1(x) = f2(x) et on regarde si
          // le x est présent dans les deux segments
          else
          {
            if (errrel (a, a2))
            {
              // La seule possibilité d'une intersection est que les
              // deux b soient identiques.
              if (errrel (b, b2))
              {
                // On regarde si les deux segments ont un x en
                // commun
                if ((errrel (xmax1, xmin2)) || (errrel (xmax2, xmin1)))
                {
                  // OK
                }
                // Si l'un des deux segments commencent ou finit à
                // l'intérieur de l'autre segment.
                else
                {
                  if (message)
                  {
                    INFO (!(((xmin2 < xmin1) && (xmin1 < xmax2)) ||
                            ((xmin2 < xmax1) && (xmax1 < xmax2)) ||
                            ((xmin1 < xmin2) && (xmin2 < xmax1)) ||
                            ((xmin1 < xmax2) && (xmax2 < xmax1))),
                          FALSE,
                          (gettext ("Le segment défini par les points x = %lf, y = %lf et x = %lf, y = %lf se coupe avec celui défini par les points x = %lf, y = %lf et x = %lf, y = %lf.\n"),
                                    m_g (point1->x),
                                    m_g (point1->y),
                                    m_g (point2->x),
                                    m_g (point2->y),
                                    m_g (point3->x),
                                    m_g (point3->y),
                                    m_g (point4->x),
                                    m_g (point4->y)); )
                  }
                  else if (((xmin2 < xmin1) && (xmin1 < xmax2)) ||
                           ((xmin2 < xmax1) && (xmax1 < xmax2)) ||
                           ((xmin1 < xmin2) && (xmin2 < xmax1)) ||
                           ((xmin1 < xmax2) && (xmax2 < xmax1)))
                  {
                    return FALSE;
                  }
                }
              }
            }
            else
            {
              // Calcule de l'intersection
              double x_inter = -(b2 - b) / (a2 - a);
              
              if ((errrel (x_inter, xmin1)) ||
                (errrel (x_inter, xmin2)) ||
                (errrel (x_inter, xmax1)) ||
                (errrel (x_inter, xmax2)))
              {
                // OK
              }
              else
              {
                if (message)
                {
                  INFO (!((xmin1 < x_inter) &&
                          (x_inter < xmax1) &&
                          (xmin2 < x_inter) &&
                          (x_inter < xmax2)),
                        FALSE,
                        (gettext ("Le segment défini par les points x = %lf, y = %lf et x = %lf, y = %lf se coupe avec celui défini par les points x = %lf, y = %lf et x = %lf, y = %lf.\n"),
                                  m_g (point1->x),
                                  m_g (point1->y),
                                  m_g (point2->x),
                                  m_g (point2->y),
                                  m_g (point3->x),
                                  m_g (point3->y),
                                  m_g (point4->x),
                                  m_g (point4->y)); )
                }
                else if ((xmin1 < x_inter) && (x_inter < xmax1) &&
                         (xmin2 < x_inter) && (x_inter < xmax2))
                {
                  return FALSE;
                }
              }
            }
          }
          
          list_parcours4 = g_list_next (list_parcours4);
        }
        
        list_parcours3 = g_list_next (list_parcours3);
        point3 = NULL;
        point4 = NULL;
        if (list_parcours3 == NULL)
        {
          list_parcours4 = NULL;
        }
        else
        {
          list_parcours4 = list_parcours3->data;
          if (list_parcours4 != NULL)
          {
            point4 = list_parcours4->data;
          }
        }
      }
      
      list_parcours2 = g_list_next (list_parcours2);
    }
    
    list_parcours = g_list_next (list_parcours);
  }
  
  return TRUE;
}


/**
 * \brief Ajouter une nouvelle section personnalisée à la liste des sections.
 * \param p : la variable projet,
 * \param nom : nom de la section,
 * \param description : description (géométrique de préférence) de la section,
 * \param j : le moment d’inertie de torsion.
 * \param iy : le moment d'inertie de flexion selon l'axe y,
 * \param iz : le moment d'inertie de flexion selon l'axe z,
 * \param vy : distance entre le centre de gravité et le point le plus haut
 *             selon l'axe y,
 * \param vyp : distance entre le centre de gravité et le point le plus bas
 *              selon l'axe y,
 * \param vz : distance entre le centre de gravité et le point le plus haut
 *             selon l'axe z,
 * \param vzp : distance entre le centre de gravité et le point le plus bas
 *              selon l'axe z,
 * \param s : surface de la section,
 * \param forme : dessin de la section. Il s'agit d'une liste contenant une
 *                liste de points reliés en eux. Cette deuxième liste permet de
 *                dessiner plusieurs formes l'une à coté des autres. Il faut
 *                bien placer le centre de gravité au point 0;0. La liste est
 *                directement ajoutée à la section. Il ne faut donc pas la
 *                libérer une fois l'ajout effectué.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL,
 *     - en cas d'erreur d'allocation mémoire.
 */
Section *
EF_sections_personnalisee_ajout (Projet     *p,
                                 const char *nom,
                                 const char *description,
                                 Flottant    j,
                                 Flottant    iy,
                                 Flottant    iz,
                                 Flottant    vy,
                                 Flottant    vyp,
                                 Flottant    vz,
                                 Flottant    vzp,
                                 Flottant    s,
                                 GList      *forme)
{
  SECTION_AJOUT (Section_Personnalisee)
  
  INFO (EF_sections_personnalisee_verif_forme (forme, TRUE),
        NULL,
        (gettext ("La forme est incorrecte.\n"));
          free (section_data);
          free (section_nouvelle->nom);
          free (section_nouvelle); )
  
  section_nouvelle->type = SECTION_PERSONNALISEE;
  BUGCRIT (section_data->description = g_strdup (description),
           NULL,
           (gettext ("Erreur d'allocation mémoire.\n"));
             free (section_data);
             free (section_nouvelle->nom);
             free (section_nouvelle); )
  section_data->j = j;
  section_data->iy = iy;
  section_data->iz = iz;
  section_data->vy = vy;
  section_data->vyp = vyp;
  section_data->vz = vz;
  section_data->vzp = vzp;
  section_data->s = s;
  section_data->forme = forme;
  
  BUG (EF_sections_insert (p, section_nouvelle),
       NULL,
       free (section_data->description);
         free (section_nouvelle->nom);
         free (section_data);
         free (section_nouvelle); )
  
  return section_nouvelle;
}


/**
 * \brief Modifie une section personnalisée.
 * \param p : la variable projet,
 * \param section : la section à modifier,
 * \param nom : nom de la section, NULL si aucun changement,
 * \param description : description de la section, NULL si aucun changement,
 * \param j : le moment d’inertie de torsion, NAN si aucun changement.
 * \param iy : le moment d'inertie de flexion selon l'axe y, NAN si aucun
 *             changement,
 * \param iz : le moment d'inertie de flexion selon l'axe z, NAN si aucun
 *             changement,
 * \param vy : distance entre le centre de gravité et le point le plus haut
 *             selon l'axe y, NAN si aucun changement,
 * \param vyp : distance entre le centre de gravité et le point le plus bas
 *              selon l'axe y, NAN si aucun changement,
 * \param vz : distance entre le centre de gravité et le point le plus haut
 *             selon l'axe z, NAN si aucun changement,
 * \param vzp : distance entre le centre de gravité et le point le plus bas
 *              selon l'axe z, NAN si aucun changement,
 * \param s : surface de la section, NAN si aucun changement,
 * \param forme : dessin de la section, NULL si aucun changement. 
 * \return
 *   Succès : TRUE.
 *   Échec : FALSE :
 *     - p == NULL,
 *     - section == NULL,
 *     - section->type != SECTION_PERSONNALISEE.
 */
gboolean
EF_sections_personnalisee_modif (Projet     *p,
                                 Section    *section,
                                 const char *nom,
                                 const char *description,
                                 Flottant    j,
                                 Flottant    iy,
                                 Flottant    iz,
                                 Flottant    vy,
                                 Flottant    vyp,
                                 Flottant    vz,
                                 Flottant    vzp,
                                 Flottant    s,
                                 GList      *forme)
{
  Section_Personnalisee *section_data = section->data;
  
  BUGPARAM (p, "%p", p, FALSE)
  BUGPARAM (section, "%p", section, FALSE)
  INFO (section->type == SECTION_PERSONNALISEE,
        FALSE,
        (gettext ("La section doit être de type personnalisée.\n")); )
  
  INFO (EF_sections_personnalisee_verif_forme (forme, TRUE),
        FALSE,
        (gettext ("La forme est incorrecte.\n")); )
  if ((nom != NULL) && (strcmp (section->nom, nom) != 0))
  {
    char *tmp;
    
    INFO (!EF_sections_cherche_nom (p, nom, FALSE),
          FALSE,
          (gettext ("La section %s existe déjà.\n"), nom); )
    tmp = section->nom;
    BUGCRIT (section->nom = g_strdup (nom),
             FALSE,
             (gettext ("Erreur d'allocation mémoire.\n"));
               section->nom = tmp; )
    free (tmp);
    BUG (EF_sections_repositionne (p, section), FALSE)
#ifdef ENABLE_GTK
    gtk_list_store_set (UI_SEC.liste_sections,
                        &section->Iter_liste,
                        0, section->nom,
                        -1);
    if ((UI_SEC_PE.builder != NULL) && (UI_SEC_PE.section == section))
    {
      gtk_text_buffer_set_text (gtk_text_view_get_buffer (GTK_TEXT_VIEW
                                    (gtk_builder_get_object (UI_SEC_PE.builder,
                                    "EF_section_personnalisee_textview_nom"))),
                                nom,
                                -1);
    }
#endif
  }
  
  if ((description != NULL) &&
      (strcmp (section_data->description, description) != 0))
  {
    char *tmp;
    
    tmp = section_data->description;
    BUGCRIT (section_data->description = g_strdup (description),
             FALSE,
             (gettext ("Erreur d'allocation mémoire.\n"));
               section_data->description = tmp; )
    free (tmp);
  }
  
  SECTION_MODIF (j, j)
  SECTION_MODIF (iy, iy)
  SECTION_MODIF (iz, iz)
  SECTION_MODIF (vy, vy)
  SECTION_MODIF (vyp, vyp)
  SECTION_MODIF (vz, vz)
  SECTION_MODIF (vzp, vzp)
  SECTION_MODIF (s, s)
  
  if (forme != NULL)
  {
    g_list_free_full (section_data->forme,
                      (GDestroyNotify) EF_sections_personnalisee_free_forme1);
    
    section_data->forme = forme;
  }
  
  // Ici, il n'est pas possible d'utiliser SECTION_MODIF2 à cause de forme.
  if ((!isnan (m_g (j))) ||
      (!isnan (m_g (iy))) ||
      (!isnan (m_g (iz))) ||
      (!isnan (m_g (vy))) ||
      (!isnan (m_g (vyp))) ||
      (!isnan (m_g (vz))) ||
      (!isnan (m_g (vzp))) ||
      (!isnan (m_g (s))) ||
      (forme != NULL))
  {
    GList *liste_sections = NULL, *liste_barres_dep;
    
    liste_sections = g_list_append (liste_sections, section);
    BUG (_1992_1_1_barres_cherche_dependances (p,
                                               NULL,
                                               NULL,
                                               liste_sections,
                                               NULL,
                                               NULL,
                                               NULL,
                                               NULL,
                                               &liste_barres_dep,
                                               NULL,
                                               FALSE,
                                               FALSE),
         FALSE,
         g_list_free (liste_sections); )
    g_list_free (liste_sections);
    
    if (liste_barres_dep != NULL)
    {
#ifdef ENABLE_GTK
      if (forme != NULL)
      {
        BUG (m3d_actualise_graphique (p, NULL, liste_barres_dep),
             FALSE,
             g_list_free (liste_barres_dep); )
        BUG (m3d_rafraichit (p),
             FALSE,
             g_list_free (liste_barres_dep); )
      }
#endif
      g_list_free (liste_barres_dep);
      if ((!isnan (m_g (j))) ||
          (!isnan (m_g (iy))) ||
          (!isnan (m_g (iz))) ||
          (!isnan (m_g (vy))) ||
          (!isnan (m_g (vyp))) ||
          (!isnan (m_g (vz))) ||
          (!isnan (m_g (vzp))) ||
          (!isnan (m_g (s))))
        {
          BUG (EF_calculs_free (p), FALSE)
        }
    }
  }
  
#ifdef ENABLE_GTK
  if (UI_SEC.builder != NULL)
  {
    gtk_widget_queue_resize (GTK_WIDGET (gtk_builder_get_object (
                                     UI_SEC.builder, "EF_sections_treeview")));
  }
#endif
  
  return TRUE;
}


/**
 * \brief Libère la forme d'une section (liste ET points). Cette fonction doit
 *        être appelée avec g_list_free_full.
 *        Pour libérer seulement la liste sans les points, il suffit
 *        d'appeler g_list_free_full (forme, (GDestroyNotify) g_list_free);
 * \param forme : la forme à libérer,
 */
void EF_sections_personnalisee_free_forme1 (GList *forme)
{
  g_list_free_full (forme, (GDestroyNotify) free);
}


/**
 * \brief Positionne dans la liste des sections l'élément courant au numéro
 *        souhaité.
 * \param p : la variable projet,
 * \param nom : le nom de la section.
 * \param critique : TRUE si en cas d'echec, la fonction BUG est utilisée
 * \return
 *   Succès : pointeur vers la section.\n
 *   Échec : NULL :
 *     - p == NULL,
 *     - section introuvable.
 */
Section *
EF_sections_cherche_nom (Projet     *p,
                         const char *nom,
                         gboolean    critique)
{
  GList *list_parcours;
  
  BUGPARAM (p, "%p", p, NULL)
  
  list_parcours = p->modele.sections;
  while (list_parcours != NULL)
  {
    Section *section = list_parcours->data;
    
    if (strcmp (section->nom, nom) == 0)
    {
      return section;
    }
    
    list_parcours = g_list_next (list_parcours);
  }
  
  if (critique)
  {
    FAILINFO (NULL, (gettext ("Section '%s' introuvable.\n"), nom); )
  }
  else
  {
    return NULL;
  }
}


/**
 * \brief Renvoie la description d'une section sous forme d'un texte.
 *        Il convient de libérer le texte renvoyée par la fonction free.
 * \param sect : section à étudier.
 * \return
 *   Succès : Résultat.\n
 *   Échec : NULL :
 *     - sect == NULL,
 *     - erreur d'allocation mémoire.
 */
char *
EF_sections_get_description (Section *sect)
{
  char *description;
  
  BUGPARAM (sect, "%p", sect, NULL)
  
  switch (sect->type)
  {
    case SECTION_RECTANGULAIRE :
    {
      char       larg[30], haut[30];
      Section_T *section = sect->data;
      
      conv_f_c (section->largeur_retombee, larg, DECIMAL_DISTANCE);
      conv_f_c (section->hauteur_retombee, haut, DECIMAL_DISTANCE);
      BUGCRIT (description = g_strdup_printf ("%s : %s m, %s : %s m",
                                              gettext ("Largeur"),
                                              larg,
                                              gettext ("Hauteur"),
                                              haut),
               NULL,
               (gettext ("Erreur d'allocation mémoire.\n")); )
      
      return description;
    }
    case SECTION_T :
    {
      char       larg_t[30], haut_t[30], larg_r[30], haut_r[30];
      Section_T *section = sect->data;
      
      conv_f_c (section->largeur_table, larg_t, DECIMAL_DISTANCE);
      conv_f_c (section->largeur_retombee, larg_r, DECIMAL_DISTANCE);
      conv_f_c (section->hauteur_table, haut_t, DECIMAL_DISTANCE);
      conv_f_c (section->hauteur_retombee, haut_r, DECIMAL_DISTANCE);
      BUGCRIT (description = g_strdup_printf ("%s : %s m, %s : %s m, %s : %s m, %s : %s m", //NS
                                              gettext ("Largeur table"),
                                              larg_t,
                                              gettext ("Hauteur table"),
                                              haut_t,
                                              gettext ("Largeur retombée"),
                                              larg_r,
                                              gettext ("Hauteur retombée"),
                                              haut_r),
               NULL,
               (gettext ("Erreur d'allocation mémoire.\n")); )
      
      return description;
    }
    case SECTION_CARREE :
    {
      char       cote[30];
      Section_T *section = sect->data;
      
      conv_f_c (section->largeur_table, cote, DECIMAL_DISTANCE);
      BUGCRIT (description = g_strdup_printf ("%s : %s m",
                                              gettext ("Coté"),
                                              cote),
               NULL,
               (gettext ("Erreur d'allocation mémoire.\n")); )
      
      return description;
    }
    case SECTION_CIRCULAIRE :
    {
      char                diam[30];
      Section_Circulaire *section = sect->data;
      
      conv_f_c (section->diametre, diam, DECIMAL_DISTANCE);
      BUGCRIT (description = g_strdup_printf ("%s : %s m",
                                              gettext ("Diamètre"),
                                              diam),
               NULL,
               (gettext ("Erreur d'allocation mémoire.\n")); )
      
      return description;
    }
    case SECTION_PERSONNALISEE :
    {
      Section_Personnalisee *section = sect->data;
      
      BUGCRIT (description = g_strdup (section->description),
               NULL,
               (gettext ("Erreur d'allocation mémoire.\n")); )
      
      return description;
    }
    default :
    {
      FAILCRIT (NULL,
                (gettext ("Type de section %d inconnu.\n"),
                          sect->type); )
      break;
    }
  }
}


/**
 * \brief Fonction permettant de libérer une section. Doit être utilisée avec
 *        g_list_free_full.
 * \param section : section à libérer.
 * \return Rien.
 */
void
EF_sections_free_un (Section *section)
{
  switch (section->type)
  {
    case SECTION_RECTANGULAIRE :
    case SECTION_T :
    case SECTION_CARREE :
    case SECTION_CIRCULAIRE :
      break;
    case SECTION_PERSONNALISEE :
    {
      Section_Personnalisee *section2 = section->data;
      
      free (section2->description);
      g_list_free_full (section2->forme,
                       (GDestroyNotify) EF_sections_personnalisee_free_forme1);
      
      break;
    }
    default :
    {
      FAILCRIT ( ,
                (gettext ("Type de section %d inconnu.\n"),
                          section->type); )
      break;
    }
  }
  free (section->data);
  free (section->nom);
  free (section);
  
  return;
}




/**
 * \brief Supprime la section spécifiée.
 * \param section : la section à supprimer,
 * \param annule_si_utilise : possibilité d'annuler la suppression si la
 *                            section est attribuée à une barre. Si l'option
 *                            est désactivée, les barres (et les barres et
 *                            noeuds intermédiaires dépendants) utilisant la
 *                            section seront supprimées.
 * \param p : la variable projet.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL,
 *     - section == NULL.
 */
gboolean
EF_sections_supprime (Section *section,
                      gboolean annule_si_utilise,
                      Projet  *p)
{
  GList *liste_sections = NULL, *liste_barres_dep;
  
  BUGPARAM (p, "%p", p, FALSE)
  BUGPARAM (section, "%p", section, FALSE)
  
  // On vérifie les dépendances.
  liste_sections = g_list_append (liste_sections, section);
  BUG (_1992_1_1_barres_cherche_dependances (p,
                                             NULL,
                                             NULL,
                                             liste_sections,
                                             NULL,
                                             NULL,
                                             NULL,
                                             NULL,
                                             &liste_barres_dep,
                                             NULL,
                                             FALSE,
                                             FALSE),
       FALSE,
       g_list_free (liste_sections); )
  g_list_free (liste_sections);
  
  if ((annule_si_utilise) && (liste_barres_dep != NULL))
  {
    char *liste;
    
    BUG (liste = common_selection_barres_en_texte (liste_barres_dep),
         FALSE,
         g_list_free (liste_barres_dep); )
    if (g_list_next (liste_barres_dep) == NULL)
    {
      FAILINFO (FALSE,
                (gettext ("Impossible de supprimer la section car elle est utilisée par la barre %s.\n"),
                          liste);
                  g_list_free (liste_barres_dep);
                  free (liste); )
    }
    else
    {
      FAILINFO (FALSE,
                (gettext ("Impossible de supprimer la section car elle est utilisée par les barres %s.\n"),
                          liste);
                  g_list_free (liste_barres_dep);
                  free (liste); )
    }
  }
  
  BUG (_1992_1_1_barres_supprime_liste (p, NULL, liste_barres_dep),
       FALSE,
       g_list_free (liste_barres_dep); )
  g_list_free (liste_barres_dep);
  
  p->modele.sections = g_list_remove (p->modele.sections, section);
  
#ifdef ENABLE_GTK
  gtk_list_store_remove (UI_SEC.liste_sections, &section->Iter_liste);
  if (UI_SEC.builder != NULL)
  {
    gtk_tree_store_remove (UI_SEC.sections, &section->Iter_fenetre);
  }
  switch (section->type)
  {
    case SECTION_RECTANGULAIRE :
    {
      if ((UI_SEC_RE.builder != NULL) && (UI_SEC_RE.section == section))
      {
        gtk_widget_destroy (UI_SEC_RE.window);
      }
      break;
    }
    case SECTION_T :
    {
      if ((UI_SEC_T.builder != NULL) && (UI_SEC_T.section == section))
      {
        gtk_widget_destroy (UI_SEC_T.window);
      }
      break;
    }
    case SECTION_CARREE :
    {
      if ((UI_SEC_CA.builder != NULL) && (UI_SEC_CA.section == section))
      {
        gtk_widget_destroy (UI_SEC_CA.window);
      }
      break;
    }
    case SECTION_CIRCULAIRE :
    {
      if ((UI_SEC_CI.builder != NULL) && (UI_SEC_CI.section == section))
      {
        gtk_widget_destroy (UI_SEC_CI.window);
      }
      break;
    }
    case SECTION_PERSONNALISEE :
    {
      if ((UI_SEC_PE.builder != NULL) && (UI_SEC_PE.section == section))
      {
        gtk_widget_destroy (UI_SEC_PE.window);
      }
      break;
    }
    default :
    {
      FAILCRIT (FALSE,
                (gettext ("Type de section %d inconnu.\n"), section->type); )
      break;
    }
  }
#endif
  
  EF_sections_free_un (section);
  
  return TRUE;
}


/**
 * \brief Renvoie l'inertie de torsion J pour la section étudiée.
 * \param sect : section à étudier.
 * \return
 *   Succès : Résultat.\n
 *   Échec : NAN :
 *     - sect == NULL,
 *     - type de section inconnu.
 */
Flottant
EF_sections_j (Section *sect)
{
  BUGPARAM (sect, "%p", sect, m_f (NAN, FLOTTANT_ORDINATEUR))
  
  switch (sect->type)
  {
    case SECTION_RECTANGULAIRE :
    case SECTION_T :
    case SECTION_CARREE :
    {
      Section_T *section = sect->data;
      double     lt = m_g (section->largeur_table);
      double     lr = m_g (section->largeur_retombee);
      double     ht = m_g (section->hauteur_table);
      double     hr = m_g (section->hauteur_retombee);
      double     a, b, aa, bb;
      
      if (lt > ht)
      {
        a = lt;
        b = ht;
      }
      else
      {
        a = ht;
        b = lt;
      }
      if (lr > hr)
      {
        aa = lr;
        bb = hr;
      }
      else
      {
        aa = hr;
        bb = lr;
      }
      if (sect->type == SECTION_RECTANGULAIRE)
      {
        return m_f (aa * bb * bb * bb / 16. * (16. / 3. - 3.364 * bb / aa *
                         (1. - bb * bb * bb * bb / (12. * aa * aa * aa * aa))),
                    FLOTTANT_ORDINATEUR);
      }
      else
      {
        return m_f (a * b * b * b / 16. * (16. / 3. - 3.364 * b / a *
                      (1. - b * b * b * b / (12. * a * a * a * a))) +
                      aa * bb * bb * bb / 16. * (16. / 3. - 3.364 * bb / aa *
                      (1 - bb * bb * bb * bb / (12. * aa * aa * aa * aa))),
                    FLOTTANT_ORDINATEUR);
      }
      
  // Pour une section en T de section constante (lt : largeur de la table,
  // lr : largeur de la retombée, ht : hauteur de la table, hr : hauteur de la
  // retombée), J vaut :\end{verbatim}\begin{displaymath}
  // J = \frac{a \cdot b^3}{16} \left[\frac{16}{3}-3.364 \frac{b}{a}
  //     \left(1-\frac{b^4}{12 a^4}\right)\right]+\frac{aa \cdot bb^3}{16}
  //     \left[\frac{16}{3}-3.364 \frac{bb}{aa} \left(1-\frac{bb^4}{12 aa^4}
  //     \right)\right]\texttt{ avec }\substack{a=max(h_t,l_t)\\
  //     b=min(h_t,l_t)\\aa=max(h_r,l_r)\\bb=min(h_r,l_r)}\end{displaymath}
  //     \begin{verbatim}
    }
    case SECTION_CIRCULAIRE :
    {
      Section_Circulaire *section = sect->data;
      double              diametre = m_g (section->diametre);
      
      return m_f (M_PI * diametre * diametre * diametre * diametre / 32.,
                  FLOTTANT_ORDINATEUR);
      
  // Pour une section circulaire de section constante, J vaut :\end{verbatim}
  // \begin{displaymath}
  // J = \frac{\pi \cdot \phi^4}{32}\end{displaymath}\begin{verbatim}
    }
    case SECTION_PERSONNALISEE :
    {
      Section_Personnalisee *section = sect->data;
      
      return section->j;
    }
    default :
    {
      FAILCRIT (m_f (NAN, FLOTTANT_ORDINATEUR),
                (gettext ("Type de section %d inconnu.\n"), sect->type); )
      break;
    }
  }
}


/**
 * \brief Renvoie l'inertie I selon l'axe y lorsque la section est constante.
 * \param sect : section à étudier,
 * \return
 *   Succès : Résultat.\n
 *   Échec : NAN :
 *     - section == NULL,
 *     - type de section inconnu.
 */
Flottant
EF_sections_iy (Section *sect)
{
  BUGPARAM (sect, "%p", sect, m_f (NAN, FLOTTANT_ORDINATEUR))
  
  switch (sect->type)
  {
    case SECTION_RECTANGULAIRE :
    case SECTION_T :
    case SECTION_CARREE :
    {
      Section_T *section = sect->data;
      double     lt = m_g (section->largeur_table);
      double     lr = m_g (section->largeur_retombee);
      double     ht = m_g (section->hauteur_table);
      double     hr = m_g( section->hauteur_retombee);
      
  // Pour une section en T de section constante (lt : largeur de la table,
  // lr : largeur de la retombée, ht : hauteur de la table, hr : hauteur de la
  // retombée), Iy vaut :\end{verbatim}\begin{displaymath}
  // I_y = \frac{l_t \cdot h_t^3}{12}+\frac{l_r \cdot h_r^3}{12}+
  //       l_t \cdot h_t \cdot \left(\frac{h_t}{2}-cdg_h \right)^2+
  //       l_r \cdot h_r \cdot \left(\frac{h_r}{2}-cdg_b \right)^2
  //       \texttt{, }\end{displaymath}\begin{displaymath}
  // cdg_h = \frac{\frac{l_t \cdot h_t^2}{2}+l_r \cdot h_r \cdot
  //         \left(h_t+\frac{h_r}{2} \right)}{S}  \texttt{, }
  // cdg_b = h_t+h_r-cdg_h \texttt{ et }
  // S = l_t \cdot h_t+l_r \cdot h_r \end{displaymath}\begin{verbatim}
      
      double    S = lt * ht + lr * hr;
      double    cdgh = (lt * ht * ht / 2. + lr * hr * (ht + hr / 2.)) / S;
      double    cdgb = (ht + hr) - cdgh;
      
      return m_f (lt * ht * ht * ht / 12. + lr * hr * hr * hr / 12. +
                    lt * ht * (ht / 2. - cdgh) * (ht / 2. - cdgh) +
                    lr * hr * (hr / 2. - cdgb) * (hr / 2. - cdgb),
                  FLOTTANT_ORDINATEUR);
    }
    case SECTION_CIRCULAIRE :
    {
      Section_Circulaire *section = sect->data;
      double              diametre = m_g (section->diametre);
      
      return m_f (M_PI * diametre * diametre * diametre * diametre / 64.,
                  FLOTTANT_ORDINATEUR);
      
  // Pour une section circulaire de section constante, Iy vaut :
  // \end{verbatim}\begin{displaymath}
  // I_y = \frac{\pi \cdot \phi^4}{64} \end{displaymath}\begin{verbatim}
    }
    case SECTION_PERSONNALISEE :
    {
      Section_Personnalisee *section = sect->data;
      
      return section->iy;
    }
    default :
    {
      FAILCRIT (m_f (NAN, FLOTTANT_ORDINATEUR),
                (gettext ("Type de section %d inconnu.\n"), sect->type); )
      break;
    }
  }
}


/**
 * \brief Renvoie l'inertie I selon l'axe z lorsque la section est constante.
 * \param sect : section à étudier.
 * \return
 *   Succès : Résultat.\n
 *   Échec : NAN :
 *     - section == NULL,
 *     - type de section inconnu.
 */
Flottant
EF_sections_iz (Section *sect)
{
  BUGPARAM (sect, "%p", sect, m_f (NAN, FLOTTANT_ORDINATEUR))
  
  switch (sect->type)
  {
    case SECTION_RECTANGULAIRE :
    case SECTION_T :
    case SECTION_CARREE :
    {
      Section_T *section = sect->data;
      double     lt = m_g (section->largeur_table);
      double     lr = m_g (section->largeur_retombee);
      double     ht = m_g (section->hauteur_table);
      double     hr = m_g (section->hauteur_retombee);
      
  // Pour une section en T de section constante (lt : largeur de la table,
  // lr : largeur de la retombée, ht : hauteur de la table, hr : hauteur de la
  // retombée), I vaut :\end{verbatim}\begin{displaymath}
  // I = \frac{h_t \cdot l_t^3}{12}+\frac{h_r \cdot l_r^3}{12}
  // \end{displaymath}\begin{verbatim}
      
      return m_f (ht * lt * lt * lt / 12. + hr * lr * lr * lr / 12.,
                  FLOTTANT_ORDINATEUR);
    }
    case SECTION_CIRCULAIRE :
    {
      Section_Circulaire *section = sect->data;
      double              diametre = m_g (section->diametre);
      return m_f (M_PI * diametre * diametre * diametre * diametre / 64.,
                  FLOTTANT_ORDINATEUR);
      
  // Pour une section circulaire de section constante, I vaut :
  // \end{verbatim}\begin{displaymath}
  // I = \frac{\pi \cdot \phi^4}{64} \end{displaymath}\begin{verbatim}
    }
    case SECTION_PERSONNALISEE :
    {
      Section_Personnalisee *section = sect->data;
      
      return section->iz;
    }
    default :
    {
      FAILCRIT (m_f (NAN, FLOTTANT_ORDINATEUR),
                (gettext ("Type de section %d inconnu.\n"), sect->type); )
      break;
    }
  }
}


/**
 * \brief Renvoie la distance entre le centre de gravité et la partie la plus à
 *        droite de la section.
 * \param sect : section à étudier.
 * \return
 *   Succès : Résultat.\n
 *   Échec : NAN :
 *     - section == NULL,
 *     - type de section inconnu.
 */
Flottant
EF_sections_vy (Section *sect)
{
  BUGPARAM (sect, "%p", sect, m_f (NAN, FLOTTANT_ORDINATEUR))
  
  switch (sect->type)
  {
    case SECTION_RECTANGULAIRE :
    case SECTION_T :
    case SECTION_CARREE :
    {
      Section_T *section = sect->data;
      
      return m_f (MAX (m_g (section->largeur_table),
                       m_g (section->largeur_retombee)) / 2.,
                  FLOTTANT_ORDINATEUR);
    }
    case SECTION_CIRCULAIRE :
    {
      Section_Circulaire *section = sect->data;
      
      return m_f (m_g (section->diametre) / 2., FLOTTANT_ORDINATEUR);
    }
    case SECTION_PERSONNALISEE :
    {
      Section_Personnalisee *section = sect->data;
      
      return section->vy;
    }
    default :
    {
      FAILCRIT (m_f (NAN, FLOTTANT_ORDINATEUR),
                (gettext ("Type de section %d inconnu.\n"), sect->type); )
      break;
    }
  }
}


/**
 * brief Renvoie la distance entre le centre de gravité et la partie la plus à
 *      gauche de la section.
 * \param sect : section à étudier.
 * \return
 *   Succès : Résultat.\n
 *   Échec : NAN :
 *     - section == NULL,
 *     - type de section inconnu.
 */
Flottant
EF_sections_vyp (Section *sect)
{
  BUGPARAM (sect, "%p", sect, m_f (NAN, FLOTTANT_ORDINATEUR))
  
  switch (sect->type)
  {
    case SECTION_RECTANGULAIRE :
    case SECTION_T :
    case SECTION_CARREE :
    {
      Section_T *section = sect->data;
      
      return m_f (MAX (m_g (section->largeur_table),
                       m_g (section->largeur_retombee)) / 2.,
                  FLOTTANT_ORDINATEUR);
    }
    case SECTION_CIRCULAIRE :
    {
      Section_Circulaire *section = sect->data;
      
      return m_f (m_g (section->diametre) / 2., FLOTTANT_ORDINATEUR);
    }
    case SECTION_PERSONNALISEE :
    {
      Section_Personnalisee *section = sect->data;
      
      return section->vyp;
    }
    default :
    {
      FAILCRIT (m_f (NAN, FLOTTANT_ORDINATEUR),
                (gettext ("Type de section %d inconnu.\n"), sect->type); )
      break;
    }
  }
}


/**
 * \brief Renvoie la distance entre le centre de gravité et la partie la plus
 *        haute de la section.
 * \param sect : section à étudier.
 * \return
 *   Succès : Résultat.\n
 *   Échec : NAN :
 *     - section == NULL,
 *     - type de section inconnu.
 */
Flottant
EF_sections_vz (Section *sect)
{
  BUGPARAM (sect, "%p", sect, m_f (NAN, FLOTTANT_ORDINATEUR))
  
  switch (sect->type)
  {
    case SECTION_RECTANGULAIRE :
    case SECTION_T :
    case SECTION_CARREE :
    {
      Section_T *section = sect->data;
      
      return m_f ( (m_g (section->largeur_table) *
                    m_g (section->hauteur_table) *
                    m_g (section->hauteur_table) / 2. +
                    m_g (section->largeur_retombee) *
                    m_g (section->hauteur_retombee) *
                    (m_g (section->hauteur_retombee) / 2. +
                     m_g (section->hauteur_table))) /
                    m_g (EF_sections_s (sect)),
                  FLOTTANT_ORDINATEUR);
    }
    case SECTION_CIRCULAIRE :
    {
      Section_Circulaire *section = sect->data;
      
      return m_f (m_g (section->diametre) / 2., FLOTTANT_ORDINATEUR);
    }
    case SECTION_PERSONNALISEE :
    {
      Section_Personnalisee *section = sect->data;
      
      return section->vz;
    }
    default :
    {
      FAILCRIT (m_f (NAN, FLOTTANT_ORDINATEUR),
                (gettext ("Type de section %d inconnu.\n"), sect->type); )
      break;
    }
  }
}


/**
 * \brief Renvoie la distance entre le centre de gravité et la partie la plus
 *        basse de la section.
 * \param sect : section à étudier.
 * \return
 *   Succès : Résultat.\n
 *   Échec : NAN :
 *     - section == NULL,
 *     - type de section inconnu.
 */
Flottant
EF_sections_vzp (Section *sect)
{
  BUGPARAM (sect, "%p", sect, m_f (NAN, FLOTTANT_ORDINATEUR))
  
  switch (sect->type)
  {
    case SECTION_RECTANGULAIRE :
    case SECTION_T :
    case SECTION_CARREE :
    {
      Section_T *section = sect->data;
      
      return m_f ((m_g (section->largeur_table) *
                   m_g (section->hauteur_table) *
                   (m_g (section->hauteur_retombee) +
                    m_g (section->hauteur_table) / 2.) +
                   m_g (section->largeur_retombee) *
                   m_g (section->hauteur_retombee) *
                   (m_g (section->hauteur_retombee) / 2.))
                   / m_g (EF_sections_s (sect)),
                  FLOTTANT_ORDINATEUR);
    }
    case SECTION_CIRCULAIRE :
    {
      Section_Circulaire *section = sect->data;
      
      return m_f (m_g (section->diametre) / 2., FLOTTANT_ORDINATEUR);
    }
    case SECTION_PERSONNALISEE :
    {
      Section_Personnalisee *section = sect->data;
      
      return section->vzp;
    }
    default :
    {
      FAILCRIT (m_f (NAN, FLOTTANT_ORDINATEUR),
                (gettext ("Type de section %d inconnu.\n"), sect->type); )
      break;
    }
  }
}


/**
 * \brief Renvoie le paramètre de souplesse a de la poutre selon l'axe y.
 * \param barre : la barre à étudier,
 * \param discretisation : partie de la barre à étudier.
 * \return
 *   Succès : Résultat.\n
 *   Échec : NAN :
 *     - barre == NULL,
 *     - discretisation <= barre->discretisation_element,
 *     - type de section inconnu.
 */
double
EF_sections_ay (EF_Barre *barre,
                uint16_t  discretisation)
{
  EF_Noeud *debut, *fin;
  double    ll;
  double    E;
  
  BUGPARAM (barre, "%p", barre, NAN)
  INFO (discretisation <= barre->discretisation_element,
        NAN,
        (gettext ("La discrétisation %d souhaitée est hors domaine %d.\n"),
                  discretisation,
                  barre->discretisation_element); )
  
  // Le coefficient a est défini par la formule :
  // \end{verbatim}\begin{displaymath}
  // a_y = \frac{1}{l^2}\int_0^l \frac{(l-x)^2}{E \cdot I_y(x)} dx
  // \end{displaymath}\begin{verbatim}
  
  if (discretisation == 0)
  {
    debut = barre->noeud_debut;
  }
  else
  {
    debut = g_list_nth_data (barre->nds_inter, discretisation - 1U);
  }
  if (discretisation == barre->discretisation_element)
  {
    fin = barre->noeud_fin;
  }
  else
  {
    fin = g_list_nth_data (barre->nds_inter, discretisation);
  }
  
  ll = EF_noeuds_distance (fin, debut);
  BUG (!isnan (ll), NAN)
  E = m_g (EF_materiaux_E (barre->materiau));
  BUG (!isnan (E), NAN)
  
  return ll / (3. * E * m_g (EF_sections_iy (barre->section)));
}


/**
 * \brief Renvoie le paramètre de souplesse b de la poutre selon l'axe y.
 * \param barre : la barre à étudier,
 * \param discretisation : partie de la barre à étudier.
 * \return
 *   Succès : Résultat.\n
 *   Échec : NAN :
 *     - barre == NULL,
 *     - discretisation>barre->discretisation_element,
 *     - type de section inconnu.
 */
double
EF_sections_by (EF_Barre *barre,
                uint16_t  discretisation)
{
  EF_Noeud *debut, *fin;
  double    ll;
  double    E;
  
  BUGPARAM (barre, "%p", barre, NAN)
  INFO (discretisation <= barre->discretisation_element,
        NAN,
        (gettext ("La discrétisation %d souhaitée est hors domaine %d.\n"),
                  discretisation,
                  barre->discretisation_element); )
  
  // Le coefficient b est défini par la formule :
  // \end{verbatim}\begin{displaymath}
  // b_y = \frac{1}{l^2}\int_0^l \frac{x \cdot (l-x)^2}{E \cdot I_y(x)} dx
  // \end{displaymath}\begin{verbatim}
  
  if (discretisation == 0)
  {
    debut = barre->noeud_debut;
  }
  else
  {
    debut = g_list_nth_data (barre->nds_inter, discretisation - 1U);
  }
  if (discretisation == barre->discretisation_element)
  {
    fin = barre->noeud_fin;
  }
  else
  {
    fin = g_list_nth_data(barre->nds_inter, discretisation);
  }
  
  ll = EF_noeuds_distance (fin, debut);
  BUG (!isnan (ll), NAN)
  E = m_g (EF_materiaux_E (barre->materiau));
  BUG (!isnan (E), NAN)
  
  return ll / (6. * E * m_g (EF_sections_iy (barre->section)));
}


/**
 * \brief Renvoie le paramètre de souplesse c de la poutre selon l'axe y.
 * \param barre : la barre à étudier,
 * \param discretisation : partie de la barre à étudier.
 * \return
 *   Succès : Résultat.\n
 *   Échec : NAN :
 *     - barre == NULL,
 *     - discretisation>barre->discretisation_element,
 *     - type de section inconnu.
 */
double
EF_sections_cy (EF_Barre *barre,
                uint16_t  discretisation)
{
  EF_Noeud *debut, *fin;
  double    ll;
  double    E;
  
  BUGPARAM (barre, "%p", barre, NAN)
  INFO (discretisation <= barre->discretisation_element,
        NAN,
        (gettext ("La discrétisation %d souhaitée est hors domaine %d.\n"),
                  discretisation,
                  barre->discretisation_element); )
  
  // Le coefficient c est défini par la formule :
  // \end{verbatim}\begin{displaymath}
  // c_y = \frac{1}{l^2}\int_0^l \frac{x^2}{E \cdot I_y(x)} dx
  // \end{displaymath}\begin{verbatim}
  
  if (discretisation == 0)
  {
    debut = barre->noeud_debut;
  }
  else
  {
    debut = g_list_nth_data (barre->nds_inter, discretisation - 1U);
  }
  if (discretisation == barre->discretisation_element)
  {
    fin = barre->noeud_fin;
  }
  else
  {
    fin = g_list_nth_data (barre->nds_inter, discretisation);
  }
  
  ll = EF_noeuds_distance (fin, debut);
  BUG (!isnan (ll), NAN)
  E = m_g (EF_materiaux_E (barre->materiau));
  BUG (!isnan (E), NAN)
  
  return ll / (3. * E * m_g (EF_sections_iy (barre->section)));
}


/**
 * \brief Renvoie le paramètre de souplesse a de la poutre selon l'axe z.
 * \param barre : la barre à étudier,
 * \param discretisation : partie de la barre à étudier.
 * \return
 *   Succès : Résultat.\n
 *   Échec : NAN :
 *     - barre == NULL,
 *     - discretisation>barre->discretisation_element,
 *     - type de section inconnu.
 */
double
EF_sections_az (EF_Barre *barre,
                uint16_t  discretisation)
{
  EF_Noeud *debut, *fin;
  double    ll;
  double    E;
  
  BUGPARAM (barre, "%p", barre, NAN)
  INFO (discretisation <= barre->discretisation_element,
        NAN,
        (gettext ("La discrétisation %d souhaitée est hors domaine %d.\n"),
                  discretisation,
                  barre->discretisation_element); )
  
  // Le coefficient a est défini par la formule :
  // \end{verbatim}\begin{displaymath}
  // a_z = \frac{1}{l^2}\int_0^l \frac{(l-x)^2}{E \cdot I_z(x)} dx
  // \end{displaymath}\begin{verbatim}
  
  if (discretisation == 0)
  {
    debut = barre->noeud_debut;
  }
  else
  {
    debut = g_list_nth_data (barre->nds_inter, discretisation - 1U);
  }
  if (discretisation == barre->discretisation_element)
  {
    fin = barre->noeud_fin;
  }
  else
  {
    fin = g_list_nth_data (barre->nds_inter, discretisation);
  }
  
  ll = EF_noeuds_distance (fin, debut);
  BUG (!isnan (ll), NAN)
  E = m_g (EF_materiaux_E (barre->materiau));
  BUG (!isnan (E), NAN)
  
  return ll / (3. * E * m_g (EF_sections_iz (barre->section)));
}


/**
 * \brief Renvoie le paramètre de souplesse b de la poutre selon l'axe z.
 * \param barre : la barre à étudier,
 * \param discretisation : partie de la barre à étudier.
 * \return
 *   Succès : Résultat.\n
 *   Échec : NAN :
 *     - barre == NULL,
 *     - discretisation>barre->discretisation_element,
 *     - type de section inconnu.
 */
double
EF_sections_bz (EF_Barre *barre,
                uint16_t  discretisation)
{
  EF_Noeud *debut, *fin;
  double    ll;
  double    E;
  
  BUGPARAM (barre, "%p", barre, NAN)
  INFO (discretisation <= barre->discretisation_element,
        NAN,
        (gettext ("La discrétisation %d souhaitée est hors domaine %d.\n"),
                  discretisation,
                  barre->discretisation_element); )
  
  // Le coefficient b est défini par la formule :
  // \end{verbatim}\begin{displaymath}
  // b_z = \frac{1}{l^2}\int_0^l \frac{x \cdot (l-x)^2}{E \cdot I_z(x)} dx
  // \end{displaymath}\begin{verbatim}
  
  if (discretisation == 0)
  {
    debut = barre->noeud_debut;
  }
  else
  {
    debut = g_list_nth_data (barre->nds_inter, discretisation - 1U);
  }
  if (discretisation == barre->discretisation_element)
  {
    fin = barre->noeud_fin;
  }
  else
  {
    fin = g_list_nth_data (barre->nds_inter, discretisation);
  }
  
  ll = EF_noeuds_distance (fin, debut);
  BUG (!isnan (ll), NAN)
  E = m_g (EF_materiaux_E (barre->materiau));
  BUG (!isnan (E), NAN)
  
  return ll / (6. * E * m_g (EF_sections_iz (barre->section)));
}


/**
 * \brief Renvoie le paramètre de souplesse c de la poutre selon l'axe z.
 * \param barre : la barre à étudier,
 * \param discretisation : partie de la barre à étudier.
 * \return
 *   Succès : Résultat.\n
 *   Échec : NAN :
 *     - barre == NULL,
 *     - discretisation>barre->discretisation_element,
 *     - type de section inconnu.
 */
double
EF_sections_cz (EF_Barre *barre,
                uint16_t  discretisation)
{
  EF_Noeud *debut, *fin;
  double    ll;
  double    E;
  
  BUGPARAM (barre, "%p", barre, NAN)
  INFO (discretisation <= barre->discretisation_element,
        NAN,
        (gettext ("La discrétisation %d souhaitée est hors domaine %d.\n"),
                  discretisation,
                  barre->discretisation_element); )
  
  // Le coefficient c est défini par la formule :
  // \end{verbatim}\begin{displaymath}
  // c_z = \frac{1}{l^2}\int_0^l \frac{x^2}{E \cdot I_y(x)} dx
  // \end{displaymath}\begin{verbatim}
  
  if (discretisation == 0)
  {
    debut = barre->noeud_debut;
  }
  else
  {
    debut = g_list_nth_data (barre->nds_inter, discretisation - 1U);
  }
  if (discretisation == barre->discretisation_element)
  {
    fin = barre->noeud_fin;
  }
  else
  {
    fin = g_list_nth_data (barre->nds_inter, discretisation);
  }
  
  ll = EF_noeuds_distance (fin, debut);
  BUG (!isnan (ll), NAN)
  E = m_g (EF_materiaux_E (barre->materiau));
  BUG (!isnan (E), NAN)
  
  return ll / (3. * E * m_g (EF_sections_iz (barre->section)));
}


/**
 * \brief Renvoie la surface de la section étudiée.
 * \param sect : section à étudier.
 * \return
 *   Succès : Résultat.\n
 *   Échec : NAN en cas de paramètres invalides :
 *     - sect == NULL,
 *     - type de section inconnue.
 */
Flottant
EF_sections_s (Section *sect)
{
  BUGPARAM (sect, "%p", sect, m_f (NAN, FLOTTANT_ORDINATEUR))
  
  switch (sect->type)
  {
    case SECTION_RECTANGULAIRE :
    case SECTION_T :
    case SECTION_CARREE :
    {
      Section_T *section = sect->data;
      
      return m_f (m_g (section->hauteur_table) * m_g (section->largeur_table) +
                    m_g (section->hauteur_retombee) *
                    m_g (section->largeur_retombee),
                  FLOTTANT_ORDINATEUR);
      
  // Pour une section en T de section constante (lt : largeur de la table,
  // lr : largeur de la retombée, ht : hauteur de la table, hr : hauteur de la
  // retombée), S vaut :\end{verbatim}\begin{displaymath}
  // S = h_t \cdot l_t+h_r \cdot l_r\end{displaymath}\begin{verbatim}
    }
    case SECTION_CIRCULAIRE :
    {
      Section_Circulaire *section = sect->data;
      
      return m_f (M_PI * m_g (section->diametre) *
                    m_g (section->diametre) / 4.,
                  FLOTTANT_ORDINATEUR);
      
  // Pour une section circulaire de section constante, S vaut :
  // \end{verbatim}\begin{displaymath}
  // S = \frac{\pi \cdot \phi^2}{4} \end{displaymath}\begin{verbatim}
    }
    case SECTION_PERSONNALISEE :
    {
      Section_Personnalisee *section = sect->data;
      
      return section->s;
    }
    default :
    {
      FAILCRIT (m_f (NAN, FLOTTANT_ORDINATEUR),
                (gettext ("Type de section %d inconnu.\n"), sect->type); )
      break;
    }
  }
}


/**
 * \brief Renvoie l'équivalent du rapport ES/L pour la barre étudiée.
 * \param barre : la barre à étudier,
 * \param discretisation : partie de la barre à étudier,
 * \param d : début de la partie à prendre en compte,
 * \param f : fin de la partie à prendre en compte.
 * \return
 *   Succès : Résultat.\n
 *   Échec : NAN :
 *     - barre == NULL,
 *     - discretisation>barre->discretisation_element,
 *     - debut>fin,
 *     - type de section inconnue.
 */
double
EF_sections_es_l (EF_Barre *barre,
                  uint16_t  discretisation,
                  double    d,
                  double    f)
{
  BUGPARAM (barre, "%p", barre, NAN)
  INFO (discretisation <= barre->discretisation_element,
        NAN,
        (gettext ("La discrétisation %d souhaitée est hors domaine %d.\n"),
                  discretisation,
                  barre->discretisation_element); )
  INFO (d <= f,
        NAN,
        (gettext ("La fin doit être après le début.\n")); )
  
  // Le facteur ES/L est défini par la formule :
  // \end{verbatim}\begin{displaymath}
  // \frac{E \cdot S}{L} = \frac{E}{\int_d^f \frac{1}{S(x)} dx}
  // \end{displaymath}\begin{verbatim}
  
  return m_g (EF_materiaux_E (barre->materiau)) *
         m_g (EF_sections_s (barre->section)) / (f - d);
}


/**
 * \brief Renvoie l'équivalent du rapport GJ/L pour la barre étudiée.
 * \param barre : la barre à étudier,
 * \param discretisation : partie de la barre à étudier.
 * \return
 *   Succès : Résultat.\n
 *   Échec : NAN :
 *     - barre == NULL,
 *     - discretisation>barre->discretisation_element,
 *     - type de section inconnue.
 */
double
EF_sections_gj_l (EF_Barre *barre,
                  uint16_t  discretisation)
{
  EF_Noeud *debut, *fin;
  double    ll;
  
  BUGPARAM (barre, "%p", barre, NAN)
  INFO (discretisation <= barre->discretisation_element,
        NAN,
        (gettext ("La discrétisation %d souhaitée est hors domaine %d.\n"),
                  discretisation,
                  barre->discretisation_element); )
  
  // Le facteur GJ/L est défini par la formule :
  // \end{verbatim}\begin{displaymath}
  // \frac{G \cdot J}{L} = \frac{G}{\int_0^l \frac{1}{J(x)} dx}
  // \end{displaymath}\begin{verbatim}
  
  if (discretisation == 0)
  {
    debut = barre->noeud_debut;
  }
  else
  {
    debut = g_list_nth_data (barre->nds_inter, discretisation - 1U);
  }
  if (discretisation == barre->discretisation_element)
  {
    fin = barre->noeud_fin;
  }
  else
  {
    fin = g_list_nth_data (barre->nds_inter, discretisation);
  }
  
  ll = EF_noeuds_distance (fin, debut);
  BUG (!isnan (ll), NAN)
  
  return m_g (EF_materiaux_G (barre->materiau, FALSE)) *
         m_g (EF_sections_j (barre->section)) / ll;
}


/**
 * \brief Libère l'ensemble des sections.
 * \param p : la variable projet.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL.
 */
gboolean
EF_sections_free (Projet *p)
{
  BUGPARAM (p, "%p", p, FALSE)
  
  if (p->modele.sections != NULL)
  {
    g_list_free_full (p->modele.sections,
                      (GDestroyNotify) &EF_sections_free_un);
    p->modele.sections = NULL;
  }
  
  BUG (EF_calculs_free (p), FALSE)
  
#ifdef ENABLE_GTK
  g_object_unref (UI_SEC.liste_sections);
#endif
  
  return TRUE;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
