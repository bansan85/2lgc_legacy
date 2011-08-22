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
#include <stdlib.h>
#include <libintl.h>
#include <math.h>
#include "common_projet.h"
#include "common_maths.h"
#include "common_erreurs.h"
#include "1992_1_1_materiaux.h"

/* _1992_1_1_materiaux_init
 * Description : Initialise la liste des matériaux en béton
 * Paramètres : Projet *projet : la variable projet
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : valeur négative
 */
int _1992_1_1_materiaux_init(Projet *projet)
{
    if (projet == NULL)
        BUGMSG(0, -1, gettext("Paramètres invalides.\n"));
    
    projet->beton.materiaux = list_init();
    if (projet->beton.materiaux == NULL)
        BUGMSG(0, -2, gettext("Erreur d'allocation mémoire.\n"));
    else
        return 0;
}

/* _1992_1_1_materiaux_ajout
 * Description : Ajoute un matériau en béton
 * Paramètres : Projet *projet : la variable projet
 *            : double fck : résistance à la compression du béton à 28 jours
 *            : double nu : coefficient de poisson pour un béton non fissuré
 * Valeur renvoyée :
 *   Succès : 0 même si aucun matériau n'est existant
 *   Échec : valeur négative si la liste des matériaux en béton  n'est pas initialisée ou a déjà été libérée
 */
int _1992_1_1_materiaux_ajout(Projet *projet, double fck, double nu)
{
    Beton_Materiau  *materiau_en_cours, materiau_nouveau;
    
    if ((projet == NULL) || (projet->beton.materiaux == NULL) || (fck > 90.))
        BUGMSG(0, -1, gettext("Paramètres invalides.\n"));
    
    list_mvrear(projet->beton.materiaux);
    materiau_nouveau.fck = fck*1000000.;
    if (fck < 12.)
        materiau_nouveau.fckcube = fck*1.25*1000000.;
    else if (fck < 16.)
        materiau_nouveau.fckcube = 5.*fck/4.*1000000.;
    else if (fck < 20.)
        materiau_nouveau.fckcube = 5.*fck/4.*1000000.;
    else if (fck < 25.)
        materiau_nouveau.fckcube = (fck+5.)*1000000.;
    else if (fck < 30.)
        materiau_nouveau.fckcube = (7.*fck/5.-5.)*1000000.;
    else if (fck < 35.)
        materiau_nouveau.fckcube = (8.*fck/5.-11.)*1000000.;
    else if (fck < 40.)
        materiau_nouveau.fckcube = (fck+10.)*1000000.;
    else if (fck < 45.)
        materiau_nouveau.fckcube = (fck+10.)*1000000.;
    else if (fck < 50.)
        materiau_nouveau.fckcube = (fck+10.)*1000000.;
    else if (fck < 55.)
        materiau_nouveau.fckcube = (7.*fck/5.-10.)*1000000.;
    else if (fck < 60.)
        materiau_nouveau.fckcube = (8.*fck/5.-21.)*1000000.;
    else if (fck < 70.)
        materiau_nouveau.fckcube = (fck+15.)*1000000.;
    else if (fck < 80.)
        materiau_nouveau.fckcube = (fck+15.)*1000000.;
    else if (fck <= 90.)
        materiau_nouveau.fckcube = (fck+15.)*1000000.;
    materiau_nouveau.fcm = (fck+8.)*1000000.;
    if (fck <= 50.)
        materiau_nouveau.fctm = 0.3*pow(fck,2./3.)*1000000.;
    else
        materiau_nouveau.fctm = 2.12*log(1.+(materiau_nouveau.fcm/10./1000000.))*1000000.;
    materiau_nouveau.fctk_0_05 = 0.7*materiau_nouveau.fctm;
    materiau_nouveau.fctk_0_95 = 1.3*materiau_nouveau.fctm;
    materiau_nouveau.ecm = 22.*pow(materiau_nouveau.fcm/10./1000000., 0.3)*1000000000.;
    materiau_nouveau.ec1 = MIN(0.7*pow(materiau_nouveau.fcm/1000000., 0.31), 2.8)/1000.;
    if (fck < 50.)
        materiau_nouveau.ecu1 = 3.5/1000.;
    else
        materiau_nouveau.ecu1 = (2.8 + 27.*pow((98.-materiau_nouveau.fcm/1000000.)/100.,4.))/1000.;
    if (fck < 50.)
        materiau_nouveau.ec2 = 2./1000.;
    else
        materiau_nouveau.ec2 = (2. + 0.085*pow(fck-50., 0.53))/1000.;
    if (fck < 50.)
        materiau_nouveau.ecu2 = 3.5/1000.;
    else
        materiau_nouveau.ecu2 = (2.6 + 35.*pow((90.-fck)/100.,4.))/1000.;
    if (fck < 50.)
        materiau_nouveau.n = 2./1000.;
    else
        materiau_nouveau.n = (1.4 + 23.4*pow((90.-fck)/100., 4.))/1000.;
    if (fck < 50.)
        materiau_nouveau.ec3 = 1.75/1000.;
    else
        materiau_nouveau.ec3 = (1.75 + 0.55*(fck-50.)/40.)/1000.;
    if (fck < 50.)
        materiau_nouveau.ecu3 = 3.5/1000.;
    else
        materiau_nouveau.ecu3 = (2.6 + 35*pow((90.-fck)/100., 4.))/1000.;
    materiau_nouveau.nu = nu;
    materiau_nouveau.gnu_0_2 = materiau_nouveau.ecm/(2.*(1.+nu));
    materiau_nouveau.gnu_0_0 = materiau_nouveau.ecm/2.;
    
    materiau_en_cours = (Beton_Materiau*)list_rear(projet->beton.materiaux);
    if (materiau_en_cours == NULL)
        materiau_nouveau.numero = 0;
    else
        materiau_nouveau.numero = materiau_en_cours->numero+1;
    
    if (list_insert_after(projet->beton.materiaux, &(materiau_nouveau), sizeof(materiau_nouveau)) == NULL)
        BUGMSG(0, -6, gettext("Erreur d'allocation mémoire.\n"));
    
    return 0;
}


