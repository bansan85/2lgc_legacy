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

// Nombre de décimal en fonction des unités
#define DECIMAL_ANGLE 3
#define DECIMAL_DISTANCE 3
#define DECIMAL_DEPLACEMENT 6
#define DECIMAL_ROTATION 6
#define DECIMAL_FORCE 3
#define DECIMAL_MOMENT 3
#define DECIMAL_M4 8
#define DECIMAL_SURFACE 3
#define DECIMAL_CONTRAINTE 3
#define DECIMAL_NEWTON_PAR_METRE 3
#define DECIMAL_SANS_UNITE 3

#ifdef ENABLE_GTK
#include <gtk/gtk.h>


typedef struct __Action Action;


typedef struct __Gtk_1990_Groupes
{
  GtkBuilder       *builder;
   
  GtkWidget        *window;
  GtkTreeStore     *tree_store_etat;
  GtkTreeStore     *tree_store_dispo;
  
  GtkTreeSelection *tree_select_etat;
  GtkTreeView      *tree_view_etat;
  GtkTreeSelection *tree_select_dispo;
  GtkWidget        *spin_button_niveau;
  GtkTreeView      *tree_view_dispo;
  
  GtkWidget        *drag_from;
  GtkToolItem      *item_groupe_and;
  GtkToolItem      *item_groupe_or;
  GtkToolItem      *item_groupe_xor;
  GtkToolItem      *item_ajout_dispo;
  GtkToolItem      *item_ajout_tout_dispo;
  GtkToolItem      *item_groupe_suppr;
} Gtk_1990_Groupes;


typedef struct __Gtk_1990_Groupes_Options
{
  GtkBuilder *builder;
  GtkWidget  *window;
} Gtk_1990_Groupes_Options;


typedef struct __Gtk_1990_Actions
{
  GtkBuilder       *builder;
  GtkWidget        *window;
  
  GtkTreeStore     *tree_store_actions;
  GtkTreeSelection *tree_select_actions;
  GtkTreeView      *tree_view_actions;
  GtkListStore     *choix_type_action;
  
  GtkMenu          *type_action; // Menu contenant la liste des différents types d'action.
  GList            *items_type_action; // Liste contenant l'ensemble des items du menu menu_type_list_action. Est uniquement utilisé pour trouver quel menu a été cliqué parmi ceux de type_action.
  
  GtkTreeStore     *tree_store_charges;
  GtkTreeSelection *tree_select_charges;
  GtkTreeView      *tree_view_charges;
  
  GtkMenu          *type_charges; // Menu contenant tous les types de charge développée dans le programme. Est utilisé uniquement par la fenêtre Action permettant de créer de nouvelles charges.
  GtkListStore     *liste; // Contient la liste des actions sous forme d'un
                            // menu
  // compatible avec les composants GtkComboBox. Cette liste est utilisée dans les fenêtres
  // permettant de créer ou d'éditer des charges afin de définir dans quelle action elles
  // s'appliqueront.
} Gtk_1990_Actions;


typedef struct __Gtk_EF_Noeud
{
  GtkBuilder   *builder;
  
  GtkWidget    *window;
  GtkWidget    *notebook;
  GtkTreeStore *tree_store_libre;
  GtkTreeStore *tree_store_barre;
} Gtk_EF_Noeud;


typedef struct __Gtk_EF_Barres
{
  GtkBuilder   *builder;
  GtkWidget    *window;
  
  GtkListStore *liste_types;
} Gtk_EF_Barres;


typedef struct __Gtk_EF_Barres_Add
{
  GtkBuilder   *builder;
  GtkWidget    *window;
} Gtk_EF_Barres_Add;


typedef struct __Gtk_EF_Appuis
{
  GtkBuilder   *builder;
  GtkWidget    *window;
  
  GtkTreeStore *appuis;
  GtkListStore *liste_appuis;
  GtkListStore *liste_type_appui;
} Gtk_EF_Appuis;


typedef struct __Gtk_EF_Sections
{
  GtkBuilder   *builder;
  GtkWidget    *window;
  
  GtkTreeStore *sections;
  GtkListStore *liste_sections;
} Gtk_EF_Sections;


typedef struct __Gtk_EF_Materiaux
{
  GtkBuilder   *builder;
  GtkWidget    *window;
  
  GtkTreeStore *materiaux;
  GtkListStore *liste_materiaux;
} Gtk_EF_Materiaux;


typedef struct __Gtk_EF_Relachements
{
  GtkBuilder   *builder;
  GtkWidget    *window;
  
  GtkTreeStore *relachements;
  GtkListStore *liste_relachements;
} Gtk_EF_Relachements;
#endif


