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

#include "config.h"
#include <libintl.h>
#include <locale.h>
#include <gmodule.h>
#include <stdio.h>


G_MODULE_EXPORT void show_warranty()
/* Description : Affiche les limites de la garantie (articles 15, 16 et 17 de la licence GPL).
 * Paramètres : Aucun.
 * Valeur renvoyée : Aucune.
 */
{
    // Trivial
    printf(gettext("15. Disclaimer of Warranty.\n"));
    printf(gettext("\n"));
    printf(gettext("THERE IS NO WARRANTY FOR THE PROGRAM, TO THE EXTENT PERMITTED BY\n"));
    printf(gettext("APPLICABLE LAW.  EXCEPT WHEN OTHERWISE STATED IN WRITING THE COPYRIGHT\n"));
    printf(gettext("HOLDERS AND/OR OTHER PARTIES PROVIDE THE PROGRAM \"AS IS\" WITHOUT WARRANTY\n"));
    printf(gettext("OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO,\n"));
    printf(gettext("THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR\n"));
    printf(gettext("PURPOSE.  THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE PROGRAM\n"));
    printf(gettext("IS WITH YOU.  SHOULD THE PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF\n"));
    printf(gettext("ALL NECESSARY SERVICING, REPAIR OR CORRECTION.\n"));
    printf(gettext("\n"));
    printf(gettext("16. Limitation of Liability.\n"));
    printf(gettext("\n"));
    printf(gettext("IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN WRITING\n"));
    printf(gettext("WILL ANY COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO MODIFIES AND/OR CONVEYS\n"));
    printf(gettext("THE PROGRAM AS PERMITTED ABOVE, BE LIABLE TO YOU FOR DAMAGES, INCLUDING ANY\n"));
    printf(gettext("GENERAL, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE\n"));
    printf(gettext("USE OR INABILITY TO USE THE PROGRAM (INCLUDING BUT NOT LIMITED TO LOSS OF\n"));
    printf(gettext("DATA OR DATA BEING RENDERED INACCURATE OR LOSSES SUSTAINED BY YOU OR THIRD\n"));
    printf(gettext("PARTIES OR A FAILURE OF THE PROGRAM TO OPERATE WITH ANY OTHER PROGRAMS),\n"));
    printf(gettext("EVEN IF SUCH HOLDER OR OTHER PARTY HAS BEEN ADVISED OF THE POSSIBILITY OF\n"));
    printf(gettext("SUCH DAMAGES.\n"));
    printf(gettext("\n"));
    printf(gettext("17. Interpretation of Sections 15 and 16.\n"));
    printf(gettext("\n"));
    printf(gettext("If the disclaimer of warranty and limitation of liability provided\n"));
    printf(gettext("above cannot be given local legal effect according to their terms,\n"));
    printf(gettext("reviewing courts shall apply local law that most closely approximates\n"));
    printf(gettext("an absolute waiver of all civil liability in connection with the\n"));
    printf(gettext("Program, unless a warranty or assumption of liability accompanies a\n"));
    printf(gettext("copy of the Program in return for a fee.\n"));
    return;
}

G_MODULE_EXPORT void show_help()
/* Description : Affiche l'aide lorsque l'utilisateur lance le programme avec l'option -h.
 * Paramètres : Aucun.
 * Valeur renvoyée : Aucune.
 */
{
    // -h, --help : affiche le présent menu
    // -w, --warranty : affiche les limites de garantie du logiciel
    printf(gettext("Utilisation : codegui [OPTION]... [FILE]...\n"));
    printf(gettext("Options :\n"));
    printf(gettext("\t-h, --help : affiche le présent menu\n"));
    printf(gettext("\t-w, --warranty : affiche les limites de garantie du logiciel\n"));
    return;
}
