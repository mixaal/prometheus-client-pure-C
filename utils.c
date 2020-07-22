#include "all.h"

void *xmalloc(size_t nbytes)
{
  void *r = malloc(nbytes);
  if(r==NULL) {
    perror("malloc()");
    exit(EXIT_FAILURE);
  }

  return r;
}

