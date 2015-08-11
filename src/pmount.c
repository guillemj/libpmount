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

#include <paths.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "main.h"

int
pmount(char *fstype, char *mntdir, int mntflags, void *data)
{
#ifdef USE_MTAB
  char *options, *line;
  int rc;
#endif
  int ret;

  ret = __pmount(fstype, mntdir, mntflags, data);
  if (ret == -1)
    return -1;

#ifdef USE_MTAB
   /* We were able to mount, but maybe we are not root. On systems where
      non-root users can mount (e.g. GNU) we only keep track of filesystems
      mounted by root. */
  if (geteuid() != 0)
  {
    errno = 0;
    return 0;
  }

  /* Generate an options substring */
  if ((mntflags & PMOUNT_READONLY) == 0)
    options = strdup("rw");
  else
    options = strdup("ro");

  if ((mntflags & PMOUNT_NOSUID) == 0)
    rc = asprintf(&options, "%s,suid", options);
  else
    rc = asprintf(&options, "%s,nosuid", options);
  if (rc < 0)
    return -1;

  if (ret == __PMOUNT_LOOPBACK)
    rc = asprintf(&options, "%s,loop", options);
  if (rc < 0)
    return -1;

  /* For non-device filesystems we list "null" as device.
     FIXME: This check only addresses virtual filesystems like procfs_*,
     but network filesystems will initialise 'data' when implemented. */
  if (data == NULL)
    data = (void *)"null";

  rc = asprintf(&line, "%s %s %s %s %d %d\n",
                (char *)data, mntdir, fstype, options, 0, 0);
  if (rc < 0)
    return -1;
  free(options);

  ret = __mtab_add(line);
  free(line);
  if (ret == -1)
    return -1;
#endif

  errno = 0;
  return 0;
}

int
pumount(char *mntdir, int mntflags)
{
#ifdef USE_MTAB
  char *line, *file, *opts;
  line = __mtab_getline(mntdir);
  opts = __mtab_getword(line, 3);

  verbose("%s: Going to umount \"%s\"\n", __func__, line);
  if (strstr(opts, ",loop") == NULL)
    file = NULL;
  else
    file = __mtab_getword(line, 0);

#ifdef DEBUG
  if (file == NULL)
    verbose("%s: There was no loop device.\n", __func__);
  else
    verbose("%s: Seems like file %s was looped.\n", __func__, file);
#endif

#else
#define file NULL
#endif
  if (__pumount(mntdir, mntflags, file) != 0)
    return -1;

#ifdef USE_MTAB
  if (geteuid() == 0)
    if (__mtab_del(mntdir) == -1)
      return -1;
#endif

  errno = 0;
  return 0;
}
