all : $(top_builddir)/files/uml/gtk.stamp \
      $(top_builddir)/files/uml/libundo.stamp \
			$(top_builddir)/files/uml/libprojet.stamp \
			$(top_builddir)/files/uml/libeurocode.stamp

if ENABLE_GTK
$(top_builddir)/files/uml/gtk.stamp : $(top_builddir)/src/gtk/GWindowMain.hpp
else
$(top_builddir)/files/uml/gtk.stamp :
endif

$(top_builddir)/files/uml/libundo.stamp : \
			$(top_builddir)/src/libundo/EEvent.hpp \
			$(top_builddir)/src/libundo/EUndoEtat.hpp \
			$(top_builddir)/src/libundo/IObserveur.hpp \
			$(top_builddir)/src/libundo/ISujet.hpp \
			$(top_builddir)/src/libundo/IUndoableFonction.hpp \
			$(top_builddir)/src/libundo/POCO/UndoData.hpp \
			$(top_builddir)/src/libundo/UndoManager.hpp

$(top_builddir)/files/uml/libprojet.stamp : \
      $(top_builddir)/src/libprojet/CCalculs.hpp \
      $(top_builddir)/src/libprojet/CModeleAddAction.hpp \
      $(top_builddir)/src/libprojet/CModele.hpp \
      $(top_builddir)/src/libprojet/CModeleRmAction.hpp \
      $(top_builddir)/src/libprojet/CModeleSetParametres.hpp \
      $(top_builddir)/src/libprojet/EAction.hpp \
      $(top_builddir)/src/libprojet/ENorme.hpp \
      $(top_builddir)/src/libprojet/IUndoFunc.hpp \
			\
      $(top_builddir)/src/libprojet/POCO/CAdresse.hpp \
      $(top_builddir)/src/libprojet/POCO/CPreferences.hpp \
      $(top_builddir)/src/libprojet/POCO/EUnite.hpp \
			\
      $(top_builddir)/src/libprojet/POCO/calcul/CAnalyseComm.hpp \
      $(top_builddir)/src/libprojet/POCO/calcul/CBarreInfoEF.hpp \
      $(top_builddir)/src/libprojet/POCO/calcul/CFonction.hpp \
      $(top_builddir)/src/libprojet/POCO/calcul/CPonderation.hpp \
      $(top_builddir)/src/libprojet/POCO/calcul/CPonderations.hpp \
      $(top_builddir)/src/libprojet/POCO/calcul/CTroncon.hpp \
			\
      $(top_builddir)/src/libprojet/POCO/calcul/INorme.hpp \
			\
      $(top_builddir)/src/libprojet/POCO/INb.hpp \
      $(top_builddir)/src/libprojet/POCO/nombre/Calcul.hpp \
      $(top_builddir)/src/libprojet/POCO/nombre/User.hpp \
			\
      $(top_builddir)/src/libprojet/POCO/sollicitation/CCombinaison.hpp \
      $(top_builddir)/src/libprojet/POCO/sollicitation/CNiveauGroupe.hpp \
			\
      $(top_builddir)/src/libprojet/POCO/sollicitation/CAction.hpp \
      $(top_builddir)/src/libprojet/POCO/sollicitation/action/CActionSetPsi.hpp \
      $(top_builddir)/src/libprojet/POCO/sollicitation/action/CGroupe.hpp \
      $(top_builddir)/src/libprojet/POCO/sollicitation/action/ECombinaison.hpp \
      $(top_builddir)/src/libprojet/POCO/sollicitation/action/IActionGroupe.hpp \
			\
      $(top_builddir)/src/libprojet/POCO/sollicitation/ICharge.hpp \
      $(top_builddir)/src/libprojet/POCO/sollicitation/charge/BarrePonctuelle.hpp \
      $(top_builddir)/src/libprojet/POCO/sollicitation/charge/BarreRepartieUniformement.hpp \
      $(top_builddir)/src/libprojet/POCO/sollicitation/charge/Noeud.hpp \
			\
      $(top_builddir)/src/libprojet/POCO/structure/CAppui.hpp \
      $(top_builddir)/src/libprojet/POCO/structure/CBarre.hpp \
      $(top_builddir)/src/libprojet/POCO/structure/CPoint.hpp \
      $(top_builddir)/src/libprojet/POCO/structure/CRelachement.hpp \
			\
      $(top_builddir)/src/libprojet/POCO/structure/IDeplacementType.hpp \
      $(top_builddir)/src/libprojet/POCO/structure/deplacementType/Bloque.hpp \
      $(top_builddir)/src/libprojet/POCO/structure/deplacementType/Libre.hpp \
      $(top_builddir)/src/libprojet/POCO/structure/deplacementType/Lineaire.hpp \
			\
      $(top_builddir)/src/libprojet/POCO/structure/IMateriau.hpp \
			\
      $(top_builddir)/src/libprojet/POCO/structure/INoeud.hpp \
      $(top_builddir)/src/libprojet/POCO/structure/noeud/Barre.hpp \
      $(top_builddir)/src/libprojet/POCO/structure/noeud/Libre.hpp \
			\
      $(top_builddir)/src/libprojet/POCO/structure/ISection.hpp \
      $(top_builddir)/src/libprojet/POCO/structure/section/Carree.hpp \
      $(top_builddir)/src/libprojet/POCO/structure/section/Circulaire.hpp \
      $(top_builddir)/src/libprojet/POCO/structure/section/Personnalisee.hpp \
      $(top_builddir)/src/libprojet/POCO/structure/section/Rectangulaire.hpp \
      $(top_builddir)/src/libprojet/POCO/structure/section/T.hpp

