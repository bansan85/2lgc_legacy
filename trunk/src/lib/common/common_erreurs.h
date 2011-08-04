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

#ifndef __COMMON_ERREURS_H
#define __COMMON_ERREURS_H

#include <stdlib.h>
#include <stdio.h>

#define BUG(X) ({free((int*)1); return X;})
// La macro BUG(X) est l'équivalent d'un return X; cependant, afin de 
// faciliter le débogage des erreurs via gdb, il est rajouté, pour la
// version expérimentale du programme un "free((int*)1) qui s'assure de
// créer une erreur de segmentation. Il va de soit que pour la version
// de production, la commande free() doit être supprimée.

#define BUGTEXTE(X, ...) ({printf(__VA_ARGS__); free((int*)1); return X;})
// La macro BUGTEXTE(X) est identique à la commande BUG mais ajoute un
// message d'erreur avant de réaliser l'erreur de segmentation. D'une
// maniète générale, BUGTEXTE doit être utilisé dès que l'erreur arrive
// et BUG doit être utilisé pour indiquer une erreur par la valeur retour
// des fonctions utilisant déjà la macro BUGTEXTE.

#endif
