#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  if(argc < 2){
    fprintf(2, "Usage: xargs cmd [args] ... \n");
    exit(1);
  }

  // read from stdin
  // for each input, exec cmd

  char arg[64];
  char *p;
  int i;
  p=arg;
  char* new_argv[MAXARG];

  while(read(0, p, sizeof(char)) > 0){
    //printf("input %c\n", *p);
    if(*p =='\n'){
      *p = '\0';
      //printf("input %s\n", arg);

      if(fork()==0){
        // argv[2...n] + arg
        //argv[argc]=arg;

        for(i=1; i<argc; i++)
          new_argv[i-1] = argv[i];
        new_argv[argc-1] = arg;

        // for(i=0; i<argc+1; i++)
        //   printf("%d %s\n", i, new_argv[i]);

        exec(argv[1], new_argv);
        exit(0);

      }else{
        wait(0);
        memset(arg, 0, strlen(arg));
        //printf("arg cleaned %s\n", arg);
        p=arg;
      }
    }else{
      ++p;
    }

  }



  exit(0);
}