#ifdef ENABLE_GTK
typedef struct __Gtk_EF_Rapport
{
  GtkBuilder   *builder;
  GtkWidget    *window;
  
  GtkListStore *liste;
  GList        *rapport;
} Gtk_EF_Rapport;
#endif


typedef enum __Filtres
{
  FILTRE_AUCUN,
  FILTRE_NOEUD_APPUI
} Filtres;


typedef enum __Colonne_Resultats
{
  COLRES_NUM_NOEUDS,        // Numéro des noeuds
  COLRES_NUM_BARRES,        // Numéro des barres
  COLRES_NOEUDS_X,        // Coordonnées des noeuds, x
  COLRES_NOEUDS_Y,        // Coordonnées des noeuds, y
  COLRES_NOEUDS_Z,        // Coordonnées des noeuds, z
  COLRES_REACTION_APPUI_FX,     // Réaction d'appui, Fx
  COLRES_REACTION_APPUI_FY,     // Réaction d'appui, Fy
  COLRES_REACTION_APPUI_FZ,     // Réaction d'appui, Fz
  COLRES_REACTION_APPUI_MX,     // Réaction d'appui, Mx
  COLRES_REACTION_APPUI_MY,     // Réaction d'appui, My
  COLRES_REACTION_APPUI_MZ,     // Réaction d'appui, Mz
  COLRES_DEPLACEMENT_UX,      // Déplacement, Ux
  COLRES_DEPLACEMENT_UY,      // Déplacement, Uy
  COLRES_DEPLACEMENT_UZ,      // Déplacement, Uz
  COLRES_DEPLACEMENT_RX,      // Déplacement, Rx
  COLRES_DEPLACEMENT_RY,      // Déplacement, Ry
  COLRES_DEPLACEMENT_RZ,      // Déplacement, Rz
  COLRES_BARRES_LONGUEUR,     // Longueur des barres
  COLRES_BARRES_PIXBUF_N,     // Dessin de l'effort normal N
  COLRES_BARRES_PIXBUF_TY,    // Dessin de l'effort tranchant en y
  COLRES_BARRES_PIXBUF_TZ,    // Dessin de l'effort tranchant en z
  COLRES_BARRES_PIXBUF_MX,    // Dessin du moment de torsion
  COLRES_BARRES_PIXBUF_MY,    // Dessin du moment fléchissant en y
  COLRES_BARRES_PIXBUF_MZ,    // Dessin du moment fléchissant en z
  COLRES_BARRES_DESC_N,       // Points caractéristiques de l'effort normal N
  COLRES_BARRES_DESC_TY,      // Points caractéristiques de l'effort tranchant en y
  COLRES_BARRES_DESC_TZ,      // Points caractéristiques de l'effort tranchant en z
  COLRES_BARRES_DESC_MX,      // Points caractéristiques du moment de torsion
  COLRES_BARRES_DESC_MY,      // Points caractéristiques du moment fléchissant en y
  COLRES_BARRES_DESC_MZ,      // Points caractéristiques du moment fléchissant en z
  COLRES_BARRES_EQ_N,       // Equation de l'effort normal N
  COLRES_BARRES_EQ_TY,      // Equation de l'effort tranchant en y
  COLRES_BARRES_EQ_TZ,      // Equation de l'effort tranchant en z
  COLRES_BARRES_EQ_MX,      // Equation du moment de torsion
  COLRES_BARRES_EQ_MY,      // Equation du moment fléchissant en y
  COLRES_BARRES_EQ_MZ,      // Equation du moment fléchissant en z
  COLRES_DEFORMATION_PIXBUF_UX,   // Dessin de la déformation des barres, Ux
  COLRES_DEFORMATION_PIXBUF_UY,   // Dessin de la déformation des barres, Uy
  COLRES_DEFORMATION_PIXBUF_UZ,   // Dessin de la déformation des barres, Uz
  COLRES_DEFORMATION_PIXBUF_RX,   // Dessin de la déformation des barres, Rx
  COLRES_DEFORMATION_PIXBUF_RY,   // Dessin de la déformation des barres, Rx
  COLRES_DEFORMATION_PIXBUF_RZ,   // Dessin de la déformation des barres, Rx
  COLRES_DEFORMATION_DESC_UX,   // Points caractéristiques de la déformation des barres, Ux
  COLRES_DEFORMATION_DESC_UY,   // Points caractéristiques de la déformation des barres, Uy
  COLRES_DEFORMATION_DESC_UZ,   // Points caractéristiques de la déformation des barres, Uz
  COLRES_DEFORMATION_DESC_RX,   // Points caractéristiques de la déformation des barres, Rx
  COLRES_DEFORMATION_DESC_RY,   // Points caractéristiques de la déformation des barres, Rx
  COLRES_DEFORMATION_DESC_RZ,   // Points caractéristiques de la déformation des barres, Rx
  COLRES_DEFORMATION_UX,      // Déformation des barres, Ux
  COLRES_DEFORMATION_UY,      // Déformation des barres, Uy
  COLRES_DEFORMATION_UZ,      // Déformation des barres, Uz
  COLRES_DEFORMATION_RX,      // Déformation des barres, Rx
  COLRES_DEFORMATION_RY,      // Déformation des barres, Ry
  COLRES_DEFORMATION_RZ       // Déformation des barres, Rz
} Colonne_Resultats;


