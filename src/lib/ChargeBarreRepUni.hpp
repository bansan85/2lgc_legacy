#ifndef CHARGEBARREREPUNI__HPP
#define CHARGEBARREREPUNI__HPP

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
 * \brief Charge uniformément répartie sur une barre.
 */
class ChargeBarreRepUni : public ICharge
{
  // Attributes
  private :
    /// Liste des barres où est appliquée la charge.
    std::list <CBarre *> barres;
    /// Charge appliquée dans le repère local.
    bool repere_local;
    /// La charge est projetée dans le repère local. Cette option est incompatible avec repere_local == TRUE.
    bool projection;
    /// Position en m du début de la charge par rapport au début de la barre.
    INb * a;
    /// Position en m de la fin de la charge par rapport à la fin de la barre.
    INb * b;
    /// Charge ponctuelle en N/m dans l'axe x.
    INb * fx;
    /// Charge ponctuelle en N/m dans l'axe y.
    INb * fy;
    /// Charge ponctuelle en N/m dans l'axe z.
    INb * fz;
    /// Moment ponctuel en N.m/m autour de l'axe x.
    INb * mx;
    /// Moment ponctuel en N.m/m autour de l'axe y.
    INb * my;
    /// Moment ponctuel en N.m/m autour de l'axe z.
    INb * mz;
  // Operations
  public :
    /**
     * \brief Constructeur d'une classe ChargeBarreRepUni.
     * \param nom (in) Nom de la charge.
     * \param barres (in) La liste des barres.
     * \param repere (in) Charge appliquée dans le repère local.
     * \param projection (in) La charge est projetée dans le repère local.
     * \param a (in) Position du début de la charge par rapport au début de la barre.
     * \param b (in) Position de la fin de la charge par rapport à la fin de la barre.
     * \param fx (in) Charge ponctuelle dans l'axe x.
     * \param fy (in) Charge ponctuelle dans l'axe y.
     * \param fz (in) Charge ponctuelle dans l'axe z.
     * \param mx (in) Moment ponctuel autour de l'axe x.
     * \param my (in) Moment ponctuel autour de l'axe y.
     * \param mz (in) Moment ponctuel autour de l'axe z.
     * \param undo (in) Le gestionnaire des modifications.
     */
    ChargeBarreRepUni (std::string nom, std::list <CBarre *> barres, bool repere, bool projection, INb * a, INb * b, INb * fx, INb * fy, INb * fz, INb * mx, INb * my, INb * mz, UndoManager & undo);
    /**
     * \brief Duplication d'une classe ChargeBarreRepUni.
     * \param other (in) La classe à dupliquer.
     */
    ChargeBarreRepUni (const ChargeBarreRepUni & other) = delete;
    /**
     * \brief Assignment operator de ChargeBarreRepUni.
     * \param other (in) La classe à dupliquer.
     * \return ChargeBarreRepUni &
     */
    ChargeBarreRepUni & operator = (const ChargeBarreRepUni & other) = delete;
    /**
     * \brief Destructeur d'une classe ChargeBarreRepUni.
     */
    virtual ~ChargeBarreRepUni ();
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
     * \brief Renvoie si la charge est projetée dans le repère global.
     * \return bool
     */
    bool getProjection () const;
    /**
     * \brief Défini si la charge est projetée dans le repère global.
     * \param val (in) La nouvelle valeur.
     * \return bool CHK
     */
    bool CHK setProjection (bool val);
    /**
     * \brief Renvoie le début de la charge par rapport au début de la barre.
     * \return INb const &
     */
    INb const & geta () const;
    /**
     * \brief Défini le début de la charge par rapport au début de la barre.
     * \param val (in) La nouvelle position.
     * \return bool CHK
     */
    bool CHK seta (INb * val);
    /**
     * \brief Renvoie la fin de la charge par rapport à la fin de la barre.
     * \return INb const &
     */
    INb const & getb () const;
    /**
     * \brief Défini la fin de la charge par rapport à la fin de la barre.
     * \param val (in) La nouvelle valeur.
     * \return bool CHK
     */
    bool CHK setb (INb * val);
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
