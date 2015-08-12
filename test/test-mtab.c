#include "config.h"

#include <string.h>
#include <stdio.h>

#include "main.h"

int
main()
{
  char *line, *word;

  line = __mtab_getline("/proc");
  if (line == NULL)
    return 77;

  word = __mtab_getword(line, 2);
  if (strcmp(word, "proc") != 0)
    return 1;

  return 0;
}
