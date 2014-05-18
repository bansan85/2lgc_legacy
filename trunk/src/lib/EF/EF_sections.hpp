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

#ifndef __1992_1_1_SECTION_H
#define __1992_1_1_SECTION_H

#include "config.h"
#include "common_projet.hpp"

bool EF_sections_init                    (Projet     *p)
                                       __attribute__((__warn_unused_result__));

Section *EF_sections_rectangulaire_ajout (Projet      *p,
                                          std::string *nom,
                                          Flottant     l,
                                          Flottant     h)
                                       __attribute__((__warn_unused_result__));
bool EF_sections_rectangulaire_modif     (Projet      *p,
                                          Section     *section,
                                          std::string *nom,
                                          Flottant     l,
                                          Flottant     h)
                                       __attribute__((__warn_unused_result__));

Section *EF_sections_T_ajout (Projet      *p,
                              std::string *nom,
                              Flottant     lt,
                              Flottant     lr,
                              Flottant     ht,
                              Flottant     hr)
                                       __attribute__((__warn_unused_result__));
bool EF_sections_T_modif     (Projet      *p,
                              Section     *section,
                              std::string *nom,
                              Flottant     lt,
                              Flottant     lr,
                              Flottant     ht,
                              Flottant     hr)
                                       __attribute__((__warn_unused_result__));

Section *EF_sections_carree_ajout (Projet      *p,
                                   std::string *nom,
                                   Flottant     cote)
                                       __attribute__((__warn_unused_result__));
bool EF_sections_carree_modif     (Projet      *p,
                                   Section     *section,
                                   std::string *nom,
                                   Flottant     cote)
                                       __attribute__((__warn_unused_result__));

Section *EF_sections_circulaire_ajout (Projet      *p,
                                       std::string *nom,
                                       Flottant     diametre)
                                       __attribute__((__warn_unused_result__));
bool EF_sections_circulaire_modif     (Projet      *p,
                                       Section     *section,
                                       std::string *nom,
                                       Flottant     diametre)
                                       __attribute__((__warn_unused_result__));

bool EF_sections_personnalisee_verif_forme (
       std::list <std::list <EF_Point *> *> *forme,
       bool        message)
                                       __attribute__((__warn_unused_result__));
Section *EF_sections_personnalisee_ajout   (
           Projet      *p,
           std::string *nom,
           std::string *description,
           Flottant     j,
           Flottant     iy,
           Flottant     iz,
           Flottant     vy,
           Flottant     vyp,
           Flottant     vz,
           Flottant     vzp,
           Flottant     s,
           std::list <std::list <EF_Point *> *> *forme)
                                       __attribute__((__warn_unused_result__));
bool     EF_sections_personnalisee_modif   (
           Projet      *p,
           Section     *section,
           std::string *nom,
           std::string *description,
           Flottant     j,
           Flottant     iy,
           Flottant     iz,
           Flottant     vy,
           Flottant     vyp,
           Flottant     vz,
           Flottant     vzp,
           Flottant     s,
           std::list <std::list <EF_Point *> *> *forme)
                                       __attribute__((__warn_unused_result__));
void     EF_sections_personnalisee_free_forme1 (
           std::list <EF_Point *> *forme_e);

Section *EF_sections_cherche_nom        (Projet      *p,
                                         std::string *nom,
                                         bool         critique)
                                       __attribute__((__warn_unused_result__));
std::string EF_sections_get_description (Section    *sect)
                                       __attribute__((__warn_unused_result__));

bool     EF_sections_supprime           (Section    *section,
                                         bool        annule_si_utilise,
                                         Projet     *p)
                                       __attribute__((__warn_unused_result__));

Flottant EF_sections_j   (Section  *section)
                                       __attribute__((__warn_unused_result__));
Flottant EF_sections_iy  (Section  *section)
                                       __attribute__((__warn_unused_result__));
Flottant EF_sections_iz  (Section  *section)
                                       __attribute__((__warn_unused_result__));
Flottant EF_sections_vy  (Section  *sect)
                                       __attribute__((__warn_unused_result__));
Flottant EF_sections_vyp (Section  *sect)
                                       __attribute__((__warn_unused_result__));
Flottant EF_sections_vz  (Section  *sect)
                                       __attribute__((__warn_unused_result__));
Flottant EF_sections_vzp (Section  *sect)
                                       __attribute__((__warn_unused_result__));

double   EF_sections_ay  (EF_Barre *barre,
                          uint16_t  discretisation)
                                       __attribute__((__warn_unused_result__));
double   EF_sections_by  (EF_Barre *barre,
                          uint16_t  discretisation)
                                       __attribute__((__warn_unused_result__));
double   EF_sections_cy  (EF_Barre *barre,
                          uint16_t  discretisation)
                                       __attribute__((__warn_unused_result__));
double   EF_sections_az  (EF_Barre *barre,
                          uint16_t  discretisation)
                                       __attribute__((__warn_unused_result__));
double   EF_sections_bz  (EF_Barre *barre,
                          uint16_t  discretisation)
                                       __attribute__((__warn_unused_result__));
double   EF_sections_cz  (EF_Barre *barre,
                          uint16_t  discretisation)
                                       __attribute__((__warn_unused_result__));

Flottant EF_sections_s   (Section  *sect)
                                       __attribute__((__warn_unused_result__));
double   EF_sections_es_l (EF_Barre *barre,
                           uint16_t  discretisation,
                           double    debut,
                           double    fin)
                                       __attribute__((__warn_unused_result__));
double   EF_sections_gj_l (EF_Barre *barre,
                           uint16_t  discretisation)
                                       __attribute__((__warn_unused_result__));

bool     EF_sections_free (Projet   *p)
                                       __attribute__((__warn_unused_result__));

#endif

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
