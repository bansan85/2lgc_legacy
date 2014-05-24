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

#include <list>
#include <vector>
#include <string>

#ifdef ENABLE_GTK
#include <gtk/gtk.h>
#endif

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


/**
 * \enum Filtres
 * \brief Pour la fenêtre des résultats.
 */
typedef enum
{
  /// Tous les noeuds ou barres sont affichés.
  FILTRE_AUCUN = 0,
  /// Seuls les noeuds avec appui sont affichés.
  FILTRE_NOEUD_APPUI
} Filtres;


/**
 * \enum Colonne_Resultats
 * \brief Type des colonnes possibles dans le treeview des résultats.
 */
typedef enum
{
  /// Numéro des noeuds.
  COLRES_NUM_NOEUDS = 0,
  /// Numéro des barres.
  COLRES_NUM_BARRES,
  /// Coordonnées des noeuds, x.
  COLRES_NOEUDS_X,
  /// Coordonnées des noeuds, y.
  COLRES_NOEUDS_Y,
  /// Coordonnées des noeuds, z.
  COLRES_NOEUDS_Z,
  /// Réaction d'appui, Fx.
  COLRES_REACTION_APPUI_FX,
  /// Réaction d'appui, Fy.
  COLRES_REACTION_APPUI_FY,
  /// Réaction d'appui, Fz.
  COLRES_REACTION_APPUI_FZ,
  /// Réaction d'appui, Mx.
  COLRES_REACTION_APPUI_MX,
  /// Réaction d'appui, My.
  COLRES_REACTION_APPUI_MY,
  /// Réaction d'appui, Mz.
  COLRES_REACTION_APPUI_MZ,
  /// Déplacement, Ux.
  COLRES_DEPLACEMENT_UX,
  /// Déplacement, Uy.
  COLRES_DEPLACEMENT_UY,
  /// Déplacement, Uz.
  COLRES_DEPLACEMENT_UZ,
  /// Déplacement, Rx.
  COLRES_DEPLACEMENT_RX,
  /// Déplacement, Ry.
  COLRES_DEPLACEMENT_RY,
  /// Déplacement, Rz.
  COLRES_DEPLACEMENT_RZ,
  /// Longueur des barres.
  COLRES_BARRES_LONGUEUR,
  /// Dessin de l'effort normal N.
  COLRES_BARRES_PIXBUF_N,
  /// Dessin de l'effort tranchant en y.
  COLRES_BARRES_PIXBUF_TY,
  /// Dessin de l'effort tranchant en z.
  COLRES_BARRES_PIXBUF_TZ,
  /// Dessin du moment de torsion.
  COLRES_BARRES_PIXBUF_MX,
  /// Dessin du moment fléchissant en y.
  COLRES_BARRES_PIXBUF_MY,
  /// Dessin du moment fléchissant en z.
  COLRES_BARRES_PIXBUF_MZ,
  /// Points caractéristiques de l'effort normal N.
  COLRES_BARRES_DESC_N,
  /// Points caractéristiques de l'effort tranchant en y.
  COLRES_BARRES_DESC_TY,
  /// Points caractéristiques de l'effort tranchant en z.
  COLRES_BARRES_DESC_TZ,
  /// Points caractéristiques du moment de torsion.
  COLRES_BARRES_DESC_MX,
  /// Points caractéristiques du moment fléchissant en y.
  COLRES_BARRES_DESC_MY,
  /// Points caractéristiques du moment fléchissant en z.
  COLRES_BARRES_DESC_MZ,
  /// Equation de l'effort normal N.
  COLRES_BARRES_EQ_N,
  /// Equation de l'effort tranchant en y.
  COLRES_BARRES_EQ_TY,
  /// Equation de l'effort tranchant en z.
  COLRES_BARRES_EQ_TZ,
  /// Equation du moment de torsion.
  COLRES_BARRES_EQ_MX,
  /// Equation du moment fléchissant en y.
  COLRES_BARRES_EQ_MY,
  /// Equation du moment fléchissant en z.
  COLRES_BARRES_EQ_MZ,
  /// Dessin de la déformation des barres, Ux.
  COLRES_DEF_PIXBUF_UX,
  /// Dessin de la déformation des barres, Uy.
  COLRES_DEF_PIXBUF_UY,
  /// Dessin de la déformation des barres, Uz.
  COLRES_DEF_PIXBUF_UZ,
  /// Dessin de la déformation des barres, Rx.
  COLRES_DEF_PIXBUF_RX,
  /// Dessin de la déformation des barres, Ry.
  COLRES_DEF_PIXBUF_RY,
  /// Dessin de la déformation des barres, Rz.
  COLRES_DEF_PIXBUF_RZ,
  /// Points caractéristiques de la déformation des barres, Ux.
  COLRES_DEF_DESC_UX,
  /// Points caractéristiques de la déformation des barres, Uy.
  COLRES_DEF_DESC_UY,
  /// Points caractéristiques de la déformation des barres, Uz.
  COLRES_DEF_DESC_UZ,
  /// Points caractéristiques de la déformation des barres, Rx.
  COLRES_DEF_DESC_RX,
  /// Points caractéristiques de la déformation des barres, Ry.
  COLRES_DEF_DESC_RY,
  /// Points caractéristiques de la déformation des barres, Rz.
  COLRES_DEF_DESC_RZ,
  /// Déformation des barres, Ux.
  COLRES_DEF_UX,
  /// Déformation des barres, Uy.
  COLRES_DEF_UY,
  /// Déformation des barres, Uz.
  COLRES_DEF_UZ,
  /// Déformation des barres, Rx.
  COLRES_DEF_RX,
  /// Déformation des barres, Ry.
  COLRES_DEF_RY,
  /// Déformation des barres, Rz.
  COLRES_DEF_RZ
} Colonne_Resultats;


/**
 * \enum Type_Element
 * \brief Liste des différents éléments de type de barres.
 */
typedef enum
{
  /// Poteau béton.
  BETON_ELEMENT_POTEAU = 0,
  /// Poutre béton.
  BETON_ELEMENT_POUTRE
} Type_Element;


/**
 * \enum Norme
 * \brief Spécifie la norme et l'annexe nationnale à utiliser.
 */
typedef enum
{
  /// Norme européenne sans application des annexes nationales.
  NORME_EU = 0,
  /// Annexe nationale française.
  NORME_FR
} Norme;


/**
 * \enum Type_Groupe_Combinaison
 * \brief La méthode de combinaison des actions d'un groupe.
 */
typedef enum
{
  GROUPE_COMBINAISON_OR = 0,
  GROUPE_COMBINAISON_XOR,
  GROUPE_COMBINAISON_AND
} Type_Groupe_Combinaison;


/**
 * \enum Action_Categorie
 * \brief Le type d'action.
 */
typedef enum
{
  /// Poids propre.
  ACTION_POIDS_PROPRE = 0,
  /// Précontrainte.
  ACTION_PRECONTRAINTE,
  /// Variable.
  ACTION_VARIABLE,
  /// Accidentelle.
  ACTION_ACCIDENTELLE,
  /// Sismique.
  ACTION_SISMIQUE,
  /// Eaux souterraines.
  ACTION_EAUX_SOUTERRAINES,
  /// Action inconnue.
  ACTION_INCONNUE
} Action_Categorie;


/**
 * \enum Type_EF_Appui
 * \brief Type d'appui.
 */
typedef enum
{
  /// Appui libre.
  EF_APPUI_LIBRE = 0,         // Déplacement libre
  /// Appui bloqué.
  EF_APPUI_BLOQUE         // Déplacement bloqué
} Type_EF_Appui;


/**
 * \enum EF_Relachement_Type
 * \brief Type de relâchement.
 */
typedef enum
{
  /// Relâchement spécial pour indiquer que le relâchement n'est pas modifié.
  EF_RELACHEMENT_UNTOUCH = 0,
  /// Relâchement bloché.
  EF_RELACHEMENT_BLOQUE,
  /// Relâchement libre.
  EF_RELACHEMENT_LIBRE,
  /// Relâchement élastique linéaire.
  EF_RELACHEMENT_ELASTIQUE_LINEAIRE
} EF_Relachement_Type;


/**
 * \enum Type_Section
 * \brief Type de section.
 */
