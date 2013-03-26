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
#include <string.h>

#include "common_projet.h"
#include "common_erreurs.h"
#include "common_math.h"
#include "common_text.h"

gboolean common_ville_init(Projet *projet)
/* Description : Initialise à NULL l'adresse du projet. Met également à défaut 
 * Paramètres : Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : NULL :
 *             projet == NULL.
 */
{
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    
    projet->parametres.adresse.destinataire = NULL;
    projet->parametres.adresse.adresse = NULL;
    projet->parametres.adresse.ville = NULL;
    projet->parametres.adresse.departement = 0;
    projet->parametres.adresse.num_ville = 0;
    projet->parametres.adresse.code_postal = 0;
    
    // On initialise au moins à une valeur par défaut
    projet->parametres.neige = 0;
    projet->parametres.vent = 0;
    projet->parametres.seisme = 0;
    
    return TRUE;
}


gboolean common_ville_set(Projet *projet, int departement, int ville)
/* Description : Initialise la ville (mais pas l'adresse exacte) du projet avec les paramètres
 *               régionaux (vent, neige, séisme).
 *               Departement DEP et ville COM doivent correspondre au fichier france_villes.csv
 * Paramètres : Projet *projet : la variable projet,
 *              int departement : le numéro du département,
 *              int ville : le numéro de la ville. Nécessite d'avoir parcouru france_villes.csv.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : NULL :
 *             en cas d'erreur d'allocation mémoire.
 */
{
    FILE    *villes;
    char    *ligne = NULL;
    int     cdc, cheflieu, reg, dep, com, ar, ct, tncc;
    char    /**artmaj, *ncc, */*artmin, *nccenr;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    
    BUGMSG(villes = fopen(DATADIR"/france_villes.csv", "r"), FALSE, gettext("Le fichier '%s' est introuvable.\n"), DATADIR"/france_villes.csv");
    
    // On passe la première ligne qui est l'étiquette des colonnes.
    ligne = common_text_get_line(villes);
    free(ligne);
    
    ligne = common_text_get_line(villes);
    do
    {
        // On récupère les numéros caractéristant la ville en cours.
        sscanf(ligne, "%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t", &cdc, &cheflieu, &reg, &dep, &com, &ar, &ct, &tncc);
        if ((dep == departement) && (ville == com))
        {
            int     dep_parcours;
            int     code_postal, altitude, population;
            int     i, j;
            char    *champ1, *champ2;
            
            i = 0;
            j = 0;
            // On est obligé de traiter à la main car sscanf("1\t\t3", "%s\t%s\t%s", un, deux,
            // trois); renvoie : un = 1, deux = 3 et trois non initialisé.
            while (i != 12)
            {
                if (ligne[j] == '\t')
                {
                    ligne[j] = '\000';
                    i++;
                }
                j++;
/*                if (i == 7)
                    artmaj = &(ligne[j+1]);
                else if (i == 8)
                    ncc = &(ligne[j+1]);
                else */if (i == 9)
                    artmin = &(ligne[j+1]);
                else if (i == 10)
                    nccenr = &(ligne[j+1]);
            }
            sscanf(&(ligne[j]), "%d\t%d\t%d\n", &code_postal, &altitude, &population);
            free(ligne);
            
            // Maintenant que tout est récupéré, on enregistre dans le projet ce qu'il faut.
            // Ville et code postal.
            projet->parametres.adresse.code_postal = code_postal;
            BUGMSG(projet->parametres.adresse.ville = g_strdup_printf("%s %s", artmin, nccenr), FALSE, gettext("Erreur d'allocation mémoire.\n"));
            projet->parametres.adresse.code_postal = code_postal;
            
            // Le zonage neige.
            BUGMSG(villes = fopen(DATADIR"/france_neige.csv", "r"), FALSE, gettext("Le fichier '%s' est introuvable.\n"), DATADIR"/france_neige.csv");
            // On commande par chercher le département
            dep_parcours = 0;
            while (dep_parcours == 0)
            {
                BUGMSG(ligne = common_text_get_line(villes), FALSE, gettext("Le fichier '%s' est incomplet.\n"), DATADIR"/france_neige.csv");
                
                BUGMSG(champ1 = malloc(sizeof(char)*(strlen(ligne)+1)), FALSE, gettext("Erreur d'allocation mémoire.\n"));
                BUGMSG(champ2 = malloc(sizeof(char)*(strlen(ligne)+1)), FALSE, gettext("Erreur d'allocation mémoire.\n"));
                
                BUGMSG(sscanf(ligne, "%s\t%d\t%s\n", champ1, &dep_parcours, champ2) == 3, FALSE, gettext("Le fichier '%s' est corrompu.\n"), DATADIR"/france_neige.csv");
                
                if ((dep_parcours != dep) || (strcmp(champ1, "DEP") != 0))
                {
                    dep_parcours = 0;
                    free(champ2);
                }
                
                free(ligne);
                free(champ1);
            }
            if (strcmp(champ2, "A1") == 0)
                projet->parametres.neige = NEIGE_A1;
            else if (strcmp(champ2, "A2") == 0)
                projet->parametres.neige = NEIGE_A2;
            else if (strcmp(champ2, "B1") == 0)
                projet->parametres.neige = NEIGE_B1;
            else if (strcmp(champ2, "B2") == 0)
                projet->parametres.neige = NEIGE_B2;
            else if (strcmp(champ2, "C1") == 0)
                projet->parametres.neige = NEIGE_C1;
            else if (strcmp(champ2, "C2") == 0)
                projet->parametres.neige = NEIGE_C2;
            else if (strcmp(champ2, "D") == 0)
                projet->parametres.neige = NEIGE_D;
            else if (strcmp(champ2, "E") == 0)
                projet->parametres.neige = NEIGE_E;
            else
                BUGMSG(NULL, FALSE, gettext("Le fichier '%s' est corrumpu. Le champ2 '%s' est inconnu.\n"), DATADIR"/france_neige.csv", champ2);
            free(champ2);
            // On recherche si il y a une exception à la règle de base.
            while (dep_parcours == dep)
            {
                ligne = common_text_get_line(villes);
                // On a atteint la fin du fichier
                if (ligne == NULL)
                    break;
                
                BUGMSG(champ1 = malloc(sizeof(char)*(strlen(ligne)+1)), FALSE, gettext("Erreur d'allocation mémoire.\n"));
                BUGMSG(champ2 = malloc(sizeof(char)*(strlen(ligne)+1)), FALSE, gettext("Erreur d'allocation mémoire.\n"));
                
                BUGMSG(sscanf(ligne, "%s\t%d\t%s\n", champ1, &dep_parcours, champ2) == 3, FALSE, gettext("Le fichier '%s' est corrompu.\n"), DATADIR"/france_neige.csv");
                
                if (strcmp(champ1, "DEP") == 0)
                    dep_parcours = 0;
                else if (((strcmp(champ1, "CAN") == 0) && (dep_parcours == ct)) ||
                        ((strcmp(champ1, "COM") == 0) && (dep_parcours == com)))
                {
                    if (strcmp(champ2, "A1") == 0)
                        projet->parametres.neige = NEIGE_A1;
                    else if (strcmp(champ2, "A2") == 0)
                        projet->parametres.neige = NEIGE_A2;
                    else if (strcmp(champ2, "B1") == 0)
                        projet->parametres.neige = NEIGE_B1;
                    else if (strcmp(champ2, "B2") == 0)
                        projet->parametres.neige = NEIGE_B2;
                    else if (strcmp(champ2, "C1") == 0)
                        projet->parametres.neige = NEIGE_C1;
                    else if (strcmp(champ2, "C2") == 0)
                        projet->parametres.neige = NEIGE_C2;
                    else if (strcmp(champ2, "D") == 0)
                        projet->parametres.neige = NEIGE_D;
                    else if (strcmp(champ2, "E") == 0)
                        projet->parametres.neige = NEIGE_E;
                    else
                        BUGMSG(NULL, FALSE, gettext("Le fichier '%s' est corrumpu. Le champ2 '%s' est inconnu.\n"), DATADIR"/france_neige.csv", champ2);
                    dep_parcours = 0;
                }
                else
                    dep_parcours = dep;
                
                free(ligne);
                free(champ1);
                free(champ2);
            }
            fclose(villes);
            // Fin Neige
            
            // Le zonage vent.
            BUGMSG(villes = fopen(DATADIR"/france_vent.csv", "r"), FALSE, gettext("Le fichier '%s' est introuvable.\n"), DATADIR"/france_vent.csv");
            // On commande par chercher le département
            dep_parcours = 0;
            while (dep_parcours == 0)
            {
                BUGMSG(ligne = common_text_get_line(villes), FALSE, gettext("Le fichier '%s' est incomplet.\n"), DATADIR"/france_vent.csv");
                
                BUGMSG(champ1 = malloc(sizeof(char)*(strlen(ligne)+1)), FALSE, gettext("Erreur d'allocation mémoire.\n"));
                BUGMSG(champ2 = malloc(sizeof(char)*(strlen(ligne)+1)), FALSE, gettext("Erreur d'allocation mémoire.\n"));
                
                BUGMSG(sscanf(ligne, "%s\t%d\t%s\n", champ1, &dep_parcours, champ2) == 3, FALSE, gettext("Le fichier '%s' est corrompu.\n"), DATADIR"/france_vent.csv");
                
                if ((dep_parcours != dep) || (strcmp(champ1, "DEP") != 0))
                {
                    dep_parcours = 0;
                    free(champ2);
                }
                
                free(ligne);
                free(champ1);
            }
            if (strcmp(champ2, "1") == 0)
                projet->parametres.vent = VENT_1;
            else if (strcmp(champ2, "2") == 0)
                projet->parametres.vent = VENT_2;
            else if (strcmp(champ2, "3") == 0)
                projet->parametres.vent = VENT_3;
            else if (strcmp(champ2, "4") == 0)
                projet->parametres.vent = VENT_4;
            else
                BUGMSG(NULL, FALSE, gettext("Le fichier '%s' est corrumpu. Le champ2 '%s' est inconnu.\n"), DATADIR"/france_vent.csv", champ2);
            free(champ2);
            // On recherche si il y a une exception à la règle de base.
            while (dep_parcours == dep)
            {
                ligne = common_text_get_line(villes);
                // On a atteint la fin du fichier
                if (ligne == NULL)
                    break;
                
                BUGMSG(champ1 = malloc(sizeof(char)*(strlen(ligne)+1)), FALSE, gettext("Erreur d'allocation mémoire.\n"));
                BUGMSG(champ2 = malloc(sizeof(char)*(strlen(ligne)+1)), FALSE, gettext("Erreur d'allocation mémoire.\n"));
                
                BUGMSG(sscanf(ligne, "%s\t%d\t%s\n", champ1, &dep_parcours, champ2) == 3, FALSE, gettext("Le fichier '%s' est corrompu.\n"), DATADIR"/france_vent.csv");
                
                if (strcmp(champ1, "DEP") == 0)
                    dep_parcours = 0;
                else if (((strcmp(champ1, "CAN") == 0) && (dep_parcours == ct)) ||
                        ((strcmp(champ1, "COM") == 0) && (dep_parcours == com)))
                {
                    if (strcmp(champ2, "1") == 0)
                        projet->parametres.vent = VENT_1;
                    else if (strcmp(champ2, "2") == 0)
                        projet->parametres.vent = VENT_2;
                    else if (strcmp(champ2, "3") == 0)
                        projet->parametres.vent = VENT_3;
                    else if (strcmp(champ2, "4") == 0)
                        projet->parametres.vent = VENT_4;
                    else
                        BUGMSG(NULL, FALSE, gettext("Le fichier '%s' est corrumpu. Le champ2 '%s' est inconnu.\n"), DATADIR"/france_vent.csv", champ2);
                    dep_parcours = 0;
                }
                else
                    dep_parcours = dep;
                
                free(ligne);
                free(champ1);
                free(champ2);
            }
            fclose(villes);
            // Fin Neige
            
            // Le zonage sismique.
            BUGMSG(villes = fopen(DATADIR"/france_seisme.csv", "r"), FALSE, gettext("Le fichier '%s' est introuvable.\n"), DATADIR"/france_seisme.csv");
            // On commande par chercher le département
            dep_parcours = 0;
            while (dep_parcours == 0)
            {
                BUGMSG(ligne = common_text_get_line(villes), FALSE, gettext("Le fichier '%s' est incomplet.\n"), DATADIR"/france_seisme.csv");
                
                BUGMSG(champ1 = malloc(sizeof(char)*(strlen(ligne)+1)), FALSE, gettext("Erreur d'allocation mémoire.\n"));
                BUGMSG(champ2 = malloc(sizeof(char)*(strlen(ligne)+1)), FALSE, gettext("Erreur d'allocation mémoire.\n"));
                
                BUGMSG(sscanf(ligne, "%s\t%d\t%s\n", champ1, &dep_parcours, champ2) == 3, FALSE, gettext("Le fichier '%s' est corrompu.\n"), DATADIR"/france_seisme.csv");
                
                if ((dep_parcours != dep) || (strcmp(champ1, "DEP") != 0))
                {
                    dep_parcours = 0;
                    free(champ2);
                }
                
                free(ligne);
                free(champ1);
            }
            if (strcmp(champ2, "1") == 0)
                projet->parametres.seisme = SEISME_1;
            else if (strcmp(champ2, "2") == 0)
                projet->parametres.seisme = SEISME_2;
            else if (strcmp(champ2, "3") == 0)
                projet->parametres.seisme = SEISME_3;
            else if (strcmp(champ2, "4") == 0)
                projet->parametres.seisme = SEISME_4;
            else if (strcmp(champ2, "5") == 0)
                projet->parametres.seisme = SEISME_5;
            else
                BUGMSG(NULL, FALSE, gettext("Le fichier '%s' est corrumpu. Le champ2 '%s' est inconnu.\n"), DATADIR"/france_seisme.csv", champ2);
            free(champ2);
            // On recherche si il y a une exception à la règle de base.
            while (dep_parcours == dep)
            {
                ligne = common_text_get_line(villes);
                // On a atteint la fin du fichier
                if (ligne == NULL)
                    break;
                
                BUGMSG(champ1 = malloc(sizeof(char)*(strlen(ligne)+1)), FALSE, gettext("Erreur d'allocation mémoire.\n"));
                BUGMSG(champ2 = malloc(sizeof(char)*(strlen(ligne)+1)), FALSE, gettext("Erreur d'allocation mémoire.\n"));
                
                BUGMSG(sscanf(ligne, "%s\t%d\t%s\n", champ1, &dep_parcours, champ2) == 3, FALSE, gettext("Le fichier '%s' est corrompu.\n"), DATADIR"/france_seisme.csv");
                
                if (strcmp(champ1, "DEP") == 0)
                    dep_parcours = 0;
                else if (((strcmp(champ1, "CAN") == 0) && (dep_parcours == ct)) ||
                        ((strcmp(champ1, "COM") == 0) && (dep_parcours == com)))
                {
                    if (strcmp(champ2, "1") == 0)
                        projet->parametres.seisme = SEISME_1;
                    else if (strcmp(champ2, "2") == 0)
                        projet->parametres.seisme = SEISME_2;
                    else if (strcmp(champ2, "3") == 0)
                        projet->parametres.seisme = SEISME_3;
                    else if (strcmp(champ2, "4") == 0)
                        projet->parametres.seisme = SEISME_4;
                    else if (strcmp(champ2, "5") == 0)
                        projet->parametres.seisme = SEISME_5;
                    else
                        BUGMSG(NULL, FALSE, gettext("Le fichier '%s' est corrumpu. Le champ2 '%s' est inconnu.\n"), DATADIR"/france_seisme.csv", champ2);
                    dep_parcours = 0;
                }
                else
                    dep_parcours = dep;
                
                free(ligne);
                free(champ1);
                free(champ2);
            }
            fclose(villes);
            // Fin Neige
            
            return TRUE;
        }
        free(ligne);
        ligne = common_text_get_line(villes);
    } while (ligne != NULL);
    
    fclose(villes);
    
    return FALSE;
}


gboolean common_ville_free(Projet *projet)
/* Description : Libère les allocations mémoires pour la gestion des villes.
 * Paramètres : Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL,
 */
{
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    
    free(projet->parametres.adresse.destinataire);
    free(projet->parametres.adresse.adresse);
    free(projet->parametres.adresse.ville);
    
    return TRUE;
}
