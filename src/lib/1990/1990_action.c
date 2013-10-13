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

/** \file 1990_action.c
 * Code source de tout ce qui concerne les actions de l'Eurocode 0.
 */

#include "config.h"
#include <libintl.h>
#include <locale.h>
#include <cholmod.h>
#include <string.h>
#include <gmodule.h>
#include <math.h>

#include "1990_action_private.h"
#include "common_projet.h"
#include "1990_coef_psi.h"
#include "1990_groupe.h"
#include "common_math.h"
#include "common_erreurs.h"
#include "common_fonction.h"
#include "EF_charge_noeud.h"
#include "EF_charge_barre_ponctuelle.h"
#include "EF_charge_barre_repartie_uniforme.h"
#include "EF_calculs.h"

#ifdef ENABLE_GTK
#include "common_gtk.h"
#include "1990_gtk_actions.h"
#endif

char *
_1990_action_bat_txt_type_eu (unsigned int type)
/**
 * \brief Renvoie la description du type de l'action pour les bâtiments de la
 *        norme européenne.
 * \param type : type de l'action. Ce paramètre doit être compris entre 0 et la
 *               valeur retournée par #_1990_action_num_bat_txt.
 * \return 
 *   Succès : le texte correspondant en fonction du paramètre type :\n
 *     - 0 : "Permanente",
 *     - 1 : "Précontrainte",
 *     - 2 : "Exploitation : Catégorie A : habitation, zones résidentielles",
 *     - 3 : "Exploitation : Catégorie B : bureaux",
 *     - 4 : "Exploitation : Catégorie C : lieux de réunion",
 *     - 5 : "Exploitation : Catégorie D : commerces",
 *     - 6 : "Exploitation : Catégorie E : stockage",
 *     - 7 : "Exploitation : Catégorie F : zone de trafic, véhicules inférieur
 *            à 30 kN",
 *     - 8 : "Exploitation : Catégorie G : zone de trafic, véhicules entre 30
 *            kN et 160 kN",
 *     - 9 : "Exploitation : Catégorie H : toits",
 *     - 10: "Neige : Finlande, Islande, Norvège, Suède",
 *     - 11: "Neige : Autres états membres CEN, altitude > 1000 m",
 *     - 12: "Neige : Autres états membres CEN, altitude <= 1000 m",
 *     - 13: "Vent",
 *     - 14: "Température (hors incendie)",
 *     - 15: "Accidentelle",
 *     - 16: "Sismique".
 * \return Échec : NULL :
 *   - Le type d'action n'existe pas.
 * \warning Fonction interne. Il convient d'utiliser la fonction
 *          #_1990_action_bat_txt_type.
 */
{
    switch (type)
    {
        case 0 : { return gettext ("Permanente"); break; }
        case 1 : { return gettext ("Précontrainte"); break; }
        case 2 : { return gettext ("Exploitation : Catégorie A : habitation, zones résidentielles"); break; }
        case 3 : { return gettext ("Exploitation : Catégorie B : bureaux"); break; }
        case 4 : { return gettext ("Exploitation : Catégorie C : lieux de réunion"); break; }
        case 5 : { return gettext ("Exploitation : Catégorie D : commerces"); break; }
        case 6 : { return gettext ("Exploitation : Catégorie E : stockage"); break; }
        case 7 : { return gettext ("Exploitation : Catégorie F : zone de trafic, véhicules de poids inférieur à 30 kN"); break; }
        case 8 : { return gettext ("Exploitation : Catégorie G : zone de trafic, véhicules de poids entre 30 kN et 160 kN"); break; }
        case 9 : { return gettext ("Exploitation : Catégorie H : toits"); break; }
        case 10 : { return gettext ("Neige : Finlande, Islande, Norvège, Suède"); break; }
        case 11 : { return gettext ("Neige : Autres états membres CEN, altitude > 1000 m"); break; }
        case 12 : { return gettext ("Neige : Autres états membres CEN, altitude <= 1000 m"); break; }
        case 13 : { return gettext ("Vent"); break; }
        case 14 : { return gettext ("Température (hors incendie)"); break; }
        case 15 : { return gettext ("Accidentelle"); break; }
        case 16 : { return gettext ("Sismique"); break; }
        default : { BUGMSG (0, NULL, gettext ("Type d'action %u inconnu.\n"), type) break; }
    }
}


char *
_1990_action_bat_txt_type_fr (unsigned int type)
/**
 * \brief Renvoie la description du type de l'action pour les bâtiments de la
 *        norme française.
 * \param type : type de l'action. Ce paramètre doit être compris entre 0 et la
 *               valeur retournée par #_1990_action_num_bat_txt.
 * \return
 *   Succès : le texte correspondant en fonction du paramètre type :\n
 *     - 0 : "Permanente",
 *     - 1 : "Précontrainte",
 *     - 2 : "Exploitation : Catégorie A : habitation, zones résidentielles",
 *     - 3 : "Exploitation : Catégorie B : bureaux",
 *     - 4 : "Exploitation : Catégorie C : lieux de réunion",
 *     - 5 : "Exploitation : Catégorie D : commerces",
 *     - 6 : "Exploitation : Catégorie E : stockage",
 *     - 7 : "Exploitation : Catégorie F : zone de trafic, véhicules inférieur
 *            à 30 kN",
 *     - 8 : "Exploitation : Catégorie G : zone de trafic, véhicules entre 30
 *            kN et 160 kN",
 *     - 9 : "Exploitation : Catégorie H : toits d'un bâtiment de catégorie A
 *            ou B",
 *     - 10: "Exploitation : Catégorie I : toitures accessibles avec locaux de
 *            type A ou B",
 *     - 11: "Exploitation : Catégorie I : toitures accessibles avec locaux de
 *            type C ou D",
 *     - 12: "Exploitation : Catégorie K : Hélicoptère sur la toiture",
 *     - 13: "Exploitation : Catégorie K : Hélicoptère sur la toiture, autres
 *            charges",
 *     - 14: "Neige : Saint-Pierre-et-Miquelon",
 *     - 15: "Neige : Altitude > 1000 m",
 *     - 16: "Neige : Altitude <= 1000 m",
 *     - 17: "Vent",
 *     - 18: "Température (hors incendie)",
 *     - 19: "Accidentelle",
 *     - 20: "Sismique",
 *     - 21: "Eaux souterraines".
 * \return Échec : NULL :
 *   - Le type d'action n'existe pas.
 * \warning Fonction interne. Il convient d'utiliser la fonction
 *          #_1990_action_bat_txt_type.
 */
{
    switch (type)
    {
        case 0 : { return gettext ("Permanente"); break; }
        case 1 : { return gettext ("Précontrainte"); break; }
        case 2 : { return gettext ("Exploitation : Catégorie A : habitation, zones résidentielles"); break; }
        case 3 : { return gettext ("Exploitation : Catégorie B : bureaux"); break; }
        case 4 : { return gettext ("Exploitation : Catégorie C : lieux de réunion"); break; }
        case 5 : { return gettext ("Exploitation : Catégorie D : commerces"); break; }
        case 6 : { return gettext ("Exploitation : Catégorie E : stockage"); break; }
        case 7 : { return gettext ("Exploitation : Catégorie F : zone de trafic, véhicules de poids inférieur à 30 kN"); break; }
        case 8 : { return gettext ("Exploitation : Catégorie G : zone de trafic, véhicules de poids entre 30 kN et 160 kN"); break; }
        case 9 : { return gettext ("Exploitation : Catégorie H : toits d'un bâtiment de catégorie A ou B"); break; }
        case 10 : { return gettext ("Exploitation : Catégorie I : toitures accessibles avec locaux des catégories A ou B"); break; }
        case 11 : { return gettext ("Exploitation : Catégorie I : toitures accessibles avec locaux des catégories C ou D"); break; }
        case 12 : { return gettext ("Exploitation : Catégorie K : Hélicoptère sur la toiture"); break; }
        case 13 : { return gettext ("Exploitation : Catégorie K : Hélicoptère sur la toiture, autres charges (fret, personnel, accessoires ou équipements divers)"); break; }
        case 14 : { return gettext ("Neige : Saint-Pierre-et-Miquelon"); break; }
        case 15 : { return gettext ("Neige : Altitude > 1000 m"); break; }
        case 16 : { return gettext ("Neige : Altitude <= 1000 m"); break; }
        case 17 : { return gettext ("Vent"); break; }
        case 18 : { return gettext ("Température (hors incendie)"); break; }
        case 19 : { return gettext ("Accidentelle"); break; }
        case 20 : { return gettext ("Sismique"); break; }
        case 21 : { return gettext ("Eaux souterraines"); break; }
        default : { BUGMSG (0, NULL, gettext ("Type d'action %u inconnu.\n"), type) break; }
    }
}


