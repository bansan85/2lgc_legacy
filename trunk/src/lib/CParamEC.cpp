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

#include <cmath>
#include <cstdlib>

#include "CParamEC.hpp"
#include "MAbrev.hh"
#include "MErreurs.hh"


/**
 * \brief Constructeur d'une classe CParamEC.
 * \param nom_ (in) Le nom décrivant la norme et ses différents options.
 * \param annexe_ (in) Le choix de l'annexe nationale.
 * \param variante_ (in) Le numéro de la variante de la norme.
 * \param undo_ (in) Le gestionnaire des modifications.
 */
CParamEC::CParamEC (std::string  * nom_,
                    ENormeEcAc     annexe_,
                    uint16_t       variante_,
                    CUndoManager & undo_) :
  IParametres (undo_),
  nom (nom_),
  annexe (annexe_),
  variante (variante_)
{
}


/**
 * \brief Duplication d'une classe CParamEC.
 * \param other La classe à dupliquer.
 */
CParamEC::CParamEC (const CParamEC & other) :
  IParametres (other.getUndoManager ()),
  nom (new std::string (*other.nom)),
  annexe (other.annexe),
  variante (other.variante)
{
}


/**
 * \brief Assignment operator de CParamEC.
 * \param other La classe à dupliquer.
 */
//CParamEC &
//CParamEC::operator = (const CParamEC & other) = delete;


/**
 * \brief Destructeur d'une classe CParamEC.
 */
CParamEC::~CParamEC ()
{
}


/**
 * \brief Renvoie le nom des paramètres de calcul.
 */
std::string *
CParamEC::getNom () const
{
  return nom;
}


/**
 * \brief Définit le nom des paramètres de calcul.
 * \param nom_ Le nouveau nom.
 */
bool CHK
CParamEC::setNom (std::string * nom_)
{
  BUGCONT (getUndoManager ().ref (), false, &getUndoManager ())
  
  BUGCONT (getUndoManager ().push (
             std::bind (&CParamEC::setNom, this, nom),
             std::bind (&CParamEC::setNom, this, nom_),
             std::bind (std::default_delete <std::string> (), nom_),
             std::bind (&CParamEC::setNomXML,
                        this,
                        nom,
                        nom_,
                        std::placeholders::_1)),
             false,
             &getUndoManager ())
  
  nom = nom_;
  
  BUGCONT (getUndoManager ().unref (), false, &getUndoManager ())
  
  return true;
}


/**
 * \brief Converti la fonction setNom sous format XML.
 * \param param Le nom du paramètre.
 * \param nom_ Le nouveau nom.
 * \param root Le noeud dans lequel doit être inséré l'action.
 */
bool CHK
CParamEC::setNomXML (std::string * param,
                     std::string * nom_,
                     xmlNodePtr    root)
{
  std::unique_ptr <xmlNode, void (*)(xmlNodePtr)> node (
                    xmlNewNode (NULL, BAD_CAST2 ("ParamSetNom")), xmlFreeNode);
  
  BUGCRIT (node.get (),
           false,
           &getUndoManager (),
           gettext ("Erreur d'allocation mémoire.\n"))
  
  BUGCRIT (xmlSetProp (node.get (),
                       BAD_CAST2 ("param"),
                       BAD_CAST2 (param->c_str ())),
           false,
           &getUndoManager (),
           gettext ("Problème depuis la librairie : %s\n"), "xml2")
  
  BUGCRIT (xmlSetProp (node.get (),
                       BAD_CAST2 ("nom"),
                       BAD_CAST2 (nom_->c_str ())),
           false,
           &getUndoManager (),
           gettext ("Problème depuis la librairie : %s\n"), "xml2")
  
  BUGCRIT (xmlAddChild (root, node.get ()),
           false,
           &getUndoManager (),
           gettext ("Problème depuis la librairie : %s\n"), "xml2")
  node.release ();
  
  return true;
}


/**
 * \brief Renvoie la variante de la norme.
 */
uint32_t
CParamEC::getVariante () const
{
  return variante;
}


/**
 * \brief Défini la variante de la norme.
 * \param variante_  (in) La nouvelle variante.
 */
bool CHK
CParamEC::setVariante (uint32_t variante_)
{
  BUGCONT (getUndoManager ().ref (), false, &getUndoManager ())
  
  BUGCONT (getUndoManager ().push (
           std::bind (&CParamEC::setVariante, this, variante),
           std::bind (&CParamEC::setVariante, this, variante_),
           NULL,
           std::bind (&CParamEC::setVarianteXML,
                      this,
                      nom,
                      variante_,
                      std::placeholders::_1)),
           false,
           &getUndoManager ())
  
  variante = variante_;
  
  return true;
}


