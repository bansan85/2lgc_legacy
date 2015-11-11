#ifndef CCALCULS__HPP
#define CCALCULS__HPP

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

#include "POCO/calc/CPonderations.hpp"
#include "POCO/calc/CAnalyseComm.hpp"
#include "POCO/calc/CBarreInfoEF.hpp"

/**
 * \brief Contient toutes les données nécessaires pour la réalisation des calculs aux éléments finis et notamment les variables utilisées par les librairies cholmod et umfpack. Établi une corrélation entre le degré de liberté (x, y, z, rx, ry, rz) d'un noeud et sa position dans la matrice de rigidité globale partielle et complète. Par partielle, il faut comprendre la matrice de rigidité globale sans les lignes et les colonnes dont les déplacements sont connus ; cette même matrice qui permet de déterminer le déplacement des noeuds. La matrice de rigidité complète permet, sur la base du calcul des déplacements, de déterminer les efforts aux noeuds et d'en déduire les sollicitations le long des barres. La position de la ligne / colonne dans la matrice se déterminera par la lecture de n_part[10][1] pour, par exemple, la position du noeud dont le numéro est le 10 et comme degré de liberté y. Si la valeur renvoyée est -1, cela signifie que le déplacement ou la rotation est bloquée et que le degré de liberté ne figure pas dans la matrice de rigidité partielle.
 */
class CCalculs
{
  // Associations
  // Attributes
  private :
    /// Paramètres des calculs de la librairie cholmod.
    cholmod_common * c;
    /// La position du noeud avec son degré de liberté dans la matrice partielle. Calcul : t_for_part->i[n_part[g_list_index (p->modele.noeuds, noeud)][0à5]]. Dans cette matrice de rigidité partielle, n_part renvoie UINT32_MAX si le degré de liberté est bloqué.
    std::array <std::vector <uint32_t>, 6> n_part;
    /// La position du noeud avec son degré de liberté dans la matrice complète. Calcul : t_for_part->i[n_part[g_list_index (p->modele.noeuds, noeud)][0à5]].
    std::array <std::vector <uint32_t>, 6> n_comp;
    /// Triplet contenant la matrice de rigidité. Variable temporaire avant transformation en matrice sparse. Les éléments sont ajoutés au fur et à mesure que la rigidité des barres est ajoutée dans la matrice de rigidité.
    cholmod_triplet * t_part;
    /// Triplet contenant la matrice de rigidité complète.
    cholmod_triplet * t_comp;
    /// Numéro de la prochaine donnée du triplet t_part qui est à compléter.
    uint32_t t_part_en_cours;
    /// Numéro de la prochaine donnée du triplet t_comp qui est à compléter.
    uint32_t t_comp_en_cours;
    /// Matrice de rigidité partielle t_part en format sparse.
    cholmod_sparse * m_part;
    /// Matrice de rigidité globale t_comp en format sparse.
    cholmod_sparse * m_comp;
    /// Variable temporaire utilisée par la fonction umfpack_di_solve. Utilisée pour le calcul de la matrice partielle lors de la résolution de chaque cas de charges.
    void * numeric;
    /// Pointeur vers la colonne de la matrice t_part.
    int * ap;
    /// La colonne j de la matrice est définie par Ai [(Ap [j]) … (Ap [j+1]-1)].
    int * ai;
    /// Le résultat. Ax [(Ap [j]) ... (Ap [j+1]-1)].
    double * ax;
    /// Erreur non relative des réactions d'appuis.
    double residu;
    /// Le rapport d'analyse.
    std::list <POCO::calc::CAnalyseComm *> rapport;
    /// Une info de calcul pour chaque barre et par élément discrétisé + 1.
    std::vector < std::vector <POCO::calc::CBarreInfoEF *> > info_EF;
    /// Pondérations conformes aux Eurocodes.
    POCO::calc::CPonderations ponderations;
  // Operations
  public :
    /**
     * \brief Constructeur d'une classe CCalculs.
     */
    CCalculs ();
    /**
     * \brief Duplication d'une classe CCalculs.
     * \param other (in) La classe à dupliquer.
     */
    CCalculs (const CCalculs & other) = delete;
    /**
     * \brief Duplication d'une classe CCalculs.
     * \param other (in) La classe à dupliquer.
     * \return CCalculs &
     */
    CCalculs & operator = (const CCalculs & other) = delete;
    /**
     * \brief Destructeur d'une classe CCalculs.
     */
    virtual ~CCalculs ();
};

#endif
