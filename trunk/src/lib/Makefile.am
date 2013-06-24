DEFAULT_INCLUDES	=	-I../.. \
						-Icommon \
						-IEF \
						-I1990 \
						-I1992_1_1

AUTOMAKE_OPTIONS = subdir-objects

lib_LTLIBRARIES = libeurocodes.la

libeurocodes_la_LIBADD		=	@BLAS_LIBS@ -lgfortran @XML_LIBS@
libeurocodes_la_CFLAGS		=	$(AM_CFLAGS)
if ENABLE_GTK
libeurocodes_la_LIBADD		+=	@GTK_LIBS@
libeurocodes_la_CFLAGS		+=	@GTK_CFLAGS@
else
libeurocodes_la_CFLAGS      +=  @GLIB_CFLAGS@
endif

libeurocodes_la_LDFLAGS		= 	$(AM_LDFLAGS) -no-undefined -version-info 0:0:0

libeurocodes_la_CPPFLAGS	=	@XML_CPPFLAGS@

libeurocodes_la_SOURCES =	1990/1990_action.cpp \
							1990/1990_coef_psi.cpp \
							1990/1990_combinaisons.cpp \
							1990/1990_duree.cpp \
							1990/1990_groupe.cpp \
							1990/1990_ponderations.cpp \
							1992_1_1/1992_1_1_barres.c \
							1992_1_1/1992_1_1_materiaux.c \
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
							EF/EF_noeuds.c \
							EF/EF_relachement.c \
							EF/EF_rigidite.c \
							EF/EF_resultat.c \
							EF/EF_sections.c \
							EF/EF_verif.c

if ENABLE_GTK
libeurocodes_la_SOURCES +=	common/common_m3d.cc \
							1990/1990_gtk_groupes.cpp \
							1990/1990_gtk_groupes_options.cpp \
							1990/1990_gtk_actions.cpp \
							1992_1_1/1992_1_1_gtk_materiaux.c \
							common/common_gtk.c \
							common/common_gtk_informations.c \
							common/ressources.c \
							EF/EF_gtk_charge_noeud.c \
							EF/EF_gtk_charge_barre_ponctuelle.c \
							EF/EF_gtk_charge_barre_repartie_uniforme.c \
							EF/EF_gtk_noeud.c \
							EF/EF_gtk_appuis.c \
							EF/EF_gtk_barres.c \
							EF/EF_gtk_barres_add.c \
							EF/EF_gtk_calculs.c \
							EF/EF_gtk_sections.c \
							EF/EF_gtk_materiaux.c \
							EF/EF_gtk_section_rectangulaire.c \
							EF/EF_gtk_section_T.c \
							EF/EF_gtk_section_carree.c \
							EF/EF_gtk_section_circulaire.c \
							EF/EF_gtk_rapport.c \
							EF/EF_gtk_relachement.c \
							EF/EF_gtk_resultats.c

endif

libeurocodes_ladir = $(includedir)/2lgc
libeurocodes_la_HEADERS =	1990/1990_action.hpp \
							1990/1990_coef_psi.hpp \
							1990/1990_combinaisons.hpp \
							1990/1990_duree.hpp \
							1990/1990_groupe.hpp \
							1990/1990_ponderations.hpp \
							1992_1_1/1992_1_1_barres.h \
							1992_1_1/1992_1_1_materiaux.h \
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
							EF/EF_noeuds.h \
							EF/EF_relachement.h \
							EF/EF_rigidite.h \
							EF/EF_resultat.h \
							EF/EF_sections.h \
							EF/EF_verif.h

if ENABLE_GTK
libeurocodes_la_HEADERS +=	common/common_m3d.hpp \
							1990/1990_gtk_groupes.hpp \
							1990/1990_gtk_groupes_options.hpp \
							1990/1990_gtk_actions.hpp \
							1992_1_1/1992_1_1_gtk_materiaux.h \
							common/common_gtk.h \
							common/common_gtk_informations.h \
							common/ressources.h \
							EF/EF_gtk_charge_noeud.h \
							EF/EF_gtk_charge_barre_ponctuelle.h \
							EF/EF_gtk_charge_barre_repartie_uniforme.h \
							EF/EF_gtk_noeud.h \
							EF/EF_gtk_appuis.h \
							EF/EF_gtk_barres.h \
							EF/EF_gtk_barres_add.h \
							EF/EF_gtk_calculs.h \
							EF/EF_gtk_sections.h \
							EF/EF_gtk_materiaux.h \
							EF/EF_gtk_section_rectangulaire.h \
							EF/EF_gtk_section_T.h \
							EF/EF_gtk_section_carree.h \
							EF/EF_gtk_section_circulaire.h \
							EF/EF_gtk_rapport.h \
							EF/EF_gtk_relachement.h \
							EF/EF_gtk_resultats.h
endif

common/common_projet.c: common/ressources.h common/ressources.c

common/ressources.h: ../../files/ressources.xml common/ressources.c
	glib-compile-resources --target=$@ --sourcedir=../../files ../../files/ressources.xml --c-name _2lgc --generate-header --manual-register
common/ressources.c: ../../files/ressources.xml $(wildcard ../../files/ui/*.ui)
	glib-compile-resources --target=$@ --sourcedir=../../files ../../files/ressources.xml --c-name _2lgc --generate-source --manual-register

CLEANFILES = *.gcno *.gcda common/ressources.c common/ressources.h