#ifdef ENABLE_GTK
typedef struct __Gtk_EF_Resultats_Tableau
{
  GtkTreeView       *treeview;
  GtkListStore      *list_store;
  
  Colonne_Resultats *col_tab;
  Filtres            filtre;
  char              *nom;
} Gtk_EF_Resultats_Tableau;


typedef struct __Gtk_EF_Resultats
{
  GtkBuilder  *builder;
  GtkWidget   *window;
  
  GtkComboBox *combobox;
  GtkComboBox *combobox_cas;
  GtkComboBox *combobox_ponderations;
  
  GtkNotebook *notebook;
  GList       *tableaux;
} Gtk_EF_Resultats;
#endif


typedef enum __Type_Element // La liste des différents éléments de type de barres gérés par le
{               // module élément fini.
  BETON_ELEMENT_POTEAU,
  BETON_ELEMENT_POUTRE
} Type_Element;


typedef enum __Norme
{       // Spécifie la norme et l'annexe nationnale à utiliser :
  NORME_EU, // Norme européenne sans application des annexes nationales,
  NORME_FR  // Annexe nationale française.
} Norme;


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


typedef enum __Type_EF_Appui
{
  EF_APPUI_LIBRE,         // Déplacement libre
  EF_APPUI_BLOQUE         // Déplacement bloqué
} Type_EF_Appui;


typedef enum __EF_Relachement_Type
{
  EF_RELACHEMENT_BLOQUE,
  EF_RELACHEMENT_LIBRE,
  EF_RELACHEMENT_ELASTIQUE_LINEAIRE
} EF_Relachement_Type;


typedef enum __Type_Section
{
  SECTION_RECTANGULAIRE,
  SECTION_T,
  SECTION_CARREE,
  SECTION_CIRCULAIRE,
  SECTION_PERSONNALISEE
} Type_Section;


typedef enum __Charge_Type
{
  CHARGE_NOEUD,
  CHARGE_BARRE_PONCTUELLE,
  CHARGE_BARRE_REPARTIE_UNIFORME
} Charge_Type;


typedef enum __Type_Noeud
{
  NOEUD_LIBRE,
  NOEUD_BARRE
} Type_Noeud;


typedef enum __Type_Liste
{
  LISTE_UINT,
  LISTE_NOEUDS,
  LISTE_BARRES,
  LISTE_CHARGES
} Type_Liste;


typedef enum __Type_Materiau
{
  MATERIAU_BETON,
  MATERIAU_ACIER
} Type_Materiau;


typedef enum __Type_Neige
{
  NEIGE_A1,
  NEIGE_A2,
  NEIGE_B1,
  NEIGE_B2,
  NEIGE_C1,
  NEIGE_C2,
  NEIGE_D,
  NEIGE_E
} Type_Neige;


typedef enum __Type_Vent
{
  VENT_1,
  VENT_2,
  VENT_3,
  VENT_4
} Type_Vent;


typedef enum __Type_Seisme
{
  SEISME_1,
  SEISME_2,
  SEISME_3,
  SEISME_4,
  SEISME_5
} Type_Seisme;


typedef enum __Type_Flottant
{
  FLOTTANT_ORDINATEUR,
  FLOTTANT_UTILISATEUR
} Type_Flottant;


typedef enum __Type_Vue
{
  VUE_XYZ
} Type_Vue;


typedef struct __Flottant
{
  double        d;
  Type_Flottant type;
} Flottant;


typedef struct __EF_Point
{
  Flottant x;
  Flottant y;
  Flottant z;
} EF_Point;


