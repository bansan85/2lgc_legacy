if HAVE_DOXYGEN

$(builddir)/doxyfile.stamp: $(wildcard ../src/lib/*.hpp ../src/macro/*.hpp ../src/libundo/*.hpp)
	$(DOXYGEN) Doxyfile
	echo Timestamp > $(builddir)/doxyfile.stamp

CLEANFILES = doxyfile.stamp

all-local: $(builddir)/doxyfile.stamp

clean-local:
	rm -Rf html latex man

endif 