char *
_1990_action_bat_txt_type (unsigned int type, Norme norme)
/**
 * \brief Renvoie la description du type de l'action pour les bâtiments en
 *        fonction de la norme demandée.
 * \param type : type de l'action,
 * \param norme : la norme souhaitée.
 * \return
 *   Succès : cf. _1990_action_bat_txt_type_PAYS\n
 *   Échec : NULL :
 *     - le type d'action n'existe pas,
 *     - la norme n'existe pas.
 */
{
    switch (norme)
    {
        case NORME_EU : { return _1990_action_bat_txt_type_eu (type); break; }
        case NORME_FR : { return _1990_action_bat_txt_type_fr (type); break; }
        default : { BUGMSG (0, NULL, gettext ("Norme %d inconnue.\n"), norme) break; }
    }
}


Action_Categorie
_1990_action_categorie_bat_eu (unsigned int type)
/**
 * \brief Renvoie la catégorie du type de l'action pour les bâtiments selon la
 *        norme européenne.
 * \param type : type de l'action.
 * \return
 *   Succès :\n
 *     - 0 : ACTION_POIDS_PROPRE (Poids propre),
 *     - 1 : ACTION_PRECONTRAINTE (Précontrainte),
 *     - 2 à 14 : ACTION_VARIABLE (Action variable),
 *     - 15 : ACTION_ACCIDENTELLE (Action accidentelle),
 *     - 16 : ACTION_SISMIQUE (Action sismique).
 * \return Échec : ACTION_INCONNUE :
 *   - Le type d'action n'existe pas.
 * \warning Fonction interne. Il convient d'utiliser la fonction
 *          #_1990_action_categorie_bat.
 */
{
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
        BUGMSG (0, ACTION_INCONNUE, gettext ("Type d'action %u inconnu.\n"), type)
}


Action_Categorie
_1990_action_categorie_bat_fr (unsigned int type)
/**
 * \brief Renvoie la catégorie du type de l'action pour les bâtiments selon la
 *        norme française.
 * \param type : type de l'action.
 * \return
 *   Succès :\n
 *     - 0 : ACTION_POIDS_PROPRE (Poids propre),
 *     - 1 : ACTION_PRECONTRAINTE (Précontrainte),
 *     - 2 à 18: ACTION_VARIABLE (Action variable),
 *     - 19 : ACTION_ACCIDENTELLE (Action accidentelle),
 *     - 20 : ACTION_SISMIQUE (Action sismique),
 *     - 21 : ACTION_EAUX_SOUTERRAINES (Action due aux eaux souterraines).
 * \return Échec : ACTION_INCONNUE :
 *   - Le type d'action n'existe pas.
 * \warning Fonction interne. Il convient d'utiliser la fonction
 *          #_1990_action_categorie_bat.
 */
{
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
        BUGMSG (0, ACTION_INCONNUE, gettext ("Type d'action %u inconnu.\n"), type)
}


Action_Categorie
_1990_action_categorie_bat (unsigned int type, Norme norme)
/**
 * \brief Renvoie la catégorie du type de l'action pour les bâtiments en
 *        fonction de la norme souhaitée.
 * \param type : type de l'action,
 * \param norme : la norme souhaitée.
 * \return
 *   Succès : cf. _1990_action_categorie_bat_PAYS\n
 *   Échec : ACTION_INCONNUE :
 *     - La catégorie n'existe pas,
 *     - La norme n'existe pas.
 */
{
    switch (norme)
    {
        case NORME_EU : { return _1990_action_categorie_bat_eu (type); break; }
        case NORME_FR : { return _1990_action_categorie_bat_fr (type); break; }
        default : { BUGMSG (0, ACTION_INCONNUE, gettext ("Norme %d inconnue.\n"), norme) break; }
    }
}


Action_Categorie _1990_action_num_bat_txt(Norme norme)
/* Description : renvoie le nombre de catégories d'actions des bâtiments en fonction de la norme.
 * Paramètres : Norme norme : le numéro de la norme.
 * Valeur renvoyée :
 *   Succès : le nombre de catégorie d'actions
 *   Échec : 0 :
 *             La norme n'existe pas.
 */
{
    // Trivial
    switch (norme)
    {
        case NORME_EU : { return 17; break; }
        case NORME_FR : { return 22; break; }
        default : { BUGMSG(0, 0, gettext("Norme %d inconnue.\n"), norme) break; }
    }
}