/**
 * \brief Converti la fonction setVariante sous format XML.
 * \param nom_ Le nom du paramètre de calcul à modifier.
 * \param variante_ La nouvelle variante.
 */
bool CHK
CParamEC::setVarianteXML (std::string * nom_,
                          uint32_t      variante_,
                          xmlNodePtr    root)
{
  std::unique_ptr <xmlNode, void (*)(xmlNodePtr)> node (
               xmlNewNode (NULL, BAD_CAST2 ("ParamSetVariante")), xmlFreeNode);
  
  BUGCRIT (node.get (),
           false,
           &getUndoManager (),
           gettext ("Erreur d'allocation mémoire.\n"))
  
  BUGCRIT (xmlSetProp (node.get (),
                       BAD_CAST2 ("param"),
                       BAD_CAST2 (nom_->c_str ())),
           false,
           &getUndoManager (),
           gettext ("Problème depuis la librairie : %s\n"), "xml2")
  
  BUGCRIT (xmlSetProp (node.get (),
                       BAD_CAST2 ("variante"),
                       BAD_CAST2 (std::to_string(variante_).c_str ())),
           false,
           &getUndoManager (),
           gettext ("Problème depuis la librairie : %s\n"), "xml2")
  
  BUGCRIT (xmlAddChild (root, node.get ()),
           false,
           &getUndoManager (),
           gettext ("Problème depuis la librairie : %s\n"), "xml2")
  node.release ();
  
  return true;
}


/**
 * \brief Renvoie le nombre de type d'actions différents de psi.
 */
uint8_t
CParamEC::getpsiN () const
{
  switch (annexe)
  {
    case NORMEEUAC_EU :
    {
      return 17;
    }
    case NORMEEUAC_FR :
    {
      return 22;
    }
    default :
    {
      BUGPARAM (annexe, "%d", NULL, 0, &getUndoManager ())
      break;
    }
  }
}


/**
 * \brief Renvoie la catégorie du type d'action.
 * \param type (in) Le type d'action.
 */
EAction
CParamEC::getpsiAction (uint8_t type) const
{
  switch (annexe)
  {
    case NORMEEUAC_EU :
    {
      if (type == 0)
      {
        return ACTION_POIDS_PROPRE;
      }
      else if (type == 1)
      {
        return ACTION_PRECONTRAINTE;
      }
      else if ((2 <= type) && (type <= 14))
      {
        return ACTION_VARIABLE;
      }
      else if (type == 15)
      {
        return ACTION_ACCIDENTELLE;
      }
      else if (type == 16)
      {
        return ACTION_SISMIQUE;
      }
      else
      {
        BUGPARAM (type, "%u", NULL, ACTION_INCONNUE, &getUndoManager ())
      }
    }
    case NORMEEUAC_FR :
    {
      if (type == 0) 
      {
        return ACTION_POIDS_PROPRE;
      }
      else if (type == 1) 
      {
        return ACTION_PRECONTRAINTE;
      }
      else if ((2 <= type) && (type <= 18))
      {
        return ACTION_VARIABLE;
      }
      else if (type == 19)
      {
        return ACTION_ACCIDENTELLE;
      }
      else if (type == 20)
      {
        return ACTION_SISMIQUE;
      }
      else if (type == 21)
      {
        return ACTION_EAUX_SOUTERRAINES;
      }
      else
      {
        BUGPARAM (type, "%u", NULL, ACTION_INCONNUE, &getUndoManager ())
      }
    }
    default :
    {
      BUGPARAM (annexe, "%d", NULL, ACTION_INCONNUE, &getUndoManager ())
      break;
    }
  }
}


/**
 * \brief Renvoie la description du type d'actions différents de psi.
 * \param type (in) Le type d'action à décrire.
 */
