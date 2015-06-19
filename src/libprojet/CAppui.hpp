#ifndef CAPPUI__HPP
#define CAPPUI__HPP

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

#include <IDeplacementType.hpp>
#include <IUndoable.hpp>

/**
 * \brief Données définissant un appui.
 */
class CAppui : public IUndoable
{
  // Attributes
  private :
    /// Nom de l'appui.
    std::string nom;
    /// Degré de liberté de la direction x.
    IDeplacementType * ux;
    /// Degré de liberté de la direction y.
    IDeplacementType * uy;
    /// Degré de liberté de la direction z.
    IDeplacementType * uz;
    /// Degré de liberté en rotation autour de l'axe x.
    IDeplacementType * rx;
    /// Degré de liberté en rotation autour de l'axe y.
    IDeplacementType * ry;
    /// Degré de liberté en rotation autour de l'axe z.
    IDeplacementType * rz;
  // Operations
  public :
    /**
     * \brief Constructeur d'une classe CAppui.
     * \param nom (in) Nom de l'appui.
     * \param ux (in) Le type d'appui selon l'axe x.
     * \param uy (in) Le type d'appui selon l'axe y.
     * \param uz (in) Le type d'appui selon l'axe z.
     * \param rx (in) Le type d'appui autour de l'axe x.
     * \param ry (in) Le type d'appui autour de l'axe y.
     * \param rz (in) Le type d'appui autour de l'axe z.
     * \param undo (in) Le gestionnaire des modifications.
     */
    CAppui (std::string nom, IDeplacementType * ux, IDeplacementType * uy, IDeplacementType * uz, IDeplacementType * rx, IDeplacementType * ry, IDeplacementType * rz, UndoManager & undo);
    /**
     * \brief Duplication d'une classe CAppui.
     * \param other (in) La classe à dupliquer.
     */
    CAppui (const CAppui & other) = delete;
    /**
     * \brief Assignment operator de CAppui.
     * \param other (in) La classe à dupliquer.
     * \return CAppui &
     */
    CAppui & operator = (const CAppui & other) = delete;
    /**
     * \brief Destructeur d'une classe CAppui.
     */
    virtual ~CAppui ();
    /**
     * \brief Renvoie le nom de l'appui.
     * \return std::string const &
     */
    std::string const & getNom () const;
    /**
     * \brief Difini le nom de l'appui.
     * \param val (in) La nouvelle valeur.
     * \return bool CHK
     */
    bool CHK setNom (std::string val);
    /**
     * \brief Renvoie le type d'appui selon l'axe x.
     * \return IDeplacementType const &
     */
    IDeplacementType const & getux () const;
    /**
     * \brief Modifie le type d'appui selon l'axe x.
     * \param val (in) La nouvelle valeur.
     * \return bool CHK
     */
    bool CHK setux (IDeplacementType * val);
    /**
     * \brief Renvoie le type d'appui selon l'axe y.
     * \return IDeplacementType const &
     */
    IDeplacementType const & getuy () const;
    /**
     * \brief Modifie le type d'appui selon l'axe y.
     * \param val (in) La nouvelle valeur.
     * \return bool CHK
     */
    bool CHK setuy (IDeplacementType * val);
    /**
     * \brief Renvoie le type d'appui selon l'axe z.
     * \return IDeplacementType const &
     */
    IDeplacementType const & getuz () const;
    /**
     * \brief Modifie le type d'appui selon l'axe z.
     * \param val (in) La nouvelle valeur.
     * \return bool CHK
     */
    bool CHK setuz (IDeplacementType * val);
    /**
     * \brief Renvoie le type d'appui autour de l'axe x.
     * \return IDeplacementType const &
     */
    IDeplacementType const & getrx () const;
    /**
     * \brief Modifie le type d'appui autour de l'axe x.
     * \param val (in) La nouvelle valeur.
     * \return bool CHK
     */
    bool CHK setrx (IDeplacementType * val);
    /**
     * \brief Renvoie le type d'appui autour de l'axe y.
     * \return IDeplacementType const &
     */
    IDeplacementType const & getry () const;
    /**
     * \brief Modifie le type d'appui autour de l'axe y.
     * \param val (in) La nouvelle valeur.
     * \return bool CHK
     */
    bool CHK setry (IDeplacementType * val);
    /**
     * \brief Renvoie le type d'appui autour de l'axe z.
     * \return IDeplacementType const &
     */
    IDeplacementType const & getrz () const;
    /**
     * \brief Modifie le type d'appui autour de l'axe z.
     * \param val (in) La nouvelle valeur.
     * \return bool CHK
     */
    bool CHK setrz (IDeplacementType * val);
};

#endif
