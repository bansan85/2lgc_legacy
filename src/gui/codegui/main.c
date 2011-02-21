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
#include "cmd_text.h"
#include <stdio.h>
#include <stdlib.h>
#include <libintl.h>
#include <locale.h>


int main(int argc, char *argv[])
{
	setlocale( LC_ALL, "" );
	bindtextdomain(PACKAGE, LOCALEDIR);
	textdomain(PACKAGE);
	
	switch (argc)
	{
		case 2:
		{
			if ((strcmp(argv[1], "-w") == 0) || (strcmp(argv[1], "--warranty") == 0))
			{
				show_warranty();
			}
			else if ((strcmp(argv[1], "-h") == 0) || (strcmp(argv[1], "--help") == 0))
			{
				show_help();
			}
			break;
		}
		default:
		{
			break;
		}
	}

	return 0;
}

