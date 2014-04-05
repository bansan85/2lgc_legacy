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
#include <stdlib.h>
#include <libintl.h>
#include <cholmod.h>
#include <string.h>
#include <math.h>
#include <gmodule.h>

#include "1990_action.h"
#include "common_projet.h"
#include "common_erreurs.h"
#include "common_math.h"
#ifdef ENABLE_GTK
#include "common_m3d.hpp"
#include "common_gtk.h"
#include "EF_gtk_appuis.h"
#include "EF_gtk_noeud.h"
#include "EF_gtk_sections.h"
#include "EF_gtk_materiaux.h"
#include "EF_gtk_relachement.h"
#endif
#include "common_selection.h"
#include "EF_charge_barre_ponctuelle.h"
#include "EF_calculs.h"
#include "EF_noeuds.h"
#include "EF_sections.h"
#include "EF_charge_barre_ponctuelle.h"
#include "EF_charge_barre_repartie_uniforme.h"
#include "EF_charge_noeud.h"

gboolean
_1992_1_1_barres_init (Projet *p)
/**
 * \brief Initialise la liste des éléments en béton.
 * \param p : la variable projet.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL.
 */
{
#ifdef ENABLE_GTK
  GtkTreeIter iter;
#endif
  
  BUGPARAM (p, "%p", p, FALSE)
  
  // Trivial
  p->modele.barres = NULL;
#ifdef ENABLE_GTK
  UI_BAR.liste_types = gtk_list_store_new (1, G_TYPE_STRING);
  gtk_list_store_append (UI_BAR.liste_types, &iter);
  gtk_list_store_set (UI_BAR.liste_types,
                      &iter,
                      0, gettext ("Poteau en béton"),
                      -1);
  gtk_list_store_append (UI_BAR.liste_types, &iter);
  gtk_list_store_set (UI_BAR.liste_types,
                      &iter,
                      0, gettext ("Poutre en béton"),
                      -1);
#endif
  
  return TRUE;
}


void
_1992_1_1_barres_free_foreach (EF_Barre *barre,
                               Projet   *p)
/**
 * \brief Fonction permettant de libérer une barre contenue dans une liste.
 * \param barre : la barre à libérer,
 * \param p : la variable projet.
 * \return Rien.
 *   Echec :\n
 *     - barre == NULL,
 *     - p == NULL.
 */
{
  BUGPARAM (p, "%p", p, )
  BUGPARAM (barre, "%p", barre, )
  
  while (barre->nds_inter != NULL)
  {
    void *tmp = barre->nds_inter->data;
     
    EF_noeuds_free_foreach ((EF_Noeud *) barre->nds_inter->data, p);
    p->modele.noeuds = g_list_remove (p->modele.noeuds, tmp);
  }
  if (barre->m_rot != NULL)
    cholmod_free_sparse (&barre->m_rot, p->calculs.c);
  if (barre->m_rot_t != NULL)
    cholmod_free_sparse (&barre->m_rot_t, p->calculs.c);
  free (barre->info_EF);
  
#ifdef ENABLE_GTK
  if (UI_BAR.builder != NULL)
  {
    GtkTreeModel *model = GTK_TREE_MODEL (gtk_builder_get_object (
                                       UI_BAR.builder, "EF_barres_treestore"));
    
    gtk_tree_store_remove (GTK_TREE_STORE (model), &barre->Iter);
  }
  m3d_barre_free (&UI_M3D, barre);
#endif
  free (barre);
  
  return;
}


gboolean
_1992_1_1_barres_ajout (Projet         *p,
                        Type_Element    type,
                        Section        *section,
                        EF_Materiau    *materiau,
                        EF_Noeud       *noeud_debut,
                        EF_Noeud       *noeud_fin,
                        Flottant        angle,
                        EF_Relachement *relachement,
                        unsigned int    discretisation_element)
/**
 * \brief Ajoute une barre.
 * \param p : la variable projet,
 * \param type : type de l'élément en béton,
 * \param section : section correspondant à l'élément,
 * \param materiau : matériau correspondant à l'élément,
 * \param noeud_debut : numéro de départ de l'élément,
 * \param noeud_fin : numéro de fin de l'élément,
 * \param angle : angle de la barre en degré,
 * \param relachement : relachement de la barre (NULL si aucun),
 * \param discretisation_element : nombre d'élément une fois discrétisé.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL,
 *     - noeud_debut == NULL,
 *     - noeud_fin == NULL,
 *     - noeud_debut == noeud_fin,
 *     - section == NULL,
 *     - materiau == NULL,
 *     - en cas d'erreur d'allocation mémoire.
 */
{
  EF_Barre *element_nouveau;
  
  // Trivial
  BUGPARAM (p, "%p", p, FALSE)
  BUGPARAM (noeud_debut, "%p", noeud_debut, FALSE)
  BUGPARAM (noeud_fin, "%p", noeud_fin, FALSE)
  INFO (noeud_debut != noeud_fin,
        FALSE,
        (gettext ("La création d'une barre nécessite l'utilisation de des noeuds différents.\n"));)
  BUGPARAM (materiau, "%p", materiau, FALSE)
  BUGPARAM (section, "%p", section, FALSE)
  INFO (!errmoy (EF_noeuds_distance (noeud_debut, noeud_fin),
                 ERRMOY_DIST),
        FALSE,
        (gettext ("Impossible de créer la barre, la distance entre les deux noeuds %d et %d est nulle.\n"),
                  noeud_debut->numero,
                  noeud_fin->numero);)
  BUGCRIT (element_nouveau = malloc (sizeof (EF_Barre)),
           FALSE,
           (gettext ("Erreur d'allocation mémoire.\n"));)
  
  element_nouveau->type = type;
  
  element_nouveau->section = section;
  element_nouveau->materiau = materiau;
  element_nouveau->angle = angle;
  element_nouveau->noeud_debut = noeud_debut;
  element_nouveau->noeud_fin = noeud_fin;
  element_nouveau->relachement = relachement;
  element_nouveau->discretisation_element = 0;
  
  BUGCRIT (element_nouveau->info_EF = (Barre_Info_EF *) malloc (sizeof (
                                                               Barre_Info_EF)),
           FALSE,
           (gettext ("Erreur d'allocation mémoire.\n"));
             free (element_nouveau);)
  memset (element_nouveau->info_EF, 0, sizeof (Barre_Info_EF));
  
  element_nouveau->m_rot = NULL;
  element_nouveau->m_rot_t = NULL;
  
  if (p->modele.barres == NULL)
    element_nouveau->numero = 0;
  else
    element_nouveau->numero = ((EF_Barre *) g_list_last (p->modele.barres)
                                                           ->data)->numero + 1;
  
  BUG (EF_calculs_free (p),
       FALSE,
       free (element_nouveau->info_EF);
         free (element_nouveau);)
  
  element_nouveau->nds_inter = NULL;
  
#ifdef ENABLE_GTK
  // On incrémente le numéro de la future barre
  if (UI_BARADD.builder != NULL)
  {
    char *nb_barres;
    
    BUGCRIT (nb_barres = g_strdup_printf ("%d", element_nouveau->numero + 1),
             FALSE,
             (gettext ("Erreur d'allocation mémoire.\n"));
               free (element_nouveau->info_EF);
               free (element_nouveau);)
    gtk_label_set_text (GTK_LABEL (gtk_builder_get_object (
                        UI_BARADD.builder, "EF_gtk_barres_add_numero_label2")),
                        nb_barres);
    free (nb_barres);
  }
  
  // On ajoute la ligne dans la liste des barres
  if (UI_BAR.builder != NULL)
  {
    char       *tmp;
    GtkTreeIter iter;
    
    BUGCRIT (tmp = g_strdup_printf ("%d", (int) type),
             FALSE,
             (gettext ("Erreur d'allocation mémoire.\n"));
               free (element_nouveau->info_EF);
               free (element_nouveau);)
    gtk_tree_model_get_iter_from_string (GTK_TREE_MODEL ( UI_BAR.liste_types),
                                         &iter,
                                         tmp);
    free (tmp);
    gtk_tree_model_get (GTK_TREE_MODEL (UI_BAR.liste_types),
                        &iter,
                        0, &tmp,
                        -1);
    
    gtk_tree_store_append (GTK_TREE_STORE (gtk_builder_get_object (
                                       UI_BAR.builder, "EF_barres_treestore")),
                           &element_nouveau->Iter,
                           NULL);
    gtk_tree_store_set (GTK_TREE_STORE (gtk_builder_get_object (
                                       UI_BAR.builder, "EF_barres_treestore")),
                        &element_nouveau->Iter,
                        0, element_nouveau,
                        -1);
    free (tmp);
  }
  
  BUG (m3d_barre (&UI_M3D, element_nouveau), FALSE)
#endif
  
  if (discretisation_element != 0)
  {
    unsigned int i;
    
    /* Création des noeuds intermédiaires */
    for (i = 0; i < discretisation_element; i++)
      BUG (EF_noeuds_ajout_noeud_barre (
             p,
             element_nouveau,
             m_f ((i + 1.) / (discretisation_element + 1.),
                  FLOTTANT_ORDINATEUR),
             NULL),
           FALSE,
           _1992_1_1_barres_free_foreach (element_nouveau, p);)
  }
  
  p->modele.barres = g_list_append (p->modele.barres, element_nouveau);
  
  return TRUE;
}


EF_Barre *
_1992_1_1_barres_cherche_numero (Projet      *p,
                                 unsigned int numero,
                                 gboolean     critique)
/**
 * \brief Renvoie la barre en fonction du numéro.
 * \param p : la variable projet,
 * \param numero : le numéro de la barre.
 * \param critique : si TRUE alors #FAILINFO, si FALSE alors return NULL.
 * \return
 *   Succès : Pointeur vers la barre.\n
 *   Échec : NULL :
 *     - p == NULL,
 *     - Barre en béton introuvable.
 */
{
  GList *list_parcours;
  
  BUGPARAM (p, "%p", p, NULL)
  
  list_parcours = p->modele.barres;
  
  while (list_parcours != NULL)
  {
    EF_Barre *element = list_parcours->data;
    
    if (element->numero == numero)
      return element;
    
    list_parcours = g_list_next (list_parcours);
  }
  
  if (critique)
    FAILINFO (NULL, (gettext ("Barre en béton %u introuvable.\n"), numero);)
  else
    return NULL;
}


gboolean
_1992_1_1_barres_cherche_dependances (Projet  *p,
                                      GList   *appuis,
                                      GList   *noeuds,
                                      GList   *sections,
                                      GList   *materiaux,
                                      GList   *relachements,
                                      GList   *barres,
                                      GList  **noeuds_dep,
                                      GList  **barres_dep,
                                      GList  **charges_dep,
                                      gboolean numero,
                                      gboolean origine)
