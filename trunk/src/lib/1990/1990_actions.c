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
#include <stdio.h>
#include <stdlib.h>
#include <cholmod.h>
#include <string.h>

#include "1990_coef_psi.h"
#include "common_projet.h"
#include "common_maths.h"
#include "common_erreurs.h"
#include "common_fonction.h"
#include "1990_gtk_actions.h"
#include "EF_charge_noeud.h"
#include "EF_charge_barre_ponctuelle.h"
#include "EF_charge_barre_repartie_uniforme.h"


char* _1990_action_type_bat_txt_eu(unsigned int type)
/* Description : renvoie la description des types de charge pour les bâtiments de la norme
 *               européenne.
 *               FONCTION INTERNE. Utiliser _1990_action_type_bat_txt.
 * Paramètres : unsigned int type : type de charge.
 * Valeur renvoyée :
 *   Succès : le texte correspondant :
 *            0 : Permanente
 *            1 : Précontrainte
 *            2 : Exploitation : Catégorie A : habitation, zones résidentielles
 *            3 : Exploitation : Catégorie B : bureaux
 *            4 : Exploitation : Catégorie C : lieux de réunion
 *            5 : Exploitation : Catégorie D : commerces
 *            6 : Exploitation : Catégorie E : stockage
 *            7 : Exploitation : Catégorie F : zone de trafic, véhicules inférieur à 30 kN
 *            8 : Exploitation : Catégorie G : zone de trafic, véhicules entre 30 kN et 160 kN
 *            9 : Exploitation : Catégorie H : toits
 *            10: Neige : Finlande, Islande, Norvège, Suède
 *            11: Neige : Autres états membres CEN, altitude > 1000 m
 *            12: Neige : Autres états membres CEN, altitude <= 1000 m
 *            13: Vent
 *            14: Température (hors incendie)
 *            15: Accidentelle
 *            16: Sismique
 *   Échec : NULL :
 *             la catégorie n'existe pas.
 */
{
    // Trivial
    switch(type)
    {
        case 0 : { return gettext("Permanente"); break; }
        case 1 : { return gettext("Précontrainte"); break; }
        case 2 : { return gettext("Exploitation : Catégorie A : habitation, zones résidentielles"); break; }
        case 3 : { return gettext("Exploitation : Catégorie B : bureaux"); break; }
        case 4 : { return gettext("Exploitation : Catégorie C : lieux de réunion"); break; }
        case 5 : { return gettext("Exploitation : Catégorie D : commerces"); break; }
        case 6 : { return gettext("Exploitation : Catégorie E : stockage"); break; }
        case 7 : { return gettext("Exploitation : Catégorie F : zone de trafic, véhicules de poids inférieur à 30 kN"); break; }
        case 8 : { return gettext("Exploitation : Catégorie G : zone de trafic, véhicules de poids entre 30 kN et 160 kN"); break; }
        case 9 : { return gettext("Exploitation : Catégorie H : toits"); break; }
        case 10 : { return gettext("Neige : Finlande, Islande, Norvège, Suède"); break; }
        case 11 : { return gettext("Neige : Autres états membres CEN, altitude > 1000 m"); break; }
        case 12 : { return gettext("Neige : Autres états membres CEN, altitude <= 1000 m"); break; }
        case 13 : { return gettext("Vent"); break; }
        case 14 : { return gettext("Température (hors incendie)"); break; }
        case 15 : { return gettext("Accidentelle"); break; }
        case 16 : { return gettext("Sismique"); break; }
        default : { BUGMSG(0, NULL, gettext("Type d'action %u inconnu.\n"), type); break; }
    }
}


