#ifndef CHARGEBARREPONCTUELLE__HPP
#define CHARGEBARREPONCTUELLE__HPP

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

#include <CBarre.hpp>
#include <INb.hpp>
#include "ICharge.hpp"

/**
 * \brief Charge ponctuelle appliquée sur une barre.
 */
class ChargeBarrePonctuelle : public ICharge
{
  // Attributes
  private :
    /// Liste des barres où est appliquée la charge.
    std::list <CBarre *> barres;
    /// Charge appliquée dans le repère local.
    bool repere_local;
    /// Position en m de la charge par rapport au début de la barre.
    INb * position;
    /// Charge ponctuelle en N dans l'axe x.
    INb * fx;
    /// Charge ponctuelle en N dans l'axe y.
    INb * fy;
    /// Charge ponctuelle en N dans l'axe z.
    INb * fz;
    /// Moment ponctuel en N.m autour de l'axe x.
    INb * mx;
    /// Moment ponctuel en N.m autour de l'axe y.
    INb * my;
    /// Moment ponctuel en N.m autour de l'axe z.
    INb * mz;
  // Operations
  public :
    /**
     * \brief Constructeur d'une classe ChargeBarrePonctuelle.
     * \param nom (in) Le nom de la charge.
     * \param barres (in) La liste des barres.
     * \param repere (in) Le repère utilisé (true indique le repère local).
     * \param position (in) La position de la charge par rapport au début de la barre.
     * \param fx (in) Charge ponctuelle en dans l'axe x.
     * \param fy (in) Charge ponctuelle en dans l'axe y.
     * \param fz (in) Charge ponctuelle en dans l'axe z.
     * \param mx (in) Moment ponctuel autour de l'axe x.
     * \param my (in) Moment ponctuel autour de l'axe y.
     * \param mz (in) Moment ponctuel autour de l'axe z.
     * \param undo (in) Le gestionnaire des modifications.
     */
    ChargeBarrePonctuelle (std::string nom, std::list <CBarre *> barres, bool repere, INb * position, INb * fx, INb * fy, INb * fz, INb * mx, INb * my, INb * mz, UndoManager & undo);
    /**
     * \brief Duplication d'une classe ChargeBarrePonctuelle.
     * \param other (in) La classe à dupliquer.
     */
    ChargeBarrePonctuelle (const ChargeBarrePonctuelle & other) = delete;
    /**
     * \brief Assignment operator de ChargeBarrePonctuelle.
     * \param other (in) La classe à dupliquer.
     * \return ChargeBarrePonctuelle &
     */
    ChargeBarrePonctuelle & operator = (const ChargeBarrePonctuelle & other) = delete;
    /**
     * \brief Destructeur d'une classe ChargeBarrePonctuelle.
     */
    virtual ~ChargeBarrePonctuelle ();
    /**
     * \brief Renvoie le choix du repère utilisé.
     * \return bool
     */
    bool getRepere () const;
    /**
     * \brief Défini le choix du repère utilisé (true si local).
     * \param val (in) La nouvelle valeur.
     * \return bool CHK
     */
    bool CHK setRepere (bool val);
    /**
     * \brief Défini la position de la charge sur la barre.
     * \return INb const &
     */
    INb const & getPosition () const;
    /**
     * \brief Défini la position de la charge sur la barre.
     * \param val (in) La nouvelle position.
     * \return bool CHK
     */
    bool CHK setPosition (INb * val);
    /**
     * \brief Renvoie la charge ponctuelle dans l'axe x.
     * \return INb const &
     */
    INb const & getfx () const;
    /**
     * \brief Défini la charge ponctuelle dans l'axe x.
     * \param val (in) La nouvelle valeur.
     * \return bool CHK
     */
    bool CHK setfx (INb * val);
    /**
     * \brief Renvoie la charge ponctuelle dans l'axe y.
     * \return INb const &
     */
    INb const & getfy () const;
    /**
     * \brief Défini la charge ponctuelle dans l'axe y.
     * \param val (in) La nouvelle valeur.
     * \return bool CHK
     */
    bool CHK setfy (INb * val);
    /**
     * \brief Renvoie la charge ponctuelle dans l'axe z.
     * \return INb const &
     */
    INb const & getfz () const;
    /**
     * \brief Défini la charge ponctuelle dans l'axe z.
     * \param val (in) La nouvelle valeur.
     * \return bool CHK
     */
    bool CHK setfz (INb * val);
    /**
     * \brief Renvoie le moment ponctuel autour de l'axe x.
     * \return INb const &
     */
    INb const & getmx () const;
    /**
     * \brief Défini le moment ponctuel autour de l'axe x.
     * \param val (in) La nouvelle valeur.
     * \return bool CHK
     */
    bool CHK setmx (INb * val);
    /**
     * \brief Renvoie le moment ponctuel autour de l'axe y.
     * \return INb const &
     */
    INb const & getmy () const;
    /**
     * \brief Défini le moment ponctuel autour de l'axe y.
     * \param val (in) La nouvelle valeur.
     * \return bool CHK
     */
    bool CHK setmy (INb * val);
    /**
     * \brief Renvoie le moment ponctuel autour de l'axe z.
     * \return INb const &
     */
    INb const & getmz () const;
    /**
     * \brief Défini le moment ponctuel autour de l'axe z.
     * \param val (in) La nouvelle valeur.
     * \return bool CHK
     */
    bool CHK setmz (INb * val);
    /**
     * \brief Ajoute plusieurs barres à la charge.
     * \param barres (in) La liste de barres à ajouter.
     * \return bool CHK
     */
    bool CHK addBarres (std::list <CBarre *> * barres);
    /**
     * \brief Enlève plusieurs barres à la charge.
     * \param barres (in) La liste de barres à enlever.
     * \return bool CHK
     */
    bool CHK rmBarres (std::list <CBarre *> * barres);
};

#endif
