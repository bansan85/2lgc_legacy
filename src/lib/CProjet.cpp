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

#include <iostream>
#include <locale>
#include <memory>

#include "CParamEC.hpp"
#include "CProjet.hpp"
#include "MErreurs.hpp"
#include "SString.hpp"

#ifdef ENABLE_GTK
#include "gtk-ressources.h"
#include <gtkmm/application.h>
#include <gtkmm/builder.h>
#include <gtkmm/applicationwindow.h>
#endif

CProjet::CProjet (ENorme norme) :
  CPreferences (),
  CCalculs (),
  CModele (),
  UndoManager (),
  parametres (nullptr)
{
  LIBXML_TEST_VERSION
  
  switch (norme)
  {
    case ENorme::EUROCODE :
    {
      std::shared_ptr <CParamEC> param
        (std::make_shared <CParamEC> (std::make_shared <std::string> (gettext ("Eurocode, annexe nationale")),
                                      ENormeEcAc::FR,
                                      0,
                                      *this));
      
      if (!setParametres (param))
      {
        throw gettext ("Impossible de créer ce projet. Echec lors de la sélection de la norme.\n");
      }
      
      break;
    }
    default :
    {
      throw gettext ("Impossible de créer ce projet. La norme est inconnue.\n");
    }
  }

#ifdef ENABLE_GTK
  _2lgc_register_resource ();
#endif
}

CProjet::~CProjet ()
{
  xmlCleanupParser();
  
#ifdef ENABLE_GTK
  _2lgc_unregister_resource ();
#endif
}

std::shared_ptr <IParametres> &
CProjet::getParametres ()
{
  return parametres;
}

bool
CProjet::setParametres (std::shared_ptr <IParametres> param)
{
  BUGCONT (ref (), false, this)
  
  BUGCONT (push (
             std::bind (&CProjet::setParametres, this, parametres),
             std::bind (&CProjet::setParametres, this, param),
             nullptr,
             std::bind (&CProjet::setParametresXML,
                        this,
                        param.get (),
                        param->getNom ().get (),
                        param->getVariante (),
                        std::placeholders::_1),
             format (gettext ("Paramètres du projet (%s)"),
                     param.get ()->getNom ()->c_str ())),
           false,
           this)
  
  BUGCONT (pushSuppr (param->getNom ()),
           false,
           this)
  
  parametres = param;
  
  BUGCONT (unref (), false, this)
  
  return true;
}

bool
CProjet::setParametresXML (IParametres *param,
                           std::string *nom,
                           uint32_t     variante,
                           xmlNodePtr   root)
{
  BUGPARAM (static_cast <void *> (root), "%p", root, false, this)
  
  std::unique_ptr <xmlNode, void (*)(xmlNodePtr)> node (
              xmlNewNode (nullptr, BAD_CAST2 ("projetSetParam")), xmlFreeNode);
  
  BUGCRIT (node.get (),
           false,
           this,
           gettext ("Erreur d'allocation mémoire.\n"))
  
  if (dynamic_cast <CParamEC *> (param) != NULL)
  {
    BUGCRIT (xmlSetProp (node.get (), BAD_CAST2 ("Type"), BAD_CAST2 ("EC")),
             false,
             this,
             gettext ("Problème depuis la librairie : %s\n"), "xml2")
  }
  else
  {
    BUGPROG (nullptr, false, this, gettext ("Le type de la norme est inconnu.\n"))
  }
  
  BUGCRIT (xmlSetProp (node.get (),
                       BAD_CAST2 ("Nom"),
                       BAD_CAST2 (nom->c_str ())),
           false,
           this,
           gettext ("Problème depuis la librairie : %s\n"), "xml2")
  
  BUGCRIT (xmlSetProp (node.get (),
                       BAD_CAST2 ("Variante"),
                       BAD_CAST2 (std::to_string (variante).c_str ())),
           false,
           this,
           gettext ("Problème depuis la librairie : %s\n"), "xml2")
  
  BUGCRIT (xmlAddChild (root, node.get ()),
           false,
           this,
           gettext ("Problème depuis la librairie : %s\n"), "xml2")
  
  node.release ();
  
  return true;
}

