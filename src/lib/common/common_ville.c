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
#include <gmodule.h>
#include <string.h>
#include <wchar.h>


#include "common_projet.h"
#include "common_erreurs.h"
#include "common_math.h"
#include "common_text.h"
#ifdef ENABLE_GTK
#include "common_gtk.h"
#endif
#include "common_gtk_informations.h"

gboolean
common_ville_init (Projet *p)
/**
 * \brief Initialise à NULL l'adresse du projet. Met également à défaut les
 *        paramètres de neige, de vent et de sismique.
 * \param p : la variable projet.
 * \return
 *   Succès : TRUE.\n
 *   Échec : NULL :
 *     - p == NULL.
 */
{
#ifdef ENABLE_GTK
  GtkTreeIter iter;
#endif
  
  BUGPARAM (p, "%p", p, FALSE)
  
  p->parametres.adresse.departement = NULL;
  p->parametres.adresse.commune = 0;
  p->parametres.adresse.destinataire = NULL;
  p->parametres.adresse.adresse = NULL;
  p->parametres.adresse.code_postal = 0;
  p->parametres.adresse.ville = NULL;
  
  // On initialise au moins à une valeur par défaut
  p->parametres.neige = NEIGE_A1;
  p->parametres.vent = VENT_1;
  p->parametres.seisme = SEISME_1;
  
#ifdef ENABLE_GTK
  p->parametres.neige_desc = gtk_list_store_new (1, G_TYPE_STRING);
  gtk_list_store_append (p->parametres.neige_desc, &iter);
  gtk_list_store_set (p->parametres.neige_desc,
                      &iter,
                      0, gettext ("Région A1"),
                      -1);
  gtk_list_store_append (p->parametres.neige_desc, &iter);
  gtk_list_store_set (p->parametres.neige_desc,
                      &iter,
                      0, gettext ("Région A2"),
                      -1);
  gtk_list_store_append (p->parametres.neige_desc, &iter);
  gtk_list_store_set (p->parametres.neige_desc,
                      &iter,
                      0, gettext ("Région B1"),
                      -1);
  gtk_list_store_append (p->parametres.neige_desc, &iter);
  gtk_list_store_set (p->parametres.neige_desc,
                      &iter,
                      0, gettext ("Région B2"),
                      -1);
  gtk_list_store_append (p->parametres.neige_desc, &iter);
  gtk_list_store_set (p->parametres.neige_desc,
                      &iter,
                      0, gettext ("Région C1"),
                      -1);
  gtk_list_store_append (p->parametres.neige_desc, &iter);
  gtk_list_store_set (p->parametres.neige_desc,
                      &iter,
                      0, gettext ("Région C2"),
                      -1);
  gtk_list_store_append (p->parametres.neige_desc, &iter);
  gtk_list_store_set (p->parametres.neige_desc,
                      &iter,
                      0, gettext ("Région D"),
                      -1);
  gtk_list_store_append (p->parametres.neige_desc, &iter);
  gtk_list_store_set (p->parametres.neige_desc,
                      &iter,
                      0, gettext ("Région E"),
                      -1);
  
  p->parametres.vent_desc = gtk_list_store_new (1, G_TYPE_STRING);
  gtk_list_store_append (p->parametres.vent_desc, &iter);
  gtk_list_store_set (p->parametres.vent_desc,
                      &iter,
                      0, gettext ("Région 1"),
                      -1);
  gtk_list_store_append (p->parametres.vent_desc, &iter);
  gtk_list_store_set (p->parametres.vent_desc,
                      &iter,
                      0, gettext ("Région 2"),
                      -1);
  gtk_list_store_append (p->parametres.vent_desc, &iter);
  gtk_list_store_set (p->parametres.vent_desc,
                      &iter,
                      0, gettext ("Région 3"),
                      -1);
  gtk_list_store_append (p->parametres.vent_desc, &iter);
  gtk_list_store_set (p->parametres.vent_desc,
                      &iter,
                      0, gettext ("Région 4"),
                      -1);
  
  p->parametres.seisme_desc = gtk_list_store_new (1, G_TYPE_STRING);
  gtk_list_store_append (p->parametres.seisme_desc, &iter);
  gtk_list_store_set (p->parametres.seisme_desc,
                      &iter,
                      0, gettext ("Zone très faible"),
                      -1);
  gtk_list_store_append (p->parametres.seisme_desc, &iter);
  gtk_list_store_set (p->parametres.seisme_desc,
                      &iter,
                      0, gettext ("Zone faible"),
                      -1);
  gtk_list_store_append (p->parametres.seisme_desc, &iter);
  gtk_list_store_set (p->parametres.seisme_desc,
                      &iter,
                      0, gettext ("Zone modérée"),
                      -1);
  gtk_list_store_append (p->parametres.seisme_desc, &iter);
  gtk_list_store_set (p->parametres.seisme_desc,
                      &iter,
                      0, gettext ("Zone moyenne"),
                      -1);
  gtk_list_store_append (p->parametres.seisme_desc, &iter);
  gtk_list_store_set (p->parametres.seisme_desc,
                      &iter,
                      0, gettext ("Zone forte"),
                      -1);
#endif
  
  return TRUE;
}


