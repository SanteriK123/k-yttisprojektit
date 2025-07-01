#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
  int reset = 0;
  if(argc > 1 && strcmp(argv[1], "clear") == 0)
    reset = 1;
  int count = getreadcount(reset);
  printf("Read count is: %d\n", count);
  exit(0);
}