$(top_builddir)/files/uml/libeurocode.stamp : \
      $(top_builddir)/src/libeurocode/CMatAcierEc.hpp \
      $(top_builddir)/src/libeurocode/CMatBetonEc.hpp \
      $(top_builddir)/src/libeurocode/CParamEC.hpp \
      $(top_builddir)/src/libeurocode/ENormeEcAc.hpp

checkcode : ../../src/libeurocode/codegui.hpp
# On est obligé de mettre un MAKE car il doit y avoir une optimisation et en
# mettant comp_hpp_cpp dans la dépendance, les wildcard doivent être évalués
# au début et non pas à l'appel de la dépendance de comp_hpp_cpp.
	$(MAKE) comp_hpp_cpp

$(top_builddir)/src/libundo/%.hpp : $(top_srcdir)/files/uml/libundo.dia $(top_builddir)/src/libundo/POCO
if HAVE_DIA2CODE
	@if test -e "$@"; then \
		echo mv "$@" "$@.old"; \
		mv "$@" "$@.old"; \
	fi
	/home/legarrec/info/programmation/dia2code-git/src/dia2code --buildtree --tab 2 -nl -l $(top_srcdir)/files/uml/COPYING.head -t cpp11 -ext hpp -d $(top_builddir)/src/libundo -cl "$*" "$(top_srcdir)/files/uml/libundo.dia"
	@if test -e "$@.old"; then \
		if test ! "`(md5sum $@.old |sed 's/^\([0-9a-f]*\).*/\1/')`" = "`(md5sum $@ |sed 's/^\([0-9a-f]*\).*/\1/')`"; then \
			echo rm "$@.old"; \
			rm "$@.old"; \
		else \
			echo mv "$@.old" "$@"; \
			mv "$@.old" "$@"; \
		fi \
	fi
	touch $@
endif

$(top_builddir)/src/libundo/POCO :
	mkdir -p $(top_builddir)/src/libundo/POCO

$(top_builddir)/src/libprojet/%.hpp : $(top_srcdir)/files/uml/libprojet.dia $(top_builddir)/src/libprojet/POCO $(top_builddir)/src/libprojet/POCO/calcul $(top_builddir)/src/libprojet/POCO/calcul/norme $(top_builddir)/src/libprojet/POCO/nombre $(top_builddir)/src/libprojet/POCO/sollicitation $(top_builddir)/src/libprojet/POCO/sollicitation/action $(top_builddir)/src/libprojet/POCO/sollicitation/charge $(top_builddir)/src/libprojet/POCO/structure $(top_builddir)/src/libprojet/POCO/structure/deplacementType $(top_builddir)/src/libprojet/POCO/structure/materiau $(top_builddir)/src/libprojet/POCO/structure/noeud $(top_builddir)/src/libprojet/POCO/structure/section
if HAVE_DIA2CODE
	@if test -e "$@"; then \
		echo mv "$@" "$@.old"; \
		mv "$@" "$@.old"; \
	fi
	/home/legarrec/info/programmation/dia2code-git/src/dia2code --buildtree --tab 2 -nl -l $(top_srcdir)/files/uml/COPYING.head -t cpp11 -ext hpp -d $(top_builddir)/src/libprojet -cl "$*" "$(top_srcdir)/files/uml/libprojet.dia"
	@if test -e "$@.old"; then \
		if test ! "`(md5sum $@.old |sed 's/^\([0-9a-f]*\).*/\1/')`" = "`(md5sum $@ |sed 's/^\([0-9a-f]*\).*/\1/')`"; then \
			echo rm "$@.old"; \
			rm "$@.old"; \
		else \
			echo mv "$@.old" "$@"; \
			mv "$@.old" "$@"; \
		fi \
	fi
	touch $@
