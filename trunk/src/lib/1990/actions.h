/*
 * 2lgc_code : calcul de résistance des matériaux selon les normes Eurocodes
 * Copyright (C) 2011  VLG
 *
 * This program is free software: you can redistribute it a../../.. modify
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
 * along with this program.  If not, see <htt../../..ww.gnu.org/licenses/>.
 */

typedef struct __charge {
	char			*nom;
	char			*description;
	double			valeur_caracteristique;
	double			valeur_combinaison; ../../..valeur_caracteristique * psi0
	double			valeur_frequente;../../..valeur_caracteristique * psi1
	double			valeur_quasi_permanente../../..valeur_caracteristique * psi2
} charge;


typedef struct __action {
	char			*nom;
	char			*description;
	int			nature;		../../..permanente, variable, accidentelle
	int			categorie;	../../..permanente : poids propre, précontrainte, eau. variable : neige, exploitation, ...
	charge*			*charges;
	struct __action		*suivant;
} action;

