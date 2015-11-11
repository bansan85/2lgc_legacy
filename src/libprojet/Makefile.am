DEFAULT_INCLUDES		  =	-I$(srcdir) \
												-I$(srcdir)/../.. \
												-I$(srcdir)/../macro \
												-I$(srcdir)/../libundo

AUTOMAKE_OPTIONS		  =	subdir-objects

lib_LTLIBRARIES			  =	libprojet.la

libprojet_la_LIBADD	 	=	@XML_LIBS@ \
                        ../libundo/libundo.la
if PLATFORM_LINUX
if ENABLE_DEBUG
libprojet_la_LIBADD	 +=	-lgcov
endif
endif
if PLATFORM_WIN32
libprojet_la_LIBADD	 += -lintl
endif
if PLATFORM_WIN64
libprojet_la_LIBADD	 += -lintl
endif

libprojet_la_CXXFLAGS = $(AM_CXXFLAGS)

libprojet_la_LDFLAGS	= $(AM_LDFLAGS) -release $(VERSION) -Wl,-O0 -no-undefined

libprojet_la_CPPFLAGS	=	@XML_CPPFLAGS@

libprojet_ladir 			=	$(includedir)/2lgc

libprojet_la_SOURCES 	= \
	CModele.cpp
#	CModeleAddAction.cpp \
	CCalculs.cpp \
	CModeleRmAction.cpp \
	CModeleSetParametres.cpp \
	EAction.cpp \
	ENorme.cpp \
	IUndoFunc.cpp \
	\
	POCO/CAdresse.cpp \
	POCO/CPreferences.cpp \
	POCO/EUnite.cpp \
	\
	POCO/calcul/CAnalyseComm.cpp \
	POCO/calcul/CBarreInfoEF.cpp \
	POCO/calcul/CFonction.cpp \
	POCO/calcul/CPonderation.cpp \
	POCO/calcul/CPonderations.cpp \
	POCO/calcul/CTroncon.cpp \
	\
	POCO/calcul/INorme.cpp \
	\
	POCO/INb.cpp \
	POCO/nombre/Calcul.cpp \
	POCO/nombre/User.cpp \
	\
	POCO/sollicitation/CCombinaison.cpp \
	POCO/sollicitation/CNiveauGroupe.cpp \
	\
	POCO/sollicitation/CAction.cpp \
	POCO/sollicitation/action/CActionSetPsi.cpp \
	POCO/sollicitation/action/CGroupe.cpp \
	POCO/sollicitation/action/ECombinaison.cpp \
	POCO/sollicitation/action/IActionGroupe.cpp \
	\
	POCO/sollicitation/ICharge.cpp \
	POCO/sollicitation/charge/BarrePonctuelle.cpp \
	POCO/sollicitation/charge/BarreRepartieUniformement.cpp \
	POCO/sollicitation/charge/Noeud.cpp \
	\
	POCO/structure/CAppui.cpp \
	POCO/structure/CBarre.cpp \
	POCO/structure/CPoint.cpp \
	POCO/structure/CRelachement.cpp \
	\
	POCO/structure/IDeplacementType.cpp \
	POCO/structure/deplacementType/Bloque.cpp \
	POCO/structure/deplacementType/Libre.cpp \
	POCO/structure/deplacementType/Lineaire.cpp \
	\
	POCO/structure/IMateriau.cpp \
	\
	POCO/structure/INoeud.cpp \
	POCO/structure/noeud/Barre.cpp \
	POCO/structure/noeud/Libre.cpp \
	\
	POCO/structure/ISection.cpp \
	POCO/structure/section/Carree.cpp \
	POCO/structure/section/Circulaire.cpp \
	POCO/structure/section/Personnalisee.cpp \
	POCO/structure/section/Rectangulaire.cpp \
	POCO/structure/section/T.cpp


libprojet_la_HEADERS 	=	\
	CCalculs.hpp \
	CModeleAddAction.hpp \
	CModele.hpp \
	CModeleRmAction.hpp \
	CModeleSetParametres.hpp \
	EAction.hpp \
	ENorme.hpp \
	IUndoFunc.hpp \
	\
	POCO/CAdresse.hpp \
	POCO/CPreferences.hpp \
	POCO/EUnite.hpp \
	\
	POCO/calcul/CAnalyseComm.hpp \
	POCO/calcul/CBarreInfoEF.hpp \
	POCO/calcul/CFonction.hpp \
	POCO/calcul/CPonderation.hpp \
	POCO/calcul/CPonderations.hpp \
	POCO/calcul/CTroncon.hpp \
	\
	POCO/calcul/INorme.hpp \
	\
	POCO/INb.hpp \
	POCO/nombre/Calcul.hpp \
	POCO/nombre/User.hpp \
	\
	POCO/sollicitation/CCombinaison.hpp \
	POCO/sollicitation/CNiveauGroupe.hpp \
	\
	POCO/sollicitation/CAction.hpp \
	POCO/sollicitation/action/CActionSetPsi.hpp \
	POCO/sollicitation/action/CGroupe.hpp \
	POCO/sollicitation/action/ECombinaison.hpp \
	POCO/sollicitation/action/IActionGroupe.hpp \
	\
	POCO/sollicitation/ICharge.hpp \
	POCO/sollicitation/charge/BarrePonctuelle.hpp \
	POCO/sollicitation/charge/BarreRepartieUniformement.hpp \
	POCO/sollicitation/charge/Noeud.hpp \
	\
	POCO/structure/CAppui.hpp \
	POCO/structure/CBarre.hpp \
	POCO/structure/CPoint.hpp \
	POCO/structure/CRelachement.hpp \
	\
	POCO/structure/IDeplacementType.hpp \
	POCO/structure/deplacementType/Bloque.hpp \
	POCO/structure/deplacementType/Libre.hpp \
	POCO/structure/deplacementType/Lineaire.hpp \
	\
	POCO/structure/IMateriau.hpp \
	\
	POCO/structure/INoeud.hpp \
	POCO/structure/noeud/Barre.hpp \
	POCO/structure/noeud/Libre.hpp \
	\
	POCO/structure/ISection.hpp \
	POCO/structure/section/Carree.hpp \
	POCO/structure/section/Circulaire.hpp \
	POCO/structure/section/Personnalisee.hpp \
	POCO/structure/section/Rectangulaire.hpp \
	POCO/structure/section/T.hpp

%.hpp :
	$(MAKE) -C $(top_builddir)/files/uml projet-uml

CLEANFILES = *.gcno *.gcda $(libprojet_la_HEADERS)

cppcheck :
	cppcheck --inconclusive --enable=all $(DEFAULT_INCLUDES) $(libprojet_la_SOURCES) &> ../../cppcheck.log
	@sed 's/^.*scanf without field.*$$//g' -i ../../cppcheck.log
