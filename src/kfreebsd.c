/*
 * Copyright © 2004  Robert Millan <rmh@debian.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *
 * Copyright © 1992, 1993, 1994
 *      The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "config.h"

#include <sys/cdio.h>
#include <sys/file.h>
#include <sys/syscall.h>
#include <arpa/inet.h>
#include <err.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h>

#include "pmount.h"
#include <sys/mount.h>
#include <isofs/cd9660/cd9660_mount.h>

static int
__iso_get_ssector(const char *dev)
{
  struct ioc_toc_header h;
  struct ioc_read_toc_entry t;
  struct cd_toc_entry toc_buffer[100];
  int fd, ntocentries, i;

  fd = open(dev, O_RDONLY);
  if (fd == -1)
    return -1;
  if (ioctl(fd, CDIOREADTOCHEADER, &h) == -1)
  {
    close(fd);
    return -1;
  }

  ntocentries = h.ending_track - h.starting_track + 1;
  if (ntocentries > 100)
  {
    /* unreasonable, only 100 allowed */
    close(fd);
    return -1;
  }
  t.address_format = CD_LBA_FORMAT;
  t.starting_track = 0;
  t.data_len = ntocentries * sizeof(struct cd_toc_entry);
  t.data = toc_buffer;

  if (ioctl(fd, CDIOREADTOCENTRYS, (char *)&t) == -1)
  {
    close(fd);
    return -1;
  }
  close(fd);

  for (i = ntocentries - 1; i >= 0; i--)
    if ((toc_buffer[i].control & 4) != 0)
      /* found a data track */
      break;
  if (i < 0)
    return -1;

  return ntohl(toc_buffer[i].addr.lba);
}

int
__pmount(char *fstype, char *mntdir, int mntflags, void *data)
{
  char *my_fstype = fstype;
  void *my_data = NULL;
  int my_mntflags = 0;
  struct iso_args args;

  if ((mntflags & PMOUNT_REMOUNT) != 0)
    my_mntflags |= MNT_UPDATE;
  if ((mntflags & PMOUNT_READONLY) != 0)
    my_mntflags |= MNT_RDONLY;
  if ((mntflags & PMOUNT_NOSUID) != 0)
    my_mntflags |= MNT_NOSUID;
  if ((mntflags & PMOUNT_NOEXEC) != 0)
    my_mntflags |= MNT_NOEXEC;
  if ((mntflags & PMOUNT_SYNCHRONOUS) != 0)
    my_mntflags |= MNT_SYNCHRONOUS;

  if (strcmp(fstype, "ext2fs") == 0)
  {
    my_data = data;
  }
  else if (strcmp(fstype, "procfs_linux") == 0)
  {
    my_fstype = "linprocfs";
    my_data = data;
  }
  else if (strcmp(fstype, "iso9660") == 0)
  {
    my_fstype = "cd9660";

    memset(&args, 0, sizeof(args));
    args.ssector = -1;
    args.cs_disk = NULL;
    args.cs_local = NULL;

    /*
     * ISO 9660 file systems are not writeable.
     */
    mntflags |= MNT_RDONLY;
    args.export.ex_flags = MNT_EXRDONLY;
    args.fspec = (char *)data;
    args.export.ex_root = -2;
    args.flags = 0;

    if (args.ssector == -1)
    {
      /*
       * The start of the session has not been specified on
       * the command line.  If we can successfully read the
       * TOC of a CD-ROM, use the last data track we find.
       * Otherwise, just use 0, in order to mount the very
       * first session.  This is compatible with the
       * historic behaviour of mount_cd9660(8).  If the user
       * has specified -s <ssector> above, we don't get here
       * and leave the user's will.
       */
      args.ssector = __iso_get_ssector((char *)data);
      if (args.ssector == -1)
      {
        /* could not determine starting sector,
           using very first session */
        args.ssector = 0;
      }
    }
    my_data = (void *)&args;
  }
  else if (strcmp(fstype, "nfs") == 0)
  {
    my_data = NULL;		/* FIXME */
  }
  else
    return PMOUNT_UNKNOWNFS;

  return syscall(SYS_mount, my_fstype, mntdir, mntflags, my_data);
}

int
__pumount(char *mntdir, int mntflags)
{
  int my_mntflags = 0;

  if ((mntflags & PUMOUNT_FORCE) != 0)
    my_mntflags |= MNT_FORCE;

  return syscall(SYS_unmount, mntdir, my_mntflags);
}
