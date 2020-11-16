#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "user/user.h"

char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), 0, DIRSIZ-strlen(p));
  return buf;
}

void find(char *path, char* name){
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;
  char *file_or_dir; // fmtname(path)
  char path_copy[64];
  char new_path[64];
  strcpy(path_copy, path);

  if((fd = open(path, 0)) < 0){
    fprintf(2, "fd: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "fd: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type){
  case T_FILE:
    file_or_dir = fmtname(path);
    if(strcmp(name, file_or_dir)==0){
      printf("%s\n", file_or_dir);
    }
    break;

  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("ls: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if(stat(buf, &st) < 0){
        printf("ls: cannot stat %s\n", buf);
        continue;
      }
      file_or_dir = fmtname(buf);
      if(st.type==T_FILE){
        if(strcmp(name, file_or_dir)==0){
          printf("%s%s\n", path_copy, file_or_dir);
        }
      }else{
        if(strcmp(".", file_or_dir)!=0 && strcmp("..", file_or_dir)!=0){
          // file_or_dir = path_copy + file_or_dir
          find(file_or_dir, name);
        }
      }   
    }
    break;
  }
  close(fd);

 // for every sub-dir in path, call find again

}

int
main(int argc, char *argv[])
{
  if(argc < 3){
    fprintf(2, "Usage: find [dir] [name]\n");
    exit(1);
  }

  char *path = argv[1];
  char *name = argv[2];

  find(path, name);

  exit(0);
}
