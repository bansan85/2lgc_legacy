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

#include <algorithm>
#include <memory>
#include <iostream>

#include "CModele.hpp"
#include "MErreurs.hpp"
#include "SString.hpp"
#include "EUnite.hpp"
#include "POCO/nombre/Calcul.hpp"
#include "norme/Eurocode.hpp"

static std::string errorMessage;

CModele::CModele (ENorme eNorme) :
  appuis (),
  noeuds (),
  sections (),
  materiaux (),
  relachements (),
  barres (),
  actions (),
  niveaux_groupes (),
  norme (nullptr),
  preferences (),
  undoManager (),
  fAction (*this),
  fNorme (*this)
{
  LIBXML_TEST_VERSION

  // On charge la localisation
  setlocale (LC_ALL, nullptr);
  std::locale foo ("");
  std::cout.imbue (foo);
  std::cin.imbue (foo);
  std::cerr.imbue (foo);
  bindtextdomain (PACKAGE_NAME, LOCALEDIR);
  bind_textdomain_codeset (PACKAGE_NAME, "UTF-8");
  textdomain (PACKAGE_NAME);

  switch (eNorme) {
    case ENorme::EUROCODE :
    {
      std::shared_ptr <INorme> norme_;
      norme_ = std::make_shared <norme::Eurocode> (
                                   std::make_shared <std::string> ("Eurocode"),
                                   norme::ENormeEcAc::FR,
                                   0);
      if (!undoManager.ref (false))
      {
        errorMessage = "Erreur lors de la sélection de la norme Eurocode.";
        throw errorMessage.c_str ();
      }
      if (!fNorme.doSet(norme_))
      {
        errorMessage = "Erreur lors de la sélection de la norme Eurocode.";
        throw errorMessage.c_str ();
      }
      if (!undoManager.unref ())
      {
        errorMessage = "Erreur lors de la sélection de la norme Eurocode.";
        throw errorMessage.c_str ();
      }
      break;
    }
    default :
    {
      errorMessage = "Erreur lors de la sélection de la norme" +
                           std::to_string (static_cast<size_t> (eNorme)) + ".";
      throw errorMessage.c_str ();
      break;
    }
  }
}

CModele::~CModele ()
{
  xmlCleanupParser ();
}

std::shared_ptr <POCO::sol::CAction> *
CModele::getAction (const std::string & nom)
{
  std::list <std::shared_ptr <POCO::sol::CAction> >::iterator it;

  it = std::find_if (actions.begin (),
                     actions.end (),
                     [&nom] (const std::shared_ptr <POCO::sol::CAction> &
                                                                        action)
                     {
                       return nom.compare (*action.get ()->getNom ()) == 0;
                     });
  
  if (it != actions.end ())
  {
    return &(*it);
  }
  else
  {
    return nullptr;
  }
}

size_t
CModele::getActionCount () const
{
  return actions.size ();
}

const std::shared_ptr <INorme> &
CModele::getNorme () const
{
  return norme;
}

UndoManager &
CModele::getUndoManager ()
{
  return undoManager;
}

bool CHK
CModele::enregistre (const std::string fichier) const
{
  std::unique_ptr <xmlDoc, void (*)(xmlDocPtr)> doc (
                                    xmlNewDoc (BAD_CAST2 ("1.0")), xmlFreeDoc);
  xmlNodePtr root_node;
  
  BUGCRIT (doc.get () != nullptr,
           false,
           UNDO_MANAGER_NULL,
           "Erreur d'allocation mémoire.\n")
  
  root_node = xmlNewNode (nullptr, BAD_CAST2 ("projet"));
  BUGCRIT (root_node != nullptr,
           false,
           UNDO_MANAGER_NULL,
           "Erreur d'allocation mémoire.\n")

  norme::Eurocode *normeEC = dynamic_cast <norme::Eurocode *> (norme.get());
  if (normeEC != nullptr)
  {
    // libxml2 ne permet pas de récupérer les attributs du nœud racine.
/*    std::unique_ptr <xmlNode, void (*)(xmlNodePtr)> node0 (
                                     xmlNewNode (nullptr, BAD_CAST2 ("norme")),
                                     xmlFreeNode);
    
    BUGCRIT (node0.get () != nullptr,
             false,
             UNDO_MANAGER_NULL,
             "Erreur d'allocation mémoire.\n")*/
    
    BUGCRIT (xmlSetProp (root_node/*node0.get ()*/,
                         BAD_CAST2 ("norme"),
                         BAD_CAST2 ("Eurocode")) != nullptr,
             false,
             UNDO_MANAGER_NULL,
             "Problème depuis la librairie : %s\n", "xml2")
    BUGCRIT (xmlSetProp (root_node/*node0.get ()*/,
                         BAD_CAST2 ("variante"),
                         BAD_CAST2 (std::to_string (static_cast <size_t>
                                (normeEC->getVariante ())).c_str ())) != nullptr,
             false,
             UNDO_MANAGER_NULL,
             "Problème depuis la librairie : %s\n", "xml2")
    BUGCRIT (xmlSetProp (root_node/*node0.get ()*/,
                         BAD_CAST2 ("options"),
                         BAD_CAST2 (std::to_string
                                 (norme->getOptions ()).c_str ())) != nullptr,
             false,
             UNDO_MANAGER_NULL,
             "Problème depuis la librairie : %s\n", "xml2")

/*    BUGCRIT (xmlAddChild (root_node, node0.get ()) != nullptr,
             false,
             UNDO_MANAGER_NULL,
             "Problème depuis la librairie : %s\n", "xml2")
             
    node0.release ();*/
  }
  else
  {
    BUGPROG (false,
             false,
             UNDO_MANAGER_NULL,
             "La norme \"%s\" est inconnue.\n", norme->getNom ()->c_str ())
  }

  
  xmlDocSetRootElement (doc.get (), root_node);
  
  BUGCONT (undoManager.undoToXML (root_node), false, UNDO_MANAGER_NULL)
  xmlSetCompressMode (0);
  
  BUGUSER (xmlSaveFormatFile (fichier.c_str (),
                              doc.get (),
                              1) != -1,
           false,
           UNDO_MANAGER_NULL,
           gettext ("Échec lors de l'enregistrement.\n"))
  
  return true;
}

void
CModele::showWarranty ()
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
CModele::showHelp ()
{
  std::cout << gettext ("Utilisation : codegui [OPTION]... [FILE]...\n")
    << gettext ("Options :\n")
    << gettext ("\t-h, --help : affiche le présent menu\n")
    << gettext ("\t-w, --warranty : affiche les limites de garantie du logiciel\n");
  
  return;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
