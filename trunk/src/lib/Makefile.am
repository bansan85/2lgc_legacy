DEFAULT_INCLUDES	=	-I../.. \
						-Icommon \
						-IEF \
						-I1990 \
						-I1992_1_1

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

libeurocodes_la_SOURCES =	1990/1990_action.c \
							1990/1990_coef_psi.c \
							1990/1990_combinaisons.c \
							1990/1990_duree.c \
							1990/1990_groupe.c \
							1990/1990_ponderations.c \
							1992_1_1/1992_1_1_barres.c \
							1992_1_1/1992_1_1_materiaux.c \
							common/common_fonction.c \
							common/common_math.c \
							common/common_projet.c \
							common/common_selection.c \
							common/common_text.c \
							common/common_tooltip.c \
							EF/EF_appuis.c \
							EF/EF_calculs.c \
							EF/EF_charge.c \
							EF/EF_charge_barre_ponctuelle.c \
							EF/EF_charge_barre_repartie_uniforme.c \
							EF/EF_charge_noeud.c \
							EF/EF_noeuds.c \
							EF/EF_relachement.c \
							EF/EF_rigidite.c \
							EF/EF_sections.c

if ENABLE_GTK
libeurocodes_la_SOURCES +=	common/common_m3d.cc \
							1990/1990_gtk_groupes.c \
							1990/1990_gtk_groupes_options.c \
							1990/1990_gtk_actions.c \
							1992_1_1/1992_1_1_gtk_materiaux.c \
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
							EF/EF_gtk_relachement.c \
							EF/EF_gtk_resultats.c \
							common/common_gtk.c
endif

libeurocodes_ladir = $(includedir)/2lgc
libeurocodes_la_HEADERS =	1990/1990_action.h \
							1990/1990_coef_psi.h \
							1990/1990_combinaisons.h \
							1990/1990_duree.h \
							1990/1990_groupe.h \
							1990/1990_ponderations.h \
							1992_1_1/1992_1_1_barres.h \
							1992_1_1/1992_1_1_materiaux.h \
							common/common_erreurs.h \
							common/common_fonction.h \
							common/common_math.h \
							common/common_projet.h \
							common/common_selection.h \
							common/common_text.h \
							common/common_tooltip.h \
							EF/EF_appuis.h \
							EF/EF_calculs.h \
							EF/EF_charge.h \
							EF/EF_charge_barre_ponctuelle.h \
							EF/EF_charge_barre_repartie_uniforme.h \
							EF/EF_charge_noeud.h \
							EF/EF_noeuds.h \
							EF/EF_relachement.h \
							EF/EF_rigidite.h \
							EF/EF_sections.h

if ENABLE_GTK
libeurocodes_la_HEADERS +=	common/common_m3d.hpp \
							1990/1990_gtk_groupes.h \
							1990/1990_gtk_groupes_options.h \
							1990/1990_gtk_actions.h \
							1992_1_1/1992_1_1_gtk_materiaux.h \
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
							EF/EF_gtk_relachement.h \
							EF/EF_gtk_resultats.h \
							common/common_gtk.h
endif

CLEANFILES = *.gcno *.gcda
