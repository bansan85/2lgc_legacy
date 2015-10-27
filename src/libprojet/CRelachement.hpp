#ifndef CRELACHEMENT__HPP
#define CRELACHEMENT__HPP

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

/**
 * \brief Données définissant un relâchement.
 */
class CRelachement
{
  // Attributes
  private :
    /// Nom de l'appui.
    std::string nom;
    /// Degré de liberté en rotation autour de l'axe x en début de la barre.
    IDeplacementType * rxd;
    /// Degré de liberté en rotation autour de l'axe y en début de la barre.
    IDeplacementType * ryd;
    /// Degré de liberté en rotation autour de l'axe z en début de la barre.
    IDeplacementType * rzd;
    /// Degré de liberté en rotation autour de l'axe x en fin de la barre.
    IDeplacementType * rxf;
    /// Degré de liberté en rotation autour de l'axe y en fin de la barre.
    IDeplacementType * ryf;
    /// Degré de liberté en rotation autour de l'axe z en fin de la barre.
    IDeplacementType * rzf;
  // Operations
  public :
    /**
     * \brief Constructeur d'une classe CRelachement.
     * \param nom (in) Nom du relâchement.
     * \param rxd (in) Le type de relâchement autour de l'axe x en début de barre.
     * \param ryd (in) Le type de relâchement autour de l'axe y en début de barre.
     * \param rzd (in) Le type de relâchement autour de l'axe z en début de barre.
     * \param rxf (in) Le type de relâchement autour de l'axe x en fin de barre.
     * \param ryf (in) Le type de relâchement autour de l'axe y en fin de barre.
     * \param rzf (in) Le type de relâchement autour de l'axe z en fin de barre.
     */
    CRelachement (std::string nom, IDeplacementType * rxd, IDeplacementType * ryd, IDeplacementType * rzd, IDeplacementType * rxf, IDeplacementType * ryf, IDeplacementType * rzf);
    /**
     * \brief Duplication d'une classe CRelachement.
     * \param other (in) La classe à dupliquer.
     */
    CRelachement (const CRelachement & other) = delete;
    /**
     * \brief Assignment operator de CRelachement.
     * \param other (in) La classe à dupliquer.
     * \return CRelachement &
     */
    CRelachement & operator = (const CRelachement & other) = delete;
    /**
     * \brief Destructeur d'une classe CRelachement.
     */
    virtual ~CRelachement ();
    /**
     * \brief Renvoie le nom du relâchement.
     * \return std::string const &
     */
    std::string const & getNom () const;
    /**
     * \brief Défini le nom du relâchement.
     * \param val (in) La nouvelle valeur.
     * \return bool CHK
     */
    bool CHK setNom (std::string val);
    /**
     * \brief Renvoie le type de relâchement autour de l'axe x en début de barre.
     * \return IDeplacementType const &
     */
    IDeplacementType const & getrxd () const;
    /**
     * \brief Modifie le type de relâchement autour de l'axe x en début de barre.
     * \param val (in) La nouvelle valeur.
     * \return bool CHK
     */
    bool CHK setrxd (IDeplacementType * val);
    /**
     * \brief Renvoie le type de relâchement autour de l'axe y en début de barre.
     * \return IDeplacementType const &
     */
    IDeplacementType const & getryd () const;
    /**
     * \brief Modifie le type de relâchement autour de l'axe y en début de barre.
     * \param val (in) La nouvelle valeur.
     * \return bool CHK
     */
    bool CHK setryd (IDeplacementType * val);
    /**
     * \brief Renvoie le type de relâchement autour de l'axe z en début de barre.
     * \return IDeplacementType const &
     */
    IDeplacementType const & getrzd () const;
    /**
     * \brief Modifie le type de relâchement autour de l'axe z en début de barre.
     * \param val (in) La nouvelle valeur.
     * \return bool CHK
     */
    bool CHK setrzd (IDeplacementType * val);
    /**
     * \brief Renvoie le type de relâchement autour de l'axe x en fin de barre.
     * \return IDeplacementType const &
     */
    IDeplacementType const & getrxf () const;
    /**
     * \brief Modifie le type de relâchement autour de l'axe x en fin de barre.
     * \param val (in) La nouvelle valeur.
     * \return bool CHK
     */
    bool CHK setrxf (IDeplacementType * val);
    /**
     * \brief Renvoie le type de relâchement autour de l'axe y en fin de barre.
     * \return IDeplacementType const &
     */
    IDeplacementType const & getryf () const;
    /**
     * \brief Modifie le type de relâchement autour de l'axe y en fin de barre.
     * \param val (in) La nouvelle valeur.
     * \return bool CHK
     */
    bool CHK setryf (IDeplacementType * val);
    /**
     * \brief Renvoie le type de relâchement autour de l'axe z en fin de barre.
     * \return IDeplacementType const &
     */
    IDeplacementType const & getrzf () const;
    /**
     * \brief Modifie le type de relâchement autour de l'axe z en fin de barre.
     * \param val (in) La nouvelle valeur.
     * \return bool CHK
     */
    bool CHK setrzf (IDeplacementType * val);
};

#endif
