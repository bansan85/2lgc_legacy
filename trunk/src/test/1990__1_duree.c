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
#include <stdio.h>
#include <stdlib.h>
#include <libintl.h>
#include <locale.h>

#include <list.h>

#include "1990_duree.h"
#include "1990_coef_psi.h"
#include "1990_actions.h"
#include "1990_groupes.h"
#include "common_maths.h"
#include "common_erreurs.h"
#include "common_projet.h"


int main(void)
{
    int i;
    
    setlocale( LC_ALL, "" );
    bindtextdomain(PACKAGE, LOCALEDIR);
    textdomain(PACKAGE);
    
    // Test 1990_duree.c
    
    BUG(_1990_duree_projet(0, PAYS_EU) == 10, -1);
    BUG(_1990_duree_projet(1, PAYS_EU) == 25, -1);
    BUG(_1990_duree_projet(2, PAYS_EU) == 30, -1);
    BUG(_1990_duree_projet(3, PAYS_EU) == 50, -1);
    BUG(_1990_duree_projet(4, PAYS_EU) == 100, -1);
    
    BUG(_1990_duree_projet(0, PAYS_FR) == 10, -1);
    BUG(_1990_duree_projet(1, PAYS_FR) == 25, -1);
    BUG(_1990_duree_projet(2, PAYS_FR) == 25, -1);
    BUG(_1990_duree_projet(3, PAYS_FR) == 50, -1);
    BUG(_1990_duree_projet(4, PAYS_FR) == 100, -1);
    
    for (i=0;i<=4;i++)
        BUG(_1990_duree_projet_txt(i, PAYS_EU), -1);
    
    for (i=0;i<=4;i++)
        BUG(_1990_duree_projet_txt(i, PAYS_FR), -1);

/*    BUG(_1990_duree_projet_eu(5) == -1, -1);
    BUG(_1990_duree_projet_fr(5) == -1, -1);
    BUG(_1990_duree_projet_txt_eu(5) == NULL, -1);
    BUG(_1990_duree_projet_txt_fr(5) == NULL, -1);*/
    
    return EXIT_SUCCESS;
}