typedef struct __EF_Appui
{
  char         *nom;
  Type_EF_Appui ux;        // Degré de liberté de la direction x
  void         *ux_donnees;   // Données complémentaire si nécessaire.
  Type_EF_Appui uy;
  void         *uy_donnees;
  Type_EF_Appui uz;
  void         *uz_donnees;
  Type_EF_Appui rx;       // Degré de liberté de la rotation autour de l'axe x
  void         *rx_donnees;
  Type_EF_Appui ry;
  void         *ry_donnees;
  Type_EF_Appui rz;
  void         *rz_donnees;
#ifdef ENABLE_GTK
  GtkTreeIter   Iter_fenetre;       // Pour le treeview
  GtkTreeIter   Iter_liste;       // pour le composant list_appuis
#endif          
} EF_Appui;


typedef struct __Ligne_Adresse
{
  char *affichage;
  int   population;
  char  departement[4];
  int   commune;
  int   code_postal;
  char *ville;
} Ligne_Adresse;


typedef struct __EF_Noeud
{
  unsigned int numero;
  Type_Noeud   type;
  void        *data; // Contient les données permettant de récupérer les coordonnées en
               // fonction du type de noeud.
  EF_Appui    *appui;
#ifdef ENABLE_GTK
  GtkTreeIter  Iter;
#endif
} EF_Noeud;


#ifdef ENABLE_GTK
#define ENTETE_ITER GtkTreeIter Iter;
#else
#define ENTETE_ITER
#endif


#define ENTETE_CHARGES \
  Charge_Type type; \
  ENTETE_ITER \
  char  *nom;


typedef struct __Charge_Noeud
{
  GList   *noeuds;
  Flottant fx;
  Flottant fy;
  Flottant fz;
  Flottant mx;
  Flottant my;
  Flottant mz;
} Charge_Noeud;


typedef struct __Charge_Barre_Ponctuelle
{
  GList       *barres; // À laisser en premier
  unsigned int repere_local : 1;
  Flottant     position; // Position de la charge ponctuelle en mètre
               // depuis le début de la barre
  Flottant     fx;
  Flottant     fy;
  Flottant     fz;
  Flottant     mx;
  Flottant     my;
  Flottant     mz;
} Charge_Barre_Ponctuelle;


typedef struct __Charge_Barre_Repartie_Uniforme
{
  GList       *barres; // À laisser en premier
  unsigned int repere_local : 1;
  unsigned int projection : 1;
  Flottant     a; // Position du début de la charge par rapport au début
  Flottant     b; // Position de la fin de la charge par rapport à la fin
  Flottant     fx;
  Flottant     fy;
  Flottant     fz;
  Flottant     mx;
  Flottant     my;
  Flottant     mz;
} Charge_Barre_Repartie_Uniforme;


typedef struct __Charge
{
  ENTETE_CHARGES
  void *data;
} Charge;


typedef struct __EF_Relachement_Donnees_Elastique_Lineaire
{
  Flottant raideur;  // La raideur doit être indiquée en N.m/rad
} EF_Relachement_Donnees_Elastique_Lineaire;


typedef struct __EF_Relachement
{
  char               *nom;
  EF_Relachement_Type rx_debut;   // Type de relachement du moment autour de l'axe x
  void               *rx_d_data; // Paramètres complémentaires au relachement. N'est à 
                  // définir que si le type de relachement est différent
                  // de LIBRE et BLOQUE
  EF_Relachement_Type ry_debut;
  void               *ry_d_data;
  EF_Relachement_Type rz_debut;
  void               *rz_d_data;
  EF_Relachement_Type rx_fin;
  void               *rx_f_data;
  EF_Relachement_Type ry_fin;
  void               *ry_f_data;
  EF_Relachement_Type rz_fin;
  void               *rz_f_data;
#ifdef ENABLE_GTK
  GtkTreeIter         Iter_fenetre;
  GtkTreeIter         Iter_liste;
#endif
} EF_Relachement;


typedef struct __Barre_Info_EF
{
  cholmod_sparse *m_rig_loc; // Matrice de rigidite locale
  
  double          ay; // Paramètres de souplesse de la poutre
  double          by;
  double          cy;
  double          az;
  double          bz;
  double          cz;
  
  double          kAx;
  double          kAy; // coefficient défissant l'inverse de la raideur aux noeuds
  double          kAz;
  double          kBx;
  double          kBy;
  double          kBz;
} Barre_Info_EF;


typedef struct __Section_T
{
  /* La section rectangulaire est intégrée dans la section en T. Il convient simplement de
     mettre la hauteur dans hauteur_retombee et la largeur dans largeur_retombee.
     largeur_table et hauteur_table sont mis à 0.*/
  Flottant largeur_table;
  Flottant largeur_retombee;
  Flottant hauteur_table;
  Flottant hauteur_retombee;
} Section_T;