std::string const
CParamEC::getpsiDescription (uint8_t type) const
{
  switch (annexe)
  {
    case NORMEEUAC_EU :
    {
      switch (type)
      {
        case 0 : return gettext ("Permanente");
        case 1 : return gettext ("Précontrainte");
        case 2 : return gettext ("Exploitation : Catégorie A : habitation, zones résidentielles");
        case 3 : return gettext ("Exploitation : Catégorie B : bureaux");
        case 4 : return gettext ("Exploitation : Catégorie C : lieux de réunion");
        case 5 : return gettext ("Exploitation : Catégorie D : commerces");
        case 6 : return gettext ("Exploitation : Catégorie E : stockage");
        case 7 : return gettext ("Exploitation : Catégorie F : zone de trafic, véhicules de poids inférieur à 30 kN");
        case 8 : return gettext ("Exploitation : Catégorie G : zone de trafic, véhicules de poids entre 30 kN et 160 kN");
        case 9 : return gettext ("Exploitation : Catégorie H : toits"); 
        case 10 : return gettext ("Neige : Finlande, Islande, Norvège, Suède"); 
        case 11 : return gettext ("Neige : Autres états membres CEN, altitude > 1000 m"); 
        case 12 : return gettext ("Neige : Autres états membres CEN, altitude <= 1000 m"); 
        case 13 : return gettext ("Vent");
        case 14 : return gettext ("Température (hors incendie)");
        case 15 : return gettext ("Accidentelle");
        case 16 : return gettext ("Sismique");
        default :
        {
          BUGPARAM (type, "%u", NULL, std::string (), &getUndoManager ())
          break;
        }
      }
    }
    case NORMEEUAC_FR :
    {
      switch (type)
      {
        case 0 : return gettext ("Permanente");
        case 1 : return gettext ("Précontrainte");
        case 2 : return gettext ("Exploitation : Catégorie A : habitation, zones résidentielles");
        case 3 : return gettext ("Exploitation : Catégorie B : bureaux");
        case 4 : return gettext ("Exploitation : Catégorie C : lieux de réunion");
        case 5 : return gettext ("Exploitation : Catégorie D : commerces");
        case 6 : return gettext ("Exploitation : Catégorie E : stockage");
        case 7 : return gettext ("Exploitation : Catégorie F : zone de trafic, véhicules de poids inférieur à 30 kN");
        case 8 : return gettext ("Exploitation : Catégorie G : zone de trafic, véhicules de poids entre 30 kN et 160 kN");
        case 9 : return gettext ("Exploitation : Catégorie H : toits d'un bâtiment de catégorie A ou B");
        case 10 : return gettext ("Exploitation : Catégorie I : toitures accessibles avec locaux des catégories A ou B");
        case 11 : return gettext ("Exploitation : Catégorie I : toitures accessibles avec locaux des catégories C ou D");
        case 12 : return gettext ("Exploitation : Catégorie K : Hélicoptère sur la toiture");
        case 13 : return gettext ("Exploitation : Catégorie K : Hélicoptère sur la toiture, autres charges (fret, personnel, accessoires ou équipements divers)");
        case 14 : return gettext ("Neige : Saint-Pierre-et-Miquelon");
        case 15 : return gettext ("Neige : Altitude > 1000 m");
        case 16 : return gettext ("Neige : Altitude <= 1000 m");
        case 17 : return gettext ("Vent");
        case 18 : return gettext ("Température (hors incendie)");
        case 19 : return gettext ("Accidentelle");
        case 20 : return gettext ("Sismique");
        case 21 : return gettext ("Eaux souterraines");
        default :
        {
          BUGPARAM (type, "%u", NULL, std::string (), &getUndoManager ())
          break;
        }
      }
    }
    default :
    {
      BUGPARAM (annexe, "%d", NULL, std::string (), &getUndoManager ())
      break;
    }
  }
}


/**
 * \brief Renvoie la valeur par défaut du coefficient psi0.
 * \param type (in) Le type de l'action.
 */
double
CParamEC::getpsi0 (uint8_t type) const
{
  switch (annexe)
  {
    case NORMEEUAC_EU :
    {
      switch (type)
      {
        case 0 : return 0.0;
        case 1 : return 0.0;
        case 2 : return 0.7;
        case 3 : return 0.7;
        case 4 : return 0.7;
        case 5 : return 0.7;
        case 6 : return 1.0;
        case 7 : return 0.7;
        case 8 : return 0.7;
        case 9 : return 0.0;
        case 10 : return 0.7;
        case 11 : return 0.7;
        case 12 : return 0.5;
        case 13 : return 0.6;
        case 14 : return 0.6;
        case 15 : return 0.0;
        case 16 : return 0.0;
        default :
        {
          BUGPARAM (type, "%u", NULL, NAN, &getUndoManager ())
          break;
        }
      }
    }
    case NORMEEUAC_FR :
    {
      switch (type)
      {
        case 0 : return 0.0;
        case 1 : return 0.0;
        case 2 : return 0.7;
        case 3 : return 0.7;
        case 4 : return 0.7;
        case 5 : return 0.7;
        case 6 : return 1.0;
        case 7 : return 0.7;
        case 8 : return 0.7;
        case 9 : return 0.0;
        case 10 : return 0.7;
        case 11 : return 0.7;
        case 12 : return 1.0;
        case 13 : return 0.7;
        case 14 : return 0.7;
        case 15 : return 0.7;
        case 16 : return 0.5;
        case 17 : return 0.6;
        case 18 : return 0.6;
        case 19 : return 0.0;
        case 20 : return 0.0;
        case 21 : return 0.0;
        default :
        {
          BUGPARAM (type, "%u", NULL, NAN, &getUndoManager ())
          break;
        }
      }
    }
    default :
    {
      BUGPARAM (annexe, "%d", NULL, NAN, &getUndoManager ())
      break;
    }
  }
}


