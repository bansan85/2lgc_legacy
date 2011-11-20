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

#ifndef __EF_APPUI_H
#define __EF_APPUI_H

#include "config.h"
#include "common_projet.h"


int EF_appuis_init(Projet *projet);
int EF_appuis_ajout(Projet *projet, Type_EF_Appui x, Type_EF_Appui y, Type_EF_Appui z, Type_EF_Appui rx, Type_EF_Appui ry, Type_EF_Appui rz);
EF_Appui* EF_appuis_cherche_numero(Projet *projet, int numero);
int EF_appuis_free(Projet *projet);

#endif
