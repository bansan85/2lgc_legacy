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
#include <algorithm>
#include <memory>
#include <iostream>
#include <locale>

#include "CAction.hpp"
#include "CNbCalcul.hpp"
#include "MAbrev.hh"
#include "MErreurs.hh"
#include "EUnite.hh"


/**
 * \brief Constructeur d'une classe CAction. Par défaut, les coefficients psi
 *        sont défini à NAN. Les valeurs sont automatiquement déterminés lors
 *        de l'insertion dans le projet en fonction de la norme du projet.
 * \param nom_ (in) Le nom de l'action.
 * \param type_ (in) Le type d'action, cf. _1990_action_bat_txt_type.
 * \param undo_ (in) Le gestionnaire des modifications.
 */
CAction::CAction (std::string  * nom_,
                  uint8_t        type_,
                  CUndoManager & undo_) :
  IActionGroupe (nom_, undo_)
  , type (type_)
  , action_predominante (false)
  , charges ()
  , psi0 (NULL)
  , psi1 (NULL)
  , psi2 (NULL)
  , parametres (NULL)
  , deplacement (NULL)
  , forces (NULL)
  , efforts_noeuds (NULL)
  , efforts {{}, {}, {}, {}, {}, {}}
  , deformation {{}, {}, {}}
  , rotation {{}, {}, {}}
#ifdef ENABLE_GTK
  , Iter_fenetre_ac (NULL)
  , Iter_liste (NULL)
#endif
{
}


/**
 * \brief Duplication d'une classe CAction.
 * \param other (in) La classe à dupliquer.
 */
CAction::CAction (const CAction & other) :
  IActionGroupe (other.getNom (), other.getUndoManager ())
  , type (other.type)
  , action_predominante (false)
  , charges ()
  , psi0 (NULL)
  , psi1 (NULL)
  , psi2 (NULL)
  , parametres (NULL)
  , deplacement (NULL)
  , forces (NULL)
  , efforts_noeuds (NULL)
  , efforts {{}, {}, {}, {}, {}, {}}
  , deformation {{}, {}, {}}
  , rotation {{}, {}, {}}
#ifdef ENABLE_GTK
  , Iter_fenetre_ac (NULL)
  , Iter_liste (NULL)
#endif
{
}


/**
 * \brief Assignment operator de CAction.
 * \param other (in) La classe à dupliquer.
 */
CAction &
CAction::operator = (const CAction & other) = delete;


/**
 * \brief Libère une classe CAction.
 */
CAction::~CAction ()
{
}


/**
 * \brief Converti la fonction d'ajout d'une action sous format XML..
 * \param root Le noeud dans lequel doit être inséré l'action.
 */
