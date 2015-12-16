DEFAULT_INCLUDES		  =	-I$(srcdir) \
												-I$(srcdir)/../.. \
												-I$(srcdir)/../macro \
												-I$(srcdir)/../libsouvenir \
												@XML_CPPFLAGS@

AUTOMAKE_OPTIONS		  =	subdir-objects

lib_LTLIBRARIES			  =	libprojet.la

libprojet_la_LIBADD	 	=	@XML_LIBS@ \
                        ../libsouvenir/libsouvenir.la
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

libprojet_ladir 			=	$(includedir)/2lgc

libprojet_la_SOURCES 	= \
	../macro/SString.cpp \
	CModele.cpp \
	CCalculs.cpp \
	POCO/CPreferences.cpp \
	POCO/calc/CPonderations.cpp \
	POCO/sol/CAction.cpp \
	POCO/sol/action/IActionGroupe.cpp \
	POCO/INb.cpp \
	POCO/nombre/Calcul.cpp \
	POCO/nombre/Utilisateur.cpp \
	INorme.cpp \
	norme/Eurocode.cpp \
	FuncModeleAction.cpp \
	FuncModeleNorme.cpp
#	EAction.cpp \
	IUndoFunc.cpp \
	\
	ENorme.cpp \
	\
	POCO/CAdresse.cpp \
	POCO/EUnite.cpp \
	\
	POCO/calc/CAnalyseComm.cpp \
	POCO/calc/CBarreInfoEF.cpp \
	POCO/calc/CFonction.cpp \
	POCO/calc/CPonderation.cpp \
	POCO/calc/CTroncon.cpp \
	\
	\
	POCO/sol/CCombinaison.cpp \
	POCO/sol/CNiveauGroupe.cpp \
	\
	POCO/sol/action/CGroupe.cpp \
	POCO/sol/action/ECombinaison.cpp \
	\
	POCO/sol/ICharge.cpp \
	POCO/sol/charge/BarrePonctuelle.cpp \
	POCO/sol/charge/BarreRepartieUniformement.cpp \
	POCO/sol/charge/Noeud.cpp \
	\
	POCO/str/CAppui.cpp \
	POCO/str/CBarre.cpp \
	POCO/str/CPoint.cpp \
	POCO/str/CRelachement.cpp \
	\
	POCO/str/IDeplacementType.cpp \
	POCO/str/deplacementType/Bloque.cpp \
	POCO/str/deplacementType/Libre.cpp \
	POCO/str/deplacementType/Lineaire.cpp \
	\
	POCO/str/IMateriau.cpp \
	\
	POCO/str/INoeud.cpp \
	POCO/str/noeud/Barre.cpp \
	POCO/str/noeud/Libre.cpp \
	\
	POCO/str/ISection.cpp \
	POCO/str/section/Carree.cpp \
	POCO/str/section/Circulaire.cpp \
	POCO/str/section/Personnalisee.cpp \
	POCO/str/section/Rectangulaire.cpp \
	POCO/str/section/T.cpp


libprojet_la_HEADERS 	=	\
	CCalculs.hpp \
	FuncModeleAction.hpp \
	FuncModeleNorme.hpp \
	CModele.hpp \
	INorme.hpp \
	EAction.hpp \
	ENorme.hpp \
	EUnite.hpp \
	\
	POCO/CAdresse.hpp \
	POCO/CPreferences.hpp \
	\
	POCO/calc/CAnalyseComm.hpp \
	POCO/calc/CBarreInfoEF.hpp \
	POCO/calc/CFonction.hpp \
	POCO/calc/CPonderation.hpp \
	POCO/calc/CPonderations.hpp \
	POCO/calc/CTroncon.hpp \
	\
	POCO/INb.hpp \
	POCO/nombre/Calcul.hpp \
	POCO/nombre/Utilisateur.hpp \
	\
	POCO/sol/CCombinaison.hpp \
	POCO/sol/CNiveauGroupe.hpp \
	\
	POCO/sol/CAction.hpp \
	POCO/sol/action/CGroupe.hpp \
	POCO/sol/action/ECombinaison.hpp \
	POCO/sol/action/IActionGroupe.hpp \
	\
	POCO/sol/ICharge.hpp \
	POCO/sol/charge/BarrePonctuelle.hpp \
	POCO/sol/charge/BarreRepartieUniformement.hpp \
	POCO/sol/charge/Noeud.hpp \
	\
	POCO/str/CAppui.hpp \
	POCO/str/CBarre.hpp \
	POCO/str/CPoint.hpp \
	POCO/str/CRelachement.hpp \
	\
	POCO/str/IDeplacementType.hpp \
	POCO/str/deplacementType/Bloque.hpp \
	POCO/str/deplacementType/Libre.hpp \
	POCO/str/deplacementType/Lineaire.hpp \
	\
	POCO/str/IMateriau.hpp \
	POCO/str/materiau/EcAcier.hpp \
	POCO/str/materiau/EcBeton.hpp \
	\
	POCO/str/INoeud.hpp \
	POCO/str/noeud/Barre.hpp \
	POCO/str/noeud/Libre.hpp \
	\
	POCO/str/ISection.hpp \
	POCO/str/section/Carree.hpp \
	POCO/str/section/Circulaire.hpp \
	POCO/str/section/Personnalisee.hpp \
	POCO/str/section/Rectangulaire.hpp \
	POCO/str/section/T.hpp \
	\
	norme/Eurocode.hpp \
	norme/ENormeEcAc.hpp

%.hpp :
	$(MAKE) -C $(top_builddir)/files/uml projet-uml
	$(MAKE) -C $(top_builddir)/files/uml projet-uml eurocode-uml

CLEANFILES = *.gcno ../macro/*.gcno POCO/sol/action/*.gcno POCO/sol/*.gcno POCO/*.gcno POCO/calc/*.gcno POCO/nombre/*.gcno norme/*.gcno *.gcda $(libprojet_la_HEADERS)

checkcode :
	../../../deheader-1.2/deheader > deheader.log
	sed -i "s/^.*\"config\.h\".*$//g" deheader.log

cppcheck :
	cppcheck --inconclusive --enable=all $(DEFAULT_INCLUDES) $(libprojet_la_SOURCES) &> ../../cppcheck.log
	@sed 's/^.*scanf without field.*$$//g' -i ../../cppcheck.log