typedef enum
{
  /// Section rectangulaire.
  SECTION_RECTANGULAIRE = 0,
  /// Section en T.
  SECTION_T,
  /// Section carrée.
  SECTION_CARREE,
  /// Section circulaire.
  SECTION_CIRCULAIRE,
  /// Section personnalisée.
  SECTION_PERSONNALISEE
} Type_Section;


/**
 * \enum Charge_Type
 * \brief Type de la charge.
 */
typedef enum
{
  /// Charge nodale.
  CHARGE_NOEUD = 0,
  /// Charge ponctuelle sur barre.
  CHARGE_BARRE_PONCTUELLE,
  /// Charge uniformément répartie sur barre.
  CHARGE_BARRE_REPARTIE_UNIFORME
} Charge_Type;


/**
 * \enum Type_Noeud
 * \brief Type de noeud.
 */
typedef enum
{
  /// Noeud libre.
  NOEUD_LIBRE = 0,
  /// Noeud le long d'une barre.
  NOEUD_BARRE
} Type_Noeud;


/**
 * \enum Type_Materiau
 * \brief Le type du matériau.
 */
typedef enum
{
  /// Béton.
  MATERIAU_BETON = 0,
  /// Acier.
  MATERIAU_ACIER
} Type_Materiau;


/**
 * \enum Type_Neige
 * \brief Zonage de la neige selon l'EC1.
 */
typedef enum
{
  /// Zone A1.
  NEIGE_A1 = 0,
  /// Zone A2.
  NEIGE_A2,
  /// Zone B1.
  NEIGE_B1,
  /// Zone B2.
  NEIGE_B2,
  /// Zone C1.
  NEIGE_C1,
  /// Zone C2.
  NEIGE_C2,
  /// Zone D.
  NEIGE_D,
  /// Zone E.
  NEIGE_E
} Type_Neige;


/**
 * \enum Type_Vent
 * \brief Zonage du vent selon l'EC1.
 */
typedef enum
{
  /// Zone 1.
  VENT_1 = 0,
  /// Zone 2.
  VENT_2,
  /// Zone 3.
  VENT_3,
  /// Zone 4.
  VENT_4
} Type_Vent;


/**
 * \enum Type_Seisme
 * \brief Zonage sismique selon l'EC8.
 */
typedef enum
{
  /// Zone 1.
  SEISME_1 = 0,
  /// Zone 2.
  SEISME_2,
  /// Zone 3.
  SEISME_3,
  /// Zone 4.
  SEISME_4,
  /// Zone 5.
  SEISME_5
} Type_Seisme;


/**
 * \enum Type_Flottant
 * \brief Type de flottant.
 */
typedef enum
{
  /// Nombre définit par l'ordinateur (et potentiellement à virgule infinie).
  FLOTTANT_ORDINATEUR = 0,
  /// Nombre définit par l'utilisateur (et donc à virgule finie).
  FLOTTANT_UTILISATEUR
} Type_Flottant;


/**
 * \struct Flottant
 * \brief Défini un nombre flottant. Il peut être soit défini par
 *        l'utilisateur, soit par calcul / ordinateur.
 */
typedef struct
{
  /// Nombre sans unité (pour l'instant).
  double        d;
  /// Ordinateur ou utilisateur ?
  Type_Flottant type;
} Flottant;


/**
 * \struct EF_Point
 * \brief Défini un point en 3D.
 */
typedef struct
{
  /// Position dans l'axe global x.
  Flottant x;
  /// Position dans l'axe global y.
  Flottant y;
  /// Position dans l'axe global z.
  Flottant z;
} EF_Point;


/**
 * \struct Materiau_Beton
 * \brief Défini un matériau béton.
 */
typedef struct
{
  // Caractéristique du matériau béton conformément à EN 1992_1_1, Tableau 3.1
  /// Résistance caractéristique en compression sur cylindre en Pa.
  Flottant fck;
  /// Résistance caractéristique en compression sur cube en Pa.
  Flottant fckcube;
  /// Valeur moyenne de la résistance en compression en Pa.
  Flottant fcm;
  /// Valeur moyenne de la résistance en traction directe en Pa.
  Flottant fctm;
  /// Résistance caractéristique en traction directe fractile 5% en Pa.
  Flottant fctk_0_05;
  /// Résistance caractéristique en traction directe fractile 95% en Pa.
  Flottant fctk_0_95;
  
  /// Déformation relative en compression au point 1.
  Flottant ec1;
  /// Déformation relative ultime en compression au point 1.
  Flottant ecu1;
  /// Déformation relative en compression au point 2.
  Flottant ec2;
  /// Déformation relative ultime en compression au point 2.
  Flottant ecu2;
  /// Exposant utilisé dans le calcul de sigmac.
  Flottant n;
  /// Déformation relative en compression au point 3.
  Flottant ec3;
  /// Déformation relative ultime en compression au point 3.
  Flottant ecu3;
  
  /// Module Young en Pa.
  Flottant ecm;
  /// Coefficient de poisson.
  Flottant nu;
} Materiau_Beton;


/**
 * \struct Materiau_Acier
 * \brief Défini un matériau acier.
 */
typedef struct
{
  /// Limite d’élasticité en Pa.
  Flottant fy;
  /// Résistance à la traction en Pa.
  Flottant fu;
  
  /// Module Young en Pa.
  Flottant e;
  /// Coefficient de poisson.
  Flottant nu;
} Materiau_Acier;


/**
 * \struct EF_Materiau
 * \brief Défini un matériau.
 */
typedef struct
{
  /// Le type de matériau (Béton ou acier).
  Type_Materiau type;
  /// Son nom.
  std::string   nom;
  /// Les informations du matériau.
  /**
   * Soit Materiau_Acier ou Materiau_Beton.
   */
  void         *data;
#ifdef ENABLE_GTK
  /// La ligne du treeview dans la fenêtre Matériaux.
  GtkTreeIter  Iter_fenetre;
  /// La liste des sections pour la fenêtre Barre.
  GtkTreeIter  Iter_liste;
#endif
} EF_Materiau;


/**
 * \struct Section_T
 * \brief Donnée d'une section en T.
 *        La section rectangulaire est intégrée dans la section en T.
 *        Il convient simplement de mettre la hauteur dans hauteur_retombee et
 *        la largeur dans largeur_retombee.
 *        largeur_table et hauteur_table sont mis à 0.
 */
typedef struct
{
  /// Largeur de la table en m.
  Flottant largeur_table;
  /// Largeur de la retombée en m.
  Flottant largeur_retombee;
  /// Hauteur de la table en m.
  Flottant hauteur_table;
  /// Hauteur de la retombée en m.
  Flottant hauteur_retombee;
} Section_T;


/**
 * \struct Section_Circulaire
 * \brief Donnée d'une section circulaire pleine.
 */
typedef struct
{
  /// Son diamètre en m.
  Flottant diametre;
} Section_Circulaire;


/**
 * \struct Section_Personnalisee
 * \brief Donnée d'une section personnalisée.
 */
typedef struct
{
  /// Sa description (plus longue que son nom).
  std::string description;
  /// Inertie de torsion J en m4.
  Flottant    j;
  /// Inertie I selon l'axe y en m4.
  Flottant    iy;
  /// Inertie I selon l'axe z en m4.
  Flottant    iz;
  /// vy en m.
  Flottant    vy;
  /// vy' en m.
  Flottant    vyp;
  /// vz en m.
  Flottant    vz;
  /// vz' en m.
  Flottant    vzp;
  /// Surface en m2.
  Flottant    s;
  /// La forme de la section.
  /**
   * Une forme est une liste de forme élémentaire. Chaque forme élémentaire
   * une liste de points. Cette forme est automatiquement refermée. Il n'est
   * pas nécessaire que le premier point soit égal au dernier.
   */
   std::list <std::list <EF_Point *> *> forme;
} Section_Personnalisee;


/**
 * \struct Section
 * \brief Définition d'une section quelconque.
 */
typedef struct
{
  /// Le type de la section.
  Type_Section type;
  /// Son nom.
  std::string  nom;
#ifdef ENABLE_GTK
  /// La ligne du treeview dans la fenêtre Action.
  GtkTreeIter  Iter_fenetre;
  /// La liste des sections pour la fenêtre Barre.
  GtkTreeIter  Iter_liste;
#endif
  /// Les informations de la section.
  /**
   * Soit Section_Te, Section_Circulaire ou Section_Personnalisee.
   */
  void        *data;
} Section;