/**
 * \brief Renvoie, sous forme d'une liste de noeuds, d'une liste de barres et
 *        d'une liste de charges l'ensemble des éléments dépendants des appuis,
 *        noeuds, sections, materiaux, relachements et barres passé en
 *        paramètres. Le retour contient également la liste d'origine. Si
 *        noeuds_dep, barres_dep ou charges_dep sont différents de NULL, la
 *        fonction renvoie TRUE si tout s'est bien passé et FALSE autrement.
 *        Si noeuds_dep, barres_dep ET charges_dep sont NULL alors la fonction
 *        renvoie TRUE si un élément dépendant a été trouvé (les noeuds
 *        intermédiaires sans dépendance ne sont pas considérés comme une
 *        dépendance pour cette fonction) et FALSE si aucun élément n'a été
 *        trouvé.
 * \param p : variable projet,
 * \param appuis : liste de pointeurs vers les appuis à analyser,
 * \param noeuds : liste de pointeurs vers les noeuds à analyser,
 * \param sections : liste de pointeurs vers les sections à analyser,
 * \param materiaux : liste de pointeurs vers les matériaux à analyser,
 * \param relachements : liste de pointeurs vers les relâchements à analyser,
 * \param barres : liste de pointeurs vers les barres à analyser,
 * \param noeuds_dep : liste de noeuds/numeros vers les noeuds dépendants,
 *                     peut être NULL,
 * \param barres_dep : liste de barres/numeros vers les barres dépendantes,
 *                     peut être NULL,
 * \param charges_dep : liste de charges (pointeur uniquement) dépendantes,
 *                   : peut être NULL.
 * \param numero : TRUE si les listes renvoient un numéro
 *               : FALSE si les listes renvoient un pointer vers les barres ou
 *                noeuds.
 * \param origine : TRUE si noeuds_dep et barres_dep doivent respectivement
 *                  inclure noeuds et barres.
 * \return Succès : cf. description.\n
 *    Échec : FALSE :
 *      - p == NULL,
 *      - #common_selection_ajout_nombre
 */
{
  GList   *list_parcours;
  GList   *noeuds_todo = NULL, *noeuds_done = NULL, *barres_todo = NULL;
  gboolean verif;
  
  BUGPARAM (p, "%p", p, FALSE)
  
  if ((noeuds_dep == NULL) && (barres_dep == NULL) && (charges_dep == NULL))
    verif = TRUE;
  else
    verif = FALSE;
  
  if (noeuds_dep != NULL)
    *noeuds_dep = NULL;
  if (barres_dep != NULL)
    *barres_dep = NULL;
  if (charges_dep != NULL)
    *charges_dep = NULL;
  
  // On ajoute les noeuds utilisant les appuis
  if (appuis != NULL)
  {
    list_parcours = p->modele.noeuds;
    
    while (list_parcours != NULL)
    {
      EF_Noeud *noeud = list_parcours->data;
      
      if ((noeud->appui != NULL) &&
          (g_list_find (appuis, noeud->appui) != NULL))
      {
        if (verif == FALSE)
          noeuds_todo = g_list_append (noeuds_todo, noeud);
        else
          return TRUE;
      }
      
      list_parcours = g_list_next (list_parcours);
    }
  }
  
  // On ajoute les noeuds dont la position est dépendante d'un des noeuds à
  // analyser
  list_parcours = p->modele.noeuds;
  while (list_parcours != NULL)
  {
    EF_Noeud *noeud = list_parcours->data;
    
    if (noeud->type == NOEUD_LIBRE)
    {
      EF_Noeud_Libre *data = noeud->data;
      
      if ((data->relatif != NULL) &&
          (g_list_find (noeuds, data->relatif) != NULL))
        noeuds_todo = g_list_append (noeuds_todo, noeud);
    }
    
    list_parcours = g_list_next (list_parcours);
  }
  
  // On ajoute les barres utilisant les sections, matériaux et relâchements.
  list_parcours = p->modele.barres;
  
  while (list_parcours != NULL)
  {
    EF_Barre *barre = list_parcours->data;
    
    if ((g_list_find (sections, barre->section) != NULL)
      || (g_list_find (materiaux, barre->materiau) != NULL)
      || ((barre->relachement != NULL) &&
          (g_list_find (relachements, barre->relachement) != NULL)))
    {
      if (verif == FALSE)
        barres_todo = g_list_append (barres_todo, barre);
      else
        return TRUE;
    }
    
    list_parcours = g_list_next (list_parcours);
  }
  
  // On initialise les barres à parcourir.
  list_parcours = barres;
  
  while (list_parcours != NULL)
  {
    EF_Barre *barre = list_parcours->data;
    
    if (g_list_find (barres_todo, barre) == NULL)
      barres_todo = g_list_append (barres_todo, barre);
    
    list_parcours = g_list_next (list_parcours);
  }
  
  // On commence par s'occuper des barres.
  list_parcours = barres_todo;
  
  while (list_parcours != NULL)
  {
    GList    *list_parcours2;
    EF_Barre *barre;
    
    // Toutes les barres sélectionnées sont forcément des barres dépendantes.
    barre = list_parcours->data;
    if ((barres_dep != NULL) &&
        ((origine) || (g_list_find (barres, barre) == NULL)))
    {
      if (numero)
        BUG (common_selection_ajout_nombre (GUINT_TO_POINTER (barre->numero),
                                            barres_dep,
                                            LISTE_UINT,
                                            NULL),
             FALSE,
             g_list_free (barres_todo);
               g_list_free (noeuds_todo);
               g_list_free (*barres_dep);)
      else
        BUG (common_selection_ajout_nombre (barre,
                                            barres_dep,
                                            LISTE_BARRES,
                                            NULL),
             FALSE,
             g_list_free (barres_todo);
               g_list_free (noeuds_todo);
               g_list_free (*barres_dep);)
    }
    // Ici, pas besoin de vérifier la variable verif. En effet à ce stade, tout
    // ce qui est dans barres_todo ne sont que les barres à analyser.
    // barres_todo ne peut contenir une dépendance vis-à-vis des sections /
    // matériaux / relâchements sinon, la fonction aurait déjà renvoyée TRUE.
    
    // Puis, tous les noeuds intermédiaires sont ajoutés à la liste des noeuds
    // à étudier.
    list_parcours2 = barre->nds_inter;
    
    while (list_parcours2 != NULL)
    {
      EF_Noeud *noeud;
      
      noeud = list_parcours2->data;
      noeuds_todo = g_list_append (noeuds_todo, noeud);
      
      list_parcours2 = g_list_next (list_parcours2);
    }
    
    list_parcours = g_list_next (list_parcours);
  }
  
  // On ajoute tous les noeuds à la liste des noeuds à étudier.
  list_parcours = noeuds;
  
  while (list_parcours != NULL)
  {
    EF_Noeud *noeud;
    
    noeud = list_parcours->data;
    noeuds_todo = g_list_append (noeuds_todo, noeud);
    
    list_parcours = g_list_next (list_parcours);
  }
  
  // On étudie enfin tous les noeuds.
  while (noeuds_todo != NULL)
  {
    EF_Noeud *dataa = noeuds_todo->data;
    
    noeuds_todo = g_list_delete_link (noeuds_todo, noeuds_todo);
    
    // On commence par ajouter le noeud en cours d'étude dans la liste des
    // noeuds traités.
    BUG (common_selection_ajout_nombre (dataa,
                                        &noeuds_done,
                                        LISTE_NOEUDS,
                                        NULL),
         FALSE,
         g_list_free (barres_todo);
           g_list_free (noeuds_todo);
           g_list_free (noeuds_done);
           if (barres_dep != NULL)
             g_list_free (*barres_dep);
           if (noeuds_dep != NULL)
             g_list_free (*noeuds_dep);)
    
    // On ajoute le noeud à la liste des noeuds dépendants.
    if ((noeuds_dep != NULL) &&
        ((origine) || (g_list_find (noeuds, dataa) == NULL)))
    {
      if (numero)
        BUG (common_selection_ajout_nombre (GUINT_TO_POINTER (((EF_Noeud *) 
                                                               dataa)->numero),
                                            noeuds_dep,
                                            LISTE_UINT,
                                            NULL),
             FALSE,
             g_list_free (barres_todo);
               g_list_free (noeuds_todo);
               g_list_free (noeuds_done);
               if (barres_dep != NULL)
                 g_list_free (*barres_dep);
               g_list_free (*noeuds_dep);)
      else
        BUG (common_selection_ajout_nombre (dataa,
                                            noeuds_dep,
                                            LISTE_NOEUDS,
                                            NULL),
             FALSE,
             g_list_free (barres_todo);
               g_list_free (noeuds_todo);
               g_list_free (noeuds_done);
               if (barres_dep != NULL)
                 g_list_free (*barres_dep);
               g_list_free (*noeuds_dep);)
    }
    // Rappel : si un noeud est de type intermédiaire, on considère qu'il n'est
    // pas une dépendance suffisante pour justifier une impossibilité de
    // suppression d'un élément.
    else if ((verif == TRUE) &&
             (dataa->type == NOEUD_LIBRE) &&
             (g_list_find (noeuds, dataa) == NULL))
    {
      g_list_free (noeuds_todo);
      g_list_free (noeuds_done);
      g_list_free (barres_todo);
      return TRUE;
    }
    
    // On parcours la liste des barres pour trouver celles qui commencent ou
    // finissent par le noeud en cours d'étude.
    list_parcours = p->modele.barres;
    
    while (list_parcours != NULL)
    {
      EF_Barre *barre;
      
      barre = list_parcours->data;
      
      // Si une barre est dépendante du noeud en cours d'étude
      if ((barre->noeud_debut == dataa) || (barre->noeud_fin == dataa))
      {
        GList *list_parcours2;
        
        // On l'ajoute à la liste des barres dépendantes.
        if ((barres_dep != NULL) &&
            ((origine) || (g_list_find (barres, barre) == NULL)))
        {
          if (numero)
            BUG (common_selection_ajout_nombre (GUINT_TO_POINTER (
                                                                barre->numero),
                                                barres_dep,
                                                LISTE_UINT,
                                                NULL),
                 FALSE,
                 g_list_free (barres_todo);
                   g_list_free (noeuds_todo);
                   g_list_free (noeuds_done);
                   g_list_free (*barres_dep);
                   if (noeuds_dep != NULL)
                     g_list_free (*noeuds_dep);)
          else
            BUG (common_selection_ajout_nombre (barre,
                                                barres_dep,
                                                LISTE_BARRES,
                                                NULL),
                 FALSE,
                 g_list_free (barres_todo);
                   g_list_free (noeuds_todo);
                   g_list_free (noeuds_done);
                   g_list_free (*barres_dep);
                   if (noeuds_dep != NULL)
                     g_list_free (*noeuds_dep);)
        }
        else if (verif == TRUE)
        {
          g_list_free (noeuds_todo);
          g_list_free (noeuds_done);
          g_list_free (barres_todo);
          return TRUE;
        }
        if (g_list_find (barres_todo, barre) == NULL)
          barres_todo = g_list_append (barres_todo, barre);
        
        // Puis on ajoute l'ensemble des noeuds intermédiaires.
        list_parcours2 = barre->nds_inter;
        
        while (list_parcours2 != NULL)
        {
          EF_Noeud *noeud;
          
          noeud = list_parcours2->data;
          if (g_list_find (noeuds_done, noeud) == NULL)
            noeuds_todo = g_list_append (noeuds_todo, noeud);
          
          list_parcours2 = g_list_next (list_parcours2);
        }
      }
      
      
      // Une fois que tous les noeuds ont été traités, on parcourt tous les
      // noeuds du modèle afin de trouver lesquels ont une position relative
      // ayant une dépendance avec les noeuds dépendants.
      if (noeuds_todo == NULL)
      {
        GList *list_parcours2 = p->modele.noeuds;
        
        while (list_parcours2 != NULL)
        {
          EF_Noeud *noeud = list_parcours2->data;
          if ((noeud->type == NOEUD_LIBRE) &&
              (g_list_find (noeuds_done, noeud) == NULL))
          {
            EF_Noeud_Libre *data = noeud->data;
            
            if ((data->relatif != NULL) &&
                (g_list_find (noeuds_done, data->relatif) != NULL))
              noeuds_todo = g_list_append (noeuds_todo, noeud);
          }
          list_parcours2 = g_list_next (list_parcours2);
        }
      }
      list_parcours = g_list_next (list_parcours);
    }
  }
  
  // Ensuite, on parcours les charges pour déterminer si certaines sont
  // utilisées par les noeuds ou barres.
  list_parcours = p->actions;
  while (list_parcours != NULL)
  {
    Action *action = list_parcours->data;
    GList  *liste_parcours2 = _1990_action_charges_renvoie (action);
    
    while (liste_parcours2 != NULL)
    {
      Charge *charge = liste_parcours2->data;
      switch (charge->type)
      {
        case CHARGE_NOEUD :
        {
          Charge_Noeud *charge_d = charge->data;
          GList        *liste_parcours3 = charge_d->noeuds;
          
          while (liste_parcours3 != NULL)
          {
            EF_Noeud *noeud = liste_parcours3->data;
            
            if (charges_dep != NULL)
            {
              if (g_list_find (noeuds_done, noeud) != NULL)
              {
                BUG (common_selection_ajout_nombre (charge,
                                                    charges_dep,
                                                    LISTE_CHARGES,
                                                    p),
                     FALSE,
                     g_list_free (barres_todo);
                       g_list_free (noeuds_done);
                       if (barres_dep != NULL)
                         g_list_free (*barres_dep);
                       if (noeuds_dep != NULL)
                         g_list_free (*noeuds_dep);
                       g_list_free (*charges_dep);)
                liste_parcours3 = NULL;
              }
            }
            else if ((verif == TRUE) &&
                     (g_list_find (noeuds_done, noeud) != NULL))
            {
              g_list_free (noeuds_done);
              g_list_free (barres_todo);
              return TRUE;
            }
            
            liste_parcours3 = g_list_next (liste_parcours3);
          }
          break;
        }
        case CHARGE_BARRE_PONCTUELLE :
        case CHARGE_BARRE_REPARTIE_UNIFORME :
        {
          Charge_Barre_Ponctuelle *charge_d = charge->data;
          GList                   *liste_parcours3 = charge_d->barres;
          
          while (liste_parcours3 != NULL)
          {
            EF_Barre *barre = liste_parcours3->data;
            
            if (charges_dep != NULL)
            {
              if (g_list_find (barres_todo, barre) != NULL)
              {
                BUG (common_selection_ajout_nombre (charge,
                                                    charges_dep,
                                                    LISTE_CHARGES,
                                                    p),
                     FALSE,
                     g_list_free (barres_todo);
                       g_list_free (noeuds_done);
                       if (barres_dep != NULL)
                         g_list_free (*barres_dep);
                       if (noeuds_dep != NULL)
                         g_list_free (*noeuds_dep);
                       g_list_free (*charges_dep);)
                liste_parcours3 = NULL;
              }
            }
            else if ((verif == TRUE) &&
                     (g_list_find (barres_todo, barre) != NULL))
            {
              g_list_free (noeuds_done);
              g_list_free (barres_todo);
              return TRUE;
            }
            
            liste_parcours3 = g_list_next (liste_parcours3);
          }
          break;
        }
        default :
        {
          FAILCRIT (FALSE,
                    (gettext ("Type de charge %d inconnu.\n"), charge->type);
                      g_list_free (barres_todo);
                      g_list_free (noeuds_done);
                      if (barres_dep != NULL)
                        g_list_free (*barres_dep);
                      if (noeuds_dep != NULL)
                        g_list_free (*noeuds_dep);
                      if (charges_dep != NULL)
                        g_list_free (*charges_dep);)
          break;
        }
      }
      
      liste_parcours2 = g_list_next (liste_parcours2);
    }
    
    list_parcours = g_list_next (list_parcours);
  }
  
  g_list_free (noeuds_done);
  g_list_free (barres_todo);
  
  if (verif == FALSE)
    return TRUE;
  else
    return FALSE;
}


