if HAVE_DOXYGEN

doxyfile.stamp:
	$(DOXYGEN) Doxyfile
	echo Timestamp > doxyfile.stamp

CLEANFILES = doxyfile.stamp

all-local: doxyfile.stamp
clean-local:
	rm -Rf html latex Doxyfile

endif 

#CLEANFILES = *.log *.aux manuel.pdf *.out *.tmp *.toc *.bbl *.blg html latex
