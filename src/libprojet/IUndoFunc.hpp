#ifndef IUNDOFUNC__HPP
#define IUNDOFUNC__HPP

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

class CPonderation;
class CPonderations;
class CCalculs;
class CActionSetPsi;
class CAction;
class CModeleAddAction;
class CModeleRmAction;
class CModeleSetParametres;
class CModele;

#include <IUndoableFonction.hpp>

/**
 * \brief Classe intermédiaire définissant la fonction t (), abbréviation de this. Il n'était malheureusement pas possible de la mettre dans la librairie undo pour des raisons de dépendance circulaire.
 */
class IUndoFunc : public IUndoableFonction
{
  // Operations
  public :
    /**
     * \brief Constructeur d'une interface IUndoFunc.
     * \param importance (in) Indique si la modification est importante ou non.
     */
    IUndoFunc (bool importance);
    /**
     * \brief Duplication d'une interface IUndoFunc.
     * \param other (in) L'interface à dupliquer.
     */
    IUndoFunc (const IUndoFunc & other) = delete;
    /**
     * \brief Duplication d'une interface IUndoFunc.
     * \param other (in) L'interface à dupliquer.
     * \return IUndoFunc &
     */
    IUndoFunc & operator = (const IUndoFunc & other);
    /**
     * \brief Destructeur d'une interface IUndoFunc.
     */
    virtual ~IUndoFunc ();
    /**
     * \brief Transtype la classe héritant de cette fonction en CModele.
     * \return CModele *
     */
    CModele * t ();
};

#endif
