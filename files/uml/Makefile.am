all : ../../src/lib/codegui.hpp \
		  ../../src/libundo/undo.hpp

checkcode : ../../src/lib/codegui.hpp
# On est obligé de mettre un MAKE car il doit y avoir une optimisation et en
# mettant comp_hpp_cpp dans la dépendance, les wildcard doivent être évalués
# au début et non pas à l'appel de la dépendance de comp_hpp_cpp.
	$(MAKE) comp_hpp_cpp

../../src/lib/codegui.hpp : COPYING.head codegui.dia INb.dia INoeud.dia ISection.dia IMateriau.dia IDeplacementType.dia ICharge.dia
	rm -f ../../src/lib/*.hpp
	../../../dia2code-git/src/dia2code -nl -nc -l COPYING.head -t cpp11 -ext hpp -d ../../src/lib codegui.dia
	../../../dia2code-git/src/dia2code -nl -nc -l COPYING.head -t cpp11 -ext hpp -d ../../src/lib INb.dia
	../../../dia2code-git/src/dia2code -nl -nc -l COPYING.head -t cpp11 -ext hpp -d ../../src/lib INoeud.dia
	../../../dia2code-git/src/dia2code -nl -nc -l COPYING.head -t cpp11 -ext hpp -d ../../src/lib ISection.dia
	../../../dia2code-git/src/dia2code -nl -nc -l COPYING.head -t cpp11 -ext hpp -d ../../src/lib IMateriau.dia
	../../../dia2code-git/src/dia2code -nl -nc -l COPYING.head -t cpp11 -ext hpp -d ../../src/lib IDeplacementType.dia
	../../../dia2code-git/src/dia2code -nl -nc -l COPYING.head -t cpp11 -ext hpp -d ../../src/lib ICharge.dia
	find ../../src/lib -maxdepth 1 -name "*.hpp" -exec echo "#include <{}>" >> ../../src/lib/codegui.hpp \;
	sed -i "s#../../src/lib/##g" ../../src/lib/codegui.hpp
	sed -i "s/#include <codegui.hpp>//g" ../../src/lib/codegui.hpp
	sort ../../src/lib/codegui.hpp -o ../../src/lib/codegui.hpp

../../src/libundo/undo.hpp : COPYING.head undo.dia
	rm -f ../../src/libundo/*.hpp
	../../../dia2code-git/src/dia2code -nl -nc -l COPYING.head -t cpp11 -ext hpp -d ../../src/libundo undo.dia
	find ../../src/libundo -maxdepth 1 -name "*.hpp" -exec echo "#include <{}>" >> ../../src/libundo/undo.hpp \;
	sed -i "s#../../src/libundo/##g" ../../src/libundo/undo.hpp
	sed -i "s/#include <undo.hpp>//g" ../../src/libundo/undo.hpp
	sort ../../src/libundo/undo.hpp -o ../../src/libundo/undo.hpp

SRC_HPP = $(wildcard *.hpp)
OBJ_CPP = $(SRC_HPP:.hpp=.cpp)

comp_hpp_cpp : $(OBJ_CPP)
	cat *.cpp.log > log

%.cpp : %.hpp
	echo -e "#include \"$^\" \nint main () { };" > $@
	g++ $@ -std=c++11 -c -o $@.o &> $@.log

CLEANFILES	=	*.cpp.log log *.cpp *.hpp *.cpp.o codegui.dia~

EXTRA_DIST	=	COPYING.head \
							codegui.dia
