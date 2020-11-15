#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int parent_fd[2], child_fd[2];
char buff[64];

int
main(int argc, char *argv[])
{
  if(argc > 2){
    fprintf(2, "Usage: pingpong\n");
    exit(1);
  }

  pipe(parent_fd);
  pipe(child_fd);
  if(fork()==0){
    // child
    read(parent_fd[0], buff, 4);
    fprintf(1, "%d: received %s\n", getpid(), buff);
    write(child_fd[1], "pong", 4);
    exit(0);
  }
  else{
    // parent
    write(parent_fd[1], "ping", 4);
    read(child_fd[0], buff, 4);
    fprintf(1, "%d: received %s\n", getpid(), buff);
    exit(0);
  }

  exit(0);
}