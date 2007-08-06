#include <pmount.h>
#include <stdio.h>

#define FSIMAGE FSNAME ".img"
#define FSMOUNT FSNAME ".mnt"

int
main()
{
  int error;

  error = pmount(FSNAME, FSMOUNT, PMOUNT_READONLY, FSIMAGE);
  if (error == -1)
  {
    perror("pmount failed");
    return 1;
  }

  error = pumount(FSMOUNT, 0);
  if (error == -1)
  {
    perror("WARNING: pmount succeeded, but pumount failed");
    return 1;
  }

  return 0;
}

