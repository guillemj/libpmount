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

#include "pmount.h"

#define __PMOUNT_LOOPBACK 1

int __pmount(char *fstype, char *mntdir, int mntflags, void *data);
int __pumount(char *mntdir, int mntflags, char *looped_file);

int __mtab_add(char *line);
int __mtab_del(char *mntdir);
char *__mtab_getline(char *mntdir);
char *__mtab_getword(char *line, int i);
