DEFAULT_INCLUDES	=	-I../.. \
						-Icommon \
						-IEF \
						-I1990 \
						-I1992_1_1 \
						-I1993_1_1

AUTOMAKE_OPTIONS = subdir-objects

lib_LTLIBRARIES = libeurocodes.la

libeurocodes_la_LIBADD		=	@BLAS_LIBS@ -lgfortran @XML_LIBS@
if ENABLE_GTK
libeurocodes_la_CFLAGS		=	$(AM_CFLAGS) @GTK_CFLAGS@
else
libeurocodes_la_CFLAGS		=	$(AM_CFLAGS) @GLIB_CFLAGS@
endif
libeurocodes_la_LDFLAGS		= $(AM_LDFLAGS) -no-undefined -version-info 0:0:0 -Wl,-O0
libeurocodes_la_CPPFLAGS	=	@XML_CPPFLAGS@
if ENABLE_GTK
libeurocodes_la_CPPFLAGS	+=	@GTK_CFLAGS@
endif
libeurocodes_ladir = $(includedir)/2lgc

libeurocodes_la_SOURCES 	=	1990/1990_action.c \
							1990/1990_coef_psi.c \
							1990/1990_combinaisons.c \
							1990/1990_duree.c \
							1990/1990_groupe.c \
							1990/1990_ponderations.c \
							1992_1_1/1992_1_1_barres.c \
							1992_1_1/1992_1_1_materiaux.c \
							1993_1_1/1993_1_1_materiaux.c \
							common/common_fonction.c \
							common/common_math.c \
							common/common_projet.c \
							common/common_selection.c \
							common/common_text.c \
							common/common_tooltip.c \
							common/common_ville.c \
							EF/EF_appuis.c \
							EF/EF_calculs.c \
							EF/EF_charge.c \
							EF/EF_charge_barre_ponctuelle.c \
							EF/EF_charge_barre_repartie_uniforme.c \
							EF/EF_charge_noeud.c \
							EF/EF_materiaux.c \
							EF/EF_noeuds.c \
							EF/EF_relachement.c \
							EF/EF_rigidite.c \
							EF/EF_resultat.c \
							EF/EF_sections.c \
							EF/EF_verif.c

libeurocodes_la_HEADERS =	1990/1990_action.h \
							1990/1990_action_private.h \
							1990/1990_coef_psi.h \
							1990/1990_combinaisons.h \
							1990/1990_duree.h \
							1990/1990_groupe.h \
							1990/1990_ponderations.h \
							1992_1_1/1992_1_1_barres.h \
							1992_1_1/1992_1_1_materiaux.h \
							1993_1_1/1993_1_1_materiaux.h \
							common/common_erreurs.h \
							common/common_fonction.h \
							common/common_math.h \
							common/common_projet.h \
							common/common_selection.h \
							common/common_text.h \
							common/common_tooltip.h \
							common/common_ville.h \
							EF/EF_appuis.h \
							EF/EF_calculs.h \
							EF/EF_charge.h \
							EF/EF_charge_barre_ponctuelle.h \
							EF/EF_charge_barre_repartie_uniforme.h \
							EF/EF_charge_noeud.h \
							EF/EF_materiaux.h \
							EF/EF_noeuds.h \
							EF/EF_relachement.h \
							EF/EF_rigidite.h \
							EF/EF_resultat.h \
							EF/EF_sections.h \
							EF/EF_verif.h

if ENABLE_GTK
DEFAULT_INCLUDES	+=	-Igtk
lib_LTLIBRARIES += libeurocodes-gtk.la
libeurocodes_gtk_la_LIBADD		=	@GTK_LIBS@
libeurocodes_gtk_la_CFLAGS		=	$(AM_CFLAGS) @GTK_CFLAGS@
libeurocodes_gtk_la_LDFLAGS		= 	$(AM_LDFLAGS) -no-undefined -version-info 0:0:0 -Wl,-O0
libeurocodes_gtk_la_CPPFLAGS	=	@XML_CPPFLAGS@ @GTK_CFLAGS@
libeurocodes_gtk_ladir = $(includedir)/2lgc

