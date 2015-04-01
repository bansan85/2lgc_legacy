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

#include <stdint.h>
#include <libintl.h>
#include <string.h>
#include <memory>
#include <iostream>
#include <cassert>

#include "CProjet.hpp"
#include "MErreurs.hpp"


int
main (int32_t argc,
      char   *argv[])
{
  /* Variables */
  CProjet projet (ENorme::EUROCODE);
  
  // On charge la localisation
  setlocale (LC_ALL, "" );
  bindtextdomain (PACKAGE_NAME, LOCALEDIR);
  bind_textdomain_codeset (PACKAGE_NAME, "UTF-8");
  textdomain (PACKAGE_NAME);
  
  // On traite les arguments du programme
  switch (argc)
  {
    case 2:
    {
      if ((strcmp (argv[1], "-w") == 0) ||
          (strcmp (argv[1], "--warranty") == 0))
      {
        CProjet::showWarranty ();
        return 0;
      }
      else if ((strcmp (argv[1], "-h") == 0) ||
               (strcmp (argv[1], "--help") == 0))
      {
        CProjet::showHelp ();
        return 0;
      }
      break;
    }
    default:
    {
      break;
    }
  }

  projet.enregistre ("eeee.xml");
  std::shared_ptr <CAction> action;
  action = std::make_shared <CAction> (std::make_shared <std::string> (
                                                               "Poids propre"),
                                       0,
                                       projet);
  assert (projet.addAction (action));
  projet.gShowMain (argc, argv);

  // Affichage de l'interface graphique
//  gtk_widget_show_all (projet->ui.comp.window);
//  gtk_main ();
  
  return 0;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