char* _1990_action_type_bat_txt_fr(unsigned int type)
/* Description : renvoie la description des types de charge pour les bâtiments de la norme
 *               française.
 *               FONCTION INTERNE. Utiliser _1990_action_type_bat_txt.
 * Paramètres : unsigned int type : type de charge.
 * Valeur renvoyée :
 *   Succès : le texte correspondant :
 *            0 : Permanente
 *            1 : Précontrainte
 *            2 : Exploitation : Catégorie A : habitation, zones résidentielles
 *            3 : Exploitation : Catégorie B : bureaux
 *            4 : Exploitation : Catégorie C : lieux de réunion
 *            5 : Exploitation : Catégorie D : commerces
 *            6 : Exploitation : Catégorie E : stockage
 *            7 : Exploitation : Catégorie F : zone de trafic, véhicules inférieur à 30 kN
 *            8 : Exploitation : Catégorie G : zone de trafic, véhicules entre 30 kN et 160 kN
 *            9 : Exploitation : Catégorie H : toits d'un bâtiment de catégorie A ou B
 *            10: Exploitation : Catégorie I : toitures accessibles avec locaux de type A ou B
 *            11: Exploitation : Catégorie I : toitures accessibles avec locaux de type C ou D
 *            12: Exploitation : Catégorie K : Hélicoptère sur la toiture
 *            13: Exploitation : Catégorie K : Hélicoptère sur la toiture, autres charges
 *            14: Neige : Saint-Pierre-et-Miquelon
 *            15: Neige : Altitude > 1000 m
 *            16: Neige : Altitude <= 1000 m
 *            17: Vent
 *            18: Température (hors incendie)
 *            19: Accidentelle
 *            20: Sismique
 *            21: Eaux souterraines
 *   Échec : NULL :
 *             la catégorie n'existe pas.
 */
{
    // Trivial
    switch(type)
    {
        case 0 : { return gettext("Permanente"); break; }
        case 1 : { return gettext("Précontrainte"); break; }
        case 2 : { return gettext("Exploitation : Catégorie A : habitation, zones résidentielles"); break; }
        case 3 : { return gettext("Exploitation : Catégorie B : bureaux"); break; }
        case 4 : { return gettext("Exploitation : Catégorie C : lieux de réunion"); break; }
        case 5 : { return gettext("Exploitation : Catégorie D : commerces"); break; }
        case 6 : { return gettext("Exploitation : Catégorie E : stockage"); break; }
        case 7 : { return gettext("Exploitation : Catégorie F : zone de trafic, véhicules de poids inférieur à 30 kN"); break; }
        case 8 : { return gettext("Exploitation : Catégorie G : zone de trafic, véhicules de poids entre 30 kN et 160 kN"); break; }
        case 9 : { return gettext("Exploitation : Catégorie H : toits d'un bâtiment de catégorie A ou B"); break; }
        case 10 : { return gettext("Exploitation : Catégorie I : toitures accessibles avec locaux des catégories A ou B"); break; }
        case 11 : { return gettext("Exploitation : Catégorie I : toitures accessibles avec locaux des catégories C ou D"); break; }
        case 12 : { return gettext("Exploitation : Catégorie K : Hélicoptère sur la toiture"); break; }
        case 13 : { return gettext("Exploitation : Catégorie K : Hélicoptère sur la toiture, autres charges (fret, personnel, accessoires ou équipements divers)"); break; }
        case 14 : { return gettext("Neige : Saint-Pierre-et-Miquelon"); break; }
        case 15 : { return gettext("Neige : Altitude > 1000 m"); break; }
        case 16 : { return gettext("Neige : Altitude <= 1000 m"); break; }
        case 17 : { return gettext("Vent"); break; }
        case 18 : { return gettext("Température (hors incendie)"); break; }
        case 19 : { return gettext("Accidentelle"); break; }
        case 20 : { return gettext("Sismique"); break; }
        case 21 : { return gettext("Eaux souterraines"); break; }
        default : { BUGMSG(0, NULL, gettext("Type d'action %u inconnu.\n"), type); break; }
    }
}


G_MODULE_EXPORT char* _1990_action_type_bat_txt(unsigned int type, Type_Pays pays)
/* Description : renvoie la description des types de charge pour les bâtiments en fonction de
 *               la norme demandée.
 * Paramètres : unsigned int type : type de charge,
 *            : Type_Pays pays : le numéro du pays.
 * Valeur renvoyée :
 *   Succès : cf. _1990_action_type_bat_txt_PAYS
 *   Échec : NULL :
 *             le type n'existe pas,
 *             le pays n'existe pas.
 */
{
    // Trivial
    switch (pays)
    {
        case PAYS_EU : { return _1990_action_type_bat_txt_eu(type); break; }
        case PAYS_FR : { return _1990_action_type_bat_txt_fr(type); break; }
        default : { BUGMSG(0, NULL, gettext("Pays %d inconnu.\n"), pays); break; }
    }
}


Action_Categorie _1990_action_categorie_bat_eu(unsigned int type)
/* Description : renvoie la categorie d'une action pour les combinaisons d'action des bâtiments
 *               selon la norme européenne.
 *               FONCTION INTERNE. Utiliser _1990_action_categorie_bat.
 * Paramètres : unsigned int type : type de l'action.
 * Valeur renvoyée :
 *   Succès : ACTION_POIDS_PROPRE : Poids propre
 *            ACTION_PRECONTRAINTE : Précontrainte
 *            ACTION_VARIABLE : Action variable
 *            ACTION_ACCIDENTELLE : Action accidentelle
 *            ACTION_SISMIQUE : Action sismique
 *   Échec : ACTION_INCONNUE :
 *             La catégorie n'existe pas.
 */
{
    // Trivial
    if (type == 0)
        return ACTION_POIDS_PROPRE;
    else if (type == 1)
        return ACTION_PRECONTRAINTE;
    else if ((2 <= type) && (type <= 14))
        return ACTION_VARIABLE;
    else if (type == 15)
        return ACTION_ACCIDENTELLE;
    else if (type == 16)
        return ACTION_SISMIQUE;
    else
        BUGMSG(0, ACTION_INCONNUE, gettext("Type d'action %u inconnu.\n"), type);
}


