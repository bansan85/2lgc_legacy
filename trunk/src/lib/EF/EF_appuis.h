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


gboolean EF_appuis_init         (Projet       *p)
                                       __attribute__((__warn_unused_result__));

EF_Appui* EF_appuis_cherche_nom (Projet       *p,
                                 const char   *nom,
                                 gboolean      critique)
                                       __attribute__((__warn_unused_result__));

EF_Appui *EF_appuis_ajout       (Projet       *p,
                                 const char   *nom,
                                 Type_EF_Appui x,
                                 Type_EF_Appui y,
                                 Type_EF_Appui z,
                                 Type_EF_Appui rx,
                                 Type_EF_Appui ry,
                                 Type_EF_Appui rz)
                                       __attribute__((__warn_unused_result__));

gboolean EF_appuis_edit         (EF_Appui     *appui,
                                 int           x,
                                 Type_EF_Appui type_x,
                                 Projet       *p)
                                       __attribute__((__warn_unused_result__));
gboolean EF_appuis_renomme      (EF_Appui     *appui,
                                 const char   *nom,
                                 Projet       *p,
                                 gboolean      critique)
                                       __attribute__((__warn_unused_result__));

gboolean EF_appuis_supprime     (EF_Appui     *appui,
                                 gboolean      annule_si_utilise,
                                 gboolean      supprime,
                                 Projet       *p)
                                       __attribute__((__warn_unused_result__));
gboolean EF_appuis_free         (Projet       *p)
                                       __attribute__((__warn_unused_result__));

#endif

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