/**
 * \brief Renvoie la valeur par défaut du coefficient psi1.
 * \param type (in) Le type de l'action.
 */
double
CParamEC::getpsi1 (uint8_t type) const
{
  switch (annexe)
  {
    case NORMEEUAC_EU :
    {
      switch (type)
      {
        case 0 : return 0.0;
        case 1 : return 0.0;
        case 2 : return 0.5;
        case 3 : return 0.5;
        case 4 : return 0.7;
        case 5 : return 0.7;
        case 6 : return 0.9;
        case 7 : return 0.7;
        case 8 : return 0.5;
        case 9 : return 0.0;
        case 10 : return 0.5;
        case 11 : return 0.5;
        case 12 : return 0.2;
        case 13 : return 0.2;
        case 14 : return 0.5;
        case 15 : return 0.0;
        case 16 : return 0.0;
        default :
        {
          BUGPARAM (type, "%u", NULL, NAN, &getUndoManager ())
          break;
        }
      }
    }
    case NORMEEUAC_FR :
    {
      switch (type)
      {
        case 0 : return 0.0;
        case 1 : return 0.0;
        case 2 : return 0.5;
        case 3 : return 0.5;
        case 4 : return 0.7;
        case 5 : return 0.7;
        case 6 : return 0.9;
        case 7 : return 0.7;
        case 8 : return 0.5;
        case 9 : return 0.0;
        case 10 : return 0.5;
        case 11 : return 0.7;
        case 12 : return 0.9;
        case 13 : return 0.5;
        case 14 : return 0.5;
        case 15 : return 0.5;
        case 16 : return 0.2;
        case 17 : return 0.2;
        case 18 : return 0.5;
        case 19 : return 0.0;
        case 20 : return 0.0;
        case 21 : return 0.0;
        default :
        {
          BUGPARAM (type, "%u", NULL, NAN, &getUndoManager ())
          break;
        }
      }
    }
    default :
    {
      BUGPARAM (annexe, "%d", NULL, NAN, &getUndoManager ())
      break;
    }
  }
}


/**
 * \brief Renvoie la valeur par défaut du coefficient psi2.
 * \param type (in) Le type de l'action.
 */
double
CParamEC::getpsi2 (uint8_t type) const
{
  switch (annexe)
  {
    case NORMEEUAC_EU :
    {
      switch (type)
      {
        case 0 : return 0.0;
        case 1 : return 0.0;
        case 2 : return 0.3;
        case 3 : return 0.3;
        case 4 : return 0.6;
        case 5 : return 0.6;
        case 6 : return 0.8;
        case 7 : return 0.6;
        case 8 : return 0.3;
        case 9 : return 0.0;
        case 10 : return 0.2;
        case 11 : return 0.2;
        case 12 : return 0.0;
        case 13 : return 0.0;
        case 14 : return 0.0;
        case 15 : return 0.0;
        case 16 : return 0.0;
        default :
        {
          BUGPARAM (type, "%u", NULL, NAN, &getUndoManager ())
          break;
        }
      }
    }
    case NORMEEUAC_FR :
    {
      switch (type)
      {
        case 0 : return 0.0;
        case 1 : return 0.0;
        case 2 : return 0.3;
        case 3 : return 0.3;
        case 4 : return 0.6;
        case 5 : return 0.6;
        case 6 : return 0.8;
        case 7 : return 0.6;
        case 8 : return 0.3;
        case 9 : return 0.0;
        case 10 : return 0.3;
        case 11 : return 0.6;
        case 12 : return 0.5;
        case 13 : return 0.3;
        case 14 : return 0.2;
        case 15 : return 0.2;
        case 16 : return 0.0;
        case 17 : return 0.0;
        case 18 : return 0.0;
        case 19 : return 0.0;
        case 20 : return 0.0;
        case 21 : return 0.0;
        default :
        {
          BUGPARAM (type, "%u", NULL, NAN, &getUndoManager ())
          break;
        }
      }
    }
    default :
    {
      BUGPARAM (annexe, "%d", NULL, NAN, &getUndoManager ())
      break;
    }
  }
}


/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
