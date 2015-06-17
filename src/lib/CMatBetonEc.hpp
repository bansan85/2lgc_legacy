#ifndef CMATBETONEC__HPP
#define CMATBETONEC__HPP

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

#include "IMateriau.hpp"

/**
 * \brief Défini un matériau béton selon l'Eurocode 2.
 */
class CMatBetonEc : public IMateriau
{
  // Attributes
  private :
    /// Résistance caractéristique en compression sur cylindre en Pa.
    INb * fck;
    /// Résistance caractéristique en compression sur cube en Pa.
    INb * fckcube;
    /// Valeur moyenne de la résistance en compression en Pa.
    INb * fcm;
    /// Valeur moyenne de la résistance en traction directe en Pa.
    INb * fctm;
    /// Résistance caractéristique en traction directe fractile 5% en Pa.
    INb * fctk_0_05;
    /// Résistance caractéristique en traction directe fractile 95% en Pa.
    INb * fctk_0_95;
    /// Déformation relative en compression au point 1.
    INb * ec1;
    /// Déformation relative ultime en compression au point 1.
    INb * ecu1;
    /// Déformation relative en compression au point 2.
    INb * ec2;
    /// Déformation relative ultime en compression au point 2.
    INb * ecu2;
    /// Exposant utilisé dans le calcul de sigmac.
    INb * n;
    /// Déformation relative en compression au point 3.
    INb * ec3;
    /// Déformation relative ultime en compression au point 3.
    INb * ecu3;
  // Operations
  public :
    /**
     * \brief Constructeur d'une classe CMatBetonEc.
     * \param fck (in) La résistance caractéristique en compression sur cylindre en MPa.
     * \param undo (in) Le gestionnaire des modifications.
     */
    CMatBetonEc (INb * fck, UndoManager & undo);
    /**
     * \brief Duplication d'une classe CMatBetonEc.
     * \param other (in) La classe à dupliquer.
     */
    CMatBetonEc (const CMatBetonEc & other) = delete;
    /**
     * \brief Assignment operator de CMatBetonEc.
     * \param other (in) La classe à dupliquer.
     * \return CMatBetonEc &
     */
    CMatBetonEc & operator = (const CMatBetonEc & other) = delete;
    /**
     * \brief Destructeur d'une classe CMatBetonEc.
     */
    virtual ~CMatBetonEc ();
    /**
     * \brief Renvoie la valeur de fck en Pa.
     * \return INb const &
     */
    INb const & getfck () const;
    /**
     * \brief Modifie la résistance caractéristique en compression sur cylindre en Pa.
     * \param val (in) La nouvelle valeur.
     * \return bool CHK
     */
    bool CHK setfck (INb * val);
    /**
     * \brief Renvoie la valeur de fckcube en Pa.
     * \return INb const &
     */
    INb const & getfckcube () const;
    /**
     * \brief Modifie la résistance caractéristique en compression sur cube en Pa.
     * \param val (in) La nouvelle valeur.
     * \return bool CHK
     */
    bool CHK setfckcube (INb * val);
    /**
     * \brief Renvoie la valeur de fcm en Pa.
     * \return INb const &
     */
    INb const & getfcm () const;
    /**
     * \brief Modifie la valeur moyenne de la résistance en compression en Pa.
     * \param val (in) La nouvelle valeur.
     * \return bool CHK
     */
    bool CHK setfcm (INb * val);
    /**
     * \brief Renvoie la valeur de fctm en Pa.
     * \return INb const &
     */
    INb const & getfctm () const;
    /**
     * \brief Modifie la valeur moyenne de la résistance en traction directe en Pa.
     * \param val (in) La nouvelle valeur.
     * \return bool CHK
     */
    bool CHK setfctm (INb * val);
    /**
     * \brief Renvoie la valeur de fctk_0_05 en Pa.
     * \return INb const &
     */
    INb const & getfctk_0_05 () const;
    /**
     * \brief Modifie la résistance caractéristique en traction directe fractile 5% en Pa.
     * \param val (in) La nouvelle valeur.
     * \return bool CHK
     */
    bool CHK setfctk_0_05 (INb * val);
    /**
     * \brief Renvoie la valeur de fctk_0_95 en Pa.
     * \return INb const &
     */
    INb const & getfctk_0_95 () const;
    /**
     * \brief Modifie la résistance caractéristique en traction directe fractile 95% en Pa.
     * \param val (in) La nouvelle valeur.
     * \return bool CHK
     */
    bool CHK setfctk_0_95 (INb * val);
    /**
     * \brief Renvoie la valeur de ec1.
     * \return INb const &
     */
    INb const & getec1 () const;
    /**
     * \brief Modifie la déformation relative en compression au point 1.
     * \param val (in) La nouvelle valeur.
     * \return bool CHK
     */
    bool CHK setec1 (INb * val);
    /**
     * \brief Renvoie la valeur de ecu1.
     * \return INb const &
     */
    INb const & getecu1 () const;
    /**
     * \brief Modifie la déformation relative ultime en compression au point 1.
     * \param val (in) La nouvelle valeur.
     * \return bool CHK
     */
    bool CHK setecu1 (INb * val);
    /**
     * \brief Renvoie la valeur de ec2.
     * \return INb const &
     */
    INb const & getec2 () const;
    /**
     * \brief Modifie la déformation relative en compression au point 2.
     * \param val (in) La nouvelle valeur.
     * \return bool CHK
     */
    bool CHK setec2 (INb * val);
    /**
     * \brief Renvoie la valeur de ecu2.
     * \return INb const &
     */
    INb const & getecu2 () const;
    /**
     * \brief Modifie la déformation relative ultime en compression au point 2.
     * \param val (in) La nouvelle valeur.
     * \return bool CHK
     */
    bool CHK setecu2 (INb * val);
    /**
     * \brief Renvoie la valeur de n.
     * \return INb const &
     */
    INb const & getn () const;
    /**
     * \brief Modifie l'exposant utilisé dans le calcul de sigmac.
     * \param val (in) La nouvelle valeur.
     * \return bool CHK
     */
    bool CHK setn (INb * val);
    /**
     * \brief Renvoie la valeur de ec3.
     * \return INb const &
     */
    INb const & getec3 () const;
    /**
     * \brief Modifie la déformation relative en compression au point 3.
     * \param val (in) La nouvelle valeur.
     * \return bool CHK
     */
    bool CHK setec3 (INb * val);
    /**
     * \brief Renvoie la valeur de ecu3.
     * \return INb const &
     */
    INb const & getecu3 () const;
    /**
     * \brief Modifie la déformation relative ultime en compression au point 3.
     * \param val (in) La nouvelle valeur.
     * \return bool CHK
     */
    bool CHK setecu3 (INb * val);
};

#endif
