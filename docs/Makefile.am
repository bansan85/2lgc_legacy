if HAVE_DOXYGEN

doxyfile.stamp: $(wildcard ../src/lib/*.hpp ../src/macro/*.hpp ../src/libundo/*.hpp)
	$(DOXYGEN) Doxyfile
	echo Timestamp > doxyfile.stamp

CLEANFILES = doxyfile.stamp

all-local: doxyfile.stamp

clean-local:
	rm -Rf html latex man

endif 