gboolean _1990_action_init(Projet *projet)
/* Description : Initialise la liste des actions.
 * Paramètres : Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL.
 */
{
#ifdef ENABLE_GTK
    unsigned int    i;
    GtkWidget       *w_temp;
#endif
    
    // Trivial
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet")
    
    projet->actions = NULL;
    
#ifdef ENABLE_GTK
    projet->list_gtk._1990_actions.list_actions = gtk_list_store_new(1, G_TYPE_STRING);
    projet->list_gtk._1990_actions.builder = NULL;
    
    projet->list_gtk._1990_actions.menu_list_widget_action = NULL;
    projet->list_gtk._1990_actions.menu_type_list_action = gtk_menu_new();
    projet->list_gtk._1990_actions.choix_type_action = gtk_list_store_new(1, G_TYPE_STRING);
    for (i=0;i<_1990_action_num_bat_txt(projet->parametres.norme);i++)
    {
        GtkTreeIter iter;
        
        // Génération du menu contenant la liste des types d'action pour la création d'une nouvelle action.
        w_temp = gtk_menu_item_new_with_label(_1990_action_bat_txt_type(i, projet->parametres.norme));
        gtk_menu_shell_append(GTK_MENU_SHELL(projet->list_gtk._1990_actions.menu_type_list_action), w_temp);
        projet->list_gtk._1990_actions.menu_list_widget_action = g_list_append(projet->list_gtk._1990_actions.menu_list_widget_action, w_temp);
        gtk_widget_show(w_temp);
        g_signal_connect(w_temp, "activate", G_CALLBACK(_1990_gtk_menu_nouvelle_action_activate), projet);
        
        // Génération de la liste des types d'action pour la modification via le treeview Action.
        gtk_list_store_append(projet->list_gtk._1990_actions.choix_type_action, &iter);
        gtk_list_store_set(projet->list_gtk._1990_actions.choix_type_action, &iter, 0, _1990_action_bat_txt_type(i, projet->parametres.norme), -1);
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
    g_object_ref(projet->list_gtk._1990_actions.menu_type_list_charge);
#endif
    
    return TRUE;
}


Action *_1990_action_ajout(Projet *projet, unsigned int type, const char* description)
/* Description : Ajoute une nouvelle action à la liste des actions en lui attribuant le numéro
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
    
    BUGMSG(projet, NULL, gettext("Paramètre %s incorrect.\n"), "projet")
    BUG(_1990_action_categorie_bat(type, projet->parametres.norme) != ACTION_INCONNUE, NULL)
    
    BUGMSG(action_nouveau = (Action*)malloc(sizeof(Action)), NULL, gettext("Erreur d'allocation mémoire.\n"))
    BUGMSG(action_nouveau->nom = g_strdup_printf("%s", description), NULL, gettext("Erreur d'allocation mémoire.\n"))
    action_nouveau->numero = g_list_length(projet->actions);
    action_nouveau->type = type;
    action_nouveau->charges = NULL;
    action_nouveau->action_predominante = 0;
    action_nouveau->psi0 = common_math_f(_1990_coef_psi0_bat(type, projet->parametres.norme), FLOTTANT_ORDINATEUR);
    BUG(!isnan(common_math_get(action_nouveau->psi0)), NULL)
    action_nouveau->psi1 = common_math_f(_1990_coef_psi1_bat(type, projet->parametres.norme), FLOTTANT_ORDINATEUR);
    BUG(!isnan(common_math_get(action_nouveau->psi1)), NULL)
    action_nouveau->psi2 = common_math_f(_1990_coef_psi2_bat(type, projet->parametres.norme), FLOTTANT_ORDINATEUR);
    BUG(!isnan(common_math_get(action_nouveau->psi2)), NULL)
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
     
#ifdef ENABLE_GTK
    gtk_list_store_append(projet->list_gtk._1990_actions.list_actions, &action_nouveau->Iter_liste);
    gtk_list_store_set(projet->list_gtk._1990_actions.list_actions, &action_nouveau->Iter_liste, 0, action_nouveau->nom, -1);
    
    if (projet->list_gtk._1990_actions.builder != NULL)
    {
        gtk_tree_store_append(projet->list_gtk._1990_actions.tree_store_actions, &action_nouveau->Iter_fenetre, NULL);
        gtk_tree_store_set(projet->list_gtk._1990_actions.tree_store_actions, &action_nouveau->Iter_fenetre, 0, action_nouveau, -1);
    }
    if ((projet->list_gtk._1990_groupes.builder != NULL) && (GTK_COMMON_SPINBUTTON_AS_UINT(GTK_SPIN_BUTTON(projet->list_gtk._1990_groupes.spin_button_niveau)) == 0))
    {
        GtkTreeIter iter;
        
        gtk_tree_store_append(projet->list_gtk._1990_groupes.tree_store_dispo, &iter, NULL);
        gtk_tree_store_set(projet->list_gtk._1990_groupes.tree_store_dispo, &iter, 0, action_nouveau->numero, 1, action_nouveau->nom, -1);
    }
#endif
    
    BUG(EF_calculs_free(projet), FALSE)
    
    return action_nouveau;
}


const char* _1990_action_nom_renvoie(Action *action)
/* Description : Renvoie le nom de l'action. Il convient de ne pas libérer ou modifier la valeur
 *               renvoyée.
 * Paramètres : Action *action : l'action dont on souhaite connaître le nom.
 * Valeur renvoyée :
 *   Succès : le nom de l'action.
 *   Échec : NULL :
 *             action == NULL,
 */
{
    BUGMSG(action, NULL, gettext("Paramètre %s incorrect.\n"), "action")
    
    return action->nom;
}


gboolean _1990_action_nom_change(Projet *projet, Action *action, const char* nom)
/* Description : Renomme une charge.
 * Paramètres : Projet *projet : la variable projet,
 *            : Action *action : l'action à renommer,
 *            : const char* nom : nouveau nom de l'action.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL,
 *             action introuvable,
 *             erreur d'allocation mémoire.
 */
{
#ifdef ENABLE_GTK
    char    *ancien_nom;
#endif
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet")
    BUGMSG(action, FALSE, gettext("Paramètre %s incorrect.\n"), "action")
    
#ifdef ENABLE_GTK
    ancien_nom = action->nom;
#else
    free(action->nom);
#endif
    BUGMSG(action->nom = g_strdup_printf("%s", nom), FALSE, gettext("Erreur d'allocation mémoire.\n"))
    
#ifdef ENABLE_GTK
    gtk_list_store_set(projet->list_gtk._1990_actions.list_actions, &action->Iter_liste, 0, nom, -1);
    
    if (projet->list_gtk._1990_actions.builder != NULL)
        gtk_widget_queue_resize(GTK_WIDGET(projet->list_gtk._1990_actions.tree_view_actions));
    if ((projet->list_gtk._1990_groupes.builder != NULL) && (GTK_COMMON_SPINBUTTON_AS_UINT(GTK_SPIN_BUTTON(projet->list_gtk._1990_groupes.spin_button_niveau)) == 0))
    {
        GtkTreeModel    *model;
        GtkTreeIter     Iter1;
        
        // On modifie les lignes dans le treeview etat
        model = gtk_tree_view_get_model(projet->list_gtk._1990_groupes.tree_view_etat);
        if (gtk_tree_model_get_iter_first(model, &Iter1))
        {
            do
            {
                GtkTreeIter Iter2;
                
                if (gtk_tree_model_iter_children(model, &Iter2, &Iter1))
                {
                    do
                    {
                        char    *nom_iter;
                        
                        gtk_tree_model_get(model, &Iter2, 1, &nom_iter, -1);
                        if (strcmp(nom_iter, ancien_nom) == 0)
                            gtk_tree_store_set(projet->list_gtk._1990_groupes.tree_store_etat, &Iter2, 1, nom, -1);
                        free(nom_iter);
                    }
                    while (gtk_tree_model_iter_next(model, &Iter2));
                }
            } while (gtk_tree_model_iter_next(model, &Iter1));
        }
        
        // On modifie les lignes dans le treeview dispo
        model = gtk_tree_view_get_model(projet->list_gtk._1990_groupes.tree_view_dispo);
        if (gtk_tree_model_get_iter_first(model, &Iter1))
        {
            do
            {
                char    *nom_iter;
                
                gtk_tree_model_get(model, &Iter1, 1, &nom_iter, -1);
                if (strcmp(nom_iter, ancien_nom) == 0)
                    gtk_tree_store_set(projet->list_gtk._1990_groupes.tree_store_dispo, &Iter1, 1, nom, -1);
                free(nom_iter);
            }
            while (gtk_tree_model_iter_next(model, &Iter1));
        }
    }
    free(ancien_nom);
#endif
    
    return TRUE;
}


unsigned int _1990_action_numero_renvoie(Action *action)
/* Description : Renvoie le numéro de l'action.
 * Paramètres : Action *action : l'action dont on souhaite connaître le nom.
 * Valeur renvoyée :
 *   Succès : le nom de l'action.
 *   Échec : -1 :
 *             action == NULL,
 */
{
    BUGMSG(action, -1, gettext("Paramètre %s incorrect.\n"), "action")
    
    return action->numero;
}


Action* _1990_action_numero_cherche(Projet *projet, unsigned int numero)
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
    
    BUGMSG(projet, NULL, gettext("Paramètre %s incorrect.\n"), "projet")
    
    // Trivial
    list_parcours = projet->actions;
    while (list_parcours != NULL)
    {
        Action  *action = (Action*)list_parcours->data;
        
        if (action->numero == numero)
            return action;
        
        list_parcours = g_list_next(list_parcours);
    }
    
    BUGMSG(0, NULL, gettext("Action %u introuvable.\n"), numero)
}


unsigned int _1990_action_type_renvoie(Action *action)
/* Description : Renvoie le type de l'action. La correspondance avec la description est obtenue
 *               avec la fonction _1990_action_bat_txt_type.
 * Paramètres : Action *action : l'action dont on souhaite connaître le type.
 * Valeur renvoyée :
 *   Succès : le type d'action.
 *   Échec : -1 :
 *             action == NULL,
 */
{
    BUGMSG(action, -1, gettext("Paramètre %s incorrect.\n"), "action")
    
    return action->type;
}