#ifdef ENABLE_GTK
#define ENTETE_ITER GtkTreeIter Iter;
#else
#define ENTETE_ITER
#endif


#define ENTETE_CHARGES \
  Charge_Type type; \
  ENTETE_ITER \
  std::string nom;


/**
 * \struct Charge
 * \brief Définition d'une charge quelconque.
 */
typedef struct
{
  /// L'entête contenant le type, son nom et Iter si affichage graphique
  ENTETE_CHARGES
  /// Les données de la charge.
  /**
   * Soit Charge_Noeud, Charge_Barre_Ponctuelle ou
   * Charge_Barre_Repartie_Uniforme.
   */
  void *data;
} Charge;


/**
 * \struct Analyse_Comm
 * \brief Un rapport est constitué de lignes d'analyse.
 */
typedef struct
{
  /// Contient la description de l'analyse
  std::string analyse;
  /// Le résultat de l'analyse.
  /** - 0 : tout va bien.
   *  - 1 : attention mais problème non critique.
   *  - 2 : erreur critique.
   */
  uint8_t     resultat;
  /// Contient le commentaire de l'analyse en cas d'erreur ou d'avertissement.
  std::string commentaire;
} Analyse_Comm;


/**
 * \struct Troncon
 * \brief Définition de la structure Troncon qui contient une fonction avec un
 *        domaine de validité précis. La fonction mathématique est définie par
 *        x0 + x1*x + x2*x^2 + x3*x^3 + x4*x^4 + x5*x^5 + x6*x^6.
 */
typedef struct
{
  /// Début du tronçon de validité de la fonction.
  double debut_troncon;
  /// Fin du tronçon de validité de la fonction.
  double fin_troncon;
  /// Coefficient x0.
  double x0;
  /// Coefficient x1.
  double x1;
  /// Coefficient x2.
  double x2;
  /// Coefficient x3.
  double x3;
  /// Coefficient x4.
  double x4;
  /// Coefficient x5.
  double x5;
  /// Coefficient x6.
  double x6;
} Troncon;


/**
 * \struct Fonction
 * \brief Une fonction décrit une courbe sous forme d'une liste de troncons.
 *        Les fonctions n'étant pas forcément continues le long de la barre
 *        (par exemple de part et d'une charge ponctuelle). Il est nécessaire
 *        de définir plusieurs tronçons avec pour chaque tronçon sa fonction.
 */
typedef struct
{
  /// Tableau dynamique contenant les fonctions continues par tronçon.
  std::list <Troncon *> t;
} Fonction;


#ifdef ENABLE_GTK
#define ENTETE_ITER_GROUPE GtkTreeIter Iter_groupe;
#else
#define ENTETE_ITER_GROUPE
#endif


#define ENTETE_ACTION_ET_GROUPE \
  std::string nom; \
  ENTETE_ITER_GROUPE


/**
 * \brief Objet définissant une action avec sa liste de charges.
 */
typedef struct
{
  /// Le nom plus si nécessaire un Iter_groupe pour l'affichage graphique.
  ENTETE_ACTION_ET_GROUPE;
  /// La description de type est donnée par #_1990_action_bat_txt_type.
  uint8_t         type;
  /// Flag utilisé temporairement.
  uint8_t         action_predominante : 1;
  /// Liste des charges (ponctuelle, répartie, …).
  std::list <Charge *> charges;
  /// Valeur de combinaison d'une charge variable.
  Flottant        psi0;
  /// Valeur fréquente d'une action variable.
  Flottant        psi1;
  /// Valeur quasi-permanente d'une action variable.
  Flottant        psi2;
  /// Déplacement des nœuds de la structure sous l'effet de l'action.
  cholmod_sparse *deplacement;
  /// Efforts équivalents des charges dans les nœuds de la structure.
  cholmod_sparse *forces;
  /// Efforts (y compris les réactions d'appui) dans les nœuds.
  cholmod_sparse *efforts_noeuds;
#ifdef ENABLE_GTK
  /// Iter utilisé par le treeview dans la fenêtre Action. 
  GtkTreeIter     Iter_fenetre;
  /// Iter utilisé par les menus des fenêtres Charges.
  GtkTreeIter     Iter_liste;
#endif
  /// Les 6 fonctions (N, Ty, Tz, Mx, My, Mz) par barre.
  std::vector <Fonction *> efforts[6];
  /// Les 3 déformations x, y, z pour chaque barre dans le repère local.
  std::vector <Fonction *> deformation[3]; 
  /// Les 3 rotations rx, ry, rz pour chaque barre dans le repère local.
  std::vector <Fonction *> rotation[3];
} Action;


#ifdef ENABLE_GTK
/**
 * \struct Gtk_EF_Resultats_Tableau
 * \brief La structure de données du tableau d'affichage des résultats.
 *        Un par onglet.
 */
typedef struct
{
  /// Le treeview de la fenêtre résultat.
  GtkTreeView       *treeview;
  /// Le model du treeview.
  GtkListStore      *list_store;
  
  /// La première valeur du tableau est le nombre de colonnes à afficher.
  Colonne_Resultats *col_tab;
  /// Le filtre des résultats.
  Filtres            filtre;
  /// Titre de l'onglet.
  std::string        nom;
} Gtk_EF_Resultats_Tableau;


/**
 * \struct Gtk_1990_Actions
 * \brief Gestion des actions et des charges selon l'Eurocode 0.
 */
typedef struct
{
  /// Le builder.
  GtkBuilder       *builder;
  /// La fenêtre window.
  GtkWidget        *window;
  
  /// Le composant 1990_actions_tree_store_action.
  GtkTreeStore     *tree_store_actions;
  /// Le composant 1990_actions_treeview_action.
  GtkTreeView      *tree_view_actions;
  /// Le composant GtkTreeSelection de tree_view_actions.
  GtkTreeSelection *tree_select_actions;
  /// Liste des types d'action pour la fenêtre Actions.
  GtkListStore     *choix_type_action;
  
  /// Menu contenant la liste des différents types d'action.
  GtkMenu          *type_action;
  /// Liste contenant l'ensemble des items du menu menu_type_list_action.
  /**
   * Est uniquement utilisé pour trouver quel menu a été cliqué parmi ceux de
   * type_action.
   */
  std::list <GtkWidget *> items_type_action;
  
  /// Le composant .
  GtkTreeStore     *tree_store_charges;
  /// Le composant .
  GtkTreeSelection *tree_select_charges;
  /// Le composant .
  GtkTreeView      *tree_view_charges;
  
  /// Menu contenant tous les types de charge développée dans le programme.
  /**
   * Est utilisé uniquement par la fenêtre Action permettant de créer de
   * nouvelles charges.
   */
  GtkMenu          *type_charges;
  /// Contient la liste des actions compatible avec les composants GtkComboBox.
  /**
   * Cette liste est utilisée dans les fenêtres permettant de créer ou
   * d'éditer des charges afin de définir dans quelle action elles
   * s'appliqueront.
   */
  GtkListStore     *liste;
} Gtk_1990_Actions;


/**
 * \struct Gtk_1990_Groupes
 * \brief Gestion des groupes selon l'Eurocode 0.
 */
typedef struct
{
  /// Le builder.
  GtkBuilder       *builder;
  /// La fenêtre window.
  GtkWidget        *window;
   
  /// Le composant 1990_actions_tree_store_etat.
  GtkTreeStore     *tree_store_etat;
  /// Le composant 1990_actions_tree_store_dispo.
  GtkTreeStore     *tree_store_dispo;
  
  /// Le composant 1990_groupes_treeview_select_etat.
  GtkTreeSelection *tree_select_etat;
  /// Le composant 1990_groupes_treeview_etat.
  GtkTreeView      *tree_view_etat;
  /// Le composant 1990_groupes_treeview_select_dispo.
  GtkTreeSelection *tree_select_dispo;
  /// Le composant 1990_groupes_spin_button_niveaux.
  GtkWidget        *spin_button_niveau;
  /// Le composant 1990_groupes_treeview_dispo.
  GtkTreeView      *tree_view_dispo;
  
  /// Pour le support du DnD.
  GtkWidget        *drag_from;
  
  /// Le composant 1990_charges_toolbar_etat_and.
  GtkToolItem      *item_groupe_and;
  /// Le composant 1990_charges_toolbar_etat_or.
  GtkToolItem      *item_groupe_or;
  /// Le composant 1990_charges_toolbar_etat_xor.
  GtkToolItem      *item_groupe_xor;
  /// Le composant 1990_charges_toolbar_dispo_ajout.
  GtkToolItem      *item_ajout_dispo;
  /// Le composant 1990_charges_toolbar_dispo_ajout_tout.
  GtkToolItem      *item_ajout_tout_dispo;
  /// Le composant 1990_charges_toolbar_etat_suppr.
  GtkToolItem      *item_groupe_suppr;
} Gtk_1990_Groupes;