gboolean
_1992_1_1_barres_angle_rotation (EF_Noeud *debut,
                                 EF_Noeud *fin,
                                 double   *y,
                                 double   *z)
/**
 * \brief Calcule les deux angles de rotation pour faire tourner une barre
 *        horizontale en une barre parallèle à une droite passant par les
 *        noeuds debut et fin.
 * \param debut : premier noeud,
 * \param fin : dernier noeud de la droite dont on souhaite connaitre les
 *              angles de rotation,
 * \param y : angle autour de l'axe y,
 * \param z : angle autour de l'axe z.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - debut == NULL,
 *     - fin == NULL,
 *     - y == NULL,
 *     - z == NULL,
 *     - #EF_noeuds_distance_x_y_z.
 */
{
  double xx, yy, zz, ll;
  
  BUGPARAM (debut, "%p", debut, FALSE)
  BUGPARAM (fin, "%p", fin, FALSE)
  BUGPARAM (y, "%p", y, FALSE)
  BUGPARAM (z, "%p", z, FALSE)
  
  ll = EF_noeuds_distance_x_y_z (debut, fin, &xx, &yy, &zz);
  
  BUG (!isnan(ll), FALSE)
  INFO (!errmoy (ll, ERRMOY_DIST),
        FALSE,
        (gettext ("La distance entre les noeuds %d et %d est nulle\n"),
                  debut->numero,
                  fin->numero);)
  
  // Détermination de l'angle de rotation autour de l'axe Y.
  *y = asin (zz / ll);
  if (errmoy (ll * ll - zz * zz, ERRMOY_DIST))
    *z = 0.;
  else
  {
    if (xx > 0.)
      *z = asin (yy / sqrt (ll * ll - zz * zz));
    else
      *z = M_PI - asin(yy / sqrt(ll * ll - zz * zz));
  }
  
  return TRUE;
}


gboolean
_1992_1_1_barres_change_type (EF_Barre    *barre,
                              Type_Element type,
                              Projet      *p)
/**
 * \brief Change le type d'une barre.
 * \param barre : barre à modifier,
 * \param type : nouveau type,
 * \param p : variable projet.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - barre == NULL,
 *     - p == NULL,
 *     - type inconnu.
 */
{
  BUGPARAM (barre, "%p", barre, FALSE)
  BUGPARAM (p, "%p", p, FALSE)
  
  switch (type)
  {
    case BETON_ELEMENT_POTEAU :
    case BETON_ELEMENT_POUTRE :
    {
#ifdef ENABLE_GTK
      gtk_widget_queue_resize (GTK_WIDGET (gtk_builder_get_object (
                                       UI_BAR.builder, "EF_barres_treeview")));
#endif
      barre->type = type;
      break;
    }
    default :
    {
      FAILCRIT (FALSE, ("Le type de l'élément %d est inconnu.\n", type);)
      break;
    }
  }
  
  return TRUE;
}


gboolean
_1992_1_1_barres_change_section (EF_Barre   *barre,
                                 Section    *section,
                                 Projet     *p)
/**
 * \brief Change la section d'une barre.
 * \param barre : barre à modifier,
 * \param section : la nouvelle section,
 * \param p : variable projet.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - barre == NULL,
 *     - p == NULL,
 *     - section == NULL.
 */
{
  BUGPARAM (barre, "%p", barre, FALSE)
  BUGPARAM (p, "%p", p, FALSE)
  BUGPARAM (section, "%p", section, FALSE)
  
  if (barre->section == section)
    return TRUE;
  
  barre->section = section;
  
  BUG (EF_calculs_free (p), FALSE)
  
#ifdef ENABLE_GTK
  BUG (m3d_barre (&UI_M3D, barre), FALSE)
  BUG (m3d_rafraichit (p), FALSE)
  
  if (UI_BAR.builder != NULL)
    gtk_widget_queue_resize (GTK_WIDGET (gtk_builder_get_object (
                                       UI_BAR.builder, "EF_barres_treeview")));
#endif
  
  return TRUE;
}


gboolean
_1992_1_1_barres_change_materiau (EF_Barre    *barre,
                                  EF_Materiau *materiau,
                                  Projet      *p)
/**
 * \brief Change le matériau d'une barre.
 * \param barre : barre à modifier,
 * \param materiau : le nouveau materiau,
 * \param p : variable projet.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - barre == NULL,
 *     - p == NULL,
 *     - #EF_calculs_free;
 */
{
  BUGPARAM (barre, "%p", barre, FALSE)
  BUGPARAM (p, "%p", p, FALSE)
  
  if (barre->materiau == materiau)
    return TRUE;
  
  barre->materiau = materiau;
  
  BUG (EF_calculs_free (p), FALSE)
  
#ifdef ENABLE_GTK
  if (UI_BAR.builder != NULL)
    gtk_widget_queue_resize (GTK_WIDGET (gtk_builder_get_object (
                                       UI_BAR.builder, "EF_barres_treeview")));
#endif
  
  return TRUE;
}


gboolean
_1992_1_1_barres_change_angle (EF_Barre *barre,
                               Flottant  angle,
                               Projet   *p)
/**
 * \brief Change l'angle d'une barre autour de son axe x.
 * \param barre : barre à modifier,
 * \param angle : le nouvel angle,
 * \param p : variable projet.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - barre == NULL,
 *     - p == NULL.
 */
{
#ifdef ENABLE_GTK
  GList *liste_barre = NULL;
#endif
  
  BUGPARAM (barre, "%p", barre, FALSE)
  BUGPARAM (p, "%p", p, FALSE)
  
  if (errmax (m_g (barre->angle), m_g (angle)))
    return TRUE;
  
  barre->angle = angle;
  
  BUG (EF_calculs_free (p), FALSE)
  
#ifdef ENABLE_GTK
  liste_barre = g_list_append (liste_barre, barre);
  BUG (m3d_actualise_graphique (p, NULL, liste_barre), FALSE)
  BUG (m3d_rafraichit (p), FALSE)
  g_list_free (liste_barre);
  
  if (UI_BAR.builder != NULL)
    gtk_widget_queue_resize (GTK_WIDGET (gtk_builder_get_object (
                                       UI_BAR.builder, "EF_barres_treeview")));
#endif
  
  return TRUE;
}