gboolean _1990_action_type_change(Projet *projet, Action *action, unsigned int type)
/* Description : Change le type d'une action, y compris psi0, psi1 et psi2.
 * Paramètres : Projet *projet : la variable projet,
 *            : Action *action : l'action à modifier,
 *            : unsigned int type : nouveau type d'action.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL,
 *             action introuvable,
 *             erreur d'allocation mémoire.
 */
{
    Flottant    psi0, psi1, psi2;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet")
    BUGMSG(action, FALSE, gettext("Paramètre %s incorrect.\n"), "action")
    
    if (action->type == type)
        return TRUE;
    
    action->type = type;
    psi0 = common_math_f(_1990_coef_psi0_bat(type, projet->parametres.norme), FLOTTANT_ORDINATEUR);
    BUG(!isnan(common_math_get(psi0)), FALSE)
    psi1 = common_math_f(_1990_coef_psi1_bat(type, projet->parametres.norme), FLOTTANT_ORDINATEUR);
    BUG(!isnan(common_math_get(psi1)), FALSE)
    psi2 = common_math_f(_1990_coef_psi2_bat(type, projet->parametres.norme), FLOTTANT_ORDINATEUR);
    BUG(!isnan(common_math_get(psi2)), FALSE)
    
    action->psi0 = psi0;
    action->psi1 = psi1;
    action->psi2 = psi2;
    
    BUG(EF_calculs_free(projet), FALSE)
    
#ifdef ENABLE_GTK
    if (projet->list_gtk._1990_actions.builder != NULL)
        gtk_widget_queue_resize(GTK_WIDGET(projet->list_gtk._1990_actions.tree_view_actions));
#endif
    
    return TRUE;
}


gboolean _1990_action_charges_vide(Action *action)
/* Description : Renvoie TRUE si la liste des charges est vide.
 * Paramètres : Action *action : l'action.
 * Valeur renvoyée :
 *   Succès : action->charges == NULL
 *   Échec : TRUE :
 *             action == NULL.
 */
{
    BUGMSG(action, TRUE, gettext("Paramètre %s incorrect.\n"), "action")
    
    return action->charges == NULL;
}


GList* _1990_action_charges_renvoie(Action *action)
/* Description : Renvoie la liste des charges.
 * Paramètres : Action *action : l'action.
 * Valeur renvoyée :
 *   Succès : action->charges
 *   Échec : NULL :
 *             action == NULL.
 */
{
    BUGMSG(action, NULL, gettext("Paramètre %s incorrect.\n"), "action")
    
    return action->charges;
}


gboolean _1990_action_charges_change(Action *action, GList *charges)
/* Description : Change la liste des charges.
 * Paramètres : Action *action : l'action,
 *            : GList *charges : la nouvelle liste des charges.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             action == NULL.
 */
{
    BUGMSG(action, FALSE, gettext("Paramètre %s incorrect.\n"), "action")
    
    action->charges = charges;
    
    return TRUE;
}


unsigned int _1990_action_flags_action_predominante_renvoie(Action *action)
/* Description : Renvoie le flag "Action prédominante" de l'action.
 * Paramètres : Action *action : l'action dont on souhaite connaître le flag.
 * Valeur renvoyée :
 *   Succès : le flag de l'action.
 *   Échec : -1 :
 *             action == NULL,
 */
{
    BUGMSG(action, -1, gettext("Paramètre %s incorrect.\n"), "action")
    
    return action->action_predominante;
}


gboolean _1990_action_flags_action_predominante_change(Action *action, unsigned int flag)
/* Description : Change le flag "Action prédominante" de l'action.
 * Paramètres : Action *action : l'action dont on souhaite connaître le flag,
 *            : unsigned int flag : la nouvelle valeur du flag (0 ou 1).
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             action == NULL,
 */
{
    BUGMSG(action, FALSE, gettext("Paramètre %s incorrect.\n"), "action")
    BUGMSG((flag == 0) || (flag == 1), FALSE, gettext("Le paramètre flag est de type strictement boolean et doit valoir soit 0 soit 1.\n"))
    
    action->action_predominante = flag;
    
    return TRUE;
}


Flottant _1990_action_psi_renvoie_0(Action *action)
/* Description : Renvoie le coefficient psi0 de l'action.
 * Paramètres : Action *action : l'action dont on souhaite connaître le coefficient psi0.
 * Valeur renvoyée :
 *   Succès : le coefficient psi0.
 *   Échec : NAN :
 *             action == NULL,
 */
{
    BUGMSG(action, common_math_f(NAN, FLOTTANT_ORDINATEUR), gettext("Paramètre %s incorrect.\n"), "action")
    
    return action->psi0;
}


Flottant _1990_action_psi_renvoie_1(Action *action)
/* Description : Renvoie le coefficient psi1 de l'action.
 * Paramètres : Action *action : l'action dont on souhaite connaître le coefficient psi1.
 * Valeur renvoyée :
 *   Succès : le coefficient psi1.
 *   Échec : NAN :
 *             action == NULL,
 */
{
    BUGMSG(action, common_math_f(NAN, FLOTTANT_ORDINATEUR), gettext("Paramètre %s incorrect.\n"), "action")
    
    return action->psi1;
}


Flottant _1990_action_psi_renvoie_2(Action *action)
/* Description : Renvoie le coefficient psi2 de l'action.
 * Paramètres : Action *action : l'action dont on souhaite connaître le coefficient psi2.
 * Valeur renvoyée :
 *   Succès : le coefficient psi2
 *   Échec : NAN :
 *             action == NULL,
 */
{
    BUGMSG(action, common_math_f(NAN, FLOTTANT_ORDINATEUR), gettext("Paramètre %s incorrect.\n"), "action")
    
    return action->psi2;
}


gboolean _1990_action_psi_change(Projet *projet, Action *action, unsigned int psi_num,
  Flottant psi)
/* Description : Change le coefficient psi d'une action.
 * Paramètres : Projet *projet : la variable projet,
 *            : unsigned int action_num : numéro de l'action à changer,
 *            : unsigned int psi_num : coefficient psi à changer (0, 1 ou 2),
 *            : double psi : nouveau coefficient psi.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL,
 *             action introuvable,
 *             coefficient psi_num incorrect,
 *             erreur d'allocation mémoire.
 */
{
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet")
    BUGMSG(action, FALSE, gettext("Paramètre %s incorrect.\n"), "action")
    BUGMSG((psi_num == 0) || (psi_num == 1) || (psi_num == 2), FALSE, gettext("Le numéro %u du coefficient spi à changer est incorrect.\n"), psi_num)
    
    if (psi_num == 0)
    {
        if (ERREUR_RELATIVE_EGALE(common_math_get(psi), common_math_get(action->psi0)))
            return TRUE;
        
        action->psi0 = psi;
#ifdef ENABLE_GTK
        if (projet->list_gtk._1990_actions.builder != NULL)
            gtk_widget_queue_resize(GTK_WIDGET(projet->list_gtk._1990_actions.tree_view_actions));
#endif
    }
    else if (psi_num == 1)
    {
        if (ERREUR_RELATIVE_EGALE(common_math_get(psi), common_math_get(action->psi1)))
            return TRUE;
        
        action->psi1 = psi;
#ifdef ENABLE_GTK
        if (projet->list_gtk._1990_actions.builder != NULL)
            gtk_widget_queue_resize(GTK_WIDGET(projet->list_gtk._1990_actions.tree_view_actions));
#endif
    }
    else if (psi_num == 2)
    {
        if (ERREUR_RELATIVE_EGALE(common_math_get(psi), common_math_get(action->psi2)))
            return TRUE;
        
        action->psi2 = psi;
#ifdef ENABLE_GTK
        if (projet->list_gtk._1990_actions.builder != NULL)
            gtk_widget_queue_resize(GTK_WIDGET(projet->list_gtk._1990_actions.tree_view_actions));
#endif
    }
    else
        BUGMSG(NULL, FALSE, gettext("Le coefficient phi %d n'existe pas.\n"), psi_num)
    
    BUG(EF_calculs_free(projet), FALSE)
    
    return TRUE;
}


cholmod_sparse* _1990_action_deplacement_complet_renvoie(Action *action)
/* Description : Renvoie la matrice sparse deplacement_complet de l'action.
 * Paramètres : Action *action : l'action dont on souhaite connaître la matrice,
 * Valeur renvoyée :
 *   Succès : la matrice sparse deplacement_complet correspondante.
 *   Échec : NULL :
 *             action == NULL,
 */
{
    BUGMSG(action, NULL, gettext("Paramètre %s incorrect.\n"), "action")
    
    return action->deplacement_complet;
}


