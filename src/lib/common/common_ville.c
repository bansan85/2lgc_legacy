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


gboolean common_ville_get_ville(char *ligne, int *cdc, int *cheflieu, int *reg, char *dep,
  int *com, int *ar, int *ct, int *tncc, char **artmaj, char **ncc, char **artmin,
  char **nccenr, int *code_postal, int *altitude, int *population)
/* Description : Renvoie sous forme de variables la ligne de ville en cours d'analyse.
 * Paramètres : char *ligne : ligne en cours d'analyse,
 *              int *cdc : découpage de la commune en cantons : 1 si oui, 0 si non,
 *              int *cheflieu : la ville est chef-lieu (d'une région : 1, d'un département : 2,
 *                              d'un arrondissement : 3 ou d'un canton : 4),
 *              int *reg : la région,
 *              char *dep : le département,
 *              int *com : la commune,
 *              int *ar : l'arrondissement,
 *              int *ct : le canton,
 *              int *tncc : Type de nom en clair ; cette variable permet d'écrire le nom complet
 *                          dans le cas d'un libellé avec article, et d'adapter la charnière si
 *                          le nom est utilisé dans une expression comme "la commune de
 *                          Marseille", "l'arrondissement du Mans", etc.
                            Pour les noms de communes (et donc de canton et d'arrondissement),
                            l'article est obligatoire ("Rochelle" n'existe pas sans article),
                            alors que ce n'est pas le cas pour les noms de département ou de
                            région ("Charente-Maritime" peut être écrit sans article). Pour les
                            départements et les régions, ce code ne sert donc que pour la
                            charnière.
            0   pas d'article et le nom commence par une consonne sauf H muet.  charnière = DE
            1   pas d'article et le nom commence par une voyelle ou un H muet.  charnière = D'
            2   article = LE    charnière = DU
            3   article = LA    charnière = DE LA
            4   article = LES   charnière = DES
            5   article = L'    charnière = DE L'
            6   article = AUX   charnière = DES
            7   article = LAS   charnière = DE LAS
            8   article = LOS   charnière = DE LOS
            source : insee.fr/fr/methodes/default.asp?page=nomenclatures/cog/doc_variables.htm
 *              char **artmaj : l'article en majuscule,
 *              char **ncc : le nom de la ville en majuscule,
 *              char **artmin : l'article en miniscule,
 *              char **nccenr : la ville en majuscule,
 *              int *code_postal : le code postal,
 *              int *altitude : l'altitude,
 *              int *population : la population.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             en cas d'erreur d'allocation mémoire.
 */
{
    int     i, j;
    int     cdc_, cheflieu_, reg_, com_, ar_, ct_, tncc_;
    int     code_postal_, altitude_, population_;
    char    *dep_;
    
    BUGMSG(ligne, FALSE, gettext("Paramètre %s incorrect.\n"), "ligne");
    
    BUGMSG(dep_ = malloc(sizeof(char)*(strlen(ligne)+1)), FALSE, gettext("Erreur d'allocation mémoire.\n"));
    
    // On récupère les numéros caractéristant la ville en cours.
    BUGMSG(sscanf(ligne, "%d\t%d\t%d\t%s\t%d\t%d\t%d\t%d\t", &cdc_, &cheflieu_, &reg_, dep_, &com_, &ar_, &ct_, &tncc_) == 8, FALSE, gettext("La ligne en cours '%s' n'est pas dans un format correct pour une ville.\n"), ligne);
    BUGMSG((0 < strlen(dep_)) && (strlen(dep_) <= 3), FALSE, gettext("La ligne en cours '%s' n'est pas dans un format correct pour une ville.\n"), ligne);
    if (cdc != NULL)
        *cdc = cdc_;
    if (cheflieu != NULL)
        *cheflieu = cheflieu_;
    if (reg != NULL)
        *reg = reg_;
    if (dep != NULL)
        strcpy(dep, dep_);
    free(dep_);
    if (com != NULL)
        *com = com_;
    if (ar != NULL)
        *ar = ar_;
    if (ct != NULL)
        *ct = ct_;
    if (tncc != NULL)
        *tncc = tncc_;
    
    i = 0;
    j = 0;
    // On est obligé de traiter à la main car sscanf("1\t\t3", "%s\t%s\t%s", un, deux,
    // trois); renvoie : un = 1, deux = 3 et trois non initialisé.
    while ((i != 12) && (ligne[j] != '\000'))
    {
        if (ligne[j] == '\t')
        {
            ligne[j] = '\000';
            i++;
        }
        j++;
        if (i == 7)
        {
            if (artmaj != NULL)
                *artmaj = &(ligne[j+1]);
        }
        else if (i == 8)
        {
            if (ncc != NULL)
                *ncc = &(ligne[j+1]);
        }
        else if (i == 9)
        {
            if (artmin != NULL)
                *artmin = &(ligne[j+1]);
        }
        else if (i == 10)
        {
            if (nccenr != NULL)
                *nccenr = &(ligne[j+1]);
        }
    }
    
    BUGMSG(ligne[j] != '\000', FALSE, gettext("La ligne en cours '%s' n'est pas dans un format correct pour une ville.\n"), ligne);
    BUGMSG(sscanf(&(ligne[j]), "%d\t%d\t%d\n", &code_postal_, &altitude_, &population_) == 3, FALSE, gettext("La ligne en cours '%s' n'est pas dans un format correct pour une ville.\n"), ligne);
    
    if (code_postal != NULL)
        *code_postal = code_postal_;
    if (altitude != NULL)
        *altitude = altitude_;
    if (population != NULL)
        *population = population_;
    
    return TRUE;
}


