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

// find(path, name)
// if path is a file, compare path and name
//  if good, print; if not good, discard
// if path is a dir, than for each item  e in path, find(path + '/' + e, name)

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
      printf("%s\n", path);
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
      //printf("%s\n",de.name);
      if(strcmp(".", file_or_dir)!=0 && strcmp("..", file_or_dir)!=0){
        
        // empty the string
        memset(new_path,0,strlen(new_path));
        strcpy(new_path, path_copy);
        // concatenate path and new entry: path/ent
        new_path[strlen(path_copy)] = '/';
        strcpy(new_path+strlen(new_path), de.name);

        find(new_path, name);
      }

          
    }
    break;
  }
  close(fd);


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
