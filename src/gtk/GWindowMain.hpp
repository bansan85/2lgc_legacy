#ifndef GWINDOWMAIN__HPP
#define GWINDOWMAIN__HPP

/*
2lgc_code : calcul de résistance des matériaux selon les normes Eurocodes
Copyright (C) 2011-2015

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

Fichier généré automatiquement avec dia2code 0.9.0.
 */

#include <gtkmm/builder.h>
#include <CModele.hpp>
#include <gtkmm/cssprovider.h>
#include <IObserveur.hpp>

/**
 * \brief La fenêtre principale avec un status d'observeur.
 */
class GWindowMain : public IObserveur
{
  // Attributes
  private :
    friend class UndoRedoMenuItem;
    /// Le composant GtkBuirder contenant la fenêtre.
    Glib::RefPtr <Gtk::Builder> build;
    /// Le projet relié à l'interface graphique.
    CModele & modele;
    /// Contient une liste des menuItem lorsque le menu Undo ou Redo est affiché. Est vide si aucun menu n'est affiché.
    std::vector <Gtk::Widget *> menuItemsUndoRedo;
    /// Vaut false si le menu en cours d'affichage est celui d'annulation, vaut true pour le menu de rétablissement.
    bool showUndoRedo : 1;
    /// Permet de savoir si le menu est affiché. le signal "show-menu" s'exécutant à l'affichage et au masquage du menu.
    bool showMenuUndoRedo : 1;
    /// CssProvider pour surligner les MenuItem qui seront annulé.
    Glib::RefPtr <Gtk::CssProvider> cssProviderMenuItemUndoRedo;
  // Operations
  public :
    /**
     * \brief Constructeur d'une classe GWindowMain.
     * \param builder (in) Le composant GtkBuirder contenant la fenêtre.
     * \param modele_ (in) Le projet lié à l'interface graphique.
     */
    GWindowMain (Glib::RefPtr <Gtk::Builder> & builder, CModele & modele_);
    /**
     * \brief Duplication d'une classe GWindowMain.
     * \param other (in) La classe à dupliquer.
     */
    GWindowMain (const GWindowMain & other) = delete;
    /**
     * \brief Duplication d'une classe GWindowMain.
     * \param other (in) La classe à dupliquer.
     * \return GWindowMain &
     */
    GWindowMain & operator = (const GWindowMain & other) = delete;
    /**
     * \brief Destructeur d'une classe GWindowMain.
     */
    virtual ~GWindowMain ();
    /**
     * \brief Procédure recevant les signaux du sujet observé.
     * \param event (in) L'évènement du sujet.
     * \param param (in) Le paramètre éventuel.
     * \return void
     */
    virtual void signal (EEvent event, void * param);
    /**
     * \brief Annule la dernière opération.
     * \return void
     */
    void onClickedUndo ();
    /**
     * \brief Met à jour le menu permettant de sélectionner l'action à annuler au moment où il s'affiche.
     * \return void
     */
    void onShowMenuUndo ();
    /**
     * \brief Rétablit la dernière opération.
     * \return void
     */
    void onClickedRedo ();
    /**
     * \brief Met à jour le menu permettant de sélectionner l'action à rétablir au moment où il s'affiche.
     * \return void
     */
    void onShowMenuRedo ();
    /**
     * \brief Supprime les composants graphiques utiles uniquement pour l'affichage du menu du gestionnaire d'annulation.
     * \return void
     */
    void onHideMenuUndoRedo ();
};

#endif
