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
#include <string.h>
#include <gmodule.h>

#include <algorithm>

#include "1990_action.hpp"
#include "common_projet.hpp"
#include "EF_noeuds.hpp"
#include "EF_charge.hpp"
#include "1992_1_1_barres.hpp"
#include "common_erreurs.hpp"
#include "common_selection.hpp"

/**
 * \brief Ajoute un nombre non signé à la liste chainée.
 * \param nb : nombre à ajouter,
 * \param liste : liste où la donnée doit être ajoutée.
 * \return Rien.
 */
void
common_selection_ajout_nombre (uint32_t              nb,
                               std::list <uint32_t> *liste)
{
  std::list <uint32_t>::iterator it;
  
  BUGPARAM (liste, "%p", liste, )
  
  it = liste->begin ();
  
  while (it != liste->end ())
  {
    uint32_t nombre_liste;
    
    nombre_liste = *it;
    
    if (nombre_liste == nb)
    {
      return;
    }
    else if (nombre_liste > nb)
    {
      liste->insert (it, nb);
      return;
    }
    
    ++it;
  }
  
  liste->push_back (nb);
  
  return;
}


/**
 * \brief Ajoute un noeud à la liste chainée.
 * \param noeud : noeud à ajouter,
 * \param liste : liste où la donnée doit être ajoutée.
 * \return Rien.
 */
void
common_selection_ajout_nombre (EF_Noeud               *noeud,
                               std::list <EF_Noeud *> *liste)
{
  std::list <EF_Noeud *>::iterator it;
  
  BUGPARAM (liste, "%p", liste, )
  
  it = liste->begin ();
  
  while (it != liste->end ())
  {
    EF_Noeud *noeud_liste;
    
    noeud_liste = *it;
    
    if (noeud_liste->numero == noeud->numero)
    {
      return;
    }
    else if (noeud_liste->numero > noeud->numero)
    {
      liste->insert (it, noeud);
      return;
    }
    
    ++it;
  }
  
  liste->push_back (noeud);
  
  return;
}


/**
 * \brief Ajoute une barre à la liste chainée.
 * \param barre : barre à ajouter,
 * \param liste : liste où la donnée doit être ajoutée.
 * \return Rien.
 */
void
common_selection_ajout_nombre (EF_Barre               *barre,
                               std::list <EF_Barre *> *liste)
{
  std::list <EF_Barre *>::iterator it;
  
  it = liste->begin ();
  while (it != liste->end ())
  {
    EF_Barre *barre_liste;
    
    barre_liste = *it;
    
    if (barre_liste->numero == barre->numero)
    {
      return;
    }
    else if (barre_liste->numero > barre->numero)
    {
      liste->insert (it, barre);
      return;
    }
    
    ++it;
  }
  
  liste->push_back (barre);
  
  return;
}


/**
 * \brief Ajoute un nombre à la liste chainée.
 * \param charge : donnée à ajouter,
 * \param liste : liste où la donnée doit être ajoutée,
 * \param p : la variable projet, doit être différent de NULL uniquement si
 *            le type vaut #LISTE_CHARGES.
 * \return Rien.
 */
void
common_selection_ajout_nombre (Charge               *charge,
                               std::list <Charge *> *liste,
                               Projet               *p)
{
  std::list <Charge *>::iterator it;
  
  Action *action = NULL;
  long    pos_act, pos_cha;
  
  BUGPARAM (liste, "%p", liste, )
  BUGPARAM (p, "%p", p, )
  
  BUG (action = EF_charge_action (p, charge), )
  
  if (liste == NULL)
  {
    liste->push_back (charge);
    return;
  }
  
  pos_act = std::distance (p->actions.begin (),
                           std::find (p->actions.begin (),
                                      p->actions.end (),
                                      action));
  pos_cha = std::distance (_1990_action_charges_renvoie (action)->begin (),
              std::find (_1990_action_charges_renvoie (action)->begin (),
                         _1990_action_charges_renvoie (action)->end (),
                         charge));
  
  it = liste->begin ();
  while (it != liste->end ())
  {
    Charge *charge_liste;
    Action *action_en_cours;
    
    charge_liste = *it;
    BUG (action_en_cours = EF_charge_action (p, charge_liste), )
    
    if ((action_en_cours == action) && (charge_liste == charge))
    {
      return;
    }
    else if ((std::distance (p->actions.begin (),
                             std::find (p->actions.begin (),
                                        p->actions.end (),
                                        action_en_cours))> pos_act) ||
             ((action_en_cours == action) &&
              (std::distance (_1990_action_charges_renvoie (action)->begin (),
                 std::find (_1990_action_charges_renvoie (action)->begin (),
                            _1990_action_charges_renvoie (action)->end (),
                            charge_liste)) >= pos_cha)))
    {
      liste->insert (it, charge);
      return;
    }
    
    ++it;
  }
  
  liste->push_back (charge);
  return;
}