typedef struct __Section_Circulaire
{
  Flottant diametre;
} Section_Circulaire;


typedef struct __Section_Personnalisee
{
  char    *description;
  Flottant j;
  Flottant iy;
  Flottant iz;
  Flottant vy;
  Flottant vyp;
  Flottant vz;
  Flottant vzp;
  Flottant s;
  GList   *forme;
} Section_Personnalisee;


typedef struct __Section
{
  Type_Section type;
  char        *nom;
#ifdef ENABLE_GTK
  GtkTreeIter  Iter_fenetre;
  GtkTreeIter  Iter_liste;
#endif
  void        *data;
} Section;


typedef struct __Materiau_Beton
{
  // Caractéristique du matériau béton conformément à EN 1992_1_1, Tableau 3.1
  Flottant fck;
  Flottant fckcube;
  Flottant fcm;
  Flottant fctm;
  Flottant fctk_0_05;
  Flottant fctk_0_95;
  
  Flottant ec1;
  Flottant ecu1;
  Flottant ec2;
  Flottant ecu2;
  Flottant n;
  Flottant ec3;
  Flottant ecu3;
  
  Flottant ecm;
  Flottant nu;
} Materiau_Beton;


typedef struct __Materiau_Acier
{
  Flottant fy;
  Flottant fu;
  
  Flottant e;
  Flottant nu;
} Materiau_Acier;


typedef struct __EF_Materiau
{
  Type_Materiau type;
  char         *nom;
  void         *data;
  
#ifdef ENABLE_GTK
  GtkTreeIter   Iter_fenetre;
  GtkTreeIter   Iter_liste;
#endif
} EF_Materiau;


#ifdef ENABLE_GTK
typedef struct __Gtk_EF_Section_Rectangulaire
{
  GtkBuilder *builder;
  GtkWidget  *window;
  
  Section    *section;
} Gtk_EF_Section_Rectangulaire;


typedef struct __Gtk_EF_Section_T
{
  GtkBuilder *builder;
  GtkWidget  *window;
  
  Section    *section;
} Gtk_EF_Section_T;


typedef struct __Gtk_EF_Section_Carree
{
  GtkBuilder *builder;
  GtkWidget  *window;
  
  Section    *section;
} Gtk_EF_Section_Carree;


typedef struct __Gtk_EF_Section_Circulaire
{
  GtkBuilder *builder;
  GtkWidget  *window;
  
  Section    *section;
} Gtk_EF_Section_Circulaire;


typedef struct __Gtk_EF_Section_Personnalisee
{
  GtkBuilder   *builder;
  GtkWidget    *window;
  
  Section      *section;
  GtkTreeModel *model;
  
  gboolean      keep; // Ne libère pas la liste des noeuds à la fermeture.
} Gtk_EF_Section_Personnalisee;


typedef struct __Gtk_1992_1_1_Materiaux
{
  GtkBuilder  *builder;
  GtkWidget   *window;
  
  EF_Materiau *materiau;
} Gtk_1992_1_1_Materiaux;


typedef struct __Gtk_1993_1_1_Materiaux
{
  GtkBuilder  *builder;
  GtkWidget   *window;
  
  EF_Materiau *materiau;
} Gtk_1993_1_1_Materiaux;


typedef struct __Gtk_Common_Informations
{
  GtkBuilder   *builder;
  GtkWidget    *window;
  
  GtkListStore *model_completion;
  
  char         *departement;
  int           commune;
} Gtk_Common_Informations;
#endif


typedef struct __EF_Barre
{
  unsigned int    numero;
  Type_Element    type;
  Section        *section;
  EF_Materiau    *materiau;
  
  EF_Noeud       *noeud_debut;
  EF_Noeud       *noeud_fin;
  Flottant        angle;
  EF_Relachement *relachement;
  unsigned int    discretisation_element;   // Nombre de noeuds intermédiaires
  GList          *nds_inter; // Noeuds intermédiaires
  Barre_Info_EF  *info_EF;           // Une info par élément discrétisé
#ifdef ENABLE_GTK
  GtkTreeIter     Iter;
#endif          
  cholmod_sparse *m_rot;
  cholmod_sparse *m_rot_t;
} EF_Barre;


typedef struct __EF_Noeud_Libre
{
  Flottant  x;
  Flottant  y;
  Flottant  z;
  EF_Noeud *relatif;
} EF_Noeud_Libre;


