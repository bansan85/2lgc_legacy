#ifndef CTRONCON__HPP
#define CTRONCON__HPP

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

#include <INb.hpp>

/**
 * \brief Définition de la structure Troncon qui contient une fonction avec un domaine de validité précis. La fonction mathématique est définie par x0 + x1*x + x2*x² + x3*x³ + x4*x⁴ + x5*x⁵ + x6*x⁶.
 */
class CTroncon
{
  // Attributes
  private :
    /// Début du tronçon de validité de la fonction.
    INb * debut;
    /// Fin du tronçon de validité de la fonction.
    INb * fin;
    /// Cœfficient x0.
    INb * x0;
    /// Cœfficient x1.
    INb * x1;
    /// Cœfficient x2.
    INb * x2;
    /// Cœfficient x3.
    INb * x3;
    /// Cœfficient x4.
    INb * x4;
    /// Cœfficient x5.
    INb * x5;
    /// Cœfficient x6.
    INb * x6;
  // Operations
  public :
    /**
     * \brief Constructeur d'une classe CTroncon.
     * \param debut (in) Le début du tronçon.
     * \param fin (in) La fin du tronçon.
     * \param x0 (in) Le cœfficient x0.
     * \param x1 (in) Le cœfficient x1.
     * \param x2 (in) Le cœfficient x2.
     * \param x3 (in) Le cœfficient x3.
     * \param x4 (in) Le cœfficient x4.
     * \param x5 (in) Le cœfficient x5.
     * \param x6 (in) Le cœfficient x6.
     */
    CTroncon (INb * debut, INb * fin, INb * x0, INb * x1, INb * x2, INb * x3, INb * x4, INb * x5, INb * x6);
    /**
     * \brief Duplication d'une classe CTroncon.
     * \param other (in) La classe à dupliquer.
     */
    CTroncon (const CTroncon & other);
    /**
     * \brief Assignment operator de CTroncon.
     * \param other (in) La classe à dupliquer.
     * \return CTroncon &
     */
    CTroncon & operator = (const CTroncon & other);
    /**
     * \brief Destructeur d'une classe CTroncon.
     */
    virtual ~CTroncon ();
    /**
     * \brief Renvoie le début du tronçon.
     * \return INb const &
     */
    INb const & getDebut () const;
    /**
     * \brief Défini le début du tronçon.
     * \param debut_ (in) Le nouveau début.
     * \return bool CHK
     */
    bool CHK setDebut (INb * debut_);
    /**
     * \brief Renvoie la fin du tronçon.
     * \return INb const &
     */
    INb const & getFin () const;
    /**
     * \brief Défini la fin du tronçon.
     * \param fin_ (in) La nouvelle fin.
     * \return bool CHK
     */
    bool CHK setFin (INb * fin_);
    /**
     * \brief Renvoie le cœfficient x0.
     * \return INb const &
     */
    INb const & getx0 () const;
    /**
     * \brief Défini le cœfficient x0.
     * \param x0_ (in) Le nouveau cœfficient x0.
     * \return bool CHK
     */
    bool CHK setx0 (INb * x0_);
    /**
     * \brief Renvoie le cœfficient x1.
     * \return INb const &
     */
    INb const & getx1 () const;
    /**
     * \brief Défini le cœfficient x1.
     * \param x1_ (in) Le nouveau cœfficient x1.
     * \return bool CHK
     */
    bool CHK setx1 (INb * x1_);
    /**
     * \brief Renvoie le cœfficient x2.
     * \return INb const &
     */
    INb const & getx2 () const;
    /**
     * \brief Défini le cœfficient x2.
     * \param x2_ (in) Le nouveau cœfficient x2.
     * \return bool CHK
     */
    bool CHK setx2 (INb * x2_);
    /**
     * \brief Renvoie le cœfficient x3.
     * \return INb const &
     */
    INb const & getx3 () const;
    /**
     * \brief Défini le cœfficient x3.
     * \param x3_ (in) Le nouveau cœfficient x3.
     * \return bool CHK
     */
    bool CHK setx3 (INb * x3_);
    /**
     * \brief Renvoie le cœfficient x4.
     * \return INb const &
     */
    INb const & getx4 () const;
    /**
     * \brief Défini le cœfficient x4.
     * \param x4_ (in) Le nouveau cœfficient x4.
     * \return bool CHK
     */
    bool CHK setx4 (INb * x4_);
    /**
     * \brief Renvoie le cœfficient x5.
     * \return INb const &
     */
    INb const & getx5 () const;
    /**
     * \brief Défini le cœfficient x5.
     * \param x5_ (in) Le nouveau cœfficient x5.
     * \return bool CHK
     */
    bool CHK setx5 (INb * x5_);
    /**
     * \brief Renvoie le cœfficient x6.
     * \return INb const &
     */
    INb const & getx6 () const;
    /**
     * \brief Défini le cœfficient x6.
     * \param x6_ (in) Le nouveau cœfficient x6.
     * \return bool CHK
     */
    bool CHK setx6 (INb * x6_);
};

#endif
