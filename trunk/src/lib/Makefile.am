DEFAULT_INCLUDES	=	-I../.. \
						-Icommon \
						-IEF \
						-I1990 \
						-I1992_1_1

lib_LTLIBRARIES = libeurocodes.la

libeurocodes_la_LIBADD		=	-llist -lcholmod -lspqr @XML_LIBS@
libeurocodes_la_CFLAGS		=	$(AM_CFLAGS)
if ENABLE_GTK
libeurocodes_la_LIBADD		+=	@GTK_LIBS@
libeurocodes_la_CFLAGS		+=	@GTK_CFLAGS@
endif

libeurocodes_la_CPPFLAGS	=	@XML_CPPFLAGS@

libeurocodes_la_SOURCES =	1990/1990_actions.c \
							1990/1990_coef_psi.c \
							1990/1990_combinaisons.c \
							1990/1990_duree.c \
							1990/1990_groupes.c \
							1990/1990_gtk_groupes.c \
							1990/1990_ponderations.c \
							1992_1_1/1992_1_1_barres.c \
							1992_1_1/1992_1_1_materiaux.c \
							1992_1_1/1992_1_1_section.c \
							common/common_fonction.c \
							common/common_maths.c \
							common/common_projet.c \
							common/common_text.c \
							common/common_tooltip.c \
							EF/EF_appuis.c \
							EF/EF_calculs.c \
							EF/EF_charge_barre_ponctuelle.c \
							EF/EF_charge_noeud.c \
							EF/EF_noeud.c \
							EF/EF_relachement.c \
							EF/EF_rigidite.c

libeurocodes_ladir = $(includedir)/2lgc
libeurocodes_la_HEADERS =	1990/1990_actions.h \
							1990/1990_coef_psi.h \
							1990/1990_combinaisons.h \
							1990/1990_duree.h \
							1990/1990_groupes.h \
							1990/1990_gtk_groupes.h \
							1990/1990_ponderations.h \
							1992_1_1/1992_1_1_barres.h \
							1992_1_1/1992_1_1_materiaux.h \
							1992_1_1/1992_1_1_section.h \
							common/common_erreurs.h \
							common/common_fonction.h \
							common/common_maths.h \
							common/common_projet.h \
							common/common_text.h \
							common/common_tooltip.h \
							EF/EF_appuis.h \
							EF/EF_calculs.h \
							EF/EF_charge_barre_ponctuelle.h \
							EF/EF_charge_noeud.h \
							EF/EF_noeud.h \
							EF/EF_relachement.h \
							EF/EF_rigidite.h