/**
 * \brief Renvoie une liste de numéros sous forme de texte. Par exemple, le
 *        texte "1;2;3-4;6-9;10-20/2" donne les numéros
 *        "1 2 3 4 6 7 8 9 10 12 14 16 18 20" via la fonction GUINT_TO_POINTER.
 *        La virgule "," est considérée comme un point virgule ";" et les
 *        espaces négligés.
 * \param texte : le texte à convertir en numéros.
 * \return
 *   Succès : Un pointeur vers une liste de numéros triée.\n
 *   Échec : NULL :
 *     - Erreur d'allocation mémoire,
 *     - Format de texte illisible.
 */
std::list <uint32_t> *
common_selection_renvoie_numeros (const char *texte)
{
  char                 *texte_clean;
  std::list <uint32_t> *list;
  uint32_t              i, j;
  
  if (texte == NULL)
  {
    return NULL;
  }
  
  BUGCRIT (texte_clean = (char *) malloc (sizeof (char) *
                                                         (strlen (texte) + 1)),
           NULL,
           (gettext ("Erreur d'allocation mémoire.\n")); )
  
  // On vérifie si le texte contient bien une liste correcte de numéros
  i = 0;
  for (j = 0; j < strlen (texte); j++)
  {
    // Les caractères autorisées sont ,;-/0123456789
    if ((texte[j] == ';') ||
        (texte[j] == '-') ||
        (texte[j] == '/') ||
        (('0' <= texte[j]) && (texte[j] <= '9')))
    {
      texte_clean[i] = texte[j];
      i++;
    }
    else if (texte[j] == ',')
    {
      texte_clean[i] = ';';
      i++;
    }
    // On remplace par défaut les espaces par des ;
    else if (texte[j] == ' ')
    {
      texte_clean[i] = ';';
      i++;
    }
    // En cas de caractères inconnus, on ne fait rien
    else
    {
      free (texte_clean);
      return NULL;
    }
  }
  
  texte_clean[i] = 0;
  if (i == 0)
  {
    free (texte_clean);
    return NULL;
  }
  
  list = new std::list <uint32_t> ();
  
  // On parcours chaque numéro et on les ajoute à la liste.
  i = 0;
  do
  {
    while ((texte_clean[i] == ';') && (texte_clean[i] != 0))
    {
      i++;
    }
    if (texte_clean[i] != 0)
    {
      j = i + 1U;
      while ((texte_clean[j] != ';') && (texte_clean[j] != 0))
      {
        j++;
      }
      j--;
      // Il y a quelque chose à faire
      if ((j > i) || (texte_clean[i] != ';'))
      {
        char    *tmp;
        char    *fake;
        uint32_t debut, fin, pas;
        
        BUGCRIT (tmp = (char *) malloc (sizeof (char) * (j - i + 2U)),
                 NULL,
                 (gettext ("Erreur d'allocation mémoire.\n"));
                   free (texte_clean);
                   delete list; )
        BUGCRIT (fake = (char *) malloc (sizeof (char) * (j - i + 2U)),
                 NULL,
                 (gettext ("Erreur d'allocation mémoire.\n"));
                   free (texte_clean);
                   free (tmp);
                   delete list; )
        
        strncpy (tmp, texte_clean + i, j - i + 1U);
        tmp[j - i + 1] = 0;
        
        // Si c'est du format debut-fin/pas
        if (sscanf (tmp, "%u-%u/%u%s", &debut, &fin, &pas, fake) == 3)
        {
          for (i = debut; i <= fin; i = i + pas)
          {
            common_selection_ajout_nombre (i, list);
          }
        }
        // Si c'est du format debut-fin
        else if (sscanf (tmp, "%u-%u%s", &debut, &fin, fake) == 2)
        {
          for (i = debut; i <= fin; i++)
          {
            common_selection_ajout_nombre (i, list);
          }
        }
        // Si c'est du format nombre simple
        else if (sscanf (tmp, "%u%s", &debut, fake) == 1)
        {
          common_selection_ajout_nombre (debut, list);
        }
        // Le format est inconnu.
        else
        {
          free (tmp);
          free (fake);
          delete list;
          free (texte_clean);
          return NULL;
        }
        
        free (tmp);
        free (fake);
      }
      i = j + 1;
    }
  }
  while (i < strlen (texte_clean));
  
  free (texte_clean);
  
  return list;
}


