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
#include <gtkmm/main.h>

#include "GWindowMain.hpp"
#include "MErreurs.hpp"
#include "common/GtkMenu.hpp"

class UndoRedoMenuItem : public Gtk::MenuItem
{
  private :
    GWindowMain & window;
    CModele & modele;
  public :
    UndoRedoMenuItem (GWindowMain & window_, CModele & modele_, const Glib::ustring& label, bool mnemonic = false);
  protected :
    // Quand la souris passe dessus. Ne possède pas de fonction virtual.
    void onSelect ();
    // Quand on valide un item du menu.
    virtual void on_activate ();
};

UndoRedoMenuItem::UndoRedoMenuItem (GWindowMain         & window_,
                                    CModele             & modele_,
                                    const Glib::ustring & label,
                                    bool                  mnemonic) :
  Gtk::MenuItem (label, mnemonic),
  window (window_),
  modele (modele_)
{
  signal_select ().connect (sigc::mem_fun (*this,
                                           &UndoRedoMenuItem::onSelect));
}

void UndoRedoMenuItem::onSelect ()
{
  // Surligne la ligne en cours ainsi que ces précédentes.
  std::vector <Gtk::Widget *>::iterator it;
  it = std::find (window.menuItemsUndoRedo.begin (),
                  window.menuItemsUndoRedo.end (),
                  this);

  BUGPROG (it != window.menuItemsUndoRedo.end (),
           ,
           UNDO_MANAGER_NULL,
           "Impossible de trouver la ligne du menu en cours.\n");

  for_each (window.menuItemsUndoRedo.begin (),
            it+1,
            [] (Gtk::Widget * it2)
            {
              UndoRedoMenuItem * item = static_cast <UndoRedoMenuItem *> (it2);
              item->get_style_context ()->add_provider (
                                      item->window.cssProviderMenuItemUndoRedo,
                                      GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
            }
           );
  // Et on déselectionne les suivantes.
  for_each (it+1,
            window.menuItemsUndoRedo.end (),
            [] (Gtk::Widget * it2)
            {
              UndoRedoMenuItem * item = static_cast <UndoRedoMenuItem *> (it2);
              item->get_style_context ()->remove_provider (
                                     item->window.cssProviderMenuItemUndoRedo);
            }
           );
}

// Valide l'annulation ou le rétablissement des opérations.
void UndoRedoMenuItem::on_activate ()
{
  // On recherche l'indice de la ligne sélectionnée.
  std::vector <Gtk::Widget *>::iterator it;
  it = std::find (window.menuItemsUndoRedo.begin (),
                  window.menuItemsUndoRedo.end (),
                  this);

  BUGPROG (it != window.menuItemsUndoRedo.end (),
           ,
           UNDO_MANAGER_NULL,
           "Impossible de trouver la ligne du menu en cours.\n");


  ssize_t index = std::distance (window.menuItemsUndoRedo.begin (), it);

  BUGPROG (index >= 0,
           ,
           UNDO_MANAGER_NULL,
           "Impossible de trouver la ligne du menu en cours.\n");
  size_t index2 = static_cast <size_t> (index);

  BUGPROG (index2 != window.menuItemsUndoRedo.size (),
           ,
           UNDO_MANAGER_NULL,
           "Impossible de trouver la ligne du menu en cours.\n");

  // Puis on exécute N fois l'opération d'annulation ou de répétition.
  if (window.showUndoRedo)
  {
    BUGCONT (modele.getUndoManager ().redoN (index2 + 1),
             ,
             UNDO_MANAGER_NULL);
  }
  else
  {
    BUGCONT (modele.getUndoManager ().undoN (index2 + 1),
             ,
             UNDO_MANAGER_NULL);
  }

  // Fonction virtuelle, il faut appeler le parent.
  Gtk::MenuItem::on_activate();

  // On finit par purger le menu qui est finalement masqué.
  // L'idéal aurait été de le mettre dans l'évènement lors du masquage
  // du menu mais 1) c'est le même évènement que celui l'affichant et surtout
  // 2) le signal indiquant l'activation d'un MenuItem est envoyé après le
  // masquage du menu. Ainsi, il n'est pas possible de savoir si le menu a été
  // fermé par le caractère d'échappement (par exemple) ou par la sélection
  // d'un MenuItem.
  // On en est donc contrait de purger le menu uniquement en cas de validation.
  // Et donc, par obligation, aussi au début de son affichage.
  Gtk::Menu * menu = nullptr;

  if (window.showUndoRedo)
  {
    window.build->get_widget ("menuRedo", menu);
    BUGPROG (menu,
             ,
             UNDO_MANAGER_NULL,
             "Fenêtre %s, composant %s introuvable.\n",
               "Main",
               "menuRedo");
  }
  else
  {
    window.build->get_widget ("menuUndo", menu);
    BUGPROG (menu,
             ,
             UNDO_MANAGER_NULL,
             "Fenêtre %s, composant %s introuvable.\n",
               "Main",
               "menuUndo");
  }

  GtkMenuC::clear (*menu);

  window.menuItemsUndoRedo.clear ();
}

GWindowMain::GWindowMain (Glib::RefPtr <Gtk::Builder> & builder,
                          CModele                     & modele_) :
  build (builder),
  modele (modele_),
  menuItemsUndoRedo (),
  showUndoRedo (false),
  showMenuUndoRedo (false),
  cssProviderMenuItemUndoRedo (Gtk::CssProvider::create ())
{
  // Activation des évènements autour du bouton Annuler.
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
  button->signal_show_menu ().connect (
                          sigc::mem_fun (*this, &GWindowMain::onShowMenuUndo));
  signal (EEvent::UNDO_NB, nullptr);

  // Activation des évènements autour du bouton Rétablir.
  build->get_widget ("buttonRedo", button);
  BUGPROG (button,
           ,
           UNDO_MANAGER_NULL,
           "Fenêtre %s, composant %s introuvable.\n",
             "Main",
             "buttonRedo");

  button->signal_clicked ().connect (
                           sigc::mem_fun (*this, &GWindowMain::onClickedRedo));
  button->signal_show_menu ().connect (
                          sigc::mem_fun (*this, &GWindowMain::onShowMenuRedo));
  signal (EEvent::REDO_NB, nullptr);

  // Initialisation du graphisme pour les MenuItem devant paraitre sélectionné.
  cssProviderMenuItemUndoRedo->load_from_data (
    "*"
    "{"
    "  background-color: @theme_selected_bg_color;"
    "  color:            @theme_selected_fg_color;"
    "}"
  );
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
    // Actualisation de la sensitivité du bouton d'annulation et
    // de la tooltip.
    case EEvent::UNDO_NB :
    {
      bool undoPossible = modele.getUndoManager ().undoNb () != 0;

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
    // Actualisation de la sensitivité du bouton de répétition et
    // de la tooltip.
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
  // Affichage du menu d'annulation.
  showMenuUndoRedo = !showMenuUndoRedo;

  Gtk::Menu * undoM = nullptr;
  build->get_widget ("menuUndo", undoM);
  BUGPROG (undoM,
           ,
           UNDO_MANAGER_NULL,
           "Fenêtre %s, composant %s introuvable.\n",
             "Main",
             "menuUndo");

  // Si il reste des menuItems, on les purge.
  if (showMenuUndoRedo)
  {
    GtkMenuC::clear (*undoM);

    menuItemsUndoRedo.clear ();
  }

  // On ajoute les nouveaux items, jusqu'à 100.
  size_t nbUndo = modele.getUndoManager ().undoNb ();
  for (size_t i = 0; i < std::min (100UL, nbUndo); i++)
  {
    UndoRedoMenuItem * menuItem = new UndoRedoMenuItem (
                        *this, modele, *modele.getUndoManager ().undoDesc (i));
    undoM->append (*menuItem);
  }
  undoM->show_all ();

  menuItemsUndoRedo = undoM->get_children ();
  showUndoRedo = false;
}

void
GWindowMain::onClickedRedo ()
{
  BUGCONT (modele.getUndoManager ().redo (), , UNDO_MANAGER_NULL);
}

void
GWindowMain::onShowMenuRedo ()
{
  // Affichage du menu de rétablissement.
  showMenuUndoRedo = !showMenuUndoRedo;

  Gtk::Menu * redoM = nullptr;
  build->get_widget ("menuRedo", redoM);
  BUGPROG (redoM,
           ,
           UNDO_MANAGER_NULL,
           "Fenêtre %s, composant %s introuvable.\n",
             "Main",
             "menuRedo");

  // Si il reste des menuItems, on les purge.
  if (showMenuUndoRedo)
  {
    GtkMenuC::clear (*redoM);

    menuItemsUndoRedo.clear ();
  }

  // On ajoute les nouveaux items, jusqu'à 100.
  size_t nbRedo = modele.getUndoManager ().redoNb ();
  for (size_t i = 0; i < std::min (100UL, nbRedo); i++)
  {
    UndoRedoMenuItem * menuItem = new UndoRedoMenuItem (
                        *this, modele, *modele.getUndoManager ().redoDesc (i));
    redoM->append (*menuItem);
  }
  redoM->show_all ();

  menuItemsUndoRedo = redoM->get_children ();
  showUndoRedo = true;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