bool CHK
CAction::addXML (std::string *nom_,
                 uint8_t      type_,
                 xmlNodePtr   root)
{
  BUGPARAM (root, "%p", root, false, &getUndoManager ())
  
  std::unique_ptr <xmlNode, void (*)(xmlNodePtr)> node (
                     xmlNewNode (NULL, BAD_CAST2 ("addAction")), xmlFreeNode);
  
  BUGCRIT (node.get (),
           false,
           &getUndoManager (),
           gettext ("Erreur d'allocation mémoire.\n"))
  
  BUGCRIT (xmlSetProp (node.get (),
                       BAD_CAST2 ("Nom"),
                       BAD_CAST2 (nom_->c_str ())),
           false,
           &getUndoManager (),
           gettext ("Problème depuis la librairie : %s\n"), "xml2")
  
  BUGCRIT (xmlSetProp (node.get (),
                       BAD_CAST2 ("Type"),
                       BAD_CAST2 (CAction::getDescription (type_).c_str ())),
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
 * \brief Renvoie le type de l'action.
 */
uint8_t
CAction::getType () const
{
  return type;
}


/**
 * \brief Converti la fonction de modification du psi d'une action sous format
 *        XML.
 * \param nom_ Le nom de l'action.
 * \param psi Le coefficient psi à changer (0, 1 ou 2).
 * \param psin Le coefficient psi à convertir.
 * \param root Le noeud dans lequel doit être inséré la branche.
 */
bool CHK
CAction::setpsiXML (std::string * const nom_,
                    uint8_t             psi,
                    INb                *psin,
                    xmlNodePtr          root)
{
  BUGPARAM (root, "%p", root, false, &getUndoManager ())
  BUGPARAM (psi, "%u", psi <= 2, false, &getUndoManager ())
  
  std::unique_ptr <xmlNode, void (*)(xmlNodePtr)> node (
                         xmlNewNode (NULL, BAD_CAST2 ("setpsi")), xmlFreeNode);
  
  BUGCRIT (node.get (),
           false,
           &getUndoManager (),
           gettext ("Erreur d'allocation mémoire.\n"))
  
  BUGCRIT (xmlSetProp (node.get (),
                       BAD_CAST2 ("psi"),
                       BAD_CAST2 (psi == 0 ? "0" : psi == 1 ? "1" : "2")),
           false,
           &getUndoManager (),
           gettext ("Problème depuis la librairie : %s\n"), "xml2")
  
  BUGCRIT (xmlSetProp (node.get (),
                       BAD_CAST2 ("Nom"),
                       BAD_CAST2 (nom_->c_str ())),
           false,
           &getUndoManager (),
           gettext ("Problème depuis la librairie : %s\n"), "xml2")
  
  std::unique_ptr <xmlNode, void (*)(xmlNodePtr)> node0 (
                            xmlNewNode (NULL, BAD_CAST2 ("val")), xmlFreeNode);
  
  BUGCRIT (node0.get (),
           false,
           &getUndoManager (),
           gettext ("Erreur d'allocation mémoire.\n"))
  
  BUGCONT (psin->newXML (node0.get ()), false, &getUndoManager ())
  
  BUGCRIT (xmlAddChild (node.get (), node0.get ()),
           false,
           &getUndoManager (),
           gettext ("Problème depuis la librairie : %s\n"), "xml2")
  node0.release ();
  
  BUGCRIT (xmlAddChild (root, node.get ()),
           false,
           &getUndoManager (),
           gettext ("Problème depuis la librairie : %s\n"), "xml2")
  node.release ();
  
  return true;
}


/**
 * \brief Renvoie le cœfficient psi0.
 */
INb const &
CAction::getpsi0 () const
{
  return *psi0;
}


/**
 * \brief Défini le cœfficient psi0.
 * \param val Le nouveau cœfficient. val vaut NULL lors de la création de
 *        l'action.
 */
bool CHK
CAction::setpsi0 (INb * val)
{
  if (val != NULL)
  {
    BUGUSER (val->getUnite () == U_,
             false,
             &this->getUndoManager (),
             gettext ("L'unité est de type [%s] à la place de [%s].\n"),
               EUniteConst[val->getUnite ()].c_str (),
               EUniteConst[0].c_str ())
  }
  
  BUGCONT (getUndoManager ().ref (), false, &getUndoManager ())
  
  BUGCONT (getUndoManager ().push (
             std::bind (&CAction::setpsi0, this, psi0),
             std::bind (&CAction::setpsi0, this, val),
             std::bind (std::default_delete <INb> (), val),
             std::bind (&CAction::setpsiXML,
                        this,
                        getNom (),
                        0,
                        val,
                        std::placeholders::_1)),
           false,
           &getUndoManager ())
  psi0 = val;
  
  BUGCONT (getUndoManager ().unref (), false, &getUndoManager ())
  
  return true;
}


/**
 * \brief Renvoie le cœfficient psi1.
 */
INb const &
CAction::getpsi1 () const
{
  return *psi1;
}


/**
 * \brief Défini le cœfficient psi1.
 * \param val Le nouveau cœfficient. val vaut NULL lors de la création de
 *        l'action.
 */
bool CHK
CAction::setpsi1 (INb * val)
{
  if (val != NULL)
  {
    BUGUSER (val->getUnite () == U_,
             false,
             &this->getUndoManager (),
             gettext ("L'unité est de type [%s] à la place de [%s].\n"),
               EUniteConst[val->getUnite ()].c_str (),
               EUniteConst[0].c_str ())
  }
  
  BUGCONT (getUndoManager ().ref (), false, &getUndoManager ())
  
  BUGCONT (getUndoManager ().push (
             std::bind (&CAction::setpsi1, this, psi1),
             std::bind (&CAction::setpsi1, this, val),
             std::bind (std::default_delete <INb> (), val),
             std::bind (&CAction::setpsiXML,
                        this,
                        getNom (),
                        1,
                        val,
                        std::placeholders::_1)),
           false,
           &getUndoManager ())
  psi1 = val;
  
  BUGCONT (getUndoManager ().unref (), false, &getUndoManager ())
  
  return true;
}


/**
 * \brief Renvoie le cœfficient psi2.
 */
INb const &
CAction::getpsi2 () const
{
  return *psi2;
}


/**
 * \brief Défini le cœfficient psi2.
 * \param val Le nouveau cœfficient. val vaut NULL lors de la création de
 *        l'action.
 */
bool CHK
CAction::setpsi2 (INb * val)
{
  if (val != NULL)
  {
    BUGUSER (val->getUnite () == U_,
             false,
             &this->getUndoManager (),
             gettext ("L'unité est de type [%s] à la place de [%s].\n"),
               EUniteConst[val->getUnite ()].c_str (),
               EUniteConst[0].c_str ())
  }
  
  BUGCONT (getUndoManager ().ref (), false, &getUndoManager ())
  
  BUGCONT (getUndoManager ().push (
             std::bind (&CAction::setpsi2, this, psi2),
             std::bind (&CAction::setpsi2, this, val),
             std::bind (std::default_delete <INb> (), val),
             std::bind (&CAction::setpsiXML,
                        this,
                        getNom (),
                        2,
                        val,
                        std::placeholders::_1)),
           false,
           &getUndoManager ())
  
  psi2 = val;
  
  BUGCONT (getUndoManager ().unref (), false, &getUndoManager ())
  
  return true;
}


/**
 * \brief Renvoie true si aucune charge n'est présente.
 */
bool
CAction::emptyCharges () const
{
  return charges.empty ();
}


/**
 * \brief Renvoie le type de l'action sous forme de texte.
 */
std::string const
CAction::getDescription (uint8_t type_) const
{
  return parametres->getpsiDescription (type_);
}


/**
 * \brief Défini la norme que doit utiliser l'action. Ne nécessite pas de
 *        fonction XML puisqu'elle ne doit être appelée que depuis la fonction
 *        CProjet::setParametres.
 * \param param Le type IParametres.
 * \param psi0_ Le coefficient psi0.
 * \param psi1_ Le coefficient psi1.
 * \param psi2_ Le coefficient psi2.
 */
bool CHK
CAction::setParam (IParametres * param,
                   INb         * psi0_,
                   INb         * psi1_,
                   INb         * psi2_)
{
  bool ins = getUndoManager ().getInsertion ();
  
  BUGCONT (getUndoManager ().ref (), false, &getUndoManager ())
  
  parametres = param;
  
  getUndoManager ().setInsertion (false);
  
  BUGCONT (setpsi0 (psi0_), false, &getUndoManager ())
  BUGCONT (setpsi1 (psi1_), false, &getUndoManager ())
  BUGCONT (setpsi2 (psi2_), false, &getUndoManager ())
  
  getUndoManager ().setInsertion (ins);
  
  BUGCONT (getUndoManager ().push (
             std::bind (&CAction::setParam,
                        this,
                        parametres,
                        psi0,
                        psi1,
                        psi2),
             std::bind (&CAction::setParam, this, param, psi0_, psi1_, psi2_),
             std::bind (std::default_delete <INb> (), psi0_),
             std::bind (&CAction::setParamXML,
                        this,
                        this->getNom (),
                        param->getNom (),
                        psi0_,
                        psi1_,
                        psi2_,
                        std::placeholders::_1)),
           false,
           &getUndoManager ())
  
  BUGCONT (getUndoManager ().pushSuppr (std::bind (std::default_delete <INb>
                                                                            (),
                                        psi1_)),
           false,
           &getUndoManager ())
  BUGCONT (getUndoManager ().pushSuppr (std::bind (std::default_delete <INb>
                                                                            (),
                                        psi2_)),
           false,
           &getUndoManager ())
  
  BUGCONT (getUndoManager ().unref (), false, &getUndoManager ())
  
  return true;
}


/**
 * \brief Converti la fonction setParam en format XML.
 * \param action Le nom de l'action.
 * \param param Le nom des paramètres.
 * \param root Le noeud dans lequel doit être inséré la branche.
 */
bool CHK
CAction::setParamXML (std::string * action,
                      std::string * param,
                      INb         * psi0_,
                      INb         * psi1_,
                      INb         * psi2_,
                      xmlNodePtr    root)
{
  BUGPARAM (root, "%p", root, false, &getUndoManager ())
  
  std::unique_ptr <xmlNode, void (*)(xmlNodePtr)> node (
                 xmlNewNode (NULL, BAD_CAST2 ("actionSetParam")), xmlFreeNode);
  
  BUGCRIT (node.get (),
           false,
           &getUndoManager (),
           gettext ("Erreur d'allocation mémoire.\n"))
  
  BUGCRIT (xmlSetProp (node.get (),
                       BAD_CAST2 ("Action"),
                       BAD_CAST2 (action->c_str ())),
           false,
           &getUndoManager (),
           gettext ("Problème depuis la librairie : %s\n"), "xml2")
  
  BUGCRIT (xmlSetProp (node.get (),
                       BAD_CAST2 ("Param"),
                       BAD_CAST2 (param->c_str ())),
           false,
           &getUndoManager (),
           gettext ("Problème depuis la librairie : %s\n"), "xml2")
  
  std::unique_ptr <xmlNode, void (*)(xmlNodePtr)> node0 (
                           xmlNewNode (NULL, BAD_CAST2 ("psi0")), xmlFreeNode);
  
  BUGCRIT (node0.get (),
           false,
           &getUndoManager (),
           gettext ("Erreur d'allocation mémoire.\n"))
  
  BUGCONT (psi0_->newXML (node0.get ()), false, &getUndoManager ())
  
  BUGCRIT (xmlAddChild (node.get (), node0.get ()),
           false,
           &getUndoManager (),
           gettext ("Problème depuis la librairie : %s\n"), "xml2")
  node0.release ();
  
  node0.reset (xmlNewNode (NULL, BAD_CAST2 ("psi1")));
  
  BUGCRIT (node0.get (),
           false,
           &getUndoManager (),
           gettext ("Erreur d'allocation mémoire.\n"))
  
  BUGCONT (psi1_->newXML (node0.get ()), false, &getUndoManager ())
  
  BUGCRIT (xmlAddChild (node.get (), node0.get ()),
           false,
           &getUndoManager (),
           gettext ("Problème depuis la librairie : %s\n"), "xml2")
  node0.release ();
  
  node0.reset (xmlNewNode (NULL, BAD_CAST2 ("psi2")));
  
  BUGCRIT (node0.get (),
           false,
           &getUndoManager (),
           gettext ("Erreur d'allocation mémoire.\n"))
  
  BUGCONT (psi2_->newXML (node0.get ()), false, &getUndoManager ())
  
  BUGCRIT (xmlAddChild (node.get (), node0.get ()),
           false,
           &getUndoManager (),
           gettext ("Problème depuis la librairie : %s\n"), "xml2")
  node0.release ();
  
  BUGCRIT (xmlAddChild (root, node.get ()),
           false,
           &getUndoManager (),
           gettext ("Problème depuis la librairie : %s\n"), "xml2")
  node.release ();
  
  return true;
}


/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