gboolean common_ville_set(Projet *projet, char *departement, int ville)
/* Description : Initialise la ville (mais pas l'adresse exacte) du projet avec les paramètres
 *               régionaux (vent, neige, séisme).
 *               Departement DEP et ville COM doivent correspondre au fichier france_villes.csv
 * Paramètres : Projet *projet : la variable projet,
 *              char departement : le numéro du département (char* car 2A et 2B),
 *              int ville : le numéro de la ville. Nécessite d'avoir parcouru france_villes.csv.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             en cas d'erreur d'allocation mémoire.
 */
{
    FILE    *villes;
    char    *ligne = NULL;
    int     com, ct, code_postal, population;
    char    *artmin, *nccenr;
    char    dep[4];
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    
    BUGMSG(villes = fopen(DATADIR"/france_villes.csv", "r"), FALSE, gettext("Le fichier '%s' est introuvable.\n"), DATADIR"/france_villes.csv");
    
    // On passe la première ligne qui est l'étiquette des colonnes.
    ligne = common_text_get_line(villes);
    free(ligne);
    
    ligne = common_text_get_line(villes);
    do
    {
        BUG(common_ville_get_ville(ligne, NULL, NULL, NULL, dep, &com, NULL, &ct, NULL, NULL, NULL, &artmin, &nccenr, &code_postal, NULL, &population), FALSE);
        // On récupère les numéros caractéristant la ville en cours.
        if ((strcmp(dep, departement) == 0) && (ville == com))
        {
            char        *dep_parcours;
            char        *champ1, *champ2;
            gboolean    done;
            
            // Maintenant que tout est récupéré, on enregistre dans le projet ce qu'il faut.
            // Ville et code postal.
            projet->parametres.adresse.code_postal = code_postal;
            BUGMSG(projet->parametres.adresse.ville = g_strdup_printf("%s %s", artmin, nccenr), FALSE, gettext("Erreur d'allocation mémoire.\n"));
            projet->parametres.adresse.code_postal = code_postal;
            
            free(ligne);
            
            // Le zonage neige.
            BUGMSG(villes = fopen(DATADIR"/france_neige.csv", "r"), FALSE, gettext("Le fichier '%s' est introuvable.\n"), DATADIR"/france_neige.csv");
            // On commande par chercher le département
            dep_parcours = NULL;
            while (dep_parcours == NULL)
            {
                BUGMSG(ligne = common_text_get_line(villes), FALSE, gettext("Le fichier '%s' est incomplet.\n"), DATADIR"/france_neige.csv");
                
                BUGMSG(champ1 = malloc(sizeof(char)*(strlen(ligne)+1)), FALSE, gettext("Erreur d'allocation mémoire.\n"));
                BUGMSG(champ2 = malloc(sizeof(char)*(strlen(ligne)+1)), FALSE, gettext("Erreur d'allocation mémoire.\n"));
                BUGMSG(dep_parcours = malloc(sizeof(char)*(strlen(ligne)+1)), FALSE, gettext("Erreur d'allocation mémoire.\n"));
                
                BUGMSG(sscanf(ligne, "%s\t%s\t%s\n", champ1, dep_parcours, champ2) == 3, FALSE, gettext("Le fichier '%s' est corrompu.\n"), DATADIR"/france_neige.csv");
                
                if ((strcmp(dep_parcours, dep) != 0) || (strcmp(champ1, "DEP") != 0))
                {
                    free(dep_parcours);
                    dep_parcours = NULL;
                    free(champ2);
                }
                else
                    free(dep_parcours);
                
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
            done = FALSE;
            while (done == FALSE)
            {
                ligne = common_text_get_line(villes);
                // On a atteint la fin du fichier
                if (ligne == NULL)
                    break;
                
                BUGMSG(champ1 = malloc(sizeof(char)*(strlen(ligne)+1)), FALSE, gettext("Erreur d'allocation mémoire.\n"));
                BUGMSG(champ2 = malloc(sizeof(char)*(strlen(ligne)+1)), FALSE, gettext("Erreur d'allocation mémoire.\n"));
                
                BUGMSG(sscanf(ligne, "%s", champ1) == 1, FALSE, gettext("Le fichier '%s' est corrompu.\n"), DATADIR"/france_neige.csv");
                
                if (strcmp(champ1, "DEP") == 0)
                    done = TRUE;
                else
                {
                    int numero;
                    
                    BUGMSG(sscanf(ligne, "%s\t%d\t%s\n", champ1, &numero, champ2) == 3, FALSE, gettext("Le fichier '%s' est corrompu.\n"), DATADIR"/france_neige.csv");
                    if (((strcmp(champ1, "CAN") == 0) && (numero == ct)) ||
                            ((strcmp(champ1, "COM") == 0) && (numero == com)))
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
                        done = TRUE;
                    }
                    else
                        done = FALSE;
                }
                
                free(ligne);
                free(champ1);
                free(champ2);
            }
            fclose(villes);
            // Fin Neige
            
            // Le zonage vent.
            BUGMSG(villes = fopen(DATADIR"/france_vent.csv", "r"), FALSE, gettext("Le fichier '%s' est introuvable.\n"), DATADIR"/france_vent.csv");
            // On commande par chercher le département
            dep_parcours = NULL;
            while (dep_parcours == NULL)
            {
                BUGMSG(ligne = common_text_get_line(villes), FALSE, gettext("Le fichier '%s' est incomplet.\n"), DATADIR"/france_vent.csv");
                
                BUGMSG(champ1 = malloc(sizeof(char)*(strlen(ligne)+1)), FALSE, gettext("Erreur d'allocation mémoire.\n"));
                BUGMSG(champ2 = malloc(sizeof(char)*(strlen(ligne)+1)), FALSE, gettext("Erreur d'allocation mémoire.\n"));
                BUGMSG(dep_parcours = malloc(sizeof(char)*(strlen(ligne)+1)), FALSE, gettext("Erreur d'allocation mémoire.\n"));
                
                BUGMSG(sscanf(ligne, "%s\t%s\t%s\n", champ1, dep_parcours, champ2) == 3, FALSE, gettext("Le fichier '%s' est corrompu.\n"), DATADIR"/france_vent.csv");
                
                if ((strcmp(dep_parcours, dep) != 0) || (strcmp(champ1, "DEP") != 0))
                {
                    free(dep_parcours);
                    dep_parcours = NULL;
                    free(champ2);
                }
                else
                    free(dep_parcours);
                
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
            done = FALSE;
            while (done == FALSE)
            {
                ligne = common_text_get_line(villes);
                // On a atteint la fin du fichier
                if (ligne == NULL)
                    break;
                
                BUGMSG(champ1 = malloc(sizeof(char)*(strlen(ligne)+1)), FALSE, gettext("Erreur d'allocation mémoire.\n"));
                BUGMSG(champ2 = malloc(sizeof(char)*(strlen(ligne)+1)), FALSE, gettext("Erreur d'allocation mémoire.\n"));
                
                BUGMSG(sscanf(ligne, "%s", champ1) == 1, FALSE, gettext("Le fichier '%s' est corrompu.\n"), DATADIR"/france_vent.csv");
                
                if (strcmp(champ1, "DEP") == 0)
                    done = TRUE;
                else
                {
                    int numero;
                    
                    BUGMSG(sscanf(ligne, "%s\t%d\t%s\n", champ1, &numero, champ2) == 3, FALSE, gettext("Le fichier '%s' est corrompu.\n"), DATADIR"/france_vent.csv");
                    if (((strcmp(champ1, "CAN") == 0) && (numero == ct)) ||
                        ((strcmp(champ1, "COM") == 0) && (numero == com)))
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
                        done = TRUE;
                    }
                    else
                        done = FALSE;
                }
                
                free(ligne);
                free(champ1);
                free(champ2);
            }
            fclose(villes);
            // Fin Neige
            
            // Le zonage sismique.
            BUGMSG(villes = fopen(DATADIR"/france_seisme.csv", "r"), FALSE, gettext("Le fichier '%s' est introuvable.\n"), DATADIR"/france_seisme.csv");
            // On commande par chercher le département
            dep_parcours = NULL;
            while (dep_parcours == NULL)
            {
                BUGMSG(ligne = common_text_get_line(villes), FALSE, gettext("Le fichier '%s' est incomplet.\n"), DATADIR"/france_seisme.csv");
                
                BUGMSG(champ1 = malloc(sizeof(char)*(strlen(ligne)+1)), FALSE, gettext("Erreur d'allocation mémoire.\n"));
                BUGMSG(champ2 = malloc(sizeof(char)*(strlen(ligne)+1)), FALSE, gettext("Erreur d'allocation mémoire.\n"));
                BUGMSG(dep_parcours = malloc(sizeof(char)*(strlen(ligne)+1)), FALSE, gettext("Erreur d'allocation mémoire.\n"));
                
                BUGMSG(sscanf(ligne, "%s\t%s\t%s\n", champ1, dep_parcours, champ2) == 3, FALSE, gettext("Le fichier '%s' est corrompu.\n"), DATADIR"/france_seisme.csv");
                
                if ((strcmp(dep_parcours, dep) != 0) || (strcmp(champ1, "DEP") != 0))
                {
                    free(dep_parcours);
                    dep_parcours = NULL;
                    free(champ2);
                }
                else
                    free(dep_parcours);
                
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
            done = FALSE;
            while (done == FALSE)
            {
                ligne = common_text_get_line(villes);
                // On a atteint la fin du fichier
                if (ligne == NULL)
                    break;
                
                BUGMSG(champ1 = malloc(sizeof(char)*(strlen(ligne)+1)), FALSE, gettext("Erreur d'allocation mémoire.\n"));
                BUGMSG(champ2 = malloc(sizeof(char)*(strlen(ligne)+1)), FALSE, gettext("Erreur d'allocation mémoire.\n"));
                
                BUGMSG(sscanf(ligne, "%s", champ1) == 1, FALSE, gettext("Le fichier '%s' est corrompu.\n"), DATADIR"/france_seisme.csv");
                
                if (strcmp(champ1, "DEP") == 0)
                    done = TRUE;
                else
                {
                    int numero;
                    
                    BUGMSG(sscanf(ligne, "%s\t%d\t%s\n", champ1, &numero, champ2) == 3, FALSE, gettext("Le fichier '%s' est corrompu.\n"), DATADIR"/france_seisme.csv");
                    if (((strcmp(champ1, "CAN") == 0) && (numero == ct)) ||
                        ((strcmp(champ1, "COM") == 0) && (numero == com)))
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
                        done = TRUE;
                    }
                    else
                        done = FALSE;
                }
                
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
