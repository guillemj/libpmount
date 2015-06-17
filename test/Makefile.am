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
test_ext2fs_SOURCES = test.c
test_ext2fs_CPPFLAGS = $(AM_CPPFLAGS) -DFSNAME=\"ext2fs\"

isofs.img:
	dd if=/dev/zero of=$@.tmpl bs=1024k count=1
	mkisofs -o $@ -r $@.tmpl

test_isofs_DEPENDENCIES = isofs.img
test_isofs_SOURCES = test.c
test_isofs_CPPFLAGS = $(AM_CPPFLAGS) -DFSNAME=\"isofs\"

check_PROGRAMS = \
	test-ext2fs \
	test-isofs \
	$(nil)

TESTS = $(check_PROGRAMS)

clean-local:
	$(RM) *.bin *.img *.tmpl
	rmdir -p *.mnt || true
