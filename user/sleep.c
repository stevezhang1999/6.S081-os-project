#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  if(argc < 2){
    fprintf(2, "Usage: sleep [time]\n");
    exit(1);
  }

  fprintf(1, "I am sleeping...\n");
  sleep(atoi(argv[1]));

  exit(0);
}