gboolean _1990_action_deplacement_complet_change(Action *action, cholmod_sparse *sparse)
/* Description : Modifie la matrice sparse deplacement_complet de l'action.
 * Paramètres : Action *action : l'action dont on souhaite modifier la matrice,
 *            : cholmod_sparse *sparse : la nouvelle matrice.
 * Valeur renvoyée :
 *   Succès : TRUE.
 *   Échec : NULL :
 *             action == NULL,
 *             sparse == NULL.
 */
{
    BUGMSG(action, FALSE, gettext("Paramètre %s incorrect.\n"), "action")
    BUGMSG(sparse, FALSE, gettext("Paramètre %s incorrect.\n"), "sparse")
    
    action->deplacement_complet = sparse;
    
    return TRUE;
}


cholmod_sparse* _1990_action_forces_complet_renvoie(Action *action)
/* Description : Renvoie la matrice sparse forces_complet de l'action.
 * Paramètres : Action *action : l'action dont on souhaite connaître la matrice,
 * Valeur renvoyée :
 *   Succès : la matrice sparse forces_complet correspondante.
 *   Échec : NULL :
 *             action == NULL,
 */
{
    BUGMSG(action, NULL, gettext("Paramètre %s incorrect.\n"), "action")
    
    return action->forces_complet;
}


gboolean _1990_action_forces_complet_change(Action *action, cholmod_sparse *sparse)
/* Description : Modifie la matrice sparse forces_complet de l'action.
 * Paramètres : Action *action : l'action dont on souhaite modifier la matrice,
 *            : cholmod_sparse *sparse : la nouvelle matrice.
 * Valeur renvoyée :
 *   Succès : TRUE.
 *   Échec : NULL :
 *             action == NULL,
 *             sparse == NULL.
 */
{
    BUGMSG(action, FALSE, gettext("Paramètre %s incorrect.\n"), "action")
    BUGMSG(sparse, FALSE, gettext("Paramètre %s incorrect.\n"), "sparse")
    
    action->forces_complet = sparse;
    
    return TRUE;
}


cholmod_sparse* _1990_action_efforts_noeuds_renvoie(Action *action)
/* Description : Renvoie la matrice sparse efforts_noeuds de l'action.
 * Paramètres : Action *action : l'action dont on souhaite connaître la matrice,
 * Valeur renvoyée :
 *   Succès : la matrice sparse efforts_noeuds correspondante.
 *   Échec : NULL :
 *             action == NULL,
 */
{
    BUGMSG(action, NULL, gettext("Paramètre %s incorrect.\n"), "action")
    
    return action->efforts_noeuds;
}


gboolean _1990_action_efforts_noeuds_change(Action *action, cholmod_sparse *sparse)
/* Description : Modifie la matrice sparse efforts_noeuds de l'action.
 * Paramètres : Action *action : l'action dont on souhaite modifier la matrice,
 *            : cholmod_sparse *sparse : la nouvelle matrice.
 * Valeur renvoyée :
 *   Succès : TRUE.
 *   Échec : NULL :
 *             action == NULL,
 *             sparse == NULL.
 */
{
    BUGMSG(action, FALSE, gettext("Paramètre %s incorrect.\n"), "action")
    BUGMSG(sparse, FALSE, gettext("Paramètre %s incorrect.\n"), "sparse")
    
    action->efforts_noeuds = sparse;
    
    return TRUE;
}


#ifdef ENABLE_GTK
GtkTreeIter* _1990_action_Iter_fenetre_renvoie(Action *action)
/* Description : Renvoie la variable Iter_fenetre.
 * Paramètres : Action *action : l'action dont on souhaite connaître la variable Iter_fenetre.
 * Valeur renvoyée :
 *   Succès : Iter_fenetre.
 *   Échec : NULL :
 *             action == NULL,
 */
{
    BUGMSG(action, NULL, gettext("Paramètre %s incorrect.\n"), "action")
    
    return &action->Iter_fenetre;
}
#endif


Fonction* _1990_action_fonctions_efforts_renvoie(Action *action, int ligne, int barre)
/* Description : Renvoie la fonction Effort dans la barre.
 * Paramètres : Action *action : l'action dont on souhaite connaître les efforts,
 *            : int ligne : ligne de l'effort (N : 0, Ty : 1, Tz : 2, Mx : 3, My : 4, Mz : 5),
 *            : int barre : ce n'est pas le numéro de la barre mais la position de la barre
 *                          dans la liste des barres.
 * Valeur renvoyée :
 *   Succès : la fonction Effort correspondante.
 *   Échec : NULL :
 *             action == NULL,
 */
{
    BUGMSG(action, NULL, gettext("Paramètre %s incorrect.\n"), "action")
    BUGMSG((0 <= ligne) && (ligne <= 5), NULL, gettext("Paramètre %s incorrect.\n"), "ligne")
    
    return action->fonctions_efforts[ligne][barre];
}


Fonction* _1990_action_fonctions_rotation_renvoie(Action *action, int ligne, int barre)
/* Description : Renvoie la fonction Rotation dans la barre.
 * Paramètres : Action *action : l'action dont on souhaite connaître les rotations,
 *            : int ligne : ligne de l'effort (rx : 0, ry : 1, rz : 2),
 *            : int barre : ce n'est pas le numéro de la barre mais la position de la barre
 *                          dans la liste des barres.
 * Valeur renvoyée :
 *   Succès : la fonction Effort correspondante.
 *   Échec : NULL :
 *             action == NULL,
 */
{
    BUGMSG(action, NULL, gettext("Paramètre %s incorrect.\n"), "action")
    BUGMSG((0 <= ligne) && (ligne <= 2), NULL, gettext("Paramètre %s incorrect.\n"), "ligne")
    
    return action->fonctions_rotation[ligne][barre];
}


Fonction* _1990_action_fonctions_deformation_renvoie(Action *action, int ligne, int barre)
/* Description : Renvoie la fonction Déformation dans la barre.
 * Paramètres : Action *action : l'action dont on souhaite connaître les déformations,
 *            : int ligne : ligne de l'effort (ux : 0, uy : 1, uz : 2),
 *            : int barre : ce n'est pas le numéro de la barre mais la position de la barre
 *                          dans la liste des barres.
 * Valeur renvoyée :
 *   Succès : la fonction Effort correspondante.
 *   Échec : NULL :
 *             action == NULL,
 */
{
    BUGMSG(action, NULL, gettext("Paramètre %s incorrect.\n"), "action")
    BUGMSG((0 <= ligne) && (ligne <= 2), NULL, gettext("Paramètre %s incorrect.\n"), "ligne")
    
    return action->fonctions_deformation[ligne][barre];
}


