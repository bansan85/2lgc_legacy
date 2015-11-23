if HAVE_DOXYGEN

HPP_SOUVENIR_FILES = $(shell find ../src/libsouvenir -name '*.hpp')
HPP_PROJET_FILES = $(shell find ../src/macro ../src/libprojet -name '*.hpp')

doxyfilesouvenir.stamp: $(HPP_SOUVENIR_FILES)
	$(DOXYGEN) DoxyfileSouvenir
	echo Timestamp > doxyfilesouvenir.stamp

doxyfileprojet.stamp: $(HPP_PROJET_FILES)
	$(DOXYGEN) DoxyfileProjet
	echo Timestamp > doxyfileprojet.stamp

CLEANFILES = doxyfilesouvenir.stamp doxyfileprojet.stamp

all-local: doxyfilesouvenir.stamp doxyfileprojet.stamp

clean-local:
	rm -Rf html latex man

endif 
