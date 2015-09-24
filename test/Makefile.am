## Process this file with automake to produce Makefile.in

export PATH := /sbin:/usr/sbin:$(PATH)

AM_CPPFLAGS = \
	-I$(top_srcdir)/src \
	-I$(top_builddir) \
	$(nil)

LDADD = $(top_builddir)/src/libpmount.la

ext2fs.img:
	dd if=/dev/zero of=$@ bs=1024k count=1
	mke2fs -F $@

test_ext2fs_DEPENDENCIES = ext2fs.img
test_ext2fs_SOURCES = test-loop.c
test_ext2fs_CPPFLAGS = $(AM_CPPFLAGS) -DFSNAME=\"ext2fs\"

iso9660.img:
	dd if=/dev/zero of=$@.tmpl bs=1024k count=1
	genisoimage -o $@ -r $@.tmpl

test_iso9660_DEPENDENCIES = iso9660.img
test_iso9660_SOURCES = test-loop.c
test_iso9660_CPPFLAGS = $(AM_CPPFLAGS) -DFSNAME=\"iso9660\"

test_mtab_SOURCES = test-mtab.c
test_mtab_CPPFLAGS = $(AM_CPPFLAGS) -DTEST_MTAB=\"$(srcdir)/mtab\"
test_mtab_LDFLAGS = -static

check_PROGRAMS = \
	test-ext2fs \
	test-iso9660 \
	test-mtab \
	$(nil)

TESTS = $(check_PROGRAMS)

clean-local:
	$(RM) *.bin *.img *.tmpl
	rmdir -p *.mnt || true
