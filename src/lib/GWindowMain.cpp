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
#include <libintl.h>
#include <gtkmm/menutoolbutton.h>
#include <gtkmm/applicationwindow.h>

#include "GWindowMain.hpp"
#include "MErreurs.hpp"

GWindowMain::GWindowMain (Glib::RefPtr <Gtk::Builder> & builder,
                          CProjet & proj) :
  build (builder),
  projet (proj)
{
  Gtk::MenuToolButton * button = nullptr;

  build->get_widget ("buttonUndo", button);

  BUGPROG (button,
           ,
           UNDO_MANAGER_NULL,
           gettext ("Fenêtre %s, composant %s introuvable.\n"),
             "Main",
             "buttonUndo");

  button->signal_clicked ().connect (sigc::mem_fun (
                                          *this, &GWindowMain::onClickedUndo));
  signal (EEvent::UNDO_NB, nullptr);

  build->get_widget ("buttonRedo", button);

  BUGPROG (button,
           ,
           UNDO_MANAGER_NULL,
           gettext ("Fenêtre %s, composant %s introuvable.\n"),
             "Main",
             "buttonRedo");

  button->signal_clicked ().connect (sigc::mem_fun (
                                          *this, &GWindowMain::onClickedRedo));
  signal (EEvent::REDO_NB, nullptr);
}

GWindowMain::~GWindowMain()
{
  Gtk::ApplicationWindow* pDialog = nullptr;

  build->get_widget ("window1", pDialog);
  delete pDialog;
}

void
GWindowMain::signal (EEvent event, void * param)
{
  switch (event)
  {
    case EEvent::UNDO_NB :
    {
      Gtk::MenuToolButton * undoB = nullptr;

      build->get_widget ("buttonUndo", undoB);

      BUGPROG (undoB,
               ,
               UNDO_MANAGER_NULL,
               gettext ("Fenêtre %s, composant %s introuvable.\n"),
                 "Main",
                 "buttonUndo");

      undoB->set_sensitive (projet.undoNb () != 0);
      if (projet.undoNb () != 0)
      {
        undoB->set_tooltip_text (*projet.undoDesc (0));
      }
      else
      {
        undoB->set_tooltip_text ("");
      }
      break;
    }
    case EEvent::REDO_NB :
    {
      Gtk::MenuToolButton * redoB = nullptr;

      build->get_widget ("buttonRedo", redoB);

      BUGPROG (redoB,
               ,
               UNDO_MANAGER_NULL,
               gettext ("Fenêtre %s, composant %s introuvable.\n"),
                 "Main",
                 "buttonRedo");

      redoB->set_sensitive (projet.redoNb () != 0);
      if (projet.redoNb () != 0)
      {
        redoB->set_tooltip_text (*projet.redoDesc (0));
      }
      else
      {
        redoB->set_tooltip_text ("");
      }
      break;
    }
    default :
    {
      BUGPROG (nullptr, , UNDO_MANAGER_NULL, gettext ("Signal inconnu.\n"));
    }
  }
}

void
GWindowMain::onClickedUndo ()
{
  projet.undo ();
}

void
GWindowMain::onClickedRedo ()
{
  projet.redo ();
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
