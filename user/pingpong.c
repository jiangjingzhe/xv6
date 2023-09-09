#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int p[2];
  pipe(p);
  if(fork()==0)
  {
    char write_buf[]= "hello";
    char read_buf[10];
    read(p[0],read_buf,6);
    printf("%d: received ping\n",getpid());
    write(p[1],write_buf,strlen(write_buf));
    close(p[0]);
    close(p[1]);
    exit(0);
  }else
  {
    char write_buf[]= "hello";
    char read_buf[10];
    write(p[1],write_buf,strlen(write_buf));
    wait(0);
    read(p[0],read_buf,6);
    printf("%d: received pong\n",getpid());
    close(p[0]);
    close(p[1]);
    exit(0);
  }
  exit(0);
}