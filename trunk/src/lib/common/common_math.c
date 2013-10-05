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
#include <math.h>
#include <gmodule.h>
#include "common_math.h"
#include "common_projet.h"
#include "common_erreurs.h"


double common_math_arrondi_nombre(double nombre)
/* Description : Arrondi un nombre en supprimant la partie négligeable (ERREUR_RELATIVE_MIN)
 *   L'algorithme est perfectible puisque lors de l'arrondi, une nouvelle imprécision apparait
 *   et certains nombres peuvent être arrondi en 1.09999999 ou -23.000000001.
 * Paramètres : double nombre : le nombre à arrondir.
 * Valeur renvoyée : le nombre arrondi.
 */
{
    double          puissance;
    
    // Si le nombre est égal parfaitement à 0. Alors
    //     Renvoie 0.
    // FinSi
    // Multiplication du nombre par 10^(ERREUR_RELATIVE_PUISSANCE-ceil(log10(ABS(nombre))))
    //   afin d'avoir une partie entière composée de ERREUR_RELATIVE_PUISSANCE chiffres.
    // Conservation de la partie entière uniquement du nombre.
    // Division par 10^(ERREUR_RELATIVE_PUISSANCE-ceil(log10(ABS(nombre)))).
    // Renvoie du nombre.
    if (ERREUR_RELATIVE_EGALE(nombre, 0.))
        return 0.;
    puissance = ERREUR_RELATIVE_PUISSANCE-ceil(log10(ABS(nombre)));
    nombre = nombre*pow(10, puissance);
    modf(nombre, &nombre);
    return nombre/pow(10., puissance);
}


void common_math_arrondi_triplet(cholmod_triplet *triplet)
/* Description : Arrondi un triplet en supprimant la partie négligeable (ERREUR_RELATIVE_MIN).
 * Paramètres : cholmod_triplet *triplet : la variable triplet à arrondir.
 * Valeur renvoyée : Aucune.
 */
{
    double          *ax;
    unsigned int    i;
    
    // Trivial
    ax = (double*)triplet->x;
    for (i=0;i<triplet->nnz;i++)
        ax[i] = common_math_arrondi_nombre(ax[i]);
    
    return;
}


void common_math_arrondi_sparse(cholmod_sparse *sparse)
/* Description : Arrondi un sparse en supprimant la partie négligeable (ERREUR_RELATIVE_MIN).
 * Paramètres : cholmod_sparse *sparse : la matrice sparse à arrondir.
 * Valeur renvoyée : Aucune.
 */
{
    double          *ax;
    unsigned int    i;
    
    // Trivial
    ax = (double*)sparse->x;
    for (i=0;i<sparse->nzmax;i++)
        ax[i] = common_math_arrondi_nombre(ax[i]);
    
    return;
}


void common_math_double_to_char(double nombre, char *dest, int decimales)
/* Description : Converti un nombre double en char *.
                 Dest doit déjà être alloué. 30 caractères devrait être suffisant.
 * Paramètres : double nombre : nombre à convertir,
 *              char *dest : nombre converti,
 *              int decimales : nombre de décimales au maximum.
 * Valeur renvoyée : Aucune.
 */
{
/*    double  test;
    int     width;*/
    
    if (decimales > 15)
        decimales = 15;
    
/*    // Si le nombre est supérieur à 1e15, on affiche sous forme scientifique
    if (ABS(nombre) > 1e15)
    {
        for (width = 0; width<=decimales; width++)
        {
            sprintf(dest, "%.*le", width, nombre);
            sscanf(dest, "%le", &test);
            if ((fabs(nombre)*0.999999999999999 <= fabs(test)) && (fabs(test) <= fabs(nombre)*1.000000000000001))
                break;
        }
    }
    // Sinon on affiche sous forme normale
    else
    {
        for (width = 0; width<=decimales; width++)
        {
            sprintf(dest, "%.*lf", width, nombre);
            sscanf(dest, "%lf", &test);
            if ((fabs(nombre)*0.999999999999999 <= fabs(test)) && (fabs(test) <= fabs(nombre)*1.000000000000001))
                break;
        }
    }*/
    sprintf(dest, "%.*lf", decimales, nombre);
    
    return;
}