gboolean
common_ville_get_ville (wchar_t  *ligne,
                        int      *cdc,
                        int      *cheflieu,
                        int      *reg,
                        wchar_t  *dep,
                        int      *com,
                        int      *ar,
                        int      *ct,
                        int      *tncc,
                        wchar_t **artmaj,
                        wchar_t **ncc,
                        wchar_t **artmin,
                        wchar_t **nccenr,
                        int      *code_postal,
                        int      *altitude,
                        int      *population)
/**
 * \brief Renvoie sous forme de variables la ligne de ville en cours d'analyse.
 * \param ligne : ligne en cours d'analyse,
 * \param cdc : découpage de la commune en cantons : 1 si oui, 0 si non, peut
 *              être NULL,
 * \param cheflieu : la ville est chef-lieu (d'une région : 1,
 *                   d'un département : 2, d'un arrondissement : 3 ou
 *                   d'un canton : 4), peut être NULL,
 * \param reg : la région, peut être NULL,
 * \param dep : le département, doit être utilisé avec une variable de type
 *              char departement[4], peut être NULL,
 * \param com : la commune, peut être NULL,
 * \param ar : l'arrondissement, peut être NULL,
 * \param ct : le canton, peut être NULL,
 * \param tncc : Type de nom en clair ; cette variable permet d'écrire le nom
 *               complet dans le cas d'un libellé avec article, et d'adapter la
 *               charnière si le nom est utilisé dans une expression comme "la
 *               commune de Marseille", "l'arrondissement du Mans", etc. Pour
 *               les noms de communes (et donc de canton et d'arrondissement),
 *               l'article est obligatoire ("Rochelle" n'existe pas sans
 *               article), alors que ce n'est pas le cas pour les noms de
 *               département ou de région ("Charente-Maritime" peut être écrit
 *               sans article). Pour les départements et les régions, ce code
 *               ne sert donc que pour la charnière. Peut être NULL,
 *               source :
 *    insee.fr/fr/methodes/default.asp?page=nomenclatures/cog/doc_variables.htm
 *                 - 0   pas d'article et le nom commence par une consonne sauf
 *                         H muet.  charnière = DE.
 *                 - 1   pas d'article et le nom commence par une voyelle ou un
 *                         H muet.  charnière = D'.
 *                 - 2   article = LE  charnière = DU.
 *                 - 3   article = LA  charnière = DE LA.
 *                 - 4   article = LES   charnière = DES.
 *                 - 5   article = L'  charnière = DE L'.
 *                 - 6   article = AUX   charnière = DES.
 *                 - 7   article = LAS   charnière = DE LAS.
 *                 - 8   article = LOS   charnière = DE LOS.
 * \param artmaj : l'article en majuscule, peut être NULL,
 * \param ncc : le nom de la ville en majuscule, peut être NULL,
 * \param artmin : l'article en miniscule, peut être NULL,
 * \param nccenr : la ville en majuscule, peut être NULL,
 * \param code_postal : le code postal, peut être NULL,
 * \param altitude : l'altitude, peut être NULL,
 * \param population : la population, peut être NULL.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - ligne == NULL,
 *     - La ligne est mal formée,
 *     - En cas d'erreur d'allocation mémoire.
 */
{
  int      i, j;
  int      cdc_, cheflieu_, reg_, com_, ar_, ct_, tncc_;
  int      code_postal_, altitude_, population_;
  wchar_t *dep_;
  
  BUGPARAM (ligne, "%p", ligne, FALSE)
  
  BUGCRIT (dep_ = malloc (sizeof (wchar_t) * (wcslen (ligne) + 1)),
           FALSE,
           (gettext ("Erreur d'allocation mémoire.\n"));)
  
  // On récupère les numéros caractéristant la ville en cours.
  INFO (swscanf (ligne,
                 L"%d\t%d\t%d\t%ls\t%d\t%d\t%d\t%d\t",
                 &cdc_,
                 &cheflieu_,
                 &reg_,
                 dep_,
                 &com_,
                 &ar_,
                 &ct_,
                 &tncc_) == 8,
        FALSE,
        (gettext ("La ligne en cours '%ls' n'est pas dans un format correct pour une ville.\n"),
                  ligne);
          free (dep_);)
  INFO ((0 < wcslen (dep_)) && (wcslen (dep_) <= 3),
        FALSE,
        (gettext ("La ligne en cours '%ls' n'est pas dans un format correct pour une ville.\n"),
                  ligne);
          free (dep_);)
  if (cdc != NULL)
    *cdc = cdc_;
  if (cheflieu != NULL)
    *cheflieu = cheflieu_;
  if (reg != NULL)
    *reg = reg_;
  if (dep != NULL)
    wcscpy (dep, dep_);
  free (dep_);
  if (com != NULL)
    *com = com_;
  if (ar != NULL)
    *ar = ar_;
  if (ct != NULL)
    *ct = ct_;
  if (tncc != NULL)
    *tncc = tncc_;
  
  i = 0;
  j = 0;
  // On est obligé de traiter à la main car sscanf("1\t\t3", "%s\t%s\t%s", un,
  // deux, trois); renvoie : un = 1, deux = 3 et trois non initialisé.
  while ((i != 12) && (ligne[j] != '\000'))
  {
    if (ligne[j] == '\t')
    {
      ligne[j] = '\000';
      i++;
    }
    j++;
    if (i == 7)
    {
      if (artmaj != NULL)
        *artmaj = &(ligne[j + 1]);
    }
    else if (i == 8)
    {
      if (ncc != NULL)
        *ncc = &(ligne[j + 1]);
    }
    else if (i == 9)
    {
      if (artmin != NULL)
        *artmin = &(ligne[j + 1]);
    }
    else if (i == 10)
    {
      if (nccenr != NULL)
        *nccenr = &(ligne[j + 1]);
    }
  }
  
  INFO (ligne[j] != '\000',
        FALSE,
        (gettext ("La ligne en cours '%ls' n'est pas dans un format correct pour une ville.\n"),
                  ligne);)
  INFO (swscanf (&(ligne[j]),
                 L"%d\t%d\t%d\n",
                 &code_postal_,
                 &altitude_,
                 &population_) == 3,
        FALSE,
        (gettext ("La ligne en cours '%ls' n'est pas dans un format correct pour une ville.\n"),
                  ligne);)
  
  if (code_postal != NULL)
    *code_postal = code_postal_;
  if (altitude != NULL)
    *altitude = altitude_;
  if (population != NULL)
    *population = population_;
  
  return TRUE;
}


