#include "config.h"

#include <string.h>
#include <stdio.h>

#include "main.h"

int
main()
{
  char *line, *word;

  __mtab_setpath(TEST_MTAB);

  line = __mtab_getline("/test");
  if (line == NULL)
    return 1;

  word = __mtab_getword(line, 2);
  if (strcmp(word, "testfs") != 0)
    return 1;

  return 0;
}
