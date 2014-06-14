all : dia2code
	mv *.hpp ../../src/lib

checkcode : dia2code
# On est obligé de mettre un MAKE car il doit y avoir une optimisation et en
# mettant comp_hpp_cpp dans la dépendance, les wildcard doivent être évalués
# au début et non pas à l'appel de la dépendance de comp_hpp_cpp.
	$(MAKE) comp_hpp_cpp

dia2code :
	/usr/bin/dia2code -l COPYING.head -t cpp -ext hpp -d . codegui.dia

SRC_HPP = $(wildcard *.hpp)
OBJ_CPP = $(SRC_HPP:.hpp=.cpp)

comp_hpp_cpp : $(OBJ_CPP)
	cat *.cpp.log > log

%.cpp : %.hpp
	echo -e "#include \"$^\" \nint main () { };" > $@
	g++ $@ -std=c++11 -c -o $@.o &> $@.log

CLEANFILES = *.cpp.log log *.cpp *.hpp *.cpp.o codegui.dia~