/**
 * \struct Gtk_1990_Groupes_Options
 * \brief Gestion des options de groupe selon l'Eurocode 0.
 */
typedef struct
{
  /// Le builder.
  GtkBuilder *builder;
  /// La fenêtre window.
  GtkWidget  *window;
} Gtk_1990_Groupes_Options;


/**
 * \struct Gtk_EF_Charge_Noeud
 * \brief Ajout ou modification d'un charge nodale.
 */
typedef struct
{
  /// Le builder.
  GtkBuilder *builder;
  /// La fenêtre window.
  GtkWidget  *window;
  GtkWidget  *combobox_charge;
  
  /// La charge à modifier ou NULL si ajout d'une nouvelle section.
  Charge     *charge;
  /// L'action contenant la charge ou NULL si ajout d'une nouvelle section.
  Action     *action;
} Gtk_EF_Charge_Noeud;


/**
 * \struct Gtk_EF_Charge_Barre_Ponctuelle
 * \brief Ajout ou modification d'une charge ponctuelle sur barre.
 */
typedef struct
{
  /// Le builder.
  GtkBuilder  *builder;
  /// La fenêtre window.
  GtkWidget   *window;
  
  /// La charge à modifier ou NULL si ajout d'une nouvelle section.
  Charge      *charge;
  /// L'action contenant la charge ou NULL si ajout d'une nouvelle section.
  Action      *action;
  
  /// Le composant EF_charge_barre_ponct_combo_box_charge.
  GtkComboBox *combobox_charge;
} Gtk_EF_Charge_Barre_Ponctuelle;


/**
 * \struct Gtk_EF_Charge_Barre_Repartie_Uniforme
 * \brief Ajout ou modification d'une charge répartie sur barre.
 */
typedef struct
{
  /// Le builder.
  GtkBuilder  *builder;
  /// La fenêtre window.
  GtkWidget   *window;
  
  /// La charge à modifier ou NULL si ajout d'une nouvelle section.
  Charge      *charge;
  /// L'action contenant la charge ou NULL si ajout d'une nouvelle section.
  Action      *action;
  
  /// Le composant EF_charge_barre_rep_uni_combo_box_charge.
  GtkComboBox *combobox_charge;
  /// Le composant EF_charge_barre_rep_uni_radio_local.
  GtkWidget   *check_button_repere_local;
  /// Le composant EF_charge_barre_rep_uni_radio_global.
  GtkWidget   *check_button_repere_global;
  /// Le composant EF_charge_barre_rep_uni_check_projection.
  GtkWidget   *check_button_projection;
} Gtk_EF_Charge_Barre_Repartie_Uniforme;


/**
 * \struct Gtk_m3d
 * \brief Affichage graphique de la structure.
 */
typedef struct
{
  /// Le composant graphique.
  GtkWidget *drawing;
  /// Les données diverses et variées.
  gpointer   data;
} Gtk_m3d;


/**
 * \struct Gtk_Common_Informations
 * \brief Saisie des informations du projet.
 */
typedef struct
{
  /// Le builder.
  GtkBuilder   *builder;
  /// La fenêtre window.
  GtkWidget    *window;
  
  /// Le composant model_completion.
  GtkListStore *model_completion;
  
  /// Département du projet.
  wchar_t      *departement;
  /// Commune du projet.
  uint32_t      commune;
} Gtk_Common_Informations;


/**
 * \struct Gtk_EF_Noeud
 * \brief Ajout ou modification des noeuds de la structure.
 */
typedef struct
{
  /// Le builder.
  GtkBuilder   *builder;
  /// La fenêtre window.
  GtkWidget    *window;
  
  /// Le composant EF_noeuds_notebook.
  GtkWidget    *notebook;
  /// Le composant EF_noeuds_treestore_noeuds_libres.
  GtkTreeStore *tree_store_libre;
  /// Le composant EF_noeuds_treestore_noeuds_intermediaires.
  GtkTreeStore *tree_store_barre;
} Gtk_EF_Noeud;


/**
 * \struct Gtk_EF_Barres
 * \brief Modification des barres de la structure.
 */
typedef struct
{
  /// Le builder.
  GtkBuilder   *builder;
  /// La fenêtre window.
  GtkWidget    *window;
  
  /// Liste des types de barres pour la fenêtre graphique des barres.
  GtkListStore *liste_types;
} Gtk_EF_Barres;


/**
 * \struct Gtk_EF_Barres_Add
 * \brief Ajout des barres de la structure.
 */
typedef struct
{
  /// Le builder.
  GtkBuilder   *builder;
  /// La fenêtre window.
  GtkWidget    *window;
} Gtk_EF_Barres_Add;


/**
 * \struct Gtk_EF_Appuis
 * \brief Ajout ou modification des appuis de la structure.
 */
typedef struct
{
  /// Le builder.
  GtkBuilder   *builder;
  /// La fenêtre window.
  GtkWidget    *window;
  
  /// Le composant EF_appuis_treestore.
  GtkTreeStore *appuis;
  /// Liste des aoouis pour la fenêtre graphique des noeuds.
  GtkListStore *liste_appuis;
  /// Liste des types d'aoouis pour la fenêtre graphique des noeuds.
  GtkListStore *liste_type_appui;
} Gtk_EF_Appuis;


/**
 * \struct Gtk_EF_Sections
 * \brief Gestion des sections des barres.
 */
typedef struct
{
  /// Le builder.
  GtkBuilder   *builder;
  /// La fenêtre window.
  GtkWidget    *window;
  
  /// Le composant EF_sections_treestore.
  GtkTreeStore *sections;
  /// Liste des sections pour la fenêtre graphique des barres.
  GtkListStore *liste_sections;
} Gtk_EF_Sections;


/**
 * \struct Gtk_EF_Section_Rectangulaire
 * \brief Ajout ou modification des sections rectangulaires.
 */
typedef struct
{
  /// Le builder.
  GtkBuilder *builder;
  /// La fenêtre window.
  GtkWidget  *window;
  
  /// La section à modifier ou NULL si ajout d'une nouvelle section.
  Section    *section;
} Gtk_EF_Section_Rectangulaire;


/**
 * \struct Gtk_EF_Section_T
 * \brief Ajout ou modification des sections en T.
 */
typedef struct
{
  /// Le builder.
  GtkBuilder *builder;
  /// La fenêtre window.
  GtkWidget  *window;
  
  /// La section à modifier ou NULL si ajout d'une nouvelle section.
  Section    *section;
} Gtk_EF_Section_T;


/**
 * \struct Gtk_EF_Section_Carree
 * \brief Ajout ou modification des sections carrées.
 */
typedef struct
{
  /// Le builder.
  GtkBuilder *builder;
  /// La fenêtre window.
  GtkWidget  *window;
  
  /// La section à modifier ou NULL si ajout d'une nouvelle section.
  Section    *section;
} Gtk_EF_Section_Carree;


/**
 * \struct Gtk_EF_Section_Circulaire
 * \brief Ajout ou modification des sections circulaires.
 */
typedef struct
{
  /// Le builder.
  GtkBuilder *builder;
  /// La fenêtre window.
  GtkWidget  *window;
  
  /// La section à modifier ou NULL si ajout d'une nouvelle section.
  Section    *section;
} Gtk_EF_Section_Circulaire;


/**
 * \struct Gtk_EF_Section_Personnalisee
 * \brief Ajout ou modification des sections personnalisées.
 */
