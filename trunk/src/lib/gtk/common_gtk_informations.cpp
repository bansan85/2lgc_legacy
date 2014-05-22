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

#include <gtk/gtk.h>

#include "common_projet.hpp"
#include "common_erreurs.hpp"
#include "common_gtk.hpp"
#include "common_math.hpp"
#include "common_text.hpp"
#include "common_ville.hpp"
#include "common_gtk_informations.hpp"


GTK_WINDOW_KEY_PRESS (common, informations);


GTK_WINDOW_DESTROY (common, informations, free (UI_INFO.departement); );


GTK_WINDOW_CLOSE (common, informations);


/**
 * \brief Récupère toutes les données de la fenêtre permettant d'éditer
 *        l'adresse du projet.
 * \param p : la variable projet,
 * \param destinataire : le nom du destinataire,
 * \param adresse : la rue du projet,
 * \param code_postal : le code postal du projet,
 * \param ville : la ville du projet.
 * \return
 *   Succès : true.\n
 *   Échec : false :
 *     - p == NULL,
 *     - destinataire == NULL,
 *     - adresse == NULL,
 *     - code_postal == NULL,
 *     - ville == NULL,
 *     - interface graphique non initialisée,
 *     - en cas d'erreur d'allocation mémoire.
 */
bool
common_gtk_informations_recupere_donnees (Projet      *p,
                                          std::string *destinataire,
                                          std::string *adresse,
                                          uint32_t    *code_postal,
                                          std::string *ville)
{
  bool           ok = true;
  GtkTextIter    start, end;
  GtkTextBuffer *textbuffer;
  char          *tmp;
  
  BUGPARAM (p, "%p", p, false)
  BUGPARAM (destinataire, "%p", destinataire, false)
  BUGPARAM (adresse, "%p", adresse, false)
  BUGPARAM (code_postal, "%p", code_postal, false)
  BUGPARAM (ville, "%p", ville, false)
  BUGCRIT (UI_INFO.builder,
           false,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Informations"); )
  
  *code_postal = common_gtk_entry_uint (GTK_ENTRY (gtk_builder_get_object (
                    UI_INFO.builder, "common_informations_entry_code_postal")),
                                        0, true,
                                        UINT_MAX, false);
  if (*code_postal == UINT_MAX)
  {
    ok = false;
  }
  
  textbuffer = GTK_TEXT_BUFFER (gtk_builder_get_object (UI_INFO.builder,
                                   "common_informations_buffer_destinataire"));
  gtk_text_buffer_get_iter_at_offset (textbuffer, &start, 0);
  gtk_text_buffer_get_iter_at_offset (textbuffer, &end, -1);
  tmp = gtk_text_buffer_get_text (textbuffer, &start, &end, FALSE);
  destinataire->assign (tmp);
  free (tmp);
  
  textbuffer = GTK_TEXT_BUFFER (gtk_builder_get_object (UI_INFO.builder,
                                        "common_informations_buffer_adresse"));
  gtk_text_buffer_get_iter_at_offset (textbuffer, &start, 0);
  gtk_text_buffer_get_iter_at_offset (textbuffer, &end, -1);
  tmp = gtk_text_buffer_get_text (textbuffer, &start, &end, FALSE);
  adresse->assign (tmp);
  free (tmp);
  
  ville->assign (gtk_entry_buffer_get_text (GTK_ENTRY_BUFFER (
                                       gtk_builder_get_object (UI_INFO.builder,
                                        "common_informations_buffer_ville"))));
  
  return ok;
}


/**
 * \brief Vérifie si l'ensemble des éléments est correct pour activer le bouton
 *        add/edit.
 * \param entrybuffer : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.
 *   Échec :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
extern "C"
void
common_gtk_informations_check (GtkEntryBuffer *entrybuffer,
                               Projet         *p)
{
  uint32_t    code_postal;
  std::string destinataire, adresse, ville;
  
  BUGPARAM (p, "%p", p, )
  BUGCRIT (UI_INFO.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Informations"); )
  
  if (!common_gtk_informations_recupere_donnees (p,
                                                 &destinataire,
                                                 &adresse,
                                                 &code_postal,
                                                 &ville))
  {
    gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
                          UI_INFO.builder, "common_informations_button_edit")),
                              FALSE);
  }
  else
  {
    gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
                          UI_INFO.builder, "common_informations_button_edit")),
                              TRUE);
  }
  
  return;
}


/**
 * \brief Fonction utilisée par la fonction g_list_insert_sorted pour trier
 *        une liste d'adresse en fonction de leur population.
 * \param a : adresse 1,
 * \param b : adresse 2.
 * \return 1 si a->population > b->population, -1 sinon.
 *   Échec : 0 :
 *     - a == NULL,
 *     - b == NULL.
 */
gint
common_gtk_informations_compare_adresse (Ligne_Adresse *a,
                                         Ligne_Adresse *b)
{
  BUGPARAM (a, "%p", a, 0)
  BUGPARAM (b, "%p", b, 0)
  
  if (a->population > b->population)
  {
    return 1;
  }
  else if (a->population < b->population)
  {
    return -1;
  }
  else
  {
    return 0;
  }
}


/**
 * \brief Libère une adresse. Peut être utilisé avec g_list_free_full.
 * \param data : adresse à libérer.
 * \return Rien.
 */
void
common_gtk_informations_free_adresse (void *data)
{
  Ligne_Adresse *adresse = (Ligne_Adresse *) data;
  
  BUGPARAM (adresse, "%p", adresse, )
  
  delete adresse;
}


/**
 * \brief Fonction permettant d'afficher en temps réel les propositions des
 *        villes en fonction des touches tapées dans le code postal ou la
 *        ville.
 * \param buffer : buffer en cours d'édition,
 * \param position : la position du cluster,
 * \param n_chars : nombre de caractère supprimé,
 * \param p : la variable projet.
 * \return Rien.
 *   Échec :
 *     - buffer == NULL,
 *     - p == NULL,
 *     - buffer est soit code postal ou ville,
 *     - fichier france_villes.csv introuvable ou malformé,
 *     - interface graphique non initialisée.
 *     - erreur d'allocation mémoire.
 */
extern "C"
void
common_gtk_informations_entry_del_char (GtkEntryBuffer *buffer,
                                        guint           position,
                                        guint           n_chars,
                                        Projet         *p)
{
  #define     MAX_VILLES  10
  FILE       *villes;
  wchar_t    *ligne = NULL;
  GtkTreeIter iter;
  uint8_t     i;
  const char *code_postal_tmp, *ville_tmp;
  wchar_t    *code_postal, *ville;
  GList      *list = NULL, *list_parcours;
  bool        f_codepostal; // true si cherche sur la base du code postal.
  uint32_t    popmin; // La population minimale que doit avoir la ville pour
              // être autorisée à entrer dans la liste des propositions.
  
  BUGPARAM (buffer, "%p", buffer, )
  BUGPARAM (p, "%p", p, )
  BUGCRIT (UI_INFO.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Informations"); )
  
  gtk_list_store_clear (UI_INFO.model_completion);
  
  if (gtk_entry_get_buffer (GTK_ENTRY (gtk_builder_get_object (UI_INFO.builder,
                          "common_informations_entry_code_postal"))) == buffer)
  {
    f_codepostal = true;
  }
  else if (gtk_entry_get_buffer (GTK_ENTRY (gtk_builder_get_object (
               UI_INFO.builder, "common_informations_entry_ville"))) == buffer)
  {
    f_codepostal = false;
  }
  else
  {
    FAILPARAM (buffer, "%p", )
  }
  
  code_postal_tmp = gtk_entry_get_text (GTK_ENTRY (gtk_builder_get_object (
                   UI_INFO.builder, "common_informations_entry_code_postal")));
  ville_tmp = gtk_entry_get_text (GTK_ENTRY (gtk_builder_get_object (
                         UI_INFO.builder, "common_informations_entry_ville")));
  BUG (code_postal = common_text_strtowcs_dup (code_postal_tmp), )
  BUG (ville = common_text_strtowcs_dup (ville_tmp), )
  
  if (((f_codepostal) && (wcscmp (code_postal, L"") == 0)) ||
      ((!f_codepostal) && (wcscmp (ville, L"") == 0)))
  {
    common_gtk_informations_check (NULL, p);
    return;
  }
  
  INFO (villes = fopen (DATADIR"/france_villes.csv", "r"),
        ,
        (gettext ("Le fichier '%s' est introuvable.\n"),
                  DATADIR"/france_villes.csv");
          free (ville);
          free (code_postal); )
  
  ligne = common_text_get_line (villes);
  free (ligne);
  
  i = 0;
  popmin = 0;
  ligne = common_text_get_line (villes);
  while (ligne != NULL)
  {
    wchar_t *artmaj, *ncc, *artmin, *nccenr;
    uint16_t article;
    uint32_t codepostal, population;
    wchar_t *minuscule;
    wchar_t  code_postal2[11];
    wchar_t  departement[4];
    uint32_t commune;
    
    BUG (common_ville_get_ville (ligne,
                                 NULL,
                                 NULL,
                                 NULL,
                                 departement,
                                 &commune,
                                 NULL,
                                 NULL,
                                 &article,
                                 &artmaj,
                                 &ncc,
                                 &artmin,
                                 &nccenr,
                                 &codepostal,
                                 NULL,
                                 &population),
         ,
         fclose (villes);
           free (ligne);
           g_list_free_full (list, common_gtk_informations_free_adresse);
           free (ville);
           free (code_postal); )
    swprintf (code_postal2, 11, L"%d", codepostal);
    minuscule = new wchar_t [wcslen (artmin) + wcslen (nccenr) + 2];
    wcscpy (minuscule, artmin);
    wcscat (minuscule,
            (article == 5) || (article == 1) || (article == 0) ? L"" : L" ");
    wcscat (minuscule, nccenr);
    
    if ((population > popmin) &&
        (((f_codepostal) &&
          (wcsncmp (code_postal2, code_postal, wcslen (code_postal)) == 0)
         ) ||
         ((!f_codepostal) && (strcasestr_internal (minuscule, ville) != NULL))
        )
       )
    {
      Ligne_Adresse *adresse;
      char           txt[5];
      
      adresse = new Ligne_Adresse;
      adresse->affichage = format ("%d %ls%s%ls",
                                   codepostal,
                                   artmin,
                                   ((article == 5) || (article == 1) ||
                                    (article == 0)) ? "" : " ",
                                   nccenr);
      adresse->population = population;
      wcstombs (txt, departement, 4);
      txt[4] = '\00';
      adresse->departement.assign (txt);
      adresse->commune = commune;
      adresse->code_postal = codepostal;
      adresse->ville = minuscule;
      list = g_list_insert_sorted (list,
                                   adresse,
                       (GCompareFunc) common_gtk_informations_compare_adresse);
      i++;
      
      // On limite le nombre de villes dans la liste à MAX_VILLES
      if (i > MAX_VILLES)
      {
        adresse = (Ligne_Adresse *) list->data;
        delete adresse;
        
        list = g_list_delete_link (list, list);
        i--;
      }
      
      adresse = (Ligne_Adresse *) list->data;
      popmin = adresse->population;
    }
    else
    {
      delete [] minuscule;
    }
    free (ligne);
    ligne = common_text_get_line (villes);
  }
  free (code_postal);
  free (ville);
  
  // On ajoute dans le treeview la liste des villes
  list_parcours = g_list_last (list);
  while (list_parcours != NULL)
  {
    Ligne_Adresse *adresse = (Ligne_Adresse *) list_parcours->data;
    std::string    tmp;
    char          *ville_tmp2;
    
    tmp = std::to_string (adresse->code_postal);
    gtk_list_store_append (UI_INFO.model_completion, &iter);
    BUG (ville_tmp2 = common_text_wcstostr_dup (adresse->ville),
         ,
         (gettext ("Erreur d'allocation mémoire.\n"));
           fclose (villes);
           g_list_free_full (list, common_gtk_informations_free_adresse); )
    gtk_list_store_set (UI_INFO.model_completion,
                        &iter,
                        0, adresse->affichage.c_str (),
                        1, tmp.c_str (),
                        2, ville_tmp2,
                        3, adresse->departement.c_str (),
                        4, adresse->commune,
                        -1);
    
    free (ville_tmp2);
    
    list_parcours = g_list_previous (list_parcours);
  }
  
  g_list_free_full (list, common_gtk_informations_free_adresse);
  
  common_gtk_informations_check (NULL, p);
  
  fclose (villes);
  
  return;
  
  #undef MAX_VILLES
}


/**
 * \brief Idem que #common_gtk_informations_entry_del_char.
 * \param buffer : buffer en cours d'édition,
 * \param position : la position du cluster,
 * \param chars : caractères ajoutés,
 * \param n_chars : nombre de caractère supprimé,
 * \param p : la variable projet.
 * \return Rien.
 */
extern "C"
void
common_gtk_informations_entry_add_char (GtkEntryBuffer *buffer,
                                        guint           position,
                                        gchar          *chars,
                                        guint           n_chars,
                                        Projet         *p)
{
  common_gtk_informations_entry_del_char (buffer, position, n_chars, p);
  return;
}


/**
 * \brief Modifie le code postal et la ville en fonction de la sélection de la
 *        ville dans la liste des propositions.
 * \param widget : composant à l'origine de l'évènement,
 * \param model : le composant model contenant la liste des propositions,
 * \param iter : la ligne sélectinnée du composant model,
 * \param p : la variable projet.
 * \return
 *   Succès : TRUE.\n
 *   Échec : FALSE :
 *     - p == NULL,
 *     - iter == NULL.
 *     - interface graphique non initialisée.
 */
extern "C"
gboolean
common_gtk_informations_match_selected (GtkEntryCompletion *widget,
                                        GtkTreeModel       *model,
                                        GtkTreeIter        *iter,
                                        Projet             *p)
{
  uint32_t    commune;
  std::string departement;
  std::string ville_tmp;
  wchar_t    *ville;
  
  BUGPARAM (p, "%p", p, FALSE)
  BUGPARAM (iter, "%p", iter, FALSE)
  BUGCRIT (UI_INFO.builder,
           FALSE,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Informations"); )
  
  gtk_tree_model_get (model,
                      iter,
                      2, &ville_tmp,
                      3, &departement,
                      4, &commune,
                      -1);
  
  free (UI_INFO.departement);
  UI_INFO.departement = common_text_strtowcs_dup (departement.c_str ());
  UI_INFO.commune = commune;
  BUG (ville = common_text_strtowcs_dup (ville_tmp.c_str ()),
       FALSE)
  
  BUG (common_ville_set (p, UI_INFO.departement, ville, TRUE),
       FALSE,
       free (ville); )
  
  common_gtk_informations_check (NULL, p);
  
  free (ville);
  
  return TRUE;
}


/**
 * \brief Ferme la fenêtre en appliquant les modifications.
 * \param button : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.
 *   Échec :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 *     - erreur d'allocation mémoire.
 */
extern "C"
void
common_gtk_informations_modifier_clicked (GtkButton *button,
                                          Projet    *p)
{
  uint32_t    code_postal;
  std::string destinataire, adresse, ville;
  Type_Neige  neige;
  Type_Vent   vent;
  Type_Seisme seisme;
  std::string message;
  const char *const_txt;
  wchar_t    *txt_tmp;
  
  BUGPARAM (p, "%p", p, )
  BUGCRIT (UI_INFO.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Informations"); )
  
  if (!common_gtk_informations_recupere_donnees (p,
                                                 &destinataire,
                                                 &adresse,
                                                 &code_postal,
                                                 &ville))
  {
    return;
  }
  
  // On récupère les valeurs en cours avant common_ville_set car ce dernier
  // réinitialise les valeurs.
  neige = (Type_Neige) gtk_combo_box_get_active (GTK_COMBO_BOX (
            gtk_builder_get_object (
            UI_INFO.builder, "common_informations_neige_combobox")));
  vent = (Type_Vent) gtk_combo_box_get_active (GTK_COMBO_BOX (
           gtk_builder_get_object (
           UI_INFO.builder, "common_informations_vent_combobox")));
  seisme = (Type_Seisme) gtk_combo_box_get_active (GTK_COMBO_BOX (
             gtk_builder_get_object (
             UI_INFO.builder, "common_informations_seisme_combobox")));
  
  const_txt = gtk_entry_get_text (GTK_ENTRY (gtk_builder_get_object (
                         UI_INFO.builder, "common_informations_entry_ville")));
  BUG (txt_tmp = common_text_strtowcs_dup (const_txt), )
  BUG (common_ville_set (p,
                         UI_INFO.departement,
                         txt_tmp,
                         FALSE),
      ,
        free (txt_tmp); )
  free (txt_tmp);
  p->parametres.adresse.destinataire = destinataire;
  p->parametres.adresse.adresse = adresse;
  p->parametres.adresse.code_postal = code_postal;
  p->parametres.adresse.ville = ville;
  
  gtk_combo_box_set_active (GTK_COMBO_BOX (gtk_builder_get_object (
                                                               UI_INFO.builder,
                                        "common_informations_neige_combobox")),
                            neige);
  gtk_combo_box_set_active (GTK_COMBO_BOX (gtk_builder_get_object (
                                                               UI_INFO.builder,
                                         "common_informations_vent_combobox")),
                            vent);
  gtk_combo_box_set_active (GTK_COMBO_BOX (gtk_builder_get_object (
                                                               UI_INFO.builder,
                                       "common_informations_seisme_combobox")),
                            seisme);
  
  if (neige != p->parametres.neige)
  {
    message = std::string (gettext ("Êtes-vous sûr de vouloir choisir pour")) +
              " " + std::string (gettext ("la neige"));
  }
  if (vent != p->parametres.vent)
  {
    if (message.length () == 0)
    {
      message = std::string (gettext ("Êtes-vous sûr de vouloir choisir pour")) +
                " " + std::string (gettext ("le vent"));
    }
    else
    {
      message += ", " + std::string (gettext ("le vent"));
    }
  }
  if (seisme != p->parametres.seisme)
  {
    if (message.length () == 0)
    {
      message = std::string (gettext ("Êtes-vous sûr de vouloir choisir pour")) +
                " " + std::string (gettext ("le séisme"));
    }
    else
    {
      message += ", " + std::string (gettext ("le séisme"));
    }
  }
  if (message.length () != 0)
  {
    GtkWidget *dialog;
    
    message += gettext(" des paramètres de calculs différents que ceux imposés par les normes ?");
    dialog = gtk_message_dialog_new (GTK_WINDOW (UI_INFO.window),
                                     GTK_DIALOG_MODAL,
                                     GTK_MESSAGE_WARNING,
                                     GTK_BUTTONS_YES_NO,
                                     "%s",
                                     message.c_str ());
    if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_YES)
    {
      p->parametres.neige = neige;
      p->parametres.vent = vent;
      p->parametres.seisme = seisme;
      gtk_widget_destroy (UI_INFO.window);
    }
    gtk_widget_destroy (dialog);
  }
  else
  {
    gtk_widget_destroy (UI_INFO.window);
  }
  
  return;
}


/**
 * \brief Indique que tous les éléments de la liste des propositions
 *        correspond à la requête. En effet, la liste des propositions est
 *        générée en temps réel via la fonction
 *        #common_gtk_informations_entry_del_char.
 * \param completion : le composant à l'origine de l'évènement,
 * \param key : le texte du composant completion,
 * \param iter : la liste du model du composant completion à valider,
 * \param user_data : la variable projet.
 * \return TRUE.
 */
extern "C"
gboolean
common_gtk_informations_always_match (GtkEntryCompletion *completion,
                                      const gchar        *key,
                                      GtkTreeIter        *iter,
                                      gpointer            user_data)
{
  return TRUE;
}


/**
 * \brief Affichage de la fenêtre permettant de personnaliser l'adresse du
 *        projet et les caractéristiques.
 * \param p : la variable projet.
 * \return
 *   Succès : true.\n
 *   Echec : false :
 *     - p == NULL,
 *     - Fenêtre graphique déjà initialisée.
 */
bool
common_gtk_informations (Projet *p)
{
  GtkEntryCompletion *completion;
  
  BUGPARAM (p, "%p", p, false)
  
  if (UI_INFO.builder != NULL)
  {
    gtk_window_present (GTK_WINDOW (UI_INFO.window));
    return true;
  }
  
  UI_INFO.builder = gtk_builder_new ();
  BUGCRIT (gtk_builder_add_from_resource (UI_INFO.builder,
                                 "/org/2lgc/codegui/ui/common_informations.ui",
                                          NULL) != 0,
          false,
          (gettext ("La génération de la fenêtre %s a échouée.\n"),
                    "Informations"); )
  gtk_builder_connect_signals (UI_INFO.builder, p);
  UI_INFO.window = GTK_WIDGET (gtk_builder_get_object (UI_INFO.builder,
                                                "common_informations_window"));
  UI_INFO.model_completion = GTK_LIST_STORE (gtk_builder_get_object (
                     UI_INFO.builder, "common_informations_completion_model"));
  
  BUG (UI_INFO.departement =
         common_text_strtowcs_dup (p->parametres.adresse.departement.c_str ()),
       false);
  UI_INFO.commune = p->parametres.adresse.commune;
  common_gtk_informations_check (NULL, p);
  
  // On ne peut pas mettre text-column directement dans le fichier ui car ça ne
  // marche pas.
  completion = GTK_ENTRY_COMPLETION (gtk_builder_get_object (UI_INFO.builder,
                                "common_informations_completion_code_postal"));
  gtk_entry_completion_set_text_column (completion, 0);
  gtk_entry_completion_set_match_func (completion,
                                       common_gtk_informations_always_match,
                                       NULL,
                                       NULL);
  completion = GTK_ENTRY_COMPLETION (gtk_builder_get_object (UI_INFO.builder,
                                      "common_informations_completion_ville"));
  gtk_entry_completion_set_text_column (completion, 0);
  gtk_entry_completion_set_match_func (completion,
                                       common_gtk_informations_always_match,
                                       NULL,
                                       NULL);
  
  if (p->parametres.adresse.destinataire.length () != 0)
  {
    gtk_text_buffer_set_text (GTK_TEXT_BUFFER (gtk_builder_get_object (
                  UI_INFO.builder, "common_informations_buffer_destinataire")),
                              p->parametres.adresse.destinataire.c_str (),
                              -1);
  }
  if (p->parametres.adresse.adresse.length () != 0)
  {
    gtk_text_buffer_set_text (GTK_TEXT_BUFFER (gtk_builder_get_object (
                       UI_INFO.builder, "common_informations_buffer_adresse")),
                              p->parametres.adresse.adresse.c_str (), -1);
  }
  if (p->parametres.adresse.code_postal != 0)
  {
    std::string texte;
    
    texte = std::to_string (p->parametres.adresse.code_postal);
    gtk_entry_set_text (GTK_ENTRY (gtk_builder_get_object (UI_INFO.builder,
                                     "common_informations_entry_code_postal")),
                        texte.c_str ());
  }
  if (p->parametres.adresse.ville.length () != 0)
  {
    gtk_entry_set_text (GTK_ENTRY (gtk_builder_get_object (UI_INFO.builder,
                                           "common_informations_entry_ville")),
                        p->parametres.adresse.ville.c_str ());
  }
  
  g_object_set (gtk_builder_get_object (UI_INFO.builder,
                                         "common_informations_neige_combobox"),
                "model", p->parametres.neige_desc,
                NULL);
  g_object_set (gtk_builder_get_object (UI_INFO.builder,
                                          "common_informations_vent_combobox"),
                "model", p->parametres.vent_desc,
                NULL);
  g_object_set (gtk_builder_get_object (UI_INFO.builder,
                                        "common_informations_seisme_combobox"),
                "model", p->parametres.seisme_desc,
                NULL);
  
  gtk_combo_box_set_active (GTK_COMBO_BOX (gtk_builder_get_object (
                       UI_INFO.builder, "common_informations_neige_combobox")),
                            p->parametres.neige);
  gtk_combo_box_set_active (GTK_COMBO_BOX (gtk_builder_get_object (
                        UI_INFO.builder, "common_informations_vent_combobox")),
                            p->parametres.vent);
  gtk_combo_box_set_active (GTK_COMBO_BOX (gtk_builder_get_object (
                      UI_INFO.builder, "common_informations_seisme_combobox")),
                            p->parametres.seisme);
  
  gtk_window_set_transient_for (GTK_WINDOW (UI_INFO.window),
                                GTK_WINDOW (UI_GTK.window));
  
  return true;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
