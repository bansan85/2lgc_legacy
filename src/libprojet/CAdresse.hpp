#ifndef CADRESSE__HPP
#define CADRESSE__HPP

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

/**
 * \brief Adresse conforme au fichier france_villes.csv.
 */
class CAdresse
{
  // Attributes
  private :
    /// Numéro du département : 1 à 976, y compris 2A et 2B.
    std::string departement;
    /// Numéro de la commune du département.
    uint32_t commune;
    /// Nom du destinataire ou du client.
    std::string destinataire;
    /// Adresse du projet (rue, immeuble, …).
    std::string adresse;
    /// Code postal du projet.
    uint32_t code_postal;
    /// Ville du projet.
    std::string ville;
  // Operations
  public :
    /**
     * \brief Constructeur d'une classe CAdresse.
     * \param departement (in) Le numéro du département.
     * \param commune (in) Numéro de la commune du département, cf. france_villes.csv.
     * \param destinataire (in) Nom du destinataire.
     * \param adresse (in) L'adresse dans la ville.
     * \param codepostal (in) Le code postal de la ville.
     * \param ville (in) La ville du projet.
     */
    CAdresse (std::string departement, uint32_t commune, std::string destinataire, std::string adresse, uint32_t codepostal, std::string ville);
    /**
     * \brief Constructeur d'une classe CAdresse.
     * \param other (in) La classe à dupliquer.
     */
    CAdresse (const CAdresse & other) = delete;
    /**
     * \brief Constructeur d'une classe CAdresse.
     * \param other (in) La classe à dupliquer.
     * \return CAdresse &
     */
    CAdresse & operator = (const CAdresse & other) = delete;
    /**
     * \brief Destructeur d'une classe CAdresse.
     */
    virtual ~CAdresse ();
    /**
     * \brief Renvoie le numéro du département.
     * \return std::string const &
     */
    std::string const & getDepartement () const;
    /**
     * \brief Défini le numéro du département.
     * \param dep (in) Le nouveau département.
     * \return bool CHK
     */
    bool CHK setDepartement (std::string dep);
    /**
     * \brief Renvoie le numéro de la commune.
     * \return uint32_t const &
     */
    uint32_t const & getCommune () const;
    /**
     * \brief Défini le numéro de la commune.
     * \param comm (in) La nouvelle commune.
     * \return bool CHK
     */
    bool CHK setCommune (uint32_t comm);
    /**
     * \brief Renvoie le destinataire du projet.
     * \return std::string const &
     */
    std::string const & getDestinataire () const;
    /**
     * \brief Défini le destinataire du projet.
     * \param destinataire (in) Le nouveau destinataire.
     * \return bool CHK
     */
    bool CHK setDestinataire (std::string destinataire);
    /**
     * \brief Renvoie le destinataire.
     * \return std::string const &
     */
    std::string const & getAdresse () const;
    /**
     * \brief Défini le destinataire.
     * \param adresse (in) La nouvelle addresse.
     * \return bool CHK
     */
    bool CHK setAdresse (std::string adresse);
    /**
     * \brief Renvoie le code postal.
     * \return uint32_t const &
     */
    uint32_t const & getCodePostal () const;
    /**
     * \brief Renvoie la ville.
     * \return std::string const &
     */
    std::string const & getVille () const;
};

#endif
