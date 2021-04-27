#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv[])
{
  int i = 0;

  if (argc < 2)
  {
      fprintf(stderr,"Error: missing the required argument");
      exit(0);
  }
  srand(time(NULL));
  int length = atoi(argv[1]);

  for (i = 0; i < length; i++)
  {
    // will be a random character between A-Z and space
    char randchar = "ABCDEFGHIJKLMNOPQRSTUVWXYZ "[random() % 27];
    fprintf(stdout, "%c", randchar);
  }
  printf('\n', stdout);

  return 0;
}