bool
CProjet::enregistre (std::string fichier)
{
  std::unique_ptr <xmlDoc, void (*)(xmlDocPtr)> doc (
                                    xmlNewDoc (BAD_CAST2 ("1.0")), xmlFreeDoc);
  xmlNodePtr root_node;
  
  BUGCRIT (doc.get (),
           false,
           this,
           gettext ("Erreur d'allocation mémoire.\n"))
  
  BUGCRIT (root_node = xmlNewNode (nullptr, BAD_CAST2 ("Projet")),
           false,
           this,
           gettext ("Erreur d'allocation mémoire.\n"))
  
  xmlDocSetRootElement (doc.get (), root_node);
  
  BUGCONT (undoToXML (root_node), false, this)
  xmlSetCompressMode (0);
  
  BUGUSER (xmlSaveFormatFile (fichier.c_str (),
                              doc.get (),
                              1) != -1,
           false,
           this,
           gettext ("Échec lors de l'enregistrement.\n"))
  
  return true;
}

void
CProjet::showWarranty ()
{
  std::cout << gettext ("15. Disclaimer of Warranty.\n")
    << gettext ("\n")
    << gettext ("THERE IS NO WARRANTY FOR THE PROGRAM, TO THE EXTENT PERMITTED BY\n")
    << gettext ("APPLICABLE LAW.  EXCEPT WHEN OTHERWISE STATED IN WRITING THE COPYRIGHT\n")
    << gettext ("HOLDERS AND/OR OTHER PARTIES PROVIDE THE PROGRAM \"AS IS\" WITHOUT WARRANTY\n")
    << gettext ("OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO,\n")
    << gettext ("THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR\n")
    << gettext ("PURPOSE.  THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE PROGRAM\n")
    << gettext ("IS WITH YOU.  SHOULD THE PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF\n")
    << gettext ("ALL NECESSARY SERVICING, REPAIR OR CORRECTION.\n")
    << gettext ("\n")
    << gettext ("16. Limitation of Liability.\n")
    << gettext ("\n")
    << gettext ("IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN WRITING\n")
    << gettext ("WILL ANY COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO MODIFIES AND/OR CONVEYS\n")
    << gettext ("THE PROGRAM AS PERMITTED ABOVE, BE LIABLE TO YOU FOR DAMAGES, INCLUDING ANY\n")
    << gettext ("GENERAL, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE\n")
    << gettext ("USE OR INABILITY TO USE THE PROGRAM (INCLUDING BUT NOT LIMITED TO LOSS OF\n")
    << gettext ("DATA OR DATA BEING RENDERED INACCURATE OR LOSSES SUSTAINED BY YOU OR THIRD\n")
    << gettext ("PARTIES OR A FAILURE OF THE PROGRAM TO OPERATE WITH ANY OTHER PROGRAMS),\n")
    << gettext ("EVEN IF SUCH HOLDER OR OTHER PARTY HAS BEEN ADVISED OF THE POSSIBILITY OF\n")
    << gettext ("SUCH DAMAGES.\n")
    << gettext ("\n")
    << gettext ("17. Interpretation of Sections 15 and 16.\n")
    << gettext ("\n")
    << gettext ("If the disclaimer of warranty and limitation of liability provided\n")
    << gettext ("above cannot be given local legal effect according to their terms,\n")
    << gettext ("reviewing courts shall apply local law that most closely approximates\n")
    << gettext ("an absolute waiver of all civil liability in connection with the\n")
    << gettext ("Program, unless a warranty or assumption of liability accompanies a\n")
    << gettext ("copy of the Program in return for a fee.\n");
  
  return;
}

void
CProjet::showHelp ()
{
  std::cout << gettext ("Utilisation : codegui [OPTION]... [FILE]...\n")
    << gettext ("Options :\n")
    << gettext ("\t-h, --help : affiche le présent menu\n")
    << gettext ("\t-w, --warranty : affiche les limites de garantie du logiciel\n");
  
  return;
}

bool
CProjet::gShowMain (int32_t argc,
                    char   *argv[])
{
#ifdef ENABLE_GTK
  Glib::RefPtr <Gtk::Application> app = Gtk::Application::create (
                                               argc, argv, "org.llgc.codegui");
  Glib::RefPtr <Gtk::Builder> builder;
  Gtk::ApplicationWindow* pDialog = nullptr;

  try
  {
    builder = Gtk::Builder::create_from_resource (
                                               "/org/2lgc/codegui/ui/main.ui");
  }
  catch (...)
  {
    BUGCRIT (NULL,
             false,
             this,
             gettext ("Echec lors de la création de la fenêtre %s\n"),
               "main")
  }

  builder->get_widget ("window1", pDialog);
  app->run (*pDialog);
  delete pDialog;

#endif
  return true;
}

bool
CProjet::gAddAction ()
{
  return true;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
