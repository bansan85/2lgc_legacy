ACLOCAL_AMFLAGS = -I m4

SUBDIRS = 	po \
		src \
		files \
		doc

AM_CFLAGS = @CFLAGS@ -Werror -DLOCALEDIR=\""@localedir@"\" -DDATADIR=\""@datadir@/2lgc"\"
export AM_CFLAGS

EXTRA_DIST = config.rpath
