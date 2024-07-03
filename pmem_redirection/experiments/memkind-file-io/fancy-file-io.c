#include <stdio.h>
#include <string.h>

#define REPORTP(P) printf("%s = %p\n", #P, (P))

int main()
{
  FILE *fp = fopen("hi", "r");
  REPORTP(fp);
  if (!fp) {
    perror("hi");
  } else {
    fclose(fp);
  }

  fp = fopen("hi", "a");
  REPORTP(fp);

  char *world = "Hello, World!\n";
  fwrite(world, 1, strlen(world), fp);

  fclose(fp);
  
  fp = fopen("hi", "r");
  char chptr[1];
  while (fread(chptr, 1, 1, fp))
    putchar(*chptr);

  fclose(fp);
}