endif

$(top_builddir)/src/libprojet/POCO :
	mkdir -p $@

$(top_builddir)/src/libprojet/POCO/calcul :
	mkdir -p $@

$(top_builddir)/src/libprojet/POCO/calcul/norme :
	mkdir -p $@

$(top_builddir)/src/libprojet/POCO/nombre :
	mkdir -p $@

$(top_builddir)/src/libprojet/POCO/sollicitation :
	mkdir -p $@

$(top_builddir)/src/libprojet/POCO/sollicitation/action :
	mkdir -p $@

$(top_builddir)/src/libprojet/POCO/sollicitation/charge :
	mkdir -p $@

$(top_builddir)/src/libprojet/POCO/structure :
	mkdir -p $@

$(top_builddir)/src/libprojet/POCO/structure/deplacementType :
	mkdir -p $@

$(top_builddir)/src/libprojet/POCO/structure/materiau :
	mkdir -p $@

$(top_builddir)/src/libprojet/POCO/structure/noeud :
	mkdir -p $@

$(top_builddir)/src/libprojet/POCO/structure/section :
	mkdir -p $@


$(top_builddir)/src/libeurocode/%.hpp : $(top_srcdir)/files/uml/libeurocode.dia
if HAVE_DIA2CODE
	@if test -e "$@"; then \
		echo mv "$@" "$@.old"; \
		mv "$@" "$@.old"; \
	fi
	/home/legarrec/info/programmation/dia2code-git/src/dia2code --buildtree --tab 2 -nl -l $(top_srcdir)/files/uml/COPYING.head -t cpp11 -ext hpp -d $(top_builddir)/src/libeurocode -cl "$*" "$(top_srcdir)/files/uml/libeurocode.dia"
	@if test -e "$@.old"; then \
		if test ! "`(md5sum $@.old |sed 's/^\([0-9a-f]*\).*/\1/')`" = "`(md5sum $@ |sed 's/^\([0-9a-f]*\).*/\1/')`"; then \
			echo rm "$@.old"; \
			rm "$@.old"; \
		else \
			echo mv "$@.old" "$@"; \
			mv "$@.old" "$@"; \
		fi \
	fi
	touch $@
endif

$(top_builddir)/src/gtk/%.hpp : $(top_srcdir)/files/uml/gtk.dia
if HAVE_DIA2CODE
	@if test -e "$@"; then \
		echo mv "$@" "$@.old"; \
		mv "$@" "$@.old"; \
	fi
	/home/legarrec/info/programmation/dia2code-git/src/dia2code --buildtree --tab 2 -nl -l $(top_srcdir)/files/uml/COPYING.head -t cpp11 -ext hpp -d $(top_builddir)/src/gtk -cl "$*" "$(top_srcdir)/files/uml/gtk.dia"
	@if test -e "$@.old"; then \
		if test ! "`(md5sum $@.old |sed 's/^\([0-9a-f]*\).*/\1/')`" = "`(md5sum $@ |sed 's/^\([0-9a-f]*\).*/\1/')`"; then \
			echo rm "$@.old"; \
			rm "$@.old"; \
		else \
			echo mv "$@.old" "$@"; \
			mv "$@.old" "$@"; \
		fi \
	fi
	touch $@
endif

SRC_HPP = $(wildcard *.hpp)
OBJ_CPP = $(SRC_HPP:.hpp=.cpp)

comp_hpp_cpp : $(OBJ_CPP)
	cat *.cpp.log > log

%.cpp : %.hpp
	echo -e "#include \"$^\" \nint main () { };" > $@
	g++ $@ -std=c++11 -c -o $@.o &> $@.log

CLEANFILES	=	*.cpp.log log *.cpp *.hpp *.cpp.o *.stamp

EXTRA_DIST	=	COPYING.head \
							gtk.dia \
							libundo.dia \
							libprojet.dia \
							libeurocode.dia
