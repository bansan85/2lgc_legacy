/*
 * 2lgc_code : calcul de résistance des matériaux selon les normes Eurocodes
 * Copyright (C) 2011
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __1992_1_1_ELEMENTS_H
#define __1992_1_1_ELEMENTS_H

#include "EF_relachement.h"
#include "EF_noeud.h"
#include "1992_1_1_section.h"
#include "1992_1_1_materiaux.h"
#include "common_projet.h"
#include "cholmod.h"

typedef struct __Beton_Element_Poutre
{
	unsigned int		numero;
	Type_Element		element;
	void			*section;
	Beton_Materiau		*materiau;
	
	EF_Noeud		*noeud_debut;
	EF_Noeud		*noeud_fin;
	unsigned int		discretisation_element; // Nombre de noeuds intermédiaires
	EF_Noeud		**noeuds_intermediaires;
	Relachement		*relachement;
	
	cholmod_sparse		*matrice_rotation;
	cholmod_sparse		*matrice_rigidite_locale;
	cholmod_sparse		*matrice_rigidite_globale;
} Beton_Element;

int _1992_1_1_elements_init(Projet *projet);
int _1992_1_1_elements_ajout(Projet *projet, Type_Element type, unsigned int section, unsigned int materiau, unsigned int noeud_debut, unsigned int noeud_fin, int relachement, unsigned int discretisation_element);
int _1992_1_1_elements_cherche_numero(Projet *projet, unsigned int numero);
int _1992_1_1_elements_rigidite_ajout(Projet *projet, unsigned int element);
int _1992_1_1_elements_free(Projet *projet);


#endif