gboolean _1990_action_fonction_init(Projet *projet, Action *action)
/* Description : Initialise les fonctions décrivant les sollicitations, les rotations ou les
 *               déplacements des barres. Cette fonction doit être appelée lorsque toutes les
 *               barres ont été modélisées. En effet, il est nécessaire de connaître leur
 *               nombre afin de stocker dans un tableau dynamique unique les fonctions.
 *               L'initialisation des fonctions consiste à définir un nombre de tronçon à 0 et
 *               les données à NULL.
 * Paramètres : Projet *projet : la variable projet,
 *            : Action *action : pointeur vers l'action.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL,
 *             action == NULL,
 *             en cas d'erreur d'allocation mémoire.
 */
{
    unsigned int        i, j;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet")
    BUGMSG(action, FALSE, gettext("Paramètre %s incorrect.\n"), "action")
    
    // Trivial
    for (i=0;i<6;i++)
    {
        BUGMSG(action->fonctions_efforts[i] = (Fonction**)malloc(sizeof(Fonction*)*g_list_length(projet->modele.barres)), FALSE, gettext("Erreur d'allocation mémoire.\n"))
        for (j=0;j<g_list_length(projet->modele.barres);j++)
        {
            BUGMSG(action->fonctions_efforts[i][j] = (Fonction*)malloc(sizeof(Fonction)), FALSE, gettext("Erreur d'allocation mémoire.\n"))
            action->fonctions_efforts[i][j]->nb_troncons = 0;
            action->fonctions_efforts[i][j]->troncons = NULL;
        }
    }
    
    for (i=0;i<3;i++)
    {
        BUGMSG(action->fonctions_deformation[i] = (Fonction**)malloc(sizeof(Fonction*)*g_list_length(projet->modele.barres)), FALSE, gettext("Erreur d'allocation mémoire.\n"))
        for (j=0;j<g_list_length(projet->modele.barres);j++)
        {
            BUGMSG(action->fonctions_deformation[i][j] = (Fonction*)malloc(sizeof(Fonction)), FALSE, gettext("Erreur d'allocation mémoire.\n"))
            action->fonctions_deformation[i][j]->nb_troncons = 0;
            action->fonctions_deformation[i][j]->troncons = NULL;
        }
        
        BUGMSG(action->fonctions_rotation[i] = (Fonction**)malloc(sizeof(Fonction*)*g_list_length(projet->modele.barres)), FALSE, gettext("Erreur d'allocation mémoire.\n"))
        for (j=0;j<g_list_length(projet->modele.barres);j++)
        {
            BUGMSG(action->fonctions_rotation[i][j] = (Fonction*)malloc(sizeof(Fonction)), FALSE, gettext("Erreur d'allocation mémoire.\n"))
            action->fonctions_rotation[i][j]->nb_troncons = 0;
            action->fonctions_rotation[i][j]->troncons = NULL;
        }
    }
    
    return TRUE;
}


gboolean _1990_action_fonction_free(Projet *projet, Action *action)
/* Description : Libère les fonctions de toutes les barres de l'action souhaitée.
 * Paramètres : Projet *projet : la variable projet,
 *            : Action *action : pointeur vers l'action.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL,
 *             action == NULL,
 *             projet->modele.barres == NULL.
 */
{
    unsigned int        i, j;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet")
    BUGMSG(action, FALSE, gettext("Paramètre %s incorrect.\n"), "action")
    
    // Trivial
    for (i=0;i<6;i++)
    {
        if (action->fonctions_efforts[i] != NULL)
        {
            for (j=0;j<g_list_length(projet->modele.barres);j++)
            {
                free(action->fonctions_efforts[i][j]->troncons);
                free(action->fonctions_efforts[i][j]);
            }
            free(action->fonctions_efforts[i]);
            action->fonctions_efforts[i] = NULL;
        }
    }
    
    for (i=0;i<3;i++)
    {
        if (action->fonctions_deformation[i] != NULL)
        {
            for (j=0;j<g_list_length(projet->modele.barres);j++)
            {
                free(action->fonctions_deformation[i][j]->troncons);
                free(action->fonctions_deformation[i][j]);
            }
            free(action->fonctions_deformation[i]);
            action->fonctions_deformation[i] = NULL;
        }
        
        if (action->fonctions_rotation[i] != NULL)
        {
            for (j=0;j<g_list_length(projet->modele.barres);j++)
            {
                free(action->fonctions_rotation[i][j]->troncons);
                free(action->fonctions_rotation[i][j]);
            }
            free(action->fonctions_rotation[i]);
            action->fonctions_rotation[i] = NULL;
        }
    }
    
    return TRUE;
}


gboolean _1990_action_affiche_tout(Projet *projet)
/* Description : Affiche dans l'entrée standard les actions existantes.
 * Paramètres : Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL.
 */
{
    GList   *list_parcours;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet")
    if (projet->actions == NULL)
    {
        printf(gettext("Aucune action existante.\n"));
        return TRUE;
    }
    
    // Trivial
    list_parcours = projet->actions;
    do
    {
        Action      *action = list_parcours->data;
        
        printf(gettext("Action n° %u, description '%s', type n°%d\n"), action->numero, action->nom, action->type);
        
        list_parcours = g_list_next(list_parcours);
    }
    while (list_parcours != NULL);
    
    return TRUE;
}


gboolean _1990_action_affiche_resultats(Projet *projet, unsigned int num_action)
/* Description : Affiche tous les résultats d'une action.
 * Paramètres : Projet *projet : la variable projet,
 *            : int num_action : numéro de l'action.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL,
 *             action introuvable,
 *             modèle de calculs vide (absence de barres),
 */
{
    Action          *action_en_cours;
    unsigned int    i;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet")
    BUG(action_en_cours = _1990_action_numero_cherche(projet, num_action), FALSE)
    if (projet->modele.barres == NULL)
    {
        printf(gettext("Aucune barre existante.\n"));
        return TRUE;
    }
    
    // Affichage des efforts aux noeuds et des réactions d'appuis
    printf("Effort aux noeuds & Réactions d'appuis :\n");
    common_math_arrondi_sparse(action_en_cours->efforts_noeuds);
    cholmod_write_sparse(stdout, action_en_cours->efforts_noeuds, NULL, NULL, projet->calculs.c);
    // Affichage des déplacements des noeuds
    printf("\nDéplacements :\n");
    common_math_arrondi_sparse(action_en_cours->deplacement_complet);
    cholmod_write_sparse(stdout, action_en_cours->deplacement_complet, NULL, NULL, projet->calculs.c);
    // Pour chaque barre
    for (i=0;i<g_list_length(projet->modele.barres);i++)
    {
    //     Affichage de la courbe des sollicitations vis-à-vis de l'effort normal
        printf("Barre n°%d, Effort normal\n", i);
        BUG(common_fonction_affiche(action_en_cours->fonctions_efforts[0][i]), FALSE)
    //     Affichage de la courbe des sollicitations vis-à-vis de l'effort trnt selon Y
        printf("Barre n°%d, Effort tranchant Y\n", i);
        BUG(common_fonction_affiche(action_en_cours->fonctions_efforts[1][i]), FALSE)
    //     Affichage de la courbe des sollicitations vis-à-vis de l'effort trnt selon Z
        printf("Barre n°%d, Effort tranchant Z\n", i);
        BUG(common_fonction_affiche(action_en_cours->fonctions_efforts[2][i]), FALSE)
    //     Affichage de la courbe des sollicitations vis-à-vis du moment de tn
        printf("Barre n°%d, Moment de torsion\n", i);
        BUG(common_fonction_affiche(action_en_cours->fonctions_efforts[3][i]), FALSE)
    //     Affichage de la courbe des sollicitations vis-à-vis du moment flécnt selon Y
        printf("Barre n°%d, Moment de flexion Y\n", i);
        BUG(common_fonction_affiche(action_en_cours->fonctions_efforts[4][i]), FALSE)
    //     Affichage de la courbe des sollicitations vis-à-vis du moment flécnt selon Z
        printf("Barre n°%d, Moment de flexion Z\n", i);
        BUG(common_fonction_affiche(action_en_cours->fonctions_efforts[5][i]), FALSE)
    }
    for (i=0;i<g_list_length(projet->modele.barres);i++)
    {
    //     Affichage de la courbe de déformation selon l'axe X
        printf("Barre n°%d, Déformation en X\n", i);
        BUG(common_fonction_affiche(action_en_cours->fonctions_deformation[0][i]), FALSE)
    //     Affichage de la courbe de déformation selon l'axe Y
        printf("Barre n°%d, Déformation en Y\n", i);
        BUG(common_fonction_affiche(action_en_cours->fonctions_deformation[1][i]), FALSE)
    //     Affichage de la courbe de déformation selon l'axe Z
        printf("Barre n°%d, Déformation en Z\n", i);
        BUG(common_fonction_affiche(action_en_cours->fonctions_deformation[2][i]), FALSE)
    //     Affichage de la courbe de rotation selon l'axe X
        printf("Barre n°%d, Rotation en X\n", i);
        BUG(common_fonction_affiche(action_en_cours->fonctions_rotation[0][i]), FALSE)
    //     Affichage de la courbe de rotation selon l'axe Y
        printf("Barre n°%d, Rotation en Y\n", i);
        BUG(common_fonction_affiche(action_en_cours->fonctions_rotation[1][i]), FALSE)
    //     Affichage de la courbe de rotation selon l'axe Z
        printf("Barre n°%d, Rotation en Z\n", i);
        BUG(common_fonction_affiche(action_en_cours->fonctions_rotation[2][i]), FALSE)
    }
    // FinPour
    
    return TRUE;
}


