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
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "main.h"

#ifndef _PATH_MOUNTED
# warning _PATH_MONTED is undefined. Using "/etc/mtab".
# define _PATH_MOUNTED "/etc/mtab"
#endif

/* Add a line to mtab. Returns 0 on success. */
int
__mtab_add(char *line)
{
  FILE *mtab;

  mtab = fopen(_PATH_MOUNTED, "a");
  if (mtab == NULL)
  {
    perror("fopen");
    return PMOUNT_NOTME;
  }

  fprintf (mtab, "%s", line);

  if (fclose (mtab) != 0)
    {
      perror ("fclose");
      return PMOUNT_NOTME;
    }

  return 0;
}

/* Parses mtab and deletes the line that refers to the mount dir 'mntdir'.
   We assume there can't be two lines with same 'mntdir' string. Returns
   0 is successful. */
int
__mtab_del (char *mntdir)
{
  FILE *mtab_r, *mtab_w;
  char line[8192]; /* Yeah; I know this is lame, but it works.
                      Patches welcome. */
  char *ret;
  int rc;

  if (rename (_PATH_MOUNTED, _PATH_MOUNTED "~") == -1)
    if (errno != ENOENT)
      {
        perror ("rename");
        return PMOUNT_NOTME;
      }
  mtab_r = fopen (_PATH_MOUNTED "~", "r");
  if (mtab_r == NULL)
    {
      perror ("fopen");
      return PMOUNT_NOTME;
    }
  mtab_w = fopen (_PATH_MOUNTED, "w");
  if (mtab_w == NULL)
    {
      perror ("fopen");
      return PMOUNT_NOTME;
    }

  rc = asprintf (&mntdir, " %s ", mntdir);
  if (rc < 0)
    {
      perror ("asprintf");
      return PMOUNT_NOTME;
    }
  while (1)
    {
      ret = fgets (line, sizeof(line), mtab_r);
      if (ret == NULL)
        {
          if (errno == 0)
            break;
          perror ("fgets");
          return PMOUNT_NOTME;
        }
      if (strstr (line, mntdir) == NULL)
        fprintf (mtab_w, "%s", ret);
    }
  free (mntdir);

  if (fclose (mtab_r) != 0)
    {
      perror ("fclose");
      return PMOUNT_NOTME;
    }
  if (fclose (mtab_w) != 0)
    {
      perror ("fclose");
      return PMOUNT_NOTME;
    }
  if (unlink (_PATH_MOUNTED "~") != 0)
    {
      perror ("unlink");
      return PMOUNT_NOTME;
    }

  return 0;
}

/* Searches mtab for a line using 'mntdir'. If successful, returns the whole
   line. Otherwise, returns NULL. */
char *
__mtab_getline (char *mntdir)
{
  FILE *mtab;
  char line[8192]; /* Yeah; I know this is lame, but it works.
                      Patches welcome. */
  char *ret;

  mtab = fopen (_PATH_MOUNTED, "r");
  if (mtab == NULL)
    {
      perror ("fopen");
      return NULL;
    }

  if (asprintf (&mntdir, " %s ", mntdir) < 0)
    {
      perror ("asprintf");
      return NULL;
    }
  while (1)
    {
      ret = fgets (line, sizeof(line), mtab);
      if (ret == NULL)
        {
          if (errno == 0)
            break;
          perror ("fgets");
          return NULL;
        }
      if (strstr (line, mntdir) != NULL)
      {
        ret = strdup(line);
        break;
      }
    }
  free (mntdir);

  if (fclose (mtab) != 0)
    {
      perror ("fclose");
      return NULL;
    }

  return ret;
}

/* Returns word number 'i' in 'line'. In case of failure, returns NULL. */
char *
__mtab_getword (char *line, int i)
{
  char *tmp;
  size_t linelen;

  for (; i > 0 ; i--)
    /* Find next space. */
    line = (strchr (line, ' ') + 1);

  /* Trim everything after last space. */
  tmp = strchr (line, ' ');
  if (tmp == NULL)
    return NULL;

  linelen = tmp - line;
  line[linelen] = '\0';

  /* Sanity check */
  if (strlen (line) != linelen)
    return NULL;

  return line;
}
