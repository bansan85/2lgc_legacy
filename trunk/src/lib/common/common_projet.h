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
#include <cholmod.h>
#include <umfpack.h>

#include <glib.h>

#ifdef ENABLE_GTK
#include <gtk/gtk.h>

typedef struct __List_Gtk_1990_Groupes
{
    GtkBuilder      *builder;
    GtkBuilder      *builder_options;
     
    GtkWidget       *window_groupe;
    GtkTreeStore    *tree_store_etat;
    GtkTreeStore    *tree_store_dispo;
    
    GtkTreeSelection *tree_select_etat;
    GtkTreeView     *tree_view_etat;
    GtkTreeSelection *tree_select_dispo;
    GtkWidget       *spin_button_niveau;
    GtkTreeView     *tree_view_dispo;
    
    GtkWidget       *drag_from;
    GtkToolItem     *item_groupe_and;
    GtkToolItem     *item_groupe_or;
    GtkToolItem     *item_groupe_xor;
    GtkToolItem     *item_ajout_dispo;
    GtkToolItem     *item_ajout_tout_dispo;
    GtkToolItem     *item_groupe_suppr;
} List_Gtk_1990_Groupes;


typedef struct __List_Gtk_1990_Actions
{
    GtkBuilder      *builder;
    GtkWidget       *window;
    
    GtkTreeStore    *tree_store_actions;
    GtkTreeSelection *tree_select_actions;
    GtkTreeView     *tree_view_actions;
    GtkListStore    *choix_type_action;
    
    GtkWidget       *menu_type_list_action;
    GList           *menu_list_widget_action;
    
    GtkTreeStore    *tree_store_charges;
    GtkTreeSelection *tree_select_charges;
    GtkTreeView     *tree_view_charges;
    GtkWidget       *menu_type_list_charge;
    GList           *menu_list_widget_charge;
    
    GtkListStore    *list_actions_pour_combobox;
} List_Gtk_1990_Actions;


typedef struct __List_Gtk_EF_Charge_Noeud
{
    GtkBuilder      *builder;
    GtkWidget       *window;
    GtkWidget       *combobox_charge;
    
    unsigned int    action;
    unsigned int    charge;
} List_Gtk_EF_Charge_Noeud;

typedef struct __List_Gtk_EF_Charge_Barre_Ponctuelle
{
    GtkBuilder      *builder;
    GtkWidget       *window;
    unsigned int    action;
    unsigned int    charge;
    GtkComboBox     *combobox_charge;
} List_Gtk_EF_Charge_Barre_Ponctuelle;

typedef struct __List_Gtk_EF_Charge_Barre_Repartie_Uniforme
{
    GtkBuilder      *builder;
    GtkWidget       *window;
    
    unsigned int    action;
    unsigned int    charge;
    
    GtkWidget       *combobox_charge;
    GtkWidget       *check_button_repere_local;
    GtkWidget       *check_button_repere_global;
    GtkWidget       *check_button_projection;
} List_Gtk_EF_Charge_Barre_Repartie_Uniforme;

typedef struct __List_Gtk_EF_Noeud
{
    GtkBuilder      *builder;
    
    GtkListStore    *liste_appuis;
    
    GtkWidget       *window;
    GtkWidget       *notebook;
    GtkTreeStore    *tree_store_libre;
    GtkTreeStore    *tree_store_barre;
} List_Gtk_EF_Noeud;


typedef struct __List_Gtk_EF_Barres
{
    GtkBuilder      *builder;
    GtkWidget       *window;
    
    GtkBuilder      *builder_add;
    GtkWidget       *window_add;
    
    GtkListStore    *liste_relachements;
    GtkListStore    *liste_types;
    GtkListStore    *liste_sections;
    GtkListStore    *liste_materiaux;
} List_Gtk_EF_Barres;

#endif

typedef enum __Type_Element // La liste des différents éléments de type de barres gérés par le
{                           // module élément fini.
    BETON_ELEMENT_POTEAU,
    BETON_ELEMENT_POUTRE
} Type_Element;


