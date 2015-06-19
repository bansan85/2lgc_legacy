#ifndef CANALYSECOMM__HPP
#define CANALYSECOMM__HPP

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
 * \brief Un rapport est constitué de lignes d'analyse.
 */
class CAnalyseComm
{
  // Attributes
  private :
    /// Description de l'analyse.
    std::string analyse;
    /// Le résultat de l'analyse : 0 : tout va bien, 1 : attention mais problème non critique, 2 : erreur critique.
    uint8_t resultat;
    /// Commentaire de l'analyse en cas d'erreur ou d'avertissement.
    std::string commentaire;
  // Operations
  public :
    /**
     * \brief Constructeur d'une classe CAnalyseComm.
     * \param analyse (in) La description de l'analyse.
     * \param resultat (in) Le résultat de l'analyse.
     * \param commentaire (in) Un commentaire sur l'analyse, vide si aucun.
     */
    CAnalyseComm (std::string analyse, uint8_t resultat, std::string commentaire);
    /**
     * \brief Duplication d'une classe CAnalyseComm.
     * \param other (in) La classe à dupliquer.
     */
    CAnalyseComm (const CAnalyseComm & other) = delete;
    /**
     * \brief Duplication d'une classe CAnalyseComm.
     * \param other (in) La classe à dupliquer.
     * \return CAnalyseComm &
     */
    CAnalyseComm & operator = (const CAnalyseComm & other) = delete;
    /**
     * \brief Destructeur d'une classe CAnalyseComm.
     */
    virtual ~CAnalyseComm ();
    /**
     * \brief Renvoie la description de l'analyse.
     * \return std::string const &
     */
    std::string const & getAnalyse () const;
    /**
     * \brief Défini la description de l'analyse.
     * \param analyse_ (in) La nouvelle description.
     * \return bool
     */
    bool setAnalyse (std::string analyse_);
    /**
     * \brief Renvoie le résultat de l'analyse.
     * \return uint8_t
     */
    uint8_t getResultat () const;
    /**
     * \brief Défini le résultat de l'analyse.
     * \param res (in) Défini le résultat de l'analyse.
     * \return bool
     */
    bool setResultat (std::string res);
    /**
     * \brief Renvoie le commentaire de l'analyse.
     * \return std::string const &
     */
    std::string const & getCommentaire () const;
    /**
     * \brief Défini le commentaire de l'analyse.
     * \param comm (in) Le nouveau commentaire.
     * \return bool
     */
    bool setCommentaire (std::string comm);
};

#endif