gboolean
_1992_1_1_barres_change_noeud (EF_Barre *barre,
                               EF_Noeud *noeud,
                               gboolean noeud_1,
                               Projet *p)
/**
 * \brief Change un des deux noeuds d'extrémité d'une barre.
 * \param barre : barre à modifier,
 * \param noeud : le nouveau noeud,
 * \param noeud_1 : TRUE si le noeud_debut est à modifier,
 *                  FALSE si le noeud_fin est à modifier.
 * \param p : variable projet.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - barre == NULL,
 *     - noeud == NULL,
 *     - p == NULL,
 *     - noeud_1 == TRUE && barre->noeud_fin == noeud,
 *     - noeud_1 == FALSE && barre->noeud_debut == noeud,
 *     - #_1992_1_1_barres_cherche_dependances,
 *     - le noeud est dépendant de la barre,
 *     - #EF_calculs_free.
 */
{
  GList *liste_barre = NULL;
  GList *liste_noeuds_dep;
  
  BUGPARAM (barre, "%p", barre, FALSE)
  BUGPARAM (noeud, "%p", noeud, FALSE)
  BUGPARAM (p, "%p", p, FALSE)
  INFO (!((noeud_1 == TRUE) && (barre->noeud_fin == noeud)),
        FALSE,
        (gettext ("Impossible d'appliquer le même noeud aux deux extrémités d'une barre.\n"));)
  INFO (!((noeud_1 == FALSE) && (barre->noeud_debut == noeud)),
        FALSE,
        (gettext ("Impossible d'appliquer le même noeud aux deux extrémités d'une barre.\n"));)
  
  if ((noeud_1) && (barre->noeud_debut == noeud))
    return TRUE;
  if ((!noeud_1) && (barre->noeud_fin == noeud))
    return TRUE;
  liste_barre = g_list_append (liste_barre, barre);
  BUG (_1992_1_1_barres_cherche_dependances (p,
                                             NULL,
                                             NULL,
                                             NULL,
                                             NULL,
                                             NULL,
                                             liste_barre,
                                             &liste_noeuds_dep,
                                             NULL,
                                             NULL,
                                             FALSE,
                                             TRUE),
       FALSE)
  INFO (g_list_find (liste_noeuds_dep, noeud) == NULL,
        FALSE,
        (gettext ("Impossible d'affecter le noeud %d à la barre %d car il est dépendant de la barre à modifier.\n"),
                  noeud->numero,
                  barre->numero);)
  g_list_free (liste_noeuds_dep);
  
  if (noeud_1 == TRUE)
    barre->noeud_debut = noeud;
  else
    barre->noeud_fin = noeud;
  
  BUG (EF_calculs_free (p), FALSE)
  
#ifdef ENABLE_GTK
  BUG (m3d_actualise_graphique (p, NULL, liste_barre), FALSE)
  BUG (m3d_rafraichit (p), FALSE)
#endif
  
  g_list_free (liste_barre);
  
#ifdef ENABLE_GTK
  if (UI_BAR.builder != NULL)
    gtk_widget_queue_resize (GTK_WIDGET (gtk_builder_get_object (
                                       UI_BAR.builder, "EF_barres_treeview")));
#endif
  
  return TRUE;
}


gboolean
_1992_1_1_barres_change_relachement (EF_Barre       *barre,
                                     EF_Relachement *relachement,
                                     Projet         *p)
/**
 * \brief Change le relâchement d'une barre.
 * \param barre : barre à modifier,
 * \param relachement : nouveau relâchement,
 * \param p : variable projet.
 * \return
 *   Succès : TRUE.
 *   Échec : FALSE :
 *     - barre == NULL,
 *     - p == NULL,
 *     - #EF_calculs_free.
 */
{
  BUGPARAM (barre, "%p", barre, FALSE)
  BUGPARAM (p, "%p", p, FALSE)
  
  if (barre->relachement == relachement)
    return TRUE;
  
  barre->relachement = relachement;
  
  BUG (EF_calculs_free (p), FALSE)
  
#ifdef ENABLE_GTK
  if (UI_BAR.builder != NULL)
    gtk_widget_queue_resize (GTK_WIDGET (gtk_builder_get_object (
                                       UI_BAR.builder, "EF_barres_treeview")));
#endif
  
  return TRUE;
}


gboolean
_1992_1_1_barres_rigidite_ajout (Projet   *p,
                                 EF_Barre *element)
