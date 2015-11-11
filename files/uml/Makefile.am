all : undo-uml projet-uml eurocode-uml gtk-uml

undo-uml : $(top_builddir)/src/libundo/IObserveur.hpp

$(top_builddir)/src/libundo/IObserveur.hpp : $(top_srcdir)/files/uml/libundo.dia
if HAVE_DIA2CODE
	/home/legarrec/info/programmation/dia2code-git/src/dia2code --buildtree --tab 2 -nl -l $(top_srcdir)/files/uml/COPYING.head -t cpp11 -ext hpp -d $(top_builddir)/src/libundo "$(top_srcdir)/files/uml/libundo.dia"
endif

projet-uml : $(top_builddir)/src/libprojet/CCalculs.hpp

$(top_builddir)/src/libprojet/CCalculs.hpp : $(top_srcdir)/files/uml/libprojet.dia
if HAVE_DIA2CODE
	/home/legarrec/info/programmation/dia2code-git/src/dia2code --buildtree --tab 2 -nl -l $(top_srcdir)/files/uml/COPYING.head -t cpp11 -ext hpp -d $(top_builddir)/src/libprojet "$(top_srcdir)/files/uml/libprojet.dia"
endif

eurocode-uml : $(top_builddir)/src/libprojet/POCO/str/materiau/EcAcier.hpp

$(top_builddir)/src/libprojet/POCO/str/materiau/EcAcier.hpp : $(top_srcdir)/files/uml/libeurocode.dia
if HAVE_DIA2CODE
	/home/legarrec/info/programmation/dia2code-git/src/dia2code --buildtree --tab 2 -nl -l $(top_srcdir)/files/uml/COPYING.head -t cpp11 -ext hpp -d $(top_builddir)/src/libprojet "$(top_srcdir)/files/uml/libeurocode.dia"
endif

if ENABLE_GTK
gtk-uml : $(top_builddir)/src/gtk/GWindowMain.hpp

$(top_builddir)/src/gtk/GWindowMain.hpp : $(top_srcdir)/files/uml/gtk.dia
if HAVE_DIA2CODE
	/home/legarrec/info/programmation/dia2code-git/src/dia2code --buildtree --tab 2 -nl -l $(top_srcdir)/files/uml/COPYING.head -t cpp11 -ext hpp -d $(top_builddir)/src/gtk "$(top_srcdir)/files/uml/gtk.dia"
endif
else
gtk-uml :
endif

checkcode :
# On est obligé de mettre un MAKE car il doit y avoir une optimisation et en
# mettant comp_hpp_cpp dans la dépendance, les wildcard doivent être évalués
# au début et non pas à l'appel de la dépendance de comp_hpp_cpp.
	$(MAKE) comp_hpp_cpp

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