Action_Categorie _1990_action_categorie_bat_fr(unsigned int type)
/* Description : renvoie la catégorie d'une action pour les combinaisons d'action des bâtiments
 *               selon la norme française.
 *               FONCTION INTERNE. Utiliser _1990_action_categorie_bat.
 * Paramètres : unsigned int type : catégorie de l'action.
 * Valeur renvoyée :
 *   Succès : ACTION_POIDS_PROPRE : Poids propre
 *            ACTION_PRECONTRAINTE : Précontrainte
 *            ACTION_VARIABLE : Action variable
 *            ACTION_ACCIDENTELLE : Action accidentelle
 *            ACTION_SISMIQUE : Action sismique
 *            ACTION_EAUX_SOUTERRAINES : Action due aux eaux souterraines
 *   Échec : ACTION_INCONNUE :
 *             La catégorie n'existe pas
 */
{
    // Trivial
    if (type == 0) 
        return ACTION_POIDS_PROPRE;
    else if (type == 1) 
        return ACTION_PRECONTRAINTE;
    else if ((2 <= type) && (type <= 18))
        return ACTION_VARIABLE;
    else if (type == 19)
        return ACTION_ACCIDENTELLE;
    else if (type == 20)
        return ACTION_SISMIQUE;
    else if (type == 21)
        return ACTION_EAUX_SOUTERRAINES;
    else
        BUGMSG(0, ACTION_INCONNUE, gettext("Type d'action %u inconnu.\n"), type);
}


G_MODULE_EXPORT Action_Categorie _1990_action_categorie_bat(unsigned int type, Type_Pays pays)
/* Description : renvoie la catégorie d'une action pour les combinaisons d'action des bâtiments
 *               en fonction du pays.
 * Paramètres : unsigned int type : type de l'action,
 *            : Type_Pays pays : le numéro du pays.
 * Valeur renvoyée :
 *   Succès : cf les fonctions _1990_action_categorie_bat_PAYS
 *   Échec : ACTION_INCONNUE :
 *             La catégorie n'existe pas,
 *             Le pays n'existe pas.
 */
{
    // Trivial
    switch (pays)
    {
        case PAYS_EU : { return _1990_action_categorie_bat_eu(type); break; }
        case PAYS_FR : { return _1990_action_categorie_bat_fr(type); break; }
        default : { BUGMSG(0, ACTION_INCONNUE, gettext("Pays %d inconnu.\n"), pays); break; }
    }
}


G_MODULE_EXPORT Action_Categorie _1990_action_num_bat_txt(Type_Pays pays)
/* Description : renvoie le nombre de catégories d'actions des bâtiments en fonction du pays.
 * Paramètres : Type_Pays pays : le numéro du pays.
 * Valeur renvoyée :
 *   Succès : le nombre de catégorie d'actions
 *   Échec : 0 :
 *             Le pays n'existe pas.
 */
{
    // Trivial
    switch (pays)
    {
        case PAYS_EU : { return 17; break; }
        case PAYS_FR : { return 22; break; }
        default : { BUGMSG(0, 0, gettext("Pays %d inconnu.\n"), pays); break; }
    }
}


