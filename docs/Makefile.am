if HAVE_DOXYGEN

HPP_FILES = $(shell find ../src/ -name '*.hpp')

$(builddir)/doxyfile.stamp: $(HPP_FILES)
	$(DOXYGEN) Doxyfile
	echo Timestamp > $(builddir)/doxyfile.stamp

CLEANFILES = doxyfile.stamp

all-local: $(builddir)/doxyfile.stamp

clean-local:
	rm -Rf html latex man

endif 
