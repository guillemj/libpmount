#
# libpmount tests
#

export PATH := /sbin:/usr/sbin:$(PATH)
MK_CFLAGS = -I../src -L.. -lpmount

tests: ext2fs.img isofs.img

ext2fs isofs: test.c
	$(CC) $< -o $@ $(CFLAGS) $(MK_CFLAGS) -DFSNAME="\"$@\""

ext2fs.img: ext2fs
	dd if=/dev/zero of=$<.img bs=1024k count=1
	mke2fs -F $<.img
	mkdir -p $<.mnt
	./$<

isofs.img: isofs
	dd if=/dev/zero of=$<.dummy bs=1024k count=1
	mkisofs -o $<.img -r $<.dummy
	mkdir -p $<.mnt
	./$<

clean:
	$(RM) ext2fs isofs
	$(RM) *.bin *.img *.dummy
	rmdir -p *.mnt || true