typedef struct
{
  /// Le builder.
  GtkBuilder   *builder;
  /// La fenêtre window.
  GtkWidget    *window;
  
  /// La section à modifier ou NULL si ajout d'une nouvelle section.
  Section      *section;
  /// Liste des points de la forme de la section.
  GtkTreeModel *model;
  
  /// Ne libère pas la liste des noeuds à la fermeture.
  bool          keep;
} Gtk_EF_Section_Personnalisee;


/**
 * \struct Gtk_EF_Materiaux
 * \brief Gestion des matériaux des barres.
 */
typedef struct
{
  /// Le builder.
  GtkBuilder   *builder;
  /// La fenêtre window.
  GtkWidget    *window;
  
  /// Le composant EF_materiaux_treestore.
  GtkTreeStore *materiaux;
  /// Liste des matériaux pour la fenêtre graphique des barres.
  GtkListStore *liste_materiaux;
} Gtk_EF_Materiaux;


/**
 * \struct Gtk_1992_1_1_Materiaux
 * \brief Ajout ou modification des matériaux béton selon l'Eurocode 2.
 */
typedef struct
{
  /// Le builder.
  GtkBuilder  *builder;
  /// La fenêtre window.
  GtkWidget   *window;
  
  /// Le matériau à modifier ou NULL si ajout d'un nouveau matériau EC2.
  EF_Materiau *materiau;
} Gtk_1992_1_1_Materiaux;


/**
 * \struct Gtk_1993_1_1_Materiaux
 * \brief Ajout ou modification des matériaux acier selon l'Eurocode 3.
 */
typedef struct
{
  /// Le builder.
  GtkBuilder  *builder;
  /// La fenêtre window.
  GtkWidget   *window;
  
  /// Le matériau à modifier ou NULL si ajout d'un nouveau matériau EC3.
  EF_Materiau *materiau;
} Gtk_1993_1_1_Materiaux;


/**
 * \struct Gtk_EF_Relachements
 * \brief Ajout ou modification des relâchements de la structure.
 */
typedef struct
{
  /// Le builder.
  GtkBuilder   *builder;
  /// La fenêtre window.
  GtkWidget    *window;
  
  /// Le composant EF_relachements_treestore.
  GtkTreeStore *relachements;
  /// Liste des relâchements pour la fenêtre graphique des barres.
  GtkListStore *liste_relachements;
} Gtk_EF_Relachements;


/**
 * \struct Gtk_EF_Rapport
 * \brief Affichage d'un rapport d'analyse.
 */
typedef struct
{
  /// Le builder.
  GtkBuilder   *builder;
  /// La fenêtre window.
  GtkWidget    *window;
  
  /// Le composant EF_rapport_treestore.
  GtkListStore *liste;
  /// Le rapport à afficher défini par la fonction EF_gtk_rapport.
  std::list <Analyse_Comm *> rapport;
} Gtk_EF_Rapport;


/**
 * \struct Gtk_EF_Resultats.
 * \brief Affichage des résultats du calcul.
 */
typedef struct
{
  /// Le builder.
  GtkBuilder  *builder;
  /// La fenêtre window.
  GtkWidget   *window;
  
  /// Le composant EF_resultats_combobox.
  GtkComboBox *combobox;
  /// Le composant EF_resultats_combo_box_cas.
  GtkComboBox *combobox_cas;
  /// Le composant EF_resultats_combo_box_ponderations.
  GtkComboBox *combobox_ponderations;
  /// Le composant EF_resultats_notebook.
  GtkNotebook *notebook;
  
  /// Liste de tableaux Gtk_EF_Resultats_Tableau.
  std::list <Gtk_EF_Resultats_Tableau *> tableaux;
} Gtk_EF_Resultats;


/**
 * \struct Comp_Gtk
 * \brief Données nécessaire pour la fenêtre graphique principale.
 */
typedef struct
{
  /// Fenêtre.
  GtkWidget *window;
  /// Grille principale.
  GtkWidget *main_grid;
  
  /// Menu.
  GtkWidget *menu;
  /// Menu Fichier.
  GtkWidget *menu_fichier;
  /// Menu liste Fichier.
  GtkWidget *menu_fichier_list;
  /// Menu Fichier Informations.
  GtkWidget *menu_fichier_informations;
  /// Menu Fichier Quitter.
  GtkWidget *menu_fichier_quitter;
  /// Menu Affichage.
  GtkWidget *menu_affichage;
  /// Menu liste Affichage.
  GtkWidget *menu_affichage_list;
  /// Menu Affichage Vues.
  GtkWidget *menu_affichage_vues;
  /// Menu liste Affichage Vues.
  GtkWidget *menu_affichage_vues_list;
  /// Menu Affichage Vues XZ+Y.
  GtkWidget *menu_affichage_xzy;
  /// Menu Affichage Vues XZ-Y.
  GtkWidget *menu_affichage_xz_y;
  /// Menu Affichage Vues YZ+X.
  GtkWidget *menu_affichage_yzx;
  /// Menu Affichage Vues YZ-X.
  GtkWidget *menu_affichage_yz_x;
  /// Menu Affichage Vues XY+Z.
  GtkWidget *menu_affichage_xyz;
  /// Menu Affichage Vues XY-Z.
  GtkWidget *menu_affichage_xy_z;
  /// Menu Modélisation.
  GtkWidget *menu_modelisation;
  /// Menu liste Modélisation.
  GtkWidget *menu_modelisation_list;
  /// Menu Modélisation Noeuds.
  GtkWidget *menu_modelisation_noeud;
  /// Menu liste Modélisation Noeuds.
  GtkWidget *menu_modelisation_noeud_list;
  /// Menu Modélisation Noeuds Appui.
  GtkWidget *menu_modelisation_noeud_appui;
  /// Menu Modélisation Noeuds Ajout.
  GtkWidget *menu_modelisation_noeud_ajout;
  /// Menu Modélisation Barres.
  GtkWidget *menu_modelisation_barres;
  /// Menu liste Modélisation Barres.
  GtkWidget *menu_modelisation_barres_list;
  /// Menu Modélisation Barres Section.
  GtkWidget *menu_modelisation_barres_section;
  /// Menu Modélisation Barres Matériaux.
  GtkWidget *menu_modelisation_barres_materiau;
  /// Menu Modélisation Barres Relâchement.
  GtkWidget *menu_modelisation_barres_relachement;
  /// Menu Modélisation Barres Ajout.
  GtkWidget *menu_modelisation_barres_ajout;
  /// Menu Modélisation Charges.
  GtkWidget *menu_modelisation_charges;
  /// Menu liste Modélisation Charges.
  GtkWidget *menu_modelisation_charges_list;
  /// Menu Modélisation Charges Actions.
  GtkWidget *menu_modelisation_charges_actions;
  /// Menu Modélisation Charges Groupes.
  GtkWidget *menu_modelisation_charges_groupes;
  /// Menu Résultats.
  GtkWidget *menu_resultats;
  /// Menu liste Résultats.
  GtkWidget *menu_resultats_list;
  /// Menu Résultats Calculer.
  GtkWidget *menu_resultats_calculer;
  /// Menu Résultats Afficher.
  GtkWidget *menu_resultats_afficher;
} Comp_Gtk;


/**
 * \struct List_Gtk
 * \brief Contient toutes les données nécessaires pour l'interface graphique
 *        GTK+3 de toutes les fenêtres.
 */