G_MODULE_EXPORT int _1990_action_init(Projet *projet)
/* Description : Initialise la liste des actions.
 * Paramètres : Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 :
 *             projet == NULL.
 */
{
    unsigned int    i;
    GtkWidget       *w_temp;
    
    // Trivial
    BUGMSG(projet, -1, gettext("Paramètre %s incorrect.\n"), "projet");
    
    projet->actions = NULL;
    projet->list_gtk._1990_actions.list_actions = gtk_list_store_new(1, G_TYPE_STRING);
    projet->list_gtk._1990_actions.builder = NULL;
    
    projet->list_gtk._1990_actions.menu_list_widget_action = NULL;
    projet->list_gtk._1990_actions.menu_type_list_action = gtk_menu_new();
    projet->list_gtk._1990_actions.choix_type_action = gtk_list_store_new(1, G_TYPE_STRING);
    for (i=0;i<_1990_action_num_bat_txt(projet->pays);i++)
    {
        GtkTreeIter iter;
        
        // Génération du menu contenant la liste des types d'action pour la création d'une nouvelle action.
        w_temp = gtk_menu_item_new_with_label(_1990_action_type_bat_txt(i, projet->pays));
        gtk_menu_shell_append(GTK_MENU_SHELL(projet->list_gtk._1990_actions.menu_type_list_action), w_temp);
        projet->list_gtk._1990_actions.menu_list_widget_action = g_list_append(projet->list_gtk._1990_actions.menu_list_widget_action, w_temp);
        gtk_widget_show(w_temp);
        g_signal_connect(w_temp, "activate", G_CALLBACK(_1990_gtk_menu_nouvelle_action_activate), projet);
        
        // Génération de la liste des types d'action pour la modification via le treeview Action.
        gtk_list_store_append(projet->list_gtk._1990_actions.choix_type_action, &iter);
        gtk_list_store_set(projet->list_gtk._1990_actions.choix_type_action, &iter, 0, _1990_action_type_bat_txt(i, projet->pays), -1);
    }
    
    projet->list_gtk._1990_actions.menu_list_widget_charge = NULL;
    projet->list_gtk._1990_actions.menu_type_list_charge = gtk_menu_new();
    w_temp = gtk_menu_item_new_with_label(gettext("Charge nodale"));
    gtk_menu_shell_append(GTK_MENU_SHELL(projet->list_gtk._1990_actions.menu_type_list_charge), w_temp);
    projet->list_gtk._1990_actions.menu_list_widget_charge = g_list_append(projet->list_gtk._1990_actions.menu_list_widget_charge, w_temp);
    g_signal_connect(w_temp, "activate", G_CALLBACK(_1990_gtk_menu_nouvelle_charge_nodale_activate), projet);
    w_temp = gtk_menu_item_new_with_label(gettext("Charge ponctuelle sur barre"));
    gtk_menu_shell_append(GTK_MENU_SHELL(projet->list_gtk._1990_actions.menu_type_list_charge), w_temp);
    projet->list_gtk._1990_actions.menu_list_widget_charge = g_list_append(projet->list_gtk._1990_actions.menu_list_widget_charge, w_temp);
    g_signal_connect(w_temp, "activate", G_CALLBACK(_1990_gtk_menu_nouvelle_charge_barre_ponctuelle_activate), projet);
    w_temp = gtk_menu_item_new_with_label(gettext("Charge répartie uniforme sur barre"));
    gtk_menu_shell_append(GTK_MENU_SHELL(projet->list_gtk._1990_actions.menu_type_list_charge), w_temp);
    projet->list_gtk._1990_actions.menu_list_widget_charge = g_list_append(projet->list_gtk._1990_actions.menu_list_widget_charge, w_temp);
    g_signal_connect(w_temp, "activate", G_CALLBACK(_1990_gtk_menu_nouvelle_charge_barre_repartie_uniforme_activate), projet);
    gtk_widget_show_all(projet->list_gtk._1990_actions.menu_type_list_charge);
    
    // Sinon, ils sont libérés à la première fermeture de la fenêtre Actions et ils ne réapparaissent plus lors de la deuxième ouverture.
    g_object_ref(projet->list_gtk._1990_actions.menu_type_list_action);
    g_object_ref(projet->list_gtk._1990_actions.menu_type_list_charge);
    
    return 0;
}


G_MODULE_EXPORT Action *_1990_action_ajout(Projet *projet, unsigned int type,
  const char* description)