void common_math_double_to_char2(Flottant nombre, char *dest, int decimales)
/* Description : Converti un nombre double en char *.
                 Dest doit déjà être alloué. 30 caractères devrait être suffisant.
 * Paramètres : double nombre : nombre à convertir,
 *              char *dest : nombre converti,
 *              int decimales : nombre de décimales au maximum.
 * Valeur renvoyée : Aucune.
 */
{
    double  test, f;
    int     width;
    
    if (decimales > 15)
        decimales = 15;
    f = common_math_get(nombre);
    
    // Si le nombre est supérieur à 1e15, on affiche sous forme scientifique
    switch (nombre.type)
    {
        case FLOTTANT_ORDINATEUR :
        {
            sprintf(dest, "%.*lf", decimales, f);
            break;
        }
        // Dans le cas d'un flottant utilisateur, on affiche toutes les décimales afin
        // d'afficher le nombre exact qu'il a saisi afin d'éviter qu'il pense que des
        // informations ont été tronquées.
        case FLOTTANT_UTILISATEUR :
        {
            if (ABS(f) > 1e15)
            {
                for (width = 0; width<=15; width++)
                {
                    sprintf(dest, "%.*le", width, f);
                    sscanf(dest, "%le", &test);
                    if ((fabs(f)*0.999999999999999 <= fabs(test)) && (fabs(test) <= fabs(f)*1.000000000000001))
                        break;
                }
            }
            // Sinon on affiche sous forme normale
            else
            {
                for (width = 0; width<=15; width++)
                {
                    sprintf(dest, "%.*lf", width, f);
                    sscanf(dest, "%lf", &test);
                    if ((fabs(f)*0.999999999999999 <= fabs(test)) && (fabs(test) <= fabs(f)*1.000000000000001))
                        break;
                }
            }
            sprintf(dest, "%.*lf", MAX(width, decimales), f);
            break;
        }
        default :
        {
            BUGMSG(NULL, , gettext("Type de nombre est inconnu.\n"))
            break;
        }
    }
    
    return;
}


double common_math_get(Flottant f)
/* Description : Renvoie la valeur d'un flottant.
 * Paramètres : Flottant *f : le nombre.
 * Valeur renvoyée :
 *   Succès : le nombre.
 *   Échec : NAN si
 *             le type de f est inconnu.
 */
{
    switch (f.type)
    {
        case FLOTTANT_ORDINATEUR :
        case FLOTTANT_UTILISATEUR :
        {
            return f.d;
            break;
        }
        default :
        {
            free(GUINT_TO_POINTER(1234156789));
            BUGMSG(NULL, NAN, gettext("Type de nombre est inconnu.\n"))
            break;
        }
    }
}


Flottant common_math_f(double f, Type_Flottant type)
/* Description : Converti un double en Flottant.
 *               Ne marche que pour les types FLOTTANT_ORDINATEUR et FLOTTANT_UTILISATEUR.
 * Paramètres : Flottant f1 : nombre 1,
 *              Flottant f2 : nombre 2.
 * Valeur renvoyée : Le résultat.
 */
{
    Flottant    retour;
    
    retour.type = type;
    switch (retour.type)
    {
        case FLOTTANT_ORDINATEUR :
        case FLOTTANT_UTILISATEUR :
        {
            retour.d = f;
            return retour;
            break;
        }
        default :
        {
            retour.d = NAN;
            BUGMSG(NULL, retour, gettext("Type de nombre est inconnu.\n"))
            break;
        }
    }
}


Flottant common_math_add(Flottant f1, Flottant f2)
/* Description : Additionne deux flottants.
 * Paramètres : Flottant f1 : nombre 1,
 *              Flottant f2 : nombre 2.
 * Valeur renvoyée : Le résultat.
 */
{
    Flottant   retour;
    
    retour.type = FLOTTANT_ORDINATEUR;
    retour.d = common_math_get(f1) + common_math_get(f2);
    
    return retour;
}


Flottant common_math_sub(Flottant f1, Flottant f2)
/* Description : Soustrait deux flottants.
 * Paramètres : Flottant f1 : nombre 1,
 *              Flottant f2 : nombre 2.
 * Valeur renvoyée : Le résultat,
 */
{
    Flottant   retour;
    
    retour.type = FLOTTANT_ORDINATEUR;
    retour.d = common_math_get(f1) - common_math_get(f2);
    
    return retour;
}


Flottant common_math_div_d(Flottant f, double d)
/* Description : Divise un Flottant par un double.
 * Paramètres : Flottant f : nombre 1,
 *              double d : nombre 2.
 * Valeur renvoyée : Le résultat.
 */
{
    Flottant   retour;
    
    retour.type = FLOTTANT_ORDINATEUR;
    retour.d = common_math_get(f) / d;
    
    return retour;
}


Flottant common_math_dot_f(Flottant f1, Flottant f2)
/* Description : Multiplie deux flottants.
 * Paramètres : Flottant f1 : nombre 1,
 *              Flottant f2 : nombre 2.
 * Valeur renvoyée : Le résultat,
 */
{
    Flottant   retour;
    
    retour.type = FLOTTANT_ORDINATEUR;
    retour.d = common_math_get(f1) * common_math_get(f2);
    
    return retour;
}


Flottant common_math_dot_d(Flottant f, double d)
/* Description : Multiplie un Flottant avec un double.
 * Paramètres : Flottant f : nombre 1,
 *              double d : nombre 2.
 * Valeur renvoyée : Le résultat.
 */
{
    Flottant   retour;
    
    retour.type = FLOTTANT_ORDINATEUR;
    retour.d = common_math_get(f) * d;
    
    return retour;
}
