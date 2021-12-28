#include "ucode.c"
int console, term, term1;
int in,out;
int parent()
{
    int pid, status;
    while(1)
    {
        printf("INIT: wait for ZOMBIE child\n");
        //printf("uhoh stinky\n");
        close(0);
        close(1);
        
        //printf("console: %d pid: %d", console, pid);
        pid = wait(&status);
        if(pid == console)
    	{
            
            in = open("/dev/tty0", O_RDONLY);
            out = open("/dev/tty0", O_WRONLY);
            printf("INIT: forks a new console login\n");
            //printf("Helpmakmo\n");
            console = fork();
            if (console)
                continue;
            else
                exec("login /dev/tty0");
        }
        if(pid == term)
    	{
            
            in = open("/dev/ttyS0", O_RDONLY);
            out = open("/dev/ttyS0", O_WRONLY);
            printf("INIT: forks a new console login\n");
            //printf("gooooogksnl\n");
            console = fork();
            if (console)
                continue;
            else
                exec("login /dev/ttyS0");
        }
        if(pid == term1)
    	{
            
            in = open("/dev/ttyS1", O_RDONLY);
            out = open("/dev/ttyS1", O_WRONLY);
            printf("INIT: forks a new console login\n");
            //printf("jfskjkjsknnjnkjnkjnkjo\n");
            console = fork();
            if (console)
                continue;
            else
                exec("login /dev/ttyS1");
        }
        printf("I just buried an Orphan lolsies\n");
    }
}

int main(void)
{
    int in,out;
    in = open("/dev/tty0", O_RDONLY);
    out = open("/dev/tty0", O_WRONLY);
    printf("INIT: fork a login proc on console");
    console = fork();
    if(console){ //and term, term1,term2 //&& term && term1
        term=fork();
        if(term){
            term1=fork();
            if (term1){
                parent();
            }
            else{
                exec("login /dev/ttyS1");
            }
        }
        else{
            exec("login /dev/ttyS0");
        }
    }
    else{
        exec("login /dev/tty0");
    }
}