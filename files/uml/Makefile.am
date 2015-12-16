all : souvenir-uml projet-uml eurocode-uml gtk-uml

souvenir-uml : $(top_builddir)/src/libsouvenir/IObserveur.hpp

$(top_builddir)/src/libsouvenir/IObserveur.hpp : $(top_srcdir)/files/uml/libsouvenir.dia
if HAVE_DIA2CODE
	/home/legarrec/info/programmation/dia2code-git/src/dia2code --buildtree --tab 2 -nl -l $(top_srcdir)/files/uml/COPYING.head -t cpp11 -ext hpp -d $(top_builddir)/src/libsouvenir "$(top_srcdir)/files/uml/libsouvenir.dia"
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

checkcode : all
# On est obligé de mettre un MAKE car il doit y avoir une optimisation et en
# mettant comp_hpp_cpp dans la dépendance, les wildcard doivent être évalués
# au début et non pas à l'appel de la dépendance de comp_hpp_cpp.
	$(MAKE) comp_hpp_cpp
	../../../deheader-1.2/deheader > deheader.log
	sed -i "s/^.*\"config\.h\".*$//g" deheader.log

SRC_HPP = $(shell find $(top_builddir)/src/{libprojet,libsouvenir,macro} -name '*.hpp')
if ENABLE_GTK
SRC_HPP += $(shell find $(top_builddir)/src -name '*.hpp')
endif
OBJ_CPP = $(SRC_HPP:.hpp=.test.cpp)
LOG_TEST = $(SRC_HPP:.hpp=.test.cpp.log)

comp_hpp_cpp : $(OBJ_CPP)
	cat $(LOG_TEST) > log

#Nécessaire pour make checkcode
%.test.cpp : %.hpp
	echo -e "#include \"config.h\"\n#include \"$^\" \nint main () { };" > `basename $@`
	g++ `basename $@` -std=c++11 $(XML_CPPFLAGS) -I$(top_srcdir) -I$(top_srcdir)/src/macro -I$(top_srcdir)/src/libsouvenir -I$(top_srcdir)/src/libprojet -I$(top_srcdir)/src/test/tools -c -o $@.o &> $@.log

#Nécessaire pour deheader
.cpp.o: .cpp
	g++ $^ -std=c++11 $(XML_CPPFLAGS) -I$(top_srcdir) -I$(top_srcdir)/src/macro -I$(top_srcdir)/src/libsouvenir -I$(top_srcdir)/src/libprojet -I$(top_srcdir)/src/test/tools -c -o $@.o

CLEANFILES	=	*.cpp.log log *.cpp *.hpp *.cpp.o *.stamp

EXTRA_DIST	=	COPYING.head \
							gtk.dia \
							libsouvenir.dia \
							libprojet.dia \
							libeurocode.dia
