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
#include <algorithm>
#include <libintl.h>
#include <gtkmm/menutoolbutton.h>
#include <gtkmm/applicationwindow.h>

#include "GWindowMain.hpp"
#include "MErreurs.hpp"
#include "common/GtkMenu.hpp"

GWindowMain::GWindowMain (Glib::RefPtr <Gtk::Builder> & builder,
                          CModele                     & modele_) :
  build (builder),
  modele (modele_)
{
  Gtk::MenuToolButton * button = nullptr;

  build->get_widget ("buttonUndo", button);

  BUGPROG (button,
           ,
           UNDO_MANAGER_NULL,
           "Fenêtre %s, composant %s introuvable.\n",
             "Main",
             "buttonUndo");

  button->signal_clicked ().connect (
                           sigc::mem_fun (*this, &GWindowMain::onClickedUndo));
  button->signal_show_menu  ().connect (
                          sigc::mem_fun (*this, &GWindowMain::onShowMenuUndo));
  signal (EEvent::UNDO_NB, nullptr);

  build->get_widget ("buttonRedo", button);

  BUGPROG (button,
           ,
           UNDO_MANAGER_NULL,
           "Fenêtre %s, composant %s introuvable.\n",
             "Main",
             "buttonRedo");

  button->signal_clicked ().connect (
                           sigc::mem_fun (*this, &GWindowMain::onClickedRedo));
  button->signal_show_menu  ().connect (
                          sigc::mem_fun (*this, &GWindowMain::onShowMenuRedo));
  signal (EEvent::REDO_NB, nullptr);
}

GWindowMain::~GWindowMain()
{
  Gtk::ApplicationWindow * pDialog = nullptr;

  build->get_widget ("window1", pDialog);
  delete pDialog;
}

void
GWindowMain::signal (EEvent event, void *)
{
  switch (event)
  {
    case EEvent::UNDO_NB :
    {
      bool undoPossible = modele.getUndoManager ().undoNb () != 0;

      // On récupère le composant buttonUndo
      Gtk::MenuToolButton * undoB = nullptr;
      build->get_widget ("buttonUndo", undoB);
      BUGPROG (undoB,
               ,
               UNDO_MANAGER_NULL,
               "Fenêtre %s, composant %s introuvable.\n",
                 "Main",
                 "buttonUndo");

      undoB->set_sensitive (undoPossible);

      if (undoPossible)
      {
        undoB->set_tooltip_text (*modele.getUndoManager ().undoDesc (0));
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
      bool redoPossible = modele.getUndoManager ().redoNb () != 0;

      build->get_widget ("buttonRedo", redoB);

      BUGPROG (redoB,
               ,
               UNDO_MANAGER_NULL,
               "Fenêtre %s, composant %s introuvable.\n",
                 "Main",
                 "buttonRedo");

      redoB->set_sensitive (redoPossible);
      if (redoPossible)
      {
        redoB->set_tooltip_text (*modele.getUndoManager ().redoDesc (0));
      }
      else
      {
        redoB->set_tooltip_text ("");
      }
      break;
    }
    default :
    {
      BUGPROG (false,
               ,
               UNDO_MANAGER_NULL,
               "Signal %zu inconnu.\n",
                 static_cast <size_t> (event));
    }
  }
}

void
GWindowMain::onClickedUndo ()
{
  BUGCONT (modele.getUndoManager ().undo (), , UNDO_MANAGER_NULL);
}

void
GWindowMain::onShowMenuUndo ()
{
  Gtk::Menu * undoM = nullptr;
  build->get_widget ("menuUndo", undoM);
  BUGPROG (undoM,
           ,
           UNDO_MANAGER_NULL,
           "Fenêtre %s, composant %s introuvable.\n",
             "Main",
             "menuUndo");

  GtkMenuC::clear (*undoM);

  // On ajoute les nouveaux items.
  size_t nbUndo = modele.getUndoManager ().undoNb ();
  for (size_t i = 0; i < std::min(100UL, nbUndo); i++)
  {
    Gtk::MenuItem * menuItem = new Gtk::MenuItem (
                                 *modele.getUndoManager ().undoDesc (i));
    undoM->append (*menuItem);
  }
  undoM->show_all ();
}

void
GWindowMain::onClickedRedo ()
{
  BUGCONT (modele.getUndoManager ().redo (), , UNDO_MANAGER_NULL);
}

void
GWindowMain::onShowMenuRedo ()
{
  Gtk::Menu * redoM = nullptr;
  build->get_widget ("menuRedo", redoM);
  BUGPROG (redoM,
           ,
           UNDO_MANAGER_NULL,
           "Fenêtre %s, composant %s introuvable.\n",
             "Main",
             "menuRedo");

  GtkMenuC::clear (*redoM);

  // On ajoute les nouveaux items.
  size_t nbRedo = modele.getUndoManager ().redoNb ();
  for (size_t i = 0; i < std::min(100UL, nbRedo); i++)
  {
    Gtk::MenuItem * menuItem = new Gtk::MenuItem (
                                       *modele.getUndoManager ().redoDesc (i));
    redoM->append (*menuItem);
  }
  redoM->show_all ();
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