/* Description : ajoute une nouvelle action à la liste des actions en lui attribuant le numéro
 *               suivant le dernier relachement existant.
 * Paramètres : Projet *projet : la variable projet,
 *            : unsigned int type : le type de l'action,
 *            : const char* description : description de la nouvelle action.
 * Valeur renvoyée :
 *   Succès : Pointeur vers la nouvelle action
 *   Échec : NULL :
 *             projet == NULL,
 *             erreur d'allocation mémoire,
 *             _1990_coef_psi0_bat,
 *             _1990_coef_psi1_bat,
 *             _1990_coef_psi2_bat.
 */
{
    // Trivial
    Action      *action_nouveau;
    
    BUGMSG(projet, NULL, gettext("Paramètre %s incorrect.\n"), "projet");
    BUG(_1990_action_categorie_bat(type, projet->pays) != ACTION_INCONNUE, NULL);
    
    BUGMSG(action_nouveau = (Action*)malloc(sizeof(Action)), NULL, gettext("Erreur d'allocation mémoire.\n"));
    BUGMSG(action_nouveau->description = g_strdup_printf("%s", description), NULL, gettext("Erreur d'allocation mémoire.\n"));
    action_nouveau->numero = g_list_length(projet->actions);
    action_nouveau->type = type;
    action_nouveau->charges = NULL;
    action_nouveau->flags = 0;
    action_nouveau->psi0 = _1990_coef_psi0_bat(type, projet->pays);
    BUG(action_nouveau->psi0 >= 0., NULL);
    action_nouveau->psi1 = _1990_coef_psi1_bat(type, projet->pays);
    BUG(action_nouveau->psi1 >= 0., NULL);
    action_nouveau->psi2 = _1990_coef_psi2_bat(type, projet->pays);
    BUG(action_nouveau->psi2 >= 0., NULL);
    action_nouveau->deplacement_complet = NULL;
    action_nouveau->forces_complet = NULL;
    action_nouveau->efforts_noeuds = NULL;
    action_nouveau->fonctions_efforts[0] = NULL;
    action_nouveau->fonctions_efforts[1] = NULL;
    action_nouveau->fonctions_efforts[2] = NULL;
    action_nouveau->fonctions_efforts[3] = NULL;
    action_nouveau->fonctions_efforts[4] = NULL;
    action_nouveau->fonctions_efforts[5] = NULL;
    action_nouveau->fonctions_deformation[0] = NULL;
    action_nouveau->fonctions_deformation[1] = NULL;
    action_nouveau->fonctions_deformation[2] = NULL;
    action_nouveau->fonctions_rotation[0] = NULL;
    action_nouveau->fonctions_rotation[1] = NULL;
    action_nouveau->fonctions_rotation[2] = NULL;
    
    projet->actions = g_list_append(projet->actions, action_nouveau);
     
    gtk_list_store_append(projet->list_gtk._1990_actions.list_actions, &action_nouveau->Iter_liste);
    gtk_list_store_set(projet->list_gtk._1990_actions.list_actions, &action_nouveau->Iter_liste, 0, action_nouveau->description, -1);
    
    if (projet->list_gtk._1990_actions.builder)
    {
        GtkTreePath *path;
        
        gtk_tree_store_append(projet->list_gtk._1990_actions.tree_store_actions, &action_nouveau->Iter_fenetre, NULL);
        gtk_tree_store_set(projet->list_gtk._1990_actions.tree_store_actions, &action_nouveau->Iter_fenetre, 0, action_nouveau->numero, 1, action_nouveau->description, 2, _1990_action_type_bat_txt(action_nouveau->type, projet->pays), 3, action_nouveau->psi0, 4, action_nouveau->psi1, 5, action_nouveau->psi2, -1);
        path = gtk_tree_model_get_path(GTK_TREE_MODEL(projet->list_gtk._1990_actions.tree_store_actions), &action_nouveau->Iter_fenetre);
        gtk_tree_view_set_cursor(GTK_TREE_VIEW(projet->list_gtk._1990_actions.tree_view_actions), path, gtk_tree_view_get_column(GTK_TREE_VIEW(projet->list_gtk._1990_actions.tree_view_actions), 1), TRUE);
        gtk_tree_path_free(path);
    }
    
    return action_nouveau;
}


G_MODULE_EXPORT Action* _1990_action_cherche_numero(Projet *projet, unsigned int numero)
/* Description : Cherche et renvoie l'action désignée par numero.
 * Paramètres : Projet *projet : la variable projet,
 *            : unsigned int numero : le numéro de l'action.
 * Valeur renvoyée :
 *   Succès : Pointeur vers l'action recherchée
 *   Échec : NULL :
 *             projet == NULL,
 *             action introuvable.
 */
{
    GList   *list_parcours;
    
    BUGMSG(projet, NULL, gettext("Paramètre %s incorrect.\n"), "projet");
    
    // Trivial
    list_parcours = projet->actions;
    while (list_parcours != NULL)
    {
        Action  *action = (Action*)list_parcours->data;
        
        if (action->numero == numero)
            return action;
        
        list_parcours = g_list_next(list_parcours);
    }
    
    BUGMSG(0, NULL, gettext("Action %u introuvable.\n"), numero);
}


G_MODULE_EXPORT int _1990_action_renomme(Projet *projet, unsigned int action_num,
  const char* description)
/* Description : Renomme une charge.
 * Paramètres : Projet *projet : la variable projet,
 *              unsigned int action : numéro de l'action à renommer,
 *              const char* description : nouveau nom de l'action.
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 :
 *             projet == NULL,
 *             action introuvable,
 *             erreur d'allocation mémoire.
 */
{
    Action      *action;
    
    BUGMSG(projet, -1, gettext("Paramètre %s incorrect.\n"), "projet");
    
    BUG(action = _1990_action_cherche_numero(projet, action_num), -1);
    
    free(action->description);
    BUGMSG(action->description = g_strdup_printf("%s", description), -1, gettext("Erreur d'allocation mémoire.\n"));
    
    gtk_list_store_set(projet->list_gtk._1990_actions.list_actions, &action->Iter_liste, 0, description, -1);
    
    if (projet->list_gtk._1990_actions.builder != NULL)
        gtk_tree_store_set(projet->list_gtk._1990_actions.tree_store_actions, &action->Iter_fenetre, 1, description, -1);
    
    return 0;
}


G_MODULE_EXPORT int _1990_action_change_type(Projet *projet, unsigned int action_num,
  unsigned int type)