typedef enum __Type_Pays
{               // Spécifie le pays et l'annexe nationnale à utiliser :
    PAYS_EU,    // Norme européenne sans application des annexes nationales
    PAYS_FR     // Annexe nationale française
} Type_Pays;


typedef enum __Type_Groupe_Combinaison
{
    GROUPE_COMBINAISON_OR,
    GROUPE_COMBINAISON_XOR,
    GROUPE_COMBINAISON_AND
} Type_Groupe_Combinaison;


typedef enum __Action_Categorie
{
    ACTION_POIDS_PROPRE,
    ACTION_PRECONTRAINTE,
    ACTION_VARIABLE,
    ACTION_ACCIDENTELLE,
    ACTION_SISMIQUE,
    ACTION_EAUX_SOUTERRAINES,
    ACTION_INCONNUE
} Action_Categorie;


typedef struct __EF_Point
{
    double      x;
    double      y;
    double      z;
} EF_Point;


typedef enum __Type_EF_Appui
{
    EF_APPUI_LIBRE,                 // Déplacement libre
    EF_APPUI_BLOQUE                 // Déplacement bloqué
} Type_EF_Appui;


typedef enum __EF_Relachement_Type
{
    EF_RELACHEMENT_BLOQUE,
    EF_RELACHEMENT_LIBRE,
    EF_RELACHEMENT_ELASTIQUE_LINEAIRE
} EF_Relachement_Type;


typedef enum __Type_Beton_Section
{
    BETON_SECTION_RECTANGULAIRE,
    BETON_SECTION_T,
    BETON_SECTION_CARRE,
    BETON_SECTION_CIRCULAIRE
} Type_Beton_Section;


typedef enum __Charge_Type
{
    CHARGE_NOEUD,
    CHARGE_BARRE_PONCTUELLE,
    CHARGE_BARRE_REPARTIE_UNIFORME
} Barre_Charge_Type;


typedef struct __EF_Appui
{
    unsigned int    numero;         // Numéro de l'appui
    char            *nom;
    Type_EF_Appui   ux;              // Degré de liberté de la direction x
    void            *ux_donnees;     // Données complémentaire si nécessaire.
    Type_EF_Appui   uy;
    void            *uy_donnees;
    Type_EF_Appui   uz;
    void            *uz_donnees;
    Type_EF_Appui   rx;             // Degré de liberté de la rotation autour de l'axe x
    void            *rx_donnees;
    Type_EF_Appui   ry;
    void            *ry_donnees;
    Type_EF_Appui   rz;
    void            *rz_donnees;
} EF_Appui;


typedef enum __Type_Noeud
{
    NOEUD_LIBRE,
    NOEUD_BARRE
} Type_Noeud;


typedef struct __EF_Noeud
{
    unsigned int    numero;
    Type_Noeud      type;
    void            *data; // Contient les données permettant de récupérer les coordonnées en fonction dy type de noeud.
    EF_Appui        *appui;
} EF_Noeud;


typedef struct __Charge_Noeud
{
    Barre_Charge_Type   type;
    unsigned int        numero;
#ifdef ENABLE_GTK
    GtkTreeIter         Iter;
#endif                  
    char                *description;
    GList               *noeuds;
    double              fx;
    double              fy;
    double              fz;
    double              mx;
    double              my;
    double              mz;
} Charge_Noeud;


typedef struct __Beton_Section_Rectangulaire
{
    Type_Beton_Section  type;
    char                *nom;
    unsigned int        numero;
    double              largeur;
    double              hauteur;
} Beton_Section_Rectangulaire;


typedef struct __Beton_Section_T
{
    Type_Beton_Section  type;
    char                *nom;
    unsigned int        numero;
    double              largeur_table;
    double              largeur_ame;
    double              hauteur_table;
    double              hauteur_ame;
} Beton_Section_T;


typedef struct __Beton_Section_Carre
{
    Type_Beton_Section  type;
    char                *nom;
    unsigned int        numero;
    double              cote;
} Beton_Section_Carre;


typedef struct __Beton_Section_Circulaire
{
    Type_Beton_Section  type;
    char                *nom;
    unsigned int        numero;
    double              diametre;
} Beton_Section_Circulaire;