Action *_1990_action_ponderation_resultat(GList* ponderation, Projet *projet)
/* Description : Crée une fausse action sur la base d'une combinaison. L'objectif de cette
 *               fonction est uniquement de regrouper les résultats pondérés de chaque action.
 * Paramètres : GList *ponderation : pondération sur laquelle sera créée l'action,
 *              Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : pointeur vers l'action équivalente créée.
 *   Échec : NULL :
 *             projet == NULL,
 *             ponderation == NULL,
 *             une des actions de pondération est introuvable.
 */
{
    GList   *list_parcours;
    Action  *action;
    double  *x, *y;

    
    BUGMSG(projet, NULL, gettext("Paramètre %s incorrect.\n"), "projet")
    
    // Initialisation de l'action
    BUGMSG(action = malloc(sizeof(Action)), NULL, gettext("Erreur d'allocation mémoire.\n"))
    BUGMSG(action->efforts_noeuds = malloc(sizeof(cholmod_sparse)), NULL, gettext("Erreur d'allocation mémoire.\n"))
    BUGMSG(action->efforts_noeuds->x = malloc(sizeof(double)*g_list_length(projet->modele.noeuds)*6), NULL, gettext("Erreur d'allocation mémoire.\n"))
    memset(action->efforts_noeuds->x, 0, sizeof(double)*g_list_length(projet->modele.noeuds)*6);
    BUGMSG(action->deplacement_complet = malloc(sizeof(cholmod_sparse)), NULL, gettext("Erreur d'allocation mémoire.\n"))
    BUGMSG(action->deplacement_complet->x = malloc(sizeof(double)*g_list_length(projet->modele.noeuds)*6), NULL, gettext("Erreur d'allocation mémoire.\n"))
    memset(action->deplacement_complet->x, 0, sizeof(double)*g_list_length(projet->modele.noeuds)*6);
    BUG(_1990_action_fonction_init(projet, action), NULL)
    x = action->efforts_noeuds->x;
    y = action->deplacement_complet->x;
    
    // Remplissage de la variable action.
    list_parcours = ponderation;
    while (list_parcours != NULL)
    {
        Ponderation     *element = list_parcours->data;
        double          *x2 = element->action->efforts_noeuds->x;
        double          *y2 = element->action->deplacement_complet->x;
        double          mult = element->ponderation*(element->psi == 0 ? common_math_get(element->action->psi0) : element->psi == 1 ? common_math_get(element->action->psi1) : element->psi == 2 ? common_math_get(element->action->psi2) : 1.);
        unsigned int    i;
        
        for (i=0;i<g_list_length(projet->modele.noeuds)*6;i++)
        {
            x[i] = x[i] + mult*x2[i];
            y[i] = y[i] + mult*y2[i];
        }
        
        for (i=0;i<g_list_length(projet->modele.barres);i++)
        {
            BUG(common_fonction_ajout_fonction(action->fonctions_efforts[0][i], element->action->fonctions_efforts[0][i], mult), NULL)
            BUG(common_fonction_ajout_fonction(action->fonctions_efforts[1][i], element->action->fonctions_efforts[1][i], mult), NULL)
            BUG(common_fonction_ajout_fonction(action->fonctions_efforts[2][i], element->action->fonctions_efforts[2][i], mult), NULL)
            BUG(common_fonction_ajout_fonction(action->fonctions_efforts[3][i], element->action->fonctions_efforts[3][i], mult), NULL)
            BUG(common_fonction_ajout_fonction(action->fonctions_efforts[4][i], element->action->fonctions_efforts[4][i], mult), NULL)
            BUG(common_fonction_ajout_fonction(action->fonctions_efforts[5][i], element->action->fonctions_efforts[5][i], mult), NULL)
            BUG(common_fonction_ajout_fonction(action->fonctions_deformation[0][i], element->action->fonctions_deformation[0][i], mult), NULL)
            BUG(common_fonction_ajout_fonction(action->fonctions_deformation[1][i], element->action->fonctions_deformation[1][i], mult), NULL)
            BUG(common_fonction_ajout_fonction(action->fonctions_deformation[2][i], element->action->fonctions_deformation[2][i], mult), NULL)
            BUG(common_fonction_ajout_fonction(action->fonctions_rotation[0][i], element->action->fonctions_rotation[0][i], mult), NULL)
            BUG(common_fonction_ajout_fonction(action->fonctions_rotation[1][i], element->action->fonctions_rotation[1][i], mult), NULL)
            BUG(common_fonction_ajout_fonction(action->fonctions_rotation[2][i], element->action->fonctions_rotation[2][i], mult), NULL)
        }
        
        list_parcours = g_list_next(list_parcours);
    }
    
    return action;
}


gboolean _1990_action_ponderation_resultat_free_calculs(Action *action)
/* Description : Libère les résultats de l'action souhaitée crée par
 *               _1990_action_ponderation_resultat.
 * Paramètres : Action *action : l'action à libérer.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             action == NULL.
 */
{
    BUGMSG(action, FALSE, gettext("Paramètre %s incorrect.\n"), "action")
    
    free(action->deplacement_complet->x);
    free(action->deplacement_complet);
    free(action->efforts_noeuds->x);
    free(action->efforts_noeuds);
    
    return TRUE;
}


gboolean _1990_action_free_calculs(Projet *projet, Action *action)
/* Description : Libère les résultats de l'action souhaitée.
 * Paramètres : Projet *projet : la variable projet,
 *            : Action *action : l'action à libérer.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL,
 *             action == NULL.
 */
{
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet")
    BUGMSG(action, FALSE, gettext("Paramètre %s incorrect.\n"), "action")
    
    if (action->deplacement_complet != NULL)
    {
        cholmod_free_sparse(&action->deplacement_complet, projet->calculs.c);
        action->deplacement_complet = NULL;
    }
    
    if (action->forces_complet != NULL)
    {
        cholmod_free_sparse(&action->forces_complet, projet->calculs.c);
        action->forces_complet = NULL;
    }
    
    if (action->efforts_noeuds != NULL)
    {
        cholmod_free_sparse(&action->efforts_noeuds, projet->calculs.c);
        action->efforts_noeuds = NULL;
    }
    
    if (action->fonctions_efforts[0] != NULL)
        BUG(_1990_action_fonction_free(projet, action), FALSE)
    
    return TRUE;
}


