## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = \
	-I$(top_builddir) \
	-I$(builddir) \
	$(nil)

EXTRA_DIST = \
	kfreebsd.c \
	linux.c \
	libpmount.map \
	$(nil)

lib_LTLIBRARIES = libpmount.la

libpmount_la_DEPENDENCIES = \
	kfreebsd.c \
	linux.c \
	libpmount.map
libpmount_la_LDFLAGS = \
	-Wl,-z,defs \
	-Wl,--version-script=$(srcdir)/libpmount.map \
	-version-number $(LIBPMOUNT_ABI)
libpmount_la_SOURCES = \
	kerndep.c \
	mtab.c \
	main.h \
	pmount.c \
	pmount.h \
	$(nil)

runtimelibdir = $(libdir)

install-exec-hook:
	if [ "$(libdir)" != "$(runtimelibdir)" ]; then \
		$(MKDIR_P) $(DESTDIR)$(runtimelibdir); \
		mv $(DESTDIR)$(libdir)/libpmount*.so.* \
		   $(DESTDIR)$(runtimelibdir)/; \
		soname=`readlink $(DESTDIR)$(libdir)/libpmount.so`; \
		sorelprefix=`echo $(libdir) | sed -r -e 's:(^/)?[^/]+:..:g'`; \
		ln -sf $$sorelprefix$(runtimelibdir)/$$soname \
		       $(DESTDIR)$(libdir)/libpmount.so; \
	fi

uninstall-hook:
	rm -f $(DESTDIR)$(runtimelibdir)/libpmount*.so*
