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

#include "common_projet.h"

typedef enum
{
	EF_APPUI_LIBRE,
	EF_APPUI_BLOQUE
} Type_EF_Appui;

typedef struct __EF_Appui
{
	int		numero;
	Type_EF_Appui	x;
	void		*x_donnees;
	Type_EF_Appui	y;
	void		*y_donnees;
	Type_EF_Appui	z;
	void		*z_donnees;
	Type_EF_Appui	rx;
	void		*rx_donnees;
	Type_EF_Appui	ry;
	void		*ry_donnees;
	Type_EF_Appui	rz;
	void		*rz_donnees;
} EF_Appui;

int EF_appuis_init(Projet *projet);
int EF_appuis_ajout(Projet *projet, Type_EF_Appui x, Type_EF_Appui y, Type_EF_Appui z, Type_EF_Appui rx, Type_EF_Appui ry, Type_EF_Appui rz);
int EF_appuis_cherche_numero(Projet *projet, int numero);
int EF_appuis_free(Projet *projet);

#endif
