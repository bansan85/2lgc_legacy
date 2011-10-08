ACLOCAL_AMFLAGS = -I m4

SUBDIRS =	po \
			src \
			files \
			doc

AM_CFLAGS = @CFLAGS@ -march=native -O0 -g3 -ggdb3 -pipe -ftracer -fsched2-use-traces -Wl,-O0 -fno-inline -Wall -Wextra -DGDK_DISABLE_DEPRECATED -DGTK_DISABLE_DEPRECATED -Wno-comment -Wswitch-enum -Wswitch-default -Wshadow -Werror -DLOCALEDIR=\""@localedir@"\" -DDATADIR=\""@datadir@/2lgc"\" `pkg-config --cflags glib-2.0`
if ENABLE_GTK
AM_CFLAGS      +=  @GTK_CFLAGS@
endif
AM_CXXFLAGS		= $(AM_CFLAGS)

export AM_CFLAGS AM_CXXFLAGS

EXTRA_DIST = config.rpath
