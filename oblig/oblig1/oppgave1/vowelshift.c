#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
  if (argc != 3) return EXIT_FAILURE;

  for (int i = 0; argv[1][i] != '\0'; i++) {
    int letter = argv[1][i]; // ASCII-verdier til argumentstrengen, castes til char senere 
    printf("%s", (char*) (strchr("aeiouAEIOU", letter) ? argv[2] : (char*) &letter)); 
  }

  printf("\n");
  

  return EXIT_SUCCESS;
}