/**
 * \brief Ajouter un élément à la matrice de rigidité partielle et complète.
 * \param p : la variable projet,
 * \param element : la barre à ajouter.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL,
 *     - p->calculs.t_part == NULL,
 *     - element == NULL,
 *     - p->calculs.t_comp == NULL,
 *     - element->section == NULL,
 *     - distance entre le début et l'extrémité de la barre est nulle),
 *     - en cas d'erreur d'allocation mémoire,
 *     - en cas d'erreur due à une fonction interne.
 */
{
  EF_Noeud        *noeud1, *noeud2;
  int             *ai, *aj;
  double          *ax;
  double           x, y, z;
  int              k;
  cholmod_triplet *triplet;
  unsigned int     j;
  
  BUGPARAM (p, "%p", p, FALSE)
  INFO (p->calculs.t_part,
        FALSE,
        (gettext ("Il est nécessaire de lancer la fonction EF_calculs_initialise avant.\n"));)
  BUGPARAM (element, "%p", element, FALSE,)
  INFO (p->calculs.t_comp,
        FALSE,
        (gettext ("Il est nécessaire de lancer la fonction EF_calculs_initialise avant.\n"));)
  
  // Calcul de la matrice de rotation 3D qui permet de passer du repère local
  // au repère global. Elle est déterminée par le calcul de deux angles :
  // z faisant une rotation dans le repère local autour de l'axe z et y faisant
  // une rotation dans le repère local autour de l'axe y. Ainsi, le système
  // d'équation à résoudre est le suivant : \end{verbatim}\begin{displaymath}
  //   \begin{bmatrix} cos(z) & -sin(z) & 0 \\
  //                   sin(z) &  cos(z) & 0 \\
  //                     0    &    0    & 1
  //   \end{bmatrix} \cdot 
  //   \begin{bmatrix} cos(y) &  0  & -sin(y) \\
  //                     0    &  1  &    0  \\
  //                   sin(y) &  0  &  cos(y)
  //   \end{bmatrix} \cdot
  //   \begin{pmatrix} L \\
  //                   0 \\
  //                   0
  //   \end{pmatrix} =
  //   \begin{pmatrix} L_x \\
  //                   L_y \\
  //                   L_z
  //   \end{pmatrix}
  //   \end{displaymath}\begin{displaymath}
  //   \begin{bmatrix} cos(z) \cdot cos(y) & -sin(z) & -cos(z) \cdot sin(y) \\
  //                   sin(z) \cdot cos(y) &  cos(z) & -sin(z) \cdot sin(y) \\
  //                   sin(y)              &    0    &  cos(y)
  //   \end{bmatrix} \cdot
  //   \begin{pmatrix} L \\
  //                   0 \\
  //                   0
  //   \end{pmatrix} =
  //   \begin{pmatrix} L_x \\
  //                   L_y \\
  //                   L_z
  //   \end{pmatrix}
  //   \end{displaymath}\begin{displaymath}
  //   \begin{pmatrix} L \cdot cos(y) \cdot cos(z) \\
  //                   L \cdot cos(y) \cdot sin(z) \\
  //                   L \cdot sin(y)
  //   \end{pmatrix} =
  //   \begin{pmatrix} L_x \\
  //                   L_y \\
  //                   L_z
  //   \end{pmatrix}
  //   \end{displaymath}\begin{verbatim}
  //   avec L_x, L_y et L_z étant la différence, dans le repère global, entre
  //   les coordonnées des points définissant la fin et le début de la barre.
  //   À partir de ce système d'équations, il est possible de déterminer deux
  //   valeurs y.\end{verbatim}\begin{displaymath}
  //   y = arcsin \left( \frac{L_z}{L} \right) \texttt{ et }
  //   y = \pi - arcsin \left( \frac{L_z}{L} \right)
  //   \end{displaymath}\begin{verbatim}
  //   Il sera retenu la première solution pour des raisons de simplicité.
  //   Ensuite, en intégrant le résultat dans les deux autres équations
  //   restantes :\end{verbatim}\begin{displaymath}
  //   L \cdot cos \left ( arcsin \left( \frac{L_z}{L} \right) \right ) \cdot
  //   cos(z) = \sqrt{L^2-L_z^2} \cdot cos (z) = L_x\end{displaymath}
  //   \begin{displaymath}
  //   L \cdot cos \left ( arcsin \left( \frac{L_z}{L} \right) \right ) \cdot
  //   sin(z) = \sqrt{L^2-L_z^2} \cdot sin (z) = L_y\end{displaymath}
  //   \begin{verbatim}
  //   Il existe alors deux solutions :\end{verbatim}\begin{displaymath}
  //   z = arcsin \left( \frac{L_y}{\sqrt{L^2-L_z^2}} \right) \texttt{ et }
  //   z = \pi - arcsin \left( \frac{L_y}{\sqrt{L^2-L_z^2}} \right)
  //   \end{displaymath}\begin{verbatim}
  //   Après application de la première solution, les deux équations deviennent
  //   :\end{verbatim}\begin{displaymath}
  //   \left | L_x \right | = L_x \texttt{ et }
  //   L_y = L_y\end{displaymath}\begin{verbatim}
  //   et après application de la deuxième solution, les deux équations
  //   deviennent :\end{verbatim}\begin{displaymath}
  //   -\left | L_x \right | = L_x\texttt{ et }
  //   L_y = L_y\end{displaymath}\begin{verbatim}
  //   On constate que la seule différence est que la première solution doit
  //   être utilisée pour avoir un L_x positif et la deuxième solution doit
  //   être utilisée pour avoir un L_x négatif. Il est donc possible d'obtenir
  //   une seule et unique solution :\end{verbatim}\begin{displaymath}
  //   cos(z) = signe\{L_x\} \cdot \sqrt{\frac{L_x^2}{L^2-L_z^2}}\texttt{ et }
  //   sin(z) = \frac{yy}{\sqrt{L^2-L_z^2}}\end{displaymath}\begin{verbatim}
  x = m_g (element->angle) / 180 * M_PI;
  BUG (_1992_1_1_barres_angle_rotation (element->noeud_debut,
                                        element->noeud_fin,
                                        &y,
                                        &z),
       FALSE)
  triplet = cholmod_allocate_triplet (12,
                                      12,
                                      36,
                                      0,
                                      CHOLMOD_REAL,
                                      p->calculs.c);
  BUGCRIT (triplet, FALSE, (gettext ("Erreur d'allocation mémoire.\n"));)
  ai = (int *) triplet->i;
  aj = (int *) triplet->j;
  ax = (double *) triplet->x;
  for (k = 0; k < 4; k++)
  {
    ai[k * 9 + 0] = k * 3 + 0; aj[k * 9 + 0] = k * 3 + 0;
    ax[k * 9 + 0] = cos (y) * cos (z);
    ai[k * 9 + 1] = k * 3 + 0; aj[k * 9 + 1] = k * 3 + 1;
    ax[k * 9 + 1] = -sin (z);
    ai[k * 9 + 2] = k * 3 + 0; aj[k * 9 + 2] = k * 3 + 2;
    ax[k * 9 + 2] = -sin (y) * cos (z);
    ai[k * 9 + 3] = k * 3 + 1; aj[k * 9 + 3] = k * 3 + 0;
    ax[k * 9 + 3] = cos (x) * cos (y) * sin (z) - sin (x) * sin (y);
    ai[k * 9 + 4] = k * 3 + 1; aj[k * 9 + 4] = k * 3 + 1;
    ax[k * 9 + 4] = cos (x) * cos(z);
    ai[k * 9 + 5] = k * 3 + 1; aj[k * 9 + 5] = k * 3 + 2;
    ax[k * 9 + 5] = -cos (x) * sin (y) * sin (z) - sin (x) * cos (y);
    ai[k * 9 + 6] = k * 3 + 2; aj[k * 9 + 6] = k * 3 + 0;
    ax[k * 9 + 6] = sin (x) * cos (y) * sin (z) + cos (x) * sin (y);
    ai[k * 9 + 7] = k * 3 + 2; aj[k * 9 + 7] = k * 3 + 1;
    ax[k * 9 + 7] = sin (x) * cos (z);
    ai[k * 9 + 8] = k * 3 + 2; aj[k * 9 + 8] = k * 3 + 2;
    ax[k * 9 + 8] = cos (x) * cos (y) - sin (x) * sin (y) * sin (z);
  }
  triplet->nnz = 36;
  element->m_rot = cholmod_triplet_to_sparse(triplet, 0, p->calculs.c);
  BUGCRIT (element->m_rot,
           FALSE,
           (gettext ("Erreur d'allocation mémoire.\n"));
             cholmod_free_triplet (&triplet, p->calculs.c);)
  cholmod_free_triplet (&triplet, p->calculs.c);
  element->m_rot_t = cholmod_transpose ( element->m_rot, 1, p->calculs.c);
  BUGCRIT (element->m_rot_t,
           FALSE,
           (gettext ("Erreur d'allocation mémoire.\n"));)
  
  // Une fois la matrice de rotation déterminée, il est nécessaire de calculer
  // la matrice de rigidité élémentaire dans le repère local. La poutre pouvant
  // être discrétisée, une matrice de rigidité élémentaire est déterminée pour
  // chaque tronçon.
  for (j = 0; j < element->discretisation_element + 1; j++)
  {
    double          MA, MB;
    double          phia_iso, phib_iso;
    double          es_l;
    int             num1, num2;
    cholmod_sparse *sparse_tmp, *matrice_rigidite_globale;
    double          ll;
    unsigned int    i;
    int            *ai2, *aj2;
    double         *ax2;
    int            *ai3, *aj3;
    double         *ax3;
    
    // Détermination du noeud de départ et de fin
    if (j == 0)
    {
      noeud1 = element->noeud_debut;
      if (element->discretisation_element != 0)
        noeud2 = g_list_first (element->nds_inter)->data;
      else
        noeud2 = element->noeud_fin;
    }
    else if (j == element->discretisation_element)
    {
      noeud1 = g_list_nth_data (element->nds_inter, j - 1);
      noeud2 = element->noeud_fin;
    }
    else
    {
      noeud1 = g_list_nth_data (element->nds_inter, j - 1);
      noeud2 = g_list_nth_data (element->nds_inter, j);
    }
    
    num1 = g_list_index (p->modele.noeuds, noeud1);
    num2 = g_list_index (p->modele.noeuds, noeud2);
    
    // Calcul des L_x, L_y, L_z et L.
    ll = EF_noeuds_distance (noeud2, noeud1);
    BUG (!isnan(ll), FALSE)
    
    // Détermination des paramètres de souplesse de l'élément de barre par
    // l'utilisation des fonctions EF_sections_ay, by, cy, az, bz et cz.
    element->info_EF[j].ay = EF_sections_ay (element, j);
    element->info_EF[j].by = EF_sections_by (element, j);
    element->info_EF[j].cy = EF_sections_cy (element, j);
    element->info_EF[j].az = EF_sections_az (element, j);
    element->info_EF[j].bz = EF_sections_bz (element, j);
    element->info_EF[j].cz = EF_sections_cz (element, j);
    
    // Calcul des coefficients kA et kB définissant l'inverse de la raideur
    // aux noeuds. Ainsi k = 0 en cas d'encastrement et infini en cas
    // d'articulation.
    if (element->relachement == NULL)
    {
      element->info_EF[j].kAx = 0;
      element->info_EF[j].kBx = 0;
      element->info_EF[j].kAy = 0;
      element->info_EF[j].kBy = 0;
      element->info_EF[j].kAz = 0;
      element->info_EF[j].kBz = 0;
    }
    else
    {
      if (noeud1 != element->noeud_debut)
      {
        element->info_EF[j].kAx = 0;
        element->info_EF[j].kAy = 0;
        element->info_EF[j].kAz = 0;
      }
      else
      {
        switch (element->relachement->rx_debut)
        {
          case EF_RELACHEMENT_BLOQUE :
          {
            element->info_EF[j].kAx = 0.;
            break;
          }
          case EF_RELACHEMENT_LIBRE :
          {
            element->info_EF[j].kAx = MAXDOUBLE;
            break;
          }
          case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
          {
            EF_Relachement_Donnees_Elastique_Lineaire *donnees;
            
            donnees = (EF_Relachement_Donnees_Elastique_Lineaire *)
                                               element->relachement->rx_d_data;
            if (errmoy (m_g (donnees->raideur), ERRMOY_RAIDEUR))
              element->info_EF[j].kAx = MAXDOUBLE;
            else
              element->info_EF[j].kAx = 1. / m_g (donnees->raideur);
            break;
          }
          case EF_RELACHEMENT_UNTOUCH :
          default :
          {
            FAILCRIT (FALSE,
                      (gettext ("Relachement %d inconnu."),
                                element->relachement->rx_debut);)
            break;
          }
        }
        
        switch (element->relachement->ry_debut)
        {
          case EF_RELACHEMENT_BLOQUE :
          {
            element->info_EF[j].kAy = 0;
            break;
          }
          case EF_RELACHEMENT_LIBRE :
          {
            element->info_EF[j].kAy = MAXDOUBLE;
            break;
          }
          case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
          {
            EF_Relachement_Donnees_Elastique_Lineaire *donnees;
            
            donnees = (EF_Relachement_Donnees_Elastique_Lineaire *)
                                               element->relachement->ry_d_data;
            if (errmoy (m_g (donnees->raideur), ERRMOY_RAIDEUR))
              element->info_EF[j].kAy = MAXDOUBLE;
            else
              element->info_EF[j].kAy = 1. / m_g (donnees->raideur);
            break;
          }
          case EF_RELACHEMENT_UNTOUCH :
          default :
          {
            FAILCRIT (FALSE,
                      (gettext ("Relachement %d inconnu."),
                                element->relachement->ry_debut);)
            break;
          }
        }
        
        switch (element->relachement->rz_debut)
        {
          case EF_RELACHEMENT_BLOQUE :
          {
            element->info_EF[j].kAz = 0;
            break;
          }
          case EF_RELACHEMENT_LIBRE :
          {
            element->info_EF[j].kAz = MAXDOUBLE;
            break;
          }
          case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
          {
            EF_Relachement_Donnees_Elastique_Lineaire *donnees;
            
            donnees = (EF_Relachement_Donnees_Elastique_Lineaire *)
                                               element->relachement->rz_d_data;
            if (errmoy (m_g (donnees->raideur), ERRMOY_RAIDEUR))
              element->info_EF[j].kAz = MAXDOUBLE;
            else
              element->info_EF[j].kAz = 1. / m_g (donnees->raideur);
            break;
          }
          case EF_RELACHEMENT_UNTOUCH :
          default :
          {
            FAILCRIT (FALSE,
                      (gettext ("Relachement %d inconnu."),
                                element->relachement->rz_debut);)
            break;
          }
        }
      }
      
      if (noeud2 != element->noeud_fin)
      {
        element->info_EF[j].kBx = 0;
        element->info_EF[j].kBy = 0;
        element->info_EF[j].kBz = 0;
      }
      else
      {
        switch (element->relachement->rx_fin)
        {
          case EF_RELACHEMENT_BLOQUE :
          {
            element->info_EF[j].kBx = 0;
            break;
          }
          case EF_RELACHEMENT_LIBRE :
          {
            element->info_EF[j].kBx = MAXDOUBLE;
            break;
          }
          case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
          {
            EF_Relachement_Donnees_Elastique_Lineaire *donnees;
            
            donnees = (EF_Relachement_Donnees_Elastique_Lineaire *)
                                               element->relachement->rx_f_data;
            if (errmoy (m_g (donnees->raideur), ERRMOY_RAIDEUR))
              element->info_EF[j].kBx = MAXDOUBLE;
            else
              element->info_EF[j].kBx = 1. / m_g (donnees->raideur);
            break;
          }
          case EF_RELACHEMENT_UNTOUCH :
          default :
          {
            FAILCRIT (FALSE,
                      (gettext ("Relachement %d inconnu."),
                                element->relachement->rx_fin);)
            break;
          }
        }
        
        switch (element->relachement->ry_fin)
        {
          case EF_RELACHEMENT_BLOQUE :
          {
            element->info_EF[j].kBy = 0;
            break;
          }
          case EF_RELACHEMENT_LIBRE :
          {
            element->info_EF[j].kBy = MAXDOUBLE;
            break;
          }
          case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
          {
            EF_Relachement_Donnees_Elastique_Lineaire *donnees;
            
            donnees = (EF_Relachement_Donnees_Elastique_Lineaire *)
                                               element->relachement->ry_f_data;
            if (errmoy (m_g (donnees->raideur), ERRMOY_RAIDEUR))
              element->info_EF[j].kBy = MAXDOUBLE;
            else
              element->info_EF[j].kBy = 1. / m_g (donnees->raideur);
            break;
          }
          case EF_RELACHEMENT_UNTOUCH :
          default :
          {
            FAILCRIT (FALSE,
                      (gettext ("Relachement %d inconnu."),
                                element->relachement->ry_fin);)
            break;
          }
        }
        
        switch (element->relachement->rz_fin)
        {
          case EF_RELACHEMENT_BLOQUE :
          {
            element->info_EF[j].kBz = 0;
            break;
          }
          case EF_RELACHEMENT_LIBRE :
          {
            element->info_EF[j].kBz = MAXDOUBLE;
            break;
          }
          case EF_RELACHEMENT_ELASTIQUE_LINEAIRE :
          {
            EF_Relachement_Donnees_Elastique_Lineaire *donnees;
            
            donnees = (EF_Relachement_Donnees_Elastique_Lineaire *)
                                               element->relachement->rz_f_data;
            if (errmoy (m_g (donnees->raideur), ERRMOY_RAIDEUR))
              element->info_EF[j].kBz = MAXDOUBLE;
            else
              element->info_EF[j].kBz = 1. / m_g (donnees->raideur);
            break;
          }
          case EF_RELACHEMENT_UNTOUCH :
          default :
          {
            FAILCRIT (FALSE,
                      (gettext ("Relachement %d inconnu."),
                                element->relachement->rx_debut);)
            break;
          }
        }
      }
    }
    
    // Calcul des valeurs de la matrice de rigidité locale :
    triplet = cholmod_allocate_triplet (12,
                                        12,
                                        40,
                                        0,
                                        CHOLMOD_REAL,
                                        p->calculs.c);
    BUGCRIT (triplet, FALSE, (gettext ("Erreur d'allocation mémoire.\n"));)
    ai = (int *) triplet->i;
    aj = (int *) triplet->j;
    ax = (double *) triplet->x;
    triplet->nnz = 40;
    i = 0;
    
    // Pour un élément travaillant en compression simple (aucune variante
    // possible dues aux relachements). Les valeurs de ES/L sont obtenues par
    // la fonction EF_sections_es_l :\end{verbatim}\begin{displaymath}
    // \begin{bmatrix}  \frac{E \cdot S}{L} & -\frac{E \cdot S}{L} \\
    //                 -\frac{E \cdot S}{L} &  \frac{E \cdot S}{L}
    // \end{bmatrix}\end{displaymath}\begin{verbatim}
    es_l = EF_sections_es_l (element, j, 0., ll);
    BUG (!isnan (es_l), FALSE, cholmod_free_triplet (&triplet, p->calculs.c);)
    ai[i] = 0;  aj[i] = 0;  ax[i] =  es_l; i++;
    ai[i] = 0;  aj[i] = 6;  ax[i] = -es_l; i++;
    ai[i] = 6;  aj[i] = 0;  ax[i] = -es_l; i++;
    ai[i] = 6;  aj[i] = 6;  ax[i] =  es_l; i++;
    
    // Détermination de la matrice de rigidité après prise en compte des
    // relachements autour de l'axe z :
    // Pour cela, il convient de prendre comme modèle de base une poutre
    // reposant sur deux appuis avec encastrement élastique, kAz et kAy, et de
    // déterminer l'effort tranchant et le moment fléchissant au droit de
    // chaque noeud. Les quatres cas de charge à étudier sont :
    // 1) Déplacement vertical imposé au noeud A. Les résultats sont exprimés
    // en fonction de la valeur du déplacement.
    // 2) Rotation imposée au noeud A avec relachement au noeud A ignoré pour
    // permettre la rotation. Les résultats sont exprimés en fonction de la
    // rotation.
    // 3) Déplacement vertical imposé au noeud B. Les résultats sont exprimés
    // en fontion de la valeur du déplacement.
    // 4) Rotation imposée au noeud B avec relachement au noeud B ignoré pour
    // permettre la rotation. Les résultats sont exprimés en fonction de la
    // rotation.
    // Pour chaque cas, il est d'abord calculé la rotation au noeud en
    // supposant le système isostatique (relachements remplacés par des
    // rotules). Ensuite les moments isostatiques sont déterminés sur la base
    // des rotations isostatiques.
    // 
    // Etude du cas 1 :
    // Les moments MA et MB sont obtenues par la fonction
    // EF_calculs_moment_hyper_z en supposant arctan(v/l) = 1/l
    // (hypothèse des petites déplacements).\end{verbatim}\begin{align*}
    //   a = &  \frac{M_A}{L}+\frac{M_B}{L} & b = & M_A\nonumber\\
    //   c = & -\frac{M_A}{L}-\frac{M_B}{L} & d = & M_B\end{align*}
    // \begin{verbatim}
    BUG (EF_calculs_moment_hyper_z (&(element->info_EF[j]),
                                    1. / ll,
                                    1. / ll,
                                    &MA,
                                    &MB),
         FALSE,
         cholmod_free_triplet (&triplet, p->calculs.c);)
    ai[i] = 1;  aj[i] = 1;  ax[i] =  MA / ll + MB / ll; i++;
    ai[i] = 5;  aj[i] = 1;  ax[i] =  MA;                i++;
    ai[i] = 7;  aj[i] = 1;  ax[i] = -MA / ll - MB / ll; i++;
    ai[i] = 11; aj[i] = 1;  ax[i] =  MB;                i++;
    
    // Etude du cas 2, rotation imposée r. phiA (positif) et phiB (négatif)
    // sont déterminés par la fonction EF_charge_barre_ponctuelle_def_ang_iso_z
    // :\end{verbatim}\begin{displaymath}
    //   M_A = \frac{1}{ k_A + \varphi_A \cdot \left(1-\frac{b}{2 \cdot
    //         (c + k_B)}\right)}\end{displaymath}\begin{displaymath}
    //   M_B = \frac{M_A*\varphi_B}{c+k_B}\end{displaymath}\begin{align*}
    //   e = & \frac{M_A}{L}-\frac{M_B}{L} & f = & M_A\nonumber\\
    //   g = & -\frac{M_A}{L}+\frac{M_B}{L} & h = & -M_B\end{align*}
    // \begin{verbatim}
    BUG (EF_charge_barre_ponctuelle_def_ang_iso_z (element,
                                                   j,
                                                   0.,
                                                   0.,
                                                   1.,
                                                   &phia_iso,
                                                   &phib_iso),
         FALSE,
         cholmod_free_triplet (&triplet, p->calculs.c);)
    if (errrel (element->info_EF[j].kAz, MAXDOUBLE))
      MA = 0.;
    else if (errrel (element->info_EF[j].kBz, MAXDOUBLE))
      MA = 1. / (element->info_EF[j].kAz + phia_iso);
    else
      MA = 1. / (element->info_EF[j].kAz + phia_iso *
           (1 - element->info_EF[j].bz / (2 * (element->info_EF[j].cz +
           element->info_EF[j].kBz))));
    MB = MA * phib_iso / (element->info_EF[j].cz + element->info_EF[j].kBz);
    ai[i] = 1;  aj[i] = 5;  ax[i] =  MA / ll - MB / ll; i++;
    ai[i] = 5;  aj[i] = 5;  ax[i] =  MA;                i++;
    ai[i] = 7;  aj[i] = 5;  ax[i] = -MA / ll + MB / ll; i++;
    ai[i] = 11; aj[i] = 5;  ax[i] = -MB;                i++;
    
    // Etude du cas 3 : Les moments MA et MB sont obtenues par la fonction
    // EF_calculs_moment_hyper_z en supposant arctan(-1/l) = -1/l (hypothèse
    // des petites déplacements).\end{verbatim}\begin{align*}
    //   i = &  \frac{M_A}{L}+\frac{M_B}{L} & j = & M_A\nonumber\\
    //   k = & -\frac{M_A}{L}-\frac{M_B}{L} & l = & M_B\end{align*}
    //   \begin{verbatim}
    BUG (EF_calculs_moment_hyper_z (&(element->info_EF[j]),
                                    -1. / ll,
                                    -1. / ll,
                                    &MA,
                                    &MB),
         FALSE,
         cholmod_free_triplet (&triplet, p->calculs.c);)
    ai[i] = 1;  aj[i] = 7;  ax[i] =  MA / ll + MB / ll; i++;
    ai[i] = 5;  aj[i] = 7;  ax[i] =  MA;                i++;
    ai[i] = 7;  aj[i] = 7;  ax[i] = -MA / ll - MB / ll; i++;
    ai[i] = 11; aj[i] = 7;  ax[i] =  MB;                i++;
    
    // Etude du cas 4, rotation imposée r. phiA (négatif) et phiB (positif)
    // sont déterminés par la fonction EF_charge_barre_ponctuelle_def_ang_iso_z
    // :\end{verbatim}\begin{displaymath}
    //   M_B = \frac{1}{ k_B + \varphi_B \cdot \left(1-\frac{b}{2 \cdot
    //         (c+k_A)}\right)}\end{displaymath}\begin{displaymath}
    //   M_A = \frac{M_B*\varphi_A}{c+k_A}\end{displaymath}\begin{align*}
    //   m = & -\frac{M_A}{L}+\frac{M_B}{L} & n = & -M_A\nonumber\\
    //   o = & \frac{M_A}{L}-\frac{M_B}{L} & p = & M_B\end{align*}
    //   \begin{verbatim}
    // L'ensemble des valeurs sont à insérer dans la matrice suivante et permet
    // d'obtenir la matrice de rigidité élémentaire.\end{verbatim}
    // \begin{displaymath}
    BUG (EF_charge_barre_ponctuelle_def_ang_iso_z (element,
                                                   j,
                                                   ll,
                                                   0.,
                                                   1.,
                                                   &phia_iso,
                                                   &phib_iso),
         FALSE,
         cholmod_free_triplet (&triplet, p->calculs.c);)
    if (errrel (element->info_EF[j].kBz, MAXDOUBLE))
      MB = 0.;
    else if (errrel (element->info_EF[j].kAz, MAXDOUBLE))
      MB = 1. / (element->info_EF[j].kBz + phib_iso);
    else
      MB = 1. / (element->info_EF[j].kBz + phib_iso *
           (1 - element->info_EF[j].bz / (2 * (element->info_EF[j].cz +
           element->info_EF[j].kAz))));
    MA = MB * phia_iso / (element->info_EF[j].cz + element->info_EF[j].kAz);
    
    ai[i] = 1;  aj[i] = 11; ax[i] = -MA / ll + MB / ll; i++;
    ai[i] = 5;  aj[i] = 11; ax[i] = -MA;                i++;
    ai[i] = 7;  aj[i] = 11; ax[i] = +MA / ll - MB / ll; i++;
    ai[i] = 11; aj[i] = 11; ax[i] = +MB;                i++;
    // \begin{bmatrix}K_e\end{bmatrix} = 
    // \begin{bmatrix}  a & e & i & m\\
    //                  b & f & j & n\\
    //                  c & g & k & o\\
    //                  d & h & l & p
    // \end{bmatrix}\end{displaymath}\begin{verbatim}
      
    // Détermination de la matrice de rigidité après prise en compte des
    // relachements autour de l'axe y. La méthode est identique que
    // précédemment, au signe près.
    // Etude du cas 1 :
    // Les moments MA et MB sont obtenues par la fonction
    // EF_calculs_moment_hyper_y en supposant arctan(v/l) = 1/l (hypothèse des
    // petites déplacements).\end{verbatim}\begin{align*}
    //     a = & -\frac{M_A}{L}-\frac{M_B}{L} & b = & M_A\nonumber\\
    //     c = &  \frac{M_A}{L}+\frac{M_B}{L} & d = & M_B\end{align*}
    // \begin{verbatim}
    BUG (EF_calculs_moment_hyper_y (&(element->info_EF[j]),
                                    1. / ll,
                                    1. / ll,
                                    &MA,
                                    &MB),
         FALSE,
         cholmod_free_triplet (&triplet, p->calculs.c);)
    ai[i] = 2;  aj[i] = 2;  ax[i] =  MA / ll + MB / ll; i++;
    ai[i] = 4;  aj[i] = 2;  ax[i] = -MA;                i++;
    ai[i] = 8;  aj[i] = 2;  ax[i] = -MA / ll - MB / ll; i++;
    ai[i] = 10; aj[i] = 2;  ax[i] = -MB;                i++;
    
    // Etude du cas 2, rotation imposée r. phiA (positif) et phiB (négatif)
    // sont déterminés par la fonction EF_charge_barre_ponctuelle_def_ang_iso_y
    // :\end{verbatim}\begin{displaymath}
    //   M_A = \frac{1}{ k_A - \varphi_A \cdot \left(1-\frac{b}{2 \cdot
    //         (c+k_B)}\right)}\end{displaymath}\begin{displaymath}
    //   M_B = \frac{M_A*\varphi_B}{c+k_B}\end{displaymath}\begin{align*}
    //   e = & -\frac{M_A}{L}-\frac{M_B}{L} & f = & M_A\nonumber\\
    //   g = &  \frac{M_A}{L}+\frac{M_B}{L} & h = & -M_B\end{align*}
    //   \begin{verbatim}
    BUG (EF_charge_barre_ponctuelle_def_ang_iso_y (element,
                                                   j,
                                                   0.,
                                                   0.,
                                                   1.,
                                                   &phia_iso,
                                                   &phib_iso),
         FALSE,
         cholmod_free_triplet (&triplet, p->calculs.c);)
    if (errrel (element->info_EF[j].kAy, MAXDOUBLE))
      MA = 0.;
    else if (errrel (element->info_EF[j].kBy, MAXDOUBLE))
      MA = 1. / (element->info_EF[j].kAy - phia_iso);
    else
      MA = 1. / (element->info_EF[j].kAy - phia_iso * (1 - 
           element->info_EF[j].by / (2 * (element->info_EF[j].cy +
           element->info_EF[j].kBy))));
    MB = MA * phib_iso / (element->info_EF[j].cy + element->info_EF[j].kBy);
    ai[i] = 2;  aj[i] = 4;  ax[i] =  MA / ll - MB / ll; i++;
    ai[i] = 4;  aj[i] = 4;  ax[i] = -MA;                i++;
    ai[i] = 8;  aj[i] = 4;  ax[i] = -MA / ll + MB / ll; i++;
    ai[i] = 10; aj[i] = 4;  ax[i] =  MB;                i++;
    
    // Etude du cas 3 : Les moments MA et MB sont obtenues par la fonction
    // EF_calculs_moment_hyper_y en supposant arctan(-1/l) = -1/l (hypothèse
    // des petites déplacements).\end{verbatim}\begin{align*}
    //   i = & -\frac{M_A}{L}-\frac{M_B}{L} & j = & M_A\nonumber\\
    //   k = &  \frac{M_A}{L}+\frac{M_B}{L} & l = & M_B\end{align*}
    //   \begin{verbatim}
    BUG (EF_calculs_moment_hyper_y (&(element->info_EF[j]),
                                    -1. / ll,
                                    -1. / ll,
                                    &MA,
                                    &MB),
         FALSE,
         cholmod_free_triplet (&triplet, p->calculs.c);)
    ai[i] = 2;  aj[i] = 8;  ax[i] =  MA / ll + MB / ll; i++;
    ai[i] = 4;  aj[i] = 8;  ax[i] = -MA;                i++;
    ai[i] = 8;  aj[i] = 8;  ax[i] = -MA / ll - MB / ll; i++;
    ai[i] = 10; aj[i] = 8;  ax[i] = -MB;                i++;
    
    // Etude du cas 4, rotation imposée r. phiA (négatif) et phiB (positif)
    // sont déterminés par la fonction EF_charge_barre_ponctuelle_def_ang_iso_y
    // :\end{verbatim}\begin{displaymath}
    //   M_B = \frac{1}{ k_B - \varphi_B \cdot \left(1-\frac{b}{2 \cdot
    //         (c+k_A)}\right)}\end{displaymath}\begin{displaymath}
    //   M_A = \frac{M_B*\varphi_A}{c+k_A}\end{displaymath}\begin{align*}
    //   m = & -\frac{M_A}{L}-\frac{M_B}{L} & n = & M_A\nonumber\\
    //   o = & \frac{M_A}{L}+\frac{M_B}{L} & p = & M_B\end{align*}
    //   \begin{verbatim}
    BUG (EF_charge_barre_ponctuelle_def_ang_iso_y (element,
                                                   j,
                                                   ll,
                                                   0.,
                                                   1.,
                                                   &phia_iso,
                                                   &phib_iso),
         FALSE,
         cholmod_free_triplet (&triplet, p->calculs.c);)
    if (errrel (element->info_EF[j].kBy, MAXDOUBLE))
      MB = 0.;
    else if (errrel (element->info_EF[j].kAy, MAXDOUBLE))
      MB = 1. / (element->info_EF[j].kBy - phib_iso);
    else
      MB = 1. / (element->info_EF[j].kBy - phib_iso * (1 -
           element->info_EF[j].by / (2 * (element->info_EF[j].cy +
           element->info_EF[j].kAy))));
    MA = MB * phia_iso / (element->info_EF[j].cy + element->info_EF[j].kAy);
    ai[i] = 2;  aj[i] = 10; ax[i] = -MA / ll + MB / ll; i++;
    ai[i] = 4;  aj[i] = 10; ax[i] =  MA;                i++;
    ai[i] = 8;  aj[i] = 10; ax[i] =  MA / ll - MB / ll; i++;
    ai[i] = 10; aj[i] = 10; ax[i] = -MB;                i++;
    
    // Pour un élément travaillant en torsion simple dont l'une des extrémités
    // est relaxée :\end{verbatim}\begin{displaymath}
    // \begin{bmatrix}      0    &       0      \\
    //                      0    &       0      
    // \end{bmatrix}\end{displaymath}\begin{verbatim}
    if (((j == 0) && (element->relachement != NULL) &&
        (element->relachement->rx_debut == EF_RELACHEMENT_LIBRE)) ||
        ((j==element->discretisation_element) &&
         (element->relachement != NULL) &&
         (element->relachement->rx_fin == EF_RELACHEMENT_LIBRE)))
    {
      ai[i] = 3;  aj[i] = 3;  ax[i] = 0; i++;
      ai[i] = 3;  aj[i] = 9;  ax[i] = 0; i++;
      ai[i] = 9;  aj[i] = 3;  ax[i] = 0; i++;
      ai[i] = 9;  aj[i] = 9;  ax[i] = 0; i++;
    }
    // Pour un élément travaillant en torsion simple dont aucune des extrémités
    // n'est relaxée :\end{verbatim}\begin{displaymath}
    // \begin{bmatrix}  \frac{G \cdot J}{L} & -\frac{G \cdot J}{L} \\
    //                 -\frac{G \cdot J}{L} &  \frac{G \cdot J}{L}
    // \end{bmatrix}\end{displaymath}\begin{verbatim}
    else
    {
      double gj_l = EF_sections_gj_l (element, j);
      ai[i] = 3;  aj[i] = 3;  ax[i] =  gj_l; i++;
      ai[i] = 3;  aj[i] = 9;  ax[i] = -gj_l; i++;
      ai[i] = 9;  aj[i] = 3;  ax[i] = -gj_l; i++;
      ai[i] = 9;  aj[i] = 9;  ax[i] =  gj_l; i++;
    }
    
    cholmod_free_sparse (&element->info_EF[j].m_rig_loc,
                         p->calculs.c);
    element->info_EF[j].m_rig_loc = cholmod_triplet_to_sparse (triplet,
                                                               0,
                                                               p->calculs.c);
    cholmod_free_triplet (&triplet, p->calculs.c);
    BUGCRIT (element->info_EF[j].m_rig_loc,
             FALSE,
             (gettext ("Erreur d'allocation mémoire.\n"));)
    
    // Calcule la matrice locale dans le repère globale :\end{verbatim}
    // \begin{displaymath}
    //     [K]_{global} = [R] \cdot [K]_{local} \cdot [R]^{-1}
    //                  = [R] \cdot [K]_{local} \cdot [R]^T\end{displaymath}
    // \begin{verbatim}
    sparse_tmp = cholmod_ssmult (element->m_rot,
                                 element->info_EF[j].m_rig_loc,
                                 0,
                                 1,
                                 0,
                                 p->calculs.c);
    BUGCRIT (sparse_tmp,
             FALSE,
             (gettext( "Erreur d'allocation mémoire.\n"));)
    matrice_rigidite_globale = cholmod_ssmult (sparse_tmp,
                                               element->m_rot_t,
                                               0,
                                               1,
                                               0,
                                               p->calculs.c);
    cholmod_free_sparse (&(sparse_tmp), p->calculs.c);
    BUGCRIT (matrice_rigidite_globale,
             FALSE,
             (gettext ("Erreur d'allocation mémoire.\n"));)
    triplet = cholmod_sparse_to_triplet (matrice_rigidite_globale,
                                         p->calculs.c);
    cholmod_free_sparse (&(matrice_rigidite_globale), p->calculs.c);
    BUGCRIT (triplet,
             FALSE,
             (gettext ("Erreur d'allocation mémoire.\n"));)
    ai = (int *)     triplet->i;
    aj = (int *)     triplet->j;
    ax = (double *)  triplet->x;
    ai2 = (int *)    p->calculs.t_part->i;
    aj2 = (int *)    p->calculs.t_part->j;
    ax2 = (double *) p->calculs.t_part->x;
    ai3 = (int *)    p->calculs.t_comp->i;
    aj3 = (int *)    p->calculs.t_comp->j;
    ax3 = (double *) p->calculs.t_comp->x;
    
    // Insertion de la matrice de rigidité élémentaire dans la matrice de
    // rigidité globale partielle et complète.
    for (i = 0; i < triplet->nnz; i++)
    {
      if ((ai[i] < 6) &&
          (aj[i] < 6) &&
          (p->calculs.n_part[num1][ai[i]] != -1) &&
          (p->calculs.n_part[num1][aj[i]] != -1))
      {
        ai2[p->calculs.t_part_en_cours] = p->calculs.n_part[num1][ai[i]];
        aj2[p->calculs.t_part_en_cours] = p->calculs.n_part[num1][aj[i]];
        ax2[p->calculs.t_part_en_cours] = ax[i];
        p->calculs.t_part_en_cours++;
      }
      else if ((ai[i] < 6) &&
               (aj[i] >= 6) &&
               (p->calculs.n_part[num1][ai[i]] != -1) &&
               (p->calculs.n_part[num2][aj[i]-6] != -1))
      {
        ai2[p->calculs.t_part_en_cours] = p->calculs.n_part[num1][ai[i]];
        aj2[p->calculs.t_part_en_cours] = p->calculs.n_part[num2][aj[i]-6];
        ax2[p->calculs.t_part_en_cours] = ax[i];
        p->calculs.t_part_en_cours++;
      }
      else if ((ai[i] >= 6) &&
               (aj[i] < 6) &&
               (p->calculs.n_part[num2][ai[i]-6] != -1) &&
               (p->calculs.n_part[num1][aj[i]] != -1))
      {
        ai2[p->calculs.t_part_en_cours] = p->calculs.n_part[num2][ai[i]-6];
        aj2[p->calculs.t_part_en_cours] = p->calculs.n_part[num1][aj[i]];
        ax2[p->calculs.t_part_en_cours] = ax[i];
        p->calculs.t_part_en_cours++;
      }
      else if ((ai[i] >= 6) &&
               (aj[i] >= 6) &&
               (p->calculs.n_part[num2][ai[i]-6] != -1) &&
               (p->calculs.n_part[num2][aj[i]-6] != -1))
      {
        ai2[p->calculs.t_part_en_cours] = p->calculs.n_part[num2][ai[i]-6];
        aj2[p->calculs.t_part_en_cours] = p->calculs.n_part[num2][aj[i]-6];
        ax2[p->calculs.t_part_en_cours] = ax[i];
        p->calculs.t_part_en_cours++;
      }
      
      if ((ai[i] < 6) && (aj[i] < 6))
      {
        ai3[p->calculs.t_comp_en_cours] = p->calculs.n_comp[num1][ai[i]];
        aj3[p->calculs.t_comp_en_cours] = p->calculs.n_comp[num1][aj[i]];
        ax3[p->calculs.t_comp_en_cours] = ax[i];
        p->calculs.t_comp_en_cours++;
      }
      else if ((ai[i] < 6) && (aj[i] >= 6))
      {
        ai3[p->calculs.t_comp_en_cours] = p->calculs.n_comp[num1][ai[i]];
        aj3[p->calculs.t_comp_en_cours] = p->calculs.n_comp[num2][aj[i]-6];
        ax3[p->calculs.t_comp_en_cours] = ax[i];
        p->calculs.t_comp_en_cours++;
      }
      else if ((ai[i] >= 6) && (aj[i] < 6))
      {
        ai3[p->calculs.t_comp_en_cours] = p->calculs.n_comp[num2][ai[i]-6];
        aj3[p->calculs.t_comp_en_cours] = p->calculs.n_comp[num1][aj[i]];
        ax3[p->calculs.t_comp_en_cours] = ax[i];
        p->calculs.t_comp_en_cours++;
      }
      else if ((ai[i] >= 6) && (aj[i] >= 6))
      {
        ai3[p->calculs.t_comp_en_cours] = p->calculs.n_comp[num2][ai[i]-6];
        aj3[p->calculs.t_comp_en_cours] = p->calculs.n_comp[num2][aj[i]-6];
        ax3[p->calculs.t_comp_en_cours] = ax[i];
        p->calculs.t_comp_en_cours++;
      }
    }
    cholmod_free_triplet (&triplet, p->calculs.c);
  }
  // FinPour
  
  return TRUE;
}


