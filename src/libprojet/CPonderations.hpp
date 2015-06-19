#ifndef CPONDERATIONS__HPP
#define CPONDERATIONS__HPP

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

#include "CPonderation.hpp"

/**
 * \brief Paramètres de combinaisons avec leurs listes générées. Spécifie la méthode des combinaisons (E0,A1.3).
 */
class CPonderations
{
  // Associations
  // Attributes
  private :
    /// ELU_GEO/STR : si 0 approche 1, 1 : approche 2 et 2 : approche 3.
    uint8_t elu_geo_str_methode;
    /// ELU_EQU : méthode 1 si 0, méthode 2 si 1.
    bool elu_equ_methode : 1;
    /// ELU_ACC : 0 si psi1,1 et 1 si psi2,1.
    bool elu_acc_psi : 1;
    /// Si 0, utilisation des formules 6.10a et b. Si 1 alors formule 6.10.
    bool form_6_10 : 1;
    /// Liste des pondérations selon l'ELU EQU.
    std::list <std::list <CPonderation *> *> elu_equ;
    /// Liste des pondérations selon l'ELU STR.
    std::list <std::list <CPonderation *> *> elu_str;
    /// Liste des pondérations selon l'ELU GEO.
    std::list <std::list <CPonderation *> *> elu_geo;
    /// Liste des pondérations selon l'ELU FAT.
    std::list <std::list <CPonderation *> *> elu_fat;
    /// Liste des pondérations selon l'ELU ACC.
    std::list <std::list <CPonderation *> *> elu_acc;
    /// Liste des pondérations selon l'ELU SIS.
    std::list <std::list <CPonderation *> *> elu_sis;
    /// Liste des pondérations selon l'ELS CAR.
    std::list <std::list <CPonderation *> *> els_car;
    /// Liste des pondérations selon l'ELS FREQ.
    std::list <std::list <CPonderation *> *> els_freq;
    /// Liste des pondérations selon l'ELS PERM.
    std::list <std::list <CPonderation *> *> els_perm;
  // Operations
  public :
    /**
     * \brief Constructeur d'une classe CPonderations.
     * \param eluequ (in) ELU_EQU : méthode 1 si 0, méthode 2 si 1.
     * \param elugeostr (in) ELU_GEO/STR : si 0 approche 1, 1 : approche 2 et 2 : approche 3.
     * \param eluacc (in) ELU_ACC : 0 si psi1,1 et 1 si psi2,1.
     * \param form6_10 (in) Si 0, utilisation des formules 6.10a et b. Si 1 alors formule 6.10.
     */
    CPonderations (bool eluequ, uint8_t elugeostr, bool eluacc, bool form6_10);
    /**
     * \brief Duplication d'une classe CPonderations.
     * \param other (in) La classe à dupliquer.
     */
    CPonderations (const CPonderations & other) = delete;
    /**
     * \brief Assignment operator de CPonderations.
     * \param other (in) La classe à dupliquer.
     * \return CPonderations &
     */
    CPonderations & operator = (const CPonderations & other) = delete;
    /**
     * \brief Destructeur d'une classe CPonderations.
     */
    virtual ~CPonderations ();
    /**
     * \brief Renvoie la méthode de combinaison à l'ELU équilibre.
     * \return bool
     */
    bool getequ () const;
    /**
     * \brief Défini la méthode de combinaison à l'ELU équilibre.
     * \param val (in) La nouvelle valeur.
     * \return bool CHK
     */
    bool CHK setequ (bool val);
    /**
     * \brief Renvoie la méthode de combinaison à l'ELU géotechnique et structure.
     * \return uint8_t
     */
    uint8_t getgeostr () const;
    /**
     * \brief Défini la méthode de combinaison à l'ELU géotechnique et structure.
     * \param val (in) La nouvelle valeur.
     * \return bool CHK
     */
    bool CHK setgeostr (uint8_t val);
    /**
     * \brief Renvoie la méthode de combinaison à l'ELU accidentel.
     * \return bool
     */
    bool getacc () const;
    /**
     * \brief Défini la méthode de combinaison à l'ELU accidentel.
     * \param val (in) La nouvelle valeur.
     * \return bool CHK
     */
    bool CHK setacc (bool val);
    /**
     * \brief Renvoie le choix de l'utilisation de la formule 6.10.
     * \return bool
     */
    bool get6_10 ();
    /**
     * \brief Défini le choix de l'utilisation de la formule 6.10.
     * \param val (in) La nouvelle valeur.
     * \return bool CHK
     */
    bool CHK set6_10 (bool val);
};

#endif
