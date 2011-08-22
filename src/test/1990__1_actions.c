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

#include "1990_actions.h"
#include "common_erreurs.h"


int main(void)
{
    int i;
    
    setlocale( LC_ALL, "" );
    bindtextdomain(PACKAGE, LOCALEDIR);
    textdomain(PACKAGE);
    
    for (i=0;i<=16;i++)
        BUG(_1990_action_type_bat_txt(i, PAYS_EU), -1);
    
    for (i=0;i<=21;i++)
        BUG(_1990_action_type_bat_txt(i, PAYS_FR), -1);
    
    BUG(_1990_action_categorie_bat(0, PAYS_EU) == ACTION_POIDS_PROPRE, -1);
    BUG(_1990_action_categorie_bat(1, PAYS_EU) == ACTION_PRECONTRAINTE, -1);
    for (i=2;i<=14;i++)
        BUG(_1990_action_categorie_bat(i, PAYS_EU) == ACTION_VARIABLE, -1);
    BUG(_1990_action_categorie_bat(15, PAYS_EU) == ACTION_ACCIDENTELLE, -1);
    BUG(_1990_action_categorie_bat(16, PAYS_EU) == ACTION_SISMIQUE, -1);
    
    BUG(_1990_action_categorie_bat(0, PAYS_FR) == ACTION_POIDS_PROPRE, -1);
    BUG(_1990_action_categorie_bat(1, PAYS_FR) == ACTION_PRECONTRAINTE, -1);
    for (i=2;i<=18;i++)
        BUG(_1990_action_categorie_bat(i, PAYS_FR) == ACTION_VARIABLE, -1);
    BUG(_1990_action_categorie_bat(19, PAYS_FR) == ACTION_ACCIDENTELLE, -1);
    BUG(_1990_action_categorie_bat(20, PAYS_FR) == ACTION_SISMIQUE, -1);
    BUG(_1990_action_categorie_bat(21, PAYS_FR) == ACTION_EAUX_SOUTERRAINES, -1);
    
/*    BUG(_1990_action_type_bat_txt_eu(17) == NULL, -1);
    BUG(_1990_action_type_bat_txt_fr(22) == NULL, -1);
    BUG(_1990_action_categorie_bat(17) == -1, -1);
    BUG(_1990_action_categorie_bat(22) == -1, -1);*/
    
    return EXIT_SUCCESS;
}
