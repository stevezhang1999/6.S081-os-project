#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// sieve: 2, 3, 5, 7, 11, 13, 17, 19
int sieves_fd[16];
int buff[64];
int i;
int sieves[] = {2, 3, 5, 7, 11, 13, 17, 19};
int sieve_num = 8;

void set_up_pipe_proc(int num);

int
main(int argc, char *argv[])
{
  if(argc > 2){
    fprintf(2, "Usage: primes\n");
    exit(1);
  }

  // sieve_2: get from the previous from sieve_2; send to the next via sieve_3
  // for(i=0; i<sieve_num; i++)
  //   pipe(sieves_fd + 2*i);

  set_up_pipe_proc(-1);
  
  wait(0);

  exit(0);
}

// algorithm
// 1. get from the left
// 2. process it
// 3. pass to the right or print

// set up this process: child as the next sieve, parent is this sieve.
// if is the first sieve, parent is the input
// if is the last sieve, only process it, doesn't set up.

// pipe 0: for sieve 2 to read from.
// pipe last:

// set up the pipe (if neccessary) and read, write, process data.
void set_up_pipe_proc(int num){


  // if need to create more process
  if (num < sieve_num -1){
    pipe(sieves_fd + num + 1);

    // init: write numbers to first sieve process
    if (num == -1){
      close(sieves_fd[0]);
      for(i=2; i<=35; i++){
        write(sieves_fd[1], &i, 4);
      }
      close(sieves_fd[1]);
    }


    if(fork()==0){
      // child: call the function again
      set_up_pipe_proc(num+1);
    }
    else{
      if(num != -1){
        // read and process
        while(read(sieves_fd[num*2], buff+num, 4) > 0){
          if(buff[num] % sieves[num] != 0){
           if(buff[num] / sieves[num] == 1){
             fprintf(1, "%d\n", buff+num);
           }else{
             write(sieves_fd[2*(num+1) + 1], buff+num, 4);
           }
          }
        }
        close(sieves_fd[num*2]);
        close(sieves_fd[num*2 + 1]);
        close(sieves_fd[2*(num+1) + 1]);
        exit(0);
      }
    }
  // the last process
  }else{
    close(sieves_fd[num*2+1]);
    while(read(sieves_fd[num*2], buff+num, 4) > 0){
      if(buff[num] % sieves[num] != 0){
        if(buff[num] / sieves[num] == 1){
          fprintf(1, "%d\n", buff+num);
        }
      }
    }
    close(sieves_fd[num*2]);
    exit(0);
  }

}