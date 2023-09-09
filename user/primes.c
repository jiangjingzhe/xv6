#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void new_fork(int *p)
{
    close(p[1]);
    int x;
    read(p[0],&x,4);
    printf("prime %d\n",x);
    int n;
    if(read(p[0],&n,4))
    {
        int new_p[2];
        pipe(new_p);
        if(fork()==0)
        {
            new_fork(new_p);
        }else
        {
            close(new_p[0]);
            if(n%x)
            {
                write(new_p[1],&n,4);
            }
            while(read(p[0],&n,4))
            {
                if(n%x)
                write(new_p[1],&n,4);
            }
            close(p[0]);
            close(new_p[1]);
            wait(0);
        }
    }
    exit(0);
}

int
main(int argc, char *argv[])
{
  int p[2];
  pipe(p);
  if(fork()==0)
  {
    new_fork(p);
  }    
  else
  {
    close(p[0]);
    for(int i=2;i<=35;i++)
    {
        write(p[1],&i,4);
    }
    close(p[1]);
    wait(0);
    exit(0);
  }
  return 0;
}