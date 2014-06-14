DEFAULT_INCLUDES					=		-I../.. \
															-I.

AUTOMAKE_OPTIONS					= 	subdir-objects

lib_LTLIBRARIES						= 	libeurocodes.la

libeurocodes_la_LIBADD		=		@BLAS_LIBS@ -lgfortran @XML_LIBS@
if ENABLE_GTK
libeurocodes_la_CXXFLAGS	=		$(AM_CXXFLAGS) @GTK_CFLAGS@
else
libeurocodes_la_CXXFLAGS	=		$(AM_CXXFLAGS)
endif
libeurocodes_la_LDFLAGS		= 	$(AM_LDFLAGS) -version-info 0:0:0 -Wl,-O0 -no-undefined
libeurocodes_la_CPPFLAGS	=		@XML_CPPFLAGS@
if ENABLE_GTK
libeurocodes_la_CPPFLAGS	+=	@GTK_CFLAGS@
endif

libeurocodes_ladir 				= 	$(includedir)/2lgc

libeurocodes_la_SOURCES 	=		CProjet.cpp \
															CCalculs.cpp

libeurocodes_la_HEADERS 	=		$(wildcard *.hpp)

$(libeurocodes_la_SOURCES) : 	uml

uml :
	$(MAKE) -C ../../files/uml


if ENABLE_GTK
DEFAULT_INCLUDES							+=	-Igtk
lib_LTLIBRARIES 							+=	libeurocodes-gtk.la
libeurocodes_gtk_la_LIBADD		=		@GTK_LIBS@
libeurocodes_gtk_la_CXXFLAGS	=		$(AM_CXXFLAGS) @GTK_CFLAGS@
libeurocodes_gtk_la_LDFLAGS		= 	$(AM_LDFLAGS) -version-info 0:0:0 -Wl,-O0 -no-undefined
libeurocodes_gtk_la_CPPFLAGS	=		@XML_CPPFLAGS@ @GTK_CFLAGS@
libeurocodes_gtk_ladir 				= 	$(includedir)/2lgc

libeurocodes_gtk_la_SOURCES 	=		gtk/1990_gtk_groupes.cpp \
							gtk/1990_gtk_groupes_options.cpp \
							gtk/1990_gtk_actions.cpp \
							gtk/1992_1_1_gtk_materiaux.cpp \
							gtk/1993_1_1_gtk_materiaux.cpp \
							gtk/common_gtk.cpp \
							gtk/common_gtk_informations.cpp \
							gtk/common_m3d.cpp \
							gtk/ressources.cpp \
							gtk/EF_gtk_charge_noeud.cpp \
							gtk/EF_gtk_charge_barre_ponctuelle.cpp \
							gtk/EF_gtk_charge_barre_repartie_uniforme.cpp \
							gtk/EF_gtk_noeud.cpp \
							gtk/EF_gtk_appuis.cpp \
							gtk/EF_gtk_barres.cpp \
							gtk/EF_gtk_barres_add.cpp \
							gtk/EF_gtk_calculs.cpp \
							gtk/EF_gtk_sections.cpp \
							gtk/EF_gtk_materiaux.cpp \
							gtk/EF_gtk_section_rectangulaire.cpp \
							gtk/EF_gtk_section_T.cpp \
							gtk/EF_gtk_section_carree.cpp \
							gtk/EF_gtk_section_circulaire.cpp \
							gtk/EF_gtk_section_personnalisee.cpp \
							gtk/EF_gtk_rapport.cpp \
							gtk/EF_gtk_relachement.cpp \
							gtk/EF_gtk_resultats.cpp

libeurocodes_gtk_la_HEADERS =	gtk/1990_gtk_groupes.hpp \
							gtk/1990_gtk_groupes_options.hpp \
							gtk/1990_gtk_actions.hpp \
							gtk/1992_1_1_gtk_materiaux.hpp \
							gtk/1993_1_1_gtk_materiaux.hpp \
							gtk/common_gtk.hpp \
							gtk/common_gtk_informations.hpp \
							gtk/common_m3d.hpp \
							gtk/ressources.hpp \
							gtk/EF_gtk_charge_noeud.hpp \
							gtk/EF_gtk_charge_barre_ponctuelle.hpp \
							gtk/EF_gtk_charge_barre_repartie_uniforme.hpp \
							gtk/EF_gtk_noeud.hpp \
							gtk/EF_gtk_appuis.hpp \
							gtk/EF_gtk_barres.hpp \
							gtk/EF_gtk_barres_add.hpp \
							gtk/EF_gtk_calculs.hpp \
							gtk/EF_gtk_sections.hpp \
							gtk/EF_gtk_materiaux.hpp \
							gtk/EF_gtk_section_rectangulaire.hpp \
							gtk/EF_gtk_section_T.hpp \
							gtk/EF_gtk_section_carree.hpp \
							gtk/EF_gtk_section_circulaire.hpp \
							gtk/EF_gtk_section_personnalisee.hpp \
							gtk/EF_gtk_rapport.hpp \
							gtk/EF_gtk_relachement.hpp \
							gtk/EF_gtk_resultats.hpp
endif

if ENABLE_GTK
common/common_projet.cpp: gtk/ressources.hpp gtk/ressources.cpp

gtk/ressources.hpp: ../../files/ui/ressources.xml gtk/ressources.cpp
	glib-compile-resources --target=$@ --sourcedir=../../files/ui ../../files/ui/ressources.xml --c-name _2lgc --generate-header --manual-register
gtk/ressources.cpp: ../../files/ui/ressources.xml $(wildcard ../../files/ui/*.ui)
	$(MAKE) -C ../../files/ui
	glib-compile-resources --target=$@ --sourcedir=../../files/ui ../../files/ui/ressources.xml --c-name _2lgc --generate-source --manual-register
endif

CLEANFILES = *.gcno *.gcda gtk/ressources.cpp gtk/ressources.hpp *.hpp

cppcheck :
	cppcheck --inconclusive --enable=all $(DEFAULT_INCLUDES) $(libeurocodes_la_SOURCES) $(libeurocodes_gtk_la_SOURCES) &> ../../cppcheck.log
	@sed 's/^.*scanf without field.*$$//g' -i ../../cppcheck.log
