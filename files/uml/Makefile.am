
all : $(top_builddir)/files/uml/gtk.stamp $(top_builddir)/files/uml/undo.stamp $(top_builddir)/files/uml/codegui.stamp

if ENABLE_GTK
$(top_builddir)/files/uml/gtk.stamp : $(top_builddir)/src/gtk/GWindowMain.hpp
else
$(top_builddir)/files/uml/gtk.stamp :
endif

$(top_builddir)/files/uml/undo.stamp : \
			$(top_builddir)/src/libundo/EEvent.hpp \
			$(top_builddir)/src/libundo/EUndoEtat.hpp \
			$(top_builddir)/src/libundo/IObserveur.hpp \
			$(top_builddir)/src/libundo/ISujet.hpp \
			$(top_builddir)/src/libundo/IUndoable.hpp \
			$(top_builddir)/src/libundo/UndoData.hpp \
			$(top_builddir)/src/libundo/UndoManager.hpp

$(top_builddir)/files/uml/codegui.stamp : \
      $(top_builddir)/src/lib/CAction.hpp \
      $(top_builddir)/src/lib/CAdresse.hpp \
      $(top_builddir)/src/lib/CAnalyseComm.hpp \
      $(top_builddir)/src/lib/CAppui.hpp \
      $(top_builddir)/src/lib/CBarre.hpp \
      $(top_builddir)/src/lib/CBarreInfoEF.hpp \
      $(top_builddir)/src/lib/CCalculs.hpp \
      $(top_builddir)/src/lib/CCombinaison.hpp \
      $(top_builddir)/src/lib/CFonction.hpp \
      $(top_builddir)/src/lib/CGroupe.hpp \
      $(top_builddir)/src/lib/ChargeBarrePonctuelle.hpp \
      $(top_builddir)/src/lib/ChargeBarreRepUni.hpp \
      $(top_builddir)/src/lib/ChargeNoeud.hpp \
      $(top_builddir)/src/lib/CMatAcierEc.hpp \
      $(top_builddir)/src/lib/CMatBetonEc.hpp \
      $(top_builddir)/src/lib/CModele.hpp \
      $(top_builddir)/src/lib/CNiveauGroupe.hpp \
      $(top_builddir)/src/lib/CParamEC.hpp \
      $(top_builddir)/src/lib/CPoint.hpp \
      $(top_builddir)/src/lib/CPonderation.hpp \
      $(top_builddir)/src/lib/CPonderations.hpp \
      $(top_builddir)/src/lib/CPreferences.hpp \
      $(top_builddir)/src/lib/CProjet.hpp \
      $(top_builddir)/src/lib/CRelachement.hpp \
      $(top_builddir)/src/lib/CTroncon.hpp \
      $(top_builddir)/src/lib/DepBloque.hpp \
      $(top_builddir)/src/lib/DepLibre.hpp \
      $(top_builddir)/src/lib/DepLineaire.hpp \
      $(top_builddir)/src/lib/EAction.hpp \
      $(top_builddir)/src/lib/ECombinaison.hpp \
      $(top_builddir)/src/lib/ENormeEcAc.hpp \
      $(top_builddir)/src/lib/ENorme.hpp \
      $(top_builddir)/src/lib/EUnite.hpp \
      $(top_builddir)/src/lib/IActionGroupe.hpp \
      $(top_builddir)/src/lib/ICharge.hpp \
      $(top_builddir)/src/lib/IDeplacementType.hpp \
      $(top_builddir)/src/lib/IMateriau.hpp \
      $(top_builddir)/src/lib/INb.hpp \
      $(top_builddir)/src/lib/INoeud.hpp \
      $(top_builddir)/src/lib/IParametres.hpp \
      $(top_builddir)/src/lib/ISection.hpp \
      $(top_builddir)/src/lib/NbCalcul.hpp \
      $(top_builddir)/src/lib/NbUser.hpp \
      $(top_builddir)/src/lib/NdBarre.hpp \
      $(top_builddir)/src/lib/NdLibre.hpp \
      $(top_builddir)/src/lib/SecCarree.hpp \
      $(top_builddir)/src/lib/SecCirc.hpp \
      $(top_builddir)/src/lib/SecPerso.hpp \
      $(top_builddir)/src/lib/SecRect.hpp \
      $(top_builddir)/src/lib/SecT.hpp

checkcode : ../../src/lib/codegui.hpp
# On est obligé de mettre un MAKE car il doit y avoir une optimisation et en
# mettant comp_hpp_cpp dans la dépendance, les wildcard doivent être évalués
# au début et non pas à l'appel de la dépendance de comp_hpp_cpp.
	$(MAKE) comp_hpp_cpp

$(top_builddir)/src/gtk/%.hpp : $(top_srcdir)/files/uml/gtk.dia
	@if test -e "$@"; then \
		echo mv "$@" "$@.old"; \
		mv "$@" "$@.old"; \
	fi
	/home/legarrec/info/programmation/dia2code-git/src/dia2code --tab 2 -nl -l $(top_srcdir)/files/uml/COPYING.head -t cpp11 -ext hpp -d $(top_builddir)/src/gtk -cl "$*" "$(top_srcdir)/files/uml/gtk.dia"
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

$(top_builddir)/src/libundo/%.hpp : $(top_srcdir)/files/uml/undo.dia
	@if test -e "$@"; then \
		echo mv "$@" "$@.old"; \
		mv "$@" "$@.old"; \
	fi
	/home/legarrec/info/programmation/dia2code-git/src/dia2code --tab 2 -nl -l $(top_srcdir)/files/uml/COPYING.head -t cpp11 -ext hpp -d $(top_builddir)/src/libundo -cl "$*" "$(top_srcdir)/files/uml/undo.dia"
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

$(top_builddir)/src/lib/%.hpp : $(top_srcdir)/files/uml/codegui.dia
	@if test -e "$@"; then \
		echo mv "$@" "$@.old"; \
		mv "$@" "$@.old"; \
	fi
	/home/legarrec/info/programmation/dia2code-git/src/dia2code --tab 2 -nl -l $(top_srcdir)/files/uml/COPYING.head -t cpp11 -ext hpp -d $(top_builddir)/src/lib -cl "$*" "$(top_srcdir)/files/uml/codegui.dia"
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

SRC_HPP = $(wildcard *.hpp)
OBJ_CPP = $(SRC_HPP:.hpp=.cpp)

comp_hpp_cpp : $(OBJ_CPP)
	cat *.cpp.log > log

%.cpp : %.hpp
	echo -e "#include \"$^\" \nint main () { };" > $@
	g++ $@ -std=c++11 -c -o $@.o &> $@.log

CLEANFILES	=	*.cpp.log log *.cpp *.hpp *.cpp.o codegui.dia~ *.stamp

EXTRA_DIST	=	COPYING.head \
							codegui.dia \
							gtk.dia \
							undo.dia
