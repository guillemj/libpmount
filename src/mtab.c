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

static const char *__mtab_path = _PATH_MOUNTED;

/* Set the mtab pathname to use. */
void
__mtab_setpath(const char *path)
{
  if (path == NULL)
    __mtab_path = _PATH_MOUNTED;
  else
    __mtab_path = path;
}

/* Add a line to mtab. Returns 0 on success. */
int
__mtab_add(char *line)
{
  FILE *mtab;

  mtab = fopen(__mtab_path, "a");
  if (mtab == NULL)
  {
    perror("fopen");
    return PMOUNT_NOTME;
  }

  fprintf(mtab, "%s", line);

  if (fclose(mtab) != 0)
  {
    perror("fclose");
    return PMOUNT_NOTME;
  }

  return 0;
}

/* Parses mtab and deletes the line that refers to the mount dir 'mntdir'.
   We assume there can't be two lines with same 'mntdir' string. Returns
   0 is successful. */
int
__mtab_del(char *mntdir)
{
  FILE *mtab_r = NULL;
  FILE *mtab_w = NULL;
  char *mtab_path_new = NULL;
  char *mntdir_str = NULL;
  char *line = NULL;
  size_t line_len = 0;
  int ret = PMOUNT_NOTME;
  int rc;

  if (asprintf(&mtab_path_new, "%s~", __mtab_path) < 0)
  {
    perror("asprintf");
    goto cleanup_release;
  }

  mtab_r = fopen(__mtab_path, "r");
  if (mtab_r == NULL)
  {
    perror("fopen");
    goto cleanup_release;
  }
  mtab_w = fopen(mtab_path_new, "w");
  if (mtab_w == NULL)
  {
    perror("fopen");
    goto cleanup_release;
  }

  rc = asprintf(&mntdir_str, " %s ", mntdir);
  if (rc < 0)
  {
    perror("asprintf");
    goto cleanup_unlink;
  }

  while (1)
  {
    ssize_t nread;

    nread = getline(&line, &line_len, mtab_r);
    if (nread < 0)
    {
      if (errno == 0)
        break;
      perror("getline");
      goto cleanup_unlink;
    }
    if (strstr(line, mntdir_str) == NULL)
      fprintf(mtab_w, "%s", line);
  }

  if (fsync(fileno(mtab_w)) < 0)
  {
    perror("fsync");
    goto cleanup_unlink;
  }

  if (fclose(mtab_r) != 0)
  {
    perror("fclose");
    mtab_r = NULL;
    goto cleanup_unlink;
  }
  mtab_r = NULL;
  if (fclose(mtab_w) != 0)
  {
    perror("fclose");
    mtab_w = NULL;
    goto cleanup_unlink;
  }
  mtab_w = NULL;
  if (rename(mtab_path_new, __mtab_path) != 0)
  {
    perror("rename");
    goto cleanup_unlink;
  }

  ret = 0;
  goto cleanup_release;

cleanup_unlink:
  if (unlink(mtab_path_new) != 0)
    perror("unlink");

cleanup_release:
  if (mtab_r != NULL && fclose(mtab_r) != 0)
    perror("fclose");
  if (mtab_w != NULL && fclose(mtab_w) != 0)
    perror("fclose");

  free(line);
  free(mntdir_str);
  free(mtab_path_new);

  return ret;
}

/* Searches mtab for a line using 'mntdir'. If successful, returns the whole
   line. Otherwise, returns NULL. */
char *
__mtab_getline(char *mntdir)
{
  FILE *mtab = NULL;
  char *mntdir_str;
  char *line;
  size_t line_len;
  char *ret = NULL;

  mtab = fopen(__mtab_path, "r");
  if (mtab == NULL)
  {
    perror("fopen");
    return NULL;
  }

  if (asprintf(&mntdir_str, " %s ", mntdir) < 0)
  {
    perror("asprintf");
    mntdir_str = NULL;
    goto cleanup;
  }
  while (1)
  {
    ssize_t nread;

    nread = getline(&line, &line_len, mtab);
    if (nread < 0)
    {
      if (errno == 0)
        break;

      perror("fgets");
      goto cleanup;
    }
    if (strstr(line, mntdir) != NULL)
    {
      ret = strdup(line);
      break;
    }
  }

cleanup:
  free(mntdir_str);

  if (mtab != NULL && fclose(mtab) != 0)
    perror("fclose");

  return ret;
}

/* Returns word number 'i' in 'line'. In case of failure, returns NULL. */
char *
__mtab_getword(char *line, int i)
{
  char *tmp;
  size_t linelen;

  for (; i > 0 ; i--)
    /* Find next space. */
    line = (strchr(line, ' ') + 1);

  /* Trim everything after last space. */
  tmp = strchr(line, ' ');
  if (tmp == NULL)
    return NULL;

  linelen = tmp - line;
  line[linelen] = '\0';

  /* Sanity check */
  if (strlen(line) != linelen)
    return NULL;

  return line;
}