gboolean _1992_1_1_barres_rigidite_ajout_tout(Projet *p)
/**h
 * \brief Ajoute à la matrice de rigidité toutes les barres.
 * \param p : la variable projet.
 * \return
 *   Succès : TRUE.
 *   Échec : FALSE :
 *     - p == NULL,
 *     - #_1992_1_1_barres_rigidite_ajout.
 */
{
  GList *list_parcours;
  
  BUGPARAM (p, "%p", p, FALSE)
  
  // Trivial
  if (p->modele.barres == NULL)
    return TRUE;
  
  list_parcours = p->modele.barres;
  do
  {
    EF_Barre *element = list_parcours->data;
    
    BUG (_1992_1_1_barres_rigidite_ajout (p, element), FALSE)
    
    list_parcours = g_list_next (list_parcours);
  }
  while (list_parcours != NULL);
  
  return TRUE;
}


gboolean
_1992_1_1_barres_supprime_liste (Projet *p,
                                 GList  *liste_noeuds,
                                 GList  *liste_barres)
/**
 * \brief Supprime une liste de barres.
 * \param p : la variable projet,
 * \param liste_noeuds : liste des noeuds à supprimer.
 * \param liste_barres : liste des barres à supprimer.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL,
 *     - #_1992_1_1_barres_cherche_dependances,
 *     - #EF_charge_noeud_enleve_noeuds,
 *     - #EF_charge_barre_ponctuelle_enleve_barres,
 *     - #EF_charge_barre_repartie_uniforme_enleve_barres.
 */
{
  GList *noeuds_suppr, *barres_suppr, *charges_suppr;
  GList *list_parcours;
  
  BUGPARAM (p, "%p", p, FALSE)
  
  BUG (_1992_1_1_barres_cherche_dependances (p,
                                             NULL,
                                             liste_noeuds,
                                             NULL,
                                             NULL,
                                             NULL,
                                             liste_barres,
                                             &noeuds_suppr,
                                             &barres_suppr,
                                             &charges_suppr,
                                             FALSE,
                                             TRUE),
       FALSE)
  
  // On enlève dans les charges les noeuds et barres qui seront supprimés
  list_parcours = charges_suppr;
  while (list_parcours != NULL)
  {
    Charge *charge = list_parcours->data;
    
    switch (charge->type)
    {
      case CHARGE_NOEUD :
      {
        BUG (EF_charge_noeud_enleve_noeuds (charge, noeuds_suppr, p),
             FALSE,
             g_list_free (noeuds_suppr);
               g_list_free (barres_suppr);
               g_list_free (charges_suppr);)
        break;
      }
      case CHARGE_BARRE_PONCTUELLE :
      {
        BUG (EF_charge_barre_ponctuelle_enleve_barres (charge,
                                                       barres_suppr,
                                                       p),
             FALSE,
             g_list_free (noeuds_suppr);
               g_list_free (barres_suppr);
               g_list_free (charges_suppr);)
        break;
      }
      case CHARGE_BARRE_REPARTIE_UNIFORME :
      {
        BUG (EF_charge_barre_repartie_uniforme_enleve_barres (charge,
                                                              barres_suppr,
                                                              p),
             FALSE,
             g_list_free (noeuds_suppr);
               g_list_free (barres_suppr);
               g_list_free (charges_suppr);)
        break;
      }
      default :
      {
        FAILCRIT (FALSE,
                  (gettext ("Type de charge %d inconnu.\n"), charge->type);
                    g_list_free (noeuds_suppr);
                    g_list_free (barres_suppr);
                    g_list_free (charges_suppr);)
        break;
      }
    }
    
    list_parcours = g_list_next (list_parcours);
  }
  g_list_free (noeuds_suppr);
  g_list_free (barres_suppr);
  g_list_free (charges_suppr);
  
  BUG (_1992_1_1_barres_cherche_dependances (p,
                                             NULL,
                                             liste_noeuds,
                                             NULL,
                                             NULL,
                                             NULL,
                                             liste_barres,
                                             &noeuds_suppr,
                                             &barres_suppr,
                                             NULL,
                                             TRUE,
                                             TRUE),
       FALSE)
  
  if ((noeuds_suppr != NULL) || (barres_suppr != NULL))
    BUG (EF_calculs_free (p),
         FALSE,
         g_list_free (noeuds_suppr);
           g_list_free (barres_suppr);)
  
  // On supprime les noeuds
  list_parcours = noeuds_suppr;
  while (list_parcours != NULL)
  {
    EF_Noeud *noeud = EF_noeuds_cherche_numero (p,
                                        GPOINTER_TO_UINT (list_parcours->data),
                                                FALSE);
    if (noeud != NULL)
    {
      EF_noeuds_free_foreach (noeud, p);
      p->modele.noeuds = g_list_remove (p->modele.noeuds, noeud);
    }
    list_parcours = g_list_next (list_parcours);
  }
  
  // On supprime les barres
  list_parcours = barres_suppr;
  while (list_parcours != NULL)
  {
    EF_Barre *barre = _1992_1_1_barres_cherche_numero (p,
                                        GPOINTER_TO_UINT (list_parcours->data),
                                                       FALSE);
    
    if (barre != NULL)
    {
      p->modele.barres = g_list_remove (p->modele.barres, barre);
      _1992_1_1_barres_free_foreach (barre, p);
    }
    list_parcours = g_list_next (list_parcours);
  }
  
#ifdef ENABLE_GTK
  if ((UI_APP.builder != NULL) && (noeuds_suppr != NULL))
    EF_gtk_appuis_select_changed (NULL, p);
  if ((UI_NOE.builder != NULL) && (noeuds_suppr != NULL))
    EF_noeuds_set_supprimer_visible (TRUE, p);
  if ((UI_SEC.builder != NULL) && (barres_suppr != NULL))
    EF_gtk_sections_select_changed (NULL, p);
  if ((UI_MAT.builder != NULL) && (barres_suppr != NULL))
    EF_gtk_materiaux_select_changed (NULL, p);
  if ((UI_REL.builder != NULL) && (barres_suppr != NULL))
    EF_gtk_relachements_select_changed (NULL, p);
#endif
  g_list_free (noeuds_suppr);
  g_list_free (barres_suppr);
  
  return TRUE;
}


gboolean
_1992_1_1_barres_free (Projet *p)
/**
 * \brief Libère l'ensemble des barres.
 * \param p : la variable projet.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL,
 *     - #EF_calculs_free.
 */
{
  BUGPARAM (p, "%p", p, FALSE)
  
  BUG (EF_calculs_free (p), FALSE)
  
  g_list_foreach (p->modele.barres, (GFunc) _1992_1_1_barres_free_foreach, p);
  g_list_free (p->modele.barres);
  p->modele.barres = NULL;
  
#ifdef ENABLE_GTK
  g_object_unref (UI_BAR.liste_types);
#endif
  
  return TRUE;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