typedef struct
{
  /// Gestion des actions et des charges selon l'Eurocode 0.
  Gtk_1990_Actions                      _1990_actions;
  /// Gestion des groupes selon l'Eurocode 0.
  Gtk_1990_Groupes                      _1990_groupes;
  /// Gestion des options de groupe selon l'Eurocode 0.
  Gtk_1990_Groupes_Options              _1990_groupes_options;
  /// Ajout ou modification d'un charge nodale.
  Gtk_EF_Charge_Noeud                   ef_charge_noeud;
  /// Ajout ou modification d'une charge ponctuelle sur barre.
  Gtk_EF_Charge_Barre_Ponctuelle        ef_charge_barre_ponctuelle;
  /// Ajout ou modification d'une charge répartie sur barre.
  Gtk_EF_Charge_Barre_Repartie_Uniforme ef_charge_barre_repartie_uniforme;
  /// Affichage graphique de la structure.
  Gtk_m3d                               m3d;
  /// Saisie des informations du projet.
  Gtk_Common_Informations               common_informations;
  /// Ajout ou modification des noeuds de la structure.
  Gtk_EF_Noeud                          ef_noeud;
  /// Modification des barres de la structure.
  Gtk_EF_Barres                         ef_barres;
  /// Ajout des barres de la structure.
  Gtk_EF_Barres_Add                     ef_barres_add;
  /// Ajout ou modification des appuis de la structure.
  Gtk_EF_Appuis                         ef_appuis;
  /// Gestion des sections des barres.
  Gtk_EF_Sections                       ef_sections;
  /// Ajout ou modification des sections rectangulaires.
  Gtk_EF_Section_Rectangulaire          ef_section_rectangulaire;
  /// Ajout ou modification des sections en T.
  Gtk_EF_Section_T                      ef_section_T;
  /// Ajout ou modification des sections carrées.
  Gtk_EF_Section_Carree                 ef_section_carree;
  /// Ajout ou modification des sections circulaires.
  Gtk_EF_Section_Circulaire             ef_section_circulaire;
  /// Ajout ou modification des sections personnalisées.
  Gtk_EF_Section_Personnalisee          ef_section_personnalisee;
  /// Gestion des matériaux des barres.
  Gtk_EF_Materiaux                      ef_materiaux;
  /// Ajout ou modification des matériaux béton selon l'Eurocode 2.
  Gtk_1992_1_1_Materiaux                _1992_1_1_materiaux;
  /// Ajout ou modification des matériaux acier selon l'Eurocode 3.
  Gtk_1993_1_1_Materiaux                _1993_1_1_materiaux;
  /// Ajout ou modification des relâchements de la structure.
  Gtk_EF_Relachements                   ef_relachements;
  /// Affichage d'un rapport d'analyse.
  Gtk_EF_Rapport                        ef_rapport;
  /// Affichage des résultats du calcul.
  Gtk_EF_Resultats                      ef_resultats;
  /// Fenêtre graphique principale.
  Comp_Gtk                              comp;
} List_Gtk;
#endif


/**
 * \struct EF_Appui
 * \brief Données définissant un appui.
 */
typedef struct
{
  /// Nom de l'appui.
  std::string   nom;
  /// Degré de liberté de la direction x.
  Type_EF_Appui ux;
  /// Données complémentaires si nécessaire pour ux.
  /**
   * NULL si ux vaut EF_APPUI_LIBRE ou EF_APPUI_BLOQUE.
   */
  void         *ux_donnees;
  /// Degré de liberté de la direction y.
  Type_EF_Appui uy;
  /// Données complémentaires si nécessaire pour uy.
  void         *uy_donnees;
  /// Degré de liberté de la direction z.
  Type_EF_Appui uz;
  /// Données complémentaires si nécessaire pour uz.
  void         *uz_donnees;
  /// Degré de liberté en rotation autour de l'axe x.
  Type_EF_Appui rx;
  /// Données complémentaires si nécessaire pour ry.
  void         *rx_donnees;
  /// Degré de liberté en rotation autour de l'axe y.
  Type_EF_Appui ry;
  /// Données complémentaires si nécessaire pour ry.
  void         *ry_donnees;
  /// Degré de liberté en rotation autour de l'axe z.
  Type_EF_Appui rz;
  /// Données complémentaires si nécessaire pour rz.
  void         *rz_donnees;
#ifdef ENABLE_GTK
  /// Affichage graphique dans le treeview de la fenêtre Appui.
  GtkTreeIter   Iter_fenetre;
  /// Pour la liste du menu déroulant du treeview de la fenêtre Barre.
  GtkTreeIter   Iter_liste;
#endif          
} EF_Appui;


/**
 * \struct EF_Noeud
 * \brief Défini un noeud par ces coordonnées et son appui.
 */
typedef struct
{
  /// Numéro de la barre.
  /**
   * Est attribué automatiquement. Le numéro est attribué automatiquement comme
   * celui du numéro du dernier noeud + 1.
   */
  uint32_t    numero;
  /// Type de noeud : Libre ou le long d'une barre.
  Type_Noeud  type;
  /// Contient les données permettant de récupérer les coordonnées
  /**
   * EF_Noeud_Libre ou EF_Noeud_Barre.
   */
  void       *data;
  /// Défini l'appui du noeud. Peut-être NULL si le noeud est libre.
  EF_Appui   *appui;
#ifdef ENABLE_GTK
  /// Affichage graphique dans le treeview de la fenêtre Noeud.
  GtkTreeIter Iter;
#endif
} EF_Noeud;


/**
 * \struct EF_Relachement_Donnees_Elastique_Lineaire
 * \brief Données du relâchement élastique linéaire.
 */
typedef struct
{
  /// La raideur doit être indiquée en N.m/rad.
  Flottant raideur;
} EF_Relachement_Donnees_Elastique_Lineaire;


/**
 * \struct EF_Relachement
 * \brief Définition d'un relâchement en rotation d'une barre
 */
typedef struct
{
  /// Nom du relâchement.
  std::string         nom;
  /// Type de relachement en rotation en rx en début de barre.
  EF_Relachement_Type rx_debut;
  /// Paramètres complémentaires au relachement à rx_debut.
  /**
   * NULL si rx_debut vaut EF_RELACHEMENT_BLOQUE ou EF_RELACHEMENT_LIBRE.
   * Si rx_debut vaut EF_RELACHEMENT_ELASTIQUE_LINEAIRE, une structure via
   * malloc de EF_Relachement_Donnees_Elastique_Lineaire est utilisée.
   */
  void               *rx_d_data;
  /// Type de relachement en rotation en ry en début de barre.
  EF_Relachement_Type ry_debut;
  /// Paramètres complémentaires au relachement à ry_debut.
  void               *ry_d_data;
  /// Type de relachement en rotation en rz en début de barre.
  EF_Relachement_Type rz_debut;
  /// Paramètres complémentaires au relachement à rz_debut.
  void               *rz_d_data;
  /// Type de relachement en rotation en rx en fin de barre.
  EF_Relachement_Type rx_fin;
  /// Paramètres complémentaires au relachement à rx_fin.
  void               *rx_f_data;
  /// Type de relachement en rotation en ry en fin de barre.
  EF_Relachement_Type ry_fin;
  /// Paramètres complémentaires au relachement à ry_fin.
  void               *ry_f_data;
  /// Type de relachement en rotation en rz en fin de barre.
  EF_Relachement_Type rz_fin;
  /// Paramètres complémentaires au relachement à rz_fin.
  void               *rz_f_data;
#ifdef ENABLE_GTK
  /// Affichage graphique dans le treeview de la fenêtre Relâchement.
  GtkTreeIter         Iter_fenetre;
  /// Pour la liste du menu déroulant du treeview de la fenêtre Barre.
  GtkTreeIter         Iter_liste;
#endif
} EF_Relachement;


/**
 * \struct Barre_Info_EF
 * \brief Diverses données de calculs de la barre.
 */
typedef struct
{
  /// Matrice de rigidite locale.
  cholmod_sparse *m_rig_loc;
  
  /// Paramètre de souplesse a de la poutre selon l'axe y.
  double ay;
  /// Paramètre de souplesse b de la poutre selon l'axe y.
  double by;
  /// Paramètre de souplesse c de la poutre selon l'axe y.
  double cy;
  /// Paramètre de souplesse a de la poutre selon l'axe z.
  double az;
  /// Paramètre de souplesse b de la poutre selon l'axe z.
  double bz;
  /// Paramètre de souplesse c de la poutre selon l'axe z.
  double cz;
  
  /// Inverse de la raideur au début du tronçon en rx.
  double kAx;
  /// Inverse de la raideur au début du tronçon en ry.
  double kAy;
  /// Inverse de la raideur au début du tronçon en rz.
  double kAz;
  /// Inverse de la raideur à la fin du tronçon en rx.
  double kBx;
  /// Inverse de la raideur à la fin du tronçon en ry.
  double kBy;
  /// Inverse de la raideur à la fin du tronçon en rz.
  double kBz;
} Barre_Info_EF;


/**
 * \struct EF_Barre
 * \brief Données d'une barre.
 */