/* _1992_1_1_materiaux_cherche_numero
 * Description : Positionne dans la liste des materiaux en béton l'élément courant au numéro souhaité
 * Paramètres : Projet *projet : la variable projet
 *            : int numero : le numéro du matériau
 * Valeur renvoyée :
 *   Succès : pointeur vers le matériau en béton
 *   Échec : NULL
 */
Beton_Materiau* _1992_1_1_materiaux_cherche_numero(Projet *projet, unsigned int numero)
{
    if ((projet == NULL) || (projet->beton.materiaux == NULL) || (list_size(projet->beton.materiaux) == 0))
        BUGMSG(0, NULL, gettext("Paramètres invalides.\n"));
    
    list_mvfront(projet->beton.materiaux);
    do
    {
        Beton_Materiau  *materiau = list_curr(projet->beton.materiaux);
        
        if (materiau->numero == numero)
            return materiau;
    }
    while (list_mvnext(projet->beton.materiaux) != NULL);
    
    BUGMSG(0, NULL, gettext("Materiau en béton n°%d introuvable.\n"), numero);
}


/* _1992_1_1_materiaux_free
 * Description : Libère l'ensemble des matériaux en béton
 * Paramètres : Projet *projet : la variable projet
 * Valeur renvoyée :
 *   Succès : 0 même si aucun matériau n'est existant
 *   Échec : valeur négative si la liste des matériaux en béton n'est pas initialisée ou a déjà été libérée
 */
int _1992_1_1_materiaux_free(Projet *projet)
{
    if ((projet == NULL) || (projet->beton.materiaux == NULL))
        BUGMSG(0, -1, gettext("Paramètres invalides.\n"));
    
    while (!list_empty(projet->beton.materiaux))
    {
        Beton_Materiau *materiau = list_remove_front(projet->beton.materiaux);
        
        free(materiau);
    }
    
    free(projet->beton.materiaux);
    projet->beton.materiaux = NULL;
    
    return 0;
}