typedef struct __EF_Noeud_Barre
{
  EF_Barre *barre;
  Flottant  position_relative_barre;
} EF_Noeud_Barre;


typedef struct __Gtk_EF_Charge_Noeud
{
  GtkBuilder *builder;
  GtkWidget  *window;
  GtkWidget  *combobox_charge;
  
  Action     *action;
  Charge     *charge;
} Gtk_EF_Charge_Noeud;


typedef struct __Gtk_EF_Charge_Barre_Ponctuelle
{
  GtkBuilder  *builder;
  GtkWidget   *window;
  Action      *action;
  Charge      *charge;
  GtkComboBox *combobox_charge;
} Gtk_EF_Charge_Barre_Ponctuelle;


typedef struct __Gtk_EF_Charge_Barre_Repartie_Uniforme
{
  GtkBuilder  *builder;
  GtkWidget   *window;
  
  Action      *action;
  Charge      *charge;
  
  GtkComboBox *combobox_charge;
  GtkWidget   *check_button_repere_local;
  GtkWidget   *check_button_repere_global;
  GtkWidget   *check_button_projection;
} Gtk_EF_Charge_Barre_Repartie_Uniforme;


typedef struct __Troncon // Définition de la structure Troncon qui contient une fonction
             // avec un domaine de validité précis.
{
  double debut_troncon;  // Début du tronçon de validité de la fonction
  double fin_troncon;  // Fin du tronçon de validité de la fonction
  double x0;       // La fonction mathématique est définie par :
  double x1;       // x0 + x1*x +
  double x2;       // x2*x^2 +
  double x3;       // x3*x^3 +
  double x4;       // x4*x^4 +
  double x5;       // x5*x^5 +
  double x6;       // x6*x^6
} Troncon;


typedef struct __Fonction
{
  unsigned int nb_troncons;  /* Les fonctions n'étant pas forcément continues le long de
                  * la barre (par exemple de part et d'une charge ponctuelle),
  * il est nécessaire de définir plusieurs tronçons avec pour chaque tronçon sa fonction.
  * nb_troncons défini le nombre de tronçons que possède la fonction.*/
  
  Troncon     *troncons;    /* Tableau dynamique contenant les fonctions continues par
                  * tronçon. */
} Fonction;


typedef struct __Ponderation
{
  double  ponderation;
  int     psi;
  int     flags;
  Action *action;
} Ponderation;


typedef struct __Combinaison
{
  Action *action;
  int     flags;      // bit 1 : 1 si les actions variables sont prédominantes
} Combinaison;


#ifdef ENABLE_GTK
#define ENTETE_ITER_GROUPE GtkTreeIter Iter_groupe;
#else
#define ENTETE_ITER_GROUPE
#endif


#define ENTETE_ACTION_ET_GROUPE \
  char  *nom; \
  ENTETE_ITER_GROUPE


typedef struct __Groupe
{
  ENTETE_ACTION_ET_GROUPE
  Type_Groupe_Combinaison type_combinaison;
  GList                  *elements;      // Contient des Action si le groupe appartient
                        // au niveau 0,
                        // Contient des Groupe sinon.
  GList                  *tmp_combinaison;
#ifdef ENABLE_GTK
  unsigned int            Iter_expand : 1;
#endif
} Groupe;


typedef struct __Niveau_Groupe
{
  GList      *groupes;   // groupes contient une liste de groupe.
                // Si le niveau vaut 0, chaque groupe contiendra une liste
                // d'action.
                // Si le niveau est supérieur à 0, chaque groupe contiendra une
                // liste de groupe du niveau inférieur.
#ifdef ENABLE_GTK
  GtkTreeIter Iter;     // Pour la fenêtre groupes
#endif
} Niveau_Groupe;


typedef struct __CombinaisonsEL
{               // Spécifie la méthode des combinaisons (E0,A1.3)
  unsigned int  flags : 6;// bit 0 : ELU_EQU : méthode 1 si le bit 1 = 0
              //      : ELU_EQU : méthode note 2 si le bit 1 = 1
              // bit 1 et 2 : ELU_GEO/STR : 00 méthode approche 1
              //      : ELU_GEO/STR : 01 méthode approche 2
              //      : ELU_GEO/STR : 10 méthode approche 3
              // bit 3    : ELU_ACC : 0 si utilisation de psi1,1
              //      : ELU_ACC : 1 si utilisation de psi2,1
              // bit 4    : 0 si utilisation des formules 6.10a et b
              //      : 1 si utilisation de la formule 6.10
              // bit 5    : 0 si valeur fréquence en cas d'accident
              //      : 1 si valeur quasi-permanente en cas d'accident
  GList        *elu_equ;   // Liste des combinaisons selon l'ELU EQU
  GList        *elu_str;   // ..
  GList        *elu_geo;   //
  GList        *elu_fat;   //
  GList        *elu_acc;   //
  GList        *elu_sis;   //
  GList        *els_car;   //
  GList        *els_freq;  //
  GList        *els_perm;  //
#ifdef ENABLE_GTK
  GtkListStore *list_el_desc;
#endif
} CombinaisonsEL;


