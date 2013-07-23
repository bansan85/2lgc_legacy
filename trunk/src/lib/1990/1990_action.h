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

#ifndef __1990_ACTIONS_H
#define __1990_ACTIONS_H

#include "config.h"
#include "common_projet.h"

char* _1990_action_type_bat_txt(unsigned int type, Type_Pays pays) __attribute__((__warn_unused_result__));
Action_Categorie _1990_action_categorie_bat(unsigned int type, Type_Pays pays) __attribute__((__warn_unused_result__));
unsigned int _1990_action_num_bat_txt(Type_Pays pays) __attribute__((__warn_unused_result__));

gboolean _1990_action_init(Projet *projet) __attribute__((__warn_unused_result__));

Action* _1990_action_ajout(Projet *projet, unsigned int categorie, const char* description) __attribute__((__warn_unused_result__));

const char* _1990_action_nom_renvoie(Action *action) __attribute__((__warn_unused_result__));
gboolean _1990_action_nom_change(Projet *projet, Action *action, const char* description) __attribute__((__warn_unused_result__));

unsigned int _1990_action_numero_renvoie(Action *action) __attribute__((__warn_unused_result__));
Action* _1990_action_numero_cherche(Projet *projet, unsigned int numero) __attribute__((__warn_unused_result__));

unsigned int _1990_action_type_renvoie(Action *action) __attribute__((__warn_unused_result__));
gboolean _1990_action_type_change(Projet *projet, Action *action, unsigned int type) __attribute__((__warn_unused_result__));

gboolean _1990_action_charges_vide(Action *action) __attribute__((__warn_unused_result__));
GList* _1990_action_charges_renvoie(Action *action) __attribute__((__warn_unused_result__));
gboolean _1990_action_charges_change(Action *action, GList *charges) __attribute__((__warn_unused_result__));

unsigned int _1990_action_flags_action_predominante_renvoie(Action *action) __attribute__((__warn_unused_result__));
gboolean _1990_action_flags_action_predominante_change(Action *action, unsigned int flag) __attribute__((__warn_unused_result__));

Flottant _1990_action_psi_renvoie_0(Action *action) __attribute__((__warn_unused_result__));
Flottant _1990_action_psi_renvoie_1(Action *action) __attribute__((__warn_unused_result__));
Flottant _1990_action_psi_renvoie_2(Action *action) __attribute__((__warn_unused_result__));
gboolean _1990_action_psi_change(Projet *projet, Action *action, unsigned int psi_num, Flottant psi) __attribute__((__warn_unused_result__));

cholmod_sparse* _1990_action_deplacement_complet_renvoie(Action *action) __attribute__((__warn_unused_result__));
gboolean _1990_action_deplacement_complet_change(Action *action, cholmod_sparse *sparse) __attribute__((__warn_unused_result__));
cholmod_sparse* _1990_action_forces_complet_renvoie(Action *action) __attribute__((__warn_unused_result__));
gboolean _1990_action_forces_complet_change(Action *action, cholmod_sparse *sparse) __attribute__((__warn_unused_result__));
cholmod_sparse* _1990_action_efforts_noeuds_renvoie(Action *action) __attribute__((__warn_unused_result__));
gboolean _1990_action_efforts_noeuds_change(Action *action, cholmod_sparse *sparse) __attribute__((__warn_unused_result__));

GtkTreeIter* _1990_action_Iter_fenetre_renvoie(Action *action) __attribute__((__warn_unused_result__));
gboolean _1990_action_Iter_fenetre_change(Action *action, GtkTreeIter iter) __attribute__((__warn_unused_result__));

Fonction* _1990_action_fonctions_efforts_renvoie(Action *action, int ligne, int barre) __attribute__((__warn_unused_result__));
Fonction* _1990_action_fonctions_rotation_renvoie(Action *action, int ligne, int barre) __attribute__((__warn_unused_result__));
Fonction* _1990_action_fonctions_deformation_renvoie(Action *action, int ligne, int barre) __attribute__((__warn_unused_result__));

gboolean _1990_action_fonction_init(Projet *projet, Action *action) __attribute__((__warn_unused_result__));
gboolean _1990_action_fonction_free(Projet *projet, Action *action) __attribute__((__warn_unused_result__));

gboolean _1990_action_affiche_tout(Projet *projet) __attribute__((__warn_unused_result__));
gboolean _1990_action_affiche_resultats(Projet *projet, unsigned int num_action) __attribute__((__warn_unused_result__));

Action *_1990_action_ponderation_resultat(GList* ponderation, Projet *projet) __attribute__((__warn_unused_result__));
gboolean _1990_action_ponderation_resultat_free_calculs(Action *action) __attribute__((__warn_unused_result__));

gboolean _1990_action_free_calculs(Projet *projet, Action *action) __attribute__((__warn_unused_result__));
gboolean _1990_action_free_num(Projet *projet, unsigned int num) __attribute__((__warn_unused_result__));
gboolean _1990_action_free(Projet *projet) __attribute__((__warn_unused_result__));

#endif
