#!/bin/sh -e

export PATH="/sbin:/usr/sbin:$PATH"
export CFLAGS="-Isrc -Lout -lpmount"

dd if=/dev/zero of=tmp/ext2fs.img bs=1024k count=1
mke2fs -F tmp/ext2fs.img
mkdir -p tmp/ext2fs.mnt
cat > tmp/ext2fs.c << EOF
#include <pmount.h>
#include <stdio.h>
main ()
  {
    if (pmount ("ext2fs", "tmp/ext2fs.mnt", PMOUNT_READONLY,
                "tmp/ext2fs.img") == -1)
      {
        perror ("pmount failed");
        return -1;
      }
    if (pumount ("tmp/ext2fs.mnt", 0) == -1)
      {
        perror ("WARNING: pmount succeeded, but pumount failed");
        return -1;
      }
    return 0;
  }
EOF
gcc tmp/ext2fs.c -o tmp/ext2fs.bin $CFLAGS
./tmp/ext2fs.bin
