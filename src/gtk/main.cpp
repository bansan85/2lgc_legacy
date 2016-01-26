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

#include <gtkmm/application.h>
#include <gtkmm/builder.h>
#include <gtkmm/applicationwindow.h>

#include "gtk-ressources.h"
#include "GWindowMain.hpp"
#include "CModele.hpp"
#include "MErreurs.hpp"
#include "MUndo.hpp"

bool
gShowMain (int32_t   argc,
           char    * argv[],
           CModele & modele)
{
  Glib::RefPtr <Gtk::Application> app =
                                 Gtk::Application::create (argc,
                                                           argv,
                                                           "org.llgc.codegui");

  Glib::RefPtr <Gtk::Builder> builder;
  Gtk::ApplicationWindow    * pDialog = nullptr;

  try
  {
    builder = Gtk::Builder::create_from_resource (
                                               "/org/2lgc/codegui/ui/main.ui");
  }
  catch (...)
  {
    BUGCRIT (false,
             false,
             UNDO_MANAGER_NULL,
             "Échec lors de la création de la fenêtre %s\n",
               "main")
  }

  modele.getUndoManager().addObserver (
                             std::make_shared <GWindowMain> (builder, modele));

  builder->get_widget ("window1", pDialog);
  app->run (*pDialog);

  return true;
}

int
main (int32_t argc,
      char   *argv[])
{
  /* Variables */
  CModele projet (ENorme::EUROCODE);
  
  // On traite les arguments du programme
  switch (argc)
  {
    case 2:
    {
      if ((strcmp (argv[1], "-w") == 0) ||
          (strcmp (argv[1], "--warranty") == 0))
      {
        CModele::showWarranty ();
        return 0;
      }
      else if ((strcmp (argv[1], "-h") == 0) ||
               (strcmp (argv[1], "--help") == 0))
      {
        CModele::showHelp ();
        return 0;
      }
      break;
    }
    default:
    {
      break;
    }
  }

  _2lgc_register_resource ();

  std::shared_ptr <POCO::sol::CAction> action;
  action = std::make_shared <POCO::sol::CAction> (
                               std::make_shared <std::string> ("Poids propre"),
                               0);
  assert (projet.fAction.doAdd (action));
  action = std::make_shared <POCO::sol::CAction> (
                               std::make_shared <std::string> ("Poids propr2"),
                               1);
  assert (projet.fAction.doAdd (action));
  BUGCONT (gShowMain (argc, argv, projet), -1, UNDO_MANAGER_NULL);

  _2lgc_unregister_resource ();

  return 0;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