/* Description : Change le type d'une action, y compris psi0, psi1 et psi2.
 * Paramètres : Projet *projet : la variable projet,
 *              unsigned int action_num : numéro de l'action à renommer,
 *              unsigned int type : nouveau type d'action.
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 :
 *             projet == NULL,
 *             action introuvable,
 *             erreur d'allocation mémoire.
 */
{
    Action      *action;
    
    BUGMSG(projet, -1, gettext("Paramètre %s incorrect.\n"), "projet");
    
    BUG(action = _1990_action_cherche_numero(projet, action_num), -1);
    
    action->type = type;
    action->psi0 = _1990_coef_psi0_bat(type, projet->pays);
    action->psi1 = _1990_coef_psi1_bat(type, projet->pays);
    action->psi2 = _1990_coef_psi2_bat(type, projet->pays);
    
    if (projet->list_gtk._1990_actions.builder != NULL)
        gtk_tree_store_set(projet->list_gtk._1990_actions.tree_store_actions, &action->Iter_fenetre, 2, _1990_action_type_bat_txt(type, projet->pays), 3, action->psi0, 4, action->psi1, 5, action->psi2, -1);
    
    return 0;
}


G_MODULE_EXPORT int _1990_action_change_psi(Projet *projet, unsigned int action_num,
  unsigned int psi_num, double psi)
/* Description : Change le coefficient psi d'une action.
 * Paramètres : Projet *projet : la variable projet,
 *              unsigned int action_num : numéro de l'action à changer,
 *              unsigned int psi_num : coefficient psi à changer (0, 1 ou 2),
 *              double psi : nouveau coefficient psi.
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 :
 *             projet == NULL,
 *             action introuvable,
 *             coefficient psi_num incorrect,
 *             erreur d'allocation mémoire.
 */
{
    Action      *action;
    
    BUGMSG(projet, -1, gettext("Paramètre %s incorrect.\n"), "projet");
    BUGMSG((psi_num == 0) || (psi_num == 1) || (psi_num == 2), -1, gettext("Le numéro %u du coefficient spi à changer est incorrect.\n"), psi_num);
    
    BUG(action = _1990_action_cherche_numero(projet, action_num), -1);
    
    if (psi_num == 0)
    {
        action->psi0 = psi;
        if (projet->list_gtk._1990_actions.builder != NULL)
            gtk_tree_store_set(projet->list_gtk._1990_actions.tree_store_actions, &action->Iter_fenetre, 3, psi, -1);
    }
    else if (psi_num == 1)
    {
        action->psi1 = psi;
        if (projet->list_gtk._1990_actions.builder != NULL)
            gtk_tree_store_set(projet->list_gtk._1990_actions.tree_store_actions, &action->Iter_fenetre, 4, psi, -1);
    }
    else
    {
        action->psi2 = psi;
        if (projet->list_gtk._1990_actions.builder != NULL)
            gtk_tree_store_set(projet->list_gtk._1990_actions.tree_store_actions, &action->Iter_fenetre, 5, psi, -1);
    }
    
    return 0;
}


G_MODULE_EXPORT int _1990_action_affiche_tout(Projet *projet)
/* Description : Affiche dans l'entrée standard les actions existantes.
 * Paramètres : Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 :
 *             projet == NULL.
 */
{
    GList   *list_parcours;
    
    BUGMSG(projet, -1, gettext("Paramètre %s incorrect.\n"), "projet");
    if (projet->actions == NULL)
    {
        printf(gettext("Aucune action existante.\n"));
        return 0;
    }
    
    // Trivial
    list_parcours = projet->actions;
    do
    {
        Action      *action = list_parcours->data;
        
        printf(gettext("Action n° %u, description '%s', type n°%d\n"), action->numero, action->description, action->type);
        
        list_parcours = g_list_next(list_parcours);
    }
    while (list_parcours != NULL);
    
    return 0;
}


