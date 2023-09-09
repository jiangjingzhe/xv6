#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

int read_line(char *new_argv[],int new_argc)
{
    char buf;
    char arg[1024];
    int argi=0;
    while(read(0,&buf,1))
    {
        if(argi>=1023)
        {
            fprintf(2, "The arg is too long\n");
            exit(1);
        }

        if(buf=='\n')
        {
            arg[argi]=0;
            new_argv[new_argc]=malloc(argi+1);
            strcpy(new_argv[new_argc],arg);
            new_argc++;
            break;
        }

        if(buf==' ')
        {
            arg[argi]=0;
            new_argv[new_argc]=malloc(argi+1);
            strcpy(new_argv[new_argc],arg);
            new_argc++;
            argi=0;
            continue;
        }
        arg[argi++]=buf;
    }
    return new_argc;
}
int
main(int argc, char *argv[])
{
    if(argc<2)
    {
        fprintf(2, "Usage: xargs command arg\n");
        exit(1);
    }
    char *new_argv[MAXARG];
    for(int i=1;i<argc;i++)
    {
        new_argv[i-1]=argv[i];
    }
    int new_argc=read_line(new_argv,argc-1);  
    while((argc-1)!=new_argc)
    {
        new_argv[new_argc]=0;
        if(fork()==0)
        {
            exec(new_argv[0],new_argv);
            fprintf(2, "xargs exec error\n");
            exit(1);
        }else{
            wait(0);
        }
        new_argc=read_line(new_argv,argc-1);
    }
    
    exit(0);
}