/**
 * \brief Renvoie sous forme d'une liste de noeuds la liste des numéros.
 * \param liste_numeros : liste des numéros (sous format GPOINTER_TO_UINT) à
 *        convertir en liste de noeuds,
 * \param p : la variable projet.
 * \return
 *   Succès : Un pointeur vers une liste des noeuds.\n
 *   Échec : NULL :
 *     - un des noeuds est introuvable.
 */
std::list <EF_Noeud *> *
common_selection_numeros_en_noeuds (std::list <uint32_t> *liste_numeros,
                                    Projet               *p)
{
  std::list <EF_Noeud *> *liste_noeuds = new std::list <EF_Noeud *> ();
  
  std::list <uint32_t>::iterator it = liste_numeros->begin ();
  
  while (it != liste_numeros->end ())
  {
    uint32_t  numero = *it;
    EF_Noeud *noeud = EF_noeuds_cherche_numero (p, numero, FALSE);
    
    if (noeud == NULL)
    {
      delete liste_noeuds;
      return NULL;
    }
    else
    {
      liste_noeuds->push_back (noeud);
    }
    
    ++it;
  }
  
  return liste_noeuds;
}


/**
 * \brief Renvoie sous forme d'une liste de barres la liste des numéros.
 * \param liste_numeros : la liste des numéros (sous format GPOINTER_TO_UINT) à
 *        convertir en liste de barres,
 * \param p : la variable projet.
 * \return
 *   Succès : Un pointeur vers une liste des barres.\n
 *   Échec : NULL :
 *     - une des barres est introuvable.
 */
std::list <EF_Barre *> *
common_selection_numeros_en_barres (std::list <uint32_t> *liste_numeros,
                                    Projet               *p)
{
  std::list <EF_Barre *> *liste_barres = new std::list <EF_Barre *> ();
  
  std::list <uint32_t>::iterator it = liste_numeros->begin ();
  
  while (it != liste_numeros->end ())
  {
    uint32_t  numero = *it;
    EF_Barre *barre = _1992_1_1_barres_cherche_numero (p, numero, FALSE);
    
    if (barre == NULL)
    {
      delete liste_barres;
      return NULL;
    }
    else
    {
      liste_barres->push_back (barre);
    }
    
    ++it;
  }
  
  return liste_barres;
}


/**
 * \brief Renvoie sous forme de texte une liste de noeuds.
 * \param liste : la liste des noeuds à convertir en texte.
 * \return
 *   Succès : le texte correspondant.\n
 *   Échec : NULL :
 *     - Erreur d'allocation mémoire,
 */
// coverity[+alloc]
char *
common_selection_noeuds_en_texte (std::list <EF_Noeud *> *liste)
{
  char *tmp = NULL, *tmp2 = NULL;
  
  if (!liste->empty ())
  {
    std::list <EF_Noeud *>::iterator it;
    EF_Noeud *noeud;
    
    it = liste->begin ();
    noeud = *it;
    BUGCRIT (tmp = g_strdup_printf ("%d", noeud->numero),
             NULL,
             (gettext ("Erreur d'allocation mémoire.\n")); )
    if (std::next (it) != liste->end ())
    {
      ++it;
      do
      {
        noeud = *it;
        BUGCRIT (tmp2 = g_strdup_printf ("%s;%d", tmp, noeud->numero),
                 NULL,
                 (gettext ("Erreur d'allocation mémoire.\n"));
                  free (tmp); )
        free (tmp);
        tmp = tmp2;
        tmp2 = NULL;
        ++it;
      }
      while (it != liste->end ());
    }
  }
  else
  {
    BUGCRIT (tmp = g_strdup_printf (" "),
             NULL,
             (gettext ("Erreur d'allocation mémoire.\n")); )
  }
  
  return tmp;
}


