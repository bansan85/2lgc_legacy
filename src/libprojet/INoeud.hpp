#ifndef INOEUD__HPP
#define INOEUD__HPP

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

#include <CAppui.hpp>

/**
 * \brief Défini un nœud par ces coordonnées et son appui.
 */
class INoeud
{
  // Attributes
  private :
    /// Numéro de la barre. Est attribué automatiquement. Le numéro est attribué automatiquement comme celui du numéro du dernier nœud + 1.
    uint32_t numero;
    /// Défini l'appui du nœud. Peut-être NULL si le nœud est libre.
    CAppui * appui;
  // Operations
  public :
    /**
     * \brief Constructeur d'une interface INoeud.
     */
    INoeud ();
    /**
     * \brief Duplication d'une classe INoeud.
     * \param other (in) La classe à dupliquer.
     */
    INoeud (const INoeud & other) = delete;
    /**
     * \brief Duplication d'une classe INoeud.
     * \param other (in) La classe à dupliquer.
     * \return INoeud &
     */
    INoeud & operator = (const INoeud & other) = delete;
    /**
     * \brief Destructeur d'une interface INœud.
     */
    virtual ~INoeud ();
    /**
     * \brief Renvoie le numéro du nœud.
     * \return uint32_t
     */
    uint32_t getNumero ();
    /**
     * \brief Renvoie l'appui du nœud.
     * \return CAppui const &
     */
    CAppui const & getAppui () const;
    /**
     * \brief Défini l'appui du nœud.
     * \param val (in) Le nouveau appui.
     * \return bool CHK
     */
    bool CHK setAppui (CAppui * val);
    /**
     * \brief Renvoie la position en x.
     * \return double
     */
    virtual double getx () = 0;
    /**
     * \brief Renvoie la position en y.
     * \return double
     */
    virtual double gety () = 0;
    /**
     * \brief Renvoie la position en z.
     * \return double
     */
    virtual double getz () = 0;
};

#endif
