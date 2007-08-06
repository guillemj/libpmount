#
# libpmount Makefile
#

CFLAGS = -Wall -g
MK_CFLAGS = -D_REENTRANT

PACKAGE = libpmount
SOVERSION = 0.0
SONAME = $(PACKAGE).so.$(SOVERSION)
LIBNAME = $(PACKAGE).a

SRCS = $(addprefix src/,kerndep.c mtab.c pmount.c)
SH_OBJS = $(SRCS:.c=.lo)
ST_OBJS = $(SRCS:.c=.o)

%.lo: %.c
	$(CC) $(CFLAGS) $(MK_CFLAGS) -fPIC -c -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) $(MK_CFLAGS) -c -o $@ $<

build: $(SONAME) $(LIBNAME)

$(LIBNAME): $(ST_OBJS)
	$(AR) cru $@ $<

$(SONAME): $(SH_OBJS)
	$(CC) -Wl,-z,defs -Wl,-soname,$(SONAME) -shared -o $@ $<

check: build
	# check if we are root, and it's not fake
	if test `id -u` = 0 && test -z "$(FAKEROOTKEY)"; then \
	  for i in tests/*.sh; do sh $i; done \
	else \
	  echo "Not running as root, skipping checks." \
	fi

install: build
	mkdir -p $(DESTDIR)/usr/include
	mkdir -p $(DESTDIR)/usr/lib
	mkdir -p $(DESTDIR)/lib
	install -m644 src/pmount.h $(DESTDIR)/usr/include/
	install -s -m755 libpmount.so.0.0 $(DESTDIR)/lib/
	strip --strip-unneeded $(DESTDIR)/lib/libpmount.so.0.0
	install -m755 libpmount.a $(DESTDIR)/usr/lib/
	ln -s /lib/libpmount.so.0 $(DESTDIR)/usr/lib/libpmount.so
	ln -s libpmount.so.0.0 $(DESTDIR)/lib/libpmount.so.0

clean:
	$(RM) $(ST_OBJS)
	$(RM) $(SH_OBJS)
	$(RM) $(SONAME)
	$(RM) $(LIBNAME)

