#ifndef GWINDOWMAIN__HPP
#define GWINDOWMAIN__HPP

/*
2lgc_code : calcul de résistance des matériaux selon les normes Eurocodes
Copyright (C) 2011

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
#include <IObserveur.hpp>

/**
 * \brief La fenêtre principale avec un status d'observeur.
 */
class GWindowMain : public IObserveur
{
  // Attributes
  private :
    /// Le composant GtkBuirder contenant la fenêtre.
    Glib::RefPtr <Gtk::Builder> build;
    /// Le projet relié à l'interface graphique.
    CModele & modele;
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
     * \brief Répète la dernière opération.
     * \return void
     */
    void onClickedRedo ();
};

#endif