typedef struct
{
  /// Numéro de la barre.
  /**
   * Est attribué automatiquement. Le numéro est attribué automatiquement comme
   * celui du numéro de la dernière barre + 1.
   */
  uint32_t        numero;
  /// Le type d'élément.
  Type_Element    type;
  /// La section de la barre.
  Section        *section;
  /// Le matériau de la barre.
  EF_Materiau    *materiau;
  /// Le noeud définissant le début de la barre.
  EF_Noeud       *noeud_debut;
  /// Le noeud définissant le fin de la barre.
  EF_Noeud       *noeud_fin;
  /// Angle de rotation de la barre autour de l'axe x local.
  Flottant        angle;
  /// Relâchement de la barre. NULL si aucun.
  EF_Relachement *relachement;
  /// Nombre de noeuds intermédiaires.
  uint16_t        discretisation_element;
  /// Noeuds intermédiaires le long de la barre.
  /**
   * Lorsque des noeuds intermédiaires sont ajoutés, ils le sont dans
   * p->modele.noeuds. La liste de noeuds nds_inter contient uniquement un
   * pointeur vers chaque noeud de la liste principale.
   */
  std::list <EF_Noeud *> nds_inter;
  /// Une info de calcul par élément discrétisé.
  std::vector <Barre_Info_EF> info_EF;
#ifdef ENABLE_GTK
  /// L'affichage graphique dans la fenêtre graphique.
  GtkTreeIter     Iter;
#endif
  /// Matrice de rotation de la barre.
  cholmod_sparse *m_rot;
  /// Matrice de rotation transposée de la barre.
  cholmod_sparse *m_rot_t;
} EF_Barre;


/**
 * \struct EF_Noeud_Libre
 * \brief Noeud dont la position est dans le repère XYZ. Sa position peut-être
 *        relative avec un autre noeud.
 */
typedef struct
{
  /// Position en x.
  Flottant  x;
  /// Position en y.
  Flottant  y;
  /// Position en z.
  Flottant  z;
  /// Le noeud relatif vaut NULL si la position est absolue.
  EF_Noeud *relatif;
} EF_Noeud_Libre;


/**
 * \struct EF_Noeud_Barre
 * \brief Noeud intermédiaire dont la position est le long d'une barre.
 */
typedef struct
{
  /// La barre en question.
  EF_Barre *barre;
  /// La position le long de la barre (entre 0. et 1.).
  Flottant  position_relative_barre;
} EF_Noeud_Barre;


/**
 * \struct Charge_Noeud
 * \brief Charge ponctuelle appliquée à un noeud.
 */
typedef struct
{
  /// Liste des noeuds où est appliquée la charge.
  std::list <EF_Noeud *> noeuds;
  /// Charge ponctuelle en N dans l'axe x.
  Flottant fx;
  /// Charge ponctuelle en N dans l'axe y.
  Flottant fy;
  /// Charge ponctuelle en N dans l'axe z.
  Flottant fz;
  /// Moment ponctuelle en N.m autour de l'axe x.
  Flottant mx;
  /// Moment ponctuelle en N.m autour de l'axe y.
  Flottant my;
  /// Moment ponctuelle en N.m autour de l'axe z.
  Flottant mz;
} Charge_Noeud;


/**
 * \struct Charge_Barre_Ponctuelle
 * \brief Charge ponctuelle appliquée sur une barre.
 */
typedef struct
{
  /// Liste des barres où est appliquée la charge.
  std::list <EF_Barre *> barres;
  /// Charge appliquée dans le repère local.
  uint8_t  repere_local : 1;
  ///Position en m du début de la charge par rapport au début de la barre.
  Flottant position;
  /// Charge ponctuelle en N dans l'axe x.
  Flottant fx;
  /// Charge ponctuelle en N dans l'axe y.
  Flottant fy;
  /// Charge ponctuelle en N dans l'axe z.
  Flottant fz;
  /// Moment ponctuelle en N.m autour de l'axe x.
  Flottant mx;
  /// Moment ponctuelle en N.m autour de l'axe y.
  Flottant my;
  /// Moment ponctuelle en N.m autour de l'axe z.
  Flottant mz;
} Charge_Barre_Ponctuelle;


/**
 * \struct Charge_Barre_Repartie_Uniforme
 * \brief Charge uniformément répartie appliquée sur une barre.
 */
typedef struct
{
  /// Liste des barres où est appliquée la charge.
  std::list <EF_Barre *> barres;
  /// Charge appliquée dans le repère local.
  uint8_t  repere_local : 1;
  /// La charge est projetée dans le repère local.
  /**
   * Cette option est incompatible avec repere_local == TRUE.
   */
  uint8_t  projection : 1;
  /// Position en m du début de la charge par rapport au début de la barre.
  Flottant a;
  /// Position en m de la fin de la charge par rapport à la fin de la barre.
  Flottant b;
  /// Charge linéaire en N/m dans l'axe x.
  Flottant fx;
  /// Charge linéaire en N/m dans l'axe y.
  Flottant fy;
  /// Charge linéaire en N/m dans l'axe z.
  Flottant fz;
  /// Moment linéaire en N.m/m autour de l'axe x.
  Flottant mx;
  /// Moment linéaire en N.m/m autour de l'axe y.
  Flottant my;
  /// Moment linéaire en N.m/m autour de l'axe z.
  Flottant mz;
} Charge_Barre_Repartie_Uniforme;


/**
 * \struct Ponderation
 * \brief Une pondération est calculée comme le produit de l'action par sa
 *        pondération et, si nécessaire, par le coefficient psi.
 */
typedef struct
{
  /// Action à manipuler.
  Action *action;
  /// La pondération à appliquer.
  double  ponderation;
  /// Le coefficient psi à multiplier avec l'action.
  /**
   * psi vaut toujours -1 s'il ne s'agit pas d'une action variable sinon, psi
   * vaut 0, 1 ou 2 pour que cette action soit pondérée avec soit psi0, psi1 ou
   * psi2 en combinaison d'accompagnement.
   */
  int8_t  psi;
  /// Ce flag vaut 1 si cette action est une action dominante.
  /**
   * 0 s'il s'agit d'une action d'accompagnement.
   */
  uint8_t flags : 1;
} Ponderation;


/**
 * \struct Combinaison
 * \brief Contient une action et un flag pour savoir si les actions variables
 *        doivent être considérées comme prédominante. Les combinaisons sont
 *        ensuite regroupées en liste et servent ensuite à générer des
 *        pondérations.
 */
typedef struct
{
  /// L'action.
  Action *action;
  /// Le flag vaut 1 si les actions variables sont prédominantes.
  uint8_t flags : 1;
} Combinaison;


/**
 * \struct Groupe
 * \brief Contient une liste d'actions ou de groupes avec la méthode pour les
 *        combiner (AND, OR ou XOR).
 */
typedef struct
{
  /// Le nom plus si nécessaire un Iter_groupe pour l'affichage graphique.
  ENTETE_ACTION_ET_GROUPE
  /// Le type de combinaisons des éléments.
  Type_Groupe_Combinaison type_combinaison;
  /// Contient des Action si le groupe est au niveau 0, des Groupe sinon.
  std::list <void *> elements;
  /// Combinaisons temporaires.
  /**
   * Ces combinaisons sont utilisées pour générer les groupes du niveau
   * supérieur jusqu'au dernier.
   */
  std::list <std::list <Combinaison *> *> tmp_combinaison;
#ifdef ENABLE_GTK
  /// Pour préserver l'affichage graphique lors d'un changement de niveau.
  /**
   * Cela permet de conserver les groupes développés.
   */
  uint8_t                 Iter_expand : 1;
#endif
} Groupe;


/**
 * \struct Niveau_Groupe
 * \brief Contient une liste de groupe. Si le niveau vaut 0, chaque groupe
 *        contiendra une liste d'action. Si le niveau est supérieur à 0, chaque
 *        groupe contiendra une liste de groupe du niveau inférieur.
 */
typedef struct
{
  /// La liste.
  std::list <Groupe *> groupes;
} Niveau_Groupe;


/**
 * \struct PonderationsEL
 * \brief Paramètres de combinaisons avec leurs listes générées.
 *        Spécifie la méthode des combinaisons (E0,A1.3).
 */
