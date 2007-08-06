/* Taken from Ted's losetup.c - Mitch <m.dsouza@mrc-apu.cam.ac.uk> */
/* Added vfs mount options - aeb - 960223 */
/* Removed lomount - aeb - 960224 */

/*
 * 1999-02-22 Arkadiusz Mi¶kiewicz <misiek@pld.ORG.PL>
 * - added Native Language Support
 * 1999-03-21 Arnaldo Carvalho de Melo <acme@conectiva.com.br>
 * - fixed strerr(errno) in gettext calls
 * 2000-09-24 Marc Mutz <Marc@Mutz.com>
 * - added -p option to pass passphrases via fd's to losetup/mount.
 *   Used for encryption in non-interactive environments.
 *   The idea behind xgetpass() is stolen from GnuPG, v.1.0.3.
 */

#define LOOPMAJOR	7

int
is_loop_device (const char *device)
{
  struct stat statbuf;
  return (stat (device, &statbuf) == 0 &&
	  S_ISBLK (statbuf.st_mode) && major (statbuf.st_rdev) == LOOPMAJOR);
}

/* device names */
/dev/loop%d /dev/loop/%d

if (ioctl (fd, LOOP_GET_STATUS, &loopinfo) == 0)
  /* in use */
else if (errno == ENXIO)
/* probably free */

/* set loop */
ioctl (fd, LOOP_SET_FD, ffd)

/* del loop */
ioctl (fd, LOOP_CLR_FD, 0)