#ifdef ENABLE_GTK
typedef struct __Comp_Gtk
{
  // Fenêtre
  GtkWidget *window;
  GtkWidget *main_grid;
  
  // Menu
  GtkWidget *menu;
  GtkWidget *menu_fichier;
  GtkWidget *menu_fichier_list;
  GtkWidget *menu_fichier_informations;
  GtkWidget *menu_fichier_quitter;
  GtkWidget *menu_affichage;
  GtkWidget *menu_affichage_list;
  GtkWidget *menu_affichage_vues;
  GtkWidget *menu_affichage_vues_list;
  GtkWidget *menu_affichage_xzy;
  GtkWidget *menu_affichage_xz_y;
  GtkWidget *menu_affichage_yzx;
  GtkWidget *menu_affichage_yz_x;
  GtkWidget *menu_affichage_xyz;
  GtkWidget *menu_affichage_xy_z;
  GtkWidget *menu_modelisation;
  GtkWidget *menu_modelisation_list;
  GtkWidget *menu_modelisation_noeud;
  GtkWidget *menu_modelisation_noeud_list;
  GtkWidget *menu_modelisation_noeud_appui;
  GtkWidget *menu_modelisation_noeud_ajout;
  GtkWidget *menu_modelisation_barres;
  GtkWidget *menu_modelisation_barres_list;
  GtkWidget *menu_modelisation_barres_section;
  GtkWidget *menu_modelisation_barres_materiau;
  GtkWidget *menu_modelisation_barres_relachement;
  GtkWidget *menu_modelisation_barres_ajout;
  GtkWidget *menu_modelisation_charges;
  GtkWidget *menu_modelisation_charges_list;
  GtkWidget *menu_modelisation_charges_actions;
  GtkWidget *menu_modelisation_charges_groupes;
  GtkWidget *menu_resultats;
  GtkWidget *menu_resultats_list;
  GtkWidget *menu_resultats_calculer;
  GtkWidget *menu_resultats_afficher;
} Comp_Gtk;


typedef struct __Gtk_m3d
{
  GtkWidget *drawing;
  gpointer   data;
  double     gdx, gdy, gdz;
  double     hbx, hby, hbz;
} Gtk_m3d;


typedef struct __List_Gtk
{             // Contient toutes les données pour l'interface graphique GTK+3
  Gtk_1990_Actions                      _1990_actions;   // pour l'Eurocode 0
  Gtk_1990_Groupes                      _1990_groupes;   // pour l'Eurocode 0
  Gtk_1990_Groupes_Options              _1990_groupes_options;   // pour l'Eurocode 0
  Gtk_EF_Charge_Noeud                   ef_charge_noeud;
  Gtk_EF_Charge_Barre_Ponctuelle        ef_charge_barre_ponctuelle;
  Gtk_EF_Charge_Barre_Repartie_Uniforme ef_charge_barre_repartie_uniforme;
  Gtk_m3d                               m3d;     // pour l'affichage graphique de la structure
  Gtk_Common_Informations               common_informations;
  Gtk_EF_Noeud                          ef_noeud;
  Gtk_EF_Barres                         ef_barres;
  Gtk_EF_Barres_Add                     ef_barres_add;
  Gtk_EF_Appuis                         ef_appuis;
  Gtk_EF_Sections                       ef_sections;
  Gtk_EF_Section_Rectangulaire          ef_section_rectangulaire;
  Gtk_EF_Section_T                      ef_section_T;
  Gtk_EF_Section_Carree                 ef_section_carree;
  Gtk_EF_Section_Circulaire             ef_section_circulaire;
  Gtk_EF_Section_Personnalisee          ef_section_personnalisee;
  Gtk_EF_Materiaux                      ef_materiaux;
  Gtk_1992_1_1_Materiaux                _1992_1_1_materiaux;
  Gtk_1993_1_1_Materiaux                _1993_1_1_materiaux;
  Gtk_EF_Relachements                   ef_relachements;
  Gtk_EF_Rapport                        ef_rapport;
  Gtk_EF_Resultats                      ef_resultats;
  Comp_Gtk                              comp;    // tous les composants grahpiques
} List_Gtk;
#endif