G_MODULE_EXPORT int _1990_action_affiche_resultats(Projet *projet, unsigned int num_action)
/* Description : Affiche tous les résultats d'une action.
 * Paramètres : Projet *projet : la variable projet,
 *            : int num_action : numéro de l'action.
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 :
 *             projet == NULL,
 *             action introuvable,
 *             modèle de calculs vide (absence de barres),
 */
{
    Action          *action_en_cours;
    unsigned int    i;
    
    BUGMSG(projet, -1, gettext("Paramètre %s incorrect.\n"), "projet");
    BUG(action_en_cours = _1990_action_cherche_numero(projet, num_action), -1);
    if (projet->beton.barres == NULL)
    {
        printf(gettext("Aucune barre existante.\n"));
        return 0;
    }
    
    // Affichage des efforts aux noeuds et des réactions d'appuis
    printf("Effort aux noeuds & Réactions d'appuis :\n");
    common_math_arrondi_sparse(action_en_cours->efforts_noeuds);
    cholmod_l_write_sparse(stdout, action_en_cours->efforts_noeuds, NULL, NULL, projet->ef_donnees.c);
    // Affichage des déplacements des noeuds
    printf("\nDéplacements :\n");
    common_math_arrondi_sparse(action_en_cours->deplacement_complet);
    cholmod_l_write_sparse(stdout, action_en_cours->deplacement_complet, NULL, NULL, projet->ef_donnees.c);
    // Pour chaque barre
    for (i=0;i<g_list_length(projet->beton.barres);i++)
    {
    //     Affichage de la courbe des sollicitations vis-à-vis de l'effort normal
        printf("Barre n°%d, Effort normal\n", i);
        BUG(common_fonction_affiche(action_en_cours->fonctions_efforts[0][i]) == 0, -1);
    //     Affichage de la courbe des sollicitations vis-à-vis de l'effort tranchant selon Y
        printf("Barre n°%d, Effort tranchant Y\n", i);
        BUG(common_fonction_affiche(action_en_cours->fonctions_efforts[1][i]) == 0, -1);
    //     Affichage de la courbe des sollicitations vis-à-vis de l'effort tranchant selon Z
        printf("Barre n°%d, Effort tranchant Z\n", i);
        BUG(common_fonction_affiche(action_en_cours->fonctions_efforts[2][i]) == 0, -1);
    //     Affichage de la courbe des sollicitations vis-à-vis du moment de torsion
        printf("Barre n°%d, Moment de torsion\n", i);
        BUG(common_fonction_affiche(action_en_cours->fonctions_efforts[3][i]) == 0, -1);
    //     Affichage de la courbe des sollicitations vis-à-vis du moment fléchissant selon Y
        printf("Barre n°%d, Moment de flexion Y\n", i);
        BUG(common_fonction_affiche(action_en_cours->fonctions_efforts[4][i]) == 0, -1);
    //     Affichage de la courbe des sollicitations vis-à-vis du moment fléchissant selon Z
        printf("Barre n°%d, Moment de flexion Z\n", i);
        BUG(common_fonction_affiche(action_en_cours->fonctions_efforts[5][i]) == 0, -1);
    }
    for (i=0;i<g_list_length(projet->beton.barres);i++)
    {
    //     Affichage de la courbe de déformation selon l'axe X
        printf("Barre n°%d, Déformation en X\n", i);
        BUG(common_fonction_affiche(action_en_cours->fonctions_deformation[0][i]) == 0, -1);
    //     Affichage de la courbe de déformation selon l'axe Y
        printf("Barre n°%d, Déformation en Y\n", i);
        BUG(common_fonction_affiche(action_en_cours->fonctions_deformation[1][i]) == 0, -1);
    //     Affichage de la courbe de déformation selon l'axe Z
        printf("Barre n°%d, Déformation en Z\n", i);
        BUG(common_fonction_affiche(action_en_cours->fonctions_deformation[2][i]) == 0, -1);
    //     Affichage de la courbe de rotation selon l'axe X
        printf("Barre n°%d, Rotation en X\n", i);
        BUG(common_fonction_affiche(action_en_cours->fonctions_rotation[0][i]) == 0, -1);
    //     Affichage de la courbe de rotation selon l'axe Y
        printf("Barre n°%d, Rotation en Y\n", i);
        BUG(common_fonction_affiche(action_en_cours->fonctions_rotation[1][i]) == 0, -1);
    //     Affichage de la courbe de rotation selon l'axe Z
        printf("Barre n°%d, Rotation en Z\n", i);
        BUG(common_fonction_affiche(action_en_cours->fonctions_rotation[2][i]) == 0, -1);
    }
    // FinPour
    
    return 0;
}