typedef struct
{
  /// ELU_EQU : méthode 1 si 0, méthode 2 si 1.
  uint8_t elu_equ_methode : 1;
  /// ELU_GEO/STR : si 0 approche 1, 1 : approche 2 et 2 : approche 3.
  uint8_t elu_geo_str_methode : 2;
  /// ELU_ACC : 0 si psi1,1 et 1 si psi2,1.
  uint8_t elu_acc_psi : 1;
  /// si 0, utilisation des formules 6.10a et b. Si 1 alors formule 6.10.
  uint8_t form_6_10 : 1;
  
  /// Liste des pondérations selon l'ELU EQU
  std::list <std::list <Ponderation *> *> elu_equ;
  /// Liste des pondérations selon l'ELU STR
  std::list <std::list <Ponderation *> *> elu_str;
  /// Liste des pondérations selon l'ELU GEO
  std::list <std::list <Ponderation *> *> elu_geo;
  /// Liste des pondérations selon l'ELU FAT
  std::list <std::list <Ponderation *> *> elu_fat;
  /// Liste des pondérations selon l'ELU ACC
  std::list <std::list <Ponderation *> *> elu_acc;
  /// Liste des pondérations selon l'ELU SIS
  std::list <std::list <Ponderation *> *> elu_sis;
  /// Liste des pondérations selon l'ELS CAR
  std::list <std::list <Ponderation *> *> els_car;
  /// Liste des pondérations selon l'ELS FREQ
  std::list <std::list <Ponderation *> *> els_freq;
  /// Liste des pondérations selon l'ELS PERM
  std::list <std::list <Ponderation *> *> els_perm;
#ifdef ENABLE_GTK
  /// Liste graphique des combinaisons ou enveloppe pour la fenêtre résultat.
  GtkListStore *list_el_desc;
#endif
} PonderationsEL;


/**
 * \struct Modele
 * \brief Contient l'ensemble de la structure modélisée sans les actions et
 *        résultats.
 */
typedef struct
{
  /// Liste de tous les noeuds de la structure.
  /** Y compris les noeuds définis par l'utilisateur ou les noeuds créés par la
   * discrétisation des éléments.
   */
  std::list <EF_Noeud *>       noeuds;
  /// Liste des types d'appuis.
  std::list <EF_Appui *>       appuis;
  
  /// Liste des types de relâchements des barres.
  std::list <EF_Relachement *> relachements;
  /// Liste des sections des barres.
  std::list <Section *>        sections;
  /// Liste des matériaux des barres.
  std::list <EF_Materiau *>    materiaux;
  /// Liste des barres.
  std::list <EF_Barre *>       barres;
} Modele;


/**
 * \struct Calculs
 * \brief Contient toutes les données nécessaires pour la réalisation des
 *        calculs aux éléments finis et notamment les variables utilisées par
 *        les librairies cholmod et umfpack.
 * Établi une corrélation entre le degré de liberté (x, y, z, rx, ry, rz) d'un
 * noeud et sa position dans la matrice de rigidité globale partielle et
 * complète. Par partielle, il faut comprendre la matrice de rigidité globale
 * sans les lignes et les colonnes dont les déplacements sont connus ; cette
 * même matrice qui permet de déterminer le déplacement des noeuds. La matrice
 * de rigidité complète permet, sur la base du calcul des déplacements, de
 * déterminer les efforts aux noeuds et d'en déduire les sollicitations le long
 * des barres. La position de la ligne / colonne dans la matrice se déterminera
 * par la lecture de n_part[10][1] pour, par exemple, la position du noeud dont
 * le numéro est le 10 et comme degré de liberté y. Si la valeur renvoyée est
 * -1, cela signifie que le déplacement ou la rotation est bloquée et que le
 *  degré de liberté ne figure pas dans la matrice de rigidité partielle.
 */
typedef struct
{
  /// Paramètres des calculs de la librairie cholmod.
  cholmod_common   Common;
  /// Pointeur vers Common
  cholmod_common  *c;
                
  /// La position du noeud avec son degré de liberté dans la matrice partielle.
  /** Calcul : t_for_part->i[n_part[g_list_index (p->modele.noeuds, noeud)]
   *  [0à5]]. Dans cette matrice de rigidité partielle, n_part renvoie
   *  UINT32_MAX si le degré de liberté est bloqué.
   */
  uint32_t       **n_part;
  /// La position du noeud avec son degré de liberté dans la matrice complète.
  /** Calcul : t_for_part->i[n_part[g_list_index (p->modele.noeuds, noeud)]
   *  [0à5]].
   */
  uint32_t       **n_comp;
  
  /// Triplet contenant la matrice de rigidité.
  /** Variable temporaire avant transformation en matrice sparse. Les éléments
   * sont ajoutés au fur et à mesure que la rigidité des barres est ajoutée
   * dans la matrice de rigidité.
   */
  cholmod_triplet *t_part;
  /// Triplet contenant la matrice de rigidité complète.
  cholmod_triplet *t_comp;
  
  /// Numéro de la prochaine donnée du triplet t_part qui est à compléter.
  uint32_t         t_part_en_cours;
  /// Numéro de la prochaine donnée du triplet t_comp qui est à compléter.
  uint32_t         t_comp_en_cours;
  
  /// Matrice de rigidité partielle t_part en format sparse.
  cholmod_sparse  *m_part;
  /// Matrice de rigidité globale t_comp en format sparse.
  cholmod_sparse  *m_comp;
  
  /// Variable temporaire utilisée par la fonction umfpack_di_solve.
  /** Utilisée pour le calcul de la matrice partielle lors de la résolution de
   * chaque cas de charges.
   */
  void            *numeric;
  /// Pointeur vers la colonne de la matrice t_part.
  int             *ap; // NS
  /// La colonne j de la matrice est définie par Ai [(Ap [j]) … (Ap [j+1]-1)].
  int             *ai; // NS
  /// Le résultat. Ax [(Ap [j]) ... (Ap [j+1]-1)].
  double          *ax;
  
  /// Erreur non relative des réactions d'appuis.
  double           residu;
} Calculs;


/**
 * \struct Adresse
 * \brief Adresse conforme au fichier france_villes.csv.
 */
typedef struct
{
  /// Numéro du département : 1 à 976, y compris 2A et 2B.
  std::string departement;
  /// Numéro de la commune du département.
  uint32_t commune;
  /// Nom du destinataire ou du client.
  std::string destinataire;
  /// Adresse du projet (rue, immeuble, …).
  std::string adresse;
  /// Code postal du projet.
  uint32_t code_postal;
  /// Ville du projet.
  std::string ville;
} Adresse;


/**
 * \struct Parametres
 * \brief Contient les options de calculs du projet.
 */
typedef struct
{
  /// Norme de calculs à prendre en compte.
  Norme         norme;
  /// Adresse du projet.
  Adresse       adresse;
  /// Zonage de la neige.
  Type_Neige    neige;
  /// Zonage du vent.
  Type_Vent     vent;
  /// Zonage du séisme.
  Type_Seisme   seisme;
#ifdef ENABLE_GTK
  /// Liste contenant la description des zones de neige.
  GtkListStore *neige_desc;
  /// Liste contenant la description des zones de vent.
  GtkListStore *vent_desc;
  /// Liste contenant la description des zones de séisme.
  GtkListStore *seisme_desc;
#endif
} Parametres;


/**
 * \struct Projet
 * \brief Variable projet contenant toutes les informations du projet.
 */
typedef struct
{
  /// Paramètres de calculs.
  Parametres                   parametres;
  /// Liste des actions contenant chacunes des charges.
  std::list <Action *>        actions;
  /// Compatibilités entres actions.
  std::list <Niveau_Groupe *> niveaux_groupes;
  /// Pondérations conformes aux Eurocodes.
  PonderationsEL               ponderations;
  /// Données du modèle de calcul.
  Modele                       modele;
  /// Données nécessaires aux calculs.
  Calculs                      calculs;
#ifdef ENABLE_GTK
  /// Informations nécessaires pour l'interface graphique.
  List_Gtk                     ui;
#endif
} Projet;


Projet *projet_init            (Norme norme)
                                       __attribute__((__warn_unused_result__));
#ifdef ENABLE_GTK
bool     projet_init_graphique (Projet *p)
                                       __attribute__((__warn_unused_result__));
#endif
bool     projet_free           (Projet *p)
                                       __attribute__((__warn_unused_result__));

#endif

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
