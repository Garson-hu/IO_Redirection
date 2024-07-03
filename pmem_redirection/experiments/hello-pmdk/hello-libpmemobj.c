#include <stdio.h>
#include <libpmemobj.h>
#include <string.h>

int main()
{
  const char *msg = pmemobj_errormsg();
  printf("Hello, pmemobj_errormsg = [%s] (%ld) at %p\n", msg, strlen(msg), msg);
}