typedef struct __Analyse_Comm
{
  char *analyse;     // Contient le critère de l'analyse
  char *commentaire;   // Contient le commentaire de l'anlayse
  int   resultat;     // 0 : tout va bien,
              // 1 : attention mais problème non critique,
              // 2 : erreur critique.
} Analyse_Comm;


typedef struct __Modele
{
  GList *noeuds;     // Liste de tous les noeuds de la structure, que ce soit
             // les noeuds définis par l'utilisateur ou les noeuds
             // créés par la discrétisation des éléments.
  GList *appuis;     // Liste des types d'appuis
  GList *relachements; // Liste des types de relâchements des barres.
  
  GList *sections;
  GList *materiaux;
  GList *barres;
} Modele;


typedef struct __Calculs
{                 // Contient toutes les données nécessaires pour la réalisation
                // des calculs aux éléments finis et notamment les variables
                // utilisées par les librairies cholmod et umfpack :
  cholmod_common   Common; // Paramètres des calculs de la librairie cholmod.
  cholmod_common  *c;   // Pointeur vers Common
                
  int            **n_part; // Etabli une corrélation entre le degré de 
  int            **n_comp;  // liberté (x, y, z, rx, ry, rz) d'un noeud
                        // et sa position dans la matrice de rigidité
  // globale partielle et complète. Par partielle, il faut comprendre la matrice de rigidité
  // globale sans les lignes et les colonnes dont les déplacements sont connus ; cette même
  // matrice qui permet de déterminer le déplacement des noeuds. La matrice de rigidité
  // complète permet, sur la base du calcul des déplacements de déterminer les efforts aux
  // noeuds et d'en déduire les sollicitations le long des barres. La position de la ligne /
  // colonne dans la matrice se déterminera par la lecture de n_part[10][1]
  // pour, par exemple, la position du noeud dont le numéro est le 10 et comme degré de
  // liberté y. Si la valeur renvoyée est -1, cela signifie que le déplacement ou la rotation
  // est bloquée et que le degré de liberté ne figure pas dans la matrice de rigidité
  // partielle.
  
  // triplet contenant la matrice de rigidité
  cholmod_triplet *t_part; // Liste temporaire avant transformation
  cholmod_triplet *t_comp;  // en matrice sparse. Les éléments sont 
                           // ajoutés au fur et à mesure que la 
  // rigidité des barres est ajoutée dans la matrice de rigidité.
  
  unsigned int     t_part_en_cours; // Numéro du prochain triplet qui
  unsigned int     t_comp_en_cours;  // est à compléter.
  
  cholmod_sparse  *m_part; // Matrice de rigidité globale ne
                           // contenant pas les lignes et colonnes
                           // dont les déplacements sont connus
  cholmod_sparse  *m_comp;  // Matrice de rigidité complète.
  
  void            *numeric;      // Matrice partielle factorisée, utilisée dans
  int             *ap, *ai;      // tous les calculs lors de la résolution de
  double          *ax;         // chaque cas de charges.
  double           residu;        // Erreur non relative des réactions d'appuis.
} Calculs;


typedef struct __Adresse
{
  char        *departement;
  int          commune;
  char        *destinataire;
  char        *adresse;
  unsigned int code_postal;
  char        *ville;
} Adresse;


typedef struct __Parametres
{
  Norme         norme;
  Adresse       adresse;
  Type_Neige    neige;
  Type_Vent     vent;
  Type_Seisme   seisme;
#ifdef ENABLE_GTK
  GtkListStore *neige_desc;
  GtkListStore *vent_desc;
  GtkListStore *seisme_desc;
#endif
} Parametres;


typedef struct __Projet
{
  Parametres     parametres;
  GList         *actions;       // Liste des actions contenant chacune des charges
  GList         *niveaux_groupes;   // Compatibilités entres actions
  CombinaisonsEL combinaisons;     // Combinaisons conformes aux Eurocodes
  Modele         modele;       // Données du modèle de calcul
  Calculs        calculs;      // Données nécessaires aux calculs
#ifdef ENABLE_GTK
  List_Gtk       ui;       // Informations nécessaires pour l'interface graphique
#endif
} Projet;


Projet *projet_init            (Norme norme)
                                       __attribute__((__warn_unused_result__));
#ifdef ENABLE_GTK
gboolean projet_init_graphique (Projet *p)
                                       __attribute__((__warn_unused_result__));
#endif
gboolean projet_free           (Projet *p)
                                       __attribute__((__warn_unused_result__));

#endif