/**
 * \brief Renvoie sous forme de texte une liste de barres.
 * \param liste_barres : la liste des barres à convertir en texte.
 * \return
 *   Succès : le texte correspondant.\n
 *   Échec : NULL :
 *     - Erreur d'allocation mémoire.
 */
// coverity[+alloc]
char *
common_selection_barres_en_texte (std::list <EF_Barre *> *liste_barres)
{
  char *tmp, *tmp2;
  
  if (!liste_barres->empty ())
  {
    std::list <EF_Barre *>::iterator it = liste_barres->begin ();
    
    EF_Barre *barre;
    
    barre = *it;
    BUGCRIT (tmp = g_strdup_printf ("%u", barre->numero),
             NULL,
             (gettext ("Erreur d'allocation mémoire.\n")); )
    
    ++it;
    while (it != liste_barres->end ())
    {
      barre = *it;
      BUGCRIT (tmp2 = g_strdup_printf ("%s;%u", tmp, barre->numero),
               NULL,
               (gettext ("Erreur d'allocation mémoire.\n"));
                 free (tmp); )
      free (tmp);
      tmp = tmp2;
      tmp2 = NULL;
      
      ++it;
    }
  }
  else
  {
    BUGCRIT (tmp = g_strdup_printf (" "),
             NULL,
             (gettext ("Erreur d'allocation mémoire.\n")); )
  }
  
  return tmp;
}


/**
 * \brief Renvoie sous forme de texte une liste de charges.
 * \param liste_charges : la liste des charges à convertir en texte,
 * \param p : la variable projet.
 * \return
 *   Succès : le texte correspondant.\n
 *   Échec : NULL :
 *     - Erreur d'allocation mémoire.
 */
// coverity[+alloc]
char *
common_selection_charges_en_texte (std::list <Charge *> *liste_charges,
                                   Projet               *p)
{
  char *tmp, *tmp2;
  
  if (!liste_charges->empty ())
  {
    std::list <Charge *>::iterator it;
    Charge *charge;
    Action *action = NULL;
    
    it = liste_charges->begin ();
    charge = *it;
    
    BUG (action = EF_charge_action (p, charge), NULL)
    
    BUGCRIT (tmp = g_strdup_printf ("%lu:%lu",
                                    std::distance (p->actions.begin (),
                                      std::find (p->actions.begin (),
                                                 p->actions.end (),
                                                 action)),
                std::distance (_1990_action_charges_renvoie (action)->begin (),
                  std::find (_1990_action_charges_renvoie (action)->begin (),
                             _1990_action_charges_renvoie (action)->end (),
                             charge))),
             NULL,
             (gettext ("Erreur d'allocation mémoire.\n")); )
    if (std::next (it) != liste_charges->end ())
    {
      ++it;
      while (it != liste_charges->end ())
      {
        std::list <Action *>::iterator it2 = p->actions.begin ();
        
        charge = *it;
        // On cherche dans la liste des actions laquelle possède la charge.
        while (it2 != p->actions.end ())
        {
          action = *it2;
          
          if (std::find (_1990_action_charges_renvoie(action)->begin (),
                         _1990_action_charges_renvoie(action)->end (),
                         charge) !=
                                  _1990_action_charges_renvoie(action)->end ())
          {
            break;
          }
           
          ++it2;
        }
        BUGCRIT (tmp2 = g_strdup_printf ("%s;%lu:%lu",
                                         tmp,
                                         std::distance (p->actions.begin (),
                                           std::find (p->actions.begin (),
                                                      p->actions.end (),
                                                      action)),
                std::distance (_1990_action_charges_renvoie (action)->begin (),
                  std::find (_1990_action_charges_renvoie (action)->begin (),
                             _1990_action_charges_renvoie (action)->end (),
                             charge))),
                 NULL,
                 (gettext ("Erreur d'allocation mémoire.\n"));
                   free (tmp); )
        free (tmp);
        tmp = tmp2;
        tmp2 = NULL;
        
        ++it;
      }
    }
  }
  else
  {
    BUGCRIT (tmp = g_strdup_printf (" "),
             NULL,
             (gettext ("Erreur d'allocation mémoire.\n")); )
  }
  
  return tmp;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