gboolean
common_ville_set (Projet  *p,
                  wchar_t *departement,
                  wchar_t *ville,
                  gboolean graphique_seul)
/**
 * \brief Initialise la ville (mais pas l'adresse exacte) du projet avec les
 *        paramètres régionaux (vent, neige, séisme). Departement DEP et ville
 *        COM doivent correspondre au fichier france_villes.csv.
 * \param p : la variable projet,
 * \param departement : le numéro du département (char* car 2A et 2B), doit
 *                      être utilisé avec une variable de type char
 *                      departement[4],
 * \param ville : le nom de la ville sous la forme :
 *                g_strdup_printf("%s%s%s",
 *                                artmin,
 *                       ((article == 5) || (article == 1) || (article == 0)) ?
 *                                "" : " ",
 *                                nccenr) pour la France,
 * \param graphique_seul : TRUE si on ne modifie que l'interface graphique et
 *                         pas la variable projet.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL,
 *     - departement == NULL,
 *     - ville == NULL,
 *     - fichier france_villes.csv introuvable,
 *     - en cas d'erreur d'allocation mémoire.
 */
{
  FILE       *villes;
  wchar_t    *ligne = NULL;
  int         com, ct, code_postal, population, article;
  wchar_t    *artmin, *nccenr;
  wchar_t     dep[4];
  Type_Neige  neige_tmp;
  Type_Vent   vent_tmp;
  Type_Seisme seisme_tmp;
  wchar_t    *tmp;
  
  BUGPARAM (p, "%p", p, FALSE)
  BUGPARAM (departement, "%p", departement, FALSE)
  BUGPARAM (ville, "%p", ville, FALSE)
  
  INFO (villes = fopen (DATADIR"/france_villes.csv", "r"),
        FALSE,
        (gettext ("Le fichier '%s' est introuvable.\n"),
                  DATADIR"/france_villes.csv");)
  
  // On passe la première ligne qui est l'étiquette des colonnes.
  BUG (ligne = common_text_get_line (villes), FALSE, fclose (villes);)
  free (ligne);
  
  BUG (ligne = common_text_get_line (villes), FALSE, fclose (villes);)
  do
  {
    BUG (common_ville_get_ville (ligne,
                                 NULL,
                                 NULL,
                                 NULL,
                                 dep,
                                 &com,
                                 NULL,
                                 &ct,
                                 &article,
                                 NULL,
                                 NULL,
                                 &artmin,
                                 &nccenr,
                                 &code_postal,
                                 NULL,
                                 &population),
         FALSE,
         free (ligne);
           fclose (villes);)
    BUGCRIT (tmp = malloc (sizeof (wchar_t) *
                                      (wcslen (artmin) + wcslen (nccenr) + 2)),
             FALSE,
             (gettext ("Erreur d'allocation mémoire.\n"));
               free (ligne);
               fclose (villes);)
    wcscpy (tmp, artmin);
    wcscat (tmp,
            ((article == 5) || (article == 1) || (article == 0)) ? L"" : L" ");
    wcscat (tmp, nccenr);
    // On récupère les numéros caractéristant la ville en cours.
    if ((wcscmp (dep, departement) == 0) && (wcscmp (tmp, ville) == 0))
    {
      wchar_t *dep_parcours;
      wchar_t *champ1, *champ2;
      gboolean done;
      
      // Maintenant que tout est récupéré, on enregistre dans le projet ce
      // qu'il faut.
      // Ville et code postal.
      if (!graphique_seul)
      {
        free (p->parametres.adresse.departement);
        BUG (p->parametres.adresse.departement =
                                        common_text_wcstostr_dup (departement),
             FALSE,
             free (ligne);
               fclose (villes);)
        p->parametres.adresse.commune = com;
        p->parametres.adresse.code_postal = code_postal;
        free (p->parametres.adresse.ville);
        BUG (p->parametres.adresse.ville = common_text_wcstostr_dup (tmp),
             FALSE,
             free (ligne);
               fclose (villes);)
      }
      
      free (ligne);
      fclose (villes);
      
      // On actualise la fenêtre graphique
#ifdef ENABLE_GTK
      if (UI_INFO.builder != NULL)
      {
        char *code_postal2;
        
        g_signal_handler_block (
          gtk_builder_get_object (UI_INFO.builder,
                                     "common_informations_buffer_code_postal"),
          g_signal_handler_find (gtk_builder_get_object (
                   UI_INFO.builder, "common_informations_buffer_code_postal"),
                                 G_SIGNAL_MATCH_FUNC,
                                 0,
                                 0,
                                 NULL,
                                 common_gtk_informations_entry_add_char,
                                 NULL));
        g_signal_handler_block (
          gtk_builder_get_object (UI_INFO.builder,
                                     "common_informations_buffer_code_postal"),
          g_signal_handler_find (gtk_builder_get_object (
                   UI_INFO.builder, "common_informations_buffer_code_postal"),
                                 G_SIGNAL_MATCH_FUNC,
                                 0,
                                 0,
                                 NULL,
                                 common_gtk_informations_entry_del_char,
                                 NULL));
        g_signal_handler_block (
          gtk_builder_get_object (UI_INFO.builder,
                                           "common_informations_buffer_ville"),
          g_signal_handler_find (gtk_builder_get_object (UI_INFO.builder,
                                           "common_informations_buffer_ville"),
                                 G_SIGNAL_MATCH_FUNC,
                                 0,
                                 0,
                                 NULL,
                                 common_gtk_informations_entry_add_char,
                                 NULL));
        g_signal_handler_block (
          gtk_builder_get_object (UI_INFO.builder,
                                           "common_informations_buffer_ville"),
          g_signal_handler_find (gtk_builder_get_object (UI_INFO.builder,
                                           "common_informations_buffer_ville"),
                                 G_SIGNAL_MATCH_FUNC,
                                 0,
                                 0,
                                 NULL,
                                 common_gtk_informations_entry_del_char,
                                 NULL));
        
        BUGCRIT (code_postal2 = g_strdup_printf ("%d", code_postal),
                 FALSE,
                 (gettext ("Erreur d'allocation mémoire.\n"));)
        gtk_entry_set_text (GTK_ENTRY (gtk_builder_get_object (UI_INFO.builder,
                                     "common_informations_entry_code_postal")),
                            code_postal2);
        free (code_postal2);
        BUG (code_postal2 = common_text_wcstostr_dup (tmp), FALSE)
        gtk_entry_set_text (GTK_ENTRY (gtk_builder_get_object (UI_INFO.builder,
                                           "common_informations_entry_ville")),
                            code_postal2);
        free (code_postal2);
        
        g_signal_handler_unblock (
          gtk_builder_get_object (UI_INFO.builder,
                                     "common_informations_buffer_code_postal"),
          g_signal_handler_find (gtk_builder_get_object (UI_INFO.builder,
                                     "common_informations_buffer_code_postal"),
                                 G_SIGNAL_MATCH_FUNC,
                                 0,
                                 0,
                                 NULL,
                                 common_gtk_informations_entry_add_char,
                                 NULL));
        g_signal_handler_unblock (
          gtk_builder_get_object (UI_INFO.builder,
                                     "common_informations_buffer_code_postal"),
          g_signal_handler_find (gtk_builder_get_object (UI_INFO.builder,
                                     "common_informations_buffer_code_postal"),
                                 G_SIGNAL_MATCH_FUNC,
                                 0,
                                 0,
                                 NULL,
                                 common_gtk_informations_entry_del_char,
                                 NULL));
        g_signal_handler_unblock (
          gtk_builder_get_object (UI_INFO.builder,
                                           "common_informations_buffer_ville"),
          g_signal_handler_find (gtk_builder_get_object (UI_INFO.builder,
                                           "common_informations_buffer_ville"),
                                 G_SIGNAL_MATCH_FUNC,
                                 0,
                                 0,
                                 NULL,
                                 common_gtk_informations_entry_add_char,
                                 NULL));
        g_signal_handler_unblock (
          gtk_builder_get_object (UI_INFO.builder,
                                           "common_informations_buffer_ville"),
          g_signal_handler_find (gtk_builder_get_object (UI_INFO.builder,
                                           "common_informations_buffer_ville"),
                                 G_SIGNAL_MATCH_FUNC,
                                 0,
                                 0,
                                 NULL,
                                 common_gtk_informations_entry_del_char,
                                 NULL));
      }
#endif
      if (graphique_seul)
        free (tmp);
      
      // Le zonage neige.
      INFO (villes = fopen (DATADIR"/france_neige.csv", "r"),
            FALSE,
            (gettext ("Le fichier '%s' est introuvable.\n"),
                      DATADIR"/france_neige.csv");)
      // On commence par chercher le département et on applique la règle de
      // base.
      dep_parcours = NULL;
      while (dep_parcours == NULL)
      {
        INFO (ligne = common_text_get_line (villes),
              FALSE,
              (gettext ("Le fichier '%s' est incomplet.\n"),
                        DATADIR"/france_neige.csv");
                fclose (villes);)
        
        BUGCRIT (champ1 = malloc (sizeof (wchar_t) * (wcslen (ligne) + 1)),
                 FALSE,
                 (gettext ("Erreur d'allocation mémoire.\n"));
                   fclose (villes);
                   free (ligne);)
        BUGCRIT (champ2 = malloc (sizeof (wchar_t) * (wcslen (ligne) + 1)),
                 FALSE,
                 (gettext ("Erreur d'allocation mémoire.\n"));
                   fclose (villes);
                   free (ligne);
                   free (champ1);)
        BUGCRIT (dep_parcours = malloc (sizeof (wchar_t) *
                                                         (wcslen (ligne) + 1)),
                 FALSE,
                 (gettext ("Erreur d'allocation mémoire.\n"));
                   fclose (villes);
                   free (ligne);
                   free (champ1);
                   free (champ2);)
        
        INFO (swscanf (ligne,
                       L"%ls\t%ls\t%ls\n",
                       champ1,
                       dep_parcours,
                       champ2) == 3,
              FALSE,
              (gettext ("Le fichier '%s' est corrompu.\n"),
                        DATADIR"/france_neige.csv");
                fclose (villes);
                free (ligne);
                free (champ1);
                free (champ2);
                free (dep_parcours);)
        
        if ((wcscmp (dep_parcours, dep) != 0) ||
            (wcscmp (champ1, L"DEP") != 0))
        {
          free (dep_parcours);
          dep_parcours = NULL;
          free (champ2);
        }
        else
          free (dep_parcours);
        
        free (ligne);
        free (champ1);
      }
      
      if (wcscmp (champ2, L"A1") == 0)
        neige_tmp = NEIGE_A1;
      else if (wcscmp (champ2, L"A2") == 0)
        neige_tmp = NEIGE_A2;
      else if (wcscmp (champ2, L"B1") == 0)
        neige_tmp = NEIGE_B1;
      else if (wcscmp (champ2, L"B2") == 0)
        neige_tmp = NEIGE_B2;
      else if (wcscmp (champ2, L"C1") == 0)
        neige_tmp = NEIGE_C1;
      else if (wcscmp (champ2, L"C2") == 0)
        neige_tmp = NEIGE_C2;
      else if (wcscmp (champ2, L"D") == 0)
        neige_tmp = NEIGE_D;
      else if (wcscmp (champ2, L"E") == 0)
        neige_tmp = NEIGE_E;
      else
        FAILINFO (FALSE,
                  (gettext ("Le fichier '%s' est corrumpu. Le champ2 '%ls' est inconnu.\n"),
                            DATADIR"/france_neige.csv",
                            champ2);
                    free (champ2);
                    fclose (villes);)
      free (champ2);
      
      // On recherche si il y a une exception à la règle de base.
      done = FALSE;
      while (done == FALSE)
      {
        ligne = common_text_get_line (villes);
        // On a atteint la fin du fichier
        if (ligne == NULL)
          break;
        
        BUGCRIT (champ1 = malloc (sizeof (wchar_t) * (wcslen (ligne) + 1)),
                 FALSE,
                 (gettext ("Erreur d'allocation mémoire.\n"));
                   fclose (villes);
                   free (ligne);)
        BUGCRIT (champ2 = malloc (sizeof (wchar_t) * (wcslen (ligne) + 1)),
                 FALSE,
                 (gettext ("Erreur d'allocation mémoire.\n"));
                   fclose (villes);
                   free (ligne);
                   free (champ1);)
        
        INFO (swscanf (ligne, L"%ls", champ1) == 1,
              FALSE,
              (gettext ("Le fichier '%s' est corrompu.\n"),
                        DATADIR"/france_neige.csv");
                fclose (villes);
                free (ligne);
                free (champ1);
                free (champ2);)
        
        // Si on arrive au département suivant sans avoir trouvé d'exception.
        if (wcscmp (champ1, L"DEP") == 0)
          done = TRUE;
        else
        {
          int numero;
          
          INFO (swscanf (ligne,
                         L"%ls\t%d\t%ls\n",
                         champ1,
                         &numero,
                         champ2) == 3,
                FALSE,
                (gettext ("Le fichier '%s' est corrompu.\n"),
                          DATADIR"/france_neige.csv");
                  fclose (villes);
                  free (ligne);
                  free (champ1);
                  free (champ2);)
          if (((wcscmp (champ1, L"CAN") == 0) && (numero == ct)) ||
              ((wcscmp (champ1, L"COM") == 0) && (numero == com)))
          {
            if (wcscmp (champ2, L"A1") == 0)
              neige_tmp = NEIGE_A1;
            else if (wcscmp (champ2, L"A2") == 0)
              neige_tmp = NEIGE_A2;
            else if (wcscmp (champ2, L"B1") == 0)
              neige_tmp = NEIGE_B1;
            else if (wcscmp (champ2, L"B2") == 0)
              neige_tmp = NEIGE_B2;
            else if (wcscmp (champ2, L"C1") == 0)
              neige_tmp = NEIGE_C1;
            else if (wcscmp (champ2, L"C2") == 0)
              neige_tmp = NEIGE_C2;
            else if (wcscmp (champ2, L"D") == 0)
              neige_tmp = NEIGE_D;
            else if (wcscmp (champ2, L"E") == 0)
              neige_tmp = NEIGE_E;
            else
              FAILINFO (FALSE,
                        (gettext ("Le fichier '%s' est corrumpu. Le champ2 '%ls' est inconnu.\n"),
                                  DATADIR"/france_neige.csv",
                                  champ2);
                          fclose (villes);
                          free (ligne);
                          free (champ1);
                          free (champ2);)
            done = TRUE;
          }
          else
            done = FALSE;
        }
        
        free (ligne);
        free (champ1);
        free (champ2);
      }
      fclose (villes);
      
      if (!graphique_seul)
        p->parametres.neige = neige_tmp;
#ifdef ENABLE_GTK
      if (UI_INFO.builder != NULL)
        gtk_combo_box_set_active (GTK_COMBO_BOX (gtk_builder_get_object (
                       UI_INFO.builder, "common_informations_neige_combobox")),
                                  neige_tmp);
#endif
      // Fin Neige
      
      // Le zonage vent.
      INFO (villes = fopen (DATADIR"/france_vent.csv", "r"),
            FALSE,
            (gettext ("Le fichier '%s' est introuvable.\n"),
                      DATADIR"/france_vent.csv");)
      // On commence par chercher le département et on applique la règle de
      // base.
      dep_parcours = NULL;
      while (dep_parcours == NULL)
      {
        INFO (ligne = common_text_get_line (villes),
              FALSE,
              (gettext ("Le fichier '%s' est incomplet.\n"),
                        DATADIR"/france_vent.csv");
                fclose (villes);)
        
        BUGCRIT (champ1 = malloc (sizeof (wchar_t) * (wcslen (ligne) + 1)),
                 FALSE,
                 (gettext ("Erreur d'allocation mémoire.\n"));
                   fclose (villes);
                   free (ligne);)
        BUGCRIT (champ2 = malloc (sizeof (wchar_t) * (wcslen (ligne) + 1)),
                 FALSE,
                 (gettext ("Erreur d'allocation mémoire.\n"));
                   free (champ1);
                   free (ligne);
                   fclose (villes);)
        BUGCRIT (dep_parcours = malloc (sizeof (wchar_t) *
                                                         (wcslen (ligne) + 1)),
                 FALSE,
                 (gettext ("Erreur d'allocation mémoire.\n"));
                   free (champ2);
                   free (champ1);
                   free (ligne);
                   fclose (villes);)
        
        INFO (swscanf (ligne,
                       L"%ls\t%ls\t%ls\n",
                       champ1,
                       dep_parcours,
                       champ2) == 3,
              FALSE,
              (gettext ("Le fichier '%s' est corrompu.\n"),
                        DATADIR"/france_vent.csv");
                free (dep_parcours);
                free (champ2);
                free (champ1);
                free (ligne);
                fclose (villes);)
        
        if ((wcscmp (dep_parcours, dep) != 0) ||
            (wcscmp (champ1, L"DEP") != 0))
        {
          free (dep_parcours);
          dep_parcours = NULL;
          free (champ2);
        }
        else
          free (dep_parcours);
        
        free (ligne);
        free (champ1);
      }
      if (wcscmp (champ2, L"1") == 0)
        vent_tmp = VENT_1;
      else if (wcscmp (champ2, L"2") == 0)
        vent_tmp = VENT_2;
      else if (wcscmp (champ2, L"3") == 0)
        vent_tmp = VENT_3;
      else if (wcscmp (champ2, L"4") == 0)
        vent_tmp = VENT_4;
      else
        FAILINFO (FALSE,
                  (gettext ("Le fichier '%s' est corrumpu. Le champ2 '%ls' est inconnu.\n"),
                            DATADIR"/france_vent.csv",
                            champ2);
                    free (champ2);
                    fclose (villes);)
      free (champ2);
      // On recherche si il y a une exception à la règle de base.
      done = FALSE;
      while (done == FALSE)
      {
        ligne = common_text_get_line (villes);
        // On a atteint la fin du fichier
        if (ligne == NULL)
          break;
        
        BUGCRIT (champ1 = malloc (sizeof (wchar_t) * (wcslen (ligne) + 1)),
                 FALSE,
                 (gettext ("Erreur d'allocation mémoire.\n"));
                   free (ligne);
                   fclose (villes);)
        BUGCRIT (champ2 = malloc (sizeof (wchar_t) * (wcslen (ligne) + 1)),
                 FALSE,
                 (gettext ("Erreur d'allocation mémoire.\n"));
                   free (champ1);
                   free (ligne);
                   fclose (villes);)
        
        INFO (swscanf (ligne, L"%ls", champ1) == 1,
              FALSE,
              (gettext ("Le fichier '%s' est corrompu.\n"),
                        DATADIR"/france_vent.csv");
                free (champ2);
                free (champ1);
                free (ligne);
                fclose (villes);)
        
        // Si on arrive au département suivant sans avoir trouvé d'exception.
        if (wcscmp (champ1, L"DEP") == 0)
          done = TRUE;
        else
        {
          int numero;
          
          INFO (swscanf (ligne,
                         L"%ls\t%d\t%ls\n",
                         champ1,
                         &numero,
                         champ2) == 3,
                FALSE,
                (gettext ("Le fichier '%s' est corrompu.\n"),
                          DATADIR"/france_vent.csv");
                  free (champ2);
                  free (champ1);
                  free (ligne);
                  fclose (villes);)
          if (((wcscmp (champ1, L"CAN") == 0) && (numero == ct)) ||
              ((wcscmp (champ1, L"COM") == 0) && (numero == com)))
          {
            if (wcscmp (champ2, L"1") == 0)
              vent_tmp = VENT_1;
            else if (wcscmp (champ2, L"2") == 0)
              vent_tmp = VENT_2;
            else if (wcscmp (champ2, L"3") == 0)
              vent_tmp = VENT_3;
            else if (wcscmp (champ2, L"4") == 0)
              vent_tmp = VENT_4;
            else
              FAILINFO (FALSE,
                        (gettext ("Le fichier '%s' est corrumpu. Le champ2 '%ls' est inconnu.\n"),
                                  DATADIR"/france_vent.csv",
                                  champ2);
                          free (champ2);
                          free (champ1);
                          free (ligne);
                          fclose (villes);)
            done = TRUE;
          }
          else
            done = FALSE;
        }
        
        free (ligne);
        free (champ1);
        free (champ2);
      }
      fclose (villes);
      
      if (!graphique_seul)
        p->parametres.vent = vent_tmp;
#ifdef ENABLE_GTK
      if (UI_INFO.builder != NULL)
        gtk_combo_box_set_active (GTK_COMBO_BOX (gtk_builder_get_object (
                        UI_INFO.builder, "common_informations_vent_combobox")),
                                  vent_tmp);
#endif
      // Fin Vent
      
      // Le zonage sismique.
      INFO (villes = fopen (DATADIR"/france_seisme.csv", "r"),
            FALSE,
            (gettext ("Le fichier '%s' est introuvable.\n"),
                      DATADIR"/france_seisme.csv");)
      // On commence par chercher le département et on applique la règle de
      // base.
      dep_parcours = NULL;
      while (dep_parcours == NULL)
      {
        INFO (ligne = common_text_get_line (villes),
              FALSE,
              (gettext ("Le fichier '%s' est incomplet.\n"),
                        DATADIR"/france_seisme.csv");
                fclose (villes);)
        
        BUGCRIT (champ1 = malloc (sizeof (wchar_t) * (wcslen (ligne) + 1)),
                 FALSE,
                 (gettext ("Erreur d'allocation mémoire.\n"));
                    free (ligne);
                    fclose (villes);)
        BUGCRIT (champ2 = malloc (sizeof (wchar_t) * (wcslen (ligne) + 1)),
                 FALSE,
                 (gettext ("Erreur d'allocation mémoire.\n"));
                   free (champ1);
                   free (ligne);
                   fclose (villes);)
        BUGCRIT (dep_parcours = malloc (sizeof (wchar_t) *
                                                         (wcslen (ligne) + 1)),
                 FALSE,
                 (gettext ("Erreur d'allocation mémoire.\n"));
                   free (champ2);
                   free (champ1);
                   free (ligne);
                   fclose (villes);)
        
        INFO (swscanf (ligne,
                       L"%ls\t%ls\t%ls\n",
                       champ1,
                       dep_parcours,
                       champ2) == 3,
              FALSE,
              (gettext ("Le fichier '%s' est corrompu.\n"),
                        DATADIR"/france_seisme.csv");
                free (dep_parcours);
                free (champ2);
                free (champ1);
                free (ligne);
                fclose (villes);)
        
        if ((wcscmp (dep_parcours, dep) != 0) ||
            (wcscmp (champ1, L"DEP") != 0))
        {
          free (dep_parcours);
          dep_parcours = NULL;
          free (champ2);
        }
        else
          free (dep_parcours);
        
        free (ligne);
        free (champ1);
      }
      if (wcscmp (champ2, L"1") == 0)
        seisme_tmp = SEISME_1;
      else if (wcscmp (champ2, L"2") == 0)
        seisme_tmp = SEISME_2;
      else if (wcscmp (champ2, L"3") == 0)
        seisme_tmp = SEISME_3;
      else if (wcscmp (champ2, L"4") == 0)
        seisme_tmp = SEISME_4;
      else if (wcscmp (champ2, L"5") == 0)
        seisme_tmp = SEISME_5;
      else
        FAILINFO (FALSE,
                  (gettext ("Le fichier '%s' est corrumpu. Le champ2 '%ls' est inconnu.\n"),
                            DATADIR"/france_seisme.csv",
                            champ2);
                    free (champ2);
                    fclose (villes);)
      free (champ2);
      // On recherche si il y a une exception à la règle de base.
      done = FALSE;
      while (done == FALSE)
      {
        ligne = common_text_get_line (villes);
        // On a atteint la fin du fichier
        if (ligne == NULL)
          break;
        
        BUGCRIT (champ1 = malloc (sizeof (wchar_t) * (wcslen (ligne) + 1)),
                 FALSE,
                 (gettext ("Erreur d'allocation mémoire.\n"));
                   free (ligne);
                   fclose (villes);)
        BUGCRIT (champ2 = malloc (sizeof (wchar_t) * (wcslen (ligne) + 1)),
                 FALSE,
                 (gettext ("Erreur d'allocation mémoire.\n"));
                   free (champ1);
                   free (ligne);
                   fclose (villes);)
        
        INFO (swscanf (ligne, L"%ls", champ1) == 1,
              FALSE,
              (gettext ("Le fichier '%s' est corrompu.\n"),
                        DATADIR"/france_seisme.csv");
                free (champ2);
                free (champ1);
                free (ligne);
                fclose (villes);)
        
        // Si on arrive au département suivant sans avoir trouvé d'exception.
        if (wcscmp (champ1, L"DEP") == 0)
          done = TRUE;
        else
        {
          int numero;
          
          INFO (swscanf (ligne,
                         L"%ls\t%d\t%ls\n",
                         champ1,
                         &numero,
                         champ2) == 3,
                FALSE,
                (gettext ("Le fichier '%s' est corrompu.\n"),
                          DATADIR"/france_seisme.csv");
                  free (champ2);
                  free (champ1);
                  free (ligne);
                  fclose (villes);)
          if (((wcscmp (champ1, L"CAN") == 0) && (numero == ct)) ||
              ((wcscmp (champ1, L"COM") == 0) && (numero == com)))
          {
            if (wcscmp (champ2, L"1") == 0)
              seisme_tmp = SEISME_1;
            else if (wcscmp (champ2, L"2") == 0)
              seisme_tmp = SEISME_2;
            else if (wcscmp (champ2, L"3") == 0)
              seisme_tmp = SEISME_3;
            else if (wcscmp (champ2, L"4") == 0)
              seisme_tmp = SEISME_4;
            else if (wcscmp (champ2, L"5") == 0)
              seisme_tmp = SEISME_5;
            else
              FAILINFO (FALSE,
                        (gettext ("Le fichier '%s' est corrumpu. Le champ2 '%ls' est inconnu.\n"),
                                  DATADIR"/france_seisme.csv",
                                  champ2);
                          free (champ2);
                          free (champ1);
                          free (ligne);
                          fclose (villes);)
            done = TRUE;
          }
          else
            done = FALSE;
        }
        
        free (ligne);
        free (champ1);
        free (champ2);
      }
      fclose (villes);
      
      if (!graphique_seul)
        p->parametres.seisme = seisme_tmp;
#ifdef ENABLE_GTK
      if (UI_INFO.builder != NULL)
        gtk_combo_box_set_active (GTK_COMBO_BOX (gtk_builder_get_object (
                      UI_INFO.builder, "common_informations_seisme_combobox")),
                                  seisme_tmp);
#endif
      // Fin Sismique
      
      return TRUE;
    }
    else
      free (tmp);
    free (ligne);
    ligne = common_text_get_line (villes);
  } while (ligne != NULL);
  
  fclose (villes);
  
  return FALSE;
}


gboolean
common_ville_free (Projet *p)
/**
 * \brief Libère les allocations mémoires pour la gestion des villes.
 * \param p : la variable projet.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL.
 */
{
  BUGPARAM (p, "%p", p, FALSE)
  
  free (p->parametres.adresse.departement);
  p->parametres.adresse.departement = NULL;
  free (p->parametres.adresse.destinataire);
  p->parametres.adresse.destinataire = NULL;
  free (p->parametres.adresse.adresse);
  p->parametres.adresse.adresse = NULL;
  free (p->parametres.adresse.ville);
  p->parametres.adresse.ville = NULL;
  
  return TRUE;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
