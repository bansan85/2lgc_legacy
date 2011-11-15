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

#ifndef __COMMON_PROJET_H
#define __COMMON_PROJET_H

#include "config.h"
#include <list.h>
#include <cholmod.h>
#include <umfpack.h>

#ifdef ENABLE_GTK
#include <gtk/gtk.h>
#endif

typedef enum __Type_Pays
{               // Spécifie le pays et l'annexe nationnale à utiliser :
    PAYS_EU,    // Norme européenne sans application des annexes nationales
    PAYS_FR     // Annexe nationale française
} Type_Pays;


typedef struct __CombinaisonsEL
{                           // Spécifie la méthode des combinaisons (E0,A1.3)
    int     flags;          // bit 1      : ELU_EQU : méthode note 1 si le bit 1 = 0
                            //            : ELU_EQU : méthode note 2 si le bit 1 = 1
                            // bit 2 et 3 : ELU_GEO/STR : 00 méthode approche 1
                            //            : ELU_GEO/STR : 01 méthode approche 2
                            //            : ELU_GEO/STR : 10 méthode approche 3
                            // bit 4      : ELU_ACC : 0 si utilisation de psi1,1
                            //            : ELU_ACC : 1 si utilisation de psi2,1
    LIST        *elu_equ;   // Liste des combinaisons selon l'ELU EQU
    LIST        *elu_str;   // ..
    LIST        *elu_geo;   //
    LIST        *elu_fat;   //
    LIST        *elu_acc;   //
    LIST        *elu_sis;   //
    LIST        *els_car;   //
    LIST        *els_freq;  //
    LIST        *els_perm;  //
} CombinaisonsEL;


#ifdef ENABLE_GTK
typedef struct __Comp_Gtk
{
    // Fenêtre
    GtkWidget   *window;
    GtkWidget   *main_table;
    
    // Menu
    GtkWidget   *menu;
    GtkWidget   *menu_fichier;
    GtkWidget   *menu_fichier_list;
    GtkWidget   *menu_fichier_quitter;
    GtkWidget   *menu_charges;
    GtkWidget   *menu_charges_list;
    GtkWidget   *menu_charges_actions;
    GtkWidget   *menu_charges_groupes;
} Comp_Gtk;


typedef struct __List_Gtk
{                       // Contient toutes les données pour l'interface graphique GTK+3
    void        *_1990_actions;     // pour l'Eurocode 0
    void        *_1990_groupes;     // pour l'Eurocode 0
    void        *m3d;       // pour l'affichage graphique de la structure
    Comp_Gtk    comp;      // tous les composants grahpiques
} List_Gtk;
#endif


typedef enum __Type_Element // La liste des différents éléments de type de barres gérés par le
{                           // module élément fini.
    BETON_ELEMENT_POTEAU,
    BETON_ELEMENT_POUTRE
} Type_Element;


typedef struct __Beton_Donnees
{                               // Liste des sections, matériaux et barres en béton.
    LIST            *sections;
    LIST            *materiaux;
    LIST            *barres;
} Beton_Donnees;


typedef struct __EF
{                               // Contient toutes les données nécessaires pour la réalisation
                                // des calculs aux éléments finis et notamment les variables
                                // utilisées par les librairies cholmod et umfpack :
    cholmod_common      Common; // Paramètres des calculs de la librairie cholmod.
    cholmod_common      *c;     // Pointeur vers Common
                                
    LIST                *noeuds;       // Liste de tous les noeuds de la structure, que ce soit
                                       // les noeuds définis par l'utilisateur ou les noeuds
                                       // créés par la discrétisation des éléments.
    LIST                *appuis;       // Liste des types d'appuis
    LIST                *relachements; // Liste des types de relâchements des barres.
    
    int                 **noeuds_pos_partielle; // Etabli une corrélation entre le degré de 
    int                 **noeuds_pos_complete;  // liberté (x, y, z, rx, ry, rz) d'un noeud
                                                // et sa position dans la matrice de rigidité
    // globale partielle et complète. Par partielle, il faut comprendre la matrice de rigidité
    // globale sans les lignes et les colonnes dont les déplacements sont connus ; cette même
    // matrice qui permet de déterminer le déplacement des noeuds. La matrice de rigidité
    // complète permet, sur la base du calcul des déplacements de déterminer les efforts aux
    // noeuds et d'en déduire les sollicitations le long des barres. La position de la ligne /
    // colonne dans la matrice se déterminera par la lecture de noeuds_pos_partielle[10][1]
    // pour, par exemple, la position du noeud dont le numéro est le 10 et comme degré de
    // liberté y. Si la valeur renvoyée est -1, cela signifie que le déplacement ou la rotation
    // est bloquée et que le degré de liberté ne figure pas dans la matrice de rigidité
    // partielle.
    
    cholmod_triplet     *triplet_rigidite_partielle; // Liste temporaire avant transformation
    cholmod_triplet     *triplet_rigidite_complete;  // en matrice sparse. Les éléments sont 
                                                     // ajoutés au fur et à mesure que la 
    // rigidité des barres est ajoutée dans la matrice de rigidité.
    
    unsigned int        triplet_rigidite_partielle_en_cours; // Numéro du prochain triplet qui
    unsigned int        triplet_rigidite_complete_en_cours;  // est à compléter.
    
    cholmod_sparse      *rigidite_matrice_partielle; // Matrice de rigidité globale ne
                                                     // contenant pas les lignes et colonnes
                                                     // dont les déplacements sont connus
    cholmod_sparse      *rigidite_matrice_complete;  // Matrice de rigidité complète.
    
    void                *numeric;            // Matrice partielle factorisée, utilisée dans
    long                *ap, *ai;            // tous les calculs lors de la résolution de
    double              *ax;                 // chaque cas de charges.
    double              residu;              // Erreur non relative des réactions d'appuis.
} EF;


typedef struct __Projet
{
    LIST            *actions;           // Liste des actions contenant chacune des charges
    LIST            *niveaux_groupes;   // Compatibilités entres actions
    CombinaisonsEL  combinaisons;       // Combinaisons conformes aux Eurocodes
    Type_Pays       pays;               // Pays de calculs
    List_Gtk        list_gtk;           // Informations nécessaires pour l'interface graphique
    EF              ef_donnees;         // Données communes à tous les éléments finis
    Beton_Donnees   beton;              // Données spécifiques au béton
} Projet;


Projet *projet_init(Type_Pays pays);
int projet_init_graphique(Projet *projet);
void projet_free(Projet *projet);

#endif