typedef struct __EF_Relachement_Donnees_Elastique_Lineaire
{
    double              raideur;    // La raideur doit être indiquée en N.m/rad
} EF_Relachement_Donnees_Elastique_Lineaire;


typedef struct __EF_Relachement
{
    unsigned int        numero;
    char                *nom;
    EF_Relachement_Type rx_debut;   // Type de relachement du moment autour de l'axe x
    void                *rx_d_data; // Paramètres complémentaires au relachement. N'est à 
                                    // définir que si le type de relachement est différent
                                    // de LIBRE et BLOQUE
    EF_Relachement_Type ry_debut;
    void                *ry_d_data;
    EF_Relachement_Type rz_debut;
    void                *rz_d_data;
    EF_Relachement_Type rx_fin;
    void                *rx_f_data;
    EF_Relachement_Type ry_fin;
    void                *ry_f_data;
    EF_Relachement_Type rz_fin;
    void                *rz_f_data;
} EF_Relachement;


typedef struct __Beton_Materiau
{
    unsigned int    numero;
    char        *nom;
    
    // Caractéristique du matériau béton conformément à EN 1992_1_1, Tableau 3.1
    double      fck;
    double      fckcube;
    double      fcm;
    double      fctm;
    double      fctk_0_05;
    double      fctk_0_95;
    double      ecm;
    double      ec1;
    double      ecu1;
    double      ec2;
    double      ecu2;
    double      n;
    double      ec3;
    double      ecu3;
    
    double      nu;
    double      gnu_0_2;
    double      gnu_0_0;
} Beton_Materiau;


typedef struct __Barre_Info_EF
{
    cholmod_sparse      *matrice_rigidite_locale;
    
    double              ay; // Paramètres de souplesse de la poutre
    double              by;
    double              cy;
    double              az;
    double              bz;
    double              cz;
    
    double              kAx;
    double              kAy; // coefficient défissant l'inverse de la raideur aux noeuds
    double              kAz;
    double              kBx;
    double              kBy;
    double              kBz;
} Barre_Info_EF;


typedef struct __Beton_Barre
{
    unsigned int        numero;
    Type_Element        type;
    void                *section;
    Beton_Materiau      *materiau;
    
    EF_Noeud            *noeud_debut;
    EF_Noeud            *noeud_fin;
    EF_Relachement      *relachement;
    unsigned int        discretisation_element;     // Nombre de noeuds intermédiaires
    GList               *noeuds_intermediaires;
    Barre_Info_EF       *info_EF;                   // Une info par élément discrétisé
    
    cholmod_sparse      *matrice_rotation;
    cholmod_sparse      *matrice_rotation_transpose;
} Beton_Barre;


typedef struct __EF_Noeud_Barre
{
    Beton_Barre *barre;
    double      position_relative_barre;
} EF_Noeud_Barre;


typedef struct __Charge_Barre_Ponctuelle
{
    Barre_Charge_Type   type;
    unsigned int        numero;
#ifdef ENABLE_GTK
    GtkTreeIter         Iter;
#endif
    char                *description;
    GList               *barres;
    gboolean            repere_local;
    double              position; // Position de la charge ponctuelle en mètre
                                  // depuis le début de la barre
    double              fx;
    double              fy;
    double              fz;
    double              mx;
    double              my;
    double              mz;
    
} Charge_Barre_Ponctuelle;


typedef struct __Charge_Barre_Repartie_Uniforme
{
    Barre_Charge_Type   type;
    unsigned int        numero;
#ifdef ENABLE_GTK
    GtkTreeIter         Iter;
#endif    
    char                *description;
    GList               *barres;
    gboolean            repere_local;
    gboolean            projection;
    double              a; // Position du début de la charge répartie par rapport au début
    double              b;   // Position de la fin de la charge par rapport à la fin
    double              fx;
    double              fy;
    double              fz;
    double              mx;
    double              my;
    double              mz;
} Charge_Barre_Repartie_Uniforme;


