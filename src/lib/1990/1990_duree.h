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

#ifndef __1990_DUREE_H
#define __1990_DUREE_H

// _1990_duree_projet_eu : renvoie la durée (en année) indicative de la norme européenne
// Dans le cas où la durée indiquée par la norme européenne est un
// intervalle d'année, la durée la plus importante est renvoyée.
// Si la valeur de type ne correspond pas à une catégorie, la fonction renvoie -1.
// type : categorie de durée d'utilisation de projet
int _1990_duree_projet_eu(int type);
// duree_projet_eu : renvoie la durée (en année) indicative de la norme française
int _1990_duree_projet_fr(int type);

// _1990_duree_projet_txt_eu : renvoie la description des catérogies des durées indicative de la norme européenne
// Renvoie NULL si la catégorie n'existe pas
// type : catégorie de durée d'utilisation de projet
char *_1990_duree_projet_txt_eu(int type);
// _1990_duree_projet_txt_fr : renvoie la description des catérogies des durées indicative de la norme française
char *_1990_duree_projet_txt_fr(int type);

#endif