libeurocodes_gtk_la_SOURCES 	=	gtk/common_m3d.cc \
							gtk/1990_gtk_groupes.c \
							gtk/1990_gtk_groupes_options.c \
							gtk/1990_gtk_actions.c \
							gtk/1992_1_1_gtk_materiaux.c \
							gtk/1993_1_1_gtk_materiaux.c \
							gtk/common_gtk.c \
							gtk/common_gtk_informations.c \
							gtk/ressources.c \
							gtk/EF_gtk_charge_noeud.c \
							gtk/EF_gtk_charge_barre_ponctuelle.c \
							gtk/EF_gtk_charge_barre_repartie_uniforme.c \
							gtk/EF_gtk_noeud.c \
							gtk/EF_gtk_appuis.c \
							gtk/EF_gtk_barres.c \
							gtk/EF_gtk_barres_add.c \
							gtk/EF_gtk_calculs.c \
							gtk/EF_gtk_sections.c \
							gtk/EF_gtk_materiaux.c \
							gtk/EF_gtk_section_rectangulaire.c \
							gtk/EF_gtk_section_T.c \
							gtk/EF_gtk_section_carree.c \
							gtk/EF_gtk_section_circulaire.c \
							gtk/EF_gtk_section_personnalisee.c \
							gtk/EF_gtk_rapport.c \
							gtk/EF_gtk_relachement.c \
							gtk/EF_gtk_resultats.c

libeurocodes_gtk_la_HEADERS =	gtk/common_m3d.hpp \
							gtk/1990_gtk_groupes.h \
							gtk/1990_gtk_groupes_options.h \
							gtk/1990_gtk_actions.h \
							gtk/1992_1_1_gtk_materiaux.h \
							gtk/1993_1_1_gtk_materiaux.h \
							gtk/common_gtk.h \
							gtk/common_gtk_informations.h \
							gtk/ressources.h \
							gtk/EF_gtk_charge_noeud.h \
							gtk/EF_gtk_charge_barre_ponctuelle.h \
							gtk/EF_gtk_charge_barre_repartie_uniforme.h \
							gtk/EF_gtk_noeud.h \
							gtk/EF_gtk_appuis.h \
							gtk/EF_gtk_barres.h \
							gtk/EF_gtk_barres_add.h \
							gtk/EF_gtk_calculs.h \
							gtk/EF_gtk_sections.h \
							gtk/EF_gtk_materiaux.h \
							gtk/EF_gtk_section_rectangulaire.h \
							gtk/EF_gtk_section_T.h \
							gtk/EF_gtk_section_carree.h \
							gtk/EF_gtk_section_circulaire.h \
							gtk/EF_gtk_section_personnalisee.h \
							gtk/EF_gtk_rapport.h \
							gtk/EF_gtk_relachement.h \
							gtk/EF_gtk_resultats.h
endif

if ENABLE_GTK
common/common_projet.c: gtk/ressources.h gtk/ressources.c

gtk/ressources.h: ../../files/ressources.xml gtk/ressources.c
	glib-compile-resources --target=$@ --sourcedir=../../files ../../files/ressources.xml --c-name _2lgc --generate-header --manual-register
gtk/ressources.c: ../../files/ressources.xml $(wildcard ../../files/ui/*.ui)
	glib-compile-resources --target=$@ --sourcedir=../../files ../../files/ressources.xml --c-name _2lgc --generate-source --manual-register
endif

CLEANFILES = *.gcno *.gcda gtk/ressources.c gtk/ressources.h

cppcheck :
	cppcheck --inconclusive --enable=all $(DEFAULT_INCLUDES) $(libeurocodes_la_SOURCES) $(libeurocodes_gtk_la_SOURCES) &> ../../cppcheck.log
	@sed 's/^.*scanf without field.*$$//g' -i ../../cppcheck.log

kernel_sparse :
	find . -name "*.c" -exec sparse {} @GTK_CFLAGS@ $(DEFAULT_INCLUDES) \; &> ../../sparse.log
