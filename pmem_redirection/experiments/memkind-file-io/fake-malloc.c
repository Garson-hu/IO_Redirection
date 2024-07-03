#include <stdio.h>
#include <stdlib.h>

#define FLUSH printf("Flushing at %d\n", __LINE__), fflush(stdout)
#define REPORTP(P) printf("%s = %p\n", #P, (P)), FLUSH
#define REPORTI(P) printf("%s = %d\n", #P, (P)), FLUSH
#define REPORTC(P) printf("%s = %c\n", #P, (P)), FLUSH

int main()
{
  FLUSH;

  puts("Calling malloc sizeof(int) ...");
  int *iptr = malloc(sizeof(int));
  REPORTP(iptr);
  *iptr = 41;
  REPORTI(*iptr);
  free(iptr);

  puts("Calling malloc sizeof(char) ...");
  char *cptr = malloc(sizeof(char));
  REPORTP(cptr);
  *cptr = 'A';
  REPORTC(*cptr);
  free(cptr);

  puts("Calling malloc 0 ...");
  char *zero = malloc(0);
  REPORTP(zero);
  free(zero);
}