typedef struct __Troncon // Définition de la structure Troncon qui contient une fonction
                         // avec un domaine de validité précis.
{
    double      debut_troncon;  // Début du tronçon de validité de la fonction
    double      fin_troncon;    // Fin du tronçon de validité de la fonction
    double      x0;             // La fonction mathématique est définie par :
    double      x1;             // x0 + x1*x +
    double      x2;             // x2*x^2 +
    double      x3;             // x3*x^3 +
    double      x4;             // x4*x^4 +
    double      x5;             // x5*x^5 +
    double      x6;             // x6*x^6
} Troncon;


typedef struct __Fonction
{
    unsigned int     nb_troncons;    /* Les fonctions n'étant pas forcément continues le long de
                                  * la barre (par exemple de part et d'une charge ponctuelle),
    * il est nécessaire de définir plusieurs tronçons avec pour chaque tronçon sa fonction.
    * nb_troncons défini le nombre de tronçons que possède la fonction.*/
    
    Troncon          *troncons;      /* Tableau dynamique contenant les fonctions continues par
                                  * tronçon. */
} Fonction;


typedef struct __Action
{
    char            *description;
    unsigned int    numero;
    unsigned int    type;  // Les catégories sont conformes à _1990_action_type
    GList           *charges;
    int             flags;
    double          psi0;       // valeur_combinaison
    double          psi1;       // valeur_frequente
    double          psi2;       // valeur_quasi_permanente
    cholmod_sparse  *deplacement_complet;
    cholmod_sparse  *forces_complet;
    cholmod_sparse  *efforts_noeuds;
    double          norm;
#ifdef ENABLE_GTK
    GtkTreeIter     Iter;
#endif
    
    Fonction        **fonctions_efforts[6]; // 6 fonctions (N, Ty, Tz, Mx, My, Mz) par barre.
                    // Les fonctions représentent la courbe des efforts dues aux charges dans
                    // les barres et dus aux déplacements calculés par la méthode des EF.
    Fonction        **fonctions_deformation[3]; // Les 3 déformations x, y, z pour chaque barre
    Fonction        **fonctions_rotation[3];    // Les 3 rotations rx, ry, rz pour chaque barre
} Action;


typedef struct __Ponderation_Element
{
    double      ponderation;
    int         psi;
    int         flags;
    Action      *action;
} Ponderation_Element;


// Ne pas supprimer l'espace après le __Ponderation, c'est pour la génération du manuel Latex
typedef struct __Ponderation 
{
    GList       *elements; // Liste de pointeur Action* avec leur ponderation
} Ponderation;


typedef struct __Element
{
    unsigned int    numero;
#ifdef ENABLE_GTK
    GtkTreeIter     Iter;         // Pour la fenêtre groupes
    int             Iter_expand;
#endif
} Element;


typedef struct __Combinaison_Element
{
    Action      *action;
    int         flags;          // bit 1 : 1 si les actions variables sont prédominantes
} Combinaison_Element;


// Nota : L'espace après __Combinaison est nécessaire pour la génération du document Latex.
//        A ne surtout pas supprimer!!!
typedef struct __Combinaison 
{
    GList       *elements;      // Liste de pointeurs Combinaison_Element
} Combinaison;


typedef struct __Combinaisons
{
    GList       *combinaisons;  // Liste de "combinaison"
} Combinaisons;


typedef struct __Groupe
{
    char                    *nom;
    unsigned int            numero;
    Type_Groupe_Combinaison type_combinaison;
    GList                   *elements;
    Combinaisons            tmp_combinaison;
#ifdef ENABLE_GTK
    GtkTreeIter             Iter;         // Pour la fenêtre groupes
    int                     Iter_expand;
#endif
} Groupe;


typedef struct __Niveau_Groupe
{
    unsigned int    niveau;
    GList           *groupes;
#ifdef ENABLE_GTK
    GtkTreeIter     Iter;                 // Pour la fenêtre groupes
#endif
} Niveau_Groupe;


