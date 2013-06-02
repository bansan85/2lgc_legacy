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

#ifndef __1992_1_1_BARRES_H
#define __1992_1_1_BARRES_H

#include "config.h"
#include "common_projet.h"


gboolean _1992_1_1_barres_init(Projet *projet) __attribute__((__warn_unused_result__));

gboolean _1992_1_1_barres_ajout(Projet *projet, Type_Element type, EF_Section *section, EF_Materiau *materiau, unsigned int noeud_debut, unsigned int noeud_fin, EF_Relachement* relachement, unsigned int discretisation_element) __attribute__((__warn_unused_result__));

Beton_Barre* _1992_1_1_barres_cherche_numero(Projet *projet, unsigned int numero, gboolean critique) __attribute__((__warn_unused_result__));
gboolean _1992_1_1_barres_cherche_dependances(Projet *projet, GList *appuis, GList* noeuds, GList* sections, GList* materiaux, GList *relachements, GList* barres, GList** noeuds_dep, GList** barres_dep, GList **charges_dep, gboolean numero, gboolean origine) __attribute__((__warn_unused_result__));
gboolean _1992_1_1_barres_angle_rotation(EF_Noeud *debut, EF_Noeud *fin, double *y, double *z) __attribute__((__warn_unused_result__));

gboolean _1992_1_1_barres_change_type(Beton_Barre *barre, Type_Element type, Projet *projet) __attribute__((__warn_unused_result__));
gboolean _1992_1_1_barres_change_section(Beton_Barre *barre, EF_Section *section, Projet *projet) __attribute__((__warn_unused_result__));
gboolean _1992_1_1_barres_change_materiau(Beton_Barre *barre, EF_Materiau *materiau, Projet *projet) __attribute__((__warn_unused_result__));
gboolean _1992_1_1_barres_change_noeud(Beton_Barre *barre, EF_Noeud *noeud, gboolean noeud_1, Projet *projet) __attribute__((__warn_unused_result__));
gboolean _1992_1_1_barres_change_relachement(Beton_Barre *barre, EF_Relachement *relachement, Projet *projet) __attribute__((__warn_unused_result__));

gboolean _1992_1_1_barres_rigidite_ajout(Projet *projet, Beton_Barre *element) __attribute__((__warn_unused_result__));
gboolean _1992_1_1_barres_rigidite_ajout_tout(Projet *projet) __attribute__((__warn_unused_result__));

gboolean _1992_1_1_barres_supprime_liste(Projet *projet, GList *liste_noeuds, GList *liste_barres) __attribute__((__warn_unused_result__));
void _1992_1_1_barres_free_foreach(Beton_Barre *barre, Projet *projet);
gboolean _1992_1_1_barres_free(Projet *projet) __attribute__((__warn_unused_result__));

#endif