gboolean _1990_action_free_num(Projet *projet, unsigned int num)
/* Description : Libère l'action souhaitée et décrémente de 1 les actions dons le numéro est
 *               supérieur.
 * Paramètres : Projet *projet : la variable projet,
 *            : unsigned int num : le numéro de l'action à supprimer.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL,
 *             action introuvable,
 *             erreur lors de la libération d'une charge de l'action à supprimer,
 *             erreur d'allocation métmoire.
 */
{
    GList       *list_parcours;
#ifdef ENABLE_GTK
    GtkTreeIter Iter;
#endif
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet")
    BUG(_1990_action_numero_cherche(projet, num), FALSE)
    
    // On enlève l'action de la liste des actions
    list_parcours = g_list_last(projet->actions);
    do
    {
        Action      *action = list_parcours->data;
        
        list_parcours = g_list_previous(list_parcours);
        if (action->numero == num)
        {
            projet->actions = g_list_remove(projet->actions, action);
            free(action->nom);
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
                        BUG(EF_charge_noeud_free(charge2), FALSE)
                        break;
                    }
                    case CHARGE_BARRE_PONCTUELLE :
                    {
                        BUG(EF_charge_barre_ponctuelle_free(charge), FALSE)
                        break;
                    }
                    case CHARGE_BARRE_REPARTIE_UNIFORME :
                    {
                        Charge_Barre_Repartie_Uniforme *charge2 = (Charge_Barre_Repartie_Uniforme *)charge;
                        BUG(EF_charge_barre_repartie_uniforme_free(charge2), FALSE)
                        break;
                    }
                    default :
                    {
                        BUGMSG(0, FALSE, gettext("Type de charge %d inconnu.\n"), charge->type)
                        break;
                    }
                }
            }
            if (action->deplacement_complet != NULL)
                cholmod_free_sparse(&action->deplacement_complet, projet->calculs.c);
            if (action->forces_complet != NULL)
                cholmod_free_sparse(&action->forces_complet, projet->calculs.c);
            if (action->efforts_noeuds != NULL)
                cholmod_free_sparse(&action->efforts_noeuds, projet->calculs.c);
            
            if (action->fonctions_efforts[0] != NULL)
                BUG(_1990_action_fonction_free(projet, action), FALSE)
            
#ifdef ENABLE_GTK
            if (projet->list_gtk._1990_actions.builder != NULL)
            {
                if (gtk_tree_selection_iter_is_selected(projet->list_gtk._1990_actions.tree_select_actions, &action->Iter_fenetre))
                    gtk_tree_store_clear(projet->list_gtk._1990_actions.tree_store_charges);
                gtk_tree_store_remove(projet->list_gtk._1990_actions.tree_store_actions, &action->Iter_fenetre);
            }
            
            gtk_list_store_remove(projet->list_gtk._1990_actions.list_actions, &action->Iter_liste);
#endif
            
            free(action);
        }
        else if (action->numero > num)
            action->numero--;
        
    } while (list_parcours != NULL);

#ifdef ENABLE_GTK
    if (projet->list_gtk._1990_actions.builder != NULL)
        gtk_widget_queue_resize(GTK_WIDGET(projet->list_gtk._1990_actions.tree_view_actions));
#endif
    
    // On enlève l'action dans la liste des groupes de niveau 0 tout en modifiant le numéro
    // des éléments dans le treeview.
    list_parcours = projet->niveaux_groupes;
    if (list_parcours != NULL)
    {
        Niveau_Groupe   *niveau_groupe = list_parcours->data;
        GList           *list_groupes = niveau_groupe->groupes;
        
        while (list_groupes != NULL)
        {
            Groupe      *groupe = list_groupes->data;
            GList       *list_elements = groupe->elements;
            gboolean    delete = FALSE;
            
            while (list_elements != NULL)
            {
                Element     *element_en_cours = list_elements->data;
                
                if (element_en_cours->numero == num)
                    delete = TRUE;
                else if (element_en_cours->numero > num)
                {
                    element_en_cours->numero--;
#ifdef ENABLE_GTK
                    if (projet->list_gtk._1990_groupes.builder != NULL)
                        gtk_tree_store_set(GTK_TREE_STORE(projet->list_gtk._1990_groupes.tree_store_etat), &element_en_cours->Iter, 0, element_en_cours->numero, -1);
#endif
                }
                
                list_elements = g_list_next(list_elements);
            }
            
            if (delete)
                BUG(_1990_groupe_free_element(projet, 0, groupe->numero, num), FALSE)
            
            list_groupes = g_list_next(list_groupes);
        }
    }
    
    // Il faut aussi parcourir le treeview des éléments inutilisés pour l'enlever au cas où et
    // aussi pour mettre à jour les numéros.
#ifdef ENABLE_GTK
    if ((projet->list_gtk._1990_groupes.builder != NULL) && (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(projet->list_gtk._1990_groupes.tree_store_dispo), &Iter)))
    {
        gboolean encore = TRUE;
        do
        {
            unsigned int    numero;
            GtkTreeIter     Iter_en_cours;
            
            Iter_en_cours = Iter;
            
            // On récupère tout de suite le suivant car en cas de suppression, on perd le fil
            // conducteur.
            encore = gtk_tree_model_iter_next(GTK_TREE_MODEL(projet->list_gtk._1990_groupes.tree_store_dispo), &Iter);
            
            gtk_tree_model_get(GTK_TREE_MODEL(projet->list_gtk._1990_groupes.tree_store_dispo), &Iter_en_cours, 0, &numero, -1);
            if (numero == num)
                gtk_tree_store_remove(GTK_TREE_STORE(projet->list_gtk._1990_groupes.tree_store_dispo), &Iter_en_cours);
            else if (numero > num)
                gtk_tree_store_set(GTK_TREE_STORE(projet->list_gtk._1990_groupes.tree_store_dispo), &Iter_en_cours, 0, numero-1, -1);
            
        } while (encore);
    }
#endif
    
    BUG(EF_calculs_free(projet), FALSE)
    
    return TRUE;
}


gboolean _1990_action_free(Projet *projet)
/* Description : Libère l'ensemble des actions existantes.
 * Paramètres : Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL,
 *             erreur lors de la libération d'une charge de l'action à supprimer.
 */
{
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet")
    
    // Trivial
    while (projet->actions != NULL)
    {
        Action      *action = projet->actions->data;
        
        projet->actions = g_list_delete_link(projet->actions, projet->actions);
        
        free(action->nom);
        while (action->charges != NULL)
        {
            Charge_Barre_Ponctuelle *charge = action->charges->data;
            
            action->charges = g_list_delete_link(action->charges, action->charges);
            
            switch (charge->type)
            {
                case CHARGE_NOEUD :
                {
                    BUG(EF_charge_noeud_free((Charge_Noeud*)charge), FALSE)
                    break;
                }
                case CHARGE_BARRE_PONCTUELLE :
                {
                    BUG(EF_charge_barre_ponctuelle_free(charge), FALSE)
                    break;
                }
                case CHARGE_BARRE_REPARTIE_UNIFORME :
                {
                    BUG(EF_charge_barre_repartie_uniforme_free((Charge_Barre_Repartie_Uniforme*)charge), FALSE)
                    break;
                }
                default :
                {
                    BUGMSG(0, FALSE, gettext("Type de charge %d inconnu.\n"), charge->type)
                    break;
                }
            }
        }
        if (action->deplacement_complet != NULL)
            cholmod_free_sparse(&action->deplacement_complet, projet->calculs.c);
        if (action->forces_complet != NULL)
            cholmod_free_sparse(&action->forces_complet, projet->calculs.c);
        if (action->efforts_noeuds != NULL)
            cholmod_free_sparse(&action->efforts_noeuds, projet->calculs.c);
        
        if (action->fonctions_efforts[0] != NULL)
            BUG(_1990_action_fonction_free(projet, action), FALSE)
        
        free(action);
    }
    
#ifdef ENABLE_GTK
    if (projet->list_gtk._1990_actions.builder != NULL)
        gtk_tree_store_clear(projet->list_gtk._1990_actions.tree_store_charges);
    g_object_ref_sink(projet->list_gtk._1990_actions.menu_type_list_action);
    g_object_unref(projet->list_gtk._1990_actions.menu_type_list_action);
    g_object_unref(projet->list_gtk._1990_actions.menu_type_list_charge);
    g_object_unref(projet->list_gtk._1990_actions.list_actions);
    gtk_list_store_clear(projet->list_gtk._1990_actions.choix_type_action);
    g_object_unref(projet->list_gtk._1990_actions.choix_type_action);
    g_list_free(projet->list_gtk._1990_actions.menu_list_widget_charge);
    projet->list_gtk._1990_actions.menu_list_widget_charge = NULL;
    g_list_free(projet->list_gtk._1990_actions.menu_list_widget_action);
    projet->list_gtk._1990_actions.menu_list_widget_action = NULL;
#endif
    
    return TRUE;
}
