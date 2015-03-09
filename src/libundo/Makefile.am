DEFAULT_INCLUDES		=		-I$(srcdir) \
															-I$(srcdir)/../.. \
															-I$(srcdir)/../macro

AUTOMAKE_OPTIONS		= 	subdir-objects

lib_LTLIBRARIES			= 	libundo.la

libundo_la_LIBADD		=		@XML_LIBS@
if PLATFORM_LINUX
if ENABLE_DEBUG
libundo_la_LIBADD   +=	-lgcov
endif
endif
if PLATFORM_WIN32
libundo_la_LIBADD   +=  -lintl
endif
if PLATFORM_WIN64
libundo_la_LIBADD   +=  -lintl
endif

if ENABLE_GTK
libundo_la_CXXFLAGS	=		@GTKMM_CFLAGS@
libundo_la_LIBADD		+=	@GTKMM_LIBS@
endif

libundo_la_LDFLAGS	= 	$(AM_LDFLAGS) -release $(VERSION) -Wl,-O0 -no-undefined

libundo_la_CPPFLAGS	=		@XML_CPPFLAGS@

libundo_ladir 			= 	$(includedir)/2lgc

libundo_la_SOURCES 	=		IUndoable.cpp \
															CUndoData.cpp \
															CUndoManager.cpp

libundo_la_HEADERS 	=		$(wildcard *.hpp *.hh)

$(libundo_la_SOURCES) : 	uml

uml : ../../files/uml/undo.dia
	$(MAKE) -C ../../files/uml

CLEANFILES = *.gcno *.gcda *.hpp

cppcheck :
	cppcheck --inconclusive --enable=all $(DEFAULT_INCLUDES) $(libundo_la_SOURCES) &> ../../cppcheck-undo.log
if ENABLE_GTK
	cppcheck --inconclusive --enable=all $(DEFAULT_INCLUDES) $(libundo_gtk_la_SOURCES) &>> ../../cppcheck-undo-gtk.log
endif
	@sed 's/^.*scanf without field.*$$//g' -i ../../cppcheck-undo.log
