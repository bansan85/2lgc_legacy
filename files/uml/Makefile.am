all : ../../src/lib/codegui.hpp \
		  ../../src/libundo/undo.hpp

checkcode : ../../src/lib/codegui.hpp
# On est obligé de mettre un MAKE car il doit y avoir une optimisation et en
# mettant comp_hpp_cpp dans la dépendance, les wildcard doivent être évalués
# au début et non pas à l'appel de la dépendance de comp_hpp_cpp.
	$(MAKE) comp_hpp_cpp

../../src/lib/codegui.hpp : COPYING.head codegui.dia
	rm -f *.hpp
	../../../dia2code-git/src/dia2code -l COPYING.head -t cpp11 -ext hpp -d . codegui.dia
	find * -maxdepth 0 -name "*.hpp" -exec echo "#include \"{}\"" >> codegui.hpp \;
	mv *.hpp ../../src/lib

../../src/libundo/undo.hpp : COPYING.head undo.dia
	rm -f *.hpp
	../../../dia2code-git/src/dia2code -l COPYING.head -t cpp11 -ext hpp -d . undo.dia
	find * -maxdepth 0 -name "*.hpp" -exec echo "#include \"{}\"" >> undo.hpp \;
	mv *.hpp ../../src/libundo

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