G_MODULE_EXPORT int _1990_action_free_num(Projet *projet, unsigned int num)
/* Description : Libère l'action souhaitée et décrémente de 1 les actions dons le numéro est
 *               supérieur.
 * Paramètres : Projet *projet : la variable projet,
 *            : unsigned int num : le numéro de l'action à supprimer.
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 :
 *             projet == NULL,
 *             action introuvable,
 *             erreur lors de la libération d'une charge de l'action à supprimer,
 *             erreur d'allocation métmoire.
 */
{
    GList   *list_parcours;
    
    BUGMSG(projet, -1, gettext("Paramètre %s incorrect.\n"), "projet");
    BUG(_1990_action_cherche_numero(projet, num), -1);
    
    // Trivial
    list_parcours = g_list_last(projet->actions);
    do
    {
        Action      *action = list_parcours->data;
        
        list_parcours = g_list_previous(list_parcours);
        if (action->numero == num)
        {
            projet->actions = g_list_remove(projet->actions, action);
            free(action->description);
            while (action->charges != NULL)
            {
                Charge_Barre_Ponctuelle *charge;
                
                charge = action->charges->data;
                action->charges = g_list_delete_link(action->charges, action->charges);
                switch (charge->type)
                {
                    case CHARGE_NOEUD :
                    {
                        Charge_Noeud *charge2 = (Charge_Noeud *)charge;
                        BUG(EF_charge_noeud_free(charge2) == 0, -1);
                        break;
                    }
                    case CHARGE_BARRE_PONCTUELLE :
                    {
                        BUG(EF_charge_barre_ponctuelle_free(charge) == 0, -1);
                        break;
                    }
                    case CHARGE_BARRE_REPARTIE_UNIFORME :
                    {
                        Charge_Barre_Repartie_Uniforme *charge2 = (Charge_Barre_Repartie_Uniforme *)charge;
                        BUG(EF_charge_barre_repartie_uniforme_free(charge2) == 0, -1);
                        break;
                    }
                    default :
                    {
                        BUGMSG(0, -1, gettext("Type de charge %d inconnu."), charge->type);
                        break;
                    }
                }
            }
            if (action->deplacement_complet != NULL)
                cholmod_l_free_sparse(&action->deplacement_complet, projet->ef_donnees.c);
            if (action->forces_complet != NULL)
                cholmod_l_free_sparse(&action->forces_complet, projet->ef_donnees.c);
            if (action->efforts_noeuds != NULL)
                cholmod_l_free_sparse(&action->efforts_noeuds, projet->ef_donnees.c);
            
            if (action->fonctions_efforts[0] != NULL)
                BUG(common_fonction_free(projet, action) == 0, -1);
            
            if (projet->list_gtk._1990_actions.builder != NULL)
            {
                if (gtk_tree_selection_iter_is_selected(projet->list_gtk._1990_actions.tree_select_actions, &action->Iter_fenetre))
                    gtk_tree_store_clear(projet->list_gtk._1990_actions.tree_store_charges);
                gtk_tree_store_remove(projet->list_gtk._1990_actions.tree_store_actions, &action->Iter_fenetre);
            }
            
            gtk_list_store_remove(projet->list_gtk._1990_actions.list_actions, &action->Iter_liste);
            
            free(action);
        }
        else if (action->numero > num)
        {
            action->numero--;
            
            if (projet->list_gtk._1990_actions.builder != NULL)
                gtk_tree_store_set(projet->list_gtk._1990_actions.tree_store_actions, &action->Iter_fenetre, 0, action->numero, -1);
        }
        
    } while (list_parcours != NULL);
    
    return 0;
}


G_MODULE_EXPORT int _1990_action_free(Projet *projet)
/* Description : Libère l'ensemble des actions existantes.
 * Paramètres : Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 :
 *             projet == NULL,
 *             erreur lors de la libération d'une charge de l'action à supprimer.
 */
{
    BUGMSG(projet, -1, gettext("Paramètre %s incorrect.\n"), "projet");
    
    // Trivial
    while (projet->actions != NULL)
    {
        Action      *action = projet->actions->data;
        
        projet->actions = g_list_delete_link(projet->actions, projet->actions);
        
        free(action->description);
        while (action->charges != NULL)
        {
            Charge_Barre_Ponctuelle *charge = action->charges->data;
            
            action->charges = g_list_delete_link(action->charges, action->charges);
            
            switch (charge->type)
            {
                case CHARGE_NOEUD :
                {
                    BUG(EF_charge_noeud_free((Charge_Noeud*)charge) == 0, -1);
                    break;
                }
                case CHARGE_BARRE_PONCTUELLE :
                {
                    BUG(EF_charge_barre_ponctuelle_free(charge) == 0, -1);
                    break;
                }
                case CHARGE_BARRE_REPARTIE_UNIFORME :
                {
                    BUG(EF_charge_barre_repartie_uniforme_free((Charge_Barre_Repartie_Uniforme*)charge) == 0, -1);
                    break;
                }
                default :
                {
                    BUGMSG(0, -1, gettext("Type de charge %d inconnu."), charge->type);
                    break;
                }
            }
        }
        if (action->deplacement_complet != NULL)
            cholmod_l_free_sparse(&action->deplacement_complet, projet->ef_donnees.c);
        if (action->forces_complet != NULL)
            cholmod_l_free_sparse(&action->forces_complet, projet->ef_donnees.c);
        if (action->efforts_noeuds != NULL)
            cholmod_l_free_sparse(&action->efforts_noeuds, projet->ef_donnees.c);
        
        if (action->fonctions_efforts[0] != NULL)
            BUG(common_fonction_free(projet, action) == 0, -1);
        
        free(action);
    }
    
    if (projet->list_gtk._1990_actions.builder != NULL)
    {
        gtk_list_store_clear(projet->list_gtk._1990_actions.list_actions);
        gtk_tree_store_clear(projet->list_gtk._1990_actions.tree_store_charges);
    }
    g_object_unref(projet->list_gtk._1990_actions.menu_type_list_action);
    g_object_unref(projet->list_gtk._1990_actions.menu_type_list_charge);
    
    return 0;
}
