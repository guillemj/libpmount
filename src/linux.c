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
 */

#include "config.h"

#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <linux/loop.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>

#include "main.h"

/* If 'file' is NULL, find a free device. Otherwise find a device that is
   already set for 'file'. In either case, on failure return NULL. */
static char *
__findloop (char *file)
{
  char *loop;
  int i, fd, ret;
  struct loop_info loopinfo;
  struct stat st;

  for (i = 0; i <= 15; i++)
    {
      int rc;

      if (i <= 7)
        rc = asprintf (&loop, "/dev/loop%d", i);
      else
        rc = asprintf (&loop, "/dev/loop/%d", i - 8);

      if (rc < 0)
        break;

      /* Make sure this is a loop device */
      if (stat (loop, &st) != 0)
        break;
      if (S_ISBLK (st.st_mode) == 0)
        break;
      if (major (st.st_rdev) != 7)
        break;

      fd = open (loop, O_RDONLY);
      if (fd < 0)
        break;

      ret = ioctl (fd, LOOP_GET_STATUS, &loopinfo);
      close (fd);

      if (ret == 0)
        {
#ifdef verbose
          fprintf (stderr, "__findloop: Device %s is in use\n", loop);
#endif
          if (file != NULL)
            {
              if (!strcmp (file, loopinfo.lo_name))
                {
#ifdef verbose
                  fprintf (stderr, "__findloop: We were looking for file %s"
                           ", which seems to match device %s\n", file, loop);
#endif
                  return loop;
                }
            }
        }
      else if ((file == NULL) && (errno == ENXIO))
        {
#ifdef verbose
          fprintf (stderr, "__findloop: We were looking for a free device, "
                   "and %s is.\n", loop);
#endif
          return loop;
        }
    }
  free (loop);

  /* We failed. Let's return NULL. */
  return NULL;
}

/* Finds a free loop device, and sets it for 'file'. If 'file' is already
   set as loop device, __setloop will re-use that device.
   Returns the loop device name is successful, NULL otherwise. */
static char *
__getloop (char *file)
{
  char *device;
  int fd_file, fd_device, ret;
  struct loop_info loopinfo;

  device = __findloop (file);
  if (device != NULL)
    return device;

  device = __findloop (NULL);
  if (device == NULL)
    return NULL;

#ifdef verbose
  fprintf (stderr, "__getloop: Setting up %s in %s\n", file, device);
#endif

  fd_file = open (file, O_RDONLY | O_WRONLY);
  fd_device = open (device, O_RDONLY);
  ret = ioctl (fd_device, LOOP_SET_FD, fd_file);
  close (fd_file);
  if (ret == -1)
   {
     close (fd_device);
     return NULL;
   }
#ifdef verbose
  fprintf (stderr, "__getloop: LOOP_SET_FD succeeded (%d).\n", ret);
#endif

  memset (&loopinfo, 0, sizeof (loopinfo));
  if (strlen (file) <= LO_NAME_SIZE)
    strncpy (loopinfo.lo_name, file, LO_NAME_SIZE);
  else
    return NULL; /* Fuck it. Filename is too long! */
  ret = ioctl (fd_device, LOOP_SET_STATUS, &loopinfo);
  close (fd_device);
  if (ret == -1)
    return NULL;
#ifdef verbose
  fprintf (stderr, "__getloop: LOOP_SET_STATUS succeeded (%d).\n", ret);
#endif

  return device;
}

/* Unsets the device found to be using 'file'. In case of failure, or if none
   found, return -1. */
static int
__clrloop (char *file)
{
  char *device;
  int fd, ret;

  device = __findloop (file);
  if (device == NULL)
    return -1;

  fd = open (device, O_RDONLY);
  free (device);
  ret = ioctl (fd, LOOP_CLR_FD, 0);
  close (fd);

  return ret;
}

int
__pmount (char *fstype, char *mntdir, int mntflags, void *data)
{
  char *my_fstype = fstype;
  void *my_data = NULL;
  int my_mntflags = 0;
  char *device = NULL;

  if ((mntflags & PMOUNT_REMOUNT) != 0)
    my_mntflags |= MS_REMOUNT;
  if ((mntflags & PMOUNT_READONLY) != 0)
    my_mntflags |= MS_RDONLY;
  if ((mntflags & PMOUNT_NOSUID) != 0)
    my_mntflags |= MS_NOSUID;
  if ((mntflags & PMOUNT_NOEXEC) != 0)
    my_mntflags |= MS_NOEXEC;
  if ((mntflags & PMOUNT_NODEV) != 0)
    my_mntflags |= MS_NODEV;
  if ((mntflags & PMOUNT_SYNCHRONOUS) != 0)
    my_mntflags |= MS_SYNCHRONOUS;

  if (!strcmp (fstype, "iso9660"))
    {
      device = (char *) data;
    }
  else if (!strcmp (fstype, "ext2fs"))
    {
      my_fstype = "ext2";
      device = (char *) data;
    }
  else if (!strcmp (fstype, "procfs_linux"))
    {
      my_fstype = "proc";
      device = (char *) data;
    }
  else if (!strcmp (fstype, "nfs"))
    {
      my_data = NULL; /* FIXME */
    }
  else
    return PMOUNT_UNKNOWNFS;

#ifdef verbose
          fprintf (stderr, "__pmount: Let's mount %s on %s.\n", device,
		  mntdir);
#endif

  if (mount (device, mntdir, my_fstype, my_mntflags, my_data) == -1)
    {
#ifdef USE_LOOP
      if (errno == ENOTBLK)
        {
          device = __getloop (device);

          if (mount (device, mntdir, my_fstype, my_mntflags,
                     my_data) == -1)
            return -1;
          else
            return __PMOUNT_LOOPBACK;
        }
      else
#endif
        return -1;
    }

  errno = 0;
  return 0;
}

int
__pumount (char *mntdir, int mntflags, char *looped_file)
{
  int my_mntflags = 0;
  if ((mntflags & PUMOUNT_FORCE) != 0)
    my_mntflags |= MNT_FORCE;

  if (umount2 (mntdir, my_mntflags) == -1)
    return -1;

#ifdef USE_LOOP
  if (looped_file != NULL)
    {
#ifdef verbose
      fprintf (stderr, "__pumount: pumount told me the loop is %s\n",
               looped_file);
#endif
      if (__clrloop (looped_file) == -1);
        return -1;
    }
#ifdef verbose
  else
    {
      fprintf (stderr, "__pumount: pumount told me there's no such loop.\n");
    }
#endif

#endif

  errno = 0;
  return 0;
}
