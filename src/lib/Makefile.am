DEFAULT_INCLUDES					=		-I$(srcdir) \
															-I$(srcdir)/../.. \
															-I$(srcdir)/../macro \
															-I$(srcdir)/../libundo

AUTOMAKE_OPTIONS					= 	subdir-objects

lib_LTLIBRARIES						= 	libeurocodes.la

libeurocodes_la_LIBADD		=		@BLAS_LIBS@ -lgfortran @XML_LIBS@
if PLATFORM_LINUX
if ENABLE_DEBUG
libeurocodes_la_LIBADD    +=	-lgcov
endif
endif
if PLATFORM_WIN32
libeurocodes_la_LIBADD    +=  -lintl
endif
if PLATFORM_WIN64
libeurocodes_la_LIBADD    +=  -lintl
endif

if ENABLE_GTK
libeurocodes_la_CXXFLAGS =   @GTKMM_CFLAGS@
libeurocodes_la_LIBADD   +=  @GTKMM_LIBS@
endif

libeurocodes_la_LDFLAGS		= 	$(AM_LDFLAGS) -version-info $(VERSION) -Wl,-O0 -no-undefined

libeurocodes_la_CPPFLAGS	=		@XML_CPPFLAGS@
if ENABLE_GTK
libeurocodes_la_CPPFLAGS	+=	@GTK_CFLAGS@
endif

libeurocodes_ladir 				= 	$(includedir)/2lgc

libeurocodes_la_SOURCES 	=		CAction.cpp \
															CBarreInfoEF.cpp \
															CCalculs.cpp \
															CModele.cpp \
															CNbCalcul.cpp \
															CNbUser.cpp \
															CParamEC.cpp \
															CPonderations.cpp \
															CPreferences.cpp \
															CProjet.cpp \
															IActionGroupe.cpp \
															INb.cpp \
															IParametres.cpp \
															SString.cpp

libeurocodes_la_HEADERS 	=		$(wildcard *.hpp *.hh)

$(libeurocodes_la_SOURCES) : 	uml

uml :
	$(MAKE) -C ../../files/uml

CLEANFILES = *.gcno *.gcda *.hpp gtk/ressources.cpp gtk/ressources.hpp

if ENABLE_GTK
DEFAULT_INCLUDES							+=	-I$(srcdir)/gtk
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

common/common_projet.cpp: gtk/ressources.hpp gtk/ressources.cpp

endif

gtk/ressources.hpp: ../../files/ui/ressources.xml gtk/ressources.cpp
if ENABLE_GTK
	glib-compile-resources --target=$@ --sourcedir=../../files/ui ../../files/ui/ressources.xml --c-name _2lgc --generate-header --manual-register
else
	touch gtk/ressources.hpp
endif

gtk/ressources.cpp: ../../files/ui/ressources.xml $(wildcard ../../files/ui/*.ui)
if ENABLE_GTK
	$(MAKE) -C ../../files/ui
	glib-compile-resources --target=$@ --sourcedir=../../files/ui ../../files/ui/ressources.xml --c-name _2lgc --generate-source --manual-register
else
	touch gtk/ressources.cpp
endif

cppcheck :
	cppcheck --inconclusive --enable=all $(DEFAULT_INCLUDES) $(libeurocodes_la_SOURCES) &> ../../cppcheck.log
if ENABLE_GTK
	cppcheck --inconclusive --enable=all $(DEFAULT_INCLUDES) $(libeurocodes_gtk_la_SOURCES) &>> ../../cppcheck.log
endif
	@sed 's/^.*scanf without field.*$$//g' -i ../../cppcheck.log