typedef struct __CombinaisonsEL
{                           // Spécifie la méthode des combinaisons (E0,A1.3)
    int     flags;          // bit 0      : ELU_EQU : méthode note 1 si le bit 1 = 0
                            //            : ELU_EQU : méthode note 2 si le bit 1 = 1
                            // bit 1 et 2 : ELU_GEO/STR : 00 méthode approche 1
                            //            : ELU_GEO/STR : 01 méthode approche 2
                            //            : ELU_GEO/STR : 10 méthode approche 3
                            // bit 3      : ELU_ACC : 0 si utilisation de psi1,1
                            //            : ELU_ACC : 1 si utilisation de psi2,1
                            // bit 4      : 0 si utilisation des formules 6.10a et b
                            //            : 1 si utilisation de la formule 6.10
                            // bit 5      : 0 si valeur fréquence en cas d'accident
                            //            : 1 si valeur quasi-permanente en cas d'accident
    GList       *elu_equ;   // Liste des combinaisons selon l'ELU EQU
    GList       *elu_str;   // ..
    GList       *elu_geo;   //
    GList       *elu_fat;   //
    GList       *elu_acc;   //
    GList       *elu_sis;   //
    GList       *els_car;   //
    GList       *els_freq;  //
    GList       *els_perm;  //
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
    GtkWidget   *menu_structure;
    GtkWidget   *menu_structure_list;
    GtkWidget   *menu_structure_noeud;
    GtkWidget   *menu_structure_barres;
    GtkWidget   *menu_charges;
    GtkWidget   *menu_charges_list;
    GtkWidget   *menu_charges_actions;
    GtkWidget   *menu_charges_groupes;
} Comp_Gtk;


typedef struct __List_Gtk_m3d
{
    GtkWidget   *drawing;
    gpointer    data;
} List_Gtk_m3d;


typedef struct __List_Gtk
{                       // Contient toutes les données pour l'interface graphique GTK+3
    List_Gtk_1990_Actions   _1990_actions;     // pour l'Eurocode 0
    List_Gtk_1990_Groupes   _1990_groupes;     // pour l'Eurocode 0
    List_Gtk_EF_Charge_Noeud ef_charge_noeud;
    List_Gtk_EF_Charge_Barre_Ponctuelle ef_charge_barre_ponctuelle;
    List_Gtk_EF_Charge_Barre_Repartie_Uniforme ef_charge_barre_repartie_uniforme;
    List_Gtk_m3d            m3d;       // pour l'affichage graphique de la structure
    List_Gtk_EF_Noeud       ef_noeud;
    List_Gtk_EF_Barres      ef_barres;
    Comp_Gtk                comp;      // tous les composants grahpiques
} List_Gtk;
#endif


typedef struct __EF
{                               // Contient toutes les données nécessaires pour la réalisation
                                // des calculs aux éléments finis et notamment les variables
                                // utilisées par les librairies cholmod et umfpack :
    cholmod_common      Common; // Paramètres des calculs de la librairie cholmod.
    cholmod_common      *c;     // Pointeur vers Common
                                
    GList               *noeuds;       // Liste de tous les noeuds de la structure, que ce soit
                                       // les noeuds définis par l'utilisateur ou les noeuds
                                       // créés par la discrétisation des éléments.
    GList               *appuis;       // Liste des types d'appuis
    GList               *relachements; // Liste des types de relâchements des barres.
    
    unsigned int        **noeuds_pos_partielle; // Etabli une corrélation entre le degré de 
    unsigned int        **noeuds_pos_complete;  // liberté (x, y, z, rx, ry, rz) d'un noeud
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


typedef struct __Beton_Donnees
{                               // Liste des sections, matériaux et barres en béton.
    GList           *sections;
    GList           *materiaux;
    GList           *barres;
} Beton_Donnees;


typedef struct __Projet
{
    GList           *actions;           // Liste des actions contenant chacune des charges
    GList           *niveaux_groupes;   // Compatibilités entres actions
    CombinaisonsEL  combinaisons;       // Combinaisons conformes aux Eurocodes
    Type_Pays       pays;               // Pays de calculs
#ifdef ENABLE_GTK
    List_Gtk        list_gtk;           // Informations nécessaires pour l'interface graphique
#endif
    EF              ef_donnees;         // Données communes à tous les éléments finis
    Beton_Donnees   beton;              // Données spécifiques au béton
} Projet;


Projet *projet_init(Type_Pays pays);
#ifdef ENABLE_GTK
int projet_init_graphique(Projet *projet);
#endif
int projet_free(Projet *projet);

#endif