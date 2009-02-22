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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef PMOUNT_H
#define PMOUNT_H

/* custom error codes */
#define PMOUNT_UNKNOWNFS	1
#define PMOUNT_NOTME		2

/* mapping of mount flags  */
#define PMOUNT_REMOUNT		1
#define PMOUNT_READONLY		2
#define PMOUNT_NOSUID		4
#define PMOUNT_NOEXEC		8
#define PMOUNT_NODEV		16
#define PMOUNT_SYNCHRONOUS	32

/* mapping of umount flags */
#define PUMOUNT_FORCE		1

int pmount(char *fstype, char *mntdir, int mntflags, void *data);
int pumount(char *mntdir, int mntflags);

#endif

