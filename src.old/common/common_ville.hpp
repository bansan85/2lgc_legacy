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

#ifndef __COMMON_VILLE_H
#define __COMMON_VILLE_H

#include "config.h"
#include "common_projet.hpp"

bool     common_ville_init      (Projet   *p)
                                       __attribute__((__warn_unused_result__));
bool     common_ville_get_ville (wchar_t  *ligne,
                                 uint16_t *cdc,
                                 uint16_t *cheflieu,
                                 uint16_t *reg,
                                 wchar_t  *dep,
                                 uint32_t *com,
                                 uint16_t *ar,
                                 uint16_t *ct,
                                 uint16_t *tncc,
                                 wchar_t **artmaj,
                                 wchar_t **ncc,
                                 wchar_t **artmin,
                                 wchar_t **nccenr,
                                 uint32_t *code_postal,
                                 uint32_t *altitude,
                                 uint32_t *population)
                                       __attribute__((__warn_unused_result__));
bool     common_ville_set       (Projet       *p,
                                 wchar_t      *departement,
                                 wchar_t      *ville,
                                 bool      graphique_seul)
                                       __attribute__((__warn_unused_result__));
bool     common_ville_free      (Projet       *p)
                                       __attribute__((__warn_unused_result__));

#endif

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
